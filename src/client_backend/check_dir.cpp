#ifndef CHECK_DIR_CPP
#define CHECK_DIR_CPP
#include "check_dir.h"
#include "well_known.h"
#include "log.h"
#include "md5hash.h"
#include "scan.h"
#include "app_ctrl.h"
#include "virus_ctrl.h"
#include "settings.h"
#include <mutex> // Include the mutex header
#include <algorithm> // Include the algorithm header
#include <string> // Include the string header
#include <iostream> // Include the iostream header

// Define a mutex for thread synchronization
std::mutex monitorMutex;

bool is_directory(const std::string& path) {
    DWORD attributes = GetFileAttributes(path.c_str());

    if (attributes == INVALID_FILE_ATTRIBUTES) {
        // Handle the error, e.g., by printing an error message
        return false;
    }

    return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

void process_changes(const FILE_NOTIFY_INFORMATION* pInfo) {
    // Allocate a buffer for the file name and copy the content
    std::wstring fileName(pInfo->FileName, pInfo->FileNameLength / sizeof(wchar_t));

    // Convert wstring to string
    std::string filename_str(fileName.begin(), fileName.end());
    filename_str = "c:\\" + filename_str;

    // Scan the file and send it to virus_ctrl if it is a virus and then process it
    std::transform(filename_str.begin(), filename_str.end(), filename_str.begin(), ::tolower);
    if (!is_folder_included(filename_str.c_str()) || is_directory(filename_str) || is_folder_excluded(filename_str.c_str())) {
        // Don't scan excluded files or folders
        return;
    }
    else {
        //log(LOGLEVEL::INFO_NOSEND, "[process_changes()]: File ", filename_str, " has been changed. Scanning it for viruses");
        std::thread scan_thread(scan_file_t, filename_str);
        scan_thread.detach();
    }
}

void monitor_directory(LPCSTR directory) {
    // Open the directory for monitoring
    HANDLE hDir = CreateFile(
        directory,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (hDir == INVALID_HANDLE_VALUE) {
        log(LOGLEVEL::ERR, "[monitor_directory()]: Error opening directory: ", directory, " while monitoring directory for changes");
        return;
    }

    // Create a buffer for file change notifications
    constexpr DWORD bufferSize = 4096;
    BYTE buffer[bufferSize];

    // Monitor the directory for changes
    OVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
    memset(buffer, 0, bufferSize);
    overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (ReadDirectoryChangesW(
        hDir,
        buffer,
        bufferSize,
        TRUE,
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
        NULL,
        &overlapped,
        NULL) == 0) {
        log(LOGLEVEL::ERR, "[monitor_directory()]: Error reading directory changes: ", GetLastError(), " while monitoring directory for changes");
        CloseHandle(hDir);
        return;
    }

    log(LOGLEVEL::INFO, "[monitor_directory()]: Monitoring directory: ", directory, " for changes");

    // Wait for changes
    while (!app_stop()) {
        DWORD bytesReturned;
        DWORD waitStatus = WaitForSingleObject(overlapped.hEvent, INFINITE);

        if (waitStatus == WAIT_OBJECT_0) {
            if (GetOverlappedResult(hDir, &overlapped, &bytesReturned, FALSE)) {
                // Process the changes in the buffer
                FILE_NOTIFY_INFORMATION* pInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);

                do {
                    process_changes(pInfo);

                    pInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
                        reinterpret_cast<BYTE*>(pInfo) + pInfo->NextEntryOffset);

                } while (pInfo->NextEntryOffset != 0);

                // Reset the event for the next wait
                ResetEvent(overlapped.hEvent);
                memset(buffer, 0, bufferSize);

                // Continue monitoring
                if (ReadDirectoryChangesW(
                    hDir,
                    buffer,
                    bufferSize,
                    TRUE,
                    FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
                    NULL,
                    &overlapped,
                    NULL) == 0) {
                    log(LOGLEVEL::ERR, "[monitor_directory()]: Error reading directory changes: ", GetLastError(), " while monitoring directory for changes");
                    break;
                }
            }
            else {
                log(LOGLEVEL::ERR, "[monitor_directory()]: GetOverlappedResult failed: ", GetLastError());
                break;
            }
        }
        else {
            log(LOGLEVEL::ERR, "[monitor_directory()]: WaitForSingleObject failed: ", GetLastError());
            break;
        }
    }

    // Cleanup
    CloseHandle(overlapped.hEvent);
    CloseHandle(hDir);
}

void folder_scanner() {
    // Lock access to the monitor function
    std::lock_guard<std::mutex> lock(monitorMutex);

    // We are in a completely separate thread than the main thread; unlimited resources wuhuii
    // Start the watch dir function used to monitor the dir for new files
    monitor_directory("C:\\");
}

#endif // !CHECK_DIR_CPP

/*
This file contains the functions that monitor the processes running on the system and scan them. It houses the rtp process scanne rfuncitons

Functions:
	- monitor_processes(): This function monitors the processes running on the system.
	- process_scanner(): This function scans the processes running on the system. It starts the monitor_processes function.

*/

#include "check_process.h"
#include "log.h"
#include "well_known.h"
#include "scan.h"
#include "app_ctrl.h"
#include "settings.h"
#include "scan.h"
#include <mutex> // Include the mutex header
#include <thread> // Include the thread header
#include <Windows.h> // Include the Windows header
#include "utils.h"

std::mutex mtx; // Declare a mutex for thread synchronization

void monitor_processes() {
    static DWORD previousProcessIds[1024] = { 0 }; // Previous snapshot of process IDs
    DWORD processIds[1024];
    DWORD bytesReturned;

    // Get the list of process IDs
    if (EnumProcesses(processIds, sizeof(processIds), &bytesReturned)) {
        // Calculate how many process IDs were returned
        DWORD numProcesses = bytesReturned / sizeof(DWORD);

        // Check for new processes
        for (DWORD i = 0; i < numProcesses; ++i) {
            DWORD processId = processIds[i];
            BOOL isNewProcess = TRUE;

            // Check if the process is new
            {
                std::lock_guard<std::mutex> guard(mtx); // Lock the mutex
                for (DWORD j = 0; j < 1024; ++j) {
                    if (processId == previousProcessIds[j]) {
                        isNewProcess = FALSE;
                        break;
                    }
                }
            } // Unlock the mutex automatically when 'guard' goes out of scope

            // If the process is new, get its executable path and print it

            if (isNewProcess) {
                // Open the process
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
                if (hProcess != NULL) {
                    TCHAR exePath[MAX_PATH];
                    char path[MAX_PATH];
                    if (GetModuleFileNameEx(hProcess, NULL, exePath, MAX_PATH) > 0) {
                        strcpy_s(path, MAX_PATH, exePath);
                        for (size_t z = 0; z < strlen(path); z++)
                            path[z] = tolower(path[z]);
                        //log(LOGLEVEL::INFO_NOSEND, "[monitor_processes()]: New process: ", path);
                        if (is_valid_path(path)) { //filter out invalid paths and paths with weird characters
                            if (!is_folder_included(path) || is_folder_excluded(path)) {
                                // Don't scan excluded files or folders
                            }
                            else if (is_disallowed_sart_folder(path) && get_setting("application_control:status")) {
                                kill_process(path);
                                log(LOGLEVEL::INFO, "[monitor_processes()]: Starting apps from this location is not allowed: ", path);
                            }
                            else {
                                int thread_timeout = 0;

                                //log(LOGLEVEL::INFO_NOSEND, "[monitor_processes()]: Scanning process: ", path);
                                std::thread scan_thread(scan_process_t, path);
                                scan_thread.detach();
                            }
                        }
                    }
                    CloseHandle(hProcess);
                }
            }
        }
        // Update the previous snapshot of process IDs
        {
            std::lock_guard<std::mutex> guard(mtx); // Lock the mutex
            memcpy(previousProcessIds, processIds, sizeof(DWORD) * 1024);
        } // Unlock the mutex automatically when 'guard' goes out of scope
    }
    else {
        log(LOGLEVEL::ERR, "[monitor_processes()]: Error enumerating processes");
    }
}

void process_scanner() {
    // We are in a thread so we can do this, unlimited resources
    while (!app_stop()) {
        monitor_processes();
        Sleep(200); // Sleep for 1 second
    }
}

#include "utils.h"
#include <windows.h>
#include <string.h>
#include <iostream>
#include "log.h"
#include <tlhelp32.h>

void split(const std::string& input, char delimiter, std::string& out1, std::string& out2) {
    // Split a string at the delimiter. The delimiter only occurs once. 
    // The first part is stored in out1 and the second part in out2.
    size_t pos = input.find(delimiter);
    if (pos != std::string::npos) {
        out1 = input.substr(0, pos);
        out2 = input.substr(pos + 1);
    }
}


void startup(LPCTSTR lpApplicationName)
{
    // additional information
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // start the program up
    CreateProcess(lpApplicationName,   // the path
        NULL,           // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi             // Pointer to PROCESS_INFORMATION structure
    );
    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}


std::string get_filename(const std::string& path) {
    auto pos = path.find_last_of("\\");
    if (pos == std::string::npos) {
        // No directory separator found, return the original path
        return path;
    }
    else {
        // Return the substring after the last directory separator
        return path.substr(pos + 1);
    }
}

int strcasecmp(const std::string& s1, const std::string& s2) {
    auto it1 = s1.begin();
    auto it2 = s2.begin();
    while (it1 != s1.end() && it2 != s2.end()) {
        int diff = std::tolower(*it1) - std::tolower(*it2);
        if (diff != 0)
            return diff;
        ++it1;
        ++it2;
    }
    return 0;
}

void kill_process(const std::string& path) {
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (strcasecmp(pEntry.szExeFile, get_filename(path).c_str()) == 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, static_cast<DWORD>(pEntry.th32ProcessID));
            if (hProcess != NULL)
            {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
            else
                log(LOGLEVEL::ERR, "[kill_process()]: Error while killing process: ", path);
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
}
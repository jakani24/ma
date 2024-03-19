#pragma warning(disable:4996)
#ifndef CONNECT_CPP
#define CONNECT_CPP
#include <Windows.h>
#include <string>
#include <fstream>
#include "../client_backend/well_known.h"
int send_to_pipe(const std::string& message) {
    HANDLE hPipe;
    DWORD dwWritten;
    std::ofstream (MAIN_COM_PATH);//enshure the file exists
    hPipe = CreateFile(TEXT(MAIN_COM_PATH),
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hPipe != INVALID_HANDLE_VALUE) {
        if (WriteFile(hPipe,
            message.c_str(),
            static_cast<DWORD>(message.size()), // Size of the message
            &dwWritten,
            NULL)) {
            CloseHandle(hPipe);
            return 0; // Success
        }
        else {
            // Failed to write to pipe
            CloseHandle(hPipe);
            return 2; // Error writing to pipe
        }
    }
    else {
        // Failed to open pipe
        DWORD dwError = GetLastError();
        return dwError; // Return the error code
    }
}
std::string read_from_pipe() {
    HANDLE hPipe;
    DWORD dwRead;
    DWORD dwWritten;
    char buffer[1000];
    hPipe = CreateFile(TEXT(MAIN_COM_PATH),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);
    if (hPipe != INVALID_HANDLE_VALUE)
    {
        ReadFile(hPipe, buffer, sizeof(buffer), &dwRead, NULL);
        CloseHandle(hPipe);
        buffer[strlen(buffer) + 1] = '\0';
        return std::string(buffer);
    }
    else {
        return "Error";
    }
}
#endif

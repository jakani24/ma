#include "check_process.h"
#include "log.h"
#include "well_known.h"
#include "scan.h"
#include "app_ctrl.h"
#include "settings.h"
#include "scan.h"

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
            for (DWORD j = 0; j < 1024; ++j) {
                if (processId == previousProcessIds[j]) {
                    isNewProcess = FALSE;
                    break;
                }
            }

            // If the process is new, get its executable path and print it
            if (isNewProcess) {
                // Open the process
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
                if (hProcess != NULL) {
                    // Get the full path of the executable
                    TCHAR exePath[MAX_PATH];
                    char path[MAX_PATH+1];
                    if (GetModuleFileNameEx(hProcess, NULL, exePath, MAX_PATH) > 0) {
                        // Print the full path of the executable
                        strcpy_s(path, MAX_PATH, exePath);
                        //convert to lower case
                        for(int i=0;i<strlen(path);i++)
                            path[i] = tolower(path[i]);
                        //scan the file
                        if (!is_folder_included(path) or is_folder_excluded(path)) {
							//dont scan excluded files or folders
						}
                        else {
                            //log(LOGLEVEL::INFO, "[monitor_processes()]: New Process to scan: ", path, " while monitoring processes");
							std::thread scan_thread(scan_process_t, path);
							scan_thread.detach();
						}
                    }
                    // Close the process handle
                    CloseHandle(hProcess);
                }
            }
        }

        // Update the previous snapshot of process IDs
        memcpy(previousProcessIds, processIds, sizeof(DWORD) * 1024);
    }
    else {
		log(LOGLEVEL::ERR, "[monitor_processes()]: Error enumerating processes");
	}
}
void process_scanner() {
    //we are in a thread so we can do this, unlimited resources wuhuiii
    while (!app_stop()) {
		monitor_processes();
        Sleep(1000); // Sleep for 1 second
	}
}
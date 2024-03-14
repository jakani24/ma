#include <iostream>
#include <windows.h>
#include "../client_backend/well_known.h"
#include <string.h>
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
int main(){
	//this is the cyberhex updater tool.
	printf("[+] Welcome to the Cyberhex updating tool.\n");
	//as soon a swe get called, cyberhex will shutdown itself.
	printf("[+] Sleeping for 3 seconds, in order to wait for cyberhex to terminate.\n");
	sleep(3); //we wait three seconds, so we can be shure, that cyberhex does not run anymore.
	
	//remove old Cyberhex .exe
	std::string path = APP_DIR;
	path += "\\cyberhex.exe";
	if(remove(path.c_str())!=0){
		printf("[-] ERROR while removing cyberhex executable! path: %s\n",path.c_str());
		return 1;
	}
	//now rename the new .exe
	path = APP_DIR;
	path += "\\cyberhex_latest.exe";
	std::string new_path = APP_DIR;
	new_path += "\\cyberhex.exe";
	if(rename(path.c_str(),new_path.c_str())!=0){
		printf("[-] ERROR while renaming cyberhex executable! from %s to %s\n",path.c_str(),new_path.c_str());
		return 2;	
	}
	printf("[+] Cyberhex update successfully finished!\n");
	printf("[+] Starting cyberhex now!\n");
	//restart cyberhex now.
	startup(new_path.c_str());
	return 0;
}


// ma_installer.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//
#define _WIN32_WINNT 0x0500

#include <iostream>
#include <windows.h>
#include <sddl.h>
#include <stdio.h>
#include "download.h"
#pragma comment(lib, "advapi32.lib")
/*
Tasks to do:
- launch as admin
- create secure folder
- download app files from server
- install app files
- create background service
- create folder for communication

*/
//check if programm is run as admin
bool is_admin() {
    BOOL fIsRunAsAdmin = FALSE;
    PSID pAdminSID = NULL;

    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0,
        &pAdminSID)) {
        if (!CheckTokenMembership(NULL, pAdminSID, &fIsRunAsAdmin)) {
            fIsRunAsAdmin = FALSE;
        }

        FreeSid(pAdminSID);
    }

    return (fIsRunAsAdmin != 0);
}
bool run_as_admin() {
    wchar_t szPath[MAX_PATH];
    if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath))) {
		// Launch itself as admin
		SHELLEXECUTEINFO sei = { sizeof(sei) };
		sei.lpVerb = L"runas";
		sei.lpFile = szPath;
		sei.hwnd = NULL;
		sei.nShow = SW_NORMAL;

        if (!ShellExecuteEx(&sei)) {
			DWORD dwError = GetLastError();
            if (dwError == ERROR_CANCELLED)
            {
				// The user refused to allow privileges elevation.
				return false;
			}
		}
        else {
			// End the calling process. User allowd admin rights
			return true;
		}
	}
	return false;
}
BOOL create_dacl(SECURITY_ATTRIBUTES* pSA)
{
    // Define the SDDL for the DACL. This example sets 
    // the following access:
    //     Built-in guests are denied all access.
    //     Anonymous logon is denied all access.
    //     Authenticated users are allowed 
    //     read/write/execute access.
    //     Administrators are allowed full control.
    // Modify these values as needed to generate the proper
    // DACL for your application.   
    const wchar_t* szSD = TEXT("D:")
        TEXT("(D;OICI;GA;;;BG)")         // Deny access to authenticated users
        TEXT("(D;OICI;GA;;;AN)")         // Deny access to authenticated users
        //TEXT("(D;OICI;GA;;;AU)")       // Deny access to authenticated users
        TEXT("(A;OICI;GA;;;BA)");        // Allow full control to builtinadministrators
        //TEXT("(A;OICI;GA;;;AA)");        // Allow full control to administrators


    if (NULL == pSA)
        return FALSE;

    return ConvertStringSecurityDescriptorToSecurityDescriptor(
        szSD,
        SDDL_REVISION_1,
        &(pSA->lpSecurityDescriptor),
        NULL);
}
int create_secure_folder(LPCWSTR folderpath) {
    int error = 0;
    SECURITY_ATTRIBUTES  sa;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = FALSE;// objects are not inherited

    // Call function to set the DACL. The DACL
    // is set in the SECURITY_ATTRIBUTES 
    // lpSecurityDescriptor member.
    if (!create_dacl(&sa))
    {
        // Error encountered; generate message and exit.
        //printf("Failed to create access control list\n");
        error = 1;
    }

    // Use the updated SECURITY_ATTRIBUTES to specify
    // security attributes for securable objects.
    // This example uses security attributes during
    // creation of a new directory.
    if (error == 0) {
        if (0 == CreateDirectory(folderpath, &sa))
        {
            // Error encountered; generate message and exit.
            //could not create directory
            error = 2;
        }
    }
    // Free the memory allocated for the SECURITY_DESCRIPTOR.
    if (error == 0) {
        if (NULL != LocalFree(sa.lpSecurityDescriptor))
        {
            // Error encountered; generate message and exit.
            //printf("Failed to free the allocated memory\n");
            error = 3;
        }
    }
    return error;
}
int main()
{
    printf("Welcome to the Cyberhex installer!\n");
    int error = 0;
    if (!is_admin()) {
        printf("We are not administrator, requesting UAC\n");
        if (!run_as_admin()) {
            printf("We did not get administrative rights. Please restart the installer!\n");
            MessageBox(NULL, L"Please start the installer with amdin privileges!", L"Error", MB_OK);
            exit(1);
        }
        else {
            //we started the app as admin. This process can be terminated now
            exit(0);
        }
    }else {
        // We're admin, so we can do admin stuff here ...
        printf("Creating directorys\n");
        printf("Creating directory for application\n");
        CreateDirectory(L"C:\\Program Files\\cyberhex", NULL);                                  //create main folder for cyberhex
        printf("Creating directory for communication\n");
        CreateDirectory(L"C:\\Program Files\\cyberhex\\com", NULL); 						    //create folder for communication with desktop client
        printf("Creating directory for desktop client\n");
        CreateDirectory(L"C:\\Program Files\\cyberhex\\app", NULL); 						    //create folder for desktop client application
        printf("Creating directory for secure files\n");
        error = create_secure_folder(L"C:\\Program Files\\cyberhex\\secure");					//create secure folder  
        if (error == 0){
            printf("Creating directory for database\n");
            error = create_secure_folder(L"C:\\Program Files\\cyberhex\\secure\\database");		//create secure folder for hash database
        }if (error == 0){
            printf("Creating directory for settings\n");
            error = create_secure_folder(L"C:\\Program Files\\cyberhex\\secure\\settings");		//create secure folder for settings
        }if (error == 0){
            printf("Creating directory for quarantined files\n");
            error = create_secure_folder(L"C:\\Program Files\\cyberhex\\secure\\quarantine");	//create secure folder for quarantined files = viruses
        }if (error == 0){
            printf("Creating directory for log files\n");
            error = create_secure_folder(L"C:\\Program Files\\cyberhex\\secure\\log");			//create secure folder for log files
        }if (error == 0){
            printf("Creating directory for communication\n");
            error = create_secure_folder(L"C:\\Program Files\\cyberhex\\secure\\com");			//create secure folder for communication with server
        }if (error == 0) {
            printf("Creating directory for application\n");
            error = create_secure_folder(L"C:\\Program Files\\cyberhex\\secure\\app");			//create secure folder for application files
        }

        //download files from server
        if (error == 0) {
            printf("Downloading files from server\n");
            printf("Downloading cyberhex.exe\n");
            error=download_file_from_srv("https://cyberhex.org/download/cyberhex.exe", "C:\\Program Files\\cyberhex\\secure\\app\\cyberhex.exe");
            if (error == 0) {
                printf("Downloading libcrypto-3-x64.dll\n");
                error = download_file_from_srv("https://cyberhex.org/download/libcrypto-3-x64.dll", "C:\\Program Files\\cyberhex\\secure\\app\\libcrypto-3-x64.dll");
            }if (error == 0) {
                printf("Downloading libcurl.dll\n");
                error = download_file_from_srv("https://cyberhex.org/download/libcurl.dll", "C:\\Program Files\\cyberhex\\secure\\app\\libcurl.dll");
            }if (error == 0) {
                printf("Downloading zlib1.dll\n");
                error = download_file_from_srv("https://cyberhex.org/download/zlib1.dll", "C:\\Program Files\\cyberhex\\secure\\app\\zlib1.dll");
            }if (error == 0) {
                printf("Downloading cyberhex_desktop.dll\n");
                error = download_file_from_srv("https://cyberhex.org/download/cyberhex_desktop.exe", "C:\\Program Files\\cyberhex\\app\\cyberhex_desktop.exe");
            }
        }


        //create background service
        if (error == 0) {
            printf("Creating background service\n");
            SC_HANDLE hSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
            if (!hSCManager) {
                //std::cerr << "Failed to open service control manager. Error code: " << GetLastError() << std::endl;
                error = 4;
            }

            LPCWSTR serviceName = L"cyberhex_background_service";
            LPCWSTR servicePath = L"C:\\Path\\To\\Your\\Executable.exe";

            SC_HANDLE hService = CreateService(
                hSCManager,
                serviceName,
                serviceName,
                SERVICE_ALL_ACCESS,
                SERVICE_WIN32_OWN_PROCESS,
                SERVICE_AUTO_START,
                SERVICE_ERROR_NORMAL,
                servicePath,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr
            );

            if (!hService) {
                //std::cerr << "Failed to create service. Error code: " << GetLastError() << std::endl;
                CloseServiceHandle(hSCManager);
                error = 5;
            }

            // Configure the service to run with LocalSystem account (administrator rights)
            if (!ChangeServiceConfig(
                hService,
                SERVICE_NO_CHANGE,
                SERVICE_NO_CHANGE,
                SERVICE_NO_CHANGE,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                L"LocalSystem",
                nullptr,
                nullptr
            )) {
                //std::cerr << "Failed to configure service. Error code: " << GetLastError() << std::endl
                error = 6;
            }
            else {
                //std::cout << "Service created and configured successfully." << std::endl;
                //no error
            }

            CloseServiceHandle(hService);
            CloseServiceHandle(hSCManager);

        }
    }
    switch (error) {
    case 0:
        printf("Installation successful\n");
        printf("You have installed Cyberhex, thank you!\n");
        MessageBox(NULL, L"Installation successful", L"Success", MB_OK);
        break;
    case 1:
        printf("Failed to create access control list\n");
        MessageBox(NULL, L"Failed to create access control list", L"Error", MB_OK);
        break;
    case 2:
        printf("Could not create directory\n");
        MessageBox(NULL, L"Could not create directory", L"Error", MB_OK);
        break;
    case 3:
        printf("Failed to free the allocated memory\n");
        MessageBox(NULL, L"Failed to free the allocated memory", L"Error", MB_OK);
        break;
    case 4:
        printf("Failed to open service control manager\n");
		MessageBox(NULL, L"Failed to open service control manager", L"Error", MB_OK);
		break;
    case 5:
        printf("Failed to create service\n");
        MessageBox(NULL, L"Failed to create service", L"Error", MB_OK);
        break;
    case 6:
        printf("Failed to configure service\n");
        MessageBox(NULL, L"Failed to configure service", L"Error", MB_OK);
        break;
    case 7:
        printf("Failed to download file\n");
        MessageBox(NULL, L"Failed to download file", L"Error", MB_OK);
		break;
    default:
        break;
    }
}

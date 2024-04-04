/*
This is the main installer for the Cyberhex application. It creates the necessary folders, downloads the files from the server, and moves them to the correct folders. It also creates a shortcut on the desktop and starts the service.

DO:
- create folder for application
- create folder for communication
- create folder for secure files
- create folder for database
- create folder for folder database
- create folder for settings
- create folder for quarantined files
- create folder for log files
- create folder for communication
- create folder for application
- create folder for yara
- download files from server
- create task
- download settings
- download databases
- move files into secure directorys
- create shortcut
- display error messages
- reboot the machine
*/
#define _WIN32_WINNT 0x0500

#include <iostream>
#include <windows.h>
#include <sddl.h>
#include <stdio.h>
#include "download.h"
#include "well_known.h"
#include "zip.h"
#include "service.h"
#include "setup_var.h"
#include "update.h"
#include "create_folder.h"
#include <objbase.h>
#include <objidl.h>
#include <propvarutil.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <propkey.h>
#include <comdef.h>
#include <taskschd.h>

#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")

using namespace std;


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

int main()
{
    printf("Welcome to the Cyberhex installer!\n");
    int error = 0;
    if (!is_admin()) {
        printf("We are not administrator, requesting UAC\n");
        if (!run_as_admin()) {
            printf("We did not get administrative rights. Please restart the installer!\n");
            MessageBox(NULL, L"Please start the installer with admin privileges!", L"Error", MB_OK);
            exit(1);
        }
        else {
            //we started the app as admin. This process can be terminated now
            exit(0);
        }
    }
    else {
        // We're admin, so we can do admin stuff here ...
        printf("Creating directorys\n");
        printf("Creating directory for application\n");
        error = create_insecure_folder(L"C:\\Program Files\\cyberhex");                                //create main folder for cyberhex
        if (error == 0) {
            printf("Creating directory for communication\n");
            error = create_insecure_folder(L"C:\\Program Files\\cyberhex\\com"); 		        		//create folder for communication with desktop client
        }if (error == 0) {
            printf("Creating directory for desktop client\n");
            error = create_insecure_folder(L"C:\\Program Files\\cyberhex\\app"); 			        	//create folder for desktop client application
        }if (error == 0) {
            printf("Creating directory for secure files\n");
            error = create_secure_folder(L"C:\\Program Files\\cyberhex\\secure");			        	//create secure folder  
        }if (error == 0){
            printf("Creating directory for database\n");
            error = create_secure_folder(L"C:\\Program Files\\cyberhex\\secure\\database");	        	//create secure folder for hash database
        }if (error == 0) {
            printf("Creating directory for folder database\n");
            error = create_secure_folder(L"C:\\Program Files\\cyberhex\\secure\\database\\folder");		//create secure folder for hash database
        }if (error == 0){
            printf("Creating directory for settings\n");
            error = create_secure_folder(L"C:\\Program Files\\cyberhex\\secure\\settings");		        //create secure folder for settings
        }if (error == 0){
            printf("Creating directory for quarantined files\n");
            error = create_secure_folder(L"C:\\Program Files\\cyberhex\\secure\\quarantine");	        //create secure folder for quarantined files = viruses
        }if (error == 0){
            printf("Creating directory for log files\n");
            error = create_secure_folder(L"C:\\Program Files\\cyberhex\\secure\\log");			        //create secure folder for log files
        }if (error == 0){
            printf("Creating directory for communication\n");
            error = create_secure_folder(L"C:\\Program Files\\cyberhex\\secure\\com");			        //create secure folder for communication with server
        }if (error == 0) {
            printf("Creating directory for application\n");
            error = create_secure_folder(L"C:\\Program Files\\cyberhex\\secure\\app");		        	//create secure folder for application files
        }if (error == 0) {
            printf("Creating directory for application\n");
            error = create_secure_folder(L"C:\\Program Files\\cyberhex\\secure\\yara");		        	//create secure folder for application files
        }

        //download files from server
        FILE* fp;
        char*apikey=get_apikey("secrets.txt");
        char*machineid=get_machineid("secrets.txt");
        char*server_url = get_server("setup.txt");
        printf("%s\n", server_url);
        //copy secrets.txt into the secrets folder
        if (rename("secrets.txt", SECRETS)!=0) {
			error = 8;
		}

        printf("Creating task\n");
        if (error == 0) {
            error = create_task();
            if (error == 1)
            {
                std::cerr << "Task creation failed!" << std::endl;
                error = 5;
            }
        }

        //download all the other files
        //download the settings:
        if (error == 0) {
            printf("Downloading settings from server\n");
            error = action_update_settings();
        }
        if (error == 0) {
            printf("Downloading databases from server\n");
            error = action_update_db();
        }


        if (error == 0) {
            printf("Moving files into secure directorys\n");
            printf("Moving cyberhex.exe\n");
            if(rename("client_backend.exe", "C:\\Program Files\\cyberhex\\secure\\app\\cyberhex.exe")!=0)
                	error = 7;
            if (error == 0) {
                printf("Moving libcrypto-3-x64.dll\n");
                
                if(rename("libcrypto-3-x64.dll", "C:\\Program Files\\cyberhex\\secure\\app\\libcrypto-3-x64.dll")!=0)
                    error = 7;
            }if (error == 0) {
                printf("Moving libcurl.dll\n");
               
                if(rename("libcurl.dll", "C:\\Program Files\\cyberhex\\secure\\app\\libcurl.dll")!=0)
					error = 7;
            }if (error == 0) {
                printf("Moving zlib1.dll\n");
                if(rename("zlib1.dll", "C:\\Program Files\\cyberhex\\secure\\app\\zlib1.dll")!=0)
                    	error = 7;
               
            }if (error == 0) {
                printf("Moving cyberhex_desktop.exe\n");
                if(rename("client_frontend.exe", "C:\\Program Files\\cyberhex\\app\\cyberhex_desktop.exe")!=0)
                    error = 7;
                
            }if (error == 0) {
                printf("Moving msvcp140.dll\n");
               
                if (copy("msvcp140.dll", "C:\\Program Files\\cyberhex\\secure\\app\\msvcp140.dll") != 0)
                    error = 7;
                if (copy("msvcp140.dll", "C:\\Program Files\\cyberhex\\app\\msvcp140.dll") != 0)
                    error = 7;
            }if (error == 0) {
                printf("Moving msvcp140d.dll\n");
               
                if (copy("msvcp140d.dll", "C:\\Program Files\\cyberhex\\secure\\app\\msvcp140d.dll") != 0)
                    error = 7;
                if (copy("msvcp140d.dll", "C:\\Program Files\\cyberhex\\app\\msvcp140d.dll") != 0)
                    error = 7;
            }if (error == 0) {
                printf("Moving vcruntime140.dll\n");
                if (copy("vcruntime140.dll", "C:\\Program Files\\cyberhex\\secure\\app\\vcruntime140.dll") != 0)
                    error = 7;
                if (copy("vcruntime140.dll", "C:\\Program Files\\cyberhex\\app\\vcruntime140.dll") != 0)
                    error = 7;
            }if (error == 0) {
                printf("Moving vcruntime140d.dll\n");
                if (copy("vcruntime140d.dll", "C:\\Program Files\\cyberhex\\secure\\app\\vcruntime140d.dll") != 0)
                    error = 7;
                if (copy("vcruntime140d.dll", "C:\\Program Files\\cyberhex\\app\\vcruntime140d.dll") != 0)
                    error = 7;
            }if (error == 0) {
                printf("Moving vcruntime140_1d.dll\n");
                if (copy("vcruntime140_1d.dll", "C:\\Program Files\\cyberhex\\secure\\app\\vcruntime140_1d.dll") != 0)
                    error = 7;
                if (copy("vcruntime140_1d.dll", "C:\\Program Files\\cyberhex\\app\\vcruntime140_1d.dll") != 0)
                    error = 7;
            }if (error == 0) {
                printf("Moving ucrtbased.dll\n");
                if (copy("ucrtbased.dll", "C:\\Program Files\\cyberhex\\secure\\app\\ucrtbased.dll") != 0)
                    error = 7;
                if (copy("ucrtbased.dll", "C:\\Program Files\\cyberhex\\app\\ucrtbased.dll") != 0)
                    error = 7;
            }if (error == 0) {
                printf("Moving icon\n");
                if (copy("cyberhex_logo2.ico", "C:\\Program Files\\cyberhex\\app\\icon.ico") != 0)
                    error = 7;
            }
        }

        
        if (error == 0) {
			printf("Creating shortcut\n");
            HRESULT hr = create_shortcut(L"C:\\Program Files\\cyberhex\\app\\cyberhex_desktop.exe", L"C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\cyberhex.lnk", L"C:\\Program Files\\cyberhex\\app\\icon.ico");
            if (FAILED(hr))
            {
				std::cerr << "Shortcut creation failed!" << std::endl;
				error = 6;
			}
		}
       
    }
    switch (error) {
    case 0:
        printf("Installation successful\n");
        printf("You have installed Cyberhex, thank you!\n");
        printf("The machine needs to be rebooted to finish the installation\n");
        if (MessageBox(NULL, L"Installation successful. The machine needs to reboot for the installation to finish. Reboot now?", L"Success",MB_YESNO )==IDYES) {
            system("shutdown /r /t 0");
        }
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
        printf("Failed to move file\n");
        MessageBox(NULL, L"Failed to move file", L"Error", MB_OK);
		break;
    case 8:
		printf("Failed to move secrets.txt\n");
		MessageBox(NULL, L"Failed to move secrets.txt", L"Error", MB_OK);
		break;
    case 9:
		printf("Failed to download settings\n");
		MessageBox(NULL, L"Failed to download settings", L"Error", MB_OK);
		break;
    case 10:
        printf("Failed to download database file\n");
        MessageBox(NULL, L"Failed to download database file", L"Error", MB_OK);
        break;
    case 11:
        printf("failed to start cyberhex\n");
        MessageBox(NULL, L"failed to start cyberhex", L"Error", MB_OK);
        break;
    default:
        break;
    }
    system("pause");
}

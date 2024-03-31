// ma_installer.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//
#define _WIN32_WINNT 0x0500

#include <iostream>
#include <windows.h>
#include <sddl.h>
#include <stdio.h>
#include "download.h"
#include "well_known.h"
#include <iostream>
#include <windows.h>
#include <objbase.h> // For CoInitialize and CoUninitialize
#include <objidl.h>  // For IPersistFile
#include <propvarutil.h> // For InitPropVariantFromString
#include <shobjidl.h> // For IShellLink
#include <shlobj.h>
#include <propkey.h> // For PKEY_AppUserModel_ID

/*
Tasks to do:
- launch as admin
- create secure folder
- download app files from server
- install app files
- create background service
- create folder for communication

*/
//create task thx chatgpt ;)#define _WIN32_DCOM

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <comdef.h>
#include <taskschd.h>
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")

using namespace std;

int create_task()
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        cout << "CoInitializeEx failed: " << hex << hr << endl;
        return 1;
    }

    hr = CoInitializeSecurity(
        NULL,
        -1,
        NULL,
        NULL,
        RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        0,
        NULL);

    if (FAILED(hr))
    {
        cout << "CoInitializeSecurity failed: " << hex << hr << endl;
        CoUninitialize();
        return 1;
    }

    LPCWSTR wszTaskName = L"CyberhexBackgroundTask";
    wstring wstrExecutablePath = L"c:\\program files\\cyberhex\\secure\\app\\cyberhex.exe";

    ITaskService* pService = NULL;
    hr = CoCreateInstance(CLSID_TaskScheduler,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_ITaskService,
        (void**)&pService);
    if (FAILED(hr))
    {
        cout << "Failed to create an instance of ITaskService: " << hex << hr << endl;
        CoUninitialize();
        return 1;
    }

    hr = pService->Connect(_variant_t(), _variant_t(),
        _variant_t(), _variant_t());
    if (FAILED(hr))
    {
        cout << "ITaskService::Connect failed: " << hex << hr << endl;
        pService->Release();
        CoUninitialize();
        return 1;
    }

    ITaskFolder* pRootFolder = NULL;
    hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
    if (FAILED(hr))
    {
        cout << "Cannot get Root Folder pointer: " << hex << hr << endl;
        pService->Release();
        CoUninitialize();
        return 1;
    }

    pRootFolder->DeleteTask(_bstr_t(wszTaskName), 0);

    ITaskDefinition* pTask = NULL;
    hr = pService->NewTask(0, &pTask);

    pService->Release();
    if (FAILED(hr))
    {
        cout << "Failed to create a task definition: " << hex << hr << endl;
        pRootFolder->Release();
        CoUninitialize();
        return 1;
    }

    IPrincipal* pPrincipal = NULL;
    hr = pTask->get_Principal(&pPrincipal);
    if (FAILED(hr))
    {
        cout << "Cannot get principal pointer: " << hex << hr << endl;
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
    pPrincipal->Release();
    if (FAILED(hr))
    {
        cout << "Cannot set highest privilege level: " << hex << hr << endl;
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    ITaskSettings* pSettings = NULL;
    hr = pTask->get_Settings(&pSettings);
    if (FAILED(hr))
    {
        cout << "Cannot get settings pointer: " << hex << hr << endl;
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    hr = pSettings->put_ExecutionTimeLimit(_bstr_t(L"PT0S")); // Set execution time limit to zero seconds
    pSettings->Release();
    if (FAILED(hr))
    {
        cout << "Cannot set execution time limit: " << hex << hr << endl;
        pRootFolder->Release();
        pService->Release();
        CoUninitialize();
        return 1;
    }


    hr = pSettings->put_DisallowStartIfOnBatteries(VARIANT_FALSE); // Set to allow start on batteries
    pSettings->Release();
    if (FAILED(hr))
    {
        cout << "Cannot set start on batteries: " << hex << hr << endl;
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    ITriggerCollection* pTriggerCollection = NULL;
    hr = pTask->get_Triggers(&pTriggerCollection);
    if (FAILED(hr))
    {
        cout << "Cannot get trigger collection: " << hex << hr << endl;
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    ITrigger* pTrigger = NULL;
    hr = pTriggerCollection->Create(TASK_TRIGGER_BOOT, &pTrigger);
    pTriggerCollection->Release();
    if (FAILED(hr))
    {
        cout << "Cannot create the trigger: " << hex << hr << endl;
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    IActionCollection* pActionCollection = NULL;
    hr = pTask->get_Actions(&pActionCollection);
    if (FAILED(hr))
    {
        cout << "Cannot get Task collection pointer: " << hex << hr << endl;
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    IAction* pAction = NULL;
    hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
    pActionCollection->Release();
    if (FAILED(hr))
    {
        cout << "Cannot create the action: " << hex << hr << endl;
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    IExecAction* pExecAction = NULL;
    hr = pAction->QueryInterface(
        IID_IExecAction, (void**)&pExecAction);
    pAction->Release();
    if (FAILED(hr))
    {
        cout << "QueryInterface call failed for IExecAction: " << hex << hr << endl;
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    hr = pExecAction->put_Path(_bstr_t(wstrExecutablePath.c_str()));
    pExecAction->Release();
    if (FAILED(hr))
    {
        cout << "Cannot set path of executable: " << hex << hr << endl;
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    IRegisteredTask* pRegisteredTask = NULL;
    VARIANT varPassword;
    varPassword.vt = VT_EMPTY;
    hr = pRootFolder->RegisterTaskDefinition(
        _bstr_t(wszTaskName),
        pTask,
        TASK_CREATE_OR_UPDATE,
        _variant_t(L"SYSTEM"),
        varPassword,
        TASK_LOGON_SERVICE_ACCOUNT,
        _variant_t(L""),
        &pRegisteredTask);
    if (FAILED(hr))
    {
        cout << "Error saving the Task : " << hex << hr << endl;
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    cout << "Success! Task successfully registered." << endl;

    pRootFolder->Release();
    pTask->Release();
    pRegisteredTask->Release();
    CoUninitialize();
    return 0;
}




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
BOOL create_insecure_dacl(SECURITY_ATTRIBUTES* pSA)
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
        TEXT("(A;OICI;GA;;;AU)")       // Deny access to authenticated users
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
int create_insecure_folder(LPCWSTR folderpath) {
    int error = 0;
    SECURITY_ATTRIBUTES  sa;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = FALSE;// objects are not inherited

    // Call function to set the DACL. The DACL
    // is set in the SECURITY_ATTRIBUTES 
    // lpSecurityDescriptor member.
    if (!create_insecure_dacl(&sa))
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

int check_cert(const char* cert, const char* secrets_path) {
    FILE* fp;
    if (fopen_s(&fp, secrets_path, "r") != 0) {
        return 1;
    }
    else {
        char* secrets = new char[505];
        while (!feof(fp)) {
            fscanf_s(fp, "%s", secrets, 500); // get the secret
            if (strcmp("cert", secrets) == 0) {
                fscanf_s(fp, "%s", secrets, 500); // get the secret
                if (strcmp(cert, secrets) == 0) {
                    delete[] secrets;
                    return 0;
                }
            }
        }
        delete[] secrets;
        return 2;
    }

}
char* get_apikey(const char* secrets_path) {
    FILE* fp;
    if (fopen_s(&fp, secrets_path, "r") != 0) {
        return 0;
    }
    else {
        char* secrets = new char[505];
        while (!feof(fp)) {
            fscanf_s(fp, "%s", secrets, 500); // get the secret
            if (strcmp("apikey", secrets) == 0) {
                fscanf_s(fp, "%s", secrets, 500); // get the secret
                fclose(fp);
                return secrets;
            }
        }
        fclose(fp);
        delete[] secrets;
        return 0;
    }

}
char* get_machineid(const char* secrets_path) {
    FILE* fp;
    if (fopen_s(&fp, secrets_path, "r") != 0) {
        return 0;
    }
    else {
        char* secrets = new char[300];
        while (!feof(fp)) {
            fscanf_s(fp, "%s", secrets, 295); // get the secret
            if (strcmp("machineid", secrets) == 0) {
                fscanf_s(fp, "%s", secrets, 295); // get the secret
                fclose(fp);
                return secrets;
            }
        }
        fclose(fp);
        delete[] secrets;
        return 0;
    }
    fclose(fp);
}
char* get_server(const char* secrets_path) {
    FILE* fp;
    if (fopen_s(&fp, secrets_path, "r") != 0) {
        return 0;
    }
    else {
        char* secrets = new char[300];
        while (!feof(fp)) {
            fscanf_s(fp, "%s", secrets, 295); // get the secret
            if (strcmp("server", secrets) == 0) {
                fscanf_s(fp, "%s", secrets, 295); // get the secret
                fclose(fp);
                return secrets;
            }
        }
        fclose(fp);
        delete[] secrets;
        return 0;
    }
    fclose(fp);
}

int update_db(const char* folder_path) {
    //download the databases from the server
    for (char firstChar = '0'; firstChar <= 'f'; ++firstChar) {
        for (char secondChar = '0'; secondChar <= 'f'; ++secondChar) {
            // Ensure that the characters are valid hexadecimal digits
            if (!std::isxdigit(firstChar) || !std::isxdigit(secondChar) or std::isupper(firstChar) or std::isupper(secondChar)) {
                continue;
            }

            // Create the filename based on the naming convention
            
            char file_name[]= { firstChar, secondChar ,'.','j','d','b','f','\0' };
            //create the strings to download the files
            char* url = new char[300];
            char* output_path = new char[300];
            strcpy_s(url, 295, get_server("setup.txt"));
            strcat_s(url, 295, "/database_srv/");
            strcat_s(url, 295, file_name);
            strcpy_s(output_path, 295, folder_path);
            strcat_s(output_path, 295, "\\");
            strcat_s(output_path, 295, file_name);
            printf("%s\n", url);
            int res = download_file_from_srv(url, output_path);
            if (res != 0) {
                return 10;
            }


            delete[] url;
            delete[] output_path;

        }
    }
    return 0;
}
int update_settings(const char* settings_type) {
    //create the strings to download the files
    char* url = new char[1000];
    strcpy_s(url, 1000, get_server("setup.txt"));
    strcat_s(url, 1000, "/api/php/settings/get_settings.php?");//need to add machine_id and apikey
    strcat_s(url, 1000, settings_type);
    strcat_s(url, 1000, "&machine_id=");
    strcat_s(url, 1000, get_machineid(SECRETS));
    strcat_s(url, 1000, "&apikey=");
    strcat_s(url, 1000, get_apikey(SECRETS));
    int res = 1;
    if (strcmp(settings_type, "settings") == 0)
        res = download_file_from_srv(url, SETTINGS_DB);
    else if (strcmp(settings_type, "rtp_included") == 0)
        res = download_file_from_srv(url, INCLUDED_FOLDERS);
    else if (strcmp(settings_type, "rtp_excluded") == 0)
        res = download_file_from_srv(url, EXCLUDED_FOLDERS);
    else if (strcmp(settings_type, "sched") == 0)
        res = download_file_from_srv(url, SCHED_PATH);
    //int res = 0;
    if (res != 0) {
        return 1;
    }

    delete[] url;
    return 0;
}
int action_update_settings() {
    //update the settings
    int err = 0;
    if (update_settings("settings") != 0) {
        err = 9;
    }
    //update the included folders
    if (update_settings("rtp_included") != 0) {
        err = 9;
    }
    //update the excluded folders
    if (update_settings("rtp_excluded") != 0) {
        err = 9;
    }
    //update the schedule
    if (update_settings("sched") != 0) {
        err = 9;
    }
    return err;
}
int action_update_db() {
    //update the databases
    return update_db(DB_DIR);
}
int copy(const char* source_path, const char* destination_path) {
    FILE* source_file, * destination_file;
    char buffer[4096]; // Buffer to store data read from source file

    // Open source file for reading
    if (fopen_s(&source_file, source_path, "rb") != 0) {
        return 1;
    }

    // Open destination file for writing
    if (fopen_s(&destination_file, destination_path, "wb") != 0) {
        fclose(source_file);
        return 1;
    }

    // Copy contents from source to destination
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), source_file)) > 0) {
        fwrite(buffer, 1, bytes_read, destination_file);
    }

    // Close files
    fclose(source_file);
    fclose(destination_file);


    return 0;

}


HRESULT create_shortcut(const wchar_t* targetPath, const wchar_t* shortcutPath, const wchar_t* iconPath) {
    // Initialize the COM library
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        return hr;
    }

    // Create an instance of the ShellLink interface
    IShellLink* pShellLink = NULL;
    hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&pShellLink);
    if (FAILED(hr)) {
        CoUninitialize();
        return hr;
    }

    // Set the path of the target file
    pShellLink->SetPath(targetPath);

    // Query the IPropertyStore interface
    IPropertyStore* pPropertyStore = NULL;
    hr = pShellLink->QueryInterface(IID_IPropertyStore, (void**)&pPropertyStore);
    if (FAILED(hr)) {
        pShellLink->Release();
        CoUninitialize();
        return hr;
    }

    // Load the icon file and set the System.Icon property
    PROPVARIANT propvar;
    hr = InitPropVariantFromString(iconPath, &propvar);
    if (SUCCEEDED(hr)) {
        hr = pPropertyStore->SetValue(PKEY_AppUserModel_ID, propvar);
        if (SUCCEEDED(hr)) {
            hr = pPropertyStore->Commit();
            if (FAILED(hr)) {
                CoUninitialize();
                pPropertyStore->Release();
                pShellLink->Release();
                return hr;
            }
        }
        else {
            CoUninitialize();
            pPropertyStore->Release();
            pShellLink->Release();
            return hr;
        }
        PropVariantClear(&propvar); // Free memory allocated by InitPropVariantFromString
    }
    else {
        CoUninitialize();
        pPropertyStore->Release();
        pShellLink->Release();
        return hr;
    }

    // Release the IPropertyStore interface
    pPropertyStore->Release();

    // Query the IPersistFile interface
    IPersistFile* pPersistFile = NULL;
    hr = pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);
    if (FAILED(hr)) {
        pShellLink->Release();
        CoUninitialize();
        return hr;
    }

    // Save the shortcut to disk
    hr = pPersistFile->Save(shortcutPath, TRUE);
    if (FAILED(hr)) {
        pPersistFile->Release();
        pShellLink->Release();
        CoUninitialize();
        return hr;
    }

    // Release COM interfaces
    pPersistFile->Release();
    pShellLink->Release();

    // Uninitialize the COM library
    CoUninitialize();

    return S_OK; // Success
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
            printf("Movin cyberhex.exe\n");
            //!!need to change: download in  advance and then move to the correct folder
            //error=download_file_from_srv("https://github.com/jakani24/cyberhex_bin_distro/raw/main/client_backend.exe", "C:\\Program Files\\cyberhex\\secure\\app\\cyberhex.exe");
            if(rename("client_backend.exe", "C:\\Program Files\\cyberhex\\secure\\app\\cyberhex.exe")!=0)
                	error = 7;
            if (error == 0) {
                printf("Moving libcrypto-3-x64.dll\n");
                //error = download_file_from_srv("https://github.com/jakani24/cyberhex_bin_distro/raw/main/libcrypto-3-x64.dll", "C:\\Program Files\\cyberhex\\secure\\app\\libcrypto-3-x64.dll");
                if(rename("libcrypto-3-x64.dll", "C:\\Program Files\\cyberhex\\secure\\app\\libcrypto-3-x64.dll")!=0)
                    error = 7;
            }if (error == 0) {
                printf("Moving libcurl.dll\n");
                //error = download_file_from_srv("https://github.com/jakani24/cyberhex_bin_distro/raw/main/libcurl.dll", "C:\\Program Files\\cyberhex\\secure\\app\\libcurl.dll");
                if(rename("libcurl.dll", "C:\\Program Files\\cyberhex\\secure\\app\\libcurl.dll")!=0)
					error = 7;
            }if (error == 0) {
                printf("Moving zlib1.dll\n");
                if(rename("zlib1.dll", "C:\\Program Files\\cyberhex\\secure\\app\\zlib1.dll")!=0)
                    	error = 7;
                //error = download_file_from_srv("https://github.com/jakani24/cyberhex_bin_distro/raw/main/zlib1.dll", "C:\\Program Files\\cyberhex\\secure\\app\\zlib1.dll");
            }if (error == 0) {
                printf("Moving cyberhex_desktop.exe\n");
                if(rename("client_frontend.exe", "C:\\Program Files\\cyberhex\\app\\cyberhex_desktop.exe")!=0)
                    error = 7;
                //error = download_file_from_srv("https://github.com/jakani24/cyberhex_bin_distro/raw/main/client_frontend.exe", "C:\\Program Files\\cyberhex\\app\\cyberhex_desktop.exe");
            }if (error == 0) {
                printf("Moving msvcp140.dll\n");
                //error = download_file_from_srv("https://github.com/jakani24/cyberhex_bin_distro/raw/main/libcurl.dll", "C:\\Program Files\\cyberhex\\secure\\app\\libcurl.dll");
                if (copy("msvcp140.dll", "C:\\Program Files\\cyberhex\\secure\\app\\msvcp140.dll") != 0)
                    error = 7;
                if (copy("msvcp140.dll", "C:\\Program Files\\cyberhex\\app\\msvcp140.dll") != 0)
                    error = 7;
            }if (error == 0) {
                printf("Moving msvcp140d.dll\n");
                //error = download_file_from_srv("https://github.com/jakani24/cyberhex_bin_distro/raw/main/libcurl.dll", "C:\\Program Files\\cyberhex\\secure\\app\\libcurl.dll");
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
            }
        }


        HRESULT hr = create_task();
        if (FAILED(hr))
        {
            std::cerr << "Task creation failed!" << std::endl;
            error=5;
        }
        //create the shortcut
        
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

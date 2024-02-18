// ma_uninstaller.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//
//todo:
/* remove folders
* remove background task
*/
#include <iostream>
#include <Windows.h>
#include <iostream>
#include <Windows.h>
#include <taskschd.h>
#include <comdef.h> 
#pragma comment(lib, "taskschd.lib")
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
bool remove_dir(const std::wstring& path) {
    // Remove the directory and its contents recursively
    if (!RemoveDirectory(path.c_str())) {
        DWORD error = GetLastError();
        if (error != ERROR_DIR_NOT_EMPTY) {
            // Failed to remove the directory
            std::wcerr << L"Error removing directory '" << path << L"'. Error code: " << error << std::endl;
            return false;
        }

        // The directory is not empty, so we need to remove its contents first
        WIN32_FIND_DATA findFileData;
        HANDLE hFind = FindFirstFile((path + L"\\*").c_str(), &findFileData);

        if (hFind == INVALID_HANDLE_VALUE) {
            std::wcerr << L"Error finding files in directory '" << path << L"'. Error code: " << GetLastError() << std::endl;
            return false;
        }

        do {
            if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0) {
                std::wstring filePath = path + L"\\" + findFileData.cFileName;

                if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    // Recursively remove subdirectories
                    if (!remove_dir(filePath)) {
                        FindClose(hFind);
                        return false;
                    }
                }
                else {
                    // Delete files in the directory
                    if (DeleteFile(filePath.c_str()) == FALSE) {
                        std::wcerr << L"Error deleting file '" << filePath << L"'. Error code: " << GetLastError() << std::endl;
                        FindClose(hFind);
                        return false;
                    }
                }
            }
        } while (FindNextFile(hFind, &findFileData) != 0);

        FindClose(hFind);

        // Try to remove the directory again after its contents have been deleted
        if (!RemoveDirectory(path.c_str())) {
            std::wcerr << L"Error removing directory '" << path << L"'. Error code: " << GetLastError() << std::endl;
            return false;
        }
    }

    return true;
}

bool remove_scheduled_task(const std::wstring& taskName) {
    HRESULT hr;

    // Initialize COM
    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        std::cerr << "COM initialization failed with error code: " << hr << std::endl;
        return false;
    }

    // Create an instance of the Task Service
    ITaskService* pService = NULL;
    hr = CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pService);
    if (FAILED(hr)) {
        std::cerr << "Failed to create an instance of ITaskService: " << hr << std::endl;
        CoUninitialize();
        return false;
    }

    // Connect to the task service
    hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
    if (FAILED(hr)) {
        std::cerr << "ITaskService::Connect failed with error code: " << hr << std::endl;
        pService->Release();
        CoUninitialize();
        return false;
    }

    // Get the root task folder
    ITaskFolder* pRootFolder = NULL;
    hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
    if (FAILED(hr)) {
        std::cerr << "Cannot get Root Folder pointer: " << hr << std::endl;
        pService->Release();
        CoUninitialize();
        return false;
    }

    // Delete the task
    hr = pRootFolder->DeleteTask(_bstr_t(taskName.c_str()), 0);
    if (FAILED(hr)) {
        std::cerr << "Failed to delete the task: " << hr << std::endl;
        pRootFolder->Release();
        pService->Release();
        CoUninitialize();
        return false;
    }

    // Release COM objects
    pRootFolder->Release();
    pService->Release();
    CoUninitialize();

    return true;
}
int main()
{
    printf("Welcome to the Cyberhex uninstaller!\n");
    int error = 0;
    if (!is_admin()) {
        printf("We are not administrator, requesting UAC\n");
        if (!run_as_admin()) {
            printf("We did not get administrative rights. Please restart the uninstaller!\n");
            MessageBox(NULL, L"Please start the uninstaller with amdin privileges!", L"Error", MB_OK);
            exit(1);
        }
        else {
            //we started the app as admin. This process can be terminated now
            exit(0);
        }
    }
    else {
        printf("Stopping cyberhex");
        system("taskkill /F /IM cyberhex.exe");
        printf("Removing directorys\n");
        printf("Removing directory for application\n");
        error = remove_dir(L"C:\\Program Files\\Cyberhex");
        if (error == 0)
            error = 4;
        else
            error = 0;
        if (error == 0) {
            printf("Removing background task\n");
            if (!remove_scheduled_task(L"CyberhexBackgroundTask")) {
				error = 5;
			}
        }
        
    }
    switch (error) {
        case 0:
			printf("Uninstall finished successfully!\n");
            MessageBox(NULL, L"Uninstall finished successfully!", L"Success", MB_OK);
			break;
        case 1:
            printf("Failed to open service control manager.\n");
            MessageBox(NULL, L"Failed to open service control manager!", L"Error", MB_OK);
            break;
        case 2:
            printf("Failed to open service.\n");
            MessageBox(NULL, L"Failed to open service!", L"Error", MB_OK);
			break;
        case 3:
			printf("Failed to delete service.\n");
            MessageBox(NULL, L"Failed to delete service!", L"Error", MB_OK);    
        break;
        case 4:
            printf("Failed to remove directory.\n");
			MessageBox(NULL, L"Failed to remove directory!", L"Error", MB_OK);
		default:
			printf("Unknown error\n");
            MessageBox(NULL, L"Unknown error!", L"Error", MB_OK);
			break;


    }
}


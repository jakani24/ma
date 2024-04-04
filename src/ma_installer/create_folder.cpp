/*
This file contains the functions to create a folder with a secure or insecure DACL, copy a file, and create a shortcut.

Functions:
	- create_dacl: Creates a DACL with secure permissions.
	- create_insecure_dacl: Creates a DACL with insecure permissions.
	- create_secure_folder: Creates a folder with a secure DACL.
	- create_insecure_folder: Creates a folder with an insecure DACL.
	- copy: Copies a file from the source path to the destination path.
	- create_shortcut: Creates a shortcut to a target file with an icon.

*/


#include "create_folder.h"
#include <iostream>
#include <windows.h>
#include <objbase.h> // For CoInitialize and CoUninitialize
#include <objidl.h>  // For IPersistFile
#include <propvarutil.h> // For InitPropVariantFromString
#include <shobjidl.h> // For IShellLink
#include <shlobj.h>
#include <propkey.h> // For PKEY_AppUserModel_ID
#include <comdef.h>
#include <taskschd.h>
#include <sddl.h>
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



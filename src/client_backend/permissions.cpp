#ifndef PERMISSIONS_CPP
#define PERMISSIONS_CPP
#define _WIN32_WINNT 0x0500
#pragma comment(lib, "advapi32.lib")
#include "permissions.h"
#include <mutex> // Include mutex for synchronization

// Mutex for synchronizing file operations
std::mutex fileMutex;

/*
1 create file (as admin)
2 set file as read only (also as admin)
file cannot be deleted or modified by anyone. admin can delete

*/

//mark as readonly
int protect_file(char* path) {
    std::lock_guard<std::mutex> lock(fileMutex); // Lock the mutex
    return _chmod(path, _S_IREAD);
}

//mark as readwrite
int unprotect_file(char* path) {
    std::lock_guard<std::mutex> lock(fileMutex); // Lock the mutex
    return _chmod(path, _S_IWRITE | _S_IREAD);
}

//deny all access and only grant access to admins
BOOL create_file_protection(SECURITY_ATTRIBUTES* pSA)
{
    // Define the SDDL for the DACL. This example sets 
    // the following access:
    //     Built-in guests are denied all access.
    //     Anonymous logon is denied all access.
    //     Authenticated users are denied all access.
    //     Administrators are allowed full control.
    // Modify these values as needed to generate the proper
    // DACL for your application.   
    LPCSTR szSD = TEXT("D:")
        TEXT("(D;OICI;GA;;;BG)")         // Deny access to guest users
        TEXT("(D;OICI;GA;;;AN)")         // Deny access to unauthenticated users
        //TEXT("(D;OICI;GA;;;AU)")         // Deny access to authenticated users do not execute else not even admins have rights anymore :(
        TEXT("(A;OICI;GA;;;BA)");        // Allow full control to builtin administrators
    TEXT("(A;OICI;GA;;;AA)");        // Allow full control to normal administrators


    if (NULL == pSA)
        return FALSE;

    return ConvertStringSecurityDescriptorToSecurityDescriptor(
        szSD,
        SDDL_REVISION_1,
        &(pSA->lpSecurityDescriptor),
        NULL);
}

#endif

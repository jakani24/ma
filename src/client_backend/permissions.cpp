/*
This file contains the implementation of the functions to handle file permissions -> to protect files from being modified or deleted by unauthorized users.

Functions:
	- protect_file(): This function marks a file as read-only.
	- unprotect_file(): This function marks a file as read-write.
	- create_file_protection(): This function creates a file protection for a file using ACL.
*/

#ifndef PERMISSIONS_CPP
#define PERMISSIONS_CPP
#define _WIN32_WINNT 0x0500
#pragma comment(lib, "advapi32.lib")
#include "permissions.h"
#include <mutex> // Include mutex for synchronizationddd
#include <Windows.h>
#include <io.h> // Include for _chmod function

// Mutex for synchronizing file operations
std::mutex fileMutex;

// Mark as read-only
int protect_file(const char* path) {
    std::lock_guard<std::mutex> lock(fileMutex); // Lock the mutex
    return _chmod(path, _S_IREAD);
}

// Mark as read-write
int unprotect_file(const char* path) {
    std::lock_guard<std::mutex> lock(fileMutex); // Lock the mutex
    return _chmod(path, _S_IWRITE | _S_IREAD);
}

// Deny all access and only grant access to admins
BOOL create_file_protection(SECURITY_ATTRIBUTES* pSA)
{
    // Define the SDDL for the DACL. This example sets 
    // the following access:
    //     Built-in guests are denied all access.
    //     Anonymous logon is denied all access.a
    //     Authenticated users are denied all access.
    //     Administrators are allowed full control.
    // Modify these values as needed to generate the proper
    // DACL for your application.   
    LPCSTR szSD = TEXT("D:")
        TEXT("(D;OICI;GA;;;BG)")         // Deny access to guest users
        TEXT("(D;OICI;GA;;;AN)")         // Deny access to unauthenticated users
        //TEXT("(D;OICI;GA;;;AU)")       // Deny access to authenticated users do not execute else not even admins have rights anymore :(
        TEXT("(A;OICI;GA;;;BA)");        // Allow full control to builtin administrators
    TEXT("(A;OICI;GA;;;AA)");            // Allow full control to normal administrators

    if (NULL == pSA)
        return FALSE;

    return ConvertStringSecurityDescriptorToSecurityDescriptor(
        szSD,
        SDDL_REVISION_1,
        &(pSA->lpSecurityDescriptor),
        NULL);
}

#endif

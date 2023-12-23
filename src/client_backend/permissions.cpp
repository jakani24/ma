#ifndef PERMISSIONS_CPP
#define PERMISSIONS_CPP
#define _WIN32_WINNT 0x0500
#pragma comment(lib, "advapi32.lib")
#include "permissions.h"
/*
1 create file (as admin)
2 set file as read only (also as admin)
file cannot be deleted or modified by anyone. admin can delete

*/
//mark as readonly
int protect_file(char* path) {
    return _chmod(path, _S_IREAD);
}
//mark as readwrite
int unprotect_file(char* path) {
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
/*


BOOL CreateMyDACL(SECURITY_ATTRIBUTES*);

int main()
{
    SECURITY_ATTRIBUTES  sa;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = FALSE;

    // Call function to set the DACL. The DACL
    // is set in the SECURITY_ATTRIBUTES 
    // lpSecurityDescriptor member.
    if (!CreateMyDACL(&sa))
    {
        // Error encountered; generate message and exit.
        printf("Failed CreateMyDACL\n");
        exit(1);
    }

    // Use the updated SECURITY_ATTRIBUTES to specify
    // security attributes for securable objects.
    // This example uses security attributes during
    // creation of a new directory.
    if (0 == CreateDirectory(TEXT("C:\\MyFolder"), &sa))
    {
        // Error encountered; generate message and exit.
        printf("Failed CreateDirectory\n");
        exit(1);
    }

    // Free the memory allocated for the SECURITY_DESCRIPTOR.
    if (NULL != LocalFree(sa.lpSecurityDescriptor))
    {
        // Error encountered; generate message and exit.
        printf("Failed LocalFree\n");
        exit(1);
    }
    return 0;
}


// CreateMyDACL.
//    Create a security descriptor that contains the DACL 
//    you want.
//    This function uses SDDL to make Deny and Allow ACEs.
//
// Parameter:
//    SECURITY_ATTRIBUTES * pSA
//    Pointer to a SECURITY_ATTRIBUTES structure. It is your
//    responsibility to properly initialize the 
//    structure and to free the structure's 
//    lpSecurityDescriptor member when you have
//    finished using it. To free the structure's 
//    lpSecurityDescriptor member, call the 
//    LocalFree function.
// 
// Return value:
//    FALSE if the address to the structure is NULL. 
//    Otherwise, this function returns the value from the
//    ConvertStringSecurityDescriptorToSecurityDescriptor 
//    function.
BOOL CreateMyDACL(SECURITY_ATTRIBUTES* pSA)
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
    TCHAR* szSD = TEXT("D:")
        TEXT("(D;OICI;GA;;;BG)")         // Deny access to authenticated users
        TEXT("(D;OICI;GA;;;AN)")         // Deny access to authenticated users
        //TEXT("(D;OICI;GA;;;AU)")         // Deny access to authenticated users
        TEXT("(A;OICI;GA;;;BA)");        // Allow full control to builtinadministrators
    TEXT("(A;OICI;GA;;;AA)");        // Allow full control to administrators


    if (NULL == pSA)
        return FALSE;

    return ConvertStringSecurityDescriptorToSecurityDescriptor(
        szSD,
        SDDL_REVISION_1,
        &(pSA->lpSecurityDescriptor),
        NULL);
}
*/
#endif
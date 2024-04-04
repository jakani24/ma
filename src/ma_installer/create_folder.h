#pragma once
#include <Windows.h>
BOOL create_dacl(SECURITY_ATTRIBUTES* pSA);
BOOL create_insecure_dacl(SECURITY_ATTRIBUTES* pSA);
int create_secure_folder(LPCWSTR folderpath);
int create_insecure_folder(LPCWSTR folderpath);
int copy(const char* src, const char* dest);
HRESULT create_shortcut(const wchar_t* targetPath, const wchar_t* shortcutPath, const wchar_t* iconPath);
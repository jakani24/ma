#include "choose_element.h"
#include <Windows.h>
#include <iostream>
#include <string>
std::wstring open_file(HWND hWnd)
{
    OPENFILENAME ofn;
    WCHAR szFile[MAX_PATH] = L"";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"All Files\0*.*\0"; // Filter for all files
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        // User selected a file, 'szFile' contains the selected file path
        return std::wstring(szFile);
    }
    else
    {
        // User canceled the dialog
        //MessageBox(hWnd, L"File selection canceled.", L"Canceled", MB_OK | MB_ICONEXCLAMATION);
        return L"cancel";
    }
}
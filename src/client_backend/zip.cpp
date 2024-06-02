/*
This file includes the unzip function which is used to unzip files. The function uses the IShellDispatch interface to interact with the Windows Shell and extract the contents of a ZIP file to a specified destination folder. The function takes two parameters: the source ZIP file path and the destination folder path where the contents will be extracted.

Functions:
- unzip(): This function extracts the contents of a ZIP file to a specified destination folder using the IShellDispatch interface. It takes two parameters: the source ZIP file path and the destination folder path.

Code copied from:
- https://vcpptips.wordpress.com/2013/10/28/how-to-zipunzip-files/
*/

#include "zip.h"
#include <windows.h>
#include <string.h>
#include <iostream>
#include <Shldisp.h>
#include <codecvt>


// Unzip the contents of a ZIP file to a specified destination folder
void unzip(const std::string& source, const std::string& dest)
{

    HRESULT hResult = S_FALSE;
    IShellDispatch* pIShellDispatch = NULL;
    Folder* pToFolder = NULL;
    VARIANT variantDir, variantFile, variantOpt;

    CoInitialize(NULL);

    // Create an instance of the Shell object
    hResult = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER,
        IID_IShellDispatch, (void**)&pIShellDispatch);
    if (SUCCEEDED(hResult))
    {
        VariantInit(&variantDir);
        variantDir.vt = VT_BSTR;
        variantDir.bstrVal = SysAllocStringLen(NULL, MultiByteToWideChar(CP_UTF8, 0, dest.c_str(), -1, NULL, 0));
        // Convert the destination folder path to a wide string
        MultiByteToWideChar(CP_UTF8, 0, dest.c_str(), -1, variantDir.bstrVal, SysStringLen(variantDir.bstrVal));
        // Get the destination folder
        hResult = pIShellDispatch->NameSpace(variantDir, &pToFolder);

        if (SUCCEEDED(hResult))
        {
            // Get the source ZIP file
            Folder* pFromFolder = NULL;
            VariantInit(&variantFile);
            variantFile.vt = VT_BSTR;
            variantFile.bstrVal = SysAllocStringLen(NULL, MultiByteToWideChar(CP_UTF8, 0, source.c_str(), -1, NULL, 0));
            MultiByteToWideChar(CP_UTF8, 0, source.c_str(), -1, variantFile.bstrVal, SysStringLen(variantFile.bstrVal));

            // Get the source folder
            pIShellDispatch->NameSpace(variantFile, &pFromFolder);

            FolderItems* fi = NULL;
            pFromFolder->Items(&fi);

            VariantInit(&variantOpt);
            variantOpt.vt = VT_I4;
            variantOpt.lVal = FOF_NO_UI;

            VARIANT newV;
            VariantInit(&newV);
            newV.vt = VT_DISPATCH;
            newV.pdispVal = fi;
            // Extract the contents of the ZIP file to the destination folder
            hResult = pToFolder->CopyHere(newV, variantOpt);
            Sleep(1000);

            pFromFolder->Release();
            pToFolder->Release();
        }
        pIShellDispatch->Release();
    }

    // Cleanup
    CoUninitialize();
    SysFreeString(variantDir.bstrVal);
    SysFreeString(variantFile.bstrVal);
}
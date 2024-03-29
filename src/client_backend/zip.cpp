#include "zip.h"
#include <windows.h>
#include <string.h>
#include <iostream>
#include <Shldisp.h>
#include <codecvt>
//thanks https://vcpptips.wordpress.com/2013/10/28/how-to-zipunzip-files/

void unzip(const std::string& source, const std::string& dest)
{
    HRESULT hResult = S_FALSE;
    IShellDispatch* pIShellDispatch = NULL;
    Folder* pToFolder = NULL;
    VARIANT variantDir, variantFile, variantOpt;

    CoInitialize(NULL);

    hResult = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER,
        IID_IShellDispatch, (void**)&pIShellDispatch);
    if (SUCCEEDED(hResult))
    {
        VariantInit(&variantDir);
        variantDir.vt = VT_BSTR;
        variantDir.bstrVal = SysAllocStringLen(NULL, MultiByteToWideChar(CP_UTF8, 0, dest.c_str(), -1, NULL, 0));
        MultiByteToWideChar(CP_UTF8, 0, dest.c_str(), -1, variantDir.bstrVal, SysStringLen(variantDir.bstrVal));

        hResult = pIShellDispatch->NameSpace(variantDir, &pToFolder);

        if (SUCCEEDED(hResult))
        {
            Folder* pFromFolder = NULL;
            VariantInit(&variantFile);
            variantFile.vt = VT_BSTR;
            variantFile.bstrVal = SysAllocStringLen(NULL, MultiByteToWideChar(CP_UTF8, 0, source.c_str(), -1, NULL, 0));
            MultiByteToWideChar(CP_UTF8, 0, source.c_str(), -1, variantFile.bstrVal, SysStringLen(variantFile.bstrVal));

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
            hResult = pToFolder->CopyHere(newV, variantOpt);
            Sleep(1000);

            pFromFolder->Release();
            pToFolder->Release();
        }
        pIShellDispatch->Release();
    }

    CoUninitialize();
    SysFreeString(variantDir.bstrVal);
    SysFreeString(variantFile.bstrVal);
}
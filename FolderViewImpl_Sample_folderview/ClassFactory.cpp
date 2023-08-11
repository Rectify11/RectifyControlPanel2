/**************************************************************************
    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.
 
   (c) Microsoft Corporation. All Rights Reserved.
**************************************************************************/
#include <shlobj.h>
#include <shlwapi.h>
#include "FolderViewImpl.h"
#include "ElementProvider.h"
#include "ClassFactory.h"
#include "ShellFolder.h"
#include "Guid.h"
#include "pch.h"
#include "CElementWithSite.h"

CFolderViewImplClassFactory::CFolderViewImplClassFactory(REFCLSID rclsid) : m_cRef(1), m_rclsid(rclsid)
{
    DllAddRef();
}

CFolderViewImplClassFactory::~CFolderViewImplClassFactory()
{
    DllRelease();
}

HRESULT CFolderViewImplClassFactory::QueryInterface(__in REFIID riid, __deref_out void **ppv)
{
     static const QITAB qit[] = {
        QITABENT (CFolderViewImplClassFactory, IClassFactory),
        { 0 },
    };
    return QISearch(this, qit, riid, ppv);
}

DWORD CFolderViewImplClassFactory::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

DWORD CFolderViewImplClassFactory::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef)
    {
        delete this;
    }
    return cRef;
}

HRESULT CElementProvider_CreateInstance(__in REFIID riid, __deref_out void** ppv)
{
    HRESULT hr = S_ALLTHRESHOLD;
    CElementProvider* pElementProvider = new CElementProvider();
    hr = pElementProvider ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        hr = pElementProvider->QueryInterface(riid, ppv);
        CElementWithSite::Register(g_hInst, (const unsigned short*)g_hInst, FALSE, NULL,0);
        ClassInfo_CRectifyMainCPLPage::Register(g_hInst, (const unsigned short*)g_hInst, FALSE, NULL, 0, FALSE);
        pElementProvider->Release();
    }
    return hr;
}
HRESULT CFolderViewImplClassFactory::CreateInstance(__in_opt IUnknown* punkOuter, 
                                                    __in REFIID riid, 
                                                    __deref_out void **ppv)
{
    *ppv = NULL;

    HRESULT hr = !punkOuter ? S_OK : CLASS_E_NOAGGREGATION;
    if (SUCCEEDED(hr))
    {
        //if (CLSID_FolderViewImpl == m_rclsid)
        //{
        //    CFolderViewImplFolder* pFolderViewImplShellFolder = new CFolderViewImplFolder(0);
        //    hr = pFolderViewImplShellFolder ? S_OK : E_OUTOFMEMORY;
        //    if (SUCCEEDED(hr))
        //    {          
        //        hr = pFolderViewImplShellFolder->QueryInterface(riid, ppv);
        //        pFolderViewImplShellFolder->Release();
        //    }
        //}
        if (CLSID_FolderViewImplElement == m_rclsid)
        {
            hr = CElementProvider_CreateInstance(riid, ppv);
           
        }
        else
        {
            hr = E_NOINTERFACE;

            char szGuid[40] = { 0 };

            sprintf(szGuid, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", riid.Data1, riid.Data2, riid.Data3, riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6], riid.Data4[7]);

            MessageBox(NULL, szGuid, TEXT("Unknown interface in CFolderViewImplClassFactory::CreateInstance()"), 0);
        }
    }
    return hr;
}

HRESULT CFolderViewImplClassFactory::LockServer(BOOL fLock)
{
    if (fLock)
    {
        DllAddRef();
    }
    else
    {
        DllRelease();
    }
    return S_OK;
}

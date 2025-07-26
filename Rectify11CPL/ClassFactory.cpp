#include <shlobj.h>
#include <shlwapi.h>
#include "Rectify11CPL.h"
#include "ElementProvider.h"
#include "ClassFactory.h"
#include "Guid.h"
#include "RectifyMainPage.h"

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
        if (FAILED(hr)) return hr;

        hr = RectifyMainPage::Register();
        if (FAILED(hr)) return hr;

        pElementProvider->Release();
    }
    return hr;
}

HRESULT CRectifyUtil_CreateInstance(__in REFIID riid, __deref_out void** ppv)
{
    HRESULT hr = S_ALLTHRESHOLD;
    CRectifyUtil* pUtility = new CRectifyUtil();
    hr = pUtility ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        hr = pUtility->QueryInterface(riid, ppv);
        pUtility->Release();
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
        if (m_rclsid == CLSID_FolderViewImplElement)
        {
            hr = CElementProvider_CreateInstance(riid, ppv);
        }
        else if (m_rclsid == CLSID_CRectifyUtil)
        {
            hr = CRectifyUtil_CreateInstance(riid, ppv);
        }
        else if (riid == IID_IUnknown)
        {
            // TODO: is this correct? Not sure why this is being called with IUnknown instead of CRectifyUtil
            //       in Windows 11 23H2
            // for some reason m_rclsid is DCC67965-7FFB-0000-1400 ....
            hr = CRectifyUtil_CreateInstance(riid, ppv);
        }
        else
        {
            hr = E_NOINTERFACE;

            WCHAR szGuid[400] = { 0 };

            swprintf(szGuid, 400, L"RIID: {%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\nRCLSID: {%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
                riid.Data1, riid.Data2, riid.Data3, riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6], riid.Data4[7],
                m_rclsid.Data1, m_rclsid.Data2, m_rclsid.Data3, m_rclsid.Data4[0], m_rclsid.Data4[1], m_rclsid.Data4[2], m_rclsid.Data4[3], m_rclsid.Data4[4], m_rclsid.Data4[5], m_rclsid.Data4[6], m_rclsid.Data4[7]);

            MessageBox(NULL, szGuid, TEXT("Unknown interface in CFolderViewImplClassFactory::CreateInstance()"), MB_ICONERROR);
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

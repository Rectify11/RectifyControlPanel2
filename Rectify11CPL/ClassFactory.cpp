#include <shlobj.h>
#include <shlwapi.h>
#include "Rectify11CPL.h"
#include "ElementProvider.h"
#include "ClassFactory.h"
#include "Guid.h"
#include "Templetes.h"
#include "RectifyMainPage.h"
#include "RectifyThemeCfgPage.h"

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

        DirectUI::ClassInfo<RectifyMainPage, DirectUI::Element, DirectUI::StandardCreator<RectifyMainPage>>::Register();
        //DirectUI::ClassInfo<RectifyThemeCfgPage, DirectUI::Element, DirectUI::StandardCreator<RectifyThemeCfgPage>>::Register();
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
        if (CLSID_FolderViewImplElement == m_rclsid)
        {
            hr = CElementProvider_CreateInstance(riid, ppv);
        }
        else if (CLSID_CRectifyUtil == m_rclsid)
        {
            hr = CRectifyUtil_CreateInstance(riid, ppv);
        }
        else
        {
            hr = E_NOINTERFACE;

            WCHAR szGuid[40] = { 0 };

            swprintf(szGuid, 40, L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", riid.Data1, riid.Data2, riid.Data3, riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6], riid.Data4[7]);

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

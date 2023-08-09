#include <windows.h>
#include <shlwapi.h>
#include <objbase.h>
#include <Shlobj.h>
#include <olectl.h>
#include <strsafe.h>
#include "Utils.h"
#include "pch.h"
// The GUID for the FolderViewImpl
#include "GUID.h"

// Implementation of ClassFactory
#include "ClassFactory.h"

#pragma comment(lib, "propsys.lib")
#pragma comment(lib, "shlwapi.lib")

#define CONTROLPANEL_NAMESPACE_GUID L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ControlPanel\\NameSpace\\%s"
#define SHELL_EXT_APPROVED        L"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved\\%s"

HINSTANCE g_hInst = NULL;
LONG g_cRefModule = 0;

void DllAddRef()
{
    InterlockedIncrement(&g_cRefModule);
}

void DllRelease()
{
    InterlockedDecrement(&g_cRefModule);
}
HANDLE g_hActCtx;
BOOL RegisterContext(HINSTANCE hInstance)
{
    if (hInstance == NULL)
    {
        hInstance = GetModuleHandle(NULL);
    }

    WCHAR moduleFileName[264];
    GetModuleFileNameW(hInstance, moduleFileName, 260);

    ACTCTXW pActCtx;
    pActCtx.cbSize = sizeof(ACTCTXW);
    pActCtx.lpApplicationName = NULL;
    pActCtx.lpSource = moduleFileName;
    pActCtx.dwFlags = 0x88;
    pActCtx.hModule = hInstance;
    
    HANDLE ctx = CreateActCtxW(&pActCtx);
    if (ctx != (HANDLE)-1)
    {
        if (_InterlockedCompareExchange((unsigned __int64*)&g_hActCtx, (unsigned long long)ctx, (unsigned long long) -1) != -1)
        {
            ReleaseActCtx(ctx);
        }
    }
    if (ctx != (HANDLE)-1)
    {
        return TRUE;
    }
    return FALSE;
}

const WCHAR g_szExtTitle[] =  L"Rectify11 settings";

STDAPI_(BOOL) DllMain(HINSTANCE hInstance, DWORD dwReason, void *lpReserved)
{ 
    if (DLL_PROCESS_ATTACH == dwReason)
    {
        if (sizeof(XProvider) != 0x28)
        {
            MessageBox(NULL, "Fatal error: unexpected size of XProvider class", "", 0);
            return FALSE;
        }
        MessageBox(NULL, "Attach a debugger NOW", "", 0);
        g_hInst = hInstance;
        DisableThreadLibraryCalls(hInstance);
        RegisterContext(hInstance);
    }
    return TRUE;
}                              

STDAPI DllCanUnloadNow(void)
{
    return g_cRefModule ? S_FALSE : S_OK;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv)
{
    *ppv = NULL;
    
    HRESULT hr = E_OUTOFMEMORY;
    CFolderViewImplClassFactory* pClassFactory = new CFolderViewImplClassFactory(rclsid);
    if (pClassFactory)
    {
        hr = pClassFactory->QueryInterface(riid, ppv);
        pClassFactory->Release();
    }
    if (hr)
    {
        char szGuid[40] = { 0 };

        sprintf(szGuid, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", riid.Data1, riid.Data2, riid.Data3, riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6], riid.Data4[7]);

        MessageBox(NULL, szGuid, TEXT("Unknown interface in DllGetClassObject()"), 0);
    }
    return hr;
}

void RefreshFolderViews(UINT csidl)
{
    PIDLIST_ABSOLUTE pidl;
    if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, csidl, &pidl)))
    {
        SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_IDLIST, pidl, 0);
        CoTaskMemFree(pidl);
    }
}

// Structure to hold data for individual keys to register.
typedef struct
{
    HKEY  hRootKey;
    PCWSTR pszSubKey;
    PCWSTR pszClassID;
    PCWSTR pszValueName;
    BYTE *pszData;
    DWORD dwType;
} REGSTRUCT;

//
//  1. The classID must be created under HKCR\CLSID.
//     a. DefaultIcon must be set to <Path and Module>,0.
//     b. InprocServer32 set to path and module.
//        i. Threading model specified as Apartment.
//     c. ShellFolder attributes must be set.
//  2. If the extension in non-rooted, its GUID is entered at the desired folder.
//  3. It must be registered as approved for Windows NT or XP.
//
STDAPI DllRegisterServer()
{
    WCHAR szFolderViewImplClassID[64], szElementClassID[64], szSubKey[MAX_PATH], szData[MAX_PATH];
    StringFromGUID2(CLSID_FolderViewImpl, szFolderViewImplClassID, ARRAYSIZE(szFolderViewImplClassID));      // Convert the IID to a string.
    StringFromGUID2(CLSID_FolderViewImplElement, szElementClassID, ARRAYSIZE(szElementClassID)); // Convert the IID to a string.

    // Get the path and module name.
    WCHAR szModulePathAndName[MAX_PATH];
    GetModuleFileNameW(g_hInst, szModulePathAndName, ARRAYSIZE(szModulePathAndName));

    // This will setup and register the basic ClassIDs. 
    DWORD dwData = 0xa0000000;// SFGAO_FOLDER | SFGAO_HASSUBFOLDER | SFGAO_CANDELETE;
    DWORD dwResourceId = 100;
    REGSTRUCT rgRegEntries[] = 
    {
        HKEY_CLASSES_ROOT,  L"CLSID\\%s",                 szFolderViewImplClassID, NULL,                       (LPBYTE)g_szExtTitle,   REG_SZ,    
        HKEY_CLASSES_ROOT,  L"CLSID\\%s",                 szFolderViewImplClassID, L"InfoTip",                 (LPBYTE)L"Customize Rectify11 settings such as theme settings",   REG_SZ,
        HKEY_CLASSES_ROOT,  L"CLSID\\%s",                 szFolderViewImplClassID, L"System.ApplicationName",  (LPBYTE)L"Rectify11.ControlPanelMain",   REG_SZ,
        HKEY_CLASSES_ROOT,  L"CLSID\\%s",                 szFolderViewImplClassID, L"System.ControlPanel.Category",(LPBYTE)L"5",   REG_SZ,
        HKEY_CLASSES_ROOT,  L"CLSID\\%s",                 szFolderViewImplClassID, L"System.Software.TasksFileUrl",(LPBYTE)L"%s,1",   REG_SZ,
        HKEY_CLASSES_ROOT,  L"CLSID\\%s\\InprocServer32", szFolderViewImplClassID, NULL,                       (LPBYTE)L"C:\\Windows\\System32\\Shdocvw.dll",          REG_SZ,
        HKEY_CLASSES_ROOT,  L"CLSID\\%s\\InprocServer32", szFolderViewImplClassID, L"ThreadingModel",          (LPBYTE)L"Apartment",   REG_SZ,
        HKEY_CLASSES_ROOT,  L"CLSID\\%s\\DefaultIcon",    szFolderViewImplClassID, NULL,                       (LPBYTE)L"%s,0",        REG_SZ,
        HKEY_CLASSES_ROOT,  L"CLSID\\%s\\ShellFolder",    szFolderViewImplClassID, L"Attributes",              (LPBYTE)&dwData,        REG_DWORD,
        HKEY_CLASSES_ROOT,  L"CLSID\\%s\\Instance", szFolderViewImplClassID,    L"CLSID",          (LPBYTE)L"{328B0346-7EAF-4BBE-A479-7CB88A095F5B}",   REG_SZ,
        HKEY_CLASSES_ROOT,  L"CLSID\\%s\\Instance\\InitPropertyBag", szFolderViewImplClassID,    L"ResourceDLL",          (LPBYTE)L"%s",   REG_SZ,
        HKEY_CLASSES_ROOT,  L"CLSID\\%s\\Instance\\InitPropertyBag", szFolderViewImplClassID,    L"ResourceID",          (LPBYTE)&dwResourceId,   REG_DWORD,
        
        //element provider
        HKEY_CLASSES_ROOT,  L"CLSID\\%s",                 szElementClassID, NULL,                       (LPBYTE)g_szExtTitle,   REG_SZ,
        HKEY_CLASSES_ROOT,  L"CLSID\\%s\\InprocServer32", szElementClassID, NULL,                       (LPBYTE)L"%s",          REG_SZ,
        HKEY_CLASSES_ROOT,  L"CLSID\\%s\\InprocServer32", szElementClassID, L"ThreadingModel",          (LPBYTE)L"Apartment",   REG_SZ,
    };

    HKEY hKey = NULL;
    HRESULT hr = S_OK;
    
    for (int i = 0; SUCCEEDED(hr) && (i < ARRAYSIZE(rgRegEntries)); i++)
    {
        // Create the sub key string.
        hr = StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), rgRegEntries[i].pszSubKey, rgRegEntries[i].pszClassID);
        if (SUCCEEDED(hr))
        {
            LONG lResult = RegCreateKeyExW(rgRegEntries[i].hRootKey, szSubKey, 0, NULL, 
                                           REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
            if (S_OK == lResult)
            {
                // If this is a string entry, create the string.
                if (REG_SZ == rgRegEntries[i].dwType)
                { 
                    hr = StringCchPrintfW(szData, ARRAYSIZE(szData), (LPWSTR)rgRegEntries[i].pszData, szModulePathAndName);
                    if (SUCCEEDED(hr))
                    {
                        RegSetValueExW(hKey, 
                                       rgRegEntries[i].pszValueName, 
                                       0,
                                       rgRegEntries[i].dwType, 
                                       (LPBYTE)szData, 
                                       (lstrlenW(szData) + 1) * sizeof(WCHAR));
                    }
                }
                else if (REG_DWORD == rgRegEntries[i].dwType)
                {
                    RegSetValueExW(hKey, 
                                   rgRegEntries[i].pszValueName, 
                                   0, rgRegEntries[i].dwType,
                                   rgRegEntries[i].pszData, 
                                   sizeof(DWORD));
                }
                        
                RegCloseKey(hKey);
            }
            else
            {
                hr = SELFREG_E_CLASS;
            }
        }
    }

    if (SUCCEEDED(hr))
    {
        hr = SELFREG_E_CLASS;

        // Now we are ready to register the namespace extension.
        // This will put our extension in My Computer.
        if (SUCCEEDED(StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), CONTROLPANEL_NAMESPACE_GUID, szFolderViewImplClassID)))
        {
            LONG lResult = RegCreateKeyExW(HKEY_LOCAL_MACHINE, szSubKey, 0, NULL, 
                                           REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
            if (S_OK == lResult)
            {
                // Copy our name into the string.
                hr = StringCchCopyW(szData, ARRAYSIZE(szData), g_szExtTitle);
                if (SUCCEEDED(hr))
                {
                    // Set the name of our extension.
                    lResult = RegSetValueExW(hKey, NULL, 0, REG_SZ, (LPBYTE)szData, (lstrlenW(szData) + 1) * sizeof(szData[0]));
                    RegCloseKey(hKey);
                
                    // It is assumed at this point that we are running on Windows XP or later
                    // and therefore the extension needs to be registered as approved.
                    PCWSTR rgszApprovedClassIDs[] = { szFolderViewImplClassID, szElementClassID };
                    for (int i = 0; SUCCEEDED(hr) && i < ARRAYSIZE(rgszApprovedClassIDs); i++)
                    {
                        hr = StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), SHELL_EXT_APPROVED, szFolderViewImplClassID);
                        if (SUCCEEDED(hr))
                        {
                            lResult = RegCreateKeyExW(HKEY_LOCAL_MACHINE, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE,
                                                      KEY_WRITE, NULL, &hKey, NULL);
                            if (S_OK == lResult)
                            {
                                // Create the value string.
                                hr = StringCchCopyW(szData, ARRAYSIZE(szData), g_szExtTitle);
                                if (SUCCEEDED(hr))
                                {
                                    lResult = RegSetValueExW(hKey, 
                                                             NULL, 
                                                             0, 
                                                             REG_SZ, 
                                                             (LPBYTE)szData, 
                                                             (lstrlenW(szData) + 1) * sizeof(WCHAR));
                                        
                                    hr = S_OK;
                                }
                                
                                RegCloseKey(hKey);
                            }
                        }
                    }

                    // The Shell has to be notified that the change has been made.
                    RefreshFolderViews(CSIDL_DRIVES);
                }
            }
        }
    }

    if (SUCCEEDED(hr))
    {
        //Note:  The property description file needs to be in the same
        // directory as FolderViewImpl.dll when registering the DLL.
        hr = PSRegisterPropertySchema(L"FolderViewImpl.propdesc");
    }

    if (FAILED(hr))
    {
        //Remove the stuff we added.
        DllUnregisterServer();
    }
    else
    {
        
    }

    return hr;
}


//
// Registry keys are removed here.
//
STDAPI DllUnregisterServer()
{
    WCHAR szSubKey[MAX_PATH], szFolderClassID[MAX_PATH], szContextMenuClassID[MAX_PATH];
    
    //Delete the context menu entries.
    HRESULT hrCM = StringFromGUID2(CLSID_FolderViewImplElement,
                                  szContextMenuClassID, 
                                  ARRAYSIZE(szContextMenuClassID));
    if (SUCCEEDED(hrCM))
    {
        hrCM = StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), L"CLSID\\%s", szContextMenuClassID);
        if (SUCCEEDED(hrCM))
        {
            hrCM = SHDeleteKeyW(HKEY_CLASSES_ROOT, szSubKey);
        }
    }

    //Delete the foldertype key.
    HRESULT hrFT = SHDeleteKeyW(HKEY_CLASSES_ROOT, L"FolderViewSampleType");
 
    // Delete the namespace extension entries
    StringFromGUID2(CLSID_FolderViewImpl, szFolderClassID, ARRAYSIZE(szFolderClassID));
    HRESULT hrSF = StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), CONTROLPANEL_NAMESPACE_GUID, szFolderClassID);
    if (SUCCEEDED(hrSF))
    {
        hrSF = SHDeleteKeyW(HKEY_LOCAL_MACHINE, szSubKey);
        if (SUCCEEDED(hrSF))
        {
            // Delete the object's registry entries
            hrSF = StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), L"CLSID\\%s", szFolderClassID);
            if (SUCCEEDED(hrSF))
            {
                hrSF = SHDeleteKeyW(HKEY_CLASSES_ROOT, szSubKey);
                if (SUCCEEDED(hrSF))
                {
                    hrSF = StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), SHELL_EXT_APPROVED, szFolderClassID);
                    if (SUCCEEDED(hrSF))
                    {
                        hrSF = SHDeleteKeyW(HKEY_LOCAL_MACHINE, szSubKey);
               
                        // Refresh the folder views that might be open
                        RefreshFolderViews(CSIDL_DRIVES);
                    }
                }
            }
        }
    }

    // Remove the property description
    HRESULT hrPS = PSUnregisterPropertySchema(L"FolderViewImpl.propdesc");
    
    return (SUCCEEDED(hrPS) && SUCCEEDED(hrCM) && SUCCEEDED(hrSF) && SUCCEEDED(hrFT)) ? S_OK : SELFREG_E_CLASS;
}

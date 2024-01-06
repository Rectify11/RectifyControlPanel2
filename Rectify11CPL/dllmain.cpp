#include <olectl.h>
#include <strsafe.h>
// The GUID for the FolderViewImpl
#include "GUID.h"

// Implementation of ClassFactory
#include "ClassFactory.h"

#pragma comment(lib, "propsys.lib")
#pragma comment(lib, "shlwapi.lib")

#include "Rectify11CPL.h"
#define CONTROLPANEL_NAMESPACE_GUID L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ControlPanel\\NameSpace\\%s"
#define SHELL_EXT_APPROVED        L"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved\\%s"

HINSTANCE g_hInst = NULL;
LONG g_cRefModule = 0;

// Import from proxy.c
extern "C"
{
	HRESULT __stdcall DllGetClassObject2(const IID* rclsid, const IID* riid, void** ppv); //proxy.c
	HRESULT DllCanUnloadNow2(void); //proxy.c
}

//void* operator new(std::size_t size, const std::nothrow_t& tag) noexcept
//{
//	if (size == 0)
//	{
//		return NULL;
//	}
//
//	return HeapAlloc(GetProcessHeap(), 8, size);
//}
//
//void* operator new[](std::size_t size, const std::nothrow_t& tag) noexcept
//{
//	if (size == 0)
//	{
//		return NULL;
//	}
//
//	return HeapAlloc(GetProcessHeap(), 8, size);
//}
//
//void operator delete(void* rawMemory, std::size_t size, const std::nothrow_t& tag) noexcept
//{
//	if (rawMemory)
//	{
//		HeapFree(GetProcessHeap(), 0, rawMemory);
//	}
//}

void DllAddRef()
{
	InterlockedIncrement(&g_cRefModule);
}

void DllRelease()
{
	InterlockedDecrement(&g_cRefModule);
}

STDAPI_(BOOL) DllMain(HINSTANCE hInstance, DWORD dwReason, void* lpReserved)
{
	if (DLL_PROCESS_ATTACH == dwReason)
	{
		/*if (sizeof(XProvider) != 0x28)
		{
			MessageBox(NULL, TEXT("Fatal error: unexpected size of XProvider class"), TEXT("DllMain"), 0);
			return FALSE;
		}*/

		g_hInst = hInstance;
		DisableThreadLibraryCalls(hInstance);
	}
	return TRUE;
}

STDAPI DllCanUnloadNow(void)
{
	HRESULT proxyUnload = DllCanUnloadNow2();

	HRESULT hr = g_cRefModule ? S_FALSE : proxyUnload;

	if (hr != S_OK)
	{
		OutputDebugString(TEXT("Rectify11CPL.dll: Not unloading as DllCanUnloadNow() returned S_FALSE\n"));
	}
	return hr;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
	*ppv = NULL;

	HRESULT hr = E_OUTOFMEMORY;
	CFolderViewImplClassFactory* pClassFactory = new CFolderViewImplClassFactory(rclsid);
	if (pClassFactory)
	{
		hr = pClassFactory->QueryInterface(riid, ppv);
		pClassFactory->Release();
	}

	if (FAILED(hr))
	{
		// for some reason rclsid is IPSFactoryBuffer?? Work around this by replacing it. ugly but it works
		hr = DllGetClassObject2(&IID_IRectifyUtil, &riid, ppv);
	}
	if (FAILED(hr))
	{
		WCHAR szGuid[100] = { 0 };

		swprintf(szGuid, 100, L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\n{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
			rclsid.Data1, rclsid.Data2, rclsid.Data3, rclsid.Data4[0], rclsid.Data4[1], rclsid.Data4[2], rclsid.Data4[3], rclsid.Data4[4], rclsid.Data4[5], rclsid.Data4[6], rclsid.Data4[7],
			riid.Data1, riid.Data2, riid.Data3, riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6], riid.Data4[7]);

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
	BYTE* pszData;
	DWORD dwType;
	DWORD dwSize;
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
	WCHAR szFolderViewImplClassID[64], szElementClassID[64], szServerID[64], szAPIClass[64], szApiInterface[64], szProxy[64], szSubKey[MAX_PATH], szData[MAX_PATH];
	if (FAILED(StringFromGUID2(CLSID_FolderViewImpl, szFolderViewImplClassID, ARRAYSIZE(szFolderViewImplClassID))))
	{
		return GetLastError();
	}
	if (FAILED(StringFromGUID2(CLSID_FolderViewImplElement, szElementClassID, ARRAYSIZE(szElementClassID))))
	{
		return GetLastError();
	}
	if (FAILED(StringFromGUID2(CLSID_CRectifyUtil, szAPIClass, ARRAYSIZE(szAPIClass))))
	{
		return GetLastError();
	}
	if (FAILED(StringFromGUID2(IID_IRectifyUtil, szApiInterface, ARRAYSIZE(szApiInterface))))
	{
		return GetLastError();
	}
	if (FAILED(StringFromGUID2(CLSID_RectifyUtilServer, szServerID, ARRAYSIZE(szServerID))))
	{
		return GetLastError();
	}
	if (FAILED(StringFromGUID2(CLSID_PROXY, szProxy, ARRAYSIZE(szProxy))))
	{
		return GetLastError();
	}

	// Get the path and module name.
	WCHAR szModulePathAndName[MAX_PATH];
	GetModuleFileNameW(g_hInst, szModulePathAndName, ARRAYSIZE(szModulePathAndName));

	// This will setup and register the basic ClassIDs. 
	DWORD dwData = 0xa0000000;// SFGAO_FOLDER | SFGAO_HASSUBFOLDER | SFGAO_CANDELETE;
	DWORD one = 1;
	DWORD dwResourceId = IDR_PAGEDEF;

	BYTE AccessPermission[] = { 0x01,0x00,0x04,0x80,0x60,0x00,0x00,0x00,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x14,0x00,0x00,0x00,0x02,0x00,0x4c,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x14,0x00,0x03,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x05,0x12,0x00,0x00,0x00,0x00,0x00,0x14,0x00,0x07,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x05,0x0a,0x00,0x00,0x00,0x00,0x00,0x14,0x00,0x03,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x05,0x04,0x00,0x00,0x00,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x05,0x20,0x00,0x00,0x00,0x20,0x02,0x00,0x00,0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x05,0x20,0x00,0x00,0x00,0x20,0x02,0x00,0x00 };
	BYTE LaunchPermission[] = { 0x01,0x00,0x04,0x80,0x78,0x00,0x00,0x00,0x88,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x14,0x00,0x00,0x00,0x02,0x00,0x64,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x14,0x00,0x1f,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x05,0x12,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x1f,0x00,0x00,0x00,0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x05,0x20,0x00,0x00,0x00,0x20,0x02,0x00,0x00,0x00,0x00,0x14,0x00,0x1f,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x05,0x04,0x00,0x00,0x00,0x00,0x00,0x14,0x00,0x0b,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x05,0x12,0x00,0x00,0x00,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0xcd,0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x05,0x20,0x00,0x00,0x00,0x20,0x02,0x00,0x00,0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x05,0x20,0x00,0x00,0x00,0x20,0x02,0x00,0x00 };
	REGSTRUCT rgRegEntries[] =
	{
		HKEY_CLASSES_ROOT,  L"CLSID\\%s",                 szFolderViewImplClassID, NULL,                       (LPBYTE)L"@%s,-107",   REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s",                 szFolderViewImplClassID, L"InfoTip",                 (LPBYTE)L"@%s,-131",   REG_SZ,0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s",                 szFolderViewImplClassID, L"System.ControlPanel.Category",(LPBYTE)L"1,5",   REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s",                 szFolderViewImplClassID, L"System.Software.TasksFileUrl",(LPBYTE)L"%s,-110",   REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s",                 szFolderViewImplClassID, L"System.ApplicationName",(LPBYTE)L"Rectify11.SettingsCPL",   REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s\\InprocServer32", szFolderViewImplClassID, NULL,                       (LPBYTE)L"@%SystemRoot%\\Rectify11\\Rectify11CPL\\Rectify11CPL.dll,-10",          REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s\\InprocServer32", szFolderViewImplClassID, L"ThreadingModel",          (LPBYTE)L"Apartment",   REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s\\DefaultIcon",    szFolderViewImplClassID, NULL,                       (LPBYTE)L"%s,0",        REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s\\ShellFolder",    szFolderViewImplClassID, L"Attributes",              (LPBYTE)&dwData,        REG_DWORD, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s\\Instance", szFolderViewImplClassID,    L"CLSID",          (LPBYTE)L"{328B0346-7EAF-4BBE-A479-7CB88A095F5B}",   REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s\\Instance\\InitPropertyBag", szFolderViewImplClassID,    L"ResourceDLL",          (LPBYTE)L"%s",   REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s\\Instance\\InitPropertyBag", szFolderViewImplClassID,    L"ResourceID",          (LPBYTE)&dwResourceId,   REG_DWORD, 0,

		//element provider
		HKEY_CLASSES_ROOT,  L"CLSID\\%s",                 szElementClassID, NULL,                       (LPBYTE)L"Rectify11 Control Panel Element Provider",   REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s\\InprocServer32", szElementClassID, NULL,                       (LPBYTE)L"%s",          REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s\\InprocServer32", szElementClassID, L"ThreadingModel",          (LPBYTE)L"Apartment",   REG_SZ, 0,

		//LUA server API interface
		// 
		// NOTE: It is 100% intentonal that this interface CLSID already exists, otherwide some actxprxy thing fails...
		HKEY_CLASSES_ROOT,  L"Interface\\%s",                 szApiInterface, NULL,                       (LPBYTE)L"IRectifyUtil",   REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"Interface\\%s\\ProxyStubClsid32", szApiInterface, NULL,                     (LPBYTE)szProxy,          REG_SZ, 0,

		// CRectifyUtil
		HKEY_CLASSES_ROOT,  L"CLSID\\%s",                 szAPIClass, NULL,                       (LPBYTE)L"CRectifyUtil",   REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s\\InprocServer32", szAPIClass, NULL,                       (LPBYTE)L"%s",          REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s\\InprocServer32", szAPIClass, L"ThreadingModel",          (LPBYTE)L"Apartment",   REG_SZ, 0,

		// CRectifyUtil server
		HKEY_CLASSES_ROOT,  L"CLSID\\%s",                szServerID, NULL,                       (LPBYTE)L"Virtual Factory for Rectify11 Control Panel",   REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s",                szServerID, L"AppId",                       (LPBYTE)szServerID,   REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s",                szServerID, L"LocalizedString",                       (LPBYTE)L"@%s,-107",   REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s\\Elevation", szServerID, L"Enabled",                       (LPBYTE)&one,          REG_DWORD, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s\\Elevation", szServerID, L"IconReference",          (LPBYTE)L"%SystemRoot%\\Rectify11\\Rectify11CPL\\Rectify11CPL.dll",   REG_EXPAND_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s\\InprocServer32", szServerID, NULL,                       (LPBYTE)L"%SystemRoot%\\System32\\shpafact.dll",          REG_EXPAND_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s\\InprocServer32", szServerID, L"ThreadingModel",          (LPBYTE)L"Apartment",   REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s\\VirtualServerObjects", szServerID, szAPIClass,          (LPBYTE)L"",   REG_SZ, 0,

		// CRectifyUtil server appid
		HKEY_CLASSES_ROOT,  L"AppID\\%s",                szServerID, NULL,                       (LPBYTE)L"Virtual Factory for Rectify11 Control Panel",   REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"AppID\\%s",                szServerID, L"DllSurrogate",                       (LPBYTE)L"",   REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"AppID\\%s",                szServerID, L"AccessPermission",                       (LPBYTE)&AccessPermission,   REG_BINARY, sizeof(AccessPermission),
		HKEY_CLASSES_ROOT,  L"AppID\\%s",                szServerID, L"LaunchPermission",                       (LPBYTE)&LaunchPermission,   REG_BINARY, sizeof(LaunchPermission),

		// Proxy
		HKEY_CLASSES_ROOT,  L"CLSID\\%s",                 szProxy, NULL,                       (LPBYTE)L"IRectifyUtil_PSFactory",   REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s\\InprocServer32", szProxy, NULL,                       (LPBYTE)L"%s",          REG_SZ, 0,
		HKEY_CLASSES_ROOT,  L"CLSID\\%s\\InprocServer32", szProxy, L"ThreadingModel",          (LPBYTE)L"Both",   REG_SZ, 0,
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
				else if (REG_EXPAND_SZ == rgRegEntries[i].dwType)
				{
					RegSetValueExW(hKey,
						rgRegEntries[i].pszValueName,
						0,
						rgRegEntries[i].dwType,
						(LPBYTE)rgRegEntries[i].pszData,
						(lstrlenW((LPWSTR)rgRegEntries[i].pszData) + 1) * sizeof(WCHAR));
				}
				else if (REG_DWORD == rgRegEntries[i].dwType)
				{
					RegSetValueExW(hKey,
						rgRegEntries[i].pszValueName,
						0, rgRegEntries[i].dwType,
						rgRegEntries[i].pszData,
						sizeof(DWORD));
				}
				else if (REG_BINARY == rgRegEntries[i].dwType)
				{
					RegSetValueExW(hKey,
						rgRegEntries[i].pszValueName,
						0, rgRegEntries[i].dwType,
						rgRegEntries[i].pszData,
						rgRegEntries[i].dwSize);
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
				WCHAR LocalizedName[] = L"@C:\\Windows\\Rectify11\\Rectify11CPL\\Rectify11CPL.dll,-107";
				hr = StringCchCopyW(szData, ARRAYSIZE(szData), LocalizedName);
				if (SUCCEEDED(hr))
				{
					// Set the name of our extension.
					lResult = RegSetValueExW(hKey, NULL, 0, REG_SZ, (LPBYTE)szData, (lstrlenW(szData) + 1) * sizeof(szData[0]));
					RegCloseKey(hKey);

					PCWSTR rgszApprovedClassIDs[] = { szFolderViewImplClassID };
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
								hr = StringCchCopyW(szData, ARRAYSIZE(szData), LocalizedName);
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

	if (FAILED(hr))
	{
		//Remove the stuff we added.
		DllUnregisterServer();
	}

	return hr;
}


//
// Registry keys are removed here.
//
STDAPI DllUnregisterServer()
{
	WCHAR szSubKey[MAX_PATH], szFolderClassID[MAX_PATH], szElementID[MAX_PATH];

	//Delete the element provider
	HRESULT hrCM = StringFromGUID2(CLSID_FolderViewImplElement,
		szElementID,
		ARRAYSIZE(szElementID));
	if (SUCCEEDED(hrCM))
	{
		hrCM = StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), L"CLSID\\%s", szElementID);
		if (SUCCEEDED(hrCM))
		{
			hrCM = SHDeleteKeyW(HKEY_CLASSES_ROOT, szSubKey);
		}
	}

	// Delete server class
	hrCM = StringFromGUID2(CLSID_RectifyUtilServer,
		szElementID,
		ARRAYSIZE(szElementID));
	if (SUCCEEDED(hrCM))
	{
		hrCM = StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), L"CLSID\\%s", szElementID);
		if (SUCCEEDED(hrCM))
		{
			hrCM = SHDeleteKeyW(HKEY_CLASSES_ROOT, szSubKey);
		}
	}

	// Delete class
	hrCM = StringFromGUID2(CLSID_CRectifyUtil,
		szElementID,
		ARRAYSIZE(szElementID));
	if (SUCCEEDED(hrCM))
	{
		hrCM = StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), L"CLSID\\%s", szElementID);
		if (SUCCEEDED(hrCM))
		{
			hrCM = SHDeleteKeyW(HKEY_CLASSES_ROOT, szSubKey);
		}
	}

	// Delete AppId
	hrCM = StringFromGUID2(CLSID_RectifyUtilServer,
		szElementID,
		ARRAYSIZE(szElementID));
	if (SUCCEEDED(hrCM))
	{
		hrCM = StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), L"AppID\\%s", szElementID);
		if (SUCCEEDED(hrCM))
		{
			hrCM = SHDeleteKeyW(HKEY_CLASSES_ROOT, szSubKey);
		}
	}

	// Delete Proxy
	hrCM = StringFromGUID2(CLSID_PROXY,
		szElementID,
		ARRAYSIZE(szElementID));
	if (SUCCEEDED(hrCM))
	{
		hrCM = StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), L"CLSID\\%s", szElementID);
		if (SUCCEEDED(hrCM))
		{
			hrCM = SHDeleteKeyW(HKEY_CLASSES_ROOT, szSubKey);
		}
	}

	// Delete Interface
	hrCM = StringFromGUID2(IID_IRectifyUtil,
		szElementID,
		ARRAYSIZE(szElementID));
	if (SUCCEEDED(hrCM))
	{
		hrCM = StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), L"Interface\\%s", szElementID);
		if (SUCCEEDED(hrCM))
		{
			hrCM = SHDeleteKeyW(HKEY_CLASSES_ROOT, szSubKey);
		}
	}

	// Delete the namespace extension entries
	HRESULT hrSF = StringFromGUID2(CLSID_FolderViewImpl, szFolderClassID, ARRAYSIZE(szFolderClassID));
	if (SUCCEEDED(hrSF))
	{
		hrSF = StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), CONTROLPANEL_NAMESPACE_GUID, szFolderClassID);
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
	}

	//Delete the approved key
	HRESULT hr = StringCchPrintfW(szSubKey, ARRAYSIZE(szSubKey), SHELL_EXT_APPROVED, szFolderClassID);
	LSTATUS lResult;
	HKEY hKey;
	if (SUCCEEDED(hr))
	{
		lResult = RegCreateKeyExW(HKEY_LOCAL_MACHINE, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE,
			KEY_WRITE, NULL, &hKey, NULL);
		if (S_OK == lResult)
		{
			lResult = RegDeleteValueW(hKey, szFolderClassID);

			hr = S_OK;

			RegCloseKey(hKey);
		}
	}

	return (SUCCEEDED(hrCM) && SUCCEEDED(hrSF)) ? S_OK : SELFREG_E_CLASS;
}
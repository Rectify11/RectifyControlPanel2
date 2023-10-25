#include <windows.h>
#include <shlobj.h>
#include <propkey.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <shellapi.h>

#include "Rectify11CPL.h"
#include "ElevationManager.h"
#include "ElementProvider.h"
#include "resource.h"
#include <map>




#define NOT_IMPLEMENTED MessageBox(NULL, TEXT(__FUNCTION__), TEXT("Non implementented function in CElementProvider"), MB_ICONERROR)
#define SHOW_ERROR(x) MessageBox(NULL, TEXT(x), TEXT("Error in CElementProvider"), MB_ICONERROR)


CElementProvider::CElementProvider() : _punkSite(NULL)
{
	if (FAILED(InitProcessPriv(14, (unsigned short*)g_hInst, 1, true)))
	{
		SHOW_ERROR("Failed to initialize DirectUI\n");
	}
	if (FAILED(InitThread(2)))
	{
		SHOW_ERROR("Failed to initialize DirectUI for thread\n");
	}
	refCount = 1;
	DllAddRef();
}

CElementProvider::~CElementProvider()
{
	UnInitThread();
	UnInitProcessPriv((unsigned short*)g_hInst);

	DllRelease();
}

HRESULT CElementProvider::QueryInterface(REFIID riid, __out void** ppv)
{
	static const QITAB qit[] = {
		QITABENT(CElementProvider, IDUIElementProviderInit),
		QITABENT(CElementProvider, IFrameNotificationClient),
		QITABENT(CElementProvider, IFrameShellViewClient),
		QITABENT(CElementProvider, IObjectWithSite),
		QITABENT(CElementProvider, IServiceProvider),
		{ 0 },
	};
	HRESULT hr = QISearch(this, qit, riid, ppv);
	if (hr != S_OK)
	{
		hr = DirectUI::XProvider::QueryInterface(riid, ppv);
	}
	if (hr != S_OK)
	{
		char szGuid[40] = { 0 };

		sprintf_s(szGuid, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", riid.Data1, riid.Data2, riid.Data3, riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6], riid.Data4[7]);

		//MessageBox(NULL, szGuid, TEXT("Unknown interface in CElementProvider::QueryInterface()"), MB_ICONERROR);
	}
	return hr;
}

ULONG CElementProvider::AddRef()
{
	return DirectUI::XProvider::AddRef();
}

ULONG CElementProvider::Release()
{
	DWORD ret = refCount--;
	if (refCount == 0)
	{
		delete this;
	}

	return ret;
}

HRESULT CElementProvider::CreateDUI(DirectUI::IXElementCP* a, HWND* result_handle)
{
	int hr = XProvider::CreateDUI(a, result_handle);
	if (SUCCEEDED(hr))
	{
		DirectUI::XProvider::SetHandleEnterKey(true);
	}
	else
	{
		WCHAR buffer[200];
		if (hr == 0x800403ED)
		{
			swprintf(buffer, 200, L"Failed to create DirectUI parser: Bad markup.");
		}
		else if (hr == 0x800403EF)
		{
			swprintf(buffer, 200, L"Failed to create DirectUI parser: A required property is missing. (are you sure that resid=main exists?)");
		}
		else if (hr == 0x800403F1)
		{
			swprintf(buffer, 200, L"Failed to create DirectUI parser: Invaild property value");
		}
		else if (hr == 0x8004005A)
		{
			swprintf(buffer, 200, L"Failed to create DirectUI parser: Probaby can't find the UIFILE?");
		}
		else if (hr == 0x800403EE)
		{
			swprintf(buffer, 200, L"Failed to create DirectUI parser: Unregistered element");
		}
		else if (hr == 0x800403F0)
		{
			swprintf(buffer, 200, L"Failed to create DirectUI parser: Something is not found");
		}
		else if (hr == 0x800403F0)
		{
			swprintf(buffer, 200, L"Failed to create DirectUI parser: Something is not found");
		}
		else if (hr == E_FAIL)
		{
			swprintf(buffer, 200, L"Failed to create DirectUI parser: E_FAIL");
		}
		else
		{
			swprintf(buffer, 200, L"Failed to create DirectUI parser: Error %X", hr);
		}

		MessageBox(NULL, buffer, TEXT("CElementProvider::CreateDUI failed"), MB_ICONERROR);
	}
	return 0;
}
#pragma warning( push )
#pragma warning( disable : 4312 ) // disable warning about compiler complaining about casting ID to pointer
HRESULT STDMETHODCALLTYPE CElementProvider::SetResourceID(UINT id)
{
	IFrameShellViewClient* client = this;

	WCHAR buffer[264];
	StringCchCopyW(buffer, 260, L"main");

	//First parmeter: hinstance of module
	//2nd: Resource ID of uifile
	//3rd param: The main resid value
	int hr = DirectUI::XResourceProvider::Create(g_hInst, (UCString)id, (UCString)buffer, 0, &this->resourceProvider);
	if (SUCCEEDED(hr))
	{
		hr = DirectUI::XProvider::Initialize(NULL, (IXProviderCP*)this->resourceProvider);
		if (!SUCCEEDED(hr))
		{
			WCHAR szResource[40] = { 0 };
			swprintf(szResource, 40, L"%d", id);

			MessageBox(NULL, szResource, TEXT("CElementProvider::SetResourceId Failed to initialize xprovider"), MB_ICONERROR);
		}
	}
	else
	{
		WCHAR szResource[40] = { 0 };
		swprintf(szResource, 40, L"%d", id);

		MessageBox(NULL, szResource, TEXT("CElementProvider::SetResourceId failed to create xprovider"), MB_ICONERROR);
	}
	return hr;
}
#pragma warning( pop )

HRESULT STDMETHODCALLTYPE CElementProvider::OptionallyTakeInitialFocus(BOOL* result)
{
	*result = 0;
	return 0;
}

void CElementProvider::InitNavLinks()
{
	auto links = new CControlPanelNavLinks();

	WCHAR buffer[1024];
	if (FAILED(LoadStringW(g_hInst, IDS_UPDATE, buffer, 1023)))
	{
		wcscpy_s(buffer, L"Failed to load localized string");
	}
	links->AddLinkControlPanel(buffer, L"Rectify11.SettingsCPL", L"pageThemePref", CPNAV_Normal, NULL);
	links->AddLinkControlPanel(L"System information", L"Microsoft.System", L"", CPNAV_SeeAlso, NULL);


	GUID SID_PerLayoutPropertyBag = {};
	HRESULT hr = CLSIDFromString(L"{a46e5c25-c09c-4ca8-9a53-49cf7f865525}", (LPCLSID)&SID_PerLayoutPropertyBag);
	if (SUCCEEDED(hr))
	{
		IPropertyBag* bag = NULL;
		int hr = IUnknown_QueryService(_punkSite, SID_PerLayoutPropertyBag, IID_IPropertyBag, (LPVOID*)&bag);
		if (SUCCEEDED(hr))
		{
			if (SUCCEEDED(PSPropertyBag_WriteUnknown(bag, L"ControlPanelNavLinks", links)))
			{

			}
			else {
				MessageBox(NULL, TEXT("Failed to write property bag for navigation links"), TEXT("CElementProvider::InitNavLinks"), 0);
			}
			bag->Release();
		}
		else {
			MessageBox(NULL, TEXT("Failed to get property bag for navigation links"), TEXT("CElementProvider::InitNavLinks"), 0);
		}
	}
	else
	{
		MessageBox(NULL, TEXT("Failed to parse hardcoded GUID (SID_PerLayoutPropertyBag)"), TEXT("CElementProvider::InitNavLinks"), 0);
	}
}


void CElementProvider::InitThemeSettingPage()
{
	Element* root = GetRoot();

	TouchCheckBox* IgnoreBg = (TouchCheckBox*)root->FindDescendent(StrToID((UCString)L"IgnoreBg"));
	TouchCheckBox* IgnoreCursors = (TouchCheckBox*)root->FindDescendent(StrToID((UCString)L"IgnoreCursors"));
	TouchCheckBox* IgnoreIcons = (TouchCheckBox*)root->FindDescendent(StrToID((UCString)L"IgnoreIcons"));
	TouchCheckBox* IgnoreColors = (TouchCheckBox*)root->FindDescendent(StrToID((UCString)L"IgnoreColors"));
	TouchCheckBox* IgnoreSounds = (TouchCheckBox*)root->FindDescendent(StrToID((UCString)L"IgnoreSounds"));
	TouchCheckBox* IgnoreScreensavers = (TouchCheckBox*)root->FindDescendent(StrToID((UCString)L"IgnoreScreensavers"));

	TouchButton* SaveThemePreferences = (TouchButton*)root->FindDescendent(StrToID((UCString)L"SaveThemePreferences"));
	TouchButton* IgnoreThemePreferences = (TouchButton*)root->FindDescendent(StrToID((UCString)L"IgnoreThemePreferences"));

	HKEY Rectify11;
	if (RegCreateKey(HKEY_CURRENT_USER, TEXT("SOFTWARE\\Rectify11"), &Rectify11))
	{
		SHOW_ERROR("Failed to create rectify11 key");
		return;
	}

	DWORD size = 4;

	DWORD IgnoreBgVal = 0;
	DWORD IgnoreCursorsVal = 0;
	DWORD IgnoreIconsVal = 0;
	DWORD IgnoreColorsVal = 0;
	DWORD IgnoreSoundsVal = 0;
	DWORD IgnoreScreensaversVal = 0;

	RegQueryValueExW(Rectify11, L"IgnoreBg", 0, NULL, (LPBYTE)&IgnoreBgVal, &size);
	RegQueryValueExW(Rectify11, L"IgnoreCursors", 0, NULL, (LPBYTE)&IgnoreCursorsVal, &size);
	RegQueryValueExW(Rectify11, L"IgnoreIcons", 0, NULL, (LPBYTE)&IgnoreIconsVal, &size);
	RegQueryValueExW(Rectify11, L"IgnoreColors", 0, NULL, (LPBYTE)&IgnoreColorsVal, &size);
	RegQueryValueExW(Rectify11, L"IgnoreSounds", 0, NULL, (LPBYTE)&IgnoreSoundsVal, &size);
	RegQueryValueExW(Rectify11, L"IgnoreScreensavers", 0, NULL, (LPBYTE)&IgnoreScreensaversVal, &size);


	IgnoreBg->SetToggleOnClick(true);
	IgnoreCursors->SetToggleOnClick(true);
	IgnoreIcons->SetToggleOnClick(true);
	IgnoreColors->SetToggleOnClick(true);
	IgnoreSounds->SetToggleOnClick(true);
	IgnoreScreensavers->SetToggleOnClick(true);

	IgnoreBg->SetCheckedState(IgnoreBgVal ? CheckedStateFlags_CHECKED : CheckedStateFlags_NONE);
	IgnoreCursors->SetCheckedState(IgnoreCursorsVal ? CheckedStateFlags_CHECKED : CheckedStateFlags_NONE);
	IgnoreIcons->SetCheckedState(IgnoreIconsVal ? CheckedStateFlags_CHECKED : CheckedStateFlags_NONE);
	IgnoreColors->SetCheckedState(IgnoreColorsVal ? CheckedStateFlags_CHECKED : CheckedStateFlags_NONE);
	IgnoreSounds->SetCheckedState(IgnoreSoundsVal ? CheckedStateFlags_CHECKED : CheckedStateFlags_NONE);
	IgnoreScreensavers->SetCheckedState(IgnoreScreensaversVal ? CheckedStateFlags_CHECKED : CheckedStateFlags_NONE);

	//// register buttons
	//static EventListener SaveThemePreferences_listener(SaveThemePreferences_OnEvent);

	//if (SaveThemePreferences->AddListener(&SaveThemePreferences_listener) != S_OK)
	//{
	//	MessageBox(NULL, TEXT("Failed to add listener for radio button"), TEXT("CElementProvider::LayoutInitialized"), MB_ICONERROR);
	//}

	//static EventListener IgnoreThemePreferences_listener(IgnoreThemePreferences_OnEvent);

	//if (IgnoreThemePreferences->AddListener(&IgnoreThemePreferences_listener) != S_OK)
	//{
	//	MessageBox(NULL, TEXT("Failed to add listener for radio button"), TEXT("CElementProvider::LayoutInitialized"), MB_ICONERROR);
	//}
}

HRESULT STDMETHODCALLTYPE CElementProvider::LayoutInitialized()
{
	InitNavLinks();
	HRESULT hr = themetool_init();
	if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED))
	{
		MessageBox(NULL, TEXT("Failed to initialize SecureUXTheme ThemeTool. Theme information will not be loaded."), TEXT("CElementProvider::LayoutInitialized"), MB_ICONERROR);
	}
	Element* root = XProvider::GetRoot();
	if (root->FindDescendent(StrToID((UCString)L"MainPageElem")) != NULL)
	{
		CRectifyMainCPLPage* page = (CRectifyMainCPLPage*)root->FindDescendent(StrToID((UCString)L"MainPageElem"));
		page->OnInit();
	}

	return S_OK;
}
HRESULT STDMETHODCALLTYPE CElementProvider::Notify(WORD* param)
{
	//the param is text

	if (!StrCmpCW((LPCWSTR)param, L"SettingsChanged"))
	{
		//This is invoked when the UI is refreshed!
	}

	if (!StrCmpCW((LPCWSTR)param, L"SearchText"))
	{
		//Sent when search text modified/added
		WCHAR value[264] = { 0 };
		WCHAR path[48];
		GUID IID_IFrameManager = {};
		GUID SID_STopLevelBrowser = {};
		HRESULT hr = CLSIDFromString(L"{4c96be40-915c-11cf-99d3-00aa004ae837}", (LPCLSID)&SID_STopLevelBrowser);
		if (SUCCEEDED(hr))
		{
			hr = CLSIDFromString(L"{31e4fa78-02b4-419f-9430-7b7585237c77}", (LPCLSID)&IID_IFrameManager);
			if (SUCCEEDED(hr))
			{
				IPropertyBag* bag = NULL;
				HRESULT hr = IUnknown_QueryService(_punkSite, IID_IFrameManager, IID_IPropertyBag, (LPVOID*)&bag);
				if (SUCCEEDED(hr))
				{
					if (SUCCEEDED(PSPropertyBag_ReadStr(bag, L"SearchText", value, 260)) && value[0])
					{
						if (SUCCEEDED(StringCchPrintfW(path, 41, L"::%s", L"{26ee0668-a00a-44d7-9371-beb064c98683}")))
						{
							LPITEMIDLIST pidlist;
							if (SUCCEEDED(SHParseDisplayName(path, NULL, &pidlist, 0, NULL)))
							{
								IShellBrowser* browser = NULL;
								if (SUCCEEDED(IUnknown_QueryService(_punkSite, SID_STopLevelBrowser, IID_IShellBrowser, (LPVOID*)&browser)))
								{
									hr = browser->BrowseObject(pidlist, SBSP_ACTIVATE_NOFOCUS | SBSP_SAMEBROWSER | SBSP_CREATENOHISTORY);
									browser->Release();
								}
							}
							ILFree(pidlist);
						}
					}
				}
			}
		}
	}
	return 0;
}
HRESULT STDMETHODCALLTYPE CElementProvider::OnNavigateAway() {
	//TODO: this causes a crash
	//DirectUI::XProvider::SetHandleEnterKey(false);
	//SetDefaultButtonTracking(false);
	return 0;
}
HRESULT STDMETHODCALLTYPE CElementProvider::OnInnerElementDestroyed()
{
	return 0;
}

HRESULT STDMETHODCALLTYPE CElementProvider::OnSelectedItemChanged()
{
	return 0;
}
HRESULT STDMETHODCALLTYPE CElementProvider::OnSelectionChanged()
{
	return 0;
}
HRESULT STDMETHODCALLTYPE CElementProvider::OnContentsChanged()
{
	return 0;
}
HRESULT STDMETHODCALLTYPE CElementProvider::OnFolderChanged()
{
	return 0;
}

HRESULT STDMETHODCALLTYPE CElementProvider::QueryService(
	REFGUID guidService,
	REFIID riid,
	void** ppvObject)
{
	*ppvObject = 0;
	NOT_IMPLEMENTED;
	return E_NOTIMPL;
}

HRESULT CElementProvider::SetSite(IUnknown* punkSite)
{
	if (punkSite != NULL)
	{
		IUnknown_Set((IUnknown**)&this->Site, punkSite);
		IUnknown_Set((IUnknown**)&this->_punkSite, punkSite);
	}

	return S_OK;
}

HRESULT CElementProvider::GetSite(REFIID riid, void** ppvSite)
{
	if (Site == NULL)
	{
		return E_FAIL;
	}

	return Site->QueryInterface(riid, ppvSite);
}

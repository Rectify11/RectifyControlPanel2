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
#include "RectifyMainPage.h"
#include "RectifyThemeCfgPage.h"
#include <map>

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

HRESULT STDMETHODCALLTYPE CElementProvider::LayoutInitialized()
{
	HRESULT hr = themetool_init();
	if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED))
	{
		MessageBox(NULL, TEXT("Failed to initialize SecureUXTheme ThemeTool. Theme information will not be loaded."), TEXT("CElementProvider::LayoutInitialized"), MB_ICONERROR);
	}

	Element* root = XProvider::GetRoot();

	if (root->FindDescendent(StrToID((UCString)L"MainPageElem")) != NULL)
	{
		RectifyMainPage* page = (RectifyMainPage*)root->FindDescendent(StrToID((UCString)L"MainPageElem"));
		page->SetSite(_punkSite);
		page->OnInit();
	}
	else if (root->FindDescendent(StrToID((UCString)L"ThemePageElem")) != NULL)
	{
		RectifyThemeCfgPage* page = (RectifyThemeCfgPage*)root->FindDescendent(StrToID((UCString)L"ThemePageElem"));
		page->SetSite(_punkSite);
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

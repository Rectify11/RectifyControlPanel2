// This class is created by Shell32 CLayoutFolder class. This class "provides" the Element for DirectUI to render in explorer.

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
#include <map>

CElementProvider::CElementProvider() : Site(NULL)
{
	if (FAILED(InitProcessPriv(DUI_VERSION_WIN10_2022, g_hInst, 1, true)))
	{
		SHOW_ERROR("Failed to initialize DirectUI\n");
	}
	if (FAILED(InitThread(2)))
	{
		SHOW_ERROR("Failed to initialize DirectUI for thread\n");
	}

	// Prevent unloading DLL when this class has not been properly destroyed
	DllAddRef();
}

CElementProvider::~CElementProvider()
{
	UnInitThread();
	UnInitProcessPriv((unsigned short*)g_hInst);

	DllRelease();
}

// IUnknown Implementation
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
	return hr;
}

ULONG CElementProvider::AddRef()
{
	return DirectUI::XProvider::AddRef();
}

ULONG CElementProvider::Release()
{
	ULONG ret = refCount--;
	ULONG newRefCount = refCount;
	if (ret == 1)
	{
		delete this;
	}

	return newRefCount;
}

// XProvider implementation
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
			if (this->XProviderCP)
			{
				swprintf(buffer, 200, L"Failed to create DirectUI parser: E_FAIL. This is most likely caused by duires.dll missing in System32. Rerun the latest version of the Rectify11 installer.");
			}
			else
			{
				swprintf(buffer, 200, L"Failed to create DirectUI parser: E_FAIL as XProviderCP is 0.");
			}
		}
		else
		{
			swprintf(buffer, 200, L"Failed to create DirectUI parser: Error %X", hr);
		}

		MessageBox(NULL, buffer, TEXT("CElementProvider::CreateDUI failed"), MB_ICONERROR);
	}
	return 0;
}

// IDUIElementProviderInit implementation
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
	int hr = DirectUI::XResourceProvider::Create(g_hInst, (const wchar_t*)id, (const wchar_t*)buffer, 0, (XResourceProvider**)&this->XProviderCP);
	if (SUCCEEDED(hr))
	{
		hr = DirectUI::XProvider::Initialize(NULL, (IXProviderCP*)this->XProviderCP);
		if (!SUCCEEDED(hr))
		{
			WCHAR szResource[40] = { 0 };
			swprintf(szResource, 40, L"%d", id);

			MessageBox(NULL, szResource, TEXT("CElementProvider::SetResourceId - failed to initialize XProvider class"), MB_ICONERROR);
		}
	}
	else
	{
		WCHAR szResource[40] = { 0 };
		swprintf(szResource, 40, L"%d", id);

		MessageBox(NULL, szResource, TEXT("CElementProvider::SetResourceId - failed to create xprovider"), MB_ICONERROR);
	}
	return hr;
}
#pragma warning( pop )

HRESULT STDMETHODCALLTYPE CElementProvider::OptionallyTakeInitialFocus(BOOL* result)
{
	*result = FALSE;
	return S_OK;
}

// IFrameNotificationClient implementation
HRESULT STDMETHODCALLTYPE CElementProvider::LayoutInitialized()
{
	HRESULT hr = themetool_init();
	if (hr != S_OK && hr != HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED))
	{
		MessageBox(NULL, TEXT("Failed to initialize SecureUXTheme ThemeTool. Theme information will not be loaded. This may be due to the lack of the ThemeDll.dll in C:\\Windows\\Rectify11\\RectifyControlPanel"), TEXT("CElementProvider::LayoutInitialized"), MB_ICONERROR);
	}

	Element* root = XProvider::GetRoot();

	// Call initialization function as DirectUI does not provide an "OnLoad" method, unlike WPF/Winforms
	// This is a bit hacky as Microsoft treats elements as COM objects, and instead iterates through each element, calling
	// the appropriate method.
	if (root->FindDescendent(StrToID((const wchar_t*)L"MainPageElem")) != NULL)
	{
		RectifyMainPage* page = (RectifyMainPage*)root->FindDescendent(StrToID((const wchar_t*)L"MainPageElem"));
		page->SetSite(Site);
		page->OnInit();
	}

	return S_OK;
}
HRESULT STDMETHODCALLTYPE CElementProvider::Notify(WORD* param)
{
	// NOTE: param is LPCWSTR

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
				// find explorer property bag
				IPropertyBag* bag = NULL;
				HRESULT hr = IUnknown_QueryService(Site, IID_IFrameManager, IID_IPropertyBag, (LPVOID*)&bag);
				if (SUCCEEDED(hr))
				{
					// read search text
					if (SUCCEEDED(PSPropertyBag_ReadStr(bag, L"SearchText", value, 260)) && value[0])
					{
						if (SUCCEEDED(StringCchPrintfW(path, 41, L"::%s", L"{26ee0668-a00a-44d7-9371-beb064c98683}")))
						{
							LPITEMIDLIST pidlist;
							if (SUCCEEDED(SHParseDisplayName(path, NULL, &pidlist, 0, NULL)))
							{
								IShellBrowser* browser = NULL;
								if (SUCCEEDED(IUnknown_QueryService(Site, SID_STopLevelBrowser, IID_IShellBrowser, (LPVOID*)&browser)))
								{
									// navigate to search text
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

HRESULT STDMETHODCALLTYPE CElementProvider::OnNavigateAway()
{
	SetHandleEnterKey(FALSE);
	SetDefaultButtonTracking(FALSE);

	// for whatever reason, OnNavigateAway is called twice. Instead of someone
	// at microsoft properly fixing the issue, they just null out some fields in XProvider
	// to prevent destroying a random memory address.

	XProviderCP = NULL;
	_RandomElement = NULL;
	_RootElement = NULL;

	return 0;
}
HRESULT STDMETHODCALLTYPE CElementProvider::OnInnerElementDestroyed()
{
	//if (XProviderCP)
	//{
		XProviderCP = NULL;
		_RootElement = NULL;
		_RandomElement = NULL;
	//}
	return 0;
}
// IFrameShellViewClient implementation
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

// IServiceProvider implementation
HRESULT STDMETHODCALLTYPE CElementProvider::QueryService(
	REFGUID guidService,
	REFIID riid,
	void** ppvObject)
{
	*ppvObject = 0;
	return E_NOTIMPL;
}

// IObjectWithSite implementation
HRESULT CElementProvider::SetSite(IUnknown* punkSite)
{
	IUnknown_Set((IUnknown**)&this->Site, punkSite);
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

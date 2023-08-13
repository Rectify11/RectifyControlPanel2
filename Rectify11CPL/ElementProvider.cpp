#include <windows.h>
#include <shlobj.h>
#include <propkey.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <shellapi.h>

#include "Rectify11CPL.h"
#include "ElementProvider.h"
#include "resource.h"
#include "pch.h"

//3072    <<not bound>>
typedef HRESULT(*Dui70_XProvider_QueryInterface)(REFIID riid, __out void** ppv);
Dui70_XProvider_QueryInterface Dui70_XProvider_QueryInterface_Func;


#define NOT_IMPLEMENTED MessageBox(NULL, TEXT(__FUNCTION__), TEXT("Non implementented function in CElementProvider"), 0)
#define SHOW_ERROR(x) MessageBox(NULL, TEXT(x), TEXT("Error in CElementProvider"), 0)


CElementProvider::CElementProvider() : _punkSite(NULL)
{
	if (FAILED(InitProcessPriv(14, NULL, 1, true)))
	{
		SHOW_ERROR("Failed to initialize DirectUI\n");
	}
	if (FAILED(InitThread(2)))
	{
		SHOW_ERROR("Failed to initialize DirectUI for thread\n");
	}
	DllAddRef();
}

CElementProvider::~CElementProvider()
{
	UnInitThread();
	UnInitProcessPriv((unsigned short*)0x180000000);

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

		sprintf(szGuid, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", riid.Data1, riid.Data2, riid.Data3, riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6], riid.Data4[7]);

		//MessageBox(NULL, szGuid, TEXT("Unknown interface in CElementProvider::QueryInterface()"), 0);
	}
	return hr;
}

ULONG CElementProvider::AddRef()
{
	return DirectUI::XProvider::AddRef();
}

ULONG CElementProvider::Release()
{
	DWORD ret = refCount;
	ret--;
	if (ret == 0)
	{
		delete this;
	}
	else
	{
		refCount = ret;
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
		CHAR buffer[2000];
		if (hr == 0x800403EF)
		{
			sprintf(buffer, "Failed to create DirectUI parser: A required property is missing.");
		}
		else if (hr == 0x8004005A)
		{
			sprintf(buffer, "Failed to create DirectUI parser: Probaby can't find the UIFILE?");
		}
		else
		{
			sprintf(buffer, "Failed to create DirectUI parser: Error %X", hr);
		}
	}
	return 0;
}

HRESULT STDMETHODCALLTYPE CElementProvider::SetResourceID(UINT id)
{
	WCHAR szGuid[40] = { 0 };

	swprintf(szGuid, L"%d", id);
	IFrameShellViewClient* client = this;

	WCHAR buffer[264];
	StringCchCopyW(buffer, 260, L"main");

	//First parmeter: hinstance of module
	//2nd: Resource ID of uifile
	//3rd param: The main resid value
	int hr = DirectUI::XResourceProvider::Create(g_hInst, (unsigned short const*)id, (unsigned short const*)buffer, 0, &this->resourceProvider);
	if (SUCCEEDED(hr))
	{
		hr = DirectUI::XProvider::Initialize(NULL, (IXProviderCP*)this->resourceProvider);
		if (!SUCCEEDED(hr))
		{
			MessageBox(NULL, szGuid, TEXT("CElementProvider::SetResourceId Failed to initialize xprovider"), 0);
		}
	}
	else
	{
		MessageBox(NULL, szGuid, TEXT("CElementProvider::SetResourceId failed to create xprovider"), 0);
	}
	return hr;
}

HRESULT STDMETHODCALLTYPE CElementProvider::OptionallyTakeInitialFocus(BOOL* result)
{
	*result = 0;
	Element* root = DirectUI::XProvider::GetRoot();
	if (root != NULL)
	{
		//root->GetClassInfoPtr();
		//TODO
	}
	return 0;
}
class InputListener : public IElementListener {
public:
	using handler_t = std::function<void(Element*, InputEvent*)>;

	handler_t f;

	InputListener(handler_t f) : f(f) { }

	void OnListenerAttach(Element* elem) override { }
	void OnListenerDetach(Element* elem) override { }
	bool OnPropertyChanging(Element* elem, const PropertyInfo* prop, int unk, Value* v1, Value* v2) override {
		return true;
	}
	void OnListenedPropertyChanged(Element* elem, const PropertyInfo* prop, int type, Value* v1, Value* v2) override { }
	void OnListenedEvent(Element* elem, struct Event* ev) override { }
	void OnListenedInput(Element* elem, struct InputEvent* iev) override {
		f(elem, iev);
	}
};

void CElementProvider::InitNavLinks()
{
	auto links = new CControlPanelNavLinks();

	//load uac icon
	SHSTOCKICONINFO icon = {};
	icon.cbSize = sizeof(SHSTOCKICONINFO);
	SHGetStockIconInfo(SIID_SHIELD, 0x101, &icon);

	links->AddLinkControlPanel(L"Update Rectify11", L"Rectify11.SettingsCPL", L"pageRectifyUpdate", CPNAV_Normal, icon.hIcon);
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

void CElementProvider::InitMainPage()
{
	Element* root = XProvider::GetRoot();
	ThemeCombo = (Combobox*)root->FindDescendent(StrToID((UCString)L"RThemeCmb"));

	static vector<wstring> themes;
	if (ThemeCombo != NULL)
	{
		static InputListener accept_listener([&](Element* elem, InputEvent* iev) {
			if (iev->event_id == *Combobox::SelectionChange().pId)
			{
				//MessageBox(NULL, TEXT("Combobox selection changed!"), TEXT("CElementProvider::LayoutInitialized"), 0);
				int selection = ((Combobox*)iev->target)->GetSelection();
				themetool_set_active(NULL, selection, TRUE, 0, 0);
			}
			});
		if (ThemeCombo->AddListener(&accept_listener) != S_OK)
		{
			MessageBox(NULL, TEXT("Failed to add"), TEXT("CElementProvider::LayoutInitialized"), 0);
		}

		ULONG themeCount = 0;
		if (SUCCEEDED(themetool_get_theme_count(&themeCount)))
		{
			for (ULONG i = 0; i < themeCount; i++)
			{
				ITheme* theme = NULL;
				if (SUCCEEDED(themetool_get_theme(i, &theme)))
				{
					std::wstring nameBuffer = std::wstring(1024, '\0');
					theme->GetDisplayName(nameBuffer);
					ThemeCombo->AddString((UString)nameBuffer.c_str());
				}
			}
			int theme = -1;
			themetool_get_manager()->GetCurrentTheme(&theme);
			ThemeCombo->SetSelection(theme);
		}
		else {
			MessageBox(NULL, TEXT("Failed to count the amount of themes"), TEXT("CElementProvider::LayoutInitialized"), 0);
		}
	}



	//helpHub
	Button* HelpButton = (Button*)root->FindDescendent(StrToID((UCString)L"helpHub"));
	if (HelpButton != NULL)
	{
		static InputListener help_listener([&](Element* elem, InputEvent* iev) {
			if (iev->event_id == *Button::Click().pId)
			{
				ShellExecute(0, 0, TEXT("http://rectify11.net"), 0, 0, SW_SHOW);
			}
			});
		if (HelpButton->AddListener(&help_listener) != S_OK)
		{
			MessageBox(NULL, TEXT("Failed to add helpbutton licenser"), TEXT("CElementProvider::LayoutInitialized"), 0);
		}
	}
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
	if (root->FindDescendent(StrToID((UCString)L"RThemeCmb")) != NULL)
	{
		InitMainPage();
	}

	
	return 0;
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
		WCHAR value[264] = {0};
		WCHAR path[48];
		GUID IID_IFrameManager = {};
		GUID SID_STopLevelBrowser = {};
		CLSIDFromString(L"{4c96be40-915c-11cf-99d3-00aa004ae837}", (LPCLSID)&SID_STopLevelBrowser);

		HRESULT hr = CLSIDFromString(L"{31e4fa78-02b4-419f-9430-7b7585237c77}", (LPCLSID)&IID_IFrameManager);
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
							IShellBrowser* browser;
							if (SUCCEEDED(IUnknown_QueryService(_punkSite, SID_STopLevelBrowser, IID_IShellBrowser, (LPVOID*)&browser)))
							{
								hr = browser->BrowseObject(pidlist, 1572865);
								browser->Release();
							}
						}
						ILFree(pidlist);
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
HRESULT STDMETHODCALLTYPE CElementProvider::OnInnerElementDestroyed() {
	return 0;
}

HRESULT STDMETHODCALLTYPE CElementProvider::OnSelectedItemChanged()
{
	NOT_IMPLEMENTED;
	return 0;
}
HRESULT STDMETHODCALLTYPE CElementProvider::OnSelectionChanged()
{
	//NOT_IMPLEMENTED;
	return 0;
}
HRESULT STDMETHODCALLTYPE CElementProvider::OnContentsChanged()
{
	//NOT_IMPLEMENTED;
	return 0;
}
HRESULT STDMETHODCALLTYPE CElementProvider::OnFolderChanged()
{
	//NOT_IMPLEMENTED;
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
	NOT_IMPLEMENTED;

	if (Site == NULL)
	{
		return E_FAIL;
	}

	return Site->QueryInterface(riid, ppvSite);
}

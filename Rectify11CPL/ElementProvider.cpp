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

static vector<ULONG> themes;


#define NOT_IMPLEMENTED MessageBox(NULL, TEXT(__FUNCTION__), TEXT("Non implementented function in CElementProvider"), MB_ICONERROR)
#define SHOW_ERROR(x) MessageBox(NULL, TEXT(x), TEXT("Error in CElementProvider"), MB_ICONERROR)


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
		WCHAR buffer[200];
		if (hr == 0x800403EF)
		{
			swprintf(buffer, 200, L"Failed to create DirectUI parser: A required property is missing. (are you sure that resid=main exists?)");
		}
		else if (hr == 0x8004005A)
		{
			swprintf(buffer, 200, L"Failed to create DirectUI parser: Probaby can't find the UIFILE?");
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
	int hr = DirectUI::XResourceProvider::Create(g_hInst, (unsigned short const*)id, (unsigned short const*)buffer, 0, &this->resourceProvider);
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
	Element* root = DirectUI::XProvider::GetRoot();
	if (root != NULL)
	{
		//root->GetClassInfoPtr();
		//TODO
	}
	return 0;
}
class EventListener : public IElementListener {

	using handler_t = std::function<void(Element*, Event*)>;

	handler_t f;
public:
	EventListener(handler_t f) : f(f) { }

	void OnListenerAttach(Element* elem) override { }
	void OnListenerDetach(Element* elem) override { }
	bool OnPropertyChanging(Element* elem, const PropertyInfo* prop, int unk, Value* v1, Value* v2) override {
		return true;
	}
	void OnListenedPropertyChanged(Element* elem, const PropertyInfo* prop, int type, Value* v1, Value* v2) override { }
	void OnListenedEvent(Element* elem, struct Event* iev) override {
		f(elem, iev);
	}
	void OnListenedInput(Element* elem, struct InputEvent* ev) override { }
};

void CElementProvider::InitNavLinks()
{
	auto links = new CControlPanelNavLinks();

	//load uac icon
	SHSTOCKICONINFO icon = {};
	icon.cbSize = sizeof(SHSTOCKICONINFO);
	SHGetStockIconInfo(SIID_SHIELD, 0x101, &icon);

	WCHAR buffer[1024];
	if (FAILED(LoadStringW(g_hInst, IDS_UPDATE, buffer, 1023)))
	{
		wcscpy_s(buffer, L"Failed to load localized string");
	}

	//links->AddLinkControlPanel(buffer, L"Rectify11.SettingsCPL", L"pageRectifyUpdate", CPNAV_Normal, icon.hIcon);
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


void MicaChk_OnEvent(Element* elem, Event* iev)
{
	TouchCheckBox* MicaForEveryoneCheckbox = (TouchCheckBox*)elem;
	TouchCheckBox* TabbedCheckbox = (TouchCheckBox*)elem->GetRoot()->FindDescendent(StrToID((UCString)L"TabChk"));

	if (iev->type == TouchButton::Click)
	{
		CheckedStateFlags MicaEnabled2 = MicaForEveryoneCheckbox->GetCheckedState();
		CRectifyUtil::SetMicaForEveryoneEnabled(MicaEnabled2);

		// Enable/disable the tabbed checkbox
		if (TabbedCheckbox != NULL)
			TabbedCheckbox->SetEnabled(MicaEnabled2);
	}
}

void TabChk_OnEvent(Element* elem, Event* iev)
{
	TouchCheckBox* TabbedCheckbox = (TouchCheckBox*)elem;

	if (iev->type == TouchButton::Click)
	{
		CRectifyUtil::SetTabbedEnabled(TabbedCheckbox->GetSelected());
	}
}

void HelpButton_OnEvent(Element* elem, Event* iev)
{
	if (iev->type == Button::Click)
	{
		ShellExecute(0, 0, TEXT("http://rectify11.net"), 0, 0, SW_SHOW);
	}
}

void ThemeCmb_OnEvent(Element* elem, Event* iev)
{
	if (iev->type == Combobox::SelectionChange)
	{
		int selection = ((Combobox*)iev->target)->GetSelection();
		themetool_set_active(NULL, themes[selection], TRUE, 0, 0);
		CElementProvider::UpdateThemeGraphic(elem->GetRoot());
	}
}

void CElementProvider::InitMainPage()
{
	Element* root = XProvider::GetRoot();
	ThemeCombo = (Combobox*)root->FindDescendent(StrToID((UCString)L"ThemeCmb"));
	Button* HelpButton = (Button*)root->FindDescendent(StrToID((UCString)L"buttonHelp"));
	TouchCheckBox* MicaForEveryoneCheckbox = (TouchCheckBox*)root->FindDescendent(StrToID((UCString)L"MicaChk"));
	TouchCheckBox* TabbedCheckbox = (TouchCheckBox*)root->FindDescendent(StrToID((UCString)L"TabChk"));
	Element* version = (Element*)root->FindDescendent(StrToID((UCString)L"RectifyVersion"));

	if (ThemeCombo != NULL)
	{
		WCHAR value[255] = {0};
		PVOID pvData = value;
		DWORD size = sizeof(value);
		RegGetValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\ThemeManager", L"DllName", RRF_RT_REG_SZ, 0, pvData, &size);
		std::wstring msstylePath = std::wstring((LPCWSTR)pvData);
		int k = 0;
		ULONG themeCount = 0;
		if (SUCCEEDED(themetool_get_theme_count(&themeCount)))
		{
			for (ULONG i = 0; i < themeCount; i++)
			{
				ITheme* theme = NULL;
				if (SUCCEEDED(themetool_get_theme(i, &theme)))
				{
					std::wstring nameBuffer = std::wstring(255, '\0');
					theme->GetDisplayName(nameBuffer);

					if (nameBuffer.starts_with(L"Rectify11"))
					{
						ThemeCombo->AddString((UString)nameBuffer.c_str());
						std::wstring pathBuff = std::wstring();
						theme->GetVisualStyle(pathBuff);
						std::wstring msstylePath = std::wstring((LPCWSTR)pvData);
						if (pathBuff == msstylePath)
						{
							ThemeCombo->SetSelection(k);
						}
						themes.push_back(i);
						k++;
					}
				}
				themetool_theme_release(theme);
			}
		}
		else {
			MessageBox(NULL, TEXT("Failed to count the amount of themes"), TEXT("CElementProvider::LayoutInitialized"), MB_ICONERROR);
		}
		if (version != NULL)
		{
			WCHAR value[255] = { 0 };
			PVOID pvData = value;
			DWORD size = sizeof(value);
			RegGetValue(HKEY_LOCAL_MACHINE, L"Software\\Rectify11", L"Version", RRF_RT_REG_SZ, 0, pvData, &size);
			std::wstring vstr = std::wstring(L"Rectify11 version: ");
			vstr += (LPCWSTR)pvData;
			version->SetContentString((UCString)vstr.c_str());
		}
		static EventListener accept_listener(ThemeCmb_OnEvent);

		if (ThemeCombo->AddListener(&accept_listener) != S_OK)
		{
			MessageBox(NULL, TEXT("Failed to add"), TEXT("CElementProvider::LayoutInitialized"), MB_ICONERROR);
		}
	}

	//helpHub
	if (HelpButton != NULL)
	{
		static EventListener help_listener(HelpButton_OnEvent);
		if (HelpButton->AddListener(&help_listener) != S_OK)
		{
			MessageBox(NULL, TEXT("Failed to add helpbutton licenser"), TEXT("CElementProvider::LayoutInitialized"), MB_ICONERROR);
		}
	}

	if (MicaForEveryoneCheckbox != NULL)
	{
		MicaForEveryoneCheckbox->SetToggleOnClick(true);
		bool MicaEnabled = CRectifyUtil::CheckIfMicaForEveryoneIsEnabled();
		MicaForEveryoneCheckbox->SetCheckedState(MicaEnabled ? CheckedStateFlags_CHECKED : CheckedStateFlags_NONE);

		if (!MicaEnabled && TabbedCheckbox != NULL)
		{
			TabbedCheckbox->SetEnabled(FALSE);
		}

		static EventListener mica_listener(MicaChk_OnEvent);
		if (MicaForEveryoneCheckbox->AddListener(&mica_listener) != S_OK)
		{
			MessageBox(NULL, TEXT("Failed to add mica checkbox licenser"), TEXT("CElementProvider::LayoutInitialized"), MB_ICONERROR);
		}
	}

	if (TabbedCheckbox != NULL)
	{
		TabbedCheckbox->SetToggleOnClick(true);
		TabbedCheckbox->SetCheckedState(CRectifyUtil::GetTabbedEnabled() ? CheckedStateFlags_CHECKED : CheckedStateFlags_NONE);

		static EventListener tab_listener(TabChk_OnEvent);
		if (TabbedCheckbox->AddListener(&tab_listener) != S_OK)
		{
			MessageBox(NULL, TEXT("Failed to add mica tab checkbox licenser"), TEXT("CElementProvider::LayoutInitialized"), MB_ICONERROR);
		}
	}

	UpdateThemeGraphic(root);
}

void CElementProvider::UpdateThemeGraphic(Element* root)
{
	LPCWSTR id = CRectifyUtil::IsDarkTheme() ? MAKEINTRESOURCE(IDB_DARKPREVIEW) : MAKEINTRESOURCE(IDB_LIGHTPREVIEW);
	HBITMAP bmp = (HBITMAP)LoadImage(g_hInst, id, IMAGE_BITMAP, 256, 256, 0);
	if (bmp == NULL)
	{
		SHOW_ERROR("failure to load bitmap");
		return;
	}
	Value* bitmap = DirectUI::Value::CreateGraphic(bmp, 3, 0xffffffff, false, false, false);
	Element* PreviewElement = root->FindDescendent(StrToID((UCString)L"ThemePreview"));
	if (PreviewElement != NULL)
		PreviewElement->SetValue(Element::ContentProp, 1, bitmap);
	bitmap->Release();
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
	if (root->FindDescendent(StrToID((UCString)L"ThemeCmb")) != NULL)
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
		UpdateThemeGraphic(GetRoot());
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

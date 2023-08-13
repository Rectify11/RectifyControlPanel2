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
	links->AddLinkControlPanel(L"Update Rectify11", L"Rectify11.SettingsCPL", L"pageRectifyUpdate", CPNAV_Normal, (HICON)LoadImageW(g_hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON,32,32,0));
	//links->AddLinkControlPanel(L"Power options", L"Microsoft.PowerOptions", L"", CPNAV_SeeAlso, SIID_DOCNOASSOC);
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

HRESULT STDMETHODCALLTYPE CElementProvider::LayoutInitialized()
{
	Element* root = XProvider::GetRoot();

	InitNavLinks();
	Combobox* ThemeCombo = (Combobox*)root->FindDescendent(StrToID((UCString)L"RThemeCmb"));
	if (ThemeCombo != NULL)
	{
		static InputListener accept_listener([&](Element* elem, InputEvent* iev) {
			if (iev->event_id == *Combobox::SelectionChange().pId)
			{
				//MessageBox(NULL, TEXT("Combobox selection changed!"), TEXT("CElementProvider::LayoutInitialized"), 0);
			}
			});
		if (ThemeCombo->AddListener(&accept_listener) != S_OK)
		{
			MessageBox(NULL, TEXT("Failed to add"), TEXT("CElementProvider::LayoutInitialized"), 0);
		}
		ThemeCombo->AddString((UCString)L"Test");
		ThemeCombo->SetSelection(0);
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
	return 0;
}
HRESULT STDMETHODCALLTYPE CElementProvider::Notify(WORD* param)
{
	//the param is text

	WCHAR buffer[2000];
	swprintf(buffer, 1990, L"Notification: %s", param);
	//MessageBoxW(NULL, buffer, L"Notification", 0);

	if (!StrCmpCW((LPCWSTR)param, L"SettingsChanged"))
	{
		//This is invoked when the UI is refreshed!
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

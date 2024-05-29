#include "Rectify11CPL.h"
#include "RectifyThemeCfgPage.h"
#include "CRectifyUtil.h"

IClassInfo* RectifyThemeCfgPage::Class = NULL;


RectifyThemeCfgPage::RectifyThemeCfgPage()
{

}

RectifyThemeCfgPage::~RectifyThemeCfgPage()
{

}

HRESULT RectifyThemeCfgPage::CreateInstance(Element* rootElement, unsigned long* debug_variable, Element** newElement)
{
	int hr = E_OUTOFMEMORY;

	RectifyThemeCfgPage* instance = (RectifyThemeCfgPage*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(RectifyThemeCfgPage));

	if (instance != NULL)
	{
		new (instance) RectifyThemeCfgPage();
		hr = instance->Initialize(0, rootElement, debug_variable);
		if (SUCCEEDED(hr))
		{
			*newElement = instance;
		}
		else
		{
			if (instance != NULL)
			{
				instance->Destroy(TRUE);
				instance = NULL;
			}
		}
	}

	return hr;
}

IClassInfo* RectifyThemeCfgPage::GetClassInfoW()
{
	return RectifyThemeCfgPage::Class;
}

void RectifyThemeCfgPage::OnEvent(Event* iev)
{
	if (iev->flag != GMF_ROUTED)
		return;
	if (!iev->handled)
		Element::OnEvent(iev);
	if (initializing) return;

	if (iev->type == TouchButton::Click)
	{
		if (iev->target->GetID() == StrToID((UCString)L"SaveThemePreferences"))
		{
			TouchCheckBox* IgnoreBg = (TouchCheckBox*)FindDescendent(StrToID((UCString)L"IgnoreBg"));
			TouchCheckBox* IgnoreCursors = (TouchCheckBox*)FindDescendent(StrToID((UCString)L"IgnoreCursors"));
			TouchCheckBox* IgnoreIcons = (TouchCheckBox*)FindDescendent(StrToID((UCString)L"IgnoreIcons"));
			TouchCheckBox* IgnoreColors = (TouchCheckBox*)FindDescendent(StrToID((UCString)L"IgnoreColors"));
			TouchCheckBox* IgnoreSounds = (TouchCheckBox*)FindDescendent(StrToID((UCString)L"IgnoreSounds"));
			TouchCheckBox* IgnoreScreensavers = (TouchCheckBox*)FindDescendent(StrToID((UCString)L"IgnoreScreensavers"));

			DWORD IgnoreBgVal = IgnoreBg->GetCheckedState() != CheckedStateFlags_NONE;
			DWORD IgnoreCursorsVal = IgnoreCursors->GetCheckedState() != CheckedStateFlags_NONE;
			DWORD IgnoreIconsVal = IgnoreIcons->GetCheckedState() != CheckedStateFlags_NONE;
			DWORD IgnoreColorsVal = IgnoreColors->GetCheckedState() != CheckedStateFlags_NONE;
			DWORD IgnoreSoundsVal = IgnoreSounds->GetCheckedState() != CheckedStateFlags_NONE;
			DWORD IgnoreScreensaversVal = IgnoreScreensavers->GetCheckedState() != CheckedStateFlags_NONE;

			HKEY Rectify11;
			if (RegCreateKey(HKEY_CURRENT_USER, Rectify11PrefsKey, &Rectify11))
			{
				SHOW_ERROR("Failed to create rectify11 key");
				return;
			}

			DWORD size = 4;

			RegSetKeyValueW(Rectify11, NULL, L"IgnoreBg", REG_DWORD, (LPCVOID)&IgnoreBgVal, sizeof(DWORD));
			RegSetKeyValueW(Rectify11, NULL, L"IgnoreCursors", REG_DWORD, (LPCVOID)&IgnoreCursorsVal, sizeof(DWORD));
			RegSetKeyValueW(Rectify11, NULL, L"IgnoreIcons", REG_DWORD, (LPCVOID)&IgnoreIconsVal, sizeof(DWORD));
			RegSetKeyValueW(Rectify11, NULL, L"IgnoreColors", REG_DWORD, (LPCVOID)&IgnoreColorsVal, sizeof(DWORD));
			RegSetKeyValueW(Rectify11, NULL, L"IgnoreSounds", REG_DWORD, (LPCVOID)&IgnoreSoundsVal, sizeof(DWORD));
			RegSetKeyValueW(Rectify11, NULL, L"IgnoreScreensavers", REG_DWORD, (LPCVOID)&IgnoreScreensaversVal, sizeof(DWORD));
			RegCloseKey(Rectify11);
			GoBack();
		}
		else if (iev->target->GetID() == StrToID((UCString)L"IgnoreThemePreferences"))
		{
			GoBack();
		}
	}
}

void RectifyThemeCfgPage::GoBack()
{
	GUID SID_STopLevelBrowser = {};
	HRESULT hr = CLSIDFromString(L"{4c96be40-915c-11cf-99d3-00aa004ae837}", (LPCLSID)&SID_STopLevelBrowser);
	if (SUCCEEDED(hr))
	{
		IShellBrowser* browser = NULL;
		hr = IUnknown_QueryService(site, SID_STopLevelBrowser, IID_IShellBrowser, (LPVOID*)&browser);
		if (SUCCEEDED(hr))
		{
			hr = browser->BrowseObject(NULL, SBSP_PARENT | SBSP_SAMEBROWSER);
			browser->Release();
		}
	}

	if (FAILED(hr))
	{
		std::string message = std::system_category().message(hr);
		CHAR buffer[1024];
		sprintf_s(buffer, "BrowseObject() failed with %d (%s)", hr, message.c_str());
		MessageBoxA(NULL, buffer, "Rectify Theme Configuration Page Element", MB_ICONERROR);
	}
}

void RectifyThemeCfgPage::OnInit()
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
	if (RegCreateKey(HKEY_CURRENT_USER, Rectify11PrefsKey, &Rectify11))
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
	RegCloseKey(Rectify11);


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
	initializing = false;
}
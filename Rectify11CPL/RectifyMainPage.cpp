#include "Rectify11CPL.h"
#include "RectifyMainPage.h"
#include "CRectifyUtil.h"
#include "ElevationManager.h"
#include "CControlPanelNavLinkCommand.h"
#include "CControlPanelNavLink.h"
#include "CControlPanelNavLinks.h"
#include <new>

IClassInfo* RectifyMainPage::Class = NULL;


RectifyMainPage::RectifyMainPage()
{

}

RectifyMainPage::~RectifyMainPage()
{

}

HRESULT RectifyMainPage::CreateInstance(Element* rootElement, unsigned long* debug_variable, Element** newElement)
{
	int hr = E_OUTOFMEMORY;

	// Using HeapAlloc instead of new() is required as DirectUI::Element::_DisplayNodeCallback calls HeapFree() with the element
	RectifyMainPage* instance = (RectifyMainPage*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(RectifyMainPage));

	if (instance != NULL)
	{
		new (instance) RectifyMainPage();
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

IClassInfo* RectifyMainPage::GetClassInfoW()
{
	return RectifyMainPage::Class;
}

void RectifyMainPage::OnEvent(Event* iev)
{
	if (iev->flag != GMF_ROUTED)
		return;
	if (!iev->handled)
		Element::OnEvent(iev);

	if (initializing) return;
	if (iev->target->GetID() == StrToID((UCString)L"Link_EnableAdmin"))
	{
		if (iev->type == TouchButton::Click)
		{
			IRectifyUtil* utility = ElevationManager::Initialize(GetMainHwnd());
			TouchCheckBox* MicaForEveryoneCheckbox = (TouchCheckBox*)FindDescendent(StrToID((UCString)L"MicaChk"));
			TouchCheckBox* TabbedCheckbox = (TouchCheckBox*)FindDescendent(StrToID((UCString)L"TabChk"));
			if (utility != NULL)
			{
				// Destroy old class
				if (RectifyUtil != NULL)
				{
					RectifyUtil->Release();
				}

				RectifyUtil = utility;
				HasAdmin = TRUE;
				iev->target->SetLayoutPos(-3);
				iev->target->SetVisible(FALSE);

				MicaForEveryoneCheckbox->SetEnabled(TRUE);
				if (MicaForEveryoneCheckbox->GetCheckedState() != CheckedStateFlags_NONE)
					TabbedCheckbox->SetEnabled(TRUE);


				CCRadioButton* Win11DefaultMenus = (CCRadioButton*)FindDescendent(StrToID((UCString)L"Win11DefaultMenus"));
				CCRadioButton* NilesoftSmall = (CCRadioButton*)FindDescendent(StrToID((UCString)L"NilesoftSmall"));
				CCRadioButton* NilesoftFull = (CCRadioButton*)FindDescendent(StrToID((UCString)L"NilesoftFull"));
				CCRadioButton* Classic = (CCRadioButton*)FindDescendent(StrToID((UCString)L"Classic"));
				CCRadioButton* ClassicTransparent = (CCRadioButton*)FindDescendent(StrToID((UCString)L"ClassicTransparent"));


				CCRadioButton* Options[] = { Win11DefaultMenus, NilesoftSmall, NilesoftFull, Classic, ClassicTransparent };
				for (size_t i = 0; i < 5; i++)
				{
					Options[i]->SetEnabled(TRUE);
				}
			}
		}
	}
	else if (iev->target->GetID() == StrToID((UCString)L"BtnRestartExplorer"))
	{
		if (iev->type == TouchButton::Click)
		{
			CRectifyUtil::RestartExplorer();

			// hide restart explorer button
			iev->target->SetLayoutPos(-3);
			iev->target->SetVisible(FALSE);
		}
	}
	else if (iev->target->GetID() == StrToID((UCString)L"ThemeCmb"))
	{
		if (iev->type == Combobox::SelectionChange)
		{
			TouchCheckBox* MicaForEveryoneCheckbox = (TouchCheckBox*)FindDescendent(StrToID((UCString)L"MicaChk"));
			TouchCheckBox* TabbedCheckbox = (TouchCheckBox*)FindDescendent(StrToID((UCString)L"TabChk"));
			int selection = ((Combobox*)iev->target)->GetSelection();

			ULONG apply_flags = 0;

			// load appy flags
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
			RegCloseKey(Rectify11);

			if (IgnoreBgVal)
			{
				apply_flags |= THEMETOOL_APPLY_FLAG_IGNORE_BACKGROUND;
			}
			if (IgnoreCursorsVal)
			{
				apply_flags |= THEMETOOL_APPLY_FLAG_IGNORE_CURSOR;
			}
			if (IgnoreIconsVal)
			{
				apply_flags |= THEMETOOL_APPLY_FLAG_IGNORE_DESKTOP_ICONS;
			}
			if (IgnoreColorsVal)
			{
				apply_flags |= THEMETOOL_APPLY_FLAG_IGNORE_COLOR;
			}
			if (IgnoreSoundsVal)
			{
				apply_flags |= THEMETOOL_APPLY_FLAG_IGNORE_SOUND;
			}
			if (IgnoreSoundsVal)
			{
				apply_flags |= THEMETOOL_APPLY_FLAG_IGNORE_SCREENSAVER;
			}

			// apply the theme
			themetool_set_active(NULL, themes[selection], TRUE, apply_flags, 0);
			UpdateThemeGraphic();

			// update mica
			if (HasAdmin)
			{
				BOOL hasMica = FALSE;
				BOOL hasTabbed = FALSE;
				RectifyUtil->GetMicaSettings(&hasMica, &hasTabbed);
				RectifyUtil->SetMicaForEveryoneEnabled(hasMica, hasTabbed);

				// update checkboxes in case we aren't using a mica theme anymore
				MicaForEveryoneCheckbox->SetCheckedState(hasMica ? CheckedStateFlags_CHECKED : CheckedStateFlags_NONE);
				TabbedCheckbox->SetCheckedState(hasTabbed ? CheckedStateFlags_CHECKED : CheckedStateFlags_NONE);
			}
		}
	}
	else if (iev->target->GetID() == StrToID((UCString)L"buttonHelp"))
	{
		if (iev->type == Button::Click)
		{
			ShellExecute(0, 0, TEXT("http://rectify11.net"), 0, 0, SW_SHOW);
		}
	}
	else if (iev->target->GetID() == StrToID((UCString)L"MicaChk"))
	{
		TouchCheckBox* MicaForEveryoneCheckbox = (TouchCheckBox*)iev->target;
		TouchCheckBox* TabbedCheckbox = (TouchCheckBox*)GetRoot()->FindDescendent(StrToID((UCString)L"TabChk"));
		Combobox* ThemeCombo = (Combobox*)GetRoot()->FindDescendent(StrToID((UCString)L"ThemeCmb"));
		if (iev->type == TouchButton::Click)
		{
			CheckedStateFlags MicaEnabled2 = MicaForEveryoneCheckbox->GetCheckedState();
			CheckedStateFlags TabbedEnabled = TabbedCheckbox->GetCheckedState();

			RectifyUtil->SetMicaForEveryoneEnabled(MicaEnabled2 ? CheckedStateFlags_CHECKED : CheckedStateFlags_NONE, TabbedEnabled ? CheckedStateFlags_CHECKED : CheckedStateFlags_NONE);

			// Enable/disable the tabbed checkbox
			if (TabbedCheckbox != NULL)
				TabbedCheckbox->SetEnabled(MicaEnabled2 ? CheckedStateFlags_CHECKED : CheckedStateFlags_NONE);
		}
	}
	else if (iev->target->GetID() == StrToID((UCString)L"TabChk"))
	{
		TouchCheckBox* TabbedCheckbox = (TouchCheckBox*)iev->target;
		Combobox* ThemeCombo = (Combobox*)FindDescendent(StrToID((UCString)L"ThemeCmb"));

		if (iev->type == TouchButton::Click)
		{
			RectifyUtil->SetMicaForEveryoneEnabled(TRUE, TabbedCheckbox->GetCheckedState() ? CheckedStateFlags_CHECKED : CheckedStateFlags_NONE);
		}
	}

	// handle menu section
	if (iev->type == Button::Click && !wcscmp((const wchar_t*)iev->target->GetClassInfoW()->GetName(), (const wchar_t*)CCRadioButton::GetClassInfoPtr()->GetName()))
	{
		CCRadioButton* chkbox = (CCRadioButton*)iev->target;
		if (chkbox->GetSelected())
		{
			HRESULT hr = E_ACTIVATIONDENIED_SHELLERROR;
			if (chkbox->GetID() == StrToID((UCString)L"Win11DefaultMenus"))
			{
				hr = RectifyUtil->SetCurrentMenuByIndex(Normal);
			}
			else if (chkbox->GetID() == StrToID((UCString)L"NilesoftSmall"))
			{
				hr = RectifyUtil->SetCurrentMenuByIndex(NilesoftSmall);
			}
			else if (chkbox->GetID() == StrToID((UCString)L"NilesoftFull"))
			{
				hr = RectifyUtil->SetCurrentMenuByIndex(NilesoftFull);
			}
			else if (chkbox->GetID() == StrToID((UCString)L"Classic"))
			{
				hr = RectifyUtil->SetCurrentMenuByIndex(Classic);
			}
			else if (chkbox->GetID() == StrToID((UCString)L"ClassicTransparent"))
			{
				hr = RectifyUtil->SetCurrentMenuByIndex(ClassicTransparent);
			}

			if (FAILED(hr))
			{
				WCHAR buffer[200];
				swprintf(buffer, 199, L"Failed to update menu settings. HRESULT is %x", hr);
				MessageBox(NULL, buffer, TEXT("ClassicTransparent_OnEvent"), MB_ICONERROR);
			}
			else
			{
				ShowRestartExplorer();
			}
		}
	}
}

void RectifyMainPage::ShowRestartExplorer()
{
	TouchButton* BtnRestartExplorer = (TouchButton*)FindDescendent(StrToID((UCString)L"BtnRestartExplorer"));
	BtnRestartExplorer->SetLayoutPos(0);
	BtnRestartExplorer->SetVisible(TRUE);
}

void RectifyMainPage::UpdateThemeGraphic()
{
	LPCWSTR id = IsDarkTheme() ? MAKEINTRESOURCE(IDB_DARKPREVIEW) : MAKEINTRESOURCE(IDB_LIGHTPREVIEW);
	HBITMAP bmp = (HBITMAP)LoadImage(g_hInst, id, IMAGE_BITMAP, 256, 256, 0);
	if (bmp == NULL)
	{
		return;
	}
	Value* bitmap = DirectUI::Value::CreateGraphic(bmp, 3, 0xffffffff, false, false, false);
	Element* PreviewElement = FindDescendent(StrToID((UCString)L"ThemePreview"));
	if (PreviewElement != NULL)
		PreviewElement->SetValue(Element::ContentProp, 1, bitmap);
	bitmap->Release();
}

void RectifyMainPage::InitNavLinks()
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
		int hr = IUnknown_QueryService(site, SID_PerLayoutPropertyBag, IID_IPropertyBag, (LPVOID*)&bag);
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


void RectifyMainPage::OnInit()
{
	Element* root = GetRoot();
	RectifyUtil = (IRectifyUtil*)new CRectifyUtil();
	InitNavLinks();

	Combobox* ThemeCombo = (Combobox*)root->FindDescendent(StrToID((UCString)L"ThemeCmb"));
	Button* HelpButton = (Button*)root->FindDescendent(StrToID((UCString)L"buttonHelp"));
	TouchCheckBox* MicaForEveryoneCheckbox = (TouchCheckBox*)root->FindDescendent(StrToID((UCString)L"MicaChk"));
	TouchCheckBox* TabbedCheckbox = (TouchCheckBox*)root->FindDescendent(StrToID((UCString)L"TabChk"));
	Element* version = (Element*)root->FindDescendent(StrToID((UCString)L"RectifyVersion"));
	TouchButton* enableAdmin = (TouchButton*)root->FindDescendent(StrToID((UCString)L"Link_EnableAdmin"));
	TouchButton* BtnRestartExplorer = (TouchButton*)root->FindDescendent(StrToID((UCString)L"BtnRestartExplorer"));

	CCRadioButton* Win11DefaultMenus = (CCRadioButton*)root->FindDescendent(StrToID((UCString)L"Win11DefaultMenus"));
	CCRadioButton* NilesoftSmall = (CCRadioButton*)root->FindDescendent(StrToID((UCString)L"NilesoftSmall"));
	CCRadioButton* NilesoftFull = (CCRadioButton*)root->FindDescendent(StrToID((UCString)L"NilesoftFull"));
	CCRadioButton* Classic = (CCRadioButton*)root->FindDescendent(StrToID((UCString)L"Classic"));
	CCRadioButton* ClassicTransparent = (CCRadioButton*)root->FindDescendent(StrToID((UCString)L"ClassicTransparent"));

	CCRadioButton* Options[] = { Win11DefaultMenus, NilesoftSmall, NilesoftFull, Classic, ClassicTransparent };

	if (ThemeCombo != NULL)
	{
		WCHAR value[255] = { 0 };
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

						std::wstring msstylePathClean = msstylePath;

						const size_t last_slash_idx = msstylePathClean.find_last_of(L"\\/");
						if (std::string::npos != last_slash_idx)
						{
							msstylePathClean.erase(0, last_slash_idx + 1);
						}

						// Remove extension if present.
						const size_t period_idx = msstylePathClean.rfind('.');
						if (std::string::npos != period_idx)
						{
							msstylePathClean.erase(period_idx);
						}

						ThemesMap[k] = msstylePathClean;
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
			LONG result = RegGetValue(HKEY_LOCAL_MACHINE, L"Software\\Rectify11", L"Version", RRF_RT_REG_SZ, 0, pvData, &size);
			std::wstring vstr = std::wstring(L"");
			if (result == 0)
			{
				WCHAR versionstr[1024];
				if (FAILED(LoadStringW(g_hInst, IDS_VERSION, versionstr, 1023)))
				{
					wcscpy_s(versionstr, L"[VERSION STRING MISSING]: ");
				}
				vstr += versionstr;
				vstr += L" ";
				vstr += value;
			}
			else
			{
				WCHAR versionstr[1024];
				WCHAR notapplicable[1024];
				if (FAILED(LoadStringW(g_hInst, IDS_VERSION, versionstr, 1023)))
				{
					wcscpy_s(versionstr, L"[VERSION STRING MISSING]: ");
				}
				if (FAILED(LoadStringW(g_hInst, IDS_NA, notapplicable, 1023)))
				{
					wcscpy_s(notapplicable, L"[N/A STRING MISSING]: ");
				}

				vstr += versionstr;
				vstr += L" ";
				vstr += notapplicable;
			}
			version->SetContentString((UCString)vstr.c_str());
		}
	}

	if (MicaForEveryoneCheckbox != NULL)
	{
		MicaForEveryoneCheckbox->SetToggleOnClick(true);
		BOOL MicaEnabled;
		BOOL TabbedEnabled;
		RectifyUtil->GetMicaSettings(&MicaEnabled, &TabbedEnabled);

		MicaForEveryoneCheckbox->SetCheckedState(MicaEnabled ? CheckedStateFlags_CHECKED : CheckedStateFlags_NONE);

		if (!MicaEnabled && TabbedCheckbox != NULL)
		{
			TabbedCheckbox->SetEnabled(FALSE);
		}
	}

	if (TabbedCheckbox != NULL)
	{
		BOOL MicaEnabled;
		BOOL TabbedEnabled;
		RectifyUtil->GetMicaSettings(&MicaEnabled, &TabbedEnabled);

		TabbedCheckbox->SetToggleOnClick(true);
		TabbedCheckbox->SetCheckedState(MicaEnabled ? CheckedStateFlags_CHECKED : CheckedStateFlags_NONE);
	}

	if (BtnRestartExplorer != NULL)
	{
		BtnRestartExplorer->SetLayoutPos(-3);
		BtnRestartExplorer->SetVisible(FALSE);
	}

	DWORD menuIndex;

	for (size_t i = 0; i < 5; i++)
	{
		if (!HasAdmin)
			Options[i]->SetEnabled(FALSE);
		else
			Options[i]->SetEnabled(TRUE);
	}
	if (SUCCEEDED(RectifyUtil->GetCurrentMenuIndex(&menuIndex)))
	{
		Options[menuIndex]->SetSelected(true);
	}

	if (HasAdmin)
	{
		enableAdmin->SetLayoutPos(-3);
		enableAdmin->SetVisible(FALSE);
	}
	else
	{
		MicaForEveryoneCheckbox->SetEnabled(FALSE);
		TabbedCheckbox->SetEnabled(FALSE);
	}

	UpdateThemeGraphic();
	initializing = false;
}

HWND RectifyMainPage::GetMainHwnd()
{
	GUID SID_STopLevelBrowser = {}, IID_IFrameManager = {};
	HRESULT hr = CLSIDFromString(L"{4c96be40-915c-11cf-99d3-00aa004ae837}", (LPCLSID)&SID_STopLevelBrowser);
	HWND result = NULL;
	if (SUCCEEDED(hr))
	{
		hr = CLSIDFromString(L"{31e4fa78-02b4-419f-9430-7b7585237c77}", (LPCLSID)&IID_IFrameManager);
		if (SUCCEEDED(hr))
		{
			IShellBrowser* browser = NULL;
			if (SUCCEEDED(IUnknown_QueryService(site, SID_STopLevelBrowser, IID_IShellBrowser, (LPVOID*)&browser)))
			{
				browser->GetWindow(&result);
				browser->Release();
			}
		}
	}

	return result;
}

void RectifyMainPage::OnDestroy()
{
	if (RectifyUtil != NULL)
	{
		RectifyUtil->Release();
		RectifyUtil = NULL;
	}

	Element::OnDestroy();
}
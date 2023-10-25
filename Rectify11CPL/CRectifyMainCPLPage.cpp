#include "Rectify11CPL.h"
#include "CRectifyMainCPLPage.h"
#include "CRectifyUtil.h"

ThemesMapBase CRectifyMainCPLPage::ThemesMap;
vector<ULONG> CRectifyMainCPLPage::themes;
IClassInfo* CRectifyMainCPLPage::Class = NULL;
#define NOT_IMPLEMENTED MessageBox(NULL, TEXT(__FUNCTION__), TEXT("Non implementented function in CRectifyMainCPLPage"), 0)


CRectifyMainCPLPage::CRectifyMainCPLPage()
{
	
}
CRectifyMainCPLPage::~CRectifyMainCPLPage()
{
	
}

HRESULT CRectifyMainCPLPage::CreateInstance(Element* rootElement, unsigned long* debug_variable, Element** newElement)
{
	int hr = E_OUTOFMEMORY;

	CRectifyMainCPLPage* instance = new CRectifyMainCPLPage();

	if (instance != NULL)
	{
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

IClassInfo* CRectifyMainCPLPage::GetClassInfoW()
{
	return CRectifyMainCPLPage::Class;
}

void CRectifyMainCPLPage::OnEvent(Event* iev)
{
	Element::OnEvent(iev);
}

void CRectifyMainCPLPage::UpdateThemeGraphic()
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

void CRectifyMainCPLPage::OnInit()
{
	Element* root = GetRoot();
	RectifyUtil = (IRectifyUtil*)new CRectifyUtil();
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
			RegGetValue(HKEY_LOCAL_MACHINE, L"Software\\Rectify11", L"Version", RRF_RT_REG_SZ, 0, pvData, &size);
			std::wstring vstr = std::wstring(L"Rectify11 version: ");
			vstr += (LPCWSTR)pvData;
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

	if (!HasAdmin)
	{
		MicaForEveryoneCheckbox->SetEnabled(FALSE);
		TabbedCheckbox->SetEnabled(FALSE);

	}
	else {
		enableAdmin->SetLayoutPos(-3);
		enableAdmin->SetVisible(FALSE);
	}


	UpdateThemeGraphic();
}

void CRectifyMainCPLPage::OnNavigateAway()
{
	HasAdmin = FALSE;
	if (RectifyUtil != NULL)
	{
		RectifyUtil->Release();
		RectifyUtil = NULL;
	}
}
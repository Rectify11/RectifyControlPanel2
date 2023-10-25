#include "Rectify11CPL.h"
#include "CRectifyMainCPLPage.h"
#include "CRectifyUtil.h"
#include "ElevationManager.h"

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
	if (iev->flag != GMF_ROUTED)
		return;
	if (!iev->handled)
		Element::OnEvent(iev);
	if (initializing) return;
	if (iev->target->GetID() == StrToID((UCString)L"Link_EnableAdmin"))
	{
		if (iev->type == TouchButton::Click)
		{
			IRectifyUtil* utility = ElevationManager::Initialize(TRUE);
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
			themetool_set_active(NULL, themes[selection], TRUE, 0, 0);
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
	if (iev->type == Button::Click && iev->target->GetClassInfoW() == CCCheckBox::GetClassInfoPtr())
	{
		CCCheckBox* chkbox = (CCCheckBox*)iev->target;
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
void CRectifyMainCPLPage::ShowRestartExplorer()
{
	TouchButton* BtnRestartExplorer = (TouchButton*)FindDescendent(StrToID((UCString)L"BtnRestartExplorer"));
	BtnRestartExplorer->SetLayoutPos(0);
	BtnRestartExplorer->SetVisible(TRUE);
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

void CRectifyMainCPLPage::OnDestroy()
{
	if (RectifyUtil != NULL)
	{
		RectifyUtil->Release();
		RectifyUtil = NULL;
	}
	Element::OnDestroy();
}
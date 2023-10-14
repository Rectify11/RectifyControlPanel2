#pragma once
class CRectifyUtil
{
public:
	static BOOL CheckIfMicaForEveryoneIsEnabled();
	static void SetMicaForEveryoneEnabled(wstring currentThemeName, BOOL micaEnabled, BOOL tabbed);

	static BOOL GetTabbedEnabled();

	static BOOL IsDarkTheme();
};


#pragma once
class CRectifyUtil
{
public:
	static BOOL CheckIfMicaForEveryoneIsEnabled();
	static void SetMicaForEveryoneEnabled(BOOL enabled);

	static BOOL GetTabbedEnabled();
	static void SetTabbedEnabled(BOOL enabled);

	static BOOL IsDarkTheme();
};


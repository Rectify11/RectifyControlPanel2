#include "Rectify11CPL.h"
#include "CRectifyUtil.h"
/// <summary>
/// Check if mica for everyone is enabled
/// </summary>
/// <returns>Returns if Mica for everyone is enabled</returns>
BOOL CRectifyUtil::CheckIfMicaForEveryoneIsEnabled()
{
	// TODO
	return FALSE;
}
/// <summary>
/// Enable/disable micaforeveryone tool
/// </summary>
/// <param name="enabled"></param>
void CRectifyUtil::SetMicaForEveryoneEnabled(BOOL enabled)
{
	// TODO
}

/// <summary>
/// Check if mica tab is used instead of mica
/// </summary>
/// <returns></returns>
BOOL CRectifyUtil::GetTabbedEnabled()
{
	// TODO
	return FALSE;
}

/// <summary>
/// Set if mica tab is used instead of regular mica.
/// </summary>
/// <param name="enaled"></param>
void CRectifyUtil::SetTabbedEnabled(BOOL enabled)
{
	// TODO
}

BOOL CRectifyUtil::IsDarkTheme()
{
	WCHAR value[255] = {0};
	PVOID pvData = value;
	DWORD size = sizeof(value);
	RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\ThemeManager", L"DllName", RRF_RT_REG_SZ, 0, pvData, &size);
	std::wstring msstylePath = std::wstring((LPCWSTR)pvData);

	size_t result = msstylePath.find(L"Dark");
	return result > 0 ? TRUE : FALSE;
}
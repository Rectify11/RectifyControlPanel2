#pragma once
class CControlPanelNavLink
{
public:
	static HRESULT CControlPanelNavLink::Create(CPNAV_LIST list, CControlPanelNavLink** result);
	void SetName(LPCWSTR name);
	CPNAV_LIST m_Type;
	DWORD m_Unknown;
	WCHAR* m_Name;
	WCHAR* m_Name2;
	HICON m_Icon;
	CPNAVTYPE m_ExecType;
	DWORD m_Unknown3;
	ULONGLONG m_Unknown4;
	ULONGLONG m_Unknown5;
	char IsBulletPoint;
	char data[0x3f];
};


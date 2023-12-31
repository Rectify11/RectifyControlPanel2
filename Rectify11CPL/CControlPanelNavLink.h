#pragma once
class CControlPanelNavLink
{
public:
	static HRESULT Create(CPNAV_LIST list, CControlPanelNavLink** result);
	void SetName(LPCWSTR name);
	CPNAV_LIST m_Type; //0x00
	DWORD m_Unknown; //0x04
	WCHAR* m_Name; //0x08
	WCHAR* m_args; //0x18
	HICON m_Icon; //0x20
	CControlPanelNavLinkCommand m_ExecType;
	char unknown_bytes[0x33];
};


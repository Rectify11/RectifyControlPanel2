#pragma once
class CControlPanelNavLinkCommand
{
public:
	CPNAVTYPE m_ExecType;
	DWORD m_space;
	LPWSTR m_AppletOrCommand;
	LPWSTR m_Arguments;
};


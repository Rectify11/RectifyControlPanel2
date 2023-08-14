#include "Rectify11CPL.h"
#include "CControlPanelNavLinkCommand.h"
#include "CControlPanelNavLink.h"

HRESULT __cdecl CControlPanelNavLink::Create(CPNAV_LIST list, CControlPanelNavLink** result)
{
	CControlPanelNavLink* navLink = new CControlPanelNavLink();
	if (navLink != NULL)
	{
		navLink->m_Icon = NULL;
		navLink->m_Name = NULL;
		navLink->m_ExecType.m_ExecType = CPNAVTYPE_None;
		navLink->m_Type = list;
		*result = navLink;
		return S_OK;
	}
	else
	{
		return E_OUTOFMEMORY;
	}
}

void CControlPanelNavLink::SetName(LPCWSTR name)
{
	SHStrDupW(name, &this->m_Name);
}
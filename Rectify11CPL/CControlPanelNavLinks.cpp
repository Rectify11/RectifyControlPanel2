#include "pch.h"
#include "CControlPanelNavLinks.h"
#include "CControlPanelNavLink.h"

CControlPanelNavLinks::CControlPanelNavLinks()
{
	m_dpaList = NULL;
	m_refCount = 1;
}

void NavLinksDPA_DeleteCB(CControlPanelNavLink* p, void* pData)
{
	delete p;
}

CControlPanelNavLinks::~CControlPanelNavLinks()
{
	if (this->m_dpaList != NULL) {
		DPA_DestroyCallback(this->m_dpaList, (PFNDAENUMCALLBACKCONST)NavLinksDPA_DeleteCB, NULL);
		this->m_dpaList = NULL;
	}
}

IFACEMETHODIMP CControlPanelNavLinks::QueryInterface(REFIID riid, __out void** ppv)
{
	*ppv = NULL;
	if (riid == IID_IUnknown)
	{
		*ppv = (IUnknown*)this;
		return S_OK;
	}
	return E_NOINTERFACE;
}
IFACEMETHODIMP_(ULONG) CControlPanelNavLinks::AddRef()
{
	return InterlockedIncrement(&m_refCount);
}
IFACEMETHODIMP_(ULONG) CControlPanelNavLinks::Release()
{
	ULONGLONG ref = InterlockedDecrement(&m_refCount);
	if (ref == 0)
	{
		delete this;
	}
	return ref;
}
HRESULT CControlPanelNavLinks::AddLinkShellEx(LPCWSTR name, LPCWSTR file, LPCWSTR arguments, CPNAV_LIST DisplayType, HICON icon)
{
	CControlPanelNavLink* link = NULL;
	HRESULT hr = CControlPanelNavLink::Create(DisplayType, &link);
	if (SUCCEEDED(hr))
	{
		link->SetName(name);

		if (icon != NULL)
		{
			link->m_Icon = icon;
		}

		link->m_ExecType.m_ExecType = CPNAVTYPE_ShellExec;
		SHStrDupW(file, &link->m_ExecType.m_AppletOrCommand);
		SHStrDupW(arguments, &link->m_ExecType.m_Arguments);
		return Add(link);
	}
	else
	{
		return hr;
	}
}
HRESULT CControlPanelNavLinks::AddLinkControlPanel(LPCWSTR name, LPCWSTR path, LPCWSTR arguments, CPNAV_LIST DisplayType, HICON icon)
{
	CControlPanelNavLink* link = NULL;
	HRESULT hr = CControlPanelNavLink::Create(DisplayType, &link);
	if (SUCCEEDED(hr))
	{
		link->SetName(name);

		if (icon != NULL)
		{
			link->m_Icon = icon;
		}

		link->m_ExecType.m_ExecType = CPNAVTYPE_Navigate;
		SHStrDupW(path, &link->m_ExecType.m_AppletOrCommand);
		SHStrDupW(arguments, &link->m_ExecType.m_Arguments);
		return Add(link);
	}
	else
	{
		return hr;
	}
}

HRESULT CControlPanelNavLinks::Add(CControlPanelNavLink* link)
{
	if (m_dpaList == NULL)
	{
		m_dpaList = DPA_Create(10);
		if (m_dpaList == NULL)
		{
			return E_OUTOFMEMORY;
		}
	}
	if (DPA_InsertPtr(m_dpaList, 0x7fffffff, link) != -1)
	{
		return S_OK;
	}
	else
	{
		return E_OUTOFMEMORY;
	}
}
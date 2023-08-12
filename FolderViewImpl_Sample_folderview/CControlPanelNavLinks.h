#pragma once
class CControlPanelNavLinks : public IUnknown
{
public:
	CControlPanelNavLinks();

	IFACEMETHODIMP QueryInterface(REFIID riid, __out void** ppv);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();

	HRESULT AddLinkControlPanel();
	HRESULT AddLinkShellEx(LPCWSTR name);
	HRESULT Add(CControlPanelNavLink* link);
private:
	HDPA m_dpaList;
	ULONGLONG m_refCount;
};


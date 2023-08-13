#pragma once
class CControlPanelNavLinks : public IUnknown
{
public:
	CControlPanelNavLinks();

	IFACEMETHODIMP QueryInterface(REFIID riid, __out void** ppv);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();

	HRESULT AddLinkControlPanel(LPCWSTR name, LPCWSTR path, LPCWSTR arguments, CPNAV_LIST DisplayType, HICON icon);
	HRESULT AddLinkShellEx(LPCWSTR name, LPCWSTR file, LPCWSTR arguments, CPNAV_LIST DisplayType, HICON icon);
	HRESULT Add(CControlPanelNavLink* link);
	virtual ~CControlPanelNavLinks();
private:
	HDPA m_dpaList;
	ULONGLONG m_refCount;
};


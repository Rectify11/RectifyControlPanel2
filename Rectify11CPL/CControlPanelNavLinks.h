#pragma once
class CControlPanelNavLinks : public IUnknown
{
public:
	CControlPanelNavLinks();

	IFACEMETHODIMP QueryInterface(REFIID riid, __out void** ppv);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();

	HRESULT AddLinkControlPanel(LPCWSTR name, LPCWSTR path, LPCWSTR arguments, CPNAV_LIST DisplayType, SHSTOCKICONID iconType);
	HRESULT AddLinkShellEx(LPCWSTR name, LPCWSTR file, LPCWSTR arguments, CPNAV_LIST DisplayType, SHSTOCKICONID iconType);
	HRESULT Add(CControlPanelNavLink* link);
	virtual ~CControlPanelNavLinks();
private:
	HDPA m_dpaList;
	ULONGLONG m_refCount;
};


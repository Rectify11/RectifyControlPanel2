#include "pch.h"
#include "CRectifyMainCPLPage.h"

IClassInfo* CRectifyMainCPLPage::Class = NULL;
#define NOT_IMPLEMENTED MessageBox(NULL, TEXT(__FUNCTION__), TEXT("Non implementented function in CRectifyMainCPLPage"), 0)

HRESULT CRectifyMainCPLPage::Create(Element* a, unsigned long* b, Element** c)
{
	int hr = E_OUTOFMEMORY;
	CRectifyMainCPLPage* instance = new CRectifyMainCPLPage();
	if (instance != NULL)
	{
		hr = instance->Initialize(0, a, b);
		if (SUCCEEDED(hr))
		{
			*c = instance;
			instance = NULL;
		}
		if (instance != NULL)
		{
			instance->Destroy(TRUE);
		}
	}

	return hr;
}

// IFrameNotificationClient
HRESULT STDMETHODCALLTYPE CRectifyMainCPLPage::LayoutInitialized()
{
	NOT_IMPLEMENTED;
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CRectifyMainCPLPage::Notify(WORD* param)
{
	NOT_IMPLEMENTED;
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CRectifyMainCPLPage::OnNavigateAway() {
	NOT_IMPLEMENTED;
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CRectifyMainCPLPage::OnInnerElementDestroyed() {
	NOT_IMPLEMENTED;
	return S_OK;
}

IClassInfo* CRectifyMainCPLPage::GetClassInfoW()
{
	return CRectifyMainCPLPage::Class;
}

//IUnknown
HRESULT STDMETHODCALLTYPE CRectifyMainCPLPage::QueryInterface(REFIID riid, void** ppv)
{
	static const QITAB qit[] = {
		QITABENT(CRectifyMainCPLPage, IObjectWithSite),
		{ 0 },
	};
	HRESULT hr = QISearch(this, qit, riid, ppv);

	if (hr != S_OK)
	{
		char szGuid[40] = { 0 };

		sprintf(szGuid, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", riid.Data1, riid.Data2, riid.Data3, riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6], riid.Data4[7]);

		MessageBox(NULL, szGuid, TEXT("Unknown interface in CRectifyMainCPLPage::QueryInterface()"), 0);
	}
	return hr;
}

ULONG STDMETHODCALLTYPE CRectifyMainCPLPage::AddRef(void)
{
	return 1;
}

ULONG STDMETHODCALLTYPE CRectifyMainCPLPage::Release(void)
{
	return 1;
}

#include "Rectify11CPL.h"
#include "CRectifyMainCPLPage.h"
#include "Rectify11CPL.h"

IClassInfo* CRectifyMainCPLPage::Class = NULL;
#define NOT_IMPLEMENTED MessageBox(NULL, TEXT(__FUNCTION__), TEXT("Non implementented function in CRectifyMainCPLPage"), 0)


CRectifyMainCPLPage::CRectifyMainCPLPage()
{
	
}
CRectifyMainCPLPage::~CRectifyMainCPLPage()
{
	
}

HRESULT CRectifyMainCPLPage::CreateInstance(Element* rootElement, unsigned long* debug_variable, Element** newElement)
{
	int hr = E_OUTOFMEMORY;

	CRectifyMainCPLPage* instance = new CRectifyMainCPLPage();

	if (instance != NULL)
	{
		hr = instance->Initialize(0, rootElement, debug_variable);
		if (SUCCEEDED(hr))
		{
			*newElement = instance;
		}
		else
		{
			if (instance != NULL)
			{
				instance->Destroy(TRUE);
				instance = NULL;
			}
		}
	}

	return hr;
}

// IFrameNotificationClient
//HRESULT STDMETHODCALLTYPE CRectifyMainCPLPage::LayoutInitialized()
//{
//	NOT_IMPLEMENTED;
//	return S_OK;
//}
//HRESULT STDMETHODCALLTYPE CRectifyMainCPLPage::Notify(WORD* param)
//{
//	NOT_IMPLEMENTED;
//	return S_OK;
//}
//HRESULT STDMETHODCALLTYPE CRectifyMainCPLPage::OnNavigateAway() {
//	NOT_IMPLEMENTED;
//	return S_OK;
//}
//HRESULT STDMETHODCALLTYPE CRectifyMainCPLPage::OnInnerElementDestroyed() {
//	NOT_IMPLEMENTED;
//	return S_OK;
//}

IClassInfo* CRectifyMainCPLPage::GetClassInfoW()
{
	return CRectifyMainCPLPage::Class;
}

////IUnknown
//HRESULT STDMETHODCALLTYPE CRectifyMainCPLPage::QueryInterface(REFIID riid, void** ppv)
//{
//	static const QITAB qit[] = {
//		QITABENT(CRectifyMainCPLPage, IObjectWithSite),
//		{ 0 },
//	};
//	HRESULT hr = QISearch(this, qit, riid, ppv);
//
//	if (hr != S_OK)
//	{
//		WCHAR szGuid[40] = { 0 };
//
//		swprintf_s(szGuid, 40, L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", riid.Data1, riid.Data2, riid.Data3, riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6], riid.Data4[7]);
//
//		MessageBox(NULL, szGuid, TEXT("Unknown interface in CRectifyMainCPLPage::QueryInterface()"), 0);
//	}
//	return hr;
//}
//
//ULONG STDMETHODCALLTYPE CRectifyMainCPLPage::AddRef(void)
//{
//	return 1;
//}
//
//ULONG STDMETHODCALLTYPE CRectifyMainCPLPage::Release(void)
//{
//	return 1;
//}
//
void CRectifyMainCPLPage::OnEvent(Event* iev)
{
	Element::OnEvent(iev);
}
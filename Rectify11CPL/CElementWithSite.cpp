#include "pch.h"
#include "CElementWithSite.h"
#include "CElementWithIUnknown.h"
using namespace DirectUI;
IClassInfo* CElementWithSite::Class = NULL;

HRESULT CElementWithSite::Register(HINSTANCE hInstance, unsigned short const* unknown, bool unknown2, PropertyInfo const* const* unknown3, unsigned int unknown4)
{
	//DirectUI::ClassInfo<class_CElementWithIUnknown, class_DirectUI::Element, class_DirectUI::StandardCreator<class_CElementWithIUnknown>_>
	int hr = ClassInfo_CElementWithIUnknown::Register(hInstance, unknown, unknown2, unknown3,0,FALSE);
	if (SUCCEEDED(hr))
	{
		hr = ClassInfo_CElementWithSite::Register(hInstance, unknown, unknown2, unknown3, 0, FALSE);
		if (!SUCCEEDED(hr))
		{
			MessageBox(NULL, TEXT("Failed to register CElementWithSite control"), TEXT("DirectUI error"), 0);
		}
	}
	else
	{
		MessageBox(NULL, TEXT("Failed to register CElementWithIUnknown control"), TEXT("DirectUI error"), 0);
	}
	return hr;
}


HRESULT STDMETHODCALLTYPE CElementWithSite::QueryInterface(REFIID riid, void** ppv)
{
	static const QITAB qit[] = {
		QITABENT(CElementWithSite, IObjectWithSite),
		{ 0 },
	};
	HRESULT hr = QISearch(this, qit, riid, ppv);

	if (hr != S_OK)
	{
		char szGuid[40] = { 0 };

		sprintf(szGuid, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", riid.Data1, riid.Data2, riid.Data3, riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6], riid.Data4[7]);

		MessageBox(NULL, szGuid, TEXT("Unknown interface in CElementWithSite::QueryInterface()"), 0);
	}
	return hr;
 }

ULONG STDMETHODCALLTYPE CElementWithSite::AddRef(void)
{
	return 1;
}

ULONG STDMETHODCALLTYPE CElementWithSite::Release(void)
{
	return 1;
}


HRESULT STDMETHODCALLTYPE CElementWithSite::SetSite(IUnknown* pUnkSite) {
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CElementWithSite::GetSite(REFIID riid, void** ppvSite) {
	return S_OK;
}
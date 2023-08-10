#include <windows.h>
#include <shlobj.h>
#include <propkey.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <shellapi.h>

#include "FolderViewImpl.h"
#include "ElementProvider.h"
#include "resource.h"
#include "pch.h"

//3072    <<not bound>>
typedef HRESULT(*Dui70_XProvider_QueryInterface)(REFIID riid, __out void** ppv);
Dui70_XProvider_QueryInterface Dui70_XProvider_QueryInterface_Func;


#define NOT_IMPLEMENTED MessageBox(NULL, TEXT(__FUNCTION__), TEXT("Non implementented function in CElementProvider"), 0)
#define SHOW_ERROR(x) MessageBox(NULL, TEXT(x), TEXT("Error in CElementProvider"), 0)


CElementProvider::CElementProvider() : _punkSite(NULL)
{
	if (FAILED(InitProcessPriv(14, NULL, 1, true)))
	{
		SHOW_ERROR("Failed to initialize DirectUI\n");
	}
	if (FAILED(InitThread(2)))
	{
		SHOW_ERROR("Failed to initialize DirectUI for thread\n");
	}
	DllAddRef();
}

CElementProvider::~CElementProvider()
{
	//if (-1 < *(int*)&this->initthread_result) {
		UnInitThread();
		UnInitProcessPriv((unsigned short*)0x180000000);
	//}
	//DllRelease();
}

HRESULT CElementProvider::QueryInterface(REFIID riid, __out void** ppv)
{
	static const QITAB qit[] = {
		QITABENT(CElementProvider, IDUIElementProviderInit),
		QITABENT(CElementProvider, IFrameNotificationClient),
		QITABENT(CElementProvider, IFrameShellViewClient),
		QITABENT(CElementProvider, IObjectWithSite),
		QITABENT(CElementProvider, IServiceProvider),
		{ 0 },
	};
	HRESULT hr = QISearch(this, qit, riid, ppv);

	if (hr != S_OK)
	{
		hr = DirectUI::XProvider::QueryInterface(riid, ppv);
	}
	if (hr != S_OK)
	{
		char szGuid[40] = { 0 };

		sprintf(szGuid, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", riid.Data1, riid.Data2, riid.Data3, riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6], riid.Data4[7]);

		MessageBox(NULL, szGuid, TEXT("Unknown interface in CElementProvider::QueryInterface()"), 0);
	}
	return hr;
}

ULONG CElementProvider::AddRef()
{
	int i = 0;
	;
	 i =DirectUI::XProvider::AddRef();

	 return i;
}

ULONG CElementProvider::Release()
{
	DWORD ret = refCount;
	ret--;
	if (ret == 0)
	{
		delete this;
	}
	else
	{
		refCount = ret;
	}
	return ret;
}

HRESULT CElementProvider::CreateDUI(DirectUI::IXElementCP* a, HWND* result_handle)
{
	MessageBox(NULL, TEXT("see title"), TEXT("CElementProvider::CreateDUI TODO"), 0);
	int hr = XProvider::CreateDUI(a, result_handle);
	if (SUCCEEDED(hr))
	{
		DirectUI::XProvider::SetHandleEnterKey(true);
	}
	else
	{
		MessageBox(NULL, TEXT("Failed to create DirectUI parser"), TEXT("CElementProvider::CreateDUI TODO"), 0);
	}
	return 0;
}

HRESULT STDMETHODCALLTYPE CElementProvider::SetResourceID(UINT id)
{
	char szGuid[40] = { 0 };

	sprintf(szGuid, "%d", id);
	MessageBox(NULL, szGuid, TEXT("CElementProvider::SetResourceId start"), 0);
	IFrameShellViewClient* client = this;
	int hr = DirectUI::XResourceProvider::Create(g_hInst, (unsigned short const*)id, (unsigned short const*)&client, 0, &this->resourceProvider);
	if (SUCCEEDED(hr))
	{
		hr = DirectUI::XProvider::Initialize(NULL, (IXProviderCP*)this->resourceProvider);
		if (!SUCCEEDED(hr))
		{
			MessageBox(NULL, szGuid, TEXT("CElementProvider::SetResourceId Failed to initialize xprovider"), 0);
		}
	}
	else
	{
		MessageBox(NULL, szGuid, TEXT("CElementProvider::SetResourceId failed to create xprovider"), 0);
	}
	return hr;
}

HRESULT STDMETHODCALLTYPE CElementProvider::OptionallyTakeInitialFocus(BOOL* result)
{
	NOT_IMPLEMENTED;
	*result = 0;
	return 0;
}

HRESULT STDMETHODCALLTYPE CElementProvider::LayoutInitialized()
{
	NOT_IMPLEMENTED;
	return 0;
}
HRESULT STDMETHODCALLTYPE CElementProvider::Notify(WORD* param) {
	NOT_IMPLEMENTED;
	return 0;
}
HRESULT STDMETHODCALLTYPE CElementProvider::OnNavigateAway() {
	NOT_IMPLEMENTED;
	return 0;
}
HRESULT STDMETHODCALLTYPE CElementProvider::OnInnerElementDestroyed() {
	NOT_IMPLEMENTED;
	return 0;
}

HRESULT STDMETHODCALLTYPE CElementProvider::OnSelectedItemChanged()
{
	NOT_IMPLEMENTED;
	return 0;
}
HRESULT STDMETHODCALLTYPE CElementProvider::OnSelectionChanged()
{
	NOT_IMPLEMENTED;
	return 0;
}
HRESULT STDMETHODCALLTYPE CElementProvider::OnContentsChanged()
{
	NOT_IMPLEMENTED;
	return 0;
}
HRESULT STDMETHODCALLTYPE CElementProvider::OnFolderChanged()
{
	NOT_IMPLEMENTED;
	return 0;
}

HRESULT STDMETHODCALLTYPE CElementProvider::QueryService(
	REFGUID guidService,
	REFIID riid,
	void** ppvObject)
{
	*ppvObject = 0;
	NOT_IMPLEMENTED;
	return E_NOTIMPL;
}

HRESULT CElementProvider::SetSite(IUnknown* punkSite)
{
	IUnknown_Set((IUnknown**)&this->Site, punkSite);
	return S_OK;
}

HRESULT CElementProvider::GetSite(REFIID riid, void** ppvSite)
{
	NOT_IMPLEMENTED;

	if (Site == NULL)
	{
		return E_FAIL;
	}
	
	return Site->QueryInterface(riid, ppvSite);
}

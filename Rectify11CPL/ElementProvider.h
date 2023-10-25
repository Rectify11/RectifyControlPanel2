/**************************************************************************
	THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.

   (c) Microsoft Corporation. All Rights Reserved.
**************************************************************************/

#include <windows.h>
#include <shlobj.h>
#include <propkey.h>
#include <shlwapi.h>
#include <strsafe.h>

#include "Rectify11CPL.h"
#include "CRectifyUtil.h"

class CElementProvider : public DirectUI::XProvider, public IDUIElementProviderInit, public IFrameNotificationClient, public IFrameShellViewClient, public IObjectWithSite, public IServiceProvider
{
public:
	CElementProvider();

	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, __out void** ppv);
	IFACEMETHODIMP_(ULONG) AddRef();
	IFACEMETHODIMP_(ULONG) Release();

	virtual long CreateDUI(DirectUI::IXElementCP* a, HWND* hwnd);

	// IDUIElementProviderInit
	virtual HRESULT STDMETHODCALLTYPE SetResourceID(UINT id);
	virtual HRESULT STDMETHODCALLTYPE OptionallyTakeInitialFocus(BOOL* result);

	//IFrameNotificationClient
	virtual HRESULT STDMETHODCALLTYPE LayoutInitialized();
	virtual HRESULT STDMETHODCALLTYPE Notify(WORD* param);
	virtual HRESULT STDMETHODCALLTYPE OnNavigateAway();
	virtual HRESULT STDMETHODCALLTYPE OnInnerElementDestroyed();

	//IFrameShellViewClient
	virtual HRESULT STDMETHODCALLTYPE OnSelectedItemChanged();
	virtual HRESULT STDMETHODCALLTYPE OnSelectionChanged();
	virtual HRESULT STDMETHODCALLTYPE OnContentsChanged();
	virtual HRESULT STDMETHODCALLTYPE OnFolderChanged();



	//IServiceProvider

	virtual HRESULT STDMETHODCALLTYPE QueryService(REFGUID guidService, REFIID riid, void** ppvObject);


	//IobjectWithSite
	virtual HRESULT STDMETHODCALLTYPE SetSite(IUnknown* pUnkSite);

	virtual HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void** ppvSite);

	void InitNavLinks();

	void InitThemeSettingPage();

private:
	IUnknown* _punkSite;
	DirectUI::XResourceProvider* resourceProvider = NULL;
	DirectUI::IXProviderCP** provider = NULL;
	~CElementProvider();
};

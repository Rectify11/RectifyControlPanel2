#pragma once
class CRectifyMainCPLPage : public CElementWithSite, public IFrameNotificationClient
{
public:
	static DirectUI::IClassInfo* Class;
	static HRESULT Create(Element* a, unsigned long* b, Element** c);

	//Element
	virtual IClassInfo* GetClassInfoW();

	//Iunknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);



	//IFrameNotificationClient implementation
	virtual HRESULT STDMETHODCALLTYPE LayoutInitialized();
	virtual HRESULT STDMETHODCALLTYPE Notify(WORD* param);
	virtual HRESULT STDMETHODCALLTYPE OnNavigateAway();
	virtual HRESULT STDMETHODCALLTYPE OnInnerElementDestroyed();

	CRectifyMainCPLPage();
	~CRectifyMainCPLPage();
};


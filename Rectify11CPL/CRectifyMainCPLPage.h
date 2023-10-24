#pragma once
class CRectifyMainCPLPage : public Element//, public IFrameNotificationClient
{
public:

	CRectifyMainCPLPage();
	virtual ~CRectifyMainCPLPage() override;

	static DirectUI::IClassInfo* Class;
	static HRESULT CreateInstance(Element* a, unsigned long* b, Element** c);

	//Element
	virtual IClassInfo* GetClassInfoW() override;

	//Iunknown
	//virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
	//virtual ULONG STDMETHODCALLTYPE AddRef(void);
	//virtual ULONG STDMETHODCALLTYPE Release(void);



	//IFrameNotificationClient implementation
	//virtual HRESULT STDMETHODCALLTYPE LayoutInitialized();
	//virtual HRESULT STDMETHODCALLTYPE Notify(WORD* param);
	//virtual HRESULT STDMETHODCALLTYPE OnNavigateAway();
	//virtual HRESULT STDMETHODCALLTYPE OnInnerElementDestroyed();

	// Element overrides
	virtual void OnEvent(Event* iev);

	static inline DirectUI::IClassInfo* GetClassInfoPtr() { return Class; }
	static inline UCString DoGetClassName() { return (UCString)L"CRectifyMainCPLPage"; }

};
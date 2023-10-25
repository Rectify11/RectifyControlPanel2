#pragma once
class RectifyThemeCfgPage : public Element
{
public:
	RectifyThemeCfgPage();
	virtual ~RectifyThemeCfgPage() override;

	static DirectUI::IClassInfo* Class;
	static HRESULT CreateInstance(Element* a, unsigned long* b, Element** c);

	//Element
	virtual IClassInfo* GetClassInfoW() override;

	// Element overrides
	virtual void OnEvent(Event* iev) override;

	// Important methods
	virtual void OnInit();
	virtual void SetSite(IUnknown* site) { this->site = site; }

	static inline DirectUI::IClassInfo* GetClassInfoPtr() { return Class; }
	static inline UCString DoGetClassName() { return (UCString)L"RectifyThemeCfgPage"; }

	void GoBack();
 private:
	IUnknown* site = NULL;
	bool initializing = true;
};


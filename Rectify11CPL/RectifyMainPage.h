#pragma once
#include <map>
#include "IRectifyUtil_h.h"
typedef std::map<int, wstring> ThemesMapBase;

class RectifyMainPage : public Element
{
public:

	RectifyMainPage();
	virtual ~RectifyMainPage() override;

	static DirectUI::IClassInfo* Class;
	static HRESULT CreateInstance(Element* a, unsigned long* b, Element** c);

	//Element
	virtual IClassInfo* GetClassInfoW() override;

	// Element overrides
	virtual void OnEvent(Event* iev) override;
	virtual void OnDestroy() override;

	// Important methods
	virtual void OnInit();
	virtual void SetSite(IUnknown* site) { this->site = site;  }

	static inline DirectUI::IClassInfo* GetClassInfoPtr() { return Class; }
	static inline UCString DoGetClassName() { return (UCString)L"RectifyMainPage"; }
private:
	bool HasAdmin = false;
	IRectifyUtil* RectifyUtil = NULL;
	vector<ULONG> themes;
	ThemesMapBase ThemesMap;
	IUnknown* site = NULL;
	bool initializing = true;

	void UpdateThemeGraphic();
	void ShowRestartExplorer();
	void InitNavLinks();
	HWND GetMainHwnd();
};
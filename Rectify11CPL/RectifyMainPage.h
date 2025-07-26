#pragma once
#include <map>
#include "IRectifyUtil_h.h"
typedef std::map<int, wstring> ThemesMapBase;

class RectifyMainPage : public Element
{
public:

	RectifyMainPage();
	virtual ~RectifyMainPage() override;

	static HRESULT Create(Element* a, unsigned long* b, Element** c);

	//Element
	virtual IClassInfo* GetClassInfoW() override;

	// Element overrides
	virtual void OnEvent(Event* iev) override;
	virtual void OnDestroy() override;

	// Important methods
	virtual void OnInit();
	virtual void SetSite(IUnknown* site) { this->site = site;  }

	static inline DirectUI::IClassInfo* GetClassInfoPtr() { return Class; }
	static inline const wchar_t* DoGetClassName() { return (const wchar_t*)L"RectifyMainPage"; }
	static void WINAPI SetClassInfoPtr(IClassInfo* ptr) { Class = ptr; };
	static HRESULT Register();
private:
	bool HasAdmin = false;
	IRectifyUtil* RectifyUtil = NULL;
	vector<ULONG> themes;
	ThemesMapBase ThemesMap;
	IUnknown* site = NULL;
	bool initializing = true;

	static DirectUI::IClassInfo* Class;

	void UpdateThemeGraphic();
	void ShowRestartExplorer();
	void InitNavLinks();
	void UpdateThemetoolStatus();
	HWND GetMainHwnd();
};
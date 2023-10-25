#pragma once
#include <map>
#include "IRectifyUtil_h.h"
typedef std::map<int, wstring> ThemesMapBase;

class CRectifyMainCPLPage : public Element
{
public:

	CRectifyMainCPLPage();
	virtual ~CRectifyMainCPLPage() override;

	static DirectUI::IClassInfo* Class;
	static HRESULT CreateInstance(Element* a, unsigned long* b, Element** c);

	//Element
	virtual IClassInfo* GetClassInfoW() override;

	// Element overrides
	virtual void OnEvent(Event* iev) override;
	virtual void OnDestroy() override;

	// Important methods
	virtual void OnInit();

	static inline DirectUI::IClassInfo* GetClassInfoPtr() { return Class; }
	static inline UCString DoGetClassName() { return (UCString)L"CRectifyMainCPLPage"; }
private:
	bool HasAdmin;
	IRectifyUtil* RectifyUtil;
	vector<ULONG> themes;
	ThemesMapBase ThemesMap;
	bool initializing = true;

	void UpdateThemeGraphic();
	void ShowRestartExplorer();
};
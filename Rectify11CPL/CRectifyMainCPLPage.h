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
	virtual void OnEvent(Event* iev);

	// Important methods
	virtual void OnInit();
	virtual void OnNavigateAway();

	static inline DirectUI::IClassInfo* GetClassInfoPtr() { return Class; }
	static inline UCString DoGetClassName() { return (UCString)L"CRectifyMainCPLPage"; }
private:
	bool HasAdmin;
	IRectifyUtil* RectifyUtil;
	static vector<ULONG> themes;
	static ThemesMapBase ThemesMap;

	void UpdateThemeGraphic();
};
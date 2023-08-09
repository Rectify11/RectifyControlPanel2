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

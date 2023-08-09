#include "pch.h"
#include "CElementWithSite.h"
#include "CElementWithIUnknown.h"
using namespace DirectUI;
IClassInfo* CElementWithSite::Class = NULL;

HRESULT CElementWithSite::Register(HINSTANCE hInstance, unsigned short const* unknown, bool unknown2, PropertyInfo const* const* unknown3, unsigned int unknown4)
{
	int hr = CElementWithIUnknown::Register(hInstance, unknown, unknown2, unknown3, (void**)unknown4);
	if (SUCCEEDED(hr))
	{

	}
	return hr;
}

#include "CElementWithSite.h"
#include "types.h"
#include "directui.h"
using namespace DirectUI;
IClassInfo* CElementWithSite::Class = NULL;


HRESULT RegisterCElementWithIUnknown()
{
	
}

HRESULT CElementWithSite::Register()
{
	int hr = RegisterCElementWithIUnknown();
	if (SUCCEEDED(hr))
	{

	}
	return hr;
}

#include "CElementWithIUnknown.h"
#include "types.h"
#include "directui.h"
using namespace DirectUI;
IClassInfo* CElementWithIUnknown::Class = NULL;

//TODO: reverse engineer
//NOTE: This is a templete
HRESULT CElementWithIUnknown::Register()
{
	HRESULT hr = S_OK;
	if (DirectUI::Element::GetClassInfoPtr() == NULL)
	{
		hr = DirectUI::Element::Register();
	}
	else
	{
		DirectUI::Element::GetClassInfoPtr()->AddRef();
	}
	if (SUCCEEDED(hr))
	{
		auto ClassPointer = DirectUI::Element::GetClassInfoPtr();
		auto lock = DirectUI::Element::GetFactoryLock();
		auto lock2 = new DirectUI::CritSecLock(lock);
		if (DirectUI::ClassInfoBase::ClassExist(&ClassPointer, NULL, 0, ClassPointer, GetModuleHandle(NULL), (unsigned short const*)L"CElementWithIUnknown", FALSE))
		{
			;
		}
		else
		{
			CElementWithIUnknown::Class = NULL;

		}
	}
	return hr;
}
#include <windows.h>
#include "CElementWithIUnknown.h"

using namespace DirectUI;
IClassInfo* CElementWithIUnknown::Class = NULL;

//TODO: reverse engineer
//NOTE: This is a templete
HRESULT CElementWithIUnknown::Create(HINSTANCE hInstance, unsigned short const* unknown, bool unknown2, PropertyInfo const* const* unknown3, UINT flags, void** Resullt)
{
	HRESULT hr = S_OK;
	//auto ba = new ClassInfoBase();
	//hr = ba->Initialize(hInstance, (unsigned short const*)L"CElementWithIUnknown", false, NULL, 0);
	if (SUCCEEDED(hr))
	{
		//*Resullt = (void*)ba;
	}
	else
	{
		//todo: delete on failure
	}
	return hr;
}
HRESULT CElementWithIUnknown::Register(HINSTANCE hInstance, unsigned short const* unknown, bool unknown2, PropertyInfo const* const* unknown3, void** Resullt)
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
		auto ElementPointer = DirectUI::Element::GetClassInfoPtr();
		auto lock = DirectUI::Element::GetFactoryLock();
		auto lock2 = new DirectUI::CritSecLock(lock);
		if (DirectUI::ClassInfoBase::ClassExist(&ElementPointer, NULL, 0, ElementPointer, GetModuleHandle(NULL), (unsigned short const*)L"CElementWithIUnknown", FALSE))
		{
			CElementWithIUnknown::Class = ElementPointer;
		}
		else
		{
			CElementWithIUnknown::Class = NULL;
			hr = Create(hInstance, NULL, false, (PropertyInfo**)ElementPointer, 0x80000000, (void**)unknown3);
			if (SUCCEEDED(hr))
			{
				//hr = ElementPointer->Register();
				if (hr < 0) {
					//hr = ElementPointer->Initialize();
					return hr;
				}
				//CElementWithIUnknown::Class = classinfoptr;
			}
		}
	}
	return hr;
}
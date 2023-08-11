#include <windows.h>
#include "pch.h"
#include "CElementWithIUnknown.h"

using namespace DirectUI;
IClassInfo* CElementWithIUnknown::Class = NULL;
typedef IClassInfo*(*minfunc)() ;


IUnknown* CElementWithIUnknown::GetUnknownFromElement(Element* t)
{
	IClassInfo* classInfo = t->GetClassInfoPtr();

	//check if it inherits CElementWithIUnknown
	while (true)
	{
		if (classInfo == NULL)
		{
			return NULL;
		}
		LPCWCH name = (LPCWCH)classInfo->GetName();
		LPCWCH name2 = (LPCWCH)CElementWithIUnknown::Class->GetName();
		

		if (CompareStringOrdinal(name, -1, name2, -1, false) == CSTR_EQUAL)
		{
			break;
		}
		IClassInfo* newClass = classInfo->GetBaseClass();
		classInfo = newClass;
	}

	return (IUnknown*)(t + 200);
}
//
//IClassInfo* WINAPI CElementWithIUnknown::GetBaseClass()
//{
//	return Element::GetClassInfoPtr();
//}
//
//HRESULT WINAPI CElementWithIUnknown::CreateInstance(Element*, unsigned long*, Element**)
//{
//	MessageBox(NULL, TEXT("CreateInstance called"), TEXT(""), 0);
//	return -0x7fffbfff;
//}
//
////TODO: reverse engineer
////NOTE: This is a templete
//HRESULT CElementWithIUnknown::Create(HINSTANCE hInstance, unsigned short const* unknown, bool unknown2, PropertyInfo const* const* unknown3, UINT flags, CElementWithIUnknown** Resullt)
//{
//	HRESULT hr = S_OK;
//	auto ba = new CElementWithIUnknown();
//	hr = ba->Initialize(hInstance, (unsigned short const*)L"CElementWithIUnknown", false, NULL, 0);
//	if (SUCCEEDED(hr))
//	{
//		*Resullt = ba;
//	}
//	else
//	{
//		//todo: delete on failure
//		
//	}
//	return hr;
//}
//HRESULT CElementWithIUnknown::Register(HINSTANCE hInstance, unsigned short const* name, bool unknown2, PropertyInfo const* const* unknown3, UINT u, BOOL b)
//{
//		
//	HRESULT hr = S_OK;
//	if (DirectUI::Element::GetClassInfoPtr() == NULL)
//	{
//		hr = DirectUI::Element::Register();
//	}
//	else
//	{
//		DirectUI::Element::GetClassInfoPtr()->AddChild();
//	}
//	if (SUCCEEDED(hr))
//	{
//		auto ElementPointer = DirectUI::Element::GetClassInfoPtr();
//		auto lock = DirectUI::Element::GetFactoryLock();
//		auto lock2 = new DirectUI::CritSecLock(lock);
//		if (DirectUI::ClassInfoBase::ClassExist(&ElementPointer, NULL, 0, ElementPointer, GetModuleHandle(NULL), (unsigned short const*)L"CElementWithIUnknown", FALSE))
//		{
//			CElementWithIUnknown::Class = ElementPointer;
//		}
//		else
//		{
//			CElementWithIUnknown::Class = NULL;
//			CElementWithIUnknown* ptr;
//			hr = Create(hInstance, NULL, false, (PropertyInfo**)ElementPointer, 0x80000000, &ptr);
//			if (SUCCEEDED(hr))
//			{
//				hr = ((ClassInfoBase*)ptr)->Register();
//				if (hr < 0) {
//					hr = ((ClassInfoBase*)ptr)->Initialize((HINSTANCE)ptr, NULL, false, (PropertyInfo const* const*)ElementPointer, 0x80000000);
//					return hr;
//				}
//				CElementWithIUnknown::Class = (IClassInfo*)(ClassInfoBase*)ptr;
//			}
//		}
//	}
//	return hr;
//}
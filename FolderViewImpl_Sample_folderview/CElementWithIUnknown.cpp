#include <windows.h>
#include "pch.h"
#include "CElementWithIUnknown.h"

using namespace DirectUI;
IClassInfo* CElementWithIUnknown::Class = NULL;

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
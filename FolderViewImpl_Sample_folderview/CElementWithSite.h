#pragma once
#include <windows.h>
#include "directui.h"
class CElementWithSite
{
public:
	static HRESULT Register();
	static DirectUI::IClassInfo* Class;
};

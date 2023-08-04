#pragma once
#include "types.h"
#include "directui.h"
using namespace DirectUI;
class CElementWithIUnknown
{
	static HRESULT Register();
	static IClassInfo* Class;
};


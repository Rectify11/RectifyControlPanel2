#pragma once
#include "pch.h"
using namespace DirectUI;
class CElementWithIUnknown : public DirectUI::ClassInfoBase
{
public:
	static HRESULT Create(HINSTANCE hInstance, unsigned short const* unknown, bool unknown2, PropertyInfo const* const* unknown3, UINT flags, void** result);
	static HRESULT Register(HINSTANCE, unsigned short const*, bool, PropertyInfo const* const*, void** result);

	static IClassInfo* Class;
};


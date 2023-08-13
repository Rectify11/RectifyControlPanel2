#pragma once
using namespace DirectUI;
class CElementWithIUnknown
{
public:
	static IUnknown* GetUnknownFromElement(Element* l);
	static IClassInfo* Class;
};


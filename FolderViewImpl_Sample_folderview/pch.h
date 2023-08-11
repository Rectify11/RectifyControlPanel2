//Core APIS
#pragma once
#include <windows.h>
#include <shlwapi.h>
#include <objbase.h>
#include <Shlobj.h>
#include <olectl.h>
#include <strsafe.h>
#include "undoc.h"

extern HINSTANCE g_hInst;

//DirectUI
#pragma comment(lib,"dui70.lib")
#include "..\dui70\DirectUI\DirectUI.h"
using namespace DirectUI;
#include "Utils.h"
#include "CElementWithIUnknown.h"
#include "CElementWithSite.h"
#include "CRectifyMainCPLPage.h"



namespace DirectUI {

#define DEFINE_DIRECTUI_CLASS_ELEMENTBASE(name) class ClassInfo_##name## : ClassInfoBase \
	{ \
public: \
	static HRESULT Create(HINSTANCE hInstance, unsigned short const* unknown, bool unknown2, PropertyInfo const* const* unknown3, UINT flags, ClassInfo_##name##** Resullt) \
{ \
	HRESULT hr = S_OK; \
	ClassInfo_##name##* ba = new ClassInfo_##name##(); \
	hr = ba->Initialize(hInstance, (unsigned short const*)L#name, false, NULL, 0); \
	if (SUCCEEDED(hr)) \
	{\
		*Resullt = ba;\
	}\
	else\
	{\
	}\
	return hr;\
}\
static HRESULT Register(HINSTANCE hInstance, unsigned short const* unknownname, bool unknown2, PropertyInfo const* const* unknown3, UINT u, BOOL b)\
{\
	HRESULT hr = S_OK;\
	if (DirectUI::Element::GetClassInfoPtr() == NULL)\
	{\
		hr = DirectUI::Element::Register();\
	}\
	else\
	{\
		DirectUI::Element::GetClassInfoPtr()->AddRef();\
	}\
	if (SUCCEEDED(hr))\
	{\
		auto ElementPointer = DirectUI::Element::GetClassInfoPtr();\
		auto lock = DirectUI::Element::GetFactoryLock();\
		auto lock2 = new DirectUI::CritSecLock(lock);\
		if (DirectUI::ClassInfoBase::ClassExist(&ElementPointer, NULL, 0, ElementPointer, g_hInst, (unsigned short const*)L#name, FALSE))\
		{\
			 ##name##::Class = ElementPointer;\
		}\
		else\
		{\
			##name##::Class = NULL;\
			ClassInfo_##name##* ptr;\
			hr = Create(hInstance, NULL, false, (PropertyInfo**)ElementPointer, 0x80000000, &ptr);\
			if (SUCCEEDED(hr))\
			{\
				hr = ((ClassInfoBase*)ptr)->Register();\
				if (hr < 0) {\
					hr = ((ClassInfoBase*)ptr)->Initialize((HINSTANCE)ptr, NULL, false, (PropertyInfo const* const*)ElementPointer, 0x80000000);\
					return hr;\
				}\
				##name##::Class = (IClassInfo*)(ClassInfoBase*)ptr;\
			}\
		}\
	}\
}\
HRESULT WINAPI CreateInstance(Element*, unsigned long*, Element**)\
{\
	return E_NOTIMPL;\
}\
IClassInfo* WINAPI GetBaseClass()\
{\
	return Element::GetClassInfoPtr();\
}\
	} \

#define DEFINE_DIRECTUI_CLASS_CUSTOMBASE(name, baseClass) class ClassInfo_##name## : ClassInfoBase \
	{ \
public: \
	static HRESULT Create(HINSTANCE hInstance, unsigned short const* unknown, bool unknown2, PropertyInfo const* const* unknown3, UINT flags, ClassInfo_##name##** Resullt) \
{ \
	HRESULT hr = S_OK; \
	ClassInfo_##name##* ba = new ClassInfo_##name##(); \
	hr = ba->Initialize(hInstance, (unsigned short const*)L#name, false, NULL, 0); \
	if (SUCCEEDED(hr)) \
	{\
		*Resullt = ba;\
	}\
	else\
	{\
	}\
	return hr;\
}\
static HRESULT Register(HINSTANCE hInstance, unsigned short const* unknownname, bool unknown2, PropertyInfo const* const* unknown3, UINT u, BOOL b)\
{\
	HRESULT hr = S_OK;\
	if (##baseClass##::Class == NULL)\
	{\
		ClassInfo_##baseClass##::Register(NULL, (const unsigned short*)g_hInst, FALSE, NULL,0,0);\
	}\
	else\
	{\
		##baseClass##::Class->AddRef();\
	}\
	if (SUCCEEDED(hr))\
	{\
		auto ElementPointer = ##baseClass##::Class;\
		auto lock = DirectUI::Element::GetFactoryLock();\
		auto lock2 = new DirectUI::CritSecLock(lock);\
		if (DirectUI::ClassInfoBase::ClassExist(&ElementPointer, NULL, 0, ElementPointer, g_hInst, (unsigned short const*)L#name, FALSE))\
		{\
			 ##name##::Class = ElementPointer;\
		}\
		else\
		{\
			##name##::Class = NULL;\
			ClassInfo_##name##* ptr;\
			hr = Create(hInstance, NULL, false, (PropertyInfo**)ElementPointer, 0x80000000, &ptr);\
			if (SUCCEEDED(hr))\
			{\
				hr = ((ClassInfoBase*)ptr)->Register();\
				if (hr < 0) {\
					hr = ((ClassInfoBase*)ptr)->Initialize((HINSTANCE)ptr, NULL, false, (PropertyInfo const* const*)ElementPointer, 0x80000000);\
					return hr;\
				}\
				##name##::Class = (IClassInfo*)(ClassInfoBase*)ptr;\
			}\
		}\
	}\
}\
HRESULT WINAPI CreateInstance(Element*, unsigned long*, Element**)\
{\
	return E_NOTIMPL;\
}\
IClassInfo* WINAPI GetBaseClass()\
{\
	return baseClass::Class;\
}\
	}

#define DEFINE_DIRECTUI_CLASS_CUSTOM_BASE_WITH_CLASS(name, baseClass) class ClassInfo_##name## : ClassInfoBase \
	{ \
public: \
	static HRESULT Create(HINSTANCE hInstance, unsigned short const* unknown, bool unknown2, PropertyInfo const* const* unknown3, UINT flags, ClassInfo_##name##** Resullt) \
{ \
	HRESULT hr = S_OK; \
	ClassInfo_##name##* ba = new ClassInfo_##name##(); \
	hr = ba->Initialize(hInstance, (unsigned short const*)L#name, false, NULL, 0); \
	if (SUCCEEDED(hr)) \
	{\
		*Resullt = ba;\
	}\
	else\
	{\
	}\
	return hr;\
}\
static HRESULT Register(HINSTANCE hInstance, unsigned short const* unknownname, bool unknown2, PropertyInfo const* const* unknown3, UINT u, BOOL b)\
{\
	HRESULT hr = S_OK;\
	if (##baseClass##::Class == NULL)\
	{\
		ClassInfo_##baseClass##::Register(NULL, (const unsigned short*)g_hInst, FALSE, NULL,0,0);\
	}\
	else\
	{\
		##baseClass##::Class->AddRef();\
	}\
	if (SUCCEEDED(hr))\
	{\
		auto ElementPointer = ##baseClass##::Class;\
		auto lock = DirectUI::Element::GetFactoryLock();\
		auto lock2 = new DirectUI::CritSecLock(lock);\
		if (DirectUI::ClassInfoBase::ClassExist(&ElementPointer, NULL, 0, ##baseClass##::Class, g_hInst, (unsigned short const*)L#name, FALSE))\
		{\
			 ##name##::Class = ElementPointer;\
		}\
		else\
		{\
			##name##::Class = NULL;\
			ClassInfo_##name##* ptr;\
			hr = Create(hInstance, NULL, false, (PropertyInfo**)ElementPointer, 0x80000000, &ptr);\
			if (SUCCEEDED(hr))\
			{\
				hr = ((ClassInfoBase*)ptr)->Register();\
				if (hr < 0) {\
					hr = ((ClassInfoBase*)ptr)->Initialize((HINSTANCE)ptr, NULL, false, (PropertyInfo const* const*)ElementPointer, 0x80000000);\
					return hr;\
				}\
				##name##::Class = (IClassInfo*)(ClassInfoBase*)ptr;\
			}\
		}\
	}\
}\
HRESULT WINAPI CreateInstance(Element* a, unsigned long* b, Element** c)\
{\
	return name::Create(a, b, c);\
}\
IClassInfo* WINAPI GetBaseClass()\
{\
	return baseClass::Class;\
}\
	}

	DEFINE_DIRECTUI_CLASS_ELEMENTBASE(CElementWithIUnknown);
	DEFINE_DIRECTUI_CLASS_CUSTOMBASE(CElementWithSite, CElementWithIUnknown);
	DEFINE_DIRECTUI_CLASS_CUSTOM_BASE_WITH_CLASS(CRectifyMainCPLPage, CElementWithSite);
}
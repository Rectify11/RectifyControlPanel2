/**************************************************************************
	THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.

   (c) Microsoft Corporation. All Rights Reserved.
**************************************************************************/

#include <windows.h>
#include <shlobj.h>
#include <propkey.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <shellapi.h>

#include "FolderViewImpl.h"
#include "resource.h"
#include "Utils.h"
#include "ShellFolder.h"
#include "Category.h"
#include "Guid.h"
#include "undoc.h"

const int g_nMaxLevel = 5;

CFolderViewImplFolder::CFolderViewImplFolder(UINT nLevel) : m_cRef(1), m_nLevel(nLevel), m_pidl(NULL)
{
	DllAddRef();
	ZeroMemory(m_rgNames, sizeof(m_rgNames));
}

CFolderViewImplFolder::~CFolderViewImplFolder()
{
	CoTaskMemFree(m_pidl);

	for (int i = 0; i < ARRAYSIZE(m_rgNames); i++)
	{
		CoTaskMemFree(m_rgNames[i]);
	}

	DllRelease();
}

HRESULT CFolderViewImplFolder::QueryInterface(__in REFIID riid, __deref_out void** ppv)
{
	static const QITAB qit[] =
	{
	   QITABENT(CFolderViewImplFolder, IFrameNotificationClient),
	   { 0 },
	};
	HRESULT hr = QISearch(this, qit, riid, ppv);;
	if (hr != S_OK)
	{
		char szGuid[40] = { 0 };

		sprintf(szGuid, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", riid.Data1, riid.Data2, riid.Data3, riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6], riid.Data4[7]);

		MessageBox(NULL, szGuid, TEXT("Unknown interface in CFolderViewImplFolder::QueryInterface()"), 0);
	}
	return hr;
}

ULONG CFolderViewImplFolder::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

ULONG CFolderViewImplFolder::Release()
{
	ULONG cRef = InterlockedDecrement(&m_cRef);
	if (0 == cRef)
	{
		delete this;
	}
	return cRef;
}
/**************************************************************************
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

(c) Microsoft Corporation. All Rights Reserved.
**************************************************************************/

#pragma once
HRESULT LoadFolderViewImplDisplayString(UINT uIndex, __out_ecount(cch) PWSTR psz, UINT cch);
STDAPI StringToStrRetW(LPCWSTR pszName, STRRET *pStrRet);

#ifndef ResultFromShort
#define ResultFromShort(i)      MAKE_HRESULT(SEVERITY_SUCCESS, 0, (USHORT)(i))
#endif

typedef void (*DUI_Callback)(IUnknown* elem, void* data);
void DUI_WalkIUnknownElements(Element* root, DUI_Callback cb, void* data);
HRESULT DUI_SetSiteOnUnknown(IUnknown* punk2, void* punkSite2);
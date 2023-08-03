/**************************************************************************
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

(c) Microsoft Corporation. All Rights Reserved.
**************************************************************************/

#pragma once
HRESULT LoadFolderViewImplDisplayString(UINT uIndex, __out_ecount(cch) PWSTR psz, UINT cch);
HRESULT LoadFolderViewImplDisplayStrings(WCHAR** wszArrStrings, UINT cArray);
STDAPI StringToStrRetW(LPCWSTR pszName, STRRET *pStrRet);

#ifndef ResultFromShort
#define ResultFromShort(i)      MAKE_HRESULT(SEVERITY_SUCCESS, 0, (USHORT)(i))
#endif


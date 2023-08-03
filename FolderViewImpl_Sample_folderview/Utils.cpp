/**************************************************************************
    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.
 
   (c) Microsoft Corporation. All Rights Reserved.
**************************************************************************/

#include <windows.h>
#include <shlobj.h>
#include "FolderViewImpl.h"
#include "resource.h"
#include "Utils.h"
#include "shlwapi.h"

HRESULT LoadFolderViewImplDisplayString(UINT uIndex, __out_ecount(cch) PWSTR psz, UINT cch)
{
    UINT uString = 0;
    switch (uIndex)
    {
    case 0:
        uString = IDS_ZERO;
        break;
    case 1:
        uString = IDS_ONE;
        break;
    case 2:
        uString = IDS_TWO;
        break;
    case 3:
        uString = IDS_THREE;
        break;
    case 4:
        uString = IDS_FOUR;
        break;
    case 5:
        uString = IDS_FIVE;
        break;
    case 6:
        uString = IDS_SIX;
        break;
    case 7:
        uString = IDS_SEVEN;
        break;
    case 8:
        uString = IDS_EIGHT;
        break;
    case 9:
        uString = IDS_NINE;
        break;
    default:
        uString = 0;
    }

    HRESULT hr = E_FAIL;
    if (uString)
    {
        if (LoadStringW(g_hInst, uString, psz, cch))
        {
            hr = S_OK;
        }
    }
    return hr;
}

HRESULT LoadFolderViewImplDisplayStrings(WCHAR* wszArrStrings[], UINT cArray)
{
    HRESULT hr = E_FAIL;
    for (UINT i = 0; SUCCEEDED(hr) && i < cArray; i++)
    {
        WCHAR wszTemp[MAX_PATH];
        hr = LoadFolderViewImplDisplayString(i, wszTemp, ARRAYSIZE(wszTemp));
        if (SUCCEEDED(hr))
        {
            hr = SHStrDupW(wszTemp, &wszArrStrings[i]);
            if (FAILED(hr))
            {
                // Free those already allocated.
                for (UINT k = 0; k < i; k++)
                {
                    CoTaskMemFree(wszArrStrings[k]);
                    wszArrStrings[k] = NULL;
                }
                
                break;
            }
        }
        else
        {
            // Somebody tried increasing the size of the array without 
            // adding additional strings.
            wszArrStrings[i] = NULL;
        }
    }
    
    return hr;
}

STDAPI StringToStrRetW(LPCWSTR pszName, STRRET *pStrRet)
{
    pStrRet->uType = STRRET_WSTR;
    return SHStrDupW(pszName, &pStrRet->pOleStr);
}

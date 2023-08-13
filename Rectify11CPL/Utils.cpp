#include <windows.h>
#include <shlobj.h>
#include "Rectify11CPL.h"
#include "resource.h"
#include "shlwapi.h"
#include "pch.h"

HRESULT DUI_SetSiteOnUnknown(IUnknown* punk, void* punkSite2)
{
	IUnknown* punkSite = (IUnknown*)punkSite2;
	return IUnknown_SetSite(punk, punkSite);
}

void DUI_WalkIUnknownElements(Element* root, DUI_Callback cb, void* data)
{
	if (root == NULL)
	{
		return;
	}
	Value* children = NULL;
	DynamicArray<Element*, 0>* childs = root->GetChildren(&children);
	int i = 0;
	if (childs != NULL)
	{
		UINT flags = *(UINT*)childs;
		int cap = (flags & 0xfffffff);
		Element* dataPtr;
		if (cap != 0)
		{
			while (i < cap)
			{
				if ((flags & 0x10000000) == 0)
				{
					dataPtr = childs->classs[i];//(Element*)(childs + 2);
				}
				else
				{
					dataPtr = (Element*)*(void**)(childs + 1);
				}
				Element* theElement = &dataPtr[2 * i];
				IUnknown* unknown = CElementWithIUnknown::GetUnknownFromElement(theElement);
				if (unknown != NULL)
				{
					cb(unknown, data);
				}
				DUI_WalkIUnknownElements(theElement, cb, data);
				flags = *(UINT*)childs;
				i++;
				return;
			}
		}
	}


}

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

STDAPI StringToStrRetW(LPCWSTR pszName, STRRET* pStrRet)
{
	pStrRet->uType = STRRET_WSTR;
	return SHStrDupW(pszName, &pStrRet->pOleStr);
}

/**************************************************************************
    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.
 
   (c) Microsoft Corporation. All Rights Reserved.
**************************************************************************/

#include <shlobj.h>
#include <shlwapi.h>
#include "Utils.h"
#include "ShellFolder.h"
#include "EnumIDList.h"
#include <stdio.h>
#include <strsafe.h>

CFolderViewImplEnumIDList::CFolderViewImplEnumIDList(DWORD grfFlags, int nLevel, CFolderViewImplFolder *pFolderViewImplShellFolder) :
    m_cRef(1), m_grfFlags(grfFlags), m_nLevel(nLevel), m_nItem(0), m_pFolder(pFolderViewImplShellFolder)
{
    m_pFolder->AddRef();
}

CFolderViewImplEnumIDList::~CFolderViewImplEnumIDList()
{
    m_pFolder->Release();
}

HRESULT CFolderViewImplEnumIDList::QueryInterface(__in REFIID riid, __deref_out void **ppv)
{
    static const QITAB qit[] = {
        QITABENT (CFolderViewImplEnumIDList, IEnumIDList),
        { 0 },
    };
    return QISearch(this, qit, riid, ppv);
}

DWORD CFolderViewImplEnumIDList::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

DWORD CFolderViewImplEnumIDList::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef)
    {
        delete this;
    }

    return cRef;
}

// This initializes the enumerator.  In this case we set up a default array of items, this represents our
// data source.  In a real-world implementation the array would be replaced and would be backed by some
// other data source that you traverse and convert into IShellFolder item IDs.
HRESULT CFolderViewImplEnumIDList::Initialize()
{
    ZeroMemory(m_aData, sizeof(m_aData));

    HRESULT hr = S_OK;
    for (int i = 0; SUCCEEDED(hr) && i < ARRAYSIZE(m_aData); i++)
    {
        hr = LoadFolderViewImplDisplayString(i, m_aData[i].szName, ARRAYSIZE(m_aData[i].szName));
        if (SUCCEEDED(hr))
        {
            m_aData[i].nSize      = (BYTE)(rand() % MAXBYTE);
            m_aData[i].nSides     = (BYTE)(rand() % 6);
            while (1 == m_aData[i].nSides || 2 == m_aData[i].nSides)
            {
                m_aData[i].nSides = (BYTE)(rand() % 6);
            }

            m_aData[i].fIsFolder  = (BOOL)(i % 2);
        }
    }
    return hr;
}

// Retrieves the specified number of item identifiers in 
// the enumeration sequence and advances the current position 
// by the number of items retrieved. 
HRESULT CFolderViewImplEnumIDList::Next(ULONG celt, 
                                        __out_ecount_part(celt, *pceltFetched) PITEMID_CHILD *rgelt, 
                                        __out_opt ULONG *pceltFetched)
{
    int celtFetched = 0;
   
    HRESULT hr = (pceltFetched || celt <= 1) ? S_OK : E_INVALIDARG;
    if (SUCCEEDED(hr))
    {
        ULONG i = 0;
        while (SUCCEEDED(hr) && i < celt && m_nItem < ARRAYSIZE(m_aData))
        {
            BOOL fSkip = FALSE;
            if (!(m_grfFlags & SHCONTF_STORAGE))
            {
                if (m_aData[m_nItem].fIsFolder)
                {
                    if (!(m_grfFlags & SHCONTF_FOLDERS))
                    {
                        // this is a folder, but caller doesnt want folders
                        fSkip = TRUE;
                    }
                }
                else
                {
                    if (!(m_grfFlags & SHCONTF_NONFOLDERS))
                    {
                        // this is a file, but caller doesnt want files
                        fSkip = TRUE;
                    }
                }
            }
            
            if (!fSkip)
            {
                hr = m_pFolder->CreateChildID(m_aData[m_nItem].szName, m_nLevel, m_aData[m_nItem].nSize, m_aData[m_nItem].nSides, m_aData[m_nItem].fIsFolder, &rgelt[i]);
                if (SUCCEEDED(hr))
                {
                    celtFetched++;
                    i++;
                }
            }

            m_nItem++;
        }
    }

    if (pceltFetched)
    {
        *pceltFetched = celtFetched;
    }

    return (celtFetched == celt) ? S_OK : S_FALSE;
}

HRESULT CFolderViewImplEnumIDList::Skip(DWORD celt)
{
    m_nItem += celt;
    return S_OK;
}

HRESULT CFolderViewImplEnumIDList::Reset()
{
    m_nItem = 0;
    return S_OK;
}

HRESULT CFolderViewImplEnumIDList::Clone(__deref_out IEnumIDList **ppenum)
{
    // this method is rarely used and it's acceptable to not implement it.
    return E_NOTIMPL;
}

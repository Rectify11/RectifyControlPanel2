/**************************************************************************
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

(c) Microsoft Corporation. All Rights Reserved.
**************************************************************************/

#pragma once

class CFolderViewImplCategoryProvider : public ICategoryProvider
{
public:
    CFolderViewImplCategoryProvider(IShellFolder2 *psf);

    // IUnknown methods
    IFACEMETHODIMP QueryInterface(__in REFIID riid, __deref_out void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // ICategoryProvider methods.
    IFACEMETHODIMP CanCategorizeOnSCID(const PROPERTYKEY *key);
    IFACEMETHODIMP GetDefaultCategory(__out GUID* pguid, __out PROPERTYKEY *key);
    IFACEMETHODIMP GetCategoryForSCID(const PROPERTYKEY *key, __out GUID *pguid);
    IFACEMETHODIMP EnumCategories(__out IEnumGUID** penum);
    IFACEMETHODIMP GetCategoryName(const GUID* pguid, __out_ecount(cch) PWSTR pszName, UINT cch);
    IFACEMETHODIMP CreateCategory(const GUID* pguid, REFIID riid, __out void** ppv);

private:
    ~CFolderViewImplCategoryProvider();
    LONG            m_cRef;
    IShellFolder2  *m_psf;
};

class CFolderViewImplEnumGUID : public IEnumGUID
{
public:
    CFolderViewImplEnumGUID();

    // IUnknown methods
    IFACEMETHODIMP QueryInterface(__in REFIID riid, __deref_out void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IEnumGUID
    IFACEMETHODIMP Next(ULONG celt, 
                        __out_ecount_part(celt, *pceltFetched) GUID *rgelt, 
                        __out_opt ULONG *pceltFetched);
    IFACEMETHODIMP Skip(ULONG);
    IFACEMETHODIMP Reset();     
    IFACEMETHODIMP Clone(__deref_out IEnumGUID**);

private:
    ~CFolderViewImplEnumGUID();
    LONG    m_cRef;
    ULONG   m_ulCurrentIndex;
};

class CFolderViewImplCategorizer_Name : public ICategorizer
{
public:
    CFolderViewImplCategorizer_Name(IShellFolder2 *psf);

    // IUnknown methods
    IFACEMETHODIMP QueryInterface(__in REFIID riid, __deref_out void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // ICategorizer
    IFACEMETHODIMP GetDescription(__out_ecount(cch) PWSTR pszDesc, UINT cch);
    IFACEMETHODIMP GetCategory(UINT cidl, 
                               __in_ecount(cidl) PCUITEMID_CHILD_ARRAY apidl, 
                               __out_ecount(cidl) DWORD *rgCategoryIds);
    IFACEMETHODIMP GetCategoryInfo(DWORD dwCategoryId, __out CATEGORY_INFO *pci);
    IFACEMETHODIMP CompareCategory(CATSORT_FLAGS csfFlags, 
                                   DWORD dwCategoryId1, 
                                   DWORD dwCategoryId2);

private:
    ~CFolderViewImplCategorizer_Name();
    LONG            m_cRef;
    IShellFolder2  *m_psf;
};

class CFolderViewImplCategorizer_Size : public ICategorizer
{
public:
    CFolderViewImplCategorizer_Size(IShellFolder2 *psf);

    // IUnknown
    IFACEMETHODIMP QueryInterface(__in REFIID riid, __deref_out void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();
    
    // ICategorizer
    IFACEMETHODIMP GetDescription(__out_ecount(cch) PWSTR pszDesc, UINT cch);
    IFACEMETHODIMP GetCategory(UINT cidl, 
                               __in_ecount(cidl) PCUITEMID_CHILD_ARRAY apidl, 
                               __out_ecount(cidl) DWORD *rgCategoryIds);
    IFACEMETHODIMP GetCategoryInfo(DWORD dwCategoryId, __out CATEGORY_INFO *pci);
    IFACEMETHODIMP CompareCategory(CATSORT_FLAGS csfFlags, 
                                   DWORD dwCategoryId1, 
                                   DWORD dwCategoryId2);

private:
    ~CFolderViewImplCategorizer_Size();
    LONG            m_cRef;
    IShellFolder2  *m_psf;
};

class CFolderViewImplCategorizer_Sides : public ICategorizer
{
public:
    CFolderViewImplCategorizer_Sides(IShellFolder2 *psf);

    // IUnknown methods
    IFACEMETHODIMP QueryInterface(__in REFIID riid, __deref_out void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // ICategorizer
    IFACEMETHODIMP GetDescription(__out_ecount(cch) PWSTR pszDesc, UINT cch);
    IFACEMETHODIMP GetCategory(UINT cidl, 
                               __in_ecount(cidl) PCUITEMID_CHILD_ARRAY apidl, 
                               __out_ecount(cidl) DWORD *rgCategoryIds);
    IFACEMETHODIMP GetCategoryInfo(DWORD dwCategoryId, __out CATEGORY_INFO *pci);
    IFACEMETHODIMP CompareCategory(CATSORT_FLAGS csfFlags, 
                                   DWORD dwCategoryId1, 
                                   DWORD dwCategoryId2);

private:
    ~CFolderViewImplCategorizer_Sides();
    LONG            m_cRef;
    IShellFolder2  *m_psf;
};

class CFolderViewImplCategorizer_Level : public ICategorizer
{
public:
    CFolderViewImplCategorizer_Level(IShellFolder2 *psf);

    // IUnknown methods
    IFACEMETHODIMP QueryInterface(__in REFIID riid, __deref_out void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // ICategorizer
    IFACEMETHODIMP GetDescription(__out_ecount(cch) PWSTR pszDesc, UINT cch);
    IFACEMETHODIMP GetCategory(UINT cidl, 
                               __in_ecount(cidl) PCUITEMID_CHILD_ARRAY apidl, 
                               __out_ecount(cidl) DWORD *rgCategoryIds);
    IFACEMETHODIMP GetCategoryInfo(DWORD dwCategoryId, __out CATEGORY_INFO *pci);
    IFACEMETHODIMP CompareCategory(CATSORT_FLAGS csfFlags, 
                                   DWORD dwCategoryId1, 
                                   DWORD dwCategoryId2);

private:
    ~CFolderViewImplCategorizer_Level();
    LONG            m_cRef;
    IShellFolder2  *m_psf;
};

class CFolderViewImplCategorizer_Value : public ICategorizer
{
public:
    CFolderViewImplCategorizer_Value(IShellFolder2 *psf);

    // IUnknown methods
    IFACEMETHODIMP QueryInterface(__in REFIID riid, __deref_out void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // ICategorizer
    IFACEMETHODIMP GetDescription(__out_ecount(cch) PWSTR pszDesc, UINT cch);
    IFACEMETHODIMP GetCategory(UINT cidl, 
                               __in_ecount(cidl) PCUITEMID_CHILD_ARRAY apidl, 
                               __out_ecount(cidl) DWORD *rgCategoryIds);
    IFACEMETHODIMP GetCategoryInfo(DWORD dwCategoryId, __out CATEGORY_INFO *pci);
    IFACEMETHODIMP CompareCategory(CATSORT_FLAGS csfFlags, 
                                   DWORD dwCategoryId1, 
                                   DWORD dwCategoryId2);

private:
    ~CFolderViewImplCategorizer_Value();
    LONG            m_cRef;
    IShellFolder2  *m_psf;
};

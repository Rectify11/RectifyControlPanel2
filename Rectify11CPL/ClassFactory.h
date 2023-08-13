/**************************************************************************
    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.
 
   (c) Microsoft Corporation. All Rights Reserved.
**************************************************************************/

#pragma once

class  CFolderViewImplClassFactory : public IClassFactory
{
public:
    CFolderViewImplClassFactory(REFCLSID rclsid);

    // IUnknown methods
    IFACEMETHODIMP QueryInterface(__in REFIID riid, __deref_out void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IClassFactory methods
    IFACEMETHODIMP CreateInstance(__in_opt IUnknown *pUnknown, __in REFIID riid, __deref_out void **ppv);
    IFACEMETHODIMP LockServer(BOOL fLock);

private:
    ~CFolderViewImplClassFactory();
    REFCLSID m_rclsid;
    LONG m_cRef;
};

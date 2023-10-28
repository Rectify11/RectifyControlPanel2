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

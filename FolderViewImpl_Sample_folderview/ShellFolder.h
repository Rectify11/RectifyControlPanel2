/**************************************************************************
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

(c) Microsoft Corporation. All Rights Reserved.
**************************************************************************/

#pragma once

#define MAX_OBJS    10
#define MYOBJID 0x1234

#pragma pack(1)
typedef struct tagObject
{
    USHORT  cb;
    WORD    MyObjID;
    BYTE    nLevel;
    BYTE    nSize;
    BYTE    nSides;
    BYTE    cchName;
    BOOL    fIsFolder;
    WCHAR   szName[1];
} FVITEMID;
#pragma pack()

typedef UNALIGNED FVITEMID *PFVITEMID;
typedef const UNALIGNED FVITEMID *PCFVITEMID;

class CFolderViewImplFolder
{
public:
    CFolderViewImplFolder(UINT nLevel);

    // IUnknown methods
    IFACEMETHODIMP QueryInterface(__in REFIID riid, __deref_out void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

private:
    ~CFolderViewImplFolder();

    long                m_cRef;
    int                 m_nLevel;
    PIDLIST_ABSOLUTE    m_pidl;             // where this folder is in the name space
    PWSTR               m_rgNames[MAX_OBJS];
    WCHAR               m_szModuleName[MAX_PATH];
};

#pragma once
#include <windows.h>

MIDL_INTERFACE("E1F5EC9F-F933-492B-A242-C3CDAC6ECFBD")
IDUIElementProviderInit: public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetResourceID(UINT id) = 0;
    virtual HRESULT STDMETHODCALLTYPE OptionallyTakeInitialFocus(BOOL* result) = 0;
};

MIDL_INTERFACE("1D3293BC-EC01-400F-8179-8DE5D53ABEB7")
IFrameNotificationClient : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE LayoutInitialized() = 0;
    virtual HRESULT STDMETHODCALLTYPE Notify(WORD* param) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnNavigateAway() = 0;
    virtual HRESULT STDMETHODCALLTYPE OnInnerElementDestroyed() = 0;
};

MIDL_INTERFACE("6B353825-C58B-4F03-AEC4-8DE179122661")
IFrameShellViewClient : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE OnSelectedItemChanged() = 0;
    virtual HRESULT STDMETHODCALLTYPE OnSelectionChanged() = 0;
    virtual HRESULT STDMETHODCALLTYPE OnContentsChanged() = 0;
    virtual HRESULT STDMETHODCALLTYPE OnFolderChanged() = 0;
};

enum CPNAV_LIST : int {
    CPNAV_Normal = 0,
    CPNAV_SeeAlso = 1,
    CPNAV_SomethingElse=2
};

enum CPNAVTYPE : int{
    CPNAVTYPE_None = 0,
    CPNAVTYPE_ShellExec = 1,
    CPNAVTYPE_Navigate = 2
};
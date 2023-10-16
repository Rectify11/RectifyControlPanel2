

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Mon Jan 18 22:14:07 2038
 */
/* Compiler settings for IRectifyUtil.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0628 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __IRectifyUtil_h_h__
#define __IRectifyUtil_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if defined(_CONTROL_FLOW_GUARD_XFG)
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

#ifndef __IRectifyUtil_FWD_DEFINED__
#define __IRectifyUtil_FWD_DEFINED__
typedef interface IRectifyUtil IRectifyUtil;

#endif 	/* __IRectifyUtil_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_IRectifyUtil_0000_0000 */
/* [local] */ 

typedef /* [public] */ 
enum __MIDL___MIDL_itf_IRectifyUtil_0000_0000_0001
    {
        Normal	= 0,
        NilesoftSmall	= 1,
        NilesoftFull	= 2,
        Classic	= 3,
        ClassicTransparent	= 4
    } 	MenuCustomizationType;



extern RPC_IF_HANDLE __MIDL_itf_IRectifyUtil_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_IRectifyUtil_0000_0000_v0_0_s_ifspec;

#ifndef __IRectifyUtil_INTERFACE_DEFINED__
#define __IRectifyUtil_INTERFACE_DEFINED__

/* interface IRectifyUtil */
/* [version][uuid][object] */ 


EXTERN_C const IID IID_IRectifyUtil;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A7BCDC3B-C5A2-44BB-B8EC-560B24ACAAD8")
    IRectifyUtil : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetMicaSettings( 
            /* [out] */ BOOL *pEnabled,
            /* [out] */ BOOL *pTabbed) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMicaForEveryoneEnabled( 
            /* [in] */ BOOL micaEnabled,
            /* [in] */ BOOL tabbed) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurrentMenuIndex( 
            /* [out] */ DWORD *pMenuIndex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCurrentMenuByIndex( 
            /* [in] */ DWORD pMenuIndex) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IRectifyUtilVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRectifyUtil * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRectifyUtil * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRectifyUtil * This);
        
        DECLSPEC_XFGVIRT(IRectifyUtil, GetMicaSettings)
        HRESULT ( STDMETHODCALLTYPE *GetMicaSettings )( 
            IRectifyUtil * This,
            /* [out] */ BOOL *pEnabled,
            /* [out] */ BOOL *pTabbed);
        
        DECLSPEC_XFGVIRT(IRectifyUtil, SetMicaForEveryoneEnabled)
        HRESULT ( STDMETHODCALLTYPE *SetMicaForEveryoneEnabled )( 
            IRectifyUtil * This,
            /* [in] */ BOOL micaEnabled,
            /* [in] */ BOOL tabbed);
        
        DECLSPEC_XFGVIRT(IRectifyUtil, GetCurrentMenuIndex)
        HRESULT ( STDMETHODCALLTYPE *GetCurrentMenuIndex )( 
            IRectifyUtil * This,
            /* [out] */ DWORD *pMenuIndex);
        
        DECLSPEC_XFGVIRT(IRectifyUtil, SetCurrentMenuByIndex)
        HRESULT ( STDMETHODCALLTYPE *SetCurrentMenuByIndex )( 
            IRectifyUtil * This,
            /* [in] */ DWORD pMenuIndex);
        
        END_INTERFACE
    } IRectifyUtilVtbl;

    interface IRectifyUtil
    {
        CONST_VTBL struct IRectifyUtilVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRectifyUtil_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRectifyUtil_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRectifyUtil_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRectifyUtil_GetMicaSettings(This,pEnabled,pTabbed)	\
    ( (This)->lpVtbl -> GetMicaSettings(This,pEnabled,pTabbed) ) 

#define IRectifyUtil_SetMicaForEveryoneEnabled(This,micaEnabled,tabbed)	\
    ( (This)->lpVtbl -> SetMicaForEveryoneEnabled(This,micaEnabled,tabbed) ) 

#define IRectifyUtil_GetCurrentMenuIndex(This,pMenuIndex)	\
    ( (This)->lpVtbl -> GetCurrentMenuIndex(This,pMenuIndex) ) 

#define IRectifyUtil_SetCurrentMenuByIndex(This,pMenuIndex)	\
    ( (This)->lpVtbl -> SetCurrentMenuByIndex(This,pMenuIndex) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRectifyUtil_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



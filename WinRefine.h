

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Thu Jul 29 14:55:48 2010
 */
/* Compiler settings for WinRefine.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __WinRefine_h__
#define __WinRefine_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IWinRefineShlExt_FWD_DEFINED__
#define __IWinRefineShlExt_FWD_DEFINED__
typedef interface IWinRefineShlExt IWinRefineShlExt;
#endif 	/* __IWinRefineShlExt_FWD_DEFINED__ */


#ifndef __WinRefineShlExt_FWD_DEFINED__
#define __WinRefineShlExt_FWD_DEFINED__

#ifdef __cplusplus
typedef class WinRefineShlExt WinRefineShlExt;
#else
typedef struct WinRefineShlExt WinRefineShlExt;
#endif /* __cplusplus */

#endif 	/* __WinRefineShlExt_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IWinRefineShlExt_INTERFACE_DEFINED__
#define __IWinRefineShlExt_INTERFACE_DEFINED__

/* interface IWinRefineShlExt */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWinRefineShlExt;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B865A80D-C21A-4D54-9420-0DB121AB67EA")
    IWinRefineShlExt : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IWinRefineShlExtVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWinRefineShlExt * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWinRefineShlExt * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWinRefineShlExt * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IWinRefineShlExt * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IWinRefineShlExt * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IWinRefineShlExt * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWinRefineShlExt * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IWinRefineShlExtVtbl;

    interface IWinRefineShlExt
    {
        CONST_VTBL struct IWinRefineShlExtVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWinRefineShlExt_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWinRefineShlExt_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWinRefineShlExt_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWinRefineShlExt_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IWinRefineShlExt_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IWinRefineShlExt_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IWinRefineShlExt_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWinRefineShlExt_INTERFACE_DEFINED__ */



#ifndef __WINREFINELib_LIBRARY_DEFINED__
#define __WINREFINELib_LIBRARY_DEFINED__

/* library WINREFINELib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_WINREFINELib;

EXTERN_C const CLSID CLSID_WinRefineShlExt;

#ifdef __cplusplus

class DECLSPEC_UUID("3BE3D911-D0F6-4DBC-A9DB-E4ECA21C53A7")
WinRefineShlExt;
#endif
#endif /* __WINREFINELib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sat Apr 21 11:40:33 2001
 */
/* Compiler settings for siobjectmodel.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
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

#ifndef __siobjectmodel_h__
#define __siobjectmodel_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __Application_FWD_DEFINED__
#define __Application_FWD_DEFINED__
typedef interface Application Application;
#endif 	/* __Application_FWD_DEFINED__ */


#ifndef __SIObject_FWD_DEFINED__
#define __SIObject_FWD_DEFINED__
typedef interface SIObject SIObject;
#endif 	/* __SIObject_FWD_DEFINED__ */


#ifndef __Project_FWD_DEFINED__
#define __Project_FWD_DEFINED__
typedef interface Project Project;
#endif 	/* __Project_FWD_DEFINED__ */


#ifndef __SIObjectCollection_FWD_DEFINED__
#define __SIObjectCollection_FWD_DEFINED__
typedef interface SIObjectCollection SIObjectCollection;
#endif 	/* __SIObjectCollection_FWD_DEFINED__ */


#ifndef __SIObjectCollection_FWD_DEFINED__
#define __SIObjectCollection_FWD_DEFINED__
typedef interface SIObjectCollection SIObjectCollection;
#endif 	/* __SIObjectCollection_FWD_DEFINED__ */


#ifndef __Application_FWD_DEFINED__
#define __Application_FWD_DEFINED__
typedef interface Application Application;
#endif 	/* __Application_FWD_DEFINED__ */


#ifndef __SIObject_FWD_DEFINED__
#define __SIObject_FWD_DEFINED__
typedef interface SIObject SIObject;
#endif 	/* __SIObject_FWD_DEFINED__ */


#ifndef __Project_FWD_DEFINED__
#define __Project_FWD_DEFINED__
typedef interface Project Project;
#endif 	/* __Project_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __SIOBJECTMODELLIB_LIBRARY_DEFINED__
#define __SIOBJECTMODELLIB_LIBRARY_DEFINED__

/* library SIOBJECTMODELLIB */
/* [helpstring][version][uuid] */ 

typedef /* [helpstring] */ 
enum tagsiSeverity
    {	siFatal	= 1,
	siError	= 2,
	siWarning	= 4,
	siInfo	= 8,
	siVerbose	= 16
    }	siSeverity;







EXTERN_C const IID LIBID_SIOBJECTMODELLIB;

#ifndef __Application_INTERFACE_DEFINED__
#define __Application_INTERFACE_DEFINED__

/* interface Application */
/* [unique][dual][local][object][helpstring][uuid] */ 


EXTERN_C const IID IID_Application;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E8E457CD-F6D3-11d3-B11F-00A0C982A112")
    Application : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_StatusBar( 
            /* [in] */ BSTR in_NewStatusBarText) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ExecuteScript( 
            /* [in] */ BSTR in_FileName,
            /* [defaultvalue][in] */ BSTR in_LanguageProgID,
            /* [defaultvalue][in] */ BSTR in_ProcName,
            /* [optional][out][in] */ LPVARIANT io_Params,
            /* [retval][out] */ LPVARIANT out_RetVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LogMessage( 
            /* [in] */ BSTR in_Message,
            /* [defaultvalue][in] */ siSeverity in_Severity = siInfo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Version( 
            /* [retval][out] */ BSTR __RPC_FAR *out_Version) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ApplicationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            Application __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            Application __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            Application __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            Application __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            Application __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            Application __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            Application __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_StatusBar )( 
            Application __RPC_FAR * This,
            /* [in] */ BSTR in_NewStatusBarText);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ExecuteScript )( 
            Application __RPC_FAR * This,
            /* [in] */ BSTR in_FileName,
            /* [defaultvalue][in] */ BSTR in_LanguageProgID,
            /* [defaultvalue][in] */ BSTR in_ProcName,
            /* [optional][out][in] */ LPVARIANT io_Params,
            /* [retval][out] */ LPVARIANT out_RetVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LogMessage )( 
            Application __RPC_FAR * This,
            /* [in] */ BSTR in_Message,
            /* [defaultvalue][in] */ siSeverity in_Severity);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Version )( 
            Application __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *out_Version);
        
        END_INTERFACE
    } ApplicationVtbl;

    interface Application
    {
        CONST_VTBL struct ApplicationVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Application_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define Application_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define Application_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define Application_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define Application_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define Application_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define Application_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define Application_put_StatusBar(This,in_NewStatusBarText)	\
    (This)->lpVtbl -> put_StatusBar(This,in_NewStatusBarText)

#define Application_ExecuteScript(This,in_FileName,in_LanguageProgID,in_ProcName,io_Params,out_RetVal)	\
    (This)->lpVtbl -> ExecuteScript(This,in_FileName,in_LanguageProgID,in_ProcName,io_Params,out_RetVal)

#define Application_LogMessage(This,in_Message,in_Severity)	\
    (This)->lpVtbl -> LogMessage(This,in_Message,in_Severity)

#define Application_Version(This,out_Version)	\
    (This)->lpVtbl -> Version(This,out_Version)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE Application_put_StatusBar_Proxy( 
    Application __RPC_FAR * This,
    /* [in] */ BSTR in_NewStatusBarText);


void __RPC_STUB Application_put_StatusBar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Application_ExecuteScript_Proxy( 
    Application __RPC_FAR * This,
    /* [in] */ BSTR in_FileName,
    /* [defaultvalue][in] */ BSTR in_LanguageProgID,
    /* [defaultvalue][in] */ BSTR in_ProcName,
    /* [optional][out][in] */ LPVARIANT io_Params,
    /* [retval][out] */ LPVARIANT out_RetVal);


void __RPC_STUB Application_ExecuteScript_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Application_LogMessage_Proxy( 
    Application __RPC_FAR * This,
    /* [in] */ BSTR in_Message,
    /* [defaultvalue][in] */ siSeverity in_Severity);


void __RPC_STUB Application_LogMessage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Application_Version_Proxy( 
    Application __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *out_Version);


void __RPC_STUB Application_Version_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __Application_INTERFACE_DEFINED__ */


#ifndef __SIObject_INTERFACE_DEFINED__
#define __SIObject_INTERFACE_DEFINED__

/* interface SIObject */
/* [unique][dual][local][object][helpstring][uuid] */ 


EXTERN_C const IID IID_SIObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D60F50AC-E3E6-11d3-811D-00A0C9AC19A9")
    SIObject : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FullName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR in_Name) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Parent( 
            /* [retval][out] */ LPDISPATCH __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ Application __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsEqualTo( 
            /* [in] */ IDispatch __RPC_FAR *in_pItem,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][hidden][propget] */ HRESULT STDMETHODCALLTYPE get_UniqueName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct SIObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            SIObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            SIObject __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            SIObject __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            SIObject __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            SIObject __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            SIObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            SIObject __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FullName )( 
            SIObject __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            SIObject __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            SIObject __RPC_FAR * This,
            /* [in] */ BSTR in_Name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            SIObject __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Parent )( 
            SIObject __RPC_FAR * This,
            /* [retval][out] */ LPDISPATCH __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Application )( 
            SIObject __RPC_FAR * This,
            /* [retval][out] */ Application __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsEqualTo )( 
            SIObject __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *in_pItem,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][hidden][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UniqueName )( 
            SIObject __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        END_INTERFACE
    } SIObjectVtbl;

    interface SIObject
    {
        CONST_VTBL struct SIObjectVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define SIObject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define SIObject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define SIObject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define SIObject_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define SIObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define SIObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define SIObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define SIObject_get_FullName(This,pVal)	\
    (This)->lpVtbl -> get_FullName(This,pVal)

#define SIObject_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define SIObject_put_Name(This,in_Name)	\
    (This)->lpVtbl -> put_Name(This,in_Name)

#define SIObject_get_Type(This,pVal)	\
    (This)->lpVtbl -> get_Type(This,pVal)

#define SIObject_get_Parent(This,pVal)	\
    (This)->lpVtbl -> get_Parent(This,pVal)

#define SIObject_get_Application(This,pVal)	\
    (This)->lpVtbl -> get_Application(This,pVal)

#define SIObject_IsEqualTo(This,in_pItem,pVal)	\
    (This)->lpVtbl -> IsEqualTo(This,in_pItem,pVal)

#define SIObject_get_UniqueName(This,pVal)	\
    (This)->lpVtbl -> get_UniqueName(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE SIObject_get_FullName_Proxy( 
    SIObject __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB SIObject_get_FullName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE SIObject_get_Name_Proxy( 
    SIObject __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB SIObject_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE SIObject_put_Name_Proxy( 
    SIObject __RPC_FAR * This,
    /* [in] */ BSTR in_Name);


void __RPC_STUB SIObject_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE SIObject_get_Type_Proxy( 
    SIObject __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB SIObject_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE SIObject_get_Parent_Proxy( 
    SIObject __RPC_FAR * This,
    /* [retval][out] */ LPDISPATCH __RPC_FAR *pVal);


void __RPC_STUB SIObject_get_Parent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE SIObject_get_Application_Proxy( 
    SIObject __RPC_FAR * This,
    /* [retval][out] */ Application __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB SIObject_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SIObject_IsEqualTo_Proxy( 
    SIObject __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *in_pItem,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB SIObject_IsEqualTo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][hidden][propget] */ HRESULT STDMETHODCALLTYPE SIObject_get_UniqueName_Proxy( 
    SIObject __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB SIObject_get_UniqueName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __SIObject_INTERFACE_DEFINED__ */


#ifndef __Project_INTERFACE_DEFINED__
#define __Project_INTERFACE_DEFINED__

/* interface Project */
/* [unique][dual][local][object][helpstring][uuid] */ 


EXTERN_C const IID IID_Project;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("52969A80-493B-11d4-80F8-00A024189A69")
    Project : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Path( 
            /* [retval][out] */ BSTR __RPC_FAR *out_Path) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UserName( 
            /* [retval][out] */ BSTR __RPC_FAR *out_UserName) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SavedDateTime( 
            /* [retval][out] */ DATE __RPC_FAR *out_DateTime) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ProjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            Project __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            Project __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            Project __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            Project __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            Project __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            Project __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            Project __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Path )( 
            Project __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *out_Path);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UserName )( 
            Project __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *out_UserName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SavedDateTime )( 
            Project __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *out_DateTime);
        
        END_INTERFACE
    } ProjectVtbl;

    interface Project
    {
        CONST_VTBL struct ProjectVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Project_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define Project_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define Project_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define Project_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define Project_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define Project_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define Project_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define Project_get_Path(This,out_Path)	\
    (This)->lpVtbl -> get_Path(This,out_Path)

#define Project_get_UserName(This,out_UserName)	\
    (This)->lpVtbl -> get_UserName(This,out_UserName)

#define Project_get_SavedDateTime(This,out_DateTime)	\
    (This)->lpVtbl -> get_SavedDateTime(This,out_DateTime)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE Project_get_Path_Proxy( 
    Project __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *out_Path);


void __RPC_STUB Project_get_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE Project_get_UserName_Proxy( 
    Project __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *out_UserName);


void __RPC_STUB Project_get_UserName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE Project_get_SavedDateTime_Proxy( 
    Project __RPC_FAR * This,
    /* [retval][out] */ DATE __RPC_FAR *out_DateTime);


void __RPC_STUB Project_get_SavedDateTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __Project_INTERFACE_DEFINED__ */


#ifndef __SIObjectCollection_INTERFACE_DEFINED__
#define __SIObjectCollection_INTERFACE_DEFINED__

/* interface SIObjectCollection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_SIObjectCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7C4B6BA1-C480-11d4-8177-00A024189A69")
    SIObjectCollection : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [optional][in] */ VARIANT idx,
            /* [retval][out] */ SIObject __RPC_FAR *__RPC_FAR *out_pParam) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *vtRetVal) = 0;
        
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get_NewEnum( 
            /* [retval][out] */ LPUNKNOWN __RPC_FAR *retval) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct SIObjectCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            SIObjectCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            SIObjectCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            SIObjectCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            SIObjectCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            SIObjectCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            SIObjectCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            SIObjectCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            SIObjectCollection __RPC_FAR * This,
            /* [optional][in] */ VARIANT idx,
            /* [retval][out] */ SIObject __RPC_FAR *__RPC_FAR *out_pParam);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            SIObjectCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *vtRetVal);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_NewEnum )( 
            SIObjectCollection __RPC_FAR * This,
            /* [retval][out] */ LPUNKNOWN __RPC_FAR *retval);
        
        END_INTERFACE
    } SIObjectCollectionVtbl;

    interface SIObjectCollection
    {
        CONST_VTBL struct SIObjectCollectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define SIObjectCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define SIObjectCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define SIObjectCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define SIObjectCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define SIObjectCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define SIObjectCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define SIObjectCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define SIObjectCollection_get_Item(This,idx,out_pParam)	\
    (This)->lpVtbl -> get_Item(This,idx,out_pParam)

#define SIObjectCollection_get_Count(This,vtRetVal)	\
    (This)->lpVtbl -> get_Count(This,vtRetVal)

#define SIObjectCollection_get_NewEnum(This,retval)	\
    (This)->lpVtbl -> get_NewEnum(This,retval)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE SIObjectCollection_get_Item_Proxy( 
    SIObjectCollection __RPC_FAR * This,
    /* [optional][in] */ VARIANT idx,
    /* [retval][out] */ SIObject __RPC_FAR *__RPC_FAR *out_pParam);


void __RPC_STUB SIObjectCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE SIObjectCollection_get_Count_Proxy( 
    SIObjectCollection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *vtRetVal);


void __RPC_STUB SIObjectCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE SIObjectCollection_get_NewEnum_Proxy( 
    SIObjectCollection __RPC_FAR * This,
    /* [retval][out] */ LPUNKNOWN __RPC_FAR *retval);


void __RPC_STUB SIObjectCollection_get_NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __SIObjectCollection_INTERFACE_DEFINED__ */

#endif /* __SIOBJECTMODELLIB_LIBRARY_DEFINED__ */

/* interface __MIDL_itf_siobjectmodel_0000 */
/* [local] */ 

#pragma once

#pragma once



extern RPC_IF_HANDLE __MIDL_itf_siobjectmodel_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_siobjectmodel_0000_v0_0_s_ifspec;

/* interface __MIDL_itf_siobjectmodel_0210 */
/* [local] */ 

typedef Application __RPC_FAR *LPAPPLICATION;



extern RPC_IF_HANDLE __MIDL_itf_siobjectmodel_0210_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_siobjectmodel_0210_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long __RPC_FAR *, unsigned long            , VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long __RPC_FAR *, VARIANT __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif

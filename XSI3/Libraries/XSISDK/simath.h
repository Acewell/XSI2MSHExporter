/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue May 08 09:24:36 2001
 */
/* Compiler settings for simath.idl:
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

#ifndef __simath_h__
#define __simath_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ISIVector3_FWD_DEFINED__
#define __ISIVector3_FWD_DEFINED__
typedef interface ISIVector3 ISIVector3;
#endif 	/* __ISIVector3_FWD_DEFINED__ */


#ifndef __ISIMatrix3_FWD_DEFINED__
#define __ISIMatrix3_FWD_DEFINED__
typedef interface ISIMatrix3 ISIMatrix3;
#endif 	/* __ISIMatrix3_FWD_DEFINED__ */


#ifndef __ISIMatrix4_FWD_DEFINED__
#define __ISIMatrix4_FWD_DEFINED__
typedef interface ISIMatrix4 ISIMatrix4;
#endif 	/* __ISIMatrix4_FWD_DEFINED__ */


#ifndef __ISIRotation_FWD_DEFINED__
#define __ISIRotation_FWD_DEFINED__
typedef interface ISIRotation ISIRotation;
#endif 	/* __ISIRotation_FWD_DEFINED__ */


#ifndef __ISIQuaternion_FWD_DEFINED__
#define __ISIQuaternion_FWD_DEFINED__
typedef interface ISIQuaternion ISIQuaternion;
#endif 	/* __ISIQuaternion_FWD_DEFINED__ */


#ifndef __ISITransformation_FWD_DEFINED__
#define __ISITransformation_FWD_DEFINED__
typedef interface ISITransformation ISITransformation;
#endif 	/* __ISITransformation_FWD_DEFINED__ */


#ifndef __XSIMath_FWD_DEFINED__
#define __XSIMath_FWD_DEFINED__
typedef interface XSIMath XSIMath;
#endif 	/* __XSIMath_FWD_DEFINED__ */


#ifndef __ISIBoundingVolume_FWD_DEFINED__
#define __ISIBoundingVolume_FWD_DEFINED__
typedef interface ISIBoundingVolume ISIBoundingVolume;
#endif 	/* __ISIBoundingVolume_FWD_DEFINED__ */


#ifndef __SIVector3_FWD_DEFINED__
#define __SIVector3_FWD_DEFINED__

#ifdef __cplusplus
typedef class SIVector3 SIVector3;
#else
typedef struct SIVector3 SIVector3;
#endif /* __cplusplus */

#endif 	/* __SIVector3_FWD_DEFINED__ */


#ifndef __SIMatrix3_FWD_DEFINED__
#define __SIMatrix3_FWD_DEFINED__

#ifdef __cplusplus
typedef class SIMatrix3 SIMatrix3;
#else
typedef struct SIMatrix3 SIMatrix3;
#endif /* __cplusplus */

#endif 	/* __SIMatrix3_FWD_DEFINED__ */


#ifndef __SIMatrix4_FWD_DEFINED__
#define __SIMatrix4_FWD_DEFINED__

#ifdef __cplusplus
typedef class SIMatrix4 SIMatrix4;
#else
typedef struct SIMatrix4 SIMatrix4;
#endif /* __cplusplus */

#endif 	/* __SIMatrix4_FWD_DEFINED__ */


#ifndef __SIRotation_FWD_DEFINED__
#define __SIRotation_FWD_DEFINED__

#ifdef __cplusplus
typedef class SIRotation SIRotation;
#else
typedef struct SIRotation SIRotation;
#endif /* __cplusplus */

#endif 	/* __SIRotation_FWD_DEFINED__ */


#ifndef __SIQuaternion_FWD_DEFINED__
#define __SIQuaternion_FWD_DEFINED__

#ifdef __cplusplus
typedef class SIQuaternion SIQuaternion;
#else
typedef struct SIQuaternion SIQuaternion;
#endif /* __cplusplus */

#endif 	/* __SIQuaternion_FWD_DEFINED__ */


#ifndef __SITransformation_FWD_DEFINED__
#define __SITransformation_FWD_DEFINED__

#ifdef __cplusplus
typedef class SITransformation SITransformation;
#else
typedef struct SITransformation SITransformation;
#endif /* __cplusplus */

#endif 	/* __SITransformation_FWD_DEFINED__ */


#ifndef __CXSIMath_FWD_DEFINED__
#define __CXSIMath_FWD_DEFINED__

#ifdef __cplusplus
typedef class CXSIMath CXSIMath;
#else
typedef struct CXSIMath CXSIMath;
#endif /* __cplusplus */

#endif 	/* __CXSIMath_FWD_DEFINED__ */


void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __SIMATHLib_LIBRARY_DEFINED__
#define __SIMATHLib_LIBRARY_DEFINED__

/* library SIMATHLib */
/* [helpstring][version][uuid] */ 









//typedef /* [helpstring] */ typedef /* [public][public][helpstring] */ 
//enum __MIDL___MIDL_itf_simath_0000_0001
//    {	siPleaseLoadMe	= -1
//   }	tagsiPleaseLoadMeType;
//;

typedef /* [public][public][helpstring] */ 
enum __MIDL___MIDL_itf_simath_0000_0001
    {	siPleaseLoadMe	= -1
    }	tagsiPleaseLoadMeType;


EXTERN_C const IID LIBID_SIMATHLib;

#ifndef __ISIVector3_INTERFACE_DEFINED__
#define __ISIVector3_INTERFACE_DEFINED__

/* interface ISIVector3 */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_ISIVector3;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0222732E-232E-11D3-BC3A-00A0C9612B97")
    ISIVector3 : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [in] */ short in_nIndex,
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ short in_nIndex,
            /* [in] */ double pVal) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_X( 
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_X( 
            /* [in] */ double pVal) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Y( 
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Y( 
            /* [in] */ double pVal) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Z( 
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Z( 
            /* [in] */ double pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Copy( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Set( 
            /* [in] */ double in_dXValue,
            /* [in] */ double in_dYValue,
            /* [in] */ double in_dZValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector1,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector2) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddInPlace( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Sub( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector1,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector2) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SubInPlace( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Negate( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE NegateInPlace( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Scale( 
            /* [in] */ double in_dScalar,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ScaleInPlace( 
            /* [in] */ double in_dScalar) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ScaleAdd( 
            /* [in] */ double in_dScalar,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector1,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector2) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ScaleAddInPlace( 
            /* [in] */ double in_dScalar,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Equals( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
            /* [retval][out] */ long __RPC_FAR *out_pbEquality) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EpsilonEquals( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
            /* [in] */ double in_dEpsilon,
            /* [retval][out] */ long __RPC_FAR *out_pbEquality) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clamp( 
            /* [in] */ double in_dMin,
            /* [in] */ double in_dMax,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ClampInPlace( 
            /* [in] */ double in_dMin,
            /* [in] */ double in_dMax) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Absolute( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AbsoluteInPlace( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LinearlyInterpolate( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector1,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector2,
            /* [in] */ double in_dAlpha) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Angle( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
            /* [retval][out] */ double __RPC_FAR *out_pdAngle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Cross( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector1,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector2) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Dot( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
            /* [retval][out] */ double __RPC_FAR *out_pdDot) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Normalize( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
            /* [retval][out] */ long __RPC_FAR *out_pbNormalized) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE NormalizeInPlace( 
            /* [retval][out] */ long __RPC_FAR *out_pbNormalized) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LengthSquared( 
            /* [retval][out] */ double __RPC_FAR *out_pdLengthSquared) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Length( 
            /* [retval][out] */ double __RPC_FAR *out_pdLength) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MinComponent( 
            /* [retval][out] */ double __RPC_FAR *out_pdMinComponent) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MinComponentIndex( 
            /* [retval][out] */ short __RPC_FAR *out_pnMinComponentIndex) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MaxComponent( 
            /* [retval][out] */ double __RPC_FAR *out_pdMaxComponent) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MaxComponentIndex( 
            /* [retval][out] */ short __RPC_FAR *out_pnMaxComponentIndex) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetNull( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MulByMatrix3( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
            /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MulByMatrix3InPlace( 
            /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MulByMatrix4( 
            ISIVector3 __RPC_FAR *in_pVector,
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MulByMatrix4InPlace( 
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MulByRotation( 
            ISIVector3 __RPC_FAR *in_pVector,
            /* [in] */ ISIRotation __RPC_FAR *in_pRotation) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MulByRotationInPlace( 
            /* [in] */ ISIRotation __RPC_FAR *in_pRotation) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MulByTransformation( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
            /* [in] */ ISITransformation __RPC_FAR *in_pTransformation) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MulByTransformationInPlace( 
            /* [in] */ ISITransformation __RPC_FAR *in_pTransformation) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Get( 
            /* [out] */ VARIANT __RPC_FAR *out_pvarX,
            /* [out] */ VARIANT __RPC_FAR *out_pvarY,
            /* [out] */ VARIANT __RPC_FAR *out_pvarZ) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISIVector3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISIVector3 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISIVector3 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISIVector3 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ short in_nIndex,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Value )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ short in_nIndex,
            /* [in] */ double pVal);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_X )( 
            ISIVector3 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_X )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ double pVal);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Y )( 
            ISIVector3 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Y )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ double pVal);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Z )( 
            ISIVector3 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Z )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ double pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Copy )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Set )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ double in_dXValue,
            /* [in] */ double in_dYValue,
            /* [in] */ double in_dZValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector1,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector2);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddInPlace )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Sub )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector1,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector2);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SubInPlace )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Negate )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *NegateInPlace )( 
            ISIVector3 __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Scale )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ double in_dScalar,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ScaleInPlace )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ double in_dScalar);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ScaleAdd )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ double in_dScalar,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector1,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector2);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ScaleAddInPlace )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ double in_dScalar,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Equals )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
            /* [retval][out] */ long __RPC_FAR *out_pbEquality);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EpsilonEquals )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
            /* [in] */ double in_dEpsilon,
            /* [retval][out] */ long __RPC_FAR *out_pbEquality);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clamp )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ double in_dMin,
            /* [in] */ double in_dMax,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ClampInPlace )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ double in_dMin,
            /* [in] */ double in_dMax);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Absolute )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AbsoluteInPlace )( 
            ISIVector3 __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LinearlyInterpolate )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector1,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector2,
            /* [in] */ double in_dAlpha);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Angle )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
            /* [retval][out] */ double __RPC_FAR *out_pdAngle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Cross )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector1,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector2);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Dot )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
            /* [retval][out] */ double __RPC_FAR *out_pdDot);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Normalize )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
            /* [retval][out] */ long __RPC_FAR *out_pbNormalized);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *NormalizeInPlace )( 
            ISIVector3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *out_pbNormalized);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LengthSquared )( 
            ISIVector3 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *out_pdLengthSquared);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Length )( 
            ISIVector3 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *out_pdLength);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MinComponent )( 
            ISIVector3 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *out_pdMinComponent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MinComponentIndex )( 
            ISIVector3 __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *out_pnMinComponentIndex);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MaxComponent )( 
            ISIVector3 __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *out_pdMaxComponent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MaxComponentIndex )( 
            ISIVector3 __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *out_pnMaxComponentIndex);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetNull )( 
            ISIVector3 __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MulByMatrix3 )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
            /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MulByMatrix3InPlace )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MulByMatrix4 )( 
            ISIVector3 __RPC_FAR * This,
            ISIVector3 __RPC_FAR *in_pVector,
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MulByMatrix4InPlace )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MulByRotation )( 
            ISIVector3 __RPC_FAR * This,
            ISIVector3 __RPC_FAR *in_pVector,
            /* [in] */ ISIRotation __RPC_FAR *in_pRotation);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MulByRotationInPlace )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISIRotation __RPC_FAR *in_pRotation);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MulByTransformation )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
            /* [in] */ ISITransformation __RPC_FAR *in_pTransformation);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MulByTransformationInPlace )( 
            ISIVector3 __RPC_FAR * This,
            /* [in] */ ISITransformation __RPC_FAR *in_pTransformation);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get )( 
            ISIVector3 __RPC_FAR * This,
            /* [out] */ VARIANT __RPC_FAR *out_pvarX,
            /* [out] */ VARIANT __RPC_FAR *out_pvarY,
            /* [out] */ VARIANT __RPC_FAR *out_pvarZ);
        
        END_INTERFACE
    } ISIVector3Vtbl;

    interface ISIVector3
    {
        CONST_VTBL struct ISIVector3Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISIVector3_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISIVector3_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISIVector3_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISIVector3_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISIVector3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISIVector3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISIVector3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISIVector3_get_Value(This,in_nIndex,pVal)	\
    (This)->lpVtbl -> get_Value(This,in_nIndex,pVal)

#define ISIVector3_put_Value(This,in_nIndex,pVal)	\
    (This)->lpVtbl -> put_Value(This,in_nIndex,pVal)

#define ISIVector3_get_X(This,pVal)	\
    (This)->lpVtbl -> get_X(This,pVal)

#define ISIVector3_put_X(This,pVal)	\
    (This)->lpVtbl -> put_X(This,pVal)

#define ISIVector3_get_Y(This,pVal)	\
    (This)->lpVtbl -> get_Y(This,pVal)

#define ISIVector3_put_Y(This,pVal)	\
    (This)->lpVtbl -> put_Y(This,pVal)

#define ISIVector3_get_Z(This,pVal)	\
    (This)->lpVtbl -> get_Z(This,pVal)

#define ISIVector3_put_Z(This,pVal)	\
    (This)->lpVtbl -> put_Z(This,pVal)

#define ISIVector3_Copy(This,in_pVector)	\
    (This)->lpVtbl -> Copy(This,in_pVector)

#define ISIVector3_Set(This,in_dXValue,in_dYValue,in_dZValue)	\
    (This)->lpVtbl -> Set(This,in_dXValue,in_dYValue,in_dZValue)

#define ISIVector3_Add(This,in_pVector1,in_pVector2)	\
    (This)->lpVtbl -> Add(This,in_pVector1,in_pVector2)

#define ISIVector3_AddInPlace(This,in_pVector)	\
    (This)->lpVtbl -> AddInPlace(This,in_pVector)

#define ISIVector3_Sub(This,in_pVector1,in_pVector2)	\
    (This)->lpVtbl -> Sub(This,in_pVector1,in_pVector2)

#define ISIVector3_SubInPlace(This,in_pVector)	\
    (This)->lpVtbl -> SubInPlace(This,in_pVector)

#define ISIVector3_Negate(This,in_pVector)	\
    (This)->lpVtbl -> Negate(This,in_pVector)

#define ISIVector3_NegateInPlace(This)	\
    (This)->lpVtbl -> NegateInPlace(This)

#define ISIVector3_Scale(This,in_dScalar,in_pVector)	\
    (This)->lpVtbl -> Scale(This,in_dScalar,in_pVector)

#define ISIVector3_ScaleInPlace(This,in_dScalar)	\
    (This)->lpVtbl -> ScaleInPlace(This,in_dScalar)

#define ISIVector3_ScaleAdd(This,in_dScalar,in_pVector1,in_pVector2)	\
    (This)->lpVtbl -> ScaleAdd(This,in_dScalar,in_pVector1,in_pVector2)

#define ISIVector3_ScaleAddInPlace(This,in_dScalar,in_pVector)	\
    (This)->lpVtbl -> ScaleAddInPlace(This,in_dScalar,in_pVector)

#define ISIVector3_Equals(This,in_pVector,out_pbEquality)	\
    (This)->lpVtbl -> Equals(This,in_pVector,out_pbEquality)

#define ISIVector3_EpsilonEquals(This,in_pVector,in_dEpsilon,out_pbEquality)	\
    (This)->lpVtbl -> EpsilonEquals(This,in_pVector,in_dEpsilon,out_pbEquality)

#define ISIVector3_Clamp(This,in_dMin,in_dMax,in_pVector)	\
    (This)->lpVtbl -> Clamp(This,in_dMin,in_dMax,in_pVector)

#define ISIVector3_ClampInPlace(This,in_dMin,in_dMax)	\
    (This)->lpVtbl -> ClampInPlace(This,in_dMin,in_dMax)

#define ISIVector3_Absolute(This,in_pVector)	\
    (This)->lpVtbl -> Absolute(This,in_pVector)

#define ISIVector3_AbsoluteInPlace(This)	\
    (This)->lpVtbl -> AbsoluteInPlace(This)

#define ISIVector3_LinearlyInterpolate(This,in_pVector1,in_pVector2,in_dAlpha)	\
    (This)->lpVtbl -> LinearlyInterpolate(This,in_pVector1,in_pVector2,in_dAlpha)

#define ISIVector3_Angle(This,in_pVector,out_pdAngle)	\
    (This)->lpVtbl -> Angle(This,in_pVector,out_pdAngle)

#define ISIVector3_Cross(This,in_pVector1,in_pVector2)	\
    (This)->lpVtbl -> Cross(This,in_pVector1,in_pVector2)

#define ISIVector3_Dot(This,in_pVector,out_pdDot)	\
    (This)->lpVtbl -> Dot(This,in_pVector,out_pdDot)

#define ISIVector3_Normalize(This,in_pVector,out_pbNormalized)	\
    (This)->lpVtbl -> Normalize(This,in_pVector,out_pbNormalized)

#define ISIVector3_NormalizeInPlace(This,out_pbNormalized)	\
    (This)->lpVtbl -> NormalizeInPlace(This,out_pbNormalized)

#define ISIVector3_LengthSquared(This,out_pdLengthSquared)	\
    (This)->lpVtbl -> LengthSquared(This,out_pdLengthSquared)

#define ISIVector3_Length(This,out_pdLength)	\
    (This)->lpVtbl -> Length(This,out_pdLength)

#define ISIVector3_MinComponent(This,out_pdMinComponent)	\
    (This)->lpVtbl -> MinComponent(This,out_pdMinComponent)

#define ISIVector3_MinComponentIndex(This,out_pnMinComponentIndex)	\
    (This)->lpVtbl -> MinComponentIndex(This,out_pnMinComponentIndex)

#define ISIVector3_MaxComponent(This,out_pdMaxComponent)	\
    (This)->lpVtbl -> MaxComponent(This,out_pdMaxComponent)

#define ISIVector3_MaxComponentIndex(This,out_pnMaxComponentIndex)	\
    (This)->lpVtbl -> MaxComponentIndex(This,out_pnMaxComponentIndex)

#define ISIVector3_SetNull(This)	\
    (This)->lpVtbl -> SetNull(This)

#define ISIVector3_MulByMatrix3(This,in_pVector,in_pMatrix)	\
    (This)->lpVtbl -> MulByMatrix3(This,in_pVector,in_pMatrix)

#define ISIVector3_MulByMatrix3InPlace(This,in_pMatrix)	\
    (This)->lpVtbl -> MulByMatrix3InPlace(This,in_pMatrix)

#define ISIVector3_MulByMatrix4(This,in_pVector,in_pMatrix)	\
    (This)->lpVtbl -> MulByMatrix4(This,in_pVector,in_pMatrix)

#define ISIVector3_MulByMatrix4InPlace(This,in_pMatrix)	\
    (This)->lpVtbl -> MulByMatrix4InPlace(This,in_pMatrix)

#define ISIVector3_MulByRotation(This,in_pVector,in_pRotation)	\
    (This)->lpVtbl -> MulByRotation(This,in_pVector,in_pRotation)

#define ISIVector3_MulByRotationInPlace(This,in_pRotation)	\
    (This)->lpVtbl -> MulByRotationInPlace(This,in_pRotation)

#define ISIVector3_MulByTransformation(This,in_pVector,in_pTransformation)	\
    (This)->lpVtbl -> MulByTransformation(This,in_pVector,in_pTransformation)

#define ISIVector3_MulByTransformationInPlace(This,in_pTransformation)	\
    (This)->lpVtbl -> MulByTransformationInPlace(This,in_pTransformation)

#define ISIVector3_Get(This,out_pvarX,out_pvarY,out_pvarZ)	\
    (This)->lpVtbl -> Get(This,out_pvarX,out_pvarY,out_pvarZ)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_get_Value_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ short in_nIndex,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB ISIVector3_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_put_Value_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ short in_nIndex,
    /* [in] */ double pVal);


void __RPC_STUB ISIVector3_put_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_get_X_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB ISIVector3_get_X_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_put_X_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ double pVal);


void __RPC_STUB ISIVector3_put_X_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_get_Y_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB ISIVector3_get_Y_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_put_Y_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ double pVal);


void __RPC_STUB ISIVector3_put_Y_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_get_Z_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB ISIVector3_get_Z_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_put_Z_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ double pVal);


void __RPC_STUB ISIVector3_put_Z_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_Copy_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector);


void __RPC_STUB ISIVector3_Copy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_Set_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ double in_dXValue,
    /* [in] */ double in_dYValue,
    /* [in] */ double in_dZValue);


void __RPC_STUB ISIVector3_Set_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_Add_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector1,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector2);


void __RPC_STUB ISIVector3_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_AddInPlace_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector);


void __RPC_STUB ISIVector3_AddInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_Sub_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector1,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector2);


void __RPC_STUB ISIVector3_Sub_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_SubInPlace_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector);


void __RPC_STUB ISIVector3_SubInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_Negate_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector);


void __RPC_STUB ISIVector3_Negate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_NegateInPlace_Proxy( 
    ISIVector3 __RPC_FAR * This);


void __RPC_STUB ISIVector3_NegateInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_Scale_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ double in_dScalar,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector);


void __RPC_STUB ISIVector3_Scale_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_ScaleInPlace_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ double in_dScalar);


void __RPC_STUB ISIVector3_ScaleInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_ScaleAdd_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ double in_dScalar,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector1,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector2);


void __RPC_STUB ISIVector3_ScaleAdd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_ScaleAddInPlace_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ double in_dScalar,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector);


void __RPC_STUB ISIVector3_ScaleAddInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_Equals_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
    /* [retval][out] */ long __RPC_FAR *out_pbEquality);


void __RPC_STUB ISIVector3_Equals_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_EpsilonEquals_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
    /* [in] */ double in_dEpsilon,
    /* [retval][out] */ long __RPC_FAR *out_pbEquality);


void __RPC_STUB ISIVector3_EpsilonEquals_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_Clamp_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ double in_dMin,
    /* [in] */ double in_dMax,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector);


void __RPC_STUB ISIVector3_Clamp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_ClampInPlace_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ double in_dMin,
    /* [in] */ double in_dMax);


void __RPC_STUB ISIVector3_ClampInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_Absolute_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector);


void __RPC_STUB ISIVector3_Absolute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_AbsoluteInPlace_Proxy( 
    ISIVector3 __RPC_FAR * This);


void __RPC_STUB ISIVector3_AbsoluteInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_LinearlyInterpolate_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector1,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector2,
    /* [in] */ double in_dAlpha);


void __RPC_STUB ISIVector3_LinearlyInterpolate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_Angle_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
    /* [retval][out] */ double __RPC_FAR *out_pdAngle);


void __RPC_STUB ISIVector3_Angle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_Cross_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector1,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector2);


void __RPC_STUB ISIVector3_Cross_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_Dot_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
    /* [retval][out] */ double __RPC_FAR *out_pdDot);


void __RPC_STUB ISIVector3_Dot_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_Normalize_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
    /* [retval][out] */ long __RPC_FAR *out_pbNormalized);


void __RPC_STUB ISIVector3_Normalize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_NormalizeInPlace_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *out_pbNormalized);


void __RPC_STUB ISIVector3_NormalizeInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_LengthSquared_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *out_pdLengthSquared);


void __RPC_STUB ISIVector3_LengthSquared_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_Length_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *out_pdLength);


void __RPC_STUB ISIVector3_Length_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_MinComponent_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *out_pdMinComponent);


void __RPC_STUB ISIVector3_MinComponent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_MinComponentIndex_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *out_pnMinComponentIndex);


void __RPC_STUB ISIVector3_MinComponentIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_MaxComponent_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *out_pdMaxComponent);


void __RPC_STUB ISIVector3_MaxComponent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_MaxComponentIndex_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *out_pnMaxComponentIndex);


void __RPC_STUB ISIVector3_MaxComponentIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_SetNull_Proxy( 
    ISIVector3 __RPC_FAR * This);


void __RPC_STUB ISIVector3_SetNull_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_MulByMatrix3_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
    /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix);


void __RPC_STUB ISIVector3_MulByMatrix3_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_MulByMatrix3InPlace_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix);


void __RPC_STUB ISIVector3_MulByMatrix3InPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_MulByMatrix4_Proxy( 
    ISIVector3 __RPC_FAR * This,
    ISIVector3 __RPC_FAR *in_pVector,
    /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix);


void __RPC_STUB ISIVector3_MulByMatrix4_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_MulByMatrix4InPlace_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix);


void __RPC_STUB ISIVector3_MulByMatrix4InPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_MulByRotation_Proxy( 
    ISIVector3 __RPC_FAR * This,
    ISIVector3 __RPC_FAR *in_pVector,
    /* [in] */ ISIRotation __RPC_FAR *in_pRotation);


void __RPC_STUB ISIVector3_MulByRotation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_MulByRotationInPlace_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISIRotation __RPC_FAR *in_pRotation);


void __RPC_STUB ISIVector3_MulByRotationInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_MulByTransformation_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pVector,
    /* [in] */ ISITransformation __RPC_FAR *in_pTransformation);


void __RPC_STUB ISIVector3_MulByTransformation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_MulByTransformationInPlace_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [in] */ ISITransformation __RPC_FAR *in_pTransformation);


void __RPC_STUB ISIVector3_MulByTransformationInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIVector3_Get_Proxy( 
    ISIVector3 __RPC_FAR * This,
    /* [out] */ VARIANT __RPC_FAR *out_pvarX,
    /* [out] */ VARIANT __RPC_FAR *out_pvarY,
    /* [out] */ VARIANT __RPC_FAR *out_pvarZ);


void __RPC_STUB ISIVector3_Get_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISIVector3_INTERFACE_DEFINED__ */


#ifndef __ISIMatrix3_INTERFACE_DEFINED__
#define __ISIMatrix3_INTERFACE_DEFINED__

/* interface ISIMatrix3 */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_ISIMatrix3;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BFB3B913-24CC-11D3-BC3A-00A0C9612B97")
    ISIMatrix3 : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [in] */ short in_nRow,
            /* [in] */ short in_nCol,
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ short in_nRow,
            /* [in] */ short in_nCol,
            /* [in] */ double pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Copy( 
            /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Set( 
            /* [in] */ double in_dM00,
            /* [in] */ double in_dM01,
            /* [in] */ double in_dM02,
            /* [in] */ double in_dM10,
            /* [in] */ double in_dM11,
            /* [in] */ double in_dM12,
            /* [in] */ double in_dM020,
            /* [in] */ double in_dM21,
            /* [in] */ double in_dM22) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Mul( 
            /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix1,
            /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix2) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MulInPlace( 
            /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Transpose( 
            /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TransposeInPlace( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TransposeInverse( 
            /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix,
            /* [retval][out] */ long __RPC_FAR *out_pbInverted) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TransposeInverseInPlace( 
            /* [retval][out] */ long __RPC_FAR *out_pbInverted) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Invert( 
            /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix,
            /* [retval][out] */ long __RPC_FAR *out_pbInverted) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InvertInPlace( 
            /* [retval][out] */ long __RPC_FAR *out_pbInverted) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetIdentity( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Get( 
            /* [out] */ VARIANT __RPC_FAR *out_pvarM00,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM01,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM02,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM10,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM11,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM12,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM020,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM21,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM22) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISIMatrix3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISIMatrix3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISIMatrix3 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISIMatrix3 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISIMatrix3 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISIMatrix3 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISIMatrix3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISIMatrix3 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            ISIMatrix3 __RPC_FAR * This,
            /* [in] */ short in_nRow,
            /* [in] */ short in_nCol,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Value )( 
            ISIMatrix3 __RPC_FAR * This,
            /* [in] */ short in_nRow,
            /* [in] */ short in_nCol,
            /* [in] */ double pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Copy )( 
            ISIMatrix3 __RPC_FAR * This,
            /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Set )( 
            ISIMatrix3 __RPC_FAR * This,
            /* [in] */ double in_dM00,
            /* [in] */ double in_dM01,
            /* [in] */ double in_dM02,
            /* [in] */ double in_dM10,
            /* [in] */ double in_dM11,
            /* [in] */ double in_dM12,
            /* [in] */ double in_dM020,
            /* [in] */ double in_dM21,
            /* [in] */ double in_dM22);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Mul )( 
            ISIMatrix3 __RPC_FAR * This,
            /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix1,
            /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix2);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MulInPlace )( 
            ISIMatrix3 __RPC_FAR * This,
            /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Transpose )( 
            ISIMatrix3 __RPC_FAR * This,
            /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TransposeInPlace )( 
            ISIMatrix3 __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TransposeInverse )( 
            ISIMatrix3 __RPC_FAR * This,
            /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix,
            /* [retval][out] */ long __RPC_FAR *out_pbInverted);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TransposeInverseInPlace )( 
            ISIMatrix3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *out_pbInverted);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invert )( 
            ISIMatrix3 __RPC_FAR * This,
            /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix,
            /* [retval][out] */ long __RPC_FAR *out_pbInverted);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InvertInPlace )( 
            ISIMatrix3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *out_pbInverted);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetIdentity )( 
            ISIMatrix3 __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get )( 
            ISIMatrix3 __RPC_FAR * This,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM00,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM01,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM02,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM10,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM11,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM12,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM020,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM21,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM22);
        
        END_INTERFACE
    } ISIMatrix3Vtbl;

    interface ISIMatrix3
    {
        CONST_VTBL struct ISIMatrix3Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISIMatrix3_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISIMatrix3_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISIMatrix3_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISIMatrix3_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISIMatrix3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISIMatrix3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISIMatrix3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISIMatrix3_get_Value(This,in_nRow,in_nCol,pVal)	\
    (This)->lpVtbl -> get_Value(This,in_nRow,in_nCol,pVal)

#define ISIMatrix3_put_Value(This,in_nRow,in_nCol,pVal)	\
    (This)->lpVtbl -> put_Value(This,in_nRow,in_nCol,pVal)

#define ISIMatrix3_Copy(This,in_pMatrix)	\
    (This)->lpVtbl -> Copy(This,in_pMatrix)

#define ISIMatrix3_Set(This,in_dM00,in_dM01,in_dM02,in_dM10,in_dM11,in_dM12,in_dM020,in_dM21,in_dM22)	\
    (This)->lpVtbl -> Set(This,in_dM00,in_dM01,in_dM02,in_dM10,in_dM11,in_dM12,in_dM020,in_dM21,in_dM22)

#define ISIMatrix3_Mul(This,in_pMatrix1,in_pMatrix2)	\
    (This)->lpVtbl -> Mul(This,in_pMatrix1,in_pMatrix2)

#define ISIMatrix3_MulInPlace(This,in_pMatrix)	\
    (This)->lpVtbl -> MulInPlace(This,in_pMatrix)

#define ISIMatrix3_Transpose(This,in_pMatrix)	\
    (This)->lpVtbl -> Transpose(This,in_pMatrix)

#define ISIMatrix3_TransposeInPlace(This)	\
    (This)->lpVtbl -> TransposeInPlace(This)

#define ISIMatrix3_TransposeInverse(This,in_pMatrix,out_pbInverted)	\
    (This)->lpVtbl -> TransposeInverse(This,in_pMatrix,out_pbInverted)

#define ISIMatrix3_TransposeInverseInPlace(This,out_pbInverted)	\
    (This)->lpVtbl -> TransposeInverseInPlace(This,out_pbInverted)

#define ISIMatrix3_Invert(This,in_pMatrix,out_pbInverted)	\
    (This)->lpVtbl -> Invert(This,in_pMatrix,out_pbInverted)

#define ISIMatrix3_InvertInPlace(This,out_pbInverted)	\
    (This)->lpVtbl -> InvertInPlace(This,out_pbInverted)

#define ISIMatrix3_SetIdentity(This)	\
    (This)->lpVtbl -> SetIdentity(This)

#define ISIMatrix3_Get(This,out_pvarM00,out_pvarM01,out_pvarM02,out_pvarM10,out_pvarM11,out_pvarM12,out_pvarM020,out_pvarM21,out_pvarM22)	\
    (This)->lpVtbl -> Get(This,out_pvarM00,out_pvarM01,out_pvarM02,out_pvarM10,out_pvarM11,out_pvarM12,out_pvarM020,out_pvarM21,out_pvarM22)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix3_get_Value_Proxy( 
    ISIMatrix3 __RPC_FAR * This,
    /* [in] */ short in_nRow,
    /* [in] */ short in_nCol,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB ISIMatrix3_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix3_put_Value_Proxy( 
    ISIMatrix3 __RPC_FAR * This,
    /* [in] */ short in_nRow,
    /* [in] */ short in_nCol,
    /* [in] */ double pVal);


void __RPC_STUB ISIMatrix3_put_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix3_Copy_Proxy( 
    ISIMatrix3 __RPC_FAR * This,
    /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix);


void __RPC_STUB ISIMatrix3_Copy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix3_Set_Proxy( 
    ISIMatrix3 __RPC_FAR * This,
    /* [in] */ double in_dM00,
    /* [in] */ double in_dM01,
    /* [in] */ double in_dM02,
    /* [in] */ double in_dM10,
    /* [in] */ double in_dM11,
    /* [in] */ double in_dM12,
    /* [in] */ double in_dM020,
    /* [in] */ double in_dM21,
    /* [in] */ double in_dM22);


void __RPC_STUB ISIMatrix3_Set_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix3_Mul_Proxy( 
    ISIMatrix3 __RPC_FAR * This,
    /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix1,
    /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix2);


void __RPC_STUB ISIMatrix3_Mul_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix3_MulInPlace_Proxy( 
    ISIMatrix3 __RPC_FAR * This,
    /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix);


void __RPC_STUB ISIMatrix3_MulInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix3_Transpose_Proxy( 
    ISIMatrix3 __RPC_FAR * This,
    /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix);


void __RPC_STUB ISIMatrix3_Transpose_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix3_TransposeInPlace_Proxy( 
    ISIMatrix3 __RPC_FAR * This);


void __RPC_STUB ISIMatrix3_TransposeInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix3_TransposeInverse_Proxy( 
    ISIMatrix3 __RPC_FAR * This,
    /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix,
    /* [retval][out] */ long __RPC_FAR *out_pbInverted);


void __RPC_STUB ISIMatrix3_TransposeInverse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix3_TransposeInverseInPlace_Proxy( 
    ISIMatrix3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *out_pbInverted);


void __RPC_STUB ISIMatrix3_TransposeInverseInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix3_Invert_Proxy( 
    ISIMatrix3 __RPC_FAR * This,
    /* [in] */ ISIMatrix3 __RPC_FAR *in_pMatrix,
    /* [retval][out] */ long __RPC_FAR *out_pbInverted);


void __RPC_STUB ISIMatrix3_Invert_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix3_InvertInPlace_Proxy( 
    ISIMatrix3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *out_pbInverted);


void __RPC_STUB ISIMatrix3_InvertInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix3_SetIdentity_Proxy( 
    ISIMatrix3 __RPC_FAR * This);


void __RPC_STUB ISIMatrix3_SetIdentity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix3_Get_Proxy( 
    ISIMatrix3 __RPC_FAR * This,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM00,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM01,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM02,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM10,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM11,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM12,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM020,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM21,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM22);


void __RPC_STUB ISIMatrix3_Get_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISIMatrix3_INTERFACE_DEFINED__ */


#ifndef __ISIMatrix4_INTERFACE_DEFINED__
#define __ISIMatrix4_INTERFACE_DEFINED__

/* interface ISIMatrix4 */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_ISIMatrix4;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BFB3B915-24CC-11D3-BC3A-00A0C9612B97")
    ISIMatrix4 : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [in] */ short in_nRow,
            /* [in] */ short in_nCol,
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ short in_nRow,
            /* [in] */ short in_nCol,
            /* [in] */ double pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Copy( 
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Set( 
            /* [in] */ double in_dM00,
            /* [in] */ double in_dM01,
            /* [in] */ double in_dM02,
            /* [in] */ double in_dM03,
            /* [in] */ double in_dM10,
            /* [in] */ double in_dM11,
            /* [in] */ double in_dM12,
            /* [in] */ double in_dM13,
            /* [in] */ double in_dM20,
            /* [in] */ double in_dM21,
            /* [in] */ double in_dM22,
            /* [in] */ double in_dM23,
            /* [in] */ double in_dM30,
            /* [in] */ double in_dM31,
            /* [in] */ double in_dM32,
            /* [in] */ double in_dM33) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Get( 
            /* [out] */ VARIANT __RPC_FAR *out_pvarM00,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM01,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM02,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM03,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM10,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM11,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM12,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM13,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM20,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM21,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM22,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM23,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM30,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM31,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM32,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM33) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Mul( 
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix1,
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix2) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MulInPlace( 
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Transpose( 
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TransposeInPlace( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TransposeInverse( 
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix,
            /* [retval][out] */ long __RPC_FAR *out_pbInverted) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TransposeInverseInPlace( 
            /* [retval][out] */ long __RPC_FAR *out_pbInverted) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Invert( 
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix,
            /* [retval][out] */ long __RPC_FAR *out_pbInverted) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InvertInPlace( 
            /* [retval][out] */ long __RPC_FAR *out_pbInverted) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetIdentity( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISIMatrix4Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISIMatrix4 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISIMatrix4 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISIMatrix4 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISIMatrix4 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISIMatrix4 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISIMatrix4 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISIMatrix4 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            ISIMatrix4 __RPC_FAR * This,
            /* [in] */ short in_nRow,
            /* [in] */ short in_nCol,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Value )( 
            ISIMatrix4 __RPC_FAR * This,
            /* [in] */ short in_nRow,
            /* [in] */ short in_nCol,
            /* [in] */ double pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Copy )( 
            ISIMatrix4 __RPC_FAR * This,
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Set )( 
            ISIMatrix4 __RPC_FAR * This,
            /* [in] */ double in_dM00,
            /* [in] */ double in_dM01,
            /* [in] */ double in_dM02,
            /* [in] */ double in_dM03,
            /* [in] */ double in_dM10,
            /* [in] */ double in_dM11,
            /* [in] */ double in_dM12,
            /* [in] */ double in_dM13,
            /* [in] */ double in_dM20,
            /* [in] */ double in_dM21,
            /* [in] */ double in_dM22,
            /* [in] */ double in_dM23,
            /* [in] */ double in_dM30,
            /* [in] */ double in_dM31,
            /* [in] */ double in_dM32,
            /* [in] */ double in_dM33);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get )( 
            ISIMatrix4 __RPC_FAR * This,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM00,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM01,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM02,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM03,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM10,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM11,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM12,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM13,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM20,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM21,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM22,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM23,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM30,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM31,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM32,
            /* [out] */ VARIANT __RPC_FAR *out_pvarM33);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Mul )( 
            ISIMatrix4 __RPC_FAR * This,
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix1,
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix2);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MulInPlace )( 
            ISIMatrix4 __RPC_FAR * This,
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Transpose )( 
            ISIMatrix4 __RPC_FAR * This,
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TransposeInPlace )( 
            ISIMatrix4 __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TransposeInverse )( 
            ISIMatrix4 __RPC_FAR * This,
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix,
            /* [retval][out] */ long __RPC_FAR *out_pbInverted);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TransposeInverseInPlace )( 
            ISIMatrix4 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *out_pbInverted);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invert )( 
            ISIMatrix4 __RPC_FAR * This,
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix,
            /* [retval][out] */ long __RPC_FAR *out_pbInverted);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InvertInPlace )( 
            ISIMatrix4 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *out_pbInverted);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetIdentity )( 
            ISIMatrix4 __RPC_FAR * This);
        
        END_INTERFACE
    } ISIMatrix4Vtbl;

    interface ISIMatrix4
    {
        CONST_VTBL struct ISIMatrix4Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISIMatrix4_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISIMatrix4_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISIMatrix4_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISIMatrix4_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISIMatrix4_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISIMatrix4_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISIMatrix4_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISIMatrix4_get_Value(This,in_nRow,in_nCol,pVal)	\
    (This)->lpVtbl -> get_Value(This,in_nRow,in_nCol,pVal)

#define ISIMatrix4_put_Value(This,in_nRow,in_nCol,pVal)	\
    (This)->lpVtbl -> put_Value(This,in_nRow,in_nCol,pVal)

#define ISIMatrix4_Copy(This,in_pMatrix)	\
    (This)->lpVtbl -> Copy(This,in_pMatrix)

#define ISIMatrix4_Set(This,in_dM00,in_dM01,in_dM02,in_dM03,in_dM10,in_dM11,in_dM12,in_dM13,in_dM20,in_dM21,in_dM22,in_dM23,in_dM30,in_dM31,in_dM32,in_dM33)	\
    (This)->lpVtbl -> Set(This,in_dM00,in_dM01,in_dM02,in_dM03,in_dM10,in_dM11,in_dM12,in_dM13,in_dM20,in_dM21,in_dM22,in_dM23,in_dM30,in_dM31,in_dM32,in_dM33)

#define ISIMatrix4_Get(This,out_pvarM00,out_pvarM01,out_pvarM02,out_pvarM03,out_pvarM10,out_pvarM11,out_pvarM12,out_pvarM13,out_pvarM20,out_pvarM21,out_pvarM22,out_pvarM23,out_pvarM30,out_pvarM31,out_pvarM32,out_pvarM33)	\
    (This)->lpVtbl -> Get(This,out_pvarM00,out_pvarM01,out_pvarM02,out_pvarM03,out_pvarM10,out_pvarM11,out_pvarM12,out_pvarM13,out_pvarM20,out_pvarM21,out_pvarM22,out_pvarM23,out_pvarM30,out_pvarM31,out_pvarM32,out_pvarM33)

#define ISIMatrix4_Mul(This,in_pMatrix1,in_pMatrix2)	\
    (This)->lpVtbl -> Mul(This,in_pMatrix1,in_pMatrix2)

#define ISIMatrix4_MulInPlace(This,in_pMatrix)	\
    (This)->lpVtbl -> MulInPlace(This,in_pMatrix)

#define ISIMatrix4_Transpose(This,in_pMatrix)	\
    (This)->lpVtbl -> Transpose(This,in_pMatrix)

#define ISIMatrix4_TransposeInPlace(This)	\
    (This)->lpVtbl -> TransposeInPlace(This)

#define ISIMatrix4_TransposeInverse(This,in_pMatrix,out_pbInverted)	\
    (This)->lpVtbl -> TransposeInverse(This,in_pMatrix,out_pbInverted)

#define ISIMatrix4_TransposeInverseInPlace(This,out_pbInverted)	\
    (This)->lpVtbl -> TransposeInverseInPlace(This,out_pbInverted)

#define ISIMatrix4_Invert(This,in_pMatrix,out_pbInverted)	\
    (This)->lpVtbl -> Invert(This,in_pMatrix,out_pbInverted)

#define ISIMatrix4_InvertInPlace(This,out_pbInverted)	\
    (This)->lpVtbl -> InvertInPlace(This,out_pbInverted)

#define ISIMatrix4_SetIdentity(This)	\
    (This)->lpVtbl -> SetIdentity(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix4_get_Value_Proxy( 
    ISIMatrix4 __RPC_FAR * This,
    /* [in] */ short in_nRow,
    /* [in] */ short in_nCol,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB ISIMatrix4_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix4_put_Value_Proxy( 
    ISIMatrix4 __RPC_FAR * This,
    /* [in] */ short in_nRow,
    /* [in] */ short in_nCol,
    /* [in] */ double pVal);


void __RPC_STUB ISIMatrix4_put_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix4_Copy_Proxy( 
    ISIMatrix4 __RPC_FAR * This,
    /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix);


void __RPC_STUB ISIMatrix4_Copy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix4_Set_Proxy( 
    ISIMatrix4 __RPC_FAR * This,
    /* [in] */ double in_dM00,
    /* [in] */ double in_dM01,
    /* [in] */ double in_dM02,
    /* [in] */ double in_dM03,
    /* [in] */ double in_dM10,
    /* [in] */ double in_dM11,
    /* [in] */ double in_dM12,
    /* [in] */ double in_dM13,
    /* [in] */ double in_dM20,
    /* [in] */ double in_dM21,
    /* [in] */ double in_dM22,
    /* [in] */ double in_dM23,
    /* [in] */ double in_dM30,
    /* [in] */ double in_dM31,
    /* [in] */ double in_dM32,
    /* [in] */ double in_dM33);


void __RPC_STUB ISIMatrix4_Set_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix4_Get_Proxy( 
    ISIMatrix4 __RPC_FAR * This,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM00,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM01,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM02,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM03,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM10,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM11,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM12,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM13,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM20,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM21,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM22,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM23,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM30,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM31,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM32,
    /* [out] */ VARIANT __RPC_FAR *out_pvarM33);


void __RPC_STUB ISIMatrix4_Get_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix4_Mul_Proxy( 
    ISIMatrix4 __RPC_FAR * This,
    /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix1,
    /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix2);


void __RPC_STUB ISIMatrix4_Mul_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix4_MulInPlace_Proxy( 
    ISIMatrix4 __RPC_FAR * This,
    /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix);


void __RPC_STUB ISIMatrix4_MulInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix4_Transpose_Proxy( 
    ISIMatrix4 __RPC_FAR * This,
    /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix);


void __RPC_STUB ISIMatrix4_Transpose_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix4_TransposeInPlace_Proxy( 
    ISIMatrix4 __RPC_FAR * This);


void __RPC_STUB ISIMatrix4_TransposeInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix4_TransposeInverse_Proxy( 
    ISIMatrix4 __RPC_FAR * This,
    /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix,
    /* [retval][out] */ long __RPC_FAR *out_pbInverted);


void __RPC_STUB ISIMatrix4_TransposeInverse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix4_TransposeInverseInPlace_Proxy( 
    ISIMatrix4 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *out_pbInverted);


void __RPC_STUB ISIMatrix4_TransposeInverseInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix4_Invert_Proxy( 
    ISIMatrix4 __RPC_FAR * This,
    /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix,
    /* [retval][out] */ long __RPC_FAR *out_pbInverted);


void __RPC_STUB ISIMatrix4_Invert_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix4_InvertInPlace_Proxy( 
    ISIMatrix4 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *out_pbInverted);


void __RPC_STUB ISIMatrix4_InvertInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIMatrix4_SetIdentity_Proxy( 
    ISIMatrix4 __RPC_FAR * This);


void __RPC_STUB ISIMatrix4_SetIdentity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISIMatrix4_INTERFACE_DEFINED__ */


#ifndef __ISIRotation_INTERFACE_DEFINED__
#define __ISIRotation_INTERFACE_DEFINED__

/* interface ISIRotation */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_ISIRotation;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BFB3B917-24CC-11D3-BC3A-00A0C9612B97")
    ISIRotation : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetIdentity( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMatrix3( 
            /* [in] */ ISIMatrix3 __RPC_FAR *io_pMatrix) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetFromXYZAngles( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pXZYAngles) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetXYZAngles( 
            /* [in] */ ISIVector3 __RPC_FAR *io_pXZYAngles) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetFromAxisAngle( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pAxis,
            /* [in] */ double in_dAngle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAxisAngle( 
            /* [in] */ ISIVector3 __RPC_FAR *io_pAxis,
            /* [out] */ VARIANT __RPC_FAR *io_pvarAngle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetFromQuaternion( 
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetQuaternion( 
            /* [in] */ ISIQuaternion __RPC_FAR *io_pQuat) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Invert( 
            /* [in] */ ISIRotation __RPC_FAR *in_pRot) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InvertInPlace( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Mul( 
            /* [in] */ ISIRotation __RPC_FAR *in_pRot1,
            /* [in] */ ISIRotation __RPC_FAR *in_pRot2) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MulInPlace( 
            /* [in] */ ISIRotation __RPC_FAR *in_pRot) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Copy( 
            /* [in] */ ISIRotation __RPC_FAR *in_pSrcRot) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetFromXYZAxes( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pXAxis,
            /* [in] */ ISIVector3 __RPC_FAR *in_pYAxis,
            /* [in] */ ISIVector3 __RPC_FAR *in_pZAxis) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetXYZAnglesValues( 
            /* [out] */ VARIANT __RPC_FAR *out_pvarX,
            /* [out] */ VARIANT __RPC_FAR *out_pvarY,
            /* [out] */ VARIANT __RPC_FAR *out_pvarZ) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetFromXYZAnglesValues( 
            /* [in] */ double in_dX,
            /* [in] */ double in_dY,
            /* [in] */ double in_dZ) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISIRotationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISIRotation __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISIRotation __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISIRotation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISIRotation __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISIRotation __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISIRotation __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISIRotation __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetIdentity )( 
            ISIRotation __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMatrix3 )( 
            ISIRotation __RPC_FAR * This,
            /* [in] */ ISIMatrix3 __RPC_FAR *io_pMatrix);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetFromXYZAngles )( 
            ISIRotation __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pXZYAngles);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetXYZAngles )( 
            ISIRotation __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *io_pXZYAngles);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetFromAxisAngle )( 
            ISIRotation __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pAxis,
            /* [in] */ double in_dAngle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAxisAngle )( 
            ISIRotation __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *io_pAxis,
            /* [out] */ VARIANT __RPC_FAR *io_pvarAngle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetFromQuaternion )( 
            ISIRotation __RPC_FAR * This,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetQuaternion )( 
            ISIRotation __RPC_FAR * This,
            /* [in] */ ISIQuaternion __RPC_FAR *io_pQuat);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invert )( 
            ISIRotation __RPC_FAR * This,
            /* [in] */ ISIRotation __RPC_FAR *in_pRot);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InvertInPlace )( 
            ISIRotation __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Mul )( 
            ISIRotation __RPC_FAR * This,
            /* [in] */ ISIRotation __RPC_FAR *in_pRot1,
            /* [in] */ ISIRotation __RPC_FAR *in_pRot2);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MulInPlace )( 
            ISIRotation __RPC_FAR * This,
            /* [in] */ ISIRotation __RPC_FAR *in_pRot);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Copy )( 
            ISIRotation __RPC_FAR * This,
            /* [in] */ ISIRotation __RPC_FAR *in_pSrcRot);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetFromXYZAxes )( 
            ISIRotation __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pXAxis,
            /* [in] */ ISIVector3 __RPC_FAR *in_pYAxis,
            /* [in] */ ISIVector3 __RPC_FAR *in_pZAxis);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetXYZAnglesValues )( 
            ISIRotation __RPC_FAR * This,
            /* [out] */ VARIANT __RPC_FAR *out_pvarX,
            /* [out] */ VARIANT __RPC_FAR *out_pvarY,
            /* [out] */ VARIANT __RPC_FAR *out_pvarZ);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetFromXYZAnglesValues )( 
            ISIRotation __RPC_FAR * This,
            /* [in] */ double in_dX,
            /* [in] */ double in_dY,
            /* [in] */ double in_dZ);
        
        END_INTERFACE
    } ISIRotationVtbl;

    interface ISIRotation
    {
        CONST_VTBL struct ISIRotationVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISIRotation_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISIRotation_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISIRotation_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISIRotation_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISIRotation_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISIRotation_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISIRotation_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISIRotation_SetIdentity(This)	\
    (This)->lpVtbl -> SetIdentity(This)

#define ISIRotation_GetMatrix3(This,io_pMatrix)	\
    (This)->lpVtbl -> GetMatrix3(This,io_pMatrix)

#define ISIRotation_SetFromXYZAngles(This,in_pXZYAngles)	\
    (This)->lpVtbl -> SetFromXYZAngles(This,in_pXZYAngles)

#define ISIRotation_GetXYZAngles(This,io_pXZYAngles)	\
    (This)->lpVtbl -> GetXYZAngles(This,io_pXZYAngles)

#define ISIRotation_SetFromAxisAngle(This,in_pAxis,in_dAngle)	\
    (This)->lpVtbl -> SetFromAxisAngle(This,in_pAxis,in_dAngle)

#define ISIRotation_GetAxisAngle(This,io_pAxis,io_pvarAngle)	\
    (This)->lpVtbl -> GetAxisAngle(This,io_pAxis,io_pvarAngle)

#define ISIRotation_SetFromQuaternion(This,in_pQuat)	\
    (This)->lpVtbl -> SetFromQuaternion(This,in_pQuat)

#define ISIRotation_GetQuaternion(This,io_pQuat)	\
    (This)->lpVtbl -> GetQuaternion(This,io_pQuat)

#define ISIRotation_Invert(This,in_pRot)	\
    (This)->lpVtbl -> Invert(This,in_pRot)

#define ISIRotation_InvertInPlace(This)	\
    (This)->lpVtbl -> InvertInPlace(This)

#define ISIRotation_Mul(This,in_pRot1,in_pRot2)	\
    (This)->lpVtbl -> Mul(This,in_pRot1,in_pRot2)

#define ISIRotation_MulInPlace(This,in_pRot)	\
    (This)->lpVtbl -> MulInPlace(This,in_pRot)

#define ISIRotation_Copy(This,in_pSrcRot)	\
    (This)->lpVtbl -> Copy(This,in_pSrcRot)

#define ISIRotation_SetFromXYZAxes(This,in_pXAxis,in_pYAxis,in_pZAxis)	\
    (This)->lpVtbl -> SetFromXYZAxes(This,in_pXAxis,in_pYAxis,in_pZAxis)

#define ISIRotation_GetXYZAnglesValues(This,out_pvarX,out_pvarY,out_pvarZ)	\
    (This)->lpVtbl -> GetXYZAnglesValues(This,out_pvarX,out_pvarY,out_pvarZ)

#define ISIRotation_SetFromXYZAnglesValues(This,in_dX,in_dY,in_dZ)	\
    (This)->lpVtbl -> SetFromXYZAnglesValues(This,in_dX,in_dY,in_dZ)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIRotation_SetIdentity_Proxy( 
    ISIRotation __RPC_FAR * This);


void __RPC_STUB ISIRotation_SetIdentity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIRotation_GetMatrix3_Proxy( 
    ISIRotation __RPC_FAR * This,
    /* [in] */ ISIMatrix3 __RPC_FAR *io_pMatrix);


void __RPC_STUB ISIRotation_GetMatrix3_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIRotation_SetFromXYZAngles_Proxy( 
    ISIRotation __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pXZYAngles);


void __RPC_STUB ISIRotation_SetFromXYZAngles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIRotation_GetXYZAngles_Proxy( 
    ISIRotation __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *io_pXZYAngles);


void __RPC_STUB ISIRotation_GetXYZAngles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIRotation_SetFromAxisAngle_Proxy( 
    ISIRotation __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pAxis,
    /* [in] */ double in_dAngle);


void __RPC_STUB ISIRotation_SetFromAxisAngle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIRotation_GetAxisAngle_Proxy( 
    ISIRotation __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *io_pAxis,
    /* [out] */ VARIANT __RPC_FAR *io_pvarAngle);


void __RPC_STUB ISIRotation_GetAxisAngle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIRotation_SetFromQuaternion_Proxy( 
    ISIRotation __RPC_FAR * This,
    /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat);


void __RPC_STUB ISIRotation_SetFromQuaternion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIRotation_GetQuaternion_Proxy( 
    ISIRotation __RPC_FAR * This,
    /* [in] */ ISIQuaternion __RPC_FAR *io_pQuat);


void __RPC_STUB ISIRotation_GetQuaternion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIRotation_Invert_Proxy( 
    ISIRotation __RPC_FAR * This,
    /* [in] */ ISIRotation __RPC_FAR *in_pRot);


void __RPC_STUB ISIRotation_Invert_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIRotation_InvertInPlace_Proxy( 
    ISIRotation __RPC_FAR * This);


void __RPC_STUB ISIRotation_InvertInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIRotation_Mul_Proxy( 
    ISIRotation __RPC_FAR * This,
    /* [in] */ ISIRotation __RPC_FAR *in_pRot1,
    /* [in] */ ISIRotation __RPC_FAR *in_pRot2);


void __RPC_STUB ISIRotation_Mul_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIRotation_MulInPlace_Proxy( 
    ISIRotation __RPC_FAR * This,
    /* [in] */ ISIRotation __RPC_FAR *in_pRot);


void __RPC_STUB ISIRotation_MulInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIRotation_Copy_Proxy( 
    ISIRotation __RPC_FAR * This,
    /* [in] */ ISIRotation __RPC_FAR *in_pSrcRot);


void __RPC_STUB ISIRotation_Copy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIRotation_SetFromXYZAxes_Proxy( 
    ISIRotation __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pXAxis,
    /* [in] */ ISIVector3 __RPC_FAR *in_pYAxis,
    /* [in] */ ISIVector3 __RPC_FAR *in_pZAxis);


void __RPC_STUB ISIRotation_SetFromXYZAxes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIRotation_GetXYZAnglesValues_Proxy( 
    ISIRotation __RPC_FAR * This,
    /* [out] */ VARIANT __RPC_FAR *out_pvarX,
    /* [out] */ VARIANT __RPC_FAR *out_pvarY,
    /* [out] */ VARIANT __RPC_FAR *out_pvarZ);


void __RPC_STUB ISIRotation_GetXYZAnglesValues_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIRotation_SetFromXYZAnglesValues_Proxy( 
    ISIRotation __RPC_FAR * This,
    /* [in] */ double in_dX,
    /* [in] */ double in_dY,
    /* [in] */ double in_dZ);


void __RPC_STUB ISIRotation_SetFromXYZAnglesValues_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISIRotation_INTERFACE_DEFINED__ */


#ifndef __ISIQuaternion_INTERFACE_DEFINED__
#define __ISIQuaternion_INTERFACE_DEFINED__

/* interface ISIQuaternion */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_ISIQuaternion;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BFB3B919-24CC-11D3-BC3A-00A0C9612B97")
    ISIQuaternion : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [in] */ short in_nIndex,
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ short in_nIndex,
            /* [in] */ double pVal) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_X( 
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_X( 
            /* [in] */ double pVal) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Y( 
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Y( 
            /* [in] */ double pVal) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Z( 
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Z( 
            /* [in] */ double pVal) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_W( 
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_W( 
            /* [in] */ double pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Set( 
            /* [in] */ double in_dW,
            /* [in] */ double in_dX,
            /* [in] */ double in_dY,
            /* [in] */ double in_dZ) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Get( 
            /* [out] */ VARIANT __RPC_FAR *out_pvarW,
            /* [out] */ VARIANT __RPC_FAR *out_pvarX,
            /* [out] */ VARIANT __RPC_FAR *out_pvarY,
            /* [out] */ VARIANT __RPC_FAR *out_pvarZ) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Copy( 
            /* [in] */ ISIQuaternion __RPC_FAR *in_pSrcQuat) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetIdentity( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Invert( 
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InvertInPlace( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Conjugate( 
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ConjugateInPlace( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat1,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat2) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddInPlace( 
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Length( 
            /* [retval][out] */ double __RPC_FAR *out_pdLength) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LengthSquared( 
            /* [retval][out] */ double __RPC_FAR *out_pdLength) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Negate( 
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE NegateInPlace( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Sub( 
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat1,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat2) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SubInPlace( 
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Mul( 
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat1,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat2) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MulInPlace( 
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SLerp( 
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat1,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat2,
            /* [in] */ double in_dU) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Equals( 
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat,
            /* [retval][out] */ long __RPC_FAR *out_pbEquality) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EpsilonEquals( 
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat,
            /* [in] */ double in_dEpsilon,
            /* [retval][out] */ long __RPC_FAR *out_pbEquality) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Normalize( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISIQuaternionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISIQuaternion __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISIQuaternion __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISIQuaternion __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ short in_nIndex,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Value )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ short in_nIndex,
            /* [in] */ double pVal);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_X )( 
            ISIQuaternion __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_X )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ double pVal);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Y )( 
            ISIQuaternion __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Y )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ double pVal);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Z )( 
            ISIQuaternion __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Z )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ double pVal);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_W )( 
            ISIQuaternion __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_W )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ double pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Set )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ double in_dW,
            /* [in] */ double in_dX,
            /* [in] */ double in_dY,
            /* [in] */ double in_dZ);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get )( 
            ISIQuaternion __RPC_FAR * This,
            /* [out] */ VARIANT __RPC_FAR *out_pvarW,
            /* [out] */ VARIANT __RPC_FAR *out_pvarX,
            /* [out] */ VARIANT __RPC_FAR *out_pvarY,
            /* [out] */ VARIANT __RPC_FAR *out_pvarZ);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Copy )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pSrcQuat);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetIdentity )( 
            ISIQuaternion __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invert )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InvertInPlace )( 
            ISIQuaternion __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Conjugate )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConjugateInPlace )( 
            ISIQuaternion __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat1,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat2);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddInPlace )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Length )( 
            ISIQuaternion __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *out_pdLength);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LengthSquared )( 
            ISIQuaternion __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *out_pdLength);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Negate )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *NegateInPlace )( 
            ISIQuaternion __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Sub )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat1,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat2);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SubInPlace )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Mul )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat1,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat2);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MulInPlace )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SLerp )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat1,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat2,
            /* [in] */ double in_dU);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Equals )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat,
            /* [retval][out] */ long __RPC_FAR *out_pbEquality);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EpsilonEquals )( 
            ISIQuaternion __RPC_FAR * This,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat,
            /* [in] */ double in_dEpsilon,
            /* [retval][out] */ long __RPC_FAR *out_pbEquality);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Normalize )( 
            ISIQuaternion __RPC_FAR * This);
        
        END_INTERFACE
    } ISIQuaternionVtbl;

    interface ISIQuaternion
    {
        CONST_VTBL struct ISIQuaternionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISIQuaternion_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISIQuaternion_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISIQuaternion_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISIQuaternion_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISIQuaternion_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISIQuaternion_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISIQuaternion_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISIQuaternion_get_Value(This,in_nIndex,pVal)	\
    (This)->lpVtbl -> get_Value(This,in_nIndex,pVal)

#define ISIQuaternion_put_Value(This,in_nIndex,pVal)	\
    (This)->lpVtbl -> put_Value(This,in_nIndex,pVal)

#define ISIQuaternion_get_X(This,pVal)	\
    (This)->lpVtbl -> get_X(This,pVal)

#define ISIQuaternion_put_X(This,pVal)	\
    (This)->lpVtbl -> put_X(This,pVal)

#define ISIQuaternion_get_Y(This,pVal)	\
    (This)->lpVtbl -> get_Y(This,pVal)

#define ISIQuaternion_put_Y(This,pVal)	\
    (This)->lpVtbl -> put_Y(This,pVal)

#define ISIQuaternion_get_Z(This,pVal)	\
    (This)->lpVtbl -> get_Z(This,pVal)

#define ISIQuaternion_put_Z(This,pVal)	\
    (This)->lpVtbl -> put_Z(This,pVal)

#define ISIQuaternion_get_W(This,pVal)	\
    (This)->lpVtbl -> get_W(This,pVal)

#define ISIQuaternion_put_W(This,pVal)	\
    (This)->lpVtbl -> put_W(This,pVal)

#define ISIQuaternion_Set(This,in_dW,in_dX,in_dY,in_dZ)	\
    (This)->lpVtbl -> Set(This,in_dW,in_dX,in_dY,in_dZ)

#define ISIQuaternion_Get(This,out_pvarW,out_pvarX,out_pvarY,out_pvarZ)	\
    (This)->lpVtbl -> Get(This,out_pvarW,out_pvarX,out_pvarY,out_pvarZ)

#define ISIQuaternion_Copy(This,in_pSrcQuat)	\
    (This)->lpVtbl -> Copy(This,in_pSrcQuat)

#define ISIQuaternion_SetIdentity(This)	\
    (This)->lpVtbl -> SetIdentity(This)

#define ISIQuaternion_Invert(This,in_pQuat)	\
    (This)->lpVtbl -> Invert(This,in_pQuat)

#define ISIQuaternion_InvertInPlace(This)	\
    (This)->lpVtbl -> InvertInPlace(This)

#define ISIQuaternion_Conjugate(This,in_pQuat)	\
    (This)->lpVtbl -> Conjugate(This,in_pQuat)

#define ISIQuaternion_ConjugateInPlace(This)	\
    (This)->lpVtbl -> ConjugateInPlace(This)

#define ISIQuaternion_Add(This,in_pQuat1,in_pQuat2)	\
    (This)->lpVtbl -> Add(This,in_pQuat1,in_pQuat2)

#define ISIQuaternion_AddInPlace(This,in_pQuat)	\
    (This)->lpVtbl -> AddInPlace(This,in_pQuat)

#define ISIQuaternion_Length(This,out_pdLength)	\
    (This)->lpVtbl -> Length(This,out_pdLength)

#define ISIQuaternion_LengthSquared(This,out_pdLength)	\
    (This)->lpVtbl -> LengthSquared(This,out_pdLength)

#define ISIQuaternion_Negate(This,in_pQuat)	\
    (This)->lpVtbl -> Negate(This,in_pQuat)

#define ISIQuaternion_NegateInPlace(This)	\
    (This)->lpVtbl -> NegateInPlace(This)

#define ISIQuaternion_Sub(This,in_pQuat1,in_pQuat2)	\
    (This)->lpVtbl -> Sub(This,in_pQuat1,in_pQuat2)

#define ISIQuaternion_SubInPlace(This,in_pQuat)	\
    (This)->lpVtbl -> SubInPlace(This,in_pQuat)

#define ISIQuaternion_Mul(This,in_pQuat1,in_pQuat2)	\
    (This)->lpVtbl -> Mul(This,in_pQuat1,in_pQuat2)

#define ISIQuaternion_MulInPlace(This,in_pQuat)	\
    (This)->lpVtbl -> MulInPlace(This,in_pQuat)

#define ISIQuaternion_SLerp(This,in_pQuat1,in_pQuat2,in_dU)	\
    (This)->lpVtbl -> SLerp(This,in_pQuat1,in_pQuat2,in_dU)

#define ISIQuaternion_Equals(This,in_pQuat,out_pbEquality)	\
    (This)->lpVtbl -> Equals(This,in_pQuat,out_pbEquality)

#define ISIQuaternion_EpsilonEquals(This,in_pQuat,in_dEpsilon,out_pbEquality)	\
    (This)->lpVtbl -> EpsilonEquals(This,in_pQuat,in_dEpsilon,out_pbEquality)

#define ISIQuaternion_Normalize(This)	\
    (This)->lpVtbl -> Normalize(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_get_Value_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ short in_nIndex,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB ISIQuaternion_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_put_Value_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ short in_nIndex,
    /* [in] */ double pVal);


void __RPC_STUB ISIQuaternion_put_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_get_X_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB ISIQuaternion_get_X_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_put_X_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ double pVal);


void __RPC_STUB ISIQuaternion_put_X_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_get_Y_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB ISIQuaternion_get_Y_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_put_Y_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ double pVal);


void __RPC_STUB ISIQuaternion_put_Y_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_get_Z_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB ISIQuaternion_get_Z_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_put_Z_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ double pVal);


void __RPC_STUB ISIQuaternion_put_Z_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_get_W_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB ISIQuaternion_get_W_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_put_W_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ double pVal);


void __RPC_STUB ISIQuaternion_put_W_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_Set_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ double in_dW,
    /* [in] */ double in_dX,
    /* [in] */ double in_dY,
    /* [in] */ double in_dZ);


void __RPC_STUB ISIQuaternion_Set_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_Get_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [out] */ VARIANT __RPC_FAR *out_pvarW,
    /* [out] */ VARIANT __RPC_FAR *out_pvarX,
    /* [out] */ VARIANT __RPC_FAR *out_pvarY,
    /* [out] */ VARIANT __RPC_FAR *out_pvarZ);


void __RPC_STUB ISIQuaternion_Get_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_Copy_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ ISIQuaternion __RPC_FAR *in_pSrcQuat);


void __RPC_STUB ISIQuaternion_Copy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_SetIdentity_Proxy( 
    ISIQuaternion __RPC_FAR * This);


void __RPC_STUB ISIQuaternion_SetIdentity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_Invert_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat);


void __RPC_STUB ISIQuaternion_Invert_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_InvertInPlace_Proxy( 
    ISIQuaternion __RPC_FAR * This);


void __RPC_STUB ISIQuaternion_InvertInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_Conjugate_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat);


void __RPC_STUB ISIQuaternion_Conjugate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_ConjugateInPlace_Proxy( 
    ISIQuaternion __RPC_FAR * This);


void __RPC_STUB ISIQuaternion_ConjugateInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_Add_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat1,
    /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat2);


void __RPC_STUB ISIQuaternion_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_AddInPlace_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat);


void __RPC_STUB ISIQuaternion_AddInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_Length_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *out_pdLength);


void __RPC_STUB ISIQuaternion_Length_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_LengthSquared_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *out_pdLength);


void __RPC_STUB ISIQuaternion_LengthSquared_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_Negate_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat);


void __RPC_STUB ISIQuaternion_Negate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_NegateInPlace_Proxy( 
    ISIQuaternion __RPC_FAR * This);


void __RPC_STUB ISIQuaternion_NegateInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_Sub_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat1,
    /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat2);


void __RPC_STUB ISIQuaternion_Sub_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_SubInPlace_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat);


void __RPC_STUB ISIQuaternion_SubInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_Mul_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat1,
    /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat2);


void __RPC_STUB ISIQuaternion_Mul_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_MulInPlace_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat);


void __RPC_STUB ISIQuaternion_MulInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_SLerp_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat1,
    /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat2,
    /* [in] */ double in_dU);


void __RPC_STUB ISIQuaternion_SLerp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_Equals_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat,
    /* [retval][out] */ long __RPC_FAR *out_pbEquality);


void __RPC_STUB ISIQuaternion_Equals_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_EpsilonEquals_Proxy( 
    ISIQuaternion __RPC_FAR * This,
    /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat,
    /* [in] */ double in_dEpsilon,
    /* [retval][out] */ long __RPC_FAR *out_pbEquality);


void __RPC_STUB ISIQuaternion_EpsilonEquals_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISIQuaternion_Normalize_Proxy( 
    ISIQuaternion __RPC_FAR * This);


void __RPC_STUB ISIQuaternion_Normalize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISIQuaternion_INTERFACE_DEFINED__ */


#ifndef __ISITransformation_INTERFACE_DEFINED__
#define __ISITransformation_INTERFACE_DEFINED__

/* interface ISITransformation */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_ISITransformation;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BFB3B91B-24CC-11D3-BC3A-00A0C9612B97")
    ISITransformation : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetScaling( 
            /* [in] */ ISIVector3 __RPC_FAR *io_pScale) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetScaling( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pScale) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRotation( 
            /* [in] */ ISIRotation __RPC_FAR *io_pRotation) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRotationMatrix3( 
            /* [in] */ ISIMatrix3 __RPC_FAR *io_pRotMat) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRotationXYZAngles( 
            /* [in] */ ISIVector3 __RPC_FAR *io_pXYZAngles) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRotationAxisAngle( 
            /* [in] */ ISIVector3 __RPC_FAR *io_pAxis,
            /* [out] */ VARIANT __RPC_FAR *io_pvarAngle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRotationQuaternion( 
            /* [in] */ ISIQuaternion __RPC_FAR *io_pQuat) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetRotation( 
            /* [in] */ ISIRotation __RPC_FAR *in_pRotation) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetRotationFromXYZAngles( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pXYZAngles) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetRotationFromAxisAngle( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pAxis,
            /* [in] */ double in_dAngle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetRotationFromQuaternion( 
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTranslation( 
            /* [in] */ ISIVector3 __RPC_FAR *io_pTransVect) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTranslation( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pTransVect) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetIdentity( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Copy( 
            /* [in] */ ISITransformation __RPC_FAR *in_pSrcTransfo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetScalingValues( 
            /* [out] */ VARIANT __RPC_FAR *out_pvarX,
            /* [out] */ VARIANT __RPC_FAR *out_pvarY,
            /* [out] */ VARIANT __RPC_FAR *out_pvarZ) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetScalingFromValues( 
            /* [in] */ double in_dX,
            /* [in] */ double in_dY,
            /* [in] */ double in_dZ) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTranslationValues( 
            /* [out] */ VARIANT __RPC_FAR *out_pvarX,
            /* [out] */ VARIANT __RPC_FAR *out_pvarY,
            /* [out] */ VARIANT __RPC_FAR *out_pvarZ) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTranslationFromValues( 
            /* [in] */ double in_dX,
            /* [in] */ double in_dY,
            /* [in] */ double in_dZ) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetRotationFromXYZAxes( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pXAxis,
            /* [in] */ ISIVector3 __RPC_FAR *in_pYAxis,
            /* [in] */ ISIVector3 __RPC_FAR *in_pZAxis) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddLocalScaling( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pScaling) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddParentScaling( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pScaling) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddLocalRotation( 
            /* [in] */ ISIRotation __RPC_FAR *in_pRotation,
            /* [defaultvalue][optional][in] */ long in_bIncrementEulerAngles = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddParentRotation( 
            /* [in] */ ISIRotation __RPC_FAR *in_pRotation,
            /* [defaultvalue][optional][in] */ long in_bIncrementEulerAngles = 0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddLocalTranslation( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pTranslation) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddParentTranslation( 
            /* [in] */ ISIVector3 __RPC_FAR *in_pTranslation) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Mul( 
            /* [in] */ ISITransformation __RPC_FAR *in_pTransfo1,
            /* [in] */ ISITransformation __RPC_FAR *in_pTransfo2) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MulInPlace( 
            /* [in] */ ISITransformation __RPC_FAR *in_pTransfo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRotationXYZAnglesValues( 
            /* [out] */ VARIANT __RPC_FAR *out_pvarX,
            /* [out] */ VARIANT __RPC_FAR *out_pvarY,
            /* [out] */ VARIANT __RPC_FAR *out_pvarZ) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetRotationFromXYZAnglesValues( 
            /* [in] */ double in_dX,
            /* [in] */ double in_dY,
            /* [in] */ double in_dZ) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMatrix4( 
            /* [in] */ ISIMatrix4 __RPC_FAR *io_pMatrix) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetMatrix4( 
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetScalingOrientationXYZAngles( 
            /* [out] */ VARIANT __RPC_FAR *out_pvarX,
            /* [out] */ VARIANT __RPC_FAR *out_pvarY,
            /* [out] */ VARIANT __RPC_FAR *out_pvarZ) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE HasScalingOrientation( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *out_pbSclOri) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetScalingOrientationFromXYZAngles( 
            /* [in] */ double in_dX,
            /* [in] */ double in_dY,
            /* [in] */ double in_dZ) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISITransformationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISITransformation __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISITransformation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISITransformation __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetScaling )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *io_pScale);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetScaling )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pScale);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRotation )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIRotation __RPC_FAR *io_pRotation);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRotationMatrix3 )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIMatrix3 __RPC_FAR *io_pRotMat);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRotationXYZAngles )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *io_pXYZAngles);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRotationAxisAngle )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *io_pAxis,
            /* [out] */ VARIANT __RPC_FAR *io_pvarAngle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRotationQuaternion )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIQuaternion __RPC_FAR *io_pQuat);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetRotation )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIRotation __RPC_FAR *in_pRotation);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetRotationFromXYZAngles )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pXYZAngles);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetRotationFromAxisAngle )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pAxis,
            /* [in] */ double in_dAngle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetRotationFromQuaternion )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTranslation )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *io_pTransVect);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetTranslation )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pTransVect);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetIdentity )( 
            ISITransformation __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Copy )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISITransformation __RPC_FAR *in_pSrcTransfo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetScalingValues )( 
            ISITransformation __RPC_FAR * This,
            /* [out] */ VARIANT __RPC_FAR *out_pvarX,
            /* [out] */ VARIANT __RPC_FAR *out_pvarY,
            /* [out] */ VARIANT __RPC_FAR *out_pvarZ);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetScalingFromValues )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ double in_dX,
            /* [in] */ double in_dY,
            /* [in] */ double in_dZ);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTranslationValues )( 
            ISITransformation __RPC_FAR * This,
            /* [out] */ VARIANT __RPC_FAR *out_pvarX,
            /* [out] */ VARIANT __RPC_FAR *out_pvarY,
            /* [out] */ VARIANT __RPC_FAR *out_pvarZ);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetTranslationFromValues )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ double in_dX,
            /* [in] */ double in_dY,
            /* [in] */ double in_dZ);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetRotationFromXYZAxes )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pXAxis,
            /* [in] */ ISIVector3 __RPC_FAR *in_pYAxis,
            /* [in] */ ISIVector3 __RPC_FAR *in_pZAxis);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddLocalScaling )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pScaling);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddParentScaling )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pScaling);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddLocalRotation )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIRotation __RPC_FAR *in_pRotation,
            /* [defaultvalue][optional][in] */ long in_bIncrementEulerAngles);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddParentRotation )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIRotation __RPC_FAR *in_pRotation,
            /* [defaultvalue][optional][in] */ long in_bIncrementEulerAngles);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddLocalTranslation )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pTranslation);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddParentTranslation )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIVector3 __RPC_FAR *in_pTranslation);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Mul )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISITransformation __RPC_FAR *in_pTransfo1,
            /* [in] */ ISITransformation __RPC_FAR *in_pTransfo2);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MulInPlace )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISITransformation __RPC_FAR *in_pTransfo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRotationXYZAnglesValues )( 
            ISITransformation __RPC_FAR * This,
            /* [out] */ VARIANT __RPC_FAR *out_pvarX,
            /* [out] */ VARIANT __RPC_FAR *out_pvarY,
            /* [out] */ VARIANT __RPC_FAR *out_pvarZ);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetRotationFromXYZAnglesValues )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ double in_dX,
            /* [in] */ double in_dY,
            /* [in] */ double in_dZ);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMatrix4 )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIMatrix4 __RPC_FAR *io_pMatrix);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetMatrix4 )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetScalingOrientationXYZAngles )( 
            ISITransformation __RPC_FAR * This,
            /* [out] */ VARIANT __RPC_FAR *out_pvarX,
            /* [out] */ VARIANT __RPC_FAR *out_pvarY,
            /* [out] */ VARIANT __RPC_FAR *out_pvarZ);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HasScalingOrientation )( 
            ISITransformation __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *out_pbSclOri);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetScalingOrientationFromXYZAngles )( 
            ISITransformation __RPC_FAR * This,
            /* [in] */ double in_dX,
            /* [in] */ double in_dY,
            /* [in] */ double in_dZ);
        
        END_INTERFACE
    } ISITransformationVtbl;

    interface ISITransformation
    {
        CONST_VTBL struct ISITransformationVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISITransformation_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISITransformation_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISITransformation_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISITransformation_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISITransformation_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISITransformation_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISITransformation_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISITransformation_GetScaling(This,io_pScale)	\
    (This)->lpVtbl -> GetScaling(This,io_pScale)

#define ISITransformation_SetScaling(This,in_pScale)	\
    (This)->lpVtbl -> SetScaling(This,in_pScale)

#define ISITransformation_GetRotation(This,io_pRotation)	\
    (This)->lpVtbl -> GetRotation(This,io_pRotation)

#define ISITransformation_GetRotationMatrix3(This,io_pRotMat)	\
    (This)->lpVtbl -> GetRotationMatrix3(This,io_pRotMat)

#define ISITransformation_GetRotationXYZAngles(This,io_pXYZAngles)	\
    (This)->lpVtbl -> GetRotationXYZAngles(This,io_pXYZAngles)

#define ISITransformation_GetRotationAxisAngle(This,io_pAxis,io_pvarAngle)	\
    (This)->lpVtbl -> GetRotationAxisAngle(This,io_pAxis,io_pvarAngle)

#define ISITransformation_GetRotationQuaternion(This,io_pQuat)	\
    (This)->lpVtbl -> GetRotationQuaternion(This,io_pQuat)

#define ISITransformation_SetRotation(This,in_pRotation)	\
    (This)->lpVtbl -> SetRotation(This,in_pRotation)

#define ISITransformation_SetRotationFromXYZAngles(This,in_pXYZAngles)	\
    (This)->lpVtbl -> SetRotationFromXYZAngles(This,in_pXYZAngles)

#define ISITransformation_SetRotationFromAxisAngle(This,in_pAxis,in_dAngle)	\
    (This)->lpVtbl -> SetRotationFromAxisAngle(This,in_pAxis,in_dAngle)

#define ISITransformation_SetRotationFromQuaternion(This,in_pQuat)	\
    (This)->lpVtbl -> SetRotationFromQuaternion(This,in_pQuat)

#define ISITransformation_GetTranslation(This,io_pTransVect)	\
    (This)->lpVtbl -> GetTranslation(This,io_pTransVect)

#define ISITransformation_SetTranslation(This,in_pTransVect)	\
    (This)->lpVtbl -> SetTranslation(This,in_pTransVect)

#define ISITransformation_SetIdentity(This)	\
    (This)->lpVtbl -> SetIdentity(This)

#define ISITransformation_Copy(This,in_pSrcTransfo)	\
    (This)->lpVtbl -> Copy(This,in_pSrcTransfo)

#define ISITransformation_GetScalingValues(This,out_pvarX,out_pvarY,out_pvarZ)	\
    (This)->lpVtbl -> GetScalingValues(This,out_pvarX,out_pvarY,out_pvarZ)

#define ISITransformation_SetScalingFromValues(This,in_dX,in_dY,in_dZ)	\
    (This)->lpVtbl -> SetScalingFromValues(This,in_dX,in_dY,in_dZ)

#define ISITransformation_GetTranslationValues(This,out_pvarX,out_pvarY,out_pvarZ)	\
    (This)->lpVtbl -> GetTranslationValues(This,out_pvarX,out_pvarY,out_pvarZ)

#define ISITransformation_SetTranslationFromValues(This,in_dX,in_dY,in_dZ)	\
    (This)->lpVtbl -> SetTranslationFromValues(This,in_dX,in_dY,in_dZ)

#define ISITransformation_SetRotationFromXYZAxes(This,in_pXAxis,in_pYAxis,in_pZAxis)	\
    (This)->lpVtbl -> SetRotationFromXYZAxes(This,in_pXAxis,in_pYAxis,in_pZAxis)

#define ISITransformation_AddLocalScaling(This,in_pScaling)	\
    (This)->lpVtbl -> AddLocalScaling(This,in_pScaling)

#define ISITransformation_AddParentScaling(This,in_pScaling)	\
    (This)->lpVtbl -> AddParentScaling(This,in_pScaling)

#define ISITransformation_AddLocalRotation(This,in_pRotation,in_bIncrementEulerAngles)	\
    (This)->lpVtbl -> AddLocalRotation(This,in_pRotation,in_bIncrementEulerAngles)

#define ISITransformation_AddParentRotation(This,in_pRotation,in_bIncrementEulerAngles)	\
    (This)->lpVtbl -> AddParentRotation(This,in_pRotation,in_bIncrementEulerAngles)

#define ISITransformation_AddLocalTranslation(This,in_pTranslation)	\
    (This)->lpVtbl -> AddLocalTranslation(This,in_pTranslation)

#define ISITransformation_AddParentTranslation(This,in_pTranslation)	\
    (This)->lpVtbl -> AddParentTranslation(This,in_pTranslation)

#define ISITransformation_Mul(This,in_pTransfo1,in_pTransfo2)	\
    (This)->lpVtbl -> Mul(This,in_pTransfo1,in_pTransfo2)

#define ISITransformation_MulInPlace(This,in_pTransfo)	\
    (This)->lpVtbl -> MulInPlace(This,in_pTransfo)

#define ISITransformation_GetRotationXYZAnglesValues(This,out_pvarX,out_pvarY,out_pvarZ)	\
    (This)->lpVtbl -> GetRotationXYZAnglesValues(This,out_pvarX,out_pvarY,out_pvarZ)

#define ISITransformation_SetRotationFromXYZAnglesValues(This,in_dX,in_dY,in_dZ)	\
    (This)->lpVtbl -> SetRotationFromXYZAnglesValues(This,in_dX,in_dY,in_dZ)

#define ISITransformation_GetMatrix4(This,io_pMatrix)	\
    (This)->lpVtbl -> GetMatrix4(This,io_pMatrix)

#define ISITransformation_SetMatrix4(This,in_pMatrix)	\
    (This)->lpVtbl -> SetMatrix4(This,in_pMatrix)

#define ISITransformation_GetScalingOrientationXYZAngles(This,out_pvarX,out_pvarY,out_pvarZ)	\
    (This)->lpVtbl -> GetScalingOrientationXYZAngles(This,out_pvarX,out_pvarY,out_pvarZ)

#define ISITransformation_HasScalingOrientation(This,out_pbSclOri)	\
    (This)->lpVtbl -> HasScalingOrientation(This,out_pbSclOri)

#define ISITransformation_SetScalingOrientationFromXYZAngles(This,in_dX,in_dY,in_dZ)	\
    (This)->lpVtbl -> SetScalingOrientationFromXYZAngles(This,in_dX,in_dY,in_dZ)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_GetScaling_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *io_pScale);


void __RPC_STUB ISITransformation_GetScaling_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_SetScaling_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pScale);


void __RPC_STUB ISITransformation_SetScaling_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_GetRotation_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIRotation __RPC_FAR *io_pRotation);


void __RPC_STUB ISITransformation_GetRotation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_GetRotationMatrix3_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIMatrix3 __RPC_FAR *io_pRotMat);


void __RPC_STUB ISITransformation_GetRotationMatrix3_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_GetRotationXYZAngles_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *io_pXYZAngles);


void __RPC_STUB ISITransformation_GetRotationXYZAngles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_GetRotationAxisAngle_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *io_pAxis,
    /* [out] */ VARIANT __RPC_FAR *io_pvarAngle);


void __RPC_STUB ISITransformation_GetRotationAxisAngle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_GetRotationQuaternion_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIQuaternion __RPC_FAR *io_pQuat);


void __RPC_STUB ISITransformation_GetRotationQuaternion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_SetRotation_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIRotation __RPC_FAR *in_pRotation);


void __RPC_STUB ISITransformation_SetRotation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_SetRotationFromXYZAngles_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pXYZAngles);


void __RPC_STUB ISITransformation_SetRotationFromXYZAngles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_SetRotationFromAxisAngle_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pAxis,
    /* [in] */ double in_dAngle);


void __RPC_STUB ISITransformation_SetRotationFromAxisAngle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_SetRotationFromQuaternion_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIQuaternion __RPC_FAR *in_pQuat);


void __RPC_STUB ISITransformation_SetRotationFromQuaternion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_GetTranslation_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *io_pTransVect);


void __RPC_STUB ISITransformation_GetTranslation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_SetTranslation_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pTransVect);


void __RPC_STUB ISITransformation_SetTranslation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_SetIdentity_Proxy( 
    ISITransformation __RPC_FAR * This);


void __RPC_STUB ISITransformation_SetIdentity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_Copy_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISITransformation __RPC_FAR *in_pSrcTransfo);


void __RPC_STUB ISITransformation_Copy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_GetScalingValues_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [out] */ VARIANT __RPC_FAR *out_pvarX,
    /* [out] */ VARIANT __RPC_FAR *out_pvarY,
    /* [out] */ VARIANT __RPC_FAR *out_pvarZ);


void __RPC_STUB ISITransformation_GetScalingValues_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_SetScalingFromValues_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ double in_dX,
    /* [in] */ double in_dY,
    /* [in] */ double in_dZ);


void __RPC_STUB ISITransformation_SetScalingFromValues_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_GetTranslationValues_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [out] */ VARIANT __RPC_FAR *out_pvarX,
    /* [out] */ VARIANT __RPC_FAR *out_pvarY,
    /* [out] */ VARIANT __RPC_FAR *out_pvarZ);


void __RPC_STUB ISITransformation_GetTranslationValues_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_SetTranslationFromValues_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ double in_dX,
    /* [in] */ double in_dY,
    /* [in] */ double in_dZ);


void __RPC_STUB ISITransformation_SetTranslationFromValues_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_SetRotationFromXYZAxes_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pXAxis,
    /* [in] */ ISIVector3 __RPC_FAR *in_pYAxis,
    /* [in] */ ISIVector3 __RPC_FAR *in_pZAxis);


void __RPC_STUB ISITransformation_SetRotationFromXYZAxes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_AddLocalScaling_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pScaling);


void __RPC_STUB ISITransformation_AddLocalScaling_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_AddParentScaling_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pScaling);


void __RPC_STUB ISITransformation_AddParentScaling_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_AddLocalRotation_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIRotation __RPC_FAR *in_pRotation,
    /* [defaultvalue][optional][in] */ long in_bIncrementEulerAngles);


void __RPC_STUB ISITransformation_AddLocalRotation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_AddParentRotation_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIRotation __RPC_FAR *in_pRotation,
    /* [defaultvalue][optional][in] */ long in_bIncrementEulerAngles);


void __RPC_STUB ISITransformation_AddParentRotation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_AddLocalTranslation_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pTranslation);


void __RPC_STUB ISITransformation_AddLocalTranslation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_AddParentTranslation_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIVector3 __RPC_FAR *in_pTranslation);


void __RPC_STUB ISITransformation_AddParentTranslation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_Mul_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISITransformation __RPC_FAR *in_pTransfo1,
    /* [in] */ ISITransformation __RPC_FAR *in_pTransfo2);


void __RPC_STUB ISITransformation_Mul_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_MulInPlace_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISITransformation __RPC_FAR *in_pTransfo);


void __RPC_STUB ISITransformation_MulInPlace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_GetRotationXYZAnglesValues_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [out] */ VARIANT __RPC_FAR *out_pvarX,
    /* [out] */ VARIANT __RPC_FAR *out_pvarY,
    /* [out] */ VARIANT __RPC_FAR *out_pvarZ);


void __RPC_STUB ISITransformation_GetRotationXYZAnglesValues_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_SetRotationFromXYZAnglesValues_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ double in_dX,
    /* [in] */ double in_dY,
    /* [in] */ double in_dZ);


void __RPC_STUB ISITransformation_SetRotationFromXYZAnglesValues_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_GetMatrix4_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIMatrix4 __RPC_FAR *io_pMatrix);


void __RPC_STUB ISITransformation_GetMatrix4_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_SetMatrix4_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ ISIMatrix4 __RPC_FAR *in_pMatrix);


void __RPC_STUB ISITransformation_SetMatrix4_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_GetScalingOrientationXYZAngles_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [out] */ VARIANT __RPC_FAR *out_pvarX,
    /* [out] */ VARIANT __RPC_FAR *out_pvarY,
    /* [out] */ VARIANT __RPC_FAR *out_pvarZ);


void __RPC_STUB ISITransformation_GetScalingOrientationXYZAngles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_HasScalingOrientation_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *out_pbSclOri);


void __RPC_STUB ISITransformation_HasScalingOrientation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISITransformation_SetScalingOrientationFromXYZAngles_Proxy( 
    ISITransformation __RPC_FAR * This,
    /* [in] */ double in_dX,
    /* [in] */ double in_dY,
    /* [in] */ double in_dZ);


void __RPC_STUB ISITransformation_SetScalingOrientationFromXYZAngles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISITransformation_INTERFACE_DEFINED__ */


#ifndef __XSIMath_INTERFACE_DEFINED__
#define __XSIMath_INTERFACE_DEFINED__

/* interface XSIMath */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_XSIMath;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("927A181C-A441-419E-9A9A-86FD93DF288B")
    XSIMath : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateQuaternion( 
            /* [retval][out] */ ISIQuaternion __RPC_FAR *__RPC_FAR *out_ppQuaternion) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateMatrix4( 
            /* [retval][out] */ ISIMatrix4 __RPC_FAR *__RPC_FAR *out_ppMatrix4) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateMatrix3( 
            /* [retval][out] */ ISIMatrix3 __RPC_FAR *__RPC_FAR *out_ppMatrix3) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateVector3( 
            /* [retval][out] */ ISIVector3 __RPC_FAR *__RPC_FAR *out_ppVector3) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateTransform( 
            /* [retval][out] */ ISITransformation __RPC_FAR *__RPC_FAR *out_ppTransformation) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateBoundingVolume( 
            /* [retval][out] */ ISIBoundingVolume __RPC_FAR *__RPC_FAR *out_ppBoundingVolume) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateRotation( 
            /* [retval][out] */ ISIRotation __RPC_FAR *__RPC_FAR *out_ppRotation) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct XSIMathVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            XSIMath __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            XSIMath __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            XSIMath __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            XSIMath __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            XSIMath __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            XSIMath __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            XSIMath __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateQuaternion )( 
            XSIMath __RPC_FAR * This,
            /* [retval][out] */ ISIQuaternion __RPC_FAR *__RPC_FAR *out_ppQuaternion);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateMatrix4 )( 
            XSIMath __RPC_FAR * This,
            /* [retval][out] */ ISIMatrix4 __RPC_FAR *__RPC_FAR *out_ppMatrix4);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateMatrix3 )( 
            XSIMath __RPC_FAR * This,
            /* [retval][out] */ ISIMatrix3 __RPC_FAR *__RPC_FAR *out_ppMatrix3);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateVector3 )( 
            XSIMath __RPC_FAR * This,
            /* [retval][out] */ ISIVector3 __RPC_FAR *__RPC_FAR *out_ppVector3);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateTransform )( 
            XSIMath __RPC_FAR * This,
            /* [retval][out] */ ISITransformation __RPC_FAR *__RPC_FAR *out_ppTransformation);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateBoundingVolume )( 
            XSIMath __RPC_FAR * This,
            /* [retval][out] */ ISIBoundingVolume __RPC_FAR *__RPC_FAR *out_ppBoundingVolume);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateRotation )( 
            XSIMath __RPC_FAR * This,
            /* [retval][out] */ ISIRotation __RPC_FAR *__RPC_FAR *out_ppRotation);
        
        END_INTERFACE
    } XSIMathVtbl;

    interface XSIMath
    {
        CONST_VTBL struct XSIMathVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define XSIMath_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define XSIMath_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define XSIMath_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define XSIMath_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define XSIMath_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define XSIMath_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define XSIMath_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define XSIMath_CreateQuaternion(This,out_ppQuaternion)	\
    (This)->lpVtbl -> CreateQuaternion(This,out_ppQuaternion)

#define XSIMath_CreateMatrix4(This,out_ppMatrix4)	\
    (This)->lpVtbl -> CreateMatrix4(This,out_ppMatrix4)

#define XSIMath_CreateMatrix3(This,out_ppMatrix3)	\
    (This)->lpVtbl -> CreateMatrix3(This,out_ppMatrix3)

#define XSIMath_CreateVector3(This,out_ppVector3)	\
    (This)->lpVtbl -> CreateVector3(This,out_ppVector3)

#define XSIMath_CreateTransform(This,out_ppTransformation)	\
    (This)->lpVtbl -> CreateTransform(This,out_ppTransformation)

#define XSIMath_CreateBoundingVolume(This,out_ppBoundingVolume)	\
    (This)->lpVtbl -> CreateBoundingVolume(This,out_ppBoundingVolume)

#define XSIMath_CreateRotation(This,out_ppRotation)	\
    (This)->lpVtbl -> CreateRotation(This,out_ppRotation)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE XSIMath_CreateQuaternion_Proxy( 
    XSIMath __RPC_FAR * This,
    /* [retval][out] */ ISIQuaternion __RPC_FAR *__RPC_FAR *out_ppQuaternion);


void __RPC_STUB XSIMath_CreateQuaternion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE XSIMath_CreateMatrix4_Proxy( 
    XSIMath __RPC_FAR * This,
    /* [retval][out] */ ISIMatrix4 __RPC_FAR *__RPC_FAR *out_ppMatrix4);


void __RPC_STUB XSIMath_CreateMatrix4_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE XSIMath_CreateMatrix3_Proxy( 
    XSIMath __RPC_FAR * This,
    /* [retval][out] */ ISIMatrix3 __RPC_FAR *__RPC_FAR *out_ppMatrix3);


void __RPC_STUB XSIMath_CreateMatrix3_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE XSIMath_CreateVector3_Proxy( 
    XSIMath __RPC_FAR * This,
    /* [retval][out] */ ISIVector3 __RPC_FAR *__RPC_FAR *out_ppVector3);


void __RPC_STUB XSIMath_CreateVector3_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE XSIMath_CreateTransform_Proxy( 
    XSIMath __RPC_FAR * This,
    /* [retval][out] */ ISITransformation __RPC_FAR *__RPC_FAR *out_ppTransformation);


void __RPC_STUB XSIMath_CreateTransform_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE XSIMath_CreateBoundingVolume_Proxy( 
    XSIMath __RPC_FAR * This,
    /* [retval][out] */ ISIBoundingVolume __RPC_FAR *__RPC_FAR *out_ppBoundingVolume);


void __RPC_STUB XSIMath_CreateBoundingVolume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE XSIMath_CreateRotation_Proxy( 
    XSIMath __RPC_FAR * This,
    /* [retval][out] */ ISIRotation __RPC_FAR *__RPC_FAR *out_ppRotation);


void __RPC_STUB XSIMath_CreateRotation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __XSIMath_INTERFACE_DEFINED__ */


#ifndef __ISIBoundingVolume_INTERFACE_DEFINED__
#define __ISIBoundingVolume_INTERFACE_DEFINED__

/* interface ISIBoundingVolume */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_ISIBoundingVolume;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("21B2A66D-266E-4B43-9919-655E9508D8E6")
    ISIBoundingVolume : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ISIBoundingVolumeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISIBoundingVolume __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISIBoundingVolume __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISIBoundingVolume __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISIBoundingVolume __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISIBoundingVolume __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISIBoundingVolume __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISIBoundingVolume __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } ISIBoundingVolumeVtbl;

    interface ISIBoundingVolume
    {
        CONST_VTBL struct ISIBoundingVolumeVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISIBoundingVolume_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISIBoundingVolume_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISIBoundingVolume_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISIBoundingVolume_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISIBoundingVolume_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISIBoundingVolume_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISIBoundingVolume_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISIBoundingVolume_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_SIVector3;

#ifdef __cplusplus

class DECLSPEC_UUID("0222732F-232E-11D3-BC3A-00A0C9612B97")
SIVector3;
#endif

EXTERN_C const CLSID CLSID_SIMatrix3;

#ifdef __cplusplus

class DECLSPEC_UUID("BFB3B914-24CC-11D3-BC3A-00A0C9612B97")
SIMatrix3;
#endif

EXTERN_C const CLSID CLSID_SIMatrix4;

#ifdef __cplusplus

class DECLSPEC_UUID("BFB3B916-24CC-11D3-BC3A-00A0C9612B97")
SIMatrix4;
#endif

EXTERN_C const CLSID CLSID_SIRotation;

#ifdef __cplusplus

class DECLSPEC_UUID("BFB3B918-24CC-11D3-BC3A-00A0C9612B97")
SIRotation;
#endif

EXTERN_C const CLSID CLSID_SIQuaternion;

#ifdef __cplusplus

class DECLSPEC_UUID("BFB3B91A-24CC-11D3-BC3A-00A0C9612B97")
SIQuaternion;
#endif

EXTERN_C const CLSID CLSID_SITransformation;

#ifdef __cplusplus

class DECLSPEC_UUID("BFB3B91C-24CC-11D3-BC3A-00A0C9612B97")
SITransformation;
#endif

EXTERN_C const CLSID CLSID_CXSIMath;

#ifdef __cplusplus

class DECLSPEC_UUID("8EB8041B-11F5-4F56-9FEB-6461811BA1B3")
CXSIMath;
#endif
#endif /* __SIMATHLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif

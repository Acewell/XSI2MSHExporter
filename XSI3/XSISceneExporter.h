// XSISceneExporter.h : Declaration of the CXSISceneExporter

#ifndef __XSISCENEEXPORTER_H_
#define __XSISCENEEXPORTER_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CXSISceneExporter
class ATL_NO_VTABLE CXSISceneExporter : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CXSISceneExporter, &CLSID_XSISceneExporter3>,
	public IDispatchImpl<IXSISceneExporter3, &IID_IXSISceneExporter3, &LIBID_XSI2MSHEXPORTER3Lib>
{
public:
	CXSISceneExporter()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_XSISCENEEXPORTER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CXSISceneExporter)
	COM_INTERFACE_ENTRY(IXSISceneExporter3)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IXSISceneExporter
public:
	STDMETHOD(ExportScene)(IDispatch *pProject, IDispatch *pScene);
	STDMETHOD(ExportSceneA)(IDispatch *pProject, IDispatch *pScene, BSTR in_bstrDestFilename);
};

#endif //__XSISCENEEXPORTER_H_

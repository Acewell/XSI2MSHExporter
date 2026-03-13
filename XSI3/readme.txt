XSI MSH Exporter notes for XSI v3
=================================

This project/VSS folder contains the code for the XSI2MSHExporter DLL that
is compatible with XSI version 3.0.  Details follow.

XSI version
-----------
This exported has been tested with XSI version 3.0, BETA, RC5.  The build
version that shows in the XSI About box for this version should be
"3.0.2002.1025".

Previous betas (well, at least RC4) had a bug with retrieving UV coordinates
in some scenarios that will get very bad (uninitialized) UVs.  RC4 has
build version "3.0.2002.1009".  Do _not_ use this, or earlier versions.

When you install XSI 3.0, make sure to install the SDK too if you want to
build this exporter.  Note that XSI 3.0 SDK installs the (needed) extra 
simath.h and simath_id.c files that were missing in previous versions of
the XSI SDK.

Building notes
--------------
As an addendum to the usual process noted in the parent readme, you should
make sure your project include path points to the XSI 3 SDK, not the 1.5/2.0
version.  You may also need to copy all the TLB files from your XSI 3.0
executable directory into your XSI2MSHExporter source directory.


Zero/MSHParser
--------------
These are zipped, grabbed from the CloneWars VSS project on 10/28/02.

Note that this grab includes a change made by Ken Miller to ZeroPolyhedra.cpp
that fixed a triangulation bug, but may have adverse affects on meshes when
they are "flattened", according to Chris Fandrich.

version 1.0.0.2 UPDATE:
-- [MSHParser] In ZeroImportExportNode.cpp, ZeroImportExportNode::OnExportFile, 
removed call to ExtractNonSkinPolygonsFromSkin().
-- In MSHParser->MSHBlockModel.cpp->CMSHBlockModelSegment::Write,
modified code to _always_ write out all three 'index' blocks for
a segment -- specifically it'll write out: 'NDXL' block for the
original XSI polygons, 'NDXT' block for triangle list, and
'STRP' block for triangle strip.  All blocks reference indices
into the same vertex table.
-- Added the following code to MSHBlockModel.h for supporting
the new idea of a model being also a procedural collision proxy:

	  enum SWCI_CollisionType
	  {
		PST_SPHERE,
		PST_ELLIPSOID,
		PST_CYLINDER,
		PST_MESH,
	  };

  protected:

	  bool m_bIsSWCICollisionObject;
	  SWCI_CollisionType m_SWCICollisionType;
	  float m_fSWCI_Radius;
	  float m_fSWCI_Height;

  public:

	  void SetSWCICollisionObject(bool bCollision)
	  {
	      m_bIsSWCICollisionObject = bCollision;
	  }
	  bool GetSWCICollisionObject(void) const
	  {
		  return m_bIsSWCICollisionObject;
	  }
	  void SetSWCICollisionType(SWCI_CollisionType coltype)
	  {
		  m_SWCICollisionType = coltype;
	  }
	  SWCI_CollisionType GetSWCICollisionType(void) const
	  {
		  return m_SWCICollisionType;
	  }
	  void SetSWCIRadius(float fRadius)
	  {
		  m_fSWCI_Radius = fRadius;
	  }
	  float GetSWCIRadius(void) const
	  {
		  return m_fSWCI_Radius;
	  }
	  void SetSWCIHeight(float fHeight)
	  {
		  m_fSWCI_Height = fHeight;
	  }
	  float GetSWCIHeight(void) const
	  {
		  return m_fSWCI_Height;
	  }

Note that currently, XSI only really provides Sphere or Cylinder
primitives (not ellipsoid or even box).  Ellipsoid primitives for
now will have to be calculated from polygonal mesh size.
Also, updated CMSHBlockModelSegment::Write and 
CMSHBlockModelSegment::Read to write/read the additional SWCI info
to the 'SWCI' block off MODL.


Exporter code
-------------
Changes:

-- Changed various files to use a new GUID for the COM interfaces.  See the
IDL and RGS files for actual new GUIDs if, for some reason, you need this.

-- Tweaked the dialog box to show a "(v3)" in the title bar.  Added comment
in version resource to identify DLL as the XSI v3 exporter DLL.

-- In CScene2MSH::AddMaterialsFromObject(), when getting "get cluster materials",
added check for null Material IDispatch ptr, changing:

	if(SUCCEEDED(pSceneItem->get_Material(&pMat)))

to:

	if(SUCCEEDED(pSceneItem->get_Material(&pMat)) && pMat != NULL)

Yep, apparently the call can now return a good HRESULT, but set the Material
ptr to NULL.  This may be correct (from a COM-sense), but differs apparently
from XSI 2.0 days.

-- Added a bunch of code in CScene2MSH::GetPolygonData to grab the UVArray
from a given Triangle interface instead of getting the Triangle->TrianglePointsCollection->
TrianglePoint->UV approach, as the latter was broken in XSI v3 RC4.  However,
didn't end up using this (see ORIG_WAY_BEFORE_XSI3BUG, which should be on),
thanks to fix in RC5.  Will probably remove this code at a future point.

version 1.0.0.2 UPDATE:

-- Added new "Export FK Animations" checkbox (subcheck under Export Animations),
and updated XSI2MSHDialog.h/cpp accordingly, with some help from MS ClassWizard
-- Added new "filter" to CScene2MSH (mExportFKAnimations)
*** [same changes from SpecWarrior Anim exporter, but keyed off new toggle flag -- specifically:)
-- In CScene2MSH::CreateIKChain(), at the end of
the method, we create a series of 0-bone effectors, instead
of 1 or 2 bone effectors, to effectively give us single node
animation data instead of IK data.  
-- Also in CScene2MSH::CreateIKChain(), we comment out the
warning about animated skeleton roots, we allow this.
-- In CScene2MSH::CreateZeroStructure(), we comment out
the call to AlignBonesChainRoot -- this routine aligns IK chain
roots (which we drop from our hierarchy) along the bones.
-- In CScene2MSH::CorrectGlobalRotationForSkeleton, we remove
the code that does rotation correction (#ifdef NO_IK_CORRECTION).
*** [same changes from SpecWarrior Anim exporter to not convert handedness:]
-- In CScene2MSH::CreateZeroStructure, we need to _remove_ any
conversion from right-handed coordinates to left-handed coordinates.
For reference: XSI is right-handed.  Zero is left-handed.  Zen is
right-handed.  This entails: Removing the calls to ApplyGlobalRotation
and CorrectGlobalRotationForSkeleton & changing the call to OnExportFile
to take a third param of "false" instead of "true" (bApplyLeftToRight in
ZeroImportExportNode::OnExportFile in MSHParser library).
-- In CScene2MSH::CreateFreeJointAnimation, removed the "hardpoint"
correction code.  [Not sure about "p=correctionMatrix.RotateVector(p);",
left in for now]
*** [same fix for animation problem:]
-- Changed the initial test in CScene2MSH::CreateSkeletonAndAnimations(Scene *pScene).
For some reason, in some cases, scenes can fail to return a AnimatedParameters interface, 
but still be "animated" via IsAnimated.  
*** [same changes from SpecWarrior Anim exporter to support collision proxies:]
-- In CScene2MSH::CreateModel, added code at the end to set the
CMSHBlockModel SWCI info (on, type, procedural radius/height if avail)
if the model name matches the given SWCI prepends (p_*...etc)
Uses ActiveX "IDispatch" interface and "GetValue" XSI 'api' in the
usual really ugly COM/C++ way:

	varResult.vt = VT_R8; // 64-bit float (verified in testing that this is what XSI returns)
	dispparams.rgvarg = new VARIANT[1];
	OLECHAR FAR* szGetValue = OLESTR("GetValue");
	dispparams.rgvarg[0].vt = VT_BSTR;
	dispparams.rgvarg[0].bstrVal = _bstr_t(pMSHModel->GetName()) + _bstr_t(".primitive.sphere.radius");
	pApp->GetIDsOfNames(IID_NULL,&szGetValue,1,LOCALE_SYSTEM_DEFAULT,&dispid);
	dispparams.cArgs = 1;
	dispparams.cNamedArgs = 0;
	hr = pApp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,DISPATCH_METHOD,&dispparams,&varResult,&ExcepInfo,&argErr);

We test for each type of primitive by basically trying to call GetValue("<modelname>.primitive.sphere.radius"),
followed by GetValue("<modelname>.primitive.cylinder.radius") if that fails, and so on.  Note that if the
artist freezes the model, even if it was originally a sphere, it won't support .primitive properties.
*** Some fixes for XSI 3.0 updated includes:
-- Kinematics->get_Global now returns KinematicState interface and not an IDispatch
-- Ditto for get_Local
-- two 'tag' enums in XSI headers changed to be named without 'tag' (actual enums are basically the same)
*** Fixed bug in Browse CFileDialog (for MSH filename) that wasn't checking for read-only files
[NOTE: This is only in the Browse file dialog.  If you just hit OK from the main dialog with a file that's
read-only, it still goes through.  Someday, there should be a file verify check in the dialog OnOK().
*** Added m_SkipBasepose checkbox and corresponding DDX member vars and registry saving in XSI2MSHDialog.cpp
-- Added new "filter" to CScene2MSH (mExportFKAnimations)
*** 12/13/03 -- Added new checkbox for exporting left-handed data, if needed (the immediate problem
is that the Level Editor is still Left-handed, being Zero-based).  Changes to:
	XSI2MSHEXporter.rc -- added checkbox to dialog
	resource.h -- added checkbox to dialog
	XSI2MSHDialog.h/cpp -- added member vars for checkbox and registry saving/loading of setting
	XSISceneExporter.cpp -- CScene2MSH.SetFilters call updated to pass in lefthanded checkbox setting
	Scene2MSH.h/cpp -- Added member var and handling (basically if () wrapping the calls mentioned
above in "[same changes from SpecWarrior Anim exporter to not convert handedness:]") for ExportLeftHanded.
*** 1/?/03 -- Fixed weird problem in CScene2MSH::CreateHierarchy when getting Camera info -- apparently
in XSI 3.0 you can get a camera rig without a camera:
		if(SUCCEEDED(pCameraRig1->get_Camera(&pCamera)) && pCamera != NULL)
*** 2/14/03 -- Happy Valentine's Day.  Fixed problem with SpecWar collision proxies.  For some bizarre
reason, while I made the changes to save collision proxy data in MSHParser, I never made the corresponding
change to Scene2MSH.cpp::CreateModel to actually get the proxies from XSI.  Weird...maybe VSS stomped
this change or something.
*** 2/26/03 -- Made change to support quasi-batch mode version -- added new Automation API "ExportSceneA"
to IDL, and XSISceneExporter.h/cpp that takes an additional BSTR arg which is the destination filename.
It then automatically exports, using the last known dialog settings (without actually doing a DoModal
dialog).  This was requested to support a batch-mode-export script for Juan for SpecWarrior.
*** 3/31/03 -- Salah found a problem with ZeroPolyhedra::ConvexPartition() -- it was welding together
verts that were less than 20cm apart.  This caused a problem for a SW FrontLine model that had a small
shadow volume mesh that was getting trashed.  Updated Zero.zip with the new ZeroPolyhedra.cpp 
(Salah commented out one call to Optimize) and checked in new binary DLL made on Salah's system.
*** 4/30/03 -- Changes made to Scene2Msh.cpp by Forrester that added a series of COM Interface Release
calls to some XSI COM Interfaces.  This substantially reduces the memory leaks that the exporter was
having.



# Microsoft Developer Studio Project File - Name="XSI2MSHExporter" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=XSI2MSHExporter - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XSI2MSHExporter.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XSI2MSHExporter.mak" CFG="XSI2MSHExporter - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XSI2MSHExporter - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "XSI2MSHExporter - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Tools/XSI_Plugins/XSI2MSHExporter", EFYAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XSI2MSHExporter - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "../../../Libraries/Zero" /I "../../../Libraries/MSHParser" /I "../../Softimage_Plugins/Common2" /I "..\..\..\application\logic" /I "../../../Libraries/zlib" /I "C:\Softimage3Beta\XSI_3.0\XSISDK\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "USE_GROUND_ZERO_STATIC_LIBRAY" /D "_DEBUG_LOG" /D "XSI_TOOLS" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - Performing registration
OutDir=.\Debug
TargetPath=.\Debug\XSI2MSHExporter.dll
InputPath=.\Debug\XSI2MSHExporter.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "XSI2MSHExporter - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "XSI2MSHExporter___Win32_Release"
# PROP BASE Intermediate_Dir "XSI2MSHExporter___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "XSI2MSHExporter___Win32_Release"
# PROP Intermediate_Dir "XSI2MSHExporter___Win32_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../Libraries/ZeroDLL/inc" /I "../../../Libraries/MSHParser" /I "../../Softimage_Plugins/Common2" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_DEBUG_LOG" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G6 /MD /W4 /GX /Zi /Od /I "../../../Libraries/Zero" /I "../../../Libraries/MSHParser" /I "../../Softimage_Plugins/Common2" /I "..\..\..\application\logic" /I "C:\Softimage3Beta\XSI_3.0\XSISDK\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "USE_GROUND_ZERO_STATIC_LIBRAY" /D "XSI_TOOLS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"LIBCD.lib" /pdbtype:sept
# ADD LINK32 winmm.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build - Performing registration
OutDir=.\XSI2MSHExporter___Win32_Release
TargetPath=.\XSI2MSHExporter___Win32_Release\XSI2MSHExporter.dll
InputPath=.\XSI2MSHExporter___Win32_Release\XSI2MSHExporter.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "XSI2MSHExporter - Win32 Debug"
# Name "XSI2MSHExporter - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Scene2MSH.cpp
# End Source File
# Begin Source File

SOURCE=C:\Softimage3Beta\XSI_3.0\XSISDK\include\simath_id.c
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\XSI2MSHDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\XSI2MSHExporter.cpp
# End Source File
# Begin Source File

SOURCE=.\XSI2MSHExporter.def
# End Source File
# Begin Source File

SOURCE=.\XSI2MSHExporter.idl
# ADD MTL /tlb ".\XSI2MSHExporter.tlb" /h "XSI2MSHExporter.h" /iid "XSI2MSHExporter_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\XSI2MSHExporter.rc
# End Source File
# Begin Source File

SOURCE=.\XSI2MSHProgressDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\XSISceneExporter.cpp
# End Source File
# Begin Source File

SOURCE=C:\Softimage3Beta\XSI_3.0\XSISDK\include\xsisdk.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Scene2MSH.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\XSI2MSHDialog.h
# End Source File
# Begin Source File

SOURCE=.\XSI2MSHProgressDialog.h
# End Source File
# Begin Source File

SOURCE=.\XSISceneExporter.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\XSISceneExporter.rgs
# End Source File
# End Group
# End Target
# End Project

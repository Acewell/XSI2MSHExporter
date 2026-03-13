XSI2MSH Exporter
================

Project Description
-------------------

This VSS project contains versions of the Softimage|XSI "plugin" exporter
that exports XSI model data into Pandemic "Zero format" MSH files.  There
are other locations of this plugin, notably in the CloneWars project vault.
This vault exists to keep track of the various modifications made to the
exporter for special project needs, since there is no way currently to
"version" the plugin.

Each subproject should have its own README.TXT that should be read to 
determine what changes were made, and what "version" of Zero the plugins
originally came from.

IMPORTANT: All versions of the exporter are assumed to be for XSI v2.x,
except the "XSI3" project (or possible future projects with "XSI3" in
the project name).  PLEASE READ THE README notes in any XSI3 project for
important notes on what version of XSI 3.0 to use, etc.

Building
--------

Typically, each sub-version of the exporter should contain the code for
the editor, and zipped up code for the version of Zero and the version
of MSHParser that the exporter links with.

Create a folder layout on your local system that looks like:

<parent>
-->Libraries
---->Zero
---->MSHParser
-->Tools
---->XSI_Plugins
------>XSI2MSHExporter

See the "BuildLayout" project for the correct layout.  Put the Zero code
in Zero, MSHParser code in MSHParser, and the XSI2MSHExporter in, well, you
guessed it, XSI2MSHExporter.  Open the XSI2MSHExporter.dsp and build.

But wait, that didn't work?  You also, of course, need the appropriate
XSI SDK.  You may need to tweak the project files for the exporter, as
different versions point to different locations for the XSI include files.
See the "XSISDK" project for the needed .h/.cpp files, although bear in mind
that these are from v1.5 XSI SDK.


Details
-------

(1)

Starts in XSISceneExporter, which pops up dlg, and then
calls CScene2MSH.CreateZeroStructure.

(2)

In Scene2MSH.cpp->CScene2MSH::CreateZeroStructure,
gets scene data, environment data, material data,
model data, and animation data

(3)

CScene2MSH::CreateZeroStructure calls 
ZeroImportExportNode::OnExportFile, which does a bunch of
extra "Tweaking" of the data.  A number of important things
happen herein, including swapping handedness (from right to
Zero left), tweaking and compression of animation keys, and
collapsing and/or tesselation of model mesh data

(4)

If _DEBUG_LOG is on, a TXT file dump of the MSH data is made
via CZeroData->DebugOutput.

(5)

Finally, the MSH data is written to file, via 
CZeroExporter->ExportFile


So, how does the model acquisition work?  

(1) CreateZeroStructure calls CreateModels which gets the X3DObject rep of the
scene root and walks all the children, adding them into m_AllModels, which is
just a tree of X3DObject ptrs organized by name CRC (just convenience, I believe)

(2) Then, CreateModels calls CreateHierarchy for each child of the root (but not the
root, oddly).  Within CreateHierarchy, we finally get to a call to CreateModel.
CreateModel creates a new ZeroImportExportNode.  The ZeroImportExportNode actually
contains a CMshBlockModel!









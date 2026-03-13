@echo off
rem Prior to XSI 3.0, XSI was setting the environment variable
rem HOME. Since this was causing conflicts for some sites, XSI is 
rem now setting the environment variable XSI_USERROOT instead.
rem If you were overriding the HOME variable, you will now have to
rem override the XSI_USERROOT variable
set XSI_USERROOT=c:\users\%USERNAME%

rem XSI Installation path
set SI_HOME=C:\Softimage\XSI_4.2
set XSI_HOME=%SI_HOME%
set XSI_BINDIR=%XSI_HOME%\Application\bin

rem Soft3D import        
set SI_DBDIR=%XSI_USERROOT%

rem Detect P4 or better
set _CPU_REVISION=-p3
"%XSI_BINDIR%\cpuinfo.exe"
if ERRORLEVEL 15 set _CPU_REVISION=-p4
rem SDK variables
set XSISDK_ROOT=C:\Softimage\XSI_4.2\XSISDK

rem Save original value of XSI_PLUGINS
if NOT "%_XSI_PLUGINS%"=="" goto Done_SaveXSI_PLUGINS
if "%XSI_PLUGINS%"=="" goto EmptyXSI_PLUGINS
set _XSI_PLUGINS=%XSI_PLUGINS%
goto Done_SaveXSI_PLUGINS

:EmptyXSI_PLUGINS
set _XSI_PLUGINS=;

:Done_SaveXSI_PLUGINS
set XSI_PLUGINS=%_XSI_PLUGINS%

rem XSI Plugin for DirectX 9
if NOT EXIST "%XSI_BINDIR%\dxver.exe" goto NODXVER
"%XSI_BINDIR%\dxver.exe" -isver 0x40009
if NOT ERRORLEVEL 1 set XSI_PLUGINS=%XSI_PLUGINS%;%XSI_BINDIR%\DX9

:NODXVER
rem set XSI_PLUGINS=%XSI_PLUGINS%;%XSISDK_ROOT%\examples\menus\SimpleMenu
rem set XSI_PLUGINS=%XSI_PLUGINS%;%XSISDK_ROOT%\examples\filters\SimpleFilter
rem set XSI_PLUGINS=%XSI_PLUGINS%;%XSISDK_ROOT%\examples\properties\PSetUIDemo
rem set XSI_PLUGINS=%XSI_PLUGINS%;%XSISDK_ROOT%\examples\customdisplayhost\CustomUITest
rem set XSI_PLUGINS=%XSI_PLUGINS%;%XSISDK_ROOT%\examples\customdisplayhost\VertexColors
rem set XSI_PLUGINS=%XSI_PLUGINS%;%XSISDK_ROOT%\examples\graphicsequencer\SampleDisplayCallback
rem set XSI_PLUGINS=%XSI_PLUGINS%;%XSISDK_ROOT%\examples\graphicsequencer\DX9Display
rem set XSI_PLUGINS=%XSI_PLUGINS%;%XSISDK_ROOT%\examples\commands\SimpleCommand

rem Render variables
set XSI_EXT=dll
set XSI_CPU=nt-x86
set XSI_CPU_OPT=%XSI_CPU%%_CPU_REVISION%
set XSI_USERHOME=%XSI_USERROOT%\Softimage\XSI_4.2
set MI_ROOT=%XSI_HOME%\Application\rsrc
set MI_RAY3_SERVICE=mi-raysatxsi4_2
set MI_RAY_HOSTSFILE=%XSI_USERHOME%\.ray3hosts

rem Added Application\bin and Application\bin\%XSI_CPU_OPT to the Path
if not "%XSI_SetenvDone%"=="" goto Done_Path
set Path=%XSI_BINDIR%\%XSI_CPU_OPT%;%XSI_BINDIR%;%Path%
:Done_Path

set SI_IMAGE_PATH=%XSI_HOME%\Application\bin\sil

rem Documentation and Editor variables
rem set SI_HELP_PROGRAM_ENV=iexplore.exe
rem set SI_TXT_EDITOR_ENV=notepad.exe

rem Shader plugins variables
set XSI_WORKGROUP_SHADERPLUGINS=%XSI_HOME%\Application\rsrc
set XSI_USER_SHADERPLUGINS=%XSI_HOME%\Application\rsrc
if exist "%XSI_USERHOME%\Addons\shader_plugins.mi" set XSI_USER_SHADERPLUGINS=%XSI_USERHOME%
set _WGRP_SHADERPLUGINS=%XSI_USERHOME%\Addons\workgroup_shader_plugins.bat
if exist "%_WGRP_SHADERPLUGINS%" call "%_WGRP_SHADERPLUGINS%"

rem Set License servers
if not "%XSI_SetenvDone%"=="" goto Done_SPM_HOST

rem License servers specified from the setup
set _SPM_HOST=library
if "%SPM_HOST%"=="" goto EmptySpmHost
if "%_SPM_HOST%"=="" goto Done_SPM_HOST
set SPM_HOST=%_SPM_HOST%;%SPM_HOST%
goto Done_SPM_HOST

:EmptySpmHost
if "%_SPM_HOST%"=="" goto Done_SPM_HOST
set SPM_HOST=%_SPM_HOST%

:Done_SPM_HOST

rem Unset temporary variables
set _WGRP_SHADERPLUGINS=
set _CPU_REVISION=
set SPM_HOST=backupstation;%SPM_HOST%

set XSI_SetenvDone=1
call "C:\Program Files\Microsoft Visual Studio .NET 2003\Common7\IDE\devenv.exe" XSI3\XSIExporter.sln



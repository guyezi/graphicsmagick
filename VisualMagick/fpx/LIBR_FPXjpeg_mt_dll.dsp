# Microsoft Developer Studio Project File - Name="LIBR_FPXjpeg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=LIBR_FPXjpeg - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LIBR_FPXjpeg_mt_dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LIBR_FPXjpeg_mt_dll.mak" CFG="LIBR_FPXjpeg - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LIBR_FPXjpeg - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LIBR_FPXjpeg - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LIBR_FPXjpeg - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir ".\djRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /base:"0x66800000" /subsystem:windows /dll /pdb:"..\bin\LIBR_FPXjpeg.pdb" /machine:I386 /out:"..\bin\LIBR_FPXjpeg.dll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "LIBR_FPXjpeg - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir ".\djDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /base:"0x66800000" /subsystem:windows /dll /incremental:no /pdb:"..\bin\LIBR_DB_FPXjpeg.pdb" /debug /machine:I386 /out:"..\bin\LIBR_DB_FPXjpeg.dll"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "LIBR_FPXjpeg - Win32 Release"
# Name "LIBR_FPXjpeg - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\fpx\jpeg\chen_dct.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\dbuffer.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\decoder.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\dectile.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\dhuff.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\djpeg.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\dllmain.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\dparser.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\dscan.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\ebuffer.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\ejpeg.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\enctile.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\eparser.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\fpxmem.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\huffman.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\mcu2rast.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\process.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\public.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\tables.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\win_dct.c
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\winograd.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\fpx\jpeg\chen_dct.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\dbuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\decoder.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\dectile.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\dhuff.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\djpeg.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\dparser.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\ebuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\ejpeg.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\enctile.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\eparser.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\FPXMem.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\huffman.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\JPEG\jpegconf.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\public.h
# End Source File
# Begin Source File

SOURCE=.\RESOURCE.H
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\tables.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\win_dct.h
# End Source File
# Begin Source File

SOURCE=..\..\fpx\jpeg\winograd.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\LIBR_FPXjpeg.rc
# End Source File
# End Group
# End Target
# End Project

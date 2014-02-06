# Microsoft Developer Studio Project File - Name="utils" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=utils - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "utils.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "utils.mak" CFG="utils - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "utils - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "utils - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""$/Core/utils", NXFEAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe

!IF  "$(CFG)" == "utils - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "d:\Project\Credit\Out\"
# PROP Intermediate_Dir "d:\Project\Credit\Release\utils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "d:\Project\Credit\Out"
# PROP Intermediate_Dir "d:\Project\Credit\Debug\utils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "utils - Win32 Release"
# Name "utils - Win32 Debug"
# Begin Group "Generate documentation files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Doxyfile

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Header files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\analytics.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\atlmfc.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\attributes.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\autoptr.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\commandline.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\config.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\convert.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cp1251.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CtrlsStateManager.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\datahelpers.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\errors.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fileutils.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\format.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GridLayoutTemplates.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GridTemplates.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hexspeek.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IUtilitiesUnknown.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\memserializer.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\msxml\msxml4.tlh

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\nulltype.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ObjectQueue.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\osversion.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PriorSqlTemplates.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\registry.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RegSerializer.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\secure.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\security.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\serialize.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\strconv.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\stringsafe.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\synchronize.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\thread.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tracealloc.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\translate.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\typelist.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnitTests.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\utils.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WinAPIExt.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\xmllangstrings.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\xmlserializer.h

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "examples"

# PROP Default_Filter "cpp;"
# Begin Source File

SOURCE=.\examples\analytic_example.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\examples\commandline_example.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\examples\cp1251_example.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\examples\errors_example.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\examples\ErrorTags_example.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\examples\format_example.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\examples\IUtilitiesUnknown_example.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\examples\registry_example.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "schemas"

# PROP Default_Filter "xsd"
# Begin Source File

SOURCE=.\schemas\LanguageStrings.xsd

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\changes.txt

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\help.zip

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnitTests.tst

!IF  "$(CFG)" == "utils - Win32 Release"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=.\UnitTests.tst

"noname" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	.\UnitTests\Debug\UnitTests.exe

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project

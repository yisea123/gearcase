# Microsoft Developer Studio Project File - Name="SmsSender" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SmsSender - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SmsSender.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SmsSender.mak" CFG="SmsSender - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SmsSender - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SmsSender - Win32 debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/code/SMSSENDER", RKEAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SmsSender - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:".\release\GearCase.exe"

!ELSEIF  "$(CFG)" == "SmsSender - Win32 debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SmsSender___Win32_debug"
# PROP BASE Intermediate_Dir "SmsSender___Win32_debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /ZI /Od /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_DEBUG" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /incremental:yes /debug /machine:I386 /out:".\debug\GearCase.exe"

!ENDIF 

# Begin Target

# Name "SmsSender - Win32 Release"
# Name "SmsSender - Win32 debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AlarmSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BtnST.cpp
# End Source File
# Begin Source File

SOURCE=.\ComSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveColorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DataQueryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DataStorageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DrawLineDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\InputPasswdDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LineChartCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ListCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=.\MD5Checksum.cpp
# End Source File
# Begin Source File

SOURCE=.\ModbusQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\PasswordSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchProcessDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SerialPort.cpp
# End Source File
# Begin Source File

SOURCE=.\SmsSender.cpp
# End Source File
# Begin Source File

SOURCE=.\SmsSender.rc
# End Source File
# Begin Source File

SOURCE=.\SmsSenderDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SpeedDownSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SpeedSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\ToolBar_Add_Word.cpp
# End Source File
# Begin Source File

SOURCE=.\TrayNotifyIcon.cpp
# End Source File
# Begin Source File

SOURCE=.\TrayTimerWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\UserSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\XString.Cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\AlarmSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\BtnST.h
# End Source File
# Begin Source File

SOURCE=.\ComSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\crack.h
# End Source File
# Begin Source File

SOURCE=.\CurveColorDlg.h
# End Source File
# Begin Source File

SOURCE=.\DataQueryDlg.h
# End Source File
# Begin Source File

SOURCE=.\DataStorageDlg.h
# End Source File
# Begin Source File

SOURCE=.\DrawLineDlg.h
# End Source File
# Begin Source File

SOURCE=.\InputPasswdDlg.h
# End Source File
# Begin Source File

SOURCE=.\LineChartCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ListCtrlEx.h
# End Source File
# Begin Source File

SOURCE=.\MD5Checksum.h
# End Source File
# Begin Source File

SOURCE=.\MD5ChecksumDefines.h
# End Source File
# Begin Source File

SOURCE=.\MemDC.h
# End Source File
# Begin Source File

SOURCE=.\ModbusQueue.h
# End Source File
# Begin Source File

SOURCE=.\PasswordSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SearchProcessDlg.h
# End Source File
# Begin Source File

SOURCE=.\SerialPort.h
# End Source File
# Begin Source File

SOURCE=.\SmsSender.h
# End Source File
# Begin Source File

SOURCE=.\SmsSenderDlg.h
# End Source File
# Begin Source File

SOURCE=.\SpeedDownSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\SpeedSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\ToolBar_Add_Word.h
# End Source File
# Begin Source File

SOURCE=.\TrayNotifyIcon.h
# End Source File
# Begin Source File

SOURCE=.\TrayTimerWnd.h
# End Source File
# Begin Source File

SOURCE=.\UserSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\XString.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\027.ico
# End Source File
# Begin Source File

SOURCE=.\res\autostart.ico
# End Source File
# Begin Source File

SOURCE=.\res\cartoon.ico
# End Source File
# Begin Source File

SOURCE=.\res\cartoon01.ico
# End Source File
# Begin Source File

SOURCE=".\res\Chicago Bulls.ico"
# End Source File
# Begin Source File

SOURCE=.\res\DRIVEDSC.ICO
# End Source File
# Begin Source File

SOURCE=.\res\DRIVENET.ICO
# End Source File
# Begin Source File

SOURCE=.\res\gearcase_2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\green.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00004.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\neusoft.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Play.ico
# End Source File
# Begin Source File

SOURCE=.\res\red.ico
# End Source File
# Begin Source File

SOURCE=.\res\SmsSender.ico
# End Source File
# Begin Source File

SOURCE=.\res\SmsSender.rc2
# End Source File
# Begin Source File

SOURCE=.\res\Stop.ico
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Watermelon.ico
# End Source File
# Begin Source File

SOURCE=.\res\ZM.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\res\search.avi
# End Source File
# Begin Source File

SOURCE=.\res\servicefind.avi
# End Source File
# End Target
# End Project
# Section SmsSender : {648A5600-2C6E-101B-82B6-000000000014}
# 	2:21:DefaultSinkHeaderFile:mscomm.h
# 	2:16:DefaultSinkClass:CMSComm
# End Section
# Section SmsSender : {E6E17E90-DF38-11CF-8E74-00A0C90F26F8}
# 	2:5:Class:CMSComm
# 	2:10:HeaderFile:mscomm.h
# 	2:8:ImplFile:mscomm.cpp
# End Section

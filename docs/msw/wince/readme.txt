
  =================================================
  Welcome to wxWidgets/CE 2.5.2
  =================================================

You have downloaded version 2.5.2 of the Windows CE port of
the wxWidgets GUI library. This runs on PocketPC 2002,
SmartPhone 2002, and Windows CE .NET 4.x.

More information about the wxWidgets project as a whole
can be found at:

  http://www.wxwidgets.org

Information about the Windows CE port in particular
can be found here:

http://wiki.wxwindows.org/wiki.pl?Developers_Notebook/WxWinCE

Supported platforms
===================

These are the supported platforms at the moment:

- x86 emulator
- PocketPC (StrongARM)

You can test the library and applications using PocketPC emulator 
and/or Compaq iPAQ or other PDAs using PocketPC

Compiler installation
=====================

Get the Visual C++ embedded (FREE) from Microsoft website
 http://msdn.microsoft.com/vstudio/device/embedded/download.asp
 
You must use Embedded Visual C++ 3.0 or Embedded Visual C++ 4.x.
 
Install selecting only the two target platforms used:

- WCE x86em (PocketPC emulation)
- WCE ARM (PocketPC StrongARM)

You can install other targets but you will need
to create new configurations for them in the
wxWidgets project files.

wxWidgets/CE Configuration
================================

You may wish to customize the following file
before compiling wxWinCE:

include/wx/msw/wince/setup.h

These are some of the Windows CE-specific settings
you can adjust.

wxUSE_POCKETPC_UI

  If you are compiling with Windows CE .NET, you may
  wish to emulate PocketPC behaviour (set it to 1), or
  take advantage of the greater flexibility of
  Windows CE .NET (set it to 0).
  If 0, the PocketPC-specific toolbar class will be
  used, which uses commandbar to combine the toolbar
  and menubar in one control. If 1, the normal menubar
  and toolbar behaviour will be used, as per
  desktop Windows.

wxUSE_SMARTPHONE

  Set this to 1 if you wish to compile for the SmartPhone
  platform (with eVC++ 3).

wxWidgets/CE Compilation
================================

Open src/msw/wince/wxWidgetsCE.vcp, select an
ARM or x86 target (or emulator target for eVC++ 4),
and compile.

Then open samples/minimal/minimalCE.vcp, select an
ARM, x86 or emulator target, and compile. You can
use the emulator or real hardware to run the
sample.

There may be long delays while VC++ computes dependencies, and expect
to wait a long time for loading and saving the workspace. 

To compile using the emulator on eVC++3:

- Open src/msw/wince/wxWidgetsCE.vcp, select the
  WIN32 (WCE x86) Debug Unicode configuration, close the dialog,
  then select Pocket PC 2002 and Pocket PC 2002 Emulation on the toolbar,
  and compile.

- Open samples/minimal/minimalCE.vcp, select the
  WIN32 (WCE x86 Debug) configuration, select Pocket PC 2002 and
  Pocket PC 2002 Emulation as before, and compile.

To compile using the emulator on eVC++4:

- Open src/msw/wince/wxWidgetsCE.vcp, select the
  WIN32 (WCE Emulator) Debug Unicode configuration, and compile.

- Open samples/minimal/minimalCE.vcp with eCV 4.0, select the
  WIN32 (WCE emulator) Debug configuration, and compile.

Status
======

Currently under development. For further information
about what has been done and what needs to be
done, see:

http://wiki.wxwindows.org/wiki.pl?Developers_Notebook/WxWinCE

Disclaimer
==========

All product names are used for identification purposes and are trademarks 
or registered trademarks owned by their respective companies.



  =================================================
  Welcome to wxWidgets/CE 2.5.3
  =================================================

You have downloaded version 2.5.3 of the Windows CE port of
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

- x86 emulator build in eVC 3 and eVC 4.x
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

wxWidgets/CE Compilation
================================

NOTE: to generate targets not in the standard project files,
you should rebuild the project files using Bakefile.
See technote docs/tech/tn0016.txt for details.

Open build/wince/wx.vcw, select an
ARM or x86 target (or emulator target for eVC++ 4),
and compile.

Then open samples/minimal/minimal.vcw, select an
ARM, x86 or emulator target, and compile. You can
use the emulator or real hardware to run the
sample.

There may be long delays while VC++ computes dependencies, and expect
to wait a long time for loading and saving the workspace. 

To compile using the emulator on eVC++3:

- Open build/wince/wx.vcw, select the
  WIN32 (WCE x86) Debug Unicode configuration, close the dialog,
  then select Pocket PC 2002 and Pocket PC 2002 Emulation on the toolbar,
  and compile.

- Open samples/minimal/minimal.vcw, select the
  WIN32 (WCE x86 Debug) configuration, select Pocket PC 2002 and
  Pocket PC 2002 Emulation as before, and compile.

To compile using the emulator on eVC++4:

- Open build/wince/wx.vcw, select the
  WIN32 (WCE Emulator) Debug Unicode configuration, and compile.

- Open samples/minimal/minimal.vcw with eCV 4.0, select the
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


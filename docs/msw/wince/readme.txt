
  =================================================
  Welcome to wxWidgets/CE 2.6.2
  =================================================

You have downloaded version 2.6.2 of the Windows CE port of
the wxWidgets GUI library. This runs on Pocket PC 2002/2003,
Smartphone 2002/2003, and Windows CE .NET 4.x.

More information about the wxWidgets project as a whole
can be found at:

  http://www.wxwidgets.org

Information about the Windows CE port in particular can be found in
the wxWinCE topic in the wxWidgets reference manual.

Supported platforms
===================

These are the supported platforms at the moment:

- x86 emulator build in eVC 3 and eVC 4.x
- Pocket PC (StrongARM)
- Smartphone (StrongARM)

You can test the library and applications using the Pocket PC emulator 
and/or Compaq iPAQ or other PDAs using Pocket PC.

Compiler installation
=====================

Get the Visual C++ embedded (FREE) from Microsoft website
 http://msdn.microsoft.com/vstudio/device/embedded/download.asp
 
You must use Embedded Visual C++ 3.0 or Embedded Visual C++ 4.x.

You can download the Smartphone 2002/3 and Pocket PC 2002/3
SDKs separately.
 
wxWidgets/CE Configuration
================================

You may wish to customize the following file
before compiling wxWinCE:

include/wx/msw/wince/setup.h

wxWidgets/CE Compilation
================================

NOTE: to generate targets not in the standard project files,
you should rebuild the project files using Bakefile.
Also, if you are building from a CVS version of wxWidgets,
you need to create eVC++ project files or get them from
somewhere, such as the daily makefile builds here:

http://biolpc22.york.ac.uk/pub/CVS_Makefiles/

See technote docs/tech/tn0016.txt for details on using Bakefile,
and you can get Bakefile from:

http://bakefile.sf.net

Open build/wince/wx.vcw, select an ARM or x86 target (or emulator
target for eVC++ 4), and compile.

Then open samples/minimal/minimal.vcw, select an ARM, x86 or emulator
target, and compile. You can use the emulator or real hardware to run
the sample.

There may be long delays while eVC++ computes dependencies, and expect
to wait a long time for loading and saving workspaces. Do not attempt to
save the wxWidgets library project file: it will take forever and generate
a huge file, so make alterations either using Bakefile or by editing the
project file with a text editor, and reloading. Sample project files are OK
to save.

To compile using the emulator on eVC++3:

- Open build/wince/wx.vcw, select the
  WIN32 (WCE x86) Debug Unicode configuration, close the dialog,
  then select Pocket PC 2002 and Pocket PC 2002 Emulation on the toolbar,
  and compile.

- Open samples/minimal/minimal.vcw, select the
  WIN32 (WCE x86 Debug) configuration, and select Pocket PC 2002 and
  Pocket PC 2002 Emulation as before.

Note: since wxWidgets/CE 2.6.2 release RTTI (and exceptions) are enabled
by default targeting eVC environment. This can lead to build problems with
eVC3 we tried to compensate. It is possible that you still have to remove
some compiler flags (like /EHsc) by hand after opening in IDE.

To compile using the emulator on eVC++4:

- Open build/wince/wx.vcw, select the
  WIN32 (WCE Emulator) Debug configuration, and compile.

- Open samples/minimal/minimal.vcw with eVC++, select the
  WIN32 (WCE emulator) Debug configuration, and compile.

Note: builds for the different SDKs (Smartphone, PPC, .Net)
use the same directory and library names, so for the moment
these builds cannot co-exist simultaneously.

Status
======

It's possible to create working Pocket PC or Smartphone
applications with this port. For further information
about how to use wxWidgets with Windows CE, and further
work that needs to be done, see the wxWinCE topic in the
reference manual, and also:

http://wiki.wxwindows.org/wiki.pl?Developers_Notebook/WxWinCE

The "Life!" demo in demos/life has some adaptations for
wxWinCE, and also demonstrates how to make an installer for
a Pocket PC application.

Disclaimer
==========

All product names are used for identification purposes and are trademarks 
or registered trademarks owned by their respective companies.


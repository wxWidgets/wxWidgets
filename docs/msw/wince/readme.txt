
  =================================================
  Welcome to wxWindows/CE 2.5.1
  =================================================

You have downloaded version 2.5.1 of the Windows CE port of
the wxWindows GUI library. This runs on PocketPC 2002
and SmartPhone 2002.

More information about the wxWindows project as a whole
can be found at:

  http://www.wxwindows.org

Information about the Windows CE port in particular
can be found here:

http://wiki.wxwindows.org/wiki.pl?Windows_CE_Port

Supported platforms
===================

These are the supported platform at the moment:

- x86 emulator
- PocketPC (StrongARM)

You can test the library and applications using PocketPC emulator 
and/or Compaq iPAQ or other PDAs using PocketPC

WindowsCE.NET is not supported.

Compiler installation
=====================

Get the Visual C++ embedded (FREE) from Microsoft website
 http://msdn.microsoft.com/vstudio/device/embedded/download.asp
 
You must use Visual Tools 3.0 for CE 3.0 
 (this is the same we are currently using for wxWinCE) 
 
Install selecting only the two target platforms used:

- WCE x86em (PocketPC emulation)
- WCE ARM (PocketPC StrongARM)

Download an archive containing the library project file
and a sample project file:

http://www.anthemion.co.uk/wxwin/wxWinCE.zip

Unarchive this over your wxWindows installation.

wxWindows/CE Compilation
================================

Open src/msw/wince/wxWindowsCE.vcp, select an
ARM or x86 target, and compile.

Then open samples/minimal/minimalCE.vcp, select an
ARM or x86 target, and compile. You can
use the emulator or real hardware to run the
sample.

Status
======

Currently under development. For further information
about what has been done and what needs to be
done, see:

http://wiki.wxwindows.org/wiki.pl?Windows_CE_Port

Disclaimer
==========

All product names are used for identification purposes and are trademarks 
or registered trademarks owned by their respective companies.



  =================================================
  Welcome to wxWidgets/CE
  =================================================

This is the readme file for the Windows CE port of the wxWidgets GUI library
which runs on Pocket PC 2002/2003, Smartphone 2002/2003, Windows CE .NET 4.x,
and Windows Mobile 5. This port requires the use of Microsoft Visual Studio
2005 and 2008, in particular eVC (Visual C++ Embedded) is not supported any
longer.

More information about the wxWidgets project as a whole can be found at:

  http://www.wxwidgets.org/

Information about the Windows CE port in particular can be found in
the wxWinCE topic in the wxWidgets reference manual.


wxWidgets/CE Configuration
================================

You may wish to customize the file include/wx/msw/wince/setup.h before building
wxWinCE to disable any features that your program doesn't need and minimize the
size of the library.


wxWidgets/CE Compilation
================================

Unless the archive you downloaded already included CE-specific project files,
you first need to generate them yourself. For this please install bakefile
(from http://www.bakefile.org/, see technote docs/tech/tn0016.txt for more
details about it) and create the file build/bakefiles/Bakefiles.local.bkgen
with the following contents for Visual Studio 2008 (replace 2008 with 2005
everywhere for the previous version):

<?xml version="1.0" ?>

<bakefile-gen xmlns="http://www.bakefile.org/schema/bakefile-gen">
    <add-formats>msvs2008prj(arm)</add-formats>
    <add-flags files="wx.bkl" formats="msvs2008prj(arm)">
        -o ../msw/wx_vc9arm.sln -DMSVS_PLATFORMS=pocketpc2003 -DCOMPILER_PREFIX=vc9arm
    </add-flags>
    <add-flags files="../../samples/*/*" formats="msvs2008prj(arm)">
        -o $(INPUT_FILE_DIR)/$(INPUT_FILE_BASENAME_NOEXT)_vc9arm.sln -DMSVS_PLATFORMS=pocketpc2003 -DCOMPILER_PREFIX=vc9arm
    </add-flags>
</bakefile-gen>

Notice that this will create the output files in the directories using "vc9arm"
prefix instead of the usual "vc" one which allows to build the normal wxMSW and
wxCE in the same wxWidgets source tree. You're, of course, free, to use
different names for the project files suffix and the output directories prefix
if you wish.

Then do

cd %WXWIN%\build\bakefiles
bakefile_gen -f msvs2008prj

to create all *vc9arm.vcproj files. After this, simply open the solution file
in Visual Studio and build it.


Status
======

It's possible to create working Pocket PC or Smartphone
applications with this port. For further information
about how to use wxWidgets with Windows CE, and further
work that needs to be done, see the wxWinCE topic in the
reference manual, and also:

http://wiki.wxwidgets.org/Developers_Notebook/WxWinCE

The "Life!" demo in demos/life has some adaptations for
wxWinCE, and also demonstrates how to make an installer for
a Pocket PC application.

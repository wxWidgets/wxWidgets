wxPython README
---------------

Welcome to the wonderful world of wxPython!

Once you have installed the wxPython extension module, you can try it
out by going to the [install dir]\wxPython\demo directory and typing:

    python demo.py

There are also some other sample files there for you to play with and
learn from.

If you selected to install the documentation then point your browser
to [install dir]\wxPython\docs\index.htm and you will then be looking
at the docs for wxWindows.  For the most part you can use the C++ docs
as most classes and methods are used identically.  Where there are
differences they are documented with a "wxPython Note."

On Win32 systems the binary self-installer creates a program group on
the Start Menu that contains a link to running the demo and a link to
the help file.  To help you save disk space I'm now using Microsoft's
HTML Help format.  If your system doesn't know what to do with the help
file, you can install the HTML Help Viewer as part of IE 4+, NT
Service Pack 4+, or the HTML Workshop at

http://msdn.microsoft.com/workshop/author/htmlhelp/download.asp.



Getting Help
------------

Since wxPython is a blending of multiple technologies, help comes from
multiple sources.  See the http://alldunn.com/wxPython for details on
various sources of help, but probably the best source is the
wxPython-users mail list.  You can view the archive or subscribe by
going to

       http://starship.python.net/mailman/listinfo/wxpython-users

Or you can send mail directly to the list using this address:

       wxpython-users@starship.python.net

----------------------------------------------------------------------

What's new in 2.1b3
--------------------

This release is syncronized with release 2.1 snapshot 9 of wxWindows.

Switched to using SWIG from CVS (see http://swig.cs.uchicago.edu/cvs.html)
for some of the new features and such.  Also they have encorporated my
patches so there is really no reason to stick with the current (very
old) release...  This version of SWIG gives the following new
features:

    1. Keyword arguments.  You no longer have to specify all the
       parameters with defaults to a method just to specify a
       non-default value on the end.  You can now do this instead:

	  win = wxWindow(parent, -1, style = mystyle)

    2. There is now an an equivalence between Python's None and C++'s
       NULL.  This means that any methods that might return NULL will
       now return None and you can use none where wxWindows might be
       expecting NULL.  This makes things much more snake-ish.


There is a new build system based on a new Python program instead of
raw makefiles.  Now wxPython builds are virtually the same on MSW or
Unix systems.  See the end of this file for new build instructions and
see distrib/build.py for more details.

wxDC.Bilt now includes the useMask parameter, and has been split into
two different versions.  wxDC.BlitXY is like what was there before and
takes raw coordinants and sizes, and the new wxDC.Blit is for the new
interface using wxPoints and a wxSize.





What's new in 2.1b2
--------------------

Added the missing wxWindow.GetUpdateRegion() method.

Made a new change in SWIG (update your patches everybody) that
provides a fix for global shadow objects that get an exception in
their __del__ when their extension module has already been deleted.
It was only a 1 line change in .../SWIG/Modules/pycpp.cxx at about
line 496 if you want to do it by hand.

It is now possible to run through MainLoop more than once in any one
process.  The cleanup that used to happen as MainLoop completed (and
prevented it from running again) has been delayed until the wxc module
is being unloaded by Python.

I fixed a bunch of stuff in the C++ version of wxGrid so it wouldn't
make wxPython look bad.

wxWindow.PopupMenu() now takes a wxPoint instead of  x,y.  Added
wxWindow.PopupMenuXY to be consistent with some other methods.

Added wxGrid.SetEditInPlace and wxGrid.GetEditInPlace.

You can now provide your own app.MainLoop method.  See
wxPython/demo/demoMainLoop.py for an example and some explaination.

Got the in-place-edit for the wxTreeCtrl fixed and added some demo
code to show how to use it.

Put the wxIcon constructor back in for GTK as it now has one that
matches MSW's.

Added wxGrid.GetCells

Added wxSystemSettings static methods as functions with names like
wxSystemSettings_GetSystemColour.

Removed wxPyMenu since using menu callbacks have been depreciated in
wxWindows.  Use wxMenu and events instead.

Added alternate wxBitmap constructor (for MSW only) as
      wxBitmapFromData(data, type, width, height, depth = 1)

Added a helper function named wxPyTypeCast that can convert shadow
objects of one type into shadow objects of another type.  (Like doing
a down-cast.)  See the implementation in wx.py for some docs.

Fixed wxImage GetData and SetData to properly use String objects for
data transfer.

Added access methods to wxGridEvent.

New Makefile/Setup files supporting multiple dynamic extension modules
for unix systems.

Fixes for the wxGLCanvas demo to work around a strange bug in gtk.

SWIG support routines now compiled separately instead of being bundled
in wx.cpp.





What's new in 2.1b1
--------------------
Fixed wxComboBox.SetSelection so that it actually sets the selected
item.  (Actually just removed it from wxPython and let it default to
wxChoice.SetSelection which was already doing the right thing.)

Added the Printing Framework.

Switched back to using the wxWindows DLL for the pre-built Win32
version.  The problem was needing to reinitialize static class info
data after loading each extension module.

Lots of little tweaks and additions to reflect changes to various
wxWindows classes.

Fixed a bug with attaching objects to tree items.  Actually was a
symptom of a larger problem with not obtaining the interpreter lock
when doing any Py_DECREFs.

wxSizer and friends.  Sizers are layout tools that manage a colection
of windows and sizers.  Different types of sizers apply different
types of layout algorithms.  You saw it here first!  These classes are
not even in the wxWindows C++ library yet!



What's new in 2.0b9
-------------------
Bug fix for ListCtrl in test4.py (Was a missing file...  DSM!)

Bug fix for occassional GPF on Win32 systems upon termination of a
wxPython application.

Added wxListBox.GetSelections returning selections as a Tuple.

Added a wxTreeItemData that is able to hold any Python object and be
associated with items in a wxTreeCtrl.  Added test pytree.py to show
this feature off.

Added wxSafeYield function.

OpenGL Canvas can be optionally compiled in to wxPython.

Awesome new Demo Framework for showing off wxPython and for learning
how it all works.

The pre-built Win32 version is no longer distributing the wxWindows
DLL.  It is statically linked with the wxWindows library instead.

Added a couple missing items from the docs.

Added wxImage, wxImageHandler, wxPNGHandler, wxJPEGHandler,
wxGIFHandler and wxBMPHandler.

Added new methods to wxTextCtrl.

Fixed some problems with how SWIG was wrapping some wxTreeCtrl
methods.



What's new in 2.0b8
-------------------
Support for using Python threads in wxPython apps.

Several missing methods from various classes.

Various bug fixes.



What's new in 2.0b7
-------------------
Added DLG_PNT and DLG_SZE convienience methods to wxWindow class.

Added missing constructor and other methods for wxMenuItem.



What's new in 2.0b6
-------------------
Just a quickie update to fix the self-installer to be compatible with
Python 1.5.2b2's Registry settings.


What's new in 2.0b5
-------------------
Well obviously the numbering scheme has changed.  I did this to
reflect the fact that this truly is the second major revision of
wxPython, (well the third actually if you count the one I did for
wxWindows 1.68 and then threw away...) and also that it is associated
with the 2.0 version of wxWindows.

I have finally started documenting wxPython.  There are several pages
in the wxWindows documentation tree specifically about wxPython, and I
have added notes within the class references about where and how wxPython
diverges from wxWindows.

Added wxWindow_FromHWND(hWnd) for wxMSW to construct a wxWindow from a
window handle.  If you can get the window handle into the python code,
it should just work...  More news on this later.

Added wxImageList, wxToolTip.

Re-enabled wxConfig.DeleteAll() since it is reportedly fixed for the
wxRegConfig class.

As usual, some bug fixes, tweaks, etc.



What's new in 0.5.3
-------------------
Added wxSashWindow, wxSashEvent, wxLayoutAlgorithm, etc.

Various cleanup, tweaks, minor additions, etc. to maintain
compatibility with the current wxWindows.



What's new in 0.5.0
-------------------
Changed the import semantics from "from wxPython import *" to "from
wxPython.wx import *"  This is for people who are worried about
namespace pollution, they can use "from wxPython import wx" and then
prefix all the wxPython identifiers with "wx."

Added wxTaskbarIcon for wxMSW.

Made the events work for wxGrid.

Added wxConfig.

Added wxMiniFrame for wxGTK.

Changed many of the args and return values that were pointers to gdi
objects to references to reflect changes in the wxWindows API.

Other assorted fixes and additions.




What's new in 0.4.2
-------------------

wxPython on wxGTK works!!!  Both dynamic and static on Linux and
static on Solaris have been tested.  Many thanks go to Harm
<H.v.d.Heijden@phys.tue.nl> for his astute detective work on tracking
down a nasty DECREF bug.  Okay so I have to confess that it was just a
DSM (Dumb Stupid Mistake) on my part but it was nasty none the less
because the behavior was so different on different platforms.

The dynamicly loaded module on Solaris is still segfaulting, so it
must have been a different issue all along...



What's New in 0.4
-----------------

1. Worked on wxGTK compatibility.  It is partially working.  On a
Solaris/Sparc box wxPython is working but only when it is statically
linked with the Python interpreter.  When built as a dyamically loaded
extension module, things start acting weirdly and it soon seg-faults.
And on Linux both the statically linked and the dynamically linked
version segfault shortly after starting up.

2. Added Toolbar, StatusBar and SplitterWindow classes.

3. Varioius bug fixes, enhancements, etc.

----------------------------------------------------------------------



Build Instructions
------------------
I used SWIG (http://www.swig.org) to create the source code for the
extension module.  This enabled me to only have to deal with a small
amount of code and only have to bother with the exceptional issues.
SWIG takes care of the rest and generates all the repetative code for
me.  You don't need SWIG to build the extension module as all the
generated C++ code is included under the src directory.

I added a few minor features to SWIG to control some of the code
generation.  If you want to play around with this you will need to get
a recent version of SWIG from their CVS or from a daily build.  See
http://www.swig.org/ for details.

wxPython is organized as a Python package.  This means that the
directory containing the results of the build process should be a
subdirectory of a directory on the PYTHONPATH.  (And preferably should
be named wxPython.)  You can control where the build process will dump
wxPython by setting the TARGETDIR variable for the build utility, (see
below.)


1. Build wxWindows as described in its BuildCVS.txt file.  For *nix
   systems I run configure with these flags:

                --with-gtk
                --with-libjpeg
                --without-odbc
                --enable-unicode=no
                --enable-threads=yes
                --enable-socket=yes
                --enable-static=no
                --enable-shared=yes
                --disable-std_iostreams

   You can use whatever flags you want, but I know these work.

   For Win32 systems I use Visual C++ 6.0, but 5.0 should work.  The
   build utility currently does not support any other win32 compilers.

2. At this point you may want to make an alias or symlink, script,
   batch file, whatever on the PATH that invokes
   $(WXWIN)/utils/wxPython/distrib/build.py to help simplify matters
   somewhat.  For example, on my win32 system I have a file named
   build.bat in a directory on the PATH that contains:

   python $(WXWIN)/utils/wxPython/distrib/build.py %1 %2 %3 %4 %5 %6


3. Change into the $(WXWIN)/utils/wxPython/src directory.

4. Type "build -b" to build wxPython and "build -i" to install it.

   The build.py script actually generates a Makefile based on what it
   finds on your system and information found in the build.cfg file.
   If you have troubles building or you want it built or installed in
   a different way, take a look at the docstring in build.py.  You may
   be able to override configuration options in a file named
   build.local.

5. To build and install the add-on modules, change to the appropriate
   directory under $(WXWIN)/utils/wxPython/modules and run the build
   utility again.

6. Change to the $(WXWIN)/utils/wxPython/demo directory.

7. Try executing the demo program.  For example:

    python demo.py

To run it without requiring a console on win32, you can use the
pythonw.exe version of Python either from the command line or from a
shortcut.



----------------
Robin Dunn
robin@alldunn.com








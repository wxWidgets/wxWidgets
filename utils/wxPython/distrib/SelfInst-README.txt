wxPython README
---------------

Welcome to the wonderful world of wxPython!

Now that you have installed the Win32 extension module, you can try it
out by going to the [install dir]\wxPython\demo directory and typing:

    python test1.py

There are also some other sample files there for you to play with and
learn from.

If you selected to install the documentation then point your browser
to [install dir]\wxPython\docs\index.htm and you will then be looking
at the docs for wxWindows.  For the most part you can use the C++ docs
as most classes and methods are used identically.  Where there are
differences they are documented with a "wxPython Note."



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



----------------
Robin Dunn
robin@alldunn.com




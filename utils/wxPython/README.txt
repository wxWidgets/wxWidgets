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

What's new in 2.1.12
--------------------
Updated wxMVCTree and added a demo for it, also fixed layout on GTK
and some flicker problems.

Added a wrapper class for the Visualization ToolKit (or VTK) in the
wxPython.lib.vtk module.  (http://www.kitware.com/)

Fixed wxTreeCtrl.SetItemImage and GetItemImage to recognise the new
"which" parameter.

Added wxPython.lib.spashscreen from Mike Fletcher.

Added wxPython.lib.filebrowsebutton also from Mike Fletcher.

Renamed wxTreeCtrl.GetParent to GetItemParent to avoid a name clash
with wxWindow.GetParent.

Added wxIntersectRect to computer the intersection of two wxRect's.
It is used like this:

   intersect = wxIntersectRect(rect1, rect2)

If r1 and r2 don't intersect then None is returned, otherwise the
rectangle representing the intersection is returned.



What's new in 2.1.11
--------------------
Skipped a few version numbers so wxMSW, wxGTK and wxPython are all
syncronized.

wxImage.SetData now makes a copy of the image data before giving it to
wxImage.  I mistakenly thought that wxImage would copy the data
itself.

Fixed wxMSW's notebook so the pages get their size set as they are
being added.  This should remove the need for our
wxNotebook.ResizeChildren hack.

wxPanels now support AutoLayout, and wxNotebooks and wxSplitterWindows
no longer tell their children to Layout() themselves.  This will
probably only effect you if you have a wxWindow with AutoLayout inside
a notebook or splitter.  If so, either change it to a wxPanel or add
an EVT_SIZE handler that calls Layout().

Fixed deadlock problem that happened when using threads.

Added new HTML printing classes.

Added wxWindow.GetHandle

Apparently wxMouseEvent.Position has been depreciated in wxWindows as
it is no longer available by default.  You can use GetPositionTuple
(returning a tuple with x,y) instead, or GetPosition (returning a
wxPoint.)

Added wxPostEvent function that allows events to be posted and then
processed later.  This is a thread-safe way to interact with the GUI
thread from other threads.

Added Clipboard and Drag-and-Drop classes.

Added wxFontEnumerator.

Many updates to wxMenu, wxMenuBar.

wxPyEvent and wxPyCommandEvent derived classes now give you the actual
Python object in the event handler instead of a new shadow.

Added a Calendar widget from Lorne White to the library.

Made some fixes to the wxFloatbar.  It still has some troubles on
wxGTK...

Added an MVC tree control from Bryn Keller to the library.




What's new in 2.1.5
-------------------
This is a quick bug-fix release to take care of a few nasties that
crept in at the last minute before 2.1.4 was called done.  No new
major features.



What's new in 2.1.4
--------------------

This release is NOT syncronized with a snapshot release of wxGTK or
wxMSW.  For MSW this isn't much of a problem since you can get the
binaries from the web site.  For other platforms you'll have to build
wxGTK from CVS.  (See http://web.ukonline.co.uk/julian.smart/wxwin/cvs.htm)
To get the same set of sources from CVS that I used, checkout using
the wxPy-2-1-4 tag.

Now back to what's new...

Much more support for event-less callbacks and add-on modules.

Created add-on module with wxOGL classes.

Added wxWindow.GetChildren().  Be careful of this.  It returns a *copy*
of the list of the window's children.  While you are using the list if
anything changes in the real list (a child is deleted, etc.) then the
list you are holding will suddenly have window references to garbage
memory and your app will likely crash.  But if you are careful it works
great!

Added a bunch of new and missing methods to wxTreeCrtl.  The
SortChildren method is now supported, but currently only for the
default sort order.

Added typemaps for wxSize, wxPoint, wxRealPoint, and wxRect that allow
either the actual objects or Python sequence values to be used.  For
example, the following are equivallent:

    win = wxWindow(parent, size = wxSize(100, 100))
    win = wxWindow(parent, size = (100, 100))

Super-charged the wxHtml module.  You can now create your own tag
handlers and also have access to the parser and cell classes.  There
is a tag handler in the library at wxPython.lib.wxpTag that
understands the WXP tag and is able to place wxPython windows on HTML
pages.  See the demo for an example.

A bunch of the methods of wxMenuBar were previously ifdef'd out for
wxGTK.  Added them back in since the methods exist now.

Wrapped the wxHtmlHelpController and related classes.

Wrapped the C++ versions of wxSizer and friends.  The Python-only
versions are still in the library, but depreciated.  (You will get a
warning message if you try to use them, but the warning can be
disabled.) The usage of the C++ versions is slightly different, and
the functionality of wxBorderSizer is now part of wxBoxSizer.  I have
added a few methods to wxSizer to try and make the transition as
smooth as possible, I combined all Add methods into a single method
that handles all cases, added an AddMany method, etc.  One step I did
not take was to make the default value of flag in the Add method be
wxGROW.  This would have made it more backward compatible, but less
portable to and from wxWin C++ code.  Please see the docs and demo for
further details.

Added wxPyEvent and wxPyCommandEvent classes, derived from wxEvent and
wxCommandEvent.  Each of them has SetPyData and GetPyData methods that
accept or return a single Python object.  You can use these classes
directly or derive from them to create your own types of event objects
that can pass through the wxWindows event system without loosing their
Python parts (as long as they are stored with SetPyData.)  Stay tuned
for more info and examples in future releases.

Added wxPython.lib.grids as an example of how to derive a new sizer
from the C++ sizers.  In this module you will find wxGridSizer and
wxFlexGridSizer.  wxGridSizer arrainges its items in a grid in which
all the widths and heights are the same.  wxFlexgridSizer allows
different widths and heights, and you can also specify rows and/or
columns that are growable.  See the demo for a couple examples for how
to use them.

Added the wxValidator class, and created a class named wxPyValidator
that should be used for the base class of any Python validators.  See
the demo for an example.  Please note that you MUST implement a Clone
method in your validator classes because of the way some things work
in the underlying C++ library.  I did not add wxTextValidator because
of some issues of how it transfers data to and from a wxString, which
in wxPython is automatically translated to and from Python strings, so
there would never be a concrete wxString that would hang around long
enough for the validator to do its job.  On the other hand, it should
be real easy to duplicate the functionality of wxTextValidator in a
pure Python class derived from wxPyValidator.

I've finally added a feature that has been on my list for close to two
years!  Ever wondered what that zero is for when you create your app
object?  Well now you can leave it out or explicitly set it to a true
value.  This value now controls what is to be done with sys.stdout and
sys.stderr.  A false value leaves them alone, and a true value sets
them to an instance of wxPyOnDemandOutputWindow.  (On windows the
default is true, on unix platforms the default is false.)  This class
creates a frame containing a wxTextCtrl as soon as anything is written
to sys.stdout or sys.stderr.  If you close the window it will come
back again the next time something is written.  (You can call
app.RestoreStdio to turn this off.)  If you would rather that the stdio be
redirected to a file, you can provide a second parameter to your app
object's constructor that is a filename.  If you want to use your own
class instead of wxPyOnDemandOutputWindow you can either implement
RedirectStdio() in you app class or change the value of
wxApp.outputWindowClass like this:

    class MyApp(wxApp):
        outputWindowClass = MyClass

        def OnInit(self):
            frame = MyFrame()
            self.SetTopWindow(frame)
            return true

Please see the implementation of wxPyOnDemandOutputWindow and wxApp in
wx.py for more details.  A few words of caution:  if you are running
your app in a debugger, changing sys.stdout and sys.stderr is likely
to really screw things up.

Added wxCaret.  Unfortunately it's author has still not documented it
in the wxWindows docs...

Some new 3rd party contributions in wxPython.lib.  PyShell, in
shell.py is an interesting implementaion of an interactive Python
shell in wxWindows.  floatbar.py has a class derived from wxToolBar
that can sense mouse drags and then reparent itself into another
frame. Moving the new frame close to where it came from puts the tool
bar back into the original parent.  (Unfortunately there is currently
a bug in wxGTK's wxFrame.SetToolBar so the FloatBar has some
problems...)




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




----------------
Robin Dunn
robin@alldunn.com








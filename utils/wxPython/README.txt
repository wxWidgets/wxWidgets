wxPython README
---------------

Introduction
------------
The code in this subtree is a Python Extension Module that enables the
use of wxWindows from the Python language.  So what is Python?  Go to
http://www.python.org to learn more but in a nutshell, it's an
extremly cool object oriented language.  It's easier than Perl and
nearly as powerful.  It runs on more platforms than Java, and by some
reports, is even faster than Java with a JIT compiler!

So why would you want to use wxPython over just C++ and wxWindows?
Personally I prefer using Python for everything.  I only use C++ when
I absolutly have to eek more performance out of an algorithm, and even
then I ususally code it as an extension module and leave the majority
of the program in Python.  Another good thing to use wxPython for is
quick prototyping of your wxWindows apps.  With C++ you have to
continuously go though the edit-compile-link-run cycle, which can be
quite time comsuming.  With Python it is only an edit-run cycle.  You
can easily build an application in a few hours with Python that would
normally take a few days with C++.  Converting a wxPython app to a
C++/wxWindows app should be a straight forward task.

This extension module attempts to mirror the class heiarchy of
wxWindows as closely as possble.  This means that there is a wxFrame
class in wxPython that looks, smells, tastes and acts almost the same
as the wxFrame class in the C++ version.  Unfortunatly, I wasn't able
to match things exactly because of differences in the languages, but
the differences should be easy to absorb because they are natural to
Python.  For example, some methods that return mutliple values via
argument pointers in C++ will return a tuple of values in Python.
These differences have not been documented yet so if something isn't
working the same as described in the wxWindows documents the best
thing to do is to scan through the wxPython sources, especially the .i
files, as that is where the interfaces for wxPython are defined.

I have reports of successfully embedding wxPython within a wxWindows
C++ app on GTK.  It hasn't yet been attempted on MSW (to my knowledge)
so I don't know how successful such an attempt would be.  However it
is certainly possible.



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
have added notes within the class references about where wxPython
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



Build Instructions
------------------
I used SWIG (http://www.swig.org) to create the source code for the
extension module.  This enabled me to only have to deal with a small
amount of code and only have to bother with the exceptional issues.
SWIG takes care of the rest and generates all the repetative code for
me.  You don't need SWIG to build the extension module as all the
generated C++ code is included under the src directory.

I added a few minor features to SWIG to control some of the code
generation.  If you want to playaround with this the patches are in
wxPython/SWIG.patches and they should be applied to the 1.1p5 version
of SWIG.  These new patches are documented at
http://starship.skyport.net/crew/robind/python/#swig, and they should
also end up in the 1.2 version of SWIG.

wxPython is organized as a Python package.  This means that the
directory containing the results of the build process should be a
subdirectory of a directory on the PYTHONPATH.  (And preferably should
be named wxPython.)  You can control where the build process will dump
wxPython by setting the TARGETDIR makefile variable.  The default is
$(WXWIN)/utils/wxPython, where this README.txt is located.  If you
leave it here then you should add $(WXWIN)/utils to your PYTHONPATH.
However, you may prefer to use something that is already on your
PYTHONPATH, such as the site-packages directory on Unix systems.


Win32
-----

1. Build wxWindows with wxUSE_RESOURCE_LOADING_IN_MSW set to 1 in
include/wx/msw/setup.h so icons can be loaded dynamically.  While
there, make sure wxUSE_OWNER_DRAWN is also set to 1.

2. Change into the $(WXWIN)/utils/wxPython/src directory.

3. Edit makefile.vc and specify where your python installation is at.
You may also want to fiddle with the TARGETDIR variable as described
above.

4. Run nmake -f makefile.vc

5. If it builds successfully, congratulations!  Move on to the next
step.  If not then you can try mailing me for help.  Also, I will
always have a pre-built win32 version of this extension module at
http://starship.skyport.net/crew/robind/python.

6. Change to the $(WXWIN)/utils/wxPython/tests directory.

7. Try executing the test programs.  Note that some of these print
diagnositc or test info to standard output, so they will require the
console version of python.  For example:

    python test1.py

To run them without requiring a console, you can use the pythonw.exe
version of Python either from the command line or from a shortcut.



Unix
----

1. Change into the $(WXWIN)/utils/wxPython/src directory.

2. Edit Setup.in and ensure that the flags, directories, and toolkit
options are correct.  See the above commentary about TARGETDIR.  There
are a few sample Setup.in.[platform] files provided.

3. Run this command to generate a makefile:

    make -f Makefile.pre.in boot

4. Run these commands to build and then install the wxPython extension
module:

    make
    make install


5. Change to the $(WXWIN)/utils/wxPython/tests directory.

6. Try executing the test programs.  For example:

    python test1.py



------------------------
10/20/1998

Robin Dunn
robin@alldunn.com




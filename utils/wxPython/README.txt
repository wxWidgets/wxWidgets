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

So why would you wan to use wxPython over just C++ and wxWindows?
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
thing to do is to scan through the wxPython sources.

Currently this extension module is designed such that the entire
application will be written in Python.  I havn't tried it yet, but I
am sure that attempting to embed wxPython in a C++ wxWindows
application will cause problems.  However there is a plan to support
this in the future.



Build Instructions
------------------
I used SWIG (http://www.swig.org) to create the source code for the
extension module.  This enabled me to only have to deal with a small
amount of code and only have to bother with the exceptional issues.
SWIG takes care of the rest and generates all the repetative code for
me.  You don't need SWIG to build the extension module as all the
generated C++ code is included in the src directory.

wxPython is organized as a Python package.  This means that the
directory containing the results of the build process should be a
subdirectory of a directory on the PYTHONPATH.  (And preferably should
be named wxPython.)  You can control where the bulid process will dump
wxPython by setting the TARGETDIR makefile variable.  The default is
$(WXWIN)/utils/wxPython, where this README.txt is located.  If you
leave it here then you should add $(WXWIN)/utils to your PYTHONPATH.
However, you may prefer to use something that is already on your
PYTHONPATH, such as the site-packages directory on Unix systems.


Win32
-----

1. Build wxWindows with USE_RESOURCE_LOADING_IN_MSW set to 1 in
include/wx/msw/setup.h so icons can be loaded dynamically.

2. Change into the $(WXWIN)/utils/wxPython/src directory.

3. Edit makefile.nt and specify where your python installation is at.
You may also want to fiddle with the TARGETDIR variable as described
above.

4. Run nmake -f makefile.nt

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

NOTE:  I don't have wxPython working yet with wxGTK, so if you aren't
using Win32 you can skip the rest of this file and check back in a
week or so.


1. Change into the $(WXWIN)/utils/wxPython/src directory.

2. Edit Setup.in and ensure that the flags, directories, and toolkit
options are correct.  See the above commentary about TARGETDIR.

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
8/8/1998

Robin Dunn
robin@alldunn.com

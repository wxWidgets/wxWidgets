This zip file contains versions of the wxWindows and wxPython binaries
that have been compiled with __WXDEBUG__ defined.  This adds code to
wxWindows that is a bit more agressive about checking parameter
values, return values, and etc.  When the debugging library senses
something is wrong it will popup a message dialog telling you so.
Unfortunately the message is specific to the C++ code but it might
give you a hint about what went wrong and how to fix it.

Another debugging feature is when the wxPython program exits, it will
print to stdout information about any wxWindows C++ objects that
havn't been properly cleaned up.

This archive contains a new wxWindows DLL named wx[version]d.dll and a
debugging version of the core wxPython module, wxc_d.pyd.  These
should be put into your wxPython package directory.  Also included are
the debuging version of Python, python_d.exe and python[VER]_d.dll
which can be put wherever you like.

In order to run the debugging version of wxPython sumply run you
program with python_d.exe instead of python.exe.  This lets the
debugging version sit side by side with the production version, with
no need for swapping this around.  You will also need _d versions of
any other extension modules you are using.  If you need _d's for any
of the other standard Python extensions you can get them here, for 2.0
at least:

http://www.pythonlabs.com/products/python2.0/downloads/BeOpen-Python-2.0-Debug.zip


Robin

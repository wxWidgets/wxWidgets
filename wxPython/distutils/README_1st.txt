This is a copy of the Distutils package from Python (currently version
2.3a2.)  This newer copy of distutils is used for all versions of
Python to avoid some problems in the older versions that show up in
wxPython builds and to avoid having to make some ugly hacks in local
modules to work around them.

There is one little 1-line customization (hack) in msvccompiler.py
that allows the CFLAGS to be given on the RC.EXE command line.  This
is required so the wx.rc files can be found when it is #included. I've
submitted this patch to the Python project so if it gets into the main
Distutils distribution I can remove this code.  (However, the newer
version of distutils should still always be used, at least on Windows,
so it will need to wait until there it a Distutils distribution that
can be installed on the older Pythons.)

I have not yet applied any patches specifically for MSCV 7 yet.  So
far it appears that if you have the PATH setup properly (like I
usually do) that distutils works as is.


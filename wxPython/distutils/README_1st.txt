This is a copy of the Distutils package from Python (currently version
2.3.)  This newer copy of distutils is used for all versions of
Python to avoid some problems in the older versions that show up in
wxPython builds and to avoid having to make some ugly hacks in local
modules to work around them.

I have not yet applied any patches specifically for MSCV 7 yet.  So
far it appears that if you have the PATH setup properly (like I
usually do) that distutils works as is.



Some minor tweaks were made to pyexp.swg and typemaps.i to allow
wxPython to build with Python 1.6.  Just put these in your
swig_lib/python dir and you'll be all set.  These are from SWIG
1.1-883.


Additionally, there are some patches for SWIG sources that are needed
for wxPython:

    1. python.cxx.patch, applied to .../Modules/python.cxx file will
       prevent the out typemap from being used on constructors.

    2. cplus.cxx.patch, applied to .../SWIG/cplus.cxx will prevent
       some redundant code from being emitted to the extension
       module.

    3. pycpp.cxx.patch, applied to .../Modules/pycpp.cxx will add
       support for the addtomethod pragma for renamed constructors.



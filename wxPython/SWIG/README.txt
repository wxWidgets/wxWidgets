
Some minor tweaks were made to pyexp.swg and typemaps.i to allow
wxPython to build with Python 1.6.  Just put these in your
swig_lib/python dir and you'll be all set.  These are from SWIG
1.1-883.


Additionally, there is a patch in python.cxx.patch that should be
applied to SWIG's .../Modules/python.cxx file.  This patch prevents
the out typemap from being used on constructors.


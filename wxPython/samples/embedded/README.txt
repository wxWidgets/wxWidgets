This sample shows how to embed wxPython into a wxWidgets application.
There are a few little tricks needed to make it work, but once over
the hurdle it should work just fine for you.  I'll try to describe the
build issues here, see the code and comments in embedded.cpp for
examples of how to use it.

1. The most important thing is that your wx application and wxPython
   must use the same version and the same instance of wxWidgets.  That
   means that you can not statically link your app with wxWidgets, but
   must use a dynamic library for wxWidgets.

2. You must ensure that your app and wxPython are using the same
   wxWidgets DLL.  By default on MSW wxPython installs the wxWidgets
   DLL to a directory not on the PATH, so you may have to do something
   creative to make that happen.  But because of #3 this may not be
   that big of a problem.

3. wxPython, your app and wxWidgets must be built with the same flags
   and settings.  This probably means that you will need to rebuild
   wxPython yourself.  I do distribute the setup.h, other headers,
   import libs and etc. that I use, but you'll need to rebuild
   everything yourself anyway to get debugger versions so I'm not too
   worried about it just yet.  BTW, on MSW if you do debug builds of
   your app and wxPython then you will need to have a debug version of
   Python built too since it expects to have extension modules in
   files with a _d in the name.  If you do a hybrid build then you
   will be able to use the stock version of Python, but you won't be
   able to trace through the PYTHON API functions.

4. I expect that most of these issues will be much more minor on
   Unix.  ;-)


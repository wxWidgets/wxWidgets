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

This archive contains a new wxWindows DLL named wx[version]d.dll that
should be copied to the windows system directory or some other
directory on the PATH.You will also find replacements for all of
wxPython's *.pyd files that are linked to use this DLL.  Until I put
together a new installer, you'll need to copy things manually.


Robin

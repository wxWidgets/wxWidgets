This tarball contains all the files needed (I hope!) to allow you to
write win32 programs or Python extension modules that link to the same
wxWindows DLL that wxPython does.  Files for both the normal (ANSI)
and unicode builds are included.

You'll need to add the following directories to your Include path (the
/I flag for MSVC):

	wxPython-[version]\lib\mswdllh   [for ANSI builds]
	wxPython-[version]\lib\mswdlluh  [for Unicode builds]
	wxPython-[version]\include

And also you should link with one of the wxmsw*.lib files in the lib
dir, use the one with the 'u' in the name for the Unicode build and
the one without for the ANSI build.

I've also included some makefiles in the src dir to give you an idea
of what compiler and linker flags are required.  You can also get this
info from wxPython's setup.py script.

I'm sure I've forgotten something, so when you discover what it is
please let me know.  Also, if anyone feels like turning this text into
a more detailed HOW-TO please send your text to me.


Robin


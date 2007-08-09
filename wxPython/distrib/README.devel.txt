This tarball contains all the files needed (I hope!) to allow you to
write win32 programs or Python extension modules that link to the same
wxWidgets DLL that wxPython does.  Files for both the normal (ANSI)
and Unicode builds are included.

You'll need to add the following directories to your Include path (the
/I flag for MSVC):

	wxPython-[version]\lib\vc_dll\mswh   [for ANSI builds]
	wxPython-[version]\lib\vc_dll\mswuh  [for Unicode builds]
	wxPython-[version]\include

And also you should link with a set of the *.lib files in the
lib/vc_dll dir.  Use the ones with the 'u' in the name after the version
numbers for Unicode builds, the ones without the 'u' are for ANSI
builds.

I'm sure I've forgotten some files or something, so when you discover
what they are please let me know.  Also, if anyone feels like turning
this text into a more detailed HOW-TO please send your text to me.


Robin


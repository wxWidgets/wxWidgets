Setup files for creating an installation for PocketPC
=====================================================

To use these files, first compile the ARM version of
the Life! demo. Then run build.bat, which will copy the
executable to ARM_bins before compiling the CAB file,
copying the distribution files into the directory
Deliver.

It assumes you have the file setup.exe from:

http://www.pocketpcdn.com/articles/creatingsetup.html

or you can create your own. It simply runs CEAppMgr.exe
with your install.ini file, and CEAppMgr.exe will
install the program on the user's device.

See also the wxWinCE section of the wxWidgets reference
manual, which gives more tips and links for PocketPC
application development.


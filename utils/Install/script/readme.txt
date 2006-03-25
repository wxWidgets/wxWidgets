Ok, this is the first release of wxInstall, my wxWindows port of my
OS/2 installation program.  Everything is under the wxWindows license
except for the Infozip code.  

Current Status:

Archive support: ACE and ZIP.  

Tested platforms: Windows 2000, FreeBSD 4.0 (GTK)

Todo:

I need to write a directory browser, since the wxDirCtrl won't work.
I need to add Project support to the wxInstall builder.
Add platform specific code for modifying the startup environment.
--- There is some code for OS/2 (currently unsupported) and Windows ---

Bugs:

XPM's crash the installer on Windows.
ZIP archives crash on Windows.

Thanks!

You can reach me at dbsoft@technologist.com
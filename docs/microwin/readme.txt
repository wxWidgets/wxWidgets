wxMicroWindows port
===================

Julian Smart 2001-07-02

This is a snapshot of my experimental port of wxWindows to
MicroWindows. Widgets are supplied by the wxUniversal project,
while the underlying port uses the Windows ports with small
modifications for the MicroWindows API.

There are many things missing from MicroWindows that will
make the port quite limited for the time being. I haven't
worked out how to create bitmaps, though there is a BMP to C
converter. There are no menus, no common dialogs, and only
one WIN32 app may be run at a time.

Some things can no doubt be worked around, and/or MicroWindows
itself tweaked. Lots of wxWin functionality is just switched off or
commented out.

Installation
============

First install MicroWindows - untar it, change config to
use X11 and any other options you feel fit, apply
microwindows.patches to fix PeekMessage, and compile
(type 'make' from within the src directory).

Untar the wxMicroWindows port, and change the TOP variable at the
top of src/microwin/Makefile to reflect where MicroWindows is installed.
Type 'make all' from src/microwin. To clean, use cleanwx and NOT clean
since that will clean MicroWindows itself.

To make the sample, cd into samples/minimal, edit the TOP variable,
and type 'make all'.
Running 'minimal' runs the virtual MicroWindows desktop
and the minimal sample, since in a MicroWindows WIN32 application
they are one and the same binary.

Status
======

A frame comes up :-)

Notes
=====

No ::GetKeyState (see microwin/private.h). Should probably use GdOpenKeyboard/GdCloseKeyboard/GdReadKeyboard. Could perhaps emulate GetKeyState this way.

No ::CreateBitmap or BITMAPINFO. But BMPs can be converted
to C using convbmp, then need to use Gr... functions.

No ::DestroyIcon, ::DestroyCursor - use ::DestroyObject instead?
Also no LoadCursor, LoadImage. So how do we make cursors? No ::SetCursor.

wxDC: no ::GetTextColor, ::GetBkColor, ::IntersectClipRect,
::GetClipBox

No ::SetMenu, so no menus or menubars.

No ::GetObject so we can't get LOGFONT from an HFONT
in wxSystemSettings.

No ::CreateDialog so how do we create dialogs? Simulate
one with a frame I guess.

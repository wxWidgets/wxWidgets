wxMicroWindows port
===================

Julian Smart 2001-07-02

This is a port of wxWindows to MicroWindows, under Linux.
Widgets are supplied by the wxUniversal project, while the
underlying port uses the Windows ports with small modifications
for the MicroWindows API.

There are many things missing from MicroWindows that will
make the port quite limited for the time being. I haven't
worked out how to create bitmaps, though there is a BMP to C
converter. There are no common dialogs (we will use generic ones),
and only one WIN32 app may be run at a time.

Note that you can gain confidence in the WIN32/wxUniversal
combination by compiling wxUniversal under Windows using VC++,
using src/wxvc_universal.dsp. You can compile the minimal
and widgets samples in wxUniversal mode using the
UnivDebug and UnivRelease targets. Most of the code is shared
between this combination, and the wxMicroWindows port.

Installation
============

MicroWindows:

- unarchive MicroWindows 0.89pre7

- change 'config' to use X11 and any other options you feel fit.
  Suggestions for changes to the defaults:

  ERASEMOVE=N (otherwise moving windows will look messy)
  X11=Y
  OPTIMIZE=N
  DEBUG=Y
  VERBOSE=Y

- apply microwindows.patches (from wxWindows:
  docs/microwin/microwindows.patches) to fix PeekMessage
  and other issues

- compile by typing 'make' from within the MicroWindows src directory

wxMicroWindows:

- Download wxMSW 2.3.3 or greater, or get it from CVS

- Copy include/wx/msw/setup_microwin.h to include/wx/setup.h if
  include/wx/setup.h doesn't exist

- change the TOP variable at the top of src/msw/makefile.mic
  to reflect where MicroWindows is installed

- type 'make all' from src/msw. To clean, use cleanwx and NOT clean
  since that will clean MicroWindows itself

- to make the sample, cd into samples/minimal, edit the TOP variable,
  and type 'make all'

Running 'minimal' runs the virtual MicroWindows desktop
and the minimal sample, since in a MicroWindows WIN32 application
they are one and the same binary.

Status
======

The minimal sample is almost fully-functional, apart from minor
menu presentation issues (no borders, for example).

Implementation Notes
====================

wxMicroWindows is essentially the wxMSW port + wxUniversal
widgets. Lots of things in include/wx/univ/setup.h are switched
off to allow the port to compile. There are also #ifdefs
switching off further functionality, such as most wxBitmap
functions, pending proper implementation.

There are some WIN32 API functions not implemented by MicroWindows
that are instead stubbed out in include/wx/msw/microwin.c,
and 'implemented' in src/msw/microwin.c. Some of these functions
are important, some less so. They will need to be implemented
in due course. But implementing missing functionality in this way
is preferably to proliferating many #ifdefs in the
wxMSW/wxMicroWindows port itself.

Things missing from MicroWindows that need to be worked around
==============================================================

No ::GetKeyState (see include/wx/msw/private.h). Should probably use
GdOpenKeyboard/GdCloseKeyboard/GdReadKeyboard. Could perhaps emulate
GetKeyState this way.

No ::CreateBitmap or BITMAPINFO. But BMPs can be converted
to C using convbmp, then need to use Gr... functions.
We MUST implement creation from XPMs, since wxUniversal
makes use of XPMs, or else create our own bitmaps for
drawing radioboxes, checkboxes etc.: see renderers
in src/univ.

No ::DestroyIcon, ::DestroyCursor - use ::DestroyObject instead?
Also no LoadCursor, LoadImage. So how do we make cursors? No ::SetCursor.

wxDC: no ::GetTextColor, ::GetBkColor, ::IntersectClipRect,
::GetClipBox

No ::SetMenu, so no menus or menubars (now implemented by
wxUniversal).

No ::GetObject so we can't get LOGFONT from an HFONT
in wxSystemSettings (worked around by passing HFONT to
the wxFont constructor).


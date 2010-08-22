/////////////////////////////////////////////////////////////////////////////
// Name:        wx/uiaction.h
// Purpose:     wxUIActionSimulator interface
// Author:      Kevin Ollivier, Steven Lamerton, Vadim Zeitlin
// Modified by:
// Created:     2010-03-06
// RCS-ID:      $Id: menu.cpp 54129 2008-06-11 19:30:52Z SC $
// Copyright:   (c) Kevin Ollivier
//              (c) 2010 Steven Lamerton
//              (c) 2010 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UIACTIONSIMULATOR_H_
#define _WX_UIACTIONSIMULATOR_H_

#include "wx/defs.h"

#if wxUSE_UIACTIONSIMULATOR

#include "wx/mousestate.h"  // for wxMOUSE_BTN_XXX constants

class WXDLLIMPEXP_CORE wxUIActionSimulator
{
public:
    wxUIActionSimulator() { }


    // Default dtor, copy ctor and assignment operator are ok (even though the
    // last two don't make much sense for this class).


    // Mouse simulation
    // ----------------

    // Low level methods
    bool MouseMove(long x, long y);
    bool MouseMove(const wxPoint& point) { return MouseMove(point.x, point.y); }

    bool MouseDown(int button = wxMOUSE_BTN_LEFT);
    bool MouseUp(int button = wxMOUSE_BTN_LEFT);

    // Higher level interface, use it if possible instead
    bool MouseClick(int button = wxMOUSE_BTN_LEFT);
    bool MouseDblClick(int button = wxMOUSE_BTN_LEFT);
    bool MouseDragDrop(long x1, long y1, long x2, long y2,
                       int button = wxMOUSE_BTN_LEFT);


    // Keyboard simulation
    // -------------------

    // Low level methods for generating key presses and releases
    bool KeyDown(int keycode, int modifiers = wxMOD_NONE)
        { return Key(keycode, modifiers, true); }

    bool KeyUp(int keycode, int modifiers = wxMOD_NONE)
        { return Key(keycode, modifiers, false); }

    // Higher level methods for generating both the key press and release for a
    // single key or for all characters in the ASCII string "text" which can
    // currently contain letters only (no digits, no punctuation).
    bool Char(int keycode, int modifiers = wxMOD_NONE);

    bool Text(const char *text);

private:
    // This is the common part of Key{Down,Up}() methods: while we keep them
    // separate at public API level for consistency with Mouse{Down,Up}(), at
    // implementation level it makes more sense to have them in a single
    // function.
    //
    // This is a simple wrapper verifying the input parameters validity around
    // the platform-specific DoKey() method implemented in platform-specific
    // files.
    bool Key(int keycode, int modifiers, bool isDown);

    bool DoKey(int keycode, int modifiers, bool isDown);
};

#endif // wxUSE_UIACTIONSIMULATOR

#endif // _WX_UIACTIONSIMULATOR_H_

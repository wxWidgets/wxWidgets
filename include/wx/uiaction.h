/////////////////////////////////////////////////////////////////////////////
// Name:        wx/uiaction.h
// Purpose:     wxUIActionSimulator interface
// Author:      Kevin Ollivier, Steven Lamerton, Vadim Zeitlin
// Created:     2010-03-06
// Copyright:   (c) 2010 Kevin Ollivier
//              (c) 2010 Steven Lamerton
//              (c) 2010-2016 Vadim Zeitlin
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
    wxUIActionSimulator();
    ~wxUIActionSimulator();


    // Mouse simulation
    // ----------------

    // Low level methods
    bool MouseMove(long x, long y);
    bool MouseMove(const wxPoint& point) { return MouseMove(point.x, point.y); }

    bool MouseDown(int button = PrimaryMouseButton());
    bool MouseUp(int button = PrimaryMouseButton());

    // Higher level interface, use it if possible instead
    bool MouseClick(int button = PrimaryMouseButton());
    bool SecondaryMouseClick()
        { return MouseClick(SecondaryMouseButton()); }
    bool MouseDblClick(int button = PrimaryMouseButton());
    bool SecondaryMouseDblClick()
        { return MouseDblClick(SecondaryMouseButton()); }
    bool MouseDragDrop(long x1, long y1, long x2, long y2,
                       int button = PrimaryMouseButton());
    bool SecondaryDragDrop(long x1, long y1, long x2, long y2)
        { return MouseDragDrop(x1, y1, x2, y2, SecondaryMouseButton()); }
    bool MouseDragDrop(const wxPoint& p1, const wxPoint& p2,
                       int button = PrimaryMouseButton())
        { return MouseDragDrop(p1.x, p1.y, p2.x, p2.y, button); }
    bool SecondaryMouseDragDrop(const wxPoint& p1, const wxPoint& p2)
        { return MouseDragDrop(p1, p2, SecondaryMouseButton()); }

    // Keyboard simulation
    // -------------------

    // Low level methods for generating key presses and releases
    bool KeyDown(int keycode, int modifiers = wxMOD_NONE)
        { return Key(keycode, modifiers, true); }

    bool KeyUp(int keycode, int modifiers = wxMOD_NONE)
        { return Key(keycode, modifiers, false); }

    // Higher level methods for generating both the key press and release for a
    // single key or for all characters in the ASCII string "text" which can currently
    // contain letters, digits and characters for the definition of numbers [+-., ].
    bool Char(int keycode, int modifiers = wxMOD_NONE);

    bool Text(const char *text);

    // Select the item with the given text in the currently focused control.
    bool Select(const wxString& text);

    // Use these helpers for mouse input to respect primary/secondary button
    // assignment on systems configured with swapped mouse buttons.
    static int PrimaryMouseButton()
        { return MouseButtonsSwapped() ? wxMOUSE_BTN_RIGHT : wxMOUSE_BTN_LEFT; }
    static int SecondaryMouseButton()
        { return MouseButtonsSwapped() ? wxMOUSE_BTN_LEFT : wxMOUSE_BTN_RIGHT; }

private:
    static bool MouseButtonsSwapped()
        { return wxSystemSettings::GetMetric(wxSYS_SWAP_BUTTONS) != 0; }

    // This is the common part of Key{Down,Up}() methods: while we keep them
    // separate at public API level for consistency with Mouse{Down,Up}(), at
    // implementation level it makes more sense to have them in a single
    // function.
    //
    // It calls DoModifiers() to simulate pressing the modifier keys if
    // necessary and then DoKey() for the key itself.
    bool Key(int keycode, int modifiers, bool isDown);

    // Call DoKey() for all modifier keys whose bits are set in the parameter.
    void SimulateModifiers(int modifier, bool isDown);



    // This pointer is allocated in the ctor and points to the
    // platform-specific implementation.
    class wxUIActionSimulatorImpl* const m_impl;

    wxDECLARE_NO_COPY_CLASS(wxUIActionSimulator);
};

#endif // wxUSE_UIACTIONSIMULATOR

#endif // _WX_UIACTIONSIMULATOR_H_

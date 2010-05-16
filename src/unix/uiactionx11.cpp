/////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/uiactionx11.cpp
// Purpose:     wxUIActionSimulator implementation
// Author:      Kevin Ollivier
// Modified by:
// Created:     2010-03-06
// RCS-ID:      $Id: menu.cpp 54129 2008-06-11 19:30:52Z SC $
// Copyright:   (c) Kevin Ollivier
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/defs.h"

#if wxUSE_UIACTIONSIMULATOR

#include "wx/uiaction.h"
#include "wx/unix/utilsx11.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace
{

void SendButtonEvent(int button, bool isDown)
{
    int xbutton = 0;
    switch (button)
    {
        case wxMOUSE_BTN_LEFT:
            xbutton = 1;
            break;
        case wxMOUSE_BTN_RIGHT:
            xbutton = 2;
            break;
        case wxMOUSE_BTN_MIDDLE:
            xbutton = 3;
            break;
        default:
            wxFAIL_MSG("Unsupported button passed in.");
    }

    XEvent event;

    Display *display = XOpenDisplay(0);
    wxASSERT_MSG(display, "No display available!");

    memset(&event, 0x00, sizeof(event));

    if (isDown)
        event.type = ButtonPress;
    else
        event.type = ButtonRelease;

    event.xbutton.button = xbutton;
    event.xbutton.same_screen = True;

    XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
    event.xbutton.subwindow = event.xbutton.window;

    while (event.xbutton.subwindow)
    {
        event.xbutton.window = event.xbutton.subwindow;
        XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
    }

    XSendEvent(display, PointerWindow, True, 0xfff, &event);
    XFlush(display);
    XCloseDisplay(display);
}

void SendKey(Display *display, Window focus, KeyCode keycode, int type, int mask)
{
    XKeyEvent event;
    event.display = display;
    event.window = focus;
    event.root = RootWindow(event.display, DefaultScreen(event.display));
    event.subwindow = None;
    event.time = CurrentTime;
    event.x = 1;
    event.y = 1;
    event.x_root = 1;
    event.y_root = 1;
    event.same_screen = True;
    event.type = type;
    event.state = 0;
    event.keycode = keycode;

    XSendEvent(event.display, event.window, True, mask, (XEvent*) &event);
}

}

bool wxUIActionSimulator::MouseDown(int button)
{
    SendButtonEvent(button, true);
    return true;
}

bool wxUIActionSimulator::MouseMove(long x, long y)
{
    Display *display = XOpenDisplay(0);
    wxASSERT_MSG(display, "No display available!");
    Window root = DefaultRootWindow(display);
    XWarpPointer(display, None, root, 0, 0, 0, 0, x, y);
    XFlush(display);
    XCloseDisplay(display);
    return true;
}

bool wxUIActionSimulator::MouseUp(int button)
{
    SendButtonEvent(button, false);
    return true;
}

bool wxUIActionSimulator::Key(int keycode, bool isDown, int WXUNUSED(modifiers))
{
    Display *display = XOpenDisplay(0);
    wxASSERT_MSG(display, "No display available!");

    int mask, type;

    if (isDown) {
        type = KeyPress;
        mask = KeyPressMask;
    }
    else {
        type = KeyRelease;
        mask = KeyReleaseMask;
    }

    WXKeySym xkeysym = wxCharCodeWXToX(keycode);
    KeyCode xkeycode = XKeysymToKeycode(display, xkeysym);
    if (xkeycode == NoSymbol)
    {
        return false;
    }

    Window focus;
    int revert;
    XGetInputFocus(display, &focus, &revert);
    if (focus == None)
    {
        return false;
    }

    SendKey(display, focus, xkeycode, type, mask);

    XCloseDisplay(display);

    return true;
}

#endif // wxUSE_UIACTIONSIMULATOR

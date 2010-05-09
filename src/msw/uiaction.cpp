/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/uiaction.cpp
// Purpose:     wxUIActionSimulator implementation
// Author:      Kevin Ollivier
// Modified by:
// Created:     2010-03-06
// RCS-ID:      $Id: menu.cpp 54129 2008-06-11 19:30:52Z SC $
// Copyright:   (c) Kevin Ollivier, Steven Lamerton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_UIACTIONSIMULATOR

#include "wx/uiaction.h"

#include "wx/msw/wrapwin.h"

DWORD EventTypeForMouseButton(int button, bool isDown)
{
    switch (button)
    {
        case wxMOUSE_BTN_LEFT:
            if (isDown)
                return MOUSEEVENTF_LEFTDOWN;
            else
                return MOUSEEVENTF_LEFTUP;
        case wxMOUSE_BTN_RIGHT:
            if (isDown)
                return MOUSEEVENTF_RIGHTDOWN;
            else
                return MOUSEEVENTF_RIGHTUP;
        case wxMOUSE_BTN_MIDDLE:
            if (isDown)
                return MOUSEEVENTF_MIDDLEDOWN;
            else
                return MOUSEEVENTF_MIDDLEUP;

        default:
            wxFAIL_MSG("Unsupported button passed in.");
            return -1;
    }
}

bool wxUIActionSimulator::MouseDown(int button)
{
    POINT p;
    GetCursorPos(&p);
    mouse_event(EventTypeForMouseButton(button, true), p.x, p.y, 0, 0);
    return true;
}

bool wxUIActionSimulator::MouseMove(long x, long y)
{
    //Because MOUSEEVENTF_ABSOLUTE takes measurements scaled between 0 & 65535
    //we need to scale our input too
    int displayx, displayy, scaledx, scaledy;
    wxDisplaySize(&displayx, &displayy);
    scaledx = ((float)x / displayx) * 65535;
    scaledy = ((float)y / displayy) * 65535;
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, scaledx, scaledy, 0, 0);
    return true;
}

bool wxUIActionSimulator::MouseUp(int button)
{
    POINT p;
    GetCursorPos(&p);
    mouse_event(EventTypeForMouseButton(button, false), p.x, p.y, 0, 0);
    return true;
}

bool wxUIActionSimulator::Key(int keycode, bool isDown, int modifiers)
{
    if (modifiers & wxMOD_SHIFT)
        keybd_event(VK_SHIFT, 0, 0, 0);
    if (modifiers & wxMOD_ALT)
        keybd_event(VK_MENU, 0, 0, 0);
    if (modifiers & wxMOD_CMD)
        keybd_event(VK_CONTROL, 0, 0, 0);

    DWORD flags = 0;
    if (!isDown)
        flags = KEYEVENTF_KEYUP;
    keybd_event(keycode, 0, flags, 0);

    if (modifiers & wxMOD_SHIFT)
        keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
    if (modifiers & wxMOD_ALT)
        keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
    if (modifiers & wxMOD_CMD)
        keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);

    return true;
}

#endif // wxUSE_UIACTIONSIMULATOR

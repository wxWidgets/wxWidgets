/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/uiaction_osx.cpp
// Purpose:     wxUIActionSimulator implementation
// Author:      Kevin Ollivier, Steven Lamerton, Vadim Zeitlin
// Modified by:
// Created:     2010-03-06
// RCS-ID:      $Id: menu.cpp 54129 2008-06-11 19:30:52Z SC $
// Copyright:   (c) Kevin Ollivier
//              (c) 2010 Steven Lamerton
//              (c) 2010 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/defs.h"

#if wxUSE_UIACTIONSIMULATOR

#include "wx/uiaction.h"

#include "wx/log.h"

#include "wx/osx/private.h"
#include "wx/osx/core/cfref.h"

namespace
{

CGEventTapLocation tap = kCGSessionEventTap;

CGEventType CGEventTypeForMouseButton(int button, bool isDown)
{
    switch ( button )
    {
        case wxMOUSE_BTN_LEFT:
            return isDown ? kCGEventLeftMouseDown : kCGEventLeftMouseUp;

        case wxMOUSE_BTN_RIGHT:
            return isDown ? kCGEventRightMouseDown : kCGEventRightMouseUp;

        // All the other buttons use the constant OtherMouseDown but we still
        // want to check for invalid parameters so assert first
        default:
            wxFAIL_MSG("Unsupported button passed in.");
            // fall back to the only known remaining case

        case wxMOUSE_BTN_MIDDLE:
            return isDown ? kCGEventOtherMouseDown : kCGEventOtherMouseUp;
    }
}

CGPoint GetMousePosition()
{
    int x, y;
    wxGetMousePosition(&x, &y);

    CGPoint pos;
    pos.x = x;
    pos.y = y;

    return pos;
}

bool SendCharCode(CGKeyCode keycode, bool isDown)
{
    wxCFRef<CGEventRef>
        event(CGEventCreateKeyboardEvent(NULL, keycode, isDown));
    if ( !event )
        return false;

    CGEventPost(kCGHIDEventTap, event);
    return true;
}

CGKeyCode wxCharCodeWXToOSX(wxKeyCode code)
{
    CGKeyCode keycode;

    switch (code)
    {
        case 'a': case 'A':   keycode = kVK_ANSI_A; break;
        case 'b': case 'B':   keycode = kVK_ANSI_B; break;
        case 'c': case 'C':   keycode = kVK_ANSI_C; break;
        case 'd': case 'D':   keycode = kVK_ANSI_D; break;
        case 'e': case 'E':   keycode = kVK_ANSI_E; break;
        case 'f': case 'F':   keycode = kVK_ANSI_F; break;
        case 'g': case 'G':   keycode = kVK_ANSI_G; break;
        case 'h': case 'H':   keycode = kVK_ANSI_H; break;
        case 'i': case 'I':   keycode = kVK_ANSI_I; break;
        case 'j': case 'J':   keycode = kVK_ANSI_J; break;
        case 'k': case 'K':   keycode = kVK_ANSI_K; break;
        case 'l': case 'L':   keycode = kVK_ANSI_L; break;
        case 'm': case 'M':   keycode = kVK_ANSI_M; break;
        case 'n': case 'N':   keycode = kVK_ANSI_N; break;
        case 'o': case 'O':   keycode = kVK_ANSI_O; break;
        case 'p': case 'P':   keycode = kVK_ANSI_P; break;
        case 'q': case 'Q':   keycode = kVK_ANSI_Q; break;
        case 'r': case 'R':   keycode = kVK_ANSI_R; break;
        case 's': case 'S':   keycode = kVK_ANSI_S; break;
        case 't': case 'T':   keycode = kVK_ANSI_T; break;
        case 'u': case 'U':   keycode = kVK_ANSI_U; break;
        case 'v': case 'V':   keycode = kVK_ANSI_V; break;
        case 'w': case 'W':   keycode = kVK_ANSI_W; break;
        case 'x': case 'X':   keycode = kVK_ANSI_X; break;
        case 'y': case 'Y':   keycode = kVK_ANSI_Y; break;
        case 'z': case 'Z':   keycode = kVK_ANSI_Z; break;

        case '0':             keycode = kVK_ANSI_0; break;
        case '1':             keycode = kVK_ANSI_1; break;
        case '2':             keycode = kVK_ANSI_2; break;
        case '3':             keycode = kVK_ANSI_3; break;
        case '4':             keycode = kVK_ANSI_4; break;
        case '5':             keycode = kVK_ANSI_5; break;
        case '6':             keycode = kVK_ANSI_6; break;
        case '7':             keycode = kVK_ANSI_7; break;
        case '8':             keycode = kVK_ANSI_8; break;
        case '9':             keycode = kVK_ANSI_9; break;

        case WXK_BACK:        keycode = kVK_Delete; break;
        case WXK_TAB:         keycode = kVK_Tab; break;
        case WXK_RETURN:      keycode = kVK_Return; break;
        case WXK_ESCAPE:      keycode = kVK_Escape; break;
        case WXK_SPACE:       keycode = kVK_Space; break;
        case WXK_DELETE:      keycode = kVK_Delete; break;

        case WXK_SHIFT:       keycode = kVK_Shift; break;
        case WXK_ALT:         keycode = kVK_Option; break;
        case WXK_CONTROL:     keycode = kVK_Control; break;
        case WXK_COMMAND:     keycode = kVK_Command; break;

        case WXK_CAPITAL:     keycode = kVK_CapsLock; break;
        case WXK_END:         keycode = kVK_End; break;
        case WXK_HOME:        keycode = kVK_Home; break;
        case WXK_LEFT:        keycode = kVK_LeftArrow; break;
        case WXK_UP:          keycode = kVK_UpArrow; break;
        case WXK_RIGHT:       keycode = kVK_RightArrow; break;
        case WXK_DOWN:        keycode = kVK_DownArrow; break;

        case WXK_HELP:        keycode = kVK_Help; break;


        case WXK_NUMPAD0:     keycode = kVK_ANSI_Keypad0; break;
        case WXK_NUMPAD1:     keycode = kVK_ANSI_Keypad1; break;
        case WXK_NUMPAD2:     keycode = kVK_ANSI_Keypad2; break;
        case WXK_NUMPAD3:     keycode = kVK_ANSI_Keypad3; break;
        case WXK_NUMPAD4:     keycode = kVK_ANSI_Keypad4; break;
        case WXK_NUMPAD5:     keycode = kVK_ANSI_Keypad5; break;
        case WXK_NUMPAD6:     keycode = kVK_ANSI_Keypad6; break;
        case WXK_NUMPAD7:     keycode = kVK_ANSI_Keypad7; break;
        case WXK_NUMPAD8:     keycode = kVK_ANSI_Keypad8; break;
        case WXK_NUMPAD9:     keycode = kVK_ANSI_Keypad9; break;
        case WXK_F1:          keycode = kVK_F1; break;
        case WXK_F2:          keycode = kVK_F2; break;
        case WXK_F3:          keycode = kVK_F3; break;
        case WXK_F4:          keycode = kVK_F4; break;
        case WXK_F5:          keycode = kVK_F5; break;
        case WXK_F6:          keycode = kVK_F6; break;
        case WXK_F7:          keycode = kVK_F7; break;
        case WXK_F8:          keycode = kVK_F8; break;
        case WXK_F9:          keycode = kVK_F9; break;
        case WXK_F10:         keycode = kVK_F10; break;
        case WXK_F11:         keycode = kVK_F11; break;
        case WXK_F12:         keycode = kVK_F12; break;
        case WXK_F13:         keycode = kVK_F13; break;
        case WXK_F14:         keycode = kVK_F14; break;
        case WXK_F15:         keycode = kVK_F15; break;
        case WXK_F16:         keycode = kVK_F16; break;
        case WXK_F17:         keycode = kVK_F17; break;
        case WXK_F18:         keycode = kVK_F18; break;
        case WXK_F19:         keycode = kVK_F19; break;
        case WXK_F20:         keycode = kVK_F20; break;

        case WXK_PAGEUP:      keycode = kVK_PageUp; break;
        case WXK_PAGEDOWN:    keycode = kVK_PageDown; break;

        case WXK_NUMPAD_DELETE:    keycode = kVK_ANSI_KeypadClear; break;
        case WXK_NUMPAD_EQUAL:     keycode = kVK_ANSI_KeypadEquals; break;
        case WXK_NUMPAD_MULTIPLY:  keycode = kVK_ANSI_KeypadMultiply; break;
        case WXK_NUMPAD_ADD:       keycode = kVK_ANSI_KeypadPlus; break;
        case WXK_NUMPAD_SUBTRACT:  keycode = kVK_ANSI_KeypadMinus; break;
        case WXK_NUMPAD_DECIMAL:   keycode = kVK_ANSI_KeypadDecimal; break;
        case WXK_NUMPAD_DIVIDE:    keycode = kVK_ANSI_KeypadDivide; break;

        default: wxLogDebug( "Unrecognised keycode %d", code );
    }

    return keycode;
}

} // anonymous namespace

bool wxUIActionSimulator::MouseDown(int button)
{
    CGEventType type = CGEventTypeForMouseButton(button, true);
    wxCFRef<CGEventRef> event(
            CGEventCreateMouseEvent(NULL, type, GetMousePosition(), button));

    if ( !event )
        return false;

    CGEventSetType(event, type);
    CGEventPost(tap, event);

    return true;
}

bool wxUIActionSimulator::MouseMove(long x, long y)
{
    CGPoint pos;
    pos.x = x;
    pos.y = y;

    CGEventType type = kCGEventMouseMoved;
    wxCFRef<CGEventRef> event(
            CGEventCreateMouseEvent(NULL, type, pos, kCGMouseButtonLeft));

    if ( !event )
        return false;

    CGEventSetType(event, type);
    CGEventPost(tap, event);

    return true;
}

bool wxUIActionSimulator::MouseUp(int button)
{
    CGEventType type = CGEventTypeForMouseButton(button, false);
    wxCFRef<CGEventRef> event(
            CGEventCreateMouseEvent(NULL, type, GetMousePosition(), button));

    if ( !event )
        return false;

    CGEventSetType(event, type);
    CGEventPost(tap, event);

    return true;
}

bool wxUIActionSimulator::DoKey(int keycode, int modifiers, bool isDown)
{
    if (isDown)
    {
        if (modifiers & wxMOD_SHIFT)
            SendCharCode(kVK_Shift, true);
        if (modifiers & wxMOD_ALT)
            SendCharCode(kVK_Option, true);
        if (modifiers & wxMOD_CMD)
            SendCharCode(kVK_Command, true);
    }

    CGKeyCode cgcode = wxCharCodeWXToOSX((wxKeyCode)keycode);
    if ( !SendCharCode(cgcode, isDown) )
        return false;

    if(!isDown)
    {
        if (modifiers & wxMOD_SHIFT)
            SendCharCode(kVK_Shift, false);
        if (modifiers & wxMOD_ALT)
            SendCharCode(kVK_Option, false);
        if (modifiers & wxMOD_CMD)
            SendCharCode(kVK_Command, false);
    }

    return true;
}

#endif // wxUSE_UIACTIONSIMULATOR


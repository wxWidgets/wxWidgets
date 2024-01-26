/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/uiaction.cpp
// Purpose:     wxUIActionSimulator implementation
// Author:      Kevin Ollivier, Steven Lamerton, Vadim Zeitlin
// Created:     2010-03-06
// Copyright:   (c) Kevin Ollivier
//              (c) 2010 Steven Lamerton
//              (c) 2010 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_UIACTIONSIMULATOR

#ifndef WX_PRECOMP
    #include "wx/msw/private.h"             // For wxGetCursorPosMSW()
#endif

#include "wx/uiaction.h"
#include "wx/private/uiaction.h"

#include "wx/msw/wrapwin.h"

#include "wx/msw/private/keyboard.h"

#include "wx/math.h"

namespace
{

class wxUIActionSimulatorMSWImpl : public wxUIActionSimulatorImpl
{
public:
    // Returns a pointer to the global simulator object: as it's stateless, we
    // can reuse the same one without having to allocate it on the heap all the
    // time.
    static wxUIActionSimulatorMSWImpl* Get()
    {
        static wxUIActionSimulatorMSWImpl s_impl;
        return &s_impl;
    }

    virtual bool MouseMove(long x, long y) override;
    virtual bool MouseDown(int button = wxMOUSE_BTN_LEFT) override;
    virtual bool MouseUp(int button = wxMOUSE_BTN_LEFT) override;

    virtual bool DoKey(int keycode, int modifiers, bool isDown) override;

private:
    // This class has no public ctors, use Get() instead.
    wxUIActionSimulatorMSWImpl() { }

    wxDECLARE_NO_COPY_CLASS(wxUIActionSimulatorMSWImpl);
};

DWORD EventTypeForMouseButton(int button, bool isDown)
{
    switch (button)
    {
        case wxMOUSE_BTN_LEFT:
            return isDown ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;

        case wxMOUSE_BTN_RIGHT:
            return isDown ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;

        case wxMOUSE_BTN_MIDDLE:
            return isDown ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;

        default:
            wxFAIL_MSG("Unsupported button passed in.");
            return (DWORD)-1;
    }
}

} // anonymous namespace

bool wxUIActionSimulatorMSWImpl::MouseDown(int button)
{
    POINT p;
    wxGetCursorPosMSW(&p);
    mouse_event(EventTypeForMouseButton(button, true), p.x, p.y, 0, 0);
    return true;
}

bool wxUIActionSimulatorMSWImpl::MouseMove(long x, long y)
{
    // Because MOUSEEVENTF_ABSOLUTE takes measurements scaled between 0 & 65535
    // we need to scale our input too
    int displayx, displayy;
    wxDisplaySize(&displayx, &displayy);

    // Casts are safe because x and y are supposed to be less than the display
    // size, so there is no danger of overflow.
    DWORD scaledx = static_cast<DWORD>(ceil(x * 65535.0 / (displayx-1)));
    DWORD scaledy = static_cast<DWORD>(ceil(y * 65535.0 / (displayy-1)));
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, scaledx, scaledy, 0, 0);

    return true;
}

bool wxUIActionSimulatorMSWImpl::MouseUp(int button)
{
    POINT p;
    wxGetCursorPosMSW(&p);
    mouse_event(EventTypeForMouseButton(button, false), p.x, p.y, 0, 0);
    return true;
}

bool
wxUIActionSimulatorMSWImpl::DoKey(int keycode, int WXUNUSED(modifiers), bool isDown)
{
    bool isExtended;
    DWORD vkkeycode = wxMSWKeyboard::WXToVK(keycode, &isExtended);

    DWORD flags = 0;
    if ( isExtended )
        flags |= KEYEVENTF_EXTENDEDKEY;
    if ( !isDown )
        flags |= KEYEVENTF_KEYUP;

    keybd_event(vkkeycode, 0, flags, 0);

    return true;
}

wxUIActionSimulator::wxUIActionSimulator()
                   : m_impl(wxUIActionSimulatorMSWImpl::Get())
{
}

wxUIActionSimulator::~wxUIActionSimulator()
{
    // We can use a static wxUIActionSimulatorMSWImpl object because it's
    // stateless, so no need to delete it.
}

#endif // wxUSE_UIACTIONSIMULATOR

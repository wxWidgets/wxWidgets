/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/uiaction_osx.cpp
// Purpose:     wxUIActionSimulatorOSXImpl implementation
// Author:      Kevin Ollivier, Steven Lamerton, Vadim Zeitlin
// Modified by:
// Created:     2010-03-06
// Copyright:   (c) Kevin Ollivier
//              (c) 2010 Steven Lamerton
//              (c) 2010 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/object.h"
#endif

#if wxUSE_UIACTIONSIMULATOR

#include "wx/uiaction.h"
#include "wx/private/uiaction.h"

#include "wx/log.h"

#include "wx/osx/private.h"
#include "wx/osx/core/cfref.h"

#include "wx/evtloop.h"

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
            wxFALLTHROUGH;// fall back to the only known remaining case

        case wxMOUSE_BTN_MIDDLE:
            return isDown ? kCGEventOtherMouseDown : kCGEventOtherMouseUp;
    }
}
    
CGEventType CGEventTypeForMouseDrag(int button)
{
    switch ( button )
    {
        case wxMOUSE_BTN_LEFT:
            return kCGEventLeftMouseDragged;
            
        case wxMOUSE_BTN_RIGHT:
            return kCGEventRightMouseDragged;
            
            // All the other buttons use the constant OtherMouseDown but we still
            // want to check for invalid parameters so assert first
        default:
            wxFAIL_MSG("Unsupported button passed in.");
            wxFALLTHROUGH;// fall back to the only known remaining case
            
        case wxMOUSE_BTN_MIDDLE:
            return kCGEventOtherMouseDragged;
    }

}

CGMouseButton CGButtonForMouseButton(int button)
{
    switch ( button )
    {
        case wxMOUSE_BTN_LEFT:
            return kCGMouseButtonLeft;
            
        case wxMOUSE_BTN_RIGHT:
            return kCGMouseButtonRight;
            
            // All the other buttons use the constant OtherMouseDown but we still
            // want to check for invalid parameters so assert first
        default:
            wxFAIL_MSG("Unsupported button passed in.");
            wxFALLTHROUGH;// fall back to the only known remaining case
            
        case wxMOUSE_BTN_MIDDLE:
            return kCGMouseButtonCenter;
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

class wxUIActionSimulatorOSXImpl : public wxUIActionSimulatorImpl
{
public:
    // Returns a pointer to the global simulator object: as it's stateless, we
    // can reuse the same one without having to allocate it on the heap all the
    // time.
    static wxUIActionSimulatorOSXImpl* Get()
    {
        static wxUIActionSimulatorOSXImpl s_impl;
        return &s_impl;
    }

    virtual bool MouseMove(long x, long y) override;
    virtual bool MouseDown(int button = wxMOUSE_BTN_LEFT) override;
    virtual bool MouseUp(int button = wxMOUSE_BTN_LEFT) override;

    virtual bool MouseDblClick(int button = wxMOUSE_BTN_LEFT) override;
    virtual bool MouseDragDrop(long x1, long y1, long x2, long y2,
                               int button = wxMOUSE_BTN_LEFT) override;

    virtual bool DoKey(int keycode, int modifiers, bool isDown) override;

private:
    // This class has no public ctors, use Get() instead.
    wxUIActionSimulatorOSXImpl() { }

    const useconds_t delay_ = 10 * 1000;

    // give the system some time to process (it seems to need it)
    void wait_for_events()
    {
       ::usleep(delay_);
    }

    wxDECLARE_NO_COPY_CLASS(wxUIActionSimulatorOSXImpl);
};

} // anonymous namespace

bool wxUIActionSimulatorOSXImpl::MouseDown(int button)
{
    CGEventType type = CGEventTypeForMouseButton(button, true);
    wxCFRef<CGEventRef> event(
            CGEventCreateMouseEvent(nullptr, type, GetMousePosition(), CGButtonForMouseButton(button)));

    if ( !event )
        return false;

    CGEventSetType(event, type);
    CGEventPost(tap, event);
    wxCFEventLoop* loop = dynamic_cast<wxCFEventLoop*>(wxEventLoop::GetActive());
    if (loop)
        loop->SetShouldWaitForEvent(true);

    wait_for_events();
    
    return true;
}

bool wxUIActionSimulatorOSXImpl::MouseMove(long x, long y)
{
    CGPoint pos;
    pos.x = x;
    pos.y = y;

    CGEventType type = kCGEventMouseMoved;
    wxCFRef<CGEventRef> event(
            CGEventCreateMouseEvent(nullptr, type, pos, kCGMouseButtonLeft));

    if ( !event )
        return false;

    CGEventSetType(event, type);
    CGEventPost(tap, event);

    wxCFEventLoop* loop = dynamic_cast<wxCFEventLoop*>(wxEventLoop::GetActive());
    if (loop)
        loop->SetShouldWaitForEvent(true);

    wait_for_events();
    
    return true;
}

bool wxUIActionSimulatorOSXImpl::MouseUp(int button)
{
    CGEventType type = CGEventTypeForMouseButton(button, false);
    wxCFRef<CGEventRef> event(
            CGEventCreateMouseEvent(nullptr, type, GetMousePosition(), CGButtonForMouseButton(button)));

    if ( !event )
        return false;

    CGEventSetType(event, type);
    CGEventPost(tap, event);
    wxCFEventLoop* loop = dynamic_cast<wxCFEventLoop*>(wxEventLoop::GetActive());
    if (loop)
        loop->SetShouldWaitForEvent(true);

    wait_for_events();
    
    return true;
}

bool wxUIActionSimulatorOSXImpl::MouseDblClick(int button)
{
    CGEventType downtype = CGEventTypeForMouseButton(button, true);
    CGEventType uptype = CGEventTypeForMouseButton(button, false);
    wxCFRef<CGEventRef> event(
                              CGEventCreateMouseEvent(nullptr, downtype, GetMousePosition(), CGButtonForMouseButton(button)));
    
    if ( !event )
        return false;
    
    CGEventSetType(event,downtype);
    CGEventPost(tap, event);
    
    CGEventSetType(event, uptype);
    CGEventPost(tap, event);
    
    CGEventSetIntegerValueField(event, kCGMouseEventClickState, 2);
    CGEventSetType(event, downtype);
    CGEventPost(tap, event);
    
    CGEventSetType(event, uptype);
    CGEventPost(tap, event);
    wxCFEventLoop* loop = dynamic_cast<wxCFEventLoop*>(wxEventLoop::GetActive());
    if (loop)
        loop->SetShouldWaitForEvent(true);

    wait_for_events();
    
    return true;
}

bool wxUIActionSimulatorOSXImpl::MouseDragDrop(long x1, long y1, long x2, long y2,
                                        int button)
{
    CGPoint pos1,pos2;
    pos1.x = x1;
    pos1.y = y1;
    pos2.x = x2;
    pos2.y = y2;

    CGEventType downtype = CGEventTypeForMouseButton(button, true);
    CGEventType uptype = CGEventTypeForMouseButton(button, false);
    CGEventType dragtype = CGEventTypeForMouseDrag(button) ;

    wxCFRef<CGEventRef> event(
                              CGEventCreateMouseEvent(nullptr, kCGEventMouseMoved, pos1, CGButtonForMouseButton(button)));
    
    if ( !event )
        return false;
    
    CGEventSetType(event,kCGEventMouseMoved);
    CGEventPost(tap, event);
    
    CGEventSetType(event,downtype);
    CGEventPost(tap, event);
    
    
    CGEventSetType(event, dragtype);
    CGEventSetLocation(event,pos2);
    CGEventPost(tap, event);
    
    CGEventSetType(event, uptype);
    CGEventPost(tap, event);
    wxCFEventLoop* loop = dynamic_cast<wxCFEventLoop*>(wxEventLoop::GetActive());
    if (loop)
        loop->SetShouldWaitForEvent(true);
    

    wait_for_events();

    return true;
}

bool
wxUIActionSimulatorOSXImpl::DoKey(int keycode, int WXUNUSED(modifiers), bool isDown)
{
    CGKeyCode cgcode = wxCharCodeWXToOSX((wxKeyCode)keycode);

    wxCFRef<CGEventRef>
        event(CGEventCreateKeyboardEvent(nullptr, cgcode, isDown));
    if ( !event )
        return false;

    CGEventPost(kCGHIDEventTap, event);
    wxCFEventLoop* loop = dynamic_cast<wxCFEventLoop*>(wxEventLoop::GetActive());
    if (loop)
        loop->SetShouldWaitForEvent(true);

    wait_for_events();

    return true;
}

wxUIActionSimulator::wxUIActionSimulator()
                   : m_impl(wxUIActionSimulatorOSXImpl::Get())
{
}

wxUIActionSimulator::~wxUIActionSimulator()
{
    // We can use a static wxUIActionSimulatorOSXImpl object because it's
    // stateless, so no need to delete it.
}

#endif // wxUSE_UIACTIONSIMULATOR

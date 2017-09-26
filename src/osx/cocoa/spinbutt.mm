/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/spinbutt.mm
// Purpose:     wxSpinButton
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SPINBTN

#include "wx/spinbutt.h"
#include "wx/osx/private.h"

@interface wxNSStepper : NSStepper
{
}
@end

@implementation wxNSStepper

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXCocoaClassAddWXMethods(self);
    }
}

@end

class wxSpinButtonCocoaImpl : public wxWidgetCocoaImpl
{
public :
    wxSpinButtonCocoaImpl(wxWindowMac* peer , WXWidget w) :
        wxWidgetCocoaImpl(peer, w)
    {
        m_formerValue = 0;
        m_trackValue = false;
    }

    ~wxSpinButtonCocoaImpl()
    {
    }

    virtual void SetValue(wxInt32 v) wxOVERRIDE;
    virtual void SetMinimum(wxInt32 v) wxOVERRIDE;
    virtual void SetMaximum(wxInt32 v) wxOVERRIDE;
    virtual void controlAction(WXWidget slf, void* _cmd, void *sender) wxOVERRIDE;
    virtual void mouseEvent(WX_NSEvent event, WXWidget slf, void* _cmd) wxOVERRIDE;
private:
    int m_formerValue;
    bool m_trackValue;
};

void wxSpinButtonCocoaImpl::SetValue(wxInt32 v)
{
    [(NSStepper*)m_osxView setIntValue:v];
    if ( m_trackValue )
        m_formerValue = [(NSStepper*)m_osxView intValue];
}

void wxSpinButtonCocoaImpl::SetMinimum(wxInt32 v)
{
    [(NSStepper*)m_osxView setMinValue:(double)v];
    // Current value might be adjusted.
    if ( m_trackValue )
        m_formerValue = [(NSStepper*)m_osxView intValue];
}

void wxSpinButtonCocoaImpl::SetMaximum(wxInt32 v)
{
    [(NSStepper*)m_osxView setMaxValue:(double)v];
    // Current value might be adjusted.
    if ( m_trackValue )
        m_formerValue = [(NSStepper*)m_osxView intValue];
}

void wxSpinButtonCocoaImpl::mouseEvent(WX_NSEvent event, WXWidget slf, void *_cmd)
{

    // Save and track the current value which may be changed
    // in the mouse event handler
    if ( strcmp( sel_getName((SEL) _cmd) , "mouseDown:") == 0 )
    {
        m_formerValue = [(NSStepper*)m_osxView intValue];
        m_trackValue = true;
    }

    wxWidgetCocoaImpl::mouseEvent(event, slf, _cmd);
}

void wxSpinButtonCocoaImpl::controlAction( WXWidget WXUNUSED(slf), void *WXUNUSED(_cmd), void *WXUNUSED(sender))
{
    wxWindow* wxpeer = (wxWindow*) GetWXPeer();
    if ( wxpeer )
    {
        // because wx expects to be able to veto
        // a change we must revert the value change
        // and expose it
        int currentValue = [(NSStepper*)m_osxView intValue];
        [(NSStepper*)m_osxView setIntValue:m_formerValue];
        int inc = currentValue-m_formerValue;

        // adjust for wrap arounds
        if ( inc > 1 )
            inc = -1;
        else if (inc < -1 )
            inc = 1;

        if ( inc == 1 )
            wxpeer->TriggerScrollEvent(wxEVT_SCROLL_LINEUP);
        else if ( inc == -1 )
            wxpeer->TriggerScrollEvent(wxEVT_SCROLL_LINEDOWN);

        m_formerValue = [(NSStepper*)m_osxView intValue];
        m_trackValue = false;
    }
}

wxWidgetImplType* wxWidgetImpl::CreateSpinButton( wxWindowMac* wxpeer,
                                    wxWindowMac* WXUNUSED(parent),
                                    wxWindowID WXUNUSED(id),
                                    wxInt32 value,
                                    wxInt32 minimum,
                                    wxInt32 maximum,
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    long WXUNUSED(extraStyle))
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSStepper* v = [[wxNSStepper alloc] initWithFrame:r];

    [v setMinValue: minimum];
    [v setMaxValue: maximum];
    [v setIntValue: value];

    if ( style & wxSP_HORIZONTAL )
        [v rotateByAngle:90.0];

    BOOL wrap = NO;
    if ( style & wxSP_WRAP )
        wrap = YES;
    [v setValueWraps:wrap];
    
    wxWidgetCocoaImpl* c = new wxSpinButtonCocoaImpl( wxpeer, v );
    return c;
}

#endif // wxUSE_SPINBTN

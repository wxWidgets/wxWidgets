/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/scrolbar.mm
// Purpose:     wxScrollBar
// Author:      Stefan Csomor, Linas Valiukas
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/scrolbar.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/settings.h"
#endif

#include "wx/osx/private.h"


class wxScrollBarIPhoneImpl : public wxWidgetIPhoneImpl
{
public:
    wxScrollBarIPhoneImpl(wxWindowMac *parent) : wxWidgetIPhoneImpl( NULL, NULL )
    {
        m_maximum = 1;
        m_parent = parent;
        
        // Pretent the dummy "scrollbar" is a root control so window_osx.cpp
        // won't require actual native control to be present
        m_isRootControl = true;
    }
    
    void SetMaximum(wxInt32 v)
    {
        m_maximum = (v == 0) ? 1 : v;
        
        // Get a UIScrollView parent; tell it to redisplay itself
        if ( m_parent ) {
            wxWidgetIPhoneImpl *parentPeer = (wxWidgetIPhoneImpl *)(m_parent->GetPeer());
            if ( parentPeer ) {
                parentPeer->SetNeedsDisplay(NULL);
            }
        }
    }
    
    void SetScrollThumb( wxInt32 value, wxInt32 thumbSize )
    {
        double v = ((double) value)/m_maximum;
        double t = ((double) thumbSize)/(m_maximum+thumbSize);
#if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_5
        //[(wxNSScroller*) m_osxView setFloatValue:v knobProportion:t];
#else
        //[(wxNSScroller*) m_osxView setDoubleValue:v];
        //[(wxNSScroller*) m_osxView setKnobProportion:t];
#endif
    }
    
    virtual wxInt32 GetValue() const
    {
        //return wxRound([(wxNSScroller*) m_osxView floatValue] * m_maximum);
        return 0;
    }
    
    virtual wxInt32 GetMaximum() const
    {
        return m_maximum;
    }
    
    virtual void controlAction(WXWidget slf, void* _cmd, void *sender);
    virtual void mouseEvent(WX_NSEvent event, WXWidget slf, void* _cmd);
    
protected:
    wxInt32 m_maximum;
    
    // Parent is a wxWindow (UIScrollView) which contains this "scrollbar"
    wxWindowMac *m_parent;
};

// we will have a mouseDown, then in the native
// implementation of mouseDown the tracking code
// is calling clickedAction, therefore we wire this
// to thumbtrack and only after super mouseDown
// returns we will call the thumbrelease

void wxScrollBarIPhoneImpl::controlAction( WXWidget WXUNUSED(slf), void *WXUNUSED(_cmd), void *WXUNUSED(sender))
{
    /*
    wxEventType scrollEvent = wxEVT_NULL;
    switch ([(NSScroller*)m_osxView hitPart])
    {
        case NSScrollerIncrementLine:
            scrollEvent = wxEVT_SCROLL_LINEDOWN;
            break;
        case NSScrollerIncrementPage:
            scrollEvent = wxEVT_SCROLL_PAGEDOWN;
            break;
        case NSScrollerDecrementLine:
            scrollEvent = wxEVT_SCROLL_LINEUP;
            break;
        case NSScrollerDecrementPage:
            scrollEvent = wxEVT_SCROLL_PAGEUP;
            break;
        case NSScrollerKnob:
        case NSScrollerKnobSlot:
            scrollEvent = wxEVT_SCROLL_THUMBTRACK;
            break;
        case NSScrollerNoPart:
        default:
            return;
    }
    
    wxWindow* wxpeer = (wxWindow*) GetWXPeer();
    if ( wxpeer )
        wxpeer->TriggerScrollEvent(scrollEvent);
    */
}

void wxScrollBarIPhoneImpl::mouseEvent(WX_NSEvent event, WXWidget slf, void *_cmd)
{
    /*
    wxWidgetCocoaImpl::mouseEvent(event, slf, _cmd);
    
    // send a release event in case we've been tracking the thumb
    if ( strcmp( sel_getName((SEL) _cmd) , "mouseDown:") == 0 )
    {
        NSScrollerPart hit = [(NSScroller*)m_osxView hitPart];
        if ( (hit == NSScrollerKnob || hit == NSScrollerKnobSlot) )
        {
            wxWindow* wxpeer = (wxWindow*) GetWXPeer();
            if ( wxpeer )
                wxpeer->OSXHandleClicked(0);
        }
    }
    */
}

wxWidgetImplType* wxWidgetImpl::CreateScrollBar(wxWindowMac* WXUNUSED(wxpeer),
                                                wxWindowMac* parent,
                                                wxWindowID WXUNUSED(id),
                                                const wxPoint& WXUNUSED(pos),
                                                const wxSize& WXUNUSED(size),
                                                long WXUNUSED(style),
                                                long WXUNUSED(extraStyle))
{    
    // Dummy scrollbar implementation; keep track of parent
    return new wxScrollBarIPhoneImpl(parent);
}

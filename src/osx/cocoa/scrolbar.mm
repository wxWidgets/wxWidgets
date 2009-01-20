/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/scrolbar.mm
// Purpose:     wxScrollBar
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: scrolbar.cpp 54129 2008-06-11 19:30:52Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/scrolbar.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/settings.h"
#endif

#include "wx/osx/private.h"

@interface wxNSScroller : NSScroller
{
    WXCOCOAIMPL_COMMON_MEMBERS
}

WXCOCOAIMPL_COMMON_INTERFACE

 - (void) clickedAction: (id) sender;

@end

@implementation wxNSScroller

- (id)initWithFrame:(NSRect)frame
{
    [super initWithFrame:frame];
    impl = NULL;
    [self setTarget: self];
    [self setAction: @selector(clickedAction:)];
    return self;
}

WXCOCOAIMPL_COMMON_IMPLEMENTATION_NO_MOUSEDOWN

// we will have a mouseDown, then in the native 
// implementation of mouseDown the tracking code
// is calling clickedAction, therefore we wire this
// to thumbtrack and only after super mouseDown 
// returns we will call the thumbrelease

- (void) clickedAction: (id) sender
{
    if ( impl )
    {
        wxEventType scrollEvent = wxEVT_NULL;
        switch ([self hitPart]) 
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

        wxWindow* wxpeer = (wxWindow*) impl->GetWXPeer();
        if ( wxpeer )
            wxpeer->TriggerScrollEvent(scrollEvent);
    }
}

-(void)mouseDown:(NSEvent *)event 
{
    if ( !impl->DoHandleMouseEvent(event) )
        [super mouseDown:event];

    // send a release event in case we've been tracking the thumb
    NSScrollerPart hit = [self hitPart];
    if ( impl && (hit == NSScrollerKnob || hit == NSScrollerKnobSlot) )
    {
        wxWindow* wxpeer = (wxWindow*) impl->GetWXPeer();
        if ( wxpeer )
            wxpeer->TriggerScrollEvent(wxEVT_SCROLL_THUMBRELEASE);
    }
}

@end

class wxOSXScrollBarCocoaImpl : public wxWidgetCocoaImpl
{
public :
    wxOSXScrollBarCocoaImpl( wxWindowMac* peer, WXWidget w) : wxWidgetCocoaImpl( peer, w )
    {
        m_maximum = 1;
    }
    
    void SetMaximum(wxInt32 v)
    {
        m_maximum = (v == 0) ? 1 : v;
    }
    
    void    SetScrollThumb( wxInt32 value, wxInt32 thumbSize ) 
    {
        double v = ((double) value)/m_maximum;
        double t = ((double) thumbSize)/m_maximum;
#if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_5
        [(wxNSScroller*) m_osxView setFloatValue:v knobProportion:t];
#else
        [(wxNSScroller*) m_osxView setDoubleValue:v];
        [(wxNSScroller*) m_osxView setKnobProportion:t];
#endif
    }
    
    wxInt32 GetValue() const
    {
        return [(wxNSScroller*) m_osxView floatValue] * m_maximum;
    }
    
    wxInt32 GetMaximum() const
    {
        return m_maximum;
    }
protected:
    wxInt32 m_maximum;
};

wxWidgetImplType* wxWidgetImpl::CreateScrollBar( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle)
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSScroller* v = [[wxNSScroller alloc] initWithFrame:r];

    wxWidgetCocoaImpl* c = new wxOSXScrollBarCocoaImpl( wxpeer, v );
    [v setImplementation:c];
    [v setEnabled:YES];
    return c;
}

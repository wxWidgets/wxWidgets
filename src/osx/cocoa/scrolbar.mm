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
    wxWidgetImpl* impl;
}

- (void)setImplementation: (wxWidgetImpl *) theImplementation;
- (wxWidgetImpl*) implementation;
- (BOOL) isFlipped;
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

- (void) clickedAction: (id) sender
{
    if ( impl )
    {
        wxWindow* wxpeer = (wxWindow*) impl->GetWXPeer();
        if ( wxpeer )
            wxpeer->HandleClicked(0);
    }
}

- (void)setImplementation: (wxWidgetImpl *) theImplementation
{
    impl = theImplementation;
}

- (wxWidgetImpl*) implementation
{
    return impl ;
}

- (BOOL) isFlipped
{
    return YES;
}

@end

class wxOSXScrollBarCocoaImpl : public wxWidgetCocoaImpl
{
public :
    wxOSXScrollBarCocoaImpl( wxWindowMac* peer, WXWidget w) : wxWidgetCocoaImpl( peer, w )
    {
    }
    
    void SetMaximum(wxInt32 v)
    {
        m_maximum = v;
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
    return c;
}

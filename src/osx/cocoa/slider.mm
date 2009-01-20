/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/slider.mm
// Purpose:     wxSlider
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: slider.cpp 54129 2008-06-11 19:30:52Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SLIDER

#include "wx/slider.h"
#include "wx/osx/private.h"

@interface wxNSSlider : NSSlider
{
    WXCOCOAIMPL_COMMON_MEMBERS
}

WXCOCOAIMPL_COMMON_INTERFACE

 - (void) clickedAction: (id) sender;

@end

@implementation wxNSSlider

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
        wxWindow* wxpeer = (wxWindow*) impl->GetWXPeer();
        if ( wxpeer )
            wxpeer->TriggerScrollEvent(wxEVT_SCROLL_THUMBTRACK);
    }
}

-(void)mouseDown:(NSEvent *)event 
{
    if ( !impl->DoHandleMouseEvent(event) )
        [super mouseDown:event];

    if ( impl )
    {
        wxWindow* wxpeer = (wxWindow*) impl->GetWXPeer();
        if ( wxpeer )
            wxpeer->HandleClicked(0);
    }
}

@end

wxWidgetImplType* wxWidgetImpl::CreateSlider( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    wxInt32 value,
                                    wxInt32 minimum,
                                    wxInt32 maximum,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle)
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSSlider* v = [[wxNSSlider alloc] initWithFrame:r];

    int tickMarks = 0;
    if ( style & wxSL_AUTOTICKS )
    {
        tickMarks = (maximum - minimum) + 1; // +1 for the 0 value

        // keep the number of tickmarks from becoming unwieldly, therefore below it is ok to cast
        // it to a UInt16
        while (tickMarks > 20)
            tickMarks /= 5;
            
        [v setNumberOfTickMarks:tickMarks];
        [v setTickMarkPosition:NSTickMarkBelow];
    }

    [v setMinValue: minimum];
    [v setMaxValue: maximum];
    [v setFloatValue: (double) value];
    wxWidgetCocoaImpl* c = new wxWidgetCocoaImpl( wxpeer, v );
    [v setImplementation:c];
    return c;
}

#endif // wxUSE_SLIDER

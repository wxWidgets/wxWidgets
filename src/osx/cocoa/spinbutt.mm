/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.cpp
// Purpose:     wxSpinButton
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: spinbutt.cpp 54129 2008-06-11 19:30:52Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SPINBTN

#include "wx/spinbutt.h"
#include "wx/osx/private.h"

@interface wxNSStepper : NSStepper
{
    WXCOCOAIMPL_COMMON_MEMBERS
    int formerValue;
}

WXCOCOAIMPL_COMMON_INTERFACE

 - (void) clickedAction: (id) sender;

@end

@implementation wxNSStepper

- (id)initWithFrame:(NSRect)frame
{
    [super initWithFrame:frame];
    impl = NULL;
    formerValue = 0;
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
        {
            // because wx expects to be able to veto 
            // a change we must revert the value change
            // and expose it
            int currentValue = [self intValue];
            [self setIntValue:formerValue];
            int inc = currentValue-formerValue;
            
            // adjust for wrap arounds
            if ( inc > 1 )
                inc = -1;
            else if (inc < -1 )
                inc = 1;
                
            if ( inc == 1 )
                wxpeer->TriggerScrollEvent(wxEVT_SCROLL_LINEUP);
            else if ( inc == -1 )
                wxpeer->TriggerScrollEvent(wxEVT_SCROLL_LINEDOWN);

            formerValue = [self intValue];
        }
    }
}

-(void)mouseDown:(NSEvent *)event 
{
    formerValue = [self intValue];
    if ( !impl->DoHandleMouseEvent(event) )
        [super mouseDown:event];
}

WXCOCOAIMPL_COMMON_IMPLEMENTATION_NO_MOUSEDOWN

@end

wxWidgetImplType* wxWidgetImpl::CreateSpinButton( wxWindowMac* wxpeer, 
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
    wxNSStepper* v = [[wxNSStepper alloc] initWithFrame:r];

    [v setMinValue: minimum];
    [v setMaxValue: maximum];
    [v setIntValue: value];
    
    if ( style & wxSP_WRAP )
        [v setValueWraps:YES];
    
    wxWidgetCocoaImpl* c = new wxWidgetCocoaImpl( wxpeer, v );
    [v setImplementation:c];
    return c;
}

#endif // wxUSE_SPINBTN

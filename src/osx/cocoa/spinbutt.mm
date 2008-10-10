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
    wxWidgetImpl* impl;
}

- (void)setImplementation: (wxWidgetImpl *) theImplementation;
- (wxWidgetImpl*) implementation;
- (BOOL) isFlipped;
 - (void) clickedAction: (id) sender;

@end

@implementation wxNSStepper

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
    return impl;
}

- (BOOL) isFlipped
{
    return YES;
}

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

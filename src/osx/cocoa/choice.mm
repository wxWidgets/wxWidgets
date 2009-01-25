/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/choice.mm
// Purpose:     wxChoice
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: choice.cpp 54129 2008-06-11 19:30:52Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_CHOICE

#include "wx/choice.h"

#ifndef WX_PRECOMP
    #include "wx/menu.h"
    #include "wx/dcclient.h"
#endif

#include "wx/osx/private.h"

@interface wxNSPopUpButton : NSPopUpButton
{
    WXCOCOAIMPL_COMMON_MEMBERS
}

WXCOCOAIMPL_COMMON_INTERFACE

- (void) clickedAction: (id) sender;

@end

@implementation wxNSPopUpButton

- (id)initWithFrame:(NSRect)frame pullsDown:(BOOL) pd
{
    [super initWithFrame:frame pullsDown:pd];
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
            wxpeer->OSXHandleClicked(0);
    }
}

WXCOCOAIMPL_COMMON_IMPLEMENTATION

- (int) intValue
{
   return [self indexOfSelectedItem];
}

- (void) setIntValue: (int) v
{
    [self selectItemAtIndex:v];
}

@end

wxWidgetImplType* wxWidgetImpl::CreateChoice( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    wxMenu* menu,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStylew)
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSPopUpButton* v = [[wxNSPopUpButton alloc] initWithFrame:r pullsDown:NO];
    [v setMenu: menu->GetHMenu()];
    wxWidgetCocoaImpl* c = new wxWidgetCocoaImpl( wxpeer, v );
    [v setImplementation:c];
    return c;
}

#endif // wxUSE_CHOICE

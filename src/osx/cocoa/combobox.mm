/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/combobox.mm
// Purpose:     wxChoice
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: combobox.mm 54129 2008-06-11 19:30:52Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_COMBOBOX

#include "wx/combobox.h"

#ifndef WX_PRECOMP
    #include "wx/menu.h"
    #include "wx/dcclient.h"
#endif

#include "wx/osx/private.h"

// work in progress

@interface wxNSComboBox : NSComboBox
{
}

@end

@implementation wxNSComboBox

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized) 
    {
        initialized = YES;
        wxOSXCocoaClassAddWXMethods( self );
    }
}

- (int) intValue
{
   return [self indexOfSelectedItem];
}

- (void) setIntValue: (int) v
{
    [self selectItemAtIndex:v];
}

@end

wxWidgetImplType* wxWidgetImpl::CreateComboBox( wxWindowMac* wxpeer, 
                                    wxWindowMac* WXUNUSED(parent), 
                                    wxWindowID WXUNUSED(id), 
                                    wxMenu* menu,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long WXUNUSED(style), 
                                    long WXUNUSED(extraStyle))
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSComboBox* v = [[wxNSComboBox alloc] initWithFrame:r pullsDown:NO];
    wxWidgetCocoaImpl* c = new wxWidgetCocoaImpl( wxpeer, v );
    return c;
}

#endif // wxUSE_CHOICE

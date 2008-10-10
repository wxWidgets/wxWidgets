/////////////////////////////////////////////////////////////////////////////
// Name:        statbox.cpp
// Purpose:     wxStaticBox
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: statbox.cpp 54129 2008-06-11 19:30:52Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_STATBOX

#include "wx/statbox.h"
#include "wx/osx/private.h"

@implementation wxNSBox

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
    return NO;
}

@end

wxWidgetImplType* wxWidgetImpl::CreateGroupBox( wxWindowMac* wxpeer, 
                                    wxWindowMac* parent, 
                                    wxWindowID id, 
                                    const wxString& label,
                                    const wxPoint& pos, 
                                    const wxSize& size,
                                    long style, 
                                    long extraStyle)
{
    NSRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxNSBox* v = [[wxNSBox alloc] initWithFrame:r];
    wxWidgetCocoaImpl* c = new wxWidgetCocoaImpl( wxpeer, v );
    [v setImplementation:c];
    return c;
}

#endif // wxUSE_STATBOX


/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/checkbox.mm
// Purpose:     wxCheckBox
// Author:      Stefan Csomor
// Modified by:
// Created:     2008-08-20
// RCS-ID:      $Id: checkbox.mm 54129 2008-06-11 19:30:52Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_CHECKBOX

#include "wx/checkbox.h"
#include "wx/osx/private.h"

@interface wxUISwitch : UISwitch
{
}

@end

@implementation wxUISwitch

+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized)
    {
        initialized = YES;
        wxOSXIPhoneClassAddWXMethods( self );
    }
}

- (int) intValue
{
    return [self isOn] ? 1 : 0;
}

- (void) setIntValue: (int) v
{
    [self setOn:v != 0 animated:NO];
}

@end

wxWidgetImplType* wxWidgetImpl::CreateCheckBox( wxWindowMac* wxpeer,
                                    wxWindowMac* WXUNUSED(parent),
                                    wxWindowID WXUNUSED(id),
                                    const wxString& WXUNUSED(label),
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    long WXUNUSED(extraStyle))
{
    CGRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    wxUISwitch* v = [[wxUISwitch alloc] initWithFrame:r];

//    if (style & wxCHK_3STATE)
//        [v setAllowsMixedState:YES];

    wxWidgetIPhoneImpl* c = new wxWidgetIPhoneImpl( wxpeer, v );
    return c;
}

#endif

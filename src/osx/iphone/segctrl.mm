/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/segctrl.mm
// Purpose:     wxMoSegmentedCtrl
// Author:      Linas Valiukas
// Modified by:
// Created:     2011-07-04
// RCS-ID:      $Id$
// Copyright:   (c) Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/app.h"
#include "wx/utils.h"
#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/settings.h"
#endif // WX_PRECOMP

#include "wx/osx/private.h"
#include "wx/osx/iphone/private/segctrlimpl.h"


@implementation wxUISegmentedControl


@end


wxSegmentedCtrlIPhoneImpl::wxSegmentedCtrlIPhoneImpl( wxWindowMac* peer , WXWidget w ) : wxWidgetIPhoneImpl(peer, w)
{
    
}    


wxWidgetImplType* wxWidgetImpl::CreateSegmentedCtrl(wxWindowMac* wxpeer,
                                                    wxWindowMac* WXUNUSED(parent),
                                                    wxWindowID WXUNUSED(id),
                                                    const wxPoint& pos,
                                                    const wxSize& size,
                                                    long style,
                                                    long WXUNUSED(extraStyle))
{
    CGRect r = wxOSXGetFrameForControl( wxpeer, pos , size ) ;
    
    wxUISegmentedControl* v = [[wxUISegmentedControl alloc] init];
    
    wxWidgetIPhoneImpl* c = new wxSegmentedCtrlIPhoneImpl( wxpeer, v );
    return c;
}

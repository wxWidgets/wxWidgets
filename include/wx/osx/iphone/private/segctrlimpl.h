/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/private/segctrlimpl.h
// Purpose:     wxMoSegmentedCtrl implementation classes that have to be exposed
// Author:      Linas Valiukas
// Modified by:
// Created:     2011-07-04
// RCS-ID:      $Id$
// Copyright:   (c) Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_IPHONE_PRIVATE_SEGCTRL_H_
#define _WX_OSX_IPHONE_PRIVATE_SEGCTRL_H_

#include "wx/segctrl.h"
#include "wx/osx/private.h"


#pragma mark Cocoa

@interface wxUISegmentedControl : UISegmentedControl
{
    wxSegmentedCtrl *moSegmentedCtrl;
}

- (id)initWithWxSegmentedCtrl:(wxSegmentedCtrl *)initWxSegmentedCtrl;
- (void)segmentHasBeenChanged:(id)sender;

@end


class wxSegmentedCtrlIPhoneImpl : public wxWidgetIPhoneImpl
{
public:
    wxSegmentedCtrlIPhoneImpl( wxWindowMac* peer , WXWidget w );
};

#endif // _WX_OSX_IPHONE_PRIVATE_SEGCTRL_H_

/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/NSControl.cpp
// Purpose:     wxCocoaNSControl
// Author:      David Elliott
// Modified by:
// Created:     2003/02/15
// RCS-ID:      $Id: 
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/cocoa/NSControl.h"

#import <Foundation/NSObject.h>

// ============================================================================
// @class wxNSControlTarget
// ============================================================================
@interface wxNSControlTarget : NSObject
{
}

- (void)wxNSControlAction: (id)sender;
@end //interface wxNSControlTarget

@implementation wxNSControlTarget : NSObject

- (void)wxNSControlAction: (id)sender
{
    wxLogTrace(wxTRACE_COCOA,wxT("wxNSControlAction"));
    wxCocoaNSControl *wxcontrol = wxCocoaNSControl::GetFromCocoa(sender);
    wxCHECK_RET(wxcontrol,wxT("wxNSControlAction received but no wxCocoaNSControl exists!"));
    wxcontrol->CocoaTarget_action();
}

@end //implementation wxNSControlTarget

// ============================================================================
// wxNSControl
// ============================================================================
WX_IMPLEMENT_OBJC_INTERFACE(NSControl)

struct objc_object *wxCocoaNSControl::sm_cocoaTarget = [[wxNSControlTarget alloc] init];


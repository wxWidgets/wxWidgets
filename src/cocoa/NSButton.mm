/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/NSButton.cpp
// Purpose:     wxCocoaNSButton
// Author:      David Elliott
// Modified by:
// Created:     2003/01/31
// RCS-ID:      $Id: 
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWindows license
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

#include "wx/cocoa/ObjcPose.h"

#include "wx/cocoa/NSButton.h"
#import <AppKit/NSButton.h>

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------
WX_IMPLEMENT_OBJC_INTERFACE_HASHMAP(NSButton)

// ============================================================================
// @class wxPoserNSButton
// ============================================================================
@interface wxPoserNSButton : NSButton
{
}

- (void)wxNSButtonAction: (id)sender;
@end // wxPoserNSButton

WX_IMPLEMENT_POSER(wxPoserNSButton);

@implementation wxPoserNSButton :  NSButton
- (void)wxNSButtonAction: (id)sender
{
    wxASSERT_MSG(self==sender,"Received wxNSButtonAction from another object");
    wxCocoaNSButton *button = wxCocoaNSButton::GetFromCocoa(self);
    wxCHECK_RET(button,"wxNSButtonAction received without associated wx object");
    button->Cocoa_wxNSButtonAction();
}

@end // implementation wxPoserNSButton

void wxCocoaNSButton::AssociateNSButton(WX_NSButton cocoaNSButton)
{
    if(cocoaNSButton)
    {
        sm_cocoaHash.insert(wxCocoaNSButtonHash::value_type(cocoaNSButton,this));
        [cocoaNSButton setTarget: cocoaNSButton];
        [cocoaNSButton setAction: @selector(wxNSButtonAction:)];
    }
}


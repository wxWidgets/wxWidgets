/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/NSWindow.mm
// Purpose:     wxCocoaNSWindow
// Author:      David Elliott
// Modified by:
// Created:     2003/03/16
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
    #include "wx/menuitem.h"
#endif // WX_PRECOMP

#include "wx/cocoa/ObjcPose.h"
#include "wx/cocoa/NSWindow.h"

#import <Appkit/NSWindow.h>

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------
WX_IMPLEMENT_OBJC_INTERFACE_HASHMAP(NSWindow)

void wxCocoaNSWindow::AssociateNSWindow(WX_NSWindow cocoaNSWindow)
{
    [cocoaNSWindow setReleasedWhenClosed: NO];
    sm_cocoaHash.insert(wxCocoaNSWindowHash::value_type(cocoaNSWindow,this));
}

// ============================================================================
// @class wxPoserNSWindow
// ============================================================================
@interface wxPoserNSWindow : NSWindow
{
}

- (void)close;
- (BOOL)windowShouldClose: (id)sender;

- (BOOL)wxMenuItemAction: (id)sender;
@end // wxPoserNSwindow

WX_IMPLEMENT_POSER(wxPoserNSWindow);
@implementation wxPoserNSWindow : NSWindow

- (void)close
{
    wxLogDebug("close");
    wxCocoaNSWindow *tlw = wxCocoaNSWindow::GetFromCocoa(self);
    if(tlw)
        tlw->Cocoa_close();
    [super close];
}

- (BOOL)windowShouldClose: (id)sender
{
    wxLogDebug("windowShouldClose");
    wxCocoaNSWindow *tlw = wxCocoaNSWindow::GetFromCocoa(sender);
    if(tlw && !tlw->Cocoa_windowShouldClose())
        return NO;
    wxLogDebug("Window will most likely be CLOSED");
    if([[wxPoserNSWindow superclass] instancesRespondToSelector:@selector(windowShouldClose:)])
        return [super windowShouldClose: sender];
    return YES;
}

- (BOOL)wxMenuItemAction: (id)sender
{
    wxLogDebug("wxMenuItemAction");
    wxMenuItem *item = wxMenuItem::GetFromCocoa(sender);
    if(!item)
        return NO;

    wxCocoaNSWindow *tlw = wxCocoaNSWindow::GetFromCocoa(self);
    wxASSERT(tlw);
    tlw->Cocoa_wxMenuItemAction(*item);
    return YES;
}
@end // implementation wxPoserNSWindow


/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/NSView.mm
// Purpose:     wxCocoaNSView
// Author:      David Elliott
// Modified by:
// Created:     2003/02/15
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
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/cocoa/NSView.h"

#import <Appkit/NSView.h>
#import <Foundation/NSNotification.h>
#import <Foundation/NSString.h>

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------
WX_IMPLEMENT_OBJC_INTERFACE_HASHMAP(NSView)

void wxCocoaNSView::AssociateNSView(WX_NSView cocoaNSView)
{
    sm_cocoaHash.insert(wxCocoaNSViewHash::value_type(cocoaNSView,this));
    [[NSNotificationCenter defaultCenter] addObserver:(id)sm_cocoaObserver selector:@selector(notificationFrameChanged:) name:@"NSViewFrameDidChangeNotification" object:cocoaNSView];
    [cocoaNSView setPostsFrameChangedNotifications: YES];
}

void wxCocoaNSView::DisassociateNSView(WX_NSView cocoaNSView)
{
    sm_cocoaHash.erase(cocoaNSView);
    [[NSNotificationCenter defaultCenter] removeObserver:(id)sm_cocoaObserver name:@"NSViewFrameDidChangeNotification" object:cocoaNSView];
}

// ============================================================================
// @class wxPoserNSView
// ============================================================================
@interface wxPoserNSView : NSView
{
}

- (void)drawRect: (NSRect)rect;
@end // wxPoserNSView

WX_IMPLEMENT_POSER(wxPoserNSView);
@implementation wxPoserNSView : NSView

- (void)drawRect: (NSRect)rect
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_drawRect(rect) )
        [super drawRect:rect];
}

@end // implementation wxPoserNSView

@interface wxNSViewNotificationObserver : NSObject
{
}

// FIXME: Initializing like this is a really bad idea.  If for some reason
// we ever require posing as an NSObject we won't be able to since an instance
// will have already been created here.  Of course, catching messages for
// NSObject seems like a LOT of overkill, so I doubt we ever will anyway!
void *wxCocoaNSView::sm_cocoaObserver = [[wxNSViewNotificationObserver alloc] init];

- (void)notificationFrameChanged: (NSNotification *)notification;
@end // interface wxNSViewNotificationObserver

@implementation wxNSViewNotificationObserver : NSObject

- (void)notificationFrameChanged: (NSNotification *)notification;
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa([notification object]);
    wxCHECK_RET(win,"notificationFrameChanged received but no wxWindow exists");
    win->Cocoa_FrameChanged();
}

@end // implementation wxNSViewNotificationObserver


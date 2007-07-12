/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/NSView.mm
// Purpose:     wxCocoaNSView
// Author:      David Elliott
// Modified by:
// Created:     2003/02/15
// RCS-ID:      $Id$
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
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/cocoa/NSView.h"

#import <Foundation/NSNotification.h>
#import <Foundation/NSString.h>
#include "wx/cocoa/objc/NSView.h"

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------
WX_IMPLEMENT_OBJC_INTERFACE_HASHMAP(NSView)

void wxCocoaNSView::AssociateNSView(WX_NSView cocoaNSView)
{
    if(cocoaNSView)
    {
        sm_cocoaHash.insert(wxCocoaNSViewHash::value_type(cocoaNSView,this));
        [[NSNotificationCenter defaultCenter] addObserver:(id)sm_cocoaObserver selector:@selector(notificationFrameChanged:) name:@"NSViewFrameDidChangeNotification" object:cocoaNSView];
        [cocoaNSView setPostsFrameChangedNotifications: YES];
    }
}

void wxCocoaNSView::DisassociateNSView(WX_NSView cocoaNSView)
{
    if(cocoaNSView)
    {
        sm_cocoaHash.erase(cocoaNSView);
        [[NSNotificationCenter defaultCenter] removeObserver:(id)sm_cocoaObserver name:@"NSViewFrameDidChangeNotification" object:cocoaNSView];
    }
}

// ============================================================================
// @class WXNSView
// ============================================================================

@implementation WXNSView : NSView

- (BOOL)acceptsFirstMouse:(NSEvent *)theEvent
{
    bool acceptsFirstMouse = false;
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if(!win || !win->Cocoa_acceptsFirstMouse(acceptsFirstMouse, theEvent))
        acceptsFirstMouse = [super acceptsFirstMouse:theEvent];
    return acceptsFirstMouse;
}

- (void)drawRect: (NSRect)rect
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_drawRect(rect) )
        [super drawRect:rect];
}

- (void)mouseDown:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_mouseDown(theEvent) )
        [super mouseDown:theEvent];
}

- (void)mouseDragged:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_mouseDragged(theEvent) )
        [super mouseDragged:theEvent];
}

- (void)mouseUp:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_mouseUp(theEvent) )
        [super mouseUp:theEvent];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_mouseMoved(theEvent) )
        [super mouseMoved:theEvent];
}

- (void)mouseEntered:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_mouseEntered(theEvent) )
        [super mouseEntered:theEvent];
}

- (void)mouseExited:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_mouseExited(theEvent) )
        [super mouseExited:theEvent];
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_rightMouseDown(theEvent) )
        [super rightMouseDown:theEvent];
}

- (void)rightMouseDragged:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_rightMouseDragged(theEvent) )
        [super rightMouseDragged:theEvent];
}

- (void)rightMouseUp:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_rightMouseUp(theEvent) )
        [super rightMouseUp:theEvent];
}

- (void)otherMouseDown:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_otherMouseDown(theEvent) )
        [super otherMouseDown:theEvent];
}

- (void)otherMouseDragged:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_otherMouseDragged(theEvent) )
        [super otherMouseDragged:theEvent];
}

- (void)otherMouseUp:(NSEvent *)theEvent
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_otherMouseUp(theEvent) )
        [super otherMouseUp:theEvent];
}

- (void)resetCursorRects
{
    wxCocoaNSView *win = wxCocoaNSView::GetFromCocoa(self);
    if( !win || !win->Cocoa_resetCursorRects() )
        [super resetCursorRects];
}

@end // implementation WXNSView

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
    wxCHECK_RET(win,wxT("notificationFrameChanged received but no wxWindow exists"));
    win->Cocoa_FrameChanged();
}

@end // implementation wxNSViewNotificationObserver


///////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/objc/NSWindow.h
// Purpose:     WXNSWindow class
// Author:      David Elliott
// Modified by:
// Created:     2007/04/20 (move from NSWindow.mm)
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#import <AppKit/NSWindow.h>
#import <AppKit/NSPanel.h>

// ============================================================================
// @class WXNSWindow
// ============================================================================
@interface WXNSWindow : NSWindow
{
}

- (BOOL)canBecomeKeyWindow;
- (BOOL)canBecomeMainWindow;
@end // WXNSWindow

// ============================================================================
// @class WXNSPanel
// ============================================================================
@interface WXNSPanel : NSPanel
{
}

- (BOOL)canBecomeKeyWindow;
- (BOOL)canBecomeMainWindow;
@end // WXNSPanel


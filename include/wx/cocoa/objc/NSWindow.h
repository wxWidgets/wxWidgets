///////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/objc/NSWindow.h
// Purpose:     wxPoserNSWindow class
// Author:      David Elliott
// Modified by:
// Created:     2007/04/20 (move from NSWindow.mm)
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#import <AppKit/NSWindow.h>

// ============================================================================
// @class wxPoserNSWindow
// ============================================================================
@interface wxPoserNSWindow : NSWindow
{
}

- (BOOL)canBecomeKeyWindow;
- (BOOL)canBecomeMainWindow;
@end // wxPoserNSwindow


///////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/NSWindow.h
// Purpose:     wxCocoaNSWindow class
// Author:      David Elliott
// Modified by:
// Created:     2003/03/16
// RCS-ID:      $Id:
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_NSWINDOW_H__
#define __WX_COCOA_NSWINDOW_H__

#include "wx/hashmap.h"
#include "wx/cocoa/ObjcAssociate.h"

WX_DECLARE_OBJC_HASHMAP(NSWindow);

class wxCocoaNSWindow
{
/* NSWindow is a rather special case and requires some extra attention */
    WX_DECLARE_OBJC_INTERFACE_HASHMAP(NSWindow)
public:
    void AssociateNSWindow(WX_NSWindow cocoaNSWindow);
    void DisassociateNSWindow(WX_NSWindow cocoaNSWindow);
    virtual void Cocoa_close(void) = 0;
    virtual bool Cocoa_canBecomeMainWindow(bool &canBecome)
    {   return false; }
    virtual bool CocoaDelegate_windowShouldClose(void) = 0;
    virtual void CocoaDelegate_windowDidBecomeKey(void) { }
    virtual void CocoaDelegate_windowDidResignKey(void) { }
    virtual void CocoaDelegate_windowDidBecomeMain(void) { }
    virtual void CocoaDelegate_windowDidResignMain(void) { }
protected:
    static struct objc_object *sm_cocoaDelegate;
};

#endif // _WX_COCOA_NSWINDOW_H_

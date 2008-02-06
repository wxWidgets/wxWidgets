///////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/NSView.h
// Purpose:     wxCocoaNSView class
// Author:      David Elliott
// Modified by:
// Created:     2003/02/15
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_NSVIEW_H__
#define __WX_COCOA_NSVIEW_H__

#include "wx/hashmap.h"
#include "wx/cocoa/ObjcAssociate.h"

#if defined(__LP64__) || defined(NS_BUILD_32_LIKE_64)
typedef struct CGRect NSRect;
#else
typedef struct _NSRect NSRect;
#endif

struct objc_object;

class wxWindow;

WX_DECLARE_OBJC_HASHMAP(NSView);
class wxCocoaNSView
{
/* NSView is a rather special case and requires some extra attention */
    WX_DECLARE_OBJC_INTERFACE_HASHMAP(NSView)
public:
    void AssociateNSView(WX_NSView cocoaNSView);
    void DisassociateNSView(WX_NSView cocoaNSView);
protected:
    static struct objc_object *sm_cocoaObserver;
public:
    virtual wxWindow* GetWxWindow() const
    {   return NULL; }
    virtual void Cocoa_FrameChanged(void) = 0;
#if wxUSE_ABI_INCOMPATIBLE_FEATURES
    virtual void Cocoa_synthesizeMouseMoved(void) = 0;
#endif
    virtual bool Cocoa_acceptsFirstMouse(bool &acceptsFirstMouse, WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_drawRect(const NSRect &rect)
    {   return false; }
    virtual bool Cocoa_mouseDown(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_mouseDragged(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_mouseUp(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_mouseMoved(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_mouseEntered(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_mouseExited(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_rightMouseDown(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_rightMouseDragged(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_rightMouseUp(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_otherMouseDown(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_otherMouseDragged(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_otherMouseUp(WX_NSEvent theEvent)
    {   return false; }
    virtual bool Cocoa_resetCursorRects()
    {   return false; }
#if wxUSE_ABI_INCOMPATIBLE_FEATURES
    virtual bool Cocoa_viewDidMoveToWindow()
    {   return false; }
    virtual bool Cocoa_viewWillMoveToWindow(WX_NSWindow newWindow)
    {   return false; }
#endif
    virtual ~wxCocoaNSView() { }
};

#endif
    // __WX_COCOA_NSVIEW_H__

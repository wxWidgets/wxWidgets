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
    inline void DisassociateNSWindow(WX_NSWindow cocoaNSWindow)
    {
        sm_cocoaHash.erase(cocoaNSWindow);
    }
    virtual void Cocoa_close(void) = 0;
    virtual bool Cocoa_windowShouldClose(void) = 0;
    virtual void Cocoa_wxMenuItemAction(wxMenuItem& item) = 0;
};

#endif // _WX_COCOA_NSWINDOW_H_

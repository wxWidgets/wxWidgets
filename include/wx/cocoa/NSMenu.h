///////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/NSMenu.h
// Purpose:     wxCocoaNSMenu class
// Author:      David Elliott
// Modified by:
// Created:     2002/12/09
// RCS-ID:      $Id: 
// Copyright:   (c) 2002 David Elliott
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_NSMENU_H__
#define __WX_COCOA_NSMENU_H__

#include "wx/hashmap.h"
#include "wx/cocoa/ObjcAssociate.h"

WX_DECLARE_OBJC_HASHMAP(NSMenu);

// ========================================================================
// wxCocoaNSMenu
// ========================================================================

class wxCocoaNSMenu
{
    WX_DECLARE_OBJC_INTERFACE(NSMenu)
// ------------------------------------------------------------------------
// initialization
// ------------------------------------------------------------------------
public:
    inline wxCocoaNSMenu()
    {
        m_cocoaNSMenu = NULL;
    }
    ~wxCocoaNSMenu();
// ------------------------------------------------------------------------
// Cocoa specifics
// ------------------------------------------------------------------------
public:
    bool CocoaCreate(const wxString &title);
    inline WX_NSMenu GetNSMenu() { return m_cocoaNSMenu; }
protected:
    WX_NSMenu m_cocoaNSMenu;
// ------------------------------------------------------------------------
// Implementation
// ------------------------------------------------------------------------
protected:
};

#endif // _WX_COCOA_NSMENU_H_

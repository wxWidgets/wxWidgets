///////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/NSMenu.h
// Purpose:     wxCocoaNSMenu class
// Author:      David Elliott
// Modified by:
// Created:     2002/12/09
// RCS-ID:      $Id$
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
};

#endif // _WX_COCOA_NSMENU_H_

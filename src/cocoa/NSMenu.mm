/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/NSMenu.mm
// Purpose:     wxCocoaNSMenu implementation
// Author:      David Elliott
// Modified by:
// Created:     2002/12/09
// RCS-ID:      $Id$
// Copyright:   (c) 2002 David Elliott
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#if wxUSE_MENUS
#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/cocoa/NSMenu.h"
//#include "wx/cocoa/ObjcPose.h"

#if 0 // There is no reason to pose for NSMenu at this time.
// ============================================================================
// @class wxPoserNSMenu
// ============================================================================
@interface wxPoserNSMenu : NSMenu
{
}

@end // wxPoserNSMenu

WX_IMPLEMENT_POSER(wxPoserNSMenu);
@implementation wxPoserNSMenu : NSMenu

@end // wxPoserNSMenu
#endif // 0

#endif // wxUSE_MENUS

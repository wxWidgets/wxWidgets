/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/NSMenu.mm
// Purpose:     wxCocoaNSMenu implementation
// Author:      David Elliott
// Modified by:
// Created:     2002/12/09
// RCS-ID:      $Id: 
// Copyright:   (c) 2002 David Elliott
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
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/cocoa/NSMenu.h"
#include "wx/cocoa/ObjcPose.h"

#import <Foundation/NSString.h>
#import <AppKit/NSMenu.h>

#if wxUSE_MENUS

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

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

// ============================================================================
// wxCocoaNSMenu implementation
// ============================================================================
bool wxCocoaNSMenu::CocoaCreate(const wxString &title)
{
    wxLogDebug("CocoaCreate: "+title);
    m_cocoaNSMenu = [[NSMenu alloc] initWithTitle: [NSString stringWithCString: title.c_str()]];
    return true;
}

wxCocoaNSMenu::~wxCocoaNSMenu()
{
    [m_cocoaNSMenu release];
}

#endif // wxUSE_MENUS

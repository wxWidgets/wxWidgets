///////////////////////////////////////////////////////////////////////////////
// Name:        menuitem.cpp
// Purpose:     wxMenuItem implementation
// Author:      David Elliott
// Modified by:
// Created:     2002/12/15
// RCS-ID:      $Id: 
// Copyright:   2002 David Elliott
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/menu.h"
    #include "wx/menuitem.h"
    #include "wx/utils.h"
    #include "wx/frame.h"
    #include "wx/log.h"
#endif

#include "wx/cocoa/ObjcPose.h"
#include "wx/cocoa/autorelease.h"

#import <AppKit/NSMenuItem.h>
#import <AppKit/NSMenu.h>
#import <Foundation/NSString.h>

#if wxUSE_MENUS

// ----------------------------------------------------------------------------
// functions prototypes
// ----------------------------------------------------------------------------

// ============================================================================
// @class wxNSMenuItemTarget
// ============================================================================
@interface wxNSMenuItemTarget : NSObject
{
}

- (void)wxMenuItemAction: (id)sender;
@end //interface wxNSMenuItemTarget

@implementation wxNSMenuItemTarget : NSObject

- (void)wxMenuItemAction: (id)sender
{
    wxLogDebug("wxMenuItemAction");
    wxMenuItem *item = wxMenuItem::GetFromCocoa(sender);
    wxCHECK_RET(item,"wxMenuItemAction received but no wxMenuItem exists!");

    wxMenu *menu = item->GetMenu();
    wxCHECK_RET(menu,"wxMenuItemAction received but wxMenuItem is not in a wxMenu");
    wxMenuBar *menubar = menu->GetMenuBar();
    if(menubar)
    {
        wxFrame *frame = menubar->GetFrame();
        wxCHECK_RET(frame, "wxMenuBar MUST be attached to a wxFrame!");
        frame->Command(item->GetId());
    }
}

@end //implementation wxNSMenuItemTarget

// ============================================================================
// @class wxPoserNSMenuItem
// ============================================================================
@interface wxPoserNSMenuItem : NSMenuItem
{
}

@end // wxPoserNSMenuItem

WX_IMPLEMENT_POSER(wxPoserNSMenuItem);
@implementation wxPoserNSMenuItem : NSMenuItem

@end // wxPoseRNSMenuItem

// ============================================================================
// wxMenuItemCocoa implementation
// ============================================================================
IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxObject)
wxMenuItemCocoaHash wxMenuItemCocoa::sm_cocoaHash;

struct objc_object *wxMenuItemCocoa::sm_cocoaTarget = [[wxNSMenuItemTarget alloc] init];

// ----------------------------------------------------------------------------
// wxMenuItemBase
// ----------------------------------------------------------------------------

wxMenuItem *wxMenuItemBase::New(wxMenu *parentMenu,
                                int itemid,
                                const wxString& name,
                                const wxString& help,
                                wxItemKind kind,
                                wxMenu *subMenu)
{
    return new wxMenuItem(parentMenu, itemid, name, help, kind, subMenu);
}

/* static */
wxString wxMenuItemBase::GetLabelFromText(const wxString& text)
{
    return wxStripMenuCodes(text);
}

// ----------------------------------------------------------------------------
// ctor & dtor
// ----------------------------------------------------------------------------
wxMenuItemCocoa::wxMenuItemCocoa(wxMenu *pParentMenu,
                       int itemid,
                       const wxString& strName,
                       const wxString& strHelp,
                       wxItemKind kind,
                       wxMenu *pSubMenu)
          : wxMenuItemBase(pParentMenu, itemid, strName, strHelp, kind, pSubMenu)
{
    wxAutoNSAutoreleasePool pool;
    NSString *menuTitle = [[NSString alloc] initWithCString: wxStripMenuCodes(strName).c_str()];
    m_cocoaNSMenuItem = [[NSMenuItem alloc] initWithTitle:menuTitle action:@selector(wxMenuItemAction:) keyEquivalent:@""];
    sm_cocoaHash.insert(wxMenuItemCocoaHash::value_type(m_cocoaNSMenuItem,this));
    [m_cocoaNSMenuItem setTarget:sm_cocoaTarget];
    if(pSubMenu)
    {
        wxASSERT(pSubMenu->GetNSMenu());
        [pSubMenu->GetNSMenu() setTitle:menuTitle];
        [m_cocoaNSMenuItem setSubmenu:pSubMenu->GetNSMenu()];
    }
    [menuTitle release];
}

wxMenuItem::~wxMenuItem()
{
    sm_cocoaHash.erase(m_cocoaNSMenuItem);
    [m_cocoaNSMenuItem release];
}

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

// change item state
// -----------------

void wxMenuItem::Enable(bool bDoEnable)
{
    wxMenuItemBase::Enable(bDoEnable);
}

void wxMenuItem::Check(bool bDoCheck)
{
    wxCHECK_RET( IsCheckable(), "only checkable items may be checked" );
    wxMenuItemBase::Check(bDoCheck);
}

void wxMenuItem::SetText(const wxString& label)
{
    wxMenuItemBase::SetText(label);
}

void wxMenuItem::SetCheckable(bool checkable)
{
    wxMenuItemBase::SetCheckable(checkable);
}

#endif // wxUSE_MENUS

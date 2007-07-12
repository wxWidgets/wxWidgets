/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/NSMenu.mm
// Purpose:     wxCocoaNSMenu implementation
// Author:      David Elliott
// Modified by:
// Created:     2002/12/09
// RCS-ID:      $Id$
// Copyright:   (c) 2002 David Elliott
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#if wxUSE_MENUS
#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/cocoa/NSMenu.h"
#include "wx/cocoa/ObjcPose.h"

#import <Foundation/NSNotification.h>
#include "wx/cocoa/objc/NSMenu.h"

// ============================================================================
// @class wxPoserNSMenu
// ============================================================================

WX_IMPLEMENT_POSER(wxPoserNSMenu);
@implementation wxPoserNSMenu : NSMenu

- (void)dealloc
{
    wxCocoaNSMenu *menu = wxCocoaNSMenu::GetFromCocoa(self);
    if(menu)
        menu->Cocoa_dealloc();
    [super dealloc];
}

@end // wxPoserNSMenu

// ============================================================================
// @class wxNSMenuNotificationObserver
// ============================================================================
@interface wxNSMenuNotificationObserver : NSObject
{
}

struct objc_object *wxCocoaNSMenu::sm_cocoaObserver = [[wxNSMenuNotificationObserver alloc] init];

- (void)menuDidAddItem: (NSNotification *)notification;
- (void)menuDidChangeItem: (NSNotification *)notification;
- (void)menuDidRemoveItem: (NSNotification *)notification;
- (void)menuDidSendAction: (NSNotification *)notification;
- (void)menuWillSendAction: (NSNotification *)notification;
@end // interface wxNSMenuNotificationObserver

@implementation wxNSMenuNotificationObserver : NSObject

- (void)menuDidAddItem: (NSNotification *)notification
{
    wxCocoaNSMenu *menu = wxCocoaNSMenu::GetFromCocoa([notification object]);
    wxCHECK_RET(menu,wxT("menuDidAddItem received but no wxMenu exists"));
    menu->CocoaNotification_menuDidAddItem(notification);
}

- (void)menuDidChangeItem: (NSNotification *)notification
{
    wxCocoaNSMenu *menu = wxCocoaNSMenu::GetFromCocoa([notification object]);
    wxCHECK_RET(menu,wxT("menuDidChangeItem received but no wxMenu exists"));
    menu->CocoaNotification_menuDidChangeItem(notification);
}

- (void)menuDidRemoveItem: (NSNotification *)notification
{
    wxCocoaNSMenu *menu = wxCocoaNSMenu::GetFromCocoa([notification object]);
    wxCHECK_RET(menu,wxT("menuDidRemoveItem received but no wxMenu exists"));
    menu->CocoaNotification_menuDidRemoveItem(notification);
}

- (void)menuDidSendAction: (NSNotification *)notification
{
    wxCocoaNSMenu *menu = wxCocoaNSMenu::GetFromCocoa([notification object]);
    wxCHECK_RET(menu,wxT("menuDidSendAction received but no wxMenu exists"));
    menu->CocoaNotification_menuDidSendAction(notification);
}

- (void)menuWillSendAction: (NSNotification *)notification
{
    wxCocoaNSMenu *menu = wxCocoaNSMenu::GetFromCocoa([notification object]);
    wxCHECK_RET(menu,wxT("menuWillSendAction received but no wxMenu exists"));
    menu->CocoaNotification_menuWillSendAction(notification);
}

@end // implementation wxNSMenuNotificationObserver

// ========================================================================
// wxCocoaNSMenu
// ========================================================================
WX_IMPLEMENT_OBJC_INTERFACE_HASHMAP(NSMenu)

void wxCocoaNSMenu::AssociateNSMenu(WX_NSMenu cocoaNSMenu, unsigned int flags)
{
    if(cocoaNSMenu)
    {
        sm_cocoaHash.insert(wxCocoaNSMenuHash::value_type(cocoaNSMenu,this));
        if(flags&OBSERVE_DidAddItem)
            [[NSNotificationCenter defaultCenter] addObserver:(id)sm_cocoaObserver selector:@selector(menuDidAddItem:) name:NSMenuDidAddItemNotification object:cocoaNSMenu];
        if(flags&OBSERVE_DidChangeItem)
            [[NSNotificationCenter defaultCenter] addObserver:(id)sm_cocoaObserver selector:@selector(menuDidChangeItem:) name:NSMenuDidChangeItemNotification object:cocoaNSMenu];
        if(flags&OBSERVE_DidRemoveItem)
            [[NSNotificationCenter defaultCenter] addObserver:(id)sm_cocoaObserver selector:@selector(menuDidRemoveItem:) name:NSMenuDidRemoveItemNotification object:cocoaNSMenu];
        if(flags&OBSERVE_DidSendAction)
            [[NSNotificationCenter defaultCenter] addObserver:(id)sm_cocoaObserver selector:@selector(menuDidSendAction:) name:NSMenuDidSendActionNotification object:cocoaNSMenu];
        if(flags&OBSERVE_WillSendAction)
            [[NSNotificationCenter defaultCenter] addObserver:(id)sm_cocoaObserver selector:@selector(menuWillSendAction:) name:NSMenuWillSendActionNotification object:cocoaNSMenu];
    }
}

void wxCocoaNSMenu::DisassociateNSMenu(WX_NSMenu cocoaNSMenu)
{
    if(cocoaNSMenu)
    {
        sm_cocoaHash.erase(cocoaNSMenu);
        [[NSNotificationCenter defaultCenter] removeObserver:(id)sm_cocoaObserver name:NSMenuDidAddItemNotification object:cocoaNSMenu];
        [[NSNotificationCenter defaultCenter] removeObserver:(id)sm_cocoaObserver name:NSMenuDidChangeItemNotification object:cocoaNSMenu];
        [[NSNotificationCenter defaultCenter] removeObserver:(id)sm_cocoaObserver name:NSMenuDidRemoveItemNotification object:cocoaNSMenu];
        [[NSNotificationCenter defaultCenter] removeObserver:(id)sm_cocoaObserver name:NSMenuDidSendActionNotification object:cocoaNSMenu];
        [[NSNotificationCenter defaultCenter] removeObserver:(id)sm_cocoaObserver name:NSMenuWillSendActionNotification object:cocoaNSMenu];
    }
}

#endif // wxUSE_MENUS

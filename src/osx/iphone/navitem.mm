/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/navitem.mm
// Purpose:     wxNavigationItem class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/osx/private.h"
#include "wx/osx/iphone/private.h"
#include "wx/viewcontroller.h"
#include "wx/navitem.h"

IMPLEMENT_DYNAMIC_CLASS(wxNavigationItem, wxNavigationItemBase)

// Create an item with a title. The optional backButton is used to specify this item's
// back button when it is under the top item. leftButton can be used to specify the button
// when it's the top item, and rightButton is an optional button placed at the right of the bar.
bool wxNavigationItem::Create(const wxString& title,
                              wxBarButton* backButton,
                              wxBarButton* leftButton,
                              wxBarButton* rightButton,
                              bool ownButtons)
{
    m_title = title;
    m_ownButtons = ownButtons;
    
    // UINavigationItem
    m_nativeNavItem = [[UINavigationItem alloc] initWithTitle:[NSString stringWithString:wxCFStringRef(title).AsNSString()]];
    if (backButton) {
        SetBackButton(backButton);
    }
    if (leftButton) {
        SetLeftButton(leftButton);
    }
    if (rightButton) {
        SetRightButton(rightButton);
    }
    
    return true;
}

wxNavigationItem::~wxNavigationItem()
{
    if (m_ownButtons) {
        delete m_backButton;
        delete m_leftButton;
        delete m_rightButton;
    }
    
    if (m_nativeNavItem) {
        [m_nativeNavItem release];
        m_nativeNavItem = NULL;
    }
}

void wxNavigationItem::Init()
{
    m_backButton = NULL;
    m_leftButton = NULL;
    m_rightButton = NULL;
    m_ownButtons = false;
    m_hideBackButton = false;
    m_viewController = NULL;
    m_nativeNavItem = NULL;
}

/// Sets the button to show on the top item, when this item is under the top item.
void wxNavigationItem::SetBackButton(wxBarButton* backButton)
{
    if ( !m_nativeNavItem ) {
        return;
    }
    
    UINavigationItem *navItem = (UINavigationItem *)m_nativeNavItem;
    
    UIBarButtonItem *buttonItem = (UIBarButtonItem *)(backButton->GetNativeBarButtonItem());
    if ( !buttonItem ) {
        return;
    }
    
    [navItem setBackBarButtonItem:buttonItem];

    m_backButton = backButton;
}

/// Sets the button to show on the left when this item is at the top.
void wxNavigationItem::SetLeftButton(wxBarButton* leftButton)
{
    if ( !m_nativeNavItem ) {
        return;
    }
    
    UINavigationItem *navItem = (UINavigationItem *)m_nativeNavItem;
    
    UIBarButtonItem *buttonItem = (UIBarButtonItem *)(leftButton->GetNativeBarButtonItem());
    if ( !buttonItem ) {
        return;
    }
    
    [navItem setLeftBarButtonItem:buttonItem];
    
    m_leftButton = leftButton;
}

/// Sets the button to show on the right when this item is at the top.
void wxNavigationItem::SetRightButton(wxBarButton* rightButton)
{
    if ( !m_nativeNavItem ) {
        return;
    }
    
    UINavigationItem *navItem = (UINavigationItem *)m_nativeNavItem;
    
    UIBarButtonItem *buttonItem = (UIBarButtonItem *)(rightButton->GetNativeBarButtonItem());
    if ( !buttonItem ) {
        return;
    }
    
    [navItem setRightBarButtonItem:buttonItem];
    
    m_rightButton = rightButton;
}

/// Sets the title.
void wxNavigationItem::SetTitle(const wxString& title)
{
    if ( !m_nativeNavItem ) {
        return;
    }
    
    UINavigationItem *navItem = (UINavigationItem *)m_nativeNavItem;
    [navItem setTitle:wxCFStringRef(title).AsNSString()];
    
    m_title = title;
}

/// Sets whether the custom buttons should be deleted on deletion of this item.
void wxNavigationItem::SetOwnButtons(bool own)
{
    m_ownButtons = own;
}

/// Sets whether the back button should be hidden when this item is at the top.
void wxNavigationItem::SetHideBackButton(bool hide)
{
    if ( !m_nativeNavItem ) {
        return;
    }
    
    UINavigationItem *navItem = (UINavigationItem *)m_nativeNavItem;
    [navItem setHidesBackButton:hide
                       animated:NO];

    m_hideBackButton = hide;
}

/// Sets the associated view controller
void wxNavigationItem::SetViewController(wxViewController* controller)
{
    controller->SetNavigationItem(this);
    m_viewController = controller;
}

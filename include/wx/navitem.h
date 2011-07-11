/////////////////////////////////////////////////////////////////////////////
// Name:        wx/navitem.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_NAVITEM_H_BASE_
#define _WX_NAVITEM_H_BASE_

#include "wx/dynarray.h"
#include "wx/barbutton.h"

class WXDLLEXPORT wxViewController;

class WXDLLIMPEXP_FWD_CORE wxNavigationItem;

/**
 @class wxNavigationItem
 
 A navigation item manages optional left and right buttons, and a title.
 Since the buttons can be reused, by default they are not deleted when the item
 is deleted. All supplied buttons are optional since the back button
 is created from the previous item's title.
 
 Currently, custom title views are not supported.
 
 @category{wxbile}
 */

class WXDLLEXPORT wxNavigationItemBase: public wxEvtHandler
{
public:
    /// Default constructor.
    wxNavigationItemBase() { }
    
    /// Create an item with a title. The optional backButton is used to specify this item's
    /// back button when it is under the top item. leftButton can be used to specify the button
    /// when it's the top item, and rightButton is an optional button placed at the right of the bar.
    wxNavigationItemBase(const wxString& title,
                         wxBarButton* backButton = NULL,
                         wxBarButton* leftButton = NULL,
                         wxBarButton* rightButton = NULL,
                         bool ownButtons = false) { }
    
    virtual ~wxNavigationItemBase();
    
    void Init();
    
    /// Sets the button to show on the top item, when this item is under the top item.
    void SetBackButton(wxBarButton* backButton) { m_backButton = backButton; }
    
    /// Gets the button to show on the top item, when this item is under the top item.
    wxBarButton* GetBackButton() const { return m_backButton; }
    
    /// Sets the button to show on the left when this item is at the top.
    void SetLeftButton(wxBarButton* leftButton) { m_leftButton = leftButton; }
    
    /// Gets the button to show on the left when this item is at the top.
    wxBarButton* GetLeftButton() const { return m_leftButton; }
    
    /// Sets the button to show on the right when this item is at the top.
    void SetRightButton(wxBarButton* rightButton) { m_rightButton = rightButton; }
    
    /// Gets the button to show on the right when this item is at the top.
    wxBarButton* GetRightButton() const { return m_rightButton; }
    
    /// Sets the title.
    void SetTitle(const wxString& title) { m_title = title; }
    
    /// Gets the title.
    const wxString& GetTitle() const { return m_title; }
    
    /// Sets whether the custom buttons should be deleted on deletion of this item.
    void SetOwnButtons(bool own) { m_ownButtons = own; }
    
    /// Returns true if the custom buttons should be deleted on deletion of this item.
    bool GetOwnButtons() const { return m_ownButtons; }
    
    /// Sets whether the back button should be hidden when this item is at the top.
    void SetHideBackButton(bool hide) { m_hideBackButton = hide; }
    
    /// Returns true if the back button should be hidden when this item is at the top.
    bool GetHideBackButton() const { return m_hideBackButton; }
    
    /// Returns the associated view controller, if any
    wxViewController* GetViewController() const { return m_viewController; }
    
    /// Sets the associated view controller
    void SetViewController(wxViewController* controller) { m_viewController = controller; }
    
protected:
    
    wxBarButton*          m_backButton;
    wxBarButton*          m_leftButton;
    wxBarButton*          m_rightButton;
    wxString              m_title;
    wxViewController*     m_viewController;
    
    bool                    m_ownButtons;
    bool                    m_hideBackButton;
    
    wxDECLARE_NO_COPY_CLASS(wxNavigationItemBase);    
};

WX_DEFINE_ARRAY_PTR(wxNavigationItem*, wxNavigationItemArray);



// ----------------------------------------------------------------------------
// wxNavigationItem class itself
// ----------------------------------------------------------------------------

#if defined(__WXOSX_IPHONE__)  // iPhone-only
    #include  "wx/osx/iphone/navitem.h"
#endif


#endif
    // _WX_NAVITEM_H_BASE_

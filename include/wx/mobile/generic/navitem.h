/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/navitem.h
// Purpose:     wxMoNavigationItem class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWidgets Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_NAVITEM_H_
#define _WX_MOBILE_GENERIC_NAVITEM_H_

#include "wx/dynarray.h"
#include "wx/mobile/barbutton.h"

class WXDLLEXPORT wxMoViewController;

/**
    @class wxMoNavigationItem

    A navigation item manages optional left and right buttons, and a title.
    Since the buttons can be reused, by default they are not deleted when the item
    is deleted. All supplied buttons are optional since the back button
    is created from the previous item's title.

    Currently, custom title views are not supported.

    @category{wxMobile}
*/

class WXDLLEXPORT wxMoNavigationItem: public wxEvtHandler
{
public:
    /// Default constructor.
    wxMoNavigationItem() { Init(); }

    /// Create an item with a title. The optional backButton is used to specify this item's
    /// back button when it is under the top item. leftButton can be used to specify the button
    /// when it's the top item, and rightButton is an optional button placed at the right of the bar.
    wxMoNavigationItem(const wxString& title, wxMoBarButton* backButton = NULL, wxMoBarButton* leftButton = NULL, wxMoBarButton* rightButton = NULL,
                        bool ownButtons = false)
    {
        Init();
        Create(title, backButton, leftButton, rightButton, ownButtons);
    }

    /// Create an item with a title. The optional backButton is used to specify this item's
    /// back button when it is under the top item. leftButton can be used to specify the button
    /// when it's the top item, and rightButton is an optional button placed at the right of the bar.
    bool Create(const wxString& title, wxMoBarButton* backButton = NULL, wxMoBarButton* leftButton = NULL, wxMoBarButton* rightButton = NULL,
                bool ownButtons = false);

    virtual ~wxMoNavigationItem();

    void Init();

    /// Sets the button to show on the top item, when this item is under the top item.
    void SetBackButton(wxMoBarButton* backButton) { m_backButton = backButton; }

    /// Gets the button to show on the top item, when this item is under the top item.
    wxMoBarButton* GetBackButton() const { return m_backButton; }

    /// Sets the button to show on the left when this item is at the top.
    void SetLeftButton(wxMoBarButton* leftButton) { m_leftButton = leftButton; }

    /// Gets the button to show on the left when this item is at the top.
    wxMoBarButton* GetLeftButton() const { return m_leftButton; }

    /// Sets the button to show on the right when this item is at the top.
    void SetRightButton(wxMoBarButton* rightButton) { m_rightButton = rightButton; }

    /// Gets the button to show on the right when this item is at the top.
    wxMoBarButton* GetRightButton() const { return m_rightButton; }

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
    wxMoViewController* GetViewController() const { return m_viewController; }

    /// Sets the associated view controller
    void SetViewController(wxMoViewController* controller) { m_viewController = controller; }

protected:

    wxMoBarButton*          m_backButton;
    wxMoBarButton*          m_leftButton;
    wxMoBarButton*          m_rightButton;
    wxString                m_title;
    wxMoViewController*     m_viewController;

    bool                    m_ownButtons;
    bool                    m_hideBackButton;

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoNavigationItem)
};

WX_DEFINE_ARRAY_PTR(wxMoNavigationItem*, wxMoNavigationItemArray);

#endif
    // _WX_MOBILE_GENERIC_NAVITEM_H_

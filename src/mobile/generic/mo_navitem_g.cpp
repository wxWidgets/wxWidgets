/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_navitem_g.cpp
// Purpose:     wxMoNavigationItem class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/mobile/generic/navitem.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoNavigationItem, wxEvtHandler)

// Create an item with a title. The optional backButton is used to specify this item's
// back button when it is under the top item. leftButton can be used to specify the button
// when it's the top item, and rightButton is an optional button placed at the right of the bar.
bool wxMoNavigationItem::Create(const wxString& title, wxMoBarButton* backButton, wxMoBarButton* leftButton, wxMoBarButton* rightButton, bool ownButtons)
{
    m_title = title;
    m_backButton = backButton;
    m_leftButton = leftButton;
    m_rightButton = rightButton;
    m_ownButtons = ownButtons;
    
    return true;
}

wxMoNavigationItem::~wxMoNavigationItem()
{
    if (m_ownButtons)
    {
        delete m_backButton;
        delete m_leftButton;
        delete m_rightButton;;
    }
}

void wxMoNavigationItem::Init()
{
    m_backButton = NULL;
    m_leftButton = NULL;
    m_rightButton = NULL;
    m_ownButtons = false;
    m_hideBackButton = false;
    m_viewController = NULL;
}

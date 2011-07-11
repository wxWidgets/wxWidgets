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
    // FIXME stub
    
    return true;
}

wxNavigationItem::~wxNavigationItem()
{
    // FIXME stub
}

void wxNavigationItem::Init()
{
    // FIXME stub
}

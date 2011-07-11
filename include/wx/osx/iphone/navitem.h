/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/navitem.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_NAVITEM_H_
#define _WX_NAVITEM_H_

#include "wx/dynarray.h"

class WXDLLEXPORT wxViewController;

/**
 @class wxNavigationItem

 @category{wxbile}
 */

class WXDLLEXPORT wxNavigationItem: public wxNavigationItemBase
{
public:
    /// Default constructor.
    wxNavigationItem() { Init(); }
    
    /// Create an item with a title. The optional backButton is used to specify this item's
    /// back button when it is under the top item. leftButton can be used to specify the button
    /// when it's the top item, and rightButton is an optional button placed at the right of the bar.
    wxNavigationItem(const wxString& title,
                     wxBarButton* backButton = NULL,
                     wxBarButton* leftButton = NULL,
                     wxBarButton* rightButton = NULL,
                     bool ownButtons = false)
    {
        Init();
        Create(title, backButton, leftButton, rightButton, ownButtons);
    }
    
    /// Create an item with a title. The optional backButton is used to specify this item's
    /// back button when it is under the top item. leftButton can be used to specify the button
    /// when it's the top item, and rightButton is an optional button placed at the right of the bar.
    bool Create(const wxString& title,
                wxBarButton* backButton = NULL,
                wxBarButton* leftButton = NULL,
                wxBarButton* rightButton = NULL,
                bool ownButtons = false);
    
    virtual ~wxNavigationItem();
        
protected:
    void Init();
    
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxNavigationItem);
};

#endif
    // _WX_NAVITEM_H_

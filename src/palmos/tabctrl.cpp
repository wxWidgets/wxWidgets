/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/tabctrl.cpp
// Purpose:     wxTabCtrl
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TAB_DIALOG

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/app.h"
#endif

#if defined(__WIN95__)

#include "wx/tabctrl.h"
#include "wx/palmos/imaglist.h"

IMPLEMENT_DYNAMIC_CLASS(wxTabCtrl, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxTabEvent, wxNotifyEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGING)

BEGIN_EVENT_TABLE(wxTabCtrl, wxControl)
END_EVENT_TABLE()

wxTabCtrl::wxTabCtrl()
{
}

bool wxTabCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name)
{
    return false;
}

wxTabCtrl::~wxTabCtrl()
{
}

bool wxTabCtrl::MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
    return false;
}

// Delete all items
bool wxTabCtrl::DeleteAllItems()
{
    return false;
}

// Delete an item
bool wxTabCtrl::DeleteItem(int item)
{
    return false;
}

// Get the selection
int wxTabCtrl::GetSelection() const
{
    return 0;
}

// Get the tab with the current keyboard focus
int wxTabCtrl::GetCurFocus() const
{
    return 0;
}

// Get the associated image list
wxImageList* wxTabCtrl::GetImageList() const
{
    return NULL;
}

// Get the number of items
int wxTabCtrl::GetItemCount() const
{
    return 0;
}

// Get the rect corresponding to the tab
bool wxTabCtrl::GetItemRect(int item, wxRect& wxrect) const
{
    return false;
}

// Get the number of rows
int wxTabCtrl::GetRowCount() const
{
    return 0;
}

// Get the item text
wxString wxTabCtrl::GetItemText(int item) const
{
    wxString str(wxEmptyString);

    return str;
}

// Get the item image
int wxTabCtrl::GetItemImage(int item) const
{
    return -1;
}

// Get the item data
void* wxTabCtrl::GetItemData(int item) const
{
    return 0;
}

// Hit test
int wxTabCtrl::HitTest(const wxPoint& pt, long& flags)
{
    return 0;
}

// Insert an item
bool wxTabCtrl::InsertItem(int item, const wxString& text, int imageId, void* data)
{
    return false;
}

// Set the selection
int wxTabCtrl::SetSelection(int item)
{
    return 0;
}

// Set the image list
void wxTabCtrl::SetImageList(wxImageList* imageList)
{
}

// Set the text for an item
bool wxTabCtrl::SetItemText(int item, const wxString& text)
{
    return false;
}

// Set the image for an item
bool wxTabCtrl::SetItemImage(int item, int image)
{
    return false;
}

// Set the data for an item
bool wxTabCtrl::SetItemData(int item, void* data)
{
    return false;
}

// Set the size for a fixed-width tab control
void wxTabCtrl::SetItemSize(const wxSize& size)
{
}

// Set the padding between tabs
void wxTabCtrl::SetPadding(const wxSize& padding)
{
}


#endif
    // __WIN95__

#endif // wxUSE_TAB_DIALOG

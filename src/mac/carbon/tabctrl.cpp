/////////////////////////////////////////////////////////////////////////////
// Name:        tabctrl.cpp
// Purpose:     wxTabCtrl
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "tabctrl.h"
#endif

#include "wx/wxprec.h"

#include "wx/control.h"
#include "wx/tabctrl.h"
#include "wx/mac/uma.h"

IMPLEMENT_DYNAMIC_CLASS(wxTabCtrl, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxTabEvent, wxNotifyEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGING)


BEGIN_EVENT_TABLE(wxTabCtrl, wxControl)
END_EVENT_TABLE()

wxTabCtrl::wxTabCtrl()
{
    m_imageList = NULL;
}

bool wxTabCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name)
{
    m_macIsUserPane = FALSE ;
    
    if ( !wxControl::Create(parent, id, pos, size,
                            style, wxDefaultValidator, name) )
        return false;

    m_imageList = NULL;
    
    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;

    UInt16 tabstyle = kControlTabDirectionNorth ;
    ControlTabSize tabsize = kControlTabSizeLarge ;
    if ( GetWindowVariant() == wxWINDOW_VARIANT_SMALL )
        tabsize = kControlTabSizeSmall ;
    else if ( GetWindowVariant() == wxWINDOW_VARIANT_MINI )
    {
        if (UMAGetSystemVersion() >= 0x1030 )
            tabsize = 3 ; 
        else
            tabsize = kControlSizeSmall; 
    }
    
    m_peer = new wxMacControl() ;
    verify_noerr ( CreateTabsControl( MAC_WXHWND(parent->MacGetTopLevelWindowRef()) , &bounds ,
     tabsize , tabstyle, 0, NULL,  m_peer->GetControlRefAddr() ) );
    

    MacPostControlCreate(pos,size) ;
    return TRUE ;
}

wxTabCtrl::~wxTabCtrl()
{
}

void wxTabCtrl::Command(wxCommandEvent& event)
{
}

// Delete all items
bool wxTabCtrl::DeleteAllItems()
{
    // TODO
    return FALSE;
}

// Delete an item
bool wxTabCtrl::DeleteItem(int item)
{
    // TODO
    return FALSE;
}

// Get the selection
int wxTabCtrl::GetSelection() const
{
    // TODO
    return 0;
}

// Get the tab with the current keyboard focus
int wxTabCtrl::GetCurFocus() const
{
    // TODO
    return 0;
}

// Get the associated image list
wxImageList* wxTabCtrl::GetImageList() const
{
    return m_imageList;
}

// Get the number of items
int wxTabCtrl::GetItemCount() const
{
    // TODO
    return 0;
}

// Get the rect corresponding to the tab
bool wxTabCtrl::GetItemRect(int item, wxRect& wxrect) const
{
    // TODO
    return FALSE;
}

// Get the number of rows
int wxTabCtrl::GetRowCount() const
{
    // TODO
    return 0;
}

// Get the item text
wxString wxTabCtrl::GetItemText(int item) const
{
    // TODO
    return wxEmptyString;
}

// Get the item image
int wxTabCtrl::GetItemImage(int item) const
{
    // TODO
    return 0;
}

// Get the item data
void* wxTabCtrl::GetItemData(int item) const
{
    // TODO
    return NULL;
}

// Hit test
int wxTabCtrl::HitTest(const wxPoint& pt, long& flags)
{
    // TODO
    return 0;
}

// Insert an item
bool wxTabCtrl::InsertItem(int item, const wxString& text, int imageId, void* data)
{
    // TODO
    return FALSE;
}

// Set the selection
int wxTabCtrl::SetSelection(int item)
{
    // TODO
    return 0;
}

// Set the image list
void wxTabCtrl::SetImageList(wxImageList* imageList)
{
    // TODO
}

// Set the text for an item
bool wxTabCtrl::SetItemText(int item, const wxString& text)
{
    // TODO
    return FALSE;
}

// Set the image for an item
bool wxTabCtrl::SetItemImage(int item, int image)
{
    // TODO
    return FALSE;
}

// Set the data for an item
bool wxTabCtrl::SetItemData(int item, void* data)
{
    // TODO
    return FALSE;
}

// Set the size for a fixed-width tab control
void wxTabCtrl::SetItemSize(const wxSize& size)
{
    // TODO
}

// Set the padding between tabs
void wxTabCtrl::SetPadding(const wxSize& padding)
{
    // TODO
}


/////////////////////////////////////////////////////////////////////////////
// Name:        tabctrl.cpp
// Purpose:     wxTabCtrl
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "tabctrl.h"
#endif

#include "wx/control.h"
#include "wx/tabctrl.h"
#include "wx/mac/uma.h"

IMPLEMENT_DYNAMIC_CLASS(wxTabCtrl, wxControl)

BEGIN_EVENT_TABLE(wxTabCtrl, wxControl)
END_EVENT_TABLE()

wxTabCtrl::wxTabCtrl()
{
    m_imageList = NULL;
}

bool wxTabCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name)
{
	Rect bounds ;
	Str255 title ;

  m_imageList = NULL;
	
	MacPreControlCreate( parent , id ,  "" , pos , size ,style, *((wxValidator*)NULL) , name , &bounds , title ) ;

	m_macControl = UMANewControl( parent->GetMacRootWindow() , &bounds , title , true , 0 , 0 , 1, 
	  	kControlTabSmallProc , (long) this ) ;
	
	MacPostControlCreate() ;
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
    return wxString("");
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

// Tab event
IMPLEMENT_DYNAMIC_CLASS(wxTabEvent, wxCommandEvent)

wxTabEvent::wxTabEvent(wxEventType commandType, int id):
  wxCommandEvent(commandType, id)
{
}


/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/tabctrl.mm
// Purpose:     wxTabCtrl class
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

#ifndef WX_PRECOMP
#include "wx/app.h"
#include "wx/brush.h"
#include "wx/settings.h"
#include "wx/dcscreen.h"
#include "wx/dcclient.h"
#include "wx/toplevel.h"
#endif

#include "wx/osx/private.h"
#include "wx/tabctrl.h"

#include "wx/imaglist.h"
#include "wx/dcbuffer.h"

#ifndef wxUSE_TAB_DIALOG || !wxUSE_TAB_DIALOG
IMPLEMENT_DYNAMIC_CLASS(wxTabEvent, wxNotifyEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGING)
#endif

IMPLEMENT_DYNAMIC_CLASS(wxTabCtrl, wxTabCtrlBase)

BEGIN_EVENT_TABLE(wxTabCtrl, wxTabCtrlBase)
    EVT_PAINT(wxTabCtrl::OnPaint)
    EVT_MOUSE_EVENTS(wxTabCtrl::OnMouseEvent)
    EVT_SYS_COLOUR_CHANGED(wxTabCtrl::OnSysColourChanged)
    EVT_SIZE(wxTabCtrl::OnSize)
    EVT_ERASE_BACKGROUND(wxTabCtrl::OnEraseBackground)
END_EVENT_TABLE()

wxTabCtrl::wxTabCtrl()
{
    // FIXME stub
}

wxTabCtrl::wxTabCtrl(wxWindow *parent,
                         wxWindowID id,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    Init();
    
    Create(parent, id, pos, size, style, name);
}

bool wxTabCtrl::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    return wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name);
}

wxTabCtrl::~wxTabCtrl()
{
}

void wxTabCtrl::Init()
{
    // FIXME stub
}

// wxEVT_COMMAND_TAB_SEL_CHANGED;
// wxEVT_COMMAND_TAB_SEL_CHANGING;

bool wxTabCtrl::SendCommand(wxEventType eventType, int selection)
{
    // FIXME stub
    
    return true;
}

// Responds to colour changes, and passes event on to children.
void wxTabCtrl::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    // FIXME stub
}

// Delete all items
bool wxTabCtrl::DeleteAllItems()
{
    // FIXME stub
    
    return true;
}

// Delete an item
bool wxTabCtrl::DeleteItem(int item)
{
    // FIXME stub
    
    return true;
}

// Get the selection
int wxTabCtrl::GetSelection() const
{
    // FIXME stub
    
    return 0;
}

// Get the tab with the current keyboard focus
int wxTabCtrl::GetCurFocus() const
{
    // FIXME stub
    // TODO
    return -1;
}

// Get the number of items
int wxTabCtrl::GetItemCount() const
{
    // FIXME stub
    
    return 0;
}

// Get the rect corresponding to the tab
bool wxTabCtrl::GetItemRect(int WXUNUSED(item), wxRect& WXUNUSED(rect)) const
{
    // FIXME stub
    // TODO
    return false;
}

// Get the number of rows
int wxTabCtrl::GetRowCount() const
{
    // FIXME stub
    return 1;
}

// Get the item text
wxString wxTabCtrl::GetItemText(int idx) const
{
    // FIXME stub
    
    return wxEmptyString;
}

// Get the item image
int wxTabCtrl::GetItemImage(int idx) const
{
    // FIXME stub
    
    return 0;
}

// Hit test
int wxTabCtrl::HitTest(const wxPoint& WXUNUSED(pt), long& WXUNUSED(flags))
{
    // FIXME stub
    // TODO
    return 0;
}

// Insert an item
bool wxTabCtrl::InsertItem(int item, const wxString& text, int imageId)
{
    // FIXME stub
    
    return true;
}

// Insert an item, passing a bitmap.
bool wxTabCtrl::InsertItem(int item, const wxString& text, const wxBitmap& bitmap)
{
    // FIXME stub
    
    return true;
}

// Add an item
bool wxTabCtrl::AddItem(const wxString& text, int imageId)
{
    // FIXME stub
    
    return true;
}

// Add an item, passing a bitmap.
bool wxTabCtrl::AddItem(const wxString& text, const wxBitmap& bitmap)
{
    // FIXME stub
    
    return true;
}

void wxTabCtrl::OnInsertItem(wxBarButton* button)
{
    // FIXME stub
}

// Set the selection
int wxTabCtrl::SetSelection(int item)
{
    // FIXME stub
    
    return 0;
}

// Set the selection, without generating events
int wxTabCtrl::ChangeSelection(int item)
{
    // FIXME stub
    
    return 0;
}

// Set the image list
void wxTabCtrl::SetImageList(wxImageList* imageList)
{
    // FIXME stub
}

// Assign (own) the image list
void wxTabCtrl::AssignImageList(wxImageList* imageList)
{
    // FIXME stub
}

// Set the text for an item
bool wxTabCtrl::SetItemText(int idx, const wxString& text)
{
    // FIXME stub
    
    return true;
}

// Set the image for an item
bool wxTabCtrl::SetItemImage(int idx, int image)
{
    // FIXME stub
    
    return false;
}

// Set the size for a fixed-width tab control
void wxTabCtrl::SetItemSize(const wxSize& WXUNUSED(size))
{
    // FIXME stub
    // TODO
}

// Set the padding between tabs
void wxTabCtrl::SetPadding(const wxSize& WXUNUSED(padding))
{
    // FIXME stub
    // TODO
}

wxSize wxTabCtrl::DoGetBestSize() const
{
    // FIXME stub
    
    wxSize empty(1, 1);
    return empty;
}

void wxTabCtrl::OnMouseEvent(wxMouseEvent& event)
{
    // FIXME stub
}

void wxTabCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // FIXME stub
}

void wxTabCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing
}

void wxTabCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // FIXME stub
}

// Set a text badge for the given item
bool wxTabCtrl::SetBadge(int item, const wxString& badge)
{
    // FIXME stub
    
    return true;
}

// Get the text badge for the given item
wxString wxTabCtrl::GetBadge(int item) const
{
    // FIXME stub
    
    return wxEmptyString;
}

bool wxTabCtrl::SetButtonBackgroundColour(const wxColour &colour)
{
    UIView *view = GetPeer()->GetWXWidget();
    if ( !view ) {
        return false;
    }
    
    UIColor *uiColor = [[[UIColor alloc] initWithCGColor:colour.GetCGColor()] autorelease];
    if ( !uiColor ) {
        return false;
    }
    
    if ([view respondsToSelector:@selector(setTintColor:)]) {
        [view performSelector:@selector(setTintColor:)
                   withObject:uiColor];
    }
    
    m_buttonBackgroundColour = colour;
    
    return true;
}

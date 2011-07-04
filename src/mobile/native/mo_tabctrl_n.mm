/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/tabctrl.cpp
// Purpose:     wxMoTabCtrl class
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

#include "wx/mobile/native/tabctrl.h"

#include "wx/imaglist.h"
#include "wx/dcbuffer.h"

#ifndef wxUSE_TAB_DIALOG || !wxUSE_TAB_DIALOG
IMPLEMENT_DYNAMIC_CLASS(wxTabEvent, wxNotifyEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGING)
#endif

IMPLEMENT_DYNAMIC_CLASS(wxMoTabCtrl, wxControl)

BEGIN_EVENT_TABLE(wxMoTabCtrl, wxControl)
    EVT_PAINT(wxMoTabCtrl::OnPaint)
    EVT_MOUSE_EVENTS(wxMoTabCtrl::OnMouseEvent)
    EVT_SYS_COLOUR_CHANGED(wxMoTabCtrl::OnSysColourChanged)
    EVT_SIZE(wxMoTabCtrl::OnSize)
    EVT_ERASE_BACKGROUND(wxMoTabCtrl::OnEraseBackground)
END_EVENT_TABLE()

wxMoTabCtrl::wxMoTabCtrl()
{
    // FIXME stub
}

wxMoTabCtrl::wxMoTabCtrl(wxWindow *parent,
                         wxWindowID id,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    Init();
    
    Create(parent, id, pos, size, style, name);
}

bool wxMoTabCtrl::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    return wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name);
}

wxMoTabCtrl::~wxMoTabCtrl()
{
}

void wxMoTabCtrl::Init()
{
    // FIXME stub
}

// wxEVT_COMMAND_TAB_SEL_CHANGED;
// wxEVT_COMMAND_TAB_SEL_CHANGING;

bool wxMoTabCtrl::SendCommand(wxEventType eventType, int selection)
{
    // FIXME stub
    
    return true;
}

// Responds to colour changes, and passes event on to children.
void wxMoTabCtrl::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    // FIXME stub
}

// Delete all items
bool wxMoTabCtrl::DeleteAllItems()
{
    // FIXME stub

    return true;
}

// Delete an item
bool wxMoTabCtrl::DeleteItem(int item)
{
    // FIXME stub

    return true;
}

// Get the selection
int wxMoTabCtrl::GetSelection() const
{
    // FIXME stub
    
    return 0;
}

// Get the tab with the current keyboard focus
int wxMoTabCtrl::GetCurFocus() const
{
    // FIXME stub
    // TODO
    return -1;
}

// Get the number of items
int wxMoTabCtrl::GetItemCount() const
{
    // FIXME stub

    return 0;
}

// Get the rect corresponding to the tab
bool wxMoTabCtrl::GetItemRect(int WXUNUSED(item), wxRect& WXUNUSED(rect)) const
{
    // FIXME stub
    // TODO
    return false;
}

// Get the number of rows
int wxMoTabCtrl::GetRowCount() const
{
    // FIXME stub
    return 1;
}

// Get the item text
wxString wxMoTabCtrl::GetItemText(int idx) const
{
    // FIXME stub

    return wxEmptyString;
}

// Get the item image
int wxMoTabCtrl::GetItemImage(int idx) const
{
    // FIXME stub

    return 0;
}

// Hit test
int wxMoTabCtrl::HitTest(const wxPoint& WXUNUSED(pt), long& WXUNUSED(flags))
{
    // FIXME stub
    // TODO
    return 0;
}

// Insert an item
bool wxMoTabCtrl::InsertItem(int item, const wxString& text, int imageId)
{
    // FIXME stub

    return true;
}

// Insert an item, passing a bitmap.
bool wxMoTabCtrl::InsertItem(int item, const wxString& text, const wxBitmap& bitmap)
{
    // FIXME stub

    return true;
}

// Add an item
bool wxMoTabCtrl::AddItem(const wxString& text, int imageId)
{
    // FIXME stub

    return true;
}

// Add an item, passing a bitmap.
bool wxMoTabCtrl::AddItem(const wxString& text, const wxBitmap& bitmap)
{
    // FIXME stub

    return true;
}

void wxMoTabCtrl::OnInsertItem(wxMoBarButton* button)
{
    // FIXME stub
}

// Set the selection
int wxMoTabCtrl::SetSelection(int item)
{
    // FIXME stub

    return 0;
}

// Set the selection, without generating events
int wxMoTabCtrl::ChangeSelection(int item)
{
    // FIXME stub

    return 0;
}

// Set the image list
void wxMoTabCtrl::SetImageList(wxImageList* imageList)
{
    // FIXME stub
}

// Assign (own) the image list
void wxMoTabCtrl::AssignImageList(wxImageList* imageList)
{
    // FIXME stub
}

// Set the text for an item
bool wxMoTabCtrl::SetItemText(int idx, const wxString& text)
{
    // FIXME stub

    return true;
}

// Set the image for an item
bool wxMoTabCtrl::SetItemImage(int idx, int image)
{
    // FIXME stub

    return false;
}

// Set the size for a fixed-width tab control
void wxMoTabCtrl::SetItemSize(const wxSize& WXUNUSED(size))
{
    // FIXME stub
    // TODO
}

// Set the padding between tabs
void wxMoTabCtrl::SetPadding(const wxSize& WXUNUSED(padding))
{
    // FIXME stub
    // TODO
}

wxSize wxMoTabCtrl::DoGetBestSize() const
{
    // FIXME stub

    wxSize empty(1, 1);
    return empty;
}

void wxMoTabCtrl::OnMouseEvent(wxMouseEvent& event)
{
    // FIXME stub
}

void wxMoTabCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // FIXME stub
}

void wxMoTabCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing
}

void wxMoTabCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // FIXME stub
}

void wxMoTabCtrl::SetBorderColour(const wxColour &colour)
{
    // FIXME stub
}

wxColour wxMoTabCtrl::GetBorderColour() const
{
    // FIXME stub

    wxColour empty;
    return empty;
}

bool wxMoTabCtrl::SetBackgroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

wxColour wxMoTabCtrl::GetBackgroundColour() const
{
    // FIXME stub

    wxColour empty;
    return empty;
}

bool wxMoTabCtrl::SetButtonBackgroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

wxColour wxMoTabCtrl::GetButtonBackgroundColour() const
{
    // FIXME stub

    wxColour empty;
    return empty;
}

bool wxMoTabCtrl::SetForegroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

wxColour wxMoTabCtrl::GetForegroundColour() const
{
    // FIXME stub

    wxColour empty;
    return empty;
}

bool wxMoTabCtrl::SetFont(const wxFont& font)
{
    // FIXME stub

    return true;
}

wxFont wxMoTabCtrl::GetFont() const
{
    // FIXME stub

    wxFont empty;
    return empty;
}

// Set a text badge for the given item
bool wxMoTabCtrl::SetBadge(int item, const wxString& badge)
{
    // FIXME stub

    return true;
}

// Get the text badge for the given item
wxString wxMoTabCtrl::GetBadge(int item) const
{
    // FIXME stub

    return wxEmptyString;
}

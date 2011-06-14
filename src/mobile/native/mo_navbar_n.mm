/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_navbar_n.mm
// Purpose:     wxMoNavigationBar class
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

#include "wx/dcclient.h"
#include "wx/dcscreen.h"
#include "wx/dcbuffer.h"

#include "wx/mobile/native/navbar.h"
#include "wx/mobile/native/utils.h"

extern WXDLLEXPORT_DATA(const wxChar) wxNavigationBarNameStr[] = wxT("NavigationBar");

IMPLEMENT_DYNAMIC_CLASS(wxMoNavigationBar, wxControl)

BEGIN_EVENT_TABLE(wxMoNavigationBar, wxControl)
    EVT_PAINT(wxMoNavigationBar::OnPaint)
    EVT_SIZE(wxMoNavigationBar::OnSize)
    EVT_MOUSE_EVENTS(wxMoNavigationBar::OnMouseEvent)
    EVT_BUTTON(wxID_BACK, wxMoNavigationBar::OnBack)
    EVT_ERASE_BACKGROUND(wxMoNavigationBar::OnEraseBackground)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxNavigationBarEvent, wxNotifyEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVBAR_POPPED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVBAR_POPPING)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVBAR_PUSHED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVBAR_PUSHING)

bool wxMoNavigationBar::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    // FIXME stub

    return true;
}

wxMoNavigationBar::~wxMoNavigationBar()
{
    // FIXME stub
}

void wxMoNavigationBar::Init()
{
    // FIXME stub
}

wxSize wxMoNavigationBar::DoGetBestSize() const
{
    // FIXME stub

	wxSize empty(1, 1);
	return empty;
}

bool wxMoNavigationBar::SendClickEvent(wxMoBarButton* button)
{
    // FIXME stub

	return true;
}

bool wxMoNavigationBar::SetBackgroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

bool wxMoNavigationBar::SetForegroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

bool wxMoNavigationBar::Enable(bool enable)
{
    // FIXME stub

    return true;
}

bool wxMoNavigationBar::SetFont(const wxFont& font)
{
    // FIXME stub

    return true;
}

void wxMoNavigationBar::OnBack(wxCommandEvent& WXUNUSED(event))
{
    // FIXME stub
}

void wxMoNavigationBar::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	// FIXME stub
}

void wxMoNavigationBar::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing
}

void wxMoNavigationBar::OnMouseEvent(wxMouseEvent& event)
{
    // FIXME stub
}

void wxMoNavigationBar::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // FIXME stub
}

// Pushes an item onto the stack.
bool wxMoNavigationBar::PushItem(wxMoNavigationItem* item)
{
    // FIXME stub
    
    return true;
}

// Pops an item off the stack. The item must then be deleted or stored by the application.
wxMoNavigationItem* wxMoNavigationBar::PopItem()
{
    // FIXME stub

    return NULL;
}

// Returns the top item.
wxMoNavigationItem* wxMoNavigationBar::GetTopItem() const
{
    // FIXME stub

	return NULL;
}

// Returns the back item.
wxMoNavigationItem* wxMoNavigationBar::GetBackItem() const
{
    // FIXME stub

	return NULL;
}

// Sets the item stack.
void wxMoNavigationBar::SetItems(const wxMoNavigationItemArray& items)
{
    // FIXME stub
	// TODO: delete old items?
}

// Clears the item stack, deleting the items.
void wxMoNavigationBar::ClearItems(bool deleteItems)
{
    // FIXME stub
}

wxMoBarButton* wxMoNavigationBar::GetLeftButton()
{
    // FIXME stub

    return NULL;
}

wxMoBarButton* wxMoNavigationBar::GetRightButton()
{
    // FIXME stub

    return NULL;
}

wxString wxMoNavigationBar::GetCurrentTitle()
{
    // FIXME stub

    return wxEmptyString;
}

void wxMoNavigationBar::PositionButtons()
{
    // FIXME stub
}

// Gets the best size for this button
wxSize wxMoNavigationBar::GetBestButtonSize(wxDC& dc, const wxMoBarButton& item) const
{
    // FIXME stub

	wxSize empty(1, 1);
	return empty;
}

// Draw the title
void wxMoNavigationBar::DrawTitle(wxDC& dc)
{
    // FIXME stub
}

// Draw all items
void wxMoNavigationBar::DrawItems(wxDC& dc)
{
    // FIXME stub
}

void wxMoNavigationBar::DrawButton(wxDC& dc, wxMoBarButton& item)
{
    // FIXME stub
}

void wxMoNavigationBar::DrawBackground(wxDC& dc)
{
    // FIXME stub
}

int wxMoNavigationBar::HitTest(const wxPoint& pt, wxMoBarButton** button)
{
    // FIXME stub

    return -1;
}

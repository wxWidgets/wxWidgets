/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_barbutton_n.mm
// Purpose:     wxMoBarButton class
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

#include "wx/mobile/native/barbutton.h"
#include "wx/mobile/native/utils.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/brush.h"
    #include "wx/panel.h"
    #include "wx/bmpbuttn.h"
    #include "wx/settings.h"
    #include "wx/dcscreen.h"
    #include "wx/dcclient.h"
    #include "wx/toplevel.h"
    #include "wx/image.h"
#endif

#include "wx/mstream.h"
#include "wx/arrimpl.cpp"

WX_DEFINE_EXPORTED_OBJARRAY(wxMoBarButtonArray);


#include "wx/stockitem.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoBarButton, wxEvtHandler)

BEGIN_EVENT_TABLE(wxMoBarButton, wxEvtHandler)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

bool wxMoBarButton::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxString& label1,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& WXUNUSED(validator),
                      const wxString& WXUNUSED(name))
{
    // FIXME stub

    return true;
}

bool wxMoBarButton::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxBitmap& bitmap1,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& WXUNUSED(validator),
                      const wxString& WXUNUSED(name))
{
    // FIXME stub

    return true;
}

wxMoBarButton::~wxMoBarButton()
{
}

void wxMoBarButton::Init()
{
    // FIXME stub
}

void wxMoBarButton::Copy(const wxMoBarButton& item)
{
    // FIXME stub
}

wxSize wxMoBarButton::DoGetBestSize() const
{
    // FIXME stub

    wxSize empty(1, 1);
    return empty;
}

wxSize wxMoBarButton::GetBestButtonSize(wxDC& dc) const
{
    // FIXME stub

    wxSize empty(1, 1);
    return empty;
}

wxSize wxMoBarButton::IPGetDefaultSize() const
{
    // FIXME stub

    wxSize empty(1, 1);
    return empty;
}

bool wxMoBarButton::SendClickEvent()
{
    // FIXME stub

    return true;
}

void wxMoBarButton::Draw(wxDC& dc, int flags)
{
    // FIXME stub
}

void wxMoBarButton::ProcessMouseEvent(wxMouseEvent& WXUNUSED(event))
{
    // Processed by the particular class using the bar button
}

// function called when any of the bitmaps changes
void wxMoBarButton::OnSetLabel()
{
}

// Is this a stock item?
bool wxMoBarButton::IsStockId(int id, int barType)
{
    // FIXME stub

    return false;
}

// Get the stock label for the given identifier
wxString wxMoBarButton::GetStockLabel(int id)
{
   // FIXME stub

    return wxEmptyString;
}

// Get the stock bitmap for the given identifier
wxBitmap wxMoBarButton::GetStockBitmap(int id, int barType)
{
    // FIXME stub

    wxBitmap empty;
    return empty;
}

// Get the stock help string for the given identifier
wxString wxMoBarButton::GetStockHelpString(int WXUNUSED(id))
{
    // FIXME stub

    return wxEmptyString;
}

// Get a bitmap, such as a PNG, from raw data
wxBitmap wxMoBarButton::GetBitmapFromData(const unsigned char *data, int length)
{
    // FIXME stub

    wxBitmap empty;
    return empty;
}

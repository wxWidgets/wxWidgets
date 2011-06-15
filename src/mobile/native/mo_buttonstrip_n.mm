/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_buttonstrip_n.mm
// Purpose:     wxMoButtonStrip helper class for tab control,
//              toolbar and segmented control.
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

#include "wx/mobile/native/buttonstrip.h"
#include "wx/mobile/native/utils.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoButtonStrip, wxObject)

#define EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)    (cy+8)

// this macro tries to adjust the default button height to a reasonable value
// using the char height as the base
#define BUTTON_HEIGHT_FROM_CHAR_HEIGHT(cy) (11*EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)/10)

wxMoButtonStrip::~wxMoButtonStrip()
{
    // FIXME stub
}

void wxMoButtonStrip::Init()
{
    // FIXME stub
}

void wxMoButtonStrip::Draw(wxWindow* win, const wxRect& WXUNUSED(rect), wxDC& dc)
{
    // FIXME stub
}

void wxMoButtonStrip::DrawButton(wxWindow* win, wxDC& dc, wxMoBarButton& item, int i)
{
    // FIXME stub
}

void wxMoButtonStrip::DrawBackground(wxWindow* win, wxDC& dc)
{
    // FIXME stub
}

int wxMoButtonStrip::HitTest(const wxPoint& pt) const
{
    // FIXME stub

    return -1;
}

int wxMoButtonStrip::ProcessMouseEvent(wxWindow* win, const wxRect& rect, wxMouseEvent& event)
{
    // FIXME stub

    return -1;
}

int wxMoButtonStrip::AddTextButton(wxWindow* parent, int id, const wxString& text, const wxString& badge)
{
    // FIXME stub

    return -1;
}

int wxMoButtonStrip::AddBitmapButton(wxWindow* parent, int id, const wxBitmap& bitmap, const wxString& text, const wxString& badge)
{
    // FIXME stub

    return -1;
}

int wxMoButtonStrip::AddBitmapButton(wxWindow* parent, int id, int imageIndex, const wxString& text, const wxString& badge)
{
    // FIXME stub

    return -1;
}

int wxMoButtonStrip::AddFlexibleSeparator()
{
    // FIXME stub

    return -1;
}

int wxMoButtonStrip::AddFixedSeparator(int width)
{
    // FIXME stub

    return -1;
}

int wxMoButtonStrip::InsertTextButton(wxWindow* parent, int id, int insertBefore, const wxString& text, const wxString& badge)
{
    // FIXME stub

    return -1;
}

int wxMoButtonStrip::InsertBitmapButton(wxWindow* parent, int id, int insertBefore, const wxBitmap& bitmap1, const wxString& text1, const wxString& badge)
{
    // FIXME stub

    return -1;
}

int wxMoButtonStrip::InsertBitmapButton(wxWindow* parent, int id, int insertBefore, int imageIndex, const wxString& text, const wxString& badge)
{
   // FIXME stub

    return -1;
}

int wxMoButtonStrip::InsertFlexibleSeparator(int insertBefore)
{
    // FIXME stub

    return -1;
}

int wxMoButtonStrip::InsertFixedSeparator(int insertBefore, int width)
{
    // FIXME stub

    return -1;
}

bool wxMoButtonStrip::DeleteButton(size_t idx)
{
    // FIXME stub

    return true;
}

bool wxMoButtonStrip::SetButtonText(size_t idx, const wxString& text)
{
    // FIXME stub

    return true;
}

bool wxMoButtonStrip::SetButtonBadge(size_t idx, const wxString& badge)
{
    // FIXME stub

    return true;
}

bool wxMoButtonStrip::SetButtonNormalBitmap(size_t idx, const wxBitmap& bitmap)
{
    // FIXME stub

    return true;
}

bool wxMoButtonStrip::SetButtonDisabledBitmap(size_t idx, const wxBitmap& bitmap)
{
    // FIXME stub

    return true;
}

bool wxMoButtonStrip::SetButtonSelectedBitmap(size_t idx, const wxBitmap& bitmap)
{
    // FIXME stub

    return true;
}

bool wxMoButtonStrip::SetButtonHighlightedBitmap(size_t idx, const wxBitmap& bitmap)
{
    // FIXME stub

    return true;
}

bool wxMoButtonStrip::EnableButton(size_t idx, bool enable)
{
    // FIXME stub

    return true;
}

bool wxMoButtonStrip::IsButtonEnabled(size_t idx) const
{
    // FIXME stub
    
    return true;
}

bool wxMoButtonStrip::SelectButton(size_t idx, bool selected)
{
    // FIXME stub

    return true;
}

bool wxMoButtonStrip::IsButtonSelected(size_t idx) const
{
    // FIXME stub

    return true;
}

wxMoBarButton* wxMoButtonStrip::GetButton(size_t idx) const
{
    // FIXME stub

    return NULL;
}

int wxMoButtonStrip::FindIndexForId(int id)
{
    // FIXME stub

    return -1;
}

void wxMoButtonStrip::Clear()
{
    // FIXME stub
}

bool wxMoButtonStrip::SetSelection(int sel)
{
    // FIXME stub

    return true;
}

wxSize wxMoButtonStrip::GetBestButtonSize(wxDC& dc, const wxMoBarButton& item) const
{
    // FIXME stub

    wxSize empty(1, 1);
    return empty;
}

wxSize wxMoButtonStrip::GetBestSize() const
{
    // FIXME stub
    
    wxSize sz(0, 0);
    return sz;
}

// Gets the space taking up by just the buttons.
wxSize wxMoButtonStrip::GetTotalButtonSize(wxSize& maxButtonSize, wxSize& totalIncludingMargins) const
{
    // FIXME stub

    wxSize sz(0, 0);
    return sz;
}

bool wxMoButtonStrip::ComputeSizes(wxWindow* win)
{
    // FIXME stub
        
    return true;
}

void wxMoButtonStrip::DoResize(wxWindow* win)
{
    // FIXME stub
}

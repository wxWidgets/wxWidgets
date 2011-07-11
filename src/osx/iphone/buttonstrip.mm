/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/buttonstrip.mm
// Purpose:     wxButtonStrip helper class for tab control,
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

#include "wx/buttonstrip.h"

IMPLEMENT_DYNAMIC_CLASS(wxButtonStrip, wxButtonStripBase)

#define EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)    (cy+8)

// this macro tries to adjust the default button height to a reasonable value
// using the char height as the base
#define BUTTON_HEIGHT_FROM_CHAR_HEIGHT(cy) (11*EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)/10)

wxButtonStrip::~wxButtonStrip()
{
    // FIXME stub
}

void wxButtonStrip::Init()
{
    // FIXME stub
}

void wxButtonStrip::Draw(wxWindow* win, const wxRect& WXUNUSED(rect), wxDC& dc)
{
    // FIXME stub
}

void wxButtonStrip::DrawButton(wxWindow* win, wxDC& dc, wxBarButton& item, int i)
{
    // FIXME stub
}

void wxButtonStrip::DrawBackground(wxWindow* win, wxDC& dc)
{
    // FIXME stub
}

int wxButtonStrip::HitTest(const wxPoint& pt) const
{
    // FIXME stub
    
    return -1;
}

int wxButtonStrip::ProcessMouseEvent(wxWindow* win, const wxRect& rect, wxMouseEvent& event)
{
    // FIXME stub
    
    return -1;
}

int wxButtonStrip::AddTextButton(wxWindow* parent, int id, const wxString& text, const wxString& badge)
{
    // FIXME stub
    
    return -1;
}

int wxButtonStrip::AddBitmapButton(wxWindow* parent, int id, const wxBitmap& bitmap, const wxString& text, const wxString& badge)
{
    // FIXME stub
    
    return -1;
}

int wxButtonStrip::AddBitmapButton(wxWindow* parent, int id, int imageIndex, const wxString& text, const wxString& badge)
{
    // FIXME stub
    
    return -1;
}

int wxButtonStrip::AddFlexibleSeparator()
{
    // FIXME stub
    
    return -1;
}

int wxButtonStrip::AddFixedSeparator(int width)
{
    // FIXME stub
    
    return -1;
}

int wxButtonStrip::InsertTextButton(wxWindow* parent, int id, int insertBefore, const wxString& text, const wxString& badge)
{
    // FIXME stub
    
    return -1;
}

int wxButtonStrip::InsertBitmapButton(wxWindow* parent, int id, int insertBefore, const wxBitmap& bitmap1, const wxString& text1, const wxString& badge)
{
    // FIXME stub
    
    return -1;
}

int wxButtonStrip::InsertBitmapButton(wxWindow* parent, int id, int insertBefore, int imageIndex, const wxString& text, const wxString& badge)
{
    // FIXME stub
    
    return -1;
}

int wxButtonStrip::InsertFlexibleSeparator(int insertBefore)
{
    // FIXME stub
    
    return -1;
}

int wxButtonStrip::InsertFixedSeparator(int insertBefore, int width)
{
    // FIXME stub
    
    return -1;
}

bool wxButtonStrip::DeleteButton(size_t idx)
{
    // FIXME stub
    
    return true;
}

bool wxButtonStrip::SetButtonText(size_t idx, const wxString& text)
{
    // FIXME stub
    
    return true;
}

bool wxButtonStrip::SetButtonBadge(size_t idx, const wxString& badge)
{
    // FIXME stub
    
    return true;
}

bool wxButtonStrip::SetButtonNormalBitmap(size_t idx, const wxBitmap& bitmap)
{
    // FIXME stub
    
    return true;
}

bool wxButtonStrip::SetButtonDisabledBitmap(size_t idx, const wxBitmap& bitmap)
{
    // FIXME stub
    
    return true;
}

bool wxButtonStrip::SetButtonSelectedBitmap(size_t idx, const wxBitmap& bitmap)
{
    // FIXME stub
    
    return true;
}

bool wxButtonStrip::SetButtonHighlightedBitmap(size_t idx, const wxBitmap& bitmap)
{
    // FIXME stub
    
    return true;
}

bool wxButtonStrip::EnableButton(size_t idx, bool enable)
{
    // FIXME stub
    
    return true;
}

bool wxButtonStrip::IsButtonEnabled(size_t idx) const
{
    // FIXME stub
    
    return true;
}

bool wxButtonStrip::SelectButton(size_t idx, bool selected)
{
    // FIXME stub
    
    return true;
}

bool wxButtonStrip::IsButtonSelected(size_t idx) const
{
    // FIXME stub
    
    return true;
}

wxBarButton* wxButtonStrip::GetButton(size_t idx) const
{
    // FIXME stub
    
    return NULL;
}

int wxButtonStrip::FindIndexForId(int id)
{
    // FIXME stub
    
    return -1;
}

void wxButtonStrip::Clear()
{
    // FIXME stub
}

bool wxButtonStrip::SetSelection(int sel)
{
    // FIXME stub
    
    return true;
}

wxSize wxButtonStrip::GetBestButtonSize(wxDC& dc, const wxBarButton& item) const
{
    // FIXME stub
    
    wxSize empty(1, 1);
    return empty;
}

wxSize wxButtonStrip::GetBestSize() const
{
    // FIXME stub
    
    wxSize sz(0, 0);
    return sz;
}

// Gets the space taking up by just the buttons.
wxSize wxButtonStrip::GetTotalButtonSize(wxSize& maxButtonSize, wxSize& totalIncludingMargins) const
{
    // FIXME stub
    
    wxSize sz(0, 0);
    return sz;
}

bool wxButtonStrip::ComputeSizes(wxWindow* win)
{
    // FIXME stub
    
    return true;
}

void wxButtonStrip::DoResize(wxWindow* win)
{
    // FIXME stub
}

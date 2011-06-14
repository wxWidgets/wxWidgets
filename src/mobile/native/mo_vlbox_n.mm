/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_vlbox_n.mm
// Purpose:     wxMoVListBox class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>, Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/mobile/native/vlbox.h"

#include "wx/dcbuffer.h"
#include "wx/selstore.h"
#include "wx/settings.h"
#include "wx/listbox.h"

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxMoVListBox, wxMoVScrolledWindow)
    EVT_PAINT(wxMoVListBox::OnPaint)

    EVT_KEY_DOWN(wxMoVListBox::OnKeyDown)
    EVT_LEFT_DOWN(wxMoVListBox::OnLeftDown)
    EVT_LEFT_DCLICK(wxMoVListBox::OnLeftDClick)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_ABSTRACT_CLASS(wxMoVListBox, wxMoVScrolledWindow)

// ----------------------------------------------------------------------------
// wxMoVListBox creation
// ----------------------------------------------------------------------------

void wxMoVListBox::Init()
{
    m_current =
    m_anchor = wxNOT_FOUND;
    m_selStore = NULL;
}

bool wxMoVListBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name)
{
    // FIXME stub

    return true;
}

wxMoVListBox::~wxMoVListBox()
{
    // FIXME stub
}

void wxMoVListBox::SetItemCount(size_t count)
{
    // FIXME stub
}

// ----------------------------------------------------------------------------
// selection handling
// ----------------------------------------------------------------------------

bool wxMoVListBox::IsSelected(size_t line) const
{
    // FIXME stub
	return true;
}

bool wxMoVListBox::Select(size_t item, bool select)
{
    // FIXME stub

	return true;
}

bool wxMoVListBox::SelectRange(size_t from, size_t to)
{
    // FIXME stub

    return true;
}

bool wxMoVListBox::DoSelectAll(bool select)
{
    // FIXME stub

    return false;
}

bool wxMoVListBox::DoSetCurrent(int current)
{
    // FIXME stub

    return true;
}

void wxMoVListBox::SendSelectedEvent()
{
    // FIXME stub
}

void wxMoVListBox::SetSelection(int selection)
{
    // FIXME stub
}

size_t wxMoVListBox::GetSelectedCount() const
{
    // FIXME stub

	return 0;
}

int wxMoVListBox::GetFirstSelected(unsigned long& cookie) const
{
    // FIXME stub

	return 0;
}

int wxMoVListBox::GetNextSelected(unsigned long& cookie) const
{
    // FIXME stub

	return 0;
}

// ----------------------------------------------------------------------------
// wxMoVListBox appearance parameters
// ----------------------------------------------------------------------------

void wxMoVListBox::SetMargins(const wxPoint& pt)
{
    // FIXME stub
}

void wxMoVListBox::SetSelectionBackground(const wxColour& col)
{
    // FIXME stub
}

// ----------------------------------------------------------------------------
// wxMoVListBox painting
// ----------------------------------------------------------------------------

wxCoord wxMoVListBox::OnGetLineHeight(size_t line) const
{
    // FIXME stub
	return NULL;
}

void wxMoVListBox::OnDrawSeparator(wxDC& WXUNUSED(dc),
                                 wxRect& WXUNUSED(rect),
                                 size_t WXUNUSED(n)) const
{
}

void wxMoVListBox::OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const
{
    // FIXME stub
}

void wxMoVListBox::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // FIXME stub
}

// ============================================================================
// wxMoVListBox keyboard/mouse handling
// ============================================================================

void wxMoVListBox::DoHandleItemClick(int item, int flags)
{
    // FIXME stub
}

// ----------------------------------------------------------------------------
// keyboard handling
// ----------------------------------------------------------------------------

void wxMoVListBox::OnKeyDown(wxKeyEvent& event)
{
    // FIXME stub
}

// ----------------------------------------------------------------------------
// wxMoVListBox mouse handling
// ----------------------------------------------------------------------------

void wxMoVListBox::OnLeftDown(wxMouseEvent& event)
{
    // FIXME stub
}

void wxMoVListBox::OnLeftDClick(wxMouseEvent& eventMouse)
{
    // FIXME stub
}


// ----------------------------------------------------------------------------
// use the same default attributes as wxListBox
// ----------------------------------------------------------------------------

//static
wxVisualAttributes
wxMoVListBox::GetClassDefaultAttributes(wxWindowVariant variant)
{
    // FIXME stub

	wxVisualAttributes empty;
	return empty;
}

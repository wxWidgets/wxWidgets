/////////////////////////////////////////////////////////////////////////////
// Name:        univ/scrolbar.cpp
// Purpose:     wxScrollBar implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univscrolbar.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SCROLLBAR

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/scrolbar.h"
    #include "wx/validate.h"
#endif

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/theme.h"

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxScrollBar, wxControl)

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

void wxScrollBar::Init()
{
    m_range =
    m_thumbSize =
    m_thumbPos =
    m_pageSize = 0;

    for ( size_t n = 0; n < Element_Max; n++ )
    {
        m_elementsState[n] = 0;
    }
}

bool wxScrollBar::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxPoint &pos,
                         const wxSize &size,
                         long style,
                         const wxValidator& validator,
                         const wxString &name)
{
    // the scrollbars always have the standard border so far
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_SUNKEN;

    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    if ( size.x == -1 || size.y == -1 )
    {
        wxSize sizeBest = DoGetBestSize();
        SetSize(size.x == -1 ? sizeBest.x : size.x,
                size.y == -1 ? sizeBest.y : size.y);
    }

    return TRUE;
}

wxScrollBar::~wxScrollBar()
{
}

// ----------------------------------------------------------------------------
// scrollbar API
// ----------------------------------------------------------------------------

void wxScrollBar::DoSetThumb(int pos)
{
    // don't do checks here, we're a private function
    if ( pos < 0 )
    {
        pos = 0;
    }
    else if ( pos > m_range - m_thumbSize )
    {
        pos = m_range - m_thumbSize;
    }

    m_thumbPos = pos;
}

int wxScrollBar::GetThumbPosition() const
{
    return m_thumbPos;
}

int wxScrollBar::GetThumbSize() const
{
    return m_thumbSize;
}

int wxScrollBar::GetPageSize() const
{
    return m_pageSize;
}

int wxScrollBar::GetRange() const
{
    return m_range;
}

void wxScrollBar::SetThumbPosition(int pos)
{
    wxCHECK_RET( pos >= 0 && pos <= m_range, _T("thumb position out of range") );

    DoSetThumb(pos);

    Refresh();
}

void wxScrollBar::SetScrollbar(int position, int thumbSize,
                               int range, int pageSize,
                               bool refresh)
{
    // set all parameters
    m_range = range;
    m_thumbSize = thumbSize;
    SetThumbPosition(position);
    m_pageSize = pageSize;

    if ( refresh )
    {
        // and update the window
        Refresh();
    }
}

// ----------------------------------------------------------------------------
// size management
// ----------------------------------------------------------------------------

wxSize wxScrollBar::DoGetBestSize() const
{
    wxSize sz(140, 140);
    wxTheme::Get()->GetRenderer()->AdjustSize(&sz, this);
    return sz;
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

void wxScrollBar::DoDraw(wxControlRenderer *renderer)
{
    renderer->DrawScrollbar(this);
}

// ----------------------------------------------------------------------------
// input processing
// ----------------------------------------------------------------------------

bool wxScrollBar::PerformAction(const wxControlAction& action,
                                const wxEvent& event)
{
    if ( action == wxACTION_SCROLL_START )
        ScrollToStart();
    else if ( action == wxACTION_SCROLL_END )
        ScrollToEnd();
    else if ( action == wxACTION_SCROLL_LINE_UP )
        ScrollLines(-1);
    else if ( action == wxACTION_SCROLL_LINE_DOWN )
        ScrollLines(1);
    else if ( action == wxACTION_SCROLL_PAGE_UP )
        ScrollPages(-1);
    else if ( action == wxACTION_SCROLL_PAGE_DOWN )
        ScrollPages(1);
    else
        return wxControl::PerformAction(action, event);

    // scrollbar position changed - update
    return TRUE;
}

void wxScrollBar::ScrollToStart()
{
    DoSetThumb(0);
}

void wxScrollBar::ScrollToEnd()
{
    DoSetThumb(m_range - m_thumbSize);
}

void wxScrollBar::ScrollLines(int nLines)
{
    DoSetThumb(m_thumbPos + nLines);
}

void wxScrollBar::ScrollPages(int nPages)
{
    DoSetThumb(m_thumbPos + nPages*m_pageSize);
}

#endif // wxUSE_SCROLLBAR


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

    if ( pos >= m_range - m_thumbSize )
    {
        pos = m_range - m_thumbSize;
    }

    m_thumbPos = pos;
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
    int thumbStart, thumbEnd;
    if ( m_range )
    {
        thumbStart = (100*m_thumbPos) / m_range;
        thumbEnd = (100*(m_thumbPos + m_thumbSize)) / m_range;
    }
    else // no range
    {
        thumbStart =
        thumbEnd = 0;
    }

    renderer->DrawScrollbar(thumbStart, thumbEnd);
}

// ----------------------------------------------------------------------------
// input processing
// ----------------------------------------------------------------------------

wxInputHandler *wxScrollBar::CreateInputHandler() const
{
    return wxTheme::Get()->GetInputHandler(wxCONTROL_SCROLLBAR);
}

bool wxScrollBar::PerformAction(const wxControlAction& action,
                                const wxEvent& event)
{
    return wxControl::PerformAction(action, event);
}

#endif // wxUSE_SCROLLBAR


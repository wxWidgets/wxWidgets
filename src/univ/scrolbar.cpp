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

    for ( size_t n = 0; n < WXSIZEOF(m_elementsState); n++ )
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

    SetBestSize(size);

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
    // completely arbitrary
    return AdjustSize(wxSize(140, 140));
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

wxCoord wxScrollBar::GetMouseCoord(const wxEvent& eventOrig) const
{
    const wxMouseEvent& event = (const wxMouseEvent&)eventOrig;
    wxPoint pt = event.GetPosition();
    return GetWindowStyle() & wxVERTICAL ? pt.y : pt.x;
}

bool wxScrollBar::PerformAction(const wxControlAction& action,
                                const wxEvent& event)
{
    int thumbOld = m_thumbPos;

    bool notify = FALSE; // send an event about the change?

    wxEventType scrollType;

    // test for thumb move first as these events happen in quick succession
    if ( action == wxACTION_SCROLL_THUMB_MOVE )
    {
        // make the thumb follow the mouse by keeping the same offset between
        // the mouse position and the top/left of the thumb
        int thumbPos = GetMouseCoord(event) - m_ofsMouse;
        DoSetThumb(GetRenderer()->PixelToScrollbar(this, thumbPos));

        scrollType = wxEVT_SCROLLWIN_THUMBTRACK;
    }
    else if ( action == wxACTION_SCROLL_LINE_UP )
    {
        scrollType = wxEVT_SCROLLWIN_LINEUP;
        ScrollLines(-1);
    }
    else if ( action == wxACTION_SCROLL_LINE_DOWN )
    {
        scrollType = wxEVT_SCROLLWIN_LINEDOWN;
        ScrollLines(1);
    }
    else if ( action == wxACTION_SCROLL_PAGE_UP )
    {
        scrollType = wxEVT_SCROLLWIN_PAGEUP;
        ScrollPages(-1);
    }
    else if ( action == wxACTION_SCROLL_PAGE_DOWN )
    {
        scrollType = wxEVT_SCROLLWIN_PAGEDOWN;
        ScrollPages(1);
    }
    else if ( action == wxACTION_SCROLL_START )
    {
        scrollType = wxEVT_SCROLLWIN_THUMBRELEASE; // anything better?
        ScrollToStart();
    }
    else if ( action == wxACTION_SCROLL_END )
    {
        scrollType = wxEVT_SCROLLWIN_THUMBRELEASE; // anything better?
        ScrollToEnd();
    }
    else if ( action == wxACTION_SCROLL_THUMB_DRAG )
    {
        m_ofsMouse = GetMouseCoord(event) -
                        GetRenderer()->ScrollbarToPixel(this);
    }
    else if ( action == wxACTION_SCROLL_THUMB_RELEASE )
    {
        // always notify about this
        notify = TRUE;
        scrollType = wxEVT_SCROLLWIN_THUMBRELEASE;
    }
    else
        return wxControl::PerformAction(action, event);

    // has scrollbar position changed?
    bool changed = m_thumbPos != thumbOld;
    if ( notify || changed )
    {
        wxScrollWinEvent event(scrollType, m_thumbPos,
                               IsVertical() ? wxVERTICAL : wxHORIZONTAL);
        event.SetEventObject(this);
        GetParent()->GetEventHandler()->ProcessEvent(event);
    }

    // refresh if something changed
    return changed;
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


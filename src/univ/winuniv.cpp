///////////////////////////////////////////////////////////////////////////////
// Name:        univ/window.cpp
// Purpose:     implementation of extra wxWindow methods for wxUniv port
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univwindow.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/window.h"
    #include "wx/dcclient.h"
    #include "wx/event.h"
    #include "wx/scrolbar.h"
#endif // WX_PRECOMP

#include "wx/univ/renderer.h"
#include "wx/univ/theme.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxWindowBase)

BEGIN_EVENT_TABLE(wxWindow, wxWindowBase)
    EVT_SIZE(wxWindow::OnSize)

    EVT_PAINT(wxWindow::OnPaint)
    EVT_ERASE_BACKGROUND(wxWindow::OnErase)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

void wxWindow::Init()
{
    m_scrollbarVert =
    m_scrollbarHorz = (wxScrollBar *)NULL;

    m_isCurrent = FALSE;
}

// ----------------------------------------------------------------------------
// background pixmap
// ----------------------------------------------------------------------------

void wxWindow::SetBackground(const wxBitmap& bitmap,
                              int alignment,
                              wxStretch stretch)
{
    m_bitmapBg = bitmap;
    m_alignBgBitmap = alignment;
    m_stretchBgBitmap = stretch;
}

const wxBitmap& wxWindow::GetBackgroundBitmap(int *alignment,
                                               wxStretch *stretch) const
{
    if ( m_bitmapBg.Ok() )
    {
        if ( alignment )
            *alignment = m_alignBgBitmap;
        if ( stretch )
            *stretch = m_stretchBgBitmap;
    }

    return m_bitmapBg;
}

// ----------------------------------------------------------------------------
// painting
// ----------------------------------------------------------------------------

wxRenderer *wxWindow::GetRenderer() const
{
    return wxTheme::Get()->GetRenderer();
}

// the event handler executed when the window background must be painted
void wxWindow::OnErase(wxEraseEvent& event)
{
    wxControlRenderer renderer(this, *event.GetDC(),
                               wxTheme::Get()->GetRenderer());

    if ( !DoDrawBackground(&renderer) )
    {
        // not processed
        event.Skip();
    }
}

// the event handler executed when the window must be repainted
void wxWindow::OnPaint(wxPaintEvent& event)
{
    // get the DC to use and create renderer on it
    wxPaintDC dc(this);
    wxControlRenderer renderer(this, dc, GetRenderer());

    // do draw the control!
    DoDraw(&renderer);
}

bool wxWindow::DoDrawBackground(wxControlRenderer *renderer)
{
    if ( !m_bitmapBg.Ok() )
        return FALSE;

    renderer->DrawBackgroundBitmap();

    return TRUE;
}

void wxWindow::DoDraw(wxControlRenderer *renderer)
{
    renderer->DrawBorder();
}

// ----------------------------------------------------------------------------
// state flags
// ----------------------------------------------------------------------------

bool wxWindow::IsFocused() const
{
    wxWindow *self = wxConstCast(this, wxWindow);
    return self->FindFocus() == self;
}

bool wxWindow::IsPressed() const
{
    return FALSE;
}

bool wxWindow::IsDefault() const
{
    return FALSE;
}

bool wxWindow::IsCurrent() const
{
    return m_isCurrent;
}

void wxWindow::SetCurrent(bool doit)
{
    m_isCurrent = doit;
}

int wxWindow::GetStateFlags() const
{
    int flags = 0;
    if ( !IsEnabled() )
        flags |= wxCONTROL_DISABLED;

    // the following states are only possible if our application is active - if
    // it is not, even our default/focused controls shouldn't appear as such
    if ( wxTheApp->IsActive() )
    {
        if ( IsCurrent() )
            flags |= wxCONTROL_CURRENT;
        if ( IsFocused() )
            flags |= wxCONTROL_FOCUSED;
        if ( IsPressed() )
            flags |= wxCONTROL_PRESSED;
        if ( IsDefault() )
            flags |= wxCONTROL_ISDEFAULT;
    }

    return flags;
}

// ----------------------------------------------------------------------------
// size
// ----------------------------------------------------------------------------

void wxWindow::OnSize(wxSizeEvent& event)
{
    if ( m_scrollbarVert || m_scrollbarHorz )
    {
        PositionScrollbars();
    }

    event.Skip();
}

wxSize wxWindow::AdjustSize(const wxSize& size) const
{
    wxSize sz = size;
    wxTheme::Get()->GetRenderer()->AdjustSize(&sz, this);
    return sz;
}

void wxWindow::DoSetClientSize(int width, int height)
{
    // take into account the scrollbars
    if ( m_scrollbarVert )
        width += m_scrollbarVert->GetSize().x;

    if ( m_scrollbarHorz )
        height += m_scrollbarHorz->GetSize().y;

    wxWindowNative::DoSetClientSize(width, height);
}

void wxWindow::DoGetClientSize(int *width, int *height) const
{
    wxWindowNative::DoGetClientSize(width, height);

    if ( width && m_scrollbarVert )
        *width -= m_scrollbarVert->GetSize().x;

    if ( height && m_scrollbarHorz )
        *height -= m_scrollbarHorz->GetSize().y;
}

// ----------------------------------------------------------------------------
// scrolling: we implement it entirely ourselves except for ScrollWindow()
// function which is supposed to be (efficiently) implemented by the native
// window class
// ----------------------------------------------------------------------------

void wxWindow::PositionScrollbars()
{
    wxCoord x, y;
    DoGetSize(&x, &y);

    int width = m_scrollbarVert ? m_scrollbarVert->GetSize().x : 0;
    int height = m_scrollbarHorz ? m_scrollbarHorz->GetSize().y : 0;

    if ( m_scrollbarVert )
        m_scrollbarVert->SetSize(x - width, 0, width, y - height);
    if ( m_scrollbarHorz )
        m_scrollbarHorz->SetSize(0, y - height, x - width, height);
}

void wxWindow::SetScrollbar(int orient,
                            int pos,
                            int thumb,
                            int range,
                            bool refresh)
{
    wxScrollBar *scrollbar = GetScrollbar(orient);
    if ( !scrollbar )
    {
        // create it
        scrollbar = new wxScrollBar(this, -1,
                                    wxDefaultPosition, wxDefaultSize,
                                    orient & wxVERTICAL ? wxSB_VERTICAL
                                                        : wxSB_HORIZONTAL);
        if ( orient & wxVERTICAL )
            m_scrollbarVert = scrollbar;
        else
            m_scrollbarHorz = scrollbar;

        PositionScrollbars();
    }

    scrollbar->SetScrollbar(pos, thumb, range, thumb, refresh);
}

void wxWindow::SetScrollPos(int orient, int pos, bool refresh)
{
    wxScrollBar *scrollbar = GetScrollbar(orient);
    wxCHECK_RET( scrollbar, _T("no scrollbar to set position for") );

    scrollbar->SetThumbPosition(pos);
    if ( refresh )
        Refresh();
}

int wxWindow::GetScrollPos(int orient) const
{
    wxScrollBar *scrollbar = GetScrollbar(orient);
    return scrollbar ? scrollbar->GetThumbPosition() : 0;
}

int wxWindow::GetScrollThumb(int orient) const
{
    wxScrollBar *scrollbar = GetScrollbar(orient);
    return scrollbar ? scrollbar->GetThumbSize() : 0;
}

int wxWindow::GetScrollRange(int orient) const
{
    wxScrollBar *scrollbar = GetScrollbar(orient);
    return scrollbar ? scrollbar->GetRange() : 0;
}

void wxWindow::ScrollWindow(int dx, int dy, const wxRect *rect)
{
    wxASSERT_MSG( !rect, _T("scrolling only part of window not implemented") );

    if ( !dx && !dy )
    {
        // nothing to do
        return;
    }

    // calculate the part of the window which we can just redraw in the new
    // location
    wxSize sizeTotal = GetClientSize();
    wxPoint ptSource, ptDest;
    wxSize size;
    size.x = sizeTotal.x - dx;
    size.y = sizeTotal.y - dy;
    if ( size.x < 0 || size.y < 0 )
    {
        // just redraw everything as nothing of the displayed image will stay
        Refresh();
    }
    else // move the part which doesn't change to the new location
    {
        // positive values mean to scroll to thr right/down
        if ( dx > 0 )
        {
            ptSource.x = 0;
            ptDest.x = dx;
        }
        else
        {
            ptSource.x = dx;
            ptDest.x = 0;
        }

        if ( dy > 0 )
        {
            ptSource.y = 0;
            ptDest.y = dy;
        }
        else
        {
            ptSource.y = dy;
            ptDest.y = 0;
        }

        // do move
        wxClientDC dc(this);
        dc.Blit(ptDest, size, &dc, ptSource);

        // and now repaint the uncovered area

        // FIXME: we repaint the intersection of these rectangles twice - is
        //        it bad?

        wxRect rect;
        rect.x = ptSource.x;
        rect.y = ptSource.y;

        if ( dx )
        {
            rect.width = abs(ptDest.x - ptSource.x);
            rect.height = sizeTotal.y;

            Refresh(TRUE /* erase bkgnd */, &rect);
        }

        if ( dy )
        {
            rect.width = sizeTotal.x;
            rect.height = abs(ptDest.y - ptSource.y);

            Refresh(TRUE /* erase bkgnd */, &rect);
        }
    }
}


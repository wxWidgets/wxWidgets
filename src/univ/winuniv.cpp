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
    #include "wx/dcmemory.h"
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

// we can't use wxWindowNative here as it won't be expanded inside the macro
#if defined(__WXMSW__)
    IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxWindowMSW)
#elif defined(__WXGTK__)
    IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxWindowGTK)
#endif

BEGIN_EVENT_TABLE(wxWindow, wxWindowNative)
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

    m_renderer = (wxRenderer *)NULL;
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
    if ( !m_renderer )
    {
        // it is a native control which paints itself
        event.Skip();
    }
    else
    {
        // get the DC to use and create renderer on it
        wxPaintDC dc(this);
        wxControlRenderer renderer(this, dc, m_renderer);

        // do draw the control!
        DoDraw(&renderer);
    }
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

wxSize wxWindow::DoGetBestSize() const
{
    return AdjustSize(DoGetBestClientSize());
}

wxSize wxWindow::DoGetBestClientSize() const
{
    return wxWindowNative::DoGetBestSize();
}

wxSize wxWindow::AdjustSize(const wxSize& size) const
{
    wxSize sz = size;
    if ( m_renderer )
        m_renderer->AdjustSize(&sz, this);
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

    wxLogTrace(_T("scroll"), _T("window is %dx%d, scroll by %d, %d"),
               sizeTotal.x, sizeTotal.y, dx, dy);

    wxPoint ptSource, ptDest;
    wxSize size;
    size.x = sizeTotal.x - abs(dx);
    size.y = sizeTotal.y - abs(dy);
    if ( size.x < 0 || size.y < 0 )
    {
        // just redraw everything as nothing of the displayed image will stay
        wxLogTrace(_T("scroll"), _T("refreshing everything"));

        Refresh();
    }
    else // move the part which doesn't change to the new location
    {
        // note that when we scroll the canvas in some direction we move the
        // block which doesn't need to be refreshed in the opposite direction

        if ( dx < 0 )
        {
            // scroll to the right, move to the left
            ptSource.x = -dx;
            ptDest.x = 0;
        }
        else
        {
            // scroll to the left, move to the right
            ptSource.x = 0;
            ptDest.x = dx;
        }

        if ( dy < 0 )
        {
            // scroll down, move up
            ptSource.y = -dy;
            ptDest.y = 0;
        }
        else
        {
            // scroll up, move down
            ptSource.y = 0;
            ptDest.y = dy;
        }

        // do move
        wxClientDC dc(this);
        wxBitmap bmp(size.x, size.y);
        wxMemoryDC dcMem;
        dcMem.SelectObject(bmp);
        dcMem.Blit(wxPoint(0, 0), size, &dc, ptSource);
        dc.Blit(ptDest, size, &dcMem, wxPoint(0, 0));

        wxLogTrace(_T("scroll"),
                   _T("Blit: (%d, %d) of size %dx%d -> (%d, %d)"),
                   ptSource.x, ptSource.y,
                   size.x, size.y,
                   ptDest.x, ptDest.y);

        // and now repaint the uncovered area

        // FIXME: we repaint the intersection of these rectangles twice - is
        //        it bad?

        wxRect rect;

        if ( dx )
        {
            if ( dx < 0 )
            {
                // refresh the area along the right border
                rect.x = size.x;
                rect.width = -dx;
            }
            else
            {
                // refresh the area along the left border
                rect.x = 0;
                rect.width = dx;
            }

            rect.y = 0;
            rect.height = sizeTotal.y;

            wxLogTrace(_T("scroll"), _T("refreshing (%d, %d)-(%d, %d)"),
                       rect.x, rect.y, rect.GetRight(), rect.GetBottom());

            Refresh(TRUE /* erase bkgnd */, &rect);
        }

        if ( dy )
        {
            if ( dy < 0 )
            {
                // refresh the area along the bottom border
                rect.y = size.y;
                rect.height = -dy;
            }
            else
            {
                // refresh the area along the top border
                rect.y = 0;
                rect.height = dy;
            }

            rect.x = 0;
            rect.width = sizeTotal.x;

            wxLogTrace(_T("scroll"), _T("refreshing (%d, %d)-(%d, %d)"),
                       rect.x, rect.y, rect.GetRight(), rect.GetBottom());

            Refresh(TRUE /* erase bkgnd */, &rect);
        }
    }
}


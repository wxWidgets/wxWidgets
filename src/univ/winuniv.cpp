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

#include "wx/univ/colschem.h"
#include "wx/univ/renderer.h"
#include "wx/univ/theme.h"

#if wxUSE_CARET
    #include "wx/caret.h"
#endif // wxUSE_CARET

// turn Refresh() debugging on/off
#define WXDEBUG_REFRESH

#ifndef __WXDEBUG__
    #undef WXDEBUG_REFRESH
#endif

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
    EVT_NC_PAINT(wxWindow::OnNcPaint)
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

    m_renderer = wxTheme::Get()->GetRenderer();
}

bool wxWindow::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
{
    // we add wxCLIP_CHILDREN and wxNO_FULL_REPAINT_ON_RESIZE because without
    // these styles we can't get rid of flicker on wxMSW
    if ( !wxWindowNative::Create(parent, id, pos, size,
                                 style |
                                 wxCLIP_CHILDREN |
                                 wxNO_FULL_REPAINT_ON_RESIZE, name) )
    {
        return FALSE;
    }

    // if we should always have the scrollbar, do show it
    if ( GetWindowStyle() & wxALWAYS_SHOW_SB )
    {
        m_scrollbarVert = new wxScrollBar(this, -1,
                                          wxDefaultPosition, wxDefaultSize,
                                          wxSB_VERTICAL);

        // and position it
        PositionScrollbars();
    }

    // the colours/fonts are default
    m_hasBgCol =
    m_hasFgCol =
    m_hasFont = FALSE;

    return TRUE;
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
    if ( !m_renderer )
    {
        event.Skip();

        return;
    }

    DoDrawBackground(*event.GetDC());

    // if we have both scrollbars, we also have a square in the corner between
    // them which we must paint
    if ( m_scrollbarVert && m_scrollbarHorz )
    {
        wxSize size = GetSize();
        wxRect rectClient = GetClientRect(),
               rectBorder = m_renderer->GetBorderDimensions(GetBorder());

        wxRect rectCorner;
        rectCorner.x = rectClient.GetRight() + 1;
        rectCorner.y = rectClient.GetBottom() + 1;
        rectCorner.SetRight(size.x - rectBorder.width);
        rectCorner.SetBottom(size.y - rectBorder.height);

        if ( GetUpdateRegion().Contains(rectCorner) )
        {
            m_renderer->DrawScrollCorner(*event.GetDC(), rectCorner);
        }
    }
}

// the event handlers executed when the window must be repainted
void wxWindow::OnNcPaint(wxPaintEvent& event)
{
    if ( m_renderer )
    {
        // get the window rect
        wxRect rect;
        wxSize size = GetSize();
        rect.x =
        rect.y = 0;
        rect.width = size.x;
        rect.height = size.y;

        // if the scrollbars are outside the border, we must adjust the rect to
        // exclude them
        if ( !m_renderer->AreScrollbarsInsideBorder() )
        {
            wxScrollBar *scrollbar = GetScrollbar(wxVERTICAL);
            if ( scrollbar )
                rect.width -= scrollbar->GetSize().x;

            scrollbar = GetScrollbar(wxHORIZONTAL);
            if ( scrollbar )
                rect.height -= scrollbar->GetSize().y;
        }

        // get the DC and draw the border on it
        wxWindowDC dc(this);
        DoDrawBorder(dc, rect);
    }
}

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

        // draw the control
        DoDraw(&renderer);
    }
}

bool wxWindow::DoDrawBackground(wxDC& dc)
{
    // FIXME: leaving this code in leads to partial bg redraws sometimes under
    //        MSW
    wxRect rect;
#ifndef __WXMSW__
    rect = GetUpdateRegion().GetBox();
    if ( !rect.width && !rect.height )
#endif
    {
        wxSize size = GetSize();
        rect.width = size.x;
        rect.height = size.y;
    }

    if ( GetBackgroundBitmap().Ok() )
    {
        // get the bitmap and the flags
        int alignment;
        wxStretch stretch;
        wxBitmap bmp = GetBackgroundBitmap(&alignment, &stretch);
        wxControlRenderer::DrawBitmap(dc, bmp, rect, alignment, stretch);
    }
    else // just fill it with bg colour if no bitmap
    {
        m_renderer->DrawBackground(dc, wxTHEME_BG_COLOUR(this),
                                   rect, GetStateFlags());
    }

    return TRUE;
}

void wxWindow::DoDrawBorder(wxDC& dc, const wxRect& rect)
{
    // draw outline unless the update region is enitrely inside it in which
    // case we don't need to do it
#if 0 // doesn't seem to work, why?
    if ( wxRegion(rect).Contains(GetUpdateRegion().GetBox()) != wxInRegion )
#endif
    {
        m_renderer->DrawBorder(dc, GetBorder(), rect, GetStateFlags());
    }
}

void wxWindow::DoDraw(wxControlRenderer *renderer)
{
}

void wxWindow::Refresh(bool eraseBackground, const wxRect *rectClient)
{
    wxRect rectWin;
    wxPoint pt = GetClientAreaOrigin();

    wxSize size = GetClientSize();

    if ( rectClient )
    {
        rectWin = *rectClient;

        // don't refresh anything beyond the client area (scrollbars for
        // example)
        if ( rectWin.GetRight() > size.x )
            rectWin.SetRight(size.x);
        if ( rectWin.GetBottom() > size.y )
            rectWin.SetBottom(size.y);

        rectWin.Offset(pt);
    }
    else // refresh the entire client area
    {
        rectWin.x = pt.x;
        rectWin.y = pt.y;
        rectWin.width = size.x;
        rectWin.height = size.y;
    }

    // debugging helper
#ifdef WXDEBUG_REFRESH
    static bool s_refreshDebug = FALSE;
    if ( s_refreshDebug )
    {
        wxWindowDC dc(this);
        dc.SetBrush(*wxCYAN_BRUSH);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRectangle(rectWin);

        // under Unix we use "--sync" X option for this
        #ifdef __WXMSW__
            ::GdiFlush();
            ::Sleep(200);
        #endif // __WXMSW__
    }
#endif // WXDEBUG_REFRESH

    wxWindowNative::Refresh(eraseBackground, &rectWin);
}

// ----------------------------------------------------------------------------
// state flags
// ----------------------------------------------------------------------------

bool wxWindow::Enable(bool enable)
{
    if ( !wxWindowNative::Enable(enable) )
        return FALSE;

    // disabled window can't keep focus
    if ( FindFocus() == this )
    {
        GetParent()->SetFocus();
    }

    if ( m_renderer )
    {
        // a window with renderer is drawn by ourselves and it has to be
        // refreshed to reflect its new status
        Refresh();
    }

    return TRUE;
}

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

bool wxWindow::SetCurrent(bool doit)
{
    if ( doit == m_isCurrent )
        return FALSE;

    m_isCurrent = doit;

    if ( CanBeHighlighted() )
        Refresh();

    return TRUE;
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

wxPoint wxWindow::GetClientAreaOrigin() const
{
    wxPoint pt = wxWindowBase::GetClientAreaOrigin();

    if ( m_renderer )
        pt += m_renderer->GetBorderDimensions(GetBorder()).GetPosition();

    return pt;
}

void wxWindow::DoGetClientSize(int *width, int *height) const
{
    int w, h;
    wxWindowNative::DoGetClientSize(&w, &h);

    // we assume that the scrollbars are positioned correctly (by a previous
    // call to PositionScrollbars()) here

    wxRect rectBorder;
    if ( m_renderer )
        rectBorder = m_renderer->GetBorderDimensions(GetBorder());

    bool inside = m_renderer->AreScrollbarsInsideBorder();

    if ( width )
    {
        // in any case, take account of the scrollbar
        if ( m_scrollbarVert )
            w -= m_scrollbarVert->GetSize().x;

        // if we don't have scrollbar or if it is outside the border (and not
        // blended into it), take account of the right border as well
        if ( !m_scrollbarVert || inside )
            w -= rectBorder.width;

        // and always account for the left border
        *width = w - rectBorder.x;
    }

    if ( height )
    {
        if ( m_scrollbarHorz )
            h -= m_scrollbarHorz->GetSize().y;

        if ( !m_scrollbarHorz || inside )
            h -= rectBorder.height;

        *height = h - rectBorder.y;
    }
}

void wxWindow::DoSetClientSize(int width, int height)
{
    // take into account the borders
    wxRect rectBorder = m_renderer->GetBorderDimensions(GetBorder());
    width += rectBorder.x;
    height += rectBorder.y;

    // and the scrollbars (as they may be offset into the border, use the
    // scrollbar position, not size - this supposes that PositionScrollbars()
    // had been called before)
    bool inside = m_renderer->AreScrollbarsInsideBorder();
    wxSize size = GetSize();
    if ( m_scrollbarVert )
        width += size.x - m_scrollbarVert->GetPosition().x;
    if ( !m_scrollbarVert || inside )
        width += rectBorder.width;

    if ( m_scrollbarHorz )
        height += size.y - m_scrollbarHorz->GetPosition().y;
    if ( !m_scrollbarHorz || inside )
        height += rectBorder.height;

    wxWindowNative::DoSetClientSize(width, height);
}

// ----------------------------------------------------------------------------
// scrolling: we implement it entirely ourselves except for ScrollWindow()
// function which is supposed to be (efficiently) implemented by the native
// window class
// ----------------------------------------------------------------------------

void wxWindow::RefreshScrollbars()
{
    if ( m_scrollbarHorz )
        m_scrollbarHorz->Refresh();

    if ( m_scrollbarVert )
        m_scrollbarVert->Refresh();
}

void wxWindow::PositionScrollbars()
{
    // do not use GetClientSize/Rect as it relies on the scrollbars being
    // correctly positioned

    wxSize size = GetSize();
    wxBorder border = GetBorder();
    wxRect rectBorder = m_renderer->GetBorderDimensions(border);
    bool inside = m_renderer->AreScrollbarsInsideBorder();

    int height = m_scrollbarHorz ? m_scrollbarHorz->GetSize().y : 0;
    int width = m_scrollbarVert ? m_scrollbarVert->GetSize().x : 0;

    wxRect rectBar;
    if ( m_scrollbarVert )
    {
        rectBar.x = size.x - width;
        if ( inside )
           rectBar.x -= rectBorder.width;
        rectBar.width = width;
        rectBar.y = 0;
        if ( inside )
            rectBar.y += rectBorder.y;
        rectBar.height = size.y - height;
        if ( inside )
            rectBar.height -= rectBorder.y + rectBorder.height;

        m_scrollbarVert->SetSize(rectBar, wxSIZE_NO_ADJUSTMENTS);
    }

    if ( m_scrollbarHorz )
    {
        rectBar.y = size.y - height;
        if ( inside )
            rectBar.y -= rectBorder.height;
        rectBar.height = height;
        rectBar.x = 0;
        if ( inside )
            rectBar.x += rectBorder.x;
        rectBar.width = size.x - width;
        if ( inside )
            rectBar.width -= rectBorder.x + rectBorder.width;

        m_scrollbarHorz->SetSize(rectBar, wxSIZE_NO_ADJUSTMENTS);
    }

    RefreshScrollbars();
}

void wxWindow::SetScrollbar(int orient,
                            int pos,
                            int pageSize,
                            int range,
                            bool refresh)
{
    bool hasClientSizeChanged = FALSE;
    wxScrollBar *scrollbar = GetScrollbar(orient);
    if ( range )
    {
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

            // the client area diminished as we created a scrollbar
            hasClientSizeChanged = TRUE;

            PositionScrollbars();
        }
        else if ( GetWindowStyle() & wxALWAYS_SHOW_SB )
        {
            // we might have disabled it before
            scrollbar->Enable();
        }

        scrollbar->SetScrollbar(pos, pageSize, range, pageSize, refresh);
    }
    else // no range means no scrollbar
    {
        if ( scrollbar )
        {
            // wxALWAYS_SHOW_SB only applies to the vertical scrollbar
            if ( (orient & wxVERTICAL) && (GetWindowStyle() & wxALWAYS_SHOW_SB) )
            {
                // just disable the scrollbar
                scrollbar->SetScrollbar(pos, pageSize, range, pageSize, refresh);
                scrollbar->Disable();
            }
            else // really remove the scrollbar
            {
                delete scrollbar;

                if ( orient & wxVERTICAL )
                    m_scrollbarVert = NULL;
                else
                    m_scrollbarHorz = NULL;

                // the client area increased as we removed a scrollbar
                hasClientSizeChanged = TRUE;

                // the size of the remaining scrollbar must be adjusted
                if ( m_scrollbarHorz || m_scrollbarVert )
                {
                    PositionScrollbars();
                }
            }
        }
    }

    // give the window a chance to relayout
    if ( hasClientSizeChanged )
    {
        wxSizeEvent event(GetSize());
        (void)GetEventHandler()->ProcessEvent(event);
    }
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
    // before scrolling it, ensure that we don't have any unpainted areas
    //Update();

    wxRect r;

    if ( dx )
    {
        r = ScrollNoRefresh(dx, 0, rect);
        Refresh(TRUE /* erase bkgnd */, &r);
    }

    if ( dy )
    {
        r = ScrollNoRefresh(0, dy, rect);
        Refresh(TRUE /* erase bkgnd */, &r);
    }
}

wxRect wxWindow::ScrollNoRefresh(int dx, int dy, const wxRect *rectTotal)
{
    wxASSERT_MSG( !dx || !dy, _T("can't be used for diag scrolling") );

    // the rect to refresh (which we will calculate)
    wxRect rect;

    if ( !dx && !dy )
    {
        // nothing to do
        return rect;
    }

    // calculate the part of the window which we can just redraw in the new
    // location
    wxSize sizeTotal = rectTotal ? rectTotal->GetSize() : GetClientSize();

    wxLogTrace(_T("scroll"), _T("rect is %dx%d, scroll by %d, %d"),
               sizeTotal.x, sizeTotal.y, dx, dy);

    // the initial and end point of the region we move in client coords
    wxPoint ptSource, ptDest;
    if ( rectTotal )
    {
        ptSource = rectTotal->GetPosition();
        ptDest = rectTotal->GetPosition();
    }

    // the size of this region
    wxSize size;
    size.x = sizeTotal.x - abs(dx);
    size.y = sizeTotal.y - abs(dy);
    if ( size.x <= 0 || size.y <= 0 )
    {
        // just redraw everything as nothing of the displayed image will stay
        wxLogTrace(_T("scroll"), _T("refreshing everything"));

        rect = rectTotal ? *rectTotal : wxRect(0, 0, sizeTotal.x, sizeTotal.y);
    }
    else // move the part which doesn't change to the new location
    {
        // note that when we scroll the canvas in some direction we move the
        // block which doesn't need to be refreshed in the opposite direction

        if ( dx < 0 )
        {
            // scroll to the right, move to the left
            ptSource.x -= dx;
        }
        else
        {
            // scroll to the left, move to the right
            ptDest.x += dx;
        }

        if ( dy < 0 )
        {
            // scroll down, move up
            ptSource.y -= dy;
        }
        else
        {
            // scroll up, move down
            ptDest.y += dy;
        }

#if wxUSE_CARET
        // we need to hide the caret before moving or it will erase itself at
        // the wrong (old) location
        wxCaret *caret = GetCaret();
        if ( caret )
            caret->Hide();
#endif // wxUSE_CARET

        // do move
        wxClientDC dc(this);
        wxBitmap bmp(size.x, size.y);
        wxMemoryDC dcMem;
        dcMem.SelectObject(bmp);

        dcMem.Blit(wxPoint(0, 0), size, &dc, ptSource
#if defined(__WXGTK__) && !defined(__WX_DC_BLIT_FIXED__)
                + GetClientAreaOrigin()
#endif // broken wxGTK wxDC::Blit
                  );
        dc.Blit(ptDest, size, &dcMem, wxPoint(0, 0));

        wxLogTrace(_T("scroll"),
                   _T("Blit: (%d, %d) of size %dx%d -> (%d, %d)"),
                   ptSource.x, ptSource.y,
                   size.x, size.y,
                   ptDest.x, ptDest.y);

        // and now repaint the uncovered area

        // FIXME: We repaint the intersection of these rectangles twice - is
        //        it bad? I don't think so as it is rare to scroll the window
        //        diagonally anyhow and so adding extra logic to compute
        //        rectangle intersection is probably not worth the effort

        rect.x = ptSource.x;
        rect.y = ptSource.y;

        if ( dx )
        {
            if ( dx < 0 )
            {
                // refresh the area along the right border
                rect.x += size.x + dx;
                rect.width = -dx;
            }
            else
            {
                // refresh the area along the left border
                rect.width = dx;
            }

            rect.height = sizeTotal.y;

            wxLogTrace(_T("scroll"), _T("refreshing (%d, %d)-(%d, %d)"),
                       rect.x, rect.y,
                       rect.GetRight() + 1, rect.GetBottom() + 1);
        }

        if ( dy )
        {
            if ( dy < 0 )
            {
                // refresh the area along the bottom border
                rect.y += size.y + dy;
                rect.height = -dy;
            }
            else
            {
                // refresh the area along the top border
                rect.height = dy;
            }

            rect.width = sizeTotal.x;

            wxLogTrace(_T("scroll"), _T("refreshing (%d, %d)-(%d, %d)"),
                       rect.x, rect.y,
                       rect.GetRight() + 1, rect.GetBottom() + 1);
        }

#if wxUSE_CARET
        if ( caret )
            caret->Show();
#endif // wxUSE_CARET
    }

    return rect;
}

// ----------------------------------------------------------------------------
// colours/fonts
// ----------------------------------------------------------------------------

bool wxWindow::SetBackgroundColour(const wxColour& colour)
{
    if ( !wxWindowNative::SetBackgroundColour(colour) )
        return FALSE;

    m_hasBgCol = TRUE;

    return TRUE;
}

bool wxWindow::SetForegroundColour(const wxColour& colour)
{
    if ( !wxWindowNative::SetForegroundColour(colour) )
        return FALSE;

    m_hasFgCol = TRUE;

    return TRUE;
}

bool wxWindow::SetFont(const wxFont& font)
{
    if ( !wxWindowNative::SetFont(font) )
        return FALSE;

    m_hasFont = TRUE;

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// Name:        univ/renderer.cpp
// Purpose:     wxControlRenderer implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.08.00
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
    #pragma implementation "renderer.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/control.h"
    #include "wx/checklst.h"
    #include "wx/listbox.h"
    #include "wx/scrolbar.h"
    #include "wx/dc.h"
#endif // WX_PRECOMP

#include "wx/image.h"

#include "wx/univ/theme.h"
#include "wx/univ/renderer.h"
#include "wx/univ/colschem.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxRenderer: drawing helpers
// ----------------------------------------------------------------------------

void wxRenderer::StandardDrawFrame(wxDC& dc,
                                   const wxRect& rectFrame,
                                   const wxRect& rectLabel)
{
    // draw left, bottom and right lines entirely
    DrawVerticalLine(dc, rectFrame.GetLeft(),
                     rectFrame.GetTop(), rectFrame.GetBottom() - 2);
    DrawHorizontalLine(dc, rectFrame.GetBottom() - 1,
                       rectFrame.GetLeft(), rectFrame.GetRight());
    DrawVerticalLine(dc, rectFrame.GetRight() - 1,
                     rectFrame.GetTop(), rectFrame.GetBottom() - 1);

    // and 2 parts of the top line
    DrawHorizontalLine(dc, rectFrame.GetTop(),
                       rectFrame.GetLeft() + 1, rectLabel.GetLeft());
    DrawHorizontalLine(dc, rectFrame.GetTop(),
                       rectLabel.GetRight(), rectFrame.GetRight() - 2);
}

/* static */
void wxRenderer::StandardDrawTextLine(wxDC& dc,
                                      const wxString& text,
                                      const wxRect& rect,
                                      int selStart, int selEnd,
                                      int flags)
{
    if ( (selStart == -1) || !(flags & wxCONTROL_FOCUSED) )
    {
        // just draw it as is
        dc.DrawText(text, rect.x, rect.y);
    }
    else // we have selection
    {
        wxCoord width,
                x = rect.x;

        // draw the part before selection
        wxString s(text, (size_t)selStart);
        if ( !s.empty() )
        {
            dc.DrawText(s, x, rect.y);

            dc.GetTextExtent(s, &width, NULL);
            x += width;
        }

        // draw the selection itself
        s = wxString(text.c_str() + selStart, text.c_str() + selEnd);
        if ( !s.empty() )
        {
            wxColour colFg = dc.GetTextForeground(),
                     colBg = dc.GetTextBackground();
            dc.SetTextForeground(wxTHEME_COLOUR(HIGHLIGHT_TEXT));
            dc.SetTextBackground(wxTHEME_COLOUR(HIGHLIGHT));
            dc.SetBackgroundMode(wxSOLID);

            dc.DrawText(s, x, rect.y);
            dc.GetTextExtent(s, &width, NULL);
            x += width;

            dc.SetBackgroundMode(wxTRANSPARENT);
            dc.SetTextBackground(colBg);
            dc.SetTextForeground(colFg);
        }

        // draw the final part
        s = text.c_str() + selEnd;
        if ( !s.empty() )
        {
            dc.DrawText(s, x, rect.y);
        }
    }
}

// ----------------------------------------------------------------------------
// wxRenderer: scrollbar geometry
// ----------------------------------------------------------------------------

/* static */
void wxRenderer::StandardScrollBarThumbSize(wxCoord length,
                                            int thumbPos,
                                            int thumbSize,
                                            int range,
                                            wxCoord *thumbStart,
                                            wxCoord *thumbEnd)
{
    // the thumb can't be made less than this number of pixels
    static const wxCoord thumbMinWidth = 8; // FIXME: should be configurable

    *thumbStart = (length*thumbPos) / range;
    *thumbEnd = (length*(thumbPos + thumbSize)) / range;

    if ( *thumbEnd - *thumbStart < thumbMinWidth )
    {
        // adjust the end if possible
        if ( *thumbStart <= length - thumbMinWidth )
        {
            // yes, just make it wider
            *thumbEnd = *thumbStart + thumbMinWidth;
        }
        else // it is at the bottom of the scrollbar
        {
            // so move it a bit up
            *thumbStart = length - thumbMinWidth;
            *thumbEnd = length;
        }
    }
}

/* static */
wxRect wxRenderer::StandardGetScrollbarRect(const wxScrollBar *scrollbar,
                                            wxScrollBar::Element elem,
                                            int thumbPos,
                                            const wxSize& sizeArrow)
{
    if ( thumbPos == -1 )
    {
        thumbPos = scrollbar->GetThumbPosition();
    }

    wxSize sizeTotal = scrollbar->GetClientSize();
    wxCoord *start, *width, length, arrow;
    wxRect rect;
    if ( scrollbar->IsVertical() )
    {
        rect.x = 0;
        rect.width = sizeTotal.x;
        length = sizeTotal.y;
        start = &rect.y;
        width = &rect.height;
        arrow = sizeArrow.y;
    }
    else // horizontal
    {
        rect.y = 0;
        rect.height = sizeTotal.y;
        length = sizeTotal.x;
        start = &rect.x;
        width = &rect.width;
        arrow = sizeArrow.x;
    }

    switch ( elem )
    {
        case wxScrollBar::Element_Arrow_Line_1:
            *start = 0;
            *width = arrow;
            break;

        case wxScrollBar::Element_Arrow_Line_2:
            *start = length - arrow;
            *width = arrow;
            break;

        case wxScrollBar::Element_Arrow_Page_1:
        case wxScrollBar::Element_Arrow_Page_2:
            // we don't have them at all
            break;

        case wxScrollBar::Element_Thumb:
        case wxScrollBar::Element_Bar_1:
        case wxScrollBar::Element_Bar_2:
            // we need to calculate the thumb position - do it
            {
                length -= 2*arrow;
                wxCoord thumbStart, thumbEnd;
                int range = scrollbar->GetRange();
                if ( !range )
                {
                    thumbStart =
                    thumbEnd = 0;
                }
                else
                {
                    StandardScrollBarThumbSize(length,
                                               thumbPos,
                                               scrollbar->GetThumbSize(),
                                               range,
                                               &thumbStart,
                                               &thumbEnd);
                }

                if ( elem == wxScrollBar::Element_Thumb )
                {
                    *start = thumbStart;
                    *width = thumbEnd - thumbStart;
                }
                else if ( elem == wxScrollBar::Element_Bar_1 )
                {
                    *start = 0;
                    *width = thumbStart;
                }
                else // elem == wxScrollBar::Element_Bar_2
                {
                    *start = thumbEnd;
                    *width = length - thumbEnd;
                }

                // everything is relative to the start of the shaft so far
                *start += arrow;
            }
            break;

        case wxScrollBar::Element_Max:
        default:
            wxFAIL_MSG( _T("unknown scrollbar element") );
    }

    return rect;
}

/* static */
wxCoord wxRenderer::StandardScrollBarSize(const wxScrollBar *scrollbar,
                                          const wxSize& sizeArrowSB)
{
    wxCoord sizeArrow, sizeTotal;
    if ( scrollbar->GetWindowStyle() & wxVERTICAL )
    {
        sizeArrow = sizeArrowSB.y;
        sizeTotal = scrollbar->GetSize().y;
    }
    else // horizontal
    {
        sizeArrow = sizeArrowSB.x;
        sizeTotal = scrollbar->GetSize().x;
    }

    return sizeTotal - 2*sizeArrow;
}

/* static */
wxCoord wxRenderer::StandardScrollbarToPixel(const wxScrollBar *scrollbar,
                                             int thumbPos,
                                             const wxSize& sizeArrow)
{
    int range = scrollbar->GetRange();
    if ( !range )
    {
        // the only valid position anyhow
        return 0;
    }

    if ( thumbPos == -1 )
    {
        // by default use the current thumb position
        thumbPos = scrollbar->GetThumbPosition();
    }

    return ( thumbPos*StandardScrollBarSize(scrollbar, sizeArrow) ) / range
             + (scrollbar->IsVertical() ? sizeArrow.y : sizeArrow.x);
}

/* static */
int wxRenderer::StandardPixelToScrollbar(const wxScrollBar *scrollbar,
                                         wxCoord coord,
                                         const wxSize& sizeArrow)
{
    return ( (coord - (scrollbar->IsVertical() ? sizeArrow.y : sizeArrow.x)) *
               scrollbar->GetRange() ) /
               StandardScrollBarSize(scrollbar, sizeArrow);
}

/* static */
wxHitTest wxRenderer::StandardHitTestScrollbar(const wxScrollBar *scrollbar,
                                               const wxPoint& pt,
                                               const wxSize& sizeArrowSB)
{
    // we only need to work with either x or y coord depending on the
    // orientation, choose one (but still check the other one to verify if the
    // mouse is in the window at all)
    wxCoord coord, sizeArrow, sizeTotal;
    wxSize size = scrollbar->GetSize();
    if ( scrollbar->GetWindowStyle() & wxVERTICAL )
    {
        if ( pt.x < 0 || pt.x > size.x )
            return wxHT_NOWHERE;

        coord = pt.y;
        sizeArrow = sizeArrowSB.y;
        sizeTotal = size.y;
    }
    else // horizontal
    {
        if ( pt.y < 0 || pt.y > size.y )
            return wxHT_NOWHERE;

        coord = pt.x;
        sizeArrow = sizeArrowSB.x;
        sizeTotal = size.x;
    }

    // test for the arrows first as it's faster
    if ( coord < 0 || coord > sizeTotal )
    {
        return wxHT_NOWHERE;
    }
    else if ( coord < sizeArrow )
    {
        return wxHT_SCROLLBAR_ARROW_LINE_1;
    }
    else if ( coord > sizeTotal - sizeArrow )
    {
        return wxHT_SCROLLBAR_ARROW_LINE_2;
    }
    else
    {
        // calculate the thumb position in pixels
        sizeTotal -= 2*sizeArrow;
        wxCoord thumbStart, thumbEnd;
        int range = scrollbar->GetRange();
        if ( !range )
        {
            // clicking the scrollbar without range has no effect
            return wxHT_NOWHERE;
        }
        else
        {
            StandardScrollBarThumbSize(sizeTotal,
                                       scrollbar->GetThumbPosition(),
                                       scrollbar->GetThumbSize(),
                                       range,
                                       &thumbStart,
                                       &thumbEnd);
        }

        // now compare with the thumb position
        coord -= sizeArrow;
        if ( coord < thumbStart )
            return wxHT_SCROLLBAR_BAR_1;
        else if ( coord > thumbEnd )
            return wxHT_SCROLLBAR_BAR_2;
        else
            return wxHT_SCROLLBAR_THUMB;
    }
}

wxRenderer::~wxRenderer()
{
}

// ----------------------------------------------------------------------------
// wxControlRenderer
// ----------------------------------------------------------------------------

wxControlRenderer::wxControlRenderer(wxWindow *window,
                                     wxDC& dc,
                                     wxRenderer *renderer)
                : m_dc(dc)
{
    m_window = window;
    m_renderer = renderer;

    wxSize size = m_window->GetClientSize();
    m_rect.x =
    m_rect.y = 0;
    m_rect.width = size.x;
    m_rect.height = size.y;
}

void wxControlRenderer::DrawLabel(const wxBitmap& bitmap,
                                  wxCoord marginX, wxCoord marginY)
{
    m_dc.SetFont(m_window->GetFont());
    m_dc.SetTextForeground(m_window->GetForegroundColour());

    wxString label = m_window->GetLabel();
    if ( !label.empty() || bitmap.Ok() )
    {
        wxRect rectLabel = m_rect;
        if ( bitmap.Ok() )
        {
            rectLabel.Inflate(-marginX, -marginY);
        }

        wxControl *ctrl = wxStaticCast(m_window, wxControl);

        m_renderer->DrawButtonLabel(m_dc,
                                    label,
                                    bitmap,
                                    rectLabel,
                                    m_window->GetStateFlags(),
                                    ctrl->GetAlignment(),
                                    ctrl->GetAccelIndex());
    }
}

void wxControlRenderer::DrawFrame()
{
    m_dc.SetFont(m_window->GetFont());
    m_dc.SetTextForeground(m_window->GetForegroundColour());
    m_dc.SetTextBackground(m_window->GetBackgroundColour());

    wxControl *ctrl = wxStaticCast(m_window, wxControl);

    m_renderer->DrawFrame(m_dc,
                          m_window->GetLabel(),
                          m_rect,
                          m_window->GetStateFlags(),
                          ctrl->GetAlignment(),
                          ctrl->GetAccelIndex());
}

void wxControlRenderer::DrawButtonBorder()
{
    int flags = m_window->GetStateFlags();

    m_renderer->DrawButtonBorder(m_dc, m_rect, flags, &m_rect);

    m_renderer->DrawBackground(m_dc, wxTHEME_BG_COLOUR(m_window), m_rect, flags);
}

void wxControlRenderer::DrawBitmap(const wxBitmap& bitmap)
{
    int style = m_window->GetWindowStyle();
    DrawBitmap(m_dc, bitmap, m_rect,
               style & wxALIGN_MASK,
               style & wxBI_EXPAND ? wxEXPAND : wxSTRETCH_NOT);
}

/* static */
void wxControlRenderer::DrawBitmap(wxDC &dc,
                                   const wxBitmap& bitmap,
                                   const wxRect& rect,
                                   int alignment,
                                   wxStretch stretch)
{
    // we may change the bitmap if we stretch it
    wxBitmap bmp = bitmap;
    if ( !bmp.Ok() )
        return;

    int width = bmp.GetWidth(),
        height = bmp.GetHeight();

    wxCoord x = 0,
            y = 0;
    if ( stretch & wxTILE )
    {
        // tile the bitmap
        for ( ; x < rect.width; x += width )
        {
            for ( y = 0; y < rect.height; y += height )
            {
                // no need to use mask here as we cover the entire window area
                dc.DrawBitmap(bmp, x, y);
            }
        }
    }
    else if ( stretch & wxEXPAND )
    {
        // stretch bitmap to fill the entire control
        bmp = wxImage(bmp).Scale(rect.width, rect.height).ConvertToBitmap();
    }
    else // not stretched, not tiled
    {
        if ( alignment & wxALIGN_RIGHT )
        {
            x = rect.GetRight() - width;
        }
        else if ( alignment & wxALIGN_CENTRE )
        {
            x = (rect.GetLeft() + rect.GetRight() - width + 1) / 2;
        }
        else // alignment & wxALIGN_LEFT
        {
            x = rect.GetLeft();
        }

        if ( alignment & wxALIGN_BOTTOM )
        {
            y = rect.GetBottom() - height;
        }
        else if ( alignment & wxALIGN_CENTRE_VERTICAL )
        {
            y = (rect.GetTop() + rect.GetBottom() - height + 1) / 2;
        }
        else // alignment & wxALIGN_TOP
        {
            y = rect.GetTop();
        }
    }

    // do draw it
    dc.DrawBitmap(bmp, x, y, TRUE /* use mask */);
}

void wxControlRenderer::DrawScrollbar(const wxScrollBar *scrollbar,
                                      int thumbPosOld)
{
    // we will only redraw the parts which must be redrawn and not everything
    wxRegion rgnUpdate = scrollbar->GetUpdateRegion();

    {
        wxRect rectUpdate = rgnUpdate.GetBox();
        wxLogTrace(_T("scrollbar"),
                   _T("%s redraw: update box is (%d, %d)-(%d, %d)"),
                   scrollbar->IsVertical() ? _T("vert") : _T("horz"),
                   rectUpdate.GetLeft(),
                   rectUpdate.GetTop(),
                   rectUpdate.GetRight(),
                   rectUpdate.GetBottom());

#if 0 //def WXDEBUG_SCROLLBAR
        static bool s_refreshDebug = FALSE;
        if ( s_refreshDebug )
        {
            wxClientDC dc(wxConstCast(scrollbar, wxScrollBar));
            dc.SetBrush(*wxRED_BRUSH);
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRectangle(rectUpdate);

            // under Unix we use "--sync" X option for this
            #ifdef __WXMSW__
                ::GdiFlush();
                ::Sleep(200);
            #endif // __WXMSW__
        }
#endif // WXDEBUG_SCROLLBAR
    }

    wxOrientation orient = scrollbar->IsVertical() ? wxVERTICAL
                                                   : wxHORIZONTAL;

    // the shaft
    for ( int nBar = 0; nBar < 2; nBar++ )
    {
        wxScrollBar::Element elem =
            (wxScrollBar::Element)(wxScrollBar::Element_Bar_1 + nBar);

        wxRect rectBar = m_renderer->GetScrollbarRect(scrollbar, elem);

        if ( rgnUpdate.Contains(rectBar) )
        {
            wxLogTrace(_T("scrollbar"),
                       _T("drawing bar part %d at (%d, %d)-(%d, %d)"),
                       nBar + 1,
                       rectBar.GetLeft(),
                       rectBar.GetTop(),
                       rectBar.GetRight(),
                       rectBar.GetBottom());

            m_renderer->DrawScrollbarShaft(m_dc,
                                           orient,
                                           rectBar,
                                           scrollbar->GetState(elem));
        }
    }

    // arrows
    for ( int nArrow = 0; nArrow < 2; nArrow++ )
    {
        wxScrollBar::Element elem =
            (wxScrollBar::Element)(wxScrollBar::Element_Arrow_Line_1 + nArrow);

        wxRect rectArrow = m_renderer->GetScrollbarRect(scrollbar, elem);
        if ( rgnUpdate.Contains(rectArrow) )
        {
            wxLogTrace(_T("scrollbar"),
                       _T("drawing arrow %d at (%d, %d)-(%d, %d)"),
                       nArrow + 1,
                       rectArrow.GetLeft(),
                       rectArrow.GetTop(),
                       rectArrow.GetRight(),
                       rectArrow.GetBottom());

            scrollbar->GetArrows().DrawArrow
            (
                (wxScrollArrows::Arrow)nArrow,
                m_dc,
                rectArrow,
                TRUE // draw a scrollbar arrow, not just an arrow
            );
        }
    }

    // TODO: support for page arrows

    // and the thumb
    wxScrollBar::Element elem = wxScrollBar::Element_Thumb;
    wxRect rectThumb = m_renderer->GetScrollbarRect(scrollbar, elem);
    if ( rectThumb.width && rectThumb.height && rgnUpdate.Contains(rectThumb) )
    {
        wxLogTrace(_T("scrollbar"),
                   _T("drawing thumb at (%d, %d)-(%d, %d)"),
                   rectThumb.GetLeft(),
                   rectThumb.GetTop(),
                   rectThumb.GetRight(),
                   rectThumb.GetBottom());

        m_renderer->DrawScrollbarThumb(m_dc,
                                       orient,
                                       rectThumb,
                                       scrollbar->GetState(elem));
    }
}

void wxControlRenderer::DrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
    wxASSERT_MSG( x1 == x2 || y1 == y2,
                  _T("line must be either horizontal or vertical") );

    if ( x1 == x2 )
        m_renderer->DrawVerticalLine(m_dc, x1, y1, y2);
    else // horizontal
        m_renderer->DrawHorizontalLine(m_dc, y1, x1, x2);
}

#if wxUSE_LISTBOX

void wxControlRenderer::DrawItems(const wxListBox *lbox,
                                  size_t itemFirst, size_t itemLast)
{
    DoDrawItems(lbox, itemFirst, itemLast);
}

void wxControlRenderer::DoDrawItems(const wxListBox *lbox,
                                    size_t itemFirst, size_t itemLast,
                                    bool isCheckLbox)
{
    // prepare for the drawing: calc the initial position
    wxCoord lineHeight = lbox->GetLineHeight();

    // note that SetClippingRegion() needs the physical (unscrolled)
    // coordinates while we use the logical (scrolled) ones for the drawing
    // itself
    wxRect rect;
    wxSize size = lbox->GetClientSize();
    rect.width = size.x;
    rect.height = size.y;

    // keep the text inside the client rect or we will overwrite the vertical
    // scrollbar for the long strings
    m_dc.SetClippingRegion(rect.x, rect.y, rect.width + 1, rect.height + 1);

    // adjust the rect position now
    lbox->CalcScrolledPosition(rect.x, rect.y, &rect.x, &rect.y);
    rect.y += itemFirst*lineHeight;
    rect.height = lineHeight;

    // the rect should go to the right visible border so adjust the width if x
    // is shifted (rightmost point should stay the same)
    rect.width -= rect.x;

    // we'll keep the text colour unchanged
    m_dc.SetTextForeground(lbox->GetForegroundColour());

    // an item should have the focused rect only when the lbox has focus, so
    // make sure that we never set wxCONTROL_FOCUSED flag if it doesn't
    int itemCurrent = wxWindow::FindFocus() == (wxWindow *)lbox // cast needed
                        ? lbox->GetCurrentItem()
                        : -1;
    for ( size_t n = itemFirst; n < itemLast; n++ )
    {
        int flags = 0;
        if ( (int)n == itemCurrent )
            flags |= wxCONTROL_FOCUSED;
        if ( lbox->IsSelected(n) )
            flags |= wxCONTROL_SELECTED;

#if wxUSE_CHECKLISTBOX
        if ( isCheckLbox )
        {
            wxCheckListBox *checklstbox = wxStaticCast(lbox, wxCheckListBox);
            if ( checklstbox->IsChecked(n) )
                flags |= wxCONTROL_CHECKED;

            m_renderer->DrawCheckItem(m_dc, lbox->GetString(n),
                                      wxNullBitmap,
                                      rect,
                                      flags);
        }
        else
#endif // wxUSE_CHECKLISTBOX
        {
            m_renderer->DrawItem(m_dc, lbox->GetString(n), rect, flags);
        }

        rect.y += lineHeight;
    }
}

#endif // wxUSE_LISTBOX

#if wxUSE_CHECKLISTBOX

void wxControlRenderer::DrawCheckItems(const wxCheckListBox *lbox,
                                       size_t itemFirst, size_t itemLast)
{
    DoDrawItems(lbox, itemFirst, itemLast, TRUE);
}

#endif // wxUSE_CHECKLISTBOX

#if wxUSE_NOTEBOOK

void wxControlRenderer::DrawTab(wxDirection dir,
                                const wxRect& rect,
                                const wxString& label,
                                const wxBitmap& bitmap,
                                int flags)
{
    m_dc.SetFont(m_window->GetFont());
    m_dc.SetTextForeground(m_window->GetForegroundColour());

    m_renderer->DrawButtonLabel(m_dc, label, bitmap, rect,
                                flags, wxALIGN_CENTRE);
    DrawLine(rect.x, rect.GetBottom(), rect.x, rect.GetTop());
    DrawLine(rect.GetRight(), rect.GetBottom(), rect.GetRight(), rect.GetTop());
    DrawLine(rect.GetLeft(), rect.y, rect.GetRight(), rect.y);
    if ( !(flags & wxCONTROL_SELECTED) )
    {
        DrawLine(rect.GetLeft(), rect.GetBottom(),
                 rect.GetRight(), rect.GetBottom());
    }
}

#endif // wxUSE_NOTEBOOK

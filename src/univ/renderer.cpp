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
    #include "wx/listbox.h"
    #include "wx/scrolbar.h"
    #include "wx/dc.h"
#endif // WX_PRECOMP

#include "wx/image.h"

#include "wx/univ/renderer.h"

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

// ----------------------------------------------------------------------------
// wxRenderer: scrollbar geometry
// ----------------------------------------------------------------------------

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
                    int thumbSize = scrollbar->GetThumbSize();

                    thumbStart = (length*thumbPos) / range;
                    thumbEnd = (length*(thumbPos + thumbSize)) / range;
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
            thumbStart =
            thumbEnd = 0;
        }
        else
        {
            int posThumb = scrollbar->GetThumbPosition(),
                sizeThumb = scrollbar->GetThumbSize();

            thumbStart = (sizeTotal*posThumb) / range;
            thumbEnd = (sizeTotal*(posThumb + sizeThumb)) / range;
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

    wxSize size = m_window->GetSize();
    m_rect.x =
    m_rect.y = 0;
    m_rect.width = size.x;
    m_rect.height = size.y;
}

void wxControlRenderer::DrawBorder()
{
    int flags = m_window->GetStateFlags();

    // draw outline
    m_renderer->DrawBorder(m_dc, m_window->GetBorder(),
                           m_rect, flags, &m_rect);

    // fill the inside unless we have the background bitmap as we don't want to
    // overwrite it
    if ( !m_window->GetBackgroundBitmap().Ok() )
    {
        m_renderer->DrawBackground(m_dc, m_window->GetBackgroundColour(),
                                   m_rect, flags);
    }
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

        m_renderer->DrawLabel(m_dc,
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

    m_renderer->DrawBackground(m_dc, m_window->GetBackgroundColour(),
                               m_rect, flags);
}

void wxControlRenderer::DrawBitmap(const wxBitmap& bitmap)
{
    int style = m_window->GetWindowStyle();
    DrawBitmap(bitmap, m_rect,
               style & wxALIGN_MASK,
               style & wxBI_EXPAND ? wxEXPAND : wxSTRETCH_NOT);
}

void wxControlRenderer::DrawBackgroundBitmap()
{
    // get the bitmap and the flags
    int alignment;
    wxStretch stretch;
    wxBitmap bmp = m_window->GetBackgroundBitmap(&alignment, &stretch);

    DrawBitmap(bmp, m_rect, alignment, stretch);
}

void wxControlRenderer::DrawBitmap(const wxBitmap& bitmap,
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
                m_dc.DrawBitmap(bmp, x, y);
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
            x = (rect.GetLeft() + rect.GetRight() - width) / 2;
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
            y = (rect.GetTop() + rect.GetBottom() - height) / 2;
        }
        else // alignment & wxALIGN_TOP
        {
            y = rect.GetTop();
        }
    }

    // do draw it
    m_dc.DrawBitmap(bmp, x, y, TRUE /* use mask */);
}

void wxControlRenderer::DrawScrollbar(const wxScrollBar *scrollbar,
                                      int thumbPosOld)
{
    // we will only redraw the parts which must be redrawn and not everything
    wxRegion rgnUpdate = scrollbar->GetUpdateRegion();

    {
        wxRect rectUpdate = rgnUpdate.GetBox();
        wxLogTrace(_T("scroll"), _T("%s redraw: update box is (%d, %d)-(%d, %d)"),
                   scrollbar->IsVertical() ? _T("vert") : _T("horz"),
                   rectUpdate.GetLeft(),
                   rectUpdate.GetTop(),
                   rectUpdate.GetRight(),
                   rectUpdate.GetBottom());
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
            wxLogTrace(_T("scroll"),
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
    static const wxDirection arrowDirs[2][2] =
    {
        { wxLEFT, wxRIGHT },
        { wxUP,   wxDOWN  }
    };

    for ( int nArrow = 0; nArrow < 2; nArrow++ )
    {
        wxScrollBar::Element elem =
            (wxScrollBar::Element)(wxScrollBar::Element_Arrow_Line_1 + nArrow);

        wxRect rectArrow = m_renderer->GetScrollbarRect(scrollbar, elem);
        if ( rgnUpdate.Contains(rectArrow) )
        {
            wxLogTrace(_T("scroll"),
                       _T("drawing arrow %d at (%d, %d)-(%d, %d)"),
                       nArrow + 1,
                       rectArrow.GetLeft(),
                       rectArrow.GetTop(),
                       rectArrow.GetRight(),
                       rectArrow.GetBottom());

            m_renderer->DrawArrow(m_dc,
                                  arrowDirs[scrollbar->IsVertical()][nArrow],
                                  rectArrow,
                                  scrollbar->GetState(elem));
        }
    }

    // TODO: support for page arrows

    // and the thumb
    wxScrollBar::Element elem = wxScrollBar::Element_Thumb;
    wxRect rectThumb = m_renderer->GetScrollbarRect(scrollbar, elem);
    if ( rgnUpdate.Contains(rectThumb) )
    {
        wxLogTrace(_T("scroll"), _T("drawing thumb at (%d, %d)-(%d, %d)"),
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

void wxControlRenderer::DrawItems(const wxListBox *lbox,
                                  size_t itemFirst, size_t itemLast)
{
    // prepare for the drawing: calc the initial position
    wxCoord lineHeight = lbox->GetLineHeight();
    int lines, pixelsPerLine;
    lbox->GetViewStart(NULL, &lines);
    lbox->GetScrollPixelsPerUnit(NULL, &pixelsPerLine);
    wxRect rect = m_rect;
    rect.y += itemFirst*lineHeight - lines*pixelsPerLine;
    rect.height = lineHeight;

    // an item should have the focused rect only when the app has focus, so
    // make sure that we never set wxCONTROL_FOCUSED flag if it doesn't
    int itemCurrent = wxTheApp->IsActive() ? lbox->GetCurrentItem() : -1;
    for ( size_t n = itemFirst; n < itemLast; n++ )
    {
        int flags = 0;
        if ( (int)n == itemCurrent )
            flags |= wxCONTROL_FOCUSED;
        if ( lbox->IsSelected(n) )
            flags |= wxCONTROL_SELECTED;

        m_renderer->DrawItem(m_dc, lbox->GetString(n), rect, flags);

        rect.y += lineHeight;
    }
}

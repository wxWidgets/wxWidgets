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
    #include "wx/scrolbar.h"
    #include "wx/dc.h"
#endif // WX_PRECOMP

#include "wx/image.h"

#include "wx/univ/renderer.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxRenderer: scrollbar geometry
// ----------------------------------------------------------------------------

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
                                             const wxSize& sizeArrow)
{
    int range = scrollbar->GetRange();
    if ( !range )
    {
        // the only valid position anyhow
        return 0;
    }

    return ( scrollbar->GetThumbPosition() *
                StandardScrollBarSize(scrollbar, sizeArrow) ) / range;
}

/* static */
int wxRenderer::StandardPixelToScrollbar(const wxScrollBar *scrollbar,
                                         wxCoord coord,
                                         const wxSize& sizeArrow)
{
    return ( coord * scrollbar->GetRange() )
                / StandardScrollBarSize(scrollbar, sizeArrow);
}

/* static */
wxHitTest wxRenderer::StandardHitTestScrollbar(const wxScrollBar *scrollbar,
                                               const wxPoint& pt,
                                               const wxSize& sizeArrowSB)
{
    // we only need to work with tiehr x or y coord depending on the
    // orientation, choose one
    wxCoord coord, sizeArrow, sizeTotal;
    if ( scrollbar->GetWindowStyle() & wxVERTICAL )
    {
        coord = pt.y;
        sizeArrow = sizeArrowSB.y;
        sizeTotal = scrollbar->GetSize().y;
    }
    else // horizontal
    {
        coord = pt.x;
        sizeArrow = sizeArrowSB.x;
        sizeTotal = scrollbar->GetSize().x;
    }

    // test for the arrows first as it's faster
    if ( coord < sizeArrow )
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

wxControlRenderer::wxControlRenderer(wxControl *control,
                                   wxDC& dc,
                                   wxRenderer *renderer)
                : m_dc(dc)
{
    m_ctrl = control;
    m_renderer = renderer;

    wxSize size = m_ctrl->GetSize();
    m_rect.x =
    m_rect.y = 0;
    m_rect.width = size.x;
    m_rect.height = size.y;
}

void wxControlRenderer::DrawBorder()
{
    int flags = m_ctrl->GetStateFlags();

    // draw outline
    m_renderer->DrawBorder(m_dc, m_ctrl->GetBorder(),
                           m_rect, flags, &m_rect);

    // fill the inside (TODO: query the theme for bg bitmap)
    m_renderer->DrawBackground(m_dc, m_rect, flags);
}

void wxControlRenderer::DrawLabel(const wxBitmap& bitmap,
                                  wxCoord marginX, wxCoord marginY)
{
    m_dc.SetFont(m_ctrl->GetFont());
    m_dc.SetTextForeground(m_ctrl->GetForegroundColour());

    wxRect rectLabel = m_rect;
    if ( bitmap.Ok() )
    {
        wxRect rectBmp;
        int width = bitmap.GetWidth();
        rectBmp.x = m_rect.x + marginX;
        rectBmp.y = m_rect.y + marginY;
        rectBmp.width = width;
        rectBmp.height = m_rect.height - marginY;

        DrawBitmap(bitmap, rectBmp, wxALIGN_CENTRE | wxALIGN_CENTRE_VERTICAL);

        width += 2*marginX;
        rectLabel.x += width;
        rectLabel.width -= width;
    }

    wxString label = m_ctrl->GetLabel();
    if ( !label.empty() )
    {
        m_renderer->DrawLabel(m_dc,
                              label,
                              rectLabel,
                              m_ctrl->GetStateFlags(),
                              m_ctrl->GetAlignment(),
                              m_ctrl->GetAccelIndex());
    }
}

void wxControlRenderer::DrawFrame()
{
    m_dc.SetFont(m_ctrl->GetFont());
    m_dc.SetTextForeground(m_ctrl->GetForegroundColour());
    m_dc.SetTextBackground(m_ctrl->GetBackgroundColour());

    m_renderer->DrawFrame(m_dc,
                          m_ctrl->GetLabel(),
                          m_rect,
                          m_ctrl->GetStateFlags(),
                          m_ctrl->GetAlignment(),
                          m_ctrl->GetAccelIndex());
}

void wxControlRenderer::DrawButtonBorder()
{
    int flags = m_ctrl->GetStateFlags();

    m_renderer->DrawButtonBorder(m_dc, m_rect, flags, &m_rect);

    m_renderer->DrawBackground(m_dc, m_rect, flags);
}

void wxControlRenderer::DrawBitmap(const wxBitmap& bitmap)
{
    int style = m_ctrl->GetWindowStyle();
    DrawBitmap(bitmap, m_rect,
               style & wxALIGN_MASK,
               style & wxBI_EXPAND ? wxEXPAND : wxSTRETCH_NOT);
}

void wxControlRenderer::DrawBackgroundBitmap()
{
    // get the bitmap and the flags
    int alignment;
    wxStretch stretch;
    wxBitmap bmp = m_ctrl->GetBackgroundBitmap(&alignment, &stretch);

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

void wxControlRenderer::DrawScrollbar(const wxScrollBar *scrollbar)
{
    int thumbStart, thumbEnd;
    int range = scrollbar->GetRange();
    if ( range )
    {
        int thumbPos = scrollbar->GetThumbPosition(),
            thumbSize = scrollbar->GetThumbSize();

        thumbStart = (100*thumbPos) / range;
        thumbEnd = (100*(thumbPos + thumbSize)) / range;
    }
    else // no range
    {
        thumbStart =
        thumbEnd = 0;
    }

    int flags[wxScrollBar::Element_Max];
    for ( size_t n = 0; n < WXSIZEOF(flags); n++ )
    {
        flags[n] = scrollbar->GetState((wxScrollBar::Element)n);
    }

    m_renderer->DrawScrollbar(m_dc,
                              m_ctrl->GetWindowStyle() & wxVERTICAL
                                ? wxVERTICAL
                                : wxHORIZONTAL,
                              thumbStart, thumbEnd, m_rect,
                              flags);
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

///////////////////////////////////////////////////////////////////////////////
// Name:        src/univ/stdrend.cpp
// Purpose:     implementation of wxStdRenderer
// Author:      Vadim Zeitlin
// Created:     2006-09-16
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/univ/stdrend.h"
#include "wx/univ/colschem.h"

// ============================================================================
// wxStdRenderer implementation
// ============================================================================

// ----------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------

wxStdRenderer::wxStdRenderer(const wxColourScheme *scheme)
             : m_scheme(scheme)
{
    m_penBlack = wxPen(wxSCHEME_COLOUR(scheme, SHADOW_DARK));
    m_penDarkGrey = wxPen(wxSCHEME_COLOUR(scheme, SHADOW_OUT));
    m_penLightGrey = wxPen(wxSCHEME_COLOUR(scheme, SHADOW_IN));
    m_penHighlight = wxPen(wxSCHEME_COLOUR(scheme, SHADOW_HIGHLIGHT));
}

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

void
wxStdRenderer::DrawSolidRect(wxDC& dc, const wxColour& col, const wxRect& rect)
{
    wxBrush brush(col, wxSOLID);
    dc.SetBrush(brush);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(rect);
}

void wxStdRenderer::DrawRect(wxDC& dc, wxRect *rect, const wxPen& pen)
{
    // draw
    dc.SetPen(pen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(*rect);

    // adjust the rect
    rect->Inflate(-1);
}

void wxStdRenderer::DrawShadedRect(wxDC& dc, wxRect *rect,
                                   const wxPen& pen1, const wxPen& pen2)
{
    // draw the rectangle
    dc.SetPen(pen1);
    dc.DrawLine(rect->GetLeft(), rect->GetTop(),
                rect->GetLeft(), rect->GetBottom());
    dc.DrawLine(rect->GetLeft() + 1, rect->GetTop(),
                rect->GetRight(), rect->GetTop());
    dc.SetPen(pen2);
    dc.DrawLine(rect->GetRight(), rect->GetTop(),
                rect->GetRight(), rect->GetBottom());
    dc.DrawLine(rect->GetLeft(), rect->GetBottom(),
                rect->GetRight() + 1, rect->GetBottom());

    // adjust the rect
    rect->Inflate(-1);
}

// ----------------------------------------------------------------------------
// background
// ----------------------------------------------------------------------------

void wxStdRenderer::DrawBackground(wxDC& dc,
                                   const wxColour& col,
                                   const wxRect& rect,
                                   int WXUNUSED(flags),
                                   wxWindow *window)
{
    wxColour colBg = col.Ok() ? col
                              : window ? m_scheme->GetBackground(window)
                                       : wxSCHEME_COLOUR(m_scheme, CONTROL);

    DrawSolidRect(dc, colBg, rect);
}


void wxStdRenderer::DrawButtonSurface(wxDC& dc,
                                      const wxColour& col,
                                      const wxRect& rect,
                                      int flags)
{
    DrawBackground(dc, col, rect, flags);
}

// ----------------------------------------------------------------------------
// text
// ----------------------------------------------------------------------------

void wxStdRenderer::DrawFocusRect(wxDC& dc, const wxRect& rect)
{
    // draw the pixels manually because the "dots" in wxPen with wxDOT style
    // may be short traits and not really dots
    //
    // note that to behave in the same manner as DrawRect(), we must exclude
    // the bottom and right borders from the rectangle
    wxCoord x1 = rect.GetLeft(),
            y1 = rect.GetTop(),
            x2 = rect.GetRight(),
            y2 = rect.GetBottom();

    dc.SetPen(m_penBlack);

    // this seems to be closer than what Windows does than wxINVERT although
    // I'm still not sure if it's correct
    dc.SetLogicalFunction(wxAND_REVERSE);

    wxCoord z;
    for ( z = x1 + 1; z < x2; z += 2 )
        dc.DrawPoint(z, rect.GetTop());

    wxCoord shift = z == x2 ? 0 : 1;
    for ( z = y1 + shift; z < y2; z += 2 )
        dc.DrawPoint(x2, z);

    shift = z == y2 ? 0 : 1;
    for ( z = x2 - shift; z > x1; z -= 2 )
        dc.DrawPoint(z, y2);

    shift = z == x1 ? 0 : 1;
    for ( z = y2 - shift; z > y1; z -= 2 )
        dc.DrawPoint(x1, z);

    dc.SetLogicalFunction(wxCOPY);
}

void wxStdRenderer::DrawLabel(wxDC& dc,
                              const wxString& label,
                              const wxRect& rect,
                              int flags,
                              int alignment,
                              int indexAccel,
                              wxRect *rectBounds)
{
    DrawButtonLabel(dc, label, wxNullBitmap, rect, flags,
                    alignment, indexAccel, rectBounds);
}

void wxStdRenderer::DrawButtonLabel(wxDC& dc,
                                    const wxString& label,
                                    const wxBitmap& image,
                                    const wxRect& rect,
                                    int flags,
                                    int alignment,
                                    int indexAccel,
                                    wxRect *rectBounds)
{
    wxRect rectLabel = rect;
    if ( !label.empty() && (flags & wxCONTROL_DISABLED) )
    {
        if ( flags & wxCONTROL_PRESSED )
        {
            // shift the label if a button is pressed
            rectLabel.Offset(1, 1);
        }

        // draw shadow of the text
        dc.SetTextForeground(m_penHighlight.GetColour());
        wxRect rectShadow = rect;
        rectShadow.Offset(1, 1);
        dc.DrawLabel(label, rectShadow, alignment, indexAccel);

        // make the main label text grey
        dc.SetTextForeground(m_penDarkGrey.GetColour());

        if ( flags & wxCONTROL_FOCUSED )
        {
            // leave enough space for the focus rect
            rectLabel.Inflate(-2);
        }
    }

    dc.DrawLabel(label, image, rectLabel, alignment, indexAccel, rectBounds);

    if ( !label.empty() && (flags & wxCONTROL_FOCUSED) )
    {
        rectLabel.Inflate(-1);

        DrawFocusRect(dc, rectLabel);
    }
}

// ----------------------------------------------------------------------------
// borders
// ----------------------------------------------------------------------------

/*
   We implement standard-looking 3D borders which have the following appearance:

   The raised border:

   WWWWWWWWWWWWWWWWWWWWWWB
   WHHHHHHHHHHHHHHHHHHHHGB
   WH                   GB  W = white       (HILIGHT)
   WH                   GB  H = light grey  (LIGHT)
   WH                   GB  G = dark grey   (SHADOI)
   WH                   GB  B = black       (DKSHADOI)
   WH                   GB
   WH                   GB
   WGGGGGGGGGGGGGGGGGGGGGB
   BBBBBBBBBBBBBBBBBBBBBBB

   The sunken border looks like this:

   GGGGGGGGGGGGGGGGGGGGGGW
   GBBBBBBBBBBBBBBBBBBBBHW
   GB                   HW
   GB                   HW
   GB                   HW
   GB                   HW
   GB                   HW
   GB                   HW
   GHHHHHHHHHHHHHHHHHHHHHW
   WWWWWWWWWWWWWWWWWWWWWWW

   The static border (used for the controls which don't get focus) is like
   this:

   GGGGGGGGGGGGGGGGGGGGGGW
   G                     W
   G                     W
   G                     W
   G                     W
   G                     W
   G                     W
   G                     W
   WWWWWWWWWWWWWWWWWWWWWWW

   The most complicated is the double border which is a combination of special
   "anti-sunken" border and an extra border inside it:

   HHHHHHHHHHHHHHHHHHHHHHB
   HWWWWWWWWWWWWWWWWWWWWGB
   HWHHHHHHHHHHHHHHHHHHHGB
   HWH                 HGB
   HWH                 HGB
   HWH                 HGB
   HWH                 HGB
   HWHHHHHHHHHHHHHHHHHHHGB
   HGGGGGGGGGGGGGGGGGGGGGB
   BBBBBBBBBBBBBBBBBBBBBBB

   And the simple border is, well, simple:

   BBBBBBBBBBBBBBBBBBBBBBB
   B                     B
   B                     B
   B                     B
   B                     B
   B                     B
   B                     B
   B                     B
   B                     B
   BBBBBBBBBBBBBBBBBBBBBBB
*/

void wxStdRenderer::DrawRaisedBorder(wxDC& dc, wxRect *rect)
{
    DrawShadedRect(dc, rect, m_penHighlight, m_penBlack);
    DrawShadedRect(dc, rect, m_penLightGrey, m_penDarkGrey);
}

void wxStdRenderer::DrawSunkenBorder(wxDC& dc, wxRect *rect)
{
    DrawShadedRect(dc, rect, m_penDarkGrey, m_penHighlight);
    DrawShadedRect(dc, rect, m_penBlack, m_penLightGrey);
}

void wxStdRenderer::DrawAntiSunkenBorder(wxDC& dc, wxRect *rect)
{
    DrawShadedRect(dc, rect, m_penLightGrey, m_penBlack);
    DrawShadedRect(dc, rect, m_penHighlight, m_penDarkGrey);
}

void wxStdRenderer::DrawBorder(wxDC& dc,
                               wxBorder border,
                               const wxRect& rectTotal,
                               int WXUNUSED(flags),
                               wxRect *rectIn)
{
    wxRect rect = rectTotal;

    switch ( border )
    {
        case wxBORDER_SUNKEN:
            DrawSunkenBorder(dc, &rect);
            break;

        case wxBORDER_DOUBLE:
            DrawAntiSunkenBorder(dc, &rect);
            DrawRect(dc, &rect, m_penLightGrey);
            break;

        case wxBORDER_STATIC:
            DrawShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
            break;

        case wxBORDER_RAISED:
            DrawRaisedBorder(dc, &rect);
            break;

        case wxBORDER_SIMPLE:
            DrawRect(dc, &rect, m_penBlack);
            break;

        default:
            wxFAIL_MSG(_T("unknown border type"));
            // fall through

        case wxBORDER_DEFAULT:
        case wxBORDER_NONE:
            break;
    }

    if ( rectIn )
        *rectIn = rect;
}

wxRect wxStdRenderer::GetBorderDimensions(wxBorder border) const
{
    wxCoord width;
    switch ( border )
    {
        case wxBORDER_SIMPLE:
        case wxBORDER_STATIC:
            width = 1;
            break;

        case wxBORDER_RAISED:
        case wxBORDER_SUNKEN:
            width = 2;
            break;

        case wxBORDER_DOUBLE:
            width = 3;
            break;

        default:
            wxFAIL_MSG(_T("unknown border type"));
            // fall through

        case wxBORDER_DEFAULT:
        case wxBORDER_NONE:
            width = 0;
            break;
    }

    wxRect rect;
    rect.x =
    rect.y =
    rect.width =
    rect.height = width;

    return rect;
}

bool wxStdRenderer::AreScrollbarsInsideBorder() const
{
    return false;
}

void wxStdRenderer::DrawTextBorder(wxDC& dc,
                                   wxBorder border,
                                   const wxRect& rect,
                                   int flags,
                                   wxRect *rectIn)
{
    DrawBorder(dc, border, rect, flags, rectIn);
}

// ----------------------------------------------------------------------------
// lines and boxes
// ----------------------------------------------------------------------------

void
wxStdRenderer::DrawHorizontalLine(wxDC& dc, wxCoord y, wxCoord x1, wxCoord x2)
{
    dc.SetPen(m_penDarkGrey);
    dc.DrawLine(x1, y, x2 + 1, y);

    dc.SetPen(m_penHighlight);
    y++;
    dc.DrawLine(x1, y, x2 + 1, y);
}

void
wxStdRenderer::DrawVerticalLine(wxDC& dc, wxCoord x, wxCoord y1, wxCoord y2)
{
    dc.SetPen(m_penDarkGrey);
    dc.DrawLine(x, y1, x, y2 + 1);

    dc.SetPen(m_penHighlight);
    x++;
    dc.DrawLine(x, y1, x, y2 + 1);
}

void wxStdRenderer::DrawFrameWithoutLabel(wxDC& dc,
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

void wxStdRenderer::DrawFrameWithLabel(wxDC& dc,
                                       const wxString& label,
                                       const wxRect& rectFrame,
                                       const wxRect& rectText,
                                       int flags,
                                       int alignment,
                                       int indexAccel)
{
    wxRect rectLabel;
    DrawLabel(dc, label, rectText, flags, alignment, indexAccel, &rectLabel);

    DrawFrameWithoutLabel(dc, rectFrame, rectLabel);
}

void wxStdRenderer::DrawFrame(wxDC& dc,
                              const wxString& label,
                              const wxRect& rect,
                              int flags,
                              int alignment,
                              int indexAccel)
{
    wxCoord height = 0; // of the label
    wxRect rectFrame = rect;
    if ( !label.empty() )
    {
        // the text should touch the top border of the rect, so the frame
        // itself should be lower
        dc.GetTextExtent(label, NULL, &height);
        rectFrame.y += height / 2;
        rectFrame.height -= height / 2;

        // we have to draw each part of the frame individually as we can't
        // erase the background beyond the label as it might contain some
        // pixmap already, so drawing everything and then overwriting part of
        // the frame with label doesn't work

        // TODO: the +5 shouldn't be hard coded
        wxRect rectText;
        rectText.x = rectFrame.x + 5;
        rectText.y = rect.y;
        rectText.width = rectFrame.width - 7; // +2 border width
        rectText.height = height;

        DrawFrameWithLabel(dc, label, rectFrame, rectText, flags,
                           alignment, indexAccel);
    }
    else // no label
    {
        // just draw the complete frame
        DrawShadedRect(dc, &rectFrame, m_penDarkGrey, m_penHighlight);
        DrawShadedRect(dc, &rectFrame, m_penHighlight, m_penDarkGrey);
    }
}

void wxStdRenderer::DrawItem(wxDC& dc,
                             const wxString& label,
                             const wxRect& rect,
                             int flags)
{
    wxDCTextColourChanger colChanger(dc);

    if ( flags & wxCONTROL_SELECTED )
    {
        colChanger.Set(wxSCHEME_COLOUR(m_scheme, HIGHLIGHT_TEXT));

        const wxColour colBg = wxSCHEME_COLOUR(m_scheme, HIGHLIGHT);
        dc.SetBrush(colBg);
        dc.SetPen(colBg);
        dc.DrawRectangle(rect);
    }

    wxRect rectText = rect;
    rectText.x += 2;
    rectText.width -= 2;
    dc.DrawLabel(label, wxNullBitmap, rectText);

    if ( flags & wxCONTROL_FOCUSED )
    {
        DrawFocusRect(dc, rect);
    }
}

// ----------------------------------------------------------------------------
// check and radio bitmaps
// ----------------------------------------------------------------------------

/* static */
void wxStdRenderer::GetIndicatorsFromFlags(int flags,
                                           IndicatorState& state,
                                           IndicatorStatus& status)
{
    if ( flags & wxCONTROL_SELECTED )
        state = flags & wxCONTROL_DISABLED ? IndicatorState_SelectedDisabled
                                           : IndicatorState_Selected;
    else if ( flags & wxCONTROL_DISABLED )
        state = IndicatorState_Disabled;
    else if ( flags & wxCONTROL_PRESSED )
        state = IndicatorState_Pressed;
    else
        state = IndicatorState_Normal;

    status = flags & wxCONTROL_CHECKED ? IndicatorStatus_Checked
                                       : flags & wxCONTROL_UNDETERMINED
                                            ? IndicatorStatus_Undetermined
                                            : IndicatorStatus_Unchecked;
}

void wxStdRenderer::DrawCheckButton(wxDC& dc,
                                    const wxString& label,
                                    const wxBitmap& bitmap,
                                    const wxRect& rect,
                                    int flags,
                                    wxAlignment align,
                                    int indexAccel)
{
    wxBitmap bmp(bitmap.Ok() ? bitmap : GetCheckBitmap(flags));

    DrawCheckOrRadioButton(dc, label, bmp, rect, flags, align, indexAccel);
}

void wxStdRenderer::DrawRadioButton(wxDC& dc,
                                    const wxString& label,
                                    const wxBitmap& bitmap,
                                    const wxRect& rect,
                                    int flags,
                                    wxAlignment align,
                                    int indexAccel)
{
    wxBitmap bmp(bitmap.Ok() ? bitmap : GetRadioBitmap(flags));

    DrawCheckOrRadioButton(dc, label, bmp, rect, flags, align, indexAccel);
}

void wxStdRenderer::DrawCheckOrRadioButton(wxDC& dc,
                                           const wxString& label,
                                           const wxBitmap& bitmap,
                                           const wxRect& rect,
                                           int flags,
                                           wxAlignment align,
                                           int indexAccel)
{
    // calculate the position of the bitmap and of the label
    wxCoord heightBmp = bitmap.GetHeight();
    wxCoord xBmp,
            yBmp = rect.y + (rect.height - heightBmp) / 2;

    wxRect rectLabel;
    dc.GetMultiLineTextExtent(label, NULL, &rectLabel.height);
    rectLabel.y = rect.y + (rect.height - rectLabel.height) / 2;

    // align label vertically with the bitmap - looks nicer like this
    rectLabel.y -= (rectLabel.height - heightBmp) % 2;

    // calc horz position
    if ( align == wxALIGN_RIGHT )
    {
        xBmp = rect.GetRight() - bitmap.GetWidth();
        rectLabel.x = rect.x + 3;
        rectLabel.SetRight(xBmp);
    }
    else // normal (checkbox to the left of the text) case
    {
        xBmp = rect.x;
        rectLabel.x = xBmp + bitmap.GetWidth() + 5;
        rectLabel.SetRight(rect.GetRight());
    }

    dc.DrawBitmap(bitmap, xBmp, yBmp, true /* use mask */);

    DrawLabel(dc, label, rectLabel, flags,
              wxALIGN_LEFT | wxALIGN_TOP, indexAccel);
}

#if wxUSE_TEXTCTRL

void wxStdRenderer::DrawTextLine(wxDC& dc,
                                 const wxString& text,
                                 const wxRect& rect,
                                 int selStart,
                                 int selEnd,
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
            dc.SetTextForeground(wxSCHEME_COLOUR(m_scheme, HIGHLIGHT_TEXT));
            dc.SetTextBackground(wxSCHEME_COLOUR(m_scheme, HIGHLIGHT));
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

void wxStdRenderer::DrawLineWrapMark(wxDC& WXUNUSED(dc),
                                     const wxRect& WXUNUSED(rect))
{
    // nothing by default
}

#endif // wxUSE_TEXTCTRL

// ----------------------------------------------------------------------------
// scrollbars geometry
// ----------------------------------------------------------------------------

#if wxUSE_SCROLLBAR

/* static */
void wxStdRenderer::GetScrollBarThumbSize(wxCoord length,
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

wxRect wxStdRenderer::GetScrollbarRect(const wxScrollBar *scrollbar,
                                       wxScrollBar::Element elem,
                                       int thumbPos) const
{
    if ( thumbPos == -1 )
    {
        thumbPos = scrollbar->GetThumbPosition();
    }

    const wxSize sizeArrow = GetScrollbarArrowSize();

    wxSize sizeTotal = scrollbar->GetClientSize();
    wxCoord *start, *width;
    wxCoord length, arrow;
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
                    GetScrollBarThumbSize(length,
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

wxCoord wxStdRenderer::GetScrollbarSize(const wxScrollBar *scrollbar)
{
    const wxSize sizeArrowSB = GetScrollbarArrowSize();

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

wxHitTest
wxStdRenderer::HitTestScrollbar(const wxScrollBar *scrollbar, const wxPoint& pt) const
{
    // we only need to work with either x or y coord depending on the
    // orientation, choose one (but still check the other one to verify if the
    // mouse is in the window at all)
    const wxSize sizeArrowSB = GetScrollbarArrowSize();

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
            GetScrollBarThumbSize(sizeTotal,
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


wxCoord
wxStdRenderer::ScrollbarToPixel(const wxScrollBar *scrollbar, int thumbPos)
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

    const wxSize sizeArrow = GetScrollbarArrowSize();
    return (thumbPos*GetScrollbarSize(scrollbar)) / range
             + (scrollbar->IsVertical() ? sizeArrow.y : sizeArrow.x);
}

int wxStdRenderer::PixelToScrollbar(const wxScrollBar *scrollbar, wxCoord coord)
{
    const wxSize sizeArrow = GetScrollbarArrowSize();
    return ((coord - (scrollbar->IsVertical() ? sizeArrow.y : sizeArrow.x)) *
               scrollbar->GetRange() ) / GetScrollbarSize(scrollbar);
}

#endif // wxUSE_SCROLLBAR


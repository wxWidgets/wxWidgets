///////////////////////////////////////////////////////////////////////////////
// Name:        univ/themes/gtk.cpp
// Purpose:     wxUniversal theme implementing GTK-like LNF
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

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/dc.h"
    #include "wx/window.h"

    #include "wx/button.h"
    #include "wx/scrolbar.h"
#endif // WX_PRECOMP

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/colschem.h"
#include "wx/univ/theme.h"

// define this for tons of debugging messages
#undef DEBUG_MOUSE

// ----------------------------------------------------------------------------
// wxGTKRenderer: draw the GUI elements in GTK style
// ----------------------------------------------------------------------------

class wxGTKRenderer : public wxRenderer
{
public:
    wxGTKRenderer(const wxColourScheme *scheme);

    // implement the base class pure virtuals
    virtual void DrawBackground(wxDC& dc,
                                const wxColour& col,
                                const wxRect& rect,
                                int flags = 0);
    virtual void DrawLabel(wxDC& dc,
                           const wxString& label,
                           const wxBitmap& image,
                           const wxRect& rect,
                           int flags = 0,
                           int alignment = wxALIGN_LEFT | wxALIGN_TOP,
                           int indexAccel = -1,
                           wxRect *rectBounds = NULL);
    virtual void DrawBorder(wxDC& dc,
                            wxBorder border,
                            const wxRect& rect,
                            int flags = 0,
                            wxRect *rectIn = (wxRect *)NULL);
    virtual void DrawHorizontalLine(wxDC& dc,
                                    wxCoord y, wxCoord x1, wxCoord x2);
    virtual void DrawVerticalLine(wxDC& dc,
                                  wxCoord x, wxCoord y1, wxCoord y2);
    virtual void DrawFrame(wxDC& dc,
                           const wxString& label,
                           const wxRect& rect,
                           int flags = 0,
                           int alignment = wxALIGN_LEFT,
                           int indexAccel = -1);
    virtual void DrawButtonBorder(wxDC& dc,
                                  const wxRect& rect,
                                  int flags = 0,
                                  wxRect *rectIn = (wxRect *)NULL);
    virtual void DrawArrow(wxDC& dc,
                           wxDirection dir,
                           const wxRect& rect,
                           int flags = 0);
    virtual void DrawScrollbarThumb(wxDC& dc,
                                    wxOrientation orient,
                                    const wxRect& rect,
                                    int flags = 0);
    virtual void DrawScrollbarShaft(wxDC& dc,
                                    wxOrientation orient,
                                    const wxRect& rect,
                                    int flags = 0);
    virtual void DrawScrollCorner(wxDC& dc,
                                  const wxRect& rect);
    virtual void DrawItem(wxDC& dc,
                          const wxString& label,
                          const wxRect& rect,
                          int flags = 0);

    virtual void AdjustSize(wxSize *size, const wxWindow *window);
    virtual wxRect GetBorderDimensions(wxBorder border) const;

    // hit testing for the input handlers
    virtual wxRect GetScrollbarRect(const wxScrollBar *scrollbar,
                                    wxScrollBar::Element elem,
                                    int thumbPos = -1) const;
    virtual wxCoord GetScrollbarSize(const wxScrollBar *scrollbar);
    virtual wxHitTest HitTestScrollbar(const wxScrollBar *scrollbar,
                                       const wxPoint& pt) const;
    virtual wxCoord ScrollbarToPixel(const wxScrollBar *scrollbar,
                                     int thumbPos = -1);
    virtual int PixelToScrollbar(const wxScrollBar *scrollbar, wxCoord coord);
    virtual wxCoord GetListboxItemHeight(wxCoord fontHeight)
        { return fontHeight + 2; }

protected:
    // DrawBackground() helpers

    // get the colour to use for background
    wxColour GetBackgroundColour(int flags) const
        { return m_scheme->Get(wxColourScheme::CONTROL, flags); }

    // draw the background with any colour, not only the default one(s)
    void DoDrawBackground(wxDC& dc,
                          const wxColour& col,
                          const wxRect& rect);

    // DrawBorder() helpers: all of them shift and clip the DC after drawing
    // the border

    // just draw a rectangle with the given pen
    void DrawRect(wxDC& dc, wxRect *rect, const wxPen& pen);

    // draw the lower left part of rectangle
    void DrawHalfRect(wxDC& dc, wxRect *rect, const wxPen& pen);

    // draw the rectange using the first brush for the left and top sides and
    // the second one for the bottom and right ones
    void DrawShadedRect(wxDC& dc, wxRect *rect,
                        const wxPen& pen1, const wxPen& pen2);

    // as DrawShadedRect() but the pixels in the bottom left and upper right
    // border are drawn with the pen1, not pen2
    void DrawAntiShadedRect(wxDC& dc, wxRect *rect,
                            const wxPen& pen1, const wxPen& pen2);

    // used for drawing opened rectangles - draws only one side of it at once
    // (and doesn't adjust the rect)
    void DrawAntiShadedRectSide(wxDC& dc,
                                const wxRect& rect,
                                const wxPen& pen1,
                                const wxPen& pen2,
                                wxDirection dir);

    // draw an opened rect for the arrow in given direction
    void DrawArrowBorder(wxDC& dc,
                         wxRect *rect,
                         wxDirection dir);

    // draw two sides of the rectangle
    void DrawThumbBorder(wxDC& dc,
                         wxRect *rect,
                         wxOrientation orient);

    // draw the normal 3D border
    void DrawRaisedBorder(wxDC& dc, wxRect *rect);

    // returns the size of the arrow for the scrollbar (depends on
    // orientation)
    wxSize GetScrollbarArrowSize(const wxScrollBar *scrollbar) const
    {
        wxSize size;
        if ( scrollbar->IsVertical() )
        {
            size = m_sizeScrollbarArrow;
        }
        else
        {
            size.x = m_sizeScrollbarArrow.y;
            size.y = m_sizeScrollbarArrow.x;
        }

        return size;
    }

private:
    const wxColourScheme *m_scheme;

    // data
    wxSize m_sizeScrollbarArrow;

    // GDI objects
    wxPen m_penBlack,
          m_penDarkGrey,
          m_penGrey,
          m_penLightGrey,
          m_penHighlight;
};

// ----------------------------------------------------------------------------
// wxGTKInputHandler and derived classes: process the keyboard and mouse
// messages according to GTK standards
// ----------------------------------------------------------------------------

class wxGTKInputHandler : public wxInputHandler
{
public:
    wxGTKInputHandler(wxGTKRenderer *renderer);

    virtual bool HandleKey(wxControl *control,
                           const wxKeyEvent& event,
                           bool pressed);
    virtual bool HandleMouse(wxControl *control,
                             const wxMouseEvent& event);
    virtual bool HandleMouseMove(wxControl *control, const wxMouseEvent& event);

protected:
    wxGTKRenderer *m_renderer;
};

class wxGTKScrollBarInputHandler : public wxStdScrollBarInputHandler
{
public:
    wxGTKScrollBarInputHandler(wxRenderer *renderer, wxInputHandler *handler)
        : wxStdScrollBarInputHandler(renderer, handler) { }

protected:
    virtual void Highlight(wxScrollBar *scrollbar, bool doIt)
    {
        // only arrows and the thumb can be highlighted
        if ( !IsArrow() && m_htLast != wxHT_SCROLLBAR_THUMB )
            return;

        wxStdScrollBarInputHandler::Highlight(scrollbar, doIt);
    }

    virtual void Press(wxScrollBar *scrollbar, bool doIt)
    {
        // only arrows can be pressed
        if ( !IsArrow() )
            return;

        wxStdScrollBarInputHandler::Press(scrollbar, doIt);
    }

    virtual bool IsAllowedButton(int WXUNUSED(button)) { return TRUE; }

    bool IsArrow() const
    {
        return m_htLast == wxHT_SCROLLBAR_ARROW_LINE_1 ||
                m_htLast == wxHT_SCROLLBAR_ARROW_LINE_2;
    }
};

// ----------------------------------------------------------------------------
// wxGTKColourScheme: uses the standard GTK colours
// ----------------------------------------------------------------------------

class wxGTKColourScheme : public wxColourScheme
{
public:
    virtual wxColour Get(StdColour col, int flags = 0) const;
};

// ----------------------------------------------------------------------------
// wxGTKTheme
// ----------------------------------------------------------------------------

WX_DEFINE_ARRAY(wxInputHandler *, wxArrayHandlers);

class wxGTKTheme : public wxTheme
{
public:
    wxGTKTheme();
    virtual ~wxGTKTheme();

    virtual wxRenderer *GetRenderer() { return m_renderer; }
    virtual wxInputHandler *GetInputHandler(const wxString& control);
    virtual wxColourScheme *GetColourScheme(const wxString& control)
        { return m_scheme; }

private:
    wxGTKRenderer *m_renderer;

    // the names of the already created handlers and the handlers themselves
    // (these arrays are synchronized)
    wxSortedArrayString m_handlerNames;
    wxArrayHandlers m_handlers;

    wxGTKColourScheme *m_scheme;

    WX_DECLARE_THEME();
};

// ============================================================================
// implementation
// ============================================================================

WX_IMPLEMENT_THEME(wxGTKTheme, gtk, wxTRANSLATE("GTK+ theme"));

// ----------------------------------------------------------------------------
// wxGTKTheme
// ----------------------------------------------------------------------------

wxGTKTheme::wxGTKTheme()
{
    m_scheme = new wxGTKColourScheme;
    m_renderer = new wxGTKRenderer(m_scheme);
}

wxGTKTheme::~wxGTKTheme()
{
    WX_CLEAR_ARRAY(m_handlers);
}

wxInputHandler *wxGTKTheme::GetInputHandler(const wxString& control)
{
    wxInputHandler *handler;
    int n = m_handlerNames.Index(control);
    if ( n == wxNOT_FOUND )
    {
        // create a new handler
        if ( control.Matches(_T("wx*Button")) )
            handler = new wxStdButtonInputHandler(GetInputHandler(_T("wxControl")));
        else if ( control == _T("wxScrollBar") )
            handler = new wxGTKScrollBarInputHandler(m_renderer,
                                                     GetInputHandler(_T("wxControl")));
        else if ( control == _T("wxListBox") )
            handler = new wxStdListboxInputHandler(GetInputHandler(_T("wxControl")));
        else
            handler = new wxGTKInputHandler(m_renderer);

        n = m_handlerNames.Add(control);
        m_handlers.Insert(handler, n);
    }
    else // we already have it
    {
        handler = m_handlers[n];
    }

    return handler;
}

// ============================================================================
// wxGTKColourScheme
// ============================================================================

wxColour wxGTKColourScheme::Get(wxGTKColourScheme::StdColour col,
                                int flags) const
{
    switch ( col )
    {
        case SHADOW_DARK:       return *wxBLACK;
        case SHADOW_HIGHLIGHT:  return *wxWHITE;
        case SHADOW_IN:         return wxColour(0xd6d6d6);
        case SHADOW_OUT:        return wxColour(0x969696);

        case CONTROL:
                                if ( flags & wxCONTROL_PRESSED )
                                {
                                    return wxColour(0xc3c3c3);
                                }
                                else if ( flags & wxCONTROL_CURRENT )
                                {
                                    return wxColour(0xeaeaea);
                                }
                                else
                                {
                                    return wxColour(0xd6d6d6);
                                }

        case CONTROL_TEXT:      return *wxBLACK;
        case SCROLLBAR:         return wxColour(0xc3c3c3);

        case HIGHLIGHT:         return wxColour(0x9c0000);
        case HIGHLIGHT_TEXT:    return wxColour(0xffffff);

        case MAX:
        default:
            wxFAIL_MSG(_T("invalid standard colour"));
            return *wxBLACK;
    }
}

// ============================================================================
// wxGTKRenderer
// ============================================================================

// ----------------------------------------------------------------------------
// construction
// ----------------------------------------------------------------------------

wxGTKRenderer::wxGTKRenderer(const wxColourScheme *scheme)
{
    // init data
    m_scheme = scheme;
    m_sizeScrollbarArrow = wxSize(15, 14);

    // init pens
    m_penBlack = wxPen(scheme->Get(wxColourScheme::SHADOW_DARK), 0, wxSOLID);
    m_penDarkGrey = wxPen(scheme->Get(wxColourScheme::SHADOW_OUT), 0, wxSOLID);
    m_penGrey = wxPen(scheme->Get(wxColourScheme::SCROLLBAR), 0, wxSOLID);
    m_penLightGrey = wxPen(scheme->Get(wxColourScheme::SHADOW_IN), 0, wxSOLID);
    m_penHighlight = wxPen(scheme->Get(wxColourScheme::SHADOW_HIGHLIGHT), 0, wxSOLID);
}

// ----------------------------------------------------------------------------
// border stuff
// ----------------------------------------------------------------------------

void wxGTKRenderer::DrawRect(wxDC& dc, wxRect *rect, const wxPen& pen)
{
    // draw
    dc.SetPen(pen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(*rect);

    // adjust the rect
    rect->Inflate(-1);
}

void wxGTKRenderer::DrawHalfRect(wxDC& dc, wxRect *rect, const wxPen& pen)
{
    // draw the bottom and right sides
    dc.SetPen(pen);
    dc.DrawLine(rect->GetLeft(), rect->GetBottom(),
                rect->GetRight() + 1, rect->GetBottom());
    dc.DrawLine(rect->GetRight(), rect->GetTop(),
                rect->GetRight(), rect->GetBottom());

    // adjust the rect
    rect->width--;
    rect->height--;
}

void wxGTKRenderer::DrawShadedRect(wxDC& dc, wxRect *rect,
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

void wxGTKRenderer::DrawAntiShadedRectSide(wxDC& dc,
                                           const wxRect& rect,
                                           const wxPen& pen1,
                                           const wxPen& pen2,
                                           wxDirection dir)
{
    dc.SetPen(dir == wxLEFT || dir == wxUP ? pen1 : pen2);

    switch ( dir )
    {
        case wxLEFT:
            dc.DrawLine(rect.GetLeft(), rect.GetTop(),
                        rect.GetLeft(), rect.GetBottom() + 1);
            break;

        case wxUP:
            dc.DrawLine(rect.GetLeft(), rect.GetTop(),
                        rect.GetRight() + 1, rect.GetTop());
            break;

        case wxRIGHT:
            dc.DrawLine(rect.GetRight(), rect.GetTop(),
                        rect.GetRight(), rect.GetBottom() + 1);
            break;

        case wxDOWN:
            dc.DrawLine(rect.GetLeft(), rect.GetBottom(),
                        rect.GetRight() + 1, rect.GetBottom());
            break;

        default:
            wxFAIL_MSG(_T("unknown rectangle side"));
    }
}

void wxGTKRenderer::DrawAntiShadedRect(wxDC& dc, wxRect *rect,
                                       const wxPen& pen1, const wxPen& pen2)
{
    // draw the rectangle
    dc.SetPen(pen1);
    dc.DrawLine(rect->GetLeft(), rect->GetTop(),
                rect->GetLeft(), rect->GetBottom() + 1);
    dc.DrawLine(rect->GetLeft() + 1, rect->GetTop(),
                rect->GetRight() + 1, rect->GetTop());
    dc.SetPen(pen2);
    dc.DrawLine(rect->GetRight(), rect->GetTop() + 1,
                rect->GetRight(), rect->GetBottom());
    dc.DrawLine(rect->GetLeft() + 1, rect->GetBottom(),
                rect->GetRight() + 1, rect->GetBottom());

    // adjust the rect
    rect->Inflate(-1);
}

void wxGTKRenderer::DrawRaisedBorder(wxDC& dc, wxRect *rect)
{
    DrawShadedRect(dc, rect, m_penHighlight, m_penBlack);
    DrawShadedRect(dc, rect, m_penLightGrey, m_penDarkGrey);
}

void wxGTKRenderer::DrawBorder(wxDC& dc,
                                 wxBorder border,
                                 const wxRect& rectTotal,
                                 int WXUNUSED(flags),
                                 wxRect *rectIn)
{
    wxRect rect = rectTotal;

    switch ( border )
    {
        case wxBORDER_SUNKEN:
            DrawAntiShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
            DrawAntiShadedRect(dc, &rect, m_penBlack, m_penLightGrey);
            break;

        case wxBORDER_STATIC:
            DrawShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
            break;

        case wxBORDER_RAISED:
            DrawRaisedBorder(dc, &rect);
            break;

        case wxBORDER_DOUBLE:
            DrawShadedRect(dc, &rect, m_penLightGrey, m_penBlack);
            DrawShadedRect(dc, &rect, m_penHighlight, m_penDarkGrey);
            DrawRect(dc, &rect, m_penLightGrey);
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

wxRect wxGTKRenderer::GetBorderDimensions(wxBorder border) const
{
    wxCoord width;
    switch ( border )
    {
        case wxBORDER_RAISED:
        case wxBORDER_SUNKEN:
            width = 2;
            break;

        case wxBORDER_SIMPLE:
        case wxBORDER_STATIC:
            width = 1;
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

// ----------------------------------------------------------------------------
// button border
// ----------------------------------------------------------------------------

void wxGTKRenderer::DrawButtonBorder(wxDC& dc,
                                     const wxRect& rectTotal,
                                     int flags,
                                     wxRect *rectIn)
{
    wxRect rect = rectTotal;

    if ( flags & wxCONTROL_PRESSED )
    {
        // button pressed: draw a black border around it and an inward shade
        DrawRect(dc, &rect, m_penBlack);
        DrawAntiShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
        DrawAntiShadedRect(dc, &rect, m_penBlack, m_penDarkGrey);
    }
    else
    {
        // button not pressed

        if ( flags & wxCONTROL_ISDEFAULT )
        {
            // TODO
        }

        if ( flags & wxCONTROL_FOCUSED )
        {
            // button is currently default: add an extra border around it
            DrawRect(dc, &rect, m_penBlack);
        }

        // now draw a normal button
        DrawShadedRect(dc, &rect, m_penHighlight, m_penBlack);
        DrawAntiShadedRect(dc, &rect,
                           wxPen(GetBackgroundColour(flags), 0, wxSOLID),
                           m_penDarkGrey);
    }

    if ( rectIn )
    {
        *rectIn = rect;
    }
}

// ----------------------------------------------------------------------------
// lines and frames
// ----------------------------------------------------------------------------

void wxGTKRenderer::DrawHorizontalLine(wxDC& dc,
                                       wxCoord y, wxCoord x1, wxCoord x2)
{
    dc.SetPen(m_penDarkGrey);
    dc.DrawLine(x1, y, x2 + 1, y);
    dc.SetPen(m_penHighlight);
    y++;
    dc.DrawLine(x1, y, x2 + 1, y);
}

void wxGTKRenderer::DrawVerticalLine(wxDC& dc,
                                     wxCoord x, wxCoord y1, wxCoord y2)
{
    dc.SetPen(m_penDarkGrey);
    dc.DrawLine(x, y1, x, y2 + 1);
    dc.SetPen(m_penHighlight);
    x++;
    dc.DrawLine(x, y1, x, y2 + 1);
}

void wxGTKRenderer::DrawFrame(wxDC& dc,
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

        // TODO: the +4 should be customizable

        wxRect rectText;
        rectText.x = rectFrame.x + 4;
        rectText.y = rect.y;
        rectText.width = rectFrame.width - 8;
        rectText.height = height;

        wxRect rectLabel;
        DrawLabel(dc, label, wxNullBitmap,
                  rectText, flags, alignment, indexAccel, &rectLabel);
        rectLabel.x -= 1;
        rectLabel.width += 2;

        StandardDrawFrame(dc, rectFrame, rectLabel);

        // GTK+ does it like this
        dc.SetPen(m_penHighlight);
        dc.DrawPoint(rectText.x, rectFrame.y);
        dc.DrawPoint(rectText.x + rectLabel.width - 3, rectFrame.y);
    }
    else
    {
        // just draw the complete frame
        DrawShadedRect(dc, &rectFrame, m_penDarkGrey, m_penHighlight);
        DrawShadedRect(dc, &rectFrame, m_penHighlight, m_penDarkGrey);
    }
}

// ----------------------------------------------------------------------------
// label
// ----------------------------------------------------------------------------

void wxGTKRenderer::DrawLabel(wxDC& dc,
                              const wxString& label,
                              const wxBitmap& image,
                              const wxRect& rect,
                              int flags,
                              int alignment,
                              int indexAccel,
                              wxRect *rectBounds)
{
    if ( flags & wxCONTROL_DISABLED )
    {
        // make the text grey and draw a shade for it
        dc.SetTextForeground(0xe0e0e0);
        wxRect rectShadow = rect;
        rectShadow.x++;
        rectShadow.y++;
        dc.DrawLabel(label, rectShadow, alignment, indexAccel);
        dc.SetTextForeground(0x7f7f7f);
    }

    dc.DrawLabel(label, image, rect, alignment, indexAccel, rectBounds);
}

void wxGTKRenderer::DrawItem(wxDC& dc,
                             const wxString& label,
                             const wxRect& rect,
                             int flags)
{
    wxLogTrace(_T("listbox"), _T("drawing item '%s' at (%d, %d)-(%d, %d)"),
               label.c_str(),
               rect.x, rect.y,
               rect.x + rect.width, rect.y + rect.height);

    wxColour colFg;
    if ( flags & wxCONTROL_SELECTED )
    {
        dc.SetBrush(wxBrush(m_scheme->Get(wxColourScheme::HIGHLIGHT), wxSOLID));
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRectangle(rect);

        colFg = dc.GetTextForeground();
        dc.SetTextForeground(m_scheme->Get(wxColourScheme::HIGHLIGHT_TEXT));
    }

    if ( flags & wxCONTROL_FOCUSED )
    {
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        wxRect rectFocus = rect;
        DrawRect(dc, &rectFocus, m_penBlack);
    }

    wxRect rectText = rect;
    rectText.x += 2;
    rectText.y++;
    dc.DrawLabel(label, wxNullBitmap, rectText);

    if ( flags & wxCONTROL_SELECTED )
    {
        dc.SetBackgroundMode(wxTRANSPARENT);
    }

    // restore the text colour
    if ( colFg.Ok() )
    {
        dc.SetTextForeground(colFg);
    }
}

// ----------------------------------------------------------------------------
// background
// ----------------------------------------------------------------------------

void wxGTKRenderer::DoDrawBackground(wxDC& dc,
                                     const wxColour& col,
                                     const wxRect& rect)
{
    wxBrush brush(col, wxSOLID);
    dc.SetBrush(brush);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(rect);
}

void wxGTKRenderer::DrawBackground(wxDC& dc,
                                   const wxColour& col,
                                   const wxRect& rect,
                                   int flags)
{
    DoDrawBackground(dc,
                     col.Ok() ? col
                               : GetBackgroundColour(flags),
                     rect);
}

// ----------------------------------------------------------------------------
// scrollbar
// ----------------------------------------------------------------------------

void wxGTKRenderer::DrawArrowBorder(wxDC& dc,
                                    wxRect *rect,
                                    wxDirection dir)
{
    static const wxDirection sides[] =
    {
        wxUP, wxLEFT, wxRIGHT, wxDOWN
    };

    wxRect rect1, rect2, rectInner;
    rect1 =
    rect2 =
    rectInner = *rect;

    rect2.Inflate(-1);
    rectInner.Inflate(-2);

    DoDrawBackground(dc, m_scheme->Get(wxColourScheme::SCROLLBAR), *rect);

    // find the side not to draw and also adjust the rectangles to compensate
    // for it
    wxDirection sideToOmit;
    switch ( dir )
    {
        case wxUP:
            sideToOmit = wxDOWN;
            rect2.height += 1;
            rectInner.height += 1;
            break;

        case wxDOWN:
            sideToOmit = wxUP;
            rect2.y -= 1;
            rect2.height += 1;
            rectInner.y -= 2;
            rectInner.height += 1;
            break;

        case wxLEFT:
            sideToOmit = wxRIGHT;
            rect2.width += 1;
            rectInner.width += 1;
            break;

        case wxRIGHT:
            sideToOmit = wxLEFT;
            rect2.x -= 1;
            rect2.width += 1;
            rectInner.x -= 2;
            rectInner.width += 1;
            break;

        default:
            wxFAIL_MSG(_T("unknown arrow direction"));
            return;
    }

    // the outer rect first
    size_t n;
    for ( n = 0; n < WXSIZEOF(sides); n++ )
    {
        wxDirection side = sides[n];
        if ( side == sideToOmit )
            continue;

        DrawAntiShadedRectSide(dc, rect1, m_penDarkGrey, m_penHighlight, side);
    }

    // and then the inner one
    for ( n = 0; n < WXSIZEOF(sides); n++ )
    {
        wxDirection side = sides[n];
        if ( side == sideToOmit )
            continue;

        DrawAntiShadedRectSide(dc, rect2, m_penBlack, m_penGrey, side);
    }

    *rect = rectInner;
}

// gtk_default_draw_arrow() takes ~350 lines and we can't do much better here
// these people are just crazy :-(
void wxGTKRenderer::DrawArrow(wxDC& dc,
                              wxDirection dir,
                              const wxRect& rectArrow,
                              int flags)
{
    // first of all, draw the border around it - but we don't want the border
    // on the side opposite to the arrow point
    wxRect rect = rectArrow;
    DrawArrowBorder(dc, &rect, dir);

    enum
    {
        Point_First,
        Point_Second,
        Point_Third,
        Point_Max
    };

    wxPoint ptArrow[Point_Max];

    wxColour colInside = m_scheme->Get(wxColourScheme::CONTROL, flags);
    wxPen penShadow[4];
    if ( flags & wxCONTROL_PRESSED )
    {
        penShadow[0] = m_penDarkGrey;
        penShadow[1] = m_penHighlight;
        penShadow[2] = wxNullPen;
        penShadow[3] = m_penBlack;
    }
    else // normal arrow
    {
        penShadow[0] = m_penHighlight;
        penShadow[1] = m_penBlack;
        penShadow[2] = m_penDarkGrey;
        penShadow[3] = wxNullPen;
    }

    wxCoord middle;
    if ( dir == wxUP || dir == wxDOWN )
    {
        // horz middle
        middle = (rect.GetRight() + rect.GetLeft() + 1) / 2;
    }
    else // horz arrow
    {
        middle = (rect.GetTop() + rect.GetBottom() + 1) / 2;
    }

    // draw the arrow interior
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(colInside, wxSOLID));

    switch ( dir )
    {
        case wxUP:
            ptArrow[Point_First].x = rect.GetLeft();
            ptArrow[Point_First].y = rect.GetBottom();
            ptArrow[Point_Second].x = middle;
            ptArrow[Point_Second].y = rect.GetTop();
            ptArrow[Point_Third].x = rect.GetRight();
            ptArrow[Point_Third].y = rect.GetBottom();
            break;

        case wxDOWN:
            ptArrow[Point_First] = rect.GetPosition();
            ptArrow[Point_Second].x = middle;
            ptArrow[Point_Second].y = rect.GetBottom();
            ptArrow[Point_Third].x = rect.GetRight();
            ptArrow[Point_Third].y = rect.GetTop();
            break;

        case wxLEFT:
            ptArrow[Point_First].x = rect.GetRight();
            ptArrow[Point_First].y = rect.GetTop();
            ptArrow[Point_Second].x = rect.GetLeft();
            ptArrow[Point_Second].y = middle;
            ptArrow[Point_Third].x = rect.GetRight();
            ptArrow[Point_Third].y = rect.GetBottom();
            break;

        case wxRIGHT:
            ptArrow[Point_First] = rect.GetPosition();
            ptArrow[Point_Second].x = rect.GetRight();
            ptArrow[Point_Second].y = middle;
            ptArrow[Point_Third].x = rect.GetLeft();
            ptArrow[Point_Third].y = rect.GetBottom();
            break;

        default:
            wxFAIL_MSG(_T("unknown arrow direction"));
    }

    dc.DrawPolygon(WXSIZEOF(ptArrow), ptArrow);

    // draw the arrow border
    dc.SetPen(penShadow[0]);
    switch ( dir )
    {
        case wxUP:
            dc.DrawLine(ptArrow[Point_Second], ptArrow[Point_First]);
            dc.DrawPoint(ptArrow[Point_First]);
            if ( penShadow[3].Ok() )
            {
                dc.SetPen(penShadow[3]);
                dc.DrawLine(ptArrow[Point_First].x + 1, ptArrow[Point_First].y,
                            ptArrow[Point_Second].x, ptArrow[Point_Second].y);
            }
            dc.SetPen(penShadow[1]);
            dc.DrawLine(ptArrow[Point_Second].x + 1, ptArrow[Point_Second].y + 1,
                        ptArrow[Point_Third].x, ptArrow[Point_Third].y);
            dc.DrawPoint(ptArrow[Point_Third]);
            dc.DrawLine(ptArrow[Point_Third].x - 2, ptArrow[Point_Third].y,
                        ptArrow[Point_First].x + 1, ptArrow[Point_First].y);
            if ( penShadow[2].Ok() )
            {
                dc.SetPen(penShadow[2]);
                dc.DrawLine(ptArrow[Point_Third].x - 1, ptArrow[Point_Third].y,
                            ptArrow[Point_Second].x, ptArrow[Point_Second].y + 1);
                dc.DrawLine(ptArrow[Point_Third].x - 1, ptArrow[Point_Third].y - 1,
                            ptArrow[Point_First].x + 2, ptArrow[Point_First].y - 1);
            }
            break;

        case wxDOWN:
            dc.DrawLine(ptArrow[Point_First], ptArrow[Point_Second]);
            dc.DrawLine(ptArrow[Point_First].x + 2, ptArrow[Point_First].y,
                        ptArrow[Point_Third].x - 1, ptArrow[Point_Third].y);
            if ( penShadow[2].Ok() )
            {
                dc.SetPen(penShadow[2]);
                dc.DrawLine(ptArrow[Point_Second].x, ptArrow[Point_Second].y - 1,
                            ptArrow[Point_Third].x - 1, ptArrow[Point_Third].y - 1);
            }
            dc.SetPen(penShadow[1]);
            dc.DrawLine(ptArrow[Point_Second], ptArrow[Point_Third]);
            dc.DrawPoint(ptArrow[Point_Third]);
            break;

        case wxLEFT:
            dc.DrawLine(ptArrow[Point_Second], ptArrow[Point_First]);
            dc.DrawPoint(ptArrow[Point_First]);
            if ( penShadow[2].Ok() )
            {
                dc.SetPen(penShadow[2]);
                dc.DrawLine(ptArrow[Point_Third].x - 1, ptArrow[Point_Third].y,
                            ptArrow[Point_First].x - 1, ptArrow[Point_First].y + 2);
                dc.DrawLine(ptArrow[Point_Third].x, ptArrow[Point_Third].y,
                            ptArrow[Point_Second].x + 2, ptArrow[Point_Second].y + 1);
            }
            dc.SetPen(penShadow[1]);
            dc.DrawLine(ptArrow[Point_Third].x, ptArrow[Point_Third].y,
                        ptArrow[Point_First].x, ptArrow[Point_First].y + 1);
            dc.DrawLine(ptArrow[Point_Second].x + 1, ptArrow[Point_Second].y + 1,
                        ptArrow[Point_Third].x - 1, ptArrow[Point_Third].y);
            break;

        case wxRIGHT:
            dc.DrawLine(ptArrow[Point_First], ptArrow[Point_Third]);
            dc.DrawLine(ptArrow[Point_First].x + 2, ptArrow[Point_First].y + 1,
                        ptArrow[Point_Second].x, ptArrow[Point_Second].y);
            dc.SetPen(penShadow[1]);
            dc.DrawLine(ptArrow[Point_Second], ptArrow[Point_Third]);
            dc.DrawPoint(ptArrow[Point_Third]);
            break;

        default:
            wxFAIL_MSG(_T("unknown arrow direction"));
            return;
    }
}

void wxGTKRenderer::DrawThumbBorder(wxDC& dc,
                                    wxRect *rect,
                                    wxOrientation orient)
{
    if ( orient == wxVERTICAL )
    {
        DrawAntiShadedRectSide(dc, *rect, m_penDarkGrey, m_penHighlight,
                               wxLEFT);
        DrawAntiShadedRectSide(dc, *rect, m_penDarkGrey, m_penHighlight,
                               wxRIGHT);
        rect->Inflate(-1, 0);

        DrawAntiShadedRectSide(dc, *rect, m_penBlack, m_penGrey,
                               wxLEFT);
        DrawAntiShadedRectSide(dc, *rect, m_penBlack, m_penGrey,
                               wxRIGHT);
        rect->Inflate(-1, 0);
    }
    else
    {
        DrawAntiShadedRectSide(dc, *rect, m_penDarkGrey, m_penHighlight,
                               wxUP);
        DrawAntiShadedRectSide(dc, *rect, m_penDarkGrey, m_penHighlight,
                               wxDOWN);
        rect->Inflate(0, -1);

        DrawAntiShadedRectSide(dc, *rect, m_penBlack, m_penGrey,
                               wxUP);
        DrawAntiShadedRectSide(dc, *rect, m_penBlack, m_penGrey,
                               wxDOWN);
        rect->Inflate(0, -1);
    }
}

void wxGTKRenderer::DrawScrollbarThumb(wxDC& dc,
                                       wxOrientation orient,
                                       const wxRect& rect,
                                       int flags)
{
    // the thumb is never pressed never has focus border under GTK and the
    // scrollbar background never changes at all
    int flagsThumb = flags & ~(wxCONTROL_PRESSED | wxCONTROL_FOCUSED);

    // we don't want the border in the direction of the scrollbar movement
    wxRect rectThumb = rect;
    DrawThumbBorder(dc, &rectThumb, orient);

    DrawButtonBorder(dc, rectThumb, flagsThumb, &rectThumb);
    DrawBackground(dc, wxNullColour, rectThumb, flagsThumb);
}

void wxGTKRenderer::DrawScrollbarShaft(wxDC& dc,
                                       wxOrientation orient,
                                       const wxRect& rect,
                                       int flags)
{
    wxRect rectBar = rect;
    DrawThumbBorder(dc, &rectBar, orient);
    DoDrawBackground(dc, m_scheme->Get(wxColourScheme::SCROLLBAR), rectBar);
}

void wxGTKRenderer::DrawScrollCorner(wxDC& dc, const wxRect& rect)
{
    DoDrawBackground(dc, m_scheme->Get(wxColourScheme::CONTROL), rect);
}

wxRect wxGTKRenderer::GetScrollbarRect(const wxScrollBar *scrollbar,
                                       wxScrollBar::Element elem,
                                       int thumbPos) const
{
    return StandardGetScrollbarRect(scrollbar, elem,
                                    thumbPos, GetScrollbarArrowSize(scrollbar));
}

wxCoord wxGTKRenderer::GetScrollbarSize(const wxScrollBar *scrollbar)
{
    return StandardScrollBarSize(scrollbar, GetScrollbarArrowSize(scrollbar));
}

wxHitTest wxGTKRenderer::HitTestScrollbar(const wxScrollBar *scrollbar,
                                          const wxPoint& pt) const
{
    return StandardHitTestScrollbar(scrollbar, pt,
                                    GetScrollbarArrowSize(scrollbar));
}

wxCoord wxGTKRenderer::ScrollbarToPixel(const wxScrollBar *scrollbar,
                                        int thumbPos)
{
    return StandardScrollbarToPixel(scrollbar, thumbPos,
                                    GetScrollbarArrowSize(scrollbar));
}

int wxGTKRenderer::PixelToScrollbar(const wxScrollBar *scrollbar,
                                    wxCoord coord)
{
    return StandardPixelToScrollbar(scrollbar, coord,
                                    GetScrollbarArrowSize(scrollbar));
}

// ----------------------------------------------------------------------------
// size adjustments
// ----------------------------------------------------------------------------

void wxGTKRenderer::AdjustSize(wxSize *size, const wxWindow *window)
{
    if ( wxDynamicCast(window, wxButton) )
    {
        // TODO: this is ad hoc...
        size->x += 3*window->GetCharWidth();
        wxCoord minBtnHeight = (11*(window->GetCharHeight() + 8))/10;
        if ( size->y < minBtnHeight )
            size->y = minBtnHeight;

        // button border width
        size->y += 4;
    }
    else if ( wxDynamicCast(window, wxScrollBar) )
    {
        // we only set the width of vert scrollbars and height of the
        // horizontal ones
        if ( window->GetWindowStyle() & wxSB_HORIZONTAL )
            size->y = m_sizeScrollbarArrow.x;
        else
            size->x = m_sizeScrollbarArrow.x;
    }
    else
    {
        // take into account the border width
        wxRect rectBorder = GetBorderDimensions(window->GetBorder());
        size->x += rectBorder.x + rectBorder.width;
        size->y += rectBorder.y + rectBorder.height;
    }
}

// ============================================================================
// wxInputHandler
// ============================================================================

// ----------------------------------------------------------------------------
// wxGTKInputHandler
// ----------------------------------------------------------------------------

wxGTKInputHandler::wxGTKInputHandler(wxGTKRenderer *renderer)
{
    m_renderer = renderer;
}

bool wxGTKInputHandler::HandleKey(wxControl *control,
                                  const wxKeyEvent& event,
                                  bool pressed)
{
    return FALSE;
}

bool wxGTKInputHandler::HandleMouse(wxControl *control,
                                    const wxMouseEvent& event)
{
    // clicking on the control gives it focus
    if ( event.ButtonDown() )
    {
        control->SetFocus();

        return TRUE;
    }

    return FALSE;
}

bool wxGTKInputHandler::HandleMouseMove(wxControl *control,
                                        const wxMouseEvent& event)
{
    if ( event.Entering() )
    {
        control->SetCurrent(TRUE);
    }
    else if ( event.Leaving() )
    {
        control->SetCurrent(FALSE);
    }
    else
    {
        return FALSE;
    }

    // don't refresh static controls uselessly - they never react to this
    if ( control->AcceptsFocus() )
    {
        control->Refresh();
    }

    return TRUE;
}


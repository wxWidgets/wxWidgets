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
    #include "wx/dc.h"
    #include "wx/window.h"

    #include "wx/button.h"
    #include "wx/scrolbar.h"
#endif // WX_PRECOMP

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/colschem.h"
#include "wx/univ/theme.h"

// ----------------------------------------------------------------------------
// wxGTKRenderer: draw the GUI elements in GTK style
// ----------------------------------------------------------------------------

class wxGTKRenderer : public wxRenderer
{
public:
    wxGTKRenderer(const wxColourScheme *scheme);

    // implement the base class pure virtuals
    virtual void DrawBackground(wxDC& dc,
                                const wxRect& rect,
                                int flags = 0);
    virtual void DrawLabel(wxDC& dc,
                           const wxString& label,
                           const wxRect& rect,
                           int flags = 0,
                           int alignment = wxALIGN_LEFT | wxALIGN_TOP,
                           int indexAccel = -1);
    virtual void DrawBorder(wxDC& dc,
                            wxBorder border,
                            const wxRect& rect,
                            int flags = 0,
                            wxRect *rectIn = (wxRect *)NULL);
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
    virtual void DrawScrollbar(wxDC& dc,
                               wxOrientation orient,
                               int thumbPosStart,
                               int thumbPosEnd,
                               const wxRect& rect,
                               int flags = 0,
                               int extraFlags = 0);

    virtual void AdjustSize(wxSize *size, const wxWindow *window);

protected:
    // DrawBackground() helper
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

    // draw the normal 3D border
    void DrawRaisedBorder(wxDC& dc, wxRect *rect);

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
    virtual wxControlActions Map(const wxKeyEvent& event, bool pressed);
    virtual wxControlActions Map(const wxMouseEvent& event);
};

class wxGTKButtonInputHandler : public wxGTKInputHandler
{
public:
    wxGTKButtonInputHandler();

    virtual wxControlActions Map(const wxKeyEvent& event, bool pressed);
    virtual wxControlActions Map(const wxMouseEvent& event);

private:
    wxWindow *m_winCapture;
    bool      m_winHasMouse;
};

class wxGTKScrollBarInputHandler : public wxGTKInputHandler
{
public:
    wxGTKScrollBarInputHandler();

    virtual wxControlActions Map(const wxKeyEvent& event, bool pressed);
    virtual wxControlActions Map(const wxMouseEvent& event);

private:
    wxWindow *m_winCapture;
    bool      m_winHasMouse;
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
    virtual wxColourScheme *GetColourScheme(const wxString& control) { return m_scheme; }

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
        n = m_handlerNames.Add(control);

        if ( control == wxCONTROL_BUTTON )
            handler = new wxGTKButtonInputHandler;
        else if ( control == wxCONTROL_SCROLLBAR )
            handler = new wxGTKScrollBarInputHandler;
        else
        {
            wxASSERT_MSG( control == wxCONTROL_DEFAULT,
                          _T("no input handler defined for this control") );

            handler = new wxGTKInputHandler;
        }

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
                                    return wxColour(0x7f7f7f);
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

        case HIGHLIGHT:         return wxColour(0x0000ff);
        case HIGHLIGHT_TEXT:    return wxColour(0x00ffff);

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
    m_sizeScrollbarArrow = wxSize(12, 12);

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
            DrawShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
            DrawShadedRect(dc, &rect, m_penBlack, m_penLightGrey);
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
            wxFAIL_MSG(_T("unknwon border type"));
            // fall through

        case wxBORDER_DEFAULT:
        case wxBORDER_NONE:
            break;
    }

    if ( rectIn )
        *rectIn = rect;
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
                           flags & wxCONTROL_CURRENT ? m_penHighlight
                                                    : m_penLightGrey,
                           m_penDarkGrey);
    }

    if ( rectIn )
    {
        *rectIn = rect;
    }
}

// ----------------------------------------------------------------------------
// frame
// ----------------------------------------------------------------------------

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
    }

    // draw the frame
    DrawShadedRect(dc, &rectFrame, m_penDarkGrey, m_penHighlight);
    DrawShadedRect(dc, &rectFrame, m_penHighlight, m_penDarkGrey);

    // and overwrite it with label (if any)
    if ( !label.empty() )
    {
        // TODO: the +5 and space insertion should be customizable

        wxRect rectText;
        rectText.x = rectFrame.x + 5;
        rectText.y = rect.y;
        rectText.width = rectFrame.width - 7; // +2 border width
        rectText.height = height;

        wxString label2;
        label2 << _T(' ') << label << _T(' ');
        if ( indexAccel != -1 )
        {
            // adjust it as we prepended a space
            indexAccel++;
        }

        dc.SetBackgroundMode(wxSOLID);
        DrawLabel(dc, label2, rectText, flags, alignment, indexAccel);
        dc.SetBackgroundMode(wxTRANSPARENT);
    }
}

// ----------------------------------------------------------------------------
// label
// ----------------------------------------------------------------------------

void wxGTKRenderer::DrawLabel(wxDC& dc,
                                const wxString& label,
                                const wxRect& rect,
                                int flags,
                                int alignment,
                                int indexAccel)
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

    dc.DrawLabel(label, rect, alignment, indexAccel);
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
                                   const wxRect& rect,
                                   int flags)
{
    DoDrawBackground(dc, m_scheme->Get(wxColourScheme::CONTROL, flags), rect);
}

// ----------------------------------------------------------------------------
// scrollbar
// ----------------------------------------------------------------------------

// gtk_default_draw_arrow() takes ~350 lines and we can't do much better here
// these people are just crazy :-(
void wxGTKRenderer::DrawArrow(wxDC& dc,
                              wxDirection dir,
                              const wxRect& rect,
                              int flags)
{
    enum
    {
        Point_First,
        Point_Second,
        Point_Third,
        Point_Max
    };

    wxPoint ptArrow[Point_Max];

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
    dc.SetBrush(wxBrush(m_scheme->Get(wxColourScheme::CONTROL), wxSOLID));

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
            return;
    }

    dc.DrawPolygon(WXSIZEOF(ptArrow), ptArrow);

    // draw the arrow border
    dc.SetPen(m_penHighlight);
    switch ( dir )
    {
        case wxUP:
            dc.DrawLine(ptArrow[Point_Second], ptArrow[Point_First]);
            dc.DrawPoint(ptArrow[Point_First]);
            dc.SetPen(m_penBlack);
            dc.DrawLine(ptArrow[Point_Second].x + 1, ptArrow[Point_Second].y + 1,
                        ptArrow[Point_Third].x, ptArrow[Point_Third].y);
            dc.DrawPoint(ptArrow[Point_Third]);
            dc.DrawLine(ptArrow[Point_Third].x - 2, ptArrow[Point_Third].y,
                        ptArrow[Point_First].x + 1, ptArrow[Point_First].y);
            dc.SetPen(m_penDarkGrey);
            dc.DrawLine(ptArrow[Point_Third].x - 1, ptArrow[Point_Third].y,
                        ptArrow[Point_Second].x, ptArrow[Point_Second].y + 1);
            dc.DrawLine(ptArrow[Point_Third].x - 1, ptArrow[Point_Third].y - 1,
                        ptArrow[Point_First].x + 2, ptArrow[Point_First].y - 1);
            break;

        case wxDOWN:
            dc.DrawLine(ptArrow[Point_First], ptArrow[Point_Second]);
            dc.DrawLine(ptArrow[Point_First].x + 2, ptArrow[Point_First].y,
                        ptArrow[Point_Third].x - 1, ptArrow[Point_Third].y);
            dc.SetPen(m_penDarkGrey);
            dc.DrawLine(ptArrow[Point_Second].x, ptArrow[Point_Second].y - 1,
                        ptArrow[Point_Third].x - 1, ptArrow[Point_Third].y - 1);
            dc.SetPen(m_penBlack);
            dc.DrawLine(ptArrow[Point_Second], ptArrow[Point_Third]);
            dc.DrawPoint(ptArrow[Point_Third]);
            break;

        case wxLEFT:
            dc.DrawLine(ptArrow[Point_Second], ptArrow[Point_First]);
            dc.DrawPoint(ptArrow[Point_First]);
            dc.SetPen(m_penDarkGrey);
            dc.DrawLine(ptArrow[Point_Third].x - 1, ptArrow[Point_Third].y,
                        ptArrow[Point_First].x - 1, ptArrow[Point_First].y + 2);
            dc.DrawLine(ptArrow[Point_Third].x, ptArrow[Point_Third].y,
                        ptArrow[Point_Second].x + 2, ptArrow[Point_Second].y + 1);
            dc.SetPen(m_penBlack);
            dc.DrawLine(ptArrow[Point_Third].x, ptArrow[Point_Third].y,
                        ptArrow[Point_First].x, ptArrow[Point_First].y + 1);
            dc.DrawLine(ptArrow[Point_Second].x + 1, ptArrow[Point_Second].y + 1,
                        ptArrow[Point_Third].x - 1, ptArrow[Point_Third].y);
            break;

        case wxRIGHT:
            dc.DrawLine(ptArrow[Point_First], ptArrow[Point_Third]);
            dc.DrawLine(ptArrow[Point_First].x + 2, ptArrow[Point_First].y + 1,
                        ptArrow[Point_Second].x, ptArrow[Point_Second].y);
            dc.SetPen(m_penBlack);
            dc.DrawLine(ptArrow[Point_Second], ptArrow[Point_Third]);
            dc.DrawPoint(ptArrow[Point_Third]);
            break;

        default:
            wxFAIL_MSG(_T("unknown arrow direction"));
            return;
    }
}

void wxGTKRenderer::DrawScrollbar(wxDC& dc,
                                  wxOrientation orient,
                                  int thumbPosStart,
                                  int thumbPosEnd,
                                  const wxRect& rect,
                                  int flags,
                                  int extraFlags)
{
    // handling of wxCONTROL_CURRENT flag is complicated for the scrollbar
    // because, though it's only one window, either an arrow or a thumb may be
    // highlighted depending on where exactly the mouse is
    int flagsArrows[2], flagsThumb;
    flagsArrows[0] =
    flagsArrows[1] =
    flagsThumb = flags & ~wxCONTROL_CURRENT;
    if ( flags & wxCONTROL_CURRENT )
    {
        if ( extraFlags & wxScrollBar::Highlight_Arrow1 )
            flagsArrows[0] |= wxCONTROL_CURRENT;
        if ( extraFlags & wxScrollBar::Highlight_Arrow2 )
            flagsArrows[1] |= wxCONTROL_CURRENT;
        if ( extraFlags & wxScrollBar::Highlight_Thumb )
            flagsThumb |= wxCONTROL_CURRENT;
    }

    // first draw the scrollbar area background
    wxRect rectBar = rect;
    DrawAntiShadedRect(dc, &rectBar, m_penDarkGrey, m_penHighlight);
    DrawAntiShadedRect(dc, &rectBar, m_penBlack, m_penGrey);
    DoDrawBackground(dc, m_scheme->Get(wxColourScheme::SCROLLBAR), rectBar);

    // next draw the arrows at the ends
    wxRect rectArrow[2];
    wxDirection arrowDir[2];

    rectArrow[0] =
    rectArrow[1] = rectBar;
    if ( orient == wxVERTICAL )
    {
        rectArrow[0].height =
        rectArrow[1].height = m_sizeScrollbarArrow.y - 1;
        rectArrow[1].y = rectBar.GetBottom() - rectArrow[1].height + 1;

        arrowDir[0] = wxUP;
        arrowDir[1] = wxDOWN;
    }
    else // horizontal
    {
        rectArrow[0].width =
        rectArrow[1].width = m_sizeScrollbarArrow.x - 1;
        rectArrow[1].x = rectBar.GetRight() - rectArrow[1].width + 1;

        arrowDir[0] = wxLEFT;
        arrowDir[1] = wxRIGHT;
    }

    for ( size_t nArrow = 0; nArrow < 2; nArrow++ )
    {
        DrawArrow(dc, arrowDir[nArrow], rectArrow[nArrow], flags);
    }

    // and, finally, the thumb, if any
    if ( thumbPosStart < thumbPosEnd )
    {
        wxRect rectThumb;
        if ( orient == wxVERTICAL )
        {
            rectBar.Inflate(0, -m_sizeScrollbarArrow.y);
            rectThumb = rectBar;
            rectThumb.y += (rectBar.height*thumbPosStart)/100;
            rectThumb.height = (rectBar.height*(thumbPosEnd - thumbPosStart))/100;
        }
        else // horizontal
        {
            rectBar.Inflate(-m_sizeScrollbarArrow.x, 0);
            rectThumb = rectBar;
            rectThumb.x += (rectBar.width*thumbPosStart)/100;
            rectThumb.width = (rectBar.width*(thumbPosEnd - thumbPosStart))/100;
        }

        // the thumb never has focus border under GTK
        DrawButtonBorder(dc, rectThumb, flags & ~wxCONTROL_FOCUSED, &rectThumb);
        DrawBackground(dc, rectThumb, flags);
    }
}

// ----------------------------------------------------------------------------
// size adjustments
// ----------------------------------------------------------------------------

void wxGTKRenderer::AdjustSize(wxSize *size, const wxWindow *window)
{
    if ( wxDynamicCast(window, wxButton) )
    {
        // TODO
        size->x += 3*window->GetCharWidth();
        size->y = (11*(window->GetCharHeight() + 8))/10;
    }
    else
    {
        if ( wxDynamicCast(window, wxScrollBar) )
        {
            // we only set the width of vert scrollbars and height of the
            // horizontal ones
            if ( window->GetWindowStyle() & wxSB_HORIZONTAL )
                size->y = m_sizeScrollbarArrow.y;
            else
                size->x = m_sizeScrollbarArrow.x;
        }

        // take into account the border width
        wxBorder border = (wxBorder)(window->GetWindowStyle() & wxBORDER_MASK);
        switch ( border )
        {
            case wxBORDER_SUNKEN:
            case wxBORDER_RAISED:
                size->x += 4;
                size->y += 4;
                break;

            case wxBORDER_SIMPLE:
            case wxBORDER_STATIC:
                size->x += 2;
                size->y += 2;
                break;

            case wxBORDER_DOUBLE:
                size->x += 6;
                size->y += 6;
                break;

            default:
                wxFAIL_MSG(_T("unknwon border type"));
                // fall through

            case wxBORDER_DEFAULT:
            case wxBORDER_NONE:
                break;
        }
    }
}

// ============================================================================
// wxInputHandler
// ============================================================================

// ----------------------------------------------------------------------------
// wxGTKInputHandler
// ----------------------------------------------------------------------------

wxControlActions wxGTKInputHandler::Map(const wxKeyEvent& event, bool pressed)
{
    return wxACTION_NONE;
}

wxControlActions wxGTKInputHandler::Map(const wxMouseEvent& event)
{
    if ( event.Entering() )
        return wxACTION_HIGHLIGHT;
    else if ( event.Leaving() )
        return wxACTION_UNHIGHLIGHT;

    return wxACTION_NONE;
}

// ----------------------------------------------------------------------------
// wxGTKButtonInputHandler
// ----------------------------------------------------------------------------

wxGTKButtonInputHandler::wxGTKButtonInputHandler()
{
    m_winCapture = NULL;
}

wxControlActions wxGTKButtonInputHandler::Map(const wxKeyEvent& event,
                                              bool pressed)
{
    int keycode = event.GetKeyCode();
    if ( keycode == WXK_SPACE || keycode == WXK_RETURN )
    {
        return wxACTION_BUTTON_TOGGLE;
    }

    return wxGTKInputHandler::Map(event, pressed);
}

wxControlActions wxGTKButtonInputHandler::Map(const wxMouseEvent& event)
{
    if ( event.IsButton() )
    {
        if ( event.ButtonDown() )
        {
            m_winCapture = wxWindow::FindFocus();
            m_winCapture->CaptureMouse();
            m_winHasMouse = TRUE;
        }
        else // up
        {
            m_winCapture->ReleaseMouse();
            m_winCapture = NULL;

            if ( !m_winHasMouse )
            {
                // the mouse was released outside the window, this doesn't
                // count as a click
                return wxGTKInputHandler::Map(event);
            }
        }

        return wxACTION_BUTTON_TOGGLE;
    }

    // leaving the button should remove its pressed state
    if ( event.Leaving() )
    {
        wxControlActions actions;
        if ( m_winCapture )
        {
            // we do have a pressed button
            actions.Add(wxACTION_BUTTON_RELEASE);

            // remember that the mouse is now outside
            m_winHasMouse = FALSE;
        }

        actions.Add(wxGTKInputHandler::Map(event));

        return actions;
    }

    // entering it back should make it pressed again if it had been pressed
    if ( event.Entering() )
    {
        wxControlActions actions;
        if ( m_winCapture )
        {
            // we do have a pressed button
            actions.Add(wxACTION_BUTTON_PRESS);

            // and the mouse is (back) inside it
            m_winHasMouse = TRUE;
        }

        actions.Add(wxGTKInputHandler::Map(event));

        return actions;
    }

    return wxGTKInputHandler::Map(event);
}

// ----------------------------------------------------------------------------
// wxGTKScrollBarInputHandler
// ----------------------------------------------------------------------------

wxGTKScrollBarInputHandler::wxGTKScrollBarInputHandler()
{
    m_winCapture = NULL;
}

wxControlActions wxGTKScrollBarInputHandler::Map(const wxKeyEvent& event,
                                                 bool pressed)
{
    return wxGTKInputHandler::Map(event, pressed);
}

wxControlActions wxGTKScrollBarInputHandler::Map(const wxMouseEvent& event)
{
    if ( event.Moving() )
    {
        // determin which part of the window mouse is in
    }

    return wxGTKInputHandler::Map(event);
}

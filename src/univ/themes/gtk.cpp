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
                               const int *flags = NULL);

    virtual void AdjustSize(wxSize *size, const wxWindow *window);

    // hit testing for the input handlers
    virtual wxHitTest HitTestScrollbar(wxScrollBar *scrollbar,
                                       const wxPoint& pt) const;
    
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
    wxGTKInputHandler(wxGTKRenderer *renderer);

    virtual wxControlActions Map(wxControl *control,
                                 const wxKeyEvent& event,
                                 bool pressed);
    virtual wxControlActions Map(wxControl *control,
                                 const wxMouseEvent& event);
    virtual bool OnMouseMove(wxControl *control, const wxMouseEvent& event);

protected:
    wxGTKRenderer *m_renderer;
};

class wxGTKButtonInputHandler : public wxGTKInputHandler
{
public:
    wxGTKButtonInputHandler(wxGTKRenderer *renderer);

    virtual wxControlActions Map(wxControl *control,
                                 const wxKeyEvent& event,
                                 bool pressed);
    virtual wxControlActions Map(wxControl *control,
                                 const wxMouseEvent& event);
    virtual bool OnMouseMove(wxControl *control, const wxMouseEvent& event);

private:
    wxWindow *m_winCapture;
    bool      m_winHasMouse;
};

class wxGTKScrollBarInputHandler : public wxGTKInputHandler
{
public:
    wxGTKScrollBarInputHandler(wxGTKRenderer *renderer);

    virtual wxControlActions Map(wxControl *control,
                                 const wxKeyEvent& event,
                                 bool pressed);
    virtual wxControlActions Map(wxControl *control,
                                 const wxMouseEvent& event);
    virtual bool OnMouseMove(wxControl *control, const wxMouseEvent& event);

private:
    // set or clear the specified flag on the scrollbar element corresponding
    // to m_htLast
    void SetElementState(wxScrollBar *scrollbar, int flag, bool doIt);

    // [un]highlight the scrollbar element corresponding to m_htLast
    void Highlight(wxScrollBar *scrollbar, bool doIt)
        { SetElementState(scrollbar, wxCONTROL_CURRENT, doIt); }

    // [un]press the scrollbar element corresponding to m_htLast
    void Press(wxScrollBar *scrollbar, bool doIt)
        { SetElementState(scrollbar, wxCONTROL_PRESSED, doIt); }

    wxWindow *m_winCapture;
    int       m_btnCapture;  // the mouse button which was captured mouse
    bool      m_winHasMouse;

    wxHitTest m_htLast;
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
        n = m_handlerNames.Add(control);

        if ( control == _T("wxButton") )
            handler = new wxGTKButtonInputHandler(m_renderer);
        else if ( control == _T("wxScrollBar") )
            handler = new wxGTKScrollBarInputHandler(m_renderer);
        else
            handler = new wxGTKInputHandler(m_renderer);

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
                           wxPen(GetBackgroundColour(flags), 0, wxSOLID),
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
    DoDrawBackground(dc, GetBackgroundColour(flags), rect);
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
            return;
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

void wxGTKRenderer::DrawScrollbar(wxDC& dc,
                                  wxOrientation orient,
                                  int thumbPosStart,
                                  int thumbPosEnd,
                                  const wxRect& rect,
                                  const int *flags)
{
#ifdef DEBUG_MOUSE
    wxLogDebug("Drawing the scrollbar (orientation = %s):\n"
               "\tarrow 1: 0x%04x\n"
               "\tarrow 2: 0x%04x\n"
               "\tthumb:   0x%04x\n"
               "\tthumb from %d to %d",
               orient == wxVERTICAL ? "vertical" : "horizontal",
               flags[wxScrollBar::Element_Arrow_Line_1],
               flags[wxScrollBar::Element_Arrow_Line_2],
               flags[wxScrollBar::Element_Thumb],
               thumbPosStart, thumbPosEnd);
#endif // DEBUG_MOUSE

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
        DrawArrow(dc, arrowDir[nArrow], rectArrow[nArrow],
                  flags[wxScrollBar::Element_Arrow_Line_1 + nArrow]);
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

        // the thumb is never pressed never has focus border under GTK and the
        // scrollbar background never changes at all
        int flagsThumb = flags[wxScrollBar::Element_Thumb] &
                            ~(wxCONTROL_PRESSED | wxCONTROL_FOCUSED);
        DrawButtonBorder(dc, rectThumb, flagsThumb, &rectThumb);
        DrawBackground(dc, rectThumb, flagsThumb);
    }
}

wxHitTest wxGTKRenderer::HitTestScrollbar(wxScrollBar *scrollbar,
                                          const wxPoint& pt) const
{
    return StandardHitTestScrollbar(scrollbar, pt, m_sizeScrollbarArrow);
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

wxGTKInputHandler::wxGTKInputHandler(wxGTKRenderer *renderer)
{
    m_renderer = renderer;
}

wxControlActions wxGTKInputHandler::Map(wxControl *control,
                                        const wxKeyEvent& event,
                                        bool pressed)
{
    return wxACTION_NONE;
}

wxControlActions wxGTKInputHandler::Map(wxControl *control,
                                        const wxMouseEvent& event)
{
    // clicking on the control gives it focus
    if ( event.ButtonDown() )
    {
        return wxACTION_FOCUS;
    }

    return wxACTION_NONE;
}

bool wxGTKInputHandler::OnMouseMove(wxControl *control,
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

    // highlighting changed
    return TRUE;
}

// ----------------------------------------------------------------------------
// wxGTKButtonInputHandler
// ----------------------------------------------------------------------------

wxGTKButtonInputHandler::wxGTKButtonInputHandler(wxGTKRenderer *renderer)
                       : wxGTKInputHandler(renderer)
{
    m_winCapture = NULL;
}

wxControlActions wxGTKButtonInputHandler::Map(wxControl *control,
                                              const wxKeyEvent& event,
                                              bool pressed)
{
    int keycode = event.GetKeyCode();
    if ( keycode == WXK_SPACE || keycode == WXK_RETURN )
    {
        return wxACTION_BUTTON_TOGGLE;
    }

    return wxGTKInputHandler::Map(control, event, pressed);
}

wxControlActions wxGTKButtonInputHandler::Map(wxControl *control,
                                              const wxMouseEvent& event)
{
    // the button has 2 states: pressed and normal with the following
    // transitions between them:
    //
    //      normal -> left down -> capture mouse and go to pressed state
    //      pressed -> left up inside -> generate click -> go to normal
    //                         outside ------------------>
    //
    // the other mouse buttons are ignored
    if ( event.Button(1) )
    {
        if ( event.ButtonDown(1) )
        {
            m_winCapture = control;
            m_winCapture->CaptureMouse();
            m_winHasMouse = TRUE;

            return wxACTION_BUTTON_PRESS;
        }
        else // up
        {
            m_winCapture->ReleaseMouse();
            m_winCapture = NULL;

            if ( m_winHasMouse )
            {
                // this will generate a click event
                return wxACTION_BUTTON_TOGGLE;
            }
            //else: the mouse was released outside the window, this doesn't
            //      count as a click
        }
    }

    return wxGTKInputHandler::Map(control, event);
}

bool wxGTKButtonInputHandler::OnMouseMove(wxControl *control,
                                          const wxMouseEvent& event)
{
    // leaving the button should remove its pressed state
    if ( event.Leaving() )
    {
        if ( m_winCapture )
        {
            // we do have a pressed button, so release it
            control->PerformAction(wxACTION_BUTTON_RELEASE, event);

            // remember that the mouse is now outside
            m_winHasMouse = FALSE;
        }
    }
    // and entering it back should make it pressed again if it had been
    // pressed
    else if ( event.Entering() )
    {
        if ( m_winCapture )
        {
            // we did have a pressed button which we released when leaving the
            // window, press it again
            control->PerformAction(wxACTION_BUTTON_PRESS, event);

            // and the mouse is (back) inside it
            m_winHasMouse = TRUE;
        }
    }

    return wxGTKInputHandler::OnMouseMove(control, event);
}

// ----------------------------------------------------------------------------
// wxGTKScrollBarInputHandler
// ----------------------------------------------------------------------------

wxGTKScrollBarInputHandler::wxGTKScrollBarInputHandler(wxGTKRenderer *renderer)
                          : wxGTKInputHandler(renderer)
{
    m_winCapture = NULL;
    m_htLast = wxHT_NOWHERE;
}

void wxGTKScrollBarInputHandler::SetElementState(wxScrollBar *control,
                                                 int flag,
                                                 bool doIt)
{
    wxScrollBar::Element elem;
    switch ( m_htLast )
    {
        case wxHT_SCROLLBAR_ARROW_LINE_1:
            elem = wxScrollBar::Element_Arrow_Line_1;
            break;

        case wxHT_SCROLLBAR_ARROW_LINE_2:
            elem = wxScrollBar::Element_Arrow_Line_2;
            break;

        case wxHT_SCROLLBAR_THUMB:
            elem = wxScrollBar::Element_Thumb;
            break;

            /*
               we don't highlight nor press the bar

        case wxHT_SCROLLBAR_BAR_1:
        case wxHT_SCROLLBAR_BAR_2:
            */

        default:
            elem = wxScrollBar::Element_Max;
    }

    if ( elem != wxScrollBar::Element_Max )
    {
        int flags = control->GetState(elem);
        if ( doIt )
            flags |= flag;
        else
            flags &= ~flag;
        control->SetState(elem, flags);
    }
}

wxControlActions wxGTKScrollBarInputHandler::Map(wxControl *control,
                                                 const wxKeyEvent& event,
                                                 bool pressed)
{
    // weirdly enough, GTK+ scrollbars don't have keyboard support - maybe we
    // should still have it though (TODO)?
    return wxGTKInputHandler::Map(control, event, pressed);
}

wxControlActions wxGTKScrollBarInputHandler::Map(wxControl *control,
                                                 const wxMouseEvent& event)
{
    if ( event.IsButton() )
    {
        // determine which part of the window mouse is in
        wxScrollBar *scrollbar = wxStaticCast(control, wxScrollBar);
        wxHitTest ht = m_renderer->HitTestScrollbar
                                   (
                                    scrollbar,
                                    event.GetPosition()
                                   );

        // when the mouse is pressed on any scrollbar element, we capture it
        // and hold capture until the same mouse button is released
        if ( event.ButtonDown() )
        {
            if ( !m_winCapture )
            {
                m_btnCapture = -1;
                for ( int i = 1; i <= 3; i++ )
                {
                    if ( event.ButtonDown(i) )
                    {
                        m_btnCapture = i;
                        break;
                    }
                }

                wxASSERT_MSG( m_btnCapture != -1, _T("unknown mouse button") );

                m_winCapture = control;
                m_winCapture->CaptureMouse();

                // generate the command
                bool hasAction = TRUE;
                wxControlAction action;
                switch ( ht )
                {
                    case wxHT_SCROLLBAR_ARROW_LINE_1:
                        action = wxACTION_SCROLL_LINE_UP;
                        break;

                    case wxHT_SCROLLBAR_ARROW_LINE_2:
                        action = wxACTION_SCROLL_LINE_DOWN;
                        break;

                    case wxHT_SCROLLBAR_BAR_1:
                        action = wxACTION_SCROLL_PAGE_UP;
                        break;

                    case wxHT_SCROLLBAR_BAR_2:
                        action = wxACTION_SCROLL_PAGE_DOWN;
                        break;

                    default:
                        hasAction = FALSE;
                }

                if ( hasAction )
                {
                    control->PerformAction(action, event);
                }

                // remove highlighting and press the arrow instead
                Highlight(scrollbar, FALSE);
                m_htLast = ht;
                Press(scrollbar, TRUE);
            }
            //else: mouse already captured, nothing to do
        }
        // release mouse if the *same* button went up
        else if ( event.ButtonUp(m_btnCapture) )
        {
            if ( m_winCapture )
            {
                m_winCapture->ReleaseMouse();
                m_winCapture = NULL;

                // unpress the arrow and highlight the current element
                Press(scrollbar, TRUE);
                m_htLast = ht;
                Highlight(scrollbar, TRUE);

                control->Refresh();
            }
            else
            {
                // this is not supposed to happen as the button can't go up
                // without going down previously and then we'd have
                // m_winCapture by now
                wxFAIL_MSG( _T("logic error in mouse capturing code") );
            }
        }
    }

    return wxGTKInputHandler::Map(control, event);
}

bool wxGTKScrollBarInputHandler::OnMouseMove(wxControl *control,
                                             const wxMouseEvent& event)
{
    if ( m_winCapture )
    {
        // everything is locked while the mouse is captured, so don't do
        // anything
        return FALSE;
    }

    wxScrollBar *scrollbar = wxStaticCast(control, wxScrollBar);

    if ( event.Moving() )
    {
        wxHitTest ht = m_renderer->HitTestScrollbar
                                   (
                                    scrollbar,
                                    event.GetPosition()
                                   );
        if ( ht == m_htLast )
        {
            // nothing changed
            return FALSE;
        }

#ifdef DEBUG_MOUSE
        wxLogDebug("Scrollbar::OnMouseMove: ht = %d", ht);
#endif // DEBUG_MOUSE

        Highlight(scrollbar, FALSE);
        m_htLast = ht;
        Highlight(scrollbar, TRUE);
    }
    else if ( event.Leaving() )
    {
        Highlight(scrollbar, FALSE);
        m_htLast = wxHT_NOWHERE;
    }

    // highlighting changed
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// Name:        univ/themes/win32.cpp
// Purpose:     wxUniversal theme implementing Win32-like LNF
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
    #include "wx/button.h"
    #include "wx/dc.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/theme.h"

// ----------------------------------------------------------------------------
// wxWin32Renderer: draw the GUI elements in Win32 style
// ----------------------------------------------------------------------------

class wxWin32Renderer : public wxRenderer
{
public:
    wxWin32Renderer();

    // implement the base class pure virtuals
    virtual void DrawBackground(wxDC& dc,
                                const wxColour& col,
                                const wxRect& rect,
                                int flags);
    virtual void DrawLabel(wxDC& dc,
                           const wxString& label,
                           const wxRect& rect,
                           int flags = wxRENDER_ENABLED,
                           int alignment = wxALIGN_LEFT | wxALIGN_TOP,
                           int indexAccel = -1);
    virtual void DrawBorder(wxDC& dc,
                            wxBorder border,
                            const wxRect& rect,
                            int flags = wxRENDER_ENABLED,
                            wxRect *rectIn = (wxRect *)NULL);
    virtual void DrawFrame(wxDC& dc,
                           const wxString& label,
                           const wxRect& rect,
                           int flags = wxRENDER_ENABLED,
                           int alignment = wxALIGN_LEFT,
                           int indexAccel = -1);
    virtual void DrawButtonBorder(wxDC& dc,
                                  const wxRect& rect,
                                  int flags = wxRENDER_ENABLED,
                                  wxRect *rectIn = (wxRect *)NULL);

    virtual void AdjustSize(wxSize *size, const wxWindow *window);

protected:
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

    // draw the normal 3D border
    void DrawRaisedBorder(wxDC& dc, wxRect *rect);

private:
    wxPen m_penBlack,
          m_penDarkGrey,
          m_penLightGrey,
          m_penWhite,
          m_penHighlight;
};

// ----------------------------------------------------------------------------
// wxWin32InputHandler and derived classes: process the keyboard and mouse
// messages according to Windows standards
// ----------------------------------------------------------------------------

class wxWin32InputHandler : public wxInputHandler
{
public:
    virtual wxControlActions Map(const wxKeyEvent& event, bool pressed);
    virtual wxControlActions Map(const wxMouseEvent& event);
};

class wxWin32ButtonInputHandler : public wxWin32InputHandler
{
public:
    wxWin32ButtonInputHandler();

    virtual wxControlActions Map(const wxKeyEvent& event, bool pressed);
    virtual wxControlActions Map(const wxMouseEvent& event);

private:
    wxWindow *m_winCapture;
};

// ----------------------------------------------------------------------------
// wxWin32ColourScheme
// ----------------------------------------------------------------------------

class wxWin32ColourScheme : public wxColourScheme
{
};

// ----------------------------------------------------------------------------
// wxWin32Theme
// ----------------------------------------------------------------------------

WX_DEFINE_ARRAY(wxInputHandler *, wxArrayHandlers);

class wxWin32Theme : public wxTheme
{
public:
    wxWin32Theme();
    virtual ~wxWin32Theme();

    virtual wxRenderer *GetRenderer() { return m_renderer; }
    virtual wxInputHandler *GetInputHandler(const wxString& control);
    virtual wxColourScheme *GetColourScheme() { return m_scheme; }

private:
    wxWin32Renderer *m_renderer;

    // the names of the already created handlers and the handlers themselves
    // (these arrays are synchronized)
    wxSortedArrayString m_handlerNames;
    wxArrayHandlers m_handlers;

    wxWin32ColourScheme *m_scheme;

    WX_DECLARE_THEME();
};

// ============================================================================
// implementation
// ============================================================================

WX_IMPLEMENT_THEME(wxWin32Theme, win32, wxTRANSLATE("Win32 theme"));

// ----------------------------------------------------------------------------
// wxWin32Theme
// ----------------------------------------------------------------------------

wxWin32Theme::wxWin32Theme()
{
    m_renderer = new wxWin32Renderer;
    m_scheme = NULL;
}

wxWin32Theme::~wxWin32Theme()
{
    WX_CLEAR_ARRAY(m_handlers);
}

wxInputHandler *wxWin32Theme::GetInputHandler(const wxString& control)
{
    wxInputHandler *handler;
    int n = m_handlerNames.Index(control);
    if ( n == wxNOT_FOUND )
    {
        // create a new handler
        n = m_handlerNames.Add(control);

        if ( control == wxCONTROL_BUTTON )
            handler = new wxWin32ButtonInputHandler;
        else
        {
            wxASSERT_MSG( control == wxCONTROL_DEFAULT,
                          _T("no input handler defined for this control") );

            handler = new wxWin32InputHandler;
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
// wxWin32Renderer
// ============================================================================

// ----------------------------------------------------------------------------
// construction
// ----------------------------------------------------------------------------

wxWin32Renderer::wxWin32Renderer()
               : m_penBlack(*wxBLACK_PEN),
                 m_penDarkGrey(wxColour(0x7f7f7f), 0, wxSOLID),
                 m_penLightGrey(wxColour(0xc0c0c0), 0, wxSOLID),
                 m_penWhite(*wxWHITE_PEN),
                 m_penHighlight(wxColour(0xe0e0e0), 0, wxSOLID)
{
}

// ----------------------------------------------------------------------------
// border stuff
// ----------------------------------------------------------------------------

/*
   The raised border in Win32 looks like this:

   IIIIIIIIIIIIIIIIIIIIIIB
   I                    GB
   I                    GB  I = white       (HILIGHT)
   I                    GB  H = light grey  (LIGHT)
   I                    GB  G = dark grey   (SHADOI)
   I                    GB  B = black       (DKSHADOI)
   I                    GB  I = hIghlight (COLOR_3DHILIGHT)
   I                    GB
   IGGGGGGGGGGGGGGGGGGGGGB
   BBBBBBBBBBBBBBBBBBBBBBB

   The sunken border looks like this:

   GGGGGGGGGGGGGGGGGGGGGGI
   GBBBBBBBBBBBBBBBBBBBBHI
   GB                   HI
   GB                   HI
   GB                   HI
   GB                   HI
   GB                   HI
   GB                   HI
   GHHHHHHHHHHHHHHHHHHHHHI
   IIIIIIIIIIIIIIIIIIIIIII

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

   The most complicated is the double border:

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

void wxWin32Renderer::DrawRect(wxDC& dc, wxRect *rect, const wxPen& pen)
{
    // draw
    dc.SetPen(pen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(*rect);

    // adjust the rect
    rect->Inflate(-1);
}

void wxWin32Renderer::DrawHalfRect(wxDC& dc, wxRect *rect, const wxPen& pen)
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

void wxWin32Renderer::DrawShadedRect(wxDC& dc, wxRect *rect,
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

void wxWin32Renderer::DrawRaisedBorder(wxDC& dc, wxRect *rect)
{
    DrawShadedRect(dc, rect, m_penHighlight, m_penBlack);
    DrawShadedRect(dc, rect, m_penLightGrey, m_penDarkGrey);
}

void wxWin32Renderer::DrawBorder(wxDC& dc,
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

void wxWin32Renderer::DrawButtonBorder(wxDC& dc,
                                       const wxRect& rectTotal,
                                       int flags,
                                       wxRect *rectIn)
{
    wxRect rect = rectTotal;

    if ( flags & wxRENDER_PRESSED )
    {
        // button pressed: draw a double border around it
        DrawRect(dc, &rect, m_penBlack);
        DrawRect(dc, &rect, m_penDarkGrey);
    }
    else
    {
        // button not pressed

        if ( flags & (wxRENDER_FOCUSED | wxRENDER_DEFAULT) )
        {
            // button either default or focused (or both): add an extra border around it
            DrawRect(dc, &rect, m_penBlack);
        }

        // now draw a normal button
        DrawShadedRect(dc, &rect, m_penHighlight, m_penBlack);
        DrawHalfRect(dc, &rect, m_penDarkGrey);
    }

    if ( rectIn )
    {
        *rectIn = rect;
    }
}

// ----------------------------------------------------------------------------
// frame
// ----------------------------------------------------------------------------

void wxWin32Renderer::DrawFrame(wxDC& dc,
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

void wxWin32Renderer::DrawLabel(wxDC& dc,
                                const wxString& label,
                                const wxRect& rect,
                                int flags,
                                int alignment,
                                int indexAccel)
{
    if ( !(flags & wxRENDER_ENABLED) )
    {
        // make the text grey and draw a shade for it
        dc.SetTextForeground(0xe0e0e0);
        wxRect rectShadow = rect;
        rectShadow.x++;
        rectShadow.y++;
        dc.DrawLabel(label, rectShadow, alignment, indexAccel);
        dc.SetTextForeground(0x7f7f7f);
    }

    wxRect rectText = rect;
    if ( flags & wxRENDER_FOCUSED )
    {
        rectText.Inflate(-2);
    }

    dc.DrawLabel(label, rectText, alignment, indexAccel);

    if ( flags & wxRENDER_FOCUSED )
    {
        // VZ: this doesn't work under Windows, the dotted pen has dots of 3
        //     pixels each while we really need dots here... PS_ALTERNATE might
        //     work, but it is for NT 5 only
#if 0
        DrawRect(dc, &rectText, wxPen(*wxBLACK, 0, wxDOT));
#else
        // draw the pixels manually
        dc.SetPen(wxPen(*wxBLACK, 0, wxSOLID));

        // Windows quirk: appears to draw them like this, from right to left
        // (and I don't have Hebrew windows to see what happens there)
        for ( wxCoord x = rectText.GetRight(); x >= rectText.GetLeft(); x -= 2 )
        {
            dc.DrawPoint(x, rectText.GetTop());
            dc.DrawPoint(x, rectText.GetBottom());
        }

        wxCoord shift = rectText.width % 2 ? 0 : 1;
        for ( wxCoord y = rectText.GetTop() + 2; y <= rectText.GetBottom(); y+= 2 )
        {
            dc.DrawPoint(rectText.GetLeft(), y - shift);
            dc.DrawPoint(rectText.GetRight(), y);
        }

        if ( shift )
        {
            dc.DrawPoint(rectText.GetLeft(), rectText.GetBottom() - 1);
        }
#endif // 0/1
    }
}

// ----------------------------------------------------------------------------
// background
// ----------------------------------------------------------------------------

void wxWin32Renderer::DrawBackground(wxDC& dc,
                                     const wxColour& col,
                                     const wxRect& rect,
                                     int flags)
{
    // just fill it with the current colour
    wxBrush brush(col, wxSOLID);
    dc.SetBrush(brush);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(rect);
}

// ----------------------------------------------------------------------------
// size adjustments
// ----------------------------------------------------------------------------

void wxWin32Renderer::AdjustSize(wxSize *size, const wxWindow *window)
{
    if ( wxDynamicCast(window, wxButton) )
    {
        // TODO
        size->x += 3*window->GetCharWidth();
        size->y = (11*(window->GetCharHeight() + 8))/10;
    }
    else
    {
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
// wxWin32InputHandler
// ----------------------------------------------------------------------------

wxControlActions wxWin32InputHandler::Map(const wxKeyEvent& event, bool pressed)
{
    return wxACTION_NONE;
}

wxControlActions wxWin32InputHandler::Map(const wxMouseEvent& event)
{
    return wxACTION_NONE;
}

// ----------------------------------------------------------------------------
// wxWin32ButtonInputHandler
// ----------------------------------------------------------------------------

wxWin32ButtonInputHandler::wxWin32ButtonInputHandler()
{
    m_winCapture = NULL;
}

wxControlActions wxWin32ButtonInputHandler::Map(const wxKeyEvent& event,
                                                bool pressed)
{
    int keycode = event.GetKeyCode();
    if ( keycode == WXK_SPACE || keycode == WXK_RETURN )
    {
        return wxACTION_BUTTON_TOGGLE;
    }

    return wxWin32InputHandler::Map(event, pressed);
}

wxControlActions wxWin32ButtonInputHandler::Map(const wxMouseEvent& event)
{
    if ( event.IsButton() )
    {
        if ( event.ButtonDown() )
        {
            m_winCapture = wxWindow::FindFocus();
            m_winCapture->CaptureMouse();
        }
        else // up
        {
            m_winCapture->ReleaseMouse();
        }

        return wxACTION_BUTTON_TOGGLE;
    }

    return wxWin32InputHandler::Map(event);
}

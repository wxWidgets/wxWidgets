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
#endif // WX_PRECOMP

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class wxWin32Renderer : public wxRenderer
{
public:
    // implement the base class pure virtuals
    virtual void DrawLabel(wxDC& dc, wxWindow *window);
    virtual void DrawBorder(wxDC& dc, wxWindow *window);

protected:
    // DrawBorder() helpers: all of them shift and clip the DC after drawing
    // the border

    // just draw a rectangle with the given pen
    void DrawRect(wxDC& dc, wxCoord x, wxCoord y, const wxPen& pen);

    // draw the lower left part of rectangle
    void DrawHalfRect(wxDC& dc, wxCoord x, wxCoord y, const wxPen& pen);

    // draw the rectange using the first brush for the left and top sides and
    // the second one for the bottom and right ones
    void DrawShadedRect(wxDC& dc, wxCoord x, wxCoord y,
                        const wxPen& pen1, const wxPen& pen2);

    // draw the normal 3D border
    void DrawRaisedBorder(wxDC& dc, wxCoord x, wxCoord y);
};

class wxWin32Theme : public wxTheme
{
public:
    wxWin32Theme();

    virtual wxRenderer *GetRenderer() { return m_renderer; }
    virtual wxInputHandler *GetInputHandler() { return m_handler; }
    virtual wxColourScheme *GetColourScheme() { return m_scheme; }

private:
    wxWin32Renderer *m_renderer;
    wxWin32InputHandler *m_handler;
    wxWin32ColourScheme *m_scheme;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxWin32Theme
// ----------------------------------------------------------------------------

wxWin32Theme::wxWin32Theme()
{
    m_renderer = new wxWin32Renderer;
}

// ----------------------------------------------------------------------------
// wxWin32Renderer
// ----------------------------------------------------------------------------

// construction
wxWin32Renderer::wxWin32Renderer()
{
    m_penBlack = new wxPen("BLACK");
    m_penDarkGrey = new wxPen("GREY");
    m_penLightGrey = new wxPen("LIGHT GREY");
    m_penWhite = new wxPen("WHITE");
}

/*
   The normal (== raised) border in Win32 looks like this:

   WWWWWWWWWWWWWWWWWWWWWWB
   W                    GB
   W                    GB  W = white       (HILIGHT)
   W                    GB  H = light grey  (LIGHT)
   W                    GB  G = dark grey   (SHADOW)
   W                    GB  B = black       (DKSHADOW)
   W                    GB
   W                    GB
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

void wxWin32Renderer::DrawRect(wxDC& dc, wxCorod x, wxCoord y, const wxPen& pen)
{
    // draw
    dc.SetPen(pen);
    dc.DrawRectangle(0, 0, x, y);

    // adjust
    dc.SetLogicalOrigin(1, 1);
    dc.SetClippingRegion(0, 0, x - 1, y - 1);
}

void wxWin32Renderer::DrawHalfRect(wxDC& dc, wxCorod x, wxCoord y,
                                   const wxPen& pen)
{
    // draw the bottom and right sides
    dc.SetPen(pen);
    dc.DrawLine(1, y, x, y);
    dc.DrawLine(x, 0, x, y - 1);

    // clip the DC
    dc.SetClippingRegion(0, 0, x - 1, y - 1);
}

void wxWin32Renderer::DrawShadedRect(wxDC& dc, wxCorod x, wxCoord y,
                                     const wxPen& pen1, const wxPen& pen2)
{
    // draw the rectangle
    dc.SetPen(pen1);
    dc.DrawLine(0, 0, 0, y - 1);
    dc.DrawLine(1, 0, x - 1, 0);
    dc.SetPen(pen2);
    dc.DrawLine(x, 0, x, y);
    dc.DrawLine(0, y, x - 1, y);

    // shift the DC and clip it
    dc.SetLogicalOrigin(1, 1);
    dc.SetClippingRegion(0, 0, x - 1, y - 1);
}

void wxWin32Renderer::DrawRaisedBorder(wxDC& dc, wxCoord x, wxCoord y)
{
    DrawShadedRect(dc, x--, y--, m_penWhite, m_penBlack);
    DrawHalfRect(dc, x, y, m_penDarkGrey);
}

void wxWin32Renderer::DrawBorder(wxDC& dc, wxWindow *window)
{
    wxCoord x, y;
    window->GetClientSize(&x, &y);
    long style = window->GetWindowStyle();
    if ( style & wxSUNKEN_BORDER )
    {
        DrawShadedRect(dc, x--, y--, m_penDarkGrey, m_penWhite);
        DrawShadedRect(dc, x,   y,   m_penBlack,    m_penLightGrey);
    }
    else if ( style & wxSTATIC_BORDER )
    {
        DrawShadedRect(dc, x, y, m_penDarkGrey, m_penWhite);
    }
    else if ( style & wxRAISED_BORDER )
    {
        DrawRaisedBorder(dc, x, y);
    }
    else if ( style & wxDOUBLE_BORDER )
    {
        DrawShadedRect(dc, x--, y--, m_penLightGrey, m_penBlack);
        DrawShadedRect(dc, x--, y--, m_penWhite,     m_penDarkGrey);
        DrawRect(dc, x, y, m_penLightGrey);
    }
    else if ( style & wxSIMPLE_BORDER )
    {
        DrawRect(dc, x, y, m_penBlack);
    }
}

void wxWin32Renderer::DrawLabel(wxDC& dc, wxWindow *window)
{
}

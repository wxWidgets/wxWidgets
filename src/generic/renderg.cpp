///////////////////////////////////////////////////////////////////////////////
// Name:        generic/renderg.cpp
// Purpose:     generic implementation of wxRendererNative (for any platform)
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.07.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows license
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
    #include "wx/string.h"
#endif //WX_PRECOMP

#include "wx/gdicmn.h"
#include "wx/dc.h"

#include "wx/settings.h"

#include "wx/renderer.h"

// ----------------------------------------------------------------------------
// wxRendererGeneric: our wxRendererNative implementation
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRendererGeneric : public wxRendererNative
{
public:
    wxRendererGeneric();

    virtual void DrawHeaderButton(wxWindow *win,
                                  wxDC& dc,
                                  const wxRect& rect,
                                  int flags = 0);

    virtual void DrawTreeItemButton(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0);

    virtual void DrawSplitterBorder(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect);

    virtual void DrawSplitterSash(wxWindow *win,
                                  wxDC& dc,
                                  const wxSize& size,
                                  wxCoord position);


    virtual wxPoint GetSplitterSashAndBorder(const wxWindow *win);


protected:
    // draw the rectange using the first pen for the left and top sides and
    // the second one for the bottom and right ones
    void DrawShadedRect(wxDC& dc, wxRect *rect,
                        const wxPen& pen1, const wxPen& pen2);

    // the standard pens
    wxPen m_penBlack,
          m_penDarkGrey,
          m_penLightGrey,
          m_penHighlight;
};

// ============================================================================
// wxRendererGeneric implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxRendererGeneric creation
// ----------------------------------------------------------------------------

/* static */
wxRendererNative& wxRendererNative::GetGeneric()
{
    static wxRendererGeneric s_rendererGeneric;

    return s_rendererGeneric;
}

// some platforms have their own renderers
#if !defined(__WXMSW__) && !defined(__WXMAC__) && !defined(__WXGTK__)

/* static */
wxRendererNative& wxRendererNative::Get()
{
    return GetGeneric();
}

#endif // platforms using their own renderers

wxRendererGeneric::wxRendererGeneric()
    : m_penBlack(wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW)),
      m_penDarkGrey(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW)),
      m_penLightGrey(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)),
      m_penHighlight(wxSystemSettings::GetColour(wxSYS_COLOUR_3DHIGHLIGHT))
{
}

// ----------------------------------------------------------------------------
// wxRendererGeneric helpers
// ----------------------------------------------------------------------------

void
wxRendererGeneric::DrawShadedRect(wxDC& dc,
                                  wxRect *rect,
                                  const wxPen& pen1,
                                  const wxPen& pen2)
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
// tree/list ctrl drawing
// ----------------------------------------------------------------------------

void
wxRendererGeneric::DrawHeaderButton(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags)
{
    const int CORNER = 1;

    const wxCoord x = rect.x,
                  y = rect.y,
                  w = rect.width,
                  h = rect.height;

    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    dc.SetPen(m_penBlack);
    dc.DrawLine( x+w-CORNER+1, y, x+w, y+h );  // right (outer)
    dc.DrawRectangle( x, y+h, w+1, 1 );        // bottom (outer)

    dc.SetPen(m_penDarkGrey);
    dc.DrawLine( x+w-CORNER, y, x+w-1, y+h );  // right (inner)
    dc.DrawRectangle( x+1, y+h-1, w-2, 1 );    // bottom (inner)

    dc.SetPen(m_penHighlight);
    dc.DrawRectangle( x, y, w-CORNER+1, 1 );   // top (outer)
    dc.DrawRectangle( x, y, 1, h );            // left (outer)
    dc.DrawLine( x, y+h-1, x+1, y+h-1 );
    dc.DrawLine( x+w-1, y, x+w-1, y+1 );
}

// draw the plus or minus sign
void
wxRendererGeneric::DrawTreeItemButton(wxWindow *win,
                                      wxDC& dc,
                                      const wxRect& rect,
                                      int flags)
{
    // white background
    dc.SetPen(*wxGREY_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(rect.Deflate(1, 2));

    // black lines
    const wxCoord xMiddle = rect.x + rect.width/2;
    const wxCoord yMiddle = rect.y + rect.height/2;

    dc.SetPen(*wxBLACK_PEN);
    dc.DrawLine(xMiddle - 2, yMiddle, xMiddle + 3, yMiddle);
    if ( !(flags & wxCONTROL_EXPANDED) )
    {
        // turn "-" into "+"
        dc.DrawLine(xMiddle, yMiddle - 2, xMiddle, yMiddle + 3);
    }
}

// ----------------------------------------------------------------------------
// sash drawing
// ----------------------------------------------------------------------------

wxPoint
wxRendererGeneric::GetSplitterSashAndBorder(const wxWindow * WXUNUSED(win))
{
    // see below
    return wxPoint(7, 2);
}

void
wxRendererGeneric::DrawSplitterBorder(wxWindow * WXUNUSED(win),
                                      wxDC& dc,
                                      const wxRect& rectOrig)
{
    wxRect rect = rectOrig;
    DrawShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
    DrawShadedRect(dc, &rect, m_penBlack, m_penLightGrey);
}

void
wxRendererGeneric::DrawSplitterSash(wxWindow * WXUNUSED(win),
                                    wxDC& dc,
                                    const wxSize& size,
                                    wxCoord position)
{
    // we draw a Win32-like sash here:
    //
    //   ---- this is position
    //  /
    // v
    // dWGGGDd
    // GWGGGDB
    // GWGGGDB  where G is light grey (face)
    // GWGGGDB        W    white      (light)
    // GWGGGDB        D    dark grey  (shadow)
    // GWGGGDB        B    black      (dark shadow)
    // GWGGGDB
    // GWGGGDB  and lower letters are our border (already drawn)
    // GWGGGDB
    // wWGGGDd

    const wxCoord h = size.y;

    // from left to right
    dc.SetPen(m_penLightGrey);
    dc.DrawLine(position, 1, position, h - 1);

    dc.SetPen(m_penHighlight);
    dc.DrawLine(position + 1, 0, position + 1, h);

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)));
    dc.DrawRectangle(position + 2, 0, 3, h);

    dc.SetPen(m_penDarkGrey);
    dc.DrawLine(position + 5, 0, position + 5, h);

    dc.SetPen(m_penBlack);
    dc.DrawLine(position + 6, 1, position + 6, h - 1);
}


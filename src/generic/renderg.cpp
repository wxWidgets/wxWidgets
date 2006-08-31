///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/renderg.cpp
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

#include "wx/renderer.h"

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/dc.h"
    #include "wx/settings.h"
    #include "wx/gdicmn.h"
    #include "wx/module.h"
#endif //WX_PRECOMP

#include "wx/splitter.h"
#include "wx/dcmirror.h"

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
                                    const wxRect& rect,
                                    int flags = 0);

    virtual void DrawSplitterSash(wxWindow *win,
                                  wxDC& dc,
                                  const wxSize& size,
                                  wxCoord position,
                                  wxOrientation orient,
                                  int flags = 0);

    virtual void DrawComboBoxDropButton(wxWindow *win,
                                        wxDC& dc,
                                        const wxRect& rect,
                                        int flags = 0);

    virtual void DrawDropArrow(wxWindow *win,
                               wxDC& dc,
                               const wxRect& rect,
                               int flags = 0);

    virtual void DrawCheckBox(wxWindow *win,
                              wxDC& dc,
                              const wxRect& rect,
                              int flags = 0);

    virtual void DrawPushButton(wxWindow *win,
                                wxDC& dc,
                                const wxRect& rect,
                                int flags = 0);

    virtual void DrawItemSelectionRect(wxWindow *win,
                                       wxDC& dc,
                                       const wxRect& rect,
                                       int flags = 0);

    virtual wxSplitterRenderParams GetSplitterParams(const wxWindow *win);

    virtual wxRendererVersion GetVersion() const
    {
        return wxRendererVersion(wxRendererVersion::Current_Version,
                                 wxRendererVersion::Current_Age);
    }


    // Cleanup by deleting standard renderer
    static void Cleanup();

    // Get the generic object
    static wxRendererGeneric* DoGetGeneric();

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

    static wxRendererGeneric* sm_rendererGeneric;
};

// ============================================================================
// wxRendererGeneric implementation
// ============================================================================

// Get the generic object
wxRendererGeneric* wxRendererGeneric::DoGetGeneric()
{
    if (!sm_rendererGeneric)
        sm_rendererGeneric = new wxRendererGeneric;
    return sm_rendererGeneric;
}

// ----------------------------------------------------------------------------
// wxRendererGeneric creation
// ----------------------------------------------------------------------------

/* static */
wxRendererNative& wxRendererNative::GetGeneric()
{
    return * wxRendererGeneric::DoGetGeneric();
}

void wxRendererGeneric::Cleanup()
{
    if (sm_rendererGeneric)
        delete sm_rendererGeneric;

    sm_rendererGeneric = NULL;
}

wxRendererGeneric* wxRendererGeneric::sm_rendererGeneric = NULL;

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
wxRendererGeneric::DrawHeaderButton(wxWindow * WXUNUSED(win),
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int WXUNUSED(flags))
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
wxRendererGeneric::DrawTreeItemButton(wxWindow * WXUNUSED(win),
                                      wxDC& dc,
                                      const wxRect& rect,
                                      int flags)
{
    // store settings
    wxDCPenChanger penChanger(dc, *wxGREY_PEN);
    wxDCBrushChanger brushChanger(dc, *wxWHITE_BRUSH);

    dc.DrawRectangle(rect);

    // black lines
    const wxCoord xMiddle = rect.x + rect.width/2;
    const wxCoord yMiddle = rect.y + rect.height/2;

    // half of the length of the horz lines in "-" and "+"
    const wxCoord halfWidth = rect.width/2 - 2;
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawLine(xMiddle - halfWidth, yMiddle,
                xMiddle + halfWidth + 1, yMiddle);

    if ( !(flags & wxCONTROL_EXPANDED) )
    {
        // turn "-" into "+"
        const wxCoord halfHeight = rect.height/2 - 2;
        dc.DrawLine(xMiddle, yMiddle - halfHeight,
                    xMiddle, yMiddle + halfHeight + 1);
    }
}

// ----------------------------------------------------------------------------
// sash drawing
// ----------------------------------------------------------------------------

wxSplitterRenderParams
wxRendererGeneric::GetSplitterParams(const wxWindow *win)
{
    // see below
    wxCoord sashWidth,
            border;

    if ( win->HasFlag(wxSP_3DSASH) )
        sashWidth = 7;
    else if ( win->HasFlag(wxSP_NOSASH) )
        sashWidth = 0;
    else // no 3D effect
        sashWidth = 3;

    if ( win->HasFlag(wxSP_3DBORDER) )
        border = 2;
    else // no 3D effect
        border = 0;

    return wxSplitterRenderParams(sashWidth, border, false);
}

void
wxRendererGeneric::DrawSplitterBorder(wxWindow *win,
                                      wxDC& dc,
                                      const wxRect& rectOrig,
                                      int WXUNUSED(falgs))
{
    if ( win->HasFlag(wxSP_3DBORDER) )
    {
        wxRect rect = rectOrig;
        DrawShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
        DrawShadedRect(dc, &rect, m_penBlack, m_penLightGrey);
    }
}

void
wxRendererGeneric::DrawSplitterSash(wxWindow *win,
                                    wxDC& dcReal,
                                    const wxSize& sizeReal,
                                    wxCoord position,
                                    wxOrientation orient,
                                    int WXUNUSED(flags))
{
    // to avoid duplicating the same code for horizontal and vertical sashes,
    // simply mirror the DC instead if needed (i.e. if horz splitter)
    wxMirrorDC dc(dcReal, orient != wxVERTICAL);
    wxSize size = dc.Reflect(sizeReal);


    // we draw a Win32-like grey sash with possible 3D border here:
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
    //
    // only the middle 3 columns are drawn unless wxSP_3D is specified

    const wxCoord h = size.y;
    wxCoord offset = 0;

    // If we're drawing the border, draw the sash 3d lines shorter
    if ( win->HasFlag(wxSP_3DBORDER) )
    {
        offset = 1;
    }

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)));

    if ( win->HasFlag(wxSP_3DSASH) )
    {
        // Draw the 3D sash
        dc.DrawRectangle(position + 2, 0, 3, h);

        dc.SetPen(m_penLightGrey);
        dc.DrawLine(position, offset, position, h - offset);

        dc.SetPen(m_penHighlight);
        dc.DrawLine(position + 1, 0, position + 1, h);

        dc.SetPen(m_penDarkGrey);
        dc.DrawLine(position + 5, 0, position + 5, h);

        dc.SetPen(m_penBlack);
        dc.DrawLine(position + 6, offset, position + 6, h - offset);
    }
    else
    {
        // Draw a flat sash
        dc.DrawRectangle(position, 0, 3, h);
    }
}

// ----------------------------------------------------------------------------
// button drawing
// ----------------------------------------------------------------------------

void
wxRendererGeneric::DrawComboBoxDropButton(wxWindow *win,
                                          wxDC& dc,
                                          const wxRect& rect,
                                          int flags)
{
    DrawPushButton(win,dc,rect,flags);
    DrawDropArrow(win,dc,rect,flags);
}

void
wxRendererGeneric::DrawDropArrow(wxWindow *win,
                                 wxDC& dc,
                                 const wxRect& rect,
                                 int WXUNUSED(flags))
{
    // This generic implementation should be good
    // enough for Windows platforms (including XP).

    int arrowHalf = rect.width/5;
    int rectMid = rect.width / 2;
    int arrowTopY = (rect.height/2) - (arrowHalf/2);

    // This should always result in arrow with odd width.
    wxPoint pt[] =
    {
        wxPoint(rectMid - arrowHalf, arrowTopY),
        wxPoint(rectMid + arrowHalf, arrowTopY),
        wxPoint(rectMid, arrowTopY + arrowHalf)
    };
    dc.SetBrush(wxBrush(win->GetForegroundColour()));
    dc.SetPen(wxPen(win->GetForegroundColour()));
    dc.DrawPolygon(WXSIZEOF(pt), pt, rect.x, rect.y);
}

void
wxRendererGeneric::DrawCheckBox(wxWindow *WXUNUSED(win),
                                wxDC& dc,
                                const wxRect& rect,
                                int flags)
{
    dc.SetPen(*(flags & wxCONTROL_DISABLED ? wxGREY_PEN : wxBLACK_PEN));
    dc.SetBrush( *wxTRANSPARENT_BRUSH );
    dc.DrawRectangle(rect);

    if ( flags & wxCONTROL_CHECKED )
    {
        dc.DrawCheckMark(rect.Deflate(2, 2));
    }
}

void
wxRendererGeneric::DrawPushButton(wxWindow *win,
                                  wxDC& dc,
                                  const wxRect& rect,
                                  int flags)
{
    // Don't try anything too fancy. It'll just turn out looking
    // out-of-place on most platforms.
    wxColour bgCol = flags & wxCONTROL_DISABLED ?
                        wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE) :
                        win->GetBackgroundColour();
    dc.SetBrush(wxBrush(bgCol));
    dc.SetPen(wxPen(bgCol));
    dc.DrawRectangle(rect);
}

void
wxRendererGeneric::DrawItemSelectionRect(wxWindow * WXUNUSED(win),
                                         wxDC& dc,
                                         const wxRect& rect,
                                         int flags)
{
    wxBrush brush;
    if ( flags & wxCONTROL_SELECTED )
    {
        if ( flags & wxCONTROL_FOCUSED )
        {
            brush = wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
        }
        else // !focused
        {
            brush = wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));
        }
    }
    else // !selected
    {
        brush = *wxTRANSPARENT_BRUSH;
    }

    dc.SetBrush(brush);
    dc.SetPen(flags & wxCONTROL_CURRENT ? *wxBLACK_PEN : *wxTRANSPARENT_PEN);

    dc.DrawRectangle( rect );
}


// ----------------------------------------------------------------------------
// A module to allow cleanup of generic renderer.
// ----------------------------------------------------------------------------

class wxGenericRendererModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxGenericRendererModule)
public:
    wxGenericRendererModule() {}
    bool OnInit() { return true; };
    void OnExit() { wxRendererGeneric::Cleanup(); };
};

IMPLEMENT_DYNAMIC_CLASS(wxGenericRendererModule, wxModule)

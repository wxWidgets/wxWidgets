///////////////////////////////////////////////////////////////////////////////
// Name:        generic/renderg.cpp
// Purpose:     generic implementation of wxRendererBase (for any platform)
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

#include "wx/renderer.h"

// ----------------------------------------------------------------------------
// wxRendererGeneric: our wxRendererBase implementation
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRendererGeneric : public wxRendererBase
{
public:
    // draw the header control button (used by wxListCtrl)
    virtual void DrawHeaderButton(wxWindow *win,
                                  wxDC& dc,
                                  const wxRect& rect,
                                  int flags = 0);

    // draw the expanded/collapsed icon for a tree control item
    virtual void DrawTreeItemButton(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0);
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxRendererGeneric creation
// ----------------------------------------------------------------------------

/* static */
wxRendererNative *wxRendererGeneric::GetGeneric()
{
    static wxRendererGeneric s_rendererGeneric;

    return s_rendererGeneric;
}

// some platforms have their own renderers
#if !defined(__WXMSW__) && !defined(__WXMAC__) && !defined(__WXGTK__)

/* static */
wxRendererNative& wxRendererGeneric::Get()
{
    return GetGeneric();
}

#endif // platforms using their own renderers

// ----------------------------------------------------------------------------
// wxRendererGeneric drawing functions
// ----------------------------------------------------------------------------

void
wxRendererGeneric::DrawHeaderButton(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags)
{
    const int m_corner = 1;

    dc->SetBrush( *wxTRANSPARENT_BRUSH );

    dc->SetPen( *wxBLACK_PEN );
    dc->DrawLine( x+w-m_corner+1, y, x+w, y+h );  // right (outer)
    dc->DrawRectangle( x, y+h, w+1, 1 );          // bottom (outer)

    wxPen pen( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNSHADOW ), 1, wxSOLID );

    dc->SetPen( pen );
    dc->DrawLine( x+w-m_corner, y, x+w-1, y+h );  // right (inner)
    dc->DrawRectangle( x+1, y+h-1, w-2, 1 );      // bottom (inner)

    dc->SetPen( *wxWHITE_PEN );
    dc->DrawRectangle( x, y, w-m_corner+1, 1 );   // top (outer)
    dc->DrawRectangle( x, y, 1, h );              // left (outer)
    dc->DrawLine( x, y+h-1, x+1, y+h-1 );
    dc->DrawLine( x+w-1, y, x+w-1, y+1 );
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
    if ( !item->IsExpanded() )
    {
        // turn "-" into "+"
        dc.DrawLine(xMiddle, yMiddle - 2, xMiddle, yMiddle + 3);
    }
}



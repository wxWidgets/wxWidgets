///////////////////////////////////////////////////////////////////////////////
// Name:        mac/renderer.cpp
// Purpose:     implementation of wxRendererBase for Mac
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
// wxRendererMac: our wxRendererBase implementation
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRendererMac : public wxRendererBase
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

private:
    // the tree buttons
    wxBitmap m_bmpTreeExpanded,
             m_bmpTreeCollapsed;
};

// ----------------------------------------------------------------------------
// Aqua arrows
// ----------------------------------------------------------------------------

/* XPM */
static const char *aqua_arrow_right_xpm[] = {
/* columns rows colors chars-per-pixel */
"13 11 4 1",
"  c None",
"b c #C0C0C0",
"c c #707070",
"d c #A0A0A0",
/* pixels */
"    b        ",
"    ddb      ",
"    cccdb    ",
"    cccccd   ",
"    ccccccdb ",
"    ccccccccd",
"    ccccccdb ",
"    cccccb   ",
"    cccdb    ",
"    ddb      ",
"    b        "
};

/* XPM */
static const char *aqua_arrow_down_xpm[] = {
/* columns rows colors chars-per-pixel */
"13 11 4 1",
"  c None",
"b c #C0C0C0",
"c c #707070",
"d c #A0A0A0",
/* pixels */
"             ",
"             ",
" bdcccccccdb ",
"  dcccccccd  ",
"  bcccccccb  ",
"   dcccccd   ",
"   bcccccb   ",
"    bcccd    ",
"     dcd     ",
"     bcb     ",
"      d      "
};

// ============================================================================
// implementation
// ============================================================================

/* static */
wxRendererNative& wxRendererMac::Get()
{
    static wxRendererMac s_rendererMac;

    return s_rendererMac;
}

void
wxRendererMac::DrawHeaderButton(wxWindow *win,
                                wxDC& dc,
                                const wxRect& rect,
                                int WXUNUSED(flags))
{
    const int CORNER = 1;

    const wxCoord x = rect.x,
                  y = rect.y,
                  w = rect.width,
                  h = rect.height;

    dc.SetBrush( *wxTRANSPARENT_BRUSH );

    dc.SetPen( wxPen( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNSHADOW ) , 1 , wxSOLID ) );
    dc.DrawLine( x+w-CORNER+1, y, x+w, y+h );       // right (outer)
    dc.DrawRectangle( x, y+h, w+1, 1 );             // bottom (outer)

    wxPen pen( wxColour( 0x88 , 0x88 , 0x88 ), 1, wxSOLID );

    dc.SetPen( pen );
    dc.DrawLine( x+w-CORNER, y, x+w-1, y+h );       // right (inner)
    dc.DrawRectangle( x+1, y+h-1, w-2, 1 );         // bottom (inner)

    dc.SetPen( *wxWHITE_PEN );
    dc.DrawRectangle( x, y, w-CORNER+1, 1 );        // top (outer)
    dc.DrawRectangle( x, y, 1, h );                 // left (outer)
    dc.DrawLine( x, y+h-1, x+1, y+h-1 );
    dc.DrawLine( x+w-1, y, x+w-1, y+1 );
}

void
wxRendererMac::DrawTreeItemButton(wxWindow *win,
                                  wxDC& dc,
                                  const wxRect& rect,
                                  int flags)
{
    // init the buttons on demand
    if ( !m_bmpTreeExpanded.Ok() )
    {
        m_bmpTreeExpanded = wxBitmap(aqua_arrow_down_xpm);
        m_bmpTreeCollapsed = wxBitmap(aqua_arrow_right_xpm);
    }

    // draw them

    // VZ: this is the old code from treectlg.cpp which apparently doesn't work
    //     but I kept it here just in case it is needed -- if not, please
    //     remove it
#if 0 // def __WXMAC__
    wxMacPortSetter helper(&dc) ;
    wxMacWindowClipper clipper(this) ;
    wxDC::MacSetupBackgroundForCurrentPort( MacGetBackgroundBrush() ) ;

    int loc_x = x - 5 ;
    int loc_y = y_mid - 6 ;
    MacWindowToRootWindow( & loc_x , & loc_y ) ;
    Rect bounds = { loc_y , loc_x , loc_y + 18 , loc_x + 12 } ;
    ThemeButtonDrawInfo info = { kThemeStateActive , item->IsExpanded() ? kThemeDisclosureDown : kThemeDisclosureRight ,
        kThemeAdornmentNone }; 
    DrawThemeButton( &bounds, kThemeDisclosureButton , 
        &info , NULL , NULL , NULL , NULL ) ;
#else // 1
    dc.DrawBitmap(flags & wxCONTROL_EXPANDED ? m_bmpTreeExpanded
                                             : m_bmpTreeCollapsed,
                  rect.x, rect.y, true /* use mask */);
#endif // 0/1
}


///////////////////////////////////////////////////////////////////////////////
// Name:        mac/renderer.cpp
// Purpose:     implementation of wxRendererNative for Mac
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.07.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows licence
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
    #include "wx/dc.h"
    #include "wx/bitmap.h"
    #include "wx/settings.h"
#endif //WX_PRECOMP

#include "wx/renderer.h"
#include "wx/toplevel.h"
#include "wx/dcclient.h"
#include "wx/mac/uma.h"

// ----------------------------------------------------------------------------
// wxRendererMac: our wxRendererNative implementation
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRendererMac : public wxDelegateRendererNative
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

    // draw a (vertical) sash
    virtual void DrawSplitterSash(wxWindow *win,
                                  wxDC& dc,
                                  const wxSize& size,
                                  wxCoord position,
                                  wxOrientation orient,
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
wxRendererNative& wxRendererNative::GetDefault()
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

    const wxCoord x = rect.x-1,
                  y = rect.y-1,
                  w = rect.width,
                  h = rect.height;

    int major,minor;
    wxGetOsVersion( &major, &minor );

    dc.SetBrush( *wxTRANSPARENT_BRUSH );

    if (major >= 10) 
    {
        dc.SetPen( wxPen( wxColour( 0xC5 , 0xC5 , 0xC5 ) , 1 , wxSOLID ) );
        dc.DrawRectangle( x, y+CORNER, 1, h-CORNER );  				// left
        // The right border is overdrawn by the left border of the right neighbouring
        // header (to maintain a proper single pixel border). Except for the 
        // rightmost header of the listctrl.
        dc.DrawRectangle( x+w+(CORNER*2), y+CORNER, 1, h-CORNER ); 	// right
        dc.SetPen( wxPen( wxColour( 0xB1 , 0xB1 , 0xB1 ) , 1 , wxSOLID ) );    
        dc.DrawRectangle( x, y+h, w+(CORNER*3), 1 );          		// bottom
        dc.DrawRectangle( x, y, w+(CORNER*3), 1 );  			    // top
        
        // Do a fill of the interior for background:
        dc.SetPen( wxPen( wxColour( 0xF6 , 0xF6 , 0xF6 ) , 1 , wxSOLID ) ); 
        dc.DrawRectangle( x+CORNER, y+CORNER, w+CORNER, h-CORNER );
    
        // Do the gradient fill:
        static int grayValues[] = 
        {
            0xF6, 0xF2, 0xEF, 0xED, 0xED, 0xEB, 0xEA, 0xEA, 0xE8, 
            0xE8, 0xE2, 0xE5, 0xE8, 0xEB, 0xEF, 0xF2, 0xFD
        };
        int i;
        for (i=0; i < h && i < (int)WXSIZEOF(grayValues); i++) 
        {
            dc.SetPen( wxPen( wxColour( grayValues[i] , grayValues[i] , grayValues[i] ),
                            1 , wxSOLID ) );
            dc.DrawRectangle( x+CORNER, y+CORNER+i, w+CORNER, 1 );
        }
    }
    else
    {
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

void
wxRendererMac::DrawSplitterSash(wxWindow *win,
                                wxDC& dc,
                                const wxSize& size,
                                wxCoord position,
                                wxOrientation orient,
                                int WXUNUSED(flags))
{
#if ( TARGET_API_MAC_OSX == 1 ) && ( MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3 )
    if ( UMAGetSystemVersion() >= 0x1030 )
    {
        bool hasMetal = win->MacGetTopLevelWindow()->MacGetMetalAppearance() ;
        SInt32 height ;
        GetThemeMetric( kThemeMetricSmallPaneSplitterHeight , &height ) ;
        HIRect splitterRect ;
        if ( orient == wxVERTICAL )
            splitterRect = CGRectMake( position , 0 , height, size.y);
        else
            splitterRect = CGRectMake( 0 , position , size.x , height );
        HIViewConvertRect( &splitterRect , (HIViewRef) win->GetHandle() , (HIViewRef) win->MacGetTopLevelWindow()->GetHandle() ) ;

        // under compositing we should only draw when called by the OS, otherwise just issue a redraw command
        // strange redraw errors occur if we don't do this

        if ( dc.IsKindOf( CLASSINFO( wxPaintDC ) ) == false )
        {
            Rect r = { (short) splitterRect.origin.y , (short) splitterRect.origin.x , 
                (short) (splitterRect.origin.y + splitterRect.size.height) , (short) (splitterRect.origin.x + splitterRect.size.width) } ;
            RgnHandle updateRgn = NewRgn() ;
            RectRgn( updateRgn , &r ) ;
            HIViewSetNeedsDisplayInRegion( (HIViewRef) win->GetHandle() , updateRgn , true ) ;
            DisposeRgn( updateRgn ) ;
        }
        else
        {
#if wxMAC_USE_CORE_GRAPHICS
#else
            CGContextRef cgContext ;
            Rect bounds ;
            GetPortBounds( (CGrafPtr) dc.m_macPort , &bounds ) ;
            QDBeginCGContext( (CGrafPtr) dc.m_macPort , &cgContext ) ;
            CGContextTranslateCTM( cgContext , 0 , bounds.bottom - bounds.top ) ;
            CGContextScaleCTM( cgContext , 1 , -1 ) ;

            {
                HIThemeSplitterDrawInfo drawInfo ;
                drawInfo.version = 0 ;
                drawInfo.state = kThemeStateActive ;
                drawInfo.adornment = hasMetal ? kHIThemeSplitterAdornmentMetal : kHIThemeSplitterAdornmentNone ;
                HIThemeDrawPaneSplitter( &splitterRect , &drawInfo , cgContext , kHIThemeOrientationNormal ) ;    
            }
            QDEndCGContext( (CGrafPtr) dc.m_macPort , &cgContext ) ;
#endif
        }
    }
    else
#endif
    {
#if 0
        dc.SetPen(*wxLIGHT_GREY_PEN);
        dc.SetBrush(*wxWHITE_BRUSH);
        if ( orient == wxVERTICAL )
            dc.DrawRectangle(position, 0, 7, size.y);
        else
            dc.DrawRectangle(0, position, size.x, 7);
#else
        // Do the gradient fill:
        static int grayValues[] = 
        {
            0xA0, 0xF6, 0xED, 0xE4, 0xE2, 0xD0, 0xA0
        };
        dc.SetBrush( *wxTRANSPARENT_BRUSH );
        if ( orient == wxVERTICAL )
        {
            int i;
            for (i=0; i < (int)WXSIZEOF(grayValues); i++) 
            {
                dc.SetPen( wxPen( wxColour( grayValues[i] , grayValues[i] , grayValues[i] ),
                                1 , wxSOLID ) );
                dc.DrawRectangle( position+i, 0, 1, size.y );
            }
        }
        else
        {
            int i;
            for (i=0; i < (int)WXSIZEOF(grayValues); i++) 
            {
                dc.SetPen( wxPen( wxColour( grayValues[i] , grayValues[i] , grayValues[i] ),
                                1 , wxSOLID ) );
                dc.DrawRectangle( 0, position+i, size.x, 1 );
            }
        }
#endif
    }
}


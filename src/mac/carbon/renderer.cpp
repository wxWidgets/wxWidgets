///////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/renderer.cpp
// Purpose:     implementation of wxRendererNative for Mac
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.07.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

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
    #include "wx/dcclient.h"
    #include "wx/toplevel.h"
#endif

#include "wx/renderer.h"
#include "wx/mac/uma.h"


class WXDLLEXPORT wxRendererMac : public wxDelegateRendererNative
{
public:
    // draw the header control button (used by wxListCtrl)
    virtual void DrawHeaderButton( wxWindow *win,
        wxDC& dc,
        const wxRect& rect,
        int flags = 0 );

    // draw the expanded/collapsed icon for a tree control item
    virtual void DrawTreeItemButton( wxWindow *win,
        wxDC& dc,
        const wxRect& rect,
        int flags = 0 );

    // draw a (vertical) sash
    virtual void DrawSplitterSash( wxWindow *win,
        wxDC& dc,
        const wxSize& size,
        wxCoord position,
        wxOrientation orient,
        int flags = 0 );

private:
    // the tree buttons
    wxBitmap m_bmpTreeExpanded;
    wxBitmap m_bmpTreeCollapsed;
};

// ----------------------------------------------------------------------------
// Aqua arrows
// ----------------------------------------------------------------------------

/* XPM */
static const char *aqua_arrow_right_xpm[] =
{
// columns rows colors chars-per-pixel
"13 11 4 1",
"  c None",
"b c #C0C0C0",
"c c #707070",
"d c #A0A0A0",

// pixels
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
static const char *aqua_arrow_down_xpm[] =
{
// columns rows colors chars-per-pixel
"13 11 4 1",
"  c None",
"b c #C0C0C0",
"c c #707070",
"d c #A0A0A0",

// pixels
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

// static
wxRendererNative& wxRendererNative::GetDefault()
{
    static wxRendererMac s_rendererMac;

    return s_rendererMac;
}

void wxRendererMac::DrawHeaderButton( wxWindow *win,
    wxDC& dc,
    const wxRect& rect,
    int flags )
{
    int major, minor;

    wxGetOsVersion( &major, &minor );

    const wxCoord x = dc.XLOG2DEV(rect.x - 1);
    const wxCoord y = dc.YLOG2DEV(rect.y - 1);
    const wxCoord w = dc.XLOG2DEVREL(rect.width);
    const wxCoord h = dc.YLOG2DEVREL(rect.height);

    dc.SetBrush( *wxTRANSPARENT_BRUSH );

    HIRect headerRect = CGRectMake( x, y, w, h );
    if ( !dc.IsKindOf( CLASSINFO( wxPaintDC ) ) )
    {
        Rect r =
        {
            (short) headerRect.origin.y, (short) headerRect.origin.x,
            (short) (headerRect.origin.y + headerRect.size.height),
            (short) (headerRect.origin.x + headerRect.size.width)
        };

        RgnHandle updateRgn = NewRgn();
        RectRgn( updateRgn, &r );
        HIViewSetNeedsDisplayInRegion( (HIViewRef) win->GetHandle(), updateRgn, true );
        DisposeRgn( updateRgn );
    }
    else
    {
        CGContextRef cgContext;

#if wxMAC_USE_CORE_GRAPHICS
        cgContext = ((wxMacCGContext*)(dc.GetGraphicContext()))->GetNativeContext();
#else
        Rect bounds;

        GetPortBounds( (CGrafPtr) dc.m_macPort, &bounds );
        QDBeginCGContext( (CGrafPtr) dc.m_macPort, &cgContext );

        CGContextTranslateCTM( cgContext, 0, bounds.bottom - bounds.top );
        CGContextScaleCTM( cgContext, 1, -1 );

        HIShapeReplacePathInCGContext( HIShapeCreateWithQDRgn( (RgnHandle) dc.m_macCurrentClipRgn ), cgContext );
        CGContextClip( cgContext );
        HIViewConvertRect( &headerRect, (HIViewRef) win->GetHandle(), (HIViewRef) win->MacGetTopLevelWindow()->GetHandle() );
#endif

        {
            HIThemeButtonDrawInfo drawInfo;
            HIRect labelRect;

            memset( &drawInfo, 0, sizeof(drawInfo) );
            drawInfo.version = 0;
            drawInfo.state = (flags & wxCONTROL_DISABLED) ? kThemeStateInactive : kThemeStateActive;
            drawInfo.kind = kThemeListHeaderButton;
            drawInfo.value = 0;
            drawInfo.adornment = kThemeAdornmentNone;
            HIThemeDrawButton( &headerRect, &drawInfo, cgContext, kHIThemeOrientationNormal, &labelRect );
        }

#if wxMAC_USE_CORE_GRAPHICS
#else
        QDEndCGContext( (CGrafPtr) dc.m_macPort, &cgContext );
#endif
    }
}

void wxRendererMac::DrawTreeItemButton( wxWindow *win,
    wxDC& dc,
    const wxRect& rect,
    int flags )
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
    wxMacPortSetter helper(&dc);
    wxMacWindowClipper clipper(this);
    wxDC::MacSetupBackgroundForCurrentPort( MacGetBackgroundBrush() );

    int loc_x = x - 5;
    int loc_y = y_mid - 6;
    MacWindowToRootWindow( &loc_x, &loc_y );
    Rect bounds = { loc_y, loc_x, loc_y + 18, loc_x + 12 };
    ThemeButtonDrawInfo info =
    {
        kThemeStateActive,
        item->IsExpanded() ? kThemeDisclosureDown : kThemeDisclosureRight,
        kThemeAdornmentNone
    };

    DrawThemeButton( &bounds, kThemeDisclosureButton, &info, NULL, NULL, NULL, NULL );
#else // 1
    dc.DrawBitmap(
        flags & wxCONTROL_EXPANDED
        ? m_bmpTreeExpanded
        : m_bmpTreeCollapsed,
         rect.x, rect.y, true /* use mask */);
#endif
}

void wxRendererMac::DrawSplitterSash( wxWindow *win,
    wxDC& dc,
    const wxSize& size,
    wxCoord position,
    wxOrientation orient,
    int WXUNUSED(flags) )
{
    bool hasMetal = win->MacGetTopLevelWindow()->MacGetMetalAppearance();
    SInt32 height;
    GetThemeMetric( kThemeMetricSmallPaneSplitterHeight, &height );
    HIRect splitterRect;
    if (orient == wxVERTICAL)
        splitterRect = CGRectMake( position, 0, height, size.y );
    else
        splitterRect = CGRectMake( 0, position, size.x, height );

#if !wxMAC_USE_CORE_GRAPHICS
    HIViewConvertRect(
        &splitterRect,
        (HIViewRef) win->GetHandle(),
        (HIViewRef) win->MacGetTopLevelWindow()->GetHandle() );
#endif

    // under compositing we should only draw when called by the OS, otherwise just issue a redraw command
    // strange redraw errors occur if we don't do this

    if ( !dc.IsKindOf( CLASSINFO( wxPaintDC ) ) )
    {
        Rect r =
        {
            (short) splitterRect.origin.y,
            (short) splitterRect.origin.x,
            (short) (splitterRect.origin.y + splitterRect.size.height),
            (short) (splitterRect.origin.x + splitterRect.size.width)
        };

        RgnHandle updateRgn = NewRgn();
        RectRgn( updateRgn, &r );
        HIViewSetNeedsDisplayInRegion( (HIViewRef) win->GetHandle(), updateRgn, true );
        DisposeRgn( updateRgn );
    }
    else
    {
        CGContextRef cgContext;

#if wxMAC_USE_CORE_GRAPHICS
        cgContext = ((wxMacCGContext*)(dc.GetGraphicContext()))->GetNativeContext();
#else
        Rect bounds;
        GetPortBounds( (CGrafPtr) dc.m_macPort, &bounds );
        QDBeginCGContext( (CGrafPtr) dc.m_macPort, &cgContext );
        CGContextTranslateCTM( cgContext, 0, bounds.bottom - bounds.top );
        CGContextScaleCTM( cgContext, 1, -1 );
#endif

        HIThemeSplitterDrawInfo drawInfo;
        drawInfo.version = 0;
        drawInfo.state = kThemeStateActive;
        drawInfo.adornment = hasMetal ? kHIThemeSplitterAdornmentMetal : kHIThemeSplitterAdornmentNone;
        HIThemeDrawPaneSplitter( &splitterRect, &drawInfo, cgContext, kHIThemeOrientationNormal );

#if wxMAC_USE_CORE_GRAPHICS
#else
        QDEndCGContext( (CGrafPtr) dc.m_macPort, &cgContext );
#endif
    }
}

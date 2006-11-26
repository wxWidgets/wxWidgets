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
#include "wx/graphics.h"
#include "wx/mac/uma.h"


class WXDLLEXPORT wxRendererMac : public wxDelegateRendererNative
{
public:
    // draw the header control button (used by wxListCtrl)
    virtual void DrawHeaderButton( wxWindow *win,
        wxDC& dc,
        const wxRect& rect,
        int flags = 0,
        wxHeaderSortIconType sortArrow = wxHDR_SORT_ICON_NONE,
        wxHeaderButtonParams* params = NULL );

    virtual int GetHeaderButtonHeight(wxWindow *win);
    
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

    virtual void DrawComboBoxDropButton(wxWindow *win,
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

private:
    void DrawMacThemeButton(wxWindow *win,
                            wxDC& dc,
                            const wxRect& rect,
                            int flags,
                            int kind,
                            int adornment);
        
    // the tree buttons
    wxBitmap m_bmpTreeExpanded;
    wxBitmap m_bmpTreeCollapsed;
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
    int flags,
    wxHeaderSortIconType sortArrow,
    wxHeaderButtonParams* params )
{
#if !wxMAC_USE_CORE_GRAPHICS
    const wxCoord x = dc.LogicalToDeviceX(rect.x);
    const wxCoord y = dc.LogicalToDeviceY(rect.y);
    const wxCoord w = dc.LogicalToDeviceXRel(rect.width);
    const wxCoord h = dc.LogicalToDeviceYRel(rect.height);
#else
    // now the wxGCDC is using native transformations
    const wxCoord x = rect.x;
    const wxCoord y = rect.y;
    const wxCoord w = rect.width;
    const wxCoord h = rect.height;
#endif

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
        cgContext = (CGContextRef) dc.GetGraphicsContext()->GetNativeContext();
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
            drawInfo.kind = kThemeListHeaderButton;
            drawInfo.state = (flags & wxCONTROL_DISABLED) ? kThemeStateInactive : kThemeStateActive;
            drawInfo.value = (flags & wxCONTROL_SELECTED) ? kThemeButtonOn : kThemeButtonOff;
            drawInfo.adornment = kThemeAdornmentNone;

            // The down arrow is drawn automatically, change it to an up arrow if needed.
            if ( sortArrow == wxHDR_SORT_ICON_UP )
                drawInfo.adornment = kThemeAdornmentHeaderButtonSortUp;
                
            HIThemeDrawButton( &headerRect, &drawInfo, cgContext, kHIThemeOrientationNormal, &labelRect );

            // If we don't want any arrows we need to draw over the one already there
            if ( (flags & wxCONTROL_SELECTED) && (sortArrow == wxHDR_SORT_ICON_NONE) )
            {
                // clip to the header rectangle
                CGContextSaveGState( cgContext );
                CGContextClipToRect( cgContext, headerRect );
                // but draw bigger than that so the arrow will get clipped off
                headerRect.size.width += 25;
                HIThemeDrawButton( &headerRect, &drawInfo, cgContext, kHIThemeOrientationNormal, &labelRect );
                CGContextRestoreGState( cgContext );
            }                
        }

#if wxMAC_USE_CORE_GRAPHICS
#else
        QDEndCGContext( (CGrafPtr) dc.m_macPort, &cgContext );
#endif
    }

    // Reserve room for the arrows before writing the label, and turn off the
    // flags we've already handled
    wxRect newRect(rect);
    if ( (flags & wxCONTROL_SELECTED) && (sortArrow != wxHDR_SORT_ICON_NONE) )
    {
        newRect.width -= 12;
        sortArrow = wxHDR_SORT_ICON_NONE;
    }
    flags &= ~wxCONTROL_SELECTED;

    DrawHeaderButtonContents(win, dc, newRect, flags, sortArrow, params);
}


int wxRendererMac::GetHeaderButtonHeight(wxWindow* WXUNUSED(win))
{
    SInt32		standardHeight;
    OSStatus		errStatus;

    errStatus = GetThemeMetric( kThemeMetricListHeaderHeight, &standardHeight );
    if (errStatus == noErr)
    {
        return standardHeight;
    }
    return -1;
}

void wxRendererMac::DrawTreeItemButton( wxWindow *win,
    wxDC& dc,
    const wxRect& rect,
    int flags )
{
#if !wxMAC_USE_CORE_GRAPHICS
    const wxCoord x = dc.LogicalToDeviceX(rect.x);
    const wxCoord y = dc.LogicalToDeviceY(rect.y);
    const wxCoord w = dc.LogicalToDeviceXRel(rect.width);
    const wxCoord h = dc.LogicalToDeviceYRel(rect.height);
#else
    // now the wxGCDC is using native transformations
    const wxCoord x = rect.x;
    const wxCoord y = rect.y;
    const wxCoord w = rect.width;
    const wxCoord h = rect.height;
#endif    

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
        cgContext = (CGContextRef) dc.GetGraphicsContext()->GetNativeContext();
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
            drawInfo.kind = kThemeDisclosureButton;
            drawInfo.state = (flags & wxCONTROL_DISABLED) ? kThemeStateInactive : kThemeStateActive;
            // Apple mailing list posts say to use the arrow adornment constants, but those don't work.
            // We need to set the value using the 'old' DrawThemeButton constants instead.
            drawInfo.value = (flags & wxCONTROL_EXPANDED) ? kThemeDisclosureDown : kThemeDisclosureRight;
            drawInfo.adornment = kThemeAdornmentNone; 

            HIThemeDrawButton( &headerRect, &drawInfo, cgContext, kHIThemeOrientationNormal, &labelRect );
            
        }

#if wxMAC_USE_CORE_GRAPHICS
#else
        QDEndCGContext( (CGrafPtr) dc.m_macPort, &cgContext );
#endif
    }
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
        cgContext = (CGContextRef) dc.GetGraphicsContext()->GetNativeContext();
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

void
wxRendererMac::DrawItemSelectionRect(wxWindow *win,
                                     wxDC& dc,
                                     const wxRect& rect,
                                     int flags )
{
    RGBColor selColor; 
    if (flags & wxCONTROL_SELECTED)
    {
        if (flags & wxCONTROL_FOCUSED)
            GetThemeBrushAsColor(kThemeBrushAlternatePrimaryHighlightColor, 32, true, &selColor);
        else
            GetThemeBrushAsColor(kThemeBrushSecondaryHighlightColor, 32, true, &selColor);
    }
    
    wxBrush selBrush = wxBrush( wxColour( selColor.red, selColor.green, selColor.blue ), wxSOLID );

    dc.SetPen( *wxTRANSPARENT_PEN );
    dc.SetBrush( selBrush );
    dc.DrawRectangle( rect );
}


void
wxRendererMac::DrawMacThemeButton(wxWindow *win,
                                  wxDC& dc,
                                  const wxRect& rect,
                                  int flags,
                                  int kind,
                                  int adornment)
{
#if !wxMAC_USE_CORE_GRAPHICS
    const wxCoord x = dc.LogicalToDeviceX(rect.x);
    const wxCoord y = dc.LogicalToDeviceY(rect.y);
    const wxCoord w = dc.LogicalToDeviceXRel(rect.width);
    const wxCoord h = dc.LogicalToDeviceYRel(rect.height);
#else
    // now the wxGCDC is using native transformations
    const wxCoord x = rect.x;
    const wxCoord y = rect.y;
    const wxCoord w = rect.width;
    const wxCoord h = rect.height;
#endif

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
        cgContext = (CGContextRef) dc.GetGraphicsContext()->GetNativeContext();
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
            drawInfo.kind = kind;
            drawInfo.state = (flags & wxCONTROL_DISABLED) ? kThemeStateInactive : kThemeStateActive;
            drawInfo.value = (flags & wxCONTROL_SELECTED) ? kThemeButtonOn : kThemeButtonOff;
            drawInfo.adornment = adornment;

            HIThemeDrawButton( &headerRect, &drawInfo, cgContext, kHIThemeOrientationNormal, &labelRect );
        }

#if wxMAC_USE_CORE_GRAPHICS
#else
        QDEndCGContext( (CGrafPtr) dc.m_macPort, &cgContext );
#endif
    }
}


void
wxRendererMac::DrawComboBoxDropButton(wxWindow *win,
                              wxDC& dc,
                              const wxRect& rect,
                              int flags)
{
    DrawMacThemeButton(win, dc, rect, flags,
                       kThemeArrowButton, kThemeAdornmentArrowDownArrow);
}
    
void
wxRendererMac::DrawPushButton(wxWindow *win,
                              wxDC& dc,
                              const wxRect& rect,
                              int flags)
{
    DrawMacThemeButton(win, dc, rect, flags,
                       kThemeBevelButton, kThemeAdornmentNone);
}
    

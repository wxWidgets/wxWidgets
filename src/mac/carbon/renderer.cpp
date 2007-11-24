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
    virtual int DrawHeaderButton( wxWindow *win,
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

    virtual void DrawCheckBox(wxWindow *win,
                              wxDC& dc,
                              const wxRect& rect,
                              int flags = 0);

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

    virtual void DrawFocusRect(wxWindow* win, wxDC& dc, const wxRect& rect, int flags = 0);

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

int wxRendererMac::DrawHeaderButton( wxWindow *win,
    wxDC& dc,
    const wxRect& rect,
    int flags,
    wxHeaderSortIconType sortArrow,
    wxHeaderButtonParams* params )
{
    const wxCoord x = rect.x;
    const wxCoord y = rect.y;
    const wxCoord w = rect.width;
    const wxCoord h = rect.height;

    dc.SetBrush( *wxTRANSPARENT_BRUSH );

    HIRect headerRect = CGRectMake( x, y, w, h );
    if ( !dc.IsKindOf( CLASSINFO( wxPaintDC ) ) )
    {
        win->Refresh( &rect );
    }
    else
    {
        CGContextRef cgContext;

        cgContext = (CGContextRef) dc.GetGraphicsContext()->GetNativeContext();

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

    return DrawHeaderButtonContents(win, dc, newRect, flags, sortArrow, params);
}


int wxRendererMac::GetHeaderButtonHeight(wxWindow* WXUNUSED(win))
{
    SInt32      standardHeight;
    OSStatus        errStatus;

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
    // now the wxGCDC is using native transformations
    const wxCoord x = rect.x;
    const wxCoord y = rect.y;
    const wxCoord w = rect.width;
    const wxCoord h = rect.height;

    dc.SetBrush( *wxTRANSPARENT_BRUSH );

    HIRect headerRect = CGRectMake( x, y, w, h );
    if ( !dc.IsKindOf( CLASSINFO( wxPaintDC ) ) )
    {
        win->Refresh( &rect );
    }
    else
    {
        CGContextRef cgContext;

        cgContext = (CGContextRef) dc.GetGraphicsContext()->GetNativeContext();

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

    // under compositing we should only draw when called by the OS, otherwise just issue a redraw command
    // strange redraw errors occur if we don't do this

    if ( !dc.IsKindOf( CLASSINFO( wxPaintDC ) ) )
    {
        wxRect rect( splitterRect.origin.x, splitterRect.origin.y, splitterRect.size.width, splitterRect.size.height );
        win->Refresh( &rect );
   }
    else
    {
        CGContextRef cgContext;

        cgContext = (CGContextRef) dc.GetGraphicsContext()->GetNativeContext();

        HIThemeSplitterDrawInfo drawInfo;
        drawInfo.version = 0;
        drawInfo.state = kThemeStateActive;
        drawInfo.adornment = hasMetal ? kHIThemeSplitterAdornmentMetal : kHIThemeSplitterAdornmentNone;
        HIThemeDrawPaneSplitter( &splitterRect, &drawInfo, cgContext, kHIThemeOrientationNormal );
    }
}

void
wxRendererMac::DrawItemSelectionRect(wxWindow *win,
                                     wxDC& dc,
                                     const wxRect& rect,
                                     int flags )
{
    if ( !(flags & wxCONTROL_SELECTED) )
        return;
        
    wxBrush selBrush(wxColour( wxMacCreateCGColorFromHITheme( flags & wxCONTROL_FOCUSED ? 
                                                             kThemeBrushAlternatePrimaryHighlightColor
                                                             : kThemeBrushSecondaryHighlightColor ) ) , wxSOLID );

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
    // now the wxGCDC is using native transformations
    const wxCoord x = rect.x;
    const wxCoord y = rect.y;
    const wxCoord w = rect.width;
    const wxCoord h = rect.height;

    dc.SetBrush( *wxTRANSPARENT_BRUSH );

    HIRect headerRect = CGRectMake( x, y, w, h );
    if ( !dc.IsKindOf( CLASSINFO( wxPaintDC ) ) )
    {
        win->Refresh( &rect );
    }
    else
    {
        CGContextRef cgContext;
        cgContext = (CGContextRef) dc.GetGraphicsContext()->GetNativeContext();

        HIThemeButtonDrawInfo drawInfo;
        HIRect labelRect;

        memset( &drawInfo, 0, sizeof(drawInfo) );
        drawInfo.version = 0;
        drawInfo.kind = kind;
        drawInfo.state = (flags & wxCONTROL_DISABLED) ? kThemeStateInactive : kThemeStateActive;
        drawInfo.value = (flags & wxCONTROL_SELECTED) ? kThemeButtonOn : kThemeButtonOff;
        if (flags & wxCONTROL_UNDETERMINED)
            drawInfo.value = kThemeButtonMixed;
        drawInfo.adornment = adornment;

        HIThemeDrawButton( &headerRect, &drawInfo, cgContext, kHIThemeOrientationNormal, &labelRect );
    }
}

void
wxRendererMac::DrawCheckBox(wxWindow *win,
                            wxDC& dc,
                            const wxRect& rect,
                            int flags)
{
    if (flags & wxCONTROL_CHECKED)
        flags |= wxCONTROL_SELECTED;

    DrawMacThemeButton(win, dc, rect, flags,
                       kThemeCheckBox, kThemeAdornmentNone);
}

void
wxRendererMac::DrawComboBoxDropButton(wxWindow *win,
                              wxDC& dc,
                              const wxRect& rect,
                              int flags)
{
    int kind;
    if (win->GetWindowVariant() == wxWINDOW_VARIANT_SMALL || (win->GetParent() && win->GetParent()->GetWindowVariant() == wxWINDOW_VARIANT_SMALL))
        kind = kThemeArrowButtonSmall;
    else if (win->GetWindowVariant() == wxWINDOW_VARIANT_MINI || (win->GetParent() && win->GetParent()->GetWindowVariant() == wxWINDOW_VARIANT_MINI))
        kind = kThemeArrowButtonMini;
    else
        kind = kThemeArrowButton;

    DrawMacThemeButton(win, dc, rect, flags,
                       kind, kThemeAdornmentArrowDownArrow);
}

void
wxRendererMac::DrawPushButton(wxWindow *win,
                              wxDC& dc,
                              const wxRect& rect,
                              int flags)
{
    int kind;
    if (win->GetWindowVariant() == wxWINDOW_VARIANT_SMALL || (win->GetParent() && win->GetParent()->GetWindowVariant() == wxWINDOW_VARIANT_SMALL))
        kind = kThemeBevelButtonSmall;
    // There is no kThemeBevelButtonMini, but in this case, use Small
    else if (win->GetWindowVariant() == wxWINDOW_VARIANT_MINI || (win->GetParent() && win->GetParent()->GetWindowVariant() == wxWINDOW_VARIANT_MINI))
        kind = kThemeBevelButtonSmall;
    else
        kind = kThemeBevelButton;

    DrawMacThemeButton(win, dc, rect, flags,
                       kind, kThemeAdornmentNone);
}

void
wxRendererMac::DrawFocusRect(wxWindow* win, wxDC& dc, const wxRect& rect, int flags)
{
    if (!win)
    {
        wxDelegateRendererNative::DrawFocusRect(win, dc, rect, flags);
        return;
    }

    CGRect cgrect = CGRectMake( rect.x , rect.y , rect.width, rect.height ) ;

    HIThemeFrameDrawInfo info ;
    memset( &info, 0 , sizeof(info) ) ;

    info.version = 0 ;
    info.kind = 0 ;
    info.state = kThemeStateActive;
    info.isFocused = true ;

    CGContextRef cgContext = (CGContextRef) win->MacGetCGContextRef() ;
    wxASSERT( cgContext ) ;

    HIThemeDrawFocusRect( &cgrect , true , cgContext , kHIThemeOrientationNormal ) ;
}


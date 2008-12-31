///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/carbon/renderer.cpp
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
#include "wx/dcgraph.h"
#include "wx/osx/private.h"

#if wxOSX_USE_COCOA
// bring in the theme headers
#include <Carbon/Carbon.h>
#endif

// check if we're currently in a paint event
inline bool wxInPaintEvent(wxWindow* win, wxDC& dc)
{
    return ( win->MacGetCGContextRef() != NULL );
}



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

    virtual wxSize GetCheckBoxSize(wxWindow* win);

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

    virtual void DrawChoice(wxWindow* win, wxDC& dc, const wxRect& rect, int flags=0);

    virtual void DrawComboBox(wxWindow* win, wxDC& dc, const wxRect& rect, int flags=0);

    virtual void DrawTextCtrl(wxWindow* win, wxDC& dc, const wxRect& rect, int flags=0);

    virtual void DrawRadioButton(wxWindow* win, wxDC& dc, const wxRect& rect, int flags=0);

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
    if ( !wxInPaintEvent(win, dc) )
    {
        win->Refresh( &rect );
    }
    else
    {
        CGContextRef cgContext;
        wxGCDCImpl *impl = (wxGCDCImpl*) dc.GetImpl();

        cgContext = (CGContextRef) impl->GetGraphicsContext()->GetNativeContext();

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
    if ( !wxInPaintEvent(win, dc) )
    {
        win->Refresh( &rect );
    }
    else
    {
        CGContextRef cgContext;

        wxGCDCImpl *impl = (wxGCDCImpl*) dc.GetImpl();
        cgContext = (CGContextRef) impl->GetGraphicsContext()->GetNativeContext();

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
    bool hasMetal = win->MacGetTopLevelWindow()->GetExtraStyle() & wxFRAME_EX_METAL;
    SInt32 height;
    GetThemeMetric( kThemeMetricSmallPaneSplitterHeight, &height );
    HIRect splitterRect;
    if (orient == wxVERTICAL)
        splitterRect = CGRectMake( position, 0, height, size.y );
    else
        splitterRect = CGRectMake( 0, position, size.x, height );

    // under compositing we should only draw when called by the OS, otherwise just issue a redraw command
    // strange redraw errors occur if we don't do this

    if ( !wxInPaintEvent(win, dc) )
    {
        wxRect rect( (int) splitterRect.origin.x, (int) splitterRect.origin.y, (int) splitterRect.size.width,
                     (int) splitterRect.size.height );
        win->Refresh( &rect );
   }
    else
    {
        CGContextRef cgContext;
        wxGCDCImpl *impl = (wxGCDCImpl*) dc.GetImpl();
        cgContext = (CGContextRef) impl->GetGraphicsContext()->GetNativeContext();

        HIThemeSplitterDrawInfo drawInfo;
        drawInfo.version = 0;
        drawInfo.state = kThemeStateActive;
        drawInfo.adornment = hasMetal ? kHIThemeSplitterAdornmentMetal : kHIThemeSplitterAdornmentNone;
        HIThemeDrawPaneSplitter( &splitterRect, &drawInfo, cgContext, kHIThemeOrientationNormal );
    }
}

void
wxRendererMac::DrawItemSelectionRect(wxWindow * WXUNUSED(win),
                                     wxDC& dc,
                                     const wxRect& rect,
                                     int flags)
{
    if ( !(flags & wxCONTROL_SELECTED) )
        return;

    wxColour col( wxMacCreateCGColorFromHITheme( (flags & wxCONTROL_FOCUSED) ?
                                                 kThemeBrushAlternatePrimaryHighlightColor
                                                                             : kThemeBrushSecondaryHighlightColor ) );
    wxBrush selBrush( col );

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
    if ( !wxInPaintEvent(win, dc) )
    {
        win->Refresh( &rect );
    }
    else
    {
        wxGCDCImpl *impl = (wxGCDCImpl*) dc.GetImpl();
        CGContextRef cgContext;
        cgContext = (CGContextRef) impl->GetGraphicsContext()->GetNativeContext();

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
        if (flags & wxCONTROL_FOCUSED)
            drawInfo.adornment |= kThemeAdornmentFocus;
        
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

    int kind;
    
    if (win->GetWindowVariant() == wxWINDOW_VARIANT_SMALL ||
        (win->GetParent() && win->GetParent()->GetWindowVariant() == wxWINDOW_VARIANT_SMALL))
        kind = kThemeCheckBoxSmall;
    else if (win->GetWindowVariant() == wxWINDOW_VARIANT_MINI ||
             (win->GetParent() && win->GetParent()->GetWindowVariant() == wxWINDOW_VARIANT_MINI))
        kind = kThemeCheckBoxMini;
    else
        kind = kThemeCheckBox;


    DrawMacThemeButton(win, dc, rect, flags,
                       kind, kThemeAdornmentNone);
}

wxSize wxRendererMac::GetCheckBoxSize(wxWindow* WXUNUSED(win))
{
    wxSize size;
    SInt32 width, height;
    OSStatus errStatus;

    errStatus = GetThemeMetric(kThemeMetricCheckBoxWidth, &width);
    if (errStatus == noErr)
    {
        size.SetWidth(width);
    }

    errStatus = GetThemeMetric(kThemeMetricCheckBoxHeight, &height);
    if (errStatus == noErr)
    {
        size.SetHeight(height);
    }

    return size;
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

void wxRendererMac::DrawChoice(wxWindow* win, wxDC& dc,
                           const wxRect& rect, int flags)
{
    int kind;
    
    if (win->GetWindowVariant() == wxWINDOW_VARIANT_SMALL ||
        (win->GetParent() && win->GetParent()->GetWindowVariant() == wxWINDOW_VARIANT_SMALL))
        kind = kThemePopupButtonSmall;
    else if (win->GetWindowVariant() == wxWINDOW_VARIANT_MINI ||
             (win->GetParent() && win->GetParent()->GetWindowVariant() == wxWINDOW_VARIANT_MINI))
        kind = kThemePopupButtonMini;
    else
        kind = kThemePopupButton;

    DrawMacThemeButton(win, dc, rect, flags, kind, kThemeAdornmentNone);
}


void wxRendererMac::DrawComboBox(wxWindow* win, wxDC& dc,
                             const wxRect& rect, int flags)
{
    int kind;
    
    if (win->GetWindowVariant() == wxWINDOW_VARIANT_SMALL ||
        (win->GetParent() && win->GetParent()->GetWindowVariant() == wxWINDOW_VARIANT_SMALL))
        kind = kThemeComboBoxSmall;
    else if (win->GetWindowVariant() == wxWINDOW_VARIANT_MINI ||
             (win->GetParent() && win->GetParent()->GetWindowVariant() == wxWINDOW_VARIANT_MINI))
        kind = kThemeComboBoxMini;
    else
        kind = kThemeComboBox;

    DrawMacThemeButton(win, dc, rect, flags, kind, kThemeAdornmentNone);
}

void wxRendererMac::DrawRadioButton(wxWindow* win, wxDC& dc,
                                const wxRect& rect, int flags)
{
    int kind;
    
    if (win->GetWindowVariant() == wxWINDOW_VARIANT_SMALL ||
        (win->GetParent() && win->GetParent()->GetWindowVariant() == wxWINDOW_VARIANT_SMALL))
        kind = kThemeRadioButtonSmall;
    else if (win->GetWindowVariant() == wxWINDOW_VARIANT_MINI ||
             (win->GetParent() && win->GetParent()->GetWindowVariant() == wxWINDOW_VARIANT_MINI))
        kind = kThemeRadioButtonMini;
    else
        kind = kThemeRadioButton;

    if (flags & wxCONTROL_CHECKED)
        flags |= wxCONTROL_SELECTED;

    DrawMacThemeButton(win, dc, rect, flags,
                          kind, kThemeAdornmentNone);
}

void wxRendererMac::DrawTextCtrl(wxWindow* win, wxDC& dc,
                             const wxRect& rect, int flags)
{
    const wxCoord x = rect.x;
    const wxCoord y = rect.y;
    const wxCoord w = rect.width;
    const wxCoord h = rect.height;

    dc.SetBrush( *wxWHITE_BRUSH );
    dc.SetPen( *wxTRANSPARENT_PEN );
    dc.DrawRectangle(rect);
    
    dc.SetBrush( *wxTRANSPARENT_BRUSH );

    HIRect hiRect = CGRectMake( x, y, w, h );
    if ( !wxInPaintEvent(win, dc) )
    {
        win->Refresh( &rect );
    }
    else
    {
        CGContextRef cgContext;

        cgContext = (CGContextRef) static_cast<wxGCDCImpl*>(dc.GetImpl())->GetGraphicsContext()->GetNativeContext();

        {
            HIThemeFrameDrawInfo drawInfo;

            memset( &drawInfo, 0, sizeof(drawInfo) );
            drawInfo.version = 0;
            drawInfo.kind = kHIThemeFrameTextFieldSquare;
            drawInfo.state = (flags & wxCONTROL_DISABLED) ? kThemeStateInactive : kThemeStateActive;
            if (flags & wxCONTROL_FOCUSED)
                drawInfo.isFocused = true;

            HIThemeDrawFrame( &hiRect, &drawInfo, cgContext, kHIThemeOrientationNormal);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/cocoa/renderer.mm
// Purpose:     implementation of wxRendererNative for Cocoa
// Author:      Vadim Zeitlin, Stefan Csomor
// Created:     20.07.2003
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxOSX_USE_COCOA_OR_CARBON


#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/dc.h"
    #include "wx/bitmap.h"
    #include "wx/settings.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/toplevel.h"
#endif

#include "wx/renderer.h"
#include "wx/graphics.h"
#include "wx/dcgraph.h"
#include "wx/splitter.h"
#include "wx/time.h"
#include "wx/osx/private.h"
#include "wx/osx/private/available.h"

#ifdef wxHAS_DRAW_TITLE_BAR_BITMAP
    #include "wx/image.h"
    #include "wx/mstream.h"
#endif // wxHAS_DRAW_TITLE_BAR_BITMAP

#define wxOSX_USE_NSCELL_RENDERER 1

// check if we're having a CGContext we can draw into
inline bool wxHasCGContext(wxWindow* WXUNUSED(win), wxDC& dc)
{
    wxGCDCImpl* gcdc = wxDynamicCast( dc.GetImpl() , wxGCDCImpl);

    if ( gcdc )
    {
        if ( gcdc->GetGraphicsContext()->GetNativeContext() )
            return true;
    }
    return false;
}



class WXDLLEXPORT wxRendererMac : public wxDelegateRendererNative
{
public:
    wxRendererMac() = default;
    virtual ~wxRendererMac();

    // draw the header control button (used by wxListCtrl)
    virtual int DrawHeaderButton( wxWindow *win,
        wxDC& dc,
        const wxRect& rect,
        int flags = 0,
        wxHeaderSortIconType sortArrow = wxHDR_SORT_ICON_NONE,
        wxHeaderButtonParams* params = nullptr ) override;

    virtual int GetHeaderButtonHeight(wxWindow *win) override;

    virtual int GetHeaderButtonMargin(wxWindow *win) override;

    // draw the expanded/collapsed icon for a tree control item
    virtual void DrawTreeItemButton( wxWindow *win,
        wxDC& dc,
        const wxRect& rect,
        int flags = 0 ) override;

    // draw a (vertical) sash
    virtual void DrawSplitterSash( wxWindow *win,
        wxDC& dc,
        const wxSize& size,
        wxCoord position,
        wxOrientation orient,
        int flags = 0 ) override;

    virtual void DrawCheckBox(wxWindow *win,
                              wxDC& dc,
                              const wxRect& rect,
                              int flags = 0) override;

    virtual wxSize GetCheckBoxSize(wxWindow* win, int flags = 0) override;

    virtual void DrawComboBoxDropButton(wxWindow *win,
                                        wxDC& dc,
                                        const wxRect& rect,
                                        int flags = 0) override;

    virtual void DrawPushButton(wxWindow *win,
                                wxDC& dc,
                                const wxRect& rect,
                                int flags = 0) override;

    virtual void DrawCollapseButton(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0) override;

    virtual wxSize GetCollapseButtonSize(wxWindow *win, wxDC& dc) override;

    virtual void DrawItemSelectionRect(wxWindow *win,
                                       wxDC& dc,
                                       const wxRect& rect,
                                       int flags = 0) override;

    virtual void DrawFocusRect(wxWindow* win, wxDC& dc, const wxRect& rect, int flags = 0) override;

    virtual void DrawChoice(wxWindow* win, wxDC& dc, const wxRect& rect, int flags=0) override;

    virtual void DrawComboBox(wxWindow* win, wxDC& dc, const wxRect& rect, int flags=0) override;

    virtual void DrawTextCtrl(wxWindow* win, wxDC& dc, const wxRect& rect, int flags=0) override;

    virtual void DrawRadioBitmap(wxWindow* win, wxDC& dc, const wxRect& rect, int flags=0) override;

#ifdef wxHAS_DRAW_TITLE_BAR_BITMAP
    virtual void DrawTitleBarBitmap(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    wxTitleBarButton button,
                                    int flags = 0) override;
#endif // wxHAS_DRAW_TITLE_BAR_BITMAP

    virtual void DrawGauge(wxWindow* win,
                           wxDC& dc,
                           const wxRect& rect,
                           int value,
                           int max,
                           int flags = 0) override;

    virtual wxSplitterRenderParams GetSplitterParams(const wxWindow *win) override;

private:
    void DrawMacThemeButton(wxWindow *win,
                            wxDC& dc,
                            const wxRect& rect,
                            int flags,
                            int kind,
                            int adornment);
#if wxOSX_USE_NSCELL_RENDERER
    void DrawMacCell(wxWindow *win,
                            wxDC& dc,
                            NSCell* cell,
                            const wxRect& rect,
                            int flags,
                            NSControlStateValue state);

   void DrawMacHeaderCell(wxWindow *win,
                            wxDC& dc,
                            NSTableHeaderCell* cell,
                            const wxRect& rect,
                            int flags,
                            wxHeaderSortIconType ascending,
                            wxHeaderButtonParams* params);
#endif
    void ApplyMacControlFlags(wxWindow* win, NSCell* cell, int flags);

    // the tree buttons
    wxBitmap m_bmpTreeExpanded;
    wxBitmap m_bmpTreeCollapsed;

#if wxOSX_USE_NSCELL_RENDERER
    NSButtonCell* GetPushButtonCell()
    {
        if ( !m_nsPushButtonCell )
        {
            m_nsPushButtonCell = [[NSButtonCell alloc] initTextCell:@""];
            m_nsPushButtonCell.buttonType = NSButtonTypeMomentaryPushIn;
            m_nsPushButtonCell.highlightsBy = NSPushInCellMask;
            m_nsPushButtonCell.bezelStyle = NSBezelStyleShadowlessSquare;
        }

        return m_nsPushButtonCell;
    }

    NSButtonCell* GetCheckBoxCell()
    {
        if ( !m_nsCheckBoxCell )
        {
            m_nsCheckBoxCell = [[NSButtonCell alloc] initTextCell:@""];
            m_nsCheckBoxCell.buttonType = NSButtonTypeSwitch;
            m_nsCheckBoxCell.allowsMixedState = YES;
        }

        return m_nsCheckBoxCell;
    }

    NSButtonCell* GetRadioButtonCell()
    {
        if ( !m_nsRadioButtonCell )
        {
            m_nsRadioButtonCell = [[NSButtonCell alloc] initTextCell:@""];
            m_nsRadioButtonCell.buttonType = NSButtonTypeRadio;
            m_nsRadioButtonCell.allowsMixedState = YES;
        }

        return m_nsRadioButtonCell;
    }

    NSButtonCell* GetDisclosureButtonCell()
    {
        if ( !m_nsDisclosureButtonCell )
        {
            m_nsDisclosureButtonCell = [[NSButtonCell alloc] initTextCell:@""];
            m_nsDisclosureButtonCell.bezelStyle = NSBezelStyleDisclosure;
            m_nsDisclosureButtonCell.buttonType = NSButtonTypePushOnPushOff;
            m_nsDisclosureButtonCell.highlightsBy = NSPushInCellMask;
        }

        return m_nsDisclosureButtonCell;
    }

    NSPopUpButtonCell* GetPopupbuttonCell()
    {
        if ( !m_nsPopupbuttonCell )
        {
            m_nsPopupbuttonCell = [[NSPopUpButtonCell alloc] initTextCell:@"" pullsDown:NO];
        }

        return m_nsPopupbuttonCell;
    }

    NSComboBoxCell* GetComboBoxCell()
    {
        if ( !m_nsComboBoxCell )
        {
            m_nsComboBoxCell = [[NSComboBoxCell alloc] initTextCell:@""];
        }

        return m_nsComboBoxCell;
    }

    NSTableHeaderCell* GetTableHeaderCell()
    {
        if ( !m_nsTableHeaderCell )
        {
            m_nsTableHeaderCell = [[NSTableHeaderCell alloc] init];
            m_nsTableHeaderCell.bezeled = NO;
            m_nsTableHeaderCell.bezelStyle = NSTextFieldSquareBezel;
            m_nsTableHeaderCell.bordered = NO;
        }

        return m_nsTableHeaderCell;
    }

    // These variables shouldn't be accessed directly as they're allocated on
    // demand, use the getters above.
    NSButtonCell* m_nsPushButtonCell = nil;
    NSButtonCell* m_nsCheckBoxCell = nil;
    NSButtonCell* m_nsRadioButtonCell = nil;
    NSButtonCell* m_nsDisclosureButtonCell = nil;
    NSPopUpButtonCell* m_nsPopupbuttonCell = nil;
    NSComboBoxCell* m_nsComboBoxCell = nil;
    NSTableHeaderCell* m_nsTableHeaderCell = nil;
#endif
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

wxRendererMac::~wxRendererMac()
{
#if wxOSX_USE_NSCELL_RENDERER
    [m_nsPushButtonCell release];
    [m_nsCheckBoxCell release];
    [m_nsRadioButtonCell release];
    [m_nsDisclosureButtonCell release];
    [m_nsPopupbuttonCell release];
    [m_nsComboBoxCell release];
    [m_nsTableHeaderCell release];
#endif
}

int wxRendererMac::DrawHeaderButton( wxWindow *win,
    wxDC& dc,
    const wxRect& rect,
    int flags,
    wxHeaderSortIconType sortArrow,
    wxHeaderButtonParams* params )
{
#if wxOSX_USE_NSCELL_RENDERER
    DrawMacHeaderCell(win, dc, GetTableHeaderCell(), rect, flags, sortArrow, params);
    return GetTableHeaderCell().cellSize.width;
#else
    if ( wxSystemSettings::GetAppearance().IsDark() )
        return wxRendererNative::GetGeneric().DrawHeaderButton(win, dc,  rect, flags, sortArrow, params);

    const wxCoord x = rect.x;
    const wxCoord y = rect.y;
    const wxCoord w = rect.width;
    const wxCoord h = rect.height;

    wxDCBrushChanger setBrush(dc, *wxTRANSPARENT_BRUSH);

    HIRect headerRect = CGRectMake( x, y, w, h );
    if ( !wxHasCGContext(win, dc) )
    {
        win->RefreshRect(rect);
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
            drawInfo.adornment = kThemeAdornmentNone;
            drawInfo.value = kThemeButtonOff;
            if ( flags & wxCONTROL_DISABLED )
                drawInfo.state = kThemeStateInactive;
            else if ( flags & wxCONTROL_PRESSED )
                drawInfo.state = kThemeStatePressed;
            else
                drawInfo.state = kThemeStateActive;

            // The down arrow is drawn automatically (if value is kThemeButtonOn)
            // change it to an up arrow if needed.
            if ( sortArrow == wxHDR_SORT_ICON_UP )
            {
                drawInfo.adornment = kThemeAdornmentHeaderButtonSortUp;
                drawInfo.value = kThemeButtonOn;
            }
            else if (sortArrow == wxHDR_SORT_ICON_DOWN )
            {
                drawInfo.value = kThemeButtonOn;
            }

            HIThemeDrawButton( &headerRect, &drawInfo, cgContext, kHIThemeOrientationNormal, &labelRect );
        }
    }

    // Reserve room for the arrows before writing the label, and turn off the
    // flags we've already handled
    wxRect newRect(rect);
    if ( sortArrow != wxHDR_SORT_ICON_NONE )
    {
        newRect.width -= 12;
        sortArrow = wxHDR_SORT_ICON_NONE;
    }
    flags &= ~wxCONTROL_PRESSED;

    return DrawHeaderButtonContents(win, dc, newRect, flags, sortArrow, params);
#endif
}


int wxRendererMac::GetHeaderButtonHeight(wxWindow* win)
{
#if wxOSX_USE_NSCELL_RENDERER
    ApplyMacControlFlags( win, GetTableHeaderCell(), 0);
    return GetTableHeaderCell().cellSize.height;
#else
    SInt32      standardHeight;
    OSStatus        errStatus;

    errStatus = GetThemeMetric( kThemeMetricListHeaderHeight, &standardHeight );
    if (errStatus == noErr)
    {
        return standardHeight;
    }
    return -1;
#endif
}

int wxRendererMac::GetHeaderButtonMargin(wxWindow *WXUNUSED(win))
{
    return 0; // TODO: How to determine the real margin?
}

void wxRendererMac::DrawTreeItemButton( wxWindow *win,
    wxDC& dc,
    const wxRect& rect,
    int flags )
{
    if ( !wxHasCGContext(win, dc) )
    {
        win->RefreshRect(rect);
    }
    else
    {
#if wxOSX_USE_NSCELL_RENDERER
        NSControlStateValue stateValue = (flags & wxCONTROL_EXPANDED) ? NSControlStateValueOn : NSControlStateValueOff;
        DrawMacCell(win, dc, GetDisclosureButtonCell(), rect, flags, stateValue);
#else
        // now the wxGCDC is using native transformations
        const wxCoord x = rect.x;
        const wxCoord y = rect.y;
        const wxCoord w = rect.width;
        const wxCoord h = rect.height;

        wxDCBrushChanger setBrush(dc, *wxTRANSPARENT_BRUSH);

        HIRect headerRect = CGRectMake( x, y, w, h );
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
#endif
    }
}

wxSplitterRenderParams
wxRendererMac::GetSplitterParams(const wxWindow *win)
{
    // see below
    SInt32 sashWidth,
            border;
#if wxOSX_USE_COCOA
    if ( win->HasFlag(wxSP_3DSASH) )
        GetThemeMetric( kThemeMetricPaneSplitterHeight, &sashWidth ); // Cocoa == Carbon == 7
    else if ( win->HasFlag(wxSP_NOSASH) ) // actually Cocoa doesn't allow 0
        sashWidth = 0;
    else // no 3D effect - Cocoa [NSSplitView dividerThickNess] for NSSplitViewDividerStyleThin
        sashWidth = 1;
#else // Carbon
    if ( win->HasFlag(wxSP_3DSASH) )
        GetThemeMetric( kThemeMetricPaneSplitterHeight, &sashWidth );
    else if ( win->HasFlag(wxSP_NOSASH) )
        sashWidth = 0;
    else // no 3D effect
        GetThemeMetric( kThemeMetricSmallPaneSplitterHeight, &sashWidth );
#endif // Cocoa/Carbon

    if ( win->HasFlag(wxSP_3DBORDER) )
        border = 2;
    else // no 3D effect
        border = 0;

    return wxSplitterRenderParams(sashWidth, border, false);
}


void wxRendererMac::DrawSplitterSash( wxWindow *win,
    wxDC& dc,
    const wxSize& size,
    wxCoord position,
    wxOrientation orient,
    int WXUNUSED(flags) )
{
    // Note that we can't use ternary ?: operator or any other construct with
    // logical operators here, WX_IS_MACOS_AVAILABLE() must appear inside an
    // "if" statement to avoid -Wunsupported-availability-guard with Xcode 10.
    bool hasMetal;
    if (WX_IS_MACOS_AVAILABLE(10, 14))
        hasMetal = false;
    else
        hasMetal = win->MacGetTopLevelWindow()->GetExtraStyle() & wxFRAME_EX_METAL;

    SInt32 height;

    height = wxRendererNative::Get().GetSplitterParams(win).widthSash;

    // Do not draw over border drawn by wxRendererGeneric::DrawSplitterBorder()
    const wxCoord borderAdjust = win->HasFlag(wxSP_3DBORDER) ? 2 : 0;

    HIRect splitterRect;
    if (orient == wxVERTICAL)
        splitterRect = CGRectMake( position, borderAdjust, height, size.y - 2*borderAdjust );
    else
        splitterRect = CGRectMake( borderAdjust, position, size.x - 2*borderAdjust, height );

    // under compositing we should only draw when called by the OS, otherwise just issue a redraw command
    // strange redraw errors occur if we don't do this

    if ( !wxHasCGContext(win, dc) )
    {
        wxRect rect( (int) splitterRect.origin.x, (int) splitterRect.origin.y, (int) splitterRect.size.width,
                     (int) splitterRect.size.height );
        win->RefreshRect( rect );
    }
    else
    {
        CGContextRef cgContext;
        wxGCDCImpl *impl = (wxGCDCImpl*) dc.GetImpl();
        cgContext = (CGContextRef) impl->GetGraphicsContext()->GetNativeContext();

        if ( hasMetal )
        {
            HIThemeBackgroundDrawInfo bgdrawInfo;
            bgdrawInfo.version = 0;
            bgdrawInfo.state = kThemeStateActive;
            bgdrawInfo.kind = hasMetal ? kThemeBackgroundMetal : kThemeBackgroundPlacard;

            HIThemeDrawBackground(&splitterRect, &bgdrawInfo, cgContext, kHIThemeOrientationNormal);
        }
        else
        {
            CGContextSetFillColorWithColor(cgContext,win->GetBackgroundColour().GetCGColor());
            CGContextFillRect(cgContext,splitterRect);
        }

        if ( win->HasFlag(wxSP_3DSASH) )
        {
            if ( !wxSystemSettings::GetAppearance().IsDark() )
            {
                HIThemeSplitterDrawInfo drawInfo;
                drawInfo.version = 0;
                drawInfo.state = kThemeStateActive;
                drawInfo.adornment = hasMetal ? kHIThemeSplitterAdornmentMetal : kHIThemeSplitterAdornmentNone;
                HIThemeDrawPaneSplitter( &splitterRect, &drawInfo, cgContext, kHIThemeOrientationNormal );
            }
        }
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

    wxDCPenChanger setPen(dc, *wxTRANSPARENT_PEN);
    wxDCBrushChanger setBrush(dc, selBrush);
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

    wxDCBrushChanger setBrush(dc, *wxTRANSPARENT_BRUSH);

    HIRect headerRect = CGRectMake( x, y, w, h );
    if ( !wxHasCGContext(win, dc) )
    {
        win->RefreshRect(rect);
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
        drawInfo.value = (flags & wxCONTROL_PRESSED) ? kThemeButtonOn : kThemeButtonOff;
        if (flags & wxCONTROL_UNDETERMINED)
            drawInfo.value = kThemeButtonMixed;
        drawInfo.adornment = adornment;
        if (flags & wxCONTROL_FOCUSED)
            drawInfo.adornment |= kThemeAdornmentFocus;

        HIThemeDrawButton( &headerRect, &drawInfo, cgContext, kHIThemeOrientationNormal, &labelRect );
    }
}

#if wxOSX_USE_NSCELL_RENDERER

void wxRendererMac::ApplyMacControlFlags(wxWindow* win, NSCell* cell, int flags)
{
    NSControlSize size = NSRegularControlSize;
    if (win->GetWindowVariant() == wxWINDOW_VARIANT_SMALL || (win->GetParent() && win->GetParent()->GetWindowVariant() == wxWINDOW_VARIANT_SMALL))
        size = NSSmallControlSize;
    else if (win->GetWindowVariant() == wxWINDOW_VARIANT_MINI || (win->GetParent() && win->GetParent()->GetWindowVariant() == wxWINDOW_VARIANT_MINI))
        size = NSMiniControlSize;
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_VERSION_11_0
    else if (win->GetWindowVariant() == wxWINDOW_VARIANT_LARGE|| (win->GetParent() && win->GetParent()->GetWindowVariant() == wxWINDOW_VARIANT_LARGE))
    {
        if (WX_IS_MACOS_AVAILABLE(11, 0))
            size = NSControlSizeLarge;
        else
            size = NSControlSizeRegular;
    }
#endif
    else
        size = NSRegularControlSize;
    cell.controlSize = size;

    cell.enabled = !(flags & wxCONTROL_DISABLED);
    cell.highlighted = (flags & wxCONTROL_PRESSED);
    cell.controlTint = (flags & wxCONTROL_FOCUSED) ? NSColor.currentControlTint : NSClearControlTint;
}


void wxRendererMac::DrawMacCell(wxWindow *win,
                            wxDC& dc,
                            NSCell* cell,
                            const wxRect& rect,
                            int flags,
                            NSControlStateValue state)
{
    wxDCBrushChanger setBrush(dc, *wxTRANSPARENT_BRUSH);

    if ( !wxHasCGContext(win, dc) )
    {
        win->RefreshRect(rect);
    }
    else
    {
        const wxCoord x = rect.x;
        const wxCoord y = rect.y;
        const wxCoord w = rect.width;
        const wxCoord h = rect.height;

        NSRect controlRect = NSMakeRect( x, y, w, h );
        ApplyMacControlFlags(win, cell, flags);
        cell.state = state;


        wxGCDCImpl *impl = (wxGCDCImpl*) dc.GetImpl();
        CGContextRef cgContext = (CGContextRef) impl->GetGraphicsContext()->GetNativeContext();

        CGContextSaveGState(cgContext);

        NSGraphicsContext* formerContext = NSGraphicsContext.currentContext;
        NSGraphicsContext.currentContext = [NSGraphicsContext graphicsContextWithCGContext:cgContext
                                                   flipped:YES];

        [cell drawWithFrame:controlRect inView:(NSView*) win->GetHandle()];
        if (flags & wxCONTROL_FOCUSED)
        {
            NSSetFocusRingStyle(NSFocusRingOnly);
            // we must draw into a separate layer, otherwise every single subcell eg in a combobox
            // will have its own focus rect drawn
            CGContextBeginTransparencyLayerWithRect(cgContext, NSRectToCGRect(controlRect), 0);
            [cell drawFocusRingMaskWithFrame:controlRect inView:(NSView*) win->GetHandle()];
            CGContextEndTransparencyLayer(cgContext);
        }

        NSGraphicsContext.currentContext = formerContext;

        CGContextRestoreGState(cgContext);
    }
}

void wxRendererMac::DrawMacHeaderCell(wxWindow *win,
                            wxDC& dc,
                            NSTableHeaderCell* cell,
                            const wxRect& rect,
                            int flags,
                            wxHeaderSortIconType sortArrow,
                            wxHeaderButtonParams* params)
{
    wxDCBrushChanger setBrush(dc, *wxTRANSPARENT_BRUSH);

    if ( !wxHasCGContext(win, dc) )
    {
        win->RefreshRect(rect);
    }
    else
    {
        wxCoord x = rect.x;
        wxCoord y = rect.y;
        wxCoord w = rect.width;
        wxCoord h = rect.height;

        NSRect controlRect = NSMakeRect( x, y, w, h );

        ApplyMacControlFlags(win, cell, flags);

        NSString* title = @("");
        NSTextAlignment alignment = NSTextAlignmentLeft;

        if ( params )
        {
            title = wxCFStringRef(params->m_labelText).AsNSString();
            switch( params->m_labelAlignment )
            {
                case wxALIGN_CENTER:
                    cell.alignment = NSTextAlignmentCenter;
                    break;
                case wxALIGN_RIGHT:
                    cell.alignment = NSTextAlignmentRight;
                    break;
                case wxALIGN_LEFT:
                default:
                    cell.alignment = NSTextAlignmentLeft;
            }

        }

        cell.title = title;
        cell.alignment = alignment;

        wxGCDCImpl *impl = (wxGCDCImpl*) dc.GetImpl();
        CGContextRef cgContext = (CGContextRef) impl->GetGraphicsContext()->GetNativeContext();

        CGContextSaveGState(cgContext);

        NSGraphicsContext* formerContext = NSGraphicsContext.currentContext;
        NSGraphicsContext.currentContext = [NSGraphicsContext graphicsContextWithCGContext:cgContext
                                                   flipped:YES];

        [cell drawWithFrame:controlRect inView:(NSView*) win->GetHandle()];

        if ( sortArrow == wxHDR_SORT_ICON_UP || sortArrow == wxHDR_SORT_ICON_DOWN )
        {
            BOOL ascending = sortArrow == wxHDR_SORT_ICON_UP;
            [cell drawSortIndicatorWithFrame:controlRect inView:(NSView*) win->GetHandle()
                ascending:ascending priority:0];
        }

        NSGraphicsContext.currentContext = formerContext;

        CGContextRestoreGState(cgContext);
    }
}
#endif

void
wxRendererMac::DrawCheckBox(wxWindow *win,
                            wxDC& dc,
                            const wxRect& rect,
                            int flags)
{
    if (flags & wxCONTROL_CHECKED)
        flags |= wxCONTROL_PRESSED;
#if wxOSX_USE_NSCELL_RENDERER
    NSControlStateValue stateValue = (flags & wxCONTROL_PRESSED) ? NSControlStateValueOn : NSControlStateValueOff;
    if (flags & wxCONTROL_UNDETERMINED)
            stateValue = NSControlStateValueMixed;

    DrawMacCell(win, dc, GetCheckBoxCell(), rect, flags, stateValue);
#else
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
#endif
}

wxSize wxRendererMac::GetCheckBoxSize(wxWindow* win, int flags)
{
    // Even though we don't use the window in this implementation, still check
    // that it's valid to avoid surprises when running the same code under the
    // other platforms.
    wxCHECK_MSG( win, wxSize(0, 0), "Must have a valid window" );
#if wxOSX_USE_NSCELL_RENDERER
    ApplyMacControlFlags( win, GetCheckBoxCell(), flags);
    NSSize sz = GetCheckBoxCell().cellSize;
    return wxSize(sz.width, sz.height);
#else
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
#endif
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
#if wxOSX_USE_NSCELL_RENDERER
    NSControlStateValue stateValue = (flags & wxCONTROL_PRESSED) ? NSControlStateValueOn : NSControlStateValueOff;
    DrawMacCell(win, dc, GetPushButtonCell(), rect, flags, stateValue);
#else
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
#endif
}

void wxRendererMac::DrawCollapseButton(wxWindow *win,
                                wxDC& dc,
                                const wxRect& rect,
                                int flags)
{
    int adornment = (flags & wxCONTROL_EXPANDED)
        ? kThemeAdornmentArrowUpArrow
        : kThemeAdornmentArrowDownArrow;

    DrawMacThemeButton(win, dc, rect, flags,
                       kThemeArrowButton, adornment);
}

wxSize wxRendererMac::GetCollapseButtonSize(wxWindow *WXUNUSED(win), wxDC& WXUNUSED(dc))
{
    wxSize size;
    SInt32 width, height;
    OSStatus errStatus;

    errStatus = GetThemeMetric(kThemeMetricDisclosureButtonWidth, &width);
    if (errStatus == noErr)
    {
        size.SetWidth(width);
    }

    errStatus = GetThemeMetric(kThemeMetricDisclosureButtonHeight, &height);
    if (errStatus == noErr)
    {
        size.SetHeight(height);
    }

    // strict metrics size cutoff the button, increase the size
    size.IncBy(1);

    return size;
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
#if wxOSX_USE_NSCELL_RENDERER
    NSControlStateValue stateValue = (flags & wxCONTROL_PRESSED) ? NSControlStateValueOn : NSControlStateValueOff;
    DrawMacCell(win, dc, GetPopupbuttonCell(), rect, flags, stateValue);
#else
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
#endif
}


void wxRendererMac::DrawComboBox(wxWindow* win, wxDC& dc,
                             const wxRect& rect, int flags)
{
#if wxOSX_USE_NSCELL_RENDERER
    NSControlStateValue stateValue = (flags & wxCONTROL_PRESSED) ? NSControlStateValueOn : NSControlStateValueOff;
    DrawMacCell(win, dc, GetComboBoxCell(), rect, flags, stateValue);
#else
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
#endif
}

void wxRendererMac::DrawRadioBitmap(wxWindow* win, wxDC& dc,
                                const wxRect& rect, int flags)
{
    if (flags & wxCONTROL_CHECKED)
        flags |= wxCONTROL_PRESSED;
#if wxOSX_USE_NSCELL_RENDERER
    NSControlStateValue stateValue = (flags & wxCONTROL_PRESSED) ? NSControlStateValueOn : NSControlStateValueOff;

    if (flags & wxCONTROL_UNDETERMINED)
            stateValue = NSControlStateValueMixed;

    DrawMacCell(win, dc, GetRadioButtonCell(), rect, flags, stateValue);
#else
    int kind;

    if (win->GetWindowVariant() == wxWINDOW_VARIANT_SMALL ||
        (win->GetParent() && win->GetParent()->GetWindowVariant() == wxWINDOW_VARIANT_SMALL))
        kind = kThemeRadioButtonSmall;
    else if (win->GetWindowVariant() == wxWINDOW_VARIANT_MINI ||
             (win->GetParent() && win->GetParent()->GetWindowVariant() == wxWINDOW_VARIANT_MINI))
        kind = kThemeRadioButtonMini;
    else
        kind = kThemeRadioButton;

    DrawMacThemeButton(win, dc, rect, flags,
                          kind, kThemeAdornmentNone);
#endif
}

void wxRendererMac::DrawTextCtrl(wxWindow* win, wxDC& dc,
                             const wxRect& rect, int flags)
{
    const wxCoord x = rect.x;
    const wxCoord y = rect.y;
    const wxCoord w = rect.width;
    const wxCoord h = rect.height;

    wxDCBrushChanger setBrush(dc, *wxWHITE_BRUSH);
    wxDCPenChanger setPen(dc, *wxTRANSPARENT_PEN);
    dc.DrawRectangle(rect);

    // Note that calling SetBrush() here is fine as we already have
    // wxDCBrushChanger above, so the original brush will get restored.
    dc.SetBrush( *wxTRANSPARENT_BRUSH );

    HIRect hiRect = CGRectMake( x, y, w, h );
    if ( !wxHasCGContext(win, dc) )
    {
        win->RefreshRect(rect);
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

#ifdef wxHAS_DRAW_TITLE_BAR_BITMAP

void wxRendererMac::DrawTitleBarBitmap(wxWindow *win,
                                       wxDC& dc,
                                       const wxRect& rect,
                                       wxTitleBarButton button,
                                       int flags)
{
    // currently we only support the close bitmap here
    if ( button != wxTITLEBAR_BUTTON_CLOSE )
    {
        m_rendererNative.DrawTitleBarBitmap(win, dc, rect, button, flags);
        return;
    }

    wxColour glyphColor;

    // The following hard coded RGB values are based the close button in
    // XCode 6+ welcome screen
    bool drawCircle;
    wxColour circleBorderCol, circleInteriorCol;
    if ( flags & wxCONTROL_PRESSED )
    {
        drawCircle = true;
        glyphColor = wxColour(104, 104, 104);
        circleBorderCol = wxColour(70, 70, 71);
        circleInteriorCol = wxColour(78, 78, 78);
    }
    else if ( flags & wxCONTROL_CURRENT )
    {
        drawCircle = true;
        glyphColor = *wxWHITE;
        circleBorderCol = wxColour(163, 165, 166);
        circleInteriorCol = wxColour(182, 184, 187);
    }
    else
    {
        drawCircle = false;
        glyphColor = wxColour(145, 147, 149);
    }

    if ( drawCircle )
    {
        wxDCPenChanger setPen(dc, circleBorderCol);
        wxDCBrushChanger setBrush(dc, circleInteriorCol);

        wxRect circleRect(rect);
        circleRect.Deflate(2);

        dc.DrawEllipse(circleRect);
    }

    wxDCPenChanger setPen(dc, glyphColor);

    wxRect centerRect(rect);
    centerRect.Deflate(5);
    centerRect.height++;
    centerRect.width++;

    dc.DrawLine(centerRect.GetTopLeft(), centerRect.GetBottomRight());
    dc.DrawLine(centerRect.GetTopRight(), centerRect.GetBottomLeft());
}

#endif // wxHAS_DRAW_TITLE_BAR_BITMAP

void wxRendererMac::DrawGauge(wxWindow* WXUNUSED(win),
                              wxDC& dc,
                              const wxRect& rect,
                              int value,
                              int max,
                              int WXUNUSED(flags))
{
    const wxCoord x = rect.x;
    const wxCoord y = rect.y;
    const wxCoord w = rect.width;
    const wxCoord h = rect.height;

    HIThemeTrackDrawInfo tdi;
    tdi.version = 0;
    tdi.min = 0;
    tdi.value = value;
    tdi.max = max;
    tdi.bounds = CGRectMake(x, y, w, h);
    tdi.attributes = kThemeTrackHorizontal;
    tdi.enableState = kThemeTrackActive;
    tdi.kind = kThemeLargeProgressBar;

    int milliSecondsPerStep = 1000 / 60;
    wxLongLongNative localTime = wxGetLocalTimeMillis();
    tdi.trackInfo.progress.phase = localTime.GetValue() / milliSecondsPerStep % 32;

    CGContextRef cgContext;
    wxGCDCImpl *impl = (wxGCDCImpl*) dc.GetImpl();

    cgContext = (CGContextRef) impl->GetGraphicsContext()->GetNativeContext();

    HIThemeDrawTrack(&tdi, nullptr, cgContext, kHIThemeOrientationNormal);
}

#endif

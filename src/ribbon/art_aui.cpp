///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/art_aui.cpp
// Purpose:     AUI style art provider for ribbon interface
// Author:      Peter Cawley
// Created:     2009-08-04
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#if wxUSE_RIBBON

#include "wx/ribbon/art.h"
#include "wx/ribbon/art_internal.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/buttonbar.h"
#include "wx/ribbon/gallery.h"
#include "wx/ribbon/toolbar.h"

#ifndef WX_PRECOMP
    #include "wx/dc.h"
#endif

#ifdef __WXMSW__
    #include "wx/msw/private.h"
#elif defined(__WXOSX__)
    #include "wx/osx/private.h"
#endif

wxRibbonAUIArtProvider::wxRibbonAUIArtProvider()
    : wxRibbonMSWArtProvider(false)
{
#ifdef __WXOSX__
    wxColor baseColour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
#else
    wxColor baseColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
#endif

    SetColourScheme(baseColour,
        wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT),
        wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));

    m_tabActiveLabelFont = m_tabLabelFont;
    m_tabActiveLabelFont.SetWeight(wxFONTWEIGHT_BOLD);

    m_pageBorderLeft = 1;
    m_pageBorderRight = 1;
    m_pageBorderTop = 1;
    m_pageBorderBottom = 2;
    m_tabSeparationSize = 0;

    m_galleryBitmapPaddingLeftSize = 3;
    m_galleryBitmapPaddingRightSize = 3;
    m_galleryBitmapPaddingTopSize = 3;
    m_galleryBitmapPaddingBottomSize = 3;
}

wxRibbonAUIArtProvider::~wxRibbonAUIArtProvider()
{
}

wxRibbonArtProvider* wxRibbonAUIArtProvider::Clone() const
{
    wxRibbonAUIArtProvider *copy = new wxRibbonAUIArtProvider();
    CloneTo(copy);

    copy->m_tabCtrlBackgroundColour = m_tabCtrlBackgroundColour;
    copy->m_tabCtrlBackgroundGradientColour = m_tabCtrlBackgroundGradientColour;
    copy->m_panelLabelBackgroundColour = m_panelLabelBackgroundColour;
    copy->m_panelLabelBackgroundGradientColour = m_panelLabelBackgroundGradientColour;
    copy->m_panelHoverLabelBackgroundColour = m_panelHoverLabelBackgroundColour;
    copy->m_panelHoverLabelBackgroundGradientColour = m_panelHoverLabelBackgroundGradientColour;

    copy->m_backgroundBrush = m_backgroundBrush;
    copy->m_tabActiveTopBackgroundBrush = m_tabActiveTopBackgroundBrush;
    copy->m_tabHoverBackgroundBrush = m_tabHoverBackgroundBrush;
    copy->m_buttonBarHoverBackgroundBrush = m_buttonBarHoverBackgroundBrush;
    copy->m_buttonBarActiveBackgroundBrush = m_buttonBarActiveBackgroundBrush;
    copy->m_galleryButtonActiveBackgroundBrush = m_galleryButtonActiveBackgroundBrush;
    copy->m_galleryButtonHoverBackgroundBrush = m_galleryButtonHoverBackgroundBrush;
    copy->m_galleryButtonDisabledBackgroundBrush = m_galleryButtonDisabledBackgroundBrush;

    copy->m_tabHighlightTopColour = m_tabHighlightTopColour;
    copy->m_tabHighlightTopGradientColour = m_tabHighlightTopGradientColour;
    copy->m_tabHighlightColour = m_tabHighlightColour;
    copy->m_tabHighlightGradientColour = m_tabHighlightGradientColour;

    copy->m_toolbarHoverBordenPen = m_toolbarHoverBordenPen;
    copy->m_toolHoverBackgroundBrush = m_toolHoverBackgroundBrush;
    copy->m_toolActiveBackgroundBrush = m_toolActiveBackgroundBrush;

    return copy;
}

void wxRibbonAUIArtProvider::SetFont(int id, const wxFont& font)
{
    wxRibbonMSWArtProvider::SetFont(id, font);
    if ( id == wxRIBBON_ART_TAB_LABEL_FONT )
    {
        m_tabActiveLabelFont = m_tabLabelFont;
        m_tabActiveLabelFont.SetWeight(wxFONTWEIGHT_BOLD);
    }
}

wxColour wxRibbonAUIArtProvider::GetColour(int id) const
{
    switch ( id )
    {
    case wxRIBBON_ART_PAGE_BACKGROUND_COLOUR:
    case wxRIBBON_ART_PAGE_BACKGROUND_GRADIENT_COLOUR:
        return m_backgroundBrush.GetColour();
    case wxRIBBON_ART_TAB_CTRL_BACKGROUND_COLOUR:
        return m_tabCtrlBackgroundColour;
    case wxRIBBON_ART_TAB_CTRL_BACKGROUND_GRADIENT_COLOUR:
        return m_tabCtrlBackgroundGradientColour;
    case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_TOP_COLOUR:
    case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_TOP_GRADIENT_COLOUR:
        return m_tabActiveTopBackgroundBrush.GetColour();
    case wxRIBBON_ART_TAB_HOVER_BACKGROUND_COLOUR:
    case wxRIBBON_ART_TAB_HOVER_BACKGROUND_GRADIENT_COLOUR:
        return m_tabHoverBackgroundBrush.GetColour();
    case wxRIBBON_ART_PANEL_LABEL_BACKGROUND_COLOUR:
        return m_panelLabelBackgroundColour;
    case wxRIBBON_ART_PANEL_LABEL_BACKGROUND_GRADIENT_COLOUR:
        return m_panelLabelBackgroundGradientColour;
    case wxRIBBON_ART_PANEL_HOVER_LABEL_BACKGROUND_COLOUR:
        return m_panelHoverLabelBackgroundColour;
    case wxRIBBON_ART_PANEL_HOVER_LABEL_BACKGROUND_GRADIENT_COLOUR:
        return m_panelHoverLabelBackgroundGradientColour;
    case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_COLOUR:
    case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_GRADIENT_COLOUR:
        return m_buttonBarHoverBackgroundBrush.GetColour();
    case wxRIBBON_ART_GALLERY_BUTTON_HOVER_BACKGROUND_COLOUR:
    case wxRIBBON_ART_GALLERY_BUTTON_HOVER_BACKGROUND_GRADIENT_COLOUR:
        return m_galleryButtonHoverBackgroundBrush.GetColour();
    case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_BACKGROUND_COLOUR:
    case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_BACKGROUND_GRADIENT_COLOUR:
        return m_galleryButtonActiveBackgroundBrush.GetColour();
    case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_BACKGROUND_COLOUR:
    case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_BACKGROUND_GRADIENT_COLOUR:
        return m_galleryButtonDisabledBackgroundBrush.GetColour();
    case wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_TOP_COLOUR:
       return m_tabHighlightTopColour;
    case wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_GRADIENT_TOP_COLOUR:
       return m_tabHighlightTopGradientColour;
    case wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_COLOUR:
       return m_tabHighlightColour;
    case wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_GRADIENT_COLOUR:
       return m_tabHighlightGradientColour;
    default:
        return wxRibbonMSWArtProvider::GetColour(id);
    }
}

void wxRibbonAUIArtProvider::SetColour(int id, const wxColor& colour)
{
    switch ( id )
    {
    case wxRIBBON_ART_PAGE_BACKGROUND_COLOUR:
    case wxRIBBON_ART_PAGE_BACKGROUND_GRADIENT_COLOUR:
        m_backgroundBrush.SetColour(colour);
        break;
    case wxRIBBON_ART_TAB_CTRL_BACKGROUND_COLOUR:
        m_tabCtrlBackgroundColour = colour;
        break;
    case wxRIBBON_ART_TAB_CTRL_BACKGROUND_GRADIENT_COLOUR:
        m_tabCtrlBackgroundGradientColour = colour;
        break;
    case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_TOP_COLOUR:
    case wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_TOP_GRADIENT_COLOUR:
        m_tabActiveTopBackgroundBrush.SetColour(colour);
        break;
    case wxRIBBON_ART_TAB_HOVER_BACKGROUND_COLOUR:
    case wxRIBBON_ART_TAB_HOVER_BACKGROUND_GRADIENT_COLOUR:
        m_tabHoverBackgroundBrush.SetColour(colour);
        break;
    case wxRIBBON_ART_PANEL_LABEL_BACKGROUND_COLOUR:
        m_panelLabelBackgroundColour = colour;
        break;
    case wxRIBBON_ART_PANEL_LABEL_BACKGROUND_GRADIENT_COLOUR:
        m_panelLabelBackgroundGradientColour = colour;
        break;
    case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_COLOUR:
    case wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_GRADIENT_COLOUR:
        m_buttonBarHoverBackgroundBrush.SetColour(colour);
        break;
    case wxRIBBON_ART_GALLERY_BUTTON_HOVER_BACKGROUND_COLOUR:
    case wxRIBBON_ART_GALLERY_BUTTON_HOVER_BACKGROUND_GRADIENT_COLOUR:
        m_galleryButtonHoverBackgroundBrush.SetColour(colour);
        break;
    case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_BACKGROUND_COLOUR:
    case wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_BACKGROUND_GRADIENT_COLOUR:
        m_galleryButtonActiveBackgroundBrush.SetColour(colour);
        break;
    case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_BACKGROUND_COLOUR:
    case wxRIBBON_ART_GALLERY_BUTTON_DISABLED_BACKGROUND_GRADIENT_COLOUR:
        m_galleryButtonDisabledBackgroundBrush.SetColour(colour);
        break;
    case wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_TOP_COLOUR:
       m_tabHighlightTopColour = colour;
       break;
    case wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_GRADIENT_TOP_COLOUR:
       m_tabHighlightTopGradientColour = colour;
       break;
    case wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_COLOUR:
       m_tabHighlightColour = colour;
       break;
    case wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_GRADIENT_COLOUR:
       m_tabHighlightGradientColour = colour;
       break;
    default:
        wxRibbonMSWArtProvider::SetColour(id, colour);
        break;
    }
}

void wxRibbonAUIArtProvider::SetColourScheme(
                         const wxColour& primary,
                         const wxColour& secondary,
                         const wxColour& tertiary)
{
    wxRibbonHSLColour primaryHsl(primary);
    wxRibbonHSLColour secondaryHsl(secondary);
    wxRibbonHSLColour tertiaryHsl(tertiary);

    // Map primary & secondary luminance from [0, 1] to [0.15, 0.85]
    primaryHsl.m_luminance   = std::cos(primaryHsl.m_luminance   * float(M_PI)) * -0.35f + 0.5f;
    secondaryHsl.m_luminance = std::cos(secondaryHsl.m_luminance * float(M_PI)) * -0.35f + 0.5f;

    // TODO: Remove next line once this provider stops piggybacking MSW
    wxRibbonMSWArtProvider::SetColourScheme(primary, secondary, tertiary);

    const auto LikePrimary = [primaryHsl](double luminance)
        {
            return wxRibbonShiftLuminance(primaryHsl, luminance).ToRGB();
        };
    const auto LikeSecondary = [secondaryHsl](double luminance)
        {
            return wxRibbonShiftLuminance(secondaryHsl, luminance).ToRGB();
        };

    m_tabCtrlBackgroundColour = LikePrimary(0.9);
#ifdef __WXOSX__
    m_tabCtrlBackgroundGradientColour = m_tabCtrlBackgroundColour;
#else
    m_tabCtrlBackgroundGradientColour = LikePrimary(1.7);
#endif
    m_tabBorderPen = LikePrimary(0.75);
#ifdef __WXOSX__
    m_tabLabelColour = wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT);
#else
    m_tabLabelColour = wxSystemSettings::SelectLightDark(
                            LikePrimary(0.1),
                            wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
#endif
    m_tabActiveLabelColour = m_tabLabelColour;
    m_tabHoverLabelColour = m_tabLabelColour;
    m_tabHoverBackgroundTopColour = primaryHsl.ToRGB();
#ifdef __WXOSX__
    m_tabHoverBackgroundTopGradientColour = m_tabHoverBackgroundTopColour;
#else
    m_tabHoverBackgroundTopGradientColour = LikePrimary(1.6);
#endif
    m_tabHoverBackgroundBrush = m_tabHoverBackgroundTopColour;
    m_tabActiveBackgroundColour = m_tabCtrlBackgroundGradientColour;
#ifdef __WXOSX__
    m_tabActiveBackgroundGradientColour = m_tabActiveBackgroundColour;
#else
    m_tabActiveBackgroundGradientColour = primaryHsl.ToRGB();
#endif
    m_tabActiveTopBackgroundBrush = m_tabActiveBackgroundColour;
    m_panelLabelColour = m_tabLabelColour;
    m_panelMinimisedLabelColour = m_panelLabelColour;
    m_panelHoverLabelColour = tertiaryHsl.ToRGB();
    m_pageBorderPen = m_tabBorderPen;
    m_panelBorderPen = m_tabBorderPen;
    m_backgroundBrush = primaryHsl.ToRGB();
    m_pageHoverBackgroundColour = LikePrimary(1.5);
    m_pageHoverBackgroundGradientColour = LikePrimary(0.9);
    m_panelLabelBackgroundColour = LikePrimary(0.85);
    m_panelLabelBackgroundGradientColour = LikePrimary(0.97);
    m_panelHoverLabelBackgroundGradientColour = secondaryHsl.ToRGB();
    m_panelHoverLabelBackgroundColour = secondaryHsl.AdjustLuminance(0.2f).ToRGB();
    m_buttonBarHoverBorderPen = secondaryHsl.ToRGB();
    m_buttonBarHoverBackgroundBrush = LikeSecondary(1.7);
    m_buttonBarActiveBackgroundBrush = LikeSecondary(1.4);
    m_buttonBarLabelColour = m_tabLabelColour;
#ifdef __WXOSX__
    m_buttonBarLabelDisabledColour = wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTIONTEXT);
#else
    m_buttonBarLabelDisabledColour = m_tabLabelColour;
#endif
    m_galleryBorderPen = m_tabBorderPen;
    m_galleryItemBorderPen = m_buttonBarHoverBorderPen;
    m_galleryHoverBackgroundBrush = LikePrimary(1.2);
    m_galleryButtonBackgroundColour = m_pageHoverBackgroundColour;
    m_galleryButtonBackgroundGradientColour = m_pageHoverBackgroundGradientColour;
    m_galleryButtonHoverBackgroundBrush = m_buttonBarHoverBackgroundBrush;
    m_galleryButtonActiveBackgroundBrush = m_buttonBarActiveBackgroundBrush;
    m_galleryButtonDisabledBackgroundBrush = primaryHsl.Desaturated(0.15f).ToRGB();
    SetColour(wxRIBBON_ART_GALLERY_BUTTON_FACE_COLOUR, LikePrimary(0.1));
    SetColour(wxRIBBON_ART_GALLERY_BUTTON_DISABLED_FACE_COLOUR, wxColour(128, 128, 128));
    SetColour(wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_FACE_COLOUR, LikeSecondary(0.1));
    SetColour(wxRIBBON_ART_GALLERY_BUTTON_HOVER_FACE_COLOUR, LikeSecondary(0.1));
    m_toolbarBorderPen = m_tabBorderPen;
    SetColour(wxRIBBON_ART_TOOLBAR_FACE_COLOUR, LikePrimary(0.1));
    m_toolBackgroundColour = m_pageHoverBackgroundColour;
    m_toolBackgroundGradientColour = m_pageHoverBackgroundGradientColour;
    m_toolbarHoverBordenPen = m_buttonBarHoverBorderPen;
    m_toolHoverBackgroundBrush = m_buttonBarHoverBackgroundBrush;
    m_toolActiveBackgroundBrush = m_buttonBarActiveBackgroundBrush;

    // For highlight pages
    wxColour topColour1((m_tabActiveBackgroundColour.Red()   + m_tabHoverBackgroundTopColour.Red())/2,
                         (m_tabActiveBackgroundColour.Green() + m_tabHoverBackgroundTopColour.Green())/2,
                         (m_tabActiveBackgroundColour.Blue()  + m_tabHoverBackgroundTopColour.Blue())/2);

    wxColour bottomColour1((m_tabActiveBackgroundGradientColour.Red()   + m_tabHoverBackgroundTopGradientColour.Red())/2,
                            (m_tabActiveBackgroundGradientColour.Green() + m_tabHoverBackgroundTopGradientColour.Green())/2,
                            (m_tabActiveBackgroundGradientColour.Blue()  + m_tabHoverBackgroundTopGradientColour.Blue())/2);

    m_tabHighlightTopColour = topColour1;
    m_tabHighlightTopGradientColour = bottomColour1;

    m_tabHighlightColour = topColour1;
    m_tabHighlightGradientColour = bottomColour1;
}

void wxRibbonAUIArtProvider::DrawTabCtrlBackground(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect)
{
    wxRect gradientRect(rect);
    gradientRect.height--;
    dc.GradientFillLinear(gradientRect, m_tabCtrlBackgroundColour,
        m_tabCtrlBackgroundGradientColour, wxSOUTH);
    dc.SetPen(m_tabBorderPen);
    dc.DrawLine(rect.x, rect.GetBottom(), rect.GetRight()+1, rect.GetBottom());
}

int wxRibbonAUIArtProvider::GetTabCtrlHeight(
                        wxReadOnlyDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRibbonPageTabInfoArray& pages)
{
    int textHeight = 0;
    int iconHeight = 0;

    if ( pages.GetCount() <= 1 && (m_flags & wxRIBBON_BAR_ALWAYS_SHOW_TABS) == 0 )
    {
        // To preserve space, a single tab need not be displayed. We still need
        // one pixel of border though.
        return 1;
    }

    if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS )
    {
        dc.SetFont(m_tabActiveLabelFont);
        textHeight = dc.GetTextExtent("ABCDEFXj").GetHeight();
    }
    if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS )
    {
        size_t numPages = pages.GetCount();
        for ( size_t i = 0; i < numPages; ++i )
        {
            const wxRibbonPageTabInfo& info = pages.Item(i);
            if ( info.m_page->GetIcon().IsOk() )
            {
                iconHeight = wxMax(iconHeight, info.m_page->GetIcon().GetLogicalHeight());
            }
        }
    }

    return wxMax(textHeight, iconHeight) + 10;
}

void wxRibbonAUIArtProvider::DrawTab(wxDC& dc,
                 wxWindow* WXUNUSED(wnd),
                 const wxRibbonPageTabInfo& tab)
{
    if ( tab.m_rect.height <= 1 )
        return;

    dc.SetFont(m_tabLabelFont);
    dc.SetPen(*wxTRANSPARENT_PEN);
    if ( tab.m_active || tab.m_hovered || tab.m_highlight )
    {
        if ( tab.m_active )
        {
            dc.SetFont(m_tabActiveLabelFont);
            dc.SetBrush(m_backgroundBrush);
            dc.DrawRectangle(tab.m_rect.x, tab.m_rect.y + tab.m_rect.height - 1,
                tab.m_rect.width - 1, 1);
        }
        wxRect gradRect(tab.m_rect);
        gradRect.height -= 4;
        gradRect.width -= 1;
        gradRect.height /= 2;
        gradRect.y = gradRect.y + tab.m_rect.height - gradRect.height - 1;
        dc.SetBrush(m_tabActiveTopBackgroundBrush);
        dc.DrawRectangle(tab.m_rect.x, tab.m_rect.y + 3, tab.m_rect.width - 1,
            gradRect.y - tab.m_rect.y - 3);
        if ( tab.m_highlight )
        {
            dc.GradientFillLinear(gradRect, m_tabHighlightTopColour, m_tabHighlightTopGradientColour, wxSOUTH);
        }
        else
        {
            dc.GradientFillLinear(gradRect, m_tabActiveBackgroundColour,
                m_tabActiveBackgroundGradientColour, wxSOUTH);
        }
    }
    else
    {
        wxRect btmRect(tab.m_rect);
        btmRect.height -= 4;
        btmRect.width -= 1;
        btmRect.height /= 2;
        btmRect.y = btmRect.y + tab.m_rect.height - btmRect.height - 1;
        dc.SetBrush(m_tabHoverBackgroundBrush);
        dc.DrawRectangle(btmRect.x, btmRect.y, btmRect.width,
            btmRect.height);
        wxRect gradRect(tab.m_rect);
        gradRect.width -= 1;
        gradRect.y += 3;
        gradRect.height = btmRect.y - gradRect.y;
        dc.GradientFillLinear(gradRect, m_tabHoverBackgroundTopColour,
            m_tabHoverBackgroundTopGradientColour, wxSOUTH);
    }

    wxPoint borderPoints[5];
    borderPoints[0] = wxPoint(0, 3);
    borderPoints[1] = wxPoint(1, 2);
    borderPoints[2] = wxPoint(tab.m_rect.width - 3, 2);
    borderPoints[3] = wxPoint(tab.m_rect.width - 1, 4);
    borderPoints[4] = wxPoint(tab.m_rect.width - 1, tab.m_rect.height - 1);

    dc.SetPen(m_tabBorderPen);
    dc.DrawLines(sizeof(borderPoints)/sizeof(wxPoint), borderPoints, tab.m_rect.x, tab.m_rect.y);

    wxRect oldClip;
    dc.GetClippingBox(oldClip);
    bool isFirstTab = false;
    wxRibbonBar* bar = wxDynamicCast(tab.m_page->GetParent(), wxRibbonBar);
    if ( bar && bar->GetPage(0) == tab.m_page )
        isFirstTab = true;

    wxBitmap icon;
    if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS )
    {
        icon = tab.m_page->GetIcon();
        if ( (m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS) == 0 )
        {
            if ( icon.IsOk() )
            {
            int x = tab.m_rect.x + (tab.m_rect.width - icon.GetLogicalWidth()) / 2;
            dc.DrawBitmap(icon, x, tab.m_rect.y + 1 + (tab.m_rect.height - 1 -
                icon.GetLogicalHeight()) / 2, true);
            }
        }
    }
    if ( m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS )
    {
        wxString label = tab.m_page->GetLabel();
        if ( !label.empty() )
        {
            if ( tab.m_active )
            {
                dc.SetTextForeground(m_tabActiveLabelColour);
            }
            else if ( tab.m_hovered )
            {
                dc.SetTextForeground(m_tabHoverLabelColour);
            }
            else
            {
                dc.SetTextForeground(m_tabLabelColour);
            }

            dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

            int offset = 0;
            if ( icon.IsOk() )
                offset += icon.GetLogicalWidth() + 2;
            int textHeight;
            int textWidth;
            dc.GetTextExtent(label, &textWidth, &textHeight);
            int x = (tab.m_rect.width - 2 - textWidth - offset) / 2;
            if ( x > 8 )
                x = 8;
            else if ( x < 1 )
                x = 1;
            int width = tab.m_rect.width - x - 2;
            x += tab.m_rect.x + offset;
            int y = tab.m_rect.y + (tab.m_rect.height - textHeight) / 2;
            if ( icon.IsOk() )
            {
                dc.DrawBitmap(icon, x - offset, tab.m_rect.y + (tab.m_rect.height -
                    icon.GetLogicalHeight()) / 2, true);
            }
            {
                wxDCClipper clip(dc, x, tab.m_rect.y, width, tab.m_rect.height);
                dc.DrawText(label, x, y);
            }
        }
    }

    // Draw the left hand edge of the tab only for the first tab (subsequent
    // tabs use the right edge of the prior tab as their left edge). As this is
    // outside the rectangle for the tab, only draw it if the leftmost part of
    // the tab is within the clip rectangle (the clip region has to be cleared
    // to draw outside the tab).
    if ( isFirstTab && oldClip.x <= tab.m_rect.x
        && tab.m_rect.x < oldClip.x + oldClip.width )
    {
        dc.DestroyClippingRegion();
        dc.DrawLine(tab.m_rect.x - 1, tab.m_rect.y + 4, tab.m_rect.x - 1,
            tab.m_rect.y + tab.m_rect.height - 1);
    }
}

void wxRibbonAUIArtProvider::GetBarTabWidth(
                        wxReadOnlyDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxString& label,
                        const wxBitmap& bitmap,
                        int* ideal,
                        int* smallBeginNeedSeparator,
                        int* smallMustHaveSeparator,
                        int* minimum)
{
    int width = 0;
    int min = 0;
    if ( (m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS) && !label.empty() )
    {
        dc.SetFont(m_tabActiveLabelFont);
        width += dc.GetTextExtent(label).GetWidth();
        min += wxMin(30, width); // enough for a few chars
        if ( bitmap.IsOk() )
        {
            // gap between label and bitmap
            width += 4;
            min += 2;
        }
    }
    if ( (m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS) && bitmap.IsOk() )
    {
        width += bitmap.GetLogicalWidth();
        min += bitmap.GetLogicalWidth();
    }

    if ( ideal != nullptr )
    {
        *ideal = width + 16;
    }
    if ( smallBeginNeedSeparator != nullptr )
    {
        *smallBeginNeedSeparator = min;
    }
    if ( smallMustHaveSeparator != nullptr )
    {
        *smallMustHaveSeparator = min;
    }
    if ( minimum != nullptr )
    {
        *minimum = min;
    }
}

void wxRibbonAUIArtProvider::DrawTabSeparator(
                    wxDC& WXUNUSED(dc),
                    wxWindow* WXUNUSED(wnd),
                    const wxRect& WXUNUSED(rect),
                    double WXUNUSED(visibility))
{
    // No explicit separators between tabs
}

void wxRibbonAUIArtProvider::DrawPageBackground(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_backgroundBrush);
    dc.DrawRectangle(rect.x + 1, rect.y, rect.width - 2, rect.height - 1);

    dc.SetPen(m_pageBorderPen);
    dc.DrawLine(rect.x, rect.y, rect.x, rect.y + rect.height);
    dc.DrawLine(rect.GetRight(), rect.y, rect.GetRight(), rect.y + rect.height);
    dc.DrawLine(rect.x, rect.GetBottom(), rect.GetRight()+1, rect.GetBottom());
}

wxSize wxRibbonAUIArtProvider::GetScrollButtonMinimumSize(
                        wxReadOnlyDC& WXUNUSED(dc),
                        wxWindow* WXUNUSED(wnd),
                        long WXUNUSED(style))
{
    return wxSize(11, 11);
}

void wxRibbonAUIArtProvider::DrawScrollButton(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect,
                        long style)
{
    wxRect trueRect(rect);
    wxPoint arrowPoints[3];

    if ( (style & wxRIBBON_SCROLL_BTN_FOR_MASK) == wxRIBBON_SCROLL_BTN_FOR_TABS )
    {
        trueRect.y += 2;
        trueRect.height -= 2;
        dc.SetPen(m_tabBorderPen);
    }
    else
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(m_backgroundBrush);
        dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
        dc.SetPen(m_pageBorderPen);
    }

    switch ( style & wxRIBBON_SCROLL_BTN_DIRECTION_MASK )
    {
    case wxRIBBON_SCROLL_BTN_LEFT:
        dc.DrawLine(trueRect.GetRight(), trueRect.y, trueRect.GetRight(),
            trueRect.y + trueRect.height);
        arrowPoints[0] = wxPoint(rect.width / 2 - 2, rect.height / 2);
        arrowPoints[1] = arrowPoints[0] + wxPoint(5, -5);
        arrowPoints[2] = arrowPoints[0] + wxPoint(5,  5);
        break;
    case wxRIBBON_SCROLL_BTN_RIGHT:
        dc.DrawLine(trueRect.x, trueRect.y, trueRect.x,
            trueRect.y + trueRect.height);
        arrowPoints[0] = wxPoint(rect.width / 2 + 3, rect.height / 2);
        arrowPoints[1] = arrowPoints[0] - wxPoint(5, -5);
        arrowPoints[2] = arrowPoints[0] - wxPoint(5,  5);
        break;
    case wxRIBBON_SCROLL_BTN_DOWN:
        dc.DrawLine(trueRect.x, trueRect.y, trueRect.x + trueRect.width,
            trueRect.y);
        arrowPoints[0] = wxPoint(rect.width / 2, rect.height / 2 + 3);
        arrowPoints[1] = arrowPoints[0] - wxPoint( 5, 5);
        arrowPoints[2] = arrowPoints[0] - wxPoint(-5, 5);
        break;
    case wxRIBBON_SCROLL_BTN_UP:
        dc.DrawLine(trueRect.x, trueRect.GetBottom(),
            trueRect.x + trueRect.width, trueRect.GetBottom());
        arrowPoints[0] = wxPoint(rect.width / 2, rect.height / 2 - 2);
        arrowPoints[1] = arrowPoints[0] + wxPoint( 5, 5);
        arrowPoints[2] = arrowPoints[0] + wxPoint(-5, 5);
        break;
    default:
        return;
    }

    int x = rect.x;
    int y = rect.y;
    if ( style & wxRIBBON_SCROLL_BTN_ACTIVE )
    {
        ++x;
        ++y;
    }

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(m_tabLabelColour));
    dc.DrawPolygon(WXSIZEOF(arrowPoints), arrowPoints, x, y);
}

wxSize wxRibbonAUIArtProvider::GetPanelSize(
                        wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize clientSize,
                        wxPoint* clientOffset)
{
    dc.SetFont(m_panelLabelFont);
    wxSize labelSize = dc.GetTextExtent(wnd->GetLabel());
    int labelHeight = labelSize.GetHeight() + 5;
    if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
    {
        clientSize.IncBy(4, labelHeight + 6);
        if ( clientOffset )
            *clientOffset = wxPoint(2, labelHeight + 3);
    }
    else
    {
        clientSize.IncBy(6, labelHeight + 4);
        if ( clientOffset )
            *clientOffset = wxPoint(3, labelHeight + 2);
    }
    return clientSize;
}

wxSize wxRibbonAUIArtProvider::GetPanelClientSize(
                        wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize size,
                        wxPoint* clientOffset)
{
    dc.SetFont(m_panelLabelFont);
    wxSize labelSize = dc.GetTextExtent(wnd->GetLabel());
    int labelHeight = labelSize.GetHeight() + 5;
    if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
    {
        size.DecBy(4, labelHeight + 6);
        if ( clientOffset )
            *clientOffset = wxPoint(2, labelHeight + 3);
    }
    else
    {
        size.DecBy(6, labelHeight + 4);
        if ( clientOffset )
            *clientOffset = wxPoint(3, labelHeight + 2);
    }
    if ( size.x < 0 )
        size.x = 0;
    if ( size.y < 0 )
        size.y = 0;
    return size;
}

wxRect wxRibbonAUIArtProvider::GetPanelExtButtonArea(wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxRect rect)
{
    wxRect trueRect(rect);
    RemovePanelPadding(&trueRect);

    trueRect.x++;
    trueRect.width -= 2;
    trueRect.y++;

    dc.SetFont(m_panelLabelFont);
    wxSize labelSize = dc.GetTextExtent(wnd->GetLabel());
    int labelHeight = labelSize.GetHeight() + 5;
    wxRect labelRect(trueRect);
    labelRect.height = labelHeight - 1;

    rect = wxRect(labelRect.GetRight()-13, labelRect.GetBottom()-13, 13, 13);
    return rect;
}

void wxRibbonAUIArtProvider::DrawPanelBackground(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_backgroundBrush);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

    wxRect trueRect(rect);
    RemovePanelPadding(&trueRect);

    dc.SetPen(m_panelBorderPen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(trueRect.x, trueRect.y, trueRect.width, trueRect.height);

    trueRect.x++;
    trueRect.width -= 2;
    trueRect.y++;

    dc.SetFont(m_panelLabelFont);
    wxSize labelSize = dc.GetTextExtent(wnd->GetLabel());
    int labelHeight = labelSize.GetHeight() + 5;
    wxRect labelRect(trueRect);
    labelRect.height = labelHeight - 1;
    dc.DrawLine(labelRect.x, labelRect.y + labelRect.height,
        labelRect.x + labelRect.width, labelRect.y + labelRect.height);

    wxColour labelBgColour = m_panelLabelBackgroundColour;
    wxColour labelBgGradColour = m_panelLabelBackgroundGradientColour;
    if ( wnd->IsHovered() )
    {
        labelBgColour = m_panelHoverLabelBackgroundColour;
        labelBgGradColour = m_panelHoverLabelBackgroundGradientColour;
        dc.SetTextForeground(m_panelHoverLabelColour);
    }
    else
    {
        dc.SetTextForeground(m_panelLabelColour);
    }
    dc.GradientFillLinear(labelRect,
#ifdef __WXOSX__
        labelBgGradColour, labelBgColour, wxSOUTH);
#else
        labelBgColour, labelBgGradColour, wxSOUTH);
#endif
    dc.SetFont(m_panelLabelFont);
    dc.DrawText(wnd->GetLabel(), labelRect.x + 3, labelRect.y + 2);

    if ( wnd->IsHovered() )
    {
        wxRect gradientRect(trueRect);
        gradientRect.y += labelRect.height + 1;
        gradientRect.height = trueRect.height - labelRect.height - 3;
#ifdef __WXOSX__
        wxColour colour = m_pageHoverBackgroundGradientColour;
        wxColour gradient = m_pageHoverBackgroundColour;
#else
        wxColour colour = m_pageHoverBackgroundColour;
        wxColour gradient = m_pageHoverBackgroundGradientColour;
#endif
        dc.GradientFillLinear(gradientRect, colour, gradient, wxSOUTH);
    }

    if ( wnd->HasExtButton() )
    {
        if ( wnd->IsExtButtonHovered() )
        {
            dc.SetPen(m_panelHoverButtonBorderPen);
            dc.SetBrush(m_panelHoverButtonBackgroundBrush);
            dc.DrawRoundedRectangle(labelRect.GetRight() - 13, labelRect.GetBottom() - 13, 13, 13, 1.0);
            dc.DrawBitmap(m_panelExtensionBundle[1].GetBitmapFor(wnd), labelRect.GetRight() - 10, labelRect.GetBottom() - 10, true);
        }
        else
            dc.DrawBitmap(m_panelExtensionBundle[0].GetBitmapFor(wnd), labelRect.GetRight() - 10, labelRect.GetBottom() - 10, true);
    }
}

void wxRibbonAUIArtProvider::DrawMinimisedPanel(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect,
                        wxBitmap& bitmap)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_backgroundBrush);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

    wxRect trueRect(rect);
    RemovePanelPadding(&trueRect);

    dc.SetPen(m_panelBorderPen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(trueRect.x, trueRect.y, trueRect.width, trueRect.height);
    trueRect.Deflate(1);

    if ( wnd->IsHovered() || wnd->GetExpandedPanel() )
    {
        wxColour colour = m_pageHoverBackgroundColour;
        wxColour gradient = m_pageHoverBackgroundGradientColour;
#ifdef __WXOSX__
        if ( !wnd->GetExpandedPanel() )
#else
        if ( wnd->GetExpandedPanel() )
#endif
        {
            wxColour temp = colour;
            colour = gradient;
            gradient = temp;
        }
        dc.GradientFillLinear(trueRect, colour, gradient, wxSOUTH);
    }

    wxRect preview;
    DrawMinimisedPanelCommon(dc, wnd, trueRect, &preview);

    dc.SetPen(m_panelBorderPen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(preview.x, preview.y, preview.width, preview.height);
    preview.Deflate(1);
    wxRect previewCaptionRect(preview);
    previewCaptionRect.height = 7;
    preview.y += previewCaptionRect.height;
    preview.height -= previewCaptionRect.height;
#ifdef __WXOSX__
    dc.GradientFillLinear(previewCaptionRect,
        m_panelHoverLabelBackgroundGradientColour,
        m_panelHoverLabelBackgroundColour, wxSOUTH);
    dc.GradientFillLinear(preview,
        m_pageHoverBackgroundGradientColour,
        m_pageHoverBackgroundColour, wxSOUTH);
#else
    dc.GradientFillLinear(previewCaptionRect,
        m_panelHoverLabelBackgroundColour,
        m_panelHoverLabelBackgroundGradientColour, wxSOUTH);
    dc.GradientFillLinear(preview,
        m_pageHoverBackgroundColour,
        m_pageHoverBackgroundGradientColour, wxSOUTH);
#endif

    if ( bitmap.IsOk() )
    {
        dc.DrawBitmap(bitmap, preview.x + (preview.width - bitmap.GetLogicalWidth()) / 2,
            preview.y + (preview.height - bitmap.GetLogicalHeight()) / 2, true);
    }
}

void wxRibbonAUIArtProvider::DrawPartialPanelBackground(wxDC& dc,
        wxWindow* wnd, const wxRect& rect)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_backgroundBrush);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

    wxPoint offset(wnd->GetPosition());
    wxWindow* parent = wnd->GetParent();
    wxRibbonPanel* panel = nullptr;

    for ( ; parent; parent = parent->GetParent() )
    {
        panel = wxDynamicCast(parent, wxRibbonPanel);
        if ( panel != nullptr )
        {
            if ( !panel->IsHovered() )
                return;
            break;
        }
        offset += parent->GetPosition();
    }
    if ( panel == nullptr )
        return;

    wxRect background(panel->GetSize());
    RemovePanelPadding(&background);
    background.x++;
    background.width -= 2;
    dc.SetFont(m_panelLabelFont);
    int captionHeight = dc.GetTextExtent(panel->GetLabel()).GetHeight() + 7;
    background.y += captionHeight - 1;
    background.height -= captionHeight;

    wxRect paintRect(rect);
    paintRect.x += offset.x;
    paintRect.y += offset.y;

    wxColour bgClr, bgGradClr;
#ifdef __WXOSX__
    bgGradClr = m_pageHoverBackgroundColour;
    bgClr = m_pageHoverBackgroundGradientColour;
#else
    bgClr = m_pageHoverBackgroundColour;
    bgGradClr = m_pageHoverBackgroundGradientColour;
#endif

    paintRect.Intersect(background);
    if ( !paintRect.IsEmpty() )
    {
        wxColour startingColour(wxRibbonInterpolateColour(bgClr, bgGradClr,
            paintRect.y, background.y, background.y + background.height));
        wxColour endingColour(wxRibbonInterpolateColour(bgClr, bgGradClr,
            paintRect.y + paintRect.height, background.y,
            background.y + background.height));
        paintRect.x -= offset.x;
        paintRect.y -= offset.y;
        dc.GradientFillLinear(paintRect, startingColour, endingColour, wxSOUTH);
    }
}

void wxRibbonAUIArtProvider::DrawGalleryBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect)
{
    DrawPartialPanelBackground(dc, wnd, rect);

    if ( wnd->IsHovered() )
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(m_galleryHoverBackgroundBrush);
        if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
        {
            dc.DrawRectangle(rect.x + 1, rect.y + 1, rect.width - 2,
                rect.height - 16);
        }
        else
        {
            dc.DrawRectangle(rect.x + 1, rect.y + 1, rect.width - 16,
                rect.height - 2);
        }
    }

    dc.SetPen(m_galleryBorderPen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

    DrawGalleryBackgroundCommon(dc, wnd, rect);
}

void wxRibbonAUIArtProvider::DrawGalleryButton(wxDC& dc, wxRect rect,
        wxRibbonGalleryButtonState state, wxBitmapBundle* bundles, wxWindow* wnd)
{
    int extraHeight = 0;
    int extraWidth = 0;
    wxRect reducedRect(rect);
    reducedRect.Deflate(1);
    if ( m_flags & wxRIBBON_BAR_FLOW_VERTICAL )
    {
        reducedRect.width++;
        extraWidth = 1;
    }
    else
    {
        reducedRect.height++;
        extraHeight = 1;
    }

    wxBitmap btnBitmap;
    switch ( state )
    {
    case wxRIBBON_GALLERY_BUTTON_NORMAL:
        dc.GradientFillLinear(reducedRect,
            m_galleryButtonBackgroundColour,
            m_galleryButtonBackgroundGradientColour, wxSOUTH);
        btnBitmap = bundles[0].GetBitmapFor(wnd);
        break;
    case wxRIBBON_GALLERY_BUTTON_HOVERED:
        dc.SetPen(m_galleryItemBorderPen);
        dc.SetBrush(m_galleryButtonHoverBackgroundBrush);
        dc.DrawRectangle(rect.x, rect.y, rect.width + extraWidth,
            rect.height + extraHeight);
        btnBitmap = bundles[1].GetBitmapFor(wnd);
        break;
    case wxRIBBON_GALLERY_BUTTON_ACTIVE:
        dc.SetPen(m_galleryItemBorderPen);
        dc.SetBrush(m_galleryButtonActiveBackgroundBrush);
        dc.DrawRectangle(rect.x, rect.y, rect.width + extraWidth,
            rect.height + extraHeight);
        btnBitmap = bundles[2].GetBitmapFor(wnd);
        break;
    case wxRIBBON_GALLERY_BUTTON_DISABLED:
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(m_galleryButtonDisabledBackgroundBrush);
        dc.DrawRectangle(reducedRect.x, reducedRect.y, reducedRect.width,
            reducedRect.height);
        btnBitmap = bundles[3].GetBitmapFor(wnd);
        break;
    }

    dc.DrawBitmap(btnBitmap, reducedRect.x + reducedRect.width / 2 - 2,
        (rect.y + rect.height / 2) - 2, true);
}

void wxRibbonAUIArtProvider::DrawGalleryItemBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect,
                        wxRibbonGalleryItem* item)
{
    if ( wnd->GetHoveredItem() != item && wnd->GetActiveItem() != item &&
         wnd->GetSelection() != item )
        return;

    dc.SetPen(m_galleryItemBorderPen);
    if ( wnd->GetActiveItem() == item || wnd->GetSelection() == item )
        dc.SetBrush(m_galleryButtonActiveBackgroundBrush);
    else
        dc.SetBrush(m_galleryButtonHoverBackgroundBrush);

    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
}

void wxRibbonAUIArtProvider::DrawButtonBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect)
{
    DrawPartialPanelBackground(dc, wnd, rect);
}

void wxRibbonAUIArtProvider::DrawButtonBarButton(
                        wxDC& dc,
                        wxWindow* WXUNUSED(wnd),
                        const wxRect& rect,
                        wxRibbonButtonKind kind,
                        long state,
                        const wxString& label,
                        const wxBitmap& bitmapLarge,
                        const wxBitmap& bitmapSmall)
{
    if ( kind == wxRIBBON_BUTTON_TOGGLE )
    {
        kind = wxRIBBON_BUTTON_NORMAL;
        if ( state & wxRIBBON_BUTTONBAR_BUTTON_TOGGLED )
            state ^= wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK;
    }

    if ( state &
         (wxRIBBON_BUTTONBAR_BUTTON_HOVER_MASK | wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK) )
    {
        dc.SetPen(m_buttonBarHoverBorderPen);

        wxRect bgRect(rect);
        bgRect.Deflate(1);

        if ( kind == wxRIBBON_BUTTON_HYBRID )
        {
            switch ( state & wxRIBBON_BUTTONBAR_BUTTON_SIZE_MASK )
            {
            case wxRIBBON_BUTTONBAR_BUTTON_LARGE:
                {
                    int iYBorder = rect.y + bitmapLarge.GetLogicalHeight() + 4;
                    wxRect partialBg(rect);
                    if ( state & wxRIBBON_BUTTONBAR_BUTTON_NORMAL_HOVERED )
                    {
                        partialBg.SetBottom(iYBorder - 1);
                    }
                    else
                    {
                        partialBg.height -= (iYBorder - partialBg.y + 1);
                        partialBg.y = iYBorder + 1;
                    }
                    dc.DrawLine(rect.x, iYBorder, rect.x + rect.width, iYBorder);
                    bgRect.Intersect(partialBg);
                }
                break;
            case wxRIBBON_BUTTONBAR_BUTTON_MEDIUM:
            case wxRIBBON_BUTTONBAR_BUTTON_SMALL:
                {
                    int iArrowWidth = 9;
                    if ( state & wxRIBBON_BUTTONBAR_BUTTON_NORMAL_HOVERED )
                    {
                        bgRect.width -= iArrowWidth;
                        dc.DrawLine(bgRect.x + bgRect.width,
                            rect.y, bgRect.x + bgRect.width,
                            rect.y + rect.height);
                    }
                    else
                    {
                        --iArrowWidth;
                        bgRect.x += bgRect.width - iArrowWidth;
                        bgRect.width = iArrowWidth;
                        dc.DrawLine(bgRect.x - 1, rect.y,
                            bgRect.x - 1, rect.y + rect.height);
                    }
                }
                break;
            }
        }

        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);

        dc.SetPen(*wxTRANSPARENT_PEN);
        if ( state & wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK )
            dc.SetBrush(m_buttonBarActiveBackgroundBrush);
        else
            dc.SetBrush(m_buttonBarHoverBackgroundBrush);
        dc.DrawRectangle(bgRect.x, bgRect.y, bgRect.width, bgRect.height);
    }

    dc.SetFont(m_buttonBarLabelFont);
    dc.SetTextForeground(state & wxRIBBON_BUTTONBAR_BUTTON_DISABLED
                            ? m_buttonBarLabelDisabledColour
                            : m_buttonBarLabelColour);
    DrawButtonBarButtonForeground(dc, rect, kind, state, label, bitmapLarge,
        bitmapSmall);
}

void wxRibbonAUIArtProvider::DrawToolBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect)
{
    DrawPartialPanelBackground(dc, wnd, rect);
}

void wxRibbonAUIArtProvider::DrawToolGroupBackground(
                    wxDC& dc,
                    wxWindow* WXUNUSED(wnd),
                    const wxRect& rect)
{
    dc.SetPen(m_toolbarBorderPen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height);
    wxRect bgRect(rect);
    bgRect.Deflate(1);
    dc.GradientFillLinear(bgRect, m_toolBackgroundColour,
        m_toolBackgroundGradientColour, wxSOUTH);
}

void wxRibbonAUIArtProvider::DrawTool(
            wxDC& dc,
            wxWindow* wnd,
            const wxRect& rect,
            const wxBitmap& bitmap,
            wxRibbonButtonKind kind,
            long state)
{
    if ( kind == wxRIBBON_BUTTON_TOGGLE )
    {
        if ( state & wxRIBBON_TOOLBAR_TOOL_TOGGLED )
            state ^= wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK;
    }

    wxRect bgRect(rect);
    bgRect.Deflate(1);
    if ( (state & wxRIBBON_TOOLBAR_TOOL_LAST) == 0 )
        bgRect.width++;
    bool isCustomBg = (state & (wxRIBBON_TOOLBAR_TOOL_HOVER_MASK |
        wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK)) != 0;
    bool isSplitHybrid = kind == wxRIBBON_BUTTON_HYBRID && isCustomBg;

    // Background
    if ( isCustomBg )
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(m_toolHoverBackgroundBrush);
        dc.DrawRectangle(bgRect.x, bgRect.y, bgRect.width, bgRect.height);
        if ( state & wxRIBBON_TOOLBAR_TOOL_ACTIVE_MASK )
        {
            wxRect activeRect(bgRect);
            if ( kind == wxRIBBON_BUTTON_HYBRID )
            {
                activeRect.width -= 8;
                if ( state & wxRIBBON_TOOLBAR_TOOL_DROPDOWN_ACTIVE )
                {
                    activeRect.x += activeRect.width;
                    activeRect.width = 8;
                }
            }
            dc.SetBrush(m_toolActiveBackgroundBrush);
            dc.DrawRectangle(activeRect.x, activeRect.y, activeRect.width,
                activeRect.height);
        }
    }

    // Border
    if ( isCustomBg )
        dc.SetPen(m_toolbarHoverBordenPen);
    else
        dc.SetPen(m_toolbarBorderPen);
    if ( (state & wxRIBBON_TOOLBAR_TOOL_FIRST) == 0 )
    {
        wxColour existing;
        if ( !dc.GetPixel(rect.x, rect.y + 1, &existing) ||
             existing != m_toolbarHoverBordenPen.GetColour() )
        {
            dc.DrawLine(rect.x, rect.y + 1, rect.x, rect.y + rect.height - 1);
        }
    }
    if ( isCustomBg )
    {
        wxRect borderRect(bgRect);
        borderRect.Inflate(1);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(borderRect.x, borderRect.y, borderRect.width,
            borderRect.height);
    }

    // Foreground
    int availWidth = bgRect.GetWidth();
    if ( kind & wxRIBBON_BUTTON_DROPDOWN )
    {
        availWidth -= 8;
        if ( isSplitHybrid )
        {
            dc.DrawLine(rect.x + availWidth + 1, rect.y,
                rect.x + availWidth + 1, rect.y + rect.height);
        }
        dc.DrawBitmap(m_toolbarDropBundle.GetBitmapFor(wnd), bgRect.x + availWidth + 2,
            bgRect.y + (bgRect.height / 2) - 2, true);
    }
    if ( bitmap.IsOk() )
        dc.DrawBitmap(bitmap, bgRect.x + (availWidth - bitmap.GetLogicalWidth()) / 2,
            bgRect.y + (bgRect.height - bitmap.GetLogicalHeight()) / 2, true);
}

#endif // wxUSE_RIBBON

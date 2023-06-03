/////////////////////////////////////////////////////////////////////////////
// Name:        src/aui/barartmsw.cpp
// Purpose:     Implementation of wxAuiMSWToolBarArt
// Author:      Tobias Taschner
// Created:     2015-09-22
// Copyright:   (c) 2015 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_AUI && wxUSE_UXTHEME

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/dcclient.h"
    #include "wx/app.h"
    #include "wx/dc.h"
#endif

#include "wx/aui/auibar.h"
#include "wx/aui/framemanager.h"
#include "wx/msw/uxtheme.h"
#include "wx/msw/private.h"
#include "wx/msw/private/darkmode.h"

wxAuiMSWToolBarArt::wxAuiMSWToolBarArt()
{
    // Theme colours don't work in dark theme, so don't use them in this case.
    if ( wxUxThemeIsActive() && !wxMSWDarkMode::IsActive() )
    {
        m_themed = true;

        // Determine sizes from theme
        wxWindow* window = static_cast<wxApp*>(wxApp::GetInstance())->GetTopWindow();
        wxUxThemeHandle hTheme(window, L"Rebar");

        m_overflowSize = hTheme.GetTrueSize(RP_CHEVRON).x;

        m_gripperSize = hTheme.GetTrueSize(RP_GRIPPER).x;

        wxUxThemeHandle hThemeToolbar(window, L"Toolbar");

        m_separatorSize = hThemeToolbar.GetTrueSize(TP_SEPARATOR).x;

        // TP_DROPDOWNBUTTON is only 7px, too small to fit the dropdown arrow,
        // use 14px instead.
        m_dropdownSize = window->FromDIP(14);

        m_buttonSize = hThemeToolbar.GetTrueSize(TP_BUTTON);
    }
    else
        m_themed = false;
}

wxAuiToolBarArt* wxAuiMSWToolBarArt::Clone()
{
    return new wxAuiMSWToolBarArt;
}

void wxAuiMSWToolBarArt::DrawBackground(
    wxDC& dc,
    wxWindow* wnd,
    const wxRect& rect)
{
    if ( m_themed )
    {
        wxUxThemeHandle hTheme(wnd, L"Rebar");

        hTheme.DrawBackground(
            GetHdcOf(dc.GetTempHDC()),
            rect,
            RP_BACKGROUND
        );
    }
    else
        wxAuiGenericToolBarArt::DrawBackground(dc, wnd, rect);
}

void wxAuiMSWToolBarArt::DrawLabel(
    wxDC& dc,
    wxWindow* wnd,
    const wxAuiToolBarItem& item,
    const wxRect& rect)
{
    wxAuiGenericToolBarArt::DrawLabel(dc, wnd, item, rect);
}

void wxAuiMSWToolBarArt::DrawButton(
    wxDC& dc,
    wxWindow* wnd,
    const wxAuiToolBarItem& item,
    const wxRect& rect)
{
    if ( m_themed )
    {
        wxUxThemeHandle hTheme(wnd, L"Toolbar");

        int btnState;
        if ( item.GetState() & wxAUI_BUTTON_STATE_DISABLED )
            btnState = TS_DISABLED;
        else if ( item.GetState() & wxAUI_BUTTON_STATE_PRESSED )
            btnState = TS_PRESSED;
        else if ( item.GetState() & wxAUI_BUTTON_STATE_HOVER &&
            item.GetState() & wxAUI_BUTTON_STATE_CHECKED )
            btnState = TS_HOTCHECKED;
        else if ( item.GetState() & wxAUI_BUTTON_STATE_CHECKED )
            btnState = TS_CHECKED;
        else if ( (item.GetState() & wxAUI_BUTTON_STATE_HOVER) || item.IsSticky() )
            btnState = TS_HOT;
        else
            btnState = TS_NORMAL;

        hTheme.DrawBackground(
            GetHdcOf(dc.GetTempHDC()),
            rect,
            TP_BUTTON,
            btnState
        );

        int textWidth = 0, textHeight = 0;

        if ( m_flags & wxAUI_TB_TEXT )
        {
            dc.SetFont(m_font);

            int tx, ty;

            dc.GetTextExtent(wxT("ABCDHgj"), &tx, &textHeight);
            textWidth = 0;
            dc.GetTextExtent(item.GetLabel(), &textWidth, &ty);
        }

        int bmpX = 0, bmpY = 0;
        int textX = 0, textY = 0;

        const wxBitmap& bmp = item.GetCurrentBitmapFor(wnd);
        if ( m_textOrientation == wxAUI_TBTOOL_TEXT_BOTTOM )
        {
            bmpX = rect.x +
                (rect.width / 2) -
                (bmp.GetWidth() / 2);

            bmpY = rect.y +
                ((rect.height - textHeight) / 2) -
                (bmp.GetHeight() / 2);

            textX = rect.x + (rect.width / 2) - (textWidth / 2) + 1;
            textY = rect.y + rect.height - textHeight - 1;
        }
        else if ( m_textOrientation == wxAUI_TBTOOL_TEXT_RIGHT )
        {
            bmpX = rect.x + wnd->FromDIP(3);

            bmpY = rect.y +
                (rect.height / 2) -
                (bmp.GetHeight() / 2);

            textX = bmpX + wnd->FromDIP(3) + bmp.GetWidth();
            textY = rect.y +
                (rect.height / 2) -
                (textHeight / 2);
        }

        if ( bmp.IsOk() )
            dc.DrawBitmap(bmp, bmpX, bmpY, true);

        // set the item's text color based on if it is disabled
        if ( item.GetState() & wxAUI_BUTTON_STATE_DISABLED )
            dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
        else
            dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));

        if ( (m_flags & wxAUI_TB_TEXT) && !item.GetLabel().empty() )
        {
            dc.DrawText(item.GetLabel(), textX, textY);
        }
    }
    else
        wxAuiGenericToolBarArt::DrawButton(dc, wnd, item, rect);
}

void wxAuiMSWToolBarArt::DrawDropDownButton(
    wxDC& dc,
    wxWindow* wnd,
    const wxAuiToolBarItem& item,
    const wxRect& rect)
{
    if ( m_themed )
    {
        wxUxThemeHandle hTheme(wnd, L"Toolbar");

        int textWidth = 0, textHeight = 0, textX = 0, textY = 0;
        int bmpX = 0, bmpY = 0;

        wxRect buttonRect = wxRect(rect.x,
            rect.y,
            rect.width - m_dropdownSize,
            rect.height);
        wxRect dropDownRect = wxRect(rect.x + rect.width - m_dropdownSize - 1,
            rect.y,
            m_dropdownSize + 1,
            rect.height);

        if ( m_flags & wxAUI_TB_TEXT )
        {
            dc.SetFont(m_font);

            int tx, ty;
            if ( m_flags & wxAUI_TB_TEXT )
            {
                dc.GetTextExtent(wxT("ABCDHgj"), &tx, &textHeight);
                textWidth = 0;
            }

            dc.GetTextExtent(item.GetLabel(), &textWidth, &ty);
        }

        int btnState;
        if ( item.GetState() & wxAUI_BUTTON_STATE_DISABLED )
            btnState = TS_DISABLED;
        else if ( item.GetState() & wxAUI_BUTTON_STATE_PRESSED )
            btnState = TS_PRESSED;
        else if ( (item.GetState() & wxAUI_BUTTON_STATE_HOVER) || item.IsSticky() )
            btnState = TS_HOT;
        else
            btnState = TS_NORMAL;

        {
        auto tempHDC = dc.GetTempHDC();

        hTheme.DrawBackground(
            GetHdcOf(tempHDC),
            buttonRect,
            TP_SPLITBUTTON,
            btnState
        );

        hTheme.DrawBackground(
            GetHdcOf(tempHDC),
            dropDownRect,
            TP_SPLITBUTTONDROPDOWN,
            btnState
        );
        } // End of tempHDC scope.

        const wxBitmap& bmp = item.GetCurrentBitmapFor(wnd);
        if ( !bmp.IsOk() )
            return;

        if ( m_textOrientation == wxAUI_TBTOOL_TEXT_BOTTOM )
        {
            bmpX = buttonRect.x +
                (buttonRect.width / 2) -
                (bmp.GetWidth() / 2);
            bmpY = buttonRect.y +
                ((buttonRect.height - textHeight) / 2) -
                (bmp.GetHeight() / 2);

            textX = rect.x + (rect.width / 2) - (textWidth / 2) + 1;
            textY = rect.y + rect.height - textHeight - 1;
        }
        else if ( m_textOrientation == wxAUI_TBTOOL_TEXT_RIGHT )
        {
            bmpX = rect.x + wnd->FromDIP(3);

            bmpY = rect.y +
                (rect.height / 2) -
                (bmp.GetHeight() / 2);

            textX = bmpX + wnd->FromDIP(3) + bmp.GetWidth();
            textY = rect.y +
                (rect.height / 2) -
                (textHeight / 2);
        }

        dc.DrawBitmap(bmp, bmpX, bmpY, true);

        // set the item's text color based on if it is disabled
        if ( item.GetState() & wxAUI_BUTTON_STATE_DISABLED )
            dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
        else
            dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));

        if ( (m_flags & wxAUI_TB_TEXT) && !item.GetLabel().empty() )
        {
            dc.DrawText(item.GetLabel(), textX, textY);
        }

    }
    else
        wxAuiGenericToolBarArt::DrawDropDownButton(dc, wnd, item, rect);
}

void wxAuiMSWToolBarArt::DrawControlLabel(
    wxDC& dc,
    wxWindow* wnd,
    const wxAuiToolBarItem& item,
    const wxRect& rect)
{
    wxAuiGenericToolBarArt::DrawControlLabel(dc, wnd, item, rect);
}

void wxAuiMSWToolBarArt::DrawSeparator(
    wxDC& dc,
    wxWindow* wnd,
    const wxRect& rect)
{
    if ( m_themed )
    {
        wxUxThemeHandle hTheme(wnd, L"Toolbar");

        hTheme.DrawBackground(
            GetHdcOf(dc.GetTempHDC()),
            rect,
            (m_flags & wxAUI_TB_VERTICAL) ? TP_SEPARATORVERT : TP_SEPARATOR
        );
    }
    else
        wxAuiGenericToolBarArt::DrawSeparator(dc, wnd, rect);
}

void wxAuiMSWToolBarArt::DrawGripper(
    wxDC& dc,
    wxWindow* wnd,
    const wxRect& rect)
{
    if ( m_themed )
    {
        wxUxThemeHandle hTheme(wnd, L"Rebar");

        hTheme.DrawBackground(
            GetHdcOf(dc.GetTempHDC()),
            rect,
            (m_flags & wxAUI_TB_VERTICAL) ? RP_GRIPPERVERT : RP_GRIPPER
        );
    }
    else
        wxAuiGenericToolBarArt::DrawGripper(dc, wnd, rect);
}

void wxAuiMSWToolBarArt::DrawOverflowButton(
    wxDC& dc,
    wxWindow* wnd,
    const wxRect& rect,
    int state)
{
    if ( m_themed )
    {
        wxUxThemeHandle hTheme(wnd, L"Rebar");

        int chevState;
        if ( state & wxAUI_BUTTON_STATE_PRESSED )
            chevState = CHEVS_PRESSED;
        else if ( state & wxAUI_BUTTON_STATE_HOVER )
            chevState = CHEVS_HOT;
        else
            chevState = CHEVS_NORMAL;

        hTheme.DrawBackground(
            GetHdcOf(dc.GetTempHDC()),
            rect,
            (m_flags & wxAUI_TB_VERTICAL) ? RP_CHEVRONVERT : RP_CHEVRON,
            chevState
        );
    }
    else
        wxAuiGenericToolBarArt::DrawOverflowButton(dc, wnd, rect, state);
}

wxSize wxAuiMSWToolBarArt::GetLabelSize(
    wxDC& dc,
    wxWindow* wnd,
    const wxAuiToolBarItem& item)
{
    return wxAuiGenericToolBarArt::GetLabelSize(dc, wnd, item);
}

wxSize wxAuiMSWToolBarArt::GetToolSize(
    wxDC& dc,
    wxWindow* wnd,
    const wxAuiToolBarItem& item)
{
    if ( m_themed )
    {
        if ( !item.GetBitmapBundle().IsOk() && !(m_flags & wxAUI_TB_TEXT) )
            return m_buttonSize;

        wxSize size = wxAuiGenericToolBarArt::GetToolSize(dc, wnd, item);

        size.IncBy(wnd->FromDIP(3)); // Add some padding for native theme

        return size;
    }
    else
        return wxAuiGenericToolBarArt::GetToolSize(dc, wnd, item);
}

int wxAuiMSWToolBarArt::GetElementSize(int element)
{
    return wxAuiGenericToolBarArt::GetElementSize(element);
}

void wxAuiMSWToolBarArt::SetElementSize(int elementId, int size)
{
    wxAuiGenericToolBarArt::SetElementSize(elementId, size);
}

int wxAuiMSWToolBarArt::ShowDropDown(wxWindow* wnd,
    const wxAuiToolBarItemArray& items)
{
    return wxAuiGenericToolBarArt::ShowDropDown(wnd, items);
}

#endif // wxUSE_AUI && wxUSE_UXTHEME

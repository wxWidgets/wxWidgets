/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_settings_g.cpp
// Purpose:     iPhone settings
// Author:      Julian Smart
// Modified by:
// Created:     12/05/2009 08:59:11
// RCS-ID:
// Copyright:   Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wx.h"

#include "wx/mobile/settings.h"

wxColour wxMoSystemSettings::GetColour(wxMoSystemColour id)
{
    wxMO_COLOUR(wxMO_COLOUR_NORMAL_BUTTON_BG,       100, 100, 100);
    wxMO_COLOUR(wxMO_COLOUR_NORMAL_BUTTON_TEXT,     255, 255, 255);
    wxMO_COLOUR(wxMO_COLOUR_NORMAL_BUTTON_BORDER,   160, 160, 160);
    wxMO_COLOUR(wxMO_COLOUR_ROUNDED_RECT_BUTTON_BG, 255, 255, 255);
    wxMO_COLOUR(wxMO_COLOUR_ROUNDED_RECT_BUTTON_TEXT, 0, 0, 0);
    wxMO_COLOUR(wxMO_COLOUR_DONE_BUTTON_BG,         18, 163, 238);

    wxMO_COLOUR(wxMO_COLOUR_SWITCH_OFF_BG,          200, 200, 200);
    wxMO_COLOUR(wxMO_COLOUR_SWITCH_ON_BG,           80, 40, 244);
    wxMO_COLOUR(wxMO_COLOUR_SWITCH_THUMB_BG,        180, 180, 180);
    wxMO_COLOUR(wxMO_COLOUR_SWITCH_ON_TEXT,         255, 255, 255);
    wxMO_COLOUR(wxMO_COLOUR_SWITCH_OFF_TEXT,        100, 100, 100);

    wxMO_COLOUR(wxMO_COLOUR_PAGE_SELECTED,          255, 255, 255);
    wxMO_COLOUR(wxMO_COLOUR_PAGE_UNSELECTED,        120, 120, 120);

    wxMO_COLOUR(wxMO_COLOUR_GAUGE_BG,               255, 255, 255);
    wxMO_COLOUR(wxMO_COLOUR_GAUGE_FILL,             41, 127, 226);

    wxMO_COLOUR(wxMO_COLOUR_SLIDER_BG,              200, 200, 200);
    wxMO_COLOUR(wxMO_COLOUR_SLIDER_FILL,            41, 127, 226);
    wxMO_COLOUR(wxMO_COLOUR_SLIDER_THUMB,           180, 180, 180);

    wxMO_COLOUR(wxMO_COLOUR_SEGCTRL_BG,             200, 200, 200);
    wxMO_COLOUR(wxMO_COLOUR_SEGCTRL_SELECTED_BG,    86,  149, 242);
    wxMO_COLOUR(wxMO_COLOUR_SEGCTRL_TEXT,           100, 100, 100);
    wxMO_COLOUR(wxMO_COLOUR_SEGCTRL_BORDER,         100, 100, 100);

    wxMO_COLOUR(wxMO_COLOUR_NAVBAR_NORMAL_BG,       125, 146, 173);
    wxMO_COLOUR(wxMO_COLOUR_NAVBAR_BLACK_OPAQUE_BG, 50, 50, 50);
    wxMO_COLOUR(wxMO_COLOUR_NAVBAR_BLACK_TRANSLUCENT_BG,50, 50, 50);
    wxMO_COLOUR(wxMO_COLOUR_NAVBAR_TEXT,            255, 255, 255);
    wxMO_COLOUR(wxMO_COLOUR_NAVBAR_NORMAL_BUTTON_BG,105, 126, 153);
    wxMO_COLOUR(wxMO_COLOUR_NAVBAR_BLACK_BUTTON_BG, 80, 80, 80);
    wxMO_COLOUR(wxMO_COLOUR_NAVBAR_BUTTON_BORDER,   100, 100, 100);

    wxMO_COLOUR(wxMO_COLOUR_TOOLBAR_NORMAL_BG,      125, 146, 173);
    wxMO_COLOUR(wxMO_COLOUR_TOOLBAR_BLACK_OPAQUE_BG,50, 50, 50);
    wxMO_COLOUR(wxMO_COLOUR_TOOLBAR_BLACK_TRANSLUCENT_BG,50, 50, 50);
    wxMO_COLOUR(wxMO_COLOUR_TOOLBAR_BLACK_BUTTON_BG,80, 80, 80);
    wxMO_COLOUR(wxMO_COLOUR_TOOLBAR_NORMAL_BUTTON_BG,73, 107, 155);
    wxMO_COLOUR(wxMO_COLOUR_TOOLBAR_TEXT,           255, 255, 255);
    wxMO_COLOUR(wxMO_COLOUR_TOOLBAR_BUTTON_BORDER,  80, 80, 80);

    wxMO_COLOUR(wxMO_COLOUR_TAB_BG,                 50, 50, 50);
    wxMO_COLOUR(wxMO_COLOUR_TAB_BUTTON_BG,          80, 80, 80);
    wxMO_COLOUR(wxMO_COLOUR_TAB_TEXT,               255, 255, 255);
    wxMO_COLOUR(wxMO_COLOUR_TAB_BUTTON_BORDER,      80, 80, 80);
    wxMO_COLOUR(wxMO_COLOUR_TAB_BADGE_BG,           255, 0, 0);

    wxMO_COLOUR(wxMO_COLOUR_PANEL_BG,               255, 255, 255);

    wxMO_COLOUR(wxMO_COLOUR_ACTION_SHEET_BG,        50, 50, 50);
    wxMO_COLOUR(wxMO_COLOUR_ACTION_SHEET_OK_BG,     227, 57, 42);
    wxMO_COLOUR(wxMO_COLOUR_ACTION_SHEET_CANCEL_BG, 40, 40, 40);
    wxMO_COLOUR(wxMO_COLOUR_ACTION_SHEET_OTHER_BG,  120, 120, 120);
    wxMO_COLOUR(wxMO_COLOUR_ACTION_SHEET_TEXT,      255, 255, 255);

    wxMO_COLOUR(wxMO_COLOUR_ALERT_SHEET_BG,         90, 90, 90);

    wxMO_COLOUR(wxMO_COLOUR_WHEELS_BG,              255, 255, 255);
    wxMO_COLOUR(wxMO_COLOUR_WHEELS_TEXT,            0, 0, 0);
    wxMO_COLOUR(wxMO_COLOUR_WHEELS_LIGHT_GREY,      220, 220, 220);
    wxMO_COLOUR(wxMO_COLOUR_WHEELS_DARK_GREY,       50, 50, 50);

    wxMO_COLOUR(wxMO_COLOUR_GREY_SHADOW,            140, 140, 140);

    wxMO_COLOUR(wxMO_COLOUR_TABLE_SURROUND,         200, 200, 200);
    wxMO_COLOUR(wxMO_COLOUR_TABLE_LINE,             180, 180, 180);
    wxMO_COLOUR(wxMO_COLOUR_TABLE_BG,               255, 255, 255);
    wxMO_COLOUR(wxMO_COLOUR_TABLE_PLAIN_SECTION_TEXT, 255, 255, 255);
    wxMO_COLOUR(wxMO_COLOUR_TABLE_GROUPED_SECTION_TEXT, 100, 100, 100);
    wxMO_COLOUR(wxMO_COLOUR_TABLE_ROW_TEXT,         0, 0, 0);
    wxMO_COLOUR(wxMO_COLOUR_TABLE_SELECTED_TEXT,    255, 255, 255);
    wxMO_COLOUR(wxMO_COLOUR_TABLE_BLUE_SELECTION,   100, 105, 255);
    wxMO_COLOUR(wxMO_COLOUR_TABLE_GREY_SELECTION,   180, 180, 180);
    wxMO_COLOUR(wxMO_COLOUR_TABLE_PLAIN_SECTION_HEADER, 175, 185, 194);
    wxMO_COLOUR(wxMO_COLOUR_TABLE_PLAIN_TABLE_HEADER,226, 231, 237);
    wxMO_COLOUR(wxMO_COLOUR_TABLE_INDEX_TEXT,       100, 100, 100);
    wxMO_COLOUR(wxMO_COLOUR_TABLE_DETAIL_TEXT_BLUE, 0, 0, 255);
    wxMO_COLOUR(wxMO_COLOUR_TABLE_DETAIL_TEXT_GREY, 120, 120, 120);

    return *wxBLACK;
}

wxFont wxMoSystemSettings::GetFont(wxMoSystemFont id)
{
    static bool s_initFonts = false;
    static wxFont s_normalButtonFont;
    static wxFont s_navBarTitleFont;
    static wxFont s_sheetMessageFont;
    static wxFont s_sheetButtonFont;
    static wxFont s_tabCtrlFont;
    static wxFont s_tableCtrlFont;
    static wxFont s_tableCtrlDetailFont;
    static wxFont s_tableCtrlSectionFont;
    static wxFont s_tableCtrlTableHeaderFont;
    static wxFont s_tableCtrlIndexFont;

    if (!s_initFonts)
    {
        s_initFonts = true;

        //wxFont baseFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
        wxFont baseFont(*wxSWISS_FONT);
        //wxFont baseFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("Arial"));

        s_normalButtonFont = baseFont;
        s_normalButtonFont.SetWeight(wxFONTWEIGHT_BOLD);
        s_normalButtonFont.SetPointSize(8);

        s_navBarTitleFont = baseFont;
        s_navBarTitleFont.SetWeight(wxFONTWEIGHT_BOLD);
        s_navBarTitleFont.SetPointSize(10);

        s_sheetMessageFont = baseFont;

        s_sheetButtonFont = baseFont;
        s_sheetButtonFont.SetWeight(wxFONTWEIGHT_BOLD);
        s_sheetButtonFont.SetPointSize(13);

        s_tabCtrlFont = baseFont;
        s_tabCtrlFont.SetPointSize(7);

        s_tableCtrlFont = baseFont;
        s_tableCtrlFont.SetWeight(wxFONTWEIGHT_BOLD);
        s_tableCtrlFont.SetPointSize(11);

        s_tableCtrlDetailFont = baseFont;
        s_tableCtrlDetailFont.SetWeight(wxFONTWEIGHT_BOLD);
        s_tableCtrlDetailFont.SetPointSize(9);

        s_tableCtrlSectionFont = baseFont;
        s_tableCtrlSectionFont.SetWeight(wxFONTWEIGHT_BOLD);
        s_tableCtrlSectionFont.SetPointSize(9);

        s_tableCtrlTableHeaderFont = baseFont;
        s_tableCtrlTableHeaderFont.SetPointSize(11);

        s_tableCtrlIndexFont = baseFont;
        s_tableCtrlIndexFont.SetWeight(wxFONTWEIGHT_BOLD);
        s_tableCtrlIndexFont.SetPointSize(8);
    }

    // TODO: other fonts

    wxMO_FONT(wxMO_FONT_NORMAL_BUTTON, s_normalButtonFont);

    wxMO_FONT(wxMO_FONT_ACTION_SHEET_BUTTON, s_sheetButtonFont);
    wxMO_FONT(wxMO_FONT_ACTION_SHEET_TITLE, s_normalButtonFont);

    wxMO_FONT(wxMO_FONT_ALERT_SHEET_BUTTON, s_sheetButtonFont);
    wxMO_FONT(wxMO_FONT_ALERT_SHEET_TITLE, s_normalButtonFont);
    wxMO_FONT(wxMO_FONT_ALERT_SHEET_MESSAGE, s_sheetMessageFont);

    wxMO_FONT(wxMO_FONT_TOOLBAR, s_tabCtrlFont);
    wxMO_FONT(wxMO_FONT_TAB, s_tabCtrlFont);
    wxMO_FONT(wxMO_FONT_SEGMENTEDCTRL, s_normalButtonFont);

    wxMO_FONT(wxMO_FONT_SWITCH, s_normalButtonFont);
    wxMO_FONT(wxMO_FONT_TEXTCTRL, s_normalButtonFont);
    wxMO_FONT(wxMO_FONT_WHEELSCTRL, s_normalButtonFont);

    wxMO_FONT(wxMO_FONT_NAVBAR_TITLE, s_navBarTitleFont);
    wxMO_FONT(wxMO_FONT_NAVBAR_BUTTON, s_normalButtonFont);

    wxMO_FONT(wxMO_FONT_TABLE_ROWS, s_tableCtrlFont);
    wxMO_FONT(wxMO_FONT_TABLE_DETAIL, s_tableCtrlDetailFont);
    wxMO_FONT(wxMO_FONT_TABLE_SECTIONS, s_tableCtrlSectionFont);

    wxMO_FONT(wxMO_FONT_TABLE_TABLE_HEADER, s_tableCtrlTableHeaderFont);
    wxMO_FONT(wxMO_FONT_TABLE_INDEX, s_tableCtrlIndexFont);

    return s_normalButtonFont;
}

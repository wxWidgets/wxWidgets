/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/settings.cpp
// Purpose:     wxSystemSettings implementation for classic Mac OS
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/settings.h"

#ifndef WX_PRECOMP
    #include "wx/colour.h"
    #include "wx/font.h"
    #include "wx/gdicmn.h"
#endif

#include "wx/retro68/private/qd.h"

wxColour wxSystemSettingsNative::GetColour(wxSystemColour WXUNUSED(index))
{
    // overridden by wxSystemSettings::GetColour in wxUniversal
    return wxColour(0,0,0);
}

wxFont wxSystemSettingsNative::GetFont(wxSystemFont index)
{
    switch (index)
    {
        case wxSYS_OEM_FIXED_FONT:
        case wxSYS_ANSI_FIXED_FONT:
        case wxSYS_SYSTEM_FIXED_FONT:
            return wxFont(9,
                          wxFONTFAMILY_TELETYPE,
                          wxFONTSTYLE_NORMAL,
                          wxFONTWEIGHT_NORMAL);

        case wxSYS_ANSI_VAR_FONT:
            return wxFont(10,
                          wxFONTFAMILY_SWISS,
                          wxFONTSTYLE_NORMAL,
                          wxFONTWEIGHT_NORMAL);

        case wxSYS_SYSTEM_FONT:
        case wxSYS_DEVICE_DEFAULT_FONT:
        case wxSYS_DEFAULT_GUI_FONT:
        {
            // Use the system font (Chicago or Charcoal) used by the Toolbox
            // for all the standard user interface elements.
            Str255 name;
            GetFontName(systemFont, name);

            return wxFont(12,
                          wxFONTFAMILY_DEFAULT,
                          wxFONTSTYLE_NORMAL,
                          wxFONTWEIGHT_NORMAL,
                          false,
                          wxMacMakeStringFromPascal(name));
        }

        default:
            wxFAIL_MSG( "unknown font type" );
            return wxNullFont;
    }
}

int wxSystemSettingsNative::GetMetric(wxSystemMetric index,
                                      const wxWindow* WXUNUSED(win))
{
    int val;

    switch ( index )
    {
        case wxSYS_SCREEN_X:
            wxDisplaySize(&val, nullptr);
            return val;

        case wxSYS_SCREEN_Y:
            wxDisplaySize(nullptr, &val);
            return val;

        case wxSYS_VSCROLL_X:
        case wxSYS_HSCROLL_Y:
        case wxSYS_VSCROLL_ARROW_X:
        case wxSYS_VSCROLL_ARROW_Y:
        case wxSYS_HSCROLL_ARROW_X:
        case wxSYS_HSCROLL_ARROW_Y:
        case wxSYS_VTHUMB_Y:
        case wxSYS_HTHUMB_X:
            return 16;

        case wxSYS_CAPTION_Y:
            return 19;

        case wxSYS_MENU_Y:
            return wxMacGetMenuBarHeight();

        case wxSYS_BORDER_X:
        case wxSYS_BORDER_Y:
        case wxSYS_EDGE_X:
        case wxSYS_EDGE_Y:
            return 1;

        case wxSYS_DCLICK_X:
        case wxSYS_DCLICK_Y:
        case wxSYS_DRAG_X:
        case wxSYS_DRAG_Y:
            return 4;

        case wxSYS_DCLICK_MSEC:
            return wxMacTicksToMillis(wxMacGetDblTime());

        case wxSYS_CARET_ON_MSEC:
        case wxSYS_CARET_OFF_MSEC:
            return wxMacTicksToMillis(wxMacGetCaretTime());

        case wxSYS_CURSOR_X:
        case wxSYS_CURSOR_Y:
            return 16;

        case wxSYS_ICON_X:
        case wxSYS_ICON_Y:
            return 32;

        case wxSYS_SMALLICON_X:
        case wxSYS_SMALLICON_Y:
            return 16;

        case wxSYS_MOUSE_BUTTONS:
            return 1;

        case wxSYS_SWAP_BUTTONS:
            return 0;

        default:
            return -1;
    }
}

bool wxSystemSettingsNative::HasFeature(wxSystemFeature index)
{
    switch (index)
    {
        case wxSYS_CAN_DRAW_FRAME_DECORATIONS:
            // Native windows always have native decorations.
            return true;

        case wxSYS_CAN_ICONIZE_FRAME:
        case wxSYS_TABLET_PRESENT:
            return false;

        default:
            wxFAIL_MSG( "unknown feature" );
            return false;
    }
}

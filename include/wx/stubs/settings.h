/////////////////////////////////////////////////////////////////////////////
// Name:        settings.h
// Purpose:     wxSystemSettings class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SETTINGS_H_
#define _WX_SETTINGS_H_

#ifdef __GNUG__
#pragma interface "settings.h"
#endif

#include "wx/setup.h"

#define wxSYS_WHITE_BRUSH         0
#define wxSYS_LTGRAY_BRUSH        1
#define wxSYS_GRAY_BRUSH          2
#define wxSYS_DKGRAY_BRUSH        3
#define wxSYS_BLACK_BRUSH         4
#define wxSYS_NULL_BRUSH          5
#define wxSYS_HOLLOW_BRUSH        wxSYS_NULL_BRUSH
#define wxSYS_WHITE_PEN           6
#define wxSYS_BLACK_PEN           7
#define wxSYS_NULL_PEN            8
#define wxSYS_OEM_FIXED_FONT      10
#define wxSYS_ANSI_FIXED_FONT     11
#define wxSYS_ANSI_VAR_FONT       12
#define wxSYS_SYSTEM_FONT         13
#define wxSYS_DEVICE_DEFAULT_FONT 14
#define wxSYS_DEFAULT_PALETTE     15
#define wxSYS_SYSTEM_FIXED_FONT   16 // Obsolete
#define wxSYS_DEFAULT_GUI_FONT    17

#define wxSYS_COLOUR_SCROLLBAR         0
#define wxSYS_COLOUR_BACKGROUND        1
#define wxSYS_COLOUR_ACTIVECAPTION     2
#define wxSYS_COLOUR_INACTIVECAPTION   3
#define wxSYS_COLOUR_MENU              4
#define wxSYS_COLOUR_WINDOW            5
#define wxSYS_COLOUR_WINDOWFRAME       6
#define wxSYS_COLOUR_MENUTEXT          7
#define wxSYS_COLOUR_WINDOWTEXT        8
#define wxSYS_COLOUR_CAPTIONTEXT       9
#define wxSYS_COLOUR_ACTIVEBORDER      10
#define wxSYS_COLOUR_INACTIVEBORDER    11
#define wxSYS_COLOUR_APPWORKSPACE      12
#define wxSYS_COLOUR_HIGHLIGHT         13
#define wxSYS_COLOUR_HIGHLIGHTTEXT     14
#define wxSYS_COLOUR_BTNFACE           15
#define wxSYS_COLOUR_BTNSHADOW         16
#define wxSYS_COLOUR_GRAYTEXT          17
#define wxSYS_COLOUR_BTNTEXT           18
#define wxSYS_COLOUR_INACTIVECAPTIONTEXT 19
#define wxSYS_COLOUR_BTNHIGHLIGHT      20

#define wxSYS_COLOUR_3DDKSHADOW        21
#define wxSYS_COLOUR_3DLIGHT           22
#define wxSYS_COLOUR_INFOTEXT          23
#define wxSYS_COLOUR_INFOBK            24

#define wxSYS_COLOUR_DESKTOP           wxSYS_COLOUR_BACKGROUND
#define wxSYS_COLOUR_3DFACE            wxSYS_COLOUR_BTNFACE
#define wxSYS_COLOUR_3DSHADOW          wxSYS_COLOUR_BTNSHADOW
#define wxSYS_COLOUR_3DHIGHLIGHT       wxSYS_COLOUR_BTNHIGHLIGHT
#define wxSYS_COLOUR_3DHILIGHT         wxSYS_COLOUR_BTNHIGHLIGHT
#define wxSYS_COLOUR_BTNHILIGHT        wxSYS_COLOUR_BTNHIGHLIGHT

// Metrics
#define wxSYS_MOUSE_BUTTONS           1
#define wxSYS_BORDER_X                2
#define wxSYS_BORDER_Y                3
#define wxSYS_CURSOR_X                4
#define wxSYS_CURSOR_Y                5
#define wxSYS_DCLICK_X                6
#define wxSYS_DCLICK_Y                7
#define wxSYS_DRAG_X                  8
#define wxSYS_DRAG_Y                  9
#define wxSYS_EDGE_X                  10
#define wxSYS_EDGE_Y                  11
#define wxSYS_HSCROLL_ARROW_X         12
#define wxSYS_HSCROLL_ARROW_Y         13
#define wxSYS_HTHUMB_X                14
#define wxSYS_ICON_X                  15
#define wxSYS_ICON_Y                  16
#define wxSYS_ICONSPACING_X           17
#define wxSYS_ICONSPACING_Y           18
#define wxSYS_WINDOWMIN_X             19
#define wxSYS_WINDOWMIN_Y             20
#define wxSYS_SCREEN_X                21
#define wxSYS_SCREEN_Y                22
#define wxSYS_FRAMESIZE_X             23
#define wxSYS_FRAMESIZE_Y             24
#define wxSYS_SMALLICON_X             25
#define wxSYS_SMALLICON_Y             26
#define wxSYS_HSCROLL_Y               27
#define wxSYS_VSCROLL_X               28
#define wxSYS_VSCROLL_ARROW_X         29
#define wxSYS_VSCROLL_ARROW_Y         30
#define wxSYS_VTHUMB_Y                31
#define wxSYS_CAPTION_Y               32
#define wxSYS_MENU_Y                  33
#define wxSYS_NETWORK_PRESENT         34
#define wxSYS_PENWINDOWS_PRESENT      35
#define wxSYS_SHOW_SOUNDS             36
#define wxSYS_SWAP_BUTTONS            37

class WXDLLEXPORT wxSystemSettings: public wxObject
{
public:
    inline wxSystemSettings() {}

    // Get a system colour
    static wxColour    GetSystemColour(int index);

    // Get a system font
    static wxFont      GetSystemFont(int index);

    // Get a system metric, e.g. scrollbar size
    static int         GetSystemMetric(int index);
};

#endif
    // _WX_SETTINGS_H_

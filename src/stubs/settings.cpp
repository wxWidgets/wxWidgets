/////////////////////////////////////////////////////////////////////////////
// Name:        settings.cpp
// Purpose:     wxSettings
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "settings.h"
#endif

#include "wx/settings.h"

wxColour wxSystemSettings::GetSystemColour(int index)
{
    // TODO
	return wxColour();
}

wxFont wxSystemSettings::GetSystemFont(int index)
{
    // TODO
    switch (index)
    {
        case wxSYS_DEVICE_DEFAULT_FONT:
        {
            break;
        }
        case wxSYS_DEFAULT_PALETTE:
        {
            break;
        }
        case wxSYS_SYSTEM_FIXED_FONT:
        {
            break;
        }
        case wxSYS_SYSTEM_FONT:
        {
            break;
        }
        default:
        case wxSYS_DEFAULT_GUI_FONT:
        {
            break;
        }
    }

    return wxFont();
}

// Get a system metric, e.g. scrollbar size
int wxSystemSettings::GetSystemMetric(int index)
{
	switch ( index)
	{
        case wxSYS_MOUSE_BUTTONS:
            // TODO
			return 0;
        case wxSYS_BORDER_X:
            // TODO
			return 0;
        case wxSYS_BORDER_Y:
            // TODO
			return 0;
        case wxSYS_CURSOR_X:
            // TODO
			return 0;
        case wxSYS_CURSOR_Y:
            // TODO
			return 0;
        case wxSYS_DCLICK_X:
            // TODO
			return 0;
        case wxSYS_DCLICK_Y:
            // TODO
			return 0;
        case wxSYS_DRAG_X:
            // TODO
			return 0;
        case wxSYS_DRAG_Y:
            // TODO
			return 0;
        case wxSYS_EDGE_X:
            // TODO
			return 0;
        case wxSYS_EDGE_Y:
            // TODO
			return 0;
        case wxSYS_HSCROLL_ARROW_X:
            // TODO
			return 0;
        case wxSYS_HSCROLL_ARROW_Y:
            // TODO
			return 0;
        case wxSYS_HTHUMB_X:
            // TODO
			return 0;
        case wxSYS_ICON_X:
            // TODO
			return 0;
        case wxSYS_ICON_Y:
            // TODO
			return 0;
        case wxSYS_ICONSPACING_X:
            // TODO
			return 0;
        case wxSYS_ICONSPACING_Y:
            // TODO
			return 0;
        case wxSYS_WINDOWMIN_X:
            // TODO
			return 0;
        case wxSYS_WINDOWMIN_Y:
            // TODO
			return 0;
        case wxSYS_SCREEN_X:
            // TODO
			return 0;
        case wxSYS_SCREEN_Y:
            // TODO
			return 0;
        case wxSYS_FRAMESIZE_X:
            // TODO
			return 0;
        case wxSYS_FRAMESIZE_Y:
            // TODO
			return 0;
        case wxSYS_SMALLICON_X:
            // TODO
			return 0;
        case wxSYS_SMALLICON_Y:
            // TODO
			return 0;
        case wxSYS_HSCROLL_Y:
            // TODO
			return 0;
        case wxSYS_VSCROLL_X:
            // TODO
			return 0;
        case wxSYS_VSCROLL_ARROW_X:
            // TODO
			return 0;
        case wxSYS_VSCROLL_ARROW_Y:
            // TODO
			return 0;
        case wxSYS_VTHUMB_Y:
            // TODO
			return 0;
        case wxSYS_CAPTION_Y:
            // TODO
			return 0;
        case wxSYS_MENU_Y:
            // TODO
			return 0;
        case wxSYS_NETWORK_PRESENT:
            // TODO
			return 0;
        case wxSYS_PENWINDOWS_PRESENT:
            // TODO
			return 0;
        case wxSYS_SHOW_SOUNDS:
            // TODO
			return 0;
        case wxSYS_SWAP_BUTTONS:
            // TODO
			return 0;
		default:
			return 0;
	}
	return 0;
}


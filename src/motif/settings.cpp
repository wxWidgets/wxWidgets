/////////////////////////////////////////////////////////////////////////////
// Name:        settings.cpp
// Purpose:     wxSettings
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// TODO: these settings should probably be configurable from some central or
// per-user file, which can be edited using a Windows-control-panel clone.
// Also they should be documented better. Some are very MS Windows-ish.

#ifdef __GNUG__
#pragma implementation "settings.h"
#endif

#include "wx/settings.h"
#include "wx/gdicmn.h"

wxColour wxSystemSettings::GetSystemColour(int index)
{
  switch (index)
  {
    case wxSYS_COLOUR_SCROLLBAR:
      //    case wxSYS_COLOUR_DESKTOP:          // Same as wxSYS_COLOUR_BACKGROUND
    case wxSYS_COLOUR_BACKGROUND:
    case wxSYS_COLOUR_ACTIVECAPTION:
    case wxSYS_COLOUR_INACTIVECAPTION:
    case wxSYS_COLOUR_MENU:
    case wxSYS_COLOUR_WINDOW:
    case wxSYS_COLOUR_WINDOWFRAME:
    case wxSYS_COLOUR_ACTIVEBORDER:
    case wxSYS_COLOUR_INACTIVEBORDER:
    case wxSYS_COLOUR_BTNFACE:
      //    case wxSYS_COLOUR_3DFACE:           // Same as wxSYS_COLOUR_BTNFACE
    case wxSYS_COLOUR_GRAYTEXT:
    {
        return wxColour("LIGHT GREY");
    }
    case wxSYS_COLOUR_BTNSHADOW:
      //    case wxSYS_COLOUR_3DSHADOW:         // Same as wxSYS_COLOUR_BTNSHADOW
    {
        return wxColour("GREY");
    }
    case wxSYS_COLOUR_3DDKSHADOW:
    {
        return *wxBLACK;
    }
    case wxSYS_COLOUR_HIGHLIGHT:
    case wxSYS_COLOUR_BTNHIGHLIGHT:
      //    case wxSYS_COLOUR_3DHIGHLIGHT:      // Same as wxSYS_COLOUR_BTNHIGHLIGHT
    {
        return *wxWHITE;
    }
    case wxSYS_COLOUR_3DLIGHT:
    {
        return wxColour("LIGHT GREY");
    }
    case wxSYS_COLOUR_MENUTEXT:
    case wxSYS_COLOUR_WINDOWTEXT:
    case wxSYS_COLOUR_CAPTIONTEXT:
    case wxSYS_COLOUR_INACTIVECAPTIONTEXT:
    case wxSYS_COLOUR_INFOTEXT:
    {
      return *wxBLACK;
    }
    case wxSYS_COLOUR_HIGHLIGHTTEXT:
    {
      return *wxBLUE;
    }
    case wxSYS_COLOUR_INFOBK:
    case wxSYS_COLOUR_APPWORKSPACE:
    {
        return wxColour("LIGHT GREY");
	//      return *wxWHITE;
    }
  }
  return *wxWHITE;
}

wxFont wxSystemSettings::GetSystemFont(int index)
{
    switch (index)
    {
        case wxSYS_SYSTEM_FIXED_FONT:
        {
            return wxFont(12, wxMODERN, wxNORMAL, wxNORMAL, FALSE);
            break;
        }
        case wxSYS_DEVICE_DEFAULT_FONT:
        case wxSYS_SYSTEM_FONT:
        case wxSYS_DEFAULT_GUI_FONT:
        default:
        {
            return wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, FALSE);
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


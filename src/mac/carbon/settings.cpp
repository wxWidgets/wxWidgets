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
	switch( index )
	{
		case wxSYS_COLOUR_SCROLLBAR :
		case wxSYS_COLOUR_BACKGROUND:
		case wxSYS_COLOUR_ACTIVECAPTION:
		case wxSYS_COLOUR_INACTIVECAPTION:
		case wxSYS_COLOUR_MENU:
		case wxSYS_COLOUR_WINDOW:
		case wxSYS_COLOUR_WINDOWFRAME:
		case wxSYS_COLOUR_ACTIVEBORDER:
		case wxSYS_COLOUR_INACTIVEBORDER:
		case wxSYS_COLOUR_BTNFACE:
			return wxColor( 0xDD , 0xDD , 0xDD ) ;
			break ;

		case wxSYS_COLOUR_BTNSHADOW:
			return wxColor( 0x44 , 0x44 , 0x44 ) ;
			break ;

		case wxSYS_COLOUR_BTNTEXT:
		case wxSYS_COLOUR_MENUTEXT:
		case wxSYS_COLOUR_WINDOWTEXT:
		case wxSYS_COLOUR_CAPTIONTEXT:
		case wxSYS_COLOUR_INFOTEXT:
		case wxSYS_COLOUR_INACTIVECAPTIONTEXT:
     		return *wxBLACK;
			break ;
		case wxSYS_COLOUR_HIGHLIGHT:
			{
				RGBColor hilite ;
				LMGetHiliteRGB(&hilite) ;
				return wxColor( hilite.red >> 8 , hilite.green >> 8  , hilite.blue >> 8  ) ;
			}
			break ;
		case wxSYS_COLOUR_BTNHIGHLIGHT:
		case wxSYS_COLOUR_GRAYTEXT:
			return wxColor( 0xCC , 0xCC , 0xCC ) ;
			break ;
	
		case wxSYS_COLOUR_3DDKSHADOW:
			return wxColor( 0x44 , 0x44 , 0x44 ) ;
			break ;
		case wxSYS_COLOUR_3DLIGHT:
			return wxColor( 0xCC , 0xCC , 0xCC ) ;
			break ;
		case wxSYS_COLOUR_HIGHLIGHTTEXT :
			{
				RGBColor hilite ;
				LMGetHiliteRGB(&hilite) ;
				if ( ( hilite.red + hilite.green + hilite.blue ) == 0 )
						return *wxWHITE ;
				else
						return *wxBLACK ;
			}
			break ;
		case wxSYS_COLOUR_INFOBK :
		case wxSYS_COLOUR_APPWORKSPACE:	
			return *wxWHITE ;
			break ;
	}
	return *wxWHITE;
}

wxFont wxSystemSettings::GetSystemFont(int index)
{
    switch (index)
    {
        case wxSYS_ANSI_VAR_FONT :
        case wxSYS_SYSTEM_FONT :
        case wxSYS_DEVICE_DEFAULT_FONT :
        case wxSYS_DEFAULT_GUI_FONT :
	        {
	        	return *wxSMALL_FONT ;
					} ;
        	break ;
        case wxSYS_OEM_FIXED_FONT :
        case wxSYS_ANSI_FIXED_FONT :
        case wxSYS_SYSTEM_FIXED_FONT :
        default :
	        {
	        	return *wxNORMAL_FONT ;
	 				} ;
	       	break ;
				
    }
    return *wxNORMAL_FONT;
}

// Get a system metric, e.g. scrollbar size
int wxSystemSettings::GetSystemMetric(int index)
{
	switch ( index)
	{
        case wxSYS_MOUSE_BUTTONS:
					return 2; // we emulate a two button mouse (ctrl + click = right button )
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
					return 16;
        case wxSYS_HSCROLL_ARROW_Y:
					return 16;
        case wxSYS_HTHUMB_X:
					return 16;
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
					return 16;
        case wxSYS_VSCROLL_X:
					return 16;
        case wxSYS_VSCROLL_ARROW_X:
					return 16;
        case wxSYS_VSCROLL_ARROW_Y:
					return 16;
        case wxSYS_VTHUMB_Y:
					return 16;
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
					return 0;
        case wxSYS_SHOW_SOUNDS:
            // TODO
			return 0;
        case wxSYS_SWAP_BUTTONS:
				return 0;
			default:
				return 0;
	}
	return 0;
}


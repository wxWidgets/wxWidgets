/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/settings.cpp
// Purpose:     wxSettings
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/settings.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/gdicmn.h"
#endif

#include "wx/mac/uma.h"

// ----------------------------------------------------------------------------
// wxSystemSettingsNative
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// colours
// ----------------------------------------------------------------------------

wxColour wxSystemSettingsNative::GetColour(wxSystemColour index)
{
    int major, minor;
    wxColour resultColor;
    RGBColor macRGB;
    ThemeBrush colorBrushID;

    wxGetOsVersion( &major, &minor );

    switch ( index )
    {
        case wxSYS_COLOUR_WINDOW:
            resultColor = *wxWHITE ;
            break ;
        case wxSYS_COLOUR_SCROLLBAR :
        case wxSYS_COLOUR_BACKGROUND:
        case wxSYS_COLOUR_ACTIVECAPTION:
        case wxSYS_COLOUR_INACTIVECAPTION:
        case wxSYS_COLOUR_MENU:
        case wxSYS_COLOUR_WINDOWFRAME:
        case wxSYS_COLOUR_ACTIVEBORDER:
        case wxSYS_COLOUR_INACTIVEBORDER:
        case wxSYS_COLOUR_BTNFACE:
        case wxSYS_COLOUR_MENUBAR:
            resultColor = wxColor( 0xDD, 0xDD, 0xDD );
            break ;

        case wxSYS_COLOUR_LISTBOX :
            if (major >= 10)
                resultColor = *wxWHITE ;
            else
                resultColor = wxColor( 0xEE, 0xEE, 0xEE );
            break ;

        case wxSYS_COLOUR_BTNSHADOW:
            if (major >= 10)
                resultColor = wxColor( 0xBE, 0xBE, 0xBE );
            else
                resultColor = wxColor( 0x44, 0x44, 0x44 );
            break ;

        case wxSYS_COLOUR_BTNTEXT:
        case wxSYS_COLOUR_MENUTEXT:
        case wxSYS_COLOUR_WINDOWTEXT:
        case wxSYS_COLOUR_CAPTIONTEXT:
        case wxSYS_COLOUR_INFOTEXT:
        case wxSYS_COLOUR_INACTIVECAPTIONTEXT:
        case wxSYS_COLOUR_LISTBOXTEXT:
            resultColor = *wxBLACK;
            break ;

        case wxSYS_COLOUR_HIGHLIGHT:

#if 0 && (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3)
            // NB: enable this case as desired
            colorBrushID = kThemeBrushAlternatePrimaryHighlightColor;
#else
            colorBrushID = kThemeBrushPrimaryHighlightColor;
#endif

            GetThemeBrushAsColor( colorBrushID, 32, true, &macRGB );
            resultColor = wxColor( macRGB.red >> 8, macRGB.green >> 8, macRGB.blue >> 8 );
            break ;

        case wxSYS_COLOUR_BTNHIGHLIGHT:
        case wxSYS_COLOUR_GRAYTEXT:
            resultColor = wxColor( 0xCC, 0xCC, 0xCC );
            break ;

        case wxSYS_COLOUR_3DDKSHADOW:
            resultColor = wxColor( 0x44, 0x44, 0x44 );
            break ;

        case wxSYS_COLOUR_3DLIGHT:
            resultColor = wxColor( 0xCC, 0xCC, 0xCC );
            break ;

        case wxSYS_COLOUR_HIGHLIGHTTEXT :
        case wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT :
#if 0 && (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3)
            // NB: enable this case as desired
            resultColor = *wxWHITE ;
#else
            GetThemeBrushAsColor( kThemeBrushPrimaryHighlightColor, 32, true, &macRGB );
            if ((macRGB.red + macRGB.green + macRGB.blue) == 0)
                resultColor = *wxWHITE ;
            else
                resultColor = *wxBLACK ;
#endif
            break ;

        case wxSYS_COLOUR_INFOBK :
            // we don't have a way to detect tooltip color, so use the
            // standard value used at least on 10.4:
            resultColor = wxColour( 0xFF, 0xFF, 0xD3 ) ;
            break ;
        case wxSYS_COLOUR_APPWORKSPACE:
            resultColor =  wxColor( 0x80, 0x80, 0x80 ); ;
            break ;

        case wxSYS_COLOUR_HOTLIGHT:
        case wxSYS_COLOUR_GRADIENTACTIVECAPTION:
        case wxSYS_COLOUR_GRADIENTINACTIVECAPTION:
        case wxSYS_COLOUR_MENUHILIGHT:
            // TODO:
            resultColor = *wxBLACK;
            break ;

        // case wxSYS_COLOUR_MAX:
        default:
            resultColor = *wxWHITE;
            // wxCHECK_MSG( index >= wxSYS_COLOUR_MAX, false, _T("unknown system colour index") );
            break ;
    }

    return resultColor;
}

// ----------------------------------------------------------------------------
// fonts
// ----------------------------------------------------------------------------

wxFont wxSystemSettingsNative::GetFont(wxSystemFont index)
{
    switch (index)
    {
        case wxSYS_ANSI_VAR_FONT :
        case wxSYS_SYSTEM_FONT :
        case wxSYS_DEVICE_DEFAULT_FONT :
        case wxSYS_DEFAULT_GUI_FONT :
            return *wxSMALL_FONT ;
            break ;

        default :
            break ;
    }

    return *wxNORMAL_FONT;
}

// ----------------------------------------------------------------------------
// system metrics/features
// ----------------------------------------------------------------------------

// Get a system metric, e.g. scrollbar size
int wxSystemSettingsNative::GetMetric(wxSystemMetric index, wxWindow* WXUNUSED(win))
{
    int value;

    switch ( index )
    {
        case wxSYS_MOUSE_BUTTONS:
            // we emulate a two button mouse (ctrl + click = right button)
            return 2;

        // TODO case wxSYS_BORDER_X:
        // TODO case wxSYS_BORDER_Y:
        // TODO case wxSYS_CURSOR_X:
        // TODO case wxSYS_CURSOR_Y:
        // TODO case wxSYS_DCLICK_X:
        // TODO case wxSYS_DCLICK_Y:
        // TODO case wxSYS_DRAG_X:
        // TODO case wxSYS_DRAG_Y:
        // TODO case wxSYS_EDGE_X:
        // TODO case wxSYS_EDGE_Y:

        case wxSYS_HSCROLL_ARROW_X:
        case wxSYS_HSCROLL_ARROW_Y:
        case wxSYS_HTHUMB_X:
            return 16;

        // TODO case wxSYS_ICON_X:
        // TODO case wxSYS_ICON_Y:
        // TODO case wxSYS_ICONSPACING_X:
        // TODO case wxSYS_ICONSPACING_Y:
        // TODO case wxSYS_WINDOWMIN_X:
        // TODO case wxSYS_WINDOWMIN_Y:

        case wxSYS_SCREEN_X:
            wxDisplaySize( &value, NULL );
            return value;

        case wxSYS_SCREEN_Y:
            wxDisplaySize( NULL, &value );
            return value;

        // TODO case wxSYS_FRAMESIZE_X:
        // TODO case wxSYS_FRAMESIZE_Y:
        // TODO case wxSYS_SMALLICON_X:
        // TODO case wxSYS_SMALLICON_Y:

        case wxSYS_HSCROLL_Y:
        case wxSYS_VSCROLL_X:
        case wxSYS_VSCROLL_ARROW_X:
        case wxSYS_VSCROLL_ARROW_Y:
        case wxSYS_VTHUMB_Y:
            return 16;

        case wxSYS_PENWINDOWS_PRESENT:
            return 0;

        case wxSYS_SWAP_BUTTONS:
            return 0;

        // TODO: case wxSYS_CAPTION_Y:
        // TODO: case wxSYS_MENU_Y:
        // TODO: case wxSYS_NETWORK_PRESENT:
        // TODO: case wxSYS_SHOW_SOUNDS:

        default:
            // unsupported metric
            break;
    }

    return -1;
}

bool wxSystemSettingsNative::HasFeature(wxSystemFeature index)
{
    switch (index)
    {
        case wxSYS_CAN_ICONIZE_FRAME:
        case wxSYS_CAN_DRAW_FRAME_DECORATIONS:
            return true;

        default:
            return false;
    }
}

/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/settings.mm
// Purpose:     wxSettings
// Author:      David Elliott
// Modified by: Tobias Taschner
// Created:     2005/01/11
// Copyright:   (c) 2005 David Elliott
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/settings.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/gdicmn.h"
#endif

#include "wx/osx/core/private.h"
#include "wx/osx/private/available.h"

#include "UIKit/UIKit.h"

#import <Foundation/Foundation.h>


static int wxOSXGetUserDefault(NSString* key, int defaultValue)
{
    NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
    if (!defaults)
    {
        return defaultValue;
    }

    id setting = [defaults objectForKey: key];
    if (!setting)
    {
        return defaultValue;
    }

    return [setting intValue];
}

// ----------------------------------------------------------------------------
// wxSystemAppearance
// ----------------------------------------------------------------------------

wxString wxSystemAppearance::GetName() const
{
    UITraitCollection* traitCollection = [UIScreen mainScreen].traitCollection;
    if (traitCollection.userInterfaceStyle == UIUserInterfaceStyleDark)
        return wxS("Dark");
    else if (traitCollection.userInterfaceStyle == UIUserInterfaceStyleLight)
        return wxT("Light");
    else
        return wxT("Unspecified");
}

bool wxSystemAppearance::IsDark() const
{
    UITraitCollection* traitCollection = [UIScreen mainScreen].traitCollection;
    if (traitCollection.userInterfaceStyle == UIUserInterfaceStyleDark)
        return true;

    return false;
}


// ----------------------------------------------------------------------------
// wxSystemSettingsNative
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// colours
// ----------------------------------------------------------------------------

wxColour wxSystemSettingsNative::GetColour(wxSystemColour index)
{
    UIColor* sysColor = nil;

    switch ( index )
    {
        case wxSYS_COLOUR_WINDOW:
            sysColor = [UIColor systemBackgroundColor];
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
            sysColor = [UIColor systemFillColor];
            break ;

        case wxSYS_COLOUR_LISTBOX :
            sysColor = [UIColor whiteColor];
            break ;

        case wxSYS_COLOUR_BTNSHADOW:
            sysColor = [UIColor systemFillColor];
            break ;

        case wxSYS_COLOUR_BTNTEXT:
        case wxSYS_COLOUR_MENUTEXT:
        case wxSYS_COLOUR_WINDOWTEXT:
        case wxSYS_COLOUR_CAPTIONTEXT:
        case wxSYS_COLOUR_INFOTEXT:
        case wxSYS_COLOUR_INACTIVECAPTIONTEXT:
        case wxSYS_COLOUR_LISTBOXTEXT:
            sysColor = [UIColor labelColor];
            break ;

        case wxSYS_COLOUR_HIGHLIGHT:
            {
            sysColor = [UIColor lightTextColor];
            }
            break ;

        case wxSYS_COLOUR_BTNHIGHLIGHT:
        case wxSYS_COLOUR_GRAYTEXT:
            sysColor = [UIColor lightTextColor];
            break ;

        case wxSYS_COLOUR_3DDKSHADOW:
            sysColor = [UIColor darkTextColor];
            break ;

        case wxSYS_COLOUR_3DLIGHT:
            sysColor = [UIColor lightTextColor];
            break ;

        case wxSYS_COLOUR_HIGHLIGHTTEXT :
        case wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT :
            sysColor = [UIColor whiteColor];
            break ;

        case wxSYS_COLOUR_INFOBK :
            // we don't have a way to detect tooltip color, so use the
            // standard value used at least on 10.4:
            sysColor = [UIColor systemFillColor];
            break ;
        case wxSYS_COLOUR_APPWORKSPACE:
            sysColor = [UIColor systemBackgroundColor];
            break ;

        case wxSYS_COLOUR_HOTLIGHT:
        case wxSYS_COLOUR_GRADIENTACTIVECAPTION:
        case wxSYS_COLOUR_GRADIENTINACTIVECAPTION:
        case wxSYS_COLOUR_MENUHILIGHT:
            // TODO:
            sysColor = [UIColor whiteColor];
            break ;

        default:
            if(index>=wxSYS_COLOUR_MAX)
            {
                wxFAIL_MSG(wxT("Invalid system colour index"));
                return wxColour();
            }

            sysColor = [UIColor whiteColor];
            break ;
    }

    return wxColour(sysColor);
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

// ----------------------------------------------------------------------------
// system metrics/features
// ----------------------------------------------------------------------------

// Get a system metric, e.g. scrollbar size
int wxSystemSettingsNative::GetMetric(wxSystemMetric index, const wxWindow* WXUNUSED(win))
{
    int value;

    switch ( index )
    {
        case wxSYS_MOUSE_BUTTONS:
                    return 2; // we emulate a two button mouse (ctrl + click = right button )

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
            return 16;
        case wxSYS_HSCROLL_ARROW_Y:
            return 16;
        case wxSYS_HTHUMB_X:
            return 16;

        // TODO case wxSYS_ICON_X:
        // TODO case wxSYS_ICON_Y:
        // TODO case wxSYS_ICONSPACING_X:
        // TODO case wxSYS_ICONSPACING_Y:
        // TODO case wxSYS_WINDOWMIN_X:
        // TODO case wxSYS_WINDOWMIN_Y:

        case wxSYS_SCREEN_X:
            wxDisplaySize(&value, nullptr);
            return value;

        case wxSYS_SCREEN_Y:
            wxDisplaySize(nullptr, &value);
            return value;

        // TODO case wxSYS_FRAMESIZE_X:
        // TODO case wxSYS_FRAMESIZE_Y:
        // TODO case wxSYS_SMALLICON_X:
        // TODO case wxSYS_SMALLICON_Y:

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

        // TODO case wxSYS_CAPTION_Y:
        // TODO case wxSYS_MENU_Y:
        // TODO case wxSYS_NETWORK_PRESENT:

        case wxSYS_PENWINDOWS_PRESENT:
            return 0;

        // TODO case wxSYS_SHOW_SOUNDS:

        case wxSYS_SWAP_BUTTONS:
            return 0;

        case wxSYS_DCLICK_MSEC:
            // default on mac is 30 ticks, we shouldn't really use wxSYS_DCLICK_MSEC anyway
            // but rather rely on the 'click-count' by the system delivered in a mouse event
            return 500;

        default:
            return -1;  // unsupported metric
    }
    return 0;
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

/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/settings.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/settings.h"
#include "wx/qt/converter.h"
#include <QtGui/QPalette>

wxColour wxSystemSettingsNative::GetColour(wxSystemColour index)
{
    QPalette pal = qApp->palette();
    QColor color;
    switch (index)
    {
        case wxSYS_COLOUR_SCROLLBAR:
        case wxSYS_COLOUR_BACKGROUND:
        //case wxSYS_COLOUR_DESKTOP:
        case wxSYS_COLOUR_INACTIVECAPTION:
        case wxSYS_COLOUR_MENU:
        case wxSYS_COLOUR_WINDOWFRAME:
        case wxSYS_COLOUR_ACTIVEBORDER:
        case wxSYS_COLOUR_INACTIVEBORDER:
            color = pal.color(QPalette::Window);
            break;

        //case wxSYS_COLOUR_3DFACE:
        case wxSYS_COLOUR_3DLIGHT:
            color = pal.color(QPalette::Light);
            break;

        case wxSYS_COLOUR_BTNFACE:
            color = pal.color(QPalette::Button);
            break;

        case wxSYS_COLOUR_WINDOW:
            color = pal.color(QPalette::Base);
            break;

        case wxSYS_COLOUR_MENUBAR:
            color = pal.window().color();
            break;

        case wxSYS_COLOUR_3DDKSHADOW:
            color = pal.color(QPalette::Shadow);
            break;

        case wxSYS_COLOUR_BTNSHADOW:
            color = pal.color(QPalette::Dark);
            break;

        case wxSYS_COLOUR_GRAYTEXT:
        //case wxSYS_COLOUR_3DSHADOW:
            color = pal.color(QPalette::Disabled, QPalette::Text);
            break;

        case wxSYS_COLOUR_BTNHIGHLIGHT:
        //case wxSYS_COLOUR_BTNHILIGHT:
        //case wxSYS_COLOUR_3DHIGHLIGHT:
        //case wxSYS_COLOUR_3DHILIGHT:
            color = pal.color(QPalette::Light);
            break;

        case wxSYS_COLOUR_HIGHLIGHT:
            color = pal.highlight().color();
            break;

        case wxSYS_COLOUR_LISTBOX:
            color = pal.base().color();
            break;

        case wxSYS_COLOUR_LISTBOXTEXT:
            color = pal.color(QPalette::WindowText);
            break;

        case wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT:
            // This is for the text in a list control (or tree) when the
            // item is selected, but not focused
            color = pal.color(QPalette::HighlightedText);
            break;

        case wxSYS_COLOUR_WINDOWTEXT:
            color = pal.color(QPalette::Text);
            break;

        case wxSYS_COLOUR_MENUTEXT:
        case wxSYS_COLOUR_CAPTIONTEXT:
        case wxSYS_COLOUR_INACTIVECAPTIONTEXT:
        case wxSYS_COLOUR_BTNTEXT:
            color = pal.color(QPalette::ButtonText);
            break;

        case wxSYS_COLOUR_INFOBK:
            color = pal.color(QPalette::ToolTipBase);
            break;

        case wxSYS_COLOUR_INFOTEXT:
            color = pal.color(QPalette::ToolTipText);
            break;

        case wxSYS_COLOUR_HIGHLIGHTTEXT:
            color = pal.color(QPalette::BrightText);
            break;

        case wxSYS_COLOUR_APPWORKSPACE:
            color = QColor(Qt::white);    // ?
            break;

        case wxSYS_COLOUR_ACTIVECAPTION:
        case wxSYS_COLOUR_MENUHILIGHT:
            color = pal.color(QPalette::Highlight);
            break;

        case wxSYS_COLOUR_HOTLIGHT:
        case wxSYS_COLOUR_GRADIENTACTIVECAPTION:
        case wxSYS_COLOUR_GRADIENTINACTIVECAPTION:
            // TODO
            color = QColor(Qt::black);
            break;

        case wxSYS_COLOUR_MAX:
        default:
            wxFAIL_MSG( wxT("unknown system colour index") );
            color = QColor(Qt::white);
            break;
    }

    return wxColor( color.red(), color.green(), color.blue(), color.alpha() );
}

wxFont wxSystemSettingsNative::GetFont(wxSystemFont index)
{
    wxFont font;
    switch (index)
    {
        case wxSYS_OEM_FIXED_FONT:
        case wxSYS_ANSI_FIXED_FONT:
        case wxSYS_SYSTEM_FIXED_FONT:
            font = *wxNORMAL_FONT;
            break;

        case wxSYS_ANSI_VAR_FONT:
        case wxSYS_SYSTEM_FONT:
        case wxSYS_DEVICE_DEFAULT_FONT:
        case wxSYS_DEFAULT_GUI_FONT:
        default:
            break;
    }

    wxASSERT( font.IsOk() );

    return font;

}

int wxSystemSettingsNative::GetMetric(wxSystemMetric index, wxWindow * win)
{
    return 0;
}

bool wxSystemSettingsNative::HasFeature(wxSystemFeature index)
{
    return false;
}


/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/settings.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "settings.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/settings.h"
#include "wx/debug.h"
#include "wx/cmndata.h"
#include "wx/fontutil.h"

#include <gdk/gdk.h>
#include <gdk/gdkprivate.h>
#include <gtk/gtk.h>

#define SHIFT (8*(sizeof(short int)-sizeof(char)))

// ----------------------------------------------------------------------------
// wxSystemObjects
// ----------------------------------------------------------------------------

struct wxSystemObjects
{
    wxColour m_colBtnFace,
             m_colBtnShadow,
             m_colBtnHighlight,
             m_colHighlight,
             m_colHighlightText,
             m_colListBox,
             m_colBtnText,
             m_colMenuItemHighlight;

    wxFont m_fontSystem;
};

static wxSystemObjects gs_objects;

// ----------------------------------------------------------------------------
// wxSystemSettings implementation
// ----------------------------------------------------------------------------

// kind of widget to use in GetColourFromGTKWidget
enum wxGtkWidgetType
{
    wxGTK_BUTTON,
    wxGTK_LIST,
    wxGTK_MENUITEM
};

// the colour we need
enum wxGtkColourType
{
    wxGTK_FG,
    wxGTK_BG,
    wxGTK_BASE
};

// wxSystemSettings::GetColour() helper: get the colours from a GTK+
// widget style, return true if we did get them, false to use defaults
static bool GetColourFromGTKWidget(int& red, int& green, int& blue,
                                   wxGtkWidgetType type = wxGTK_BUTTON,
                                   GtkStateType state = GTK_STATE_NORMAL,
                                   wxGtkColourType colour = wxGTK_BG)
{
    GtkWidget *widget;
    switch ( type )
    {
        default:
            wxFAIL_MSG( _T("unexpected GTK widget type") );
            // fall through

        case wxGTK_BUTTON:
            widget = gtk_button_new();
            break;

        case wxGTK_LIST:
            widget = gtk_list_new();
	
        case wxGTK_MENUITEM:
            widget = gtk_menu_item_new();
    }

    GtkStyle *def = gtk_rc_get_style( widget );
    if ( !def )
        def = gtk_widget_get_default_style();

    bool ok;
    if ( def )
    {
        GdkColor *col;
        switch ( colour )
        {
            default:
                wxFAIL_MSG( _T("unexpected GTK colour type") );
                // fall through

            case wxGTK_FG:
                col = def->fg;
                break;

            case wxGTK_BG:
                col = def->bg;
                break;

            case wxGTK_BASE:
                col = def->base;
                break;
        }

        red = col[state].red;
        green = col[state].green;
        blue = col[state].blue;

        ok = TRUE;
    }
    else
    {
        ok = FALSE;
    }

    gtk_widget_destroy( widget );

    return ok;
}

wxColour wxSystemSettingsNative::GetColour( wxSystemColour index )
{
    switch (index)
    {
        case wxSYS_COLOUR_SCROLLBAR:
        case wxSYS_COLOUR_BACKGROUND:
        case wxSYS_COLOUR_INACTIVECAPTION:
        case wxSYS_COLOUR_MENU:
        case wxSYS_COLOUR_WINDOWFRAME:
        case wxSYS_COLOUR_ACTIVEBORDER:
        case wxSYS_COLOUR_INACTIVEBORDER:
        case wxSYS_COLOUR_BTNFACE:
        case wxSYS_COLOUR_MENUBAR:
        case wxSYS_COLOUR_3DLIGHT:
            if (!gs_objects.m_colBtnFace.Ok())
            {
                int red, green, blue;
                if ( !GetColourFromGTKWidget(red, green, blue) )
                {
                    red =
                    green = 0;
                    blue = 0x9c40;
                }

                gs_objects.m_colBtnFace = wxColour( red   >> SHIFT,
                                                   green >> SHIFT,
                                                   blue  >> SHIFT );
            }
            return gs_objects.m_colBtnFace;

        case wxSYS_COLOUR_WINDOW:
            return *wxWHITE;

        case wxSYS_COLOUR_3DDKSHADOW:
            return *wxBLACK;

        case wxSYS_COLOUR_GRAYTEXT:
        case wxSYS_COLOUR_BTNSHADOW:
        //case wxSYS_COLOUR_3DSHADOW:
            if (!gs_objects.m_colBtnShadow.Ok())
            {
                wxColour faceColour(GetColour(wxSYS_COLOUR_3DFACE));
                gs_objects.m_colBtnShadow =
                   wxColour((unsigned char) (faceColour.Red() * 0.666),
                            (unsigned char) (faceColour.Green() * 0.666),
                            (unsigned char) (faceColour.Blue() * 0.666));
            }

            return gs_objects.m_colBtnShadow;

        case wxSYS_COLOUR_3DHIGHLIGHT:
        //case wxSYS_COLOUR_BTNHIGHLIGHT:
            return * wxWHITE;

        case wxSYS_COLOUR_HIGHLIGHT:
            if (!gs_objects.m_colHighlight.Ok())
            {
                int red, green, blue;
                if ( !GetColourFromGTKWidget(red, green, blue,
                                             wxGTK_BUTTON,
                                             GTK_STATE_SELECTED) )
                {
                    red =
                    green = 0;
                    blue = 0x9c40;
                }

                gs_objects.m_colHighlight = wxColour( red   >> SHIFT,
                                                        green >> SHIFT,
                                                        blue  >> SHIFT );
            }
            return gs_objects.m_colHighlight;

        case wxSYS_COLOUR_LISTBOX:
            if (!gs_objects.m_colListBox.Ok())
            {
                int red, green, blue;
                if ( GetColourFromGTKWidget(red, green, blue,
                                            wxGTK_LIST,
                                            GTK_STATE_NORMAL,
                                            wxGTK_BASE) )
                {
                    gs_objects.m_colListBox = wxColour( red   >> SHIFT,
                                                          green >> SHIFT,
                                                          blue  >> SHIFT );
                }
                else
                {
                    gs_objects.m_colListBox = wxColour(*wxWHITE);
                }
            }
            return gs_objects.m_colListBox;

        case wxSYS_COLOUR_MENUTEXT:
        case wxSYS_COLOUR_WINDOWTEXT:
        case wxSYS_COLOUR_CAPTIONTEXT:
        case wxSYS_COLOUR_INACTIVECAPTIONTEXT:
        case wxSYS_COLOUR_BTNTEXT:
        case wxSYS_COLOUR_INFOTEXT:
            if (!gs_objects.m_colBtnText.Ok())
            {
                int red, green, blue;
                if ( !GetColourFromGTKWidget(red, green, blue,
                                             wxGTK_BUTTON,
                                             GTK_STATE_NORMAL,
                                             wxGTK_FG) )
                {
                    red =
                    green =
                    blue = 0;
                }

                gs_objects.m_colBtnText = wxColour( red   >> SHIFT,
                                                      green >> SHIFT,
                                                      blue  >> SHIFT );
            }
            return gs_objects.m_colBtnText;

            // this (as well as wxSYS_COLOUR_INFOTEXT above) is used for
            // tooltip windows - Robert, please change this code to use the
            // real GTK tooltips when/if you can (TODO)
        case wxSYS_COLOUR_INFOBK:
            return wxColour(255, 255, 225);

        case wxSYS_COLOUR_HIGHLIGHTTEXT:
            if (!gs_objects.m_colHighlightText.Ok())
            {
                wxColour hclr = GetColour(wxSYS_COLOUR_HIGHLIGHT);
                if (hclr.Red() > 200 && hclr.Green() > 200 && hclr.Blue() > 200)
                    gs_objects.m_colHighlightText = wxColour(*wxBLACK);
                else
                    gs_objects.m_colHighlightText = wxColour(*wxWHITE);
            }
            return gs_objects.m_colHighlightText;

        case wxSYS_COLOUR_APPWORKSPACE:
            return *wxWHITE;    // ?

        case wxSYS_COLOUR_ACTIVECAPTION:
        case wxSYS_COLOUR_MENUHILIGHT:
            if (!gs_objects.m_colMenuItemHighlight.Ok())
            {
                int red, green, blue;
                if ( !GetColourFromGTKWidget(red, green, blue,
                                             wxGTK_MENUITEM,
                                             GTK_STATE_SELECTED,
                                             wxGTK_BG) )
                {
                    red =
                    green =
                    blue = 0;
                }

                gs_objects.m_colMenuItemHighlight = wxColour( red  >> SHIFT,
                                                              green >> SHIFT,
                                                              blue  >> SHIFT );
            }
            return gs_objects.m_colMenuItemHighlight;

        case wxSYS_COLOUR_HOTLIGHT:
        case wxSYS_COLOUR_GRADIENTACTIVECAPTION:
        case wxSYS_COLOUR_GRADIENTINACTIVECAPTION:
            // TODO
            return *wxBLACK;

        case wxSYS_COLOUR_MAX:
        default:
            wxFAIL_MSG( _T("unknown system colour index") );
  }

  return *wxWHITE;
}

wxFont wxSystemSettingsNative::GetFont( wxSystemFont index )
{
    switch (index)
    {
        case wxSYS_OEM_FIXED_FONT:
        case wxSYS_ANSI_FIXED_FONT:
        case wxSYS_SYSTEM_FIXED_FONT:
        {
            return *wxNORMAL_FONT;
        }
        case wxSYS_ANSI_VAR_FONT:
        case wxSYS_SYSTEM_FONT:
        case wxSYS_DEVICE_DEFAULT_FONT:
        case wxSYS_DEFAULT_GUI_FONT:
        {
            if (!gs_objects.m_fontSystem.Ok())
            {
#ifdef __WXGTK20__
                GtkWidget *widget = gtk_button_new();
                GtkStyle *def = gtk_rc_get_style( widget );
                if ( !def || !def->font_desc )
                    def = gtk_widget_get_default_style();  
                if ( def && def->font_desc )
                {  
                    wxNativeFontInfo info;  
                    info.description = 
                        pango_font_description_copy(def->font_desc);
                    gs_objects.m_fontSystem = wxFont(info);  
                }  
                else  
                {  
                    GtkSettings *settings = gtk_settings_get_default();
                    gchar *font_name = NULL;
                    g_object_get ( settings,
                                   "gtk-font-name", 
                                   &font_name,
                                   NULL);
                    if (!font_name)
                        gs_objects.m_fontSystem = wxFont( 12, wxSWISS, wxNORMAL, wxNORMAL );
                    else
                        gs_objects.m_fontSystem = wxFont(wxString::FromAscii(font_name));
                    g_free (font_name);
                }  
                gtk_widget_destroy( widget );
#else
                gs_objects.m_fontSystem = wxFont( 12, wxSWISS, wxNORMAL, wxNORMAL );
#endif
            }
            return gs_objects.m_fontSystem;
        }

        default:
            return wxNullFont;
    }
}

int wxSystemSettingsNative::GetMetric( wxSystemMetric index )
{
    switch (index)
    {
        case wxSYS_SCREEN_X:   return gdk_screen_width();
        case wxSYS_SCREEN_Y:   return gdk_screen_height();
        case wxSYS_HSCROLL_Y:  return 15;
        case wxSYS_VSCROLL_X:  return 15;

#if defined(__WXGTK20__) && GTK_CHECK_VERSION(2, 4, 0)
        case wxSYS_DCLICK_X:
        case wxSYS_DCLICK_Y:
            gint dclick_distance;
            g_object_get(gtk_settings_get_default(), "gtk-double-click-distance", &dclick_distance, NULL);
            return dclick_distance * 2;
#endif

#if defined(__WXGTK20__)
        case wxSYS_DRAG_X:
        case wxSYS_DRAG_Y:
            gint drag_threshold;
            g_object_get(gtk_settings_get_default(), "gtk-dnd-drag-threshold", &drag_threshold, NULL);
            return drag_threshold * 2;
#endif

        // VZ: is there any way to get the cursor size with GDK?
        // Mart Raudsepp: Yes, there is a way to get the default cursor size for a display ever since GDK 2.4
#if defined(__WXGTK20__) && GTK_CHECK_VERSION(2, 4, 0)
        case wxSYS_CURSOR_X:
        case wxSYS_CURSOR_Y:
            return gdk_display_get_default_cursor_size(gdk_display_get_default());
#else
        case wxSYS_CURSOR_X:   return 16;
        case wxSYS_CURSOR_Y:   return 16;
#endif
        // MBN: ditto for icons
        case wxSYS_ICON_X:     return 32;
        case wxSYS_ICON_Y:     return 32;
        default:               
            return -1;   // metric is unknown
    }
}

bool wxSystemSettingsNative::HasFeature(wxSystemFeature index)
{
    switch (index)
    {
        case wxSYS_CAN_ICONIZE_FRAME: 
            return FALSE; 
            break;
        case wxSYS_CAN_DRAW_FRAME_DECORATIONS:
            return TRUE; 
            break;
        default:
            return FALSE;
    }
}

/////////////////////////////////////////////////////////////////////////////
// Name:        settings.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "settings.h"
#endif

#include "wx/settings.h"
#include "wx/debug.h"

#include "gdk/gdk.h"
#include "gtk/gtk.h"

/*
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
*/

#define SHIFT (8*(sizeof(short int)-sizeof(char)))

wxColour *g_systemBtnFaceColour      = (wxColour *) NULL;
wxColour *g_systemBtnShadowColour    = (wxColour *) NULL;
wxColour *g_systemBtnHighlightColour = (wxColour *) NULL;
wxColour *g_systemHighlightColour    = (wxColour *) NULL;

wxFont *g_systemFont = (wxFont *) NULL;

void wxSystemSettings::Done() 
{
    wxDELETE(g_systemBtnFaceColour);
    wxDELETE(g_systemBtnShadowColour);
    wxDELETE(g_systemBtnHighlightColour);
    wxDELETE(g_systemHighlightColour);
    wxDELETE(g_systemFont);
}

wxColour wxSystemSettings::GetSystemColour( int index )
{
  switch (index)
  {
    case wxSYS_COLOUR_SCROLLBAR:
    case wxSYS_COLOUR_BACKGROUND:
    case wxSYS_COLOUR_ACTIVECAPTION:
    case wxSYS_COLOUR_INACTIVECAPTION:
    case wxSYS_COLOUR_MENU:
    case wxSYS_COLOUR_WINDOW:
    case wxSYS_COLOUR_WINDOWFRAME:
    case wxSYS_COLOUR_ACTIVEBORDER:
    case wxSYS_COLOUR_INACTIVEBORDER:
    case wxSYS_COLOUR_BTNFACE:
    {
      GtkStyle *style = gtk_widget_get_default_style();
      if (!g_systemBtnFaceColour)
      {
        g_systemBtnFaceColour = 
	  new wxColour( style->bg[0].red >> SHIFT,
	                style->bg[0].green >> SHIFT,
			style->bg[0].blue >> SHIFT );
      }
      return *g_systemBtnFaceColour;
    }
    case wxSYS_COLOUR_BTNSHADOW:
    {
      GtkStyle *style = gtk_widget_get_default_style();
      if (!g_systemBtnShadowColour)
      {
        g_systemBtnShadowColour = 
	  new wxColour( style->dark[0].red >> SHIFT,
	                style->dark[0].green >> SHIFT,
			style->dark[0].blue >> SHIFT );
      }
      return *g_systemBtnShadowColour;
    }
    case wxSYS_COLOUR_GRAYTEXT:
    case wxSYS_COLOUR_BTNHIGHLIGHT:
    {
      GtkStyle *style = gtk_widget_get_default_style();
      if (!g_systemBtnHighlightColour)
      {
        g_systemBtnHighlightColour = 
	  new wxColour( style->light[0].red >> SHIFT,
	                style->light[0].green >> SHIFT,
			style->light[0].blue >> SHIFT );
      }
      return *g_systemBtnHighlightColour;
    }
    case wxSYS_COLOUR_HIGHLIGHT:
    {
      GtkStyle *style = gtk_widget_get_default_style();
      if (!g_systemHighlightColour)
      {
        g_systemHighlightColour = 
	  new wxColour( style->bg[GTK_STATE_SELECTED].red >> SHIFT,
	                style->bg[GTK_STATE_SELECTED].green >> SHIFT,
			style->bg[GTK_STATE_SELECTED].blue >> SHIFT );
      }
      return *g_systemHighlightColour;
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
      return *wxWHITE;
    }
    case wxSYS_COLOUR_INFOBK:
    case wxSYS_COLOUR_APPWORKSPACE:
    {
      return *wxWHITE;    // ?
    }
  }
  return *wxWHITE;
}

wxFont wxSystemSettings::GetSystemFont( int index ) 
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
            if (!g_systemFont)
                g_systemFont = new wxFont( 12, wxSWISS, wxNORMAL, wxNORMAL );
            return *g_systemFont;
        }
    }

    return wxNullFont;
}

int wxSystemSettings::GetSystemMetric( int index )
{
    switch (index)
    {
        case wxSYS_SCREEN_X:   return gdk_screen_width();
        case wxSYS_SCREEN_Y:   return gdk_screen_height();
        case wxSYS_HSCROLL_Y:  return 15;
        case wxSYS_VSCROLL_X:  return 15;
    }
    
    wxCHECK_MSG( index, 0, "wxSystemSettings::GetSystemMetric not fully implemented" );
    
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Name:        settings.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "settings.h"
#endif

#include "wx/settings.h"

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

wxColour *g_systemBtnFaceColour      = NULL;
wxColour *g_systemBtnShadowColour    = NULL;
wxColour *g_systemBtnHighlightColour = NULL;
wxColour *g_systemHighlightColour    = NULL;

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
      return *g_systemBtnFaceColour;
    };
    case wxSYS_COLOUR_BTNSHADOW:
    {
      return *g_systemBtnShadowColour;
    };
    case wxSYS_COLOUR_GRAYTEXT:
    case wxSYS_COLOUR_BTNHIGHLIGHT:
    {
      return *g_systemBtnHighlightColour;
    };
    case wxSYS_COLOUR_HIGHLIGHT:
    {
      return *g_systemHighlightColour;
    };
    case wxSYS_COLOUR_MENUTEXT:
    case wxSYS_COLOUR_WINDOWTEXT:
    case wxSYS_COLOUR_CAPTIONTEXT:
    case wxSYS_COLOUR_INACTIVECAPTIONTEXT:
    case wxSYS_COLOUR_INFOTEXT:
    {
      return *wxBLACK;
    };
    case wxSYS_COLOUR_HIGHLIGHTTEXT:
    {
      return *wxWHITE;
    };
    case wxSYS_COLOUR_INFOBK:
    case wxSYS_COLOUR_APPWORKSPACE:
    {
      return *wxWHITE;    // ?
    };
  };
  return *wxWHITE;
};

wxFont *g_systemFont = NULL;

wxFont wxSystemSettings::GetSystemFont( int index ) 
{
  switch (index)
  {
    case wxSYS_OEM_FIXED_FONT:
    case wxSYS_ANSI_FIXED_FONT:
    case wxSYS_SYSTEM_FIXED_FONT:
    {
      return *wxNORMAL_FONT;
    };
    case wxSYS_ANSI_VAR_FONT:
    case wxSYS_SYSTEM_FONT:
    case wxSYS_DEVICE_DEFAULT_FONT:
    case wxSYS_DEFAULT_GUI_FONT:
    {
      return *g_systemFont;
    };
  };
  return wxNullFont;
};

int wxSystemSettings::GetSystemMetric( int index )
{
  switch (index)
  {
    case wxSYS_SCREEN_X: return 0;
    case wxSYS_SCREEN_Y: return 0;
  };
  return 0;
};


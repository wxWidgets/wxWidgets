/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/settings.h
// Purpose:     wxSystemSettings class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SETTINGS_H_
#define _WX_SETTINGS_H_

#ifdef __GNUG__
#pragma interface "settings.h"
#endif

#include "wx/colour.h"
#include "wx/font.h"

class WXDLLEXPORT wxSystemSettings : public wxObject
{
public:
    wxSystemSettings() { }

    // Get a system colour
    static wxColour GetSystemColour(int index);

    // Get a system font
    static wxFont GetSystemFont(int index);

    // Get a system metric, e.g. scrollbar size
    static int GetSystemMetric(int index);

    // User-customizable hints to wxWindows or associated libraries
    // These could also be used to influence GetSystem... calls, indeed
    // to implement SetSystemColour/Font/Metric

    static void SetOption(const wxString& name, const wxString& value);
    static void SetOption(const wxString& name, int value);
    static wxString GetOption(const wxString& name) ;
    static int GetOptionInt(const wxString& name) ;
    static bool HasOption(const wxString& name) ;
};

#endif
    // _WX_SETTINGS_H_

/////////////////////////////////////////////////////////////////////////////
// Name:        settings.h
// Purpose:     wxSystemSettings class
// Author:      David Webster
// Modified by:
// Created:     10/15/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SETTINGS_H_
#define _WX_SETTINGS_H_

#include "wx/setup.h"
#include "wx/colour.h"
#include "wx/font.h"


class WXDLLEXPORT wxSystemSettings: public wxObject
{
public:
    inline wxSystemSettings() {}
    static wxColour    GetSystemColour(int index);
    static wxFont      GetSystemFont(int index);
    static int         GetSystemMetric(int index);
    static void        SetOption( const wxString& rsName
                                 ,const wxString& rsValue
                                );
    static void        SetOption( const wxString& rsName
                                 ,int             nValue
                                );
    static wxString    GetOption(const wxString& rsName) ;
    static int         GetOptionInt(const wxString& rsName) ;
    static bool        HasOption(const wxString& rsName) ;
}; // end of CLASS wxSystemSettings

#endif
    // _WX_SETTINGS_H_

/////////////////////////////////////////////////////////////////////////////
// Name:        sysopt.h
// Purpose:     wxSystemOptions
// Author:      Julian Smart
// Modified by:
// Created:     2001-07-10
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SYSOPT_H_
#define _WX_SYSOPT_H_

#include "wx/object.h"

#if wxUSE_SYSTEM_OPTIONS

// ----------------------------------------------------------------------------
// Enables an application to influence the wxWidgets implementation
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxSystemOptions : public wxObject
{
public:
    wxSystemOptions() { }

    // User-customizable hints to wxWidgets or associated libraries
    // These could also be used to influence GetSystem... calls, indeed
    // to implement SetSystemColour/Font/Metric

    static void SetOption(const wxString& name, const wxString& value);
    static void SetOption(const wxString& name, int value);
    static wxString GetOption(const wxString& name) ;
    static int GetOptionInt(const wxString& name) ;
    static bool HasOption(const wxString& name) ;
};

#endif


#endif
    // _WX_SYSOPT_H_


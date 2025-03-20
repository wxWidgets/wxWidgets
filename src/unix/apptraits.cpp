///////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/apptraits.cpp
// Purpose:     implementation of wxGUIAppTraits for Unix systems
// Author:      Vadim Zeitlin
// Created:     2008-03-22
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/apptrait.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
#endif // WX_PRECOMP

#include "wx/unix/private/execute.h"
#include "wx/evtloop.h"

#ifdef __WXGTK__
    #include "wx/sysopt.h"
#endif

#if wxUSE_DETECT_SM
    #include <X11/SM/SMlib.h>

    #include "wx/unix/utilsx11.h"
#endif

#if wxUSE_DETECT_SM
static wxString GetSM()
{
    wxX11Display dpy;
    if ( !dpy )
        return wxEmptyString;

    char smerr[256];
    char *client_id;
    SmcConn smc_conn = SmcOpenConnection(nullptr, nullptr,
                                         999, 999,
                                         0 /* mask */, nullptr /* callbacks */,
                                         nullptr, &client_id,
                                         WXSIZEOF(smerr), smerr);

    if ( !smc_conn )
    {
        // Don't report error if there is no session manager at all
        if (getenv("SESSION_MANAGER"))
        {
            wxLogDebug("Failed to connect to session manager: %s", smerr);
        }
        return wxEmptyString;
    }

    char *vendor = SmcVendor(smc_conn);
    wxString ret = wxString::FromAscii( vendor );
    free(vendor);

    SmcCloseConnection(smc_conn, 0, nullptr);
    free(client_id);

    return ret;
}
#endif // wxUSE_DETECT_SM

// ============================================================================
// implementation
// ============================================================================

int wxGUIAppTraits::WaitForChild(wxExecuteData& execData)
{
    // prepare to wait for the child termination: show to the user that we're
    // busy and refuse all input unless explicitly told otherwise
    wxBusyCursor bc;
    wxWindowDisabler wd(!(execData.m_flags & wxEXEC_NODISABLE));

    // Allocate an event loop that will be used to wait for the process
    // to terminate, will handle stdout, stderr, and any other events and pass
    // it to the common (to console and GUI) code which will run it.
    wxGUIEventLoop loop;
    return RunLoopUntilChildExit(execData, loop);
}

wxString wxGUIAppTraits::GetDesktopEnvironment() const
{
    wxString de;

#ifdef __WXGTK__
    de = wxSystemOptions::GetOption(wxT("gtk.desktop"));
    if (!de.empty())
        return de;
#endif

    de = wxGetenv(wxS("XDG_CURRENT_DESKTOP"));
    if (!de.empty())
    {
        // Can be a colon separated list according to
        // https://wiki.archlinux.org/title/Environment_variables#Examples
        de = de.BeforeFirst(':');
    }
#if wxUSE_DETECT_SM
    if ( de.empty() )
    {
        static const wxString s_SM(GetSM());
        de = s_SM;
        de.Replace(wxS("-session"), wxString());
    }
#endif // wxUSE_DETECT_SM

    de.MakeUpper();
    if (de.Contains(wxS("GNOME")))
        de = wxS("GNOME");
    else if (de.Contains(wxS("KDE")))
        de = wxS("KDE");
    else if (de.Contains(wxS("XFCE")))
        de = wxS("XFCE");

    return de;
}

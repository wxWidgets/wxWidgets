/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/settcmn.cpp
// Purpose:     common (to all ports) wxWindow functions
// Author:      Robert Roebling
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/settings.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
#endif //WX_PRECOMP

// ----------------------------------------------------------------------------
// static data
// ----------------------------------------------------------------------------

wxSystemScreenType wxSystemSettings::ms_screen = wxSYS_SCREEN_NONE;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

wxSystemScreenType wxSystemSettings::GetScreenType()
{
    if (ms_screen == wxSYS_SCREEN_NONE)
    {
        // wxUniv will be used on small devices, too.
        int x = GetMetric( wxSYS_SCREEN_X );

        ms_screen = wxSYS_SCREEN_DESKTOP;

        if (x < 800)
            ms_screen = wxSYS_SCREEN_SMALL;

        if (x < 640)
            ms_screen = wxSYS_SCREEN_PDA;

        if (x < 200)
            ms_screen = wxSYS_SCREEN_TINY;

        // This is probably a bug, but VNC seems to report 0
        if (x < 10)
            ms_screen = wxSYS_SCREEN_DESKTOP;
    }

    return ms_screen;
}

void wxSystemSettings::SetScreenType( wxSystemScreenType screen )
{
    ms_screen = screen;
}

// ----------------------------------------------------------------------------
// Trivial wxSystemAppearance implementation
// ----------------------------------------------------------------------------

#if !defined(__WXOSX__)

wxString wxSystemAppearance::GetName() const
{
    return wxString();
}

#endif // !__WXOSX__

// These ports implement this function using platform-specific API.
#if !defined(__WXOSX__) && !defined(__WXMSW__)

bool wxSystemAppearance::IsDark() const
{
    return IsUsingDarkBackground();
}

#endif // !__WXOSX__ && !__WXMSW__

bool wxSystemAppearance::IsUsingDarkBackground() const
{
    const wxColour bg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    const wxColour fg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);

    // The threshold here is rather arbitrary, but it seems that using just
    // inequality would be wrong as it could result in false positivies.
    return fg.GetLuminance() - bg.GetLuminance() > 0.2;
}

wxSystemAppearance wxSystemSettingsNative::GetAppearance()
{
    return wxSystemAppearance();
}

/////////////////////////////////////////////////////////////////////////////
// Name:        common/settcmn.cpp
// Purpose:     common (to all ports) wxWindow functions
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "settings.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/utils.h"
    #include "wx/settings.h"
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
#ifndef __WXUNIVERSAL__
    {
        // As a start, all GUI are desktops.
        ms_screen = wxSYS_SCREEN_DESKTOP;
    }
#else
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
    }
#endif

    return ms_screen;
}
    
void wxSystemSettings::SetScreenType( wxSystemScreenType screen )
{
    ms_screen = screen;
}


///////////////////////////////////////////////////////////////////////////////
// Name:        univ/theme.cpp
// Purpose:     implementation of wxTheme
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "theme.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/theme.h"

// ============================================================================
// implementation
// ============================================================================

wxTheme::wxThemeInfo *wxTheme::ms_allThemes = (wxTheme::wxThemeInfo *)NULL;
wxTheme *wxTheme::ms_theme = (wxTheme *)NULL;

// ----------------------------------------------------------------------------
// "dynamic" theme creation
// ----------------------------------------------------------------------------

wxTheme::wxThemeInfo::wxThemeInfo(wxTheme::Constructor c,
                                  const wxChar *n,
                                  const wxChar *d)
       : name(n), desc(d), ctor(c)
{
    // insert us (in the head of) the linked list
    next = ms_allThemes;
    ms_allThemes = this;
}

/* static */ wxTheme *wxTheme::Create(const wxString& name)
{
    // find the theme in the list by name
    wxThemeInfo *info = ms_allThemes;
    while ( info )
    {
        if ( name == info->name )
        {
            return info->ctor();
        }

        info = info->next;
    }

    return (wxTheme *)NULL;
}

// ----------------------------------------------------------------------------
// the default theme (called by wxApp::OnInitGui)
// ----------------------------------------------------------------------------

/* static */ bool wxTheme::CreateDefault()
{
    wxCHECK_MSG( !ms_theme, TRUE, _T("we already have a theme") );

#if defined(__WXMSW__)
    ms_theme = Create(_T("win32"));
#endif

    // fallback to the first one in the list
    if ( !ms_theme && ms_allThemes )
    {
        ms_theme = ms_allThemes->ctor();
    }

    // abort if still nothing
    if ( !ms_theme )
    {
        wxLogError(_("Failed to initialize GUI: no built-in themes found."));

        return FALSE;
    }

    return TRUE;
}

/* static */ wxTheme *wxTheme::Set(wxTheme *theme)
{
    wxTheme *themeOld = ms_theme;
    ms_theme = theme;
    return themeOld;
}

// ----------------------------------------------------------------------------
// assorted trivial dtors
// ----------------------------------------------------------------------------

wxTheme::~wxTheme()
{
}


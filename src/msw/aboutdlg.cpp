///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/aboutdlg.cpp
// Purpose:     implementation of wxAboutBox() for wxMSW
// Author:      Vadim Zeitlin
// Created:     2006-10-07
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwindows.org>
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_ABOUTDLG

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/aboutdlg.h"
#include "wx/generic/aboutdlgg.h"

// ============================================================================
// implementation
// ============================================================================

// helper function: returns all array elements in a single comma-separated and
// newline-terminated string
static wxString AllAsString(const wxArrayString& a)
{
    wxString s;
    const size_t count = a.size();
    for ( size_t n = 0; n < count; n++ )
    {
        s << a[n] << (n == count - 1 ? _T("\n") : _T(", "));
    }

    return s;
}

// our public entry point
void wxAboutBox(const wxAboutDialogInfo& info)
{
    // we prefer to show a simple message box if we don't have any fields which
    // can't be shown in it because as much as there is a standard about box
    // under MSW at all, this is it
    if ( info.HasWebSite() || info.HasIcon() || info.HasLicence() )
    {
        // we need to use the full-blown generic version
        wxGenericAboutBox(info);
    }
    else // simple "native" version should do
    {
        // build the text to show in the box
        const wxString name = info.GetName();
        wxString msg;
        msg << name;
        if ( info.HasVersion() )
            msg << _(" Version ") << info.GetVersion();
        msg << _T('\n');

        if ( info.HasCopyright() )
            msg << info.GetCopyright() << _T('\n');

        msg << info.GetDescription() << _T('\n');

        if ( info.HasDevelopers() )
            msg << _("Developed by ") << AllAsString(info.GetDevelopers());

        if ( info.HasDocWriters() )
            msg << _("Documentation by ") << AllAsString(info.GetDocWriters());

        if ( info.HasArtists() )
            msg << _("Graphics art by ") << AllAsString(info.GetArtists());

        if ( info.HasTranslators() )
            msg << _("Translations by ") << AllAsString(info.GetTranslators());

        wxMessageBox(msg, _T("About ") + name);
    }
}

#endif // wxUSE_ABOUTDLG

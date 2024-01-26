///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/aboutdlg.cpp
// Purpose:     implementation of wxAboutBox() for wxMSW
// Author:      Vadim Zeitlin
// Created:     2006-10-07
// Copyright:   (c) 2006 Vadim Zeitlin <vadim@wxwidgets.org>
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


#if wxUSE_ABOUTDLG

#ifndef WX_PRECOMP
    #include "wx/msgdlg.h"
#endif //WX_PRECOMP

#include "wx/aboutdlg.h"
#include "wx/generic/aboutdlgg.h"

// ============================================================================
// implementation
// ============================================================================

// our public entry point
void wxAboutBox(const wxAboutDialogInfo& info, wxWindow* parent)
{
    // we prefer to show a simple message box if we don't have any fields which
    // can't be shown in it because as much as there is a standard about box
    // under MSW at all, this is it
    if ( info.IsSimple() )
    {
        // build the text to show in the box
        const wxString name = info.GetName();
        wxString msg;
        msg << name;
        if ( info.HasVersion() )
            msg << wxT(' ') << info.GetVersion();

        // Separate the title from the rest with an extra blank line.
        msg << wxT("\n\n");

        msg << info.GetDescriptionAndCredits();

        if ( info.HasCopyright() )
            msg << wxT('\n') << info.GetCopyrightToDisplay();

        wxMessageBox(msg, wxString::Format(_("About %s"), name), wxOK | wxCENTRE, parent);
    }
    else // simple "native" version is not enough
    {
        // we need to use the full-blown generic version
        wxGenericAboutBox(info, parent);
    }
}

#endif // wxUSE_ABOUTDLG

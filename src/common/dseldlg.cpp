///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dseldlg.cpp
// Purpose:     implementation of ::wxDirSelector()
// Author:      Paul Thiessen
// Modified by:
// Created:     20.02.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 wxWidgets team
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "dseldlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/dirdlg.h"

#if wxUSE_DIRDLG

// ============================================================================
// implementation
// ============================================================================

const wxChar *wxDirSelectorPromptStr = wxT("Select a directory");

wxString wxDirSelector(const wxString& message,
                       const wxString& defaultPath,
                       long style,
                       const wxPoint& pos,
                       wxWindow *parent)
{
    wxString path;

    wxDirDialog dirDialog(parent, message, defaultPath, style, pos);
    if ( dirDialog.ShowModal() == wxID_OK )
    {
        path = dirDialog.GetPath();
    }

    return path;
}

#endif // wxUSE_DIRDLG


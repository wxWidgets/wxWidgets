/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/filehistory.cpp
// Purpose:     GTK+ bits for wxFileHistory class
// Author:      Vaclav Slavik
// Created:     2010-05-06
// Copyright:   (c) 2010 Vaclav Slavik
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


#include "wx/filehistory.h"

#if wxUSE_FILE_HISTORY

#include "wx/filename.h"

#include "wx/gtk/private.h"
#include "wx/gtk/private/string.h"

// ============================================================================
// implementation
// ============================================================================

void wxFileHistory::AddFileToHistory(const wxString& file)
{
    wxFileHistoryBase::AddFileToHistory(file);

#ifdef __WXGTK210__
    const wxString fullPath = wxFileName(file).GetFullPath();
    if ( wx_is_at_least_gtk2(10) )
    {
        wxGtkString uri(g_filename_to_uri(wxGTK_CONV_FN(fullPath), NULL, NULL));

        if ( uri )
            gtk_recent_manager_add_item(gtk_recent_manager_get_default(), uri);
    }
#endif
}

#endif // wxUSE_FILE_HISTORY

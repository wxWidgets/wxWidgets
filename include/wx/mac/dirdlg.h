/////////////////////////////////////////////////////////////////////////////
// Name:        dirdlg.h
// Purpose:     wxDirDialog class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIRDLG_H_
#define _WX_DIRDLG_H_

#ifdef __GNUG__
#pragma interface "dirdlg.h"
#endif

#include "wx/dialog.h"

WXDLLEXPORT_DATA(extern const char*) wxFileSelectorPromptStr;

class WXDLLEXPORT wxDirDialog: public wxDialog
{
DECLARE_DYNAMIC_CLASS(wxDirDialog)
public:
    wxDirDialog(wxWindow *parent, const wxString& message = wxFileSelectorPromptStr,
        const wxString& defaultPath = "",
        long style = 0, const wxPoint& pos = wxDefaultPosition);

    inline void SetMessage(const wxString& message) { m_message = message; }
    inline void SetPath(const wxString& path) { m_path = path; }
    inline void SetStyle(long style) { m_dialogStyle = style; }

    inline wxString GetMessage() const { return m_message; }
    inline wxString GetPath() const { return m_path; }
    inline long GetStyle() const { return m_dialogStyle; }

    int ShowModal();

protected:
    wxString    m_message;
    long        m_dialogStyle;
    wxWindow *  m_parent;
    wxString    m_path;
};

#endif
    // _WX_DIRDLG_H_

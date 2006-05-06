/////////////////////////////////////////////////////////////////////////////
// Name:        dirdlg.h
// Purpose:     wxDirDialog class
// Author:      David Webster
// Modified by:
// Created:     10/14/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIRDLG_H_
#define _WX_DIRDLG_H_

#include "wx/dialog.h"

WXDLLEXPORT_DATA(extern const wxChar) wxFileSelectorPromptStr[];

class WXDLLEXPORT wxDirDialog: public wxDirDialogBase
{
    DECLARE_DYNAMIC_CLASS(wxDirDialog)
public:
    wxDirDialog(wxWindow *parent, const wxString& message = wxFileSelectorPromptStr,
        const wxString& defaultPath = "",
        long style = 0, const wxPoint& pos = wxDefaultPosition);

    int ShowModal();

protected:
    wxWindow *  m_parent;
};

#endif
    // _WX_DIRDLG_H_

/////////////////////////////////////////////////////////////////////////////
// Name:        wx/motif/msgdlg.h
// Purpose:     wxMessageDialog class. Use generic version if no
//              platform-specific implementation.
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSGBOXDLG_H_
#define _WX_MSGBOXDLG_H_

#include "wx/defs.h"
#include "wx/dialog.h"

// ----------------------------------------------------------------------------
// Message box dialog
// ----------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern const wxChar) wxMessageBoxCaptionStr[];

class WXDLLEXPORT wxMessageDialog: public wxDialog, public wxMessageDialogBase
{
    DECLARE_DYNAMIC_CLASS(wxMessageDialog)

public:
    wxMessageDialog(wxWindow *parent,
        const wxString& message,
        const wxString& caption = wxMessageBoxCaptionStr,
        long style = wxOK | wxCENTRE,
        const wxPoint& pos = wxDefaultPosition);

    int ShowModal();

    // implementation only from now on
    // called by the Motif callback
    void SetResult(long result) { m_result = result; }

protected:
    wxString    m_caption;
    wxString    m_message;
    wxWindow *  m_parent;
    long        m_result;
};

#endif
// _WX_MSGBOXDLG_H_

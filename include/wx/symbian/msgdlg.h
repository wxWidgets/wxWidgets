/////////////////////////////////////////////////////////////////////////////
// Name:        wx/symbianmsgdlg.h
// Purpose:     wxMessageDialog class
// Author:      Jordan Langholz
// Modified by:
// Created:     04/24/07
// RCS-ID:      $Id$
// Copyright:   (c) Jordan Langholz
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSGBOXDLG_H_
#define _WX_MSGBOXDLG_H_

#include "wx/defs.h"
#include "wx/dialog.h"

/*
 * Message box dialog
 */
WXDLLEXPORT_DATA(extern const wxChar*) wxMessageBoxCaptionStr;

class WXDLLEXPORT wxMessageDialog: public wxDialog, public wxMessageDialogBase
{
DECLARE_DYNAMIC_CLASS(wxMessageDialog)
protected:
    wxString    m_caption;
    wxString    m_message;
    wxWindow *  m_parent;
public:
    wxMessageDialog(wxWindow *parent, const wxString& message, const wxString& caption = wxMessageBoxCaptionStr,
        long style = wxOK|wxCENTRE, const wxPoint& pos = wxDefaultPosition);

    int ShowModal(void);

    DECLARE_NO_COPY_CLASS(wxMessageDialog)
};


#endif
    // _WX_MSGBOXDLG_H_

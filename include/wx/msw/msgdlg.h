/////////////////////////////////////////////////////////////////////////////
// Name:        msgdlg.h
// Purpose:     wxMessageDialog class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __MSGBOXDLGH__
#define __MSGBOXDLGH__

#ifdef __GNUG__
#pragma interface "msgdlg.h"
#endif

#include "wx/setup.h"
#include "wx/dialog.h"

/*
 * Message box dialog
 */

WXDLLEXPORT_DATA(extern const char*) wxMessageBoxCaptionStr;

class WXDLLEXPORT wxMessageDialog: public wxDialog
{
DECLARE_DYNAMIC_CLASS(wxMessageDialog)
protected:
    wxString    m_caption;
    wxString    m_message;
    long        m_dialogStyle;
    wxWindow *  m_parent;
public:
    wxMessageDialog(wxWindow *parent, const wxString& message, const wxString& caption = wxMessageBoxCaptionStr,
        long style = wxOK|wxCENTRE, const wxPoint& pos = wxDefaultPosition);

    int ShowModal(void);
};


int WXDLLEXPORT wxMessageBox(const wxString& message, const wxString& caption = wxMessageBoxCaptionStr,
  long style = wxOK|wxCENTRE,
  wxWindow *parent = NULL, int x = -1, int y = -1);

#endif
    // __MSGBOXDLGH__

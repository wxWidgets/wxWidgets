/////////////////////////////////////////////////////////////////////////////
// Name:        msgdlg.h
// Purpose:     wxMessageDialog for GTK+2
// Author:      Vaclav Slavik
// Modified by:
// Created:     2003/02/28
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik, 2003
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __MSGDLG_H__
#define __MSGDLG_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "msgdlg.h"
#endif

#include "wx/setup.h"
#include "wx/dialog.h"

// type is an 'or' (|) of wxOK, wxCANCEL, wxYES_NO
// Returns wxYES/NO/OK/CANCEL

WXDLLEXPORT_DATA(extern const wxChar*) wxMessageBoxCaptionStr;

class WXDLLEXPORT wxMessageDialog: public wxDialog
{
public:
    wxMessageDialog(wxWindow *parent, const wxString& message,
                    const wxString& caption = wxMessageBoxCaptionStr,
                    long style = wxOK|wxCENTRE,
                    const wxPoint& pos = wxDefaultPosition);

    int ShowModal();

private:
    wxString m_caption;
    wxString m_message;
    long m_dialogStyle;
    wxWindow *m_parent;

    DECLARE_DYNAMIC_CLASS(wxMessageDialog)
};

#endif

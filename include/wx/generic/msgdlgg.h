/////////////////////////////////////////////////////////////////////////////
// Name:        msgdlgg.h
// Purpose:     Generic wxMessageDialog
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __MSGDLGH_G__
#define __MSGDLGH_G__

#ifdef __GNUG__
#pragma interface "msgdlgg.h"
#endif

#include "wx/setup.h"
#include "wx/dialog.h"

// type is an 'or' (|) of wxOK, wxCANCEL, wxYES_NO
// Returns wxYES/NO/OK/CANCEL

WXDLLEXPORT_DATA(extern const char*) wxMessageBoxCaptionStr;

class WXDLLEXPORT wxGenericMessageDialog: public wxDialog
{
DECLARE_DYNAMIC_CLASS(wxGenericMessageDialog)
protected:
    long m_dialogStyle;
public:
    wxGenericMessageDialog(wxWindow *parent, const wxString& message, const wxString& caption = wxMessageBoxCaptionStr,
        long style = wxOK|wxCENTRE, const wxPoint& pos = wxDefaultPosition);

    void OnYes(wxCommandEvent& event);
    void OnNo(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};

#ifndef __WINDOWS__
#define wxMessageDialog wxGenericMessageDialog

int wxMessageBox(const wxString& message, const wxString& caption = wxMessageBoxCaptionStr, 
  const long style = wxOK|wxCENTRE, wxWindow *parent = NULL, const int x = -1, const int y = -1);

#endif

#endif
	// __MSGDLGH_G__

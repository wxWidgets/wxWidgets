/////////////////////////////////////////////////////////////////////////////
// Name:        textdlgg.h
// Purpose:     wxStatusBar class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __TEXTDLGH_G__
#define __TEXTDLGH_G__

#ifdef __GNUG__
#pragma interface "textdlgg.h"
#endif

#include "wx/setup.h"
#include "wx/dialog.h"

// Handy dialog functions (will be converted into classes at some point)
WXDLLEXPORT_DATA(extern const char*) wxGetTextFromUserPromptStr;
WXDLLEXPORT_DATA(extern const char*) wxEmptyString;

#define wxID_TEXT 3000

class WXDLLEXPORT wxTextEntryDialog: public wxDialog
{
DECLARE_DYNAMIC_CLASS(wxTextEntryDialog)
protected:
    long m_dialogStyle;
    wxString m_value;
public:
    wxTextEntryDialog(wxWindow *parent, const wxString& message, const wxString& caption = wxGetTextFromUserPromptStr,
        const wxString& value = wxEmptyString, long style = wxOK|wxCANCEL|wxCENTRE, const wxPoint& pos = wxDefaultPosition);

    inline void SetValue(const wxString& val) { m_value = val; }
    inline wxString GetValue(void) const { return m_value; }

    void OnOK(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};

wxString WXDLLEXPORT wxGetTextFromUser(const wxString& message, const wxString& caption = wxGetTextFromUserPromptStr,
                        const wxString& default_value = wxEmptyString, wxWindow *parent = NULL,
                        int x = -1, int y = -1, bool centre = TRUE);

#endif
    // __TEXTDLGH_G__

/////////////////////////////////////////////////////////////////////////////
// Name:        textdlgg.h
// Purpose:     wxStatusBar class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __TEXTDLGH_G__
#define __TEXTDLGH_G__

#ifdef __GNUG__
    #pragma interface "textdlgg.h"
#endif

#include "wx/defs.h"

#include "wx/dialog.h"

class WXDLLEXPORT wxTextCtrl;

WXDLLEXPORT_DATA(extern const wxChar*) wxGetTextFromUserPromptStr;
WXDLLEXPORT_DATA(extern const wxChar*) wxEmptyString;

#define wxTextEntryDialogStyle (wxOK | wxCANCEL | wxCENTRE)

// ----------------------------------------------------------------------------
// wxTextEntryDialog: a dialog with text control, [ok] and [cancel] buttons
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTextEntryDialog : public wxDialog
{
public:
    wxTextEntryDialog(wxWindow *parent,
                      const wxString& message,
                      const wxString& caption = wxGetTextFromUserPromptStr,
                      const wxString& value = wxEmptyString,
                      long style = wxTextEntryDialogStyle,
                      const wxPoint& pos = wxDefaultPosition);

    void SetValue(const wxString& val) { m_value = val; }
    wxString GetValue() const { return m_value; }

    // implementation only
    void OnOK(wxCommandEvent& event);

protected:
    wxTextCtrl *m_textctrl;
    wxString    m_value;
    long        m_dialogStyle;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxTextEntryDialog)
};

// ----------------------------------------------------------------------------
// function to get a string from user
// ----------------------------------------------------------------------------

wxString WXDLLEXPORT
wxGetTextFromUser(const wxString& message,
                  const wxString& caption = wxGetTextFromUserPromptStr,
                  const wxString& default_value = wxEmptyString,
                  wxWindow *parent = (wxWindow *) NULL,
                  int x = -1,
                  int y = -1,
                  bool centre = TRUE);

wxString WXDLLEXPORT
wxGetPasswordFromUser(const wxString& message,
                      const wxString& caption = wxGetTextFromUserPromptStr,
                      const wxString& default_value = wxEmptyString,
                      wxWindow *parent = (wxWindow *) NULL);

#endif
    // __TEXTDLGH_G__

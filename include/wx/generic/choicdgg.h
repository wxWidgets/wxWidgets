/////////////////////////////////////////////////////////////////////////////
// Name:        choicdgg.h
// Purpose:     Generic choice dialogs
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __CHOICEDLGH_G__
#define __CHOICEDLGH_G__

#ifdef __GNUG__
    #pragma interface "choicdgg.h"
#endif

#include "wx/setup.h"
#include "wx/dialog.h"
#include "wx/listbox.h"

#define wxCHOICE_HEIGHT 150
#define wxCHOICE_WIDTH 200

#define wxCHOICEDLG_STYLE (wxDEFAULT_DIALOG_STYLE|wxOK | wxCANCEL | wxCENTRE)

class WXDLLEXPORT wxSingleChoiceDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS(wxSingleChoiceDialog)

public:
    wxSingleChoiceDialog(wxWindow *parent,
                         const wxString& message,
                         const wxString& caption,
                         int n,
                         const wxString *choices,
                         char **clientData = (char **)NULL,
                         long style = wxCHOICEDLG_STYLE,
                         const wxPoint& pos = wxDefaultPosition);

    wxSingleChoiceDialog(wxWindow *parent,
                         const wxString& message,
                         const wxString& caption,
                         const wxStringList& choices,
                         char **clientData = (char **)NULL,
                         long style = wxCHOICEDLG_STYLE,
                         const wxPoint& pos = wxDefaultPosition);

    bool Create(wxWindow *parent,
                const wxString& message,
                const wxString& caption,
                int n,
                const wxString *choices,
                char **clientData = (char **)NULL,
                long style = wxCHOICEDLG_STYLE,
                const wxPoint& pos = wxDefaultPosition);

    bool Create(wxWindow *parent,
                const wxString& message,
                const wxString& caption,
                const wxStringList& choices,
                char **clientData = (char **)NULL,
                long style = wxCHOICEDLG_STYLE,
                const wxPoint& pos = wxDefaultPosition);

    void SetSelection(int sel) ;
    int GetSelection() const { return m_selection; }
    wxString GetStringSelection() const { return m_stringSelection; }

    // get client data associated with selection
    void *GetClientData() const { return m_clientData; }

    // obsolete function (NB: no need to make it return wxChar, it's untyped)
    char *GetSelectionClientData() const { return (char *)m_clientData; }

    // implementation from now on
    void OnOK(wxCommandEvent& event);
    void OnListBoxDClick(wxCommandEvent& event);

protected:
    int         m_selection;
    int         m_dialogStyle;
    wxString    m_stringSelection;
    wxListBox  *m_listbox;

private:
    DECLARE_EVENT_TABLE()
};

WXDLLEXPORT wxString wxGetSingleChoice(const wxString& message, const wxString& caption,
                        int n, const wxString *choices, wxWindow *parent = (wxWindow *) NULL,
                        int x = -1, int y = -1, bool centre = TRUE,
                        int width = wxCHOICE_WIDTH, int height = wxCHOICE_HEIGHT);

WXDLLEXPORT wxString wxGetSingleChoice(const wxString& message, const wxString& caption,
                        int n, wxChar *choices[], wxWindow *parent = (wxWindow *) NULL,
                        int x = -1, int y = -1, bool centre = TRUE,
                        int width = wxCHOICE_WIDTH, int height = wxCHOICE_HEIGHT);

// Same as above but gets position in list of strings, instead of string,
// or -1 if no selection
WXDLLEXPORT int wxGetSingleChoiceIndex(const wxString& message, const wxString& caption,
                           int n, const wxString *choices, wxWindow *parent = (wxWindow *) NULL,
                           int x = -1, int y = -1, bool centre = TRUE,
                           int width = wxCHOICE_WIDTH, int height = wxCHOICE_HEIGHT);

WXDLLEXPORT int wxGetSingleChoiceIndex(const wxString& message, const wxString& caption,
                           int n, wxChar *choices[], wxWindow *parent = (wxWindow *) NULL,
                           int x = -1, int y = -1, bool centre = TRUE,
                           int width = wxCHOICE_WIDTH, int height = wxCHOICE_HEIGHT);

// Return client data instead
// FIXME: this is horrible, using "char *" instead of "void *" belongs to the 70s!
WXDLLEXPORT wxChar* wxGetSingleChoiceData(const wxString& message, const wxString& caption,
                            int n, const wxString *choices, char **client_data,
                            wxWindow *parent = (wxWindow *) NULL, int x = -1, int y = -1,
                            bool centre = TRUE,
                            int width = wxCHOICE_WIDTH, int height = wxCHOICE_HEIGHT);

WXDLLEXPORT wxChar* wxGetSingleChoiceData(const wxString& message, const wxString& caption,
                            int n, wxChar *choices[], char **client_data,
                            wxWindow *parent = (wxWindow *) NULL, int x = -1, int y = -1,
                            bool centre = TRUE,
                            int width = wxCHOICE_WIDTH, int height = wxCHOICE_HEIGHT);

/*
WXDLLEXPORT int wxGetMultipleChoice(const wxString& message, const wxString& caption,
			  int n, const wxString *choices,
			  int nsel, int * selection,
			  wxWindow *parent = NULL, int x = -1 , int y = -1, bool centre = TRUE,
			  int width = wxCHOICE_WIDTH, int height = wxCHOICE_HEIGHT);
*/

#endif

/////////////////////////////////////////////////////////////////////////////
// Name:        choicdgg.h
// Purpose:     Generic choice dialogs
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __CHOICEDLGH_G__
#define __CHOICEDLGH_G__

#ifdef __GNUG__
#pragma interface "choicdgg.h"
#endif

#include "wx/setup.h"
#include "wx/dialog.h"

#define wxCHOICE_HEIGHT 150
#define wxCHOICE_WIDTH 200

#define wxID_LISTBOX 3000

class WXDLLEXPORT wxSingleChoiceDialog: public wxDialog
{
DECLARE_DYNAMIC_CLASS(wxSingleChoiceDialog)
public:
    wxSingleChoiceDialog(wxWindow *parent, const wxString& message, const wxString& caption,
        const int n, const wxString *choices, char **clientData = NULL, long style = wxOK|wxCANCEL|wxCENTRE, const wxPoint& pos = wxDefaultPosition);

    wxSingleChoiceDialog(wxWindow *parent, const wxString& message, const wxString& caption,
        const wxStringList& choices, char **clientData = NULL, long style = wxOK|wxCANCEL|wxCENTRE, const wxPoint& pos = wxDefaultPosition);

    bool Create(wxWindow *parent, const wxString& message, const wxString& caption,
        const int n, const wxString *choices, char **clientData = NULL, long style = wxOK|wxCANCEL|wxCENTRE, const wxPoint& pos = wxDefaultPosition);
    bool Create(wxWindow *parent, const wxString& message, const wxString& caption,
        const wxStringList& choices, char **clientData = NULL, long style = wxOK|wxCANCEL|wxCENTRE, const wxPoint& pos = wxDefaultPosition);

    void SetSelection(int sel) ;
    inline int GetSelection(void) const { return m_selection; }
    inline wxString GetStringSelection(void) const { return m_stringSelection; }
    inline char *GetSelectionClientData(void) const { return m_clientData; }

    void OnOK(wxCommandEvent& event);

DECLARE_EVENT_TABLE()

protected:
    long        m_dialogStyle;
    int         m_selection;
    wxString    m_stringSelection;
    char*       m_clientData;
};

wxString WXDLLEXPORT wxGetSingleChoice(const wxString& message, const wxString& caption,
                        const int n, const wxString *choices, wxWindow *parent = NULL,
                        const int x = -1, const int y = -1, const bool centre = TRUE,
                        const int width = wxCHOICE_WIDTH, const int height = wxCHOICE_HEIGHT);

wxString WXDLLEXPORT wxGetSingleChoice(const wxString& message, const wxString& caption,
                        const int n, char *choices[], wxWindow *parent = NULL,
                        const int x = -1, const int y = -1, const bool centre = TRUE,
                        const int width = wxCHOICE_WIDTH, const int height = wxCHOICE_HEIGHT);

// Same as above but gets position in list of strings, instead of string,
// or -1 if no selection
int WXDLLEXPORT wxGetSingleChoiceIndex(const wxString& message, const wxString& caption,
                           const int n, const wxString *choices, wxWindow *parent = NULL,
                           const int x = -1, const int y = -1, const bool centre = TRUE,
                           const int width = wxCHOICE_WIDTH, const int height = wxCHOICE_HEIGHT);

int WXDLLEXPORT wxGetSingleChoiceIndex(const wxString& message, const wxString& caption,
                           const int n, char *choices[], wxWindow *parent = NULL,
                           const int x = -1, const int y = -1, const bool centre = TRUE,
                           const int width = wxCHOICE_WIDTH, const int height = wxCHOICE_HEIGHT);

// Return client data instead
char* WXDLLEXPORT wxGetSingleChoiceData(const wxString& message, const wxString& caption,
                            const int n, const wxString *choices, char **client_data,
                            wxWindow *parent = NULL, const int x = -1, const int y = -1,
                            const bool centre = TRUE,
                            const int width = wxCHOICE_WIDTH, const int height = wxCHOICE_HEIGHT);

char* WXDLLEXPORT wxGetSingleChoiceData(const wxString& message, const wxString& caption,
                            const int n, char *choices[], char **client_data,
                            wxWindow *parent = NULL, const int x = -1, const int y = -1,
                            const bool centre = TRUE,
                            const int width = wxCHOICE_WIDTH, const int height = wxCHOICE_HEIGHT);
                           
/*
int WXDLLEXPORT wxGetMultipleChoice(const wxString& message, const wxString& caption,
			  const int n, const wxString *choices,
			  const int nsel, int * selection,
			  wxWindow *parent = NULL, const int x = -1 , const int y = -1, const bool centre = TRUE,
			  const int width = wxCHOICE_WIDTH, const int height = wxCHOICE_HEIGHT);
*/

#endif

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
        int n, const wxString *choices, wxChar **clientData = (wxChar **) NULL, long style = wxOK|wxCANCEL|wxCENTRE, const wxPoint& pos = wxDefaultPosition);

    wxSingleChoiceDialog(wxWindow *parent, const wxString& message, const wxString& caption,
        const wxStringList& choices, wxChar **clientData = (wxChar **) NULL, long style = wxOK|wxCANCEL|wxCENTRE, const wxPoint& pos = wxDefaultPosition);

    bool Create(wxWindow *parent, const wxString& message, const wxString& caption,
        int n, const wxString *choices, wxChar **clientData = (wxChar **) NULL, long style = wxOK|wxCANCEL|wxCENTRE, const wxPoint& pos = wxDefaultPosition);
    bool Create(wxWindow *parent, const wxString& message, const wxString& caption,
        const wxStringList& choices, wxChar **clientData = (wxChar **) NULL, long style = wxOK|wxCANCEL|wxCENTRE, const wxPoint& pos = wxDefaultPosition);

    void SetSelection(int sel) ;
    inline int GetSelection(void) const { return m_selection; }
    inline wxString GetStringSelection(void) const { return m_stringSelection; }
    inline wxChar *GetSelectionClientData(void) const { return m_clientData; }

    void OnOK(wxCommandEvent& event);
    void OnListBoxDClick(wxCommandEvent& event);

DECLARE_EVENT_TABLE()

protected:
    long        m_dialogStyle;
    int         m_selection;
    wxString    m_stringSelection;
    wxChar*     m_clientData;
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
WXDLLEXPORT wxChar* wxGetSingleChoiceData(const wxString& message, const wxString& caption,
                            int n, const wxString *choices, wxChar **client_data,
                            wxWindow *parent = (wxWindow *) NULL, int x = -1, int y = -1,
                            bool centre = TRUE,
                            int width = wxCHOICE_WIDTH, int height = wxCHOICE_HEIGHT);

WXDLLEXPORT wxChar* wxGetSingleChoiceData(const wxString& message, const wxString& caption,
                            int n, wxChar *choices[], wxChar **client_data,
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

/////////////////////////////////////////////////////////////////////////////
// Name:        choicdgg.cpp
// Purpose:     Choice dialogs
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "choicdgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <stdio.h>
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/button.h"
    #include "wx/listbox.h"
    #include "wx/stattext.h"
    #include "wx/intl.h"
    #include "wx/sizer.h"
#endif

#if wxUSE_STATLINE
    #include "wx/statline.h"
#endif

#include "wx/generic/choicdgg.h"

#define wxID_LISTBOX 3000

wxString wxGetSingleChoice( const wxString& message, const wxString& caption, int n,
                            const wxString *choices, wxWindow *parent,
                            int WXUNUSED(x), int WXUNUSED(y), bool WXUNUSED(centre),
                            int WXUNUSED(width), int WXUNUSED(height) )
{
    wxSingleChoiceDialog dialog(parent, message, caption, n, choices);
    if ( dialog.ShowModal() == wxID_OK )
        return dialog.GetStringSelection();
    else
        return wxT("");
}

// Overloaded for backward compatibility
wxString wxGetSingleChoice( const wxString& message, const wxString& caption, int n,
                            char *choices[], wxWindow *parent,
                            int x, int y, bool centre,
                            int width, int height )
{
    wxString *strings = new wxString[n];
    int i;
    for ( i = 0; i < n; i++)
    {
        strings[i] = choices[i];
    }
    wxString ans(wxGetSingleChoice(message, caption, n, (const wxString *)strings, parent,
        x, y, centre, width, height));
    delete[] strings;
    return ans;
}

int wxGetSingleChoiceIndex( const wxString& message, const wxString& caption, int n,
                            const wxString *choices, wxWindow *parent,
                int WXUNUSED(x), int WXUNUSED(y), bool WXUNUSED(centre),
                int WXUNUSED(width), int WXUNUSED(height) )
{
    wxSingleChoiceDialog dialog(parent, message, caption, n, choices);
    if ( dialog.ShowModal() == wxID_OK )
        return dialog.GetSelection();
    else
        return -1;
}

// Overloaded for backward compatibility
int wxGetSingleChoiceIndex( const wxString& message, const wxString& caption, int n,
                            wxChar *choices[], wxWindow *parent,
                int x, int y, bool centre,
                int width, int height )
{
    wxString *strings = new wxString[n];
    for ( int i = 0; i < n; i++)
        strings[i] = choices[i];
    int ans = wxGetSingleChoiceIndex(message, caption, n, (const wxString *)strings, parent,
        x, y, centre, width, height);
    delete[] strings;
    return ans;
}

void *wxGetSingleChoiceData( const wxString& message, const wxString& caption, int n,
                   const wxString *choices, void **client_data, wxWindow *parent,
                   int WXUNUSED(x), int WXUNUSED(y), bool WXUNUSED(centre),
                   int WXUNUSED(width), int WXUNUSED(height) )
{
    wxSingleChoiceDialog dialog(parent, message, caption, n, choices, (char **)client_data);
    if ( dialog.ShowModal() == wxID_OK )
        return dialog.GetSelectionClientData();
    else
        return NULL;
}

// Overloaded for backward compatibility
void *wxGetSingleChoiceData( const wxString& message, const wxString& caption, int n,
                   wxChar *choices[], void **client_data, wxWindow *parent,
                   int x, int y, bool centre,
                   int width, int height )
{
    wxString *strings = new wxString[n];
    int i;
    for ( i = 0; i < n; i++)
    {
        strings[i] = choices[i];
    }
    void *data = wxGetSingleChoiceData(message, caption, n, (const wxString *)strings, client_data, parent,
        x, y, centre, width, height);
    delete[] strings;
    return data;
}


/* Multiple choice dialog contributed by Robert Cowell
 *

The new data passed are in the "int nsel" and "int * selection"

The idea is to make a multiple selection from list of strings.
The returned value is the total number selected. initialily there
are nsel selected, with indices stored in
selection[0],...,selection[nsel-1] which appear highlighted to
begin with. On exit with value i
selection[0..i-1] contains the indices of the selected items.
(Some prior selectecions might be deselected.)
Thus selection must be as big as choices, in case all items are
selected.

*/
/*
int wxGetMultipleChoice(const wxString& message, const wxString& caption,
              int n, const wxString *choices,
              int nsel, int * selection,
              wxWindow *parent , int x , int y, bool centre,
              int width, int height)
{
    return -1;
}
*/

// wxSingleChoiceDialog

#if !USE_SHARED_LIBRARY
BEGIN_EVENT_TABLE(wxSingleChoiceDialog, wxDialog)
    EVT_BUTTON(wxID_OK, wxSingleChoiceDialog::OnOK)
    EVT_LISTBOX_DCLICK(wxID_LISTBOX, wxSingleChoiceDialog::OnListBoxDClick)
END_EVENT_TABLE()

IMPLEMENT_CLASS(wxSingleChoiceDialog, wxDialog)
#endif

#if defined(__WXMSW__) || defined(__WXMAC__)
#define wxCHOICEDLG_DIALOG_STYLE (wxDEFAULT_DIALOG_STYLE | \
                                  wxDIALOG_MODAL |         \
                                  wxTAB_TRAVERSAL)
#else
#define wxCHOICEDLG_DIALOG_STYLE (wxDEFAULT_DIALOG_STYLE | \
                                  wxDIALOG_MODAL |         \
                                  wxRESIZE_BORDER |        \
                                  wxTAB_TRAVERSAL)
#endif


wxSingleChoiceDialog::wxSingleChoiceDialog(wxWindow *parent,
                                           const wxString& message,
                                           const wxString& caption,
                                           int n,
                                           const wxString *choices,
                                           char **clientData,
                                           long style,
                                           const wxPoint& pos)
                    : wxDialog(parent, -1, caption, pos, wxDefaultSize,
                               wxCHOICEDLG_DIALOG_STYLE)
{
    Create(parent, message, caption, n, choices, clientData, style);
}

wxSingleChoiceDialog::wxSingleChoiceDialog(wxWindow *parent,
                                           const wxString& message,
                                           const wxString& caption,
                                           const wxStringList& choices,
                                           char **clientData,
                                           long style,
                                           const wxPoint& pos)
                    : wxDialog(parent, -1, caption, pos, wxDefaultSize,
                               wxCHOICEDLG_DIALOG_STYLE)
{
    Create(parent, message, caption, choices, clientData, style);
}

bool wxSingleChoiceDialog::Create(wxWindow *parent,
                                  const wxString& message,
                                  const wxString& caption,
                                  const wxStringList& choices,
                                  char **clientData,
                                  long style,
                                  const wxPoint& pos)
{
    wxString *strings = new wxString[choices.Number()];
    int i;
    for ( i = 0; i < choices.Number(); i++)
    {
        strings[i] = (char *)choices.Nth(i)->Data();
    }
    bool ans = Create(parent, message, caption, choices.Number(), strings, clientData, style, pos);
    delete[] strings;
    return ans;
}

bool wxSingleChoiceDialog::Create( wxWindow *WXUNUSED(parent),
                                   const wxString& message,
                                   const wxString& WXUNUSED(caption),
                                   int n,
                                   const wxString *choices,
                                   char **clientData,
                                   long style,
                                   const wxPoint& WXUNUSED(pos) )
{
    m_selection = 0;

    m_dialogStyle = style;

    wxBeginBusyCursor();
    
    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    // 1) text message
    topsizer->Add( CreateTextSizer( message ), 0, wxALL, 10 );
    
    // 2) list box
    m_listbox = new wxListBox( this, wxID_LISTBOX, wxDefaultPosition, wxSize(160,100) , 
                                        n, choices, wxLB_ALWAYS_SB );
    m_listbox->SetSelection( m_selection );
    if (clientData)
    {
        for (int i = 0; i < n; i++)
            m_listbox->SetClientData(i, clientData[i]);
    }
    topsizer->Add( m_listbox, 1, wxEXPAND | wxLEFT|wxRIGHT, 15 );

#if wxUSE_STATLINE
    // 3) static line
    topsizer->Add( new wxStaticLine( this, -1 ), 0, wxEXPAND | wxLEFT|wxRIGHT|wxTOP, 10 );
#endif

    // 4) buttons
    topsizer->Add( CreateButtonSizer( wxOK|wxCANCEL ), 0, wxCENTRE | wxALL, 10 );

    SetAutoLayout( TRUE );
    SetSizer( topsizer );
    
    topsizer->SetSizeHints( this );
    topsizer->Fit( this );

    Centre( wxBOTH );

    m_listbox->SetFocus();

    wxEndBusyCursor();

    return TRUE;
}

// Set the selection
void wxSingleChoiceDialog::SetSelection(int sel)
{
    m_listbox->SetSelection(sel);
    m_selection = sel;
}

void wxSingleChoiceDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
    m_selection = m_listbox->GetSelection();
    m_stringSelection = m_listbox->GetStringSelection();
    // TODO!
#ifndef __WXMOTIF__
    if ( m_listbox->HasClientUntypedData() )
        SetClientData(m_listbox->GetClientData(m_selection));
#endif
    EndModal(wxID_OK);
}

void wxSingleChoiceDialog::OnListBoxDClick(wxCommandEvent& WXUNUSED(event))
{
    m_selection = m_listbox->GetSelection();
    m_stringSelection = m_listbox->GetStringSelection();

    // TODO!
#ifndef __WXMOTIF__
    if ( m_listbox->HasClientUntypedData() )
        SetClientData(m_listbox->GetClientData(m_selection));
#endif

    EndModal(wxID_OK);
}


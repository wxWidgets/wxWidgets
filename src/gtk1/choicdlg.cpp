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

#include "wx/gtk/choicdlg.h"

static void wxSplitMessage2( const wxString &message, wxWindow *parent, wxSizer* sizer )
{
    wxString line;
    for (size_t pos = 0; pos < message.Len(); pos++)
    {
        if (message[pos] == _T('\n'))
        {
            if (!line.IsEmpty())
            {
                wxStaticText *s1 = new wxStaticText( parent, -1, line );
		sizer->Add( s1 );
                line = _T("");
            }
        }
        else
        {
            line += message[pos];
        }
    }
    
    // remaining text behind last '\n'
    if (!line.IsEmpty())
    {
        wxStaticText *s2 = new wxStaticText( parent, -1, line );
	sizer->Add( s2 );
    }
}


wxString wxGetSingleChoice( const wxString& message, const wxString& caption, int n, 
                            const wxString *choices, wxWindow *parent,
                int WXUNUSED(x), int WXUNUSED(y), bool WXUNUSED(centre), 
                int WXUNUSED(width), int WXUNUSED(height) )
{
    wxSingleChoiceDialog dialog(parent, message, caption, n, choices);
    if ( dialog.ShowModal() == wxID_OK )
        return dialog.GetStringSelection();
    else
        return _T("");
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

wxChar *wxGetSingleChoiceData( const wxString& message, const wxString& caption, int n,
                   const wxString *choices, char **client_data, wxWindow *parent,
                   int WXUNUSED(x), int WXUNUSED(y), bool WXUNUSED(centre), 
                   int WXUNUSED(width), int WXUNUSED(height) )
{
    wxSingleChoiceDialog dialog(parent, message, caption, n, choices, client_data);
    if ( dialog.ShowModal() == wxID_OK )
        return (wxChar *)dialog.GetSelectionClientData();
    else
        return NULL;
}

// Overloaded for backward compatibility
wxChar *wxGetSingleChoiceData( const wxString& message, const wxString& caption, int n, 
                   wxChar *choices[], char **client_data, wxWindow *parent,
                   int x, int y, bool centre, 
                   int width, int height )
{
    wxString *strings = new wxString[n];
    int i;
    for ( i = 0; i < n; i++)
    {
        strings[i] = choices[i];
    }
    wxChar *data = wxGetSingleChoiceData(message, caption, n, (const wxString *)strings, client_data, parent,
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

wxSingleChoiceDialog::wxSingleChoiceDialog(wxWindow *parent, const wxString& message, const wxString& caption,
        int n, const wxString *choices, char **clientData, long style, const wxPoint& pos):
      wxDialog(parent, -1, caption, pos, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxDIALOG_MODAL|wxTAB_TRAVERSAL)
{
        Create(parent, message, caption, n, choices, clientData, style);
}

wxSingleChoiceDialog::wxSingleChoiceDialog(wxWindow *parent, const wxString& message, const wxString& caption,
        const wxStringList& choices, char **clientData, long style, const wxPoint& pos):
      wxDialog(parent, -1, caption, pos, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL)
{
        Create(parent, message, caption, choices, clientData, style);
}

bool wxSingleChoiceDialog::Create(wxWindow *parent, const wxString& message, const wxString& caption,
        const wxStringList& choices, char **clientData, long style, const wxPoint& pos)
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

bool wxSingleChoiceDialog::Create( wxWindow *WXUNUSED(parent), const wxString& message, 
                                   const wxString& WXUNUSED(caption), int n, 
                                   const wxString *choices, char **clientData, long style,
                                   const wxPoint& WXUNUSED(pos) )
{
    m_dialogStyle = style;
    m_selection = 0;
    m_stringSelection = _T("");
    m_clientData = NULL;

    wxBeginBusyCursor();
    
    wxBox *topsizer = new wxBox( wxVERTICAL );

    // 1) text message
    wxBox *textsizer = new wxBox( wxVERTICAL );
    wxSplitMessage2( message, this, textsizer );
    topsizer->Add( textsizer, 0, wxALL, 10 );

    // 2) list box
    wxListBox *listBox = new wxListBox( this, wxID_LISTBOX, wxDefaultPosition, wxSize(160,100) , 
                                        n, choices, wxLB_ALWAYS_SB );
    listBox->SetSelection( m_selection );
    if (clientData)
    {
        for (int i = 0; i < n; i++)
            listBox->SetClientData(i, clientData[i]);
    }
    topsizer->Add( listBox, 1, wxEXPAND | wxLEFT|wxRIGHT, 15 );


#if wxUSE_STATLINE
    // 3) static line
    topsizer->Add( new wxStaticLine( this, -1 ), 0, wxEXPAND | wxLEFT|wxRIGHT|wxTOP, 3 );
#endif


    // 4) buttons
    wxBox *buttonsizer = new wxBox( wxHORIZONTAL );

    wxButton *ok = (wxButton *) NULL;
    if (style & wxOK)
    {
        ok = new wxButton( this, wxID_OK, _("OK") );
	buttonsizer->Add( ok, 0, wxLEFT|wxRIGHT, 10 );
    }

    wxButton *cancel = (wxButton *) NULL;
    if (style & wxCANCEL) 
    {
        cancel = new wxButton( this, wxID_CANCEL, _("Cancel") );
	buttonsizer->Add( cancel, 0, wxLEFT|wxRIGHT, 10 );
    }
    
    topsizer->Add( buttonsizer, 0, wxCENTRE | wxALL, 10 );

    topsizer->SetSizeHints( this );
    topsizer->Fit( this );
    SetSizer( topsizer );
    SetAutoLayout( TRUE );

    Centre( wxBOTH );

    if (ok)
        ok->SetDefault();

    listBox->SetFocus();

    wxEndBusyCursor();

    return TRUE;
}

// Set the selection
void wxSingleChoiceDialog::SetSelection(int sel)
{
    wxListBox *listBox = (wxListBox *)FindWindow(wxID_LISTBOX);
    if (listBox)
    {
        listBox->SetSelection(sel);
    }
    m_selection = sel;
}

void wxSingleChoiceDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
    wxListBox *listBox = (wxListBox *)FindWindow(wxID_LISTBOX);
    if ( listBox )
    {
        m_selection = listBox->GetSelection();
        m_stringSelection = listBox->GetStringSelection();
        m_clientData = listBox->GetClientData(m_selection);
    }

    EndModal(wxID_OK);
}

void wxSingleChoiceDialog::OnListBoxDClick(wxCommandEvent& WXUNUSED(event))
{
    wxListBox *listBox = (wxListBox *)FindWindow(wxID_LISTBOX);
    if ( listBox )
    {
        m_selection = listBox->GetSelection();
        m_stringSelection = listBox->GetStringSelection();
        m_clientData = listBox->GetClientData(m_selection);
    }

    EndModal(wxID_OK);
}


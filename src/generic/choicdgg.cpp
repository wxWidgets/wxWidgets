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
#endif

#if wxUSE_STATLINE
  #include "wx/statline.h" 
#endif

#include "wx/generic/choicdgg.h"

/* Split message, using constraints to position controls */
static wxSize wxSplitMessage2( const wxString &message, wxWindow *parent )
{
    int y = 10;
    int w = 50;
    wxString line( _T("") );
    for (size_t pos = 0; pos < message.Len(); pos++)
    {
        if (message[pos] == _T('\n'))
        {
            if (!line.IsEmpty())
            {
                wxStaticText *s1 = new wxStaticText( parent, -1, line, wxPoint(15,y) );
                wxSize size1( s1->GetSize() );
                if (size1.x > w) w = size1.x;
                line = _T("");
            }
            y += 18;
        }
        else
        {
            line += message[pos];
        }
    }
    
    if (!line.IsEmpty())
    {
        wxStaticText *s2 = new wxStaticText( parent, -1, line, wxPoint(15,y) );
        wxSize size2( s2->GetSize() );
        if (size2.x > w) w = size2.x;
    }
        
    y += 18;
    
    return wxSize(w+30,y);
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
                   const wxString *choices, wxChar **client_data, wxWindow *parent,
                   int WXUNUSED(x), int WXUNUSED(y), bool WXUNUSED(centre), 
                   int WXUNUSED(width), int WXUNUSED(height) )
{
    wxSingleChoiceDialog dialog(parent, message, caption, n, choices, client_data);
    if ( dialog.ShowModal() == wxID_OK )
        return dialog.GetSelectionClientData();
    else
        return NULL;
}

// Overloaded for backward compatibility
wxChar *wxGetSingleChoiceData( const wxString& message, const wxString& caption, int n, 
                   wxChar *choices[], wxChar **client_data, wxWindow *parent,
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
        int n, const wxString *choices, wxChar **clientData, long style, const wxPoint& pos):
      wxDialog(parent, -1, caption, pos, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL|wxTAB_TRAVERSAL)
{
        Create(parent, message, caption, n, choices, clientData, style);
}

wxSingleChoiceDialog::wxSingleChoiceDialog(wxWindow *parent, const wxString& message, const wxString& caption,
        const wxStringList& choices, wxChar **clientData, long style, const wxPoint& pos):
      wxDialog(parent, -1, caption, pos, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL)
{
        Create(parent, message, caption, choices, clientData, style);
}

bool wxSingleChoiceDialog::Create(wxWindow *parent, const wxString& message, const wxString& caption,
        const wxStringList& choices, wxChar **clientData, long style, const wxPoint& pos)
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
                                   const wxString *choices, wxChar **clientData, long style,
                                   const wxPoint& WXUNUSED(pos) )
{
    m_dialogStyle = style;
    m_selection = 0;
    m_stringSelection = _T("");
    m_clientData = NULL;

    wxBeginBusyCursor();
    
    wxSize message_size( wxSplitMessage2( message, this ) );

    wxButton *ok = (wxButton *) NULL;
    wxButton *cancel = (wxButton *) NULL;
    wxList m_buttons;
    
    int y = message_size.y + 15;
    
    int listbox_height = wxMin( 160, n*20 );
    
    wxListBox *listBox = new wxListBox( this, wxID_LISTBOX, wxPoint(10, y), wxSize(240, listbox_height), 
                                        n, choices, wxLB_ALWAYS_SB );
    listBox->SetSelection( m_selection );
    if (clientData)
    {
        for (int i = 0; i < n; i++)
            listBox->SetClientData(i, clientData[i]);
    }
    
    y += listbox_height + 35;

    if (style & wxOK) 
    {
        ok = new wxButton( this, wxID_OK, _("OK"), wxPoint(-1,y), wxSize(80,-1) );
	m_buttons.Append( ok );
    }

    if (style & wxCANCEL) 
    {
        cancel = new wxButton( this, wxID_CANCEL, _("Cancel"), wxPoint(-1,y), wxSize(80,-1) );
	m_buttons.Append( cancel );
    }

    if (ok)
    {
        ok->SetDefault();
        ok->SetFocus();
    }

    int w = m_buttons.GetCount() * 100;
    if (message_size.x > w) w = message_size.x;
    int space = w / (m_buttons.GetCount()*2);
    
    listBox->SetSize( 20, -1, w-10, listbox_height );
    
    int m = 0;
    wxNode *node = m_buttons.First();
    while (node)
    {
        wxWindow *win = (wxWindow*)node->Data();
        int x = (m*2+1)*space - 40 + 15;
        win->Move( x, -1 );
        node = node->Next();
        m++;
    }
    
#if wxUSE_STATLINE
    (void) new wxStaticLine( this, -1, wxPoint(0,y-20), wxSize(w+30, 5) );
#endif
    
    SetSize( w+30, y+40 );

    Centre( wxBOTH );

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
        m_clientData = (wxChar*)listBox->GetClientData(m_selection);
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
        m_clientData = (wxChar*)listBox->GetClientData(m_selection);
    }

    EndModal(wxID_OK);
}


/////////////////////////////////////////////////////////////////////////////
// Name:        choicdgg.cpp
// Purpose:     Choice dialogs
// Author:      Julian Smart
// Modified by: 03.11.00: VZ to add wxArrayString and multiple sel functions
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

#ifdef __GNUG__
    #pragma implementation "choicdgg.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#define wxID_LISTBOX 3000

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

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// convert wxArrayString into a wxString[] which must be delete[]d by caller
static int ConvertWXArrayToC(const wxArrayString& aChoices, wxString **choices);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

int ConvertWXArrayToC(const wxArrayString& aChoices, wxString **choices)
{
    int n = aChoices.GetCount();
    *choices = new wxString[n];

    for ( int i = 0; i < n; i++ )
    {
        (*choices)[i] = aChoices[i];   
    }

    return n;
}

// ----------------------------------------------------------------------------
// wrapper functions
// ----------------------------------------------------------------------------

wxString wxGetSingleChoice( const wxString& message,
                            const wxString& caption,
                            int n, const wxString *choices,
                            wxWindow *parent,
                            int WXUNUSED(x), int WXUNUSED(y),
                            bool WXUNUSED(centre),
                            int WXUNUSED(width), int WXUNUSED(height) )
{
    wxSingleChoiceDialog dialog(parent, message, caption, n, choices);
    wxString choice;
    if ( dialog.ShowModal() == wxID_OK )
        choice = dialog.GetStringSelection();

    return choice;
}

wxString wxGetSingleChoice( const wxString& message,
                            const wxString& caption,
                            const wxArrayString& aChoices,
                            wxWindow *parent,
                            int x, int y,
                            bool centre,
                            int width, int height)
{
    wxString *choices;
    int n = ConvertWXArrayToC(aChoices, &choices);
    wxString res = wxGetSingleChoice(message, caption, n, choices, parent,
                                     x, y, centre, width, height);
    delete [] choices;

    return res;
}

#ifdef WXWIN_COMPATIBILITY_2
// Overloaded for backward compatibility
wxString wxGetSingleChoice( const wxString& message,
                            const wxString& caption,
                            int n, char *choices[],
                            wxWindow *parent,
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
#endif // WXWIN_COMPATIBILITY_2

int wxGetSingleChoiceIndex( const wxString& message,
                            const wxString& caption,
                            int n, const wxString *choices,
                            wxWindow *parent,
                            int WXUNUSED(x), int WXUNUSED(y),
                            bool WXUNUSED(centre),
                            int WXUNUSED(width), int WXUNUSED(height) )
{
    wxSingleChoiceDialog dialog(parent, message, caption, n, choices);
    int choice;
    if ( dialog.ShowModal() == wxID_OK )
        choice = dialog.GetSelection();
    else
        choice = -1;

    return choice;
}

#ifdef WXWIN_COMPATIBILITY_2
// Overloaded for backward compatibility
int wxGetSingleChoiceIndex( const wxString& message,
                            const wxString& caption,
                            int n, wxChar *choices[],
                            wxWindow *parent,
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
#endif // WXWIN_COMPATIBILITY_2

void *wxGetSingleChoiceData( const wxString& message,
                             const wxString& caption,
                             int n, const wxString *choices,
                             void **client_data,
                             wxWindow *parent,
                             int WXUNUSED(x), int WXUNUSED(y),
                             bool WXUNUSED(centre),
                             int WXUNUSED(width), int WXUNUSED(height) )
{
    wxSingleChoiceDialog dialog(parent, message, caption, n, choices,
                                (char **)client_data);
    void *data;
    if ( dialog.ShowModal() == wxID_OK )
        data = dialog.GetSelectionClientData();
    else
        data = NULL;

    return data;
}

void *wxGetSingleChoiceData( const wxString& message,
                             const wxString& caption,
                             const wxArrayString& aChoices,
                             void **client_data,
                             wxWindow *parent,
                             int x, int y,
                             bool centre,
                             int width, int height)
{
    wxString *choices;
    int n = ConvertWXArrayToC(aChoices, &choices);
    void *res = wxGetSingleChoiceData(message, caption, n, choices,
                                      client_data, parent,
                                      x, y, centre, width, height);
    delete [] choices;

    return res;
}

#ifdef WXWIN_COMPATIBILITY_2
// Overloaded for backward compatibility
void *wxGetSingleChoiceData( const wxString& message,
                             const wxString& caption,
                             int n, wxChar *choices[],
                             void **client_data,
                             wxWindow *parent,
                             int x, int y, bool centre, int width, int height )
{
    wxString *strings = new wxString[n];
    int i;
    for ( i = 0; i < n; i++)
    {
        strings[i] = choices[i];
    }
    void *data = wxGetSingleChoiceData(message, caption,
                                       n, (const wxString *)strings,
                                       client_data, parent,
                                       x, y, centre, width, height);
    delete[] strings;
    return data;
}
#endif // WXWIN_COMPATIBILITY_2

size_t wxGetMultipleChoices(wxArrayInt& selections,
                            const wxString& message,
                            const wxString& caption,
                            int n, const wxString *choices,
                            wxWindow *parent,
                            int WXUNUSED(x), int WXUNUSED(y),
                            bool WXUNUSED(centre),
                            int WXUNUSED(width), int WXUNUSED(height))
{
    wxMultiChoiceDialog dialog(parent, message, caption, n, choices);
    if ( dialog.ShowModal() == wxID_OK )
        selections = dialog.GetSelections();
    else
        selections.Empty();

    return selections.GetCount();
}

size_t wxGetMultipleChoices(wxArrayInt& selections,
                            const wxString& message,
                            const wxString& caption,
                            const wxArrayString& aChoices,
                            wxWindow *parent,
                            int x, int y,
                            bool centre,
                            int width, int height)
{
    wxString *choices;
    int n = ConvertWXArrayToC(aChoices, &choices);
    size_t res = wxGetMultipleChoices(selections, message, caption,
                                      n, choices, parent,
                                      x, y, centre, width, height);
    delete [] choices;

    return res;
}

// ----------------------------------------------------------------------------
// wxAnyChoiceDialog
// ----------------------------------------------------------------------------

bool wxAnyChoiceDialog::Create(wxWindow *parent,
                               const wxString& message,
                               const wxString& caption,
                               int n, const wxString *choices,
                               long WXUNUSED(styleDlg), // FIXME: why unused?
                               const wxPoint& pos,
                               long styleLbox)
{
    if ( !wxDialog::Create(parent, -1, caption, pos, wxDefaultSize,
                           wxCHOICEDLG_DIALOG_STYLE) )
        return FALSE;

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    // 1) text message
    topsizer->Add( CreateTextSizer( message ), 0, wxALL, 10 );

    // 2) list box
    m_listbox = new wxListBox( this, wxID_LISTBOX,
                               wxDefaultPosition, wxDefaultSize,
                               n, choices,
                               styleLbox );
    if ( n > 0 )
        m_listbox->SetSelection(0);

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

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxSingleChoiceDialog
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxSingleChoiceDialog, wxDialog)
    EVT_BUTTON(wxID_OK, wxSingleChoiceDialog::OnOK)
    EVT_LISTBOX_DCLICK(wxID_LISTBOX, wxSingleChoiceDialog::OnListBoxDClick)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxSingleChoiceDialog, wxDialog)

wxSingleChoiceDialog::wxSingleChoiceDialog(wxWindow *parent,
                                           const wxString& message,
                                           const wxString& caption,
                                           int n,
                                           const wxString *choices,
                                           char **clientData,
                                           long style,
                                           const wxPoint& WXUNUSED(pos))
{
    Create(parent, message, caption, n, choices, clientData, style);
}

#ifdef WXWIN_COMPATIBILITY_2

wxSingleChoiceDialog::wxSingleChoiceDialog(wxWindow *parent,
                                           const wxString& message,
                                           const wxString& caption,
                                           const wxStringList& choices,
                                           char **clientData,
                                           long style,
                                           const wxPoint& WXUNUSED(pos))
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

#endif // WXWIN_COMPATIBILITY_2

bool wxSingleChoiceDialog::Create( wxWindow *parent,
                                   const wxString& message,
                                   const wxString& caption,
                                   int n,
                                   const wxString *choices,
                                   char **clientData,
                                   long style,
                                   const wxPoint& pos )
{
    if ( !wxAnyChoiceDialog::Create(parent, message, caption,
                                    n, choices,
                                    style, pos) )
        return FALSE;

    m_selection = n > 0 ? 0 : -1;

    if (clientData)
    {
        for (int i = 0; i < n; i++)
            m_listbox->SetClientData(i, clientData[i]);
    }

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

// ----------------------------------------------------------------------------
// wxMultiChoiceDialog
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMultiChoiceDialog, wxDialog)

bool wxMultiChoiceDialog::Create( wxWindow *parent,
                                  const wxString& message,
                                  const wxString& caption,
                                  int n,
                                  const wxString *choices,
                                  long style,
                                  const wxPoint& pos )
{
    if ( !wxAnyChoiceDialog::Create(parent, message, caption,
                                    n, choices,
                                    style, pos,
                                    wxLB_ALWAYS_SB | wxLB_EXTENDED) )
        return FALSE;

    return TRUE;
}

void wxMultiChoiceDialog::SetSelections(const wxArrayInt& selections)
{
    size_t count = selections.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_listbox->Select(selections[n]);
    }
}

bool wxMultiChoiceDialog::TransferDataFromWindow()
{
    // VZ: I hate to do it but I can't fix wxMotif right now (FIXME)
#ifdef __WXMOTIF__
    #define IsSelected Selected
#endif

    m_selections.Empty();
    size_t count = m_listbox->GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        if ( m_listbox->IsSelected(n) )
            m_selections.Add(n);
    }

    return TRUE;
}

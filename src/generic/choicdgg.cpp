/////////////////////////////////////////////////////////////////////////////
// Name:        choicdgg.cpp
// Purpose:     Choice dialogs
// Author:      Julian Smart
// Modified by: 03.11.00: VZ to add wxArrayString and multiple sel functions
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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

#if wxUSE_CHOICEDLG

#ifndef WX_PRECOMP
    #include <stdio.h>
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/button.h"
    #include "wx/listbox.h"
    #include "wx/stattext.h"
    #include "wx/intl.h"
    #include "wx/sizer.h"
    #include "wx/arrstr.h"
#endif

#if wxUSE_STATLINE
    #include "wx/statline.h"
#endif

#include "wx/generic/choicdgg.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#define wxID_LISTBOX 3000

// ---------------------------------------------------------------------------
// macros
// ---------------------------------------------------------------------------

/* Macro for avoiding #ifdefs when value have to be different depending on size of
   device we display on - take it from something like wxDesktopPolicy in the future
 */

#if defined(__SMARTPHONE__)
    #define wxLARGESMALL(large,small) small
#else
    #define wxLARGESMALL(large,small) large
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

int wxGetSingleChoiceIndex( const wxString& message,
                            const wxString& caption,
                            const wxArrayString& aChoices,
                            wxWindow *parent,
                            int x, int y,
                            bool centre,
                            int width, int height)
{
    wxString *choices;
    int n = ConvertWXArrayToC(aChoices, &choices);
    int res = wxGetSingleChoiceIndex(message, caption, n, choices, parent,
                                     x, y, centre, width, height);
    delete [] choices;

    return res;
}

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

    if ( !selections.IsEmpty() )
        dialog.SetSelections(selections);

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
                               long styleDlg,
                               const wxPoint& pos,
                               long styleLbox)
{
    if ( !wxDialog::Create(parent, wxID_ANY, caption, pos, wxDefaultSize, styleDlg) )
        return false;

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    // 1) text message
    topsizer->Add( CreateTextSizer( message ), 0, wxALL, wxLARGESMALL(10,0) );

    // 2) list box
    m_listbox = new wxListBox( this, wxID_LISTBOX,
                               wxDefaultPosition, wxDefaultSize,
                               n, choices,
                               styleLbox );
    if ( n > 0 )
        m_listbox->SetSelection(0);

    topsizer->Add( m_listbox, 1, wxEXPAND | wxLEFT|wxRIGHT, wxLARGESMALL(15,0) );

    // smart phones does not support or do not waste space for wxButtons
#ifdef __SMARTPHONE__

    SetRightMenu(wxID_CANCEL, _("Cancel"));

#else // __SMARTPHONE__/!__SMARTPHONE__

#if wxUSE_STATLINE
    // 3) static line
    topsizer->Add( new wxStaticLine( this, wxID_ANY ), 0, wxEXPAND | wxLEFT|wxRIGHT|wxTOP, 10 );
#endif

    // 4) buttons
    topsizer->Add( CreateButtonSizer( styleDlg & (wxOK|wxCANCEL) ), 0, wxCENTRE | wxALL, 10 );

#endif // !__SMARTPHONE__

    SetAutoLayout( true );
    SetSizer( topsizer );

    topsizer->SetSizeHints( this );
    topsizer->Fit( this );

    if ( styleDlg & wxCENTRE )
        Centre(wxBOTH);

    m_listbox->SetFocus();

    return true;
}

bool wxAnyChoiceDialog::Create(wxWindow *parent,
                               const wxString& message,
                               const wxString& caption,
                               const wxArrayString& choices,
                               long styleDlg,
                               const wxPoint& pos,
                               long styleLbox)
{
    wxCArrayString chs(choices);
    return Create(parent, message, caption, chs.GetCount(), chs.GetStrings(),
                  styleDlg, pos, styleLbox);
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

wxSingleChoiceDialog::wxSingleChoiceDialog(wxWindow *parent,
                                           const wxString& message,
                                           const wxString& caption,
                                           const wxArrayString& choices,
                                           char **clientData,
                                           long style,
                                           const wxPoint& WXUNUSED(pos))
{
    Create(parent, message, caption, choices, clientData, style);
}

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
        return false;

    m_selection = n > 0 ? 0 : -1;

    if (clientData)
    {
        for (int i = 0; i < n; i++)
            m_listbox->SetClientData(i, clientData[i]);
    }

    return true;
}

bool wxSingleChoiceDialog::Create( wxWindow *parent,
                                   const wxString& message,
                                   const wxString& caption,
                                   const wxArrayString& choices,
                                   char **clientData,
                                   long style,
                                   const wxPoint& pos )
{
    wxCArrayString chs(choices);
    return Create( parent, message, caption, chs.GetCount(), chs.GetStrings(),
                   clientData, style, pos );
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
    if ( m_listbox->HasClientUntypedData() )
        SetClientData(m_listbox->GetClientData(m_selection));
    EndModal(wxID_OK);
}

void wxSingleChoiceDialog::OnListBoxDClick(wxCommandEvent& WXUNUSED(event))
{
    m_selection = m_listbox->GetSelection();
    m_stringSelection = m_listbox->GetStringSelection();

    if ( m_listbox->HasClientUntypedData() )
        SetClientData(m_listbox->GetClientData(m_selection));

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
        return false;

    return true;
}

bool wxMultiChoiceDialog::Create( wxWindow *parent,
                                  const wxString& message,
                                  const wxString& caption,
                                  const wxArrayString& choices,
                                  long style,
                                  const wxPoint& pos )
{
    wxCArrayString chs(choices);
    return Create( parent, message, caption, chs.GetCount(),
                   chs.GetStrings(), style, pos );
}

void wxMultiChoiceDialog::SetSelections(const wxArrayInt& selections)
{
    // first clear all currently selected items
    size_t n,
           count = m_listbox->GetCount();
    for ( n = 0; n < count; ++n )
    {
        m_listbox->Deselect(n);
    }

    // now select the ones which should be selected
    count = selections.GetCount();
    for ( n = 0; n < count; n++ )
    {
        m_listbox->Select(selections[n]);
    }
}

bool wxMultiChoiceDialog::TransferDataFromWindow()
{
    m_selections.Empty();
    size_t count = m_listbox->GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        if ( m_listbox->IsSelected(n) )
            m_selections.Add(n);
    }

    return true;
}

#endif // wxUSE_CHOICEDLG

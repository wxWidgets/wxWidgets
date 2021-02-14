/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/choicdgg.cpp
// Purpose:     Choice dialogs
// Author:      Julian Smart
// Modified by: 03.11.00: VZ to add wxArrayString and multiple sel functions
// Created:     04/01/98
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_CHOICEDLG

#ifndef WX_PRECOMP
    #include <stdio.h>
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/button.h"
    #include "wx/listbox.h"
    #include "wx/checklst.h"
    #include "wx/stattext.h"
    #include "wx/intl.h"
    #include "wx/sizer.h"
    #include "wx/arrstr.h"
#endif

#include "wx/statline.h"
#include "wx/settings.h"
#include "wx/generic/choicdgg.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#define wxID_LISTBOX 3000

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wrapper functions
// ----------------------------------------------------------------------------

wxString wxGetSingleChoice( const wxString& message,
                            const wxString& caption,
                            int n, const wxString *choices,
                            wxWindow *parent,
                            int WXUNUSED(x), int WXUNUSED(y),
                            bool WXUNUSED(centre),
                            int WXUNUSED(width), int WXUNUSED(height),
                            int initialSelection)
{
    wxSingleChoiceDialog dialog(parent, message, caption, n, choices);

    dialog.SetSelection(initialSelection);
    return dialog.ShowModal() == wxID_OK ? dialog.GetStringSelection() : wxString();
}

wxString wxGetSingleChoice( const wxString& message,
                            const wxString& caption,
                            const wxArrayString& choices,
                            wxWindow *parent,
                            int WXUNUSED(x), int WXUNUSED(y),
                            bool WXUNUSED(centre),
                            int WXUNUSED(width), int WXUNUSED(height),
                            int initialSelection)
{
    wxSingleChoiceDialog dialog(parent, message, caption, choices);

    dialog.SetSelection(initialSelection);
    return dialog.ShowModal() == wxID_OK ? dialog.GetStringSelection() : wxString();
}

wxString wxGetSingleChoice( const wxString& message,
                            const wxString& caption,
                            const wxArrayString& choices,
                            int initialSelection,
                            wxWindow *parent)
{
    return wxGetSingleChoice(message, caption, choices, parent,
                             wxDefaultCoord, wxDefaultCoord,
                             true, wxCHOICE_WIDTH, wxCHOICE_HEIGHT,
                             initialSelection);
}

wxString wxGetSingleChoice( const wxString& message,
                            const wxString& caption,
                            int n, const wxString *choices,
                            int initialSelection,
                            wxWindow *parent)
{
    return wxGetSingleChoice(message, caption, n, choices, parent,
                             wxDefaultCoord, wxDefaultCoord,
                             true, wxCHOICE_WIDTH, wxCHOICE_HEIGHT,
                             initialSelection);
}

int wxGetSingleChoiceIndex( const wxString& message,
                            const wxString& caption,
                            int n, const wxString *choices,
                            wxWindow *parent,
                            int WXUNUSED(x), int WXUNUSED(y),
                            bool WXUNUSED(centre),
                            int WXUNUSED(width), int WXUNUSED(height),
                            int initialSelection)
{
    wxSingleChoiceDialog dialog(parent, message, caption, n, choices);

    dialog.SetSelection(initialSelection);
    return dialog.ShowModal() == wxID_OK ? dialog.GetSelection() : -1;
}

int wxGetSingleChoiceIndex( const wxString& message,
                            const wxString& caption,
                            const wxArrayString& choices,
                            wxWindow *parent,
                            int WXUNUSED(x), int WXUNUSED(y),
                            bool WXUNUSED(centre),
                            int WXUNUSED(width), int WXUNUSED(height),
                            int initialSelection)
{
    wxSingleChoiceDialog dialog(parent, message, caption, choices);

    dialog.SetSelection(initialSelection);
    return dialog.ShowModal() == wxID_OK ? dialog.GetSelection() : -1;
}

int wxGetSingleChoiceIndex( const wxString& message,
                            const wxString& caption,
                            const wxArrayString& choices,
                            int initialSelection,
                            wxWindow *parent)
{
    return wxGetSingleChoiceIndex(message, caption, choices, parent,
                                  wxDefaultCoord, wxDefaultCoord,
                                  true, wxCHOICE_WIDTH, wxCHOICE_HEIGHT,
                                  initialSelection);
}


int wxGetSingleChoiceIndex( const wxString& message,
                            const wxString& caption,
                            int n, const wxString *choices,
                            int initialSelection,
                            wxWindow *parent)
{
    return wxGetSingleChoiceIndex(message, caption, n, choices, parent,
                                  wxDefaultCoord, wxDefaultCoord,
                                  true, wxCHOICE_WIDTH, wxCHOICE_HEIGHT,
                                  initialSelection);
}


void *wxGetSingleChoiceData( const wxString& message,
                             const wxString& caption,
                             int n, const wxString *choices,
                             void **client_data,
                             wxWindow *parent,
                             int WXUNUSED(x), int WXUNUSED(y),
                             bool WXUNUSED(centre),
                             int WXUNUSED(width), int WXUNUSED(height),
                             int initialSelection)
{
    wxSingleChoiceDialog dialog(parent, message, caption, n, choices,
                                client_data);

    dialog.SetSelection(initialSelection);
    return dialog.ShowModal() == wxID_OK ? dialog.GetSelectionData() : NULL;
}

void *wxGetSingleChoiceData( const wxString& message,
                             const wxString& caption,
                             const wxArrayString& choices,
                             void **client_data,
                             wxWindow *parent,
                             int WXUNUSED(x), int WXUNUSED(y),
                             bool WXUNUSED(centre),
                             int WXUNUSED(width), int WXUNUSED(height),
                             int initialSelection)
{
    wxSingleChoiceDialog dialog(parent, message, caption, choices, client_data);

    dialog.SetSelection(initialSelection);
    return dialog.ShowModal() == wxID_OK ? dialog.GetSelectionData() : NULL;
}

void* wxGetSingleChoiceData( const wxString& message,
                             const wxString& caption,
                             const wxArrayString& choices,
                             void **client_data,
                             int initialSelection,
                             wxWindow *parent)
{
    return wxGetSingleChoiceData(message, caption, choices,
                                 client_data, parent,
                                 wxDefaultCoord, wxDefaultCoord,
                                 true, wxCHOICE_WIDTH, wxCHOICE_HEIGHT,
                                 initialSelection);
}

void* wxGetSingleChoiceData( const wxString& message,
                             const wxString& caption,
                             int n, const wxString *choices,
                             void **client_data,
                             int initialSelection,
                             wxWindow *parent)
{
    return wxGetSingleChoiceData(message, caption, n, choices,
                                 client_data, parent,
                                 wxDefaultCoord, wxDefaultCoord,
                                 true, wxCHOICE_WIDTH, wxCHOICE_HEIGHT,
                                 initialSelection);
}


int wxGetSelectedChoices(wxArrayInt& selections,
                         const wxString& message,
                         const wxString& caption,
                         int n, const wxString *choices,
                         wxWindow *parent,
                         int WXUNUSED(x), int WXUNUSED(y),
                         bool WXUNUSED(centre),
                         int WXUNUSED(width), int WXUNUSED(height))
{
    wxMultiChoiceDialog dialog(parent, message, caption, n, choices);

    // call this even if selections array is empty and this then (correctly)
    // deselects the first item which is selected by default
    dialog.SetSelections(selections);

    if ( dialog.ShowModal() != wxID_OK )
    {
        // NB: intentionally do not clear the selections array here, the caller
        //     might want to preserve its original contents if the dialog was
        //     cancelled
        return -1;
    }

    selections = dialog.GetSelections();
    return static_cast<int>(selections.GetCount());
}

int wxGetSelectedChoices(wxArrayInt& selections,
                         const wxString& message,
                         const wxString& caption,
                         const wxArrayString& choices,
                         wxWindow *parent,
                         int WXUNUSED(x), int WXUNUSED(y),
                         bool WXUNUSED(centre),
                         int WXUNUSED(width), int WXUNUSED(height))
{
    wxMultiChoiceDialog dialog(parent, message, caption, choices);

    // call this even if selections array is empty and this then (correctly)
    // deselects the first item which is selected by default
    dialog.SetSelections(selections);

    if ( dialog.ShowModal() != wxID_OK )
    {
        // NB: intentionally do not clear the selections array here, the caller
        //     might want to preserve its original contents if the dialog was
        //     cancelled
        return -1;
    }

    selections = dialog.GetSelections();
    return static_cast<int>(selections.GetCount());
}

#if WXWIN_COMPATIBILITY_2_8
size_t wxGetMultipleChoices(wxArrayInt& selections,
                            const wxString& message,
                            const wxString& caption,
                            int n, const wxString *choices,
                            wxWindow *parent,
                            int x, int y,
                            bool centre,
                            int width, int height)
{
    int rc = wxGetSelectedChoices(selections, message, caption,
                                  n, choices,
                                  parent, x, y, centre, width, height);
    if ( rc == -1 )
    {
        selections.clear();
        return 0;
    }

    return rc;
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
    int rc = wxGetSelectedChoices(selections, message, caption,
                                  aChoices,
                                  parent, x, y, centre, width, height);
    if ( rc == -1 )
    {
        selections.clear();
        return 0;
    }

    return rc;
}
#endif // WXWIN_COMPATIBILITY_2_8

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
    // extract the buttons styles from the dialog one and remove them from it
    const long styleBtns = styleDlg & (wxOK | wxCANCEL);
    styleDlg &= ~styleBtns;

    if ( !wxDialog::Create(GetParentForModalDialog(parent, styleDlg), wxID_ANY, caption, pos, wxDefaultSize, styleDlg) )
        return false;

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    // 1) text message
    topsizer->
        Add(CreateTextSizer(message), wxSizerFlags().Expand().TripleBorder());

    // 2) list box
    m_listbox = CreateList(n, choices, styleLbox);

    if ( n > 0 )
        m_listbox->SetSelection(0);

    topsizer->
        Add(m_listbox, wxSizerFlags().Expand().Proportion(1).TripleBorder(wxLEFT | wxRIGHT));

    // 3) buttons if any
    wxSizer *
        buttonSizer = CreateSeparatedButtonSizer(styleBtns);
    if ( buttonSizer )
    {
        topsizer->Add(buttonSizer, wxSizerFlags().Expand().DoubleBorder());
    }

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

wxListBoxBase *wxAnyChoiceDialog::CreateList(int n, const wxString *choices, long styleLbox)
{
    return new wxListBox( this, wxID_LISTBOX,
                          wxDefaultPosition, wxDefaultSize,
                          n, choices,
                          styleLbox );
}

// ----------------------------------------------------------------------------
// wxSingleChoiceDialog
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxSingleChoiceDialog, wxDialog)
    EVT_BUTTON(wxID_OK, wxSingleChoiceDialog::OnOK)
    EVT_LISTBOX_DCLICK(wxID_LISTBOX, wxSingleChoiceDialog::OnListBoxDClick)
wxEND_EVENT_TABLE()

wxIMPLEMENT_DYNAMIC_CLASS(wxSingleChoiceDialog, wxDialog);

bool wxSingleChoiceDialog::Create( wxWindow *parent,
                                   const wxString& message,
                                   const wxString& caption,
                                   int n,
                                   const wxString *choices,
                                   void **clientData,
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
                                   void **clientData,
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
    wxCHECK_RET( sel >= 0 && (unsigned)sel < m_listbox->GetCount(),
                 "Invalid initial selection" );

    m_listbox->SetSelection(sel);
    m_selection = sel;
}

void wxSingleChoiceDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
    DoChoice();
}

void wxSingleChoiceDialog::OnListBoxDClick(wxCommandEvent& WXUNUSED(event))
{
    DoChoice();
}

void wxSingleChoiceDialog::DoChoice()
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

wxIMPLEMENT_DYNAMIC_CLASS(wxMultiChoiceDialog, wxDialog);

bool wxMultiChoiceDialog::Create( wxWindow *parent,
                                  const wxString& message,
                                  const wxString& caption,
                                  int n,
                                  const wxString *choices,
                                  long style,
                                  const wxPoint& pos )
{
    long styleLbox;
#if wxUSE_CHECKLISTBOX
    styleLbox = wxLB_ALWAYS_SB;
#else
    styleLbox = wxLB_ALWAYS_SB | wxLB_EXTENDED;
#endif

    if ( !wxAnyChoiceDialog::Create(parent, message, caption,
                                    n, choices,
                                    style, pos,
                                    styleLbox) )
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
#if wxUSE_CHECKLISTBOX
    wxCheckListBox* checkListBox = wxDynamicCast(m_listbox, wxCheckListBox);
    if (checkListBox)
    {
        // first clear all currently selected items
        size_t n,
            count = checkListBox->GetCount();
        for ( n = 0; n < count; ++n )
        {
            if (checkListBox->IsChecked(n))
                checkListBox->Check(n, false);
        }

        // now select the ones which should be selected
        count = selections.GetCount();
        for ( n = 0; n < count; n++ )
        {
            checkListBox->Check(selections[n]);
        }

        return;
    }
#endif

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

#if wxUSE_CHECKLISTBOX
    wxCheckListBox* checkListBox = wxDynamicCast(m_listbox, wxCheckListBox);
    if (checkListBox)
    {
        size_t count = checkListBox->GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            if ( checkListBox->IsChecked(n) )
                m_selections.Add(n);
        }
        return true;
    }
#endif

    size_t count = m_listbox->GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        if ( m_listbox->IsSelected(n) )
            m_selections.Add(n);
    }

    return true;
}

#if wxUSE_CHECKLISTBOX

wxListBoxBase *wxMultiChoiceDialog::CreateList(int n, const wxString *choices, long styleLbox)
{
    return new wxCheckListBox( this, wxID_LISTBOX,
                               wxDefaultPosition, wxDefaultSize,
                               n, choices,
                               styleLbox );
}

#endif // wxUSE_CHECKLISTBOX

#endif // wxUSE_CHOICEDLG

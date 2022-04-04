/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/numdlgg.cpp
// Purpose:     wxGetNumberFromUser implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.07.99
// Copyright:   (c) Vadim Zeitlin
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


#if wxUSE_NUMBERDLG

#ifndef WX_PRECOMP
    #include <stdio.h>

    #include "wx/dialog.h"
    #include "wx/button.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
    #include "wx/intl.h"
    #include "wx/sizer.h"
    #include "wx/wxcrtvararg.h"
#endif

#if wxUSE_STATLINE
  #include "wx/statline.h"
#endif

#if wxUSE_SPINCTRL
#include "wx/spinctrl.h"
#endif

// this is where wxGetNumberFromUser() is declared
#include "wx/numdlg.h"

#if !wxUSE_SPINCTRL
    // wxTextCtrl will do instead of wxSpinCtrl if we don't have it
    #define wxSpinCtrl wxTextCtrl
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxNumberEntryDialog
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxNumberEntryDialog, wxDialog)
    EVT_BUTTON(wxID_OK, wxNumberEntryDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, wxNumberEntryDialog::OnCancel)
wxEND_EVENT_TABLE()

wxIMPLEMENT_CLASS(wxNumberEntryDialog, wxDialog);

bool wxNumberEntryDialog::Create(wxWindow *parent,
                                         const wxString& message,
                                         const wxString& prompt,
                                         const wxString& caption,
                                         long value,
                                         long min,
                                         long max,
                                         const wxPoint& pos)
{
    if ( !wxDialog::Create(GetParentForModalDialog(parent, 0),
                           wxID_ANY, caption,
                           pos, wxDefaultSize) )
    {
        return false;
    }

    m_value = value;
    m_max = max;
    m_min = min;

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
#if wxUSE_STATTEXT
    // 1) text message
    topsizer->Add( CreateTextSizer( message ), wxSizerFlags().DoubleBorder() );
#endif

    // 2) prompt and text ctrl
    wxBoxSizer *inputsizer = new wxBoxSizer( wxHORIZONTAL );

#if wxUSE_STATTEXT
    // prompt if any
    if (!prompt.empty())
        inputsizer->Add( new wxStaticText( this, wxID_ANY, prompt ),
                         wxSizerFlags().Center().DoubleBorder(wxLEFT) );
#endif

    // spin ctrl
    wxString valStr;
    valStr.Printf(wxT("%ld"), m_value);
#if wxUSE_SPINCTRL
    m_spinctrl = new wxSpinCtrl(this, wxID_ANY, valStr, wxDefaultPosition, wxSize( 140, wxDefaultCoord ), wxSP_ARROW_KEYS, (int)m_min, (int)m_max, (int)m_value);
#else
    m_spinctrl = new wxTextCtrl(this, wxID_ANY, valStr, wxDefaultPosition, wxSize( 140, wxDefaultCoord ));
#endif
    inputsizer->Add( m_spinctrl, wxSizerFlags(1).Center().DoubleBorder(wxLEFT | wxRIGHT));
    // add both
    topsizer->Add( inputsizer, wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT));

    // 3) buttons if any
    wxSizer *buttonSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
    if ( buttonSizer )
    {
        topsizer->Add(buttonSizer, wxSizerFlags().Expand().DoubleBorder());
    }

    SetSizer( topsizer );

    topsizer->SetSizeHints( this );

    Centre( wxBOTH );

    m_spinctrl->SetSelection(-1, -1);
    m_spinctrl->SetFocus();

    return true;
}

void wxNumberEntryDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
#if !wxUSE_SPINCTRL
    wxString tmp = m_spinctrl->GetValue();
    if ( wxSscanf(tmp, wxT("%ld"), &m_value) != 1 )
        EndModal(wxID_CANCEL);
    else
#else
    m_value = m_spinctrl->GetValue();
#endif
    if ( m_value < m_min || m_value > m_max )
    {
        // not a number or out of range
        m_value = -1;
        EndModal(wxID_CANCEL);
    }

    EndModal(wxID_OK);
}

void wxNumberEntryDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    EndModal(wxID_CANCEL);
}

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

// wxGetTextFromUser is in utilscmn.cpp

long wxGetNumberFromUser(const wxString& msg,
                         const wxString& prompt,
                         const wxString& title,
                         long value,
                         long min,
                         long max,
                         wxWindow *parent,
                         const wxPoint& pos)
{
    wxNumberEntryDialog dialog(parent, msg, prompt, title,
                               value, min, max, pos);
    if (dialog.ShowModal() == wxID_OK)
        return dialog.GetValue();

    return -1;
}

#endif // wxUSE_NUMBERDLG

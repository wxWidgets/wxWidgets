/////////////////////////////////////////////////////////////////////////////
// Name:        numdlgg.cpp
// Purpose:     wxGetNumberFromUser implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.07.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma interface "numdlgg.cpp"
    #pragma implementation "numdlgg.cpp"
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
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
    #include "wx/intl.h"
    #include "wx/sizer.h"
#endif

#if wxUSE_STATLINE
  #include "wx/statline.h"
#endif

#include "wx/spinctrl.h"

// this is where wxGetNumberFromUser() is declared
#include "wx/generic/textdlgg.h"

#if !wxUSE_SPINCTRL
    // wxTextCtrl will do instead of wxSpinCtrl if we don't have it
    #define wxSpinCtrl wxTextCtrl
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxNumberEntryDialog : public wxDialog
{
public:
    wxNumberEntryDialog(wxWindow *parent,
                        const wxString& message,
                        const wxString& prompt,
                        const wxString& caption,
                        long value, long min, long max,
                        const wxPoint& pos);

    long GetValue() const { return m_value; }

    // implementation only
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

protected:
    wxSpinCtrl *m_spinctrl;

    long m_value, m_min, m_max;

private:
    DECLARE_EVENT_TABLE()
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxNumberEntryDialog
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxNumberEntryDialog, wxDialog)
    EVT_BUTTON(wxID_OK, wxNumberEntryDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, wxNumberEntryDialog::OnCancel)
END_EVENT_TABLE()

wxNumberEntryDialog::wxNumberEntryDialog(wxWindow *parent,
                                         const wxString& message,
                                         const wxString& prompt,
                                         const wxString& caption,
                                         long value,
                                         long min,
                                         long max,
                                         const wxPoint& pos)
                   : wxDialog(parent, -1, caption,
                              pos, wxDefaultSize,
                              wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
    m_value = value;
    m_max = max;
    m_min = min;

    wxBeginBusyCursor();

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    // 1) text message
    topsizer->Add( CreateTextSizer( message ), 0, wxALL, 10 );

    // 2) prompt and text ctrl
    wxBoxSizer *inputsizer = new wxBoxSizer( wxHORIZONTAL );
    // prompt if any
    if (!prompt.IsEmpty())
        inputsizer->Add( new wxStaticText( this, -1, prompt ), 0, wxCENTER | wxLEFT, 10 );
    // spin ctrl
    wxString valStr;
    valStr.Printf(wxT("%lu"), m_value);
    m_spinctrl = new wxSpinCtrl(this, -1, valStr, wxDefaultPosition, wxSize( 140, -1 ) );
    m_spinctrl->SetRange((int)m_min, (int)m_max);
    inputsizer->Add( m_spinctrl, 1, wxCENTER | wxLEFT | wxRIGHT, 10 );
    // add both
    topsizer->Add( inputsizer, 1, wxEXPAND | wxLEFT|wxRIGHT, 5 );

#if wxUSE_STATLINE
    // 3) static line
    topsizer->Add( new wxStaticLine( this, -1 ), 0, wxEXPAND | wxLEFT|wxRIGHT|wxTOP, 10 );
#endif

    // 4) buttons
    topsizer->Add( CreateButtonSizer( wxOK|wxCANCEL ), 0, wxCENTRE | wxALL, 10 );

    SetSizer( topsizer );
    SetAutoLayout( TRUE );

    topsizer->SetSizeHints( this );
    topsizer->Fit( this );

    Centre( wxBOTH );

    m_spinctrl->SetFocus();

    wxEndBusyCursor();
}

void wxNumberEntryDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
#if !wxUSE_SPINCTRL
    wxString tmp = m_spinctrl->GetValue();
    if ( wxSscanf(tmp, _T("%ld"), &m_value) != 1 )
        m_value = -1;
    else
#else
    m_value = m_spinctrl->GetValue();
#endif
    if ( m_value < m_min || m_value > m_max )
    {
        // not a number or out of range
        m_value = -1;
    }

    EndModal(wxID_OK);
}

void wxNumberEntryDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    m_value = -1;

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
    (void)dialog.ShowModal();

    return dialog.GetValue();
}

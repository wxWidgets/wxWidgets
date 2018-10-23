///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/creddlgg.h
// Purpose:     wxGenericCredentialEntryDialog implementation
// Author:      Tobias Taschner
// Created:     2018-10-23
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_CREDENTIALDLG

#ifndef WX_PRECOMP
    #include "wx/dialog.h"
    #include "wx/button.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
    #include "wx/intl.h"
    #include "wx/sizer.h"
#endif

#include "wx/generic/creddlgg.h"

wxGenericCredentialEntryDialog::wxGenericCredentialEntryDialog()
{
}

wxGenericCredentialEntryDialog::wxGenericCredentialEntryDialog(
    wxWindow* parent, const wxString& message, const wxString& title,
    const wxString& user, const wxString& password):
    wxDialog(parent, wxID_ANY, title)
{
    Init(message, user, password);
}

bool wxGenericCredentialEntryDialog::Create(wxWindow* parent,
    const wxString& message, const wxString& title, const wxString& user,
    const wxString& password)
{
    if ( !wxDialog::Create(parent, wxID_ANY, title) )
        return false;

    Init(message, user, password);
    return true;
}

void wxGenericCredentialEntryDialog::Init(const wxString& message,
    const wxString& user, const wxString& password)
{
    wxSizer* topsizer = new wxBoxSizer(wxVERTICAL);

    topsizer->Add(CreateTextSizer(message), wxSizerFlags().Border());

    topsizer->Add(new wxStaticText(this, wxID_ANY, _("Username:")),
        wxSizerFlags().Border(wxLEFT | wxRIGHT));
    m_userTextCtrl = new wxTextCtrl(this, wxID_ANY, user, wxDefaultPosition, wxSize(FromDIP(300), wxDefaultCoord));
    topsizer->Add(m_userTextCtrl, wxSizerFlags().Expand().Border());

    topsizer->Add(new wxStaticText(this, wxID_ANY, _("Password:")),
        wxSizerFlags().Border(wxLEFT | wxRIGHT));
    m_passwordTextCtrl = new wxTextCtrl(this, wxID_ANY, password,
        wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    topsizer->Add(m_passwordTextCtrl, wxSizerFlags().Expand().Border());

    topsizer->Add(CreateStdDialogButtonSizer(wxOK | wxCANCEL), wxSizerFlags().Expand().Border());
    SetSizerAndFit(topsizer);

    m_userTextCtrl->SetFocus();
}

#endif // wxUSE_CREDENTIALDLG

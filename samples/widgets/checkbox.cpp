/////////////////////////////////////////////////////////////////////////////
// Program:     wxWindows Widgets Sample
// Name:        checkbox.cpp
// Purpose:     Part of the widgets sample showing wxCheckBox
// Author:      Dimitri Schoolwerth
// Created:     27 Sep 2003
// Id:          $Id$
// Copyright:   (c) 2003 wxWindows team
// License:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"

    #include "wx/bitmap.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"

    #include "wx/sizer.h"

#endif

#include "widgets.h"

#include "icons/checkbox.xpm"

// ----------------------------------------------------------------------------
// CheckBoxWidgetsPage
// ----------------------------------------------------------------------------

class CheckBoxWidgetsPage : public WidgetsPage
{
public:
    CheckBoxWidgetsPage(wxNotebook *notebook, wxImageList *imaglist);
    virtual ~CheckBoxWidgetsPage();

protected:
    // event handlers
    void OnCheckBox(wxCommandEvent& event);

    void OnButton(wxCommandEvent& event);

    // the controls
    // ------------

    wxCheckBox *m_chk2States,
               *m_chk3States,
               *m_chk3StatesAllows3rdStateForUser;

    wxButton *m_button;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(CheckBoxWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CheckBoxWidgetsPage, WidgetsPage)
    EVT_CHECKBOX(wxID_ANY, CheckBoxWidgetsPage::OnCheckBox)
    EVT_BUTTON(wxID_ANY, CheckBoxWidgetsPage::OnButton)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_WIDGETS_PAGE(CheckBoxWidgetsPage, wxT("CheckBox"));

CheckBoxWidgetsPage::CheckBoxWidgetsPage(wxNotebook *notebook,
                                       wxImageList *imaglist)
                  : WidgetsPage(notebook)
{
    imaglist->Add(wxBitmap(checkbox_xpm));

    m_chk2States = new wxCheckBox( this, wxID_ANY,
        wxT("I'm a standard 2-state checkbox") );
    m_chk3States = new wxCheckBox( this, wxID_ANY,
        wxT("I'm a 3-state checkbox that disallows setting the undetermined")
        wxT(" state by the user" ),
        wxDefaultPosition, wxDefaultSize, wxCHK_3STATE);
    m_button = new wxButton( this, wxID_ANY, wxT("&Programmatically set this")
        wxT(" checkbox to undetermined state") );

    m_chk3StatesAllows3rdStateForUser = new wxCheckBox(this, wxID_ANY,
        wxT("I'm a 3-state checkbox that allows setting the 3rd state by the user"),
        wxDefaultPosition, wxDefaultSize, wxCHK_3STATE
        | wxCHK_ALLOW_3RD_STATE_FOR_USER);

    wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

    sizerTop->Add(0, 0, 1, wxEXPAND);
    sizerTop->Add(m_chk2States, 0, wxEXPAND);
    sizerTop->Add(0, 0, 1, wxEXPAND);
    wxSizer *sizerCheckBoxAndButton = new wxBoxSizer(wxHORIZONTAL);
    {
        wxSizer *szr = sizerCheckBoxAndButton;
        szr->Add(m_chk3States, 0, wxEXPAND);
        szr->Add(0, 0, 1, wxEXPAND);
        szr->Add(m_button, 0, wxEXPAND);

        sizerTop->Add(szr, 0, wxEXPAND);
    }

    sizerTop->Add(0, 0, 1, wxEXPAND);
    sizerTop->Add(m_chk3StatesAllows3rdStateForUser, 0, wxEXPAND);
    sizerTop->Add(0, 0, 1, wxEXPAND);

    SetSizer(sizerTop);

    sizerTop->Fit(this);
}

CheckBoxWidgetsPage::~CheckBoxWidgetsPage()
{
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void CheckBoxWidgetsPage::OnCheckBox(wxCommandEvent& event)
{
    static const wxString stateNames[] =
    {
        wxT("unchecked"),
        wxT("checked"),
        wxT("undetermined/mixed"),
    };
    wxCheckBoxState state = (wxCheckBoxState) event.GetInt();

    wxCHECK_RET( (state >= 0) && (state < WXSIZEOF(stateNames)),
        "event.GetInt() returned an invalid wxCheckBoxState" );

    wxLogMessage(wxT("Checkbox now set to state: %s"),
        stateNames[state].c_str());
}

void CheckBoxWidgetsPage::OnButton(wxCommandEvent& WXUNUSED(event))
{
    m_chk3States->Set3StateValue(wxCHK_UNDETERMINED);
}

/////////////////////////////////////////////////////////////////////////////
// Name:        settingsdialog.cpp
// Purpose:     Settings dialog
// Author:      Julian Smart
// Modified by:
// Created:     2003-06-12
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "settingsdialog.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP

#include "wx/statline.h"
#include "wx/splitter.h"
#include "wx/scrolwin.h"
#include "wx/spinctrl.h"
#include "wx/spinbutt.h"
#include "wx/sizer.h"
#include "wx/statbox.h"
#include "wx/dirdlg.h"

#endif

#include "wx/cshelp.h"
#include "wx/notebook.h"
#include "wx/valgen.h"
#include "wxconfigtool.h"
#include "settingsdialog.h"

////@begin XPM images
////@end XPM images

/*!
 * ctSettingsDialog type definition
 */

IMPLEMENT_CLASS( ctSettingsDialog, wxDialog )

/*!
 * ctSettingsDialog event table definition
 */

BEGIN_EVENT_TABLE( ctSettingsDialog, wxDialog )

////@begin ctSettingsDialog event table entries
    EVT_BUTTON( wxID_OK, ctSettingsDialog::OnOk )

    EVT_BUTTON( wxID_CANCEL, ctSettingsDialog::OnCancel )

    EVT_BUTTON( wxID_HELP, ctSettingsDialog::OnHelp )

////@end ctSettingsDialog event table entries

END_EVENT_TABLE()

/*!
 * ctSettingsDialog constructor
 */

ctSettingsDialog::ctSettingsDialog( wxWindow* parent )
{
    SetExtraStyle(wxDIALOG_EX_CONTEXTHELP|wxWS_EX_VALIDATE_RECURSIVELY);
    wxDialog::Create( parent, wxID_ANY, _("Configuration Settings"));
    CreateControls();
}

/*!
 * Control creation for ctSettingsDialog
 */

void ctSettingsDialog::CreateControls()
{
////@begin ctSettingsDialog content construction

    ctSettingsDialog* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);

    m_notebook = new wxNotebook(item1, wxID_ANY, wxDefaultPosition, wxSize(200, 200), wxNB_TOP);
    ctGeneralSettingsDialog* item4 = new ctGeneralSettingsDialog(m_notebook, ID_GENERAL_SETTINGS_DIALOG, wxDefaultPosition, wxSize(100, 80), 0);
    m_notebook->AddPage(item4, _("General"));
    ctLocationSettingsDialog* item11 = new ctLocationSettingsDialog(m_notebook, ID_LOCATION_SETTINGS_DIALOG, wxDefaultPosition, wxSize(100, 80), 0);
    m_notebook->AddPage(item11, _("Locations"));
    item2->Add(m_notebook, 0, wxGROW|wxALL, 5);

    wxBoxSizer* item21 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item21, 0, wxGROW|wxALL, 5);

    item21->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item23 = new wxButton(item1, wxID_OK);
    item23->SetDefault();
    item21->Add(item23, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item24 = new wxButton(item1, wxID_CANCEL);
    item21->Add(item24, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item25 = new wxButton(item1, wxID_HELP);
    item21->Add(item25, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

#if defined(__WXGTK__) || defined(__WXMAC__)
    wxContextHelpButton* item26 = new wxContextHelpButton(item1, wxID_CONTEXT_HELP, wxDefaultPosition, wxSize(20, wxDefaultCoord), wxBU_AUTODRAW);
    item21->Add(item26, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
#endif

    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end ctSettingsDialog content construction
}

/*!
 * Event handler for wxID_OK
 */

void ctSettingsDialog::OnOk( wxCommandEvent& event )
{
    // Replace with custom code
    event.Skip();
}

/*!
 * Event handler for wxID_CANCEL
 */

void ctSettingsDialog::OnCancel( wxCommandEvent& event )
{
    // Replace with custom code
    event.Skip();
}

/*!
 * Event handler for wxID_HELP
 */

void ctSettingsDialog::OnHelp( wxCommandEvent& WXUNUSED(event) )
{
    if(m_notebook)
    {
        int sel = m_notebook->GetSelection();

        wxASSERT_MSG( (sel != -1), wxT("A notebook tab should always be selected."));

        wxWindow* page = (wxWindow*) m_notebook->GetPage(sel);

        wxString helpTopic;
        if (page->GetId() == ID_GENERAL_SETTINGS_DIALOG)
        {
            helpTopic = wxT("General settings dialog");
        }
        else if (page->GetId() == ID_LOCATION_SETTINGS_DIALOG)
        {
            helpTopic = wxT("Location settings dialog");
        }

        if (!helpTopic.empty())
        {
            wxGetApp().GetHelpController().DisplaySection(helpTopic);
        }
    }
}

/*!
 * Should we show tooltips?
 */

bool ctSettingsDialog::ShowToolTips()
{
  return true;
}

/*!
 * ctGeneralSettingsDialog type definition
 */

IMPLEMENT_CLASS( ctGeneralSettingsDialog, wxPanel )

/*!
 * ctGeneralSettingsDialog event table definition
 */

BEGIN_EVENT_TABLE( ctGeneralSettingsDialog, wxPanel )

////@begin ctGeneralSettingsDialog event table entries
////@end ctGeneralSettingsDialog event table entries

END_EVENT_TABLE()

/*!
 * ctGeneralSettingsDialog constructor
 */

ctGeneralSettingsDialog::ctGeneralSettingsDialog( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
}

/*!
 * Control creation for ctGeneralSettingsDialog
 */

void ctGeneralSettingsDialog::CreateControls()
{
////@begin ctGeneralSettingsDialog content construction

    ctGeneralSettingsDialog* item4 = this;

    wxBoxSizer* item5 = new wxBoxSizer(wxVERTICAL);
    item4->SetSizer(item5);

    wxStaticBox* item6Static = new wxStaticBox(item4, wxID_ANY, _("General settings"));
    wxStaticBoxSizer* item6 = new wxStaticBoxSizer(item6Static, wxVERTICAL);
    item5->Add(item6, 1, wxGROW|wxALL, 5);

    wxCheckBox* item7 = new wxCheckBox(item4, ID_LOAD_LAST_DOCUMENT, _("&Load last document"), wxDefaultPosition, wxDefaultSize, 0);
    item7->SetValue(false);
    item7->SetHelpText(_("Check to load the last document on startup"));
#if wxUSE_TOOLTIPS
    if (ShowToolTips())
        item7->SetToolTip(_("Check to load the last document on startup"));
#endif
    item6->Add(item7, 0, wxALIGN_LEFT|wxALL, 5);

    wxCheckBox* item8 = new wxCheckBox(item4, ID_SHOW_TOOLTIPS, _("&Show tooltips"), wxDefaultPosition, wxDefaultSize, 0);
    item8->SetValue(false);
    item8->SetHelpText(_("Check to show tooltips"));
#if wxUSE_TOOLTIPS
    if (ShowToolTips())
        item8->SetToolTip(_("Check to show tooltips"));
#endif
    item6->Add(item8, 0, wxALIGN_LEFT|wxALL, 5);

    wxStaticText* item9 = new wxStaticText(item4, wxID_STATIC, _("&Default file kind to save when using the Go command:"), wxDefaultPosition, wxDefaultSize, 0);
    item6->Add(item9, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString item10Strings[] = {
        _("Setup file"),
        _("Configure script")
    };
    wxChoice* item10 = new wxChoice(item4, ID_DEFAULT_FILE_KIND, wxDefaultPosition, wxSize(200, wxDefaultCoord), 2, item10Strings, 0);
    item10->SetStringSelection(_("Setup file"));
    item10->SetHelpText(_("Select the default kind of file to save using Go"));
#if wxUSE_TOOLTIPS
    if (ShowToolTips())
        item10->SetToolTip(_("Select the default kind of file to save using Go"));
#endif
    item6->Add(item10, 0, wxGROW|wxALL, 5);

    GetSizer()->Fit(this);
////@end ctGeneralSettingsDialog content construction

    item7->SetValidator(wxGenericValidator(& wxGetApp().GetSettings().m_loadLastDocument));
    item8->SetValidator(wxGenericValidator(& wxGetApp().GetSettings().m_useToolTips));
    item10->SetValidator(wxGenericValidator(& wxGetApp().GetSettings().m_defaultFileKind));
}

/*!
 * Should we show tooltips?
 */

bool ctGeneralSettingsDialog::ShowToolTips()
{
  return true;
}

/*!
 * ctLocationSettingsDialog type definition
 */

IMPLEMENT_CLASS( ctLocationSettingsDialog, wxPanel )

/*!
 * ctLocationSettingsDialog event table definition
 */

BEGIN_EVENT_TABLE( ctLocationSettingsDialog, wxPanel )

////@begin ctLocationSettingsDialog event table entries
    EVT_UPDATE_UI( ID_WXWIN_HIERARCHY, ctLocationSettingsDialog::OnUpdateWxwinHierarchy )

    EVT_BUTTON( ID_CHOOSE_WXWIN_HIERARCHY, ctLocationSettingsDialog::OnChooseWxwinHierarchy )
    EVT_UPDATE_UI( ID_CHOOSE_WXWIN_HIERARCHY, ctLocationSettingsDialog::OnUpdateChooseWxwinHierarchy )

////@end ctLocationSettingsDialog event table entries

END_EVENT_TABLE()

/*!
 * ctLocationSettingsDialog constructor
 */

ctLocationSettingsDialog::ctLocationSettingsDialog( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
}

/*!
 * Control creation for ctLocationSettingsDialog
 */

void ctLocationSettingsDialog::CreateControls()
{
////@begin ctLocationSettingsDialog content construction

    ctLocationSettingsDialog* item11 = this;

    wxBoxSizer* item12 = new wxBoxSizer(wxVERTICAL);
    item11->SetSizer(item12);

    wxStaticBox* item13Static = new wxStaticBox(item11, wxID_ANY, _("Locations"));
    wxStaticBoxSizer* item13 = new wxStaticBoxSizer(item13Static, wxVERTICAL);
    item12->Add(item13, 1, wxGROW|wxALL, 5);

    wxStaticText* item14 = new wxStaticText(item11, wxID_STATIC, _("&wxWidgets hierarchy:"), wxDefaultPosition, wxDefaultSize, 0);
    item13->Add(item14, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* item15 = new wxBoxSizer(wxHORIZONTAL);
    item13->Add(item15, 0, wxGROW, 5);

    m_wxWinHierarchy = new wxTextCtrl(item11, ID_WXWIN_HIERARCHY, wxEmptyString, wxDefaultPosition, wxSize(200, wxDefaultCoord), 0);
    m_wxWinHierarchy->SetHelpText(_("Enter the root path of the wxWidgets hierarchy"));
#if wxUSE_TOOLTIPS
    if (ShowToolTips())
        m_wxWinHierarchy->SetToolTip(_("Enter the root path of the wxWidgets hierarchy"));
#endif
    item15->Add(m_wxWinHierarchy, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item17 = new wxButton(item11, ID_CHOOSE_WXWIN_HIERARCHY, _("&Choose..."), wxDefaultPosition, wxDefaultSize, 0);
    item17->SetHelpText(_("Click to choose the root path of the wxWidgets hierarchy\\n"));
#if wxUSE_TOOLTIPS
    if (ShowToolTips())
        item17->SetToolTip(_("Click to choose the root path of the wxWidgets hierarchy\\n"));
#endif
    item15->Add(item17, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* item18 = new wxBoxSizer(wxHORIZONTAL);
    item13->Add(item18, 0, wxGROW, 5);

    item18->Add(60, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_wxWinUse = new wxCheckBox(item11, ID_USE_WXWIN, _("&Use WXWIN environment variable"));
    m_wxWinUse->SetValue(false);
    m_wxWinUse->SetHelpText(_("Check to use the value of WXWIN instead"));
#if wxUSE_TOOLTIPS
    if (ShowToolTips())
        m_wxWinUse->SetToolTip(_("Check to use the value of WXWIN instead"));
#endif
    item18->Add(m_wxWinUse, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    GetSizer()->Fit(this);
////@end ctLocationSettingsDialog content construction

    m_wxWinHierarchy->SetValidator(wxGenericValidator(& wxGetApp().GetSettings().m_frameworkDir));
    m_wxWinUse->SetValidator(wxGenericValidator(& wxGetApp().GetSettings().m_useEnvironmentVariable));
}

/*!
 * Update event handler for ID_WXWIN_HIERARCHY
 */

void ctLocationSettingsDialog::OnUpdateWxwinHierarchy( wxUpdateUIEvent& event )
{
    if(m_wxWinUse)
        event.Enable(!m_wxWinUse->GetValue());
}

/*!
 * Event handler for ID_CHOOSE_WXWIN_HIERARCHY
 */

void ctLocationSettingsDialog::OnChooseWxwinHierarchy( wxCommandEvent& WXUNUSED(event) )
{
    if (m_wxWinHierarchy)
    {
        wxString defaultPath = m_wxWinHierarchy->GetValue();

        wxDirDialog dialog(this, _("Choose the location of the wxWidgets hierarchy"),
                defaultPath);
        if (dialog.ShowModal() == wxID_OK)
        {
            m_wxWinHierarchy->SetValue(dialog.GetPath());
        }
    }
}

/*!
 * Update event handler for ID_CHOOSE_WXWIN_HIERARCHY
 */

void ctLocationSettingsDialog::OnUpdateChooseWxwinHierarchy( wxUpdateUIEvent& event )
{
    if (m_wxWinUse)
        event.Enable(!m_wxWinUse->GetValue());
}

/*!
 * Should we show tooltips?
 */

bool ctLocationSettingsDialog::ShowToolTips()
{
    return wxGetApp().GetSettings().m_useToolTips;
}

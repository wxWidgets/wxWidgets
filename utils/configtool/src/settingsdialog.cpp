/////////////////////////////////////////////////////////////////////////////
// Name:        settingsdialog.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "settingsdialog.h"
#endif

#include "wx/wx.h"
#include "wx/cshelp.h"
#include "wx/statline.h"
#include "wx/splitter.h"
#include "wx/scrolwin.h"
#include "wx/spinctrl.h"
#include "wx/spinbutt.h"
#include "wx/valgen.h"
#include "wx/notebook.h"

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

ctSettingsDialog::ctSettingsDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(wxDIALOG_EX_CONTEXTHELP|wxWS_EX_VALIDATE_RECURSIVELY);
    wxDialog::Create( parent, id, caption, pos, size, style );

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
    item1->SetAutoLayout(TRUE);

    wxNotebook* item3 = new wxNotebook(item1, ID_NOTEBOOK, wxDefaultPosition, wxSize(200, 200), wxNB_TOP);
    wxNotebookSizer* item3Sizer = new wxNotebookSizer(item3);
    ctGeneralSettingsDialog* item4 = new ctGeneralSettingsDialog(item3, ID_GENERAL_SETTINGS_DIALOG, wxDefaultPosition, wxSize(100, 80), 0);
    item3->AddPage(item4, _("General"));
    ctLocationSettingsDialog* item11 = new ctLocationSettingsDialog(item3, ID_LOCATION_SETTINGS_DIALOG, wxDefaultPosition, wxSize(100, 80), 0);
    item3->AddPage(item11, _("Locations"));
    item2->Add(item3Sizer, 0, wxGROW|wxALL, 5);

    wxBoxSizer* item21 = new wxBoxSizer(wxHORIZONTAL);
    item2->Add(item21, 0, wxGROW|wxALL, 5);

    item21->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item23 = new wxButton(item1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0);
    item23->SetDefault();
    item21->Add(item23, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item24 = new wxButton(item1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0);
    item21->Add(item24, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item25 = new wxButton(item1, wxID_HELP, _("&Help"), wxDefaultPosition, wxDefaultSize, 0);
    item21->Add(item25, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

#if defined(__WXGTK__) || defined(__WXMAC__)
    wxContextHelpButton* item26 = new wxContextHelpButton(item1, wxID_CONTEXT_HELP, wxDefaultPosition, wxSize(20, -1), wxBU_AUTODRAW);
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

void ctSettingsDialog::OnHelp( wxCommandEvent& event )
{
    wxNotebook* notebook = (wxNotebook*) FindWindow(ID_NOTEBOOK);

    int sel = notebook->GetSelection();

    wxASSERT_MSG( (sel != -1), wxT("A notebook tab should always be selected."));

    wxWindow* page = (wxWindow*) notebook->GetPage(sel);

    wxString helpTopic;
    if (page->GetId() == ID_GENERAL_SETTINGS_DIALOG)
    {
        helpTopic = wxT("General settings dialog");
    }
    else if (page->GetId() == ID_LOCATION_SETTINGS_DIALOG)
    {
        helpTopic = wxT("Location settings dialog");
    }

    if (!helpTopic.IsEmpty())
    {
        wxGetApp().GetHelpController().DisplaySection(helpTopic);
    }
}

/*!
 * Should we show tooltips?
 */

bool ctSettingsDialog::ShowToolTips()
{
  return TRUE;
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
    item4->SetAutoLayout(TRUE);

    wxStaticBox* item6Static = new wxStaticBox(item4, -1, _("General settings"));
    wxStaticBoxSizer* item6 = new wxStaticBoxSizer(item6Static, wxVERTICAL);
    item5->Add(item6, 1, wxGROW|wxALL, 5);

    wxCheckBox* item7 = new wxCheckBox(item4, ID_LOAD_LAST_DOCUMENT, _("&Load last document"), wxDefaultPosition, wxDefaultSize, 0);
    item7->SetValue(FALSE);
    item7->SetHelpText(_("Check to load the last document on startup"));
    if (ShowToolTips())
        item7->SetToolTip(_("Check to load the last document on startup"));
    item6->Add(item7, 0, wxALIGN_LEFT|wxALL, 5);

    wxCheckBox* item8 = new wxCheckBox(item4, ID_SHOW_TOOLTIPS, _("&Show tooltips"), wxDefaultPosition, wxDefaultSize, 0);
    item8->SetValue(FALSE);
    item8->SetHelpText(_("Check to show tooltips"));
    if (ShowToolTips())
        item8->SetToolTip(_("Check to show tooltips"));
    item6->Add(item8, 0, wxALIGN_LEFT|wxALL, 5);

    wxStaticText* item9 = new wxStaticText(item4, wxID_STATIC, _("&Default file kind to save when using the Go command:"), wxDefaultPosition, wxDefaultSize, 0);
    item6->Add(item9, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString item10Strings[] = {
        _("Setup file"),
        _("Configure script")
    };
    wxChoice* item10 = new wxChoice(item4, ID_DEFAULT_FILE_KIND, wxDefaultPosition, wxSize(200, -1), 2, item10Strings, 0);
    item10->SetStringSelection(_("Setup file"));
    item10->SetHelpText(_("Select the default kind of file to save using Go"));
    if (ShowToolTips())
        item10->SetToolTip(_("Select the default kind of file to save using Go"));
    item6->Add(item10, 0, wxGROW|wxALL, 5);

    GetSizer()->Fit(this);
////@end ctGeneralSettingsDialog content construction

    FindWindow(ID_LOAD_LAST_DOCUMENT)->SetValidator(wxGenericValidator(& wxGetApp().GetSettings().m_loadLastDocument));
    FindWindow(ID_SHOW_TOOLTIPS)->SetValidator(wxGenericValidator(& wxGetApp().GetSettings().m_useToolTips));
    FindWindow(ID_DEFAULT_FILE_KIND)->SetValidator(wxGenericValidator(& wxGetApp().GetSettings().m_defaultFileKind));
}

/*!
 * Should we show tooltips?
 */

bool ctGeneralSettingsDialog::ShowToolTips()
{
  return TRUE;
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
    item11->SetAutoLayout(TRUE);

    wxStaticBox* item13Static = new wxStaticBox(item11, -1, _("Locations"));
    wxStaticBoxSizer* item13 = new wxStaticBoxSizer(item13Static, wxVERTICAL);
    item12->Add(item13, 1, wxGROW|wxALL, 5);

    wxStaticText* item14 = new wxStaticText(item11, wxID_STATIC, _("&wxWindows hierarchy:"), wxDefaultPosition, wxDefaultSize, 0);
    item13->Add(item14, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* item15 = new wxBoxSizer(wxHORIZONTAL);
    item13->Add(item15, 0, wxGROW, 5);

    wxTextCtrl* item16 = new wxTextCtrl(item11, ID_WXWIN_HIERARCHY, _(""), wxDefaultPosition, wxSize(200, -1), 0);
    item16->SetHelpText(_("Enter the root path of the wxWindows hierarchy"));
    if (ShowToolTips())
        item16->SetToolTip(_("Enter the root path of the wxWindows hierarchy"));
    item15->Add(item16, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item17 = new wxButton(item11, ID_CHOOSE_WXWIN_HIERARCHY, _("&Choose..."), wxDefaultPosition, wxDefaultSize, 0);
    item17->SetHelpText(_("Click to choose the root path of the wxWindows hierarchy\\n"));
    if (ShowToolTips())
        item17->SetToolTip(_("Click to choose the root path of the wxWindows hierarchy\\n"));
    item15->Add(item17, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* item18 = new wxBoxSizer(wxHORIZONTAL);
    item13->Add(item18, 0, wxGROW, 5);

    item18->Add(60, 5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxCheckBox* item20 = new wxCheckBox(item11, ID_USE_WXWIN, _("&Use WXWIN environment variable"), wxDefaultPosition, wxDefaultSize, 0);
    item20->SetValue(FALSE);
    item20->SetHelpText(_("Check to use the value of WXWIN instead"));
    if (ShowToolTips())
        item20->SetToolTip(_("Check to use the value of WXWIN instead"));
    item18->Add(item20, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    GetSizer()->Fit(this);
////@end ctLocationSettingsDialog content construction

    FindWindow(ID_WXWIN_HIERARCHY)->SetValidator(wxGenericValidator(& wxGetApp().GetSettings().m_frameworkDir));
    FindWindow(ID_USE_WXWIN)->SetValidator(wxGenericValidator(& wxGetApp().GetSettings().m_useEnvironmentVariable));
}

/*!
 * Update event handler for ID_WXWIN_HIERARCHY
 */

void ctLocationSettingsDialog::OnUpdateWxwinHierarchy( wxUpdateUIEvent& event )
{
    wxCheckBox* checkbox = (wxCheckBox*) FindWindow(ID_USE_WXWIN);
    event.Enable(!checkbox->GetValue());
}

/*!
 * Event handler for ID_CHOOSE_WXWIN_HIERARCHY
 */

void ctLocationSettingsDialog::OnChooseWxwinHierarchy( wxCommandEvent& event )
{
    wxTextCtrl* textCtrl = (wxTextCtrl*) FindWindow( ID_WXWIN_HIERARCHY );
    wxASSERT( textCtrl != NULL );
    wxString defaultPath = textCtrl->GetValue();

    wxDirDialog dialog(this, _("Choose the location of the wxWindows hierarchy"),
            defaultPath);
    if (dialog.ShowModal() == wxID_OK)
    {
        textCtrl->SetValue(dialog.GetPath());
    }
}

/*!
 * Update event handler for ID_CHOOSE_WXWIN_HIERARCHY
 */

void ctLocationSettingsDialog::OnUpdateChooseWxwinHierarchy( wxUpdateUIEvent& event )
{
    wxCheckBox* checkbox = (wxCheckBox*) FindWindow(ID_USE_WXWIN);
    event.Enable(!checkbox->GetValue());
}

/*!
 * Should we show tooltips?
 */

bool ctLocationSettingsDialog::ShowToolTips()
{
    return wxGetApp().GetSettings().m_useToolTips;
}

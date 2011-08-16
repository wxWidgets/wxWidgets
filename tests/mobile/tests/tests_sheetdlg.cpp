/*
 *  tests_sheetdlg.cpp
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-12.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "tests_sheetdlg.h"

BEGIN_EVENT_TABLE(MobileTestsWxSheetDialogPanel, wxPanel)
END_EVENT_TABLE()


bool MobileTestsWxSheetDialogPanel::CreateWithControls(wxWindow* parent,
                                                  wxWindowID id,
                                                  const wxPoint& pos,
                                                  const wxSize& size,
                                                  long style,
                                                  const wxString& name)
{
    if ( !wxPanel::Create(parent, id, pos, size, style, name)) {
        wxFAIL_MSG("Unable to create parent wxPanel");
        return false;
    }
    
    if ( !CreateControls()) {
        wxFAIL_MSG("Unable to create controls for this test panel");
        return false;
    }
    
    return true;
}

bool MobileTestsWxSheetDialogPanel::CreateControls()
{
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(buttonSizer);
    
    //
    // "Show sheet" buttons
    // 
    
    m_showActionSheetButton = new wxButton(this, wxID_ANY, "Show wxActionSheetDialog", wxDefaultPosition, wxDefaultSize, wxBU_ROUNDED_RECTANGLE);
    m_showActionSheetButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MobileTestsWxSheetDialogPanel::OnShowActionSheet), NULL, this);
    buttonSizer->Add(m_showActionSheetButton,0,wxEXPAND|wxALL,5);

    m_showAlertSheetButton = new wxButton(this, wxID_ANY, "Show wxAlertSheetDialog", wxDefaultPosition, wxDefaultSize, wxBU_ROUNDED_RECTANGLE);
    m_showAlertSheetButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MobileTestsWxSheetDialogPanel::OnShowAlertSheet), NULL, this);
    buttonSizer->Add(m_showAlertSheetButton,0,wxEXPAND|wxALL,5);
    
    return true;
}

// Show action sheet
void MobileTestsWxSheetDialogPanel::OnShowActionSheet(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Showing action sheet");
    
    wxArrayString otherButtons;
    otherButtons.Add(_("First other button"));
    otherButtons.Add(_("Another other button"));    
    
    wxActionSheetDialog* sheet = new wxActionSheetDialog(this,
                                                         _("Testing an action sheet, with a relatively long title on it."),
                                                         _("OK"),
                                                         otherButtons);
    
    sheet->Connect(wxEVT_COMMAND_SHEET_DIALOG_BUTTON_CLICKED, wxSheetDialogEventHandler(MobileTestsWxSheetDialogPanel::OnActionSheetButtonClicked), NULL, this);

    // Redirect events to this window
    sheet->SetOwner(this);
    sheet->ShowSheetDialog(NULL);    
}

// Show alert sheet
void MobileTestsWxSheetDialogPanel::OnShowAlertSheet(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Showing action sheet");
    
    wxArrayString otherButtons;
    otherButtons.Add(_("First other button"));
    otherButtons.Add(_("Another other button"));
    
    wxAlertSheetDialog* sheet = new wxAlertSheetDialog(this,
                                                       _("Testing an alert sheet"),
                                                       _("This is the message for the alert sheet, and it can be quite long if necessary."),
                                                       _("Cancel"),
                                                       otherButtons);
        
    sheet->Connect(wxEVT_COMMAND_SHEET_DIALOG_BUTTON_CLICKED, wxSheetDialogEventHandler(MobileTestsWxSheetDialogPanel::OnAlertSheetButtonClicked), NULL, this);
    
    // Redirect events to this window
    sheet->SetOwner(this);
    sheet->ShowSheetDialog(NULL);
}

// Action sheet button clicked
void MobileTestsWxSheetDialogPanel::OnActionSheetButtonClicked(wxSheetDialogEvent& WXUNUSED(event))
{
    wxLogMessage("Action sheet button clicked");
    
}

// Alert sheet button clicked
void MobileTestsWxSheetDialogPanel::OnAlertSheetButtonClicked(wxSheetDialogEvent& WXUNUSED(event))
{
    wxLogMessage("Alert sheet button clicked");
    
}

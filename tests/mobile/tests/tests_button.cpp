//
//  tests_button.cpp
//  testios
//
//  Created by Linas Valiukas on 2011-07-27.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "tests_button.h"

#define MobileTestsWxButtonClickedID    wxID_PROPERTIES

BEGIN_EVENT_TABLE(MobileTestsWxButtonPanel, wxPanel)
    EVT_BUTTON(MobileTestsWxButtonClickedID, MobileTestsWxButtonPanel::OnButtonClicked)
END_EVENT_TABLE()


bool MobileTestsWxButtonPanel::CreateWithControls(wxWindow* parent,
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

bool MobileTestsWxButtonPanel::CreateControls()
{
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(buttonSizer);
    //
    // Various styles
    // 
    
    m_buttonRoundedRectangle = new wxButton(this, MobileTestsWxButtonClickedID, "Click me!", wxDefaultPosition, wxDefaultSize, wxBU_ROUNDED_RECTANGLE);
    buttonSizer->Add(m_buttonRoundedRectangle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    m_buttonDisclosure = new wxButton(this, wxID_ANY, "This text should not be shown.", wxDefaultPosition, wxDefaultSize, wxBU_DISCLOSURE);
    buttonSizer->Add(m_buttonDisclosure, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    m_buttonInfoLight = new wxButton(this, wxID_ANY, "This text should not be shown.", wxDefaultPosition, wxDefaultSize, wxBU_INFO_LIGHT);
    buttonSizer->Add(m_buttonInfoLight, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    m_buttonInfoDark = new wxButton(this, wxID_ANY, "This text should not be shown.", wxDefaultPosition, wxDefaultSize, wxBU_INFO_DARK);
    buttonSizer->Add(m_buttonInfoDark, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);    
    
    m_buttonContactAdd = new wxButton(this, wxID_ANY, "This text should not be shown.", wxDefaultPosition, wxDefaultSize, wxBU_CONTACT_ADD);
    buttonSizer->Add(m_buttonContactAdd, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    return true;
}

/// Show an action sheet
void MobileTestsWxButtonPanel::OnButtonClicked(wxCommandEvent& WXUNUSED(event))
{
    wxLogInfo("m_buttonRoundedRectangle button clicked");
}

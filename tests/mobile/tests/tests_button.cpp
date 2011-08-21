//
//  tests_button.cpp
//  testios
//
//  Created by Linas Valiukas on 2011-07-27.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "tests_button.h"
#include "tests_bitmap.h"


BEGIN_EVENT_TABLE(MobileTestsWxButtonPanel, wxPanel)
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
    
    m_buttonRoundedRectangle = new wxButton(this, wxID_ANY, "Click me!", wxDefaultPosition, wxDefaultSize, wxBU_ROUNDED_RECTANGLE);
    m_buttonRoundedRectangle->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MobileTestsWxButtonPanel::OnButtonRoundedRectangleClicked), NULL, this);
    buttonSizer->Add(m_buttonRoundedRectangle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    m_buttonDisclosure = new wxButton(this, wxID_ANY, "This text should not be shown.", wxDefaultPosition, wxDefaultSize, wxBU_DISCLOSURE);
    m_buttonDisclosure->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MobileTestsWxButtonPanel::OnButtonDisclosureClicked), NULL, this);
    buttonSizer->Add(m_buttonDisclosure, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    m_buttonInfoLight = new wxButton(this, wxID_ANY, "This text should not be shown.", wxDefaultPosition, wxDefaultSize, wxBU_INFO_LIGHT);
    m_buttonInfoLight->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MobileTestsWxButtonPanel::OnButtonInfoLightClicked), NULL, this);
    buttonSizer->Add(m_buttonInfoLight, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    m_buttonInfoDark = new wxButton(this, wxID_ANY, "This text should not be shown.", wxDefaultPosition, wxDefaultSize, wxBU_INFO_DARK);
    m_buttonInfoDark->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MobileTestsWxButtonPanel::OnButtonInfoDarkClicked), NULL, this);
    buttonSizer->Add(m_buttonInfoDark, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);    
    
    m_buttonContactAdd = new wxButton(this, wxID_ANY, "This text should not be shown.", wxDefaultPosition, wxDefaultSize, wxBU_CONTACT_ADD);
    m_buttonContactAdd->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MobileTestsWxButtonPanel::OnButtonContactAddClicked), NULL, this);
    buttonSizer->Add(m_buttonContactAdd, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    m_buttonBitmap = new wxBitmapButton(this, wxID_ANY, tests_bitmap, wxDefaultPosition, wxDefaultSize);
    m_buttonBitmap->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MobileTestsWxButtonPanel::OnButtonBitmapClicked), NULL, this);
    buttonSizer->Add(m_buttonBitmap, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    return true;
}

void MobileTestsWxButtonPanel::OnButtonRoundedRectangleClicked(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Rectangle button clicked");
}

void MobileTestsWxButtonPanel::OnButtonDisclosureClicked(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Disclosure button clicked");    
}

void MobileTestsWxButtonPanel::OnButtonInfoLightClicked(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Info light button clicked");
}

void MobileTestsWxButtonPanel::OnButtonInfoDarkClicked(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Info dark button clicked");
}

void MobileTestsWxButtonPanel::OnButtonContactAddClicked(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Contact add button clicked");
}

void MobileTestsWxButtonPanel::OnButtonBitmapClicked(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Bitmap button clicked");
}

/*
 *  tests_stattext.cpp
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-10.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "tests_stattext.h"

BEGIN_EVENT_TABLE(MobileTestsWxStaticTextPanel, wxPanel)
    //EVT_BUTTON(MobileTestsWxButtonClickedID, MobileTestsWxStaticTextPanel::OnButtonClicked)
END_EVENT_TABLE()


bool MobileTestsWxStaticTextPanel::CreateWithControls(wxWindow* parent,
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

bool MobileTestsWxStaticTextPanel::CreateControls()
{
    wxBoxSizer* textSizer = new wxBoxSizer(wxHORIZONTAL);
    
    //
    // Various labels
    // 
    
    m_textLeft = new wxStaticText(this, wxID_ANY, "Left", wxPoint(0, 0), wxSize(300, 40), wxALIGN_LEFT);
    m_textLeft->SetBackgroundColour(*wxRED);
    //textSizer->Add(m_textLeft, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_textCentre = new wxStaticText(this, wxID_ANY, "Centre", wxPoint(0, 45), wxSize(300, 40), wxALIGN_CENTRE);
    m_textCentre->SetBackgroundColour(*wxGREEN);
    //textSizer->Add(m_textCentre, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_textRight = new wxStaticText(this, wxID_ANY, "Right", wxPoint(0, 90), wxSize(300, 40), wxALIGN_RIGHT);
    m_textRight->SetBackgroundColour(*wxBLUE);
    //textSizer->Add(m_textRight, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    return true;
}

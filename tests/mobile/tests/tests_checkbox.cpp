/*
 *  tests_checkbox.cpp
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-10.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "tests_checkbox.h"

BEGIN_EVENT_TABLE(MobileTestsWxCheckBoxPanel, wxPanel)
END_EVENT_TABLE()


bool MobileTestsWxCheckBoxPanel::CreateWithControls(wxWindow* parent,
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

bool MobileTestsWxCheckBoxPanel::CreateControls()
{
    wxBoxSizer* checkBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    
    //
    // Check box
    // 

    m_checkBox = new wxCheckBox(this, wxID_ANY, "This should not be shown.");
    checkBoxSizer->Add(m_checkBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    // Setters/Getters
    m_checkBox->SetValue(true);
    wxASSERT_MSG(m_checkBox->GetValue() == true, "Check box value should be true.");
    
    return true;
}

/*
 *  tests_segctrl.cpp
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-12.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "tests_segctrl.h"

BEGIN_EVENT_TABLE(MobileTestsWxSegmentedCtrlPanel, wxPanel)
END_EVENT_TABLE()


bool MobileTestsWxSegmentedCtrlPanel::CreateWithControls(wxWindow* parent,
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

bool MobileTestsWxSegmentedCtrlPanel::CreateControls()
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);
    //
    // Segmented control
    // 
    
    m_segCtrl = new wxSegmentedCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    m_segCtrl->AddItem(_("Tiger"));
    m_segCtrl->AddItem(_("Cheetah"));
    m_segCtrl->AddItem(_("Pig"));
    m_segCtrl->SetSelection(1);
    sizer->Add(m_segCtrl, 0, wxEXPAND|wxALL, 5);
    
    // Events
    m_segCtrl->Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MobileTestsWxSegmentedCtrlPanel::OnChanged), NULL, this);
    
    // Test getters
    wxASSERT_MSG(m_segCtrl->GetItemCount() == 3, "Incorrect item count");
    wxASSERT_MSG(m_segCtrl->GetSelection() == 1, "Incorrect selected index");
    
    // Set tint colour
    m_segCtrl->SetButtonBackgroundColour(*wxRED);
    wxASSERT_MSG(m_segCtrl->GetButtonBackgroundColour() == *wxRED, "Incorrect button background colour");
    
    return true;
}

void MobileTestsWxSegmentedCtrlPanel::OnChanged(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Event wxEVT_COMMAND_MENU_SELECTED");
}

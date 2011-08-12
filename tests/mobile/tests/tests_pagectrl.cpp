/*
 *  tests_pagectrl.cpp
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-12.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "tests_pagectrl.h"

BEGIN_EVENT_TABLE(MobileTestsWxPageCtrlPanel, wxPanel)
END_EVENT_TABLE()


bool MobileTestsWxPageCtrlPanel::CreateWithControls(wxWindow* parent,
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

bool MobileTestsWxPageCtrlPanel::CreateControls()
{
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    
    //
    // Page control
    // 
    
    m_pageCtrl = new wxPageCtrl(this, wxID_ANY, wxPoint(0, 30), wxSize(90, 45));
    m_pageCtrl->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MobileTestsWxPageCtrlPanel::OnPageChanged), NULL, this);
    m_pageCtrl->SetPageCount(6);
    m_pageCtrl->SetCurrentPage(2);
    sizer->Add(m_pageCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    return true;
}

void MobileTestsWxPageCtrlPanel::OnPageChanged(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Event wxEVT_COMMAND_BUTTON_CLICKED");
}

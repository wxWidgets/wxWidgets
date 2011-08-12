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
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    
    //
    // Segmented control
    // 
    
    m_segCtrl = new wxSegmentedCtrl(this, wxID_ANY, wxPoint(50, 50), wxSize(100, 40));
    m_segCtrl->AddItem(_("Tiger"));
    m_segCtrl->AddItem(_("Cheetah"));
    m_segCtrl->AddItem(_("Pig"));
    m_segCtrl->SetSelection(1);
    sizer->Add(m_segCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    return true;
}

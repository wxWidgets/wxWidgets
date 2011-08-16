/*
 *  tests_viewcontroller.cpp
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-15.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "tests_viewcontroller.h"

BEGIN_EVENT_TABLE(MobileTestsWxViewControllerPanel, wxPanel)
END_EVENT_TABLE()


bool MobileTestsWxViewControllerPanel::CreateWithControls(wxWindow* parent,
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

bool MobileTestsWxViewControllerPanel::CreateControls()
{
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    
    //
    // View controller
    // 
    
    m_viewController = new MobileTestsWxViewController("First title");
    wxASSERT_MSG(m_viewController->GetTitle() == "First title", "Incorrect title #1");
    
    m_viewController->SetTitle("Second title");
    wxASSERT_MSG(m_viewController->GetTitle() == "Second title", "Incorrect title #2");
    
    return true;
}

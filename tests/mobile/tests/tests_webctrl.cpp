/*
 *  tests_webctrl.cpp
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-10.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "tests_webctrl.h"

BEGIN_EVENT_TABLE(MobileTestsWxWebCtrlPanel, wxPanel)
END_EVENT_TABLE()


bool MobileTestsWxWebCtrlPanel::CreateWithControls(wxWindow* parent,
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

bool MobileTestsWxWebCtrlPanel::CreateControls()
{
    wxBoxSizer* textCtrlSizer = new wxBoxSizer(wxHORIZONTAL);
    
    //
    // Web control
    // 

    m_webCtrl = new wxWebKitCtrl(this, wxID_ANY);
    m_webCtrl->LoadURL("http://www.wxwidgets.org/");
    
    wxASSERT_MSG(!m_webCtrl->CanGoBack(), "Web control can't go back at the moment");
    wxASSERT_MSG(!m_webCtrl->CanGoForward(), "Web control can't go forward at the moment");
    
    return true;
}

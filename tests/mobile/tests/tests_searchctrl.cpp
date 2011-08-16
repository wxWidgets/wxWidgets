/*
 *  tests_searchctrl.cpp
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-14.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "tests_searchctrl.h"

BEGIN_EVENT_TABLE(MobileTestsWxSearchCtrlPanel, wxPanel)
END_EVENT_TABLE()


bool MobileTestsWxSearchCtrlPanel::CreateWithControls(wxWindow* parent,
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

bool MobileTestsWxSearchCtrlPanel::CreateControls()
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);
    
    //
    // Search control
    // 
    
    m_searchCtrl = new wxSearchCtrl( this, wxID_ANY, _("Search"), wxDefaultPosition, wxDefaultSize, 0 );
    sizer->Add(m_searchCtrl, 0, wxEXPAND|wxALL, 5);
    
    return true;
}

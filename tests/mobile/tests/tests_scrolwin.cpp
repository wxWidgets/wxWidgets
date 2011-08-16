/*
 *  tests_scrolwin.cpp
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-12.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "tests_scrolwin.h"

BEGIN_EVENT_TABLE(MobileTestsWxScrolledWindowPanel, wxPanel)
END_EVENT_TABLE()


bool MobileTestsWxScrolledWindowPanel::CreateWithControls(wxWindow* parent,
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

bool MobileTestsWxScrolledWindowPanel::CreateControls()
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);
    
    //
    // Scrolled window
    // 

    m_scrolWin = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, (wxHSCROLL|wxVSCROLL));
    sizer->Add(m_scrolWin,1,wxEXPAND|wxALL,5);
    m_scrolWin->SetBackgroundColour(*wxGREEN);
    m_scrolWin->SetVirtualSize(wxSize(600, 600));
    
    return true;
}

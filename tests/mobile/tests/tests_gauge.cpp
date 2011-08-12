/*
 *  tests_gauge.cpp
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-12.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "tests_gauge.h"

BEGIN_EVENT_TABLE(MobileTestsWxGaugePanel, wxPanel)
END_EVENT_TABLE()


bool MobileTestsWxGaugePanel::CreateWithControls(wxWindow* parent,
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

bool MobileTestsWxGaugePanel::CreateControls()
{
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    
    //
    // Gauge
    // 
    
    m_gauge = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(200, 15), 0 );
    m_gauge->SetValue(50);
    sizer->Add(m_gauge, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    return true;
}

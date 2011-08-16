/*
 *  tests_viewcontroller_vc.cpp
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-15.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "tests_viewcontroller_vc.h"

MobileTestsWxViewController::MobileTestsWxViewController(const wxString& title): wxViewController(title)
{
    // Create off-screen to avoid flicker
    wxPanel* panel = new wxPanel(NULL, wxID_ANY, wxDefaultPosition, wxSize(100, 100));
    panel->SetBackgroundColour(wxColour(wxT("#ff0000")));
    SetWindow(panel);
}

bool MobileTestsWxViewController::OnDelete()
{
    wxLogMessage("MobileTestsWxViewController is being deleted");
    return true;
}

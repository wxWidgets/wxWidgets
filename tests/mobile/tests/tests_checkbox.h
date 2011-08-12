/*
 *  tests_checkbox.h
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-10.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

//
// TESTS:
// 
// events:
//   wxEVT_CHECKBOX(id, func)
// - SetValue()
// - GetValue()

#ifndef testios_tests_checkbox_h
#define testios_tests_checkbox_h

#include "testpanel.h"
#include "wx/checkbox.h"

class MobileTestsWxCheckBoxPanel : public MobileTestsWxPanel {
    
public:
    bool CreateWithControls(wxWindow* parent,
                            wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxTAB_TRAVERSAL,
                            const wxString& name = _("wxCheckBox Mobile"));
    
    // Event tests
    void OnCheckBox(wxCommandEvent& WXUNUSED(event));
    
protected:
    bool CreateControls();
    
    DECLARE_EVENT_TABLE()
    
private:
    wxCheckBox* m_checkBox;
};

#endif

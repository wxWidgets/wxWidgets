//
//  tests_stattext.h
//  testios
//
//  Created by Linas Valiukas on 2011-07-27.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

//
// TESTS:
// 

#ifndef testios_tests_stattext_h
#define testios_tests_stattext_h

#include "testpanel.h"
#include "wx/stattext.h"

class MobileTestsWxStaticTextPanel : public MobileTestsWxPanel {
    
public:
    bool CreateWithControls(wxWindow* parent,
                            wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxTAB_TRAVERSAL,
                            const wxString& name = _("wxStaticText Mobile"));
    
protected:
    bool CreateControls();
    
    DECLARE_EVENT_TABLE()
    
private:
    wxStaticText* m_textLeft;
    wxStaticText* m_textCentre;
    wxStaticText* m_textRight;
};

#endif

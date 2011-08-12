/*
 *  tests_gauge.h
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-12.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

//
// TESTS:
// 
// styles:
//   - wxGA_HORIZONTAL
//   - wxGA_VERTICAL


#ifndef testios_tests_gauge_h
#define testios_tests_gauge_h

#include "testpanel.h"
#include "wx/gauge.h"

class MobileTestsWxGaugePanel : public MobileTestsWxPanel {
    
public:
    bool CreateWithControls(wxWindow* parent,
                            wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxTAB_TRAVERSAL,
                            const wxString& name = _("wxGauge Mobile"));
    
protected:
    bool CreateControls();
    
    DECLARE_EVENT_TABLE()
    
private:
    wxGauge* m_gauge;
};

#endif

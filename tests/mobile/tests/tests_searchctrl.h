/*
 *  tests_searchctrl.h
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-14.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

//
// TESTS:
// 


#ifndef testios_tests_searchctrl_h
#define testios_tests_searchctrl_h

#include "testpanel.h"
#include "wx/srchctrl.h"

class MobileTestsWxSearchCtrlPanel : public MobileTestsWxPanel {
    
public:
    bool CreateWithControls(wxWindow* parent,
                            wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxTAB_TRAVERSAL,
                            const wxString& name = _("wxSearchCtrl Mobile"));
    
protected:
    bool CreateControls();
    
    DECLARE_EVENT_TABLE()
    
private:
    wxSearchCtrl* m_searchCtrl;
};

#endif

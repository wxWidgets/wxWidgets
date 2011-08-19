/*
 *  tests_wheelsctrl.h
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-18.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

//
// TESTS:
// 

#ifndef testios_tests_wheelsctrl_h
#define testios_tests_wheelsctrl_h

#include "testpanel.h"
#include "wx/wheelsctrl.h"

class MobileTestsWxWheelsCtrlPanel : public MobileTestsWxPanel {
    
public:
    bool CreateWithControls(wxWindow* parent,
                            wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxTAB_TRAVERSAL,
                            const wxString& name = _("wxWheelsCtrl Mobile"));
    
    // Event tests
    void OnTextChange(wxWheelsCtrlEvent& WXUNUSED(event));
    void OnDateTimeChange(wxWheelsCtrlEvent& WXUNUSED(event));
    void OnCountDownChange(wxWheelsCtrlEvent& WXUNUSED(event));
        
protected:
    bool CreateControls();
    
    DECLARE_EVENT_TABLE()
    
private:
    wxWheelsCtrl* m_textWheelsCtrl;
    wxWheelsCtrl* m_dateTimeWheelsCtrl;
    wxWheelsCtrl* m_countDownWheelsCtrl;
};

#endif

/*
 *  tests_webctrl.h
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-10.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */



//
// TESTS:
// 
// - wxWebKitCtrl()
// + wxWebKitCtrl (wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxWebCtrlNameStr)
// + bool  LoadURL (const wxString &url)
// + bool  CanGoBack ()
// + bool  CanGoForward ()
// - bool  GoBack ()
// - bool  GoForward ()
// - void  SetUserScaling (bool scaling)
// - bool  GetUserScaling () const

#ifndef testios_tests_webctrl_h
#define testios_tests_webctrl_h

#include "testpanel.h"
#include "wx/html/webkit.h"

class MobileTestsWxWebCtrlPanel : public MobileTestsWxPanel {
    
public:
    bool CreateWithControls(wxWindow* parent,
                            wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxTAB_TRAVERSAL,
                            const wxString& name = _("wxWebCtrl Mobile"));
        
protected:
    bool CreateControls();
    
    DECLARE_EVENT_TABLE()
    
private:
    wxWebKitCtrl* m_webCtrl;
};

#endif

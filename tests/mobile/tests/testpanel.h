//
//  testpanel.h
//  testios
//
//  Created by Linas Valiukas on 2011-07-27.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef testios_testpanel_h
#define testios_testpanel_h

#include "wx/panel.h"
#include "wx/navctrl.h"


#pragma mark Abstract test wxPanel

class MobileTestsWxPanel : public wxPanel {
    
public:
    
    MobileTestsWxPanel() : wxPanel() { m_navCtrl = NULL; }
    MobileTestsWxPanel(wxNavigationCtrl* navCtrl) : wxPanel() { m_navCtrl = navCtrl; }
    
    virtual bool CreateWithControls(wxWindow* parent,
                                    wxWindowID id = wxID_ANY,
                                    const wxPoint& pos = wxDefaultPosition,
                                    const wxSize& size = wxDefaultSize,
                                    long style = wxTAB_TRAVERSAL,
                                    const wxString& name = _("Test")) = 0;
    
protected:
    wxNavigationCtrl* m_navCtrl;
};

#endif

/*
 *  tests_scrolwin.h
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-12.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

//
// TESTS:
// 
// - void EnableScrolling(const bool xScrolling, const bool yScrolling)
// - FIXME some sort of scrolling event?


#ifndef testios_tests_scrolwin_h
#define testios_tests_scrolwin_h

#include "testpanel.h"
#include "wx/scrolwin.h"

class MobileTestsWxScrolledWindowPanel : public MobileTestsWxPanel {
    
public:
    bool CreateWithControls(wxWindow* parent,
                            wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxTAB_TRAVERSAL,
                            const wxString& name = _("wxScrolledWindow Mobile"));
    
protected:
    bool CreateControls();
    
    DECLARE_EVENT_TABLE()
    
private:
    wxScrolledWindow* m_scrolWin;
};

#endif

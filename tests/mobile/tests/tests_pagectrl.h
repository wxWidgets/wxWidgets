/*
 *  tests_pagectrl.h
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-12.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

//
// TESTS:
// 
// events:
//   - wxEVT_COMMAND_BUTTON_CLICKED
// - wxPageCtrl (wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxT("pagectrl"))
// - bool   Create (wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxT("pagectrl"))
// - int    GetPageCount () const 
// - void   SetPageCount (int pageCount)
// - int    GetCurrentPage () const 
// - void   SetCurrentPage (int page)
// - bool   GetHideForSinglePage () const 
// - wxSize GetSizeForPageCount (int pageCount) const 

#ifndef testios_tests_pagectrl_h
#define testios_tests_pagectrl_h

#include "testpanel.h"
#include "wx/pagectrl.h"

class MobileTestsWxPageCtrlPanel : public MobileTestsWxPanel {
    
public:
    bool CreateWithControls(wxWindow* parent,
                            wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxTAB_TRAVERSAL,
                            const wxString& name = _("wxPageCtrl Mobile"));
    
    // Event tests
    void OnPageChanged(wxCommandEvent& WXUNUSED(event));
    
protected:
    bool CreateControls();
    
    DECLARE_EVENT_TABLE()
    
private:
    wxPageCtrl* m_pageCtrl;
};

#endif

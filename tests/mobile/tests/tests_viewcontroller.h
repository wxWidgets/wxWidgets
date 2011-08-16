/*
 *  tests_viewcontroller.h
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-15.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

//
// TESTS:
// 
// + wxViewController (const wxString &title=wxEmptyString, bool autoDelete=true)
// + virtual bool     OnDelete ()
// - void     SetNavigationItem (wxMoNavigationItem *item)
// - wxNavigationItem *     GetNavigationItem () const
// - void     SetWindow (wxWindow *window)
// - wxWindow *     GetWindow () const
// - void     SetAutoDelete (bool autoDelete)
// - bool     GetAutoDelete () const
// + void     SetTitle (const wxString &title)
// + wxString     GetTitle () const
// - void     SetOwnsWindow (bool owns)
// - bool     GetOwnsWindow () const


#ifndef testios_tests_viewcontroller_h
#define testios_tests_viewcontroller_h

#include "testpanel.h"
#include "tests_viewcontroller_vc.h"

class MobileTestsWxViewControllerPanel : public MobileTestsWxPanel {
    
public:
    bool CreateWithControls(wxWindow* parent,
                            wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxTAB_TRAVERSAL,
                            const wxString& name = _("wxViewController Mobile"));
    
protected:
    bool CreateControls();
    
    DECLARE_EVENT_TABLE()
    
private:
    MobileTestsWxViewController* m_viewController;
};

#endif

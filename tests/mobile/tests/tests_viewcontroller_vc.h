/*
 *  tests_viewcontroller_vc.h
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-15.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef testios_tests_viewcontroller_vc_h
#define testios_tests_viewcontroller_vc_h

#include "wx/viewcontroller.h"

class MobileTestsWxViewController: public wxViewController {
    
public:
    MobileTestsWxViewController(const wxString& title);
    bool OnDelete();
};

#endif

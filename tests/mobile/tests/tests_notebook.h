//
//  tests_notebook.h
//  testios
//
//  Created by Linas Valiukas on 2011-07-27.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef testios_tests_notebook_h
#define testios_tests_notebook_h

#include "testpanel.h"
#include "wx/notebook.h"

class MobileTestsWxNotebookPanel : public MobileTestsWxPanel {
    
public:
    bool CreateWithControls(wxWindow* parent,
                            wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxTAB_TRAVERSAL,
                            const wxString& name = _("wxButton Mobile"));
    
protected:
    bool CreateControls();
    
    void OnButtonClicked(wxCommandEvent& WXUNUSED(event));
    
    DECLARE_EVENT_TABLE()
    
private:
    
    wxNotebook *m_notebook;
};

#endif

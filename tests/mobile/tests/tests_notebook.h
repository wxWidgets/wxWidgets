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
    
    DECLARE_EVENT_TABLE()
    
private:
    
    wxNotebook *m_notebook;
};


// Sample notebook page
class MobileTestsWxNotebookPage : public wxPanel {
    
public:
    MobileTestsWxNotebookPage(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "panel") : wxPanel(parent, id, pos, size, style, name) { }
  
protected:
    
    void OnInsertPageButtonClicked(wxCommandEvent& WXUNUSED(event));
    void OnDeletePageButtonClicked(wxCommandEvent& WXUNUSED(event));
    void OnDeleteAllPagesButtonClicked(wxCommandEvent& WXUNUSED(event));

    DECLARE_EVENT_TABLE()
    
};

#endif

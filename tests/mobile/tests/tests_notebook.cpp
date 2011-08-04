//
//  tests_notebook.cpp
//  testios
//
//  Created by Linas Valiukas on 2011-07-27.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "tests_notebook.h"

#define MobileTestsWxButtonClickedID    wxID_PROPERTIES

BEGIN_EVENT_TABLE(MobileTestsWxNotebookPanel, wxPanel)
    //EVT_BUTTON(MobileTestsWxButtonClickedID, MobileTestsWxNotebookPanel::OnButtonClicked)
END_EVENT_TABLE()


bool MobileTestsWxNotebookPanel::CreateWithControls(wxWindow* parent,
                                                  wxWindowID id,
                                                  const wxPoint& pos,
                                                  const wxSize& size,
                                                  long style,
                                                  const wxString& name)
{
    if ( !wxPanel::Create(parent, id, pos, size, style, name)) {
        wxFAIL_MSG("Unable to create parent wxPanel");
        return false;
    }
    
    if ( !CreateControls()) {
        wxFAIL_MSG("Unable to create controls for this test panel");
        return false;
    }
    
    return true;
}

bool MobileTestsWxNotebookPanel::CreateControls()
{
    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(topSizer);
    
    m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), 0);
    topSizer->Add(m_notebook, 1, wxEXPAND);
    
    wxPanel* page1 = new wxPanel(m_notebook, wxID_ANY, wxPoint(0, 0), wxSize(320, 411), 0);
    m_notebook->AddPage(page1, _("First"), true, wxID_TOPRATED);
    
    wxPanel* page2 = new wxPanel(m_notebook, wxID_ANY, wxPoint(0, 0), wxSize(320, 411), wxNO_BORDER);
    m_notebook->AddPage(page2, _("Second"), false, wxID_SEARCH);

    wxPanel* page3 = new wxPanel(m_notebook, wxID_ANY, wxPoint(0, 0), wxSize(320, 411), wxNO_BORDER);
    m_notebook->AddPage(page3, _("Third"), false, wxID_CONTACTS);
    m_notebook->SetBadge(2, wxT("1"));
    
    return true;
}

/// Show an action sheet
void MobileTestsWxNotebookPanel::OnButtonClicked(wxCommandEvent& WXUNUSED(event))
{
    wxLogInfo("Button clicked");
}

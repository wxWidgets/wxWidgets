//
//  tests_notebook.cpp
//  testios
//
//  Created by Linas Valiukas on 2011-07-27.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "tests_notebook.h"


#define MobileTestsWxNotebookInsertPageID       1234
#define MobileTestsWxNotebookDeletePageID       1235
#define MobileTestsWxNotebookDeleteAllPagesID   1236


#pragma mark -
#pragma mark wxNotebook

BEGIN_EVENT_TABLE(MobileTestsWxNotebookPanel, wxPanel)
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
    
    // Notebook
    m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    topSizer->Add(m_notebook, 1, wxEXPAND);
    
    // Three panels
    MobileTestsWxNotebookPage* page1 = new MobileTestsWxNotebookPage(m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    m_notebook->AddPage(page1, _("First"), true, wxID_TOPRATED);
    
    MobileTestsWxNotebookPage* page2 = new MobileTestsWxNotebookPage(m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
    m_notebook->AddPage(page2, _("Second"), false, wxID_SEARCH);

    MobileTestsWxNotebookPage* page3 = new MobileTestsWxNotebookPage(m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
    m_notebook->AddPage(page3, _("Third"), false, wxID_CONTACTS);
    m_notebook->SetBadge(2, wxT("1"));
        
    // Getters
    wxASSERT_MSG(m_notebook->GetPageCount() == 3, "Invalid page count");
    wxASSERT_MSG(m_notebook->GetSelection() == 0, "Invalid selection");
    wxASSERT_MSG(m_notebook->GetBadge(3) == "1", "Invalid badge");
    
    // Returns 1 because multi-row tab controls are not supported.
    wxASSERT_MSG(m_notebook->GetRowCount() == 1, "Notebook row count must be 1");
    
    // FIXME test event too
    m_notebook->SetSelection(1);
    wxASSERT_MSG(m_notebook->GetSelection() == 1, "Invalid selection");
    
    m_notebook->SetSelection(2);
    wxASSERT_MSG(m_notebook->GetSelection() == 2, "Invalid selection");
    
    return true;
}


#pragma mark -
#pragma mark wxNotebook page

BEGIN_EVENT_TABLE(MobileTestsWxNotebookPage, wxPanel)
    EVT_BUTTON(MobileTestsWxNotebookInsertPageID, MobileTestsWxNotebookPage::OnInsertPageButtonClicked)
    EVT_BUTTON(MobileTestsWxNotebookDeletePageID, MobileTestsWxNotebookPage::OnDeletePageButtonClicked)
    EVT_BUTTON(MobileTestsWxNotebookDeleteAllPagesID, MobileTestsWxNotebookPage::OnDeleteAllPagesButtonClicked)
END_EVENT_TABLE()


MobileTestsWxNotebookPage::MobileTestsWxNotebookPage(wxWindow* parent,
                                                     wxWindowID id,
                                                     const wxPoint& pos,
                                                     const wxSize& size,
                                                     long style,
                                                     const wxString& name) : wxPanel(parent, id, pos, size, style, name)
{
    // FIXME nav. bar's height doesn't get counted in wxPanel
    wxButton* insertPageButton = new wxButton(this, MobileTestsWxNotebookInsertPageID, "Insert page", wxPoint(0, 45), wxDefaultSize, wxBU_ROUNDED_RECTANGLE);
    wxButton* deletePageButton = new wxButton(this, MobileTestsWxNotebookDeletePageID, "Delete page", wxPoint(0, 90), wxDefaultSize, wxBU_ROUNDED_RECTANGLE);
    wxButton* deleteAllPagesButton = new wxButton(this, MobileTestsWxNotebookDeleteAllPagesID, "Delete all pages", wxPoint(0, 135), wxDefaultSize, wxBU_ROUNDED_RECTANGLE);
}

void MobileTestsWxNotebookPage::OnInsertPageButtonClicked(wxCommandEvent& WXUNUSED(event))
{
    wxLogInfo("Insert page button clicked");
    
    wxNotebook* notebook = (wxNotebook *)GetParent();
    MobileTestsWxNotebookPage* newPage = new MobileTestsWxNotebookPage(notebook, wxID_ANY, wxPoint(0, 0), wxSize(320, 411), wxNO_BORDER);
    
    wxASSERT_MSG(notebook->InsertPage(0, newPage, "New page"), "Failed inserting a new page");
}

void MobileTestsWxNotebookPage::OnDeletePageButtonClicked(wxCommandEvent& WXUNUSED(event))
{
    wxLogInfo("Delete page button clicked");
    
    wxNotebook* notebook = (wxNotebook *)GetParent();
    wxASSERT_MSG(notebook->DeletePage(0), "Failed deleting a page");
}

void MobileTestsWxNotebookPage::OnDeleteAllPagesButtonClicked(wxCommandEvent& WXUNUSED(event))
{
    wxLogInfo("Delete all pages button clicked");
    
    wxNotebook* notebook = (wxNotebook *)GetParent();
    wxASSERT_MSG(notebook->DeleteAllPages(), "Failed deleting all pages");
}

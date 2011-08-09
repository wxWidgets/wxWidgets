//
//  tests_notebook.h
//  testios
//
//  Created by Linas Valiukas on 2011-07-27.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

//
// Tests:
// + wxNotebook ()
// + wxNotebook (wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=0, const wxString &name=wxNotebookNameStr)
// + bool       Create (wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=0, const wxString &name=wxNotebookNameStr)
// + virtual size_t     GetPageCount () const 
// + bool               AddPage(wxNotebookPage* page, const wxString& text, bool select = false, int imageId = -1)
// + int                SetSelection (size_t page)
// + int                GetSelection () const 
// + int                ChangeSelection (size_t page)
// - bool               SetPageText (size_t page, const wxString &strText)
// - wxString           GetPageText (size_t page) const 
// - void               SetImageList (wxImageList *imageList)
// - bool               SetPageImage (size_t page, int nImage)
// - int                GetPageImage (size_t page) const 
// + int                GetRowCount () const 
// - void               SetPadding (const wxSize &padding)
// + bool               DeleteAllPages ()
// + bool               InsertPage (size_t page, wxNotebookPage *pPage, const wxString &strText, bool bSelect=false, int imageId=-1)
// - virtual int        HitTest (const wxPoint &pt, long *flags=NULL) const 
// - virtual wxSize     CalcSizeFromPage (const wxSize &sizePage) const 
// + bool               SetBadge (int item, const wxString &badge)
// + wxString           GetBadge (int item) const 
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
                            const wxString& name = _("wxNotebook Mobile"));
        
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

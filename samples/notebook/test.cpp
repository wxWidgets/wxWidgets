/////////////////////////////////////////////////////////////////////////////
// Name:        test.cpp
// Purpose:     wxNotebook demo
// Author:      Julian Smart
// Modified by:
// Created:     26/10/98
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "test.h"

// If 1, use a dialog. Otherwise use a frame.
#define USE_TABBED_DIALOG 0
#if USE_TABBED_DIALOG
MyDialog* dialog = (MyDialog *) NULL;
#else // !USE_TABBED_DIALOG
MyFrame*  frame = (MyFrame *) NULL;
#endif // USE_TABBED_DIALOG

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
  // Create the main window
#if USE_TABBED_DIALOG
  dialog = new MyDialog((wxFrame *) NULL, -1, (char *) "Notebook", wxPoint(-1, -1), wxSize(365, 390), wxDIALOG_MODAL|wxDEFAULT_DIALOG_STYLE);

  dialog->ShowModal();

  // Quit immediately the dialog has been dismissed
  return FALSE;
#else
  frame = new MyFrame((wxFrame*) NULL, -1, "Notebook", wxPoint(-1, -1), wxSize(365, 390) );

  // Problem with generic wxNotebook implementation whereby it doesn't size properly unless
  // you set the size again
#if defined(__WIN16__)
  int width, height;
  frame->GetSize(& width, & height);
  frame->SetSize(-1, -1, width, height);
#endif

  return TRUE;
#endif
}

void MyApp::InitTabView(wxNotebook* notebook, wxPanel* window)
{
  m_okButton = new wxButton(window, wxID_OK, "Close", wxPoint(-1, -1), wxSize(80, 25));
  m_cancelButton = new wxButton(window, ID_DELETE_PAGE, "&Delete page", wxPoint(-1, -1), wxSize(80, 25));
  m_addPageButton = new wxButton(window, ID_ADD_PAGE, "&Add page", wxPoint(-1, -1), wxSize(80, 25));
  m_insertPageButton = new wxButton(window, ID_INSERT_PAGE, "&Insert page", wxPoint(-1, -1), wxSize(80, 25));
  m_nextPageButton = new wxButton(window, ID_NEXT_PAGE, "&Next page", wxPoint(-1, -1), wxSize(80, 25));
  m_okButton->SetDefault();

  wxLayoutConstraints *c;

  c = new wxLayoutConstraints;
  c->right.SameAs(window, wxRight, 4);
  c->bottom.SameAs(window, wxBottom, 4);
  c->height.AsIs();
  c->width.AsIs();
  m_addPageButton->SetConstraints(c);

  c = new wxLayoutConstraints;
  c->right.SameAs(m_addPageButton, wxLeft, 4);
  c->bottom.SameAs(window, wxBottom, 4);
  c->height.AsIs();
  c->width.AsIs();
  m_insertPageButton->SetConstraints(c);

  c = new wxLayoutConstraints;
  c->right.SameAs(m_insertPageButton, wxLeft, 4);
  c->bottom.SameAs(window, wxBottom, 4);
  c->height.AsIs();
  c->width.AsIs();
  m_nextPageButton->SetConstraints(c);

  c = new wxLayoutConstraints;
  c->right.SameAs(m_nextPageButton, wxLeft, 4);
  c->bottom.SameAs(window, wxBottom, 4);
  c->height.AsIs();
  c->width.AsIs();
  m_cancelButton->SetConstraints(c);

  c = new wxLayoutConstraints;
  c->right.SameAs(m_cancelButton, wxLeft, 4);
  c->bottom.SameAs(window, wxBottom, 4);
  c->height.AsIs();
  c->width.AsIs();
  m_okButton->SetConstraints(c);

  // Add some panels
  wxPanel *panel1 = new wxPanel(notebook, -1);
  //  panel1->SetBackgroundColour(wxColour("RED"));
  (void)new wxButton(panel1, -1, "Press me", wxPoint(10, 10));
  (void)new wxTextCtrl(panel1, -1, "1234", wxPoint(10, 40), wxSize(120, 150));

  notebook->AddPage(panel1, "Cat", TRUE);

  wxPanel *panel2 = new wxPanel(notebook, -1);
  panel2->SetAutoLayout(TRUE);
  panel2->SetBackgroundColour(wxColour("BLUE"));

  wxString animals[] = { "Fox", "Hare", "Rabbit", "Sabre-toothed tiger", "T Rex" };
  wxRadioBox *radiobox = new wxRadioBox(panel2, -1, "Choose one",
                                        wxDefaultPosition, wxDefaultSize, 5, animals,
                                        2, wxRA_SPECIFY_ROWS);

  c = new wxLayoutConstraints;
  c->left.SameAs(panel2, wxLeft, 10);
  c->top.SameAs(panel2, wxTop, 5);
  c->height.PercentOf(panel2, wxHeight, 50);
  c->right.SameAs(panel2, wxRight, 10);
  radiobox->SetConstraints(c);

  wxRadioBox *radiobox2 = new wxRadioBox(panel2, -1, "Choose one",
                                         wxDefaultPosition, wxDefaultSize,
                                         5, animals,
                                         2, wxRA_SPECIFY_ROWS);

  c = new wxLayoutConstraints;
  c->left.SameAs(radiobox, wxLeft);
  c->height.AsIs();
  c->top.Below(radiobox, 5);
  c->right.SameAs(radiobox, wxRight);
  radiobox2->SetConstraints(c);

  notebook->AddPage(panel2, "Dog");

  wxPanel *panel3 = new wxPanel(notebook, -1);
  panel3->SetBackgroundColour(wxColour("WHITE"));
  notebook->AddPage(panel3, "Goat");

  wxPanel *panel4 = new wxPanel(notebook, -1);
  panel4->SetBackgroundColour(wxColour("YELLOW"));
  notebook->AddPage(panel4, "Sheep");

  wxPanel *panel5 = new wxPanel(notebook, -1);
  panel5->SetBackgroundColour(wxColour("MAGENTA"));
  (void)new wxStaticText(panel5, -1, "This page has been inserted, not added", wxPoint(10, 10) );
  notebook->InsertPage(0, panel5, "Sheep");

  notebook->SetSelection(2);
}

#if USE_TABBED_DIALOG

BEGIN_EVENT_TABLE(MyDialog, wxDialog)
    EVT_BUTTON(wxID_OK, MyDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, MyDialog::OnOK)
END_EVENT_TABLE()

MyDialog::MyDialog(wxWindow* parent, const wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, const long windowStyle):
  wxDialog(parent, id, title, pos, size, windowStyle)
{
    Init();
}

void MyDialog::OnOK(wxCommandEvent& WXUNUSED(event) )
{
    EndModal(wxID_OK);
}

void MyDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event) )
{
    EndModal(wxID_CANCEL);
}

void MyDialog::Init()
{
  m_notebook = new wxNotebook(this, ID_NOTEBOOK);

  wxLayoutConstraints* c = new wxLayoutConstraints;
  c->left.SameAs(this, wxLeft, 4);
  c->right.SameAs(this, wxRight, 4);
  c->top.SameAs(this, wxTop, 4);
  c->bottom.SameAs(this, wxBottom, 40);

  m_notebook->SetConstraints(c);

  wxGetApp().InitTabView(m_notebook, this);

  SetAutoLayout(TRUE);
  Layout();

  Centre(wxBOTH);
}

#else // USE_TABBED_DIALOG

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_BUTTON(wxID_OK, MyFrame::OnOK)
    EVT_BUTTON(ID_DELETE_PAGE, MyFrame::OnDeletePage)
    EVT_BUTTON(ID_ADD_PAGE, MyFrame::OnAddPage)
    EVT_BUTTON(ID_INSERT_PAGE, MyFrame::OnInsertPage)
    EVT_BUTTON(ID_NEXT_PAGE, MyFrame::OnNextPage)
    EVT_IDLE(MyFrame::OnIdle)
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame* parent, const wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, const long windowStyle):
  wxFrame(parent, id, title, pos, size, windowStyle)
{
    m_panel = (wxPanel*) NULL;
    m_notebook = (wxNotebook*) NULL;
    Init();
}

void MyFrame::OnAddPage(wxCommandEvent& WXUNUSED(event))
{
    static size_t s_pageAdded = 0;

    wxPanel *panel = new wxPanel( m_notebook, -1 );
    (void)new wxButton( panel, -1, "Button", wxPoint( 10,10 ), wxSize(-1,-1) );

    m_notebook->AddPage( panel, wxString::Format("Added %u", ++s_pageAdded) );
}

void MyFrame::OnInsertPage(wxCommandEvent& WXUNUSED(event))
{
    static size_t s_pageIns = 0;

    wxPanel *panel = new wxPanel( m_notebook, -1 );
    (void)new wxButton( panel, -1, "Button", wxPoint( 10,10 ), wxSize(-1,-1) );

    m_notebook->InsertPage(0, panel, wxString::Format("Inserted %u", ++s_pageIns) );
    m_notebook->SetSelection(0);
}

void MyFrame::OnDeletePage(wxCommandEvent& WXUNUSED(event))
{
    m_notebook->DeletePage( m_notebook->GetPageCount()-1 );
}

void MyFrame::OnNextPage(wxCommandEvent& WXUNUSED(event))
{
    m_notebook->AdvanceSelection();
}

void MyFrame::OnOK(wxCommandEvent& WXUNUSED(event) )
{
    Destroy();
}

void MyFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event) )
{
    Destroy();
}

void MyFrame::Init()
{
  m_panel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxCLIP_CHILDREN);

  wxLayoutConstraints* c = new wxLayoutConstraints;
  c->left.SameAs(this, wxLeft);
  c->right.SameAs(this, wxRight);
  c->top.SameAs(this, wxTop);
  c->bottom.SameAs(this, wxBottom);
  m_panel->SetConstraints(c);

  // Note, omit the wxTAB_STYLE_COLOUR_INTERIOR, so we will guarantee a match
  // with the panel background, and save a bit of time.
  m_notebook = new wxNotebook(m_panel, ID_NOTEBOOK);

  c = new wxLayoutConstraints;
  c->left.SameAs(m_panel, wxLeft, 4);
  c->right.SameAs(m_panel, wxRight, 4);
  c->top.SameAs(m_panel, wxTop, 4);
  c->bottom.SameAs(m_panel, wxBottom, 40);

  m_notebook->SetConstraints(c);

  wxGetApp().InitTabView(m_notebook, m_panel);

  m_panel->SetAutoLayout(TRUE);
  SetAutoLayout(TRUE);

  Centre(wxBOTH);

  Show(TRUE);
}

void MyFrame::OnIdle(wxIdleEvent& WXUNUSED(event))
{
    static int s_nPages = -1;
    static int s_nSel = -1;

    int nPages = m_notebook->GetPageCount();
    int nSel = m_notebook->GetSelection();
    if ( nPages != s_nPages || nSel != s_nSel )
    {
        s_nPages = nPages;
        s_nSel = nSel;

        wxString title;
        title.Printf("Notebook (%d pages, selection: %d)", nPages, nSel);

        SetTitle(title);
    }
}

#endif // USE_TABBED_DIALOG

/////////////////////////////////////////////////////////////////////////////
// Name:        test.cpp
// Purpose:     wxNotebook demo
// Author:      Julian Smart
// Modified by:
// Created:     26/10/98
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/tab.h"
#include "test.h"

// If 1, use a dialog. Otherwise use a frame.
#define USE_TABBED_DIALOG 0

MyDialog* dialog = (MyDialog *) NULL;
MyFrame*  frame = (MyFrame *) NULL;

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit(void)
{
  // Create the main window
#if USE_TABBED_DIALOG
  dialog = new MyDialog((wxFrame *) NULL, -1, (char *) "Notebook", wxPoint(-1, -1), wxSize(365, 390), wxDIALOG_MODAL|wxDEFAULT_DIALOG_STYLE);

  dialog->ShowModal();

  // Quit immediately the dialog has been dismissed
  return FALSE;
#else
  frame = new MyFrame((wxFrame*) NULL, -1, (char *) "Notebook", wxPoint(-1, -1), wxSize(365, 390), wxDEFAULT_FRAME_STYLE);

  // Problem with Motif whereby it doesn't size properly unless
  // you set the size again (to a different size than before,
  // since SetSize is optimized)
#ifdef __WXMOTIF__
  frame->SetSize(-1, -1, 370, 390);
#endif

  return TRUE;
#endif
}

void MyApp::InitTabView(wxNotebook* notebook, wxWindow* window)
{
  m_okButton = new wxButton(window, wxID_OK, "Close", wxPoint(-1, -1), wxSize(80, 25));
  m_cancelButton = new wxButton(window, wxID_CANCEL, "Cancel", wxPoint(-1, -1), wxSize(80, 25));
  m_helpButton = new wxButton(window, wxID_HELP, "Help", wxPoint(-1, -1), wxSize(80, 25));
  m_okButton->SetDefault();

  wxLayoutConstraints* c = new wxLayoutConstraints;
  c->right.SameAs(window, wxRight, 4);
  c->bottom.SameAs(window, wxBottom, 4);
  c->height.AsIs();
  c->width.AsIs();
  m_helpButton->SetConstraints(c);

  c = new wxLayoutConstraints;
  c->right.SameAs(m_helpButton, wxLeft, 4);
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
  panel2->SetBackgroundColour(wxColour("BLUE"));

  wxString animals[] = { "Fox", "Hare", "Rabbit", "Sabre-toothed tiger", "T Rex" };
  (void)new wxListBox(panel2, -1, wxPoint(5, 5), wxSize(170, 80), 5, animals);

  (void)new wxTextCtrl(panel2, -1, "Some notes about the animals in this house", wxPoint(5, 100), wxSize(170, 100),
    wxTE_MULTILINE);

  notebook->AddPage(panel2, "Dog");

  wxPanel *panel3 = new wxPanel(notebook, -1);
  panel3->SetBackgroundColour(wxColour("WHITE"));
  notebook->AddPage(panel3, "Goat");

  wxPanel *panel4 = new wxPanel(notebook, -1);
  panel4->SetBackgroundColour(wxColour("YELLOW"));
  notebook->AddPage(panel4, "Sheep");
}

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

void MyDialog::Init(void)
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

  this->Centre(wxBOTH);
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_BUTTON(wxID_OK, MyFrame::OnOK)
    EVT_BUTTON(wxID_CANCEL, MyFrame::OnOK)
    EVT_SIZE(MyFrame::OnSize)
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame* parent, const wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, const long windowStyle):
  wxFrame(parent, id, title, pos, size, windowStyle)
{
    m_panel = (wxPanel*) NULL;
    m_notebook = (wxNotebook*) NULL;
    Init();
}

void MyFrame::OnOK(wxCommandEvent& WXUNUSED(event) )
{
    this->Destroy();
}

void MyFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event) )
{
    this->Destroy();
}

void MyFrame::Init(void)
{
  m_panel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxCLIP_CHILDREN);

  // Note, omit the wxTAB_STYLE_COLOUR_INTERIOR, so we will guarantee a match
  // with the panel background, and save a bit of time.
  m_notebook = new wxNotebook(m_panel, ID_NOTEBOOK);

  wxLayoutConstraints* c = new wxLayoutConstraints;
  c->left.SameAs(m_panel, wxLeft, 4);
  c->right.SameAs(m_panel, wxRight, 4);
  c->top.SameAs(m_panel, wxTop, 4);
  c->bottom.SameAs(m_panel, wxBottom, 40);

  m_notebook->SetConstraints(c);

  wxGetApp().InitTabView(m_notebook, m_panel);

  m_panel->SetAutoLayout(TRUE);

  m_panel->Layout();

  this->Centre(wxBOTH);

  Show(TRUE);
}

void MyFrame::OnSize(wxSizeEvent& event)
{
    wxFrame::OnSize(event);
    m_panel->Layout();
}


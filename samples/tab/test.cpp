/////////////////////////////////////////////////////////////////////////////
// Name:        test.cpp
// Purpose:     Tab demo
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
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
  dialog = new MyDialog((wxFrame *) NULL, -1, (char *) "Tabbed Dialog", wxPoint(-1, -1), wxSize(365, 390), wxDIALOG_MODAL|wxDEFAULT_DIALOG_STYLE);

  dialog->ShowModal();

  // Quit immediately the dialog has been dismissed
  return FALSE;
#else
  frame = new MyFrame((wxFrame*) NULL, -1, (char *) "Tabbed Panel", wxPoint(-1, -1), wxSize(365, 390), wxDEFAULT_FRAME_STYLE);

  return TRUE;
#endif
}

void MyApp::InitTabView(wxPanelTabView* view, wxWindow* window)
{
  int dialogWidth = 365;
  int dialogHeight = 390;
  
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

  wxRect rect;
  rect.x = 5;
  rect.y = 70;
  // Could calculate the view width from the tab width and spacing,
  // as below, but let's assume we have a fixed view width.
//  rect.width = view->GetTabWidth()*4 + 3*view->GetHorizontalTabSpacing();
  rect.width = 326;
  rect.height = 250;
  
  view->SetViewRect(rect);

  // Calculate the tab width for 4 tabs, based on a view width of 326 and
  // the current horizontal spacing. Adjust the view width to exactly fit
  // the tabs.
  view->CalculateTabWidth(4, TRUE);

  if (!view->AddTab(TEST_TAB_CAT,        wxString("Cat")))
    return;

  if (!view->AddTab(TEST_TAB_DOG,        wxString("Dog")))
    return;
  if (!view->AddTab(TEST_TAB_GUINEAPIG,  wxString("Guinea Pig")))
    return;
  if (!view->AddTab(TEST_TAB_GOAT,       wxString("Goat")))
    return;
  if (!view->AddTab(TEST_TAB_ANTEATER,   wxString("Ant-eater")))
    return;
  if (!view->AddTab(TEST_TAB_SHEEP,      wxString("Sheep")))
    return;
  if (!view->AddTab(TEST_TAB_COW,        wxString("Cow")))
    return;
  if (!view->AddTab(TEST_TAB_HORSE,      wxString("Horse")))
    return;
  if (!view->AddTab(TEST_TAB_PIG,        wxString("Pig")))
    return;
  if (!view->AddTab(TEST_TAB_OSTRICH,    wxString("Ostrich")))
    return;
  if (!view->AddTab(TEST_TAB_AARDVARK,   wxString("Aardvark")))
    return;
  if (!view->AddTab(TEST_TAB_HUMMINGBIRD,wxString("Hummingbird")))
    return;
    
  // Add some panels
  wxPanel *panel1 = new wxPanel(window, -1, wxPoint(rect.x + 20, rect.y + 10), wxSize(290, 220), wxTAB_TRAVERSAL);
  (void)new wxButton(panel1, -1, "Press me", wxPoint(10, 10));
  (void)new wxTextCtrl(panel1, -1, "1234", wxPoint(10, 40), wxSize(120, 150));
  
  view->AddTabWindow(TEST_TAB_CAT, panel1);

  wxPanel *panel2 = new wxPanel(window, -1, wxPoint(rect.x + 20, rect.y + 10), wxSize(290, 220));

  wxString animals[] = { "Fox", "Hare", "Rabbit", "Sabre-toothed tiger", "T Rex" };
  (void)new wxListBox(panel2, -1, wxPoint(5, 5), wxSize(170, 80), 5, animals);

  (void)new wxTextCtrl(panel2, -1, "Some notes about the animals in this house", wxPoint(5, 100), wxSize(170, 100),
    wxTE_MULTILINE);
  
  view->AddTabWindow(TEST_TAB_DOG, panel2);
  view->SetTabSelection(TEST_TAB_CAT);
}

BEGIN_EVENT_TABLE(MyDialog, wxTabbedDialog)
    EVT_BUTTON(wxID_OK, MyDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, MyDialog::OnOK)
END_EVENT_TABLE()

MyDialog::MyDialog(wxWindow* parent, const wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, const long windowStyle):
  wxTabbedDialog(parent, id, title, pos, size, windowStyle)
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
  int dialogWidth = 365;
  int dialogHeight = 390;
  
  // Note, omit the wxTAB_STYLE_COLOUR_INTERIOR, so we will guarantee a match
  // with the panel background, and save a bit of time.
  wxPanelTabView *view = new wxPanelTabView(this, wxTAB_STYLE_DRAW_BOX);

  wxGetApp().InitTabView(view, this);

  // Don't know why this is necessary under Motif...
#ifndef __WXMSW__
  this->SetSize(dialogWidth, dialogHeight-20);
#endif

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
    m_panel = (wxTabbedPanel*) NULL;
    m_view = (wxPanelTabView*) NULL;
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
  int dialogWidth = 365;
  int dialogHeight = 390;

  m_panel = new wxTabbedPanel(this, -1);
  
  // Note, omit the wxTAB_STYLE_COLOUR_INTERIOR, so we will guarantee a match
  // with the panel background, and save a bit of time.
  m_view = new wxPanelTabView(m_panel, wxTAB_STYLE_DRAW_BOX);

  wxGetApp().InitTabView(m_view, m_panel);

  this->Centre(wxBOTH);

  Show(TRUE);
}

void MyFrame::OnSize(wxSizeEvent& event)
{
    wxFrame::OnSize(event);

    int cw, ch;
    GetClientSize(& cw, & ch);

    if (m_view && m_panel)
    {
        m_panel->Layout();

        int tabHeight = m_view->GetTotalTabHeight();
        wxRect rect;
        rect.x = 4;
        rect.y = tabHeight + 4;
        rect.width = cw - 8;
        rect.height = ch - 4 - rect.y - 30; // 30 for buttons
  
        m_view->SetViewRect(rect);

        m_view->Layout();

        // Need to do it a 2nd time to get the tab height with
        // the new view width
        tabHeight = m_view->GetTotalTabHeight();
        rect.x = 4;
        rect.y = tabHeight + 4;
        rect.width = cw - 8;
        rect.height = ch - 4 - rect.y - 30; // 30 for buttons
  
        m_view->SetViewRect(rect);

        m_view->Layout();

        // Move all the panels to the new view position and size
        wxNode* node = m_view->GetWindows().First();
        while (node)
        {
            wxWindow* win = (wxWindow*) node->Data();
            win->SetSize(rect.x+2, rect.y+2, rect.width-4, rect.height-4);

            node = node->Next();
        }

        m_panel->Refresh();
    }
}


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

// Declare two frames
MyDialog   *dialog = NULL;

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit(void)
{
  // Create the main frame window
  dialog = new MyDialog(NULL, -1, "Tabbed Dialog", wxPoint(-1, -1), wxSize(365, 390), wxDIALOG_MODAL|wxDEFAULT_DIALOG_STYLE);

  dialog->ShowModal();

  // Quit immediately the dialog has been dismissed
  return FALSE;
}

BEGIN_EVENT_TABLE(MyDialog, wxTabbedDialog)
    EVT_BUTTON(wxID_OK, MyDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, MyDialog::OnOK)
//    EVT_MENU(TEST_ABOUT, MyDialog::OnAbout)
END_EVENT_TABLE()

MyDialog::MyDialog(wxWindow* parent, const wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, const long windowStyle):
  wxTabbedDialog(parent, id, title, pos, size, windowStyle)
{
    Init();
}

void MyDialog::OnOK(wxCommandEvent& event)
{
    EndModal(wxID_OK);
}

void MyDialog::OnCloseWindow(wxCloseEvent& event)
{
    EndModal(wxID_CANCEL);
}

void MyDialog::Init(void)
{
  int dialogWidth = 365;
  int dialogHeight = 390;
  
  wxButton *okButton = new wxButton(this, wxID_OK, "Close", wxPoint(100, 330), wxSize(80, 25));
  wxButton *cancelButton = new wxButton(this, wxID_CANCEL, "Cancel", wxPoint(185, 330), wxSize(80, 25));
  wxButton *HelpButton = new wxButton(this, wxID_HELP, "Help", wxPoint(270, 330), wxSize(80, 25));
  okButton->SetDefault();

  // Note, omit the wxTAB_STYLE_COLOUR_INTERIOR, so we will guarantee a match
  // with the panel background, and save a bit of time.
  wxPanelTabView *view = new wxPanelTabView((wxPanel*)this, wxTAB_STYLE_DRAW_BOX);

  wxRectangle rect;
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
  wxPanel *panel1 = new wxPanel(this, -1, wxPoint(rect.x + 20, rect.y + 10), wxSize(290, 220), wxTAB_TRAVERSAL);
  (void)new wxButton(panel1, -1, "Press me", wxPoint(10, 10));
  (void)new wxTextCtrl(panel1, -1, "1234", wxPoint(10, 40), wxSize(120, 150));
  
  view->AddTabWindow(TEST_TAB_CAT, panel1);

  wxPanel *panel2 = new wxPanel(this, -1, wxPoint(rect.x + 20, rect.y + 10), wxSize(290, 220));

  wxString animals[] = { "Fox", "Hare", "Rabbit", "Sabre-toothed tiger", "T Rex" };
  (void)new wxListBox(panel2, -1, wxPoint(5, 5), wxSize(170, 80), 5, animals);

  (void)new wxTextCtrl(panel2, -1, "Some notes about the animals in this house", wxPoint(5, 100), wxSize(170, 100)),
    wxTE_MULTILINE;
  
  view->AddTabWindow(TEST_TAB_DOG, panel2);
  
  // Don't know why this is necessary under Motif...
#ifdef wx_motif
  this->SetSize(dialogWidth, dialogHeight-20);
#endif

  view->SetTabSelection(TEST_TAB_CAT);
  
  this->Centre(wxBOTH);
}


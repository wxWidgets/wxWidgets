/////////////////////////////////////////////////////////////////////////////
// Name:        test.cpp
// Purpose:     Property sheet test implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "test.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "test.h"

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

IMPLEMENT_APP(MyApp)

wxPropertyValidatorRegistry myListValidatorRegistry;
wxPropertyValidatorRegistry myFormValidatorRegistry;

MyApp::MyApp(void)
{
    m_childWindow = NULL;
    m_mainFrame = NULL;
}

bool MyApp::OnInit(void)
{
  RegisterValidators();

  // Create the main frame window
  m_mainFrame = new MyFrame(NULL, "wxPropertySheet Demo", wxPoint(0, 0), wxSize(300, 400), wxDEFAULT_FRAME_STYLE);

  // Make a menubar
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(PROPERTY_TEST_DIALOG_LIST, "Test property list &dialog...");
  file_menu->Append(PROPERTY_TEST_FRAME_LIST, "Test property list &frame...");
  file_menu->AppendSeparator();
  file_menu->Append(PROPERTY_TEST_DIALOG_FORM, "Test property form d&ialog...");
  file_menu->Append(PROPERTY_TEST_FRAME_FORM, "Test property form f&rame...");
  file_menu->AppendSeparator();
  file_menu->Append(PROPERTY_QUIT, "E&xit");

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(PROPERTY_ABOUT, "&About");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(help_menu, "&Help");

  // Associate the menu bar with the frame
  m_mainFrame->SetMenuBar(menu_bar);

  m_mainFrame->Centre(wxBOTH);
  m_mainFrame->Show(TRUE);

  SetTopWindow(m_mainFrame);
    
  return TRUE;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_CLOSE(MyFrame::OnCloseWindow)
    EVT_MENU(PROPERTY_QUIT, MyFrame::OnQuit)
    EVT_MENU(PROPERTY_ABOUT, MyFrame::OnAbout)
    EVT_MENU(PROPERTY_TEST_DIALOG_LIST, MyFrame::OnDialogList)
    EVT_MENU(PROPERTY_TEST_FRAME_LIST, MyFrame::OnFrameList)
    EVT_MENU(PROPERTY_TEST_DIALOG_FORM, MyFrame::OnDialogForm)
    EVT_MENU(PROPERTY_TEST_FRAME_FORM, MyFrame::OnFrameForm)
END_EVENT_TABLE()

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size, long type):
  wxFrame(frame, -1, title, pos, size, type)
{
}

// Define the behaviour for the frame closing
// - must delete all frames except for the main one.
void MyFrame::OnCloseWindow(wxCloseEvent& event)
{
    if (wxGetApp().m_childWindow)
    {
        wxGetApp().m_childWindow->Close(TRUE);
    }

    Destroy();
}

void MyFrame::OnQuit(wxCommandEvent& event)
{
    Close(TRUE);
}

void MyFrame::OnDialogList(wxCommandEvent& event)
{
    wxGetApp().PropertyListTest(TRUE);
}

void MyFrame::OnFrameList(wxCommandEvent& event)
{
    wxGetApp().PropertyListTest(FALSE);
}

void MyFrame::OnDialogForm(wxCommandEvent& event)
{
    wxGetApp().PropertyFormTest(TRUE);
}

void MyFrame::OnFrameForm(wxCommandEvent& event)
{
    wxGetApp().PropertyFormTest(FALSE);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    (void)wxMessageBox("Property Classes Demo\nAuthor: Julian Smart", "About Property Classes Test");
}

void MyApp::RegisterValidators(void)
{
  myListValidatorRegistry.RegisterValidator((wxString)"real", new wxRealListValidator);
  myListValidatorRegistry.RegisterValidator((wxString)"string", new wxStringListValidator);
  myListValidatorRegistry.RegisterValidator((wxString)"integer", new wxIntegerListValidator);
  myListValidatorRegistry.RegisterValidator((wxString)"bool", new wxBoolListValidator);
  myListValidatorRegistry.RegisterValidator((wxString)"stringlist", new wxListOfStringsListValidator);

  myFormValidatorRegistry.RegisterValidator((wxString)"real", new wxRealFormValidator);
  myFormValidatorRegistry.RegisterValidator((wxString)"string", new wxStringFormValidator);
  myFormValidatorRegistry.RegisterValidator((wxString)"integer", new wxIntegerFormValidator);
  myFormValidatorRegistry.RegisterValidator((wxString)"bool", new wxBoolFormValidator);
}

void MyApp::PropertyListTest(bool useDialog)
{
  if (m_childWindow)
    return;

  wxPropertySheet *sheet = new wxPropertySheet;

  sheet->AddProperty(new wxProperty("fred", 1.0, "real"));
  sheet->AddProperty(new wxProperty("tough choice", (bool)TRUE, "bool"));
  sheet->AddProperty(new wxProperty("ian", (long)45, "integer", new wxIntegerListValidator(-50, 50)));
  sheet->AddProperty(new wxProperty("bill", 25.0, "real", new wxRealListValidator(0.0, 100.0)));
  sheet->AddProperty(new wxProperty("julian", "one", "string"));
  sheet->AddProperty(new wxProperty("bitmap", "none", "string", new wxFilenameListValidator("Select a bitmap file", "*.bmp")));
  wxStringList *strings = new wxStringList("one", "two", "three", NULL);
  sheet->AddProperty(new wxProperty("constrained", "one", "string", new wxStringListValidator(strings)));

  wxStringList *strings2 = new wxStringList("earth", "fire", "wind", "water", NULL);
  sheet->AddProperty(new wxProperty("string list", strings2, "stringlist"));

  wxPropertyListView *view =
    new wxPropertyListView(NULL,
     wxPROP_BUTTON_CHECK_CROSS|wxPROP_DYNAMIC_VALUE_FIELD|wxPROP_PULLDOWN|wxPROP_SHOWVALUES);

  wxDialog *propDialog = NULL;
  wxPropertyListFrame *propFrame = NULL;
  if (useDialog)
  {
    propDialog = new PropListDialog(view, NULL, "Property Sheet Test",
		wxPoint(-1, -1), wxSize(400, 500), wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODELESS);
    m_childWindow = propDialog;
  }
  else
  {
    propFrame = new PropListFrame(view, NULL, "Property Sheet Test", wxPoint(-1, -1), wxSize(400, 500));
    m_childWindow = propFrame;
  }
  
  view->AddRegistry(&myListValidatorRegistry);

  if (useDialog)
  {
    view->ShowView(sheet, propDialog);
    propDialog->Centre(wxBOTH);
    propDialog->Show(TRUE);
  }
  else
  {
    propFrame->Initialize();
    view->ShowView(sheet, propFrame->GetPropertyPanel());

    propFrame->Centre(wxBOTH);
    propFrame->Show(TRUE);
  }
}

void MyApp::PropertyFormTest(bool useDialog)
{
  if (m_childWindow)
    return;

#if 0
  wxPropertySheet *sheet = new wxPropertySheet;

  sheet->AddProperty(new wxProperty("fred", 25.0, "real", new wxRealFormValidator(0.0, 100.0)));
  sheet->AddProperty(new wxProperty("tough choice", (bool)TRUE, "bool"));
  sheet->AddProperty(new wxProperty("ian", (long)45, "integer", new wxIntegerFormValidator(-50, 50)));
  sheet->AddProperty(new wxProperty("julian", "one", "string"));
  wxStringList *strings = new wxStringList("one", "two", "three", NULL);
  sheet->AddProperty(new wxProperty("constrained", "one", "string", new wxStringFormValidator(strings)));

  wxPropertyFormView *view = new wxPropertyFormView(NULL);

  wxDialogBox *propDialog = NULL;
  wxPropertyFormFrame *propFrame = NULL;
  if (useDialog)
  {
    propDialog = new PropFormDialog(view, NULL, "Property Form Test", wxPoint(-1, -1), wxSize(380, 250),
		wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL);
    m_childWindow = propDialog;
  }
  else
  {
    propFrame = new PropFormFrame(view, NULL, "Property Form Test", wxPoint(-1, -1), wxSize(280, 250));
    propFrame->Initialize();
    m_childWindow = propFrame;
  }
  
  wxPanel *panel = propDialog ? propDialog : propFrame->GetPropertyPanel();
  panel->SetLabelPosition(wxVERTICAL);
  
  // Add items to the panel
  
  (void) new wxButton(panel, -1, "OK", -1, -1, -1, -1, 0, "ok");
  (void) new wxButton(panel, -1, "Cancel", -1, -1, 80, -1, 0, "cancel");
  (void) new wxButton(panel, -1, "Update", -1, -1, 80, -1, 0, "update");
  (void) new wxButton(panel, -1, "Revert", -1, -1, -1, -1, 0, "revert");
  panel->NewLine();
  
  // The name of this text item matches the "fred" property
  (void) new wxText(panel, -1, "Fred", "", -1, -1, 90, -1, 0, "fred");
  (void) new wxCheckBox(panel, -1, "Yes or no", -1, -1, -1, -1, 0, "tough choice");
  (void) new wxSlider(panel, -1, "Scale", 0, -50, 50, 150, -1, -1, wxHORIZONTAL, "ian");
  panel->NewLine();
  (void) new wxListBox(panel, -1, "Constrained", wxSINGLE, -1, -1, 100, 90, 0, NULL, 0, "constrained");

  view->AddRegistry(&myFormValidatorRegistry);

  if (useDialog)
  {
    view->ShowView(sheet, propDialog);
    view->AssociateNames();
    view->TransferToDialog();
    propDialog->Centre(wxBOTH);
    propDialog->Show(TRUE);
  }
  else
  {
    view->ShowView(sheet, propFrame->GetPropertyPanel());
    view->AssociateNames();
    view->TransferToDialog();
    propFrame->Centre(wxBOTH);
    propFrame->Show(TRUE);
  }
#endif
}

BEGIN_EVENT_TABLE(PropListFrame, wxPropertyListFrame)
    EVT_CLOSE(PropListFrame::OnCloseWindow)
END_EVENT_TABLE()

void PropListFrame::OnCloseWindow(wxCloseEvent& event)
{
    wxGetApp().m_childWindow = NULL;

    wxPropertyListFrame::OnCloseWindow(event);
}

BEGIN_EVENT_TABLE(PropListDialog, wxPropertyListDialog)
    EVT_CLOSE(PropListDialog::OnCloseWindow)
END_EVENT_TABLE()

void PropListDialog::OnCloseWindow(wxCloseEvent& event)
{
    wxGetApp().m_childWindow = NULL;

    wxPropertyListDialog::OnCloseWindow(event);
}

BEGIN_EVENT_TABLE(PropFormFrame, wxPropertyFormFrame)
    EVT_CLOSE(PropFormFrame::OnCloseWindow)
END_EVENT_TABLE()

void PropFormFrame::OnCloseWindow(wxCloseEvent& event)
{
    wxGetApp().m_childWindow = NULL;

    wxPropertyFormFrame::OnCloseWindow(event);
}

BEGIN_EVENT_TABLE(PropFormDialog, wxPropertyFormDialog)
    EVT_CLOSE(PropFormDialog::OnCloseWindow)
END_EVENT_TABLE()

void PropFormDialog::OnCloseWindow(wxCloseEvent& event)
{
    wxGetApp().m_childWindow = NULL;

    wxPropertyFormDialog::OnCloseWindow(event);
}


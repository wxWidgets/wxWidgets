/////////////////////////////////////////////////////////////////////////////
// Name:        proplist.cpp
// Purpose:     Property sheet test implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "proplist_sample.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#if !wxUSE_PROPSHEET
#error Please set wxUSE_PROPSHEET to 1 in include/wx/msw/setup.h and recompile.
#endif

#include "proplist.h"

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
  m_mainFrame = new MyFrame(NULL, _T("wxPropertySheet Demo"), wxPoint(0, 0), wxSize(300, 400), wxDEFAULT_FRAME_STYLE);

  // Make a menubar
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(PROPERTY_TEST_DIALOG_LIST, _T("Test property list &dialog..."));
  file_menu->Append(PROPERTY_TEST_FRAME_LIST, _T("Test property list &frame..."));
  file_menu->AppendSeparator();
  file_menu->Append(PROPERTY_TEST_DIALOG_FORM, _T("Test property form d&ialog..."));
  file_menu->Append(PROPERTY_TEST_FRAME_FORM, _T("Test property form f&rame..."));
  file_menu->AppendSeparator();
  file_menu->Append(PROPERTY_QUIT, _T("E&xit"));

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(PROPERTY_ABOUT, _T("&About"));

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, _T("&File"));
  menu_bar->Append(help_menu, _T("&Help"));

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
    (void)wxMessageBox(_T("Property Classes Demo\nAuthor: Julian Smart"), _T("About Property Classes Test"));
}

void MyApp::RegisterValidators(void)
{
  myListValidatorRegistry.RegisterValidator((wxString)_T("real"), new wxRealListValidator);
  myListValidatorRegistry.RegisterValidator((wxString)_T("string"), new wxStringListValidator);
  myListValidatorRegistry.RegisterValidator((wxString)_T("integer"), new wxIntegerListValidator);
  myListValidatorRegistry.RegisterValidator((wxString)_T("bool"), new wxBoolListValidator);
  myListValidatorRegistry.RegisterValidator((wxString)_T("stringlist"), new wxListOfStringsListValidator);

  myFormValidatorRegistry.RegisterValidator((wxString)_T("real"), new wxRealFormValidator);
  myFormValidatorRegistry.RegisterValidator((wxString)_T("string"), new wxStringFormValidator);
  myFormValidatorRegistry.RegisterValidator((wxString)_T("integer"), new wxIntegerFormValidator);
  myFormValidatorRegistry.RegisterValidator((wxString)_T("bool"), new wxBoolFormValidator);
}

void MyApp::PropertyListTest(bool useDialog)
{
    if (m_childWindow)
        return;

    wxPropertySheet *sheet = new wxPropertySheet;

    sheet->AddProperty(new wxProperty(_T("fred"), 1.0, _T("real")));
    sheet->AddProperty(new wxProperty(_T("tough choice"), (bool)TRUE, _T("bool")));
    sheet->AddProperty(new wxProperty(_T("ian"), (long)45, _T("integer"), new wxIntegerListValidator(-50, 50)));
    sheet->AddProperty(new wxProperty(_T("bill"), 25.0, _T("real"), new wxRealListValidator(0.0, 100.0)));
    sheet->AddProperty(new wxProperty(_T("julian"), _T("one"), _T("string")));
    sheet->AddProperty(new wxProperty(_T("bitmap"), _T("none"), _T("string"), new wxFilenameListValidator(_T("Select a bitmap file"), _T("*.bmp"))));
    wxStringList *strings = new wxStringList(wxT("one"), wxT("two"), wxT("three"), NULL);
    sheet->AddProperty(new wxProperty(_T("constrained"), _T("one"), _T("string"), new wxStringListValidator(strings)));

    wxStringList *strings2 = new wxStringList(wxT("earth"), wxT("fire"), wxT("wind"), wxT("water"), NULL);
    sheet->AddProperty(new wxProperty(_T("string list"), strings2, _T("stringlist")));

    wxPropertyListView *view = new wxPropertyListView
        (
            NULL,
            wxPROP_BUTTON_OK | wxPROP_BUTTON_CANCEL | wxPROP_BUTTON_CHECK_CROSS
            |wxPROP_DYNAMIC_VALUE_FIELD|wxPROP_PULLDOWN|wxPROP_SHOWVALUES
        );

    wxDialog *propDialog = NULL;
    wxPropertyListFrame *propFrame = NULL;
    if (useDialog)
    {
        propDialog = new PropListDialog(view, NULL, _T("Property Sheet Test"),
        wxPoint(-1, -1), wxSize(400, 500), wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODELESS);
        m_childWindow = propDialog;
    }
    else
    {
        propFrame = new PropListFrame(view, NULL, _T("Property Sheet Test"), wxPoint(-1, -1), wxSize(400, 500));
        m_childWindow = propFrame;
    }

    view->AddRegistry(&myListValidatorRegistry);

    if (useDialog)
    {
        view->ShowView(sheet, (wxPanel *)propDialog);
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

    wxPropertySheet *sheet = new wxPropertySheet;

    sheet->AddProperty(new wxProperty(_T("fred"), 25.0, _T("real"), new wxRealFormValidator(0.0, 100.0)));
    sheet->AddProperty(new wxProperty(_T("tough choice"), (bool)TRUE, _T("bool")));
    sheet->AddProperty(new wxProperty(_T("ian"), (long)45, _T("integer"), new wxIntegerFormValidator(-50, 50)));
    sheet->AddProperty(new wxProperty(_T("julian"), _T("one"), _T("string")));
    wxStringList *strings = new wxStringList(wxT("one"), wxT("two"), wxT("three"), NULL);
    sheet->AddProperty(new wxProperty(_T("constrained"), _T("one"), _T("string"), new wxStringFormValidator(strings)));

    wxPropertyFormView *view = new wxPropertyFormView(NULL);

    wxDialog *propDialog = NULL;
    wxPropertyFormFrame *propFrame = NULL;

    if (useDialog)
    {
        propDialog = new PropFormDialog(view, NULL, _T("Property Form Test"),
        wxPoint(-1, -1), wxSize(380, 250), wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODAL);
        m_childWindow = propDialog;
    }
    else
    {
        propFrame = new PropFormFrame(view, NULL, _T("Property Form Test"),
            wxPoint(-1, -1), wxSize(380, 250));
        propFrame->Initialize();
        m_childWindow = propFrame;
    }

    // BCC32 does not like ?:
    wxWindow *panel ;
    if ( propDialog )
    {
        panel = propDialog;
    }
    else
    {
        panel = propFrame->GetPropertyPanel() ;
    }

    wxLayoutConstraints* c;

#if 0
    if (!propDialog)
    {
        c = new wxLayoutConstraints;
        c->left.SameAs(m_childWindow, wxLeft, 4);
        c->right.SameAs(m_childWindow, wxRight, 4);
        c->top.SameAs(m_childWindow, wxTop, 4);
        c->bottom.SameAs(m_childWindow, wxBottom, 40);

        panel->SetConstraints(c);
    }
#endif
 
    // Add items to the panel
    wxButton *okButton = new wxButton(panel, wxID_OK, _T("OK"), wxPoint(-1, -1),
        wxSize(80, 26), 0, wxDefaultValidator, _T("ok"));
    wxButton *cancelButton = new wxButton(panel, wxID_CANCEL, _T("Cancel"),  wxPoint(-1, -1),
        wxSize(80, 26), 0, wxDefaultValidator, _T("cancel"));
    wxButton *updateButton = new wxButton(panel, wxID_PROP_UPDATE, _T("Update"),  wxPoint(-1, -1),
        wxSize(80, 26), 0, wxDefaultValidator, _T("update"));
    wxButton *revertButton = new wxButton(panel, wxID_PROP_REVERT, _T("Revert"),  wxPoint(-1, -1),
        wxSize(80, 26), 0, wxDefaultValidator, _T("revert"));

    c = new wxLayoutConstraints;
    c->right.SameAs(panel, wxRight, 4);
    c->bottom.SameAs(panel, wxBottom, 4);
    c->height.AsIs();
    c->width.AsIs();
    revertButton->SetConstraints(c);

    c = new wxLayoutConstraints;
    c->right.SameAs(revertButton, wxLeft, 4);
    c->bottom.SameAs(panel, wxBottom, 4);
    c->height.AsIs();
    c->width.AsIs();
    updateButton->SetConstraints(c);

    c = new wxLayoutConstraints;
    c->right.SameAs(updateButton, wxLeft, 4);
    c->bottom.SameAs(panel, wxBottom, 4);
    c->height.AsIs();
    c->width.AsIs();
    cancelButton->SetConstraints(c);

    c = new wxLayoutConstraints;
    c->right.SameAs(cancelButton, wxLeft, 4);
    c->bottom.SameAs(panel, wxBottom, 4);
    c->height.AsIs();
    c->width.AsIs();
    okButton->SetConstraints(c);

    // The name of this text item matches the "fred" property
    wxTextCtrl *text = new wxTextCtrl(panel, -1, _T("Fred"), wxPoint(-1, -1), wxSize(
    200, -1), 0, wxDefaultValidator, _T("fred"));

    c = new wxLayoutConstraints;
    c->left.SameAs(panel, wxLeft, 4);
    c->top.SameAs(panel, wxTop, 4);
    c->height.AsIs();
    c->width.AsIs();
    text->SetConstraints(c);

    wxCheckBox *checkBox = new wxCheckBox(panel, -1, _T("Yes or no"), wxPoint(-1, -1),
        wxSize(-1, -1), 0, wxDefaultValidator, _T("tough choice"));

    c = new wxLayoutConstraints;
    c->left.SameAs(text, wxRight, 20);
    c->top.SameAs(panel, wxTop, 4);
    c->height.AsIs();
    c->width.AsIs();
    checkBox->SetConstraints(c);

  wxSlider *slider = new wxSlider(panel, -1, -50, 50, 150, wxPoint(-1, -1),
    wxSize(200,10), 0, wxDefaultValidator, _T("ian"));

    c = new wxLayoutConstraints;
    c->left.SameAs(panel, wxLeft, 4);
    c->top.SameAs(text, wxBottom, 10);
    c->height.AsIs();
    c->width.AsIs();
    slider->SetConstraints(c);

    wxListBox *listBox = new wxListBox(panel, -1, wxPoint(-1, -1),
        wxSize(200, 100), 0, NULL, 0, wxDefaultValidator, _T("constrained"));

    c = new wxLayoutConstraints;
    c->left.SameAs(panel, wxLeft, 4);
    c->top.SameAs(slider, wxBottom, 10);
    c->height.AsIs();
    c->width.AsIs();
    listBox->SetConstraints(c);

    view->AddRegistry(&myFormValidatorRegistry);

    panel->SetAutoLayout(TRUE);

    view->ShowView(sheet, panel);
    view->AssociateNames();
    view->TransferToDialog();

    if (useDialog) {
        propDialog->Layout();
        propDialog->Centre(wxBOTH);
        propDialog->Show(TRUE);
    }
    else
    {
        // panel->Layout();
        propFrame->Centre(wxBOTH);
        propFrame->Show(TRUE);
    }
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
    EVT_SIZE(PropFormFrame::OnSize)
END_EVENT_TABLE()

void PropFormFrame::OnCloseWindow(wxCloseEvent& event)
{
    wxGetApp().m_childWindow = NULL;

    wxPropertyFormFrame::OnCloseWindow(event);
}

void PropFormFrame::OnSize(wxSizeEvent& event)
{
    wxPropertyFormFrame::OnSize(event);
    GetPropertyPanel()->Layout();
}

BEGIN_EVENT_TABLE(PropFormDialog, wxPropertyFormDialog)
    EVT_CLOSE(PropFormDialog::OnCloseWindow)
END_EVENT_TABLE()

void PropFormDialog::OnCloseWindow(wxCloseEvent& event)
{
    wxGetApp().m_childWindow = NULL;

    wxPropertyFormDialog::OnCloseWindow(event);
}


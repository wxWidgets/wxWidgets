///////////////////////////////////////////////////////////////////////////////
// Name:        conftest.cpp
// Purpose:     demo of wxConfig and related classes
// Author:      Vadim Zeitlin
// Modified by:
// Created:     03.08.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "wx/log.h"
#include "wx/config.h"

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------
class MyApp: public wxApp
{
public:
  // implement base class virtuals
  virtual bool OnInit();
  virtual int OnExit();
};

class MyFrame: public wxFrame
{
public:
  MyFrame();
  virtual ~MyFrame();

  // callbacks
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnDelete(wxCommandEvent& event);
  bool OnClose() { return TRUE; }

private:
  wxTextCtrl *m_text;
  wxCheckBox *m_check;

  DECLARE_EVENT_TABLE()
};

enum
{
  Minimal_Quit,
  Minimal_About,
  Minimal_Delete
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(Minimal_Quit, MyFrame::OnQuit)
  EVT_MENU(Minimal_About, MyFrame::OnAbout)
  EVT_MENU(Minimal_Delete, MyFrame::OnDelete)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// application
// ----------------------------------------------------------------------------
IMPLEMENT_APP(MyApp)

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
  // we're using wxConfig's "create-on-demand" feature: it will create the
  // config object when it's used for the first time. It has a number of
  // advantages compared with explicitly creating our wxConfig:
  //  1) we don't pay for it if we don't use it
  //  2) there is no danger to create it twice

  // application and vendor name are used by wxConfig to construct the name
  // of the config file/registry key and must be set before the first call
  // to Get() if you want to override the default values (the application
  // name is the name of the executable and the vendor name is the same)
  SetVendorName("wxWindows");
  SetAppName("conftest"); // not needed, it's the default value

  wxConfigBase *pConfig = wxConfigBase::Get();

  // or you could also write something like this:
  //  wxFileConfig *pConfig = new wxFileConfig("conftest");
  //  wxConfigBase::Set(pConfig);
  // where you can also specify the file names explicitly if you wish.
  // Of course, calling Set() is optional and you only must do it if
  // you want to later retrieve this pointer with Get().

  // create the main program window
  MyFrame *frame = new MyFrame;
  frame->Show(TRUE);
  SetTopWindow(frame);

  // use our config object...
  if ( pConfig->Read("/Controls/Check", 1l) != 0 ) {
    wxMessageBox("You can disable this message box by unchecking\n"
                 "the checkbox in the main window (of course, a real\n"
                 "program would have a checkbox right here but we\n"
                 "keep it simple)", "Welcome to wxConfig demo",
                 wxICON_INFORMATION | wxOK);
  }

  return TRUE;
}

int MyApp::OnExit()
{
  // clean up: Set() returns the active config object as Get() does, but unlike
  // Get() it doesn't try to create one if there is none (definitely not what
  // we want here!)
  delete wxConfigBase::Set((wxConfigBase *) NULL);

  return 0;
}

// ----------------------------------------------------------------------------
// frame
// ----------------------------------------------------------------------------

// main frame ctor
MyFrame::MyFrame()
       : wxFrame((wxFrame *) NULL, -1, "wxConfig Demo")
{
  // menu
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(Minimal_Delete, "&Delete", "Delete config file");
  file_menu->AppendSeparator();
  file_menu->Append(Minimal_About, "&About", "About this sample");
  file_menu->AppendSeparator();
  file_menu->Append(Minimal_Quit, "E&xit", "Exit the program");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "&File");
  SetMenuBar(menu_bar);

  CreateStatusBar();

  // child controls
  wxPanel *panel = new wxPanel(this);
  (void)new wxStaticText(panel, -1, "These controls remember their values!",
                         wxPoint(10, 10), wxSize(300, 20));
  m_text = new wxTextCtrl(panel, -1, "", wxPoint(10, 40), wxSize(300, 20));
  m_check = new wxCheckBox(panel, -1, "show welcome message box at startup",
                           wxPoint(10, 70), wxSize(300, 20));

  // restore the control's values from the config

  // NB: in this program, the config object is already created at this moment
  // because we had called Get() from MyApp::OnInit(). However, if you later
  // change the code and don't create it before this line, it won't break
  // anything - unlike if you manually create wxConfig object with Create()
  // or in any other way (then you must be sure to create it before using it!).
  wxConfigBase *pConfig = wxConfigBase::Get();

  // we could write Read("/Controls/Text") as well, it's just to show SetPath()
  pConfig->SetPath("/Controls");

  m_text->SetValue(pConfig->Read("Text", ""));
  m_check->SetValue(pConfig->Read("Check", 1l) != 0);

  // SetPath() understands ".."
  pConfig->SetPath("../MainFrame");

  // restore frame position and size
  int x = pConfig->Read("x", 50),
      y = pConfig->Read("y", 50),
      w = pConfig->Read("w", 350),
      h = pConfig->Read("h", 200);
  Move(x, y);
  SetClientSize(w, h);
}

void MyFrame::OnQuit(wxCommandEvent&)
{
  Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent&)
{
  wxMessageBox("wxConfig demo\n© Vadim Zeitlin 1998", "About",
               wxICON_INFORMATION | wxOK);
}

void MyFrame::OnDelete(wxCommandEvent&)
{
  // VZ: it seems that DeleteAll() wreaks havoc on NT. Disabled until I
  // investigate it further, do _not_ compile this code in meanwhile!
#if 0
  if ( wxConfigBase::Get()->DeleteAll() ) {
    wxLogMessage("Config file/registry key successfully deleted.");

    delete wxConfigBase::Set((wxConfigBase *) NULL);
    wxConfigBase::DontCreateOnDemand();
  }
  else
#endif // 0
    wxLogError("Deleting config file/registry key failed.");
}

MyFrame::~MyFrame()
{
  // save the control's values to the config
  wxConfigBase *pConfig = wxConfigBase::Get();
  if ( pConfig == NULL )
    return;
  pConfig->Write("/Controls/Text", m_text->GetValue());
  pConfig->Write("/Controls/Check", m_check->GetValue());

  // save the frame position
  int x, y, w, h;
  GetClientSize(&w, &h);
  GetPosition(&x, &y);
  pConfig->Write("/MainFrame/x", (long) x);
  pConfig->Write("/MainFrame/y", (long) y);
  pConfig->Write("/MainFrame/w", (long) w);
  pConfig->Write("/MainFrame/h", (long) h);
}

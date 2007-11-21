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

private:
  wxTextCtrl *m_text;
  wxCheckBox *m_check;

  DECLARE_EVENT_TABLE()
};

enum
{
  ConfTest_Quit,
  ConfTest_About,
  ConfTest_Delete
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(ConfTest_Quit, MyFrame::OnQuit)
  EVT_MENU(ConfTest_About, MyFrame::OnAbout)
  EVT_MENU(ConfTest_Delete, MyFrame::OnDelete)
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
  SetVendorName(_T("wxWidgets"));
  SetAppName(_T("conftest")); // not needed, it's the default value

  wxConfigBase *pConfig = wxConfigBase::Get();

  // uncomment this to force writing back of the defaults for all values
  // if they're not present in the config - this can give the user an idea
  // of all possible settings for this program
  pConfig->SetRecordDefaults();

  // or you could also write something like this:
  //  wxFileConfig *pConfig = new wxFileConfig("conftest");
  //  wxConfigBase::Set(pConfig);
  // where you can also specify the file names explicitly if you wish.
  // Of course, calling Set() is optional and you only must do it if
  // you want to later retrieve this pointer with Get().

  // create the main program window
  MyFrame *frame = new MyFrame;
  frame->Show(true);
  SetTopWindow(frame);

  // use our config object...
  if ( pConfig->Read(_T("/Controls/Check"), 1l) != 0 ) {
      wxMessageBox(_T("You can disable this message box by unchecking\n")
                   _T("the checkbox in the main window (of course, a real\n")
                   _T("program would have a checkbox right here but we\n")
                 _T("keep it simple)"), _T("Welcome to wxConfig demo"),
                 wxICON_INFORMATION | wxOK);
  }

  return true;
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
       : wxFrame((wxFrame *) NULL, wxID_ANY, _T("wxConfig Demo"))
{
  // menu
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(ConfTest_Delete, _T("&Delete"), _T("Delete config file"));
  file_menu->AppendSeparator();
  file_menu->Append(ConfTest_About, _T("&About\tF1"), _T("About this sample"));
  file_menu->AppendSeparator();
  file_menu->Append(ConfTest_Quit, _T("E&xit\tAlt-X"), _T("Exit the program"));
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, _T("&File"));
  SetMenuBar(menu_bar);

#if wxUSE_STATUSBAR
  CreateStatusBar();
#endif // wxUSE_STATUSBAR

  // child controls
  wxPanel *panel = new wxPanel(this);
  (void)new wxStaticText(panel, wxID_ANY, _T("These controls remember their values!"),
                         wxPoint(10, 10), wxSize(300, 20));
  m_text = new wxTextCtrl(panel, wxID_ANY, _T(""), wxPoint(10, 40), wxSize(300, 20));
  m_check = new wxCheckBox(panel, wxID_ANY, _T("show welcome message box at startup"),
                           wxPoint(10, 70), wxSize(300, 20));

  // restore the control's values from the config

  // NB: in this program, the config object is already created at this moment
  // because we had called Get() from MyApp::OnInit(). However, if you later
  // change the code and don't create it before this line, it won't break
  // anything - unlike if you manually create wxConfig object with Create()
  // or in any other way (then you must be sure to create it before using it!).
  wxConfigBase *pConfig = wxConfigBase::Get();

  // we could write Read("/Controls/Text") as well, it's just to show SetPath()
  pConfig->SetPath(_T("/Controls"));

  m_text->SetValue(pConfig->Read(_T("Text"), _T("")));
  m_check->SetValue(pConfig->Read(_T("Check"), 1l) != 0);

  // SetPath() understands ".."
  pConfig->SetPath(_T("../MainFrame"));

  // restore frame position and size
  int x = pConfig->Read(_T("x"), 50),
      y = pConfig->Read(_T("y"), 50),
      w = pConfig->Read(_T("w"), 350),
      h = pConfig->Read(_T("h"), 200);
  Move(x, y);
  SetClientSize(w, h);

  pConfig->SetPath(_T("/"));
  wxString s;
  if ( pConfig->Read(_T("TestValue"), &s) )
  {
      wxLogStatus(this, wxT("TestValue from config is '%s'"), s.c_str());
  }
  else
  {
      wxLogStatus(this, wxT("TestValue not found in the config"));
  }
}

void MyFrame::OnQuit(wxCommandEvent&)
{
  Close(true);
}

void MyFrame::OnAbout(wxCommandEvent&)
{
  wxMessageBox(_T("wxConfig demo\n(c) 1998-2001 Vadim Zeitlin"), _T("About"),
               wxICON_INFORMATION | wxOK);
}

void MyFrame::OnDelete(wxCommandEvent&)
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    if ( pConfig == NULL )
    {
        wxLogError(_T("No config to delete!"));
        return;
    }

    if ( pConfig->DeleteAll() )
    {
        wxLogMessage(_T("Config file/registry key successfully deleted."));

        delete wxConfigBase::Set(NULL);
        wxConfigBase::DontCreateOnDemand();
    }
    else
    {
        wxLogError(_T("Deleting config file/registry key failed."));
    }
}

MyFrame::~MyFrame()
{
  wxConfigBase *pConfig = wxConfigBase::Get();
  if ( pConfig == NULL )
    return;

  // save the control's values to the config
  pConfig->Write(_T("/Controls/Text"), m_text->GetValue());
  pConfig->Write(_T("/Controls/Check"), m_check->GetValue());

  // save the frame position
  int x, y, w, h;
  GetClientSize(&w, &h);
  GetPosition(&x, &y);
  pConfig->Write(_T("/MainFrame/x"), (long) x);
  pConfig->Write(_T("/MainFrame/y"), (long) y);
  pConfig->Write(_T("/MainFrame/w"), (long) w);
  pConfig->Write(_T("/MainFrame/h"), (long) h);

  pConfig->Write(_T("/TestValue"), wxT("A test value"));
}


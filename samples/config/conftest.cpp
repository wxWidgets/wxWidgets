///////////////////////////////////////////////////////////////////////////////
// Name:        conftest.cpp
// Purpose:     demo of wxConfig and related classes
// Author:      Vadim Zeitlin
// Created:     03.08.98
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
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

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------

class MyApp: public wxApp
{
public:
  // implement base class virtuals
  virtual bool OnInit() override;
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

    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
  EVT_MENU(wxID_DELETE, MyFrame::OnDelete)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// application
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // We can either use wxConfig "create-on-demand" feature, which creates the
    // config object when it's used for the first time or create it explicitly.
    //
    // On demand creation has the advantage of not creating wxConfig at all if
    // it is not needed, and so is recommended in general, but because we do
    // always use it here, we can just as well create it directly, as this
    // allows to customize its creation more easily.

    // application and vendor name are used by wxConfig to construct the name
    // of the config file/registry key and must be set before the first call
    // to Get() if you want to override the default values (the application
    // name is the name of the executable and the vendor name is the same)
    SetVendorName("wxWidgets");
    SetAppName("conftest"); // not needed, it's the default value

    wxConfigBase* pConfig;
    if ( wxGetEnv("WX_CONFIG_CREATE_ON_DEMAND", nullptr) )
    {
        pConfig = wxConfig::Get();
    }
    else
    {
        pConfig = new wxConfig
                      (
                        GetAppName(),
                        GetVendorName(),
                        wxString{}, // use default local file name
                        wxString{}, // use default global file name
                        wxCONFIG_USE_LOCAL_FILE
                        | wxCONFIG_USE_GLOBAL_FILE
                        //
                        // When creating the config object explicitly, it's
                        // easier to customize the flags, e.g. you could
                        // uncomment the line below to put the local config
                        // file in a subdirectory.
                        //
                        //| wxCONFIG_USE_SUBDIR
                      );

        // Set it as the global config object: as usual, this also gives its
        // ownership to wxWidgets, which will delete it.
        wxConfig::Set(pConfig);
    }

    // uncomment this to force writing back of the defaults for all values
    // if they're not present in the config - this can give the user an idea
    // of all possible settings for this program
    pConfig->SetRecordDefaults();

    // create the main program window
    MyFrame *frame = new MyFrame;
    frame->Show(true);

    // use our config object...
    if ( pConfig->Read("/Controls/Check", 1l) != 0 ) {
        wxMessageBox("You can disable this message box by unchecking\n"
                    "the checkbox in the main window (of course, a real\n"
                    "program would have a checkbox right here but we\n"
                    "keep it simple)", "Welcome to wxConfig demo",
                    wxICON_INFORMATION | wxOK);
    }

    return true;
}

// ----------------------------------------------------------------------------
// frame
// ----------------------------------------------------------------------------

// main frame ctor
MyFrame::MyFrame()
       : wxFrame(nullptr, wxID_ANY, "wxConfig Demo")
{
    SetIcon(wxICON(sample));

    // menu
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(wxID_DELETE, "&Delete", "Delete config file");
    file_menu->AppendSeparator();
    file_menu->Append(wxID_ABOUT, "&About\tF1", "About this sample");
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, "E&xit\tAlt-X", "Exit the program");
    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, "&File");
    SetMenuBar(menu_bar);

#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif // wxUSE_STATUSBAR

    // child controls
    wxPanel *panel = new wxPanel(this);
    wxStaticText* st = new wxStaticText(panel, wxID_ANY, "These controls remember their values!");
    m_text = new wxTextCtrl(panel, wxID_ANY);
    m_check = new wxCheckBox(panel, wxID_ANY, "show welcome message box at startup");

    // put everything in a sizer
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(st, wxSizerFlags().Border(wxLEFT|wxBOTTOM|wxTOP, 10));
    sizer->Add(m_text, wxSizerFlags().Border(wxLEFT|wxBOTTOM|wxRIGHT, 10).Expand());
    sizer->Add(m_check, wxSizerFlags().Border(wxLEFT, 10));
    panel->SetSizer(sizer);

    // restore the control's values from the config
    wxConfigBase *pConfig = wxConfig::Get();

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

    pConfig->SetPath("/");
    wxString s;
    if ( pConfig->Read("TestValue", &s) )
    {
        wxLogStatus(this, "TestValue from config is '%s'", s);
    }
    else
    {
        wxLogStatus(this, "TestValue not found in the config");
    }
}

void MyFrame::OnQuit(wxCommandEvent&)
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent&)
{
    wxMessageBox("wxConfig demo\n(c) 1998-2001 Vadim Zeitlin", "About",
                 wxICON_INFORMATION | wxOK);
}

void MyFrame::OnDelete(wxCommandEvent&)
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    if ( pConfig == nullptr )
    {
        wxLogError("No config to delete!");
        return;
    }

    if ( pConfig->DeleteAll() )
    {
        wxLogMessage("Config file/registry key successfully deleted.");

        delete wxConfigBase::Set(nullptr);
        wxConfigBase::DontCreateOnDemand();
    }
    else
    {
        wxLogError("Deleting config file/registry key failed.");
    }
}

MyFrame::~MyFrame()
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    if ( pConfig == nullptr )
        return;

    // save the control's values to the config
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

    pConfig->Write("/TestValue", "A test value");
}


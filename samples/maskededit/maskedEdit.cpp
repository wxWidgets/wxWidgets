/////////////////////////////////////////////////////////////////////////////
// Name:        maskededit.cpp
// Purpose:     MaskedEdit  sample
// Author:      Julien Weinzorn
// Modified by:
// Created:     04/01/98
// RCS-ID:      $.???????$
// Copyright:   (c) Julien Weinzorn
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
 
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
 
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources and even
// though we could still include the XPM here it would be unused)
#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
#endif

#include "wx/notebook.h"


// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);

    void CreateFirstPage(wxPanel* pan);
    void CreateSecondPage(wxPanel* pan);
    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Minimal_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    MyFrame *frame = new MyFrame("Example of MaskedEdit");

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About...\tF1", "Show about dialog");

    fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWidgets!");
#endif // wxUSE_STATUSBAR
   
    //Create the center
    wxBoxSizer* globalSizer = new wxBoxSizer(wxVERTICAL);

    wxNotebook* tab = new wxNotebook(this, wxID_ANY);

    wxPanel* firstPagePanel = new wxPanel(tab);
    CreateFirstPage(firstPagePanel);
    tab->AddPage(firstPagePanel, wxT("Basic example"));

    wxPanel* secondPagePanel = new wxPanel(tab);
    CreateSecondPage(secondPagePanel);
    tab->AddPage(secondPagePanel, wxT("User choice example"));


    globalSizer->Add(tab,wxALL|wxEXPAND );
  
    SetSizer(globalSizer);
    globalSizer->SetSizeHints(this);
}

void MyFrame::CreateFirstPage(wxPanel* pan)
{
    wxGridSizer* gridSizer = new wxGridSizer(4);

    wxStaticText* ipText     = new wxStaticText(pan, wxID_ANY, wxT("Ip address"));
    wxStaticText* ipMask     = new wxStaticText(pan, wxID_ANY, wxT("###.###.###.###")); 
    wxStaticText* ipFormat   = new wxStaticText(pan, wxID_ANY, wxT("F")); 
    wxTextCtrl*   ipCtrl     = new wxTextCtrl(pan, wxID_ANY); 
    wxMaskedEdit ipMaskEdit(wxT("###.###.###.###"), wxT("F"));

    ipCtrl->SetMask(ipMaskEdit);
    ipCtrl->ChangeValue(ipMaskEdit.GetEmptyMask());

    gridSizer->Add(ipText, wxALL|wxEXPAND);
    gridSizer->Add(ipMask, wxALL|wxEXPAND);
    gridSizer->Add(ipFormat, wxALL|wxEXPAND);
    gridSizer->Add(ipCtrl, wxALL|wxEXPAND);

    wxStaticText* phoneText     = new wxStaticText(pan, wxID_ANY, wxT("Phone number"));
    wxStaticText* phoneMask     = new wxStaticText(pan, wxID_ANY, wxT("(###) ###-####")); 
    wxStaticText* phoneFormat   = new wxStaticText(pan, wxID_ANY, wxT("F")); 
    wxTextCtrl*   phoneCtrl     = new wxTextCtrl(pan, wxID_ANY); 
    wxMaskedEdit phoneMaskEdit(wxT("(###) ###-####"), wxT("F"));

    phoneCtrl->SetMask(phoneMaskEdit);
    phoneCtrl->ChangeValue(phoneMaskEdit.GetEmptyMask());

    gridSizer->Add(phoneText, wxALL|wxEXPAND);
    gridSizer->Add(phoneMask, wxALL|wxEXPAND);
    gridSizer->Add(phoneFormat, wxALL|wxEXPAND);
    gridSizer->Add(phoneCtrl, wxALL|wxEXPAND);


    wxStaticText* fullNameText     = new wxStaticText(pan, wxID_ANY, wxT("Full Name"));
    wxStaticText* fullNameMask     = new wxStaticText(pan, wxID_ANY, wxT("C{14}")); 
    wxStaticText* fullNameFormat   = new wxStaticText(pan, wxID_ANY, wxT("F_")); 
    wxTextCtrl*   fullNameCtrl     = new wxTextCtrl(pan, wxID_ANY); 
    wxMaskedEdit fullNameMaskEdit(wxT("C{14}"), wxT("F_"));

    fullNameCtrl->SetMask(fullNameMaskEdit);
    fullNameCtrl->ChangeValue(fullNameMaskEdit.GetEmptyMask());

    gridSizer->Add(fullNameText, wxALL|wxEXPAND);
    gridSizer->Add(fullNameMask, wxALL|wxEXPAND);
    gridSizer->Add(fullNameFormat, wxALL|wxEXPAND);
    gridSizer->Add(fullNameCtrl, wxALL|wxEXPAND);

    wxStaticText* intText     = new wxStaticText(pan, wxID_ANY, wxT("Integer"));
    wxStaticText* intMask     = new wxStaticText(pan, wxID_ANY, wxT("#{6}")); 
    wxStaticText* intFormat   = new wxStaticText(pan, wxID_ANY, wxT("F-_")); 
    wxTextCtrl*   intCtrl     = new wxTextCtrl(pan, wxID_ANY); 
    wxMaskedEdit intMaskEdit(wxT("#{6}"), wxT("F-_"));

    intCtrl->SetMask(intMaskEdit);
    intCtrl->ChangeValue(intMaskEdit.GetEmptyMask());

    gridSizer->Add(intText, wxALL|wxEXPAND);
    gridSizer->Add(intMask, wxALL|wxEXPAND);
    gridSizer->Add(intFormat, wxALL|wxEXPAND);
    gridSizer->Add(intCtrl, wxALL|wxEXPAND);

    wxStaticText* floatText     = new wxStaticText(pan, wxID_ANY, wxT("Floating number"));
    wxStaticText* floatMask     = new wxStaticText(pan, wxID_ANY, wxT("#{6}.#{2}")); 
    wxStaticText* floatFormat   = new wxStaticText(pan, wxID_ANY, wxT("F-_")); 
    wxTextCtrl*   floatCtrl     = new wxTextCtrl(pan, wxID_ANY); 
    wxMaskedEdit floatMaskEdit(wxT("#{6}.#{2}"), wxT("F-_"));

    floatCtrl->SetMask(floatMaskEdit);
    floatCtrl->ChangeValue(floatMaskEdit.GetEmptyMask());

    gridSizer->Add(floatText, wxALL|wxEXPAND);
    gridSizer->Add(floatMask, wxALL|wxEXPAND);
    gridSizer->Add(floatFormat, wxALL|wxEXPAND);
    gridSizer->Add(floatCtrl, wxALL|wxEXPAND);


    pan->SetSizer(gridSizer);
    gridSizer->SetSizeHints(pan);
}


void MyFrame::CreateSecondPage(wxPanel* pan)
{
    wxGridSizer* gridSizer = new wxGridSizer(4);

    wxStaticText* zipText     = new wxStaticText(pan, wxID_ANY, wxT("French zip"));
    wxStaticText* zipMask     = new wxStaticText(pan, wxID_ANY, wxT("#####")); 
    wxStaticText* zipFormat   = new wxStaticText(pan, wxID_ANY, wxT("F")); 
    wxTextCtrl*   zipCtrl     = new wxTextCtrl(pan, wxID_ANY); 


    wxArrayString choice;
    
    choice.Add("57990");
    choice.Add("67000");
    choice.Add("75000");
    choice.Add("69000");
    
    wxMaskedEdit  zipMaskEdit(wxT("#####"), wxT("F"));
    zipMaskEdit.AddChoices(choice);

    zipCtrl->SetMask(zipMaskEdit);
    zipCtrl->ChangeValue(zipMaskEdit.GetEmptyMask());

    gridSizer->Add(zipText, wxALL|wxEXPAND);
    gridSizer->Add(zipMask, wxALL|wxEXPAND);
    gridSizer->Add(zipFormat, wxALL|wxEXPAND);
    gridSizer->Add(zipCtrl, wxALL|wxEXPAND);
 
    wxStaticText* houreText    = new wxStaticText(pan, wxID_ANY, wxT("hour format HH:MM"));
    wxStaticText* houreMask    = new wxStaticText(pan, wxID_ANY, wxT("##|:|##"));
    wxStaticText* houreFormat  = new wxStaticText(pan, wxID_ANY, wxT("F"));
    wxTextCtrl*   houreCtrl    = new wxTextCtrl(pan, wxID_ANY);
    wxMaskedEdit  houreMaskEdit(wxT("##|:|##"), wxT("F"));

    
    choice.Clear();

    choice.Add(wxT("00"));
    choice.Add(wxT("01"));
    choice.Add(wxT("02"));
    choice.Add(wxT("03"));
    choice.Add(wxT("04"));
    choice.Add(wxT("05"));
    
    houreMaskEdit.AddChoices(0, choice);

    houreMaskEdit.AddChoices(2, choice);

    

    houreCtrl->SetMask(houreMaskEdit);

    houreCtrl->ChangeValue(houreMaskEdit.GetEmptyMask());

    gridSizer->Add(houreText, wxALL|wxEXPAND);
    gridSizer->Add(houreMask, wxALL|wxEXPAND);
    gridSizer->Add(houreFormat, wxALL|wxEXPAND);
    gridSizer->Add(houreCtrl, wxALL|wxEXPAND);
 
    pan->SetSizer(gridSizer);
    gridSizer->SetSizeHints(pan);
}
// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format
                 (
                    "Welcome to %s!\n"
                    "\n"
                    "This is the example of maskedEdit\n"
                    "running under %s.",
                    wxVERSION_STRING,
                    wxGetOsDescription()
                 ),
                 "About wxWidgets maskedEdit",
                 wxOK | wxICON_INFORMATION,
                 this);
}

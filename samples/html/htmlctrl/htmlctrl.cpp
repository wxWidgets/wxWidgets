/////////////////////////////////////////////////////////////////////////////
// Name:        htmlctrl.cpp
// Purpose:     HtmlCtrl sample
// Author:      Julian Smart / Kevin Ollivier
// Modified by:
// Created:     04/16/2004
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart / Kevin Ollivier
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

#include "wx/html/webkit.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

enum {
    ID_BACK = wxID_HIGHEST + 1,
    ID_NEXT = wxID_HIGHEST + 2,
    ID_RELOAD = wxID_HIGHEST + 3,
    ID_URLLIST = wxID_HIGHEST + 4,
    ID_STOP = wxID_HIGHEST + 5,
    ID_WEBKIT = wxID_HIGHEST + 6,
    ID_VIEW_SOURCE = wxID_HIGHEST + 7,
    ID_OPEN = wxID_HIGHEST + 8,
    ID_SAVE = wxID_HIGHEST + 9,
    ID_SET_SOURCE = wxID_HIGHEST + 10
    };

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
    void OnBackButton(wxCommandEvent& myEvent);
    void OnNextButton(wxCommandEvent& myEvent);
    void OnURLEnter(wxCommandEvent& myEvent);
    void OnStopButton(wxCommandEvent& myEvent);
    void OnReloadButton(wxCommandEvent& myEvent);
    void OnViewSource(wxCommandEvent& myEvent);
    void OnSetSource(wxCommandEvent& myEvent);
    void OnStateChanged(wxWebKitStateChangedEvent& myEvent);
    wxWebKitCtrl* mySafari;
    wxTextCtrl* urlText;
private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_BUTTON(ID_BACK, MyFrame::OnBackButton)
    EVT_BUTTON(ID_NEXT, MyFrame::OnNextButton)
    EVT_BUTTON(ID_STOP, MyFrame::OnStopButton)
    EVT_BUTTON(ID_RELOAD, MyFrame::OnReloadButton)
    EVT_MENU(ID_VIEW_SOURCE, MyFrame::OnViewSource)
    EVT_MENU(ID_SET_SOURCE, MyFrame::OnSetSource)
    EVT_TEXT_ENTER(ID_URLLIST, MyFrame::OnURLEnter)
    EVT_WEBKIT_STATE_CHANGED(MyFrame::OnStateChanged)
    //EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    //EVT_MENU(Minimal_About, MyFrame::OnAbout)
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
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    MyFrame *frame = new MyFrame(_T("wxWebKit Sample"));

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
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(500,500))
{
    wxMenuBar* myBar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(ID_OPEN, _("&Open"));
    fileMenu->Append(ID_SAVE, _("&Save"));
    myBar->Append(fileMenu, _("&File"));
    
    wxMenu* editMenu = new wxMenu;
    editMenu->Append(ID_SET_SOURCE, _("Set Page Source"));
    myBar->Append(editMenu, _("&Edit"));
    
    //wxMenu* viewMenu = new wxMenu(_("View"));
    //viewMenu->Append(ID_VIEW_SOURCE, _("View Source"));
    //myBar->Append(viewMenu, _("View"));
    
    SetMenuBar(myBar);

    wxToolBar* myToolbar = CreateToolBar();
    wxButton* btnBack = new wxButton(myToolbar, ID_BACK, _("Back"));
    myToolbar->AddControl(btnBack);
    myToolbar->AddSeparator();
    wxButton* btnNext = new wxButton(myToolbar, ID_NEXT, _("Next"));
    myToolbar->AddControl(btnNext);
    myToolbar->AddSeparator();
    wxButton* btnStop = new wxButton(myToolbar, ID_STOP, _("Stop"));
    myToolbar->AddControl(btnStop);
    myToolbar->AddSeparator();
    wxButton* btnReload = new wxButton(myToolbar, ID_RELOAD, _("Reload"));
    myToolbar->AddControl(btnReload);
    myToolbar->AddSeparator();
    urlText = new wxTextCtrl(myToolbar, ID_URLLIST, _T("http://www.wxwidgets.org"), wxDefaultPosition, wxSize(220, -1), wxTE_PROCESS_ENTER);
    myToolbar->AddControl(urlText);
    myToolbar->AddSeparator();
    myToolbar->Realize();

    // Testing wxWebKitCtrl inside a panel
#if 1
    wxPanel* panel = new wxPanel(this, wxID_ANY);

    wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(boxSizer);

    mySafari = new wxWebKitCtrl(panel, ID_WEBKIT, _T("http://www.wxwidgets.org"), wxDefaultPosition, wxSize(200, 200));

    boxSizer->Add(mySafari, 1, wxEXPAND);

    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(frameSizer);
    frameSizer->Add(panel, 1, wxEXPAND);
#else
    mySafari = new wxWebKitCtrl(this, ID_WEBKIT, _T("http://www.wxwidgets.org"), wxDefaultPosition, wxSize(200, 200));
#endif
    
#if wxUSE_STATUSBAR
    CreateStatusBar(2);
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnBackButton(wxCommandEvent& myEvent){
    if (mySafari->CanGoBack())
        mySafari->GoBack();
}

void MyFrame::OnNextButton(wxCommandEvent& myEvent){
    if (mySafari->CanGoForward())
        mySafari->GoForward();
}

void MyFrame::OnStopButton(wxCommandEvent& myEvent){
        mySafari->Stop();
}

void MyFrame::OnReloadButton(wxCommandEvent& myEvent){
    mySafari->Reload();
}

void MyFrame::OnURLEnter(wxCommandEvent& myEvent){
    mySafari->LoadURL(urlText->GetValue());
}

void MyFrame::OnStateChanged(wxWebKitStateChangedEvent& myEvent){
    if (GetStatusBar() != NULL){
        if (myEvent.GetState() == wxWEBKIT_STATE_NEGOTIATING){
            GetStatusBar()->SetStatusText(_("Contacting ") + myEvent.GetURL());
            urlText->SetValue(myEvent.GetURL());
        }
        else if (myEvent.GetState() == wxWEBKIT_STATE_TRANSFERRING){
            GetStatusBar()->SetStatusText(_("Loading ") + myEvent.GetURL());
        }
        else if (myEvent.GetState() == wxWEBKIT_STATE_STOP){
            GetStatusBar()->SetStatusText(_("Load complete."));
            SetTitle(mySafari->GetTitle());
        }
        else if (myEvent.GetState() == wxWEBKIT_STATE_FAILED){
            GetStatusBar()->SetStatusText(_("Failed to load ") + myEvent.GetURL());
        }
    }

}

void MyFrame::OnViewSource(wxCommandEvent& myEvent){
    if (mySafari->CanGetPageSource())
        wxMessageBox(mySafari->GetPageSource());
}

void MyFrame::OnSetSource(wxCommandEvent& myEvent){
    if (mySafari){
        wxString myText = wxT("<HTML><HEAD></HEAD><BODY><P>Hello world!</P></BODY></HTML>");
        mySafari->SetPageSource(myText);
    }
}

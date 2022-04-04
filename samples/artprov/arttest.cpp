/////////////////////////////////////////////////////////////////////////////
// Name:        arttest.cpp
// Purpose:     wxArtProvider sample
// Author:      Vaclav Slavik
// Modified by:
// Created:     2002/03/25
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

#include "wx/artprov.h"
#include "artbrows.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    virtual bool OnInit() wxOVERRIDE;
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
            long style = wxDEFAULT_FRAME_STYLE);

private:
    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
#if wxUSE_LOG
    void OnLogs(wxCommandEvent& event);
#endif // wxUSE_LOG
    void OnBrowser(wxCommandEvent& event);
    void OnPlugProvider(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    ID_Quit = wxID_EXIT,
    ID_Logs = wxID_HIGHEST+1,
    ID_Browser,
    ID_PlugProvider
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_Quit,         MyFrame::OnQuit)
#if wxUSE_LOG
    EVT_MENU(ID_Logs,         MyFrame::OnLogs)
#endif // wxUSE_LOG
    EVT_MENU(wxID_ABOUT,      MyFrame::OnAbout)
    EVT_MENU(ID_Browser,      MyFrame::OnBrowser)
    EVT_MENU(ID_PlugProvider, MyFrame::OnPlugProvider)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);

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
    MyFrame *frame = new MyFrame("wxArtProvider sample",
                                 wxPoint(50, 50), wxSize(450, 340));
    frame->Show(true);
    return true;
}

// ----------------------------------------------------------------------------
// custom art provider
// ----------------------------------------------------------------------------

class MyArtProvider : public wxArtProvider
{
protected:
    virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client,
                                  const wxSize& size) wxOVERRIDE;
};

#include "info.xpm"
#include "error.xpm"
#include "warning.xpm"
#include "question.xpm"

wxBitmap MyArtProvider::CreateBitmap(const wxArtID& id,
                                     const wxArtClient& client,
                                     const wxSize& WXUNUSED(size))
{
    if ( client == wxART_MESSAGE_BOX )
    {
        if ( id == wxART_INFORMATION )
            return wxBitmap(info_xpm);
        if ( id == wxART_ERROR )
            return wxBitmap(error_xpm);
        if ( id == wxART_WARNING )
            return wxBitmap(warning_xpm);
        if ( id == wxART_QUESTION )
            return wxBitmap(question_xpm);
    }
    return wxNullBitmap;
}



// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
       : wxFrame(NULL, wxID_ANY, title, pos, size, style)
{
    SetIcon(wxICON(sample));

    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, "&About\tF1", "Show about dialog");

    menuFile->AppendCheckItem(ID_PlugProvider, "&Plug-in art provider\tCtrl-P", "Enable custom art provider");
    menuFile->AppendSeparator();

#if wxUSE_LOG
    menuFile->Append(ID_Logs, "&Logging test\tCtrl-L", "Show some logging output");
#endif // wxUSE_LOG
    menuFile->Append(ID_Browser, "&Resources browser\tCtrl-R", "Browse all available icons");
    menuFile->AppendSeparator();

    menuFile->Append(ID_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

#if wxUSE_LOG
void MyFrame::OnLogs(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Some information.");
    wxLogError("This is an error.");
    wxLogWarning("A warning.");
    wxLogError("Yet another error.");
    wxLog::GetActiveTarget()->Flush();
    wxLogMessage("Check/uncheck 'File/Plug-in art provider' and try again.");
}
#endif // wxUSE_LOG

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( "This is the about dialog of wxArtProvider sample.\n"
                "Welcome to %s", wxVERSION_STRING);

    wxMessageBox(msg, "About wxArtProvider sample",
        wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnBrowser(wxCommandEvent& WXUNUSED(event))
{
    wxArtBrowserDialog dlg(this);
    dlg.ShowModal();
}

void MyFrame::OnPlugProvider(wxCommandEvent& event)
{
    if ( event.IsChecked() )
        wxArtProvider::Push(new MyArtProvider);
    else
        wxArtProvider::Pop();
}

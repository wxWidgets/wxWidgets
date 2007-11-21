/////////////////////////////////////////////////////////////////////////////
// Name:        arttest.cpp
// Purpose:     wxArtProvider sample
// Author:      Vaclav Slavik
// Modified by:
// Created:     2002/03/25
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "mondrian.xpm"
#endif

#include "wx/artprov.h"
#include "artbrows.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
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

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    ID_Quit = wxID_HIGHEST,
    ID_Logs,
    ID_Browser,
    ID_PlugProvider
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_Quit,         MyFrame::OnQuit)
#if wxUSE_LOG
    EVT_MENU(ID_Logs,         MyFrame::OnLogs)
#endif // wxUSE_LOG
    EVT_MENU(wxID_ABOUT,      MyFrame::OnAbout)
    EVT_MENU(ID_Browser,      MyFrame::OnBrowser)
    EVT_MENU(ID_PlugProvider, MyFrame::OnPlugProvider)
END_EVENT_TABLE()

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
    // create the main application window
    MyFrame *frame = new MyFrame(_T("wxArtProvider sample"),
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
                                  const wxSize& size);
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
    SetIcon(wxICON(mondrian));

    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, _T("&About...\tF1"), _T("Show about dialog"));

    menuFile->AppendCheckItem(ID_PlugProvider, _T("&Plug-in art provider"), _T("Enable custom art provider"));
    menuFile->AppendSeparator();

#if wxUSE_LOG
    menuFile->Append(ID_Logs, _T("&Logging test"), _T("Show some logging output"));
#endif // wxUSE_LOG
    menuFile->Append(ID_Browser, _T("&Resources browser"), _T("Browse all available icons"));
    menuFile->AppendSeparator();

    menuFile->Append(ID_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

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
    wxLogMessage(_T("Some information."));
    wxLogError(_T("This is an error."));
    wxLogWarning(_T("A warning."));
    wxLogError(_T("Yet another error."));
    wxLog::GetActiveTarget()->Flush();
    wxLogMessage(_T("Check/uncheck 'File/Plug-in art provider' and try again."));
}
#endif // wxUSE_LOG

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the about dialog of wxArtProvider sample.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, _T("About wxArtProvider sample"),
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

/////////////////////////////////////////////////////////////////////////////
// Name:        xmldemo.cpp
// Purpose:     XML resources sample
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifdef __GNUG__
    #pragma implementation "xmldemo.cpp"
    #pragma interface "xmldemo.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/image.h"
#endif

#include "wx/xml/xmlres.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------
// the application icon
#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "rc/appicon.xpm"
#endif

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
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnDlg1(wxCommandEvent& event);
    void OnDlg2(wxCommandEvent& event);

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(XMLID("menu_quit"),  MyFrame::OnQuit)
    EVT_MENU(XMLID("menu_about"), MyFrame::OnAbout)
    EVT_MENU(XMLID("menu_dlg1"), MyFrame::OnDlg1)
    EVT_MENU(XMLID("menu_dlg2"), MyFrame::OnDlg2)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
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
    wxImage::AddHandler(new wxGIFHandler);
    wxTheXmlResource->InitAllHandlers();
    wxTheXmlResource->Load("rc/resource.xml");

    MyFrame *frame = new MyFrame("XML resources demo",
                                 wxPoint(50, 50), wxSize(450, 340));
    frame->Show(TRUE);
    return TRUE;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    SetIcon(wxICON(appicon));

    SetMenuBar(wxTheXmlResource->LoadMenuBar("mainmenu"));
    SetToolBar(wxTheXmlResource->LoadToolBar(this, "toolbar"));
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the about dialog of XML resources demo.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, "About XML resources demo", wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnDlg1(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg;
    wxTheXmlResource->LoadDialog(&dlg, this, "dlg1");
    dlg.ShowModal();
}


void MyFrame::OnDlg2(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg;
    wxTheXmlResource->LoadDialog(&dlg, this, "dlg2");
    dlg.ShowModal();
}

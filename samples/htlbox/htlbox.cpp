/////////////////////////////////////////////////////////////////////////////
// Name:        htmllbox.cpp
// Purpose:     HtmlLbox wxWindows sample
// Author:      Vadim Zeitlin
// Modified by:
// Created:     31.05.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
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
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/log.h"
#endif

#include "wx/htmllbox.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources)
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
    #include "mondrian.xpm"
#endif

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
    MyFrame();
    virtual ~MyFrame();

    // event handlers
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnLboxSelect(wxCommandEvent& event)
    {
        wxLogMessage(_T("Listbox selection is now %d."), event.GetInt());
    }

    void OnLboxDClick(wxCommandEvent& event)
    {
        wxLogMessage(_T("Listbox item %d double clicked."), event.GetInt());
    }

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// to use wxHtmlListBox you must derive a new class from it as you must
// implement pure virtual OnGetItem()
class MyHtmlListBox : public wxHtmlListBox
{
public:
    MyHtmlListBox(wxWindow *parent) : wxHtmlListBox(parent)
    {
        SetItemCount(1000);
        SetSelection(10);
        SetMargins(5, 5);
    }

protected:
    virtual wxString OnGetItem(size_t n) const
    {
        int level = n % 6 + 1;
        return wxString::Format(_T("<h%d><font color=#%2x%2x%2x>")
                                _T("Item</font> <b>%lu</b>")
                                _T("</h%d>"),
                                level,
                                abs(n - 192) % 256,
                                abs(n - 256) % 256,
                                abs(n - 128) % 256,
                                (unsigned long)n, level);
    }

    virtual void OnDrawSeparator(wxDC& dc,
                                 wxRect& rect,
                                 size_t WXUNUSED(n)) const
    {
        dc.SetPen(*wxBLACK_DASHED_PEN);
        dc.DrawLine(rect.x, rect.y, rect.GetRight(), rect.y);
        dc.DrawLine(rect.x, rect.GetBottom(), rect.GetRight(), rect.GetBottom());
    }

};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    HtmlLbox_Quit = 1,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    HtmlLbox_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(HtmlLbox_Quit,  MyFrame::OnQuit)
    EVT_MENU(HtmlLbox_About, MyFrame::OnAbout)

    EVT_LISTBOX(wxID_ANY, MyFrame::OnLboxSelect)
    EVT_LISTBOX_DCLICK(wxID_ANY, MyFrame::OnLboxDClick)
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
    (new MyFrame())->Show();

    return TRUE;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame()
       : wxFrame(NULL, -1, _T("HtmlLbox wxWindows Sample"),
                 wxDefaultPosition, wxSize(400, 500))
{
    // set the frame icon
    SetIcon(wxICON(mondrian));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(HtmlLbox_About, _T("&About...\tF1"), _T("Show about dialog"));

    menuFile->Append(HtmlLbox_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWindows!"));
#endif // wxUSE_STATUSBAR

    // create the child controls
    MyHtmlListBox *hlbox = new MyHtmlListBox(this);
    wxTextCtrl *text = new wxTextCtrl(this, -1, _T(""),
                                      wxDefaultPosition, wxDefaultSize,
                                      wxTE_MULTILINE);
    delete wxLog::SetActiveTarget(new wxLogTextCtrl(text));

    // and lay them out
    wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(hlbox, 1, wxGROW);
    sizer->Add(text, 1, wxGROW);

    SetSizer(sizer);
}

MyFrame::~MyFrame()
{
    delete wxLog::SetActiveTarget(NULL);
}

// event handlers
void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("This sample shows wxHtmlListBox class.\n")
                 _T("\n")
                 _T("© 2003 Vadim Zeitlin"),
                 _T("About HtmlLbox"),
                 wxOK | wxICON_INFORMATION,
                 this);
}


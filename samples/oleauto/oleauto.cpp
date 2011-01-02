/////////////////////////////////////////////////////////////////////////////
// Name:        oleauto.cpp
// Purpose:     OLE Automation wxWidgets sample
// Author:      Julian Smart
// Modified by:
// Created:     08/12/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
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
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/msw/ole/automtn.h"

#ifndef __WXMSW__
#error "Sorry, this sample works under Windows only."
#endif

// ----------------------------------------------------------------------------
// ressources
// ----------------------------------------------------------------------------
// the application icon
#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
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
    void OnTest(wxCommandEvent& event);

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
    OleAuto_Quit = 1,
    OleAuto_About,
    OleAuto_Test,

    // controls start here (the numbers are, of course, arbitrary)
    OleAuto_Text = 1000
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(OleAuto_Quit,  MyFrame::OnQuit)
    EVT_MENU(OleAuto_About, MyFrame::OnAbout)
    EVT_MENU(OleAuto_Test, MyFrame::OnTest)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
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

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create the main application window
    MyFrame *frame = new MyFrame(wxT("OleAuto wxWidgets App"),
                                 wxPoint(50, 50), wxSize(450, 340));

    // Show it
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
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size)
{
    // set the frame icon
    SetIcon(wxICON(sample));

    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(OleAuto_Test, wxT("&Test Excel Automation..."));
    menuFile->Append(OleAuto_About, wxT("&About..."));
    menuFile->AppendSeparator();
    menuFile->Append(OleAuto_Quit, wxT("E&xit"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, wxT("&File"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(wxT("Welcome to wxWidgets!"));
#endif // wxUSE_STATUSBAR
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxT("This is an OLE Automation sample"),
                 wxT("About OleAuto"), wxOK | wxICON_INFORMATION, this);
}

/* Tests OLE automation by making the active Excel cell bold,
 * and changing the text.
 */
void MyFrame::OnTest(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxT("Excel will be started if it is not running after you have pressed OK button.")
        wxT("\nThe active cell should then say 'wxWidgets automation test!' in bold."),
        wxT("Excel start"));

    wxAutomationObject excelObject;
    if ( !excelObject.GetInstance(wxT("Excel.Application")) )
    {
        wxLogError(wxT("Could not create Excel object."));
        return;
    }

    // Ensure that Excel is visible
    if (!excelObject.PutProperty(wxT("Visible"), true))
    {
        wxLogError(wxT("Could not make Excel object visible"));
    }
    const wxVariant workbooksCountVariant = excelObject.GetProperty(wxT("Workbooks.Count"));
    if (workbooksCountVariant.IsNull())
    {
        wxLogError(wxT("Could not get workbooks count"));
        return;
    }
    const long workbooksCount = workbooksCountVariant;
    if (workbooksCount == 0)
    {
        const wxVariant workbook = excelObject.CallMethod(wxT("Workbooks.Add"));
        if (workbook.IsNull())
        {
            wxLogError(wxT("Could not create new Workbook"));
            return;
        }
    }

    if (!excelObject.PutProperty(wxT("ActiveCell.Value"), wxT("wxWidgets automation test!")))
    {
        wxLogError(wxT("Could not set active cell value."));
        return;
    }
    if (!excelObject.PutProperty(wxT("ActiveCell.Font.Bold"), wxVariant(true)) )
    {
        wxLogError(wxT("Could not put Bold property to active cell."));
        return;
    }
}

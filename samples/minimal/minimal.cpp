/*
    Test for menu swapping bug (?) with GTK 2.1.13
*/



/////////////////////////////////////////////////////////////////////////////
// Name:        minimal.cpp
// Purpose:     Minimal wxWindows sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
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
#ifdef __GNUG__
    #pragma implementation "minimal.cpp"
    #pragma interface "minimal.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
static wxMenuBar *bar[2];
static int current_bar;

// ----------------------------------------------------------------------------
// ressources
// ----------------------------------------------------------------------------
// the application icon
#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "mondrian.xpm"
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
    void OnSwapMenus(wxCommandEvent& event);
    void OnReplaceMenu(wxCommandEvent& event);
    void OnRemoveInsertMenu(wxCommandEvent& event);

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = 1,
    Minimal_About,
    Minimal_SwapMenus,
    Minimal_ReplaceMenu,
    Minimal_RemoveInsertMenu
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
    EVT_MENU(Minimal_SwapMenus, MyFrame::OnSwapMenus)
    EVT_MENU(Minimal_ReplaceMenu, MyFrame::OnReplaceMenu)
    EVT_MENU(Minimal_RemoveInsertMenu, MyFrame::OnRemoveInsertMenu)
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

wxString IntTowxString(int number)
{
  return(wxString(IntToString(number)));
}

wxMenu *GetFileMenu(int menu_number)
{
  wxMenu *menuFile = new wxMenu("", wxMENU_TEAROFF);
  menuFile->Append(Minimal_Quit, "E&xit" + IntTowxString(menu_number) +
   "\tAlt-X", "Quit this program");
  menuFile->Append(Minimal_SwapMenus, "&SwapMenus" + IntTowxString(menu_number)
   + "\tAlt-S", "Swap Menus");
  menuFile->Append(Minimal_ReplaceMenu, "&ReplaceMenu" +
   IntTowxString(menu_number) + "\tAlt-R", "Replace Menu");
  menuFile->Append(Minimal_RemoveInsertMenu, "&RemoveInsertMenu" +
   IntTowxString(menu_number) + "\tAlt-I", "Remove Then Insert Menu");

  return(menuFile);
}

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
    MyFrame *frame = new MyFrame("Minimal wxWindows App",
                                 wxPoint(50, 50), wxSize(450, 340));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(TRUE);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = Minimal_About;
#endif

    // set the frame icon
    SetIcon(wxICON(mondrian));

    bar[0] = new wxMenuBar();

    wxMenu *menuFile0 = GetFileMenu(0);

    wxMenu *helpMenu0 = new wxMenu;
    helpMenu0->Append(Minimal_About, "&About0...\tCtrl-A",
     "Show about dialog");

    bar[0]->Append(menuFile0, "&File0");
    bar[0]->Append(helpMenu0, "&Help0");


    bar[1] = new wxMenuBar();

    wxMenu *menuFile1 = GetFileMenu(1);

    wxMenu *helpMenu1 = new wxMenu;
    helpMenu1->Append(Minimal_About, "&About1...\tCtrl-A",
     "Show about dialog");

    bar[1]->Append(menuFile1, "&File1");
    bar[1]->Append(helpMenu1, "&Help1");

    current_bar = 1;

    // ... and attach this menu bar to the frame
    SetMenuBar(bar[current_bar]);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWindows!");
#endif // wxUSE_STATUSBAR
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
    msg.Printf( _T("This is the about dialog of minimal sample.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, "About Minimal", wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnSwapMenus(wxCommandEvent& WXUNUSED(event))
{
  // Change the menu set around
  current_bar = 1 - current_bar;
  SetMenuBar(bar[current_bar]);
}

void MyFrame::OnReplaceMenu(wxCommandEvent& WXUNUSED(event))
{
  wxMenuBar *curr_bar = bar[current_bar];
  wxMenu *menu = GetFileMenu(3);
  wxString title = "&File3";

  // Replace the first menu with the same thing
  int pos = 1;
  if (pos != wxNOT_FOUND)
  {
      curr_bar->Replace(pos, menu, title);
//    SetMenuBar(curr_bar);
  }
}


void MyFrame::OnRemoveInsertMenu(wxCommandEvent& WXUNUSED(event))
{
  wxMenuBar *curr_bar = bar[current_bar];
  wxMenu *menu = GetFileMenu(current_bar);
  wxString title = "&File3";

  // Remove the first menu then insert it back in
  int pos = 1; 
  if (pos != wxNOT_FOUND)
  {
    curr_bar->Remove(pos);
    if (curr_bar->GetMenuCount() != 0)
      curr_bar->Insert(pos, menu, title);
    else
      curr_bar->Append(menu, title);

    SetMenuBar(curr_bar);
  }
}




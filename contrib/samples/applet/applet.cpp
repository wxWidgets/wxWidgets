/****************************************************************************
*
*                       wxWindows HTML Applet Package
*
*               Copyright (C) 1991-2001 SciTech Software, Inc.
*                            All rights reserved.
*
*  ========================================================================
*
*    The contents of this file are subject to the wxWindows License
*    Version 3.0 (the "License"); you may not use this file except in
*    compliance with the License. You may obtain a copy of the License at
*    http://www.wxwindows.org/licence3.txt
*
*    Software distributed under the License is distributed on an
*    "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
*    implied. See the License for the specific language governing
*    rights and limitations under the License.
*
*  ========================================================================
*
* Language:     ANSI C++
* Environment:  Any
*
* Description:  Main wxApplet sample program
*
****************************************************************************/

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/wx.h"
#include "wx/applet/window.h"
#include "applet.h"

/*---------------------------- Global variables ---------------------------*/

// Define the event tables for handling application frame events
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,      MyFrame::OnQuit)
    EVT_MENU(Minimal_About,     MyFrame::OnAbout)
    EVT_MENU(Minimal_Back,      MyFrame::OnBack)
    EVT_MENU(Minimal_Forward,   MyFrame::OnForward)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
title   - Title for the frame window
pos     - Position to place to frame window
size    - Size of the frame window

REMARKS:
Application frame window constructor
****************************************************************************/
MyFrame::MyFrame(
    const wxString& title,
    const wxPoint& pos,
    const wxSize& size)
    : wxFrame(NULL, -1, title, pos, size)
{
    // Create a menu bar
    wxMenu *menuFile = new wxMenu;
    wxMenu *menuNav = new wxMenu;
    menuFile->Append(Minimal_Quit, "E&xit");
    menuNav->Append(Minimal_Back, "Go &back");
    menuNav->Append(Minimal_Forward, "Go &forward");

    // Now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuNav, "&Navigate");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
    CreateStatusBar(2);

    // Create the HTML window
    html = new wxHtmlAppletWindow(this);
    html->SetRelatedFrame(this, "wxApplet Demo: '%s'");
    html->SetRelatedStatusBar(1);
    html->LoadPage("index.html");
}

/****************************************************************************
REMARKS:
Event handler for the 'Exit' menu item
****************************************************************************/
void MyFrame::OnQuit(
    wxCommandEvent&)
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

/****************************************************************************
REMARKS:
Event handler for the 'About' menu item
****************************************************************************/
void MyFrame::OnAbout(
    wxCommandEvent&)
{
    // TODO: Bring up and about html page!
}

/****************************************************************************
REMARKS:
Event handler for the 'Go back' menu item
****************************************************************************/
void MyFrame::OnBack(
    wxCommandEvent&)
{
    if (!html -> HistoryBack())
        wxMessageBox("You reached prehistory era!");
}

/****************************************************************************
REMARKS:
Event handler for the 'Go forward' menu item
****************************************************************************/
void MyFrame::OnForward(
    wxCommandEvent&)
{
    if (!html -> HistoryForward())
        wxMessageBox("No more items in history!");
}

/****************************************************************************
REMARKS:
`Main program' equivalent: the program execution "starts" here
****************************************************************************/
bool MyApp::OnInit()
{
    // Create the main application window
    MyFrame *frame = new MyFrame("wxApplet testing application",
        wxPoint(50, 50), wxSize(640, 480));

    // Show it and tell the application that it's our main window
    frame->Show(TRUE);
    SetTopWindow(frame);

    // Success: wxApp::OnRun() will be called to run the application
    return TRUE;
}


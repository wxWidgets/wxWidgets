/////////////////////////////////////////////////////////////////////////////
// Name:        tree.cpp
// Purpose:     Minimal wxWidgets sample
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

#if !defined(__WXMSW__) // || wxUSE_XPM_IN_MSW
/* Closed folder */
static char * icon1_xpm[] = {
/* width height ncolors chars_per_pixel */
"16 16 6 1",
/* colors */
"   s None  c None",
".  c #000000",
"+  c #c0c0c0",
"@  c #808080",
"#  c #ffff00",
"$  c #ffffff",
/* pixels */
"                ",
"   @@@@@        ",
"  @#+#+#@       ",
" @#+#+#+#@@@@@@ ",
" @$$$$$$$$$$$$@.",
" @$#+#+#+#+#+#@.",
" @$+#+#+#+#+#+@.",
" @$#+#+#+#+#+#@.",
" @$+#+#+#+#+#+@.",
" @$#+#+#+#+#+#@.",
" @$+#+#+#+#+#+@.",
" @$#+#+#+#+#+#@.",
" @@@@@@@@@@@@@@.",
"  ..............",
"                ",
"                "};

/* File */
static char * icon2_xpm[] = {
/* width height ncolors chars_per_pixel */
"16 16 3 1",
/* colors */
"     s None    c None",
".    c #000000",
"+    c #ffffff",
/* pixels */
"                ",
"  ........      ",
"  .++++++..     ",
"  .+.+.++.+.    ",
"  .++++++....   ",
"  .+.+.+++++.   ",
"  .+++++++++.   ",
"  .+.+.+.+.+.   ",
"  .+++++++++.   ",
"  .+.+.+.+.+.   ",
"  .+++++++++.   ",
"  .+.+.+.+.+.   ",
"  .+++++++++.   ",
"  ...........   ",
"                ",
"                "};
#endif

#include "wx/imaglist.h"
#include "tree.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------
// the application icon
#ifndef __WXMSW__
    #include "mondrian.xpm"
#endif

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
    // create the main application window
    MyFrame *frame = new MyFrame(wxT("Tree Testing"),
                                 wxPoint(50, 50), wxSize(450, 340));

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
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, idMAIN_FRAME, title, pos, size)
{
    m_splitter = NULL;
    m_scrolledWindow = NULL;
    m_tree = NULL;
    m_valueWindow = NULL;
#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = Minimal_About;
#endif

    m_scrolledWindow = new wxSplitterScrolledWindow(this, idSCROLLED_WINDOW, wxDefaultPosition,
        wxSize(300, 400), wxNO_BORDER | wxCLIP_CHILDREN | wxVSCROLL);
    m_splitter = new wxThinSplitterWindow(m_scrolledWindow, idSPLITTER_WINDOW, wxDefaultPosition,
        wxDefaultSize, wxSP_3DBORDER | wxCLIP_CHILDREN /* | wxSP_LIVE_UPDATE */);
    m_splitter->SetSashSize(2);

    /* Note the wxTR_ROW_LINES style: draws horizontal lines between items */
    m_tree = new TestTree(m_splitter , idTREE_CTRL, wxDefaultPosition,
        wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_NO_LINES | wxNO_BORDER | wxTR_ROW_LINES );
    m_valueWindow = new TestValueWindow(m_splitter, idVALUE_WINDOW, wxDefaultPosition,
        wxDefaultSize, wxNO_BORDER);
    m_splitter->SplitVertically(m_tree, m_valueWindow);
    //m_splitter->AdjustScrollbars();
    m_splitter->SetSashPosition(200);
    m_scrolledWindow->SetTargetWindow(m_tree);

    m_scrolledWindow->EnableScrolling(false, false);

    // Let the two controls know about each other
    m_valueWindow->SetTreeCtrl(m_tree);
    m_tree->SetCompanionWindow(m_valueWindow);

    // set the frame icon
    SetIcon(wxICON(mondrian));

    // create a menu bar
    wxMenu *menuFile = new wxMenu(wxEmptyString, wxMENU_TEAROFF);

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, wxT("&About...\tCtrl-A"), wxT("Show about dialog"));

    menuFile->Append(Minimal_Quit, wxT("E&xit\tAlt-X"), wxT("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, wxT("&File"));
    menuBar->Append(helpMenu, wxT("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( wxT("This is the about dialog of splittree sample.\n")
                wxT("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, wxT("About Tree Test"), wxOK | wxICON_INFORMATION, this);
}

/*
 * TesTree
 */

IMPLEMENT_CLASS(TestTree, wxRemotelyScrolledTreeCtrl)

BEGIN_EVENT_TABLE(TestTree, wxRemotelyScrolledTreeCtrl)
END_EVENT_TABLE()

TestTree::TestTree(wxWindow* parent, wxWindowID id, const wxPoint& pt,
        const wxSize& sz, long style):
        wxRemotelyScrolledTreeCtrl(parent, id, pt, sz, style)
{
    m_imageList = new wxImageList(16, 16, true);
#if !defined(__WXMSW__) // || wxUSE_XPM_IN_MSW
    m_imageList->Add(wxIcon(icon1_xpm));
    m_imageList->Add(wxIcon(icon2_xpm));
#elif defined(__WXMSW__)
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_CLOSED_FOLDER"), wxBITMAP_TYPE_ICO_RESOURCE));
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_FILE"), wxBITMAP_TYPE_ICO_RESOURCE));
#else
#error "Sorry, we don't have icons available for this platforms."
#endif
    SetImageList(m_imageList);


    // Add some dummy items
    wxTreeItemId rootId = AddRoot(_("Root"), -1, -1);
    int i;
    for (i = 1; i <= 20; i++)
    {
        wxString label;
        label.Printf(wxT("Item %d"), i);
        wxTreeItemId id = AppendItem(rootId, label, 0);
        //SetItemImage( id, 1, wxTreeItemIcon_Expanded );

        int j;
        for (j = 0; j < 10; j++)
            AppendItem(id, _("Child"), 1);
    }
    Expand(rootId);
}

TestTree::~TestTree()
{
    SetImageList(NULL);
    delete m_imageList;
}

/*
 * TestValueWindow
 */

//IMPLEMENT_CLASS(TestValueWindow, wxWindow)

BEGIN_EVENT_TABLE(TestValueWindow, wxTreeCompanionWindow)
END_EVENT_TABLE()

TestValueWindow::TestValueWindow(wxWindow* parent, wxWindowID id,
      const wxPoint& pos,
      const wxSize& sz,
      long style):
      wxTreeCompanionWindow(parent, id, pos, sz, style)
{
    SetBackgroundColour(* wxWHITE);
}

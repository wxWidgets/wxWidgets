/////////////////////////////////////////////////////////////////////////////
// Name:        tree.cpp
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
    #pragma implementation "tree.cpp"
    #pragma interface "tree.cpp"
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
#endif

#include "wx/imaglist.h"
#include "tree.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------
// the application icon
#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "mondrian.xpm"
#endif

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
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
    // create the main application window
    MyFrame *frame = new MyFrame("Tree Testing",
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
		wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN | wxVSCROLL);
    m_splitter = new wxThinSplitterWindow(m_scrolledWindow, idSPLITTER_WINDOW, wxDefaultPosition,
		wxDefaultSize, wxSP_3DBORDER | wxCLIP_CHILDREN /* | wxSP_LIVE_UPDATE */);
	m_splitter->SetSashSize(2);
    m_tree = new TestTree(m_splitter, idTREE_CTRL, wxDefaultPosition,
		wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_NO_LINES | wxNO_BORDER );
    m_valueWindow = new TestValueWindow(m_splitter, idVALUE_WINDOW, wxDefaultPosition,
		wxDefaultSize, wxNO_BORDER);
    m_splitter->SplitVertically(m_tree, m_valueWindow);
	//m_splitter->AdjustScrollbars();
	m_splitter->SetSashPosition(200);
	m_scrolledWindow->SetTargetWindow(m_tree);

	m_scrolledWindow->EnableScrolling(FALSE, FALSE);

    // set the frame icon
    SetIcon(wxICON(mondrian));

    // create a menu bar
    wxMenu *menuFile = new wxMenu("", wxMENU_TEAROFF);

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About...\tCtrl-A", "Show about dialog");

    menuFile->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

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
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the about dialog of tree sample.\n")
                _T("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, "About Tree Test", wxOK | wxICON_INFORMATION, this);
}

/*
 * TesTree
 */

IMPLEMENT_CLASS(TestTree, wxRemotelyScrolledTreeCtrl)

BEGIN_EVENT_TABLE(TestTree, wxRemotelyScrolledTreeCtrl)
	EVT_PAINT(TestTree::OnPaint)
END_EVENT_TABLE()

TestTree::TestTree(wxWindow* parent, wxWindowID id, const wxPoint& pt,
        const wxSize& sz, long style):
        wxRemotelyScrolledTreeCtrl(parent, id, pt, sz, style)
{
    m_imageList = new wxImageList(16, 16, TRUE);
#if !defined(__WXMSW__) // || wxUSE_XPM_IN_MSW
    m_imageList->Add(wxIcon(icon1_xpm));
    m_imageList->Add(wxIcon(icon2_xpm));
    m_imageList->Add(wxIcon(icon3_xpm));
    m_imageList->Add(wxIcon(icon4_xpm));
    m_imageList->Add(wxIcon(icon5_xpm));
    m_imageList->Add(wxIcon(icon6_xpm));
    m_imageList->Add(wxIcon(icon7_xpm));
    m_imageList->Add(wxIcon(icon8_xpm));
#elif defined(__WXMSW__)
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_CLOSED_FOLDER"), wxBITMAP_TYPE_ICO_RESOURCE));
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_OPEN_FOLDER"), wxBITMAP_TYPE_ICO_RESOURCE));
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_FILE"), wxBITMAP_TYPE_ICO_RESOURCE));
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_COMPUTER"), wxBITMAP_TYPE_ICO_RESOURCE));
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_DRIVE"), wxBITMAP_TYPE_ICO_RESOURCE));
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_CDROM"), wxBITMAP_TYPE_ICO_RESOURCE));
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_FLOPPY"), wxBITMAP_TYPE_ICO_RESOURCE));
    m_imageList->Add(wxIcon(wxT("wxICON_SMALL_REMOVEABLE"), wxBITMAP_TYPE_ICO_RESOURCE));
#else
#error "Sorry, we don't have icons available for this platforms."
#endif
    SetImageList(m_imageList);

		
	// Add some dummy items
	wxTreeItemId rootId = AddRoot(_("Root"), 3, -1);
	int i;
	for (i = 1; i <= 20; i++)
	{
		wxString label;
		label.Printf(wxT("Item %d"), i);
		wxTreeItemId id = AppendItem(rootId, label, 0);
		SetItemImage( id, 1, wxTreeItemIcon_Expanded );

		int j;
		for (j = 0; j < 10; j++)
			AppendItem(id, _("Child"), 2);
	}
	Expand(rootId);
}

TestTree::~TestTree()
{
	SetImageList(NULL);
	delete m_imageList;
}

void TestTree::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);

	wxTreeCtrl::OnPaint(event);

    // Reset the device origin since it may have been set
    dc.SetDeviceOrigin(0, 0);

	wxSize sz = GetClientSize();

	wxPen pen(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DLIGHT), 1, wxSOLID);
	dc.SetPen(pen);
	dc.SetBrush(* wxTRANSPARENT_BRUSH);

	wxRect itemRect;
	if (GetBoundingRect(GetRootItem(), itemRect))
	{
		int itemHeight = itemRect.GetHeight();
		wxRect rcClient = GetRect();
		wxRect itemRect;
		int cy=0;
		wxTreeItemId h, lastH;
		for(h=GetFirstVisibleItem();h;h=GetNextVisible(h))
		{
			if (GetBoundingRect(h, itemRect))
			{
				cy = itemRect.GetTop();
				dc.DrawLine(rcClient.x, cy, rcClient.x + rcClient.width, cy);
				lastH = h;
				//cy += itemHeight;
			}
		}
		if (GetBoundingRect(lastH, itemRect))
		{
			cy = itemRect.GetBottom();
			dc.DrawLine(rcClient.x, cy, rcClient.x + rcClient.width, cy);
		}
	}
}

/*
 * TestValueWindow
 */

//IMPLEMENT_CLASS(TestValueWindow, wxWindow)

BEGIN_EVENT_TABLE(TestValueWindow, wxWindow)
	EVT_SIZE(TestValueWindow::OnSize)
END_EVENT_TABLE()

TestValueWindow::TestValueWindow(wxWindow* parent, wxWindowID id,
      const wxPoint& pos,
      const wxSize& sz,
      long style):
      wxWindow(parent, id, pos, sz, style)
{
	SetBackgroundColour(* wxWHITE);
}

void TestValueWindow::OnSize(wxSizeEvent& event)
{
}

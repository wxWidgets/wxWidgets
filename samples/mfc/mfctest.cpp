// hello.cpp : Defines the class behaviors for the application.
//           Hello is a simple program which consists of a main window
//           and an "About" dialog which can be invoked by a menu choice.
//           It is intended to serve as a starting-point for new
//           applications.
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// *** MODIFIED BY JULIAN SMART TO DEMONSTRATE CO-EXISTANCE WITH wxWINDOWS ***
//
// This sample pops up an initial wxWindows frame, with a menu item
// that allows a new MFC window to be created. Note that CDummyWindow
// is a class that allows a wxWindows window to be seen as a CWnd
// for the purposes of specifying a valid main window to the
// MFC initialisation.
//
// You can easily modify this code so that an MFC window pops up
// initially as the main frame, and allows wxWindows frames to be
// created subsequently:
//
// (1) Make MyApp::OnInit return FALSE, not creating a window.
// (2) Restore the MFC code to create a window in InitInstance, and remove
//     creation of CDummyWindow.
//
// IMPORTANT NOTES:
//
// (1) You need to set wxUSE_MFC to 1 in include/wx/msw/setup.h, which switches
// off some debugging features and also removes the windows.h inclusion
// in wxprec.h (MFC headers don't like this to have been included previously).
// Then recompile wxWindows and this sample.
//
// (2) 10/3/2000, wxWindows 2.1.14: unfortunately there is an assert when
// the sample tries to create an MFC window. Any suggestions welcome. It may be
// a problem with conflicting project settings. Ignoring the assert (several times)
// allows the sample to continue. In release mode the asserts don't happen.
//
// (3) I can't get the sample to link using a static MFC library, only the DLL
// version. Perhaps someone else is a wizard at working out the required settings
// in the wxWin library and the sample; then debugging the assert problem may be
// easier.

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/wx.h"

#ifdef _WINDOWS_
#error Sorry, you need to edit include/wx/wxprec.h, comment out the windows.h inclusion, and recompile.
#endif

#ifdef new
#undef new
#endif

#include "stdafx.h"

#ifdef DrawText
#undef DrawText
#endif

#include "resource.h"

#include "mfctest.h"

/////////////////////////////////////////////////////////////////////////////

// theApp:
// Just creating this application object runs the whole application.
//
CTheApp theApp;

// wxWindows elements

// Define a new application type
class MyApp: public wxApp
{ public:
    bool OnInit(void);
    wxFrame *CreateFrame(void);
 };

class MyCanvas: public wxScrolledWindow
{
  public:
    MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size);
    void OnPaint(wxPaintEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
DECLARE_EVENT_TABLE()
};

class MyChild: public wxFrame
{
  public:
    MyCanvas *canvas;
    MyChild(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size, const long style);
    ~MyChild(void);

    void OnQuit(wxCommandEvent& event);
    void OnNew(wxCommandEvent& event);
    void OnActivate(wxActivateEvent& event);

DECLARE_EVENT_TABLE()
};

// For drawing lines in a canvas
long xpos = -1;
long ypos = -1;

// ID for the menu quit command
#define HELLO_QUIT 1
#define HELLO_NEW  2

DECLARE_APP(MyApp)
IMPLEMENT_APP(MyApp)

/////////////////////////////////////////////////////////////////////////////

// CMainWindow constructor:
// Create the window with the appropriate style, size, menu, etc.
//
CMainWindow::CMainWindow()
{
	LoadAccelTable( "MainAccelTable" );
	Create( NULL, "Hello Foundation Application",
		WS_OVERLAPPEDWINDOW, rectDefault, NULL, "MainMenu" );
}

// OnPaint:
// This routine draws the string "Hello, Windows!" in the center of the
// client area.  It is called whenever Windows sends a WM_PAINT message.
// Note that creating a CPaintDC automatically does a BeginPaint and
// an EndPaint call is done when it is destroyed at the end of this
// function.  CPaintDC's constructor needs the window (this).
//
void CMainWindow::OnPaint()
{
	CString s = "Hello, Windows!";
	CPaintDC dc( this );
	CRect rect;

	GetClientRect( rect );
	dc.SetTextAlign( TA_BASELINE | TA_CENTER );
	dc.SetTextColor( ::GetSysColor( COLOR_WINDOWTEXT ) );
	dc.SetBkMode(TRANSPARENT);
	dc.TextOut( ( rect.right / 2 ), ( rect.bottom / 2 ),
				s, s.GetLength() );
}

// OnAbout:
// This member function is called when a WM_COMMAND message with an
// IDM_ABOUT code is received by the CMainWindow class object.  The
// message map below is responsible for this routing.
//
// We create a ClDialog object using the "AboutBox" resource (see
// hello.rc), and invoke it.
//
void CMainWindow::OnAbout()
{
	CDialog about( "AboutBox", this );
	about.DoModal();
}

void CMainWindow::OnTest()
{
  wxMessageBox("This is a wxWindows message box.\nWe're about to create a new wxWindows frame.", "wxWindows", wxOK);
  wxGetApp().CreateFrame();
}

// CMainWindow message map:
// Associate messages with member functions.
//
// It is implied that the ON_WM_PAINT macro expects a member function
// "void OnPaint()".
//
// It is implied that members connected with the ON_COMMAND macro
// receive no arguments and are void of return type, e.g., "void OnAbout()".
//
BEGIN_MESSAGE_MAP( CMainWindow, CFrameWnd )
	//{{AFX_MSG_MAP( CMainWindow )
	ON_WM_PAINT()
	ON_COMMAND( IDM_ABOUT, OnAbout )
	ON_COMMAND( IDM_TEST, OnTest )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTheApp

// InitInstance:
// When any CTheApp object is created, this member function is automatically
// called.  Any data may be set up at this point.
//
// Also, the main window of the application should be created and shown here.
// Return TRUE if the initialization is successful.
//
BOOL CTheApp::InitInstance()
{
	TRACE( "HELLO WORLD\n" );

	SetDialogBkColor();     // hook gray dialogs (was default in MFC V1)

    wxEntry((WXHINSTANCE) m_hInstance, (WXHINSTANCE) m_hPrevInstance, m_lpCmdLine, m_nCmdShow, FALSE);

/*
	m_pMainWnd = new CMainWindow();
	m_pMainWnd->ShowWindow( m_nCmdShow );
	m_pMainWnd->UpdateWindow();
*/

    if (wxTheApp && wxTheApp->GetTopWindow())
    {
        m_pMainWnd = new CDummyWindow((HWND) wxTheApp->GetTopWindow()->GetHWND());
    }

	return TRUE;
}

int CTheApp::ExitInstance()
{
  // OnExit isn't called by CleanUp so must be called explicitly.
  wxTheApp->OnExit();
  wxApp::CleanUp();

  return CWinApp::ExitInstance();
}

// Override this to provide wxWindows message loop
// compatibility

BOOL CTheApp::PreTranslateMessage(MSG *msg)
{
  if (wxTheApp && wxTheApp->ProcessMessage((WXMSG*) msg))
    return TRUE;
  else
    return CWinApp::PreTranslateMessage(msg);
}

BOOL CTheApp::OnIdle(LONG lCount)
{
    if (wxTheApp)
        return wxTheApp->ProcessIdle();
    else
        return FALSE;
}

/*********************************************************************
 * wxWindows elements
 ********************************************************************/
 
bool MyApp::OnInit(void)
{
  // Don't exit app when the top level frame is deleted
//  SetExitOnFrameDelete(FALSE);
  
  wxFrame* frame = CreateFrame();
  return TRUE;
}

wxFrame *MyApp::CreateFrame(void)
{
      MyChild *subframe = new MyChild(NULL, "Canvas Frame", wxPoint(10, 10), wxSize(300, 300),
                             wxDEFAULT_FRAME_STYLE);

      subframe->SetTitle("wxWindows canvas frame");

      // Give it a status line
      subframe->CreateStatusBar();

      // Make a menubar
      wxMenu *file_menu = new wxMenu;

      file_menu->Append(HELLO_NEW, "&New MFC Window");
      file_menu->Append(HELLO_QUIT, "&Close");

      wxMenuBar *menu_bar = new wxMenuBar;

      menu_bar->Append(file_menu, "&File");

      // Associate the menu bar with the frame
      subframe->SetMenuBar(menu_bar);

      int width, height;
      subframe->GetClientSize(&width, &height);

      MyCanvas *canvas = new MyCanvas(subframe, wxPoint(0, 0), wxSize(width, height));
      canvas->SetCursor(wxCursor(wxCURSOR_PENCIL));
      subframe->canvas = canvas;

      // Give it scrollbars
//      canvas->SetScrollbars(20, 20, 50, 50, 4, 4);

      subframe->Show(TRUE);
      // Return the main frame window
      return subframe;
}

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT(MyCanvas::OnPaint)
    EVT_MOUSE_EVENTS(MyCanvas::OnMouseEvent)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size):
 wxScrolledWindow(parent, -1, pos, size)
{
}

// Define the repainting behaviour
void MyCanvas::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    dc.SetFont(* wxSWISS_FONT);
    dc.SetPen(* wxGREEN_PEN);
    dc.DrawLine(0, 0, 200, 200);
    dc.DrawLine(200, 0, 0, 200);

    dc.SetBrush(* wxCYAN_BRUSH);
    dc.SetPen(* wxRED_PEN);
    dc.DrawRectangle(100, 100, 100, 50);
    dc.DrawRoundedRectangle(150, 150, 100, 50, 20);

    dc.DrawEllipse(250, 250, 100, 50);
    dc.DrawSpline(50, 200, 50, 100, 200, 10);
    dc.DrawLine(50, 230, 200, 230);
    dc.DrawText("This is a test string", 50, 230);
}

// This implements a tiny doodling program! Drag the mouse using
// the left button.
void MyCanvas::OnMouseEvent(wxMouseEvent& event)
{
    wxClientDC dc(this);
    dc.SetPen(* wxBLACK_PEN);
    wxPoint pos = event.GetPosition();
    if (xpos > -1 && ypos > -1 && event.Dragging())
    {
        dc.DrawLine(xpos, ypos, pos.x, pos.y);
    }
    xpos = pos.x;
    ypos = pos.y;
}

BEGIN_EVENT_TABLE(MyChild, wxFrame)
    EVT_MENU(HELLO_QUIT, MyChild::OnQuit)
    EVT_MENU(HELLO_NEW, MyChild::OnNew)
    EVT_ACTIVATE(MyChild::OnActivate)
END_EVENT_TABLE()

MyChild::MyChild(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size, const long style):
  wxFrame(frame, -1, title, pos, size, style)
{
  canvas = NULL;
}

MyChild::~MyChild(void)
{
}

void MyChild::OnQuit(wxCommandEvent& event)
{
    Close(TRUE);
}

void MyChild::OnNew(wxCommandEvent& event)
{
    CMainWindow *mainWin = new CMainWindow();
    mainWin->ShowWindow( TRUE );
    mainWin->UpdateWindow();
}
 
void MyChild::OnActivate(wxActivateEvent& event)
{
  if (event.GetActive() && canvas)
    canvas->SetFocus();
}

// Dummy MFC window for specifying a valid main window to MFC, using
// a wxWindows HWND.
CDummyWindow::CDummyWindow(HWND hWnd):CWnd()
{
  Attach(hWnd);
}

// Don't let the CWnd destructor delete the HWND
CDummyWindow::~CDummyWindow(void)
{
  Detach();
}


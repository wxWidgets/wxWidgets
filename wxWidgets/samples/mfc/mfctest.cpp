/////////////////////////////////////////////////////////////////////////////
// Name:        mfctest.cpp
// Purpose:     Sample to demonstrate mixing MFC and wxWidgets code
// Author:      Julian Smart
// Id:          $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// This sample pops up an initial wxWidgets frame, with a menu item
// that allows a new MFC window to be created. Note that CDummyWindow
// is a class that allows a wxWidgets window to be seen as a CWnd
// for the purposes of specifying a valid main window to the
// MFC initialisation.
//
// You can easily modify this code so that an MFC window pops up
// initially as the main frame, and allows wxWidgets frames to be
// created subsequently.
//
// (1) Make MyApp::OnInit not create a main window.
// (2) Make MFC's InitInstance create a main window, and remove
//     creation of CDummyWindow.
//
// This can be accomplished by setting START_WITH_MFC_WINDOW to 1 below.

#define START_WITH_MFC_WINDOW 0

//
// IMPORTANT NOTES:
//
// (1) You may need to set wxUSE_MFC to 1 in include/wx/msw/setup.h but
//     normally this shouldn't be needed any longer, i.e. it works without
//     it for me (VZ)
//
// (2) You should link with MFC DLL, not static libraries: or, to use static
//     run-time libraries, use this command for both building wxWidgets and
//     the sample:
//
//     nmake -f makefile.vc BUILD=debug SHARED=0 DEBUG_RUNTIME_LIBS=0 RUNTIME_LIBS=static all
//
//     Unless the run-time library settings match for wxWidgets and MFC, you will get
//     link errors for symbols such as __mbctype, __argc, and __argv 

#include "stdafx.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/evtloop.h"

#include "resource.h"

#include "mfctest.h"

/////////////////////////////////////////////////////////////////////////////

// theApp:
// Just creating this application object runs the whole application.
//
CTheApp theApp;

// wxWidgets elements

// Define a new application type
class MyApp: public wxApp
{
public:
    virtual bool OnInit();

    // we need to override this as the default behaviour only works when we're
    // running wxWidgets main loop, not MFC one
    virtual void ExitMainLoop();

    wxFrame *CreateFrame();
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
    MyChild(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size, const long style);
    virtual ~MyChild();

    void OnQuit(wxCommandEvent& event);
    void OnNew(wxCommandEvent& event);
    void OnActivate(wxActivateEvent& event);

    MyCanvas *canvas;

    DECLARE_EVENT_TABLE()
};

// ID for the menu quit command
#define HELLO_QUIT 1
#define HELLO_NEW  2

DECLARE_APP(MyApp)

// notice use of IMPLEMENT_APP_NO_MAIN() instead of the usual IMPLEMENT_APP!
IMPLEMENT_APP_NO_MAIN(MyApp)

CMainWindow::CMainWindow()
{
    LoadAccelTable( _T("MainAccelTable") );
    Create( NULL, _T("Hello Foundation Application"),
        WS_OVERLAPPEDWINDOW, rectDefault, NULL, _T("MainMenu") );
}

void CMainWindow::OnPaint()
{
    CString s = _T("Hello, Windows!");
    CPaintDC dc( this );
    CRect rect;

    GetClientRect( rect );
    dc.SetTextAlign( TA_BASELINE | TA_CENTER );
    dc.SetTextColor( ::GetSysColor( COLOR_WINDOWTEXT ) );
    dc.SetBkMode(TRANSPARENT);
    dc.TextOut( ( rect.right / 2 ), ( rect.bottom / 2 ),
        s, s.GetLength() );
}

void CMainWindow::OnAbout()
{
    CDialog about( _T("AboutBox"), this );
    about.DoModal();
}

void CMainWindow::OnTest()
{
    wxMessageBox(_T("This is a wxWidgets message box.\nWe're about to create a new wxWidgets frame."), _T("wxWidgets"), wxOK);
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

BOOL CTheApp::InitInstance()
{
    if ( !CWinApp::InitInstance() )
        return FALSE;

    // TODO: cmd line parsing
    WXDLLIMPEXP_BASE void wxSetInstance(HINSTANCE hInst);
    wxSetInstance(m_hInstance);
    wxApp::m_nCmdShow = m_nCmdShow;
    int argc = 0;
    wxChar **argv = NULL;
    wxEntryStart(argc, argv);
    if ( !wxTheApp || !wxTheApp->CallOnInit() )
        return FALSE;

#if START_WITH_MFC_WINDOW
    // Demonstrate creation of an initial MFC main window.
    m_pMainWnd = new CMainWindow();
    m_pMainWnd->ShowWindow( m_nCmdShow );
    m_pMainWnd->UpdateWindow();
#else
    // Demonstrate creation of an initial wxWidgets main window.
    // Wrap wxWidgets window in a dummy MFC window and
    // make the main window.
    if (wxTheApp && wxTheApp->GetTopWindow())
    {
        m_pMainWnd = new CDummyWindow((HWND) wxTheApp->GetTopWindow()->GetHWND());
    }
#endif

    return TRUE;
}

int CTheApp::ExitInstance()
{
#if !START_WITH_MFC_WINDOW
    delete m_pMainWnd;
#endif

    if ( wxTheApp )
        wxTheApp->OnExit();
    wxEntryCleanup();

    return CWinApp::ExitInstance();
}

// Override this to provide wxWidgets message loop compatibility
BOOL CTheApp::PreTranslateMessage(MSG *msg)
{
    wxEventLoop *evtLoop = wxEventLoop::GetActive();
    if ( evtLoop && evtLoop->PreProcessMessage(msg) )
        return TRUE;

    return CWinApp::PreTranslateMessage(msg);
}

BOOL CTheApp::OnIdle(LONG WXUNUSED(lCount))
{
    return wxTheApp && wxTheApp->ProcessIdle();
}

/*********************************************************************
* wxWidgets elements
********************************************************************/

bool MyApp::OnInit()
{
#if !START_WITH_MFC_WINDOW
    // as we're not inside wxWidgets main loop, the default logic doesn't work
    // in our case and we need to do this explicitly
    SetExitOnFrameDelete(true);

    (void) CreateFrame();
#endif

    return true;
}

void MyApp::ExitMainLoop()
{
    // instead of existing wxWidgets main loop, terminate the MFC one
    ::PostQuitMessage(0);
}

wxFrame *MyApp::CreateFrame()
{
    MyChild *subframe = new MyChild(NULL, _T("Canvas Frame"), wxPoint(10, 10), wxSize(300, 300),
        wxDEFAULT_FRAME_STYLE);

    subframe->SetTitle(_T("wxWidgets canvas frame"));

    // Give it a status line
    subframe->CreateStatusBar();

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(HELLO_NEW, _T("&New MFC Window"));
    file_menu->Append(HELLO_QUIT, _T("&Close"));

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, _T("&File"));

    // Associate the menu bar with the frame
    subframe->SetMenuBar(menu_bar);

    int width, height;
    subframe->GetClientSize(&width, &height);

    MyCanvas *canvas = new MyCanvas(subframe, wxPoint(0, 0), wxSize(width, height));
    canvas->SetCursor(wxCursor(wxCURSOR_PENCIL));
    subframe->canvas = canvas;
    subframe->Show(true);

    // Return the main frame window
    return subframe;
}

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT(MyCanvas::OnPaint)
    EVT_MOUSE_EVENTS(MyCanvas::OnMouseEvent)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size)
        : wxScrolledWindow(parent, -1, pos, size)
{
}

// Define the repainting behaviour
void MyCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
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
    dc.DrawLine(50, 230, 200, 230);
    dc.DrawText(_T("This is a test string"), 50, 230);
}

// This implements a tiny doodling program! Drag the mouse using
// the left button.
void MyCanvas::OnMouseEvent(wxMouseEvent& event)
{
    static long s_xpos = -1;
    static long s_ypos = -1;

    wxClientDC dc(this);
    dc.SetPen(* wxBLACK_PEN);
    wxPoint pos = event.GetPosition();
    if (s_xpos > -1 && s_ypos > -1 && event.Dragging())
    {
        dc.DrawLine(s_xpos, s_ypos, pos.x, pos.y);
    }

    s_xpos = pos.x;
    s_ypos = pos.y;
}

BEGIN_EVENT_TABLE(MyChild, wxFrame)
    EVT_MENU(HELLO_QUIT, MyChild::OnQuit)
    EVT_MENU(HELLO_NEW, MyChild::OnNew)
    EVT_ACTIVATE(MyChild::OnActivate)
END_EVENT_TABLE()

MyChild::MyChild(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size, const long style)
       : wxFrame(frame, -1, title, pos, size, style)
{
    canvas = NULL;
}

MyChild::~MyChild()
{
    if ( IsLastBeforeExit() )
        PostQuitMessage(0);
}

void MyChild::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyChild::OnNew(wxCommandEvent& WXUNUSED(event))
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
// a wxWidgets HWND.
CDummyWindow::CDummyWindow(HWND hWnd)
{
    Attach(hWnd);
}

// Don't let the CWnd destructor delete the HWND
CDummyWindow::~CDummyWindow()
{
    Detach();
}


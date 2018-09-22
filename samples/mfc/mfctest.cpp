/////////////////////////////////////////////////////////////////////////////
// Name:        mfctest.cpp
// Purpose:     Sample to demonstrate mixing MFC and wxWidgets code
// Author:      Julian Smart
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
// (2) Define a class deriving from wxMFCWinApp and override its InitMainWnd()
//     to create the main window in MFC code.
//
// This can be accomplished by setting START_WITH_MFC_WINDOW to 1 below.

#define START_WITH_MFC_WINDOW 0

// NOTES:
//
//  *  You should link with MFC DLL, not static libraries: or, to use static
//     run-time libraries, use this command for both building wxWidgets and
//     the sample:
//
//     nmake -f makefile.vc BUILD=debug SHARED=0 DEBUG_RUNTIME_LIBS=0 RUNTIME_LIBS=static all
//
//     Unless the run-time library settings match for wxWidgets and MFC, you
//     will get link errors for symbols such as __mbctype, __argc, and __argv
//
//  *  If you see bogus memory leaks within the MSVC IDE on exit, in this
//     sample or in your own project, you must be using __WXDEBUG__ +
//     WXUSINGDLL + _AFXDLL
//     Unfortunately this confuses the MSVC/MFC leak detector. To do away with
//     these bogus memory leaks, add this to the list of link objects, make it
//     first: mfc[version][u]d.lib
//     -  [version] -> 42 or 70 or 80 etc
//     -  u if using Unicode

// Disable deprecation warnings from headers included from stdafx.h for VC8+
#ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
#endif

// Also define WINVER to avoid warnings about it being undefined from the
// platform SDK headers (usually this is not necessary because it is done by wx
// headers but here we include the system ones before them)
#ifndef WINVER
    #define WINVER 0x0600
#endif

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
#include "wx/nativewin.h"
#include "wx/spinctrl.h"

#include "wx/msw/mfc.h"

#include "resource.h"

#include "mfctest.h"

/////////////////////////////////////////////////////////////////////////////

// theApp:
// Just creating this application object runs the whole application.
//
SampleMFCWinApp theApp;

// wxWidgets elements

// Define a new application type inheriting from wxAppWithMFC
class MyApp: public wxAppWithMFC
{
public:
    virtual bool OnInit() wxOVERRIDE;

    wxFrame *CreateFrame();
};

class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size);
    void OnPaint(wxPaintEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    wxDECLARE_EVENT_TABLE();
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

    wxDECLARE_EVENT_TABLE();
};

class MyPanel : public wxPanel
{
public:
    MyPanel(wxWindow *parent, const wxPoint& pos)
        : wxPanel(parent, wxID_ANY, pos)
    {
        wxSizer* const sizer = new wxFlexGridSizer(2, wxSize(5, 5));
        sizer->Add(new wxStaticText(this, wxID_ANY, "Enter your &name:"),
                   wxSizerFlags().Center().Right());
        m_textName = new wxTextCtrl(this, wxID_ANY);
        m_textName->SetHint("First Last");
        sizer->Add(m_textName, wxSizerFlags().Expand().CenterVertical());

        sizer->Add(new wxStaticText(this, wxID_ANY, "And your &age:"),
                   wxSizerFlags().Center().Right());
        m_spinAge = new wxSpinCtrl(this, wxID_ANY);
        sizer->Add(m_spinAge, wxSizerFlags().Expand().CenterVertical());

        wxStaticBoxSizer* const
            box = new wxStaticBoxSizer(wxVERTICAL, this, "wxWidgets box");
        box->Add(sizer, wxSizerFlags(1).Expand());
        SetSizer(box);

        // We won't be resized automatically, so set our size ourselves.
        SetSize(GetBestSize());
    }

private:
    wxTextCtrl* m_textName;
    wxSpinCtrl* m_spinAge;
};

// ID for the menu quit command
#define HELLO_QUIT 1
#define HELLO_NEW  2

wxDECLARE_APP(MyApp);

// Notice use of wxIMPLEMENT_APP_NO_MAIN() instead of the usual wxIMPLEMENT_APP!
wxIMPLEMENT_APP_NO_MAIN(MyApp);

#ifdef _UNICODE
// In Unicode build MFC normally requires to manually change the entry point to
// wWinMainCRTStartup() but to avoid having to modify the project options to do
// it we provide an adapter for it.
extern "C" int wWinMainCRTStartup();

int WINAPI WinMain(HINSTANCE, HINSTANCE, char *, int)
{
    wWinMainCRTStartup();
}
#endif // _UNICODE

CMainWindow::CMainWindow()
{
    LoadAccelTable( "MainAccelTable" );
    Create( NULL, "Hello Foundation Application",
        WS_OVERLAPPEDWINDOW, rectDefault, NULL, "MainMenu" );

    // Create a container representing the MFC window in wxWidgets window
    // hierarchy.
    m_containerWX = new wxNativeContainerWindow(m_hWnd);

    // Now we can create children of this container as usual.
    new MyPanel(m_containerWX, wxPoint(5, 5));

    // An ugly but necessary workaround to prevent the container TLW from
    // resizing the panel to fit its entire client area as it would do if it
    // were its only child.
    new wxWindow(m_containerWX, wxID_ANY, wxPoint(4, 4), wxSize(1, 1));
}

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

void CMainWindow::OnAbout()
{
    CDialog about( "AboutBox", this );
    about.DoModal();
}

void CMainWindow::OnTest()
{
    wxMessageBox("This is a wxWidgets message box.\nWe're about to create a new wxWidgets frame.", "wxWidgets", wxOK);
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

/*********************************************************************
* wxWidgets elements
********************************************************************/

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

#if !START_WITH_MFC_WINDOW
    (void) CreateFrame();
#endif

    return true;
}

wxFrame *MyApp::CreateFrame()
{
    MyChild *subframe = new MyChild(NULL, "Canvas Frame", wxPoint(10, 10), wxSize(300, 300),
        wxDEFAULT_FRAME_STYLE);

    subframe->SetTitle("wxWidgets canvas frame");

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
    subframe->Show(true);

    // Return the main frame window
    return subframe;
}

wxBEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT(MyCanvas::OnPaint)
    EVT_MOUSE_EVENTS(MyCanvas::OnMouseEvent)
wxEND_EVENT_TABLE()

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
    dc.DrawText("This is a test string", 50, 230);
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

wxBEGIN_EVENT_TABLE(MyChild, wxFrame)
    EVT_MENU(HELLO_QUIT, MyChild::OnQuit)
    EVT_MENU(HELLO_NEW, MyChild::OnNew)
    EVT_ACTIVATE(MyChild::OnActivate)
wxEND_EVENT_TABLE()

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

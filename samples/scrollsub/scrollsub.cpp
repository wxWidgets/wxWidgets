/*
 * Program: scrollsub
 *
 * Author: Robert Roebling
 *
 * Copyright: (C) 1999, Robert Roebling
 *
 */

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/image.h"
#include "wx/listctrl.h"
#include "wx/sizer.h"
#include "wx/log.h"


// derived classes

class MyFrame;
class MyScrolledWindow;
class MyCanvas;
class MyApp;

// MyScrolledWindow

class MyScrolledWindow: public wxScrolledWindow
{
public:
    MyScrolledWindow() {}
    MyScrolledWindow( wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size );
    ~MyScrolledWindow();
    void OnPaint( wxPaintEvent &event );

private:
    MyCanvas    *m_canvas;

    DECLARE_DYNAMIC_CLASS(MyScrolledWindow)
    DECLARE_EVENT_TABLE()
};

// MyCanvas

class MyCanvas: public wxPanel
{
public:
    MyCanvas() {}
    MyCanvas( wxScrolledWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size );
    ~MyCanvas();
    void OnPaint( wxPaintEvent &event );

private:
    wxScrolledWindow   *m_owner;

    DECLARE_DYNAMIC_CLASS(MyCanvas)
    DECLARE_EVENT_TABLE()
};

// MyFrame

class MyFrame: public wxFrame
{
public:
    MyFrame();

    void OnAbout( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );

    wxScrolledWindow   *m_scrolled;
    wxTextCtrl         *m_log;

private:
    DECLARE_DYNAMIC_CLASS(MyFrame)
    DECLARE_EVENT_TABLE()
};

// MyApp

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};

// main program

IMPLEMENT_APP(MyApp)

// MyScrolledWindow

IMPLEMENT_DYNAMIC_CLASS(MyScrolledWindow, wxScrolledWindow)

BEGIN_EVENT_TABLE(MyScrolledWindow, wxScrolledWindow)
  EVT_PAINT(        MyScrolledWindow::OnPaint)
END_EVENT_TABLE()

MyScrolledWindow::MyScrolledWindow( wxWindow *parent, wxWindowID id,
                    const wxPoint &pos, const wxSize &size )
        : wxScrolledWindow( parent, id, pos, size, wxSUNKEN_BORDER, "test canvas" )
{
    m_canvas = new MyCanvas( this, -1, wxDefaultPosition, wxDefaultSize );
    
    SetTargetWindow( m_canvas );

    SetBackgroundColour( "WHEAT" );
  
    SetCursor( wxCursor( wxCURSOR_HAND ) );
    
    
    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
    
    topsizer->Add( m_canvas, 1, wxEXPAND|wxALL, 30 );

    SetAutoLayout( TRUE );
    SetSizer( topsizer );
}

MyScrolledWindow::~MyScrolledWindow()
{
}

void MyScrolledWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );

    wxSize size( GetClientSize() );
    
    long w,h;
    dc.GetTextExtent( wxT("Headline"), &w, &h );

    dc.DrawText( wxT("Headline"), long (size.x / 2 - w / 2), 10 );
}

// MyCanvas

IMPLEMENT_DYNAMIC_CLASS(MyCanvas, wxPanel)

BEGIN_EVENT_TABLE(MyCanvas, wxPanel)
  EVT_PAINT(      MyCanvas::OnPaint)
END_EVENT_TABLE()

MyCanvas::MyCanvas( wxScrolledWindow *parent, wxWindowID id,
                    const wxPoint &pos, const wxSize &size )
        : wxPanel( parent, id, pos, size, wxSUNKEN_BORDER, "test canvas" )
{
    m_owner = parent;
    
    (void)new wxButton( this, -1, "Hallo I", wxPoint(20,20), wxSize(100,30) );
    (void)new wxButton( this, -1, "Hallo II", wxPoint(220,20), wxSize(100,30) );

    SetBackgroundColour( *wxWHITE );
  
    SetCursor( wxCursor( wxCURSOR_IBEAM ) );
}

MyCanvas::~MyCanvas()
{
}

void MyCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );
    m_owner->PrepareDC( dc );

    dc.DrawText( "Some text", 140, 140 );
  
    dc.DrawRectangle( 100, 160, 200, 200 );
    
    dc.SetBrush( *wxTRANSPARENT_BRUSH );
    
    dc.DrawRectangle( 10, 10, 480, 980 );
}


// MyFrame

const int ID_QUIT  = 108;
const int ID_ABOUT = 109;

IMPLEMENT_DYNAMIC_CLASS( MyFrame, wxFrame )

BEGIN_EVENT_TABLE(MyFrame,wxFrame)
  EVT_MENU    (ID_ABOUT, MyFrame::OnAbout)
  EVT_MENU    (ID_QUIT,  MyFrame::OnQuit)
END_EVENT_TABLE()

MyFrame::MyFrame()
       : wxFrame( (wxFrame *)NULL, -1, "wxScrolledWindow sample",
                  wxPoint(20,20), wxSize(470,500) )
{
    wxMenu *file_menu = new wxMenu();
    file_menu->Append( ID_ABOUT, "&About..");
    file_menu->Append( ID_QUIT, "E&xit\tAlt-X");

    wxMenuBar *menu_bar = new wxMenuBar();
    menu_bar->Append(file_menu, "&File");

    SetMenuBar( menu_bar );

    CreateStatusBar(2);
    int widths[] = { -1, 100 };
    SetStatusWidths( 2, widths );

    m_scrolled = new MyScrolledWindow( this, -1, wxPoint(0,0), wxSize(100,100) );
    m_scrolled->SetScrollbars( 10, 10, 50, 100 );
  
    m_log = new wxTextCtrl( this, -1, "This is the log window.\n", wxPoint(0,0), wxSize(100,100), wxTE_MULTILINE );
    wxLog *old_log = wxLog::SetActiveTarget( new wxLogTextCtrl( m_log ) );
    delete old_log;
    
    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
    
    topsizer->Add( m_scrolled, 1, wxEXPAND );
    topsizer->Add( m_log, 0, wxEXPAND );

    SetAutoLayout( TRUE );
    SetSizer( topsizer );
}

void MyFrame::OnQuit( wxCommandEvent &WXUNUSED(event) )
{
  Close( TRUE );
}

void MyFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
  (void)wxMessageBox( "wxScroll demo II\n"
                      "Robert Roebling (c) 1998",
                      "About wxScroll II Demo", wxICON_INFORMATION | wxOK );
}

//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------

bool MyApp::OnInit()
{
    wxFrame *frame = new MyFrame();
    frame->Show( TRUE );

    return TRUE;
}


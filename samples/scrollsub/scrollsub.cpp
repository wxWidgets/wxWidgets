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

// MyTopLabels

class MyTopLabels: public wxWindow
{
public:
    MyTopLabels() {}
    MyTopLabels( wxScrolledWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size );
   
    void OnPaint( wxPaintEvent &event );
    
private:
    wxScrolledWindow   *m_owner;
    
    DECLARE_DYNAMIC_CLASS(MyTopLabels)
    DECLARE_EVENT_TABLE()
};

// MyRightLabels

class MyRightLabels: public wxWindow
{
public:
    MyRightLabels() {}
    MyRightLabels( wxScrolledWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size );
   
    void OnPaint( wxPaintEvent &event );
    
private:
    wxScrolledWindow   *m_owner;
    
    DECLARE_DYNAMIC_CLASS(MyRightLabels)
    DECLARE_EVENT_TABLE()
};

// MyCanvas

class MyCanvas: public wxPanel
{
public:
    MyCanvas() {}
    MyCanvas( wxScrolledWindow *parent, MyTopLabels *top, MyRightLabels *right,
              wxWindowID id, const wxPoint &pos, const wxSize &size );
    ~MyCanvas();
    void OnPaint( wxPaintEvent &event );
    void ScrollWindow( int dx, int dy, const wxRect *rect );

private:
    wxScrolledWindow   *m_owner;
    MyTopLabels        *m_topLabels;
    MyRightLabels      *m_rightLabels;

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
    void OnFullScreen( wxCommandEvent &event );

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
    MyTopLabels *top = new MyTopLabels( this, -1, wxDefaultPosition, wxSize(-1,25) );
    MyRightLabels *right = new MyRightLabels( this, -1, wxDefaultPosition, wxSize(60,-1) );

    m_canvas = new MyCanvas( this, top, right, -1, wxDefaultPosition, wxDefaultSize );
    
    SetTargetWindow( m_canvas );

    SetBackgroundColour( "WHEAT" );
  
    SetCursor( wxCursor( wxCURSOR_HAND ) );
    
    wxBoxSizer *mainsizer = new wxBoxSizer( wxVERTICAL );
    
    wxBoxSizer *topsizer = new wxBoxSizer( wxHORIZONTAL );
    topsizer->Add( 60,25 );
    topsizer->Add( top, 1 );
    
    mainsizer->Add( topsizer, 0, wxEXPAND );
    
    wxBoxSizer *middlesizer = new wxBoxSizer( wxHORIZONTAL );
    middlesizer->Add( right, 0, wxEXPAND );
    middlesizer->Add( m_canvas, 1, wxEXPAND );
    
    mainsizer->Add( middlesizer, 1, wxEXPAND );

    SetAutoLayout( TRUE );
    SetSizer( mainsizer );
}

MyScrolledWindow::~MyScrolledWindow()
{
}

void MyScrolledWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );

/*
    wxSize size( GetClientSize() );
    
    long w,h;
    dc.GetTextExtent( wxT("Headline"), &w, &h );

    dc.DrawText( wxT("Headline"), long (size.x / 2 - w / 2), 10 );
*/
}

// MyTopLabels

IMPLEMENT_DYNAMIC_CLASS(MyTopLabels,wxWindow)

BEGIN_EVENT_TABLE(MyTopLabels, wxWindow)
  EVT_PAINT(      MyTopLabels::OnPaint)
END_EVENT_TABLE()

MyTopLabels::MyTopLabels( wxScrolledWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size )
  : wxWindow( parent, id, pos, size )
{
    m_owner = parent;
}

void MyTopLabels::OnPaint( wxPaintEvent &event )
{
    wxPaintDC dc(this);
    m_owner->PrepareDC( dc );
    dc.DrawText( "Column 1", 5, 5 );
    dc.DrawText( "Column 2", 105, 5 );
    dc.DrawText( "Column 3", 205, 5 );
}

// MyRightLabels

IMPLEMENT_DYNAMIC_CLASS(MyRightLabels,wxWindow)

BEGIN_EVENT_TABLE(MyRightLabels, wxWindow)
  EVT_PAINT(      MyRightLabels::OnPaint)
END_EVENT_TABLE()

MyRightLabels::MyRightLabels( wxScrolledWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size )
  : wxWindow( parent, id, pos, size )
{
    m_owner = parent;
}

void MyRightLabels::OnPaint( wxPaintEvent &event )
{
    wxPaintDC dc(this);
    m_owner->PrepareDC( dc );
    dc.DrawText( "Row 1", 5, 5 );
    dc.DrawText( "Row 2", 5, 30 );
    dc.DrawText( "Row 3", 5, 55 );
    dc.DrawText( "Row 4", 5, 80 );
    dc.DrawText( "Row 5", 5, 105 );
    dc.DrawText( "Row 6", 5, 130 );
}

// MyCanvas

IMPLEMENT_DYNAMIC_CLASS(MyCanvas, wxPanel)

BEGIN_EVENT_TABLE(MyCanvas, wxPanel)
  EVT_PAINT(      MyCanvas::OnPaint)
END_EVENT_TABLE()

MyCanvas::MyCanvas( wxScrolledWindow *parent, MyTopLabels *top, MyRightLabels *right,
    wxWindowID id, const wxPoint &pos, const wxSize &size )
        : wxPanel( parent, id, pos, size, wxSUNKEN_BORDER, "test canvas" )
{
    m_owner = parent;
    m_topLabels = top;
    m_rightLabels = right;
    
    (void)new wxButton( this, -1, "Hallo I", wxPoint(0,50), wxSize(100,25) );
    (void)new wxButton( this, -1, "Hallo II", wxPoint(200,50), wxSize(100,25) );
    
    (void)new wxTextCtrl( this, -1, "Text I", wxPoint(0,100), wxSize(100,25) );
    (void)new wxTextCtrl( this, -1, "Text II", wxPoint(200,100), wxSize(100,25) );

    SetBackgroundColour( "WHEAT" );
  
    SetCursor( wxCursor( wxCURSOR_IBEAM ) );
}

MyCanvas::~MyCanvas()
{
}

void MyCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );
    m_owner->PrepareDC( dc );

    dc.SetPen( *wxBLACK_PEN );
    
    // OK, let's assume we are a grid control and we have two
    // grid cells. Here in OnPaint we want to know which cell
    // to redraw so that we prevent redrawing cells that don't
    // need to get redrawn. We have one cell at (0,0) and one
    // more at (200,0), both having a size of (100,25).
    
    // We can query how much the window has been scrolled 
    // by calling CalcUnscrolledPosition()
    
    int scroll_x = 0;
    int scroll_y = 0;
    m_owner->CalcUnscrolledPosition( scroll_x, scroll_y, &scroll_x, &scroll_y );
    
    // We also need to know the size of the window to see which
    // cells are completely hidden and not get redrawn
    
    int size_x = 0;
    int size_y = 0;
    GetClientSize( &size_x, &size_y );
    
    // First cell: (0,0)(100,25)
    // It it on screen?
    if ((0+100-scroll_x > 0) && (0+25-scroll_y > 0) &&
        (0-scroll_x < size_x) && (0-scroll_y < size_y))
    {
        // Has the region on screen been exposed?
	    if (IsExposed(0,0,100,25))
	    {
	        wxLogMessage( "Redraw first cell" );
            dc.DrawRectangle( 0, 0, 100, 25 );
	        dc.DrawText( "First Cell", 5, 5 );
	    }
    }
    
    
    // Second cell: (0,200)(100,25)
    // It it on screen?
    if ((200+100-scroll_x > 0) && (0+25-scroll_y > 0) &&
        (200-scroll_x < size_x) && (0-scroll_y < size_y))
    {
        // Has the region on screen been exposed?
	    if (IsExposed(200,0,100,25))
	    {
	        wxLogMessage( "Redraw second cell" );
            dc.DrawRectangle( 200, 0, 100, 25 );
	        dc.DrawText( "Second Cell", 205, 5 );
	    }
    }
    
}

void MyCanvas::ScrollWindow( int dx, int dy, const wxRect *rect )
{
    wxPanel::ScrollWindow( dx, dy, rect );
    m_topLabels->ScrollWindow( dx, 0, rect );
    m_rightLabels->ScrollWindow( 0, dy, rect );
}

// MyFrame

const int ID_QUIT  = 108;
const int ID_FULL  = 109;
const int ID_ABOUT = 110;

IMPLEMENT_DYNAMIC_CLASS( MyFrame, wxFrame )

BEGIN_EVENT_TABLE(MyFrame,wxFrame)
  EVT_MENU    (ID_ABOUT, MyFrame::OnAbout)
  EVT_MENU    (ID_QUIT,  MyFrame::OnQuit)
  EVT_MENU    (ID_FULL,  MyFrame::OnFullScreen)
END_EVENT_TABLE()

MyFrame::MyFrame()
       : wxFrame( (wxFrame *)NULL, -1, "wxScrolledWindow sample",
                  wxPoint(20,20), wxSize(470,500) )
{
    wxMenu *file_menu = new wxMenu();
    file_menu->Append( ID_ABOUT, "&About..");
    file_menu->Append( ID_FULL, "&Full screen..");
    file_menu->Append( ID_QUIT, "E&xit\tAlt-X");

    wxMenuBar *menu_bar = new wxMenuBar();
    menu_bar->Append(file_menu, "&File");

    SetMenuBar( menu_bar );

    CreateStatusBar(2);
    int widths[] = { -1, 100 };
    SetStatusWidths( 2, widths );

    m_scrolled = new MyScrolledWindow( this, -1, wxDefaultPosition, wxSize(100,100) );
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

void MyFrame::OnFullScreen( wxCommandEvent &WXUNUSED(event) )
{
#ifdef __MSW__
   ShowFullScreen( TRUE, wxFULLSCREEN_NOBORDER|wxFULLSCREEN_NOCAPTION );
#endif
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


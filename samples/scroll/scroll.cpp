/*
 * Program: scroll
 *
 * Author: Robert Roebling
 *
 * Copyright: (C) 1998, Robert Roebling
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
class MyApp;

// MyCanvas

class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas() {};
    MyCanvas( wxWindow *parent, wxWindowID, const wxPoint &pos, const wxSize &size );
    ~MyCanvas();
    void OnPaint( wxPaintEvent &event );
    void OnQueryPosition( wxCommandEvent &event );
    void OnAddButton( wxCommandEvent &event );
    void OnDeleteButton( wxCommandEvent &event );
    void OnMoveButton( wxCommandEvent &event );
    void OnScrollWin( wxCommandEvent &event );
    void OnMouseDown( wxMouseEvent &event );

    wxButton *m_button;

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

    MyCanvas         *m_canvas;
    wxTextCtrl       *m_log;

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

// ids

#define   ID_ADDBUTTON    1
#define   ID_DELBUTTON    2
#define   ID_MOVEBUTTON   3
#define   ID_SCROLLWIN    4
#define   ID_QUERYPOS     5

#define   ID_NEWBUTTON    10

// MyCanvas

IMPLEMENT_DYNAMIC_CLASS(MyCanvas, wxScrolledWindow)

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
  EVT_PAINT(                  MyCanvas::OnPaint)
  EVT_MOUSE_EVENTS(                  MyCanvas::OnMouseDown)
  EVT_BUTTON( ID_QUERYPOS,    MyCanvas::OnQueryPosition)
  EVT_BUTTON( ID_ADDBUTTON,   MyCanvas::OnAddButton)
  EVT_BUTTON( ID_DELBUTTON,   MyCanvas::OnDeleteButton)
  EVT_BUTTON( ID_MOVEBUTTON,  MyCanvas::OnMoveButton)
  EVT_BUTTON( ID_SCROLLWIN,   MyCanvas::OnScrollWin)
END_EVENT_TABLE()

MyCanvas::MyCanvas( wxWindow *parent, wxWindowID id,
                    const wxPoint &pos, const wxSize &size )
        : wxScrolledWindow( parent, id, pos, size, wxSUNKEN_BORDER | wxTAB_TRAVERSAL, "test canvas" )
{
    (void) new wxButton( this, ID_ADDBUTTON, "add button", wxPoint(10,10) );
    (void) new wxButton( this, ID_DELBUTTON, "del button", wxPoint(10,40) );
    (void) new wxButton( this, ID_MOVEBUTTON, "move button", wxPoint(150,10) );
    (void) new wxButton( this, ID_SCROLLWIN, "scroll win", wxPoint(250,10) );

    wxString choices[] =
    {
        "This",
        "is one of my",
        "really",
        "wonderful",
        "examples."
    };
  
    m_button = new wxButton( this, ID_QUERYPOS, "Query position", wxPoint(10,110) );
  
    (void) new wxTextCtrl( this, -1, "wxTextCtrl", wxPoint(10,150), wxSize(80,-1) );
  
    (void) new wxRadioButton( this, -1, "Disable", wxPoint(10,190) );
  
    (void) new wxComboBox( this, -1, "This", wxPoint(10,230), wxDefaultSize, 5, choices );
  
    (void) new wxRadioBox( this, -1, "This", wxPoint(10,310), wxDefaultSize, 5, choices, 2, wxRA_SPECIFY_COLS );

    (void) new wxRadioBox( this, -1, "This", wxPoint(10,440), wxDefaultSize, 5, choices, 2, wxRA_SPECIFY_ROWS );

    wxListCtrl *m_listCtrl = new wxListCtrl(
	this, -1, wxPoint(200, 110), wxSize(180, 120),
	wxLC_REPORT | wxSIMPLE_BORDER | wxLC_SINGLE_SEL );

    m_listCtrl->InsertColumn(0, "First", wxLIST_FORMAT_LEFT, 90);
    m_listCtrl->InsertColumn(1, "Last", wxLIST_FORMAT_LEFT, 90);

    for ( int i=0; i < 30; i++)
    {
        char buf[20];
        sprintf(buf, "Item %d", i);
        m_listCtrl->InsertItem(i, buf);
    }
    m_listCtrl->SetItemState( 3, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );

    (void) new wxListBox( this, -1, wxPoint(260,280), wxSize(120,120), 5, choices, wxLB_ALWAYS_SB );

    wxPanel *test = new wxPanel( this, -1, wxPoint(10, 530), wxSize(130,120), wxSIMPLE_BORDER | wxTAB_TRAVERSAL );
    test->SetBackgroundColour( "WHEAT" );
    wxButton *test2 = new wxButton( test, -1, "Hallo", wxPoint(10,10) );
  
    test = new wxPanel( this, -1, wxPoint(160, 530), wxSize(130,120), wxSUNKEN_BORDER | wxTAB_TRAVERSAL );
    test->SetBackgroundColour( "WHEAT" );
    test->SetCursor( wxCursor( wxCURSOR_NO_ENTRY ) );
    test2 = new wxButton( test, -1, "Hallo", wxPoint(10,10) );
    test2->SetCursor( wxCursor( wxCURSOR_PENCIL ) );
  
    test = new wxPanel( this, -1, wxPoint(310, 530), wxSize(130,120), wxRAISED_BORDER | wxTAB_TRAVERSAL );
    test->SetBackgroundColour( "WHEAT" );
    test->SetCursor( wxCursor( wxCURSOR_PENCIL ) );
    test2 = new wxButton( test, -1, "Hallo", wxPoint(10,10) );
    test2->SetCursor( wxCursor( wxCURSOR_NO_ENTRY ) );

    SetBackgroundColour( "WHEAT" );
  
    SetCursor( wxCursor( wxCURSOR_IBEAM ) );
}

MyCanvas::~MyCanvas()
{
}

void MyCanvas::OnMouseDown( wxMouseEvent &event )
{
    if (event.LeftDown())
    {
        wxPoint pt( event.GetPosition() );
        int x,y;
        CalcUnscrolledPosition( pt.x, pt.y, &x, &y );
        wxLogMessage( "Mouse down event at: %d %d, scrolled: %d %d", pt.x, pt.y, x, y );
    }
    
    if (event.LeftIsDown() &&
        event.LeftDown())
    {
        wxLogMessage( "Error: both LeftDown() and LeftIsDown() are TRUE!" );
    }
}

void MyCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );
    PrepareDC( dc );

    dc.DrawText( "Press mouse button to test calculations!", 160, 50 );

    dc.DrawText( "Some text", 140, 140 );
  
    dc.DrawRectangle( 100, 160, 200, 200 );
}

void MyCanvas::OnQueryPosition( wxCommandEvent &WXUNUSED(event) )
{
    wxPoint pt( m_button->GetPosition() );
    wxLogMessage( "Position of ""Query position"" is %d %d", pt.x, pt.y );
    pt = ClientToScreen( pt );
    wxLogMessage( "Position of ""Query position"" on screen is %d %d", pt.x, pt.y );
}

void MyCanvas::OnAddButton( wxCommandEvent &WXUNUSED(event) )
{
    wxLogMessage( "Inserting button at position 10,70..." );
    wxButton *button = new wxButton( this, ID_NEWBUTTON, "new button", wxPoint(10,70), wxSize(80,25) );
    wxPoint pt( button->GetPosition() );
    wxLogMessage( "-> Position after inserting %d %d", pt.x, pt.y );
}

void MyCanvas::OnDeleteButton( wxCommandEvent &event )
{
    wxLogMessage( "Deleting button inserted with ""Add button""..." );
    wxWindow *win = FindWindow( ID_NEWBUTTON );
    if (win)
       win->Destroy();
    else
       wxLogMessage( "-> No window with id = ID_NEWBUTTON found." );
}

void MyCanvas::OnMoveButton( wxCommandEvent &event )
{
    wxLogMessage( "Moving button 10 pixels downward.." );
    wxWindow *win = FindWindow( event.GetId() );
    wxPoint pt( win->GetPosition() );
    wxLogMessage( "-> Position before move is %d %d", pt.x, pt.y );
    win->Move( -1, pt.y + 10 );
    pt = win->GetPosition();
    wxLogMessage( "-> Position after move is %d %d", pt.x, pt.y );
}

void MyCanvas::OnScrollWin( wxCommandEvent &WXUNUSED(event) )
{
    wxLogMessage( "Scrolling 2 units up.\nThe white square and the controls should move equally!" );
    int x,y;
    ViewStart( &x, &y );
    Scroll( -1, y+2 );
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

    m_canvas = new MyCanvas( this, -1, wxPoint(0,0), wxSize(100,100) );
    m_canvas->SetScrollbars( 10, 10, 50, 100 );
  
    m_log = new wxTextCtrl( this, -1, "This is the log window.\n", wxPoint(0,0), wxSize(100,100), wxTE_MULTILINE );
    wxLog *old_log = wxLog::SetActiveTarget( new wxLogTextCtrl( m_log ) );
    delete old_log;
    
    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
    
    topsizer->Add( m_canvas, 1, wxEXPAND );
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
  (void)wxMessageBox( "wxScroll demo\n"
                      "Robert Roebling (c) 1998",
                      "About wxScroll Demo", wxICON_INFORMATION | wxOK );
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


/*
 * Program: scroll
 *
 * Author: Robert Roebling
 *
 * Copyright: (C) 1998, Robert Roebling
 *                2002, Ron Lee
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
    MyCanvas() {}
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


// ----------------------------------------------------------------------------
// Autoscrolling example.
// ----------------------------------------------------------------------------

// this class uses the 'virtual' size attribute along with an internal
// sizer to automatically set up scrollbars as needed

class MyAutoScrollWindow : public wxScrolledWindow
{
private:

    wxButton    *m_button;

public:

    MyAutoScrollWindow( wxWindow *parent );

    void OnResizeClick( wxCommandEvent &WXUNUSED( event ) );

    DECLARE_EVENT_TABLE()
};


// ----------------------------------------------------------------------------
// MyScrolledWindow classes: examples of wxScrolledWindow usage
// ----------------------------------------------------------------------------

// base class for both of them
class MyScrolledWindowBase : public wxScrolledWindow
{
public:
    MyScrolledWindowBase(wxWindow *parent)
        : wxScrolledWindow(parent)
        , m_nLines( 100 )
    {
        wxClientDC dc(this);
        dc.GetTextExtent(_T("Line 17"), NULL, &m_hLine);
    }

protected:
    // the height of one line on screen
    wxCoord m_hLine;

    // the number of lines we draw
    size_t m_nLines;
};

// this class does "stupid" redrawing - it redraws everything each time
// and sets the scrollbar extent directly.

class MyScrolledWindowDumb : public MyScrolledWindowBase
{
public:
    MyScrolledWindowDumb(wxWindow *parent) : MyScrolledWindowBase(parent)
    {
        // no horz scrolling
        SetScrollbars(0, m_hLine, 0, m_nLines + 1, 0, 0, TRUE /* no refresh */);
    }

    virtual void OnDraw(wxDC& dc);
};

// this class does "smart" redrawing - only redraws the lines which must be
// redrawn and sets the scroll rate and virtual size to affect the
// scrollbars.
//
// Note that this class should produce identical results to the one above.

class MyScrolledWindowSmart : public MyScrolledWindowBase
{
public:
    MyScrolledWindowSmart(wxWindow *parent) : MyScrolledWindowBase(parent)
    {
        // no horz scrolling
        SetScrollRate( 0, m_hLine );
        SetVirtualSize( -1, ( m_nLines + 1 ) * m_hLine );
    }

    virtual void OnDraw(wxDC& dc);
};


// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

class MyFrame: public wxFrame
{
public:
    MyFrame();

    void OnAbout( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );
    void OnDeleteAll( wxCommandEvent &event );
    void OnInsertNew( wxCommandEvent &event );

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

const long   ID_ADDBUTTON   = wxNewId();
const long   ID_DELBUTTON   = wxNewId();
const long   ID_MOVEBUTTON  = wxNewId();
const long   ID_SCROLLWIN   = wxNewId();
const long   ID_QUERYPOS    = wxNewId();

const long   ID_NEWBUTTON   = wxNewId();

// MyCanvas

IMPLEMENT_DYNAMIC_CLASS(MyCanvas, wxScrolledWindow)

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
  EVT_PAINT(                  MyCanvas::OnPaint)
  EVT_MOUSE_EVENTS(           MyCanvas::OnMouseDown)
  EVT_BUTTON( ID_QUERYPOS,    MyCanvas::OnQueryPosition)
  EVT_BUTTON( ID_ADDBUTTON,   MyCanvas::OnAddButton)
  EVT_BUTTON( ID_DELBUTTON,   MyCanvas::OnDeleteButton)
  EVT_BUTTON( ID_MOVEBUTTON,  MyCanvas::OnMoveButton)
  EVT_BUTTON( ID_SCROLLWIN,   MyCanvas::OnScrollWin)
END_EVENT_TABLE()

MyCanvas::MyCanvas( wxWindow *parent, wxWindowID id,
                    const wxPoint &pos, const wxSize &size )
    : wxScrolledWindow( parent, id, pos, size, wxSUNKEN_BORDER | wxTAB_TRAVERSAL, _T("test canvas") )
{
    SetScrollRate( 10, 10 );
    SetVirtualSize( 500, 1000 );

    (void) new wxButton( this, ID_ADDBUTTON,  _T("add button"), wxPoint(10,10) );
    (void) new wxButton( this, ID_DELBUTTON,  _T("del button"), wxPoint(10,40) );
    (void) new wxButton( this, ID_MOVEBUTTON, _T("move button"), wxPoint(150,10) );
    (void) new wxButton( this, ID_SCROLLWIN,  _T("scroll win"), wxPoint(250,10) );

#if 0

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

#endif

    wxPanel *test = new wxPanel( this, -1, wxPoint(10, 110), wxSize(130,50), wxSIMPLE_BORDER | wxTAB_TRAVERSAL );
    test->SetBackgroundColour( wxT("WHEAT") );

#if 0

    wxButton *test2 = new wxButton( test, -1, "Hallo", wxPoint(10,10) );

    test = new wxPanel( this, -1, wxPoint(160, 530), wxSize(130,120), wxSUNKEN_BORDER | wxTAB_TRAVERSAL );
    test->SetBackgroundColour( wxT("WHEAT") );
    test->SetCursor( wxCursor( wxCURSOR_NO_ENTRY ) );
    test2 = new wxButton( test, -1, "Hallo", wxPoint(10,10) );
    test2->SetCursor( wxCursor( wxCURSOR_PENCIL ) );

    test = new wxPanel( this, -1, wxPoint(310, 530), wxSize(130,120), wxRAISED_BORDER | wxTAB_TRAVERSAL );
    test->SetBackgroundColour( wxT("WHEAT") );
    test->SetCursor( wxCursor( wxCURSOR_PENCIL ) );
    test2 = new wxButton( test, -1, "Hallo", wxPoint(10,10) );
    test2->SetCursor( wxCursor( wxCURSOR_NO_ENTRY ) );

#endif

    SetBackgroundColour( wxT("BLUE") );

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
        wxLogMessage( wxT("Mouse down event at: %d %d, scrolled: %d %d"), pt.x, pt.y, x, y );

        if ( !event.LeftIsDown() )
            wxLogMessage( wxT("Error: LeftIsDown() should be TRUE if for LeftDown()") );
    }
}

void MyCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );
    PrepareDC( dc );

    dc.DrawText( _T("Press mouse button to test calculations!"), 160, 50 );

    dc.DrawText( _T("Some text"), 140, 140 );

    dc.DrawRectangle( 100, 160, 200, 200 );
}

void MyCanvas::OnQueryPosition( wxCommandEvent &WXUNUSED(event) )
{
    wxPoint pt( m_button->GetPosition() );
    wxLogMessage( wxT("Position of \"Query position\" is %d %d"), pt.x, pt.y );
    pt = ClientToScreen( pt );
    wxLogMessage( wxT("Position of \"Query position\" on screen is %d %d"), pt.x, pt.y );
}

void MyCanvas::OnAddButton( wxCommandEvent &WXUNUSED(event) )
{
    wxLogMessage( wxT("Inserting button at position 10,70...") );
    wxButton *button = new wxButton( this, ID_NEWBUTTON, wxT("new button"), wxPoint(10,70), wxSize(80,25) );
    wxPoint pt( button->GetPosition() );
    wxLogMessage( wxT("-> Position after inserting %d %d"), pt.x, pt.y );
}

void MyCanvas::OnDeleteButton( wxCommandEvent &event )
{
    wxLogMessage( wxT("Deleting button inserted with \"Add button\"...") );
    wxWindow *win = FindWindow( ID_NEWBUTTON );
    if (win)
       win->Destroy();
    else
       wxLogMessage( wxT("-> No window with id = ID_NEWBUTTON found.") );
}

void MyCanvas::OnMoveButton( wxCommandEvent &event )
{
    wxLogMessage( wxT("Moving button 10 pixels downward..") );
    wxWindow *win = FindWindow( event.GetId() );
    wxPoint pt( win->GetPosition() );
    wxLogMessage( wxT("-> Position before move is %d %d"), pt.x, pt.y );
    win->Move( -1, pt.y + 10 );
    pt = win->GetPosition();
    wxLogMessage( wxT("-> Position after move is %d %d"), pt.x, pt.y );
}

void MyCanvas::OnScrollWin( wxCommandEvent &WXUNUSED(event) )
{
    wxLogMessage( wxT("Scrolling 2 units up.\nThe white square and the controls should move equally!") );
    int x,y;
    GetViewStart( &x, &y );
    Scroll( -1, y+2 );
}

// MyAutoScrollWindow

const long   ID_RESIZEBUTTON = wxNewId();
const wxSize SMALL_BUTTON( 100, 50 );
const wxSize LARGE_BUTTON( 300, 100 );

BEGIN_EVENT_TABLE( MyAutoScrollWindow, wxScrolledWindow)
  EVT_BUTTON( ID_RESIZEBUTTON,    MyAutoScrollWindow::OnResizeClick)
END_EVENT_TABLE()

MyAutoScrollWindow::MyAutoScrollWindow( wxWindow *parent )
    : wxScrolledWindow( parent )
{
    SetBackgroundColour( wxT("GREEN") );

    // Set the rate we'd like for scrolling.

    SetScrollRate( 5, 5 );

    // Populate a sizer with a 'resizing' button and some
    // other static decoration

    wxFlexGridSizer  *innersizer = new wxFlexGridSizer( 2, 2 );

    m_button = new wxButton( this,
                             ID_RESIZEBUTTON,
                             _T("Press me"),
                             wxDefaultPosition,
                             SMALL_BUTTON );

    // We need to do this here, because wxADJUST_MINSIZE below
    // will cause the initial size to be ignored for Best/Min size.
    // It would be nice to fix the sizers to handle this a little
    // more cleanly.

    m_button->SetSizeHints( SMALL_BUTTON.GetWidth(), SMALL_BUTTON.GetHeight() );

    innersizer->Add( m_button,
                     0,
                     wxALIGN_CENTER | wxALL | wxADJUST_MINSIZE,
                     20 );

    innersizer->Add( new wxStaticText( this, -1, _T("This is just") ),
                    0,
                    wxALIGN_CENTER );

    innersizer->Add( new wxStaticText( this, -1, _T("some decoration") ),
                    0,
                    wxALIGN_CENTER );

    innersizer->Add( new wxStaticText( this, -1, _T("for you to scroll...") ),
                    0,
                    wxALIGN_CENTER );

    // Then use the sizer to set the scrolled region size.

    SetSizer( innersizer );
}

void MyAutoScrollWindow::OnResizeClick( wxCommandEvent &WXUNUSED( event ) )
{
    // Arbitrarily resize the button to change the minimum size of
    // the (scrolled) sizer.

    if( m_button->GetSize() == SMALL_BUTTON )
        m_button->SetSizeHints( LARGE_BUTTON.GetWidth(), LARGE_BUTTON.GetHeight() );
    else
        m_button->SetSizeHints( SMALL_BUTTON.GetWidth(), SMALL_BUTTON.GetHeight() );

    // Force update layout and scrollbars, since nothing we do here
    // necessarily generates a size event which would do it for us.

    FitInside();
}

// MyFrame

const long ID_QUIT       = wxNewId();
const long ID_ABOUT      = wxNewId();
const long ID_DELETE_ALL = wxNewId();
const long ID_INSERT_NEW = wxNewId();

IMPLEMENT_DYNAMIC_CLASS( MyFrame, wxFrame )

BEGIN_EVENT_TABLE(MyFrame,wxFrame)
  EVT_MENU    (ID_DELETE_ALL, MyFrame::OnDeleteAll)
  EVT_MENU    (ID_INSERT_NEW,  MyFrame::OnInsertNew)
  EVT_MENU    (ID_ABOUT, MyFrame::OnAbout)
  EVT_MENU    (ID_QUIT,  MyFrame::OnQuit)
END_EVENT_TABLE()

MyFrame::MyFrame()
       : wxFrame( (wxFrame *)NULL, -1, _T("wxScrolledWindow sample"),
                  wxPoint(20,20), wxSize(470,500) )
{
    wxMenu *file_menu = new wxMenu();
    file_menu->Append( ID_DELETE_ALL, _T("Delete all"));
    file_menu->Append( ID_INSERT_NEW, _T("Insert new"));
    file_menu->Append( ID_ABOUT,      _T("&About.."));
    file_menu->Append( ID_QUIT,       _T("E&xit\tAlt-X"));

    wxMenuBar *menu_bar = new wxMenuBar();
    menu_bar->Append(file_menu, _T("&File"));

    SetMenuBar( menu_bar );

    CreateStatusBar(2);
    int widths[] = { -1, 100 };
    SetStatusWidths( 2, widths );

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    // Setting an explicit size here is superfluous, it will be overridden
    // by the sizer in any case.
    m_canvas = new MyCanvas( this, -1, wxPoint(0,0), wxSize(100,100) );

    // This is done with ScrollRate/VirtualSize in MyCanvas ctor now,
    // both should produce identical results.
    //m_canvas->SetScrollbars( 10, 10, 50, 100 );

    topsizer->Add( m_canvas, 1, wxEXPAND );
    topsizer->Add( new MyAutoScrollWindow( this ), 1, wxEXPAND );

    wxSizer *sizerBtm = new wxBoxSizer(wxHORIZONTAL);
    sizerBtm->Add( new MyScrolledWindowDumb(this), 1, wxEXPAND );
    sizerBtm->Add( new MyScrolledWindowSmart(this), 1, wxEXPAND );
    topsizer->Add( sizerBtm, 1, wxEXPAND );

    SetSizer( topsizer );
}

void MyFrame::OnDeleteAll( wxCommandEvent &WXUNUSED(event) )
{
    m_canvas->DestroyChildren();
}

void MyFrame::OnInsertNew( wxCommandEvent &WXUNUSED(event) )
{
    (void)new wxButton( m_canvas, -1, _T("Hello"), wxPoint(100,100) );
}

void MyFrame::OnQuit( wxCommandEvent &WXUNUSED(event) )
{
  Close( TRUE );
}

void MyFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
  (void)wxMessageBox( _T("wxScroll demo\n"
                         "Robert Roebling (c) 1998\n"
                         "Autoscrolling examples\n"
                         "Ron Lee (c) 2002"),
                      _T("About wxScroll Demo"),
                      wxICON_INFORMATION | wxOK );
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

// ----------------------------------------------------------------------------
// MyScrolledWindowXXX
// ----------------------------------------------------------------------------

void MyScrolledWindowDumb::OnDraw(wxDC& dc)
{
    // this is useful to see which lines are redrawn
    static size_t s_redrawCount = 0;
    dc.SetTextForeground(s_redrawCount++ % 2 ? *wxRED : *wxBLUE);

    wxCoord y = 0;
    for ( size_t line = 0; line < m_nLines; line++ )
    {
        wxCoord yPhys;
        CalcScrolledPosition(0, y, NULL, &yPhys);

        dc.DrawText(wxString::Format(_T("Line %u (logical %d, physical %d)"),
                                     line, y, yPhys), 0, y);
        y += m_hLine;
    }
}

void MyScrolledWindowSmart::OnDraw(wxDC& dc)
{
    // this is useful to see which lines are redrawn
    static size_t s_redrawCount = 0;
    dc.SetTextForeground(s_redrawCount++ % 2 ? *wxRED : *wxBLUE);

    // update region is always in device coords, translate to logical ones
    wxRect rectUpdate = GetUpdateRegion().GetBox();
    CalcUnscrolledPosition(rectUpdate.x, rectUpdate.y,
                           &rectUpdate.x, &rectUpdate.y);

    size_t lineFrom = rectUpdate.y / m_hLine,
           lineTo = rectUpdate.GetBottom() / m_hLine;

    if ( lineTo > m_nLines - 1)
        lineTo = m_nLines - 1;

    wxCoord y = lineFrom*m_hLine;
    for ( size_t line = lineFrom; line <= lineTo; line++ )
    {
        wxCoord yPhys;
        CalcScrolledPosition(0, y, NULL, &yPhys);

        dc.DrawText(wxString::Format(_T("Line %u (logical %d, physical %d)"),
                                     line, y, yPhys), 0, y);
        y += m_hLine;
    }
}

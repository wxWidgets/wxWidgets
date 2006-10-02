/////////////////////////////////////////////////////////////////////////////
// Name:        scroll.cpp
// Purpose:     wxScrolledWindow sample
// Author:      Robert Roebling
// Modified by:
// Created:
// RCS-ID:      $Id$
// Copyright:   (C) 1998 Robert Roebling, 2002 Ron Lee, 2003 Matt Gregory
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

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

const long ID_QUIT       = wxID_EXIT;
const long ID_ABOUT      = wxID_ABOUT;
const long ID_DELETE_ALL = 100;
const long ID_INSERT_NEW = 101;

// ----------------------------------------------------------------------
// a trivial example
// ----------------------------------------------------------------------

class MySimpleFrame;
class MySimpleCanvas;

// MySimpleCanvas

class MySimpleCanvas: public wxScrolledWindow
{
public:
    MySimpleCanvas() { }
    MySimpleCanvas( wxWindow *parent, wxWindowID, const wxPoint &pos, const wxSize &size );

    void OnPaint( wxPaintEvent &event );

private:
    DECLARE_DYNAMIC_CLASS(MyCanvas)
    DECLARE_EVENT_TABLE()
};

IMPLEMENT_DYNAMIC_CLASS(MySimpleCanvas, wxScrolledWindow)

BEGIN_EVENT_TABLE(MySimpleCanvas, wxScrolledWindow)
  EVT_PAINT(      MySimpleCanvas::OnPaint)
END_EVENT_TABLE()

MySimpleCanvas::MySimpleCanvas( wxWindow *parent, wxWindowID id,
                    const wxPoint &pos, const wxSize &size )
    : wxScrolledWindow( parent, id, pos, size, wxSUNKEN_BORDER, _T("test canvas") )
{
    SetScrollRate( 10, 10 );
    SetVirtualSize( 92, 97 );
    SetBackgroundColour( *wxWHITE );
}

void MySimpleCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc(this);
    PrepareDC( dc );

    dc.SetPen( *wxRED_PEN );
    dc.SetBrush( *wxTRANSPARENT_BRUSH );
    dc.DrawRectangle( 0,0,92,97 );
}

// MySimpleFrame

class MySimpleFrame: public wxFrame
{
public:
    MySimpleFrame();

    void OnQuit( wxCommandEvent &event );

    MySimpleCanvas         *m_canvas;

private:
    DECLARE_DYNAMIC_CLASS(MySimpleFrame)
    DECLARE_EVENT_TABLE()
};


IMPLEMENT_DYNAMIC_CLASS( MySimpleFrame, wxFrame )

BEGIN_EVENT_TABLE(MySimpleFrame,wxFrame)
  EVT_MENU    (ID_QUIT,  MySimpleFrame::OnQuit)
END_EVENT_TABLE()

MySimpleFrame::MySimpleFrame()
       : wxFrame( (wxFrame *)NULL, wxID_ANY, _T("wxScrolledWindow sample"),
                  wxPoint(120,120), wxSize(150,150) )
{
    wxMenu *file_menu = new wxMenu();
    file_menu->Append( ID_QUIT,       _T("E&xit\tAlt-X"));

    wxMenuBar *menu_bar = new wxMenuBar();
    menu_bar->Append(file_menu, _T("&File"));

    SetMenuBar( menu_bar );

    m_canvas = new MySimpleCanvas( this, wxID_ANY, wxPoint(0,0), wxSize(100,100) );
}

void MySimpleFrame::OnQuit( wxCommandEvent &WXUNUSED(event) )
{
  Close( true );
}

// ----------------------------------------------------------------------
// a complex example
// ----------------------------------------------------------------------

// derived classes

class MyFrame;
class MyApp;

// MyCanvas

class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas() {}
    MyCanvas( wxWindow *parent, wxWindowID, const wxPoint &pos, const wxSize &size );
    ~MyCanvas(){};
    void OnPaint( wxPaintEvent &event );
    void OnQueryPosition( wxCommandEvent &event );
    void OnAddButton( wxCommandEvent &event );
    void OnDeleteButton( wxCommandEvent &event );
    void OnMoveButton( wxCommandEvent &event );
    void OnScrollWin( wxCommandEvent &event );
    void OnMouseRightDown( wxMouseEvent &event );
    void OnMouseWheel( wxMouseEvent &event );

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
        SetScrollbars(0, m_hLine, 0, m_nLines + 1, 0, 0, true /* no refresh */);
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
        SetVirtualSize( wxDefaultCoord, ( m_nLines + 1 ) * m_hLine );
    }

    virtual void OnDraw(wxDC& dc);
};

// ----------------------------------------------------------------------------
// MyAutoTimedScrollingWindow: implements a text viewer with simple blocksize
//                             selection to test auto-scrolling functionality
// ----------------------------------------------------------------------------

class MyAutoTimedScrollingWindow : public wxScrolledWindow
{
protected:  // member data
    // test data variables
    static const wxChar* sm_testData;
    static const int sm_lineCnt; // line count
    static const int sm_lineLen; // line length in characters
    // sizes for graphical data
    wxCoord m_fontH, m_fontW;
    // selection tracking
    wxPoint m_selStart; // beginning of blockwise selection
    wxPoint m_cursor;   // end of blockwise selection (mouse position)

protected:  // gui stuff
    wxFont m_font;

public: // interface
    MyAutoTimedScrollingWindow( wxWindow* parent );
    wxRect DeviceCoordsToGraphicalChars(wxRect updRect) const;
    wxPoint DeviceCoordsToGraphicalChars(wxPoint pos) const;
    wxPoint GraphicalCharToDeviceCoords(wxPoint pos) const;
    wxRect LogicalCoordsToGraphicalChars(wxRect updRect) const;
    wxPoint LogicalCoordsToGraphicalChars(wxPoint pos) const;
    wxPoint GraphicalCharToLogicalCoords(wxPoint pos) const;
    void MyRefresh();
    bool IsSelected(int chX, int chY) const;
    static bool IsInside(int k, int bound1, int bound2);
    static wxRect DCNormalize(wxCoord x, wxCoord y, wxCoord w, wxCoord h);

protected: // event stuff
    void OnDraw(wxDC& dc);
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnScroll(wxScrollWinEvent& event);

    DECLARE_EVENT_TABLE()
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

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};


// ----------------------------------------------------------------------------
// main program
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

// ids

const long   ID_ADDBUTTON   = wxNewId();
const long   ID_DELBUTTON   = wxNewId();
const long   ID_MOVEBUTTON  = wxNewId();
const long   ID_SCROLLWIN   = wxNewId();
const long   ID_QUERYPOS    = wxNewId();

const long   ID_NEWBUTTON   = wxNewId();

// ----------------------------------------------------------------------------
// MyCanvas
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(MyCanvas, wxScrolledWindow)

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
  EVT_PAINT(                  MyCanvas::OnPaint)
  EVT_RIGHT_DOWN(             MyCanvas::OnMouseRightDown)
  EVT_MOUSEWHEEL(             MyCanvas::OnMouseWheel)
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

    (void) new wxTextCtrl( this, wxID_ANY, "wxTextCtrl", wxPoint(10,150), wxSize(80,wxDefaultCoord) );

    (void) new wxRadioButton( this, wxID_ANY, "Disable", wxPoint(10,190) );

    (void) new wxComboBox( this, wxID_ANY, "This", wxPoint(10,230), wxDefaultSize, 5, choices );

    (void) new wxRadioBox( this, wxID_ANY, "This", wxPoint(10,310), wxDefaultSize, 5, choices, 2, wxRA_SPECIFY_COLS );

    (void) new wxRadioBox( this, wxID_ANY, "This", wxPoint(10,440), wxDefaultSize, 5, choices, 2, wxRA_SPECIFY_ROWS );

    wxListCtrl *m_listCtrl = new wxListCtrl(
            this, wxID_ANY, wxPoint(200, 110), wxSize(180, 120),
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

    (void) new wxListBox( this, wxID_ANY, wxPoint(260,280), wxSize(120,120), 5, choices, wxLB_ALWAYS_SB );

#endif

    wxPanel *test = new wxPanel( this, wxID_ANY, wxPoint(10, 110), wxSize(130,50), wxSIMPLE_BORDER | wxTAB_TRAVERSAL );
    test->SetBackgroundColour( wxT("WHEAT") );

#if 0

    wxButton *test2 = new wxButton( test, wxID_ANY, "Hallo", wxPoint(10,10) );

    test = new wxPanel( this, wxID_ANY, wxPoint(160, 530), wxSize(130,120), wxSUNKEN_BORDER | wxTAB_TRAVERSAL );
    test->SetBackgroundColour( wxT("WHEAT") );
    test->SetCursor( wxCursor( wxCURSOR_NO_ENTRY ) );
    test2 = new wxButton( test, wxID_ANY, "Hallo", wxPoint(10,10) );
    test2->SetCursor( wxCursor( wxCURSOR_PENCIL ) );

    test = new wxPanel( this, wxID_ANY, wxPoint(310, 530), wxSize(130,120), wxRAISED_BORDER | wxTAB_TRAVERSAL );
    test->SetBackgroundColour( wxT("WHEAT") );
    test->SetCursor( wxCursor( wxCURSOR_PENCIL ) );
    test2 = new wxButton( test, wxID_ANY, "Hallo", wxPoint(10,10) );
    test2->SetCursor( wxCursor( wxCURSOR_NO_ENTRY ) );

#endif

    SetBackgroundColour( wxT("BLUE") );

    SetCursor( wxCursor( wxCURSOR_IBEAM ) );
}

void MyCanvas::OnMouseRightDown( wxMouseEvent &event )
{
    wxPoint pt( event.GetPosition() );
    int x,y;
    CalcUnscrolledPosition( pt.x, pt.y, &x, &y );
    wxLogMessage( wxT("Mouse down event at: %d %d, scrolled: %d %d"), pt.x, pt.y, x, y );
}

void MyCanvas::OnMouseWheel( wxMouseEvent &event )
{
    wxPoint pt( event.GetPosition() );
    int x,y;
    CalcUnscrolledPosition( pt.x, pt.y, &x, &y );
    wxLogMessage( wxT("Mouse wheel event at: %d %d, scrolled: %d %d\n")
                  wxT("Rotation: %d, delta = %d"),
                  pt.x, pt.y, x, y,
                  event.GetWheelRotation(), event.GetWheelDelta() );

    event.Skip();
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

void MyCanvas::OnDeleteButton( wxCommandEvent &WXUNUSED(event) )
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
    win->Move( wxDefaultCoord, pt.y + 10 );
    pt = win->GetPosition();
    wxLogMessage( wxT("-> Position after move is %d %d"), pt.x, pt.y );
}

void MyCanvas::OnScrollWin( wxCommandEvent &WXUNUSED(event) )
{
    wxLogMessage( wxT("Scrolling 2 units up.\nThe white square and the controls should move equally!") );
    int x,y;
    GetViewStart( &x, &y );
    Scroll( wxDefaultCoord, y+2 );
}

// ----------------------------------------------------------------------------
// MyAutoScrollWindow
// ----------------------------------------------------------------------------

const long   ID_RESIZEBUTTON = wxNewId();
const wxSize SMALL_BUTTON( 100, 50 );
const wxSize LARGE_BUTTON( 300, 100 );

BEGIN_EVENT_TABLE( MyAutoScrollWindow, wxScrolledWindow)
  EVT_BUTTON( ID_RESIZEBUTTON,    MyAutoScrollWindow::OnResizeClick)
END_EVENT_TABLE()

MyAutoScrollWindow::MyAutoScrollWindow( wxWindow *parent )
    : wxScrolledWindow( parent, -1, wxDefaultPosition, wxDefaultSize,
                        wxSUNKEN_BORDER|wxScrolledWindowStyle )
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

    innersizer->Add( new wxStaticText( this, wxID_ANY, _T("This is just") ),
                    0,
                    wxALIGN_CENTER );

    innersizer->Add( new wxStaticText( this, wxID_ANY, _T("some decoration") ),
                    0,
                    wxALIGN_CENTER );

    innersizer->Add( new wxStaticText( this, wxID_ANY, _T("for you to scroll...") ),
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

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( MyFrame, wxFrame )

BEGIN_EVENT_TABLE(MyFrame,wxFrame)
  EVT_MENU    (ID_DELETE_ALL, MyFrame::OnDeleteAll)
  EVT_MENU    (ID_INSERT_NEW,  MyFrame::OnInsertNew)
  EVT_MENU    (ID_ABOUT, MyFrame::OnAbout)
  EVT_MENU    (ID_QUIT,  MyFrame::OnQuit)
END_EVENT_TABLE()

MyFrame::MyFrame()
       : wxFrame( (wxFrame *)NULL, wxID_ANY, _T("wxScrolledWindow sample"),
                  wxPoint(20,20), wxSize(800,500) )
{
    wxMenu *file_menu = new wxMenu();
    file_menu->Append( ID_DELETE_ALL, _T("Delete all"));
    file_menu->Append( ID_INSERT_NEW, _T("Insert new"));
    file_menu->Append( ID_ABOUT,      _T("&About.."));
    file_menu->Append( ID_QUIT,       _T("E&xit\tAlt-X"));

    wxMenuBar *menu_bar = new wxMenuBar();
    menu_bar->Append(file_menu, _T("&File"));

    SetMenuBar( menu_bar );

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
    int widths[] = { -1, 100 };
    SetStatusWidths( 2, widths );
#endif // wxUSE_STATUSBAR

     wxBoxSizer *topsizer = new wxBoxSizer( wxHORIZONTAL );
     // subsizer splits topsizer down the middle
     wxBoxSizer *subsizer = new wxBoxSizer( wxVERTICAL );

    // Setting an explicit size here is superfluous, it will be overridden
    // by the sizer in any case.
    m_canvas = new MyCanvas( this, wxID_ANY, wxPoint(0,0), wxSize(100,100) );

    // This is done with ScrollRate/VirtualSize in MyCanvas ctor now,
    // both should produce identical results.
    //m_canvas->SetScrollbars( 10, 10, 50, 100 );

    subsizer->Add( m_canvas, 1, wxEXPAND );
    subsizer->Add( new MyAutoScrollWindow( this ), 1, wxEXPAND );

    wxSizer *sizerBtm = new wxBoxSizer(wxHORIZONTAL);
    sizerBtm->Add( new MyScrolledWindowDumb(this), 1, wxEXPAND );
    sizerBtm->Add( new MyScrolledWindowSmart(this), 1, wxEXPAND );
    subsizer->Add( sizerBtm, 1, wxEXPAND );

    topsizer->Add( subsizer, 1, wxEXPAND );
    topsizer->Add( new MyAutoTimedScrollingWindow( this ), 1, wxEXPAND );

    SetSizer( topsizer );
}

void MyFrame::OnDeleteAll( wxCommandEvent &WXUNUSED(event) )
{
    m_canvas->DestroyChildren();
}

void MyFrame::OnInsertNew( wxCommandEvent &WXUNUSED(event) )
{
    (void)new wxButton( m_canvas, wxID_ANY, _T("Hello"), wxPoint(100,100) );
}

void MyFrame::OnQuit( wxCommandEvent &WXUNUSED(event) )
{
  Close( true );
}

void MyFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
   (void)wxMessageBox( _T("wxScroll demo\n")
                       _T("Robert Roebling (c) 1998\n")
                       _T("Autoscrolling examples\n")
                       _T("Ron Lee (c) 2002\n")
                       _T("Auto-timed-scrolling example\n")
                       _T("Matt Gregory (c) 2003\n"),
                       _T("About wxScroll Demo"),
                       wxICON_INFORMATION | wxOK );
}

//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------

bool MyApp::OnInit()
{
    wxFrame *frame = new MyFrame();
    frame->Show( true );

    frame = new MySimpleFrame();
    frame->Show();

    return true;
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
                                     unsigned(line), y, yPhys), 0, y);
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
                                     unsigned(line), y, yPhys), 0, y);
        y += m_hLine;
    }
}

// ----------------------------------------------------------------------------
// MyAutoTimedScrollingWindow
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyAutoTimedScrollingWindow, wxScrolledWindow)
    EVT_LEFT_DOWN(MyAutoTimedScrollingWindow::OnMouseLeftDown)
    EVT_LEFT_UP(MyAutoTimedScrollingWindow::OnMouseLeftUp)
    EVT_MOTION(MyAutoTimedScrollingWindow::OnMouseMove)
    EVT_SCROLLWIN(MyAutoTimedScrollingWindow::OnScroll)
END_EVENT_TABLE()

MyAutoTimedScrollingWindow::MyAutoTimedScrollingWindow(wxWindow* parent)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize
            //, wxSUNKEN_BORDER) // can't seem to do it this way
            , wxVSCROLL | wxHSCROLL | wxSUNKEN_BORDER)
    , m_selStart(-1, -1), m_cursor(-1, -1)
    , m_font(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL
            , wxFONTWEIGHT_NORMAL)
{
    wxClientDC dc(this);
    // query dc for text size
    dc.SetFont(m_font);
    dc.GetTextExtent(wxString(_T("A")), &m_fontW, &m_fontH);
    // set up the virtual window
    SetScrollbars(m_fontW, m_fontH, sm_lineLen, sm_lineCnt);
}

wxRect MyAutoTimedScrollingWindow::DeviceCoordsToGraphicalChars
        (wxRect updRect) const
{
    wxPoint pos(updRect.GetPosition());
    pos = DeviceCoordsToGraphicalChars(pos);
    updRect.x = pos.x;
    updRect.y = pos.y;
    updRect.width /= m_fontW;
    updRect.height /= m_fontH;
    // the *CoordsToGraphicalChars() funcs round down to upper-left corner,
    //   so an off-by-one correction is needed
    ++updRect.width; // kludge
    ++updRect.height; // kludge
    return updRect;
}

wxPoint MyAutoTimedScrollingWindow::DeviceCoordsToGraphicalChars
        (wxPoint pos) const
{
    pos.x /= m_fontW;
    pos.y /= m_fontH;
    int vX, vY;
    GetViewStart(&vX, &vY);
    pos.x += vX;
    pos.y += vY;
    return pos;
}

wxPoint MyAutoTimedScrollingWindow::GraphicalCharToDeviceCoords
        (wxPoint pos) const
{
    int vX, vY;
    GetViewStart(&vX, &vY);
    pos.x -= vX;
    pos.y -= vY;
    pos.x *= m_fontW;
    pos.y *= m_fontH;
    return pos;
}

wxRect MyAutoTimedScrollingWindow::LogicalCoordsToGraphicalChars
        (wxRect updRect) const
{
    wxPoint pos(updRect.GetPosition());
    pos = LogicalCoordsToGraphicalChars(pos);
    updRect.x = pos.x;
    updRect.y = pos.y;
    updRect.width /= m_fontW;
    updRect.height /= m_fontH;
    // the *CoordsToGraphicalChars() funcs round down to upper-left corner,
    //   so an off-by-one correction is needed
    ++updRect.width; // kludge
    ++updRect.height; // kludge
    return updRect;
}

wxPoint MyAutoTimedScrollingWindow::LogicalCoordsToGraphicalChars
        (wxPoint pos) const
{
    pos.x /= m_fontW;
    pos.y /= m_fontH;
    return pos;
}

wxPoint MyAutoTimedScrollingWindow::GraphicalCharToLogicalCoords
        (wxPoint pos) const
{
    pos.x *= m_fontW;
    pos.y *= m_fontH;
    return pos;
}

void MyAutoTimedScrollingWindow::MyRefresh()
{
    static wxPoint lastSelStart(-1, -1), lastCursor(-1, -1);
    // refresh last selected area (to deselect previously selected text)
    wxRect lastUpdRect(
            GraphicalCharToDeviceCoords(lastSelStart),
            GraphicalCharToDeviceCoords(lastCursor)
        );
    // off-by-one corrections, necessary because it's not possible to know
    //   when to round up until rect is normalized by lastUpdRect constructor
    lastUpdRect.width += m_fontW; // kludge
    lastUpdRect.height += m_fontH; // kludge
    // refresh currently selected (to select previously unselected text)
    wxRect updRect(
            GraphicalCharToDeviceCoords(m_selStart),
            GraphicalCharToDeviceCoords(m_cursor)
        );
    // off-by-one corrections
    updRect.width += m_fontW; // kludge
    updRect.height += m_fontH; // kludge
    // find necessary refresh areas
    wxCoord rx = lastUpdRect.x;
    wxCoord ry = lastUpdRect.y;
    wxCoord rw = updRect.x - lastUpdRect.x;
    wxCoord rh = lastUpdRect.height;
    if (rw && rh) {
        RefreshRect(DCNormalize(rx, ry, rw, rh));
    }
    rx = updRect.x;
    ry = updRect.y + updRect.height;
    rw= updRect.width;
    rh = (lastUpdRect.y + lastUpdRect.height) - (updRect.y + updRect.height);
    if (rw && rh) {
        RefreshRect(DCNormalize(rx, ry, rw, rh));
    }
    rx = updRect.x + updRect.width;
    ry = lastUpdRect.y;
    rw = (lastUpdRect.x + lastUpdRect.width) - (updRect.x + updRect.width);
    rh = lastUpdRect.height;
    if (rw && rh) {
        RefreshRect(DCNormalize(rx, ry, rw, rh));
    }
    rx = updRect.x;
    ry = lastUpdRect.y;
    rw = updRect.width;
    rh = updRect.y - lastUpdRect.y;
    if (rw && rh) {
        RefreshRect(DCNormalize(rx, ry, rw, rh));
    }
    // update last
    lastSelStart = m_selStart;
    lastCursor = m_cursor;
}

bool MyAutoTimedScrollingWindow::IsSelected(int chX, int chY) const
{
    if (IsInside(chX, m_selStart.x, m_cursor.x)
            && IsInside(chY, m_selStart.y, m_cursor.y)) {
        return true;
    }
    return false;
}

bool MyAutoTimedScrollingWindow::IsInside(int k, int bound1, int bound2)
{
    if ((k >= bound1 && k <= bound2) || (k >= bound2 && k <= bound1)) {
        return true;
    }
    return false;
}

wxRect MyAutoTimedScrollingWindow::DCNormalize(wxCoord x, wxCoord y
        , wxCoord w, wxCoord h)
{
    // this is needed to get rid of the graphical remnants from the selection
    // I think it's because DrawRectangle() excludes a pixel in either direction
    const int kludge = 1;
    // make (x, y) the top-left corner
    if (w < 0) {
        w = -w + kludge;
        x -= w;
    } else {
        x -= kludge;
        w += kludge;
    }
    if (h < 0) {
        h = -h + kludge;
        y -= h;
    } else {
        y -= kludge;
        h += kludge;
    }
    return wxRect(x, y, w, h);
}

void MyAutoTimedScrollingWindow::OnDraw(wxDC& dc)
{
    dc.SetFont(m_font);
    wxBrush normBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)
            , wxSOLID);
    wxBrush selBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT)
            , wxSOLID);
    dc.SetPen(*wxTRANSPARENT_PEN);
    wxString str = sm_testData;

    // draw the characters
    // 1. for each update region
    for (wxRegionIterator upd(GetUpdateRegion()); upd; ++upd) {
        wxRect updRect = upd.GetRect();
        wxRect updRectInGChars(DeviceCoordsToGraphicalChars(updRect));
        // 2. for each row of chars in the update region
        for (int chY = updRectInGChars.y
                ; chY <= updRectInGChars.y + updRectInGChars.height; ++chY) {
            // 3. for each character in the row
            for (int chX = updRectInGChars.x
                    ; chX <= updRectInGChars.x + updRectInGChars.width
                    ; ++chX) {
                // 4. set up dc
                if (IsSelected(chX, chY)) {
                    dc.SetBrush(selBrush);
                    dc.SetTextForeground( wxSystemSettings::GetColour
                            (wxSYS_COLOUR_HIGHLIGHTTEXT));
                } else {
                    dc.SetBrush(normBrush);
                    dc.SetTextForeground( wxSystemSettings::GetColour
                            (wxSYS_COLOUR_WINDOWTEXT));
                }
                // 5. find position info
                wxPoint charPos = GraphicalCharToLogicalCoords(wxPoint
                        (chX, chY));
                // 6. draw!
                dc.DrawRectangle(charPos.x, charPos.y, m_fontW, m_fontH);
                size_t charIndex = chY * sm_lineLen + chX;
                if (chY < sm_lineCnt &&
                    chX < sm_lineLen &&
                    charIndex < str.Length())
                {
                    dc.DrawText(str.Mid(charIndex,1),
                                charPos.x, charPos.y);
                }
            }
        }
    }
}

void MyAutoTimedScrollingWindow::OnMouseLeftDown(wxMouseEvent& event)
{
    // initial press of mouse button sets the beginning of the selection
    m_selStart = DeviceCoordsToGraphicalChars(event.GetPosition());
    // set the cursor to the same position
    m_cursor = m_selStart;
    // draw/erase selection
    MyRefresh();
}

void MyAutoTimedScrollingWindow::OnMouseLeftUp(wxMouseEvent& WXUNUSED(event))
{
    // this test is necessary
    if (HasCapture()) {
        // uncapture mouse
        ReleaseMouse();
    }
}

void MyAutoTimedScrollingWindow::OnMouseMove(wxMouseEvent& event)
{
    // if user is dragging
    if (event.Dragging() && event.LeftIsDown()) {
        // set the new cursor position
        m_cursor = DeviceCoordsToGraphicalChars(event.GetPosition());
        // draw/erase selection
        MyRefresh();
        // capture mouse to activate auto-scrolling
        if (!HasCapture()) {
            CaptureMouse();
        }
    }
}

void MyAutoTimedScrollingWindow::OnScroll(wxScrollWinEvent& event)
{
    // need to move the cursor when autoscrolling
    // FIXME: the cursor also moves when the scrollbar arrows are clicked
    if (HasCapture()) {
        if (event.GetOrientation() == wxHORIZONTAL) {
            if (event.GetEventType() == wxEVT_SCROLLWIN_LINEUP) {
                --m_cursor.x;
            } else if (event.GetEventType() == wxEVT_SCROLLWIN_LINEDOWN) {
                ++m_cursor.x;
            }
        } else if (event.GetOrientation() == wxVERTICAL) {
            if (event.GetEventType() == wxEVT_SCROLLWIN_LINEUP) {
                --m_cursor.y;
            } else if (event.GetEventType() == wxEVT_SCROLLWIN_LINEDOWN) {
                ++m_cursor.y;
            }
        }
    }
    MyRefresh();
    event.Skip();
}

const int MyAutoTimedScrollingWindow::sm_lineCnt = 125;
const int MyAutoTimedScrollingWindow::sm_lineLen = 79;
const wxChar* MyAutoTimedScrollingWindow::sm_testData =
_T("162 Cult of the genius out of vanity. Because we think well of ourselves, but ")
_T("nonetheless never suppose ourselves capable of producing a painting like one of ")
_T("Raphael's or a dramatic scene like one of Shakespeare's, we convince ourselves ")
_T("that the capacity to do so is quite extraordinarily marvelous, a wholly ")
_T("uncommon accident, or, if we are still religiously inclined, a mercy from on ")
_T("high. Thus our vanity, our self-love, promotes the cult of the genius: for only ")
_T("if we think of him as being very remote from us, as a miraculum, does he not ")
_T("aggrieve us (even Goethe, who was without envy, called Shakespeare his star of ")
_T("the most distant heights [\"William! Stern der schonsten Ferne\": from Goethe's, ")
_T("\"Between Two Worlds\"]; in regard to which one might recall the lines: \"the ")
_T("stars, these we do not desire\" [from Goethe's, \"Comfort in Tears\"]). But, aside ")
_T("from these suggestions of our vanity, the activity of the genius seems in no ")
_T("way fundamentally different from the activity of the inventor of machines, the ")
_T("scholar of astronomy or history, the master of tactics. All these activities ")
_T("are explicable if one pictures to oneself people whose thinking is active in ")
_T("one direction, who employ everything as material, who always zealously observe ")
_T("their own inner life and that of others, who perceive everywhere models and ")
_T("incentives, who never tire of combining together the means available to them. ")
_T("Genius too does nothing except learn first how to lay bricks then how to build, ")
_T("except continually seek for material and continually form itself around it. ")
_T("Every activity of man is amazingly complicated, not only that of the genius: ")
_T("but none is a \"miracle.\" Whence, then, the belief that genius exists only in ")
_T("the artist, orator and philosopher? that only they have \"intuition\"? (Whereby ")
_T("they are supposed to possess a kind of miraculous eyeglass with which they can ")
_T("see directly into \"the essence of the thing\"!) It is clear that people speak of ")
_T("genius only where the effects of the great intellect are most pleasant to them ")
_T("and where they have no desire to feel envious. To call someone \"divine\" means: ")
_T("\"here there is no need for us to compete.\" Then, everything finished and ")
_T("complete is regarded with admiration, everything still becoming is undervalued. ")
_T("But no one can see in the work of the artist how it has become; that is its ")
_T("advantage, for wherever one can see the act of becoming one grows somewhat ")
_T("cool. The finished and perfect art of representation repulses all thinking as ")
_T("to how it has become; it tyrannizes as present completeness and perfection. ")
_T("That is why the masters of the art of representation count above all as gifted ")
_T("with genius and why men of science do not. In reality, this evaluation of the ")
_T("former and undervaluation of the latter is only a piece of childishness in the ")
_T("realm of reason. ")
_T("\n\n")
_T("163 The serious workman. Do not talk about giftedness, inborn talents! One can ")
_T("name great men of all kinds who were very little gifted. The acquired ")
_T("greatness, became \"geniuses\" (as we put it), through qualities the lack of ")
_T("which no one who knew what they were would boast of: they all possessed that ")
_T("seriousness of the efficient workman which first learns to construct the parts ")
_T("properly before it ventures to fashion a great whole; they allowed themselves ")
_T("time for it, because they took more pleasure in making the little, secondary ")
_T("things well than in the effect of a dazzling whole. the recipe for becoming a ")
_T("good novelist, for example, is easy to give, but to carry it out presupposes ")
_T("qualities one is accustomed to overlook when one says \"I do not have enough ")
_T("talent.\" One has only to make a hundred or so sketches for novels, none longer ")
_T("than two pages but of such distinctness that every word in them is necessary; ")
_T("one should write down anecdotes each day until one has learned how to give them ")
_T("the most pregnant and effective form; one should be tireless in collecting and ")
_T("describing human types and characters; one should above all relate things to ")
_T("others and listen to others relate, keeping one's eyes and ears open for the ")
_T("effect produced on those present, one should travel like a landscape painter or ")
_T("costume designer; one should excerpt for oneself out of the individual sciences ")
_T("everything that will produce an artistic effect when it is well described, one ")
_T("should, finally, reflect on the motives of human actions, disdain no signpost ")
_T("to instruction about them and be a collector of these things by day and night. ")
_T("One should continue in this many-sided exercise some ten years: what is then ")
_T("created in the workshop, however, will be fit to go out into the world. What, ")
_T("however, do most people do? They begin, not with the parts, but with the whole. ")
_T("Perhaps they chance to strike a right note, excite attention and from then on ")
_T("strike worse and worse notes, for good, natural reasons. Sometimes, when the ")
_T("character and intellect needed to formulate such a life-plan are lacking, fate ")
_T("and need take their place and lead the future master step by step through all ")
_T("the stipulations of his trade. ")
_T("\n\n")
_T("164 Peril and profit in the cult of the genius. The belief in great, superior, ")
_T("fruitful spirits is not necessarily, yet nonetheless is very frequently ")
_T("associated with that religious or semi-religious superstition that these ")
_T("spirits are of supra-human origin and possess certain miraculous abilities by ")
_T("virtue of which they acquire their knowledge by quite other means than the rest ")
_T("of mankind. One ascribes to them, it seems, a direct view of the nature of the ")
_T("world, as it were a hole in the cloak of appearance, and believes that, by ")
_T("virtue of this miraculous seer's vision, they are able to communicate something ")
_T("conclusive and decisive about man and the world without the toil and ")
_T("rigorousness required by science. As long as there continue to be those who ")
_T("believe in the miraculous in the domain of knowledge one can perhaps concede ")
_T("that these people themselves derive some benefit from their belief, inasmuch as ")
_T("through their unconditional subjection to the great spirits they create for ")
_T("their own spirit during its time of development the finest form of discipline ")
_T("and schooling. On the other hand, it is at least questionable whether the ")
_T("superstitious belief in genius, in its privileges and special abilities, is of ")
_T("benefit to the genius himself if it takes root in him. It is in any event a ")
_T("dangerous sign when a man is assailed by awe of himself, whether it be the ")
_T("celebrated Caesar's awe of Caesar or the awe of one's own genius now under ")
_T("consideration; when the sacrificial incense which is properly rendered only to ")
_T("a god penetrates the brain of the genius, so that his head begins to swim and ")
_T("he comes to regard himself as something supra-human. The consequences that ")
_T("slowly result are: the feeling of irresponsibility, of exceptional rights, the ")
_T("belief that he confers a favor by his mere presence, insane rage when anyone ")
_T("attempts even to compare him with others, let alone to rate him beneath them, ")
_T("or to draw attention to lapses in his work. Because he ceases to practice ")
_T("criticism of himself, at last one pinion after the other falls out of his ")
_T("plumage: that superstitious eats at the roots of his powers and perhaps even ")
_T("turns him into a hypocrite after his powers have fled from him. For the great ")
_T("spirits themselves it is therefore probably more beneficial if they acquire an ")
_T("insight into the nature and origin of their powers, if they grasp, that is to ")
_T("say, what purely human qualities have come together in them and what fortunate ")
_T("circumstances attended them: in the first place undiminished energy, resolute ")
_T("application to individual goals, great personal courage, then the good fortune ")
_T("to receive an upbringing which offered in the early years the finest teachers, ")
_T("models and methods. To be sure, when their goal is the production of the ")
_T("greatest possible effect, unclarity with regard to oneself and that ")
_T("semi-insanity superadded to it has always achieved much; for what has been ")
_T("admired and envied at all times has been that power in them by virtue of which ")
_T("they render men will-less and sweep them away into the delusion that the ")
_T("leaders they are following are supra-natural. Indeed, it elevates and inspires ")
_T("men to believe that someone is in possession of supra-natural powers: to this ")
_T("extent Plato was right to say [Plato: Phaedrus, 244a] that madness has brought ")
_T("the greatest of blessings upon mankind. In rare individual cases this portion ")
_T("of madness may, indeed, actually have been the means by which such a nature, ")
_T("excessive in all directions, was held firmly together: in the life of ")
_T("individuals, too, illusions that are in themselves poisons often play the role ")
_T("of healers; yet, in the end, in the case of every \"genius\" who believes in his ")
_T("own divinity the poison shows itself to the same degree as his \"genius\" grows ")
_T("old: one may recall, for example, the case of Napoleon, whose nature certainly ")
_T("grew into the mighty unity that sets him apart from all men of modern times ")
_T("precisely through his belief in himself and his star and through the contempt ")
_T("for men that flowed from it; until in the end, however, this same belief went ")
_T("over into an almost insane fatalism, robbed him of his acuteness and swiftness ")
_T("of perception, and became the cause of his destruction.");

/////////////////////////////////////////////////////////////////////////////
// Name:        scroll.cpp
// Purpose:     wxScrolled sample
// Author:      Robert Roebling
// Copyright:   (C) 1998 Robert Roebling, 2002 Ron Lee, 2003 Matt Gregory
//              (C) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/sizer.h"
#include "wx/log.h"
#include "wx/tglbtn.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// a trivial example
// ----------------------------------------------------------------------------

// MySimpleCanvas: a scrolled window which draws a simple rectangle
class MySimpleCanvas : public wxScrolled<wxWindow>
{
public:
    enum
    {
        // these numbers are not multiple of 10 (our scroll step) to test for
        // the absence of rounding errors (e.g. we should have one more page
        // than WIDTH/10 to show the right side of the rectangle)
        WIDTH = 292,
        HEIGHT = 297
    };

    MySimpleCanvas(wxWindow *parent)
        : wxScrolled<wxWindow>(parent, wxID_ANY)
    {
        SetScrollRate( 10, 10 );
        SetVirtualSize( WIDTH, HEIGHT );
        SetBackgroundColour( *wxWHITE );

        Bind(wxEVT_PAINT, &MySimpleCanvas::OnPaint, this);
    }

private:
    void OnPaint(wxPaintEvent& WXUNUSED(event))
    {
        wxPaintDC dc(this);

        // this call is vital: it adjusts the dc to account for the current
        // scroll offset
        PrepareDC(dc);

        dc.SetPen( *wxRED_PEN );
        dc.SetBrush( *wxTRANSPARENT_BRUSH );
        dc.DrawRectangle( 0, 0, WIDTH, HEIGHT );
    }
};


// MySimpleFrame: a frame which contains a MySimpleCanvas
class MySimpleFrame : public wxFrame
{
public:
    MySimpleFrame(wxWindow *parent)
        : wxFrame(parent, wxID_ANY, "MySimpleCanvas")
    {
        new MySimpleCanvas(this);

        // ensure that we have scrollbars initially
        SetClientSize(MySimpleCanvas::WIDTH/2, MySimpleCanvas::HEIGHT/2);

        Show();
    }
};

// ----------------------------------------------------------------------
// a more complex example
// ----------------------------------------------------------------------

// MyCanvas
class MyCanvas : public wxScrolled<wxPanel>
{
public:
    MyCanvas(wxWindow *parent);

private:
    void OnPaint(wxPaintEvent& event);
    void OnQueryPosition(wxCommandEvent& event);
    void OnAddButton(wxCommandEvent& event);
    void OnDeleteButton(wxCommandEvent& event);
    void OnMoveButton(wxCommandEvent& event);
    void OnScrollWin(wxCommandEvent& event);
    void OnMouseRightDown(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);

    wxButton *m_button;

    wxDECLARE_EVENT_TABLE();
};

class MyCanvasFrame : public wxFrame
{
public:
    MyCanvasFrame(wxWindow *parent)
        : wxFrame(parent, wxID_ANY, "MyCanvas")
    {
        m_canvas = new MyCanvas(this);

        wxMenu *menuFile = new wxMenu();
        menuFile->Append(wxID_DELETE, "&Delete all");
        menuFile->Append(wxID_NEW, "Insert &new");

        wxMenuBar *mbar = new wxMenuBar();
        mbar->Append(menuFile, "&File");
        SetMenuBar( mbar );

        Bind(wxEVT_MENU, &MyCanvasFrame::OnDeleteAll, this, wxID_DELETE);
        Bind(wxEVT_MENU, &MyCanvasFrame::OnInsertNew, this, wxID_NEW);

        Show();
    }

private:
    void OnDeleteAll(wxCommandEvent& WXUNUSED(event))
    {
        m_canvas->DestroyChildren();
    }

    void OnInsertNew(wxCommandEvent& WXUNUSED(event))
    {
        (void)new wxButton(m_canvas, wxID_ANY, "Hello", wxPoint(100,100));
    }

    MyCanvas *m_canvas;
};

// ----------------------------------------------------------------------------
// example using sizers with wxScrolled
// ----------------------------------------------------------------------------

const wxSize SMALL_BUTTON( 100, 50 );
const wxSize LARGE_BUTTON( 300, 200 );

class MySizerScrolledWindow : public wxScrolled<wxWindow>
{
public:
    MySizerScrolledWindow(wxWindow *parent);

private:
    // this button can be clicked to change its own size in the handler below,
    // the window size will be automatically adjusted to fit the button
    wxButton *m_button;

    void OnResizeClick(wxCommandEvent& event);
};

class MySizerFrame : public wxFrame
{
public:
    MySizerFrame(wxWindow *parent)
        : wxFrame(parent, wxID_ANY, "MySizerScrolledWindow")
    {
        new MySizerScrolledWindow(this);

        // ensure that the scrollbars appear when the button becomes large
        SetClientSize(LARGE_BUTTON/2);
        Show();
    }
};

// ----------------------------------------------------------------------------
// example showing scrolling only part of the window
// ----------------------------------------------------------------------------

// this window consists of an empty space in its corner, column labels window
// along its top, row labels window along its left hand side and a canvas in
// the remaining space

class MySubColLabels : public wxWindow
{
public:
    MySubColLabels(wxScrolled<wxWindow> *parent)
        : wxWindow(parent, wxID_ANY)
    {
        m_owner = parent;

        Bind(wxEVT_PAINT, &MySubColLabels::OnPaint, this);
    }

private:
    void OnPaint(wxPaintEvent& WXUNUSED(event))
    {
        wxPaintDC dc(this);

        // This is wrong..  it will translate both x and y if the
        // window is scrolled, the label windows are active in one
        // direction only.  Do the action below instead -- RL.
        //m_owner->PrepareDC( dc );

        int xScrollUnits, xOrigin;

        m_owner->GetViewStart( &xOrigin, 0 );
        m_owner->GetScrollPixelsPerUnit( &xScrollUnits, 0 );
        dc.SetDeviceOrigin( -xOrigin * xScrollUnits, 0 );

        dc.DrawText("Column 1", 5, 5);
        dc.DrawText("Column 2", 105, 5);
        dc.DrawText("Column 3", 205, 5);
    }

    wxScrolled<wxWindow> *m_owner;
};

class MySubRowLabels : public wxWindow
{
public:
    MySubRowLabels(wxScrolled<wxWindow> *parent)
        : wxWindow(parent, wxID_ANY)
    {
        m_owner = parent;

        Bind(wxEVT_PAINT, &MySubRowLabels::OnPaint, this);
    }

private:
    void OnPaint(wxPaintEvent& WXUNUSED(event))
    {
        wxPaintDC dc(this);

        // This is wrong..  it will translate both x and y if the
        // window is scrolled, the label windows are active in one
        // direction only.  Do the action below instead -- RL.
        //m_owner->PrepareDC( dc );

        int yScrollUnits, yOrigin;

        m_owner->GetViewStart( 0, &yOrigin );
        m_owner->GetScrollPixelsPerUnit( 0, &yScrollUnits );
        dc.SetDeviceOrigin( 0, -yOrigin * yScrollUnits );

        dc.DrawText("Row 1", 5, 5);
        dc.DrawText("Row 2", 5, 30);
        dc.DrawText("Row 3", 5, 55);
        dc.DrawText("Row 4", 5, 80);
        dc.DrawText("Row 5", 5, 105);
        dc.DrawText("Row 6", 5, 130);
    }

    wxScrolled<wxWindow> *m_owner;
};

class MySubCanvas : public wxPanel
{
public:
    MySubCanvas(wxScrolled<wxWindow> *parent, wxWindow *cols, wxWindow *rows)
        : wxPanel(parent, wxID_ANY)
    {
        m_owner = parent;
        m_colLabels = cols;
        m_rowLabels = rows;

        (void)new wxButton(this, wxID_ANY, "Hallo I",
                           wxPoint(0,50), wxSize(100,25) );
        (void)new wxButton(this, wxID_ANY, "Hallo II",
                           wxPoint(200,50), wxSize(100,25) );

        (void)new wxTextCtrl(this, wxID_ANY, "Text I",
                             wxPoint(0,100), wxSize(100,25) );
        (void)new wxTextCtrl(this, wxID_ANY, "Text II",
                             wxPoint(200,100), wxSize(100,25) );

        (void)new wxComboBox(this, wxID_ANY, "ComboBox I",
                             wxPoint(0,150), wxSize(100,25));
        (void)new wxComboBox(this, wxID_ANY, "ComboBox II",
                             wxPoint(200,150), wxSize(100,25));

        SetBackgroundColour("WHEAT");

        Bind(wxEVT_PAINT, &MySubCanvas::OnPaint, this);
    }

    // override the base class function so that when this window is scrolled,
    // the labels are scrolled in sync
    virtual void ScrollWindow(int dx, int dy, const wxRect *rect) override
    {
        wxPanel::ScrollWindow( dx, dy, rect );
        m_colLabels->ScrollWindow( dx, 0, rect );
        m_rowLabels->ScrollWindow( 0, dy, rect );
    }

private:
    void OnPaint(wxPaintEvent& WXUNUSED(event))
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
        // Is it on screen?
        if ((0+100-scroll_x > 0) && (0+25-scroll_y > 0) &&
            (0-scroll_x < size_x) && (0-scroll_y < size_y))
        {
            // Has the region on screen been exposed?
            if (IsExposed(0,0,100,25))
            {
                dc.DrawRectangle( 0, 0, 100, 25 );
                dc.DrawText("First Cell", 5, 5);
            }
        }


        // Second cell: (200,0)(100,25)
        // Is it on screen?
        if ((200+100-scroll_x > 0) && (0+25-scroll_y > 0) &&
            (200-scroll_x < size_x) && (0-scroll_y < size_y))
        {
            // Has the region on screen been exposed?
            if (IsExposed(200,0,100,25))
            {
                dc.DrawRectangle( 200, 0, 100, 25 );
                dc.DrawText("Second Cell", 205, 5);
            }
        }
    }

    wxScrolled<wxWindow> *m_owner;
    wxWindow *m_colLabels,
             *m_rowLabels;
};

class MySubScrolledWindow : public wxScrolled<wxWindow>
{
public:
    enum
    {
        CORNER_WIDTH = 60,
        CORNER_HEIGHT = 25
    };

    MySubScrolledWindow(wxWindow *parent)
        : wxScrolled<wxWindow>(parent, wxID_ANY)
    {
        // create the children
        MySubColLabels *cols = new MySubColLabels(this);
        MySubRowLabels *rows = new MySubRowLabels(this);

        m_canvas = new MySubCanvas(this, cols, rows);

        // lay them out
        wxFlexGridSizer *sizer = new wxFlexGridSizer(2, 2, 10, 10);
        sizer->Add(CORNER_WIDTH, CORNER_HEIGHT); // just a spacer
        sizer->Add(cols, wxSizerFlags().Expand());
        sizer->Add(rows, wxSizerFlags().Expand());
        sizer->Add(m_canvas, wxSizerFlags().Expand());
        sizer->AddGrowableRow(1);
        sizer->AddGrowableCol(1);
        SetSizer(sizer);

        // this is the key call: it means that only m_canvas will be scrolled
        // and not this window itself
        SetTargetWindow(m_canvas);

        SetScrollbars(10, 10, 50, 50);

        Bind(wxEVT_SIZE, &MySubScrolledWindow::OnSize, this);
    }

protected:
    // scrolled windows which use scroll target different from the window
    // itself must override this virtual method
    virtual wxSize GetSizeAvailableForScrollTarget(const wxSize& size) override
    {
        // decrease the total size by the size of the non-scrollable parts
        // above/to the left of the canvas
        wxSize sizeCanvas(size);
        sizeCanvas.x -= 60;
        sizeCanvas.y -= 25;
        return sizeCanvas;
    }

private:
    void OnSize(wxSizeEvent& WXUNUSED(event))
    {
        // We need to override OnSize so that our scrolled
        // window a) does call Layout() to use sizers for
        // positioning the controls but b) does not query
        // the sizer for their size and use that for setting
        // the scrollable area as set that ourselves by
        // calling SetScrollbar() further down.

        Layout();

        AdjustScrollbars();
    }

    MySubCanvas *m_canvas;
};

class MySubFrame : public wxFrame
{
public:
    MySubFrame(wxWindow *parent)
        : wxFrame(parent, wxID_ANY, "MySubScrolledWindow")
    {
        new MySubScrolledWindow(this);

        Show();
    }
};

// ----------------------------------------------------------------------------
// more simple examples of wxScrolled usage
// ----------------------------------------------------------------------------

// base class for both of them
class MyScrolledWindowBase : public wxScrolled<wxWindow>
{
public:
    MyScrolledWindowBase(wxWindow *parent)
        : wxScrolled<wxWindow>(parent, wxID_ANY,
                               wxDefaultPosition, wxDefaultSize,
                               wxBORDER_SUNKEN)
    {
        m_nLines = 50;
        m_winSync = nullptr;
        m_inDoSync = false;

        wxClientDC dc(this);
        dc.GetTextExtent("Line 17", nullptr, &m_hLine);
    }

    // this scrolled window can be synchronized with another one: if this
    // function is called with a non-null pointer, the given window will be
    // scrolled to the same position as this one
    void SyncWith(MyScrolledWindowBase *win)
    {
        m_winSync = win;

        DoSyncIfNecessary();
    }

    virtual void ScrollWindow(int dx, int dy, const wxRect *rect = nullptr) override
    {
        wxScrolled<wxWindow>::ScrollWindow(dx, dy, rect);

        DoSyncIfNecessary();
    }

protected:
    // the height of one line on screen
    int m_hLine;

    // the number of lines we draw
    size_t m_nLines;

private:
    bool WasScrolledFirst() const { return m_inDoSync; }

    void DoSyncIfNecessary()
    {
        if ( m_winSync && !m_winSync->WasScrolledFirst() )
        {
            m_inDoSync = true;

            m_winSync->Scroll(GetViewStart());

            m_inDoSync = false;
        }
    }

    // the window to synchronize with this one or nullptr
    MyScrolledWindowBase *m_winSync;

    // the flag preventing infinite recursion which would otherwise happen if
    // one window synchronized the other one which in turn synchronized this
    // one and so on
    bool m_inDoSync;
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

    virtual void OnDraw(wxDC& dc) override;
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

    virtual void OnDraw(wxDC& dc) override;
};

// ----------------------------------------------------------------------------
// implements a text viewer with simple block selection to test auto-scrolling
// functionality
// ----------------------------------------------------------------------------

class MyAutoScrollingWindow : public wxScrolled<wxWindow>
{
public:
    MyAutoScrollingWindow( wxWindow* parent );
    wxRect DeviceCoordsToGraphicalChars(wxRect updRect) const;
    wxPoint DeviceCoordsToGraphicalChars(wxPoint pos) const;
    wxPoint GraphicalCharToDeviceCoords(wxPoint pos) const;
    wxRect LogicalCoordsToGraphicalChars(wxRect updRect) const;
    wxPoint LogicalCoordsToGraphicalChars(wxPoint pos) const;
    wxPoint GraphicalCharToLogicalCoords(wxPoint pos) const;
    void MyRefresh();
    bool IsSelected(int chX, int chY) const;
    static bool IsInside(int k, int bound1, int bound2);
    static wxRect DCNormalize(int x, int y, int w, int h);

private:
    // event handlers
    void OnDraw(wxDC& dc) override;
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);
    void OnScroll(wxScrollWinEvent& event);

    // test data variables
    static const char* sm_testData;
    static const int sm_lineCnt; // line count
    static const int sm_lineLen; // line length in characters
    // sizes for graphical data
    int m_fontH, m_fontW;
    // selection tracking
    wxPoint m_selStart; // beginning of blockwise selection
    wxPoint m_cursor;   // end of blockwise selection (mouse position)

    // gui stuff
    wxFont m_font;


    wxDECLARE_EVENT_TABLE();
};

class MyAutoFrame : public wxFrame
{
public:
    MyAutoFrame(wxWindow *parent)
        : wxFrame(parent, wxID_ANY, "MyAutoScrollingWindow")
    {
        new MyAutoScrollingWindow(this);

        Show();
    }
};


// ----------------------------------------------------------------------------
// MyFrame: the main application frame showing all the classes above
// ----------------------------------------------------------------------------

class MyFrame: public wxFrame
{
public:
    MyFrame();

private:
    void OnAbout(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);

    void OnTestSimple(wxCommandEvent& WXUNUSED(event)) { new MySimpleFrame(this); }
    void OnTestCanvas(wxCommandEvent& WXUNUSED(event)) { new MyCanvasFrame(this); }
    void OnTestSizer(wxCommandEvent& WXUNUSED(event)) { new MySizerFrame(this); }
    void OnTestSub(wxCommandEvent& WXUNUSED(event)) { new MySubFrame(this); }
    void OnTestAuto(wxCommandEvent& WXUNUSED(event)) { new MyAutoFrame(this); }

    void OnToggleSync(wxCommandEvent& event);
    void OnScrollbarVisibility(wxCommandEvent& event);

    MyScrolledWindowBase *m_win1,
                         *m_win2;

    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    virtual bool OnInit() override;
};


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyCanvas
// ----------------------------------------------------------------------------

const wxWindowIDRef ID_ADDBUTTON   = wxWindow::NewControlId();
const wxWindowIDRef ID_DELBUTTON   = wxWindow::NewControlId();
const wxWindowIDRef ID_MOVEBUTTON  = wxWindow::NewControlId();
const wxWindowIDRef ID_SCROLLWIN   = wxWindow::NewControlId();
const wxWindowIDRef ID_QUERYPOS    = wxWindow::NewControlId();

const wxWindowIDRef ID_NEWBUTTON   = wxWindow::NewControlId();

wxBEGIN_EVENT_TABLE(MyCanvas, wxScrolled<wxPanel>)
    EVT_PAINT(                  MyCanvas::OnPaint)
    EVT_RIGHT_DOWN(             MyCanvas::OnMouseRightDown)
    EVT_MOUSEWHEEL(             MyCanvas::OnMouseWheel)
    EVT_BUTTON( ID_QUERYPOS,    MyCanvas::OnQueryPosition)
    EVT_BUTTON( ID_ADDBUTTON,   MyCanvas::OnAddButton)
    EVT_BUTTON( ID_DELBUTTON,   MyCanvas::OnDeleteButton)
    EVT_BUTTON( ID_MOVEBUTTON,  MyCanvas::OnMoveButton)
    EVT_BUTTON( ID_SCROLLWIN,   MyCanvas::OnScrollWin)
wxEND_EVENT_TABLE()

MyCanvas::MyCanvas(wxWindow *parent)
    : wxScrolled<wxPanel>(parent, wxID_ANY,
                          wxDefaultPosition, wxDefaultSize,
                          wxSUNKEN_BORDER | wxTAB_TRAVERSAL)
{
    // you can use either a single SetScrollbars() call or these 2 functions,
    // usually using them is better because you normally won't need to change
    // the scroll rate in the future and the sizer can be used to update the
    // virtual size automatically
    SetScrollRate( 10, 10 );
    SetVirtualSize( 500, 1000 );

    (void) new wxButton( this, ID_ADDBUTTON,  "add button", wxPoint(10,10) );
    (void) new wxButton( this, ID_DELBUTTON,  "del button", wxPoint(10,40) );
    (void) new wxButton( this, ID_MOVEBUTTON, "move button", wxPoint(150,10) );
    (void) new wxButton( this, ID_SCROLLWIN,  "scroll win", wxPoint(250,10) );

    wxPanel *test = new wxPanel( this, wxID_ANY,
                                 wxPoint(10, 110), wxSize(130,50),
                                 wxSIMPLE_BORDER | wxTAB_TRAVERSAL );
    test->SetBackgroundColour( "WHEAT" );

    SetBackgroundColour( "BLUE" );
}

void MyCanvas::OnMouseRightDown( wxMouseEvent &event )
{
    wxPoint pt( event.GetPosition() );
    int x,y;
    CalcUnscrolledPosition( pt.x, pt.y, &x, &y );
    wxLogMessage("Mouse down event at: %d %d, scrolled: %d %d",
                 pt.x, pt.y, x, y);
}

void MyCanvas::OnMouseWheel( wxMouseEvent &event )
{
    wxPoint pt( event.GetPosition() );
    int x,y;
    CalcUnscrolledPosition( pt.x, pt.y, &x, &y );
    wxLogMessage( "Mouse wheel event at: %d %d, scrolled: %d %d\n"
                  "Rotation: %d, delta: %d, inverted: %d",
                  pt.x, pt.y, x, y,
                  event.GetWheelRotation(), event.GetWheelDelta(),
                  event.IsWheelInverted() );

    event.Skip();
}

void MyCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );
    PrepareDC( dc );

    dc.DrawText( "Press right mouse button to test calculations!", 160, 50 );

    dc.DrawText( "Some text", 140, 140 );

    dc.DrawRectangle( 100, 160, 200, 200 );
}

void MyCanvas::OnQueryPosition( wxCommandEvent &WXUNUSED(event) )
{
    wxPoint pt( m_button->GetPosition() );
    wxLogMessage( "Position of \"Query position\" is %d %d", pt.x, pt.y );
    pt = ClientToScreen( pt );
    wxLogMessage("Position of \"Query position\" on screen is %d %d",
                 pt.x, pt.y);
}

void MyCanvas::OnAddButton( wxCommandEvent &WXUNUSED(event) )
{
    wxLogMessage( "Inserting button at position 10,70..." );
    wxButton *button = new wxButton( this, ID_NEWBUTTON, "new button",
                                     wxPoint(10,70), wxSize(80,25) );
    wxPoint pt( button->GetPosition() );
    wxLogMessage( "-> Position after inserting %d %d", pt.x, pt.y );
}

void MyCanvas::OnDeleteButton( wxCommandEvent &WXUNUSED(event) )
{
    wxLogMessage( "Deleting button inserted with \"Add button\"..." );
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
    win->Move( wxDefaultCoord, pt.y + 10 );
    pt = win->GetPosition();
    wxLogMessage( "-> Position after move is %d %d", pt.x, pt.y );
}

void MyCanvas::OnScrollWin( wxCommandEvent &WXUNUSED(event) )
{
    wxLogMessage("Scrolling 2 units up.\n"
                 "The white square and the controls should move equally!");
    Scroll( wxDefaultCoord, GetViewStart().y+2 );
}

// ----------------------------------------------------------------------------
// MySizerScrolledWindow
// ----------------------------------------------------------------------------

MySizerScrolledWindow::MySizerScrolledWindow(wxWindow *parent)
    : wxScrolled<wxWindow>(parent)
{
    SetBackgroundColour( "GREEN" );

    // Set the rate we'd like for scrolling.

    SetScrollRate( 5, 5 );

    // Populate a sizer with a 'resizing' button and some other static
    // decoration

    wxFlexGridSizer *sizer = new wxFlexGridSizer(2);

    m_button = new wxButton( this, wxID_RESIZE_FRAME, "Press me",
                             wxDefaultPosition, SMALL_BUTTON );

    sizer->Add(m_button, wxSizerFlags().Centre().Border(wxALL, 20));
    sizer->Add(new wxStaticText(this, wxID_ANY, "This is just"),
               wxSizerFlags().Centre());
    sizer->Add(new wxStaticText(this, wxID_ANY, "some decoration"),
               wxSizerFlags().Centre());
    sizer->Add(new wxStaticText(this, wxID_ANY, "for you to scroll..."),
               wxSizerFlags().Centre());

    // Then use the sizer to set the scrolled region size.

    SetSizer( sizer );

    Bind(wxEVT_BUTTON, &MySizerScrolledWindow::OnResizeClick, this,
         wxID_RESIZE_FRAME);
}

void MySizerScrolledWindow::OnResizeClick(wxCommandEvent &WXUNUSED(event))
{
    // Arbitrarily resize the button to change the minimum size of
    // the (scrolled) sizer.

    if ( m_button->GetSize() == SMALL_BUTTON )
        m_button->SetSizeHints(LARGE_BUTTON);
    else
        m_button->SetSizeHints(SMALL_BUTTON);

    // Force update layout and scrollbars, since nothing we do here
    // necessarily generates a size event which would do it for us.
    FitInside();
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

const wxWindowID Scroll_Test_Simple = wxWindow::NewControlId();
const wxWindowID Scroll_Test_Canvas = wxWindow::NewControlId();
const wxWindowID Scroll_Test_Sizers = wxWindow::NewControlId();
const wxWindowID Scroll_Test_Sub    = wxWindow::NewControlId();
const wxWindowID Scroll_Test_Auto   = wxWindow::NewControlId();

const wxWindowID Scroll_TglBtn_Sync = wxWindow::NewControlId();
const wxWindowID Scroll_Radio_ShowScrollbar = wxWindow::NewControlId();

wxBEGIN_EVENT_TABLE(MyFrame,wxFrame)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_MENU(wxID_EXIT,  MyFrame::OnQuit)

    EVT_MENU(Scroll_Test_Simple, MyFrame::OnTestSimple)
    EVT_MENU(Scroll_Test_Canvas, MyFrame::OnTestCanvas)
    EVT_MENU(Scroll_Test_Sizers, MyFrame::OnTestSizer)
    EVT_MENU(Scroll_Test_Sub, MyFrame::OnTestSub)
    EVT_MENU(Scroll_Test_Auto, MyFrame::OnTestAuto)

    EVT_TOGGLEBUTTON(Scroll_TglBtn_Sync, MyFrame::OnToggleSync)
    EVT_RADIOBOX(Scroll_Radio_ShowScrollbar, MyFrame::OnScrollbarVisibility)
wxEND_EVENT_TABLE()

MyFrame::MyFrame()
       : wxFrame(nullptr, wxID_ANY, "wxWidgets scroll sample")
{
    SetIcon(wxICON(sample));

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(wxID_ABOUT, "&About..");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, "E&xit\tAlt-X");

    wxMenu *menuTest = new wxMenu;
    menuTest->Append(Scroll_Test_Simple, "&Simple scroll window\tF1",
                     "Simplest possible scrolled window test.");
    menuTest->Append(Scroll_Test_Canvas, "Scrolled window with &children\tF2",
                     "Scrolled window with controls on it.");
    menuTest->Append(Scroll_Test_Sizers, "Scrolled window with si&zer\tF3",
                     "Scrolled window with children managed by sizer.");
    menuTest->Append(Scroll_Test_Sub, "Scrolled s&ub-window\tF4",
                     "Window only part of which is scrolled.");
    menuTest->Append(Scroll_Test_Auto, "&Auto-scrolled window\tF5",
                     "Window which scrolls when the mouse is held pressed "
                     "outside of it.");

    wxMenuBar *mbar = new wxMenuBar;
    mbar->Append(menuFile, "&File");
    mbar->Append(menuTest, "&Test");

    SetMenuBar( mbar );


    wxPanel *panel = new wxPanel(this);

    const wxSizerFlags flagsExpand(wxSizerFlags(1).Expand());

    wxSizer *topsizer = new wxBoxSizer(wxVERTICAL);
    topsizer->Add(new wxStaticText(panel, wxID_ANY,
        "The windows below should behave in the same way, even though\n"
        "they're implemented quite differently, see the code for details.\n"
        "\n"
        "The lines redrawn during odd/even repaint iterations are drawn in\n"
        "red/blue colour to allow seeing immediately how much is repainted,\n"
        "don't be surprised by this."),
        wxSizerFlags().Centre().Border());

    m_win1 = new MyScrolledWindowDumb(panel);
    m_win2 = new MyScrolledWindowSmart(panel);

    wxSizer *sizerScrollWin = new wxBoxSizer(wxHORIZONTAL);
    sizerScrollWin->Add(m_win1, flagsExpand);
    sizerScrollWin->Add(m_win2, flagsExpand);
    topsizer->Add(sizerScrollWin, flagsExpand);

    const wxSizerFlags
        flagsHBorder(wxSizerFlags().Centre().Border(wxLEFT | wxRIGHT));

    wxSizer *sizerBtns = new wxBoxSizer(wxHORIZONTAL);

    // the radio buttons are in the same order as wxSHOW_SB_XXX values but
    // offset by 1
    const wxString visibilities[] = { "&never", "&default", "&always" };
    wxRadioBox *radio = new wxRadioBox(panel, Scroll_Radio_ShowScrollbar,
                                       "Left &scrollbar visibility: ",
                                       wxDefaultPosition, wxDefaultSize,
                                       WXSIZEOF(visibilities), visibilities);
    radio->SetSelection(wxSHOW_SB_DEFAULT + 1);
    sizerBtns->Add(radio, flagsHBorder);

    sizerBtns->Add(new wxToggleButton(panel, Scroll_TglBtn_Sync, "S&ynchronize"),
                   flagsHBorder);

    topsizer->Add(sizerBtns, wxSizerFlags().Centre().Border());

    panel->SetSizer(topsizer);

    wxSize size = panel->GetBestSize();
    SetSizeHints(size);
    SetClientSize(2*size);

    Show();
}

void MyFrame::OnToggleSync(wxCommandEvent& event)
{
    if ( event.IsChecked() )
    {
        m_win1->SyncWith(m_win2);
        m_win2->SyncWith(m_win1);
    }
    else
    {
        m_win1->SyncWith(nullptr);
        m_win2->SyncWith(nullptr);
    }
}

void MyFrame::OnScrollbarVisibility(wxCommandEvent& event)
{
    m_win1->ShowScrollbars(wxSHOW_SB_NEVER,
                           wxScrollbarVisibility(event.GetSelection() - 1));
}

void MyFrame::OnQuit(wxCommandEvent &WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
    (void)wxMessageBox( "Scrolled window sample\n"
                        "\n"
                        "Robert Roebling (c) 1998\n"
                        "Vadim Zeitlin (c) 2008\n"
                        "Autoscrolling examples\n"
                        "Ron Lee (c) 2002\n"
                        "Auto-timed-scrolling example\n"
                        "Matt Gregory (c) 2003\n",
                        "About wxWidgets scroll sample",
                        wxICON_INFORMATION | wxOK );
}

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    new MyFrame();

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

    int y = 0;
    for ( size_t line = 0; line < m_nLines; line++ )
    {
        int yPhys;
        CalcScrolledPosition(0, y, nullptr, &yPhys);

        dc.DrawText(wxString::Format("Line %u (logical %d, physical %d)",
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

    int y = lineFrom*m_hLine;
    for ( size_t line = lineFrom; line <= lineTo; line++ )
    {
        int yPhys;
        CalcScrolledPosition(0, y, nullptr, &yPhys);

        dc.DrawText(wxString::Format("Line %u (logical %d, physical %d)",
                                     unsigned(line), y, yPhys), 0, y);
        y += m_hLine;
    }
}

// ----------------------------------------------------------------------------
// MyAutoScrollingWindow
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyAutoScrollingWindow, wxScrolled<wxWindow>)
    EVT_LEFT_DOWN(MyAutoScrollingWindow::OnMouseLeftDown)
    EVT_LEFT_UP(MyAutoScrollingWindow::OnMouseLeftUp)
    EVT_MOTION(MyAutoScrollingWindow::OnMouseMove)
    EVT_MOUSE_CAPTURE_LOST(MyAutoScrollingWindow::OnMouseCaptureLost)
    EVT_SCROLLWIN(MyAutoScrollingWindow::OnScroll)
wxEND_EVENT_TABLE()

MyAutoScrollingWindow::MyAutoScrollingWindow(wxWindow* parent)
    : wxScrolled<wxWindow>(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                           wxVSCROLL | wxHSCROLL | wxSUNKEN_BORDER),
      m_selStart(-1, -1),
      m_cursor(-1, -1),
      m_font(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL)
{
    wxClientDC dc(this);
    // query dc for text size
    dc.SetFont(m_font);
    dc.GetTextExtent(wxString("A"), &m_fontW, &m_fontH);
    // set up the virtual window
    SetScrollbars(m_fontW, m_fontH, sm_lineLen, sm_lineCnt);
}

wxRect
MyAutoScrollingWindow::DeviceCoordsToGraphicalChars(wxRect updRect) const
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

wxPoint
MyAutoScrollingWindow::DeviceCoordsToGraphicalChars(wxPoint pos) const
{
    pos.x /= m_fontW;
    pos.y /= m_fontH;
    pos += GetViewStart();
    return pos;
}

wxPoint
MyAutoScrollingWindow::GraphicalCharToDeviceCoords(wxPoint pos) const
{
    pos -= GetViewStart();
    pos.x *= m_fontW;
    pos.y *= m_fontH;
    return pos;
}

wxRect
MyAutoScrollingWindow::LogicalCoordsToGraphicalChars(wxRect updRect) const
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

wxPoint
MyAutoScrollingWindow::LogicalCoordsToGraphicalChars(wxPoint pos) const
{
    pos.x /= m_fontW;
    pos.y /= m_fontH;
    return pos;
}

wxPoint
MyAutoScrollingWindow::GraphicalCharToLogicalCoords(wxPoint pos) const
{
    pos.x *= m_fontW;
    pos.y *= m_fontH;
    return pos;
}

void MyAutoScrollingWindow::MyRefresh()
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
    int rx = lastUpdRect.x;
    int ry = lastUpdRect.y;
    int rw = updRect.x - lastUpdRect.x;
    int rh = lastUpdRect.height;
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

bool MyAutoScrollingWindow::IsSelected(int chX, int chY) const
{
    if (IsInside(chX, m_selStart.x, m_cursor.x)
            && IsInside(chY, m_selStart.y, m_cursor.y)) {
        return true;
    }
    return false;
}

bool MyAutoScrollingWindow::IsInside(int k, int bound1, int bound2)
{
    if ((k >= bound1 && k <= bound2) || (k >= bound2 && k <= bound1)) {
        return true;
    }
    return false;
}

wxRect
MyAutoScrollingWindow::DCNormalize(int x, int y, int w, int h)
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

void MyAutoScrollingWindow::OnDraw(wxDC& dc)
{
    dc.SetFont(m_font);
    wxBrush normBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    wxBrush selBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    dc.SetPen(*wxTRANSPARENT_PEN);
    const wxString str = sm_testData;
    size_t strLength = str.length();
    wxString::const_iterator str_i = str.begin();

    // draw the characters
    // 1. for each update region
    for (wxRegionIterator upd(GetUpdateRegion()); upd; ++upd) {
        wxRect updRect = upd.GetRect();
        wxRect updRectInGChars(DeviceCoordsToGraphicalChars(updRect));
        // 2. for each row of chars in the update region
        for (int chY = updRectInGChars.y
                ; chY <= updRectInGChars.y + updRectInGChars.height; ++chY) {
            // 3. for each character in the row
            bool isFirstX = true;
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
                    charIndex < strLength)
                {
                    if (isFirstX)
                    {
                        str_i = str.begin() + charIndex;
                        isFirstX = false;
                    }
                    dc.DrawText(*str_i, charPos.x, charPos.y);
                    ++str_i;
                }
            }
        }
    }
}

void MyAutoScrollingWindow::OnMouseLeftDown(wxMouseEvent& event)
{
    // initial press of mouse button sets the beginning of the selection
    m_selStart = DeviceCoordsToGraphicalChars(event.GetPosition());
    // set the cursor to the same position
    m_cursor = m_selStart;
    // draw/erase selection
    MyRefresh();
}

void MyAutoScrollingWindow::OnMouseLeftUp(wxMouseEvent& WXUNUSED(event))
{
    // this test is necessary
    if (HasCapture()) {
        // uncapture mouse
        ReleaseMouse();
    }
}

void MyAutoScrollingWindow::OnMouseMove(wxMouseEvent& event)
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

void
MyAutoScrollingWindow::OnMouseCaptureLost(wxMouseCaptureLostEvent&
                                                WXUNUSED(event))
{
    // we only capture mouse for timed scrolling, so nothing is needed here
    // other than making sure to not call event.Skip()
}

void MyAutoScrollingWindow::OnScroll(wxScrollWinEvent& event)
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

const int MyAutoScrollingWindow::sm_lineCnt = 125;
const int MyAutoScrollingWindow::sm_lineLen = 79;
const char *MyAutoScrollingWindow::sm_testData =
"162 Cult of the genius out of vanity. Because we think well of ourselves, but "
"nonetheless never suppose ourselves capable of producing a painting like one of "
"Raphael's or a dramatic scene like one of Shakespeare's, we convince ourselves "
"that the capacity to do so is quite extraordinarily marvelous, a wholly "
"uncommon accident, or, if we are still religiously inclined, a mercy from on "
"high. Thus our vanity, our self-love, promotes the cult of the genius: for only "
"if we think of him as being very remote from us, as a miraculum, does he not "
"aggrieve us (even Goethe, who was without envy, called Shakespeare his star of "
"the most distant heights [\"William! Stern der schonsten Ferne\": from Goethe's, "
"\"Between Two Worlds\"]; in regard to which one might recall the lines: \"the "
"stars, these we do not desire\" [from Goethe's, \"Comfort in Tears\"]). But, aside "
"from these suggestions of our vanity, the activity of the genius seems in no "
"way fundamentally different from the activity of the inventor of machines, the "
"scholar of astronomy or history, the master of tactics. All these activities "
"are explicable if one pictures to oneself people whose thinking is active in "
"one direction, who employ everything as material, who always zealously observe "
"their own inner life and that of others, who perceive everywhere models and "
"incentives, who never tire of combining together the means available to them. "
"Genius too does nothing except learn first how to lay bricks then how to build, "
"except continually seek for material and continually form itself around it. "
"Every activity of man is amazingly complicated, not only that of the genius: "
"but none is a \"miracle.\" Whence, then, the belief that genius exists only in "
"the artist, orator and philosopher? that only they have \"intuition\"? (Whereby "
"they are supposed to possess a kind of miraculous eyeglass with which they can "
"see directly into \"the essence of the thing\"!) It is clear that people speak of "
"genius only where the effects of the great intellect are most pleasant to them "
"and where they have no desire to feel envious. To call someone \"divine\" means: "
"\"here there is no need for us to compete.\" Then, everything finished and "
"complete is regarded with admiration, everything still becoming is undervalued. "
"But no one can see in the work of the artist how it has become; that is its "
"advantage, for wherever one can see the act of becoming one grows somewhat "
"cool. The finished and perfect art of representation repulses all thinking as "
"to how it has become; it tyrannizes as present completeness and perfection. "
"That is why the masters of the art of representation count above all as gifted "
"with genius and why men of science do not. In reality, this evaluation of the "
"former and undervaluation of the latter is only a piece of childishness in the "
"realm of reason. "
"\n\n"
"163 The serious workman. Do not talk about giftedness, inborn talents! One can "
"name great men of all kinds who were very little gifted. The acquired "
"greatness, became \"geniuses\" (as we put it), through qualities the lack of "
"which no one who knew what they were would boast of: they all possessed that "
"seriousness of the efficient workman which first learns to construct the parts "
"properly before it ventures to fashion a great whole; they allowed themselves "
"time for it, because they took more pleasure in making the little, secondary "
"things well than in the effect of a dazzling whole. the recipe for becoming a "
"good novelist, for example, is easy to give, but to carry it out presupposes "
"qualities one is accustomed to overlook when one says \"I do not have enough "
"talent.\" One has only to make a hundred or so sketches for novels, none longer "
"than two pages but of such distinctness that every word in them is necessary; "
"one should write down anecdotes each day until one has learned how to give them "
"the most pregnant and effective form; one should be tireless in collecting and "
"describing human types and characters; one should above all relate things to "
"others and listen to others relate, keeping one's eyes and ears open for the "
"effect produced on those present, one should travel like a landscape painter or "
"costume designer; one should excerpt for oneself out of the individual sciences "
"everything that will produce an artistic effect when it is well described, one "
"should, finally, reflect on the motives of human actions, disdain no signpost "
"to instruction about them and be a collector of these things by day and night. "
"One should continue in this many-sided exercise some ten years: what is then "
"created in the workshop, however, will be fit to go out into the world. What, "
"however, do most people do? They begin, not with the parts, but with the whole. "
"Perhaps they chance to strike a right note, excite attention and from then on "
"strike worse and worse notes, for good, natural reasons. Sometimes, when the "
"character and intellect needed to formulate such a life-plan are lacking, fate "
"and need take their place and lead the future master step by step through all "
"the stipulations of his trade. "
"\n\n"
"164 Peril and profit in the cult of the genius. The belief in great, superior, "
"fruitful spirits is not necessarily, yet nonetheless is very frequently "
"associated with that religious or semi-religious superstition that these "
"spirits are of supra-human origin and possess certain miraculous abilities by "
"virtue of which they acquire their knowledge by quite other means than the rest "
"of mankind. One ascribes to them, it seems, a direct view of the nature of the "
"world, as it were a hole in the cloak of appearance, and believes that, by "
"virtue of this miraculous seer's vision, they are able to communicate something "
"conclusive and decisive about man and the world without the toil and "
"rigorousness required by science. As long as there continue to be those who "
"believe in the miraculous in the domain of knowledge one can perhaps concede "
"that these people themselves derive some benefit from their belief, inasmuch as "
"through their unconditional subjection to the great spirits they create for "
"their own spirit during its time of development the finest form of discipline "
"and schooling. On the other hand, it is at least questionable whether the "
"superstitious belief in genius, in its privileges and special abilities, is of "
"benefit to the genius himself if it takes root in him. It is in any event a "
"dangerous sign when a man is assailed by awe of himself, whether it be the "
"celebrated Caesar's awe of Caesar or the awe of one's own genius now under "
"consideration; when the sacrificial incense which is properly rendered only to "
"a god penetrates the brain of the genius, so that his head begins to swim and "
"he comes to regard himself as something supra-human. The consequences that "
"slowly result are: the feeling of irresponsibility, of exceptional rights, the "
"belief that he confers a favor by his mere presence, insane rage when anyone "
"attempts even to compare him with others, let alone to rate him beneath them, "
"or to draw attention to lapses in his work. Because he ceases to practice "
"criticism of himself, at last one pinion after the other falls out of his "
"plumage: that superstitious eats at the roots of his powers and perhaps even "
"turns him into a hypocrite after his powers have fled from him. For the great "
"spirits themselves it is therefore probably more beneficial if they acquire an "
"insight into the nature and origin of their powers, if they grasp, that is to "
"say, what purely human qualities have come together in them and what fortunate "
"circumstances attended them: in the first place undiminished energy, resolute "
"application to individual goals, great personal courage, then the good fortune "
"to receive an upbringing which offered in the early years the finest teachers, "
"models and methods. To be sure, when their goal is the production of the "
"greatest possible effect, unclarity with regard to oneself and that "
"semi-insanity superadded to it has always achieved much; for what has been "
"admired and envied at all times has been that power in them by virtue of which "
"they render men will-less and sweep them away into the delusion that the "
"leaders they are following are supra-natural. Indeed, it elevates and inspires "
"men to believe that someone is in possession of supra-natural powers: to this "
"extent Plato was right to say [Plato: Phaedrus, 244a] that madness has brought "
"the greatest of blessings upon mankind. In rare individual cases this portion "
"of madness may, indeed, actually have been the means by which such a nature, "
"excessive in all directions, was held firmly together: in the life of "
"individuals, too, illusions that are in themselves poisons often play the role "
"of healers; yet, in the end, in the case of every \"genius\" who believes in his "
"own divinity the poison shows itself to the same degree as his \"genius\" grows "
"old: one may recall, for example, the case of Napoleon, whose nature certainly "
"grew into the mighty unity that sets him apart from all men of modern times "
"precisely through his belief in himself and his star and through the contempt "
"for men that flowed from it; until in the end, however, this same belief went "
"over into an almost insane fatalism, robbed him of his acuteness and swiftness "
"of perception, and became the cause of his destruction.";

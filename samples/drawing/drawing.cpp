/////////////////////////////////////////////////////////////////////////////
// Name:        drawing.cpp
// Purpose:     shows and tests wxDC features
// Author:      Robert Roebling
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifdef __GNUG__
    #pragma implementation "drawing.cpp"
    #pragma interface "drawing.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/colordlg.h"

// ----------------------------------------------------------------------------
// ressources
// ----------------------------------------------------------------------------
// the application icon
#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "mondrian.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

class MyCanvas;

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnOption(wxCommandEvent &event);
    void OnMouseMove(wxMouseEvent &event);

    wxColour SelectColour();
    void PrepareDC(wxDC& dc);

    int         m_backgroundMode;
    int         m_mapMode;
    double      m_xUserScale;
    double      m_yUserScale;
    int         m_xLogicalOrigin;
    int         m_yLogicalOrigin;
    bool        m_xAxisReversed,
                m_yAxisReversed;
    wxColour    m_colourForeground,    // these are _text_ colours
                m_colourBackground;
    wxBrush     m_backgroundBrush;
    MyCanvas   *m_canvas;

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// define a scrollable canvas for drawing onto
class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas( MyFrame *parent );
    
    void DoDrawTests( int x, int y, wxDC &dc );
    void OnPaint(wxPaintEvent &event);
    
protected:
    MyFrame *m_owner;
    
private:
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = 1,
    Minimal_About,

    MenuOption_First,

    MapMode_Text = MenuOption_First,
    MapMode_Lometric,
    MapMode_Twips,
    MapMode_Points,
    MapMode_Metric,

    UserScale_StretchHoriz,
    UserScale_ShrinkHoriz,
    UserScale_StretchVertic,
    UserScale_ShrinkVertic,
    UserScale_Restore,

    AxisMirror_Horiz,
    AxisMirror_Vertic,

    LogicalOrigin_MoveDown,
    LogicalOrigin_MoveUp,
    LogicalOrigin_MoveLeft,
    LogicalOrigin_MoveRight,

    Colour_TextForeground,
    Colour_TextBackground,
    Colour_Background,
    Colour_BackgroundMode,

    MenuOption_Last = Colour_BackgroundMode
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------


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

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // Create the main application window
    MyFrame *frame = new MyFrame("Drawing sample",
                                 wxPoint(50, 50), wxSize(550, 340));

    // Show it and tell the application that it's our main window
    frame->Show(TRUE);
    SetTopWindow(frame);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}

// ----------------------------------------------------------------------------
// MyCanvas
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them.
BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT  (MyCanvas::OnPaint)
END_EVENT_TABLE()

MyCanvas::MyCanvas( MyFrame *parent )
  : wxScrolledWindow( parent )
{
  m_owner = parent;
}

void MyCanvas::DoDrawTests( int x, int y, wxDC &dc )
{
   wxRect rect;
   rect.x = dc.LogicalToDeviceX( x-20 );
   rect.y = dc.LogicalToDeviceY( y-20 );
   rect.width = dc.LogicalToDeviceXRel( 500 );
   rect.height = dc.LogicalToDeviceYRel( 200 );
   if (!IsExposed(rect)) return;

   dc.DrawLine( x    , y, x    , y    );
   dc.DrawLine( x+ 20, y, x+ 20, y+ 1 );
   dc.DrawLine( x+ 40, y, x+ 40, y+ 2 );
   dc.DrawLine( x+ 60, y, x+ 60, y+ 3 );
   dc.DrawLine( x+ 80, y, x+ 80, y+ 4 );
   dc.DrawLine( x+100, y, x+100, y+ 5 );
   dc.DrawLine( x+120, y, x+120, y+ 6 );
   dc.DrawLine( x+140, y, x+140, y+10 );
   dc.DrawLine( x+160, y, x+160, y+20 );
   dc.DrawLine( x+180, y, x+180, y+30 );
   
   dc.DrawLine( x+200, y, x+200   , y    );
   dc.DrawLine( x+220, y, x+220+ 1, y+ 1 );
   dc.DrawLine( x+240, y, x+240+ 2, y+ 2 );
   dc.DrawLine( x+260, y, x+260+ 3, y+ 3 );
   dc.DrawLine( x+280, y, x+280+ 4, y+ 4 );
   dc.DrawLine( x+300, y, x+300+ 5, y+ 5 );
   dc.DrawLine( x+320, y, x+320+ 6, y+ 6 );
   dc.DrawLine( x+340, y, x+340+10, y+10 );
   dc.DrawLine( x+360, y, x+360+20, y+20 );
   dc.DrawLine( x+380, y, x+380+30, y+30 );
   
   dc.DrawLine( x+420, y   , x+420  ,  y    );
   dc.DrawLine( x+420, y+10, x+420+1,  y+10 );
   dc.DrawLine( x+420, y+20, x+420+2,  y+20 );
   dc.DrawLine( x+420, y+30, x+420+3,  y+30 );
   dc.DrawLine( x+420, y+40, x+420+4,  y+40 );
   dc.DrawLine( x+420, y+50, x+420+5,  y+50 );
   dc.DrawLine( x+420, y+60, x+420+6,  y+60 );
   dc.DrawLine( x+420, y+70, x+420+10, y+70 );
   dc.DrawLine( x+420, y+80, x+420+20, y+80 );
   dc.DrawLine( x+420, y+90, x+420+30, y+90 );
   
   y -= 40;
   
   dc.DrawCircle( x, y+100, 1);
   dc.DrawCircle( x, y+110, 2);
   dc.DrawCircle( x, y+120, 3);
   dc.DrawCircle( x, y+130, 4);
   dc.DrawCircle( x, y+140, 5);
   dc.DrawCircle( x, y+160, 7);
   dc.DrawCircle( x, y+180, 8);
   
   dc.DrawRectangle( x+50, y+100, 1, 1);
   dc.DrawRectangle( x+50, y+110, 2, 2);
   dc.DrawRectangle( x+50, y+120, 3, 3);
   dc.DrawRectangle( x+50, y+130, 4, 4);
   dc.DrawRectangle( x+50, y+140, 5, 5);
   dc.DrawRectangle( x+50, y+160, 10, 10);
   dc.DrawRectangle( x+50, y+180, 20, 20);
   
   dc.DrawRoundedRectangle( x+100, y+100, 1, 1, 1);
   dc.DrawRoundedRectangle( x+100, y+110, 2, 2, 1);
   dc.DrawRoundedRectangle( x+100, y+120, 3, 3, 1);
   dc.DrawRoundedRectangle( x+100, y+130, 4, 4, 1);
   dc.DrawRoundedRectangle( x+100, y+140, 5, 5, 1);
   dc.DrawRoundedRectangle( x+100, y+160, 10, 10, 1);
   dc.DrawRoundedRectangle( x+100, y+180, 20, 20, 1);
   
   dc.DrawRoundedRectangle( x+150, y+100, 1, 1, 2);
   dc.DrawRoundedRectangle( x+150, y+110, 2, 2, 2);
   dc.DrawRoundedRectangle( x+150, y+120, 3, 3, 2);
   dc.DrawRoundedRectangle( x+150, y+130, 4, 4, 2);
   dc.DrawRoundedRectangle( x+150, y+140, 5, 5, 2);
   dc.DrawRoundedRectangle( x+150, y+160, 10, 10, 2);
   dc.DrawRoundedRectangle( x+150, y+180, 20, 20, 2);
   
   dc.DrawRoundedRectangle( x+200, y+100, 1, 1, 3);
   dc.DrawRoundedRectangle( x+200, y+110, 2, 2, 3);
   dc.DrawRoundedRectangle( x+200, y+120, 3, 3, 3);
   dc.DrawRoundedRectangle( x+200, y+130, 4, 4, 3);
   dc.DrawRoundedRectangle( x+200, y+140, 5, 5, 3);
   dc.DrawRoundedRectangle( x+200, y+160, 10, 10, 3);
   dc.DrawRoundedRectangle( x+200, y+180, 20, 20, 3);
   
   dc.DrawRoundedRectangle( x+250, y+100, 1, 1, 5);
   dc.DrawRoundedRectangle( x+250, y+110, 2, 2, 5);
   dc.DrawRoundedRectangle( x+250, y+120, 3, 3, 5);
   dc.DrawRoundedRectangle( x+250, y+130, 4, 4, 5);
   dc.DrawRoundedRectangle( x+250, y+140, 5, 5, 5);
   dc.DrawRoundedRectangle( x+250, y+160, 10, 10, 5);
   dc.DrawRoundedRectangle( x+250, y+180, 20, 20, 5);
   
   dc.DrawRoundedRectangle( x+300, y+100, 1, 1, 10);
   dc.DrawRoundedRectangle( x+300, y+110, 2, 2, 10);
   dc.DrawRoundedRectangle( x+300, y+120, 3, 3, 10);
   dc.DrawRoundedRectangle( x+300, y+130, 4, 4, 10);
   dc.DrawRoundedRectangle( x+300, y+140, 5, 5, 10);
   dc.DrawRoundedRectangle( x+300, y+160, 10, 10, 10);
   dc.DrawRoundedRectangle( x+300, y+180, 20, 20, 10);
   
}

void MyCanvas::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    wxPaintDC dc(this);
    PrepareDC(dc);
    m_owner->PrepareDC(dc);

    dc.SetBackgroundMode( m_owner->m_backgroundMode );
    if ( m_owner->m_backgroundBrush.Ok() )
        dc.SetBackground( m_owner->m_backgroundBrush );
    if ( m_owner->m_colourForeground.Ok() )
        dc.SetTextForeground( m_owner->m_colourForeground );
    if ( m_owner->m_colourBackground.Ok() )
        dc.SetTextBackground( m_owner->m_colourBackground );

    // mark the origin
    dc.DrawCircle(0, 0, 10);
#ifndef __WXGTK__   // not implemented in wxGTK :-(
    dc.FloodFill(0, 0, wxColour(255, 0, 0));
#endif // __WXGTK__

    dc.DrawText( "This is text", 110, 10 );

    dc.DrawIcon( wxICON(mondrian), 110, 40 );
    
    dc.SetBrush( *wxRED_BRUSH );
    
    int x = 20;
    int y = 80;
    int step = 200;
    
    dc.SetPen( wxPen( "black", 1, 0) );
    DoDrawTests( x, y, dc );
       
    y += step;
    
    dc.SetPen( wxPen( "black", 1, wxDOT) );
    DoDrawTests( x, y, dc );
    
    y += step;
    
    dc.SetPen( wxPen( "black", 1, wxSHORT_DASH) );
    DoDrawTests( x, y, dc );
    
    y += step;
    
    dc.SetPen( wxPen( "black", 1, wxLONG_DASH) );
    DoDrawTests( x, y, dc );
    
    y += step;
    
    dc.SetPen( wxPen( "black", 1, wxDOT_DASH) );
    DoDrawTests( x, y, dc );
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MOTION    (MyFrame::OnMouseMove)
    EVT_MENU      (Minimal_Quit,     MyFrame::OnQuit)
    EVT_MENU      (Minimal_About,    MyFrame::OnAbout)
    EVT_MENU_RANGE(MenuOption_First, MenuOption_Last, MyFrame::OnOption)
END_EVENT_TABLE()

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    // set the frame icon
    SetIcon(wxICON(mondrian));

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(Minimal_About, "&About...\tCtrl-A", "Show about dialog");
    menuFile->AppendSeparator();
    menuFile->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    wxMenu *menuMapMode = new wxMenu;
    menuMapMode->Append( MapMode_Text, "&TEXT map mode" );
    menuMapMode->Append( MapMode_Lometric, "&LOMETRIC map mode" );
    menuMapMode->Append( MapMode_Twips, "T&WIPS map mode" );
    menuMapMode->Append( MapMode_Points, "&POINTS map mode" );
    menuMapMode->Append( MapMode_Metric, "&METRIC map mode" );

    wxMenu *menuUserScale = new wxMenu;
    menuUserScale->Append( UserScale_StretchHoriz, "Stretch horizontally\tCtrl-H" );
    menuUserScale->Append( UserScale_ShrinkHoriz, "Shrink  horizontally\tCtrl-G" );
    menuUserScale->Append( UserScale_StretchVertic, "Stretch vertically\tCtrl-V" );
    menuUserScale->Append( UserScale_ShrinkVertic, "Shrink vertically\tCtrl-W" );
    menuUserScale->AppendSeparator();
    menuUserScale->Append( UserScale_Restore, "Restore to normal\tCtrl-0" );

    wxMenu *menuAxis = new wxMenu;
    menuAxis->Append( AxisMirror_Horiz, "Mirror horizontally\tCtrl-M", "", TRUE );
    menuAxis->Append( AxisMirror_Vertic, "Mirror vertically\tCtrl-N", "", TRUE );

    wxMenu *menuLogical = new wxMenu;
    menuLogical->Append( LogicalOrigin_MoveDown, "Move &down\tCtrl-D" );
    menuLogical->Append( LogicalOrigin_MoveUp, "Move &up\tCtrl-U" );
    menuLogical->Append( LogicalOrigin_MoveLeft, "Move &right\tCtrl-L" );
    menuLogical->Append( LogicalOrigin_MoveRight, "Move &left\tCtrl-R" );

    wxMenu *menuColour = new wxMenu;
    menuColour->Append( Colour_TextForeground, "Text foreground..." );
    menuColour->Append( Colour_TextBackground, "Text background..." );
    menuColour->Append( Colour_Background, "Background colour..." );
    menuColour->Append( Colour_BackgroundMode, "Opaque/transparent\tCtrl-B", "", TRUE );

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuMapMode, "&MapMode");
    menuBar->Append(menuUserScale, "&UserScale");
    menuBar->Append(menuAxis, "&Axis");
    menuBar->Append(menuLogical, "&LogicalOrigin");
    menuBar->Append(menuColour, "&Colours");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWindows!");

    m_mapMode = wxMM_TEXT;
    m_xUserScale = 1.0;
    m_yUserScale = 1.0;
    m_xLogicalOrigin = 0;
    m_yLogicalOrigin = 0;
    m_xAxisReversed =
    m_yAxisReversed = FALSE;
    m_backgroundMode = wxSOLID;

    m_canvas = new MyCanvas( this );
    m_canvas->SetScrollbars( 10, 10, 100, 200 );
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
    msg.Printf( _T("This is the about dialog of the drawing sample.\n")
                _T("Copyright (c) Robert Roebling 1999")
              );

    wxMessageBox(msg, "About Drawing", wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnOption(wxCommandEvent &event)
{
    switch (event.GetInt())
    {
        case MapMode_Text:
            m_mapMode = wxMM_TEXT;
            break;
        case MapMode_Lometric:
            m_mapMode = wxMM_LOMETRIC;
            break;
        case MapMode_Twips:
            m_mapMode = wxMM_TWIPS;
            break;
        case MapMode_Points:
            m_mapMode = wxMM_POINTS;
            break;
        case MapMode_Metric:
            m_mapMode = wxMM_METRIC;
            break;

        case LogicalOrigin_MoveDown:
            m_yLogicalOrigin += 10;
            break;
        case LogicalOrigin_MoveUp:
            m_yLogicalOrigin -= 10;
            break;
        case LogicalOrigin_MoveLeft:
            m_xLogicalOrigin += 10;
            break;
        case LogicalOrigin_MoveRight:
            m_xLogicalOrigin -= 10;
            break;

        case UserScale_StretchHoriz:
            m_xUserScale *= 1.10;
            break;
        case UserScale_ShrinkHoriz:
            m_xUserScale /= 1.10;
            break;
        case UserScale_StretchVertic:
            m_yUserScale *= 1.10;
            break;
        case UserScale_ShrinkVertic:
            m_yUserScale /= 1.10;
            break;
        case UserScale_Restore:
            m_xUserScale =
            m_yUserScale = 1.0;
            break;

        case AxisMirror_Vertic:
            m_yAxisReversed = !m_yAxisReversed;
            break;
        case AxisMirror_Horiz:
            m_xAxisReversed = !m_xAxisReversed;
            break;

        case Colour_TextForeground:
            m_colourForeground = SelectColour();
            break;
        case Colour_TextBackground:
            m_colourBackground = SelectColour();
            break;
        case Colour_Background:
            {
                wxColour col = SelectColour();
                if ( col.Ok() )
                {
                    m_backgroundBrush.SetColour(col);
                }
            }
            break;
        case Colour_BackgroundMode:
            m_backgroundMode = m_backgroundMode == wxSOLID ? wxTRANSPARENT
                                                           : wxSOLID;
            break;

        default:
            // skip Refresh()
            return;
    }

    Refresh();
}

void MyFrame::PrepareDC(wxDC& dc)
{
    dc.SetMapMode( m_mapMode );
    dc.SetUserScale( m_xUserScale, m_yUserScale );
    dc.SetLogicalOrigin( m_xLogicalOrigin, m_yLogicalOrigin );
    dc.SetAxisOrientation( !m_xAxisReversed, m_yAxisReversed );
}

void MyFrame::OnMouseMove(wxMouseEvent &event)
{
    wxClientDC dc(this);
    PrepareDC(dc);

    wxPoint pos = event.GetPosition();
    long x = dc.DeviceToLogicalX( pos.x );
    long y = dc.DeviceToLogicalY( pos.y );
    wxString str;
    str.Printf( "Current mouse position: %d,%d", (int)x, (int)y );
    SetStatusText( str );
}

wxColour MyFrame::SelectColour()
{
    wxColour col;
    wxColourData data;
    wxColourDialog dialog(this, &data);

    if ( dialog.ShowModal() == wxID_OK )
    {
        col = dialog.GetColourData().GetColour();
    }

    return col;
}

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

    void DrawTestLines( int x, int y, int width, wxDC &dc );    
    void OnPaint(wxPaintEvent &event);
    void OnMouseMove(wxMouseEvent &event);
    
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
    EVT_MOTION (MyCanvas::OnMouseMove)
END_EVENT_TABLE()

MyCanvas::MyCanvas( MyFrame *parent )
  : wxScrolledWindow( parent )
{
  m_owner = parent;
}

void MyCanvas::DrawTestLines( int x, int y, int width, wxDC &dc )
{
    dc.SetPen( wxPen( "black", width, wxSOLID) );
    dc.SetBrush( *wxRED_BRUSH );
    dc.DrawRectangle( x+10, y+10, 400, 190 );
    
    dc.SetPen( wxPen( "black", width, wxSOLID) );
    dc.DrawLine( x+20, y+20, 390, y+20 );
    dc.SetPen( wxPen( "black", width, wxDOT) );
    dc.DrawLine( x+20, y+30, 390, y+30 );
    dc.SetPen( wxPen( "black", width, wxSHORT_DASH) );
    dc.DrawLine( x+20, y+40, 390, y+40 );
    dc.SetPen( wxPen( "black", width, wxLONG_DASH) );
    dc.DrawLine( x+20, y+50, 390, y+50 );
    dc.SetPen( wxPen( "black", width, wxDOT_DASH) );
    dc.DrawLine( x+20, y+60, 390, y+60 );

    dc.SetPen( wxPen( "black", width, wxBDIAGONAL_HATCH) );
    dc.DrawLine( x+20, y+70, 390, y+70 );
    dc.SetPen( wxPen( "black", width, wxCROSSDIAG_HATCH) );
    dc.DrawLine( x+20, y+80, 390, y+80 );
    dc.SetPen( wxPen( "black", width, wxFDIAGONAL_HATCH) );
    dc.DrawLine( x+20, y+90, 390, y+90 );
    dc.SetPen( wxPen( "black", width, wxCROSS_HATCH) );
    dc.DrawLine( x+20, y+100, 390, y+100 );
    dc.SetPen( wxPen( "black", width, wxHORIZONTAL_HATCH) );
    dc.DrawLine( x+20, y+110, 390, y+110 );
    dc.SetPen( wxPen( "black", width, wxVERTICAL_HATCH) );
    dc.DrawLine( x+20, y+120, 390, y+120 );

    wxPen ud( "black", width, wxUSER_DASH );
    wxDash dash1[1];
    dash1[0] = 0;
    ud.SetDashes( 1, dash1 );
    dc.DrawLine( x+20, y+140, 390, y+140 );
    dash1[0] = 1;
    ud.SetDashes( 1, dash1 );
    dc.DrawLine( x+20, y+150, 390, y+150 );
    dash1[0] = 2;
    ud.SetDashes( 1, dash1 );
    dc.DrawLine( x+20, y+160, 390, y+160 );
    dash1[0] = 0xFF;
    ud.SetDashes( 1, dash1 );
    dc.DrawLine( x+20, y+170, 390, y+170 );

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
#if !(defined __WXGTK__) && !(defined __WXMOTIF__)
    // not implemented in wxGTK or wxMOTIF :-(
    dc.FloodFill(0, 0, wxColour(255, 0, 0));
#endif //

    dc.DrawText( "This is text", 110, 10 );

    dc.DrawRotatedText( "+90 rotated text", 30, 30, 90 );
    dc.DrawRotatedText( "-90 rotated text", 30, 30, -90 );

    dc.SetFont( wxFont( 18, wxSWISS, wxNORMAL, wxNORMAL ) );

    dc.DrawText( "This is Swiss 18pt text.", 110, 40 );

    long length;
    long height;
    long descent;
    dc.GetTextExtent( "This is Swiss 18pt text.", &length, &height, &descent );
    wxString text;
    text.Printf( "Dimensions are length %ld, height %ld, descent %ld", length, height, descent );
    dc.DrawText( text, 110, 80 );

    dc.DrawRectangle( 100, 40, 4, height );

    text.Printf( "CharHeight() returns: %d", dc.GetCharHeight() );
    dc.DrawText( text, 110, 120 );


    dc.DrawIcon( wxICON(mondrian), 310, 40 );
    
    DrawTestLines( 0, 200, 0, dc );

    DrawTestLines( 0, 400, 1, dc );

    DrawTestLines( 0, 600, 2, dc );

    DrawTestLines( 0, 800, 6, dc );

}

void MyCanvas::OnMouseMove(wxMouseEvent &event)
{
    wxClientDC dc(this);
    PrepareDC(dc);
    m_owner->PrepareDC(dc);

    wxPoint pos = event.GetPosition();
    long x = dc.DeviceToLogicalX( pos.x );
    long y = dc.DeviceToLogicalY( pos.y );
    wxString str;
    str.Printf( "Current mouse position: %d,%d", (int)x, (int)y );
    m_owner->SetStatusText( str );
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
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

    m_canvas->Refresh();
}

void MyFrame::PrepareDC(wxDC& dc)
{
    dc.SetMapMode( m_mapMode );
    dc.SetUserScale( m_xUserScale, m_yUserScale );
    dc.SetLogicalOrigin( m_xLogicalOrigin, m_yLogicalOrigin );
    dc.SetAxisOrientation( !m_xAxisReversed, m_yAxisReversed );
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

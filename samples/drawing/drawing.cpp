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

    void DrawTestPoly( int x, int y, wxDC &dc ,int transparent );
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

MyCanvas::MyCanvas( MyFrame *parent ) : wxScrolledWindow( parent )
{
    m_owner = parent;
}

//draw a polygon and an overlapping rectangle
//is transparent is 1, the fill pattern are made transparent
//is transparent is 2, the fill pattern are made transparent but inversed
//is transparent is 0 the text for and background color will be used to represent/map
//the colors of the monochrome bitmap pixels to the fillpattern
//
//i miss_used the the menu items for setting so called back and fore ground color
//just to show how the those colors do influence the fillpatterns
//just play with those,
//and with the code
//variations are endless using other logical functions
void MyCanvas::DrawTestPoly( int x, int y,wxDC &dc,int transparent )
{
    char pathfile[80];

#ifdef __WXMSW__
    //this if run from ide project file
    sprintf(pathfile,"./drawing/pat%i.bmp",4);
    wxBitmap* nonMonoBitmap4 = new wxBitmap(32,32,-1);
    nonMonoBitmap4->LoadFile(pathfile,wxBITMAP_TYPE_BMP);

    sprintf(pathfile,"./drawing/pat%i.bmp",36);
    wxBitmap* nonMonoBitmap36 = new wxBitmap(32,32,-1);
    nonMonoBitmap36->LoadFile(pathfile,wxBITMAP_TYPE_BMP);
#else
    sprintf(pathfile,"./pat%i.bmp",4);
    wxBitmap* nonMonoBitmap4 = new wxBitmap(32,32,-1);
    nonMonoBitmap4->LoadFile(pathfile,wxBITMAP_TYPE_BMP);

    sprintf(pathfile,"./pat%i.bmp",36);
    wxBitmap* nonMonoBitmap36 = new wxBitmap(32,32,-1);
    nonMonoBitmap36->LoadFile(pathfile,wxBITMAP_TYPE_BMP);
#endif

    if ( !nonMonoBitmap4->Ok() || !nonMonoBitmap36->Ok() )
    {
        static bool s_errorGiven = FALSE;

        if ( !s_errorGiven )
        {
            wxLogError("Couldn't find bitmap files near the program file, "
                       "please copy them there.");

            s_errorGiven = TRUE;
        }
    }

    //set mask to monochrome bitmap based on color bitmap
    wxColour white("WHITE");
    wxPen _Pen = wxPen(white,1,wxSOLID);
    wxColour black("BLACK");

    wxMask* monochrome_mask4 = new wxMask(*nonMonoBitmap4,black);
    nonMonoBitmap4->SetMask(monochrome_mask4);

    wxMask* monochrome_mask36 = new wxMask(*nonMonoBitmap36,black);
    nonMonoBitmap36->SetMask(monochrome_mask36);

    wxBrush* _brush4  = new wxBrush(*nonMonoBitmap4);
    wxBrush* _brush36 = new wxBrush(*nonMonoBitmap36);

    wxPoint todraw[5];
    todraw[0].x=(long)x+100;
    todraw[0].y=(long)y+100;
    todraw[1].x=(long)x+300;
    todraw[1].y=(long)y+100;
    todraw[2].x=(long)x+300;
    todraw[2].y=(long)y+300;
    todraw[3].x=(long)x+150;
    todraw[3].y=(long)y+350;
    todraw[4].x=(long)x+100;
    todraw[4].y=(long)y+300;

    wxPoint todraw2[5];
    todraw2[0].x=100;
    todraw2[0].y=100;
    todraw2[1].x=300;
    todraw2[1].y=100;
    todraw2[2].x=300;
    todraw2[2].y=300;
    todraw2[3].x=150;
    todraw2[3].y=350;
    todraw2[4].x=100;
    todraw2[4].y=300;

    switch (transparent)
    {
        case 0:
            {
                dc.SetPen( wxPen( "black", 4, wxSOLID) );
                dc.SetBrush( *_brush4 );
                dc.SetTextForeground(*wxGREEN);
                //       dc.SetTextBackground(*wxBLUE);
                dc.SetTextBackground(m_owner->m_colourForeground);
                dc.SetLogicalFunction(wxCOPY);
                dc.DrawPolygon(5,todraw,0,0,wxWINDING_RULE);

                //don't understand hwo but the outline is also depending on logicalfunction
                dc.SetPen( wxPen( "red", 4, wxSOLID) );
                dc.SetBrush( *_brush36 );
                dc.SetTextForeground(*wxCYAN);
                //       dc.SetTextBackground(*wxRED);
                dc.SetTextBackground(m_owner->m_colourBackground);
                dc.SetLogicalFunction(wxCOPY);
                dc.DrawRectangle( x+10, y+10, 200, 200 );
                dc.SetBrush(wxNullBrush);
                dc.SetPen(wxNullPen);
                break;
            }
        case 1:  //now with transparent fillpatterns
            {

                wxBitmap* _blitbitmap = new wxBitmap(600,400);
                wxMemoryDC* _memDC= new wxMemoryDC();
                //      wxBrush _clearbrush(*wxGREEN,wxSOLID);
                wxBrush _clearbrush(*wxBLACK,wxSOLID);
                _memDC->SelectObject(*_blitbitmap);
                _memDC->BeginDrawing();
                _memDC->SetBackground(_clearbrush);
                _memDC->Clear();
                _memDC->SetBackground(wxNullBrush);

                _memDC->SetPen( wxPen( "black", 4, wxSOLID) );
                _memDC->SetBrush( wxNullBrush);
                _memDC->SetBrush( *_brush4 );
                _memDC->SetTextForeground(*wxBLACK);            // 0s --> 0x000000 (black)
                _memDC->SetTextBackground(*wxWHITE);    // 1s --> 0xFFFFFF (white)
                _memDC->SetLogicalFunction(wxAND_INVERT);

                // BLACK OUT the opaque pixels and leave the rest as is
                _memDC->DrawPolygon(5,todraw2,0,0,wxWINDING_RULE);

                // Set background and foreground colors for fill pattern
                //the previous blacked out pixels are now merged with the layer color
                //while the non blacked out pixels stay as they are.
                _memDC->SetTextForeground(*wxBLACK);            // 0s --> 0x000000 (black)

                //now define what will be the color of the fillpattern parts that are not transparent
                //      _memDC->SetTextBackground(*wxBLUE);
                _memDC->SetTextBackground(m_owner->m_colourForeground);
                _memDC->SetLogicalFunction(wxOR);


                //don't understand how but the outline is also depending on logicalfunction
                _memDC->SetPen( wxPen( "red", 4, wxSOLID) );
                _memDC->DrawPolygon(5,todraw2,0,0,wxWINDING_RULE);

                _memDC->SetLogicalFunction(wxCOPY);

                _memDC->SetPen( wxPen( "black", 4, wxSOLID) );
                _memDC->SetBrush( wxNullBrush);
                _memDC->SetBrush( *_brush36 );
                _memDC->SetTextForeground(*wxBLACK);            // 0s --> 0x000000 (black)
                _memDC->SetTextBackground(*wxWHITE);    // 1s --> 0xFFFFFF (white)
                _memDC->SetLogicalFunction(wxAND_INVERT);

                _memDC->DrawRectangle( 10, 10, 200, 200 );

                // Set background and foreground colors for fill pattern
                //the previous blacked out pixels are now merged with the layer color
                //while the non blacked out pixels stay as they are.
                _memDC->SetTextForeground(*wxBLACK);            // 0s --> 0x000000 (black)
                //now define what will be the color of the fillpattern parts that are not transparent
                //      _memDC->SetTextBackground(*wxRED);
                _memDC->SetTextBackground(m_owner->m_colourBackground);
                _memDC->SetLogicalFunction(wxOR);

                //don't understand how but the outline is also depending on logicalfunction
                _memDC->SetPen( wxPen( "yellow", 4, wxSOLID) );
                _memDC->DrawRectangle( 10, 10, 200, 200 );

                _memDC->SetBrush(wxNullBrush);
                _memDC->SetPen(wxNullPen);

                _memDC->EndDrawing();
                dc.Blit(x,y,600,400,_memDC,0,0,wxCOPY);
                delete _blitbitmap;
                delete _memDC;
                break;
            }
        case 2:  //now with transparent inversed fillpatterns
            {
                wxBitmap* _blitbitmap = new wxBitmap(600,400);
                wxMemoryDC* _memDC= new wxMemoryDC();
                wxBrush _clearbrush(*wxWHITE,wxSOLID);
                _memDC->SelectObject(*_blitbitmap);
                _memDC->BeginDrawing();
                _memDC->SetBackground(_clearbrush);
                _memDC->Clear();
                _memDC->SetBackground(wxNullBrush);

                _memDC->SetPen( wxPen( "black", 4, wxSOLID) );
                _memDC->SetBrush( *_brush4 );
                _memDC->SetTextBackground(*wxBLACK);            // 0s --> 0x000000 (black)
                _memDC->SetTextForeground(*wxWHITE);    // 1s --> 0xFFFFFF (white)
                _memDC->SetLogicalFunction(wxAND_INVERT);

                // BLACK OUT the opaque pixels and leave the rest as is
                _memDC->DrawPolygon(5,todraw2,0,0,wxWINDING_RULE);

                // Set background and foreground colors for fill pattern
                //the previous blacked out pixels are now merged with the layer color
                //while the non blacked out pixels stay as they are.
                _memDC->SetTextBackground(*wxBLACK);            // 0s --> 0x000000 (black)

                //now define what will be the color of the fillpattern parts that are not transparent
                _memDC->SetTextForeground(m_owner->m_colourForeground);
                _memDC->SetLogicalFunction(wxOR);


                //don't understand how but the outline is also depending on logicalfunction
                _memDC->SetPen( wxPen( "red", 4, wxSOLID) );
                _memDC->DrawPolygon(5,todraw2,0,0,wxWINDING_RULE);

                _memDC->SetLogicalFunction(wxCOPY);

                _memDC->SetPen( wxPen( "black", 4, wxSOLID) );
                _memDC->SetBrush( *_brush36 );
                _memDC->SetTextBackground(*wxBLACK);            // 0s --> 0x000000 (black)
                _memDC->SetTextForeground(*wxWHITE);    // 1s --> 0xFFFFFF (white)
                _memDC->SetLogicalFunction(wxAND_INVERT);

                _memDC->DrawRectangle( 10,10, 200, 200 );

                // Set background and foreground colors for fill pattern
                //the previous blacked out pixels are now merged with the layer color
                //while the non blacked out pixels stay as they are.
                _memDC->SetTextBackground(*wxBLACK);            // 0s --> 0x000000 (black)
                //now define what will be the color of the fillpattern parts that are not transparent
                _memDC->SetTextForeground(m_owner->m_colourBackground);
                _memDC->SetLogicalFunction(wxOR);

                //don't understand how but the outline is also depending on logicalfunction
                _memDC->SetPen( wxPen( "yellow", 4, wxSOLID) );
                _memDC->DrawRectangle( 10, 10, 200, 200 );

                _memDC->SetBrush(wxNullBrush);
                _memDC->SetPen(wxNullPen);
                dc.Blit(x,y,600,400,_memDC,0,0,wxCOPY);
                delete _blitbitmap;
                delete _memDC;
            }
    }

    delete _brush4;
    delete _brush36;
}

void MyCanvas::DrawTestLines( int x, int y, int width, wxDC &dc )
{
    dc.SetPen( wxPen( "black", width, wxSOLID) );
    dc.SetBrush( *wxRED_BRUSH );
    dc.DrawRectangle( x+10, y+10, 100, 190 );
    
    dc.SetPen( wxPen( "black", width, wxSOLID) );
    dc.DrawLine( x+20, y+20, 100, y+20 );
    dc.SetPen( wxPen( "black", width, wxDOT) );
    dc.DrawLine( x+20, y+30, 100, y+30 );
    dc.SetPen( wxPen( "black", width, wxSHORT_DASH) );
    dc.DrawLine( x+20, y+40, 100, y+40 );
    dc.SetPen( wxPen( "black", width, wxLONG_DASH) );
    dc.DrawLine( x+20, y+50, 100, y+50 );
    dc.SetPen( wxPen( "black", width, wxDOT_DASH) );
    dc.DrawLine( x+20, y+60, 100, y+60 );

    dc.SetPen( wxPen( "black", width, wxBDIAGONAL_HATCH) );
    dc.DrawLine( x+20, y+70, 100, y+70 );
    dc.SetPen( wxPen( "black", width, wxCROSSDIAG_HATCH) );
    dc.DrawLine( x+20, y+80, 100, y+80 );
    dc.SetPen( wxPen( "black", width, wxFDIAGONAL_HATCH) );
    dc.DrawLine( x+20, y+90, 100, y+90 );
    dc.SetPen( wxPen( "black", width, wxCROSS_HATCH) );
    dc.DrawLine( x+20, y+100, 100, y+100 );
    dc.SetPen( wxPen( "black", width, wxHORIZONTAL_HATCH) );
    dc.DrawLine( x+20, y+110, 100, y+110 );
    dc.SetPen( wxPen( "black", width, wxVERTICAL_HATCH) );
    dc.DrawLine( x+20, y+120, 100, y+120 );

    wxPen ud( "black", width, wxUSER_DASH );
    wxDash dash1[1];
    dash1[0] = 0;
    ud.SetDashes( 1, dash1 );
    dc.DrawLine( x+20, y+140, 100, y+140 );
    dash1[0] = 1;
    ud.SetDashes( 1, dash1 );
    dc.DrawLine( x+20, y+150, 100, y+150 );
    dash1[0] = 2;
    ud.SetDashes( 1, dash1 );
    dc.DrawLine( x+20, y+160, 100, y+160 );
    dash1[0] = 0xFF;
    ud.SetDashes( 1, dash1 );
    dc.DrawLine( x+20, y+170, 100, y+170 );

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

    // set underlined font for testing
    dc.SetFont( wxFont(12, wxMODERN, wxNORMAL, wxNORMAL, TRUE) );
    dc.DrawText( "This is text", 110, 10 );
    dc.DrawRotatedText( "That is text", 60, 30, -45 );

    dc.SetFont( *wxNORMAL_FONT );

    wxString text;
    dc. SetBackgroundMode(wxTRANSPARENT);

    for ( int n = -180; n < 180; n += 30 )
    {
        text.Printf("     %d rotated text", n);
        dc.DrawRotatedText(text , 400, 400, n);
    }

    dc.SetFont( wxFont( 18, wxSWISS, wxNORMAL, wxNORMAL ) );

    dc.DrawText( "This is Swiss 18pt text.", 110, 40 );

    long length;
    long height;
    long descent;
    dc.GetTextExtent( "This is Swiss 18pt text.", &length, &height, &descent );
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

    DrawTestPoly( 0, 1000, dc ,0);
    DrawTestPoly( 33, 1400, dc ,1);
    DrawTestPoly( 43, 1900, dc ,2);
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
    m_colourForeground = *wxRED;
    m_colourBackground = *wxBLUE;

    m_canvas = new MyCanvas( this );
    m_canvas->SetScrollbars( 10, 10, 100, 240 );
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

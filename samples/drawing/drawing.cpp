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
#include "wx/image.h"

// ----------------------------------------------------------------------------
// ressources
// ----------------------------------------------------------------------------

// the application icon
#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "mondrian.xpm"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// what do we show on screen (there are too many shapes to put them all on
// screen simultaneously)
enum ScreenToShow
{
    Show_Default,
    Show_Text,
    Show_Lines,
    Show_Polygons,
    Show_Mask,
    Show_Ops
};

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

static wxBitmap gs_bmpNoMask,
                gs_bmpWithColMask,
                gs_bmpMask,
                gs_bmpWithMask,
                gs_bmp4,
                gs_bmp36;

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

protected:
    bool LoadImages();
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
    void OnShow(wxCommandEvent &event);
    void OnOption(wxCommandEvent &event);

    wxColour SelectColour();
    void PrepareDC(wxDC& dc);

    int         m_backgroundMode;
    int         m_textureBackground;
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

    void OnPaint(wxPaintEvent &event);
    void OnMouseMove(wxMouseEvent &event);

    void Show(ScreenToShow show) { m_show = show; Refresh(); }

protected:
    void DrawTestPoly( int x, int y, wxDC &dc ,int transparent );
    void DrawTestLines( int x, int y, int width, wxDC &dc );
    void DrawText(wxDC& dc);
    void DrawImages(wxDC& dc);
    void DrawWithLogicalOps(wxDC& dc);
    void DrawDefault(wxDC& dc);

private:
    MyFrame *m_owner;

    ScreenToShow m_show;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    File_Quit = 1,
    File_About,

    MenuShow_First,
    File_ShowDefault = MenuShow_First,
    File_ShowText,
    File_ShowLines,
    File_ShowPolygons,
    File_ShowMask,
    File_ShowOps,
    MenuShow_Last = File_ShowOps,

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
    Colour_TextureBackgound,

    MenuOption_Last = Colour_TextureBackgound
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

bool MyApp::LoadImages()
{
    wxPathList pathList;
    pathList.Add(".");
    pathList.Add("..");

    wxString path = pathList.FindValidPath("pat4.bmp");
    if ( !path )
        return FALSE;
    gs_bmp4.LoadFile(path, wxBITMAP_TYPE_BMP);
    wxMask* mask4 = new wxMask(gs_bmp4, *wxBLACK);
    gs_bmp4.SetMask(mask4);

    path = pathList.FindValidPath("pat36.bmp");
    if ( !path )
        return FALSE;
    gs_bmp36.LoadFile(path, wxBITMAP_TYPE_BMP);
    wxMask* mask36 = new wxMask(gs_bmp36, *wxBLACK);
    gs_bmp36.SetMask(mask36);

    path = pathList.FindValidPath("image.bmp");
    if ( !path )
        return FALSE;
    gs_bmpNoMask.LoadFile(path, wxBITMAP_TYPE_BMP);
    gs_bmpWithMask.LoadFile(path, wxBITMAP_TYPE_BMP);
    gs_bmpWithColMask.LoadFile(path, wxBITMAP_TYPE_BMP);

    path = pathList.FindValidPath("mask.bmp");
    if ( !path )
        return FALSE;
    gs_bmpMask.LoadFile(path, wxBITMAP_TYPE_BMP);

//    This is so wrong, it hurts.
//    gs_bmpMask.SetDepth(1);
//    wxMask *mask = new wxMask(gs_bmpMask);

    wxMask *mask = new wxMask(gs_bmpMask, *wxBLACK);
    gs_bmpWithMask.SetMask(mask);

    mask = new wxMask(gs_bmpWithColMask, *wxWHITE);
    gs_bmpWithColMask.SetMask(mask);

    return TRUE;
}

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // Create the main application window
    MyFrame *frame = new MyFrame("Drawing sample",
                                 wxPoint(50, 50), wxSize(550, 340));

    // Show it and tell the application that it's our main window
    frame->Show(TRUE);
    SetTopWindow(frame);

    if ( !LoadImages() )
    {
        wxLogError("Can't load one of the bitmap files needed for this sample "
                   "from the current or parent directory, please copy them "
                   "there.");

        // stop here
        return FALSE;
    }

    // ok, continue
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
    m_show = Show_Default;
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
    wxBrush* brush4  = new wxBrush(gs_bmp4);
    wxBrush* brush36 = new wxBrush(gs_bmp36);

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
                dc.SetBrush( *brush4 );
                dc.SetTextForeground(*wxGREEN);
                dc.SetTextBackground(m_owner->m_colourForeground);
                dc.SetLogicalFunction(wxCOPY);
                dc.DrawPolygon(5,todraw,0,0,wxWINDING_RULE);

                //don't understand hwo but the outline is also depending on logicalfunction
                dc.SetPen( wxPen( "red", 4, wxSOLID) );
                dc.SetBrush( *brush36 );
                dc.SetTextForeground(*wxCYAN);
                dc.SetTextBackground(m_owner->m_colourBackground);
                dc.SetLogicalFunction(wxCOPY);
                dc.DrawRectangle( x+10, y+10, 200, 200 );
                dc.SetBrush(wxNullBrush);
                dc.SetPen(wxNullPen);
                break;
            }
        case 1:  //now with transparent fillpatterns
            {

                wxBitmap* bmpBlit = new wxBitmap(600,400);
                wxMemoryDC* memDC= new wxMemoryDC();
                //      wxBrush _clearbrush(*wxGREEN,wxSOLID);
                wxBrush _clearbrush(*wxBLACK,wxSOLID);
                memDC->SelectObject(*bmpBlit);
                memDC->BeginDrawing();
                memDC->SetBackground(_clearbrush);
                memDC->Clear();
                memDC->SetBackground(wxNullBrush);

                memDC->SetPen( wxPen( "black", 4, wxSOLID) );
                memDC->SetBrush( wxNullBrush);
                memDC->SetBrush( *brush4 );
                memDC->SetTextForeground(*wxBLACK);            // 0s --> 0x000000 (black)
                memDC->SetTextBackground(*wxWHITE);    // 1s --> 0xFFFFFF (white)
                memDC->SetLogicalFunction(wxAND_INVERT);

                // BLACK OUT the opaque pixels and leave the rest as is
                memDC->DrawPolygon(5,todraw2,0,0,wxWINDING_RULE);

                // Set background and foreground colors for fill pattern
                //the previous blacked out pixels are now merged with the layer color
                //while the non blacked out pixels stay as they are.
                memDC->SetTextForeground(*wxBLACK);            // 0s --> 0x000000 (black)

                //now define what will be the color of the fillpattern parts that are not transparent
                //      memDC->SetTextBackground(*wxBLUE);
                memDC->SetTextBackground(m_owner->m_colourForeground);
                memDC->SetLogicalFunction(wxOR);


                //don't understand how but the outline is also depending on logicalfunction
                memDC->SetPen( wxPen( "red", 4, wxSOLID) );
                memDC->DrawPolygon(5,todraw2,0,0,wxWINDING_RULE);

                memDC->SetLogicalFunction(wxCOPY);

                memDC->SetPen( wxPen( "black", 4, wxSOLID) );
                memDC->SetBrush( wxNullBrush);
                memDC->SetBrush( *brush36 );
                memDC->SetTextForeground(*wxBLACK);            // 0s --> 0x000000 (black)
                memDC->SetTextBackground(*wxWHITE);    // 1s --> 0xFFFFFF (white)
                memDC->SetLogicalFunction(wxAND_INVERT);

                memDC->DrawRectangle( 10, 10, 200, 200 );

                // Set background and foreground colors for fill pattern
                //the previous blacked out pixels are now merged with the layer color
                //while the non blacked out pixels stay as they are.
                memDC->SetTextForeground(*wxBLACK);            // 0s --> 0x000000 (black)
                //now define what will be the color of the fillpattern parts that are not transparent
                //      memDC->SetTextBackground(*wxRED);
                memDC->SetTextBackground(m_owner->m_colourBackground);
                memDC->SetLogicalFunction(wxOR);

                //don't understand how but the outline is also depending on logicalfunction
                memDC->SetPen( wxPen( "yellow", 4, wxSOLID) );
                memDC->DrawRectangle( 10, 10, 200, 200 );

                memDC->SetBrush(wxNullBrush);
                memDC->SetPen(wxNullPen);

                memDC->EndDrawing();
                dc.Blit(x,y,600,400,memDC,0,0,wxCOPY);
                delete bmpBlit;
                delete memDC;
                break;
            }
        case 2:  //now with transparent inversed fillpatterns
            {
                wxBitmap* bmpBlit = new wxBitmap(600,400);
                wxMemoryDC* memDC= new wxMemoryDC();
                wxBrush _clearbrush(*wxWHITE,wxSOLID);
                memDC->SelectObject(*bmpBlit);
                memDC->BeginDrawing();
                memDC->SetBackground(_clearbrush);
                memDC->Clear();
                memDC->SetBackground(wxNullBrush);

                memDC->SetPen( wxPen( "black", 4, wxSOLID) );
                memDC->SetBrush( *brush4 );
                memDC->SetTextBackground(*wxBLACK);            // 0s --> 0x000000 (black)
                memDC->SetTextForeground(*wxWHITE);    // 1s --> 0xFFFFFF (white)
                memDC->SetLogicalFunction(wxAND_INVERT);

                // BLACK OUT the opaque pixels and leave the rest as is
                memDC->DrawPolygon(5,todraw2,0,0,wxWINDING_RULE);

                // Set background and foreground colors for fill pattern
                //the previous blacked out pixels are now merged with the layer color
                //while the non blacked out pixels stay as they are.
                memDC->SetTextBackground(*wxBLACK);            // 0s --> 0x000000 (black)

                //now define what will be the color of the fillpattern parts that are not transparent
                memDC->SetTextForeground(m_owner->m_colourForeground);
                memDC->SetLogicalFunction(wxOR);


                //don't understand how but the outline is also depending on logicalfunction
                memDC->SetPen( wxPen( "red", 4, wxSOLID) );
                memDC->DrawPolygon(5,todraw2,0,0,wxWINDING_RULE);

                memDC->SetLogicalFunction(wxCOPY);

                memDC->SetPen( wxPen( "black", 4, wxSOLID) );
                memDC->SetBrush( *brush36 );
                memDC->SetTextBackground(*wxBLACK);            // 0s --> 0x000000 (black)
                memDC->SetTextForeground(*wxWHITE);    // 1s --> 0xFFFFFF (white)
                memDC->SetLogicalFunction(wxAND_INVERT);

                memDC->DrawRectangle( 10,10, 200, 200 );

                // Set background and foreground colors for fill pattern
                //the previous blacked out pixels are now merged with the layer color
                //while the non blacked out pixels stay as they are.
                memDC->SetTextBackground(*wxBLACK);            // 0s --> 0x000000 (black)
                //now define what will be the color of the fillpattern parts that are not transparent
                memDC->SetTextForeground(m_owner->m_colourBackground);
                memDC->SetLogicalFunction(wxOR);

                //don't understand how but the outline is also depending on logicalfunction
                memDC->SetPen( wxPen( "yellow", 4, wxSOLID) );
                memDC->DrawRectangle( 10, 10, 200, 200 );

                memDC->SetBrush(wxNullBrush);
                memDC->SetPen(wxNullPen);
                dc.Blit(x,y,600,400,memDC,0,0,wxCOPY);
                delete bmpBlit;
                delete memDC;
            }
    }

    delete brush4;
    delete brush36;
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

void MyCanvas::DrawDefault(wxDC& dc)
{
    // mark the origin
    dc.DrawCircle(0, 0, 10);
#if !(defined __WXGTK__) && !(defined __WXMOTIF__)
    // not implemented in wxGTK or wxMOTIF :-(
    dc.FloodFill(0, 0, wxColour(255, 0, 0));
#endif //

    dc.DrawIcon( wxICON(mondrian), 40, 40 );

    dc.DrawCheckMark(5, 80, 15, 15);
    dc.DrawCheckMark(25, 80, 30, 30);
    dc.DrawCheckMark(60, 80, 60, 60);

    // this is the test for "blitting bitmap into DC damages selected brush"
    // bug
    wxIcon icon = wxTheApp->GetStdIcon(wxICON_INFORMATION);
    wxCoord rectSize = icon.GetWidth() + 10;
    wxCoord x = 100;
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush( *wxGREEN_BRUSH );
    dc.DrawRectangle(x, 10, rectSize, rectSize);
    dc.DrawBitmap(icon, x + 5, 15, TRUE);
    x += rectSize + 10;
    dc.DrawRectangle(x, 10, rectSize, rectSize);
    dc.DrawIcon(wxTheApp->GetStdIcon(wxICON_INFORMATION), x + 5, 15);
    x += rectSize + 10;
    dc.DrawRectangle(x, 10, rectSize, rectSize);

    // test for "transparent" bitmap drawing (it intersects with the last
    // rectangle above)
    //dc.SetBrush( *wxTRANSPARENT_BRUSH );
    #include "../image/smile.xpm"
    wxBitmap bmp(smile_xpm);

    if (bmp.Ok())
        dc.DrawBitmap(bmp, x + rectSize - 20, rectSize - 10, TRUE);

    dc.SetBrush( *wxBLACK_BRUSH );
    dc.DrawRectangle( 0, 160, 1000, 300 );

    // draw lines
    wxBitmap bitmap(20,70);
    wxMemoryDC memdc;
    memdc.SelectObject( bitmap );
    memdc.SetBrush( *wxBLACK_BRUSH );
    memdc.SetPen( *wxWHITE_PEN );
    memdc.DrawRectangle(0,0,20,70);
    memdc.DrawLine( 10,0,10,70 );

    // to the right
    wxPen pen = *wxRED_PEN;
    memdc.SetPen(pen);
    memdc.DrawLine( 10, 5,10, 5 );
    memdc.DrawLine( 10,10,11,10 );
    memdc.DrawLine( 10,15,12,15 );
    memdc.DrawLine( 10,20,13,20 );

/*
    memdc.SetPen(*wxRED_PEN);
    memdc.DrawLine( 12, 5,12, 5 );
    memdc.DrawLine( 12,10,13,10 );
    memdc.DrawLine( 12,15,14,15 );
    memdc.DrawLine( 12,20,15,20 );
*/

    // same to the left
    memdc.DrawLine( 10,25,10,25 );
    memdc.DrawLine( 10,30, 9,30 );
    memdc.DrawLine( 10,35, 8,35 );
    memdc.DrawLine( 10,40, 7,40 );

    // XOR draw lines
    dc.SetPen(*wxWHITE_PEN);
    memdc.SetLogicalFunction( wxINVERT );
    memdc.SetPen( *wxWHITE_PEN );
    memdc.DrawLine( 10,50,10,50 );
    memdc.DrawLine( 10,55,11,55 );
    memdc.DrawLine( 10,60,12,60 );
    memdc.DrawLine( 10,65,13,65 );

    memdc.DrawLine( 12,50,12,50 );
    memdc.DrawLine( 12,55,13,55 );
    memdc.DrawLine( 12,60,14,60 );
    memdc.DrawLine( 12,65,15,65 );

    memdc.SelectObject( wxNullBitmap );
    dc.DrawBitmap( bitmap, 10, 170 );
    wxImage image( bitmap );
    image.Rescale( 60,210 );
    bitmap = image.ConvertToBitmap();
    dc.DrawBitmap( bitmap, 50, 170 );

    // test the rectangle outline drawing - there should be one pixel between
    // the rect and the lines
    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush( *wxTRANSPARENT_BRUSH );
    dc.DrawRectangle(150, 170, 49, 29);
    dc.DrawRectangle(200, 170, 49, 29);
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawLine(250, 210, 250, 170);
    dc.DrawLine(260, 200, 150, 200);

    // test the rectangle filled drawing - there should be one pixel between
    // the rect and the lines
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawRectangle(300, 170, 49, 29);
    dc.DrawRectangle(350, 170, 49, 29);
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawLine(400, 170, 400, 210);
    dc.DrawLine(300, 200, 410, 200);

    // a few more tests of this kind
    dc.SetPen(*wxRED_PEN);
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawRectangle(300, 220, 1, 1);
    dc.DrawRectangle(310, 220, 2, 2);
    dc.DrawRectangle(320, 220, 3, 3);
    dc.DrawRectangle(330, 220, 4, 4);

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawRectangle(300, 230, 1, 1);
    dc.DrawRectangle(310, 230, 2, 2);
    dc.DrawRectangle(320, 230, 3, 3);
    dc.DrawRectangle(330, 230, 4, 4);

    // and now for filled rect with outline
    dc.SetPen(*wxRED_PEN);
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawRectangle(500, 170, 49, 29);
    dc.DrawRectangle(550, 170, 49, 29);
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawLine(600, 170, 600, 210);
    dc.DrawLine(500, 200, 610, 200);

    // test the rectangle outline drawing - there should be one pixel between
    // the rect and the lines
    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush( *wxTRANSPARENT_BRUSH );
    dc.DrawRoundedRectangle(150, 270, 49, 29, 6);
    dc.DrawRoundedRectangle(200, 270, 49, 29, 6);
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawLine(250, 270, 250, 310);
    dc.DrawLine(150, 300, 260, 300);

    // test the rectangle filled drawing - there should be one pixel between
    // the rect and the lines
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawRoundedRectangle(300, 270, 49, 29, 6);
    dc.DrawRoundedRectangle(350, 270, 49, 29, 6);
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawLine(400, 270, 400, 310);
    dc.DrawLine(300, 300, 410, 300);

    // Added by JACS to demonstrate bizarre behaviour.
    // With a size of 70, we get a missing red RHS,
    // and the hight is too small, so we get yellow
    // showing. With a size of 40, it draws as expected:
    // it just shows a white rectangle with red outline.
    int totalWidth = 70;
    int totalHeight = 70;
    wxBitmap bitmap2(totalWidth, totalHeight);

    wxMemoryDC memdc2;
    memdc2.SelectObject(bitmap2);

    wxBrush yellowBrush(wxColour(255, 255, 0), wxSOLID);
    memdc2.SetBackground(yellowBrush);
    memdc2.Clear();

    wxPen yellowPen(wxColour(255, 255, 0), 1, wxSOLID);

    // Now draw a white rectangle with red outline. It should
    // entirely eclipse the yellow background.
    memdc2.SetPen(*wxRED_PEN);
    memdc2.SetBrush(*wxWHITE_BRUSH);

    memdc2.DrawRectangle(0, 0, totalWidth, totalHeight);

    memdc2.SetPen(wxNullPen);
    memdc2.SetBrush(wxNullBrush);
    memdc2.SelectObject(wxNullBitmap);

    dc.DrawBitmap(bitmap2, 500, 270);

    // Repeat, but draw directly on dc
    // Draw a yellow rectangle filling the bitmap

    x = 600; int y = 270;
    dc.SetPen(yellowPen);
    dc.SetBrush(yellowBrush);
    dc.DrawRectangle(x, y, totalWidth, totalHeight);

    // Now draw a white rectangle with red outline. It should
    // entirely eclipse the yellow background.
    dc.SetPen(*wxRED_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);

    dc.DrawRectangle(x, y, totalWidth, totalHeight);
}

void MyCanvas::DrawText(wxDC& dc)
{
    // set underlined font for testing
    dc.SetFont( wxFont(12, wxMODERN, wxNORMAL, wxNORMAL, TRUE) );
    dc.DrawText( "This is text", 110, 10 );
    dc.DrawRotatedText( "That is text", 20, 10, -45 );

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

    text.Printf( "CharHeight() returns: %d", dc.GetCharHeight() );
    dc.DrawText( text, 110, 120 );

    dc.DrawRectangle( 100, 40, 4, height );
}

static const struct
{
    const wxChar *name;
    int           rop;
} rasterOperations[] =
{
    { "wxAND",          wxAND           },
    { "wxAND_INVERT",   wxAND_INVERT    },
    { "wxAND_REVERSE",  wxAND_REVERSE   },
    { "wxCLEAR",        wxCLEAR         },
    { "wxCOPY",         wxCOPY          },
    { "wxEQUIV",        wxEQUIV         },
    { "wxINVERT",       wxINVERT        },
    { "wxNAND",         wxNAND          },
    { "wxNO_OP",        wxNO_OP         },
    { "wxOR",           wxOR            },
    { "wxOR_INVERT",    wxOR_INVERT     },
    { "wxOR_REVERSE",   wxOR_REVERSE    },
    { "wxSET",          wxSET           },
    { "wxSRC_INVERT",   wxSRC_INVERT    },
    { "wxXOR",          wxXOR           },
};

void MyCanvas::DrawImages(wxDC& dc)
{
    dc.DrawText("original image", 0, 0);
    dc.DrawBitmap(gs_bmpNoMask, 0, 20, 0);
    dc.DrawText("with colour mask", 0, 100);
    dc.DrawBitmap(gs_bmpWithColMask, 0, 120, TRUE);
    dc.DrawText("the mask image", 0, 200);
    dc.DrawBitmap(gs_bmpMask, 0, 220, 0);
    dc.DrawText("masked image", 0, 300);
    dc.DrawBitmap(gs_bmpWithMask, 0, 320, TRUE);

    int cx = gs_bmpWithColMask.GetWidth(),
        cy = gs_bmpWithColMask.GetHeight();

    wxMemoryDC memDC;
    for ( size_t n = 0; n < WXSIZEOF(rasterOperations); n++ )
    {
        wxCoord x = 120 + 150*(n%4),
                y =  20 + 100*(n/4);

        dc.DrawText(rasterOperations[n].name, x, y - 20);
        memDC.SelectObject(gs_bmpWithColMask);
        dc.Blit(x, y, cx, cy, &memDC, 0, 0, rasterOperations[n].rop, TRUE);
    }
}

void MyCanvas::DrawWithLogicalOps(wxDC& dc)
{
    static const wxCoord w = 60;
    static const wxCoord h = 60;

    // reuse the text colour here
    dc.SetPen(wxPen(m_owner->m_colourForeground, 1, wxSOLID));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    size_t n;
    for ( n = 0; n < WXSIZEOF(rasterOperations); n++ )
    {
        wxCoord x = 20 + 150*(n%4),
                y = 20 + 100*(n/4);

        dc.DrawText(rasterOperations[n].name, x, y - 20);
        dc.SetLogicalFunction(rasterOperations[n].rop);
        dc.DrawRectangle(x, y, w, h);
        dc.DrawLine(x, y, x + w, y + h);
        dc.DrawLine(x + w, y, x, y + h);
    }

    // now some filled rectangles
    dc.SetBrush(wxBrush(m_owner->m_colourForeground, wxSOLID));

    for ( n = 0; n < WXSIZEOF(rasterOperations); n++ )
    {
        wxCoord x = 20 + 150*(n%4),
                y = 500 + 100*(n/4);

        dc.DrawText(rasterOperations[n].name, x, y - 20);
        dc.SetLogicalFunction(rasterOperations[n].rop);
        dc.DrawRectangle(x, y, w, h);
    }
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

    if ( m_owner->m_textureBackground) {
        if ( ! m_owner->m_backgroundBrush.Ok() ) {
            wxBrush b(wxColour(0,128,0), wxSOLID);
            dc.SetBackground(b);
        }
    }

    dc.Clear();

    if ( m_owner->m_textureBackground) {
        dc.SetPen(*wxMEDIUM_GREY_PEN);
        for (int i=0; i<200; i++)
            dc.DrawLine(0, i*10, i*10, 0);
    }

    switch ( m_show )
    {
        case Show_Default:
            DrawDefault(dc);
            break;

        case Show_Text:
            DrawText(dc);
            break;

        case Show_Lines:
            DrawTestLines( 0, 100, 0, dc );
            DrawTestLines( 0, 300, 1, dc );
            DrawTestLines( 0, 500, 2, dc );
            DrawTestLines( 0, 700, 6, dc );
            break;

        case Show_Polygons:
            DrawTestPoly( 0,  100, dc, 0 );
            DrawTestPoly( 33, 500, dc, 1 );
            DrawTestPoly( 43, 1000, dc, 2 );
            break;

        case Show_Mask:
            DrawImages(dc);
            break;

        case Show_Ops:
            DrawWithLogicalOps(dc);
            break;
    }
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
    EVT_MENU      (File_Quit,     MyFrame::OnQuit)
    EVT_MENU      (File_About,    MyFrame::OnAbout)

    EVT_MENU_RANGE(MenuShow_First,   MenuShow_Last,   MyFrame::OnShow)

    EVT_MENU_RANGE(MenuOption_First, MenuOption_Last, MyFrame::OnOption)
END_EVENT_TABLE()

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    // set the frame icon
    SetIcon(wxICON(mondrian));

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(File_ShowDefault, "&Default screen\tF1");
    menuFile->Append(File_ShowText, "&Text screen\tF2");
    menuFile->Append(File_ShowLines, "&Lines screen\tF3");
    menuFile->Append(File_ShowPolygons, "&Polygons screen\tF4");
    menuFile->Append(File_ShowMask, "wx&Mask screen\tF5");
    menuFile->Append(File_ShowOps, "&ROP screen\tF6");
    menuFile->AppendSeparator();
    menuFile->Append(File_About, "&About...\tCtrl-A", "Show about dialog");
    menuFile->AppendSeparator();
    menuFile->Append(File_Quit, "E&xit\tAlt-X", "Quit this program");

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
    menuColour->Append( Colour_TextureBackgound, "Draw textured background\tCtrl-T", "", TRUE);

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
    m_textureBackground = FALSE;

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
    msg.Printf( wxT("This is the about dialog of the drawing sample.\n")
                wxT("This sample tests various primitive drawing functions\n")
                wxT("without any tests to prevent flicker.\n")
                wxT("Copyright (c) Robert Roebling 1999")
              );

    wxMessageBox(msg, "About Drawing", wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnShow(wxCommandEvent& event)
{
    m_canvas->Show((ScreenToShow)(event.GetInt() - MenuShow_First));
}

void MyFrame::OnOption(wxCommandEvent& event)
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

        case Colour_TextureBackgound:
            m_textureBackground = ! m_textureBackground;
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

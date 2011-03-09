/////////////////////////////////////////////////////////////////////////////
// Name:        samples/drawing/drawing.cpp
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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/colordlg.h"
#include "wx/image.h"
#include "wx/artprov.h"
#include "wx/dcgraph.h"
#include "wx/overlay.h"
#include "wx/graphics.h"
#include "wx/filename.h"

#define TEST_CAIRO_EVERYWHERE 0

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon
#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

static wxBitmap *gs_bmpNoMask = NULL,
                *gs_bmpWithColMask = NULL,
                *gs_bmpMask = NULL,
                *gs_bmpWithMask = NULL,
                *gs_bmp4 = NULL,
                *gs_bmp4_mono = NULL,
                *gs_bmp36 = NULL;

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

    virtual int OnExit() { DeleteBitmaps(); return 0; }

protected:
    void DeleteBitmaps();

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
    void OnClip(wxCommandEvent& event);
#if wxUSE_GRAPHICS_CONTEXT
    void OnGraphicContext(wxCommandEvent& event);
#endif
    void OnShow(wxCommandEvent &event);
    void OnOption(wxCommandEvent &event);

#if wxUSE_COLOURDLG
    wxColour SelectColour();
#endif // wxUSE_COLOURDLG
    void PrepareDC(wxDC& dc);

    int         m_backgroundMode;
    int         m_textureBackground;
    wxMappingMode m_mapMode;
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
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// define a scrollable canvas for drawing onto
class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas( MyFrame *parent );

    void OnPaint(wxPaintEvent &event);
    void OnMouseMove(wxMouseEvent &event);
    void OnMouseDown(wxMouseEvent &event);
    void OnMouseUp(wxMouseEvent &event);

    void ToShow(int show) { m_show = show; Refresh(); }

    // set or remove the clipping region
    void Clip(bool clip) { m_clip = clip; Refresh(); }
#if wxUSE_GRAPHICS_CONTEXT
    void UseGraphicContext(bool use) { m_useContext = use; Refresh(); }
#endif

protected:
    enum DrawMode
    {
        Draw_Normal,
        Draw_Stretch
    };

    void DrawTestLines( int x, int y, int width, wxDC &dc );
    void DrawTestPoly(wxDC& dc);
    void DrawTestBrushes(wxDC& dc);
    void DrawText(wxDC& dc);
    void DrawImages(wxDC& dc, DrawMode mode);
    void DrawWithLogicalOps(wxDC& dc);
#if wxUSE_GRAPHICS_CONTEXT
    void DrawAlpha(wxDC& dc);
    void DrawGraphics(wxGraphicsContext* gc);
#endif
    void DrawRegions(wxDC& dc);
    void DrawCircles(wxDC& dc);
    void DrawSplines(wxDC& dc);
    void DrawDefault(wxDC& dc);
    void DrawGradients(wxDC& dc);

    void DrawRegionsHelper(wxDC& dc, wxCoord x, bool firstTime);

private:
    MyFrame *m_owner;

    int          m_show;
    wxBitmap     m_smile_bmp;
    wxIcon       m_std_icon;
    bool         m_clip;
    wxOverlay    m_overlay;
    bool         m_rubberBand;
    wxPoint      m_anchorpoint;
    wxPoint      m_currentpoint;
#if wxUSE_GRAPHICS_CONTEXT
    bool         m_useContext ;
#endif

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    File_Quit = wxID_EXIT,
    File_About = wxID_ABOUT,

    MenuShow_First = wxID_HIGHEST,
    File_ShowDefault = MenuShow_First,
    File_ShowText,
    File_ShowLines,
    File_ShowBrushes,
    File_ShowPolygons,
    File_ShowMask,
    File_ShowMaskStretch,
    File_ShowOps,
    File_ShowRegions,
    File_ShowCircles,
    File_ShowSplines,
#if wxUSE_GRAPHICS_CONTEXT
    File_ShowAlpha,
    File_ShowGraphics,
#endif
    File_ShowGradients,
    MenuShow_Last = File_ShowGradients,

    File_Clip,
#if wxUSE_GRAPHICS_CONTEXT
    File_GraphicContext,
#endif

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
    LogicalOrigin_Set,
    LogicalOrigin_Restore,

#if wxUSE_COLOURDLG
    Colour_TextForeground,
    Colour_TextBackground,
    Colour_Background,
#endif // wxUSE_COLOURDLG
    Colour_BackgroundMode,
    Colour_TextureBackgound,

    MenuOption_Last = Colour_TextureBackgound
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------


// Create a new application object: this macro will allow wxWidgets to create
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
    gs_bmpNoMask = new wxBitmap;
    gs_bmpWithColMask = new wxBitmap;
    gs_bmpMask = new wxBitmap;
    gs_bmpWithMask = new wxBitmap;
    gs_bmp4 = new wxBitmap;
    gs_bmp4_mono = new wxBitmap;
    gs_bmp36 = new wxBitmap;

    wxPathList pathList;
    // special hack for Unix in-tree sample build, don't do this in real
    // programs, use wxStandardPaths instead
    pathList.Add(wxFileName(argv[0]).GetPath());
    pathList.Add(wxT("."));
    pathList.Add(wxT(".."));
    pathList.Add(wxT("../.."));

    wxString path = pathList.FindValidPath(wxT("pat4.bmp"));
    if ( !path )
        return false;

    /* 4 colour bitmap */
    gs_bmp4->LoadFile(path, wxBITMAP_TYPE_BMP);
    /* turn into mono-bitmap */
    gs_bmp4_mono->LoadFile(path, wxBITMAP_TYPE_BMP);
    wxMask* mask4 = new wxMask(*gs_bmp4_mono, *wxBLACK);
    gs_bmp4_mono->SetMask(mask4);

    path = pathList.FindValidPath(wxT("pat36.bmp"));
    if ( !path )
        return false;
    gs_bmp36->LoadFile(path, wxBITMAP_TYPE_BMP);
    wxMask* mask36 = new wxMask(*gs_bmp36, *wxBLACK);
    gs_bmp36->SetMask(mask36);

    path = pathList.FindValidPath(wxT("image.bmp"));
    if ( !path )
        return false;
    gs_bmpNoMask->LoadFile(path, wxBITMAP_TYPE_BMP);
    gs_bmpWithMask->LoadFile(path, wxBITMAP_TYPE_BMP);
    gs_bmpWithColMask->LoadFile(path, wxBITMAP_TYPE_BMP);

    path = pathList.FindValidPath(wxT("mask.bmp"));
    if ( !path )
        return false;
    gs_bmpMask->LoadFile(path, wxBITMAP_TYPE_BMP);

    wxMask *mask = new wxMask(*gs_bmpMask, *wxBLACK);
    gs_bmpWithMask->SetMask(mask);

    mask = new wxMask(*gs_bmpWithColMask, *wxWHITE);
    gs_bmpWithColMask->SetMask(mask);

    return true;
}

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create the main application window
    MyFrame *frame = new MyFrame(wxT("Drawing sample"),
                                 wxDefaultPosition, wxSize(550, 840));

    // Show it
    frame->Show(true);

    if ( !LoadImages() )
    {
        wxLogError(wxT("Can't load one of the bitmap files needed ")
                   wxT("for this sample from the current or parent ")
                   wxT("directory, please copy them there."));

        // still continue, the sample can be used without images too if they're
        // missing for whatever reason
    }

    return true;
}

void MyApp::DeleteBitmaps()
{
    wxDELETE(gs_bmpNoMask);
    wxDELETE(gs_bmpWithColMask);
    wxDELETE(gs_bmpMask);
    wxDELETE(gs_bmpWithMask);
    wxDELETE(gs_bmp4);
    wxDELETE(gs_bmp4_mono);
    wxDELETE(gs_bmp36);
}

// ----------------------------------------------------------------------------
// MyCanvas
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them.
BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT  (MyCanvas::OnPaint)
    EVT_MOTION (MyCanvas::OnMouseMove)
    EVT_LEFT_DOWN (MyCanvas::OnMouseDown)
    EVT_LEFT_UP (MyCanvas::OnMouseUp)
END_EVENT_TABLE()

#include "smile.xpm"

MyCanvas::MyCanvas(MyFrame *parent)
        : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                           wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE)
{
    m_owner = parent;
    m_show = File_ShowDefault;
    m_smile_bmp = wxBitmap(smile_xpm);
    m_std_icon = wxArtProvider::GetIcon(wxART_INFORMATION);
    m_clip = false;
    m_rubberBand = false;
#if wxUSE_GRAPHICS_CONTEXT
    m_useContext = false;
#endif
}

void MyCanvas::DrawTestBrushes(wxDC& dc)
{
    static const wxCoord WIDTH = 200;
    static const wxCoord HEIGHT = 80;

    wxCoord x = 10,
            y = 10;

    dc.SetBrush(wxBrush(*wxGREEN, wxSOLID));
    dc.DrawRectangle(x, y, WIDTH, HEIGHT);
    dc.DrawText(wxT("Solid green"), x + 10, y + 10);

    y += HEIGHT;
    dc.SetBrush(wxBrush(*wxRED, wxCROSSDIAG_HATCH));
    dc.DrawRectangle(x, y, WIDTH, HEIGHT);
    dc.DrawText(wxT("Diagonally hatched red"), x + 10, y + 10);

    y += HEIGHT;
    dc.SetBrush(wxBrush(*wxBLUE, wxCROSS_HATCH));
    dc.DrawRectangle(x, y, WIDTH, HEIGHT);
    dc.DrawText(wxT("Cross hatched blue"), x + 10, y + 10);

    y += HEIGHT;
    dc.SetBrush(wxBrush(*wxCYAN, wxVERTICAL_HATCH));
    dc.DrawRectangle(x, y, WIDTH, HEIGHT);
    dc.DrawText(wxT("Vertically hatched cyan"), x + 10, y + 10);

    y += HEIGHT;
    dc.SetBrush(wxBrush(*wxBLACK, wxHORIZONTAL_HATCH));
    dc.DrawRectangle(x, y, WIDTH, HEIGHT);
    dc.DrawText(wxT("Horizontally hatched black"), x + 10, y + 10);

    y += HEIGHT;
    dc.SetBrush(wxBrush(*gs_bmpMask));
    dc.DrawRectangle(x, y, WIDTH, HEIGHT);
    dc.DrawText(wxT("Stipple mono"), x + 10, y + 10);

    y += HEIGHT;
    dc.SetBrush(wxBrush(*gs_bmpNoMask));
    dc.DrawRectangle(x, y, WIDTH, HEIGHT);
    dc.DrawText(wxT("Stipple colour"), x + 10, y + 10);
}

void MyCanvas::DrawTestPoly(wxDC& dc)
{
    wxBrush brushHatch(*wxRED, wxFDIAGONAL_HATCH);
    dc.SetBrush(brushHatch);

    wxPoint star[5];
    star[0] = wxPoint(100, 60);
    star[1] = wxPoint(60, 150);
    star[2] = wxPoint(160, 100);
    star[3] = wxPoint(40, 100);
    star[4] = wxPoint(140, 150);

    dc.DrawText(wxT("You should see two (irregular) stars below, the left one ")
                wxT("hatched"), 10, 10);
    dc.DrawText(wxT("except for the central region and the right ")
                wxT("one entirely hatched"), 10, 30);
    dc.DrawText(wxT("The third star only has a hatched outline"), 10, 50);

    dc.DrawPolygon(WXSIZEOF(star), star, 0, 30);
    dc.DrawPolygon(WXSIZEOF(star), star, 160, 30, wxWINDING_RULE);

    wxPoint star2[10];
    star2[0] = wxPoint(0, 100);
    star2[1] = wxPoint(-59, -81);
    star2[2] = wxPoint(95, 31);
    star2[3] = wxPoint(-95, 31);
    star2[4] = wxPoint(59, -81);
    star2[5] = wxPoint(0, 80);
    star2[6] = wxPoint(-47, -64);
    star2[7] = wxPoint(76, 24);
    star2[8] = wxPoint(-76, 24);
    star2[9] = wxPoint(47, -64);
    int count[2] = {5, 5};

    dc.DrawPolyPolygon(WXSIZEOF(count), count, star2, 450, 150);
}

void MyCanvas::DrawTestLines( int x, int y, int width, wxDC &dc )
{
    dc.SetPen( wxPen( wxT("black"), width, wxSOLID) );
    dc.SetBrush( *wxRED_BRUSH );
    dc.DrawText(wxString::Format(wxT("Testing lines of width %d"), width), x + 10, y - 10);
    dc.DrawRectangle( x+10, y+10, 100, 190 );

    dc.DrawText(wxT("Solid/dot/short dash/long dash/dot dash"), x + 150, y + 10);
    dc.SetPen( wxPen( wxT("black"), width, wxSOLID) );
    dc.DrawLine( x+20, y+20, 100, y+20 );
    dc.SetPen( wxPen( wxT("black"), width, wxDOT) );
    dc.DrawLine( x+20, y+30, 100, y+30 );
    dc.SetPen( wxPen( wxT("black"), width, wxSHORT_DASH) );
    dc.DrawLine( x+20, y+40, 100, y+40 );
    dc.SetPen( wxPen( wxT("black"), width, wxLONG_DASH) );
    dc.DrawLine( x+20, y+50, 100, y+50 );
    dc.SetPen( wxPen( wxT("black"), width, wxDOT_DASH) );
    dc.DrawLine( x+20, y+60, 100, y+60 );

    dc.DrawText(wxT("Misc hatches"), x + 150, y + 70);
    dc.SetPen( wxPen( wxT("black"), width, wxBDIAGONAL_HATCH) );
    dc.DrawLine( x+20, y+70, 100, y+70 );
    dc.SetPen( wxPen( wxT("black"), width, wxCROSSDIAG_HATCH) );
    dc.DrawLine( x+20, y+80, 100, y+80 );
    dc.SetPen( wxPen( wxT("black"), width, wxFDIAGONAL_HATCH) );
    dc.DrawLine( x+20, y+90, 100, y+90 );
    dc.SetPen( wxPen( wxT("black"), width, wxCROSS_HATCH) );
    dc.DrawLine( x+20, y+100, 100, y+100 );
    dc.SetPen( wxPen( wxT("black"), width, wxHORIZONTAL_HATCH) );
    dc.DrawLine( x+20, y+110, 100, y+110 );
    dc.SetPen( wxPen( wxT("black"), width, wxVERTICAL_HATCH) );
    dc.DrawLine( x+20, y+120, 100, y+120 );

    dc.DrawText(wxT("User dash"), x + 150, y + 140);
    wxPen ud( wxT("black"), width, wxUSER_DASH );
    wxDash dash1[6];
    dash1[0] = 8;  // Long dash  <---------+
    dash1[1] = 2;  // Short gap            |
    dash1[2] = 3;  // Short dash           |
    dash1[3] = 2;  // Short gap            |
    dash1[4] = 3;  // Short dash           |
    dash1[5] = 2;  // Short gap and repeat +
    ud.SetDashes( 6, dash1 );
    dc.SetPen( ud );
    dc.DrawLine( x+20, y+140, 100, y+140 );
    dash1[0] = 5;  // Make first dash shorter
    ud.SetDashes( 6, dash1 );
    dc.SetPen( ud );
    dc.DrawLine( x+20, y+150, 100, y+150 );
    dash1[2] = 5;  // Make second dash longer
    ud.SetDashes( 6, dash1 );
    dc.SetPen( ud );
    dc.DrawLine( x+20, y+160, 100, y+160 );
    dash1[4] = 5;  // Make third dash longer
    ud.SetDashes( 6, dash1 );
    dc.SetPen( ud );
    dc.DrawLine( x+20, y+170, 100, y+170 );
}

void MyCanvas::DrawDefault(wxDC& dc)
{
    // mark the origin
    dc.DrawCircle(0, 0, 10);

#if !defined(wxMAC_USE_CORE_GRAPHICS) || !wxMAC_USE_CORE_GRAPHICS
    // GetPixel and FloodFill not supported by Mac OS X CoreGraphics
    // (FloodFill uses Blit from a non-wxMemoryDC)
    //flood fill using brush, starting at 1,1 and replacing whatever colour we find there
    dc.SetBrush(wxBrush(wxColour(128,128,0), wxSOLID));

    wxColour tmpColour ;
    dc.GetPixel(1,1, &tmpColour);
    dc.FloodFill(1,1, tmpColour, wxFLOOD_SURFACE);
#endif

    dc.DrawCheckMark(5, 80, 15, 15);
    dc.DrawCheckMark(25, 80, 30, 30);
    dc.DrawCheckMark(60, 80, 60, 60);

    // this is the test for "blitting bitmap into DC damages selected brush" bug
    wxCoord rectSize = m_std_icon.GetWidth() + 10;
    wxCoord x = 100;
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush( *wxGREEN_BRUSH );
    dc.DrawRectangle(x, 10, rectSize, rectSize);
    dc.DrawBitmap(m_std_icon, x + 5, 15, true);
    x += rectSize + 10;
    dc.DrawRectangle(x, 10, rectSize, rectSize);
    dc.DrawIcon(m_std_icon, x + 5, 15);
    x += rectSize + 10;
    dc.DrawRectangle(x, 10, rectSize, rectSize);

    // test for "transparent" bitmap drawing (it intersects with the last
    // rectangle above)
    //dc.SetBrush( *wxTRANSPARENT_BRUSH );

    if (m_smile_bmp.Ok())
        dc.DrawBitmap(m_smile_bmp, x + rectSize - 20, rectSize - 10, true);

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
    wxImage image = bitmap.ConvertToImage();
    image.Rescale( 60,210 );
    bitmap = wxBitmap(image);
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
    // and the height is too small, so we get yellow
    // showing. With a size of 40, it draws as expected:
    // it just shows a white rectangle with red outline.
    int totalWidth = 70;
    int totalHeight = 70;
    wxBitmap bitmap2(totalWidth, totalHeight);

    wxMemoryDC memdc2;
    memdc2.SelectObject(bitmap2);

    wxColour clr(255, 255, 0);
    wxBrush yellowBrush(clr, wxSOLID);
    memdc2.SetBackground(yellowBrush);
    memdc2.Clear();

    wxPen yellowPen(clr, 1, wxSOLID);

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
    dc.SetFont( wxFont(12, wxMODERN, wxNORMAL, wxNORMAL, true) );
    dc.DrawText( wxT("This is text"), 110, 10 );
    dc.DrawRotatedText( wxT("That is text"), 20, 10, -45 );

    // use wxSWISS_FONT and not wxNORMAL_FONT as the latter can't be rotated
    // under Win9x (it is not TrueType)
    dc.SetFont( *wxSWISS_FONT );

    wxString text;
    dc.SetBackgroundMode(wxTRANSPARENT);

    for ( int n = -180; n < 180; n += 30 )
    {
        text.Printf(wxT("     %d rotated text"), n);
        dc.DrawRotatedText(text , 400, 400, n);
    }

    dc.SetFont( wxFont( 18, wxSWISS, wxNORMAL, wxNORMAL ) );

    dc.DrawText( wxT("This is Swiss 18pt text."), 110, 40 );

    wxCoord length;
    wxCoord height;
    wxCoord descent;
    dc.GetTextExtent( wxT("This is Swiss 18pt text."), &length, &height, &descent );
    text.Printf( wxT("Dimensions are length %d, height %d, descent %d"), length, height, descent );
    dc.DrawText( text, 110, 80 );

    text.Printf( wxT("CharHeight() returns: %d"), dc.GetCharHeight() );
    dc.DrawText( text, 110, 120 );

    dc.DrawRectangle( 100, 40, 4, height );

    // test the logical function effect
    wxCoord y = 150;
    dc.SetLogicalFunction(wxINVERT);
    // text drawing should ignore logical function
    dc.DrawText( wxT("There should be a text below"), 110, 150 );
    dc.DrawRectangle( 110, y, 100, height );

    y += height;
    dc.DrawText( wxT("Visible text"), 110, y );
    dc.DrawRectangle( 110, y, 100, height );
    dc.DrawText( wxT("Visible text"), 110, y );
    dc.DrawRectangle( 110, y, 100, height );
    dc.SetLogicalFunction(wxCOPY);

    y += height;
    dc.DrawRectangle( 110, y, 100, height );
    dc.DrawText( wxT("Another visible text"), 110, y );

    y += height;
    dc.DrawText("And\nmore\ntext on\nmultiple\nlines", 110, y);
}

static const struct
{
    const wxChar *name;
    wxRasterOperationMode rop;
} rasterOperations[] =
{
    { wxT("wxAND"),          wxAND           },
    { wxT("wxAND_INVERT"),   wxAND_INVERT    },
    { wxT("wxAND_REVERSE"),  wxAND_REVERSE   },
    { wxT("wxCLEAR"),        wxCLEAR         },
    { wxT("wxCOPY"),         wxCOPY          },
    { wxT("wxEQUIV"),        wxEQUIV         },
    { wxT("wxINVERT"),       wxINVERT        },
    { wxT("wxNAND"),         wxNAND          },
    { wxT("wxNO_OP"),        wxNO_OP         },
    { wxT("wxOR"),           wxOR            },
    { wxT("wxOR_INVERT"),    wxOR_INVERT     },
    { wxT("wxOR_REVERSE"),   wxOR_REVERSE    },
    { wxT("wxSET"),          wxSET           },
    { wxT("wxSRC_INVERT"),   wxSRC_INVERT    },
    { wxT("wxXOR"),          wxXOR           },
};

void MyCanvas::DrawImages(wxDC& dc, DrawMode mode)
{
    dc.DrawText(wxT("original image"), 0, 0);
    dc.DrawBitmap(*gs_bmpNoMask, 0, 20, 0);
    dc.DrawText(wxT("with colour mask"), 0, 100);
    dc.DrawBitmap(*gs_bmpWithColMask, 0, 120, true);
    dc.DrawText(wxT("the mask image"), 0, 200);
    dc.DrawBitmap(*gs_bmpMask, 0, 220, 0);
    dc.DrawText(wxT("masked image"), 0, 300);
    dc.DrawBitmap(*gs_bmpWithMask, 0, 320, true);

    int cx = gs_bmpWithColMask->GetWidth(),
        cy = gs_bmpWithColMask->GetHeight();

    wxMemoryDC memDC;
    for ( size_t n = 0; n < WXSIZEOF(rasterOperations); n++ )
    {
        wxCoord x = 120 + 150*(n%4),
                y =  20 + 100*(n/4);

        dc.DrawText(rasterOperations[n].name, x, y - 20);
        memDC.SelectObject(*gs_bmpWithColMask);
        if ( mode == Draw_Stretch )
        {
            dc.StretchBlit(x, y, cx, cy, &memDC, 0, 0, cx/2, cy/2,
                           rasterOperations[n].rop, true);
        }
        else
        {
            dc.Blit(x, y, cx, cy, &memDC, 0, 0, rasterOperations[n].rop, true);
        }
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

#if wxUSE_GRAPHICS_CONTEXT
#ifdef __WXGTK20__
void MyCanvas::DrawAlpha(wxDC& WXUNUSED(dummyDC))
#else
void MyCanvas::DrawAlpha(wxDC& dc)
#endif
{
#ifdef __WXGTK__
    wxGCDC dc( this );
    PrepareDC( dc );
#endif

    wxDouble margin = 20 ;
    wxDouble width = 180 ;
    wxDouble radius = 30 ;

    dc.SetPen( wxPen( wxColour( 128, 0, 0, 255 ),12, wxSOLID));
    dc.SetBrush( wxBrush( wxColour( 255, 0, 0, 255),wxSOLID));

    wxRect r(margin,margin+width*0.66,width,width) ;

    dc.DrawRoundedRectangle( r.x, r.y, r.width, r.width, radius ) ;

    dc.SetPen( wxPen( wxColour( 0, 0, 128, 255 ),12, wxSOLID));
    dc.SetBrush( wxBrush( wxColour( 0, 0, 255, 255),wxSOLID));

    r.Offset( width * 0.8 , - width * 0.66 ) ;

    dc.DrawRoundedRectangle( r.x, r.y, r.width, r.width, radius ) ;

    dc.SetPen( wxPen( wxColour( 128, 128, 0, 255 ),12, wxSOLID));
    dc.SetBrush( wxBrush( wxColour( 192, 192, 0, 255),wxSOLID));

    r.Offset( width * 0.8 , width *0.5 ) ;

    dc.DrawRoundedRectangle( r.x, r.y, r.width, r.width, radius ) ;

    dc.SetPen( *wxTRANSPARENT_PEN ) ;
    dc.SetBrush( wxBrush( wxColour(255,255,128,128) ) );
    dc.DrawRoundedRectangle( 0 , margin + width / 2 , width * 3 , 100 , radius) ;

    dc.SetTextForeground( wxColour(255,255,0,128) );
    dc.SetFont( wxFont( 40, wxFONTFAMILY_SWISS, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL ) );
    dc.DrawText( wxT("Hello!"), 120, 80 );
}

#endif

#if wxUSE_GRAPHICS_CONTEXT

const int BASE  = 80.0;
const int BASE2 = BASE/2;
const int BASE4 = BASE/4;

static inline double DegToRad(double deg) { return (deg * M_PI) / 180.0; }


// modeled along Robin Dunn's GraphicsContext.py sample

void MyCanvas::DrawGraphics(wxGraphicsContext* gc)
{
    wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    gc->SetFont(font,*wxBLACK);

    // make a path that contains a circle and some lines, centered at 0,0
    wxGraphicsPath path = gc->CreatePath() ;
    path.AddCircle( 0, 0, BASE2 );
    path.MoveToPoint(0, -BASE2);
    path.AddLineToPoint(0, BASE2);
    path.MoveToPoint(-BASE2, 0);
    path.AddLineToPoint(BASE2, 0);
    path.CloseSubpath();
    path.AddRectangle(-BASE4, -BASE4/2, BASE2, BASE4);

    // Now use that path to demonstrate various capbilites of the grpahics context
    gc->PushState(); // save current translation/scale/other state
    gc->Translate(60, 75); // reposition the context origin

    gc->SetPen(wxPen("navy", 1));
    gc->SetBrush(wxBrush("pink"));

    for( int i = 0 ; i < 3 ; ++i )
    {
        wxString label;
        switch( i )
        {
            case 0 :
                label = "StrokePath";
                break;
            case 1 :
                label = "FillPath";
                break;
            case 2 :
                label = "DrawPath";
                break;
        }
        wxDouble w, h;
        gc->GetTextExtent(label, &w, &h, NULL, NULL);
        gc->DrawText(label, -w/2, -BASE2-h-4);
        switch( i )
        {
            case 0 :
                gc->StrokePath(path);
                break;
            case 1 :
                gc->FillPath(path);
                break;
            case 2 :
                gc->DrawPath(path);
                break;
        }
        gc->Translate(2*BASE, 0);
    }

    gc->PopState(); // restore saved state
    gc->PushState(); // save it again
    gc->Translate(60, 200); // offset to the lower part of the window

    gc->DrawText("Scale", 0, -BASE2);
    gc->Translate(0, 20);

    gc->SetBrush(wxBrush(wxColour(178,  34,  34, 128)));// 128 == half transparent
    for( int i = 0 ; i < 8 ; ++i )
    {
        gc->Scale(1.08, 1.08); // increase scale by 8%
        gc->Translate(5,5);
        gc->DrawPath(path);
    }

    gc->PopState(); // restore saved state
    gc->PushState(); // save it again
    gc->Translate(400, 200);

    gc->DrawText("Rotate", 0, -BASE2);

    // Move the origin over to the next location
    gc->Translate(0, 75);

    // draw our path again, rotating it about the central point,
    // and changing colors as we go
    for ( int angle = 0 ; angle < 360 ; angle += 30 )
    {
        gc->PushState(); // save this new current state so we can
        //  pop back to it at the end of the loop
        wxImage::RGBValue val = wxImage::HSVtoRGB(wxImage::HSVValue(float(angle)/360, 1, 1));
        gc->SetBrush(wxBrush(wxColour(val.red, val.green, val.blue, 64)));
        gc->SetPen(wxPen(wxColour(val.red, val.green, val.blue, 128)));

        // use translate to artfully reposition each drawn path
        gc->Translate(1.5 * BASE2 * cos(DegToRad(angle)),
                     1.5 * BASE2 * sin(DegToRad(angle)));

        // use Rotate to rotate the path
        gc->Rotate(DegToRad(angle));

        // now draw it
        gc->DrawPath(path);
        gc->PopState();
    }
    gc->PopState();

    gc->PushState();
    gc->Translate(60, 400);
    gc->DrawText("Scaled smiley inside a square", 0, 0);
    gc->DrawRectangle(BASE2, BASE2, 100, 100);
    gc->DrawBitmap(m_smile_bmp, BASE2, BASE2, 100, 100);
    gc->PopState();
}
#endif // wxUSE_GRAPHICS_CONTEXT

void MyCanvas::DrawCircles(wxDC& dc)
{
    int x = 100,
        y = 100,
        r = 20;

    dc.SetPen( *wxRED_PEN );
    dc.SetBrush( *wxGREEN_BRUSH );

    dc.DrawText(wxT("Some circles"), 0, y);
    dc.DrawCircle(x, y, r);
    dc.DrawCircle(x + 2*r, y, r);
    dc.DrawCircle(x + 4*r, y, r);

    y += 2*r;
    dc.DrawText(wxT("And ellipses"), 0, y);
    dc.DrawEllipse(x - r, y, 2*r, r);
    dc.DrawEllipse(x + r, y, 2*r, r);
    dc.DrawEllipse(x + 3*r, y, 2*r, r);

    y += 2*r;
    dc.DrawText(wxT("And arcs"), 0, y);
    dc.DrawArc(x - r, y, x + r, y, x, y);
    dc.DrawArc(x + 4*r, y, x + 2*r, y, x + 3*r, y);
    dc.DrawArc(x + 5*r, y, x + 5*r, y, x + 6*r, y);

    y += 2*r;
    dc.DrawEllipticArc(x - r, y, 2*r, r, 0, 90);
    dc.DrawEllipticArc(x + r, y, 2*r, r, 90, 180);
    dc.DrawEllipticArc(x + 3*r, y, 2*r, r, 180, 270);
    dc.DrawEllipticArc(x + 5*r, y, 2*r, r, 270, 360);

    // same as above, just transparent brush

    dc.SetPen( *wxRED_PEN );
    dc.SetBrush( *wxTRANSPARENT_BRUSH );

    y += 2*r;
    dc.DrawText(wxT("Some circles"), 0, y);
    dc.DrawCircle(x, y, r);
    dc.DrawCircle(x + 2*r, y, r);
    dc.DrawCircle(x + 4*r, y, r);

    y += 2*r;
    dc.DrawText(wxT("And ellipses"), 0, y);
    dc.DrawEllipse(x - r, y, 2*r, r);
    dc.DrawEllipse(x + r, y, 2*r, r);
    dc.DrawEllipse(x + 3*r, y, 2*r, r);

    y += 2*r;
    dc.DrawText(wxT("And arcs"), 0, y);
    dc.DrawArc(x - r, y, x + r, y, x, y);
    dc.DrawArc(x + 4*r, y, x + 2*r, y, x + 3*r, y);
    dc.DrawArc(x + 5*r, y, x + 5*r, y, x + 6*r, y);

    y += 2*r;
    dc.DrawEllipticArc(x - r, y, 2*r, r, 0, 90);
    dc.DrawEllipticArc(x + r, y, 2*r, r, 90, 180);
    dc.DrawEllipticArc(x + 3*r, y, 2*r, r, 180, 270);
    dc.DrawEllipticArc(x + 5*r, y, 2*r, r, 270, 360);

}

void MyCanvas::DrawSplines(wxDC& dc)
{
#if wxUSE_SPLINES
    dc.DrawText(wxT("Some splines"), 10, 5);

    // values are hardcoded rather than randomly generated
    // so the output can be compared between native
    // implementations on platforms with different random
    // generators

    const int R = 300;
    const wxPoint center( R + 20, R + 20 );
    const int angles[7] = { 0, 10, 33, 77, 13, 145, 90 };
    const int radii[5] = { 100 , 59, 85, 33, 90 };
    const int n = 200;
    wxPoint pts[n];

    // background spline calculation
    unsigned int radius_pos = 0;
    unsigned int angle_pos = 0;
    int angle = 0;
    for ( int i = 0; i < n; i++ )
    {
        angle += angles[ angle_pos ];
        int r = R * radii[ radius_pos ] / 100;
        pts[ i ].x = center.x + (wxCoord)( r * cos( M_PI * angle / 180.0) );
        pts[ i ].y = center.y + (wxCoord)( r * sin( M_PI * angle / 180.0) );

        angle_pos++;
        if ( angle_pos >= WXSIZEOF(angles) ) angle_pos = 0;

        radius_pos++;
        if ( radius_pos >= WXSIZEOF(radii) ) radius_pos = 0;
    }

    // background spline drawing
    dc.SetPen(*wxRED_PEN);
    dc.DrawSpline(WXSIZEOF(pts), pts);

    // less detailed spline calculation
    wxPoint letters[4][5];
    // w
    letters[0][0] = wxPoint( 0,1); //  O           O
    letters[0][1] = wxPoint( 1,3); //   *         *
    letters[0][2] = wxPoint( 2,2); //    *   O   *
    letters[0][3] = wxPoint( 3,3); //     * * * *
    letters[0][4] = wxPoint( 4,1); //      O   O
    // x1
    letters[1][0] = wxPoint( 5,1); //  O*O
    letters[1][1] = wxPoint( 6,1); //     *
    letters[1][2] = wxPoint( 7,2); //      O
    letters[1][3] = wxPoint( 8,3); //       *
    letters[1][4] = wxPoint( 9,3); //        O*O
    // x2
    letters[2][0] = wxPoint( 5,3); //        O*O
    letters[2][1] = wxPoint( 6,3); //       *
    letters[2][2] = wxPoint( 7,2); //      O
    letters[2][3] = wxPoint( 8,1); //     *
    letters[2][4] = wxPoint( 9,1); //  O*O
    // W
    letters[3][0] = wxPoint(10,0); //  O           O
    letters[3][1] = wxPoint(11,3); //   *         *
    letters[3][2] = wxPoint(12,1); //    *   O   *
    letters[3][3] = wxPoint(13,3); //     * * * *
    letters[3][4] = wxPoint(14,0); //      O   O

    const int dx = 2 * R / letters[3][4].x;
    const int h[4] = { -R/2, 0, R/4, R/2 };

    for ( int m = 0; m < 4; m++ )
    {
        for ( int n = 0; n < 5; n++ )
        {
            letters[m][n].x = center.x - R + letters[m][n].x * dx;
            letters[m][n].y = center.y + h[ letters[m][n].y ];
        }

        dc.SetPen( wxPen( wxT("blue"), 1, wxDOT) );
        dc.DrawLines(5, letters[m]);
        dc.SetPen( wxPen( wxT("black"), 4, wxSOLID) );
        dc.DrawSpline(5, letters[m]);
    }

#else
    dc.DrawText(wxT("Splines not supported."), 10, 5);
#endif
}

void MyCanvas::DrawGradients(wxDC& dc)
{
    static const int TEXT_HEIGHT = 15;

    // LHS: linear
    wxRect r(10, 10, 50, 50);
    dc.DrawText(wxT("wxRIGHT"), r.x, r.y);
    r.Offset(0, TEXT_HEIGHT);
    dc.GradientFillLinear(r, *wxWHITE, *wxBLUE, wxRIGHT);

    r.Offset(0, r.height + 10);
    dc.DrawText(wxT("wxLEFT"), r.x, r.y);
    r.Offset(0, TEXT_HEIGHT);
    dc.GradientFillLinear(r, *wxWHITE, *wxBLUE, wxLEFT);

    r.Offset(0, r.height + 10);
    dc.DrawText(wxT("wxDOWN"), r.x, r.y);
    r.Offset(0, TEXT_HEIGHT);
    dc.GradientFillLinear(r, *wxWHITE, *wxBLUE, wxDOWN);

    r.Offset(0, r.height + 10);
    dc.DrawText(wxT("wxUP"), r.x, r.y);
    r.Offset(0, TEXT_HEIGHT);
    dc.GradientFillLinear(r, *wxWHITE, *wxBLUE, wxUP);

    wxRect  gfr = wxRect(r);

    // RHS: concentric
    r = wxRect(200, 10, 50, 50);
    dc.DrawText(wxT("Blue inside"), r.x, r.y);
    r.Offset(0, TEXT_HEIGHT);
    dc.GradientFillConcentric(r, *wxBLUE, *wxWHITE);

    r.Offset(0, r.height + 10);
    dc.DrawText(wxT("White inside"), r.x, r.y);
    r.Offset(0, TEXT_HEIGHT);
    dc.GradientFillConcentric(r, *wxWHITE, *wxBLUE);

    r.Offset(0, r.height + 10);
    dc.DrawText(wxT("Blue in top left corner"), r.x, r.y);
    r.Offset(0, TEXT_HEIGHT);
    dc.GradientFillConcentric(r, *wxBLUE, *wxWHITE, wxPoint(0, 0));

    r.Offset(0, r.height + 10);
    dc.DrawText(wxT("Blue in bottom right corner"), r.x, r.y);
    r.Offset(0, TEXT_HEIGHT);
    dc.GradientFillConcentric(r, *wxBLUE, *wxWHITE, wxPoint(r.width, r.height));

    // check that the area filled by the gradient is exactly the interior of
    // the rectangle
    r.x = 350;
    r.y = 30;
    dc.DrawText("The interior should be filled but", r.x, r.y);
    r.y += 15;
    dc.DrawText(" the red border should remain visible:", r.x, r.y);
    r.y += 15;

    r.width =
    r.height = 50;
    wxRect r2 = r;
    r2.x += 60;
    wxRect r3 = r;
    r3.y += 60;
    wxRect r4 = r2;
    r4.y += 60;
    dc.SetPen(wxPen(wxColour(255, 0, 0)));
    dc.DrawRectangle(r);
    r.Deflate(1);
    dc.GradientFillLinear(r, wxColour(0,255,0), wxColour(0,0,0), wxNORTH);
    dc.DrawRectangle(r2);
    r2.Deflate(1);
    dc.GradientFillLinear(r2, wxColour(0,0,0), wxColour(0,255,0), wxSOUTH);
    dc.DrawRectangle(r3);
    r3.Deflate(1);
    dc.GradientFillLinear(r3, wxColour(0,255,0), wxColour(0,0,0), wxEAST);
    dc.DrawRectangle(r4);
    r4.Deflate(1);
    dc.GradientFillLinear(r4, wxColour(0,0,0), wxColour(0,255,0), wxWEST);

#if wxUSE_GRAPHICS_CONTEXT
    if (m_useContext)
    {
        wxGCDC                      &gdc = (wxGCDC&)dc;
        wxGraphicsContext           *gc = gdc.GetGraphicsContext();
        wxGraphicsPath              pth;
        wxGraphicsGradientStops     stops;

        gfr.Offset(0, gfr.height + 10);
        dc.DrawText(wxT("Linear Gradient with Stops"), gfr.x, gfr.y);
        gfr.Offset(0, TEXT_HEIGHT);

        stops = wxGraphicsGradientStops(wxColour(255,0,0), wxColour(0,0,255));
        stops.Add(wxColour(255,255,0), 0.33f);
        stops.Add(wxColour(0,255,0), 0.67f);

        gc->SetBrush(gc->CreateLinearGradientBrush(gfr.x, gfr.y,
                                                   gfr.x + gfr.width, gfr.y + gfr.height,
                                                   stops));
        pth = gc->CreatePath();
        pth.MoveToPoint(gfr.x,gfr.y);
        pth.AddLineToPoint(gfr.x + gfr.width,gfr.y);
        pth.AddLineToPoint(gfr.x + gfr.width,gfr.y+gfr.height);
        pth.AddLineToPoint(gfr.x,gfr.y+gfr.height);
        pth.CloseSubpath();
        gc->FillPath(pth);

        gfr.Offset(0, gfr.height + 10);
        dc.DrawText(wxT("Radial Gradient with Stops"), gfr.x, gfr.y);
        gfr.Offset(0, TEXT_HEIGHT);

        gc->SetBrush(gc->CreateRadialGradientBrush(gfr.x + gfr.width / 2,
                                                   gfr.y + gfr.height / 2,
                                                   gfr.x + gfr.width / 2,
                                                   gfr.y + gfr.height / 2,
                                                   gfr.width / 2,
                                                   stops));
        pth = gc->CreatePath();
        pth.MoveToPoint(gfr.x,gfr.y);
        pth.AddLineToPoint(gfr.x + gfr.width,gfr.y);
        pth.AddLineToPoint(gfr.x + gfr.width,gfr.y+gfr.height);
        pth.AddLineToPoint(gfr.x,gfr.y+gfr.height);
        pth.CloseSubpath();
        gc->FillPath(pth);

        gfr.Offset(0, gfr.height + 10);
        dc.DrawText(wxT("Linear Gradient with Stops and Gaps"), gfr.x, gfr.y);
        gfr.Offset(0, TEXT_HEIGHT);

        stops = wxGraphicsGradientStops(wxColour(255,0,0), wxColour(0,0,255));
        stops.Add(wxColour(255,255,0), 0.33f);
        stops.Add(wxTransparentColour, 0.33f);
        stops.Add(wxTransparentColour, 0.67f);
        stops.Add(wxColour(0,255,0), 0.67f);

        gc->SetBrush(gc->CreateLinearGradientBrush(gfr.x, gfr.y + gfr.height,
                                                   gfr.x + gfr.width, gfr.y,
                                                   stops));
        pth = gc->CreatePath();
        pth.MoveToPoint(gfr.x,gfr.y);
        pth.AddLineToPoint(gfr.x + gfr.width,gfr.y);
        pth.AddLineToPoint(gfr.x + gfr.width,gfr.y+gfr.height);
        pth.AddLineToPoint(gfr.x,gfr.y+gfr.height);
        pth.CloseSubpath();
        gc->FillPath(pth);

        gfr.Offset(0, gfr.height + 10);
        dc.DrawText(wxT("Radial Gradient with Stops and Gaps"), gfr.x, gfr.y);
        gfr.Offset(0, TEXT_HEIGHT);

        gc->SetBrush(gc->CreateRadialGradientBrush(gfr.x + gfr.width / 2,
                                                   gfr.y + gfr.height / 2,
                                                   gfr.x + gfr.width / 2,
                                                   gfr.y + gfr.height / 2,
                                                   gfr.width / 2,
                                                   stops));
        pth = gc->CreatePath();
        pth.MoveToPoint(gfr.x,gfr.y);
        pth.AddLineToPoint(gfr.x + gfr.width,gfr.y);
        pth.AddLineToPoint(gfr.x + gfr.width,gfr.y+gfr.height);
        pth.AddLineToPoint(gfr.x,gfr.y+gfr.height);
        pth.CloseSubpath();
        gc->FillPath(pth);

        gfr.Offset(0, gfr.height + 10);
        dc.DrawText(wxT("Gradients with Stops and Transparency"), gfr.x, gfr.y);
        gfr.Offset(0, TEXT_HEIGHT);

        stops = wxGraphicsGradientStops(wxColour(255,0,0), wxTransparentColour);
        stops.Add(wxColour(255,0,0), 0.33f);
        stops.Add(wxTransparentColour, 0.33f);
        stops.Add(wxTransparentColour, 0.67f);
        stops.Add(wxColour(0,0,255), 0.67f);
        stops.Add(wxColour(0,0,255), 1.0f);

        pth = gc->CreatePath();
        pth.MoveToPoint(gfr.x,gfr.y);
        pth.AddLineToPoint(gfr.x + gfr.width,gfr.y);
        pth.AddLineToPoint(gfr.x + gfr.width,gfr.y+gfr.height);
        pth.AddLineToPoint(gfr.x,gfr.y+gfr.height);
        pth.CloseSubpath();

        gc->SetBrush(gc->CreateRadialGradientBrush(gfr.x + gfr.width / 2,
                                                   gfr.y + gfr.height / 2,
                                                   gfr.x + gfr.width / 2,
                                                   gfr.y + gfr.height / 2,
                                                   gfr.width / 2,
                                                   stops));
        gc->FillPath(pth);

        stops = wxGraphicsGradientStops(wxColour(255,0,0, 128), wxColour(0,0,255, 128));
        stops.Add(wxColour(255,255,0,128), 0.33f);
        stops.Add(wxColour(0,255,0,128), 0.67f);

        gc->SetBrush(gc->CreateLinearGradientBrush(gfr.x, gfr.y,
                                                   gfr.x + gfr.width, gfr.y,
                                                   stops));
        gc->FillPath(pth);
    }
#endif // wxUSE_GRAPHICS_CONTEXT
}

void MyCanvas::DrawRegions(wxDC& dc)
{
    dc.DrawText(wxT("You should see a red rect partly covered by a cyan one ")
                wxT("on the left"), 10, 5);
    dc.DrawText(wxT("and 5 smileys from which 4 are partially clipped on the right"),
                10, 5 + dc.GetCharHeight());
    dc.DrawText(wxT("The second copy should be identical but right part of it ")
                wxT("should be offset by 10 pixels."),
                10, 5 + 2*dc.GetCharHeight());

    DrawRegionsHelper(dc, 10, true);
    DrawRegionsHelper(dc, 350, false);
}

void MyCanvas::DrawRegionsHelper(wxDC& dc, wxCoord x, bool firstTime)
{
    wxCoord y = 100;

    dc.DestroyClippingRegion();
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.SetPen( *wxTRANSPARENT_PEN );
    dc.DrawRectangle( x, y, 310, 310 );

    dc.SetClippingRegion( x + 10, y + 10, 100, 270 );

    dc.SetBrush( *wxRED_BRUSH );
    dc.DrawRectangle( x, y, 310, 310 );

    dc.SetClippingRegion( x + 10, y + 10, 100, 100 );

    dc.SetBrush( *wxCYAN_BRUSH );
    dc.DrawRectangle( x, y, 310, 310 );

    dc.DestroyClippingRegion();

    wxRegion region(x + 110, y + 20, 100, 270);
#if !defined(__WXMOTIF__)
    if ( !firstTime )
        region.Offset(10, 10);
#endif
    dc.SetDeviceClippingRegion(region);

    dc.SetBrush( *wxGREY_BRUSH );
    dc.DrawRectangle( x, y, 310, 310 );

    if (m_smile_bmp.Ok())
    {
        dc.DrawBitmap( m_smile_bmp, x + 150, y + 150, true );
        dc.DrawBitmap( m_smile_bmp, x + 130, y + 10,  true );
        dc.DrawBitmap( m_smile_bmp, x + 130, y + 280, true );
        dc.DrawBitmap( m_smile_bmp, x + 100, y + 70,  true );
        dc.DrawBitmap( m_smile_bmp, x + 200, y + 70,  true );
    }
}

#if TEST_CAIRO_EVERYWHERE
extern wxGraphicsRenderer* gCairoRenderer;
#endif

void MyCanvas::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    wxPaintDC pdc(this);

#if wxUSE_GRAPHICS_CONTEXT
#if TEST_CAIRO_EVERYWHERE
    wxGCDC gdc;
    gdc.SetGraphicsContext( gCairoRenderer->CreateContext( pdc ) );
#else
     wxGCDC gdc( pdc ) ;
#endif
    wxDC &dc = m_useContext ? (wxDC&) gdc : (wxDC&) pdc ;
#else
    wxDC &dc = pdc ;
#endif

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
            wxColour clr(0,128,0);
            wxBrush b(clr, wxSOLID);
            dc.SetBackground(b);
        }
    }

    if ( m_clip )
        dc.SetClippingRegion(100, 100, 100, 100);

    dc.Clear();

    if ( m_owner->m_textureBackground )
    {
        dc.SetPen(*wxMEDIUM_GREY_PEN);
        for ( int i = 0; i < 200; i++ )
            dc.DrawLine(0, i*10, i*10, 0);
    }

    switch ( m_show )
    {
        case File_ShowDefault:
            DrawDefault(dc);
            break;

        case File_ShowCircles:
            DrawCircles(dc);
            break;

        case File_ShowSplines:
            DrawSplines(dc);
            break;

        case File_ShowRegions:
            DrawRegions(dc);
            break;

        case File_ShowText:
            DrawText(dc);
            break;

        case File_ShowLines:
            DrawTestLines( 0, 100, 0, dc );
            DrawTestLines( 0, 320, 1, dc );
            DrawTestLines( 0, 540, 2, dc );
            DrawTestLines( 0, 760, 6, dc );
            break;

        case File_ShowBrushes:
            DrawTestBrushes(dc);
            break;

        case File_ShowPolygons:
            DrawTestPoly(dc);
            break;

        case File_ShowMask:
            DrawImages(dc, Draw_Normal);
            break;

        case File_ShowMaskStretch:
            DrawImages(dc, Draw_Stretch);
            break;

        case File_ShowOps:
            DrawWithLogicalOps(dc);
            break;

#if wxUSE_GRAPHICS_CONTEXT
        case File_ShowAlpha:
            DrawAlpha(dc);
            break;
        case File_ShowGraphics:
            DrawGraphics(gdc.GetGraphicsContext());
            break;
#endif

        case File_ShowGradients:
            DrawGradients(dc);
            break;

        default:
            break;
    }
}

void MyCanvas::OnMouseMove(wxMouseEvent &event)
{
#if wxUSE_STATUSBAR
    {
        wxClientDC dc(this);
        PrepareDC(dc);
        m_owner->PrepareDC(dc);

        wxPoint pos = event.GetPosition();
        long x = dc.DeviceToLogicalX( pos.x );
        long y = dc.DeviceToLogicalY( pos.y );
        wxString str;
        str.Printf( wxT("Current mouse position: %d,%d"), (int)x, (int)y );
        m_owner->SetStatusText( str );
    }

    if ( m_rubberBand )
    {
        int x,y, xx, yy ;
        event.GetPosition(&x,&y);
        CalcUnscrolledPosition( x, y, &xx, &yy );
        m_currentpoint = wxPoint( xx , yy ) ;
        wxRect newrect ( m_anchorpoint , m_currentpoint ) ;

        wxClientDC dc( this ) ;
        PrepareDC( dc ) ;

        wxDCOverlay overlaydc( m_overlay, &dc );
        overlaydc.Clear();
#ifdef __WXMAC__
        dc.SetPen( *wxGREY_PEN );
        dc.SetBrush( wxColour( 192,192,192,64 ) );
#else
        dc.SetPen( wxPen( *wxLIGHT_GREY, 2, wxSOLID ) );
        dc.SetBrush( *wxTRANSPARENT_BRUSH );
#endif
        dc.DrawRectangle( newrect );
    }
#else
    wxUnusedVar(event);
#endif // wxUSE_STATUSBAR
}

void MyCanvas::OnMouseDown(wxMouseEvent &event)
{
    int x,y,xx,yy ;
    event.GetPosition(&x,&y);
    CalcUnscrolledPosition( x, y, &xx, &yy );
    m_anchorpoint = wxPoint( xx , yy ) ;
    m_currentpoint = m_anchorpoint ;
    m_rubberBand = true ;
    CaptureMouse() ;
}

void MyCanvas::OnMouseUp(wxMouseEvent &event)
{
    if ( m_rubberBand )
    {
        ReleaseMouse();
        {
            wxClientDC dc( this );
            PrepareDC( dc );
            wxDCOverlay overlaydc( m_overlay, &dc );
            overlaydc.Clear();
        }
        m_overlay.Reset();
        m_rubberBand = false;

        wxPoint endpoint = CalcUnscrolledPosition(event.GetPosition());

        // Don't pop up the message box if nothing was actually selected.
        if ( endpoint != m_anchorpoint )
        {
            wxLogMessage("Selected rectangle from (%d, %d) to (%d, %d)",
                         m_anchorpoint.x, m_anchorpoint.y,
                         endpoint.x, endpoint.y);
        }
    }
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU      (File_Quit,     MyFrame::OnQuit)
    EVT_MENU      (File_About,    MyFrame::OnAbout)
    EVT_MENU      (File_Clip,     MyFrame::OnClip)
#if wxUSE_GRAPHICS_CONTEXT
    EVT_MENU      (File_GraphicContext, MyFrame::OnGraphicContext)
#endif

    EVT_MENU_RANGE(MenuShow_First,   MenuShow_Last,   MyFrame::OnShow)

    EVT_MENU_RANGE(MenuOption_First, MenuOption_Last, MyFrame::OnOption)
END_EVENT_TABLE()

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size,
                 wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE)
{
    // set the frame icon
    SetIcon(wxICON(sample));

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(File_ShowDefault, wxT("&Default screen\tF1"));
    menuFile->Append(File_ShowText, wxT("&Text screen\tF2"));
    menuFile->Append(File_ShowLines, wxT("&Lines screen\tF3"));
    menuFile->Append(File_ShowBrushes, wxT("&Brushes screen\tF4"));
    menuFile->Append(File_ShowPolygons, wxT("&Polygons screen\tF5"));
    menuFile->Append(File_ShowMask, wxT("&Mask screen\tF6"));
    menuFile->Append(File_ShowMaskStretch, wxT("1/&2 scaled mask\tShift-F6"));
    menuFile->Append(File_ShowOps, wxT("&Raster operations screen\tF7"));
    menuFile->Append(File_ShowRegions, wxT("Re&gions screen\tF8"));
    menuFile->Append(File_ShowCircles, wxT("&Circles screen\tF9"));
#if wxUSE_GRAPHICS_CONTEXT
    menuFile->Append(File_ShowAlpha, wxT("&Alpha screen\tF10"));
#endif
    menuFile->Append(File_ShowSplines, wxT("&Splines screen\tF11"));
    menuFile->Append(File_ShowGradients, wxT("&Gradients screen\tF12"));
#if wxUSE_GRAPHICS_CONTEXT
     menuFile->Append(File_ShowGraphics, wxT("&Graphics screen"));
#endif
    menuFile->AppendSeparator();
    menuFile->AppendCheckItem(File_Clip, wxT("&Clip\tCtrl-C"), wxT("Clip/unclip drawing"));
#if wxUSE_GRAPHICS_CONTEXT
    menuFile->AppendCheckItem(File_GraphicContext, wxT("&Use GraphicContext\tCtrl-Y"), wxT("Use GraphicContext"));
#endif
    menuFile->AppendSeparator();
    menuFile->Append(File_About, wxT("&About...\tCtrl-A"), wxT("Show about dialog"));
    menuFile->AppendSeparator();
    menuFile->Append(File_Quit, wxT("E&xit\tAlt-X"), wxT("Quit this program"));

    wxMenu *menuMapMode = new wxMenu;
    menuMapMode->Append( MapMode_Text, wxT("&TEXT map mode") );
    menuMapMode->Append( MapMode_Lometric, wxT("&LOMETRIC map mode") );
    menuMapMode->Append( MapMode_Twips, wxT("T&WIPS map mode") );
    menuMapMode->Append( MapMode_Points, wxT("&POINTS map mode") );
    menuMapMode->Append( MapMode_Metric, wxT("&METRIC map mode") );

    wxMenu *menuUserScale = new wxMenu;
    menuUserScale->Append( UserScale_StretchHoriz, wxT("Stretch &horizontally\tCtrl-H") );
    menuUserScale->Append( UserScale_ShrinkHoriz, wxT("Shrin&k horizontally\tCtrl-G") );
    menuUserScale->Append( UserScale_StretchVertic, wxT("Stretch &vertically\tCtrl-V") );
    menuUserScale->Append( UserScale_ShrinkVertic, wxT("&Shrink vertically\tCtrl-W") );
    menuUserScale->AppendSeparator();
    menuUserScale->Append( UserScale_Restore, wxT("&Restore to normal\tCtrl-0") );

    wxMenu *menuAxis = new wxMenu;
    menuAxis->AppendCheckItem( AxisMirror_Horiz, wxT("Mirror horizontally\tCtrl-M") );
    menuAxis->AppendCheckItem( AxisMirror_Vertic, wxT("Mirror vertically\tCtrl-N") );

    wxMenu *menuLogical = new wxMenu;
    menuLogical->Append( LogicalOrigin_MoveDown, wxT("Move &down\tCtrl-D") );
    menuLogical->Append( LogicalOrigin_MoveUp, wxT("Move &up\tCtrl-U") );
    menuLogical->Append( LogicalOrigin_MoveLeft, wxT("Move &right\tCtrl-L") );
    menuLogical->Append( LogicalOrigin_MoveRight, wxT("Move &left\tCtrl-R") );
    menuLogical->AppendSeparator();
    menuLogical->Append( LogicalOrigin_Set, wxT("Set to (&100, 100)\tShift-Ctrl-1") );
    menuLogical->Append( LogicalOrigin_Restore, wxT("&Restore to normal\tShift-Ctrl-0") );

    wxMenu *menuColour = new wxMenu;
#if wxUSE_COLOURDLG
    menuColour->Append( Colour_TextForeground, wxT("Text &foreground...") );
    menuColour->Append( Colour_TextBackground, wxT("Text &background...") );
    menuColour->Append( Colour_Background, wxT("Background &colour...") );
#endif // wxUSE_COLOURDLG
    menuColour->AppendCheckItem( Colour_BackgroundMode, wxT("&Opaque/transparent\tCtrl-B") );
    menuColour->AppendCheckItem( Colour_TextureBackgound, wxT("Draw textured back&ground\tCtrl-T") );

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, wxT("&File"));
    menuBar->Append(menuMapMode, wxT("&Mode"));
    menuBar->Append(menuUserScale, wxT("&Scale"));
    menuBar->Append(menuAxis, wxT("&Axis"));
    menuBar->Append(menuLogical, wxT("&Origin"));
    menuBar->Append(menuColour, wxT("&Colours"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
    SetStatusText(wxT("Welcome to wxWidgets!"));
#endif // wxUSE_STATUSBAR

    m_mapMode = wxMM_TEXT;
    m_xUserScale = 1.0;
    m_yUserScale = 1.0;
    m_xLogicalOrigin = 0;
    m_yLogicalOrigin = 0;
    m_xAxisReversed =
    m_yAxisReversed = false;
    m_backgroundMode = wxSOLID;
    m_colourForeground = *wxBLACK;
    m_colourBackground = *wxLIGHT_GREY;
    m_textureBackground = false;

    m_canvas = new MyCanvas( this );
    m_canvas->SetScrollbars( 10, 10, 100, 240 );
}

// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( wxT("This is the about dialog of the drawing sample.\n")
                wxT("This sample tests various primitive drawing functions\n")
                wxT("(without any attempts to prevent flicker).\n")
                wxT("Copyright (c) Robert Roebling 1999")
              );

    wxMessageBox(msg, wxT("About Drawing"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnClip(wxCommandEvent& event)
{
    m_canvas->Clip(event.IsChecked());
}

#if wxUSE_GRAPHICS_CONTEXT
void MyFrame::OnGraphicContext(wxCommandEvent& event)
{
    m_canvas->UseGraphicContext(event.IsChecked());
}
#endif

void MyFrame::OnShow(wxCommandEvent& event)
{
    m_canvas->ToShow(event.GetId());
}

void MyFrame::OnOption(wxCommandEvent& event)
{
    switch (event.GetId())
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
        case LogicalOrigin_Set:
            m_xLogicalOrigin =
            m_yLogicalOrigin = -100;
            break;
        case LogicalOrigin_Restore:
            m_xLogicalOrigin =
            m_yLogicalOrigin = 0;
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

#if wxUSE_COLOURDLG
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
#endif // wxUSE_COLOURDLG

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
    dc.SetLogicalOrigin( m_xLogicalOrigin, m_yLogicalOrigin );
    dc.SetAxisOrientation( !m_xAxisReversed, m_yAxisReversed );
    dc.SetUserScale( m_xUserScale, m_yUserScale );
    dc.SetMapMode( m_mapMode );
}

#if wxUSE_COLOURDLG
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
#endif // wxUSE_COLOURDLG

/////////////////////////////////////////////////////////////////////////////
// Name:        samples/drawing/drawing.cpp
// Purpose:     shows and tests wxDC features
// Author:      Robert Roebling
// Modified by:
// Created:     04/01/98
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


// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/colordlg.h"
#include "wx/image.h"
#include "wx/artprov.h"
#include "wx/dcbuffer.h"
#include "wx/dcgraph.h"
#include "wx/overlay.h"
#include "wx/graphics.h"
#include "wx/filename.h"
#include "wx/metafile.h"
#include "wx/settings.h"
#if wxUSE_SVG
#include "wx/dcsvg.h"
#endif
#if wxUSE_POSTSCRIPT
#include "wx/dcps.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// Standard DC supports drawing with alpha on OSX and GTK3.
#if defined(__WXOSX__) || defined(__WXGTK3__)
#define wxDRAWING_DC_SUPPORTS_ALPHA 1
#else
#define wxDRAWING_DC_SUPPORTS_ALPHA 0
#endif // __WXOSX__ || __WXGTK3__

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

static wxBitmap *gs_bmpNoMask = nullptr,
                *gs_bmpWithColMask = nullptr,
                *gs_bmpMask = nullptr,
                *gs_bmpWithMask = nullptr,
                *gs_bmp4 = nullptr,
                *gs_bmp4_mono = nullptr,
                *gs_bmp36 = nullptr;

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
    virtual bool OnInit() override;

    virtual int OnExit() override { DeleteBitmaps(); return 0; }

protected:
    void DeleteBitmaps();

    bool LoadImages();
};

class MyFrame;

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
    int GetPage() { return m_show; }

    // set or remove the clipping region
    void Clip(bool clip) { m_clip = clip; Refresh(); }
#if wxUSE_GRAPHICS_CONTEXT
    bool HasRenderer() const { return m_renderer != nullptr; }
    void UseGraphicRenderer(wxGraphicsRenderer* renderer);
    bool IsDefaultRenderer() const
    {   if ( !m_renderer ) return false;
        return m_renderer == wxGraphicsRenderer::GetDefaultRenderer();
    }
    wxGraphicsRenderer* GetRenderer() const { return m_renderer; }
    void EnableAntiAliasing(bool use) { m_useAntiAliasing = use; Refresh(); }
#endif // wxUSE_GRAPHICS_CONTEXT
    void UseBuffer(bool use) { m_useBuffer = use; Refresh(); }
    void ShowBoundingBox(bool show) { m_showBBox = show; Refresh(); }
    wxSize GetDIPDrawingSize() const;

    void Draw(wxDC& dc);

protected:
    enum DrawMode
    {
        Draw_Normal,
        Draw_Stretch
    };

    void DrawTestLines( int x, int y, int width, wxDC &dc );
    void DrawCrossHair(int x, int y, int width, int heigth, wxDC &dc);
    void DrawTestPoly(wxDC& dc);
    void DrawTestBrushes(wxDC& dc);
    void DrawText(wxDC& dc);
    void DrawImages(wxDC& dc, DrawMode mode);
    void DrawWithLogicalOps(wxDC& dc);
#if wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
    void DrawAlpha(wxDC& dc);
#endif
#if wxUSE_GRAPHICS_CONTEXT
    void DrawGraphics(wxGraphicsContext* gc);
#endif
    void DrawRegions(wxDC& dc);
    void DrawCircles(wxDC& dc);
    void DrawSplines(wxDC& dc);
    void DrawDefault(wxDC& dc);
    void DrawGradients(wxDC& dc);
    void DrawSystemColours(wxDC& dc);
    void DrawDatabaseColours(wxDC& dc);
    void DrawColour(wxDC& dc, const wxFont& mono, wxCoord x, const wxRect& r, const wxString& colourName, const wxColour& col);

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
    wxGraphicsRenderer* m_renderer;
    bool         m_useAntiAliasing;
#endif
    bool         m_useBuffer;
    bool         m_showBBox;
    wxSize       m_sizeDIP;

    wxDECLARE_EVENT_TABLE();
};


// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnClip(wxCommandEvent& event);

#if wxUSE_GRAPHICS_CONTEXT
    void OnGraphicContextNone(wxCommandEvent& WXUNUSED(event))
    {
        m_canvas->UseGraphicRenderer(nullptr);
    }

    void OnGraphicContextDefault(wxCommandEvent& WXUNUSED(event))
    {
        m_canvas->UseGraphicRenderer(wxGraphicsRenderer::GetDefaultRenderer());
    }

#if wxUSE_CAIRO
    void OnGraphicContextCairo(wxCommandEvent& WXUNUSED(event))
    {
        m_canvas->UseGraphicRenderer(wxGraphicsRenderer::GetCairoRenderer());
    }
#endif // wxUSE_CAIRO
#ifdef __WXMSW__
#if wxUSE_GRAPHICS_GDIPLUS
    void OnGraphicContextGDIPlus(wxCommandEvent& WXUNUSED(event))
    {
        m_canvas->UseGraphicRenderer(wxGraphicsRenderer::GetGDIPlusRenderer());
    }
#endif
#if wxUSE_GRAPHICS_DIRECT2D
    void OnGraphicContextDirect2D(wxCommandEvent& WXUNUSED(event))
    {
        m_canvas->UseGraphicRenderer(wxGraphicsRenderer::GetDirect2DRenderer());
    }
#endif
#endif // __WXMSW__
    void OnAntiAliasing(wxCommandEvent& event)
    {
        m_canvas->EnableAntiAliasing(event.IsChecked());
    }

    void OnAntiAliasingUpdateUI(wxUpdateUIEvent& event)
    {
        event.Enable(m_canvas->GetRenderer() != nullptr);
    }
#endif // wxUSE_GRAPHICS_CONTEXT

    void OnBuffer(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnShow(wxCommandEvent &event);
    void OnOption(wxCommandEvent &event);
    void OnBoundingBox(wxCommandEvent& evt);
    void OnBoundingBoxUpdateUI(wxUpdateUIEvent& evt);

#if wxUSE_COLOURDLG
    wxColour SelectColour();
#endif // wxUSE_COLOURDLG
    void PrepareDC(wxDC& dc) override;

    int         m_backgroundMode;
    int         m_textureBackground;
    wxMappingMode m_mapMode;
    double      m_xUserScale;
    double      m_yUserScale;
    int         m_xLogicalOrigin;
    int         m_yLogicalOrigin;
    bool        m_xAxisReversed,
                m_yAxisReversed;
#if wxUSE_DC_TRANSFORM_MATRIX
    wxDouble    m_transform_dx;
    wxDouble    m_transform_dy;
    wxDouble    m_transform_scx;
    wxDouble    m_transform_scy;
    wxDouble    m_transform_rot;
#endif // wxUSE_DC_TRANSFORM_MATRIX
    wxColour    m_colourForeground,    // these are _text_ colours
                m_colourBackground;
    wxBrush     m_backgroundBrush;
    MyCanvas   *m_canvas;
    wxMenuItem *m_menuItemUseDC;
private:
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
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
#if wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
    File_ShowAlpha,
#endif // wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
#if wxUSE_GRAPHICS_CONTEXT
    File_ShowGraphics,
#endif
    File_ShowSystemColours,
    File_ShowDatabaseColours,
    File_ShowGradients,
    MenuShow_Last = File_ShowGradients,

#if wxUSE_GRAPHICS_CONTEXT
    File_DC,
    File_GC_Default,
#if wxUSE_CAIRO
    File_GC_Cairo,
#endif // wxUSE_CAIRO
#ifdef __WXMSW__
#if wxUSE_GRAPHICS_GDIPLUS
    File_GC_GDIPlus,
#endif
#if wxUSE_GRAPHICS_DIRECT2D
    File_GC_Direct2D,
#endif
#endif // __WXMSW__
#endif // wxUSE_GRAPHICS_CONTEXT
    File_BBox,
    File_Clip,
    File_Buffer,
#if wxUSE_GRAPHICS_CONTEXT
    File_AntiAliasing,
#endif
    File_Copy,
    File_Save,

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

#if wxUSE_DC_TRANSFORM_MATRIX
    TransformMatrix_Set,
    TransformMatrix_Reset,
#endif // wxUSE_DC_TRANSFORM_MATRIX

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
wxIMPLEMENT_APP(MyApp);

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
    pathList.Add(".");
    pathList.Add("..");
    pathList.Add("../drawing");
    pathList.Add("../../../samples/drawing");

    wxString path = pathList.FindValidPath("pat4.bmp");
    if ( !path )
        return false;

    /* 4 colour bitmap */
    gs_bmp4->LoadFile(path, wxBITMAP_TYPE_BMP);
    /* turn into mono-bitmap */
    gs_bmp4_mono->LoadFile(path, wxBITMAP_TYPE_BMP);
    wxMask* mask4 = new wxMask(*gs_bmp4_mono, *wxBLACK);
    gs_bmp4_mono->SetMask(mask4);

    path = pathList.FindValidPath("pat36.bmp");
    if ( !path )
        return false;
    gs_bmp36->LoadFile(path, wxBITMAP_TYPE_BMP);
    wxMask* mask36 = new wxMask(*gs_bmp36, *wxBLACK);
    gs_bmp36->SetMask(mask36);

    path = pathList.FindValidPath("image.bmp");
    if ( !path )
        return false;
    gs_bmpNoMask->LoadFile(path, wxBITMAP_TYPE_BMP);
    gs_bmpWithMask->LoadFile(path, wxBITMAP_TYPE_BMP);
    gs_bmpWithColMask->LoadFile(path, wxBITMAP_TYPE_BMP);

    path = pathList.FindValidPath("mask.bmp");
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

#if wxUSE_LIBPNG
      wxImage::AddHandler( new wxPNGHandler );
#endif

    // Create the main application window
    MyFrame *frame = new MyFrame("Drawing sample");

    // Show it
    frame->Show(true);

    if ( !LoadImages() )
    {
        wxLogError("Can't load one of the bitmap files needed "
                   "for this sample from the current or parent "
                   "directory, please copy them there.");

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
wxBEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT  (MyCanvas::OnPaint)
    EVT_MOTION (MyCanvas::OnMouseMove)
    EVT_LEFT_DOWN (MyCanvas::OnMouseDown)
    EVT_LEFT_UP (MyCanvas::OnMouseUp)
wxEND_EVENT_TABLE()

#include "smile.xpm"

MyCanvas::MyCanvas(MyFrame *parent)
        : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                           wxHSCROLL | wxVSCROLL)
{
    m_owner = parent;
    m_show = File_ShowDefault;
    m_smile_bmp = wxBitmap(smile_xpm);
    m_std_icon = wxArtProvider::GetIcon(wxART_INFORMATION);
    m_clip = false;
    m_rubberBand = false;
#if wxUSE_GRAPHICS_CONTEXT
    m_renderer = nullptr;
    m_useAntiAliasing = true;
#endif
    m_useBuffer = false;
    m_showBBox = false;
    m_sizeDIP = wxSize(0, 0);

    Bind(wxEVT_SYS_COLOUR_CHANGED, [this](wxSysColourChangedEvent& event) {
        event.Skip();

        if ( m_show == File_ShowSystemColours )
            Refresh();
    });
}

void MyCanvas::DrawTestBrushes(wxDC& dc)
{
    const wxCoord WIDTH = dc.FromDIP(200);
    const wxCoord HEIGHT = dc.FromDIP(80);

    wxCoord x = dc.FromDIP(10),
            y = dc.FromDIP(10),
            o = dc.FromDIP(10);

    dc.SetBrush(*wxGREEN_BRUSH);
    dc.DrawRectangle(x, y, WIDTH, HEIGHT);
    dc.DrawText("Solid green", x + o, y + o);

    y += HEIGHT;
    dc.SetBrush(wxBrush(*wxRED, wxBRUSHSTYLE_CROSSDIAG_HATCH));
    dc.DrawRectangle(x, y, WIDTH, HEIGHT);
    dc.DrawText("Diagonally hatched red", x + o, y + o);

    y += HEIGHT;
    dc.SetBrush(wxBrush(*wxBLUE, wxBRUSHSTYLE_CROSS_HATCH));
    dc.DrawRectangle(x, y, WIDTH, HEIGHT);
    dc.DrawText("Cross hatched blue", x + o, y + o);

    y += HEIGHT;
    dc.SetBrush(wxBrush(*wxCYAN, wxBRUSHSTYLE_VERTICAL_HATCH));
    dc.DrawRectangle(x, y, WIDTH, HEIGHT);
    dc.DrawText("Vertically hatched cyan", x + o, y + o);

    y += HEIGHT;
    dc.SetBrush(wxBrush(*wxBLACK, wxBRUSHSTYLE_HORIZONTAL_HATCH));
    dc.DrawRectangle(x, y, WIDTH, HEIGHT);
    dc.DrawText("Horizontally hatched black", x + o, y + o);

    y += HEIGHT;
    dc.SetBrush(wxBrush(*gs_bmpMask));
    dc.DrawRectangle(x, y, WIDTH, HEIGHT);
    dc.DrawText("Stipple mono", x + o, y + o);

    y += HEIGHT;
    dc.SetBrush(wxBrush(*gs_bmpNoMask));
    dc.DrawRectangle(x, y, WIDTH, HEIGHT);
    dc.DrawText("Stipple colour", x + o, y + o);
}

void MyCanvas::DrawTestPoly(wxDC& dc)
{
    wxBrush brushHatch(*wxRED, wxBRUSHSTYLE_FDIAGONAL_HATCH);
    dc.SetBrush(brushHatch);

    wxPoint star[5];
    star[0] = dc.FromDIP(wxPoint(100, 60));
    star[1] = dc.FromDIP(wxPoint(60, 150));
    star[2] = dc.FromDIP(wxPoint(160, 100));
    star[3] = dc.FromDIP(wxPoint(40, 100));
    star[4] = dc.FromDIP(wxPoint(140, 150));

    dc.DrawText("You should see two (irregular) stars below, the left one "
                "hatched", dc.FromDIP(10), dc.FromDIP(10));
    dc.DrawText("except for the central region and the right "
                "one entirely hatched", dc.FromDIP(10), dc.FromDIP(30));
    dc.DrawText("The third star only has a hatched outline", dc.FromDIP(10), dc.FromDIP(50));

    dc.DrawPolygon(WXSIZEOF(star), star, 0, dc.FromDIP(30));
    dc.DrawPolygon(WXSIZEOF(star), star, dc.FromDIP(160), dc.FromDIP(30), wxWINDING_RULE);

    wxBrush brushHatchGreen(*wxGREEN, wxBRUSHSTYLE_FDIAGONAL_HATCH);
    dc.SetBrush(brushHatchGreen);
    wxPoint star2[10];
    star2[0] = dc.FromDIP(wxPoint(0, 100));
    star2[1] = dc.FromDIP(wxPoint(-59, -81));
    star2[2] = dc.FromDIP(wxPoint(95, 31));
    star2[3] = dc.FromDIP(wxPoint(-95, 31));
    star2[4] = dc.FromDIP(wxPoint(59, -81));
    star2[5] = dc.FromDIP(wxPoint(0, 80));
    star2[6] = dc.FromDIP(wxPoint(-47, -64));
    star2[7] = dc.FromDIP(wxPoint(76, 24));
    star2[8] = dc.FromDIP(wxPoint(-76, 24));
    star2[9] = dc.FromDIP(wxPoint(47, -64));
    int count[2] = {5, 5};

    dc.DrawPolyPolygon(WXSIZEOF(count), count, star2, dc.FromDIP(450), dc.FromDIP(150));
}

void MyCanvas::DrawTestLines( int x, int y, int width, wxDC &dc )
{
    dc.SetPen( wxPen( *wxBLACK, width ) );
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawText(wxString::Format("Testing lines of width %d", width), dc.FromDIP(x + 10), dc.FromDIP(y - 10));
    dc.DrawRectangle(dc.FromDIP(x + 10), dc.FromDIP(y + 10), dc.FromDIP(100), dc.FromDIP(190));

    dc.DrawText("Solid/dot/short dash/long dash/dot dash", dc.FromDIP(x + 150), dc.FromDIP(y + 10));
    dc.SetPen( wxPen( *wxBLACK, width ) );
    dc.DrawLine(dc.FromDIP(x + 20), dc.FromDIP(y + 20), dc.FromDIP(100), dc.FromDIP(y + 20));
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_DOT) );
    dc.DrawLine(dc.FromDIP(x + 20), dc.FromDIP(y + 30), dc.FromDIP(100), dc.FromDIP(y + 30));
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_SHORT_DASH) );
    dc.DrawLine(dc.FromDIP(x + 20), dc.FromDIP(y + 40), dc.FromDIP(100), dc.FromDIP(y + 40));
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_LONG_DASH) );
    dc.DrawLine(dc.FromDIP(x + 20), dc.FromDIP(y + 50), dc.FromDIP(100), dc.FromDIP(y + 50));
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_DOT_DASH) );
    dc.DrawLine(dc.FromDIP(x + 20), dc.FromDIP(y + 60), dc.FromDIP(100), dc.FromDIP(y + 60));

    dc.DrawText("Hatches", dc.FromDIP(x + 150), dc.FromDIP(y + 70));
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_BDIAGONAL_HATCH) );
    dc.DrawLine(dc.FromDIP(x + 20), dc.FromDIP(y + 70), dc.FromDIP(100), dc.FromDIP(y + 70));
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_CROSSDIAG_HATCH) );
    dc.DrawLine(dc.FromDIP(x + 20), dc.FromDIP(y + 80), dc.FromDIP(100), dc.FromDIP(y + 80));
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_FDIAGONAL_HATCH) );
    dc.DrawLine(dc.FromDIP(x + 20), dc.FromDIP(y + 90), dc.FromDIP(100), dc.FromDIP(y + 90));
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_CROSS_HATCH) );
    dc.DrawLine(dc.FromDIP(x + 20), dc.FromDIP(y + 100), dc.FromDIP(100), dc.FromDIP(y + 100));
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_HORIZONTAL_HATCH) );
    dc.DrawLine(dc.FromDIP(x + 20), dc.FromDIP(y + 110), dc.FromDIP(100), dc.FromDIP(y + 110));
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_VERTICAL_HATCH) );
    dc.DrawLine(dc.FromDIP(x + 20), dc.FromDIP(y + 120), dc.FromDIP(100), dc.FromDIP(y + 120));

    dc.DrawText("User dash", dc.FromDIP(x + 150), dc.FromDIP(y + 140));
    wxPen ud( *wxBLACK, width, wxPENSTYLE_USER_DASH );
    wxDash dash1[6];
    dash1[0] = 8;  // Long dash  <---------+
    dash1[1] = 2;  // Short gap            |
    dash1[2] = 3;  // Short dash           |
    dash1[3] = 2;  // Short gap            |
    dash1[4] = 3;  // Short dash           |
    dash1[5] = 2;  // Short gap and repeat +
    ud.SetDashes( 6, dash1 );
    dc.SetPen( ud );
    dc.DrawLine(dc.FromDIP(x + 20), dc.FromDIP(y + 140), dc.FromDIP(100), dc.FromDIP(y + 140));
    dash1[0] = 5;  // Make first dash shorter
    ud.SetDashes( 6, dash1 );
    dc.SetPen( ud );
    dc.DrawLine(dc.FromDIP(x + 20), dc.FromDIP(y + 150), dc.FromDIP(100), dc.FromDIP(y + 150));
    dash1[2] = 5;  // Make second dash longer
    ud.SetDashes( 6, dash1 );
    dc.SetPen( ud );
    dc.DrawLine(dc.FromDIP(x + 20), dc.FromDIP(y + 160), dc.FromDIP(100), dc.FromDIP(y + 160));
    dash1[4] = 5;  // Make third dash longer
    ud.SetDashes( 6, dash1 );
    dc.SetPen( ud );
    dc.DrawLine(dc.FromDIP(x + 20), dc.FromDIP(y + 170), dc.FromDIP(100), dc.FromDIP(y + 170));

    wxPen penWithCap(*wxBLACK, width);
    dc.SetPen(penWithCap);
    dc.DrawText("Default cap", dc.FromDIP(x + 270), dc.FromDIP(y + 40));
    dc.DrawLine(dc.FromDIP(x + 200), dc.FromDIP(y + 50), dc.FromDIP(x + 250), dc.FromDIP(y + 50));

    penWithCap.SetCap(wxCAP_BUTT);
    dc.SetPen(penWithCap);
    dc.DrawText("Butt ", dc.FromDIP(x + 270), dc.FromDIP(y + 60));
    dc.DrawLine(dc.FromDIP(x + 200), dc.FromDIP(y + 70), dc.FromDIP(x + 250), dc.FromDIP(y + 70));

    penWithCap.SetCap(wxCAP_ROUND);
    dc.SetPen(penWithCap);
    dc.DrawText("Round cap", dc.FromDIP(x + 270), dc.FromDIP(y + 80));
    dc.DrawLine(dc.FromDIP(x + 200), dc.FromDIP(y + 90), dc.FromDIP(x + 250), dc.FromDIP(y + 90));

    penWithCap.SetCap(wxCAP_PROJECTING);
    dc.SetPen(penWithCap);
    dc.DrawText("Projecting cap", dc.FromDIP(x + 270), dc.FromDIP(y + 100));
    dc.DrawLine(dc.FromDIP(x + 200), dc.FromDIP(y + 110), dc.FromDIP(x + 250), dc.FromDIP(y + 110));
}

void MyCanvas::DrawCrossHair(int x, int y, int width, int heigth, wxDC &dc)
{
    dc.DrawText("Cross hair", dc.FromDIP(x + 10), dc.FromDIP(y + 10));
    dc.SetClippingRegion(dc.FromDIP(x), dc.FromDIP(y), dc.FromDIP(width), dc.FromDIP(heigth));
    dc.SetPen(wxPen(*wxBLUE, 2));
    dc.CrossHair(dc.FromDIP(x + width / 2), dc.FromDIP(y + heigth / 2));
    dc.DestroyClippingRegion();
}

void MyCanvas::DrawDefault(wxDC& dc)
{
    // Draw circle centered at the origin, then flood fill it with a different
    // color. Done with a wxMemoryDC because Blit (used by generic
    // wxDoFloodFill) from a window that is being painted gives unpredictable
    // results on wxGTK
    {
        wxImage img(dc.FromDIP(21), dc.FromDIP(21), false);
        img.Clear(1);
        wxBitmap bmp(img);
        {
            wxMemoryDC mdc(bmp);
            mdc.SetBrush(dc.GetBrush());
            mdc.SetPen(dc.GetPen());
            mdc.DrawCircle(dc.FromDIP(10), dc.FromDIP(10), dc.FromDIP(10));
            wxColour c;
            if (mdc.GetPixel(dc.FromDIP(11), dc.FromDIP(11), &c))
            {
                mdc.SetBrush(wxColour(128, 128, 0));
                mdc.FloodFill(dc.FromDIP(11), dc.FromDIP(11), c, wxFLOOD_SURFACE);
            }
        }
        bmp.SetMask(new wxMask(bmp, wxColour(1, 1, 1)));
        dc.DrawBitmap(bmp, dc.FromDIP(-10), dc.FromDIP(-10), true);
    }

    dc.DrawCheckMark(dc.FromDIP(5), dc.FromDIP(80), dc.FromDIP(15), dc.FromDIP(15));
    dc.DrawCheckMark(dc.FromDIP(25), dc.FromDIP(80), dc.FromDIP(30), dc.FromDIP(30));
    dc.DrawCheckMark(dc.FromDIP(60), dc.FromDIP(80), dc.FromDIP(60), dc.FromDIP(60));

    // this is the test for "blitting bitmap into DC damages selected brush" bug
    wxCoord rectSize = m_std_icon.GetWidth() + dc.FromDIP(10);
    wxCoord x = dc.FromDIP(100);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush( *wxGREEN_BRUSH );
    dc.DrawRectangle(x, dc.FromDIP(10), rectSize, rectSize);
    dc.DrawBitmap(m_std_icon, x + dc.FromDIP(5), dc.FromDIP(15), true);
    x += rectSize + dc.FromDIP(10);
    dc.DrawRectangle(x, dc.FromDIP(10), rectSize, rectSize);
    dc.DrawIcon(m_std_icon, x + dc.FromDIP(5), dc.FromDIP(15));
    x += rectSize + dc.FromDIP(10);
    dc.DrawRectangle(x, dc.FromDIP(10), rectSize, rectSize);

    // test for "transparent" bitmap drawing (it intersects with the last
    // rectangle above)
    //dc.SetBrush( *wxTRANSPARENT_BRUSH );

    if (m_smile_bmp.IsOk())
        dc.DrawBitmap(m_smile_bmp, x + rectSize - dc.FromDIP(20), rectSize - dc.FromDIP(10), true);

    dc.SetBrush( *wxBLACK_BRUSH );
    dc.DrawRectangle( 0, dc.FromDIP(160), dc.FromDIP(1000), dc.FromDIP(300) );

    // draw lines
    wxBitmap bitmap(dc.FromDIP(wxSize(20,70)));
    wxMemoryDC memdc;
    memdc.SelectObject( bitmap );
    memdc.SetBrush( *wxBLACK_BRUSH );
    memdc.SetPen( *wxWHITE_PEN );
    memdc.DrawRectangle(0, 0, dc.FromDIP(20), dc.FromDIP(70));
    memdc.DrawLine( dc.FromDIP(10), 0, dc.FromDIP(10), dc.FromDIP(70) );

    // to the right
    wxPen pen = *wxRED_PEN;
    memdc.SetPen(pen);
    memdc.DrawLine( dc.FromDIP(10), dc.FromDIP(5),  dc.FromDIP(10), dc.FromDIP(5)  );
    memdc.DrawLine( dc.FromDIP(10), dc.FromDIP(10), dc.FromDIP(11), dc.FromDIP(10) );
    memdc.DrawLine( dc.FromDIP(10), dc.FromDIP(15), dc.FromDIP(12), dc.FromDIP(15) );
    memdc.DrawLine( dc.FromDIP(10), dc.FromDIP(20), dc.FromDIP(13), dc.FromDIP(20) );

/*
    memdc.SetPen(*wxRED_PEN);
    memdc.DrawLine( dc.FromDIP(12),dc.FromDIP( 5),dc.FromDIP(12),dc.FromDIP( 5) );
    memdc.DrawLine( dc.FromDIP(12),dc.FromDIP(10),dc.FromDIP(13),dc.FromDIP(10) );
    memdc.DrawLine( dc.FromDIP(12),dc.FromDIP(15),dc.FromDIP(14),dc.FromDIP(15) );
    memdc.DrawLine( dc.FromDIP(12),dc.FromDIP(20),dc.FromDIP(15),dc.FromDIP(20) );
*/

    // same to the left
    memdc.DrawLine( dc.FromDIP(10), dc.FromDIP(25), dc.FromDIP(10), dc.FromDIP(25) );
    memdc.DrawLine( dc.FromDIP(10), dc.FromDIP(30), dc.FromDIP(9),  dc.FromDIP(30) );
    memdc.DrawLine( dc.FromDIP(10), dc.FromDIP(35), dc.FromDIP(8),  dc.FromDIP(35) );
    memdc.DrawLine( dc.FromDIP(10), dc.FromDIP(40), dc.FromDIP(7),  dc.FromDIP(40) );

    // XOR draw lines
    dc.SetPen(*wxWHITE_PEN);
    memdc.SetLogicalFunction( wxINVERT );
    memdc.SetPen( *wxWHITE_PEN );
    memdc.DrawLine( dc.FromDIP(10), dc.FromDIP(50), dc.FromDIP(10), dc.FromDIP(50) );
    memdc.DrawLine( dc.FromDIP(10), dc.FromDIP(55), dc.FromDIP(11), dc.FromDIP(55) );
    memdc.DrawLine( dc.FromDIP(10), dc.FromDIP(60), dc.FromDIP(12), dc.FromDIP(60) );
    memdc.DrawLine( dc.FromDIP(10), dc.FromDIP(65), dc.FromDIP(13), dc.FromDIP(65) );

    memdc.DrawLine( dc.FromDIP(12), dc.FromDIP(50), dc.FromDIP(12), dc.FromDIP(50) );
    memdc.DrawLine( dc.FromDIP(12), dc.FromDIP(55), dc.FromDIP(13), dc.FromDIP(55) );
    memdc.DrawLine( dc.FromDIP(12), dc.FromDIP(60), dc.FromDIP(14), dc.FromDIP(60) );
    memdc.DrawLine( dc.FromDIP(12), dc.FromDIP(65), dc.FromDIP(15), dc.FromDIP(65) );

    memdc.SelectObject( wxNullBitmap );
    dc.DrawBitmap( bitmap, dc.FromDIP(10), dc.FromDIP(170) );
    wxImage image = bitmap.ConvertToImage();
    image.Rescale( dc.FromDIP(60), dc.FromDIP(210) );
    bitmap = wxBitmap(image);
    dc.DrawBitmap( bitmap, dc.FromDIP(50), dc.FromDIP(170) );

    // test the rectangle outline drawing - there should be one pixel between
    // the rect and the lines
    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush( *wxTRANSPARENT_BRUSH );
    dc.DrawRectangle(dc.FromDIP(150), dc.FromDIP(170), dc.FromDIP(49), dc.FromDIP(29));
    dc.DrawRectangle(dc.FromDIP(200), dc.FromDIP(170), dc.FromDIP(49), dc.FromDIP(29));
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawLine(dc.FromDIP(250), dc.FromDIP(210), dc.FromDIP(250), dc.FromDIP(170));
    dc.DrawLine(dc.FromDIP(260), dc.FromDIP(200), dc.FromDIP(150), dc.FromDIP(200));

    // test the rectangle filled drawing - there should be one pixel between
    // the rect and the lines
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawRectangle(dc.FromDIP(300), dc.FromDIP(170), dc.FromDIP(49), dc.FromDIP(29));
    dc.DrawRectangle(dc.FromDIP(350), dc.FromDIP(170), dc.FromDIP(49), dc.FromDIP(29));
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawLine(dc.FromDIP(400), dc.FromDIP(170), dc.FromDIP(400), dc.FromDIP(210));
    dc.DrawLine(dc.FromDIP(300), dc.FromDIP(200), dc.FromDIP(410), dc.FromDIP(200));

    // a few more tests of this kind
    dc.SetPen(*wxRED_PEN);
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawRectangle(dc.FromDIP(300), dc.FromDIP(220), dc.FromDIP(1), dc.FromDIP(1));
    dc.DrawRectangle(dc.FromDIP(310), dc.FromDIP(220), dc.FromDIP(2), dc.FromDIP(2));
    dc.DrawRectangle(dc.FromDIP(320), dc.FromDIP(220), dc.FromDIP(3), dc.FromDIP(3));
    dc.DrawRectangle(dc.FromDIP(330), dc.FromDIP(220), dc.FromDIP(4), dc.FromDIP(4));

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawRectangle(dc.FromDIP(300), dc.FromDIP(230), dc.FromDIP(1), dc.FromDIP(1));
    dc.DrawRectangle(dc.FromDIP(310), dc.FromDIP(230), dc.FromDIP(2), dc.FromDIP(2));
    dc.DrawRectangle(dc.FromDIP(320), dc.FromDIP(230), dc.FromDIP(3), dc.FromDIP(3));
    dc.DrawRectangle(dc.FromDIP(330), dc.FromDIP(230), dc.FromDIP(4), dc.FromDIP(4));

    // and now for filled rect with outline
    dc.SetPen(*wxRED_PEN);
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawRectangle(dc.FromDIP(500), dc.FromDIP(170), dc.FromDIP(49), dc.FromDIP(29));
    dc.DrawRectangle(dc.FromDIP(550), dc.FromDIP(170), dc.FromDIP(49), dc.FromDIP(29));
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawLine(dc.FromDIP(600), dc.FromDIP(170), dc.FromDIP(600), dc.FromDIP(210));
    dc.DrawLine(dc.FromDIP(500), dc.FromDIP(200), dc.FromDIP(610), dc.FromDIP(200));

    // test the rectangle outline drawing - there should be one pixel between
    // the rect and the lines
    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush( *wxTRANSPARENT_BRUSH );
    dc.DrawRoundedRectangle(dc.FromDIP(150), dc.FromDIP(270), dc.FromDIP(49), dc.FromDIP(29), dc.FromDIP(6));
    dc.DrawRoundedRectangle(dc.FromDIP(200), dc.FromDIP(270), dc.FromDIP(49), dc.FromDIP(29), dc.FromDIP(6));
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawLine(dc.FromDIP(250), dc.FromDIP(270), dc.FromDIP(250), dc.FromDIP(310));
    dc.DrawLine(dc.FromDIP(150), dc.FromDIP(300), dc.FromDIP(260), dc.FromDIP(300));

    // test the rectangle filled drawing - there should be one pixel between
    // the rect and the lines
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawRoundedRectangle(dc.FromDIP(300), dc.FromDIP(270), dc.FromDIP(49), dc.FromDIP(29), dc.FromDIP(6));
    dc.DrawRoundedRectangle(dc.FromDIP(350), dc.FromDIP(270), dc.FromDIP(49), dc.FromDIP(29), dc.FromDIP(6));
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawLine(dc.FromDIP(400), dc.FromDIP(270), dc.FromDIP(400), dc.FromDIP(310));
    dc.DrawLine(dc.FromDIP(300), dc.FromDIP(300), dc.FromDIP(410), dc.FromDIP(300));

    // Added by JACS to demonstrate bizarre behaviour.
    // With a size of 70, we get a missing red RHS,
    // and the height is too small, so we get yellow
    // showing. With a size of 40, it draws as expected:
    // it just shows a white rectangle with red outline.
    int totalWidth = dc.FromDIP(70);
    int totalHeight = dc.FromDIP(70);
    wxBitmap bitmap2(totalWidth, totalHeight);

    wxMemoryDC memdc2;
    memdc2.SelectObject(bitmap2);

    memdc2.SetBackground(*wxYELLOW_BRUSH);
    memdc2.Clear();

    // Now draw a white rectangle with red outline. It should
    // entirely eclipse the yellow background.
    memdc2.SetPen(*wxRED_PEN);
    memdc2.SetBrush(*wxWHITE_BRUSH);

    memdc2.DrawRectangle(0, 0, totalWidth, totalHeight);

    memdc2.SetPen(wxNullPen);
    memdc2.SetBrush(wxNullBrush);
    memdc2.SelectObject(wxNullBitmap);

    dc.DrawBitmap(bitmap2, dc.FromDIP(500), dc.FromDIP(270));

    // Repeat, but draw directly on dc
    // Draw a yellow rectangle filling the bitmap

    x = dc.FromDIP(600); int y = dc.FromDIP(270);
    dc.SetPen(*wxYELLOW_PEN);
    dc.SetBrush(*wxYELLOW_BRUSH);
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
    dc.SetFont( wxFontInfo(12).Family(wxFONTFAMILY_MODERN).Underlined() );
    dc.DrawText( "This is text", dc.FromDIP(110), dc.FromDIP(10) );
    dc.DrawRotatedText( "That is text", dc.FromDIP(20), dc.FromDIP(10), -45 );

    // use wxSWISS_FONT and not wxNORMAL_FONT as the latter can't be rotated
    // under MSW (it is not TrueType)
    dc.SetFont( *wxSWISS_FONT );

    wxString text;
    dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

    for ( int n = -180; n < 180; n += 30 )
    {
        text.Printf("     %d rotated text", n);
        dc.DrawRotatedText(text , dc.FromDIP(400), dc.FromDIP(400), n);
    }

    dc.SetFont( wxFontInfo(18).Family(wxFONTFAMILY_SWISS) );

    dc.DrawText( "This is Swiss 18pt text.", dc.FromDIP(110), dc.FromDIP(40) );

    wxCoord length;
    wxCoord height;
    wxCoord descent;
    dc.GetTextExtent( "This is Swiss 18pt text.", &length, &height, &descent );
    text.Printf( "Dimensions are length %d, height %d, descent %d", length, height, descent );
    dc.DrawText( text, dc.FromDIP(110), dc.FromDIP(80) );

    text.Printf( "CharHeight() returns: %d", dc.GetCharHeight() );
    dc.DrawText( text, dc.FromDIP(110), dc.FromDIP(120) );

    dc.DrawRectangle( dc.FromDIP(100), dc.FromDIP(40), dc.FromDIP(4), dc.FromDIP(height) );

    // test the logical function effect
    wxCoord y = dc.FromDIP(150);
    dc.SetLogicalFunction(wxINVERT);
    // text drawing should ignore logical function
    dc.DrawText( "There should be a text below", dc.FromDIP(110), y );
    dc.DrawRectangle( dc.FromDIP(110), y, dc.FromDIP(100), height );

    y += height;
    dc.DrawText( "Visible text", dc.FromDIP(110), y );
    dc.DrawRectangle( dc.FromDIP(110), y, dc.FromDIP(100), height );
    dc.DrawText( "Visible text", dc.FromDIP(110), y );
    dc.DrawRectangle( dc.FromDIP(110), y, dc.FromDIP(100), height );
    dc.SetLogicalFunction(wxCOPY);

    y += height;
    dc.DrawRectangle( dc.FromDIP(110), y, dc.FromDIP(100), height );
    dc.DrawText( "Another visible text", dc.FromDIP(110), y );

    y += height;
    dc.DrawText("And\nmore\ntext on\nmultiple\nlines", dc.FromDIP(110), y);
    y += 5*height;

    dc.SetTextForeground(*wxBLUE);
    dc.DrawRotatedText("Rotated text\ncan have\nmultiple lines\nas well", dc.FromDIP(110), y, 15);

    y += 7*height;
    dc.SetFont(wxFontInfo(12).Family(wxFONTFAMILY_TELETYPE));
    dc.SetTextForeground(wxColour(150, 75, 0));
    dc.DrawText("And some text with tab characters:\n123456789012345678901234567890\n\taa\tbbb\tcccc", dc.FromDIP(10), y);
}

static const struct
{
    wxString name;
    wxRasterOperationMode rop;
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

void MyCanvas::DrawImages(wxDC& dc, DrawMode mode)
{
    dc.DrawText("original image", 0, 0);
    dc.DrawBitmap(*gs_bmpNoMask, 0, dc.FromDIP(20), false);
    dc.DrawText("with colour mask", 0, dc.FromDIP(100));
    dc.DrawBitmap(*gs_bmpWithColMask, 0, dc.FromDIP(120), true);
    dc.DrawText("the mask image", 0, dc.FromDIP(200));
    dc.DrawBitmap(*gs_bmpMask, 0, dc.FromDIP(220), false);
    dc.DrawText("masked image", 0, dc.FromDIP(300));
    dc.DrawBitmap(*gs_bmpWithMask, 0, dc.FromDIP(320), true);

    int cx = gs_bmpWithColMask->GetWidth(),
        cy = gs_bmpWithColMask->GetHeight();

    wxMemoryDC memDC;
    for ( size_t n = 0; n < WXSIZEOF(rasterOperations); n++ )
    {
        wxCoord x = dc.FromDIP(120) + dc.FromDIP(150)*(n%4),
                y = dc.FromDIP(20)  + dc.FromDIP(100)*(n/4);

        dc.DrawText(rasterOperations[n].name, x, y - dc.FromDIP(20));
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
    static const wxCoord w = dc.FromDIP(60);
    static const wxCoord h = dc.FromDIP(60);

    // reuse the text colour here
    dc.SetPen(wxPen(m_owner->m_colourForeground));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    size_t n;
    for ( n = 0; n < WXSIZEOF(rasterOperations); n++ )
    {
        wxCoord x = dc.FromDIP(20) + dc.FromDIP(150)*(n%4),
                y = dc.FromDIP(20) + dc.FromDIP(100)*(n/4);

        dc.DrawText(rasterOperations[n].name, x, y - dc.FromDIP(20));
        dc.SetLogicalFunction(rasterOperations[n].rop);
        dc.DrawRectangle(x, y, w, h);
        dc.DrawLine(x, y, x + w, y + h);
        dc.DrawLine(x + w, y, x, y + h);
    }

    // now some filled rectangles
    dc.SetBrush(wxBrush(m_owner->m_colourForeground));

    for ( n = 0; n < WXSIZEOF(rasterOperations); n++ )
    {
        wxCoord x = dc.FromDIP(20) + dc.FromDIP(150)*(n%4),
                y = dc.FromDIP(500) + dc.FromDIP(100)*(n/4);

        dc.DrawText(rasterOperations[n].name, x, y - dc.FromDIP(20));
        dc.SetLogicalFunction(rasterOperations[n].rop);
        dc.DrawRectangle(x, y, w, h);
    }
}

#if wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
void MyCanvas::DrawAlpha(wxDC& dc)
{
    const int margin = dc.FromDIP(20);
    const int width = dc.FromDIP(180);
    wxDouble radius = dc.FromDIP(30);

    dc.SetPen( wxPen( wxColour( 128, 0, 0 ), 12 ));
    dc.SetBrush(*wxRED_BRUSH);

    wxRect r(margin, margin + width * 2 / 3, width, width);

    dc.DrawRoundedRectangle( r.x, r.y, r.width, r.width, radius ) ;

    dc.SetPen( wxPen( wxColour( 0, 0, 128 ), 12));
    dc.SetBrush( wxColour(0, 0, 255, 192) );

    r.Offset(width * 4 / 5, -width * 2 / 3);

    dc.DrawRoundedRectangle( r.x, r.y, r.width, r.width, radius ) ;

    dc.SetPen( wxPen( wxColour( 128, 128, 0 ), 12));
    dc.SetBrush( wxBrush( wxColour( 192, 192, 0, 192)));

    r.Offset(width * 4 / 5, width / 2);

    dc.DrawRoundedRectangle( r.x, r.y, r.width, r.width, radius ) ;

    dc.SetPen( *wxTRANSPARENT_PEN ) ;
    dc.SetBrush( wxBrush( wxColour(255,255,128,128) ) );
    dc.DrawRoundedRectangle( 0 , margin + width / 2 , width * 3 , dc.FromDIP(100) , radius) ;

    dc.SetTextBackground( wxColour(160, 192, 160, 160) );
    dc.SetTextForeground( wxColour(255, 128, 128, 128) );
    dc.SetFont( wxFontInfo(40).Family(wxFONTFAMILY_SWISS).Italic() );
    dc.DrawText( "Hello!", dc.FromDIP(120), dc.FromDIP(80) );
}
#endif // wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT

#if wxUSE_GRAPHICS_CONTEXT

// modeled along Robin Dunn's GraphicsContext.py sample

void MyCanvas::DrawGraphics(wxGraphicsContext* gc)
{
    const int BASE = gc->FromDIP(80.0);
    const int BASE2 = BASE / 2;
    const int BASE4 = BASE / 4;

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

    // Now use that path to demonstrate various capabilities of the graphics context
    gc->PushState(); // save current translation/scale/other state
    gc->Translate(gc->FromDIP(60), gc->FromDIP(75)); // reposition the context origin

    gc->SetPen(wxPen("navy"));
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
        gc->GetTextExtent(label, &w, &h, nullptr, nullptr);
        gc->DrawText(label, -w/2, -BASE2 - h - gc->FromDIP(4));
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
    gc->Translate(gc->FromDIP(60), gc->FromDIP(200)); // offset to the lower part of the window

    gc->DrawText("Scale", 0, -BASE2);
    gc->Translate(0, gc->FromDIP(20));

    gc->SetBrush(wxBrush(wxColour(178,  34,  34, 128)));// 128 == half transparent
    for( int i = 0 ; i < 8 ; ++i )
    {
        gc->Scale(1.08, 1.08); // increase scale by 8%
        gc->Translate(gc->FromDIP(5), gc->FromDIP(5));
        gc->DrawPath(path);
    }

    gc->PopState(); // restore saved state
    gc->PushState(); // save it again
    gc->Translate(gc->FromDIP(400), gc->FromDIP(200));

    gc->DrawText("Rotate", 0, -BASE2);

    // Move the origin over to the next location
    gc->Translate(0, gc->FromDIP(75));

    // draw our path again, rotating it about the central point,
    // and changing colors as we go
    for ( int angle = 0 ; angle < 360 ; angle += 30 )
    {
        gc->PushState(); // save this new current state so we can
        //  pop back to it at the end of the loop
        wxImage::RGBValue val = wxImage::HSVtoRGB(wxImage::HSVValue(angle / 360.0, 1, 1));
        gc->SetBrush(wxBrush(wxColour(val.red, val.green, val.blue, 64)));
        gc->SetPen(wxPen(wxColour(val.red, val.green, val.blue, 128)));

        // use translate to artfully reposition each drawn path
        gc->Translate(1.5 * BASE2 * cos(wxDegToRad(angle)),
                     1.5 * BASE2 * sin(wxDegToRad(angle)));

        // use Rotate to rotate the path
        gc->Rotate(wxDegToRad(angle));

        // now draw it
        gc->DrawPath(path);
        gc->PopState();
    }
    gc->PopState();

    gc->PushState();
    gc->Translate(gc->FromDIP(60), gc->FromDIP(400));
    const wxString labelText("Scaled smiley inside a square");
    gc->DrawText(labelText, 0, 0);
    // Center a bitmap horizontally
    wxDouble textWidth;
    gc->GetTextExtent(labelText, &textWidth, nullptr);
    const wxDouble rectSize = gc->FromDIP(100);
    wxDouble x0 = (textWidth - rectSize) / 2;
    gc->DrawRectangle(x0, BASE2, rectSize, rectSize);
    gc->DrawBitmap(m_smile_bmp, x0, BASE2, rectSize, rectSize);
    gc->PopState();

    // Draw graphics bitmap and its subbitmap
    gc->PushState();
    gc->Translate(gc->FromDIP(300), gc->FromDIP(400));
    gc->DrawText("Smiley as a graphics bitmap", 0, 0);

    wxGraphicsBitmap gbmp1 = gc->CreateBitmap(m_smile_bmp);
    gc->DrawBitmap(gbmp1, 0, BASE2, gc->FromDIP(50), gc->FromDIP(50));
    int bmpw = m_smile_bmp.GetWidth();
    int bmph = m_smile_bmp.GetHeight();
    wxGraphicsBitmap gbmp2 = gc->CreateSubBitmap(gbmp1, 0, bmph/5, bmpw/2, bmph/2);
    gc->DrawBitmap(gbmp2, gc->FromDIP(80), BASE2, gc->FromDIP(50), gc->FromDIP(50)*(bmph/2)/(bmpw/2));
    gc->PopState();
}
#endif // wxUSE_GRAPHICS_CONTEXT

void MyCanvas::DrawCircles(wxDC& dc)
{
    int x = dc.FromDIP(100),
        y = dc.FromDIP(100),
        r = dc.FromDIP(20);

    dc.SetPen( *wxRED_PEN );
    dc.SetBrush( *wxGREEN_BRUSH );

    dc.DrawText("Some circles", 0, y);
    dc.DrawCircle(x, y, r);
    dc.DrawCircle(x + 2*r, y, r);
    dc.DrawCircle(x + 4*r, y, r);

    y += 2*r;
    dc.DrawText("And ellipses", 0, y);
    dc.DrawEllipse(x - r, y, 2*r, r);
    dc.DrawEllipse(x + r, y, 2*r, r);
    dc.DrawEllipse(x + 3*r, y, 2*r, r);

    y += 2*r;
    dc.DrawText("And arcs", 0, y);
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
    dc.DrawText("Some circles", 0, y);
    dc.DrawCircle(x, y, r);
    dc.DrawCircle(x + 2*r, y, r);
    dc.DrawCircle(x + 4*r, y, r);

    y += 2*r;
    dc.DrawText("And ellipses", 0, y);
    dc.DrawEllipse(x - r, y, 2*r, r);
    dc.DrawEllipse(x + r, y, 2*r, r);
    dc.DrawEllipse(x + 3*r, y, 2*r, r);

    y += 2*r;
    dc.DrawText("And arcs", 0, y);
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
    dc.DrawText("Some splines", dc.FromDIP(10), dc.FromDIP(5));

    // values are hardcoded rather than randomly generated
    // so the output can be compared between native
    // implementations on platforms with different random
    // generators

    const int R = dc.FromDIP(300);
    const wxPoint center( R + dc.FromDIP(20), R + dc.FromDIP(20) );
    const int angles[7] = { 0, 10, 33, 77, 13, 145, 90 };
    const int radii[5] = { 100 , 59, 85, 33, 90 };
    const int numPoints = 200;
    wxPoint pts[numPoints];

    // background spline calculation
    unsigned int radius_pos = 0;
    unsigned int angle_pos = 0;
    int angle = 0;
    for ( int i = 0; i < numPoints; i++ )
    {
        angle += angles[ angle_pos ];
        int r = R * radii[ radius_pos ] / 100;
        pts[ i ].x = center.x + (wxCoord)( r * cos(wxDegToRad(angle)) );
        pts[ i ].y = center.y + (wxCoord)( r * sin(wxDegToRad(angle)) );

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

        dc.SetPen( wxPen( *wxBLUE, 1, wxPENSTYLE_DOT) );
        dc.DrawLines(5, letters[m]);
        dc.SetPen( wxPen( *wxBLACK, 4) );
        dc.DrawSpline(5, letters[m]);
    }

#else
    dc.DrawText("Splines not supported.", 10, 5);
#endif
}

void MyCanvas::DrawGradients(wxDC& dc)
{
    static const int TEXT_HEIGHT = dc.GetCharHeight();

    // LHS: linear
    wxRect r(dc.FromDIP(10), dc.FromDIP(10), dc.FromDIP(50), dc.FromDIP(50));
    dc.DrawText("wxRIGHT", r.x, r.y);
    r.Offset(0, TEXT_HEIGHT);
    dc.GradientFillLinear(r, *wxWHITE, *wxBLUE, wxRIGHT);

    r.Offset(0, r.height + dc.FromDIP(10));
    dc.DrawText("wxLEFT", r.x, r.y);
    r.Offset(0, TEXT_HEIGHT);
    dc.GradientFillLinear(r, *wxWHITE, *wxBLUE, wxLEFT);

    r.Offset(0, r.height + dc.FromDIP(10));
    dc.DrawText("wxDOWN", r.x, r.y);
    r.Offset(0, TEXT_HEIGHT);
    dc.GradientFillLinear(r, *wxWHITE, *wxBLUE, wxDOWN);

    r.Offset(0, r.height + dc.FromDIP(10));
    dc.DrawText("wxUP", r.x, r.y);
    r.Offset(0, TEXT_HEIGHT);
    dc.GradientFillLinear(r, *wxWHITE, *wxBLUE, wxUP);

    wxRect  gfr = wxRect(r);

    // RHS: concentric
    r = wxRect(dc.FromDIP(200), dc.FromDIP(10), dc.FromDIP(50), dc.FromDIP(50));
    dc.DrawText("Blue inside", r.x, r.y);
    r.Offset(0, TEXT_HEIGHT);
    dc.GradientFillConcentric(r, *wxBLUE, *wxWHITE);

    r.Offset(0, r.height + dc.FromDIP(10));
    dc.DrawText("White inside", r.x, r.y);
    r.Offset(0, TEXT_HEIGHT);
    dc.GradientFillConcentric(r, *wxWHITE, *wxBLUE);

    r.Offset(0, r.height + dc.FromDIP(10));
    dc.DrawText("Blue in top left corner", r.x, r.y);
    r.Offset(0, TEXT_HEIGHT);
    dc.GradientFillConcentric(r, *wxBLUE, *wxWHITE, wxPoint(0, 0));

    r.Offset(0, r.height + dc.FromDIP(10));
    dc.DrawText("Blue in bottom right corner", r.x, r.y);
    r.Offset(0, TEXT_HEIGHT);
    dc.GradientFillConcentric(r, *wxBLUE, *wxWHITE, wxPoint(r.width, r.height));

    // check that the area filled by the gradient is exactly the interior of
    // the rectangle
    r.x = dc.FromDIP(350);
    r.y = dc.FromDIP(30);
    dc.DrawText("The interior should be filled but", r.x, r.y);
    r.y += TEXT_HEIGHT;
    dc.DrawText(" the red border should remain visible:", r.x, r.y);
    r.y += TEXT_HEIGHT;

    r.width =
    r.height = dc.FromDIP(50);
    wxRect r2 = r;
    r2.x += dc.FromDIP(60);
    wxRect r3 = r;
    r3.y += dc.FromDIP(60);
    wxRect r4 = r2;
    r4.y += dc.FromDIP(60);
    dc.SetPen(*wxRED_PEN);
    dc.DrawRectangle(r);
    r.Deflate(1);
    dc.GradientFillLinear(r, *wxGREEN, *wxBLACK, wxNORTH);
    dc.DrawRectangle(r2);
    r2.Deflate(1);
    dc.GradientFillLinear(r2, *wxBLACK, *wxGREEN, wxSOUTH);
    dc.DrawRectangle(r3);
    r3.Deflate(1);
    dc.GradientFillLinear(r3, *wxGREEN, *wxBLACK, wxEAST);
    dc.DrawRectangle(r4);
    r4.Deflate(1);
    dc.GradientFillLinear(r4, *wxBLACK, *wxGREEN, wxWEST);

#if wxUSE_GRAPHICS_CONTEXT
    if (m_renderer)
    {
        wxGraphicsContext           *gc = dc.GetGraphicsContext();
        wxGraphicsPath              pth;
        wxGraphicsGradientStops     stops;
        double boxX, boxY, boxWidth, boxHeight;

        gfr.Offset(0, gfr.height + gc->FromDIP(10));
        dc.DrawText("Linear Gradient with Stops", gfr.x, gfr.y);
        gfr.Offset(0, TEXT_HEIGHT);

        stops = wxGraphicsGradientStops(*wxRED, *wxBLUE);
        stops.Add(wxColour(255,255,0), 0.33f);
        stops.Add(*wxGREEN, 0.67f);

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
        pth.GetBox(&boxX, &boxY, &boxWidth, &boxHeight);
        dc.CalcBoundingBox(wxRound(boxX), wxRound(boxY));
        dc.CalcBoundingBox(wxRound(boxX+boxWidth), wxRound(boxY+boxHeight));

        wxGraphicsGradientStops simpleStops(*wxRED, *wxBLUE);

        gfr.Offset(0, gfr.height + gc->FromDIP(10));
        dc.DrawText("Radial Gradient from Red to Blue without intermediary Stops",
            gfr.x, gfr.y);
        gfr.Offset(0, TEXT_HEIGHT);

        gc->SetBrush(gc->CreateRadialGradientBrush(gfr.x + gfr.width / 2,
            gfr.y + gfr.height / 2,
            gfr.x + gfr.width / 2,
            gfr.y + gfr.height / 2,
            gfr.width / 2,
            simpleStops));

        pth = gc->CreatePath();
        pth.MoveToPoint(gfr.x,gfr.y);
        pth.AddLineToPoint(gfr.x + gfr.width,gfr.y);
        pth.AddLineToPoint(gfr.x + gfr.width,gfr.y+gfr.height);
        pth.AddLineToPoint(gfr.x,gfr.y+gfr.height);
        pth.CloseSubpath();
        gc->FillPath(pth);
        pth.GetBox(&boxX, &boxY, &boxWidth, &boxHeight);
        dc.CalcBoundingBox(wxRound(boxX), wxRound(boxY));
        dc.CalcBoundingBox(wxRound(boxX+boxWidth), wxRound(boxY+boxHeight));

        gfr.Offset(0, gfr.height + gc->FromDIP(10));
        dc.DrawText("Radial Gradient from Red to Blue with Yellow and Green Stops",
            gfr.x, gfr.y);
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
        pth.GetBox(&boxX, &boxY, &boxWidth, &boxHeight);
        dc.CalcBoundingBox(wxRound(boxX), wxRound(boxY));
        dc.CalcBoundingBox(wxRound(boxX+boxWidth), wxRound(boxY+boxHeight));

        gfr.Offset(0, gfr.height + gc->FromDIP(10));
        dc.DrawText("Linear Gradient with Stops and Gaps", gfr.x, gfr.y);
        gfr.Offset(0, TEXT_HEIGHT);

        stops = wxGraphicsGradientStops(*wxRED, *wxBLUE);
        stops.Add(wxColour(255,255,0), 0.33f);
        stops.Add(wxTransparentColour, 0.33f);
        stops.Add(wxTransparentColour, 0.67f);
        stops.Add(*wxGREEN, 0.67f);

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
        pth.GetBox(&boxX, &boxY, &boxWidth, &boxHeight);
        dc.CalcBoundingBox(wxRound(boxX), wxRound(boxY));
        dc.CalcBoundingBox(wxRound(boxX+boxWidth), wxRound(boxY+boxHeight));

        gfr.Offset(0, gfr.height + gc->FromDIP(10));
        dc.DrawText("Radial Gradient with Stops and Gaps", gfr.x, gfr.y);
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
        pth.GetBox(&boxX, &boxY, &boxWidth, &boxHeight);
        dc.CalcBoundingBox(wxRound(boxX), wxRound(boxY));
        dc.CalcBoundingBox(wxRound(boxX+boxWidth), wxRound(boxY+boxHeight));

        gfr.Offset(0, gfr.height + gc->FromDIP(10));
        dc.DrawText("Gradients with Stops and Transparency", gfr.x, gfr.y);
        gfr.Offset(0, TEXT_HEIGHT);

        stops = wxGraphicsGradientStops(*wxRED, wxTransparentColour);
        stops.Add(*wxRED, 0.33f);
        stops.Add(wxTransparentColour, 0.33f);
        stops.Add(wxTransparentColour, 0.67f);
        stops.Add(*wxBLUE, 0.67f);
        stops.Add(*wxBLUE, 1.0f);

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
        pth.GetBox(&boxX, &boxY, &boxWidth, &boxHeight);
        dc.CalcBoundingBox(wxRound(boxX), wxRound(boxY));
        dc.CalcBoundingBox(wxRound(boxX+boxWidth), wxRound(boxY+boxHeight));

        gfr.Offset(0, gfr.height + gc->FromDIP(10));
        dc.DrawText("Stroked path with a gradient pen", gfr.x, gfr.y);
        gfr.Offset(0, TEXT_HEIGHT);

        pth = gc->CreatePath();
        pth.MoveToPoint(gfr.x + gfr.width/2, gfr.y);
        pth.AddLineToPoint(gfr.x + gfr.width, gfr.y + gfr.height/2);
        pth.AddLineToPoint(gfr.x + gfr.width/2, gfr.y + gfr.height);
        pth.AddLineToPoint(gfr.x, gfr.y + gfr.height/2);
        pth.CloseSubpath();

        stops = wxGraphicsGradientStops(*wxRED, *wxBLUE);
        stops.Add(wxColour(255,255,0), 0.33f);
        stops.Add(*wxGREEN, 0.67f);

        wxGraphicsPen pen = gc->CreatePen(
            wxGraphicsPenInfo(wxColour(0,0,0)).Width(6).Join(wxJOIN_BEVEL).LinearGradient(
                gfr.x + gfr.width/2, gfr.y,
                gfr.x + gfr.width/2, gfr.y + gfr.height,
                stops));
        gc->SetPen(pen);
        gc->StrokePath(pth);
    }
#endif // wxUSE_GRAPHICS_CONTEXT
}

void MyCanvas::DrawSystemColours(wxDC& dc)
{
    wxFont mono(wxFontInfo().Family(wxFONTFAMILY_TELETYPE));
    wxSize textSize;
    {
        wxDCFontChanger setMono(dc, mono);
        textSize = dc.GetTextExtent("#01234567");
    }

    int lineHeight = textSize.GetHeight();
    wxCoord x(FromDIP(10));
    wxRect r(textSize.GetWidth() + x, x, dc.FromDIP(100), lineHeight);

    dc.DrawText("System colours", x, r.y);
    r.y += 2*lineHeight;

    const wxSystemAppearance appearance = wxSystemSettings::GetAppearance();
    const wxString appearanceName = appearance.GetName();
    if ( !appearanceName.empty() )
    {
        dc.DrawText(wxString::Format("System appearance: %s", appearanceName),
                    x, r.y);
        r.y += lineHeight;
    }

    auto const showDarkOrLight = [&](const char* what, bool dark)
    {
        dc.DrawText(wxString::Format("%s: %s", what, dark ? "dark" : "light"),
                    x, r.y);
        r.y += 1.5*lineHeight;
    };

    showDarkOrLight("System", appearance.IsSystemDark());
    showDarkOrLight("App default", appearance.AreAppsDark());
    showDarkOrLight("Current app", appearance.IsDark());
    showDarkOrLight("Background", appearance.IsUsingDarkBackground());

    r.y += lineHeight;

    dc.SetPen(*wxTRANSPARENT_PEN);

    static const struct {
        wxSystemColour index;
        const char* name;
    } sysColours[] =
    {
        { wxSYS_COLOUR_3DDKSHADOW, "wxSYS_COLOUR_3DDKSHADOW" },
        { wxSYS_COLOUR_3DLIGHT, "wxSYS_COLOUR_3DLIGHT" },
        { wxSYS_COLOUR_ACTIVEBORDER, "wxSYS_COLOUR_ACTIVEBORDER" },
        { wxSYS_COLOUR_ACTIVECAPTION, "wxSYS_COLOUR_ACTIVECAPTION" },
        { wxSYS_COLOUR_APPWORKSPACE, "wxSYS_COLOUR_APPWORKSPACE" },
        { wxSYS_COLOUR_BTNFACE, "wxSYS_COLOUR_BTNFACE" },
        { wxSYS_COLOUR_BTNHIGHLIGHT, "wxSYS_COLOUR_BTNHIGHLIGHT" },
        { wxSYS_COLOUR_BTNSHADOW, "wxSYS_COLOUR_BTNSHADOW" },
        { wxSYS_COLOUR_BTNTEXT, "wxSYS_COLOUR_BTNTEXT" },
        { wxSYS_COLOUR_CAPTIONTEXT, "wxSYS_COLOUR_CAPTIONTEXT" },
        { wxSYS_COLOUR_DESKTOP, "wxSYS_COLOUR_DESKTOP" },
        { wxSYS_COLOUR_GRADIENTACTIVECAPTION, "wxSYS_COLOUR_GRADIENTACTIVECAPTION" },
        { wxSYS_COLOUR_GRADIENTINACTIVECAPTION, "wxSYS_COLOUR_GRADIENTINACTIVECAPTION" },
        { wxSYS_COLOUR_GRAYTEXT, "wxSYS_COLOUR_GRAYTEXT" },
        { wxSYS_COLOUR_HIGHLIGHTTEXT, "wxSYS_COLOUR_HIGHLIGHTTEXT" },
        { wxSYS_COLOUR_HIGHLIGHT, "wxSYS_COLOUR_HIGHLIGHT" },
        { wxSYS_COLOUR_HOTLIGHT, "wxSYS_COLOUR_HOTLIGHT" },
        { wxSYS_COLOUR_INACTIVEBORDER, "wxSYS_COLOUR_INACTIVEBORDER" },
        { wxSYS_COLOUR_INACTIVECAPTIONTEXT, "wxSYS_COLOUR_INACTIVECAPTIONTEXT" },
        { wxSYS_COLOUR_INACTIVECAPTION, "wxSYS_COLOUR_INACTIVECAPTION" },
        { wxSYS_COLOUR_INFOBK, "wxSYS_COLOUR_INFOBK" },
        { wxSYS_COLOUR_INFOTEXT, "wxSYS_COLOUR_INFOTEXT" },
        { wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT, "wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT" },
        { wxSYS_COLOUR_LISTBOXTEXT, "wxSYS_COLOUR_LISTBOXTEXT" },
        { wxSYS_COLOUR_LISTBOX, "wxSYS_COLOUR_LISTBOX" },
        { wxSYS_COLOUR_MENUBAR, "wxSYS_COLOUR_MENUBAR" },
        { wxSYS_COLOUR_MENUHILIGHT, "wxSYS_COLOUR_MENUHILIGHT" },
        { wxSYS_COLOUR_MENUTEXT, "wxSYS_COLOUR_MENUTEXT" },
        { wxSYS_COLOUR_MENU, "wxSYS_COLOUR_MENU" },
        { wxSYS_COLOUR_SCROLLBAR, "wxSYS_COLOUR_SCROLLBAR" },
        { wxSYS_COLOUR_WINDOWFRAME, "wxSYS_COLOUR_WINDOWFRAME" },
        { wxSYS_COLOUR_WINDOWTEXT, "wxSYS_COLOUR_WINDOWTEXT" },
        { wxSYS_COLOUR_WINDOW, "wxSYS_COLOUR_WINDOW" }
    };

    for (int i = 0; i < wxSYS_COLOUR_MAX; i++)
    {
        DrawColour(dc, mono, x, r, sysColours[i].name, wxSystemSettings::GetColour(sysColours[i].index));
        r.y += lineHeight;
    }
}

void MyCanvas::DrawDatabaseColours(wxDC& dc)
{
    // initial setup to compute coordinates is same as DrawSystemColours
    wxFont mono(wxFontInfo().Family(wxFONTFAMILY_TELETYPE));
    wxSize textSize;
    {
        wxDCFontChanger setMono(dc, mono);
        textSize = dc.GetTextExtent("#01234567");
    }

    int lineHeight = textSize.GetHeight();
    wxCoord x(FromDIP(10));
    wxRect r(textSize.GetWidth() + x, x, dc.FromDIP(100), lineHeight);

    wxString title = "wxColourDatabase colours";
    dc.DrawText(title, x, r.y);
    r.y += 3*lineHeight;

    const wxVector<wxString> names(wxTheColourDatabase->GetAllNames());
    for (wxVector<wxString>::const_iterator p = names.begin(); p != names.end(); ++p)
    {
        DrawColour(dc, mono, x, r, *p, wxTheColourDatabase->Find(*p));
        r.y += lineHeight;
    }
}

void MyCanvas::DrawColour(wxDC& dc, const wxFont& mono, wxCoord x, const wxRect& r, const wxString& colourName, const wxColour& col)
{
    {
        wxDCFontChanger setMono(dc, mono);
        dc.DrawText(col.GetAsString(wxC2S_HTML_SYNTAX), x, r.y);
    }

    dc.SetBrush(wxBrush(col));
    dc.DrawRectangle(r);

    dc.DrawText(colourName, r.GetRight() + x, r.y);
}

void MyCanvas::DrawRegions(wxDC& dc)
{
    dc.DrawText("You should see a red rect partly covered by a cyan one "
                "on the left", dc.FromDIP(10), dc.FromDIP(5));
    dc.DrawText("and 5 smileys from which 4 are partially clipped on the right",
                dc.FromDIP(10), dc.FromDIP(5) + dc.GetCharHeight());
    dc.DrawText("The second copy should be identical but right part of it "
                "should be offset by 10 pixels.",
                dc.FromDIP(10), dc.FromDIP(5) + 2*dc.GetCharHeight());

    DrawRegionsHelper(dc, dc.FromDIP(10), true);
    DrawRegionsHelper(dc, dc.FromDIP(350), false);
}

void MyCanvas::DrawRegionsHelper(wxDC& dc, wxCoord x, bool firstTime)
{
    wxCoord y = dc.FromDIP(100);

    dc.DestroyClippingRegion();
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.SetPen( *wxTRANSPARENT_PEN );
    dc.DrawRectangle( x, y, dc.FromDIP(310), dc.FromDIP(310) );

    dc.SetClippingRegion( x + dc.FromDIP(10), y + dc.FromDIP(10), dc.FromDIP(100), dc.FromDIP(270) );

    dc.SetBrush( *wxRED_BRUSH );
    dc.DrawRectangle( x, y, dc.FromDIP(310), dc.FromDIP(310) );

    dc.SetClippingRegion( x + dc.FromDIP(10), y + dc.FromDIP(10), dc.FromDIP(100), dc.FromDIP(100) );

    dc.SetBrush( *wxCYAN_BRUSH );
    dc.DrawRectangle( x, y, dc.FromDIP(310), dc.FromDIP(310) );

    dc.DestroyClippingRegion();

    wxRegion region(x + dc.FromDIP(110), y + dc.FromDIP(20), dc.FromDIP(100), dc.FromDIP(270));
    if ( !firstTime )
        region.Offset(dc.FromDIP(10), dc.FromDIP(10));
    dc.SetDeviceClippingRegion(region);

    dc.SetBrush( *wxGREY_BRUSH );
    dc.DrawRectangle( x, y, dc.FromDIP(310), dc.FromDIP(310) );

    if (m_smile_bmp.IsOk())
    {
        dc.DrawBitmap( m_smile_bmp, x + dc.FromDIP(150), y + dc.FromDIP(150), true );
        dc.DrawBitmap( m_smile_bmp, x + dc.FromDIP(130), y + dc.FromDIP(10),  true );
        dc.DrawBitmap( m_smile_bmp, x + dc.FromDIP(130), y + dc.FromDIP(280), true );
        dc.DrawBitmap( m_smile_bmp, x + dc.FromDIP(100), y + dc.FromDIP(70),  true );
        dc.DrawBitmap( m_smile_bmp, x + dc.FromDIP(200), y + dc.FromDIP(70),  true );
    }
}

wxSize MyCanvas::GetDIPDrawingSize() const
{
    return m_sizeDIP;
}

void MyCanvas::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    if ( m_useBuffer )
    {
        wxBufferedPaintDC bpdc(this);
        PrepareDC(bpdc); // Adjust scrolled contents.
        Draw(bpdc);
    }
    else
    {
        wxPaintDC pdc(this);
        PrepareDC(pdc); // Adjust scrolled contents.
        Draw(pdc);
    }
}

void MyCanvas::Draw(wxDC& pdc)
{
#if wxUSE_GRAPHICS_CONTEXT
    wxGCDC gdc;

    if ( m_renderer )
    {
        wxGraphicsContext* context;
        if ( wxPaintDC *paintdc = wxDynamicCast(&pdc, wxPaintDC) )
        {
            context = m_renderer->CreateContext(*paintdc);
        }
        else if ( wxMemoryDC *memdc = wxDynamicCast(&pdc, wxMemoryDC) )
        {
            context = m_renderer->CreateContext(*memdc);
        }
#if wxUSE_METAFILE && defined(wxMETAFILE_IS_ENH)
        else if ( wxMetafileDC *metadc = wxDynamicCast(&pdc, wxMetafileDC) )
        {
            context = m_renderer->CreateContext(*metadc);
        }
#endif
        else
        {
            wxFAIL_MSG( "Unknown wxDC kind" );
            return;
        }

        context->SetAntialiasMode(m_useAntiAliasing ? wxANTIALIAS_DEFAULT : wxANTIALIAS_NONE);

        gdc.SetBackground(GetBackgroundColour());
        gdc.SetGraphicsContext(context);
    }

    wxDC &dc = m_renderer ? static_cast<wxDC&>(gdc) : pdc;
#else
    wxDC &dc = pdc ;
#endif

    m_owner->PrepareDC(dc);

    dc.SetBackgroundMode( m_owner->m_backgroundMode );
    if ( m_owner->m_backgroundBrush.IsOk() )
        dc.SetBackground( m_owner->m_backgroundBrush );
    if ( m_owner->m_colourForeground.IsOk() )
        dc.SetTextForeground( m_owner->m_colourForeground );
    if ( m_owner->m_colourBackground.IsOk() )
        dc.SetTextBackground( m_owner->m_colourBackground );

    if ( m_owner->m_textureBackground) {
        if ( ! m_owner->m_backgroundBrush.IsOk() ) {
            dc.SetBackground(wxBrush(wxColour(0, 128, 0)));
        }
    }

    if ( m_clip )
    {
        dc.SetClippingRegion(wxPoint(dc.FromDIP(100), dc.FromDIP(100)),
                             wxSize(dc.FromDIP(100), dc.FromDIP(100)));
    }

    dc.Clear();

    if ( m_owner->m_textureBackground )
    {
        dc.SetPen(*wxMEDIUM_GREY_PEN);
        for ( int i = 0; i < 200; i++ )
            dc.DrawLine(0, dc.FromDIP(i*10), dc.FromDIP(i*10), 0);
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
            DrawCrossHair( 0, 0, 400, 90, dc);
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

#if wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
        case File_ShowAlpha:
            DrawAlpha(dc);
            break;
#endif // wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
#if wxUSE_GRAPHICS_CONTEXT
        case File_ShowGraphics:
            DrawGraphics(gdc.GetGraphicsContext());
            break;
#endif

        case File_ShowGradients:
            DrawGradients(dc);
            break;

        case File_ShowSystemColours:
            DrawSystemColours(dc);
            break;

        case File_ShowDatabaseColours:
            DrawDatabaseColours(dc);
            break;

        default:
            break;
    }

    // For drawing with raw wxGraphicsContext
    // there is no bounding box to obtain.
    if ( m_showBBox
#if wxUSE_GRAPHICS_CONTEXT
         && m_show != File_ShowGraphics
#endif // wxUSE_GRAPHICS_CONTEXT
       )
    {
        dc.SetPen(wxPen(wxColor(0, 128, 0), 1, wxPENSTYLE_DOT));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(dc.MinX(), dc.MinY(), dc.MaxX()-dc.MinX()+1, dc.MaxY()-dc.MinY()+1);
    }

    // Adjust drawing area dimensions only if screen drawing is invoked.
    if ( wxDynamicCast(&pdc, wxBufferedPaintDC) ||
         wxDynamicCast(&pdc, wxPaintDC) )
    {
        wxCoord x0, y0;
        dc.GetDeviceOrigin(&x0, &y0);
        m_sizeDIP.x = dc.ToDIP(dc.LogicalToDeviceX(dc.MaxX()) - x0) + 1;
        m_sizeDIP.y = dc.ToDIP(dc.LogicalToDeviceY(dc.MaxY()) - y0) + 1;
    }
}

void MyCanvas::OnMouseMove(wxMouseEvent &event)
{
#if wxUSE_STATUSBAR
    {
        wxClientDC dc(this);
        PrepareDC(dc);
        m_owner->PrepareDC(dc);

        wxPoint pos = dc.DeviceToLogical(event.GetPosition());
        wxPoint dipPos = dc.ToDIP(pos);
        wxString str;
        str.Printf( "Mouse position: %d,%d", pos.x, pos.y );
        if ( pos != dipPos )
            str += wxString::Format("; DIP position: %d,%d", dipPos.x, dipPos.y);
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
        dc.SetPen( wxPen( *wxLIGHT_GREY, 2 ) );
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

#if wxUSE_GRAPHICS_CONTEXT
void MyCanvas::UseGraphicRenderer(wxGraphicsRenderer* renderer)
{
    m_renderer = renderer;
    if (renderer)
    {
        int major, minor, micro;
        renderer->GetVersion(&major, &minor, &micro);
        wxString str = wxString::Format("Graphics renderer: %s %i.%i.%i",
                         renderer->GetName(), major, minor, micro);
        m_owner->SetStatusText(str, 1);
    }
    else
    {
        m_owner->SetStatusText(wxEmptyString, 1);
    }

    Refresh();
}
#endif // wxUSE_GRAPHICS_CONTEXT


#if wxUSE_DC_TRANSFORM_MATRIX
#include "wx/valnum.h"

class TransformDataDialog : public wxDialog
{
public:
    TransformDataDialog(wxWindow* parent, wxDouble dx, wxDouble dy, wxDouble scx, wxDouble scy, wxDouble rotAngle)
        : wxDialog(parent, wxID_ANY, "Affine transformation parameters")
        , m_dx(dx)
        , m_dy(dy)
        , m_scx(scx)
        , m_scy(scy)
        , m_rotAngle(rotAngle)
    {
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        const int border = wxSizerFlags::GetDefaultBorder();
        wxFlexGridSizer* paramSizer = new wxFlexGridSizer(2, wxSize(border, border));
        paramSizer->Add(new wxStaticText(this, wxID_ANY, "Translation X:"), wxSizerFlags().CentreVertical());
        wxFloatingPointValidator<wxDouble> val_dx(1, &m_dx, wxNUM_VAL_NO_TRAILING_ZEROES);
        paramSizer->Add(new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, val_dx), wxSizerFlags().CentreVertical());
        paramSizer->Add(new wxStaticText(this, wxID_ANY, "Translation Y:"), wxSizerFlags().CentreVertical());
        wxFloatingPointValidator<wxDouble> val_dy(1, &m_dy, wxNUM_VAL_NO_TRAILING_ZEROES);
        paramSizer->Add(new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, val_dy), wxSizerFlags().CentreVertical());
        paramSizer->Add(new wxStaticText(this, wxID_ANY, "Scale X (0.2 - 5):"), wxSizerFlags().CentreVertical());
        wxFloatingPointValidator<wxDouble> val_scx(2, &m_scx, wxNUM_VAL_NO_TRAILING_ZEROES);
        paramSizer->Add(new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, val_scx), wxSizerFlags().CentreVertical());
        paramSizer->Add(new wxStaticText(this, wxID_ANY, "Scale Y (0.2 - 5):"), wxSizerFlags().CentreVertical());
        wxFloatingPointValidator<wxDouble> val_scy(2, &m_scy, wxNUM_VAL_NO_TRAILING_ZEROES);
        paramSizer->Add(new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, val_scy), wxSizerFlags().CentreVertical());
        paramSizer->Add(new wxStaticText(this, wxID_ANY, "Rotation angle (deg):"), wxSizerFlags().CentreVertical());
        wxFloatingPointValidator<wxDouble> val_rot(1, &m_rotAngle, wxNUM_VAL_NO_TRAILING_ZEROES);
        paramSizer->Add(new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, val_rot), wxSizerFlags().CentreVertical());
        sizer->Add(paramSizer, wxSizerFlags().DoubleBorder());

        wxSizer *btnSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
        sizer->Add(btnSizer, wxSizerFlags().Expand().Border());

        SetSizerAndFit(sizer);
    }

    virtual bool TransferDataFromWindow() override
    {
        if ( !wxDialog::TransferDataFromWindow() )
            return false;

        if ( m_scx < 0.2 || m_scx > 5.0 || m_scy < 0.2 || m_scy > 5.0 )
        {
            if ( !wxValidator::IsSilent() )
                wxBell();

            return false;
        }

        return true;
    }

    void GetTransformationData(wxDouble* dx, wxDouble* dy, wxDouble* scx, wxDouble* scy, wxDouble* rotAngle) const
    {
        if ( dx )
            *dx = m_dx;

        if ( dy )
            *dy = m_dy;

        if ( scx )
            *scx = m_scx;

        if ( scy )
            *scy = m_scy;

        if ( rotAngle )
            *rotAngle = m_rotAngle;
    }

private:
    wxDouble m_dx;
    wxDouble m_dy;
    wxDouble m_scx;
    wxDouble m_scy;
    wxDouble m_rotAngle;
};
#endif // wxUSE_DC_TRANSFORM_MATRIX

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU      (File_Quit,     MyFrame::OnQuit)
    EVT_MENU      (File_About,    MyFrame::OnAbout)
    EVT_MENU      (File_Clip,     MyFrame::OnClip)

#if wxUSE_GRAPHICS_CONTEXT
    EVT_MENU      (File_GC_Default, MyFrame::OnGraphicContextDefault)
    EVT_MENU      (File_DC,         MyFrame::OnGraphicContextNone)
#if wxUSE_CAIRO
    EVT_MENU      (File_GC_Cairo, MyFrame::OnGraphicContextCairo)
#endif // wxUSE_CAIRO
#ifdef __WXMSW__
#if wxUSE_GRAPHICS_GDIPLUS
    EVT_MENU      (File_GC_GDIPlus, MyFrame::OnGraphicContextGDIPlus)
#endif
#if wxUSE_GRAPHICS_DIRECT2D
    EVT_MENU      (File_GC_Direct2D, MyFrame::OnGraphicContextDirect2D)
#endif
#endif // __WXMSW__
    EVT_MENU      (File_AntiAliasing, MyFrame::OnAntiAliasing)
    EVT_UPDATE_UI (File_AntiAliasing, MyFrame::OnAntiAliasingUpdateUI)
#endif // wxUSE_GRAPHICS_CONTEXT

    EVT_MENU      (File_Buffer,   MyFrame::OnBuffer)
    EVT_MENU      (File_Copy,     MyFrame::OnCopy)
    EVT_MENU      (File_Save,     MyFrame::OnSave)
    EVT_MENU      (File_BBox,     MyFrame::OnBoundingBox)
    EVT_UPDATE_UI (File_BBox,     MyFrame::OnBoundingBoxUpdateUI)

    EVT_MENU_RANGE(MenuShow_First,   MenuShow_Last,   MyFrame::OnShow)

    EVT_MENU_RANGE(MenuOption_First, MenuOption_Last, MyFrame::OnOption)
wxEND_EVENT_TABLE()

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(nullptr, wxID_ANY, title)
{
    // set the frame icon
    SetIcon(wxICON(sample));

    wxMenu *menuScreen = new wxMenu;
    menuScreen->Append(File_ShowDefault, "&Default screen\tF1");
    menuScreen->Append(File_ShowText, "&Text screen\tF2");
    menuScreen->Append(File_ShowLines, "&Lines screen\tF3");
    menuScreen->Append(File_ShowBrushes, "&Brushes screen\tF4");
    menuScreen->Append(File_ShowPolygons, "&Polygons screen\tF5");
    menuScreen->Append(File_ShowMask, "&Mask screen\tF6");
    menuScreen->Append(File_ShowMaskStretch, "1/&2 scaled mask\tShift-F6");
    menuScreen->Append(File_ShowOps, "&Raster operations screen\tF7");
    menuScreen->Append(File_ShowRegions, "Re&gions screen\tF8");
    menuScreen->Append(File_ShowCircles, "&Circles screen\tF9");
#if wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
    menuScreen->Append(File_ShowAlpha, "&Alpha screen\tF10");
#endif // wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
    menuScreen->Append(File_ShowSplines, "Spl&ines screen\tF11");
    menuScreen->Append(File_ShowGradients, "&Gradients screen\tF12");
#if wxUSE_GRAPHICS_CONTEXT
    menuScreen->Append(File_ShowGraphics, "&Graphics screen");
#endif
    menuScreen->Append(File_ShowSystemColours, "System &colours");
    menuScreen->Append(File_ShowDatabaseColours, "Databa&se colours");

    wxMenu *menuFile = new wxMenu;
#if wxUSE_GRAPHICS_CONTEXT
    // Number the different renderer choices consecutively, starting from 0.
    int accel = 0;
    m_menuItemUseDC = menuFile->AppendRadioItem
                      (
                        File_DC,
                        wxString::Format("Use wx&DC\t%d", accel++)
                      );
    menuFile->AppendRadioItem
              (
                File_GC_Default,
                wxString::Format("Use default wx&GraphicContext\t%d", accel++)
              );
#if wxUSE_CAIRO
    menuFile->AppendRadioItem
              (
                File_GC_Cairo,
                wxString::Format("Use &Cairo\t%d", accel++)
              );
#endif // wxUSE_CAIRO
#ifdef __WXMSW__
#if wxUSE_GRAPHICS_GDIPLUS
    menuFile->AppendRadioItem
              (
                File_GC_GDIPlus,
                wxString::Format("Use &GDI+\t%d", accel++)
              );
#endif
#if wxUSE_GRAPHICS_DIRECT2D
    menuFile->AppendRadioItem
              (
                File_GC_Direct2D,
                wxString::Format("Use &Direct2D\t%d", accel++)
              );
#endif
#endif // __WXMSW__
#endif // wxUSE_GRAPHICS_CONTEXT
    menuFile->AppendSeparator();
    menuFile->AppendCheckItem(File_BBox, "Show bounding box\tCtrl-E",
                              "Show extents used in drawing operations");
    menuFile->AppendCheckItem(File_Clip, "&Clip\tCtrl-C", "Clip/unclip drawing");
    menuFile->AppendCheckItem(File_Buffer, "&Use wx&BufferedPaintDC\tCtrl-Z", "Buffer painting");
#if wxUSE_GRAPHICS_CONTEXT
    menuFile->AppendCheckItem(File_AntiAliasing,
                              "&Anti-Aliasing in wxGraphicContext\tCtrl-Shift-A",
                              "Enable Anti-Aliasing in wxGraphicContext")
            ->Check();
#endif
    menuFile->AppendSeparator();
#if wxUSE_METAFILE && defined(wxMETAFILE_IS_ENH)
    menuFile->Append(File_Copy, "Copy to clipboard");
#endif
    menuFile->Append(File_Save, "&Save...\tCtrl-S", "Save drawing to file");
    menuFile->AppendSeparator();
    menuFile->Append(File_About, "&About\tCtrl-A", "Show about dialog");
    menuFile->AppendSeparator();
    menuFile->Append(File_Quit, "E&xit\tAlt-X", "Quit this program");

    wxMenu *menuMapMode = new wxMenu;
    menuMapMode->Append( MapMode_Text, "&TEXT map mode" );
    menuMapMode->Append( MapMode_Lometric, "&LOMETRIC map mode" );
    menuMapMode->Append( MapMode_Twips, "T&WIPS map mode" );
    menuMapMode->Append( MapMode_Points, "&POINTS map mode" );
    menuMapMode->Append( MapMode_Metric, "&METRIC map mode" );

    wxMenu *menuUserScale = new wxMenu;
    menuUserScale->Append( UserScale_StretchHoriz, "Stretch &horizontally\tCtrl-H" );
    menuUserScale->Append( UserScale_ShrinkHoriz, "Shrin&k horizontally\tCtrl-G" );
    menuUserScale->Append( UserScale_StretchVertic, "Stretch &vertically\tCtrl-V" );
    menuUserScale->Append( UserScale_ShrinkVertic, "&Shrink vertically\tCtrl-W" );
    menuUserScale->AppendSeparator();
    menuUserScale->Append( UserScale_Restore, "&Restore to normal\tCtrl-0" );

    wxMenu *menuAxis = new wxMenu;
    menuAxis->AppendCheckItem( AxisMirror_Horiz, "Mirror horizontally\tCtrl-M" );
    menuAxis->AppendCheckItem( AxisMirror_Vertic, "Mirror vertically\tCtrl-N" );

    wxMenu *menuLogical = new wxMenu;
    menuLogical->Append( LogicalOrigin_MoveDown, "Move &down\tCtrl-D" );
    menuLogical->Append( LogicalOrigin_MoveUp, "Move &up\tCtrl-U" );
    menuLogical->Append( LogicalOrigin_MoveLeft, "Move &right\tCtrl-L" );
    menuLogical->Append( LogicalOrigin_MoveRight, "Move &left\tCtrl-R" );
    menuLogical->AppendSeparator();
    menuLogical->Append( LogicalOrigin_Set, "Set to (&100, 100)\tShift-Ctrl-1" );
    menuLogical->Append( LogicalOrigin_Restore, "&Restore to normal\tShift-Ctrl-0" );

#if wxUSE_DC_TRANSFORM_MATRIX
    wxMenu *menuTransformMatrix = new wxMenu;
    menuTransformMatrix->Append(TransformMatrix_Set, "Set &transformation matrix");
    menuTransformMatrix->AppendSeparator();
    menuTransformMatrix->Append(TransformMatrix_Reset, "Restore to &normal");
#endif // wxUSE_DC_TRANSFORM_MATRIX

    wxMenu *menuColour = new wxMenu;
#if wxUSE_COLOURDLG
    menuColour->Append( Colour_TextForeground, "Text &foreground..." );
    menuColour->Append( Colour_TextBackground, "Text &background..." );
    menuColour->Append( Colour_Background, "Background &colour..." );
#endif // wxUSE_COLOURDLG
    menuColour->AppendCheckItem( Colour_BackgroundMode, "&Opaque/transparent\tCtrl-B" );
    menuColour->AppendCheckItem( Colour_TextureBackgound, "Draw textured back&ground\tCtrl-T" );

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&Drawing");
    menuBar->Append(menuScreen, "Scree&n");
    menuBar->Append(menuMapMode, "&Mode");
    menuBar->Append(menuUserScale, "&Scale");
    menuBar->Append(menuAxis, "&Axis");
    menuBar->Append(menuLogical, "&Origin");
#if wxUSE_DC_TRANSFORM_MATRIX
    menuBar->Append(menuTransformMatrix, "&Transformation");
#endif // wxUSE_DC_TRANSFORM_MATRIX
    menuBar->Append(menuColour, "&Colours");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWidgets!");
#endif // wxUSE_STATUSBAR

    m_mapMode = wxMM_TEXT;
    m_xUserScale = 1.0;
    m_yUserScale = 1.0;
    m_xLogicalOrigin = 0;
    m_yLogicalOrigin = 0;
    m_xAxisReversed =
    m_yAxisReversed = false;
#if wxUSE_DC_TRANSFORM_MATRIX
    m_transform_dx = 0.0;
    m_transform_dy = 0.0;
    m_transform_scx = 1.0;
    m_transform_scy = 1.0;
    m_transform_rot = 0.0;
#endif // wxUSE_DC_TRANSFORM_MATRIX
    m_backgroundMode = wxBRUSHSTYLE_SOLID;
    m_colourForeground = *wxBLACK;
    m_colourBackground = *wxLIGHT_GREY;
    m_textureBackground = false;

    m_canvas = new MyCanvas( this );
    m_canvas->SetScrollbars( 10, 10, 100, 240 );

    SetSize(FromDIP(wxSize(800, 700)));
    Center(wxBOTH);
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
    msg.Printf( "This is the about dialog of the drawing sample.\n"
                "This sample tests various primitive drawing functions\n"
                "(without any attempts to prevent flicker).\n"
                "Copyright (c) Robert Roebling 1999"
              );

    wxMessageBox(msg, "About Drawing", wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnClip(wxCommandEvent& event)
{
    m_canvas->Clip(event.IsChecked());
}

void MyFrame::OnBuffer(wxCommandEvent& event)
{
    m_canvas->UseBuffer(event.IsChecked());
}

void MyFrame::OnCopy(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_METAFILE && defined(wxMETAFILE_IS_ENH)
    wxMetafileDC dc;
    if (!dc.IsOk())
        return;
    m_canvas->Draw(dc);
    wxMetafile *mf = dc.Close();
    if (!mf)
        return;
    mf->SetClipboard();
    delete mf;
#endif
}

void MyFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
    wxString wildCard = "Bitmap image (*.bmp)|*.bmp;*.BMP";
#if wxUSE_LIBPNG
    wildCard.Append("|PNG image (*.png)|*.png;*.PNG");
#endif
#if wxUSE_SVG
    wildCard.Append("|SVG image (*.svg)|*.svg;*.SVG");
#endif
#if wxUSE_POSTSCRIPT
    wildCard.Append("|PostScript file (*.ps)|*.ps;*.PS");
#endif

    wxFileDialog dlg(this, "Save as bitmap", wxEmptyString, wxEmptyString,
                     wildCard,
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxSize canvasSize = m_canvas->GetDIPDrawingSize();
        wxFileName fn(dlg.GetPath());
        wxString ext = fn.GetExt().Lower();
#if wxUSE_SVG
        if (ext == "svg")
        {
#if wxUSE_GRAPHICS_CONTEXT
            // Graphics screen can only be drawn using GraphicsContext
            if (m_canvas->GetPage() == File_ShowGraphics) {
                wxLogMessage("Graphics screen can not be saved as SVG.");
                return;
            }
            wxGraphicsRenderer* tempRenderer = m_canvas->GetRenderer();
            m_canvas->UseGraphicRenderer(nullptr);
#endif
            wxSVGFileDC svgdc(dlg.GetPath(),
                              canvasSize.GetWidth(),
                              canvasSize.GetHeight(),
                              72,
                              "Drawing sample");
            svgdc.SetBitmapHandler(new wxSVGBitmapEmbedHandler());
            m_canvas->Draw(svgdc);
#if wxUSE_GRAPHICS_CONTEXT
            m_canvas->UseGraphicRenderer(tempRenderer);
#endif
        }
        else
#endif
#if wxUSE_POSTSCRIPT
        if ( ext == "ps" )
        {
#if wxUSE_GRAPHICS_CONTEXT
            // Graphics screen can only be drawn using wxGraphicsContext
            if (m_canvas->GetPage() == File_ShowGraphics)
            {
                wxLogMessage("Graphics screen can not be saved as PostScript file.");
                return;
            }
            wxGraphicsRenderer* curRenderer = m_canvas->GetRenderer();
            m_canvas->UseGraphicRenderer(nullptr);
#endif // wxUSE_GRAPHICS_CONTEXT
            wxPrintData printData;
            printData.SetPrintMode(wxPRINT_MODE_FILE);
            printData.SetFilename(dlg.GetPath());
            printData.SetOrientation(wxPORTRAIT);
            printData.SetPaperId(wxPAPER_A4);
            wxPostScriptDC psdc(printData);
            // Save current scale factor
            const double curUserScaleX = m_xUserScale;
            const double curUserScaleY = m_yUserScale;
            // Change the scale temporarily to fit the drawing into the page.
            int w, h;
            psdc.GetSize(&w, &h);
            double sc = wxMin((double)w / canvasSize.GetWidth(), (double)h / canvasSize.GetHeight());
            m_xUserScale *= sc;
            m_yUserScale *= sc;
            psdc.StartDoc("Drawing sample");
            // Define default font.
            psdc.SetFont( wxFontInfo(10).Family(wxFONTFAMILY_MODERN) );
            psdc.StartPage();
            m_canvas->Draw(psdc);
            psdc.EndPage();
            psdc.EndDoc();
            // Restore original scale facor
            m_xUserScale = curUserScaleX;
            m_yUserScale = curUserScaleY;
#if wxUSE_GRAPHICS_CONTEXT
            m_canvas->UseGraphicRenderer(curRenderer);
#endif // wxUSE_GRAPHICS_CONTEXT
        }
        else
#endif // wxUSE_POSTSCRIPT
        {
            wxBitmap bmp;
            bmp.CreateWithDIPSize(canvasSize, GetDPIScaleFactor());
            wxMemoryDC mdc(bmp);
            mdc.SetBackground(*wxWHITE_BRUSH);
            mdc.Clear();
            m_canvas->Draw(mdc);
            bmp.ConvertToImage().SaveFile(dlg.GetPath());
        }
    }
}

void MyFrame::OnShow(wxCommandEvent& event)
{
    const int show = event.GetId();

#if wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
    // Make sure we do use a graphics context when selecting one of the screens
    // requiring it.
#if wxDRAWING_DC_SUPPORTS_ALPHA
    // If DC supports drawing with alpha
    // then GC is necessary only for graphics screen.
    if ( show == File_ShowGraphics )
#else // wxUSE_GRAPHICS_CONTEXT
    // DC doesn't support drawing with alpha
    // so GC is necessary both for alpha and graphics screen.
    if ( show == File_ShowAlpha || show == File_ShowGraphics )
#endif // wxDRAWING_DC_SUPPORTS_ALPHA, wxUSE_GRAPHICS_CONTEXT
    {
        if ( !m_canvas->HasRenderer() )
            m_canvas->UseGraphicRenderer(wxGraphicsRenderer::GetDefaultRenderer());
        // Disable selecting wxDC, if necessary.
        m_menuItemUseDC->Enable(!m_canvas->HasRenderer());
    }
    else
    {
        m_menuItemUseDC->Enable(true);
    }
#endif // wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
    m_canvas->ToShow(show);
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

#if wxUSE_DC_TRANSFORM_MATRIX
        case TransformMatrix_Set:
            {
                TransformDataDialog dlg(this, m_transform_dx, m_transform_dy,
                    m_transform_scx, m_transform_scy, m_transform_rot);
                if ( dlg.ShowModal() == wxID_OK )
                {
                    dlg.GetTransformationData(&m_transform_dx, &m_transform_dy,
                        &m_transform_scx, &m_transform_scy, &m_transform_rot);
                }
            }
            break;
        case TransformMatrix_Reset:
            m_transform_dx = 0.0;
            m_transform_dy = 0.0;
            m_transform_scx = 1.0;
            m_transform_scy = 1.0;
            m_transform_rot = 0.0;
            break;
#endif // wxUSE_DC_TRANSFORM_MATRIX

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
                if ( col.IsOk() )
                {
                    m_backgroundBrush.SetColour(col);
                }
            }
            break;
#endif // wxUSE_COLOURDLG

        case Colour_BackgroundMode:
            m_backgroundMode = m_backgroundMode == wxBRUSHSTYLE_SOLID
                                                 ? wxBRUSHSTYLE_TRANSPARENT
                                                 : wxBRUSHSTYLE_SOLID;
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

void MyFrame::OnBoundingBox(wxCommandEvent& evt)
{
    m_canvas->ShowBoundingBox(evt.IsChecked());
}

void MyFrame::OnBoundingBoxUpdateUI(wxUpdateUIEvent& evt)
{
#if wxUSE_GRAPHICS_CONTEXT
    evt.Enable(m_canvas->GetPage() != File_ShowGraphics);
#else
    wxUnusedVar(evt);
#endif // wxUSE_GRAPHICS_CONTEXT / !wxUSE_GRAPHICS_CONTEXT
}

void MyFrame::PrepareDC(wxDC& dc)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( dc.CanUseTransformMatrix() )
    {
        wxAffineMatrix2D mtx;
        mtx.Translate(m_transform_dx, m_transform_dy);
        mtx.Rotate(wxDegToRad(m_transform_rot));
        mtx.Scale(m_transform_scx, m_transform_scy);
        dc.SetTransformMatrix(mtx);
    }
#endif // wxUSE_DC_TRANSFORM_MATRIX
    dc.SetLogicalOrigin( dc.FromDIP(m_xLogicalOrigin), dc.FromDIP(m_yLogicalOrigin) );
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

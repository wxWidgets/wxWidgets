/////////////////////////////////////////////////////////////////////////////
// Name:        svgtest.cpp
// Purpose:     SVG sample
// Author:      Chris Elliott
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/dcsvg.h"
#include "wx/notebook.h"

#if wxUSE_GRAPHICS_CONTEXT
    #include "wx/graphics.h"
    #include "wx/dcgraph.h"
    #include "wx/svggc.h"
#endif

#include "SVGlogo24.xpm"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

#include <math.h>
#include <memory>

// ---------------------------------------------------------------------------
// classes
// ---------------------------------------------------------------------------

class MyApp : public wxApp
{
public:
    bool OnInit() override;
};

// Existing pages:
enum Page
{
    Page_Lines,
    Page_Polygons,
    Page_Text,
    Page_Arcs,
    Page_Checkmarks,
    Page_ScaledText,
    Page_Bitmaps,
    Page_Clipping,
    Page_TextPos,
#if wxUSE_GRAPHICS_CONTEXT
    Page_SVGGraphicsContext,
    Page_SVGGraphicsContextCreate,
    Page_SVGGraphicsContextDirect,
#endif
    Page_Max
};

static const char* pageNames[] =
{
    "Lines",
    "Polygons",
    "Text",
    "Arcs",
    "Checkmarks",
    "Scaled text",
    "Bitmaps",
    "Clipping",
    "Text position",
#if wxUSE_GRAPHICS_CONTEXT
    "SVG graphics context",
    "SVG graphics context (via Create)",
    "SVG graphics context (direct)",
#endif
};

wxCOMPILE_TIME_ASSERT( WXSIZEOF(pageNames) == Page_Max, PageNamesMismatch );

static const char* pageDescriptions[] =
{
     "Green Cross, Cyan Line and spline",
     "Blue rectangle, red edge, clear rounded rectangle, gold ellipse, gold and clear stars",
     "Swiss, Times text; red text, rotated and colored orange",
     "This is an arc test page",
     "Two check marks",
     "Scaling test page",
     "Icon and Bitmap ",
     "Clipping region",
     "Text position test page",
#if wxUSE_GRAPHICS_CONTEXT
     "Shapes drawn via wxGraphicsContext obtained from wxSVGFileDC",
     "Same as the previous page, but the context is obtained via wxSVGGraphicsContext::Create() when saving as SVG",
     "Same as the previous page, but the context is created directly, without creating wxSVGFileDC at all",
#endif
};

wxCOMPILE_TIME_ASSERT( WXSIZEOF(pageDescriptions) == Page_Max, PageDescriptionsMismatch );

class MyPage : public wxScrolledWindow
{
public:
    MyPage(wxNotebook *parent, int index);
    virtual void OnDraw(wxDC& dc) override;
    bool OnSave(const wxString& filename);

    void OnPaint(wxPaintEvent& event);

private:
    int m_index;
};

class MyFrame : public wxFrame
{
public:
    MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title);

    void FileSavePicture(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
private:
    wxNotebook *m_notebook;

    wxDECLARE_EVENT_TABLE();
};

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
    EVT_MENU(wxID_SAVE, MyFrame::FileSavePicture)
wxEND_EVENT_TABLE()

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// MyApp
// ---------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    // Create the main frame window

    MyFrame* frame = new MyFrame(nullptr, -1, "SVG Demo");
    frame->SetSize(frame->FromDIP(wxSize(600, 500)));
    frame->Show(true);

    return true;
}

// ---------------------------------------------------------------------------
// MyFrame
// ---------------------------------------------------------------------------

// Define my frame constructor

MyFrame::MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title)
        : wxFrame(parent, id, title)
{
    SetIcon(wxICON(sample));

#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif // wxUSE_STATUSBAR

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(wxID_SAVE);
    file_menu->Append(wxID_EXIT);

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT);

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, "&File");
    menu_bar->Append(help_menu, "&Help");

    // Associate the menu bar with the frame
    SetMenuBar(menu_bar);

    // Create a notebook
    m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBK_TOP);

    //Add SVG Windows to a notebook
    for (int i = 0; i < Page_Max; ++i)
    {
        m_notebook->AddPage(new MyPage(m_notebook, i), pageNames[i]);
    }
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    (void)wxMessageBox("wxWidgets SVG sample\n"
        "Authors:\n"
        "   Chris Elliott (c) 2002-2009\n"
        "   Prashant Kumar Nirmal (c) 2017\n"
        "Usage: click File|Save to Save the Selected SVG Test",
        "About SVG Test");
}

void MyFrame::FileSavePicture(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_FILEDLG
    MyPage * const page = (MyPage *) m_notebook->GetCurrentPage();

    wxFileDialog dialog(this, "Save Picture as", wxEmptyString,
        m_notebook->GetPageText(m_notebook->GetSelection()),
        "SVG vector picture files (*.svg)|*.svg",
        wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_OK)
    {
        if (!page->OnSave ( dialog.GetPath() ))
        {
            return;
        }
    }
    return;
#endif // wxUSE_FILEDLG
}

// ---------------------------------------------------------------------------
// MyPage
// ---------------------------------------------------------------------------

// Define a constructor for my page
MyPage::MyPage(wxNotebook *parent, int index)
    : wxScrolledWindow(parent)
{
    SetBackgroundColour(*wxWHITE);
    SetScrollbars(20, 20, 50, 50);
    m_index = index;

    Bind(wxEVT_PAINT, &MyPage::OnPaint, this);
}

void MyPage::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    PrepareDC(dc);

#if wxUSE_GRAPHICS_CONTEXT
    wxGCDC gcdc(dc);
    OnDraw(gcdc);
#else
    OnDraw(dc);
#endif
}

namespace
{

#if wxUSE_GRAPHICS_CONTEXT
// Shared content for the two SVGGraphicsContext demo pages: each page acquires
// the GC differently, but draws the same shapes.
//
// All coordinates and sizes here are in logical units (DIPs). The caller is
// responsible for scaling the context (e.g., via gc->Scale()) to match the
// target device's DPI.
void DrawSVGGraphicsContextDemo(wxGraphicsContext* gc)
{
    // Cyan rectangle with red border.
    gc->SetPen(*wxRED_PEN);
    gc->SetBrush(*wxCYAN_BRUSH);
    gc->DrawRectangle(10, 10, 100, 70);

    // Transparent rounded rectangle.
    gc->SetBrush(*wxTRANSPARENT_BRUSH);
    gc->DrawRoundedRectangle(50, 50, 100, 70, 20);

    // Goldenrod ellipse.
    gc->SetBrush(wxBrush("GOLDENROD"));
    gc->DrawEllipse(100, 100, 100, 50);

    // Star polygon, built as a wxGraphicsPath and filled.
    wxPoint2DDouble starPts[] = {
        { 100.0, 200.0 },
        { 70.0,  260.0 },
        { 160.0, 230.0 },
        { 40.0,  230.0 },
        { 130.0, 260.0 },
    };
    wxGraphicsPath path = gc->CreatePath();
    path.MoveToPoint(starPts[0]);
    for ( size_t i = 1; i < WXSIZEOF(starPts); ++i )
        path.AddLineToPoint(starPts[i]);
    path.CloseSubpath();
    gc->DrawPath(path);

    // Stroked-only path: a thick magenta zig-zag.
    {
        gc->SetPen(wxPenInfo("MAGENTA").Width(3));
        wxGraphicsPath zig = gc->CreatePath();
        zig.MoveToPoint(220, 20);
        zig.AddLineToPoint(260, 60);
        zig.AddLineToPoint(220, 100);
        zig.AddLineToPoint(260, 140);
        zig.AddLineToPoint(220, 180);
        gc->StrokePath(zig);
    }

    // Cubic Bezier curve.
    {
        gc->SetPen(wxPenInfo("FOREST GREEN").Width(2));
        wxGraphicsPath bez = gc->CreatePath();
        bez.MoveToPoint(220, 220);
        bez.AddCurveToPoint(260, 160, 320, 280, 360, 220);
        gc->StrokePath(bez);
    }

    // Quadratic Bezier curve.
    {
        gc->SetPen(wxPenInfo("ORANGE").Width(2));
        wxGraphicsPath bez = gc->CreatePath();
        bez.MoveToPoint(380, 220);
        bez.AddQuadCurveToPoint(420, 160, 460, 220);
        gc->StrokePath(bez);
    }

    // Arc segment built into a filled wedge.
    {
        gc->SetPen(*wxBLACK_PEN);
        gc->SetBrush(wxBrush("LIGHT BLUE"));
        wxGraphicsPath wedge = gc->CreatePath();
        const double cx = 310;
        const double cy = 110;
        const double r  = 40;
        wedge.MoveToPoint(cx, cy);
        wedge.AddArc(cx, cy, r, 0.0, wxDegToRad(135.0), true);
        wedge.CloseSubpath();
        gc->DrawPath(wedge);
    }

    // Push/Pop with rotation.
    // Draw a rotated rectangle without disturbing later drawing.
    gc->PushState();
    gc->Translate(420, 80);
    gc->Rotate(wxDegToRad(30.0));
    gc->SetPen(*wxBLACK_PEN);
    gc->SetBrush(wxBrush("GOLDENROD"));
    gc->DrawRectangle(-30, -15, 60, 30);
    gc->PopState();

    // Translate and scale, creating a smaller copy of the goldenrod ellipse.
    gc->PushState();
    gc->Translate(420, 160);
    gc->Scale(0.5, 0.5);
    gc->SetBrush(wxBrush("GOLDENROD"));
    gc->DrawEllipse(0, 0, 100, 50);
    gc->PopState();

    // Text drawn through the GC.
    gc->SetFont(wxFontInfo(12).FaceName("Arial"), *wxBLACK);
    gc->DrawText("Drawn via wxGraphicsContext", 10, 290);

    // Linear Gradient Brush
    {
        wxGraphicsGradientStops stops(wxColour(255, 0, 0), wxColour(0, 0, 255));
        stops.Add(wxColour(0, 255, 0), 0.5f);
        wxGraphicsBrush gradBrush = gc->CreateLinearGradientBrush(
            10, 320, 110, 320, stops);
        gc->SetBrush(gradBrush);
        gc->SetPen(*wxBLACK_PEN);
        gc->DrawRectangle(10, 320, 100, 40);
    }

    // Radial Gradient Brush
    {
        wxGraphicsGradientStops stops(*wxWHITE, *wxRED);
        wxGraphicsBrush gradBrush = gc->CreateRadialGradientBrush(
            180, 340, 180, 340, 40, stops);
        gc->SetBrush(gradBrush);
        gc->SetPen(*wxBLACK_PEN);
        gc->DrawEllipse(140, 300, 80, 80);
    }

    // Linear Gradient Pen
    {
        wxGraphicsGradientStops stops(*wxRED, *wxBLUE);
        wxGraphicsPen gradPen = gc->CreatePen(
            wxGraphicsPenInfo(*wxBLACK).Width(5).LinearGradient(
                250, 320, 350, 320, stops));
        gc->SetPen(gradPen);
        gc->StrokeLine(250, 320, 350, 360);
    }

    // Radial Gradient Pen
    {
        wxGraphicsGradientStops stops(*wxYELLOW, *wxGREEN);
        wxGraphicsPen gradPen = gc->CreatePen(
            wxGraphicsPenInfo(*wxBLACK).Width(5).RadialGradient(
                400, 340, 400, 340, 40, stops));
        gc->SetPen(gradPen);
        gc->StrokeLine(370, 310, 430, 370);
    }

    // Draw an arc, transform it (rotate and scale), and draw it again.
    {
        gc->SetPen(wxPen(*wxRED, 2));
        gc->SetBrush(wxBrush(*wxRED, wxBRUSHSTYLE_TRANSPARENT));
        wxGraphicsPath arcPath = gc->CreatePath();
        arcPath.AddArc(450, 250, 30, 0, M_PI, true);
        gc->StrokePath(arcPath);

        wxGraphicsMatrix m = gc->CreateMatrix();
        m.Translate(0, 40);
        m.Scale(1.5, 0.5);
        arcPath.Transform(m);
        gc->SetPen(wxPen(*wxBLUE, 2));
        gc->StrokePath(arcPath);
    }



    // Draw some bitmaps.
    {
        wxGraphicsBitmap bmp = gc->CreateBitmap(wxBitmap(svgbitmap_xpm));
        gc->DrawBitmap(bmp, 500, 60, 32, 32);
        // Rescaled bitmap
        gc->DrawBitmap(bmp, 500, 100, 64, 64);
    }

    // Clip a rectangular area and then draw a square inside of it.
    // The square will look like a small rectangle because of the clipping.
    {
        gc->PushState();
        gc->Clip(500, 200, 50, 30);
        gc->SetBrush(*wxRED_BRUSH);
        gc->DrawRectangle(480, 180, 300, 300);
        gc->PopState();
    }

    // Layer with 50% opacity.
    {
        gc->BeginLayer(0.5);
        gc->SetPen(*wxBLACK_PEN);
        gc->SetBrush(*wxGREEN_BRUSH);
        gc->DrawRectangle(10, 380, 100, 100);
        gc->DrawEllipse(60, 380, 100, 100);
        gc->EndLayer();
    }

    // Composition modes.
    {
        gc->PushState();
        gc->Translate(200, 400);

        gc->SetFont(wxFontInfo(10).FaceName("Arial"), *wxBLACK);
        gc->DrawText("Additive (on black)", 0, -15);

        // Backdrop for ADD: colors add up from black.
        gc->SetCompositionMode(wxCOMPOSITION_OVER);
        gc->SetBrush(*wxBLACK_BRUSH);
        gc->DrawRectangle(0, 0, 100, 70);

        // Additive blending (plus-lighter): Red + Green = Yellow.
        gc->SetCompositionMode(wxCOMPOSITION_ADD);
        gc->SetBrush(wxBrush(*wxRED));
        gc->SetPen(*wxTRANSPARENT_PEN);
        gc->DrawEllipse(10, 5, 50, 50);
        gc->SetBrush(wxBrush(*wxGREEN));
        gc->DrawEllipse(40, 5, 50, 50);

        // Difference blending: Inverts background colors.
        gc->Translate(150, 0);
        gc->SetCompositionMode(wxCOMPOSITION_OVER);
        gc->DrawText("Difference (on blue)", 0, -15);
        gc->SetBrush(*wxBLUE_BRUSH);
        gc->DrawRectangle(0, 0, 80, 70);

        gc->SetCompositionMode(wxCOMPOSITION_DIFF);
        gc->SetBrush(wxBrush(*wxWHITE));
        // White diff Blue = Yellow.
        gc->DrawRectangle(10, 10, 60, 50);

        gc->PopState();
    }
}
#endif // wxUSE_GRAPHICS_CONTEXT

// Define this function in an anonymous namespace, to prevent accidentally
// using (wxWindow::)FromDIP instead of dc.FromDIP().
void DrawOnDC(wxDC& dc, const int index)
{
    // vars to use ...
    wxPen wP;
    wxBrush wB;
    wxPoint points[6];
    wxColour wC;

    dc.SetFont(wxFontInfo(9).FaceName("Arial").Family(wxFONTFAMILY_SWISS));
    dc.SetPen(*wxGREEN_PEN);

    switch (index)
    {
        case Page_Lines:
            // draw lines to make a cross
            dc.DrawLine(dc.FromDIP(0), dc.FromDIP(0), dc.FromDIP(200), dc.FromDIP(200));
            dc.DrawLine(dc.FromDIP(200), dc.FromDIP(0), dc.FromDIP(0), dc.FromDIP(200));
            // draw point colored line and spline
            wP = *wxCYAN_PEN;
            wP.SetWidth(3);
            dc.SetPen(wP);

            dc.DrawPoint(dc.FromDIP(25), dc.FromDIP(15));
            dc.DrawLine(dc.FromDIP(50), dc.FromDIP(30), dc.FromDIP(200), dc.FromDIP(30));
            dc.DrawSpline(dc.FromDIP(50), dc.FromDIP(200), dc.FromDIP(50), dc.FromDIP(100), dc.FromDIP(200), dc.FromDIP(10));
            break;

        case Page_Polygons:
        {
            // When rendering to an SVG DC, label each shape individually
            // so assistive technology can announce them separately. These
            // per-shape groups nest inside the page-level accessible group
            // opened in MyPage::OnSave().
            wxSVGFileDC* const svgDC = wxDynamicCast(&dc, wxSVGFileDC);

            // draw standard shapes
            dc.SetBrush(*wxCYAN_BRUSH);
            dc.SetPen(*wxRED_PEN);
            if ( svgDC )
                svgDC->BeginAccessibleGroup(wxSVGAttributes().AriaLabel("Cyan rectangle with red border"));
            dc.DrawRectangle(dc.FromDIP(10), dc.FromDIP(10), dc.FromDIP(100), dc.FromDIP(70));
            if ( svgDC )
                svgDC->EndAccessibleGroup();

            wB = wxBrush ("DARK ORCHID", wxBRUSHSTYLE_TRANSPARENT);
            dc.SetBrush (wB);
            if ( svgDC )
                svgDC->BeginAccessibleGroup(wxSVGAttributes().AriaLabel("Transparent rounded rectangle"));
            dc.DrawRoundedRectangle(dc.FromDIP(50), dc.FromDIP(50), dc.FromDIP(100), dc.FromDIP(70), dc.FromDIP(20));
            if ( svgDC )
                svgDC->EndAccessibleGroup();

            dc.SetBrush (wxBrush("GOLDENROD") );
            if ( svgDC )
                svgDC->BeginAccessibleGroup(wxSVGAttributes().AriaLabel("Goldenrod ellipse"));
            dc.DrawEllipse(dc.FromDIP(100), dc.FromDIP(100), dc.FromDIP(100), dc.FromDIP(50));
            if ( svgDC )
                svgDC->EndAccessibleGroup();

            points[0].x = dc.FromDIP(100); points[0].y = dc.FromDIP(200);
            points[1].x = dc.FromDIP(70); points[1].y = dc.FromDIP(260);
            points[2].x = dc.FromDIP(160); points[2].y = dc.FromDIP(230);
            points[3].x = dc.FromDIP(40); points[3].y = dc.FromDIP(230);
            points[4].x = dc.FromDIP(130); points[4].y = dc.FromDIP(260);
            points[5].x = dc.FromDIP(100); points[5].y = dc.FromDIP(200);

            // Long-form example: role + label plus a <title> child.
            // (Uses the RAII wxSVGAccessibleGroup helper.)
            if ( svgDC )
            {
                wxSVGAccessibleGroup starGroup(*svgDC,
                    wxSVGAttributes().Role("graphics-symbol").AriaLabel("Five-pointed star"),
                    "Star polygon");
                dc.DrawPolygon(5, points);
                dc.DrawLines(6, points, dc.FromDIP(160));
            }
            else
            {
                dc.DrawPolygon(5, points);
                dc.DrawLines(6, points, dc.FromDIP(160));
            }
            break;
        }

        case Page_Text:
            // draw text in Arial or similar font
            dc.DrawLine(dc.FromDIP(50), dc.FromDIP(25), dc.FromDIP(50), dc.FromDIP(35));
            dc.DrawLine(dc.FromDIP(45), dc.FromDIP(30), dc.FromDIP(55), dc.FromDIP(30));
            dc.DrawText("This is a Swiss-style string", dc.FromDIP(50), dc.FromDIP(30));
            wC = dc.GetTextForeground();
            dc.SetTextForeground ("FIREBRICK");

            // no effect in msw ??
            dc.SetTextBackground ("WHEAT");
            dc.DrawText("This is a Red string", dc.FromDIP(50), dc.FromDIP(200));
            dc.DrawRotatedText("This is a 45 deg string", dc.FromDIP(50), dc.FromDIP(200), 45);
            dc.DrawRotatedText("This is a 90 deg string", dc.FromDIP(50), dc.FromDIP(200), 90);
            dc.SetFont(wxFontInfo(18)
                        .FaceName("Times New Roman")
                        .Family(wxFONTFAMILY_ROMAN)
                        .Italic().Bold());
            dc.SetTextForeground (wC);
            dc.DrawText("This is a Times-style string", dc.FromDIP(50), dc.FromDIP(60));
            break;

        case Page_Arcs:
            // four arcs start and end points, center
            dc.SetBrush(*wxGREEN_BRUSH);
            dc.DrawArc(dc.FromDIP(200), dc.FromDIP(300), dc.FromDIP(370), dc.FromDIP(230), dc.FromDIP(300), dc.FromDIP(300));
            dc.SetBrush(*wxBLUE_BRUSH);
            dc.DrawArc(dc.FromDIP(270 - 50), dc.FromDIP(270 - 86), dc.FromDIP(270 - 86), dc.FromDIP(270 - 50), dc.FromDIP(270), dc.FromDIP(270));
            dc.SetDeviceOrigin(-10,-10);
            dc.DrawArc(dc.FromDIP(270 - 50), dc.FromDIP(270 - 86), dc.FromDIP(270 - 86), dc.FromDIP(270 - 50), dc.FromDIP(270), dc.FromDIP(270));
            dc.SetDeviceOrigin(0,0);

            wP.SetColour ("CADET BLUE");
            dc.SetPen(wP);
            dc.DrawArc(dc.FromDIP(75), dc.FromDIP(125), dc.FromDIP(110), dc.FromDIP(40), dc.FromDIP(75), dc.FromDIP(75));

            wP.SetColour ("SALMON");
            dc.SetPen(wP);
            dc.SetBrush(*wxRED_BRUSH);
            //top left corner, width and height, start and end angle
                                 // 315 same center and x-radius as last pie-arc, half Y radius
            dc.DrawEllipticArc(dc.FromDIP(25), dc.FromDIP(50), dc.FromDIP(100), dc.FromDIP(50), 180.0, 45.0);

            wP = *wxCYAN_PEN;
            wP.SetWidth(3);
            dc.SetPen(wP);
                                 //wxBRUSHSTYLE_TRANSPARENT));
            dc.SetBrush (wxBrush ("SALMON"));
            dc.DrawEllipticArc(dc.FromDIP(300), dc.FromDIP(0), dc.FromDIP(200), dc.FromDIP(100), 0.0, 145.0);
                                 //same end point
            dc.DrawEllipticArc(dc.FromDIP(300), dc.FromDIP(50), dc.FromDIP(200), dc.FromDIP(100), 90.0, 145.0);
            dc.DrawEllipticArc(dc.FromDIP(300), dc.FromDIP(100), dc.FromDIP(200), dc.FromDIP(100), 90.0, 345.0);

            break;

        case Page_Checkmarks:
            dc.DrawCheckMark(dc.FromDIP(30), dc.FromDIP(30), dc.FromDIP(25), dc.FromDIP(25));
            dc.SetBrush (wxBrush ("SALMON",wxBRUSHSTYLE_TRANSPARENT));
            dc.DrawCheckMark(dc.FromDIP(80), dc.FromDIP(50), dc.FromDIP(75), dc.FromDIP(75));
            dc.DrawRectangle(dc.FromDIP(80), dc.FromDIP(50), dc.FromDIP(75), dc.FromDIP(75));
            break;

        case Page_ScaledText:
            dc.SetFont(wxFontInfo(18)
                        .FaceName("Times New Roman")
                        .Family(wxFONTFAMILY_ROMAN)
                        .Italic().Bold());
            dc.DrawLine(dc.FromDIP(0), dc.FromDIP(0), dc.FromDIP(200), dc.FromDIP(200));
            dc.DrawLine(dc.FromDIP(200), dc.FromDIP(0), dc.FromDIP(0), dc.FromDIP(200));
            dc.DrawText("This is an 18pt string", dc.FromDIP(50), dc.FromDIP(60));

            // rescale and draw in blue
            wP = *wxCYAN_PEN;
            dc.SetPen(wP);
            dc.SetUserScale (2.0,0.5);
            dc.SetDeviceOrigin(dc.FromDIP(200), dc.FromDIP(0));
            dc.DrawLine(dc.FromDIP(0), dc.FromDIP(0), dc.FromDIP(200), dc.FromDIP(200));
            dc.DrawLine(dc.FromDIP(200), dc.FromDIP(0), dc.FromDIP(0), dc.FromDIP(200));
            dc.DrawText("This is an 18pt string 2 x 0.5 UserScaled", dc.FromDIP(50), dc.FromDIP(60));
            dc.SetUserScale (2.0,2.0);
            dc.SetDeviceOrigin(dc.FromDIP(200), dc.FromDIP(200));
            dc.DrawText("This is an 18pt string 2 x 2 UserScaled", dc.FromDIP(50), dc.FromDIP(60));

            wP = *wxRED_PEN;
            dc.SetPen(wP);
            dc.SetUserScale (1.0,1.0);
            dc.SetDeviceOrigin(dc.FromDIP(0), dc.FromDIP(10));
            dc.SetMapMode (wxMM_METRIC); //svg ignores this
            dc.DrawLine(dc.FromDIP(0), dc.FromDIP(0), dc.FromDIP(200), dc.FromDIP(200));
            dc.DrawLine(dc.FromDIP(200), dc.FromDIP(0), dc.FromDIP(0), dc.FromDIP(200));
            dc.DrawText("This is an 18pt string in MapMode", dc.FromDIP(50), dc.FromDIP(60));
            break;

        case Page_Bitmaps:
            dc.DrawIcon(wxICON(sample), dc.FromDIP(10), dc.FromDIP(10));
            dc.DrawBitmap(wxBitmap(svgbitmap_xpm), dc.FromDIP(50), dc.FromDIP(15));
            break;

        case Page_Clipping:
            dc.SetTextForeground("RED");
            dc.DrawText("Red = Clipping Off", dc.FromDIP(30), dc.FromDIP(5));
            dc.SetTextForeground("GREEN");
            dc.DrawText("Green = Clipping On", dc.FromDIP(30), dc.FromDIP(25));

            dc.SetTextForeground("BLACK");

            dc.SetPen(*wxRED_PEN);
            dc.SetBrush (wxBrush ("SALMON",wxBRUSHSTYLE_TRANSPARENT));
            dc.DrawCheckMark(dc.FromDIP(80), dc.FromDIP(50), dc.FromDIP(75), dc.FromDIP(75));
            dc.DrawRectangle(dc.FromDIP(80), dc.FromDIP(50), dc.FromDIP(75), dc.FromDIP(75));

            dc.SetPen(*wxGREEN_PEN);

            // Clipped checkmarks
            dc.DrawRectangle(dc.FromDIP(180), dc.FromDIP(50), dc.FromDIP(75), dc.FromDIP(75));
            dc.SetClippingRegion(dc.FromDIP(180), dc.FromDIP(50), dc.FromDIP(75), dc.FromDIP(75)); // x,y,width,height version
            dc.DrawCheckMark(dc.FromDIP(180), dc.FromDIP(50), dc.FromDIP(75), dc.FromDIP(75));
            dc.DestroyClippingRegion();

            dc.DrawRectangle(wxRect(dc.FromDIP(wxPoint(80, 150)), dc.FromDIP(wxSize(75, 75))));
            dc.SetClippingRegion(dc.FromDIP(wxPoint(80, 150)), dc.FromDIP(wxSize(75, 75))); // pt,size version
            dc.DrawCheckMark(dc.FromDIP(80), dc.FromDIP(150), dc.FromDIP(75), dc.FromDIP(75));
            dc.DestroyClippingRegion();

            dc.DrawRectangle(wxRect(dc.FromDIP(wxPoint(180, 150)), dc.FromDIP(wxSize(75, 75))));
            dc.SetClippingRegion(wxRect(dc.FromDIP(wxPoint(180, 150)), dc.FromDIP(wxSize(75, 75)))); // rect version
            dc.DrawCheckMark(dc.FromDIP(180), dc.FromDIP(150), dc.FromDIP(75), dc.FromDIP(75));
            dc.DestroyClippingRegion();

            dc.DrawRectangle(wxRect(dc.FromDIP(wxPoint(80, 250)), dc.FromDIP(wxSize(50, 65))));
            dc.DrawRectangle(wxRect(dc.FromDIP(wxPoint(105, 260)), dc.FromDIP(wxSize(50, 65))));
            dc.SetClippingRegion(wxRect(dc.FromDIP(wxPoint(80, 250)), dc.FromDIP(wxSize(50, 65)))); // second call to SetClippingRegion
            dc.SetClippingRegion(wxRect(dc.FromDIP(wxPoint(105, 260)), dc.FromDIP(wxSize(50, 65)))); // forms intersection with previous
            dc.DrawCheckMark(dc.FromDIP(80), dc.FromDIP(250), dc.FromDIP(75), dc.FromDIP(75));
            dc.DestroyClippingRegion(); // only one call to destroy (there's no stack)

            /*
            ** Clipping by wxRegion not implemented for SVG.   Should be
            ** possible, but need to access points that define the wxRegion
            ** from inside DoSetDeviceClippingRegion() and wxRegion does not
            ** implement anything like getPoints().
            points[0].x = 180; points[0].y = 250;
            points[1].x = 255; points[1].y = 250;
            points[2].x = 180; points[2].y = 325;
            points[3].x = 255; points[3].y = 325;
            points[4].x = 180; points[4].y = 250;

            dc.DrawLines (5, points);
            wxRegion reg = wxRegion(5,points);

            dc.SetClippingRegion(reg);
            dc.DrawCheckMark ( 180,250,75,75);
            dc.DestroyClippingRegion();
            */

            break;

        case Page_TextPos:
        {
            wxString txtStr;
            wxCoord txtX, txtY, txtW, txtH, txtDescent, txtEL;
            wxCoord txtPad = 0;

            wP = *wxRED_PEN;
            dc.SetPen(wP);
            //dc.SetBackgroundMode(wxBRUSHSTYLE_SOLID);
            //dc.SetTextBackground(*wxBLUE);

            // Horizontal text
            txtStr = "Horizontal string";
            dc.GetTextExtent(txtStr, &txtW, &txtH, &txtDescent, &txtEL);
            txtX = dc.FromDIP(50);
            txtY = dc.FromDIP(200);
            dc.DrawRectangle(txtX, txtY, txtW + 2*txtPad, txtH + 2*txtPad);
            dc.DrawText(txtStr, txtX + txtPad, txtY + txtPad);

            // Vertical text
            txtStr = "Vertical string";
            dc.GetTextExtent(txtStr, &txtW, &txtH, &txtDescent, &txtEL);
            txtX = dc.FromDIP(50);
            txtY = dc.FromDIP(150);
            dc.DrawRectangle(txtX, txtY - (txtW + 2*txtPad), txtH + 2*txtPad, txtW + 2*txtPad);
            dc.DrawRotatedText(txtStr, txtX + txtPad, txtY - txtPad, 90);

            // 45 degree text
            txtStr = "45 deg string";
            dc.GetTextExtent(txtStr, &txtW, &txtH, &txtDescent, &txtEL);
            double lenW = (double)(txtW + 2*txtPad) / sqrt(2.0);
            double lenH = (double)(txtH + 2*txtPad) / sqrt(2.0);
            double padding = (double)txtPad / sqrt(2.0);
            txtX = dc.FromDIP(150);
            txtY = dc.FromDIP(100);
            dc.DrawLine(txtX - int(padding), txtY, txtX + int(lenW), txtY - int(lenW)); // top
            dc.DrawLine(txtX + int(lenW), txtY - int(lenW), txtX - int(padding + lenH + lenW), txtY + int(lenH - lenW));
            dc.DrawLine(txtX - int(padding), txtY, txtX - int(padding + lenH), txtY + int(lenH));
            dc.DrawLine(txtX - int(padding + lenH), txtY + int(lenH), txtX - int(padding + lenH + lenW), txtY + int(lenH - lenW)); // bottom
            dc.DrawRotatedText(txtStr, txtX, txtY, 45);
            break;
        }

#if wxUSE_GRAPHICS_CONTEXT
        case Page_SVGGraphicsContext:
        {
            // Acquire the GC from the DC itself.
            wxGraphicsContext* const gc = dc.GetGraphicsContext();
            if ( !gc )
                break;

            const double scale = dc.FromDIP(100) / 100.0;
            gc->PushState();
            gc->Scale(scale, scale);
            DrawSVGGraphicsContextDemo(gc);
            gc->PopState();
            break;
        }

        case Page_SVGGraphicsContextCreate:
        {
            // Acquire the GC via wxSVGGraphicsContext::Create(). This only
            // works when drawing into a wxSVGFileDC (i.e., when saving);
            // otherwise, will render using GetGraphicsContext() from the DC.
            wxSVGFileDC* const svgDC = wxDynamicCast(&dc, wxSVGFileDC);
            if ( !svgDC )
            {
                wxGraphicsContext* const gc = dc.GetGraphicsContext();
                if (!gc)
                    break;

                const double scale = dc.FromDIP(100) / 100.0;
                gc->PushState();
                gc->Scale(scale, scale);
                DrawSVGGraphicsContextDemo(gc);
                gc->PopState();
                break;
            }

            // We are responsible for deleting the wxGraphicsContext when
            // calling Create() (which std::unique_ptr will handle via RAII).
            std::unique_ptr<wxSVGGraphicsContext> gc(wxSVGGraphicsContext::Create(*svgDC));
            if ( !gc )
                break;

            const double scale = dc.FromDIP(100) / 100.0;
            gc->Scale(scale, scale);
            DrawSVGGraphicsContextDemo(gc.get());
            break;
        }

        case Page_SVGGraphicsContextDirect:
        {
            // This page is purely about the direct-to-file creation API,
            // so we don't render the demo shapes to the screen here.
            // Instead, we show a message explaining how to use it.
            dc.SetFont(wxFontInfo(12).Bold());
            dc.DrawText("This page demonstrates direct wxSVGGraphicsContext creation.",
                        dc.FromDIP(10), dc.FromDIP(20));
            dc.SetFont(wxFontInfo(10));
            dc.DrawText("Drawing commands are sent directly to the SVG context,",
                        dc.FromDIP(10), dc.FromDIP(50));
            dc.DrawText("bypassing the wxDC interface.",
                        dc.FromDIP(10), dc.FromDIP(70));
            dc.SetTextForeground(*wxBLUE);
            dc.DrawText("Use \"File -> Save\" to generate the SVG and see the result.",
                        dc.FromDIP(10), dc.FromDIP(100));
            break;
        }
#endif // wxUSE_GRAPHICS_CONTEXT

        case Page_Max:
            break;
    }
}

} // namespace

bool MyPage::OnSave(const wxString& filename)
{
#if wxUSE_GRAPHICS_CONTEXT
    if ( m_index == Page_SVGGraphicsContextDirect )
    {
        // Direct creation demo: we create the context directly with the
        // filename, bypassing the need to create a wxSVGFileDC ourselves.
        // We use a larger width (800) and height (700) to ensure all demo shapes fit.
        wxSVGGraphicsContext gc(filename, 800, 700, 72, pageNames[m_index]);

        gc.SetBitmapHandler(new wxSVGBitmapEmbedHandler());

        // Draw onto the context. (All drawing in DrawSVGGraphicsContextDemo
        // is done in logical units (DIPs).)
        DrawSVGGraphicsContextDemo(&gc);

        // Save the resulting SVG file.
        return gc.Save();
    }
#endif

    wxSize svgSize;
    wxSVGFileDC tempSvgDC(svgSize);
    OnDraw(tempSvgDC);

    svgSize = wxSize(tempSvgDC.MaxX(), tempSvgDC.MaxY());
    svgSize.IncBy(15); // account for wxPen width exceeding bounds

    wxSVGFileDC svgDC(svgSize, filename, pageNames[m_index]);
    svgDC.SetBitmapHandler(new wxSVGBitmapEmbedHandler());

    // Wrap the drawing in an accessible group so that assistive technology
    // can announce the page's title and description when the SVG is viewed.
    //
    // wxSVGAccessibleGroup is RAII: it opens the group in its constructor
    // and closes it automatically when the scope exits, so there's no need
    // to pair Begin/EndAccessibleGroup() calls by hand.
    {
        wxSVGAccessibleGroup group(svgDC,
            wxSVGAttributes().Role("img").AriaLabel(pageNames[m_index]),
            pageNames[m_index],
            pageDescriptions[m_index]);
        OnDraw(svgDC);
    }

    return svgDC.IsOk();
}

// Define the repainting behaviour
void MyPage::OnDraw(wxDC& dc)
{
    DrawOnDC(dc, m_index);

    wxLogStatus(pageDescriptions[m_index]);
}

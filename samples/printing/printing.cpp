/////////////////////////////////////////////////////////////////////////////
// Name:        samples/printing.cpp
// Purpose:     Printing demo for wxWidgets
// Author:      Julian Smart
// Modified by: Francesco Montorsi
// Created:     1995
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/log.h"
#endif

#if !wxUSE_PRINTING_ARCHITECTURE
    #error "You must set wxUSE_PRINTING_ARCHITECTURE to 1 in setup.h, and recompile the library."
#endif

#include <ctype.h>
#include "wx/metafile.h"
#include "wx/print.h"
#include "wx/printdlg.h"
#include "wx/image.h"
#include "wx/accel.h"

#if wxUSE_POSTSCRIPT
    #include "wx/generic/printps.h"
    #include "wx/generic/prntdlgg.h"
#endif

#if wxUSE_GRAPHICS_CONTEXT
    #include "wx/graphics.h"
    #include "wx/scopedptr.h"
#endif

#ifdef __WXMAC__
    #include "wx/osx/printdlg.h"
#endif

#include "printing.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// Global print data, to remember settings during the session
wxPrintData *g_printData = NULL;

// Global page setup data
wxPageSetupDialogData* g_pageSetupData = NULL;



// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit(void)
{
    if ( !wxApp::OnInit() )
        return false;

    wxInitAllImageHandlers();


    // init global objects
    // -------------------

    g_printData = new wxPrintData;

    // You could set an initial paper size here
#if 0
    g_printData->SetPaperId(wxPAPER_LETTER); // for Americans
    g_printData->SetPaperId(wxPAPER_A4);    // for everyone else
#endif

    g_pageSetupData = new wxPageSetupDialogData;

    // copy over initial paper size from print record
    (*g_pageSetupData) = *g_printData;

    // Set some initial page margins in mm.
    g_pageSetupData->SetMarginTopLeft(wxPoint(15, 15));
    g_pageSetupData->SetMarginBottomRight(wxPoint(15, 15));


    // init local GUI objects
    // ----------------------

#if 0
    wxImage image( "test.jpg" );
    image.SetAlpha();
    int i,j;
    for (i = 0; i < image.GetWidth(); i++)
       for (j = 0; j < image.GetHeight(); j++)
          image.SetAlpha( i, j, 50 );
    m_bitmap = image;
#endif
    m_angle = 30;
    m_testFont = wxFontInfo(10).Family(wxFONTFAMILY_SWISS);


    // Create the main frame window
    // ----------------------------

    MyFrame* frame = new MyFrame((wxFrame *) NULL, "wxWidgets Printing Demo",
                                 wxPoint(0, 0), wxSize(400, 400));

    frame->Centre(wxBOTH);
    frame->Show();

    return true;
}

int MyApp::OnExit()
{
    delete g_printData;
    delete g_pageSetupData;

    return wxApp::OnExit();
}

void MyApp::Draw(wxDC&dc)
{
    // This routine just draws a bunch of random stuff on the screen so that we
    // can check that different types of object are being drawn consistently
    // between the screen image, the print preview image (at various zoom
    // levels), and the printed page.
    dc.SetBackground(*wxWHITE_BRUSH);
    // dc.Clear();
    dc.SetFont(m_testFont);

    // dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxLIGHT_GREY_BRUSH);

    dc.DrawRectangle(0, 0, 230, 350);
    dc.DrawLine(0, 0, 229, 349);
    dc.DrawLine(229, 0, 0, 349);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    dc.SetBrush(*wxCYAN_BRUSH);
    dc.SetPen(*wxRED_PEN);

    dc.DrawRoundedRectangle(0, 20, 200, 80, 20);

    dc.DrawText( "Rectangle 200 by 80", 40, 40);

    dc.SetPen( wxPen(*wxBLACK, 0, wxPENSTYLE_DOT_DASH) );
    dc.DrawEllipse(50, 140, 100, 50);
    dc.SetPen(*wxRED_PEN);

    dc.DrawText( "Test message: this is in 10 point text", 10, 180);

    dc.DrawRotatedText( "This\nis\na multi-line\ntext", 170, 100, -m_angle/1.5);

#if wxUSE_UNICODE
    const char *test = "Hebrew    שלום -- Japanese (日本語)";
    wxString tmp = wxConvUTF8.cMB2WC( test );
    dc.DrawText( tmp, 10, 200 );
#endif

    wxPoint points[5];
    points[0].x = 0;
    points[0].y = 0;
    points[1].x = 20;
    points[1].y = 0;
    points[2].x = 20;
    points[2].y = 20;
    points[3].x = 10;
    points[3].y = 20;
    points[4].x = 10;
    points[4].y = -20;
    dc.DrawPolygon( 5, points, 20, 250, wxODDEVEN_RULE );
    dc.DrawPolygon( 5, points, 50, 250, wxWINDING_RULE );

    dc.DrawArc( 20, 330, 40, 300, 20, 300 );
    {
        wxDCBrushChanger changeBrush(dc, *wxTRANSPARENT_BRUSH);
        dc.DrawArc( 60, 330, 80, 300, 60, 300 );
    }

    dc.DrawEllipticArc( 80, 250, 60, 30, 0.0, 270.0 );

    points[0].x = 150;
    points[0].y = 250;
    points[1].x = 180;
    points[1].y = 250;
    points[2].x = 180;
    points[2].y = 220;
    points[3].x = 200;
    points[3].y = 220;
    dc.DrawSpline( 4, points );

    wxString str;
    int i = 0;
    str.Printf( "---- Text at angle %d ----", i );
    dc.DrawRotatedText( str, 100, 300, i );

    i = m_angle;
    str.Printf( "---- Text at angle %d ----", i );
    dc.DrawRotatedText( str, 100, 300, i );

    wxIcon my_icon = wxICON(sample);

    dc.DrawIcon( my_icon, 100, 100);

    if (m_bitmap.IsOk())
        dc.DrawBitmap( m_bitmap, 10, 10 );

#if wxUSE_GRAPHICS_CONTEXT
    wxScopedPtr<wxGraphicsContext> gc(wxGraphicsContext::CreateFromUnknownDC(dc));

    if (gc)
    {
        // make a path that contains a circle and some lines, centered at 100,100
        gc->SetPen( *wxRED_PEN );

        wxGraphicsPath path = gc->CreatePath();
        path.AddCircle( 50.0, 50.0, 50.0 );
        path.MoveToPoint(0.0, 50.0);
        path.AddLineToPoint(100.0, 50.0);
        path.MoveToPoint(50.0, 0.0);
        path.AddLineToPoint(50.0, 100.0 );
        path.CloseSubpath();
        path.AddRectangle(25.0, 25.0, 50.0, 50.0);

        gc->StrokePath(path);

        // draw some text
        wxString text("Text by wxGraphicsContext");
        gc->SetFont( m_testFont, *wxBLACK );
        gc->DrawText(text, 25.0, 60.0);

        // draw rectangle around the text
        double w, h;
        gc->GetTextExtent(text, &w, &h);
        gc->SetPen( *wxBLACK_PEN );
        gc->DrawRectangle(25.0, 60.0, w, h);
    }
#endif

}


// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
    EVT_MENU(wxID_PRINT, MyFrame::OnPrint)
    EVT_MENU(wxID_PREVIEW, MyFrame::OnPrintPreview)
    EVT_MENU(WXPRINT_PAGE_SETUP, MyFrame::OnPageSetup)
    EVT_MENU(wxID_ABOUT, MyFrame::OnPrintAbout)
#if wxUSE_POSTSCRIPT
    EVT_MENU(WXPRINT_PRINT_PS, MyFrame::OnPrintPS)
    EVT_MENU(WXPRINT_PREVIEW_PS, MyFrame::OnPrintPreviewPS)
    EVT_MENU(WXPRINT_PAGE_SETUP_PS, MyFrame::OnPageSetupPS)
#endif
#ifdef __WXMAC__
    EVT_MENU(WXPRINT_PAGE_MARGINS, MyFrame::OnPageMargins)
#endif
    EVT_MENU(WXPRINT_ANGLEUP, MyFrame::OnAngleUp)
    EVT_MENU(WXPRINT_ANGLEDOWN, MyFrame::OnAngleDown)

    EVT_MENU_RANGE(WXPRINT_FRAME_MODAL_APP,
                   WXPRINT_FRAME_MODAL_NON,
                   MyFrame::OnPreviewFrameModalityKind)
wxEND_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, const wxString&title, const wxPoint&pos, const wxSize&size)
        : wxFrame(frame, wxID_ANY, title, pos, size)
{
    m_canvas = NULL;
    m_previewModality = wxPreviewFrame_AppModal;

#if wxUSE_STATUSBAR
    // Give us a status line
    CreateStatusBar(2);
    SetStatusText("Printing demo");
#endif // wxUSE_STATUSBAR

    // Load icon and bitmap
    SetIcon( wxICON( sample) );

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(wxID_PRINT, "&Print...",                 "Print");
    file_menu->Append(WXPRINT_PAGE_SETUP, "Page Set&up...",    "Page setup");
#ifdef __WXMAC__
    file_menu->Append(WXPRINT_PAGE_MARGINS, "Page Margins...", "Page margins");
#endif
    file_menu->Append(wxID_PREVIEW, "Print Pre&view",          "Preview");

    wxMenu * const menuModalKind = new wxMenu;
    menuModalKind->AppendRadioItem(WXPRINT_FRAME_MODAL_APP, "&App modal");
    menuModalKind->AppendRadioItem(WXPRINT_FRAME_MODAL_WIN, "&Window modal");
    menuModalKind->AppendRadioItem(WXPRINT_FRAME_MODAL_NON, "&Not modal");
    file_menu->AppendSubMenu(menuModalKind, "Preview frame &modal kind");
#if wxUSE_ACCEL
    // Accelerators
    wxAcceleratorEntry entries[1];
    entries[0].Set(wxACCEL_CTRL, (int) 'V', wxID_PREVIEW);
    wxAcceleratorTable accel(1, entries);
    SetAcceleratorTable(accel);
#endif

#if wxUSE_POSTSCRIPT
    file_menu->AppendSeparator();
    file_menu->Append(WXPRINT_PRINT_PS, "Print PostScript...",           "Print (PostScript)");
    file_menu->Append(WXPRINT_PAGE_SETUP_PS, "Page Setup PostScript...", "Page setup (PostScript)");
    file_menu->Append(WXPRINT_PREVIEW_PS, "Print Preview PostScript",    "Preview (PostScript)");
#endif

    file_menu->AppendSeparator();
    file_menu->Append(WXPRINT_ANGLEUP, "Angle up\tAlt-U",                "Raise rotated text angle");
    file_menu->Append(WXPRINT_ANGLEDOWN, "Angle down\tAlt-D",            "Lower rotated text angle");
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, "E&xit",                                "Exit program");

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT, "&About",                              "About this demo");

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, "&File");
    menu_bar->Append(help_menu, "&Help");

    // Associate the menu bar with the frame
    SetMenuBar(menu_bar);


    // create the canvas
    // -----------------

    m_canvas = new MyCanvas(this, wxPoint(0, 0), wxSize(100, 100),
                            wxRETAINED|wxHSCROLL|wxVSCROLL);

    // Give it scrollbars: the virtual canvas is 20 * 50 = 1000 pixels in each direction
    m_canvas->SetScrollbars(20, 20, 50, 50);
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true /*force closing*/);
}

void MyFrame::OnPrint(wxCommandEvent& WXUNUSED(event))
{
    wxPrintDialogData printDialogData(* g_printData);

    wxPrinter printer(&printDialogData);
    MyPrintout printout(this, "My printout");
    if (!printer.Print(this, &printout, true /*prompt*/))
    {
        if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
        {
            wxLogError("There was a problem printing. Perhaps your current printer is not set correctly?");
        }
        else
        {
            wxLogMessage("You canceled printing");
        }
    }
    else
    {
        (*g_printData) = printer.GetPrintDialogData().GetPrintData();
    }
}

void MyFrame::OnPrintPreview(wxCommandEvent& WXUNUSED(event))
{
    // Pass two printout objects: for preview, and possible printing.
    wxPrintDialogData printDialogData(* g_printData);
    wxPrintPreview *preview =
        new wxPrintPreview(new MyPrintout(this), new MyPrintout(this), &printDialogData);
    if (!preview->IsOk())
    {
        delete preview;
        wxLogError("There was a problem previewing.\nPerhaps your current printer is not set correctly?");
        return;
    }

    wxPreviewFrame *frame =
        new wxPreviewFrame(preview, this, "Demo Print Preview", wxPoint(100, 100), wxSize(600, 650));
    frame->Centre(wxBOTH);
    frame->InitializeWithModality(m_previewModality);
    frame->Show();
}

void MyFrame::OnPageSetup(wxCommandEvent& WXUNUSED(event))
{
    (*g_pageSetupData) = *g_printData;

    wxPageSetupDialog pageSetupDialog(this, g_pageSetupData);
    pageSetupDialog.ShowModal();

    (*g_printData) = pageSetupDialog.GetPageSetupDialogData().GetPrintData();
    (*g_pageSetupData) = pageSetupDialog.GetPageSetupDialogData();
}

#if wxUSE_POSTSCRIPT
void MyFrame::OnPrintPS(wxCommandEvent& WXUNUSED(event))
{
    wxPrintDialogData printDialogData(* g_printData);

    wxPostScriptPrinter printer(&printDialogData);
    MyPrintout printout(this, "My printout");
    printer.Print(this, &printout, true/*prompt*/);

    (*g_printData) = printer.GetPrintDialogData().GetPrintData();
}

void MyFrame::OnPrintPreviewPS(wxCommandEvent& WXUNUSED(event))
{
    // Pass two printout objects: for preview, and possible printing.
    wxPrintDialogData printDialogData(* g_printData);
    wxPrintPreview *preview = new wxPrintPreview(new MyPrintout(this), new MyPrintout(this), &printDialogData);
    wxPreviewFrame *frame =
        new wxPreviewFrame(preview, this, "Demo Print Preview", wxPoint(100, 100), wxSize(600, 650));
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show();
}

void MyFrame::OnPageSetupPS(wxCommandEvent& WXUNUSED(event))
{
    (*g_pageSetupData) = * g_printData;

    wxGenericPageSetupDialog pageSetupDialog(this, g_pageSetupData);
    pageSetupDialog.ShowModal();

    (*g_printData) = pageSetupDialog.GetPageSetupDialogData().GetPrintData();
    (*g_pageSetupData) = pageSetupDialog.GetPageSetupDialogData();
}
#endif

#ifdef __WXMAC__
void MyFrame::OnPageMargins(wxCommandEvent& WXUNUSED(event))
{
    (*g_pageSetupData) = *g_printData;

    wxMacPageMarginsDialog pageMarginsDialog(this, g_pageSetupData);
    pageMarginsDialog.ShowModal();

    (*g_printData) = pageMarginsDialog.GetPageSetupDialogData().GetPrintData();
    (*g_pageSetupData) = pageMarginsDialog.GetPageSetupDialogData();
}
#endif

void MyFrame::OnPrintAbout(wxCommandEvent& WXUNUSED(event))
{
    (void)wxMessageBox("wxWidgets printing demo\nAuthor: Julian Smart",
                       "About wxWidgets printing demo", wxOK|wxCENTRE);
}

void MyFrame::OnAngleUp(wxCommandEvent& WXUNUSED(event))
{
    wxGetApp().IncrementAngle();
    m_canvas->Refresh();
}

void MyFrame::OnAngleDown(wxCommandEvent& WXUNUSED(event))
{
    wxGetApp().DecrementAngle();
    m_canvas->Refresh();
}

void MyFrame::OnPreviewFrameModalityKind(wxCommandEvent& event)
{
    m_previewModality = static_cast<wxPreviewFrameModalityKind>(
                            wxPreviewFrame_AppModal +
                                (event.GetId() - WXPRINT_FRAME_MODAL_APP));
}

// ----------------------------------------------------------------------------
// MyCanvas
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
  //  EVT_PAINT(MyCanvas::OnPaint)
wxEND_EVENT_TABLE()

MyCanvas::MyCanvas(wxFrame *frame, const wxPoint&pos, const wxSize&size, long style)
    : wxScrolledWindow(frame, wxID_ANY, pos, size, style)
{
    SetBackgroundColour(*wxWHITE);
}

//void MyCanvas::OnPaint(wxPaintEvent& WXUNUSED(evt))
void MyCanvas::OnDraw(wxDC& dc)
{
    //wxPaintDC dc(this);
    wxGetApp().Draw(dc);
}


// ----------------------------------------------------------------------------
// MyPrintout
// ----------------------------------------------------------------------------

bool MyPrintout::OnPrintPage(int page)
{
    wxDC *dc = GetDC();
    if (dc)
    {
        if (page == 1)
            DrawPageOne();
        else if (page == 2)
            DrawPageTwo();

        // Draw page numbers at top left corner of printable area, sized so that
        // screen size of text matches paper size.
        MapScreenSizeToPage();

        dc->DrawText(wxString::Format("PAGE %d", page), 0, 0);

        return true;
    }
    else
        return false;
}

bool MyPrintout::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return false;

    return true;
}

void MyPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
    *minPage = 1;
    *maxPage = 2;
    *selPageFrom = 1;
    *selPageTo = 2;
}

bool MyPrintout::HasPage(int pageNum)
{
    return (pageNum == 1 || pageNum == 2);
}

void MyPrintout::DrawPageOne()
{
    // You might use THIS code if you were scaling graphics of known size to fit
    // on the page. The commented-out code illustrates different ways of scaling
    // the graphics.

    // We know the graphic is 230x350. If we didn't know this, we'd need to
    // calculate it.
    wxCoord maxX = 230;
    wxCoord maxY = 350;

    // This sets the user scale and origin of the DC so that the image fits
    // within the paper rectangle (but the edges could be cut off by printers
    // that can't print to the edges of the paper -- which is most of them. Use
    // this if your image already has its own margins.
//    FitThisSizeToPaper(wxSize(maxX, maxY));
//    wxRect fitRect = GetLogicalPaperRect();

    // This sets the user scale and origin of the DC so that the image fits
    // within the page rectangle, which is the printable area on Mac and MSW
    // and is the entire page on other platforms.
//    FitThisSizeToPage(wxSize(maxX, maxY));
//    wxRect fitRect = GetLogicalPageRect();

    // This sets the user scale and origin of the DC so that the image fits
    // within the page margins as specified by g_PageSetupData, which you can
    // change (on some platforms, at least) in the Page Setup dialog. Note that
    // on Mac, the native Page Setup dialog doesn't let you change the margins
    // of a wxPageSetupDialogData object, so you'll have to write your own dialog or
    // use the Mac-only wxMacPageMarginsDialog, as we do in this program.
    FitThisSizeToPageMargins(wxSize(maxX, maxY), *g_pageSetupData);
    wxRect fitRect = GetLogicalPageMarginsRect(*g_pageSetupData);

    // This sets the user scale and origin of the DC so that the image appears
    // on the paper at the same size that it appears on screen (i.e., 10-point
    // type on screen is 10-point on the printed page) and is positioned in the
    // top left corner of the page rectangle (just as the screen image appears
    // in the top left corner of the window).
//    MapScreenSizeToPage();
//    wxRect fitRect = GetLogicalPageRect();

    // You could also map the screen image to the entire paper at the same size
    // as it appears on screen.
//    MapScreenSizeToPaper();
//    wxRect fitRect = GetLogicalPaperRect();

    // You might also wish to do you own scaling in order to draw objects at
    // full native device resolution. In this case, you should do the following.
    // Note that you can use the GetLogicalXXXRect() commands to obtain the
    // appropriate rect to scale to.
//    MapScreenSizeToDevice();
//    wxRect fitRect = GetLogicalPageRect();

    // Each of the preceding Fit or Map routines positions the origin so that
    // the drawn image is positioned at the top left corner of the reference
    // rectangle. You can easily center or right- or bottom-justify the image as
    // follows.

    // This offsets the image so that it is centered within the reference
    // rectangle defined above.
    wxCoord xoff = (fitRect.width - maxX) / 2;
    wxCoord yoff = (fitRect.height - maxY) / 2;
    OffsetLogicalOrigin(xoff, yoff);

    // This offsets the image so that it is positioned at the bottom right of
    // the reference rectangle defined above.
//    wxCoord xoff = (fitRect.width - maxX);
//    wxCoord yoff = (fitRect.height - maxY);
//    OffsetLogicalOrigin(xoff, yoff);

    wxGetApp().Draw(*GetDC());
}

void MyPrintout::DrawPageTwo()
{
    // You might use THIS code to set the printer DC to ROUGHLY reflect
    // the screen text size. This page also draws lines of actual length
    // 5cm on the page.

    // Compare this to DrawPageOne(), which uses the really convenient routines
    // from wxPrintout to fit the screen image onto the printed page. This page
    // illustrates how to do all the scaling calculations yourself, if you're so
    // inclined.

    wxDC *dc = GetDC();

    // Get the logical pixels per inch of screen and printer
    int ppiScreenX, ppiScreenY;
    GetPPIScreen(&ppiScreenX, &ppiScreenY);
    int ppiPrinterX, ppiPrinterY;
    GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);

    // This scales the DC so that the printout roughly represents the screen
    // scaling. The text point size _should_ be the right size but in fact is
    // too small for some reason. This is a detail that will need to be
    // addressed at some point but can be fudged for the moment.
    float scale = (float)((float)ppiPrinterX/(float)ppiScreenX);

    // Now we have to check in case our real page size is reduced (e.g. because
    // we're drawing to a print preview memory DC)
    int pageWidth, pageHeight;
    int w, h;
    dc->GetSize(&w, &h);
    GetPageSizePixels(&pageWidth, &pageHeight);

    // If printer pageWidth == current DC width, then this doesn't change. But w
    // might be the preview bitmap width, so scale down.
    float overallScale = scale * (float)(w/(float)pageWidth);
    dc->SetUserScale(overallScale, overallScale);

    // Calculate conversion factor for converting millimetres into logical
    // units. There are approx. 25.4 mm to the inch. There are ppi device units
    // to the inch. Therefore 1 mm corresponds to ppi/25.4 device units. We also
    // divide by the screen-to-printer scaling factor, because we need to
    // unscale to pass logical units to DrawLine.

    // Draw 50 mm by 50 mm L shape
    float logUnitsFactor = (float)(ppiPrinterX/(scale*25.4));
    float logUnits = (float)(50*logUnitsFactor);
    dc->SetPen(* wxBLACK_PEN);
    dc->DrawLine(50, 250, (long)(50.0 + logUnits), 250);
    dc->DrawLine(50, 250, 50, (long)(250.0 + logUnits));

    dc->SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
    dc->SetBrush(*wxTRANSPARENT_BRUSH);

    { // GetTextExtent demo:
        wxString words[7] = { "This ", "is ", "GetTextExtent ",
                             "testing ", "string. ", "Enjoy ", "it!" };
        long x = 200, y= 250;

        dc->SetFont(wxFontInfo(15).Family(wxFONTFAMILY_SWISS));

        for (int i = 0; i < 7; i++)
        {
            wxCoord wordWidth, wordHeight;
            wxString word = words[i];
            word.Remove( word.Len()-1, 1 );
            dc->GetTextExtent(word, &wordWidth, &wordHeight);
            dc->DrawRectangle(x, y, wordWidth, wordHeight);
            dc->GetTextExtent(words[i], &wordWidth, &wordHeight);
            dc->DrawText(words[i], x, y);
            x += wordWidth;
        }

    }

    dc->SetFont(wxGetApp().GetTestFont());

    dc->DrawText("Some test text", 200, 300 );

    // TESTING

    int leftMargin = 20;
    int rightMargin = 20;
    int topMargin = 20;
    int bottomMargin = 20;

    int pageWidthMM, pageHeightMM;
    GetPageSizeMM(&pageWidthMM, &pageHeightMM);

    float leftMarginLogical = (float)(logUnitsFactor*leftMargin);
    float topMarginLogical = (float)(logUnitsFactor*topMargin);
    float bottomMarginLogical = (float)(logUnitsFactor*(pageHeightMM - bottomMargin));
    float rightMarginLogical = (float)(logUnitsFactor*(pageWidthMM - rightMargin));

    dc->SetPen(* wxRED_PEN);
    dc->DrawLine( (long)leftMarginLogical, (long)topMarginLogical,
        (long)rightMarginLogical, (long)topMarginLogical);
    dc->DrawLine( (long)leftMarginLogical, (long)bottomMarginLogical,
        (long)rightMarginLogical,  (long)bottomMarginLogical);

    WritePageHeader(this, dc, "A header", logUnitsFactor);
}

// Writes a header on a page. Margin units are in millimetres.
bool MyPrintout::WritePageHeader(wxPrintout *printout, wxDC *dc, const wxString&text, float mmToLogical)
{
    int pageWidthMM, pageHeightMM;

    printout->GetPageSizeMM(&pageWidthMM, &pageHeightMM);
    wxUnusedVar(pageHeightMM);

    int leftMargin = 10;
    int topMargin = 10;
    int rightMargin = 10;

    float leftMarginLogical = (float)(mmToLogical*leftMargin);
    float topMarginLogical = (float)(mmToLogical*topMargin);
    float rightMarginLogical = (float)(mmToLogical*(pageWidthMM - rightMargin));

    wxCoord xExtent, yExtent;
    dc->GetTextExtent(text, &xExtent, &yExtent);

    float xPos = (float)(((((pageWidthMM - leftMargin - rightMargin)/2.0)+leftMargin)*mmToLogical) - (xExtent/2.0));
    dc->DrawText(text, (long)xPos, (long)topMarginLogical);

    dc->SetPen(* wxBLACK_PEN);
    dc->DrawLine( (long)leftMarginLogical, (long)(topMarginLogical+yExtent),
                  (long)rightMarginLogical, (long)topMarginLogical+yExtent );

    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Name:        printing.cpp
// Purpose:     Printing demo for wxWindows
// Author:      Julian Smart
// Modified by:
// Created:     1995
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_PRINTING_ARCHITECTURE
#error You must set wxUSE_PRINTING_ARCHITECTURE to 1 in setup.h to compile this demo.
#endif

// Set this to 1 if you want to test PostScript printing under MSW.
// However, you'll also need to edit src/msw/makefile.nt.
#define wxTEST_POSTSCRIPT_IN_MSW 0

#include <ctype.h>
#include "wx/metafile.h"
#include "wx/print.h"
#include "wx/printdlg.h"

#include "wx/accel.h"

#if wxTEST_POSTSCRIPT_IN_MSW
#include "wx/generic/printps.h"
#include "wx/generic/prntdlgg.h"
#endif

#include "printing.h"

#ifndef __WXMSW__
#include "mondrian.xpm"
#endif

// Declare a frame
MyFrame   *frame = (MyFrame *) NULL;
// int orientation = wxPORTRAIT;

// Global print data, to remember settings during the session
wxPrintData *g_printData = (wxPrintData*) NULL ;

// Global page setup data
wxPageSetupData* g_pageSetupData = (wxPageSetupData*) NULL;

// Main proc
IMPLEMENT_APP(MyApp)

// Writes a header on a page. Margin units are in millimetres.
bool WritePageHeader(wxPrintout *printout, wxDC *dc, char *text, float mmToLogical);

MyApp::MyApp()
{
}

// The `main program' equivalent, creating the windows and returning the
// main frame
bool MyApp::OnInit(void)
{
    m_testFont = new wxFont(10, wxSWISS, wxNORMAL, wxNORMAL);
    
    g_printData = new wxPrintData;
    g_pageSetupData = new wxPageSetupDialogData;

    // Compatibility with old system. In fact, we might keep wxThePrintSetupData
    // just for useful default values which we can optionally assign to our
    // own print data object.

#if defined(__WXGTK__) || defined(__WXMOTIF__)
    (*g_printData) = * wxThePrintSetupData;
#endif
    
    // Create the main frame window
    frame = new MyFrame((wxFrame *) NULL, (char *) "wxWindows Printing Demo", wxPoint(0, 0), wxSize(400, 400));
    
    // Give it a status line
    frame->CreateStatusBar(2);
    
    // Load icon and bitmap
    frame->SetIcon( wxICON( mondrian) );
    
    // Make a menubar
    wxMenu *file_menu = new wxMenu;
    
    file_menu->Append(WXPRINT_PRINT, "&Print...",              "Print");
    file_menu->Append(WXPRINT_PRINT_SETUP, "Print &Setup...",              "Setup printer properties");
    file_menu->Append(WXPRINT_PAGE_SETUP, "Page Set&up...",              "Page setup");
    file_menu->Append(WXPRINT_PREVIEW, "Print Pre&view",              "Preview");
    
#if wxUSE_ACCEL
    // Accelerators
    wxAcceleratorEntry entries[1];
    entries[0].Set(wxACCEL_CTRL, (int) 'V', WXPRINT_PREVIEW);
    wxAcceleratorTable accel(1, entries);
    frame->SetAcceleratorTable(accel);
#endif

#if defined(__WXMSW__) && wxTEST_POSTSCRIPT_IN_MSW
    file_menu->AppendSeparator();
    file_menu->Append(WXPRINT_PRINT_PS, "Print PostScript...",              "Print (PostScript)");
    file_menu->Append(WXPRINT_PRINT_SETUP_PS, "Print Setup PostScript...",              "Setup printer properties (PostScript)");
    file_menu->Append(WXPRINT_PAGE_SETUP_PS, "Page Setup PostScript...",              "Page setup (PostScript)");
    file_menu->Append(WXPRINT_PREVIEW_PS, "Print Preview PostScript",              "Preview (PostScript)");
#endif
    file_menu->AppendSeparator();
    file_menu->Append(WXPRINT_QUIT, "E&xit",                "Exit program");
    
    wxMenu *help_menu = new wxMenu;
    help_menu->Append(WXPRINT_ABOUT, "&About",              "About this demo");
    
    wxMenuBar *menu_bar = new wxMenuBar;
    
    menu_bar->Append(file_menu, "&File");
    menu_bar->Append(help_menu, "&Help");
    
    // Associate the menu bar with the frame
    frame->SetMenuBar(menu_bar);
    
    MyCanvas *canvas = new MyCanvas(frame, wxPoint(0, 0), wxSize(100, 100), wxRETAINED|wxHSCROLL|wxVSCROLL);
    
    // Give it scrollbars: the virtual canvas is 20 * 50 = 1000 pixels in each direction
    canvas->SetScrollbars(20, 20, 50, 50);
    
    frame->canvas = canvas;
    
    frame->Centre(wxBOTH);
    frame->Show(TRUE);
    
    frame->SetStatusText("Printing demo");
    
    SetTopWindow(frame);
    
    return TRUE;
}

int MyApp::OnExit()
{
    delete wxGetApp().m_testFont;
    delete g_printData;
    delete g_pageSetupData;
    return 1;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(WXPRINT_QUIT, MyFrame::OnExit)
EVT_MENU(WXPRINT_PRINT, MyFrame::OnPrint)
EVT_MENU(WXPRINT_PREVIEW, MyFrame::OnPrintPreview)
EVT_MENU(WXPRINT_PRINT_SETUP, MyFrame::OnPrintSetup)
EVT_MENU(WXPRINT_PAGE_SETUP, MyFrame::OnPageSetup)
EVT_MENU(WXPRINT_ABOUT, MyFrame::OnPrintAbout)
#if defined(__WXMSW__) && wxTEST_POSTSCRIPT_IN_MSW
EVT_MENU(WXPRINT_PRINT_PS, MyFrame::OnPrintPS)
EVT_MENU(WXPRINT_PREVIEW_PS, MyFrame::OnPrintPreviewPS)
EVT_MENU(WXPRINT_PRINT_SETUP_PS, MyFrame::OnPrintSetupPS)
EVT_MENU(WXPRINT_PAGE_SETUP_PS, MyFrame::OnPageSetupPS)
#endif
END_EVENT_TABLE()

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size):
wxFrame(frame, -1, title, pos, size)
{
    canvas = (MyCanvas *) NULL;
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void MyFrame::OnPrint(wxCommandEvent& WXUNUSED(event))
{
    wxPrintDialogData printDialogData(* g_printData);

    wxPrinter printer(& printDialogData);
    MyPrintout printout("My printout");
    if (!printer.Print(this, &printout, TRUE))
        wxMessageBox("There was a problem printing.\nPerhaps your current printer is not set correctly?", "Printing", wxOK);
    else
    {
        (*g_printData) = printer.GetPrintDialogData().GetPrintData();
    }
}

void MyFrame::OnPrintPreview(wxCommandEvent& WXUNUSED(event))
{
    // Pass two printout objects: for preview, and possible printing.
    wxPrintDialogData printDialogData(* g_printData);
    wxPrintPreview *preview = new wxPrintPreview(new MyPrintout, new MyPrintout, & printDialogData);
    if (!preview->Ok())
    {
        delete preview;
        wxMessageBox("There was a problem previewing.\nPerhaps your current printer is not set correctly?", "Previewing", wxOK);
        return;
    }
    
    wxPreviewFrame *frame = new wxPreviewFrame(preview, this, "Demo Print Preview", wxPoint(100, 100), wxSize(600, 650));
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show(TRUE);
}

void MyFrame::OnPrintSetup(wxCommandEvent& WXUNUSED(event))
{
    wxPrintDialogData printDialogData(* g_printData);
    wxPrintDialog printerDialog(this, & printDialogData);
    
    printerDialog.GetPrintDialogData().SetSetupDialog(TRUE);
    printerDialog.ShowModal();

    (*g_printData) = printerDialog.GetPrintDialogData().GetPrintData();
}

void MyFrame::OnPageSetup(wxCommandEvent& WXUNUSED(event))
{
    (*g_pageSetupData) = * g_printData;

    wxPageSetupDialog pageSetupDialog(this, g_pageSetupData);
    pageSetupDialog.ShowModal();
    
    (*g_printData) = pageSetupDialog.GetPageSetupData().GetPrintData();
    (*g_pageSetupData) = pageSetupDialog.GetPageSetupData();
}

#if defined(__WXMSW__) && wxTEST_POSTSCRIPT_IN_MSW
void MyFrame::OnPrintPS(wxCommandEvent& WXUNUSED(event))
{
    wxPostScriptPrinter printer(g_printData);
    MyPrintout printout("My printout");
    printer.Print(this, &printout, TRUE);

    (*g_printData) = printer.GetPrintData();
}

void MyFrame::OnPrintPreviewPS(wxCommandEvent& WXUNUSED(event))
{
    // Pass two printout objects: for preview, and possible printing.
    wxPrintDialogData printDialogData(* g_printData);
    wxPrintPreview *preview = new wxPrintPreview(new MyPrintout, new MyPrintout, & printDialogData);
    wxPreviewFrame *frame = new wxPreviewFrame(preview, this, "Demo Print Preview", wxPoint(100, 100), wxSize(600, 650));
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show(TRUE);
}

void MyFrame::OnPrintSetupPS(wxCommandEvent& WXUNUSED(event))
{
    wxPrintDialogData printDialogData(* g_printData);
    wxGenericPrintDialog printerDialog(this, & printDialogData);

    printerDialog.GetPrintDialogData().SetSetupDialog(TRUE);
    printerDialog.ShowModal();

    (*g_printData) = printerDialog.GetPrintDialogData().GetPrintData();
}

void MyFrame::OnPageSetupPS(wxCommandEvent& WXUNUSED(event))
{
    (*g_pageSetupData) = * g_printData;

    wxGenericPageSetupDialog pageSetupDialog(this, g_pageSetupData);
    pageSetupDialog.ShowModal();

    (*g_printData) = pageSetupDialog.GetPageSetupData().GetPrintData();
    (*g_pageSetupData) = pageSetupDialog.GetPageSetupData();
}
#endif


void MyFrame::OnPrintAbout(wxCommandEvent& WXUNUSED(event))
{
    (void)wxMessageBox("wxWindows printing demo\nAuthor: Julian Smart julian.smart@ukonline.co.uk",
        "About wxWindows printing demo", wxOK|wxCENTRE);
}

void MyFrame::Draw(wxDC& dc)
{
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();
    dc.SetFont(* wxGetApp().m_testFont);
    
    dc.SetBackgroundMode(wxTRANSPARENT);
    
    dc.SetBrush(* wxCYAN_BRUSH);
    dc.SetPen(* wxRED_PEN);
    
    dc.DrawRectangle(0, 30, 200, 100);
    dc.DrawText("Rectangle 200 by 100", 40, 40);
    
    dc.DrawEllipse(50, 140, 100, 50);
    
    dc.DrawText("Test message: this is in 10 point text", 10, 180);
    
    dc.SetPen(* wxBLACK_PEN);
    dc.DrawLine(0, 0, 200, 200);
    dc.DrawLine(200, 0, 0, 200);
    
    wxIcon my_icon = wxICON(mondrian) ;
    
    dc.DrawIcon( my_icon, 100, 100);
}

void MyFrame::OnSize(wxSizeEvent& event )
{
    wxFrame::OnSize(event);
}

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
EVT_MOUSE_EVENTS(MyCanvas::OnEvent)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxFrame *frame, const wxPoint& pos, const wxSize& size, long style):
    wxScrolledWindow(frame, -1, pos, size, style)
{
    SetBackgroundColour(* wxWHITE);
}

MyCanvas::~MyCanvas(void)
{
}

// Define the repainting behaviour
void MyCanvas::OnDraw(wxDC& dc)
{
    frame->Draw(dc);
}

void MyCanvas::OnEvent(wxMouseEvent& WXUNUSED(event))
{
}

bool MyPrintout::OnPrintPage(int page)
{
    wxDC *dc = GetDC();
    if (dc)
    {
        if (page == 1)
            DrawPageOne(dc);
        else if (page == 2)
            DrawPageTwo(dc);
        
        dc->SetDeviceOrigin(0, 0);
        dc->SetUserScale(1.0, 1.0);
        
        char buf[200];
        sprintf(buf, "PAGE %d", page);
        dc->DrawText(buf, 10, 10);
        
        return TRUE;
    }
    else
        return FALSE;
}

bool MyPrintout::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return FALSE;
    
    return TRUE;
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

void MyPrintout::DrawPageOne(wxDC *dc)
{
/* You might use THIS code if you were scaling
* graphics of known size to fit on the page.
    */
    int w, h;
    
    // We know the graphic is 200x200. If we didn't know this,
    // we'd need to calculate it.
    float maxX = 200;
    float maxY = 200;
    
    // Let's have at least 50 device units margin
    float marginX = 50;
    float marginY = 50;
    
    // Add the margin to the graphic size
    maxX += (2*marginX);
    maxY += (2*marginY);
    
    // Get the size of the DC in pixels
    dc->GetSize(&w, &h);
    
    // Calculate a suitable scaling factor
    float scaleX=(float)(w/maxX);
    float scaleY=(float)(h/maxY);
    
    // Use x or y scaling factor, whichever fits on the DC
    float actualScale = wxMin(scaleX,scaleY);
    
    // Calculate the position on the DC for centring the graphic
    float posX = (float)((w - (200*actualScale))/2.0);
    float posY = (float)((h - (200*actualScale))/2.0);
    
    // Set the scale and origin
    dc->SetUserScale(actualScale, actualScale);
    dc->SetDeviceOrigin( (long)posX, (long)posY );
    
    frame->Draw(*dc);
}

void MyPrintout::DrawPageTwo(wxDC *dc)
{
/* You might use THIS code to set the printer DC to ROUGHLY reflect
* the screen text size. This page also draws lines of actual length 5cm
* on the page.
    */
    // Get the logical pixels per inch of screen and printer
    int ppiScreenX, ppiScreenY;
    GetPPIScreen(&ppiScreenX, &ppiScreenY);
    int ppiPrinterX, ppiPrinterY;
    GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);
    
    // This scales the DC so that the printout roughly represents the
    // the screen scaling. The text point size _should_ be the right size
    // but in fact is too small for some reason. This is a detail that will
    // need to be addressed at some point but can be fudged for the
    // moment.
    float scale = (float)((float)ppiPrinterX/(float)ppiScreenX);
    
    // Now we have to check in case our real page size is reduced
    // (e.g. because we're drawing to a print preview memory DC)
    int pageWidth, pageHeight;
    int w, h;
    dc->GetSize(&w, &h);
    GetPageSizePixels(&pageWidth, &pageHeight);
    
    // If printer pageWidth == current DC width, then this doesn't
    // change. But w might be the preview bitmap width, so scale down.
    float overallScale = scale * (float)(w/(float)pageWidth);
    dc->SetUserScale(overallScale, overallScale);
    
    // Calculate conversion factor for converting millimetres into
    // logical units.
    // There are approx. 25.1 mm to the inch. There are ppi
    // device units to the inch. Therefore 1 mm corresponds to
    // ppi/25.1 device units. We also divide by the
    // screen-to-printer scaling factor, because we need to
    // unscale to pass logical units to DrawLine.
    
    // Draw 50 mm by 50 mm L shape
    float logUnitsFactor = (float)(ppiPrinterX/(scale*25.1));
    float logUnits = (float)(50*logUnitsFactor);
    dc->SetPen(* wxBLACK_PEN);
    dc->DrawLine(50, 250, (long)(50.0 + logUnits), 250);
    dc->DrawLine(50, 250, 50, (long)(250.0 + logUnits));
    
    dc->SetFont(* wxGetApp().m_testFont);
    dc->SetBackgroundMode(wxTRANSPARENT);
    
    dc->DrawText("Some test text", 200, 200 );
    
    { // GetTextExtent demo:
        wxString words[7] = {"This ", "is ", "GetTextExtent ", "testing ", "string. ", "Enjoy ", "it!"};
        long w, h;
        long x = 200, y= 250;
        wxFont fnt(15, wxSWISS, wxNORMAL, wxNORMAL);
        
        dc->SetFont(fnt);
        for (int i = 0; i < 7; i++) {
            dc->GetTextExtent(words[i], &w, &h);
            dc->DrawRectangle(x, y, w, h);
            dc->DrawText(words[i], x, y);
            x += w;
        }
        dc->SetFont(* wxGetApp().m_testFont);
    }
    
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
bool WritePageHeader(wxPrintout *printout, wxDC *dc, char *text, float mmToLogical)
{
/*
static wxFont *headerFont = (wxFont *) NULL;
if (!headerFont)
{
headerFont = wxTheFontList->FindOrCreateFont(16, wxSWISS, wxNORMAL, wxBOLD);
}
dc->SetFont(headerFont);
    */
    
    int pageWidthMM, pageHeightMM;
    
    printout->GetPageSizeMM(&pageWidthMM, &pageHeightMM);
    
    int leftMargin = 10;
    int topMargin = 10;
    int rightMargin = 10;
    
    float leftMarginLogical = (float)(mmToLogical*leftMargin);
    float topMarginLogical = (float)(mmToLogical*topMargin);
    float rightMarginLogical = (float)(mmToLogical*(pageWidthMM - rightMargin));
    
    long xExtent, yExtent;
    dc->GetTextExtent(text, &xExtent, &yExtent);
    float xPos = (float)(((((pageWidthMM - leftMargin - rightMargin)/2.0)+leftMargin)*mmToLogical) - (xExtent/2.0));
    dc->DrawText(text, (long)xPos, (long)topMarginLogical);
    
    dc->SetPen(* wxBLACK_PEN);
    dc->DrawLine( (long)leftMarginLogical, (long)(topMarginLogical+yExtent), 
        (long)rightMarginLogical, (long)topMarginLogical+yExtent );
    
    return TRUE;
}

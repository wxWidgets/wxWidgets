/*
 * File:	printing.cc
 * Purpose:	Printing demo for wxWindows class library
 * Author:	Julian Smart
 *          modified by Vaclav Slavik (wxHTML stuffs)
 * Created:	1995
 * Updated:	
 * Copyright:   (c) 1995, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

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

//!!! DON'T DO THAT! This is wxHTML sample now
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

#include <wx/wxhtml.h>
#include <wx/wfstream.h>
#include "printing.h"

#ifndef __WXMSW__
#include "mondrian.xpm"
#endif

// Global print data, to remember settings during the session
wxPrintData *g_printData = (wxPrintData*) NULL ;

// Global page setup data
wxPageSetupData* g_pageSetupData = (wxPageSetupData*) NULL;


// Declare a frame
MyFrame   *frame = (MyFrame *) NULL;
wxHtmlWindow *html = NULL;
int orientation = wxPORTRAIT;

// Main proc
IMPLEMENT_APP(MyApp)


MyApp::MyApp()
{
}

// The `main program' equivalent, creating the windows and returning the
// main frame
bool MyApp::OnInit(void)
{
    g_printData = new wxPrintData;
    g_pageSetupData = new wxPageSetupDialogData;

  // Create the main frame window
  frame = new MyFrame((wxFrame *) NULL, (char *) "wxWindows Printing Demo", wxPoint(0, 0), wxSize(600, 400));

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

    // Accelerators
    wxAcceleratorEntry entries[1];
    entries[0].Set(wxACCEL_CTRL, (int) 'V', WXPRINT_PREVIEW);
    wxAcceleratorTable accel(1, entries);
    frame->SetAcceleratorTable(accel);
    
  file_menu->AppendSeparator();
  file_menu->Append(WXPRINT_QUIT, "E&xit",                "Exit program");

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(WXPRINT_ABOUT, "&About",              "About this demo");

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(help_menu, "&Help");

  // Associate the menu bar with the frame
  frame->SetMenuBar(menu_bar);

  frame->Centre(wxBOTH);
  frame->Show(TRUE);

  frame->SetStatusText("Printing demo");

  SetTopWindow(frame);

  return TRUE;
}

int MyApp::OnExit()
{
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
END_EVENT_TABLE()

// Define my frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size):
  wxFrame(frame, -1, title, pos, size)
{
  html = new wxHtmlWindow(this);
  html -> LoadPage("test.htm");
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
      Close(TRUE);
}

void MyFrame::OnPrint(wxCommandEvent& WXUNUSED(event))
{
      wxPrinter printer;
      MyPrintout printout("My printout");
      if (!printer.Print(this, &printout, TRUE))
        wxMessageBox("There was a problem printing.\nPerhaps your current printer is not set correctly?", "Printing", wxOK);
}

void MyFrame::OnPrintPreview(wxCommandEvent& WXUNUSED(event))
{
      wxPrintData printData;
      printData.SetOrientation(orientation);

      // Pass two printout objects: for preview, and possible printing.
      wxPrintPreview *preview = new wxPrintPreview(new MyPrintout, new MyPrintout, & printData);
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



void MyFrame::OnPrintAbout(wxCommandEvent& WXUNUSED(event))
{
      (void)wxMessageBox("wxWindows printing demo\nAuthor: Julian Smart julian.smart@ukonline.co.uk\n\nModified by Vaclav Slavik to show wxHtml features",
            "About wxWindows printing demo", wxOK|wxCENTRE);
}


bool MyPrintout::OnPrintPage(int page)
{
  wxDC *dc = GetDC();
  if (dc)
  {
    if (page == 1)
      DrawPageOne(dc);

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
  *maxPage = 1;
  *selPageFrom = 1;
  *selPageTo = 1;
}

bool MyPrintout::HasPage(int pageNum)
{
  return (pageNum == 1);
}


void MyPrintout::DrawPageOne(wxDC *dc)
{
  int leftMargin = 20;
  int topMargin = 40;

/* You might use THIS code to set the printer DC to ROUGHLY reflect
 * the screen text size. This page also draws lines of actual length 5cm
 * on the page.
 */
  // Get the logical pixels per inch of screen and printer
  int ppiScreenX, ppiScreenY;
  GetPPIScreen(&ppiScreenX, &ppiScreenY);
  int ppiPrinterX, ppiPrinterY;
  GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);

  // Here we obtain internal cell representation of HTML document:
  wxHtmlContainerCell *cell = html -> GetInternalRepresentation();

  // Now we have to check in case our real page size is reduced
  // (e.g. because we're drawing to a print preview memory DC)
  int pageWidth, pageHeight;
  int w, h;
  dc->GetSize(&w, &h);
  GetPageSizePixels(&pageWidth, &pageHeight);

  // Now we must scale it somehow. The best would be to suppose that html window
  // width is equal to page width:

  float scale = (float)((float)(pageWidth - 0 * leftMargin)/((float)cell -> GetMaxLineWidth() + 2 * leftMargin));

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

  dc->SetBackgroundMode(wxTRANSPARENT);

  // TESTING

  int pageWidthMM, pageHeightMM;
  GetPageSizeMM(&pageWidthMM, &pageHeightMM);


  // This is all the printing :
  cell -> Draw(*dc, leftMargin, topMargin, 0, cell -> GetHeight());
}




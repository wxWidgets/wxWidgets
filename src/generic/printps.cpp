/////////////////////////////////////////////////////////////////////////////
// Name:        printps.cpp
// Purpose:     Postscript print/preview framework
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "printps.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#ifndef WX_PRECOMP
#include "wx/utils.h"
#include "wx/dc.h"
#include "wx/app.h"
#include "wx/msgdlg.h"
#include <wx/intl.h>
#endif

#include "wx/generic/printps.h"
#include "wx/dcprint.h"
#include "wx/printdlg.h"
#include "wx/generic/prntdlgg.h"

#include <stdlib.h>

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxPostScriptPrinter, wxPrinterBase)
IMPLEMENT_CLASS(wxPostScriptPrintPreview, wxPrintPreviewBase)
#endif

/*
 * Printer
 */
 
wxPostScriptPrinter::wxPostScriptPrinter(wxPrintData *data):
    wxPrinterBase(data)
{
}

wxPostScriptPrinter::~wxPostScriptPrinter(void)
{
}

bool wxPostScriptPrinter::Print(wxWindow *parent, wxPrintout *printout, bool prompt)
{
  abortIt = FALSE;
  abortWindow = (wxWindow *) NULL;

  if (!printout)
    return FALSE;
    
  printout->SetIsPreview(FALSE);
  printout->OnPreparePrinting();

  // Get some parameters from the printout, if defined
  int fromPage, toPage;
  int minPage, maxPage;
  printout->GetPageInfo(&minPage, &maxPage, &fromPage, &toPage);

  if (maxPage == 0)
    return FALSE;

  printData.SetMinPage(minPage);
  printData.SetMaxPage(maxPage);
  if (fromPage != 0)
    printData.SetFromPage(fromPage);
  if (toPage != 0)
    printData.SetToPage(toPage);

  if (minPage != 0)
  {
    printData.EnablePageNumbers(TRUE);
    if (printData.GetFromPage() < printData.GetMinPage())
      printData.SetFromPage(printData.GetMinPage());
    else if (printData.GetFromPage() > printData.GetMaxPage())
      printData.SetFromPage(printData.GetMaxPage());
    if (printData.GetToPage() > printData.GetMaxPage())
      printData.SetToPage(printData.GetMaxPage());
    else if (printData.GetToPage() < printData.GetMinPage())
      printData.SetToPage(printData.GetMinPage());
  }
  else
    printData.EnablePageNumbers(FALSE);
  
  // Create a suitable device context  
  wxDC *dc = (wxDC *) NULL;
  if (prompt)
  {
     wxGenericPrintDialog dialog(parent, & printData);
     if (dialog.ShowModal() == wxID_OK)
     {
       dc = dialog.GetPrintDC();
       printData = dialog.GetPrintData();
     }
  }
  else
  {
     dc = new wxPostScriptDC(wxThePrintSetupData->GetPrinterFile(), FALSE, (wxWindow *) NULL);
  }

  // May have pressed cancel.
  if (!dc || !dc->Ok())
  {
    if (dc) delete dc;
    return FALSE;
  }
  
  int logPPIScreenX = 0;
  int logPPIScreenY = 0;
  int logPPIPrinterX = 0;
  int logPPIPrinterY = 0;

  // Correct values for X/PostScript?
  logPPIScreenX = 100;
  logPPIScreenY = 100;
  logPPIPrinterX = 100;
  logPPIPrinterY = 100;

  printout->SetPPIScreen(logPPIScreenX, logPPIScreenY);
  printout->SetPPIPrinter(logPPIPrinterX, logPPIPrinterY);

  // Set printout parameters  
  printout->SetDC(dc);

  int w, h;
  long ww, hh;
  dc->GetSize(&w, &h);
  printout->SetPageSizePixels((int)w, (int)h);
  dc->GetSizeMM(&ww, &hh);
  printout->SetPageSizeMM((int)ww, (int)hh);

  // Create an abort window
  wxBeginBusyCursor();

  printout->OnBeginPrinting();
  
  bool keepGoing = TRUE;

  int copyCount;
  for (copyCount = 1; copyCount <= printData.GetNoCopies(); copyCount ++)
  {
    if (!printout->OnBeginDocument(printData.GetFromPage(), printData.GetToPage()))
    {
      wxEndBusyCursor();
      wxMessageBox(_("Could not start printing."), _("Print Error"), wxOK, parent);
      break;
    }
    if (abortIt)
      break;

    int pn;
    for (pn = printData.GetFromPage(); keepGoing && (pn <= printData.GetToPage()) && printout->HasPage(pn);
         pn++)
    {
      if (abortIt)
      {
        keepGoing = FALSE;
        break;
      }
      else
      {
        dc->StartPage();
        printout->OnPrintPage(pn);
        dc->EndPage();
      }
    }
    printout->OnEndDocument();
  }

  printout->OnEndPrinting();

  wxEndBusyCursor();

  delete dc;
  
  return TRUE;
}

bool wxPostScriptPrinter::PrintDialog(wxWindow *parent)
{
    wxGenericPrintDialog dialog(parent, & printData);
    return (dialog.ShowModal() == wxID_OK);
}

bool wxPostScriptPrinter::Setup(wxWindow *parent)
{
    wxGenericPrintDialog dialog(parent, & printData);
    dialog.GetPrintData().SetSetupDialog(TRUE);
    return (dialog.ShowModal() == wxID_OK);
}

/*
 * Print preview
 */

wxPostScriptPrintPreview::wxPostScriptPrintPreview(wxPrintout *printout, wxPrintout *printoutForPrinting, wxPrintData *data):
  wxPrintPreviewBase(printout, printoutForPrinting, data)
{
  // Have to call it here since base constructor can't call it
  DetermineScaling();
}

wxPostScriptPrintPreview::~wxPostScriptPrintPreview(void)
{
}

bool wxPostScriptPrintPreview::Print(bool interactive)
{
  if (!printPrintout)
    return FALSE;
  wxPostScriptPrinter printer(&printData);
  return printer.Print(previewFrame, printPrintout, interactive);
}

void wxPostScriptPrintPreview::DetermineScaling(void)
{
    const char *paperType = wxThePrintSetupData->GetPaperName();
    if (!paperType)
      paperType = _("A4 210 x 297 mm");

    wxPrintPaperType *paper = wxThePrintPaperDatabase->FindPaperType(paperType);
    if (!paper)
      paper = wxThePrintPaperDatabase->FindPaperType(_("A4 210 x 297 mm"));
    if (paper)
    {
      previewPrintout->SetPPIScreen(100, 100);
      previewPrintout->SetPPIPrinter(100, 100);

      // If in landscape mode, we need to swap the width and height.
      if ( printData.GetOrientation() == wxLANDSCAPE )
      {
        pageWidth = paper->heightPixels;
        pageHeight = paper->widthPixels;
        previewPrintout->SetPageSizeMM(paper->heightMM, paper->widthMM);
        previewPrintout->SetPageSizePixels(paper->heightPixels, paper->widthPixels);
      }
      else
      {
        pageWidth = paper->widthPixels;
        pageHeight = paper->heightPixels;
        previewPrintout->SetPageSizeMM(paper->widthMM, paper->heightMM);
        previewPrintout->SetPageSizePixels(paper->widthPixels, paper->heightPixels);
      }

      // At 100%, the page should look about page-size on the screen.
      previewScale = (float)0.8;
//      previewScale = (float)((float)screenWidth/(float)printerWidth);
//      previewScale = previewScale * (float)((float)screenXRes/(float)printerYRes);
    }
}


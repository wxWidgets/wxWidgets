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
  sm_abortIt = FALSE;
  sm_abortWindow = (wxWindow *) NULL;

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

  m_printData.SetMinPage(minPage);
  m_printData.SetMaxPage(maxPage);
  if (fromPage != 0)
    m_printData.SetFromPage(fromPage);
  if (toPage != 0)
    m_printData.SetToPage(toPage);

  if (minPage != 0)
  {
    m_printData.EnablePageNumbers(TRUE);
    if (m_printData.GetFromPage() < m_printData.GetMinPage())
      m_printData.SetFromPage(m_printData.GetMinPage());
    else if (m_printData.GetFromPage() > m_printData.GetMaxPage())
      m_printData.SetFromPage(m_printData.GetMaxPage());
    if (m_printData.GetToPage() > m_printData.GetMaxPage())
      m_printData.SetToPage(m_printData.GetMaxPage());
    else if (m_printData.GetToPage() < m_printData.GetMinPage())
      m_printData.SetToPage(m_printData.GetMinPage());
  }
  else
    m_printData.EnablePageNumbers(FALSE);

  // Create a suitable device context  
  wxDC *dc = (wxDC *) NULL;
  if (prompt)
  {
     wxGenericPrintDialog dialog(parent, & m_printData);
     if (dialog.ShowModal() == wxID_OK)
     {
       dc = dialog.GetPrintDC();
       m_printData = dialog.GetPrintData();
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

  logPPIScreenX = 100;
  logPPIScreenY = 100;
  
/*
  // Correct values for X/PostScript?
  logPPIPrinterX = 100;
  logPPIPrinterY = 100;
*/

  logPPIPrinterX = 72;
  logPPIPrinterY = 72;

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
  for (copyCount = 1; copyCount <= m_printData.GetNoCopies(); copyCount ++)
  {
    if (!printout->OnBeginDocument(m_printData.GetFromPage(), m_printData.GetToPage()))
    {
      wxEndBusyCursor();
      wxMessageBox(_("Could not start printing."), _("Print Error"), wxOK, parent);
      break;
    }
    if (sm_abortIt)
      break;

    int pn;
    for (pn = m_printData.GetFromPage(); keepGoing && (pn <= m_printData.GetToPage()) && printout->HasPage(pn);
         pn++)
    {
      if (sm_abortIt)
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
    wxGenericPrintDialog dialog(parent, & m_printData);
    return (dialog.ShowModal() == wxID_OK);
}

bool wxPostScriptPrinter::Setup(wxWindow *parent)
{
    wxGenericPrintDialog dialog(parent, & m_printData);
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
  if (!m_printPrintout)
    return FALSE;
  wxPostScriptPrinter printer(&m_printData);
  return printer.Print(m_previewFrame, m_printPrintout, interactive);
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
      m_previewPrintout->SetPPIScreen(100, 100);
//      m_previewPrintout->SetPPIPrinter(100, 100);
      m_previewPrintout->SetPPIPrinter(72, 72);

      // If in landscape mode, we need to swap the width and height.
      if ( m_printData.GetOrientation() == wxLANDSCAPE )
      {
        m_pageWidth = paper->heightPixels;
        m_pageHeight = paper->widthPixels;
        m_previewPrintout->SetPageSizeMM(paper->heightMM, paper->widthMM);
        m_previewPrintout->SetPageSizePixels(paper->heightPixels, paper->widthPixels);
      }
      else
      {
        m_pageWidth = paper->widthPixels;
        m_pageHeight = paper->heightPixels;
        m_previewPrintout->SetPageSizeMM(paper->widthMM, paper->heightMM);
        m_previewPrintout->SetPageSizePixels(paper->widthPixels, paper->heightPixels);
      }

      // At 100%, the page should look about page-size on the screen.
      m_previewScale = (float)0.8;
//      m_previewScale = (float)((float)screenWidth/(float)printerWidth);
//      m_previewScale = previewScale * (float)((float)screenXRes/(float)printerYRes);
    }
}


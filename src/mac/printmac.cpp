/////////////////////////////////////////////////////////////////////////////
// Name:        printwin.cpp
// Purpose:     wxMacPrinter framework
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "printwin.h"
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
#endif

#include "wx/mac/printmac.h"
#include "wx/dcprint.h"
#include "wx/printdlg.h"

#include <stdlib.h>

IMPLEMENT_DYNAMIC_CLASS(wxMacPrinter, wxPrinterBase)
IMPLEMENT_CLASS(wxMacPrintPreview, wxPrintPreviewBase)

/*
 * Printer
 */
 
wxMacPrinter::wxMacPrinter(wxPrintDialogData *data):
  wxPrinterBase(data)
{
}

wxMacPrinter::~wxMacPrinter(void)
{
}

bool wxMacPrinter::Print(wxWindow *parent, wxPrintout *printout, bool prompt)
{
  sm_abortIt = FALSE;
  sm_abortWindow = NULL;

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

  m_printDialogData.SetMinPage(minPage);
  m_printDialogData.SetMaxPage(maxPage);
  if (fromPage != 0)
    m_printDialogData.SetFromPage(fromPage);
  if (toPage != 0)
    m_printDialogData.SetToPage(toPage);

  if (minPage != 0)
  {
    m_printDialogData.EnablePageNumbers(TRUE);
    if (m_printDialogData.GetFromPage() < m_printDialogData.GetMinPage())
      m_printDialogData.SetFromPage(m_printDialogData.GetMinPage());
    else if (m_printDialogData.GetFromPage() > m_printDialogData.GetMaxPage())
      m_printDialogData.SetFromPage(m_printDialogData.GetMaxPage());
    if (m_printDialogData.GetToPage() > m_printDialogData.GetMaxPage())
      m_printDialogData.SetToPage(m_printDialogData.GetMaxPage());
    else if (m_printDialogData.GetToPage() < m_printDialogData.GetMinPage())
      m_printDialogData.SetToPage(m_printDialogData.GetMinPage());
  }
  else
    m_printDialogData.EnablePageNumbers(FALSE);

  // Create a suitable device context  
 // Create a suitable device context  
  wxDC *dc = NULL;
  if (prompt)
  {
 		PrOpen() ;
  		m_printDialogData.ConvertToNative() ; // make sure we have a valid handle
  		if ( m_printDialogData.m_macPrintInfo )
  		{
   			// todo incorporate the changes from a global page setup
 			if  ( ::PrStlDialog( m_printDialogData.m_macPrintInfo ) ) // we should have the page setup dialog
 			{
 				PrClose() ;
		        wxPrintDialog dialog(parent, & m_printDialogData);
		        if (dialog.ShowModal() == wxID_OK)
		        {
		          dc = dialog.GetPrintDC();
		          m_printDialogData = dialog.GetPrintData();
		        }
 			}
 			else
 			{
  				PrClose() ;
  			}
  		}
  }
  else
  {
  		dc = new wxPrinterDC( m_printDialogData.GetPrintData() ) ;
  }


  // May have pressed cancel.
  if (!dc || !dc->Ok())
  {
    if (dc) delete dc;
    return FALSE;
  }
  
	// on the mac we have always pixels as addressing mode with 72 dpi

  printout->SetPPIScreen(72, 72);
  printout->SetPPIPrinter(72, 72);

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

/*
  wxWindow *win = CreateAbortWindow(parent, printout);
  wxYield();

  if (!win)
  {
    wxEndBusyCursor();
    wxMessageBox("Sorry, could not create an abort dialog.", "Print Error", wxOK, parent);
    delete dc;
  }
  sm_abortWindow = win;
  sm_abortWindow->Show(TRUE);
  wxYield();
*/

  printout->OnBeginPrinting();
  
  bool keepGoing = TRUE;

  int copyCount;
  for (copyCount = 1; copyCount <= m_printDialogData.GetNoCopies(); copyCount ++)
  {
    if (!printout->OnBeginDocument(m_printDialogData.GetFromPage(), m_printDialogData.GetToPage()))
    {
      wxEndBusyCursor();
      wxMessageBox("Could not start printing.", "Print Error", wxOK, parent);
      break;
    }
    if (sm_abortIt)
      break;

    int pn;
    for (pn = m_printDialogData.GetFromPage(); keepGoing && (pn <= m_printDialogData.GetToPage()) && printout->HasPage(pn);
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
        keepGoing = printout->OnPrintPage(pn);
        dc->EndPage();
      }
    }
    printout->OnEndDocument();
  }

  printout->OnEndPrinting();

  if (sm_abortWindow)
  {
    sm_abortWindow->Show(FALSE);
    delete sm_abortWindow;
    sm_abortWindow = NULL;
  }

  wxEndBusyCursor();

  delete dc;
  
  return TRUE;
}

wxDC* wxMacPrinter::PrintDialog(wxWindow *parent)
{
    wxDC* dc = (wxDC*) NULL;

    wxPrintDialog dialog(parent, & m_printDialogData);
    int ret = dialog.ShowModal();

    if (ret == wxID_OK)
    {
        dc = dialog.GetPrintDC();
        m_printDialogData = dialog.GetPrintDialogData();
    }

    return dc;
}

bool wxMacPrinter::Setup(wxWindow *parent)
{
    wxPrintDialog dialog(parent, & m_printDialogData);
    dialog.GetPrintDialogData().SetSetupDialog(TRUE);

    int ret = dialog.ShowModal();

    if (ret == wxID_OK)
    {
        m_printDialogData = dialog.GetPrintDialogData();
    }

    return (ret == wxID_OK);
}

/*
 * Print preview
 */

wxMacPrintPreview::wxMacPrintPreview(wxPrintout *printout,
                                             wxPrintout *printoutForPrinting,
                                             wxPrintDialogData *data)
                     : wxPrintPreviewBase(printout, printoutForPrinting, data)
{
    DetermineScaling();
}

wxMacPrintPreview::wxMacPrintPreview(wxPrintout *printout, wxPrintout *printoutForPrinting, wxPrintData *data):
  wxPrintPreviewBase(printout, printoutForPrinting, data)
{
  DetermineScaling();
}

wxMacPrintPreview::~wxMacPrintPreview(void)
{
}

bool wxMacPrintPreview::Print(bool interactive)
{
  if (!m_printPrintout)
    return FALSE;
  wxMacPrinter printer(&m_printDialogData);
  return printer.Print(m_previewFrame, m_printPrintout, interactive);
}

void wxMacPrintPreview::DetermineScaling(void)
{
/*
    HDC dc = ::GetDC(NULL);
    int screenWidth = ::GetDeviceCaps(dc, HORZSIZE);
//    int screenHeight = ::GetDeviceCaps(dc, VERTSIZE);
    int screenXRes = ::GetDeviceCaps(dc, HORZRES);
//    int screenYRes = ::GetDeviceCaps(dc, VERTRES);
    int logPPIScreenX = ::GetDeviceCaps(dc, LOGPIXELSX);
    int logPPIScreenY = ::GetDeviceCaps(dc, LOGPIXELSY);
    m_previewPrintout->SetPPIScreen(logPPIScreenX, logPPIScreenY);

    ::ReleaseDC(NULL, dc);

    // Get a device context for the currently selected printer
    wxPrinterDC printerDC("", "", "", FALSE, m_printDialogData.GetOrientation());

    int printerWidth = 150;
    int printerHeight = 250;
    int printerXRes = 1500;
    int printerYRes = 2500;

    if (printerDC.GetHDC())
    {
      printerWidth = ::GetDeviceCaps((HDC) printerDC.GetHDC(), HORZSIZE);
      printerHeight = ::GetDeviceCaps((HDC) printerDC.GetHDC(), VERTSIZE);
      printerXRes = ::GetDeviceCaps((HDC) printerDC.GetHDC(), HORZRES);
      printerYRes = ::GetDeviceCaps((HDC) printerDC.GetHDC(), VERTRES);

      int logPPIPrinterX = ::GetDeviceCaps((HDC) printerDC.GetHDC(), LOGPIXELSX);
      int logPPIPrinterY = ::GetDeviceCaps((HDC) printerDC.GetHDC(), LOGPIXELSY);

      m_previewPrintout->SetPPIPrinter(logPPIPrinterX, logPPIPrinterY);
      m_previewPrintout->SetPageSizeMM(printerWidth, printerHeight);
      
      if (logPPIPrinterX == 0 || logPPIPrinterY == 0 || printerWidth == 0 || printerHeight == 0)
        m_isOk = FALSE;
    }
    else
      m_isOk = FALSE;

    m_pageWidth = printerXRes;
    m_pageHeight = printerYRes;

    // At 100%, the page should look about page-size on the screen.
    m_previewScale = (float)((float)screenWidth/(float)printerWidth);
    m_previewScale = m_previewScale * (float)((float)screenXRes/(float)printerYRes);
*/
}

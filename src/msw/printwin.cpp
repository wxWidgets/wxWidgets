/////////////////////////////////////////////////////////////////////////////
// Name:        printwin.cpp
// Purpose:     wxWindowsPrinter framework
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

#define WINDOWS_PRINTING    (wxTheApp->GetPrintMode() == wxPRINT_WINDOWS)

#ifndef WX_PRECOMP
#include "wx/utils.h"
#include "wx/dc.h"
#include "wx/app.h"
#include "wx/msgdlg.h"
#endif

#include "wx/msw/printwin.h"
#include "wx/dcprint.h"
#include "wx/printdlg.h"
#include "wx/msw/private.h"

#include <stdlib.h>
#include <windows.h>
#include <commdlg.h>

// Clash with Windows header files
#ifdef StartDoc
#undef StartDoc
#endif

#ifndef __WIN32__
#include <print.h>
#endif

LONG APIENTRY _EXPORT wxAbortProc(HDC hPr, int Code);

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxWindowsPrinter, wxPrinterBase)
IMPLEMENT_CLASS(wxWindowsPrintPreview, wxPrintPreviewBase)
#endif

/*
 * Printer
 */
 
wxWindowsPrinter::wxWindowsPrinter(wxPrintData *data):
  wxPrinterBase(data)
{
  lpAbortProc = (WXFARPROC) MakeProcInstance((FARPROC) wxAbortProc, wxGetInstance());
}

wxWindowsPrinter::~wxWindowsPrinter(void)
{
  FreeProcInstance((FARPROC) lpAbortProc);
}

bool wxWindowsPrinter::Print(wxWindow *parent, wxPrintout *printout, bool prompt)
{
  abortIt = FALSE;
  abortWindow = NULL;

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
  wxDC *dc = NULL;
  if (prompt)
  {
        wxPrintDialog dialog(parent, & printData);
        if (dialog.ShowModal() == wxID_OK)
        {
          dc = dialog.GetPrintDC();
          printData = dialog.GetPrintData();
        }
  }
  else
  {
      dc = new wxPrinterDC("", "", "", FALSE, printData.GetOrientation());
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

  HDC hdc = ::GetDC(NULL);
  logPPIScreenX = ::GetDeviceCaps(hdc, LOGPIXELSX);
  logPPIScreenY = ::GetDeviceCaps(hdc, LOGPIXELSY);
  ::ReleaseDC(NULL, hdc);

  logPPIPrinterX = ::GetDeviceCaps((HDC) dc->GetHDC(), LOGPIXELSX);
  logPPIPrinterY = ::GetDeviceCaps((HDC) dc->GetHDC(), LOGPIXELSY);
  if (logPPIPrinterX == 0 || logPPIPrinterY == 0)
  {
    delete dc;
    return FALSE;
  }

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

  wxWindow *win = CreateAbortWindow(parent, printout);
  wxYield();

#if defined(__BORLANDC__) || defined(__GNUWIN32__)
  ::SetAbortProc((HDC) dc->GetHDC(), (FARPROC) lpAbortProc);
#else
  ::SetAbortProc((HDC) dc->GetHDC(), (int (_stdcall *)(HDC, int)) lpAbortProc);
#endif

  if (!win)
  {
    wxEndBusyCursor();
    wxMessageBox("Sorry, could not create an abort dialog.", "Print Error", wxOK, parent);
    delete dc;
  }
  abortWindow = win;
  abortWindow->Show(TRUE);
  wxYield();

  printout->OnBeginPrinting();
  
  bool keepGoing = TRUE;

  int copyCount;
  for (copyCount = 1; copyCount <= printData.GetNoCopies(); copyCount ++)
  {
    if (!printout->OnBeginDocument(printData.GetFromPage(), printData.GetToPage()))
    {
      wxEndBusyCursor();
      wxMessageBox("Could not start printing.", "Print Error", wxOK, parent);
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
//        int dcID = ::SaveDC(dc->GetHDC());
        dc->StartPage();
        printout->OnPrintPage(pn);
        dc->EndPage();
//        ::RestoreDC(dc->GetHDC(), dcID);
      }
    }
    printout->OnEndDocument();
  }

  printout->OnEndPrinting();

  if (abortWindow)
  {
    abortWindow->Show(FALSE);
    delete abortWindow;
    abortWindow = NULL;
  }

  wxEndBusyCursor();

  delete dc;
  
  return TRUE;
}

bool wxWindowsPrinter::PrintDialog(wxWindow *parent)
{
  wxPrintDialog dialog(parent, & printData);
  return (dialog.ShowModal() == wxID_OK);
}

bool wxWindowsPrinter::Setup(wxWindow *parent)
{
  wxPrintDialog dialog(parent, & printData);
  dialog.GetPrintData().SetSetupDialog(TRUE);
  return (dialog.ShowModal() == wxID_OK);
}

/*
 * Print preview
 */

wxWindowsPrintPreview::wxWindowsPrintPreview(wxPrintout *printout, wxPrintout *printoutForPrinting, wxPrintData *data):
  wxPrintPreviewBase(printout, printoutForPrinting, data)
{
  DetermineScaling();
}

wxWindowsPrintPreview::~wxWindowsPrintPreview(void)
{
}

bool wxWindowsPrintPreview::Print(bool interactive)
{
  if (!printPrintout)
    return FALSE;
  wxWindowsPrinter printer(&printData);
  return printer.Print(previewFrame, printPrintout, interactive);
}

void wxWindowsPrintPreview::DetermineScaling(void)
{
    HDC dc = ::GetDC(NULL);
    int screenWidth = ::GetDeviceCaps(dc, HORZSIZE);
//    int screenHeight = ::GetDeviceCaps(dc, VERTSIZE);
    int screenXRes = ::GetDeviceCaps(dc, HORZRES);
//    int screenYRes = ::GetDeviceCaps(dc, VERTRES);
    int logPPIScreenX = ::GetDeviceCaps(dc, LOGPIXELSX);
    int logPPIScreenY = ::GetDeviceCaps(dc, LOGPIXELSY);
    previewPrintout->SetPPIScreen(logPPIScreenX, logPPIScreenY);

    ::ReleaseDC(NULL, dc);

    // Get a device context for the currently selected printer
    wxPrinterDC printerDC("", "", "", FALSE, printData.GetOrientation());

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

      previewPrintout->SetPPIPrinter(logPPIPrinterX, logPPIPrinterY);
      previewPrintout->SetPageSizeMM(printerWidth, printerHeight);
      
      if (logPPIPrinterX == 0 || logPPIPrinterY == 0 || printerWidth == 0 || printerHeight == 0)
        isOk = FALSE;
    }
    else
      isOk = FALSE;

    pageWidth = printerXRes;
    pageHeight = printerYRes;

    // At 100%, the page should look about page-size on the screen.
    previewScale = (float)((float)screenWidth/(float)printerWidth);
    previewScale = previewScale * (float)((float)screenXRes/(float)printerYRes);
}

/****************************************************************************

    FUNCTION: wxAbortProc()

    PURPOSE:  Processes messages for the Abort Dialog box

****************************************************************************/

LONG APIENTRY _EXPORT wxAbortProc(HDC WXUNUSED(hPr), int WXUNUSED(Code))
{
    MSG msg;

    if (!wxPrinterBase::abortWindow)              /* If the abort dialog isn't up yet */
        return(TRUE);

    /* Process messages intended for the abort dialog box */

    while (!wxPrinterBase::abortIt && PeekMessage(&msg, 0, 0, 0, TRUE))
        if (!IsDialogMessage((HWND) wxPrinterBase::abortWindow->GetHWND(), &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

    /* bAbort is TRUE (return is FALSE) if the user has aborted */

    return (!wxPrinterBase::abortIt);
}


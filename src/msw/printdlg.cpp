/////////////////////////////////////////////////////////////////////////////
// Name:        printdlg.cpp
// Purpose:     wxPrintDialog, wxPageSetupDialog
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "printdlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#define IN_WX_MAIN_CPP
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif

#include "wx/printdlg.h"
#include "wx/dcprint.h"

// Have to emulate page setup dialog for Win16
#if !defined(__WIN95__)
#include "wx/generic/prntdlgg.h"
#endif

#include <stdlib.h>
#include <windows.h>
#include <commdlg.h>

#ifndef __WIN32__
#include <print.h>
#endif

// Clash with Windows header files
#ifdef StartDoc
#undef StartDoc
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxPrintDialog, wxDialog)
IMPLEMENT_CLASS(wxPageSetupDialog, wxDialog)
#endif

wxPrintDialog::wxPrintDialog(void):
 wxDialog()
{
  dialogParent = NULL;
  printerDC = NULL;
  destroyDC = TRUE;
  deviceName = NULL;
  driverName = NULL;
  portName = NULL;
}

wxPrintDialog::wxPrintDialog(wxWindow *p, wxPrintData* data):
 wxDialog()
{
  Create(p, data);
}

bool wxPrintDialog::Create(wxWindow *p, wxPrintData* data)
{
  dialogParent = p;
  printerDC = NULL;
  destroyDC = TRUE;
  deviceName = NULL;
  driverName = NULL;
  portName = NULL;

  if ( data )
    printData = *data;

#ifdef __WXMSW__
  printData.SetOwnerWindow(p);
#endif

  return TRUE;
}

wxPrintDialog::~wxPrintDialog(void)
{
  if (destroyDC && printerDC)
    delete printerDC;
  if (deviceName) delete[] deviceName;
  if (driverName) delete[] driverName;
  if (portName) delete[] portName;
}

int wxPrintDialog::ShowModal(void)
{
  printData.ConvertToNative();

  bool ret = (PrintDlg( (PRINTDLG *)printData.printData ) != 0);
  if ( ret != FALSE && ((PRINTDLG *)printData.printData)->hDC)
  {
    wxPrinterDC *pdc = new wxPrinterDC((WXHDC) ((PRINTDLG *)printData.printData)->hDC);
    printerDC = pdc;
    printData.ConvertFromNative();
    return wxID_OK;
  }
  else
  {
/*
      char buf[256];
      DWORD exError = CommDlgExtendedError();
      sprintf(buf, "ret = %d, ex error = %d", (int) ret, (int) exError);
      wxMessageBox(buf);
*/
      return wxID_CANCEL;
  }
}

wxDC *wxPrintDialog::GetPrintDC(void)
{
  if (printerDC)
  {
    destroyDC = FALSE;
    return printerDC;
  }
  else
    return NULL;
}

/*
 * wxPageSetupDialog
 */

wxPageSetupDialog::wxPageSetupDialog(void):
 wxDialog()
{
  m_dialogParent = NULL;
}

wxPageSetupDialog::wxPageSetupDialog(wxWindow *p, wxPageSetupData *data):
 wxDialog()
{
  Create(p, data);
}

bool wxPageSetupDialog::Create(wxWindow *p, wxPageSetupData *data)
{
  m_dialogParent = p;

  if (data)
    m_pageSetupData = (*data);

#if defined(__WIN95__)
  m_pageSetupData.SetOwnerWindow(p);
#endif
  return TRUE;
}

wxPageSetupDialog::~wxPageSetupDialog(void)
{
}

int wxPageSetupDialog::ShowModal(void)
{
#ifdef __WIN95__
    m_pageSetupData.ConvertToNative();
    if (PageSetupDlg( (PAGESETUPDLG *)m_pageSetupData.GetNativeData() ))
    {
      m_pageSetupData.ConvertFromNative();
      return wxID_OK;
    }
    else
      return wxID_CANCEL;
#else
    wxGenericPageSetupDialog *genericPageSetupDialog = new wxGenericPageSetupDialog(GetParent(), & m_pageSetupData);
    int ret = genericPageSetupDialog->ShowModal();
    m_pageSetupData = genericPageSetupDialog->GetPageSetupData();
    genericPageSetupDialog->Close(TRUE);
    return ret;
#endif
}


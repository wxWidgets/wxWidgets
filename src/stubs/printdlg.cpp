/////////////////////////////////////////////////////////////////////////////
// Name:        printdlg.cpp
// Purpose:     wxPrintDialog, wxPageSetupDialog
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "printdlg.h"
#endif

#include "wx/printdlg.h"
#include "wx/dcprint.h"

// Use generic page setup dialog: use your own native one if one exists.
#include "wx/generic/prntdlgg.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxPrintDialog, wxDialog)
IMPLEMENT_CLASS(wxPageSetupDialog, wxDialog)
#endif

wxPrintDialog::wxPrintDialog():
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

    return TRUE;
}

wxPrintDialog::~wxPrintDialog()
{
    if (destroyDC && printerDC)
        delete printerDC;
    if (deviceName) delete[] deviceName;
    if (driverName) delete[] driverName;
    if (portName) delete[] portName;
}

int wxPrintDialog::ShowModal()
{
    // TODO
    return wxID_CANCEL;
}

wxDC *wxPrintDialog::GetPrintDC()
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

wxPageSetupDialog::wxPageSetupDialog():
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

    return TRUE;
}

wxPageSetupDialog::~wxPageSetupDialog()
{
}

int wxPageSetupDialog::ShowModal()
{
    // Uses generic page setup dialog
    wxGenericPageSetupDialog *genericPageSetupDialog = new wxGenericPageSetupDialog(GetParent(), & m_pageSetupData);
    int ret = genericPageSetupDialog->ShowModal();
    m_pageSetupData = genericPageSetupDialog->GetPageSetupData();
    genericPageSetupDialog->Close(TRUE);
    return ret;
}


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

#include "wx/object.h"
#include "wx/stubs/printdlg.h"
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
    m_dialogParent = NULL;
    m_printerDC = NULL;
}

wxPrintDialog::wxPrintDialog(wxWindow *p, wxPrintData* data):
 wxDialog()
{
    Create(p, data);
}

bool wxPrintDialog::Create(wxWindow *p, wxPrintData* data)
{
    m_dialogParent = p;
    m_printerDC = NULL;

    if ( data )
        m_printData = *data;

    return TRUE;
}

wxPrintDialog::~wxPrintDialog()
{
    if (m_printerDC)
        delete m_printerDC;
}

int wxPrintDialog::ShowModal()
{
    // TODO
    return wxID_CANCEL;
}

wxDC *wxPrintDialog::GetPrintDC()
{
  if (m_printerDC)
  {
    wxDC* dc = m_printerDC;
    m_printerDC = NULL;
    return dc;
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


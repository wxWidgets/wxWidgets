/////////////////////////////////////////////////////////////////////////////
// Name:        print.cpp
// Purpose:     Print framework
// Author:      David Webster
// Modified by:
// Created:     10/14/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/setup.h"
#include "wx/list.h"
#include "wx/utils.h"
#include "wx/app.h"
#endif


#include "wx/os2/print.h"
#include "wx/generic/prntdlgg.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxPrinter, wxPrinterBase)
IMPLEMENT_CLASS(wxPrintPreview, wxPrintPreviewBase)
#endif

/*
 * Printer
 */

wxPrinter::wxPrinter(wxPrintData *data):
  wxPrinterBase((wxPrintDialogData*)data)
{
}

wxPrinter::~wxPrinter()
{
}

bool wxPrinter::Print(wxWindow *parent, wxPrintout *printout, bool prompt)
{
    // TODO. See wxPostScriptPrinter::Print for hints.
    return FALSE;
}

wxDC* wxPrinter::PrintDialog(wxWindow *parent)
{
// TODO:
/*
    wxPrintDialog dialog(parent, & m_printData);
    return (dialog.GetPrintDC());
*/
    return NULL;
}

bool wxPrinter::Setup(wxWindow *parent)
{
// TODO:
/*
    wxPrintDialog dialog(parent, & m_printData);
    dialog.GetPrintData().SetSetupDialog(TRUE);
    return (dialog.ShowModal() == wxID_OK);
*/
    return FALSE;
}

/*
 * Print preview
 */

wxPrintPreview::wxPrintPreview(wxPrintout *printout, wxPrintout *printoutForPrinting, wxPrintData *data):
  wxPrintPreviewBase(printout, printoutForPrinting, data)
{
    DetermineScaling();
}

wxPrintPreview::~wxPrintPreview()
{
}

bool wxPrintPreview::Print(bool interactive)
{
    if (!m_printPrintout)
        return FALSE;
//    wxPrinter printer(&m_printData);
//    return printer.Print(m_previewFrame, m_printPrintout, interactive);
    return FALSE;
}

void wxPrintPreview::DetermineScaling()
{
    // TODO
}


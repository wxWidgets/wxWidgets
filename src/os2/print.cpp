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

#if wxUSE_PRINTING_ARCHITECTURE

#include "wx/os2/printos2.h"
#include "wx/generic/prntdlgg.h"

IMPLEMENT_DYNAMIC_CLASS(wxOS2Printer, wxPrinterBase)
IMPLEMENT_CLASS(wxOS2PrintPreview, wxPrintPreviewBase)

/*
 * Printer
 */

wxOS2Printer::wxOS2Printer(wxPrintDialogData *data):
  wxPrinterBase(data)
{
}

wxOS2Printer::~wxOS2Printer()
{
}

bool wxOS2Printer::Print(wxWindow *parent, wxPrintout *printout, bool prompt)
{
    // TODO. See wxPostScriptPrinter::Print for hints.
    return FALSE;
}

wxDC* wxOS2Printer::PrintDialog(wxWindow *parent)
{
// TODO:
/*
    wxPrintDialog dialog(parent, & m_printData);
    return (dialog.GetPrintDC());
*/
    return NULL;
}

bool wxOS2Printer::Setup(wxWindow *parent)
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

wxOS2PrintPreview::wxOS2PrintPreview(wxPrintout *printout, wxPrintout *printoutForPrinting, wxPrintDialogData *data):
  wxPrintPreviewBase(printout, printoutForPrinting, data)
{
    DetermineScaling();
}

wxOS2PrintPreview::wxOS2PrintPreview(wxPrintout *printout, wxPrintout *printoutForPrinting, wxPrintData *data):
  wxPrintPreviewBase(printout, printoutForPrinting, data)
{
    DetermineScaling();
}

wxOS2PrintPreview::~wxOS2PrintPreview()
{
}

bool wxOS2PrintPreview::Print(bool interactive)
{
    if (!m_printPrintout)
        return FALSE;
//    wxOS2Printer printer(&m_printData);
//    return printer.Print(m_previewFrame, m_printPrintout, interactive);
    return FALSE;
}

void wxOS2PrintPreview::DetermineScaling()
{
    // TODO
}

#endif  //wxUSE_PRINTING_ARCHITECTURE

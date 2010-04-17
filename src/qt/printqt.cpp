/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/printqt.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/print.h"

wxQtPrinter::wxQtPrinter( wxPrintDialogData *data)
{
}

bool wxQtPrinter::Setup(wxWindow *parent)
{
    return false;
}

bool wxQtPrinter::Print(wxWindow *parent, wxPrintout *printout, bool prompt)
{
    return false;
}

wxDC* wxQtPrinter::PrintDialog(wxWindow *parent)
{
    return NULL;
}

//##############################################################################

wxQtPrintPreview::wxQtPrintPreview(wxPrintout *printout,
                      wxPrintout *printoutForPrinting,
                      wxPrintDialogData *data)
    : wxPrintPreviewBase( printout, printoutForPrinting, data )
{
}

wxQtPrintPreview::wxQtPrintPreview(wxPrintout *printout,
                      wxPrintout *printoutForPrinting,
                      wxPrintData *data)
    : wxPrintPreviewBase( printout, printoutForPrinting, data )
{
}

bool wxQtPrintPreview::Print(bool interactive)
{
    return false;
}

void wxQtPrintPreview::DetermineScaling()
{
}


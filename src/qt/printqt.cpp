/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/printqt.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_PRINTING_ARCHITECTURE

#include "wx/print.h"

wxQtPrinter::wxQtPrinter( wxPrintDialogData *WXUNUSED(data))
{
}

bool wxQtPrinter::Setup(wxWindow *WXUNUSED(parent))
{
    return false;
}

bool wxQtPrinter::Print(wxWindow *WXUNUSED(parent), wxPrintout *WXUNUSED(printout), bool WXUNUSED(prompt))
{
    return false;
}

wxDC* wxQtPrinter::PrintDialog(wxWindow *WXUNUSED(parent))
{
    return nullptr;
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

bool wxQtPrintPreview::Print(bool WXUNUSED(interactive))
{
    return false;
}

void wxQtPrintPreview::DetermineScaling()
{
}

#endif // wxUSE_PRINTING_ARCHITECTURE

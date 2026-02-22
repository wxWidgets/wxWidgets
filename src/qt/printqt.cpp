/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/printqt.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_PRINTING_ARCHITECTURE

#include "wx/log.h"
#include "wx/display.h"
#include "wx/print.h"
#include "wx/qt/dcprint.h"
#include "wx/qt/printdlg.h"
#include "wx/private/print.h"

#include <QPrinter>

wxIMPLEMENT_DYNAMIC_CLASS(wxQtPrinter, wxPrinterBase);

wxQtPrinter::wxQtPrinter(wxPrintDialogData* data)
    : wxPrinterBase(data)
{
}

bool wxQtPrinter::Setup(wxWindow* WXUNUSED(parent))
{
    return false;
}

bool wxQtPrinter::Print(wxWindow* parent, wxPrintout* printout, bool prompt)
{
    if ( !printout )
    {
        sm_lastError = wxPRINTER_ERROR;

        return false;
    }

    if ( m_printDialogData.GetMinPage() < 1 )
        m_printDialogData.SetMinPage(1);

    if ( m_printDialogData.GetMaxPage() < 1 )
        m_printDialogData.SetMaxPage(9999);

    // Create a suitable device context
    std::unique_ptr<wxPrinterDC> dc;

    if ( prompt )
        dc.reset(wxDynamicCast(PrintDialog(parent), wxPrinterDC));
    else
        dc.reset(new wxPrinterDC(m_printDialogData.GetPrintData()));

    // May have pressed cancel.
    if ( !dc || !dc->IsOk() )
    {
        return false;
    }

    // Set printout parameters
    if ( !printout->SetUp(*dc) )
    {
        sm_lastError = wxPRINTER_ERROR;

        return false;
    }

    wxBusyCursor busyCursor;

    printout->OnPreparePrinting();

    // Initialize page ranges with the value from the dialog, but then allow
    // the printout to customize them.
    std::vector<wxPrintPageRange> pageRanges;

    if ( m_printDialogData.GetSpecifiedPages() )
        pageRanges = m_printDialogData.GetPageRanges();

    const wxPrintPageRange allPages = printout->GetPagesInfo(pageRanges);

    if ( !allPages.IsValid() )
    {
        sm_lastError = wxPRINTER_ERROR;
        return false;
    }

    if ( pageRanges.empty() )
    {
        // Not having any ranges to print is equivalent to printing all pages.
        pageRanges.push_back(allPages);
    }

    // Only set min and max, because from and to have been
    // set by the user
    m_printDialogData.SetMinPage(allPages.fromPage);
    m_printDialogData.SetMaxPage(allPages.toPage);

    wxPrintingGuard guard(printout);

    sm_lastError = wxPRINTER_NO_ERROR;

    auto* const impl = static_cast<wxPrinterDCImpl*>(dc->GetImpl());
    auto* const qtPrinter = impl->GetQtPrinter();

    // The dc we get from the PrintDialog will do multiple copies without help
    // if the device supports it. Loop only if we have created a dc from our
    // own m_printDialogData or the device does not support multiple copies.
    // m_printDialogData.GetPrintData().GetNoCopies() is set from device
    // m_qtPrinter in printdlg.cpp/wxQtPrintNativeData::TransferTo()
    const int maxCopyCount = !prompt ||
                             !m_printDialogData.GetPrintData().GetNoCopies()
                             ? m_printDialogData.GetNoCopies() : 1;
    for ( int copyCount = 1; copyCount <= maxCopyCount; ++copyCount )
    {
        if ( !printout->OnBeginDocument(allPages.fromPage, allPages.toPage) )
        {
            QPrinter::PrinterState state = qtPrinter->printerState();

            if ( state == QPrinter::Aborted )
            {
                // No need to log an error if printing was cancelled by user.
                sm_lastError = wxPRINTER_CANCELLED;
            }
            else
            {
                sm_lastError = wxPRINTER_ERROR;

                wxLogError(_("Could not start printing."));
            }
            break;
        }

        int numPrinted = 0;

        for ( const wxPrintPageRange& range : pageRanges )
        {
            for ( int pn = range.fromPage; pn <= range.toPage; ++pn )
            {
                if ( !printout->HasPage(pn) )
                    continue;

                if ( ++numPrinted > 1 )
                {
                    if ( !qtPrinter->newPage() )
                    {
                        sm_lastError = wxPRINTER_ERROR;
                        break;
                    }
                }

                wxPrintingPageGuard pageGuard(*dc);

                if ( !printout->OnPrintPage(pn) )
                {
                    sm_lastError = wxPRINTER_CANCELLED;
                    break;
                }
            }
        }

        printout->OnEndDocument();
    }

    return sm_lastError == wxPRINTER_NO_ERROR;
}

wxDC* wxQtPrinter::PrintDialog(wxWindow* parent)
{
    wxDC* dc = nullptr;

    wxQtPrintDialog dialog(parent, &m_printDialogData);

    if ( dialog.ShowModal() == wxID_OK )
    {
        dc = dialog.GetPrintDC();
        m_printDialogData = dialog.GetPrintDialogData();

        sm_lastError = dc != nullptr ? wxPRINTER_NO_ERROR : wxPRINTER_ERROR;
    }
    else
        sm_lastError = wxPRINTER_CANCELLED;

    return dc;
}

//##############################################################################
wxIMPLEMENT_CLASS(wxQtPrintPreview, wxPrintPreviewBase);

wxQtPrintPreview::wxQtPrintPreview(wxPrintout* printout,
                                   wxPrintout* printoutForPrinting,
                                   wxPrintDialogData* data)
    : wxPrintPreviewBase(printout, printoutForPrinting, data)
{
    DetermineScaling();
}

wxQtPrintPreview::wxQtPrintPreview(wxPrintout* printout,
                                   wxPrintout* printoutForPrinting,
                                   wxPrintData* data)
    : wxPrintPreviewBase(printout, printoutForPrinting, data)
{
    DetermineScaling();
}

bool wxQtPrintPreview::Print(bool interactive)
{
    if ( !m_printPrintout )
        return false;

    wxQtPrinter printer(&m_printDialogData);
    return printer.Print(m_previewFrame, m_printPrintout, interactive);
}

void wxQtPrintPreview::DetermineScaling()
{
    const wxSize logPPIScreen = wxDisplay::GetStdPPI();
    m_previewPrintout->SetPPIScreen(logPPIScreen);

    // Get a device context for the currently selected printer
    wxPrinterDC printerDC(m_printDialogData.GetPrintData());

    int printerWidthMM;
    int printerHeightMM;
    int printerXRes;
    int printerYRes;
    wxSize logPPIPrinter;

    wxRect paperRect;

    if ( printerDC.IsOk() )
    {
        printerDC.GetSizeMM(&printerWidthMM, &printerHeightMM);
        printerDC.GetSize(&printerXRes, &printerYRes);

        logPPIPrinter = printerDC.GetPPI();

        paperRect = printerDC.GetPaperRect();

        if ( logPPIPrinter.x == 0 ||
                logPPIPrinter.y == 0 ||
                    printerWidthMM == 0 ||
                        printerHeightMM == 0 )
        {
            m_isOk = false;
        }
    }
    else
    {
        // use some defaults
        printerWidthMM = 150;
        printerHeightMM = 250;
        printerXRes = 1500;
        printerYRes = 2500;
        logPPIPrinter = wxSize(600, 600);

        paperRect = wxRect(0, 0, printerXRes, printerYRes);
    }

    m_pageWidth  = printerXRes;
    m_pageHeight = printerYRes;
    m_previewPrintout->SetPageSizePixels(printerXRes, printerYRes);
    m_previewPrintout->SetPageSizeMM(printerWidthMM, printerHeightMM);
    m_previewPrintout->SetPaperRectPixels(paperRect);
    m_previewPrintout->SetPPIPrinter(logPPIPrinter);

    // At 100%, the page should look about page-size on the screen.
    m_previewScaleX = float(logPPIScreen.x) / logPPIPrinter.x;
    m_previewScaleY = float(logPPIScreen.y) / logPPIPrinter.y;
}

#endif // wxUSE_PRINTING_ARCHITECTURE

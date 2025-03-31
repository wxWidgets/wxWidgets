/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/printwin.cpp
// Purpose:     wxWindowsPrinter framework
// Author:      Julian Smart
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


// Don't use the Windows printer if we're in wxUniv mode and using
// the PostScript architecture
#if wxUSE_PRINTING_ARCHITECTURE && (!defined(__WXUNIVERSAL__) || !wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW)

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/msw/private.h"
    #include "wx/utils.h"
    #include "wx/dc.h"
    #include "wx/app.h"
    #include "wx/msgdlg.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/dcprint.h"
    #include "wx/dcmemory.h"
    #include "wx/image.h"
#endif

#include "wx/msw/dib.h"
#include "wx/msw/dcmemory.h"
#include "wx/msw/printwin.h"
#include "wx/msw/printdlg.h"
#include "wx/msw/private.h"
#include "wx/msw/dcprint.h"
#include "wx/msw/enhmeta.h"
#include "wx/display.h"

#include "wx/private/print.h"

#include <stdlib.h>

// This variable is defined in src/msw/dcprint.cpp.
extern bool wxPrinterOperationCancelled;

// ---------------------------------------------------------------------------
// private functions
// ---------------------------------------------------------------------------

BOOL CALLBACK wxAbortProc(HDC hdc, int error);

// ---------------------------------------------------------------------------
// wxWin macros
// ---------------------------------------------------------------------------

    wxIMPLEMENT_DYNAMIC_CLASS(wxWindowsPrinter, wxPrinterBase);
    wxIMPLEMENT_CLASS(wxWindowsPrintPreview, wxPrintPreviewBase);

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// Printer
// ---------------------------------------------------------------------------

wxWindowsPrinter::wxWindowsPrinter(wxPrintDialogData *data)
                : wxPrinterBase(data)
{
}

bool wxWindowsPrinter::Print(wxWindow *parent, wxPrintout *printout, bool prompt)
{
    sm_abortIt = false;
    sm_abortWindow = nullptr;

    if (!printout)
    {
        sm_lastError = wxPRINTER_ERROR;
        return false;
    }

    // Small helper ensuring that we destroy sm_abortWindow if it was created.
    class AbortWindowCloser
    {
    public:
        AbortWindowCloser() = default;

        void Initialize(wxWindow* win)
        {
            wxPrinterBase::sm_abortWindow = win;
            win->Show();
            wxSafeYield();
        }

        ~AbortWindowCloser()
        {
            if ( wxPrinterBase::sm_abortWindow )
            {
                wxPrinterBase::sm_abortWindow->Show(false);
                wxDELETE(wxPrinterBase::sm_abortWindow);
            }
        }

        AbortWindowCloser(const AbortWindowCloser&) = delete;
        AbortWindowCloser& operator=(const AbortWindowCloser&) = delete;
    } abortWindowCloser;

    if (m_printDialogData.GetMinPage() < 1)
        m_printDialogData.SetMinPage(1);
    if (m_printDialogData.GetMaxPage() < 1)
        m_printDialogData.SetMaxPage(9999);

    // Create a suitable device context
    std::unique_ptr<wxPrinterDC> dc;
    if (prompt)
    {
        dc.reset(wxDynamicCast(PrintDialog(parent), wxPrinterDC));
        if (!dc)
            return false;
    }
    else
    {
        dc.reset(new wxPrinterDC(m_printDialogData.GetPrintData()));
    }

    // May have pressed cancel.
    if (!dc || !dc->IsOk())
    {
        return false;
    }

    // Set printout parameters
    if (!printout->SetUp(*dc))
    {
        sm_lastError = wxPRINTER_ERROR;
        return false;
    }

    // Create an abort window
    wxBusyCursor busyCursor;

    printout->OnPreparePrinting();

    // Initialize page ranges with the value from the dialog, but then allow
    // the printout to customize them.
    std::vector<wxPrintPageRange> pageRanges;
    if ( m_printDialogData.GetSpecifiedPages() )
        pageRanges = m_printDialogData.GetPageRanges();
    const wxPrintPageRange allPages = printout->GetPagesInfo(pageRanges);

    if (!allPages.IsValid())
    {
        sm_lastError = wxPRINTER_ERROR;
        return false;
    }

    if (pageRanges.empty())
    {
        // Not having any ranges to print is equivalent to printing all pages.
        pageRanges.push_back(allPages);
    }

    // Only set min and max, because from and to have been
    // set by the user
    m_printDialogData.SetMinPage(allPages.fromPage);
    m_printDialogData.SetMaxPage(allPages.toPage);

    wxPrintAbortDialog *win = CreateAbortWindow(parent, printout);
    wxYield();

    ::SetAbortProc(GetHdcOf(*dc), wxAbortProc);

    if (!win)
    {
        wxLogDebug(wxT("Could not create an abort dialog."));
        sm_lastError = wxPRINTER_ERROR;

        return false;
    }

    abortWindowCloser.Initialize(win);

    wxPrintingGuard guard(printout);

    sm_lastError = wxPRINTER_NO_ERROR;

    // calculate total number of pages to print
    int numToPrint = 0;
    for (const wxPrintPageRange& range : pageRanges)
    {
        for (int pn = range.fromPage; pn <= range.toPage; pn++)
        {
            if (printout->HasPage(pn))
                numToPrint++;
        }
    }

    // The dc we get from the PrintDialog will do multiple copies without help
    // if the device supports it. Loop only if we have created a dc from our
    // own m_printDialogData or the device does not support multiple copies.
    // m_printDialogData.GetPrintData().GetNoCopies() is set from device
    // devMode in printdlg.cpp/wxWindowsPrintDialog::ConvertFromNative()
    const int maxCopyCount = !prompt ||
                             !m_printDialogData.GetPrintData().GetNoCopies()
                             ? m_printDialogData.GetNoCopies() : 1;
    for ( int copyCount = 1; copyCount <= maxCopyCount; copyCount++ )
    {
        // We have no good way to return the fact that starting printing was
        // cancelled, as can be the case e.g. when showing a dialog when
        // printing to file, from wxPrinterDCImpl::StartDoc() which is called
        // by OnBeginDocument() by default, so pass this information out of
        // band using this global variable.
        wxPrinterOperationCancelled = false;

        if ( !printout->OnBeginDocument(allPages.fromPage, allPages.toPage) )
        {
            if ( wxPrinterOperationCancelled )
            {
                // No need to log an error if printing was cancelled by user.
                sm_lastError = wxPRINTER_CANCELLED;
            }
            else
            {
                wxLogError(_("Could not start printing."));
                sm_lastError = wxPRINTER_ERROR;
            }
            break;
        }
        if (sm_abortIt)
        {
            sm_lastError = wxPRINTER_CANCELLED;
            break;
        }

        int numPrinted = 0;

        for (const wxPrintPageRange& range : pageRanges)
        {
            for (int pn = range.fromPage; pn <= range.toPage; pn++)
            {
                if ( !printout->HasPage(pn) )
                    continue;

                win->SetProgress(++numPrinted, numToPrint,
                                 copyCount, maxCopyCount);

                if ( sm_abortIt )
                {
                    sm_lastError = wxPRINTER_CANCELLED;
                    break;
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

wxDC *wxWindowsPrinter::PrintDialog(wxWindow *parent)
{
    wxDC *dc = nullptr;

    wxWindowsPrintDialog dialog(parent, & m_printDialogData);
    int ret = dialog.ShowModal();

    if (ret == wxID_OK)
    {
        dc = dialog.GetPrintDC();
        m_printDialogData = dialog.GetPrintDialogData();
        if (dc == nullptr)
            sm_lastError = wxPRINTER_ERROR;
        else
            sm_lastError = wxPRINTER_NO_ERROR;
    }
    else
        sm_lastError = wxPRINTER_CANCELLED;

    return dc;
}

bool wxWindowsPrinter::Setup(wxWindow *WXUNUSED(parent))
{
#if 0
    // We no longer expose that dialog
    wxPrintDialog dialog(parent, & m_printDialogData);
    dialog.GetPrintDialogData().SetSetupDialog(true);

    int ret = dialog.ShowModal();

    if (ret == wxID_OK)
    {
        m_printDialogData = dialog.GetPrintDialogData();
    }

    return (ret == wxID_OK);
#else
    return false;
#endif
}

/*
* Print preview
*/

wxWindowsPrintPreview::wxWindowsPrintPreview(wxPrintout *printout,
                                             wxPrintout *printoutForPrinting,
                                             wxPrintDialogData *data)
                     : wxPrintPreviewBase(printout, printoutForPrinting, data)
{
    DetermineScaling();
}

wxWindowsPrintPreview::wxWindowsPrintPreview(wxPrintout *printout,
                                             wxPrintout *printoutForPrinting,
                                             wxPrintData *data)
                     : wxPrintPreviewBase(printout, printoutForPrinting, data)
{
    DetermineScaling();
}

wxWindowsPrintPreview::~wxWindowsPrintPreview()
{
}

bool wxWindowsPrintPreview::Print(bool interactive)
{
    if (!m_printPrintout)
        return false;
    wxWindowsPrinter printer(&m_printDialogData);
    return printer.Print(m_previewFrame, m_printPrintout, interactive);
}

void wxWindowsPrintPreview::DetermineScaling()
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
        wxPrinterDCImpl *impl = (wxPrinterDCImpl*) printerDC.GetImpl();
        HDC hdc = GetHdcOf(*impl);
        printerWidthMM = ::GetDeviceCaps(hdc, HORZSIZE);
        printerHeightMM = ::GetDeviceCaps(hdc, VERTSIZE);
        printerXRes = ::GetDeviceCaps(hdc, HORZRES);
        printerYRes = ::GetDeviceCaps(hdc, VERTRES);
        logPPIPrinter = wxGetDPIofHDC(hdc);

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
        m_isOk = false;
    }
    m_pageWidth = printerXRes;
    m_pageHeight = printerYRes;
    m_previewPrintout->SetPageSizePixels(printerXRes, printerYRes);
    m_previewPrintout->SetPageSizeMM(printerWidthMM, printerHeightMM);
    m_previewPrintout->SetPaperRectPixels(paperRect);
    m_previewPrintout->SetPPIPrinter(logPPIPrinter);

    // At 100%, the page should look about page-size on the screen.
    m_previewScaleX = float(logPPIScreen.x) / logPPIPrinter.x;
    m_previewScaleY = float(logPPIScreen.y) / logPPIPrinter.y;
}

#if wxUSE_ENH_METAFILE
bool wxWindowsPrintPreview::RenderPageIntoBitmap(wxBitmap& bmp, int pageNum)
{
    // The preview, as implemented in wxPrintPreviewBase (and as used prior to
    // wx3) is inexact: it uses screen DC, which has much lower resolution and
    // has other properties different from printer DC, so the preview is not
    // quite right.
    //
    // To make matters worse, if the application depends heavily on
    // GetTextExtent() or does text layout itself, the output in preview and on
    // paper can be very different. In particular, wxHtmlEasyPrinting is
    // affected and the preview can be easily off by several pages.
    //
    // To fix this, we render the preview into high-resolution enhanced
    // metafile with properties identical to the printer DC. This guarantees
    // metrics correctness while still being fast.


    // print the preview into a metafile:
    wxPrinterDC printerDC(m_printDialogData.GetPrintData());
    wxEnhMetaFileDC metaDC(printerDC,
                           wxEmptyString,
                           printerDC.GetSize().x, printerDC.GetSize().y);

    if ( !RenderPageIntoDC(metaDC, pageNum) )
        return false;

    wxEnhMetaFile *metafile = metaDC.Close();
    if ( !metafile )
        return false;

    // now render the metafile:
    wxMemoryDC bmpDC;
    bmpDC.SelectObject(bmp);
    bmpDC.Clear();

    wxRect outRect(0, 0, bmp.GetWidth(), bmp.GetHeight());
    metafile->Play(&bmpDC, &outRect);


    delete metafile;

    // TODO: we should keep the metafile and reuse it when changing zoom level

    return true;
}
#endif // wxUSE_ENH_METAFILE

BOOL CALLBACK wxAbortProc(HDC WXUNUSED(hdc), int WXUNUSED(error))
{
    MSG msg;

    if (!wxPrinterBase::sm_abortWindow)              /* If the abort dialog isn't up yet */
        return(TRUE);

    /* Process messages intended for the abort dialog box */
    const HWND hwnd = GetHwndOf(wxPrinterBase::sm_abortWindow);

    while (!wxPrinterBase::sm_abortIt && ::PeekMessage(&msg, 0, 0, 0, TRUE))
    {
        // Apparently handling the message may, somehow, result in
        // sm_abortWindow being destroyed, so guard against this happening.
        if (!wxPrinterBase::sm_abortWindow)
        {
            wxLogDebug(wxS("Print abort dialog unexpected disappeared."));
            return TRUE;
        }

        if (!IsDialogMessage(hwnd, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    /* bAbort is TRUE (return is FALSE) if the user has aborted */

    return !wxPrinterBase::sm_abortIt;
}

#endif
    // wxUSE_PRINTING_ARCHITECTURE

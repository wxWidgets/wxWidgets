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
#include "wx/scopeguard.h"
#include "wx/weakref.h"

#include "wx/private/print.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "wx/winui/private/tlwhostmsw.h"
#endif

#include <stdlib.h>

namespace
{

// The abort procedure is called by GDI while arbitrary application events can
// be dispatched. In particular, those events can destroy the progress dialog
// (directly or together with its owner), so the legacy public raw pointer must
// never be used as a lifetime guard.
wxWeakRef<wxWindow> gs_abortWindow;
wxWeakRef<wxWindow> gs_printOwner;
bool gs_abortSessionActive = false;
bool gs_printJobHadOwner = false;

bool IsWindowUnavailable(wxWindow* window)
{
    if ( !window || window->IsBeingDeleted() ||
         (wxTheApp &&
          wxTheApp->IsScheduledForDestruction(window)) )
    {
        return true;
    }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( wxWinUITLWHostIsDestroyScheduled(window) )
        return true;
#endif

    return false;
}

} // anonymous namespace

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
    const bool hadParent = parent != nullptr;
    const wxWeakRef<wxWindow> parentLifetime(parent);
    const auto isParentUnavailable =
        [&parentLifetime, hadParent]()
        {
            wxWindow* const liveParent = parentLifetime.get();
            return hadParent &&
                   IsWindowUnavailable(liveParent);
        };

    if ( sm_printJobActive )
    {
        wxLogWarning(_("A print job is already active."));
        return false;
    }

    sm_printJobActive = true;
    wxScopeGuard clearActiveJob =
        wxMakeGuard([]() { sm_printJobActive = false; });
    wxUnusedVar(clearActiveJob);

    gs_printOwner = parentLifetime;
    gs_printJobHadOwner = hadParent;
    wxScopeGuard clearPrintOwner =
        wxMakeGuard(
            []()
            {
                gs_printOwner.Release();
                gs_printJobHadOwner = false;
            });
    wxUnusedVar(clearPrintOwner);

    sm_abortIt = false;
    sm_abortWindow = nullptr;
    gs_abortWindow.Release();
    gs_abortSessionActive = false;
    sm_lastError = wxPRINTER_NO_ERROR;

    if (!printout)
    {
        sm_lastError = wxPRINTER_ERROR;
        return false;
    }

    // Ensure that the abort window is destroyed if we still own a live one,
    // while tolerating it being destroyed from a callback dispatched by GDI.
    class AbortWindowCloser
    {
    public:
        AbortWindowCloser() = default;

        bool Initialize(wxWindow* win)
        {
            m_window = wxWeakRef<wxWindow>(win);
            gs_abortWindow = m_window;
            wxPrinterBase::sm_abortWindow = win;
            gs_abortSessionActive = true;
            win->Bind(
                wxEVT_DESTROY,
                [](wxWindowDestroyEvent& event)
                {
                    wxWindow* const current = gs_abortWindow.get();
                    if ( gs_abortSessionActive &&
                         event.GetEventObject() == current )
                    {
                        wxPrinterBase::sm_abortWindow = nullptr;
                        wxPrinterBase::sm_abortIt = true;
                    }
                    event.Skip();
                });
            win->Show();
            wxSafeYield();

            return Get() != nullptr;
        }

        wxPrintAbortDialog* Get() const
        {
            wxWindow* const win = m_window.get();
            if ( !gs_abortSessionActive ||
                 wxPrinterBase::sm_abortWindow != win ||
                 IsWindowUnavailable(win) ||
                 (gs_printJobHadOwner &&
                  IsWindowUnavailable(gs_printOwner.get())) )
                return nullptr;

            // m_window is initialized exclusively from the
            // wxPrintAbortDialog* returned by CreateAbortWindow().
            return static_cast<wxPrintAbortDialog*>(win);
        }

        ~AbortWindowCloser()
        {
            wxWindow* win = m_window.get();

            // Close the shared session before hiding/deleting the window:
            // either action can dispatch events and re-enter wxAbortProc().
            gs_abortSessionActive = false;
            wxPrinterBase::sm_abortWindow = nullptr;
            gs_abortWindow.Release();

            // OnCancel() or destruction of the owner can already have queued
            // this TLW in either the regular pending-delete list or WinUI's
            // private retained-callback queue. Never delete it a second time.
            if ( !IsWindowUnavailable(win) )
            {
                win->Show(false);
                win = m_window.get();
                if ( !IsWindowUnavailable(win) )
                    delete win;
            }

            m_window.Release();
        }

        AbortWindowCloser(const AbortWindowCloser&) = delete;
        AbortWindowCloser& operator=(const AbortWindowCloser&) = delete;

    private:
        wxWeakRef<wxWindow> m_window;
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
        if ( sm_lastError == wxPRINTER_NO_ERROR )
            sm_lastError = wxPRINTER_ERROR;
        return false;
    }

    // Set printout parameters
    if (!printout->SetUp(*dc))
    {
        sm_lastError = wxPRINTER_ERROR;
        return false;
    }
    wxScopeGuard clearPrintoutDC =
        wxMakeGuard([printout]() { printout->SetDC(nullptr); });
    wxUnusedVar(clearPrintoutDC);
    wxPrinterDCImpl* const printerDCImpl =
        wxDynamicCast(dc->GetImpl(), wxPrinterDCImpl);

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

    // The print dialog and the printout callbacks above may have yielded.
    // Never pass a stale owner to the abort dialog constructor.
    if ( isParentUnavailable() )
    {
        sm_abortIt = true;
        sm_lastError = wxPRINTER_CANCELLED;
        return false;
    }

    wxPrintAbortDialog *win =
        CreateAbortWindow(parentLifetime.get(), printout);
    if ( isParentUnavailable() )
    {
        // CreateAbortWindow() is virtual and may dispatch application code.
        // If its owner entered destruction, do not even attach a tracker to
        // the returned pointer: an override could already have invalidated it.
        sm_abortIt = true;
        sm_lastError = wxPRINTER_CANCELLED;
        return false;
    }

    if (!win)
    {
        wxLogDebug(wxT("Could not create an abort dialog."));
        sm_lastError = wxPRINTER_ERROR;

        return false;
    }

    if ( !abortWindowCloser.Initialize(win) || isParentUnavailable() )
    {
        // Showing the progress window dispatches events and may destroy it or
        // its owner. Treat this as cancellation instead of retaining a
        // dangling raw pointer.
        sm_abortWindow = nullptr;
        sm_abortIt = true;
        sm_lastError = wxPRINTER_CANCELLED;
        return false;
    }

    ::SetAbortProc(GetHdcOf(*dc), wxAbortProc);

    sm_lastError = wxPRINTER_NO_ERROR;

    {
        wxPrintingGuard guard(printout);

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

        // The dc we get from the PrintDialog will do multiple copies without
        // help if the device supports it. Loop only if we have created a dc
        // from our own data or the device does not support multiple copies.
        const int maxCopyCount = wxMax(
            1,
            !prompt ||
            !m_printDialogData.GetPrintData().GetNoCopies()
                ? m_printDialogData.GetNoCopies()
                : 1);
        bool stopPrinting = false;
        for ( int copyCount = 1;
              copyCount <= maxCopyCount && !stopPrinting;
              copyCount++ )
        {
            if ( sm_abortIt )
            {
                sm_lastError = wxPRINTER_CANCELLED;
                stopPrinting = true;
                break;
            }

            if ( !printout->OnBeginDocument(
                     allPages.fromPage, allPages.toPage) )
            {
                if ( sm_abortIt ||
                     (printerDCImpl &&
                      printerDCImpl->WasLastStartDocCancelled()) )
                {
                    sm_lastError = wxPRINTER_CANCELLED;
                }
                else
                {
                    wxLogError(_("Could not start printing."));
                    sm_lastError = wxPRINTER_ERROR;
                }
                stopPrinting = true;
                break;
            }

            {
                wxScopeGuard endDocument =
                    wxMakeGuard(
                        [printout]() { printout->OnEndDocument(); });
                wxUnusedVar(endDocument);

                if (sm_abortIt)
                {
                    sm_lastError = wxPRINTER_CANCELLED;
                    stopPrinting = true;
                }

                int numPrinted = 0;
                for (const wxPrintPageRange& range : pageRanges)
                {
                    if ( stopPrinting )
                        break;

                    for (int pn = range.fromPage;
                         pn <= range.toPage;
                         pn++)
                    {
                        if ( !printout->HasPage(pn) )
                            continue;

                        wxPrintAbortDialog* const liveAbortWindow =
                            abortWindowCloser.Get();
                        if ( !liveAbortWindow || isParentUnavailable() )
                        {
                            sm_abortWindow = nullptr;
                            sm_abortIt = true;
                            sm_lastError = wxPRINTER_CANCELLED;
                            stopPrinting = true;
                            break;
                        }

                        liveAbortWindow->SetProgress(
                            ++numPrinted, numToPrint,
                            copyCount, maxCopyCount);

                        if ( sm_abortIt )
                        {
                            sm_lastError = wxPRINTER_CANCELLED;
                            stopPrinting = true;
                            break;
                        }

                        {
                            dc->StartPage();
                            if ( printerDCImpl &&
                                 !printerDCImpl->
                                    WasLastStartPageSuccessful() )
                            {
                                if ( sm_abortIt )
                                {
                                    sm_lastError = wxPRINTER_CANCELLED;
                                }
                                else if ( sm_lastError ==
                                          wxPRINTER_NO_ERROR )
                                {
                                    sm_lastError = wxPRINTER_ERROR;
                                }
                                stopPrinting = true;
                                break;
                            }

                            wxDC* const pageDC = dc.get();
                            wxScopeGuard endPage =
                                wxMakeGuard(
                                    [pageDC]()
                                    {
                                        pageDC->EndPage();
                                    });
                            wxUnusedVar(endPage);

                            if ( !printout->OnPrintPage(pn) )
                            {
                                sm_lastError = wxPRINTER_CANCELLED;
                                stopPrinting = true;
                            }

                            if ( sm_abortIt )
                            {
                                sm_lastError = wxPRINTER_CANCELLED;
                                stopPrinting = true;
                            }

                            if ( !abortWindowCloser.Get() ||
                                 isParentUnavailable() )
                            {
                                sm_abortWindow = nullptr;
                                sm_abortIt = true;
                                sm_lastError = wxPRINTER_CANCELLED;
                                stopPrinting = true;
                            }
                        }

                        if ( printerDCImpl &&
                             !printerDCImpl->
                                WasLastEndPageSuccessful() )
                        {
                            if ( sm_abortIt )
                            {
                                sm_lastError = wxPRINTER_CANCELLED;
                            }
                            else if ( sm_lastError ==
                                      wxPRINTER_NO_ERROR )
                            {
                                sm_lastError = wxPRINTER_ERROR;
                            }
                            stopPrinting = true;
                        }

                        if ( stopPrinting )
                            break;
                    }
                }
            } // OnEndDocument()

            if ( printerDCImpl &&
                 !printerDCImpl->WasLastEndDocSuccessful() )
            {
                if ( sm_abortIt )
                    sm_lastError = wxPRINTER_CANCELLED;
                else if ( sm_lastError == wxPRINTER_NO_ERROR )
                    sm_lastError = wxPRINTER_ERROR;
                stopPrinting = true;
            }

            if ( isParentUnavailable() &&
                 sm_lastError == wxPRINTER_NO_ERROR )
            {
                sm_abortWindow = nullptr;
                sm_abortIt = true;
                sm_lastError = wxPRINTER_CANCELLED;
                stopPrinting = true;
            }

            if ( sm_abortIt &&
                 sm_lastError == wxPRINTER_NO_ERROR )
            {
                sm_lastError = wxPRINTER_CANCELLED;
                stopPrinting = true;
            }
        }
    } // OnEndPrinting()

    if ( (sm_abortIt || isParentUnavailable()) &&
         sm_lastError == wxPRINTER_NO_ERROR )
    {
        sm_abortIt = true;
        sm_lastError = wxPRINTER_CANCELLED;
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

    if ( !gs_abortSessionActive )
        return TRUE;

    const auto resolveAbortHwnd =
        []() -> HWND
        {
            wxWindow* const abortWindow = gs_abortWindow.get();
            if ( IsWindowUnavailable(abortWindow) ||
                 (gs_printJobHadOwner &&
                  IsWindowUnavailable(gs_printOwner.get())) ||
                 wxPrinterBase::sm_abortWindow != abortWindow )
            {
                return nullptr;
            }

            const HWND hwnd = GetHwndOf(abortWindow);
            return hwnd && ::IsWindow(hwnd) ? hwnd : nullptr;
        };
    const auto failClosed =
        []() -> BOOL
        {
            wxPrinterBase::sm_abortWindow = nullptr;
            wxPrinterBase::sm_abortIt = true;
            return FALSE;
        };

    if ( !resolveAbortHwnd() )
    {
        wxLogDebug(wxS("Print abort dialog unexpectedly disappeared."));
        return failClosed();
    }

    while (!wxPrinterBase::sm_abortIt && ::PeekMessage(&msg, 0, 0, 0, TRUE))
    {
        // Resolve the HWND for this dispatch only. IsDialogMessage() and
        // DispatchMessage() can both destroy the dialog or its owner.
        const HWND hwnd = resolveAbortHwnd();
        if ( !hwnd )
        {
            wxLogDebug(wxS("Print abort dialog unexpectedly disappeared."));
            return failClosed();
        }

        if (!IsDialogMessage(hwnd, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if ( !resolveAbortHwnd() )
        {
            wxLogDebug(wxS("Print abort dialog unexpectedly disappeared."));
            return failClosed();
        }
    }

    /* bAbort is TRUE (return is FALSE) if the user has aborted */
    if ( !wxPrinterBase::sm_abortIt && !resolveAbortHwnd() )
    {
        wxLogDebug(wxS("Print abort dialog unexpectedly disappeared."));
        return failClosed();
    }

    return !wxPrinterBase::sm_abortIt;
}

#if defined(__WXWINUI__) && wxUSE_WINUI3
bool wxMSWInvokePrintAbortProcForTesting()
{
    return wxAbortProc(nullptr, 0) != FALSE;
}
#endif

#endif
    // wxUSE_PRINTING_ARCHITECTURE

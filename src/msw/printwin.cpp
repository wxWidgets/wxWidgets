/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/printwin.cpp
// Purpose:     wxWindowsPrinter framework
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// Don't use the Windows printer if we're in wxUniv mode and using
// the PostScript architecture
#if wxUSE_PRINTING_ARCHITECTURE && (!defined(__WXUNIVERSAL__) || !wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW)

#ifndef WX_PRECOMP
    #include "wx/msw/wrapcdlg.h"
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

#include <stdlib.h>

#ifndef __WIN32__
    #include <print.h>
#endif

// ---------------------------------------------------------------------------
// private functions
// ---------------------------------------------------------------------------

LONG APIENTRY _EXPORT wxAbortProc(HDC hPr, int Code);

// ---------------------------------------------------------------------------
// wxWin macros
// ---------------------------------------------------------------------------

    IMPLEMENT_DYNAMIC_CLASS(wxWindowsPrinter, wxPrinterBase)
    IMPLEMENT_CLASS(wxWindowsPrintPreview, wxPrintPreviewBase)

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// Printer
// ---------------------------------------------------------------------------

wxWindowsPrinter::wxWindowsPrinter(wxPrintDialogData *data)
                : wxPrinterBase(data)
{
    m_lpAbortProc = (WXFARPROC)wxAbortProc;
}

wxWindowsPrinter::~wxWindowsPrinter()
{
    // avoids warning about statement with no effect (FreeProcInstance
    // doesn't do anything under Win32)
#if !defined(__WIN32__) && !defined(__NT__)
    FreeProcInstance((FARPROC) m_lpAbortProc);
#endif
}

bool wxWindowsPrinter::Print(wxWindow *parent, wxPrintout *printout, bool prompt)
{
    sm_abortIt = false;
    sm_abortWindow = NULL;

    if (!printout)
    {
        sm_lastError = wxPRINTER_ERROR;
        return false;
    }

    printout->SetIsPreview(false);

    if (m_printDialogData.GetMinPage() < 1)
        m_printDialogData.SetMinPage(1);
    if (m_printDialogData.GetMaxPage() < 1)
        m_printDialogData.SetMaxPage(9999);

    // Create a suitable device context
    wxPrinterDC *dc wxDUMMY_INITIALIZE(NULL);
    if (prompt)
    {
        dc = wxDynamicCast(PrintDialog(parent), wxPrinterDC);
        if (!dc)
            return false;
    }
    else
    {
        dc = new wxPrinterDC(m_printDialogData.GetPrintData());
    }

    // May have pressed cancel.
    if (!dc || !dc->IsOk())
    {
        if (dc) delete dc;
        return false;
    }

    wxPrinterDCImpl *impl = (wxPrinterDCImpl*) dc->GetImpl();

    HDC hdc = ::GetDC(NULL);
    int logPPIScreenX = ::GetDeviceCaps(hdc, LOGPIXELSX);
    int logPPIScreenY = ::GetDeviceCaps(hdc, LOGPIXELSY);
    ::ReleaseDC(NULL, hdc);

    int logPPIPrinterX = ::GetDeviceCaps((HDC) impl->GetHDC(), LOGPIXELSX);
    int logPPIPrinterY = ::GetDeviceCaps((HDC) impl->GetHDC(), LOGPIXELSY);
    if (logPPIPrinterX == 0 || logPPIPrinterY == 0)
    {
        delete dc;
        sm_lastError = wxPRINTER_ERROR;
        return false;
    }

    printout->SetPPIScreen(logPPIScreenX, logPPIScreenY);
    printout->SetPPIPrinter(logPPIPrinterX, logPPIPrinterY);

    // Set printout parameters
    printout->SetDC(dc);

    int w, h;
    dc->GetSize(&w, &h);
    printout->SetPageSizePixels((int)w, (int)h);
    printout->SetPaperRectPixels(dc->GetPaperRect());

    dc->GetSizeMM(&w, &h);
    printout->SetPageSizeMM((int)w, (int)h);

    // Create an abort window
    wxBusyCursor busyCursor;

    printout->OnPreparePrinting();

    // Get some parameters from the printout, if defined
    int fromPage, toPage;
    int minPage, maxPage;
    printout->GetPageInfo(&minPage, &maxPage, &fromPage, &toPage);

    if (maxPage == 0)
    {
        sm_lastError = wxPRINTER_ERROR;
        return false;
    }

    // Only set min and max, because from and to have been
    // set by the user
    m_printDialogData.SetMinPage(minPage);
    m_printDialogData.SetMaxPage(maxPage);

    wxWindow *win = CreateAbortWindow(parent, printout);
    wxYield();

#if defined(__WATCOMC__) || defined(__BORLANDC__) || defined(__GNUWIN32__) || !defined(__WIN32__)
#ifdef STRICT
    ::SetAbortProc((HDC) impl->GetHDC(), (ABORTPROC) m_lpAbortProc);
#else
    ::SetAbortProc((HDC) impl->GetHDC(), (FARPROC) m_lpAbortProc);
#endif
#else
    ::SetAbortProc((HDC) impl->GetHDC(), (int (_stdcall *)
        // cast it to right type only if required
        // FIXME it's really cdecl and we're casting it to stdcall - either there is
        //       something I don't understand or it will crash at first usage
#ifdef STRICT
        (HDC, int)
#else
        ()
#endif
        )m_lpAbortProc);
#endif

    if (!win)
    {
        wxLogDebug(wxT("Could not create an abort dialog."));
        sm_lastError = wxPRINTER_ERROR;

        delete dc;
        return false;
    }
    sm_abortWindow = win;
    sm_abortWindow->Show();
    wxSafeYield();

    printout->OnBeginPrinting();

    sm_lastError = wxPRINTER_NO_ERROR;

    int minPageNum = minPage, maxPageNum = maxPage;

    if ( !m_printDialogData.GetAllPages() )
    {
        minPageNum = m_printDialogData.GetFromPage();
        maxPageNum = m_printDialogData.GetToPage();
    }

    int copyCount;
    for ( copyCount = 1;
          copyCount <= m_printDialogData.GetNoCopies();
          copyCount++ )
    {
        if ( !printout->OnBeginDocument(minPageNum, maxPageNum) )
        {
            wxLogError(_("Could not start printing."));
            sm_lastError = wxPRINTER_ERROR;
            break;
        }
        if (sm_abortIt)
        {
            sm_lastError = wxPRINTER_CANCELLED;
            break;
        }

        int pn;

        for ( pn = minPageNum;
              pn <= maxPageNum && printout->HasPage(pn);
              pn++ )
        {
            if ( sm_abortIt )
            {
                sm_lastError = wxPRINTER_CANCELLED;
                break;
            }

            dc->StartPage();
            bool cont = printout->OnPrintPage(pn);
            dc->EndPage();

            if ( !cont )
            {
                sm_lastError = wxPRINTER_CANCELLED;
                break;
            }
        }

        printout->OnEndDocument();
    }

    printout->OnEndPrinting();

    if (sm_abortWindow)
    {
        sm_abortWindow->Show(false);
        delete sm_abortWindow;
        sm_abortWindow = NULL;
    }

    delete dc;

    return sm_lastError == wxPRINTER_NO_ERROR;
}

wxDC *wxWindowsPrinter::PrintDialog(wxWindow *parent)
{
    wxDC *dc = NULL;

    wxWindowsPrintDialog dialog(parent, & m_printDialogData);
    int ret = dialog.ShowModal();

    if (ret == wxID_OK)
    {
        dc = dialog.GetPrintDC();
        m_printDialogData = dialog.GetPrintDialogData();
        if (dc == NULL)
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
#if wxUSE_HIGH_QUALITY_PREVIEW
    m_hqPreviewFailed = false;
#endif
    DetermineScaling();
}

wxWindowsPrintPreview::wxWindowsPrintPreview(wxPrintout *printout,
                                             wxPrintout *printoutForPrinting,
                                             wxPrintData *data)
                     : wxPrintPreviewBase(printout, printoutForPrinting, data)
{
#if wxUSE_HIGH_QUALITY_PREVIEW
    m_hqPreviewFailed = false;
#endif
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
    ScreenHDC dc;
    int logPPIScreenX = ::GetDeviceCaps(dc, LOGPIXELSX);
    int logPPIScreenY = ::GetDeviceCaps(dc, LOGPIXELSY);
    m_previewPrintout->SetPPIScreen(logPPIScreenX, logPPIScreenY);

    // Get a device context for the currently selected printer
    wxPrinterDC printerDC(m_printDialogData.GetPrintData());

    int printerWidthMM;
    int printerHeightMM;
    int printerXRes;
    int printerYRes;
    int logPPIPrinterX;
    int logPPIPrinterY;

    wxRect paperRect;

    if ( printerDC.IsOk() )
    {
        wxPrinterDCImpl *impl = (wxPrinterDCImpl*) printerDC.GetImpl();
        HDC dc = GetHdcOf(*impl);
        printerWidthMM = ::GetDeviceCaps(dc, HORZSIZE);
        printerHeightMM = ::GetDeviceCaps(dc, VERTSIZE);
        printerXRes = ::GetDeviceCaps(dc, HORZRES);
        printerYRes = ::GetDeviceCaps(dc, VERTRES);
        logPPIPrinterX = ::GetDeviceCaps(dc, LOGPIXELSX);
        logPPIPrinterY = ::GetDeviceCaps(dc, LOGPIXELSY);

        paperRect = printerDC.GetPaperRect();

        if ( logPPIPrinterX == 0 ||
                logPPIPrinterY == 0 ||
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
        logPPIPrinterX = 600;
        logPPIPrinterY = 600;

        paperRect = wxRect(0, 0, printerXRes, printerYRes);
        m_isOk = false;
    }
    m_pageWidth = printerXRes;
    m_pageHeight = printerYRes;
    m_previewPrintout->SetPageSizePixels(printerXRes, printerYRes);
    m_previewPrintout->SetPageSizeMM(printerWidthMM, printerHeightMM);
    m_previewPrintout->SetPaperRectPixels(paperRect);
    m_previewPrintout->SetPPIPrinter(logPPIPrinterX, logPPIPrinterY);

    // At 100%, the page should look about page-size on the screen.
    m_previewScaleX = float(logPPIScreenX) / logPPIPrinterX;
    m_previewScaleY = float(logPPIScreenY) / logPPIPrinterY;
}


#if wxUSE_HIGH_QUALITY_PREVIEW

// The preview, as implemented in wxPrintPreviewBase (and as used prior to wx3)
// is inexact: it uses screen DC, which has much lower resolution and has
// other properties different from printer DC, so the preview is not quite
// right.
//
// To make matters worse, if the application depends heavily on GetTextExtent()
// or does text layout itself, the output in preview and on paper can be very
// different. In particular, wxHtmlEasyPrinting is affected and the preview
// can be easily off by several pages.
//
// To fix this, we attempt to render the preview into high-resolution bitmap
// using DC with same resolution etc. as the printer DC. This takes lot of
// memory, so the code is more complicated than it could be, but the results
// are much better.
//
// Finally, this code is specific to wxMSW, because it doesn't make sense to
// bother with it on other platforms. Both OSX and modern GNOME/GTK+
// environments have builtin accurate preview (that applications should use
// instead) and the differences between screen and printer DC in wxGTK are so
// large than this trick doesn't help at all.

namespace
{

// If there's not enough memory, we need to render the preview in parts.
// Unfortunately we cannot simply use wxMemoryDC, because it reports its size
// as bitmap's size, and we need to use smaller bitmap while still reporting
// original ("correct") DC size, because printing code frequently uses
// GetSize() to determine scaling factor. This DC class handles this.

class PageFragmentDCImpl : public wxMemoryDCImpl
{
public:
    PageFragmentDCImpl(wxMemoryDC *owner, wxDC *printer,
                       const wxPoint& offset,
                       const wxSize& fullSize)
        : wxMemoryDCImpl(owner, printer),
          m_offset(offset),
          m_fullSize(fullSize)
    {
        SetDeviceOrigin(0, 0);
    }

    virtual void SetDeviceOrigin(wxCoord x, wxCoord y)
    {
        wxMemoryDCImpl::SetDeviceOrigin(x - m_offset.x, y - m_offset.y);
    }

    virtual void DoGetDeviceOrigin(wxCoord *x, wxCoord *y) const
    {
        wxMemoryDCImpl::DoGetDeviceOrigin(x, y);
        if ( x ) *x += m_offset.x;
        if ( x ) *y += m_offset.y;
    }

    virtual void DoGetSize(int *width, int *height) const
    {
        if ( width )
            *width = m_fullSize.x;
        if ( height )
            *height = m_fullSize.y;
    }

private:
    wxPoint m_offset;
    wxSize m_fullSize;
};

class PageFragmentDC : public wxDC
{
public:
    PageFragmentDC(wxDC* printer, wxBitmap& bmp,
                   const wxPoint& offset,
                   const wxSize& fullSize)
        : wxDC(new PageFragmentDCImpl((wxMemoryDC*)this, printer, offset, fullSize))
    {
        static_cast<PageFragmentDCImpl*>(m_pimpl)->DoSelect(bmp);
    }
};

// estimate how big chunks we can render, given available RAM
long ComputeFragmentSize(long printerDepth,
                         long width,
                         long height)
{
    // Compute the amount of memory needed to generate the preview.
    // Memory requirements of RenderPageFragment() are as follows:
    //
    // (memory DC - always)
    //    width * height * printerDepth/8
    // (wxImage + wxDIB instance)
    //    width * height * (3 + 4)
    //    (this could be reduced to *3 if using wxGraphicsContext)
    //
    // So, given amount of memory M, we can render at most
    //
    //    height = M / (width * (printerDepth/8 + F))
    //
    // where F is 3 or 7 depending on whether wxGraphicsContext is used or not

    wxMemorySize memAvail = wxGetFreeMemory();
    if ( memAvail == -1 )
    {
        // we don't know;  10meg shouldn't be a problem hopefully
        memAvail = 10000000;
    }
    else
    {
        // limit ourselves to half of available RAM to have a margin for other
        // apps, for our rendering code, and for miscalculations
        memAvail /= 2;
    }

    const float perPixel = float(printerDepth)/8 + (3 + 4);

    const long perLine = long(width * perPixel);
    const long maxstep = (memAvail / perLine).GetValue();
    const long step = wxMin(height, maxstep);

    wxLogTrace("printing",
               "using %liMB of RAM (%li lines) for preview, %li %lipx fragments",
               long((memAvail >> 20).GetValue()),
               maxstep,
               (height+step-1) / step,
               step);

    return step;
}

} // anonymous namespace


bool wxWindowsPrintPreview::RenderPageFragment(float scaleX, float scaleY,
                                               int *nextFinalLine,
                                               wxPrinterDC& printer,
                                               wxMemoryDC& finalDC,
                                               const wxRect& rect,
                                               int pageNum)
{
    // compute 'rect' equivalent in the small final bitmap:
    const wxRect smallRect(wxPoint(0, *nextFinalLine),
                           wxPoint(int(rect.GetRight() * scaleX),
                                   int(rect.GetBottom() * scaleY)));
    wxLogTrace("printing",
               "rendering fragment of page %i: [%i,%i,%i,%i] scaled down to [%i,%i,%i,%i]",
               pageNum,
               rect.x, rect.y, rect.GetRight(), rect.GetBottom(),
               smallRect.x, smallRect.y, smallRect.GetRight(), smallRect.GetBottom()
               );

    // create DC and bitmap compatible with printer DC:
    wxBitmap large(rect.width, rect.height, printer);
    if ( !large.IsOk() )
        return false;

    // render part of the page into it:
    {
        PageFragmentDC memoryDC(&printer, large,
                                rect.GetPosition(),
                                wxSize(m_pageWidth, m_pageHeight));
        if ( !memoryDC.IsOk() )
            return false;

        memoryDC.Clear();

        if ( !RenderPageIntoDC(memoryDC, pageNum) )
            return false;
    } // release bitmap from memoryDC

    // now scale the rendered part down and blit it into final output:

    wxImage img;
    {
        wxDIB dib(large);
        if ( !dib.IsOk() )
            return false;
        large = wxNullBitmap; // free memory a.s.a.p.
        img = dib.ConvertToImage();
    } // free the DIB now that it's no longer needed, too

    if ( !img.IsOk() )
        return false;

    img.Rescale(smallRect.width, smallRect.height, wxIMAGE_QUALITY_HIGH);
    if ( !img.IsOk() )
        return false;

    wxBitmap bmp(img);
    if ( !bmp.IsOk() )
        return false;

    img = wxNullImage;
    finalDC.DrawBitmap(bmp, smallRect.x, smallRect.y);
    if ( bmp.IsOk() )
    {
        *nextFinalLine += smallRect.height;
        return true;
    }

    return false;
}

bool wxWindowsPrintPreview::RenderPageIntoBitmapHQ(wxBitmap& bmp, int pageNum)
{
    wxLogTrace("printing", "rendering HQ preview of page %i", pageNum);

    wxPrinterDC printerDC(m_printDialogData.GetPrintData());
    if ( !printerDC.IsOk() )
        return false;

    // compute scale factor
    const float scaleX = float(bmp.GetWidth()) / float(m_pageWidth);
    const float scaleY =  float(bmp.GetHeight()) / float(m_pageHeight);

    wxMemoryDC bmpDC;
    bmpDC.SelectObject(bmp);
    bmpDC.Clear();

    const int initialStep = ComputeFragmentSize(printerDC.GetDepth(),
                                                m_pageWidth, m_pageHeight);

    wxRect todo(0, 0, m_pageWidth, initialStep); // rect to render
    int nextFinalLine = 0; // first not-yet-rendered output line

    while ( todo.y < m_pageHeight )
    {
        todo.SetBottom(wxMin(todo.GetBottom(), m_pageHeight - 1));

        if ( !RenderPageFragment(scaleX, scaleY,
                                 &nextFinalLine,
                                 printerDC,
                                 bmpDC,
                                 todo,
                                 pageNum) )
        {
            if ( todo.height < 20 )
            {
                // something is very wrong if we can't render even at this
                // slow space, let's bail out and fall back to low quality
                // preview
                wxLogTrace("printing",
                           "it seems that HQ preview doesn't work at all");
                return false;
            }

            // it's possible our memory calculation was off, or conditions
            // changed, or there's not enough _bitmap_ resources; try if using
            // smaller bitmap would help:
            todo.height /= 2;

            wxLogTrace("printing",
                       "preview of fragment failed, reducing height to %ipx",
                       todo.height);

            continue; // retry at the same position again
        }

        // move to the next segment
        todo.Offset(0, todo.height);
    }

    return true;
}

bool wxWindowsPrintPreview::RenderPageIntoBitmap(wxBitmap& bmp, int pageNum)
{
    // try high quality rendering first:
    if ( !m_hqPreviewFailed )
    {
        if ( RenderPageIntoBitmapHQ(bmp, pageNum) )
        {
            return true;
        }
        else
        {
            wxLogTrace("printing",
                       "high-quality preview failed, falling back to normal");
            m_hqPreviewFailed = true; // don't bother re-trying
        }
    }

    // if it fails, use the generic method that is less resource intensive,
    // but inexact
    return wxPrintPreviewBase::RenderPageIntoBitmap(bmp, pageNum);
}

#endif // wxUSE_HIGH_QUALITY_PREVIEW

/****************************************************************************

  FUNCTION: wxAbortProc()

    PURPOSE:  Processes messages for the Abort Dialog box

****************************************************************************/

LONG APIENTRY _EXPORT wxAbortProc(HDC WXUNUSED(hPr), int WXUNUSED(Code))
{
    MSG msg;

    if (!wxPrinterBase::sm_abortWindow)              /* If the abort dialog isn't up yet */
        return(TRUE);

    /* Process messages intended for the abort dialog box */

    while (!wxPrinterBase::sm_abortIt && ::PeekMessage(&msg, 0, 0, 0, TRUE))
        if (!IsDialogMessage((HWND) wxPrinterBase::sm_abortWindow->GetHWND(), &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

    /* bAbort is TRUE (return is FALSE) if the user has aborted */

    return !wxPrinterBase::sm_abortIt;
}

#endif
    // wxUSE_PRINTING_ARCHITECTURE

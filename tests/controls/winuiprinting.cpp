///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiprinting.cpp
// Purpose:     Deterministic WinUI contracts for the MSW printing backend
// Author:      wxWidgets development team
// Created:     2026-07-30
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_PRINTING_ARCHITECTURE

#include "wx/app.h"
#include "wx/bitmap.h"
#include "wx/dc.h"
#include "wx/dcmemory.h"
#include "wx/frame.h"
#include "wx/log.h"
#include "wx/modalhook.h"
#include "wx/printdlg.h"
#include "wx/prntbase.h"
#include "wx/scopeguard.h"
#include "wx/weakref.h"
#include "wx/msw/printdlg.h"
#include "wx/msw/printwin.h"
#include "wx/msw/private.h"
#include "wx/msw/private/gethwnd.h"
#include "wx/msw/wrapcdlg.h"

#include <vector>

namespace
{

HGLOBAL CreateTestDevMode(short orientation)
{
    HGLOBAL const handle =
        ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(DEVMODE));
    if ( !handle )
        return nullptr;

    DEVMODE* const devMode =
        static_cast<DEVMODE*>(::GlobalLock(handle));
    if ( !devMode )
    {
        ::GlobalFree(handle);
        return nullptr;
    }

    devMode->dmSize = sizeof(DEVMODE);
    devMode->dmFields = DM_ORIENTATION | DM_COPIES | DM_COLLATE;
    devMode->dmOrientation = orientation;
    devMode->dmCopies = 1;
    devMode->dmCollate = DMCOLLATE_FALSE;
    ::GlobalUnlock(handle);
    return handle;
}

wxWindowsPrintNativeData* GetWindowsNativeData(wxPrintData& data)
{
    return wxDynamicCast(data.GetNativeData(), wxWindowsPrintNativeData);
}

bool InstallTestDevMode(wxPrintData& data, short orientation)
{
    wxWindowsPrintNativeData* const nativeData =
        GetWindowsNativeData(data);
    if ( !nativeData )
        return false;

    HGLOBAL const handle = CreateTestDevMode(orientation);
    if ( !handle )
        return false;

    if ( nativeData->GetDevMode() )
        ::GlobalFree(static_cast<HGLOBAL>(nativeData->GetDevMode()));
    nativeData->SetDevMode(handle);
    return true;
}

short GetNativeOrientation(const wxPrintData& data)
{
    wxWindowsPrintNativeData* const nativeData =
        wxDynamicCast(
            const_cast<wxPrintData&>(data).GetNativeData(),
            wxWindowsPrintNativeData);
    if ( !nativeData || !nativeData->GetDevMode() )
        return 0;

    HGLOBAL const handle =
        static_cast<HGLOBAL>(nativeData->GetDevMode());
    const DEVMODE* const devMode =
        static_cast<const DEVMODE*>(::GlobalLock(handle));
    if ( !devMode )
        return 0;

    const short orientation = devMode->dmOrientation;
    ::GlobalUnlock(handle);
    return orientation;
}

void SetNativeOrientation(HGLOBAL handle, short orientation)
{
    DEVMODE* const devMode =
        handle ? static_cast<DEVMODE*>(::GlobalLock(handle)) : nullptr;
    if ( !devMode )
        return;

    devMode->dmFields |= DM_ORIENTATION;
    devMode->dmOrientation = orientation;
    ::GlobalUnlock(handle);
}

void SetNativeCopies(HGLOBAL handle, short copies)
{
    DEVMODE* const devMode =
        handle ? static_cast<DEVMODE*>(::GlobalLock(handle)) : nullptr;
    if ( !devMode )
        return;

    devMode->dmFields |= DM_COPIES;
    devMode->dmCopies = copies;
    ::GlobalUnlock(handle);
}

HWND GetWindowHWND(wxWindow* window)
{
    return window ? static_cast<HWND>(GetHwndOf(window)) : nullptr;
}

struct PrintHookContext
{
    bool accept = false;
    bool mutateOnCancel = false;
    bool softwareCopies = false;
    bool destroyOwner = false;
    bool invokeNested = false;
    bool nestedSetupFailed = false;
    bool nestedResultWasCancel = false;
    int depth = 0;
    int calls = 0;
    int maxDepth = 0;
    bool shownBeforeNested = false;
    bool shownInsideNested = false;
    bool shownAfterNested = false;
    bool createdDC = false;
    int cancelReplacementCount = 0;
    DWORD resultCopies = 3;
    bool ownerEnabledDuringCall = false;
    bool siblingDisabledDuringCall = false;
    wxWeakRef<wxWindow> owner;
    wxWeakRef<wxWindow> sibling;
    std::vector<HWND> owners;
    std::vector<DWORD> flags;
    std::vector<DWORD> rangeCounts;
    std::vector<HGLOBAL> devModes;
};

long WINAPI PrintDialogHook(void* rawContext, void* rawDialog)
{
    PrintHookContext& context =
        *static_cast<PrintHookContext*>(rawContext);
    PRINTDLGEX& dialog = *static_cast<PRINTDLGEX*>(rawDialog);

    ++context.calls;
    context.maxDepth = wxMax(context.maxDepth, context.depth + 1);
    context.owners.push_back(dialog.hwndOwner);
    context.flags.push_back(dialog.Flags);
    context.rangeCounts.push_back(dialog.nPageRanges);
    context.devModes.push_back(dialog.hDevMode);
    if ( wxWindow* const owner = context.owner.get() )
        context.ownerEnabledDuringCall = owner->IsEnabled();
    if ( wxWindow* const sibling = context.sibling.get() )
        context.siblingDisabledDuringCall = !sibling->IsEnabled();

    if ( context.depth != 0 )
        context.shownInsideNested = wxMSWIsPrinterDialogShownForTesting();

    if ( context.invokeNested && context.depth == 0 )
    {
        context.shownBeforeNested =
            wxMSWIsPrinterDialogShownForTesting();
        context.depth = 1;

        wxPrintData nestedPrintData;
        if ( !InstallTestDevMode(
                 nestedPrintData, DMORIENT_PORTRAIT) )
        {
            context.nestedSetupFailed = true;
        }
        else
        {
            wxPrintDialogData nestedData(nestedPrintData);
            wxWindowsPrintDialog nestedDialog(nullptr, &nestedData);
            context.nestedResultWasCancel =
                nestedDialog.ShowModal() == wxID_CANCEL;
        }

        context.depth = 0;
        context.shownAfterNested =
            wxMSWIsPrinterDialogShownForTesting();
    }

    if ( context.destroyOwner )
    {
        if ( wxWindow* const owner = context.owner.get() )
        {
            owner->Destroy();
            wxYield();
        }
    }

    if ( !context.accept || context.depth != 0 )
    {
        if ( context.mutateOnCancel && context.depth == 0 )
        {
            if ( dialog.hDevMode )
                ::GlobalFree(dialog.hDevMode);
            dialog.hDevMode =
                CreateTestDevMode(DMORIENT_LANDSCAPE);
            if ( dialog.hDevMode )
                ++context.cancelReplacementCount;

            dialog.nCopies = 99;
            dialog.Flags |= PD_SELECTION | PD_PRINTTOFILE;
            if ( dialog.lpPageRanges && dialog.nMaxPageRanges )
            {
                dialog.nPageRanges = 1;
                dialog.lpPageRanges[0] = { 40, 41 };
            }
        }

        dialog.dwResultAction = PD_RESULT_CANCEL;
        return S_OK;
    }

    SetNativeOrientation(dialog.hDevMode, DMORIENT_LANDSCAPE);
    if ( context.softwareCopies )
        SetNativeCopies(dialog.hDevMode, 0);

    dialog.Flags &=
        ~(PD_ALLPAGES | PD_SELECTION | PD_CURRENTPAGE |
          PD_NOPAGENUMS | PD_DISABLEPRINTTOFILE |
          PD_NOSELECTION | PD_NOCURRENTPAGE);
    dialog.Flags |=
        PD_PAGENUMS | PD_COLLATE | PD_PRINTTOFILE | PD_SHOWHELP;
    dialog.nMinPage = 1;
    dialog.nMaxPage = 90;
    dialog.nCopies = context.resultCopies;
    if ( dialog.lpPageRanges && dialog.nMaxPageRanges >= 2 )
    {
        dialog.nPageRanges = 2;
        dialog.lpPageRanges[0] = { 4, 6 };
        dialog.lpPageRanges[1] = { 9, 11 };
    }

    dialog.hDC = ::CreateCompatibleDC(nullptr);
    context.createdDC = dialog.hDC != nullptr;
    dialog.dwResultAction = PD_RESULT_PRINT;
    return S_OK;
}

struct PageSetupHookContext
{
    bool accept = false;
    bool returnInvalidUnits = false;
    bool destroyOwner = false;
    bool provideDefaultDevMode = false;
    int calls = 0;
    bool shown = false;
    bool sawNullDefaultHandles = false;
    bool suppliedDefaultDevMode = false;
    wxWeakRef<wxWindow> owner;
    std::vector<HWND> owners;
    std::vector<DWORD> flags;
    std::vector<POINT> paperSizes;
    std::vector<RECT> margins;
};

int WINAPI PageSetupDialogHook(void* rawContext, void* rawDialog)
{
    PageSetupHookContext& context =
        *static_cast<PageSetupHookContext*>(rawContext);
    PAGESETUPDLG& dialog = *static_cast<PAGESETUPDLG*>(rawDialog);

    ++context.calls;
    context.shown = wxMSWIsPrinterDialogShownForTesting();
    context.owners.push_back(dialog.hwndOwner);
    context.flags.push_back(dialog.Flags);
    context.paperSizes.push_back(dialog.ptPaperSize);
    context.margins.push_back(dialog.rtMargin);

    if ( context.provideDefaultDevMode )
    {
        context.sawNullDefaultHandles =
            !dialog.hDevMode && !dialog.hDevNames;
        dialog.hDevMode = CreateTestDevMode(DMORIENT_LANDSCAPE);
        context.suppliedDefaultDevMode = dialog.hDevMode != nullptr;
    }

    if ( context.destroyOwner )
    {
        if ( wxWindow* const owner = context.owner.get() )
        {
            owner->Destroy();
            wxYield();
        }
    }

    if ( !context.accept )
        return FALSE;

    SetNativeOrientation(dialog.hDevMode, DMORIENT_LANDSCAPE);

    if ( context.returnInvalidUnits )
    {
        dialog.Flags |=
            PSD_INTHOUSANDTHSOFINCHES |
            PSD_INHUNDREDTHSOFMILLIMETERS;
        dialog.rtMargin = { 9900, 9900, 9900, 9900 };
        return TRUE;
    }

    // Deliberately return metric units even when the input paper was
    // imperial: ConvertFromNative() must follow the returned flags.
    dialog.Flags &=
        ~(PSD_INTHOUSANDTHSOFINCHES | PSD_MINMARGINS |
          PSD_DISABLEMARGINS | PSD_DISABLEORIENTATION |
          PSD_DISABLEPRINTER | PSD_SHOWHELP);
    dialog.Flags |=
        PSD_INHUNDREDTHSOFMILLIMETERS |
        PSD_DISABLEPAPER | PSD_SHOWHELP;
    dialog.ptPaperSize = { 21000, 29700 };
    dialog.rtMinMargin = { 500, 600, 700, 800 };
    dialog.rtMargin = { 1100, 1200, 1300, 1400 };
    return TRUE;
}

class ScopedPrintNativeOps
{
public:
    explicit ScopedPrintNativeOps(PrintHookContext& context)
    {
        wxMSWResetPrintDialogNativeOpsForTesting();

        wxMSWPrintDialogNativeOpsForTesting ops;
        ops.context = &context;
        ops.printDialogEx = &PrintDialogHook;
        wxMSWSetPrintDialogNativeOpsForTesting(ops);
    }

    explicit ScopedPrintNativeOps(PageSetupHookContext& context)
    {
        wxMSWResetPrintDialogNativeOpsForTesting();

        wxMSWPrintDialogNativeOpsForTesting ops;
        ops.context = &context;
        ops.pageSetupDialog = &PageSetupDialogHook;
        wxMSWSetPrintDialogNativeOpsForTesting(ops);
    }

    ~ScopedPrintNativeOps()
    {
        wxMSWResetPrintDialogNativeOpsForTesting();
    }

private:
    wxDECLARE_NO_COPY_CLASS(ScopedPrintNativeOps);
};

struct PrinterDCHookContext
{
    int startDoc = 0;
    int endDoc = 0;
    int startPage = 0;
    int endPage = 0;
    int failStartDocCall = 0;
    int failEndDocCall = 0;
    int failStartPageCall = 0;
    int failEndPageCall = 0;
    int abortStartDocCall = 0;
    int abortEndDocCall = 0;
    int abortStartPageCall = 0;
    int abortEndPageCall = 0;
    DWORD startDocError = ERROR_CANCELLED;
    bool sawHDC = true;
    bool sequenceValid = true;
    int documentDepth = 0;
    int pageDepth = 0;
    wxString documentName;
};

int WINAPI PrinterDCStartDocHook(void* rawContext,
                                 void* rawHdc,
                                 const void* rawInfo)
{
    PrinterDCHookContext& context =
        *static_cast<PrinterDCHookContext*>(rawContext);
    ++context.startDoc;
    if ( context.documentDepth != 0 || context.pageDepth != 0 )
        context.sequenceValid = false;
    if ( !rawHdc )
        context.sawHDC = false;

    const DOCINFO* const info =
        static_cast<const DOCINFO*>(rawInfo);
    if ( info && info->lpszDocName )
        context.documentName = info->lpszDocName;

    if ( context.startDoc == context.failStartDocCall )
    {
        if ( context.startDoc == context.abortStartDocCall )
            wxPrinterBase::sm_abortIt = true;
        ::SetLastError(context.startDocError);
        return 0;
    }

    context.documentDepth = 1;
    return 1;
}

int WINAPI PrinterDCEndDocHook(void* rawContext, void* rawHdc)
{
    PrinterDCHookContext& context =
        *static_cast<PrinterDCHookContext*>(rawContext);
    ++context.endDoc;
    if ( context.documentDepth != 1 || context.pageDepth != 0 )
        context.sequenceValid = false;
    context.documentDepth = 0;
    if ( !rawHdc )
        context.sawHDC = false;
    if ( context.endDoc == context.abortEndDocCall )
        wxPrinterBase::sm_abortIt = true;
    return context.endDoc == context.failEndDocCall ? 0 : 1;
}

int WINAPI PrinterDCStartPageHook(void* rawContext, void* rawHdc)
{
    PrinterDCHookContext& context =
        *static_cast<PrinterDCHookContext*>(rawContext);
    ++context.startPage;
    if ( context.documentDepth != 1 || context.pageDepth != 0 )
        context.sequenceValid = false;
    if ( !rawHdc )
        context.sawHDC = false;
    if ( context.startPage == context.failStartPageCall )
    {
        if ( context.startPage == context.abortStartPageCall )
            wxPrinterBase::sm_abortIt = true;
        return 0;
    }

    context.pageDepth = 1;
    return 1;
}

int WINAPI PrinterDCEndPageHook(void* rawContext, void* rawHdc)
{
    PrinterDCHookContext& context =
        *static_cast<PrinterDCHookContext*>(rawContext);
    ++context.endPage;
    if ( context.documentDepth != 1 || context.pageDepth != 1 )
        context.sequenceValid = false;
    context.pageDepth = 0;
    if ( !rawHdc )
        context.sawHDC = false;
    if ( context.endPage == context.abortEndPageCall )
        wxPrinterBase::sm_abortIt = true;
    return context.endPage == context.failEndPageCall ? 0 : 1;
}

class ScopedPrinterDCNativeOps
{
public:
    explicit ScopedPrinterDCNativeOps(PrinterDCHookContext& context)
    {
        wxMSWResetPrinterDCNativeOpsForTesting();

        wxMSWPrinterDCNativeOpsForTesting ops;
        ops.context = &context;
        ops.startDoc = &PrinterDCStartDocHook;
        ops.endDoc = &PrinterDCEndDocHook;
        ops.startPage = &PrinterDCStartPageHook;
        ops.endPage = &PrinterDCEndPageHook;
        wxMSWSetPrinterDCNativeOpsForTesting(ops);
    }

    ~ScopedPrinterDCNativeOps()
    {
        wxMSWResetPrinterDCNativeOpsForTesting();
    }

private:
    wxDECLARE_NO_COPY_CLASS(ScopedPrinterDCNativeOps);
};

void CheckPrinterDCSequence(const PrinterDCHookContext& context)
{
    CHECK(context.sequenceValid);
    CHECK(context.documentDepth == 0);
    CHECK(context.pageDepth == 0);
}

struct PreviewCounters
{
    int prepare = 0;
    int beginPrinting = 0;
    int endPrinting = 0;
    int beginDocument = 0;
    int endDocument = 0;
    int printPage = 0;
    int destruct = 0;
    bool beginDocumentResult = true;
    bool printPageResult = true;
    bool dcPresentInCallbacks = true;
    std::vector<int> events;
};

class PreviewContractPrintout final : public wxPrintout
{
public:
    explicit PreviewContractPrintout(PreviewCounters& counters)
        : wxPrintout("preview contract"),
          m_counters(counters)
    {
    }

    ~PreviewContractPrintout() override
    {
        ++m_counters.destruct;
    }

    void OnPreparePrinting() override
    {
        Record(1);
        ++m_counters.prepare;
    }

    void OnBeginPrinting() override
    {
        Record(2);
        ++m_counters.beginPrinting;
    }

    void OnEndPrinting() override
    {
        Record(6);
        ++m_counters.endPrinting;
    }

    bool OnBeginDocument(int WXUNUSED(startPage),
                         int WXUNUSED(endPage)) override
    {
        Record(3);
        ++m_counters.beginDocument;
        return m_counters.beginDocumentResult;
    }

    void OnEndDocument() override
    {
        Record(5);
        ++m_counters.endDocument;
    }

    bool OnPrintPage(int WXUNUSED(page)) override
    {
        Record(4);
        ++m_counters.printPage;
        return m_counters.printPageResult;
    }

    bool HasPage(int page) override
    {
        return page == 1;
    }

    void GetPageInfo(int* minPage,
                     int* maxPage,
                     int* pageFrom,
                     int* pageTo) override
    {
        *minPage = 1;
        *maxPage = 1;
        *pageFrom = 1;
        *pageTo = 1;
    }

private:
    void Record(int event)
    {
        if ( !GetDC() )
            m_counters.dcPresentInCallbacks = false;
        m_counters.events.push_back(event);
    }

    PreviewCounters& m_counters;
};

class PreviewContractHarness final : public wxPrintPreviewBase
{
public:
    explicit PreviewContractHarness(wxPrintout* printout)
        : wxPrintPreviewBase(printout)
    {
    }

    bool Print(bool WXUNUSED(interactive)) override
    {
        return false;
    }

    void DetermineScaling() override
    {
    }

    bool RenderInto(wxDC& dc, int page)
    {
        return RenderPageIntoDC(dc, page);
    }
};

class DismissPreviewFailureHook final : public wxModalDialogHook
{
public:
    int Enter(wxDialog* WXUNUSED(dialog)) override
    {
        ++calls;
        return wxID_OK;
    }

    int calls = 0;
};

struct PrinterLoopCounters
{
    int prepare = 0;
    int beginPrinting = 0;
    int endPrinting = 0;
    int beginDocument = 0;
    int endDocument = 0;
    int failBeginDocumentCall = 0;
    int failPage = 0;
    int abortPage = 0;
    int destroyOwnerPage = 0;
    int destroyAbortWindowThroughProcPage = 0;
    int abortInEndDocumentCall = 0;
    bool abortInEndPrinting = false;
    bool ownerDestroyRequested = false;
    bool abortProcInvoked = false;
    bool abortProcResult = true;
    int nestedAttempts = 0;
    bool nestedResult = true;
    bool nestedSawAbortWindow = false;
    bool nestedConstructorPreservedState = false;
    bool nestedGuardPreservedState = false;
    bool callbacksHadDC = true;
    bool useNativeDocument = true;
    wxPrintDialogData* nestedDialogData = nullptr;
    wxWeakRef<wxWindow> ownerToDestroy;
    wxWeakRef<wxWindow> abortWindowAtOwnerDestroy;
    wxWeakRef<wxWindow> abortWindowDestroyedThroughProc;
    wxWeakRef<wxWindow> nestedAbortWindow;
    std::vector<int> pages;
};

class PrinterLoopPrintout final : public wxPrintout
{
public:
    explicit PrinterLoopPrintout(PrinterLoopCounters& counters)
        : wxPrintout("printer loop contract"),
          m_counters(counters)
    {
    }

    void OnPreparePrinting() override
    {
        CheckDC();
        ++m_counters.prepare;
    }

    void OnBeginPrinting() override
    {
        CheckDC();
        ++m_counters.beginPrinting;
    }

    void OnEndPrinting() override
    {
        CheckDC();
        ++m_counters.endPrinting;
        if ( m_counters.abortInEndPrinting )
            wxPrinterBase::sm_abortIt = true;
    }

    bool OnBeginDocument(int startPage, int endPage) override
    {
        CheckDC();
        const int call = ++m_counters.beginDocument;
        if ( call == m_counters.failBeginDocumentCall )
            return false;

        return !m_counters.useNativeDocument ||
               wxPrintout::OnBeginDocument(startPage, endPage);
    }

    void OnEndDocument() override
    {
        CheckDC();
        if ( m_counters.useNativeDocument )
            wxPrintout::OnEndDocument();
        const int call = ++m_counters.endDocument;
        if ( call == m_counters.abortInEndDocumentCall )
            wxPrinterBase::sm_abortIt = true;
    }

    bool OnPrintPage(int page) override
    {
        CheckDC();
        m_counters.pages.push_back(page);

        if ( m_counters.nestedDialogData &&
             m_counters.nestedAttempts == 0 )
        {
            ++m_counters.nestedAttempts;
            wxWindow* const abortWindow =
                wxPrinterBase::sm_abortWindow;
            m_counters.nestedSawAbortWindow = abortWindow != nullptr;
            m_counters.nestedAbortWindow = abortWindow;

            wxWindowsPrinter nestedPrinter(
                m_counters.nestedDialogData);
            m_counters.nestedConstructorPreservedState =
                wxPrinterBase::sm_abortWindow == abortWindow;
            {
                wxLogNull suppressExpectedWarning;
                m_counters.nestedResult =
                    nestedPrinter.Print(nullptr, this, true);
            }
            m_counters.nestedGuardPreservedState =
                wxPrinterBase::sm_abortWindow == abortWindow;
        }

        if ( page == m_counters.abortPage )
            wxPrinterBase::sm_abortIt = true;

        if ( page == m_counters.destroyOwnerPage )
        {
            m_counters.abortWindowAtOwnerDestroy =
                wxWeakRef<wxWindow>(wxPrinterBase::sm_abortWindow);
            if ( wxWindow* const owner =
                     m_counters.ownerToDestroy.get() )
            {
                m_counters.ownerDestroyRequested = true;
                owner->Destroy();
                wxYield();
            }
        }

        if ( page == m_counters.destroyAbortWindowThroughProcPage )
        {
            wxWindow* const abortWindow =
                wxPrinterBase::sm_abortWindow;
            m_counters.abortWindowDestroyedThroughProc =
                wxWeakRef<wxWindow>(abortWindow);
            if ( abortWindow )
            {
                const wxWeakRef<wxWindow> weakAbortWindow(abortWindow);
                abortWindow->CallAfter(
                    [weakAbortWindow]()
                    {
                        if ( wxWindow* const live =
                                 weakAbortWindow.get() )
                        {
                            live->Destroy();
                        }
                    });
                m_counters.abortProcInvoked = true;
                m_counters.abortProcResult =
                    wxMSWInvokePrintAbortProcForTesting();
            }
        }

        return page != m_counters.failPage;
    }

    bool HasPage(int page) override
    {
        return page >= 1 && page <= 20;
    }

    void GetPageInfo(int* minPage,
                     int* maxPage,
                     int* pageFrom,
                     int* pageTo) override
    {
        *minPage = 1;
        *maxPage = 20;
        *pageFrom = 1;
        *pageTo = 20;
    }

private:
    void CheckDC()
    {
        if ( !GetDC() )
            m_counters.callbacksHadDC = false;
    }

    PrinterLoopCounters& m_counters;
};

} // anonymous namespace

TEST_CASE("WinUIPrinting::PrintDialogCancelOwnerAndReuse",
          "[winui-printing][winui-013][print-dialog]")
{
    wxFrame owner(
        nullptr, wxID_ANY, "print owner",
        wxPoint(-32000, -32000), wxSize(360, 220));
    owner.ShowWithoutActivating();
    wxFrame sibling(
        nullptr, wxID_ANY, "print sibling",
        wxPoint(-32000, -32000), wxSize(360, 220));
    sibling.ShowWithoutActivating();

    wxPrintData originalPrintData;
    REQUIRE(InstallTestDevMode(
        originalPrintData, DMORIENT_PORTRAIT));
    originalPrintData.SetOrientation(wxPORTRAIT);
    wxWindowsPrintNativeData* const originalNative =
        GetWindowsNativeData(originalPrintData);
    REQUIRE(originalNative);
    void* const originalHandle = originalNative->GetDevMode();

    wxPrintDialogData input(originalPrintData);
    input.SetMinPage(1);
    input.SetMaxPage(50);
    input.SetNoCopies(2);
    input.SetAllPages(false);
    input.SetPageRanges({ { 2, 3 }, { 7, 9 } });
    input.SetMaxPageRanges(2);
    input.SetCollate(true);
    input.EnableHelp(true);

    PrintHookContext context;
    context.mutateOnCancel = true;
    context.owner = &owner;
    context.sibling = &sibling;
    ScopedPrintNativeOps hook(context);
    wxWindowsPrintDialog dialog(&owner, &input);

    for ( int cycle = 0; cycle < 100; ++cycle )
        CHECK(dialog.ShowModal() == wxID_CANCEL);

    CHECK(context.calls == 100);
    CHECK(context.cancelReplacementCount == 100);
    CHECK(context.ownerEnabledDuringCall);
    CHECK(context.siblingDisabledDuringCall);
    CHECK(owner.IsEnabled());
    CHECK(sibling.IsEnabled());
    REQUIRE(context.owners.size() == 100);
    REQUIRE(context.devModes.size() == 100);
    REQUIRE(context.rangeCounts.size() == 100);
    for ( std::size_t n = 0; n < context.owners.size(); ++n )
    {
        CHECK(context.owners[n] == GetWindowHWND(&owner));
        CHECK(context.devModes[n] != nullptr);
        CHECK(context.devModes[n] != originalHandle);
        CHECK(context.rangeCounts[n] == 2);
    }
    CHECK_FALSE(wxMSWIsPrinterDialogShownForTesting());

    CHECK(originalNative->GetDevMode() == originalHandle);
    CHECK(GetNativeOrientation(originalPrintData) == DMORIENT_PORTRAIT);
    CHECK(input.GetNoCopies() == 2);
    REQUIRE(input.GetPageRanges().size() == 2);
    CHECK(input.GetPageRanges()[0].fromPage == 2);
    CHECK(input.GetPageRanges()[0].toPage == 3);
    CHECK(input.GetPageRanges()[1].fromPage == 7);
    CHECK(input.GetPageRanges()[1].toPage == 9);
    CHECK(dialog.GetPrintDialogData().GetNoCopies() == 2);
    REQUIRE(dialog.GetPrintDialogData().GetPageRanges().size() == 2);
    CHECK(dialog.GetPrintDialogData().GetPageRanges()[0].fromPage == 2);
    CHECK(dialog.GetPrintDialogData().GetPageRanges()[0].toPage == 3);
    CHECK(dialog.GetPrintDialogData().GetPageRanges()[1].fromPage == 7);
    CHECK(dialog.GetPrintDialogData().GetPageRanges()[1].toPage == 9);

    REQUIRE(dialog.Create(&owner, &input));
    CHECK(dialog.ShowModal() == wxID_CANCEL);
    CHECK(context.calls == 101);
    CHECK(context.cancelReplacementCount == 101);
}

TEST_CASE("WinUIPrinting::PrintDialogCommitAndNestedState",
          "[winui-printing][winui-013][print-dialog][reentrant]")
{
    wxFrame owner(
        nullptr, wxID_ANY, "print commit owner",
        wxPoint(-32000, -32000), wxSize(360, 220));
    owner.ShowWithoutActivating();

    wxPrintData originalPrintData;
    REQUIRE(InstallTestDevMode(
        originalPrintData, DMORIENT_PORTRAIT));
    originalPrintData.SetOrientation(wxPORTRAIT);
    wxWindowsPrintNativeData* const originalNative =
        GetWindowsNativeData(originalPrintData);
    REQUIRE(originalNative);
    void* const originalHandle = originalNative->GetDevMode();

    wxPrintDialogData input(originalPrintData);
    input.SetMinPage(1);
    input.SetMaxPage(20);
    input.SetNoCopies(1);
    input.SetPageRanges({ { 1, 2 }, { 5, 8 } });
    input.SetMaxPageRanges(2);

    PrintHookContext context;
    context.accept = true;
    context.invokeNested = true;
    ScopedPrintNativeOps hook(context);
    wxWindowsPrintDialog dialog(&owner, &input);

    CHECK(dialog.ShowModal() == wxID_OK);
    CHECK(context.calls == 2);
    CHECK(context.maxDepth == 2);
    CHECK(context.shownBeforeNested);
    CHECK(context.shownInsideNested);
    CHECK(context.shownAfterNested);
    CHECK_FALSE(context.nestedSetupFailed);
    CHECK(context.nestedResultWasCancel);
    CHECK(context.createdDC);
    CHECK_FALSE(wxMSWIsPrinterDialogShownForTesting());

    wxPrintDialogData& result = dialog.GetPrintDialogData();
    CHECK(result.GetMinPage() == 1);
    CHECK(result.GetMaxPage() == 90);
    CHECK(result.GetNoCopies() == 3);
    CHECK(result.GetCollate());
    CHECK(result.GetPrintToFile());
    CHECK(result.GetEnableHelp());
    REQUIRE(result.GetPageRanges().size() == 2);
    CHECK(result.GetPageRanges()[0].fromPage == 4);
    CHECK(result.GetPageRanges()[0].toPage == 6);
    CHECK(result.GetPageRanges()[1].fromPage == 9);
    CHECK(result.GetPageRanges()[1].toPage == 11);
    CHECK(result.GetPrintData().GetOrientation() == wxLANDSCAPE);

    wxDC* const dc = dialog.GetPrintDC();
    REQUIRE(dc);
    CHECK(dc->IsOk());
    delete dc;
    CHECK(dialog.GetPrintDC() == nullptr);

    CHECK(originalNative->GetDevMode() == originalHandle);
    CHECK(GetNativeOrientation(originalPrintData) == DMORIENT_PORTRAIT);

    context.invokeNested = false;
    for ( int cycle = 0; cycle < 100; ++cycle )
    {
        context.accept = cycle % 2 == 0;
        CHECK(dialog.ShowModal() ==
              (context.accept ? wxID_OK : wxID_CANCEL));
        CHECK_FALSE(wxMSWIsPrinterDialogShownForTesting());

        if ( context.accept )
        {
            wxDC* const cycleDC = dialog.GetPrintDC();
            REQUIRE(cycleDC);
            CHECK(cycleDC->IsOk());
            delete cycleDC;
        }
        else
        {
            CHECK(dialog.GetPrintDC() == nullptr);
        }
    }
    CHECK(context.calls == 102);

    // Re-creating a public dialog object must release an unclaimed result DC
    // and all per-invocation native state.
    context.accept = true;
    CHECK(dialog.ShowModal() == wxID_OK);
    REQUIRE(dialog.Create(&owner, &input));
    CHECK(dialog.GetPrintDC() == nullptr);
    context.accept = false;
    CHECK(dialog.ShowModal() == wxID_CANCEL);
    CHECK(context.calls == 104);
}

TEST_CASE("WinUIPrinting::PageSetupUnitsCancelAndCommit",
          "[winui-printing][winui-013][page-setup]")
{
    wxFrame owner(
        nullptr, wxID_ANY, "page setup owner",
        wxPoint(-32000, -32000), wxSize(360, 220));
    owner.ShowWithoutActivating();

    wxPrintData originalPrintData;
    REQUIRE(InstallTestDevMode(
        originalPrintData, DMORIENT_PORTRAIT));
    originalPrintData.SetOrientation(wxPORTRAIT);
    wxWindowsPrintNativeData* const originalNative =
        GetWindowsNativeData(originalPrintData);
    REQUIRE(originalNative);
    void* const originalHandle = originalNative->GetDevMode();

    wxPageSetupDialogData input(originalPrintData);
    input.SetPaperSize(wxPAPER_LETTER);
    input.SetDefaultMinMargins(false);
    input.SetMinMarginTopLeft({ 5, 6 });
    input.SetMinMarginBottomRight({ 7, 8 });
    input.SetMarginTopLeft({ 11, 12 });
    input.SetMarginBottomRight({ 13, 14 });
    const wxSize inputPaperSize = input.GetPaperSize();

    PageSetupHookContext context;
    ScopedPrintNativeOps hook(context);
    wxWindowsPageSetupDialog dialog(&owner, &input);

    CHECK(dialog.ShowModal() == wxID_CANCEL);
    REQUIRE(context.flags.size() == 1);
    REQUIRE(context.paperSizes.size() == 1);
    REQUIRE(context.margins.size() == 1);
    CHECK(context.owners[0] == GetWindowHWND(&owner));
    CHECK(context.shown);
    CHECK((context.flags[0] & PSD_INTHOUSANDTHSOFINCHES) != 0);
    CHECK((context.flags[0] & PSD_INHUNDREDTHSOFMILLIMETERS) == 0);
    CHECK((context.flags[0] & PSD_MINMARGINS) != 0);
    CHECK(context.paperSizes[0].x ==
          ::MulDiv(inputPaperSize.x, 10000, 254));
    CHECK(context.paperSizes[0].y ==
          ::MulDiv(inputPaperSize.y, 10000, 254));
    CHECK(context.margins[0].left == ::MulDiv(11, 10000, 254));
    CHECK(context.margins[0].top == ::MulDiv(12, 10000, 254));
    CHECK_FALSE(wxMSWIsPrinterDialogShownForTesting());

    CHECK(originalNative->GetDevMode() == originalHandle);
    CHECK(GetNativeOrientation(originalPrintData) == DMORIENT_PORTRAIT);
    CHECK(input.GetMarginTopLeft() == wxPoint(11, 12));
    CHECK(dialog.GetPageSetupDialogData().GetMarginTopLeft() ==
          wxPoint(11, 12));

    context.accept = true;
    context.returnInvalidUnits = true;
    CHECK(dialog.ShowModal() == wxID_CANCEL);
    CHECK(dialog.GetPageSetupDialogData().GetPrintData().GetOrientation() ==
          wxPORTRAIT);
    CHECK(dialog.GetPageSetupDialogData().GetMarginTopLeft() ==
          wxPoint(11, 12));

    context.returnInvalidUnits = false;
    CHECK(dialog.ShowModal() == wxID_OK);
    CHECK(context.calls == 3);

    const wxPageSetupDialogData& result =
        dialog.GetPageSetupDialogData();
    CHECK(result.GetPrintData().GetOrientation() == wxLANDSCAPE);
    CHECK(result.GetPaperSize() == wxSize(297, 210));
    CHECK(result.GetMinMarginTopLeft() == wxPoint(5, 6));
    CHECK(result.GetMinMarginBottomRight() == wxPoint(7, 8));
    CHECK(result.GetMarginTopLeft() == wxPoint(11, 12));
    CHECK(result.GetMarginBottomRight() == wxPoint(13, 14));
    CHECK(result.GetDefaultMinMargins());
    CHECK(result.GetEnableMargins());
    CHECK(result.GetEnableOrientation());
    CHECK_FALSE(result.GetEnablePaper());
    CHECK(result.GetEnablePrinter());
    CHECK(result.GetEnableHelp());

    context.accept = false;
    REQUIRE(dialog.Create(&owner, &input));
    CHECK(dialog.ShowModal() == wxID_CANCEL);
    CHECK(context.calls == 4);
}

TEST_CASE("WinUIPrinting::PageSetupDefaultsAndOwnerLifetime",
          "[winui-printing][winui-013][page-setup][lifetime]")
{
    wxFrame * const owner =
        new wxFrame(
            nullptr, wxID_ANY, "doomed page setup owner",
            wxPoint(-32000, -32000), wxSize(360, 220));
    owner->ShowWithoutActivating();
    const wxWeakRef<wxWindow> weakOwner(owner);
    const HWND ownerHwnd = GetWindowHWND(owner);

    wxPageSetupDialogData input;
    input.SetDefaultInfo(true);

    PageSetupHookContext context;
    context.accept = true;
    context.destroyOwner = true;
    context.provideDefaultDevMode = true;
    context.owner = owner;
    ScopedPrintNativeOps hook(context);
    wxWindowsPageSetupDialog dialog(owner, &input);

    CHECK(dialog.ShowModal() == wxID_OK);
    CHECK(context.calls == 1);
    CHECK(context.owners[0] == ownerHwnd);
    CHECK(context.sawNullDefaultHandles);
    CHECK(context.suppliedDefaultDevMode);
    CHECK_FALSE(weakOwner.get());
    CHECK_FALSE(wxMSWIsPrinterDialogShownForTesting());
    CHECK(dialog.GetPageSetupDialogData().GetDefaultInfo());
    CHECK(dialog.GetPageSetupDialogData().GetPrintData().GetOrientation() ==
          wxLANDSCAPE);
}

TEST_CASE("WinUIPrinting::PreviewRenderBalancesCallbacks",
          "[winui-printing][winui-013][print-preview]")
{
    wxBitmap bitmap(64, 64);
    REQUIRE(bitmap.IsOk());
    wxMemoryDC dc;
    dc.SelectObject(bitmap);

    SECTION("successful page")
    {
        PreviewCounters counters;
        PreviewContractPrintout* const printout =
            new PreviewContractPrintout(counters);
        {
            PreviewContractHarness preview(printout);
            CHECK(preview.RenderInto(dc, 1));
            CHECK(printout->GetDC() == nullptr);
            CHECK(counters.events ==
                  std::vector<int>({ 1, 2, 3, 4, 5, 6 }));
            CHECK(counters.prepare == 1);
            CHECK(counters.beginPrinting == 1);
            CHECK(counters.beginDocument == 1);
            CHECK(counters.printPage == 1);
            CHECK(counters.endDocument == 1);
            CHECK(counters.endPrinting == 1);
            CHECK(counters.dcPresentInCallbacks);
            CHECK(counters.destruct == 0);
        }
        CHECK(counters.destruct == 1);
    }

    SECTION("begin document failure")
    {
        PreviewCounters counters;
        counters.beginDocumentResult = false;
        PreviewContractPrintout* const printout =
            new PreviewContractPrintout(counters);
        DismissPreviewFailureHook modalHook;
        modalHook.Register();
        {
            PreviewContractHarness preview(printout);
            CHECK_FALSE(preview.RenderInto(dc, 1));
            CHECK(printout->GetDC() == nullptr);
            CHECK(counters.events ==
                  std::vector<int>({ 1, 2, 3, 6 }));
            CHECK(counters.endDocument == 0);
            CHECK(counters.endPrinting == 1);
            CHECK(counters.dcPresentInCallbacks);
            CHECK(modalHook.calls == 1);
        }
        CHECK(counters.destruct == 1);
    }

    SECTION("page callback failure")
    {
        PreviewCounters counters;
        counters.printPageResult = false;
        PreviewContractPrintout* const printout =
            new PreviewContractPrintout(counters);
        {
            PreviewContractHarness preview(printout);
            CHECK_FALSE(preview.RenderInto(dc, 1));
            CHECK(printout->GetDC() == nullptr);
            CHECK(counters.events ==
                  std::vector<int>({ 1, 2, 3, 4, 5, 6 }));
            CHECK(counters.printPage == 1);
            CHECK(counters.endDocument == 1);
            CHECK(counters.endPrinting == 1);
            CHECK(counters.dcPresentInCallbacks);
        }
        CHECK(counters.destruct == 1);
    }

    dc.SelectObject(wxNullBitmap);
}

TEST_CASE("WinUIPrinting::PreviewFrameMayDieBeforeInitialize",
          "[winui-printing][winui-013][print-preview][lifetime]")
{
    PreviewCounters counters;
    PreviewContractHarness* const preview =
        new PreviewContractHarness(
            new PreviewContractPrintout(counters));
    wxPreviewFrame* const frame =
        new wxPreviewFrame(
            preview,
            nullptr,
            "uninitialized preview",
            wxPoint(-32000, -32000),
            wxSize(360, 220),
            wxDEFAULT_FRAME_STYLE);

    delete frame;
    CHECK(counters.destruct == 1);
}

TEST_CASE("WinUIPrinting::PrinterStopsAcrossRangesAndCopies",
          "[winui-printing][winui-013][printer-loop]")
{
    wxFrame owner(
        nullptr, wxID_ANY, "printer loop owner",
        wxPoint(-32000, -32000), wxSize(360, 220));
    owner.ShowWithoutActivating();

    wxPrintData printData;
    REQUIRE(InstallTestDevMode(printData, DMORIENT_PORTRAIT));
    printData.SetOrientation(wxPORTRAIT);
    printData.SetNoCopies(1);

    wxPrintDialogData dialogData(printData);
    dialogData.SetMinPage(1);
    dialogData.SetMaxPage(20);
    dialogData.SetNoCopies(3);
    dialogData.SetAllPages(false);
    dialogData.SetPageRanges({ { 4, 6 }, { 9, 11 } });
    dialogData.SetMaxPageRanges(2);

    PrintHookContext hookContext;
    hookContext.accept = true;
    hookContext.softwareCopies = true;
    ScopedPrintNativeOps hook(hookContext);
    PrinterDCHookContext dcContext;
    ScopedPrinterDCNativeOps dcHook(dcContext);

    wxWindowsPrinter printer(&dialogData);
    PrinterLoopCounters counters;
    PrinterLoopPrintout printout(counters);

    SECTION("success visits every range for every software copy")
    {
        CHECK(printer.Print(&owner, &printout, true));
        CHECK(counters.pages ==
              std::vector<int>(
                  { 4, 5, 6, 9, 10, 11,
                    4, 5, 6, 9, 10, 11,
                    4, 5, 6, 9, 10, 11 }));
        CHECK(counters.prepare == 1);
        CHECK(counters.beginPrinting == 1);
        CHECK(counters.endPrinting == 1);
        CHECK(counters.beginDocument == 3);
        CHECK(counters.endDocument == 3);
        CHECK(counters.callbacksHadDC);
        CHECK(printout.GetDC() == nullptr);
        CHECK(dcContext.startDoc == 3);
        CHECK(dcContext.endDoc == 3);
        CHECK(dcContext.startPage == 18);
        CHECK(dcContext.endPage == 18);
        CHECK(dcContext.sawHDC);
        CHECK(dcContext.documentName == "printer loop contract");
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_NO_ERROR);
    }

    SECTION("OnPrintPage false stops all later ranges and copies")
    {
        counters.failPage = 5;
        CHECK_FALSE(printer.Print(&owner, &printout, true));
        CHECK(counters.pages == std::vector<int>({ 4, 5 }));
        CHECK(counters.beginPrinting == 1);
        CHECK(counters.endPrinting == 1);
        CHECK(counters.beginDocument == 1);
        CHECK(counters.endDocument == 1);
        CHECK(counters.callbacksHadDC);
        CHECK(printout.GetDC() == nullptr);
        CHECK(dcContext.startDoc == 1);
        CHECK(dcContext.endDoc == 1);
        CHECK(dcContext.startPage == 2);
        CHECK(dcContext.endPage == 2);
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_CANCELLED);
    }

    SECTION("abort flag stops all later ranges and copies")
    {
        counters.abortPage = 5;
        CHECK_FALSE(printer.Print(&owner, &printout, true));
        CHECK(counters.pages == std::vector<int>({ 4, 5 }));
        CHECK(counters.beginPrinting == 1);
        CHECK(counters.endPrinting == 1);
        CHECK(counters.beginDocument == 1);
        CHECK(counters.endDocument == 1);
        CHECK(counters.callbacksHadDC);
        CHECK(printout.GetDC() == nullptr);
        CHECK(dcContext.startDoc == 1);
        CHECK(dcContext.endDoc == 1);
        CHECK(dcContext.startPage == 2);
        CHECK(dcContext.endPage == 2);
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_CANCELLED);
    }

    SECTION("owner destruction during a page callback cancels safely")
    {
        wxFrame* const doomedOwner =
            new wxFrame(
                nullptr, wxID_ANY, "doomed printer loop owner",
                wxPoint(-32000, -32000), wxSize(360, 220));
        doomedOwner->ShowWithoutActivating();
        const wxWeakRef<wxWindow> weakOwner(doomedOwner);

        counters.destroyOwnerPage = 4;
        counters.ownerToDestroy = weakOwner;

        CHECK_FALSE(printer.Print(doomedOwner, &printout, true));
        wxYield();

        CHECK(counters.ownerDestroyRequested);
        CHECK_FALSE(weakOwner.get());
        CHECK_FALSE(counters.abortWindowAtOwnerDestroy.get());
        CHECK(counters.pages == std::vector<int>({ 4 }));
        CHECK(counters.beginPrinting == 1);
        CHECK(counters.endPrinting == 1);
        CHECK(counters.beginDocument == 1);
        CHECK(counters.endDocument == 1);
        CHECK(counters.callbacksHadDC);
        CHECK(printout.GetDC() == nullptr);
        CHECK(dcContext.startDoc == 1);
        CHECK(dcContext.endDoc == 1);
        CHECK(dcContext.startPage == 1);
        CHECK(dcContext.endPage == 1);
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_CANCELLED);
    }

    SECTION("abort proc revalidates after dispatch destroys its dialog")
    {
        counters.destroyAbortWindowThroughProcPage = 4;

        CHECK_FALSE(printer.Print(&owner, &printout, true));
        wxYield();

        CHECK(counters.abortProcInvoked);
        CHECK_FALSE(counters.abortProcResult);
        CHECK_FALSE(counters.abortWindowDestroyedThroughProc.get());
        CHECK(counters.pages == std::vector<int>({ 4 }));
        CHECK(counters.beginPrinting == 1);
        CHECK(counters.endPrinting == 1);
        CHECK(counters.beginDocument == 1);
        CHECK(counters.endDocument == 1);
        CHECK(counters.callbacksHadDC);
        CHECK(printout.GetDC() == nullptr);
        CHECK(dcContext.startDoc == 1);
        CHECK(dcContext.endDoc == 1);
        CHECK(dcContext.startPage == 1);
        CHECK(dcContext.endPage == 1);
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_CANCELLED);
    }

    SECTION("failed second document prevents the third copy")
    {
        counters.failBeginDocumentCall = 2;
        CHECK_FALSE(printer.Print(&owner, &printout, true));
        CHECK(counters.pages ==
              std::vector<int>({ 4, 5, 6, 9, 10, 11 }));
        CHECK(counters.beginPrinting == 1);
        CHECK(counters.endPrinting == 1);
        CHECK(counters.beginDocument == 2);
        CHECK(counters.endDocument == 1);
        CHECK(counters.callbacksHadDC);
        CHECK(printout.GetDC() == nullptr);
        CHECK(dcContext.startDoc == 1);
        CHECK(dcContext.endDoc == 1);
        CHECK(dcContext.startPage == 6);
        CHECK(dcContext.endPage == 6);
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_ERROR);
    }

    SECTION("recursive print is rejected without corrupting outer state")
    {
        counters.nestedDialogData = &dialogData;
        CHECK(printer.Print(&owner, &printout, true));
        CHECK(counters.nestedAttempts == 1);
        CHECK_FALSE(counters.nestedResult);
        CHECK(counters.nestedSawAbortWindow);
        CHECK(counters.nestedConstructorPreservedState);
        CHECK(counters.nestedGuardPreservedState);
        CHECK_FALSE(counters.nestedAbortWindow.get());
        CHECK(counters.pages.size() == 18);
        CHECK(counters.beginDocument == 3);
        CHECK(counters.endDocument == 3);
        CHECK(counters.endPrinting == 1);
        CHECK(counters.callbacksHadDC);
        CHECK(printout.GetDC() == nullptr);
        CHECK(hookContext.calls == 1);
        CHECK(dcContext.startDoc == 3);
        CHECK(dcContext.endDoc == 3);
        CHECK(dcContext.startPage == 18);
        CHECK(dcContext.endPage == 18);
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_NO_ERROR);
    }

    SECTION("native StartDoc cancellation is propagated without EndDoc")
    {
        dcContext.failStartDocCall = 1;
        dcContext.startDocError = ERROR_CANCELLED;
        CHECK_FALSE(printer.Print(&owner, &printout, true));
        CHECK(counters.pages.empty());
        CHECK(counters.beginPrinting == 1);
        CHECK(counters.endPrinting == 1);
        CHECK(counters.beginDocument == 1);
        CHECK(counters.endDocument == 0);
        CHECK(counters.callbacksHadDC);
        CHECK(printout.GetDC() == nullptr);
        CHECK(dcContext.startDoc == 1);
        CHECK(dcContext.endDoc == 0);
        CHECK(dcContext.startPage == 0);
        CHECK(dcContext.endPage == 0);
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_CANCELLED);
    }

    SECTION("native StartDoc error is distinguished from cancellation")
    {
        wxLogNull suppressExpectedError;
        dcContext.failStartDocCall = 1;
        dcContext.startDocError = ERROR_GEN_FAILURE;
        CHECK_FALSE(printer.Print(&owner, &printout, true));
        CHECK(counters.pages.empty());
        CHECK(counters.beginDocument == 1);
        CHECK(counters.endDocument == 0);
        CHECK(counters.endPrinting == 1);
        CHECK(dcContext.startDoc == 1);
        CHECK(dcContext.endDoc == 0);
        CHECK(dcContext.startPage == 0);
        CHECK(dcContext.endPage == 0);
        CHECK(printout.GetDC() == nullptr);
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_ERROR);
    }

    SECTION("native StartPage failure skips the page and stops the job")
    {
        dcContext.failStartPageCall = 1;
        CHECK_FALSE(printer.Print(&owner, &printout, true));
        CHECK(counters.pages.empty());
        CHECK(counters.beginDocument == 1);
        CHECK(counters.endDocument == 1);
        CHECK(counters.endPrinting == 1);
        CHECK(dcContext.startDoc == 1);
        CHECK(dcContext.endDoc == 1);
        CHECK(dcContext.startPage == 1);
        CHECK(dcContext.endPage == 0);
        CHECK(printout.GetDC() == nullptr);
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_ERROR);
    }

    SECTION("native EndPage failure stops later pages and copies")
    {
        dcContext.failEndPageCall = 1;
        CHECK_FALSE(printer.Print(&owner, &printout, true));
        CHECK(counters.pages == std::vector<int>({ 4 }));
        CHECK(counters.beginDocument == 1);
        CHECK(counters.endDocument == 1);
        CHECK(counters.endPrinting == 1);
        CHECK(dcContext.startDoc == 1);
        CHECK(dcContext.endDoc == 1);
        CHECK(dcContext.startPage == 1);
        CHECK(dcContext.endPage == 1);
        CHECK(printout.GetDC() == nullptr);
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_ERROR);
    }

    SECTION("native EndDoc failure prevents the next copy")
    {
        dcContext.failEndDocCall = 1;
        CHECK_FALSE(printer.Print(&owner, &printout, true));
        CHECK(counters.pages ==
              std::vector<int>({ 4, 5, 6, 9, 10, 11 }));
        CHECK(counters.beginDocument == 1);
        CHECK(counters.endDocument == 1);
        CHECK(counters.endPrinting == 1);
        CHECK(dcContext.startDoc == 1);
        CHECK(dcContext.endDoc == 1);
        CHECK(dcContext.startPage == 6);
        CHECK(dcContext.endPage == 6);
        CHECK(printout.GetDC() == nullptr);
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_ERROR);
    }

    SECTION("teardown failures do not overwrite an earlier cancellation")
    {
        counters.abortPage = 4;
        dcContext.failEndPageCall = 1;
        dcContext.failEndDocCall = 1;
        CHECK_FALSE(printer.Print(&owner, &printout, true));
        CHECK(counters.pages == std::vector<int>({ 4 }));
        CHECK(dcContext.startDoc == 1);
        CHECK(dcContext.endDoc == 1);
        CHECK(dcContext.startPage == 1);
        CHECK(dcContext.endPage == 1);
        CHECK(printout.GetDC() == nullptr);
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_CANCELLED);
    }

    SECTION("abort raised inside a failing StartDoc takes precedence")
    {
        dcContext.failStartDocCall = 1;
        dcContext.abortStartDocCall = 1;
        dcContext.startDocError = ERROR_GEN_FAILURE;
        CHECK_FALSE(printer.Print(&owner, &printout, true));
        CHECK(counters.pages.empty());
        CHECK(counters.beginDocument == 1);
        CHECK(counters.endDocument == 0);
        CHECK(counters.endPrinting == 1);
        CHECK(dcContext.startDoc == 1);
        CHECK(dcContext.endDoc == 0);
        CHECK(dcContext.startPage == 0);
        CHECK(dcContext.endPage == 0);
        CHECK(printout.GetDC() == nullptr);
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_CANCELLED);
    }

    SECTION("abort raised inside a failing StartPage takes precedence")
    {
        dcContext.failStartPageCall = 1;
        dcContext.abortStartPageCall = 1;
        CHECK_FALSE(printer.Print(&owner, &printout, true));
        CHECK(counters.pages.empty());
        CHECK(counters.beginDocument == 1);
        CHECK(counters.endDocument == 1);
        CHECK(counters.endPrinting == 1);
        CHECK(dcContext.startDoc == 1);
        CHECK(dcContext.endDoc == 1);
        CHECK(dcContext.startPage == 1);
        CHECK(dcContext.endPage == 0);
        CHECK(printout.GetDC() == nullptr);
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_CANCELLED);
    }

    SECTION("abort raised inside a failing EndPage takes precedence")
    {
        dcContext.failEndPageCall = 1;
        dcContext.abortEndPageCall = 1;
        CHECK_FALSE(printer.Print(&owner, &printout, true));
        CHECK(counters.pages == std::vector<int>({ 4 }));
        CHECK(counters.beginDocument == 1);
        CHECK(counters.endDocument == 1);
        CHECK(counters.endPrinting == 1);
        CHECK(dcContext.startDoc == 1);
        CHECK(dcContext.endDoc == 1);
        CHECK(dcContext.startPage == 1);
        CHECK(dcContext.endPage == 1);
        CHECK(printout.GetDC() == nullptr);
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_CANCELLED);
    }

    SECTION("abort raised inside a failing EndDoc takes precedence")
    {
        dcContext.failEndDocCall = 1;
        dcContext.abortEndDocCall = 1;
        CHECK_FALSE(printer.Print(&owner, &printout, true));
        CHECK(counters.pages ==
              std::vector<int>({ 4, 5, 6, 9, 10, 11 }));
        CHECK(counters.beginDocument == 1);
        CHECK(counters.endDocument == 1);
        CHECK(counters.endPrinting == 1);
        CHECK(dcContext.startDoc == 1);
        CHECK(dcContext.endDoc == 1);
        CHECK(dcContext.startPage == 6);
        CHECK(dcContext.endPage == 6);
        CHECK(printout.GetDC() == nullptr);
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_CANCELLED);
    }

    SECTION("zero requested software copies is clamped to one")
    {
        hookContext.resultCopies = 0;
        CHECK(printer.Print(&owner, &printout, true));
        CHECK(counters.pages ==
              std::vector<int>({ 4, 5, 6, 9, 10, 11 }));
        CHECK(counters.beginDocument == 1);
        CHECK(counters.endDocument == 1);
        CHECK(dcContext.startDoc == 1);
        CHECK(dcContext.endDoc == 1);
        CHECK(dcContext.startPage == 6);
        CHECK(dcContext.endPage == 6);
        CHECK(printout.GetDC() == nullptr);
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_NO_ERROR);
    }

    SECTION("abort raised by the final EndDocument changes the result")
    {
        counters.abortInEndDocumentCall = 3;
        CHECK_FALSE(printer.Print(&owner, &printout, true));
        CHECK(counters.pages.size() == 18);
        CHECK(counters.beginDocument == 3);
        CHECK(counters.endDocument == 3);
        CHECK(counters.endPrinting == 1);
        CHECK(dcContext.startDoc == 3);
        CHECK(dcContext.endDoc == 3);
        CHECK(dcContext.startPage == 18);
        CHECK(dcContext.endPage == 18);
        CHECK(printout.GetDC() == nullptr);
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_CANCELLED);
    }

    SECTION("abort raised by OnEndPrinting changes the result")
    {
        counters.abortInEndPrinting = true;
        CHECK_FALSE(printer.Print(&owner, &printout, true));
        CHECK(counters.pages.size() == 18);
        CHECK(counters.beginDocument == 3);
        CHECK(counters.endDocument == 3);
        CHECK(counters.endPrinting == 1);
        CHECK(dcContext.startDoc == 3);
        CHECK(dcContext.endDoc == 3);
        CHECK(dcContext.startPage == 18);
        CHECK(dcContext.endPage == 18);
        CHECK(printout.GetDC() == nullptr);
        CheckPrinterDCSequence(dcContext);
        CHECK(wxPrinterBase::GetLastError() ==
              wxPRINTER_CANCELLED);
    }
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_PRINTING_ARCHITECTURE

///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/nonownedwnd.cpp
// Purpose:     wxNonOwnedWindow implementation for MSW.
// Author:      Vadim Zeitlin
// Created:     2011-10-09 (extracted from src/msw/toplevel.cpp)
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/frame.h"       // Only for wxFRAME_SHAPED.
    #include "wx/region.h"
    #include "wx/sizer.h"
    #include "wx/msw/private.h"
#endif // WX_PRECOMP

#include "wx/nonownedwnd.h"

#if wxUSE_GRAPHICS_CONTEXT
    #include "wx/msw/wrapgdip.h"
    #include "wx/graphics.h"
#endif // wxUSE_GRAPHICS_CONTEXT

#include "wx/dynlib.h"
#include "wx/msw/missing.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "wx/weakref.h"
    #include "wx/winui/private/tlwhostmsw.h"
    #include "wx/winui/private/transient.h"
#endif

#include <memory>

// ============================================================================
// wxNonOwnedWindow implementation
// ============================================================================

bool wxNonOwnedWindow::DoClearShape()
{
    if (::SetWindowRgn(GetHwnd(), nullptr, TRUE) == 0)
    {
        wxLogLastError(wxT("SetWindowRgn"));
        return false;
    }

    return true;
}

bool wxNonOwnedWindow::DoSetRegionShape(const wxRegion& region)
{
    // Windows takes ownership of the region, so
    // we'll have to make a copy of the region to give to it.
    const DWORD noBytes =
        ::GetRegionData(GetHrgnOf(region), 0, nullptr);
    if ( !noBytes )
    {
        wxLogLastError(wxT("GetRegionData(size)"));
        return false;
    }

    std::unique_ptr<unsigned char[]> rgnData(
        new unsigned char[noBytes]);
    if ( ::GetRegionData(
             GetHrgnOf(region),
             noBytes,
             reinterpret_cast<RGNDATA *>(rgnData.get())) != noBytes )
    {
        wxLogLastError(wxT("GetRegionData"));
        return false;
    }

    HRGN hrgn = ::ExtCreateRegion(
        nullptr,
        noBytes,
        reinterpret_cast<RGNDATA *>(rgnData.get()));
    if ( !hrgn )
    {
        wxLogLastError(wxT("ExtCreateRegion"));
        return false;
    }

    // SetWindowRgn expects the region to be in coordinates
    // relative to the window, not the client area.
    const wxPoint clientOrigin = GetClientAreaOrigin();
    if ( ::OffsetRgn(
             hrgn, -clientOrigin.x, -clientOrigin.y) == ERROR )
    {
        wxLogLastError(wxT("OffsetRgn"));
        ::DeleteObject(hrgn);
        return false;
    }

    // Now call the shape API with the new region.
    if (::SetWindowRgn(GetHwnd(), hrgn, TRUE) == 0)
    {
        wxLogLastError(wxT("SetWindowRgn"));
        // USER32 assumes ownership only after success.
        ::DeleteObject(hrgn);
        return false;
    }
    return true;
}

#if wxUSE_GRAPHICS_CONTEXT

#include "wx/msw/wrapgdip.h"

// This class contains data used only when SetPath(wxGraphicsPath) is called.
class wxNonOwnedWindowShapeImpl
{
public:
    wxNonOwnedWindowShapeImpl(wxNonOwnedWindow* win, const wxGraphicsPath& path) :
        m_win(win),
        m_path(path)
    {
        // Create the region corresponding to this path and set it as windows
        // shape.
        std::unique_ptr<wxGraphicsContext> context(wxGraphicsContext::Create(win));
        Region gr(static_cast<GraphicsPath*>(m_path.GetNativePath()));
        win->SetShape(
            wxRegion(
                gr.GetHRGN(static_cast<Graphics*>(context->GetNativeContext()))
            )
        );


        // Connect to the paint event to draw the border.
        //
        // TODO: Do this only optionally?
        m_win->Bind(wxEVT_PAINT, &wxNonOwnedWindowShapeImpl::OnPaint, this);
    }

    virtual ~wxNonOwnedWindowShapeImpl()
    {
        m_win->Unbind(wxEVT_PAINT, &wxNonOwnedWindowShapeImpl::OnPaint, this);
    }

private:
    void OnPaint(wxPaintEvent& event)
    {
        event.Skip();

        wxPaintDC dc(m_win);
        std::unique_ptr<wxGraphicsContext> context(wxGraphicsContext::Create(dc));
        context->SetPen(wxPen(*wxLIGHT_GREY, 2));
        context->StrokePath(m_path);
    }

    wxNonOwnedWindow* const m_win;
    wxGraphicsPath m_path;

    wxDECLARE_NO_COPY_CLASS(wxNonOwnedWindowShapeImpl);
};

bool wxNonOwnedWindow::DoSetPathShape(const wxGraphicsPath& path)
{
    delete m_shapeImpl;
    m_shapeImpl = new wxNonOwnedWindowShapeImpl(this, path);

    return true;
}

#endif // wxUSE_GRAPHICS_CONTEXT

wxNonOwnedWindow::wxNonOwnedWindow()
{
#if wxUSE_GRAPHICS_CONTEXT
    m_shapeImpl = nullptr;
#endif // wxUSE_GRAPHICS_CONTEXT

    m_activeDPI = wxDefaultSize;
    m_perMonitorDPIaware = false;
}

wxNonOwnedWindow::~wxNonOwnedWindow()
{
#if wxUSE_GRAPHICS_CONTEXT
    delete m_shapeImpl;
#endif // wxUSE_GRAPHICS_CONTEXT
}

#if defined(__WXWINUI__) && wxUSE_WINUI3
namespace
{
bool gs_winuiFailNextOwnerWriteForTest = false;
wxWinUINonOwnedOwnerPublishHookForTesting
    gs_winuiNonOwnedOwnerPublishHookForTesting = nullptr;
wxWinUINonOwnedLayoutRefreshHookForTesting
    gs_winuiNonOwnedLayoutRefreshHookForTesting = nullptr;
}

void wxWinUIMSWFailNextOwnerWriteForTest()
{
    gs_winuiFailNextOwnerWriteForTest = true;
}

void wxWinUISetNonOwnedOwnerPublishHookForTesting(
    wxWinUINonOwnedOwnerPublishHookForTesting hook)
{
    gs_winuiNonOwnedOwnerPublishHookForTesting = hook;
}

void wxWinUIResetNonOwnedOwnerPublishHookForTesting()
{
    gs_winuiNonOwnedOwnerPublishHookForTesting = nullptr;
}

void wxWinUISetNonOwnedLayoutRefreshHookForTesting(
    wxWinUINonOwnedLayoutRefreshHookForTesting hook)
{
    gs_winuiNonOwnedLayoutRefreshHookForTesting = hook;
}

void wxWinUIResetNonOwnedLayoutRefreshHookForTesting()
{
    gs_winuiNonOwnedLayoutRefreshHookForTesting = nullptr;
}
#endif

bool wxNonOwnedWindow::Reparent(wxWindowBase* newParent)
{
    // ::SetParent() can't be used for non-owned windows, as they don't have
    // any parent, only the owner, so use a different function for them even
    // if, confusingly, the owner is stored at the same location as the parent
    // and so uses the same GWLP_HWNDPARENT offset.

    // Do not call the base class function here to skip wxWindow reparenting.
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const wxWeakRef<wxWindow> alive(this);
    const wxWeakRef<wxWindow> oldParentIdentity(GetParent());
    const WXHWND hwnd = GetHWND();
    const unsigned long long hwndGeneration =
        wxWinUIMSWGetHwndGeneration(this, hwnd);
    const HWND oldNativeOwner =
        hwnd ? ::GetWindow(reinterpret_cast<HWND>(hwnd), GW_OWNER)
             : nullptr;
#endif
    if ( !wxWindowBase::Reparent(newParent) )
    {
#if defined(__WXWINUI__) && wxUSE_WINUI3
        // The seam describes the next native publication, not an unrelated
        // later Reparent() after this logical transaction already failed.
        gs_winuiFailNextOwnerWriteForTest = false;
#endif
        return false;
    }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    const auto getCurrentWindow =
        [alive, this, hwnd, hwndGeneration]() -> wxWindow *
        {
             wxWindow * const live = alive.get();
             return live == this &&
                            !live->IsBeingDeleted() &&
                            !wxWinUITLWHostIsDestroyScheduled(live) &&
                            live->GetHWND() == hwnd &&
                           hwndGeneration &&
                           wxWinUIMSWGetHwndGeneration(
                               live, hwnd) == hwndGeneration
                       ? live
                       : nullptr;
        };
    const auto reconcileLogicalParentToNative =
        [&]() -> bool
        {
            wxWindow * const live = getCurrentWindow();
            if ( !live )
                return false;

            const HWND nativeOwner =
                ::GetWindow(reinterpret_cast<HWND>(hwnd), GW_OWNER);
            wxWindow *nativeOwnerWindow =
                nativeOwner ? wxFindWinFromHandle(nativeOwner) : nullptr;
            if ( nativeOwner && !nativeOwnerWindow )
            {
                // The only non-wx owner we can identify safely is the exact
                // original association captured before the logical mutation.
                wxWindow * const oldParent = oldParentIdentity.get();
                if ( nativeOwner != oldNativeOwner || !oldParent ||
                     oldParent->GetHWND() !=
                         reinterpret_cast<WXHWND>(nativeOwner) ||
                     !wxWinUIMSWGetHwndGeneration(
                         oldParent, oldParent->GetHWND()) )
                {
                    return false;
                }
                nativeOwnerWindow = oldParent;
            }

            if ( live->GetParent() != nativeOwnerWindow &&
                 !live->wxWindowBase::Reparent(nativeOwnerWindow) )
            {
                return false;
            }

            wxWindow * const afterLogical = getCurrentWindow();
            return afterLogical &&
                   afterLogical->GetParent() == nativeOwnerWindow &&
                   ::GetWindow(
                       reinterpret_cast<HWND>(hwnd), GW_OWNER) ==
                       nativeOwner;
        };
    const auto failAndReconcile =
        [&]() -> bool
        {
            if ( !reconcileLogicalParentToNative() && alive )
            {
                wxLogWarning(
                    "wxWinUI: failed to reconcile a non-owned window's "
                    "logical parent with its verified native owner");
            }
            return false;
        };

    // SetWindowLongPtr is a synchronous owner/style boundary. Converge on the
    // current logical owner if application code redirects this TLW while the
    // outer request is still on the stack. The inherited-layout projection is
    // part of the same bounded transaction: it may dispatch into a local peer,
    // so both logical and native owner identities are checked again afterwards.
    constexpr unsigned MaxOwnerProjectionPasses = 16;
    for ( unsigned pass = 0;
          pass < MaxOwnerProjectionPasses;
          ++pass )
    {
        wxWindow * const live = getCurrentWindow();
        if ( !live )
            return false;

        wxWindow * const owner = live->GetParent();
        const wxWeakRef<wxWindow> ownerIdentity(owner);
        const WXHWND ownerHwnd = owner ? owner->GetHWND() : nullptr;
        const unsigned long long ownerGeneration =
            owner ? wxWinUIMSWGetHwndGeneration(owner, ownerHwnd) : 0;
        if ( owner &&
             (owner->IsBeingDeleted() ||
              wxWinUITLWHostIsDestroyScheduled(owner) ||
              !ownerHwnd || !ownerGeneration) )
        {
            return failAndReconcile();
        }

        const auto ownerProjectionIsExact =
            [&, owner, ownerIdentity, ownerHwnd, ownerGeneration](
                wxWindow *candidate)
            {
                if ( !candidate || candidate->GetParent() != owner ||
                     ::GetWindow(
                         reinterpret_cast<HWND>(hwnd), GW_OWNER) !=
                         reinterpret_cast<HWND>(ownerHwnd) )
                {
                    return false;
                }

                return !owner ||
                       (ownerIdentity.get() == owner &&
                        !owner->IsBeingDeleted() &&
                        !wxWinUITLWHostIsDestroyScheduled(owner) &&
                        owner->GetHWND() == ownerHwnd &&
                        wxWinUIMSWGetHwndGeneration(
                            owner, ownerHwnd) == ownerGeneration);
            };

        ::SetLastError(ERROR_SUCCESS);
        LONG_PTR previous = 0;
        DWORD error = ERROR_SUCCESS;
        if ( gs_winuiFailNextOwnerWriteForTest )
        {
            gs_winuiFailNextOwnerWriteForTest = false;
            error = ERROR_ACCESS_DENIED;
        }
        else
        {
            previous = ::SetWindowLongPtr(
                reinterpret_cast<HWND>(hwnd),
                GWLP_HWNDPARENT,
                reinterpret_cast<LONG_PTR>(ownerHwnd));
            error = ::GetLastError();
        }
        if ( !previous && error != ERROR_SUCCESS )
            return failAndReconcile();

        wxWindow * const afterWrite = getCurrentWindow();
        if ( !afterWrite )
            return false;
        if ( !ownerProjectionIsExact(afterWrite) )
            continue;

        if ( gs_winuiNonOwnedOwnerPublishHookForTesting )
            gs_winuiNonOwnedOwnerPublishHookForTesting(afterWrite);

        wxWindow * const afterNative = getCurrentWindow();
        if ( !afterNative )
        {
            // Deletion/destroy scheduling invalidates the requested commit.
            // Never report success for a transaction which did not reach the
            // checked owner+layout postcondition.
            return false;
        }
        if ( !ownerProjectionIsExact(afterNative) )
            continue;

        // Changing an owner does not refresh WS_EX_LAYOUTRTL on an existing
        // non-owned HWND. This projection can enter a local peer hook; never
        // report success until its exact owner generation is still current.
        afterNative->MSWRefreshInheritedLayoutDirection();
        wxWindow * const afterLayout = getCurrentWindow();
        if ( !afterLayout )
            return false;
        if ( !ownerProjectionIsExact(afterLayout) )
            continue;

        if ( gs_winuiNonOwnedLayoutRefreshHookForTesting )
            gs_winuiNonOwnedLayoutRefreshHookForTesting(afterLayout);

        wxWindow * const afterLayoutHook = getCurrentWindow();
        if ( !afterLayoutHook )
            return false;
        if ( ownerProjectionIsExact(afterLayoutHook) )
            return true;
    }

    return failAndReconcile();
#else
    const HWND hwndOwner = GetParent() ? GetHwndOf(GetParent()) : 0;

    ::SetWindowLongPtr(GetHwnd(), GWLP_HWNDPARENT, (LONG_PTR)hwndOwner);

    return true;
#endif
}

namespace
{

static bool IsPerMonitorDPIAware(HWND hwnd)
{
    bool dpiAware = false;

    // Determine if 'Per Monitor v2' DPI awareness is enabled in the
    // applications manifest.
    #define WXDPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((WXDPI_AWARENESS_CONTEXT)-4)
    typedef WXDPI_AWARENESS_CONTEXT(WINAPI * GetWindowDpiAwarenessContext_t)(HWND hwnd);
    typedef BOOL(WINAPI * AreDpiAwarenessContextsEqual_t)(WXDPI_AWARENESS_CONTEXT dpiContextA, WXDPI_AWARENESS_CONTEXT dpiContextB);
    static GetWindowDpiAwarenessContext_t s_pfnGetWindowDpiAwarenessContext = nullptr;
    static AreDpiAwarenessContextsEqual_t s_pfnAreDpiAwarenessContextsEqual = nullptr;
    static bool s_initDone = false;

    if ( !s_initDone )
    {
        wxLoadedDLL dllUser32("user32.dll");
        wxDL_INIT_FUNC(s_pfn, GetWindowDpiAwarenessContext, dllUser32);
        wxDL_INIT_FUNC(s_pfn, AreDpiAwarenessContextsEqual, dllUser32);
        s_initDone = true;
    }

    if ( s_pfnGetWindowDpiAwarenessContext && s_pfnAreDpiAwarenessContextsEqual )
    {
        WXDPI_AWARENESS_CONTEXT dpiAwarenessContext = s_pfnGetWindowDpiAwarenessContext(hwnd);

        if ( s_pfnAreDpiAwarenessContextsEqual(dpiAwarenessContext, WXDPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2) == TRUE )
        {
            dpiAware = true;
        }
    }

    return dpiAware;
}

}

bool wxNonOwnedWindow::IsThisEnabled() const
{
    // Under MSW we use the actual window state rather than the value of
    // m_isEnabled because the latter might be out of sync for TLWs disabled
    // by a native modal dialog being shown, as native functions such as
    // ::MessageBox() etc just call ::EnableWindow() on them without updating
    // m_isEnabled and we have no way to be notified about this.
    //
    // But we can only do this if the window had been already created, so test
    // for this in order to return correct result if it was disabled after
    // using default ctor but before calling Create().
    return m_hWnd ? !(::GetWindowLong(GetHwnd(), GWL_STYLE) & WS_DISABLED)
                  : m_isEnabled;
}

WXLRESULT wxNonOwnedWindow::MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
    WXLRESULT rc = 0;
    bool processed = false;

    switch ( message )
    {
        case WM_NCCALCSIZE:
            // Use this message ID to determine the DPI information on
            // window creation, since WM_NCCREATE is not generated for dialogs.
            if ( m_activeDPI == wxDefaultSize )
            {
                m_perMonitorDPIaware = IsPerMonitorDPIAware(GetHwnd());
                m_activeDPI = GetDPI();
            }
            break;

        case WM_DPICHANGED:
            {
                const RECT* const prcNewWindow =
                                         reinterpret_cast<const RECT*>(lParam);

                processed = HandleDPIChange(wxSize(LOWORD(wParam),
                                                   HIWORD(wParam)),
                                            wxRectFromRECT(*prcNewWindow));
            }
            break;

        case WM_SHOWWINDOW:
            // Default handling of this message in Windows restores the TLW
            // even if it had been hidden before, and we don't want this to
            // happen, so suppress the default behaviour and simply ignore it.
            if ( lParam == SW_PARENTOPENING && !IsShown() )
                processed = true;
            break;
    }

    if (!processed)
        rc = wxNonOwnedWindowBase::MSWWindowProc(message, wParam, lParam);

    return rc;
}

bool wxNonOwnedWindow::HandleDPIChange(const wxSize& newDPI, const wxRect& newRect)
{
    if ( !m_perMonitorDPIaware )
    {
        return false;
    }

    // Update the window decoration size to the new DPI: this seems to be the
    // call with the least amount of side effects that is sufficient to do it
    // and we need to do this in order for the size calculations, either in the
    // user-defined wxEVT_DPI_CHANGED handler or in our own GetBestSize() call
    // below, to work correctly.
    ::SetWindowPos(GetHwnd(),
                   0, 0, 0, 0, 0,
                   SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW |
                   SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING |
                   SWP_FRAMECHANGED);

    const bool processed = MSWUpdateOnDPIChange(m_activeDPI, newDPI);
    m_activeDPI = newDPI;

    // We consider that if the event was processed, the application resized the
    // window on its own already, but otherwise do it ourselves.
    if ( !processed )
    {
        // The best size doesn't scale exactly with the DPI, so while the new
        // size is usually a decent guess, it's typically not exactly correct.
        // We can't always do much better, but at least ensure that the window
        // is still big enough to show its contents if it uses a sizer.
        //
        // Note that if it doesn't use a sizer, we can't do anything here as
        // using the best size wouldn't be the right thing to do: some controls
        // (e.g. multiline wxTextCtrl) can have best size much bigger than
        // their current, or minimal, size and we don't want to expand them
        // significantly just because the DPI has changed, see #23091.
        wxRect actualNewRect = newRect;
        if ( wxSizer* sizer = GetSizer() )
        {
            const wxSize minSize = ClientToWindowSize(sizer->GetMinSize());
            wxSize diff = minSize - newRect.GetSize();
            diff.IncTo(wxSize(0, 0));

            // Use wxRect::Inflate() to ensure that the center of the bigger
            // rectangle is at the same position as the center of the proposed
            // one, to prevent moving the window back to the old display from
            // which it might have been just moved to this one, as doing this
            // would result in an infinite stream of WM_DPICHANGED messages.
            actualNewRect.Inflate(diff);
        }

        SetSize(actualNewRect);
    }

    Refresh();

    return true;
}

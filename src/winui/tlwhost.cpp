/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/tlwhost.cpp
// Purpose:     one shared XAML island per top-level window
// Author:      wxWidgets development team
// Created:     2026-07-20
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_WINUI3

#include "wx/winui/private/tlwhost.h"

#include "private.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/window.h"
#endif

#include "wx/toplevel.h"
#include "wx/msw/private.h"

#include <windowsx.h>
#include <commctrl.h>

#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>

#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <functional>
#include <utility>
#include <vector>

void wxWinUILogException(const char *what, const winrt::hresult_error& e);

namespace
{

// ----------------------------------------------------------------------------
// Input black box: set WX_WINUI_INPUT_LOG=<file> to trace the island input
// pipeline (root pointer events, forwards, capture states).  This exists to
// DIAGNOSE the input-freeze family with facts instead of theories: when a
// window freezes, the tail of this log says exactly where the flow stopped
// (island no longer receiving? receiving but not forwarding? forwarding but
// the target ignoring?).  No-op when the variable is unset.
// ----------------------------------------------------------------------------

FILE *wxWinUIInputLogFile()
{
    static FILE *s_file = []() -> FILE *
    {
        wchar_t path[MAX_PATH] = {};
        if ( !::GetEnvironmentVariableW(L"WX_WINUI_INPUT_LOG", path,
                                        MAX_PATH) || !path[0] )
            return nullptr;

        FILE * const f = _wfopen(path, L"a");
        if ( f )
        {
            fprintf(f, "\n==== session pid=%lu ====\n",
                    ::GetCurrentProcessId());
            fflush(f);
        }
        return f;
    }();

    return s_file;
}

void wxWinUIInputLog(const char *fmt, ...)
{
    FILE * const f = wxWinUIInputLogFile();
    if ( !f )
        return;

    // Every line carries the tick, the current Win32 capture window and the
    // foreground window: the three state values the freeze family hinges on.
    fprintf(f, "[%9llu cap=%p fg=%p] ",
            static_cast<unsigned long long>(::GetTickCount64()),
            static_cast<void *>(::GetCapture()),
            static_cast<void *>(::GetForegroundWindow()));

    va_list args;
    va_start(args, fmt);
    vfprintf(f, fmt, args);
    va_end(args);

    fputc('\n', f);
    fflush(f);
}

bool wxWinUIInputLogEnabled() { return wxWinUIInputLogFile() != nullptr; }

// All live hosts, keyed by their top-level window.
std::map<const wxWindow *, wxWinUITopLevelHost *> gs_tlwHosts;

// Cumulative counters for the lifecycle unit tests: the add/revoke balance
// of the per-slot XAML handlers (every add must be matched by a revocation
// on every removal path) and the schedule/run balance of the coalesced
// flush (a Freeze() must not turn a MarkDirty() storm into a CallAfter
// storm).
unsigned gs_slotHandlerAdds = 0;
unsigned gs_slotHandlerRevokes = 0;
unsigned gs_flushSchedules = 0;
unsigned gs_flushRuns = 0;

constexpr UINT_PTR wxWINUI_TLW_SUBCLASS_BRIDGE = 1;
constexpr UINT_PTR wxWINUI_TLW_SUBCLASS_INNER = 2;

// The slot containers are Grids extended with cursor support: ProtectedCursor
// is only reachable from a derived element, and it is how a per-window
// wxWindow::SetCursor() (e.g. a wait cursor on one control) is reflected over
// the island, which otherwise insists on its own arrow.
struct wxWinUISlotGrid
    : winrt::Microsoft::UI::Xaml::Controls::GridT<wxWinUISlotGrid>
{
    void SetWindowCursor(winrt::Microsoft::UI::Input::InputCursor const& cursor)
    {
        try
        {
            // A null cursor means "use the default arrow".
            ProtectedCursor(cursor);
        }
        catch ( const winrt::hresult_error& )
        {
            // Not being able to set the cursor is never fatal.
        }
    }
};

// The implementation object of each slot's container, for cursor access.
std::map<const wxWinUISlot *, winrt::com_ptr<wxWinUISlotGrid>> gs_slotGrids;

// Root canvas subclass with cursor access, so the island can mirror the
// native cursor of whatever wx window the pointer hovers.
struct wxWinUIRootCanvas
    : winrt::Microsoft::UI::Xaml::Controls::CanvasT<wxWinUIRootCanvas>
{
    void SetPointerCursor(winrt::Microsoft::UI::Input::InputCursor const& cursor)
    {
        try
        {
            ProtectedCursor(cursor);
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }
};

// The implementation object of each host's root canvas.
std::map<const wxWinUITopLevelHost *,
         winrt::com_ptr<wxWinUIRootCanvas>> gs_rootCanvases;

// Give the slotted element a UIA name from the wx side when it doesn't have
// one: one coherent accessibility tree per window instead of anonymous
// fragments.  The wx label (mnemonics stripped) wins over the internal
// window name, which is only used when it was explicitly set (not the
// default class-based one).
void wxWinUIApplyAutomationName(wxWindow *window,
                                const winrt::Microsoft::UI::Xaml::UIElement& element)
{
    if ( !window || !element )
        return;

    try
    {
        using winrt::Microsoft::UI::Xaml::Automation::AutomationProperties;

        if ( !AutomationProperties::GetName(element).empty() )
            return;

        wxString text = wxStripMenuCodes(window->GetLabel(),
                                         wxStrip_Mnemonics);
        if ( text.empty() )
        {
            // GetName() only helps when the developer actually set it: the
            // library-wide default names ("panel", "choice", "treeCtrl"...)
            // are class labels, and a screen reader announcing one of those
            // is worse than announcing nothing.
            static const wxChar *const defaultNames[] =
            {
                wxT("bitmapButton"), wxT("button"), wxT("check"),
                wxT("checkList"), wxT("choice"), wxT("comboBox"),
                wxT("control"), wxT("dialog"), wxT("frame"), wxT("gauge"),
                wxT("grid"), wxT("groupBox"), wxT("listBox"), wxT("listCtrl"),
                wxT("notebook"), wxT("panel"), wxT("radioBox"),
                wxT("radioButton"), wxT("scrollBar"), wxT("slider"),
                wxT("spinButton"), wxT("splitter"), wxT("staticBitmap"),
                wxT("staticText"), wxT("statusBar"), wxT("text"),
                wxT("toolbar"), wxT("treeCtrl"), wxT("wxSpinCtrl"),
            };

            const wxString name = window->GetName();
            bool isDefault = name.empty() ||
                             name == window->GetClassInfo()->GetClassName();
            for ( size_t n = 0; !isDefault && n < WXSIZEOF(defaultNames); ++n )
                isDefault = (name == defaultNames[n]);

            if ( !isDefault )
                text = name;
        }

        if ( !text.empty() )
            AutomationProperties::SetName(element,
                                          winrt::hstring(text.ToStdWstring()));
    }
    catch ( const winrt::hresult_error& )
    {
    }
}

// Best-effort HCURSOR -> InputSystemCursor mapping (null = default arrow).
winrt::Microsoft::UI::Input::InputCursor
wxWinUICursorFromHCURSOR(HCURSOR hcursor)
{
    namespace MUI = winrt::Microsoft::UI::Input;

    if ( !hcursor )
        return nullptr;

    static const struct { const wchar_t *id; MUI::InputSystemCursorShape shape; }
    s_map[] =
    {
        { IDC_ARROW,      MUI::InputSystemCursorShape::Arrow },
        { IDC_IBEAM,      MUI::InputSystemCursorShape::IBeam },
        { IDC_WAIT,       MUI::InputSystemCursorShape::Wait },
        { IDC_APPSTARTING,MUI::InputSystemCursorShape::AppStarting },
        { IDC_HAND,       MUI::InputSystemCursorShape::Hand },
        { IDC_CROSS,      MUI::InputSystemCursorShape::Cross },
        { IDC_SIZEALL,    MUI::InputSystemCursorShape::SizeAll },
        { IDC_SIZENWSE,   MUI::InputSystemCursorShape::SizeNorthwestSoutheast },
        { IDC_SIZENESW,   MUI::InputSystemCursorShape::SizeNortheastSouthwest },
        { IDC_SIZEWE,     MUI::InputSystemCursorShape::SizeWestEast },
        { IDC_SIZENS,     MUI::InputSystemCursorShape::SizeNorthSouth },
        { IDC_NO,         MUI::InputSystemCursorShape::UniversalNo },
        { IDC_HELP,       MUI::InputSystemCursorShape::Help },
        { IDC_UPARROW,    MUI::InputSystemCursorShape::UpArrow },
    };

    for ( const auto& entry : s_map )
    {
        if ( hcursor == ::LoadCursorW(nullptr, entry.id) )
            return MUI::InputSystemCursor::Create(entry.shape);
    }

    return nullptr;
}

} // anonymous namespace

// ============================================================================
// wxWinUISlot
// ============================================================================

void wxWinUISlot::SetContent(const winrt::Microsoft::UI::Xaml::UIElement& element)
{
    if ( !m_container )
        return;

    try
    {
        m_container.Children().Clear();
        m_content = element;
        if ( element )
            m_container.Children().Append(element);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("slot content swap", e);
    }
}

// ============================================================================
// wxWinUITopLevelHost: lifetime
// ============================================================================

wxWinUITopLevelHost *
wxWinUITopLevelHost::ForWindow(wxWindow *window, bool createIfNeeded)
{
    if ( !window )
        return nullptr;

    wxWindow * const tlw = wxGetTopLevelParent(window);
    if ( !tlw || !tlw->GetHandle() )
        return nullptr;

    const auto it = gs_tlwHosts.find(tlw);
    if ( it != gs_tlwHosts.end() )
        return it->second;

    if ( !createIfNeeded )
        return nullptr;

    wxWinUITopLevelHost * const host = new wxWinUITopLevelHost(tlw);
    if ( !host->Initialize() )
    {
        delete host;
        return nullptr;
    }

    gs_tlwHosts[tlw] = host;
    return host;
}

wxWinUITopLevelHost *wxWinUITopLevelHost::FindForTLW(const wxWindow *tlw)
{
    const auto it = gs_tlwHosts.find(tlw);
    return it != gs_tlwHosts.end() ? it->second : nullptr;
}

wxWinUITopLevelHost *wxWinUITopLevelHost::FindForBridge(HWND hwnd)
{
    if ( !hwnd )
        return nullptr;

    for ( const auto& kv : gs_tlwHosts )
    {
        if ( kv.second->m_bridge == hwnd || kv.second->m_inner == hwnd )
            return kv.second;
    }

    return nullptr;
}

wxWinUITopLevelHost::wxWinUITopLevelHost(wxWindow *tlw)
    : m_tlw(tlw)
{
}

wxWinUITopLevelHost::~wxWinUITopLevelHost()
{
    if ( m_source )
        Shutdown();
}

bool wxWinUITopLevelHost::Initialize()
{
    if ( !m_tlw || !wxWinUI3Initialize() )
        return false;

    try
    {
        using namespace winrt::Microsoft::UI;
        using namespace winrt::Microsoft::UI::Content;
        using namespace winrt::Microsoft::UI::Xaml;
        using namespace winrt::Microsoft::UI::Xaml::Controls;
        using namespace winrt::Microsoft::UI::Xaml::Hosting;

        const HWND hwndTLW = GetHwndOf(m_tlw);

        m_source = DesktopWindowXamlSource();
        m_source.Initialize(GetWindowIdFromWindow(hwndTLW));
        m_source.SiteBridge().ResizePolicy(
            ContentSizePolicy::ResizeContentToParentWindow);

        m_bridge = GetWindowFromWindowId(m_source.SiteBridge().WindowId());
        ::SetWindowLongPtr
        (
            m_bridge,
            GWL_STYLE,
            ::GetWindowLongPtr(m_bridge, GWL_STYLE) |
                WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
        );

        // The composition band renders relative to the siblings' GDI in HWND
        // z-order: the bridge must live at the top to be visible at all.
        ::SetWindowPos(m_bridge, HWND_TOP, 0, 0, 0, 0,
                       SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                       SWP_SHOWWINDOW);

        ::SetWindowSubclass(m_bridge, BridgeSubclassProc,
                            wxWINUI_TLW_SUBCLASS_BRIDGE,
                            reinterpret_cast<DWORD_PTR>(this));

        auto rootImpl = winrt::make_self<wxWinUIRootCanvas>();
        gs_rootCanvases[this] = rootImpl;
        m_root = rootImpl.as<Canvas>();
        m_root.RequestedTheme(wxWinUIGetCurrentElementTheme());

        // The root paints nothing (Transparent) so the wx children's GDI
        // shows through wherever XAML draws nothing -- but unlike a null
        // background it IS hit-testable, so the island receives ALL the
        // input.  The root router below then forwards real native messages
        // wherever no actual XAML content is hit: relying on WM_NCHITTEST
        // pass-through instead proved non-deterministic, the pointer
        // pipeline does not reliably continue to the windows below.
        m_root.Background(winrt::Microsoft::UI::Xaml::Media::SolidColorBrush(
            winrt::Microsoft::UI::Colors::Transparent()));
        // No SystemBackdrop.

        m_source.Content(m_root);

        if ( !m_root.XamlRoot() )
        {
            wxWinUIDebugLog("wxWinUITopLevelHost: no XamlRoot after Content()");
            Shutdown();
            return false;
        }

        m_takeFocusToken = m_source.TakeFocusRequested(
            [this](DesktopWindowXamlSource const&,
                   DesktopWindowXamlSourceTakeFocusRequestedEventArgs const& args)
            {
                const bool previous =
                    args.Request().Reason() ==
                        XamlSourceFocusNavigationReason::Last;
                OnTakeFocusRequested(previous);
            });

        BindRootRouter();
        EnsureInnerSubclass();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("TLW island initialization", e);
        Shutdown();
        return false;
    }

    // Every TLW that hosts WinUI content gets its backdrop/theme applied at
    // host creation: dialogs created after startup never got it before (the
    // only caller was the app-theme broadcast over the windows alive at that
    // moment), so they showed the stock light background with no dark title
    // bar and mismatched control fills.
    wxWinUIApplyWindowBackdrop(m_tlw);

    m_tlw->Bind(wxEVT_SIZE, &wxWinUITopLevelHost::OnTLWSize, this);
    m_tlw->Bind(wxEVT_DPI_CHANGED, &wxWinUITopLevelHost::OnTLWDpiChanged, this);
    m_tlw->Bind(wxEVT_DESTROY, &wxWinUITopLevelHost::OnTLWDestroy, this);

    wxWinUIDebugLog("wxWinUITopLevelHost: island created for TLW %p (bridge %p)",
                    static_cast<void *>(m_tlw), static_cast<void *>(m_bridge));
    return true;
}

void wxWinUITopLevelHost::Shutdown()
{
    if ( m_shuttingDown )
        return;
    m_shuttingDown = true;

    // Phase 1 -- before Close(): cut every path from a XAML callback back
    // to wx.  Closing the source fires teardown events (Unloaded,
    // LostFocus...) on the tree it still holds, and those must find revoked
    // handlers and invalidated state, never a window already in
    // destruction.  The containers deliberately stay attached: the source
    // must be closed with its tree in place (see below), so the visual
    // release only happens in phase 2.
    for ( auto& kv : m_slots )
    {
        UnbindSlotWindow(*kv.second);
        RevokeSlotCallbacks(*kv.second);
    }
    m_dirty.clear();

    // Every per-slot reference was released above; whatever remains (there
    // should be nothing) is dropped without unbinding dead windows.
    m_ancestorRefs.clear();

    // When the shutdown is triggered by the TLW's own destruction we are
    // being called from inside the dispatch of its wxEVT_DESTROY: unbinding
    // there would mutate the very dynamic-event table wx is iterating (a
    // crash).  The bindings go away with the window anyway, so skip them.
    if ( m_tlw && !m_tlw->IsBeingDeleted() )
    {
        m_tlw->Unbind(wxEVT_SIZE, &wxWinUITopLevelHost::OnTLWSize, this);
        m_tlw->Unbind(wxEVT_DPI_CHANGED, &wxWinUITopLevelHost::OnTLWDpiChanged, this);
        m_tlw->Unbind(wxEVT_DESTROY, &wxWinUITopLevelHost::OnTLWDestroy, this);
    }

    if ( m_inner )
    {
        ::RemoveWindowSubclass(m_inner, BridgeSubclassProc,
                               wxWINUI_TLW_SUBCLASS_INNER);
        m_inner = nullptr;
    }
    if ( m_bridge )
    {
        ::RemoveWindowSubclass(m_bridge, BridgeSubclassProc,
                               wxWINUI_TLW_SUBCLASS_BRIDGE);
        m_bridge = nullptr;
    }

    if ( m_source )
    {
        try
        {
            if ( m_takeFocusToken.value )
            {
                m_source.TakeFocusRequested(m_takeFocusToken);
                m_takeFocusToken = {};
            }
        }
        catch ( const winrt::hresult_error& )
        {
        }

        try
        {
            // Deliberately don't clear the Content first: closing the source
            // with the content still attached is the teardown order that
            // doesn't crash inside XAML (same rule as wxWinUIControlHost).
            m_source.Close();
        }
        catch ( const winrt::hresult_error& )
        {
        }
        m_source = nullptr;
    }

    // Phase 2 -- after Close(): the tree is dead, release our references.
    for ( auto& kv : m_slots )
    {
        gs_slotGrids.erase(kv.second);
        delete kv.second;
    }
    m_slots.clear();

    m_root = nullptr;
    gs_rootCanvases.erase(this);

    gs_tlwHosts.erase(m_tlw);
}

// ============================================================================
// slots
// ============================================================================

unsigned wxWinUISlotLifetime::ms_liveCount = 0;

unsigned wxWinUITopLevelHost::GetLiveHostCount()
{
    return static_cast<unsigned>(gs_tlwHosts.size());
}

unsigned wxWinUITopLevelHost::GetLiveSlotCount()
{
    unsigned count = 0;
    for ( const auto& kv : gs_tlwHosts )
        count += static_cast<unsigned>(kv.second->m_slots.size());
    return count;
}

unsigned wxWinUITopLevelHost::GetSlotHandlerAddCount()
{
    return gs_slotHandlerAdds;
}

unsigned wxWinUITopLevelHost::GetSlotHandlerRevokeCount()
{
    return gs_slotHandlerRevokes;
}

unsigned wxWinUITopLevelHost::GetFlushScheduleCount()
{
    return gs_flushSchedules;
}

unsigned wxWinUITopLevelHost::GetFlushRunCount()
{
    return gs_flushRuns;
}

wxWinUISlot *
wxWinUITopLevelHost::RegisterSlot(wxWindow *window,
                                  const winrt::Microsoft::UI::Xaml::UIElement& element)
{
    if ( !window || !m_root || m_shuttingDown )
        return nullptr;

    const auto it = m_slots.find(window);
    if ( it != m_slots.end() )
    {
        // Re-registration just swaps the content.
        it->second->SetContent(element);
        wxWinUIApplyAutomationName(window, element);
        MarkDirty(window);
        return it->second;
    }

    wxWinUISlot * const slot = new wxWinUISlot;
    slot->m_window = window;
    slot->m_hwnd = window->GetHWND();
    slot->m_lifetime = std::make_shared<wxWinUISlotLifetime>(window, this);

    try
    {
        auto gridImpl = winrt::make_self<wxWinUISlotGrid>();
        gs_slotGrids[slot] = gridImpl;
        slot->m_container =
            gridImpl.as<winrt::Microsoft::UI::Xaml::Controls::Grid>();
        if ( element )
        {
            slot->m_container.Children().Append(element);
            slot->m_content = element;
        }

        // Until the first sync runs the container must not cover the island
        // origin, where it would eat input meant for whatever lives there.
        slot->m_container.Visibility(
            winrt::Microsoft::UI::Xaml::Visibility::Collapsed);

        // Slot positions are physical (already mirrored by wx for RTL), so
        // the root Canvas stays LTR; only the hosted element's own content
        // mirrors, per window.
        if ( window->GetLayoutDirection() == wxLayout_RightToLeft )
        {
            slot->m_container.FlowDirection(
                winrt::Microsoft::UI::Xaml::FlowDirection::RightToLeft);
        }

        m_root.Children().Append(slot->m_container);

        // Registration and event binding belong to the same transaction:
        // AddHandler()/GotFocus()/LostFocus() can throw halfway through and
        // a slot must never stay half-registered.
        m_slots[window] = slot;
        BindSlotWindow(*slot);
        BindSlotEvents(*slot);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("slot registration", e);
        // Same rollback as every other failure/removal path: revoke
        // whatever was already attached, then drop the slot.
        m_slots.erase(window);
        m_dirty.erase(window);
        UnbindSlotWindow(*slot);
        DisconnectSlot(*slot);
        delete slot;
        return nullptr;
    }

    wxWinUIApplyAutomationName(window, element);

    m_structureDirty = true;
    MarkDirty(window);

    return slot;
}

void wxWinUITopLevelHost::RevokeSlotCallbacks(wxWinUISlot& slot)
{
    // 1. No callback may reach wx through this slot any more, even one
    //    already queued in the dispatcher.
    if ( slot.m_lifetime )
        slot.m_lifetime->Invalidate();

    if ( !slot.m_container )
        return;

    // 2. Revoke every handler so the container stops referencing the
    //    lambdas (and through them the shared state).
    for ( auto& handler : slot.m_routedHandlers )
    {
        try
        {
            slot.m_container.RemoveHandler(handler.first, handler.second);
            ++gs_slotHandlerRevokes;
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }
    slot.m_routedHandlers.clear();

    try
    {
        if ( slot.m_gotFocusToken )
        {
            slot.m_container.GotFocus(slot.m_gotFocusToken);
            ++gs_slotHandlerRevokes;
        }
        if ( slot.m_lostFocusToken )
        {
            slot.m_container.LostFocus(slot.m_lostFocusToken);
            ++gs_slotHandlerRevokes;
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }
    slot.m_gotFocusToken = {};
    slot.m_lostFocusToken = {};
}

void wxWinUITopLevelHost::DisconnectSlot(wxWinUISlot& slot)
{
    RevokeSlotCallbacks(slot);

    if ( slot.m_container )
    {
        // 3. Detach the hosted element (it belongs to the control, not to
        //    the slot) ...
        try
        {
            slot.m_container.Children().Clear();
        }
        catch ( const winrt::hresult_error& )
        {
        }
        slot.m_content = nullptr;

        // 4. ... and drop the container from the shared tree.
        if ( m_root )
        {
            try
            {
                uint32_t index = 0;
                if ( m_root.Children().IndexOf(slot.m_container, index) )
                    m_root.Children().RemoveAt(index);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException("slot removal", e);
            }
        }
    }

    gs_slotGrids.erase(&slot);
}

void wxWinUITopLevelHost::UnregisterSlot(wxWindow *window)
{
    const auto it = m_slots.find(window);
    if ( it == m_slots.end() )
        return;

    wxWinUISlot * const slot = it->second;

    UnbindSlotWindow(*slot);
    m_dirty.erase(window);
    m_slots.erase(it);

    DisconnectSlot(*slot);
    delete slot;
}

wxWinUISlot *wxWinUITopLevelHost::FindSlot(const wxWindow *window) const
{
    const auto it = m_slots.find(const_cast<wxWindow *>(window));
    return it != m_slots.end() ? it->second : nullptr;
}

void wxWinUITopLevelHost::SetSlotClipHeight(wxWindow *window, int physicalHeight)
{
    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot )
        return;

    const int clip = physicalHeight > 0 ? physicalHeight : -1;
    if ( slot->m_clipHeightPx == clip )
        return;

    slot->m_clipHeightPx = clip;
    MarkDirty(window);
}

void wxWinUITopLevelHost::SetSlotCursor(
    wxWindow *window,
    const winrt::Microsoft::UI::Input::InputCursor& cursor)
{
    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot )
        return;

    const auto it = gs_slotGrids.find(slot);
    if ( it != gs_slotGrids.end() )
        it->second->SetWindowCursor(cursor);
}

void wxWinUITopLevelHost::MirrorNativeCursor(HWND target, LRESULT hitTest)
{
    if ( target == m_cursorTarget && hitTest == m_cursorHit && m_cursorMirrored )
        return;

    m_cursorTarget = target;
    m_cursorHit = hitTest;

    // Let the target run its WM_SETCURSOR protocol (wx windows call
    // ::SetCursor from it), then mirror whatever cursor it installed.
    ::SendMessage(target, WM_SETCURSOR, reinterpret_cast<WPARAM>(target),
                  MAKELPARAM(hitTest, WM_MOUSEMOVE));

    const auto it = gs_rootCanvases.find(this);
    if ( it == gs_rootCanvases.end() )
        return;

    it->second->SetPointerCursor(wxWinUICursorFromHCURSOR(::GetCursor()));
    m_cursorMirrored = true;
}

void wxWinUITopLevelHost::ResetRootCursor()
{
    if ( !m_cursorMirrored )
        return;

    m_cursorMirrored = false;
    m_cursorTarget = nullptr;
    m_cursorHit = 0;

    const auto it = gs_rootCanvases.find(this);
    if ( it != gs_rootCanvases.end() )
        it->second->SetPointerCursor(nullptr);
}

void wxWinUITopLevelHost::CancelIslandPointerState()
{
    // Release any XAML pointer capture the root grabbed for this gesture...
    if ( m_root )
    {
        try
        {
            m_root.ReleasePointerCaptures();
        }
        catch ( const winrt::hresult_error& )
        {
        }
    }

    // ...and post WM_CANCELMODE to the island's internal input window (and
    // the bridge) so its input site abandons the in-flight pointer
    // interaction.  Posted, not sent: it must run AFTER the native message
    // just forwarded -- for a caption/border hit that is after the modal
    // move/size loop the forwarded WM_NC*BUTTONDOWN starts, which is exactly
    // when the phantom "pressed" state would otherwise take hold.  This is
    // the same signal the taskbar system menu sends to revive a stuck
    // window, issued automatically.
    if ( m_inner )
        ::PostMessage(m_inner, WM_CANCELMODE, 0, 0);
    if ( m_bridge )
        ::PostMessage(m_bridge, WM_CANCELMODE, 0, 0);

    if ( wxWinUIInputLogEnabled() )
    {
        wxWinUIInputLog("island CANCELMODE (inner=%p bridge=%p)",
                        static_cast<void *>(m_inner),
                        static_cast<void *>(m_bridge));
    }
}

void wxWinUITopLevelHost::FocusSlot(wxWindow *window)
{
    wxWinUISlot * const slot = FindSlot(window);
    if ( !slot )
        return;

    m_focusOwner = window;

    m_forwardingFocus = true;
    ::SetFocus(m_bridge);
    try
    {
        const auto control =
            slot->m_content.try_as<winrt::Microsoft::UI::Xaml::Controls::Control>();
        if ( control )
            control.Focus(winrt::Microsoft::UI::Xaml::FocusState::Programmatic);
    }
    catch ( const winrt::hresult_error& )
    {
    }
    m_forwardingFocus = false;
}

void wxWinUITopLevelHost::BindSlotWindow(wxWinUISlot& slot)
{
    wxWindow * const window = slot.m_window;

    window->Bind(wxEVT_MOVE, &wxWinUITopLevelHost::OnSlotWindowGeometry, this);
    window->Bind(wxEVT_SIZE, &wxWinUITopLevelHost::OnSlotWindowGeometry, this);
    window->Bind(wxEVT_SHOW, &wxWinUITopLevelHost::OnSlotWindowGeometry, this);
    window->Bind(wxEVT_DESTROY, &wxWinUITopLevelHost::OnSlotWindowDestroy, this);
    window->Bind(wxEVT_SET_FOCUS, &wxWinUITopLevelHost::OnSlotWindowFocus, this);

    // A child window gets NO event when one of its ancestors moves, resizes
    // or is shown/hidden (a notebook page switch, a sash drag...): listen on
    // the whole parent chain, with reference counting since chains overlap.
    slot.m_ancestors.clear();
    for ( wxWindow *p = window->GetParent();
          p && p != m_tlw;
          p = p->GetParent() )
    {
        slot.m_ancestors.push_back(p);
        AddAncestorRef(p);
    }
}

void wxWinUITopLevelHost::UnbindSlotWindow(wxWinUISlot& slot)
{
    wxWindow * const window = slot.m_window;

    window->Unbind(wxEVT_MOVE, &wxWinUITopLevelHost::OnSlotWindowGeometry, this);
    window->Unbind(wxEVT_SIZE, &wxWinUITopLevelHost::OnSlotWindowGeometry, this);
    window->Unbind(wxEVT_SHOW, &wxWinUITopLevelHost::OnSlotWindowGeometry, this);
    window->Unbind(wxEVT_DESTROY, &wxWinUITopLevelHost::OnSlotWindowDestroy, this);
    window->Unbind(wxEVT_SET_FOCUS, &wxWinUITopLevelHost::OnSlotWindowFocus, this);

    for ( wxWindow *ancestor : slot.m_ancestors )
        ReleaseAncestorRef(ancestor);
    slot.m_ancestors.clear();

    if ( m_focusOwner == window )
        m_focusOwner = nullptr;
}

void wxWinUITopLevelHost::AddAncestorRef(wxWindow *ancestor)
{
    if ( ++m_ancestorRefs[ancestor] > 1 )
        return;

    ancestor->Bind(wxEVT_MOVE, &wxWinUITopLevelHost::OnAncestorGeometry, this);
    ancestor->Bind(wxEVT_SIZE, &wxWinUITopLevelHost::OnAncestorGeometry, this);
    ancestor->Bind(wxEVT_SHOW, &wxWinUITopLevelHost::OnAncestorGeometry, this);
    ancestor->Bind(wxEVT_DESTROY, &wxWinUITopLevelHost::OnAncestorDestroy, this);
}

void wxWinUITopLevelHost::ReleaseAncestorRef(wxWindow *ancestor)
{
    const auto it = m_ancestorRefs.find(ancestor);
    if ( it == m_ancestorRefs.end() )
        return;     // already dropped by OnAncestorDestroy

    if ( --it->second > 0 )
        return;

    m_ancestorRefs.erase(it);

    ancestor->Unbind(wxEVT_MOVE, &wxWinUITopLevelHost::OnAncestorGeometry, this);
    ancestor->Unbind(wxEVT_SIZE, &wxWinUITopLevelHost::OnAncestorGeometry, this);
    ancestor->Unbind(wxEVT_SHOW, &wxWinUITopLevelHost::OnAncestorGeometry, this);
    ancestor->Unbind(wxEVT_DESTROY, &wxWinUITopLevelHost::OnAncestorDestroy, this);
}

void wxWinUITopLevelHost::OnAncestorGeometry(wxEvent& event)
{
    event.Skip();

    // Anything under it may have moved on screen or changed visibility: the
    // flush is coalesced, so just resync everything.
    MarkAllDirty();
}

void wxWinUITopLevelHost::OnAncestorDestroy(wxWindowDestroyEvent& event)
{
    event.Skip();

    wxWindow * const window = event.GetWindow();
    if ( window && m_ancestorRefs.erase(window) )
    {
        // The window is going away, no point unbinding from it; the slots
        // that referenced it are being destroyed too and their release will
        // find the entry already gone.
    }
}

// ============================================================================
// input synthesis + focus arbiter
// ============================================================================

void wxWinUITopLevelHost::SendSlotMouseEvent(
    wxWindow *window,
    wxEventType type,
    const winrt::Microsoft::UI::Xaml::Controls::Grid& container,
    const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args)
{
    if ( !window || !FindSlot(window) )
        return;

    wxMouseEvent event(type);
    event.SetEventObject(window);
    event.SetId(window->GetId());

    try
    {
        const auto point = args.GetCurrentPoint(container);
        const auto pos = point.Position();
        const wxPoint pt = window->FromDIP(
            wxPoint(static_cast<int>(std::lround(pos.X)),
                    static_cast<int>(std::lround(pos.Y))));
        event.m_x = pt.x;
        event.m_y = pt.y;

        const auto props = point.Properties();
        event.m_leftDown = props.IsLeftButtonPressed();
        event.m_middleDown = props.IsMiddleButtonPressed();
        event.m_rightDown = props.IsRightButtonPressed();

        if ( type == wxEVT_MOUSEWHEEL )
        {
            event.m_wheelRotation = props.MouseWheelDelta();
            event.m_wheelDelta = WHEEL_DELTA;
            event.m_linesPerAction = 3;
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }

    event.m_shiftDown = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
    event.m_controlDown = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
    event.m_altDown = (::GetKeyState(VK_MENU) & 0x8000) != 0;

    window->HandleWindowEvent(event);
}

void wxWinUITopLevelHost::BindSlotEvents(wxWinUISlot& slot)
{
    using namespace winrt::Microsoft::UI::Xaml;
    using winrt::Microsoft::UI::Input::PointerUpdateKind;
    using winrt::Windows::Foundation::IInspectable;

    // Weak-capture discipline: every handler captures only the shared
    // lifetime state (never raw wxWindow/host pointers, which a late
    // callback could dereference after free, and never the container, which
    // would create a Grid -> handler -> Grid retention cycle).  The
    // container comes from the sender argument instead.
    const std::shared_ptr<wxWinUISlotLifetime> state = slot.m_lifetime;

    // All the pointer handlers are attached with handledEventsToo=true: many
    // XAML controls (Button among them) mark the pointer events as handled,
    // which would starve a plain subscription -- and thus the wx application
    // -- of the very clicks it is most interested in.  None of the handlers
    // sets Handled itself, so the hosted control keeps reacting as before.
    // The boxed delegates are recorded on the slot so DisconnectSlot() can
    // revoke them (AddHandler has no token-based removal).
    const auto addPointerHandler =
        [&slot](const RoutedEvent& routedEvent,
                Input::PointerEventHandler handler)
        {
            const auto boxed = winrt::box_value(std::move(handler));
            slot.m_container.AddHandler(routedEvent, boxed,
                                        true /* handledEventsToo */);
            slot.m_routedHandlers.emplace_back(routedEvent, boxed);
            ++gs_slotHandlerAdds;
        };

    const auto sendFromSender =
        [state](IInspectable const& sender,
                Input::PointerRoutedEventArgs const& e,
                wxEventType type)
        {
            wxWinUITopLevelHost * const host = state->GetHost();
            wxWindow * const window = state->GetWindow();
            const auto container =
                sender.try_as<winrt::Microsoft::UI::Xaml::Controls::Grid>();
            if ( !host || !window || !container )
                return;

            host->SendSlotMouseEvent(window, type, container, e);
        };

    addPointerHandler(
        UIElement::PointerEnteredEvent(),
        [sendFromSender](IInspectable const& sender,
                         Input::PointerRoutedEventArgs const& e)
        {
            sendFromSender(sender, e, wxEVT_ENTER_WINDOW);
        });

    addPointerHandler(
        UIElement::PointerExitedEvent(),
        [sendFromSender](IInspectable const& sender,
                         Input::PointerRoutedEventArgs const& e)
        {
            sendFromSender(sender, e, wxEVT_LEAVE_WINDOW);
        });

    addPointerHandler(
        UIElement::PointerMovedEvent(),
        [sendFromSender](IInspectable const& sender,
                         Input::PointerRoutedEventArgs const& e)
        {
            sendFromSender(sender, e, wxEVT_MOTION);
        });

    addPointerHandler(
        UIElement::PointerWheelChangedEvent(),
        [state](IInspectable const& sender,
                Input::PointerRoutedEventArgs const& e)
        {
            wxWinUITopLevelHost * const host = state->GetHost();
            wxWindow * const window = state->GetWindow();
            const auto container =
                sender.try_as<winrt::Microsoft::UI::Xaml::Controls::Grid>();
            if ( !host || !window || !container )
                return;
            // The wheel is NOT synthesized as a wx event: the native message
            // is injected into the slot window instead, so that the whole
            // native chain runs -- DefWindowProc forwards an unhandled wheel
            // to the parent, which is exactly what makes a scrolled page
            // scroll while the pointer sits over one of its XAML-backed
            // children.  (A synthesized wxEVT_MOUSEWHEEL would dead-end on
            // the control.)  Legacy subclassers get the real message too.
            if ( !host->FindSlot(window) )
                return;

            const HWND hwnd = GetHwndOf(window);
            if ( !hwnd )
                return;

            try
            {
                const auto props = e.GetCurrentPoint(container).Properties();
                const int delta = props.MouseWheelDelta();
                if ( !delta )
                    return;

                WORD keys = 0;
                if ( ::GetKeyState(VK_SHIFT) & 0x8000 )
                    keys |= MK_SHIFT;
                if ( ::GetKeyState(VK_CONTROL) & 0x8000 )
                    keys |= MK_CONTROL;

                POINT pt;
                ::GetCursorPos(&pt);

                ::PostMessage(hwnd,
                              props.IsHorizontalMouseWheel() ? WM_MOUSEHWHEEL
                                                             : WM_MOUSEWHEEL,
                              MAKEWPARAM(keys,
                                         static_cast<WORD>(
                                             static_cast<short>(delta))),
                              MAKELPARAM(static_cast<short>(pt.x),
                                         static_cast<short>(pt.y)));
            }
            catch ( const winrt::hresult_error& )
            {
            }
        });

    addPointerHandler(
        UIElement::PointerPressedEvent(),
        [sendFromSender](IInspectable const& sender,
                         Input::PointerRoutedEventArgs const& e)
        {
            wxEventType type = wxEVT_NULL;
            try
            {
                const auto container =
                    sender.try_as<winrt::Microsoft::UI::Xaml::Controls::Grid>();
                if ( !container )
                    return;

                switch ( e.GetCurrentPoint(container).Properties()
                             .PointerUpdateKind() )
                {
                    case PointerUpdateKind::LeftButtonPressed:
                        type = wxEVT_LEFT_DOWN;
                        break;
                    case PointerUpdateKind::RightButtonPressed:
                        type = wxEVT_RIGHT_DOWN;
                        break;
                    case PointerUpdateKind::MiddleButtonPressed:
                        type = wxEVT_MIDDLE_DOWN;
                        break;
                    default:
                        break;
                }
            }
            catch ( const winrt::hresult_error& )
            {
            }

            if ( type != wxEVT_NULL )
                sendFromSender(sender, e, type);
        });

    addPointerHandler(
        UIElement::PointerReleasedEvent(),
        [sendFromSender](IInspectable const& sender,
                         Input::PointerRoutedEventArgs const& e)
        {
            wxEventType type = wxEVT_NULL;
            try
            {
                const auto container =
                    sender.try_as<winrt::Microsoft::UI::Xaml::Controls::Grid>();
                if ( !container )
                    return;

                switch ( e.GetCurrentPoint(container).Properties()
                             .PointerUpdateKind() )
                {
                    case PointerUpdateKind::LeftButtonReleased:
                        type = wxEVT_LEFT_UP;
                        break;
                    case PointerUpdateKind::RightButtonReleased:
                        type = wxEVT_RIGHT_UP;
                        break;
                    case PointerUpdateKind::MiddleButtonReleased:
                        type = wxEVT_MIDDLE_UP;
                        break;
                    default:
                        break;
                }
            }
            catch ( const winrt::hresult_error& )
            {
            }

            if ( type != wxEVT_NULL )
                sendFromSender(sender, e, type);
        });

    {
        const auto boxed = winrt::box_value(Input::DoubleTappedEventHandler(
        [state](IInspectable const& sender,
                Input::DoubleTappedRoutedEventArgs const& e)
        {
            wxWinUITopLevelHost * const host = state->GetHost();
            wxWindow * const window = state->GetWindow();
            const auto container =
                sender.try_as<winrt::Microsoft::UI::Xaml::Controls::Grid>();
            if ( !host || !window || !container || !host->FindSlot(window) )
                return;

            wxMouseEvent event(wxEVT_LEFT_DCLICK);
            event.SetEventObject(window);
            event.SetId(window->GetId());

            try
            {
                const auto pos = e.GetPosition(container);
                const wxPoint pt = window->FromDIP(
                    wxPoint(static_cast<int>(std::lround(pos.X)),
                            static_cast<int>(std::lround(pos.Y))));
                event.m_x = pt.x;
                event.m_y = pt.y;
                event.m_leftDown = true;
            }
            catch ( const winrt::hresult_error& )
            {
            }

            event.m_shiftDown = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
            event.m_controlDown = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
            event.m_altDown = (::GetKeyState(VK_MENU) & 0x8000) != 0;

            window->HandleWindowEvent(event);
        }));
        slot.m_container.AddHandler(UIElement::DoubleTappedEvent(), boxed,
                                    true /* handledEventsToo */);
        slot.m_routedHandlers.emplace_back(UIElement::DoubleTappedEvent(),
                                           boxed);
        ++gs_slotHandlerAdds;
    }

    // Focus arbiter, XAML -> wx side: the island holds the native focus, so
    // slotted windows never get WM_SETFOCUS; synthesize the wx focus events
    // and track the owner for FindFocus() resolution.
    slot.m_gotFocusToken = slot.m_container.GotFocus(
        [state](IInspectable const&, RoutedEventArgs const&)
        {
            wxWinUITopLevelHost * const host = state->GetHost();
            wxWindow * const window = state->GetWindow();
            if ( !host || !window || !host->FindSlot(window) )
                return;

            // GotFocus bubbles once per inner element: only the first
            // acquisition for this slot becomes a wx event.
            const bool wasOwner = host->m_focusOwner == window;
            host->m_focusOwner = window;

            if ( wasOwner || host->m_forwardingFocus )
                return;

            host->m_forwardingFocus = true;
            wxFocusEvent event(wxEVT_SET_FOCUS, window->GetId());
            event.SetEventObject(window);
            window->HandleWindowEvent(event);
            host->m_forwardingFocus = false;
        });
    ++gs_slotHandlerAdds;

    slot.m_lostFocusToken = slot.m_container.LostFocus(
        [state](IInspectable const&, RoutedEventArgs const&)
        {
            wxWinUITopLevelHost * const host = state->GetHost();
            wxWindow * const window = state->GetWindow();
            if ( !host || !window || !host->FindSlot(window) )
                return;

            if ( host->m_focusOwner == window )
                host->m_focusOwner = nullptr;

            if ( host->m_forwardingFocus )
                return;

            host->m_forwardingFocus = true;
            wxFocusEvent event(wxEVT_KILL_FOCUS, window->GetId());
            event.SetEventObject(window);
            window->HandleWindowEvent(event);
            host->m_forwardingFocus = false;
        });
    ++gs_slotHandlerAdds;
}

// ============================================================================
// root router: island-first input, native forwarding over non-XAML areas
// ============================================================================

void wxWinUITopLevelHost::BindRootRouter()
{
    using namespace winrt::Microsoft::UI::Xaml;
    using winrt::Windows::Foundation::IInspectable;

    const auto addHandler =
        [this](const RoutedEvent& routedEvent,
               Input::PointerEventHandler handler)
        {
            m_root.AddHandler(routedEvent,
                              winrt::box_value(std::move(handler)),
                              true /* handledEventsToo */);
        };

    addHandler(
        UIElement::PointerMovedEvent(),
        [this](IInspectable const&, Input::PointerRoutedEventArgs const& e)
        {
            OnRootPointer(WM_MOUSEMOVE, e);
        });

    addHandler(
        UIElement::PointerPressedEvent(),
        [this](IInspectable const&, Input::PointerRoutedEventArgs const& e)
        {
            OnRootPointer(WM_LBUTTONDOWN, e);
        });

    addHandler(
        UIElement::PointerReleasedEvent(),
        [this](IInspectable const&, Input::PointerRoutedEventArgs const& e)
        {
            OnRootPointer(WM_LBUTTONUP, e);
        });

    addHandler(
        UIElement::PointerWheelChangedEvent(),
        [this](IInspectable const&, Input::PointerRoutedEventArgs const& e)
        {
            OnRootPointer(WM_MOUSEWHEEL, e);
        });

    // Black-box-only transitions: these four are the signatures of a broken
    // input state (phantom capture, cancelled interaction), so always log
    // them when the black box is armed.
    if ( wxWinUIInputLogEnabled() )
    {
        addHandler(
            UIElement::PointerCaptureLostEvent(),
            [](IInspectable const&, Input::PointerRoutedEventArgs const&)
            {
                wxWinUIInputLog("root CAPTURE-LOST");
            });
        addHandler(
            UIElement::PointerCanceledEvent(),
            [](IInspectable const&, Input::PointerRoutedEventArgs const&)
            {
                wxWinUIInputLog("root POINTER-CANCELED");
            });
        // The frozen-window signature is an infinite ENTER/EXIT storm at a
        // stationary point: sample it and name the window under the cursor
        // at each transition so the oscillation partner is identified.
        static const auto logEnterExit = [](const char *what)
        {
            static unsigned s_count = 0;
            ++s_count;
            if ( (s_count % 64) != 1 )
                return;

            POINT pt = { 0, 0 };
            ::GetCursorPos(&pt);
            const HWND under = ::WindowFromPoint(pt);
            wchar_t cls[64] = L"";
            if ( under )
                ::GetClassNameW(under, cls, WXSIZEOF(cls));
            wxWinUIInputLog("root %s #%u under=%p '%S' pt=(%ld,%ld)",
                            what, s_count, static_cast<void *>(under),
                            cls, pt.x, pt.y);
        };
        addHandler(
            UIElement::PointerEnteredEvent(),
            [](IInspectable const&, Input::PointerRoutedEventArgs const&)
            {
                logEnterExit("ENTER");
            });
        addHandler(
            UIElement::PointerExitedEvent(),
            [](IInspectable const&, Input::PointerRoutedEventArgs const&)
            {
                logEnterExit("EXIT");
            });

        m_tlw->Bind(wxEVT_MOVE_START,
            [](wxMoveEvent& event)
            {
                wxWinUIInputLog("tlw ENTER-SIZEMOVE");
                event.Skip();
            });
        m_tlw->Bind(wxEVT_MOVE_END,
            [](wxMoveEvent& event)
            {
                wxWinUIInputLog("tlw EXIT-SIZEMOVE");
                event.Skip();
            });
        m_tlw->Bind(wxEVT_ACTIVATE,
            [](wxActivateEvent& event)
            {
                wxWinUIInputLog("tlw ACTIVATE=%d", event.GetActive());
                event.Skip();
            });
    }

    m_root.AddHandler(
        UIElement::DoubleTappedEvent(),
        winrt::box_value(Input::DoubleTappedEventHandler(
            [this](IInspectable const&,
                   Input::DoubleTappedRoutedEventArgs const& e)
            {
                if ( m_shuttingDown )
                    return;

                try
                {
                    const double scale = GetScale();
                    const auto pos = e.GetPosition(m_root);
                    const wxPoint ptPx(
                        static_cast<int>(std::lround(pos.X * scale)),
                        static_cast<int>(std::lround(pos.Y * scale)));

                    if ( PointOverXamlContent(ptPx) )
                        return;

                    const RECT rcCS = GetClientScreenRect();
                    const POINT ptScreen = { rcCS.left + ptPx.x,
                                             rcCS.top + ptPx.y };
                    const HWND target = FindNativeTargetAt(ptScreen);
                    if ( !target )
                        return;

                    POINT ptClient = ptScreen;
                    ::ScreenToClient(target, &ptClient);
                    ::PostMessage(target, WM_LBUTTONDBLCLK, MK_LBUTTON,
                                  MAKELPARAM(static_cast<short>(ptClient.x),
                                             static_cast<short>(ptClient.y)));
                }
                catch ( const winrt::hresult_error& )
                {
                }
            })),
        true /* handledEventsToo */);
}

void wxWinUITopLevelHost::OnRootPointer(
    UINT message,
    const winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args)
{
    if ( m_shuttingDown || !m_tlw )
        return;

    try
    {
        const auto point = args.GetCurrentPoint(m_root);
        const auto props = point.Properties();

        const double scale = GetScale();
        const auto pos = point.Position();
        const wxPoint ptPx(static_cast<int>(std::lround(pos.X * scale)),
                           static_cast<int>(std::lround(pos.Y * scale)));

        const RECT rcCS = GetClientScreenRect();
        const POINT ptScreen = { rcCS.left + ptPx.x, rcCS.top + ptPx.y };

        // Feed the spurious-WM_SETCURSOR filter (BridgeSubclassProc): any
        // WM_SETCURSOR arriving while the pointer still sits exactly here
        // was caused by window repositioning, not by pointer motion.
        m_lastPointerScreen = ptScreen;

        // Phantom-replay breaker.  The frozen-window signature (captured in
        // the black box): after a press whose capture was canceled ends
        // OUTSIDE the window (press near a border, fast swipe out, release
        // elsewhere), the input site replays one stale PointerMoved at a
        // fixed -- typically out-of-bounds -- position tens of thousands of
        // times per second, starving the thread and eating all input until
        // something sends it WM_CANCELMODE (the taskbar-menu recovery).  A
        // real mouse never delivers a run of moves at the EXACT same pixel
        // (the queue coalesces them), so a long identical-position run IS
        // the storm: break it ourselves with the same antidote, re-issued
        // rate-limited while it persists.
        if ( message == WM_MOUSEMOVE )
        {
            if ( ptPx == m_lastMovePx )
            {
                ++m_samePosMoves;
                if ( m_samePosMoves == 256 ||
                        (m_samePosMoves > 256 &&
                            (m_samePosMoves % 4096) == 0) )
                {
                    wxWinUIInputLog("phantom-replay storm at (%d,%d) x%u: "
                                    "breaking",
                                    ptPx.x, ptPx.y, m_samePosMoves);
                    CancelIslandPointerState();
                }
            }
            else
            {
                m_samePosMoves = 0;
                m_lastMovePx = ptPx;
            }

            // A repeated same-position move carries no new information for
            // anyone (the wx target already received that exact position);
            // skip the whole per-event work (hit-test included) so a storm
            // cannot saturate the thread while the breaker takes effect.
            if ( m_samePosMoves > 8 )
                return;
        }

        if ( wxWinUIInputLogEnabled() )
        {
            // Sample the moves (1/8) so a healthy stream stays readable but
            // its sudden absence is still unmistakable in the tail.
            static unsigned s_moveCount = 0;
            if ( message != WM_MOUSEMOVE || (++s_moveCount % 8) == 0 )
            {
                wxWinUIInputLog("root %s px=(%d,%d) pressed=%d",
                                message == WM_MOUSEMOVE ? "MOVE"
                                : message == WM_LBUTTONDOWN ? "PRESS"
                                : message == WM_LBUTTONUP ? "RELEASE"
                                : "WHEEL",
                                ptPx.x, ptPx.y,
                                props.IsLeftButtonPressed() ? 1 : 0);
            }
        }

        if ( PointOverXamlContent(ptPx) )
        {
            // Back over XAML: close the native hover if one was active and
            // give the cursor back to the XAML controls.
            if ( m_lastNativeTarget && message == WM_MOUSEMOVE )
            {
                ::PostMessage(m_lastNativeTarget, WM_MOUSELEAVE, 0, 0);
                m_lastNativeTarget = nullptr;
            }
            if ( message == WM_MOUSEMOVE )
                ResetRootCursor();
            return;
        }

        HWND target = FindNativeTargetAt(ptScreen);
        if ( !target )
            return;

        // Client or non-client area of the target?  A HTTRANSPARENT answer
        // (e.g. the dialog resize gripper) bubbles up the parent chain, as
        // the native hit-testing would.
        LRESULT ht = HTNOWHERE;
        for ( HWND probe = target; probe; probe = ::GetParent(probe) )
        {
            ht = ::SendMessage(
                probe, WM_NCHITTEST, 0,
                MAKELPARAM(static_cast<short>(ptScreen.x),
                           static_cast<short>(ptScreen.y)));
            if ( ht != HTTRANSPARENT )
            {
                target = probe;
                break;
            }
        }

        // Hover bookkeeping so wx enter/leave logic stays coherent.
        if ( message == WM_MOUSEMOVE && target != m_lastNativeTarget )
        {
            if ( m_lastNativeTarget )
                ::PostMessage(m_lastNativeTarget, WM_MOUSELEAVE, 0, 0);
            m_lastNativeTarget = target;
        }

        // Mirror the target's cursor onto the island, which otherwise paints
        // its own arrow over the whole client area (splitter sash arrows,
        // I-beams over wx-drawn text controls...).
        if ( message == WM_MOUSEMOVE )
            MirrorNativeCursor(target, ht);

        WPARAM keys = 0;
        if ( ::GetKeyState(VK_SHIFT) & 0x8000 )
            keys |= MK_SHIFT;
        if ( ::GetKeyState(VK_CONTROL) & 0x8000 )
            keys |= MK_CONTROL;
        if ( props.IsLeftButtonPressed() )
            keys |= MK_LBUTTON;
        if ( props.IsRightButtonPressed() )
            keys |= MK_RBUTTON;
        if ( props.IsMiddleButtonPressed() )
            keys |= MK_MBUTTON;

        POINT ptClient = ptScreen;
        ::ScreenToClient(target, &ptClient);
        const LPARAM lpClient = MAKELPARAM(static_cast<short>(ptClient.x),
                                           static_cast<short>(ptClient.y));
        const LPARAM lpScreen = MAKELPARAM(static_cast<short>(ptScreen.x),
                                           static_cast<short>(ptScreen.y));

        switch ( message )
        {
            case WM_MOUSEMOVE:
                if ( ht == HTCLIENT )
                    ::PostMessage(target, WM_MOUSEMOVE, keys, lpClient);
                else
                    ::PostMessage(target, WM_NCMOUSEMOVE,
                                  static_cast<WPARAM>(ht), lpScreen);
                break;

            case WM_MOUSEWHEEL:
            {
                const int delta = props.MouseWheelDelta();
                if ( !delta )
                    break;

                // The wheel is always a client message, carrying screen
                // coordinates; the target resolves it internally.
                ::PostMessage(target,
                              props.IsHorizontalMouseWheel() ? WM_MOUSEHWHEEL
                                                             : WM_MOUSEWHEEL,
                              MAKEWPARAM(keys,
                                         static_cast<WORD>(
                                             static_cast<short>(delta))),
                              lpScreen);
                break;
            }

            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            {
                // Identify which button actually changed.
                using winrt::Microsoft::UI::Input::PointerUpdateKind;
                UINT msgClient = 0, msgNC = 0;
                switch ( point.Properties().PointerUpdateKind() )
                {
                    case PointerUpdateKind::LeftButtonPressed:
                        msgClient = WM_LBUTTONDOWN; msgNC = WM_NCLBUTTONDOWN;
                        break;
                    case PointerUpdateKind::LeftButtonReleased:
                        msgClient = WM_LBUTTONUP; msgNC = WM_NCLBUTTONUP;
                        break;
                    case PointerUpdateKind::RightButtonPressed:
                        msgClient = WM_RBUTTONDOWN; msgNC = WM_NCRBUTTONDOWN;
                        break;
                    case PointerUpdateKind::RightButtonReleased:
                        msgClient = WM_RBUTTONUP; msgNC = WM_NCRBUTTONUP;
                        break;
                    case PointerUpdateKind::MiddleButtonPressed:
                        msgClient = WM_MBUTTONDOWN; msgNC = WM_NCMBUTTONDOWN;
                        break;
                    case PointerUpdateKind::MiddleButtonReleased:
                        msgClient = WM_MBUTTONUP; msgNC = WM_NCMBUTTONUP;
                        break;
                    default:
                        break;
                }
                if ( !msgClient )
                    break;

                // DOWNs are forwarded SYNCHRONOUSLY: the real button is
                // still physically pressed at this point, so a target that
                // starts a native tracking loop (scrollbar arrows/thumb,
                // splitter sash, anything calling SetCapture) captures the
                // mouse and takes over the REAL input stream from the island
                // until the release -- which is exactly the native
                // behaviour.  UPs are posted: with a capture active the real
                // release went to the target directly and no synthetic one
                // is generated here (the island never saw it).
                const bool isDown = msgClient == WM_LBUTTONDOWN ||
                                    msgClient == WM_RBUTTONDOWN ||
                                    msgClient == WM_MBUTTONDOWN;

                if ( wxWinUIInputLogEnabled() )
                {
                    wxWinUIInputLog("fwd %s target=%p ht=%d %s",
                                    isDown ? "DOWN" : "UP",
                                    static_cast<void *>(target),
                                    static_cast<int>(ht),
                                    ht == HTCLIENT ? "client" : "nc");
                }

                // Physical button backing this message, to detect targets
                // that ran a whole native tracking loop inside our
                // synchronous forward (the release then happened in there).
                int vkButton = VK_LBUTTON;
                if ( msgClient == WM_RBUTTONDOWN || msgClient == WM_RBUTTONUP )
                    vkButton = VK_RBUTTON;
                else if ( msgClient == WM_MBUTTONDOWN ||
                              msgClient == WM_MBUTTONUP )
                    vkButton = VK_MBUTTON;

                bool cancelIsland = false;

                if ( ht == HTCLIENT )
                {
                    if ( isDown )
                    {
                        const ULONGLONG t0 = ::GetTickCount64();
                        ::SendMessage(target, msgClient, keys, lpClient);

                        // The send ran long AND the button is already back
                        // up: the target ran a native tracking loop
                        // (scrollbar thumb, sash drag...) and consumed the
                        // release; the island will never see it.  A plain
                        // fast click returns immediately, so the duration
                        // check keeps legitimate press/release pairs intact.
                        cancelIsland =
                            ::GetTickCount64() - t0 > 100 &&
                                !(::GetAsyncKeyState(vkButton) & 0x8000);
                    }
                    else
                        ::PostMessage(target, msgClient, keys, lpClient);
                }
                else
                {
                    // Hit codes that start a SYSTEM-MODAL loop (move/size,
                    // caption buttons, system menu) must NEVER be forwarded
                    // synchronously: DefWindowProc runs the whole modal loop
                    // before SendMessage returns, so the island sits inside
                    // this pointer callback for its entire duration, misses
                    // the release and keeps a phantom "pressed" state that
                    // eats all further input (rapid near-miss resize clicks
                    // froze the window; WM_CANCELMODE was the only cure).
                    // Scrollbar tracking et al. stay synchronous: those
                    // targets SetCapture and take over the real stream,
                    // which is the native behaviour.
                    bool modalLoopHt = false;
                    switch ( ht )
                    {
                        case HTCAPTION:
                        case HTSYSMENU:
                        case HTLEFT:
                        case HTRIGHT:
                        case HTTOP:
                        case HTBOTTOM:
                        case HTTOPLEFT:
                        case HTTOPRIGHT:
                        case HTBOTTOMLEFT:
                        case HTBOTTOMRIGHT:
                        case HTMINBUTTON:
                        case HTMAXBUTTON:
                        case HTCLOSE:
                            modalLoopHt = true;
                            break;
                    }

                    if ( isDown && !modalLoopHt )
                    {
                        const ULONGLONG t0 = ::GetTickCount64();
                        ::SendMessage(target, msgNC,
                                      static_cast<WPARAM>(ht), lpScreen);
                        // Same tracking-loop detection as the client branch.
                        cancelIsland =
                            ::GetTickCount64() - t0 > 100 &&
                                !(::GetAsyncKeyState(vkButton) & 0x8000);
                    }
                    else
                    {
                        // The island's input window holds the Win32 capture
                        // for the duration of the press: without releasing
                        // it the move/size loop never sees the mouse (no
                        // resize happens and the modal state hangs again).
                        if ( isDown && modalLoopHt )
                        {
                            wxWinUIInputLog("modal-ht %d: ReleaseCapture+post",
                                            static_cast<int>(ht));
                            ::ReleaseCapture();

                            // The posted WM_NC*BUTTONDOWN is about to start
                            // a system-modal move/size loop that eats the
                            // real input stream up to and including the
                            // release: the island will never receive the
                            // PointerReleased matching the PointerPressed it
                            // just delivered and its input site would stay
                            // stuck "pressed", silently dropping all later
                            // pointer events (the rapid near-border resize
                            // freeze; the taskbar-menu WM_CANCELMODE was the
                            // only cure).  Cancel that in-flight interaction
                            // ourselves, right behind the forwarded press.
                            cancelIsland = true;
                        }
                        ::PostMessage(target, msgNC,
                                      static_cast<WPARAM>(ht), lpScreen);
                    }
                }

                if ( cancelIsland )
                    CancelIslandPointerState();
                break;
            }
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }
}

bool wxWinUITopLevelHost::PointOverXamlContent(const wxPoint& ptClientPx)
{
    const auto xamlRoot = GetXamlRoot();
    if ( !xamlRoot )
        return false;

    try
    {
        using winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper;

        const double scale = GetScale();
        const winrt::Windows::Foundation::Point dip
        {
            float(ptClientPx.x / scale),
            float(ptClientPx.y / scale)
        };

        // Note: there is deliberately NO "any open popup makes the island
        // solid everywhere" shortcut here.  A ContentDialog's smoke layer
        // and a flyout's content ARE hit-testable elements found by the
        // query below, so modality works through the normal path -- while
        // the shortcut turned a single stale popup (e.g. a flyout whose
        // closing was interrupted by a modal loop) into a permanently
        // input-dead window.
        //
        // The root canvas itself is hit-testable by design (it must receive
        // all the input): only count REAL content above it.  The query runs
        // against the XamlRoot content so popup layers are part of the walk.
        auto scope = xamlRoot.Content();
        if ( !scope )
            scope = m_root;
        const auto hits =
            VisualTreeHelper::FindElementsInHostCoordinates(dip, scope);
        for ( const auto& element : hits )
        {
            if ( element != m_root )
                return true;
        }

        return false;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

HWND wxWinUITopLevelHost::FindNativeTargetAt(const POINT& ptScreen) const
{
    if ( !m_tlw )
        return nullptr;

    // Deepest visible wx window at the point, skipping the island windows.
    HWND current = GetHwndOf(m_tlw);
    if ( !current )
        return nullptr;

    for ( ;; )
    {
        HWND matched = nullptr;

        for ( HWND child = ::GetWindow(current, GW_CHILD);
              child;
              child = ::GetWindow(child, GW_HWNDNEXT) )
        {
            if ( child == m_bridge || child == m_inner )
                continue;
            if ( !::IsWindowVisible(child) )
                continue;
            if ( ::GetWindowLongPtr(child, GWL_EXSTYLE) & WS_EX_TRANSPARENT )
                continue;

            RECT rect;
            if ( !::GetWindowRect(child, &rect) )
                continue;
            if ( !::PtInRect(&rect, ptScreen) )
                continue;

            // A disabled window doesn't receive input, its parent does.
            if ( !::IsWindowEnabled(child) )
            {
                matched = nullptr;
                break;
            }

            matched = child;
            break;
        }

        if ( !matched )
            return current;

        current = matched;
    }
}

void wxWinUITopLevelHost::OnSlotWindowFocus(wxFocusEvent& event)
{
    event.Skip();

    if ( m_forwardingFocus || m_shuttingDown )
        return;

    wxWindow * const window = wxDynamicCast(event.GetEventObject(), wxWindow);
    if ( !window )
        return;

    // wx gave the focus to the slotted control: push it into the island so
    // the XAML element gets the caret/keyboard.
    FocusSlot(window);
}

void wxWinUITopLevelHost::OnTakeFocusRequested(bool previous)
{
    // The island's own Tab navigation ran off its first/last element: hand
    // the focus to the neighbouring wx tab stop.
    wxWindow * const current = m_focusOwner;
    const HWND hwndCurrent = current ? GetHwndOf(current) : nullptr;
    if ( !hwndCurrent || !m_tlw )
        return;

    const HWND next =
        ::GetNextDlgTabItem(GetHwndOf(m_tlw), hwndCurrent, previous);
    if ( !next || next == hwndCurrent )
        return;

    wxWindow * const nextWin = wxGetWindowFromHWND((WXHWND)next);
    if ( nextWin && FindSlot(nextWin) )
        FocusSlot(nextWin);
    else
        ::SetFocus(next);
}

wxWindow *wxWinUITopLevelHost::ResolveFocusHwnd(WXHWND hwnd)
{
    const HWND focus = static_cast<HWND>(hwnd);
    if ( !focus )
        return nullptr;

    for ( const auto& kv : gs_tlwHosts )
    {
        wxWinUITopLevelHost * const host = kv.second;
        if ( !host->m_bridge )
            continue;

        if ( focus == host->m_bridge || focus == host->m_inner ||
                ::IsChild(host->m_bridge, focus) )
        {
            return host->m_focusOwner ? host->m_focusOwner : host->m_tlw;
        }
    }

    return nullptr;
}

wxWindow *wxWinUITLWHostResolveFocus(WXHWND hwnd)
{
    return wxWinUITopLevelHost::ResolveFocusHwnd(hwnd);
}

// ============================================================================
// wx event plumbing
// ============================================================================

void wxWinUITopLevelHost::OnSlotWindowGeometry(wxEvent& event)
{
    event.Skip();

    wxWindow * const window = wxDynamicCast(event.GetEventObject(), wxWindow);
    if ( window && m_slots.count(window) )
        MarkDirty(window);
}

void wxWinUITopLevelHost::OnSlotWindowDestroy(wxWindowDestroyEvent& event)
{
    event.Skip();

    // wxWindowDestroyEvent propagates upwards, so this may be about a child
    // of the window the handler is bound to: only act on tracked windows.
    wxWindow * const window = event.GetWindow();
    if ( window && m_slots.count(window) )
        UnregisterSlot(window);
}

void wxWinUITopLevelHost::OnTLWSize(wxSizeEvent& event)
{
    event.Skip();
    MarkAllDirty();
}

void wxWinUITopLevelHost::OnTLWDpiChanged(wxDPIChangedEvent& event)
{
    event.Skip();

    // The island's RasterizationScale follows the monitor by itself; the
    // slot geometry (positions, sizes, clips, scrollbar cutouts) has to be
    // recomputed with the new scale.
    MarkAllDirty();
}

void wxWinUITopLevelHost::OnTLWDestroy(wxWindowDestroyEvent& event)
{
    event.Skip();

    if ( event.GetWindow() != m_tlw )
        return;

    Shutdown();

    // Never delete the host from inside the dispatch of its own TLW's
    // wxEVT_DESTROY: the event machinery is still iterating that window's
    // handler table and would touch this object (and its functors) after
    // the free.  Defer the delete to the next idle turn -- Shutdown() has
    // already unregistered the host from every global map, so nothing can
    // reach it in the meantime.
    wxWinUITopLevelHost * const self = this;
    wxTheApp->CallAfter([self]() { delete self; });
}

// ============================================================================
// geometry sync
// ============================================================================

// Called from wxTopLevelWindowMSW::DoThaw(): run the flush the freeze held
// back.  The notification must come from the TLW override, not from
// wxWindowMSW::DoThaw(): the freeze is detected at the TLW level, and a TLW
// may thaw without any child running the base DoThaw().
void wxWinUITLWHostNotifyThaw(wxWindow *window)
{
    if ( !window )
        return;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindForTLW(wxGetTopLevelParent(window));
    if ( !host || !host->m_frozenDirty )
        return;

    host->m_frozenDirty = false;
    host->MarkAllDirty();
}

void wxWinUITopLevelHost::MarkDirty(wxWindow *window)
{
    if ( m_shuttingDown )
        return;

    m_dirty.insert(window);
    ScheduleFlush();
}

void wxWinUITopLevelHost::MarkAllDirty()
{
    if ( m_shuttingDown )
        return;

    m_allDirty = true;
    ScheduleFlush();
}

void wxWinUITopLevelHost::ScheduleFlush()
{
    if ( m_flushScheduled )
        return;

    // While the TLW is frozen, schedule nothing at all: each queued flush
    // would just see the freeze and bail out, so a Move/Size storm under
    // Freeze() would keep posting one CallAfter per event.  Note that a
    // catch-up flush is owed instead; wxWinUITLWHostNotifyThaw() -- called
    // from wxTopLevelWindowMSW::DoThaw() -- schedules that single flush at
    // thaw time.
    if ( m_tlw && m_tlw->IsFrozen() )
    {
        m_frozenDirty = true;
        return;
    }

    m_flushScheduled = true;
    ++gs_flushSchedules;
    CallAfter(&wxWinUITopLevelHost::FlushSync);
}

void wxWinUITopLevelHost::FlushSync()
{
    m_flushScheduled = false;

    if ( m_shuttingDown || !m_source )
        return;

    if ( wxWinUIInputLogEnabled() )
    {
        // Sampled: a flush STORM (self-rescheduling loop) must be visible in
        // the black box without drowning it.
        static unsigned s_flushCount = 0;
        if ( (++s_flushCount % 32) == 1 )
            wxWinUIInputLog("flush #%u", s_flushCount);
    }

    if ( m_tlw->IsFrozen() )
    {
        // This flush was scheduled before the freeze started (new ones are
        // not posted while frozen, see ScheduleFlush): remember that a
        // catch-up is owed and let wxWinUITLWHostNotifyThaw() schedule it.
        m_frozenDirty = true;
        return;
    }

    ++gs_flushRuns;

    EnsureInnerSubclass();
    EnsureBridgeOnTop();
    RebuildBridgeRegion();

    // Windows reparented into another TLW migrate after the sweep.
    std::vector<wxWindow *> migrating;

    const auto syncOne = [&](wxWinUISlot *slot)
    {
        wxWindow * const w = slot->m_window;
        if ( !w->GetHandle() )
            return;

        if ( wxGetTopLevelParent(w) != m_tlw )
        {
            migrating.push_back(w);
            return;
        }

        SyncSlot(*slot);
    };

    if ( m_allDirty )
    {
        for ( auto& kv : m_slots )
            syncOne(kv.second);
    }
    else
    {
        for ( wxWindow *w : m_dirty )
        {
            wxWinUISlot * const slot = FindSlot(w);
            if ( slot )
                syncOne(slot);
        }
    }

    m_allDirty = false;
    m_dirty.clear();

    if ( m_structureDirty )
    {
        RecomputeZOrder();
        m_structureDirty = false;
    }

    for ( wxWindow *w : migrating )
    {
        wxWinUISlot * const slot = FindSlot(w);
        if ( !slot )
            continue;

        const auto element = slot->m_content;
        UnregisterSlot(w);

        wxWinUITopLevelHost * const other = ForWindow(w, true);
        if ( other )
            other->RegisterSlot(w, element);
    }
}

void wxWinUITopLevelHost::SyncSlot(wxWinUISlot& slot)
{
    wxWindow * const w = slot.m_window;
    const HWND hwnd = GetHwndOf(w);
    if ( !hwnd || !slot.m_container )
        return;

    slot.m_hwnd = w->GetHWND();

    const HWND hwndTLW = GetHwndOf(m_tlw);

    // Control rect in TLW client coordinates (physical px) -- the island
    // covers exactly the TLW client area.  The TLW itself can be slotted too
    // (the Window-presentation dialog shell hosts its whole content this
    // way): its slot simply covers the client area.
    wxRect rect;
    if ( w == m_tlw )
    {
        RECT rc;
        ::GetClientRect(hwndTLW, &rc);
        rect = wxRect(0, 0, rc.right - rc.left, rc.bottom - rc.top);
    }
    else
    {
        // All island geometry is physical-LTR: window rects are physical
        // already, so anchor them on the physical client origin.  (See
        // GetClientScreenRect for why ScreenToClient must not be used.)
        const RECT rcCS = GetClientScreenRect();
        RECT wr;
        ::GetWindowRect(hwnd, &wr);
        rect = wxRect(wr.left - rcCS.left, wr.top - rcCS.top,
                      wr.right - wr.left, wr.bottom - wr.top);
    }

    // Clip against every ancestor's client area, like HWND clipping does.
    wxRect vis = rect;

    if ( slot.m_clipHeightPx >= 0 )
    {
        wxRect limit = rect;
        limit.height = std::min(rect.height, slot.m_clipHeightPx);
        vis.Intersect(limit);
    }
    if ( w != m_tlw )
    {
        const RECT rcCS = GetClientScreenRect();
        for ( wxWindow *p = w->GetParent();
              p && p != m_tlw && !vis.IsEmpty();
              p = p->GetParent() )
        {
            const HWND hp = GetHwndOf(p);
            if ( !hp )
                break;

            RECT rc;
            ::GetClientRect(hp, &rc);
            ::MapWindowPoints(hp, nullptr, reinterpret_cast<POINT *>(&rc), 2);
            if ( rc.left > rc.right )
            {
                const LONG swap = rc.left;
                rc.left = rc.right;
                rc.right = swap;
            }

            vis.Intersect(wxRect(rc.left - rcCS.left, rc.top - rcCS.top,
                                 rc.right - rc.left, rc.bottom - rc.top));
        }
    }

    try
    {
        using namespace winrt::Microsoft::UI::Xaml;
        using winrt::Microsoft::UI::Xaml::Controls::Canvas;

        const double scale = GetScale();

        Canvas::SetLeft(slot.m_container, rect.x / scale);
        Canvas::SetTop(slot.m_container, rect.y / scale);
        slot.m_container.Width(rect.width / scale);
        slot.m_container.Height(rect.height / scale);

        // Follow dynamic direction changes (SetLayoutDirection after
        // creation): the hosted content mirrors, the positions above stay
        // physical.
        const auto dir = w->GetLayoutDirection() == wxLayout_RightToLeft
            ? FlowDirection::RightToLeft
            : FlowDirection::LeftToRight;
        if ( slot.m_container.FlowDirection() != dir )
            slot.m_container.FlowDirection(dir);

        if ( vis == rect || vis.IsEmpty() )
        {
            slot.m_container.Clip(nullptr);
        }
        else
        {
            winrt::Microsoft::UI::Xaml::Media::RectangleGeometry geometry;
            geometry.Rect(winrt::Windows::Foundation::Rect
            {
                float((vis.x - rect.x) / scale),
                float((vis.y - rect.y) / scale),
                float(vis.width / scale),
                float(vis.height / scale)
            });
            slot.m_container.Clip(geometry);
        }

        const bool effectiveVisible = w->IsShownOnScreen() && !vis.IsEmpty();
        slot.m_container.Visibility(effectiveVisible ? Visibility::Visible
                                                     : Visibility::Collapsed);

        slot.m_rectPx = rect;
        slot.m_visible = effectiveVisible;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("slot geometry sync", e);
    }
}

void wxWinUITopLevelHost::RecomputeZOrder()
{
    // Mirror the wx paint order: pre-order traversal, siblings in creation
    // order.
    int z = 0;
    std::function<void (wxWindow *)> walk = [&](wxWindow *win)
    {
        for ( wxWindow *child : win->GetChildren() )
        {
            wxWinUISlot * const slot = FindSlot(child);
            if ( slot && slot->m_container )
            {
                try
                {
                    winrt::Microsoft::UI::Xaml::Controls::Canvas::SetZIndex(
                        slot->m_container, ++z);
                }
                catch ( const winrt::hresult_error& )
                {
                }
            }

            walk(child);
        }
    };

    walk(m_tlw);
}

// ============================================================================
// bridge plumbing
// ============================================================================

void wxWinUITopLevelHost::EnsureInnerSubclass()
{
    if ( m_inner || !m_bridge )
        return;

    m_inner = ::GetWindow(m_bridge, GW_CHILD);
    if ( m_inner )
    {
        ::SetWindowSubclass(m_inner, BridgeSubclassProc,
                            wxWINUI_TLW_SUBCLASS_INNER,
                            reinterpret_cast<DWORD_PTR>(this));

        // Null out the CLASS cursor of both island windows.  With a class
        // cursor installed, USER32 re-applies it directly (no WM_SETCURSOR
        // involved) whenever any window repositions under a resting pointer
        // -- our coalesced slot sync, a sizer relayout... -- wiping the
        // per-element cursor the island installed (the I-beam over a
        // TextBox held only while the mouse was moving).  A null class
        // cursor means "the application manages the cursor": the island
        // sets it on real pointer activity and nothing resets it in
        // between.
        ::SetClassLongPtrW(m_inner, GCLP_HCURSOR, 0);
        ::SetClassLongPtrW(m_bridge, GCLP_HCURSOR, 0);
    }
}

void wxWinUITopLevelHost::EnsureBridgeOnTop()
{
    if ( !m_bridge || !m_tlw )
        return;

    // wx children created after the bridge stack above it; re-pin (cheap,
    // and only when actually needed).
    const HWND top = ::GetWindow(GetHwndOf(m_tlw), GW_CHILD);
    if ( top == m_bridge )
        return;

    // A transient island (popup menu, overlay dialog, rich tooltip) pins its
    // own bridge on top for its lifetime: never leapfrog those.
    if ( top )
    {
        wchar_t cls[96] = L"";
        ::GetClassNameW(top, cls, WXSIZEOF(cls));
        if ( wxString(cls).Contains(wxS("DesktopChildSiteBridge")) )
            return;
    }

    if ( wxWinUIInputLogEnabled() )
    {
        wchar_t cls[64] = L"";
        if ( top )
            ::GetClassNameW(top, cls, WXSIZEOF(cls));
        wxWinUIInputLog("bridge REPIN over %p '%S'",
                        static_cast<void *>(top), cls);
    }

    ::SetWindowPos(m_bridge, HWND_TOP, 0, 0, 0, 0,
                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void wxWinUITopLevelHost::RebuildBridgeRegion()
{
    if ( !m_bridge || !m_tlw )
        return;

    const HWND hwndTLW = GetHwndOf(m_tlw);
    if ( !hwndTLW )
        return;

    RECT rcClient;
    if ( !::GetClientRect(hwndTLW, &rcClient) )
        return;

    // Collect the scrollbar bands first, so an unchanged set skips the
    // (redraw-forcing) SetWindowRgn call entirely.
    std::vector<RECT> cuts;
    cuts.push_back(rcClient);   // client size participates in the signature

    // The exact physical rect comes from GetScrollBarInfo: correct wherever
    // the bar actually is (left side under RTL layouts), correct under any
    // per-monitor DPI, and absent when the bar is currently hidden -- the
    // style bit alone says none of that.  Region coordinates are physical
    // and left-anchored even for mirrored windows, exactly like the island
    // geometry (see GetClientScreenRect).
    const RECT rcCS = GetClientScreenRect();
    const auto addBarCut = [&](HWND hwnd, LONG objId)
    {
        SCROLLBARINFO sbi = { sizeof(SCROLLBARINFO) };
        if ( !::GetScrollBarInfo(hwnd, objId, &sbi) )
            return;
        if ( sbi.rgstate[0] & (STATE_SYSTEM_INVISIBLE | STATE_SYSTEM_OFFSCREEN) )
            return;

        const RECT cut = { sbi.rcScrollBar.left - rcCS.left,
                           sbi.rcScrollBar.top - rcCS.top,
                           sbi.rcScrollBar.right - rcCS.left,
                           sbi.rcScrollBar.bottom - rcCS.top };
        cuts.push_back(cut);
    };

    std::function<void (wxWindow *)> walk = [&](wxWindow *win)
    {
        for ( wxWindow *child : win->GetChildren() )
        {
            const HWND hwnd = GetHwndOf(child);
            if ( hwnd && ::IsWindowVisible(hwnd) )
            {
                const LONG_PTR style = ::GetWindowLongPtr(hwnd, GWL_STYLE);
                if ( style & WS_VSCROLL )
                    addBarCut(hwnd, OBJID_VSCROLL);
                if ( style & WS_HSCROLL )
                    addBarCut(hwnd, OBJID_HSCROLL);
            }

            walk(child);
        }
    };
    walk(m_tlw);

    if ( cuts.size() == m_regionCuts.size() &&
             memcmp(cuts.data(), m_regionCuts.data(),
                    cuts.size() * sizeof(RECT)) == 0 )
        return;
    m_regionCuts = cuts;

    HRGN region = ::CreateRectRgn(0, 0, rcClient.right, rcClient.bottom);
    if ( !region )
        return;

    for ( size_t i = 1; i < cuts.size(); ++i )
    {
        HRGN cut = ::CreateRectRgn(cuts[i].left, cuts[i].top,
                                   cuts[i].right, cuts[i].bottom);
        if ( cut )
        {
            ::CombineRgn(region, region, cut, RGN_DIFF);
            ::DeleteObject(cut);
        }
    }

    // The system owns the region after this call.
    ::SetWindowRgn(m_bridge, region, TRUE);
}

LRESULT CALLBACK
wxWinUITopLevelHost::BridgeSubclassProc(HWND hwnd, UINT msg,
                                        WPARAM wParam, LPARAM lParam,
                                        UINT_PTR subclassId, DWORD_PTR refData)
{
    wxWinUITopLevelHost * const host =
        reinterpret_cast<wxWinUITopLevelHost *>(refData);

    switch ( msg )
    {
        // Note: WM_NCHITTEST is deliberately NOT answered with HTTRANSPARENT
        // any more.  The island is the single input receiver (its root is
        // hit-testable everywhere) and the root router forwards real native
        // messages to the wx windows below: hit-test pass-through through
        // the pointer pipeline proved non-deterministic.

        case WM_SETCURSOR:
        {
            // A window repositioned under a resting pointer (a sizer
            // relayout, a child move, the coalesced slot sync...) makes the
            // system re-send WM_SETCURSOR without any pointer motion.  The
            // island answers those with its default arrow, wiping the
            // per-element cursor (the I-beam over a TextBox...) until the
            // next real move re-applies it -- visibly, hover cursors only
            // held while the mouse was moving.  Swallow them: returning
            // TRUE keeps whatever cursor is currently on screen.  (The
            // position comparison is tolerant: the island reports pointer
            // positions in DIPs and the round-trip can be off by a pixel.)
            POINT pt;
            const bool still =
                host && ::GetCursorPos(&pt) &&
                    std::abs(pt.x - host->m_lastPointerScreen.x) <= 2 &&
                        std::abs(pt.y - host->m_lastPointerScreen.y) <= 2;

            if ( host && wxWinUIInputLogEnabled() )
            {
                POINT ptLog = { -1, -1 };
                ::GetCursorPos(&ptLog);
                wxWinUIInputLog("bridge WM_SETCURSOR cur=(%ld,%ld) "
                                "last=(%ld,%ld) ht=%d => %s",
                                ptLog.x, ptLog.y,
                                host->m_lastPointerScreen.x,
                                host->m_lastPointerScreen.y,
                                (int)LOWORD(lParam),
                                still ? "SWALLOW" : "pass");
            }

            if ( still )
                return TRUE;
            break;
        }

        case WM_NCDESTROY:
            ::RemoveWindowSubclass(hwnd, BridgeSubclassProc, subclassId);
            break;
    }

    return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

// ============================================================================
// hit test
// ============================================================================

bool wxWinUITopLevelHost::HitTestContent(const POINT& ptScreen)
{
    if ( !m_root )
        return false;

    const auto xamlRoot = GetXamlRoot();
    if ( !xamlRoot )
        return false;

    const RECT rcCS = GetClientScreenRect();
    const POINT pt = { ptScreen.x - rcCS.left, ptScreen.y - rcCS.top };

    try
    {
        using winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper;

        // Fast rejection on the synced slot rects; the precise (and more
        // expensive) visual-tree query only runs when a rect matches or when
        // free elements (transients, menu bar...) live on the root.
        bool inSlot = false;
        for ( const auto& kv : m_slots )
        {
            const wxWinUISlot * const slot = kv.second;
            if ( slot->m_visible && slot->m_rectPx.Contains(pt.x, pt.y) )
            {
                inSlot = true;
                break;
            }
        }

        const bool hasFreeContent =
            m_root.Children().Size() > m_slots.size();
        if ( !inSlot && !hasFreeContent )
            return false;

        const double scale = GetScale();
        const winrt::Windows::Foundation::Point dip
        {
            float(pt.x / scale),
            float(pt.y / scale)
        };

        const auto hits =
            VisualTreeHelper::FindElementsInHostCoordinates(dip, m_root);
        return hits.First().HasCurrent();
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

// ============================================================================
// misc
// ============================================================================

winrt::Microsoft::UI::Xaml::XamlRoot wxWinUITopLevelHost::GetXamlRoot() const
{
    return m_root ? m_root.XamlRoot()
                  : winrt::Microsoft::UI::Xaml::XamlRoot{ nullptr };
}

// Physical screen rect of the TLW client area, normalized left<right even
// for mirrored (RTL) windows.  Island coordinates are physically LTR while
// ClientToScreen/ScreenToClient on a WS_EX_LAYOUTRTL window flip the X
// axis, so every island<->screen conversion must go through this rect and
// never through those two calls (with the TLW handle) directly.
RECT wxWinUITopLevelHost::GetClientScreenRect() const
{
    RECT rc = { 0, 0, 0, 0 };
    const HWND hwndTLW = m_tlw ? GetHwndOf(m_tlw) : nullptr;
    if ( !hwndTLW )
        return rc;

    ::GetClientRect(hwndTLW, &rc);
    ::MapWindowPoints(hwndTLW, nullptr, reinterpret_cast<POINT *>(&rc), 2);

    if ( rc.left > rc.right )
    {
        const LONG swap = rc.left;
        rc.left = rc.right;
        rc.right = swap;
    }

    return rc;
}

wxPoint wxWinUITopLevelHost::ScreenToRootDIP(const wxPoint& screenPt) const
{
    if ( !m_tlw )
        return screenPt;

    const RECT rcCS = GetClientScreenRect();
    const POINT pt = { screenPt.x - rcCS.left, screenPt.y - rcCS.top };

    const double scale = GetScale();
    return wxPoint(static_cast<int>(pt.x / scale),
                   static_cast<int>(pt.y / scale));
}

double wxWinUITopLevelHost::GetScale() const
{
    try
    {
        const auto xamlRoot = GetXamlRoot();
        if ( xamlRoot )
        {
            const double scale = xamlRoot.RasterizationScale();
            if ( scale > 0 )
                return scale;
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }

    return m_tlw ? m_tlw->GetDPIScaleFactor() : 1.0;
}

void wxWinUITopLevelHost::ApplyTheme(winrt::Microsoft::UI::Xaml::ElementTheme theme)
{
    if ( !m_root )
        return;

    try
    {
        m_root.RequestedTheme(theme);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("TLW theme change", e);
    }
}

void wxWinUITopLevelHost::ApplyThemeToAll(
    winrt::Microsoft::UI::Xaml::ElementTheme theme)
{
    for ( const auto& kv : gs_tlwHosts )
        kv.second->ApplyTheme(theme);
}

#endif // wxUSE_WINUI3

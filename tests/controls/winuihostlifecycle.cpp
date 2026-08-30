///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuihostlifecycle.cpp
// Purpose:     lifecycle tests of the shared wxWinUI top-level host
// Author:      wxWidgets development team
// Created:     2026-07-22
// Copyright:   (c) wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/button.h"
    #include "wx/frame.h"
    #include "wx/panel.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP

#include "wx/evtloop.h"
#include "wx/busycursor.h"
#include "wx/dcmemory.h"
#include "wx/dialog.h"
#include "wx/scopeguard.h"
#include "wx/checkbox.h"
#include "wx/radiobox.h"
#include "wx/radiobut.h"
#include "wx/textctrl.h"
#if wxUSE_STATTEXT
    #include "wx/stattext.h"
#endif
#if wxUSE_GAUGE
    #include "wx/gauge.h"
#endif
#if wxUSE_STATBOX
    #include "wx/statbox.h"
#endif
#if wxUSE_STATUSBAR
    #include "wx/statusbr.h"
#endif
#if wxUSE_TOOLBAR
    #include "wx/toolbar.h"
#endif
#if wxUSE_NOTEBOOK
    #include "wx/notebook.h"
#endif
#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif
#if wxUSE_POPUPWIN
    #include "wx/popupwin.h"
#endif

#include "wx/winui/private/tlwhost.h"
#include "control-host-test-access.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "wx/winui/private/inputtest.h"
#include "wx/winui/winui.h"
#include "wx/winui/xamlhost.h"

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

#include <UIAutomation.h>

#include <cmath>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace
{

// Let the coalesced flushes, pending deletes and XAML finalization drain.
void DrainDispatch(int rounds = 20)
{
    for ( int i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(5);
    }
}

template <typename Predicate>
bool DrainUntil(Predicate predicate, int rounds = 100)
{
    for ( int i = 0; i < rounds; ++i )
    {
        if ( predicate() )
            return true;
        DrainDispatch(1);
    }

    return predicate();
}

// The shared host of a TLW is created lazily with its first slot and then
// lives until the TLW dies, root-router handlers and host lifetime state
// included.  Create the test top window's host up front so the per-test
// deltas below measure the slots, not that one-off host creation.
void PrimeTopWindowHost()
{
    delete new wxButton(wxTheApp->GetTopWindow(), wxID_ANY, "prime");
    DrainDispatch(3);
}

// Drain until the flush counters stop moving: the strict scheduling
// assertions below are only deterministic once every pre-existing flush
// (deferred Loaded re-measures included) has run out.
void DrainToQuiescence()
{
    for ( int i = 0; i < 50; ++i )
    {
        const unsigned sched = wxWinUITopLevelHost::GetFlushScheduleCount();
        const unsigned runs = wxWinUITopLevelHost::GetFlushRunCount();
        DrainDispatch(2);
        if ( sched == wxWinUITopLevelHost::GetFlushScheduleCount() &&
                runs == wxWinUITopLevelHost::GetFlushRunCount() )
            return;
    }

    FAIL("WinUI host dispatch did not reach quiescence after 50 rounds");
}

wxLayoutDirection gs_nestedLayoutDirectionRequest = wxLayout_Default;
unsigned gs_nestedLayoutDirectionRequestCalls = 0;

void RequestNestedLayoutDirection(wxWindow *window)
{
    ++gs_nestedLayoutDirectionRequestCalls;
    if ( window &&
         gs_nestedLayoutDirectionRequest != wxLayout_Default )
    {
        window->SetLayoutDirection(gs_nestedLayoutDirectionRequest);
    }
}

unsigned gs_oscillatingLayoutDirectionCalls = 0;

void OscillateNestedLayoutDirection(wxWindow *window)
{
    ++gs_oscillatingLayoutDirectionCalls;
    if ( window )
    {
        window->SetLayoutDirection(
            window->GetLayoutDirection() == wxLayout_RightToLeft
                ? wxLayout_LeftToRight
                : wxLayout_RightToLeft);
        wxWinUIMSWSetAfterLayoutDirectionNativeWriteForTest(
            &OscillateNestedLayoutDirection);
    }
}

void FlipNativeLayoutDirectionStyle(wxWindow *window)
{
    if ( !window )
        return;

    const HWND hwnd = GetHwndOf(window);
    const LONG_PTR style = ::GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    ::SetWindowLongPtr(
        hwnd, GWL_EXSTYLE, style ^ WS_EX_LAYOUTRTL);
}

class LayoutDirectionProjectionProbe final : public wxWindow
{
public:
    explicit LayoutDirectionProjectionProbe(wxWindow *parent)
        : wxWindow(parent, wxID_ANY,
                   wxPoint(10, 220), wxSize(100, 30))
    {
    }

    bool failPeerProjection = true;
    unsigned peerProjectionCalls = 0;

protected:
    bool MSWOnEffectiveLayoutDirectionChanged() override
    {
        ++peerProjectionCalls;
        return !failPeerProjection;
    }
};

// The plan-009 cross-window gates need a realized DesktopWindowXamlSource,
// but must not activate, cover or intercept input from the user's desktop.
// Keep their real TLWs far outside the virtual desktop and remove them from
// Alt-Tab before showing without activation.
void ShowOffscreenWithoutActivating(wxFrame *frame, int ordinal = 0)
{
    const HWND hwnd = GetHwndOf(frame);
    REQUIRE(hwnd != nullptr);
    ::SetWindowLongPtr(
        hwnd, GWL_EXSTYLE,
        ::GetWindowLongPtr(hwnd, GWL_EXSTYLE) |
            WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
    frame->Move(wxPoint(-32000 + ordinal * 400, -32000));
    frame->ShowWithoutActivating();
}

class WinUIAppThemeRestorer final
{
public:
    WinUIAppThemeRestorer()
        : m_theme(wxWinUIGetAppTheme())
    {
    }

    ~WinUIAppThemeRestorer()
    {
        wxWinUISetAppTheme(m_theme);
    }

    WinUIAppThemeRestorer(const WinUIAppThemeRestorer&) = delete;
    WinUIAppThemeRestorer& operator=(const WinUIAppThemeRestorer&) = delete;

private:
    const wxWinUIAppTheme m_theme;
};

// A real cross-monitor move cannot be automated without taking over the
// user's desktop. Production reads the HWND DPI without virtual dispatch;
// this scoped internal seam supplies deterministic per-TLW values while every
// queue, native-parent, host-migration and event path remains real.
std::vector<std::pair<wxWindow *, wxSize>> gs_reparentDPIByTLW;
wxWindow *gs_syntheticDPIQueryWindow = nullptr;
wxWindow *gs_syntheticDPIQueryTLW = nullptr;
std::function<void(wxWindow *)> gs_syntheticDPIQueryCallback;
std::function<void(wxWindow *)> gs_afterSetParentCallback;

void InvokeAfterSetParentForTest(wxWindow *window)
{
    auto callback = std::move(gs_afterSetParentCallback);
    gs_afterSetParentCallback = {};
    if ( callback )
        callback(window);
}

wxSize QuerySyntheticReparentDPI(wxWindow *window)
{
    wxWindow * const tlw = wxGetTopLevelParent(window);
    if ( gs_syntheticDPIQueryCallback &&
         window == gs_syntheticDPIQueryWindow &&
         tlw == gs_syntheticDPIQueryTLW )
    {
        auto callback = std::move(gs_syntheticDPIQueryCallback);
        gs_syntheticDPIQueryCallback = {};
        gs_syntheticDPIQueryWindow = nullptr;
        gs_syntheticDPIQueryTLW = nullptr;
        callback(window);
    }

    for ( const auto& entry : gs_reparentDPIByTLW )
    {
        if ( entry.first == tlw )
            return entry.second;
    }
    return wxSize(96, 96);
}

class SyntheticReparentDPI final
{
public:
    SyntheticReparentDPI()
    {
        wxWinUIMSWSetReparentDPIQueryForTest(
            QuerySyntheticReparentDPI);
    }

    ~SyntheticReparentDPI()
    {
        wxWinUIMSWSetReparentDPIQueryForTest(nullptr);
        wxWinUIMSWSetAfterSetParentForTest(nullptr);
        gs_reparentDPIByTLW.clear();
        gs_syntheticDPIQueryWindow = nullptr;
        gs_syntheticDPIQueryTLW = nullptr;
        gs_syntheticDPIQueryCallback = {};
        gs_afterSetParentCallback = {};
    }

    void SetTLWDPI(wxWindow *tlw, int dpi)
    {
        for ( auto& entry : gs_reparentDPIByTLW )
        {
            if ( entry.first == tlw )
            {
                entry.second = wxSize(dpi, dpi);
                return;
            }
        }
        gs_reparentDPIByTLW.push_back({tlw, wxSize(dpi, dpi)});
    }

    void RemoveTLW(wxWindow *tlw)
    {
        for ( auto it = gs_reparentDPIByTLW.begin();
              it != gs_reparentDPIByTLW.end(); )
        {
            if ( it->first == tlw )
                it = gs_reparentDPIByTLW.erase(it);
            else
                ++it;
        }
    }

    void OnNextTargetDPIQuery(
        wxWindow *window,
        wxWindow *tlw,
        std::function<void(wxWindow *)> callback)
    {
        gs_syntheticDPIQueryWindow = window;
        gs_syntheticDPIQueryTLW = tlw;
        gs_syntheticDPIQueryCallback = std::move(callback);
    }

    void AfterNextSetParent(
        std::function<void(wxWindow *)> callback)
    {
        gs_afterSetParentCallback = std::move(callback);
        wxWinUIMSWSetAfterSetParentForTest(
            InvokeAfterSetParentForTest);
    }

private:
    wxDECLARE_NO_COPY_CLASS(SyntheticReparentDPI);
};

class ReparentOnAddChildPanel final : public wxPanel
{
public:
    explicit ReparentOnAddChildPanel(wxWindow *parent)
        : wxPanel(parent)
    {
    }

    void Arm(wxWindowBase *expectedChild, wxWindowBase *targetParent)
    {
        m_expectedChild = expectedChild;
        m_targetParent = targetParent;
        m_armed = true;
    }

    bool WasTriggered() const { return m_triggered; }
    bool NestedReparentSucceeded() const { return m_nestedSucceeded; }

    void AddChild(wxWindowBase *child) override
    {
        wxPanel::AddChild(child);
        if ( !m_armed || child != m_expectedChild )
            return;

        m_armed = false;
        m_triggered = true;
        m_nestedSucceeded = Reparent(m_targetParent);
    }

private:
    wxWindowBase *m_expectedChild = nullptr;
    wxWindowBase *m_targetParent = nullptr;
    bool m_armed = false;
    bool m_triggered = false;
    bool m_nestedSucceeded = false;
};

// Count the real DesktopChildSiteBridge windows under a TLW: the direct
// proof of how many XAML sources exist, independent of any bookkeeping a
// rogue class could forget to update.
unsigned CountSiteBridges(HWND hwndTLW)
{
    struct Counter { unsigned n; };
    Counter counter = { 0 };
    ::EnumChildWindows(
        hwndTLW,
        [](HWND hwnd, LPARAM lparam) -> BOOL
        {
            wchar_t cls[96] = L"";
            ::GetClassNameW(hwnd, cls, WXSIZEOF(cls));
            if ( wxString(cls).Contains(wxS("DesktopChildSiteBridge")) )
                ++reinterpret_cast<Counter *>(lparam)->n;
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&counter));
    return counter.n;
}

// Collect every RadioButton in a composite slot subtree (the radio box
// root holds the group label and an inner grid, so recurse through the
// logical children).
void
CollectRadioButtonsInto(
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    std::vector<winrt::Microsoft::UI::Xaml::Controls::RadioButton>& out)
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    if ( !element )
        return;

    if ( const auto rb = element.try_as<MUXC::RadioButton>() )
    {
        out.push_back(rb);
        return;
    }
    if ( const auto panel = element.try_as<MUXC::Panel>() )
    {
        for ( const auto& child : panel.Children() )
            CollectRadioButtonsInto(child, out);
        return;
    }
    if ( const auto border = element.try_as<MUXC::Border>() )
    {
        CollectRadioButtonsInto(border.Child(), out);
        return;
    }
    if ( const auto cc = element.try_as<MUXC::ContentControl>() )
    {
        CollectRadioButtonsInto(
            cc.Content().try_as<winrt::Microsoft::UI::Xaml::UIElement>(),
            out);
    }
}

std::vector<winrt::Microsoft::UI::Xaml::Controls::RadioButton>
CollectRadioButtons(const winrt::Microsoft::UI::Xaml::UIElement& content)
{
    std::vector<winrt::Microsoft::UI::Xaml::Controls::RadioButton> buttons;
    CollectRadioButtonsInto(content, buttons);
    return buttons;
}

// FocusManager may report a templated leaf instead of the Control whose
// routed GotFocus owns it. Walk only the live visual-parent chain and keep the
// query bounded so a malformed native tree can never hang the oracle.
bool IsInVisualSubtree(
    const winrt::Windows::Foundation::IInspectable& element,
    const winrt::Microsoft::UI::Xaml::UIElement& root)
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

    try
    {
        if ( !element || !root )
            return false;

        const auto rootDO = root.as<MUX::DependencyObject>();
        auto node = element.try_as<MUX::DependencyObject>();
        for ( unsigned depth = 0; node && depth < 128; ++depth )
        {
            if ( node == rootDO )
                return true;
            node = MUXM::VisualTreeHelper::GetParent(node);
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }

    return false;
}

#if wxUSE_ACCESSIBILITY
class LazyAccessibleButton final : public wxButton
{
public:
    unsigned GetCreateAccessibleCalls() const
        { return m_createAccessibleCalls; }

protected:
    wxAccessible *CreateAccessible() override
    {
        ++m_createAccessibleCalls;
        return new wxWindowAccessible(this);
    }

private:
    unsigned m_createAccessibleCalls = 0;
};

class ReparentingAccessibleXamlHost final : public wxWinUIXamlHost
{
public:
    void ArmReparentFromFactory(wxWindow *target)
    {
        m_reparentTarget = target;
        m_reparentAttempted = false;
        m_reparentSucceeded = false;
    }

    unsigned GetCreateAccessibleCalls() const
        { return m_createAccessibleCalls; }
    bool WasReparentAttempted() const
        { return m_reparentAttempted; }
    bool DidReparentSucceed() const
        { return m_reparentSucceeded; }

protected:
    wxAccessible *CreateAccessible() override
    {
        ++m_createAccessibleCalls;
        if ( m_reparentTarget )
        {
            wxWindow * const target = m_reparentTarget;
            m_reparentTarget = nullptr;
            m_reparentAttempted = true;
            m_reparentSucceeded = Reparent(target);
        }

        // Keep the factory lazy so the migration performs a second query.
        // The reentrant Reparent above is the arbitrary application callback
        // whose payload ordering this test exercises.
        return nullptr;
    }

private:
    wxWindow *m_reparentTarget = nullptr;
    unsigned m_createAccessibleCalls = 0;
    bool m_reparentAttempted = false;
    bool m_reparentSucceeded = false;
};
#endif // wxUSE_ACCESSIBILITY

#if wxUSE_TOOLTIPS
wxString GetToolTipInspectableText(
    const winrt::Windows::Foundation::IInspectable& tip)
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    if ( !tip )
        return wxString();

    if ( const auto object = tip.try_as<MUXC::ToolTip>() )
    {
        return wxString(
            winrt::unbox_value<winrt::hstring>(
                object.Content()).c_str());
    }
    return wxString(winrt::unbox_value<winrt::hstring>(tip).c_str());
}

wxString GetPeerToolTipText(wxWindow *window, bool *present = nullptr)
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(window);
    wxWinUISlot * const slot = host ? host->FindSlot(window) : nullptr;
    if ( !slot || !slot->GetContent() )
    {
        if ( present )
            *present = false;
        return wxString();
    }

    const auto tip = MUXC::ToolTipService::GetToolTip(slot->GetContent());
    if ( present )
        *present = tip != nullptr;
    return GetToolTipInspectableText(tip);
}
#endif // wxUSE_TOOLTIPS

class GettingFocusUnregisterProbe final : public wxWindow
{
public:
    void Arm(wxWinUITopLevelHost *host)
    {
        m_host = host;
        m_armed = true;
        m_keyboardPredicateCalls = 0;
    }

    unsigned GetKeyboardPredicateCalls() const
        { return m_keyboardPredicateCalls; }

    bool CanBeFocused() const override
    {
        if ( m_armed )
        {
            m_armed = false;
            wxWinUITopLevelHost * const host = m_host;
            m_host = nullptr;
            if ( host )
            {
                host->UnregisterSlot(
                    const_cast<GettingFocusUnregisterProbe *>(this));
            }
        }
        return true;
    }

    bool AcceptsFocusFromKeyboard() const override
    {
        ++m_keyboardPredicateCalls;
        return true;
    }

private:
    mutable wxWinUITopLevelHost *m_host = nullptr;
    mutable bool m_armed = false;
    mutable unsigned m_keyboardPredicateCalls = 0;
};

class GettingFocusDestroyTLWProbe final : public wxWindow
{
public:
    void Arm(wxFrame **frame, bool *invoked)
    {
        m_frame = frame;
        m_invoked = invoked;
    }

    bool CanBeFocused() const override
    {
        wxFrame ** const frame = m_frame;
        bool * const invoked = m_invoked;
        m_frame = nullptr;
        m_invoked = nullptr;
        if ( frame && *frame )
        {
            wxFrame * const doomed = *frame;
            *frame = nullptr;
            if ( invoked )
                *invoked = true;
            // Request TLW destruction from the host's routed GettingFocus
            // predicate, then pump the pending-delete path while the XAML
            // callback is still on the stack. Production code must use
            // Destroy(), never direct delete, for a live top-level window.
            doomed->Destroy();
            wxYield();
        }
        return true;
    }

private:
    mutable wxFrame **m_frame = nullptr;
    mutable bool *m_invoked = nullptr;
};

struct DeferredDestroyVirtualCounters
{
    unsigned destroyCalls = 0;
    unsigned hideCalls = 0;
};

class DeferredDestroyProbeFrame final : public wxFrame
{
public:
    DeferredDestroyProbeFrame(
        const wxString& title,
        const std::shared_ptr<DeferredDestroyVirtualCounters>& counters)
        : wxFrame(nullptr, wxID_ANY, title,
                  wxPoint(-32000, -32000), wxSize(280, 160)),
          m_counters(counters)
    {
    }

    bool Destroy() override
    {
        ++m_counters->destroyCalls;
        return wxFrame::Destroy();
    }

    bool Show(bool show = true) override
    {
        if ( !show )
            ++m_counters->hideCalls;
        return wxFrame::Show(show);
    }

private:
    std::shared_ptr<DeferredDestroyVirtualCounters> m_counters;
};

#if wxUSE_POPUPWIN

class DeferredDestroyProbePopup final : public wxPopupWindow
{
public:
    DeferredDestroyProbePopup(
        wxWindow *parent,
        const std::shared_ptr<DeferredDestroyVirtualCounters>& counters)
        : wxPopupWindow(parent, wxPU_CONTAINS_CONTROLS),
          m_counters(counters)
    {
    }

    bool Destroy() override
    {
        ++m_counters->destroyCalls;
        return wxPopupWindow::Destroy();
    }

private:
    std::shared_ptr<DeferredDestroyVirtualCounters> m_counters;
};

class DeferredDestroyProbeTransientPopup final
    : public wxPopupTransientWindow
{
public:
    DeferredDestroyProbeTransientPopup(
        wxWindow *parent,
        const std::shared_ptr<DeferredDestroyVirtualCounters>& counters)
        : wxPopupTransientWindow(parent, wxPU_CONTAINS_CONTROLS),
          m_counters(counters)
    {
    }

    bool Destroy() override
    {
        ++m_counters->destroyCalls;
        return wxPopupTransientWindow::Destroy();
    }

private:
    std::shared_ptr<DeferredDestroyVirtualCounters> m_counters;
};

// Deliberately bypasses the concrete MSW wxPopupTransientWindow class. The
// public base is subclassable and owns its own Destroy() contract, which must
// enter the same WinUI deferral transaction.
class DirectTransientPopupBaseProbe final
    : public wxPopupTransientWindowBase
{
public:
    explicit DirectTransientPopupBaseProbe(wxWindow *parent)
    {
        (void)Create(parent, wxPU_CONTAINS_CONTROLS);
    }

    void Popup(wxWindow *focus = nullptr) override
    {
        Show();
        if ( focus && IsDescendant(focus) )
            focus->SetFocus();
    }

    void Dismiss() override
    {
        Hide();
    }
};

#endif // wxUSE_POPUPWIN

} // anonymous namespace

TEST_CASE("HostLifecycle::RegisterUnregisterLoop", "[HostLifecycle]")
{
    PrimeTopWindowHost();

    const unsigned slots0 = wxWinUITopLevelHost::GetLiveSlotCount();
    const unsigned states0 = wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
    const unsigned adds0 = wxWinUITopLevelHost::GetSlotHandlerAddCount();
    const unsigned revokes0 = wxWinUITopLevelHost::GetSlotHandlerRevokeCount();

    for ( int i = 0; i < 100; ++i )
    {
        wxButton * const button =
            new wxButton(wxTheApp->GetTopWindow(), wxID_ANY, "lifecycle");
        delete button;
    }

    DrainDispatch();

    // no slot may leak, and -- the real point -- no callback state may
    // outlive its slot: every handler must have been revoked
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == slots0);
    CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() == states0);

    // and not just by the shared state going null: every AddHandler /
    // focus subscription of the loop was individually revoked
    const unsigned adds =
        wxWinUITopLevelHost::GetSlotHandlerAddCount() - adds0;
    const unsigned revokes =
        wxWinUITopLevelHost::GetSlotHandlerRevokeCount() - revokes0;
    CHECK(adds > 0);
    CHECK(adds == revokes);
}

TEST_CASE("HostLifecycle::DestroyTLWWithSlots", "[HostLifecycle]")
{
    const unsigned hosts0 = wxWinUITopLevelHost::GetLiveHostCount();
    const unsigned states0 = wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
    const unsigned subclasses0 =
        wxWinUITopLevelHost::GetLiveSubclassContextCount();
    const unsigned adds0 = wxWinUITopLevelHost::GetSlotHandlerAddCount();
    const unsigned revokes0 = wxWinUITopLevelHost::GetSlotHandlerRevokeCount();

    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "lifecycle-tlw");
    for ( int i = 0; i < 10; ++i )
        new wxButton(frame, wxID_ANY, wxString::Format("b%d", i));
    frame->Show();
    DrainDispatch(5);

    CHECK(wxWinUITopLevelHost::GetLiveHostCount() > hosts0);

    frame->Destroy();
    DrainDispatch();

    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hosts0);
    CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() == states0);
    CHECK(wxWinUITopLevelHost::GetLiveSubclassContextCount() == subclasses0);

    // the TLW teardown goes through Shutdown(), not UnregisterSlot(): that
    // path too must revoke every single handler before closing the source
    const unsigned adds =
        wxWinUITopLevelHost::GetSlotHandlerAddCount() - adds0;
    const unsigned revokes =
        wxWinUITopLevelHost::GetSlotHandlerRevokeCount() - revokes0;
    CHECK(adds > 0);
    CHECK(adds == revokes);
}

TEST_CASE("HostLifecycle::ShutdownRestoresRetainedContentState",
          "[HostLifecycle][HostState][tooltip][accessibility]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
    namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

#if wxUSE_TOOLTIPS
    struct PolicyReset
    {
        ~PolicyReset() { wxToolTip::Enable(true); }
    } reset;
    wxToolTip::Enable(true);
    const unsigned managedBefore =
        wxWinUIGetManagedToolTipCountForTesting();
#endif
    const unsigned styleObserversBefore =
        wxWinUITopLevelHost::
            GetLiveAutomationNameStyleObserverCountForTest();
    const unsigned loadedObserversBefore =
        wxWinUITopLevelHost::GetLiveContentLoadedObserverCountForTest();

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "shutdown-wx-name");
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(frame, true);
    REQUIRE(host != nullptr);

    const MUXC::Border content;
    MUXC::ToolTip authoredToolTip;
    authoredToolTip.Content(winrt::box_value(L"shutdown-authored-tip"));
    MUXC::ToolTipService::SetToolTip(content, authoredToolTip);
    MUXA::AutomationProperties::SetHelpText(
        content, L"shutdown-authored-help");
    MUXA::AutomationProperties::SetAccessibilityView(
        content, MUXAP::AccessibilityView::Control);
    REQUIRE(content.ReadLocalValue(
                MUXA::AutomationProperties::NameProperty()) ==
            MUX::DependencyProperty::UnsetValue());

    wxWinUISlot * const slot = host->RegisterSlot(frame, content);
    REQUIRE(slot != nullptr);
    CHECK(wxWinUITopLevelHost::
              GetLiveAutomationNameStyleObserverCountForTest() ==
          styleObserversBefore + 1);
    CHECK(wxWinUITopLevelHost::GetLiveContentLoadedObserverCountForTest() ==
          loadedObserversBefore + 1);
    frame->SetToolTip("shutdown-wx-tip");
#if wxUSE_ACCESSIBILITY
    new wxWindowAccessible(frame);
#endif
    ShowOffscreenWithoutActivating(frame);
    host->FlushSync();

#if wxUSE_TOOLTIPS
    CHECK(MUXC::ToolTipService::GetToolTip(content) != authoredToolTip);
    CHECK(wxWinUIGetManagedToolTipCountForTesting() ==
          managedBefore + 1);
#endif
    CHECK(MUXA::AutomationProperties::GetHelpText(content) ==
          L"shutdown-wx-tip");
    CHECK_FALSE(MUXA::AutomationProperties::GetName(content).empty());
#if wxUSE_ACCESSIBILITY
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(content) ==
          MUXAP::AccessibilityView::Raw);
#endif

    // Attached-property restoration is an application callback boundary.
    // Retiring the current slot from AutomationProperties.Name must neither
    // invalidate Shutdown()'s slot-table traversal nor free its in-flight slot.
    bool shutdownRestoreCallback = false;
    bool armShutdownRestoreCallback = true;
    const int64_t shutdownRestoreToken =
        content.RegisterPropertyChangedCallback(
            MUXA::AutomationProperties::NameProperty(),
            [&](const MUX::DependencyObject&,
                const MUX::DependencyProperty&)
            {
                if ( armShutdownRestoreCallback &&
                     !shutdownRestoreCallback )
                {
                    shutdownRestoreCallback = true;
                    host->UnregisterSlot(frame);
                }
            });

    // The strong WinRT reference deliberately outlives the frame. Shutdown()
    // must restore while Source.Close() still owns the attached visual tree.
    frame->Destroy();
    DrainDispatch();
    armShutdownRestoreCallback = false;
    content.UnregisterPropertyChangedCallback(
        MUXA::AutomationProperties::NameProperty(),
        shutdownRestoreToken);
    CHECK(shutdownRestoreCallback);

#if wxUSE_TOOLTIPS
    CHECK(MUXC::ToolTipService::GetToolTip(content) == authoredToolTip);
    CHECK(wxWinUIGetManagedToolTipCountForTesting() == managedBefore);
#endif
    CHECK(MUXA::AutomationProperties::GetHelpText(content) ==
          L"shutdown-authored-help");
    CHECK(content.ReadLocalValue(
              MUXA::AutomationProperties::NameProperty()) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(content) ==
          MUXAP::AccessibilityView::Control);
    CHECK(wxWinUITopLevelHost::
              GetLiveAutomationNameStyleObserverCountForTest() ==
          styleObserversBefore);
    CHECK(wxWinUITopLevelHost::GetLiveContentLoadedObserverCountForTest() ==
          loadedObserversBefore);
}

TEST_CASE("HostLifecycle::ReparentAcrossTLW",
          "[HostLifecycle][winui-009]")
{
    const unsigned hosts0 = wxWinUITopLevelHost::GetLiveHostCount();
    const unsigned states0 = wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
    const unsigned adds0 = wxWinUITopLevelHost::GetSlotHandlerAddCount();
    const unsigned revokes0 = wxWinUITopLevelHost::GetSlotHandlerRevokeCount();

    wxFrame * const frameA = new wxFrame(nullptr, wxID_ANY, "lifecycle-A");
    wxFrame * const frameB = new wxFrame(nullptr, wxID_ANY, "lifecycle-B");
    wxButton * const button = new wxButton(frameA, wxID_ANY, "migrant");
    ShowOffscreenWithoutActivating(frameA, 0);
    ShowOffscreenWithoutActivating(frameB, 1);
    DrainDispatch(5);

    for ( int i = 0; i < 100; ++i )
    {
        wxFrame * const target = i % 2 ? frameA : frameB;
        wxFrame * const other = i % 2 ? frameB : frameA;

        REQUIRE(button->Reparent(target));
        // the migration happens in the coalesced flush
        DrainDispatch(3);
        // the control must stay alive and usable after each migration
        button->SetLabel(wxString::Format("migrant %d", i));

        // exactly one host owns the slot after the migration: the target's
        wxWinUITopLevelHost * const hostTarget =
            wxWinUITopLevelHost::FindForTLW(target);
        REQUIRE(hostTarget != nullptr);
        CHECK(hostTarget->FindSlot(button) != nullptr);

        wxWinUITopLevelHost * const hostOther =
            wxWinUITopLevelHost::FindForTLW(other);
        if ( hostOther )
            CHECK(hostOther->FindSlot(button) == nullptr);
    }

    frameA->Destroy();
    frameB->Destroy();
    DrainDispatch();

    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hosts0);
    CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() == states0);

    // every migration is an unregister + register pair: the handler
    // add/revoke balance must close over the whole shuffle
    const unsigned adds =
        wxWinUITopLevelHost::GetSlotHandlerAddCount() - adds0;
    const unsigned revokes =
        wxWinUITopLevelHost::GetSlotHandlerRevokeCount() - revokes0;
    CHECK(adds > 0);
    CHECK(adds == revokes);
}

TEST_CASE("HostLifecycle::PopupGateRejectsCrossTLWReparentWithoutMutation",
          "[HostLifecycle][popup-gate][migration][causal]")
{
    wxFrame * const source =
        new wxFrame(nullptr, wxID_ANY, "popup-gate-source");
    wxFrame * const destination =
        new wxFrame(nullptr, wxID_ANY, "popup-gate-destination");
    wxButton * const button = new wxButton(source, wxID_ANY, "peer");

    wxWinUITopLevelHost * const sourceHost =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(sourceHost != nullptr);
    wxWinUISlot * const sourceSlot = sourceHost->FindSlot(button);
    REQUIRE(sourceSlot != nullptr);
    const auto content = sourceSlot->GetContent();
    const HWND nativeParent = ::GetParent(GetHwndOf(button));
    const auto gate = wxWinUICreatePhysicalDisconnectGateForTesting(
        content.DispatcherQueue(), false);
    REQUIRE(gate != nullptr);
    sourceHost->SetSlotPhysicalDisconnectGate(button, gate);
    const std::uintptr_t gateIdentity =
        reinterpret_cast<std::uintptr_t>(gate.get());

    // Check 1 rejects before wxWindowBase::Reparent() or USER32 SetParent().
    CHECK_FALSE(button->Reparent(destination));
    CHECK(button->GetParent() == source);
    CHECK(::GetParent(GetHwndOf(button)) == nativeParent);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(button) == sourceHost);
    CHECK(sourceHost->FindSlot(button) == sourceSlot);
    CHECK(sourceSlot->GetContent() == content);
    CHECK(sourceSlot->GetPhysicalDisconnectGateIdentityForTest() ==
          gateIdentity);

    // The caller closes the popup, its exact causal gate completes, and an
    // explicit retry transfers the same reusable/satisfied gate identity.
    REQUIRE(wxWinUISetPhysicalDisconnectGateStateForTesting(
        gate, true));
    REQUIRE(button->Reparent(destination));
    wxWinUITopLevelHost * const destinationHost =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(destinationHost != nullptr);
    REQUIRE(destinationHost != sourceHost);
    wxWinUISlot * const destinationSlot =
        destinationHost->FindSlot(button);
    REQUIRE(destinationSlot != nullptr);
    CHECK(destinationSlot->GetContent() == content);
    CHECK(destinationSlot->GetPhysicalDisconnectGateIdentityForTest() ==
          gateIdentity);
    CHECK(sourceHost->FindSlot(button) == nullptr);

    destination->Destroy();
    source->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::PopupOpenedBySetParentCallbackRollsBackExactly",
          "[HostLifecycle][popup-gate][migration][rollback]")
{
    SyntheticReparentDPI dpi;
    wxFrame * const source =
        new wxFrame(nullptr, wxID_ANY, "popup-after-setparent-source");
    wxFrame * const destination =
        new wxFrame(nullptr, wxID_ANY, "popup-after-setparent-destination");
    wxButton * const button = new wxButton(source, wxID_ANY, "peer");
    wxWinUITopLevelHost * const sourceHost =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(sourceHost != nullptr);
    wxWinUISlot * const sourceSlot = sourceHost->FindSlot(button);
    REQUIRE(sourceSlot != nullptr);
    const auto content = sourceSlot->GetContent();
    const HWND nativeParent = ::GetParent(GetHwndOf(button));
    const auto gate = wxWinUICreatePhysicalDisconnectGateForTesting(
        content.DispatcherQueue(), true);
    REQUIRE(gate != nullptr);
    sourceHost->SetSlotPhysicalDisconnectGate(button, gate);

    bool gateOpenedAtBoundary = false;
    dpi.AfterNextSetParent(
        [gate, &gateOpenedAtBoundary](wxWindow *)
        {
            gateOpenedAtBoundary =
                wxWinUISetPhysicalDisconnectGateStateForTesting(
                    gate, false);
        });
    CHECK_FALSE(button->Reparent(destination));
    REQUIRE(gateOpenedAtBoundary);
    CHECK(button->GetParent() == source);
    CHECK(::GetParent(GetHwndOf(button)) == nativeParent);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(button) == sourceHost);
    CHECK(sourceHost->FindSlot(button) == sourceSlot);
    CHECK(sourceSlot->GetContent() == content);

    REQUIRE(wxWinUISetPhysicalDisconnectGateStateForTesting(
        gate, true));
    REQUIRE(button->Reparent(destination));
    wxWinUITopLevelHost * const destinationHost =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(destinationHost != nullptr);
    REQUIRE(destinationHost != sourceHost);
    REQUIRE(destinationHost->FindSlot(button) != nullptr);
    CHECK(destinationHost->FindSlot(button)->
              GetPhysicalDisconnectGateIdentityForTest() ==
          reinterpret_cast<std::uintptr_t>(gate.get()));

    destination->Destroy();
    source->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::PhysicalGateCountersAreCausalAndBounded",
          "[HostLifecycle][popup-gate][counters][terminal]")
{
    const unsigned pending0 =
        wxWinUITopLevelHost::GetPendingPhysicalDisconnectGateCountForTest();
    const unsigned terminals0 =
        wxWinUITopLevelHost::GetPendingTLWTerminalRetirementCountForTest();

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "popup-gate-counters");
    wxButton * const neverOpened =
        new wxButton(frame, wxID_ANY, "never-opened");
    wxButton * const causal =
        new wxButton(frame, wxID_ANY, "causal-close");
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(neverOpened);
    REQUIRE(host != nullptr);
    wxWinUISlot * const neverSlot = host->FindSlot(neverOpened);
    wxWinUISlot * const causalSlot = host->FindSlot(causal);
    REQUIRE(neverSlot != nullptr);
    REQUIRE(causalSlot != nullptr);

    const auto satisfied = wxWinUICreatePhysicalDisconnectGateForTesting(
        neverSlot->GetContent().DispatcherQueue(), true);
    REQUIRE(satisfied != nullptr);
    host->SetSlotPhysicalDisconnectGate(neverOpened, satisfied);
    host->UnregisterSlot(neverOpened, satisfied);
    CHECK(host->FindSlot(neverOpened) == nullptr);
    CHECK(wxWinUITopLevelHost::
              GetPendingPhysicalDisconnectGateCountForTest() == pending0);
    CHECK(wxWinUITopLevelHost::
              GetPendingTLWTerminalRetirementCountForTest() == terminals0);

    const auto retiring = wxWinUICreatePhysicalDisconnectGateForTesting(
        causalSlot->GetContent().DispatcherQueue(), false);
    REQUIRE(retiring != nullptr);
    host->SetSlotPhysicalDisconnectGate(causal, retiring);
    host->UnregisterSlot(causal, retiring);
    CHECK(host->FindSlot(causal) == nullptr);
    CHECK(wxWinUITopLevelHost::
              GetPendingPhysicalDisconnectGateCountForTest() ==
          pending0 + 1);
    CHECK(wxWinUITopLevelHost::
              GetPendingTLWTerminalRetirementCountForTest() == terminals0);
    REQUIRE(wxWinUISetPhysicalDisconnectGateStateForTesting(
        retiring, true));
    CHECK(wxWinUITopLevelHost::
              GetPendingPhysicalDisconnectGateCountForTest() == pending0);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::TLWShutdownCollectsLivePopupGate",
          "[HostLifecycle][popup-gate][terminal][live-slot]")
{
    const unsigned pending0 =
        wxWinUITopLevelHost::GetPendingPhysicalDisconnectGateCountForTest();
    const unsigned terminals0 =
        wxWinUITopLevelHost::GetPendingTLWTerminalRetirementCountForTest();
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "live-popup-terminal");
    wxButton * const child = new wxButton(frame, wxID_ANY, "live-child");
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(child);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(child);
    REQUIRE(slot != nullptr);
    const auto gate = wxWinUICreatePhysicalDisconnectGateForTesting(
        slot->GetContent().DispatcherQueue(), false);
    REQUIRE(gate != nullptr);
    host->SetSlotPhysicalDisconnectGate(child, gate);

    // Child destruction has not run: Shutdown must discover the gate in the
    // live slot table and must not charge the deferred-slot counter for it.
    host->ShutdownForTest();
    CHECK(wxWinUITopLevelHost::FindSlotOwner(child) == nullptr);
    CHECK(wxWinUITopLevelHost::
              GetPendingPhysicalDisconnectGateCountForTest() == pending0);
    CHECK(wxWinUITopLevelHost::
              GetPendingTLWTerminalRetirementCountForTest() ==
          terminals0 + 1);
    REQUIRE(wxWinUISetPhysicalDisconnectGateStateForTesting(gate, true));
    CHECK(wxWinUITopLevelHost::
              GetPendingTLWTerminalRetirementCountForTest() ==
          terminals0);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::TLWShutdownConsumesSealedNoPopupTickets",
          "[HostLifecycle][popup-gate][terminal][seal]")
{
    const unsigned terminals0 =
        wxWinUITopLevelHost::GetPendingTLWTerminalRetirementCountForTest();
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "sealed-no-popup");
    wxWinUITopLevelHost *host = nullptr;
    std::vector<std::shared_ptr<wxWinUIPhysicalDisconnectGate>> gates;
    for ( unsigned n = 0; n != 4; ++n )
    {
        wxButton * const child = new wxButton(
            frame, wxID_ANY, wxString::Format("peer-%u", n));
        host = wxWinUITopLevelHost::FindSlotOwner(child);
        REQUIRE(host != nullptr);
        wxWinUISlot * const slot = host->FindSlot(child);
        REQUIRE(slot != nullptr);
        const auto gate = wxWinUICreatePhysicalDisconnectGateForTesting(
            slot->GetContent().DispatcherQueue(), true);
        REQUIRE(gate != nullptr);
        host->SetSlotPhysicalDisconnectGate(child, gate);
        gates.push_back(gate);
    }

    host->ShutdownForTest();
    CHECK(wxWinUITopLevelHost::
              GetPendingTLWTerminalRetirementCountForTest() ==
          terminals0);
    for ( const auto& gate : gates )
    {
        // The logical gate was consumed, but publication authority is
        // permanently sealed rather than made reusable after host shutdown.
        CHECK_FALSE(wxWinUIIsPhysicalDisconnectGateSatisfiedForTesting(
            gate));
        CHECK_FALSE(wxWinUIIsPhysicalDisconnectGateDegradedForTesting(
            gate));
    }

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::LateOpenAtSealedSweepUsesExistingTerminalLedger",
          "[HostLifecycle][popup-gate][terminal][seal][late-open]")
{
    wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting();
    const auto resetPoison = wxMakeGuard([]()
    {
        wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting();
    });
    wxUnusedVar(resetPoison);
    const unsigned terminals0 =
        wxWinUITopLevelHost::GetPendingTLWTerminalRetirementCountForTest();
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "sealed-late-open");
    wxButton * const child = new wxButton(frame, wxID_ANY, "peer");
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(child);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(child);
    REQUIRE(slot != nullptr);
    const auto gate = wxWinUICreatePhysicalDisconnectGateForTesting(
        slot->GetContent().DispatcherQueue(), true);
    REQUIRE(gate != nullptr);
    host->SetSlotPhysicalDisconnectGate(child, gate);
    REQUIRE(wxWinUIOpenPhysicalDisconnectGateAtSealedCompletionForTesting(
        gate));

    host->ShutdownForTest();
    CHECK(wxWinUIIsPhysicalDisconnectGateDegradedForTesting(gate));
    CHECK(wxWinUITopLevelHost::
              GetPendingTLWTerminalRetirementCountForTest() ==
          terminals0 + 1);
    REQUIRE(wxWinUISetPhysicalDisconnectGateStateForTesting(
        gate, true, false));
    CHECK(wxWinUITopLevelHost::
              GetPendingTLWTerminalRetirementCountForTest() ==
          terminals0);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::AcceptedOpenIsLatchedBeforeSourceClose",
          "[HostLifecycle][popup-gate][terminal][seal][pre-close]")
{
    wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting();
    const auto resetPoison = wxMakeGuard([]()
    {
        wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting();
    });
    wxUnusedVar(resetPoison);
    const unsigned terminals0 =
        wxWinUITopLevelHost::GetPendingTLWTerminalRetirementCountForTest();
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "accepted-before-opened");
    wxButton * const child = new wxButton(frame, wxID_ANY, "peer");
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(child);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(child);
    REQUIRE(slot != nullptr);
    const auto gate = wxWinUICreatePhysicalDisconnectGateForTesting(
        slot->GetContent().DispatcherQueue(), true);
    REQUIRE(gate != nullptr);
    host->SetSlotPhysicalDisconnectGate(child, gate);
    REQUIRE(wxWinUIOpenPhysicalDisconnectGateBeforeSourceCloseForTesting(
        gate));

    host->ShutdownForTest();
    CHECK(wxWinUIIsPhysicalDisconnectGateDegradedForTesting(gate));
    CHECK(wxWinUITopLevelHost::
              GetPendingTLWTerminalRetirementCountForTest() ==
          terminals0 + 1);
    REQUIRE(wxWinUISetPhysicalDisconnectGateStateForTesting(
        gate, true, false));
    CHECK(wxWinUITopLevelHost::
              GetPendingTLWTerminalRetirementCountForTest() ==
          terminals0);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::DegradedLivePeerPoisonsWithoutDetach",
          "[HostLifecycle][popup-gate][poison]")
{
    wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting();
    const auto resetPoison = wxMakeGuard([]()
    {
        wxWinUIResetPhysicalDisconnectPublicationPoisonForTesting();
    });
    wxUnusedVar(resetPoison);

    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "poison-peer");
    wxButton * const child = new wxButton(frame, wxID_ANY, "live-peer");
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(child);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(child);
    REQUIRE(slot != nullptr);
    const auto content = slot->GetContent();
    const auto gate = wxWinUICreatePhysicalDisconnectGateForTesting(
        content.DispatcherQueue(), true);
    REQUIRE(gate != nullptr);
    host->SetSlotPhysicalDisconnectGate(child, gate);

    REQUIRE(wxWinUISetPhysicalDisconnectGateStateForTesting(
        gate, false, true));
    CHECK(wxWinUIIsPhysicalDisconnectPublicationPoisoned());
    CHECK_FALSE(wxWinUITLWHostCanReparentSubtreeNow(child));
    CHECK(wxWinUITopLevelHost::FindSlotOwner(child) == host);
    CHECK(host->FindSlot(child) == slot);
    CHECK(slot->GetContent() == content);
    CHECK(slot->GetPhysicalDisconnectGateIdentityForTest() ==
          reinterpret_cast<std::uintptr_t>(gate.get()));

    REQUIRE(wxWinUISetPhysicalDisconnectGateStateForTesting(
        gate, true, false));
    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::FocusedReparentAcrossTLW",
           "[HostLifecycle][HostState][winui-009]")
{
    const unsigned migrations0 =
        wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest();
    const unsigned slots0 =
        wxWinUITopLevelHost::GetLiveSlotCount();
    const unsigned slotLifetimes0 =
        wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "focused-reparent-A");
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "focused-reparent-B");
    wxFrame * const frameC =
        new wxFrame(nullptr, wxID_ANY, "focused-reparent-C");
    wxButton * const button =
        new wxButton(frameA, wxID_ANY, "focused migrant");
    frameA->Show();
    frameB->Show();
    frameC->Show();
    DrainToQuiescence();

    frameA->Raise();
    button->SetFocus();
    DrainToQuiescence();
    wxWinUITopLevelHost *host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host != nullptr);
    REQUIRE(host->GetFocusOwner() == button);
    REQUIRE(wxWindow::FindFocus() == button);

    unsigned setEvents = 0;
    unsigned killEvents = 0;
    button->Bind(
        wxEVT_SET_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++setEvents;
            event.Skip();
        });
    button->Bind(
        wxEVT_KILL_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++killEvents;
            event.Skip();
        });

    wxFrame *current = frameA;
    for ( int i = 0; i < 20; ++i )
    {
        wxFrame * const target =
            current == frameA ? frameB : frameA;
        REQUIRE(button->Reparent(target));
        DrainToQuiescence();

        host = wxWinUITopLevelHost::FindSlotOwner(button);
        REQUIRE(host ==
                wxWinUITopLevelHost::FindForTLW(target));
        CHECK(host->GetFocusOwner() == button);
        CHECK(wxWindow::FindFocus() == button);
        CHECK(setEvents == 0);
        CHECK(killEvents == 0);
        CHECK(wxWinUITopLevelHost::
                  GetPendingFocusMigrationCountForTest() == migrations0);
        current = target;
    }

    bool reentered = false;
    bool nestedSucceeded = false;
    wxWinUITopLevelHost::TestOnNextSlotAttached(
        [&](wxWindow *attached)
        {
            if ( attached != button )
                return;

            reentered = true;
            nestedSucceeded = button->Reparent(frameC);
        });
    REQUIRE(button->Reparent(frameB));
    wxWinUITopLevelHost::TestOnNextSlotAttached({});
    DrainToQuiescence();

    REQUIRE(reentered);
    REQUIRE(nestedSucceeded);
    host = wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host ==
            wxWinUITopLevelHost::FindForTLW(frameC));
    wxWinUITopLevelHost * const hostA =
        wxWinUITopLevelHost::FindForTLW(frameA);
    wxWinUITopLevelHost * const hostB =
        wxWinUITopLevelHost::FindForTLW(frameB);
    REQUIRE(hostA != nullptr);
    REQUIRE(hostB != nullptr);
    CHECK(hostA->FindSlot(button) == nullptr);
    CHECK(hostB->FindSlot(button) == nullptr);
    CHECK(host->GetFocusOwner() == button);
    CHECK(wxWindow::FindFocus() == button);
    CHECK(setEvents == 0);
    CHECK(killEvents == 0);
    CHECK(wxWinUITopLevelHost::
              GetPendingFocusMigrationCountForTest() == migrations0);

    frameA->Destroy();
    frameB->Destroy();
    frameC->Destroy();
    DrainDispatch();
    CHECK(wxWinUITopLevelHost::
              GetPendingFocusMigrationCountForTest() == migrations0);
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == slots0);
    CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
          slotLifetimes0);
}

TEST_CASE("HostLifecycle::DeferredFocusSurvivesReentrantTripleAttach",
          "[HostLifecycle][HostState][focus][loaded][winui-009]")
{
    const unsigned pending0 =
        wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest();
    const unsigned slots0 = wxWinUITopLevelHost::GetLiveSlotCount();
    const unsigned lifetimes0 =
        wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
    const unsigned adds0 =
        wxWinUITopLevelHost::GetSlotHandlerAddCount();
    const unsigned revokes0 =
        wxWinUITopLevelHost::GetSlotHandlerRevokeCount();

    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "deferred-focus-A");
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "deferred-focus-B");
    wxFrame * const frameC =
        new wxFrame(nullptr, wxID_ANY, "deferred-focus-C");
    wxButton * const target =
        new wxButton(frameA, wxID_ANY, "deferred target",
                     wxPoint(10, 10));
    wxWindow * const holder =
        new wxWindow(frameA, wxID_ANY,
                     wxPoint(180, 10), wxSize(60, 30));

    ShowOffscreenWithoutActivating(frameA, 0);
    ShowOffscreenWithoutActivating(frameB, 1);
    ShowOffscreenWithoutActivating(frameC, 2);
    DrainToQuiescence();

    wxWinUITopLevelHost * const hostA =
        wxWinUITopLevelHost::FindSlotOwner(target);
    REQUIRE(hostA != nullptr);
    REQUIRE(hostA == wxWinUITopLevelHost::FindForTLW(frameA));

    wxWinUISlot * const slotA = hostA->FindSlot(target);
    REQUIRE(slotA != nullptr);

    holder->SetFocus();
    DrainDispatch(2);
    const HWND holderHwnd = GetHwndOf(holder);
    REQUIRE(holderHwnd != nullptr);
    REQUIRE(::GetFocus() == holderHwnd);
    REQUIRE(wxWindow::FindFocus() == holder);

    unsigned targetSets = 0;
    unsigned targetKills = 0;
    unsigned holderKills = 0;
    target->Bind(
        wxEVT_SET_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++targetSets;
            event.Skip();
        });
    target->Bind(
        wxEVT_KILL_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++targetKills;
            event.Skip();
        });
    holder->Bind(
        wxEVT_KILL_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++holderKills;
            event.Skip();
        });

    // Queue a logical request while the peer is unloaded. No logical focus
    // owner exists yet and the native focus must remain on the holder.
    slotA->SetContentLoadedForTest(false);
    hostA->FocusSlot(target);

    CHECK(hostA->GetFocusOwner() != target);
    CHECK(::GetFocus() == holderHwnd);
    CHECK(wxWindow::FindFocus() == holder);
    CHECK(targetSets == 0);
    CHECK(targetKills == 0);
    CHECK(holderKills == 0);
    CHECK(wxWinUITopLevelHost::
              GetPendingFocusMigrationCountForTest() == pending0);

    bool sawAttachB = false;
    bool sawAttachC = false;
    bool nestedSucceeded = false;
    wxScopeGuard hookGuard = wxMakeGuard(
        []()
        {
            wxWinUITopLevelHost::TestOnNextSlotAttached({});
        });
    wxUnusedVar(hookGuard);

    wxWinUITopLevelHost::TestOnNextSlotAttached(
        [&](wxWindow *attachedB)
        {
            if ( attachedB != target )
                return;

            sawAttachB = true;
            wxWinUITopLevelHost * const hostB =
                wxWinUITopLevelHost::FindSlotOwner(target);
            REQUIRE(hostB == wxWinUITopLevelHost::FindForTLW(frameB));

            wxWinUISlot * const slotB = hostB->FindSlot(target);
            REQUIRE(slotB != nullptr);
            slotB->SetContentLoadedForTest(false);

            // A to B owns exactly one transport ticket.
            CHECK(wxWinUITopLevelHost::
                      GetPendingFocusMigrationCountForTest() == pending0 + 1);
            CHECK(hostB->GetFocusOwner() != target);

            // The attach hook is one-shot; arm it again for B to C.
            wxWinUITopLevelHost::TestOnNextSlotAttached(
                [&](wxWindow *attachedC)
                {
                    if ( attachedC != target )
                        return;

                    sawAttachC = true;
                    wxWinUITopLevelHost * const hostC =
                        wxWinUITopLevelHost::FindSlotOwner(target);
                    REQUIRE(hostC ==
                            wxWinUITopLevelHost::FindForTLW(frameC));

                    wxWinUISlot * const slotC = hostC->FindSlot(target);
                    REQUIRE(slotC != nullptr);
                    slotC->SetContentLoadedForTest(false);

                    // The second attach forwards the same ticket.
                    CHECK(wxWinUITopLevelHost::
                              GetPendingFocusMigrationCountForTest() ==
                          pending0 + 1);
                    CHECK(hostC->GetFocusOwner() != target);
                });

            nestedSucceeded = target->Reparent(frameC);
        });

    // Do not drain between the deferred request and completion of the
    // re-entrant A to B to C topology transaction.
    const bool outerSucceeded = target->Reparent(frameB);
    wxWinUITopLevelHost::TestOnNextSlotAttached({});

    REQUIRE(outerSucceeded);
    REQUIRE(sawAttachB);
    REQUIRE(nestedSucceeded);
    REQUIRE(sawAttachC);

    wxWinUITopLevelHost * const hostB =
        wxWinUITopLevelHost::FindForTLW(frameB);
    wxWinUITopLevelHost * const hostC =
        wxWinUITopLevelHost::FindForTLW(frameC);
    REQUIRE(hostB != nullptr);
    REQUIRE(hostC != nullptr);

    CHECK(wxWinUITopLevelHost::FindSlotOwner(target) == hostC);
    CHECK(hostA->FindSlot(target) == nullptr);
    CHECK(hostB->FindSlot(target) == nullptr);

    wxWinUISlot * const slotC = hostC->FindSlot(target);
    REQUIRE(slotC != nullptr);
    REQUIRE_FALSE(slotC->IsContentLoadedForTest());

    // The migration ticket is gone, but C still owns the original deferred
    // request. It must become effective without another FocusSlot() call.
    CHECK(wxWinUITopLevelHost::
              GetPendingFocusMigrationCountForTest() == pending0);
    CHECK(hostC->GetFocusOwner() != target);
    CHECK(::GetFocus() == holderHwnd);
    CHECK(wxWindow::FindFocus() == holder);
    CHECK(targetSets == 0);
    CHECK(targetKills == 0);
    CHECK(holderKills == 0);

    slotC->SetContentLoadedForTest(true);
    hostC->FlushSync();
    DrainToQuiescence();

    CHECK(hostC->GetFocusOwner() == target);
    CHECK(wxWindow::FindFocus() == target);
    CHECK(targetSets == 1);
    CHECK(targetKills == 0);
    CHECK(holderKills == 1);
    CHECK(wxWinUITopLevelHost::
              GetPendingFocusMigrationCountForTest() == pending0);

    frameA->Destroy();
    frameB->Destroy();
    frameC->Destroy();
    DrainToQuiescence();

    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == slots0);
    CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() == lifetimes0);
    CHECK(wxWinUITopLevelHost::
              GetPendingFocusMigrationCountForTest() == pending0);

    const unsigned adds =
        wxWinUITopLevelHost::GetSlotHandlerAddCount() - adds0;
    const unsigned revokes =
        wxWinUITopLevelHost::GetSlotHandlerRevokeCount() - revokes0;
    CHECK(adds > 0);
    CHECK(adds == revokes);
}

TEST_CASE("HostLifecycle::FocusedReparentHonoursNativeRedirect",
          "[HostLifecycle][HostState][winui-009]")
{
    const unsigned migrations0 =
        wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest();
    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "focused-redirect-A");
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "focused-redirect-B");
    wxButton * const migrant =
        new wxButton(frameA, wxID_ANY, "redirected migrant");
    wxWindow * const redirect =
        new wxWindow(frameB, wxID_ANY, wxPoint(10, 10), wxSize(40, 30));
    frameA->Show();
    frameB->Show();
    DrainToQuiescence();

    frameA->Raise();
    migrant->SetFocus();
    DrainToQuiescence();
    wxWinUITopLevelHost *host =
        wxWinUITopLevelHost::FindSlotOwner(migrant);
    REQUIRE(host != nullptr);
    REQUIRE(host->GetFocusOwner() == migrant);
    REQUIRE(wxWindow::FindFocus() == migrant);

    unsigned setEvents = 0;
    unsigned killEvents = 0;
    bool killCounterpartWasRedirect = false;
    migrant->Bind(
        wxEVT_SET_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++setEvents;
            event.Skip();
        });
    migrant->Bind(
        wxEVT_KILL_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++killEvents;
            killCounterpartWasRedirect = event.GetWindow() == redirect;
            event.Skip();
        });

    const HWND redirectHwnd = GetHwndOf(redirect);
    REQUIRE(redirectHwnd != nullptr);
    bool hookRan = false;
    wxWinUITopLevelHost::TestOnNextSlotAttached(
        [&](wxWindow *attached)
        {
            if ( attached == migrant )
            {
                hookRan = true;
                ::SetFocus(redirectHwnd);
            }
        });
    REQUIRE(migrant->Reparent(frameB));
    wxWinUITopLevelHost::TestOnNextSlotAttached({});
    DrainToQuiescence();

    REQUIRE(hookRan);
    CHECK(::GetFocus() == redirectHwnd);
    CHECK(wxWindow::FindFocus() == redirect);
    CHECK(setEvents == 0);
    CHECK(killEvents == 1);
    CHECK(killCounterpartWasRedirect);
    host = wxWinUITopLevelHost::FindSlotOwner(migrant);
    REQUIRE(host != nullptr);
    CHECK(host->GetFocusOwner() != migrant);
    CHECK(wxWinUITopLevelHost::
              GetPendingFocusMigrationCountForTest() == migrations0);

    frameA->Destroy();
    frameB->Destroy();
    DrainDispatch();
    CHECK(wxWinUITopLevelHost::
              GetPendingFocusMigrationCountForTest() == migrations0);
}

TEST_CASE("HostLifecycle::FocusedReparentHonoursGettingFocusRedirect",
          "[HostLifecycle][HostState][winui-009]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
    namespace MUXI = winrt::Microsoft::UI::Xaml::Input;

    const unsigned migrations0 =
        wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest();
    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "focused-getting-A");
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "focused-getting-B");
    wxButton * const migrant =
        new wxButton(frameA, wxID_ANY, "getting-focus migrant");
    wxWindow * const redirect =
        new wxWindow(frameB, wxID_ANY, wxPoint(10, 10), wxSize(40, 30));
    frameA->Show();
    frameB->Show();
    DrainToQuiescence();

    frameA->Raise();
    migrant->SetFocus();
    DrainToQuiescence();
    REQUIRE(wxWindow::FindFocus() == migrant);

    unsigned setEvents = 0;
    unsigned killEvents = 0;
    migrant->Bind(
        wxEVT_SET_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++setEvents;
            event.Skip();
        });
    migrant->Bind(
        wxEVT_KILL_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++killEvents;
            event.Skip();
        });

    const HWND redirectHwnd = GetHwndOf(redirect);
    REQUIRE(redirectHwnd != nullptr);
    bool hookRan = false;
    bool gettingFocusRan = false;
    MUXC::Button destinationPeer{ nullptr };
    winrt::event_token gettingFocusToken{};
    wxWinUITopLevelHost::TestOnNextSlotAttached(
        [&](wxWindow *attached)
        {
            if ( attached != migrant )
                return;

            hookRan = true;
            wxWinUITopLevelHost * const owner =
                wxWinUITopLevelHost::FindSlotOwner(migrant);
            wxWinUISlot * const slot =
                owner ? owner->FindSlot(migrant) : nullptr;
            REQUIRE(slot != nullptr);
            destinationPeer =
                slot->GetContent().try_as<MUXC::Button>();
            REQUIRE(destinationPeer != nullptr);
            gettingFocusToken = destinationPeer.GettingFocus(
                [&](const MUX::UIElement&,
                    const MUXI::GettingFocusEventArgs&)
                {
                    gettingFocusRan = true;
                    ::SetFocus(redirectHwnd);
                });
        });
    REQUIRE(migrant->Reparent(frameB));
    wxWinUITopLevelHost::TestOnNextSlotAttached({});
    DrainToQuiescence();

    if ( destinationPeer )
        destinationPeer.GettingFocus(gettingFocusToken);
    REQUIRE(hookRan);
    REQUIRE(gettingFocusRan);
    CHECK(::GetFocus() == redirectHwnd);
    CHECK(wxWindow::FindFocus() == redirect);
    CHECK(setEvents == 0);
    CHECK(killEvents == 1);
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(migrant);
    REQUIRE(host != nullptr);
    CHECK(host->GetFocusOwner() != migrant);
    CHECK(wxWinUITopLevelHost::
              GetPendingFocusMigrationCountForTest() == migrations0);

    frameA->Destroy();
    frameB->Destroy();
    DrainDispatch();
    CHECK(wxWinUITopLevelHost::
              GetPendingFocusMigrationCountForTest() == migrations0);
}

TEST_CASE("HostLifecycle::FreezeThawCoalescedFlush", "[HostLifecycle]")
{
    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "lifecycle-freeze");
    wxButton * const button = new wxButton(frame, wxID_ANY, "frozen");
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    REQUIRE(host->FindSlot(button) != nullptr);

    const unsigned sched0 = wxWinUITopLevelHost::GetFlushScheduleCount();
    const unsigned runs0 = wxWinUITopLevelHost::GetFlushRunCount();
    const unsigned attempts0 =
        wxWinUITopLevelHost::GetFlushCallbackAttemptCount();

    // Queue one callback BEFORE the freeze.  It must run while frozen, record
    // that a catch-up is owed and stop -- without re-posting itself.
    host->MarkDirty(button);
    REQUIRE(wxWinUITopLevelHost::GetFlushScheduleCount() - sched0 == 1);
    frame->Freeze();
    DrainDispatch(5);
    CHECK(wxWinUITopLevelHost::GetFlushCallbackAttemptCount() - attempts0 == 1);
    CHECK(wxWinUITopLevelHost::GetFlushRunCount() - runs0 == 0);

    wxPoint last;
    for ( int i = 0; i < 20; ++i )
    {
        last = wxPoint(10 + i, 10);
        button->Move(last);
        // A drain between every mutation is what exposes a rescheduling loop.
        DrainDispatch(1);
    }

    // Strictly bounded freeze: the only schedule/callback is the one posted
    // before Freeze(); the Move storm added neither.
    CHECK(wxWinUITopLevelHost::GetFlushScheduleCount() - sched0 == 1);
    CHECK(wxWinUITopLevelHost::GetFlushCallbackAttemptCount() - attempts0 == 1);
    CHECK(wxWinUITopLevelHost::GetFlushRunCount() - runs0 == 0);

    // Nested freeze: the first Thaw still leaves the TLW frozen and must not
    // release the catch-up.
    frame->Freeze();
    frame->Thaw();
    DrainDispatch(5);
    CHECK(wxWinUITopLevelHost::GetFlushScheduleCount() - sched0 == 1);
    CHECK(wxWinUITopLevelHost::GetFlushRunCount() - runs0 == 0);

    frame->Thaw();
    DrainDispatch(5);

    // exactly ONE catch-up flush was scheduled and executed at thaw time,
    // and it brought the geometry current
    CHECK(wxWinUITopLevelHost::GetFlushScheduleCount() - sched0 == 2);
    CHECK(wxWinUITopLevelHost::GetFlushCallbackAttemptCount() - attempts0 == 2);
    CHECK(wxWinUITopLevelHost::GetFlushRunCount() - runs0 == 1);
    wxWinUISlot * const slot = host->FindSlot(button);
    REQUIRE(slot != nullptr);
    CHECK(slot->GetRectInTLW().x == last.x);
    CHECK(slot->GetRectInTLW().y == last.y);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::FrozenTooltipPolicyIsSynchronous",
          "[HostState][HostLifecycle][tooltip][reentrant][winui-008f]")
{
#if wxUSE_TOOLTIPS
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    struct PolicyReset
    {
        ~PolicyReset() { wxToolTip::Enable(true); }
    } reset;
    wxToolTip::Enable(true);

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "frozen-tooltip-policy");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"><ToolTipService.ToolTip>"
        "<ToolTip Content=\"frozen-authored\"/>"
        "</ToolTipService.ToolTip></Border>"));
    xamlHost->SetSize(10, 10, 120, 40);
    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto content = slot->GetContent();
    REQUIRE(content != nullptr);
    const auto authored = MUXC::ToolTipService::GetToolTip(content);
    REQUIRE(authored != nullptr);
    const auto authoredObject = authored.try_as<MUXC::ToolTip>();
    REQUIRE(authoredObject != nullptr);

    MUXC::ToolTip closedReplacement;
    closedReplacement.Content(
        winrt::box_value(L"frozen-deferred-replacement"));
    authoredObject.PlacementTarget(content);
    authoredObject.IsOpen(true);
    REQUIRE(authoredObject.IsOpen());

    const wxRect geometryBefore = slot->GetRectInTLW();
    const unsigned runsBefore = host->GetOwnFlushRunCountForTest();
    frame->Freeze();
    xamlHost->Move(70, 35);
    CHECK(slot->GetRectInTLW() == geometryBefore);

    // Popup closure is asynchronous and the private off-screen test desktop
    // does not present ToolTip popups. Publish the same late application write
    // in the next dispatcher turn: the ToolTipProperty observer must suppress
    // it inside that setter even though the complete host is frozen, without
    // consuming the owed geometry catch-up.
    wxToolTip::Enable(false);
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    struct DeferredWrite
    {
        bool completed = false;
        bool suppressedInline = false;
    };
    const auto deferredWrite = std::make_shared<DeferredWrite>();
    wxTheApp->CallAfter(
        [deferredWrite, content, closedReplacement]()
        {
            MUXC::ToolTipService::SetToolTip(
                content, closedReplacement);
            deferredWrite->suppressedInline =
                MUXC::ToolTipService::GetToolTip(content) == nullptr;
            deferredWrite->completed = true;
        });
    REQUIRE(DrainUntil(
        [deferredWrite]() { return deferredWrite->completed; }));
    REQUIRE(deferredWrite->suppressedInline);
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    CHECK(slot->GetRectInTLW() == geometryBefore);
    CHECK(host->GetOwnFlushRunCountForTest() == runsBefore);

    wxToolTip::Enable(true);
    CHECK(MUXC::ToolTipService::GetToolTip(content) ==
          closedReplacement);
    CHECK(slot->GetRectInTLW() == geometryBefore);
    CHECK(host->GetOwnFlushRunCountForTest() == runsBefore);

    // The detach setter itself is re-entrant too. A replacement published
    // from its property callback becomes the exact new restoration baseline
    // and is nevertheless hidden before Enable(false) returns.
    MUXC::ToolTip detachReplacement;
    detachReplacement.Content(
        winrt::box_value(L"frozen-detach-replacement"));
    bool detachReplaced = false;
    const int64_t detachToken =
        content.RegisterPropertyChangedCallback(
            MUXC::ToolTipService::ToolTipProperty(),
            [&](MUX::DependencyObject const&,
                MUX::DependencyProperty const&)
            {
                if ( detachReplaced ||
                     MUXC::ToolTipService::GetToolTip(content) )
                {
                    return;
                }
                detachReplaced = true;
                MUXC::ToolTipService::SetToolTip(
                    content, detachReplacement);
            });
    wxToolTip::Enable(false);
    content.UnregisterPropertyChangedCallback(
        MUXC::ToolTipService::ToolTipProperty(), detachToken);
    REQUIRE(detachReplaced);
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    CHECK(slot->GetRectInTLW() == geometryBefore);
    CHECK(host->GetOwnFlushRunCountForTest() == runsBefore);

    wxToolTip::Enable(true);
    CHECK(MUXC::ToolTipService::GetToolTip(content) ==
          detachReplacement);
    CHECK(slot->GetRectInTLW() == geometryBefore);
    CHECK(host->GetOwnFlushRunCountForTest() == runsBefore);

    frame->Thaw();
    DrainToQuiescence();
    CHECK(slot->GetRectInTLW().x == 70);
    CHECK(slot->GetRectInTLW().y == 35);
    CHECK(host->GetOwnFlushRunCountForTest() == runsBefore + 1);

    frame->Destroy();
    DrainDispatch();
#endif // wxUSE_TOOLTIPS
}

TEST_CASE("HostLifecycle::TopLevelWindowIsASlot", "[HostLifecycle]")
{
    const unsigned hosts0 = wxWinUITopLevelHost::GetLiveHostCount();
    const unsigned slotStates0 =
        wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
    const unsigned hostStates0 =
        wxWinUITopLevelHost::GetLiveHostLifetimeCount();
    const unsigned slotAdds0 =
        wxWinUITopLevelHost::GetSlotHandlerAddCount();
    const unsigned slotRevokes0 =
        wxWinUITopLevelHost::GetSlotHandlerRevokeCount();
    const unsigned rootAdds0 =
        wxWinUITopLevelHost::GetRootHandlerAddCount();
    const unsigned rootRevokes0 =
        wxWinUITopLevelHost::GetRootHandlerRevokeCount();

    // Frame-class dialogs register their TLW itself as a slot.  Destroying it
    // dispatches wxEVT_DESTROY through both the slot and TLW handlers; the
    // slot handler must leave the visual tree attached for source.Close().
    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "tlw-slot");
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(frame, true);
    REQUIRE(host != nullptr);
    REQUIRE(host->RegisterSlot(
                frame,
                winrt::Microsoft::UI::Xaml::Controls::Border()) != nullptr);
    frame->Show();
    DrainToQuiescence();

    frame->Destroy();
    DrainDispatch();

    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hosts0);
    CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() == slotStates0);
    CHECK(wxWinUITopLevelHost::GetLiveHostLifetimeCount() == hostStates0);
    CHECK(wxWinUITopLevelHost::GetSlotHandlerAddCount() - slotAdds0 ==
          wxWinUITopLevelHost::GetSlotHandlerRevokeCount() - slotRevokes0);
    CHECK(wxWinUITopLevelHost::GetRootHandlerAddCount() - rootAdds0 ==
          wxWinUITopLevelHost::GetRootHandlerRevokeCount() - rootRevokes0);
}

TEST_CASE("HostLifecycle::ReparentIsSynchronous", "[HostLifecycle]")
{
    wxFrame * const frameA = new wxFrame(nullptr, wxID_ANY, "reparent-A");
    wxFrame * const frameB = new wxFrame(nullptr, wxID_ANY, "reparent-B");
    wxPanel * const panel = new wxPanel(frameA);
    wxButton * const child = new wxButton(panel, wxID_ANY, "child");
    wxButton * const direct = new wxButton(frameA, wxID_ANY, "direct");
    frameA->Show();
    frameB->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const hostA =
        wxWinUITopLevelHost::FindForTLW(frameA);
    REQUIRE(hostA != nullptr);
    REQUIRE(hostA->FindSlot(child) != nullptr);
    REQUIRE(hostA->FindSlot(direct) != nullptr);

    // No yield between Reparent() and the ownership checks: both a direct slot
    // and a slotted descendant of a native container move synchronously.
    REQUIRE(direct->Reparent(frameB));
    REQUIRE(panel->Reparent(frameB));

    wxWinUITopLevelHost * const hostB =
        wxWinUITopLevelHost::FindForTLW(frameB);
    REQUIRE(hostB != nullptr);
    CHECK(hostA->FindSlot(direct) == nullptr);
    CHECK(hostA->FindSlot(child) == nullptr);
    REQUIRE(hostB->FindSlot(direct) != nullptr);
    REQUIRE(hostB->FindSlot(child) != nullptr);

    // The source TLW may die immediately; the migrated controls must remain
    // alive and registered in the target host.
    frameA->Destroy();
    DrainDispatch(3);
    direct->SetLabel("still alive");
    child->SetLabel("still alive too");
    CHECK(hostB->FindSlot(direct) != nullptr);
    CHECK(hostB->FindSlot(child) != nullptr);

    frameB->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::ReparentDeliversOrderedDPITransitions",
          "[HostLifecycle][winui-009][dpi-reparent]")
{
    SyntheticReparentDPI dpi;
    wxFrame * const frame96 =
        new wxFrame(nullptr, wxID_ANY, "dpi-reparent-96");
    wxFrame * const frame144 =
        new wxFrame(nullptr, wxID_ANY, "dpi-reparent-144");
    wxFrame * const frame144Peer =
        new wxFrame(nullptr, wxID_ANY, "dpi-reparent-144-peer");
    wxFrame * const frame192 =
        new wxFrame(nullptr, wxID_ANY, "dpi-reparent-192");
    ShowOffscreenWithoutActivating(frame96, 0);
    ShowOffscreenWithoutActivating(frame144, 1);
    ShowOffscreenWithoutActivating(frame144Peer, 2);
    ShowOffscreenWithoutActivating(frame192, 3);

    dpi.SetTLWDPI(frame96, 96);
    dpi.SetTLWDPI(frame144, 144);
    dpi.SetTLWDPI(frame144Peer, 144);
    dpi.SetTLWDPI(frame192, 192);
    wxPanel * const panel = new wxPanel(frame96);
    wxButton * const child =
        new wxButton(panel, wxID_ANY, "dpi child");

    std::vector<wxString> transitions;
    const auto record =
        [&](const wxString& owner, wxDPIChangedEvent& event)
        {
            transitions.push_back(
                wxString::Format(
                    "%s:%d>%d",
                    owner,
                    event.GetOldDPI().GetWidth(),
                    event.GetNewDPI().GetWidth()));
            event.Skip();
        };
    child->Bind(
        wxEVT_DPI_CHANGED,
        [&](wxDPIChangedEvent& event) { record("child", event); });
    panel->Bind(
        wxEVT_DPI_CHANGED,
        [&](wxDPIChangedEvent& event) { record("panel", event); });

    // Reparent() itself is the gate: no yield is needed to deliver one
    // children-before-parent transition for the original subtree.
    REQUIRE(panel->Reparent(frame144));
    REQUIRE(transitions.size() == 2);
    CHECK(transitions[0] == "child:96>144");
    CHECK(transitions[1] == "panel:96>144");

    transitions.clear();
    REQUIRE(panel->Reparent(frame144Peer));
    CHECK(transitions.empty());

    // wxWindowBase::Reparent() updates logical child lists before USER32.
    // Re-enter by moving the requested parent B to C from B::AddChild(root):
    // the nested B->C traversal must exclude root because root's HWND is
    // still physically under A. The outer operation then performs the sole
    // real root transition A->C.
    ReparentOnAddChildPanel * const reentrantTarget =
        new ReparentOnAddChildPanel(frame144);
    wxPanel * const preBoundaryRoot = new wxPanel(frame96);
    preBoundaryRoot->SetMinSize(wxSize(80, 40));
    std::vector<wxString> preBoundaryTransitions;
    preBoundaryRoot->Bind(
        wxEVT_DPI_CHANGED,
        [&](wxDPIChangedEvent& event)
        {
            preBoundaryTransitions.push_back(
                wxString::Format(
                    "root:%d>%d",
                    event.GetOldDPI().GetWidth(),
                    event.GetNewDPI().GetWidth()));
            event.Skip();
        });
    reentrantTarget->Arm(preBoundaryRoot, frame192);
    REQUIRE(preBoundaryRoot->Reparent(reentrantTarget));
    REQUIRE(reentrantTarget->WasTriggered());
    REQUIRE(reentrantTarget->NestedReparentSucceeded());
    CHECK(reentrantTarget->GetParent() == frame192);
    CHECK(preBoundaryRoot->GetParent() == reentrantTarget);
    CHECK(wxGetTopLevelParent(preBoundaryRoot) == frame192);
    CHECK(::GetParent(GetHwndOf(preBoundaryRoot)) ==
          GetHwndOf(reentrantTarget));
    REQUIRE(preBoundaryTransitions.size() == 1);
    CHECK(preBoundaryTransitions[0] == "root:96>192");
    CHECK(preBoundaryRoot->GetMinSize() == wxSize(160, 80));

    // A nested B->C reparent from the child's A->B handler is queued behind
    // the current transition. It must not recurse into the child iterator or
    // deliver 144->192 before the parent receives 96->144.
    wxPanel * const nestedPanel = new wxPanel(frame96);
    wxButton * const nestedChild =
        new wxButton(nestedPanel, wxID_ANY, "nested dpi child");
    std::vector<wxString> nestedTransitions;
    bool nestedReparented = false;
    nestedChild->Bind(
        wxEVT_DPI_CHANGED,
        [&](wxDPIChangedEvent& event)
        {
            nestedTransitions.push_back(
                wxString::Format(
                    "child:%d>%d",
                    event.GetOldDPI().GetWidth(),
                    event.GetNewDPI().GetWidth()));
            if ( !nestedReparented &&
                 event.GetOldDPI().GetWidth() == 96 &&
                 event.GetNewDPI().GetWidth() == 144 )
            {
                nestedReparented = true;
                REQUIRE(nestedPanel->Reparent(frame192));
            }
            event.Skip();
        });
    nestedPanel->Bind(
        wxEVT_DPI_CHANGED,
        [&](wxDPIChangedEvent& event)
        {
            nestedTransitions.push_back(
                wxString::Format(
                    "panel:%d>%d",
                    event.GetOldDPI().GetWidth(),
                    event.GetNewDPI().GetWidth()));
            event.Skip();
        });

    REQUIRE(nestedPanel->Reparent(frame144));
    REQUIRE(nestedReparented);
    CHECK(nestedPanel->GetParent() == frame192);
    REQUIRE(nestedTransitions.size() == 4);
    CHECK(nestedTransitions[0] == "child:96>144");
    CHECK(nestedTransitions[1] == "panel:96>144");
    CHECK(nestedTransitions[2] == "child:144>192");
    CHECK(nestedTransitions[3] == "panel:144>192");

    wxWinUITopLevelHost * const finalHost =
        wxWinUITopLevelHost::FindSlotOwner(nestedChild);
    REQUIRE(finalHost != nullptr);
    CHECK(finalHost == wxWinUITopLevelHost::FindForTLW(frame192));
    CHECK(finalHost->FindSlot(nestedChild) != nullptr);

    // If an already-processed child moves below a later sibling from its DPI
    // handler, the immutable request must not resnapshot and scale it twice.
    wxPanel * const movingRoot = new wxPanel(frame96);
    wxPanel * const first = new wxPanel(movingRoot);
    wxPanel * const second = new wxPanel(movingRoot);
    movingRoot->SetMinSize(wxSize(80, 40));
    first->SetMinSize(wxSize(40, 20));
    unsigned firstEvents = 0;
    unsigned secondEvents = 0;
    unsigned movingRootEvents = 0;
    bool firstSawPreparedParent = false;
    first->Bind(
        wxEVT_DPI_CHANGED,
        [&](wxDPIChangedEvent& event)
        {
            ++firstEvents;
            firstSawPreparedParent =
                movingRoot->GetMinSize() == wxSize(120, 60);
            REQUIRE(first->Reparent(second));
            event.Skip();
        });
    second->Bind(
        wxEVT_DPI_CHANGED,
        [&](wxDPIChangedEvent& event)
        {
            ++secondEvents;
            event.Skip();
        });
    movingRoot->Bind(
        wxEVT_DPI_CHANGED,
        [&](wxDPIChangedEvent& event)
        {
            ++movingRootEvents;
            event.Skip();
        });
    REQUIRE(movingRoot->Reparent(frame144));
    CHECK(firstEvents == 1);
    CHECK(secondEvents == 1);
    CHECK(movingRootEvents == 1);
    CHECK(firstSawPreparedParent);
    CHECK(movingRoot->GetMinSize() == wxSize(120, 60));
    CHECK(first->GetMinSize() == wxSize(60, 30));

    // The ordinary WM_DPICHANGED driver uses the same immutable generation,
    // not the historic recursive resnapshot. Exercise it directly without a
    // monitor move: a processed child moved below its later sibling remains
    // scaled and notified exactly once.
    wxPanel * const normalRoot = new wxPanel(frame96);
    wxPanel * const normalFirst = new wxPanel(normalRoot);
    wxPanel * const normalSecond = new wxPanel(normalRoot);
    normalFirst->SetMinSize(wxSize(40, 20));
    unsigned normalFirstEvents = 0;
    unsigned normalSecondEvents = 0;
    unsigned normalRootEvents = 0;
    normalFirst->Bind(
        wxEVT_DPI_CHANGED,
        [&](wxDPIChangedEvent& event)
        {
            ++normalFirstEvents;
            REQUIRE(normalFirst->Reparent(normalSecond));
            event.Skip();
        });
    normalSecond->Bind(
        wxEVT_DPI_CHANGED,
        [&](wxDPIChangedEvent& event)
        {
            ++normalSecondEvents;
            event.Skip();
        });
    normalRoot->Bind(
        wxEVT_DPI_CHANGED,
        [&](wxDPIChangedEvent& event)
        {
            ++normalRootEvents;
            event.Skip();
        });
    (void)normalRoot->MSWUpdateOnDPIChange(
        wxSize(96, 96), wxSize(144, 144));
    CHECK(normalFirstEvents == 1);
    CHECK(normalSecondEvents == 1);
    CHECK(normalRootEvents == 1);
    CHECK(normalFirst->GetMinSize() == wxSize(60, 30));

    // Membership is frozen after the logical base-reparent and target-DPI
    // query, at the last callback-free point before SetParent(). A child
    // created by that final query really crosses the native A->B boundary and
    // must therefore receive the transition.
    wxPanel * const boundaryRoot = new wxPanel(frame96);
    wxButton *boundaryChild = nullptr;
    unsigned boundaryChildEvents = 0;
    dpi.OnNextTargetDPIQuery(
        boundaryRoot,
        frame144,
        [&](wxWindow *queried)
        {
            REQUIRE(queried == boundaryRoot);
            boundaryChild =
                new wxButton(boundaryRoot, wxID_ANY, "boundary child");
            boundaryChild->Bind(
                wxEVT_DPI_CHANGED,
                [&](wxDPIChangedEvent& event)
                {
                    ++boundaryChildEvents;
                    event.Skip();
                });
        });
    REQUIRE(boundaryRoot->Reparent(frame144));
    REQUIRE(boundaryChild != nullptr);
    CHECK(boundaryChildEvents == 1);

    // The inverse is just as important: a child in the frozen snapshot did
    // cross A->B even if a synchronous SetParent callback immediately moves
    // it back out. It receives A->B before its queued B->A transition; current
    // topology must never be used to erase the already-crossed boundary.
    wxPanel * const crossedRoot = new wxPanel(frame96);
    wxButton * const crossedChild =
        new wxButton(crossedRoot, wxID_ANY, "crossed then moved");
    std::vector<wxString> crossedTransitions;
    crossedChild->Bind(
        wxEVT_DPI_CHANGED,
        [&](wxDPIChangedEvent& event)
        {
            crossedTransitions.push_back(
                wxString::Format(
                    "child:%d>%d",
                    event.GetOldDPI().GetWidth(),
                    event.GetNewDPI().GetWidth()));
            event.Skip();
        });
    crossedRoot->Bind(
        wxEVT_DPI_CHANGED,
        [&](wxDPIChangedEvent& event)
        {
            crossedTransitions.push_back(
                wxString::Format(
                    "root:%d>%d",
                    event.GetOldDPI().GetWidth(),
                    event.GetNewDPI().GetWidth()));
            event.Skip();
        });
    bool movedAfterBoundary = false;
    dpi.AfterNextSetParent(
        [&](wxWindow *window)
        {
            REQUIRE(window == crossedRoot);
            movedAfterBoundary = true;
            REQUIRE(crossedChild->Reparent(frame96));
        });
    REQUIRE(crossedRoot->Reparent(frame144));
    REQUIRE(movedAfterBoundary);
    CHECK(crossedRoot->GetParent() == frame144);
    CHECK(crossedChild->GetParent() == frame96);
    REQUIRE(crossedTransitions.size() == 3);
    CHECK(crossedTransitions[0] == "child:96>144");
    CHECK(crossedTransitions[1] == "root:96>144");
    CHECK(crossedTransitions[2] == "child:144>96");

    // A target queued for private WinUI destruction still has a live weak
    // reference and HWND. The target-DPI callback is the final application
    // boundary before the subtree snapshot, so the scheduled-destroy state
    // must reject it and roll the logical base-reparent back without ever
    // crossing the native boundary.
    wxFrame * const doomedAtQuery =
        new wxFrame(nullptr, wxID_ANY, "doomed-at-dpi-query");
    ShowOffscreenWithoutActivating(doomedAtQuery, 4);
    new wxButton(doomedAtQuery, wxID_ANY, "prime doomed host");
    dpi.SetTLWDPI(doomedAtQuery, 144);
    wxPanel * const queryRollbackRoot = new wxPanel(frame96);
    wxButton * const queryRollbackChild =
        new wxButton(queryRollbackRoot, wxID_ANY, "query rollback child");
    wxWinUITopLevelHost * const querySourceOwner =
        wxWinUITopLevelHost::FindSlotOwner(queryRollbackChild);
    REQUIRE(querySourceOwner != nullptr);
    const HWND queryOriginalNativeParent =
        ::GetParent(GetHwndOf(queryRollbackRoot));
    unsigned queryRollbackEvents = 0;
    queryRollbackRoot->Bind(
        wxEVT_DPI_CHANGED,
        [&](wxDPIChangedEvent& event)
        {
            ++queryRollbackEvents;
            event.Skip();
        });
    dpi.OnNextTargetDPIQuery(
        queryRollbackRoot,
        doomedAtQuery,
        [&](wxWindow *)
        {
            REQUIRE(doomedAtQuery->Destroy());
        });
    {
        wxWinUITLWHostWindowEventGuard retainedCallback(
            queryRollbackRoot);
        CHECK_FALSE(queryRollbackRoot->Reparent(doomedAtQuery));
        CHECK(queryRollbackRoot->GetParent() == frame96);
        CHECK(::GetParent(GetHwndOf(queryRollbackRoot)) ==
              queryOriginalNativeParent);
        CHECK(queryRollbackEvents == 0);
        CHECK(wxWinUITopLevelHost::FindSlotOwner(queryRollbackChild) ==
              querySourceOwner);
    }
    dpi.RemoveTLW(doomedAtQuery);
    DrainDispatch();

    // The same validation is required after USER32 returns: a synchronous
    // callback can queue B for ordinary wxPendingDelete while its weak
    // reference, HWND and native parent relation are all still present. The
    // A->B operation is then rolled back atomically and publishes no DPI.
    wxFrame * const doomedAfterBoundary =
        new wxFrame(nullptr, wxID_ANY, "doomed-after-setparent");
    ShowOffscreenWithoutActivating(doomedAfterBoundary, 5);
    new wxButton(doomedAfterBoundary, wxID_ANY, "prime boundary host");
    dpi.SetTLWDPI(doomedAfterBoundary, 144);
    wxPanel * const boundaryRollbackRoot = new wxPanel(frame96);
    wxButton * const boundaryRollbackChild =
        new wxButton(
            boundaryRollbackRoot, wxID_ANY, "boundary rollback child");
    wxWinUITopLevelHost * const boundarySourceOwner =
        wxWinUITopLevelHost::FindSlotOwner(boundaryRollbackChild);
    REQUIRE(boundarySourceOwner != nullptr);
    const HWND boundaryOriginalNativeParent =
        ::GetParent(GetHwndOf(boundaryRollbackRoot));
    unsigned boundaryRollbackEvents = 0;
    boundaryRollbackRoot->Bind(
        wxEVT_DPI_CHANGED,
        [&](wxDPIChangedEvent& event)
        {
            ++boundaryRollbackEvents;
            event.Skip();
        });
    dpi.AfterNextSetParent(
        [&](wxWindow *window)
        {
            REQUIRE(window == boundaryRollbackRoot);
            REQUIRE(doomedAfterBoundary->Destroy());
        });
    CHECK_FALSE(boundaryRollbackRoot->Reparent(doomedAfterBoundary));
    CHECK(boundaryRollbackRoot->GetParent() == frame96);
    CHECK(::GetParent(GetHwndOf(boundaryRollbackRoot)) ==
          boundaryOriginalNativeParent);
    CHECK(boundaryRollbackEvents == 0);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(boundaryRollbackChild) ==
          boundarySourceOwner);
    dpi.RemoveTLW(doomedAfterBoundary);
    DrainDispatch();

    // Host migration itself is an application-code boundary. A slotted child
    // created there did not cross the old native boundary and must not be
    // retroactively included in the DPI transition.
    wxPanel * const lateRoot = new wxPanel(frame96);
    wxButton * const anchor =
        new wxButton(lateRoot, wxID_ANY, "pre-existing");
    wxButton *lateChild = nullptr;
    unsigned lateChildEvents = 0;
    bool lateChildCreated = false;
    wxWinUITopLevelHost::TestOnNextSlotAttached(
        [&](wxWindow *window)
        {
            if ( window != anchor )
                return;

            lateChildCreated = true;
            lateChild =
                new wxButton(lateRoot, wxID_ANY, "created-in-migration");
            lateChild->Bind(
                wxEVT_DPI_CHANGED,
                [&](wxDPIChangedEvent& event)
                {
                    ++lateChildEvents;
                    event.Skip();
                });
        });
    REQUIRE(lateRoot->Reparent(frame144));
    wxWinUITopLevelHost::TestOnNextSlotAttached({});
    REQUIRE(lateChildCreated);
    REQUIRE(lateChild != nullptr);
    CHECK(lateChildEvents == 0);

    // Destroying a not-yet-visited sibling from the first callback must make
    // its weak identity disappear without invalidating the remaining walk.
    wxPanel * const destroyingRoot = new wxPanel(frame96);
    wxButton * const destroyingFirst =
        new wxButton(destroyingRoot, wxID_ANY, "destroy sibling");
    wxPanel * const destroyedSibling = new wxPanel(destroyingRoot);
    const wxWeakRef<wxWindow> destroyedSiblingIdentity(destroyedSibling);
    unsigned destroyingFirstEvents = 0;
    unsigned destroyedSiblingEvents = 0;
    unsigned destroyingRootEvents = 0;
    destroyingFirst->Bind(
        wxEVT_DPI_CHANGED,
        [&](wxDPIChangedEvent& event)
        {
            ++destroyingFirstEvents;
            destroyedSibling->Destroy();
            event.Skip();
        });
    destroyedSibling->Bind(
        wxEVT_DPI_CHANGED,
        [&](wxDPIChangedEvent& event)
        {
            ++destroyedSiblingEvents;
            event.Skip();
        });
    destroyingRoot->Bind(
        wxEVT_DPI_CHANGED,
        [&](wxDPIChangedEvent& event)
        {
            ++destroyingRootEvents;
            event.Skip();
        });
    REQUIRE(destroyingRoot->Reparent(frame144));
    CHECK(destroyingFirstEvents == 1);
    CHECK(destroyedSiblingEvents == 0);
    CHECK(destroyingRootEvents == 1);
    wxWindow * const pendingDestroyedSibling =
        destroyedSiblingIdentity.get();
    CHECK((!pendingDestroyedSibling ||
           pendingDestroyedSibling->IsBeingDeleted()));

    // A native SetParent failure must restore the wx child list and leave the
    // source host authoritative; no synthetic DPI transition is published.
    wxPanel * const failingRoot = new wxPanel(frame96);
    wxButton * const failingChild =
        new wxButton(failingRoot, wxID_ANY, "setparent failure");
    wxWinUITopLevelHost * const failingOwner =
        wxWinUITopLevelHost::FindSlotOwner(failingChild);
    REQUIRE(failingOwner != nullptr);
    const HWND failingHwnd = GetHwndOf(failingRoot);
    const HWND failingNativeParent = ::GetParent(failingHwnd);
    unsigned failingEvents = 0;
    unsigned failingSetFocusEvents = 0;
    unsigned failingKillFocusEvents = 0;
    failingRoot->Bind(
        wxEVT_DPI_CHANGED,
        [&](wxDPIChangedEvent& event)
        {
            ++failingEvents;
            event.Skip();
        });
    frame96->Raise();
    failingChild->SetFocus();
    DrainToQuiescence();
    REQUIRE(failingOwner->GetFocusOwner() == failingChild);
    REQUIRE(wxWindow::FindFocus() == failingChild);
    const unsigned pendingFocusMigrations =
        wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest();
    failingChild->Bind(
        wxEVT_SET_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++failingSetFocusEvents;
            event.Skip();
        });
    failingChild->Bind(
        wxEVT_KILL_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++failingKillFocusEvents;
            event.Skip();
        });
    wxWinUIMSWFailNextSetParentForTest();
    CHECK_FALSE(failingRoot->Reparent(frame144));
    CHECK(failingRoot->GetParent() == frame96);
    CHECK(::GetParent(failingHwnd) == failingNativeParent);
    CHECK(failingEvents == 0);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(failingChild) ==
          failingOwner);
    CHECK(failingOwner->FindSlot(failingChild) != nullptr);
    CHECK(failingOwner->GetFocusOwner() == failingChild);
    CHECK(wxWindow::FindFocus() == failingChild);
    CHECK(failingSetFocusEvents == 0);
    CHECK(failingKillFocusEvents == 0);
    CHECK(wxWinUITopLevelHost::
              GetPendingFocusMigrationCountForTest() ==
          pendingFocusMigrations);

    frame96->Destroy();
    frame144->Destroy();
    frame144Peer->Destroy();
    frame192->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::SameTLWReparentTracksNewAncestors", "[HostLifecycle]")
{
    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "reparent-same");
    wxPanel * const left = new wxPanel(frame, wxID_ANY,
                                      wxPoint(10, 10), wxSize(100, 100));
    wxPanel * const right = new wxPanel(frame, wxID_ANY,
                                       wxPoint(150, 10), wxSize(100, 100));
    wxButton * const button = new wxButton(left, wxID_ANY, "tracked",
                                           wxPoint(5, 5));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    host->FocusSlot(button);
    DrainDispatch(3);
    REQUIRE(host->GetFocusOwner() == button);
    REQUIRE(wxWindow::FindFocus() == button);

    REQUIRE(button->Reparent(right));
    // Rebinding the ancestor geometry chain must not clear the logical owner:
    // the XAML element never lost focus, so no GotFocus will arrive to repair
    // an owner erased during same-TLW reparenting.
    CHECK(host->GetFocusOwner() == button);
    CHECK(wxWindow::FindFocus() == button);
    DrainToQuiescence();
    wxWinUISlot * const slot = host->FindSlot(button);
    REQUIRE(slot != nullptr);
    const int x0 = slot->GetRectInTLW().x;

    right->Move(right->GetPosition() + wxPoint(30, 0));
    DrainToQuiescence();
    CHECK(slot->GetRectInTLW().x == x0 + 30);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::EffectiveDisableSimpleControl", "[HostState]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
    using MUX::Automation::Peers::FrameworkElementAutomationPeer;

    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "state-simple");
    wxPanel * const panel = new wxPanel(frame);
    wxButton * const button = new wxButton(panel, wxID_ANY, "hosted");
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(button);
    REQUIRE(slot != nullptr);
    const auto control = slot->GetContent().try_as<MUXC::Control>();
    REQUIRE(control != nullptr);
    REQUIRE(control.IsEnabled());

    // Disabling the PARENT sends no message to the child: only the host's
    // effective-state sync can reflect it on the XAML side.
    panel->Disable();
    DrainDispatch(5);

    CHECK(!button->IsEnabled());
    CHECK(!control.IsEnabled());
    // keyboard: a disabled control refuses the focus
    CHECK(!control.Focus(MUX::FocusState::Programmatic));
    // UIA: the automation peer reports the disabled state
    const auto peer =
        FrameworkElementAutomationPeer::CreatePeerForElement(control);
    REQUIRE(peer != nullptr);
    CHECK(!peer.IsEnabled());
    // pointer: the whole slot stops hit-testing, the input falls through to
    // the (equally disabled) native windows below
    CHECK(!slot->GetContainer().IsHitTestVisible());

    panel->Enable();
    DrainDispatch(5);
    CHECK(control.IsEnabled());
    CHECK(slot->GetContainer().IsHitTestVisible());

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::EffectiveDisableComposite", "[HostState]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
    using MUX::Automation::Peers::FrameworkElementAutomationPeer;

    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "state-composite");
    wxPanel * const panel = new wxPanel(frame);
    const wxString choices[] = { "alpha", "beta", "gamma" };
    wxRadioBox * const radio =
        new wxRadioBox(panel, wxID_ANY, "group", wxDefaultPosition,
                       wxDefaultSize, WXSIZEOF(choices), choices);
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(radio);
    REQUIRE(slot != nullptr);

    // The whole point of this case: the content root is a composite Panel,
    // NOT a Control -- Control::IsEnabled cannot be set on it directly.
    REQUIRE(slot->GetContent().try_as<MUXC::Control>() == nullptr);
    const auto buttons = CollectRadioButtons(slot->GetContent());
    REQUIRE(buttons.size() == WXSIZEOF(choices));

    // one item individually disabled by the application
    REQUIRE(radio->Enable(1, false));
    DrainDispatch(3);
    CHECK(!buttons[1].IsEnabled());

    panel->Disable();
    DrainDispatch(5);

    // every descendant must be disabled for pointer, keyboard and UIA
    for ( size_t i = 0; i < buttons.size(); ++i )
    {
        INFO("radio button #" << i);
        CHECK(!buttons[i].IsEnabled());
        CHECK(!buttons[i].Focus(MUX::FocusState::Programmatic));
        const auto peer =
            FrameworkElementAutomationPeer::CreatePeerForElement(buttons[i]);
        REQUIRE(peer != nullptr);
        CHECK(!peer.IsEnabled());
    }
    CHECK(!slot->GetContainer().IsHitTestVisible());

    // re-enabling the parent must NOT resurrect the individually disabled
    // item: the host state must never overwrite the content's own state
    panel->Enable();
    DrainDispatch(5);
    CHECK(buttons[0].IsEnabled());
    CHECK(!buttons[1].IsEnabled());
    CHECK(buttons[2].IsEnabled());

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::PreferredRadioBoxFocus", "[HostState]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;

    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "state-focus");
    const wxString choices[] = { "alpha", "beta", "gamma" };
    wxRadioBox * const radio =
        new wxRadioBox(frame, wxID_ANY, "group", wxDefaultPosition,
                       wxDefaultSize, WXSIZEOF(choices), choices);
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(radio);
    REQUIRE(slot != nullptr);
    const auto buttons = CollectRadioButtons(slot->GetContent());
    REQUIRE(buttons.size() == WXSIZEOF(choices));

    // name the items so the focused element can be identified reliably
    for ( size_t i = 0; i < buttons.size(); ++i )
        buttons[i].Name(winrt::hstring(L"wxtest-rb" + std::to_wstring(i)));

    radio->SetSelection(2);
    DrainDispatch(2);

    host->FocusSlot(radio);
    DrainDispatch(3);

    // wxMSW contract: wxRadioBox::SetFocus() focuses the SELECTED item, and
    // only falls back to the first one without a selection
    const auto focused =
        MUX::Input::FocusManager::GetFocusedElement(host->GetXamlRoot());
    const auto focusedFE = focused.try_as<MUX::FrameworkElement>();
    REQUIRE(focusedFE != nullptr);
    CHECK(wxString(focusedFE.Name().c_str()) == "wxtest-rb2");

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::RadioBox item contracts are projected to XAML",
          "[HostState][radiobox][tooltip][help][hit-test]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
    namespace WF = winrt::Windows::Foundation;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "radiobox-item-contracts");
    const wxString choices[] = { "alpha", "beta", "gamma" };
    wxRadioBox * const radio =
        new wxRadioBox(frame, wxID_ANY, "group", wxDefaultPosition,
                       wxDefaultSize, WXSIZEOF(choices), choices);
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(radio);
    REQUIRE(host != nullptr);
    wxWinUISlot *slot = host->FindSlot(radio);
    REQUIRE(slot != nullptr);
    auto buttons = CollectRadioButtons(slot->GetContent());
    REQUIRE(buttons.size() == WXSIZEOF(choices));

    radio->SetSelection(1);
    radio->SetFocus();
    REQUIRE(DrainUntil(
        [&]()
        {
            const auto focused =
                MUX::Input::FocusManager::GetFocusedElement(
                    host->GetXamlRoot());
            return IsInVisualSubtree(focused, buttons[1]);
        }));

    const auto focused =
        MUX::Input::FocusManager::GetFocusedElement(host->GetXamlRoot());
    REQUIRE(focused != nullptr);
    CHECK_FALSE(IsInVisualSubtree(focused, buttons[0]));
    CHECK(IsInVisualSubtree(focused, buttons[1]));
    CHECK_FALSE(IsInVisualSubtree(focused, buttons[2]));

#if wxUSE_TOOLTIPS
    radio->SetItemToolTip(1, "beta item");
    DrainToQuiescence();
    slot = host->FindSlot(radio);
    REQUIRE(slot != nullptr);
    buttons = CollectRadioButtons(slot->GetContent());
    REQUIRE(buttons.size() == WXSIZEOF(choices));
    CHECK(MUXC::ToolTipService::GetToolTip(buttons[0]) == nullptr);
    CHECK(GetToolTipInspectableText(
              MUXC::ToolTipService::GetToolTip(buttons[1])) ==
          "beta item");
    CHECK(radio->HasToolTips());

    const MUXC::RadioButton previousBeta = buttons[1];
    radio->SetItemToolTip(1, "updated beta item");
    DrainToQuiescence();
    CHECK(MUXC::ToolTipService::GetToolTip(previousBeta) == nullptr);
    slot = host->FindSlot(radio);
    REQUIRE(slot != nullptr);
    buttons = CollectRadioButtons(slot->GetContent());
    REQUIRE(buttons.size() == WXSIZEOF(choices));
    CHECK(GetToolTipInspectableText(
              MUXC::ToolTipService::GetToolTip(buttons[1])) ==
          "updated beta item");
#endif // wxUSE_TOOLTIPS

    const float width = static_cast<float>(buttons[1].ActualWidth());
    const float height = static_cast<float>(buttons[1].ActualHeight());
    REQUIRE(width > 0.0f);
    REQUIRE(height > 0.0f);
    WF::Rect itemBounds{};
    REQUIRE(wxWinUIVisualCoordinates::ElementBoundsToClient(
                radio,
                buttons[1],
                WF::Rect{ 0.0f, 0.0f, width, height },
                &itemBounds) == wxWinUICoordinateResult::Mapped);
    const wxPoint itemCentre(
        static_cast<int>(std::lround(
            itemBounds.X + itemBounds.Width / 2.0f)),
        static_cast<int>(std::lround(
            itemBounds.Y + itemBounds.Height / 2.0f)));
    CHECK(radio->GetItemFromPoint(itemCentre) == 1);
    CHECK(radio->GetItemFromPoint(wxPoint(-20, -20)) == wxNOT_FOUND);

#if wxUSE_HELP
    radio->SetItemHelpText(1, "beta context help");
    CHECK(radio->GetHelpTextAtPoint(
              itemCentre, wxHelpEvent::Origin_HelpButton) ==
          "beta context help");
#endif // wxUSE_HELP

#if wxUSE_TOOLTIPS
    radio->SetItemToolTip(1, wxString());
    DrainToQuiescence();
    slot = host->FindSlot(radio);
    REQUIRE(slot != nullptr);
    buttons = CollectRadioButtons(slot->GetContent());
    REQUIRE(buttons.size() == WXSIZEOF(choices));
    CHECK(MUXC::ToolTipService::GetToolTip(buttons[1]) == nullptr);
#endif // wxUSE_TOOLTIPS

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::RadioBoxNoFocusableItems", "[HostState]")
{
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "state-radio-focusability");
    wxButton * const holder = new wxButton(frame, wxID_ANY, "holder");
    const wxString choices[] = { "alpha", "beta", "gamma" };
    wxRadioBox * const radio =
        new wxRadioBox(frame, wxID_ANY, "group", wxDefaultPosition,
                       wxDefaultSize, WXSIZEOF(choices), choices);
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(radio);
    REQUIRE(host != nullptr);
    host->FocusSlot(holder);
    DrainDispatch(3);
    REQUIRE(host->GetFocusOwner() == holder);

    for ( unsigned i = 0; i < WXSIZEOF(choices); ++i )
        REQUIRE(radio->Enable(i, false));
    CHECK(!radio->CanAcceptFocus());
    host->FocusSlot(radio);
    DrainDispatch(2);
    CHECK(host->GetFocusOwner() == holder);

    REQUIRE(radio->Enable(1, true));
    CHECK(radio->CanAcceptFocus());
    host->FocusSlot(radio);
    DrainDispatch(3);
    CHECK(host->GetFocusOwner() == radio);

    REQUIRE(radio->Enable(1, false));
    for ( unsigned i = 0; i < WXSIZEOF(choices); ++i )
        REQUIRE(radio->Show(i, false));
    CHECK(!radio->CanAcceptFocus());

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::IntrinsicContentStatePreserved", "[HostState]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    // Application content that is INTRINSICALLY disabled: the host state
    // sync must never overwrite it -- neither on the initial sync nor when
    // the wx window toggles enabled.
    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "state-intrinsic");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"off\" IsEnabled=\"False\"/>"));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto control = slot->GetContent().try_as<MUXC::Control>();
    REQUIRE(control != nullptr);

    // the initial state sync must not have force-enabled it
    CHECK(!control.IsEnabled());

    // a wx disable/re-enable round trip must not resurrect it either
    xamlHost->Disable();
    DrainDispatch(3);
    CHECK(!control.IsEnabled());
    xamlHost->Enable();
    DrainDispatch(3);
    CHECK(!control.IsEnabled());

    frame->Destroy();
    DrainDispatch();
}

#if wxUSE_TOOLTIPS
TEST_CASE("HostState::TooltipState", "[HostState]")
{
    namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "state-tooltip");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);

    // A tooltip set with NO geometry churn afterwards must still reach the
    // peer: the mutation itself has to reach the dirty seam.  (The XAML
    // host has no per-control implementation pushing it, unlike wxButton:
    // only the common slot seam can do it.)
    xamlHost->SetToolTip("tip one");
    DrainDispatch(3);
    CHECK(GetPeerToolTipText(xamlHost) == "tip one");
    CHECK(MUXA::AutomationProperties::GetHelpText(
              slot->GetContent()) == L"tip one");

    xamlHost->UnsetToolTip();
    DrainDispatch(3);
    CHECK(MUXC::ToolTipService::GetToolTip(slot->GetContent()) == nullptr);
    CHECK(MUXA::AutomationProperties::GetHelpText(
              slot->GetContent()).empty());

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::GlobalTooltipPolicy",
          "[HostState][tooltip][policy]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    struct PolicyReset
    {
        ~PolicyReset()
        {
            wxToolTip::Enable(true);
            wxToolTip::SetMaxWidth(0);
        }
    } reset;

    wxToolTip::Enable(true);
    wxToolTip::SetMaxWidth(0);

    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    // Exercise the native shared TTM host before constructing the XAML
    // tooltip. AdjustMaxWidth() may compute an effective native limit, but it
    // must not mutate the public 0 sentinel captured by future WinUI objects.
    wxFrame nativeOwner(
        nullptr, wxID_ANY, "native-tooltip-policy-owner",
        wxDefaultPosition, wxSize(120, 80),
        wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxToolTip nativeTip(
        "a deliberately long native tooltip line used to adjust width");
    nativeTip.SetWindow(&nativeOwner);
    CHECK(wxWinUIGetToolTipMaxWidth() == 0);

    wxButton button(parent, wxID_ANY, "Policy");
    button.SetToolTip("managed-tip");

    // Creation is the logical wxToolTip construction above, not the later
    // coalesced materialization of its XAML peer.
    wxToolTip::SetMaxWidth(137);
    DrainDispatch(3);
    CHECK(GetPeerToolTipText(&button) == "managed-tip");

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(&button);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(&button);
    REQUIRE(slot != nullptr);
    const auto object =
        MUXC::ToolTipService::GetToolTip(slot->GetContent())
            .try_as<MUXC::ToolTip>();
    REQUIRE(object != nullptr);
    const auto maxWidthProperty =
        winrt::Microsoft::UI::Xaml::FrameworkElement::MaxWidthProperty();
    CHECK(object.ReadLocalValue(maxWidthProperty) ==
          winrt::Microsoft::UI::Xaml::DependencyProperty::UnsetValue());

    // The public contract says SetMaxWidth() affects only subsequently-created
    // logical tooltips. The peer materialized after the call above must retain
    // the creation-time default, and its unit is a physical pixel rather than
    // a XAML DIP.
    CHECK(object.ReadLocalValue(maxWidthProperty) ==
          winrt::Microsoft::UI::Xaml::DependencyProperty::UnsetValue());
    CHECK(wxWinUIGetToolTipMaxWidth() == 137);

    // Replacing the text creates a new XAML ToolTip object, but it is still
    // the same logical wxToolTip and must keep the old policy snapshot.
    button.GetToolTip()->SetTip("managed-tip-updated");
    host->FlushSync();
    const auto updatedObject =
        MUXC::ToolTipService::GetToolTip(slot->GetContent())
            .try_as<MUXC::ToolTip>();
    REQUIRE(updatedObject != nullptr);
    CHECK(updatedObject != object);
    CHECK(updatedObject.ReadLocalValue(maxWidthProperty) ==
          winrt::Microsoft::UI::Xaml::DependencyProperty::UnsetValue());

    wxButton newerButton(parent, wxID_ANY, "New policy");
    newerButton.SetToolTip("new-managed-tip");
    DrainDispatch(3);
    wxWinUITopLevelHost * const newerHost =
        wxWinUITopLevelHost::FindSlotOwner(&newerButton);
    REQUIRE(newerHost != nullptr);
    wxWinUISlot * const newerSlot = newerHost->FindSlot(&newerButton);
    REQUIRE(newerSlot != nullptr);
    const auto newerObject =
        MUXC::ToolTipService::GetToolTip(newerSlot->GetContent())
            .try_as<MUXC::ToolTip>();
    REQUIRE(newerObject != nullptr);
    const int dpi = newerButton.GetDPI().GetWidth();
    REQUIRE(dpi > 0);
    const double expectedDIP = 137.0 * 96.0 / dpi;
    CHECK(std::abs(newerObject.MaxWidth() - expectedDIP) < 0.01);

    wxToolTip::SetMaxWidth(241);
    newerButton.GetToolTip()->SetTip("new-managed-tip-updated");
    newerHost->FlushSync();
    const auto newerUpdatedObject =
        MUXC::ToolTipService::GetToolTip(newerSlot->GetContent())
            .try_as<MUXC::ToolTip>();
    REQUIRE(newerUpdatedObject != nullptr);
    CHECK(newerUpdatedObject != newerObject);
    CHECK(std::abs(newerUpdatedObject.MaxWidth() - expectedDIP) < 0.01);

    wxToolTip::Enable(false);
    bool peerPresent = true;
    GetPeerToolTipText(&button, &peerPresent);
    CHECK_FALSE(peerPresent);
    GetPeerToolTipText(&newerButton, &peerPresent);
    CHECK_FALSE(peerPresent);
    CHECK(button.GetToolTipText() == "managed-tip-updated");
    CHECK_FALSE(wxWinUIAreToolTipsEnabled());

    // A logical SetTip() while the global policy owns the attached null must
    // replace the restored old object when policy is enabled again. Caching
    // the new text without rebuilding here would lose it permanently.
    button.GetToolTip()->SetTip("managed-tip-while-disabled");
    host->FlushSync();
    GetPeerToolTipText(&button, &peerPresent);
    CHECK_FALSE(peerPresent);

    wxToolTip::Enable(true);
    CHECK(GetPeerToolTipText(&button) ==
          "managed-tip-while-disabled");
    CHECK(GetPeerToolTipText(&newerButton) == "new-managed-tip-updated");
    CHECK(wxWinUIAreToolTipsEnabled());
}

TEST_CASE("HostState::TooltipPolicySpansIndependentXamlRoots",
          "[HostState][tooltip][policy][winui-008f]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    struct PolicyReset
    {
        ~PolicyReset()
        {
            wxToolTip::Enable(true);
            wxToolTip::SetMaxWidth(0);
        }
    } reset;

    wxToolTip::Enable(true);
    wxToolTip::SetMaxWidth(96);

    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "tooltip-root-A",
                    wxDefaultPosition, wxSize(260, 140),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "tooltip-root-B",
                    wxDefaultPosition, wxSize(260, 140),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxButton * const buttonA =
        new wxButton(frameA, wxID_ANY, "first root");
    wxButton * const buttonB =
        new wxButton(frameB, wxID_ANY, "second root");
    buttonA->SetToolTip("first line\nsecond line");
    buttonB->SetToolTip("second-root");

    // Both logical objects have now captured the width policy. Resetting the
    // global value before peer realization proves that each XAML root uses
    // the creation-time snapshot instead of the later global value.
    wxToolTip::SetMaxWidth(0);
    ShowOffscreenWithoutActivating(frameA);
    ShowOffscreenWithoutActivating(frameB, 1);
    DrainToQuiescence();

    wxWinUITopLevelHost * const hostA =
        wxWinUITopLevelHost::FindSlotOwner(buttonA);
    wxWinUITopLevelHost * const hostB =
        wxWinUITopLevelHost::FindSlotOwner(buttonB);
    REQUIRE(hostA != nullptr);
    REQUIRE(hostB != nullptr);
    CHECK(hostA != hostB);
    wxWinUISlot * const slotA = hostA->FindSlot(buttonA);
    wxWinUISlot * const slotB = hostB->FindSlot(buttonB);
    REQUIRE(slotA != nullptr);
    REQUIRE(slotB != nullptr);
    const auto contentA = slotA->GetContent();
    const auto contentB = slotB->GetContent();
    REQUIRE(contentA != nullptr);
    REQUIRE(contentB != nullptr);

    auto objectA =
        MUXC::ToolTipService::GetToolTip(contentA).try_as<MUXC::ToolTip>();
    const auto objectB =
        MUXC::ToolTipService::GetToolTip(contentB).try_as<MUXC::ToolTip>();
    REQUIRE(objectA != nullptr);
    REQUIRE(objectB != nullptr);
    CHECK(GetToolTipInspectableText(objectA) ==
          "first line\nsecond line");
    CHECK(GetToolTipInspectableText(objectB) == "second-root");

    const int dpiA = buttonA->GetDPI().GetWidth();
    const int dpiB = buttonB->GetDPI().GetWidth();
    REQUIRE(dpiA > 0);
    REQUIRE(dpiB > 0);
    CHECK(std::abs(objectA.MaxWidth() - 96.0 * 96.0 / dpiA) < 0.01);
    CHECK(std::abs(objectB.MaxWidth() - 96.0 * 96.0 / dpiB) < 0.01);

    // Preserve the physical-pixel contract across a monitor-DPI resync even
    // when text and creation-time width are unchanged.
    slotA->SetLastToolTipDPIForTest(dpiA + 1);
    wxWinUITLWHostNotifySlotState(buttonA);
    hostA->FlushSync();
    const auto dpiResyncedObject =
        MUXC::ToolTipService::GetToolTip(contentA)
            .try_as<MUXC::ToolTip>();
    REQUIRE(dpiResyncedObject != nullptr);
    CHECK(dpiResyncedObject != objectA);
    CHECK(std::abs(
              dpiResyncedObject.MaxWidth() -
              96.0 * 96.0 / dpiA) < 0.01);
    objectA = dpiResyncedObject;

    wxToolTip::Enable(false);
    CHECK(MUXC::ToolTipService::GetToolTip(contentA) == nullptr);
    CHECK(MUXC::ToolTipService::GetToolTip(contentB) == nullptr);

    wxToolTip::Enable(true);
    CHECK(MUXC::ToolTipService::GetToolTip(contentA) == objectA);
    CHECK(MUXC::ToolTipService::GetToolTip(contentB) == objectB);

    frameA->Destroy();
    frameB->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::TooltipClosesForHiddenOrDisabledSlot",
          "[HostState][tooltip][winui-008f]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    struct PolicyReset
    {
        ~PolicyReset() { wxToolTip::Enable(true); }
    } reset;
    wxToolTip::Enable(true);

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "tooltip-effective-state",
                    wxDefaultPosition, wxSize(260, 140),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxButton * const button =
        new wxButton(frame, wxID_ANY, "target");
    button->SetToolTip("effective-state");
    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(button);
    REQUIRE(slot != nullptr);
    const auto content = slot->GetContent();
    REQUIRE(content != nullptr);
    const auto object =
        MUXC::ToolTipService::GetToolTip(content).try_as<MUXC::ToolTip>();
    REQUIRE(object != nullptr);
    object.PlacementTarget(content);

    object.IsOpen(true);
    REQUIRE(object.IsOpen());
    button->Disable();
    host->FlushSync();
    CHECK_FALSE(object.IsOpen());
    CHECK(MUXC::ToolTipService::GetToolTip(content) == object);

    button->Enable();
    host->FlushSync();
    object.IsOpen(true);
    REQUIRE(object.IsOpen());
    button->Hide();
    host->FlushSync();
    CHECK_FALSE(object.IsOpen());
    CHECK(MUXC::ToolTipService::GetToolTip(content) == object);

    button->Show();
    host->FlushSync();
    object.IsOpen(true);
    REQUIRE(object.IsOpen());
    frame->Show(false);
    DrainToQuiescence();
    CHECK_FALSE(object.IsOpen());
    CHECK(MUXC::ToolTipService::GetToolTip(content) == object);

    frame->ShowWithoutActivating();
    DrainToQuiescence();
    object.IsOpen(true);
    REQUIRE(object.IsOpen());
    frame->Destroy();
    DrainDispatch();
    CHECK_FALSE(object.IsOpen());
}

TEST_CASE("HostState::TooltipDeferredReplacementIsNeverClobbered",
          "[HostState][tooltip][policy][reentrant][winui-008f]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    struct PolicyReset
    {
        ~PolicyReset() { wxToolTip::Enable(true); }
    } reset;
    wxToolTip::Enable(true);

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "tooltip-closed-replacement",
                    wxPoint(-30000, -30000), wxSize(260, 140),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxButton * const button =
        new wxButton(frame, wxID_ANY, "target");
    frame->ShowWithoutActivating();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(button);
    REQUIRE(slot != nullptr);
    const auto content = slot->GetContent();
    REQUIRE(content != nullptr);
    MUXC::ToolTip authored;
    authored.Content(winrt::box_value(
        winrt::hstring(L"authored-before-close")));
    MUXC::ToolTipService::SetToolTip(content, authored);
    host->FlushSync();
    REQUIRE(MUXC::ToolTipService::GetToolTip(content) == authored);

    MUXC::ToolTip replacement;
    replacement.Content(winrt::box_value(
        winrt::hstring(L"authored-from-deferred-callback")));

    authored.PlacementTarget(content);
    authored.IsOpen(true);
    REQUIRE(authored.IsOpen());

    wxToolTip::Enable(false);
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    struct DeferredWrite
    {
        bool completed = false;
        bool suppressedInline = false;
    };
    const auto deferredWrite = std::make_shared<DeferredWrite>();
    wxTheApp->CallAfter(
        [deferredWrite, content, replacement]()
        {
            MUXC::ToolTipService::SetToolTip(content, replacement);
            deferredWrite->suppressedInline =
                MUXC::ToolTipService::GetToolTip(content) == nullptr;
            deferredWrite->completed = true;
        });
    REQUIRE(DrainUntil(
        [deferredWrite]() { return deferredWrite->completed; }));
    REQUIRE(deferredWrite->suppressedInline);
    // The late replacement was detached inside its own ToolTipProperty
    // setter; no later coalesced host pass is needed to finish suppression.
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);

    wxToolTip::Enable(true);
    CHECK(MUXC::ToolTipService::GetToolTip(content) == replacement);

    MUXC::ToolTip detachReplacement;
    detachReplacement.Content(winrt::box_value(
        winrt::hstring(L"authored-from-detach")));
    bool detachReplaced = false;
    const int64_t detachToken =
        content.RegisterPropertyChangedCallback(
            MUXC::ToolTipService::ToolTipProperty(),
            [&](MUX::DependencyObject const&,
                MUX::DependencyProperty const&)
            {
                if ( detachReplaced ||
                     MUXC::ToolTipService::GetToolTip(content) )
                {
                    return;
                }
                detachReplaced = true;
                MUXC::ToolTipService::SetToolTip(
                    content, detachReplacement);
            });

    wxToolTip::Enable(false);
    content.UnregisterPropertyChangedCallback(
        MUXC::ToolTipService::ToolTipProperty(), detachToken);
    REQUIRE(detachReplaced);
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);

    wxToolTip::Enable(true);
    CHECK(MUXC::ToolTipService::GetToolTip(content) ==
          detachReplacement);

    // A hostile callback can keep replacing every null. The local transaction
    // has a strict budget: it leaves the final replacement attached (and does
    // not falsely publish "suppressed"), then a later real pass can adopt that
    // exact object and recover once the callback is gone.
    unsigned budgetReplacements = 0;
    MUXC::ToolTip lastBudgetReplacement{ nullptr };
    const int64_t budgetToken =
        content.RegisterPropertyChangedCallback(
            MUXC::ToolTipService::ToolTipProperty(),
            [&](MUX::DependencyObject const&,
                MUX::DependencyProperty const&)
            {
                if ( MUXC::ToolTipService::GetToolTip(content) )
                    return;

                ++budgetReplacements;
                MUXC::ToolTip candidate;
                candidate.Content(winrt::box_value(
                    winrt::hstring(L"bounded-detach-replacement")));
                lastBudgetReplacement = candidate;
                MUXC::ToolTipService::SetToolTip(content, candidate);
            });

    wxToolTip::Enable(false);
    content.UnregisterPropertyChangedCallback(
        MUXC::ToolTipService::ToolTipProperty(), budgetToken);
    CHECK(budgetReplacements == 8);
    REQUIRE(lastBudgetReplacement != nullptr);
    CHECK(MUXC::ToolTipService::GetToolTip(content) ==
          lastBudgetReplacement);

    host->FlushSync();
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    wxToolTip::Enable(true);
    CHECK(MUXC::ToolTipService::GetToolTip(content) ==
          lastBudgetReplacement);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::ReentrantGlobalTooltipPolicy",
          "[HostState][tooltip][policy][reentrant]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    struct PolicyReset
    {
        ~PolicyReset() { wxToolTip::Enable(true); }
    } reset;
    wxToolTip::Enable(true);

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "tooltip-policy-reentrant",
                    wxPoint(-30000, -30000), wxSize(260, 140),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"><ToolTipService.ToolTip>"
        "<ToolTip Content=\"authored-baseline\"/>"
        "</ToolTipService.ToolTip></Border>"));
    frame->ShowWithoutActivating();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto content = slot->GetContent();
    REQUIRE(content != nullptr);
    const auto baseline = MUXC::ToolTipService::GetToolTip(content);
    REQUIRE(baseline != nullptr);
    const unsigned managedBefore =
        wxWinUIGetManagedToolTipCountForTesting();

    xamlHost->SetToolTip("managed-reentrant");
    host->FlushSync();
    const auto managed =
        MUXC::ToolTipService::GetToolTip(content).try_as<MUXC::ToolTip>();
    REQUIRE(managed != nullptr);
    CHECK(wxWinUIGetManagedToolTipCountForTesting() ==
          managedBefore + 1);

    // First interrupt suppression at its earliest side effect. IsOpen(false)
    // can synchronously invoke application code while the exact object is
    // still attached and enabled.
    managed.PlacementTarget(content);
    managed.IsOpen(true);
    REQUIRE(managed.IsOpen());
    bool oppositeCloseFired = false;
    const int64_t oppositeCloseToken =
        managed.RegisterPropertyChangedCallback(
            MUXC::ToolTip::IsOpenProperty(),
            [&](MUX::DependencyObject const&, MUX::DependencyProperty const&)
            {
                if ( oppositeCloseFired || managed.IsOpen() )
                    return;
                oppositeCloseFired = true;
                wxToolTip::Enable(true);
            });
    wxToolTip::Enable(false);
    managed.UnregisterPropertyChangedCallback(
        MUXC::ToolTip::IsOpenProperty(), oppositeCloseToken);

    CHECK(oppositeCloseFired);
    CHECK(wxWinUIAreToolTipsEnabled());
    CHECK_FALSE(managed.IsOpen());
    CHECK(managed.IsEnabled());
    CHECK(MUXC::ToolTipService::GetToolTip(content) == managed);
    CHECK(wxWinUIGetManagedToolTipCountForTesting() ==
          managedBefore + 1);

    // A same-value nested request at the IsEnabled boundary is coalesced.
    // It may publish the native TTM state immediately, but it must not recurse
    // into another registry walk or lose the entry being suppressed.
    bool sameDisableFired = false;
    const int64_t sameDisableToken =
        managed.RegisterPropertyChangedCallback(
            MUXC::Control::IsEnabledProperty(),
            [&](MUX::DependencyObject const&, MUX::DependencyProperty const&)
            {
                if ( sameDisableFired || managed.IsEnabled() )
                    return;
                sameDisableFired = true;
                wxToolTip::Enable(false);
            });
    wxToolTip::Enable(false);
    managed.UnregisterPropertyChangedCallback(
        MUXC::Control::IsEnabledProperty(), sameDisableToken);

    CHECK(sameDisableFired);
    CHECK_FALSE(wxWinUIAreToolTipsEnabled());
    CHECK_FALSE(managed.IsEnabled());
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    CHECK(wxWinUIGetManagedToolTipCountForTesting() ==
          managedBefore + 1);

    wxToolTip::Enable(true);
    REQUIRE(MUXC::ToolTipService::GetToolTip(content) == managed);
    CHECK(managed.IsEnabled());

    // Flip the request at the attached-property detach boundary. At callback
    // entry null has already been published, while the entry is not yet in
    // stable Suppressed state. The partial phase must retain ownership and
    // let the nested last writer restore the exact object.
    bool oppositeDetachFired = false;
    const int64_t oppositeDetachToken =
        content.RegisterPropertyChangedCallback(
            MUXC::ToolTipService::ToolTipProperty(),
            [&](MUX::DependencyObject const&, MUX::DependencyProperty const&)
            {
                if ( oppositeDetachFired ||
                     MUXC::ToolTipService::GetToolTip(content) )
                {
                    return;
                }
                oppositeDetachFired = true;
                wxToolTip::Enable(true);
            });
    wxToolTip::Enable(false);
    content.UnregisterPropertyChangedCallback(
        MUXC::ToolTipService::ToolTipProperty(), oppositeDetachToken);

    CHECK(oppositeDetachFired);
    CHECK(wxWinUIAreToolTipsEnabled());
    CHECK(managed.IsEnabled());
    CHECK(MUXC::ToolTipService::GetToolTip(content) == managed);
    CHECK(wxWinUIGetManagedToolTipCountForTesting() ==
          managedBefore + 1);

    // Exercise the mirror partial state: restore attaches the candidate, and
    // its property callback immediately makes disable the newest request.
    wxToolTip::Enable(false);
    REQUIRE(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    bool oppositeRestoreFired = false;
    const int64_t oppositeRestoreToken =
        content.RegisterPropertyChangedCallback(
            MUXC::ToolTipService::ToolTipProperty(),
            [&](MUX::DependencyObject const&, MUX::DependencyProperty const&)
            {
                if ( oppositeRestoreFired ||
                     MUXC::ToolTipService::GetToolTip(content) != managed )
                {
                    return;
                }
                oppositeRestoreFired = true;
                wxToolTip::Enable(false);
            });
    wxToolTip::Enable(true);
    content.UnregisterPropertyChangedCallback(
        MUXC::ToolTipService::ToolTipProperty(), oppositeRestoreToken);

    CHECK(oppositeRestoreFired);
    CHECK_FALSE(wxWinUIAreToolTipsEnabled());
    CHECK_FALSE(managed.IsEnabled());
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    CHECK(wxWinUIGetManagedToolTipCountForTesting() ==
          managedBefore + 1);

    // Same-value reentrancy on the restore-side IsEnabled seam must also
    // converge without an unbounded recursive walk.
    bool sameEnableFired = false;
    const int64_t sameEnableToken =
        managed.RegisterPropertyChangedCallback(
            MUXC::Control::IsEnabledProperty(),
            [&](MUX::DependencyObject const&, MUX::DependencyProperty const&)
            {
                if ( sameEnableFired || !managed.IsEnabled() )
                    return;
                sameEnableFired = true;
                wxToolTip::Enable(true);
            });
    wxToolTip::Enable(true);
    managed.UnregisterPropertyChangedCallback(
        MUXC::Control::IsEnabledProperty(), sameEnableToken);

    CHECK(sameEnableFired);
    CHECK(wxWinUIAreToolTipsEnabled());
    CHECK(managed.IsEnabled());
    CHECK(MUXC::ToolTipService::GetToolTip(content) == managed);
    CHECK(wxWinUIGetManagedToolTipCountForTesting() ==
          managedBefore + 1);

    // The transaction never adopts or reconstructs the authored value:
    // releasing the wx override restores its original exact identity.
    xamlHost->UnsetToolTip();
    host->FlushSync();
    CHECK(MUXC::ToolTipService::GetToolTip(content) == baseline);
    CHECK(wxWinUIGetManagedToolTipCountForTesting() == managedBefore);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::TooltipPolicySetterFaultRollback",
          "[HostState][tooltip][policy][reentrant][fault]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    struct PolicyReset
    {
        ~PolicyReset()
        {
            wxWinUISetToolTipFaultForTesting(
                wxWinUIToolTipFault_None, 0);
            if ( wxWinUIIsToolTipPolicyShutdownForTesting() )
                wxWinUIInitializeToolTipPolicy();
            wxToolTip::Enable(true);
        }
    } reset;

    const auto runScenario =
        [&](unsigned fault, const wxString& replacementText)
        {
            wxToolTip::Enable(true);
            wxWinUISetToolTipFaultForTesting(
                wxWinUIToolTipFault_None, 0);

            wxFrame * const frame =
                new wxFrame(nullptr, wxID_ANY, "tooltip-setter-fault",
                            wxPoint(-30000, -30000), wxSize(260, 140),
                            wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
            wxWinUIXamlHost * const xamlHost =
                new wxWinUIXamlHost(frame);
            REQUIRE(xamlHost->SetContentFromXaml(
                "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/"
                "xaml/presentation\"><ToolTipService.ToolTip>"
                "<ToolTip Content=\"fault-baseline\"/>"
                "</ToolTipService.ToolTip></Border>"));
            frame->ShowWithoutActivating();
            DrainToQuiescence();

            wxWinUITopLevelHost * const host =
                wxWinUITopLevelHost::FindSlotOwner(xamlHost);
            REQUIRE(host != nullptr);
            wxWinUISlot * const slot = host->FindSlot(xamlHost);
            REQUIRE(slot != nullptr);
            const auto content = slot->GetContent();
            REQUIRE(content != nullptr);
            const auto baseline =
                MUXC::ToolTipService::GetToolTip(content);
            REQUIRE(baseline != nullptr);
            const unsigned managedBefore =
                wxWinUIGetManagedToolTipCountForTesting();

            xamlHost->SetToolTip("fault-original");
            host->FlushSync();
            const auto managed =
                MUXC::ToolTipService::GetToolTip(content)
                    .try_as<MUXC::ToolTip>();
            REQUIRE(managed != nullptr);

            // Enter after SetToolTip(nullptr) published null but before the
            // disable transaction could mark it stably suppressed. Flipping
            // the last writer to enabled leaves the old entry in the
            // SuppressingDetach/owns-null phase with `suppressed == false`;
            // the nested model update then exercises the helper setter's
            // before/after-commit fault seam against that exact state.
            bool callbackFired = false;
            const int64_t token =
                content.RegisterPropertyChangedCallback(
                    MUXC::ToolTipService::ToolTipProperty(),
                    [&](MUX::DependencyObject const&,
                        MUX::DependencyProperty const&)
                    {
                        if ( callbackFired ||
                             MUXC::ToolTipService::GetToolTip(content) )
                        {
                            return;
                        }

                        callbackFired = true;
                        wxToolTip::Enable(true);
                        xamlHost->GetToolTip()->SetTip(replacementText);
                        wxWinUISetToolTipFaultForTesting(fault, 1);
                        host->FlushSync();
                    });

            wxToolTip::Enable(false);
            content.UnregisterPropertyChangedCallback(
                MUXC::ToolTipService::ToolTipProperty(), token);

            CHECK(callbackFired);
            CHECK(wxWinUIAreToolTipsEnabled());
            CHECK(GetPeerToolTipText(xamlHost) == replacementText);
            CHECK(wxWinUIGetManagedToolTipCountForTesting() ==
                  managedBefore + 1);

            xamlHost->UnsetToolTip();
            host->FlushSync();
            CHECK(MUXC::ToolTipService::GetToolTip(content) == baseline);
            CHECK(wxWinUIGetManagedToolTipCountForTesting() ==
                  managedBefore);

            frame->Destroy();
            DrainDispatch();
        };

    SECTION("failure before attached-property commit")
    {
        runScenario(wxWinUIToolTipFault_SetBeforeCommit,
                    "fault-before-commit");
    }

    SECTION("failure reported after attached-property commit")
    {
        runScenario(wxWinUIToolTipFault_SetAfterCommit,
                    "fault-after-commit");
    }
}

TEST_CASE("HostState::TooltipPolicyPermanentFaultIsBounded",
          "[HostState][tooltip][policy][fault]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    struct PolicyReset
    {
        ~PolicyReset()
        {
            wxWinUISetToolTipFaultForTesting(
                wxWinUIToolTipFault_None, 0);
            if ( wxWinUIIsToolTipPolicyShutdownForTesting() )
                wxWinUIInitializeToolTipPolicy();
            wxToolTip::Enable(true);
            wxWinUIResetToolTipPolicyDiagnosticsForTesting();
        }
    } reset;

    wxToolTip::Enable(true);
    wxWinUISetToolTipFaultForTesting(wxWinUIToolTipFault_None, 0);

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "tooltip-policy-permanent-fault",
                    wxPoint(-30000, -30000), wxSize(260, 140),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    frame->ShowWithoutActivating();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto content = slot->GetContent();
    REQUIRE(content != nullptr);

    xamlHost->SetToolTip("permanent-policy-fault");
    host->FlushSync();
    REQUIRE(MUXC::ToolTipService::GetToolTip(content) != nullptr);
    const unsigned policyEntryCount =
        wxWinUIGetManagedToolTipCountForTesting();
    REQUIRE(policyEntryCount > 0);

    wxWinUIResetToolTipPolicyDiagnosticsForTesting();
    wxWinUISetToolTipFaultForTesting(
        wxWinUIToolTipFault_Policy,
        std::numeric_limits<unsigned>::max());
    wxToolTip::Enable(false);

    // Eight synchronous attempts yield once. The persistent per-entry budget
    // is then exhausted by the one deferred replay and the entry is
    // quarantined; no callback can perpetually reschedule itself.
    CHECK(wxWinUIGetToolTipPolicyFaultAttemptCountForTesting() ==
          8 * policyEntryCount);
    CHECK(wxWinUIGetToolTipPolicyReplayScheduleCountForTesting() == 1);
    CHECK(wxWinUIGetToolTipPolicyReplayExecutionCountForTesting() == 0);
    CHECK(wxWinUIIsToolTipPolicyReplayScheduledForTesting());

    DrainDispatch(3);
    CHECK(wxWinUIGetToolTipPolicyFaultAttemptCountForTesting() ==
          12 * policyEntryCount);
    CHECK(wxWinUIGetToolTipPolicyReplayScheduleCountForTesting() == 1);
    CHECK(wxWinUIGetToolTipPolicyReplayExecutionCountForTesting() == 1);
    CHECK(wxWinUIGetToolTipPolicyQuarantinedEntryCountForTesting() ==
          policyEntryCount);
    CHECK_FALSE(wxWinUIIsToolTipPolicyReplayScheduledForTesting());

    const unsigned executions =
        wxWinUIGetToolTipPolicyReplayExecutionCountForTesting();
    DrainDispatch(10);
    CHECK(wxWinUIGetToolTipPolicyReplayExecutionCountForTesting() ==
          executions);
    CHECK_FALSE(wxWinUIIsToolTipPolicyReplayScheduledForTesting());

    wxWinUISetToolTipFaultForTesting(wxWinUIToolTipFault_None, 0);
    wxToolTip::Enable(true);
    CHECK(GetPeerToolTipText(xamlHost) == "permanent-policy-fault");

    xamlHost->UnsetToolTip();
    host->FlushSync();
    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::TooltipPolicyShutdownCancelsReplay",
          "[HostState][tooltip][policy][fault][shutdown]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    struct PolicyReset
    {
        ~PolicyReset()
        {
            wxWinUISetToolTipFaultForTesting(
                wxWinUIToolTipFault_None, 0);
            if ( wxWinUIIsToolTipPolicyShutdownForTesting() )
                wxWinUIInitializeToolTipPolicy();
            wxToolTip::Enable(true);
            wxWinUIResetToolTipPolicyDiagnosticsForTesting();
        }
    } reset;

    wxToolTip::Enable(true);
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "tooltip-policy-shutdown",
                    wxPoint(-30000, -30000), wxSize(260, 140),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    frame->ShowWithoutActivating();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto content = slot->GetContent();
    REQUIRE(content != nullptr);

    xamlHost->SetToolTip("shutdown-policy-fault");
    host->FlushSync();
    REQUIRE(wxWinUIGetManagedToolTipCountForTesting() > 0);

    wxWinUIResetToolTipPolicyDiagnosticsForTesting();
    wxWinUISetToolTipFaultForTesting(
        wxWinUIToolTipFault_Policy,
        std::numeric_limits<unsigned>::max());
    wxToolTip::Enable(false);
    REQUIRE(wxWinUIIsToolTipPolicyReplayScheduledForTesting());
    REQUIRE(wxWinUIGetToolTipPolicyReplayExecutionCountForTesting() == 0);

    // This is the exact production teardown hook, called before
    // WindowsXamlManager::Close(): it invalidates the queued token and drops
    // every registry-held strong XAML reference synchronously.
    wxWinUIShutdownToolTipPolicy();
    CHECK(wxWinUIIsToolTipPolicyShutdownForTesting());
    CHECK_FALSE(wxWinUIIsToolTipPolicyReplayScheduledForTesting());
    CHECK(wxWinUIGetManagedToolTipCountForTesting() == 0);

    DrainDispatch(5);
    CHECK(wxWinUIGetToolTipPolicyReplayExecutionCountForTesting() == 0);
    CHECK(wxWinUIGetManagedToolTipCountForTesting() == 0);

    // Re-arm without restarting the process, as a second bootstrap would.
    wxWinUIInitializeToolTipPolicy();
    wxWinUISetToolTipFaultForTesting(wxWinUIToolTipFault_None, 0);
    wxToolTip::Enable(true);
    CHECK_FALSE(wxWinUIIsToolTipPolicyShutdownForTesting());
    CHECK(GetPeerToolTipText(xamlHost) == "shutdown-policy-fault");

    xamlHost->UnsetToolTip();
    host->FlushSync();
    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::XamlAuthoredTooltipOwnership", "[HostState]")
{
    namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "state-xaml-tooltip");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" AutomationProperties.HelpText=\"xaml-help\">"
        "<ToolTipService.ToolTip>"
        "<ToolTip Content=\"xaml-tip\"/>"
        "</ToolTipService.ToolTip>"
        "</Border>"));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto content = slot->GetContent();
    REQUIRE(content != nullptr);

    // Initial state sync must not treat "no wxToolTip" as an instruction to
    // erase an authored XAML value.
    const auto baseline = MUXC::ToolTipService::GetToolTip(content);
    REQUIRE(baseline != nullptr);
    const auto baselineTip = baseline.try_as<MUXC::ToolTip>();
    REQUIRE(baselineTip != nullptr);
    CHECK(winrt::unbox_value<winrt::hstring>(baselineTip.Content()) ==
          L"xaml-tip");
    CHECK(MUXA::AutomationProperties::GetHelpText(content) ==
          L"xaml-help");

    // The global wx policy also applies to a ToolTip authored directly in
    // XAML, without converting it into a wx-owned string or losing identity.
    wxToolTip::Enable(false);
    host->FlushSync();
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);

    // Direct XAML writes have no wx geometry event. The per-content property
    // observer must schedule suppression without a manual host FlushSync().
    MUXC::ToolTip whileDisabled;
    whileDisabled.Content(winrt::box_value(L"while-disabled"));
    MUXC::ToolTipService::SetToolTip(content, whileDisabled);
    DrainDispatch(3);
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    wxToolTip::Enable(true);
    CHECK(MUXC::ToolTipService::GetToolTip(content) == whileDisabled);
    MUXC::ToolTipService::SetToolTip(content, baseline);
    host->FlushSync();

    // A direct authored replacement made immediately before the global call
    // is the new baseline. Enable() is synchronous: it must be hidden before
    // returning and restored by exact object identity afterwards.
    MUXC::ToolTip newerBaseline;
    newerBaseline.Content(winrt::box_value(L"xaml-tip-new"));
    MUXC::ToolTipService::SetToolTip(content, newerBaseline);
    wxToolTip::Enable(false);
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    wxToolTip::Enable(true);
    CHECK(MUXC::ToolTipService::GetToolTip(content) == newerBaseline);
    MUXC::ToolTipService::SetToolTip(content, baseline);
    host->FlushSync();

    // An explicit wx override owns the property temporarily; Unset restores
    // the exact original inspectable, not merely an equivalent string.
    xamlHost->SetToolTip("wx-tip");
    host->FlushSync();
    CHECK(GetPeerToolTipText(xamlHost) == "wx-tip");
    CHECK(MUXA::AutomationProperties::GetHelpText(content) ==
          L"wx-tip");

    xamlHost->UnsetToolTip();
    host->FlushSync();
    const auto restored = MUXC::ToolTipService::GetToolTip(content);
    CHECK(restored == baseline);
    CHECK(MUXA::AutomationProperties::GetHelpText(content) ==
          L"xaml-help");

    // Ownership is object identity, not text equality. An application object
    // with the same visible text wins and is not erased by UnsetToolTip().
    xamlHost->SetToolTip("same-text");
    host->FlushSync();
    MUXC::ToolTip sameTextReplacement;
    sameTextReplacement.Content(winrt::box_value(L"same-text"));
    MUXC::ToolTipService::SetToolTip(content, sameTextReplacement);
    wxToolTip::Enable(false);
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    wxToolTip::Enable(true);
    CHECK(MUXC::ToolTipService::GetToolTip(content) ==
          sameTextReplacement);
    host->FlushSync();
    xamlHost->UnsetToolTip();
    host->FlushSync();
    CHECK(MUXC::ToolTipService::GetToolTip(content) ==
          sameTextReplacement);
    MUXC::ToolTipService::SetToolTip(content, baseline);
    host->FlushSync();

    // A replacement written while a managed wx tooltip is globally
    // suppressed retires the old registry entry before the replacement is
    // masked. Re-enable must restore the application object, not the old wx
    // object.
    xamlHost->SetToolTip("owned-before-disable");
    host->FlushSync();
    wxToolTip::Enable(false);
    MUXC::ToolTip replacementWhileDisabled;
    replacementWhileDisabled.Content(
        winrt::box_value(L"replacement-while-disabled"));
    MUXC::ToolTipService::SetToolTip(
        content, replacementWhileDisabled);
    host->FlushSync();
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    wxToolTip::Enable(true);
    CHECK(MUXC::ToolTipService::GetToolTip(content) ==
          replacementWhileDisabled);
    xamlHost->UnsetToolTip();
    host->FlushSync();
    CHECK(MUXC::ToolTipService::GetToolTip(content) ==
          replacementWhileDisabled);
    MUXC::ToolTipService::SetToolTip(content, baseline);
    host->FlushSync();

    // An explicit empty wx tooltip masks the baseline with null too.
    xamlHost->SetToolTip(wxString());
    host->FlushSync();
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    xamlHost->UnsetToolTip();
    host->FlushSync();
    CHECK(MUXC::ToolTipService::GetToolTip(content) == baseline);
    CHECK(MUXA::AutomationProperties::GetHelpText(content) ==
          L"xaml-help");

    // The null mask remains owned when an existing non-empty wx tooltip is
    // changed to empty, and Unset restores the authored baseline.
    xamlHost->SetToolTip("non-empty-first");
    host->FlushSync();
    xamlHost->SetToolTip(wxString());
    host->FlushSync();
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    xamlHost->UnsetToolTip();
    host->FlushSync();
    CHECK(MUXC::ToolTipService::GetToolTip(content) == baseline);

    // Conversely, an application object installed after the explicit null
    // mask takes ownership and must survive UnsetToolTip().
    xamlHost->SetToolTip(wxString());
    host->FlushSync();
    MUXC::ToolTip afterMaskReplacement;
    afterMaskReplacement.Content(winrt::box_value(L"after-mask"));
    MUXC::ToolTipService::SetToolTip(content, afterMaskReplacement);
    xamlHost->UnsetToolTip();
    host->FlushSync();
    CHECK(MUXC::ToolTipService::GetToolTip(content) ==
          afterMaskReplacement);
    MUXC::ToolTipService::SetToolTip(content, baseline);
    host->FlushSync();

    // A direct XAML mutation while wx owns HelpText relinquishes only that
    // property. The next explicit wx-side change may reclaim it, and Unset
    // then restores the newer authored value rather than the initial one.
    xamlHost->SetToolTip("owned");
    host->FlushSync();
    MUXA::AutomationProperties::SetHelpText(content, L"app-help");
    host->FlushSync();
    CHECK(MUXA::AutomationProperties::GetHelpText(content) ==
          L"app-help");
    xamlHost->SetToolTip("reclaimed");
    host->FlushSync();
    CHECK(MUXA::AutomationProperties::GetHelpText(content) ==
          L"reclaimed");
    xamlHost->UnsetToolTip();
    host->FlushSync();
    CHECK(MUXA::AutomationProperties::GetHelpText(content) ==
          L"app-help");

    // A content swap retires the managed entry and restores the old XAML
    // element before it leaves the carrier.
    const unsigned managedBeforeSwap =
        wxWinUIGetManagedToolTipCountForTesting();
    xamlHost->SetToolTip("swap-owned");
    host->FlushSync();
    CHECK(wxWinUIGetManagedToolTipCountForTesting() ==
          managedBeforeSwap + 1);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    CHECK(MUXC::ToolTipService::GetToolTip(content) == baseline);
    host->FlushSync();
    CHECK(GetPeerToolTipText(xamlHost) == "swap-owned");
    CHECK(wxWinUIGetManagedToolTipCountForTesting() ==
          managedBeforeSwap + 1);
    xamlHost->UnsetToolTip();
    host->FlushSync();
    CHECK(wxWinUIGetManagedToolTipCountForTesting() ==
          managedBeforeSwap);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::TooltipOwnershipAcrossTLWReparent",
          "[HostState][HostLifecycle][tooltip]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    struct PolicyReset
    {
        ~PolicyReset() { wxToolTip::Enable(true); }
    } reset;
    wxToolTip::Enable(true);

    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "tooltip-reparent-A");
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "tooltip-reparent-B");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frameA);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"><ToolTipService.ToolTip>"
        "<ToolTip Content=\"authored\"/>"
        "</ToolTipService.ToolTip></Border>"));
    frameA->Show();
    frameB->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost *host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot *slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto content = slot->GetContent();
    const auto baseline = MUXC::ToolTipService::GetToolTip(content);
    REQUIRE(baseline != nullptr);
    const unsigned managedBefore =
        wxWinUIGetManagedToolTipCountForTesting();

    xamlHost->SetToolTip("cross-owned");
    host->FlushSync();
    CHECK(wxWinUIGetManagedToolTipCountForTesting() ==
          managedBefore + 1);

    wxToolTip::Enable(false);
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    REQUIRE(xamlHost->Reparent(frameB));
    host = wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    CHECK(host->GetTLW() == frameB);
    host->FlushSync();
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    wxToolTip::Enable(true);
    CHECK(GetPeerToolTipText(xamlHost) == "cross-owned");
    CHECK(wxWinUIGetManagedToolTipCountForTesting() ==
          managedBefore + 1);

    xamlHost->UnsetToolTip();
    host->FlushSync();
    CHECK(MUXC::ToolTipService::GetToolTip(content) == baseline);
    CHECK(wxWinUIGetManagedToolTipCountForTesting() == managedBefore);

    frameA->Destroy();
    frameB->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::AuthoredTooltipAcrossFrozenTLWReparent",
          "[HostState][HostLifecycle][tooltip][policy][winui-008f]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    struct PolicyReset
    {
        ~PolicyReset() { wxToolTip::Enable(true); }
    } reset;
    wxToolTip::Enable(true);

    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "authored-tooltip-frozen-A");
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "authored-tooltip-frozen-B");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frameA);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"><ToolTipService.ToolTip>"
        "<ToolTip Content=\"authored-only-frozen\"/>"
        "</ToolTipService.ToolTip></Border>"));
    ShowOffscreenWithoutActivating(frameA);
    ShowOffscreenWithoutActivating(frameB, 1);
    DrainToQuiescence();

    wxWinUITopLevelHost *host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot *slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto content = slot->GetContent();
    REQUIRE(content != nullptr);
    const auto baseline = MUXC::ToolTipService::GetToolTip(content);
    REQUIRE(baseline != nullptr);
    CHECK(xamlHost->GetToolTip() == nullptr);
    const unsigned managedBefore =
        wxWinUIGetManagedToolTipCountForTesting();

    // Global suppression must not lose an authored-only baseline while the
    // exact element migrates into a TLW whose ordinary flushes are frozen.
    frameB->Freeze();
    wxToolTip::Enable(false);
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);

    const bool reparented = xamlHost->Reparent(frameB);
    CHECK(reparented);
    host = wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    CHECK(host != nullptr);
    if ( host )
    {
        CHECK(host->GetTLW() == frameB);
        slot = host->FindSlot(xamlHost);
        CHECK(slot != nullptr);
        if ( slot )
            CHECK(slot->GetContent() == content);
    }
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    CHECK(wxWinUIGetManagedToolTipCountForTesting() == managedBefore);

    // Enable() is a synchronous process policy transaction even for a frozen
    // destination. It must restore the exact authored IInspectable.
    wxToolTip::Enable(true);
    CHECK(MUXC::ToolTipService::GetToolTip(content) == baseline);
    CHECK(xamlHost->GetToolTip() == nullptr);
    CHECK(wxWinUIGetManagedToolTipCountForTesting() == managedBefore);

    frameB->Thaw();
    DrainToQuiescence();
    frameA->Destroy();
    frameB->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::EmptyTooltipMaskAcrossFrozenTLWReparent",
          "[HostState][HostLifecycle][tooltip][policy][winui-008f]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    struct PolicyReset
    {
        ~PolicyReset() { wxToolTip::Enable(true); }
    } reset;
    wxToolTip::Enable(true);

    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "empty-tooltip-frozen-A");
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "empty-tooltip-frozen-B");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frameA);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"><ToolTipService.ToolTip>"
        "<ToolTip Content=\"empty-mask-baseline\"/>"
        "</ToolTipService.ToolTip></Border>"));
    ShowOffscreenWithoutActivating(frameA);
    ShowOffscreenWithoutActivating(frameB, 1);
    DrainToQuiescence();

    wxWinUITopLevelHost *host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot *slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto content = slot->GetContent();
    REQUIRE(content != nullptr);
    const auto baseline = MUXC::ToolTipService::GetToolTip(content);
    REQUIRE(baseline != nullptr);
    const unsigned managedBefore =
        wxWinUIGetManagedToolTipCountForTesting();

    // An explicit empty wxToolTip owns a null mask without a managed XAML
    // object. The authored object remains its restoration baseline.
    xamlHost->SetToolTip(wxString());
    host->FlushSync();
    REQUIRE(xamlHost->GetToolTip() != nullptr);
    CHECK(xamlHost->GetToolTipText().empty());
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    CHECK(wxWinUIGetManagedToolTipCountForTesting() == managedBefore);

    frameB->Freeze();
    wxToolTip::Enable(false);
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);

    const bool reparented = xamlHost->Reparent(frameB);
    CHECK(reparented);
    host = wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    CHECK(host != nullptr);
    if ( host )
    {
        CHECK(host->GetTLW() == frameB);
        slot = host->FindSlot(xamlHost);
        CHECK(slot != nullptr);
        if ( slot )
            CHECK(slot->GetContent() == content);
    }
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    CHECK(xamlHost->GetToolTip() != nullptr);
    CHECK(xamlHost->GetToolTipText().empty());

    // First prove that the logical empty mask itself migrated: re-enabling
    // while it is still present must keep the peer null, not reveal the
    // authored baseline from a destination slot which forgot wx ownership.
    wxToolTip::Enable(true);
    CHECK(MUXC::ToolTipService::GetToolTip(content) == nullptr);
    CHECK(xamlHost->GetToolTip() != nullptr);
    CHECK(xamlHost->GetToolTipText().empty());

    // Remove the logical null mask while the destination is still frozen and
    // suppressed. Re-enabling policy must observe that wx-side change and
    // restore the exact authored baseline without waiting for a geometry pass.
    wxToolTip::Enable(false);
    xamlHost->UnsetToolTip();
    CHECK(xamlHost->GetToolTip() == nullptr);
    wxToolTip::Enable(true);
    CHECK(MUXC::ToolTipService::GetToolTip(content) == baseline);
    CHECK(wxWinUIGetManagedToolTipCountForTesting() == managedBefore);

    frameB->Thaw();
    DrainToQuiescence();
    frameA->Destroy();
    frameB->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::StyledTooltipLocalProvenance",
          "[HostState][HostLifecycle][tooltip][policy][winui-008f]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
    namespace MUXM = winrt::Microsoft::UI::Xaml::Markup;

    struct PolicyReset
    {
        ~PolicyReset() { wxToolTip::Enable(true); }
    } reset;
    wxToolTip::Enable(true);

    const auto toolTipProperty =
        MUXC::ToolTipService::ToolTipProperty();
    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "styled-tooltip-A");
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "styled-tooltip-B");

    wxWinUIXamlHost * const styledHost =
        new wxWinUIXamlHost(frameA);
    REQUIRE(styledHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"styled\">"
        "<Button.Style><Style TargetType=\"Button\">"
        "<Setter Property=\"ToolTipService.ToolTip\" "
        "Value=\"styled-tooltip\"/>"
        "</Style></Button.Style></Button>"));

    wxWinUIXamlHost * const emptyStyledHost =
        new wxWinUIXamlHost(frameA);
    REQUIRE(emptyStyledHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"empty-styled\">"
        "<Button.Style><Style TargetType=\"Button\">"
        "<Setter Property=\"ToolTipService.ToolTip\" Value=\"\"/>"
        "</Style></Button.Style></Button>"));

    wxWinUIXamlHost * const lateStyleHost =
        new wxWinUIXamlHost(frameA);
    REQUIRE(lateStyleHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"late-styled\"/>"));

    wxWinUIXamlHost * const clearedDuringSuppressionHost =
        new wxWinUIXamlHost(frameA);
    REQUIRE(clearedDuringSuppressionHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"clear-suppressed\" "
        "ToolTipService.ToolTip=\"authored-before-clear\"/>"));

    ShowOffscreenWithoutActivating(frameA);
    ShowOffscreenWithoutActivating(frameB, 1);
    DrainToQuiescence();

    wxWinUITopLevelHost * const hostA =
        wxWinUITopLevelHost::FindSlotOwner(styledHost);
    REQUIRE(hostA != nullptr);
    const auto styledContent =
        hostA->FindSlot(styledHost)->GetContent();
    const auto emptyStyledContent =
        hostA->FindSlot(emptyStyledHost)->GetContent();
    const auto lateStyleContent =
        hostA->FindSlot(lateStyleHost)->GetContent();
    const auto clearedDuringSuppressionContent =
        hostA->FindSlot(clearedDuringSuppressionHost)->GetContent();
    REQUIRE(styledContent != nullptr);
    REQUIRE(emptyStyledContent != nullptr);
    REQUIRE(lateStyleContent != nullptr);
    REQUIRE(clearedDuringSuppressionContent != nullptr);

    REQUIRE(styledContent.ReadLocalValue(toolTipProperty) ==
            MUX::DependencyProperty::UnsetValue());
    REQUIRE(GetToolTipInspectableText(
                MUXC::ToolTipService::GetToolTip(styledContent)) ==
            "styled-tooltip");
    REQUIRE(emptyStyledContent.ReadLocalValue(toolTipProperty) ==
            MUX::DependencyProperty::UnsetValue());
    // WinUI normalizes an empty string Setter to an effective null. The
    // important contract here is that the Style still owns precedence:
    // there must be no local value pinning the property.
    CHECK(MUXC::ToolTipService::GetToolTip(emptyStyledContent) == nullptr);
    REQUIRE(clearedDuringSuppressionContent.ReadLocalValue(
                toolTipProperty) !=
            MUX::DependencyProperty::UnsetValue());
    REQUIRE(GetToolTipInspectableText(
                MUXC::ToolTipService::GetToolTip(
                    clearedDuringSuppressionContent)) ==
            "authored-before-clear");

    // Global suppression owns a temporary local null. Re-enable must ClearValue
    // when the original local baseline was Unset, revealing the Style again.
    wxToolTip::Enable(false);
    CHECK(MUXC::ToolTipService::GetToolTip(styledContent) == nullptr);
    CHECK(MUXC::ToolTipService::GetToolTip(emptyStyledContent) == nullptr);
    CHECK(MUXC::ToolTipService::GetToolTip(
              clearedDuringSuppressionContent) == nullptr);
    // Effective null is unchanged by this ClearValue(), so an effective-value
    // observer alone cannot reveal the application mutation. Re-enable must
    // inspect the local provenance and must not restore the retired baseline.
    clearedDuringSuppressionContent.ClearValue(toolTipProperty);
    wxToolTip::Enable(true);
    CHECK(styledContent.ReadLocalValue(toolTipProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(GetToolTipInspectableText(
              MUXC::ToolTipService::GetToolTip(styledContent)) ==
          "styled-tooltip");
    CHECK(emptyStyledContent.ReadLocalValue(toolTipProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXC::ToolTipService::GetToolTip(emptyStyledContent) == nullptr);
    CHECK(clearedDuringSuppressionContent.ReadLocalValue(
              toolTipProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXC::ToolTipService::GetToolTip(
              clearedDuringSuppressionContent) == nullptr);

    // The ordinary wx Set/Unset round-trip has the same provenance rule.
    styledHost->SetToolTip("wx-override");
    hostA->FlushSync();
    CHECK(styledContent.ReadLocalValue(toolTipProperty) !=
          MUX::DependencyProperty::UnsetValue());
    styledHost->UnsetToolTip();
    hostA->FlushSync();
    CHECK(styledContent.ReadLocalValue(toolTipProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(GetToolTipInspectableText(
              MUXC::ToolTipService::GetToolTip(styledContent)) ==
          "styled-tooltip");

    // With no authored baseline, Set/Unset must also clear wx's local null;
    // otherwise a Style assigned later would remain permanently masked.
    lateStyleHost->SetToolTip("temporary-wx-tip");
    hostA->FlushSync();
    lateStyleHost->UnsetToolTip();
    hostA->FlushSync();
    REQUIRE(lateStyleContent.ReadLocalValue(toolTipProperty) ==
            MUX::DependencyProperty::UnsetValue());
    const auto lateStyleSource =
        MUXM::XamlReader::Load(
            L"<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/"
            L"xaml/presentation\"><Button.Style>"
            L"<Style TargetType=\"Button\">"
            L"<Setter Property=\"ToolTipService.ToolTip\" "
            L"Value=\"late-style-tooltip\"/>"
            L"</Style></Button.Style></Button>")
            .as<MUXC::Button>();
    lateStyleContent.as<MUX::FrameworkElement>().Style(
        lateStyleSource.Style());
    CHECK(lateStyleContent.ReadLocalValue(toolTipProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(GetToolTipInspectableText(
              MUXC::ToolTipService::GetToolTip(lateStyleContent)) ==
          "late-style-tooltip");

    // Migration and detach preserve the exact local Unset provenance for both
    // non-empty and empty Style setters.
    REQUIRE(styledHost->Reparent(frameB));
    REQUIRE(emptyStyledHost->Reparent(frameB));
    wxWinUITopLevelHost * const hostB =
        wxWinUITopLevelHost::FindSlotOwner(styledHost);
    REQUIRE(hostB != nullptr);
    hostB->FlushSync();
    CHECK(styledContent.ReadLocalValue(toolTipProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(emptyStyledContent.ReadLocalValue(toolTipProperty) ==
          MUX::DependencyProperty::UnsetValue());

    styledHost->Destroy();
    emptyStyledHost->Destroy();
    lateStyleHost->Destroy();
    clearedDuringSuppressionHost->Destroy();
    DrainDispatch();
    CHECK(styledContent.ReadLocalValue(toolTipProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(emptyStyledContent.ReadLocalValue(toolTipProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(lateStyleContent.ReadLocalValue(toolTipProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(clearedDuringSuppressionContent.ReadLocalValue(
              toolTipProperty) ==
          MUX::DependencyProperty::UnsetValue());

    frameA->Destroy();
    frameB->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::TooltipObserverRegistrationRetry",
          "[HostState][HostLifecycle][tooltip][winui-008f]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    struct PolicyReset
    {
        ~PolicyReset()
        {
            wxWinUITopLevelHost::TestFailToolTipObserverAdds(0);
            wxToolTip::Enable(true);
        }
    } reset;
    wxToolTip::Enable(true);

    // Observer installation is auxiliary to the truthful publication of a
    // fresh slot. A transient failure must leave the slot installed and
    // explicitly pending, then recover through the bounded deferred retry.
    wxFrame * const freshFrame =
        new wxFrame(nullptr, wxID_ANY, "tooltip-observer-fresh-slot");
    wxWinUIXamlHost * const freshXamlHost = new wxWinUIXamlHost(freshFrame);
    wxWinUITopLevelHost::TestFailToolTipObserverAdds(1);
    REQUIRE(freshXamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    wxWinUITopLevelHost * const freshHost =
        wxWinUITopLevelHost::FindSlotOwner(freshXamlHost);
    REQUIRE(freshHost != nullptr);
    wxWinUISlot * const freshSlot = freshHost->FindSlot(freshXamlHost);
    REQUIRE(freshSlot != nullptr);
    CHECK(freshSlot->IsToolTipPropertyObserverPendingForTest());
    DrainToQuiescence();
    CHECK_FALSE(freshSlot->IsToolTipPropertyObserverPendingForTest());
    CHECK(freshSlot->GetToolTipPropertyObserverRetryFailuresForTest() == 0);
    freshFrame->Destroy();
    DrainDispatch();

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "tooltip-observer-retry");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);

    const MUXC::Border replacement;
    wxWinUITopLevelHost::TestFailToolTipObserverAdds(2);

    // Content placement remains truthful when the auxiliary observer fails.
    // Two coalesced deferred attempts are enough to consume the second fault
    // and then install successfully; no explicit same-content call is needed.
    CHECK(slot->SetContent(replacement));
    CHECK(slot->GetContent() == replacement);
    CHECK(slot->IsToolTipPropertyObserverPendingForTest());
    DrainToQuiescence();
    CHECK_FALSE(slot->IsToolTipPropertyObserverPendingForTest());
    CHECK(slot->GetToolTipPropertyObserverRetryFailuresForTest() == 0);

    wxToolTip::Enable(false);
    MUXC::ToolTip authoredWhileDisabled;
    authoredWhileDisabled.Content(winrt::box_value(L"observer-retry"));
    MUXC::ToolTipService::SetToolTip(
        replacement, authoredWhileDisabled);
    DrainDispatch(3);
    CHECK(MUXC::ToolTipService::GetToolTip(replacement) == nullptr);

    wxToolTip::Enable(true);
    CHECK(MUXC::ToolTipService::GetToolTip(replacement) ==
          authoredWhileDisabled);

    // A new content transaction must enforce process-wide suppression before
    // returning even if observer registration fails and the TLW is frozen.
    // With no observer installed, a subsequent authored write remains visible
    // until the explicit same-content retry; that retry must suppress it even
    // when registration fails again and remains pending.
    const MUXC::Border frozenReplacement;
    MUXC::ToolTip frozenInitialBaseline;
    frozenInitialBaseline.Content(
        winrt::box_value(L"observer-frozen-new-content"));
    MUXC::ToolTipService::SetToolTip(
        frozenReplacement, frozenInitialBaseline);
    DrainToQuiescence();
    const unsigned frozenRuns =
        host->GetOwnFlushRunCountForTest();
    frame->Freeze();
    wxToolTip::Enable(false);
    wxWinUITopLevelHost::TestFailToolTipObserverAdds(2);

    CHECK(slot->SetContent(frozenReplacement));
    CHECK(slot->GetContent() == frozenReplacement);
    CHECK(slot->IsToolTipPropertyObserverPendingForTest());
    CHECK(slot->GetToolTipPropertyObserverRetryFailuresForTest() == 1);
    CHECK(MUXC::ToolTipService::GetToolTip(frozenReplacement) == nullptr);
    CHECK(host->GetOwnFlushRunCountForTest() == frozenRuns);

    MUXC::ToolTip frozenSameContentBaseline;
    frozenSameContentBaseline.Content(
        winrt::box_value(L"observer-frozen-same-content"));
    MUXC::ToolTipService::SetToolTip(
        frozenReplacement, frozenSameContentBaseline);
    CHECK(MUXC::ToolTipService::GetToolTip(frozenReplacement) ==
          frozenSameContentBaseline);

    CHECK_FALSE(slot->SetContent(frozenReplacement));
    CHECK(slot->GetContent() == frozenReplacement);
    CHECK(slot->IsToolTipPropertyObserverPendingForTest());
    CHECK(slot->GetToolTipPropertyObserverRetryFailuresForTest() == 2);
    CHECK(MUXC::ToolTipService::GetToolTip(frozenReplacement) == nullptr);
    CHECK(host->GetOwnFlushRunCountForTest() == frozenRuns);

    wxWinUITopLevelHost::TestFailToolTipObserverAdds(0);
    CHECK(slot->SetContent(frozenReplacement));
    CHECK_FALSE(slot->IsToolTipPropertyObserverPendingForTest());
    CHECK(slot->GetToolTipPropertyObserverRetryFailuresForTest() == 0);
    CHECK(MUXC::ToolTipService::GetToolTip(frozenReplacement) == nullptr);
    CHECK(host->GetOwnFlushRunCountForTest() == frozenRuns);

    wxToolTip::Enable(true);
    CHECK(MUXC::ToolTipService::GetToolTip(frozenReplacement) ==
          frozenSameContentBaseline);
    CHECK(host->GetOwnFlushRunCountForTest() == frozenRuns);
    frame->Thaw();
    DrainToQuiescence();

    // A permanently failing registration has a strict automatic budget.
    // It remains explicitly pending after three attempts but posts no
    // unbounded self-retry storm; a later explicit same-content call can
    // recover once the fault is removed.
    const MUXC::Border boundedReplacement;
    wxWinUITopLevelHost::TestFailToolTipObserverAdds(20);
    const unsigned schedulesBefore =
        wxWinUITopLevelHost::GetFlushScheduleCount();
    CHECK(slot->SetContent(boundedReplacement));
    DrainToQuiescence();
    CHECK(slot->IsToolTipPropertyObserverPendingForTest());
    CHECK(slot->GetToolTipPropertyObserverRetryFailuresForTest() == 3);
    CHECK(wxWinUITopLevelHost::GetFlushScheduleCount() -
              schedulesBefore <= 4);

    wxWinUITopLevelHost::TestFailToolTipObserverAdds(0);
    CHECK(slot->SetContent(boundedReplacement));
    CHECK_FALSE(slot->IsToolTipPropertyObserverPendingForTest());
    CHECK(slot->GetToolTipPropertyObserverRetryFailuresForTest() == 0);

    wxToolTip::Enable(false);
    MUXC::ToolTip afterRecovery;
    afterRecovery.Content(winrt::box_value(L"after-recovery"));
    MUXC::ToolTipService::SetToolTip(boundedReplacement, afterRecovery);
    DrainDispatch(3);
    CHECK(MUXC::ToolTipService::GetToolTip(boundedReplacement) == nullptr);
    wxToolTip::Enable(true);
    CHECK(MUXC::ToolTipService::GetToolTip(boundedReplacement) ==
          afterRecovery);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::SemanticPropertiesRestoreOnDetach",
          "[HostState][HostLifecycle][accessibility]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;

    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "semantics-detach-A");
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "semantics-detach-B");
    wxWinUIXamlHost * const xamlHost =
        new wxWinUIXamlHost(frameA, wxID_ANY,
                            wxDefaultPosition, wxDefaultSize,
                            wxTAB_TRAVERSAL | wxBORDER_NONE,
                            "semantic-technical-id");
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" AutomationProperties.HelpText=\"authored-a\"/>"));
    CHECK(xamlHost->GetName() == "semantic-technical-id");
    CHECK(xamlHost->GetLabel().empty());
    xamlHost->SetLabel("semantic-host");
    CHECK(xamlHost->GetName() == "semantic-technical-id");
    CHECK(xamlHost->GetLabel() == "semantic-host");
    xamlHost->SetToolTip("wx-help");
    frameA->Show();
    frameB->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost *host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    host->FlushSync();
    wxWinUISlot *slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto contentA = slot->GetContent();
    REQUIRE(contentA != nullptr);
    CHECK(MUXA::AutomationProperties::GetHelpText(contentA) ==
          L"wx-help");
    CHECK(MUXA::AutomationProperties::GetName(contentA) ==
          L"semantic-host");

    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" AutomationProperties.HelpText=\"authored-b\"/>"));
    CHECK(MUXA::AutomationProperties::GetHelpText(contentA) ==
          L"authored-a");
    CHECK(contentA.ReadLocalValue(
              MUXA::AutomationProperties::NameProperty()) ==
          MUX::DependencyProperty::UnsetValue());

    host->FlushSync();
    slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto contentB = slot->GetContent();
    REQUIRE(contentB != nullptr);
    CHECK(MUXA::AutomationProperties::GetHelpText(contentB) ==
          L"wx-help");
    CHECK(MUXA::AutomationProperties::GetName(contentB) ==
          L"semantic-host");

    // Direct application mutations relinquish ownership and survive detach.
    MUXA::AutomationProperties::SetHelpText(contentB, L"app-help");
    MUXA::AutomationProperties::SetName(contentB, L"app-name");
    host->FlushSync();
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" AutomationProperties.HelpText=\"authored-c\"/>"));
    CHECK(MUXA::AutomationProperties::GetHelpText(contentB) ==
          L"app-help");
    CHECK(MUXA::AutomationProperties::GetName(contentB) ==
          L"app-name");

    host->FlushSync();
    slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto contentC = slot->GetContent();
    REQUIRE(contentC != nullptr);
    CHECK(MUXA::AutomationProperties::GetHelpText(contentC) ==
          L"wx-help");
    CHECK(MUXA::AutomationProperties::GetName(contentC) ==
          L"semantic-host");

    REQUIRE(xamlHost->Reparent(frameB));
    host = wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    host->FlushSync();
    CHECK(MUXA::AutomationProperties::GetHelpText(contentC) ==
          L"wx-help");
    CHECK(MUXA::AutomationProperties::GetName(contentC) ==
          L"semantic-host");

    xamlHost->Destroy();
    DrainDispatch();
    CHECK(MUXA::AutomationProperties::GetHelpText(contentC) ==
          L"authored-c");
    CHECK(contentC.ReadLocalValue(
              MUXA::AutomationProperties::NameProperty()) ==
          MUX::DependencyProperty::UnsetValue());

    frameA->Destroy();
    frameB->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::TooltipSetBeforeCreate", "[HostState]")
{
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "state-tooltip-precreate");
    wxButton * const button = new wxButton;

    // This associates the wxToolTip before an HWND or slot exists. Slot
    // registration must reclassify it as an island tooltip so later direct
    // wxToolTip mutations still dirty the peer.
    button->SetToolTip("pre");
    REQUIRE(button->Create(frame, wxID_ANY, "button"));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host != nullptr);
    REQUIRE(button->GetToolTip() != nullptr);
    CHECK(button->GetToolTip()->GetWindow() == button);
    CHECK(GetPeerToolTipText(button) == "pre");

    button->GetToolTip()->SetTip("post");
    host->FlushSync();
    DrainToQuiescence();
    CHECK(GetPeerToolTipText(button) == "post");

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::TooltipCanonicalObjectMatrix", "[HostState]")
{
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "state-tooltip-matrix");
    const wxString choices[] = { "one", "two" };

    std::vector<std::pair<const char *, wxWindow *>> windows;
    windows.push_back({ "button",
                        new wxButton(frame, wxID_ANY, "button") });
    windows.push_back({ "checkbox",
                        new wxCheckBox(frame, wxID_ANY, "checkbox") });
    windows.push_back({ "radiobutton",
                        new wxRadioButton(frame, wxID_ANY, "radio") });
    windows.push_back({ "radiobox",
                        new wxRadioBox(frame, wxID_ANY, "group",
                                       wxDefaultPosition, wxDefaultSize,
                                       WXSIZEOF(choices), choices) });
    windows.push_back({ "textctrl",
                        new wxTextCtrl(frame, wxID_ANY, "text") });

    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    windows.push_back({ "xamlhost", xamlHost });

    frame->Show();
    DrainToQuiescence();

    for ( const auto& entry : windows )
    {
        INFO(entry.first);
        wxWindow * const window = entry.second;
        wxWinUITopLevelHost * const host =
            wxWinUITopLevelHost::FindSlotOwner(window);
        REQUIRE(host != nullptr);

        window->SetToolTip("one");
        host->FlushSync();
        REQUIRE(window->GetToolTip() != nullptr);
        CHECK(window->GetToolTip()->GetWindow() == window);
        CHECK(window->GetToolTipText() == "one");
        CHECK(GetPeerToolTipText(window) == "one");

        window->GetToolTip()->SetTip("two");
        host->FlushSync();
        CHECK(window->GetToolTipText() == "two");
        CHECK(GetPeerToolTipText(window) == "two");

        window->SetToolTip(new wxToolTip("three"));
        host->FlushSync();
        REQUIRE(window->GetToolTip() != nullptr);
        CHECK(window->GetToolTip()->GetWindow() == window);
        CHECK(GetPeerToolTipText(window) == "three");

        // Empty text keeps the canonical wxToolTip object but clears the
        // visual peer; only UnsetToolTip() removes the object itself.
        window->SetToolTip(wxString());
        host->FlushSync();
        CHECK(window->GetToolTip() != nullptr);
        bool peerPresent = true;
        GetPeerToolTipText(window, &peerPresent);
        CHECK(!peerPresent);

        window->UnsetToolTip();
        host->FlushSync();
        CHECK(window->GetToolTip() == nullptr);
        GetPeerToolTipText(window, &peerPresent);
        CHECK(!peerPresent);
    }

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::SyncSlotContentSwapReentrancy", "[HostState]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "state-sync-reentry");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto oldContent = slot->GetContent();
    REQUIRE(oldContent != nullptr);
    const MUXC::Border replacement;

    bool callbackFired = false;
    bool swapSucceeded = false;
    const int64_t callbackToken = oldContent.RegisterPropertyChangedCallback(
        MUXC::ToolTipService::ToolTipProperty(),
        [&](MUX::DependencyObject const&, MUX::DependencyProperty const&)
        {
            if ( callbackFired )
                return;
            callbackFired = true;
            swapSucceeded = slot->SetContent(replacement);
        });

    xamlHost->SetToolTip("after");
    host->FlushSync();
    DrainToQuiescence();
    oldContent.UnregisterPropertyChangedCallback(
        MUXC::ToolTipService::ToolTipProperty(), callbackToken);

    CHECK(callbackFired);
    CHECK(swapSucceeded);
    CHECK(slot->GetContent() == replacement);
    CHECK(GetPeerToolTipText(xamlHost) == "after");

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::ContentSwapFlushSeesOnlyPublishedModel",
          "[HostState][HostLifecycle][tooltip][accessibility]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "content-swap-flush");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    xamlHost->SetLabel("swap-wx-name");
    xamlHost->SetToolTip("swap-wx-tip");
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" AutomationProperties.HelpText=\"authored-a\">"
        "<ToolTipService.ToolTip><ToolTip Content=\"tip-a\"/>"
        "</ToolTipService.ToolTip></Border>"));
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto contentA = slot->GetContent();
    REQUIRE(contentA != nullptr);
    const auto baselineA = MUXC::ToolTipService::GetToolTip(contentA);
    REQUIRE(baselineA != nullptr);

    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();
    CHECK(MUXC::ToolTipService::GetToolTip(contentA) != baselineA);
    const unsigned managedWithA =
        wxWinUIGetManagedToolTipCountForTesting();
    REQUIRE(managedWithA > 0);
    const unsigned managedBefore =
        managedWithA - 1;

    const MUXC::Border contentB;
    MUXC::ToolTip baselineB;
    baselineB.Content(winrt::box_value(L"tip-b"));
    MUXC::ToolTipService::SetToolTip(contentB, baselineB);
    MUXA::AutomationProperties::SetHelpText(contentB, L"authored-b");

    bool seamRan = false;
    wxWinUITopLevelHost::TestOnNextContentCarrierSet(
        [&](wxWindow *window)
        {
            REQUIRE(window == xamlHost);
            seamRan = true;
            CHECK(slot->IsContentTransactionInProgressForTest());
            CHECK(slot->GetContent() == contentA);
            winrt::Microsoft::UI::Xaml::UIElement carrier{ nullptr };
            REQUIRE(slot->TryGetCarrierContent(carrier));
            CHECK(carrier == contentB);
            host->FlushSync();
            CHECK(slot->GetContent() == contentA);
        });

    REQUIRE(slot->SetContent(contentB));
    wxWinUITopLevelHost::TestOnNextContentCarrierSet({});
    CHECK(seamRan);
    CHECK_FALSE(slot->IsContentTransactionInProgressForTest());
    CHECK(slot->GetContent() == contentB);
    CHECK(MUXC::ToolTipService::GetToolTip(contentA) == baselineA);
    CHECK(MUXA::AutomationProperties::GetHelpText(contentA) ==
          L"authored-a");
    CHECK(contentA.ReadLocalValue(
              MUXA::AutomationProperties::NameProperty()) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(wxWinUIGetManagedToolTipCountForTesting() == managedBefore);

    host->FlushSync();
    CHECK(GetToolTipInspectableText(
              MUXC::ToolTipService::GetToolTip(contentB)) ==
          "swap-wx-tip");
    CHECK(MUXA::AutomationProperties::GetHelpText(contentB) ==
          L"swap-wx-tip");
    CHECK(MUXA::AutomationProperties::GetName(contentB) ==
          L"swap-wx-name");
    CHECK(wxWinUIGetManagedToolTipCountForTesting() ==
          managedBefore + 1);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::LoadedNotificationIsContentTransactional",
          "[HostState][HostLifecycle][focus]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "loaded-content-transaction");
    wxWindow * const probe =
        new wxWindow(frame, wxID_ANY, wxPoint(10, 10), wxSize(80, 30));
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(probe, true);
    REQUIRE(host != nullptr);

    const MUXC::Border contentA;
    wxWinUISlot * const slot = host->RegisterSlot(probe, contentA);
    REQUIRE(slot != nullptr);
    REQUIRE(slot->HasContentLoadedObserverForTest());
    slot->SetContentLoadedForTest(false);
    REQUIRE_FALSE(slot->IsContentLoadedForTest());

    // Force the install of B to fail after A was detached. The real observer
    // callback core fires while the transaction still publishes A; it must be
    // latched, not applied to the A-model/B-carrier intermediate state, and
    // then transferred to the restored generation only.
    bool rollbackSeamRan = false;
    wxWinUITopLevelHost::TestOnNextContentTransactionStarted(
        [&](wxWindow *window)
        {
            REQUIRE(window == probe);
            rollbackSeamRan = true;
            REQUIRE(slot->IsContentTransactionInProgressForTest());
            slot->TestInvokeContentLoadedObserver();
            CHECK_FALSE(slot->IsContentLoadedForTest());
        });
    wxWinUITopLevelHost::TestFailContentSwap(
        wxWinUITopLevelHost::TestContentFault_Install);
    CHECK_FALSE(slot->SetContent(MUXC::Button()));
    wxWinUITopLevelHost::TestOnNextContentTransactionStarted({});
    wxWinUITopLevelHost::TestFailContentSwap(0);

    REQUIRE(rollbackSeamRan);
    CHECK_FALSE(slot->IsContentTransactionInProgressForTest());
    CHECK(slot->GetContent() == contentA);
    CHECK(slot->IsContentLoadedForTest());
    CHECK(slot->HasContentLoadedObserverForTest());

    // A successful commit must discard the same old-generation latch. Null
    // content is used so no legitimate Loaded edge for B can obscure the
    // postcondition.
    bool commitSeamRan = false;
    wxWinUITopLevelHost::TestOnNextContentTransactionStarted(
        [&](wxWindow *window)
        {
            REQUIRE(window == probe);
            commitSeamRan = true;
            slot->TestInvokeContentLoadedObserver();
        });
    REQUIRE(slot->SetContent(nullptr));
    wxWinUITopLevelHost::TestOnNextContentTransactionStarted({});

    REQUIRE(commitSeamRan);
    CHECK_FALSE(slot->IsContentTransactionInProgressForTest());
    CHECK(slot->GetContent() == nullptr);
    CHECK_FALSE(slot->IsContentLoadedForTest());
    CHECK_FALSE(slot->HasContentLoadedObserverForTest());

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::LoadedObserverRetryIsBounded",
          "[HostLifecycle][HostState]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    struct FaultReset
    {
        ~FaultReset()
        {
            wxWinUITopLevelHost::TestFailContentLoadedObserverAdds(0);
        }
    } reset;

    DrainToQuiescence();
    const unsigned schedulesBefore =
        wxWinUITopLevelHost::GetFlushScheduleCount();

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "loaded-observer-retry");
    wxWindow * const probe =
        new wxWindow(frame, wxID_ANY, wxPoint(10, 10), wxSize(80, 30));
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(probe, true);
    REQUIRE(host != nullptr);

    wxWinUITopLevelHost::TestFailContentLoadedObserverAdds(20);
    const MUXC::Border content;
    wxWinUISlot * const slot = host->RegisterSlot(probe, content);
    REQUIRE(slot != nullptr);
    REQUIRE(slot->IsContentLoadedObserverPendingForTest());

    DrainToQuiescence();
    CHECK(slot->IsContentLoadedObserverPendingForTest());
    CHECK(slot->GetContentLoadedObserverRetryFailuresForTest() == 3);
    CHECK_FALSE(slot->HasContentLoadedObserverForTest());
    // Initial slot synchronization plus the two autonomous retries remain a
    // small constant; a permanent HRESULT never creates a CallAfter storm.
    CHECK(wxWinUITopLevelHost::GetFlushScheduleCount() -
              schedulesBefore <= 4);

    wxWinUITopLevelHost::TestFailContentLoadedObserverAdds(0);
    REQUIRE(slot->SetContent(content));
    CHECK_FALSE(slot->IsContentLoadedObserverPendingForTest());
    CHECK(slot->GetContentLoadedObserverRetryFailuresForTest() == 0);
    CHECK(slot->HasContentLoadedObserverForTest());

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::NestedSyncEpochSupersedesOuterEnabledPass",
          "[HostState][HostLifecycle]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "sync-epoch-enabled");
    wxButton * const button =
        new wxButton(frame, wxID_ANY, "epoch-button");
    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(button);
    REQUIRE(slot != nullptr);
    const auto container = slot->GetContainer();
    REQUIRE(container != nullptr);
    const unsigned long long epochBefore = slot->GetSyncEpochForTest();

    bool callbackRan = false;
    bool inCallback = false;
    const int64_t token = container.RegisterPropertyChangedCallback(
        MUX::UIElement::IsHitTestVisibleProperty(),
        [&](MUX::DependencyObject const&, MUX::DependencyProperty const&)
        {
            if ( inCallback || container.IsHitTestVisible() )
                return;
            inCallback = true;
            callbackRan = true;
            button->Enable();
            host->FlushSync();
            inCallback = false;
        });

    button->Disable();
    host->FlushSync();
    CHECK(callbackRan);
    CHECK(button->IsEnabled());
    CHECK(container.IsHitTestVisible());
    CHECK(slot->GetSyncEpochForTest() >= epochBefore + 2);

    container.UnregisterPropertyChangedCallback(
        MUX::UIElement::IsHitTestVisibleProperty(), token);

    // The stale outer false verdict was never cached: a later real disable
    // still reaches both the carrier inheritance and hit-test seam.
    button->Disable();
    host->FlushSync();
    CHECK_FALSE(container.IsHitTestVisible());

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::ScalarSetterCallbackKeepsApplicationOwnership",
          "[HostState][HostLifecycle][accessibility]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "scalar-callback-ownership");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    xamlHost->SetLabel("name-one");
    xamlHost->SetToolTip("help-one");
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto content = slot->GetContent();
    REQUIRE(content != nullptr);

    bool nameCallback = false;
    bool inNameCallback = false;
    const auto nameProperty =
        MUXA::AutomationProperties::NameProperty();
    const int64_t nameToken = content.RegisterPropertyChangedCallback(
        nameProperty,
        [&](MUX::DependencyObject const&, MUX::DependencyProperty const&)
        {
            if ( inNameCallback )
                return;
            inNameCallback = true;
            nameCallback = true;
            MUXA::AutomationProperties::SetName(
                content, L"application-name");
            inNameCallback = false;
        });

    xamlHost->SetLabel("name-two");
    host->FlushSync();
    CHECK(nameCallback);
    CHECK(MUXA::AutomationProperties::GetName(content) ==
          L"application-name");
    content.UnregisterPropertyChangedCallback(nameProperty, nameToken);

    // A later wx label mutation must not reclaim a Name which the callback
    // made application-owned in the same setter transaction.
    xamlHost->SetLabel("name-three");
    host->FlushSync();
    CHECK(MUXA::AutomationProperties::GetName(content) ==
          L"application-name");

    bool helpCallback = false;
    bool inHelpCallback = false;
    const auto helpProperty =
        MUXA::AutomationProperties::HelpTextProperty();
    const int64_t helpToken = content.RegisterPropertyChangedCallback(
        helpProperty,
        [&](MUX::DependencyObject const&, MUX::DependencyProperty const&)
        {
            if ( inHelpCallback )
                return;
            inHelpCallback = true;
            helpCallback = true;
            MUXA::AutomationProperties::SetHelpText(
                content, L"application-help");
            inHelpCallback = false;
        });

    xamlHost->SetToolTip("help-two");
    host->FlushSync();
    CHECK(helpCallback);
    CHECK(MUXA::AutomationProperties::GetHelpText(content) ==
          L"application-help");
    content.UnregisterPropertyChangedCallback(helpProperty, helpToken);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::SyncSlotUnregisterReentrancy", "[HostState]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    const unsigned states0 =
        wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "state-sync-unregister");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto content = slot->GetContent();
    REQUIRE(content != nullptr);

    bool callbackFired = false;
    const int64_t callbackToken = content.RegisterPropertyChangedCallback(
        MUXC::ToolTipService::ToolTipProperty(),
        [&](MUX::DependencyObject const&, MUX::DependencyProperty const&)
        {
            if ( callbackFired )
                return;
            callbackFired = true;
            host->UnregisterSlot(xamlHost);
        });

    xamlHost->SetToolTip("unregister");
    host->FlushSync();
    DrainToQuiescence();
    content.UnregisterPropertyChangedCallback(
        MUXC::ToolTipService::ToolTipProperty(), callbackToken);

    CHECK(callbackFired);
    CHECK(host->FindSlot(xamlHost) == nullptr);
    CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() == states0);

    frame->Destroy();
    DrainDispatch();
}
#endif // wxUSE_TOOLTIPS

TEST_CASE("HostState::AutomationState", "[HostState]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    using MUX::Automation::AutomationProperties;
    using MUX::Automation::Peers::FrameworkElementAutomationPeer;

    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "state-uia");
    // A real control label remains semantic even when it equals a standard
    // class/default name filtered for infrastructure wxWindow instances.
    wxButton * const button = new wxButton(frame, wxID_ANY, "button");
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(button);
    REQUIRE(slot != nullptr);
    const auto content = slot->GetContent();
    REQUIRE(content != nullptr);

    CHECK(wxString(AutomationProperties::GetName(content).c_str())
              == "button");

    // the UIA name follows wx label changes
    button->SetLabel("after");
    DrainDispatch(3);
    CHECK(wxString(AutomationProperties::GetName(content).c_str())
              == "after");

    // Empty is an intentional application value, not permission for wx to
    // reclaim a name it previously supplied.
    AutomationProperties::SetName(content, L"");
    button->SetLabel("must stay empty");
    DrainDispatch(3);
    CHECK(AutomationProperties::GetName(content).empty());

    // ... but an application-set name is never clobbered
    AutomationProperties::SetName(content, L"app-name");
    button->SetLabel("changed again");
    DrainDispatch(3);
    CHECK(wxString(AutomationProperties::GetName(content).c_str())
              == "app-name");

    // the UIA state follows the effective enabled state
    button->Disable();
    DrainDispatch(3);
    const auto peer = FrameworkElementAutomationPeer::CreatePeerForElement(
        content.try_as<MUX::FrameworkElement>());
    REQUIRE(peer != nullptr);
    CHECK(!peer.IsEnabled());

    // the per-window cursor plumbing reaches the slot seam
    const unsigned cursors0 = wxWinUITopLevelHost::GetSlotCursorSetCount();
    button->SetCursor(wxCursor(wxCURSOR_HAND));
    DrainToQuiescence();
    CHECK(wxWinUITopLevelHost::GetSlotCursorSetCount() > cursors0);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::CompositeRootsHaveSemanticAutomationPeers",
          "[HostState][winui-009][uia-topology]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXAP = MUX::Automation::Peers;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "semantic-roots",
                    wxPoint(-32000, -32000), wxSize(520, 360));

    const auto checkRoot =
        [](wxWindow *window,
           MUXAP::AutomationControlType expectedRole,
           const wxString& expectedClass,
           const wxString& expectedName)
        {
            INFO("semantic root " << expectedClass);
            wxWinUITopLevelHost * const host =
                wxWinUITopLevelHost::FindSlotOwner(window);
            REQUIRE(host != nullptr);
            host->FlushSync();
            wxWinUISlot * const slot = host->FindSlot(window);
            REQUIRE(slot != nullptr);
            const MUX::FrameworkElement root =
                slot->GetContent().try_as<MUX::FrameworkElement>();
            REQUIRE(root != nullptr);
            const MUXAP::AutomationPeer peer =
                MUXAP::FrameworkElementAutomationPeer::
                    CreatePeerForElement(root);
            REQUIRE(peer != nullptr);
            CHECK(peer.GetAutomationControlType() == expectedRole);
            CHECK(wxString(peer.GetClassName().c_str()) ==
                  expectedClass);
            CHECK(wxString(peer.GetName().c_str()) == expectedName);
        };

#if wxUSE_STATBOX
    wxStaticBox * const staticBox =
        new wxStaticBox(frame, wxID_ANY, "&Options",
                        wxPoint(8, 8), wxSize(220, 90));
#endif
#if wxUSE_STATUSBAR
    wxStatusBar * const statusBar =
        new wxStatusBar(frame, wxID_ANY, wxSTB_DEFAULT_STYLE,
                        "statusbar-technical-id");
    statusBar->SetLabel("Main status");
    CHECK(statusBar->GetName() == "statusbar-technical-id");
    statusBar->SetSize(0, 300, 500, 32);
    statusBar->SetStatusText("Ready");
#endif
#if wxUSE_TOOLBAR
    wxToolBar * const toolBar =
        new wxToolBar(frame, wxID_ANY, wxPoint(240, 8),
                      wxSize(250, 64), wxTB_HORIZONTAL,
                      "toolbar-technical-id");
    toolBar->SetLabel("Main commands");
    CHECK(toolBar->GetName() == "toolbar-technical-id");
    toolBar->AddTool(wxID_ANY, "Command", wxNullBitmap);
    REQUIRE(toolBar->Realize());
#endif
#if wxUSE_RADIOBOX
    const wxString choices[] = { "One", "Two" };
    wxRadioBox * const radioBox =
        new wxRadioBox(frame, wxID_ANY, "&Numbers",
                       wxPoint(8, 110), wxSize(220, 100),
                       WXSIZEOF(choices), choices);
#endif

    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();

#if wxUSE_STATBOX
    checkRoot(staticBox, MUXAP::AutomationControlType::Group,
              "wxStaticBox", "Options");
#endif
#if wxUSE_STATUSBAR
    checkRoot(statusBar, MUXAP::AutomationControlType::StatusBar,
              "wxStatusBar", "Main status");
#endif
#if wxUSE_TOOLBAR
    checkRoot(toolBar, MUXAP::AutomationControlType::ToolBar,
              "wxToolBar", "Main commands");
#endif
#if wxUSE_RADIOBOX
    checkRoot(radioBox, MUXAP::AutomationControlType::Group,
              "wxRadioBox", "Numbers");
#endif

    // Label-bearing composites replace their visual tree when the label
    // changes. Re-resolve the current slot content and prove the replacement
    // still creates the same semantic peer with its refreshed accessible
    // name; a retained peer from the old root would be the wrong oracle.
#if wxUSE_STATBOX
    staticBox->SetLabel("&Advanced options");
#endif
#if wxUSE_RADIOBOX
    radioBox->SetLabel("&Digits");
#endif
    DrainToQuiescence();
#if wxUSE_STATBOX
    checkRoot(staticBox, MUXAP::AutomationControlType::Group,
              "wxStaticBox", "Advanced options");
#endif
#if wxUSE_RADIOBOX
    checkRoot(radioBox, MUXAP::AutomationControlType::Group,
              "wxRadioBox", "Digits");
#endif

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::StaticTextAndVerticalGaugeUseSemanticTargets",
          "[HostState][winui-009][uia-topology][semantic-target]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = MUX::Automation;
    namespace MUXAP = MUX::Automation::Peers;
    namespace MUXC = MUX::Controls;

#if wxUSE_STATTEXT || wxUSE_GAUGE
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "semantic-descendants",
                    wxPoint(-32000, -32000), wxSize(420, 220));
#endif

#if wxUSE_STATTEXT
    wxStaticText * const label =
        new wxStaticText(frame, wxID_ANY, "&Semantic label",
                         wxPoint(8, 8), wxSize(180, 40));
    // Tooltip text is the HelpText fallback when no application help
    // provider is installed (wxWindowBase::SetHelpText() deliberately stores
    // nothing in that configuration).
    label->SetToolTip("semantic help");
#endif
#if wxUSE_GAUGE
    wxGauge * const gauge =
        new wxGauge(frame, wxID_ANY, 100,
                    wxPoint(220, 8), wxSize(40, 150),
                    wxGA_VERTICAL);
#endif

#if wxUSE_STATTEXT || wxUSE_GAUGE
    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();
#endif

#if wxUSE_STATTEXT
    wxWinUITopLevelHost * const labelHost =
        wxWinUITopLevelHost::FindSlotOwner(label);
    REQUIRE(labelHost != nullptr);
    labelHost->FlushSync();
    wxWinUISlot * const labelSlot = labelHost->FindSlot(label);
    REQUIRE(labelSlot != nullptr);
    const MUXC::Border labelRoot =
        labelSlot->GetVisualRoot().try_as<MUXC::Border>();
    const MUXC::TextBlock labelTarget =
        labelSlot->GetSemanticTarget().try_as<MUXC::TextBlock>();
    REQUIRE(labelRoot != nullptr);
    REQUIRE(labelTarget != nullptr);
    CHECK(labelSlot->GetContent() == labelRoot);
    CHECK(labelRoot.Child() == labelTarget);
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(labelRoot) ==
          MUXAP::AccessibilityView::Raw);
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(labelTarget) ==
          MUXAP::AccessibilityView::Control);
    CHECK(MUXA::AutomationProperties::GetName(labelRoot).empty());
    CHECK(wxString(
              MUXA::AutomationProperties::GetName(labelTarget).c_str()) ==
          "Semantic label");
    CHECK(wxString(
              MUXA::AutomationProperties::GetHelpText(labelTarget).c_str()) ==
          "semantic help");

    const MUXAP::AutomationPeer labelPeer =
        MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(
            labelTarget);
    REQUIRE(labelPeer != nullptr);
    CHECK(labelPeer.GetAutomationControlType() ==
          MUXAP::AutomationControlType::Text);
    CHECK(wxString(labelPeer.GetName().c_str()) == "Semantic label");

    label->SetLabel("&Renamed semantic label");
    labelHost->FlushSync();
    CHECK(wxString(
              MUXA::AutomationProperties::GetName(labelTarget).c_str()) ==
          "Renamed semantic label");
    CHECK(MUXA::AutomationProperties::GetName(labelRoot).empty());
#endif

#if wxUSE_GAUGE
    wxWinUITopLevelHost * const gaugeHost =
        wxWinUITopLevelHost::FindSlotOwner(gauge);
    REQUIRE(gaugeHost != nullptr);
    gaugeHost->FlushSync();
    wxWinUISlot * const gaugeSlot = gaugeHost->FindSlot(gauge);
    REQUIRE(gaugeSlot != nullptr);
    const MUXC::Grid gaugeRoot =
        gaugeSlot->GetVisualRoot().try_as<MUXC::Grid>();
    const MUXC::ProgressBar gaugeTarget =
        gaugeSlot->GetSemanticTarget().try_as<MUXC::ProgressBar>();
    REQUIRE(gaugeRoot != nullptr);
    REQUIRE(gaugeTarget != nullptr);
    CHECK(gaugeSlot->GetContent() == gaugeRoot);
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(gaugeRoot) ==
          MUXAP::AccessibilityView::Raw);
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(gaugeTarget) !=
          MUXAP::AccessibilityView::Raw);

    const MUXAP::AutomationPeer gaugePeer =
        MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(
            gaugeTarget);
    REQUIRE(gaugePeer != nullptr);
    CHECK(gaugePeer.GetAutomationControlType() ==
          MUXAP::AutomationControlType::ProgressBar);
    gauge->Disable();
    gaugeHost->FlushSync();
    CHECK_FALSE(gaugePeer.IsEnabled());
#endif

#if wxUSE_STATTEXT || wxUSE_GAUGE
    frame->Destroy();
    DrainDispatch();
#endif
}

TEST_CASE("HostState::SemanticTargetIsTransactionalWithVisualRoot",
          "[HostState][winui-009][uia-topology][semantic-target]"
          "[transaction]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = MUX::Automation;
    namespace MUXAP = MUX::Automation::Peers;
    namespace MUXC = MUX::Controls;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "semantic-transaction",
                    wxPoint(-32000, -32000), wxSize(320, 180));
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    xamlHost->SetLabel("semantic pair");
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);

    const MUXC::Grid root;
    const MUXC::TextBlock targetA;
    const MUXC::Button targetB;
    root.Children().Append(targetA);
    root.Children().Append(targetB);

    REQUIRE(slot->SetContent(root, targetA));
    host->FlushSync();
    CHECK(slot->GetVisualRoot() == root);
    CHECK(slot->GetSemanticTarget() == targetA);
    CHECK(MUXA::AutomationProperties::GetName(root).empty());
    CHECK(wxString(
              MUXA::AutomationProperties::GetName(targetA).c_str()) ==
          "semantic pair");
    CHECK(MUXA::AutomationProperties::GetName(targetB).empty());

    // A target outside the visual subtree is rejected before any carrier or
    // ownership mutation.
    const MUXC::Button foreignTarget;
    CHECK_FALSE(slot->SetContent(root, foreignTarget));
    CHECK(slot->GetVisualRoot() == root);
    CHECK(slot->GetSemanticTarget() == targetA);

    // Changing only the semantic descendant is still a full content
    // transaction: the former target gets its application baseline back.
    REQUIRE(slot->SetContent(root, targetB));
    host->FlushSync();
    CHECK(slot->GetVisualRoot() == root);
    CHECK(slot->GetSemanticTarget() == targetB);
    CHECK(MUXA::AutomationProperties::GetName(targetA).empty());
    CHECK(wxString(
              MUXA::AutomationProperties::GetName(targetB).c_str()) ==
          "semantic pair");

    // A failed same-root target change restores the exact old pair and does
    // not leak host-owned UIA values onto the candidate.
    wxWinUITopLevelHost::TestFailContentSwap(
        wxWinUITopLevelHost::TestContentFault_Install);
    CHECK_FALSE(slot->SetContent(root, targetA));
    CHECK(slot->GetVisualRoot() == root);
    CHECK(slot->GetSemanticTarget() == targetB);
    CHECK(MUXA::AutomationProperties::GetName(targetA).empty());
    CHECK(wxString(
              MUXA::AutomationProperties::GetName(targetB).c_str()) ==
          "semantic pair");

    // Content() is a re-entrant XAML boundary. If Loaded/application code
    // removes the requested semantic descendant after the carrier accepts
    // the root, the pair must be rejected and the exact previous generation
    // restored instead of publishing UIA state onto a detached element.
    const MUXC::Grid detachedCandidateRoot;
    const MUXC::TextBlock detachedCandidateTarget;
    detachedCandidateRoot.Children().Append(detachedCandidateTarget);
    bool detachSeamRan = false;
    wxWinUITopLevelHost::TestOnNextContentCarrierSet(
        [&](wxWindow *window)
        {
            REQUIRE(window == xamlHost);
            detachSeamRan = true;
            detachedCandidateRoot.Children().Clear();
        });
    CHECK_FALSE(
        slot->SetContent(
            detachedCandidateRoot, detachedCandidateTarget));
    wxWinUITopLevelHost::TestOnNextContentCarrierSet({});
    CHECK(detachSeamRan);
    CHECK(slot->GetVisualRoot() == root);
    CHECK(slot->GetSemanticTarget() == targetB);
    winrt::Microsoft::UI::Xaml::UIElement carrierContent{ nullptr };
    REQUIRE(slot->TryGetCarrierContent(carrierContent));
    CHECK(carrierContent == root);
    CHECK(MUXA::AutomationProperties::GetName(
              detachedCandidateTarget).empty());
    host->FlushSync();
    CHECK(wxString(
              MUXA::AutomationProperties::GetName(targetB).c_str()) ==
          "semantic pair");

    // The same callback can mutate the detached OLD tree too. Rollback keeps
    // its visual root but must demote the now-detached semantic descendant,
    // including moving the target-specific Style/Name observer to the root.
    const MUXC::Grid fallbackCandidateRoot;
    const MUXC::TextBlock fallbackCandidateTarget;
    fallbackCandidateRoot.Children().Append(fallbackCandidateTarget);
    bool fallbackSeamRan = false;
    wxWinUITopLevelHost::TestOnNextContentCarrierSet(
        [&](wxWindow *window)
        {
            REQUIRE(window == xamlHost);
            fallbackSeamRan = true;
            fallbackCandidateRoot.Children().Clear();
            root.Children().Clear();
        });
    CHECK_FALSE(
        slot->SetContent(
            fallbackCandidateRoot, fallbackCandidateTarget));
    wxWinUITopLevelHost::TestOnNextContentCarrierSet({});
    CHECK(fallbackSeamRan);
    CHECK(slot->GetVisualRoot() == root);
    CHECK(slot->GetSemanticTarget() == root);
    CHECK(slot->GetAutomationNameStyleObservedContentForTest() == root);
    REQUIRE(slot->TryGetCarrierContent(carrierContent));
    CHECK(carrierContent == root);
    CHECK(MUXA::AutomationProperties::GetName(targetB).empty());
    CHECK(MUXA::AutomationProperties::GetName(
              fallbackCandidateTarget).empty());
    host->FlushSync();
    CHECK(wxString(
              MUXA::AutomationProperties::GetName(root).c_str()) ==
          "semantic pair");

#if wxUSE_TOOLTIPS
    // Post-commit adapter work is re-entrant too. Global tooltip suppression
    // writes the visual root after observer registration; an application
    // callback from that exact property setter detaches the semantic child.
    // SetContent must report loss of the requested pair and leave a repaired
    // root/root generation, never a structurally false published slot.
    struct ToolTipPolicyRestorer
    {
        ~ToolTipPolicyRestorer() { wxToolTip::Enable(true); }
    } toolTipPolicyRestorer;
    wxToolTip::Enable(false);
    const MUXC::Grid postCommitRoot;
    const MUXC::TextBlock postCommitTarget;
    postCommitRoot.Children().Append(postCommitTarget);
    MUXC::ToolTip postCommitToolTip;
    postCommitToolTip.Content(
        winrt::box_value(L"semantic-post-commit"));
    MUXC::ToolTipService::SetToolTip(
        postCommitRoot, postCommitToolTip);
    bool postCommitCallbackRan = false;
    const int64_t postCommitToken =
        postCommitRoot.RegisterPropertyChangedCallback(
            MUXC::ToolTipService::ToolTipProperty(),
            [&](const MUX::DependencyObject&,
                const MUX::DependencyProperty&)
            {
                if ( postCommitCallbackRan ||
                     MUXC::ToolTipService::GetToolTip(postCommitRoot) )
                {
                    return;
                }
                postCommitCallbackRan = true;
                postCommitRoot.Children().Clear();
            });
    CHECK_FALSE(
        slot->SetContent(postCommitRoot, postCommitTarget));
    postCommitRoot.UnregisterPropertyChangedCallback(
        MUXC::ToolTipService::ToolTipProperty(), postCommitToken);
    REQUIRE(postCommitCallbackRan);
    CHECK(slot->GetVisualRoot() == postCommitRoot);
    CHECK(slot->GetSemanticTarget() == postCommitRoot);
    CHECK(slot->GetAutomationNameStyleObservedContentForTest() ==
          postCommitRoot);
    REQUIRE(slot->TryGetCarrierContent(carrierContent));
    CHECK(carrierContent == postCommitRoot);
    CHECK(MUXA::AutomationProperties::GetName(
              postCommitTarget).empty());
    wxToolTip::Enable(true);
    host->FlushSync();
    CHECK(wxString(
              MUXA::AutomationProperties::GetName(postCommitRoot).c_str()) ==
          "semantic pair");
#endif // wxUSE_TOOLTIPS

#if wxUSE_ACCESSIBILITY
    // The rollback path has the same post-publication boundaries. Restoring
    // native accessibility authority writes Raw to the old semantic target;
    // an application callback from that setter detaches it. The failed swap
    // must still finish with a valid root/root generation.
    const MUXC::Grid rollbackBoundaryRoot;
    const MUXC::TextBlock rollbackBoundaryTarget;
    rollbackBoundaryRoot.Children().Append(rollbackBoundaryTarget);
    REQUIRE(
        slot->SetContent(
            rollbackBoundaryRoot, rollbackBoundaryTarget));
    host->FlushSync();
    new wxWindowAccessible(xamlHost);
    REQUIRE(
        MUXA::AutomationProperties::GetAccessibilityView(
            rollbackBoundaryTarget) ==
        MUXAP::AccessibilityView::Raw);

    bool rollbackBoundaryCallbackRan = false;
    const auto accessibilityViewProperty =
        MUXA::AutomationProperties::AccessibilityViewProperty();
    const int64_t rollbackBoundaryToken =
        rollbackBoundaryTarget.RegisterPropertyChangedCallback(
            accessibilityViewProperty,
            [&](const MUX::DependencyObject&,
                const MUX::DependencyProperty&)
            {
                if ( rollbackBoundaryCallbackRan ||
                     MUXA::AutomationProperties::GetAccessibilityView(
                         rollbackBoundaryTarget) !=
                         MUXAP::AccessibilityView::Raw )
                {
                    return;
                }
                rollbackBoundaryCallbackRan = true;
                rollbackBoundaryRoot.Children().Clear();
            });
    const MUXC::Border failedBoundaryReplacement;
    wxWinUITopLevelHost::TestFailContentSwap(
        wxWinUITopLevelHost::TestContentFault_Install);
    CHECK_FALSE(slot->SetContent(failedBoundaryReplacement));
    rollbackBoundaryTarget.UnregisterPropertyChangedCallback(
        accessibilityViewProperty, rollbackBoundaryToken);
    REQUIRE(rollbackBoundaryCallbackRan);
    CHECK(slot->GetVisualRoot() == rollbackBoundaryRoot);
    CHECK(slot->GetSemanticTarget() == rollbackBoundaryRoot);
    CHECK(slot->GetAutomationNameStyleObservedContentForTest() ==
          rollbackBoundaryRoot);
    REQUIRE(slot->TryGetCarrierContent(carrierContent));
    CHECK(carrierContent == rollbackBoundaryRoot);
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(
              rollbackBoundaryTarget) !=
          MUXAP::AccessibilityView::Raw);
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(
              rollbackBoundaryRoot) ==
          MUXAP::AccessibilityView::Raw);
    host->FlushSync();
    CHECK(wxString(
              MUXA::AutomationProperties::GetName(
                  rollbackBoundaryRoot).c_str()) ==
          "semantic pair");
    xamlHost->SetAccessible(nullptr);
#endif // wxUSE_ACCESSIBILITY

    host->UnregisterSlot(xamlHost);
    CHECK(MUXA::AutomationProperties::GetName(targetA).empty());
    CHECK(MUXA::AutomationProperties::GetName(targetB).empty());
    CHECK(MUXA::AutomationProperties::GetName(root).empty());

    // The same invariant applies to a brand-new slot: Append() makes it
    // discoverable before firing Loaded, so a callback can mutate the visual
    // subtree while registration is still provisional. Rollback must remove
    // both the slot and its global ownership entry.
    wxWindow * const freshProbe =
        new wxWindow(frame, wxID_ANY, wxPoint(10, 90), wxSize(100, 30));
    const MUXC::Grid freshRoot;
    const MUXC::TextBlock freshTarget;
    freshRoot.Children().Append(freshTarget);
    bool freshDetachSeamRan = false;
    wxWinUITopLevelHost::TestOnNextSlotAttached(
        [&](wxWindow *window)
        {
            if ( window != freshProbe )
                return;

            freshDetachSeamRan = true;
            freshRoot.Children().Clear();
        });
    CHECK(host->RegisterSlot(freshProbe, freshRoot, freshTarget) == nullptr);
    wxWinUITopLevelHost::TestOnNextSlotAttached({});
    CHECK(freshDetachSeamRan);
    CHECK(host->FindSlot(freshProbe) == nullptr);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(freshProbe) == nullptr);
    CHECK(MUXA::AutomationProperties::GetName(freshTarget).empty());

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::MigrationDemotesDetachedSemanticTarget",
          "[HostState][HostLifecycle][winui-009][uia-topology]"
          "[semantic-target][transaction][migration]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = MUX::Automation;
    namespace MUXC = MUX::Controls;

    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "semantic-migration-A",
                    wxPoint(-32000, -32000), wxSize(260, 160));
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "semantic-migration-B",
                    wxPoint(-31700, -32000), wxSize(260, 160));
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frameA);
    xamlHost->SetLabel("semantic migrant");
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    ShowOffscreenWithoutActivating(frameA, 0);
    ShowOffscreenWithoutActivating(frameB, 1);
    DrainToQuiescence();

    wxWinUITopLevelHost * const hostA =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(hostA != nullptr);
    wxWinUISlot * const sourceSlot = hostA->FindSlot(xamlHost);
    REQUIRE(sourceSlot != nullptr);

    const MUXC::Grid root;
    const MUXC::TextBlock target;
    root.Children().Append(target);
    REQUIRE(sourceSlot->SetContent(root, target));
    hostA->FlushSync();
    REQUIRE(MUXA::AutomationProperties::GetName(target) ==
            L"semantic migrant");

    // Application ownership/relinquishment belongs to the exact semantic
    // element. It must not suppress the wx name after that element is
    // detached and the visual root becomes the replacement target.
    target.ClearValue(
        MUXA::AutomationProperties::NameProperty());
    hostA->FlushSync();
    REQUIRE(MUXA::AutomationProperties::GetName(target).empty());

    bool detachSeamRan = false;
    wxWinUITopLevelHost::TestOnNextContentCarrierSet(
        [&](wxWindow *window)
        {
            REQUIRE(window == xamlHost);
            detachSeamRan = true;
            root.Children().Clear();
        });
    REQUIRE(xamlHost->Reparent(frameB));
    wxWinUITopLevelHost::TestOnNextContentCarrierSet({});
    REQUIRE(detachSeamRan);

    wxWinUITopLevelHost * const hostB =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(hostB != nullptr);
    REQUIRE(hostB != hostA);
    CHECK(hostA->FindSlot(xamlHost) == nullptr);
    wxWinUISlot * const destinationSlot = hostB->FindSlot(xamlHost);
    REQUIRE(destinationSlot != nullptr);
    CHECK(destinationSlot->GetVisualRoot() == root);
    CHECK(destinationSlot->GetSemanticTarget() == root);
    CHECK(destinationSlot->GetAutomationNameStyleObservedContentForTest() ==
          root);
    winrt::Microsoft::UI::Xaml::UIElement carrierContent{ nullptr };
    REQUIRE(destinationSlot->TryGetCarrierContent(carrierContent));
    CHECK(carrierContent == root);
    CHECK(MUXA::AutomationProperties::GetName(target).empty());
    hostB->FlushSync();
    CHECK(MUXA::AutomationProperties::GetName(root) ==
          L"semantic migrant");

    frameA->Destroy();
    frameB->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::FailedMigrationRestoresDemotedSemanticTarget",
          "[HostState][HostLifecycle][winui-009][uia-topology]"
          "[semantic-target][transaction][migration]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = MUX::Automation;
    namespace MUXC = MUX::Controls;

    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "semantic-restore-A",
                    wxPoint(-32000, -32000), wxSize(260, 160));
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "semantic-restore-B",
                    wxPoint(-31700, -32000), wxSize(260, 160));
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frameA);
    xamlHost->SetLabel("semantic restore");
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    ShowOffscreenWithoutActivating(frameA, 0);
    ShowOffscreenWithoutActivating(frameB, 1);
    DrainToQuiescence();

    wxWinUITopLevelHost * const hostA =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(hostA != nullptr);
    wxWinUISlot * const sourceSlot = hostA->FindSlot(xamlHost);
    REQUIRE(sourceSlot != nullptr);

    const MUXC::Grid root;
    const MUXC::TextBlock target;
    root.Children().Append(target);
    REQUIRE(sourceSlot->SetContent(root, target));
    hostA->FlushSync();

    bool detachSeamRan = false;
    bool destinationSeamRan = false;
    wxWinUITopLevelHost::TestOnNextContentCarrierSet(
        [&](wxWindow *window)
        {
            REQUIRE(window == xamlHost);
            detachSeamRan = true;
            root.Children().Clear();
        });
    wxWinUITopLevelHost::TestOnNextSlotAttached(
        [&](wxWindow *window)
        {
            if ( window != xamlHost )
                return;
            destinationSeamRan = true;
            wxWinUITopLevelHost * const provisionalOwner =
                wxWinUITopLevelHost::FindSlotOwner(window);
            REQUIRE(provisionalOwner != nullptr);
            provisionalOwner->UnregisterSlot(window);
        });
    REQUIRE(xamlHost->Reparent(frameB));
    wxWinUITopLevelHost::TestOnNextContentCarrierSet({});
    wxWinUITopLevelHost::TestOnNextSlotAttached({});
    REQUIRE(detachSeamRan);
    REQUIRE(destinationSeamRan);

    // The native parent changed, but the failed host transaction remains
    // immediately recoverable in A. Its queued dirty retry may migrate the
    // now-valid root/root pair later; no empty or duplicate slot exists now.
    CHECK(wxWinUITopLevelHost::FindSlotOwner(xamlHost) == hostA);
    wxWinUISlot * const restoredSlot = hostA->FindSlot(xamlHost);
    REQUIRE(restoredSlot != nullptr);
    CHECK(restoredSlot->GetVisualRoot() == root);
    CHECK(restoredSlot->GetSemanticTarget() == root);
    CHECK(restoredSlot->GetAutomationNameStyleObservedContentForTest() ==
          root);
    winrt::Microsoft::UI::Xaml::UIElement carrierContent{ nullptr };
    REQUIRE(restoredSlot->TryGetCarrierContent(carrierContent));
    CHECK(carrierContent == root);
    CHECK(MUXA::AutomationProperties::GetName(target).empty());

    // The normal dirty pass retries the migration after the one-shot failure.
    // It must converge without losing the restored root/root generation.
    DrainToQuiescence();
    wxWinUITopLevelHost * const convergedOwner =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(convergedOwner != nullptr);
    REQUIRE(convergedOwner != hostA);
    CHECK(hostA->FindSlot(xamlHost) == nullptr);
    wxWinUISlot * const convergedSlot =
        convergedOwner->FindSlot(xamlHost);
    REQUIRE(convergedSlot != nullptr);
    CHECK(convergedSlot->GetVisualRoot() == root);
    CHECK(convergedSlot->GetSemanticTarget() == root);
    convergedOwner->FlushSync();
    CHECK(MUXA::AutomationProperties::GetName(root) ==
          L"semantic restore");

    frameA->Destroy();
    frameB->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::AccessibilityShellIsRawOnly",
          "[HostState][winui-dialog-uia]")
{
    namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
    namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    constexpr LPARAM UiaRootObjectIdForTest = -25;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "uia-shell-owner",
                    wxPoint(-32000, -32000), wxSize(360, 220));
    wxPanel * const nativePanel = new wxPanel(frame, wxID_ANY);
    wxButton * const button =
        new wxButton(nativePanel, wxID_ANY, "one accessible button");
    frame->ShowWithoutActivating();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(button);
    REQUIRE(slot != nullptr);
    const auto content = slot->GetContent();
    REQUIRE(content != nullptr);

    CHECK(wxWinUITLWHostIsInvisibleAccessibilityShell(button));
    CHECK_FALSE(wxWinUITLWHostIsInvisibleAccessibilityShell(nativePanel));
    CHECK_FALSE(wxWinUITLWHostIsInvisibleAccessibilityShell(frame));

    const HWND buttonHwnd = static_cast<HWND>(button->GetHWND());
    REQUIRE(buttonHwnd != nullptr);
    const unsigned shellRetiresBefore =
        wxWinUITestGetAccessibilityShellProviderRetireCount();

    // With no wxAccessible the XAML peer is authoritative. An application
    // authored view remains untouched, OBJID_CLIENT is suppressed, and the
    // covered HWND exposes only our explicit server-side Raw provider.
    MUXA::AutomationProperties::SetAccessibilityView(
        content, MUXAP::AccessibilityView::Control);
    host->FlushSync();
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(content) ==
          MUXAP::AccessibilityView::Control);
    CHECK(::SendMessage(
              buttonHwnd, WM_GETOBJECT, 0,
              static_cast<LPARAM>(OBJID_CLIENT)) == 0);
    CHECK(::SendMessage(
              buttonHwnd, WM_GETOBJECT, 0,
              UiaRootObjectIdForTest) != 0);

#if wxUSE_ACCESSIBILITY
    // Installing a native accessible flips authority synchronously: MSAA is
    // available and XAML is Raw before SetAccessible() returns.
    new wxWindowAccessible(button);
    CHECK(wxWinUITestGetAccessibilityShellProviderRetireCount() ==
          shellRetiresBefore + 1);
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(content) ==
          MUXAP::AccessibilityView::Raw);
    CHECK(::SendMessage(
              buttonHwnd, WM_GETOBJECT, 0,
              static_cast<LPARAM>(OBJID_CLIENT)) != 0);

    // Removing and reapplying authority restores the exact authored XAML
    // value, then hides it again without waiting for a dispatch turn.
    button->SetAccessible(nullptr);
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(content) ==
          MUXAP::AccessibilityView::Control);
    CHECK(::SendMessage(
              buttonHwnd, WM_GETOBJECT, 0,
              static_cast<LPARAM>(OBJID_CLIENT)) == 0);
    CHECK(::SendMessage(
              buttonHwnd, WM_GETOBJECT, 0,
              UiaRootObjectIdForTest) != 0);

    new wxWindowAccessible(button);
    CHECK(wxWinUITestGetAccessibilityShellProviderRetireCount() ==
          shellRetiresBefore + 2);
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(content) ==
          MUXAP::AccessibilityView::Raw);

    // If the application authors a newer value while the native tree owns
    // visibility, a later sync keeps the effective peer Raw but remembers
    // that exact local value for the next authority removal.
    MUXA::AutomationProperties::SetAccessibilityView(
        content, MUXAP::AccessibilityView::Content);
    wxWinUITLWHostNotifySlotState(button);
    host->FlushSync();
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(content) ==
          MUXAP::AccessibilityView::Raw);
    button->SetAccessible(nullptr);
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(content) ==
          MUXAP::AccessibilityView::Content);

    MUXA::AutomationProperties::SetAccessibilityView(
        content, MUXAP::AccessibilityView::Control);
    new wxWindowAccessible(button);
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(content) ==
          MUXAP::AccessibilityView::Raw);

    // Initial registration must query a lazy CreateAccessible() too; no
    // WM_GETOBJECT round-trip is needed to reach the singular native state.
    LazyAccessibleButton * const lazy = new LazyAccessibleButton;
    REQUIRE(lazy->Create(nativePanel, wxID_ANY, "lazy accessible"));
    REQUIRE(lazy->GetCreateAccessibleCalls() == 1);
    wxWinUITopLevelHost * const lazyHost =
        wxWinUITopLevelHost::FindSlotOwner(lazy);
    REQUIRE(lazyHost != nullptr);
    wxWinUISlot * const lazySlot = lazyHost->FindSlot(lazy);
    REQUIRE(lazySlot != nullptr);
    REQUIRE(lazySlot->GetContent() != nullptr);
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(
              lazySlot->GetContent()) == MUXAP::AccessibilityView::Raw);
    CHECK(::SendMessage(
              static_cast<HWND>(lazy->GetHWND()), WM_GETOBJECT, 0,
              static_cast<LPARAM>(OBJID_CLIENT)) != 0);

    // A content replacement restores the old element before detaching it,
    // captures the replacement's own authored value, and restores that value
    // when native authority is later removed.
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(nativePanel);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    wxWinUITopLevelHost * const xamlOwner =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(xamlOwner != nullptr);
    wxWinUISlot * const xamlSlot = xamlOwner->FindSlot(xamlHost);
    REQUIRE(xamlSlot != nullptr);
    const auto oldContent = xamlSlot->GetContent();
    REQUIRE(oldContent != nullptr);
    MUXA::AutomationProperties::SetAccessibilityView(
        oldContent, MUXAP::AccessibilityView::Control);

    new wxWindowAccessible(xamlHost);
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(oldContent) ==
          MUXAP::AccessibilityView::Raw);

    const MUXC::Border replacement;
    const auto accessibilityViewProperty =
        MUXA::AutomationProperties::AccessibilityViewProperty();
    REQUIRE(replacement.ReadLocalValue(accessibilityViewProperty) ==
            winrt::Microsoft::UI::Xaml::DependencyProperty::UnsetValue());
    REQUIRE(xamlSlot->SetContent(replacement));
    xamlOwner->FlushSync();
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(oldContent) ==
          MUXAP::AccessibilityView::Control);
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(replacement) ==
          MUXAP::AccessibilityView::Raw);

    xamlHost->SetAccessible(nullptr);
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(replacement) ==
          MUXAP::AccessibilityView::Content);
    CHECK(replacement.ReadLocalValue(accessibilityViewProperty) ==
          winrt::Microsoft::UI::Xaml::DependencyProperty::UnsetValue());
#endif

    // Disconnect also restores a host-owned Raw value on the content object
    // that outlives the slot.
    host->UnregisterSlot(button);
    CHECK_FALSE(wxWinUITLWHostIsInvisibleAccessibilityShell(button));
#if wxUSE_ACCESSIBILITY
    CHECK(MUXA::AutomationProperties::GetAccessibilityView(content) ==
          MUXAP::AccessibilityView::Control);
#endif

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::RetainedAccessibilityShellProviderExpiresOnContentSwap",
          "[HostState][winui-009][uia-topology][generation]")
{
    constexpr LPARAM UiaRootObjectIdForTest = -25;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "uia-shell-content-generation",
                    wxPoint(-32000, -32000), wxSize(300, 180));
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));

    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto oldContent = slot->GetContent();
    REQUIRE(oldContent != nullptr);
    const WXHWND hwnd = xamlHost->GetHWND();
    REQUIRE(hwnd != nullptr);
    const unsigned shellRetiresBefore =
        wxWinUITestGetAccessibilityShellProviderRetireCount();

    // Prime the real USER32/UIA provider cache for generation A. The direct
    // COM seam below intentionally does not touch that cache.
    REQUIRE(::SendMessage(
                static_cast<HWND>(hwnd), WM_GETOBJECT, 0,
                UiaRootObjectIdForTest) != 0);

    winrt::com_ptr<IRawElementProviderSimple> retained;
    REQUIRE(wxWinUITestCreateInvisibleShellProvider(
                xamlHost, retained.put()) == S_OK);

    ProviderOptions providerOptions{};
    REQUIRE(retained->get_ProviderOptions(&providerOptions) == S_OK);
    CHECK((providerOptions & ProviderOptions_ServerSideProvider) != 0);
    CHECK((providerOptions & ProviderOptions_UseComThreading) != 0);

    VARIANT value;
    ::VariantInit(&value);
    REQUIRE(retained->GetPropertyValue(
                UIA_IsControlElementPropertyId, &value) == S_OK);
    CHECK(value.vt == VT_BOOL);
    CHECK(value.boolVal == VARIANT_FALSE);
    ::VariantClear(&value);

    winrt::com_ptr<IRawElementProviderSimple> retainedHostProvider;
    REQUIRE(retained->get_HostRawElementProvider(
                retainedHostProvider.put()) == S_OK);
    REQUIRE(retainedHostProvider != nullptr);

    // Keep the same logical slot, HWND and accessibility authority while
    // replacing only its XAML content. The retained provider must represent
    // the old content generation, not silently retarget to the new peer.
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"replacement\"/>"));
    REQUIRE(wxWinUITopLevelHost::FindSlotOwner(xamlHost) == host);
    REQUIRE(host->FindSlot(xamlHost) == slot);
    REQUIRE(xamlHost->GetHWND() == hwnd);
    REQUIRE(slot->GetContent() != nullptr);
    REQUIRE(slot->GetContent() != oldContent);
    CHECK(wxWinUITestGetAccessibilityShellProviderRetireCount() ==
          shellRetiresBefore + 1);

    ::VariantInit(&value);
    CHECK(retained->GetPropertyValue(
              UIA_IsControlElementPropertyId, &value) ==
          UIA_E_ELEMENTNOTAVAILABLE);
    CHECK(value.vt == VT_EMPTY);
    ::VariantClear(&value);

    IRawElementProviderSimple *expiredHostProvider =
        reinterpret_cast<IRawElementProviderSimple *>(1);
    CHECK(retained->get_HostRawElementProvider(
              &expiredHostProvider) == UIA_E_ELEMENTNOTAVAILABLE);
    CHECK(expiredHostProvider == nullptr);

    // A newly requested provider is bound to the replacement generation and
    // remains usable, proving the invalidation is generation-specific.
    winrt::com_ptr<IRawElementProviderSimple> current;
    REQUIRE(wxWinUITestCreateInvisibleShellProvider(
                xamlHost, current.put()) == S_OK);
    ::VariantInit(&value);
    CHECK(current->GetPropertyValue(
              UIA_IsControlElementPropertyId, &value) == S_OK);
    CHECK(value.vt == VT_BOOL);
    CHECK(value.boolVal == VARIANT_FALSE);
    ::VariantClear(&value);
    winrt::com_ptr<IRawElementProviderSimple> currentHostProvider;
    REQUIRE(current->get_HostRawElementProvider(
                currentHostProvider.put()) == S_OK);
    REQUIRE(currentHostProvider != nullptr);

    // The same HWND can now expose generation B instead of remaining pinned
    // to UIA's cached, unavailable generation-A provider.
    REQUIRE(::SendMessage(
                static_cast<HWND>(hwnd), WM_GETOBJECT, 0,
                UiaRootObjectIdForTest) != 0);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::AccessibilityShellProviderRetiresWithExactHwnd",
          "[HostState][HostLifecycle][winui-009][uia-topology][destroy]")
{
    constexpr LPARAM UiaRootObjectIdForTest = -25;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "uia-shell-retire",
                    wxPoint(-32000, -32000), wxSize(260, 140));
    wxButton * const button =
        new wxButton(frame, wxID_ANY, "provider owner");
    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();

    const HWND hwnd = static_cast<HWND>(button->GetHWND());
    REQUIRE(hwnd != nullptr);
    const unsigned retiresBefore =
        wxWinUITestGetAccessibilityShellProviderRetireCount();

    // Exercise the real WM_GETOBJECT path so USER32/UIA records this exact
    // HWND as having exposed our provider and the destruction marker is set.
    REQUIRE(::SendMessage(
                hwnd, WM_GETOBJECT, 0, UiaRootObjectIdForTest) != 0);

    const wxWeakRef<wxWindow> weakButton(button);
    REQUIRE(button->Destroy());
    REQUIRE(DrainUntil([&]()
    {
        return weakButton.get() == nullptr && !::IsWindow(hwnd);
    }));

    // HandleDestroy() and DoDetachHWND() both call the retirement seam, but
    // its per-HWND marker makes the exact provider identity retire once.
    CHECK(wxWinUITestGetAccessibilityShellProviderRetireCount() ==
          retiresBefore + 1);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::AccessibilityShellCacheRenewsOnReregister",
          "[HostState][HostLifecycle][winui-009][uia-topology]"
          "[generation]")
{
    constexpr LPARAM UiaRootObjectIdForTest = -25;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "uia-shell-reregister",
                    wxPoint(-32000, -32000), wxSize(260, 140));
    wxButton * const button =
        new wxButton(frame, wxID_ANY, "same shell");
    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host != nullptr);
    wxWinUISlot * const oldSlot = host->FindSlot(button);
    REQUIRE(oldSlot != nullptr);
    const auto content = oldSlot->GetContent();
    REQUIRE(content != nullptr);
    const WXHWND hwnd = button->GetHWND();
    REQUIRE(hwnd != nullptr);

    const unsigned retiresBefore =
        wxWinUITestGetAccessibilityShellProviderRetireCount();
    REQUIRE(::SendMessage(
                static_cast<HWND>(hwnd), WM_GETOBJECT, 0,
                UiaRootObjectIdForTest) != 0);
    winrt::com_ptr<IRawElementProviderSimple> retained;
    REQUIRE(wxWinUITestCreateInvisibleShellProvider(
                button, retained.put()) == S_OK);

    // Retire the complete slot identity while preserving both the wxWindow
    // and its exact HWND, then publish a fresh lifetime on that shell.
    host->UnregisterSlot(button);
    CHECK(wxWinUITestGetAccessibilityShellProviderRetireCount() ==
          retiresBefore + 1);
    REQUIRE(wxWinUITopLevelHost::FindSlotOwner(button) == nullptr);
    wxWinUISlot * const newSlot = host->RegisterSlot(button, content);
    REQUIRE(newSlot != nullptr);
    REQUIRE(button->GetHWND() == hwnd);

    VARIANT oldValue;
    ::VariantInit(&oldValue);
    CHECK(retained->GetPropertyValue(
              UIA_IsControlElementPropertyId, &oldValue) ==
          UIA_E_ELEMENTNOTAVAILABLE);
    ::VariantClear(&oldValue);

    REQUIRE(::SendMessage(
                static_cast<HWND>(hwnd), WM_GETOBJECT, 0,
                UiaRootObjectIdForTest) != 0);
    winrt::com_ptr<IRawElementProviderSimple> current;
    REQUIRE(wxWinUITestCreateInvisibleShellProvider(
                button, current.put()) == S_OK);
    VARIANT value;
    ::VariantInit(&value);
    CHECK(current->GetPropertyValue(
              UIA_IsControlElementPropertyId, &value) == S_OK);
    CHECK(value.vt == VT_BOOL);
    CHECK(value.boolVal == VARIANT_FALSE);
    ::VariantClear(&value);

    frame->Destroy();
    DrainDispatch();
}

#if wxUSE_ACCESSIBILITY
TEST_CASE("HostState::AccessibilityMutationDoesNotCreateHost",
          "[HostState][winui-dialog-uia]")
{
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "uia-native-only",
                    wxPoint(-32000, -32000), wxSize(220, 120));
    wxPanel * const panel = new wxPanel(frame);

    REQUIRE(wxWinUITopLevelHost::FindForTLW(frame) == nullptr);
    new wxWindowAccessible(panel);
    CHECK(wxWinUITopLevelHost::FindForTLW(frame) == nullptr);
    panel->SetAccessible(nullptr);
    CHECK(wxWinUITopLevelHost::FindForTLW(frame) == nullptr);

    frame->Destroy();
    DrainDispatch();
}
#endif // wxUSE_ACCESSIBILITY

TEST_CASE("HostState::InheritedCursorState", "[HostState]")
{
    namespace MUI = winrt::Microsoft::UI::Input;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "state-cursor");
    wxPanel * const panel = new wxPanel(frame);
    panel->SetCursor(wxCursor(wxCURSOR_HAND));
    wxButton * const button = new wxButton(panel, wxID_ANY, "child");
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host != nullptr);

    const auto checkCursorShape =
        [](const MUI::InputCursor& cursor,
           MUI::InputSystemCursorShape expected)
        {
            INFO("expected cursor shape " << static_cast<int>(expected));
            REQUIRE(cursor != nullptr);
            const auto systemCursor = cursor.try_as<MUI::InputSystemCursor>();
            REQUIRE(systemCursor != nullptr);
            CHECK(systemCursor.CursorShape() == expected);
        };
    const auto checkShape =
        [&](MUI::InputSystemCursorShape expected)
        {
            checkCursorShape(host->GetSlotCursorForTest(button), expected);
        };
    const auto checkIslandShape =
        [&](MUI::InputSystemCursorShape expected)
        {
            checkCursorShape(host->GetIslandCursorForTest(), expected);
        };

    // The initial sync must discover a cursor set on a native ancestor before
    // the child slot even existed.
    checkShape(MUI::InputSystemCursorShape::Hand);
    REQUIRE(host->TestActivateSlotCursor(button));
    checkIslandShape(MUI::InputSystemCursorShape::Hand);

    // A transient InputPointerSource failure must not claim that the weaker
    // root ProtectedCursor won: a descendant XAML cursor can outrank it. The
    // failed application remains non-authoritative and the next attempt
    // retries/promotes the source-level override.
    wxWinUITopLevelHost::TestFailInputPointerSourceSet(1);
    CHECK_FALSE(host->TestActivateSlotCursor(button));
    CHECK_FALSE(host->IsInputPointerSourceAuthoritativeForTest());
    REQUIRE(host->TestActivateSlotCursor(button));
    CHECK(host->IsInputPointerSourceAuthoritativeForTest());

    int setCursorMode = 1;
    int setCursorEvents = 0;
    bool parentCursorHandlerEnabled = false;
    int parentSetCursorEvents = 0;
    panel->Bind(
        wxEVT_SET_CURSOR,
        [&](wxSetCursorEvent& event)
        {
            if ( parentCursorHandlerEnabled )
            {
                ++parentSetCursorEvents;
                event.SetCursor(wxCursor(wxCURSOR_SIZING));
            }
            else
            {
                event.Skip();
            }
        });
    button->Bind(
        wxEVT_SET_CURSOR,
        [&](wxSetCursorEvent& event)
        {
            ++setCursorEvents;
            if ( setCursorMode == 1 )
                event.SetCursor(wxCursor(wxCURSOR_IBEAM));
            else if ( setCursorMode == 3 )
            {
                // Mutate policy from inside the callback. The nested
                // notification must win over this stale outer verdict.
                setCursorMode = 0;
                button->SetCursor(wxCursor(wxCURSOR_CROSS));
                event.SetCursor(wxCursor(wxCURSOR_IBEAM));
            }
            else if ( setCursorMode == 0 )
                event.Skip();
            // mode 2 intentionally handles the event without a cursor:
            // native MSW then skips this window's m_cursor but lets its
            // parent/global cursor answer.
        });
    RECT cursorButtonRect;
    REQUIRE(::GetWindowRect(
                GetHwndOf(button), &cursorButtonRect) != 0);
    const wxPoint cursorButtonPoint(
        (cursorButtonRect.left + cursorButtonRect.right) / 2,
        (cursorButtonRect.top + cursorButtonRect.bottom) / 2);

    // One routed Move selects a position-sensitive event cursor. The
    // following Press/Release/capture-resync applications reuse that exact
    // verdict: they neither emit wxEVT_SET_CURSOR again nor regress to the
    // static hand/cross cursor.
    REQUIRE(host->TestApplySlotPointerCursor(
        button, cursorButtonPoint, wxWinUIInputKind::Move));
    CHECK(setCursorEvents == 1);
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_IBEAM).GetHCURSOR());
    REQUIRE(host->TestApplySlotPointerCursor(
        button, cursorButtonPoint, wxWinUIInputKind::Press));
    CHECK(setCursorEvents == 1);
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_IBEAM).GetHCURSOR());
    REQUIRE(host->TestApplySlotPointerCursor(
        button, cursorButtonPoint, wxWinUIInputKind::Release));
    CHECK(setCursorEvents == 1);
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_IBEAM).GetHCURSOR());
    REQUIRE(host->TestApplySlotPointerCursor(
        button, cursorButtonPoint, wxWinUIInputKind::Wheel));
    CHECK(setCursorEvents == 1);
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_IBEAM).GetHCURSOR());
    REQUIRE(host->TestApplySlotPointerCursor(
        button, cursorButtonPoint, wxWinUIInputKind::CaptureLost));
    CHECK(setCursorEvents == 1);
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_IBEAM).GetHCURSOR());

    setCursorMode = 2;
    parentCursorHandlerEnabled = true;
    REQUIRE(host->TestApplySlotPointerCursor(
        button, cursorButtonPoint, wxWinUIInputKind::Move));
    CHECK(setCursorEvents == 2);
    CHECK(parentSetCursorEvents == 1);
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_SIZING).GetHCURSOR());
    parentCursorHandlerEnabled = false;
    setCursorMode = 3;
    CHECK_FALSE(host->TestApplySlotPointerCursor(
        button, cursorButtonPoint, wxWinUIInputKind::Move));
    CHECK(host->GetIslandCursorHandleForTest() ==
          button->GetCursor().GetHCURSOR());
    button->SetCursor(wxNullCursor);
    setCursorMode = 0;

    panel->SetCursor(wxCursor(wxCURSOR_CROSS));
    DrainToQuiescence();
    checkShape(MUI::InputSystemCursorShape::Cross);
    checkIslandShape(MUI::InputSystemCursorShape::Cross);

    button->SetCursor(wxCursor(wxCURSOR_IBEAM));
    DrainToQuiescence();
    checkShape(MUI::InputSystemCursorShape::IBeam);
    checkIslandShape(MUI::InputSystemCursorShape::IBeam);

    button->SetCursor(wxNullCursor);
    DrainToQuiescence();
    checkShape(MUI::InputSystemCursorShape::Cross);
    checkIslandShape(MUI::InputSystemCursorShape::Cross);

    panel->SetCursor(wxNullCursor);
    DrainToQuiescence();
    CHECK(host->GetSlotCursorForTest(button) == nullptr);
    CHECK_FALSE(host->IsInputPointerSourceAuthoritativeForTest());
    checkIslandShape(MUI::InputSystemCursorShape::Arrow);

    // When neither wx nor busy/global policy supplies an override, leave the
    // island source owned by XAML. This models TextBox's natural I-beam and
    // catches an accidental Cursor(nullptr) written after every routed move.
    const auto naturalIBeam = MUI::InputSystemCursor::Create(
        MUI::InputSystemCursorShape::IBeam);
    REQUIRE(host->TestSimulateXamlCursorSelection(naturalIBeam));
    REQUIRE(host->TestActivateSlotCursor(button));
    CHECK_FALSE(host->IsInputPointerSourceAuthoritativeForTest());
    checkIslandShape(MUI::InputSystemCursorShape::IBeam);

    // The process-wide cursor is the final fallback after the complete wx
    // ancestor chain. Its notification must update the island without
    // requiring a physical pointer move.
    wxSetCursor(wxCursor(wxCURSOR_SIZING));
    DrainToQuiescence();
    checkShape(MUI::InputSystemCursorShape::SizeAll);
    checkIslandShape(MUI::InputSystemCursorShape::SizeAll);
    const int eventsAfterGlobalCursor = setCursorEvents;
    wxSetCursor(wxCursor(wxCURSOR_SIZING));
    DrainToQuiescence();
    CHECK(setCursorEvents == eventsAfterGlobalCursor);
    wxSetCursor(wxNullCursor);
    DrainToQuiescence();
    CHECK_FALSE(host->IsInputPointerSourceAuthoritativeForTest());
    checkIslandShape(MUI::InputSystemCursorShape::Arrow);

    // The WinAppSDK interop path must preserve application-created bitmap
    // cursors instead of silently falling back to the default arrow.
    wxBitmap cursorBitmap(16, 16, 32);
    {
        wxMemoryDC dc(cursorBitmap);
        dc.SetBackground(*wxWHITE_BRUSH);
        dc.Clear();
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawLine(0, 0, 15, 15);
    }
    const wxCursor customCursor(cursorBitmap, wxPoint(1, 1));
    REQUIRE(customCursor.IsOk());
    button->SetCursor(customCursor);
    DrainToQuiescence();
    const auto customPeerCursor = host->GetSlotCursorForTest(button);
    REQUIRE(customPeerCursor != nullptr);
    CHECK(customPeerCursor.try_as<MUI::InputCustomCursor>() != nullptr);
    const auto customIslandCursor = host->GetIslandCursorForTest();
    REQUIRE(customIslandCursor != nullptr);
    CHECK(customIslandCursor.try_as<MUI::InputCustomCursor>() != nullptr);

    // Busy is an application-wide override with true nesting semantics.
    // Ending the inner scope must keep Wait; ending the outer one restores
    // the exact custom cursor that was active before it.
    {
        wxBusyCursor outerBusy;
        const WXHCURSOR busyHandle = wxHOURGLASS_CURSOR->GetHCURSOR();
        CHECK(host->GetIslandCursorHandleForTest() == busyHandle);
        {
            wxBusyCursor innerBusy;
            CHECK(host->GetIslandCursorHandleForTest() == busyHandle);
        }
        CHECK(host->GetIslandCursorHandleForTest() == busyHandle);
    }
    DrainToQuiescence();
    const auto restoredSlotCursor = host->GetSlotCursorForTest(button);
    REQUIRE(restoredSlotCursor != nullptr);
    CHECK(restoredSlotCursor.try_as<MUI::InputCustomCursor>() != nullptr);
    const auto restoredIslandCursor = host->GetIslandCursorForTest();
    REQUIRE(restoredIslandCursor != nullptr);
    CHECK(restoredIslandCursor.try_as<MUI::InputCustomCursor>() != nullptr);
    CHECK(host->GetIslandCursorHandleForTest() ==
          customCursor.GetHCURSOR());

    const unsigned sets =
        wxWinUITopLevelHost::GetSlotCursorSetCount();
    host->FlushSync();
    CHECK(wxWinUITopLevelHost::GetSlotCursorSetCount() == sets);

    // A topology mutation under a stationary pointer must re-hit-test, not
    // blindly retain the destroyed slot or fall back to arrow. Expose the
    // native panel with its own cursor at the exact former button point.
    panel->SetCursor(wxCursor(wxCURSOR_HAND));
    RECT buttonRect;
    REQUIRE(::GetWindowRect(GetHwndOf(button), &buttonRect) != 0);
    const wxPoint buttonCentre((buttonRect.left + buttonRect.right) / 2,
                               (buttonRect.top + buttonRect.bottom) / 2);
    REQUIRE(host->TestRefreshPointerCursorAt(buttonCentre));
    CHECK(host->GetIslandCursorHandleForTest() ==
          customCursor.GetHCURSOR());
    button->Destroy();
    DrainToQuiescence();
    CHECK(host->GetIslandCursorHandleForTest() ==
          panel->GetCursor().GetHCURSOR());

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::CursorVerdictTopologyKey",
          "[HostState][winui-cursor]")
{
    namespace MUI = winrt::Microsoft::UI::Input;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "cursor-topology",
                    wxPoint(-20000, -20000), wxSize(360, 180));
    wxPanel * const panelA =
        new wxPanel(frame, wxID_ANY, wxPoint(10, 10), wxSize(300, 120));
    wxPanel * const panelB =
        new wxPanel(frame, wxID_ANY, wxPoint(10, 10), wxSize(300, 120));
    wxButton * const button =
        new wxButton(panelA, wxID_ANY, "dynamic",
                     wxPoint(20, 20), wxSize(100, 32));
    wxButton * const buttonB =
        new wxButton(panelB, wxID_ANY, "policy mutation",
                     wxPoint(150, 20), wxSize(120, 32));
    wxButton * const captureButton =
        new wxButton(panelB, wxID_ANY, "capture mutation",
                     wxPoint(20, 70), wxSize(120, 32));
    panelB->Lower();
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host != nullptr);

    int events = 0;
    bool useParentCursor = false;
    bool resizeLeafInHandler = false;
    int parentEvents = 0;
    panelA->Bind(
        wxEVT_SET_CURSOR,
        [&](wxSetCursorEvent& event)
        {
            ++parentEvents;
            event.SetCursor(wxCursor(
                panelA->GetClientSize().x >= 250
                    ? wxCURSOR_CROSS
                    : wxCURSOR_SIZING));
        });
    button->Bind(
        wxEVT_SET_CURSOR,
        [&](wxSetCursorEvent& event)
        {
            ++events;
            if ( resizeLeafInHandler )
            {
                resizeLeafInHandler = false;
                button->SetSize(40, 32);
            }
            if ( useParentCursor )
            {
                event.Skip();
                return;
            }

            if ( button->GetParent() == panelB )
            {
                event.SetCursor(wxCursor(wxCURSOR_CROSS));
            }
            else if ( event.GetX() < button->GetClientSize().x / 2 )
            {
                event.SetCursor(wxCursor(wxCURSOR_IBEAM));
            }
            else
            {
                event.SetCursor(wxCursor(wxCURSOR_HAND));
            }
        });

    const wxPoint screenPoint =
        button->ClientToScreen(wxPoint(25, 16));
    REQUIRE(host->TestApplySlotPointerCursor(
        button, screenPoint, wxWinUIInputKind::Move));
    CHECK(events == 1);
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_IBEAM).GetHCURSOR());

    // A flush with identical geometry reuses the exact verdict.
    host->FlushSync();
    CHECK(events == 1);

    // A handler may mutate the very geometry used to compute its verdict.
    // The first result must be rejected; the retry resolves against the new
    // size and is the only result allowed to reach the island.
    resizeLeafInHandler = true;
    const wxPoint mutationPoint =
        button->ClientToScreen(wxPoint(26, 16));
    CHECK_FALSE(host->TestApplySlotPointerCursor(
        button, mutationPoint, wxWinUIInputKind::Move));
    CHECK(events == 2);
    REQUIRE(host->TestApplySlotPointerCursor(
        button, mutationPoint, wxWinUIInputKind::Move));
    CHECK(events == 3);
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_HAND).GetHCURSOR());

    // Size is part of the key: handlers may compare event coordinates with
    // GetClientSize(), so a stationary point can legitimately change shape.
    button->SetSize(100, 32);
    DrainToQuiescence();
    CHECK(events == 4);
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_IBEAM).GetHCURSOR());

    // Moving the leaf changes its client coordinates at the same screen point.
    button->Move(25, 20);
    DrainToQuiescence();
    CHECK(events == 5);

    // Geometry of an ancestor is also part of the event contract: a parent
    // handler can choose from its own current size while the leaf stays put.
    useParentCursor = true;
    REQUIRE(host->TestApplySlotPointerCursor(
        button, screenPoint, wxWinUIInputKind::Move));
    CHECK(events == 6);
    CHECK(parentEvents == 1);
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_CROSS).GetHCURSOR());
    panelA->SetSize(200, 120);
    DrainToQuiescence();
    CHECK(events == 7);
    CHECK(parentEvents == 2);
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_SIZING).GetHCURSOR());
    useParentCursor = false;

    // Reparent at the same screen location changes the ancestor callback
    // chain even when both screen and leaf-client coordinates stay equal.
    const wxPoint oldScreenOrigin =
        button->ClientToScreen(wxPoint(0, 0));
    REQUIRE(button->Reparent(panelB));
    button->Move(panelB->ScreenToClient(oldScreenOrigin));
    DrainToQuiescence();
    CHECK(events == 8);
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_CROSS).GetHCURSOR());

    // Resolving a new slot must not refresh the previously active slot when
    // the new handler changes cursor policy re-entrantly. The policy replay
    // targets B exactly once and is callback-free.
    int buttonBEvents = 0;
    buttonB->Bind(
        wxEVT_SET_CURSOR,
        [&](wxSetCursorEvent& event)
        {
            ++buttonBEvents;
            buttonB->SetCursor(wxCursor(wxCURSOR_CROSS));
            event.SetCursor(wxCursor(wxCURSOR_IBEAM));
        });
    const wxPoint buttonBPoint =
        buttonB->ClientToScreen(wxPoint(20, 16));
    CHECK_FALSE(host->TestApplySlotPointerCursor(
        buttonB, buttonBPoint, wxWinUIInputKind::Move));
    CHECK(buttonBEvents == 1);
    CHECK(events == 8);
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_CROSS).GetHCURSOR());

    // A capture transition inside the callback must not reassert the cursor
    // from the previously active surface. The in-flight verdict remains the
    // sole winner and the application callback runs exactly once.
    int captureEvents = 0;
    captureButton->Bind(
        wxEVT_SET_CURSOR,
        [&](wxSetCursorEvent& event)
        {
            ++captureEvents;
            host->NotifyNativeCaptureMutation();
            event.SetCursor(wxCursor(wxCURSOR_SIZING));
        });
    const wxPoint capturePoint =
        captureButton->ClientToScreen(wxPoint(20, 16));
    REQUIRE(host->TestApplySlotPointerCursor(
        captureButton, capturePoint, wxWinUIInputKind::Move));
    CHECK(captureEvents == 1);
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_SIZING).GetHCURSOR());

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::CursorSourceFailureIsTruthful",
          "[HostState][winui-cursor]")
{
    namespace MUI = winrt::Microsoft::UI::Input;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "cursor-source-failure",
                    wxPoint(-20000, -20000), wxSize(300, 120));
    wxTextCtrl * const text =
        new wxTextCtrl(frame, wxID_ANY, "text",
                       wxPoint(20, 20), wxSize(180, 32));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(text);
    REQUIRE(host != nullptr);

    text->SetCursor(wxCursor(wxCURSOR_HAND));
    DrainToQuiescence();
    REQUIRE(host->TestActivateSlotCursor(text));
    text->SetCursor(wxNullCursor);
    DrainToQuiescence();

    // Model the descendant-owned TextBox I-beam that outranks a root
    // ProtectedCursor. A failed source-level Cross override must report
    // failure and leave this truthful runtime cursor observable.
    const auto naturalIBeam = MUI::InputSystemCursor::Create(
        MUI::InputSystemCursorShape::IBeam);
    REQUIRE(host->TestSimulateXamlCursorSelection(naturalIBeam));
    CHECK_FALSE(host->IsInputPointerSourceAuthoritativeForTest());

    int events = 0;
    text->Bind(
        wxEVT_SET_CURSOR,
        [&](wxSetCursorEvent& event)
        {
            ++events;
            event.SetCursor(wxCursor(wxCURSOR_CROSS));
        });
    const wxPoint point = text->ClientToScreen(wxPoint(20, 16));

    wxWinUITopLevelHost::TestFailInputPointerSourceSet(1);
    CHECK_FALSE(host->TestApplySlotPointerCursor(
        text, point, wxWinUIInputKind::Move));
    CHECK(events == 1);
    CHECK_FALSE(host->IsInputPointerSourceAuthoritativeForTest());
    const auto afterFailure = host->GetIslandCursorForTest().
        try_as<MUI::InputSystemCursor>();
    REQUIRE(afterFailure != nullptr);
    CHECK(afterFailure.CursorShape() ==
          MUI::InputSystemCursorShape::IBeam);

    // The terminal replay reuses the cached callback verdict without
    // invoking application code twice, and the one-shot failure is retried.
    REQUIRE(host->TestApplySlotPointerCursor(
        text, point, wxWinUIInputKind::Press));
    CHECK(events == 1);
    CHECK(host->IsInputPointerSourceAuthoritativeForTest());
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_CROSS).GetHCURSOR());

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::NativeCursorVerdictPositionKey",
          "[HostState][winui-cursor]")
{
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "native-cursor-position",
                    wxPoint(-20000, -20000), wxSize(460, 180));
    wxPanel * const nativePanel =
        new wxPanel(frame, wxID_ANY, wxPoint(10, 10), wxSize(220, 120));
    // A separate WinUI control realizes the shared island while leaving the
    // panel rectangle as a native bridge-region hole.
    new wxButton(frame, wxID_ANY, "island",
                 wxPoint(280, 20), wxSize(120, 32));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);

    int events = 0;
    bool useRightCursor = false;
    nativePanel->Bind(
        wxEVT_SET_CURSOR,
        [&](wxSetCursorEvent& event)
        {
            ++events;
            event.SetCursor(wxCursor(
                useRightCursor ? wxCURSOR_HAND : wxCURSOR_IBEAM));
        });

    const wxPoint left =
        nativePanel->ClientToScreen(wxPoint(30, 30));
    const wxPoint right =
        nativePanel->ClientToScreen(wxPoint(140, 30));
    REQUIRE(host->TestRefreshPointerCursorAt(left));
    CHECK(events == 1);
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_IBEAM).GetHCURSOR());

    useRightCursor = true;
    REQUIRE(host->TestRefreshPointerCursorAt(right));
    CHECK(events == 2);
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_HAND).GetHCURSOR());

    // A stationary resync reuses the complete native verdict.
    REQUIRE(host->TestRefreshPointerCursorAt(right));
    CHECK(events == 2);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::NestedNativeCursorReplayIsSingleShot",
          "[HostState][winui-cursor]")
{
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "nested-native-cursor",
                    wxPoint(-20000, -20000), wxSize(460, 180));
    wxPanel * const nativePanel =
        new wxPanel(frame, wxID_ANY, wxPoint(10, 10), wxSize(180, 120));
    wxButton * const slotButton =
        new wxButton(frame, wxID_ANY, "slot",
                     wxPoint(260, 30), wxSize(120, 32));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(slotButton);
    REQUIRE(host != nullptr);

    const wxPoint nativePoint =
        nativePanel->ClientToScreen(wxPoint(40, 30));
    int nativeEvents = 0;
    nativePanel->Bind(
        wxEVT_SET_CURSOR,
        [&](wxSetCursorEvent& event)
        {
            ++nativeEvents;
            if ( nativeEvents == 1 )
                nativePanel->SetCursor(wxCursor(wxCURSOR_CROSS));
            event.SetCursor(wxCursor(wxCURSOR_IBEAM));
        });

    int slotEvents = 0;
    bool nestedResult = false;
    bool mutateOuterPolicy = false;
    slotButton->Bind(
        wxEVT_SET_CURSOR,
        [&](wxSetCursorEvent& event)
        {
            ++slotEvents;
            if ( mutateOuterPolicy )
                slotButton->SetCursor(wxCursor(wxCURSOR_HAND));
            nestedResult =
                host->TestRefreshPointerCursorAt(nativePoint);
            event.SetCursor(wxCursor(wxCURSOR_IBEAM));
        });

    // The nested native descriptor is newer than the outer slot route. Its
    // policy replay must resolve the newly installed static policy in place,
    // without making the outer unwind send WM_SETCURSOR to the native panel
    // again. The transient event cursor cannot outrank that new policy.
    const wxPoint slotPoint =
        slotButton->ClientToScreen(wxPoint(20, 16));
    CHECK_FALSE(host->TestApplySlotPointerCursor(
        slotButton, slotPoint, wxWinUIInputKind::Move));
    CHECK(nestedResult);
    CHECK(slotEvents == 1);
    CHECK(nativeEvents == 1);
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_CROSS).GetHCURSOR());

    // A pending policy inherited from the outer slot is not evidence that
    // this newer native WM_SETCURSOR changed policy. Its fresh event verdict
    // must be preserved, again without a second native callback.
    mutateOuterPolicy = true;
    CHECK_FALSE(host->TestApplySlotPointerCursor(
        slotButton,
        slotButton->ClientToScreen(wxPoint(21, 16)),
        wxWinUIInputKind::Move));
    CHECK(nestedResult);
    CHECK(slotEvents == 2);
    CHECK(nativeEvents == 2);
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_IBEAM).GetHCURSOR());

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::PopupCursorScope",
          "[HostState][winui-cursor]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
    namespace MUXCP =
        winrt::Microsoft::UI::Xaml::Controls::Primitives;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "cursor-popup",
                    wxPoint(-20000, -20000), wxSize(360, 180));
    wxButton * const button =
        new wxButton(frame, wxID_ANY, "under popup",
                     wxPoint(30, 30), wxSize(140, 40));
    button->SetCursor(wxCursor(wxCURSOR_HAND));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host != nullptr);
    const wxPoint buttonPoint =
        button->ClientToScreen(wxPoint(70, 20));
    REQUIRE(host->TestRefreshPointerCursorAt(buttonPoint));
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_HAND).GetHCURSOR());

    MUXCP::Popup popup;
    popup.XamlRoot(host->GetXamlRoot());
    MUXC::Button popupButton;
    popupButton.Content(winrt::box_value(L"popup"));
    popupButton.Width(100);
    popupButton.Height(50);
    popup.Child(popupButton);

    const wxPoint popupCentre = host->ScreenToRootDIP(buttonPoint);
    popup.HorizontalOffset(popupCentre.x - 50);
    popup.VerticalOffset(popupCentre.y - 25);
    popup.IsOpen(true);
    DrainToQuiescence();

    // PopupRoot is outside xamlRoot.Content(): explicit per-popup hit testing
    // must yield the island-wide wx override at the covered point.
    REQUIRE(host->TestRefreshPointerCursorAt(buttonPoint));
    CHECK_FALSE(host->IsInputPointerSourceAuthoritativeForTest());
    CHECK(host->GetIslandCursorHandleForTest() == nullptr);

    // An unrelated open popup elsewhere must not make the whole TLW generic.
    popup.HorizontalOffset(popupCentre.x + 120);
    popup.VerticalOffset(popupCentre.y + 70);
    DrainToQuiescence();
    REQUIRE(host->TestRefreshPointerCursorAt(buttonPoint));
    CHECK(host->GetIslandCursorHandleForTest() ==
          wxCursor(wxCURSOR_HAND).GetHCURSOR());

    popup.IsOpen(false);
    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::ModalBusyCursorPolicy",
          "[HostState][winui-cursor]")
{
    wxDialog * const dialog =
        new wxDialog(nullptr, wxID_ANY, "modal-cursor",
                     wxPoint(-20000, -20000), wxSize(240, 120));
    wxButton * const button =
        new wxButton(dialog, wxID_ANY, "modal child",
                     wxPoint(10, 10), wxSize(120, 32));
    button->SetCursor(wxCursor(wxCURSOR_HAND));

    // Realize the off-screen island before entering the modal loop so every
    // assertion below observes the same host and InputPointerSource.
    dialog->Show();
    DrainToQuiescence();
    dialog->Hide();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host != nullptr);
    REQUIRE(host->TestActivateSlotCursor(button));
    const WXHCURSOR hand = button->GetCursor().GetHCURSOR();
    CHECK(host->GetIslandCursorHandleForTest() == hand);

    int modalCursorEvents = 0;
    int modalPolicyEvents = -1;
    bool destroyOnModalCursorEvent = false;
    button->Bind(
        wxEVT_SET_CURSOR,
        [&](wxSetCursorEvent& event)
        {
            ++modalCursorEvents;
            if ( destroyOnModalCursorEvent )
                dialog->Destroy();
            event.Skip();
        });

    bool callbackRan = false;
    bool wasModalInCallback = false;
    bool activatedInCallback = false;
    WXHCURSOR cursorInCallback = nullptr;
    {
        wxBusyCursor outerBusy;
        const WXHCURSOR wait = wxHOURGLASS_CURSOR->GetHCURSOR();
        CHECK(host->GetIslandCursorHandleForTest() == wait);

        dialog->CallAfter(
            [&]()
            {
                callbackRan = true;
                wasModalInCallback = dialog->IsModal();
                activatedInCallback =
                    host->TestActivateSlotCursor(button);
                cursorInCallback =
                    host->GetIslandCursorHandleForTest();
                modalCursorEvents = 0;
                destroyOnModalCursorEvent = true;
                wxWinUINotifyModalCursorChanged();
                modalPolicyEvents = modalCursorEvents;
                destroyOnModalCursorEvent = false;
                dialog->EndModal(wxID_OK);
            });

        CHECK(dialog->ShowModal() == wxID_OK);
        CHECK(callbackRan);
        CHECK(wasModalInCallback);
        CHECK(activatedInCallback);
        CHECK(cursorInCallback == hand);
        CHECK(modalPolicyEvents == 0);

        // Destroying the modal-data sentinel makes the still-active outer
        // busy policy authoritative again before ShowModal() returns.
        CHECK(host->GetIslandCursorHandleForTest() == wait);
    }

    CHECK(host->GetIslandCursorHandleForTest() == hand);
    dialog->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::CarrierStretchesContent", "[HostState]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;

    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "state-stretch");
    wxWinUIXamlHost * const xamlHost =
        new wxWinUIXamlHost(frame, wxID_ANY, wxPoint(0, 0), wxSize(300, 200));
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Background=\"Red\"/>"));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto fe =
        slot->GetContent().try_as<MUX::FrameworkElement>();
    REQUIRE(fe != nullptr);

    // A size-less element must FILL the slot: the carrier's content
    // alignments must be Stretch (the ContentControl defaults are
    // Left/Top, which would leave the content hugging the corner).
    const double scale = host->GetScale();
    CHECK(std::fabs(fe.ActualWidth() -
                    slot->GetRectInTLW().width / scale) <= 2.0);
    CHECK(std::fabs(fe.ActualHeight() -
                    slot->GetRectInTLW().height / scale) <= 2.0);

    // ... and it must follow a resize of the hosting window.
    xamlHost->SetSize(400, 250);
    DrainToQuiescence();
    CHECK(std::fabs(fe.ActualWidth() -
                    slot->GetRectInTLW().width / scale) <= 2.0);
    CHECK(std::fabs(fe.ActualHeight() -
                    slot->GetRectInTLW().height / scale) <= 2.0);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::EffectiveDisableArbitraryComposite", "[HostState]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    // An ARBITRARY composite through wxWinUIXamlHost: unlike wxRadioBox,
    // nothing here propagates the enabled state itself -- only the
    // carrier's Control::IsEnabled inheritance can do it.
    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "state-arb");
    wxPanel * const panel = new wxPanel(frame);
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(panel);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<StackPanel xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"><Button Content=\"b\"/><CheckBox Content=\"c\"/>"
        "</StackPanel>"));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto stack = slot->GetContent().try_as<MUXC::Panel>();
    REQUIRE(stack != nullptr);
    REQUIRE(stack.Children().Size() == 2u);

    panel->Disable();
    DrainDispatch(5);
    for ( uint32_t i = 0; i < stack.Children().Size(); ++i )
    {
        INFO("composite child #" << i);
        const auto control =
            stack.Children().GetAt(i).try_as<MUXC::Control>();
        REQUIRE(control != nullptr);
        CHECK(!control.IsEnabled());
    }

    panel->Enable();
    DrainDispatch(5);
    for ( uint32_t i = 0; i < stack.Children().Size(); ++i )
    {
        const auto control =
            stack.Children().GetAt(i).try_as<MUXC::Control>();
        CHECK(control.IsEnabled());
    }

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::ContentSwapFaultKeepsOldContent", "[HostState]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "state-swapfault");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
#if wxUSE_TOOLTIPS
    xamlHost->SetToolTip("keep");
#endif
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto oldContent = slot->GetContent();
    REQUIRE(oldContent != nullptr);
#if wxUSE_TOOLTIPS
    REQUIRE(MUXC::ToolTipService::GetToolTip(oldContent) != nullptr);
#endif

    // An element that already has a parent: installing it throws inside
    // XAML, and the transaction must roll back to the OLD content.
    MUXC::Border stolen;
    MUXC::StackPanel foreignParent;
    foreignParent.Children().Append(stolen);

    CHECK(!slot->SetContent(stolen));
    CHECK(slot->GetContent() == oldContent);

    // ... with its state still coherent after the next flush
    DrainDispatch(3);
#if wxUSE_TOOLTIPS
    const auto tip = MUXC::ToolTipService::GetToolTip(slot->GetContent());
    REQUIRE(tip != nullptr);
    CHECK(GetToolTipInspectableText(tip) == "keep");
#endif

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::FocusEventContract", "[HostState]")
{
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "state-focus-events");
    wxButton * const first =
        new wxButton(frame, wxID_ANY, "first", wxPoint(10, 10));
    wxButton * const second =
        new wxButton(frame, wxID_ANY, "second", wxPoint(10, 50));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    host->FocusSlot(first);
    DrainDispatch(3);
    REQUIRE(host->GetFocusOwner() == first);

    std::vector<wxString> events;
    bool counterpartsCorrect = true;
    first->Bind(wxEVT_KILL_FOCUS,
        [&](wxFocusEvent& event)
        {
            events.push_back("kill-first");
            counterpartsCorrect =
                counterpartsCorrect && event.GetWindow() == second;
            event.Skip();
        });
    second->Bind(wxEVT_CHILD_FOCUS,
        [&](wxChildFocusEvent& event)
        {
            events.push_back("child-second");
            counterpartsCorrect =
                counterpartsCorrect && event.GetWindow() == second;
            event.Skip();
        });
    second->Bind(wxEVT_SET_FOCUS,
        [&](wxFocusEvent& event)
        {
            events.push_back("set-second");
            counterpartsCorrect =
                counterpartsCorrect && event.GetWindow() == first;
            event.Skip();
        });
    second->Bind(wxEVT_KILL_FOCUS,
        [&](wxFocusEvent& event)
        {
            events.push_back("kill-second");
            event.Skip();
        });

    // Exercise the public wxWindow path. It records the host intent directly;
    // the implementation transfer into the island must preserve native wx
    // KILL/CHILD/SET ordering and never duplicate the logical transition.
    second->SetFocus();
    DrainDispatch(3);

    REQUIRE(events.size() == 3);
    CHECK(events[0] == "kill-first");
    CHECK(events[1] == "child-second");
    CHECK(events[2] == "set-second");
    CHECK(counterpartsCorrect);
    CHECK(host->GetFocusOwner() == second);
    CHECK(wxWindow::FindFocus() == second);

    // Refocusing the already-owned slot (including a bubbling inner-element
    // GotFocus) is not another logical wx transition.
    host->FocusSlot(second);
    DrainDispatch(3);
    CHECK(events.size() == 3);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::FocusCounterpartsBidirectional",
          "[HostState][HostLifecycle][focus]")
{
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "focus-counterparts");
    wxButton * const slotWindow =
        new wxButton(frame, wxID_ANY, "slot", wxPoint(10, 10));
    wxWindow * const native =
        new wxWindow(frame, wxID_ANY, wxPoint(180, 10), wxSize(60, 30));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(slotWindow);
    REQUIRE(host != nullptr);
    ::SetFocus(static_cast<HWND>(native->GetHWND()));
    DrainDispatch(2);
    REQUIRE(::GetFocus() == static_cast<HWND>(native->GetHWND()));

    unsigned nativeKills = 0;
    unsigned nativeSets = 0;
    unsigned slotKills = 0;
    unsigned slotSets = 0;
    bool counterpartsCorrect = true;
    native->Bind(wxEVT_KILL_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++nativeKills;
            counterpartsCorrect =
                counterpartsCorrect && event.GetWindow() == slotWindow;
            event.Skip();
        });
    native->Bind(wxEVT_SET_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++nativeSets;
            counterpartsCorrect =
                counterpartsCorrect && event.GetWindow() == slotWindow;
            event.Skip();
        });
    slotWindow->Bind(wxEVT_KILL_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++slotKills;
            counterpartsCorrect =
                counterpartsCorrect && event.GetWindow() == native;
            event.Skip();
        });
    slotWindow->Bind(wxEVT_SET_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++slotSets;
            counterpartsCorrect =
                counterpartsCorrect && event.GetWindow() == native;
            event.Skip();
        });

    slotWindow->SetFocus();
    REQUIRE(DrainUntil([&]()
    {
        return host->GetFocusOwner() == slotWindow;
    }));
    REQUIRE(slotSets == 1);
    REQUIRE(nativeKills == 1);

    ::SetFocus(static_cast<HWND>(native->GetHWND()));
    DrainDispatch(4);

    CHECK(slotKills == 1);
    CHECK(nativeSets == 1);
    CHECK(counterpartsCorrect);
    CHECK(::GetFocus() == static_cast<HWND>(native->GetHWND()));
    CHECK(wxWindow::FindFocus() == native);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::UnloadedPublicFocusUsesNativeShell",
          "[HostState][HostLifecycle][focus][loaded]")
{
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "focus-before-loaded",
                    wxPoint(-32000, -32000), wxSize(240, 140));
    wxButton * const button =
        new wxButton(frame, wxID_ANY, "target", wxPoint(10, 10));
    wxWindow * const holder =
        new wxWindow(frame, wxID_ANY, wxPoint(140, 10), wxSize(60, 30));

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(button);
    REQUIRE(slot != nullptr);

    // Settle the one-time native TLW activation before counting the target's
    // transition. Otherwise Windows legitimately restores the child focus once
    // during activation and the test measures two different operations.
    holder->SetFocus();
    REQUIRE(::GetFocus() == static_cast<HWND>(holder->GetHWND()));
    REQUIRE_FALSE(slot->IsContentLoaded());

    unsigned setEvents = 0;
    button->Bind(
        wxEVT_SET_FOCUS,
        [&setEvents](wxFocusEvent& event)
        {
            ++setEvents;
            event.Skip();
        });

    // XAML cannot own focus until Loaded, but wxMSW still gives a hidden
    // child's real shell HWND synchronous authority. WM_SETFOCUS then records
    // the bounded XAML hand-off for a future Loaded edge.
    button->SetFocus();
    CHECK(::GetFocus() == static_cast<HWND>(button->GetHWND()));
    CHECK(wxWindow::FindFocus() == button);
    CHECK(setEvents == 1);
    CHECK(host->GetFocusOwner() != button);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::UnloadedContentSwapRefreshesSameShellFocusIntent",
          "[HostState][HostLifecycle][focus][loaded][winui-009]")
{
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "focus-same-shell-content-swap",
                    wxPoint(-32000, -32000), wxSize(280, 160));
    wxWinUIXamlHost * const target = new wxWinUIXamlHost(frame);
    REQUIRE(target->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"generation A\"/>"));
    wxWindow * const holder =
        new wxWindow(frame, wxID_ANY, wxPoint(170, 10), wxSize(60, 30));

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(target);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(target);
    REQUIRE(slot != nullptr);

    holder->SetFocus();
    REQUIRE(::GetFocus() == static_cast<HWND>(holder->GetHWND()));
    REQUIRE_FALSE(slot->IsContentLoaded());

    unsigned setEvents = 0;
    target->Bind(
        wxEVT_SET_FOCUS,
        [&setEvents](wxFocusEvent& event)
        {
            ++setEvents;
            event.Skip();
        });

    // Generation A receives the ordinary synchronous shell transition.
    target->SetFocus();
    const HWND shell = static_cast<HWND>(target->GetHWND());
    REQUIRE(::GetFocus() == shell);
    REQUIRE(setEvents == 1);
    REQUIRE(host->GetFocusOwner() != target);

    // Replace only the XAML content while the same native shell remains
    // focused. USER32 cannot emit WM_SETFOCUS again, so the public SetFocus()
    // seam must refresh the deferred intent to generation B itself.
    REQUIRE(target->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"generation B\"/>"));
    REQUIRE(host->FindSlot(target) == slot);
    REQUIRE(target->GetHWND() == reinterpret_cast<WXHWND>(shell));
    REQUIRE_FALSE(slot->IsContentLoaded());
    target->SetFocus();
    CHECK(::GetFocus() == shell);
    CHECK(setEvents == 1);

    frame->Show();
    REQUIRE(DrainUntil([&]()
    {
        return host->GetFocusOwner() == target;
    }));
    CHECK(wxWindow::FindFocus() == target);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::LoadedUnfocusableDoesNotStealNativeAuthority",
           "[HostState][HostLifecycle][focus]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
    namespace MUXI = winrt::Microsoft::UI::Xaml::Input;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "focus-unfocusable");
    wxButton * const target =
        new wxButton(frame, wxID_ANY, "target", wxPoint(10, 10));
    wxWindow * const holder =
        new wxWindow(frame, wxID_ANY, wxPoint(180, 10), wxSize(60, 30));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(target);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(target);
    REQUIRE(slot != nullptr);

    ::SetFocus(static_cast<HWND>(holder->GetHWND()));
    DrainDispatch(2);
    REQUIRE(::GetFocus() == static_cast<HWND>(holder->GetHWND()));
    unsigned disabledSets = 0;
    unsigned disabledHolderKills = 0;
    target->Bind(wxEVT_SET_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++disabledSets;
            event.Skip();
        });
    holder->Bind(wxEVT_KILL_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++disabledHolderKills;
            event.Skip();
        });
    target->Disable();
    target->SetFocus();
    DrainDispatch(3);
    CHECK(disabledSets == 0);
    CHECK(disabledHolderKills == 0);
    CHECK(::GetFocus() == static_cast<HWND>(holder->GetHWND()));
    CHECK(host->GetFocusOwner() != target);
    target->Enable();

    host->FocusSlot(target);
    REQUIRE(DrainUntil([&]()
    {
        return host->GetFocusOwner() == target;
    }));

    // Swapping a focused peer for non-focusable content is not guaranteed to
    // produce a useful XAML LostFocus. A subsequent focus attempt must
    // reconcile the old logical owner from the exact current visual subtree.
    REQUIRE(slot->SetContent(MUXC::Border()));
    slot->SetContentLoadedForTest(true);
    host->FlushSync();
    host->FocusSlot(target);
    DrainToQuiescence();
    CHECK(host->GetFocusOwner() != target);

    ::SetFocus(static_cast<HWND>(holder->GetHWND()));
    DrainDispatch(2);
    REQUIRE(::GetFocus() == static_cast<HWND>(holder->GetHWND()));

    unsigned holderKills = 0;
    holder->Bind(wxEVT_KILL_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++holderKills;
            event.Skip();
        });

    host->FocusSlot(target);
    DrainToQuiescence();
    CHECK(holderKills == 0);
    CHECK(::GetFocus() == static_cast<HWND>(holder->GetHWND()));
    CHECK(host->GetFocusOwner() != target);
    CHECK(wxWindow::FindFocus() == holder);

    // A candidate can still refuse GettingFocus after the preflight. In that
    // case the exact native authority displaced by this attempt is restored;
    // the bridge never remains focused without a logical owner.
    MUXC::Button refusingPeer;
    const auto cancelToken = refusingPeer.GettingFocus(
        [](const MUX::UIElement&,
           const MUXI::GettingFocusEventArgs& event)
        {
            event.TryCancel();
        });
    REQUIRE(slot->SetContent(refusingPeer));
    slot->SetContentLoadedForTest(true);
    host->FlushSync();
    host->FocusSlot(target);
    DrainToQuiescence();

    CHECK(::GetFocus() == static_cast<HWND>(holder->GetHWND()));
    CHECK(host->GetFocusOwner() != target);
    CHECK(wxWindow::FindFocus() == holder);

    // The same refusal reached through the native shell must roll bridge
    // authority back to that exact shell without recursively treating the
    // nested WM_SETFOCUS as a fresh XAML handoff.
    unsigned shellSets = 0;
    target->Bind(wxEVT_SET_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++shellSets;
            event.Skip();
        });
    ::SetFocus(static_cast<HWND>(target->GetHWND()));
    DrainDispatch(4);
    CHECK(::GetFocus() == static_cast<HWND>(target->GetHWND()));
    CHECK(shellSets == 1);
    CHECK(host->GetFocusOwner() != target);
    CHECK(wxWindow::FindFocus() == target);
    refusingPeer.GettingFocus(cancelToken);

    // If rollback to the displaced HWND is refused, its pre-armed departure
    // counterpart has no authority after synchronous SetFocus() returns.
    // Re-enabling/focusing the same HWND later must not resurrect the slot as
    // the source of an unrelated native transition.
    ::SetFocus(static_cast<HWND>(holder->GetHWND()));
    DrainDispatch(2);
    REQUIRE(::GetFocus() == static_cast<HWND>(holder->GetHWND()));
    MUXC::Button failedRollbackPeer;
    const HWND holderHwnd = static_cast<HWND>(holder->GetHWND());
    const auto failedRollbackToken = failedRollbackPeer.GettingFocus(
        [holderHwnd](const MUX::UIElement&,
                     const MUXI::GettingFocusEventArgs& event)
        {
            ::EnableWindow(holderHwnd, FALSE);
            event.TryCancel();
        });
    REQUIRE(slot->SetContent(failedRollbackPeer));
    slot->SetContentLoadedForTest(true);
    host->FlushSync();
    host->FocusSlot(target);
    DrainDispatch(3);
    CHECK_FALSE(::IsWindowEnabled(holderHwnd));
    CHECK(host->GetFocusOwner() != target);

    wxWindow *failedRollbackCounterpart = target;
    holder->Bind(wxEVT_SET_FOCUS,
        [&](wxFocusEvent& event)
        {
            failedRollbackCounterpart = event.GetWindow();
            event.Skip();
        });
    ::EnableWindow(holderHwnd, TRUE);
    ::SetFocus(holderHwnd);
    DrainDispatch(3);
    CHECK(::GetFocus() == holderHwnd);
    CHECK(failedRollbackCounterpart != target);
    failedRollbackPeer.GettingFocus(failedRollbackToken);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::GettingFocusRevalidatesAfterPredicate",
          "[HostLifecycle][HostState][focus]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "getting-focus-revalidation");
    GettingFocusUnregisterProbe * const probe =
        new GettingFocusUnregisterProbe;
    REQUIRE(probe->Create(
        frame, wxID_ANY, wxPoint(10, 10), wxSize(100, 30)));
    wxWindow * const holder =
        new wxWindow(frame, wxID_ANY, wxPoint(180, 10), wxSize(60, 30));
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(probe, true);
    REQUIRE(host != nullptr);

    MUXC::Button peer;
    REQUIRE(host->RegisterSlot(probe, peer) != nullptr);
    frame->Show();
    DrainToQuiescence();
    ::SetFocus(static_cast<HWND>(holder->GetHWND()));
    DrainDispatch(2);
    REQUIRE(::GetFocus() == static_cast<HWND>(holder->GetHWND()));
    probe->Arm(host);

    // TryCancel() is advisory: WinUI can report true from UIElement::Focus()
    // even though the predicate synchronously retired its routed source.
    // The contract we own is immediate logical retirement, no second virtual
    // predicate on the stale wx window, and bounded physical cleanup.
    (void)peer.Focus(MUX::FocusState::Keyboard);
    CHECK(host->FindSlot(probe) == nullptr);
    CHECK(probe->GetKeyboardPredicateCalls() == 0);
    DrainDispatch(2);
    CHECK(host->FindSlot(probe) == nullptr);
    CHECK(probe->GetKeyboardPredicateCalls() == 0);
    CHECK(::GetFocus() == static_cast<HWND>(holder->GetHWND()));
    CHECK(host->GetFocusOwner() != probe);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::DestroyTLWFromGettingFocusDefersPhysicalShutdown",
          "[HostLifecycle][HostState][focus]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    const unsigned hostsBefore = wxWinUITopLevelHost::GetLiveHostCount();
    const unsigned statesBefore =
        wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
    const unsigned subclassesBefore =
        wxWinUITopLevelHost::GetLiveSubclassContextCount();
    const unsigned addsBefore =
        wxWinUITopLevelHost::GetSlotHandlerAddCount();
    const unsigned revokesBefore =
        wxWinUITopLevelHost::GetSlotHandlerRevokeCount();

    wxFrame *frame =
        new wxFrame(nullptr, wxID_ANY, "getting-focus-destroy-tlw");
    GettingFocusDestroyTLWProbe * const probe =
        new GettingFocusDestroyTLWProbe;
    REQUIRE(probe->Create(
        frame, wxID_ANY, wxPoint(10, 10), wxSize(100, 30)));
    wxWindow * const holder =
        new wxWindow(frame, wxID_ANY, wxPoint(180, 10), wxSize(60, 30));
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(probe, true);
    REQUIRE(host != nullptr);
    MUXC::Button peer;
    REQUIRE(host->RegisterSlot(probe, peer) != nullptr);
    frame->Show();
    DrainToQuiescence();
    ::SetFocus(static_cast<HWND>(holder->GetHWND()));
    DrainDispatch(2);
    REQUIRE(::GetFocus() == static_cast<HWND>(holder->GetHWND()));

    bool predicateInvoked = false;
    probe->Arm(&frame, &predicateInvoked);
    (void)peer.Focus(MUX::FocusState::Keyboard);
    CHECK(predicateInvoked);
    CHECK(frame == nullptr);
    if ( frame )
    {
        frame->Destroy();
        frame = nullptr;
    }

    REQUIRE(DrainUntil([&]()
    {
        return wxWinUITopLevelHost::GetLiveHostCount() == hostsBefore &&
               wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
                   statesBefore &&
               wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
                   subclassesBefore;
    }));
    const unsigned adds =
        wxWinUITopLevelHost::GetSlotHandlerAddCount() - addsBefore;
    const unsigned revokes =
        wxWinUITopLevelHost::GetSlotHandlerRevokeCount() - revokesBefore;
    CHECK(adds > 0);
    CHECK(adds == revokes);
}

TEST_CASE("HostLifecycle::DestroyTLWFromButtonCallbackDefersPhysicalShutdown",
          "[HostLifecycle][HostState][button]")
{
    namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    const unsigned hostsBefore = wxWinUITopLevelHost::GetLiveHostCount();
    const unsigned statesBefore =
        wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
    const unsigned subclassesBefore =
        wxWinUITopLevelHost::GetLiveSubclassContextCount();
    const unsigned slotAddsBefore =
        wxWinUITopLevelHost::GetSlotHandlerAddCount();
    const unsigned slotRevokesBefore =
        wxWinUITopLevelHost::GetSlotHandlerRevokeCount();
    const unsigned rootAddsBefore =
        wxWinUITopLevelHost::GetRootHandlerAddCount();
    const unsigned rootRevokesBefore =
        wxWinUITopLevelHost::GetRootHandlerRevokeCount();

    const auto virtualCounters =
        std::make_shared<DeferredDestroyVirtualCounters>();
    wxFrame * const frame =
        new DeferredDestroyProbeFrame(
            "button-callback-destroy-tlw", virtualCounters);
    wxButton * const button =
        new wxButton(frame, wxID_ANY, "destroy", wxPoint(10, 10));
    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(button);
    REQUIRE(slot != nullptr);
    const MUXC::Button peer = slot->GetContent().try_as<MUXC::Button>();
    REQUIRE(peer != nullptr);
    const MUXAP::ButtonAutomationPeer automationPeer(peer);

    const unsigned slotRevokesAtInvoke =
        wxWinUITopLevelHost::GetSlotHandlerRevokeCount();
    const unsigned rootRevokesAtInvoke =
        wxWinUITopLevelHost::GetRootHandlerRevokeCount();
    struct CallbackObservation
    {
        wxFrame *frame = nullptr;
        wxWindow *expectedFrameIdentity = nullptr;
        unsigned hostsBefore = 0;
        unsigned slotRevokesAtInvoke = 0;
        unsigned rootRevokesAtInvoke = 0;
        bool handlerEntered = false;
        bool destroyRequested = false;
        bool invokeReturned = false;
        bool hideEventSeen = false;
        bool hideEventNestedUnderInvoke = false;
        bool hideYieldCompleted = false;
        bool reshownFromFirstHide = false;
        unsigned hideEvents = 0;
        bool destroyEventSeen = false;
        bool resurrectionRejected = false;
        bool nativeDestroyDeferredInsideHandler = false;
        bool physicalHandlersStillAttachedInsideHandler = false;
    };
    const auto observation = std::make_shared<CallbackObservation>();
    observation->frame = frame;
    observation->expectedFrameIdentity = frame;
    observation->hostsBefore = hostsBefore;
    observation->slotRevokesAtInvoke = slotRevokesAtInvoke;
    observation->rootRevokesAtInvoke = rootRevokesAtInvoke;
    frame->Bind(
        wxEVT_SHOW,
        [observation](wxShowEvent& event)
        {
            // Destroy() hides a non-last TLW synchronously. This callback is
            // deliberately hostile: if Destroy() is replayed from the WinUI
            // delegate tail, the nested yield can delete the island while
            // ButtonAutomationPeer::Invoke() is still on its stack.
            const std::shared_ptr<CallbackObservation> state = observation;
            event.Skip();
            if ( event.IsShown() )
                return;

            state->hideEventSeen = true;
            ++state->hideEvents;
            state->hideEventNestedUnderInvoke = !state->invokeReturned;
            if ( state->hideEvents == 1 )
            {
                wxWindow * const frame = state->expectedFrameIdentity;
                state->reshownFromFirstHide = frame && frame->Show();
            }
            wxYield();
            state->hideYieldCompleted = true;
        });
    frame->Bind(
        wxEVT_DESTROY,
        [observation](wxWindowDestroyEvent& event)
        {
            const std::shared_ptr<CallbackObservation> state = observation;
            if ( event.GetWindow() == state->expectedFrameIdentity )
            {
                state->destroyEventSeen = true;
                // A property/focus callback during teardown must not be able
                // to resurrect a second island host on this dying TLW.
                state->resurrectionRejected =
                    wxWinUITopLevelHost::ForWindow(
                        event.GetWindow(), true) == nullptr;
            }
            event.Skip();
        });
    button->Bind(
        wxEVT_BUTTON,
        [observation](wxCommandEvent&)
        {
            // Copy the capture before destroying the button which owns this
            // dynamic binding. The closure itself may be reclaimed by the
            // nested yield; the independent shared state remains valid.
            const std::shared_ptr<CallbackObservation> state = observation;
            state->handlerEntered = true;
            wxFrame * const doomed = state->frame;
            state->frame = nullptr;
            state->destroyRequested = doomed && doomed->Destroy();

            // Process the pending wx destruction while the real WinUI
            // Button.Click/UIA Invoke callback is still on the stack.
            wxYield();
            state->nativeDestroyDeferredInsideHandler =
                wxWinUITopLevelHost::GetLiveHostCount() ==
                    state->hostsBefore + 1;
            state->physicalHandlersStillAttachedInsideHandler =
                wxWinUITopLevelHost::GetSlotHandlerRevokeCount() ==
                    state->slotRevokesAtInvoke &&
                wxWinUITopLevelHost::GetRootHandlerRevokeCount() ==
                    state->rootRevokesAtInvoke;
        });

    automationPeer.Invoke();
    observation->invokeReturned = true;
    CHECK(observation->handlerEntered);
    CHECK(observation->destroyRequested);
    CHECK(observation->frame == nullptr);
    CHECK(observation->nativeDestroyDeferredInsideHandler);
    CHECK(observation->physicalHandlersStillAttachedInsideHandler);
    CHECK_FALSE(observation->hideEventSeen);

    REQUIRE(DrainUntil([&]()
    {
        return wxWinUITopLevelHost::GetLiveHostCount() == hostsBefore &&
               wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
                   statesBefore &&
               wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
                   subclassesBefore;
    }));
    CHECK(observation->hideEventSeen);
    CHECK_FALSE(observation->hideEventNestedUnderInvoke);
    CHECK(observation->hideYieldCompleted);
    CHECK(observation->reshownFromFirstHide);
    CHECK(observation->hideEvents == 1);
    CHECK(virtualCounters->destroyCalls == 1);
    CHECK(virtualCounters->hideCalls == 1);
    CHECK(observation->destroyEventSeen);
    CHECK(observation->resurrectionRejected);
    CHECK(wxWinUITopLevelHost::GetSlotHandlerAddCount() - slotAddsBefore ==
          wxWinUITopLevelHost::GetSlotHandlerRevokeCount() -
              slotRevokesBefore);
    CHECK(wxWinUITopLevelHost::GetRootHandlerAddCount() - rootAddsBefore ==
          wxWinUITopLevelHost::GetRootHandlerRevokeCount() -
              rootRevokesBefore);
}

TEST_CASE("HostLifecycle::PendingTLWSurvivesRetainedButtonCallbackYield",
          "[HostLifecycle][HostState][button][pending-delete]")
{
    namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    const unsigned hostsBefore = wxWinUITopLevelHost::GetLiveHostCount();
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "pending-before-xaml-callback",
                    wxPoint(-32000, -32000), wxSize(280, 160));
    wxButton * const button =
        new wxButton(frame, wxID_ANY, "yield", wxPoint(10, 10));
    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(button);
    REQUIRE(slot != nullptr);
    const MUXC::Button peer = slot->GetContent().try_as<MUXC::Button>();
    REQUIRE(peer != nullptr);
    const MUXAP::ButtonAutomationPeer automationPeer(peer);

    struct Observation
    {
        bool handlerEntered = false;
        bool frameAliveAfterNestedYield = false;
        bool hostAliveAfterNestedYield = false;
    };
    const auto observation = std::make_shared<Observation>();
    const wxWeakRef<wxWindow> frameWeak(frame);
    const HWND frameHwnd = static_cast<HWND>(frame->GetHWND());
    button->Bind(
        wxEVT_BUTTON,
        [observation, frameWeak, frameHwnd, hostsBefore](wxCommandEvent&)
        {
            // Copy every capture before wxYield(): the old implementation can
            // delete the button and reclaim this closure from inside the yield.
            const std::shared_ptr<Observation> state = observation;
            const wxWeakRef<wxWindow> weak = frameWeak;
            state->handlerEntered = true;
            wxYield();
            state->frameAliveAfterNestedYield =
                weak.get() != nullptr && ::IsWindow(frameHwnd);
            state->hostAliveAfterNestedYield =
                wxWinUITopLevelHost::GetLiveHostCount() ==
                    hostsBefore + 1;
        });

    // Arm the normal wx delayed deletion before entering a retained XAML peer.
    // The outermost WinUI operation must temporarily remove that pending entry
    // so the nested yield cannot tear down the island under Invoke().
    REQUIRE(frame->Destroy());
    automationPeer.Invoke();

    CHECK(observation->handlerEntered);
    CHECK(observation->frameAliveAfterNestedYield);
    CHECK(observation->hostAliveAfterNestedYield);
    CHECK(frameWeak.get() != nullptr);

    REQUIRE(DrainUntil([&]()
    {
        return frameWeak.get() == nullptr &&
               wxWinUITopLevelHost::GetLiveHostCount() == hostsBefore;
    }));
}

TEST_CASE("HostLifecycle::ModalDialogDestroyWaitsForButtonCallback",
          "[HostLifecycle][HostState][button][dialog][modal]")
{
    namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    const unsigned hostsBefore = wxWinUITopLevelHost::GetLiveHostCount();
    wxDialog * const dialog =
        new wxDialog(nullptr, wxID_ANY, "modal-destroy-from-xaml",
                     wxPoint(-32000, -32000), wxSize(280, 160));
    wxButton * const button =
        new wxButton(dialog, wxID_ANY, "destroy", wxPoint(10, 10));

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(button);
    REQUIRE(slot != nullptr);
    const MUXC::Button peer = slot->GetContent().try_as<MUXC::Button>();
    REQUIRE(peer != nullptr);
    const MUXAP::ButtonAutomationPeer automationPeer(peer);

    struct Observation
    {
        bool invoked = false;
        bool invokeReturned = false;
        bool wasModal = false;
        bool destroyRequested = false;
        bool dialogAliveAfterNestedYield = false;
        bool hostAliveAfterNestedYield = false;
        bool destroyEventSeen = false;
        bool destroyEventNestedUnderInvoke = false;
    };
    const auto observation = std::make_shared<Observation>();
    const wxWeakRef<wxWindow> dialogWeak(dialog);
    const HWND dialogHwnd = static_cast<HWND>(dialog->GetHWND());
    dialog->Bind(
        wxEVT_DESTROY,
        [observation, dialog](wxWindowDestroyEvent& event)
        {
            if ( event.GetWindow() == dialog )
            {
                observation->destroyEventSeen = true;
                observation->destroyEventNestedUnderInvoke =
                    !observation->invokeReturned;
            }
            event.Skip();
        });
    button->Bind(
        wxEVT_BUTTON,
        [observation, dialogWeak, dialogHwnd, hostsBefore](wxCommandEvent&)
        {
            const std::shared_ptr<Observation> state = observation;
            const wxWeakRef<wxWindow> weak = dialogWeak;
            wxDialog * const doomed =
                wxDynamicCast(weak.get(), wxDialog);
            state->invoked = true;
            state->wasModal = doomed && doomed->IsModal();
            state->destroyRequested = doomed && doomed->Destroy();
            wxYield();
            state->dialogAliveAfterNestedYield =
                weak.get() != nullptr && ::IsWindow(dialogHwnd);
            state->hostAliveAfterNestedYield =
                wxWinUITopLevelHost::GetLiveHostCount() ==
                    hostsBefore + 1;
        });

    wxTheApp->CallAfter(
        [automationPeer, observation]()
        {
            automationPeer.Invoke();
            observation->invokeReturned = true;
        });
    (void)dialog->ShowModal();

    CHECK(observation->invoked);
    CHECK(observation->invokeReturned);
    CHECK(observation->wasModal);
    CHECK(observation->destroyRequested);
    CHECK(observation->dialogAliveAfterNestedYield);
    CHECK(observation->hostAliveAfterNestedYield);
    CHECK(observation->destroyEventSeen);
    CHECK_FALSE(observation->destroyEventNestedUnderInvoke);

    REQUIRE(DrainUntil([&]()
    {
        return dialogWeak.get() == nullptr &&
               wxWinUITopLevelHost::GetLiveHostCount() == hostsBefore;
    }));
}

#if wxUSE_POPUPWIN

TEST_CASE("HostLifecycle::PopupImmediateDestroyWaitsForButtonCallback",
          "[HostLifecycle][HostState][button][popup][winui-009]")
{
    namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    const unsigned hostsBefore = wxWinUITopLevelHost::GetLiveHostCount();
    const unsigned lifetimesBefore =
        wxWinUITopLevelHost::GetLiveHostLifetimeCount();
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "popup-owner",
                    wxPoint(-32000, -32000), wxSize(220, 120));
    const auto counters =
        std::make_shared<DeferredDestroyVirtualCounters>();
    DeferredDestroyProbePopup * const popup =
        new DeferredDestroyProbePopup(owner, counters);
    wxButton * const button =
        new wxButton(popup, wxID_ANY, "destroy popup");

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(button);
    REQUIRE(slot != nullptr);
    const MUXC::Button peer = slot->GetContent().try_as<MUXC::Button>();
    REQUIRE(peer != nullptr);
    const MUXAP::ButtonAutomationPeer automationPeer(peer);

    struct Observation
    {
        bool invokeReturned = false;
        bool destroyRequested = false;
        bool secondDestroy = true;
        unsigned overrideCallsAfterFirstRequest = 0;
        unsigned overrideCallsAfterSecondRequest = 0;
        bool aliveAfterYield = false;
        bool hostPhysicalLifetimeAfterYield = false;
    };
    const auto observation = std::make_shared<Observation>();
    const wxWeakRef<wxWindow> weakPopup(popup);
    const HWND popupHwnd = static_cast<HWND>(popup->GetHWND());
    button->Bind(
        wxEVT_BUTTON,
        [observation, weakPopup, popupHwnd, lifetimesBefore, counters](
            wxCommandEvent&)
        {
            const auto state = observation;
            const wxWeakRef<wxWindow> weak = weakPopup;
            wxWindow * const live = weak.get();
            state->destroyRequested = live && live->Destroy();
            state->overrideCallsAfterFirstRequest = counters->destroyCalls;

            wxAssertHandler_t const oldHandler =
                wxSetAssertHandler(nullptr);
            state->secondDestroy = live && live->Destroy();
            wxSetAssertHandler(oldHandler);
            state->overrideCallsAfterSecondRequest = counters->destroyCalls;

            wxYield();
            state->aliveAfterYield =
                weak.get() != nullptr && ::IsWindow(popupHwnd);
            state->hostPhysicalLifetimeAfterYield =
                wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
                    lifetimesBefore + 1;
        });

    automationPeer.Invoke();
    observation->invokeReturned = true;
    CHECK(observation->destroyRequested);
    CHECK_FALSE(observation->secondDestroy);
    CHECK(observation->overrideCallsAfterFirstRequest == 1);
    // The deliberately repeated public virtual call necessarily enters the
    // application override before wxPopupWindow can reject it. This is two
    // caller entries, not a duplicate deferred replay.
    CHECK(observation->overrideCallsAfterSecondRequest == 2);
    CHECK(observation->aliveAfterYield);
    CHECK(observation->hostPhysicalLifetimeAfterYield);
    CHECK(counters->destroyCalls == 2);

    REQUIRE(DrainUntil([&]()
    {
        return weakPopup.get() == nullptr &&
               wxWinUITopLevelHost::GetLiveHostCount() == hostsBefore &&
               wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
                   lifetimesBefore;
    }));
    // The replay is explicitly non-virtual: no third override entry appears.
    CHECK(counters->destroyCalls == 2);

    owner->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::TransientPopupDestroyWaitsForButtonCallback",
          "[HostLifecycle][HostState][button][popup][pending-delete]"
          "[winui-009]")
{
    namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    const unsigned lifetimesBefore =
        wxWinUITopLevelHost::GetLiveHostLifetimeCount();
    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "transient-popup-owner",
                    wxPoint(-32000, -32000), wxSize(220, 120));
    DirectTransientPopupBaseProbe * const popup =
        new DirectTransientPopupBaseProbe(owner);
    wxButton * const button =
        new wxButton(popup, wxID_ANY, "destroy transient");

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(button);
    REQUIRE(slot != nullptr);
    const MUXC::Button peer = slot->GetContent().try_as<MUXC::Button>();
    REQUIRE(peer != nullptr);
    const MUXAP::ButtonAutomationPeer automationPeer(peer);

    struct Observation
    {
        bool firstDestroy = false;
        bool secondDestroy = true;
        bool aliveAfterYield = false;
        bool hostPhysicalLifetimeAfterYield = false;
    };
    const auto observation = std::make_shared<Observation>();
    const wxWeakRef<wxWindow> weakPopup(popup);
    const HWND popupHwnd = static_cast<HWND>(popup->GetHWND());
    button->Bind(
        wxEVT_BUTTON,
        [observation, weakPopup, popupHwnd, lifetimesBefore](
            wxCommandEvent&)
        {
            const auto state = observation;
            const wxWeakRef<wxWindow> weak = weakPopup;
            wxWindow * const live = weak.get();
            state->firstDestroy = live && live->Destroy();

            // Preserve and verify the common transient contract without
            // reporting the deliberately triggered diagnostic as a test
            // failure.
            wxAssertHandler_t const oldHandler =
                wxSetAssertHandler(nullptr);
            state->secondDestroy = live && live->Destroy();
            wxSetAssertHandler(oldHandler);

            wxYield();
            state->aliveAfterYield =
                weak.get() != nullptr && ::IsWindow(popupHwnd);
            state->hostPhysicalLifetimeAfterYield =
                wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
                    lifetimesBefore + 1;
        });

    automationPeer.Invoke();
    CHECK(observation->firstDestroy);
    CHECK_FALSE(observation->secondDestroy);
    CHECK(observation->aliveAfterYield);
    CHECK(observation->hostPhysicalLifetimeAfterYield);
    REQUIRE(DrainUntil([&]()
    {
        return weakPopup.get() == nullptr &&
               wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
                   lifetimesBefore;
    }));
    owner->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::PendingTransientPopupSurvivesRetainedPeerYield",
          "[HostLifecycle][HostState][button][popup][pending-delete]"
          "[winui-009]")
{
    namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const owner =
        new wxFrame(nullptr, wxID_ANY, "pending-popup-owner",
                    wxPoint(-32000, -32000), wxSize(220, 120));
    const auto counters =
        std::make_shared<DeferredDestroyVirtualCounters>();
    DeferredDestroyProbeTransientPopup * const popup =
        new DeferredDestroyProbeTransientPopup(owner, counters);
    wxButton * const button =
        new wxButton(popup, wxID_ANY, "retained peer");

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(button);
    REQUIRE(slot != nullptr);
    const MUXC::Button peer = slot->GetContent().try_as<MUXC::Button>();
    REQUIRE(peer != nullptr);
    const MUXAP::ButtonAutomationPeer automationPeer(peer);

    const wxWeakRef<wxWindow> weakPopup(popup);
    const HWND popupHwnd = static_cast<HWND>(popup->GetHWND());
    struct Observation
    {
        bool callbackEntered = false;
        bool aliveAfterYield = false;
    };
    const auto observation = std::make_shared<Observation>();
    button->Bind(
        wxEVT_BUTTON,
        [observation, weakPopup, popupHwnd](wxCommandEvent&)
        {
            const auto state = observation;
            const wxWeakRef<wxWindow> weak = weakPopup;
            state->callbackEntered = true;
            wxYield();
            state->aliveAfterYield =
                weak.get() != nullptr && ::IsWindow(popupHwnd);
        });

    REQUIRE(popup->Destroy());
    REQUIRE(wxPendingDelete.Member(popup));
    CHECK_FALSE(wxTopLevelWindows.Member(popup));
    automationPeer.Invoke();
    CHECK(observation->callbackEntered);
    CHECK(observation->aliveAfterYield);
    CHECK(weakPopup.get() != nullptr);

    REQUIRE(DrainUntil([&]()
    {
        return weakPopup.get() == nullptr;
    }));
    CHECK(counters->destroyCalls == 1);
    owner->Destroy();
    DrainDispatch();
}

#endif // wxUSE_POPUPWIN

TEST_CASE("HostLifecycle::PendingModalSurvivesOtherRetainedPeerYield",
          "[HostLifecycle][HostState][button][dialog][modal]"
          "[pending-delete][winui-009]")
{
    namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const driverFrame =
        new wxFrame(nullptr, wxID_ANY, "modal-lifetime-driver",
                    wxPoint(-32000, -32000), wxSize(220, 120));
    wxButton * const driver =
        new wxButton(driverFrame, wxID_ANY, "yield driver");
    ShowOffscreenWithoutActivating(driverFrame);
    DrainToQuiescence();

    wxWinUITopLevelHost * const driverHost =
        wxWinUITopLevelHost::FindSlotOwner(driver);
    REQUIRE(driverHost != nullptr);
    wxWinUISlot * const driverSlot = driverHost->FindSlot(driver);
    REQUIRE(driverSlot != nullptr);
    const MUXC::Button driverPeer =
        driverSlot->GetContent().try_as<MUXC::Button>();
    REQUIRE(driverPeer != nullptr);
    const MUXAP::ButtonAutomationPeer automationPeer(driverPeer);

    wxDialog * const dialog =
        new wxDialog(nullptr, wxID_ANY, "pending modal under peer",
                     wxPoint(-32000, -32000), wxSize(260, 140));
    wxButton * const modalButton =
        new wxButton(dialog, wxID_ANY, "modal hosted child");
    REQUIRE(wxWinUITopLevelHost::FindForTLW(dialog) != nullptr);

    struct Observation
    {
        bool destroyRequested = false;
        bool secondDestroyAccepted = false;
        bool logicalDestroyPendingBeforeInvoke = false;
        bool globalOnlyCallbackEntered = false;
        bool globalOnlySecondDestroyAccepted = false;
        bool globalOnlyDialogAliveAfterYield = false;
        bool globalOnlyPhysicalHostAliveAfterYield = false;
        bool callbackEntered = false;
        bool dialogAliveAfterYield = false;
        bool physicalHostAliveAfterYield = false;
        unsigned physicalHostsAtInvoke = 0;
    };
    const auto observation = std::make_shared<Observation>();
    const wxWeakRef<wxWindow> weakDialog(dialog);
    const HWND dialogHwnd = static_cast<HWND>(dialog->GetHWND());
    modalButton->Bind(
        wxEVT_BUTTON,
        [observation, weakDialog, dialogHwnd](wxCommandEvent&)
        {
            const auto state = observation;
            const wxWeakRef<wxWindow> weak = weakDialog;
            wxWindow * const live = weak.get();
            state->globalOnlyCallbackEntered = true;
            state->globalOnlySecondDestroyAccepted =
                live && live->Destroy();
            wxYield();
            state->globalOnlyDialogAliveAfterYield =
                weak.get() != nullptr && ::IsWindow(dialogHwnd);
            state->globalOnlyPhysicalHostAliveAfterYield =
                wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
                    state->physicalHostsAtInvoke;
        });
    driver->Bind(
        wxEVT_BUTTON,
        [observation, weakDialog, dialogHwnd](wxCommandEvent&)
        {
            const auto state = observation;
            wxWindow * const live = weakDialog.get();
            state->callbackEntered = true;
            state->secondDestroyAccepted = live && live->Destroy();
            wxYield();
            state->dialogAliveAfterYield =
                weakDialog.get() != nullptr && ::IsWindow(dialogHwnd);
            state->physicalHostAliveAfterYield =
                wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
                    state->physicalHostsAtInvoke;
        });

    wxTheApp->CallAfter(
        [dialog, modalButton, observation, automationPeer]()
        {
            observation->destroyRequested = dialog->Destroy();
            // The Show(false) path may itself cross a guarded wx event and
            // temporarily move the object out of wxPendingDelete into the
            // host's private replay queue. IsBeingDeleted() is the public
            // logical-destruction contract here; the retained callbacks below
            // prove that physical teardown remains shielded.
            observation->logicalDestroyPendingBeforeInvoke =
                dialog->IsBeingDeleted() &&
                ::IsWindow(static_cast<HWND>(dialog->GetHWND()));
            observation->physicalHostsAtInvoke =
                wxWinUITopLevelHost::GetLiveHostLifetimeCount();

            // Logical shutdown removed the dialog host and invalidated its
            // XAML callbacks, but a wx event already materialized by a
            // retained delegate must still acquire the global-only cookie.
            wxCommandEvent retainedEvent(wxEVT_BUTTON,
                                         modalButton->GetId());
            retainedEvent.SetEventObject(modalButton);
            (void)modalButton->HandleWindowEvent(retainedEvent);

            automationPeer.Invoke();
        });
    (void)dialog->ShowModal();

    CHECK(observation->destroyRequested);
    CHECK(observation->secondDestroyAccepted);
    CHECK(observation->logicalDestroyPendingBeforeInvoke);
    CHECK(observation->globalOnlyCallbackEntered);
    CHECK(observation->globalOnlySecondDestroyAccepted);
    CHECK(observation->globalOnlyDialogAliveAfterYield);
    CHECK(observation->globalOnlyPhysicalHostAliveAfterYield);
    CHECK(observation->callbackEntered);
    CHECK(observation->dialogAliveAfterYield);
    CHECK(observation->physicalHostAliveAfterYield);
    REQUIRE(DrainUntil([&]()
    {
        return weakDialog.get() == nullptr;
    }));

    driverFrame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::CrossHostButtonCallbackDefersDestinationDestroy",
          "[HostLifecycle][HostState][button][reparent]")
{
    namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const source =
        new wxFrame(nullptr, wxID_ANY, "cross-host-source",
                    wxPoint(-32000, -32000), wxSize(240, 140));
    wxFrame * const destination =
        new wxFrame(nullptr, wxID_ANY, "cross-host-destination",
                    wxPoint(-31600, -32000), wxSize(240, 140));
    wxButton * const button =
        new wxButton(source, wxID_ANY, "move-and-destroy",
                     wxPoint(10, 10));
    ShowOffscreenWithoutActivating(source);
    ShowOffscreenWithoutActivating(destination);
    DrainToQuiescence();

    wxWinUITopLevelHost * const sourceHost =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(sourceHost != nullptr);
    wxWinUISlot * const sourceSlot = sourceHost->FindSlot(button);
    REQUIRE(sourceSlot != nullptr);
    const MUXC::Button peer =
        sourceSlot->GetContent().try_as<MUXC::Button>();
    REQUIRE(peer != nullptr);
    const MUXAP::ButtonAutomationPeer automationPeer(peer);

    struct Observation
    {
        wxFrame *destination = nullptr;
        bool handlerEntered = false;
        bool reparented = false;
        bool destinationOwnedSlot = false;
        bool destroyRequested = false;
        bool destinationAliveInsideHandler = false;
        bool invokeReturned = false;
        bool hideEventSeen = false;
        bool hideEventNestedUnderInvoke = false;
        bool hideYieldCompleted = false;
    };
    const auto observation = std::make_shared<Observation>();
    observation->destination = destination;
    const wxWeakRef<wxWindow> destinationLifetime(destination);

    destination->Bind(
        wxEVT_SHOW,
        [observation](wxShowEvent& event)
        {
            const std::shared_ptr<Observation> state = observation;
            event.Skip();
            if ( event.IsShown() )
                return;

            state->hideEventSeen = true;
            state->hideEventNestedUnderInvoke = !state->invokeReturned;
            wxYield();
            state->hideYieldCompleted = true;
        });
    button->Bind(
        wxEVT_BUTTON,
        [observation, button, destinationLifetime](wxCommandEvent&)
        {
            const std::shared_ptr<Observation> state = observation;
            const wxWeakRef<wxWindow> doomedLifetime =
                destinationLifetime;
            state->handlerEntered = true;
            wxFrame * const doomed = state->destination;
            state->reparented = doomed && button->Reparent(doomed);
            wxWinUITopLevelHost * const owner =
                wxWinUITopLevelHost::FindSlotOwner(button);
            state->destinationOwnedSlot =
                doomed && owner ==
                    wxWinUITopLevelHost::FindForTLW(doomed) &&
                owner && owner->FindSlot(button);
            state->destination = nullptr;
            state->destroyRequested = doomed && doomed->Destroy();

            // The destination host has no local operation on this stack: only
            // the global WinUI transaction can prevent this yield from
            // deleting it under the source peer's Click delegate.
            wxYield();
            wxWindow * const liveDestination = doomedLifetime.get();
            state->destinationAliveInsideHandler =
                liveDestination &&
                !liveDestination->IsBeingDeleted() &&
                wxWinUITopLevelHost::FindForTLW(liveDestination) != nullptr;
        });

    automationPeer.Invoke();
    observation->invokeReturned = true;
    CHECK(observation->handlerEntered);
    CHECK(observation->reparented);
    CHECK(observation->destinationOwnedSlot);
    CHECK(observation->destroyRequested);
    CHECK(observation->destinationAliveInsideHandler);
    CHECK_FALSE(observation->hideEventSeen);

    REQUIRE(DrainUntil([&]()
    {
        return destinationLifetime.get() == nullptr;
    }));
    CHECK(observation->hideEventSeen);
    CHECK_FALSE(observation->hideEventNestedUnderInvoke);
    CHECK(observation->hideYieldCompleted);

    source->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::NativeShellConsumedSetStillHandsOff",
          "[HostState]")
{
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "state-shell-set-consumed");
    wxButton * const target =
        new wxButton(frame, wxID_ANY, "target", wxPoint(10, 10));
    wxWindow * const holder =
        new wxWindow(frame, wxID_ANY, wxPoint(180, 10), wxSize(40, 30));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(target);
    REQUIRE(host != nullptr);
    ::SetFocus(static_cast<HWND>(holder->GetHWND()));
    DrainDispatch(2);
    REQUIRE(::GetFocus() == static_cast<HWND>(holder->GetHWND()));

    unsigned childEvents = 0;
    unsigned setEvents = 0;
    unsigned killEvents = 0;
    bool counterpartCorrect = true;
    target->Bind(wxEVT_CHILD_FOCUS,
        [&](wxChildFocusEvent& event)
        {
            ++childEvents;
            counterpartCorrect =
                counterpartCorrect && event.GetWindow() == target;
            event.Skip();
        });
    target->Bind(wxEVT_SET_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++setEvents;
            counterpartCorrect =
                counterpartCorrect && event.GetWindow() == holder;
            // Deliberately no Skip(): focus is not vetoable, and application
            // consumption must not hide the infrastructure hand-off.
        });
    target->Bind(wxEVT_KILL_FOCUS,
        [&](wxFocusEvent&)
        {
            ++killEvents;
        });

    ::SetFocus(static_cast<HWND>(target->GetHWND()));
    REQUIRE(DrainUntil([&]()
    {
        return host->GetFocusOwner() == target &&
               wxWindow::FindFocus() == target;
    }));

    CHECK(childEvents == 1);
    CHECK(setEvents == 1);
    CHECK(killEvents == 0);
    CHECK(counterpartCorrect);
    CHECK(wxWinUITLWHostResolveFocus(
              reinterpret_cast<WXHWND>(::GetFocus())) == target);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::NativeParentChildRedirectSuppressesStaleSet",
          "[HostState][focus]")
{
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "native-parent-child-redirect");
    wxWindow * const parent =
        new wxWindow(frame, wxID_ANY, wxPoint(10, 10), wxSize(120, 80));
    wxWindow * const child =
        new wxWindow(parent, wxID_ANY, wxPoint(10, 10), wxSize(40, 30));
    wxWindow * const holder =
        new wxWindow(frame, wxID_ANY, wxPoint(180, 10), wxSize(40, 30));
    frame->Show();
    DrainDispatch(3);
    ::SetFocus(static_cast<HWND>(holder->GetHWND()));
    REQUIRE(::GetFocus() == static_cast<HWND>(holder->GetHWND()));

    bool redirected = false;
    unsigned parentSets = 0;
    parent->Bind(wxEVT_CHILD_FOCUS,
        [&](wxChildFocusEvent& event)
        {
            if ( !redirected && event.GetWindow() == parent )
            {
                redirected = true;
                ::SetFocus(static_cast<HWND>(child->GetHWND()));
            }
            event.Skip();
        });
    parent->Bind(wxEVT_SET_FOCUS,
        [&](wxFocusEvent& event)
        {
            ++parentSets;
            event.Skip();
        });

    ::SetFocus(static_cast<HWND>(parent->GetHWND()));
    DrainDispatch(3);
    CHECK(redirected);
    CHECK(::GetFocus() == static_cast<HWND>(child->GetHWND()));
    CHECK(parentSets == 0);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::NativeShellSetFocusRedirectIsAuthoritative",
          "[HostState]")
{
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "state-shell-set-redirect");
    wxButton * const target =
        new wxButton(frame, wxID_ANY, "target", wxPoint(10, 10));
    wxWindow * const holder =
        new wxWindow(frame, wxID_ANY, wxPoint(180, 10), wxSize(40, 30));
    wxWindow * const redirect =
        new wxWindow(frame, wxID_ANY, wxPoint(180, 50), wxSize(40, 30));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(target);
    REQUIRE(host != nullptr);
    ::SetFocus(static_cast<HWND>(holder->GetHWND()));
    DrainDispatch(2);
    REQUIRE(::GetFocus() == static_cast<HWND>(holder->GetHWND()));

    unsigned setEvents = 0;
    unsigned killEvents = 0;
    bool counterpartsCorrect = true;
    target->Bind(wxEVT_SET_FOCUS,
        [holder, redirect, &setEvents,
         &counterpartsCorrect](wxFocusEvent& event)
        {
            ++setEvents;
            counterpartsCorrect =
                counterpartsCorrect && event.GetWindow() == holder;
            ::SetFocus(static_cast<HWND>(redirect->GetHWND()));
            // Deliberately consume SET_FOCUS: the post-event host hook still
            // runs, but must observe and respect this native redirection.
        });
    target->Bind(wxEVT_KILL_FOCUS,
        [redirect, &killEvents,
         &counterpartsCorrect](wxFocusEvent& event)
        {
            ++killEvents;
            counterpartsCorrect =
                counterpartsCorrect && event.GetWindow() == redirect;
            event.Skip();
        });

    // Bypass wxWindow::SetFocus() deliberately: this is the shell path used
    // by native dialog/navigation code after its wx SET event was delivered.
    ::SetFocus(static_cast<HWND>(target->GetHWND()));
    DrainDispatch(5);

    CHECK(setEvents == 1);
    CHECK(killEvents == 1);
    CHECK(counterpartsCorrect);
    CHECK(::GetFocus() == static_cast<HWND>(redirect->GetHWND()));
    CHECK(host->GetFocusOwner() != target);
    CHECK(wxWindow::FindFocus() != target);
    // No retained hand-off may steal focus after the handler's decision.
    host->FlushSync();
    DrainDispatch(5);
    CHECK(::GetFocus() == static_cast<HWND>(redirect->GetHWND()));
    CHECK(setEvents == 1);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::LogicalSetFocusRedirectIsAuthoritative",
          "[HostState]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "state-logical-set-redirect");
    wxButton * const first =
        new wxButton(frame, wxID_ANY, "first", wxPoint(10, 10));
    wxButton * const target =
        new wxButton(frame, wxID_ANY, "target", wxPoint(10, 50));
    wxWindow * const redirect =
        new wxWindow(frame, wxID_ANY, wxPoint(180, 50), wxSize(40, 30));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(target);
    REQUIRE(host != nullptr);
    first->SetFocus();
    DrainDispatch(3);
    REQUIRE(host->GetFocusOwner() == first);

    // Replace the peer without a component Loaded hook. This leaves the
    // content generation deterministically non-Loaded while remaining a
    // focusable Control, exercising the synchronous logical-publication path.
    wxWinUISlot * const targetSlot = host->FindSlot(target);
    REQUIRE(targetSlot != nullptr);
    REQUIRE(targetSlot->SetContent(MUXC::AutoSuggestBox()));

    std::vector<wxString> events;
    bool counterpartsCorrect = true;
    first->Bind(wxEVT_KILL_FOCUS,
        [&](wxFocusEvent& event)
        {
            events.push_back("kill-first");
            counterpartsCorrect =
                counterpartsCorrect && event.GetWindow() == target;
            event.Skip();
        });
    target->Bind(wxEVT_CHILD_FOCUS,
        [&](wxChildFocusEvent& event)
        {
            events.push_back("child-target");
            counterpartsCorrect =
                counterpartsCorrect && event.GetWindow() == target;
            event.Skip();
        });
    target->Bind(wxEVT_SET_FOCUS,
        [first, redirect, &events,
         &counterpartsCorrect](wxFocusEvent& event)
        {
            events.push_back("set-target");
            counterpartsCorrect =
                counterpartsCorrect && event.GetWindow() == first;
            ::SetFocus(static_cast<HWND>(redirect->GetHWND()));
            event.Skip();
        });
    target->Bind(wxEVT_KILL_FOCUS,
        [redirect, &events,
         &counterpartsCorrect](wxFocusEvent& event)
        {
            events.push_back("kill-target");
            counterpartsCorrect =
                counterpartsCorrect && event.GetWindow() == redirect;
            event.Skip();
        });

    target->SetFocus();
    DrainDispatch(5);

    REQUIRE(events.size() == 4);
    CHECK(events[0] == "kill-first");
    CHECK(events[1] == "child-target");
    CHECK(events[2] == "set-target");
    CHECK(events[3] == "kill-target");
    CHECK(counterpartsCorrect);
    CHECK(::GetFocus() == static_cast<HWND>(redirect->GetHWND()));
    CHECK(host->GetFocusOwner() != target);
    CHECK(wxWindow::FindFocus() != target);
    host->FlushSync();
    DrainDispatch(5);
    CHECK(::GetFocus() == static_cast<HWND>(redirect->GetHWND()));
    CHECK(events.size() == 4);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::FocusReentrancyUnregister", "[HostState]")
{
    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "state-reentry");
    const wxString choices[] = { "alpha", "beta" };
    wxRadioBox * const radio =
        new wxRadioBox(frame, wxID_ANY, "group", wxDefaultPosition,
                       wxDefaultSize, WXSIZEOF(choices), choices);
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    REQUIRE(host->FindSlot(radio) != nullptr);

    // Give the native focus to a bare (slot-less, non-delegating) child,
    // then unregister the target slot from inside its kill-focus handler:
    // it runs synchronously inside FocusSlot()'s ::SetFocus(bridge), i.e.
    // exactly across the reentrant boundary.  FocusSlot must revalidate
    // and bail out cleanly.  (A TLW cannot hold the focus itself: it
    // delegates it back to a child on WM_SETFOCUS.)
    wxWindow * const holder = new wxWindow(frame, wxID_ANY);
    frame->Raise();
    ::SetFocus(static_cast<HWND>(holder->GetHWND()));
    DrainDispatch(2);
    REQUIRE(::GetFocus() == static_cast<HWND>(holder->GetHWND()));

    bool unregistered = false;
    holder->Bind(wxEVT_KILL_FOCUS,
        [&unregistered, host, radio](wxFocusEvent& event)
        {
            event.Skip();
            if ( !unregistered )
            {
                unregistered = true;
                host->UnregisterSlot(radio);
            }
        });

    host->FocusSlot(radio);
    DrainDispatch(3);

    CHECK(unregistered);
    CHECK(host->FindSlot(radio) == nullptr);

    // The forwarding flag must not have been left set: the arbiter still
    // records focus ownership for the next slot.
    wxButton * const after = new wxButton(frame, wxID_ANY, "after");
    DrainDispatch(3);
    host->FocusSlot(after);
    DrainDispatch(3);
    CHECK(host->GetFocusOwner() == after);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::FocusReentrancyNativeRedirect", "[HostState]")
{
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "state-native-redirect");
    wxButton * const target =
        new wxButton(frame, wxID_ANY, "target", wxPoint(10, 10));
    wxButton * const after =
        new wxButton(frame, wxID_ANY, "after", wxPoint(10, 50));
    // Bare wx windows are native focus targets, not XAML slots. They let the
    // test redirect focus synchronously from the WM_KILLFOCUS raised inside
    // FocusSlot()'s ::SetFocus(bridge) boundary.
    wxWindow * const holder =
        new wxWindow(frame, wxID_ANY, wxPoint(200, 10), wxSize(40, 30));
    wxWindow * const redirect =
        new wxWindow(frame, wxID_ANY, wxPoint(200, 50), wxSize(40, 30));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    REQUIRE(host->FindSlot(target) != nullptr);
    REQUIRE(host->FindSlot(after) != nullptr);

    frame->Raise();
    ::SetFocus(static_cast<HWND>(holder->GetHWND()));
    DrainDispatch(2);
    REQUIRE(::GetFocus() == static_cast<HWND>(holder->GetHWND()));

    bool redirected = false;
    holder->Bind(wxEVT_KILL_FOCUS,
        [redirect, &redirected](wxFocusEvent& event)
        {
            event.Skip();
            if ( !redirected )
            {
                redirected = true;
                ::SetFocus(static_cast<HWND>(redirect->GetHWND()));
            }
        });

    host->FocusSlot(target);
    DrainDispatch(3);

    REQUIRE(redirected);
    CHECK(::GetFocus() == static_cast<HWND>(redirect->GetHWND()));
    CHECK(host->GetFocusOwner() != target);
    CHECK(wxWindow::FindFocus() != target);

    // The superseded request and its one-shot deferred retry must not become
    // visible later, and the next independent intent must still work.
    DrainDispatch(5);
    CHECK(host->GetFocusOwner() != target);
    host->FocusSlot(after);
    DrainDispatch(3);
    CHECK(host->GetFocusOwner() == after);
    CHECK(wxWindow::FindFocus() == after);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::DeferredFocusDoesNotReactivateAnotherTLW",
          "[HostState]")
{
    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "focus-retry-a",
                    wxPoint(-22000, -22000), wxSize(260, 120));
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "focus-retry-b",
                    wxPoint(-21600, -22000), wxSize(260, 120));
    wxButton * const buttonA =
        new wxButton(frameA, wxID_ANY, "a",
                     wxPoint(20, 20), wxSize(100, 32));
    wxButton * const buttonB =
        new wxButton(frameB, wxID_ANY, "b",
                     wxPoint(20, 20), wxSize(100, 32));

    frameA->Show();
    frameB->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const hostA =
        wxWinUITopLevelHost::FindSlotOwner(buttonA);
    wxWinUITopLevelHost * const hostB =
        wxWinUITopLevelHost::FindSlotOwner(buttonB);
    REQUIRE(hostA != nullptr);
    REQUIRE(hostB != nullptr);
    REQUIRE(hostA != hostB);

    // Showing B can supersede A's one-shot template focus retry. Draining the
    // queue must terminate without A/B repeatedly reactivating each other.
    // A fresh explicit request remains authoritative afterwards.
    buttonA->SetFocus();
    DrainToQuiescence();
    CHECK(wxWindow::FindFocus() == buttonA);

    frameA->Destroy();
    frameB->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::KeyboardFocusExclusion", "[HostState]")
{
    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "state-tab");
    wxPanel * const panel = new wxPanel(frame);
    wxButton * const b1 = new wxButton(panel, wxID_ANY, "one",
                                       wxPoint(10, 10));
    wxButton * const b2 = new wxButton(panel, wxID_ANY, "two",
                                       wxPoint(10, 50));
    wxButton * const b3 = new wxButton(panel, wxID_ANY, "three",
                                       wxPoint(10, 90));
    // b2 is enabled but explicitly excluded from keyboard navigation.
    b2->DisableFocusFromKeyboard();
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);

    host->FocusSlot(b1);
    DrainDispatch(3);
    REQUIRE(host->GetFocusOwner() == b1);

    // Exercise the real event-loop fallback. In production an installed
    // WH_GETMESSAGE hook handles the removed message before ProcessMessage();
    // a direct test call must force the fallback or it would bypass the one
    // mixed-navigation arbiter and dispatch Tab straight to XAML.
    const HWND focusHwnd = ::GetFocus();
    REQUIRE(focusHwnd != nullptr);
    MSG tabMsg = {};
    tabMsg.hwnd = focusHwnd;
    tabMsg.message = WM_KEYDOWN;
    tabMsg.wParam = VK_TAB;
    const bool previousFallback =
        wxWinUI3ForceEventLoopFallbackForTesting(true);
    wxGUIEventLoop().ProcessMessage(&tabMsg);
    wxWinUI3ForceEventLoopFallbackForTesting(previousFallback);
    DrainDispatch(5);

    // wx contract: Tab skips a window whose AcceptsFocusFromKeyboard() is
    // false -- the island navigation must not focus b2's XAML content.
    CHECK(host->GetFocusOwner() == b3);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::KeyboardFocusExclusionPaths", "[HostState]")
{
    // The focusability walk itself, on every path and shape the review
    // called out: backward, several refusals in a row, everything refused
    // (bounded, no infinite loop) and the island's TakeFocusRequested exit.
    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "state-tab2");
    wxPanel * const panel = new wxPanel(frame);
    wxButton * const b1 = new wxButton(panel, wxID_ANY, "one",
                                       wxPoint(10, 10));
    wxButton * const b2 = new wxButton(panel, wxID_ANY, "two",
                                       wxPoint(10, 50));
    wxButton * const b3 = new wxButton(panel, wxID_ANY, "three",
                                       wxPoint(10, 90));
    wxButton * const b4 = new wxButton(panel, wxID_ANY, "four",
                                       wxPoint(10, 130));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);

    // backward navigation skips a refused window: b3 -Shift+Tab-> b1.
    // Driven through the take-focus seam: previous=true exercises the same
    // shared walk backward (a posted VK_SHIFT never reaches GetKeyState).
    b2->DisableFocusFromKeyboard();
    host->FocusSlot(b3);
    DrainDispatch(3);
    REQUIRE(host->GetFocusOwner() == b3);
    host->TestTakeFocusRequested(true /* previous */);
    DrainDispatch(3);
    CHECK(host->GetFocusOwner() == b1);

    // two refused windows in a row: b1 -Tab-> b4 (via TakeFocusRequested,
    // the island's own exit path -- not only the WM_KEYDOWN one)
    b3->DisableFocusFromKeyboard();
    host->FocusSlot(b1);
    DrainDispatch(3);
    REQUIRE(host->GetFocusOwner() == b1);
    host->TestTakeFocusRequested(false /* forward */);
    DrainDispatch(3);
    CHECK(host->GetFocusOwner() == b4);

    // everything else refused: the walk must terminate and stay put
    b4->DisableFocusFromKeyboard();
    host->FocusSlot(b1);
    DrainDispatch(3);
    REQUIRE(host->GetFocusOwner() == b1);
    const HWND nativeFocusBeforeExhaustedWalk = ::GetFocus();
    REQUIRE(nativeFocusBeforeExhaustedWalk != nullptr);
    host->TestTakeFocusRequested(false);
    // The TLW also contains private WinAppSDK WS_TABSTOP helpers. They are not
    // wx tab stops: an exhausted logical walk must leave both native and
    // logical focus exactly where they were.
    CHECK(::GetFocus() == nativeFocusBeforeExhaustedWalk);
    CHECK(host->GetFocusOwner() == b1);
    DrainDispatch(3);
    CHECK(host->GetFocusOwner() == b1);
    CHECK(wxWindow::FindFocus() == b1);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::ContentSwapFaultPhases", "[HostState]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "state-phases");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    wxWinUISlot *slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto oldContainer = slot->GetContainer();
    REQUIRE(oldContainer != nullptr);
    const auto oldContent = slot->GetContent();
    REQUIRE(oldContent != nullptr);
    const MUXC::Border replacement;

    const auto carrierNow =
        [&slot]() -> winrt::Microsoft::UI::Xaml::UIElement
        {
            winrt::Microsoft::UI::Xaml::UIElement element{ nullptr };
            REQUIRE(slot->TryGetCarrierContent(element));
            return element;
        };

    // 1. detach fails: NOTHING may change, model and carrier still agree
    wxWinUITopLevelHost::TestFailContentSwap(
        wxWinUITopLevelHost::TestContentFault_Detach);
    CHECK(!slot->SetContent(replacement));
    CHECK(slot->GetContent() == oldContent);
    CHECK(carrierNow() == oldContent);

    // 2. install fails: the old content is restored, coherently
    wxWinUITopLevelHost::TestFailContentSwap(
        wxWinUITopLevelHost::TestContentFault_Install);
    CHECK(!slot->SetContent(replacement));
    CHECK(slot->GetContent() == oldContent);
    CHECK(carrierNow() == oldContent);

    // 3. install AND restore fail: the model must mirror the REAL carrier
    // state (readably empty, since the detach succeeded) -- never guess
    wxWinUITopLevelHost::TestFailContentSwap(
        wxWinUITopLevelHost::TestContentFault_Install |
        wxWinUITopLevelHost::TestContentFault_Restore);
    CHECK(!slot->SetContent(replacement));
    CHECK(slot->GetContent() == nullptr);
    CHECK(carrierNow() == nullptr);
    DrainDispatch(3);

    // The carrier is now truthfully poisoned: callbacks were invalidated and
    // this slot must never be reused. A fresh registration disposes it,
    // installs a new lifetime/carrier and remains fully usable.
#if wxUSE_TOOLTIPS
    xamlHost->SetToolTip("phases");
#endif
    slot = host->RegisterSlot(xamlHost, replacement);
    REQUIRE(slot != nullptr);
    CHECK(host->FindSlot(xamlHost) == slot);
    CHECK(slot->GetContainer() != oldContainer);
    CHECK(slot->GetContent() == replacement);
    CHECK(carrierNow() == replacement);
    DrainDispatch(3);
#if wxUSE_TOOLTIPS
    const auto tip = MUXC::ToolTipService::GetToolTip(slot->GetContent());
    REQUIRE(tip != nullptr);
    CHECK(GetToolTipInspectableText(tip) == "phases");
#endif

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::XamlAuthoredNamePreserved", "[HostState]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    using winrt::Microsoft::UI::Xaml::Automation::AutomationProperties;

    // A UIA name authored in the XAML itself, BEFORE the slot registration:
    // neither the registration nor any later state sync may clobber it.
    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "state-xamlname");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    // Equality with the wx label is the historically ambiguous case: it must
    // still be classified as authored because it existed before registration.
    xamlHost->SetLabel("xaml-name");
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" AutomationProperties.Name=\"xaml-name\" "
        "Content=\"n\"/>"));
    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    CHECK(wxString(AutomationProperties::GetName(slot->GetContent()).c_str())
              == "xaml-name");

    // A different desired wx label must not claim the authored property.
    xamlHost->SetLabel("changed-wx-label");
    xamlHost->Move(5, 5);
    DrainDispatch(3);
    CHECK(wxString(AutomationProperties::GetName(slot->GetContent()).c_str())
              == "xaml-name");

    // Clearing an authored value is also an authored decision. It remains
    // empty across later wx label changes for this content generation.
    AutomationProperties::SetName(slot->GetContent(), L"");
    xamlHost->SetLabel("must-not-reclaim");
    DrainDispatch(3);
    CHECK(AutomationProperties::GetName(slot->GetContent()).empty());

    // Empty can also be authored before registration. ReadLocalValue() is the
    // only way to distinguish this deliberate suppression from an unset Name.
    wxWinUIXamlHost * const emptyNameHost =
        new wxWinUIXamlHost(frame);
    emptyNameHost->SetLabel("must never be claimed");
    REQUIRE(emptyNameHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" AutomationProperties.Name=\"\" Content=\"empty\"/>"));
    wxWinUITopLevelHost * const emptyOwner =
        wxWinUITopLevelHost::FindSlotOwner(emptyNameHost);
    REQUIRE(emptyOwner != nullptr);
    wxWinUISlot * const emptySlot =
        emptyOwner->FindSlot(emptyNameHost);
    REQUIRE(emptySlot != nullptr);
    const auto emptyContent = emptySlot->GetContent();
    REQUIRE(emptyContent != nullptr);
    const auto nameProperty = AutomationProperties::NameProperty();
    REQUIRE(emptyContent.ReadLocalValue(nameProperty) !=
            MUX::DependencyProperty::UnsetValue());
    CHECK(AutomationProperties::GetName(emptyContent).empty());

    emptyNameHost->SetLabel("still must not be claimed");
    emptyOwner->FlushSync();
    CHECK(AutomationProperties::GetName(emptyContent).empty());

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::AuthoredAutomationRelationsSurviveSyncAndMigration",
          "[HostState][HostLifecycle][accessibility][winui-009]"
          "[uia-topology][relations][migration]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = MUX::Automation;
    namespace MUXAP = MUXA::Peers;
    namespace MUXC = MUX::Controls;

    DrainToQuiescence();
    const unsigned observersBefore =
        wxWinUITopLevelHost::
            GetLiveAutomationNameStyleObserverCountForTest();

    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "uia-relations-A",
                    wxPoint(-32000, -32000), wxSize(300, 180));
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "uia-relations-B",
                    wxPoint(-31680, -32000), wxSize(300, 180));
    wxWinUIXamlHost * const xamlHost =
        new wxWinUIXamlHost(frameA);
    xamlHost->SetLabel("wx fallback");
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    ShowOffscreenWithoutActivating(frameA, 0);
    ShowOffscreenWithoutActivating(frameB, 1);
    DrainToQuiescence();

    wxWinUITopLevelHost *host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot *slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    REQUIRE(
        wxWinUITopLevelHost::
            GetLiveAutomationNameStyleObserverCountForTest() ==
        observersBefore + 1);

    const MUXC::Grid root;
    const MUXC::TextBlock labelA;
    const MUXC::Button targetA;
    const MUXC::TextBlock labelB;
    const MUXC::Button targetB;
    const MUXC::TextBlock description;
    const MUXC::Button flowPredecessor;
    const MUXC::Button flowSuccessor;
    const MUXC::Button controlledPeer;
    root.Children().Append(labelA);
    root.Children().Append(targetA);
    root.Children().Append(labelB);
    root.Children().Append(targetB);
    root.Children().Append(description);
    root.Children().Append(flowPredecessor);
    root.Children().Append(flowSuccessor);
    root.Children().Append(controlledPeer);

    MUXA::AutomationProperties::SetName(labelA, L"authored label A");
    MUXA::AutomationProperties::SetName(labelB, L"authored label B");
    MUXA::AutomationProperties::SetLabeledBy(targetA, labelA);
    MUXA::AutomationProperties::SetLabeledBy(targetB, labelB);

    const auto describedByA =
        MUXA::AutomationProperties::GetDescribedBy(targetA);
    const auto flowsFromA =
        MUXA::AutomationProperties::GetFlowsFrom(targetA);
    const auto flowsToA =
        MUXA::AutomationProperties::GetFlowsTo(targetA);
    const auto controlledPeersA =
        MUXA::AutomationProperties::GetControlledPeers(targetA);
    REQUIRE(describedByA != nullptr);
    REQUIRE(flowsFromA != nullptr);
    REQUIRE(flowsToA != nullptr);
    REQUIRE(controlledPeersA != nullptr);
    describedByA.Append(description);
    flowsFromA.Append(flowPredecessor);
    flowsToA.Append(flowSuccessor);
    controlledPeersA.Append(controlledPeer);

    const auto describedByB =
        MUXA::AutomationProperties::GetDescribedBy(targetB);
    const auto flowsFromB =
        MUXA::AutomationProperties::GetFlowsFrom(targetB);
    const auto flowsToB =
        MUXA::AutomationProperties::GetFlowsTo(targetB);
    const auto controlledPeersB =
        MUXA::AutomationProperties::GetControlledPeers(targetB);
    REQUIRE(describedByB != nullptr);
    REQUIRE(flowsFromB != nullptr);
    REQUIRE(flowsToB != nullptr);
    REQUIRE(controlledPeersB != nullptr);
    describedByB.Append(description);
    flowsFromB.Append(flowPredecessor);
    flowsToB.Append(flowSuccessor);
    controlledPeersB.Append(controlledPeer);

    const auto checkRelations =
        [&](const MUX::UIElement& target,
            const MUX::UIElement& label,
            const auto& describedBy,
            const auto& flowsFrom,
            const auto& flowsTo,
            const auto& controlledPeers)
        {
            CHECK(MUXA::AutomationProperties::GetLabeledBy(target) ==
                  label);
            CHECK(MUXA::AutomationProperties::GetDescribedBy(target) ==
                  describedBy);
            REQUIRE(describedBy.Size() == 1);
            CHECK(describedBy.GetAt(0) == description);
            CHECK(MUXA::AutomationProperties::GetFlowsFrom(target) ==
                  flowsFrom);
            REQUIRE(flowsFrom.Size() == 1);
            CHECK(flowsFrom.GetAt(0) == flowPredecessor);
            CHECK(MUXA::AutomationProperties::GetFlowsTo(target) ==
                  flowsTo);
            REQUIRE(flowsTo.Size() == 1);
            CHECK(flowsTo.GetAt(0) == flowSuccessor);
            CHECK(MUXA::AutomationProperties::GetControlledPeers(target) ==
                  controlledPeers);
            REQUIRE(controlledPeers.Size() == 1);
            CHECK(controlledPeers.GetAt(0) == controlledPeer);
        };

    const auto checkRelationsA = [&]()
    {
        checkRelations(
            targetA, labelA, describedByA, flowsFromA, flowsToA,
            controlledPeersA);
    };
    const auto checkRelationsB = [&]()
    {
        checkRelations(
            targetB, labelB, describedByB, flowsFromB, flowsToB,
            controlledPeersB);
    };

    const auto nameProperty =
        MUXA::AutomationProperties::NameProperty();
    const auto nameIsUnset = [&](const MUX::UIElement& target)
    {
        return target.ReadLocalValue(nameProperty) ==
                   MUX::DependencyProperty::UnsetValue() &&
               MUXA::AutomationProperties::GetName(target).empty();
    };

    // The relation is authored before attachment. The common adapter must
    // preserve all typed relations and keep its scalar fallback Name unset.
    REQUIRE(slot->SetContent(root, targetA));
    host->FlushSync();
    REQUIRE(nameIsUnset(targetA));
    checkRelationsA();
    checkRelationsB();
    const MUXAP::AutomationPeer labelAPeer =
        MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(
            labelA);
    const MUXAP::AutomationPeer targetAPeer =
        MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(
            targetA);
    REQUIRE(labelAPeer != nullptr);
    REQUIRE(targetAPeer != nullptr);
    CHECK(targetAPeer.GetLabeledBy() == labelAPeer);
    CHECK(targetAPeer.GetName() == L"authored label A");
    CHECK(
        wxWinUITopLevelHost::
            GetLiveAutomationNameStyleObserverCountForTest() ==
        observersBefore + 1);

    // A label update changes the object referred to by LabeledBy, not the
    // relation itself, and must not make wx publish a competing Name.
    MUXA::AutomationProperties::SetName(
        labelA, L"authored label A updated");
    xamlHost->SetLabel("wx fallback updated");
    host->FlushSync();
    CHECK(nameIsUnset(targetA));
    checkRelationsA();
    CHECK(targetAPeer.GetLabeledBy() == labelAPeer);
    CHECK(targetAPeer.GetName() == L"authored label A updated");

    // The LabeledBy observer must be sufficient by itself: removing the
    // relation restores the current wx fallback, and adding it back releases
    // only that exact wx-owned local scalar.
    MUXA::AutomationProperties::SetLabeledBy(
        targetA, MUX::UIElement{ nullptr });
    REQUIRE(DrainUntil(
        [&]()
        {
            return targetA.ReadLocalValue(nameProperty) !=
                       MUX::DependencyProperty::UnsetValue() &&
                   MUXA::AutomationProperties::GetName(targetA) ==
                       L"wx fallback updated";
        }));
    MUXA::AutomationProperties::SetLabeledBy(targetA, labelA);
    REQUIRE(DrainUntil([&]() { return nameIsUnset(targetA); }));
    checkRelationsA();
    CHECK(targetAPeer.GetName() == L"authored label A updated");

    // Moving the semantic target is a full content transaction. Neither the
    // old nor the new element may lose its authored relation collections.
    REQUIRE(slot->SetContent(root, targetB));
    host->FlushSync();
    CHECK(slot->GetSemanticTarget() == targetB);
    CHECK(nameIsUnset(targetA));
    CHECK(nameIsUnset(targetB));
    checkRelationsA();
    checkRelationsB();
    CHECK(
        wxWinUITopLevelHost::
            GetLiveAutomationNameStyleObserverCountForTest() ==
        observersBefore + 1);

    MUXA::AutomationProperties::SetLabeledBy(
        targetB, MUX::UIElement{ nullptr });
    REQUIRE(DrainUntil(
        [&]()
        {
            return MUXA::AutomationProperties::GetName(targetB) ==
                   L"wx fallback updated";
        }));
    MUXA::AutomationProperties::SetLabeledBy(targetB, labelB);
    REQUIRE(DrainUntil([&]() { return nameIsUnset(targetB); }));

    // Cross-TLW migration rebuilds the slot and its observer while retaining
    // the exact application-owned graph and the reversible suppression state.
    REQUIRE(xamlHost->Reparent(frameB));
    DrainToQuiescence();
    host = wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    REQUIRE(host == wxWinUITopLevelHost::FindForTLW(frameB));
    slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    CHECK(slot->GetVisualRoot() == root);
    CHECK(slot->GetSemanticTarget() == targetB);
    CHECK(nameIsUnset(targetB));
    checkRelationsA();
    checkRelationsB();
    CHECK(
        wxWinUITopLevelHost::
            GetLiveAutomationNameStyleObserverCountForTest() ==
        observersBefore + 1);

    xamlHost->SetLabel("wx fallback after migration");
    MUXA::AutomationProperties::SetLabeledBy(
        targetB, MUX::UIElement{ nullptr });
    REQUIRE(DrainUntil(
        [&]()
        {
            return MUXA::AutomationProperties::GetName(targetB) ==
                   L"wx fallback after migration";
        }));

    // If application code replaces wx's local scalar immediately before
    // publishing LabeledBy, the adapter must remove neither value: only the
    // exact scalar last written by wx is eligible for reversible release.
    MUXA::AutomationProperties::SetName(
        targetB, L"application-owned target name");
    MUXA::AutomationProperties::SetLabeledBy(targetB, labelB);
    REQUIRE(DrainUntil(
        [&]()
        {
            return MUXA::AutomationProperties::GetLabeledBy(targetB) ==
                       labelB &&
                   MUXA::AutomationProperties::GetName(targetB) ==
                       L"application-owned target name";
        }));
    CHECK(targetB.ReadLocalValue(nameProperty) !=
          MUX::DependencyProperty::UnsetValue());
    checkRelationsB();

    // Detach must revoke the exact relation observer. The retained content
    // can then be mutated without keeping its old slot state alive or
    // scheduling work on a dead host.
    xamlHost->Destroy();
    DrainDispatch();
    REQUIRE(
        wxWinUITopLevelHost::
            GetLiveAutomationNameStyleObserverCountForTest() ==
        observersBefore);
    const unsigned schedulesAfterDetach =
        wxWinUITopLevelHost::GetFlushScheduleCount();
    MUXA::AutomationProperties::SetLabeledBy(
        targetB, MUX::UIElement{ nullptr });
    DrainDispatch(3);
    CHECK(MUXA::AutomationProperties::GetName(targetB) ==
          L"application-owned target name");
    CHECK(wxWinUITopLevelHost::GetFlushScheduleCount() ==
          schedulesAfterDetach);
    CHECK(
        wxWinUITopLevelHost::
            GetLiveAutomationNameStyleObserverCountForTest() ==
        observersBefore);
    CHECK(MUXA::AutomationProperties::GetDescribedBy(targetB) ==
          describedByB);
    CHECK(MUXA::AutomationProperties::GetFlowsFrom(targetB) ==
          flowsFromB);
    CHECK(MUXA::AutomationProperties::GetFlowsTo(targetB) ==
          flowsToB);
    CHECK(MUXA::AutomationProperties::GetControlledPeers(targetB) ==
          controlledPeersB);

    frameA->Destroy();
    frameB->Destroy();
    DrainDispatch();
}

TEST_CASE(
    "HostState::AutomationNamePublishProvenancePrecedesReentrantFlush",
    "[HostState][HostLifecycle][accessibility][winui-009]"
    "[uia-topology][reentrancy]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = MUX::Automation;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "uia-name-publish-reentrant",
                    wxPoint(-32000, -32000), wxSize(300, 180));
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    xamlHost->SetLabel("name A");
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const MUX::UIElement content = slot->GetSemanticTarget();
    REQUIRE(content != nullptr);
    const auto nameProperty = MUXA::AutomationProperties::NameProperty();
    REQUIRE(MUXA::AutomationProperties::GetName(content) == L"name A");

    bool inCallback = false;
    unsigned nestedFlushes = 0;
    const int64_t flushToken = content.RegisterPropertyChangedCallback(
        nameProperty,
        [&](const MUX::DependencyObject&,
            const MUX::DependencyProperty&)
        {
            if ( inCallback ||
                 MUXA::AutomationProperties::GetName(content) !=
                     L"name B" )
            {
                return;
            }

            inCallback = true;
            ++nestedFlushes;
            xamlHost->SetLabel("name C");
            host->MarkDirty(xamlHost);
            host->FlushSync();
            inCallback = false;
        });

    // SetName("name B") fires the callback before the outer SyncSlot pass
    // returns. The nested pass must observe the already-published wx
    // provenance, not misclassify the transient B scalar as application-owned.
    xamlHost->SetLabel("name B");
    host->FlushSync();
    REQUIRE(nestedFlushes == 1);
    CHECK(MUXA::AutomationProperties::GetName(content) == L"name C");
    content.UnregisterPropertyChangedCallback(nameProperty, flushToken);

    xamlHost->SetLabel("name C2");
    host->FlushSync();
    CHECK(MUXA::AutomationProperties::GetName(content) == L"name C2");

    // Destruction/unregistration can happen from the same setter boundary.
    // DisconnectSlot() must recognize D as wx-owned and remove it from the
    // retained application element exactly once.
    bool detachCallbackRan = false;
    const int64_t detachToken = content.RegisterPropertyChangedCallback(
        nameProperty,
        [&](const MUX::DependencyObject&,
            const MUX::DependencyProperty&)
        {
            if ( detachCallbackRan ||
                 MUXA::AutomationProperties::GetName(content) !=
                     L"name D" )
            {
                return;
            }

            detachCallbackRan = true;
            host->UnregisterSlot(xamlHost);
        });
    xamlHost->SetLabel("name D");
    host->FlushSync();
    content.UnregisterPropertyChangedCallback(nameProperty, detachToken);

    REQUIRE(detachCallbackRan);
    REQUIRE(DrainUntil(
        [&]()
        {
            return host->FindSlot(xamlHost) == nullptr &&
                   content.ReadLocalValue(nameProperty) ==
                       MUX::DependencyProperty::UnsetValue() &&
                   MUXA::AutomationProperties::GetName(content).empty();
        }));

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE(
    "HostState::AutomationNameClearDuringDetachSurvivesMigration",
    "[HostState][HostLifecycle][accessibility][winui-009]"
    "[uia-topology][migration][reentrancy]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = MUX::Automation;

    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "uia-detach-name-A",
                    wxPoint(-32000, -32000), wxSize(300, 180));
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "uia-detach-name-B",
                    wxPoint(-31680, -32000), wxSize(300, 180));
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frameA);
    xamlHost->SetLabel("wx before migration");
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    ShowOffscreenWithoutActivating(frameA, 0);
    ShowOffscreenWithoutActivating(frameB, 1);
    DrainToQuiescence();

    wxWinUITopLevelHost *host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot *slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const MUX::UIElement content = slot->GetSemanticTarget();
    REQUIRE(content != nullptr);
    const auto nameProperty = MUXA::AutomationProperties::NameProperty();
    REQUIRE(MUXA::AutomationProperties::GetName(content) ==
            L"wx before migration");

    bool transactionCallbackRan = false;
    wxWinUITopLevelHost::TestOnNextContentTransactionStarted(
        [&](wxWindow *window)
        {
            REQUIRE(window == xamlHost);
            transactionCallbackRan = true;
            REQUIRE(slot->IsContentTransactionInProgressForTest());

            // The final value is indistinguishable from an untouched unset
            // property. The transaction observer is what preserves this
            // deliberate application ClearValue across ResetContentCaches().
            MUXA::AutomationProperties::SetName(
                content, L"application transient");
            content.ClearValue(nameProperty);
        });
    REQUIRE(xamlHost->Reparent(frameB));
    wxWinUITopLevelHost::TestOnNextContentTransactionStarted({});
    DrainToQuiescence();
    REQUIRE(transactionCallbackRan);

    host = wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host == wxWinUITopLevelHost::FindForTLW(frameB));
    slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    CHECK(slot->GetSemanticTarget() == content);
    CHECK(content.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(content).empty());

    xamlHost->SetLabel("must not reclaim after migration");
    host->FlushSync();
    CHECK(content.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(content).empty());

    frameA->Destroy();
    frameB->Destroy();
    DrainDispatch();
}

TEST_CASE(
    "HostState::AutomationNameClearDuringFailedInstallSurvivesRollback",
    "[HostState][HostLifecycle][accessibility][winui-009]"
    "[uia-topology][rollback][reentrancy]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = MUX::Automation;
    namespace MUXC = MUX::Controls;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "uia-rollback-name",
                    wxPoint(-32000, -32000), wxSize(300, 180));
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    xamlHost->SetLabel("wx before rollback");
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const MUX::UIElement content = slot->GetSemanticTarget();
    REQUIRE(content != nullptr);
    const auto nameProperty = MUXA::AutomationProperties::NameProperty();
    REQUIRE(MUXA::AutomationProperties::GetName(content) ==
            L"wx before rollback");

    bool transactionCallbackRan = false;
    wxWinUITopLevelHost::TestOnNextContentTransactionStarted(
        [&](wxWindow *window)
        {
            REQUIRE(window == xamlHost);
            transactionCallbackRan = true;
            REQUIRE(slot->IsContentTransactionInProgressForTest());
            MUXA::AutomationProperties::SetName(
                content, L"application transient");
            content.ClearValue(nameProperty);
        });
    wxWinUITopLevelHost::TestFailContentSwap(
        wxWinUITopLevelHost::TestContentFault_Install);
    const MUXC::Button replacement;
    CHECK_FALSE(slot->SetContent(replacement));
    wxWinUITopLevelHost::TestOnNextContentTransactionStarted({});
    REQUIRE(transactionCallbackRan);

    CHECK(slot->GetSemanticTarget() == content);
    CHECK(content.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(content).empty());
    xamlHost->SetLabel("must not reclaim after rollback");
    host->FlushSync();
    CHECK(content.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(content).empty());

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE(
    "HostState::AutomationNameLabeledByRestorePrecedesReentrantFlush",
    "[HostState][HostLifecycle][accessibility][winui-009]"
    "[uia-topology][relations][reentrancy]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = MUX::Automation;
    namespace MUXC = MUX::Controls;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "uia-labeledby-reentrant",
                    wxPoint(-32000, -32000), wxSize(300, 180));
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    xamlHost->SetLabel("wx fallback");
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"target\"/>"));
    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const MUX::UIElement content = slot->GetSemanticTarget();
    REQUIRE(content != nullptr);
    const auto nameProperty = MUXA::AutomationProperties::NameProperty();
    REQUIRE(MUXA::AutomationProperties::GetName(content) ==
            L"wx fallback");

    const MUXC::TextBlock label;
    MUXA::AutomationProperties::SetName(label, L"authored relation label");

    bool inCallback = false;
    unsigned nestedFlushes = 0;
    const int64_t token = content.RegisterPropertyChangedCallback(
        nameProperty,
        [&](const MUX::DependencyObject&,
            const MUX::DependencyProperty&)
        {
            if ( inCallback ||
                 content.ReadLocalValue(nameProperty) !=
                     MUX::DependencyProperty::UnsetValue() ||
                 !MUXA::AutomationProperties::GetLabeledBy(content) )
            {
                return;
            }

            inCallback = true;
            ++nestedFlushes;
            host->MarkDirty(xamlHost);
            host->FlushSync();
            inCallback = false;
        });

    MUXA::AutomationProperties::SetLabeledBy(content, label);
    host->FlushSync();
    REQUIRE(nestedFlushes == 1);
    CHECK(content.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetLabeledBy(content) == label);
    CHECK(MUXA::AutomationProperties::GetName(content).empty());

    // Suppression by a typed relationship is reversible: once LabeledBy is
    // removed, the current wx fallback must be claimable again.
    MUXA::AutomationProperties::SetLabeledBy(
        content, MUX::UIElement{ nullptr });
    host->FlushSync();
    CHECK(MUXA::AutomationProperties::GetLabeledBy(content) == nullptr);
    CHECK(MUXA::AutomationProperties::GetName(content) ==
          L"wx fallback");
    CHECK(content.ReadLocalValue(nameProperty) !=
          MUX::DependencyProperty::UnsetValue());
    content.UnregisterPropertyChangedCallback(nameProperty, token);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::XamlStyledNamePreserved",
          "[HostState][HostLifecycle][accessibility]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;

    const auto nameProperty = MUXA::AutomationProperties::NameProperty();
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "state-xaml-styled-name");

    // A Style setter is not a local dependency-property value. The slot must
    // inspect the explicit Style/BasedOn chain before contributing its label,
    // including when the authored value is deliberately empty.
    wxWinUIXamlHost * const styledHost =
        new wxWinUIXamlHost(frame);
    styledHost->SetLabel("must-not-mask-style");
    REQUIRE(styledHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"styled\">"
        "<Button.Style><Style TargetType=\"Button\">"
        "<Setter Property=\"AutomationProperties.Name\" "
        "Value=\"styled-name\"/>"
        "</Style></Button.Style></Button>"));

    wxWinUIXamlHost * const emptyBasedOnHost =
        new wxWinUIXamlHost(frame);
    emptyBasedOnHost->SetLabel("must-not-fill-empty-style");
    REQUIRE(emptyBasedOnHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" "
        "xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\" "
        "Content=\"empty-style\">"
        "<Button.Resources>"
        "<Style x:Key=\"AutomationBase\" TargetType=\"Button\">"
        "<Setter Property=\"AutomationProperties.Name\" Value=\"\"/>"
        "</Style>"
        "</Button.Resources>"
        "<Button.Style>"
        "<Style TargetType=\"Button\" "
        "BasedOn=\"{StaticResource AutomationBase}\"/>"
        "</Button.Style></Button>"));

    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(styledHost);
    REQUIRE(host != nullptr);
    wxWinUISlot * const styledSlot = host->FindSlot(styledHost);
    wxWinUISlot * const emptySlot = host->FindSlot(emptyBasedOnHost);
    REQUIRE(styledSlot != nullptr);
    REQUIRE(emptySlot != nullptr);
    const auto styledContent = styledSlot->GetContent();
    const auto emptyContent = emptySlot->GetContent();
    REQUIRE(styledContent != nullptr);
    REQUIRE(emptyContent != nullptr);

    CHECK(MUXA::AutomationProperties::GetName(styledContent) ==
          L"styled-name");
    CHECK(styledContent.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(emptyContent).empty());
    CHECK(emptyContent.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());

    styledHost->SetLabel("later-wx-label");
    emptyBasedOnHost->SetLabel("later-empty-wx-label");
    host->FlushSync();
    CHECK(MUXA::AutomationProperties::GetName(styledContent) ==
          L"styled-name");
    CHECK(styledContent.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(emptyContent).empty());
    CHECK(emptyContent.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());

    // Detaching the element must preserve both its Style and the exact local
    // UnsetValue; no wx fallback may leak onto retained application content.
    styledHost->Destroy();
    emptyBasedOnHost->Destroy();
    DrainDispatch();
    CHECK(MUXA::AutomationProperties::GetName(styledContent) ==
          L"styled-name");
    CHECK(styledContent.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(emptyContent).empty());
    CHECK(emptyContent.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::LateXamlStyleRelinquishesWxName",
          "[HostState][HostLifecycle][accessibility]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
    namespace MUXM = winrt::Microsoft::UI::Xaml::Markup;

    const auto nameProperty = MUXA::AutomationProperties::NameProperty();
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "state-late-xaml-style-name");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    xamlHost->SetLabel("wx-owned-name");
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"late-style\"/>"));
    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto content = slot->GetContent();
    REQUIRE(content != nullptr);
    const auto framework = content.as<MUX::FrameworkElement>();

    REQUIRE(content.ReadLocalValue(nameProperty) !=
            MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(content) ==
          L"wx-owned-name");

    // An unrelated Style change must not make the slot abandon its valid
    // fallback merely because StyleProperty itself changed.
    const auto neutralStyleSource =
        MUXM::XamlReader::Load(
            L"<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/"
            L"xaml/presentation\"><Button.Style>"
            L"<Style TargetType=\"Button\">"
            L"<Setter Property=\"MinWidth\" Value=\"17\"/>"
            L"</Style></Button.Style></Button>")
            .as<MUXC::Button>();
    framework.Style(neutralStyleSource.Style());
    DrainToQuiescence();
    CHECK(MUXA::AutomationProperties::GetName(content) ==
          L"wx-owned-name");
    CHECK(content.ReadLocalValue(nameProperty) !=
          MUX::DependencyProperty::UnsetValue());

    // Assigning an authored Name through Style does not change the effective
    // value while wx's local scalar masks it. The StyleProperty observer must
    // nevertheless schedule reconciliation without an explicit MarkDirty().
    const auto namedStyleSource =
        MUXM::XamlReader::Load(
            L"<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/"
            L"xaml/presentation\"><Button.Style>"
            L"<Style TargetType=\"Button\">"
            L"<Setter Property=\"AutomationProperties.Name\" "
            L"Value=\"late-style-name\"/>"
            L"</Style></Button.Style></Button>")
            .as<MUXC::Button>();
    framework.Style(namedStyleSource.Style());
    REQUIRE(DrainUntil(
        [&]()
        {
            return content.ReadLocalValue(nameProperty) ==
                       MUX::DependencyProperty::UnsetValue() &&
                   MUXA::AutomationProperties::GetName(content) ==
                       L"late-style-name";
        }));

    xamlHost->SetLabel("must-not-reclaim-late-style");
    host->FlushSync();
    CHECK(content.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(content) ==
          L"late-style-name");

    xamlHost->Destroy();
    DrainDispatch();
    CHECK(content.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(content) ==
          L"late-style-name");

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::ImplicitXamlStyleRelinquishesWxName",
          "[HostState][HostLifecycle][accessibility]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
    namespace MUXM = winrt::Microsoft::UI::Xaml::Markup;

    const auto nameProperty = MUXA::AutomationProperties::NameProperty();

    // Let XamlReader create the real implicit-style key. The dictionary is
    // scoped to this slot container and uses otherwise-unused control types:
    // WinUI can cache an implicit Style after its dictionary is removed.
    const MUX::ResourceDictionary testResources =
        MUXM::XamlReader::Load(
            L"<ResourceDictionary "
            L"xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
            L"presentation\" "
            L"xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\">"
            L"<Style x:Key=\"ImplicitNameBase\" TargetType=\"Viewbox\">"
            L"<Setter Property=\"AutomationProperties.Name\" Value=\"\"/>"
            L"</Style>"
            L"<Style TargetType=\"Viewbox\" "
            L"BasedOn=\"{StaticResource ImplicitNameBase}\">"
            L"<Setter Property=\"MinWidth\" Value=\"321\"/>"
            L"</Style>"
            L"<Style x:Key=\"NullImplicitNameBase\" "
            L"TargetType=\"ItemsWrapGrid\">"
            L"<Setter Property=\"AutomationProperties.Name\" Value=\"\"/>"
            L"</Style>"
            L"<Style TargetType=\"ItemsWrapGrid\" "
            L"BasedOn=\"{StaticResource NullImplicitNameBase}\">"
            L"<Setter Property=\"MinWidth\" Value=\"323\"/>"
            L"</Style>"
            L"</ResourceDictionary>")
            .as<MUX::ResourceDictionary>();
    const MUX::ResourceDictionary lateResources =
        MUXM::XamlReader::Load(
            L"<ResourceDictionary "
            L"xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
            L"presentation\" "
            L"xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\">"
            L"<Style x:Key=\"LateImplicitNameBase\" "
            L"TargetType=\"VariableSizedWrapGrid\">"
            L"<Setter Property=\"AutomationProperties.Name\" Value=\"\"/>"
            L"</Style>"
            L"<Style TargetType=\"VariableSizedWrapGrid\" "
            L"BasedOn=\"{StaticResource LateImplicitNameBase}\">"
            L"<Setter Property=\"MinWidth\" Value=\"322\"/>"
            L"</Style>"
            L"</ResourceDictionary>")
            .as<MUX::ResourceDictionary>();
    const auto viewboxStyleKey =
        winrt::box_value(winrt::xaml_typename<MUXC::Viewbox>());
    REQUIRE(testResources.HasKey(viewboxStyleKey));

    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "uia-implicit-style-A");
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "uia-implicit-style-B");

    wxWinUIXamlHost * const implicitHost = new wxWinUIXamlHost(frameA);
    REQUIRE(implicitHost->SetContentFromXaml(
        "<Grid xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    ShowOffscreenWithoutActivating(frameA);
    ShowOffscreenWithoutActivating(frameB, 1);
    DrainToQuiescence();

    wxWinUITopLevelHost *owner =
        wxWinUITopLevelHost::FindSlotOwner(implicitHost);
    REQUIRE(owner != nullptr);
    wxWinUISlot *slot = owner->FindSlot(implicitHost);
    REQUIRE(slot != nullptr);
    const auto mergedDictionaries =
        slot->GetContainer().Resources().MergedDictionaries();
    mergedDictionaries.Append(testResources);
    wxScopeGuard restoreResource = wxMakeGuard(
        [mergedDictionaries, testResources]()
        {
            uint32_t index = 0;
            if ( mergedDictionaries.IndexOf(testResources, index) )
                mergedDictionaries.RemoveAt(index);
        });
    wxUnusedVar(restoreResource);

    implicitHost->SetLabel("must-not-mask-implicit-style");
    REQUIRE(implicitHost->SetContentFromXaml(
        "<Viewbox xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    DrainToQuiescence();

    owner = wxWinUITopLevelHost::FindSlotOwner(implicitHost);
    REQUIRE(owner != nullptr);
    slot = owner->FindSlot(implicitHost);
    REQUIRE(slot != nullptr);
    const auto content = slot->GetContent();
    REQUIRE(content != nullptr);
    const auto viewbox = content.as<MUXC::Viewbox>();

    const winrt::Windows::UI::Xaml::Interop::TypeName dynamicViewboxType
    {
        winrt::get_class_name(content),
        winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata
    };
    CHECK(dynamicViewboxType.Name ==
          winrt::xaml_typename<MUXC::Viewbox>().Name);
    CHECK(dynamicViewboxType.Kind ==
          winrt::xaml_typename<MUXC::Viewbox>().Kind);
    CHECK(testResources.HasKey(winrt::box_value(dynamicViewboxType)));

    // FrameworkElement::Style intentionally stays null for an implicit Style.
    // The witness setter proves that WinUI resolved this exact resource, while
    // the empty Name remains indistinguishable from the DP default.
    CHECK(viewbox.Style() == nullptr);
    CHECK(viewbox.MinWidth() == 321.0);
    CHECK(content.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(content).empty());
    implicitHost->SetLabel("must-not-reclaim-implicit-style");
    owner->FlushSync();
    CHECK(MUXA::AutomationProperties::GetName(content).empty());

    // A local x:Null has differed across WinUI realization paths. Use a fresh
    // control type and a witness setter, then require wx ownership to match
    // the Style that WinUI actually applied instead of assuming either
    // interpretation.
    implicitHost->SetLabel("x-null-existing-resource-wx-name");
    REQUIRE(implicitHost->SetContentFromXaml(
        "<ItemsWrapGrid "
        "xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" "
        "xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\" "
        "Style=\"{x:Null}\"/>"));
    owner = wxWinUITopLevelHost::FindSlotOwner(implicitHost);
    REQUIRE(owner != nullptr);
    slot = owner->FindSlot(implicitHost);
    REQUIRE(slot != nullptr);
    owner->FlushSync();
    const auto nullWithResourceContent = slot->GetContent();
    REQUIRE(nullWithResourceContent != nullptr);
    const auto nullWithResourceFramework =
        nullWithResourceContent.as<MUX::FrameworkElement>();
    REQUIRE(nullWithResourceFramework.Style() == nullptr);
    REQUIRE(nullWithResourceContent.ReadLocalValue(
                MUX::FrameworkElement::StyleProperty()) !=
            MUX::DependencyProperty::UnsetValue());
    REQUIRE((nullWithResourceFramework.MinWidth() == 0.0 ||
             nullWithResourceFramework.MinWidth() == 323.0));
    if ( nullWithResourceFramework.MinWidth() == 323.0 )
    {
        CHECK(nullWithResourceContent.ReadLocalValue(nameProperty) ==
              MUX::DependencyProperty::UnsetValue());
        CHECK(MUXA::AutomationProperties::GetName(
                  nullWithResourceContent).empty());
    }
    else
    {
        CHECK(nullWithResourceContent.ReadLocalValue(nameProperty) !=
              MUX::DependencyProperty::UnsetValue());
        CHECK(MUXA::AutomationProperties::GetName(
                  nullWithResourceContent) ==
              L"x-null-existing-resource-wx-name");
    }

    // Create the x:Null content before the dictionary exists in its resource
    // chain. Adding the dictionary later is deliberately not a style
    // resolution boundary for an already-realized element.
    uint32_t resourceIndex = 0;
    REQUIRE(mergedDictionaries.IndexOf(testResources, resourceIndex));
    mergedDictionaries.RemoveAt(resourceIndex);
    implicitHost->SetLabel("x-null-wx-name");
    REQUIRE(implicitHost->SetContentFromXaml(
        "<VariableSizedWrapGrid "
        "xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" "
        "xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\" "
        "Style=\"{x:Null}\"/>"));
    owner = wxWinUITopLevelHost::FindSlotOwner(implicitHost);
    REQUIRE(owner != nullptr);
    slot = owner->FindSlot(implicitHost);
    REQUIRE(slot != nullptr);
    owner->FlushSync();
    const auto nullStyleContent = slot->GetContent();
    REQUIRE(nullStyleContent != nullptr);
    const auto nullStyleFramework =
        nullStyleContent.as<MUX::FrameworkElement>();
    CHECK(nullStyleFramework.Style() == nullptr);
    CHECK(nullStyleContent.ReadLocalValue(
              MUX::FrameworkElement::StyleProperty()) !=
          MUX::DependencyProperty::UnsetValue());
    CHECK(nullStyleFramework.MinWidth() == 0.0);
    CHECK(MUXA::AutomationProperties::GetName(nullStyleContent) ==
          L"x-null-wx-name");

    mergedDictionaries.Append(lateResources);
    wxScopeGuard restoreLateResource = wxMakeGuard(
        [mergedDictionaries, lateResources]()
        {
            uint32_t index = 0;
            if ( mergedDictionaries.IndexOf(lateResources, index) )
                mergedDictionaries.RemoveAt(index);
        });
    wxUnusedVar(restoreLateResource);
    DrainToQuiescence();
    REQUIRE(nullStyleFramework.MinWidth() == 0.0);
    REQUIRE(MUXA::AutomationProperties::GetName(nullStyleContent) ==
            L"x-null-wx-name");

    // Clearing x:Null is a real style-resolution boundary on this fresh
    // control type. The StyleProperty observer must inspect the now-applied
    // implicit witness and relinquish wx's accessible name.
    nullStyleContent.ClearValue(MUX::FrameworkElement::StyleProperty());
    REQUIRE(DrainUntil(
        [&]()
        {
            return nullStyleFramework.Style() == nullptr &&
                   nullStyleFramework.MinWidth() == 322.0 &&
                   nullStyleContent.ReadLocalValue(nameProperty) ==
                       MUX::DependencyProperty::UnsetValue() &&
                   MUXA::AutomationProperties::GetName(
                       nullStyleContent).empty();
        }));

    // Re-applying the same application theme is not another style-resolution
    // boundary and must not disturb the relinquished application state.
    wxWinUISetAppTheme(wxWinUIGetAppTheme());
    DrainToQuiescence();
    CHECK(nullStyleFramework.MinWidth() == 322.0);
    CHECK(nullStyleContent.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(nullStyleContent).empty());

    // A real detach/restore is a style-resolution boundary. The failed
    // replacement must observe the implicit witness on reattach, relinquish
    // wx's Name, and preserve that application decision transactionally.
    const MUXC::Button failedReplacement;
    wxWinUITopLevelHost::TestFailContentSwap(
        wxWinUITopLevelHost::TestContentFault_Install);
    CHECK_FALSE(slot->SetContent(failedReplacement));
    CHECK(slot->GetContent() == nullStyleContent);
    REQUIRE(DrainUntil(
        [&]()
        {
            return nullStyleFramework.MinWidth() == 322.0 &&
                   nullStyleContent.ReadLocalValue(nameProperty) ==
                       MUX::DependencyProperty::UnsetValue() &&
                   MUXA::AutomationProperties::GetName(
                       nullStyleContent).empty();
        }));
    implicitHost->SetLabel("must-not-reclaim-after-reattach");
    owner->FlushSync();
    CHECK(MUXA::AutomationProperties::GetName(nullStyleContent).empty());

    // Cross-TLW migration creates a new slot/container and must carry the
    // permanent ownership decision even though the scoped Style stays behind.
    REQUIRE(implicitHost->Reparent(frameB));
    owner = wxWinUITopLevelHost::FindSlotOwner(implicitHost);
    REQUIRE(owner != nullptr);
    slot = owner->FindSlot(implicitHost);
    REQUIRE(slot != nullptr);
    owner->FlushSync();
    CHECK(nullStyleContent.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(nullStyleContent).empty());

    // The local Style source can also become x:Null after the observer was
    // installed. WinUI may keep the effective Style null for both that write
    // and its later ClearValue(), so the ordinary property callback is not a
    // sufficient signal. Tag is deliberately a non-layout witness: WinUI has
    // no source-change callback when x:Null and UnsetValue both resolve to a
    // null effective Style, and keeping CompositionTarget.Rendering subscribed
    // would force the UI thread to tick every frame. The next ordinary wx host
    // transaction must nevertheless detect the new implicit Style exactly.
    const MUX::ResourceDictionary dynamicResources =
        MUXM::XamlReader::Load(
            L"<ResourceDictionary "
            L"xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
            L"presentation\" "
            L"xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\">"
            L"<Style TargetType=\"Canvas\">"
            L"<Setter Property=\"AutomationProperties.Name\" Value=\"\"/>"
            L"<Setter Property=\"Tag\" Value=\"dynamic-style-applied\"/>"
            L"</Style>"
            L"</ResourceDictionary>")
            .as<MUX::ResourceDictionary>();
    wxWinUIXamlHost * const dynamicHost =
        new wxWinUIXamlHost(frameA);
    dynamicHost->SetLabel("dynamic-x-null-wx-name");
    REQUIRE(dynamicHost->SetContentFromXaml(
        "<Canvas xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    DrainToQuiescence();

    wxWinUITopLevelHost * const dynamicOwner =
        wxWinUITopLevelHost::FindSlotOwner(dynamicHost);
    REQUIRE(dynamicOwner != nullptr);
    wxWinUISlot * const dynamicSlot =
        dynamicOwner->FindSlot(dynamicHost);
    REQUIRE(dynamicSlot != nullptr);
    const auto dynamicContent = dynamicSlot->GetContent();
    REQUIRE(dynamicContent != nullptr);
    const auto dynamicCanvas = dynamicContent.as<MUXC::Canvas>();
    REQUIRE(MUXA::AutomationProperties::GetName(dynamicContent) ==
            L"dynamic-x-null-wx-name");

    dynamicCanvas.Style(nullptr);
    REQUIRE(dynamicContent.ReadLocalValue(
                MUX::FrameworkElement::StyleProperty()) !=
            MUX::DependencyProperty::UnsetValue());
    const auto dynamicMerged =
        dynamicSlot->GetContainer().Resources().MergedDictionaries();
    dynamicMerged.Append(dynamicResources);
    wxScopeGuard restoreDynamicResource = wxMakeGuard(
        [dynamicMerged, dynamicResources]()
        {
            uint32_t index = 0;
            if ( dynamicMerged.IndexOf(dynamicResources, index) )
                dynamicMerged.RemoveAt(index);
        });
    wxUnusedVar(restoreDynamicResource);
    DrainToQuiescence();
    REQUIRE(winrt::unbox_value_or<winrt::hstring>(
                dynamicCanvas.Tag(), L"").empty());
    REQUIRE(MUXA::AutomationProperties::GetName(dynamicContent) ==
            L"dynamic-x-null-wx-name");

    dynamicContent.ClearValue(MUX::FrameworkElement::StyleProperty());
    DrainToQuiescence();
    CHECK(winrt::unbox_value_or<winrt::hstring>(
              dynamicCanvas.Tag(), L"") ==
          L"dynamic-style-applied");
    CHECK(MUXA::AutomationProperties::GetName(dynamicContent) ==
          L"dynamic-x-null-wx-name");

    dynamicOwner->MarkDirty(dynamicHost);
    dynamicOwner->FlushSync();
    REQUIRE(DrainUntil(
        [&]()
        {
            return winrt::unbox_value_or<winrt::hstring>(
                       dynamicCanvas.Tag(), L"") ==
                       L"dynamic-style-applied" &&
                   dynamicContent.ReadLocalValue(nameProperty) ==
                       MUX::DependencyProperty::UnsetValue() &&
                   MUXA::AutomationProperties::GetName(
                       dynamicContent).empty();
        }));
    dynamicHost->Destroy();
    DrainDispatch();
    CHECK(dynamicContent.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());

    // Symmetric claim path: a host which started with no wx label owns no
    // local Name yet. If application XAML silently acquires an empty implicit
    // Name before the first non-empty SetLabel(), that label transaction must
    // inspect ownership before publishing, not mask the authored empty value.
    const MUX::ResourceDictionary emptyLabelResources =
        MUXM::XamlReader::Load(
            L"<ResourceDictionary "
            L"xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
            L"presentation\" "
            L"xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\">"
            L"<Style TargetType=\"Canvas\">"
            L"<Setter Property=\"AutomationProperties.Name\" Value=\"\"/>"
            L"<Setter Property=\"Tag\" Value=\"empty-label-style-applied\"/>"
            L"</Style>"
            L"</ResourceDictionary>")
            .as<MUX::ResourceDictionary>();
    wxWinUIXamlHost * const emptyLabelHost =
        new wxWinUIXamlHost(frameA);
    REQUIRE(emptyLabelHost->SetContentFromXaml(
        "<Canvas xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    DrainToQuiescence();

    wxWinUITopLevelHost * const emptyLabelOwner =
        wxWinUITopLevelHost::FindSlotOwner(emptyLabelHost);
    REQUIRE(emptyLabelOwner != nullptr);
    wxWinUISlot * const emptyLabelSlot =
        emptyLabelOwner->FindSlot(emptyLabelHost);
    REQUIRE(emptyLabelSlot != nullptr);
    const auto emptyLabelContent = emptyLabelSlot->GetContent();
    REQUIRE(emptyLabelContent != nullptr);
    const auto emptyLabelCanvas = emptyLabelContent.as<MUXC::Canvas>();
    CHECK(emptyLabelContent.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(emptyLabelContent).empty());

    emptyLabelCanvas.Style(nullptr);
    const auto emptyLabelMerged =
        emptyLabelSlot->GetContainer().Resources().MergedDictionaries();
    emptyLabelMerged.Append(emptyLabelResources);
    wxScopeGuard restoreEmptyLabelResource = wxMakeGuard(
        [emptyLabelMerged, emptyLabelResources]()
        {
            uint32_t index = 0;
            if ( emptyLabelMerged.IndexOf(emptyLabelResources, index) )
                emptyLabelMerged.RemoveAt(index);
        });
    wxUnusedVar(restoreEmptyLabelResource);
    emptyLabelContent.ClearValue(MUX::FrameworkElement::StyleProperty());
    DrainToQuiescence();
    REQUIRE(winrt::unbox_value_or<winrt::hstring>(
                emptyLabelCanvas.Tag(), L"") ==
            L"empty-label-style-applied");

    const unsigned flushRunsBeforeLabel =
        emptyLabelOwner->GetOwnFlushRunCountForTest();
    emptyLabelHost->SetLabel("must-not-mask-empty-implicit-name");
    REQUIRE(DrainUntil(
        [&]()
        {
            return emptyLabelOwner->GetOwnFlushRunCountForTest() >
                       flushRunsBeforeLabel &&
                   emptyLabelContent.ReadLocalValue(nameProperty) ==
                       MUX::DependencyProperty::UnsetValue() &&
                   MUXA::AutomationProperties::GetName(
                       emptyLabelContent).empty();
        }));
    emptyLabelHost->Destroy();
    DrainDispatch();
    CHECK(emptyLabelContent.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());

    implicitHost->Destroy();
    DrainDispatch();
    CHECK(nullStyleContent.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());

    frameA->Destroy();
    frameB->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::AutomationNameBindingPreserved",
          "[HostState][HostLifecycle][accessibility]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
    namespace MUXD = winrt::Microsoft::UI::Xaml::Data;

    const auto nameProperty = MUXA::AutomationProperties::NameProperty();
    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "uia-binding-A");
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "uia-binding-B");
    wxWinUIXamlHost * const xamlHost =
        new wxWinUIXamlHost(frameA);
    xamlHost->SetLabel("wx-name-before-binding");
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"binding\"/>"));
    ShowOffscreenWithoutActivating(frameA);
    ShowOffscreenWithoutActivating(frameB, 1);
    DrainToQuiescence();

    wxWinUITopLevelHost *owner =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(owner != nullptr);
    wxWinUISlot *slot = owner->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto content = slot->GetContent();
    REQUIRE(content != nullptr);
    const auto framework = content.as<MUX::FrameworkElement>();
    REQUIRE(MUXA::AutomationProperties::GetName(content) ==
            L"wx-name-before-binding");

    const MUXC::TextBlock bindingSource;
    bindingSource.Text(L"application-bound-name");
    MUXD::Binding binding;
    binding.Source(bindingSource);
    binding.Path(MUX::PropertyPath(L"Text"));
    binding.Mode(MUXD::BindingMode::OneWay);
    framework.SetBinding(nameProperty, binding);
    REQUIRE(framework.GetBindingExpression(nameProperty) != nullptr);
    REQUIRE(DrainUntil(
        [&]()
        {
            return MUXA::AutomationProperties::GetName(content) ==
                   L"application-bound-name";
        }));

    // ReadLocalValue for a binding is not guaranteed to project as a string.
    // The binding expression is the authoritative application-ownership
    // signal and must survive rollback, migration and detach without SetValue.
    const MUXC::Button failedReplacement;
    wxWinUITopLevelHost::TestFailContentSwap(
        wxWinUITopLevelHost::TestContentFault_Install);
    CHECK_FALSE(slot->SetContent(failedReplacement));
    CHECK(slot->GetContent() == content);
    CHECK(framework.GetBindingExpression(nameProperty) != nullptr);
    CHECK(MUXA::AutomationProperties::GetName(content) ==
          L"application-bound-name");

    REQUIRE(xamlHost->Reparent(frameB));
    owner = wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(owner != nullptr);
    slot = owner->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    owner->FlushSync();
    CHECK(framework.GetBindingExpression(nameProperty) != nullptr);
    CHECK(MUXA::AutomationProperties::GetName(content) ==
          L"application-bound-name");
    xamlHost->SetLabel("must-not-reclaim-over-binding");
    owner->FlushSync();
    CHECK(framework.GetBindingExpression(nameProperty) != nullptr);
    CHECK(MUXA::AutomationProperties::GetName(content) ==
          L"application-bound-name");

    xamlHost->Destroy();
    DrainDispatch();
    CHECK(framework.GetBindingExpression(nameProperty) != nullptr);
    CHECK(MUXA::AutomationProperties::GetName(content) ==
          L"application-bound-name");

    frameA->Destroy();
    frameB->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::ThemeImplicitStyleRelinquishesWxName",
          "[HostState][HostLifecycle][accessibility]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
    namespace MUXM = winrt::Microsoft::UI::Xaml::Markup;

    const wxWinUIAppTheme originalTheme = wxWinUIGetAppTheme();
    wxWinUISetAppTheme(wxWinUIAppTheme::Light);
    wxScopeGuard restoreTheme = wxMakeGuard(
        [originalTheme]()
        {
            wxWinUISetAppTheme(originalTheme);
        });
    wxUnusedVar(restoreTheme);

    const MUX::ResourceDictionary themeResources =
        MUXM::XamlReader::Load(
            L"<ResourceDictionary "
            L"xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
            L"presentation\" "
            L"xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\">"
            L"<ResourceDictionary.ThemeDictionaries>"
            L"<ResourceDictionary x:Key=\"Default\">"
            L"<Style TargetType=\"RelativePanel\">"
            L"<Setter Property=\"MinWidth\" Value=\"333\"/>"
            L"</Style>"
            L"</ResourceDictionary>"
            L"<ResourceDictionary x:Key=\"Light\">"
            L"<Style TargetType=\"RelativePanel\">"
            L"<Setter Property=\"MinWidth\" Value=\"111\"/>"
            L"</Style>"
            L"</ResourceDictionary>"
            L"<ResourceDictionary x:Key=\"Dark\">"
            L"<Style TargetType=\"RelativePanel\">"
            L"<Setter Property=\"AutomationProperties.Name\" Value=\"\"/>"
            L"<Setter Property=\"MinWidth\" Value=\"222\"/>"
            L"</Style>"
            L"</ResourceDictionary>"
            L"<ResourceDictionary x:Key=\"HighContrast\">"
            L"<Style TargetType=\"RelativePanel\">"
            L"<Setter Property=\"AutomationProperties.Name\" Value=\"\"/>"
            L"<Setter Property=\"MinWidth\" Value=\"444\"/>"
            L"</Style>"
            L"</ResourceDictionary>"
            L"</ResourceDictionary.ThemeDictionaries>"
            L"</ResourceDictionary>")
            .as<MUX::ResourceDictionary>();

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "uia-theme-style");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Grid xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();

    wxWinUITopLevelHost * const owner =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(owner != nullptr);
    wxWinUISlot * const slot = owner->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto mergedDictionaries =
        slot->GetContainer().Resources().MergedDictionaries();
    mergedDictionaries.Append(themeResources);
    wxScopeGuard restoreResources = wxMakeGuard(
        [mergedDictionaries, themeResources]()
        {
            uint32_t index = 0;
            if ( mergedDictionaries.IndexOf(themeResources, index) )
                mergedDictionaries.RemoveAt(index);
        });
    wxUnusedVar(restoreResources);

    xamlHost->SetLabel("light-wx-name");
    REQUIRE(xamlHost->SetContentFromXaml(
        "<RelativePanel xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    DrainToQuiescence();

    wxWinUISlot * const contentSlot = owner->FindSlot(xamlHost);
    REQUIRE(contentSlot != nullptr);
    const auto content = contentSlot->GetContent();
    REQUIRE(content != nullptr);
    const auto panel = content.as<MUXC::RelativePanel>();
    const auto nameProperty = MUXA::AutomationProperties::NameProperty();
    const auto isKnownWitness = [](double width)
    {
        return width == 111.0 || width == 222.0 ||
               width == 333.0 || width == 444.0;
    };
    const auto branchAuthorsName = [](double width)
    {
        return width == 222.0 || width == 444.0;
    };
    const auto checkNameState =
        [&](bool nameWasRelinquished, const wchar_t *wxName)
        {
            if ( nameWasRelinquished )
            {
                CHECK(content.ReadLocalValue(nameProperty) ==
                      MUX::DependencyProperty::UnsetValue());
                CHECK(MUXA::AutomationProperties::GetName(content).empty());
            }
            else
            {
                CHECK(content.ReadLocalValue(nameProperty) !=
                      MUX::DependencyProperty::UnsetValue());
                CHECK(MUXA::AutomationProperties::GetName(content) ==
                      wxName);
            }
        };

    REQUIRE(panel.ActualTheme() == MUX::ElementTheme::Light);
    REQUIRE(panel.Style() == nullptr);
    REQUIRE(isKnownWitness(panel.MinWidth()));
    bool nameWasRelinquished = branchAuthorsName(panel.MinWidth());
    checkNameState(nameWasRelinquished, L"light-wx-name");

    wxWinUISetAppTheme(wxWinUIAppTheme::Dark);
    REQUIRE(DrainUntil(
        [&]()
        {
            return panel.ActualTheme() == MUX::ElementTheme::Dark &&
                   isKnownWitness(panel.MinWidth());
        }));
    nameWasRelinquished =
        nameWasRelinquished || branchAuthorsName(panel.MinWidth());
    checkNameState(nameWasRelinquished, L"light-wx-name");

    // Returning to a neutral branch doesn't authorize wx to reclaim a
    // property once an authored branch owned it in this content generation.
    wxWinUISetAppTheme(wxWinUIAppTheme::Light);
    REQUIRE(DrainUntil(
        [&]()
        {
            return panel.ActualTheme() == MUX::ElementTheme::Light &&
                   isKnownWitness(panel.MinWidth());
        }));
    nameWasRelinquished =
        nameWasRelinquished || branchAuthorsName(panel.MinWidth());
    xamlHost->SetLabel("must-not-reclaim-after-theme-roundtrip");
    owner->FlushSync();
    checkNameState(
        nameWasRelinquished, L"must-not-reclaim-after-theme-roundtrip");

    // Qualify the final Application.Resources fallback separately. Direct
    // ResourceDictionary lookup can select a different active theme branch
    // from the element's ActualTheme, so the MinWidth witness tells us which
    // Style WinUI really applied and the Name ownership must match that exact
    // branch.
    const MUX::ResourceDictionary applicationThemeResources =
        MUXM::XamlReader::Load(
            L"<ResourceDictionary "
            L"xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
            L"presentation\" "
            L"xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\">"
            L"<ResourceDictionary.ThemeDictionaries>"
            L"<ResourceDictionary x:Key=\"Default\">"
            L"<Style TargetType=\"RatingControl\">"
            L"<Setter Property=\"AutomationProperties.Name\" Value=\"\"/>"
            L"<Setter Property=\"MinWidth\" Value=\"533\"/>"
            L"</Style></ResourceDictionary>"
            L"<ResourceDictionary x:Key=\"Light\">"
            L"<Style TargetType=\"RatingControl\">"
            L"<Setter Property=\"AutomationProperties.Name\" Value=\"\"/>"
            L"<Setter Property=\"MinWidth\" Value=\"511\"/>"
            L"</Style></ResourceDictionary>"
            L"<ResourceDictionary x:Key=\"Dark\">"
            L"<Style TargetType=\"RatingControl\">"
            L"<Setter Property=\"AutomationProperties.Name\" Value=\"\"/>"
            L"<Setter Property=\"MinWidth\" Value=\"522\"/>"
            L"</Style></ResourceDictionary>"
            L"<ResourceDictionary x:Key=\"HighContrast\">"
            L"<Style TargetType=\"RatingControl\">"
            L"<Setter Property=\"AutomationProperties.Name\" Value=\"\"/>"
            L"<Setter Property=\"MinWidth\" Value=\"544\"/>"
            L"</Style></ResourceDictionary>"
            L"</ResourceDictionary.ThemeDictionaries>"
            L"</ResourceDictionary>")
            .as<MUX::ResourceDictionary>();
    const MUX::Application application = MUX::Application::Current();
    REQUIRE(application != nullptr);
    const auto applicationMergedDictionaries =
        application.Resources().MergedDictionaries();
    applicationMergedDictionaries.Append(applicationThemeResources);
    wxScopeGuard restoreApplicationResources = wxMakeGuard(
        [applicationMergedDictionaries, applicationThemeResources]()
        {
            uint32_t index = 0;
            if ( applicationMergedDictionaries.IndexOf(
                    applicationThemeResources, index) )
            {
                applicationMergedDictionaries.RemoveAt(index);
            }
        });
    wxUnusedVar(restoreApplicationResources);

    wxWinUIXamlHost * const applicationHost =
        new wxWinUIXamlHost(frame);
    applicationHost->SetLabel("application-resource-wx-name");
    REQUIRE(applicationHost->SetContentFromXaml(
        "<RatingControl "
        "xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    DrainToQuiescence();
    wxWinUITopLevelHost * const applicationOwner =
        wxWinUITopLevelHost::FindSlotOwner(applicationHost);
    REQUIRE(applicationOwner != nullptr);
    wxWinUISlot * const applicationSlot =
        applicationOwner->FindSlot(applicationHost);
    REQUIRE(applicationSlot != nullptr);
    const auto applicationContent = applicationSlot->GetContent();
    REQUIRE(applicationContent != nullptr);
    const auto rating = applicationContent.as<MUXC::RatingControl>();
    const double applicationWitness = rating.MinWidth();
    REQUIRE((applicationWitness == 511.0 ||
             applicationWitness == 522.0 ||
             applicationWitness == 533.0 ||
             applicationWitness == 544.0));
    CHECK(applicationContent.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(applicationContent).empty());

    applicationHost->Destroy();
    xamlHost->Destroy();
    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::AutomationNameRelinquishmentSurvivesTransactions",
          "[HostState][HostLifecycle][accessibility]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
    namespace MUXMA = winrt::Microsoft::UI::Xaml::Media::Animation;

    const auto nameProperty = MUXA::AutomationProperties::NameProperty();
    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "uia-relinquishment-A");
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "uia-relinquishment-B");
    wxWinUIXamlHost * const xamlHost =
        new wxWinUIXamlHost(frameA);
    xamlHost->SetLabel("wx-name");
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"owned\"/>"));
    ShowOffscreenWithoutActivating(frameA);
    ShowOffscreenWithoutActivating(frameB, 1);
    DrainToQuiescence();

    wxWinUITopLevelHost *host =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    wxWinUISlot *slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    const auto content = slot->GetContent();
    REQUIRE(content != nullptr);
    REQUIRE(MUXA::AutomationProperties::GetName(content) ==
            L"wx-name");

    // ClearValue is an application decision to suppress narration. Unlike a
    // local empty string, it leaves no dependency-property evidence once the
    // slot's internal ownership record is discarded.
    content.ClearValue(nameProperty);
    host->FlushSync();
    REQUIRE(content.ReadLocalValue(nameProperty) ==
            MUX::DependencyProperty::UnsetValue());
    REQUIRE(MUXA::AutomationProperties::GetName(content).empty());

    // A failed replacement restores the same content generation. The
    // rollback must restore the otherwise-unobservable relinquishment before
    // its catch-up SyncSlot can consider republishing the wx label.
    const MUXC::Button replacement;
    wxWinUITopLevelHost::TestFailContentSwap(
        wxWinUITopLevelHost::TestContentFault_Install);
    CHECK_FALSE(slot->SetContent(replacement));
    CHECK(slot->GetContent() == content);
    CHECK(content.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(content).empty());

    // Cross-TLW migration builds a fresh slot. Seed the decision before its
    // container is attached, because Loaded callbacks may flush immediately.
    REQUIRE(xamlHost->Reparent(frameB));
    host = wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(host != nullptr);
    REQUIRE(host->FindSlot(xamlHost) != nullptr);
    host->FlushSync();
    CHECK(content.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(content).empty());
    xamlHost->SetLabel("must-not-reclaim-after-migration");
    host->FlushSync();
    CHECK(MUXA::AutomationProperties::GetName(content).empty());

    // The transfer snapshot also detects a ClearValue which has not yet had
    // a coalesced old-host SyncSlot pass.
    wxWinUIXamlHost * const immediateHost =
        new wxWinUIXamlHost(frameA);
    immediateHost->SetLabel("immediate-wx-name");
    REQUIRE(immediateHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"immediate\"/>"));
    wxWinUITopLevelHost *immediateOwner =
        wxWinUITopLevelHost::FindSlotOwner(immediateHost);
    REQUIRE(immediateOwner != nullptr);
    immediateOwner->FlushSync();
    wxWinUISlot * const immediateSlot =
        immediateOwner->FindSlot(immediateHost);
    REQUIRE(immediateSlot != nullptr);
    const auto immediateContent = immediateSlot->GetContent();
    REQUIRE(immediateContent != nullptr);
    REQUIRE(MUXA::AutomationProperties::GetName(immediateContent) ==
            L"immediate-wx-name");
    immediateContent.ClearValue(nameProperty);
    REQUIRE(immediateHost->Reparent(frameB));
    immediateOwner =
        wxWinUITopLevelHost::FindSlotOwner(immediateHost);
    REQUIRE(immediateOwner != nullptr);
    immediateOwner->FlushSync();
    CHECK(immediateContent.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(immediateContent).empty());

    // A failed ownership read followed by a failed restoration must keep the
    // existing wx ownership intact. Seeding fail-closed transfer provenance
    // here would strand the still-local wx scalar as application-owned.
    wxWinUIXamlHost * const restoreFaultHost =
        new wxWinUIXamlHost(frameA);
    restoreFaultHost->SetLabel("restore-fault-wx-name");
    REQUIRE(restoreFaultHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"restore-fault\"/>"));
    wxWinUITopLevelHost *restoreFaultOwner =
        wxWinUITopLevelHost::FindSlotOwner(restoreFaultHost);
    REQUIRE(restoreFaultOwner != nullptr);
    restoreFaultOwner->FlushSync();
    wxWinUISlot * const restoreFaultSlot =
        restoreFaultOwner->FindSlot(restoreFaultHost);
    REQUIRE(restoreFaultSlot != nullptr);
    const auto restoreFaultContent = restoreFaultSlot->GetContent();
    REQUIRE(restoreFaultContent != nullptr);
    REQUIRE(MUXA::AutomationProperties::GetName(restoreFaultContent) ==
            L"restore-fault-wx-name");

    const MUXC::Button restoreFaultReplacement;
    wxWinUITopLevelHost::TestFailContentSwap(
        wxWinUITopLevelHost::
            TestContentFault_AutomationNameTransferRead |
        wxWinUITopLevelHost::
            TestContentFault_AutomationNameRestoreRead);
    CHECK_FALSE(restoreFaultSlot->SetContent(restoreFaultReplacement));
    CHECK(restoreFaultSlot->GetContent() == restoreFaultContent);
    CHECK(MUXA::AutomationProperties::GetName(restoreFaultContent) ==
          L"restore-fault-wx-name");
    restoreFaultHost->SetLabel("restore-fault-updated");
    restoreFaultOwner->FlushSync();
    CHECK(MUXA::AutomationProperties::GetName(restoreFaultContent) ==
          L"restore-fault-updated");
    restoreFaultHost->Destroy();
    DrainDispatch();
    CHECK(restoreFaultContent.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());

    // A higher-precedence animation is application authorship even while the
    // exact wx local scalar remains present underneath it. The transfer
    // snapshot must mirror SyncSlot's effective-value rule so a failed swap
    // cannot make the wx name reappear when the animation ends.
    wxWinUIXamlHost * const animatedHost =
        new wxWinUIXamlHost(frameA);
    animatedHost->SetLabel("animated-wx-name");
    REQUIRE(animatedHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"animated\"/>"));
    wxWinUITopLevelHost *animatedOwner =
        wxWinUITopLevelHost::FindSlotOwner(animatedHost);
    REQUIRE(animatedOwner != nullptr);
    animatedOwner->FlushSync();
    wxWinUISlot * const animatedSlot =
        animatedOwner->FindSlot(animatedHost);
    REQUIRE(animatedSlot != nullptr);
    const auto animatedContent = animatedSlot->GetContent();
    REQUIRE(animatedContent != nullptr);
    REQUIRE(MUXA::AutomationProperties::GetName(animatedContent) ==
            L"animated-wx-name");

    MUXMA::ObjectAnimationUsingKeyFrames nameAnimation;
    nameAnimation.EnableDependentAnimation(true);
    MUXMA::DiscreteObjectKeyFrame emptyNameFrame;
    emptyNameFrame.KeyTime(MUXMA::KeyTime{});
    emptyNameFrame.Value(winrt::box_value(winrt::hstring()));
    nameAnimation.KeyFrames().Append(emptyNameFrame);
    MUXMA::Storyboard::SetTarget(nameAnimation, animatedContent);
    MUXMA::Storyboard::SetTargetProperty(
        nameAnimation, L"(AutomationProperties.Name)");
    MUXMA::Storyboard nameStoryboard;
    nameStoryboard.Children().Append(nameAnimation);
    nameStoryboard.Begin();
    nameStoryboard.SeekAlignedToLastTick(
        winrt::Windows::Foundation::TimeSpan{ 0 });
    REQUIRE(animatedContent.ReadLocalValue(nameProperty) !=
            MUX::DependencyProperty::UnsetValue());
    REQUIRE(MUXA::AutomationProperties::GetName(animatedContent).empty());
    REQUIRE(DrainUntil(
        [&]()
        {
            // The Name observer must autonomously remove the wx base value
            // while the animation is active. No label, host flush, content
            // transaction or layout nudge is allowed to supply the signal.
            return animatedContent.ReadLocalValue(nameProperty) ==
                       MUX::DependencyProperty::UnsetValue() &&
                   MUXA::AutomationProperties::GetName(
                       animatedContent).empty();
        }));

    const MUXC::Button animatedReplacement;
    wxWinUITopLevelHost::TestFailContentSwap(
        wxWinUITopLevelHost::TestContentFault_Install);
    CHECK_FALSE(animatedSlot->SetContent(animatedReplacement));
    CHECK(animatedSlot->GetContent() == animatedContent);
    nameStoryboard.Stop();
    animatedHost->SetLabel("must-not-reclaim-after-animation");
    animatedOwner->FlushSync();
    CHECK(animatedContent.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(animatedContent).empty());

#if wxUSE_ACCESSIBILITY
    // CreateAccessible() is arbitrary application code. During A->B it
    // reparents again to C. The factory must run while A still exposes the
    // original content/provenance, so the nested migration can transfer both.
    wxFrame * const frameC =
        new wxFrame(nullptr, wxID_ANY, "uia-relinquishment-C");
    ReparentingAccessibleXamlHost * const reentrantHost =
        new ReparentingAccessibleXamlHost;
    REQUIRE(reentrantHost->Create(frameA));
    reentrantHost->SetLabel("reentrant-wx-name");
    REQUIRE(reentrantHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"reentrant\"/>"));
    wxWinUITopLevelHost * const reentrantOwnerA =
        wxWinUITopLevelHost::FindSlotOwner(reentrantHost);
    REQUIRE(reentrantOwnerA != nullptr);
    reentrantOwnerA->FlushSync();
    wxWinUISlot * const reentrantSlotA =
        reentrantOwnerA->FindSlot(reentrantHost);
    REQUIRE(reentrantSlotA != nullptr);
    const auto reentrantContent = reentrantSlotA->GetContent();
    REQUIRE(reentrantContent != nullptr);
    reentrantContent.ClearValue(nameProperty);
    reentrantOwnerA->FlushSync();
    REQUIRE(MUXA::AutomationProperties::GetName(reentrantContent).empty());

    reentrantHost->ArmReparentFromFactory(frameC);
    REQUIRE(reentrantHost->Reparent(frameB));
    REQUIRE(reentrantHost->WasReparentAttempted());
    REQUIRE(reentrantHost->DidReparentSucceed());
    CHECK(reentrantHost->GetParent() == frameC);
    wxWinUITopLevelHost * const reentrantOwnerC =
        wxWinUITopLevelHost::FindSlotOwner(reentrantHost);
    REQUIRE(reentrantOwnerC != nullptr);
    REQUIRE(reentrantOwnerC ==
            wxWinUITopLevelHost::FindForTLW(frameC));
    CHECK(reentrantOwnerA->FindSlot(reentrantHost) == nullptr);
    wxWinUITopLevelHost * const frameBHost =
        wxWinUITopLevelHost::FindForTLW(frameB);
    CHECK((!frameBHost ||
           frameBHost->FindSlot(reentrantHost) == nullptr));
    wxWinUISlot * const reentrantSlotC =
        reentrantOwnerC->FindSlot(reentrantHost);
    REQUIRE(reentrantSlotC != nullptr);
    CHECK(reentrantSlotC->GetContent() == reentrantContent);
    CHECK(reentrantContent.ReadLocalValue(nameProperty) ==
          MUX::DependencyProperty::UnsetValue());
    CHECK(MUXA::AutomationProperties::GetName(reentrantContent).empty());
    reentrantHost->SetLabel("must-not-reclaim-after-reentrant-migration");
    reentrantOwnerC->FlushSync();
    CHECK(MUXA::AutomationProperties::GetName(reentrantContent).empty());
    frameC->Destroy();
#endif // wxUSE_ACCESSIBILITY

    frameA->Destroy();
    frameB->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::ControlHostContentTransaction", "[HostState]")
{
    // The same transaction, exercised through wxWinUIControlHost -- the
    // layer every ordinary component goes through.  wxRadioBox::SetString()
    // rebuilds its XAML root and swaps it via the control host.
    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "state-ctrlhost");
    const wxString choices[] = { "alpha", "beta", "gamma" };
    wxRadioBox * const radio =
        new wxRadioBox(frame, wxID_ANY, "group", wxDefaultPosition,
                       wxDefaultSize, WXSIZEOF(choices), choices);
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(radio);
    REQUIRE(slot != nullptr);
    const auto oldRoot = slot->GetContent();
    REQUIRE(oldRoot != nullptr);
    const unsigned hooks0 = wxWinUITopLevelHost::GetLiveLoadedHookCount();

    // Failed swap through the control host: the slot, the carrier AND the
    // control host's model must all keep the OLD root, and the old Loaded
    // hook must survive (only the new one is dropped: net count unchanged).
    wxWinUITopLevelHost::TestFailContentSwap(
        wxWinUITopLevelHost::TestContentFault_Install);
    radio->SetString(0, "renamed");
    DrainDispatch(3);

    CHECK(host->FindSlot(radio) == slot);
    CHECK(slot->GetContent() == oldRoot);
    {
        winrt::Microsoft::UI::Xaml::UIElement actual{ nullptr };
        REQUIRE(slot->TryGetCarrierContent(actual));
        CHECK(actual == oldRoot);
    }
    CHECK(wxWinUITopLevelHost::GetLiveLoadedHookCount() == hooks0);

    // ... and the control recovers: a clean rebuild really swaps, with the
    // hook balance still closed (old revoked + new added).
    radio->SetString(0, "again");
    DrainDispatch(3);
    CHECK(slot->GetContent() != oldRoot);
    CHECK(slot->GetContent() != nullptr);
    CHECK(wxWinUITopLevelHost::GetLiveLoadedHookCount() == hooks0);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::ControlHostNestedFreshContentWins",
          "[HostState][HostLifecycle]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "state-ctrlhost-nested");
    wxWindow * const probeWin = new wxWindow(frame, wxID_ANY);
    const unsigned hooks0 = wxWinUITopLevelHost::GetLiveLoadedHookCount();

    wxWinUIControlHostProbe * const probe =
        wxWinUIControlHostProbe::CreateEmpty(probeWin);
    REQUIRE(probe != nullptr);
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(probeWin, false);
    REQUIRE(host != nullptr);
    REQUIRE(host->FindSlot(probeWin) == nullptr);

    bool seamRan = false;
    bool nestedSucceeded = false;
    wxWinUITopLevelHost::TestOnNextSlotAttached(
        [&](wxWindow *attached)
        {
            if ( attached != probeWin )
                return;

            seamRan = true;
            nestedSucceeded = probe->SetContent("inner");
        });

    // B is attached by the outer registration, then C replaces it while the
    // fresh slot is already discoverable. C is the authoritative generation:
    // the outer operation must report loss and must neither unregister C nor
    // overwrite the control proxy's C model/Loaded hook with stale B state.
    const bool outerSucceeded = probe->SetContent("outer");
    wxWinUITopLevelHost::TestOnNextSlotAttached({});

    CHECK(seamRan);
    CHECK(nestedSucceeded);
    CHECK_FALSE(outerSucceeded);
    wxWinUISlot * const slot = host->FindSlot(probeWin);
    REQUIRE(slot != nullptr);
    REQUIRE(slot->GetContent() != nullptr);
    CHECK(probe->GetContentForTesting() == slot->GetContent());
    winrt::Microsoft::UI::Xaml::UIElement carrier{ nullptr };
    REQUIRE(slot->TryGetCarrierContent(carrier));
    CHECK(carrier == slot->GetContent());
    const auto innerButton = slot->GetContent().try_as<MUXC::Button>();
    REQUIRE(innerButton != nullptr);
    CHECK(winrt::unbox_value<winrt::hstring>(innerButton.Content()) ==
          L"inner");

    DrainDispatch(3);
    CHECK(wxWinUITopLevelHost::GetLiveLoadedHookCount() == hooks0 + 1);

    // The preserved slot/proxy remains a normal reusable transaction.
    REQUIRE(probe->SetContent("after"));
    wxWinUISlot * const afterSlot = host->FindSlot(probeWin);
    REQUIRE(afterSlot != nullptr);
    CHECK(probe->GetContentForTesting() == afterSlot->GetContent());
    const auto afterButton = afterSlot->GetContent().try_as<MUXC::Button>();
    REQUIRE(afterButton != nullptr);
    CHECK(winrt::unbox_value<winrt::hstring>(afterButton.Content()) ==
          L"after");
    DrainDispatch(3);
    CHECK(wxWinUITopLevelHost::GetLiveLoadedHookCount() == hooks0 + 1);

    delete probe;
    DrainDispatch(3);
    CHECK(host->FindSlot(probeWin) == nullptr);
    CHECK(wxWinUITopLevelHost::GetLiveLoadedHookCount() == hooks0);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::ControlHostDestroyedDuringFreshContent",
          "[HostLifecycle][HostState]")
{
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "state-ctrlhost-destroy");
    wxWindow * const probeWin = new wxWindow(frame, wxID_ANY);
    const unsigned hooks0 = wxWinUITopLevelHost::GetLiveLoadedHookCount();
    const unsigned states0 =
        wxWinUITopLevelHost::GetLiveSlotLifetimeCount();

    wxWinUIControlHostProbe * const probe =
        wxWinUIControlHostProbe::CreateEmpty(probeWin);
    REQUIRE(probe != nullptr);
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(probeWin, false);
    REQUIRE(host != nullptr);

    bool seamRan = false;
    wxWinUITopLevelHost::TestOnNextSlotAttached(
        [&](wxWindow *attached)
        {
            if ( attached != probeWin )
                return;

            seamRan = true;
            probe->DestroyControlHostForTesting();
        });

    // SetContent() must finish through locals only after the callback destroys
    // the actual wxWinUIControlHost object underneath the still-live probe.
    const bool outerSucceeded = probe->SetContent("doomed");
    wxWinUITopLevelHost::TestOnNextSlotAttached({});

    CHECK(seamRan);
    CHECK_FALSE(outerSucceeded);
    CHECK(probe->GetContentForTesting() == nullptr);
    CHECK(host->FindSlot(probeWin) == nullptr);
    DrainDispatch(3);
    CHECK(wxWinUITopLevelHost::GetLiveLoadedHookCount() == hooks0);
    CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() == states0);

    delete probe;
    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::ControlHostClearContent", "[HostState]")
{
    // wxWinUIControlHost::ClearContent() has no production caller, so drive
    // it directly through the probe.  The point: a successful clear must
    // revoke the OLD content's Loaded hook (its lambda captures the host),
    // and a failed detach must change nothing.
    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "state-chclear");
    wxWindow * const probeWin = new wxWindow(frame, wxID_ANY);
    frame->Show();
    DrainToQuiescence();

    // The bare window is no winui control, so the frame has no island yet;
    // the probe's control host creates it (and sets an initial content).
    const unsigned hooks0 = wxWinUITopLevelHost::GetLiveLoadedHookCount();

    wxWinUIControlHostProbe * const probe =
        wxWinUIControlHostProbe::Create(probeWin);
    REQUIRE(probe != nullptr);
    DrainDispatch(3);

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);

    // content set: exactly one new live Loaded hook, a slot with content
    CHECK(wxWinUITopLevelHost::GetLiveLoadedHookCount() == hooks0 + 1);
    REQUIRE(host->FindSlot(probeWin) != nullptr);
    CHECK(host->FindSlot(probeWin)->GetContent() != nullptr);

    // ClearContent: the hook is revoked (count back to baseline), and the
    // slot and carrier are truly empty
    probe->ClearContent();
    DrainDispatch(3);
    CHECK(wxWinUITopLevelHost::GetLiveLoadedHookCount() == hooks0);
    wxWinUISlot * const slot = host->FindSlot(probeWin);
    REQUIRE(slot != nullptr);
    CHECK(slot->GetContent() == nullptr);
    {
        winrt::Microsoft::UI::Xaml::UIElement actual{ nullptr };
        REQUIRE(slot->TryGetCarrierContent(actual));
        CHECK(actual == nullptr);
    }

    // a FAILED detach must change nothing: content and hook both survive
    probe->SetContent();
    DrainDispatch(3);
    REQUIRE(wxWinUITopLevelHost::GetLiveLoadedHookCount() == hooks0 + 1);
    wxWinUITopLevelHost::TestFailContentSwap(
        wxWinUITopLevelHost::TestContentFault_Detach);
    probe->ClearContent();
    DrainDispatch(3);
    CHECK(wxWinUITopLevelHost::GetLiveLoadedHookCount() == hooks0 + 1);
    CHECK(host->FindSlot(probeWin)->GetContent() != nullptr);

    // recovers: a real clear drops the hook, a real set adds one again
    probe->ClearContent();
    DrainDispatch(3);
    CHECK(wxWinUITopLevelHost::GetLiveLoadedHookCount() == hooks0);
    probe->SetContent();
    DrainDispatch(3);
    CHECK(wxWinUITopLevelHost::GetLiveLoadedHookCount() == hooks0 + 1);
    CHECK(host->FindSlot(probeWin)->GetContent() != nullptr);

    // destroying the probe closes the host: the last hook goes too
    delete probe;
    DrainDispatch(3);
    CHECK(wxWinUITopLevelHost::GetLiveLoadedHookCount() == hooks0);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::PoisonedSlotRebuilt", "[HostState]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    // When even reading the carrier fails, the slot is poisoned: the next
    // registration must DISPOSE of it and build a fresh, fully working slot,
    // not resuscitate a corpse whose callbacks are all dead.
    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "state-poison");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    wxWinUISlot * const original = host->FindSlot(xamlHost);
    REQUIRE(original != nullptr);
    const auto originalContainer = original->GetContainer();
    REQUIRE(originalContainer != nullptr);

    const unsigned states0 = wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
    const unsigned adds0 = wxWinUITopLevelHost::GetSlotHandlerAddCount();
    const unsigned revokes0 = wxWinUITopLevelHost::GetSlotHandlerRevokeCount();

    // Install fails and the immediate carrier read fails: the old slot is
    // poisoned before the restore phase is reachable. RegisterSlotInternal
    // disposes it and completes this same registration with a fresh slot, so
    // the public operation succeeds. Do not arm Restore here: that phase
    // cannot run after an unreadable carrier and its unused global bit would
    // contaminate the next test.
    wxWinUITopLevelHost::TestFailContentSwap(
        wxWinUITopLevelHost::TestContentFault_Install |
        wxWinUITopLevelHost::TestContentFault_Read);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"x\"/>"));
    DrainDispatch(3);

    wxWinUISlot * const rebuilt = host->FindSlot(xamlHost);
    REQUIRE(rebuilt != nullptr);
    CHECK(rebuilt->GetContainer() != originalContainer);
    const auto firstButton =
        rebuilt->GetContent().try_as<MUXC::Button>();
    REQUIRE(firstButton != nullptr);
    CHECK(winrt::unbox_value<winrt::hstring>(firstButton.Content()) ==
          L"x");
    const auto rebuiltContainer = rebuilt->GetContainer();

    // A subsequent registration proves that the rebuilt slot is reusable.
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"ok\"/>"));
    DrainDispatch(3);

    wxWinUISlot * const slot = host->FindSlot(xamlHost);
    REQUIRE(slot != nullptr);
    CHECK(slot == rebuilt);
    CHECK(slot->GetContainer() == rebuiltContainer);
    const auto secondButton =
        slot->GetContent().try_as<MUXC::Button>();
    REQUIRE(secondButton != nullptr);
    CHECK(winrt::unbox_value<winrt::hstring>(secondButton.Content()) ==
          L"ok");

    // the rebuilt slot is fully functional: focus and the state adapter work
    frame->Raise();
    host->FocusSlot(xamlHost);
    DrainDispatch(3);
    CHECK(host->GetFocusOwner() == xamlHost);
#if wxUSE_TOOLTIPS
    xamlHost->SetToolTip("rebuilt");
    DrainDispatch(3);
    const auto tip = MUXC::ToolTipService::GetToolTip(slot->GetContent());
    REQUIRE(tip != nullptr);
    CHECK(GetToolTipInspectableText(tip) == "rebuilt");
#endif

    // no zombie: exactly one live lifetime for this window (old released,
    // new created), and every handler of the disposed slot was revoked.
    CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() == states0);
    CHECK(wxWinUITopLevelHost::GetSlotHandlerAddCount() - adds0 ==
          wxWinUITopLevelHost::GetSlotHandlerRevokeCount() - revokes0);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostState::FocusOwnerOnlyOnRealFocus", "[HostState]")
{
    // The ownership must never be published for a slot that did not
    // actually receive the focus: FindFocus() and the Tab navigation
    // would start from a lie.
    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "state-ownreal");
    wxButton * const b1 = new wxButton(frame, wxID_ANY, "real",
                                       wxPoint(10, 10));
    // A slot whose content is NOT focusable: a bare Border.
    wxWinUIXamlHost * const emptyHost = new wxWinUIXamlHost(frame);
    REQUIRE(emptyHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    // And a slot that is effectively disabled.
    wxButton * const b2 = new wxButton(frame, wxID_ANY, "off",
                                       wxPoint(10, 90));
    b2->Disable();
    frame->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);

    host->FocusSlot(b1);
    DrainDispatch(3);
    REQUIRE(host->GetFocusOwner() == b1);

    // Focusing an unfocusable slot must change nothing.
    host->FocusSlot(emptyHost);
    DrainDispatch(3);
    CHECK(host->GetFocusOwner() == b1);
    CHECK(wxWinUITopLevelHost::ResolveFocusHwnd((WXHWND)::GetFocus()) == b1);

    // Focusing a disabled slot must change nothing either.
    host->FocusSlot(b2);
    DrainDispatch(3);
    CHECK(host->GetFocusOwner() == b1);
    CHECK(wxWinUITopLevelHost::ResolveFocusHwnd((WXHWND)::GetFocus()) == b1);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::XamlHostReconcilesFailedMigration",
          "[HostLifecycle]")
{
    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "xaml-owner-A");
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "xaml-owner-B");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frameA);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    frameA->Show();
    frameB->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const hostA =
        wxWinUITopLevelHost::FindForTLW(frameA);
    REQUIRE(hostA != nullptr);
    REQUIRE(hostA->FindSlot(xamlHost) != nullptr);

    // The wx parent changes, but the injected detach failure deliberately
    // leaves the only truthful slot in A. Every public mutation must find
    // that actual owner rather than inferring B and creating a duplicate.
    wxWinUITopLevelHost::TestFailContentSwap(
        wxWinUITopLevelHost::TestContentFault_Detach);
    REQUIRE(xamlHost->Reparent(frameB));
    CHECK(wxWinUITopLevelHost::FindSlotOwner(xamlHost) == hostA);
    REQUIRE(hostA->FindSlot(xamlHost) != nullptr);

    xamlHost->ClearContent();
    REQUIRE(hostA->FindSlot(xamlHost) != nullptr);
    CHECK(hostA->FindSlot(xamlHost)->GetContent() == nullptr);

    // The next install first reconciles the empty slot into B, then swaps
    // the new content. At no point may two hosts own the same wx window.
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"survivor\"/>"));
    wxWinUITopLevelHost * const hostB =
        wxWinUITopLevelHost::FindForTLW(frameB);
    REQUIRE(hostB != nullptr);
    CHECK(hostA->FindSlot(xamlHost) == nullptr);
    REQUIRE(hostB->FindSlot(xamlHost) != nullptr);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(xamlHost) == hostB);

    frameA->Destroy();
    DrainDispatch(3);
    REQUIRE(hostB->FindSlot(xamlHost) != nullptr);
    CHECK(hostB->FindSlot(xamlHost)->GetContent() != nullptr);

    frameB->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::FailedMigrationThenImmediateReparent",
          "[HostLifecycle]")
{
    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "xaml-reparent-A");
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "xaml-reparent-B");
    wxFrame * const frameC =
        new wxFrame(nullptr, wxID_ANY, "xaml-reparent-C");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frameA);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"survivor\"/>"));
    frameA->Show();
    frameB->Show();
    frameC->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const hostA =
        wxWinUITopLevelHost::FindForTLW(frameA);
    REQUIRE(hostA != nullptr);

    // A->B fails and truthfully leaves the slot in A. The next B->C
    // reparent happens without a yield: its notification must consult actual
    // slot owners, not just B, and synchronously migrate A's survivor to C.
    wxWinUITopLevelHost::TestFailContentSwap(
        wxWinUITopLevelHost::TestContentFault_Detach);
    REQUIRE(xamlHost->Reparent(frameB));
    REQUIRE(wxWinUITopLevelHost::FindSlotOwner(xamlHost) == hostA);
    REQUIRE(xamlHost->Reparent(frameC));

    wxWinUITopLevelHost * const hostB =
        wxWinUITopLevelHost::FindForTLW(frameB);
    wxWinUITopLevelHost * const hostC =
        wxWinUITopLevelHost::FindForTLW(frameC);
    REQUIRE(hostC != nullptr);
    CHECK(hostA->FindSlot(xamlHost) == nullptr);
    CHECK((!hostB || hostB->FindSlot(xamlHost) == nullptr));
    REQUIRE(hostC->FindSlot(xamlHost) != nullptr);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(xamlHost) == hostC);

    // The stale source TLW can die immediately without taking the only slot.
    frameA->Destroy();
    DrainDispatch(3);
    REQUIRE(hostC->FindSlot(xamlHost) != nullptr);
    CHECK(hostC->FindSlot(xamlHost)->GetContent() != nullptr);

    frameB->Destroy();
    frameC->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::ReentrantDoubleMigration", "[HostLifecycle]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "xaml-double-A");
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "xaml-double-B");
    wxFrame * const frameC =
        new wxFrame(nullptr, wxID_ANY, "xaml-double-C");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frameA);
    xamlHost->SetLabel("migrated-name");
#if wxUSE_TOOLTIPS
    xamlHost->SetToolTip("migrated-tip");
#endif
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"payload\"/>"));
    frameA->Show();
    frameB->Show();
    frameC->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const hostA =
        wxWinUITopLevelHost::FindForTLW(frameA);
    REQUIRE(hostA != nullptr);
    wxWinUISlot * const slotA = hostA->FindSlot(xamlHost);
    REQUIRE(slotA != nullptr);
    const auto content = slotA->GetContent();

    bool reentered = false;
    bool nestedReparentSucceeded = false;
    wxWinUITopLevelHost::TestOnNextSlotAttached(
        [&](wxWindow *attached)
        {
            if ( attached != xamlHost )
                return;

            reentered = true;
            nestedReparentSucceeded = xamlHost->Reparent(frameC);
        });

    // Re-enter at the synchronous slot-publication boundary: the nested
    // migration reaches C before the outer A->B operation returns.
    const bool outerReparentSucceeded = xamlHost->Reparent(frameB);
    wxWinUITopLevelHost::TestOnNextSlotAttached({});
    REQUIRE(outerReparentSucceeded);
    CHECK(reentered);
    CHECK(nestedReparentSucceeded);

    wxWinUITopLevelHost * const hostB =
        wxWinUITopLevelHost::FindForTLW(frameB);
    wxWinUITopLevelHost * const hostC =
        wxWinUITopLevelHost::FindForTLW(frameC);
    REQUIRE(hostC != nullptr);
    CHECK(hostA->FindSlot(xamlHost) == nullptr);
    CHECK((!hostB || hostB->FindSlot(xamlHost) == nullptr));
    wxWinUISlot * const slotC = hostC->FindSlot(xamlHost);
    REQUIRE(slotC != nullptr);
    CHECK(slotC->GetContent() == content);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(xamlHost) == hostC);

    DrainToQuiescence();
#if wxUSE_TOOLTIPS
    const auto tip = MUXC::ToolTipService::GetToolTip(content);
    REQUIRE(tip != nullptr);
    CHECK(GetToolTipInspectableText(tip) == "migrated-tip");
#endif
    CHECK(MUX::Automation::AutomationProperties::GetName(content) ==
          L"migrated-name");

    frameA->Destroy();
    frameB->Destroy();
    frameC->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::ReentrantMigrationAndContentReplacement",
          "[HostLifecycle]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "xaml-replace-A");
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "xaml-replace-B");
    wxFrame * const frameC =
        new wxFrame(nullptr, wxID_ANY, "xaml-replace-C");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frameA);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    frameA->Show();
    frameB->Show();
    frameC->Show();
    DrainToQuiescence();

    wxWinUITopLevelHost * const hostA =
        wxWinUITopLevelHost::FindForTLW(frameA);
    REQUIRE(hostA != nullptr);
    wxWinUISlot * const oldSlot = hostA->FindSlot(xamlHost);
    REQUIRE(oldSlot != nullptr);
    const auto oldContent = oldSlot->GetContent();

    bool reentered = false;
    bool nestedReparentSucceeded = false;
    bool replacementSucceeded = false;
    wxWinUITopLevelHost::TestOnNextSlotAttached(
        [&](wxWindow *attached)
        {
            if ( attached != xamlHost )
                return;

            reentered = true;
            nestedReparentSucceeded = xamlHost->Reparent(frameC);
            if ( nestedReparentSucceeded )
            {
                replacementSucceeded = xamlHost->SetContentFromXaml(
                    "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/"
                    "xaml/presentation\" Content=\"replacement\"/>");
            }
        });

    const bool outerReparentSucceeded = xamlHost->Reparent(frameB);
    wxWinUITopLevelHost::TestOnNextSlotAttached({});
    REQUIRE(outerReparentSucceeded);
    CHECK(reentered);
    CHECK(nestedReparentSucceeded);
    CHECK(replacementSucceeded);

    wxWinUITopLevelHost * const hostB =
        wxWinUITopLevelHost::FindForTLW(frameB);
    wxWinUITopLevelHost * const hostC =
        wxWinUITopLevelHost::FindForTLW(frameC);
    REQUIRE(hostC != nullptr);
    CHECK(hostA->FindSlot(xamlHost) == nullptr);
    CHECK((!hostB || hostB->FindSlot(xamlHost) == nullptr));
    wxWinUISlot * const replacementSlot = hostC->FindSlot(xamlHost);
    REQUIRE(replacementSlot != nullptr);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(xamlHost) == hostC);
    CHECK(replacementSlot->GetContent() != oldContent);
    CHECK(replacementSlot->GetContent().try_as<MUXC::Button>() != nullptr);

    frameA->Destroy();
    frameB->Destroy();
    frameC->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::ReparentCallbackDestroysSubtree",
          "[HostLifecycle]")
{
    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "xaml-destroy-A");
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "xaml-destroy-B");
    wxFrame * const frameC =
        new wxFrame(nullptr, wxID_ANY, "xaml-destroy-C");
    // Ensure all three hosts are already in the notification snapshot.
    new wxButton(frameB, wxID_ANY, "prime-B");
    new wxButton(frameC, wxID_ANY, "prime-C");

    wxPanel * const subtree = new wxPanel(frameA);
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(subtree);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\" Content=\"destroy\"/>"));
    frameA->Show();
    frameB->Show();
    frameC->Show();
    DrainToQuiescence();
    REQUIRE(wxWinUITopLevelHost::FindForTLW(frameA) != nullptr);
    REQUIRE(wxWinUITopLevelHost::FindForTLW(frameB) != nullptr);
    REQUIRE(wxWinUITopLevelHost::FindForTLW(frameC) != nullptr);

    wxWeakRef<wxWindow> subtreeWeak(subtree);
    wxWeakRef<wxWindow> hostedWeak(xamlHost);
    wxWinUITopLevelHost * const contentOwner =
        wxWinUITopLevelHost::FindSlotOwner(xamlHost);
    REQUIRE(contentOwner != nullptr);
    wxWinUISlot * const contentSlot = contentOwner->FindSlot(xamlHost);
    REQUIRE(contentSlot != nullptr);

    bool destroyRequested = false;
    wxWinUITopLevelHost::TestOnNextSlotAttached(
        [&](wxWindow *attached)
        {
            if ( attached != xamlHost )
                return;

            destroyRequested = subtree->Destroy();
        });

    // The first host callback destroys the root while the all-host snapshot
    // still contains B and C. The dispatcher must stop through its weak root,
    // never dereference the freed subtree for the remaining hosts.
    const bool reparentSucceeded = subtree->Reparent(frameB);
    wxWinUITopLevelHost::TestOnNextSlotAttached({});
    REQUIRE(reparentSucceeded);
    CHECK(destroyRequested);
    DrainDispatch(5);
    CHECK(!subtreeWeak);
    CHECK(!hostedWeak);

    frameA->Destroy();
    frameB->Destroy();
    frameC->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::XamlHostSharesTheIsland", "[HostLifecycle]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    const unsigned hosts0 = wxWinUITopLevelHost::GetLiveHostCount();
    const unsigned states0 = wxWinUITopLevelHost::GetLiveSlotLifetimeCount();

    wxFrame * const frame = new wxFrame(nullptr, wxID_ANY, "lifecycle-xaml");
    new wxButton(frame, wxID_ANY, "native");
    wxWinUIXamlHost * const xamlHost = new wxWinUIXamlHost(frame);
    REQUIRE(xamlHost->SetContentFromXaml(
        "<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        "presentation\"/>"));
    frame->Show();
    DrainToQuiescence();

    // ONE island per TLW: the XAML host's content is a slot of the shared
    // host...
    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hosts0 + 1);
    wxWinUITopLevelHost * const host = wxWinUITopLevelHost::FindForTLW(frame);
    REQUIRE(host != nullptr);
    REQUIRE(host->FindSlot(xamlHost) != nullptr);
    // ... proven DIRECTLY: exactly one DesktopChildSiteBridge window exists
    // under the TLW, whatever any bookkeeping says
    CHECK(CountSiteBridges(static_cast<HWND>(frame->GetHWND())) == 1);

#if wxUSE_TOOLTIPS
    // a tooltip that must survive every content swap below: each new
    // content has to receive it again (the per-content caches must reset)
    xamlHost->SetToolTip("xh-tip");
    DrainDispatch(3);
#endif

    for ( int k = 0; k < 3; ++k )
    {
        INFO("content swap #" << k);

        // ClearContent() really empties the slot, keeping it usable
        xamlHost->ClearContent();
        wxWinUISlot * const slot = host->FindSlot(xamlHost);
        REQUIRE(slot != nullptr);
        CHECK(slot->GetContent() == nullptr);

        // the next SetContent installs the NEW element type for real
        REQUIRE(xamlHost->SetContentFromXaml(
            "<Button xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
            "presentation\" Content=\"again\"/>"));
        DrainDispatch(3);

        const auto content = host->FindSlot(xamlHost)->GetContent();
        REQUIRE(content != nullptr);
        CHECK(content.try_as<MUXC::Button>() != nullptr);

#if wxUSE_TOOLTIPS
        // the fresh content received the state again: stale caches would
        // skip it
        const auto tip = MUXC::ToolTipService::GetToolTip(content);
        REQUIRE(tip != nullptr);
        CHECK(GetToolTipInspectableText(tip) == "xh-tip");
#endif
    }

    CHECK(CountSiteBridges(static_cast<HWND>(frame->GetHWND())) == 1);

    frame->Destroy();
    DrainDispatch();

    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hosts0);
    CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() == states0);
}

TEST_CASE("HostLifecycle::RegisterRollbackOnFault", "[HostLifecycle]")
{
    // A bare wxWindow has no XAML peer of its own: the slot registered (and
    // sabotaged) below is fully under the test's control.
    wxWindow * const probe =
        new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY);
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(probe, true);
    REQUIRE(host != nullptr);
    REQUIRE(host->FindSlot(probe) == nullptr);

    const winrt::Microsoft::UI::Xaml::UIElement element =
        winrt::Microsoft::UI::Xaml::Controls::Border();

    // BindSlotEvents() performs 11 subscriptions (8 pointer handlers,
    // GettingFocus, GotFocus, LostFocus): fail each one in turn and check
    // that the transaction leaves strictly nothing behind.
    for ( unsigned nth = 1; nth <= 11; ++nth )
    {
        const unsigned slots0 = wxWinUITopLevelHost::GetLiveSlotCount();
        const unsigned states0 =
            wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
        const unsigned adds0 = wxWinUITopLevelHost::GetSlotHandlerAddCount();
        const unsigned revokes0 =
            wxWinUITopLevelHost::GetSlotHandlerRevokeCount();

        wxWinUITopLevelHost::TestFailHandlerAdd(nth);
        wxWinUISlot * const slot = host->RegisterSlot(probe, element);
        DrainDispatch(2);

        INFO("failing subscription #" << nth);
        CHECK(slot == nullptr);
        CHECK(host->FindSlot(probe) == nullptr);
        CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == slots0);
        CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() == states0);
        // whatever was subscribed before the fault was revoked again
        CHECK(wxWinUITopLevelHost::GetSlotHandlerAddCount() - adds0 ==
              wxWinUITopLevelHost::GetSlotHandlerRevokeCount() - revokes0);
    }

    // the injection disarmed itself and the rollbacks left the element
    // parentless: a normal registration must now succeed
    wxWinUISlot * const slot = host->RegisterSlot(probe, element);
    REQUIRE(slot != nullptr);
    CHECK(host->FindSlot(probe) == slot);
    host->UnregisterSlot(probe);
    CHECK(host->FindSlot(probe) == nullptr);

    probe->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::SlotPointerOwnerBalancesAcrossSurfaces",
          "[HostLifecycle][winui-slot-input]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxWindow * const slotA =
        new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY);
    wxWindow * const slotB =
        new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY);
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(slotA, true);
    REQUIRE(host != nullptr);
    REQUIRE(wxWinUITopLevelHost::ForWindow(slotB, true) == host);
    REQUIRE(host->RegisterSlot(slotA, MUXC::Border()) != nullptr);
    REQUIRE(host->RegisterSlot(slotB, MUXC::Border()) != nullptr);

    unsigned downsA = 0;
    unsigned doubleClicksA = 0;
    unsigned upsA = 0;
    unsigned upsB = 0;
    unsigned leavesA = 0;
    bool reenterOnUp = false;
    bool nestedPressSucceeded = false;
    wxWinUIPointerSample nestedPress;
    slotA->Bind(wxEVT_LEFT_DOWN,
        [&downsA](wxMouseEvent&) { ++downsA; });
    slotA->Bind(wxEVT_LEFT_DCLICK,
        [&doubleClicksA](wxMouseEvent&) { ++doubleClicksA; });
    slotA->Bind(wxEVT_LEFT_UP,
        [&](wxMouseEvent&)
        {
            ++upsA;
            if ( reenterOnUp )
            {
                reenterOnUp = false;
                nestedPressSucceeded =
                    host->TestRouteSlotPointer(slotA, nestedPress);
            }
        });
    slotA->Bind(wxEVT_LEAVE_WINDOW,
        [&leavesA](wxMouseEvent&) { ++leavesA; });
    slotB->Bind(wxEVT_LEFT_UP,
        [&upsB](wxMouseEvent&) { ++upsB; });

    wxWinUIPointerSample press;
    press.kind = wxWinUIInputKind::Press;
    press.device = wxWinUIInputDevice::Mouse;
    press.pointerId = 73;
    press.isPrimary = true;
    press.button = wxWinUIInputButton::Left;
    press.buttonMask = MK_LBUTTON;
    press.screenX = 40;
    press.screenY = 50;
    press.timestamp = 1000;

    REQUIRE(host->TestRouteSlotPointer(slotA, press));
    CHECK(downsA == 1);
    CHECK(host->GetSlotPointerOwnerCountForTest() == 1);

    wxWinUIPointerSample release = press;
    release.kind = wxWinUIInputKind::Release;
    release.buttonMask = 0;
    release.timestamp = 1010;

    // XAML first bubbles the release through the element currently under
    // the pointer. Its local state has no matching press and must not invent
    // an UP; the host broker then balances the original slot A gesture.
    REQUIRE(host->TestRouteSlotPointer(slotB, release));
    CHECK(upsB == 0);
    REQUIRE(host->TestRouteOwnedSlotRelease(release));
    CHECK(upsA == 1);
    CHECK(upsB == 0);
    CHECK(host->GetSlotPointerOwnerCountForTest() == 0);

    // A pointer-scoped cancellation retires the owner without fabricating a
    // wx UP and leaves the slot ready for an unrelated later gesture.
    press.timestamp = 1200;
    REQUIRE(host->TestRouteSlotPointer(slotA, press));
    CHECK(downsA == 2);
    CHECK(doubleClicksA == 0);
    CHECK(host->GetSlotPointerOwnerCountForTest() == 1);

    wxWinUIPointerSample cancel = press;
    cancel.kind = wxWinUIInputKind::Cancel;
    cancel.buttonMask = 0;
    cancel.timestamp = 1210;
    REQUIRE(host->TestRouteSlotPointer(slotA, cancel));
    CHECK(upsA == 1);
    CHECK(host->GetSlotPointerOwnerCountForTest() == 0);

    // The local UP handler can pump a newer DOWN before this routed release
    // bubbles to the root. The old receipt/serial must not clear the newer
    // owner or send it an orphan UP. Establish a real slot hover first: the
    // release is then classified outside, so its prepared Leave must survive
    // the re-entrant DOWN and be delivered exactly once.
    press.pointerId = 74;
    press.timestamp = 100000;
    wxWinUIPointerSample hover = press;
    hover.kind = wxWinUIInputKind::Move;
    hover.button = wxWinUIInputButton::None;
    hover.buttonMask = 0;
    hover.timestamp = press.timestamp - 10;
    REQUIRE(host->TestRouteSlotPointer(slotA, hover));
    REQUIRE(host->TestRouteSlotPointer(slotA, press));
    CHECK(downsA == 3);
    CHECK(host->GetSlotPointerOwnerCountForTest() == 1);

    nestedPress = press;
    nestedPress.timestamp = press.timestamp + 1000;
    reenterOnUp = true;
    wxWinUIPointerSample localRelease = press;
    localRelease.kind = wxWinUIInputKind::Release;
    localRelease.buttonMask = 0;
    localRelease.screenX = -10000;
    localRelease.screenY = -10000;
    localRelease.timestamp = press.timestamp + 10;
    REQUIRE(host->TestRouteSlotPointer(
        slotA, localRelease, wxWinUIInputSurface::Outside));
    REQUIRE(nestedPressSucceeded);
    CHECK(upsA == 2);
    CHECK(leavesA == 1);
    CHECK(downsA == 4);
    CHECK(host->GetSlotPointerOwnerCountForTest() == 1);
    REQUIRE(host->TestRouteOwnedSlotRelease(localRelease));
    CHECK(upsA == 2);
    CHECK(host->GetSlotPointerOwnerCountForTest() == 1);

    cancel = nestedPress;
    cancel.kind = wxWinUIInputKind::Cancel;
    cancel.buttonMask = 0;
    cancel.timestamp = nestedPress.timestamp + 10;
    REQUIRE(host->TestRouteSlotPointer(slotA, cancel));
    CHECK(host->GetSlotPointerOwnerCountForTest() == 0);

    // Destruction/unregistration is the other mandatory owner-retirement
    // path: a late physical release must have nowhere stale to dispatch.
    press.pointerId = 75;
    press.timestamp = 200000;
    REQUIRE(host->TestRouteSlotPointer(slotA, press));
    CHECK(downsA == 5);
    CHECK(host->GetSlotPointerOwnerCountForTest() == 1);
    host->UnregisterSlot(slotA);
    CHECK(host->GetSlotPointerOwnerCountForTest() == 0);

    release.pointerId = press.pointerId;
    release.timestamp = press.timestamp + 10;
    CHECK_FALSE(host->TestRouteOwnedSlotRelease(release));
    CHECK(upsA == 2);
    CHECK(upsB == 0);

    host->UnregisterSlot(slotB);
    slotA->Destroy();
    slotB->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::SlotPointerPolicy",
          "[HostLifecycle][winui-slot-input]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxWindow * const slotA =
        new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY);
    wxWindow * const slotB =
        new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY);
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(slotA, true);
    REQUIRE(host != nullptr);
    REQUIRE(host->RegisterSlot(slotA, MUXC::Border()) != nullptr);
    REQUIRE(host->RegisterSlot(slotB, MUXC::Border()) != nullptr);

    unsigned downsA = 0;
    unsigned doubleClicksA = 0;
    unsigned upsA = 0;
    unsigned downsB = 0;
    slotA->Bind(wxEVT_LEFT_DOWN,
        [&downsA](wxMouseEvent&) { ++downsA; });
    slotA->Bind(wxEVT_LEFT_DCLICK,
        [&doubleClicksA](wxMouseEvent&) { ++doubleClicksA; });
    slotA->Bind(wxEVT_LEFT_UP,
        [&upsA](wxMouseEvent&) { ++upsA; });
    slotB->Bind(wxEVT_LEFT_DOWN,
        [&downsB](wxMouseEvent&) { ++downsB; });

    wxWinUIPointerSample press;
    press.kind = wxWinUIInputKind::Press;
    press.device = wxWinUIInputDevice::Mouse;
    press.pointerId = 30;
    press.isPrimary = true;
    press.button = wxWinUIInputButton::Left;
    press.buttonMask = MK_LBUTTON;
    press.timestamp = 100;
    const std::uint32_t generationBeforePress =
        host->GetInputSiteGenerationForTest();
    REQUIRE(host->TestRouteSlotPointer(slotA, press));
    const std::uint32_t firstGestureGeneration =
        host->GetInputSiteGenerationForTest();
    CHECK(firstGestureGeneration != generationBeforePress);
    CHECK_FALSE(host->TestCanPostIslandCancelMode(
        firstGestureGeneration));
    host->TestPostIslandCancelMode(firstGestureGeneration);
    CHECK(host->HasDeferredIslandCancelForTest());
    CHECK(host->GetPostedIslandCancelCountForTest() == 0);

    wxWinUIPointerSample release = press;
    release.kind = wxWinUIInputKind::Release;
    release.buttonMask = 0;
    release.timestamp = 120;
    REQUIRE(host->TestRouteSlotPointer(
        slotA, release, wxWinUIInputSurface::Outside));
    CHECK(upsA == 1);
    CHECK(host->GetSlotPointerOwnerCountForTest() == 0);
    CHECK(host->TestCanPostIslandCancelMode(
        firstGestureGeneration));
    CHECK_FALSE(host->HasDeferredIslandCancelForTest());
    const unsigned postedResetCount =
        host->GetPostedIslandCancelCountForTest();
    CHECK(postedResetCount > 0);
    host->TestPostIslandCancelMode(firstGestureGeneration);
    CHECK(host->GetPostedIslandCancelCountForTest() ==
          postedResetCount);
    DrainDispatch(2);
    CHECK(host->GetPostedIslandCancelCountForTest() == 0);

    // A reset batch targets concrete bridge/inner HWNDs. If one of them is
    // recreated before acknowledging its message, the old count must not
    // coalesce every later reset for that input site generation.
    const std::uint32_t generationBeforeRetire =
        host->GetInputSiteGenerationForTest();
    host->TestPostIslandCancelMode(generationBeforeRetire);
    REQUIRE(host->GetPostedIslandCancelCountForTest() > 0);
    host->TestRetireIslandCancelTarget();
    const std::uint32_t generationAfterRetire =
        host->GetInputSiteGenerationForTest();
    CHECK(generationAfterRetire != generationBeforeRetire);
    CHECK_FALSE(host->HasDeferredIslandCancelForTest());
    const unsigned replacementBatch =
        host->GetPostedIslandCancelCountForTest();
    REQUIRE(replacementBatch > 0);
    host->TestPostIslandCancelMode(generationAfterRetire);
    CHECK(host->GetPostedIslandCancelCountForTest() ==
          replacementBatch);
    DrainDispatch(4);
    CHECK(host->GetPostedIslandCancelCountForTest() == 0);
    CHECK_FALSE(host->HasDeferredIslandCancelForTest());

    // A release outside the visible/ancestor-clipped slot balances the UP
    // but cannot seed a double-click.
    press.timestamp = 200;
    REQUIRE(host->TestRouteSlotPointer(slotA, press));
    CHECK(downsA == 2);
    CHECK(doubleClicksA == 0);
    CHECK_FALSE(host->TestCanPostIslandCancelMode(
        firstGestureGeneration));
    wxWinUIPointerSample cancel = press;
    cancel.kind = wxWinUIInputKind::Cancel;
    cancel.buttonMask = 0;
    cancel.timestamp = 210;
    REQUIRE(host->TestRouteSlotPointer(slotA, cancel));

    // A promoted compatibility release is not allowed to consume the owner
    // created by the real pointer stream.
    press.pointerId = 31;
    press.timestamp = 1000;
    REQUIRE(host->TestRouteSlotPointer(slotA, press));
    release = press;
    release.kind = wxWinUIInputKind::Release;
    release.buttonMask = 0;
    release.timestamp = 1010;
    release.isCompatibilityMouse = true;
    CHECK_FALSE(host->TestRouteOwnedSlotRelease(release));
    CHECK(host->GetSlotPointerOwnerCountForTest() == 1);
    release.isCompatibilityMouse = false;
    release.timestamp = 1020;
    REQUIRE(host->TestRouteOwnedSlotRelease(release));
    CHECK(upsA == 2);
    CHECK(host->GetSlotPointerOwnerCountForTest() == 0);

    // The same physical button/pointer cannot own two slots concurrently.
    press.pointerId = 32;
    press.timestamp = 2000;
    REQUIRE(host->TestRouteSlotPointer(slotA, press));
    const std::uint32_t generationWithOwner =
        host->GetInputSiteGenerationForTest();
    CHECK_FALSE(host->TestRouteSlotPointer(slotB, press));
    CHECK(host->GetInputSiteGenerationForTest() ==
          generationWithOwner);
    CHECK(downsB == 0);
    CHECK(host->GetSlotPointerOwnerCountForTest() == 1);
    release = press;
    release.kind = wxWinUIInputKind::Release;
    release.buttonMask = 0;
    release.timestamp = 2010;
    REQUIRE(host->TestRouteOwnedSlotRelease(release));
    CHECK(upsA == 3);
    CHECK(host->GetSlotPointerOwnerCountForTest() == 0);

    // Native thread-wide interruption reconstructs an admissible primary
    // identity for touch and pen owners instead of clearing only the broker
    // entry and stranding their per-slot PressState.
    for ( const wxWinUIInputDevice device :
          { wxWinUIInputDevice::Touch, wxWinUIInputDevice::Pen } )
    {
        wxWinUIPointerSample contact;
        contact.kind = wxWinUIInputKind::Press;
        contact.device = device;
        contact.pointerId =
            device == wxWinUIInputDevice::Touch ? 41 : 42;
        contact.isPrimary = true;
        contact.timestamp =
            device == wxWinUIInputDevice::Touch ? 3000 : 4000;
        REQUIRE(host->TestRouteSlotPointer(slotA, contact));
        CHECK(host->GetSlotPointerOwnerCountForTest() == 1);

        wxWinUIPointerSample interrupted;
        interrupted.kind = wxWinUIInputKind::Cancel;
        interrupted.interruptAll = true;
        host->TestCancelSlotPointerOwners(interrupted, true);
        CHECK(host->GetSlotPointerOwnerCountForTest() == 0);

        contact.timestamp += 100;
        REQUIRE(host->TestRouteSlotPointer(slotA, contact));
        CHECK(host->GetSlotPointerOwnerCountForTest() == 1);
        contact.kind = wxWinUIInputKind::Cancel;
        host->TestCancelSlotPointerOwners(contact, false);
        CHECK(host->GetSlotPointerOwnerCountForTest() == 0);
    }

    host->UnregisterSlot(slotA);
    host->UnregisterSlot(slotB);
    slotA->Destroy();
    slotB->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::HotAncestorRTLHonoursExplicitDescendantsAndReparent",
          "[HostLifecycle][winui-rtl][winui-009]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "hot-rtl-a",
                    wxPoint(100, 100), wxSize(420, 300));
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "hot-rtl-b",
                    wxPoint(550, 100), wxSize(420, 300));
    frameA->SetLayoutDirection(wxLayout_LeftToRight);
    frameB->SetLayoutDirection(wxLayout_LeftToRight);

    wxPanel * const inheritedPanel =
        new wxPanel(frameA, wxID_ANY, wxPoint(10, 10), wxSize(180, 120));
    wxButton * const inheritedButton =
        new wxButton(inheritedPanel, wxID_ANY, "inherited",
                     wxPoint(8, 8), wxSize(120, 32));

    wxPanel * const explicitPanel =
        new wxPanel(frameA, wxID_ANY, wxPoint(210, 10), wxSize(180, 120));
    explicitPanel->SetLayoutDirection(wxLayout_LeftToRight);
    wxButton * const childOfExplicitPanel =
        new wxButton(explicitPanel, wxID_ANY, "anchored subtree",
                     wxPoint(8, 8), wxSize(140, 32));

    wxButton * const explicitButton =
        new wxButton(inheritedPanel, wxID_ANY, "explicit",
                     wxPoint(8, 52), wxSize(120, 32));
    explicitButton->SetLayoutDirection(wxLayout_LeftToRight);

    wxPanel * const destinationPanel =
        new wxPanel(frameB, wxID_ANY, wxPoint(10, 10), wxSize(360, 180));

    ShowOffscreenWithoutActivating(frameA, 0);
    ShowOffscreenWithoutActivating(frameB, 1);
    DrainToQuiescence();

    wxWinUITopLevelHost * const hostA =
        wxWinUITopLevelHost::ForWindow(inheritedButton, false);
    wxWinUITopLevelHost * const hostB =
        wxWinUITopLevelHost::ForWindow(destinationPanel, true);
    REQUIRE(hostA != nullptr);
    REQUIRE(hostB != nullptr);
    hostA->FlushSync();
    hostB->FlushSync();

    const auto isNativeRTL =
        [](wxWindow *window)
        {
            return (::GetWindowLongPtr(
                        GetHwndOf(window), GWL_EXSTYLE) &
                    WS_EX_LAYOUTRTL) != 0;
        };
    const auto requireButtonFlow =
        [](wxWindow *window, MUX::FlowDirection expected)
        {
            wxWinUITopLevelHost * const host =
                wxWinUITopLevelHost::ForWindow(window, false);
            REQUIRE(host != nullptr);
            host->FlushSync();
            wxWinUISlot * const slot = host->FindSlot(window);
            REQUIRE(slot != nullptr);
            CHECK(slot->GetContainer().FlowDirection() == expected);
            const MUXC::Button peer =
                slot->GetSemanticTarget().try_as<MUXC::Button>();
            REQUIRE(peer != nullptr);
            CHECK(peer.FlowDirection() == expected);
        };

    // Existing default descendants follow a hot ancestor switch. An explicit
    // child, and the entire inherited subtree below an explicit panel, remain
    // anchored to their own effective LTR direction.
    frameA->SetLayoutDirection(wxLayout_RightToLeft);
    DrainToQuiescence();
    CHECK(isNativeRTL(inheritedPanel));
    CHECK(isNativeRTL(inheritedButton));
    CHECK_FALSE(isNativeRTL(explicitPanel));
    CHECK_FALSE(isNativeRTL(childOfExplicitPanel));
    CHECK_FALSE(isNativeRTL(explicitButton));
    requireButtonFlow(
        inheritedButton, MUX::FlowDirection::RightToLeft);
    requireButtonFlow(
        childOfExplicitPanel, MUX::FlowDirection::LeftToRight);
    requireButtonFlow(
        explicitButton, MUX::FlowDirection::LeftToRight);

    frameA->SetLayoutDirection(wxLayout_LeftToRight);
    DrainToQuiescence();
    CHECK_FALSE(isNativeRTL(inheritedPanel));
    CHECK_FALSE(isNativeRTL(inheritedButton));
    requireButtonFlow(
        inheritedButton, MUX::FlowDirection::LeftToRight);

    // Reparenting an already-created HWND must recompute inherited direction;
    // USER32 SetParent() alone leaves the old WS_EX_LAYOUTRTL bit untouched.
    frameB->SetLayoutDirection(wxLayout_RightToLeft);
    DrainToQuiescence();
    CHECK(isNativeRTL(destinationPanel));

    REQUIRE(inheritedButton->Reparent(destinationPanel));
    DrainToQuiescence();
    CHECK(isNativeRTL(inheritedButton));
    requireButtonFlow(
        inheritedButton, MUX::FlowDirection::RightToLeft);

    REQUIRE(explicitButton->Reparent(destinationPanel));
    DrainToQuiescence();
    CHECK_FALSE(isNativeRTL(explicitButton));
    requireButtonFlow(
        explicitButton, MUX::FlowDirection::LeftToRight);

    // Resetting an explicit child to Default restores provenance, not merely
    // the direction which happened to be effective at this instant.
    explicitButton->SetLayoutDirection(wxLayout_Default);
    DrainToQuiescence();
    CHECK(isNativeRTL(explicitButton));
    requireButtonFlow(
        explicitButton, MUX::FlowDirection::RightToLeft);

    frameB->SetLayoutDirection(wxLayout_LeftToRight);
    DrainToQuiescence();
    CHECK_FALSE(isNativeRTL(inheritedButton));
    CHECK_FALSE(isNativeRTL(explicitButton));
    requireButtonFlow(
        inheritedButton, MUX::FlowDirection::LeftToRight);
    requireButtonFlow(
        explicitButton, MUX::FlowDirection::LeftToRight);

    explicitButton->SetLayoutDirection(wxLayout_LeftToRight);
    frameB->SetLayoutDirection(wxLayout_RightToLeft);
    DrainToQuiescence();
    CHECK(isNativeRTL(inheritedButton));
    CHECK_FALSE(isNativeRTL(explicitButton));
    requireButtonFlow(
        inheritedButton, MUX::FlowDirection::RightToLeft);
    requireButtonFlow(
        explicitButton, MUX::FlowDirection::LeftToRight);

    frameA->Destroy();
    frameB->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::HotRTLIsLastWriterWinsAndDeletionSafe",
          "[HostLifecycle][winui-rtl][winui-009]")
{
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "hot-rtl-reentrance",
                    wxPoint(100, 100), wxSize(420, 300));
    frame->SetLayoutDirection(wxLayout_LeftToRight);

    wxScopeGuard seamGuard = wxMakeGuard([]()
    {
        wxWinUIMSWSetAfterLayoutDirectionNativeWriteForTest(nullptr);
        gs_nestedLayoutDirectionRequest = wxLayout_Default;
        gs_nestedLayoutDirectionRequestCalls = 0;
        gs_oscillatingLayoutDirectionCalls = 0;
    });
    wxUnusedVar(seamGuard);

    // A nested request at the native style boundary supersedes the outer
    // request. The revisioned transaction must converge instead of allowing
    // the outer SetWindowLongPtr result to win accidentally.
    gs_nestedLayoutDirectionRequest = wxLayout_RightToLeft;
    gs_nestedLayoutDirectionRequestCalls = 0;
    wxWinUIMSWSetAfterLayoutDirectionNativeWriteForTest(
        &RequestNestedLayoutDirection);
    frame->SetLayoutDirection(wxLayout_LeftToRight);
    CHECK(gs_nestedLayoutDirectionRequestCalls == 1);
    CHECK((::GetWindowLongPtr(GetHwndOf(frame), GWL_EXSTYLE) &
           WS_EX_LAYOUTRTL) != 0);

    gs_nestedLayoutDirectionRequest = wxLayout_LeftToRight;
    gs_nestedLayoutDirectionRequestCalls = 0;
    wxWinUIMSWSetAfterLayoutDirectionNativeWriteForTest(
        &RequestNestedLayoutDirection);
    frame->SetLayoutDirection(wxLayout_RightToLeft);
    CHECK(gs_nestedLayoutDirectionRequestCalls == 1);
    CHECK((::GetWindowLongPtr(GetHwndOf(frame), GWL_EXSTYLE) &
           WS_EX_LAYOUTRTL) == 0);

    // USER32/style handlers can retain a value different from the one just
    // requested without changing HWND identity. The transaction must read
    // the actual bit back and retry before publishing peer success.
    wxWinUIMSWSetAfterLayoutDirectionNativeWriteForTest(
        &FlipNativeLayoutDirectionStyle);
    frame->SetLayoutDirection(wxLayout_RightToLeft);
    CHECK((::GetWindowLongPtr(GetHwndOf(frame), GWL_EXSTYLE) &
           WS_EX_LAYOUTRTL) != 0);
    frame->SetLayoutDirection(wxLayout_LeftToRight);

    // A hostile handler changing direction at every native write is bounded,
    // not converted into an infinite synchronous/CallAfter loop. Clearing
    // the seam and issuing the next external request must recover.
    gs_oscillatingLayoutDirectionCalls = 0;
    wxWinUIMSWSetAfterLayoutDirectionNativeWriteForTest(
        &OscillateNestedLayoutDirection);
    frame->SetLayoutDirection(wxLayout_LeftToRight);
    CHECK(gs_oscillatingLayoutDirectionCalls == 32);
    wxWinUIMSWSetAfterLayoutDirectionNativeWriteForTest(nullptr);
    frame->SetLayoutDirection(wxLayout_LeftToRight);
    CHECK((::GetWindowLongPtr(GetHwndOf(frame), GWL_EXSTYLE) &
           WS_EX_LAYOUTRTL) == 0);

    // A local XAML projection failure must not publish the transaction as
    // complete. The same explicit request is an external retry boundary and
    // must call the peer hook again even though the native bit is already
    // correct.
    auto* const projectionProbe =
        new LayoutDirectionProjectionProbe(frame);
    wxWindow* const projectionChild =
        new wxWindow(projectionProbe, wxID_ANY,
                     wxPoint(2, 2), wxSize(40, 20));
    projectionProbe->SetLayoutDirection(wxLayout_RightToLeft);
    CHECK(projectionProbe->peerProjectionCalls == 1);
    CHECK((::GetWindowLongPtr(
               GetHwndOf(projectionProbe), GWL_EXSTYLE) &
           WS_EX_LAYOUTRTL) != 0);
    CHECK((::GetWindowLongPtr(
               GetHwndOf(projectionChild), GWL_EXSTYLE) &
           WS_EX_LAYOUTRTL) != 0);
    projectionProbe->failPeerProjection = false;
    projectionProbe->SetLayoutDirection(wxLayout_RightToLeft);
    CHECK(projectionProbe->peerProjectionCalls == 2);
    CHECK((::GetWindowLongPtr(
               GetHwndOf(projectionProbe), GWL_EXSTYLE) &
           WS_EX_LAYOUTRTL) != 0);

    // Provenance set before Create() is retained and emitted in the initial
    // HWND style, without invoking a not-yet-constructed local WinUI peer.
    wxWindow* const precreated = new wxWindow;
    precreated->SetLayoutDirection(wxLayout_RightToLeft);
    REQUIRE(precreated->Create(
        frame, wxID_ANY, wxPoint(130, 220), wxSize(100, 30)));
    CHECK((::GetWindowLongPtr(
               GetHwndOf(precreated), GWL_EXSTYLE) &
           WS_EX_LAYOUTRTL) != 0);

    // wxEVT_SIZE is dispatched before the local peer hook. Deleting the
    // control from that callback must prevent any post-event access.
    wxButton *doomedButton =
        new wxButton(frame, wxID_ANY, "delete-on-rtl",
                     wxPoint(10, 10), wxSize(140, 32));
    const wxWeakRef<wxButton> doomedButtonIdentity(doomedButton);
    doomedButton->Bind(
        wxEVT_SIZE,
        [&doomedButton](wxSizeEvent&)
        {
            wxButton * const deleting = doomedButton;
            doomedButton = nullptr;
            delete deleting;
        });
    doomedButton->SetLayoutDirection(wxLayout_RightToLeft);
    CHECK(doomedButton == nullptr);
    CHECK_FALSE(doomedButtonIdentity);

    // wxRadioButton::Reparent() has additional group work after the common
    // transaction. Exercise deletion from its terminal inherited-direction
    // size event so that work can use only weak parent identities.
    wxPanel * const rtlPanel =
        new wxPanel(frame, wxID_ANY, wxPoint(10, 60), wxSize(180, 120));
    wxPanel * const ltrPanel =
        new wxPanel(frame, wxID_ANY, wxPoint(210, 60), wxSize(180, 120));
    rtlPanel->SetLayoutDirection(wxLayout_RightToLeft);
    ltrPanel->SetLayoutDirection(wxLayout_LeftToRight);
    wxRadioButton *doomedRadio =
        new wxRadioButton(rtlPanel, wxID_ANY, "delete-on-reparent",
                          wxPoint(8, 8), wxSize(150, 32),
                          wxRB_GROUP);
    const wxWeakRef<wxRadioButton> doomedRadioIdentity(doomedRadio);
    doomedRadio->Bind(
        wxEVT_SIZE,
        [&doomedRadio](wxSizeEvent&)
        {
            wxRadioButton * const deleting = doomedRadio;
            doomedRadio = nullptr;
            delete deleting;
        });
    (void)doomedRadio->Reparent(ltrPanel);
    CHECK(doomedRadio == nullptr);
    CHECK_FALSE(doomedRadioIdentity);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::NonOwnedWindowsPreserveRTLProvenanceAcrossOwners",
          "[HostLifecycle][winui-rtl][winui-009]")
{
    wxFrame * const ownerRTL =
        new wxFrame(nullptr, wxID_ANY, "rtl-owner",
                    wxPoint(100, 100), wxSize(320, 220));
    wxFrame * const ownerLTR =
        new wxFrame(nullptr, wxID_ANY, "ltr-owner",
                    wxPoint(450, 100), wxSize(320, 220));
    ownerRTL->SetLayoutDirection(wxLayout_RightToLeft);
    ownerLTR->SetLayoutDirection(wxLayout_LeftToRight);

    const auto isNativeRTL =
        [](wxWindow *window)
        {
            return (::GetWindowLongPtr(
                        GetHwndOf(window), GWL_EXSTYLE) &
                    WS_EX_LAYOUTRTL) != 0;
        };

    wxDialog * const dialog =
        new wxDialog(ownerRTL, wxID_ANY, "owned-dialog",
                     wxDefaultPosition, wxSize(240, 140));
    dialog->SetLayoutDirection(wxLayout_Default);
    CHECK(isNativeRTL(dialog));

    REQUIRE(dialog->Reparent(ownerLTR));
    CHECK_FALSE(isNativeRTL(dialog));
    CHECK(dialog->GetParent() == ownerLTR);
    CHECK(::GetWindow(
              reinterpret_cast<HWND>(GetHwndOf(dialog)), GW_OWNER) ==
          reinterpret_cast<HWND>(GetHwndOf(ownerLTR)));

    wxWinUIMSWFailNextOwnerWriteForTest();
    CHECK_FALSE(dialog->Reparent(ownerRTL));
    CHECK(dialog->GetParent() == ownerLTR);
    CHECK(::GetWindow(
              reinterpret_cast<HWND>(GetHwndOf(dialog)), GW_OWNER) ==
          reinterpret_cast<HWND>(GetHwndOf(ownerLTR)));
    CHECK_FALSE(isNativeRTL(dialog));

    dialog->SetLayoutDirection(wxLayout_RightToLeft);
    REQUIRE(dialog->Reparent(ownerRTL));
    REQUIRE(dialog->Reparent(ownerLTR));
    CHECK(isNativeRTL(dialog));

    dialog->SetLayoutDirection(wxLayout_Default);
    CHECK_FALSE(isNativeRTL(dialog));
    ownerLTR->SetLayoutDirection(wxLayout_RightToLeft);
    CHECK(isNativeRTL(dialog));
    ownerLTR->SetLayoutDirection(wxLayout_LeftToRight);
    CHECK_FALSE(isNativeRTL(dialog));

#if wxUSE_POPUPWIN
    wxPopupWindow * const popup =
        new wxPopupWindow(ownerRTL, wxBORDER_SIMPLE);
    popup->SetLayoutDirection(wxLayout_Default);
    CHECK(isNativeRTL(popup));
    REQUIRE(popup->Reparent(ownerLTR));
    CHECK_FALSE(isNativeRTL(popup));
    popup->SetLayoutDirection(wxLayout_RightToLeft);
    REQUIRE(popup->Reparent(ownerRTL));
    REQUIRE(popup->Reparent(ownerLTR));
    CHECK(isNativeRTL(popup));
    popup->SetLayoutDirection(wxLayout_Default);
    CHECK_FALSE(isNativeRTL(popup));
    delete popup;
#endif

    dialog->Destroy();
    ownerRTL->Destroy();
    ownerLTR->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::FractionalCoordinateArithmetic",
          "[HostLifecycle][winui-coordinates][winui-009]")
{
    namespace WF = winrt::Windows::Foundation;

    const WF::Rect rootScreen{-1919.0f, -311.0f, 1500.0f, 900.0f};
    const WF::Rect anchorScreen{-1703.0f, -207.0f, 317.0f, 143.0f};
    const std::vector<double> scales{1.25, 1.5, 1.75};
    const std::vector<WF::Point> points{
        {-0.375f, -2.625f},
        {0.0f, 0.0f},
        {19.375f, 37.625f},
        {316.875f, 142.5f}
    };

    for ( const double scale : scales )
    {
        for ( const bool mirrored : {false, true} )
        {
            for ( const WF::Point& client : points )
            {
                WF::Point root{};
                WF::Point roundTrip{};
                REQUIRE(
                    wxWinUIVisualCoordinates::
                        TestRoundTripPhysicalSpaces(
                            rootScreen,
                            anchorScreen,
                            mirrored,
                            scale,
                            client,
                            &root,
                            &roundTrip));

                const double screenX =
                    mirrored
                        ? anchorScreen.X + anchorScreen.Width - client.X
                        : anchorScreen.X + client.X;
                const double screenY = anchorScreen.Y + client.Y;
                CHECK(std::abs(
                          root.X -
                          (screenX - rootScreen.X) / scale) <
                      0.0005);
                CHECK(std::abs(
                          root.Y -
                          (screenY - rootScreen.Y) / scale) <
                      0.0005);
                CHECK(std::abs(roundTrip.X - client.X) < 0.0005);
                CHECK(std::abs(roundTrip.Y - client.Y) < 0.0005);
            }
        }
    }

    WF::Point untouchedRoot{11.0f, 12.0f};
    WF::Point untouchedClient{13.0f, 14.0f};
    CHECK_FALSE(
        wxWinUIVisualCoordinates::TestRoundTripPhysicalSpaces(
            rootScreen,
            anchorScreen,
            false,
            0.0,
            WF::Point{},
            &untouchedRoot,
            &untouchedClient));
    CHECK(untouchedRoot.X == 11.0f);
    CHECK(untouchedRoot.Y == 12.0f);
    CHECK(untouchedClient.X == 13.0f);
    CHECK(untouchedClient.Y == 14.0f);
}

TEST_CASE("HostLifecycle::SlotPointerCoordinatesRTL",
          "[HostLifecycle][winui-slot-input][winui-009]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "slot-pointer-rtl",
                    wxPoint(100, 100), wxSize(360, 240));
    wxWindow * const window =
        new wxWindow(frame, wxID_ANY, wxPoint(37, 29), wxSize(121, 83));
    window->SetLayoutDirection(wxLayout_RightToLeft);

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(window, true);
    REQUIRE(host != nullptr);
    REQUIRE(host->RegisterSlot(window, MUXC::Border()) != nullptr);
    ShowOffscreenWithoutActivating(frame);
    DrainToQuiescence();
    host->FlushSync();

    wxWinUISlot * const slot = host->FindSlot(window);
    REQUIRE(slot != nullptr);
    const wxRect rect = slot->GetRectInTLW();
    REQUIRE(rect.width > 4);
    REQUIRE(rect.height > 4);
    REQUIRE((::GetWindowLongPtr(GetHwndOf(window), GWL_EXSTYLE) &
             WS_EX_LAYOUTRTL) != 0);

    const RECT rootScreen = host->GetClientScreenRect();
    const double scale = host->GetScale();
    REQUIRE(scale > 0.0);
    CHECK(host->Root().FlowDirection() ==
          winrt::Microsoft::UI::Xaml::FlowDirection::LeftToRight);
    CHECK(slot->GetContainer().FlowDirection() ==
          winrt::Microsoft::UI::Xaml::FlowDirection::RightToLeft);

    // Exercise the production visual mapper itself with fractional and
    // negative element-local coordinates. The result is expressed in the
    // mirrored HWND's client space and must round-trip without an integer
    // DIP intermediate.
    const auto content = slot->GetContent();
    const winrt::Windows::Foundation::Point elementPoint{
        -0.375f, 7.625f
    };
    winrt::Windows::Foundation::Point mappedClient{};
    REQUIRE(
        wxWinUIVisualCoordinates::ElementPointToClient(
            window, content, elementPoint, &mappedClient) ==
        wxWinUICoordinateResult::Mapped);
    winrt::Windows::Foundation::Point mappedBack{};
    REQUIRE(
        wxWinUIVisualCoordinates::ClientPointToElement(
            window, mappedClient, content, &mappedBack) ==
        wxWinUICoordinateResult::Mapped);
    CHECK(std::abs(mappedBack.X - elementPoint.X) < 0.001);
    CHECK(std::abs(mappedBack.Y - elementPoint.Y) < 0.001);

    winrt::Windows::Foundation::Rect clientBounds{};
    REQUIRE(
        wxWinUIVisualCoordinates::ElementBoundsToClient(
            window,
            content,
            winrt::Windows::Foundation::Rect{
                -0.375f, 1.25f, 11.5f, 9.75f},
            &clientBounds) ==
        wxWinUICoordinateResult::Mapped);
    CHECK(clientBounds.Width > 0.0f);
    CHECK(clientBounds.Height > 0.0f);

    // Direction is not creation-only state. Exercise the complete hot
    // LTR -> RTL transition in both directions before relying on the RTL
    // coordinates below.
    window->SetLayoutDirection(wxLayout_LeftToRight);
    host->FlushSync();
    CHECK(slot->GetContainer().FlowDirection() ==
          winrt::Microsoft::UI::Xaml::FlowDirection::LeftToRight);
    window->SetLayoutDirection(wxLayout_RightToLeft);
    host->FlushSync();
    CHECK(slot->GetContainer().FlowDirection() ==
          winrt::Microsoft::UI::Xaml::FlowDirection::RightToLeft);

    bool inside = false;
    wxPoint physicalLeft;
    REQUIRE(host->TestMapRootPointerPointForSlot(
        window, rect.x / scale, (rect.y + 1) / scale,
        &physicalLeft, &inside));
    CHECK(inside);
    CHECK(physicalLeft.x == rootScreen.left + rect.x);

    wxPoint physicalRight;
    REQUIRE(host->TestMapRootPointerPointForSlot(
        window, rect.GetRight() / scale, (rect.y + 1) / scale,
        &physicalRight, &inside));
    CHECK(inside);
    CHECK(physicalRight.x == rootScreen.left + rect.GetRight());

    // Containment stays continuous even though the Win32 delivery point is
    // integer. These two samples used to be misclassified after lround():
    // one just outside rounded onto the left edge, and one just inside the
    // half-open right edge rounded out of the slot.
    wxPoint fractionalPoint;
    REQUIRE(host->TestMapRootPointerPointForSlot(
        window,
        (rect.x - 0.25) / scale,
        (rect.y + 1.25) / scale,
        &fractionalPoint,
        &inside));
    CHECK_FALSE(inside);
    REQUIRE(host->TestMapRootPointerPointForSlot(
        window,
        (rect.x + rect.width - 0.25) / scale,
        (rect.y + 1.25) / scale,
        &fractionalPoint,
        &inside));
    CHECK(inside);

    // Only the final screen->native-client conversion is mirrored: XAML's
    // physical left edge becomes the RTL HWND's rightmost client pixel.
    const wxPoint nativeLeft = window->ScreenToClient(physicalLeft);
    const wxPoint nativeRight = window->ScreenToClient(physicalRight);
    CHECK(nativeLeft.x >= rect.width - 2);
    CHECK(std::abs(nativeRight.x) <= 1);

    std::vector<int> deliveredX;
    window->Bind(
        wxEVT_MOTION,
        [&deliveredX](wxMouseEvent& event)
        {
            deliveredX.push_back(event.GetX());
        });

    wxWinUIPointerSample move;
    move.kind = wxWinUIInputKind::Move;
    move.device = wxWinUIInputDevice::Mouse;
    move.pointerId = 1;
    move.isPrimary = true;
    move.timestamp = 1;
    move.screenX = physicalLeft.x;
    move.screenY = physicalLeft.y;
    REQUIRE(host->TestRouteSlotPointer(window, move));
    move.timestamp = 2;
    move.screenX = physicalRight.x;
    move.screenY = physicalRight.y;
    REQUIRE(host->TestRouteSlotPointer(window, move));
    REQUIRE(deliveredX.size() == 2);
    CHECK(deliveredX[0] >= rect.width - 2);
    CHECK(std::abs(deliveredX[1]) <= 1);

    // Hit-testing uses the same physical-root point and honours the slot's
    // clip independently of RTL mirroring.
    host->SetSlotClipHeight(window, 12);
    host->FlushSync();
    wxPoint clippedPoint;
    REQUIRE(host->TestMapRootPointerPointForSlot(
        window, rect.x / scale, rect.y / scale + 13.0,
        &clippedPoint, &inside));
    CHECK_FALSE(inside);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::HotThemePropagatesToExistingAndFutureHosts",
          "[HostLifecycle][winui-theme][winui-009]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;

    WinUIAppThemeRestorer restoreTheme;

    wxFrame * const frameA =
        new wxFrame(nullptr, wxID_ANY, "theme-existing-A");
    wxFrame * const frameB =
        new wxFrame(nullptr, wxID_ANY, "theme-existing-B");
    new wxButton(frameA, wxID_ANY, "a");
    new wxButton(frameB, wxID_ANY, "b");
    ShowOffscreenWithoutActivating(frameA, 0);
    ShowOffscreenWithoutActivating(frameB, 1);
    DrainToQuiescence();

    wxWinUITopLevelHost * const hostA =
        wxWinUITopLevelHost::FindForTLW(frameA);
    wxWinUITopLevelHost * const hostB =
        wxWinUITopLevelHost::FindForTLW(frameB);
    REQUIRE(hostA != nullptr);
    REQUIRE(hostB != nullptr);

    wxWinUISetAppTheme(wxWinUIAppTheme::Dark);
    CHECK(hostA->Root().RequestedTheme() == MUX::ElementTheme::Dark);
    CHECK(hostB->Root().RequestedTheme() == MUX::ElementTheme::Dark);

    // Whichever host is visited first re-enters the global setter. The outer
    // Light snapshot must not resume on the remaining host after the nested
    // Dark transaction has become authoritative.
    bool nestedThemeChange = false;
    const auto requestedThemeProperty =
        MUX::FrameworkElement::RequestedThemeProperty();
    const auto reenterTheme =
        [&](const MUX::DependencyObject& sender,
            const MUX::DependencyProperty&)
        {
            const MUX::FrameworkElement element =
                sender.as<MUX::FrameworkElement>();
            if ( !nestedThemeChange &&
                 element.RequestedTheme() == MUX::ElementTheme::Light )
            {
                nestedThemeChange = true;
                wxWinUISetAppTheme(wxWinUIAppTheme::Dark);
            }
        };
    const int64_t themeTokenA =
        hostA->Root().RegisterPropertyChangedCallback(
            requestedThemeProperty, reenterTheme);
    const int64_t themeTokenB =
        hostB->Root().RegisterPropertyChangedCallback(
            requestedThemeProperty, reenterTheme);
    wxWinUISetAppTheme(wxWinUIAppTheme::Light);
    hostA->Root().UnregisterPropertyChangedCallback(
        requestedThemeProperty, themeTokenA);
    hostB->Root().UnregisterPropertyChangedCallback(
        requestedThemeProperty, themeTokenB);
    REQUIRE(nestedThemeChange);
    CHECK(wxWinUIGetAppTheme() == wxWinUIAppTheme::Dark);
    CHECK(hostA->Root().RequestedTheme() == MUX::ElementTheme::Dark);
    CHECK(hostB->Root().RequestedTheme() == MUX::ElementTheme::Dark);

    // A host created after the application-wide change must start in the same
    // effective theme; this catches a split between live-host propagation and
    // the initialization path.
    wxFrame * const frameC =
        new wxFrame(nullptr, wxID_ANY, "theme-future-C");
    new wxButton(frameC, wxID_ANY, "c");
    ShowOffscreenWithoutActivating(frameC, 2);
    DrainToQuiescence();
    wxWinUITopLevelHost * const hostC =
        wxWinUITopLevelHost::FindForTLW(frameC);
    REQUIRE(hostC != nullptr);
    CHECK(hostC->Root().RequestedTheme() == MUX::ElementTheme::Dark);

    wxWinUISetAppTheme(wxWinUIAppTheme::Light);
    CHECK(hostA->Root().RequestedTheme() == MUX::ElementTheme::Light);
    CHECK(hostB->Root().RequestedTheme() == MUX::ElementTheme::Light);
    CHECK(hostC->Root().RequestedTheme() == MUX::ElementTheme::Light);

    wxWinUISetAppTheme(wxWinUIAppTheme::System);
    CHECK(hostA->Root().RequestedTheme() == MUX::ElementTheme::Default);
    CHECK(hostB->Root().RequestedTheme() == MUX::ElementTheme::Default);
    CHECK(hostC->Root().RequestedTheme() == MUX::ElementTheme::Default);

    frameA->Destroy();
    frameB->Destroy();
    frameC->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::ModalReleaseSetKeepsIndependentButtons",
          "[HostLifecycle][winui-input-router]")
{
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "modal-release-set");
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(frame, true);
    REQUIRE(host != nullptr);

    wxWinUIInputAction left;
    left.action = wxWinUIInputActionKind::Dispatch;
    left.kind = wxWinUIInputKind::Press;
    left.device = wxWinUIInputDevice::Mouse;
    left.pointerId = 7;
    left.button = wxWinUIInputButton::Left;
    left.gestureSerial = 101;

    wxWinUIInputAction right = left;
    right.button = wxWinUIInputButton::Right;
    right.gestureSerial = 102;

    REQUIRE(host->TestRememberModalAwaitingRelease(left));
    REQUIRE(host->TestRememberModalAwaitingRelease(right));
    CHECK(host->GetModalAwaitingReleaseCountForTest() == 2);

    // An unrelated serial for the same physical button cannot overwrite the
    // old obligation; the caller must retire it explicitly first.
    wxWinUIInputAction newerRight = right;
    newerRight.gestureSerial = 103;
    CHECK_FALSE(host->TestRememberModalAwaitingRelease(newerRight));
    CHECK(host->GetModalAwaitingReleaseCountForTest() == 2);

    const wxWinUIInputAction takenLeft =
        host->TestTakeModalAwaitingRelease(
            left.device, left.pointerId, left.button);
    CHECK(takenLeft.gestureSerial == left.gestureSerial);
    CHECK(host->GetModalAwaitingReleaseCountForTest() == 1);

    const wxWinUIInputAction missing =
        host->TestTakeModalAwaitingRelease(
            right.device, right.pointerId, wxWinUIInputButton::X1);
    CHECK(missing.gestureSerial == 0);
    CHECK(host->GetModalAwaitingReleaseCountForTest() == 1);

    const wxWinUIInputAction takenRight =
        host->TestTakeModalAwaitingRelease(
            right.device, right.pointerId, right.button);
    CHECK(takenRight.gestureSerial == right.gestureSerial);
    CHECK(host->GetModalAwaitingReleaseCountForTest() == 0);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::PointerContactPolicyIsButtonSpecific",
          "[HostLifecycle][winui-input-state]")
{
    const auto active =
        [](wxWinUIInputDevice device,
           int virtualKey,
           unsigned pointerType,
           unsigned pointerFlags,
           unsigned penFlags = 0)
        {
            return wxWinUITopLevelHost::TestPointerContactActiveSnapshot(
                device, virtualKey, pointerType, pointerFlags, penFlags);
        };

    CHECK(active(wxWinUIInputDevice::Touch, VK_LBUTTON, PT_TOUCH,
                 POINTER_FLAG_INCONTACT));
    CHECK_FALSE(active(wxWinUIInputDevice::Touch, VK_RBUTTON, PT_TOUCH,
                       POINTER_FLAG_INCONTACT));
    CHECK_FALSE(active(wxWinUIInputDevice::Touch, VK_LBUTTON, PT_TOUCH,
                       POINTER_FLAG_INCONTACT | POINTER_FLAG_UP));

    CHECK(active(wxWinUIInputDevice::Pen, VK_LBUTTON, PT_PEN,
                 POINTER_FLAG_INCONTACT));
    CHECK_FALSE(active(wxWinUIInputDevice::Pen, VK_LBUTTON, PT_PEN, 0));
    CHECK(active(wxWinUIInputDevice::Pen, VK_RBUTTON, PT_PEN, 0,
                 PEN_FLAG_BARREL));
    CHECK_FALSE(active(wxWinUIInputDevice::Pen, VK_RBUTTON, PT_PEN,
                       POINTER_FLAG_INCONTACT, 0));
    CHECK_FALSE(active(wxWinUIInputDevice::Pen, VK_RBUTTON, PT_PEN,
                       POINTER_FLAG_UP, PEN_FLAG_BARREL));
    CHECK_FALSE(active(wxWinUIInputDevice::Pen, VK_XBUTTON1, PT_PEN,
                       POINTER_FLAG_INCONTACT, PEN_FLAG_BARREL));
    CHECK_FALSE(active(wxWinUIInputDevice::Pen, VK_LBUTTON, PT_TOUCH,
                       POINTER_FLAG_INCONTACT));
}

TEST_CASE("HostLifecycle::DestroyWithFlushInFlight", "[HostLifecycle]")
{
    PrimeTopWindowHost();

    const unsigned slots0 = wxWinUITopLevelHost::GetLiveSlotCount();
    const unsigned states0 = wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
    const unsigned adds0 = wxWinUITopLevelHost::GetSlotHandlerAddCount();
    const unsigned revokes0 = wxWinUITopLevelHost::GetSlotHandlerRevokeCount();

    for ( int i = 0; i < 25; ++i )
    {
        wxButton * const button =
            new wxButton(wxTheApp->GetTopWindow(), wxID_ANY, "in-flight");
        // queue a coalesced flush (CallAfter) ...
        button->Move(10 + i, 10);
        // ... and destroy the window before it runs: the flush must cope
        delete button;
    }

    DrainDispatch();
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == slots0);
    CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() == states0);
    CHECK(wxWinUITopLevelHost::GetSlotHandlerAddCount() - adds0 ==
          wxWinUITopLevelHost::GetSlotHandlerRevokeCount() - revokes0);
}

TEST_CASE("HostLifecycle::NativeSiblingZOrderAndImpossibleOverlap",
          "[HostLifecycle][winui-zorder]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "z-order-offscreen",
                    wxPoint(-30000, -30000), wxSize(420, 260),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxWindow * const xamlA =
        new wxWindow(frame, wxID_ANY, wxPoint(20, 20), wxSize(180, 120));
    wxWindow * const native =
        new wxWindow(frame, wxID_ANY, wxPoint(60, 45), wxSize(180, 120));
    wxWindow * const xamlB =
        new wxWindow(frame, wxID_ANY, wxPoint(100, 70), wxSize(180, 120));

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(xamlA, true);
    REQUIRE(host != nullptr);
    REQUIRE(host->RegisterSlot(xamlA, MUXC::Border()) != nullptr);
    REQUIRE(host->RegisterSlot(xamlB, MUXC::Border()) != nullptr);
    frame->ShowWithoutActivating();
    DrainToQuiescence();

    REQUIRE(host->FindSlot(xamlA) != nullptr);
    REQUIRE(host->FindSlot(xamlB) != nullptr);
    const auto zIndex =
        [host](wxWindow *window)
        {
            wxWinUISlot * const slot = host->FindSlot(window);
            return slot
                ? MUXC::Canvas::GetZIndex(slot->GetContainer())
                : -1;
        };

    // Raise()/Lower() mutate USER32's sibling chain, not wxWindowList. The
    // WM_WINDOWPOSCHANGED bridge must schedule and mirror both operations.
    xamlA->Raise();
    DrainToQuiescence();
    CHECK(zIndex(xamlA) > zIndex(xamlB));

    xamlA->Lower();
    DrainToQuiescence();
    CHECK(zIndex(xamlA) < zIndex(xamlB));

    // Establish top-to-bottom A > native > B. Relative XAML order remains
    // representable, but native-over-B is not: the one bridge is necessarily
    // above all native children. The overlap counter makes this limit visible.
    constexpr UINT flags =
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER;
    const HWND hwndA = GetHwndOf(xamlA);
    const HWND hwndNative = GetHwndOf(native);
    const HWND hwndB = GetHwndOf(xamlB);
    REQUIRE(hwndA != nullptr);
    REQUIRE(hwndNative != nullptr);
    REQUIRE(hwndB != nullptr);

    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    REQUIRE(::SetWindowPos(hwndA, HWND_TOP, 0, 0, 0, 0, flags));
    REQUIRE(::SetWindowPos(hwndNative, hwndA, 0, 0, 0, 0, flags));
    REQUIRE(::SetWindowPos(hwndB, hwndNative, 0, 0, 0, 0, flags));
    host->FlushSync();
    CHECK(zIndex(xamlA) > zIndex(xamlB));
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() >= 1);

    // Keep the native HWND inside B's raw window rectangle but entirely
    // below its host clip. The diagnostic must be recalculated for both the
    // geometry and clip mutations and use the synchronized visible surface,
    // not GetWindowRect()'s now-irrelevant covered portion.
    REQUIRE(::SetWindowPos(hwndNative, nullptr, 220, 130, 50, 40,
                           SWP_NOZORDER | SWP_NOACTIVATE));
    host->SetSlotClipHeight(xamlB, 8);
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    host->FlushSync();
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() == 0);

    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::NativeLayoutNotificationDoesNotCreateHost",
          "[HostLifecycle][winui-zorder]")
{
    const unsigned hosts0 = wxWinUITopLevelHost::GetLiveHostCount();
    wxFrame * const frameNoHost =
        new wxFrame(nullptr, wxID_ANY, "native-layout-no-host",
                    wxPoint(-30000, -30000), wxSize(240, 140),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxWindow * const nativeNoHost =
        new wxWindow(frameNoHost, wxID_ANY);

    wxWinUITLWHostNotifyNativeLayout(nativeNoHost, true);
    nativeNoHost->SetScrollbar(wxHORIZONTAL, 0, 10, 100);
    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hosts0);

    frameNoHost->Destroy();
    DrainDispatch();

    // Once a TLW does have a host, SetScrollbar() must invalidate the bridge
    // cut-outs through the same coalesced projection-free seam.
    wxFrame * const frameHost =
        new wxFrame(nullptr, wxID_ANY, "native-layout-host",
                    wxPoint(-30000, -30000), wxSize(280, 160),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxWindow * const nativeHost =
        new wxWindow(frameHost, wxID_ANY, wxDefaultPosition,
                     wxDefaultSize, wxHSCROLL);
    wxButton * const button =
        new wxButton(frameHost, wxID_ANY, "slot");
    wxUnusedVar(button);
    frameHost->ShowWithoutActivating();
    DrainToQuiescence();
    REQUIRE(wxWinUITopLevelHost::FindForTLW(frameHost) != nullptr);

    const unsigned schedules =
        wxWinUITopLevelHost::GetFlushScheduleCount();
    nativeHost->SetScrollbar(wxHORIZONTAL, 1, 10, 100);
    CHECK(wxWinUITopLevelHost::GetFlushScheduleCount() > schedules);

    frameHost->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::NestedZOrderFlushKeepsNewestEpoch",
          "[HostLifecycle][winui-zorder]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "nested-z-order-offscreen",
                    wxPoint(-30000, -30000), wxSize(260, 150),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxWindow * const a =
        new wxWindow(frame, wxID_ANY, wxPoint(10, 10), wxSize(120, 80));
    wxWindow * const b =
        new wxWindow(frame, wxID_ANY, wxPoint(40, 30), wxSize(120, 80));
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(a, true);
    REQUIRE(host != nullptr);
    REQUIRE(host->RegisterSlot(a, MUXC::Border()) != nullptr);
    REQUIRE(host->RegisterSlot(b, MUXC::Border()) != nullptr);
    frame->ShowWithoutActivating();
    DrainToQuiescence();

    const HWND hwndA = GetHwndOf(a);
    const HWND hwndB = GetHwndOf(b);
    REQUIRE(hwndA != nullptr);
    REQUIRE(hwndB != nullptr);
    constexpr UINT flags =
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER;

    // The outer snapshot wants A above B. Immediately after its first real
    // SetZIndex boundary, make B topmost and run a complete nested flush. The
    // outer frame must observe the monotone epoch change even though the
    // nested frame has already published/consumed that generation.
    REQUIRE(::SetWindowPos(hwndA, HWND_TOP, 0, 0, 0, 0, flags));
    bool hookRan = false;
    bool movedB = false;
    wxWinUITopLevelHost::TestOnNextZOrderSet(
        [&]()
        {
            hookRan = true;
            movedB =
                ::SetWindowPos(hwndB, HWND_TOP, 0, 0, 0, 0, flags) != 0;
            host->FlushSync();
        });
    host->FlushSync();
    wxWinUITopLevelHost::TestOnNextZOrderSet({});

    REQUIRE(hookRan);
    REQUIRE(movedB);
    wxWinUISlot * const slotA = host->FindSlot(a);
    wxWinUISlot * const slotB = host->FindSlot(b);
    REQUIRE(slotA != nullptr);
    REQUIRE(slotB != nullptr);
    CHECK(MUXC::Canvas::GetZIndex(slotB->GetContainer()) >
          MUXC::Canvas::GetZIndex(slotA->GetContainer()));

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::ReentrantSlotSyncDefersNewStructureEpoch",
          "[HostLifecycle][winui-zorder]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "sync-epoch-offscreen",
                    wxPoint(-30000, -30000), wxSize(260, 150),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxWindow * const a =
        new wxWindow(frame, wxID_ANY, wxPoint(10, 10), wxSize(100, 70));
    wxWindow * const b =
        new wxWindow(frame, wxID_ANY, wxPoint(130, 10), wxSize(100, 70));
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(a, true);
    REQUIRE(host != nullptr);
    REQUIRE(host->RegisterSlot(a, MUXC::Border()) != nullptr);
    REQUIRE(host->RegisterSlot(b, MUXC::Border()) != nullptr);
    frame->ShowWithoutActivating();
    DrainToQuiescence();

    const HWND hwndB = GetHwndOf(b);
    REQUIRE(hwndB != nullptr);
    constexpr UINT flags =
        SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER;

    bool syncHookRan = false;
    bool movedB = false;
    bool zOrderPassRan = false;
    wxWinUITopLevelHost::TestOnNextSlotSynced(
        [&](wxWindow *synced)
        {
            syncHookRan = synced == a;
            movedB =
                ::SetWindowPos(hwndB, nullptr, 120, 20, 0, 0, flags) != 0;
        });
    wxWinUITopLevelHost::TestOnNextZOrderSet(
        [&]()
        {
            zOrderPassRan = true;
        });

    // Only A belongs to the current dirty snapshot. Its completed sync
    // reentrantly moves B, whose cached visible rectangle is therefore stale
    // until the next dirty pass. The outer frame must leave the z-order seam
    // armed rather than publishing B's new epoch against its old cache.
    host->SetSlotClipHeight(a, 9);
    host->FlushSync();
    wxWinUITopLevelHost::TestOnNextSlotSynced({});
    REQUIRE(syncHookRan);
    REQUIRE(movedB);
    CHECK_FALSE(zOrderPassRan);

    host->FlushSync();
    CHECK(zOrderPassRan);
    wxWinUITopLevelHost::TestOnNextZOrderSet({});

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::ZOrderFlushPinsHostAcrossDestroyYield",
          "[HostLifecycle][winui-zorder]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    const unsigned hosts0 = wxWinUITopLevelHost::GetLiveHostCount();
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "z-order-destroy-offscreen",
                    wxPoint(-30000, -30000), wxSize(240, 140),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxWindow * const window =
        new wxWindow(frame, wxID_ANY, wxPoint(10, 10), wxSize(100, 60));
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(window, true);
    REQUIRE(host != nullptr);
    REQUIRE(host->RegisterSlot(window, MUXC::Border()) != nullptr);
    frame->ShowWithoutActivating();
    DrainToQuiescence();

    bool hookRan = false;
    wxWinUITopLevelHost::TestOnNextZOrderSet(
        [&]()
        {
            hookRan = true;
            frame->Destroy();
            // Exercise the deferred host-delete callback while both
            // FlushSync() and RecomputeZOrder() are still on the stack.
            wxYield();
        });
    host->SetSlotClipHeight(window, 9);
    host->FlushSync();
    wxWinUITopLevelHost::TestOnNextZOrderSet({});

    CHECK(hookRan);
    if ( !hookRan )
        frame->Destroy();
    DrainDispatch();
    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hosts0);
}

TEST_CASE("HostLifecycle::TopLevelSlotDiagnosesNativeChildOnce",
          "[HostLifecycle][winui-zorder]")
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "tlw-slot-overlap-offscreen",
                    wxPoint(-30000, -30000), wxSize(260, 160),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxWindow * const native =
        new wxWindow(frame, wxID_ANY, wxPoint(20, 20), wxSize(100, 70));
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(frame, true);
    REQUIRE(host != nullptr);
    REQUIRE(host->RegisterSlot(frame, MUXC::Border()) != nullptr);
    frame->ShowWithoutActivating();
    DrainToQuiescence();

    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    native->Move(21, 20);
    host->FlushSync();
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() == 1);

    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::ClipDIPAndTopLevelVisibility",
          "[HostLifecycle][winui-zorder]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
    namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "clip-visibility-offscreen",
                    wxPoint(-30000, -30000), wxSize(320, 200),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxWindow * const window =
        new wxWindow(frame, wxID_ANY, wxPoint(20, 20), wxSize(180, 80));
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(window, true);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot =
        host->RegisterSlot(window, MUXC::Border());
    REQUIRE(slot != nullptr);

    frame->ShowWithoutActivating();
    DrainToQuiescence();
    host->FlushSync();
    CHECK(slot->GetContainer().Visibility() == MUX::Visibility::Visible);

    host->SetSlotClipHeight(window, 12);
    host->FlushSync();
    const auto clip =
        slot->GetContainer().Clip().try_as<MUXM::RectangleGeometry>();
    REQUIRE(clip != nullptr);
    CHECK(std::abs(clip.Rect().Height - 12.0f) < 1.0f);

    frame->Show(false);
    DrainToQuiescence();
    CHECK(slot->GetContainer().Visibility() == MUX::Visibility::Collapsed);

    frame->ShowWithoutActivating();
    DrainToQuiescence();
    CHECK(slot->GetContainer().Visibility() == MUX::Visibility::Visible);

    frame->Iconize(true);
    DrainToQuiescence();
    REQUIRE(::IsIconic(GetHwndOf(frame)));
    CHECK(slot->GetContainer().Visibility() == MUX::Visibility::Collapsed);

    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::ExactSlotClipRectIsPhysicalAndGenerationSafe",
          "[HostLifecycle][winui-zorder][clip-rect]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
    namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

    // This is the exact conversion pair used by wxWinUIControlHost and by
    // SyncSlot. In particular, no integer DIP intermediate may turn a one
    // pixel origin into two pixels at a fractional monitor scale.
    const wxRect physicalClip(1, 3, 137, 59);
    for ( const double scale : { 1.25, 1.5 } )
    {
        CAPTURE(scale);
        const wxWinUIClipRectDIP dip =
            wxWinUITopLevelHost::PhysicalClipRectToDIP(
                physicalClip, scale);
        CHECK(std::abs(dip.x * scale - physicalClip.x) < 0.000001);
        CHECK(std::abs(dip.y * scale - physicalClip.y) < 0.000001);
        CHECK(wxWinUITopLevelHost::ClipRectDIPToPhysical(
                  dip, scale, wxSize(300, 180)) == physicalClip);
    }

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "clip-rect-offscreen",
                    wxPoint(-30000, -30000), wxSize(360, 240),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxWindow * const window =
        new wxWindow(frame, wxID_ANY, wxPoint(20, 20), wxSize(200, 100));
    window->SetLayoutDirection(wxLayout_RightToLeft);
    wxWinUITopLevelHost *host =
        wxWinUITopLevelHost::ForWindow(window, true);
    REQUIRE(host != nullptr);
    wxWinUISlot *slot =
        host->RegisterSlot(window, MUXC::Border());
    REQUIRE(slot != nullptr);

    frame->ShowWithoutActivating();
    DrainToQuiescence();
    host->SetSlotClipRect(window, wxRect(17, 9, 61, 23));
    host->FlushSync();

    auto clip =
        slot->GetContainer().Clip().try_as<MUXM::RectangleGeometry>();
    REQUIRE(clip != nullptr);
    // Slot clip coordinates stay physical-LTR even while content flow is RTL.
    CHECK(std::abs(clip.Rect().X - 17.0f) < 1.0f);
    CHECK(std::abs(clip.Rect().Y - 9.0f) < 1.0f);
    CHECK(std::abs(clip.Rect().Width - 61.0f) < 1.0f);
    CHECK(std::abs(clip.Rect().Height - 23.0f) < 1.0f);

    bool nestedWriterRan = false;
    wxWinUITopLevelHost::TestOnNextSlotSynced(
        [&](wxWindow *synced)
        {
            if ( synced == window )
            {
                nestedWriterRan = true;
                host->SetSlotClipRect(window, wxRect(4, 6, 77, 19));
            }
        });
    host->SetSlotClipRect(window, wxRect(3, 5, 70, 18));
    host->FlushSync();
    REQUIRE(nestedWriterRan);
    host->FlushSync();
    clip = slot->GetContainer().Clip().try_as<MUXM::RectangleGeometry>();
    REQUIRE(clip != nullptr);
    CHECK(std::abs(clip.Rect().X - 4.0f) < 1.0f);
    CHECK(std::abs(clip.Rect().Y - 6.0f) < 1.0f);
    CHECK(std::abs(clip.Rect().Width - 77.0f) < 1.0f);
    CHECK(std::abs(clip.Rect().Height - 19.0f) < 1.0f);

    const double sourceScale = host->GetScale();
    REQUIRE(sourceScale > 0.0);
    wxRect slotRect = slot->GetRectInTLW();
    const wxRect localClip =
        wxWinUITopLevelHost::ClipRectDIPToPhysical(
            wxWinUIClipRectDIP{4.0, 6.0, 77.0, 19.0},
            sourceScale,
            slotRect.GetSize());
    REQUIRE(!localClip.IsEmpty());
    bool inside = false;
    wxPoint screen;
    REQUIRE(host->TestMapRootPointerPointForSlot(
        window,
        (slotRect.x + localClip.x + localClip.width / 2.0) /
            sourceScale,
        (slotRect.y + localClip.y + localClip.height / 2.0) /
            sourceScale,
        &screen,
        &inside));
    CHECK(inside);
    REQUIRE(host->TestMapRootPointerPointForSlot(
        window,
        (slotRect.x + localClip.x - 0.25) / sourceScale,
        (slotRect.y + localClip.y + 1.0) / sourceScale,
        &screen,
        &inside));
    CHECK_FALSE(inside);

    // Empty is distinct from unbounded and removes both render and input.
    host->SetSlotClipRect(window, wxRect(0, 0, 0, 0));
    host->FlushSync();
    CHECK(slot->GetContainer().Visibility() == MUX::Visibility::Collapsed);

    host->ClearSlotClip(window);
    host->FlushSync();
    CHECK(slot->GetContainer().Visibility() == MUX::Visibility::Visible);
    CHECK(slot->GetContainer().Clip() == nullptr);

    // The legacy height API remains full-width after a resize.
    host->SetSlotClipHeight(window, 14);
    window->SetSize(wxSize(260, 120));
    host->FlushSync();
    clip = slot->GetContainer().Clip().try_as<MUXM::RectangleGeometry>();
    REQUIRE(clip != nullptr);
    CHECK(std::abs(clip.Rect().X) < 1.0f);
    CHECK(std::abs(clip.Rect().Width -
                   slot->GetContainer().Width()) < 1.0f);
    CHECK(std::abs(clip.Rect().Height - 14.0f) < 1.0f);

    // A cross-TLW migration transfers the fractional contract and its
    // generation, then recomputes the exact physical rectangle using the
    // destination XamlRoot scale. Rendering and hit-testing must agree.
    const wxWinUIClipRectDIP migratedClip =
        wxWinUITopLevelHost::PhysicalClipRectToDIP(
            physicalClip, sourceScale);
    host->SetSlotClipRect(
        window,
        migratedClip.x,
        migratedClip.y,
        migratedClip.width,
        migratedClip.height);
    host->FlushSync();

    wxFrame * const destination =
        new wxFrame(nullptr, wxID_ANY, "clip-rect-destination-offscreen",
                    wxPoint(-29500, -29500), wxSize(360, 240),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    destination->ShowWithoutActivating();
    DrainToQuiescence();
    REQUIRE(window->Reparent(destination));
    host = wxWinUITopLevelHost::FindSlotOwner(window);
    REQUIRE(host != nullptr);
    slot = host->FindSlot(window);
    REQUIRE(slot != nullptr);
    host->FlushSync();

    const double destinationScale = host->GetScale();
    REQUIRE(destinationScale > 0.0);
    slotRect = slot->GetRectInTLW();
    const wxRect migratedPhysical =
        wxWinUITopLevelHost::ClipRectDIPToPhysical(
            migratedClip, destinationScale, slotRect.GetSize());
    REQUIRE(!migratedPhysical.IsEmpty());
    clip = slot->GetContainer().Clip().try_as<MUXM::RectangleGeometry>();
    REQUIRE(clip != nullptr);
    CHECK(std::abs(clip.Rect().X -
                   migratedPhysical.x / destinationScale) < 0.001);
    CHECK(std::abs(clip.Rect().Y -
                   migratedPhysical.y / destinationScale) < 0.001);
    CHECK(std::abs(clip.Rect().Width -
                   migratedPhysical.width / destinationScale) < 0.001);
    CHECK(std::abs(clip.Rect().Height -
                   migratedPhysical.height / destinationScale) < 0.001);

    REQUIRE(host->TestMapRootPointerPointForSlot(
        window,
        (slotRect.x + migratedPhysical.x + 0.5) /
            destinationScale,
        (slotRect.y + migratedPhysical.y + 0.5) /
            destinationScale,
        &screen,
        &inside));
    CHECK(inside);
    REQUIRE(host->TestMapRootPointerPointForSlot(
        window,
        (slotRect.x + migratedPhysical.x - 0.25) /
            destinationScale,
        (slotRect.y + migratedPhysical.y + 0.5) /
            destinationScale,
        &screen,
        &inside));
    CHECK_FALSE(inside);

    wxWinUITopLevelHost::TestOnNextSlotSynced({});
    destination->Destroy();
    frame->Destroy();
    DrainDispatch();
}

TEST_CASE("HostLifecycle::DetachClipWriterWinsMigration",
          "[HostLifecycle][winui-zorder][clip-rect][migration][reentrancy]")
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

    wxFrame * const source =
        new wxFrame(nullptr, wxID_ANY, "clip-writer-source-offscreen",
                    wxPoint(-30000, -30000), wxSize(360, 240),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxFrame * const destination =
        new wxFrame(nullptr, wxID_ANY, "clip-writer-destination-offscreen",
                    wxPoint(-29500, -29500), wxSize(360, 240),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    wxWindow * const window =
        new wxWindow(source, wxID_ANY, wxPoint(20, 20), wxSize(220, 110));
    wxWinUIControlHostProbe * const probe =
        wxWinUIControlHostProbe::Create(window);
    REQUIRE(probe != nullptr);

    source->ShowWithoutActivating();
    destination->ShowWithoutActivating();
    DrainToQuiescence();

    // Establish a recognizably different pre-migration value. The callback
    // below must replace this through wxWinUIControlHost, not by reaching into
    // the source slot directly.
    probe->SetBridgeClipRect(wxRect(2, 4, 43, 17));
    wxWinUITopLevelHost *host =
        wxWinUITopLevelHost::FindSlotOwner(window);
    REQUIRE(host != nullptr);
    host->FlushSync();

    const wxRect newestPhysical(7, 5, 91, 31);
    wxWinUIClipRectDIP newestDIP;
    bool detachWriterRan = false;
    wxWinUITopLevelHost::TestOnNextContentTransactionStarted(
        [&](wxWindow *transitioning)
        {
            if ( transitioning != window )
                return;

            detachWriterRan = true;
            double authoredScale = window->GetDPIScaleFactor();
            if ( !std::isfinite(authoredScale) || authoredScale <= 0.0 )
                authoredScale = 1.0;
            newestDIP =
                wxWinUITopLevelHost::PhysicalClipRectToDIP(
                    newestPhysical, authoredScale);
            probe->SetBridgeClipRect(newestPhysical);
        });
    const auto clearDetachHook = wxMakeGuard([]()
    {
        wxWinUITopLevelHost::TestOnNextContentTransactionStarted({});
    });
    wxUnusedVar(clearDetachHook);

    REQUIRE(window->Reparent(destination));
    REQUIRE(detachWriterRan);
    host = wxWinUITopLevelHost::FindSlotOwner(window);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(window);
    REQUIRE(slot != nullptr);
    host->FlushSync();

    const double destinationScale = host->GetScale();
    REQUIRE(destinationScale > 0.0);
    const wxRect slotRect = slot->GetRectInTLW();
    const wxRect expectedPhysical =
        wxWinUITopLevelHost::ClipRectDIPToPhysical(
            newestDIP, destinationScale, slotRect.GetSize());
    REQUIRE(!expectedPhysical.IsEmpty());
    const auto clip =
        slot->GetContainer().Clip().try_as<MUXM::RectangleGeometry>();
    REQUIRE(clip != nullptr);
    CHECK(std::abs(clip.Rect().X -
                   expectedPhysical.x / destinationScale) < 0.001);
    CHECK(std::abs(clip.Rect().Y -
                   expectedPhysical.y / destinationScale) < 0.001);
    CHECK(std::abs(clip.Rect().Width -
                   expectedPhysical.width / destinationScale) < 0.001);
    CHECK(std::abs(clip.Rect().Height -
                   expectedPhysical.height / destinationScale) < 0.001);
    CHECK(slot->GetContainer().Visibility() == MUX::Visibility::Visible);

    // Rendering and the root-pointer adapter must consume the same winning
    // rectangle after the migration.
    wxPoint screen;
    bool inside = false;
    REQUIRE(host->TestMapRootPointerPointForSlot(
        window,
        (slotRect.x + expectedPhysical.x +
         expectedPhysical.width / 2.0) / destinationScale,
        (slotRect.y + expectedPhysical.y +
         expectedPhysical.height / 2.0) / destinationScale,
        &screen,
        &inside));
    CHECK(inside);
    REQUIRE(host->TestMapRootPointerPointForSlot(
        window,
        (slotRect.x + expectedPhysical.x - 0.25) / destinationScale,
        (slotRect.y + expectedPhysical.y + 0.5) / destinationScale,
        &screen,
        &inside));
    CHECK_FALSE(inside);

    delete probe;
    DrainDispatch(3);
    destination->Destroy();
    source->Destroy();
    DrainDispatch();
}

#if wxUSE_NOTEBOOK

TEST_CASE("HostLifecycle::NotebookInitialBandClip",
          "[HostLifecycle][winui-zorder]")
{
    namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "notebook-clip-offscreen",
                    wxPoint(-30000, -30000), wxSize(300, 200),
                    wxFRAME_NO_TASKBAR | wxFRAME_TOOL_WINDOW);
    const HWND hwndFrame = GetHwndOf(frame);
    REQUIRE(hwndFrame != nullptr);

    ::SetLastError(ERROR_SUCCESS);
    const LONG_PTR oldStyle =
        ::GetWindowLongPtr(hwndFrame, GWL_EXSTYLE);
    REQUIRE( (oldStyle || ::GetLastError() == ERROR_SUCCESS) );
    ::SetLastError(ERROR_SUCCESS);
    (void)::SetWindowLongPtr(
        hwndFrame, GWL_EXSTYLE,
        oldStyle | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
    REQUIRE(::GetLastError() == ERROR_SUCCESS);

    wxNotebook * const notebook =
        new wxNotebook(frame, wxID_ANY, wxPoint(20, 20), wxSize(240, 140));
    wxPanel * const page = new wxPanel(notebook);
    REQUIRE(notebook->AddPage(page, "first", true));

    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(notebook);
    REQUIRE(host != nullptr);
    wxWinUISlot * const slot = host->FindSlot(notebook);
    REQUIRE(slot != nullptr);

    frame->ShowWithoutActivating();
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    host->FlushSync();

    const auto clip =
        slot->GetContainer().Clip().try_as<MUXM::RectangleGeometry>();
    REQUIRE(clip != nullptr);
    const double pageTopDIP =
        static_cast<double>(notebook->ToDIP(page->GetPosition().y));
    CHECK(pageTopDIP > 0.0);
    CHECK(std::abs(clip.Rect().Height - pageTopDIP) < 1.0);
    CHECK(clip.Rect().Height < slot->GetContainer().Height());
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() == 0);

    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    frame->Destroy();
    DrainDispatch();
}

#endif // wxUSE_NOTEBOOK

#endif // __WXWINUI__ && wxUSE_WINUI3

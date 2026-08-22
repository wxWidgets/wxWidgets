///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiwebview.cpp
// Purpose:     test-only WinUI XAML WebView2/shared-host qualification spike
// Author:      wxWidgets development team
// Created:     2026-08-08
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_WEBVIEW && \
        wxUSE_WEBVIEW_EDGE

#include "wx/winui/xamlhost.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/button.h"
    #include "wx/frame.h"
    #include "wx/panel.h"
    #include "wx/utils.h"
#endif // WX_PRECOMP

#include "wx/msw/private.h"
#include "wx/stopwatch.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"
#include "wx/winui/winui.h"

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
    #include "wx/dataobj.h"
    #include "wx/winui/private/dropbroker.h"
#endif

#include <UIAutomation.h>

// ICoreWebView2Interop2 is the documented ABI bridge from the projected
// Microsoft.Web.WebView2.Core object to the COM pointer returned today by the
// Edge backend's wxWebView::GetNativeBackend(). This header is supplied by the
// same pinned Microsoft.Web.WebView2 package that generated the projection.
#include <WebView2Interop.h>

#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.Web.WebView2.Core.h>
#include <winrt/Windows.Foundation.h>

#include <cmath>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace
{

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;
namespace WV2 = winrt::Microsoft::Web::WebView2::Core;
namespace WF = winrt::Windows::Foundation;

constexpr unsigned InitWaitRounds = 1000;       // <= 20 seconds
constexpr unsigned NavigationWaitRounds = 750; // <= 15 seconds
constexpr unsigned ScriptWaitRounds = 500;     // <= 10 seconds

const wchar_t WebDocumentA[] = LR"HTML(
<!doctype html>
<html>
<head><meta charset="utf-8"><title>wxWinUI WebView2 A</title></head>
<body>
  <input id="editor" value="focus target">
  <div id="marker">ready-a</div>
  <div id="dropzone">drop zone</div>
  <script>
    window.wxDropCount = 0;
    document.addEventListener('dragover', function(e) { e.preventDefault(); });
    document.addEventListener('drop', function(e) {
      e.preventDefault();
      window.wxDropCount++;
      document.getElementById('marker').textContent =
        'drop:' + e.dataTransfer.getData('text/plain');
      window.chrome.webview.postMessage('dom-drop');
    });
    window.chrome.webview.postMessage('dom-ready-a');
  </script>
</body>
</html>)HTML";

const wchar_t WebDocumentB[] = LR"HTML(
<!doctype html>
<html>
<head><meta charset="utf-8"><title>wxWinUI WebView2 B</title></head>
<body>
  <input id="editor" value="focus target b">
  <div id="marker">ready-b</div>
  <script>
    window.chrome.webview.postMessage('dom-ready-b');
  </script>
</body>
</html>)HTML";

void DrainDispatch(unsigned rounds = 1)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(2);
    }
}

template <typename Predicate>
bool DrainUntil(Predicate predicate, unsigned rounds)
{
    for ( unsigned i = 0; i < rounds; ++i )
    {
        if ( predicate() )
            return true;
        DrainDispatch();
    }

    return predicate();
}

bool DrainToQuiescence()
{
    for ( unsigned i = 0; i < 80; ++i )
    {
        const unsigned schedules =
            wxWinUITopLevelHost::GetFlushScheduleCount();
        const unsigned runs = wxWinUITopLevelHost::GetFlushRunCount();
        const unsigned attempts =
            wxWinUITopLevelHost::GetFlushCallbackAttemptCount();
        DrainDispatch(2);
        if ( schedules == wxWinUITopLevelHost::GetFlushScheduleCount() &&
             runs == wxWinUITopLevelHost::GetFlushRunCount() &&
             attempts ==
                 wxWinUITopLevelHost::GetFlushCallbackAttemptCount() )
        {
            return true;
        }
    }

    return false;
}

struct HostSnapshot
{
    static HostSnapshot Capture()
    {
        HostSnapshot result;
        result.hosts = wxWinUITopLevelHost::GetLiveHostCount();
        result.hostLifetimes =
            wxWinUITopLevelHost::GetLiveHostLifetimeCount();
        result.slots = wxWinUITopLevelHost::GetLiveSlotCount();
        result.slotLifetimes =
            wxWinUITopLevelHost::GetLiveSlotLifetimeCount();
        result.subclasses =
            wxWinUITopLevelHost::GetLiveSubclassContextCount();
        result.loadedHooks =
            wxWinUITopLevelHost::GetLiveLoadedHookCount();
        result.contentObservers =
            wxWinUITopLevelHost::GetLiveContentLoadedObserverCountForTest();
        result.automationObservers =
            wxWinUITopLevelHost::
                GetLiveAutomationNameStyleObserverCountForTest();
        result.invisibleProviders =
            wxWinUITestGetLiveInvisibleShellProviderCount();
        result.pendingFocusMigrations =
            wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest();
        result.slotAdds = wxWinUITopLevelHost::GetSlotHandlerAddCount();
        result.slotRevokes =
            wxWinUITopLevelHost::GetSlotHandlerRevokeCount();
        result.rootAdds = wxWinUITopLevelHost::GetRootHandlerAddCount();
        result.rootRevokes =
            wxWinUITopLevelHost::GetRootHandlerRevokeCount();
        return result;
    }

    bool IsRestored() const
    {
        return wxWinUITopLevelHost::GetLiveHostCount() == hosts &&
               wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
                   hostLifetimes &&
               wxWinUITopLevelHost::GetLiveSlotCount() == slots &&
               wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
                   slotLifetimes &&
               wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
                   subclasses &&
               wxWinUITopLevelHost::GetLiveLoadedHookCount() == loadedHooks &&
               wxWinUITopLevelHost::
                       GetLiveContentLoadedObserverCountForTest() ==
                   contentObservers &&
               wxWinUITopLevelHost::
                       GetLiveAutomationNameStyleObserverCountForTest() ==
                   automationObservers &&
               wxWinUITestGetLiveInvisibleShellProviderCount() ==
                   invisibleProviders &&
               wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
                   pendingFocusMigrations &&
               wxWinUITopLevelHost::GetSlotHandlerAddCount() - slotAdds ==
                   wxWinUITopLevelHost::GetSlotHandlerRevokeCount() -
                       slotRevokes &&
               wxWinUITopLevelHost::GetRootHandlerAddCount() - rootAdds ==
                   wxWinUITopLevelHost::GetRootHandlerRevokeCount() -
                       rootRevokes;
    }

    void CheckRestored(bool requireHostTraffic = true) const
    {
        CHECK(wxWinUITopLevelHost::GetLiveHostCount() == hosts);
        CHECK(wxWinUITopLevelHost::GetLiveHostLifetimeCount() ==
              hostLifetimes);
        CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == slots);
        CHECK(wxWinUITopLevelHost::GetLiveSlotLifetimeCount() ==
              slotLifetimes);
        CHECK(wxWinUITopLevelHost::GetLiveSubclassContextCount() ==
              subclasses);
        CHECK(wxWinUITopLevelHost::GetLiveLoadedHookCount() == loadedHooks);
        CHECK(wxWinUITopLevelHost::
                  GetLiveContentLoadedObserverCountForTest() ==
              contentObservers);
        CHECK(wxWinUITopLevelHost::
                  GetLiveAutomationNameStyleObserverCountForTest() ==
              automationObservers);
        CHECK(wxWinUITestGetLiveInvisibleShellProviderCount() ==
              invisibleProviders);
        CHECK(wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
              pendingFocusMigrations);

        const unsigned addedSlots =
            wxWinUITopLevelHost::GetSlotHandlerAddCount() - slotAdds;
        const unsigned revokedSlots =
            wxWinUITopLevelHost::GetSlotHandlerRevokeCount() - slotRevokes;
        const unsigned addedRoots =
            wxWinUITopLevelHost::GetRootHandlerAddCount() - rootAdds;
        const unsigned revokedRoots =
            wxWinUITopLevelHost::GetRootHandlerRevokeCount() - rootRevokes;
        if ( requireHostTraffic )
        {
            CHECK(addedSlots > 0);
            CHECK(addedRoots > 0);
        }
        CHECK(addedSlots == revokedSlots);
        CHECK(addedRoots == revokedRoots);
    }

    unsigned hosts = 0;
    unsigned hostLifetimes = 0;
    unsigned slots = 0;
    unsigned slotLifetimes = 0;
    unsigned subclasses = 0;
    unsigned loadedHooks = 0;
    unsigned contentObservers = 0;
    unsigned automationObservers = 0;
    unsigned invisibleProviders = 0;
    unsigned pendingFocusMigrations = 0;
    unsigned slotAdds = 0;
    unsigned slotRevokes = 0;
    unsigned rootAdds = 0;
    unsigned rootRevokes = 0;
};

bool ShowOffscreenWithoutActivation(wxFrame *frame, unsigned ordinal)
{
    if ( !frame )
        return false;

    const int left = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int top = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int width = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int height = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
    const RECT desktop = { left, top, left + width, top + height };
    frame->Move(wxPoint(left - 6000 - static_cast<int>(ordinal) * 500,
                        top - 6000));

    const auto outside = [frame, desktop]()
    {
        RECT window = {};
        RECT intersection = {};
        return ::GetWindowRect(GetHwndOf(frame), &window) &&
               !::IntersectRect(&intersection, &window, &desktop);
    };
    if ( !outside() )
        return false;

    frame->ShowWithoutActivating();
    if ( !outside() )
    {
        frame->Hide();
        return false;
    }
    return true;
}

struct CallbackLedger
{
    void RecordCloseFailure(HRESULT hr)
    {
        if ( SUCCEEDED(closeHr) )
            closeHr = hr;
    }

    unsigned accepted = 0;
    unsigned rejectedGeneration = 0;
    unsigned expiredState = 0;
    unsigned closeTransactions = 0;
    unsigned peerCloseAttempts = 0;
    HRESULT closeHr = S_OK;
};

struct WebViewState : public std::enable_shared_from_this<WebViewState>
{
    bool IsCurrent(std::uint64_t expected) const
    {
        return alive && generation == expected;
    }

    void RecordFailure(HRESULT hr)
    {
        if ( SUCCEEDED(callbackHr) )
            callbackHr = hr;
    }

    bool alive = true;
    std::uint64_t generation = 1;
    HRESULT callbackHr = S_OK;
    HRESULT initializationHr = E_PENDING;
    bool initializationEventSeen = false;
    bool initializationAsyncDone = false;
    WF::AsyncStatus initializationStatus = WF::AsyncStatus::Started;
    unsigned callbackSequence = 0;
    unsigned initializationEventSequence = 0;
    unsigned initializationAsyncSequence = 0;

    unsigned navigationStarting = 0;
    unsigned navigationCompleted = 0;
    unsigned successfulNavigations = 0;
    unsigned vetoedNavigations = 0;
    unsigned webMessages = 0;
    unsigned historyChanges = 0;
    unsigned processFailures = 0;
    std::wstring lastUri;
    std::wstring lastMessage;
    std::wstring vetoNeedle;
    std::function<void()> navigationStartingAction;

    unsigned scriptEpoch = 0;
    unsigned completedScriptEpoch = 0;
    WF::AsyncStatus scriptStatus = WF::AsyncStatus::Started;
    HRESULT scriptHr = E_PENDING;
    std::wstring scriptResult;

    winrt::weak_ref<MUXC::WebView2> peer;
    WV2::CoreWebView2 core{ nullptr };
    winrt::com_ptr<::ICoreWebView2> nativeCore;
    winrt::com_ptr<::IUnknown> nativeIdentity;
    std::wstring runtimeVersion;

    winrt::event_token coreMessageToken{};
    winrt::event_token coreNavigationStartingToken{};
    winrt::event_token coreNavigationCompletedToken{};
    winrt::event_token historyToken{};
    winrt::event_token processFailedToken{};
    bool hasCoreMessageToken = false;
    bool hasCoreNavigationStartingToken = false;
    bool hasCoreNavigationCompletedToken = false;
    bool hasHistoryToken = false;
    bool hasProcessFailedToken = false;
};

struct CoreIdentitySnapshot
{
    WV2::CoreWebView2 projected{ nullptr };
    winrt::com_ptr<::ICoreWebView2> native;
    winrt::com_ptr<::IUnknown> identity;
};

HRESULT CaptureNativeCoreIdentity(
    const WV2::CoreWebView2& core,
    winrt::com_ptr<::ICoreWebView2> *native,
    winrt::com_ptr<::IUnknown> *identity) noexcept
{
    if ( !native || !identity )
        return E_POINTER;

    *native = nullptr;
    *identity = nullptr;
    if ( !core )
        return E_NOINTERFACE;

    winrt::com_ptr<ICoreWebView2Interop2> interop;
    HRESULT hr = winrt::get_unknown(core)->QueryInterface(
        IID_PPV_ARGS(interop.put()));
    if ( SUCCEEDED(hr) )
        hr = interop->GetComICoreWebView2(native->put());
    if ( SUCCEEDED(hr) && *native )
        hr = (*native)->QueryInterface(IID_PPV_ARGS(identity->put()));
    if ( SUCCEEDED(hr) && (!*native || !*identity) )
        hr = E_NOINTERFACE;
    return hr;
}

HRESULT CapturePeerCoreIdentity(const MUXC::WebView2& peer,
                                CoreIdentitySnapshot *snapshot) noexcept
{
    if ( !snapshot )
        return E_POINTER;

    *snapshot = CoreIdentitySnapshot{};
    try
    {
        if ( peer )
            snapshot->projected = peer.CoreWebView2();
        return CaptureNativeCoreIdentity(
            snapshot->projected, &snapshot->native, &snapshot->identity);
    }
    catch ( const winrt::hresult_error& e )
    {
        return e.code().value;
    }
    catch ( ... )
    {
        return E_UNEXPECTED;
    }
}

template <typename Callback>
void WithCurrentState(const std::weak_ptr<WebViewState>& weakState,
                      const std::shared_ptr<CallbackLedger>& ledger,
                      std::uint64_t generation,
                      Callback callback) noexcept
{
    const std::shared_ptr<WebViewState> state = weakState.lock();
    if ( !state )
    {
        ++ledger->expiredState;
        return;
    }
    if ( !state->IsCurrent(generation) )
    {
        ++ledger->rejectedGeneration;
        return;
    }

    ++ledger->accepted;
    try
    {
        callback(*state);
    }
    catch ( const winrt::hresult_error& e )
    {
        state->RecordFailure(e.code().value);
    }
    catch ( ... )
    {
        state->RecordFailure(E_UNEXPECTED);
    }
}

void InstallCoreHandlers(const std::shared_ptr<WebViewState>& state,
                         const std::shared_ptr<CallbackLedger>& ledger,
                         std::uint64_t generation)
{
    const std::weak_ptr<WebViewState> weakState(state);

    state->coreMessageToken = state->core.WebMessageReceived(
        [weakState, ledger, generation](
            const WV2::CoreWebView2&,
            const WV2::CoreWebView2WebMessageReceivedEventArgs& args)
        {
            WithCurrentState(
                weakState, ledger, generation,
                [&args](WebViewState& current)
                {
                    ++current.webMessages;
                    current.lastMessage =
                        args.TryGetWebMessageAsString().c_str();
                });
        });
    state->hasCoreMessageToken = true;

    state->coreNavigationStartingToken = state->core.NavigationStarting(
        [weakState, ledger, generation](
            const WV2::CoreWebView2&,
            const WV2::CoreWebView2NavigationStartingEventArgs& args)
        {
            WithCurrentState(
                weakState, ledger, generation,
                [&args](WebViewState& current)
                {
                    ++current.navigationStarting;
                    current.lastUri = args.Uri().c_str();
                    if ( !current.vetoNeedle.empty() &&
                         current.lastUri.find(current.vetoNeedle) !=
                             std::wstring::npos )
                    {
                        args.Cancel(true);
                        ++current.vetoedNavigations;
                    }

                    const std::function<void()> action =
                        current.navigationStartingAction;
                    current.navigationStartingAction = nullptr;
                    if ( action )
                        action();
                });
        });
    state->hasCoreNavigationStartingToken = true;

    state->coreNavigationCompletedToken = state->core.NavigationCompleted(
        [weakState, ledger, generation](
            const WV2::CoreWebView2&,
            const WV2::CoreWebView2NavigationCompletedEventArgs& args)
        {
            WithCurrentState(
                weakState, ledger, generation,
                [&args](WebViewState& current)
                {
                    ++current.navigationCompleted;
                    if ( args.IsSuccess() )
                        ++current.successfulNavigations;
                });
        });
    state->hasCoreNavigationCompletedToken = true;

    state->historyToken = state->core.HistoryChanged(
        [weakState, ledger, generation](
            const WV2::CoreWebView2&, const WF::IInspectable&)
        {
            WithCurrentState(
                weakState, ledger, generation,
                [](WebViewState& current)
                {
                    ++current.historyChanges;
                });
        });
    state->hasHistoryToken = true;

    state->processFailedToken = state->core.ProcessFailed(
        [weakState, ledger, generation](
            const WV2::CoreWebView2&,
            const WV2::CoreWebView2ProcessFailedEventArgs&)
        {
            WithCurrentState(
                weakState, ledger, generation,
                [](WebViewState& current)
                {
                    ++current.processFailures;
                });
        });
    state->hasProcessFailedToken = true;

    state->runtimeVersion =
        state->core.Environment().BrowserVersionString().c_str();

    const HRESULT hr = CaptureNativeCoreIdentity(
        state->core, &state->nativeCore, &state->nativeIdentity);
    if ( FAILED(hr) )
        state->RecordFailure(hr);
}

class WebViewSession final
{
public:
    WebViewSession(wxWinUIXamlHost *owner,
                   const std::shared_ptr<CallbackLedger>& ledger)
        : m_owner(owner), m_ledger(ledger),
          m_state(std::make_shared<WebViewState>()),
          m_peer(MUXC::WebView2())
    {
        m_state->peer = winrt::make_weak(m_peer);
    }

    ~WebViewSession()
    {
        Close();
    }

    bool Attach()
    {
        wxWinUIXamlHost * const owner = m_owner.get();
        if ( !owner || !m_peer )
            return false;

        wxWinUITopLevelHost * const host =
            wxWinUITopLevelHost::ReconcileSlotOwner(owner, true);
        if ( !host || !host->RegisterSlot(owner, m_peer) )
            return false;

        host->SetSlotPreferredFocus(owner, m_peer);
        return true;
    }

    void BeginInitialize()
    {
        REQUIRE(m_peer != nullptr);
        REQUIRE(m_state != nullptr);

        const std::weak_ptr<WebViewState> weakState(m_state);
        const std::uint64_t generation = m_state->generation;
        m_initializedToken = m_peer.CoreWebView2Initialized(
            [weakState, ledger = m_ledger, generation](
                const MUXC::WebView2& sender,
                const MUXC::CoreWebView2InitializedEventArgs& args)
            {
                WithCurrentState(
                    weakState, ledger, generation,
                    [&sender, &args, ledger, generation](
                        WebViewState& current)
                    {
                        current.initializationEventSeen = true;
                        current.initializationEventSequence =
                            ++current.callbackSequence;
                        current.initializationHr = args.Exception().value;
                        if ( FAILED(current.initializationHr) )
                            return;

                        current.core = sender.CoreWebView2();
                        if ( !current.core )
                        {
                            current.RecordFailure(E_NOINTERFACE);
                            return;
                        }
                        InstallCoreHandlers(
                            current.shared_from_this(), ledger, generation);
                    });
            });
        m_hasInitializedToken = true;

        m_initializeOperation = m_peer.EnsureCoreWebView2Async();
        m_initializeOperation.Completed(
            [weakState, ledger = m_ledger, generation](
                const WF::IAsyncAction& operation, WF::AsyncStatus status)
            {
                WithCurrentState(
                    weakState, ledger, generation,
                    [&operation, status](WebViewState& current)
                    {
                        current.initializationStatus = status;
                        current.initializationAsyncSequence =
                            ++current.callbackSequence;
                        if ( status == WF::AsyncStatus::Completed )
                            operation.GetResults();
                        else if ( status == WF::AsyncStatus::Error )
                            current.initializationHr =
                                operation.ErrorCode().value;
                        current.initializationAsyncDone = true;
                    });
            });
    }

    bool WaitInitialized() const
    {
        return DrainUntil(
            [state = m_state]()
            {
                return state->initializationEventSeen &&
                       state->initializationAsyncDone;
            },
            InitWaitRounds);
    }

    bool IsReady() const
    {
        return m_state &&
               m_state->initializationEventSeen &&
               m_state->initializationAsyncDone &&
               m_state->initializationStatus == WF::AsyncStatus::Completed &&
               SUCCEEDED(m_state->initializationHr) &&
               SUCCEEDED(m_state->callbackHr) &&
               m_state->core && m_state->nativeCore &&
               m_state->nativeIdentity &&
               !m_state->runtimeVersion.empty();
    }

    bool NavigateToDocument(const wchar_t *html)
    {
        if ( !IsReady() || !html )
            return false;

        const unsigned completed = m_state->navigationCompleted;
        m_peer.NavigateToString(html);
        return DrainUntil(
            [state = m_state, completed]()
            {
                return state->navigationCompleted > completed;
            },
            NavigationWaitRounds);
    }

    unsigned BeginScript(const wchar_t *script)
    {
        REQUIRE(IsReady());
        REQUIRE(script != nullptr);

        const WV2::CoreWebView2 currentCore = m_peer.CoreWebView2();
        REQUIRE(currentCore != nullptr);

        const unsigned epoch = ++m_state->scriptEpoch;
        m_state->scriptStatus = WF::AsyncStatus::Started;
        m_state->scriptHr = E_PENDING;
        m_state->scriptResult.clear();

        const std::weak_ptr<WebViewState> weakState(m_state);
        const std::uint64_t generation = m_state->generation;
        // Always execute through the Core currently published by the peer.
        // A cached pre-transfer Core would let a silent peer recreation pass
        // while probing a detached but still-live document.
        m_scriptOperation = currentCore.ExecuteScriptAsync(script);
        m_scriptOperation.Completed(
            [weakState, ledger = m_ledger, generation, epoch](
                const WF::IAsyncOperation<winrt::hstring>& operation,
                WF::AsyncStatus status)
            {
                WithCurrentState(
                    weakState, ledger, generation,
                    [&operation, status, epoch](WebViewState& current)
                    {
                        current.scriptStatus = status;
                        if ( status == WF::AsyncStatus::Completed )
                        {
                            current.scriptResult =
                                operation.GetResults().c_str();
                            current.scriptHr = S_OK;
                        }
                        else if ( status == WF::AsyncStatus::Error )
                        {
                            current.scriptHr = operation.ErrorCode().value;
                        }
                        else
                        {
                            current.scriptHr = E_ABORT;
                        }
                        current.completedScriptEpoch = epoch;
                    });
            });
        return epoch;
    }

    bool WaitScript(unsigned epoch) const
    {
        return DrainUntil(
            [state = m_state, epoch]()
            {
                return state->completedScriptEpoch == epoch;
            },
            ScriptWaitRounds);
    }

    bool RunScript(const wchar_t *script, std::wstring *result = nullptr)
    {
        const unsigned epoch = BeginScript(script);
        if ( !WaitScript(epoch) || FAILED(m_state->scriptHr) ||
             m_state->scriptStatus != WF::AsyncStatus::Completed )
        {
            return false;
        }

        if ( result )
            *result = m_state->scriptResult;
        return true;
    }

    bool IsInitializationInFlight() const
    {
        return m_initializeOperation &&
               m_initializeOperation.Status() == WF::AsyncStatus::Started &&
               !m_state->initializationAsyncDone;
    }

    bool IsScriptInFlight(unsigned epoch) const
    {
        return m_scriptOperation &&
               m_scriptOperation.Status() == WF::AsyncStatus::Started &&
               m_state->completedScriptEpoch != epoch;
    }

    void SetNavigationStartingAction(const std::function<void()>& action)
    {
        REQUIRE(m_state != nullptr);
        m_state->navigationStartingAction = action;
    }

    void RevokeHandlers() noexcept
    {
        if ( !m_state )
            return;

        if ( m_state->core )
        {
            if ( m_state->hasCoreMessageToken )
            {
                RunTeardownStep([this]()
                {
                    m_state->core.WebMessageReceived(
                        m_state->coreMessageToken);
                });
            }
            if ( m_state->hasCoreNavigationStartingToken )
            {
                RunTeardownStep([this]()
                {
                    m_state->core.NavigationStarting(
                        m_state->coreNavigationStartingToken);
                });
            }
            if ( m_state->hasCoreNavigationCompletedToken )
            {
                RunTeardownStep([this]()
                {
                    m_state->core.NavigationCompleted(
                        m_state->coreNavigationCompletedToken);
                });
            }
            if ( m_state->hasHistoryToken )
            {
                RunTeardownStep([this]()
                {
                    m_state->core.HistoryChanged(m_state->historyToken);
                });
            }
            if ( m_state->hasProcessFailedToken )
            {
                RunTeardownStep([this]()
                {
                    m_state->core.ProcessFailed(
                        m_state->processFailedToken);
                });
            }
        }
        m_state->hasCoreMessageToken = false;
        m_state->hasCoreNavigationStartingToken = false;
        m_state->hasCoreNavigationCompletedToken = false;
        m_state->hasHistoryToken = false;
        m_state->hasProcessFailedToken = false;

        if ( m_peer )
        {
            if ( m_hasInitializedToken )
            {
                RunTeardownStep([this]()
                {
                    m_peer.CoreWebView2Initialized(m_initializedToken);
                });
            }
            if ( m_hasNavigationStartingToken )
            {
                RunTeardownStep([this]()
                {
                    m_peer.NavigationStarting(m_navigationStartingToken);
                });
            }
            if ( m_hasNavigationCompletedToken )
            {
                RunTeardownStep([this]()
                {
                    m_peer.NavigationCompleted(m_navigationCompletedToken);
                });
            }
        }
        m_hasInitializedToken = false;
        m_hasNavigationStartingToken = false;
        m_hasNavigationCompletedToken = false;
    }

    void Close() noexcept
    {
        if ( m_closed )
            return;
        m_closed = true;
        ++m_ledger->closeTransactions;

        if ( m_state )
        {
            m_state->alive = false;
            if ( ++m_state->generation == 0 )
                ++m_state->generation;
            m_state->navigationStartingAction = nullptr;
        }

        RevokeHandlers();
        RunTeardownStep([this]()
        {
            if ( m_initializeOperation &&
                 m_initializeOperation.Status() == WF::AsyncStatus::Started )
                m_initializeOperation.Cancel();
        });
        RunTeardownStep([this]()
        {
            if ( m_scriptOperation &&
                 m_scriptOperation.Status() == WF::AsyncStatus::Started )
                m_scriptOperation.Cancel();
        });
        if ( m_peer )
        {
            ++m_ledger->peerCloseAttempts;
            RunTeardownStep([this]()
            {
                m_peer.Close();
            });
        }

        RunTeardownStep([this]()
        {
            if ( wxWinUIXamlHost * const owner = m_owner.get() )
            {
                if ( wxWinUITopLevelHost * const host =
                         wxWinUITopLevelHost::FindSlotOwner(owner) )
                {
                    host->UnregisterSlot(owner);
                }
            }
        });

        m_initializeOperation = nullptr;
        m_scriptOperation = nullptr;
        if ( m_state )
        {
            m_state->nativeIdentity = nullptr;
            m_state->nativeCore = nullptr;
            m_state->core = nullptr;
        }
        m_peer = nullptr;
        m_state.reset();
    }

    MUXC::WebView2 Peer() const { return m_peer; }
    std::shared_ptr<WebViewState> State() const { return m_state; }
    std::weak_ptr<WebViewState> WeakState() const { return m_state; }

private:
    void RecordTeardownFailure(HRESULT hr) noexcept
    {
        if ( m_state )
            m_state->RecordFailure(hr);
        if ( m_ledger )
            m_ledger->RecordCloseFailure(hr);
    }

    template <typename Action>
    void RunTeardownStep(const Action& action) noexcept
    {
        try
        {
            action();
        }
        catch ( const winrt::hresult_error& e )
        {
            RecordTeardownFailure(e.code().value);
        }
        catch ( ... )
        {
            RecordTeardownFailure(E_UNEXPECTED);
        }
    }

    wxWeakRef<wxWinUIXamlHost> m_owner;
    std::shared_ptr<CallbackLedger> m_ledger;
    std::shared_ptr<WebViewState> m_state;
    MUXC::WebView2 m_peer{ nullptr };
    WF::IAsyncAction m_initializeOperation{ nullptr };
    WF::IAsyncOperation<winrt::hstring> m_scriptOperation{ nullptr };

    winrt::event_token m_initializedToken{};
    winrt::event_token m_navigationStartingToken{};
    winrt::event_token m_navigationCompletedToken{};
    bool m_hasInitializedToken = false;
    bool m_hasNavigationStartingToken = false;
    bool m_hasNavigationCompletedToken = false;
    bool m_closed = false;

    wxDECLARE_NO_COPY_CLASS(WebViewSession);
};

bool QueryRuntimeVersion(std::wstring *version, HRESULT *error)
{
    REQUIRE(version != nullptr);
    REQUIRE(error != nullptr);
    version->clear();
    *error = S_OK;

    try
    {
        if ( !wxWinUI3Initialize() )
        {
            *error = E_FAIL;
            return false;
        }
        *version =
            WV2::CoreWebView2Environment::
                GetAvailableBrowserVersionString().c_str();
        return !version->empty();
    }
    catch ( const winrt::hresult_error& e )
    {
        *error = e.code().value;
        return false;
    }
}

class TwoTLWWebFixture final
{
public:
    bool Create()
    {
        frameA = new wxFrame(nullptr, wxID_ANY, "xaml-webview-a",
                             wxDefaultPosition, wxSize(480, 300),
                             wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
                                 wxFRAME_TOOL_WINDOW);
        frameB = new wxFrame(nullptr, wxID_ANY, "xaml-webview-b",
                             wxDefaultPosition, wxSize(480, 300),
                             wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
                                 wxFRAME_TOOL_WINDOW);
        panelA = new wxPanel(frameA, wxID_ANY,
                             wxPoint(0, 0), wxSize(450, 250));
        panelB = new wxPanel(frameB, wxID_ANY,
                             wxPoint(0, 0), wxSize(450, 250));
        owner = new wxWinUIXamlHost(panelA, wxID_ANY,
                                    wxPoint(20, 20), wxSize(240, 130));
        owner->SetLabel("offline web document");
        buttonA = new wxButton(panelA, wxID_ANY, "after-web-a",
                               wxPoint(285, 35), wxSize(135, 44));
        buttonB = new wxButton(panelB, wxID_ANY, "after-web-b",
                               wxPoint(285, 35), wxSize(135, 44));

        ledger = std::make_shared<CallbackLedger>();
        session.reset(new WebViewSession(owner, ledger));
        if ( !session->Attach() ||
             !ShowOffscreenWithoutActivation(frameA, 0) ||
             !ShowOffscreenWithoutActivation(frameB, 1) ||
             !DrainToQuiescence() )
        {
            Destroy();
            return false;
        }

        hostA = wxWinUITopLevelHost::FindForTLW(frameA);
        hostB = wxWinUITopLevelHost::FindForTLW(frameB);
        if ( !hostA || !hostB || hostA == hostB ||
             wxWinUITopLevelHost::FindSlotOwner(owner) != hostA ||
             wxWinUITopLevelHost::FindSlotOwner(buttonA) != hostA ||
             wxWinUITopLevelHost::FindSlotOwner(buttonB) != hostB )
        {
            Destroy();
            return false;
        }

        hostA->FlushSync();
        hostB->FlushSync();
        return true;
    }

    ~TwoTLWWebFixture()
    {
        Destroy();
    }

    void Destroy()
    {
        session.reset();
        if ( frameA )
            delete frameA;
        if ( frameB )
            delete frameB;
        frameA = nullptr;
        frameB = nullptr;
        panelA = nullptr;
        panelB = nullptr;
        owner = nullptr;
        buttonA = nullptr;
        buttonB = nullptr;
        hostA = nullptr;
        hostB = nullptr;
        DrainDispatch(5);
    }

    wxFrame *frameA = nullptr;
    wxFrame *frameB = nullptr;
    wxPanel *panelA = nullptr;
    wxPanel *panelB = nullptr;
    wxWinUIXamlHost *owner = nullptr;
    wxButton *buttonA = nullptr;
    wxButton *buttonB = nullptr;
    wxWinUITopLevelHost *hostA = nullptr;
    wxWinUITopLevelHost *hostB = nullptr;
    std::shared_ptr<CallbackLedger> ledger;
    std::unique_ptr<WebViewSession> session;
};

void FinishFixture(TwoTLWWebFixture& fixture, const HostSnapshot& before)
{
    REQUIRE(fixture.session != nullptr);
    const std::shared_ptr<WebViewState> state = fixture.session->State();
    REQUIRE(state != nullptr);
    const winrt::weak_ref<MUXC::WebView2> weakPeer = state->peer;
    const unsigned closeTransactions = fixture.ledger->closeTransactions;
    const unsigned peerCloseAttempts = fixture.ledger->peerCloseAttempts;
    fixture.Destroy();
    CHECK(fixture.ledger->closeTransactions == closeTransactions + 1);
    CHECK(fixture.ledger->peerCloseAttempts == peerCloseAttempts + 1);
    CHECK(SUCCEEDED(fixture.ledger->closeHr));
    REQUIRE(DrainUntil([&weakPeer]() { return !weakPeer.get(); }, 500));
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }, 500));
    before.CheckRestored();
}

bool InitializeFixtureWebView(TwoTLWWebFixture& fixture)
{
    fixture.session->BeginInitialize();
    if ( !fixture.session->WaitInitialized() )
        return false;
    return fixture.session->IsReady();
}

void CheckMainDocument(WebViewSession& session)
{
    REQUIRE(session.NavigateToDocument(WebDocumentA));
    const std::shared_ptr<WebViewState> state = session.State();
    REQUIRE(state != nullptr);
    REQUIRE(DrainUntil(
        [state]()
        {
            return state->lastMessage == L"dom-ready-a";
        },
        NavigationWaitRounds));
    CHECK(state->successfulNavigations >= 1);
    CHECK(state->processFailures == 0);

    std::wstring marker;
    REQUIRE(session.RunScript(
        L"document.getElementById('marker').textContent", &marker));
    CHECK(marker.find(L"ready-a") != std::wstring::npos);
}

bool CheckLogicalFocusAndTab(TwoTLWWebFixture& fixture)
{
    fixture.hostA->FocusSlot(fixture.owner);
    const bool ownerFocused = DrainUntil(
        [&fixture]()
        {
            return fixture.hostA->GetFocusOwner() == fixture.owner &&
                   wxWindow::FindFocus() == fixture.owner;
        },
        200);
    INFO("raw focus HWND " << static_cast<void *>(::GetFocus()));
    INFO("bridge HWND " << static_cast<void *>(fixture.hostA->GetBridgeHwnd()));
    INFO("focus is bridge descendant " <<
         !!::IsChild(fixture.hostA->GetBridgeHwnd(), ::GetFocus()));
    INFO("resolved focus owner " <<
         wxWinUITopLevelHost::ResolveFocusHwnd(
             reinterpret_cast<WXHWND>(::GetFocus())));
    INFO("slot owner " << fixture.owner << ", frame " << fixture.frameA <<
         ", panel " << fixture.panelA << ", next " << fixture.buttonA);
    INFO("host focus owner " << fixture.hostA->GetFocusOwner());
    INFO("wx focus owner " << wxWindow::FindFocus());
    if ( !ownerFocused )
    {
        // The XAML WebView2 currently transfers Win32 focus to an external
        // CoreWebView2 HWND which is not a descendant of our bridge. The
        // shared host consequently cannot resolve it back to this wx slot.
        // Keep the ordinary topology gate running, but prove that we neither
        // claim success nor leave a stale wx focus owner. The explicit hidden
        // focus STOP below remains red until production provides an adapter.
        CHECK(fixture.hostA->GetFocusOwner() != fixture.owner);
        CHECK(wxWindow::FindFocus() != fixture.owner);
        WARN("WebView2 external HWND focus is not mapped to its wx slot; "
             "run [.winui-webview-focus-stop] for the blocking gate");

        std::wstring focused;
        REQUIRE(fixture.session->RunScript(
            L"(()=>{document.getElementById('editor').focus();"
            L"return document.activeElement.id;})()",
            &focused));
        CHECK(focused.find(L"editor") != std::wstring::npos);
        return false;
    }

    REQUIRE(wxWindow::FindFocus() == fixture.owner);

    std::wstring focused;
    REQUIRE(fixture.session->RunScript(
        L"(()=>{document.getElementById('editor').focus();"
        L"return document.activeElement.id;})()",
        &focused));
    CHECK(focused.find(L"editor") != std::wstring::npos);
    REQUIRE(fixture.hostA->GetFocusOwner() == fixture.owner);
    REQUIRE(wxWindow::FindFocus() == fixture.owner);

    fixture.hostA->TestTakeFocusRequested(false);
    REQUIRE(DrainUntil(
        [&fixture]()
        {
            return fixture.hostA->GetFocusOwner() == fixture.buttonA &&
                   wxWindow::FindFocus() == fixture.buttonA;
        },
        200));
    fixture.hostA->TestTakeFocusRequested(true);
    REQUIRE(DrainUntil(
        [&fixture]()
        {
            return fixture.hostA->GetFocusOwner() == fixture.owner &&
                   wxWindow::FindFocus() == fixture.owner;
        },
        200));

    std::wstring refocused;
    REQUIRE(fixture.session->RunScript(
        L"document.activeElement.id", &refocused));
    CHECK(refocused.find(L"editor") != std::wstring::npos);
    return true;
}

void CheckGeometryClipAndZ(TwoTLWWebFixture& fixture)
{
    wxWinUISlot * const webSlot = fixture.hostA->FindSlot(fixture.owner);
    wxWinUISlot * const buttonSlot =
        fixture.hostA->FindSlot(fixture.buttonA);
    REQUIRE(webSlot != nullptr);
    REQUIRE(buttonSlot != nullptr);

    fixture.hostA->FlushSync();
    CHECK(webSlot->GetRectInTLW().width > 0);
    CHECK(webSlot->GetRectInTLW().height > 0);
    CHECK(std::abs(webSlot->GetContainer().Width() -
                   fixture.owner->ToDIP(
                       fixture.owner->GetClientSize().x)) < 2.0);
    CHECK(std::abs(fixture.session->Peer().ActualWidth() -
                   webSlot->GetContainer().Width()) < 2.0);
    CHECK(std::abs(fixture.session->Peer().ActualHeight() -
                   webSlot->GetContainer().Height()) < 2.0);

    fixture.owner->SetSize(wxRect(24, 24, 270, 155));
    fixture.hostA->FlushSync();
    CHECK(std::abs(webSlot->GetContainer().Width() -
                   fixture.owner->ToDIP(270)) < 3.0);
    CHECK(std::abs(webSlot->GetContainer().Height() -
                   fixture.owner->ToDIP(155)) < 3.0);
    REQUIRE(DrainUntil(
        [&fixture, webSlot]()
        {
            return std::abs(fixture.session->Peer().ActualWidth() -
                            webSlot->GetContainer().Width()) < 3.0 &&
                   std::abs(fixture.session->Peer().ActualHeight() -
                            webSlot->GetContainer().Height()) < 3.0;
        },
        250));

    fixture.hostA->SetSlotClipHeight(fixture.owner, 39);
    fixture.hostA->FlushSync();
    const auto clip =
        webSlot->GetContainer().Clip().try_as<MUXM::RectangleGeometry>();
    REQUIRE(clip != nullptr);
    CHECK(std::abs(clip.Rect().Height - 39.0f) < 1.0f);

    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
    fixture.owner->Raise();
    fixture.hostA->FlushSync();
    CHECK(MUXC::Canvas::GetZIndex(webSlot->GetContainer()) >
          MUXC::Canvas::GetZIndex(buttonSlot->GetContainer()));
    fixture.owner->Lower();
    fixture.hostA->FlushSync();
    CHECK(MUXC::Canvas::GetZIndex(webSlot->GetContainer()) <
          MUXC::Canvas::GetZIndex(buttonSlot->GetContainer()));
    CHECK(wxWinUITopLevelHost::GetImpossibleZOrderCount() == 0);

    fixture.hostA->SetSlotClipHeight(fixture.owner, -1);
    wxWinUITopLevelHost::ResetImpossibleZOrderCountForTest();
}

void CheckAccessibilityTopology(TwoTLWWebFixture& fixture,
                                wxWinUITopLevelHost *expectedHost)
{
    REQUIRE(expectedHost != nullptr);
    CHECK(wxWinUITopLevelHost::FindSlotOwner(fixture.owner) == expectedHost);
    wxWinUISlot * const slot = expectedHost->FindSlot(fixture.owner);
    REQUIRE(slot != nullptr);
    CHECK(slot->GetContent() == fixture.session->Peer());
    CHECK(slot->GetSemanticTarget() == fixture.session->Peer());
    CHECK(wxWinUITLWHostIsInvisibleAccessibilityShell(fixture.owner));

    const MUXAP::AutomationPeer peer =
        MUXAP::FrameworkElementAutomationPeer::CreatePeerForElement(
            fixture.session->Peer());
    REQUIRE(peer != nullptr);

    const HWND bridgeA = fixture.hostA->GetBridgeHwnd();
    const HWND bridgeB = fixture.hostB->GetBridgeHwnd();
    REQUIRE(bridgeA != nullptr);
    REQUIRE(bridgeB != nullptr);
    CHECK(bridgeA != bridgeB);
    CHECK(wxWinUITopLevelHost::FindForBridge(bridgeA) == fixture.hostA);
    CHECK(wxWinUITopLevelHost::FindForBridge(bridgeB) == fixture.hostB);

    // The shell remains a real wx HWND, but its accessibility authority is
    // the single XAML peer above. A full Chromium document subtree and its
    // Narrator order remain a physical Accessibility Insights gate.
    CHECK(wxFindWinFromHandle(GetHwndOf(fixture.owner)) == fixture.owner);
}

void DestroyAtAsyncPhase(const char *phase)
{
    const HostSnapshot before = HostSnapshot::Capture();
    const auto ledger = std::make_shared<CallbackLedger>();
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "webview-async-retire",
                    wxDefaultPosition, wxSize(360, 220),
                    wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
                        wxFRAME_TOOL_WINDOW);
    wxPanel * const panel = new wxPanel(frame);
    wxWinUIXamlHost *owner =
        new wxWinUIXamlHost(panel, wxID_ANY,
                            wxPoint(10, 10), wxSize(300, 160));
    std::unique_ptr<WebViewSession> session(
        new WebViewSession(owner, ledger));
    REQUIRE(session->Attach());
    REQUIRE(ShowOffscreenWithoutActivation(frame, 2));
    REQUIRE(DrainToQuiescence());

    session->BeginInitialize();
    std::shared_ptr<WebViewState> phaseState = session->State();
    REQUIRE(phaseState != nullptr);
    const std::uint64_t phaseGeneration = phaseState->generation;
    const winrt::weak_ref<MUXC::WebView2> weakPeer = phaseState->peer;
    bool closedInCallback = false;
    const std::string phaseName(phase);
    if ( phaseName == "initialization" )
    {
        REQUIRE(session->IsInitializationInFlight());
    }
    else
    {
        REQUIRE(session->WaitInitialized());
        REQUIRE(session->IsReady());
        if ( phaseName == "navigation" )
        {
            // Close from the real NavigationStarting stack so this case
            // cannot accidentally observe an already-completed navigation.
            session->SetNavigationStartingAction([&session,
                                                   &closedInCallback]()
            {
                closedInCallback = true;
                session->Close();
            });
            session->Peer().NavigateToString(WebDocumentA);
            REQUIRE(DrainUntil(
                [&closedInCallback]() { return closedInCallback; },
                NavigationWaitRounds));
        }
        else
        {
            REQUIRE(session->NavigateToDocument(WebDocumentA));
            const unsigned scriptEpoch = session->BeginScript(
                L"(()=>{const until=performance.now()+250;"
                L"while(performance.now()<until){};"
                L"document.getElementById('marker').textContent='retiring';"
                L"return document.body.innerHTML.length;})()");
            REQUIRE(session->IsScriptInFlight(scriptEpoch));
        }
    }

    const std::weak_ptr<WebViewState> state(phaseState);
    const unsigned acceptedAtClose = ledger->accepted;
    if ( !closedInCallback )
        session->Close();

    // Exercise both stale-generation paths deterministically instead of
    // assuming a cancelled runtime operation must still invoke Completion.
    bool staleCallbackRan = false;
    const unsigned rejectedBefore = ledger->rejectedGeneration;
    WithCurrentState(
        state, ledger, phaseGeneration,
        [&staleCallbackRan](WebViewState&) { staleCallbackRan = true; });
    CHECK(!staleCallbackRan);
    CHECK(ledger->rejectedGeneration == rejectedBefore + 1);

    phaseState.reset();
    session.reset();
    const unsigned expiredBefore = ledger->expiredState;
    WithCurrentState(
        state, ledger, phaseGeneration,
        [&staleCallbackRan](WebViewState&) { staleCallbackRan = true; });
    CHECK(!staleCallbackRan);
    CHECK(ledger->expiredState == expiredBefore + 1);
    delete owner;
    owner = nullptr;
    delete frame;
    REQUIRE(DrainUntil([&state]() { return state.expired(); }, 250));
    REQUIRE(DrainUntil([&weakPeer]() { return !weakPeer.get(); }, 500));
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }, 500));
    CHECK(ledger->accepted == acceptedAtClose);
    CHECK(ledger->closeTransactions == 1);
    CHECK(ledger->peerCloseAttempts == 1);
    CHECK(SUCCEEDED(ledger->closeHr));
    before.CheckRestored();
}

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP
POINTL WindowCentreInScreen(HWND hwnd)
{
    RECT rect = {};
    REQUIRE(::GetWindowRect(hwnd, &rect));
    return {
        rect.left + (rect.right - rect.left) / 2,
        rect.top + (rect.bottom - rect.top) / 2
    };
}
#endif

} // namespace

TEST_CASE("WinUIWebView::RuntimeNavigationScriptVetoAndNativeBackend",
          "[winui-webview][winui-015][webview2][navigation][lifetime]")
{
    std::wstring availableVersion;
    HRESULT runtimeHr = S_OK;
    const bool runtimeAvailable =
        QueryRuntimeVersion(&availableVersion, &runtimeHr);
    INFO("WebView2 Evergreen runtime probe HRESULT 0x" <<
         std::hex << static_cast<unsigned long>(runtimeHr));
    REQUIRE(runtimeAvailable);

    const HostSnapshot before = HostSnapshot::Capture();
    TwoTLWWebFixture fixture;
    REQUIRE(fixture.Create());
    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == before.hosts + 2);
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == before.slots + 3);
    REQUIRE(InitializeFixtureWebView(fixture));
    const std::shared_ptr<WebViewState> state = fixture.session->State();
    REQUIRE(state != nullptr);
    INFO("available WebView2 runtime " <<
         wxString(availableVersion).ToStdString());
    INFO("initialized WebView2 runtime " <<
         wxString(state->runtimeVersion).ToStdString());
    // Evergreen can update between the availability probe and environment
    // creation. Both values must be real, but equality is not a stable SDK or
    // runtime-provenance oracle.
    CHECK(!availableVersion.empty());
    CHECK(!state->runtimeVersion.empty());
    CHECK(state->initializationEventSequence > 0);
    CHECK(state->initializationAsyncSequence >
          state->initializationEventSequence);
    CHECK(SUCCEEDED(state->callbackHr));
    CHECK(state->nativeCore != nullptr);
    CHECK(state->nativeIdentity != nullptr);

    CheckMainDocument(*fixture.session);

    std::wstring historyResult;
    const unsigned historyBefore = state->historyChanges;
    REQUIRE(fixture.session->RunScript(
        L"(()=>{history.replaceState({},'', '#history-a');"
        L"history.pushState({},'', '#history-b');"
        L"document.getElementById('marker').textContent='history-live';"
        L"return location.hash + ':' + document.getElementById('marker')."
        L"textContent;})()",
        &historyResult));
    CHECK(historyResult.find(L"#history-b:history-live") !=
          std::wstring::npos);
    REQUIRE(DrainUntil(
        [state, historyBefore]()
        {
            return state->historyChanges > historyBefore;
        },
        250));
    CHECK(state->core.CanGoBack());

    state->vetoNeedle = L"wx-veto";
    const unsigned vetoBefore = state->vetoedNavigations;
    // A fragment-only change is a same-document navigation and doesn't fire
    // NavigationStarting. Changing the query forces the vetoable navigation
    // path while remaining a local about:blank URL.
    state->core.Navigate(L"about:blank?wx-veto");
    REQUIRE(DrainUntil(
        [state, vetoBefore]()
        {
            return state->vetoedNavigations > vetoBefore;
        },
        250));
    std::wstring afterVeto;
    REQUIRE(fixture.session->RunScript(
        L"location.href + ':' + document.getElementById('marker').textContent",
        &afterVeto));
    CHECK(afterVeto.find(L"wx-veto") == std::wstring::npos);
    CHECK(afterVeto.find(L"history-live") != std::wstring::npos);
    CHECK(state->processFailures == 0);

    CheckAccessibilityTopology(fixture, fixture.hostA);
    FinishFixture(fixture, before);
}

TEST_CASE("WinUIWebView::SharedSlotGeometryFocusAndCrossTLWTransfer",
          "[winui-webview][winui-015][HostLifecycle][HostState]"
          "[reparent][focus][z-order][uia]")
{
    std::wstring runtimeVersion;
    HRESULT runtimeHr = S_OK;
    const bool runtimeAvailable =
        QueryRuntimeVersion(&runtimeVersion, &runtimeHr);
    INFO("WebView2 Evergreen runtime probe HRESULT 0x" <<
         std::hex << static_cast<unsigned long>(runtimeHr));
    REQUIRE(runtimeAvailable);

    const HostSnapshot before = HostSnapshot::Capture();
    TwoTLWWebFixture fixture;
    REQUIRE(fixture.Create());
    CHECK(wxWinUITopLevelHost::GetLiveHostCount() == before.hosts + 2);
    CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == before.slots + 3);
    REQUIRE(InitializeFixtureWebView(fixture));
    CheckMainDocument(*fixture.session);
    CheckGeometryClipAndZ(fixture);
    const bool logicalFocusSupported = CheckLogicalFocusAndTab(fixture);

    MUXC::WebView2 peer = fixture.session->Peer();
    const IUnknown * const peerIdentity = winrt::get_unknown(peer);
    const auto sourceXamlRoot = peer.XamlRoot();
    REQUIRE(sourceXamlRoot != nullptr);
    const std::shared_ptr<WebViewState> state = fixture.session->State();
    REQUIRE(state != nullptr);
    REQUIRE(state->nativeIdentity != nullptr);
    CoreIdentitySnapshot initialIdentity;
    REQUIRE(SUCCEEDED(CapturePeerCoreIdentity(peer, &initialIdentity)));
    CHECK(winrt::get_unknown(initialIdentity.projected) ==
          winrt::get_unknown(state->core));
    CHECK(initialIdentity.identity.get() == state->nativeIdentity.get());
    // Build a real Core navigation stack and prove it before changing
    // XamlRoot. Same-document pushState entries on NavigateToString's
    // about:blank origin aren't a stable Core GoBack oracle.
    REQUIRE(fixture.session->NavigateToDocument(WebDocumentB));
    REQUIRE(DrainUntil(
        [state]() { return state->lastMessage == L"dom-ready-b"; },
        NavigationWaitRounds));
    REQUIRE(initialIdentity.projected.CanGoBack());

    unsigned messageBefore = state->webMessages;
    initialIdentity.projected.GoBack();
    REQUIRE(DrainUntil(
        [state, messageBefore]()
        {
            return state->webMessages > messageBefore &&
                   state->lastMessage == L"dom-ready-a";
        },
        NavigationWaitRounds));
    REQUIRE(initialIdentity.projected.CanGoForward());

    messageBefore = state->webMessages;
    initialIdentity.projected.GoForward();
    REQUIRE(DrainUntil(
        [state, messageBefore]()
        {
            return state->webMessages > messageBefore &&
                   state->lastMessage == L"dom-ready-b";
        },
        NavigationWaitRounds));
    REQUIRE(initialIdentity.projected.CanGoBack());

    std::wstring prepared;
    REQUIRE(fixture.session->RunScript(
        L"(()=>{document.getElementById('marker').textContent='transfer-live';"
        L"return document.title + ':' + "
        L"document.getElementById('marker').textContent;})()",
        &prepared));
    CHECK(prepared.find(L"wxWinUI WebView2 B:transfer-live") !=
          std::wstring::npos);

    // This is the decisive topology experiment. The shared host transfers the
    // same UIElement between roots transactionally. Recreating a WebView2 or
    // its Core here would hide an API-breaking loss of DOM/history state.
    REQUIRE(fixture.owner->Reparent(fixture.panelB));
    REQUIRE(DrainUntil(
        [&fixture]()
        {
            return wxWinUITopLevelHost::FindSlotOwner(fixture.owner) ==
                       fixture.hostB &&
                   fixture.hostB->FindSlot(fixture.owner) != nullptr;
        },
        500));
    CHECK(fixture.hostA->FindSlot(fixture.owner) == nullptr);
    wxWinUISlot * const migrated =
        fixture.hostB->FindSlot(fixture.owner);
    REQUIRE(migrated != nullptr);
    CHECK(migrated->GetContent() == peer);
    CHECK(migrated->GetSemanticTarget() == peer);
    fixture.hostB->FlushSync();
    REQUIRE(DrainUntil(
        [&peer, migrated]()
        {
            return std::abs(peer.ActualWidth() -
                            migrated->GetContainer().Width()) < 3.0 &&
                   std::abs(peer.ActualHeight() -
                            migrated->GetContainer().Height()) < 3.0;
        },
        500));
    CHECK(winrt::get_unknown(fixture.session->Peer()) == peerIdentity);
    MUX::XamlRoot destinationXamlRoot{ nullptr };
    REQUIRE(DrainUntil(
        [&peer, &sourceXamlRoot, &destinationXamlRoot]()
        {
            const auto current = peer.XamlRoot();
            if ( !current || current == sourceXamlRoot )
                return false;
            destinationXamlRoot = current;
            return true;
        },
        500));

    CoreIdentitySnapshot destinationIdentity;
    REQUIRE(SUCCEEDED(CapturePeerCoreIdentity(peer, &destinationIdentity)));
    CHECK(winrt::get_unknown(destinationIdentity.projected) ==
          winrt::get_unknown(initialIdentity.projected));
    CHECK(destinationIdentity.identity.get() == initialIdentity.identity.get());
    if ( logicalFocusSupported )
    {
        REQUIRE(DrainUntil(
            [&fixture]()
            {
                return fixture.hostB->GetFocusOwner() == fixture.owner &&
                       wxWindow::FindFocus() == fixture.owner;
            },
            500));
    }
    else
    {
        DrainDispatch(10);
        CHECK(fixture.hostA->GetFocusOwner() != fixture.owner);
        CHECK(fixture.hostB->GetFocusOwner() != fixture.owner);
        CHECK(wxWindow::FindFocus() != fixture.owner);
    }
    CheckAccessibilityTopology(fixture, fixture.hostB);

    std::wstring preserved;
    REQUIRE(fixture.session->RunScript(
        L"document.title + ':' + document.getElementById('marker').textContent",
        &preserved));
    CHECK(preserved.find(L"wxWinUI WebView2 B:transfer-live") !=
          std::wstring::npos);
    REQUIRE(destinationIdentity.projected.CanGoBack());

    // The exact Core navigation stack established before migration must still
    // work from the destination root.
    messageBefore = state->webMessages;
    destinationIdentity.projected.GoBack();
    REQUIRE(DrainUntil(
        [state, messageBefore]()
        {
            return state->webMessages > messageBefore &&
                   state->lastMessage == L"dom-ready-a";
        },
        NavigationWaitRounds));
    std::wstring afterBack;
    REQUIRE(fixture.session->RunScript(
        L"document.title + ':' + document.getElementById('marker').textContent",
        &afterBack));
    CHECK(afterBack.find(L"wxWinUI WebView2 A:ready-a") !=
          std::wstring::npos);
    REQUIRE(destinationIdentity.projected.CanGoForward());

    // The reverse transfer must preserve the exact same identities too.
    REQUIRE(fixture.owner->Reparent(fixture.panelA));
    REQUIRE(DrainUntil(
        [&fixture, &peer, &sourceXamlRoot]()
        {
            return wxWinUITopLevelHost::FindSlotOwner(fixture.owner) ==
                       fixture.hostA &&
                   fixture.hostA->FindSlot(fixture.owner) != nullptr &&
                   peer.XamlRoot() == sourceXamlRoot;
        },
        500));
    CHECK(fixture.hostB->FindSlot(fixture.owner) == nullptr);
    wxWinUISlot * const restoredSlot =
        fixture.hostA->FindSlot(fixture.owner);
    REQUIRE(restoredSlot != nullptr);
    CHECK(restoredSlot->GetContent() == peer);
    CHECK(restoredSlot->GetSemanticTarget() == peer);
    fixture.hostA->FlushSync();
    REQUIRE(DrainUntil(
        [&peer, restoredSlot]()
        {
            return std::abs(peer.ActualWidth() -
                            restoredSlot->GetContainer().Width()) < 3.0 &&
                   std::abs(peer.ActualHeight() -
                            restoredSlot->GetContainer().Height()) < 3.0;
        },
        500));
    CHECK(winrt::get_unknown(fixture.session->Peer()) == peerIdentity);
    CoreIdentitySnapshot restoredIdentity;
    REQUIRE(SUCCEEDED(CapturePeerCoreIdentity(peer, &restoredIdentity)));
    CHECK(winrt::get_unknown(restoredIdentity.projected) ==
          winrt::get_unknown(initialIdentity.projected));
    CHECK(restoredIdentity.identity.get() == initialIdentity.identity.get());
    if ( logicalFocusSupported )
    {
        REQUIRE(DrainUntil(
            [&fixture]()
            {
                return fixture.hostA->GetFocusOwner() == fixture.owner &&
                       wxWindow::FindFocus() == fixture.owner;
            },
            500));
    }
    else
    {
        DrainDispatch(10);
        CHECK(fixture.hostA->GetFocusOwner() != fixture.owner);
        CHECK(fixture.hostB->GetFocusOwner() != fixture.owner);
        CHECK(wxWindow::FindFocus() != fixture.owner);
    }
    CheckAccessibilityTopology(fixture, fixture.hostA);

    std::wstring restoredDocument;
    REQUIRE(fixture.session->RunScript(
        L"document.title + ':' + document.getElementById('marker').textContent",
        &restoredDocument));
    CHECK(restoredDocument.find(L"wxWinUI WebView2 A:ready-a") !=
          std::wstring::npos);
    CHECK(restoredIdentity.projected.CanGoForward());
    CHECK(state->processFailures == 0);
    CHECK(wxWinUITopLevelHost::GetPendingFocusMigrationCountForTest() ==
          before.pendingFocusMigrations);

    peer = nullptr;
    initialIdentity = CoreIdentitySnapshot{};
    destinationIdentity = CoreIdentitySnapshot{};
    restoredIdentity = CoreIdentitySnapshot{};
    FinishFixture(fixture, before);
}

TEST_CASE("WinUIWebView::AsyncDestructionPhases",
          "[winui-webview][winui-015][HostLifecycle][async][destroy]")
{
    std::wstring runtimeVersion;
    HRESULT runtimeHr = S_OK;
    const bool runtimeAvailable =
        QueryRuntimeVersion(&runtimeVersion, &runtimeHr);
    INFO("WebView2 Evergreen runtime probe HRESULT 0x" <<
         std::hex << static_cast<unsigned long>(runtimeHr));
    REQUIRE(runtimeAvailable);

    SECTION("destroy during initialization")
    {
        DestroyAtAsyncPhase("initialization");
    }
    SECTION("destroy during navigation")
    {
        DestroyAtAsyncPhase("navigation");
    }
    SECTION("destroy during script")
    {
        DestroyAtAsyncPhase("script");
    }
}

TEST_CASE("WinUIWebView::TwentyRuntimeAndHundredRetirementCycles",
          "[winui-webview][winui-015][HostLifecycle][stress][async]")
{
    std::wstring runtimeVersion;
    HRESULT runtimeHr = S_OK;
    const bool runtimeAvailable =
        QueryRuntimeVersion(&runtimeVersion, &runtimeHr);
    INFO("WebView2 Evergreen runtime probe HRESULT 0x" <<
         std::hex << static_cast<unsigned long>(runtimeHr));
    REQUIRE(runtimeAvailable);

    const HostSnapshot before = HostSnapshot::Capture();
    const auto ledger = std::make_shared<CallbackLedger>();
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "webview-soak",
                    wxDefaultPosition, wxSize(380, 230),
                    wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR |
                        wxFRAME_TOOL_WINDOW);
    wxPanel * const panel = new wxPanel(frame);
    REQUIRE(ShowOffscreenWithoutActivation(frame, 3));
    const wxStopWatch fullCycles;

    // Twenty complete runtime cycles: initialize, navigate in-memory, execute
    // a DOM probe, revoke and Close. No profile/network state is injected.
    for ( unsigned i = 0; i < 20; ++i )
    {
        wxWinUIXamlHost * const owner =
            new wxWinUIXamlHost(panel, wxID_ANY,
                                wxPoint(10, 10), wxSize(300, 160));
        {
            WebViewSession session(owner, ledger);
            REQUIRE(session.Attach());
            session.BeginInitialize();
            REQUIRE(session.WaitInitialized());
            REQUIRE(session.IsReady());
            REQUIRE(session.NavigateToDocument(WebDocumentA));
            std::wstring marker;
            REQUIRE(session.RunScript(
                L"document.getElementById('marker').textContent", &marker));
            CHECK(marker.find(L"ready-a") != std::wstring::npos);
        }
        delete owner;
        REQUIRE(DrainToQuiescence());
        CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == before.slots);
    }
    INFO("20 complete WebView2 cycles took " << fullCycles.Time() << " ms");
    CHECK(fullCycles.Time() < 300000);

    // Then one hundred bounded cancellation cycles. Every cycle starts the
    // real Core initialization but retires the generation before pumping it;
    // ten-cycle drains prove no callback can rebuild a removed slot or retain
    // its wx owner. This is intentionally distinct from the 20 full cycles.
    const wxStopWatch retireCycles;
    for ( unsigned i = 0; i < 100; ++i )
    {
        wxWinUIXamlHost * const owner =
            new wxWinUIXamlHost(panel, wxID_ANY,
                                wxPoint(10, 10), wxSize(300, 160));
        std::weak_ptr<WebViewState> weak;
        {
            WebViewSession session(owner, ledger);
            REQUIRE(session.Attach());
            session.BeginInitialize();
            weak = session.WeakState();
        }
        delete owner;
        REQUIRE(weak.expired());
        if ( i % 10 == 9 )
        {
            REQUIRE(DrainToQuiescence());
            CHECK(wxWinUITopLevelHost::GetLiveSlotCount() == before.slots);
        }
    }
    INFO("100 immediate retirement cycles took " <<
         retireCycles.Time() << " ms");
    CHECK(retireCycles.Time() < 180000);
    CHECK(ledger->closeTransactions == 120);
    CHECK(ledger->peerCloseAttempts == 120);
    CHECK(SUCCEEDED(ledger->closeHr));

    delete frame;
    REQUIRE(DrainUntil([&before]() { return before.IsRestored(); }, 750));
    CHECK(ledger->accepted > 0);
    before.CheckRestored();
}

// Mandatory negative gate, hidden from the ordinary green suite. XAML
// WebView2 transfers focus to an external browser HWND. Until the common host
// has a generation-safe mapping from that HWND back to this wx slot, Tab and
// wxWindow::FindFocus() cannot satisfy the wx control contract. Keeping this
// as a real red test prevents the Experimental spike from being mistaken for
// a Supported backend.
TEST_CASE("WinUIWebView::ExternalHwndFocusMustMapToWxSlot",
          "[.winui-webview-focus-stop]")
{
    std::wstring runtimeVersion;
    HRESULT runtimeHr = S_OK;
    const bool runtimeAvailable =
        QueryRuntimeVersion(&runtimeVersion, &runtimeHr);
    INFO("WebView2 Evergreen runtime probe HRESULT 0x" <<
         std::hex << static_cast<unsigned long>(runtimeHr));
    REQUIRE(runtimeAvailable);

    const HostSnapshot before = HostSnapshot::Capture();
    TwoTLWWebFixture fixture;
    REQUIRE(fixture.Create());
    REQUIRE(InitializeFixtureWebView(fixture));
    CheckMainDocument(*fixture.session);

    const bool focusContractSatisfied = CheckLogicalFocusAndTab(fixture);
    FinishFixture(fixture, before);
    REQUIRE(focusContractSatisfied);
}

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP

// Mandatory negative gate, deliberately hidden from the ordinary green
// suite. Run it explicitly with `[.winui-webview-drop-stop]`. It drives the
// real host-owned IDropTarget with a real IDataObject and requires the DOM
// `drop` event. The current broker only delegates to wxDropTarget and has no
// WebView2 composition-controller adapter, so this test must remain red until
// production routing exists; adding a wxTextDropTarget here would test the
// wrong contract and conceal the STOP.
TEST_CASE("WinUIWebView::OleBrokerMustReachDomDrop",
          "[.winui-webview-drop-stop]")
{
    std::wstring runtimeVersion;
    HRESULT runtimeHr = S_OK;
    const bool runtimeAvailable =
        QueryRuntimeVersion(&runtimeVersion, &runtimeHr);
    INFO("WebView2 Evergreen runtime probe HRESULT 0x" <<
         std::hex << static_cast<unsigned long>(runtimeHr));
    REQUIRE(runtimeAvailable);

    const HostSnapshot before = HostSnapshot::Capture();
    TwoTLWWebFixture fixture;
    REQUIRE(fixture.Create());
    REQUIRE(InitializeFixtureWebView(fixture));
    CheckMainDocument(*fixture.session);

    wxWinUIDropBroker * const broker =
        fixture.hostA->GetDropBrokerForTest();
    REQUIRE(broker != nullptr);
    winrt::com_ptr<IDropTarget> target;
    target.copy_from(broker->GetCOMTargetForTest());
    REQUIRE(target != nullptr);

    const POINTL point =
        WindowCentreInScreen(GetHwndOf(fixture.owner));
    wxTextDataObject source("webview-dom-drop");
    DWORD effect = DROPEFFECT_COPY;
    REQUIRE(target->DragEnter(
                source.GetInterface(), 0, point, &effect) == S_OK);
    const DWORD enterEffect = effect;
    effect = DROPEFFECT_COPY;
    REQUIRE(target->DragOver(0, point, &effect) == S_OK);
    const DWORD overEffect = effect;
    effect = DROPEFFECT_COPY;
    REQUIRE(target->Drop(
                source.GetInterface(), 0, point, &effect) == S_OK);
    const DWORD dropEffect = effect;
    DrainDispatch(20);

    std::wstring dropCount;
    REQUIRE(fixture.session->RunScript(L"window.wxDropCount",
                                       &dropCount));
    INFO("broker effects enter=" << enterEffect << " over=" << overEffect <<
         " drop=" << dropEffect << " DOM count=" <<
         wxString(dropCount).ToStdString());
    CHECK((enterEffect & DROPEFFECT_COPY) != 0);
    CHECK((overEffect & DROPEFFECT_COPY) != 0);
    CHECK((dropEffect & DROPEFFECT_COPY) != 0);
    CHECK(dropCount == L"1");

    FinishFixture(fixture, before);
}

#endif // wxUSE_OLE && wxUSE_DRAG_AND_DROP

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_WEBVIEW && wxUSE_WEBVIEW_EDGE

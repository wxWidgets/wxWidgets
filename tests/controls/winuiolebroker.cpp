///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiolebroker.cpp
// Purpose:     ownership/lifetime tests for the WinUI OLE drop broker
// Author:      wxWidgets development team
// Created:     2026-07-24
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && \
        wxUSE_OLE && wxUSE_DRAG_AND_DROP

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/button.h"
    #include "wx/frame.h"
    #include "wx/panel.h"
    #include "wx/utils.h"
    #include "wx/window.h"
#endif

#include "wx/dataobj.h"
#include "wx/dnd.h"
#include "wx/evtloop.h"
#include "wx/nativewin.h"
#include "wx/msw/private.h"
#include "wx/msw/private/dropsession.h"
#include "wx/msw/wrapshl.h"
#include "wx/winui/private/dropbroker.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"

#include <shellapi.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>

#include <algorithm>
#include <array>
#include <functional>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace
{

struct NativeLog
{
    ~NativeLog()
    {
        delete testBridge;
        delete testFrame;
    }

    wxWindow* EnsureTestWindow()
    {
        if ( !testFrame )
        {
            testFrame = new wxFrame(nullptr, wxID_ANY,
                                    "ole-broker-native-ops-seam",
                                    wxPoint(-24000, -24000),
                                    wxSize(320, 180));
        }

        return testFrame;
    }

    HWND GetTestHwnd() const
    {
        return testFrame ? GetHwndOf(testFrame) : nullptr;
    }

    wxWindow* EnsureTestBridge()
    {
        if ( !testBridge )
        {
            testBridge = new wxFrame(
                nullptr, wxID_ANY, "ole-broker-native-ops-bridge-seam",
                wxPoint(-24500, -24500), wxSize(32, 32));
        }
        return testBridge;
    }

    HWND GetBridgeHwnd() const
    {
        return testBridge ? GetHwndOf(testBridge) : nullptr;
    }

    void DestroyTestBridge()
    {
        delete testBridge;
        testBridge = nullptr;
    }

    HRESULT lockResult = S_OK;
    HRESULT registerResult = S_OK;
    HRESULT revokeResult = S_OK;
    HRESULT unlockResult = S_OK;
    std::vector<HRESULT> lockResults;
    std::vector<HRESULT> registerResults;
    std::vector<HRESULT> revokeResults;
    std::vector<HRESULT> unlockResults;
    HWND registeredHwnd = nullptr;
    HWND revokedHwnd = nullptr;
    std::vector<HWND> registeredHwnds;
    std::vector<HWND> revokedHwnds;
    std::vector<IUnknown*> lockedTargets;
    std::vector<IUnknown*> unlockedTargets;
    std::vector<IDropTarget*> registeredTargets;
    std::vector<std::string> calls;
    wxWinUIDropBroker* reentrantBroker = nullptr;
    bool shutdownOnLock = false;
    bool shutdownOnRegister = false;
    bool shutdownOnRevoke = false;
    bool shutdownOnUnlock = false;
    bool destroyBridgeOnRevoke = false;
    bool retireBridgeOnRevoke = false;
    bool registerShellOnRegister = false;
    bool shellRegisterSucceeded = false;
    wxDropTarget* shellTargetToRegister = nullptr;
    HWND shellHwndToRegister = nullptr;

private:
    // MakeBroker() uses a distinct real HWND for each native-ops seam. The
    // log is declared before the broker in every test, so this frame is
    // destroyed after the broker and retires that exact HWND generation.
    // This prevents a deliberately retained tombstone in one SECTION from
    // contaminating another independent transaction.
    wxFrame* testFrame = nullptr;
    wxFrame* testBridge = nullptr;
    std::size_t lockResultIndex = 0;
    std::size_t registerResultIndex = 0;
    std::size_t revokeResultIndex = 0;
    std::size_t unlockResultIndex = 0;

public:

    static HRESULT Lock(void* context,
                        IUnknown* target,
                        BOOL lock,
                        BOOL lastUnlockReleases) noexcept
    {
        NativeLog& self = *static_cast<NativeLog*>(context);
        if ( lock )
        {
            self.lockedTargets.push_back(target);
            self.calls.push_back("lock(T,F)");
            if ( self.shutdownOnLock && self.reentrantBroker )
            {
                self.shutdownOnLock = false;
                self.reentrantBroker->Shutdown();
            }
            const std::size_t index = self.lockResultIndex++;
            return index < self.lockResults.size()
                       ? self.lockResults[index]
                       : self.lockResult;
        }

        self.unlockedTargets.push_back(target);
        self.calls.push_back(
            lastUnlockReleases ? "unlock(F,T)" : "unlock(F,F)");
        if ( self.shutdownOnUnlock && self.reentrantBroker )
        {
            self.shutdownOnUnlock = false;
            self.reentrantBroker->Shutdown();
        }
        const std::size_t index = self.unlockResultIndex++;
        return index < self.unlockResults.size()
                   ? self.unlockResults[index]
                   : self.unlockResult;
    }

    static HRESULT Register(void* context,
                            HWND hwnd,
                            IDropTarget* target) noexcept
    {
        NativeLog& self = *static_cast<NativeLog*>(context);
        self.registeredHwnd = hwnd;
        self.registeredHwnds.push_back(hwnd);
        self.registeredTargets.push_back(target);
        self.calls.push_back("register");
        if ( self.shutdownOnRegister && self.reentrantBroker )
        {
            self.shutdownOnRegister = false;
            self.reentrantBroker->Shutdown();
        }
        if ( self.registerShellOnRegister &&
             self.shellTargetToRegister && self.shellHwndToRegister )
        {
            self.registerShellOnRegister = false;
            self.shellRegisterSucceeded =
                self.shellTargetToRegister->Register(
                    reinterpret_cast<WXHWND>(self.shellHwndToRegister));
        }
        const std::size_t index = self.registerResultIndex++;
        return index < self.registerResults.size()
                   ? self.registerResults[index]
                   : self.registerResult;
    }

    static HRESULT Revoke(void* context, HWND hwnd) noexcept
    {
        NativeLog& self = *static_cast<NativeLog*>(context);
        self.revokedHwnd = hwnd;
        self.revokedHwnds.push_back(hwnd);
        self.calls.push_back("revoke");
        if ( self.shutdownOnRevoke && self.reentrantBroker )
        {
            self.shutdownOnRevoke = false;
            self.reentrantBroker->Shutdown();
        }
        if ( hwnd == self.GetBridgeHwnd() )
        {
            if ( self.retireBridgeOnRevoke )
            {
                self.retireBridgeOnRevoke = false;
                wxMSWOleRetireShellHwnd(
                    wxMSWOleCaptureShellHwndIdentity(
                        reinterpret_cast<WXHWND>(hwnd)));
            }
            if ( self.destroyBridgeOnRevoke )
            {
                self.destroyBridgeOnRevoke = false;
                self.DestroyTestBridge();
            }
        }
        const std::size_t index = self.revokeResultIndex++;
        return index < self.revokeResults.size()
                   ? self.revokeResults[index]
                   : self.revokeResult;
    }

    wxWinUIDropBrokerNativeOps Ops()
    {
        wxWinUIDropBrokerNativeOps ops;
        ops.context = this;
        ops.lock = Lock;
        ops.registerTarget = Register;
        ops.revokeTarget = Revoke;
        return ops;
    }
};

std::unique_ptr<wxWinUIDropBroker> MakeBroker(NativeLog& log)
{
    wxWindow* const tlw = log.EnsureTestWindow();
    REQUIRE(tlw);
    const HWND hwnd = GetHwndOf(tlw);
    REQUIRE(hwnd);
    wxWindow* const bridge = log.EnsureTestBridge();
    REQUIRE(bridge);
    const HWND bridgeHwnd = GetHwndOf(bridge);
    REQUIRE(bridgeHwnd);
    REQUIRE(bridgeHwnd != hwnd);

    // The native-ops seam exercises ownership only, so the invalidatable
    // host state intentionally contains no host. Active COM callbacks still
    // fail closed through the exact production State/Session path.
    const std::weak_ptr<wxWinUIHostLifetime> lifetime;
    const wxWinUIDropBrokerNativeOps ops = log.Ops();
    return wxWinUIDropBroker::Create(tlw, bridgeHwnd, lifetime, &ops);
}

HWND RequireDistinctBridgeHwnd(NativeLog& log, HWND tlwHwnd)
{
    wxWindow* const bridge = log.EnsureTestBridge();
    REQUIRE(bridge);
    const HWND bridgeHwnd = GetHwndOf(bridge);
    REQUIRE(bridgeHwnd);
    REQUIRE(bridgeHwnd != tlwHwnd);
    return bridgeHwnd;
}

void DrainDispatch(int rounds = 12)
{
    for ( int i = 0; i < rounds; ++i )
    {
        wxYield();
        wxMilliSleep(3);
    }
}

class HostInitializationFaultGuard final
{
public:
    explicit HostInitializationFaultGuard(unsigned fault)
    {
        wxWinUITopLevelHost::TestFailInitialization(fault);
    }

    ~HostInitializationFaultGuard()
    {
        wxWinUITopLevelHost::TestFailInitialization(
            wxWinUITopLevelHost::TestInitializationFault_None);
    }
};

wxWindow* gs_deferredInitializationTLW = nullptr;
unsigned gs_deferredInitializationTLWBindings = 0;

void CountDeferredInitializationTLWBindings(
    wxWindow* tlw,
    wxWinUITopLevelHost::InitializationHookPointForTest point)
{
    if ( tlw == gs_deferredInitializationTLW &&
         point == wxWinUITopLevelHost::
                      InitializationHookPointForTest::AfterTLWBindings )
    {
        ++gs_deferredInitializationTLWBindings;
    }
}

class DeferredInitializationHookGuard final
{
public:
    explicit DeferredInitializationHookGuard(wxWindow* tlw)
    {
        gs_deferredInitializationTLW = tlw;
        gs_deferredInitializationTLWBindings = 0;
        wxWinUITopLevelHost::TestSetInitializationHook(
            CountDeferredInitializationTLWBindings);
    }

    ~DeferredInitializationHookGuard()
    {
        wxWinUITopLevelHost::TestSetInitializationHook(nullptr);
        gs_deferredInitializationTLW = nullptr;
        gs_deferredInitializationTLWBindings = 0;
    }

    unsigned GetTLWBindingCount() const
    {
        return gs_deferredInitializationTLWBindings;
    }
};

wxWindow* gs_presentationInvalidationTLW = nullptr;
bool gs_presentationInvalidationObserved = false;

void InvalidatePresentationAfterDropBrokerReady(
    wxWindow* tlw,
    wxWinUITopLevelHost::InitializationHookPointForTest point)
{
    if ( tlw != gs_presentationInvalidationTLW ||
         point != wxWinUITopLevelHost::InitializationHookPointForTest::
                      AfterDropBrokerReadyBeforeCommit ||
         gs_presentationInvalidationObserved )
    {
        return;
    }

    gs_presentationInvalidationObserved = true;
    tlw->Hide();
    tlw->Show();
}

class PresentationInvalidationHookGuard final
{
public:
    explicit PresentationInvalidationHookGuard(wxWindow* tlw)
    {
        gs_presentationInvalidationTLW = tlw;
        gs_presentationInvalidationObserved = false;
        wxWinUITopLevelHost::TestSetInitializationHook(
            InvalidatePresentationAfterDropBrokerReady);
    }

    ~PresentationInvalidationHookGuard()
    {
        wxWinUITopLevelHost::TestSetInitializationHook(nullptr);
        gs_presentationInvalidationTLW = nullptr;
    }

    bool WasObserved() const
        { return gs_presentationInvalidationObserved; }
};

wxWindow* gs_provisionalReparentWindow = nullptr;
wxWindow* gs_provisionalReparentDestination = nullptr;
bool gs_provisionalReparentAttempted = false;
bool gs_provisionalReparentSucceeded = false;

void ReparentDuringProvisionalHostInitialization(
    wxWindow* tlw,
    wxWinUITopLevelHost::InitializationHookPointForTest point)
{
    if ( point != wxWinUITopLevelHost::
                    InitializationHookPointForTest::AfterTLWBindings ||
         tlw != gs_provisionalReparentDestination ||
         !gs_provisionalReparentWindow ||
         gs_provisionalReparentAttempted )
    {
        return;
    }

    gs_provisionalReparentAttempted = true;
    gs_provisionalReparentSucceeded =
        gs_provisionalReparentWindow->Reparent(tlw);
}

class ProvisionalReparentHookGuard final
{
public:
    ProvisionalReparentHookGuard(wxWindow* window, wxWindow* destination)
    {
        gs_provisionalReparentWindow = window;
        gs_provisionalReparentDestination = destination;
        gs_provisionalReparentAttempted = false;
        gs_provisionalReparentSucceeded = false;
        wxWinUITopLevelHost::TestSetInitializationHook(
            ReparentDuringProvisionalHostInitialization);
    }

    ~ProvisionalReparentHookGuard()
    {
        wxWinUITopLevelHost::TestSetInitializationHook(nullptr);
        gs_provisionalReparentWindow = nullptr;
        gs_provisionalReparentDestination = nullptr;
    }

    bool WasAttempted() const { return gs_provisionalReparentAttempted; }
    bool Succeeded() const { return gs_provisionalReparentSucceeded; }
};

class DestroyTLWOnDropTargetLookup final : public wxPanel
{
public:
    DestroyTLWOnDropTargetLookup(wxWindow* parent,
                                 bool* attempted,
                                 bool* accepted)
        : wxPanel(parent, wxID_ANY, wxPoint(20, 20), wxSize(220, 100)),
          m_attempted(attempted),
          m_accepted(accepted)
    {
    }

    wxDropTarget* GetDropTarget() const override
    {
        wxDropTarget* const target = wxPanel::GetDropTarget();
        if ( !m_fired )
        {
            m_fired = true;
            if ( m_attempted )
                *m_attempted = true;
            wxWindow* const tlw = wxGetTopLevelParent(
                const_cast<DestroyTLWOnDropTargetLookup*>(this));
            const bool accepted = tlw && tlw->Destroy();
            if ( m_accepted )
                *m_accepted = accepted;
        }
        return target;
    }

private:
    bool* m_attempted = nullptr;
    bool* m_accepted = nullptr;
    mutable bool m_fired = false;
};

class RoutedTextTarget final : public wxTextDropTarget
{
public:
    wxDragResult OnEnter(wxCoord x,
                         wxCoord y,
                         wxDragResult def) override
    {
        ++enters;
        lastX = x;
        lastY = y;
        return def;
    }

    wxDragResult OnDragOver(wxCoord x,
                            wxCoord y,
                            wxDragResult def) override
    {
        ++overs;
        lastX = x;
        lastY = y;
        return def;
    }

    void OnLeave() override
    {
        ++leaves;
    }

    bool OnDropText(wxCoord x,
                    wxCoord y,
                    const wxString& value) override
    {
        ++drops;
        lastX = x;
        lastY = y;
        text = value;
        return true;
    }

    int enters = 0;
    int overs = 0;
    int leaves = 0;
    int drops = 0;
    int lastX = 0;
    int lastY = 0;
    wxString text;
};

class RoutedFileTarget final : public wxFileDropTarget
{
public:
    bool OnDropFiles(wxCoord x,
                     wxCoord y,
                     const wxArrayString& values) override
    {
        ++drops;
        lastX = x;
        lastY = y;
        files = values;
        return true;
    }

    int drops = 0;
    int lastX = 0;
    int lastY = 0;
    wxArrayString files;
};

constexpr const char* ProductPhysicalProbeArmVariable =
    "WX_WINUI_ALLOW_PHYSICAL_PRODUCT_OLE_PROBE";
constexpr const char* ProductPhysicalProbeArmValue =
    "I_UNDERSTAND_THIS_MOVES_THE_CURSOR_AND_CLICKS";
constexpr DWORD ProductPhysicalProbeMinimumIdleMs = 8000;
constexpr DWORD ProductPhysicalProbeArmTimeoutMs = 3000;
constexpr DWORD ProductPhysicalProbeCancelGraceMs = 5000;
constexpr ULONG_PTR ProductPhysicalProbeInputTag =
    static_cast<ULONG_PTR>(0x42524F4BUL);

bool IsProductPhysicalProbeArmed()
{
    const char* const value =
        std::getenv(ProductPhysicalProbeArmVariable);
    return value && std::strcmp(value, ProductPhysicalProbeArmValue) == 0;
}

bool IsProductPhysicalInputDesktop()
{
    const HDESK desktop = ::GetThreadDesktop(::GetCurrentThreadId());
    BOOL receivesInput = FALSE;
    DWORD bytesNeeded = 0;
    return desktop &&
           ::GetUserObjectInformationW(
               desktop, UOI_IO, &receivesInput, sizeof(receivesInput),
               &bytesNeeded) &&
           receivesInput != FALSE;
}

bool GetProductPhysicalInputIdleMs(DWORD* idleMs)
{
    LASTINPUTINFO info = { sizeof(info) };
    if ( !idleMs || !::GetLastInputInfo(&info) )
        return false;

    // DWORD subtraction intentionally handles GetTickCount() wrap-around.
    *idleMs = ::GetTickCount() - info.dwTime;
    return true;
}

bool WaitForProductPhysicalInputIdle(DWORD timeoutMs, DWORD* idleMs)
{
    const ULONGLONG deadline = ::GetTickCount64() + timeoutMs;
    for ( ;; )
    {
        DWORD currentIdle = 0;
        if ( !GetProductPhysicalInputIdleMs(&currentIdle) )
            return false;
        if ( idleMs )
            *idleMs = currentIdle;
        if ( currentIdle >= ProductPhysicalProbeMinimumIdleMs )
            return true;
        if ( ::GetTickCount64() >= deadline )
            return false;

        wxYield();
        wxMilliSleep(50);
    }
}

constexpr std::size_t ProductPhysicalVirtualKeyCount = 256;
constexpr std::size_t ProductPhysicalHeldInputSampleCount = 3;

using ProductPhysicalVirtualKeyState =
    std::array<unsigned char, ProductPhysicalVirtualKeyCount>;

struct ProductPhysicalHeldInputSample
{
    ProductPhysicalVirtualKeyState asyncDown{};
    ProductPhysicalVirtualKeyState keyboardDown{};
    bool keyboardStateRead = true;
};

enum class ProductPhysicalHeldInputReason
{
    Safe,
    InsufficientSamples,
    IdleNotQualified,
    HookInterrupted,
    KeyboardStateReadFailed,
    StateVaried,
    SafetyKeyHeld,
    OrdinaryKeyHeld,
    KeyboardOnlyState
};

struct ProductPhysicalHeldInputDecision
{
    ProductPhysicalHeldInputReason reason =
        ProductPhysicalHeldInputReason::InsufficientSamples;
    int blockingVirtualKey = 0;
    int firstAsyncOnlyVirtualKey = 0;

    bool IsSafe() const
    {
        return reason == ProductPhysicalHeldInputReason::Safe;
    }
};

bool IsProductPhysicalSafetyVirtualKey(int virtualKey)
{
    switch ( virtualKey )
    {
        case VK_LBUTTON:
        case VK_RBUTTON:
        case VK_MBUTTON:
        case VK_XBUTTON1:
        case VK_XBUTTON2:
        case VK_SHIFT:
        case VK_CONTROL:
        case VK_MENU:
        case VK_ESCAPE:
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_LCONTROL:
        case VK_RCONTROL:
        case VK_LMENU:
        case VK_RMENU:
        case VK_LWIN:
        case VK_RWIN:
            return true;
    }

    return false;
}

ProductPhysicalHeldInputDecision ClassifyProductPhysicalHeldInput(
    const ProductPhysicalHeldInputSample* samples,
    std::size_t sampleCount,
    bool idleQualified,
    bool hookSilent)
{
    ProductPhysicalHeldInputDecision decision;
    if ( !samples || sampleCount < 2 )
        return decision;

    if ( !idleQualified )
    {
        decision.reason =
            ProductPhysicalHeldInputReason::IdleNotQualified;
        return decision;
    }
    if ( !hookSilent )
    {
        decision.reason = ProductPhysicalHeldInputReason::HookInterrupted;
        return decision;
    }

    for ( std::size_t sample = 0; sample < sampleCount; ++sample )
    {
        if ( !samples[sample].keyboardStateRead )
        {
            decision.reason =
                ProductPhysicalHeldInputReason::KeyboardStateReadFailed;
            return decision;
        }
        if ( sample != 0 &&
             (samples[sample].asyncDown != samples[0].asyncDown ||
              samples[sample].keyboardDown != samples[0].keyboardDown) )
        {
            decision.reason = ProductPhysicalHeldInputReason::StateVaried;
            return decision;
        }
    }

    for ( int virtualKey = 1; virtualKey < 255; ++virtualKey )
    {
        const bool asyncDown = samples[0].asyncDown[virtualKey] != 0;
        const bool keyboardDown =
            samples[0].keyboardDown[virtualKey] != 0;
        if ( !asyncDown && !keyboardDown )
            continue;

        if ( IsProductPhysicalSafetyVirtualKey(virtualKey) )
        {
            decision.reason =
                ProductPhysicalHeldInputReason::SafetyKeyHeld;
            decision.blockingVirtualKey = virtualKey;
            return decision;
        }
        if ( asyncDown && keyboardDown )
        {
            decision.reason =
                ProductPhysicalHeldInputReason::OrdinaryKeyHeld;
            decision.blockingVirtualKey = virtualKey;
            return decision;
        }
        if ( keyboardDown )
        {
            // A queue-only DOWN cannot be proven stale without mutating the
            // thread keyboard state. Keep this disagreement fail-closed.
            decision.reason =
                ProductPhysicalHeldInputReason::KeyboardOnlyState;
            decision.blockingVirtualKey = virtualKey;
            return decision;
        }
        if ( decision.firstAsyncOnlyVirtualKey == 0 )
            decision.firstAsyncOnlyVirtualKey = virtualKey;
    }

    // An ordinary async-only key is not an OLE modifier. The already-live
    // low-level hook will still reject any repeat or release transition.
    decision.reason = ProductPhysicalHeldInputReason::Safe;
    return decision;
}

const char* ProductPhysicalHeldInputReasonName(
    ProductPhysicalHeldInputReason reason)
{
    switch ( reason )
    {
        case ProductPhysicalHeldInputReason::Safe:
            return "safe";
        case ProductPhysicalHeldInputReason::InsufficientSamples:
            return "insufficient-samples";
        case ProductPhysicalHeldInputReason::IdleNotQualified:
            return "idle-not-qualified";
        case ProductPhysicalHeldInputReason::HookInterrupted:
            return "hook-interrupted";
        case ProductPhysicalHeldInputReason::KeyboardStateReadFailed:
            return "keyboard-state-read-failed";
        case ProductPhysicalHeldInputReason::StateVaried:
            return "state-varied";
        case ProductPhysicalHeldInputReason::SafetyKeyHeld:
            return "safety-key-held";
        case ProductPhysicalHeldInputReason::OrdinaryKeyHeld:
            return "ordinary-key-held";
        case ProductPhysicalHeldInputReason::KeyboardOnlyState:
            return "keyboard-only-state";
    }

    return "unknown";
}

ProductPhysicalHeldInputSample CaptureProductPhysicalHeldInputSample()
{
    ProductPhysicalHeldInputSample sample;
    BYTE keyboardState[ProductPhysicalVirtualKeyCount]{};
    sample.keyboardStateRead =
        ::GetKeyboardState(keyboardState) != FALSE;
    for ( int virtualKey = 1; virtualKey < 255; ++virtualKey )
    {
        sample.asyncDown[virtualKey] =
            (::GetAsyncKeyState(virtualKey) & 0x8000) != 0;
        if ( sample.keyboardStateRead )
        {
            sample.keyboardDown[virtualKey] =
                (keyboardState[virtualKey] & 0x80) != 0;
        }
    }
    return sample;
}

struct ProductPhysicalInputState
{
    std::atomic<bool> interrupted{ false };
    std::atomic<unsigned> taggedMouseEvents{ 0 };
    std::atomic<unsigned> kind{ 0 };
    std::atomic<ULONG_PTR> message{ 0 };
    std::atomic<DWORD> flags{ 0 };
    std::atomic<LONG> x{ 0 };
    std::atomic<LONG> y{ 0 };
    std::atomic<ULONG_PTR> extraInfo{ 0 };

    void Record(unsigned inputKind,
                WPARAM inputMessage,
                DWORD inputFlags,
                POINT point,
                ULONG_PTR inputExtraInfo) noexcept
    {
        if ( interrupted.load(std::memory_order_relaxed) )
            return;

        kind.store(inputKind, std::memory_order_relaxed);
        message.store(inputMessage, std::memory_order_relaxed);
        flags.store(inputFlags, std::memory_order_relaxed);
        x.store(point.x, std::memory_order_relaxed);
        y.store(point.y, std::memory_order_relaxed);
        extraInfo.store(inputExtraInfo, std::memory_order_relaxed);
        interrupted.store(true, std::memory_order_release);
    }

    void RecordTaggedMouse() noexcept
    {
        taggedMouseEvents.fetch_add(1, std::memory_order_release);
    }
};

std::atomic<ProductPhysicalInputState*> gs_productPhysicalInputState{
    nullptr
};

LRESULT CALLBACK ProductPhysicalMouseHook(
    int code, WPARAM wParam, LPARAM lParam)
{
    if ( code >= 0 )
    {
        const auto* const data =
            reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
        ProductPhysicalInputState* const state =
            gs_productPhysicalInputState.load(std::memory_order_acquire);
        if ( state && data )
        {
            if ( data->dwExtraInfo == ProductPhysicalProbeInputTag &&
                 (data->flags & LLMHF_INJECTED) != 0 )
            {
                state->RecordTaggedMouse();
            }
            else
            {
                state->Record(
                    1, wParam, data->flags, data->pt, data->dwExtraInfo);
            }
        }
    }

    return ::CallNextHookEx(nullptr, code, wParam, lParam);
}

LRESULT CALLBACK ProductPhysicalKeyboardHook(
    int code, WPARAM wParam, LPARAM lParam)
{
    if ( code >= 0 )
    {
        const auto* const data =
            reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
        ProductPhysicalInputState* const state =
            gs_productPhysicalInputState.load(std::memory_order_acquire);
        if ( state && data &&
             (data->dwExtraInfo != ProductPhysicalProbeInputTag ||
              (data->flags & LLKHF_INJECTED) == 0) )
        {
            state->Record(
                2, wParam, data->flags, POINT{}, data->dwExtraInfo);
        }
    }

    return ::CallNextHookEx(nullptr, code, wParam, lParam);
}

class ProductPhysicalInputGuard final
{
public:
    bool Install()
    {
        ProductPhysicalInputState* expected = nullptr;
        if ( !gs_productPhysicalInputState.compare_exchange_strong(
                expected, &m_state, std::memory_order_acq_rel) )
        {
            return false;
        }

        const HINSTANCE module = ::GetModuleHandleW(nullptr);
        m_mouseHook = ::SetWindowsHookExW(
            WH_MOUSE_LL, ProductPhysicalMouseHook, module, 0);
        m_keyboardHook = ::SetWindowsHookExW(
            WH_KEYBOARD_LL, ProductPhysicalKeyboardHook, module, 0);
        if ( !m_mouseHook || !m_keyboardHook )
        {
            Remove();
            return false;
        }
        return true;
    }

    ~ProductPhysicalInputGuard()
    {
        Remove();
    }

    bool WasInterrupted() const
    {
        return m_state.interrupted.load(std::memory_order_acquire);
    }

    unsigned TaggedMouseEventCount() const
    {
        return m_state.taggedMouseEvents.load(std::memory_order_acquire);
    }

    // Freeze the oracle before its final report. Once this returns, no new
    // hook callback can mutate m_state and PrintStatus()/the JSON summary are
    // guaranteed to describe the same terminal observation.
    bool Stop()
    {
        return Remove();
    }

    void PrintStatus() const
    {
        if ( !m_hooksRemovedCleanly )
        {
            std::printf(
                "wx_winui_product_ole_delivery: {\"event\":"
                "\"input-guard\",\"result\":\"cleanup-failed\","
                "\"tagged_mouse_events\":%u}\n",
                TaggedMouseEventCount());
        }
        else if ( !WasInterrupted() )
        {
            std::printf(
                "wx_winui_product_ole_delivery: {\"event\":"
                "\"input-guard\",\"result\":\"clean\","
                "\"tagged_mouse_events\":%u}\n",
                TaggedMouseEventCount());
        }
        else
        {
            std::printf(
                "wx_winui_product_ole_delivery: {\"event\":"
                "\"input-interruption\",\"kind\":%u,"
                "\"message\":%llu,\"flags\":%lu,"
                "\"point\":[%ld,%ld],\"extra_info\":\"0x%llx\"}\n",
                m_state.kind.load(std::memory_order_relaxed),
                static_cast<unsigned long long>(
                    m_state.message.load(std::memory_order_relaxed)),
                static_cast<unsigned long>(
                    m_state.flags.load(std::memory_order_relaxed)),
                m_state.x.load(std::memory_order_relaxed),
                m_state.y.load(std::memory_order_relaxed),
                static_cast<unsigned long long>(
                    m_state.extraInfo.load(std::memory_order_relaxed)));
        }
        std::fflush(stdout);
    }

private:
    bool Remove()
    {
        if ( m_stopped )
            return m_hooksRemovedCleanly;
        m_stopped = true;

        ProductPhysicalInputState* expected = &m_state;
        (void)gs_productPhysicalInputState.compare_exchange_strong(
            expected, nullptr, std::memory_order_acq_rel);

        if ( m_keyboardHook )
        {
            if ( !::UnhookWindowsHookEx(m_keyboardHook) )
                m_hooksRemovedCleanly = false;
            m_keyboardHook = nullptr;
        }
        if ( m_mouseHook )
        {
            if ( !::UnhookWindowsHookEx(m_mouseHook) )
                m_hooksRemovedCleanly = false;
            m_mouseHook = nullptr;
        }
        return m_hooksRemovedCleanly;
    }

    ProductPhysicalInputState m_state;
    HHOOK m_mouseHook = nullptr;
    HHOOK m_keyboardHook = nullptr;
    bool m_stopped = false;
    bool m_hooksRemovedCleanly = true;
};

bool InjectProductPhysicalPoint(POINT point)
{
    const int left = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int top = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int width = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int height = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if ( width <= 1 || height <= 1 )
        return false;

    const auto normalize = [](LONG value, LONG origin, LONG extent) -> LONG
    {
        return static_cast<LONG>(
            static_cast<long long>(value - origin) * 65535LL /
            (extent - 1));
    };

    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dx = normalize(point.x, left, width);
    input.mi.dy = normalize(point.y, top, height);
    input.mi.dwFlags =
        MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
    input.mi.dwExtraInfo = ProductPhysicalProbeInputTag;
    return ::SendInput(1, &input, sizeof(input)) == 1;
}

bool InjectProductPhysicalLeftButton(bool down)
{
    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = down ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
    input.mi.dwExtraInfo = ProductPhysicalProbeInputTag;
    return ::SendInput(1, &input, sizeof(input)) == 1;
}

bool ProductPhysicalPointsAreClose(
    POINT first, POINT second, LONG tolerance = 3)
{
    return std::abs(first.x - second.x) <= tolerance &&
           std::abs(first.y - second.y) <= tolerance;
}

template <typename Predicate>
bool PumpProductPhysicalUntil(Predicate predicate, DWORD timeoutMs)
{
    const ULONGLONG deadline = ::GetTickCount64() + timeoutMs;
    while ( !predicate() )
    {
        if ( ::GetTickCount64() >= deadline )
            return false;
        wxYield();
        wxMilliSleep(5);
    }
    return true;
}

struct ProductPhysicalPointOwner
{
    HWND root = nullptr;
    HWND exact = nullptr;

    bool Matches(HWND window) const
    {
        if ( !window || !root )
            return false;
        if ( exact && window != exact )
            return false;
        return window == root || ::IsChild(root, window);
    }

    bool Owns(POINT point) const
    {
        return Matches(::WindowFromPoint(point));
    }
};

class ProductPhysicalForegroundRestore final
{
public:
    explicit ProductPhysicalForegroundRestore(HWND foreground)
        : m_foreground(foreground)
    {
    }

    ~ProductPhysicalForegroundRestore()
    {
        if ( m_foreground && ::IsWindow(m_foreground) )
            (void)::SetForegroundWindow(m_foreground);
    }

    ProductPhysicalForegroundRestore(
        const ProductPhysicalForegroundRestore&) = delete;
    ProductPhysicalForegroundRestore& operator=(
        const ProductPhysicalForegroundRestore&) = delete;

private:
    HWND m_foreground = nullptr;
};

class ProductPhysicalTextTarget final : public wxTextDropTarget
{
public:
    wxDragResult OnEnter(wxCoord x,
                         wxCoord y,
                         wxDragResult def) override
    {
        lastX = x;
        lastY = y;
        lastEnterDefault = def;
        m_enters.fetch_add(1, std::memory_order_release);
        return def;
    }

    wxDragResult OnDragOver(wxCoord x,
                            wxCoord y,
                            wxDragResult def) override
    {
        lastX = x;
        lastY = y;
        lastOverDefault = def;
        ++overs;
        return def;
    }

    void OnLeave() override
    {
        ++leaves;
    }

    bool OnDropText(wxCoord x,
                    wxCoord y,
                    const wxString& value) override
    {
        lastX = x;
        lastY = y;
        text = value;
        ++drops;
        return true;
    }

    unsigned EnterCount() const
    {
        return m_enters.load(std::memory_order_acquire);
    }

    unsigned overs = 0;
    unsigned leaves = 0;
    unsigned drops = 0;
    wxCoord lastX = 0;
    wxCoord lastY = 0;
    wxDragResult lastEnterDefault = wxDragNone;
    wxDragResult lastOverDefault = wxDragNone;
    wxString text;

private:
    std::atomic<unsigned> m_enters{ 0 };
};

struct ProductPhysicalDragControl
{
    const ProductPhysicalInputGuard* inputGuard = nullptr;
    const ProductPhysicalPointOwner* pointOwner = nullptr;
    POINT dropPoint{};
    std::atomic<bool> driverReady{ false };
    std::atomic<bool> dragStarted{ false };
    std::atomic<bool> driverFinished{ false };
    std::atomic<bool> requestDrop{ false };
    std::atomic<bool> cancel{ false };
    std::atomic<bool> leftButtonHeld{ false };
    std::atomic<bool> injectionFailed{ false };
    std::atomic<bool> timedOut{ false };
    std::atomic<unsigned> queryContinueCount{ 0 };
};

class ProductPhysicalDropSource final : public IDropSource
{
public:
    explicit ProductPhysicalDropSource(ProductPhysicalDragControl* control)
        : m_control(control)
    {
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(
        REFIID iid, void** object) override
    {
        if ( !object )
            return E_POINTER;
        *object = nullptr;
        if ( iid == IID_IUnknown || iid == IID_IDropSource )
        {
            *object = static_cast<IDropSource*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override
    {
        return static_cast<ULONG>(::InterlockedIncrement(&m_references));
    }

    ULONG STDMETHODCALLTYPE Release() override
    {
        const LONG references = ::InterlockedDecrement(&m_references);
        if ( references == 0 )
            delete this;
        return static_cast<ULONG>(references);
    }

    HRESULT STDMETHODCALLTYPE QueryContinueDrag(
        BOOL escapePressed, DWORD keyState) override
    {
        m_control->queryContinueCount.fetch_add(
            1, std::memory_order_relaxed);
        if ( escapePressed ||
             m_control->cancel.load(std::memory_order_acquire) )
        {
            return DRAGDROP_S_CANCEL;
        }
        if ( (keyState & MK_LBUTTON) == 0 )
        {
            POINT current{};
            const bool ownsDropPoint =
                m_control->pointOwner &&
                m_control->pointOwner->Owns(m_control->dropPoint) &&
                ::GetCursorPos(&current) &&
                ProductPhysicalPointsAreClose(
                    current, m_control->dropPoint);
            return m_control->requestDrop.load(std::memory_order_acquire) &&
                   ownsDropPoint
                       ? DRAGDROP_S_DROP
                       : DRAGDROP_S_CANCEL;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD) override
    {
        return DRAGDROP_S_USEDEFAULTCURSORS;
    }

private:
    ~ProductPhysicalDropSource() = default;

    LONG m_references = 1;
    ProductPhysicalDragControl* m_control = nullptr;
};

struct ProductPhysicalDropSourceReleaser
{
    void operator()(ProductPhysicalDropSource* source) const
    {
        if ( source )
            source->Release();
    }
};

using ProductPhysicalDropSourcePtr =
    std::unique_ptr<ProductPhysicalDropSource,
                    ProductPhysicalDropSourceReleaser>;

bool ReleaseProductPhysicalLeftButton(ProductPhysicalDragControl& control)
{
    bool expected = true;
    if ( !control.leftButtonHeld.compare_exchange_strong(
            expected, false, std::memory_order_acq_rel) )
    {
        return true;
    }

    const bool released = InjectProductPhysicalLeftButton(false);
    if ( !released )
    {
        control.leftButtonHeld.store(true, std::memory_order_release);
        control.injectionFailed.store(true, std::memory_order_release);
    }
    return released;
}

bool EnsureProductPhysicalLeftButtonReleased(
    ProductPhysicalDragControl& control, DWORD timeoutMs = 500)
{
    const ULONGLONG deadline = ::GetTickCount64() + timeoutMs;
    for ( ;; )
    {
        const bool heldByProbe =
            control.leftButtonHeld.load(std::memory_order_acquire);
        const bool observedDown =
            (::GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        if ( !heldByProbe && !observedDown )
            return true;

        bool injected = true;
        if ( heldByProbe )
            injected = ReleaseProductPhysicalLeftButton(control);
        else
            injected = InjectProductPhysicalLeftButton(false);
        if ( !injected )
            control.injectionFailed.store(true, std::memory_order_release);

        if ( ::GetTickCount64() >= deadline )
            break;
        ::Sleep(10);
    }

    return !control.leftButtonHeld.load(std::memory_order_acquire) &&
           (::GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0;
}

void WakeProductPhysicalDragLoop(DWORD uiThread)
{
    (void)::PostThreadMessageW(uiThread, WM_KEYDOWN, VK_ESCAPE, 0);
    (void)::PostThreadMessageW(uiThread, WM_KEYUP, VK_ESCAPE, 0);
}

[[noreturn]] void AbortHungProductPhysicalDrag(
    ProductPhysicalDragControl& control, const char* reason)
{
    const bool buttonUp =
        EnsureProductPhysicalLeftButtonReleased(control, 1000);
    std::fprintf(
        stderr,
        "wx_winui_product_ole_delivery: {\"event\":\"summary\","
        "\"result\":\"failed\",\"reason\":\"%s\","
        "\"button_up\":%s,"
        "\"exit_code\":20}\n",
        reason,
        buttonUp ? "true" : "false");
    std::fflush(stderr);
    ::ExitProcess(20);
}

struct ProductPhysicalDragResult
{
    HRESULT dragHr = E_UNEXPECTED;
    DWORD effect = DROPEFFECT_NONE;
    bool attempted = false;
    bool inputInterrupted = false;
    bool injectionFailed = false;
    bool timedOut = false;
    bool buttonDownObserved = false;
    bool buttonUpObserved = false;
    unsigned queryContinueCount = 0;
    unsigned enterDelta = 0;
    unsigned overDelta = 0;
    unsigned leaveDelta = 0;
    unsigned dropDelta = 0;
    wxCoord lastX = 0;
    wxCoord lastY = 0;
    wxDragResult lastEnterDefault = wxDragNone;
    wxDragResult lastOverDefault = wxDragNone;
    wxString text;

    bool Delivered() const
    {
        return attempted && dragHr == DRAGDROP_S_DROP &&
               effect == DROPEFFECT_COPY &&
               !inputInterrupted && !injectionFailed && !timedOut &&
               buttonDownObserved && buttonUpObserved &&
               enterDelta == 1 && leaveDelta == 0 && dropDelta == 1 &&
               lastEnterDefault == wxDragCopy;
    }
};

void PrintProductPhysicalDragResult(
    const char* surface,
    const ProductPhysicalDragResult& result,
    const wxWinUIDropBrokerSnapshot& brokerBefore,
    const wxWinUIDropBrokerSnapshot& brokerAfter)
{
    std::printf(
        "wx_winui_product_ole_delivery: {\"event\":\"arm\","
        "\"surface\":\"%s\",\"attempted\":%s,"
        "\"drag_hr\":\"0x%08lx\",\"effect\":%lu,"
        "\"input_interrupted\":%s,\"injection_failed\":%s,"
        "\"timed_out\":%s,\"button_down\":%s,\"button_up\":%s,"
        "\"query_continue\":%u,\"enter\":%u,\"over\":%u,"
        "\"leave\":%u,\"drop\":%u,"
        "\"broker_drag_enter\":%u,\"broker_drag_over\":%u,"
        "\"broker_drag_leave\":%u,\"broker_drop\":%u,"
        "\"delivered\":%s}\n",
        surface,
        result.attempted ? "true" : "false",
        static_cast<unsigned long>(result.dragHr),
        static_cast<unsigned long>(result.effect),
        result.inputInterrupted ? "true" : "false",
        result.injectionFailed ? "true" : "false",
        result.timedOut ? "true" : "false",
        result.buttonDownObserved ? "true" : "false",
        result.buttonUpObserved ? "true" : "false",
        result.queryContinueCount,
        result.enterDelta,
        result.overDelta,
        result.leaveDelta,
        result.dropDelta,
        brokerAfter.dragEnterCalls - brokerBefore.dragEnterCalls,
        brokerAfter.dragOverCalls - brokerBefore.dragOverCalls,
        brokerAfter.dragLeaveCalls - brokerBefore.dragLeaveCalls,
        brokerAfter.dropCalls - brokerBefore.dropCalls,
        result.Delivered() ? "true" : "false");
    std::fflush(stdout);
}

ProductPhysicalDragResult RunProductPhysicalDrag(
    HWND sourceHwnd,
    POINT departurePoint,
    POINT targetPoint,
    POINT nearbyPoint,
    const wxString& payload,
    ProductPhysicalTextTarget* target,
    const ProductPhysicalInputGuard& inputGuard,
    const ProductPhysicalPointOwner& pointOwner)
{
    ProductPhysicalDragResult result;
    if ( !sourceHwnd || !target )
        return result;
    if ( inputGuard.WasInterrupted() )
    {
        result.inputInterrupted = true;
        return result;
    }

    const unsigned entersBefore = target->EnterCount();
    const unsigned oversBefore = target->overs;
    const unsigned leavesBefore = target->leaves;
    const unsigned dropsBefore = target->drops;

    if ( !IsProductPhysicalInputDesktop() ||
         ::WindowFromPoint(departurePoint) != sourceHwnd ||
         !pointOwner.Owns(targetPoint) ||
         !pointOwner.Owns(nearbyPoint) ||
         !InjectProductPhysicalPoint(departurePoint) )
    {
        result.injectionFailed = true;
        return result;
    }

    const bool departureReached = PumpProductPhysicalUntil(
        [&]()
        {
            POINT current{};
            return ::GetCursorPos(&current) &&
                   ProductPhysicalPointsAreClose(
                       current, departurePoint) &&
                   ::WindowFromPoint(current) == sourceHwnd;
        },
        1000);
    if ( !departureReached || inputGuard.WasInterrupted() )
    {
        result.injectionFailed = !departureReached;
        result.inputInterrupted = inputGuard.WasInterrupted();
        return result;
    }

    ProductPhysicalDragControl control;
    control.inputGuard = &inputGuard;
    control.pointOwner = &pointOwner;
    control.dropPoint = nearbyPoint;
    const DWORD uiThread = ::GetCurrentThreadId();

    // Construct every potentially throwing object before starting the driver.
    // This guarantees that a construction failure can never destroy a
    // joinable thread or strand a synthetic button-down.
    wxTextDataObject data(payload);
    ProductPhysicalDropSourcePtr source(
        new ProductPhysicalDropSource(&control));

    std::thread driver(
        [&]()
        {
            control.driverReady.store(true, std::memory_order_release);

            const auto cancelAndWakeUntilFinished = [&]()
            {
                if ( !EnsureProductPhysicalLeftButtonReleased(control) )
                {
                    AbortHungProductPhysicalDrag(
                        control, "left-button-release");
                }
                control.cancel.store(true, std::memory_order_release);
                const ULONGLONG cancelDeadline =
                    ::GetTickCount64() + ProductPhysicalProbeCancelGraceMs;
                while ( !control.driverFinished.load(
                            std::memory_order_acquire) )
                {
                    if ( ::GetTickCount64() >= cancelDeadline )
                    {
                        AbortHungProductPhysicalDrag(
                            control, "do-drag-drop-cancel-grace");
                    }
                    WakeProductPhysicalDragLoop(uiThread);
                    ::Sleep(25);
                }
            };

            while ( !control.dragStarted.load(std::memory_order_acquire) &&
                    !control.driverFinished.load(std::memory_order_acquire) )
            {
                ::Sleep(1);
            }
            if ( control.driverFinished.load(std::memory_order_acquire) )
                return;

            ::Sleep(100);
            if ( inputGuard.WasInterrupted() ||
                 !IsProductPhysicalInputDesktop() ||
                 !pointOwner.Owns(targetPoint) ||
                 !pointOwner.Owns(nearbyPoint) )
            {
                if ( !inputGuard.WasInterrupted() )
                {
                    control.injectionFailed.store(
                        true, std::memory_order_release);
                }
                cancelAndWakeUntilFinished();
                return;
            }

            if ( !InjectProductPhysicalPoint(targetPoint) )
            {
                control.injectionFailed.store(
                    true, std::memory_order_release);
                cancelAndWakeUntilFinished();
                return;
            }

            const ULONGLONG deadline =
                ::GetTickCount64() + ProductPhysicalProbeArmTimeoutMs;
            bool targetReached = false;
            bool targetOwnershipLost = false;
            while ( !control.driverFinished.load(
                        std::memory_order_acquire) &&
                    !inputGuard.WasInterrupted() &&
                    ::GetTickCount64() < deadline )
            {
                if ( !IsProductPhysicalInputDesktop() ||
                     !pointOwner.Owns(targetPoint) ||
                     !pointOwner.Owns(nearbyPoint) )
                {
                    targetOwnershipLost = true;
                    break;
                }

                POINT current{};
                if ( ::GetCursorPos(&current) &&
                     ProductPhysicalPointsAreClose(current, targetPoint) )
                {
                    if ( !pointOwner.Owns(current) )
                    {
                        targetOwnershipLost = true;
                        break;
                    }
                    targetReached = true;
                    if ( target->EnterCount() != entersBefore )
                        break;
                }
                ::Sleep(10);
            }

            if ( control.driverFinished.load(std::memory_order_acquire) )
                return;
            if ( inputGuard.WasInterrupted() )
            {
                cancelAndWakeUntilFinished();
                return;
            }
            if ( targetOwnershipLost || !targetReached )
            {
                control.injectionFailed.store(
                    true, std::memory_order_release);
                cancelAndWakeUntilFinished();
                return;
            }
            if ( target->EnterCount() == entersBefore )
            {
                control.timedOut.store(true, std::memory_order_release);
                cancelAndWakeUntilFinished();
                return;
            }

            if ( !pointOwner.Owns(nearbyPoint) ||
                 !InjectProductPhysicalPoint(nearbyPoint) )
            {
                control.injectionFailed.store(
                    true, std::memory_order_release);
                cancelAndWakeUntilFinished();
                return;
            }

            const ULONGLONG nearbyDeadline =
                ::GetTickCount64() + 1000;
            bool nearbyReached = false;
            while ( !control.driverFinished.load(
                        std::memory_order_acquire) &&
                    !inputGuard.WasInterrupted() &&
                    ::GetTickCount64() < nearbyDeadline )
            {
                if ( !IsProductPhysicalInputDesktop() ||
                     !pointOwner.Owns(nearbyPoint) )
                {
                    break;
                }
                POINT current{};
                if ( ::GetCursorPos(&current) &&
                     ProductPhysicalPointsAreClose(current, nearbyPoint) )
                {
                    nearbyReached = pointOwner.Owns(current);
                    break;
                }
                ::Sleep(10);
            }
            if ( control.driverFinished.load(std::memory_order_acquire) )
                return;
            if ( inputGuard.WasInterrupted() )
            {
                cancelAndWakeUntilFinished();
                return;
            }
            if ( !nearbyReached || !pointOwner.Owns(nearbyPoint) )
            {
                control.injectionFailed.store(
                    true, std::memory_order_release);
                cancelAndWakeUntilFinished();
                return;
            }

            control.requestDrop.store(true, std::memory_order_release);
            if ( !ReleaseProductPhysicalLeftButton(control) )
            {
                cancelAndWakeUntilFinished();
                return;
            }

            // Delivery gets its own deadline. Reusing the Enter deadline can
            // manufacture a timeout after an otherwise valid long Enter arm.
            const ULONGLONG completionDeadline =
                ::GetTickCount64() + ProductPhysicalProbeArmTimeoutMs;
            while ( !control.driverFinished.load(
                        std::memory_order_acquire) )
            {
                if ( inputGuard.WasInterrupted() )
                {
                    cancelAndWakeUntilFinished();
                    return;
                }
                if ( ::GetTickCount64() >= completionDeadline )
                {
                    control.timedOut.store(
                        true, std::memory_order_release);
                    cancelAndWakeUntilFinished();
                    return;
                }
                ::Sleep(10);
            }
        });

    const bool driverReady = PumpProductPhysicalUntil(
        [&]()
        {
            return control.driverReady.load(std::memory_order_acquire);
        },
        1000);
    if ( !driverReady || inputGuard.WasInterrupted() ||
         !IsProductPhysicalInputDesktop() ||
         ::WindowFromPoint(departurePoint) != sourceHwnd ||
         !pointOwner.Owns(targetPoint) ||
         !pointOwner.Owns(nearbyPoint) )
    {
        control.injectionFailed.store(
            !driverReady || !inputGuard.WasInterrupted(),
            std::memory_order_release);
        control.driverFinished.store(true, std::memory_order_release);
        driver.join();
        result.inputInterrupted = inputGuard.WasInterrupted();
        result.injectionFailed =
            control.injectionFailed.load(std::memory_order_acquire);
        return result;
    }

    if ( !InjectProductPhysicalLeftButton(true) )
    {
        control.injectionFailed.store(true, std::memory_order_release);
        control.driverFinished.store(true, std::memory_order_release);
        driver.join();
        result.injectionFailed = true;
        return result;
    }
    control.leftButtonHeld.store(true, std::memory_order_release);
    result.buttonDownObserved = PumpProductPhysicalUntil(
        []()
        {
            return (::GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        },
        250);
    if ( !result.buttonDownObserved || inputGuard.WasInterrupted() )
    {
        if ( !result.buttonDownObserved )
        {
            control.injectionFailed.store(
                true, std::memory_order_release);
        }
        control.driverFinished.store(true, std::memory_order_release);
        const bool released =
            EnsureProductPhysicalLeftButtonReleased(control);
        driver.join();
        if ( !released )
        {
            AbortHungProductPhysicalDrag(
                control, "left-button-release");
        }
        result.inputInterrupted = inputGuard.WasInterrupted();
        result.injectionFailed =
            control.injectionFailed.load(std::memory_order_acquire);
        result.buttonUpObserved = released;
        return result;
    }

    control.dragStarted.store(true, std::memory_order_release);
    result.attempted = true;
    result.dragHr = ::DoDragDrop(
        data.GetInterface(), source.get(), DROPEFFECT_COPY, &result.effect);
    control.driverFinished.store(true, std::memory_order_release);
    driver.join();
    result.buttonUpObserved =
        EnsureProductPhysicalLeftButtonReleased(control);
    if ( !result.buttonUpObserved )
    {
        AbortHungProductPhysicalDrag(control, "left-button-release");
    }

    result.inputInterrupted = inputGuard.WasInterrupted();
    result.injectionFailed =
        control.injectionFailed.load(std::memory_order_acquire);
    result.timedOut = control.timedOut.load(std::memory_order_acquire);
    result.queryContinueCount =
        control.queryContinueCount.load(std::memory_order_acquire);
    result.enterDelta = target->EnterCount() - entersBefore;
    result.overDelta = target->overs - oversBefore;
    result.leaveDelta = target->leaves - leavesBefore;
    result.dropDelta = target->drops - dropsBefore;
    result.lastX = target->lastX;
    result.lastY = target->lastY;
    result.lastEnterDefault = target->lastEnterDefault;
    result.lastOverDefault = target->lastOverDefault;
    result.text = target->text;
    return result;
}

class ProductPhysicalSourceWindow final
{
public:
    ~ProductPhysicalSourceWindow()
    {
        if ( m_hwnd && ::IsWindow(m_hwnd) )
            ::DestroyWindow(m_hwnd);
    }

    bool Create(const RECT& targetSurface)
    {
        // Match the already-qualified raw OLE gesture: the button-down starts
        // on a tiny probe-owned popup inside the visible bridge, then crosses
        // to the real target. This keeps the whole injected path bounded to
        // the fixture and removes desktop/window geometry as a test variable.
        constexpr int Size = 24;
        constexpr int Margin = 8;
        const int x = targetSurface.left + Margin;
        const int y = targetSurface.top + Margin;
        if ( x + Size >= targetSurface.right ||
             y + Size >= targetSurface.bottom )
        {
            return false;
        }

        m_hwnd = ::CreateWindowExW(
            WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
            L"STATIC", L"wx OLE source",
            WS_POPUP | WS_VISIBLE | SS_NOTIFY,
            x, y, Size, Size,
            nullptr, nullptr, wxGetInstance(), nullptr);
        if ( !m_hwnd )
            return false;

        (void)::SetWindowPos(
            m_hwnd, HWND_TOPMOST, x, y, Size, Size,
            SWP_NOACTIVATE | SWP_SHOWWINDOW);
        m_point = { x + Size / 2, y + Size / 2 };
        return ::WindowFromPoint(m_point) == m_hwnd;
    }

    HWND Handle() const { return m_hwnd; }
    POINT GetPoint() const { return m_point; }

private:
    HWND m_hwnd = nullptr;
    POINT m_point{};
};

class ReentrantDestroyTextTarget final : public wxTextDropTarget
{
public:
    enum class Trigger
    {
        Enter,
        Data
    };

    ReentrantDestroyTextTarget(Trigger trigger, std::function<void()> action)
        : m_trigger(trigger),
          m_action(std::move(action))
    {
    }

    wxDragResult OnEnter(wxCoord,
                         wxCoord,
                         wxDragResult def) override
    {
        if ( m_trigger == Trigger::Enter )
            RunAction();
        return def;
    }

    bool OnDropText(wxCoord,
                    wxCoord,
                    const wxString&) override
    {
        if ( m_trigger == Trigger::Data )
            RunAction();
        return true;
    }

private:
    void RunAction()
    {
        // Move the callback off the target before it destroys the owning TLW
        // (and, with it, this target).
        std::function<void()> action = std::move(m_action);
        if ( action )
            action();
    }

    Trigger m_trigger;
    std::function<void()> m_action;
};

class RedirectPanel final : public wxPanel
{
public:
    RedirectPanel(wxWindow* parent,
                  const wxPoint& pos,
                  const wxSize& size)
        : wxPanel(parent, wxID_ANY, pos, size)
    {
    }

    void RedirectTo(wxWindow* owner)
        { m_owner = owner; }

    wxDropTarget* GetDropTarget() const override
        { return m_owner ? m_owner->GetDropTarget() : nullptr; }

private:
    wxWindow* m_owner = nullptr;
};

HDROP MakeDropFiles(const POINT& bridgeClient, const wxString& path)
{
    const size_t chars = path.length() + 2;
    const size_t bytes = sizeof(DROPFILES) + chars * sizeof(wchar_t);
    HGLOBAL global = ::GlobalAlloc(GHND, bytes);
    if ( !global )
        return nullptr;

    DROPFILES* const drop =
        static_cast<DROPFILES*>(::GlobalLock(global));
    if ( !drop )
    {
        ::GlobalFree(global);
        return nullptr;
    }

    drop->pFiles = sizeof(DROPFILES);
    drop->pt = bridgeClient;
    drop->fNC = FALSE;
    drop->fWide = TRUE;
    wchar_t* const names =
        reinterpret_cast<wchar_t*>(
            reinterpret_cast<unsigned char*>(drop) + sizeof(DROPFILES));
    memcpy(names, path.wc_str(), path.length() * sizeof(wchar_t));
    names[path.length()] = L'\0';
    names[path.length() + 1] = L'\0';
    ::GlobalUnlock(global);
    return reinterpret_cast<HDROP>(global);
}

} // anonymous namespace

// Keep the safety oracle independently testable without installing hooks or
// reading, moving or clicking any real desktop input.
TEST_CASE("WinUI product OLE held-input classifier is fail-closed",
          "[winui-drop-broker][product-input-classifier]")
{
    using Samples =
        std::array<ProductPhysicalHeldInputSample,
                   ProductPhysicalHeldInputSampleCount>;

    SECTION("a stable ordinary async-only key is diagnostic")
    {
        Samples samples{};
        for ( ProductPhysicalHeldInputSample& sample : samples )
            sample.asyncDown[VK_OEM_PERIOD] = 1;

        const ProductPhysicalHeldInputDecision decision =
            ClassifyProductPhysicalHeldInput(
                samples.data(), samples.size(), true, true);
        CHECK(decision.IsSafe());
        CHECK(decision.reason == ProductPhysicalHeldInputReason::Safe);
        CHECK(decision.blockingVirtualKey == 0);
        CHECK(decision.firstAsyncOnlyVirtualKey == VK_OEM_PERIOD);
    }

    SECTION("an ordinary key corroborated by the queue blocks")
    {
        Samples samples{};
        for ( ProductPhysicalHeldInputSample& sample : samples )
        {
            sample.asyncDown[VK_OEM_PERIOD] = 1;
            sample.keyboardDown[VK_OEM_PERIOD] = 1;
        }

        const ProductPhysicalHeldInputDecision decision =
            ClassifyProductPhysicalHeldInput(
                samples.data(), samples.size(), true, true);
        CHECK_FALSE(decision.IsSafe());
        CHECK(decision.reason ==
              ProductPhysicalHeldInputReason::OrdinaryKeyHeld);
        CHECK(decision.blockingVirtualKey == VK_OEM_PERIOD);
    }

    SECTION("every async-only safety key remains fail-closed")
    {
        const int safetyVirtualKeys[] =
        {
            VK_LBUTTON, VK_RBUTTON, VK_MBUTTON,
            VK_XBUTTON1, VK_XBUTTON2,
            VK_SHIFT, VK_LSHIFT, VK_RSHIFT,
            VK_CONTROL, VK_LCONTROL, VK_RCONTROL,
            VK_MENU, VK_LMENU, VK_RMENU,
            VK_LWIN, VK_RWIN, VK_ESCAPE
        };
        for ( const int virtualKey : safetyVirtualKeys )
        {
            INFO("safety virtual key was " << virtualKey);
            Samples samples{};
            for ( ProductPhysicalHeldInputSample& sample : samples )
                sample.asyncDown[virtualKey] = 1;

            const ProductPhysicalHeldInputDecision decision =
                ClassifyProductPhysicalHeldInput(
                    samples.data(), samples.size(), true, true);
            CHECK_FALSE(decision.IsSafe());
            CHECK(decision.reason ==
                  ProductPhysicalHeldInputReason::SafetyKeyHeld);
            CHECK(decision.blockingVirtualKey == virtualKey);
        }
    }

    SECTION("a keyboard-state read failure blocks")
    {
        Samples samples{};
        samples[1].keyboardStateRead = false;

        const ProductPhysicalHeldInputDecision decision =
            ClassifyProductPhysicalHeldInput(
                samples.data(), samples.size(), true, true);
        CHECK_FALSE(decision.IsSafe());
        CHECK(decision.reason ==
              ProductPhysicalHeldInputReason::KeyboardStateReadFailed);
    }

    SECTION("variation between samples blocks")
    {
        Samples samples{};
        samples[1].asyncDown[VK_OEM_PERIOD] = 1;

        const ProductPhysicalHeldInputDecision decision =
            ClassifyProductPhysicalHeldInput(
                samples.data(), samples.size(), true, true);
        CHECK_FALSE(decision.IsSafe());
        CHECK(decision.reason ==
              ProductPhysicalHeldInputReason::StateVaried);
    }

    SECTION("idle and hook evidence remain mandatory")
    {
        Samples samples{};
        CHECK(ClassifyProductPhysicalHeldInput(
                  samples.data(), samples.size(), false, true).reason ==
              ProductPhysicalHeldInputReason::IdleNotQualified);
        CHECK(ClassifyProductPhysicalHeldInput(
                  samples.data(), samples.size(), true, false).reason ==
              ProductPhysicalHeldInputReason::HookInterrupted);
    }
}

// This gate is intentionally hidden from every ordinary test_gui run. It can
// move and click the real desktop pointer, and therefore refuses to start
// unless its dedicated acknowledgement is present. Unlike the raw USER32/OLE
// registration matrix and
// topology probe, this test leaves the production wxWinUIDropBroker installed
// on the real TLW and proves delivery through a real wxTextDropTarget.
TEST_CASE("WinUI OLE broker qualifies physical product delivery",
          "[.winui-product-ole-physical][physical]")
{
    INFO("physical input is disabled unless " <<
         ProductPhysicalProbeArmVariable << "=" <<
         ProductPhysicalProbeArmValue);
    REQUIRE(IsProductPhysicalProbeArmed());
    REQUIRE(IsProductPhysicalInputDesktop());

    POINT cursorBefore{};
    REQUIRE(::GetCursorPos(&cursorBefore));
    const HWND foregroundBefore = ::GetForegroundWindow();
    ProductPhysicalForegroundRestore restoreForeground(foregroundBefore);

    MONITORINFO monitorInfo = { sizeof(monitorInfo) };
    REQUIRE(::GetMonitorInfoW(
        ::MonitorFromPoint(cursorBefore, MONITOR_DEFAULTTONEAREST),
        &monitorInfo));
    const RECT work = monitorInfo.rcWork;
    const int workWidth = work.right - work.left;
    const int workHeight = work.bottom - work.top;
    REQUIRE(workWidth >= 640);
    REQUIRE(workHeight >= 480);

    constexpr int ClientWidth = 560;
    constexpr int ClientHeight = 320;
    const wxPoint framePosition(
        work.left + (workWidth - ClientWidth) / 2,
        work.top + (workHeight - ClientHeight) / 2);
    std::unique_ptr<wxFrame> frame(new wxFrame(
        nullptr, wxID_ANY, "wxWinUI physical product OLE qualification",
        framePosition, wxDefaultSize));
    frame->SetClientSize(ClientWidth, ClientHeight);
    const HWND frameHwnd = GetHwndOf(frame.get());
    REQUIRE(frameHwnd);

    wxPanel* const panel = new wxPanel(
        frame.get(), wxID_ANY, wxPoint(0, 0),
        wxSize(ClientWidth, ClientHeight), wxVSCROLL);
    panel->SetScrollbar(wxVERTICAL, 0, 20, 200, true);
    ProductPhysicalTextTarget* const logicalTarget =
        new ProductPhysicalTextTarget;
    panel->SetDropTarget(logicalTarget);

    wxButton* const button = new wxButton(
        panel, wxID_ANY, "physical XAML drop surface",
        wxPoint(130, 90), wxSize(250, 80));
    wxWinUITopLevelHost* const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host);
    REQUIRE(host->Root());
    const bool rootAllowDrop = host->Root().AllowDrop();
    REQUIRE(rootAllowDrop);
    wxWinUIDropBroker* productBroker =
        host->GetDropBrokerForTest();
    REQUIRE(productBroker);
    const wxWinUIDropBrokerSnapshot hiddenBrokerSnapshot =
        productBroker->GetSnapshotForTest();
    REQUIRE_FALSE(host->OwnsOleDropRegistration());
    REQUIRE_FALSE(hiddenBrokerSnapshot.active);
    REQUIRE_FALSE(hiddenBrokerSnapshot.ownsRegistration);
    REQUIRE(wxMSWOleIsShellDropTargetRegistered(
        logicalTarget, GetHwndOf(panel)));

    frame->Show();
    DrainDispatch();
    panel->SetSize(frame->GetClientRect());
    panel->SetScrollbar(wxVERTICAL, 0, 20, 200, true);
    ::ShowScrollBar(GetHwndOf(panel), SB_VERT, TRUE);
    wxWinUITLWHostNotifyNativeLayout(panel, true);
    host->FlushSync();
    DrainDispatch(8);

    productBroker = host->GetDropBrokerForTest();
    REQUIRE(productBroker);
    const wxWinUIDropBrokerSnapshot brokerSnapshot =
        productBroker->GetSnapshotForTest();
    REQUIRE(host->OwnsOleDropRegistration());
    REQUIRE(brokerSnapshot.active);
    REQUIRE(brokerSnapshot.ownsRegistration);
    REQUIRE(brokerSnapshot.registrationHwnd == frameHwnd);
    REQUIRE_FALSE(wxMSWOleIsShellDropTargetRegistered(
        logicalTarget, GetHwndOf(panel)));
    const bool registeredAfterPresentation =
        !hiddenBrokerSnapshot.active &&
        !hiddenBrokerSnapshot.ownsRegistration &&
        brokerSnapshot.active && brokerSnapshot.ownsRegistration;

    // Keep the newly-created fixture in the topmost band until it is destroyed.
    // Every injection stage still revalidates exact point ownership.
    REQUIRE(::SetWindowPos(
        frameHwnd, HWND_TOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE));
    (void)::SetForegroundWindow(frameHwnd);
    DrainDispatch(4);

    wxWinUISlot* const slot = host->FindSlot(button);
    REQUIRE(slot);
    const wxRect slotRect = slot->GetRectInTLW();
    REQUIRE_FALSE(slotRect.IsEmpty());
    const RECT clientScreen = host->GetClientScreenRect();
    const POINT normalPoint =
    {
        clientScreen.left + slotRect.x + slotRect.width / 2,
        clientScreen.top + slotRect.y + slotRect.height / 2
    };
    POINT normalNearby = normalPoint;
    normalNearby.x += 3;

    SCROLLBARINFO scrollbar = { sizeof(scrollbar) };
    REQUIRE(::GetScrollBarInfo(
        GetHwndOf(panel), OBJID_VSCROLL, &scrollbar));
    REQUIRE((scrollbar.rgstate[0] &
             (STATE_SYSTEM_INVISIBLE | STATE_SYSTEM_OFFSCREEN)) == 0);
    REQUIRE(scrollbar.rcScrollBar.right > scrollbar.rcScrollBar.left);
    REQUIRE(scrollbar.rcScrollBar.bottom > scrollbar.rcScrollBar.top + 8);
    const POINT holePoint =
    {
        (scrollbar.rcScrollBar.left + scrollbar.rcScrollBar.right) / 2,
        (scrollbar.rcScrollBar.top + scrollbar.rcScrollBar.bottom) / 2
    };
    POINT holeNearby = holePoint;
    holeNearby.y += 3;
    if ( holeNearby.y >= scrollbar.rcScrollBar.bottom - 1 )
        holeNearby.y = holePoint.y - 3;

    const HWND bridgeHwnd = host->GetBridgeHwnd();
    REQUIRE(bridgeHwnd);
    const HWND normalPointHwnd = ::WindowFromPoint(normalPoint);
    const HWND normalNearbyHwnd = ::WindowFromPoint(normalNearby);
    REQUIRE(normalPointHwnd);
    REQUIRE((normalPointHwnd == bridgeHwnd ||
             ::IsChild(bridgeHwnd, normalPointHwnd)));
    REQUIRE((normalNearbyHwnd == normalPointHwnd ||
             ::IsChild(bridgeHwnd, normalNearbyHwnd)));

    const HWND holePointHwnd = ::WindowFromPoint(holePoint);
    const HWND holeNearbyHwnd = ::WindowFromPoint(holeNearby);
    REQUIRE(holePointHwnd);
    REQUIRE(holePointHwnd != bridgeHwnd);
    REQUIRE_FALSE(::IsChild(bridgeHwnd, holePointHwnd));
    REQUIRE((holePointHwnd == frameHwnd ||
             ::IsChild(frameHwnd, holePointHwnd)));
    REQUIRE(holeNearbyHwnd == holePointHwnd);

    const ProductPhysicalPointOwner normalOwner{ bridgeHwnd, nullptr };
    const ProductPhysicalPointOwner holeOwner{ frameHwnd, holePointHwnd };

    RECT bridgeRect{};
    REQUIRE(::GetWindowRect(bridgeHwnd, &bridgeRect));
    ProductPhysicalSourceWindow sourceWindow;
    REQUIRE(sourceWindow.Create(bridgeRect));
    REQUIRE(sourceWindow.Handle());
    REQUIRE(::WindowFromPoint(sourceWindow.GetPoint()) ==
            sourceWindow.Handle());

    // Freeze the exact dual-registration evidence after all fixture layout
    // and immediately before installing the input guard. No partial or stale
    // set can reach even the harmless tagged heartbeat, let alone a drag.
    const wxWinUIDropBrokerSnapshot preInjectionBrokerSnapshot =
        productBroker->GetSnapshotForTest();
    const bool bridgeRegistration =
        preInjectionBrokerSnapshot.bridgeRegistration.ownsRegistration &&
        preInjectionBrokerSnapshot.bridgeRegistration.locked &&
        preInjectionBrokerSnapshot.bridgeRegistration.registrationHwnd ==
            bridgeHwnd;
    const bool tlwRegistration =
        preInjectionBrokerSnapshot.tlwRegistration.ownsRegistration &&
        preInjectionBrokerSnapshot.tlwRegistration.locked &&
        preInjectionBrokerSnapshot.tlwRegistration.registrationHwnd ==
            frameHwnd;
    const bool completeRegistrationSet =
        preInjectionBrokerSnapshot.ownsCompleteRegistrationSet;
    const bool bridgeRegistrationCurrent =
        preInjectionBrokerSnapshot.bridgeRegistration.current;
    const bool tlwRegistrationCurrent =
        preInjectionBrokerSnapshot.tlwRegistration.current;
    const std::uint64_t bridgeRegistrationGeneration =
        preInjectionBrokerSnapshot.bridgeRegistration.generation;
    const std::uint64_t tlwRegistrationGeneration =
        preInjectionBrokerSnapshot.tlwRegistration.generation;
    REQUIRE(preInjectionBrokerSnapshot.active);
    REQUIRE(bridgeRegistration);
    REQUIRE(tlwRegistration);
    REQUIRE(completeRegistrationSet);
    REQUIRE(bridgeRegistrationCurrent);
    REQUIRE(tlwRegistrationCurrent);
    REQUIRE(bridgeRegistrationGeneration != 0);
    REQUIRE(tlwRegistrationGeneration != 0);
    REQUIRE(preInjectionBrokerSnapshot.bridgeRegistration.registrationHwnd !=
            preInjectionBrokerSnapshot.tlwRegistration.registrationHwnd);

    ProductPhysicalDragResult normal;
    ProductPhysicalDragResult hole;
    bool guardClean = false;
    bool cursorRestored = false;
    bool hooksRemovedCleanly = false;
    bool taggedInputObserved = false;
    bool finalHeartbeatObserved = false;
    bool registrationSetRevalidated = false;
    unsigned taggedMouseEvents = 0;
    {
        ProductPhysicalInputGuard inputGuard;
        REQUIRE(inputGuard.Install());

        DWORD idleMs = 0;
        const bool idle =
            WaitForProductPhysicalInputIdle(30000, &idleMs);
        INFO("physical desktop must remain untouched for 8 seconds; "
             "last idle sample was " << idleMs << " ms");
        REQUIRE((idle && !inputGuard.WasInterrupted()));
        REQUIRE(IsProductPhysicalInputDesktop());

        std::array<ProductPhysicalHeldInputSample,
                   ProductPhysicalHeldInputSampleCount> heldInputSamples;
        bool idleRemainedQualified = true;
        for ( std::size_t sample = 0;
              sample < heldInputSamples.size(); ++sample )
        {
            heldInputSamples[sample] =
                CaptureProductPhysicalHeldInputSample();
            DWORD sampledIdleMs = 0;
            if ( !GetProductPhysicalInputIdleMs(&sampledIdleMs) ||
                 sampledIdleMs < ProductPhysicalProbeMinimumIdleMs )
            {
                idleRemainedQualified = false;
            }
            if ( sample + 1 != heldInputSamples.size() )
            {
                wxYield();
                wxMilliSleep(50);
            }
        }
        const ProductPhysicalHeldInputDecision heldInputDecision =
            ClassifyProductPhysicalHeldInput(
                heldInputSamples.data(), heldInputSamples.size(),
                idleRemainedQualified,
                !inputGuard.WasInterrupted());
        std::printf(
            "wx_winui_product_ole_delivery: {\"event\":"
            "\"held-input-guard\",\"result\":\"%s\","
            "\"reason\":\"%s\",\"blocking_virtual_key\":%d,"
            "\"async_only_virtual_key\":%d,\"samples\":%llu}\n",
            heldInputDecision.IsSafe() ? "passed" : "failed",
            ProductPhysicalHeldInputReasonName(heldInputDecision.reason),
            heldInputDecision.blockingVirtualKey,
            heldInputDecision.firstAsyncOnlyVirtualKey,
            static_cast<unsigned long long>(heldInputSamples.size()));
        std::fflush(stdout);
        INFO("held-input guard reason was " <<
             ProductPhysicalHeldInputReasonName(heldInputDecision.reason) <<
             "; blocking virtual key was " <<
             heldInputDecision.blockingVirtualKey <<
             "; first stable async-only virtual key was " <<
             heldInputDecision.firstAsyncOnlyVirtualKey);
        REQUIRE(heldInputDecision.IsSafe());

        const wxWinUIDropBrokerSnapshot injectionBrokerSnapshot =
            productBroker->GetSnapshotForTest();
        registrationSetRevalidated =
            injectionBrokerSnapshot.active &&
            injectionBrokerSnapshot.ownsCompleteRegistrationSet &&
            injectionBrokerSnapshot.bridgeRegistration.locked &&
            injectionBrokerSnapshot.bridgeRegistration.ownsRegistration &&
            injectionBrokerSnapshot.bridgeRegistration.current &&
            injectionBrokerSnapshot.bridgeRegistration.registrationHwnd ==
                bridgeHwnd &&
            injectionBrokerSnapshot.bridgeRegistration.generation ==
                bridgeRegistrationGeneration &&
            injectionBrokerSnapshot.tlwRegistration.locked &&
            injectionBrokerSnapshot.tlwRegistration.ownsRegistration &&
            injectionBrokerSnapshot.tlwRegistration.current &&
            injectionBrokerSnapshot.tlwRegistration.registrationHwnd ==
                frameHwnd &&
            injectionBrokerSnapshot.tlwRegistration.generation ==
                tlwRegistrationGeneration;
        INFO("the complete bridge+TLW registration set must remain current "
             "through the final pre-injection boundary");
        REQUIRE(registrationSetRevalidated);

        // Prove that the safety hook is alive before the first button-down.
        // This tagged move is to the cursor's current location and cannot
        // click or target another application.
        POINT heartbeatPoint{};
        REQUIRE(::GetCursorPos(&heartbeatPoint));
        const unsigned heartbeatBefore =
            inputGuard.TaggedMouseEventCount();
        REQUIRE(InjectProductPhysicalPoint(heartbeatPoint));
        const bool heartbeatObserved = PumpProductPhysicalUntil(
            [&]()
            {
                return inputGuard.TaggedMouseEventCount() > heartbeatBefore;
            },
            1000);
        INFO("the tagged-input safety hook must acknowledge its heartbeat");
        REQUIRE((heartbeatObserved && !inputGuard.WasInterrupted()));
        const unsigned heartbeatCount =
            inputGuard.TaggedMouseEventCount();

        const wxWinUIDropBrokerSnapshot normalBrokerBefore =
            productBroker->GetSnapshotForTest();
        normal = RunProductPhysicalDrag(
            sourceWindow.Handle(), sourceWindow.GetPoint(),
            normalPoint, normalNearby, "product-normal-surface",
            logicalTarget, inputGuard, normalOwner);
        const wxWinUIDropBrokerSnapshot normalBrokerAfter =
            productBroker->GetSnapshotForTest();
        PrintProductPhysicalDragResult(
            "normal", normal, normalBrokerBefore, normalBrokerAfter);

        wxWinUIDropBrokerSnapshot holeBrokerBefore = normalBrokerAfter;
        wxWinUIDropBrokerSnapshot holeBrokerAfter = normalBrokerAfter;
        if ( !inputGuard.WasInterrupted() &&
             normal.Delivered() && normal.buttonUpObserved )
        {
            holeBrokerBefore = productBroker->GetSnapshotForTest();
            hole = RunProductPhysicalDrag(
                sourceWindow.Handle(), sourceWindow.GetPoint(),
                holePoint, holeNearby, "product-native-hole",
                logicalTarget, inputGuard, holeOwner);
            holeBrokerAfter = productBroker->GetSnapshotForTest();
        }
        PrintProductPhysicalDragResult(
            "hole", hole, holeBrokerBefore, holeBrokerAfter);

        const bool cleanBeforeRestore = !inputGuard.WasInterrupted();
        POINT currentCursor{};
        const bool currentCursorAvailable =
            ::GetCursorPos(&currentCursor);
        const bool cursorAlreadyRestored =
            currentCursorAvailable &&
            ProductPhysicalPointsAreClose(currentCursor, cursorBefore);
        const bool cursorOnProbeOwnedSurface =
            currentCursorAvailable &&
            ((ProductPhysicalPointsAreClose(
                  currentCursor, sourceWindow.GetPoint()) &&
              ::WindowFromPoint(currentCursor) == sourceWindow.Handle()) ||
             ((ProductPhysicalPointsAreClose(currentCursor, normalPoint) ||
               ProductPhysicalPointsAreClose(currentCursor, normalNearby)) &&
              normalOwner.Owns(currentCursor)) ||
             ((ProductPhysicalPointsAreClose(currentCursor, holePoint) ||
               ProductPhysicalPointsAreClose(currentCursor, holeNearby)) &&
              holeOwner.Owns(currentCursor)));
        if ( cleanBeforeRestore && cursorOnProbeOwnedSurface &&
             IsProductPhysicalInputDesktop() )
        {
            const unsigned finalHeartbeatBefore =
                inputGuard.TaggedMouseEventCount();
            finalHeartbeatObserved =
                InjectProductPhysicalPoint(currentCursor) &&
                PumpProductPhysicalUntil(
                    [&]()
                    {
                        return inputGuard.TaggedMouseEventCount() >
                               finalHeartbeatBefore;
                    },
                    1000) &&
                !inputGuard.WasInterrupted();
            if ( !inputGuard.WasInterrupted() )
            {
                cursorRestored = InjectProductPhysicalPoint(cursorBefore) &&
                    PumpProductPhysicalUntil(
                        [&]()
                        {
                            POINT current{};
                            return ::GetCursorPos(&current) &&
                                ProductPhysicalPointsAreClose(
                                    current, cursorBefore);
                        },
                        1000);
            }
        }
        else if ( cleanBeforeRestore && cursorAlreadyRestored )
        {
            cursorRestored = true;
        }
        taggedMouseEvents = inputGuard.TaggedMouseEventCount();
        taggedInputObserved =
            finalHeartbeatObserved &&
            taggedMouseEvents >= heartbeatCount + 11;
        hooksRemovedCleanly = inputGuard.Stop();
        guardClean = !inputGuard.WasInterrupted() &&
                     hooksRemovedCleanly;
        inputGuard.PrintStatus();
    }

    POINT expectedNormal = normalNearby;
    ::SetLastError(ERROR_SUCCESS);
    const int normalMapped = ::MapWindowPoints(
        HWND_DESKTOP, GetHwndOf(panel), &expectedNormal, 1);
    REQUIRE((normalMapped != 0 || ::GetLastError() == ERROR_SUCCESS));
    POINT expectedHole = holeNearby;
    ::SetLastError(ERROR_SUCCESS);
    const int holeMapped = ::MapWindowPoints(
        HWND_DESKTOP, GetHwndOf(panel), &expectedHole, 1);
    REQUIRE((holeMapped != 0 || ::GetLastError() == ERROR_SUCCESS));

    const bool normalSemantic =
        normal.Delivered() && normal.queryContinueCount > 0 &&
        normal.overDelta > 0 && normal.lastOverDefault == wxDragCopy &&
        normal.text == "product-normal-surface" &&
        std::abs(normal.lastX - expectedNormal.x) <= 4 &&
        std::abs(normal.lastY - expectedNormal.y) <= 4;
    const bool holeSemantic =
        hole.Delivered() && hole.queryContinueCount > 0 &&
        hole.overDelta > 0 && hole.lastOverDefault == wxDragCopy &&
        hole.text == "product-native-hole" &&
        std::abs(hole.lastX - expectedHole.x) <= 4 &&
        std::abs(hole.lastY - expectedHole.y) <= 4;
    const bool qualified =
        normalSemantic && holeSemantic && guardClean &&
        taggedInputObserved && cursorRestored && bridgeRegistration &&
        tlwRegistration && completeRegistrationSet &&
        bridgeRegistrationCurrent && tlwRegistrationCurrent &&
        registrationSetRevalidated && registeredAfterPresentation;

    std::printf(
        "wx_winui_product_ole_delivery: {\"event\":\"summary\","
        "\"result\":\"%s\",\"registration\":\"bridge+tlw\","
        "\"registered_after_presentation\":%s,"
        "\"root_allow_drop\":%s,\"bridge_registration\":%s,"
        "\"tlw_registration\":%s,"
        "\"complete_registration_set\":%s,"
        "\"registration_set_revalidated\":%s,"
        "\"bridge_registration_current\":%s,"
        "\"tlw_registration_current\":%s,"
        "\"bridge_registration_generation\":%llu,"
        "\"tlw_registration_generation\":%llu,"
        "\"normal_delivered\":%s,\"hole_delivered\":%s,"
        "\"normal_semantic\":%s,\"hole_semantic\":%s,"
        "\"input_guard_clean\":%s,\"hooks_removed\":%s,"
        "\"tagged_input_observed\":%s,\"final_heartbeat\":%s,"
        "\"tagged_mouse_events\":%u,"
        "\"cursor_restored\":%s}\n",
        qualified ? "passed" : "failed",
        registeredAfterPresentation ? "true" : "false",
        rootAllowDrop ? "true" : "false",
        bridgeRegistration ? "true" : "false",
        tlwRegistration ? "true" : "false",
        completeRegistrationSet ? "true" : "false",
        registrationSetRevalidated ? "true" : "false",
        bridgeRegistrationCurrent ? "true" : "false",
        tlwRegistrationCurrent ? "true" : "false",
        static_cast<unsigned long long>(bridgeRegistrationGeneration),
        static_cast<unsigned long long>(tlwRegistrationGeneration),
        normal.Delivered() ? "true" : "false",
        hole.Delivered() ? "true" : "false",
        normalSemantic ? "true" : "false",
        holeSemantic ? "true" : "false",
        guardClean ? "true" : "false",
        hooksRemovedCleanly ? "true" : "false",
        taggedInputObserved ? "true" : "false",
        finalHeartbeatObserved ? "true" : "false",
        taggedMouseEvents,
        cursorRestored ? "true" : "false");
    std::fflush(stdout);

    CHECK(guardClean);
    CHECK(hooksRemovedCleanly);
    CHECK(taggedInputObserved);
    CHECK(cursorRestored);
    REQUIRE(normalSemantic);
    REQUIRE(holeSemantic);
}

TEST_CASE("WinUI OLE broker owns exact bridge and TLW adapter registrations",
          "[winui-drop-broker]")
{
    NativeLog log;
    std::unique_ptr<wxWinUIDropBroker> broker = MakeBroker(log);
    REQUIRE(broker);

    const wxWinUIDropBrokerInitResult init = broker->Initialize();
    REQUIRE(init.IsReady());
    CHECK(init.hresult == S_OK);
    const wxWinUIDropBrokerInitResult repeated = broker->Initialize();
    CHECK(repeated.status == init.status);
    CHECK(repeated.hresult == init.hresult);

    wxWinUIDropBrokerSnapshot snapshot = broker->GetSnapshotForTest();
    CHECK(snapshot.active);
    CHECK(snapshot.locked);
    CHECK(snapshot.ownsRegistration);
    CHECK(snapshot.ownsCompleteRegistrationSet);
    CHECK(snapshot.registrationHwnd == log.GetTestHwnd());
    CHECK(log.registeredHwnd == snapshot.registrationHwnd);
    CHECK(snapshot.bridgeRegistration.registrationHwnd ==
          log.GetBridgeHwnd());
    CHECK(snapshot.tlwRegistration.registrationHwnd ==
          log.GetTestHwnd());
    CHECK(snapshot.bridgeRegistration.locked);
    CHECK(snapshot.tlwRegistration.locked);
    CHECK(snapshot.bridgeRegistration.ownsRegistration);
    CHECK(snapshot.tlwRegistration.ownsRegistration);
    CHECK(snapshot.bridgeRegistration.current);
    CHECK(snapshot.tlwRegistration.current);
    CHECK(snapshot.bridgeRegistration.generation != 0);
    CHECK(snapshot.tlwRegistration.generation != 0);
    CHECK(snapshot.bridgeRegistration.lockCalls == 1);
    CHECK(snapshot.bridgeRegistration.registerCalls == 1);
    CHECK(snapshot.tlwRegistration.lockCalls == 1);
    CHECK(snapshot.tlwRegistration.registerCalls == 1);
    CHECK(snapshot.lockCalls == 2);
    CHECK(snapshot.registerCalls == 2);
    CHECK(snapshot.revokeCalls == 0);
    CHECK(snapshot.unlockCalls == 0);
    CHECK((log.registeredHwnds == std::vector<HWND>{
        log.GetBridgeHwnd(), log.GetTestHwnd() }));
    REQUIRE(log.lockedTargets.size() == 2);
    REQUIRE(log.registeredTargets.size() == 2);
    CHECK(log.registeredTargets[0] != log.registeredTargets[1]);
    CHECK(log.lockedTargets[0] ==
          static_cast<IUnknown*>(log.registeredTargets[0]));
    CHECK(log.lockedTargets[1] ==
          static_cast<IUnknown*>(log.registeredTargets[1]));
    void* bridgeUnknown = nullptr;
    void* tlwUnknown = nullptr;
    REQUIRE(log.registeredTargets[0]->QueryInterface(
                IID_IUnknown, &bridgeUnknown) == S_OK);
    REQUIRE(log.registeredTargets[1]->QueryInterface(
                IID_IUnknown, &tlwUnknown) == S_OK);
    REQUIRE(bridgeUnknown);
    REQUIRE(tlwUnknown);
    CHECK(bridgeUnknown != tlwUnknown);
    static_cast<IUnknown*>(tlwUnknown)->Release();
    static_cast<IUnknown*>(bridgeUnknown)->Release();

    IDropTarget* const external = broker->GetCOMTargetForTest();
    REQUIRE(external);
    CHECK(external == log.registeredTargets[0]);
    CHECK(external != log.registeredTargets[1]);
    external->AddRef();

    void* queried = nullptr;
    CHECK(external->QueryInterface(IID_IDropTarget, &queried) == S_OK);
    REQUIRE(queried);
    static_cast<IDropTarget*>(queried)->Release();
    CHECK(external->QueryInterface(IID_IDataObject, &queried) ==
          E_NOINTERFACE);
    CHECK(queried == nullptr);
    CHECK(external->QueryInterface(IID_IUnknown, nullptr) == E_POINTER);

    broker->Shutdown();
    broker->Shutdown();
    snapshot = broker->GetSnapshotForTest();
    CHECK_FALSE(snapshot.active);
    CHECK(snapshot.shutdown);
    CHECK_FALSE(snapshot.locked);
    CHECK_FALSE(snapshot.ownsRegistration);
    CHECK_FALSE(snapshot.ownsCompleteRegistrationSet);
    CHECK_FALSE(snapshot.bridgeRegistration.locked);
    CHECK_FALSE(snapshot.tlwRegistration.locked);
    CHECK_FALSE(snapshot.bridgeRegistration.ownsRegistration);
    CHECK_FALSE(snapshot.tlwRegistration.ownsRegistration);
    CHECK(snapshot.bridgeRegistration.revokeCalls == 1);
    CHECK(snapshot.bridgeRegistration.unlockCalls == 1);
    CHECK(snapshot.tlwRegistration.revokeCalls == 1);
    CHECK(snapshot.tlwRegistration.unlockCalls == 1);
    CHECK(snapshot.revokeCalls == 2);
    CHECK(snapshot.unlockCalls == 2);
    CHECK((log.revokedHwnds == std::vector<HWND>{
        log.GetTestHwnd(), log.GetBridgeHwnd() }));
    REQUIRE(log.unlockedTargets.size() == 2);
    CHECK(log.unlockedTargets[0] ==
          static_cast<IUnknown*>(log.registeredTargets[1]));
    CHECK(log.unlockedTargets[1] ==
          static_cast<IUnknown*>(log.registeredTargets[0]));
    const wxWinUIDropBrokerInitResult afterShutdown = broker->Initialize();
    CHECK(afterShutdown.status ==
          wxWinUIDropBrokerInitStatus::InvalidContext);
    CHECK(FAILED(afterShutdown.hresult));

    const std::vector<std::string> expected =
    {
        "lock(T,F)", "register", "lock(T,F)", "register",
        "revoke", "unlock(F,T)", "revoke", "unlock(F,T)"
    };
    CHECK(log.calls == expected);

    broker.reset();

    wxTextDataObject source("late callback");
    DWORD effect = DROPEFFECT_COPY;
    POINTL point = { 10, 10 };
    CHECK(external->DragEnter(source.GetInterface(), 0, point, &effect) ==
          S_OK);
    CHECK(effect == DROPEFFECT_NONE);
    external->Release();
}

TEST_CASE("WinUI OLE broker production queries are passive",
          "[winui-drop-broker][winui-broker-observation]")
{
    NativeLog log;
    auto broker = MakeBroker(log);
    REQUIRE(broker);
    CHECK_FALSE(broker->HasNativeOwnership());
    CHECK_FALSE(broker->IsReady());
    REQUIRE(broker->Initialize().IsReady());
    CHECK(broker->HasNativeOwnership());
    CHECK(broker->IsReady());

    bool expectedOwnership = true;
    bool expectedReady = false;
    SECTION("ready pair")
    {
        expectedReady = true;
    }
    SECTION("clean shutdown")
    {
        broker->Shutdown();
        expectedOwnership = false;
    }
    SECTION("failed revoke retains registration only")
    {
        log.revokeResult = E_FAIL;
        broker->Shutdown();
        REQUIRE(broker->GetSnapshotForTest().ownsRegistration);
        REQUIRE_FALSE(broker->GetSnapshotForTest().locked);
    }
    SECTION("failed unlock retains lock only")
    {
        log.unlockResult = E_ACCESSDENIED;
        broker->Shutdown();
        REQUIRE_FALSE(broker->GetSnapshotForTest().ownsRegistration);
        REQUIRE(broker->GetSnapshotForTest().locked);
    }

    const auto before = broker->GetSnapshotForTest();
    const auto callsBefore = log.calls;
    const auto mutationBefore = wxMSWOleGetDropTargetMutationGeneration();
    const auto schedulesBefore = wxWinUITopLevelHost::GetFlushScheduleCount();
    const auto runsBefore = wxWinUITopLevelHost::GetFlushRunCount();
    const auto attemptsBefore =
        wxWinUITopLevelHost::GetFlushCallbackAttemptCount();
    for ( unsigned i = 0; i < 32; ++i )
    {
        CHECK(broker->HasNativeOwnership() == expectedOwnership);
        CHECK(broker->IsReady() == expectedReady);
    }

    // In particular, observing a failed native cleanup must never retry it.
    // The production queries do not consume the diagnostic snapshot, either.
    const auto after = broker->GetSnapshotForTest();
    CHECK(log.calls == callsBefore);
    CHECK(wxMSWOleGetDropTargetMutationGeneration() == mutationBefore);
    CHECK(wxWinUITopLevelHost::GetFlushScheduleCount() == schedulesBefore);
    CHECK(wxWinUITopLevelHost::GetFlushRunCount() == runsBefore);
    CHECK(wxWinUITopLevelHost::GetFlushCallbackAttemptCount() == attemptsBefore);
    CHECK(after.status == before.status);
    CHECK(after.active == before.active);
    CHECK(after.ownsRegistration == before.ownsRegistration);
    CHECK(after.locked == before.locked);
    CHECK(after.lockCalls == before.lockCalls);
    CHECK(after.registerCalls == before.registerCalls);
    CHECK(after.revokeCalls == before.revokeCalls);
    CHECK(after.unlockCalls == before.unlockCalls);
    CHECK(after.dragEnterCalls == before.dragEnterCalls);
    CHECK(after.dragOverCalls == before.dragOverCalls);
    CHECK(after.dragLeaveCalls == before.dragLeaveCalls);
    CHECK(after.dropCalls == before.dropCalls);
}

TEST_CASE("WinUI OLE broker failure paths never revoke foreign state",
          "[winui-drop-broker]")
{
    SECTION("lock failure stops before registration")
    {
        NativeLog log;
        log.lockResult = E_ACCESSDENIED;
        auto broker = MakeBroker(log);
        REQUIRE(broker);
        const auto result = broker->Initialize();
        CHECK(result.status == wxWinUIDropBrokerInitStatus::LockFailed);
        broker->Shutdown();
        CHECK(log.calls == std::vector<std::string>{ "lock(T,F)" });
    }

    SECTION("second lock failure rolls the exact bridge back")
    {
        NativeLog log;
        log.lockResults = { S_OK, E_ACCESSDENIED };
        auto broker = MakeBroker(log);
        REQUIRE(broker);

        const wxWinUIDropBrokerInitResult result = broker->Initialize();
        CHECK(result.status == wxWinUIDropBrokerInitStatus::LockFailed);
        CHECK(result.hresult == E_ACCESSDENIED);

        const wxWinUIDropBrokerSnapshot snapshot =
            broker->GetSnapshotForTest();
        CHECK(snapshot.shutdown);
        CHECK_FALSE(snapshot.active);
        CHECK_FALSE(snapshot.locked);
        CHECK_FALSE(snapshot.ownsRegistration);
        CHECK_FALSE(snapshot.ownsCompleteRegistrationSet);
        CHECK(snapshot.bridgeRegistration.lockCalls == 1);
        CHECK(snapshot.bridgeRegistration.registerCalls == 1);
        CHECK(snapshot.bridgeRegistration.revokeCalls == 1);
        CHECK(snapshot.bridgeRegistration.unlockCalls == 1);
        CHECK(snapshot.tlwRegistration.lockCalls == 1);
        CHECK(snapshot.tlwRegistration.registerCalls == 0);
        CHECK(snapshot.tlwRegistration.revokeCalls == 0);
        CHECK(snapshot.tlwRegistration.unlockCalls == 0);
        CHECK(log.registeredHwnds ==
              std::vector<HWND>{ log.GetBridgeHwnd() });
        CHECK(log.revokedHwnds ==
              std::vector<HWND>{ log.GetBridgeHwnd() });
        CHECK((log.calls == std::vector<std::string>{
            "lock(T,F)", "register", "lock(T,F)",
            "revoke", "unlock(F,T)" }));

        broker->Shutdown();
        CHECK(log.revokedHwnds.size() == 1);
    }

    SECTION("already registered unlocks but never revokes")
    {
        NativeLog log;
        log.registerResult = DRAGDROP_E_ALREADYREGISTERED;
        auto broker = MakeBroker(log);
        REQUIRE(broker);
        const auto result = broker->Initialize();
        CHECK(result.status ==
              wxWinUIDropBrokerInitStatus::AlreadyRegistered);
        broker->Shutdown();
        const std::vector<std::string> expected =
        {
            "lock(T,F)", "register", "unlock(F,T)"
        };
        CHECK(log.calls == expected);
        CHECK(broker->GetSnapshotForTest().revokeCalls == 0);
    }

    SECTION("generic register failure has the same ownership discipline")
    {
        NativeLog log;
        log.registerResult = E_FAIL;
        auto broker = MakeBroker(log);
        REQUIRE(broker);
        const auto result = broker->Initialize();
        CHECK(result.status == wxWinUIDropBrokerInitStatus::RegisterFailed);
        broker->Shutdown();
        const std::vector<std::string> expected =
        {
            "lock(T,F)", "register", "unlock(F,T)"
        };
        CHECK(log.calls == expected);
    }

    SECTION("second registration failure rolls back TLW then bridge")
    {
        NativeLog log;
        log.registerResults = { S_OK, E_FAIL };
        auto broker = MakeBroker(log);
        REQUIRE(broker);

        const wxWinUIDropBrokerInitResult result = broker->Initialize();
        CHECK(result.status == wxWinUIDropBrokerInitStatus::RegisterFailed);
        const wxWinUIDropBrokerSnapshot snapshot =
            broker->GetSnapshotForTest();
        CHECK_FALSE(snapshot.active);
        CHECK_FALSE(snapshot.ownsCompleteRegistrationSet);
        CHECK_FALSE(snapshot.bridgeRegistration.locked);
        CHECK_FALSE(snapshot.bridgeRegistration.ownsRegistration);
        CHECK(snapshot.bridgeRegistration.lockCalls == 1);
        CHECK(snapshot.bridgeRegistration.registerCalls == 1);
        CHECK(snapshot.bridgeRegistration.revokeCalls == 1);
        CHECK(snapshot.bridgeRegistration.unlockCalls == 1);
        CHECK_FALSE(snapshot.tlwRegistration.locked);
        CHECK_FALSE(snapshot.tlwRegistration.ownsRegistration);
        CHECK(snapshot.tlwRegistration.lockCalls == 1);
        CHECK(snapshot.tlwRegistration.registerCalls == 1);
        CHECK(snapshot.tlwRegistration.revokeCalls == 0);
        CHECK(snapshot.tlwRegistration.unlockCalls == 1);
        CHECK((log.registeredHwnds == std::vector<HWND>{
            log.GetBridgeHwnd(), log.GetTestHwnd() }));
        CHECK(log.revokedHwnds ==
              std::vector<HWND>{ log.GetBridgeHwnd() });
        CHECK((log.calls == std::vector<std::string>{
            "lock(T,F)", "register", "lock(T,F)", "register",
            "unlock(F,T)", "revoke", "unlock(F,T)" }));

        broker->Shutdown();
        CHECK((log.calls == std::vector<std::string>{
            "lock(T,F)", "register", "lock(T,F)", "register",
            "unlock(F,T)", "revoke", "unlock(F,T)" }));
    }

    SECTION("revoke failure retains registration but balances its lock")
    {
        NativeLog log;
        log.revokeResult = E_FAIL;
        auto broker = MakeBroker(log);
        REQUIRE(broker->Initialize().IsReady());

        IDropTarget* const external = broker->GetCOMTargetForTest();
        external->AddRef();
        broker->Shutdown();
        const wxWinUIDropBrokerSnapshot snapshot =
            broker->GetSnapshotForTest();
        CHECK(snapshot.shutdown);
        CHECK_FALSE(snapshot.locked);
        CHECK(snapshot.ownsRegistration);
        CHECK(snapshot.revokeCalls == 2);
        CHECK(snapshot.unlockCalls == 2);
        broker.reset();

        const std::vector<std::string> expected =
        {
            "lock(T,F)", "register", "lock(T,F)", "register",
            "revoke", "unlock(F,T)", "revoke", "unlock(F,T)"
        };
        CHECK(log.calls == expected);

        DWORD effect = DROPEFFECT_MOVE;
        POINTL point = { 0, 0 };
        CHECK(external->DragOver(0, point, &effect) == S_OK);
        CHECK(effect == DROPEFFECT_NONE);
        external->Release();
    }
}

TEST_CASE("WinUI OLE broker detects bridge churn and skips stale revoke",
          "[winui-drop-broker][shell-ledger][transaction]")
{
    NativeLog log;
    wxWindow* const tlw = log.EnsureTestWindow();
    REQUIRE(tlw);
    const HWND tlwHwnd = GetHwndOf(tlw);
    REQUIRE(tlwHwnd);

    RoutedTextTarget* const routed = new RoutedTextTarget;
    tlw->SetDropTarget(routed);
    REQUIRE(wxMSWOleIsShellDropTargetRegistered(routed, tlwHwnd));

    auto broker = MakeBroker(log);
    REQUIRE(broker);
    REQUIRE(broker->InitializeAndAdoptShellDropTargets().IsReady());
    REQUIRE(broker->CoversShellDropTargets());
    REQUIRE(wxWinUITLWHostOwnsOleDropRegistration(tlw));
    REQUIRE_FALSE(wxMSWOleIsShellDropTargetRegistered(routed, tlwHwnd));

    const wxWinUIDropBrokerSnapshot before =
        broker->GetSnapshotForTest();
    REQUIRE(before.bridgeRegistration.current);
    REQUIRE(before.tlwRegistration.current);
    const HWND staleBridge = before.bridgeRegistration.registrationHwnd;
    const std::uint64_t staleGeneration =
        before.bridgeRegistration.generation;
    REQUIRE(staleBridge);
    REQUIRE(staleGeneration != 0);

    log.DestroyTestBridge();
    DrainDispatch(4);
    REQUIRE_FALSE(::IsWindow(staleBridge));

    const wxWinUIDropBrokerSnapshot afterDestroy =
        broker->GetSnapshotForTest();
    CHECK(afterDestroy.bridgeRegistration.registrationHwnd == staleBridge);
    CHECK(afterDestroy.bridgeRegistration.generation == staleGeneration);
    CHECK_FALSE(afterDestroy.bridgeRegistration.current);
    CHECK(afterDestroy.tlwRegistration.current);
    CHECK_FALSE(broker->IsReady());

    // Give USER32 one ordinary replacement-window opportunity. If the numeric
    // HWND is recycled immediately, the captured generation must still reject
    // the ABA identity. The stale-revoke assertion below is unconditional.
    wxFrame* const replacement =
        new wxFrame(nullptr, wxID_ANY, "ole-broker-bridge-replacement",
                    wxPoint(-24500, -24500), wxSize(32, 32));
    const HWND replacementHwnd = GetHwndOf(replacement);
    REQUIRE(replacementHwnd);
    const std::uint64_t replacementGeneration =
        wxWinUIMSWGetNativeHwndGeneration(
            reinterpret_cast<WXHWND>(replacementHwnd));
    REQUIRE(replacementGeneration != 0);
    if ( replacementHwnd == staleBridge )
        CHECK(replacementGeneration != staleGeneration);

    const wxWinUIDropBrokerSnapshot afterReplacement =
        broker->GetSnapshotForTest();
    CHECK_FALSE(afterReplacement.bridgeRegistration.current);
    CHECK(afterReplacement.tlwRegistration.current);

    broker->Shutdown();
    const wxWinUIDropBrokerSnapshot afterShutdown =
        broker->GetSnapshotForTest();
    CHECK(afterShutdown.shutdown);
    CHECK_FALSE(afterShutdown.active);
    CHECK(afterShutdown.bridgeRegistration.revokeCalls == 0);
    CHECK(afterShutdown.bridgeRegistration.revokeHr == E_HANDLE);
    CHECK_FALSE(afterShutdown.bridgeRegistration.ownsRegistration);
    CHECK(afterShutdown.tlwRegistration.revokeCalls == 1);
    CHECK_FALSE(afterShutdown.tlwRegistration.ownsRegistration);
    CHECK_FALSE(afterShutdown.ownsRegistration);
    CHECK_FALSE(afterShutdown.ownsCompleteRegistrationSet);
    CHECK(log.revokedHwnds ==
          std::vector<HWND>{ afterShutdown.tlwRegistration.registrationHwnd });
    CHECK(std::find(log.revokedHwnds.begin(), log.revokedHwnds.end(),
                    staleBridge) == log.revokedHwnds.end());
    CHECK_FALSE(broker->CoversShellDropTargets());
    CHECK_FALSE(wxWinUITLWHostOwnsOleDropRegistration(tlw));
    CHECK(wxMSWOleIsShellDropTargetRegistered(routed, tlwHwnd));

    broker.reset();
    CHECK_FALSE(wxWinUITLWHostOwnsOleDropRegistration(tlw));
    CHECK(wxMSWOleIsShellDropTargetRegistered(routed, tlwHwnd));
    delete replacement;
    DrainDispatch(4);
}

TEST_CASE("WinUI OLE broker revalidates bridge churn pumped from revoke",
          "[winui-drop-broker][shell-ledger][transaction][reentrancy]")
{
    NativeLog log;
    bool bridgeDestroyed = false;
    SECTION("failed bridge revoke destroys its HWND")
    {
        bridgeDestroyed = true;
        log.destroyBridgeOnRevoke = true;
    }
    SECTION("failed bridge revoke observes its exact retire latch")
    {
        log.retireBridgeOnRevoke = true;
    }
    log.revokeResults = { S_OK, E_FAIL };

    wxWindow* const tlw = log.EnsureTestWindow();
    REQUIRE(tlw);
    const HWND tlwHwnd = GetHwndOf(tlw);
    REQUIRE(tlwHwnd);
    RoutedTextTarget* const routed = new RoutedTextTarget;
    tlw->SetDropTarget(routed);
    REQUIRE(wxMSWOleIsShellDropTargetRegistered(routed, tlwHwnd));

    auto broker = MakeBroker(log);
    REQUIRE(broker);
    REQUIRE(broker->InitializeAndAdoptShellDropTargets().IsReady());
    REQUIRE(broker->CoversShellDropTargets());
    REQUIRE(wxWinUITLWHostOwnsOleDropRegistration(tlw));
    REQUIRE_FALSE(wxMSWOleIsShellDropTargetRegistered(routed, tlwHwnd));

    const wxWinUIDropBrokerSnapshot before =
        broker->GetSnapshotForTest();
    const HWND bridgeHwnd = before.bridgeRegistration.registrationHwnd;
    REQUIRE(bridgeHwnd);
    REQUIRE(before.bridgeRegistration.current);
    REQUIRE(before.bridgeRegistration.ownsRegistration);

    broker->Shutdown();
    const wxWinUIDropBrokerSnapshot after =
        broker->GetSnapshotForTest();
    CHECK(after.shutdown);
    CHECK_FALSE(after.active);
    CHECK(after.tlwRegistration.revokeCalls == 1);
    CHECK(after.tlwRegistration.revokeHr == S_OK);
    CHECK_FALSE(after.tlwRegistration.ownsRegistration);
    CHECK(after.bridgeRegistration.revokeCalls == 1);
    CHECK(after.bridgeRegistration.revokeHr == E_FAIL);
    CHECK_FALSE(after.bridgeRegistration.ownsRegistration);
    CHECK_FALSE(after.ownsRegistration);
    CHECK_FALSE(after.ownsCompleteRegistrationSet);
    CHECK((log.revokedHwnds ==
           std::vector<HWND>{ tlwHwnd, bridgeHwnd }));
    CHECK(std::count(log.revokedHwnds.begin(), log.revokedHwnds.end(),
                     bridgeHwnd) == 1);

    if ( bridgeDestroyed )
    {
        CHECK_FALSE(::IsWindow(bridgeHwnd));
        CHECK_FALSE(after.bridgeRegistration.current);
    }
    else
    {
        // The numeric HWND and generation are still live: the retire latch,
        // not an IsWindow() failure, is what terminated physical ownership.
        CHECK(::IsWindow(bridgeHwnd));
        CHECK(after.bridgeRegistration.current);
    }

    CHECK_FALSE(broker->CoversShellDropTargets());
    CHECK_FALSE(wxWinUITLWHostOwnsOleDropRegistration(tlw));
    CHECK(wxMSWOleIsShellDropTargetRegistered(routed, tlwHwnd));

    broker.reset();
    CHECK_FALSE(wxWinUITLWHostOwnsOleDropRegistration(tlw));
    CHECK(wxMSWOleIsShellDropTargetRegistered(routed, tlwHwnd));
    DrainDispatch(4);
}

TEST_CASE("WinUI OLE broker oscillates exact dual PendingFixed keys",
          "[winui-drop-broker][shell-ledger][transaction][pending]")
{
    NativeLog log;
    wxWindow* const tlw = log.EnsureTestWindow();
    REQUIRE(tlw);
    const HWND tlwHwnd = GetHwndOf(tlw);
    REQUIRE(tlwHwnd);
    const HWND bridgeHwnd = RequireDistinctBridgeHwnd(log, tlwHwnd);

    RoutedTextTarget bridgeBlocker;
    RoutedTextTarget tlwBlocker;
    IDropTarget* const bridgeBlockerCom =
        wxMSWOleDropTargetAccess::GetCOMInterface(&bridgeBlocker);
    IDropTarget* const tlwBlockerCom =
        wxMSWOleDropTargetAccess::GetCOMInterface(&tlwBlocker);
    REQUIRE(bridgeBlockerCom);
    REQUIRE(tlwBlockerCom);

    wxMSWOleShellDropTargetOperation fixedBridge =
        wxMSWOleBeginShellDropTargetOperation(
            &bridgeBlocker, reinterpret_cast<WXHWND>(bridgeHwnd),
            wxMSWOleShellDropTargetOperationKind::Register,
            bridgeBlockerCom);
    wxMSWOleShellDropTargetOperation fixedTLW =
        wxMSWOleBeginShellDropTargetOperation(
            &tlwBlocker, reinterpret_cast<WXHWND>(tlwHwnd),
            wxMSWOleShellDropTargetOperationKind::Register,
            tlwBlockerCom);
    REQUIRE(fixedBridge.IsActive());
    REQUIRE(fixedTLW.IsActive());
    REQUIRE(wxMSWOleArmShellDropTargetOperation(&fixedBridge));
    REQUIRE(wxMSWOleArmShellDropTargetOperation(&fixedTLW));

    const std::weak_ptr<wxWinUIHostLifetime> lifetime;
    const wxWinUIDropBrokerNativeOps ops = log.Ops();
    auto broker = wxWinUIDropBroker::Create(
        tlw, bridgeHwnd, lifetime, &ops);
    REQUIRE(broker);

    const wxWinUIDropBrokerInitResult bridgePending =
        broker->Initialize();
    CHECK(bridgePending.status ==
          wxWinUIDropBrokerInitStatus::PendingRegistration);
    CHECK(bridgePending.hresult == E_PENDING);
    CHECK(broker->IsInitializationPending());
    CHECK(broker->CoversShellDropTargets());
    CHECK(log.calls.empty());

    REQUIRE(wxMSWOleCancelShellDropTargetOperation(&fixedBridge));
    const wxWinUIDropBrokerInitResult tlwPending = broker->Initialize();
    CHECK(tlwPending.status ==
          wxWinUIDropBrokerInitStatus::PendingRegistration);
    CHECK(tlwPending.hresult == E_PENDING);
    CHECK(broker->IsInitializationPending());
    CHECK(broker->CoversShellDropTargets());
    CHECK(log.calls.empty());

    REQUIRE(wxMSWOleCancelShellDropTargetOperation(&fixedTLW));
    const wxWinUIDropBrokerInitResult ready = broker->Initialize();
    REQUIRE(ready.IsReady());
    CHECK_FALSE(broker->IsInitializationPending());
    CHECK(broker->IsReady());
    CHECK((log.registeredHwnds ==
           std::vector<HWND>{ bridgeHwnd, tlwHwnd }));
    CHECK((log.calls == std::vector<std::string>{
        "lock(T,F)", "register", "lock(T,F)", "register" }));

    broker->Shutdown();
    CHECK_FALSE(broker->CoversShellDropTargets());
    CHECK((log.revokedHwnds ==
           std::vector<HWND>{ tlwHwnd, bridgeHwnd }));
}

TEST_CASE("WinUI OLE broker retains ambiguous external unlocks exactly",
          "[winui-drop-broker][shell-ledger]")
{
    SECTION("register rollback unlock failure is one-shot and blocks reuse")
    {
        NativeLog log;
        log.registerResult = E_FAIL;
        log.unlockResult = E_ACCESSDENIED;
        auto broker = MakeBroker(log);
        REQUIRE(broker);

        const wxWinUIDropBrokerInitResult result = broker->Initialize();
        CHECK(result.status == wxWinUIDropBrokerInitStatus::RegisterFailed);
        wxWinUIDropBrokerSnapshot snapshot = broker->GetSnapshotForTest();
        CHECK(snapshot.locked);
        CHECK_FALSE(snapshot.ownsRegistration);
        CHECK(snapshot.unlockCalls == 1);

        // Shutdown must not guess that a failed FALSE had no side effects and
        // issue it a second time. The exact retained identity blocks a new
        // broker on this HWND generation instead.
        broker->Shutdown();
        snapshot = broker->GetSnapshotForTest();
        CHECK(snapshot.unlockCalls == 1);
        broker.reset();
        CHECK(log.calls == std::vector<std::string>{
            "lock(T,F)", "register", "unlock(F,T)" });

        log.calls.clear();
        log.registerResult = S_OK;
        log.unlockResult = S_OK;
        auto successor = MakeBroker(log);
        REQUIRE(successor);
        CHECK(successor->Initialize().status ==
              wxWinUIDropBrokerInitStatus::AlreadyRegistered);
        CHECK(log.calls.empty());
        successor->Shutdown();
    }

    SECTION("successful revoke with failed final unlock retains identity")
    {
        NativeLog log;
        auto broker = MakeBroker(log);
        REQUIRE(broker);
        REQUIRE(broker->Initialize().IsReady());
        log.unlockResult = E_ACCESSDENIED;

        broker->Shutdown();
        const wxWinUIDropBrokerSnapshot snapshot =
            broker->GetSnapshotForTest();
        CHECK(snapshot.locked);
        CHECK_FALSE(snapshot.ownsRegistration);
        CHECK(snapshot.revokeCalls == 2);
        CHECK(snapshot.unlockCalls == 2);
        broker.reset();
        CHECK((log.calls == std::vector<std::string>{
            "lock(T,F)", "register", "lock(T,F)", "register",
            "revoke", "unlock(F,T)", "revoke", "unlock(F,T)" }));

        log.calls.clear();
        log.unlockResult = S_OK;
        auto successor = MakeBroker(log);
        REQUIRE(successor);
        CHECK(successor->Initialize().status ==
              wxWinUIDropBrokerInitStatus::AlreadyRegistered);
        CHECK(log.calls.empty());
        successor->Shutdown();
    }
}

TEST_CASE("WinUI OLE broker coverage is scoped to its exact owner",
          "[winui-drop-broker][transaction][coverage]")
{
    wxFrame* const frame =
        new wxFrame(nullptr, wxID_ANY, "ole-broker-coverage-owner",
                    wxPoint(-24000, -24000), wxSize(320, 180));
    const HWND frameHwnd = GetHwndOf(frame);
    REQUIRE(frameHwnd);

    const std::weak_ptr<wxWinUIHostLifetime> lifetime;
    NativeLog ownerLog;
    const HWND bridgeHwnd =
        RequireDistinctBridgeHwnd(ownerLog, frameHwnd);
    const wxWinUIDropBrokerNativeOps ownerOps = ownerLog.Ops();
    auto owner = wxWinUIDropBroker::Create(
        frame, bridgeHwnd, lifetime, &ownerOps);
    REQUIRE(owner);
    REQUIRE(owner->Initialize().IsReady());
    REQUIRE(owner->CoversShellDropTargets());
    REQUIRE(wxWinUITLWHostOwnsOleDropRegistration(frame));

    // A retry publishes its own provisional coverage for the same exact TLW
    // generation. Its failed RegisterDragDrop() must withdraw only that token,
    // never the incumbent broker's coverage.
    NativeLog retryLog;
    retryLog.registerResult = DRAGDROP_E_ALREADYREGISTERED;
    const wxWinUIDropBrokerNativeOps retryOps = retryLog.Ops();
    auto retry = wxWinUIDropBroker::Create(
        frame, bridgeHwnd, lifetime, &retryOps);
    REQUIRE(retry);
    const wxWinUIDropBrokerInitResult retryResult = retry->Initialize();
    CHECK(retryResult.status ==
          wxWinUIDropBrokerInitStatus::AlreadyRegistered);
    CHECK_FALSE(retry->CoversShellDropTargets());
    CHECK(wxWinUITLWHostOwnsOleDropRegistration(frame));

    retry->Shutdown();
    owner->Shutdown();
    CHECK_FALSE(owner->CoversShellDropTargets());
    CHECK_FALSE(wxWinUITLWHostOwnsOleDropRegistration(frame));

    retry.reset();
    owner.reset();
    delete frame;
    DrainDispatch();
}

TEST_CASE("WinUI OLE broker retry preserves a retained coverage tombstone",
          "[winui-drop-broker][transaction][coverage]")
{
    wxFrame* const frame =
        new wxFrame(nullptr, wxID_ANY, "ole-broker-coverage-tombstone",
                    wxPoint(-24000, -24000), wxSize(320, 180));
    const HWND frameHwnd = GetHwndOf(frame);
    REQUIRE(frameHwnd);

    const std::weak_ptr<wxWinUIHostLifetime> lifetime;
    NativeLog ownerLog;
    ownerLog.revokeResult = E_FAIL;
    const HWND bridgeHwnd =
        RequireDistinctBridgeHwnd(ownerLog, frameHwnd);
    const wxWinUIDropBrokerNativeOps ownerOps = ownerLog.Ops();
    auto owner = wxWinUIDropBroker::Create(
        frame, bridgeHwnd, lifetime, &ownerOps);
    REQUIRE(owner);
    REQUIRE(owner->Initialize().IsReady());
    REQUIRE(wxWinUITLWHostOwnsOleDropRegistration(frame));

    // A failed exact revoke deliberately outlives the logical broker object:
    // its owner token becomes the only evidence that this TLW generation is
    // still occupied by the inactive, fail-closed COM receiver.
    owner->Shutdown();
    const wxWinUIDropBrokerSnapshot retained = owner->GetSnapshotForTest();
    REQUIRE(retained.shutdown);
    REQUIRE(retained.ownsRegistration);
    REQUIRE(retained.revokeCalls == 2);
    REQUIRE(retained.bridgeRegistration.ownsRegistration);
    REQUIRE(retained.tlwRegistration.ownsRegistration);
    REQUIRE(retained.bridgeRegistration.revokeCalls == 1);
    REQUIRE(retained.tlwRegistration.revokeCalls == 1);
    REQUIRE(owner->CoversShellDropTargets());
    owner.reset();
    REQUIRE(wxWinUITLWHostOwnsOleDropRegistration(frame));

    // A later broker publishes a distinct provisional token. Its native
    // retry cannot acquire the occupied HWND and withdrawing that token must
    // leave the ownerless retained tombstone untouched.
    NativeLog retryLog;
    retryLog.registerResult = DRAGDROP_E_ALREADYREGISTERED;
    const wxWinUIDropBrokerNativeOps retryOps = retryLog.Ops();
    auto retry = wxWinUIDropBroker::Create(
        frame, bridgeHwnd, lifetime, &retryOps);
    REQUIRE(retry);
    const wxWinUIDropBrokerInitResult retryResult = retry->Initialize();
    CHECK(retryResult.status ==
          wxWinUIDropBrokerInitStatus::AlreadyRegistered);
    CHECK_FALSE(retry->CoversShellDropTargets());
    CHECK(wxWinUITLWHostOwnsOleDropRegistration(frame));

    retry->Shutdown();
    retry.reset();
    CHECK(wxWinUITLWHostOwnsOleDropRegistration(frame));

    delete frame;
    DrainDispatch();
}

TEST_CASE("WinUI OLE broker acquisition is terminal under native reentrancy",
          "[winui-drop-broker][reentrancy][transaction]")
{
    SECTION("shutdown during external lock rolls the acquired lock back once")
    {
        NativeLog log;
        auto broker = MakeBroker(log);
        REQUIRE(broker);
        log.reentrantBroker = broker.get();
        log.shutdownOnLock = true;

        const wxWinUIDropBrokerInitResult result = broker->Initialize();
        CHECK(result.status == wxWinUIDropBrokerInitStatus::InvalidContext);
        CHECK(result.hresult == HRESULT_FROM_WIN32(ERROR_INVALID_STATE));

        const wxWinUIDropBrokerSnapshot snapshot =
            broker->GetSnapshotForTest();
        CHECK(snapshot.shutdown);
        CHECK_FALSE(snapshot.active);
        CHECK_FALSE(snapshot.locked);
        CHECK_FALSE(snapshot.ownsRegistration);
        CHECK(snapshot.lockCalls == 1);
        CHECK(snapshot.registerCalls == 0);
        CHECK(snapshot.revokeCalls == 0);
        CHECK(snapshot.unlockCalls == 1);
        CHECK(log.calls == std::vector<std::string>{
            "lock(T,F)", "unlock(F,T)" });
        log.reentrantBroker = nullptr;
    }

    SECTION("shutdown during registration revokes the exact acquired target")
    {
        NativeLog log;
        auto broker = MakeBroker(log);
        REQUIRE(broker);
        log.reentrantBroker = broker.get();
        log.shutdownOnRegister = true;

        const wxWinUIDropBrokerInitResult result = broker->Initialize();
        CHECK(result.status == wxWinUIDropBrokerInitStatus::InvalidContext);
        CHECK(result.hresult == HRESULT_FROM_WIN32(ERROR_INVALID_STATE));

        const wxWinUIDropBrokerSnapshot snapshot =
            broker->GetSnapshotForTest();
        CHECK(snapshot.shutdown);
        CHECK_FALSE(snapshot.active);
        CHECK_FALSE(snapshot.locked);
        CHECK_FALSE(snapshot.ownsRegistration);
        CHECK(snapshot.lockCalls == 1);
        CHECK(snapshot.registerCalls == 1);
        CHECK(snapshot.revokeCalls == 1);
        CHECK(snapshot.unlockCalls == 1);
        CHECK(snapshot.bridgeRegistration.registerCalls == 1);
        CHECK(snapshot.bridgeRegistration.revokeCalls == 1);
        CHECK(snapshot.tlwRegistration.registerCalls == 0);
        CHECK(snapshot.tlwRegistration.revokeCalls == 0);
        CHECK(log.registeredHwnd ==
              snapshot.bridgeRegistration.registrationHwnd);
        CHECK(log.revokedHwnd ==
              snapshot.bridgeRegistration.registrationHwnd);
        CHECK(log.calls == std::vector<std::string>{
            "lock(T,F)", "register", "revoke", "unlock(F,T)" });
        log.reentrantBroker = nullptr;
    }

    SECTION("shutdown cleanup remains single-owner when revoke and unlock pump")
    {
        NativeLog log;
        auto broker = MakeBroker(log);
        REQUIRE(broker);
        REQUIRE(broker->Initialize().IsReady());
        log.reentrantBroker = broker.get();
        log.shutdownOnRevoke = true;
        log.shutdownOnUnlock = true;

        broker->Shutdown();
        broker->Shutdown();

        const wxWinUIDropBrokerSnapshot snapshot =
            broker->GetSnapshotForTest();
        CHECK(snapshot.shutdown);
        CHECK_FALSE(snapshot.active);
        CHECK_FALSE(snapshot.locked);
        CHECK_FALSE(snapshot.ownsRegistration);
        CHECK(snapshot.revokeCalls == 2);
        CHECK(snapshot.unlockCalls == 2);
        CHECK(snapshot.bridgeRegistration.revokeCalls == 1);
        CHECK(snapshot.bridgeRegistration.unlockCalls == 1);
        CHECK(snapshot.tlwRegistration.revokeCalls == 1);
        CHECK(snapshot.tlwRegistration.unlockCalls == 1);
        CHECK((log.registeredHwnds == std::vector<HWND>{
            snapshot.bridgeRegistration.registrationHwnd,
            snapshot.tlwRegistration.registrationHwnd }));
        CHECK((log.revokedHwnds == std::vector<HWND>{
            snapshot.tlwRegistration.registrationHwnd,
            snapshot.bridgeRegistration.registrationHwnd }));
        CHECK((log.calls == std::vector<std::string>{
            "lock(T,F)", "register", "lock(T,F)", "register",
            "revoke", "unlock(F,T)", "revoke", "unlock(F,T)" }));
        log.reentrantBroker = nullptr;
    }
}

TEST_CASE("WinUI OLE broker rejects active cross-thread COM dispatch",
          "[winui-drop-broker]")
{
    NativeLog log;
    auto broker = MakeBroker(log);
    REQUIRE(broker->Initialize().IsReady());

    IDropTarget* const target = broker->GetCOMTargetForTest();
    target->AddRef();
    HRESULT hr = S_OK;
    DWORD effect = DROPEFFECT_COPY;
    std::thread worker(
        [&]()
        {
            POINTL point = { 0, 0 };
            hr = target->DragOver(0, point, &effect);
        });
    worker.join();

    CHECK(hr == RPC_E_WRONG_THREAD);
    CHECK(effect == DROPEFFECT_NONE);
    target->Release();
}

TEST_CASE("WinUI OLE broker restores detached targets on acquire failure",
          "[winui-drop-broker][transaction]")
{
    wxFrame* const frame =
        new wxFrame(nullptr, wxID_ANY, "ole-broker-acquire-rollback",
                    wxPoint(-24000, -24000), wxSize(320, 180));
    const HWND frameHwnd = GetHwndOf(frame);
    REQUIRE(frameHwnd);

    RoutedTextTarget* const routed = new RoutedTextTarget;
    frame->SetDropTarget(routed);
    REQUIRE(wxMSWOleIsShellDropTargetRegistered(routed, frameHwnd));

    NativeLog log;
    log.registerResult = E_FAIL;
    const HWND bridgeHwnd = RequireDistinctBridgeHwnd(log, frameHwnd);
    const std::weak_ptr<wxWinUIHostLifetime> lifetime;
    const wxWinUIDropBrokerNativeOps ops = log.Ops();
    auto broker = wxWinUIDropBroker::Create(
        frame, bridgeHwnd, lifetime, &ops);
    REQUIRE(broker);

    const wxWinUIDropBrokerInitResult result =
        broker->InitializeAndAdoptShellDropTargets();
    CHECK(result.status == wxWinUIDropBrokerInitStatus::RegisterFailed);
    CHECK(result.hresult == E_FAIL);
    CHECK(wxMSWOleIsShellDropTargetRegistered(routed, frameHwnd));
    CHECK(log.registeredHwnd == bridgeHwnd);
    CHECK(log.revokedHwnd == nullptr);
    CHECK(log.calls == std::vector<std::string>{
        "lock(T,F)", "register", "unlock(F,T)" });

    broker->Shutdown();
    delete frame;
    DrainDispatch();
}

TEST_CASE("WinUI OLE broker restores an uncommitted Ready adoption",
          "[winui-drop-broker][transaction][host-initialization]")
{
    wxFrame* const frame =
        new wxFrame(nullptr, wxID_ANY, "ole-broker-uncommitted-ready",
                    wxPoint(-24000, -24000), wxSize(320, 180));
    const HWND frameHwnd = GetHwndOf(frame);
    REQUIRE(frameHwnd);

    RoutedTextTarget* const routed = new RoutedTextTarget;
    frame->SetDropTarget(routed);
    REQUIRE(wxMSWOleIsShellDropTargetRegistered(routed, frameHwnd));

    NativeLog log;
    const HWND bridgeHwnd = RequireDistinctBridgeHwnd(log, frameHwnd);
    const std::weak_ptr<wxWinUIHostLifetime> lifetime;
    const wxWinUIDropBrokerNativeOps ops = log.Ops();
    auto broker = wxWinUIDropBroker::Create(
        frame, bridgeHwnd, lifetime, &ops);
    REQUIRE(broker);

    REQUIRE(broker->InitializeAndAdoptShellDropTargets().IsReady());
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(routed, frameHwnd));
    CHECK(broker->GetSnapshotForTest().ownsCompleteRegistrationSet);

    // No CommitHostInitialization(): this is the exact rollback latch used
    // when the presentation epoch changes after Ready but before host commit.
    broker->Shutdown();
    const wxWinUIDropBrokerSnapshot stopped = broker->GetSnapshotForTest();
    CHECK(stopped.shutdown);
    CHECK_FALSE(stopped.ownsRegistration);
    CHECK_FALSE(stopped.ownsCompleteRegistrationSet);
    CHECK_FALSE(stopped.locked);
    CHECK(wxMSWOleIsShellDropTargetRegistered(routed, frameHwnd));
    CHECK((log.revokedHwnds == std::vector<HWND>{
        frameHwnd, bridgeHwnd }));

    broker.reset();
    delete frame;
    DrainDispatch();
}

TEST_CASE("WinUI OLE broker never restores beside a retained bridge registration",
          "[winui-drop-broker][transaction][reentrancy]")
{
    wxFrame* const frame =
        new wxFrame(nullptr, wxID_ANY, "ole-broker-terminal-revoke-failure",
                    wxPoint(-24000, -24000), wxSize(320, 180));
    const HWND frameHwnd = GetHwndOf(frame);
    REQUIRE(frameHwnd);

    RoutedTextTarget* const routed = new RoutedTextTarget;
    frame->SetDropTarget(routed);
    REQUIRE(wxMSWOleIsShellDropTargetRegistered(routed, frameHwnd));

    NativeLog log;
    log.revokeResult = E_FAIL;
    const HWND bridgeHwnd = RequireDistinctBridgeHwnd(log, frameHwnd);
    const std::weak_ptr<wxWinUIHostLifetime> lifetime;
    const wxWinUIDropBrokerNativeOps ops = log.Ops();
    auto broker = wxWinUIDropBroker::Create(
        frame, bridgeHwnd, lifetime, &ops);
    REQUIRE(broker);
    log.reentrantBroker = broker.get();
    log.shutdownOnRegister = true;

    const wxWinUIDropBrokerInitResult result =
        broker->InitializeAndAdoptShellDropTargets();
    CHECK(result.status == wxWinUIDropBrokerInitStatus::InvalidContext);
    const wxWinUIDropBrokerSnapshot snapshot =
        broker->GetSnapshotForTest();
    CHECK(snapshot.shutdown);
    CHECK_FALSE(snapshot.active);
    CHECK_FALSE(snapshot.locked);
    CHECK(snapshot.ownsRegistration);
    CHECK(snapshot.revokeCalls == 1);
    CHECK(snapshot.unlockCalls == 1);
    CHECK(snapshot.bridgeRegistration.ownsRegistration);
    CHECK(snapshot.bridgeRegistration.registrationHwnd == bridgeHwnd);
    CHECK_FALSE(snapshot.tlwRegistration.ownsRegistration);
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(routed, frameHwnd));
    CHECK(log.calls == std::vector<std::string>{
        "lock(T,F)", "register", "revoke", "unlock(F,T)" });

    // The host can already be logically terminal here. The generation-keyed
    // native coverage tombstone must still suppress a brand-new target beside
    // the retained bridge registration.
    RoutedTextTarget* const replacement = new RoutedTextTarget;
    frame->SetDropTarget(replacement);
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(
        replacement, frameHwnd));

    log.reentrantBroker = nullptr;
    broker.reset();
    delete frame;
    DrainDispatch();
}

TEST_CASE("WinUI OLE broker acquires an incumbent TLW drop target",
          "[winui-drop-broker][transaction]")
{
    wxFrame* const frame =
        new wxFrame(nullptr, wxID_ANY, "ole-broker-tlw-incumbent",
                    wxPoint(-24000, -24000), wxSize(420, 220));
    const HWND frameHwnd = GetHwndOf(frame);
    REQUIRE(frameHwnd);

    RoutedTextTarget* const routed = new RoutedTextTarget;
    frame->SetDropTarget(routed);
    REQUIRE(wxMSWOleIsShellDropTargetRegistered(routed, frameHwnd));

    wxButton* const button =
        new wxButton(frame, wxID_ANY, "slot",
                     wxPoint(40, 40), wxSize(180, 70));
    wxWinUITopLevelHost* const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host);
    REQUIRE_FALSE(host->OwnsOleDropRegistration());
    const wxWinUIDropBrokerSnapshot hiddenSnapshot =
        host->GetDropBrokerForTest()->GetSnapshotForTest();
    CHECK_FALSE(hiddenSnapshot.active);
    CHECK_FALSE(hiddenSnapshot.ownsRegistration);
    CHECK(wxMSWOleIsShellDropTargetRegistered(routed, frameHwnd));

    frame->Show();
    DrainDispatch();
    host->FlushSync();
    DrainDispatch(4);

    REQUIRE(host->OwnsOleDropRegistration());
    const wxWinUIDropBrokerSnapshot snapshot =
        host->GetDropBrokerForTest()->GetSnapshotForTest();
    CHECK(snapshot.active);
    CHECK(snapshot.registrationHwnd == frameHwnd);
    CHECK(snapshot.registrationHwnd != host->GetBridgeHwnd());
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(routed, frameHwnd));

    wxWinUISlot* const slot = host->FindSlot(button);
    REQUIRE(slot);
    const wxRect slotRect = slot->GetRectInTLW();
    REQUIRE_FALSE(slotRect.IsEmpty());
    const RECT clientScreen = host->GetClientScreenRect();
    POINTL point =
    {
        clientScreen.left + slotRect.x + slotRect.width / 2,
        clientScreen.top + slotRect.y + slotRect.height / 2
    };

    IDropTarget* const target =
        host->GetDropBrokerForTest()->GetCOMTargetForTest();
    REQUIRE(target);
    target->AddRef();
    wxTextDataObject source("TLW incumbent route");
    DWORD effect = DROPEFFECT_COPY;
    REQUIRE(target->DragEnter(
        source.GetInterface(), 0, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_COPY);
    REQUIRE(target->Drop(
        source.GetInterface(), 0, point, &effect) == S_OK);
    CHECK(routed->drops == 1);
    CHECK(routed->text == "TLW incumbent route");
    target->Release();

    delete frame;
    DrainDispatch();
}

TEST_CASE("WinUI OLE broker acquires once after presentation",
          "[winui-drop-broker][presentation]")
{
    wxFrame* const frame =
        new wxFrame(nullptr, wxID_ANY, "ole-broker-presentation-once",
                    wxPoint(-24000, -24000), wxSize(360, 190));
    wxPanel* const panel =
        new wxPanel(frame, wxID_ANY, wxPoint(0, 0), wxSize(340, 170));
    RoutedTextTarget* const routed = new RoutedTextTarget;
    panel->SetDropTarget(routed);
    const HWND panelHwnd = GetHwndOf(panel);
    REQUIRE(panelHwnd);
    REQUIRE(wxMSWOleIsShellDropTargetRegistered(routed, panelHwnd));

    wxButton* const slot =
        new wxButton(panel, wxID_ANY, "slot",
                     wxPoint(40, 40), wxSize(180, 70));
    wxWinUITopLevelHost* const host =
        wxWinUITopLevelHost::FindSlotOwner(slot);
    REQUIRE(host);
    wxWinUIDropBroker* const hiddenBroker =
        host->GetDropBrokerForTest();
    REQUIRE(hiddenBroker);
    CHECK_FALSE(hiddenBroker->IsReady());
    CHECK_FALSE(host->OwnsOleDropRegistration());
    CHECK(wxMSWOleIsShellDropTargetRegistered(routed, panelHwnd));

    frame->Show();
    DrainDispatch();
    host->FlushSync();
    DrainDispatch(4);

    wxWinUIDropBroker* const visibleBroker =
        host->GetDropBrokerForTest();
    REQUIRE(visibleBroker == hiddenBroker);
    IDropTarget* const visibleTarget =
        visibleBroker->GetCOMTargetForTest();
    REQUIRE(visibleTarget);
    visibleTarget->AddRef();
    const wxWinUIDropBrokerSnapshot firstVisible =
        visibleBroker->GetSnapshotForTest();
    CHECK(firstVisible.active);
    CHECK(firstVisible.registerCalls == 2);
    CHECK(firstVisible.revokeCalls == 0);
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(routed, panelHwnd));

    frame->Hide();
    DrainDispatch();
    frame->Show();
    DrainDispatch();
    host->FlushSync();
    DrainDispatch(4);

    wxWinUIDropBroker* const shownAgain =
        host->GetDropBrokerForTest();
    REQUIRE(shownAgain == visibleBroker);
    CHECK(shownAgain->GetCOMTargetForTest() == visibleTarget);
    const wxWinUIDropBrokerSnapshot secondVisible =
        shownAgain->GetSnapshotForTest();
    CHECK(secondVisible.active);
    CHECK(secondVisible.registerCalls == 2);
    CHECK(secondVisible.revokeCalls == 0);
    CHECK(secondVisible.ownsCompleteRegistrationSet);

    visibleTarget->Release();
    delete frame;
    DrainDispatch();
}

TEST_CASE("WinUI OLE broker defers acquisition for an already visible TLW",
          "[winui-drop-broker][presentation]")
{
    wxFrame* const frame =
        new wxFrame(nullptr, wxID_ANY, "ole-broker-visible-construction",
                    wxPoint(-24000, -24000), wxSize(360, 190));
    wxPanel* const panel =
        new wxPanel(frame, wxID_ANY, wxPoint(0, 0), wxSize(340, 170));
    RoutedTextTarget* const routed = new RoutedTextTarget;
    panel->SetDropTarget(routed);
    const HWND panelHwnd = GetHwndOf(panel);
    REQUIRE(panelHwnd);
    REQUIRE(wxMSWOleIsShellDropTargetRegistered(routed, panelHwnd));

    frame->Show();
    DrainDispatch();

    wxButton* const slot =
        new wxButton(panel, wxID_ANY, "late slot",
                     wxPoint(40, 40), wxSize(180, 70));
    wxWinUITopLevelHost* const host =
        wxWinUITopLevelHost::FindSlotOwner(slot);
    REQUIRE(host);
    wxWinUIDropBroker* const broker = host->GetDropBrokerForTest();
    REQUIRE(broker);

    // Visibility alone is insufficient: the construction stack must never
    // acquire native OLE ownership before its first completed layout flush.
    CHECK_FALSE(broker->IsReady());
    CHECK_FALSE(host->OwnsOleDropRegistration());
    CHECK(wxMSWOleIsShellDropTargetRegistered(routed, panelHwnd));

    host->FlushSync();
    DrainDispatch(4);
    CHECK(broker->IsReady());
    CHECK(host->OwnsOleDropRegistration());
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(routed, panelHwnd));

    delete frame;
    DrainDispatch();
}

TEST_CASE("WinUI OLE broker retries a reentrant presentation invalidation",
          "[winui-drop-broker][presentation][reentrancy]")
{
    wxFrame* const frame =
        new wxFrame(nullptr, wxID_ANY, "ole-broker-presentation-epoch",
                    wxPoint(-24000, -24000), wxSize(360, 190));
    wxPanel* const panel =
        new wxPanel(frame, wxID_ANY, wxPoint(0, 0), wxSize(340, 170));
    RoutedTextTarget* const routed = new RoutedTextTarget;
    panel->SetDropTarget(routed);
    const HWND panelHwnd = GetHwndOf(panel);
    REQUIRE(panelHwnd);
    REQUIRE(wxMSWOleIsShellDropTargetRegistered(routed, panelHwnd));

    wxButton* const slot =
        new wxButton(panel, wxID_ANY, "epoch slot",
                     wxPoint(40, 40), wxSize(180, 70));
    wxWinUITopLevelHost* const host =
        wxWinUITopLevelHost::FindSlotOwner(slot);
    REQUIRE(host);
    wxWinUIDropBroker* const firstBroker = host->GetDropBrokerForTest();
    REQUIRE(firstBroker);
    IDropTarget* const firstTarget = firstBroker->GetCOMTargetForTest();
    REQUIRE(firstTarget);
    firstTarget->AddRef();

    frame->Show();
    {
        PresentationInvalidationHookGuard invalidate(frame);
        host->FlushSync();
        REQUIRE(invalidate.WasObserved());

        wxWinUIDropBroker* const retryBroker = host->GetDropBrokerForTest();
        REQUIRE(retryBroker);
        CHECK(retryBroker->GetCOMTargetForTest() != firstTarget);
        CHECK_FALSE(retryBroker->IsReady());
        CHECK_FALSE(retryBroker->GetSnapshotForTest().ownsRegistration);
        CHECK(wxMSWOleIsShellDropTargetRegistered(routed, panelHwnd));
    }

    host->FlushSync();
    DrainDispatch(4);
    wxWinUIDropBroker* const readyBroker = host->GetDropBrokerForTest();
    REQUIRE(readyBroker);
    CHECK(readyBroker->IsReady());
    CHECK(readyBroker->GetSnapshotForTest().ownsCompleteRegistrationSet);
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(routed, panelHwnd));

    firstTarget->Release();
    delete frame;
    DrainDispatch();
}

TEST_CASE("WinUI OLE broker audits a manual registration race before Ready",
          "[winui-drop-broker][transaction][reentrancy]")
{
    wxFrame* const frame =
        new wxFrame(nullptr, wxID_ANY, "ole-broker-manual-register-race",
                    wxPoint(-24000, -24000), wxSize(360, 190));
    wxPanel* const panel =
        new wxPanel(frame, wxID_ANY, wxPoint(20, 20), wxSize(240, 110));
    const HWND panelHwnd = GetHwndOf(panel);
    REQUIRE(panelHwnd);

    RoutedTextTarget* const routed = new RoutedTextTarget;
    panel->SetDropTarget(routed);
    REQUIRE(wxMSWOleIsShellDropTargetRegistered(routed, panelHwnd));

    NativeLog log;
    log.registerShellOnRegister = true;
    log.shellTargetToRegister = routed;
    log.shellHwndToRegister = panelHwnd;
    const HWND frameHwnd = GetHwndOf(frame);
    REQUIRE(frameHwnd);
    const HWND bridgeHwnd = RequireDistinctBridgeHwnd(log, frameHwnd);
    const std::weak_ptr<wxWinUIHostLifetime> lifetime;
    const wxWinUIDropBrokerNativeOps ops = log.Ops();
    auto broker = wxWinUIDropBroker::Create(
        frame, bridgeHwnd, lifetime, &ops);
    REQUIRE(broker);

    const wxWinUIDropBrokerInitResult result =
        broker->InitializeAndAdoptShellDropTargets();
    REQUIRE(result.IsReady());
    CHECK(log.shellRegisterSucceeded);
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(routed, panelHwnd));
    const wxWinUIDropBrokerSnapshot snapshot =
        broker->GetSnapshotForTest();
    CHECK(snapshot.active);
    CHECK(snapshot.ownsRegistration);
    CHECK(snapshot.ownsCompleteRegistrationSet);
    CHECK(snapshot.locked);

    broker->CommitHostInitialization();
    broker->Shutdown();
    CHECK((log.registeredHwnds == std::vector<HWND>{
        bridgeHwnd, frameHwnd }));
    CHECK((log.revokedHwnds == std::vector<HWND>{
        frameHwnd, bridgeHwnd }));
    CHECK((log.calls == std::vector<std::string>{
        "lock(T,F)", "register", "lock(T,F)", "register",
        "revoke", "unlock(F,T)", "revoke", "unlock(F,T)" }));
    broker.reset();
    delete frame;
    DrainDispatch();
}

TEST_CASE("WinUI OLE pending broker publishes XAML content before continuation",
          "[winui-drop-broker][transaction][host-initialization][pending]")
{
    DrainDispatch();

    wxFrame* const frame =
        new wxFrame(nullptr, wxID_ANY, "ole-broker-pending-host",
                    wxPoint(-24000, -24000), wxSize(420, 220));
    const HWND frameHwnd = GetHwndOf(frame);
    REQUIRE(frameHwnd);

    RoutedTextTarget blocker;
    IDropTarget* const blockerCom =
        wxMSWOleDropTargetAccess::GetCOMInterface(&blocker);
    REQUIRE(blockerCom);

    DeferredInitializationHookGuard initializationHook(frame);
    wxButton* button = nullptr;
    wxWinUITopLevelHost* host = nullptr;
    wxWinUISlot* slot = nullptr;
    winrt::Microsoft::UI::Xaml::UIElement content{ nullptr };

    {
        wxMSWOleShellDropTargetOperation fixed =
            wxMSWOleBeginShellDropTargetOperation(
                &blocker,
                reinterpret_cast<WXHWND>(frameHwnd),
                wxMSWOleShellDropTargetOperationKind::Register,
                blockerCom);
        REQUIRE(fixed.IsActive());
        REQUIRE(wxMSWOleArmShellDropTargetOperation(&fixed));

        button = new wxButton;
        REQUIRE(button->Create(frame, wxID_ANY, "pending slot",
                               wxPoint(20, 20), wxSize(180, 70)));

        host = wxWinUITopLevelHost::FindSlotOwner(button);
        REQUIRE(host);
        CHECK(wxWinUITopLevelHost::FindForTLW(frame) == host);

        wxWinUIDropBroker* const pendingBroker =
            host->GetDropBrokerForTest();
        REQUIRE(pendingBroker);
        CHECK_FALSE(pendingBroker->IsInitializationPending());
        CHECK_FALSE(pendingBroker->IsReady());

        // Hidden construction performs no native broker acquisition. Hold
        // the fixed TLW operation through first presentation so the visible
        // flush, rather than host construction, owns the pending seam.
        frame->Show();
        host->FlushSync();
        CHECK(pendingBroker->IsInitializationPending());
        CHECK_FALSE(pendingBroker->IsReady());

        slot = host->FindSlot(button);
        REQUIRE(slot);
        content = slot->GetContent();
        REQUIRE(content != nullptr);
        CHECK(initializationHook.GetTLWBindingCount() == 1);

        // A queued fixed-operation continuation must not finish native OLE
        // acquisition after the presentation becomes hidden.
        frame->Hide();
        REQUIRE(wxMSWOleCancelShellDropTargetOperation(&fixed));
    }

    CHECK(initializationHook.GetTLWBindingCount() == 1);
    DrainDispatch(24);

    wxWinUIDropBroker* const hiddenPendingBroker =
        host->GetDropBrokerForTest();
    REQUIRE(hiddenPendingBroker);
    CHECK(hiddenPendingBroker->IsInitializationPending());
    CHECK_FALSE(hiddenPendingBroker->IsReady());
    const wxWinUIDropBrokerSnapshot hiddenPendingSnapshot =
        hiddenPendingBroker->GetSnapshotForTest();
    CHECK_FALSE(hiddenPendingSnapshot.ownsRegistration);
    CHECK_FALSE(hiddenPendingSnapshot.ownsCompleteRegistrationSet);
    CHECK(host->OwnsOleDropRegistration());

    frame->Show();
    DrainDispatch();
    host->FlushSync();
    DrainDispatch(8);

    CHECK(initializationHook.GetTLWBindingCount() == 1);
    REQUIRE(wxWinUITopLevelHost::FindForTLW(frame) == host);
    REQUIRE(wxWinUITopLevelHost::FindSlotOwner(button) == host);
    REQUIRE(host->FindSlot(button));
    CHECK(host->FindSlot(button) == slot);
    CHECK(host->FindSlot(button)->GetContent() == content);

    wxWinUIDropBroker* const readyBroker = host->GetDropBrokerForTest();
    REQUIRE(readyBroker);
    CHECK_FALSE(readyBroker->IsInitializationPending());
    CHECK(readyBroker->IsReady());

    delete frame;
    DrainDispatch();
}

TEST_CASE("WinUI OLE broker keeps TLW target through hidden host init rollback",
          "[winui-drop-broker][transaction][host-initialization]")
{
    wxFrame* const frame =
        new wxFrame(nullptr, wxID_ANY, "ole-broker-tlw-rollback",
                    wxPoint(-24000, -24000), wxSize(420, 220));
    const HWND frameHwnd = GetHwndOf(frame);
    REQUIRE(frameHwnd);

    RoutedTextTarget* const routed = new RoutedTextTarget;
    frame->SetDropTarget(routed);
    REQUIRE(wxMSWOleIsShellDropTargetRegistered(routed, frameHwnd));

    {
        HostInitializationFaultGuard fault(
            wxWinUITopLevelHost::
                TestInitializationFault_AfterTLWBindings);
        CHECK(wxWinUITopLevelHost::ForWindow(frame, true) == nullptr);
    }
    DrainDispatch();

    CHECK(wxWinUITopLevelHost::FindForTLW(frame) == nullptr);
    CHECK(wxMSWOleIsShellDropTargetRegistered(routed, frameHwnd));

    delete frame;
    DrainDispatch();
}

TEST_CASE("WinUI OLE broker survives TLW destruction during target discovery",
          "[winui-drop-broker][transaction][host-initialization][reentrancy]")
{
    wxFrame* const frame =
        new wxFrame(nullptr, wxID_ANY, "ole-broker-discovery-destroy",
                    wxPoint(-24000, -24000), wxSize(420, 220));
    bool destroyAttempted = false;
    bool destroyAccepted = false;
    auto* const child = new DestroyTLWOnDropTargetLookup(
        frame, &destroyAttempted, &destroyAccepted);
    child->SetDropTarget(new RoutedTextTarget);

    wxWinUITopLevelHost* const host =
        wxWinUITopLevelHost::ForWindow(frame, true);
    REQUIRE(host);
    CHECK_FALSE(destroyAttempted);

    frame->Show();
    host->FlushSync();
    CHECK(destroyAttempted);
    CHECK(destroyAccepted);
    DrainDispatch(24);
    CHECK(wxWinUITopLevelHost::FindForTLW(frame) == nullptr);
}

TEST_CASE("WinUI OLE broker preserves reparenting until presentation",
          "[winui-drop-broker][transaction][host-initialization][reentrancy]")
{
    wxFrame* const source =
        new wxFrame(nullptr, wxID_ANY, "ole-broker-provisional-source",
                    wxPoint(-24000, -24000), wxSize(420, 220));
    wxFrame* const destination =
        new wxFrame(nullptr, wxID_ANY, "ole-broker-provisional-destination",
                    wxPoint(-23500, -24000), wxSize(420, 220));
    wxPanel* const child =
        new wxPanel(source, wxID_ANY, wxPoint(20, 20), wxSize(220, 100));
    const HWND childHwnd = GetHwndOf(child);
    REQUIRE(childHwnd);

    RoutedTextTarget* const routed = new RoutedTextTarget;
    child->SetDropTarget(routed);
    REQUIRE(wxMSWOleIsShellDropTargetRegistered(routed, childHwnd));

    wxWinUITopLevelHost* host = nullptr;
    {
        ProvisionalReparentHookGuard hook(child, destination);
        host = wxWinUITopLevelHost::ForWindow(destination, true);
        CHECK(hook.WasAttempted());
        CHECK(hook.Succeeded());
    }

    REQUIRE(host);
    CHECK(child->GetParent() == destination);
    CHECK_FALSE(host->OwnsOleDropRegistration());
    CHECK(wxMSWOleIsShellDropTargetRegistered(routed, childHwnd));

    destination->Show();
    DrainDispatch();
    host->FlushSync();
    DrainDispatch(4);
    CHECK(host->OwnsOleDropRegistration());
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(routed, childHwnd));

    delete destination;
    delete source;
    DrainDispatch();
}

TEST_CASE("WinUI OLE broker routes a real slot and legacy file drop",
          "[winui-drop-broker]")
{
    wxFrame* const frame =
        new wxFrame(nullptr, wxID_ANY, "ole-broker-route",
                    wxPoint(-24000, -24000), wxSize(420, 220));
    wxPanel* const panel =
        new wxPanel(frame, wxID_ANY, wxPoint(0, 0), wxSize(400, 200));

    // This target predates the first XAML slot, so it starts life registered
    // on the panel's shell HWND. Host creation must migrate it atomically.
    RoutedTextTarget* const routed = new RoutedTextTarget;
    panel->SetDropTarget(routed);
    REQUIRE(wxMSWOleIsShellDropTargetRegistered(
        routed, panel->GetHWND()));

    wxButton* const button =
        new wxButton(panel, wxID_ANY, "slot",
                     wxPoint(40, 40), wxSize(180, 70));
    wxWinUITopLevelHost* const host =
        wxWinUITopLevelHost::FindSlotOwner(button);
    REQUIRE(host);
    REQUIRE_FALSE(host->OwnsOleDropRegistration());
    REQUIRE(host->Root());
    CHECK(host->Root().AllowDrop());
    const wxWinUIDropBrokerSnapshot hiddenHostSnapshot =
        host->GetDropBrokerForTest()->GetSnapshotForTest();
    CHECK_FALSE(hiddenHostSnapshot.active);
    CHECK_FALSE(hiddenHostSnapshot.ownsRegistration);
    CHECK(wxMSWOleIsShellDropTargetRegistered(
        routed, panel->GetHWND()));

    frame->Show();
    DrainDispatch();
    host->FlushSync();
    DrainDispatch(4);

    REQUIRE(host->OwnsOleDropRegistration());
    const wxWinUIDropBrokerSnapshot hostSnapshot =
        host->GetDropBrokerForTest()->GetSnapshotForTest();
    CHECK(hostSnapshot.active);
    CHECK(hostSnapshot.registrationHwnd == GetHwndOf(frame));
    CHECK(hostSnapshot.registrationHwnd != host->GetBridgeHwnd());
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(
        routed, panel->GetHWND()));

    wxWinUISlot* const slot = host->FindSlot(button);
    REQUIRE(slot);
    const wxRect slotRect = slot->GetRectInTLW();
    REQUIRE_FALSE(slotRect.IsEmpty());
    const RECT clientScreen = host->GetClientScreenRect();
    POINTL point =
    {
        clientScreen.left + slotRect.x + slotRect.width / 2,
        clientScreen.top + slotRect.y + slotRect.height / 2
    };

    wxWinUIDropBroker* const broker = host->GetDropBrokerForTest();
    REQUIRE(broker);
    IDropTarget* const target = broker->GetCOMTargetForTest();
    REQUIRE(target);
    target->AddRef();

    wxTextDataObject source(wxString::FromUTF8("broker hélice"));
    DWORD effect = DROPEFFECT_COPY | DROPEFFECT_MOVE;
    REQUIRE(target->DragEnter(
        source.GetInterface(), MK_CONTROL, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_COPY);
    CHECK(routed->enters == 1);
    CHECK(routed->overs == 0);

    effect = DROPEFFECT_COPY | DROPEFFECT_MOVE;
    REQUIRE(target->DragOver(MK_SHIFT, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_MOVE);
    CHECK(routed->overs == 1);

    effect = DROPEFFECT_COPY;
    REQUIRE(target->Drop(source.GetInterface(), 0, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_COPY);
    CHECK(routed->drops == 1);
    CHECK(routed->text == wxString::FromUTF8("broker hélice"));
    POINT expectedPanelClient = { point.x, point.y };
    ::SetLastError(ERROR_SUCCESS);
    const int mappedPanel = ::MapWindowPoints(
        HWND_DESKTOP, GetHwndOf(panel), &expectedPanelClient, 1);
    REQUIRE((mappedPanel != 0 || ::GetLastError() == ERROR_SUCCESS));
    CHECK(routed->lastX == expectedPanelClient.x);
    CHECK(routed->lastY == expectedPanelClient.y);

    panel->Disable();
    effect = DROPEFFECT_COPY;
    REQUIRE(target->DragEnter(
        source.GetInterface(), 0, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_NONE);
    REQUIRE(target->DragLeave() == S_OK);
    panel->Enable();

    winrt::Microsoft::UI::Xaml::Controls::Primitives::Popup popup;
    popup.XamlRoot(host->GetXamlRoot());
    popup.Child(winrt::Microsoft::UI::Xaml::Controls::Border());
    popup.IsOpen(true);
    effect = DROPEFFECT_COPY;
    REQUIRE(target->DragEnter(
        source.GetInterface(), 0, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_NONE);
    REQUIRE(target->DragLeave() == S_OK);
    popup.IsOpen(false);

    // Removing the active logical target invalidates every outstanding route;
    // a subsequent COM movement fails closed instead of reusing its address.
    effect = DROPEFFECT_COPY;
    REQUIRE(target->DragEnter(
        source.GetInterface(), 0, point, &effect) == S_OK);
    panel->SetDropTarget(nullptr);
    effect = DROPEFFECT_COPY;
    REQUIRE(target->DragOver(0, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_NONE);
    REQUIRE(target->DragLeave() == S_OK);

    RoutedFileTarget* const fileTarget = new RoutedFileTarget;
    panel->SetDropTarget(fileTarget);
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(
        fileTarget, panel->GetHWND()));
    wxFileDataObject fileSource;
    const wxString oleFile = "C:\\ole-broker-file.txt";
    fileSource.AddFile(oleFile);
    effect = DROPEFFECT_COPY;
    REQUIRE(target->DragEnter(
        fileSource.GetInterface(), 0, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_COPY);
    REQUIRE(target->Drop(
        fileSource.GetInterface(), 0, point, &effect) == S_OK);
    CHECK(fileTarget->drops == 1);
    REQUIRE(fileTarget->files.size() == 1);
    CHECK(fileTarget->files[0] == oleFile);
    panel->SetDropTarget(nullptr);

    bool gotFiles = false;
    wxPoint filePoint;
    wxString fileName;
    button->Bind(
        wxEVT_DROP_FILES,
        [&](wxDropFilesEvent& event)
        {
            gotFiles = true;
            filePoint = event.GetPosition();
            REQUIRE(event.GetNumberOfFiles() == 1);
            fileName = event.GetFiles()[0];
        });
    button->DragAcceptFiles(true);
    CHECK((::GetWindowLongPtr(
               host->GetBridgeHwnd(), GWL_EXSTYLE) & WS_EX_ACCEPTFILES) != 0);

    POINT bridgePoint = { point.x, point.y };
    ::MapWindowPoints(
        HWND_DESKTOP, host->GetBridgeHwnd(), &bridgePoint, 1);
    const wxString expectedFile = "C:\\drop-broker-test.txt";
    HDROP drop = MakeDropFiles(bridgePoint, expectedFile);
    REQUIRE(drop);
    ::SendMessage(
        host->GetBridgeHwnd(), WM_DROPFILES,
        reinterpret_cast<WPARAM>(drop), 0);
    CHECK(gotFiles);
    CHECK(fileName == expectedFile);
    CHECK(filePoint.x >= 0);
    CHECK(filePoint.y >= 0);

    button->DragAcceptFiles(false);
    wxPanel* const transientReceiver =
        new wxPanel(panel, wxID_ANY, wxPoint(260, 20), wxSize(80, 50));
    transientReceiver->DragAcceptFiles(true);
    CHECK((::GetWindowLongPtr(
               host->GetBridgeHwnd(), GWL_EXSTYLE) & WS_EX_ACCEPTFILES) != 0);
    delete transientReceiver;
    CHECK((::GetWindowLongPtr(
               host->GetBridgeHwnd(), GWL_EXSTYLE) & WS_EX_ACCEPTFILES) == 0);

    IDropTarget* const lateTarget = target;
    lateTarget->AddRef();
    target->Release();
    delete frame;
    DrainDispatch();

    effect = DROPEFFECT_MOVE;
    REQUIRE(lateTarget->DragOver(0, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_NONE);
    lateTarget->Release();
}

TEST_CASE("MSW drop binding follows a recreated shell HWND",
          "[winui-drop-broker]")
{
    wxFrame* const frame =
        new wxFrame(nullptr, wxID_ANY, "ole-binding-recreate",
                    wxPoint(-24000, -24000), wxSize(240, 140));
    const int id = 7011;
    const auto makeNative = [&]() -> HWND
    {
        return ::CreateWindowExW(
            0, L"STATIC", L"drop-binding",
            WS_CHILD | WS_VISIBLE,
            10, 10, 120, 40,
            GetHwndOf(frame),
            reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
            wxGetInstance(), nullptr);
    };

    HWND first = makeNative();
    REQUIRE(first);
    wxNativeWindow* const window =
        new wxNativeWindow(frame, id, first);
    REQUIRE(window->GetHWND() == first);

    RoutedTextTarget* const target = new RoutedTextTarget;
    window->SetDropTarget(target);
    REQUIRE(wxMSWOleIsShellDropTargetRegistered(target, first));

    wxMSWOleDropTargetBinding oldBinding;
    REQUIRE(wxMSWOleLookupDropTarget(window, &oldBinding) ==
            wxMSWOleDropTargetLookup::Found);
    REQUIRE(oldBinding.GetOwnerHwndIfCurrent() == first);

    window->DissociateHandle();
    CHECK_FALSE(oldBinding.IsCurrent());
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(target, first));
    ::DestroyWindow(first);

    HWND second = makeNative();
    REQUIRE(second);
    window->AssociateHandle(second);

    wxMSWOleDropTargetBinding newBinding;
    REQUIRE(wxMSWOleLookupDropTarget(window, &newBinding) ==
            wxMSWOleDropTargetLookup::Found);
    CHECK(newBinding.IsCurrent());
    CHECK(newBinding.GetOwnerHwndIfCurrent() == second);
    CHECK_FALSE(newBinding.IsSameBinding(oldBinding));
    CHECK(wxMSWOleIsShellDropTargetRegistered(target, second));

    delete window;
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(target, second));
    ::DestroyWindow(second);
    delete frame;
}

TEST_CASE("WinUI OLE broker pins COM state across synchronous TLW teardown",
          "[winui-drop-broker]")
{
    const auto run = [](ReentrantDestroyTextTarget::Trigger trigger)
    {
        wxFrame* frame =
            new wxFrame(nullptr, wxID_ANY, "ole-reentrant-teardown",
                        wxPoint(-24000, -24000), wxSize(320, 170));
        wxPanel* const panel =
            new wxPanel(frame, wxID_ANY, wxPoint(0, 0), wxSize(300, 150));

        panel->SetDropTarget(new ReentrantDestroyTextTarget(
            trigger,
            [&frame]()
            {
                wxFrame* const doomed = frame;
                frame = nullptr;
                delete doomed;
            }));

        wxButton* const slot =
            new wxButton(panel, wxID_ANY, "slot",
                         wxPoint(40, 35), wxSize(150, 60));
        wxWinUITopLevelHost* const host =
            wxWinUITopLevelHost::FindSlotOwner(slot);
        REQUIRE(host);
        frame->Show();
        DrainDispatch();
        host->FlushSync();

        wxWinUISlot* const hostedSlot = host->FindSlot(slot);
        REQUIRE(hostedSlot);
        const wxRect rect = hostedSlot->GetRectInTLW();
        const RECT clientScreen = host->GetClientScreenRect();
        const POINTL point =
        {
            clientScreen.left + rect.x + rect.width / 2,
            clientScreen.top + rect.y + rect.height / 2
        };

        // Deliberately do not AddRef(): this proves the entry point's own pin,
        // not an external test reference masking a re-entrant destruction.
        IDropTarget* const target =
            host->GetDropBrokerForTest()->GetCOMTargetForTest();
        REQUIRE(target);

        wxTextDataObject source("destroy-owner");
        DWORD effect = DROPEFFECT_COPY;
        REQUIRE(target->DragEnter(
            source.GetInterface(), 0, point, &effect) == S_OK);

        if ( trigger == ReentrantDestroyTextTarget::Trigger::Data )
        {
            REQUIRE(frame);
            effect = DROPEFFECT_COPY;
            REQUIRE(target->Drop(
                source.GetInterface(), 0, point, &effect) == S_OK);
        }

        CHECK(frame == nullptr);
        CHECK(effect == DROPEFFECT_NONE);
        DrainDispatch();
    };

    SECTION("destroy from OnEnter")
    {
        run(ReentrantDestroyTextTarget::Trigger::Enter);
    }
    SECTION("destroy from OnData")
    {
        run(ReentrantDestroyTextTarget::Trigger::Data);
    }
}

TEST_CASE("MSW fixed OLE wrapper pins itself across owner teardown",
          "[winui-drop-broker]")
{
    const auto run = [](ReentrantDestroyTextTarget::Trigger trigger)
    {
        wxFrame* frame =
            new wxFrame(nullptr, wxID_ANY, "ole-fixed-reentrant",
                        wxPoint(-24000, -24000), wxSize(260, 140));
        wxPanel* const panel =
            new wxPanel(frame, wxID_ANY, wxPoint(10, 10), wxSize(180, 80));
        auto* const logical = new ReentrantDestroyTextTarget(
            trigger,
            [&frame]()
            {
                wxFrame* const doomed = frame;
                frame = nullptr;
                delete doomed;
            });
        panel->SetDropTarget(logical);
        REQUIRE(wxMSWOleIsShellDropTargetRegistered(
            logical, panel->GetHWND()));
        frame->Show();
        DrainDispatch(2);

        IDropTarget* const target =
            wxMSWOleDropTargetAccess::GetCOMInterface(logical);
        REQUIRE(target);
        POINT point = { 20, 20 };
        REQUIRE(::ClientToScreen(GetHwndOf(panel), &point));
        const POINTL screen = { point.x, point.y };

        wxTextDataObject source("fixed-destroy-owner");
        DWORD effect = DROPEFFECT_COPY;
        REQUIRE(target->DragEnter(
            source.GetInterface(), 0, screen, &effect) == S_OK);

        if ( trigger == ReentrantDestroyTextTarget::Trigger::Data )
        {
            REQUIRE(frame);
            effect = DROPEFFECT_COPY;
            REQUIRE(target->Drop(
                source.GetInterface(), 0, screen, &effect) == S_OK);
        }

        CHECK(frame == nullptr);
        CHECK(effect == DROPEFFECT_NONE);
        DrainDispatch();
    };

    SECTION("destroy from OnEnter")
    {
        run(ReentrantDestroyTextTarget::Trigger::Enter);
    }
    SECTION("destroy from OnData")
    {
        run(ReentrantDestroyTextTarget::Trigger::Data);
    }
}

TEST_CASE("WinUI OLE broker transfers a route across top-level hosts",
          "[winui-drop-broker]")
{
    wxFrame* const frameA =
        new wxFrame(nullptr, wxID_ANY, "ole-host-a",
                    wxPoint(-24000, -24000), wxSize(360, 200));
    wxFrame* const frameB =
        new wxFrame(nullptr, wxID_ANY, "ole-host-b",
                    wxPoint(-23500, -24000), wxSize(360, 200));

    wxPanel* const moving =
        new wxPanel(frameA, wxID_ANY, wxPoint(0, 0), wxSize(330, 170));
    RoutedTextTarget* const logicalTarget = new RoutedTextTarget;
    moving->SetDropTarget(logicalTarget);
    wxButton* const movingSlot =
        new wxButton(moving, wxID_ANY, "moving-slot",
                     wxPoint(35, 35), wxSize(170, 65));
    wxButton* const destinationPrime =
        new wxButton(frameB, wxID_ANY, "destination-prime",
                     wxPoint(220, 100), wxSize(100, 40));

    frameA->Show();
    frameB->Show();
    DrainDispatch();

    wxWinUITopLevelHost* const hostA =
        wxWinUITopLevelHost::FindSlotOwner(movingSlot);
    wxWinUITopLevelHost* const hostB =
        wxWinUITopLevelHost::FindSlotOwner(destinationPrime);
    REQUIRE(hostA);
    REQUIRE(hostB);
    REQUIRE(hostA != hostB);

    IDropTarget* const comA =
        hostA->GetDropBrokerForTest()->GetCOMTargetForTest();
    IDropTarget* const comB =
        hostB->GetDropBrokerForTest()->GetCOMTargetForTest();
    REQUIRE(comA);
    REQUIRE(comB);
    comA->AddRef();
    comB->AddRef();

    REQUIRE(moving->Reparent(frameB));
    frameB->SetLayoutDirection(wxLayout_RightToLeft);
    DrainDispatch();
    hostB->FlushSync();
    DrainDispatch(4);

    REQUIRE(wxWinUITopLevelHost::FindSlotOwner(movingSlot) == hostB);
    CHECK_FALSE(wxMSWOleIsShellDropTargetRegistered(
        logicalTarget, moving->GetHWND()));

    wxWinUISlot* const slot = hostB->FindSlot(movingSlot);
    REQUIRE(slot);
    const wxRect rect = slot->GetRectInTLW();
    REQUIRE_FALSE(rect.IsEmpty());
    const RECT clientScreen = hostB->GetClientScreenRect();
    POINTL point =
    {
        clientScreen.left + rect.x + rect.width / 2,
        clientScreen.top + rect.y + rect.height / 2
    };

    wxTextDataObject source("cross-tlw");
    DWORD effect = DROPEFFECT_COPY;
    REQUIRE(comA->DragEnter(
        source.GetInterface(), 0, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_NONE);
    REQUIRE(comA->DragLeave() == S_OK);

    effect = DROPEFFECT_COPY;
    REQUIRE(comB->DragEnter(
        source.GetInterface(), 0, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_COPY);
    CHECK(logicalTarget->enters == 1);
    CHECK(logicalTarget->lastX >= 0);
    CHECK(logicalTarget->lastY >= 0);
    REQUIRE(comB->DragLeave() == S_OK);
    CHECK(logicalTarget->leaves == 1);

    comA->Release();
    comB->Release();
    delete frameA;
    delete frameB;
    DrainDispatch();
}

TEST_CASE("WinUI OLE broker honours virtual composite target redirection",
          "[winui-drop-broker]")
{
    wxFrame* const frame =
        new wxFrame(nullptr, wxID_ANY, "ole-redirect",
                    wxPoint(-24000, -24000), wxSize(420, 210));
    RedirectPanel* const hitPanel =
        new RedirectPanel(frame, wxPoint(20, 25), wxSize(140, 90));
    wxPanel* const ownerPanel =
        new wxPanel(frame, wxID_ANY, wxPoint(230, 25), wxSize(140, 90));
    RoutedTextTarget* const logicalTarget = new RoutedTextTarget;
    ownerPanel->SetDropTarget(logicalTarget);
    hitPanel->RedirectTo(ownerPanel);

    wxButton* const hostPrime =
        new wxButton(frame, wxID_ANY, "host-prime",
                     wxPoint(160, 135), wxSize(110, 40));
    frame->Show();
    DrainDispatch();

    wxWinUITopLevelHost* const host =
        wxWinUITopLevelHost::FindSlotOwner(hostPrime);
    REQUIRE(host);
    host->FlushSync();

    RECT hitRect = { 0, 0, 0, 0 };
    REQUIRE(::GetWindowRect(GetHwndOf(hitPanel), &hitRect));
    POINTL point =
    {
        (hitRect.left + hitRect.right) / 2,
        (hitRect.top + hitRect.bottom) / 2
    };

    IDropTarget* const com =
        host->GetDropBrokerForTest()->GetCOMTargetForTest();
    REQUIRE(com);
    com->AddRef();
    wxTextDataObject source("redirect");
    DWORD effect = DROPEFFECT_COPY;
    REQUIRE(com->DragEnter(
        source.GetInterface(), 0, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_COPY);
    CHECK(logicalTarget->enters == 1);
    // Coordinates belong to the actual binding owner, not the queried
    // composite window; the point lies to its left in this fixture.
    CHECK(logicalTarget->lastX < 0);
    REQUIRE(com->DragLeave() == S_OK);
    com->Release();

    delete frame;
    DrainDispatch();
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_OLE && wxUSE_DRAG_AND_DROP

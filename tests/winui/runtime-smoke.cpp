/////////////////////////////////////////////////////////////////////////////
// Name:        tests/winui/runtime-smoke.cpp
// Purpose:     Standalone probe of the unpackaged wxWinUI runtime payload
// Author:      wxWidgets development team
// Created:     2026-07-21
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// This program verifies that the directory it runs from contains everything a
// wxWinUI application needs to start on a machine without any manually copied
// artefacts: the Windows App SDK bootstrapper, and the PRI/asset payload that
// XAML needs to resolve its theme and control template resources.
//
// It deliberately does not link against wxWidgets: it must be runnable from
// its own output directory to prove that the payload deployed next to *any*
// executable is self-sufficient, and it must stay buildable while the wx test
// suite itself is broken for unrelated reasons.  It replicates the exact
// initialization sequence of src/winui/winui.cpp (bootstrap, dispatcher
// queue, application object providing the XAML metadata, XAML manager, then
// XamlControlsResources) and then realizes Button, TreeView and TabView in a
// real (off-screen) XAML island.
//
// The normal executable and --ole-drop-probe never generate physical input.
// The separate --ole-drop-delivery-probe is intentionally absent from CTest:
// it briefly presents a no-activate island under an idle cursor and performs
// one bounded, tagged left-button drag from a probe-owned source pad into each
// candidate HWND so that DoDragDrop performs real target selection.
// It refuses to start unless the interactive desktop has been idle and
// WX_WINUI_ALLOW_PHYSICAL_OLE_PROBE is set to the exact acknowledgement shown
// by the usage diagnostic. Any real mouse or keyboard input cancels it
// immediately.
//
// Exit codes:
//   0  success
//   1  Win32 setup failed (window class/window creation)
//   2  MddBootstrapInitialize2 failed: the Windows App Runtime framework
//      package is not installed on this machine
//   3  XamlControlsResources could not be created: the WinUI resource payload
//      (PRI files / asset trees) is missing next to the executable
//   4  the probe controls did not load or did not get their templates
//   5  unexpected WinRT error
//   6  dispatcher queue / XAML manager initialization failed
//   7  watchdog timeout (a stage hung instead of failing)
//   8  invalid command line
//   9  OLE initialization failed for either OLE probe
//  10  the OLE probe could not find a valid TLW / bridge / inner InputSite
//  11  the TLW or bridge already owns an OLE drop target
//  12  OLE registration or exact revocation failed
//  13  XAML stopped being usable after the OLE probe
//  14  the inner InputSite already owns an OLE drop target; callback
//      coexistence must be proved before installing one on the bridge
//  15  --ole-drop-delivery-probe was not explicitly armed, or the interactive
//      desktop was not idle enough to run it without disturbing the user
//  16  the opt-in delivery probe could not install its safety guards, present
//      its window, inject its bounded drag samples, or was interrupted by
//      real user input
//  17  the delivery matrix is inconclusive because one of the candidate HWNDs
//      already owns a foreign OLE drop target (never revoked by this probe)
//  18  the physical callback matrix found no viable registration topology,
//      an arm timed out, or a callback/data/thread/effect contract was only
//      partially delivered
//  19  an owned physical-probe registration could not be revoked/unlocked
//      exactly; the inactive target is deliberately retained until exit
//  20  DoDragDrop did not return within the bounded cancellation grace period;
//      the physical-probe process was terminated without unsafe COM cleanup
//  21  the DispatcherQueue shutdown phases, deferral or deferred timer did not
//      satisfy the documented synchronous shutdown contract

#include <windows.h>
#include <ole2.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <memory>
#include <thread>

#include <MddBootstrap.h>
#include <WindowsAppSDK-VersionInfo.h>

#ifdef GetCurrentTime
    #undef GetCurrentTime
#endif

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Interop.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.XamlTypeInfo.h>

namespace
{

// What the watchdog reports if a stage hangs instead of returning an error.
std::atomic<const char*> gs_stage{ "startup" };

enum class DispatcherShutdownPhase : unsigned
{
    Active,
    ShutdownStarting,
    MarkerDispatched,
    TimerTick,
    FrameworkShutdownStarting,
    FrameworkShutdownCompleted,
    ShutdownCompleted,
    Returned
};

struct DispatcherShutdownProbeState
{
    explicit DispatcherShutdownProbeState(DWORD owner)
        : ownerThreadId(owner)
    {
    }

    void Fail(HRESULT hr = E_FAIL) noexcept
    {
        failed = true;
        if ( firstFailure == S_OK )
            firstFailure = hr;
    }

    void CheckOwnerThread() noexcept
    {
        if ( ::GetCurrentThreadId() != ownerThreadId )
            Fail(RPC_E_WRONG_THREAD);
    }

    void Advance(DispatcherShutdownPhase expected,
                 DispatcherShutdownPhase next) noexcept
    {
        CheckOwnerThread();
        if ( phase != expected )
            Fail(E_UNEXPECTED);
        phase = next;
    }

    void CompleteApplicationDeferral() noexcept
    {
        const auto deferral = applicationDeferral;
        applicationDeferral = nullptr;
        if ( !deferral )
        {
            Fail(E_UNEXPECTED);
            return;
        }

        // Mark this before Complete(): advancing to the framework phase may be
        // re-entrant from this call.
        applicationDeferralCompleteCalled = true;
        try
        {
            deferral.Complete();
        }
        catch ( const winrt::hresult_error& e )
        {
            Fail(static_cast<HRESULT>(e.code()));
        }
        catch ( ... )
        {
            Fail();
        }
    }

    const DWORD ownerThreadId;
    DispatcherShutdownPhase phase = DispatcherShutdownPhase::Active;
    winrt::Windows::Foundation::Deferral applicationDeferral{ nullptr };
    HRESULT firstFailure = S_OK;
    unsigned shutdownStartingCount = 0;
    unsigned markerCount = 0;
    unsigned timerTickCount = 0;
    unsigned frameworkStartingCount = 0;
    unsigned xamlCompletedCount = 0;
    unsigned frameworkCompletedCount = 0;
    unsigned shutdownCompletedCount = 0;
    bool failed = false;
    bool markerAccepted = false;
    bool timerStarted = false;
    bool applicationDeferralCompleteCalled = false;
    bool xamlDeferralCompleteCalled = false;
    bool frameworkEnqueueRejected = false;
};

class OleApartment
{
public:
    explicit OleApartment(bool initialize)
    {
        if ( initialize )
        {
            m_result = ::OleInitialize(nullptr);
            m_initialized = SUCCEEDED(m_result);
        }
    }

    ~OleApartment()
    {
        if ( m_initialized )
            ::OleUninitialize();
    }

    void RetainForProcessLifetime() noexcept
    {
        // The OLE registration probe is interested in the island's owned drop
        // targets, not in re-testing the wx runtime apartment ledger. With the
        // unpackaged Windows App SDK 1.8 framework, balancing the final OLE
        // reference after XAML shutdown lets COM unload the framework DLLs in
        // an unsafe order: Microsoft.UI.Xaml.Controls.dll can run a static
        // destructor after the implementation module behind one of its cached
        // WinRT objects has already gone away. Keep this one probe-owned STA
        // reference until ExitProcess instead. The installed-consumer fixture
        // separately verifies that the actual wx runtime balances its own COM
        // ownership on every clean path.
        m_initialized = false;
    }

    HRESULT Result() const { return m_result; }
    bool IsInitialized() const { return m_initialized; }

private:
    HRESULT m_result = S_OK;
    bool m_initialized = false;
};

class OleDropProbeTarget final : public IDropTarget
{
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **object) override
    {
        if ( !object )
            return E_POINTER;

        *object = nullptr;
        if ( iid == IID_IUnknown || iid == IID_IDropTarget )
        {
            *object = static_cast<IDropTarget *>(this);
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

    HRESULT STDMETHODCALLTYPE DragEnter(
        IDataObject *, DWORD, POINTL, DWORD *effect) override
    {
        if ( effect )
            *effect = DROPEFFECT_NONE;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragOver(DWORD, POINTL, DWORD *effect) override
    {
        if ( effect )
            *effect = DROPEFFECT_NONE;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragLeave() override
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Drop(
        IDataObject *, DWORD, POINTL, DWORD *effect) override
    {
        if ( effect )
            *effect = DROPEFFECT_NONE;
        return S_OK;
    }

private:
    ~OleDropProbeTarget() = default;

    LONG m_references = 1;
};

constexpr const char* PhysicalProbeArmVariable =
    "WX_WINUI_ALLOW_PHYSICAL_OLE_PROBE";
constexpr const char* PhysicalProbeArmValue =
    "I_UNDERSTAND_THIS_MOVES_THE_CURSOR_AND_CLICKS";
constexpr DWORD PhysicalProbeMinimumIdleMs = 8000;
constexpr DWORD PhysicalProbeArmTimeoutMs = 2500;
constexpr DWORD PhysicalProbeCancelWakeIntervalMs = 25;
constexpr DWORD PhysicalProbeCancelGraceMs = 5000;
// Keep the tag within 32 bits. The low-level input path can preserve only the
// low DWORD of dwExtraInfo even for this x64 process; a wider tag would make
// the guard misclassify its own LLMHF_INJECTED movement as physical input.
constexpr ULONG_PTR PhysicalProbeInputTag =
    static_cast<ULONG_PTR>(0x4F4C4555UL);

bool IsPhysicalProbeArmed()
{
    const char* const value = std::getenv(PhysicalProbeArmVariable);
    return value && std::strcmp(value, PhysicalProbeArmValue) == 0;
}

bool GetPhysicalInputIdleMs(DWORD* idleMs)
{
    LASTINPUTINFO info{ sizeof(info) };
    if ( !idleMs || !::GetLastInputInfo(&info) )
        return false;

    // DWORD subtraction intentionally handles GetTickCount() wrap-around.
    *idleMs = ::GetTickCount() - info.dwTime;
    return true;
}

bool WaitForPhysicalInputIdle(DWORD timeoutMs, DWORD* idleMs)
{
    const ULONGLONG deadline = ::GetTickCount64() + timeoutMs;
    for ( ;; )
    {
        DWORD currentIdle = 0;
        if ( !GetPhysicalInputIdleMs(&currentIdle) )
            return false;
        if ( idleMs )
            *idleMs = currentIdle;
        if ( currentIdle >= PhysicalProbeMinimumIdleMs )
            return true;
        if ( ::GetTickCount64() >= deadline )
            return false;

        MSG msg;
        while ( ::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE) )
        {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        }
        ::MsgWaitForMultipleObjectsEx(
            0, nullptr, 100, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
    }
}

void ReportPhysicalSafetyRefusal(const char* reason, DWORD idleMs = 0)
{
    std::fprintf(
        stderr,
        "wx_winui_ole_delivery: {\"event\":\"safety\","
        "\"result\":\"refused\",\"reason\":\"%s\","
        "\"idle_ms\":%lu,\"required_idle_ms\":%lu,"
        "\"required_env\":\"%s=%s\",\"exit_code\":15}\n",
        reason,
        static_cast<unsigned long>(idleMs),
        static_cast<unsigned long>(PhysicalProbeMinimumIdleMs),
        PhysicalProbeArmVariable,
        PhysicalProbeArmValue);
    std::fflush(stderr);
}

struct PhysicalInputGuardState
{
    std::atomic<bool> interrupted{ false };
    std::atomic<unsigned> kind{ 0 };
    std::atomic<ULONG_PTR> message{ 0 };
    std::atomic<DWORD> flags{ 0 };
    std::atomic<LONG> pointX{ 0 };
    std::atomic<LONG> pointY{ 0 };
    std::atomic<DWORD> data1{ 0 };
    std::atomic<DWORD> data2{ 0 };
    std::atomic<DWORD> time{ 0 };
    std::atomic<ULONG_PTR> extraInfo{ 0 };

    void Record(
        unsigned inputKind,
        WPARAM inputMessage,
        DWORD inputFlags,
        POINT point,
        DWORD inputData1,
        DWORD inputData2,
        DWORD inputTime,
        ULONG_PTR inputExtraInfo) noexcept
    {
        if ( interrupted.load(std::memory_order_relaxed) )
            return;

        kind.store(inputKind, std::memory_order_relaxed);
        message.store(inputMessage, std::memory_order_relaxed);
        flags.store(inputFlags, std::memory_order_relaxed);
        pointX.store(point.x, std::memory_order_relaxed);
        pointY.store(point.y, std::memory_order_relaxed);
        data1.store(inputData1, std::memory_order_relaxed);
        data2.store(inputData2, std::memory_order_relaxed);
        time.store(inputTime, std::memory_order_relaxed);
        extraInfo.store(inputExtraInfo, std::memory_order_relaxed);
        interrupted.store(true, std::memory_order_release);
    }
};

std::atomic<PhysicalInputGuardState*> gs_physicalInputGuard{ nullptr };

LRESULT CALLBACK PhysicalMouseHook(int code, WPARAM wParam, LPARAM lParam)
{
    if ( code >= 0 )
    {
        const auto* const data =
            reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
        PhysicalInputGuardState* const state =
            gs_physicalInputGuard.load(std::memory_order_acquire);
        if ( state && data )
        {
            if ( data->dwExtraInfo != PhysicalProbeInputTag ||
                 (data->flags & LLMHF_INJECTED) == 0 )
            {
                state->Record(
                    1, wParam, data->flags, data->pt,
                    data->mouseData, 0, data->time, data->dwExtraInfo);
            }
        }
    }

    return ::CallNextHookEx(nullptr, code, wParam, lParam);
}

LRESULT CALLBACK PhysicalKeyboardHook(int code, WPARAM wParam, LPARAM lParam)
{
    if ( code >= 0 )
    {
        const auto* const data =
            reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
        PhysicalInputGuardState* const state =
            gs_physicalInputGuard.load(std::memory_order_acquire);
        if ( state && data &&
             (data->dwExtraInfo != PhysicalProbeInputTag ||
              (data->flags & LLKHF_INJECTED) == 0) )
        {
            state->Record(
                2, wParam, data->flags, POINT{},
                data->vkCode, data->scanCode, data->time,
                data->dwExtraInfo);
        }
    }

    return ::CallNextHookEx(nullptr, code, wParam, lParam);
}

class PhysicalInputGuard
{
public:
    bool Install()
    {
        PhysicalInputGuardState* expected = nullptr;
        if ( !gs_physicalInputGuard.compare_exchange_strong(
                expected, &m_state, std::memory_order_acq_rel) )
        {
            return false;
        }

        const HINSTANCE module = ::GetModuleHandleW(nullptr);
        m_mouseHook = ::SetWindowsHookExW(
            WH_MOUSE_LL, PhysicalMouseHook, module, 0);
        m_keyboardHook = ::SetWindowsHookExW(
            WH_KEYBOARD_LL, PhysicalKeyboardHook, module, 0);
        if ( !m_mouseHook || !m_keyboardHook )
        {
            Remove();
            return false;
        }
        return true;
    }

    ~PhysicalInputGuard()
    {
        Remove();
    }

    bool WasInterrupted() const
    {
        return m_state.interrupted.load(std::memory_order_acquire);
    }

    void PrintInterruption(const char* stage) const
    {
        if ( !WasInterrupted() )
            return;

        const unsigned inputKind =
            m_state.kind.load(std::memory_order_relaxed);
        const DWORD inputFlags =
            m_state.flags.load(std::memory_order_relaxed);
        const bool injected =
            inputKind == 1
                ? (inputFlags & LLMHF_INJECTED) != 0
                : (inputFlags & LLKHF_INJECTED) != 0;
        std::printf(
            "wx_winui_ole_delivery: {\"event\":\"input-interruption\","
            "\"stage\":\"%s\",\"kind\":\"%s\","
            "\"message\":%llu,\"flags\":%lu,\"injected\":%s,"
            "\"point\":[%ld,%ld],\"data\":[%lu,%lu],\"time\":%lu,"
            "\"extra_info\":\"0x%llx\"}\n",
            stage,
            inputKind == 1 ? "mouse" : "keyboard",
            static_cast<unsigned long long>(
                m_state.message.load(std::memory_order_relaxed)),
            static_cast<unsigned long>(inputFlags),
            injected ? "true" : "false",
            m_state.pointX.load(std::memory_order_relaxed),
            m_state.pointY.load(std::memory_order_relaxed),
            static_cast<unsigned long>(
                m_state.data1.load(std::memory_order_relaxed)),
            static_cast<unsigned long>(
                m_state.data2.load(std::memory_order_relaxed)),
            static_cast<unsigned long>(
                m_state.time.load(std::memory_order_relaxed)),
            static_cast<unsigned long long>(
                m_state.extraInfo.load(std::memory_order_relaxed)));
        std::fflush(stdout);
    }

    void PrintCleanStatus() const
    {
        if ( WasInterrupted() )
            return;

        std::printf(
            "wx_winui_ole_delivery: {\"event\":\"input-guard\","
            "\"result\":\"clean\"}\n");
        std::fflush(stdout);
    }

private:
    void Remove()
    {
        // Stop callbacks from publishing into this object before unhooking.
        PhysicalInputGuardState* expected = &m_state;
        (void)gs_physicalInputGuard.compare_exchange_strong(
            expected, nullptr, std::memory_order_acq_rel);

        if ( m_keyboardHook )
        {
            ::UnhookWindowsHookEx(m_keyboardHook);
            m_keyboardHook = nullptr;
        }
        if ( m_mouseHook )
        {
            ::UnhookWindowsHookEx(m_mouseHook);
            m_mouseHook = nullptr;
        }
    }

    PhysicalInputGuardState m_state;
    HHOOK m_mouseHook = nullptr;
    HHOOK m_keyboardHook = nullptr;
};

bool InjectPhysicalProbePoint(POINT point)
{
    const int left = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int top = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int width = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int height = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if ( width <= 1 || height <= 1 )
        return false;

    const auto normalize = [](LONG value, LONG origin, LONG extent) -> LONG
    {
        const long long scaled =
            static_cast<long long>(value - origin) * 65535LL;
        return static_cast<LONG>(scaled / (extent - 1));
    };

    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dx = normalize(point.x, left, width);
    input.mi.dy = normalize(point.y, top, height);
    input.mi.dwFlags =
        MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
    input.mi.dwExtraInfo = PhysicalProbeInputTag;
    return ::SendInput(1, &input, sizeof(input)) == 1;
}

bool InjectPhysicalProbeLeftButton(bool down)
{
    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = down ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
    input.mi.dwExtraInfo = PhysicalProbeInputTag;
    return ::SendInput(1, &input, sizeof(input)) == 1;
}

class DeliveryProbeDataObject final : public IDataObject
{
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override
    {
        if ( !object )
            return E_POINTER;
        *object = nullptr;

        if ( iid == IID_IUnknown || iid == IID_IDataObject )
        {
            *object = static_cast<IDataObject*>(this);
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

    HRESULT STDMETHODCALLTYPE GetData(
        FORMATETC* format, STGMEDIUM* medium) override
    {
        if ( !format || !medium )
            return E_INVALIDARG;

        const HRESULT query = QueryGetData(format);
        if ( FAILED(query) )
            return query;

        static constexpr wchar_t Payload[] =
            L"wxWinUI OLE delivery probe";
        const SIZE_T bytes = sizeof(Payload);
        HGLOBAL storage = ::GlobalAlloc(GMEM_MOVEABLE, bytes);
        if ( !storage )
            return STG_E_MEDIUMFULL;

        void* const destination = ::GlobalLock(storage);
        if ( !destination )
        {
            ::GlobalFree(storage);
            return STG_E_MEDIUMFULL;
        }
        std::memcpy(destination, Payload, bytes);
        ::GlobalUnlock(storage);

        medium->tymed = TYMED_HGLOBAL;
        medium->hGlobal = storage;
        medium->pUnkForRelease = nullptr;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetDataHere(
        FORMATETC*, STGMEDIUM*) override
    {
        return DATA_E_FORMATETC;
    }

    HRESULT STDMETHODCALLTYPE QueryGetData(FORMATETC* format) override
    {
        if ( !format )
            return E_INVALIDARG;
        if ( format->cfFormat != CF_UNICODETEXT )
            return DV_E_FORMATETC;
        if ( format->dwAspect != DVASPECT_CONTENT )
            return DV_E_DVASPECT;
        if ( format->lindex != -1 )
            return DV_E_LINDEX;
        if ( !(format->tymed & TYMED_HGLOBAL) )
            return DV_E_TYMED;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(
        FORMATETC*, FORMATETC* output) override
    {
        if ( !output )
            return E_INVALIDARG;
        output->ptd = nullptr;
        return DATA_S_SAMEFORMATETC;
    }

    HRESULT STDMETHODCALLTYPE SetData(
        FORMATETC*, STGMEDIUM*, BOOL) override
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE EnumFormatEtc(
        DWORD, IEnumFORMATETC** enumerator) override
    {
        if ( enumerator )
            *enumerator = nullptr;
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE DAdvise(
        FORMATETC*, DWORD, IAdviseSink*, DWORD*) override
    {
        return OLE_E_ADVISENOTSUPPORTED;
    }

    HRESULT STDMETHODCALLTYPE DUnadvise(DWORD) override
    {
        return OLE_E_ADVISENOTSUPPORTED;
    }

    HRESULT STDMETHODCALLTYPE EnumDAdvise(IEnumSTATDATA** enumerator) override
    {
        if ( enumerator )
            *enumerator = nullptr;
        return OLE_E_ADVISENOTSUPPORTED;
    }

private:
    ~DeliveryProbeDataObject() = default;

    LONG m_references = 1;
};

struct DeliveryProbeControl
{
    std::atomic<bool> cancel{ false };
    std::atomic<bool> requestDrop{ false };
    std::atomic<bool> dragStarted{ false };
    std::atomic<bool> driverFinished{ false };
    std::atomic<bool> injectionFailed{ false };
    std::atomic<bool> timedOut{ false };
    std::atomic<bool> leftButtonHeld{ false };
    std::atomic<unsigned> queryContinueCount{ 0 };
    std::atomic<DWORD> lastKeyState{ 0 };
    const PhysicalInputGuard* inputGuard = nullptr;
};

bool ReleasePhysicalProbeLeftButton(DeliveryProbeControl& control)
{
    bool held = true;
    if ( !control.leftButtonHeld.compare_exchange_strong(
            held, false, std::memory_order_acq_rel) )
    {
        return true;
    }

    if ( InjectPhysicalProbeLeftButton(false) )
        return true;

    // Retain the held state so the caller can make one final best-effort
    // release after the drag driver has joined.
    control.leftButtonHeld.store(true, std::memory_order_release);
    control.injectionFailed.store(true, std::memory_order_release);
    return false;
}

class DeliveryProbeSource final : public IDropSource
{
public:
    explicit DeliveryProbeSource(DeliveryProbeControl* control)
        : m_control(control)
    {
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override
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
        if ( m_control )
        {
            m_control->queryContinueCount.fetch_add(
                1, std::memory_order_acq_rel);
            m_control->lastKeyState.store(
                keyState, std::memory_order_release);
        }
        if ( escapePressed || !m_control ||
             m_control->cancel.load(std::memory_order_acquire) ||
             (m_control->inputGuard &&
              m_control->inputGuard->WasInterrupted()) )
        {
            return DRAGDROP_S_CANCEL;
        }

        if ( m_control->requestDrop.load(std::memory_order_acquire) )
            return DRAGDROP_S_DROP;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD) override
    {
        // Keep the user's existing cursor instead of briefly substituting
        // OLE's copy/no-drop artwork during this diagnostic.
        return S_OK;
    }

private:
    ~DeliveryProbeSource() = default;

    LONG m_references = 1;
    DeliveryProbeControl* m_control = nullptr;
};

class DeliveryProbeTarget final : public IDropTarget
{
public:
    explicit DeliveryProbeTarget(DWORD expectedThread)
        : m_expectedThread(expectedThread)
    {
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override
    {
        if ( !object )
            return E_POINTER;
        *object = nullptr;

        if ( iid == IID_IUnknown || iid == IID_IDropTarget )
        {
            *object = static_cast<IDropTarget*>(this);
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

    HRESULT STDMETHODCALLTYPE DragEnter(
        IDataObject* data, DWORD, POINTL, DWORD* effect) override
    {
        NoteThread();
        m_enter.fetch_add(1, std::memory_order_acq_rel);
        if ( ValidateData(data) )
            m_enterDataValid.store(true, std::memory_order_release);
        SetCopyEffect(effect);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragOver(
        DWORD, POINTL, DWORD* effect) override
    {
        NoteThread();
        m_over.fetch_add(1, std::memory_order_acq_rel);
        SetCopyEffect(effect);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragLeave() override
    {
        NoteThread();
        m_leave.fetch_add(1, std::memory_order_acq_rel);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Drop(
        IDataObject* data, DWORD, POINTL, DWORD* effect) override
    {
        NoteThread();
        m_drop.fetch_add(1, std::memory_order_acq_rel);
        if ( ValidateData(data) )
            m_dropDataValid.store(true, std::memory_order_release);
        SetCopyEffect(effect);
        return S_OK;
    }

    unsigned EnterCount() const
        { return m_enter.load(std::memory_order_acquire); }
    unsigned OverCount() const
        { return m_over.load(std::memory_order_acquire); }
    unsigned LeaveCount() const
        { return m_leave.load(std::memory_order_acquire); }
    unsigned DropCount() const
        { return m_drop.load(std::memory_order_acquire); }
    bool EnterDataWasValid() const
        { return m_enterDataValid.load(std::memory_order_acquire); }
    bool DropDataWasValid() const
        { return m_dropDataValid.load(std::memory_order_acquire); }
    bool WrongThread() const
        { return m_wrongThread.load(std::memory_order_acquire); }
    DWORD LastEffect() const
        { return m_lastEffect.load(std::memory_order_acquire); }

private:
    ~DeliveryProbeTarget() = default;

    void NoteThread()
    {
        if ( ::GetCurrentThreadId() != m_expectedThread )
            m_wrongThread.store(true, std::memory_order_release);
    }

    void SetCopyEffect(DWORD* effect)
    {
        if ( !effect )
            return;
        *effect = (*effect & DROPEFFECT_COPY)
                    ? DROPEFFECT_COPY
                    : DROPEFFECT_NONE;
        m_lastEffect.store(*effect, std::memory_order_release);
    }

    static bool ValidateData(IDataObject* data)
    {
        if ( !data )
            return false;

        FORMATETC format{};
        format.cfFormat = CF_UNICODETEXT;
        format.dwAspect = DVASPECT_CONTENT;
        format.lindex = -1;
        format.tymed = TYMED_HGLOBAL;
        if ( data->QueryGetData(&format) != S_OK )
            return false;

        STGMEDIUM medium{};
        if ( FAILED(data->GetData(&format, &medium)) )
            return false;

        bool valid = false;
        if ( medium.tymed == TYMED_HGLOBAL && medium.hGlobal )
        {
            const auto* const text = static_cast<const wchar_t*>(
                ::GlobalLock(medium.hGlobal));
            if ( text )
            {
                valid =
                    std::wcscmp(text, L"wxWinUI OLE delivery probe") == 0;
                ::GlobalUnlock(medium.hGlobal);
            }
        }
        ::ReleaseStgMedium(&medium);
        return valid;
    }

    LONG m_references = 1;
    DWORD m_expectedThread = 0;
    std::atomic<unsigned> m_enter{ 0 };
    std::atomic<unsigned> m_over{ 0 };
    std::atomic<unsigned> m_leave{ 0 };
    std::atomic<unsigned> m_drop{ 0 };
    std::atomic<bool> m_enterDataValid{ false };
    std::atomic<bool> m_dropDataValid{ false };
    std::atomic<bool> m_wrongThread{ false };
    std::atomic<DWORD> m_lastEffect{ DROPEFFECT_NONE };
};

struct DeliveryRegistrationResult
{
    HRESULT lockHr = E_UNEXPECTED;
    HRESULT registerHr = E_UNEXPECTED;
    HRESULT revokeHr = E_UNEXPECTED;
    HRESULT unlockHr = E_UNEXPECTED;
    bool registerAttempted = false;
    bool revokeAttempted = false;
    bool unlockAttempted = false;
    bool ownsRegistration = false;
    bool locked = false;
};

class DeliveryTargetRegistration
{
public:
    DeliveryTargetRegistration(HWND hwnd, IDropTarget* target)
        : m_hwnd(hwnd),
          m_target(target)
    {
    }

    void Open()
    {
        m_result.lockHr =
            ::CoLockObjectExternal(m_target, TRUE, FALSE);
        if ( FAILED(m_result.lockHr) )
            return;
        m_result.locked = true;

        m_result.registerAttempted = true;
        m_result.registerHr = ::RegisterDragDrop(m_hwnd, m_target);
        if ( m_result.registerHr == S_OK )
        {
            m_result.ownsRegistration = true;
            return;
        }

        // The target was never ours if registration failed, especially for
        // DRAGDROP_E_ALREADYREGISTERED. Never call RevokeDragDrop in this
        // branch: the existing registration belongs to somebody else.
        m_result.unlockAttempted = true;
        m_result.unlockHr =
            ::CoLockObjectExternal(m_target, FALSE, FALSE);
        if ( SUCCEEDED(m_result.unlockHr) )
            m_result.locked = false;
    }

    void Close()
    {
        if ( m_closed )
            return;
        m_closed = true;

        if ( m_result.ownsRegistration )
        {
            m_result.revokeAttempted = true;
            m_result.revokeHr = ::RevokeDragDrop(m_hwnd);
            if ( SUCCEEDED(m_result.revokeHr) )
                m_result.ownsRegistration = false;
        }

        // If revocation failed, retain the external lock deliberately: the
        // native registration still owns and may call the target.
        if ( !m_result.ownsRegistration && m_result.locked )
        {
            m_result.unlockAttempted = true;
            m_result.unlockHr =
                ::CoLockObjectExternal(
                    m_target, FALSE,
                    m_result.registerHr == S_OK ? TRUE : FALSE);
            if ( SUCCEEDED(m_result.unlockHr) )
                m_result.locked = false;
        }
    }

    ~DeliveryTargetRegistration()
    {
        Close();
    }

    const DeliveryRegistrationResult& Result() const
    {
        return m_result;
    }

private:
    HWND m_hwnd = nullptr;
    IDropTarget* m_target = nullptr;
    DeliveryRegistrationResult m_result;
    bool m_closed = false;
};

struct OleRegistrationProbe
{
    const char *role = nullptr;
    HWND hwnd = nullptr;
    HRESULT lockResult = E_UNEXPECTED;
    HRESULT registerResult = E_UNEXPECTED;
    HRESULT revokeResult = E_UNEXPECTED;
    HRESULT revokeRetryResult = E_UNEXPECTED;
    HRESULT unlockResult = E_UNEXPECTED;
    bool registerAttempted = false;
    bool revokeAttempted = false;
    bool revokeRetryAttempted = false;
    bool ownsRegistration = false;
};

struct InputSiteSearch
{
    DWORD threadId = 0;
    HWND candidate = nullptr;
    unsigned matches = 0;
    bool report = false;
};

void ReportStage(const char* stage)
{
    gs_stage = stage;
    std::printf("wx_winui_runtime_smoke: %s\n", stage);
    std::fflush(stdout);
}

int FailWinRT(int code, const char* what, const winrt::hresult_error& e)
{
    std::fprintf(stderr,
                 "wx_winui_runtime_smoke: FAILED: %s: HRESULT 0x%08lx: %ls\n",
                 what,
                 static_cast<unsigned long>(e.code().value),
                 e.message().c_str());
    return code;
}

void PrintPayloadHint()
{
    wchar_t path[MAX_PATH]{};
    ::GetModuleFileNameW(nullptr, path, MAX_PATH);
    std::fprintf(stderr,
        "wx_winui_runtime_smoke: the unpackaged WinUI resource payload is "
        "expected next to the executable:\n"
        "  %ls\n"
        "  required: resources.pri (the Windows App Runtime framework "
        "resource index) and the Microsoft.UI.Xaml asset tree\n"
        "  (deployed by the wx_winui3_deploy_runtime() CMake helper)\n",
        path);
}

// Same shape as wxWinUIApplication in src/winui/winui.cpp: the application
// object must provide the XAML metadata for the WinUI controls, otherwise
// XamlControlsResources cannot be constructed at all, payload or not.
struct SmokeApplication :
    winrt::Microsoft::UI::Xaml::ApplicationT
    <
        SmokeApplication,
        winrt::Microsoft::UI::Xaml::Markup::IXamlMetadataProvider
    >
{
    SmokeApplication() = default;

    void OnLaunched(
        winrt::Microsoft::UI::Xaml::LaunchActivatedEventArgs const&)
    {
    }

    winrt::Microsoft::UI::Xaml::Markup::IXamlType GetXamlType(
        winrt::Windows::UI::Xaml::Interop::TypeName const& type)
    {
        return m_provider.GetXamlType(type);
    }

    winrt::Microsoft::UI::Xaml::Markup::IXamlType GetXamlType(
        winrt::hstring const& fullName)
    {
        return m_provider.GetXamlType(fullName);
    }

    winrt::com_array<winrt::Microsoft::UI::Xaml::Markup::XmlnsDefinition>
    GetXmlnsDefinitions()
    {
        return m_provider.GetXmlnsDefinitions();
    }

private:
    winrt::Microsoft::UI::Xaml::XamlTypeInfo::XamlControlsXamlMetaDataProvider
        m_provider;
};

LRESULT CALLBACK SmokeWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return ::DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Pump the message loop until the condition holds or the deadline passes.
template <typename Pred>
bool PumpUntil(Pred pred, DWORD timeoutMs)
{
    const ULONGLONG deadline = ::GetTickCount64() + timeoutMs;
    for ( ;; )
    {
        MSG msg;
        while ( ::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE) )
        {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        }

        if ( pred() )
            return true;
        if ( ::GetTickCount64() >= deadline )
            return false;

        ::MsgWaitForMultipleObjectsEx(0, nullptr, 50, QS_ALLINPUT,
                                      MWMO_INPUTAVAILABLE);
    }
}

OleRegistrationProbe ProbeOleRegistration(const char *role, HWND hwnd)
{
    OleRegistrationProbe result;
    result.role = role;
    result.hwnd = hwnd;

    auto * const target = new OleDropProbeTarget;
    result.lockResult = ::CoLockObjectExternal(target, TRUE, FALSE);
    if ( SUCCEEDED(result.lockResult) )
    {
        result.registerAttempted = true;
        result.registerResult = ::RegisterDragDrop(hwnd, target);
        if ( result.registerResult == S_OK )
        {
            result.ownsRegistration = true;
            result.revokeAttempted = true;
            result.revokeResult = ::RevokeDragDrop(hwnd);
            if ( FAILED(result.revokeResult) )
            {
                result.revokeRetryAttempted = true;
                result.revokeRetryResult = ::RevokeDragDrop(hwnd);
            }

            if ( result.revokeResult == S_OK ||
                 result.revokeRetryResult == S_OK )
            {
                result.ownsRegistration = false;
                result.unlockResult =
                    ::CoLockObjectExternal(target, FALSE, TRUE);
            }
        }
        else
        {
            // Mirror the RegisterDragDrop failure path used by wxDropTarget:
            // this object was never externally registered.
            result.unlockResult =
                ::CoLockObjectExternal(target, FALSE, FALSE);
        }
    }
    target->Release();

    wchar_t className[128] = L"";
    (void)::GetClassNameW(hwnd, className, ARRAYSIZE(className));
    std::printf(
        "wx_winui_runtime_smoke: OLE probe role=%s hwnd=%p class=\"%ls\" "
        "thread=%lu lock=0x%08lx register=%s0x%08lx "
        "revoke=%s0x%08lx retry=%s0x%08lx unlock=%s0x%08lx "
        "owns=%d\n",
        role,
        static_cast<void *>(hwnd),
        className,
        ::GetWindowThreadProcessId(hwnd, nullptr),
        static_cast<unsigned long>(result.lockResult),
        result.registerAttempted ? "" : "not-called/",
        static_cast<unsigned long>(result.registerResult),
        result.revokeAttempted ? "" : "not-called/",
        static_cast<unsigned long>(result.revokeResult),
        result.revokeRetryAttempted ? "" : "not-called/",
        static_cast<unsigned long>(result.revokeRetryResult),
        result.ownsRegistration ? "held/" : "",
        static_cast<unsigned long>(result.unlockResult),
        int(result.ownsRegistration));
    std::fflush(stdout);
    return result;
}

bool OleRegistrationWasExact(const OleRegistrationProbe& result)
{
    return SUCCEEDED(result.lockResult) &&
           result.registerResult == S_OK &&
           result.revokeAttempted &&
           (result.revokeResult == S_OK ||
            result.revokeRetryResult == S_OK) &&
           !result.ownsRegistration &&
           SUCCEEDED(result.unlockResult);
}

BOOL CALLBACK FindInputSiteWindow(HWND hwnd, LPARAM parameter)
{
    auto& search = *reinterpret_cast<InputSiteSearch *>(parameter);
    const DWORD threadId = ::GetWindowThreadProcessId(hwnd, nullptr);

    wchar_t className[128] = L"";
    (void)::GetClassNameW(hwnd, className, ARRAYSIZE(className));
    if ( search.report )
    {
        std::printf(
            "wx_winui_runtime_smoke: OLE probe descendant hwnd=%p "
            "class=\"%ls\" thread=%lu\n",
            static_cast<void *>(hwnd),
            className,
            threadId);
    }

    if ( threadId == search.threadId &&
         std::wcsstr(className, L"InputSite") )
    {
        search.candidate = hwnd;
        ++search.matches;
    }
    return TRUE;
}

bool IsSameWindowOrDescendant(HWND window, HWND ancestor)
{
    for ( HWND current = window; current; current = ::GetParent(current) )
    {
        if ( current == ancestor )
            return true;
    }
    return false;
}

unsigned long long HwndAsInteger(HWND hwnd)
{
    return static_cast<unsigned long long>(
        reinterpret_cast<std::uintptr_t>(hwnd));
}

const char* JsonBool(bool value)
{
    return value ? "true" : "false";
}

struct DeliveryArmResult
{
    const char* role = nullptr;
    const char* surface = nullptr;
    HWND registrationHwnd = nullptr;
    HWND pointHwnd = nullptr;
    DeliveryRegistrationResult registration;
    HRESULT dragHr = E_UNEXPECTED;
    DWORD finalEffect = DROPEFFECT_NONE;
    unsigned enter = 0;
    unsigned over = 0;
    unsigned leave = 0;
    unsigned drop = 0;
    bool enterDataValid = false;
    bool dropDataValid = false;
    bool wrongThread = false;
    DWORD callbackEffect = DROPEFFECT_NONE;
    bool inputInterrupted = false;
    bool injectionFailed = false;
    bool timedOut = false;
    bool buttonDownObserved = false;
    bool buttonUpObserved = false;
    unsigned queryContinueCount = 0;
    DWORD lastKeyState = 0;
    bool attemptedDrag = false;
    bool delivered = false;
    bool cleanMiss = false;
    bool partialDelivery = false;
};

void PrintDeliveryRegistration(
    const char* event,
    const char* role,
    HWND hwnd,
    const DeliveryRegistrationResult& registration)
{
    std::printf(
        "wx_winui_ole_delivery: {\"event\":\"%s\",\"role\":\"%s\","
        "\"hwnd\":\"0x%llx\",\"lock_hr\":\"0x%08lx\","
        "\"register_attempted\":%s,\"register_hr\":\"0x%08lx\","
        "\"revoke_attempted\":%s,\"revoke_hr\":\"0x%08lx\","
        "\"unlock_attempted\":%s,\"unlock_hr\":\"0x%08lx\","
        "\"owns_registration\":%s,\"locked\":%s}\n",
        event,
        role,
        HwndAsInteger(hwnd),
        static_cast<unsigned long>(registration.lockHr),
        JsonBool(registration.registerAttempted),
        static_cast<unsigned long>(registration.registerHr),
        JsonBool(registration.revokeAttempted),
        static_cast<unsigned long>(registration.revokeHr),
        JsonBool(registration.unlockAttempted),
        static_cast<unsigned long>(registration.unlockHr),
        JsonBool(registration.ownsRegistration),
        JsonBool(registration.locked));
    std::fflush(stdout);
}

void PrintDeliveryArm(const DeliveryArmResult& result)
{
    std::printf(
        "wx_winui_ole_delivery: {\"event\":\"arm\","
        "\"role\":\"%s\",\"surface\":\"%s\","
        "\"registration_hwnd\":\"0x%llx\","
        "\"point_hwnd\":\"0x%llx\","
        "\"lock_hr\":\"0x%08lx\",\"register_hr\":\"0x%08lx\","
        "\"revoke_hr\":\"0x%08lx\",\"unlock_hr\":\"0x%08lx\","
        "\"attempted_drag\":%s,\"drag_hr\":\"0x%08lx\","
        "\"effect\":%lu,\"enter\":%u,\"over\":%u,"
        "\"leave\":%u,\"drop\":%u,\"enter_data_valid\":%s,"
        "\"drop_data_valid\":%s,\"callback_effect\":%lu,"
        "\"wrong_thread\":%s,\"input_interrupted\":%s,"
        "\"injection_failed\":%s,\"timed_out\":%s,"
        "\"button_down_observed\":%s,\"button_up_observed\":%s,"
        "\"query_continue_count\":%u,\"last_key_state\":%lu,"
        "\"delivered\":%s,"
        "\"clean_miss\":%s,\"partial_delivery\":%s,"
        "\"owns_registration\":%s,\"locked\":%s}\n",
        result.role,
        result.surface,
        HwndAsInteger(result.registrationHwnd),
        HwndAsInteger(result.pointHwnd),
        static_cast<unsigned long>(result.registration.lockHr),
        static_cast<unsigned long>(result.registration.registerHr),
        static_cast<unsigned long>(result.registration.revokeHr),
        static_cast<unsigned long>(result.registration.unlockHr),
        JsonBool(result.attemptedDrag),
        static_cast<unsigned long>(result.dragHr),
        static_cast<unsigned long>(result.finalEffect),
        result.enter,
        result.over,
        result.leave,
        result.drop,
        JsonBool(result.enterDataValid),
        JsonBool(result.dropDataValid),
        static_cast<unsigned long>(result.callbackEffect),
        JsonBool(result.wrongThread),
        JsonBool(result.inputInterrupted),
        JsonBool(result.injectionFailed),
        JsonBool(result.timedOut),
        JsonBool(result.buttonDownObserved),
        JsonBool(result.buttonUpObserved),
        result.queryContinueCount,
        static_cast<unsigned long>(result.lastKeyState),
        JsonBool(result.delivered),
        JsonBool(result.cleanMiss),
        JsonBool(result.partialDelivery),
        JsonBool(result.registration.ownsRegistration),
        JsonBool(result.registration.locked));
    std::fflush(stdout);
}

int ReportDeliveryTerminal(
    const char* result, const char* reason, int exitCode)
{
    std::fprintf(
        exitCode == 0 ? stdout : stderr,
        "wx_winui_ole_delivery: {\"event\":\"summary\","
        "\"result\":\"%s\",\"reason\":\"%s\","
        "\"recommendation\":\"none\",\"exit_code\":%d}\n",
        result,
        reason,
        exitCode);
    std::fflush(exitCode == 0 ? stdout : stderr);
    return exitCode;
}

DeliveryRegistrationResult PreflightDeliveryRegistration(
    const char* role, HWND hwnd)
{
    auto* const target = new DeliveryProbeTarget(::GetCurrentThreadId());
    DeliveryTargetRegistration registration(hwnd, target);
    registration.Open();
    registration.Close();
    const DeliveryRegistrationResult result = registration.Result();
    target->Release();
    PrintDeliveryRegistration("preflight", role, hwnd, result);
    return result;
}

bool DeliveryRegistrationCleaned(
    const DeliveryRegistrationResult& registration)
{
    if ( registration.registerHr == S_OK )
    {
        return registration.revokeAttempted &&
               registration.revokeHr == S_OK &&
               registration.unlockAttempted &&
               registration.unlockHr == S_OK &&
               !registration.ownsRegistration &&
               !registration.locked;
    }

    if ( SUCCEEDED(registration.lockHr) )
    {
        return registration.unlockAttempted &&
               registration.unlockHr == S_OK &&
               !registration.locked;
    }

    return true;
}

void WakeDeliveryLoop(DWORD uiThread)
{
    // These are private queue messages, not physical keyboard input. They
    // merely make OLE ask IDropSource::QueryContinueDrag() again after the
    // shared cancel flag has been published.
    (void)::PostThreadMessageW(uiThread, WM_KEYDOWN, VK_ESCAPE, 0);
    (void)::PostThreadMessageW(uiThread, WM_KEYUP, VK_ESCAPE, 0);
}

[[noreturn]] void AbortHungDeliveryDrag()
{
    std::fprintf(
        stderr,
        "wx_winui_ole_delivery: {\"event\":\"summary\","
        "\"result\":\"failed\","
        "\"reason\":\"do-drag-drop-cancel-grace\","
        "\"recommendation\":\"none\",\"hard_timeout\":true,"
        "\"stage\":\"%s\","
        "\"action\":\"capture-hang-dump-and-investigate-ole-loop\","
        "\"cancel_grace_ms\":%lu,\"exit_code\":20}\n",
        gs_stage.load(std::memory_order_acquire),
        static_cast<unsigned long>(PhysicalProbeCancelGraceMs));
    std::fflush(stderr);

    // DoDragDrop still owns the source and data objects and may still be using
    // the stack-owned control block. Joining the driver, releasing COM objects
    // or unwinding WinUI would therefore be unsafe. This explicitly armed,
    // standalone diagnostic is never a CTest, so terminate its process after
    // the grace period instead of waiting for the unrelated 120 s watchdog.
    ::ExitProcess(20);
    std::abort();
}

POINT MakeNearbyPoint(POINT point, const RECT& bounds)
{
    constexpr LONG Delta = 2;
    if ( point.x + Delta < bounds.right )
        point.x += Delta;
    else
        point.x -= Delta;
    if ( point.y + Delta < bounds.bottom )
        point.y += Delta;
    else
        point.y -= Delta;
    return point;
}

bool PointsAreClose(POINT first, POINT second, LONG tolerance = 3)
{
    return std::abs(first.x - second.x) <= tolerance &&
           std::abs(first.y - second.y) <= tolerance;
}

struct DeliveryDragResult
{
    HRESULT dragHr = E_UNEXPECTED;
    DWORD finalEffect = DROPEFFECT_NONE;
    bool attempted = false;
    bool inputInterrupted = false;
    bool injectionFailed = false;
    bool timedOut = false;
    bool buttonDownObserved = false;
    bool buttonUpObserved = false;
    unsigned queryContinueCount = 0;
    DWORD lastKeyState = 0;
};

DeliveryDragResult RunDeliveryDrag(
    DeliveryProbeTarget* expectedTarget,
    POINT point,
    POINT nearbyPoint,
    POINT departurePoint,
    const PhysicalInputGuard& inputGuard)
{
    DeliveryDragResult result;
    if ( !expectedTarget )
        return result;

    const HWND departureHwnd = ::WindowFromPoint(departurePoint);
    const HWND pointHwnd = ::WindowFromPoint(point);
    const HWND nearbyHwnd = ::WindowFromPoint(nearbyPoint);
    const auto printGesture =
        [&](const char* gestureResult, const char* reason)
        {
            std::printf(
                "wx_winui_ole_delivery: {\"event\":\"drag-gesture\","
                "\"result\":\"%s\",\"reason\":\"%s\","
                "\"departure\":[%ld,%ld],\"departure_hwnd\":\"0x%llx\","
                "\"target\":[%ld,%ld],\"target_hwnd\":\"0x%llx\","
                "\"wake\":[%ld,%ld],\"wake_hwnd\":\"0x%llx\","
                "\"left_button\":true}\n",
                gestureResult,
                reason,
                departurePoint.x,
                departurePoint.y,
                HwndAsInteger(departureHwnd),
                point.x,
                point.y,
                HwndAsInteger(pointHwnd),
                nearbyPoint.x,
                nearbyPoint.y,
                HwndAsInteger(nearbyHwnd));
            std::fflush(stdout);
        };

    if ( !departureHwnd || !pointHwnd || departureHwnd == pointHwnd )
    {
        result.injectionFailed = true;
        printGesture("failed", "invalid-hwnd-transition");
        return result;
    }
    if ( !InjectPhysicalProbePoint(departurePoint) )
    {
        result.injectionFailed = true;
        printGesture("failed", "departure-injection");
        return result;
    }
    const bool departureReached = PumpUntil(
        [&]()
        {
            POINT current{};
            return ::GetCursorPos(&current) &&
                   PointsAreClose(current, departurePoint) &&
                   ::WindowFromPoint(current) == departureHwnd;
        },
        1000);
    if ( !departureReached )
    {
        result.injectionFailed = true;
        printGesture("failed", "departure-not-reached");
        return result;
    }
    if ( inputGuard.WasInterrupted() )
    {
        result.inputInterrupted = true;
        printGesture("failed", "user-interrupted-before-button");
        return result;
    }

    const DWORD uiThread = ::GetCurrentThreadId();
    DeliveryProbeControl control;
    control.inputGuard = &inputGuard;
    auto* const data = new DeliveryProbeDataObject;
    auto* const source = new DeliveryProbeSource(&control);

    std::thread driver(
        [&control, expectedTarget, point, nearbyPoint, uiThread]()
        {
            const auto cancelAndWakeUntilFinished =
                [&control, uiThread]()
                {
                    (void)ReleasePhysicalProbeLeftButton(control);
                    control.cancel.store(true, std::memory_order_release);
                    const ULONGLONG cancelDeadline =
                        ::GetTickCount64() + PhysicalProbeCancelGraceMs;
                    while ( !control.driverFinished.load(
                                std::memory_order_acquire) )
                    {
                        if ( ::GetTickCount64() >= cancelDeadline )
                        {
                            (void)ReleasePhysicalProbeLeftButton(control);
                            AbortHungDeliveryDrag();
                        }

                        WakeDeliveryLoop(uiThread);
                        ::Sleep(PhysicalProbeCancelWakeIntervalMs);
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
            if ( control.driverFinished.load(std::memory_order_acquire) )
                return;

            if ( control.inputGuard->WasInterrupted() )
            {
                cancelAndWakeUntilFinished();
                return;
            }

            // Cross from the probe-owned source pad into the candidate HWND.
            // This is the event that makes OLE resolve the registered target
            // and call IDropTarget::DragEnter().
            if ( !InjectPhysicalProbePoint(point) )
            {
                control.injectionFailed.store(
                    true, std::memory_order_release);
                cancelAndWakeUntilFinished();
                return;
            }

            // The arm deadline starts only after the first physical sample
            // was accepted. On expiry, keep publishing cancel and waking
            // OLE's nested message loop until DoDragDrop has actually
            // returned. Cooperative cancellation retains normal lifetime
            // cleanup; a broken OLE loop is bounded by the hard grace timeout.
            const ULONGLONG deadline =
                ::GetTickCount64() + PhysicalProbeArmTimeoutMs;
            while ( expectedTarget->EnterCount() == 0 &&
                    !control.driverFinished.load(
                        std::memory_order_acquire) &&
                    !control.inputGuard->WasInterrupted() &&
                    ::GetTickCount64() < deadline )
            {
                ::Sleep(10);
            }

            if ( control.driverFinished.load(std::memory_order_acquire) )
                return;

            if ( control.inputGuard->WasInterrupted() )
            {
                cancelAndWakeUntilFinished();
                return;
            }

            if ( expectedTarget->EnterCount() == 0 )
            {
                control.timedOut.store(true, std::memory_order_release);
                cancelAndWakeUntilFinished();
                return;
            }

            // Keep the release on the same target surface, but give OLE a
            // distinct motion sample before the initiating button goes up.
            if ( !InjectPhysicalProbePoint(nearbyPoint) )
            {
                control.injectionFailed.store(
                    true, std::memory_order_release);
                cancelAndWakeUntilFinished();
                return;
            }
            control.requestDrop.store(true, std::memory_order_release);
            if ( !ReleasePhysicalProbeLeftButton(control) )
            {
                cancelAndWakeUntilFinished();
                return;
            }

            while ( !control.driverFinished.load(
                        std::memory_order_acquire) )
            {
                if ( control.inputGuard->WasInterrupted() )
                {
                    cancelAndWakeUntilFinished();
                    return;
                }
                if ( ::GetTickCount64() >= deadline )
                {
                    control.timedOut.store(
                        true, std::memory_order_release);
                    cancelAndWakeUntilFinished();
                    return;
                }
                ::Sleep(10);
            }
        });

    if ( !InjectPhysicalProbeLeftButton(true) )
    {
        control.injectionFailed.store(true, std::memory_order_release);
        control.driverFinished.store(true, std::memory_order_release);
        driver.join();
        result.injectionFailed = true;
        source->Release();
        data->Release();
        printGesture("failed", "left-button-down");
        return result;
    }
    control.leftButtonHeld.store(true, std::memory_order_release);
    const bool buttonDownObserved = PumpUntil(
        []()
        {
            return (::GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        },
        250);
    if ( !buttonDownObserved || inputGuard.WasInterrupted() )
    {
        if ( !buttonDownObserved )
            control.injectionFailed.store(true, std::memory_order_release);
        (void)ReleasePhysicalProbeLeftButton(control);
        control.driverFinished.store(true, std::memory_order_release);
        driver.join();
        result.inputInterrupted = inputGuard.WasInterrupted();
        result.injectionFailed =
            control.injectionFailed.load(std::memory_order_acquire);
        result.buttonDownObserved = buttonDownObserved;
        result.buttonUpObserved =
            (::GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0;
        source->Release();
        data->Release();
        printGesture(
            "failed",
            inputGuard.WasInterrupted()
                ? "user-interrupted-after-button"
                : "left-button-down-not-observed");
        return result;
    }
    control.dragStarted.store(true, std::memory_order_release);
    printGesture("armed", "probe-owned-departure-ready");

    result.attempted = true;
    result.dragHr =
        ::DoDragDrop(data, source, DROPEFFECT_COPY, &result.finalEffect);
    control.driverFinished.store(true, std::memory_order_release);
    driver.join();
    (void)ReleasePhysicalProbeLeftButton(control);
    bool buttonUpObserved =
        (::GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0;
    if ( !buttonUpObserved && !inputGuard.WasInterrupted() )
    {
        buttonUpObserved = PumpUntil(
            []()
            {
                return (::GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0;
            },
            250);
    }
    if ( !buttonUpObserved && !inputGuard.WasInterrupted() )
        control.injectionFailed.store(true, std::memory_order_release);

    // Leave the cursor where the user had parked it. If a real input raced
    // with us, the hook wins and we deliberately do not pull the cursor back
    // out from under the user.
    POINT cursorAfter{};
    if ( !inputGuard.WasInterrupted() &&
         ::GetCursorPos(&cursorAfter) &&
         (PointsAreClose(cursorAfter, point) ||
          PointsAreClose(cursorAfter, nearbyPoint) ||
          PointsAreClose(cursorAfter, departurePoint)) &&
         !InjectPhysicalProbePoint(point) )
    {
        control.injectionFailed.store(true, std::memory_order_release);
    }

    result.inputInterrupted = inputGuard.WasInterrupted();
    result.injectionFailed =
        control.injectionFailed.load(std::memory_order_acquire);
    result.timedOut = control.timedOut.load(std::memory_order_acquire);
    result.buttonDownObserved = buttonDownObserved;
    result.buttonUpObserved = buttonUpObserved;
    result.queryContinueCount =
        control.queryContinueCount.load(std::memory_order_acquire);
    result.lastKeyState =
        control.lastKeyState.load(std::memory_order_acquire);

    source->Release();
    data->Release();
    return result;
}

void CaptureDeliveryArm(
    DeliveryArmResult& result,
    const DeliveryProbeTarget& target,
    const DeliveryDragResult& drag)
{
    result.dragHr = drag.dragHr;
    result.finalEffect = drag.finalEffect;
    result.attemptedDrag = drag.attempted;
    result.inputInterrupted = drag.inputInterrupted;
    result.injectionFailed = drag.injectionFailed;
    result.timedOut = drag.timedOut;
    result.buttonDownObserved = drag.buttonDownObserved;
    result.buttonUpObserved = drag.buttonUpObserved;
    result.queryContinueCount = drag.queryContinueCount;
    result.lastKeyState = drag.lastKeyState;
    result.enter = target.EnterCount();
    result.over = target.OverCount();
    result.leave = target.LeaveCount();
    result.drop = target.DropCount();
    result.enterDataValid = target.EnterDataWasValid();
    result.dropDataValid = target.DropDataWasValid();
    result.wrongThread = target.WrongThread();
    result.callbackEffect = target.LastEffect();

    // A successful qualification is deliberately exact. Duplicate enters,
    // a terminal leave before/after Drop, partially readable data or an
    // effect observed only on one side all invalidate the arm.
    result.delivered =
        result.dragHr == DRAGDROP_S_DROP &&
        result.enter == 1 &&
        result.drop == 1 &&
        result.leave == 0 &&
        result.enterDataValid &&
        result.dropDataValid &&
        !result.wrongThread &&
        result.callbackEffect == DROPEFFECT_COPY &&
        result.finalEffect == DROPEFFECT_COPY;
    result.cleanMiss =
        result.dragHr == DRAGDROP_S_CANCEL &&
        result.enter == 0 &&
        result.over == 0 &&
        result.leave == 0 &&
        result.drop == 0;
    result.partialDelivery =
        !result.delivered && !result.cleanMiss &&
        !result.inputInterrupted && !result.injectionFailed &&
        !result.timedOut;
}

DeliveryArmResult RunDeliveryArm(
    const char* role,
    const char* surface,
    HWND registrationHwnd,
    POINT point,
    POINT nearbyPoint,
    POINT departurePoint,
    const PhysicalInputGuard& inputGuard)
{
    DeliveryArmResult result;
    result.role = role;
    result.surface = surface;
    result.registrationHwnd = registrationHwnd;
    result.pointHwnd = ::WindowFromPoint(point);

    const DWORD uiThread = ::GetCurrentThreadId();
    auto* const target = new DeliveryProbeTarget(uiThread);
    DeliveryTargetRegistration registration(registrationHwnd, target);
    registration.Open();

    if ( registration.Result().registerHr == S_OK )
    {
        const DeliveryDragResult drag =
            RunDeliveryDrag(
                target, point, nearbyPoint, departurePoint, inputGuard);
        CaptureDeliveryArm(result, *target, drag);
    }

    registration.Close();
    result.registration = registration.Result();
    target->Release();
    PrintDeliveryArm(result);
    return result;
}

struct CombinedDeliveryArmResult
{
    const char* surface = nullptr;
    const char* expectedRole = nullptr;
    DeliveryArmResult bridge;
    DeliveryArmResult tlw;
    bool delivered = false;
    bool partialDelivery = false;
    bool inputInterrupted = false;
    bool injectionFailed = false;
    bool timedOut = false;
};

bool DeliveryCallbacksAreEmpty(const DeliveryArmResult& result)
{
    return result.enter == 0 &&
           result.over == 0 &&
           result.leave == 0 &&
           result.drop == 0;
}

void PrintCombinedDeliveryArm(const CombinedDeliveryArmResult& result)
{
    std::printf(
        "wx_winui_ole_delivery: {\"event\":\"combined-arm\","
        "\"surface\":\"%s\",\"expected\":\"%s\","
        "\"point_hwnd\":\"0x%llx\","
        "\"bridge_lock_hr\":\"0x%08lx\","
        "\"bridge_register_hr\":\"0x%08lx\","
        "\"bridge_revoke_hr\":\"0x%08lx\","
        "\"bridge_unlock_hr\":\"0x%08lx\","
        "\"tlw_lock_hr\":\"0x%08lx\","
        "\"tlw_register_hr\":\"0x%08lx\","
        "\"tlw_revoke_hr\":\"0x%08lx\","
        "\"tlw_unlock_hr\":\"0x%08lx\","
        "\"drag_hr\":\"0x%08lx\",\"effect\":%lu,"
        "\"bridge_enter\":%u,\"bridge_over\":%u,"
        "\"bridge_leave\":%u,\"bridge_drop\":%u,"
        "\"bridge_enter_data_valid\":%s,"
        "\"bridge_drop_data_valid\":%s,"
        "\"bridge_callback_effect\":%lu,"
        "\"bridge_wrong_thread\":%s,"
        "\"tlw_enter\":%u,\"tlw_over\":%u,"
        "\"tlw_leave\":%u,\"tlw_drop\":%u,"
        "\"tlw_enter_data_valid\":%s,"
        "\"tlw_drop_data_valid\":%s,"
        "\"tlw_callback_effect\":%lu,"
        "\"tlw_wrong_thread\":%s,"
        "\"input_interrupted\":%s,\"injection_failed\":%s,"
        "\"timed_out\":%s,\"bridge_cleaned\":%s,"
        "\"tlw_cleaned\":%s,\"delivered\":%s}\n",
        result.surface,
        result.expectedRole,
        HwndAsInteger(result.bridge.pointHwnd),
        static_cast<unsigned long>(result.bridge.registration.lockHr),
        static_cast<unsigned long>(result.bridge.registration.registerHr),
        static_cast<unsigned long>(result.bridge.registration.revokeHr),
        static_cast<unsigned long>(result.bridge.registration.unlockHr),
        static_cast<unsigned long>(result.tlw.registration.lockHr),
        static_cast<unsigned long>(result.tlw.registration.registerHr),
        static_cast<unsigned long>(result.tlw.registration.revokeHr),
        static_cast<unsigned long>(result.tlw.registration.unlockHr),
        static_cast<unsigned long>(result.bridge.dragHr),
        static_cast<unsigned long>(result.bridge.finalEffect),
        result.bridge.enter,
        result.bridge.over,
        result.bridge.leave,
        result.bridge.drop,
        JsonBool(result.bridge.enterDataValid),
        JsonBool(result.bridge.dropDataValid),
        static_cast<unsigned long>(result.bridge.callbackEffect),
        JsonBool(result.bridge.wrongThread),
        result.tlw.enter,
        result.tlw.over,
        result.tlw.leave,
        result.tlw.drop,
        JsonBool(result.tlw.enterDataValid),
        JsonBool(result.tlw.dropDataValid),
        static_cast<unsigned long>(result.tlw.callbackEffect),
        JsonBool(result.tlw.wrongThread),
        JsonBool(result.inputInterrupted),
        JsonBool(result.injectionFailed),
        JsonBool(result.timedOut),
        JsonBool(DeliveryRegistrationCleaned(
            result.bridge.registration)),
        JsonBool(DeliveryRegistrationCleaned(
            result.tlw.registration)),
        JsonBool(result.delivered));
    std::fflush(stdout);
}

CombinedDeliveryArmResult RunCombinedDeliveryArm(
    const char* surface,
    HWND host,
    HWND bridge,
    bool expectBridge,
    POINT point,
    POINT nearbyPoint,
    POINT departurePoint,
    const PhysicalInputGuard& inputGuard)
{
    CombinedDeliveryArmResult result;
    result.surface = surface;
    result.expectedRole = expectBridge ? "bridge" : "tlw";

    result.bridge.role = "bridge";
    result.bridge.surface = surface;
    result.bridge.registrationHwnd = bridge;
    result.bridge.pointHwnd = ::WindowFromPoint(point);
    result.tlw.role = "tlw";
    result.tlw.surface = surface;
    result.tlw.registrationHwnd = host;
    result.tlw.pointHwnd = result.bridge.pointHwnd;

    const DWORD uiThread = ::GetCurrentThreadId();
    auto* const bridgeTarget = new DeliveryProbeTarget(uiThread);
    auto* const tlwTarget = new DeliveryProbeTarget(uiThread);
    DeliveryTargetRegistration bridgeRegistration(bridge, bridgeTarget);
    DeliveryTargetRegistration tlwRegistration(host, tlwTarget);

    // Both registrations remain owned simultaneously for the entire drag.
    // Closing in reverse order below gives each one an independently audited
    // RevokeDragDrop/CoLockObjectExternal(FALSE) transaction.
    bridgeRegistration.Open();
    tlwRegistration.Open();
    if ( bridgeRegistration.Result().registerHr == S_OK &&
         tlwRegistration.Result().registerHr == S_OK )
    {
        DeliveryProbeTarget* const expectedTarget =
            expectBridge ? bridgeTarget : tlwTarget;
        const DeliveryDragResult drag =
            RunDeliveryDrag(
                expectedTarget, point, nearbyPoint,
                departurePoint, inputGuard);
        CaptureDeliveryArm(result.bridge, *bridgeTarget, drag);
        CaptureDeliveryArm(result.tlw, *tlwTarget, drag);

        const DeliveryArmResult& expected =
            expectBridge ? result.bridge : result.tlw;
        const DeliveryArmResult& unexpected =
            expectBridge ? result.tlw : result.bridge;
        result.delivered =
            expected.delivered && DeliveryCallbacksAreEmpty(unexpected);
        result.partialDelivery =
            !result.delivered &&
            !drag.inputInterrupted &&
            !drag.injectionFailed &&
            !drag.timedOut;
        result.inputInterrupted = drag.inputInterrupted;
        result.injectionFailed = drag.injectionFailed;
        result.timedOut = drag.timedOut;
    }

    tlwRegistration.Close();
    bridgeRegistration.Close();
    result.bridge.registration = bridgeRegistration.Result();
    result.tlw.registration = tlwRegistration.Result();
    bridgeTarget->Release();
    tlwTarget->Release();

    PrintCombinedDeliveryArm(result);
    return result;
}

class DeliveryWindowPresentation
{
public:
    bool Show(
        HWND hwnd,
        HWND bridge,
        HWND inner,
        const PhysicalInputGuard& inputGuard,
        POINT* point,
        POINT* nearby,
        POINT* departure)
    {
        m_inputGuard = &inputGuard;
        if ( !hwnd || !bridge || !inner || !point || !nearby || !departure )
            return Fail("invalid-arguments");

        if ( !::GetCursorPos(&m_originalCursor) )
            return Fail("get-cursor-position");

        MONITORINFO monitorInfo{ sizeof(monitorInfo) };
        const HMONITOR monitor =
            ::MonitorFromPoint(m_originalCursor, MONITOR_DEFAULTTONEAREST);
        if ( !monitor || !::GetMonitorInfoW(monitor, &monitorInfo) )
            return Fail("monitor-work-area");

        m_workRect = monitorInfo.rcWork;
        const RECT& work = m_workRect;

        const LONG workWidth = work.right - work.left;
        const LONG workHeight = work.bottom - work.top;
        const LONG width = std::min<LONG>(320, workWidth);
        const LONG height = std::min<LONG>(240, workHeight);
        if ( width < 96 || height < 96 )
            return Fail("work-area-too-small");

        // The pointer can legitimately be parked on a taskbar or at a
        // monitor edge. Clamp only the presentation anchor; the original
        // position is retained and restored after the physical probe.
        const POINT anchor{
            std::clamp<LONG>(
                m_originalCursor.x, work.left, work.right - 1),
            std::clamp<LONG>(
                m_originalCursor.y, work.top, work.bottom - 1)
        };

        const LONG x = std::clamp<LONG>(
            anchor.x - width / 2,
            work.left,
            work.right - width);
        const LONG y = std::clamp<LONG>(
            anchor.y - height / 2,
            work.top,
            work.bottom - height);

        if ( !::SetWindowPos(
                hwnd, HWND_TOPMOST, x, y, width, height,
                SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_FRAMECHANGED) )
        {
            return Fail("set-window-position");
        }
        m_hwnd = hwnd;
        m_bridge = bridge;
        m_inner = inner;
        ::UpdateWindow(hwnd);

        // Mirror the production wxWinUITopLevelHost publication contract.
        // A DesktopWindowXamlSource can load and measure while its bridge is
        // still hidden, which is sufficient for the off-screen smoke test but
        // not for WindowFromPoint() or physical OLE delivery.
        ::SetLastError(ERROR_SUCCESS);
        const LONG_PTR bridgeStyle =
            ::GetWindowLongPtrW(bridge, GWL_STYLE);
        DWORD styleError = ::GetLastError();
        if ( !bridgeStyle && styleError != ERROR_SUCCESS )
            return Fail("get-bridge-style");

        ::SetLastError(ERROR_SUCCESS);
        const LONG_PTR previousStyle = ::SetWindowLongPtrW(
            bridge,
            GWL_STYLE,
            bridgeStyle |
                WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
        styleError = ::GetLastError();
        if ( !previousStyle && styleError != ERROR_SUCCESS )
            return Fail("publish-bridge-style");

        RECT hostClient{};
        if ( !::GetClientRect(hwnd, &hostClient) ||
             hostClient.right <= hostClient.left ||
             hostClient.bottom <= hostClient.top )
        {
            return Fail("host-client-geometry");
        }

        // The hosted child starts at 0x0. ResizeContentToParentWindow tracks
        // subsequent layout, but the Win32 host remains responsible for the
        // first non-empty island rectangle before physical hit-testing.
        if ( !::SetWindowPos(
                bridge, HWND_TOP,
                hostClient.left,
                hostClient.top,
                hostClient.right - hostClient.left,
                hostClient.bottom - hostClient.top,
                SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_FRAMECHANGED) )
        {
            return Fail("size-and-pin-visible-bridge");
        }

        RECT visibleBridge{};
        const bool exposed = PumpUntil(
            [&]()
            {
                RECT currentBridge{};
                if ( !::GetWindowRect(bridge, &currentBridge) )
                    return false;
                m_bridgeRect = currentBridge;

                // InputSiteWindowClass is an internal routing HWND. In the
                // current ContentIsland topology it can remain hidden at 0x0
                // while the bridge is visibly composited and hit-testable.
                RECT currentInner{};
                if ( ::GetWindowRect(inner, &currentInner) )
                    m_innerRect = currentInner;

                RECT currentVisible{};
                if ( !::IntersectRect(
                        &currentVisible, &currentBridge, &work) )
                {
                    return false;
                }

                constexpr LONG Margin = 4;
                if ( currentVisible.right - currentVisible.left <=
                        2 * Margin ||
                     currentVisible.bottom - currentVisible.top <=
                        2 * Margin )
                {
                    return false;
                }

                const POINT candidate{
                    currentVisible.left +
                        (currentVisible.right - currentVisible.left) / 2,
                    currentVisible.top +
                        (currentVisible.bottom - currentVisible.top) / 2
                };
                m_probePoint = candidate;
                if ( !IsSameWindowOrDescendant(
                        ::WindowFromPoint(candidate), bridge) )
                {
                    return false;
                }
                visibleBridge = currentVisible;
                return true;
            },
            2000);
        if ( !exposed )
            return Fail("bridge-not-exposed");

        constexpr LONG Margin = 4;
        if ( m_probePoint.x < visibleBridge.left + Margin ||
             m_probePoint.x >= visibleBridge.right - Margin ||
             m_probePoint.y < visibleBridge.top + Margin ||
             m_probePoint.y >= visibleBridge.bottom - Margin )
        {
            return Fail("safe-point-margin");
        }

        m_nearbyPoint = MakeNearbyPoint(m_probePoint, visibleBridge);
        if ( !IsSameWindowOrDescendant(
                ::WindowFromPoint(m_nearbyPoint), bridge) )
        {
            return Fail("nearby-point-not-on-bridge");
        }

        // DoDragDrop is entered with a real initiating button held. Keep that
        // button-down entirely inside a tiny probe-owned no-activate popup,
        // then cross into the candidate HWND. No third-party window ever sees
        // the injected source gesture.
        constexpr LONG SourcePadExtent = 24;
        constexpr LONG SourcePadMargin = 8;
        const LONG sourceX = visibleBridge.left + SourcePadMargin;
        const LONG sourceY = visibleBridge.top + SourcePadMargin;
        if ( sourceX + SourcePadExtent >= visibleBridge.right ||
             sourceY + SourcePadExtent >= visibleBridge.bottom )
        {
            return Fail("source-pad-geometry");
        }
        m_sourcePad = ::CreateWindowExW(
            WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST,
            L"STATIC", L"",
            WS_POPUP | WS_VISIBLE | SS_GRAYRECT | SS_NOTIFY,
            sourceX, sourceY, SourcePadExtent, SourcePadExtent,
            nullptr, nullptr, ::GetModuleHandleW(nullptr), nullptr);
        if ( !m_sourcePad )
            return Fail("create-source-pad");
        if ( !::SetWindowPos(
                m_sourcePad, HWND_TOPMOST,
                sourceX, sourceY, SourcePadExtent, SourcePadExtent,
                SWP_NOACTIVATE | SWP_SHOWWINDOW) )
        {
            return Fail("position-source-pad");
        }
        m_sourcePadRect = {
            sourceX, sourceY,
            sourceX + SourcePadExtent, sourceY + SourcePadExtent
        };
        m_departurePoint = {
            sourceX + SourcePadExtent / 2,
            sourceY + SourcePadExtent / 2
        };
        const bool sourcePadExposed = PumpUntil(
            [&]()
            {
                return ::WindowFromPoint(m_departurePoint) == m_sourcePad;
            },
            1000);
        if ( !sourcePadExposed )
            return Fail("source-pad-not-exposed");
        if ( PointsAreClose(m_departurePoint, m_probePoint, 4) )
            return Fail("source-pad-overlaps-target");

        if ( !PointsAreClose(m_originalCursor, m_probePoint) )
        {
            if ( !InjectPhysicalProbePoint(m_probePoint) )
                return Fail("safe-point-injection");
            m_cursorMoved = true;
        }

        const bool cursorPlaced = PumpUntil(
            [&]()
            {
                POINT current{};
                return ::GetCursorPos(&current) &&
                       PointsAreClose(current, m_probePoint) &&
                       IsSameWindowOrDescendant(
                           ::WindowFromPoint(current), bridge);
            },
            1000);
        if ( !cursorPlaced )
            return Fail("safe-point-not-reached");

        *point = m_probePoint;
        *nearby = m_nearbyPoint;
        *departure = m_departurePoint;
        m_cursorMoved = true;
        Print("passed", "safe-point-ready");
        return true;
    }

    ~DeliveryWindowPresentation()
    {
        if ( m_sourcePad )
        {
            ::ShowWindow(m_sourcePad, SW_HIDE);
            (void)::DestroyWindow(m_sourcePad);
            m_sourcePad = nullptr;
        }
        if ( m_hwnd )
        {
            ::ShowWindow(m_hwnd, SW_HIDE);
            (void)::SetWindowPos(
                m_hwnd, HWND_NOTOPMOST, -32000, -32000, 0, 0,
                SWP_NOSIZE | SWP_NOACTIVATE);
        }

        // Restore only if our tagged injections still own the pointer. Real
        // user input wins and must never be pulled back by cleanup.
        POINT current{};
        if ( m_cursorMoved && m_inputGuard &&
              !m_inputGuard->WasInterrupted() &&
              ::GetCursorPos(&current) &&
              (PointsAreClose(current, m_probePoint) ||
               PointsAreClose(current, m_nearbyPoint) ||
               PointsAreClose(current, m_departurePoint)) )
        {
            (void)InjectPhysicalProbePoint(m_originalCursor);
        }
    }

private:
    void Print(const char* result, const char* reason) const
    {
        std::printf(
            "wx_winui_ole_delivery: {\"event\":\"presentation\","
            "\"result\":\"%s\",\"reason\":\"%s\","
            "\"cursor\":[%ld,%ld],\"work\":[%ld,%ld,%ld,%ld],"
            "\"bridge_visible\":%s,\"inner_visible\":%s,"
            "\"bridge\":[%ld,%ld,%ld,%ld],"
            "\"inner\":[%ld,%ld,%ld,%ld],\"point\":[%ld,%ld],"
            "\"point_hwnd\":\"0x%llx\","
            "\"source_pad\":[%ld,%ld,%ld,%ld],"
            "\"departure\":[%ld,%ld],"
            "\"departure_hwnd\":\"0x%llx\"}\n",
            result,
            reason,
            m_originalCursor.x,
            m_originalCursor.y,
            m_workRect.left,
            m_workRect.top,
            m_workRect.right,
            m_workRect.bottom,
            JsonBool(m_bridge && ::IsWindowVisible(m_bridge)),
            JsonBool(m_inner && ::IsWindowVisible(m_inner)),
            m_bridgeRect.left,
            m_bridgeRect.top,
            m_bridgeRect.right,
            m_bridgeRect.bottom,
            m_innerRect.left,
            m_innerRect.top,
            m_innerRect.right,
            m_innerRect.bottom,
            m_probePoint.x,
            m_probePoint.y,
            HwndAsInteger(::WindowFromPoint(m_probePoint)),
            m_sourcePadRect.left,
            m_sourcePadRect.top,
            m_sourcePadRect.right,
            m_sourcePadRect.bottom,
            m_departurePoint.x,
            m_departurePoint.y,
            HwndAsInteger(::WindowFromPoint(m_departurePoint)));
        std::fflush(stdout);
    }

    bool Fail(const char* reason)
    {
        Print("failed", reason);
        return false;
    }

    HWND m_hwnd = nullptr;
    HWND m_bridge = nullptr;
    HWND m_inner = nullptr;
    HWND m_sourcePad = nullptr;
    const PhysicalInputGuard* m_inputGuard = nullptr;
    POINT m_originalCursor{};
    POINT m_probePoint{};
    POINT m_nearbyPoint{};
    POINT m_departurePoint{};
    RECT m_workRect{};
    RECT m_bridgeRect{};
    RECT m_innerRect{};
    RECT m_sourcePadRect{};
    bool m_cursorMoved = false;
};

class DeliveryBridgeHole
{
public:
    bool Install(
        HWND host, HWND bridge, POINT screenPoint,
        POINT* nearbyPoint, HWND* exposedWindow)
    {
        m_hostForLog = host;
        m_bridgeForLog = bridge;
        m_screenPoint = screenPoint;
        if ( !host || !bridge || !nearbyPoint || !exposedWindow )
            return Fail("invalid-arguments", ERROR_INVALID_PARAMETER);

        RECT bridgeClient{};
        if ( !::GetClientRect(bridge, &bridgeClient) )
            return Fail("get-bridge-client", ::GetLastError());
        m_bridgeClient = bridgeClient;

        POINT bridgePoint = screenPoint;
        if ( !::ScreenToClient(bridge, &bridgePoint) )
            return Fail("screen-to-client", ::GetLastError());
        m_bridgePoint = bridgePoint;

        constexpr LONG HalfWidth = 8;
        constexpr LONG HalfHeight = 24;
        RECT cut{
            std::max<LONG>(bridgeClient.left, bridgePoint.x - HalfWidth),
            std::max<LONG>(bridgeClient.top, bridgePoint.y - HalfHeight),
            std::min<LONG>(bridgeClient.right, bridgePoint.x + HalfWidth + 1),
            std::min<LONG>(bridgeClient.bottom, bridgePoint.y + HalfHeight + 1)
        };
        m_cut = cut;
        if ( cut.right - cut.left < 6 || cut.bottom - cut.top < 6 )
            return Fail("cut-too-small", ERROR_INVALID_DATA);

        POINT parentCorners[2] = {
            { cut.left, cut.top },
            { cut.right, cut.bottom }
        };
        ::SetLastError(ERROR_SUCCESS);
        const int mapped = ::MapWindowPoints(
            bridge, host, parentCorners, 2);
        if ( mapped == 0 && ::GetLastError() != ERROR_SUCCESS )
            return Fail("map-cut-to-host", ::GetLastError());
        m_cutInHost = {
            parentCorners[0].x, parentCorners[0].y,
            parentCorners[1].x, parentCorners[1].y
        };

        m_nativeChild = ::CreateWindowExW(
            0, L"STATIC", L"",
            WS_CHILD | WS_VISIBLE | SS_GRAYRECT | SS_NOTIFY,
            parentCorners[0].x,
            parentCorners[0].y,
            parentCorners[1].x - parentCorners[0].x,
            parentCorners[1].y - parentCorners[0].y,
            host, nullptr, ::GetModuleHandleW(nullptr), nullptr);
        if ( !m_nativeChild )
            return Fail("create-native-child", ::GetLastError());
        // Keep the native strip immediately behind the bridge. Once the
        // bridge region is cut, this hit-testable sibling is the exposed
        // surface, matching production's native-scrollbar topology.
        if ( !::SetWindowPos(
                m_nativeChild, bridge, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE |
                    SWP_NOACTIVATE | SWP_SHOWWINDOW) )
        {
            return Fail("position-native-child", ::GetLastError());
        }

        HRGN region = ::CreateRectRgn(
            bridgeClient.left, bridgeClient.top,
            bridgeClient.right, bridgeClient.bottom);
        HRGN cutRegion =
            ::CreateRectRgn(cut.left, cut.top, cut.right, cut.bottom);
        if ( !region || !cutRegion ||
             ::CombineRgn(region, region, cutRegion, RGN_DIFF) == ERROR )
        {
            if ( cutRegion )
                ::DeleteObject(cutRegion);
            if ( region )
                ::DeleteObject(region);
            return Fail("combine-region", ::GetLastError());
        }
        ::DeleteObject(cutRegion);

        if ( !::SetWindowRgn(bridge, region, TRUE) )
        {
            ::DeleteObject(region);
            return Fail("set-window-region", ::GetLastError());
        }
        // USER32 owns region after the successful call.
        m_bridge = bridge;

        const bool exposed = PumpUntil(
            [&]()
            {
                return ::WindowFromPoint(screenPoint) == m_nativeChild;
            },
            1000);
        if ( !exposed )
            return Fail("expose-point-timeout", ERROR_TIMEOUT);

        RECT cutScreen = cut;
        POINT screenCorners[2] = {
            { cut.left, cut.top },
            { cut.right, cut.bottom }
        };
        ::SetLastError(ERROR_SUCCESS);
        const int screenMapped = ::MapWindowPoints(
            bridge, HWND_DESKTOP, screenCorners, 2);
        if ( screenMapped == 0 && ::GetLastError() != ERROR_SUCCESS )
            return Fail("map-cut-to-screen", ::GetLastError());
        cutScreen = {
            screenCorners[0].x, screenCorners[0].y,
            screenCorners[1].x, screenCorners[1].y
        };
        *nearbyPoint = MakeNearbyPoint(screenPoint, cutScreen);
        m_nearbyPoint = *nearbyPoint;
        *exposedWindow = m_nativeChild;
        if ( ::WindowFromPoint(*nearbyPoint) != m_nativeChild )
            return Fail("nearby-not-exposed", ERROR_INVALID_WINDOW_HANDLE);
        Print("passed", "ready", ERROR_SUCCESS);
        return true;
    }

    ~DeliveryBridgeHole()
    {
        (void)Remove();
    }

    bool Remove()
    {
        bool ok = true;
        if ( m_bridge )
        {
            if ( !::SetWindowRgn(m_bridge, nullptr, TRUE) )
                ok = false;
            m_bridge = nullptr;
        }
        if ( m_nativeChild )
        {
            if ( !::DestroyWindow(m_nativeChild) )
                ok = false;
            m_nativeChild = nullptr;
        }
        return ok;
    }

private:
    void Print(const char* result, const char* stage, DWORD error) const
    {
        std::printf(
            "wx_winui_ole_delivery: {\"event\":\"bridge-hole-install\","
            "\"result\":\"%s\",\"stage\":\"%s\",\"win32_error\":%lu,"
            "\"host\":\"0x%llx\",\"bridge\":\"0x%llx\","
            "\"native_child\":\"0x%llx\","
            "\"screen_point\":[%ld,%ld],\"bridge_point\":[%ld,%ld],"
            "\"bridge_client\":[%ld,%ld,%ld,%ld],"
            "\"cut_bridge\":[%ld,%ld,%ld,%ld],"
            "\"cut_host\":[%ld,%ld,%ld,%ld],"
            "\"nearby_point\":[%ld,%ld],"
            "\"point_hwnd\":\"0x%llx\",\"nearby_hwnd\":\"0x%llx\","
            "\"native_visible\":%s}\n",
            result,
            stage,
            static_cast<unsigned long>(error),
            HwndAsInteger(m_hostForLog),
            HwndAsInteger(m_bridgeForLog),
            HwndAsInteger(m_nativeChild),
            m_screenPoint.x,
            m_screenPoint.y,
            m_bridgePoint.x,
            m_bridgePoint.y,
            m_bridgeClient.left,
            m_bridgeClient.top,
            m_bridgeClient.right,
            m_bridgeClient.bottom,
            m_cut.left,
            m_cut.top,
            m_cut.right,
            m_cut.bottom,
            m_cutInHost.left,
            m_cutInHost.top,
            m_cutInHost.right,
            m_cutInHost.bottom,
            m_nearbyPoint.x,
            m_nearbyPoint.y,
            HwndAsInteger(::WindowFromPoint(m_screenPoint)),
            HwndAsInteger(::WindowFromPoint(m_nearbyPoint)),
            JsonBool(m_nativeChild && ::IsWindowVisible(m_nativeChild)));
        std::fflush(stdout);
    }

    bool Fail(const char* stage, DWORD error)
    {
        Print("failed", stage, error);
        Cleanup();
        return false;
    }

    void Cleanup()
    {
        (void)Remove();
    }

    HWND m_bridge = nullptr;
    HWND m_nativeChild = nullptr;
    HWND m_hostForLog = nullptr;
    HWND m_bridgeForLog = nullptr;
    POINT m_screenPoint{};
    POINT m_bridgePoint{};
    POINT m_nearbyPoint{};
    RECT m_bridgeClient{};
    RECT m_cut{};
    RECT m_cutInHost{};
};

int RunPhysicalDeliveryProbe(HWND host, HWND bridge, HWND inner)
{
    const DeliveryRegistrationResult preflightBridge =
        PreflightDeliveryRegistration("bridge", bridge);
    const DeliveryRegistrationResult preflightInner =
        PreflightDeliveryRegistration("inner", inner);
    const DeliveryRegistrationResult preflightTlw =
        PreflightDeliveryRegistration("tlw", host);

    const DeliveryRegistrationResult preflights[] = {
        preflightBridge, preflightInner, preflightTlw
    };
    for ( const DeliveryRegistrationResult& preflight : preflights )
    {
        if ( !DeliveryRegistrationCleaned(preflight) )
            return ReportDeliveryTerminal(
                "failed", "preflight-cleanup", 19);
        if ( preflight.registerHr == DRAGDROP_E_ALREADYREGISTERED )
            return ReportDeliveryTerminal(
                "inconclusive", "foreign-registration", 17);
        if ( preflight.registerHr != S_OK )
            return ReportDeliveryTerminal(
                "failed", "registration-preflight", 16);
    }

    DWORD idleMs = 0;
    std::printf(
        "wx_winui_ole_delivery: {\"event\":\"safety\","
        "\"result\":\"waiting-for-idle\","
        "\"required_idle_ms\":%lu,\"timeout_ms\":30000}\n",
        static_cast<unsigned long>(PhysicalProbeMinimumIdleMs));
    std::fflush(stdout);
    if ( !WaitForPhysicalInputIdle(30000, &idleMs) )
    {
        ReportPhysicalSafetyRefusal("idle-timeout", idleMs);
        return 15;
    }

    PhysicalInputGuard inputGuard;
    if ( !inputGuard.Install() )
        return ReportDeliveryTerminal("failed", "input-hooks", 16);

    if ( !GetPhysicalInputIdleMs(&idleMs) ||
         idleMs < PhysicalProbeMinimumIdleMs )
    {
        ReportPhysicalSafetyRefusal("not-idle-at-delivery", idleMs);
        return 15;
    }
    if ( (::GetAsyncKeyState(VK_LBUTTON) & 0x8000) ||
         (::GetAsyncKeyState(VK_RBUTTON) & 0x8000) ||
         (::GetAsyncKeyState(VK_MBUTTON) & 0x8000) ||
         (::GetAsyncKeyState(VK_XBUTTON1) & 0x8000) ||
         (::GetAsyncKeyState(VK_XBUTTON2) & 0x8000) )
    {
        ReportPhysicalSafetyRefusal("mouse-button-down", idleMs);
        return 15;
    }
    if ( inputGuard.WasInterrupted() )
    {
        inputGuard.PrintInterruption("before-presentation");
        return ReportDeliveryTerminal(
            "failed", "user-input-before-presentation", 16);
    }

    DeliveryWindowPresentation presentation;
    POINT normalPoint{};
    POINT normalNearby{};
    POINT departurePoint{};
    if ( !presentation.Show(
            host, bridge, inner, inputGuard,
            &normalPoint, &normalNearby, &departurePoint) )
        return ReportDeliveryTerminal(
            "failed", "window-presentation", 16);
    if ( inputGuard.WasInterrupted() )
    {
        inputGuard.PrintInterruption("during-presentation");
        return ReportDeliveryTerminal(
            "failed", "user-input-during-presentation", 16);
    }

    DeliveryArmResult innerNormal = RunDeliveryArm(
        "inner", "normal", inner,
        normalPoint, normalNearby, departurePoint, inputGuard);
    if ( inputGuard.WasInterrupted() )
    {
        inputGuard.PrintInterruption("after-inner");
        PrintDeliveryRegistration(
            "interrupted-after", "inner", inner, innerNormal.registration);
        return ReportDeliveryTerminal(
            "failed", "user-interrupted-after-inner", 16);
    }

    DeliveryArmResult bridgeNormal = RunDeliveryArm(
        "bridge", "normal", bridge,
        normalPoint, normalNearby, departurePoint, inputGuard);
    if ( inputGuard.WasInterrupted() )
    {
        inputGuard.PrintInterruption("after-bridge");
        return ReportDeliveryTerminal(
            "failed", "user-interrupted-after-bridge", 16);
    }

    DeliveryArmResult tlwNormal = RunDeliveryArm(
        "tlw", "normal", host,
        normalPoint, normalNearby, departurePoint, inputGuard);
    if ( inputGuard.WasInterrupted() )
    {
        inputGuard.PrintInterruption("after-tlw");
        return ReportDeliveryTerminal(
            "failed", "user-interrupted-after-tlw", 16);
    }

    CombinedDeliveryArmResult combinedNormal = RunCombinedDeliveryArm(
        "normal", host, bridge, true,
        normalPoint, normalNearby, departurePoint, inputGuard);
    if ( inputGuard.WasInterrupted() )
    {
        inputGuard.PrintInterruption("after-combined-normal");
        return ReportDeliveryTerminal(
            "failed", "user-interrupted-after-combined-normal", 16);
    }

    DeliveryBridgeHole hole;
    POINT holeNearby{};
    HWND exposedNative = nullptr;
    if ( !hole.Install(
            host, bridge, normalPoint, &holeNearby, &exposedNative) )
        return ReportDeliveryTerminal(
            "failed", "bridge-hole", 16);
    std::printf(
        "wx_winui_ole_delivery: {\"event\":\"surface\","
        "\"surface\":\"bridge-hole\",\"point_hwnd\":\"0x%llx\","
        "\"native_child\":\"0x%llx\"}\n",
        HwndAsInteger(::WindowFromPoint(normalPoint)),
        HwndAsInteger(exposedNative));
    std::fflush(stdout);

    DeliveryArmResult bridgeHole = RunDeliveryArm(
        "bridge", "hole", bridge,
        normalPoint, holeNearby, departurePoint, inputGuard);
    if ( inputGuard.WasInterrupted() )
    {
        inputGuard.PrintInterruption("after-bridge-hole");
        return ReportDeliveryTerminal(
            "failed", "user-interrupted-in-hole", 16);
    }

    DeliveryArmResult tlwHole = RunDeliveryArm(
        "tlw", "hole", host,
        normalPoint, holeNearby, departurePoint, inputGuard);
    if ( inputGuard.WasInterrupted() )
    {
        inputGuard.PrintInterruption("after-tlw-hole");
        return ReportDeliveryTerminal(
            "failed", "user-interrupted-after-tlw-hole", 16);
    }

    CombinedDeliveryArmResult combinedHole = RunCombinedDeliveryArm(
        "hole", host, bridge, false,
        normalPoint, holeNearby, departurePoint, inputGuard);
    const bool holeCleanupOk = hole.Remove();
    if ( inputGuard.WasInterrupted() )
    {
        inputGuard.PrintInterruption("after-combined-hole");
        return ReportDeliveryTerminal(
            "failed", "user-interrupted-after-combined-hole", 16);
    }

    const DeliveryArmResult* const arms[] = {
        &innerNormal, &bridgeNormal, &tlwNormal, &bridgeHole, &tlwHole
    };
    const DeliveryRegistrationResult* const combinedRegistrations[] = {
        &combinedNormal.bridge.registration,
        &combinedNormal.tlw.registration,
        &combinedHole.bridge.registration,
        &combinedHole.tlw.registration
    };

    bool cleanupFailed = false;
    bool infrastructureFailed = false;
    bool partialDelivery = false;
    bool injectionFailed = false;
    bool armTimedOut = false;
    bool foreignRegistration = false;
    for ( const DeliveryArmResult* const arm : arms )
    {
        cleanupFailed =
            cleanupFailed || !DeliveryRegistrationCleaned(arm->registration);
        infrastructureFailed =
            infrastructureFailed ||
            (arm->registration.registerHr != S_OK &&
             arm->registration.registerHr !=
                DRAGDROP_E_ALREADYREGISTERED);
        partialDelivery = partialDelivery || arm->partialDelivery;
        injectionFailed = injectionFailed || arm->injectionFailed;
        // A bounded zero-callback cancellation is the expected observation
        // for an optional receiver that is not under the physical point
        // (notably the hidden 0x0 InputSite and the bridge-hole arm). It must
        // remain a clean miss, while any timeout with partial callbacks stays
        // fatal through partialDelivery above.
        armTimedOut =
            armTimedOut || (arm->timedOut && !arm->cleanMiss);
        foreignRegistration =
            foreignRegistration ||
            arm->registration.registerHr ==
                DRAGDROP_E_ALREADYREGISTERED;
    }
    for ( const DeliveryRegistrationResult* const registration :
          combinedRegistrations )
    {
        cleanupFailed =
            cleanupFailed || !DeliveryRegistrationCleaned(*registration);
        infrastructureFailed =
            infrastructureFailed ||
            (registration->registerHr != S_OK &&
             registration->registerHr != DRAGDROP_E_ALREADYREGISTERED);
        foreignRegistration =
            foreignRegistration ||
            registration->registerHr == DRAGDROP_E_ALREADYREGISTERED;
    }
    partialDelivery =
        partialDelivery ||
        combinedNormal.partialDelivery ||
        combinedHole.partialDelivery;
    injectionFailed =
        injectionFailed ||
        combinedNormal.injectionFailed ||
        combinedHole.injectionFailed;
    armTimedOut =
        armTimedOut || combinedNormal.timedOut || combinedHole.timedOut;

    if ( inputGuard.WasInterrupted() )
        inputGuard.PrintInterruption("matrix-finalization");
    else
        inputGuard.PrintCleanStatus();

    const char* recommendation = "none";
    bool viable = false;
    if ( tlwNormal.delivered && tlwHole.delivered )
    {
        recommendation = "tlw";
        viable = true;
    }
    else if ( bridgeNormal.delivered && bridgeHole.delivered )
    {
        recommendation = "bridge";
        viable = true;
    }
    else if ( combinedNormal.delivered && combinedHole.delivered )
    {
        recommendation = "bridge+tlw";
        viable = true;
    }

    int resultCode = 0;
    const char* result = "passed";
    const char* reason = "receiver-qualified";
    if ( inputGuard.WasInterrupted() || injectionFailed )
    {
        resultCode = 16;
        result = "failed";
        reason = inputGuard.WasInterrupted()
                    ? "user-interrupted"
                    : "input-injection";
    }
    else if ( cleanupFailed )
    {
        resultCode = 19;
        result = "failed";
        reason = "registration-cleanup";
    }
    else if ( foreignRegistration )
    {
        resultCode = 17;
        result = "inconclusive";
        reason = "foreign-registration";
    }
    else if ( infrastructureFailed )
    {
        resultCode = 16;
        result = "failed";
        reason = "registration-runtime";
    }
    else if ( !holeCleanupOk )
    {
        resultCode = 16;
        result = "failed";
        reason = "bridge-hole-cleanup";
    }
    else if ( armTimedOut || partialDelivery || !viable )
    {
        resultCode = 18;
        result = "failed";
        reason = armTimedOut
                    ? "arm-timeout"
                    : (partialDelivery
                        ? "partial-delivery"
                        : "no-viable-topology");
    }

    std::printf(
        "wx_winui_ole_delivery: {\"event\":\"summary\","
        "\"result\":\"%s\",\"reason\":\"%s\","
        "\"recommendation\":\"%s\","
        "\"inner_normal\":%s,\"bridge_normal\":%s,"
        "\"tlw_normal\":%s,\"bridge_hole\":%s,"
        "\"tlw_hole\":%s,\"combined_normal\":%s,"
        "\"combined_hole\":%s,\"exit_code\":%d}\n",
        result,
        reason,
        recommendation,
        JsonBool(innerNormal.delivered),
        JsonBool(bridgeNormal.delivered),
        JsonBool(tlwNormal.delivered),
        JsonBool(bridgeHole.delivered),
        JsonBool(tlwHole.delivered),
        JsonBool(combinedNormal.delivered),
        JsonBool(combinedHole.delivered),
        resultCode);
    std::fflush(stdout);
    return resultCode;
}

} // anonymous namespace

int main(int argc, char **argv)
{
    bool oleDropProbe = false;
    bool oleDropDeliveryProbe = false;
    if ( argc == 2 && std::strcmp(argv[1], "--ole-drop-probe") == 0 )
    {
        oleDropProbe = true;
    }
    else if ( argc == 2 &&
              std::strcmp(
                  argv[1], "--ole-drop-delivery-probe") == 0 )
    {
        oleDropDeliveryProbe = true;
    }
    else if ( argc != 1 )
    {
        std::fprintf(
            stderr,
            "wx_winui_runtime_smoke: usage: wx_winui_runtime_smoke "
            "[--ole-drop-probe|--ole-drop-delivery-probe]\n"
            "  --ole-drop-delivery-probe moves the physical cursor by a "
            "bounded amount and is disabled unless\n"
            "  %s=%s\n",
            PhysicalProbeArmVariable,
            PhysicalProbeArmValue);
        return 8;
    }

    if ( oleDropDeliveryProbe && !IsPhysicalProbeArmed() )
    {
        ReportPhysicalSafetyRefusal("not-armed");
        return 15;
    }

    // Terminate before the corresponding CTest timeout so a blocked shutdown
    // reports its exact stage instead of becoming an opaque test-runner kill.
    // The explicitly armed physical diagnostic is not a CTest and retains its
    // original, wider budget.
    const DWORD watchdogTimeoutMs = oleDropDeliveryProbe
                                        ? 120000
                                        : (oleDropProbe ? 55000 : 25000);
    std::thread([watchdogTimeoutMs]()
    {
        ::Sleep(watchdogTimeoutMs);
        std::fprintf(stderr,
                     "wx_winui_runtime_smoke: FAILED: watchdog timeout in "
                     "stage '%s'\n", gs_stage.load());
        std::fflush(stderr);
        ::ExitProcess(7);
    }).detach();

    const bool useOle = oleDropProbe || oleDropDeliveryProbe;
    OleApartment oleApartment(useOle);
    if ( useOle )
    {
        ReportStage("initializing OLE apartment");
        if ( !oleApartment.IsInitialized() )
        {
            std::fprintf(
                stderr,
                "wx_winui_runtime_smoke: FAILED: OleInitialize: "
                "HRESULT 0x%08lx\n",
                static_cast<unsigned long>(oleApartment.Result()));
            return 9;
        }
    }
    else
    {
        ReportStage("initializing COM apartment");
        try
        {
            winrt::init_apartment(winrt::apartment_type::single_threaded);
        }
        catch ( const winrt::hresult_error& e )
        {
            if ( e.code() != RPC_E_CHANGED_MODE )
                return FailWinRT(5, "winrt::init_apartment", e);
        }
    }

    ReportStage("initializing Windows App SDK bootstrap");
    PACKAGE_VERSION minVersion{};
    minVersion.Version = WINDOWSAPPSDK_RUNTIME_VERSION_UINT64;
    const HRESULT hrBootstrap = ::MddBootstrapInitialize2(
        WINDOWSAPPSDK_RELEASE_MAJORMINOR,
        WINDOWSAPPSDK_RELEASE_VERSION_TAG_W,
        minVersion,
        MddBootstrapInitializeOptions_OnPackageIdentity_NOOP);
    if ( FAILED(hrBootstrap) )
    {
        std::fprintf(stderr,
            "wx_winui_runtime_smoke: FAILED: MddBootstrapInitialize2: "
            "HRESULT 0x%08lx\n"
            "  the Windows App Runtime %u.%u framework package (version >= "
            "%u.%u.%u.%u) is not installed; install it from the "
            "Microsoft.WindowsAppSDK.Runtime NuGet package (tools/MSIX) or "
            "the Windows App SDK runtime installer\n",
            static_cast<unsigned long>(hrBootstrap),
            WINDOWSAPPSDK_RELEASE_MAJOR,
            WINDOWSAPPSDK_RELEASE_MINOR,
            WINDOWSAPPSDK_RUNTIME_VERSION_MAJOR,
            WINDOWSAPPSDK_RUNTIME_VERSION_MINOR,
            WINDOWSAPPSDK_RUNTIME_VERSION_BUILD,
            WINDOWSAPPSDK_RUNTIME_VERSION_REVISION);
        return 2;
    }

    int rc = 5;
    try
    {
        using namespace winrt::Microsoft::UI::Dispatching;
        using namespace winrt::Microsoft::UI::Xaml;
        using namespace winrt::Microsoft::UI::Xaml::Controls;
        using namespace winrt::Microsoft::UI::Xaml::Hosting;

        ReportStage("creating dispatcher queue");
        auto dispatcherController =
            DispatcherQueueController::CreateOnCurrentThread();
        if ( !dispatcherController )
        {
            std::fprintf(stderr, "wx_winui_runtime_smoke: FAILED: "
                                 "DispatcherQueueController is null\n");
            ::MddBootstrapShutdown();
            return 6;
        }

        // Application first, XAML manager second: same ordering constraint
        // as wxWinUI3Initialize().
        ReportStage("creating application object");
        auto application = winrt::make<SmokeApplication>().as<Application>();

        ReportStage("initializing XAML manager");
        auto xamlManager = WindowsXamlManager::InitializeForCurrentThread();
        if ( !xamlManager )
        {
            std::fprintf(stderr, "wx_winui_runtime_smoke: FAILED: "
                                 "WindowsXamlManager is null\n");
            ::MddBootstrapShutdown();
            return 6;
        }

        ReportStage("loading XamlControlsResources");
        try
        {
            auto resources = Application::Current().Resources();
            resources.MergedDictionaries().Append(XamlControlsResources());

            if ( useOle )
                oleApartment.RetainForProcessLifetime();
        }
        catch ( const winrt::hresult_error& e )
        {
            FailWinRT(3, "XamlControlsResources", e);
            PrintPayloadHint();
            return 3;
        }

        ReportStage("creating host window");
        WNDCLASSW wc{};
        wc.lpfnWndProc = SmokeWndProc;
        wc.hInstance = ::GetModuleHandleW(nullptr);
        wc.lpszClassName = L"wxWinUIRuntimeSmokeWindow";
        if ( !::RegisterClassW(&wc) )
        {
            std::fprintf(stderr, "wx_winui_runtime_smoke: FAILED: "
                                 "RegisterClass: %lu\n", ::GetLastError());
            return 1;
        }
        // Off-screen and never activated so that running the probe does not
        // disturb whoever is using the machine.
        HWND hwnd = ::CreateWindowExW(
            WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
                (oleDropDeliveryProbe ? WS_EX_TOPMOST : 0),
            wc.lpszClassName, L"wx_winui_runtime_smoke",
            oleDropDeliveryProbe ? WS_POPUP : WS_OVERLAPPEDWINDOW,
            -32000, -32000, 800, 600,
            nullptr, nullptr, wc.hInstance, nullptr);
        if ( !hwnd )
        {
            std::fprintf(stderr, "wx_winui_runtime_smoke: FAILED: "
                                 "CreateWindowEx: %lu\n", ::GetLastError());
            return 1;
        }
        ::ShowWindow(hwnd, SW_SHOWNOACTIVATE);

        ReportStage("creating XAML island");
        auto source = DesktopWindowXamlSource();
        source.Initialize(winrt::Microsoft::UI::GetWindowIdFromWindow(hwnd));
        source.SiteBridge().ResizePolicy(
            winrt::Microsoft::UI::Content::ContentSizePolicy::
                ResizeContentToParentWindow);

        ReportStage("realizing Button, TreeView and TabView");
        auto panel = StackPanel();
        if ( useOle )
            panel.AllowDrop(true);

        auto button = Button();
        button.Content(winrt::box_value(L"wx"));
        panel.Children().Append(button);

        auto tree = TreeView();
        auto node = TreeViewNode();
        node.Content(winrt::box_value(L"node"));
        tree.RootNodes().Append(node);
        panel.Children().Append(tree);

        auto tabs = TabView();
        auto tab = TabViewItem();
        tab.Header(winrt::box_value(L"tab"));
        tabs.TabItems().Append(tab);
        panel.Children().Append(tabs);

        source.Content(panel);

        const bool loaded = PumpUntil(
            [&]()
            {
                return button.IsLoaded() && tree.IsLoaded() && tabs.IsLoaded();
            },
            15000);

        panel.UpdateLayout();

        if ( !loaded )
        {
            std::fprintf(stderr,
                "wx_winui_runtime_smoke: FAILED: controls did not load within "
                "15s (Button=%d TreeView=%d TabView=%d)\n",
                int(button.IsLoaded()), int(tree.IsLoaded()),
                int(tabs.IsLoaded()));
            PrintPayloadHint();
            rc = 4;
        }
        else if ( button.DesiredSize().Height <= 0 ||
                  tree.DesiredSize().Height <= 0 ||
                  tabs.DesiredSize().Height <= 0 )
        {
            // A control without its template measures to nothing.
            std::fprintf(stderr,
                "wx_winui_runtime_smoke: FAILED: a control measured to an "
                "empty size, its template is missing (Button=%.0fx%.0f "
                "TreeView=%.0fx%.0f TabView=%.0fx%.0f)\n",
                button.DesiredSize().Width, button.DesiredSize().Height,
                tree.DesiredSize().Width, tree.DesiredSize().Height,
                tabs.DesiredSize().Width, tabs.DesiredSize().Height);
            PrintPayloadHint();
            rc = 4;
        }
        else
        {
            std::printf(
                "wx_winui_runtime_smoke: OK: Button %.0fx%.0f, TreeView "
                "%.0fx%.0f, TabView %.0fx%.0f\n",
                button.DesiredSize().Width, button.DesiredSize().Height,
                tree.DesiredSize().Width, tree.DesiredSize().Height,
                tabs.DesiredSize().Width, tabs.DesiredSize().Height);
            rc = 0;
        }

        if ( rc == 0 && oleDropProbe )
        {
            ReportStage("probing OLE drop registration");
            const HWND bridge = GetWindowFromWindowId(
                source.SiteBridge().WindowId());
            InputSiteSearch inputSite;
            inputSite.threadId = ::GetCurrentThreadId();
            const bool foundInner = bridge && PumpUntil(
                [&]()
                {
                    inputSite.candidate = nullptr;
                    inputSite.matches = 0;
                    (void)::EnumChildWindows(
                        bridge, FindInputSiteWindow,
                        reinterpret_cast<LPARAM>(&inputSite));
                    return inputSite.matches == 1;
                },
                5000);
            inputSite.report = true;
            inputSite.candidate = nullptr;
            inputSite.matches = 0;
            if ( bridge )
            {
                (void)::EnumChildWindows(
                    bridge, FindInputSiteWindow,
                    reinterpret_cast<LPARAM>(&inputSite));
            }
            const HWND inner = inputSite.candidate;

            if ( !::IsWindow(hwnd) ||
                 !bridge || !::IsWindow(bridge) ||
                 !foundInner || inputSite.matches != 1 ||
                 !::IsWindow(inner) ||
                 ::GetWindowThreadProcessId(hwnd, nullptr) !=
                    ::GetCurrentThreadId() ||
                 ::GetWindowThreadProcessId(bridge, nullptr) !=
                    ::GetCurrentThreadId() ||
                 ::GetWindowThreadProcessId(inner, nullptr) !=
                    ::GetCurrentThreadId() )
            {
                std::fprintf(
                    stderr,
                    "wx_winui_runtime_smoke: FAILED: OLE probe could not "
                    "resolve one same-thread TLW, bridge and InputSite "
                    "descendant (tlw=%p bridge=%p inner=%p matches=%u)\n",
                    static_cast<void *>(hwnd),
                    static_cast<void *>(bridge),
                    static_cast<void *>(inner),
                    inputSite.matches);
                rc = 10;
            }
            else
            {
                const OleRegistrationProbe tlwProbe =
                    ProbeOleRegistration("tlw", hwnd);
                const OleRegistrationProbe bridgeProbe =
                    ProbeOleRegistration("bridge", bridge);
                const OleRegistrationProbe innerProbe =
                    ProbeOleRegistration("inner", inner);

                if ( tlwProbe.registerResult ==
                        DRAGDROP_E_ALREADYREGISTERED ||
                     bridgeProbe.registerResult ==
                        DRAGDROP_E_ALREADYREGISTERED )
                {
                    std::fprintf(
                        stderr,
                        "wx_winui_runtime_smoke: OLE probe BLOCKED: the "
                        "TLW or bridge already has a drop target; no foreign "
                        "registration was revoked\n");
                    rc = 11;
                }
                else if ( innerProbe.registerResult ==
                            DRAGDROP_E_ALREADYREGISTERED )
                {
                    std::fprintf(
                        stderr,
                        "wx_winui_runtime_smoke: OLE probe INCONCLUSIVE: "
                        "the inner InputSite already has a drop target; "
                        "callback coexistence must be proved and no foreign "
                        "registration was revoked\n");
                    rc = 14;
                }
                else if ( !OleRegistrationWasExact(tlwProbe) ||
                          !OleRegistrationWasExact(bridgeProbe) ||
                          !OleRegistrationWasExact(innerProbe) )
                {
                    std::fprintf(
                        stderr,
                        "wx_winui_runtime_smoke: FAILED: OLE registration "
                        "or exact owned revocation failed\n");
                    rc = 12;
                }

                // Give the island a message turn after all owned
                // registrations have been revoked. Never revoke an
                // ALREADYREGISTERED window: that target belongs to XAML.
                PumpUntil([]() { return false; }, 100);
                panel.UpdateLayout();
                if ( !button.IsLoaded() || !tree.IsLoaded() ||
                     !tabs.IsLoaded() ||
                     button.DesiredSize().Height <= 0 )
                {
                    std::fprintf(
                        stderr,
                        "wx_winui_runtime_smoke: FAILED: XAML became "
                        "unusable after the OLE registration probe\n");
                    if ( rc == 0 )
                        rc = 13;
                }
            }
        }

        if ( rc == 0 && oleDropDeliveryProbe )
        {
            ReportStage("resolving physical OLE delivery windows");
            const HWND bridge = GetWindowFromWindowId(
                source.SiteBridge().WindowId());
            InputSiteSearch inputSite;
            inputSite.threadId = ::GetCurrentThreadId();
            const bool foundInner = bridge && PumpUntil(
                [&]()
                {
                    inputSite.candidate = nullptr;
                    inputSite.matches = 0;
                    (void)::EnumChildWindows(
                        bridge, FindInputSiteWindow,
                        reinterpret_cast<LPARAM>(&inputSite));
                    return inputSite.matches == 1;
                },
                5000);
            inputSite.report = true;
            inputSite.candidate = nullptr;
            inputSite.matches = 0;
            if ( bridge )
            {
                (void)::EnumChildWindows(
                    bridge, FindInputSiteWindow,
                    reinterpret_cast<LPARAM>(&inputSite));
            }
            const HWND inner = inputSite.candidate;

            if ( !bridge || !::IsWindow(bridge) ||
                 !foundInner || inputSite.matches != 1 ||
                 !::IsWindow(inner) ||
                 ::GetWindowThreadProcessId(bridge, nullptr) !=
                    ::GetCurrentThreadId() ||
                 ::GetWindowThreadProcessId(inner, nullptr) !=
                    ::GetCurrentThreadId() )
            {
                std::fprintf(
                    stderr,
                    "wx_winui_ole_delivery: {\"event\":\"summary\","
                    "\"result\":\"failed\","
                    "\"reason\":\"window-resolution\","
                    "\"bridge\":\"0x%llx\",\"inner\":\"0x%llx\","
                    "\"matches\":%u,\"exit_code\":16}\n",
                    HwndAsInteger(bridge),
                    HwndAsInteger(inner),
                    inputSite.matches);
                rc = 16;
            }
            else
            {
                ReportStage("running opt-in physical OLE delivery matrix");
                rc = RunPhysicalDeliveryProbe(hwnd, bridge, inner);
            }
        }

        // Prove the documented synchronous DispatcherQueue shutdown contract.
        // In particular, ShutdownStarting deferrals keep the queue usable: a
        // timer started from that event must tick before framework teardown is
        // allowed to begin, and ShutdownQueue() must not return before the
        // complete DispatcherQueue/XAML event sequence has finished.
        const auto shutdownState = std::make_shared<
            DispatcherShutdownProbeState>(::GetCurrentThreadId());
        const DispatcherQueue dispatcherQueue =
            dispatcherController.DispatcherQueue();
        DispatcherQueueTimer shutdownTimer = dispatcherQueue.CreateTimer();
        shutdownTimer.Interval(std::chrono::milliseconds(10));
        shutdownTimer.IsRepeating(false);

        const winrt::event_token timerTickToken = shutdownTimer.Tick(
            [shutdownState](
                const DispatcherQueueTimer& timer,
                const winrt::Windows::Foundation::IInspectable&) noexcept
            {
                ++shutdownState->timerTickCount;
                shutdownState->Advance(
                    DispatcherShutdownPhase::MarkerDispatched,
                    DispatcherShutdownPhase::TimerTick);

                try
                {
                    timer.Stop();
                }
                catch ( const winrt::hresult_error& e )
                {
                    shutdownState->Fail(static_cast<HRESULT>(e.code()));
                }
                catch ( ... )
                {
                    shutdownState->Fail();
                }

                shutdownState->CompleteApplicationDeferral();
            });

        const winrt::event_token shutdownStartingToken =
            dispatcherQueue.ShutdownStarting(
                [shutdownState, shutdownTimer](
                    const DispatcherQueue& queue,
                    const DispatcherQueueShutdownStartingEventArgs& args)
                    noexcept
                {
                    ++shutdownState->shutdownStartingCount;
                    shutdownState->Advance(
                        DispatcherShutdownPhase::Active,
                        DispatcherShutdownPhase::ShutdownStarting);

                    try
                    {
                        shutdownState->applicationDeferral =
                            args.GetDeferral();
                        if ( !shutdownState->applicationDeferral )
                        {
                            shutdownState->Fail(E_UNEXPECTED);
                            return;
                        }

                        shutdownState->markerAccepted = queue.TryEnqueue(
                            [shutdownState]() noexcept
                            {
                                ++shutdownState->markerCount;
                                shutdownState->Advance(
                                    DispatcherShutdownPhase::ShutdownStarting,
                                    DispatcherShutdownPhase::MarkerDispatched);
                            });
                        if ( !shutdownState->markerAccepted )
                            shutdownState->Fail(E_UNEXPECTED);

                        shutdownTimer.Start();
                        shutdownState->timerStarted = true;
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        shutdownState->Fail(
                            static_cast<HRESULT>(e.code()));
                        if ( shutdownState->applicationDeferral )
                            shutdownState->CompleteApplicationDeferral();
                    }
                    catch ( ... )
                    {
                        shutdownState->Fail();
                        if ( shutdownState->applicationDeferral )
                            shutdownState->CompleteApplicationDeferral();
                    }
                });

        const winrt::event_token frameworkStartingToken =
            dispatcherQueue.FrameworkShutdownStarting(
                [shutdownState](
                    const DispatcherQueue&,
                    const DispatcherQueueShutdownStartingEventArgs&) noexcept
                {
                    ++shutdownState->frameworkStartingCount;
                    if ( !shutdownState->applicationDeferralCompleteCalled )
                        shutdownState->Fail(E_UNEXPECTED);
                    shutdownState->Advance(
                        DispatcherShutdownPhase::TimerTick,
                        DispatcherShutdownPhase::FrameworkShutdownStarting);
                });

        // XAML raises this while handling FrameworkShutdownStarting. Handler
        // ordering within that event is unspecified, so constrain this passive
        // XAML deferral only between the timer tick and framework completion.
        const winrt::event_token xamlCompletedToken =
            xamlManager.XamlShutdownCompletedOnThread(
                [shutdownState](
                    const WindowsXamlManager&,
                    const XamlShutdownCompletedOnThreadEventArgs& args)
                    noexcept
                {
                    shutdownState->CheckOwnerThread();
                    ++shutdownState->xamlCompletedCount;
                    if ( shutdownState->phase !=
                            DispatcherShutdownPhase::TimerTick &&
                         shutdownState->phase !=
                            DispatcherShutdownPhase::
                                FrameworkShutdownStarting )
                    {
                        shutdownState->Fail(E_UNEXPECTED);
                    }

                    try
                    {
                        const auto deferral =
                            args.GetDispatcherQueueDeferral();
                        if ( !deferral )
                        {
                            shutdownState->Fail(E_UNEXPECTED);
                            return;
                        }

                        // As above, completion can advance shutdown re-entrantly.
                        shutdownState->xamlDeferralCompleteCalled = true;
                        deferral.Complete();
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        shutdownState->Fail(
                            static_cast<HRESULT>(e.code()));
                    }
                    catch ( ... )
                    {
                        shutdownState->Fail();
                    }
                });

        const winrt::event_token frameworkCompletedToken =
            dispatcherQueue.FrameworkShutdownCompleted(
                [shutdownState](
                    const DispatcherQueue& queue,
                    const winrt::Windows::Foundation::IInspectable&) noexcept
                {
                    ++shutdownState->frameworkCompletedCount;
                    if ( shutdownState->xamlCompletedCount != 1 ||
                         !shutdownState->xamlDeferralCompleteCalled )
                    {
                        shutdownState->Fail(E_UNEXPECTED);
                    }
                    shutdownState->Advance(
                        DispatcherShutdownPhase::FrameworkShutdownStarting,
                        DispatcherShutdownPhase::FrameworkShutdownCompleted);

                    try
                    {
                        shutdownState->frameworkEnqueueRejected =
                            !queue.TryEnqueue([]() noexcept {});
                        if ( !shutdownState->frameworkEnqueueRejected )
                            shutdownState->Fail(E_UNEXPECTED);
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        shutdownState->Fail(
                            static_cast<HRESULT>(e.code()));
                    }
                    catch ( ... )
                    {
                        shutdownState->Fail();
                    }
                });

        const winrt::event_token shutdownCompletedToken =
            dispatcherQueue.ShutdownCompleted(
                [shutdownState](
                    const DispatcherQueue&,
                    const winrt::Windows::Foundation::IInspectable&) noexcept
                {
                    ++shutdownState->shutdownCompletedCount;
                    if ( !shutdownState->frameworkEnqueueRejected )
                        shutdownState->Fail(E_UNEXPECTED);
                    shutdownState->Advance(
                        DispatcherShutdownPhase::FrameworkShutdownCompleted,
                        DispatcherShutdownPhase::ShutdownCompleted);
                });

        ReportStage("shutting down: DispatcherQueue deferral timer");
        source.Close();
        ::DestroyWindow(hwnd);
        dispatcherController.ShutdownQueue();

        shutdownState->Advance(
            DispatcherShutdownPhase::ShutdownCompleted,
            DispatcherShutdownPhase::Returned);

        // Keep the manager/application/control graph strongly referenced
        // through ShutdownQueue(), then revoke every callback while its state
        // is still alive. A terminal projection can reject removal, so these
        // are best-effort cleanup and never alter the already-recorded oracle.
        const auto revokeBestEffort = [](auto&& revoke) noexcept
        {
            try
            {
                revoke();
            }
            catch ( ... )
            {
            }
        };
        revokeBestEffort(
            [&]() { shutdownTimer.Tick(timerTickToken); });
        revokeBestEffort(
            [&]() { dispatcherQueue.ShutdownStarting(shutdownStartingToken); });
        revokeBestEffort(
            [&]()
            {
                dispatcherQueue.FrameworkShutdownStarting(
                    frameworkStartingToken);
            });
        revokeBestEffort(
            [&]()
            {
                xamlManager.XamlShutdownCompletedOnThread(xamlCompletedToken);
            });
        revokeBestEffort(
            [&]()
            {
                dispatcherQueue.FrameworkShutdownCompleted(
                    frameworkCompletedToken);
            });
        revokeBestEffort(
            [&]() { dispatcherQueue.ShutdownCompleted(shutdownCompletedToken); });

        const bool shutdownPassed =
            !shutdownState->failed &&
            shutdownState->phase == DispatcherShutdownPhase::Returned &&
            shutdownState->shutdownStartingCount == 1 &&
            shutdownState->markerAccepted &&
            shutdownState->markerCount == 1 &&
            shutdownState->timerStarted &&
            shutdownState->timerTickCount == 1 &&
            shutdownState->applicationDeferralCompleteCalled &&
            !shutdownState->applicationDeferral &&
            shutdownState->frameworkStartingCount == 1 &&
            shutdownState->xamlCompletedCount == 1 &&
            shutdownState->xamlDeferralCompleteCalled &&
            shutdownState->frameworkCompletedCount == 1 &&
            shutdownState->frameworkEnqueueRejected &&
            shutdownState->shutdownCompletedCount == 1;
        if ( !shutdownPassed )
        {
            std::fprintf(
                stderr,
                "wx_winui_runtime_smoke: FAILED: DispatcherQueue shutdown "
                "oracle: phase=%u, HRESULT=0x%08lx, events=%u/%u/%u/%u/%u/"
                "%u/%u, marker=%d, timer=%d, app-deferral=%d, "
                "xaml-deferral=%d, enqueue-rejected=%d\n",
                static_cast<unsigned>(shutdownState->phase),
                static_cast<unsigned long>(shutdownState->firstFailure),
                shutdownState->shutdownStartingCount,
                shutdownState->markerCount,
                shutdownState->timerTickCount,
                shutdownState->frameworkStartingCount,
                shutdownState->xamlCompletedCount,
                shutdownState->frameworkCompletedCount,
                shutdownState->shutdownCompletedCount,
                shutdownState->markerAccepted ? 1 : 0,
                shutdownState->timerStarted ? 1 : 0,
                shutdownState->applicationDeferralCompleteCalled ? 1 : 0,
                shutdownState->xamlDeferralCompleteCalled ? 1 : 0,
                shutdownState->frameworkEnqueueRejected ? 1 : 0);
            if ( rc == 0 )
                rc = 21;
        }
        ReportStage("shutdown complete");
    }
    catch ( const winrt::hresult_error& e )
    {
        ::MddBootstrapShutdown();
        return FailWinRT(5, "runtime smoke", e);
    }

    ::MddBootstrapShutdown();

    if ( rc == 0 )
        std::printf("wx_winui_runtime_smoke: PASSED\n");
    return rc;
}

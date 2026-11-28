/////////////////////////////////////////////////////////////////////////////
// Private, non-installed framework-retirement test adapters.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_RUNTIME_TEST_ACCESS_H
#define WX_WINUI_RUNTIME_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/defs.h"

#include <cstddef>
#include <winrt/Microsoft.UI.Dispatching.h>

#if wxUSE_WINUI3

// The Notebook accessor delegates inside wxcore, so these adapters need no
// DLL export. No state or macro-dependent class layout enters a shared header.
class wxWinUIRuntimeTestAccess final
{
public:
    using FrameworkRetirementPhaseHook = void (*)(void *);

    struct FrameworkRetirementSnapshot
    {
        std::size_t entries = 0;
        std::size_t unboundEntries = 0;
        std::size_t queueStates = 0;
        std::size_t shutdownStartingHooks = 0;
        std::size_t frameworkStartingHooks = 0;
        std::size_t frameworkHooks = 0;
        std::size_t shutdownHooks = 0;
        std::size_t activeStates = 0;
        std::size_t shutdownStartingStates = 0;
        std::size_t frameworkStartingStates = 0;
        std::size_t xamlCompletedStates = 0;
        std::size_t frameworkDoneStates = 0;
        std::size_t shutdownDoneStates = 0;
        bool xamlShutdownHookInstalled = false;
        bool rundown = false;
        bool xamlTerminal = false;
        bool xamlCompletionActive = false;
        bool runtimeTerminal = false;
        bool phaseOrderValid = true;
    };

    // Publish the same shutdown facts without shutting down the suite's
    // process-wide dispatcher. Only the XAML seam consumes retained entries.
    static void SimulateShutdownStarting(
        FrameworkRetirementPhaseHook hook = nullptr,
        void *context = nullptr) noexcept;
    static void SimulateFrameworkShutdownStarting(
        FrameworkRetirementPhaseHook hook = nullptr,
        void *context = nullptr) noexcept;
    static void SimulateFrameworkShutdownCompleted(
        FrameworkRetirementPhaseHook hook = nullptr,
        void *context = nullptr) noexcept;
    static void SimulateShutdownCompleted(
        FrameworkRetirementPhaseHook hook = nullptr,
        void *context = nullptr) noexcept;
    static void SimulateXamlShutdownCompleted(
        FrameworkRetirementPhaseHook hook = nullptr,
        void *context = nullptr) noexcept;
    static void ResetFrameworkRetirementRuntime(
        const winrt::Microsoft::UI::Dispatching::DispatcherQueue& queue) noexcept;
    static void SetFrameworkRetirementHookFault(unsigned faultMask) noexcept;
    static unsigned GetFrameworkRetirementHookFault() noexcept;
    static FrameworkRetirementSnapshot GetFrameworkRetirementSnapshot() noexcept;
    static std::size_t GetFrameworkRetirementCount() noexcept;
};

#endif // wxUSE_WINUI3

#endif // WX_WINUI_RUNTIME_TEST_ACCESS_H

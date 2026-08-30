#include <wx/init.h>
#include <wx/log.h>
#include <wx/msw/wrapwin.h>
#include <wx/renderer.h>
#include <wx/winui/winui.h>

#include <cstdio>
#include <cstring>
#include <memory>
#include <thread>

namespace
{

volatile LONG gs_accessViolationCount = 0;

constexpr wchar_t RuntimeFaultGuardName[] = L"WX_WINUI_TEST_ONLY";
constexpr wchar_t RuntimeFaultName[] =
    L"WX_WINUI_TEST_ONLY_RUNTIME_FAULT";

void UseStderrLogging()
{
    // wxEntryStart and wxEntryCleanup can delete the active log target. Give
    // wx a heap-owned sink and install a fresh one after each such boundary;
    // retaining a stack target or a saved pointer would be unsafe. Expected
    // startup failures must remain visible, never open a modal message box.
    delete wxLog::SetActiveTarget(new wxLogStderr(stderr));
}

const wchar_t *GetRuntimeFaultValue(const char *name)
{
    if ( !name )
        return nullptr;
    if ( std::strcmp(name, "before-bootstrap") == 0 )
        return L"before-bootstrap";
    if ( std::strcmp(name, "after-application") == 0 )
        return L"after-application";
    if ( std::strcmp(name, "after-xaml-manager") == 0 )
        return L"after-xaml-manager";
    if ( std::strcmp(name, "reject-xaml-hook") == 0 )
        return L"reject-xaml-hook";
    if ( std::strcmp(name, "reject-required-queue-hook") == 0 )
        return L"reject-required-queue-hook";
    if ( std::strcmp(name, "throw-shutdown-queue") == 0 )
        return L"throw-shutdown-queue";
    if ( std::strcmp(name, "suppress-framework-completed") == 0 )
        return L"suppress-framework-completed";
    return nullptr;
}

bool RuntimeFaultEnvironmentWasConsumed()
{
    wchar_t inherited[2]{};
    return ::GetEnvironmentVariableW(
               RuntimeFaultGuardName, inherited,
               static_cast<DWORD>(WXSIZEOF(inherited))) == 0 &&
           ::GetEnvironmentVariableW(
               RuntimeFaultName, inherited,
               static_cast<DWORD>(WXSIZEOF(inherited))) == 0;
}

bool ProbeApartment(DWORD mode, HRESULT expected)
{
    const HRESULT result = ::CoInitializeEx(nullptr, mode);
    if ( SUCCEEDED(result) )
        ::CoUninitialize();
    return result == expected;
}

int TestCleanRuntimeFault(int& argc, char** argv)
{
    const HRESULT externalSta =
        ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if ( externalSta != S_OK )
    {
        if ( SUCCEEDED(externalSta) )
            ::CoUninitialize();
        return 26;
    }

    int result = 0;
    bool initializerRejected = false;
    bool envConsumed = false;
    bool callerStaPreserved = false;
    {
        wxInitializer initializer(argc, argv);
        UseStderrLogging();
        initializerRejected = !initializer;
        envConsumed = RuntimeFaultEnvironmentWasConsumed();
        callerStaPreserved =
            ProbeApartment(COINIT_APARTMENTTHREADED, S_FALSE);
        if ( !initializerRejected )
            result = 27;
        else if ( !envConsumed )
            result = 28;
        else if ( !callerStaPreserved )
            result = 29;
    }

    UseStderrLogging();
    ::CoUninitialize();
    bool comBalanced =
        callerStaPreserved && ProbeApartment(COINIT_MULTITHREADED, S_OK);
    if ( !comBalanced && result == 0 )
        result = 30;

    bool recovered = false;
    if ( result == 0 )
    {
        // A second wxInitializer can return true via wx's initialization
        // counter after wxEntryStart failed. Test runtime recovery directly:
        // the consumed fault must leave both owner and state reusable.
        const HRESULT recoverySta =
            ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if ( recoverySta == S_OK )
        {
            const bool initialized = wxWinUI3Initialize();
            const bool repeated = initialized && wxWinUI3Initialize();
            wxWinUI3Uninitialize();
            const bool recoveryStaPreserved =
                ProbeApartment(COINIT_APARTMENTTHREADED, S_FALSE);
            ::CoUninitialize();
            comBalanced = comBalanced && recoveryStaPreserved &&
                ProbeApartment(COINIT_MULTITHREADED, S_OK);
            recovered = initialized && repeated && comBalanced;
        }
        else if ( SUCCEEDED(recoverySta) )
        {
            ::CoUninitialize();
        }
        if ( !recovered )
            result = 31;
    }

    std::printf(
        "wx_winui_runtime_clean_fault: name=before-bootstrap "
        "initializer_rejected=%d env_consumed=%d com_balanced=%d "
        "recovered=%d result=%d\n",
        initializerRejected ? 1 : 0, envConsumed ? 1 : 0,
        comBalanced ? 1 : 0, recovered ? 1 : 0, result);
    std::fflush(stdout);
    return result;
}

class CountingRenderer : public wxDelegateRendererNative
{
public:
    explicit CountingRenderer(unsigned& destroyed)
        // Do not call Get(): the custom-before-first-Get contract must also
        // cover the first lazy renderer lookup made during wxInitializer.
        : wxDelegateRendererNative(wxRendererNative::GetDefault()),
          m_destroyed(destroyed)
    {
    }

    ~CountingRenderer() override { ++m_destroyed; }

private:
    unsigned& m_destroyed;
};

bool RestartWithRenderer(wxRendererNative *expected)
{
    wxWinUI3Uninitialize();
    return &wxRendererNative::Get() == expected &&
           wxWinUI3Initialize() && &wxRendererNative::Get() == expected;
}

int TestRendererOwnership(int& argc, char** argv, bool customBeforeInit)
{
    unsigned initialDestroyed = 0;
    unsigned replacementDestroyed = 0;
    unsigned epochs = 0;
    int result = 0;
    wxRendererNative *initial = nullptr;
    if ( customBeforeInit )
    {
        initial = new CountingRenderer(initialDestroyed);
        // Set() must not implicitly mark the first Get() as already done.
        // No Get() and no wxInitializer has run in this fresh process yet.
        std::unique_ptr<wxRendererNative> previous(
            wxRendererNative::Set(initial));
        if ( previous )
            result = 32;
    }

    std::unique_ptr<wxRendererNative> initialOwner;
    std::unique_ptr<wxRendererNative> replacementOwner;
    bool liveReplacementPreserved = false;
    bool explicitDefaultPreserved = false;
    {
        wxInitializer initializer(argc, argv);
        UseStderrLogging();
        if ( !initializer )
            result = 33;
        else if ( result == 0 )
        {
            wxRendererNative * const selected = &wxRendererNative::Get();
            if ( customBeforeInit )
            {
                if ( selected != initial || initialDestroyed != 0 )
                    result = 34;
            }
            else if ( selected == &wxRendererNative::GetDefault() )
            {
                // WinUI's process default is the Fluent renderer, distinct
                // from the native renderer used as its drawing delegate.
                result = 35;
            }

            for ( ; result == 0 && epochs < 50; ++epochs )
            {
                if ( !RestartWithRenderer(selected) || initialDestroyed != 0 )
                    result = 36;
            }

            if ( result == 0 )
            {
                auto * const replacement =
                    new CountingRenderer(replacementDestroyed);
                initialOwner.reset(wxRendererNative::Set(replacement));
                liveReplacementPreserved = initialOwner.get() == selected &&
                    initialDestroyed == 0 && replacementDestroyed == 0 &&
                    &wxRendererNative::Get() == replacement &&
                    wxWinUI3Initialize() &&
                    &wxRendererNative::Get() == replacement &&
                    RestartWithRenderer(replacement) &&
                    initialDestroyed == 0 && replacementDestroyed == 0;
                if ( !liveReplacementPreserved )
                    result = 37;

                replacementOwner.reset(wxRendererNative::Set(nullptr));
                wxRendererNative * const nativeDefault =
                    &wxRendererNative::GetDefault();
                explicitDefaultPreserved =
                    replacementOwner.get() == replacement &&
                    &wxRendererNative::Get() == nativeDefault &&
                    wxWinUI3Initialize() &&
                    &wxRendererNative::Get() == nativeDefault &&
                    RestartWithRenderer(nativeDefault) &&
                    initialDestroyed == 0 && replacementDestroyed == 0;
                if ( !explicitDefaultPreserved && result == 0 )
                    result = 38;
            }
        }
    }

    UseStderrLogging();

    // Set() hands the displaced renderer to its caller; neither a runtime
    // epoch nor wx module cleanup may delete these caller-owned objects.
    if ( (initialDestroyed != 0 || replacementDestroyed != 0) && result == 0 )
        result = 39;
    initialOwner.reset();
    replacementOwner.reset();
    if ( (initialDestroyed != (customBeforeInit ? 1u : 0u) ||
          replacementDestroyed != 1) && result == 0 )
    {
        result = 40;
    }

    // Also clean up a still-selected custom renderer on a failing path while
    // its counter remains alive. Only the pointer returned by Set() is owned.
    delete wxRendererNative::Set(nullptr);
    const bool comBalanced = ProbeApartment(COINIT_MULTITHREADED, S_OK);
    if ( !comBalanced && result == 0 )
        result = 41;

    std::printf(
        "wx_winui_renderer: mode=%s epochs=%u live_preserved=%d "
        "default_preserved=%d initial_destroyed=%u replacement_destroyed=%u "
        "com_balanced=%d result=%d\n",
        customBeforeInit ? "custom" : "default", epochs,
        liveReplacementPreserved ? 1 : 0, explicitDefaultPreserved ? 1 : 0,
        initialDestroyed, replacementDestroyed, comBalanced ? 1 : 0, result);
    std::fflush(stdout);
    return result;
}

LONG CALLBACK ObserveAccessViolations(EXCEPTION_POINTERS *exception)
{
    if ( exception && exception->ExceptionRecord &&
         exception->ExceptionRecord->ExceptionCode ==
             EXCEPTION_ACCESS_VIOLATION )
    {
        ::InterlockedIncrement(&gs_accessViolationCount);
        // This is a dedicated crash probe executable. Terminate while the
        // observer still lives in the EXE so an AV during DLL/CRT teardown
        // cannot be absorbed after main() has already reported success.
        ::TerminateProcess(::GetCurrentProcess(), 5);
    }

    // Non-AV exceptions continue through the framework's normal handlers.
    return EXCEPTION_CONTINUE_SEARCH;
}

} // namespace

int main(int argc, char** argv)
{
    std::printf("wx_winui_install_consumer: PID=%lu\n",
                static_cast<unsigned long>(::GetCurrentProcessId()));
    std::fflush(stdout);

    void * const exceptionObserver =
        ::AddVectoredExceptionHandler(1, ObserveAccessViolations);
    if ( !exceptionObserver )
        return 4;

    UseStderrLogging();

    // Never let an inherited test guard affect the normal or MTA probes. A
    // fault child re-arms both exact variables below immediately before wx
    // module initialization.
    (void)::SetEnvironmentVariableW(RuntimeFaultGuardName, nullptr);
    (void)::SetEnvironmentVariableW(RuntimeFaultName, nullptr);

    const bool testMtaRejection =
        argc == 2 && std::strcmp(argv[1], "--mta-reject") == 0;
    const bool testRendererCustom =
        argc == 2 && std::strcmp(argv[1], "--renderer-custom") == 0;
    const bool testRendererDefault =
        argc == 2 && std::strcmp(argv[1], "--renderer-default") == 0;
    const bool testRuntimeFault =
        argc == 3 && std::strcmp(argv[1], "--runtime-fault") == 0;
    const char * const runtimeFaultName = testRuntimeFault ? argv[2] : nullptr;
    const wchar_t * const runtimeFault = testRuntimeFault
        ? GetRuntimeFaultValue(runtimeFaultName)
        : nullptr;

    if ( (testRuntimeFault && !runtimeFault) ||
         (argc != 1 && !testMtaRejection && !testRuntimeFault &&
          !testRendererCustom && !testRendererDefault) )
    {
        ::RemoveVectoredExceptionHandler(exceptionObserver);
        return 18;
    }

    if ( testRendererCustom || testRendererDefault )
        return TestRendererOwnership(argc, argv, testRendererCustom);

    if ( testRuntimeFault )
    {
        if ( !::SetEnvironmentVariableW(RuntimeFaultGuardName, L"1") ||
             !::SetEnvironmentVariableW(RuntimeFaultName, runtimeFault) )
        {
            ::RemoveVectoredExceptionHandler(exceptionObserver);
            return 19;
        }

        if ( std::strcmp(runtimeFaultName, "before-bootstrap") == 0 )
            return TestCleanRuntimeFault(argc, argv);

        // As in the normal consumer path, let wx enter an externally-owned
        // STA. A quarantined epoch must retain only its own additional
        // apartment reference and must never consume this one.
        const HRESULT externalSta =
            ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if ( externalSta != S_OK )
        {
            if ( SUCCEEDED(externalSta) )
                ::CoUninitialize();
            ::RemoveVectoredExceptionHandler(exceptionObserver);
            return 20;
        }

        int result = 0;
        bool initializerRejected = false;
        bool envConsumed = false;
        bool bothInitializationsRejected = false;
        bool staActiveInsideScope = false;
        {
            wxInitializer initializer(argc, argv);
            UseStderrLogging();
            initializerRejected = !initializer;
            if ( initializer )
            {
                result = 21;
            }
            else
            {
                envConsumed = RuntimeFaultEnvironmentWasConsumed();

                // wxWinUI3Module::OnInit() consumed the one-shot fault. Both
                // explicit attempts must now observe the permanent
                // Quarantined state, and Uninitialize() must remain inert.
                const bool firstInitialized = wxWinUI3Initialize();
                const bool secondInitialized = wxWinUI3Initialize();
                wxWinUI3Uninitialize();
                bothInitializationsRejected =
                    !firstInitialized && !secondInitialized;

                const HRESULT stillSta =
                    ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
                staActiveInsideScope = stillSta == S_FALSE;
                if ( SUCCEEDED(stillSta) )
                    ::CoUninitialize();

                if ( !envConsumed )
                    result = 22;
                else if ( !bothInitializationsRejected )
                    result = 23;
                else if ( !staActiveInsideScope )
                    result = 24;
            }
        }

        UseStderrLogging();

        // Balance only the caller-owned reference. The deliberately immortal
        // quarantine must leave its own STA reference active through process
        // termination, including after wx module teardown.
        ::CoUninitialize();
        const HRESULT retainedSta =
            ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        const bool staRetainedAfterWx = retainedSta == S_FALSE;
        if ( SUCCEEDED(retainedSta) )
            ::CoUninitialize();
        if ( !staRetainedAfterWx && result == 0 )
            result = 25;

        const LONG accessViolations = ::InterlockedCompareExchange(
            &gs_accessViolationCount, 0, 0);
        if ( accessViolations )
            result = 5;

        std::printf(
            "wx_winui_runtime_fault: name=%s initializer_rejected=%d env_consumed=%d "
            "init_rejected=%d sta_retained=%d result=%d\n",
            runtimeFaultName, initializerRejected ? 1 : 0, envConsumed ? 1 : 0,
            bothInitializationsRejected ? 1 : 0,
            staRetainedAfterWx ? 1 : 0, result);
        std::fflush(stdout);
        return result;
    }

    if ( testMtaRejection )
    {
        const HRESULT mtaResult =
            ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if ( mtaResult != S_OK )
        {
            if ( SUCCEEDED(mtaResult) )
                ::CoUninitialize();
            return 7;
        }

        int result = 0;
        bool initializerRejected = false;
        bool runtimeRejected = false;
        bool mtaPreserved = false;
        bool recovered = false;
        bool comBalanced = false;
        {
            wxInitializer initializer(argc, argv);
            UseStderrLogging();
            initializerRejected = !initializer;
            if ( initializer )
            {
                result = 8;
            }
            else
            {
                runtimeRejected = !wxWinUI3Initialize();
                if ( !runtimeRejected )
                {
                    // XAML Islands require an STA. The rejected attempt must
                    // not publish a usable MTA-backed runtime.
                    result = 9;
                }

                const HRESULT stillMta =
                    ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
                mtaPreserved = stillMta == S_FALSE;
                if ( stillMta == S_FALSE )
                {
                    ::CoUninitialize();
                    ::CoUninitialize();
                }
                else
                {
                    // S_OK owns only the fresh probe reference because wx
                    // consumed the caller's reference. On failure, balance
                    // the caller's still-outstanding reference instead.
                    ::CoUninitialize();
                    if ( result == 0 )
                        result = 10;
                }

                // A rejected MTA attempt must leave owner/state reusable in
                // the same process, not merely avoid leaking COM references.
                const HRESULT recoverySta =
                    ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
                if ( recoverySta != S_OK )
                {
                    if ( SUCCEEDED(recoverySta) )
                        ::CoUninitialize();
                    if ( result == 0 )
                        result = 14;
                }
                else
                {
                    if ( !wxWinUI3Initialize() )
                    {
                        if ( result == 0 )
                            result = 15;
                    }
                    else
                    {
                        recovered = true;
                        wxWinUI3Uninitialize();
                    }

                    const HRESULT stillSta =
                        ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
                    if ( stillSta == S_FALSE )
                    {
                        // Balance the probe and the caller-owned STA.
                        ::CoUninitialize();
                        ::CoUninitialize();
                    }
                    else
                    {
                        // S_OK owns only the probe because the caller's STA
                        // was consumed; failure leaves only the caller's STA.
                        ::CoUninitialize();
                        if ( result == 0 )
                            result = 16;
                    }
                }

                const HRESULT freshMta =
                    ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
                comBalanced = freshMta == S_OK;
                if ( freshMta != S_OK && result == 0 )
                    result = 17;
                if ( SUCCEEDED(freshMta) )
                    ::CoUninitialize();
            }
        }

        UseStderrLogging();

        const LONG accessViolations = ::InterlockedCompareExchange(
            &gs_accessViolationCount, 0, 0);
        std::printf(
            "wx_winui_mta_reject: initializer_rejected=%d runtime_rejected=%d "
            "mta_preserved=%d recovered=%d com_balanced=%d result=%d\n",
            initializerRejected ? 1 : 0, runtimeRejected ? 1 : 0,
            mtaPreserved ? 1 : 0, recovered ? 1 : 0,
            comBalanced ? 1 : 0, result);
        std::fflush(stdout);
        return accessViolations ? 5 : result;
    }

    // Make the first wx runtime epoch enter an already initialized STA. Its
    // S_FALSE result still owns one COM/WinRT reference and must be balanced
    // without consuming this external reference.
    const HRESULT externalSta =
        ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if ( externalSta != S_OK )
    {
        if ( SUCCEEDED(externalSta) )
            ::CoUninitialize();
        ::RemoveVectoredExceptionHandler(exceptionObserver);
        return 11;
    }

    int result = 0;
    {
        wxInitializer initializer(argc, argv);
        UseStderrLogging();
        if ( !initializer )
        {
            result = 1;
        }
        else if ( !wxWinUI3Initialize() )
        {
            // wxInitializer loads the wx modules, including the WinUI
            // bootstrap module. Querying it explicitly makes a missing or
            // incomplete deployed payload a deterministic smoke-test failure.
            result = 2;
        }
        else
        {
            bool wrongThreadClean = false;
            std::thread wrongThread([&wrongThreadClean]
            {
                const HRESULT workerMta =
                    ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
                if ( workerMta != S_OK )
                {
                    if ( SUCCEEDED(workerMta) )
                        ::CoUninitialize();
                    return;
                }

                const bool initialized = wxWinUI3Initialize();
                wxWinUI3Uninitialize();

                const HRESULT stillWorkerMta =
                    ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
                const bool mtaBalanced = stillWorkerMta == S_FALSE;
                if ( SUCCEEDED(stillWorkerMta) )
                    ::CoUninitialize();
                ::CoUninitialize();

                const HRESULT freshWorkerSta =
                    ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
                const bool workerReleased = freshWorkerSta == S_OK;
                if ( SUCCEEDED(freshWorkerSta) )
                    ::CoUninitialize();

                wrongThreadClean =
                    !initialized && mtaBalanced && workerReleased;
            });
            wrongThread.join();
            if ( !wrongThreadClean || !wxWinUI3Initialize() )
            {
                result = 3;
            }

            // Exercise repeated complete runtime epochs. Every one must
            // balance its apartment reference while the process-lifetime XAML
            // module pin remains safely reusable.
            for ( unsigned cycle = 0; result == 0 && cycle < 50; ++cycle )
            {
                wxWinUI3Uninitialize();
                if ( !wxWinUI3Initialize() )
                    result = 12;
            }
        }
    }

    UseStderrLogging();

    // wxWinUI must not consume the caller's external STA reference.
    const HRESULT stillSta =
        ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if ( stillSta == S_FALSE )
    {
        ::CoUninitialize();
        ::CoUninitialize();
    }
    else
    {
        // See the equivalent MTA branch above for the ownership split.
        ::CoUninitialize();
        if ( result == 0 )
            result = 13;
    }

    // After balancing the external STA reference, a fresh MTA returning S_OK
    // proves that no wx epoch leaked an apartment reference.
    const HRESULT postShutdownMta =
        ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if ( SUCCEEDED(postShutdownMta) )
        ::CoUninitialize();
    if ( postShutdownMta != S_OK && result == 0 )
    {
        result = 6;
    }

    const LONG accessViolations = ::InterlockedCompareExchange(
        &gs_accessViolationCount, 0, 0);

    // Some COM cleanup paths report and absorb a first-chance access violation,
    // leaving the process exit code at zero. Never let that masquerade as a
    // successful installed-consumer smoke test.
    return accessViolations ? 5 : result;
}

/////////////////////////////////////////////////////////////////////////////
// Private external UIA client launcher for test-enabled WinUI builds.
// This source is not installed or compiled into shipping libraries.
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "combobox-uia-helper.h"
#include "combobox-test-access.h"
#include "wx/scopeguard.h"

#include <UIAutomation.h>
#include <cstdint>
#include <thread>
#include <vector>

#if wxUSE_COMBOBOX

namespace
{

constexpr std::uint32_t wxWinUIUiaResultMagic = 0x57495541;
constexpr std::uint32_t wxWinUIUiaResultVersion = 1;
constexpr DWORD wxWinUIUiaHelperTimeoutMs = 1500;

struct wxWinUIUiaSetValueResult
{
    std::uint32_t magic;
    std::uint32_t version;
    std::int32_t stage;
    std::int32_t hresult;
};
static_assert(sizeof(wxWinUIUiaSetValueResult) == 16,
              "UIA helper protocol must remain fixed-width");

std::wstring wxWinUIQuoteCommandLineArgument(const std::wstring& argument)
{
    if ( argument.empty() ||
         argument.find_first_of(L" \t\n\v\"") != std::wstring::npos )
    {
        std::wstring quoted(1, L'"');
        std::size_t backslashes = 0;
        for ( const wchar_t ch : argument )
        {
            if ( ch == L'\\' )
            {
                ++backslashes;
                continue;
            }
            if ( ch == L'"' )
            {
                quoted.append(backslashes * 2 + 1, L'\\');
                quoted.push_back(L'"');
            }
            else
            {
                quoted.append(backslashes, L'\\');
                quoted.push_back(ch);
            }
            backslashes = 0;
        }
        quoted.append(backslashes * 2, L'\\');
        quoted.push_back(L'"');
        return quoted;
    }
    return argument;
}

class wxWinUIComboUniqueHandle
{
public:
    wxWinUIComboUniqueHandle() = default;
    explicit wxWinUIComboUniqueHandle(HANDLE handle) : m_handle(handle) {}
    wxWinUIComboUniqueHandle(const wxWinUIComboUniqueHandle&) = delete;
    wxWinUIComboUniqueHandle& operator=(
        const wxWinUIComboUniqueHandle&) = delete;
    ~wxWinUIComboUniqueHandle()
    {
        Reset();
    }

    HANDLE Get() const { return m_handle; }
    explicit operator bool() const
    {
        return m_handle && m_handle != INVALID_HANDLE_VALUE;
    }
    HANDLE Release()
    {
        const HANDLE handle = m_handle;
        m_handle = nullptr;
        return handle;
    }
    void Reset(HANDLE handle = nullptr)
    {
        if ( *this )
            ::CloseHandle(m_handle);
        m_handle = handle;
    }

private:
    HANDLE m_handle{nullptr};
};

struct wxWinUIUiaHelperWatchContext
{
    wxWinUIComboUniqueHandle process;
    wxWinUIComboUniqueHandle resultRead;
};

} // anonymous namespace

wxWinUIUiaHelperLaunchResult wxWinUILaunchUiaSetValueHelper(
    const std::shared_ptr<wxWinUIComboAutomationTestState>& operation,
    HWND bridge,
    const std::wstring& outerAutomationId,
    const std::wstring& editAutomationId,
    const std::wstring& requestedValue)
{
    wxWinUIUiaHelperLaunchResult launch;
    DWORD bridgePid = 0;
    if ( !bridge || !::IsWindow(bridge) ||
         !::GetWindowThreadProcessId(bridge, &bridgePid) ||
         bridgePid != ::GetCurrentProcessId() )
    {
        launch.hresult = UIA_E_ELEMENTNOTAVAILABLE;
        return launch;
    }

    const DWORD helperLength = ::GetEnvironmentVariableW(
        L"WX_WINUI_UIA_HELPER", nullptr, 0);
    if ( !helperLength )
    {
        launch.hresult = HRESULT_FROM_WIN32(::GetLastError());
        return launch;
    }
    std::vector<wchar_t> helperBuffer(helperLength, L'\0');
    const DWORD copied = ::GetEnvironmentVariableW(
        L"WX_WINUI_UIA_HELPER", helperBuffer.data(), helperLength);
    if ( !copied || copied >= helperLength )
    {
        launch.hresult = HRESULT_FROM_WIN32(::GetLastError());
        return launch;
    }
    const std::wstring helperPath(helperBuffer.data(), copied);

    SECURITY_ATTRIBUTES inheritable{};
    inheritable.nLength = sizeof(inheritable);
    inheritable.bInheritHandle = TRUE;
    HANDLE readRaw = nullptr;
    HANDLE writeRaw = nullptr;
    if ( !::CreatePipe(&readRaw, &writeRaw, &inheritable, 0) )
    {
        launch.hresult = HRESULT_FROM_WIN32(::GetLastError());
        return launch;
    }
    wxWinUIComboUniqueHandle readPipe(readRaw);
    wxWinUIComboUniqueHandle writePipe(writeRaw);
    if ( !::SetHandleInformation(readPipe.Get(), HANDLE_FLAG_INHERIT, 0) )
    {
        launch.hresult = HRESULT_FROM_WIN32(::GetLastError());
        return launch;
    }

    STARTUPINFOEXW startup{};
    startup.StartupInfo.cb = sizeof(startup);
    SIZE_T attributeBytes = 0;
    (void)::InitializeProcThreadAttributeList(
        nullptr, 1, 0, &attributeBytes);
    if ( !attributeBytes )
    {
        launch.hresult = HRESULT_FROM_WIN32(::GetLastError());
        return launch;
    }
    std::vector<unsigned char> attributeStorage(attributeBytes);
    startup.lpAttributeList =
        reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(
            attributeStorage.data());
    if ( !::InitializeProcThreadAttributeList(
            startup.lpAttributeList, 1, 0, &attributeBytes) )
    {
        launch.hresult = HRESULT_FROM_WIN32(::GetLastError());
        return launch;
    }
    wxScopeGuard attributeGuard = wxMakeGuard([&startup]()
    {
        ::DeleteProcThreadAttributeList(startup.lpAttributeList);
    });
    wxUnusedVar(attributeGuard);

    HANDLE inheritedHandle = writePipe.Get();
    if ( !::UpdateProcThreadAttribute(
            startup.lpAttributeList, 0,
            PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
            &inheritedHandle, sizeof(inheritedHandle), nullptr, nullptr) )
    {
        launch.hresult = HRESULT_FROM_WIN32(::GetLastError());
        return launch;
    }

    const std::wstring commandLine =
        wxWinUIQuoteCommandLineArgument(helperPath) +
        L" --uia-set-value " +
        std::to_wstring(reinterpret_cast<std::uintptr_t>(writePipe.Get())) +
        L" " +
        std::to_wstring(reinterpret_cast<std::uintptr_t>(bridge)) +
        L" " + std::to_wstring(static_cast<unsigned long>(bridgePid)) +
        L" " + wxWinUIQuoteCommandLineArgument(outerAutomationId) +
        L" " + wxWinUIQuoteCommandLineArgument(editAutomationId) +
        L" " + wxWinUIQuoteCommandLineArgument(requestedValue);
    std::vector<wchar_t> mutableCommand(
        commandLine.begin(), commandLine.end());
    mutableCommand.push_back(L'\0');

    // Allocate all potentially throwing ownership state before the external
    // process exists. After CreateProcess succeeds, every handle is either in
    // this context or synchronously terminated/waited in the guarded paths.
    const auto watchContext =
        std::make_shared<wxWinUIUiaHelperWatchContext>();
    PROCESS_INFORMATION processInfo{};
    if ( !::CreateProcessW(
            helperPath.c_str(), mutableCommand.data(), nullptr, nullptr,
            TRUE, CREATE_NO_WINDOW | EXTENDED_STARTUPINFO_PRESENT,
            nullptr, nullptr, &startup.StartupInfo, &processInfo) )
    {
        launch.hresult = HRESULT_FROM_WIN32(::GetLastError());
        return launch;
    }
    wxWinUIComboUniqueHandle helperProcess(processInfo.hProcess);
    wxWinUIComboUniqueHandle helperThread(processInfo.hThread);

    // The parent must never retain the writer: after helper exit ReadFile is
    // guaranteed to finish and a malformed/short protocol record is visible.
    writePipe.Reset();
    helperThread.Reset();
    watchContext->process.Reset(helperProcess.Release());
    watchContext->resultRead.Reset(readPipe.Release());

    try
    {
        std::thread watcher(
            [operation, watchContext]()
            {
                const HANDLE processHandle = watchContext->process.Get();
                const HANDLE readHandle = watchContext->resultRead.Get();

                const auto publishResult = [&]()
                {
                    wxWinUIUiaSetValueResult result{};
                    DWORD bytesRead = 0;
                    const bool complete = ::ReadFile(
                        readHandle, &result, sizeof(result), &bytesRead,
                        nullptr) && bytesRead == sizeof(result);
                    const bool valid = complete &&
                        result.magic == wxWinUIUiaResultMagic &&
                        result.version == wxWinUIUiaResultVersion &&
                        (result.stage ==
                             wxWinUIComboBoxTestAccess::WinUIAutomation_Succeeded ||
                         result.stage < 0);
                    if ( valid )
                    {
                        operation->hresult.store(
                            static_cast<long>(result.hresult),
                            std::memory_order_relaxed);
                        operation->stage.store(
                            result.stage, std::memory_order_release);
                    }
                    else
                    {
                        operation->hresult.store(
                            static_cast<long>(E_INVALIDARG),
                            std::memory_order_relaxed);
                        operation->stage.store(-23,
                                               std::memory_order_release);
                    }
                };

                DWORD waitResult = ::WaitForSingleObject(
                    processHandle, wxWinUIUiaHelperTimeoutMs);
                if ( waitResult == WAIT_TIMEOUT )
                {
                    // Close the boundary race where the helper exits between
                    // the timed wait and TerminateProcess().
                    const DWORD finalProbe =
                        ::WaitForSingleObject(processHandle, 0);
                    if ( finalProbe == WAIT_OBJECT_0 )
                        waitResult = WAIT_OBJECT_0;
                }
                if ( waitResult == WAIT_OBJECT_0 )
                {
                    publishResult();
                    return;
                }

                const HRESULT waitHr = waitResult == WAIT_TIMEOUT
                    ? HRESULT_FROM_WIN32(WAIT_TIMEOUT)
                    : HRESULT_FROM_WIN32(::GetLastError());
                operation->hresult.store(static_cast<long>(waitHr),
                                         std::memory_order_relaxed);

                // Publish a replaceable failure only after proving the old
                // helper is dead. If termination/wait itself fails, retain a
                // positive stage so single-flight blocks any overlapping
                // helper which could still complete SetValue later.
                const bool terminated =
                    ::TerminateProcess(processHandle, 124) != FALSE;
                const DWORD terminatedWait =
                    ::WaitForSingleObject(processHandle, 5000);
                if ( terminatedWait == WAIT_OBJECT_0 && !terminated )
                {
                    // TerminateProcess commonly reports access denied when
                    // natural exit won the race. Its exact record remains
                    // authoritative in that case.
                    publishResult();
                }
                else
                {
                    operation->stage.store(
                        terminatedWait == WAIT_OBJECT_0 ? -23 : 23,
                        std::memory_order_release);
                }
            });
        try
        {
            watcher.detach();
        }
        catch ( const std::system_error& e )
        {
            launch.hresult = HRESULT_FROM_WIN32(
                static_cast<unsigned long>(e.code().value()));
            (void)::TerminateProcess(watchContext->process.Get(), 125);
            const DWORD terminatedWait = ::WaitForSingleObject(
                watchContext->process.Get(), 5000);
            if ( watcher.joinable() )
                watcher.join();
            if ( terminatedWait != WAIT_OBJECT_0 )
            {
                operation->hresult.store(
                    static_cast<long>(launch.hresult),
                    std::memory_order_relaxed);
                operation->stage.store(23,
                                       std::memory_order_release);
            }
            // The watcher ran and is now joined, so its final protocol/fail
            // state remains authoritative even though detach itself failed.
            launch.scheduled = true;
            return launch;
        }
    }
    catch ( const std::system_error& e )
    {
        launch.hresult = HRESULT_FROM_WIN32(
            static_cast<unsigned long>(e.code().value()));
        const bool terminated =
            ::TerminateProcess(watchContext->process.Get(), 125) != FALSE;
        wxUnusedVar(terminated);
        const DWORD terminatedWait =
            ::WaitForSingleObject(watchContext->process.Get(), 5000);
        if ( !terminated && terminatedWait == WAIT_OBJECT_0 )
        {
            wxWinUIUiaSetValueResult result{};
            DWORD bytesRead = 0;
            const bool complete = ::ReadFile(
                watchContext->resultRead.Get(), &result, sizeof(result),
                &bytesRead, nullptr) && bytesRead == sizeof(result);
            const bool valid = complete &&
                result.magic == wxWinUIUiaResultMagic &&
                result.version == wxWinUIUiaResultVersion &&
                (result.stage ==
                     wxWinUIComboBoxTestAccess::WinUIAutomation_Succeeded ||
                 result.stage < 0);
            if ( valid )
            {
                operation->hresult.store(
                    static_cast<long>(result.hresult),
                    std::memory_order_relaxed);
                operation->stage.store(result.stage,
                                       std::memory_order_release);
                launch.scheduled = true;
                launch.hresult = S_OK;
                return launch;
            }
            launch.hresult = E_INVALIDARG;
        }
        if ( terminatedWait != WAIT_OBJECT_0 )
        {
            // The helper still owns the pipe and may still write. Keep the
            // operation in-flight and let the test report the shielded state.
            operation->hresult.store(static_cast<long>(launch.hresult),
                                     std::memory_order_relaxed);
            operation->stage.store(23, std::memory_order_release);
            launch.scheduled = true;
        }
        return launch;
    }

    launch.scheduled = true;
    launch.hresult = S_OK;
    return launch;
}

#endif // wxUSE_COMBOBOX

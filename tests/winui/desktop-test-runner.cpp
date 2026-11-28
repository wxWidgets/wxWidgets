///////////////////////////////////////////////////////////////////////////////
// Name:        tests/winui/desktop-test-runner.cpp
// Purpose:     Run focus-sensitive WinUI tests on an isolated Win32 desktop
// Author:      wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <UIAutomation.h>
#include <UIAutomationClient.h>

#include <wrl/client.h>

#include <cstdint>
#include <cstdio>
#include <cwchar>
#include <cerrno>
#include <exception>
#include <limits>
#include <string>
#include <thread>
#include <vector>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "uiautomationcore.lib")

namespace
{

constexpr DWORD ExitSetupFailure = 125;
constexpr DWORD ExitTimeout = 124;
constexpr DWORD MaximumTimeoutMs = 10 * 60 * 1000;
constexpr DWORD CleanupTimeoutMs = 5000;
constexpr std::uint32_t UiaResultMagic = 0x57495541; // "WIUA"
constexpr std::uint32_t UiaResultVersion = 1;

struct UiaSetValueResult
{
    std::uint32_t magic{UiaResultMagic};
    std::uint32_t version{UiaResultVersion};
    std::int32_t stage{-16};
    std::int32_t hresult{static_cast<std::int32_t>(E_FAIL)};
};
static_assert(sizeof(UiaSetValueResult) == 16,
              "UIA helper protocol must remain fixed-width");

class UniqueHandle
{
public:
    UniqueHandle() = default;
    explicit UniqueHandle(HANDLE handle) : m_handle(handle) {}

    UniqueHandle(const UniqueHandle&) = delete;
    UniqueHandle& operator=(const UniqueHandle&) = delete;

    ~UniqueHandle()
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

void ReportWin32Error(const wchar_t* operation)
{
    std::fwprintf(
        stderr,
        L"wxWinUI isolated desktop runner: %ls failed (error %lu).\n",
        operation,
        static_cast<unsigned long>(::GetLastError()));
}

std::wstring QuoteCommandLineArgument(const std::wstring& argument)
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

std::wstring BuildCommandLine(int argc, wchar_t** argv, int firstArgument)
{
    std::wstring commandLine;
    for ( int i = firstArgument; i < argc; ++i )
    {
        if ( !commandLine.empty() )
            commandLine.push_back(L' ');
        commandLine += QuoteCommandLineArgument(argv[i]);
    }
    return commandLine;
}

bool ParseUnsigned64(const wchar_t* value, std::uint64_t* parsed)
{
    if ( !value || !value[0] || !parsed )
        return false;

    for ( const wchar_t* current = value; *current; ++current )
    {
        if ( *current < L'0' || *current > L'9' )
            return false;
    }

    wchar_t* end = nullptr;
    errno = 0;
    const unsigned long long result = _wcstoui64(value, &end, 10);
    if ( errno == ERANGE || !end || *end != L'\0' )
        return false;
    *parsed = static_cast<std::uint64_t>(result);
    return true;
}

int WriteUiaResult(HANDLE resultPipe, const UiaSetValueResult& result)
{
    DWORD written = 0;
    const bool complete = resultPipe &&
        ::WriteFile(resultPipe, &result, sizeof(result), &written, nullptr) &&
        written == sizeof(result);
    if ( resultPipe )
        ::CloseHandle(resultPipe);
    return complete && result.stage == 24 ? 0 : ExitSetupFailure;
}

int RunUiaSetValueHelper(int argc, wchar_t** argv)
{
    // argv: helper --uia-set-value result-pipe bridge-hwnd target-pid
    //       outer-automation-id edit-automation-id value
    if ( argc != 8 )
        return ExitSetupFailure;

    std::uint64_t pipeValue = 0;
    std::uint64_t bridgeValue = 0;
    std::uint64_t targetPidValue = 0;
    if ( !ParseUnsigned64(argv[2], &pipeValue) ||
         !ParseUnsigned64(argv[3], &bridgeValue) ||
         !ParseUnsigned64(argv[4], &targetPidValue) ||
         pipeValue == 0 || bridgeValue == 0 ||
         pipeValue > (std::numeric_limits<std::uintptr_t>::max)() ||
         bridgeValue > (std::numeric_limits<std::uintptr_t>::max)() ||
         targetPidValue == 0 || targetPidValue > MAXDWORD )
    {
        return ExitSetupFailure;
    }

    const HANDLE resultPipe = reinterpret_cast<HANDLE>(
        static_cast<std::uintptr_t>(pipeValue));
    const HWND bridge = reinterpret_cast<HWND>(
        static_cast<std::uintptr_t>(bridgeValue));
    const DWORD targetPid = static_cast<DWORD>(targetPidValue);
    const std::wstring outerAutomationId(argv[5]);
    const std::wstring editAutomationId(argv[6]);
    const std::wstring requestedValue(argv[7]);
    UiaSetValueResult result;

    const auto finishFailure = [&](int stage, HRESULT hr)
    {
        result.stage = -stage;
        result.hresult = static_cast<std::int32_t>(hr);
        return WriteUiaResult(resultPipe, result);
    };

    DWORD actualPid = 0;
    if ( !::IsWindow(bridge) ||
         !::GetWindowThreadProcessId(bridge, &actualPid) ||
         actualPid != targetPid )
    {
        return finishFailure(16, UIA_E_ELEMENTNOTAVAILABLE);
    }

    HRESULT hr = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if ( FAILED(hr) )
        return finishFailure(16, hr);
    struct CoUninitializeGuard
    {
        ~CoUninitializeGuard() { ::CoUninitialize(); }
    } coGuard;
    (void)coGuard;

    using Microsoft::WRL::ComPtr;
    ComPtr<IUIAutomation> automation;
    hr = ::CoCreateInstance(
        __uuidof(CUIAutomation8), nullptr, CLSCTX_INPROC_SERVER,
        __uuidof(IUIAutomation),
        reinterpret_cast<void**>(automation.GetAddressOf()));
    if ( FAILED(hr) || !automation )
        return finishFailure(17, FAILED(hr) ? hr : E_NOINTERFACE);

    // The caller has already established and passively verified focus on the
    // exact editor. UIAutomationCore otherwise applies its client-side
    // AutoSetFocus policy again before ValuePattern::SetValue(), crossing the
    // isolated HWND focus boundary even though CurrentHasKeyboardFocus is
    // already true. Disable that redundant policy while retaining the exact
    // focus check below as the helper's authority.
    ComPtr<IUIAutomation2> automation2;
    hr = automation.As(&automation2);
    if ( FAILED(hr) || !automation2 )
        return finishFailure(17, FAILED(hr) ? hr : E_NOINTERFACE);
    hr = automation2->put_AutoSetFocus(FALSE);
    if ( FAILED(hr) )
        return finishFailure(17, hr);

    ComPtr<IUIAutomationElement> bridgeElement;
    hr = automation->ElementFromHandle(bridge,
                                       bridgeElement.GetAddressOf());
    if ( FAILED(hr) || !bridgeElement )
    {
        return finishFailure(
            18, FAILED(hr) ? hr : UIA_E_ELEMENTNOTAVAILABLE);
    }

    const auto makeCondition =
        [&](const std::wstring& automationId,
            CONTROLTYPEID controlType,
            ComPtr<IUIAutomationCondition>& condition) -> HRESULT
    {
        VARIANT idValue;
        ::VariantInit(&idValue);
        idValue.vt = VT_BSTR;
        idValue.bstrVal = ::SysAllocStringLen(
            automationId.data(),
            static_cast<UINT>(automationId.size()));
        if ( !idValue.bstrVal && !automationId.empty() )
            return E_OUTOFMEMORY;

        ComPtr<IUIAutomationCondition> idCondition;
        HRESULT conditionHr = automation->CreatePropertyCondition(
            UIA_AutomationIdPropertyId, idValue,
            idCondition.GetAddressOf());
        ::VariantClear(&idValue);
        if ( FAILED(conditionHr) )
            return conditionHr;

        VARIANT typeValue;
        ::VariantInit(&typeValue);
        typeValue.vt = VT_I4;
        typeValue.lVal = controlType;
        ComPtr<IUIAutomationCondition> typeCondition;
        conditionHr = automation->CreatePropertyCondition(
            UIA_ControlTypePropertyId, typeValue,
            typeCondition.GetAddressOf());
        if ( FAILED(conditionHr) )
            return conditionHr;
        return automation->CreateAndCondition(
            idCondition.Get(), typeCondition.Get(),
            condition.ReleaseAndGetAddressOf());
    };

    ComPtr<IUIAutomationCondition> outerCondition;
    hr = makeCondition(outerAutomationId, UIA_ComboBoxControlTypeId,
                       outerCondition);
    if ( FAILED(hr) )
        return finishFailure(19, hr);

    ComPtr<IUIAutomationElement> outerElement;
    hr = bridgeElement->FindFirst(
        TreeScope_Descendants, outerCondition.Get(),
        outerElement.GetAddressOf());
    if ( FAILED(hr) || !outerElement )
    {
        return finishFailure(
            19, FAILED(hr) ? hr : UIA_E_ELEMENTNOTAVAILABLE);
    }

    BSTR foundOuterId = nullptr;
    CONTROLTYPEID foundOuterType = 0;
    hr = outerElement->get_CurrentAutomationId(&foundOuterId);
    if ( SUCCEEDED(hr) )
        hr = outerElement->get_CurrentControlType(&foundOuterType);
    const bool exactOuter = SUCCEEDED(hr) && foundOuterId &&
        outerAutomationId == foundOuterId &&
        foundOuterType == UIA_ComboBoxControlTypeId;
    ::SysFreeString(foundOuterId);
    if ( !exactOuter )
    {
        return finishFailure(
            19, FAILED(hr) ? hr : UIA_E_ELEMENTNOTAVAILABLE);
    }

    ComPtr<IUIAutomationCondition> editCondition;
    hr = makeCondition(editAutomationId, UIA_EditControlTypeId,
                       editCondition);
    if ( FAILED(hr) )
        return finishFailure(20, hr);

    ComPtr<IUIAutomationElement> editElement;
    hr = outerElement->FindFirst(
        TreeScope_Descendants, editCondition.Get(),
        editElement.GetAddressOf());
    if ( FAILED(hr) || !editElement )
    {
        return finishFailure(
            20, FAILED(hr) ? hr : UIA_E_ELEMENTNOTAVAILABLE);
    }

    BSTR foundEditId = nullptr;
    CONTROLTYPEID foundEditType = 0;
    hr = editElement->get_CurrentAutomationId(&foundEditId);
    if ( SUCCEEDED(hr) )
        hr = editElement->get_CurrentControlType(&foundEditType);
    const bool exactEdit = SUCCEEDED(hr) && foundEditId &&
        editAutomationId == foundEditId &&
        foundEditType == UIA_EditControlTypeId;
    ::SysFreeString(foundEditId);
    if ( !exactEdit )
    {
        return finishFailure(
            21, FAILED(hr) ? hr : UIA_E_ELEMENTNOTAVAILABLE);
    }

    BOOL hasKeyboardFocus = FALSE;
    hr = editElement->get_CurrentHasKeyboardFocus(&hasKeyboardFocus);
    if ( FAILED(hr) || !hasKeyboardFocus )
    {
        return finishFailure(
            21, FAILED(hr) ? hr : UIA_E_ELEMENTNOTAVAILABLE);
    }

    ComPtr<IUIAutomationValuePattern> valuePattern;
    hr = editElement->GetCurrentPatternAs(
        UIA_ValuePatternId, __uuidof(IUIAutomationValuePattern),
        reinterpret_cast<void**>(valuePattern.GetAddressOf()));
    if ( FAILED(hr) || !valuePattern )
        return finishFailure(21, FAILED(hr) ? hr : E_NOINTERFACE);

    BOOL isReadOnly = TRUE;
    hr = valuePattern->get_CurrentIsReadOnly(&isReadOnly);
    if ( FAILED(hr) || isReadOnly )
        return finishFailure(22, FAILED(hr) ? hr : UIA_E_NOTSUPPORTED);

    BSTR value = ::SysAllocStringLen(
        requestedValue.data(),
        static_cast<UINT>(requestedValue.size()));
    if ( !value && !requestedValue.empty() )
        return finishFailure(23, E_OUTOFMEMORY);
    hr = valuePattern->SetValue(value);
    ::SysFreeString(value);
    if ( FAILED(hr) )
        return finishFailure(23, hr);

    result.stage = 24;
    result.hresult = static_cast<std::int32_t>(S_OK);
    return WriteUiaResult(resultPipe, result);
}

bool ParseTimeout(const wchar_t* value, DWORD* timeout)
{
    if ( !value || !value[0] )
        return false;

    std::uint64_t parsed = 0;
    for ( const wchar_t* current = value; *current; ++current )
    {
        if ( *current < L'0' || *current > L'9' )
            return false;
        parsed = parsed * 10 + static_cast<unsigned>(*current - L'0');
        if ( parsed > MaximumTimeoutMs )
            return false;
    }

    if ( parsed == 0 || parsed >= INFINITE )
        return false;

    *timeout = static_cast<DWORD>(parsed);
    return true;
}

bool WaitForEmptyJob(HANDLE job, DWORD terminationCode)
{
    DWORD result = ::WaitForSingleObject(job, 0);
    if ( result == WAIT_OBJECT_0 )
        return true;
    if ( result != WAIT_TIMEOUT )
    {
        ReportWin32Error(L"WaitForSingleObject(job)");
        return false;
    }

    if ( !::TerminateJobObject(job, terminationCode) )
    {
        ReportWin32Error(L"TerminateJobObject");
        return false;
    }

    result = ::WaitForSingleObject(job, CleanupTimeoutMs);
    if ( result != WAIT_OBJECT_0 )
    {
        if ( result == WAIT_TIMEOUT )
        {
            std::fwprintf(
                stderr,
                L"wxWinUI isolated desktop runner: "
                L"job teardown timed out.\n");
        }
        else
        {
            ReportWin32Error(L"WaitForSingleObject(job teardown)");
        }
        return false;
    }

    return true;
}

} // namespace

int wmain(int argc, wchar_t** argv)
{
    // This mode is entered by the test process running below. It must happen
    // before creating another desktop/job: the helper is the external,
    // windowless UIA client and inherits the already isolated desktop.
    if ( argc > 1 &&
         std::wcscmp(argv[1], L"--uia-set-value") == 0 )
    {
        return RunUiaSetValueHelper(argc, argv);
    }

    DWORD timeout = 180000;
    int commandIndex = 1;

    if ( commandIndex < argc &&
            std::wcsncmp(argv[commandIndex], L"--timeout-ms=", 13) == 0 )
    {
        if ( !ParseTimeout(argv[commandIndex] + 13, &timeout) )
        {
            std::fwprintf(stderr, L"Invalid --timeout-ms value.\n");
            return ExitSetupFailure;
        }
        ++commandIndex;
    }

    if ( commandIndex < argc && std::wcscmp(argv[commandIndex], L"--") == 0 )
        ++commandIndex;

    if ( commandIndex >= argc )
    {
        std::fwprintf(
            stderr,
            L"Usage: wx_winui_desktop_test_runner "
            L"[--timeout-ms=N] -- executable [arguments...]\n");
        return ExitSetupFailure;
    }

    DWORD stationNameBytes = 0;
    ::GetUserObjectInformationW(
        ::GetProcessWindowStation(),
        UOI_NAME,
        nullptr,
        0,
        &stationNameBytes);
    if ( !stationNameBytes )
    {
        ReportWin32Error(L"GetUserObjectInformationW(size)");
        return ExitSetupFailure;
    }
    std::vector<wchar_t> stationName(
        stationNameBytes / sizeof(wchar_t) + 1, L'\0');
    if ( !::GetUserObjectInformationW(
            ::GetProcessWindowStation(),
            UOI_NAME,
            stationName.data(),
            stationNameBytes,
            &stationNameBytes) )
    {
        ReportWin32Error(L"GetUserObjectInformationW(name)");
        return ExitSetupFailure;
    }

    const HDESK originalDesktop =
        ::GetThreadDesktop(::GetCurrentThreadId());
    const std::wstring desktopName =
        L"wxWinUITest-" + std::to_wstring(::GetCurrentProcessId()) + L"-" +
        std::to_wstring(::GetTickCount64());
    const HDESK desktop = ::CreateDesktopW(
        desktopName.c_str(),
        nullptr,
        nullptr,
        0,
        GENERIC_ALL,
        nullptr);
    if ( !desktop )
    {
        ReportWin32Error(L"CreateDesktopW");
        return ExitSetupFailure;
    }

    const std::wstring qualifiedDesktop =
        std::wstring(stationName.data()) + L"\\" + desktopName;

    SECURITY_ATTRIBUTES inheritable{};
    inheritable.nLength = sizeof(inheritable);
    inheritable.bInheritHandle = TRUE;

    HANDLE readPipeRaw = nullptr;
    HANDLE writePipeRaw = nullptr;
    if ( !::CreatePipe(&readPipeRaw, &writePipeRaw, &inheritable, 0) )
    {
        ReportWin32Error(L"CreatePipe");
        ::SetThreadDesktop(originalDesktop);
        ::CloseDesktop(desktop);
        return ExitSetupFailure;
    }

    UniqueHandle readPipe(readPipeRaw);
    UniqueHandle writePipe(writePipeRaw);
    if ( !::SetHandleInformation(
            readPipe.Get(), HANDLE_FLAG_INHERIT, 0) )
    {
        ReportWin32Error(L"SetHandleInformation");
        ::SetThreadDesktop(originalDesktop);
        ::CloseDesktop(desktop);
        return ExitSetupFailure;
    }

    UniqueHandle nullInput(::CreateFileW(
        L"NUL",
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        &inheritable,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr));
    if ( !nullInput )
    {
        ReportWin32Error(L"CreateFileW(NUL)");
        ::SetThreadDesktop(originalDesktop);
        ::CloseDesktop(desktop);
        return ExitSetupFailure;
    }

    UniqueHandle job(::CreateJobObjectW(nullptr, nullptr));
    if ( !job )
    {
        ReportWin32Error(L"CreateJobObjectW");
        ::SetThreadDesktop(originalDesktop);
        ::CloseDesktop(desktop);
        return ExitSetupFailure;
    }

    JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobLimits{};
    jobLimits.BasicLimitInformation.LimitFlags =
        JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if ( !::SetInformationJobObject(
            job.Get(),
            JobObjectExtendedLimitInformation,
            &jobLimits,
            sizeof(jobLimits)) )
    {
        ReportWin32Error(L"SetInformationJobObject");
        ::SetThreadDesktop(originalDesktop);
        ::CloseDesktop(desktop);
        return ExitSetupFailure;
    }

    STARTUPINFOEXW startup{};
    startup.StartupInfo.cb = sizeof(startup);
    startup.StartupInfo.lpDesktop =
        const_cast<wchar_t*>(qualifiedDesktop.c_str());
    startup.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    startup.StartupInfo.hStdInput = nullInput.Get();
    startup.StartupInfo.hStdOutput = writePipe.Get();
    startup.StartupInfo.hStdError = writePipe.Get();

    SIZE_T attributeBytes = 0;
    (void)::InitializeProcThreadAttributeList(
        nullptr, 1, 0, &attributeBytes);
    if ( !attributeBytes )
    {
        ReportWin32Error(L"InitializeProcThreadAttributeList(size)");
        ::SetThreadDesktop(originalDesktop);
        ::CloseDesktop(desktop);
        return ExitSetupFailure;
    }
    std::vector<unsigned char> attributeStorage(attributeBytes);
    startup.lpAttributeList =
        reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(
            attributeStorage.data());
    if ( !::InitializeProcThreadAttributeList(
            startup.lpAttributeList, 1, 0, &attributeBytes) )
    {
        ReportWin32Error(L"InitializeProcThreadAttributeList");
        ::SetThreadDesktop(originalDesktop);
        ::CloseDesktop(desktop);
        return ExitSetupFailure;
    }

    HANDLE inheritedHandles[] =
    {
        nullInput.Get(),
        writePipe.Get()
    };
    if ( !::UpdateProcThreadAttribute(
            startup.lpAttributeList,
            0,
            PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
            inheritedHandles,
            sizeof(inheritedHandles),
            nullptr,
            nullptr) )
    {
        ReportWin32Error(L"UpdateProcThreadAttribute");
        ::DeleteProcThreadAttributeList(startup.lpAttributeList);
        ::SetThreadDesktop(originalDesktop);
        ::CloseDesktop(desktop);
        return ExitSetupFailure;
    }

    PROCESS_INFORMATION processInfo{};

    // Publish the exact runner executable to the isolated test process. The
    // ComboBox UIA oracle launches it back in the early helper mode above,
    // ensuring that UIAutomationCore sees a genuine out-of-process client.
    std::vector<wchar_t> runnerPath(32768, L'\0');
    const DWORD runnerPathLength = ::GetModuleFileNameW(
        nullptr, runnerPath.data(), static_cast<DWORD>(runnerPath.size()));
    if ( !runnerPathLength || runnerPathLength >= runnerPath.size() ||
         !::SetEnvironmentVariableW(
             L"WX_WINUI_UIA_HELPER", runnerPath.data()) )
    {
        ReportWin32Error(L"publish UIA helper path");
        ::DeleteProcThreadAttributeList(startup.lpAttributeList);
        ::SetThreadDesktop(originalDesktop);
        ::CloseDesktop(desktop);
        return ExitSetupFailure;
    }

    std::wstring commandLine =
        BuildCommandLine(argc, argv, commandIndex);
    std::vector<wchar_t> mutableCommand(
        commandLine.begin(), commandLine.end());
    mutableCommand.push_back(L'\0');

    const BOOL processCreated = ::CreateProcessW(
            argv[commandIndex],
            mutableCommand.data(),
            nullptr,
            nullptr,
            TRUE,
            CREATE_SUSPENDED | CREATE_NO_WINDOW |
                EXTENDED_STARTUPINFO_PRESENT,
            nullptr,
            nullptr,
            &startup.StartupInfo,
            &processInfo);
    ::DeleteProcThreadAttributeList(startup.lpAttributeList);
    if ( !processCreated )
    {
        ReportWin32Error(L"CreateProcessW");
        ::SetThreadDesktop(originalDesktop);
        ::CloseDesktop(desktop);
        return ExitSetupFailure;
    }

    UniqueHandle process(processInfo.hProcess);
    UniqueHandle processThread(processInfo.hThread);

    if ( !::AssignProcessToJobObject(job.Get(), process.Get()) )
    {
        ReportWin32Error(L"AssignProcessToJobObject");
        ::TerminateProcess(process.Get(), ExitSetupFailure);
        ::WaitForSingleObject(process.Get(), 5000);
        ::SetThreadDesktop(originalDesktop);
        ::CloseDesktop(desktop);
        return ExitSetupFailure;
    }

    writePipe.Reset();
    if ( ::ResumeThread(processThread.Get()) == static_cast<DWORD>(-1) )
    {
        ReportWin32Error(L"ResumeThread");
        ::TerminateJobObject(job.Get(), ExitSetupFailure);
        ::WaitForSingleObject(process.Get(), 5000);
        ::SetThreadDesktop(originalDesktop);
        ::CloseDesktop(desktop);
        return ExitSetupFailure;
    }

    UniqueHandle readerDone(::CreateEventW(
        nullptr, TRUE, FALSE, nullptr));
    if ( !readerDone )
    {
        ReportWin32Error(L"CreateEventW(reader)");
        ::TerminateJobObject(job.Get(), ExitSetupFailure);
        ::WaitForSingleObject(job.Get(), CleanupTimeoutMs);
        ::SetThreadDesktop(originalDesktop);
        ::CloseDesktop(desktop);
        return ExitSetupFailure;
    }

    std::thread outputThread;
    try
    {
        const HANDLE readerDoneHandle = readerDone.Get();
        outputThread = std::thread(
        [&readPipe, readerDoneHandle]
        {
            char buffer[4096];
            DWORD bytesRead = 0;
            while ( ::ReadFile(
                        readPipe.Get(),
                        buffer,
                        sizeof(buffer),
                        &bytesRead,
                        nullptr) &&
                    bytesRead )
            {
                std::fwrite(buffer, 1, bytesRead, stdout);
                std::fflush(stdout);
            }
            ::SetEvent(readerDoneHandle);
        });
    }
    catch ( const std::exception& )
    {
        std::fwprintf(
            stderr,
            L"wxWinUI isolated desktop runner: "
            L"could not create the output reader thread.\n");
        ::TerminateJobObject(job.Get(), ExitSetupFailure);
        ::WaitForSingleObject(job.Get(), CleanupTimeoutMs);
        ::SetThreadDesktop(originalDesktop);
        ::CloseDesktop(desktop);
        return ExitSetupFailure;
    }

    const DWORD waitResult = ::WaitForSingleObject(process.Get(), timeout);
    DWORD exitCode = ExitSetupFailure;
    if ( waitResult == WAIT_TIMEOUT )
    {
        std::fwprintf(
            stderr,
            L"wxWinUI isolated desktop runner: timed out after %lu ms.\n",
            static_cast<unsigned long>(timeout));
        exitCode = ExitTimeout;
    }
    else if ( waitResult != WAIT_OBJECT_0 )
    {
        ReportWin32Error(L"WaitForSingleObject");
    }
    else if ( !::GetExitCodeProcess(process.Get(), &exitCode) )
    {
        ReportWin32Error(L"GetExitCodeProcess");
        exitCode = ExitSetupFailure;
    }

    // Terminate and wait for descendants even when the direct child returned:
    // otherwise an inherited pipe in a leaked grandchild could block output
    // collection indefinitely.
    if ( !WaitForEmptyJob(
            job.Get(),
            exitCode == ExitTimeout ? ExitTimeout : ExitSetupFailure) )
    {
        exitCode = ExitSetupFailure;
    }

    job.Reset();
    processThread.Reset();
    process.Reset();
    nullInput.Reset();

    DWORD readerWait =
        ::WaitForSingleObject(readerDone.Get(), CleanupTimeoutMs);
    if ( readerWait != WAIT_OBJECT_0 )
    {
        (void)::CancelSynchronousIo(outputThread.native_handle());
        readPipe.Reset();
        readerWait =
            ::WaitForSingleObject(readerDone.Get(), CleanupTimeoutMs);
    }
    if ( readerWait != WAIT_OBJECT_0 )
    {
        std::fwprintf(
            stderr,
            L"wxWinUI isolated desktop runner: "
            L"output reader teardown timed out.\n");
        // This is our own helper process. An abrupt self-termination is the
        // final watchdog and cannot affect any user process.
        ::TerminateProcess(::GetCurrentProcess(), ExitSetupFailure);
        return ExitSetupFailure;
    }

    outputThread.join();
    readPipe.Reset();

    if ( !::SetThreadDesktop(originalDesktop) )
        ReportWin32Error(L"SetThreadDesktop");
    if ( !::CloseDesktop(desktop) )
        ReportWin32Error(L"CloseDesktop");

    return static_cast<int>(exitCode);
}

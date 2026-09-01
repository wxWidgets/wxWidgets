/////////////////////////////////////////////////////////////////////////////
// Non-installed adapters for the real dual-registration OLE broker.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_DROP_BROKER_TEST_ACCESS_H
#define WX_WINUI_DROP_BROKER_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/winui/private/dropbroker.h"

#if wxUSE_WINUI3 && wxUSE_OLE && wxUSE_DRAG_AND_DROP

struct wxWinUIDropBrokerNativeOps
{
    void* context = nullptr;
    HRESULT (*lock)(void*, IUnknown*, BOOL, BOOL) noexcept = nullptr;
    HRESULT (*registerTarget)(void*, HWND, IDropTarget*) noexcept = nullptr;
    HRESULT (*revokeTarget)(void*, HWND) noexcept = nullptr;
};

// The current flags below record the last production update in a
// test-enabled build. A passive copy is not a claim of current native
// identity; explicit validation is separate.
struct wxWinUIDropBrokerRegistrationSnapshot
{
    HRESULT lockHr = E_UNEXPECTED;
    HRESULT registerHr = E_UNEXPECTED;
    HRESULT revokeHr = E_UNEXPECTED;
    HRESULT unlockHr = E_UNEXPECTED;
    HWND registrationHwnd = nullptr;
    std::uint64_t generation = 0;
    unsigned lockCalls = 0;
    unsigned registerCalls = 0;
    unsigned revokeCalls = 0;
    unsigned unlockCalls = 0;
    bool locked = false;
    bool ownsRegistration = false;
    bool current = false;
};

struct wxWinUIDropBrokerSnapshot
{
    wxWinUIDropBrokerInitStatus status =
        wxWinUIDropBrokerInitStatus::NotInitialized;
    HRESULT lockHr = E_UNEXPECTED;
    HRESULT registerHr = E_UNEXPECTED;
    HRESULT revokeHr = E_UNEXPECTED;
    HRESULT unlockHr = E_UNEXPECTED;
    HRESULT resultHr = E_UNEXPECTED;
    HWND registrationHwnd = nullptr;
    unsigned lockCalls = 0;
    unsigned registerCalls = 0;
    unsigned revokeCalls = 0;
    unsigned unlockCalls = 0;
    unsigned dragEnterCalls = 0;
    unsigned dragOverCalls = 0;
    unsigned dragLeaveCalls = 0;
    unsigned dropCalls = 0;
    wxWinUIDropBrokerRegistrationSnapshot bridgeRegistration;
    wxWinUIDropBrokerRegistrationSnapshot tlwRegistration;
    bool active = false;
    bool locked = false;
    bool ownsRegistration = false;
    bool ownsCompleteRegistrationSet = false;
    bool shutdown = false;
};

class WXDLLIMPEXP_CORE wxWinUIDropBrokerTestAccess final
{
public:
    static std::unique_ptr<wxWinUIDropBroker> Create(
        wxWindow* tlw, HWND bridge,
        const std::weak_ptr<wxWinUIHostLifetime>& lifetime,
        const wxWinUIDropBrokerNativeOps* nativeOps) noexcept;

    // Pure stored-fact copy: no HWND/generation query, COM call or queued work.
    // Registration current flags describe the last production update only.
    static wxWinUIDropBrokerSnapshot ReadSnapshot(
        const wxWinUIDropBroker& broker) noexcept;

    // Explicit native operation, not passive observation: performs the same
    // HWND/generation validation as the historical qualification getter before
    // copying counters. Physical pre-injection gates must keep using this.
    static wxWinUIDropBrokerSnapshot ValidateNativeIdentity(
        const wxWinUIDropBroker& broker) noexcept;

    // Borrowed bridge adapter; callers retaining it must still AddRef/copy_from.
    static IDropTarget* GetCOMTarget(const wxWinUIDropBroker& broker) noexcept;
};

#endif
#endif

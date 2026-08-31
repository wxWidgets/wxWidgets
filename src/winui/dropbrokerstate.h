/////////////////////////////////////////////////////////////////////////////
// Intracore OLE broker operations and retained diagnostic records.
// Not installed. Counters and ownership facts remain present in shipping builds.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_DROPBROKER_STATE_H
#define WX_WINUI_DROPBROKER_STATE_H

#include "wx/winui/private/dropbroker.h"

#if wxUSE_WINUI3 && wxUSE_OLE && wxUSE_DRAG_AND_DROP

// Production uses the real OLE operations. Only the non-installed test factory
// can replace this table; its storage has the same layout in ON/OFF builds.
struct wxWinUIDropBrokerNativeOperations
{
    void* context = nullptr;
    HRESULT (*lock)(void*, IUnknown*, BOOL, BOOL) noexcept = nullptr;
    HRESULT (*registerTarget)(void*, HWND, IDropTarget*) noexcept = nullptr;
    HRESULT (*revokeTarget)(void*, HWND) noexcept = nullptr;
};


// The current flags below record the last production update. A passive copy
// is not a claim of current native identity; explicit validation is separate.
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


#endif
#endif

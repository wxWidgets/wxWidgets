/////////////////////////////////////////////////////////////////////////////
// Non-installed adapters for the real dual-registration OLE broker.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_DROP_BROKER_TEST_ACCESS_H
#define WX_WINUI_DROP_BROKER_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "../../../src/winui/dropbrokerstate.h"

#if wxUSE_WINUI3 && wxUSE_OLE && wxUSE_DRAG_AND_DROP

using wxWinUIDropBrokerNativeOps = wxWinUIDropBrokerNativeOperations;

class WXDLLIMPEXP_CORE wxWinUIDropBrokerTestAccess final
{
public:
    static std::unique_ptr<wxWinUIDropBroker> Create(
        wxWindow* tlw, HWND bridge,
        const std::weak_ptr<wxWinUIHostLifetime>& lifetime,
        const wxWinUIDropBrokerNativeOps* nativeOps) noexcept;

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

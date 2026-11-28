/////////////////////////////////////////////////////////////////////////////
// Private, non-installed deterministic physical-disconnect test access.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_PHYSICAL_DISCONNECT_TEST_ACCESS_H
#define WX_WINUI_PHYSICAL_DISCONNECT_TEST_ACCESS_H
#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/winui/private/tlwhost.h"

#if wxUSE_WINUI3
class WXDLLIMPEXP_CORE wxWinUIPhysicalDisconnectTestAccess final
{
public:
    // Synthetic gate only. Production gates/composites keep their own types.
    static std::shared_ptr<wxWinUIPhysicalDisconnectGate> CreateGate(
        const winrt::Microsoft::UI::Dispatching::DispatcherQueue& queue,
        bool satisfied = true,
        bool degraded = false);
    static bool SetGateState(
        const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate,
        bool satisfied,
        bool degraded = false);
    static bool IsGateSatisfied(
        const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate);
    static bool IsGateDegraded(
        const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate);
    static bool OpenGateAtSealedCompletion(
        const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate);
    static bool OpenGateBeforeSourceClose(
        const std::shared_ptr<wxWinUIPhysicalDisconnectGate>& gate);

    // IsPublicationPoisoned delegates to the real production query. Reset and
    // Poison are deterministic test mutations of that same real authority.
    static bool IsPublicationPoisoned() noexcept;
    static void ResetPublicationPoison() noexcept;
    static void PoisonPublication() noexcept;
};
#endif // wxUSE_WINUI3
#endif // WX_WINUI_PHYSICAL_DISCONNECT_TEST_ACCESS_H

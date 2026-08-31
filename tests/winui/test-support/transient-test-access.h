/////////////////////////////////////////////////////////////////////////////
// Non-installed observations and allocation faults of the real transient manager.
/////////////////////////////////////////////////////////////////////////////
#ifndef WX_WINUI_TRANSIENT_TEST_ACCESS_H
#define WX_WINUI_TRANSIENT_TEST_ACCESS_H
#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/winui/private/dialogsession.h"

#if wxUSE_WINUI3
class WXDLLIMPEXP_CORE wxWinUITransientTestAccess final
{
public:
    // Named sites retain the exact one-shot allocation failure contract.
    enum class AllocationSite
    {
        Any,
        PopupCancelToken,
        RegistrationImpl,
        RegistrationOwnerEntry,
        RegistrationObserver,
        RegistrationEntry,
        PopupRegistrationHolder,
        PopupRetirementHolder
    };

    // Key/epoch lookup never dereferences a retired owner window.
    struct OwnerIdentity
    {
        std::uintptr_t ownerKey = 0;
        std::uint64_t epoch = 0;

        explicit operator bool() const
        {
            return ownerKey != 0 && epoch != 0;
        }
    };

    // A passive copy of real manager counts; no HWND validation or queued work.
    struct RegistrySnapshot
    {
        std::size_t ownerCount = 0;
        std::size_t transientCount = 0;
        std::size_t cancelCallbackCount = 0;
        std::size_t modalCount = 0;
        std::size_t popupCount = 0;
        std::size_t teachingTipCount = 0;
        std::size_t pendingOwnerRetireCount = 0;
        std::size_t activeOwnerRetirementCount = 0;
        std::uint64_t ownerRetireEnqueueCount = 0;
        std::uint64_t ownerRetireCallbackCount = 0;
    };

    static void FailAllocationAt(AllocationSite site);
    static void ResetAllocationFailure();
    // Count requires a live wx owner, as before; no new lifetime contract.
    static std::size_t Count(wxWindow *ownerOrTLW);
    static std::size_t Count(wxWindow *ownerOrTLW, wxWinUITransientKind kind);
    static RegistrySnapshot GetRegistrySnapshot();
    static OwnerIdentity GetOwnerIdentity(std::uintptr_t ownerKey);
    static bool HasOwnerIdentity(const OwnerIdentity& identity);
};
#endif // wxUSE_WINUI3
#endif // WX_WINUI_TRANSIENT_TEST_ACCESS_H

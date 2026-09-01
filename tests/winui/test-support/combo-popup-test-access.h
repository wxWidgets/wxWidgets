/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to wxChoice exact-popup diagnostics.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_COMBO_POPUP_TEST_ACCESS_H
#define WX_WINUI_COMBO_POPUP_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
    #error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/defs.h"

#if wxUSE_CHOICE

class WXDLLIMPEXP_CORE wxWinUIComboPopupTestAccess final
{
public:
    enum class Origin : unsigned
    {
        None,
        DropDownOpenedNewSession,
        DropDownOpenedExistingSession,
        AfterNativeSetter
    };

    enum class Result : unsigned
    {
        None,
        Resolved,
        TemplatePending,
        PublicationPending,
        Closing,
        Failed
    };

    enum class FailCloseReason : unsigned
    {
        None,
        HostShutdownSealed,
        PublicationPoisoned,
        ReopenBeforeRetirement,
        BeginSessionRejected,
        ObserveOpenedException,
        StrictResolutionRejected,
        RequestCloseException
    };

    // Passive snapshot of values already observed by the exact-popup
    // retirement protocol. Reading or resetting this recorder never calls
    // into XAML and never changes the real popup state.
    struct Diagnostic
    {
        Origin origin = Origin::None;
        Result result = Result::None;
        FailCloseReason failCloseReason = FailCloseReason::None;
        unsigned failCloseCount = 0;
        bool propertyOpen = false;
        bool rootAvailable = false;
        bool queueAvailable = false;
        bool sessionActive = false;
        bool completionArmed = false;
        bool rootMatches = false;
        bool controlAvailable = false;
        bool popupAvailable = false;
        bool childAvailable = false;
        bool hooksArmed = false;
        bool popupIsOpen = false;
        bool exactOpenList = false;
        bool duplicateOpenList = false;
    };

    static void ResetDiagnostic() noexcept;
    static Diagnostic GetDiagnostic() noexcept;
};

#endif // wxUSE_CHOICE

#endif // WX_WINUI_COMBO_POPUP_TEST_ACCESS_H

/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to wxChoice test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_CHOICE_TEST_ACCESS_H
#define WX_WINUI_CHOICE_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
    #error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/choice.h"
#include <cstdint>
#include <memory>

#if wxUSE_CHOICE
class WXDLLIMPEXP_CORE wxWinUIChoiceTestAccess final
{
public:
    struct PopupRetirementCoreProbe
    {
        bool detachedCorrelationRetired = false;
        bool reopenRejected = false;
        bool reopenDegraded = false;
        bool compositeReentrantCompletion = false;
        bool sealRemainsClosed = false;
        bool sealedAddRejectedWithoutPoison = false;
        bool sealedNativeOpenFailsClosed = false;
    };

    // Implementation-only seams used by deterministic WinUI tests.
    static std::uint64_t GetItemId(const wxChoice* control, unsigned int n);
    static std::uintptr_t GetItemPeerIdentity(
        const wxChoice* control,
        unsigned int n);
    static bool GetItemPeerBitmapState(
        const wxChoice* control,
        unsigned int n,
        wxSize *pixelSize,
        wxSize *dipSize);
    static bool SelectPeerItem(wxChoice* control, int selection);
    static bool SetDropDown(wxChoice* control, bool open);
    static bool IsPeerDropDownOpen(const wxChoice* control);
    static bool GetPopupReopenSnapshot(
        const wxChoice* control,
        bool *pending,
        std::uint64_t *generation,
        unsigned *schedules,
        unsigned *runs);
    static PopupRetirementCoreProbe ProbePopupRetirementCore();
};
#endif

#endif

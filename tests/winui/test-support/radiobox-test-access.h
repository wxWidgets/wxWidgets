/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to RadioBox test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_RADIOBOX_TEST_ACCESS_H
#define WX_WINUI_RADIOBOX_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
    #error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/radiobox.h"

struct wxWinUIAppearanceSnapshot;

#if wxUSE_RADIOBOX
class WXDLLIMPEXP_CORE wxWinUIRadioBoxTestAccess final
{
public:
    using PeerWriteHook = void (*)(void *);

    static bool GetAppearance(const wxRadioBox& box,
                              wxWinUIAppearanceSnapshot* snapshot,
                              bool* titleIsRaw = nullptr);
    static bool GetPeerState(const wxRadioBox& box,
                             wxArrayString* strings,
                             int* selection,
                             unsigned long long* generation = nullptr);
    static bool SelectItem(wxRadioBox& box, unsigned int item);
    // One-shot seam immediately before the real SetContent() operation.
    static void SetNextPeerWriteHook(wxRadioBox& box,
                                     PeerWriteHook hook, void* context);
    static bool HasDeferredPeerWrite(const wxRadioBox& box);
    static bool IsPeerProjectionQuarantined(const wxRadioBox& box);
    static unsigned long long GetModelRevision(const wxRadioBox& box);
    static void GetCheckedHandlerCounts(const wxRadioBox& box,
                                        unsigned long long* added,
                                        unsigned long long* revoked);
};
#endif

#endif

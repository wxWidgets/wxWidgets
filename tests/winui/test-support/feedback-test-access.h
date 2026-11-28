/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to ActivityIndicator/Hyperlink test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_FEEDBACK_TEST_ACCESS_H
#define WX_WINUI_FEEDBACK_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
    #error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/activityindicator.h"
#include "wx/hyperlink.h"

struct wxWinUIAppearanceSnapshot;

#if wxUSE_ACTIVITYINDICATOR
class WXDLLIMPEXP_CORE wxWinUIActivityIndicatorTestAccess final
{
public:
    using PeerWriteHook = void (*)(void *);

    static bool GetState(const wxActivityIndicator& activity,
                         bool* peerActive, bool* isTabStop,
                         wxString* itemStatus,
                         wxWinUIAppearanceSnapshot* appearance);
    // One-shot seam after the real AutomationProperties::SetItemStatus().
    static void SetNextPeerWriteHook(wxActivityIndicator& activity,
                                     PeerWriteHook hook, void* context);
    static bool HasDeferredPeerWrite(const wxActivityIndicator& activity);
    static bool IsPeerProjectionQuarantined(const wxActivityIndicator& activity);
    static unsigned long long GetModelRevision(const wxActivityIndicator& activity);
};
#endif

#if wxUSE_HYPERLINKCTRL
class WXDLLIMPEXP_CORE wxWinUIHyperlinkTestAccess final
{
public:
    using PeerWriteHook = void (*)(void *);

    static bool Invoke(wxHyperlinkCtrl& link);
    static bool GetInteractiveRect(const wxHyperlinkCtrl& link, wxRect* rect);
    static bool HitTest(const wxHyperlinkCtrl& link, const wxPoint& point);
    static bool InvokeAt(wxHyperlinkCtrl& link, const wxPoint& point);
    static void SetPointerOver(wxHyperlinkCtrl& link, bool pointerOver);
    static bool GetState(const wxHyperlinkCtrl& link,
                         bool* pointerOver, int* horizontalAlignment,
                         bool* contextMenuEnabled, wxColour* effectiveColour,
                         wxWinUIAppearanceSnapshot* appearance,
                         bool* peerEnabled = nullptr);
    // One-shot seam after the real HyperlinkButton::Content() write.
    static void SetNextPeerWriteHook(wxHyperlinkCtrl& link,
                                     PeerWriteHook hook, void* context);
    static bool HasDeferredPeerWrite(const wxHyperlinkCtrl& link);
    static bool IsPeerProjectionQuarantined(const wxHyperlinkCtrl& link);
    static unsigned long long GetModelRevision(const wxHyperlinkCtrl& link);
    static unsigned GetLiveCallbackStateCount();
};
#endif

#endif

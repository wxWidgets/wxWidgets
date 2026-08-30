/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to wxListBox test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_LISTBOX_TEST_ACCESS_H
#define WX_WINUI_LISTBOX_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/listbox.h"

#if wxUSE_LISTBOX
class WXDLLIMPEXP_CORE wxWinUIListBoxTestAccess final
{
  public:
    struct ScrollMetrics
    {
        bool hasItemsPanelRoot = false;
        bool hasAuthoritativeScroll = false;
        bool hasScrollContentPresenter = false;
        bool contentPresenterCanHorizontallyScroll = false;
        bool contentPresenterSizesContentToTemplatedParent = true;
        bool hasItemsStackPanel = false;
        bool hasVirtualizingStackPanel = false;
        bool hasOrientedPanel = false;
        bool panelCanHorizontallyScroll = false;
        bool panelScrollOwnerResolvedFromAncestor = false;
        bool panelScrollOwnerMatches = false;
        bool horizontalPresentationPending = false;
        bool horizontalResetPending = false;
        bool materializationRetryQueued = false;
        unsigned int materializationAttempts = 0;
        unsigned int materializationRetriesRemaining = 0;
        int listVisualChildCount = 0;
        double scrollExtentWidth = 0.0;
        double scrollViewportWidth = 0.0;
        double scrollExtentHeight = 0.0;
        double scrollViewportHeight = 0.0;
        double scrollScrollableHeight = 0.0;
        double contentPresenterExtentWidth = 0.0;
        double contentPresenterViewportWidth = 0.0;
        double contentActualWidth = 0.0;
        double contentDesiredWidth = 0.0;
        double panelActualWidth = 0.0;
        double panelDesiredWidth = 0.0;
        double itemActualWidth = 0.0;
        double itemDesiredWidth = 0.0;
        double itemExplicitWidth = 0.0;
    };
    static std::uint64_t GetItemId(const wxListBox &control, unsigned int n);
    static std::uintptr_t GetItemPeerIdentity(const wxListBox &control,
                                              unsigned int n);
    static double GetControlWidthDIPs(const wxListBox &control);
    static unsigned int GetPeerCount(const wxListBox &control);
    static bool PoisonPeer(wxListBox &control);
    static bool SetPeerSelection(wxListBox &control, unsigned int n,
                                 bool select);
    static bool FocusPeerItem(wxListBox &control, unsigned int n);
    static bool DoubleTapPeer(wxListBox &control, unsigned int n);
    static bool SetPeerCheck(wxListBox &control, unsigned int n, bool check);
    static bool ActivatePeerCheck(wxListBox &control, unsigned int n,
                                  bool *focused = nullptr,
                                  bool *pointerTarget = nullptr);
    static bool TogglePeerViaAutomation(wxListBox &control, unsigned int n);
    static bool TogglePeerWithKeyboard(wxListBox &control, unsigned int n);
    static bool DispatchCheckKey(wxListBox &control, int virtualKey,
                                 bool *handled, bool shiftDown = false,
                                 std::uintptr_t keyboardLayout = 0);
    static bool GetPeerCheck(const wxListBox &control, unsigned int n);
    static bool
    GetScrollPresentation(const wxListBox &control, int *horizontalVisibility,
                          int *verticalVisibility,
                          double *horizontalScrollableWidth = nullptr,
                          ScrollMetrics *metrics = nullptr);
    static bool GetPeerLayoutDirection(const wxListBox &control,
                                       bool *rightToLeft);
    static bool
    GetItemPresentation(const wxListBox &control, unsigned int n,
                        bool *ownerDrawBitmap, wxSize *bitmapPixelSize,
                        unsigned int *tabRunCount, double *contentWidthDips,
                        bool *checkOverlay,
                        wxRealPoint *bitmapDIPSize = nullptr,
                        wxVector<double> *tabRunOffsetsDips = nullptr,
                        bool *checkOverlayFocused = nullptr,
                        unsigned int *contentFontWeight = nullptr,
                        double *contentFontSizeDips = nullptr,
                        wxString *contentFontFamily = nullptr,
                        double *containerHeightDips = nullptr,
                        double *containerMinHeightDips = nullptr,
                        double *containerActualHeightDips = nullptr,
                        double *xamlRasterizationScale = nullptr);
    static bool GetThemePresentation(const wxListBox &control, unsigned int n,
                                     int *actualTheme,
                                     std::uint32_t *foregroundARGB,
                                     std::uint32_t *bitmapARGB);
    static bool SetPeerTheme(wxListBox &control, bool dark);
};
#endif // wxUSE_LISTBOX

#endif

/////////////////////////////////////////////////////////////////////////////
// Private, non-installed access to wxTreeCtrl test operations.
// Available only in an explicitly test-enabled WinUI library build.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_TREECTRL_TEST_ACCESS_H
#define WX_WINUI_TREECTRL_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/treectrl.h"

#if wxUSE_TREECTRL
class WXDLLIMPEXP_CORE wxWinUITreeCtrlTestAccess final
{
  public:
    enum class PeerMutation
    {
        InsertItem,
        InsertRollbackAfterCommit,
        RemoveItem,
        RemoveItemAfterCommit,
        SetExpandedAfterCommit,
        ClearItems
    };

    enum class DragCompletion
    {
        Drop,
        Cancel
    };

    struct ModelStats
    {
        size_t itemCount = 0;
        size_t expandableItemCount = 0;
        size_t nodeLookupCount = 0;
        size_t peerUpdateCount = 0;
        size_t fullRefreshCount = 0;
        size_t modelGrowthCount = 0;
    };

    struct MeasuredItemParts
    {
        wxRect item;
        wxRect expander;
        wxRect stateImage;
        wxRect image;
        wxRect label;
    };
    static void FailNextPeerMutation(wxTreeCtrl &control,
                                     PeerMutation mutation);
    static void FailPeerMutations(wxTreeCtrl &control, PeerMutation mutation,
                                  unsigned count);
    static bool SelectPeerItem(wxTreeCtrl &control, const wxTreeItemId &item,
                               bool addToSelection = false);
    static bool FocusPeerItem(wxTreeCtrl &control, const wxTreeItemId &item);
    static bool SetPeerExpanded(wxTreeCtrl &control, const wxTreeItemId &item,
                                bool expanded);
    static bool IsPeerExpanded(const wxTreeCtrl &control,
                               const wxTreeItemId &item);
    static bool KeyDown(wxTreeCtrl &control, int keyCode,
                        bool controlDown = false, bool shiftDown = false,
                        bool altDown = false);
    static bool BeginInternalDrag(wxTreeCtrl &control,
                                  const wxTreeItemId &item);
    static bool CompleteInternalDrag(wxTreeCtrl &control,
                                     const wxTreeItemId &target,
                                     DragCompletion completion);
    static bool CompletePeerDrag(wxTreeCtrl &control,
                                 const wxTreeItemId &target,
                                 bool dropResultNone);
    static bool ScheduleLabelEdit(wxTreeCtrl &control,
                                  const wxTreeItemId &item);
    static bool FireLabelEditDelay(wxTreeCtrl &control);
    static bool ClickStateImage(wxTreeCtrl &control, const wxTreeItemId &item);
    static bool DoubleClickItem(wxTreeCtrl &control, const wxTreeItemId &item);
    static bool RightClickItem(wxTreeCtrl &control, const wxTreeItemId &item);
    static bool InvokeItem(wxTreeCtrl &control, const wxTreeItemId &item);
    static void PointerPressed(wxTreeCtrl &control, const wxPoint &point);
    static void PointerMoved(wxTreeCtrl &control, const wxPoint &point);
    static void PointerReleased(wxTreeCtrl &control, const wxPoint &point);
    static bool QueueSelectionCorrection(wxTreeCtrl &control);
    static bool IsPeerSelectionChange(const wxTreeCtrl &control);
    static bool IsPeerStructureRepairPending(const wxTreeCtrl &control);
    static std::uintptr_t GetItemPeerIdentity(const wxTreeCtrl &control,
                                              const wxTreeItemId &item);
    static bool IsItemAttachedToPeer(const wxTreeCtrl &control,
                                     const wxTreeItemId &item);
    static size_t
    GetPeerChildCount(const wxTreeCtrl &control,
                      const wxTreeItemId &parent = wxTreeItemId());
    static bool GetMeasuredItemParts(const wxTreeCtrl &control,
                                     const wxTreeItemId &item,
                                     MeasuredItemParts *parts);
    static bool GetPeerIndent(const wxTreeCtrl &control,
                              const wxTreeItemId &item, double *leading,
                              double *trailing);
    static bool IsPeerDropHighlighted(const wxTreeCtrl &control,
                                      const wxTreeItemId &item);
    static wxString GetPeerAutomationName(const wxTreeCtrl &control,
                                          const wxTreeItemId &item);
    static wxString GetPeerItemText(const wxTreeCtrl &control,
                                    const wxTreeItemId &item);
    static std::uintptr_t GetPeerItemImageIdentity(const wxTreeCtrl &control,
                                                   const wxTreeItemId &item);
    static bool RefreshForScale(wxTreeCtrl &control, double scale);
    static bool GetPeerItemImageProjection(const wxTreeCtrl &control,
                                           const wxTreeItemId &item,
                                           wxSize *imagePixelSize,
                                           wxSize *stateImagePixelSize,
                                           std::uint64_t *generation,
                                           wxSize *imageDIPSize = nullptr,
                                           wxSize *stateImageDIPSize = nullptr);
    static ModelStats GetModelStats(const wxTreeCtrl &control);
    static void ResetModelStats(wxTreeCtrl &control);
    static void ClosePeer(wxTreeCtrl &control);
    static size_t GetLiveCallbackStateCount();
};
#endif // wxUSE_TREECTRL

#endif

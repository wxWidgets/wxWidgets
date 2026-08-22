/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/treectrl.h
// Purpose:     wxWinUI wxTreeCtrl declaration
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_TREECTRL_H_
#define _WX_WINUI_TREECTRL_H_

#if wxUSE_TREECTRL

#include "wx/treebase.h"
#include "wx/textctrl.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

class wxWinUITreeItem;
class wxWinUITreeCtrlImpl;

class WXDLLIMPEXP_CORE wxTreeCtrl : public wxTreeCtrlBase
{
public:
    enum class WinUIPeerMutationForTesting
    {
        InsertItem,
        InsertRollbackAfterCommit,
        RemoveItem,
        RemoveItemAfterCommit,
        SetExpandedAfterCommit,
        ClearItems
    };

    enum class WinUIDragCompletionForTesting
    {
        Drop,
        Cancel
    };

    struct WinUIModelStats
    {
        size_t itemCount = 0;
        size_t expandableItemCount = 0;
        size_t nodeLookupCount = 0;
        size_t peerUpdateCount = 0;
        size_t fullRefreshCount = 0;
        size_t modelGrowthCount = 0;
    };

    struct WinUIMeasuredItemParts
    {
        wxRect item;
        wxRect expander;
        wxRect stateImage;
        wxRect image;
        wxRect label;
    };

    wxTreeCtrl();
    wxTreeCtrl(wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxTreeCtrlNameStr));
    ~wxTreeCtrl() override;

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxTreeCtrlNameStr));

    void SetWindowStyleFlag(long style) override;
    bool Reparent(wxWindowBase *newParent) override;

    unsigned int GetCount() const override;

    unsigned int GetIndent() const override;
    void SetIndent(unsigned int indent) override;

    void SetStateImages(const wxVector<wxBitmapBundle>& images) override;
    void SetImageList(wxImageList *imageList) override;
    void SetStateImageList(wxImageList *imageList) override;

    wxString GetItemText(const wxTreeItemId& item) const override;
    int GetItemImage(const wxTreeItemId& item,
                     wxTreeItemIcon which = wxTreeItemIcon_Normal) const override;
    wxTreeItemData *GetItemData(const wxTreeItemId& item) const override;
    wxColour GetItemTextColour(const wxTreeItemId& item) const override;
    wxColour GetItemBackgroundColour(const wxTreeItemId& item) const override;
    wxFont GetItemFont(const wxTreeItemId& item) const override;

    void SetItemText(const wxTreeItemId& item, const wxString& text) override;
    void SetItemImage(const wxTreeItemId& item,
                      int image,
                      wxTreeItemIcon which = wxTreeItemIcon_Normal) override;
    void SetItemData(const wxTreeItemId& item, wxTreeItemData *data) override;
    void SetItemHasChildren(const wxTreeItemId& item, bool has = true) override;
    void SetItemBold(const wxTreeItemId& item, bool bold = true) override;
    void SetItemDropHighlight(const wxTreeItemId& item, bool highlight = true) override;
    void SetItemTextColour(const wxTreeItemId& item, const wxColour& col) override;
    void SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& col) override;
    void SetItemFont(const wxTreeItemId& item, const wxFont& font) override;

    bool IsVisible(const wxTreeItemId& item) const override;
    bool ItemHasChildren(const wxTreeItemId& item) const override;
    bool IsExpanded(const wxTreeItemId& item) const override;
    bool IsSelected(const wxTreeItemId& item) const override;
    bool IsBold(const wxTreeItemId& item) const override;
    size_t GetChildrenCount(const wxTreeItemId& item,
                            bool recursively = true) const override;

    wxTreeItemId GetRootItem() const override;
    wxTreeItemId GetSelection() const override;
    size_t GetSelections(wxArrayTreeItemIds& selections) const override;
    wxTreeItemId GetFocusedItem() const override;
    void ClearFocusedItem() override;
    void SetFocusedItem(const wxTreeItemId& item) override;

    wxTreeItemId GetItemParent(const wxTreeItemId& item) const override;
    wxTreeItemId GetFirstChild(const wxTreeItemId& item,
                               wxTreeItemIdValue& cookie) const override;
    wxTreeItemId GetNextChild(const wxTreeItemId& item,
                              wxTreeItemIdValue& cookie) const override;
    wxTreeItemId GetLastChild(const wxTreeItemId& item) const override;

    wxTreeItemId GetNextSibling(const wxTreeItemId& item) const override;
    wxTreeItemId GetPrevSibling(const wxTreeItemId& item) const override;

    wxTreeItemId GetFirstVisibleItem() const override;
    wxTreeItemId GetNextVisible(const wxTreeItemId& item) const override;
    wxTreeItemId GetPrevVisible(const wxTreeItemId& item) const override;

    wxTreeItemId AddRoot(const wxString& text,
                         int image = -1,
                         int selectedImage = -1,
                         wxTreeItemData *data = nullptr) override;

    void Delete(const wxTreeItemId& item) override;
    void DeleteChildren(const wxTreeItemId& item) override;
    void DeleteAllItems() override;

    void Expand(const wxTreeItemId& item) override;
    void Collapse(const wxTreeItemId& item) override;
    void CollapseAndReset(const wxTreeItemId& item) override;
    void Toggle(const wxTreeItemId& item) override;

    void Unselect() override;
    void UnselectAll() override;
    void SelectItem(const wxTreeItemId& item, bool select = true) override;
    void SelectChildren(const wxTreeItemId& parent) override;

    void EnsureVisible(const wxTreeItemId& item) override;
    void ScrollTo(const wxTreeItemId& item) override;

    wxTextCtrl *EditLabel(const wxTreeItemId& item,
                          wxClassInfo* textCtrlClass = wxCLASSINFO(wxTextCtrl)) override;
    wxTextCtrl *GetEditControl() const override;
    void EndEditLabel(const wxTreeItemId& item, bool discardChanges = false) override;

    void SortChildren(const wxTreeItemId& item) override;

    bool GetBoundingRect(const wxTreeItemId& item,
                         wxRect& rect,
                         bool textOnly = false) const override;

    wxVisualAttributes GetDefaultAttributes() const override
    {
        return GetClassDefaultAttributes(GetWindowVariant());
    }

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    bool CanApplyThemeBorder() const override { return false; }

    // Implementation-only deterministic seams. They exercise the real WinUI
    // callback, transaction and internal-drag paths without SendInput.
    void WinUIFailNextPeerMutationForTesting(
        WinUIPeerMutationForTesting mutation);
    void WinUIFailPeerMutationsForTesting(
        WinUIPeerMutationForTesting mutation,
        unsigned count);
    bool WinUISelectPeerItemForTesting(
        const wxTreeItemId& item,
        bool addToSelection = false);
    bool WinUIFocusPeerItemForTesting(const wxTreeItemId& item);
    bool WinUISetPeerExpandedForTesting(const wxTreeItemId& item,
                                        bool expanded);
    bool WinUIIsPeerExpandedForTesting(
        const wxTreeItemId& item) const;
    bool WinUIKeyDownForTesting(int keyCode,
                                bool controlDown = false,
                                bool shiftDown = false,
                                bool altDown = false);
    bool WinUIBeginInternalDragForTesting(const wxTreeItemId& item);
    bool WinUICompleteInternalDragForTesting(
        const wxTreeItemId& target,
        WinUIDragCompletionForTesting completion);
    bool WinUICompletePeerDragForTesting(
        const wxTreeItemId& target,
        bool dropResultNone);
    bool WinUIScheduleLabelEditForTesting(
        const wxTreeItemId& item);
    bool WinUIFireLabelEditDelayForTesting();
    bool WinUIClickStateImageForTesting(
        const wxTreeItemId& item);
    bool WinUIDoubleClickItemForTesting(
        const wxTreeItemId& item);
    bool WinUIRightClickItemForTesting(
        const wxTreeItemId& item);
    bool WinUIInvokeItemForTesting(
        const wxTreeItemId& item);
    void WinUIPointerPressedForTesting(const wxPoint& point);
    void WinUIPointerMovedForTesting(const wxPoint& point);
    void WinUIPointerReleasedForTesting(const wxPoint& point);
    bool WinUIQueueSelectionCorrectionForTesting();
    bool WinUIIsPeerSelectionChangeForTesting() const;
    bool WinUIIsPeerStructureRepairPendingForTesting() const;
    std::uintptr_t
    WinUIGetItemPeerIdentityForTesting(const wxTreeItemId& item) const;
    bool WinUIIsItemAttachedToPeerForTesting(
        const wxTreeItemId& item) const;
    size_t WinUIGetPeerChildCountForTesting(
        const wxTreeItemId& parent = wxTreeItemId()) const;
    bool WinUIGetMeasuredItemPartsForTesting(
        const wxTreeItemId& item,
        WinUIMeasuredItemParts *parts) const;
    bool WinUIGetPeerIndentForTesting(
        const wxTreeItemId& item,
        double *leading,
        double *trailing) const;
    bool WinUIIsPeerDropHighlightedForTesting(
        const wxTreeItemId& item) const;
    wxString WinUIGetPeerAutomationNameForTesting(
        const wxTreeItemId& item) const;
    wxString WinUIGetPeerItemTextForTesting(
        const wxTreeItemId& item) const;
    std::uintptr_t WinUIGetPeerItemImageIdentityForTesting(
        const wxTreeItemId& item) const;
    bool WinUIRefreshForScaleForTesting(double scale);
    bool WinUIGetPeerItemImageProjectionForTesting(
        const wxTreeItemId& item,
        wxSize *imagePixelSize,
        wxSize *stateImagePixelSize,
        std::uint64_t *generation,
        wxSize *imageDIPSize = nullptr,
        wxSize *stateImageDIPSize = nullptr) const;
    WinUIModelStats WinUIGetModelStatsForTesting() const;
    void WinUIResetModelStatsForTesting();
    void WinUIClosePeerForTesting();
    static size_t WinUIGetLiveCallbackStateCountForTesting();

protected:
    bool MSWOnEffectiveLayoutDirectionChanged() override;
    void DoThaw() override;
    int DoGetItemState(const wxTreeItemId& item) const override;
    void DoSetItemState(const wxTreeItemId& item, int state) override;

    wxTreeItemId DoInsertItem(const wxTreeItemId& parent,
                              size_t pos,
                              const wxString& text,
                              int image,
                              int selImage,
                              wxTreeItemData *data) override;
    wxTreeItemId DoInsertAfter(const wxTreeItemId& parent,
                               const wxTreeItemId& idPrevious,
                               const wxString& text,
                               int image = -1,
                               int selImage = -1,
                               wxTreeItemData *data = nullptr) override;
    wxTreeItemId DoTreeHitTest(const wxPoint& point, int& flags) const override;

    wxSize DoGetBestSize() const override;
    void OnImagesChanged() override;

private:
    enum class SelectionPreflightResult
    {
        Allowed,
        Vetoed,
        Superseded
    };

    enum class PeerProjectionResult
    {
        Done,
        Stale,
        Failed
    };

    enum class PeerExpansionMode
    {
        Immediate,
        PeerAlreadyUpdated,
        Deferred
    };

    enum class AncestorExpansionResult
    {
        Done,
        Stale,
        PeerDeferred
    };

    wxWinUITreeItem *GetItem(const wxTreeItemId& item) const;
    wxTreeItemId MakeId(wxWinUITreeItem *item) const;
    wxWinUITreeItem *ResolveItem(std::uint64_t itemId) const;

    void SendTreeEvent(wxEventType type,
                       wxWinUITreeItem *item,
                       wxWinUITreeItem *oldItem = nullptr);
    bool EnsureVisibleItem(std::uint64_t itemId,
                           std::uint64_t operation,
                           bool scroll);
    bool SetExpanded(wxWinUITreeItem *item,
                     bool expanded,
                     bool sendEvent,
                     PeerExpansionMode peerMode,
                     std::uint64_t operation = 0);
    AncestorExpansionResult ExpandAncestorPath(
        std::uint64_t itemId,
        std::uint64_t operation,
        std::vector<std::uint64_t> *projectionIds);
    SelectionPreflightResult PreflightSelectionChange(
        wxWinUITreeItem *item,
        std::uint64_t *oldItemId);
    void CommitSelectionForDeletion(wxWinUITreeItem *replacement,
                                    wxWinUITreeItem *doomedSubtree);
    bool ChangeSelection(wxWinUITreeItem *item,
                         bool sendEvent,
                         bool updatePeer,
                         std::uint64_t *operationOut = nullptr);
    bool ApplySelectionToPeer();
    // Rebuild only the peer hierarchy from the authoritative wx model. This
    // is the bounded recovery path when a best-effort WinRT rollback itself
    // fails part-way through.
    bool ReconcilePeerStructureFromModel();
    // Push the current selection back to the WinUI TreeView, but deferred to
    // the dispatcher queue.  This is required when a selection change is
    // rejected (e.g. a wxTreebook category page that has no associated page):
    // the TreeView ignores a SelectedNode write made synchronously from inside
    // its own SelectionChanged callback, so the correction has to run once the
    // control's selection transaction has completed.
    void SchedulePeerSelectionCorrection(bool newRequest = true);
    PeerProjectionResult UpdatePeerItem(wxWinUITreeItem *item);
    PeerProjectionResult UpdatePeerItems(
        const std::vector<std::uint64_t>& itemIds);
    PeerProjectionResult RefreshProjectedItems();
    PeerProjectionResult RequestPeerProjection(
        const std::vector<std::uint64_t>& itemIds,
        bool allItems);
    PeerProjectionResult SyncPeerProjection();
    PeerProjectionResult ProjectPeerItemPass(
        std::uint64_t itemId,
        std::uint64_t revision,
        wxWinUITreeCtrlImpl *impl);
    // Retrieve the on-screen rectangle of the item's realized container, in
    // client coordinates.  Fails if the container is not realized (yet).
    bool GetItemPeerRect(wxWinUITreeItem *item,
                         wxRect& rect,
                         bool textOnly = false) const;
    // True if any item in the tree has (or claims to have) children, i.e. if
    // the expander column is ever going to be used.
    bool HasExpandableItem() const;
    void OnPeerRightTapped(const wxPoint& pt);
    void OnPeerPointerPressed(const wxPoint& pt);
    void OnPeerPointerMoved(const wxPoint& pt);
    void OnPeerPointerReleased(const wxPoint& pt);
    void OnPeerDoubleTapped(const wxPoint& pt);
    bool SendRightClickEvents(wxWinUITreeItem *item,
                              const wxPoint& point);
    void OnPeerItemInvoked(wxWinUITreeItem *item);
    void OnPeerSelectionChanged();
    void OnPeerNodeExpanded(wxWinUITreeItem *item);
    void OnPeerNodeCollapsed(wxWinUITreeItem *item);
    bool OnPeerKeyDown(int keyCode,
                       wchar_t unicode = 0,
                       bool controlDown = false,
                       bool shiftDown = false,
                       bool altDown = false,
                       bool modifiersProvided = false);
    bool BeginInternalDrag(wxWinUITreeItem *item, const wxPoint& point);
    bool HandlePeerDragStarting(wxWinUITreeItem *item,
                                const wxPoint& point);
    void HandlePeerDragCompleted(wxWinUITreeItem *target,
                                 const wxPoint& point,
                                 bool dropResultNone);
    void CompleteInternalDrag(wxWinUITreeItem *target,
                              const wxPoint& point,
                              bool cancelled);
    bool ScheduleDelayedLabelEdit(wxWinUITreeItem *item);
    bool CompleteDelayedLabelEdit(std::uint64_t ticket);
    void CancelDelayedLabelEdit();
    void SendStateImageClick(wxWinUITreeItem *item,
                             const wxPoint& point);
    void OnDPIChanged(wxDPIChangedEvent& event);

    std::unique_ptr<wxWinUITreeCtrlImpl> m_winui;
    // wxTreeCtrl supports two-step construction. Keep bundle state in the
    // control independently of the peer implementation so SetStateImages()
    // made before Create() can seed the first projection. m_imagesState remains
    // the wxTreeCtrlBase-authoritative public/image-list storage.
    wxVector<wxBitmapBundle> m_winuiStateImageBundles;
    unsigned int m_indent = 16;

    // In-place label editing state.
    wxTextCtrl *m_editControl = nullptr;

    wxDECLARE_DYNAMIC_CLASS(wxTreeCtrl);
    wxDECLARE_NO_COPY_CLASS(wxTreeCtrl);
};

#endif // wxUSE_TREECTRL

#endif // _WX_WINUI_TREECTRL_H_

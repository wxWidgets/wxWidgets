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

#include <memory>

class wxWinUITreeItem;
class wxWinUITreeCtrlImpl;

class WXDLLIMPEXP_CORE wxTreeCtrl : public wxTreeCtrlBase
{
public:
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

protected:
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
    wxWinUITreeItem *GetItem(const wxTreeItemId& item) const;
    wxTreeItemId MakeId(wxWinUITreeItem *item) const;

    bool IsSelectionChangeAllowed(wxWinUITreeItem *item, wxWinUITreeItem *oldItem);
    void SendTreeEvent(wxEventType type,
                       wxWinUITreeItem *item,
                       wxWinUITreeItem *oldItem = nullptr);
    bool ChangeSelection(wxWinUITreeItem *item, bool sendEvent, bool updatePeer);
    void ApplySelectionToPeer();
    // Push the current selection back to the WinUI TreeView, but deferred to
    // the dispatcher queue.  This is required when a selection change is
    // rejected (e.g. a wxTreebook category page that has no associated page):
    // the TreeView ignores a SelectedNode write made synchronously from inside
    // its own SelectionChanged callback, so the correction has to run once the
    // control's selection transaction has completed.
    void SchedulePeerSelectionCorrection();
    void UpdatePeerItem(wxWinUITreeItem *item);
    void RefreshPeerItems();
    // Retrieve the on-screen rectangle of the item's realized container, in
    // client coordinates.  Fails if the container is not realized (yet).
    bool GetItemPeerRect(wxWinUITreeItem *item, wxRect& rect) const;
    void OnPeerRightTapped(const wxPoint& pt);
    void OnPeerSelectionChanged();
    void OnPeerNodeExpanded(wxWinUITreeItem *item);
    void OnPeerNodeCollapsed(wxWinUITreeItem *item);

    std::unique_ptr<wxWinUITreeCtrlImpl> m_winui;
    unsigned int m_indent = 16;
    bool m_updatingPeer = false;

    // Set while we are handling a SelectionChanged notification coming from the
    // WinUI TreeView: in this state a peer-selection write must be deferred (see
    // SchedulePeerSelectionCorrection).
    bool m_inPeerSelectionChange = false;
    // True while a deferred selection correction is queued: incoming peer
    // selection changes are ignored until it runs, to avoid an event storm.
    bool m_peerCorrectionPending = false;
    // The item whose selection the application refused; if the control keeps
    // trying to select it we just push the real selection back without
    // re-dispatching the (rejected) selection event.
    wxWinUITreeItem *m_peerRejectedItem = nullptr;

    // In-place label editing state.
    wxTextCtrl *m_editControl = nullptr;
    wxWinUITreeItem *m_editItem = nullptr;

    // The item being dragged, if the application allowed the drag.
    wxWinUITreeItem *m_dragItem = nullptr;

    wxDECLARE_DYNAMIC_CLASS(wxTreeCtrl);
    wxDECLARE_NO_COPY_CLASS(wxTreeCtrl);
};

#endif // wxUSE_TREECTRL

#endif // _WX_WINUI_TREECTRL_H_

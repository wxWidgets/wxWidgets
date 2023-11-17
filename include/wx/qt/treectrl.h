/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/treectrl.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_TREECTRL_H_
#define _WX_QT_TREECTRL_H_

class wxQTreeWidget;

class WXDLLIMPEXP_CORE wxTreeCtrl : public wxTreeCtrlBase
{
public:
    wxTreeCtrl();
    wxTreeCtrl(wxWindow *parent, wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxTreeCtrlNameStr));

    virtual ~wxTreeCtrl();

    bool Create(wxWindow *parent, wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxTreeCtrlNameStr));

    virtual unsigned int GetCount() const override;

    virtual unsigned int GetIndent() const override;
    virtual void SetIndent(unsigned int indent) override;

    virtual void SetStateImages(const wxVector<wxBitmapBundle>& images) override;

    virtual void SetImageList(wxImageList *imageList) override;
    virtual void SetStateImageList(wxImageList *imageList) override;

    virtual wxString GetItemText(const wxTreeItemId& item) const override;
    virtual int GetItemImage(const wxTreeItemId& item,
                     wxTreeItemIcon which = wxTreeItemIcon_Normal) const override;
    virtual wxTreeItemData *GetItemData(const wxTreeItemId& item) const override;
    virtual wxColour GetItemTextColour(const wxTreeItemId& item) const override;
    virtual wxColour GetItemBackgroundColour(const wxTreeItemId& item) const override;
    virtual wxFont GetItemFont(const wxTreeItemId& item) const override;

    virtual void SetItemText(const wxTreeItemId& item, const wxString& text) override;
    virtual void SetItemImage(const wxTreeItemId& item,
                              int image,
                              wxTreeItemIcon which = wxTreeItemIcon_Normal) override;
    virtual void SetItemData(const wxTreeItemId& item, wxTreeItemData *data) override;
    virtual void SetItemHasChildren(const wxTreeItemId& item, bool has = true) override;
    virtual void SetItemBold(const wxTreeItemId& item, bool bold = true) override;
    virtual void SetItemDropHighlight(const wxTreeItemId& item, bool highlight = true) override;
    virtual void SetItemTextColour(const wxTreeItemId& item, const wxColour& col) override;
    virtual void SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& col) override;
    virtual void SetItemFont(const wxTreeItemId& item, const wxFont& font) override;

    virtual bool IsVisible(const wxTreeItemId& item) const override;
    virtual bool ItemHasChildren(const wxTreeItemId& item) const override;
    virtual bool IsExpanded(const wxTreeItemId& item) const override;
    virtual bool IsSelected(const wxTreeItemId& item) const override;
    virtual bool IsBold(const wxTreeItemId& item) const override;

    virtual size_t GetChildrenCount(const wxTreeItemId& item, bool recursively = true) const override;

    virtual wxTreeItemId GetRootItem() const override;
    virtual wxTreeItemId GetSelection() const override;
    virtual size_t GetSelections(wxArrayTreeItemIds& selections) const override;

    virtual void SetFocusedItem(const wxTreeItemId& item) override;
    virtual void ClearFocusedItem() override;
    virtual wxTreeItemId GetFocusedItem() const override;

    virtual wxTreeItemId GetItemParent(const wxTreeItemId& item) const override;
    virtual wxTreeItemId GetFirstChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const override;
    virtual wxTreeItemId GetNextChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const override;
    virtual wxTreeItemId GetLastChild(const wxTreeItemId& item) const override;
    virtual wxTreeItemId GetNextSibling(const wxTreeItemId& item) const override;
    virtual wxTreeItemId GetPrevSibling(const wxTreeItemId& item) const override;
    virtual wxTreeItemId GetFirstVisibleItem() const override;
    virtual wxTreeItemId GetNextVisible(const wxTreeItemId& item) const override;
    virtual wxTreeItemId GetPrevVisible(const wxTreeItemId& item) const override;

    virtual wxTreeItemId AddRoot(const wxString& text,
                                 int image = -1, int selImage = -1,
                                 wxTreeItemData *data = nullptr) override;

    virtual void Delete(const wxTreeItemId& item) override;
    virtual void DeleteChildren(const wxTreeItemId& item) override;
    virtual void DeleteAllItems() override;

    virtual void Expand(const wxTreeItemId& item) override;
    virtual void Collapse(const wxTreeItemId& item) override;
    virtual void CollapseAndReset(const wxTreeItemId& item) override;
    virtual void Toggle(const wxTreeItemId& item) override;

    virtual void Unselect() override;
    virtual void UnselectAll() override;
    virtual void SelectItem(const wxTreeItemId& item, bool select = true) override;
    virtual void SelectChildren(const wxTreeItemId& parent) override;

    virtual void EnsureVisible(const wxTreeItemId& item) override;
    virtual void ScrollTo(const wxTreeItemId& item) override;

    virtual wxTextCtrl *EditLabel(const wxTreeItemId& item, wxClassInfo* textCtrlClass = CLASSINFO(wxTextCtrl)) override;
    virtual wxTextCtrl *GetEditControl() const override;
    virtual void EndEditLabel(const wxTreeItemId& item, bool discardChanges = false) override;

    virtual void SortChildren(const wxTreeItemId& item) override;

    virtual bool GetBoundingRect(const wxTreeItemId& item, wxRect& rect, bool textOnly = false) const override;

    virtual void SetWindowStyleFlag(long styles) override;

    virtual QWidget *GetHandle() const override;

protected:
    virtual int DoGetItemState(const wxTreeItemId& item) const override;
    virtual void DoSetItemState(const wxTreeItemId& item, int state) override;

    virtual wxTreeItemId DoInsertItem(const wxTreeItemId& parent,
                                      size_t pos,
                                      const wxString& text,
                                      int image, int selImage,
                                      wxTreeItemData *data) override;

    virtual wxTreeItemId DoInsertAfter(const wxTreeItemId& parent,
                                       const wxTreeItemId& idPrevious,
                                       const wxString& text,
                                       int image = -1, int selImage = -1,
                                       wxTreeItemData *data = nullptr) override;

    virtual wxTreeItemId DoTreeHitTest(const wxPoint& point, int& flags) const override;

    virtual void OnImagesChanged() override;

    // For wxEVT_TREE_KEY_DOWN generation
    void OnKeyDown(wxKeyEvent &event);

private:
    void SendDeleteEvent(const wxTreeItemId &item);
    wxTreeItemId GetNext(const wxTreeItemId &item) const;

    void DoUpdateIconsSize(wxImageList *imageList);

    wxQTreeWidget *m_qtTreeWidget;
    wxDECLARE_DYNAMIC_CLASS(wxTreeCtrl);
};

#endif // _WX_QT_TREECTRL_H_

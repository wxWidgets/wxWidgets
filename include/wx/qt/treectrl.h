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

    virtual unsigned int GetCount() const wxOVERRIDE;

    virtual unsigned int GetIndent() const wxOVERRIDE;
    virtual void SetIndent(unsigned int indent) wxOVERRIDE;

    virtual void SetImageList(wxImageList *imageList) wxOVERRIDE;
    virtual void SetStateImageList(wxImageList *imageList) wxOVERRIDE;

    virtual wxString GetItemText(const wxTreeItemId& item) const wxOVERRIDE;
    virtual int GetItemImage(const wxTreeItemId& item,
                     wxTreeItemIcon which = wxTreeItemIcon_Normal) const wxOVERRIDE;
    virtual wxTreeItemData *GetItemData(const wxTreeItemId& item) const wxOVERRIDE;
    virtual wxColour GetItemTextColour(const wxTreeItemId& item) const wxOVERRIDE;
    virtual wxColour GetItemBackgroundColour(const wxTreeItemId& item) const wxOVERRIDE;
    virtual wxFont GetItemFont(const wxTreeItemId& item) const wxOVERRIDE;

    virtual void SetItemText(const wxTreeItemId& item, const wxString& text) wxOVERRIDE;
    virtual void SetItemImage(const wxTreeItemId& item,
                              int image,
                              wxTreeItemIcon which = wxTreeItemIcon_Normal) wxOVERRIDE;
    virtual void SetItemData(const wxTreeItemId& item, wxTreeItemData *data) wxOVERRIDE;
    virtual void SetItemHasChildren(const wxTreeItemId& item, bool has = true) wxOVERRIDE;
    virtual void SetItemBold(const wxTreeItemId& item, bool bold = true) wxOVERRIDE;
    virtual void SetItemDropHighlight(const wxTreeItemId& item, bool highlight = true) wxOVERRIDE;
    virtual void SetItemTextColour(const wxTreeItemId& item, const wxColour& col) wxOVERRIDE;
    virtual void SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& col) wxOVERRIDE;
    virtual void SetItemFont(const wxTreeItemId& item, const wxFont& font) wxOVERRIDE;

    virtual bool IsVisible(const wxTreeItemId& item) const wxOVERRIDE;
    virtual bool ItemHasChildren(const wxTreeItemId& item) const wxOVERRIDE;
    virtual bool IsExpanded(const wxTreeItemId& item) const wxOVERRIDE;
    virtual bool IsSelected(const wxTreeItemId& item) const wxOVERRIDE;
    virtual bool IsBold(const wxTreeItemId& item) const wxOVERRIDE;

    virtual size_t GetChildrenCount(const wxTreeItemId& item, bool recursively = true) const wxOVERRIDE;

    virtual wxTreeItemId GetRootItem() const wxOVERRIDE;
    virtual wxTreeItemId GetSelection() const wxOVERRIDE;
    virtual size_t GetSelections(wxArrayTreeItemIds& selections) const wxOVERRIDE;

    virtual void SetFocusedItem(const wxTreeItemId& item) wxOVERRIDE;
    virtual void ClearFocusedItem() wxOVERRIDE;
    virtual wxTreeItemId GetFocusedItem() const wxOVERRIDE;

    virtual wxTreeItemId GetItemParent(const wxTreeItemId& item) const wxOVERRIDE;
    virtual wxTreeItemId GetFirstChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const wxOVERRIDE;
    virtual wxTreeItemId GetNextChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const wxOVERRIDE;
    virtual wxTreeItemId GetLastChild(const wxTreeItemId& item) const wxOVERRIDE;
    virtual wxTreeItemId GetNextSibling(const wxTreeItemId& item) const wxOVERRIDE;
    virtual wxTreeItemId GetPrevSibling(const wxTreeItemId& item) const wxOVERRIDE;
    virtual wxTreeItemId GetFirstVisibleItem() const wxOVERRIDE;
    virtual wxTreeItemId GetNextVisible(const wxTreeItemId& item) const wxOVERRIDE;
    virtual wxTreeItemId GetPrevVisible(const wxTreeItemId& item) const wxOVERRIDE;

    virtual wxTreeItemId AddRoot(const wxString& text,
                                 int image = -1, int selImage = -1,
                                 wxTreeItemData *data = NULL) wxOVERRIDE;

    virtual void Delete(const wxTreeItemId& item) wxOVERRIDE;
    virtual void DeleteChildren(const wxTreeItemId& item) wxOVERRIDE;
    virtual void DeleteAllItems() wxOVERRIDE;

    virtual void Expand(const wxTreeItemId& item) wxOVERRIDE;
    virtual void Collapse(const wxTreeItemId& item) wxOVERRIDE;
    virtual void CollapseAndReset(const wxTreeItemId& item) wxOVERRIDE;
    virtual void Toggle(const wxTreeItemId& item) wxOVERRIDE;

    virtual void Unselect() wxOVERRIDE;
    virtual void UnselectAll() wxOVERRIDE;
    virtual void SelectItem(const wxTreeItemId& item, bool select = true) wxOVERRIDE;
    virtual void SelectChildren(const wxTreeItemId& parent) wxOVERRIDE;

    virtual void EnsureVisible(const wxTreeItemId& item) wxOVERRIDE;
    virtual void ScrollTo(const wxTreeItemId& item) wxOVERRIDE;

    virtual wxTextCtrl *EditLabel(const wxTreeItemId& item, wxClassInfo* textCtrlClass = CLASSINFO(wxTextCtrl)) wxOVERRIDE;
    virtual wxTextCtrl *GetEditControl() const wxOVERRIDE;
    virtual void EndEditLabel(const wxTreeItemId& item, bool discardChanges = false) wxOVERRIDE;

    virtual void SortChildren(const wxTreeItemId& item) wxOVERRIDE;

    virtual bool GetBoundingRect(const wxTreeItemId& item, wxRect& rect, bool textOnly = false) const wxOVERRIDE;

    virtual void SetWindowStyleFlag(long styles) wxOVERRIDE;

    virtual QWidget *GetHandle() const wxOVERRIDE;

protected:
    virtual int DoGetItemState(const wxTreeItemId& item) const wxOVERRIDE;
    virtual void DoSetItemState(const wxTreeItemId& item, int state) wxOVERRIDE;

    virtual wxTreeItemId DoInsertItem(const wxTreeItemId& parent,
                                      size_t pos,
                                      const wxString& text,
                                      int image, int selImage,
                                      wxTreeItemData *data) wxOVERRIDE;

    virtual wxTreeItemId DoInsertAfter(const wxTreeItemId& parent,
                                       const wxTreeItemId& idPrevious,
                                       const wxString& text,
                                       int image = -1, int selImage = -1,
                                       wxTreeItemData *data = NULL) wxOVERRIDE;

    virtual wxTreeItemId DoTreeHitTest(const wxPoint& point, int& flags) const wxOVERRIDE;

private:
    void SendDeleteEvent(const wxTreeItemId &item);
    wxTreeItemId GetNext(const wxTreeItemId &item) const;

    wxQTreeWidget *m_qtTreeWidget;
    wxDECLARE_DYNAMIC_CLASS(wxTreeCtrl);
};

#endif // _WX_QT_TREECTRL_H_

/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/treectrl.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_TREECTRL_H_
#define _WX_QT_TREECTRL_H_

#include <QtWidgets/QTreeWidget>

class WXDLLIMPEXP_CORE wxTreeCtrl : public wxTreeCtrlBase
{
public:
    wxTreeCtrl();
    wxTreeCtrl(wxWindow *parent, wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxTreeCtrlNameStr);

    bool Create(wxWindow *parent, wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxTreeCtrlNameStr);

    virtual unsigned int GetCount() const;
    
    virtual unsigned int GetIndent() const;
    virtual void SetIndent(unsigned int indent);

    virtual void SetImageList(wxImageList *imageList);
    virtual void SetStateImageList(wxImageList *imageList);

    virtual wxString GetItemText(const wxTreeItemId& item) const;
    virtual int GetItemImage(const wxTreeItemId& item,
                     wxTreeItemIcon which = wxTreeItemIcon_Normal) const;
    virtual wxTreeItemData *GetItemData(const wxTreeItemId& item) const;
    virtual wxColour GetItemTextColour(const wxTreeItemId& item) const;
    virtual wxColour GetItemBackgroundColour(const wxTreeItemId& item) const;
    virtual wxFont GetItemFont(const wxTreeItemId& item) const;

    virtual void SetItemText(const wxTreeItemId& item, const wxString& text);
    virtual void SetItemImage(const wxTreeItemId& item,
                              int image,
                              wxTreeItemIcon which = wxTreeItemIcon_Normal);
    virtual void SetItemData(const wxTreeItemId& item, wxTreeItemData *data);
    virtual void SetItemHasChildren(const wxTreeItemId& item, bool has = true);
    virtual void SetItemBold(const wxTreeItemId& item, bool bold = true);
    virtual void SetItemDropHighlight(const wxTreeItemId& item, bool highlight = true);
    virtual void SetItemTextColour(const wxTreeItemId& item, const wxColour& col);
    virtual void SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& col);
    virtual void SetItemFont(const wxTreeItemId& item, const wxFont& font);
    
    virtual bool IsVisible(const wxTreeItemId& item) const;
    virtual bool ItemHasChildren(const wxTreeItemId& item) const;
    virtual bool IsExpanded(const wxTreeItemId& item) const;
    virtual bool IsSelected(const wxTreeItemId& item) const;
    virtual bool IsBold(const wxTreeItemId& item) const;
    
    virtual size_t GetChildrenCount(const wxTreeItemId& item, bool recursively = true) const;
    
    virtual wxTreeItemId GetRootItem() const;
    virtual wxTreeItemId GetSelection() const;
    virtual size_t GetSelections(wxArrayTreeItemIds& selections) const;

    virtual void SetFocusedItem(const wxTreeItemId& item);
    virtual void ClearFocusedItem();
    virtual wxTreeItemId GetFocusedItem() const;

    virtual wxTreeItemId GetItemParent(const wxTreeItemId& item) const;
    
    virtual wxTreeItemId GetFirstChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
    virtual wxTreeItemId GetNextChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
    virtual wxTreeItemId GetLastChild(const wxTreeItemId& item) const;
    virtual wxTreeItemId GetNextSibling(const wxTreeItemId& item) const;
    virtual wxTreeItemId GetPrevSibling(const wxTreeItemId& item) const;
    virtual wxTreeItemId GetFirstVisibleItem() const;
    virtual wxTreeItemId GetNextVisible(const wxTreeItemId& item) const;
    virtual wxTreeItemId GetPrevVisible(const wxTreeItemId& item) const;

    virtual wxTreeItemId AddRoot(const wxString& text,
                                 int image = -1, int selImage = -1,
                                 wxTreeItemData *data = NULL);

    virtual void Delete(const wxTreeItemId& item);
    virtual void DeleteChildren(const wxTreeItemId& item);
    virtual void DeleteAllItems();

    virtual void Expand(const wxTreeItemId& item);
    virtual void Collapse(const wxTreeItemId& item);
    virtual void CollapseAndReset(const wxTreeItemId& item);
    virtual void Toggle(const wxTreeItemId& item);

    virtual void Unselect();
    virtual void UnselectAll();
    virtual void SelectItem(const wxTreeItemId& item, bool select = true);
    virtual void SelectChildren(const wxTreeItemId& parent);

    virtual void EnsureVisible(const wxTreeItemId& item);
    virtual void ScrollTo(const wxTreeItemId& item);

    virtual wxTextCtrl *EditLabel(const wxTreeItemId& item, wxClassInfo* textCtrlClass = CLASSINFO(wxTextCtrl));
    virtual wxTextCtrl *GetEditControl() const;
    virtual void EndEditLabel(const wxTreeItemId& item, bool discardChanges = false);

    virtual void SortChildren(const wxTreeItemId& item);

    virtual bool GetBoundingRect(const wxTreeItemId& item, wxRect& rect, bool textOnly = false) const;

    virtual QTreeWidget *GetHandle() const;

protected:
    virtual int DoGetItemState(const wxTreeItemId& item) const;
    virtual void DoSetItemState(const wxTreeItemId& item, int state);

    virtual wxTreeItemId DoInsertItem(const wxTreeItemId& parent,
                                      size_t pos,
                                      const wxString& text,
                                      int image, int selImage,
                                      wxTreeItemData *data);

    virtual wxTreeItemId DoInsertAfter(const wxTreeItemId& parent,
                                       const wxTreeItemId& idPrevious,
                                       const wxString& text,
                                       int image = -1, int selImage = -1,
                                       wxTreeItemData *data = NULL);

    virtual wxTreeItemId DoTreeHitTest(const wxPoint& point, int& flags) const;

private:
    QTreeWidget *m_qtTreeWidget;

    wxDECLARE_DYNAMIC_CLASS(wxTreeCtrl);
};

#endif // _WX_QT_TREECTRL_H_

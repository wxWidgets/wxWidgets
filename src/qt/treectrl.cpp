/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/treectrl.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/treectrl.h"

wxTreeCtrl::wxTreeCtrl()
{
}

wxTreeCtrl::wxTreeCtrl(wxWindow *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxValidator& validator,
           const wxString& name)
{
    Create( parent, id, pos, size, style, validator, name );
}

bool wxTreeCtrl::Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    m_qtTreeWidget = new QTreeWidget( parent->GetHandle() );

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

unsigned wxTreeCtrl::GetCount() const
{
    return 0;
}


unsigned wxTreeCtrl::GetIndent() const
{
    return m_qtTreeWidget->columnCount();
}

void wxTreeCtrl::SetIndent(unsigned int indent)
{
    m_qtTreeWidget->setColumnCount( indent );
}

void wxTreeCtrl::SetImageList(wxImageList *imageList)
{
}

void wxTreeCtrl::SetStateImageList(wxImageList *imageList)
{
}

wxString wxTreeCtrl::GetItemText(const wxTreeItemId& item) const
{
    return wxString();
}

int wxTreeCtrl::GetItemImage(const wxTreeItemId& item,
                 wxTreeItemIcon which) const
{
    return 0;
}

wxTreeItemData *wxTreeCtrl::GetItemData(const wxTreeItemId& item) const
{
    return NULL;
}

wxColour wxTreeCtrl::GetItemTextColour(const wxTreeItemId& item) const
{
    return wxColour();
}

wxColour wxTreeCtrl::GetItemBackgroundColour(const wxTreeItemId& item) const
{
    return wxColour();
}

wxFont wxTreeCtrl::GetItemFont(const wxTreeItemId& item) const
{
    return wxFont();
}

void wxTreeCtrl::SetItemText(const wxTreeItemId& item, const wxString& text)
{
}

void wxTreeCtrl::SetItemImage(const wxTreeItemId& item,
                          int image,
                          wxTreeItemIcon which)
{
}

void wxTreeCtrl::SetItemData(const wxTreeItemId& item, wxTreeItemData *data)
{
}

void wxTreeCtrl::SetItemHasChildren(const wxTreeItemId& item, bool has)
{
}

void wxTreeCtrl::SetItemBold(const wxTreeItemId& item, bool bold)
{
}

void wxTreeCtrl::SetItemDropHighlight(const wxTreeItemId& item, bool highlight)
{
}

void wxTreeCtrl::SetItemTextColour(const wxTreeItemId& item, const wxColour& col)
{
}

void wxTreeCtrl::SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& col)
{
}

void wxTreeCtrl::SetItemFont(const wxTreeItemId& item, const wxFont& font)
{
}

bool wxTreeCtrl::IsVisible(const wxTreeItemId& item) const
{
    return false;
}

bool wxTreeCtrl::ItemHasChildren(const wxTreeItemId& item) const
{
    return false;
}

bool wxTreeCtrl::IsExpanded(const wxTreeItemId& item) const
{
    return false;
}

bool wxTreeCtrl::IsSelected(const wxTreeItemId& item) const
{
    return false;
}

bool wxTreeCtrl::IsBold(const wxTreeItemId& item) const
{
    return false;
}

size_t wxTreeCtrl::GetChildrenCount(const wxTreeItemId& item, bool recursively) const
{
    return 0;
}

wxTreeItemId wxTreeCtrl::GetRootItem() const
{
    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetSelection() const
{
    return wxTreeItemId();
}

size_t wxTreeCtrl::GetSelections(wxArrayTreeItemIds& selections) const
{
    return 0;
}

void wxTreeCtrl::SetFocusedItem(const wxTreeItemId& item)
{

}

void wxTreeCtrl::ClearFocusedItem()
{

}

wxTreeItemId wxTreeCtrl::GetFocusedItem() const
{
    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetItemParent(const wxTreeItemId& item) const
{
    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetFirstChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const
{
    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetNextChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const
{
    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetLastChild(const wxTreeItemId& item) const
{
    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetNextSibling(const wxTreeItemId& item) const
{
    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetPrevSibling(const wxTreeItemId& item) const
{
    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetFirstVisibleItem() const
{
    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetNextVisible(const wxTreeItemId& item) const
{
    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetPrevVisible(const wxTreeItemId& item) const
{
    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::AddRoot(const wxString& text,
                             int image, int selImage,
                             wxTreeItemData *data)
{
    return wxTreeItemId();
}

void wxTreeCtrl::Delete(const wxTreeItemId& item)
{
}

void wxTreeCtrl::DeleteChildren(const wxTreeItemId& item)
{
}

void wxTreeCtrl::DeleteAllItems()
{
}

void wxTreeCtrl::Expand(const wxTreeItemId& item)
{
}

void wxTreeCtrl::Collapse(const wxTreeItemId& item)
{
}

void wxTreeCtrl::CollapseAndReset(const wxTreeItemId& item)
{
}

void wxTreeCtrl::Toggle(const wxTreeItemId& item)
{
}

void wxTreeCtrl::Unselect()
{
}

void wxTreeCtrl::UnselectAll()
{
}

void wxTreeCtrl::SelectItem(const wxTreeItemId& item, bool select)
{
}

void wxTreeCtrl::SelectChildren(const wxTreeItemId& parent)
{
}

void wxTreeCtrl::EnsureVisible(const wxTreeItemId& item)
{
}

void wxTreeCtrl::ScrollTo(const wxTreeItemId& item)
{
}

wxTextCtrl *wxTreeCtrl::EditLabel(const wxTreeItemId& item, wxClassInfo* textCtrlClass)
{
    return NULL;
}

wxTextCtrl *wxTreeCtrl::GetEditControl() const
{
    return NULL;
}

void wxTreeCtrl::EndEditLabel(const wxTreeItemId& item, bool discardChanges)
{
}

void wxTreeCtrl::SortChildren(const wxTreeItemId& item)
{
}

bool wxTreeCtrl::GetBoundingRect(const wxTreeItemId& item, wxRect& rect, bool textOnly) const
{
    return false;
}

int wxTreeCtrl::DoGetItemState(const wxTreeItemId& item) const
{
    return 0;
}

void wxTreeCtrl::DoSetItemState(const wxTreeItemId& item, int state)
{
}

wxTreeItemId wxTreeCtrl::DoInsertItem(const wxTreeItemId& parent,
                                  size_t pos,
                                  const wxString& text,
                                  int image, int selImage,
                                  wxTreeItemData *data)
{
    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::DoInsertAfter(const wxTreeItemId& parent,
                                   const wxTreeItemId& idPrevious,
                                   const wxString& text,
                                   int image, int selImage,
                                   wxTreeItemData *data)
{
    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::DoTreeHitTest(const wxPoint& point, int& flags) const
{
    return wxTreeItemId();
}

QTreeWidget *wxTreeCtrl::GetHandle() const
{
    return m_qtTreeWidget;
}

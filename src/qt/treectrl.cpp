/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/treectrl.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#include "wx/treectrl.h"
#include "wx/qt/private/winevent.h"
#include "wx/imaglist.h"

#include <QtWidgets/QTreeWidget>

namespace
{
    QTreeWidgetItem *wxQtConvertTreeItem(const wxTreeItemId &item)
    {
        return static_cast<QTreeWidgetItem*>(item.GetID());
    }

    wxTreeItemId wxQtConvertTreeItem(QTreeWidgetItem *item)
    {
        return wxTreeItemId(item);
    }

    class wxQTreeWidget : public wxQtEventSignalHandler<QTreeWidget, wxTreeCtrl>
    {
    public:
        wxQTreeWidget(wxWindow *parent, wxTreeCtrl *handler) :
            wxQtEventSignalHandler(parent, handler)
        {      
            connect(this, &QTreeWidget::currentItemChanged, this, &wxQTreeWidget::OnCurrentItemChanged);
            connect(this, &QTreeWidget::itemActivated, this, &wxQTreeWidget::OnItemActivated);
            connect(this, &QTreeWidget::itemClicked, this, &wxQTreeWidget::OnItemClicked);
            connect(this, &QTreeWidget::itemCollapsed, this, &wxQTreeWidget::OnItemCollapsed);
            connect(this, &QTreeWidget::itemExpanded, this, &wxQTreeWidget::OnItemExpanded);
        }

    private:
        void OnCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
        {
            wxTreeEvent changingEvent(wxEVT_TREE_SEL_CHANGING, GetHandler(), wxQtConvertTreeItem(current));
            EmitEvent(changingEvent);

            if ( !changingEvent.IsAllowed() )
            {
                blockSignals(true);
                setCurrentItem(previous);
                blockSignals(false);
                return;
            }

            wxTreeEvent changedEvent(wxEVT_TREE_SEL_CHANGED, GetHandler(), wxQtConvertTreeItem(current));
            EmitEvent(changedEvent);
        }

        void OnItemActivated(QTreeWidgetItem *item, int WXUNUSED(column))
        {
            wxTreeEvent event(wxEVT_TREE_ITEM_ACTIVATED, GetHandler(), wxQtConvertTreeItem(item));
            EmitEvent(event);
        }

        void OnItemClicked(QTreeWidgetItem *item)
        {
            wxMouseState mouseState = wxGetMouseState();

            wxEventType eventType;
            if (mouseState.RightIsDown())
                eventType = wxEVT_TREE_ITEM_RIGHT_CLICK;
            else if (mouseState.MiddleIsDown())
                eventType = wxEVT_TREE_ITEM_MIDDLE_CLICK;
            else
                return;

            wxTreeEvent event(eventType, GetHandler(), wxQtConvertTreeItem(item));
            EmitEvent(event);
        }

        void OnItemCollapsed(QTreeWidgetItem *item)
        {
            wxTreeEvent collapsingEvent(wxEVT_TREE_ITEM_COLLAPSING, GetHandler(), wxQtConvertTreeItem(item));
            EmitEvent(collapsingEvent);

            if (!collapsingEvent.IsAllowed())
            {
                blockSignals(true);
                item->setExpanded(true);
                blockSignals(false);
                return;
            }

            wxTreeEvent collapsedEvent(wxEVT_TREE_ITEM_COLLAPSED, GetHandler(), wxQtConvertTreeItem(item));
            EmitEvent(collapsedEvent);
        }

        void OnItemExpanded(QTreeWidgetItem *item)
        {
            wxTreeEvent expandingEvent(wxEVT_TREE_ITEM_EXPANDING, GetHandler(), wxQtConvertTreeItem(item));
            EmitEvent(expandingEvent);

            if (!expandingEvent.IsAllowed())
            {
                blockSignals(true);
                item->setExpanded(false);
                blockSignals(false);
                return;
            }

            wxTreeEvent expandedEvent(wxEVT_TREE_ITEM_EXPANDED, GetHandler(), wxQtConvertTreeItem(item));
            EmitEvent(expandedEvent);

        }
    };


    size_t CountChildren(QTreeWidgetItem *item)
    {
        const int currentCount = item->childCount();
        size_t totalCount = currentCount;

        for(int i = 0; i < currentCount; ++i)
        {
            totalCount += CountChildren(item->child(i));
        }

        return totalCount;
    }

    QIcon::Mode TreeIconToQIconMode(wxTreeItemIcon icon)
    {
        switch (icon)
        {
        case wxTreeItemIcon_Normal:
            return QIcon::Normal;
        case wxTreeItemIcon_Selected:
            return QIcon::Selected;
        }

        wxFAIL_MSG("Unspported tree icon state");
        return QIcon::Normal;
    }

}

wxTreeCtrl::wxTreeCtrl() :
    m_qtTreeWidget(NULL)
{
}

wxTreeCtrl::wxTreeCtrl(wxWindow *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxValidator& validator,
           const wxString& name) 
{
    Create(parent, id, pos, size, style, validator, name);
}

bool wxTreeCtrl::Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    m_qtTreeWidget = new wxQTreeWidget(parent, this);
    SetWindowStyleFlag(style);

    return QtCreateControl(parent, id, pos, size, style, validator, name);
}

wxTreeCtrl::~wxTreeCtrl()
{
    if (m_qtTreeWidget != NULL)
        m_qtTreeWidget->deleteLater();
}

unsigned wxTreeCtrl::GetCount() const
{
    QTreeWidgetItem *root = m_qtTreeWidget->invisibleRootItem();
    if (root->childCount() == 0)
        return 0;

    return CountChildren(root->child(0));
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
    m_imageListNormal = imageList;
}

void wxTreeCtrl::SetStateImageList(wxImageList *imageList)
{
    m_imageListState = imageList;
}

wxString wxTreeCtrl::GetItemText(const wxTreeItemId& item) const
{
    if (!item.IsOk())
        return "";

    QTreeWidgetItem* qTreeItem = wxQtConvertTreeItem(item);
    return wxQtConvertString(qTreeItem->text(0));
}

int wxTreeCtrl::GetItemImage(const wxTreeItemId& item, wxTreeItemIcon which) const
{
    wxCHECK_MSG(item.IsOk(), 0, "invalid tree item");
    return 0;

}

wxTreeItemData *wxTreeCtrl::GetItemData(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), NULL, "invalid tree item");

    QTreeWidgetItem* qTreeItem = wxQtConvertTreeItem(item);
    QVariant itemData = qTreeItem->data(0, Qt::UserRole);
    void* value =  itemData.value<void *>();
    return static_cast<wxTreeItemData*>(value);
}

wxColour wxTreeCtrl::GetItemTextColour(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxNullColour, "invalid tree item");

    QTreeWidgetItem* qTreeItem = wxQtConvertTreeItem(item);
    return wxQtConvertColour(qTreeItem->textColor(0));

}

wxColour wxTreeCtrl::GetItemBackgroundColour(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxNullColour, "invalid tree item");

    QTreeWidgetItem* qTreeItem = wxQtConvertTreeItem(item);
    return wxQtConvertColour(qTreeItem->backgroundColor(0));
}

wxFont wxTreeCtrl::GetItemFont(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxNullFont, "invalid tree item");

    QTreeWidgetItem* qTreeItem = wxQtConvertTreeItem(item);
    return wxFont(qTreeItem->font(0));
}

void wxTreeCtrl::SetItemText(const wxTreeItemId& item, const wxString& text)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");
    QTreeWidgetItem* qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setText(0, wxQtConvertString(text));
}

void wxTreeCtrl::SetItemImage(const wxTreeItemId& item, int image, wxTreeItemIcon which)
{
    if (m_imageListNormal == NULL)
        return;

    if (image == -1)
        return;

    wxBitmap bitmap = m_imageListNormal->GetBitmap(image);
    wxASSERT(bitmap.IsOk());

    QTreeWidgetItem* qTreeItem = wxQtConvertTreeItem(item);
    QIcon::Mode mode = TreeIconToQIconMode(which);
    QIcon icon = qTreeItem->icon(0);
    icon.addPixmap(*bitmap.GetHandle(), mode);
    qTreeItem->setIcon(0, icon);
}

void wxTreeCtrl::SetItemData(const wxTreeItemId& item, wxTreeItemData *data)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    if (data != NULL)
        data->SetId(item);

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setData(0, Qt::UserRole, QVariant::fromValue(static_cast<void*>(data)));
}

void wxTreeCtrl::SetItemHasChildren(const wxTreeItemId& item, bool has)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setChildIndicatorPolicy(has ? QTreeWidgetItem::ShowIndicator : QTreeWidgetItem::DontShowIndicatorWhenChildless);
}

void wxTreeCtrl::SetItemBold(const wxTreeItemId& item, bool bold)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    QFont font = qTreeItem->font(0);
    font.setBold(bold);
    qTreeItem->setFont(0, font);
}

void wxTreeCtrl::SetItemDropHighlight(const wxTreeItemId& item, bool highlight)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);

    wxColour fg, bg;

    if (highlight)
    {
        bg = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
        fg = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    }
    else
    {
        bg = GetBackgroundColour();
        fg = GetForegroundColour();
    }

    qTreeItem->setBackgroundColor(0, wxQtConvertColour(bg));
    qTreeItem->setTextColor(0, wxQtConvertColour(fg));
}

void wxTreeCtrl::SetItemTextColour(const wxTreeItemId& item, const wxColour& col)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setTextColor(0, wxQtConvertColour(col));
}

void wxTreeCtrl::SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& col)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setTextColor(0, wxQtConvertColour(col));
}

void wxTreeCtrl::SetItemFont(const wxTreeItemId& item, const wxFont& font)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setFont(0, font.GetHandle());
}

bool wxTreeCtrl::IsVisible(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    const QRect visualRect = m_qtTreeWidget->visualItemRect(qTreeItem);
    return visualRect.isValid();
}

bool wxTreeCtrl::ItemHasChildren(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    return qTreeItem->childCount() > 0;
}

bool wxTreeCtrl::IsExpanded(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    return qTreeItem->isExpanded();
}

bool wxTreeCtrl::IsSelected(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    return qTreeItem->isSelected();
}

bool wxTreeCtrl::IsBold(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    QFont font = qTreeItem->font(0);
    return font.bold();
}

size_t wxTreeCtrl::GetChildrenCount(const wxTreeItemId& item, bool recursively) const
{
    wxCHECK_MSG(item.IsOk(), 0, "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);

    if (recursively)
        return CountChildren(qTreeItem);

    return qTreeItem->childCount();
}

wxTreeItemId wxTreeCtrl::GetRootItem() const
{
    QTreeWidgetItem *root = m_qtTreeWidget->invisibleRootItem();
    return wxQtConvertTreeItem(root->child(0));
}

wxTreeItemId wxTreeCtrl::GetSelection() const
{
    QList<QTreeWidgetItem*> selections = m_qtTreeWidget->selectedItems();
    return selections.isEmpty() ? wxTreeItemId() : wxQtConvertTreeItem(selections[0]);

}

size_t wxTreeCtrl::GetSelections(wxArrayTreeItemIds& selections) const
{
    QList<QTreeWidgetItem*> qtSelections = m_qtTreeWidget->selectedItems();

    const size_t numberOfSelections = qtSelections.size();
    selections.SetCount(numberOfSelections);

    for (size_t i = 0; i < numberOfSelections; ++i)
    {
        selections[i] = qtSelections[i];
    }

    return numberOfSelections;
}

void wxTreeCtrl::SetFocusedItem(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");
    m_qtTreeWidget->setCurrentItem(wxQtConvertTreeItem(item), 0);
}

void wxTreeCtrl::ClearFocusedItem()
{
    m_qtTreeWidget->setCurrentItem(NULL);
}

wxTreeItemId wxTreeCtrl::GetFocusedItem() const
{
    return wxQtConvertTreeItem(m_qtTreeWidget->currentItem());
}

wxTreeItemId wxTreeCtrl::GetItemParent(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    return wxQtConvertTreeItem(qTreeItem->parent());
}

wxTreeItemId wxTreeCtrl::GetFirstChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    cookie = 0;
    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);

    return qTreeItem->childCount() > 0 ? wxQtConvertTreeItem(qTreeItem->child(0)) : wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetNextChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    int currentIndex = reinterpret_cast<int>(cookie);
    ++currentIndex;

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);

    if ( currentIndex < qTreeItem->childCount() )
    {
        cookie = reinterpret_cast<wxTreeItemIdValue>(currentIndex);
        return wxQtConvertTreeItem(qTreeItem->child(currentIndex));
    }

    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetLastChild(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    const int childCount = qTreeItem->childCount();
    return childCount == 0 ? wxTreeItemId() : wxQtConvertTreeItem(qTreeItem->child(childCount - 1));
}

wxTreeItemId wxTreeCtrl::GetNextSibling(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    QTreeWidgetItem *parent = qTreeItem->parent();

    if ( parent != NULL )
    {
        int index = parent->indexOfChild(qTreeItem);
        wxASSERT(index != -1);

        ++index;
        return index < parent->childCount() ? wxQtConvertTreeItem(parent->child(index)) : wxTreeItemId();
    }

    int index = m_qtTreeWidget->indexOfTopLevelItem(qTreeItem);
    wxASSERT(index != -1);

    ++index;
    return index < m_qtTreeWidget->topLevelItemCount() ? wxQtConvertTreeItem(m_qtTreeWidget->topLevelItem(index)) : wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetPrevSibling(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    QTreeWidgetItem *parent = qTreeItem->parent();

    if (parent != NULL)
    {
        int index = parent->indexOfChild(qTreeItem);
        wxASSERT(index != -1);

        --index;
        return index >= 0 ? wxQtConvertTreeItem(parent->child(index)) : wxTreeItemId();
    }

    int index = m_qtTreeWidget->indexOfTopLevelItem(qTreeItem);
    wxASSERT(index != -1);

    --index;
    return index >= 0 ? wxQtConvertTreeItem(m_qtTreeWidget->topLevelItem(index)) : wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetFirstVisibleItem() const
{
   wxTreeItemId itemid = GetRootItem();
    if (!itemid.IsOk())
        return itemid;
    do
    {
        if (IsVisible(itemid))
            return itemid;
        itemid = GetNext(itemid);
    } while (itemid.IsOk());

    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetNextVisible(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), wxT("invalid tree item"));
    wxASSERT_MSG(IsVisible(item), wxT("this item itself should be visible"));

    wxTreeItemId id = item;
    if (id.IsOk())
    {
        while (id = GetNext(id), id.IsOk())
        {
            if (IsVisible(id))
                return id;
        }
    }
    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetPrevVisible(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), wxT("invalid tree item"));
    wxASSERT_MSG(IsVisible(item), wxT("this item itself should be visible"));

    // find out the starting point
    wxTreeItemId prevItem = GetPrevSibling(item);
    if (!prevItem.IsOk())
    {
        prevItem = GetItemParent(item);
    }

    // find the first visible item after it
    while (prevItem.IsOk() && !IsVisible(prevItem))
    {
        prevItem = GetNext(prevItem);
        if (!prevItem.IsOk() || prevItem == item)
        {
            // there are no visible items before item
            return wxTreeItemId();
        }
    }

    // from there we must be able to navigate until this item
    while (prevItem.IsOk())
    {
        const wxTreeItemId nextItem = GetNextVisible(prevItem);
        if (!nextItem.IsOk() || nextItem == item)
            break;

        prevItem = nextItem;
    }

    return prevItem;
}

wxTreeItemId wxTreeCtrl::AddRoot(const wxString& text,
                             int image, int selImage,
                             wxTreeItemData *data)
{
    QTreeWidgetItem *root = m_qtTreeWidget->invisibleRootItem();
    return DoInsertItem(wxQtConvertTreeItem(root), 0, text, image, selImage, data);
}

void wxTreeCtrl::Delete(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    QTreeWidgetItem *parent = qTreeItem->parent();

    if ( parent != NULL )
    {
        parent->removeChild(qTreeItem);
    }
    else
    {
        m_qtTreeWidget->removeItemWidget(qTreeItem, 0);
    }

    SendDeleteEvent(item);

    delete qTreeItem;
}

void wxTreeCtrl::DeleteChildren(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    while (qTreeItem->childCount() > 0)
    {
        QTreeWidgetItem *child = qTreeItem->child(0);
        DeleteChildren(wxQtConvertTreeItem(child));
        qTreeItem->removeChild(child);

        SendDeleteEvent(wxQtConvertTreeItem(child));

        delete child;
    }
}

void wxTreeCtrl::DeleteAllItems()
{
    DeleteChildren(wxQtConvertTreeItem(m_qtTreeWidget->invisibleRootItem()));
}

void wxTreeCtrl::Expand(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setExpanded(true);
}

void wxTreeCtrl::Collapse(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setExpanded(false);
}

void wxTreeCtrl::CollapseAndReset(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    Collapse(item);
    DeleteChildren(item);
}

void wxTreeCtrl::Toggle(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setSelected(!qTreeItem->isSelected());
}

void wxTreeCtrl::Unselect()
{
    QTreeWidgetItem *current = m_qtTreeWidget->currentItem();

    if (current != NULL)
        current->setSelected(false);
}

void wxTreeCtrl::UnselectAll()
{
    QList<QTreeWidgetItem *> selections = m_qtTreeWidget->selectedItems();
    const size_t selectedCount = selections.size();
    for ( size_t i = 0; i < selectedCount; ++i)
    {
        selections[i]->setSelected(false);
    }
}

void wxTreeCtrl::SelectItem(const wxTreeItemId& item, bool select)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    if ( !HasFlag(wxTR_MULTIPLE) )
    {
        QList<QTreeWidgetItem *> selections = m_qtTreeWidget->selectedItems();
        const size_t nSelections = selections.size();
        m_qtTreeWidget->clearSelection();
    }

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setSelected(select);
}

void wxTreeCtrl::SelectChildren(const wxTreeItemId& parent)
{
    wxCHECK_RET(parent.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(parent);
    const int childCount = qTreeItem->childCount();

    for (int i = 0; i < childCount; ++i)
    {
        qTreeItem->child(i)->setSelected(true);
    }
}

void wxTreeCtrl::EnsureVisible(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    QTreeWidgetItem *parent = qTreeItem->parent();

    while ( parent != NULL )
    {
        parent->setExpanded(true);
        parent = parent->parent();
    }

    ScrollTo(item);
}

void wxTreeCtrl::ScrollTo(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    m_qtTreeWidget->scrollToItem(qTreeItem);
}

wxTextCtrl *wxTreeCtrl::EditLabel(const wxTreeItemId& item, wxClassInfo* textCtrlClass)
{
    wxCHECK_MSG(item.IsOk(), NULL, "invalid tree item");
    return NULL;
}

wxTextCtrl *wxTreeCtrl::GetEditControl() const
{
    return NULL;
}

void wxTreeCtrl::EndEditLabel(const wxTreeItemId& item, bool discardChanges)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");
}

void wxTreeCtrl::SortChildren(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->sortChildren(0, Qt::AscendingOrder);
}

bool wxTreeCtrl::GetBoundingRect(const wxTreeItemId& item, wxRect& rect, bool textOnly) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");
    return false;
}

void wxTreeCtrl::SetWindowStyleFlag(long styles)
{
    wxControl::SetWindowStyleFlag(styles);
    m_qtTreeWidget->invisibleRootItem()->setHidden((styles & wxTR_HIDE_ROOT) != 0);
    m_qtTreeWidget->setSelectionMode(styles & wxTR_MULTIPLE ? QTreeWidget::MultiSelection : QTreeWidget::SingleSelection);
}

int wxTreeCtrl::DoGetItemState(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTREE_ITEMSTATE_NONE, "invalid tree item");
    return 0;
}

void wxTreeCtrl::DoSetItemState(const wxTreeItemId& item, int state)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");
}

wxTreeItemId wxTreeCtrl::DoInsertItem(const wxTreeItemId& parent,
                                  size_t pos,
                                  const wxString& text,
                                  int image, int selImage,
                                  wxTreeItemData *data)
{
    wxCHECK_MSG(parent.IsOk(), wxTreeItemId(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(parent);

    QTreeWidgetItem *newItem = new QTreeWidgetItem;
    newItem->setText(0, wxQtConvertString(text));
    newItem->setData(0, Qt::UserRole, QVariant::fromValue(static_cast<void*>(data)));

    if (pos == static_cast<size_t>(-1))
    {
        qTreeItem->addChild(newItem);
    }
    else
    {
        qTreeItem->insertChild(pos, newItem);
    }

    wxTreeItemId wxItem = wxQtConvertTreeItem(newItem);
    SetItemImage(wxItem, image, wxTreeItemIcon_Normal);
    SetItemImage(wxItem, selImage, wxTreeItemIcon_Selected);

    if (data != NULL)
        data->SetId(wxItem);

    return wxItem;
}

wxTreeItemId wxTreeCtrl::DoInsertAfter(const wxTreeItemId& parent,
                                   const wxTreeItemId& idPrevious,
                                   const wxString& text,
                                   int image, int selImage,
                                   wxTreeItemData *data)
{
    wxCHECK_MSG(parent.IsOk(), wxTreeItemId(), "invalid tree item");
    wxCHECK_MSG(idPrevious.IsOk(), wxTreeItemId(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(parent);
    const int index = qTreeItem->indexOfChild(wxQtConvertTreeItem(idPrevious));
    return DoInsertItem(parent, index + 1, text, image, selImage, data);
}

wxTreeItemId wxTreeCtrl::DoTreeHitTest(const wxPoint& point, int& flags) const
{
    int w, h;
    GetSize(&w, &h);
    flags = 0;
    if (point.x < 0) flags |= wxTREE_HITTEST_TOLEFT;
    if (point.x > w) flags |= wxTREE_HITTEST_TORIGHT;
    if (point.y < 0) flags |= wxTREE_HITTEST_ABOVE;
    if (point.y > h) flags |= wxTREE_HITTEST_BELOW;
    if (flags) return wxTreeItemId();

    QTreeWidgetItem *hitItem = m_qtTreeWidget->itemAt(wxQtConvertPoint(point));

    if (hitItem == NULL)
        flags |= wxTREE_HITTEST_NOWHERE;

    return wxQtConvertTreeItem(hitItem);
}

QWidget *wxTreeCtrl::GetHandle() const
{
    return m_qtTreeWidget;
}

void wxTreeCtrl::SendDeleteEvent(const wxTreeItemId &item)
{
    wxTreeEvent event(wxEVT_TREE_DELETE_ITEM, GetId());
    event.SetItem(item);
    HandleWindowEvent(event);
}

wxTreeItemId wxTreeCtrl::GetNext(const wxTreeItemId &item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), wxT("invalid tree item"));

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);

    if ( qTreeItem->childCount() > 0)
    {
        return qTreeItem->child(0);
    }
    
    // Try a sibling of this or ancestor instead
    wxTreeItemId p = item;
    wxTreeItemId toFind;
    do
    {
        toFind = GetNextSibling(p);
        p = GetItemParent(p);
    } while (p.IsOk() && !toFind.IsOk());
    return toFind;
}


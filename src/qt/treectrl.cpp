/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/treectrl.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_TREECTRL

#include "wx/treectrl.h"
#include "wx/imaglist.h"
#include "wx/settings.h"
#include "wx/sharedptr.h"
#include "wx/withimages.h"

#include "wx/qt/private/winevent.h"
#include "wx/qt/private/treeitemdelegate.h"

#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QScrollBar>
#include <QtGui/QPainter>

namespace
{
struct TreeItemDataQt
{
    TreeItemDataQt()
    {
    }

    explicit TreeItemDataQt(wxTreeItemData* data) : data(data)
    {
        static bool s_registered = false;
        if ( !s_registered )
        {
            qRegisterMetaTypeStreamOperators<TreeItemDataQt>("TreeItemDataQt");
            s_registered = true;
        }
    }

    wxTreeItemData *getData() const
    {
        return data.get();
    }

private:
    wxSharedPtr<wxTreeItemData> data;
};

QDataStream &operator<<(QDataStream &out, const TreeItemDataQt &WXUNUSED(obj))
{
    return out;
}
QDataStream &operator>>(QDataStream &in, TreeItemDataQt &WXUNUSED(obj))
{
    return in;
}

QTreeWidgetItem *wxQtConvertTreeItem(const wxTreeItemId &item)
{
    return static_cast<QTreeWidgetItem*>(item.GetID());
}

wxTreeItemId wxQtConvertTreeItem(QTreeWidgetItem *item)
{
    return wxTreeItemId(item);
}

size_t CountChildren(QTreeWidgetItem *item)
{
    const int currentCount = item->childCount();
    size_t totalCount = currentCount;

    for ( int i = 0; i < currentCount; ++i )
    {
        totalCount += CountChildren(item->child(i));
    }

    return totalCount;
}

class ImageState
{
public:
    ImageState() : m_state(wxTREE_ITEMSTATE_NONE)
    {
        for ( int i = wxTreeItemIcon_Normal; i < wxTreeItemIcon_Max; ++i )
        {
            m_imageStates[i] = wxWithImages::NO_IMAGE;
        }
    }

    int &operator[](size_t index)
    {
        wxASSERT(index < wxTreeItemIcon_Max);
        return m_imageStates[index];
    }

    int operator[](size_t index) const
    {
        wxASSERT(index < wxTreeItemIcon_Max);
        return m_imageStates[index];
    }

    void SetState(int state)
    {
        m_state = state;
    }

    int GetState() const
    {
        return m_state;
    }

private:
    int m_imageStates[wxTreeItemIcon_Max];
    int m_state;
};

}

Q_DECLARE_METATYPE(TreeItemDataQt)

class wxQTreeWidget : public wxQtEventSignalHandler<QTreeWidget, wxTreeCtrl>
{
public:
    wxQTreeWidget(wxWindow *parent, wxTreeCtrl *handler) :
        wxQtEventSignalHandler<QTreeWidget, wxTreeCtrl>(parent, handler),
        m_item_delegate(handler),
        m_closing_editor(0)
    {
        connect(this, &QTreeWidget::currentItemChanged,
                this, &wxQTreeWidget::OnCurrentItemChanged);
        connect(this, &QTreeWidget::itemActivated,
                this, &wxQTreeWidget::OnItemActivated);
        connect(this, &QTreeWidget::itemCollapsed,
                this, &wxQTreeWidget::OnItemCollapsed);
        connect(this, &QTreeWidget::itemExpanded,
                this, &wxQTreeWidget::OnItemExpanded);
        connect(verticalScrollBar(), &QScrollBar::valueChanged,
                this, &wxQTreeWidget::OnTreeScrolled);

        setItemDelegate(&m_item_delegate);
        setDragEnabled(true);
        viewport()->setAcceptDrops(true);
        setDropIndicatorShown(true);
        setEditTriggers(QAbstractItemView::SelectedClicked);
    }

    virtual void paintEvent (QPaintEvent * event)
    {
        //QT generates warnings if we try to paint to a QTreeWidget
        //(perhaps because it's a compound widget) so we've disabled
        //wx paint and erase background events
        QTreeWidget::paintEvent(event);
    }

    virtual void mouseReleaseEvent(QMouseEvent * event) override
    {
        const QPoint qPos = event->pos();
        QTreeWidgetItem *item = itemAt(qPos);
        if ( item != nullptr )
        {
            const wxPoint pos(qPos.x(), qPos.y());
            switch ( event->button() )
            {
                case Qt::RightButton:
                {
                    wxTreeEvent treeEvent(wxEVT_TREE_ITEM_RIGHT_CLICK,
                        GetHandler(),
                        wxQtConvertTreeItem(item));
                    treeEvent.SetPoint(pos);
                    EmitEvent(treeEvent);

                    wxTreeEvent menuEvent(wxEVT_TREE_ITEM_MENU,
                        GetHandler(),
                        wxQtConvertTreeItem(item));
                    menuEvent.SetPoint(pos);
                    EmitEvent(menuEvent);

                    break;
                }
                case Qt::MiddleButton:
                {
                    wxTreeEvent treeEvent(wxEVT_TREE_ITEM_MIDDLE_CLICK,
                        GetHandler(),
                        wxQtConvertTreeItem(item));
                    treeEvent.SetPoint(pos);
                    EmitEvent(treeEvent);
                    break;
                }
                default:
                    break;
                }
        }

        return wxQtEventSignalHandler<QTreeWidget, wxTreeCtrl>::mouseReleaseEvent(event);
    }

    wxTextCtrl *GetEditControl()
    {
        return m_item_delegate.GetEditControl();
    }

    void SetItemImage(QTreeWidgetItem *item, int image, wxTreeItemIcon which)
    {
        m_imageStates[item][which] = image;
    }

    int GetItemImage(QTreeWidgetItem *item, wxTreeItemIcon which)
    {
        if ( m_imageStates.find(item) == m_imageStates.end() )
            return 0;

        return m_imageStates[item][which];
    }

    void SetItemState(QTreeWidgetItem *item, int state)
    {
        m_imageStates[item].SetState(state);
    }
        
    int GetItemState(QTreeWidgetItem *item) const
    {
        const ImageStateMap::const_iterator i = m_imageStates.find(item);
        if ( i == m_imageStates.end() )
            return wxTREE_ITEMSTATE_NONE;

        return i->second.GetState();
    }

    void ResizeIcons(const QSize &size)
    {
        m_placeHolderImage = QPixmap(size);
        m_placeHolderImage.fill(Qt::transparent);
        ReplaceIcons(invisibleRootItem());
    }

    QPixmap GetPlaceHolderImage() const
    {
        return m_placeHolderImage;
    }

    void select(QTreeWidgetItem* item, QItemSelectionModel::SelectionFlag selectionFlag)
    {
        const QModelIndex &index = indexFromItem(item);
        selectionModel()->select(index, selectionFlag);
    }

protected:
    virtual void drawRow(
        QPainter *painter,
        const QStyleOptionViewItem &options,
        const QModelIndex &index

    ) const override
    {
        QTreeWidget::drawRow(painter, options, index);

        QTreeWidgetItem *item = itemFromIndex(index);
        const int imageIndex = ChooseBestImage(item);

        if ( imageIndex != -1 )
        {
            const wxImageList *imageList = GetHandler()->GetImageList();
            const wxBitmap bitmap = imageList->GetBitmap(imageIndex);
            const QRect rect = visualRect(index);
            const int offset = (rect.height() / 2) - (bitmap.GetHeight() / 2);
            painter->drawPixmap(rect.topLeft() + QPoint(0,offset), *bitmap.GetHandle());
        }
    }

    bool edit(const QModelIndex &index, EditTrigger trigger, QEvent *event) override
    {
        // AllEditTriggers means that editor is about to open, not waiting for double click
        if (trigger == AllEditTriggers)
        {
            // Allow event handlers to veto opening the editor
            wxTreeEvent wx_event(
                wxEVT_TREE_BEGIN_LABEL_EDIT,
                GetHandler(),
                wxQtConvertTreeItem(itemFromIndex(index))
                );
            if (GetHandler()->HandleWindowEvent(wx_event) && !wx_event.IsAllowed())
                return false;
        }
        return QTreeWidget::edit(index, trigger, event);
    }

    void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint) override
    {
        // Close process can re-signal closeEditor so we need to guard against
        // reentrant calls.
        wxRecursionGuard guard(m_closing_editor);

        if (guard.IsInside())
            return;

        // There can be multiple calls to close editor when the item loses focus
        const QModelIndex current_index = m_item_delegate.GetCurrentModelIndex();
        if (!current_index.isValid())
            return;

        wxTreeEvent event(
            wxEVT_TREE_END_LABEL_EDIT,
            GetHandler(),
            wxQtConvertTreeItem(itemFromIndex(current_index))
            );
        if (hint == QAbstractItemDelegate::RevertModelCache)
        {
            event.SetEditCanceled(true);
            EmitEvent(event);
        }
        else
        {
            // Allow event handlers to decide whether to accept edited text
            const wxString editor_text = m_item_delegate.GetEditControl()->GetLineText(0);
            event.SetLabel(editor_text);
            if (!GetHandler()->HandleWindowEvent(event) || event.IsAllowed())
                m_item_delegate.AcceptModelData(editor, model(), current_index);
        }
        // wx doesn't have hints to edit next/previous item
        if (hint == QAbstractItemDelegate::EditNextItem || hint == QAbstractItemDelegate::EditPreviousItem)
            hint = QAbstractItemDelegate::SubmitModelCache;

        QTreeWidget::closeEditor(editor, hint);
    }

private:
    void ReplaceIcons(QTreeWidgetItem *item)
    {
        item->setIcon(0, m_placeHolderImage);
        const int childCount = item->childCount();
        for ( int i = 0; i < childCount; ++i )
        {
            ReplaceIcons(item->child(i));
        }
    }

    void OnCurrentItemChanged(
        QTreeWidgetItem *current,
        QTreeWidgetItem *previous
    )
    {
        wxTreeCtrl* treeCtrl = GetHandler();

        wxTreeEvent changingEvent(
            wxEVT_TREE_SEL_CHANGING,
            treeCtrl,
            wxQtConvertTreeItem(current)
        );

        changingEvent.SetOldItem(wxQtConvertTreeItem(previous));
        EmitEvent(changingEvent);

        if ( !changingEvent.IsAllowed() )
        {
            wxQtEnsureSignalsBlocked blocker(this);
            setCurrentItem(previous);
            return;
        }

        // QT doesn't update the selection until this signal has been
        // processed. Deferring this event ensures that
        // wxTreeCtrl::GetSelection returns the new selection in the
        // wx event handler.
        wxTreeEvent changedEvent(
            wxEVT_TREE_SEL_CHANGED,
            treeCtrl,
            wxQtConvertTreeItem(current)
        );
        changedEvent.SetOldItem(wxQtConvertTreeItem(previous));
        wxPostEvent(treeCtrl, changedEvent);
    }

    void OnItemActivated(QTreeWidgetItem *item, int WXUNUSED(column))
    {
        wxTreeEvent event(
            wxEVT_TREE_ITEM_ACTIVATED,
            GetHandler(),
            wxQtConvertTreeItem(item)
        );

        EmitEvent(event);
    }

    void OnItemCollapsed(QTreeWidgetItem *item)
    {
        wxTreeEvent collapsingEvent(
            wxEVT_TREE_ITEM_COLLAPSING,
            GetHandler(),
            wxQtConvertTreeItem(item)
        );
        EmitEvent(collapsingEvent);

        if ( !collapsingEvent.IsAllowed() )
        {
            wxQtEnsureSignalsBlocked blocker(this);
            item->setExpanded(true);
            return;
        }

        wxTreeEvent collapsedEvent(
            wxEVT_TREE_ITEM_COLLAPSED,
            GetHandler(),
            wxQtConvertTreeItem(item)
        );
        EmitEvent(collapsedEvent);
    }

    void OnItemExpanded(QTreeWidgetItem *item)
    {
        wxTreeEvent expandingEvent(
            wxEVT_TREE_ITEM_EXPANDING,
            GetHandler(),
            wxQtConvertTreeItem(item)
        );
        EmitEvent(expandingEvent);

        if ( !expandingEvent.IsAllowed() )
        {
            wxQtEnsureSignalsBlocked blocker(this);
            item->setExpanded(false);
            return;
        }

        wxTreeEvent expandedEvent(
            wxEVT_TREE_ITEM_EXPANDED,
            GetHandler(),
            wxQtConvertTreeItem(item)
        );
        EmitEvent(expandedEvent);
    }

    void OnTreeScrolled(int)
    {
        if ( GetEditControl() != nullptr )
            closeEditor(GetEditControl()->GetHandle(), QAbstractItemDelegate::RevertModelCache);
    }

    void tryStartDrag(const QMouseEvent *event)
    {
        wxEventType command = event->buttons() & Qt::RightButton
            ? wxEVT_TREE_BEGIN_RDRAG
            : wxEVT_TREE_BEGIN_DRAG;

        QTreeWidgetItem *hitItem = itemAt(event->pos());

        wxTreeEvent tree_event(
            command,
            GetHandler(),
            wxQtConvertTreeItem(hitItem)
        );

        tree_event.SetPoint(wxQtConvertPoint(event->pos()));

        // Client must explicitly accept drag and drop. Vetoed by default.
        tree_event.Veto();

        EmitEvent(tree_event);

        if ( !tree_event.IsAllowed() )
        {
            setState(NoState);
        }
    }

    void endDrag(QPoint position)
    {
        QTreeWidgetItem *hitItem = itemAt(position);

        wxTreeEvent tree_event(
            wxEVT_TREE_END_DRAG,
            GetHandler(),
            wxQtConvertTreeItem(hitItem)
        );

        tree_event.SetPoint(wxQtConvertPoint(position));

        EmitEvent(tree_event);
    }

    virtual QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index, const QEvent *event) const override
    {
        return state() == DragSelectingState ? QItemSelectionModel::NoUpdate : QTreeWidget::selectionCommand(index, event);
    }

    virtual void dropEvent(QDropEvent* event) override
    {
        endDrag(event->pos());

        // We don't want Qt to actually do the drop.
        event->ignore();
    }

    virtual void mouseMoveEvent(QMouseEvent *event) override
    {
        const bool wasDragging = state() == DraggingState;
        wxQtEventSignalHandler<QTreeWidget, wxTreeCtrl>::mouseMoveEvent(event);

        const bool nowDragging = state() == DraggingState;
        if ( !wasDragging && nowDragging )
        {
            tryStartDrag(event);
        }
    }

    int ChooseBestImage(QTreeWidgetItem *item) const
    {
        int imageIndex = -1;

        const ImageStateMap::const_iterator i = m_imageStates.find(item);

        if ( i == m_imageStates.end() )
        {
            return -1;
        }

        const ImageState &states = i->second;

        if ( item->isExpanded() )
        {
            if ( item->isSelected() )
                imageIndex = states[wxTreeItemIcon_SelectedExpanded];

            if (imageIndex == -1)
                imageIndex = states[wxTreeItemIcon_Expanded];
        }
        else
        {
            if ( item->isSelected() )
                imageIndex = states[wxTreeItemIcon_Selected];
        }

        if ( imageIndex == -1 )
            imageIndex = states[wxTreeItemIcon_Normal];

        return imageIndex;
    }

    wxQTTreeItemDelegate m_item_delegate;
    wxRecursionGuardFlag m_closing_editor;

    typedef std::map<QTreeWidgetItem*,ImageState> ImageStateMap;
    ImageStateMap m_imageStates;

    // Place holder image to reserve enough space in a row 
    // for us to draw our icon
    QPixmap m_placeHolderImage;
};

wxTreeCtrl::wxTreeCtrl() :
    m_qtTreeWidget(nullptr)
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
    m_qtWindow =
    m_qtTreeWidget = new wxQTreeWidget(parent, this);
    m_qtTreeWidget->header()->hide();

    Bind(wxEVT_KEY_DOWN, &wxTreeCtrl::OnKeyDown, this);

    if ( !wxTreeCtrlBase::Create(parent, id, pos, size, style|wxHSCROLL|wxVSCROLL, validator, name) )
    {
        return false;
    }

    SetWindowStyleFlag(style);

    return true;
}

wxTreeCtrl::~wxTreeCtrl()
{
    if ( m_qtTreeWidget != nullptr )
        m_qtTreeWidget->deleteLater();
}

unsigned wxTreeCtrl::GetCount() const
{
    QTreeWidgetItem *root = m_qtTreeWidget->invisibleRootItem();
    if ( root->childCount() == 0 )
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
    wxWithImages::SetImageList(imageList);

    DoUpdateIconsSize(imageList);
}

void wxTreeCtrl::DoUpdateIconsSize(wxImageList *imageList)
{
    int width, height;
    imageList->GetSize(0, width, height);
    m_qtTreeWidget->ResizeIcons(QSize(width, height));
    m_qtTreeWidget->update();
}

void wxTreeCtrl::OnImagesChanged()
{
    if ( HasImages() )
    {
        DoUpdateIconsSize(GetUpdatedImageListFor(this));
    }
}

void wxTreeCtrl::SetStateImages(const wxVector<wxBitmapBundle>& images)
{
    m_imagesState.SetImages(images);
    m_qtTreeWidget->update();
}

void wxTreeCtrl::SetStateImageList(wxImageList *imageList)
{
    m_imagesState.SetImageList(imageList);
    m_qtTreeWidget->update();
}

wxString wxTreeCtrl::GetItemText(const wxTreeItemId& item) const
{
    if ( !item.IsOk() )
        return wxString();

    const QTreeWidgetItem* qTreeItem = wxQtConvertTreeItem(item);
    return wxQtConvertString(qTreeItem->text(0));
}

int wxTreeCtrl::GetItemImage(
    const wxTreeItemId& item,
    wxTreeItemIcon which
) const
{
    wxCHECK_MSG(item.IsOk(), -1, "invalid tree item");
    return m_qtTreeWidget->GetItemImage(wxQtConvertTreeItem(item), which);
}

wxTreeItemData *wxTreeCtrl::GetItemData(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), nullptr, "invalid tree item");

    const QTreeWidgetItem* qTreeItem = wxQtConvertTreeItem(item);
    const QVariant itemData = qTreeItem->data(0, Qt::UserRole);
    const TreeItemDataQt value = itemData.value<TreeItemDataQt>();
    return value.getData();
}

wxColour wxTreeCtrl::GetItemTextColour(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxNullColour, "invalid tree item");

    const QTreeWidgetItem* qTreeItem = wxQtConvertTreeItem(item);
    return wxQtConvertColour(qTreeItem->foreground(0).color());
}

wxColour wxTreeCtrl::GetItemBackgroundColour(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxNullColour, "invalid tree item");

    const QTreeWidgetItem* qTreeItem = wxQtConvertTreeItem(item);
    return wxQtConvertColour(qTreeItem->background(0).color());
}

wxFont wxTreeCtrl::GetItemFont(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxNullFont, "invalid tree item");

    const QTreeWidgetItem* qTreeItem = wxQtConvertTreeItem(item);
    return wxFont(qTreeItem->font(0));
}

void wxTreeCtrl::SetItemText(const wxTreeItemId& item, const wxString& text)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem* qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setText(0, wxQtConvertString(text));
}

void wxTreeCtrl::SetItemImage(
    const wxTreeItemId& item,
    int image,
    wxTreeItemIcon which
)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    m_qtTreeWidget->SetItemImage(wxQtConvertTreeItem(item), image, which);
}

void wxTreeCtrl::SetItemData(const wxTreeItemId& item, wxTreeItemData *data)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    if ( data != nullptr )
        data->SetId(item);

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    const TreeItemDataQt treeItemData(data);
    qTreeItem->setData(0, Qt::UserRole, QVariant::fromValue(treeItemData));
}

void wxTreeCtrl::SetItemHasChildren(const wxTreeItemId& item, bool has)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setChildIndicatorPolicy(has
        ? QTreeWidgetItem::ShowIndicator
        : QTreeWidgetItem::DontShowIndicatorWhenChildless);
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

    if ( highlight )
    {
        bg = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
        fg = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    }
    else
    {
        bg = GetBackgroundColour();
        fg = GetForegroundColour();
    }

    qTreeItem->setBackground(0, wxQtConvertColour(bg));
    qTreeItem->setForeground(0, wxQtConvertColour(fg));
}

void wxTreeCtrl::SetItemTextColour(
    const wxTreeItemId& item,
    const wxColour& col
)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setForeground(0, wxQtConvertColour(col));
}

void wxTreeCtrl::SetItemBackgroundColour(
    const wxTreeItemId& item,
    const wxColour& col
)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setBackground(0, wxQtConvertColour(col));
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

    const QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    const QRect itemRect = m_qtTreeWidget->visualItemRect(qTreeItem);
    const QRect clientRect = m_qtTreeWidget->rect();
    return itemRect.isValid() && clientRect.contains(itemRect);
}

bool wxTreeCtrl::ItemHasChildren(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    const QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    return qTreeItem->childCount() > 0;
}

bool wxTreeCtrl::IsExpanded(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    const QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    return qTreeItem->isExpanded();
}

bool wxTreeCtrl::IsSelected(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    const QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    return qTreeItem->isSelected();
}

bool wxTreeCtrl::IsBold(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    const QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    const QFont font = qTreeItem->font(0);
    return font.bold();
}

size_t wxTreeCtrl::GetChildrenCount(
    const wxTreeItemId& item,
    bool recursively
) const
{
    wxCHECK_MSG(item.IsOk(), 0, "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);

    if ( recursively )
        return CountChildren(qTreeItem);

    return qTreeItem->childCount();
}

wxTreeItemId wxTreeCtrl::GetRootItem() const
{
    const QTreeWidgetItem *root = m_qtTreeWidget->invisibleRootItem();
    return wxQtConvertTreeItem(root->child(0));
}

wxTreeItemId wxTreeCtrl::GetSelection() const
{
    QList<QTreeWidgetItem*> selections = m_qtTreeWidget->selectedItems();
    return selections.isEmpty()
        ? wxTreeItemId()
        : wxQtConvertTreeItem(selections[0]);
}

size_t wxTreeCtrl::GetSelections(wxArrayTreeItemIds& selections) const
{
    QList<QTreeWidgetItem*> qtSelections = m_qtTreeWidget->selectedItems();

    const size_t numberOfSelections = qtSelections.size();
    selections.reserve(numberOfSelections);
    for ( size_t i = 0; i < numberOfSelections; ++i )
    {
        QTreeWidgetItem *item = qtSelections[i];
        selections.push_back(wxQtConvertTreeItem(item));
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
    m_qtTreeWidget->setCurrentItem(nullptr);
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

wxTreeItemId wxTreeCtrl::GetFirstChild(
    const wxTreeItemId& item,
    wxTreeItemIdValue& cookie
) const
{
    cookie = nullptr;
    return GetNextChild(item, cookie);
}

wxTreeItemId wxTreeCtrl::GetNextChild(
    const wxTreeItemId& item,
    wxTreeItemIdValue& cookie
) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    wxIntPtr currentIndex = reinterpret_cast<wxIntPtr>(cookie);

    const QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);

    wxTreeItemId childItem;
    if ( currentIndex < qTreeItem->childCount() )
    {
        childItem = wxQtConvertTreeItem(qTreeItem->child(currentIndex++));
        cookie = reinterpret_cast<wxTreeItemIdValue>(currentIndex);
    }

    return childItem;
}

wxTreeItemId wxTreeCtrl::GetLastChild(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    const QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    const int childCount = qTreeItem->childCount();
    return childCount == 0 
        ? wxTreeItemId()
        : wxQtConvertTreeItem(qTreeItem->child(childCount - 1));
}

wxTreeItemId wxTreeCtrl::GetNextSibling(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    QTreeWidgetItem *parent = qTreeItem->parent();

    if ( parent != nullptr )
    {
        int index = parent->indexOfChild(qTreeItem);
        wxASSERT(index != -1);

        ++index;
        return index < parent->childCount()
            ? wxQtConvertTreeItem(parent->child(index))
            : wxTreeItemId();
    }

    int index = m_qtTreeWidget->indexOfTopLevelItem(qTreeItem);
    wxASSERT(index != -1);

    ++index;
    return index < m_qtTreeWidget->topLevelItemCount()
        ? wxQtConvertTreeItem(m_qtTreeWidget->topLevelItem(index)) 
        : wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetPrevSibling(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    QTreeWidgetItem *parent = qTreeItem->parent();

    if ( parent != nullptr )
    {
        int index = parent->indexOfChild(qTreeItem);
        wxASSERT(index != -1);

        --index;
        return index >= 0
            ? wxQtConvertTreeItem(parent->child(index))
            : wxTreeItemId();
    }

    int index = m_qtTreeWidget->indexOfTopLevelItem(qTreeItem);
    wxASSERT(index != -1);

    --index;
    return index >= 0
        ? wxQtConvertTreeItem(m_qtTreeWidget->topLevelItem(index))
        : wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetFirstVisibleItem() const
{
   wxTreeItemId itemid = GetRootItem();
    if ( !itemid.IsOk() )
        return itemid;

    do
    {
        if ( IsVisible(itemid) )
            return itemid;
        itemid = GetNext(itemid);
    } while ( itemid.IsOk() );

    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetNextVisible(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");
    wxASSERT_MSG(IsVisible(item), "this item itself should be visible");

    wxTreeItemId id = item;
    if ( id.IsOk() )
    {
        while ( id = GetNext(id), id.IsOk() )
        {
            if ( IsVisible(id) )
                return id;
        }
    }
    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetPrevVisible(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");
    wxASSERT_MSG(IsVisible(item), "this item itself should be visible");

    // find out the starting point
    wxTreeItemId prevItem = GetPrevSibling(item);
    if ( !prevItem.IsOk() )
    {
        prevItem = GetItemParent(item);
    }

    // find the first visible item after it
    while ( prevItem.IsOk() && !IsVisible(prevItem) )
    {
        prevItem = GetNext(prevItem);
        if ( !prevItem.IsOk() || prevItem == item )
        {
            // there are no visible items before item
            return wxTreeItemId();
        }
    }

    // from there we must be able to navigate until this item
    while ( prevItem.IsOk() )
    {
        const wxTreeItemId nextItem = GetNextVisible(prevItem);
        if ( !nextItem.IsOk() || nextItem == item )
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
    wxTreeItemId newItem = DoInsertItem(
        wxQtConvertTreeItem(root),
        0,
        text,
        image,
        selImage,
        data
    );

    m_qtTreeWidget->setCurrentItem(nullptr);

    if ( (GetWindowStyleFlag() & wxTR_HIDE_ROOT) != 0 )
        m_qtTreeWidget->setRootIndex(m_qtTreeWidget->model()->index(0, 0));
    else
        m_qtTreeWidget->setRootIndex(QModelIndex());

    return newItem;
}

void wxTreeCtrl::Delete(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    QTreeWidgetItem *parent = qTreeItem->parent();

    DeleteChildren(qTreeItem);

    if ( parent != nullptr )
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
    wxQtEnsureSignalsBlocked ensureSignalsBlock(m_qtTreeWidget);
    while ( qTreeItem->childCount() > 0 )
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
    qTreeItem->setExpanded(!qTreeItem->isExpanded());
}

void wxTreeCtrl::Unselect()
{
    QTreeWidgetItem *current = m_qtTreeWidget->currentItem();
    if ( current != nullptr )
        m_qtTreeWidget->select(current, QItemSelectionModel::Deselect);
}

void wxTreeCtrl::UnselectAll()
{
    m_qtTreeWidget->selectionModel()->clearSelection();
}

void wxTreeCtrl::SelectItem(const wxTreeItemId& item, bool select)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    if ( !HasFlag(wxTR_MULTIPLE) )
    {
        m_qtTreeWidget->clearSelection();
    }

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);

    if ( qTreeItem )
    {
        m_qtTreeWidget->select(qTreeItem, select ? QItemSelectionModel::Select : QItemSelectionModel::Deselect);
        if ( select && m_qtTreeWidget->selectionMode() == QTreeWidget::SingleSelection )
        {
            m_qtTreeWidget->setCurrentItem(qTreeItem);
        }
    }
}

void wxTreeCtrl::SelectChildren(const wxTreeItemId& parent)
{
    wxCHECK_RET(parent.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(parent);
    const int childCount = qTreeItem->childCount();

    for ( int i = 0; i < childCount; ++i )
    {
        m_qtTreeWidget->select(qTreeItem->child(i), QItemSelectionModel::Select);
    }
}

void wxTreeCtrl::EnsureVisible(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    QTreeWidgetItem *parent = qTreeItem->parent();

    while ( parent != nullptr )
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

wxTextCtrl *wxTreeCtrl::EditLabel(
    const wxTreeItemId& item,
    wxClassInfo* WXUNUSED(textCtrlClass)
)
{
    wxCHECK_MSG(item.IsOk(), nullptr, "invalid tree item");
    m_qtTreeWidget->editItem(wxQtConvertTreeItem(item));
    return m_qtTreeWidget->GetEditControl();
}

wxTextCtrl *wxTreeCtrl::GetEditControl() const
{
    return m_qtTreeWidget->GetEditControl();
}

void wxTreeCtrl::EndEditLabel(
    const wxTreeItemId& item,
    bool WXUNUSED(discardChanges)
)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");
    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    m_qtTreeWidget->closePersistentEditor(qTreeItem);
}

void wxTreeCtrl::SortChildren(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->sortChildren(0, Qt::AscendingOrder);
}

bool wxTreeCtrl::GetBoundingRect(
    const wxTreeItemId& item,
    wxRect& rect,
    bool WXUNUSED(textOnly)
) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    const QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    const QRect visualRect = m_qtTreeWidget->visualItemRect(qTreeItem);
    if ( !visualRect.isValid() )
        return false;

    rect = wxQtConvertRect(visualRect);
    return true;
}

void wxTreeCtrl::SetWindowStyleFlag(long styles)
{
    wxControl::SetWindowStyleFlag(styles);

    m_qtTreeWidget->setSelectionMode(
        styles & wxTR_MULTIPLE
            ? QTreeWidget::ExtendedSelection
            : QTreeWidget::SingleSelection
    );
}

int wxTreeCtrl::DoGetItemState(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTREE_ITEMSTATE_NONE, "invalid tree item");
    return m_qtTreeWidget->GetItemState(wxQtConvertTreeItem(item));
}

void wxTreeCtrl::DoSetItemState(const wxTreeItemId& item, int state)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");
    m_qtTreeWidget->SetItemState(wxQtConvertTreeItem(item), state);
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
    newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);

    TreeItemDataQt treeItemData(data);
    newItem->setData(0, Qt::UserRole, QVariant::fromValue(treeItemData));

    m_qtTreeWidget->SetItemImage(newItem, image, wxTreeItemIcon_Normal);
    m_qtTreeWidget->SetItemImage(newItem, selImage, wxTreeItemIcon_Selected);

    newItem->setIcon(0, m_qtTreeWidget->GetPlaceHolderImage());

    wxTreeItemId wxItem = wxQtConvertTreeItem(newItem);

    if ( data != nullptr )
        data->SetId(wxItem);

    if ( pos == static_cast<size_t>(-1) )
    {
        qTreeItem->addChild(newItem);
    }
    else
    {
        qTreeItem->insertChild(pos, newItem);
    }

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

    const QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(parent);
    const int index = qTreeItem->indexOfChild(wxQtConvertTreeItem(idPrevious));
    return DoInsertItem(parent, index + 1, text, image, selImage, data);
}

wxTreeItemId wxTreeCtrl::DoTreeHitTest(const wxPoint& point, int& flags) const
{
    int w, h;
    GetSize(&w, &h);
    flags = 0;
    if ( point.x < 0 )
        flags |= wxTREE_HITTEST_TOLEFT;
    else if ( point.x > w )
        flags |= wxTREE_HITTEST_TORIGHT;

    if ( point.y < 0 )
        flags |= wxTREE_HITTEST_ABOVE;
    else if ( point.y > h )
        flags |= wxTREE_HITTEST_BELOW;

    if ( flags != 0 )
        return wxTreeItemId();

    QTreeWidgetItem *hitItem = m_qtTreeWidget->itemAt(wxQtConvertPoint(point));
    flags = hitItem == nullptr ? wxTREE_HITTEST_NOWHERE : wxTREE_HITTEST_ONITEM;
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
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);

    if ( qTreeItem->childCount() > 0 )
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
    } while ( p.IsOk() && !toFind.IsOk() );
    return toFind;
}

void wxTreeCtrl::OnKeyDown(wxKeyEvent& event)
{
    // send a tree event
    wxTreeEvent te( wxEVT_TREE_KEY_DOWN, this);
    te.m_evtKey = event;
    if ( GetEventHandler()->ProcessEvent( te ) )
        return;

    event.Skip();
}

#endif // wxUSE_TREECTRL

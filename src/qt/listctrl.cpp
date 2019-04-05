/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/listctrl.cpp
// Author:      Mariano Reingart, Peter Most
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QItemDelegate>
#include <QtWidgets/QItemEditorFactory>

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
#endif // WX_PRECOMP

#include "wx/listctrl.h"
#include "wx/imaglist.h"
#include "wx/recguard.h"

#include "wx/qt/private/winevent.h"

namespace
{

// wxQT Doesn't have a mechanism for "adopting" external widgets so we have to
// create an instance of wxTextCtrl rather than adopting the control QT would
// create.
//
// Unfortunately the factory is given an internal widget as the parent for
// editor.
//
// To work around these issues we create a wxTextCtl parented by the wxListCtrl
// then recalculate its position relative to the internal widget.
class wxQtListTextCtrl : public wxTextCtrl
{
public:
    wxQtListTextCtrl(wxWindow* parent, QWidget* actualParent)
        : wxTextCtrl(parent, wxID_ANY, wxEmptyString,
                     wxDefaultPosition, wxDefaultSize,
                     wxNO_BORDER),
        m_actualParent(actualParent),
        m_moving(0)
    {

        Bind(wxEVT_MOVE, &wxQtListTextCtrl::onMove, this);
    }

    void onMove(wxMoveEvent &event)
    {
        // QWidget::move generates a QMoveEvent so we need to guard against
        // reentrant calls.
        wxRecursionGuard guard(m_moving);
        if ( guard.IsInside() )
        {
            event.Skip();
            return;
        }

        const QPoint eventPosition = wxQtConvertPoint(event.GetPosition());
        const QPoint globalPosition  = m_actualParent->mapToGlobal(eventPosition);

        // For some reason this always gives us the offset from the header info
        // the internal control. So we need to treat this as an offset rather
        // than a position.
        QWidget* widget = GetHandle();
        const QPoint offset = widget->mapFromGlobal(globalPosition);

        widget->move(eventPosition + offset);
    }

private:
    QWidget* m_actualParent;
    wxRecursionGuardFlag m_moving;

    wxDECLARE_NO_COPY_CLASS(wxQtListTextCtrl);
};

} // anonymous namespace


// QT doesn't give us direct access to the editor within the QTreeWidget.
// Instead, we'll supply a factory to create the widget for QT and keep track
// of it ourselves.
class wxQtItemEditorFactory : public QItemEditorFactory
{
public:
    explicit wxQtItemEditorFactory(wxWindow* parent)
        : m_parent(parent),
          m_textCtrl(NULL)
    {
    }

    QWidget* createEditor(int WXUNUSED(userType), QWidget* parent) const wxOVERRIDE
    {
        m_textCtrl = new wxQtListTextCtrl(m_parent, parent);
        m_textCtrl->SetFocus();
        return m_textCtrl->GetHandle();
    }

    wxTextCtrl* GetEditControl()
    {
        return m_textCtrl;
    }

    void ClearEditor()
    {
        delete m_textCtrl;
        m_textCtrl = NULL;
    }

private:
    wxWindow* m_parent;
    mutable wxTextCtrl* m_textCtrl;

    wxDECLARE_NO_COPY_CLASS(wxQtItemEditorFactory);
};

class wxQtTreeWidget : public wxQtEventSignalHandler< QTreeWidget, wxListCtrl >
{
public:
    wxQtTreeWidget( wxWindow *parent, wxListCtrl *handler );

    void EmitListEvent(wxEventType typ, QTreeWidgetItem *qitem, int column) const;

    void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint) wxOVERRIDE
    {
        QTreeWidget::closeEditor(editor, hint);
        m_editorFactory.ClearEditor();
    }

    wxTextCtrl *GetEditControl()
    {
        return m_editorFactory.GetEditControl();
    }

private:
    void itemClicked(QTreeWidgetItem * item, int column);
    void itemActivated(QTreeWidgetItem * item, int column);
    void itemPressed(QTreeWidgetItem * item, int column);

    void ChangeEditorFactory()
    {
        QItemDelegate *qItemDelegate = static_cast<QItemDelegate*>(itemDelegate());
        qItemDelegate->itemEditorFactory();
        qItemDelegate->setItemEditorFactory(&m_editorFactory);
    }

    wxQtItemEditorFactory m_editorFactory;
};

wxQtTreeWidget::wxQtTreeWidget( wxWindow *parent, wxListCtrl *handler )
    : wxQtEventSignalHandler< QTreeWidget, wxListCtrl >( parent, handler ),
      m_editorFactory(handler)
{
    connect(this, &QTreeWidget::itemClicked, this, &wxQtTreeWidget::itemClicked);
    connect(this, &QTreeWidget::itemPressed, this, &wxQtTreeWidget::itemPressed);
    connect(this, &QTreeWidget::itemActivated, this, &wxQtTreeWidget::itemActivated);

    ChangeEditorFactory();
}

void wxQtTreeWidget::EmitListEvent(wxEventType typ, QTreeWidgetItem *qitem, int column) const
{
    wxListCtrl *handler = GetHandler();
    if ( handler )
    {
        // prepare the event
        // -----------------
        wxListEvent event;
        event.SetEventType(typ);
        event.SetId(handler->GetId());
        event.m_itemIndex = this->indexFromItem(qitem, column).row();
        event.m_item.SetId(event.m_itemIndex);
        event.m_item.SetMask(wxLIST_MASK_TEXT |
                             wxLIST_MASK_IMAGE |
                             wxLIST_MASK_DATA);
        handler->GetItem(event.m_item);
        EmitEvent(event);
    }
}

void wxQtTreeWidget::itemClicked(QTreeWidgetItem *qitem, int column)
{
    EmitListEvent(wxEVT_LIST_ITEM_SELECTED, qitem, column);
}

void wxQtTreeWidget::itemPressed(QTreeWidgetItem *qitem, int column)
{
    EmitListEvent(wxEVT_LIST_ITEM_SELECTED, qitem, column);
}

void wxQtTreeWidget::itemActivated(QTreeWidgetItem *qitem, int column)
{
    EmitListEvent(wxEVT_LIST_ITEM_ACTIVATED, qitem, column);
}


Qt::AlignmentFlag wxQtConvertTextAlign(wxListColumnFormat align)
{
    switch (align)
    {
        case wxLIST_FORMAT_LEFT:
            return Qt::AlignLeft;
        case wxLIST_FORMAT_RIGHT:
            return Qt::AlignRight;
        case wxLIST_FORMAT_CENTRE:
            return Qt::AlignCenter;
    }
    return Qt::AlignLeft;
}

wxListColumnFormat wxQtConvertAlignFlag(int align)
{
    switch (align)
    {
        case Qt::AlignLeft:
            return wxLIST_FORMAT_LEFT;
        case Qt::AlignRight:
            return wxLIST_FORMAT_RIGHT;
        case Qt::AlignCenter:
            return wxLIST_FORMAT_CENTRE;
    }
    return wxLIST_FORMAT_LEFT;
}


wxListCtrl::wxListCtrl()
{
    Init();
}

wxListCtrl::wxListCtrl(wxWindow *parent,
           wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxValidator& validator,
           const wxString& name)
{
    Init();
    Create( parent, id, pos, size, style, validator, name );
}


bool wxListCtrl::Create(wxWindow *parent,
            wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    m_qtTreeWidget = new wxQtTreeWidget( parent, this );

    if (style & wxLC_NO_HEADER)
        m_qtTreeWidget->setHeaderHidden(true);

    m_qtTreeWidget->setRootIsDecorated(false);

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

void wxListCtrl::Init()
{
    m_imageListNormal = NULL;
    m_ownsImageListNormal = false;
    m_imageListSmall = NULL;
    m_ownsImageListSmall = false;
    m_imageListState = NULL;
    m_ownsImageListState = false;
    m_qtTreeWidget = NULL;
}

wxListCtrl::~wxListCtrl()
{

    if (m_ownsImageListNormal)
        delete m_imageListNormal;
    if (m_ownsImageListSmall)
        delete m_imageListSmall;
    if (m_ownsImageListState)
        delete m_imageListState;
}

bool wxListCtrl::SetForegroundColour(const wxColour& col)
{
    return wxListCtrlBase::SetForegroundColour(col);
}

bool wxListCtrl::SetBackgroundColour(const wxColour& col)
{
    return wxListCtrlBase::SetBackgroundColour(col);
}

bool wxListCtrl::GetColumn(int col, wxListItem& info) const
{
    QTreeWidgetItem *qitem = m_qtTreeWidget->headerItem();
    if ( qitem != NULL )
    {
        info.SetText(wxQtConvertString(qitem->text(col)));
        info.SetAlign(wxQtConvertAlignFlag(qitem->textAlignment(col)));
        info.SetWidth(m_qtTreeWidget->columnWidth(col));
        return true;
    }
    else
        return false;
}

bool wxListCtrl::SetColumn(int col, const wxListItem& info)
{
    DoInsertColumn(col, info);
    return true;
}

int wxListCtrl::GetColumnWidth(int col) const
{
    return m_qtTreeWidget->columnWidth(col);
}

bool wxListCtrl::SetColumnWidth(int col, int width)
{
    m_qtTreeWidget->setColumnWidth(col, width);
    return true;
}

int wxListCtrl::GetColumnOrder(int col) const
{
    return col;
}

int wxListCtrl::GetColumnIndexFromOrder(int order) const
{
    return order;
}

wxArrayInt wxListCtrl::GetColumnsOrder() const
{
    return wxArrayInt();
}

bool wxListCtrl::SetColumnsOrder(const wxArrayInt& WXUNUSED(orders))
{
    return false;
}

int wxListCtrl::GetCountPerPage() const
{
    // this may not be exact but should be a good aproximation:
    int h = m_qtTreeWidget->visualItemRect(m_qtTreeWidget->headerItem()).height();
    if ( h )
        return m_qtTreeWidget->height() / h;
    else
        return 0;
}

wxRect wxListCtrl::GetViewRect() const
{
    // this may not be exact but should be a good aproximation:
    wxRect rect = wxQtConvertRect(m_qtTreeWidget->rect());
    int h = m_qtTreeWidget->header()->defaultSectionSize();
    rect.SetTop(h);
    rect.SetHeight(rect.GetHeight() - h);
    return rect;
}

wxTextCtrl* wxListCtrl::GetEditControl() const
{
    return m_qtTreeWidget->GetEditControl();
}

QTreeWidgetItem *wxListCtrl::QtGetItem(int id) const
{
    wxCHECK_MSG( id >= 0 && id < GetItemCount(), NULL,
                 wxT("invalid item index in wxListCtrl") );
    QModelIndex index = m_qtTreeWidget->model()->index(id, 0);
    // note that itemFromIndex(index) is protected
    return (QTreeWidgetItem*)index.internalPointer();
}

bool wxListCtrl::GetItem(wxListItem& info) const
{
    const long id = info.GetId();
    QTreeWidgetItem *qitem = QtGetItem(id);
    if ( qitem != NULL )
    {
        if ( !info.m_mask )
            // by default, get everything for backwards compatibility
            info.m_mask = -1;
        if ( info.m_mask & wxLIST_MASK_TEXT )
            info.SetText(wxQtConvertString(qitem->text(info.GetColumn())));
        if ( info.m_mask & wxLIST_MASK_DATA )
        {
            QVariant variant = qitem->data(0, Qt::UserRole);
            info.SetData(variant.value<long>());
        }
        if ( info.m_mask & wxLIST_MASK_STATE )
        {
            info.m_state = wxLIST_STATE_DONTCARE;
            if ( info.m_stateMask & wxLIST_STATE_FOCUSED )
            {
                if ( m_qtTreeWidget->currentIndex().row() == id )
                    info.m_state |= wxLIST_STATE_FOCUSED;
            }
            if ( info.m_stateMask & wxLIST_STATE_SELECTED )
            {
                if ( qitem->isSelected() )
                    info.m_state |= wxLIST_STATE_SELECTED;
            }
        }
        return true;
    }
    else
        return false;
}

bool wxListCtrl::SetItem(wxListItem& info)
{
    const long id = info.GetId();
    if ( id < 0 )
        return false;
    QTreeWidgetItem *qitem = QtGetItem(id);
    if ( qitem != NULL )
    {
        if ((info.m_mask & wxLIST_MASK_TEXT) && !info.GetText().IsNull() )
            qitem->setText(info.GetColumn(), wxQtConvertString(info.GetText()));
        qitem->setTextAlignment(info.GetColumn(), wxQtConvertTextAlign(info.GetAlign()));

        if ( info.m_mask & wxLIST_MASK_DATA )
        {
            QVariant variant = qVariantFromValue(info.GetData());
            qitem->setData(0, Qt::UserRole, variant);
        }
        if (info.m_mask & wxLIST_MASK_STATE)
        {
            if ((info.m_stateMask & wxLIST_STATE_FOCUSED) &&
                (info.m_state & wxLIST_STATE_FOCUSED))
                    m_qtTreeWidget->setCurrentItem(qitem, 0);
            if (info.m_stateMask & wxLIST_STATE_SELECTED)
                qitem->setSelected(info.m_state & wxLIST_STATE_SELECTED);
        }
        if (info.m_mask & wxLIST_MASK_IMAGE)
        {
            if (info.m_image >= 0)
            {
                wxImageList *imglst = GetImageList(InReportView() ? wxIMAGE_LIST_SMALL : wxIMAGE_LIST_NORMAL);
                wxCHECK_MSG(imglst, false, "invalid listctrl imagelist");
                const wxBitmap bitmap = imglst->GetBitmap(info.m_image);
                // set the new image:
                qitem->setIcon( info.GetColumn(), QIcon( *bitmap.GetHandle() ));
            }
            else
            {
                // remove the image using and empty qt icon:
                qitem->setIcon( info.GetColumn(), QIcon() );
            }
        }
        for (int col=0; col<GetColumnCount(); col++)
        {
            if ( info.GetFont().IsOk() )
                qitem->setFont(col, info.GetFont().GetHandle() );
            if ( info.GetTextColour().IsOk() )
                qitem->setTextColor(col, info.GetTextColour().GetQColor());
            if ( info.GetBackgroundColour().IsOk() )
                qitem->setBackgroundColor(col, info.GetBackgroundColour().GetQColor());
        }
        return true;
    }
    else
        return false;
}

long wxListCtrl::SetItem(long index, int col, const wxString& label, int imageId)
{
    wxListItem info;
    info.m_text = label;
    info.m_mask = wxLIST_MASK_TEXT;
    info.m_itemId = index;
    info.m_col = col;
    if ( imageId > -1 )
    {
        info.m_image = imageId;
        info.m_mask |= wxLIST_MASK_IMAGE;
    }
    return SetItem(info);
}

int  wxListCtrl::GetItemState(long item, long stateMask) const
{
    wxListItem info;

    info.m_mask = wxLIST_MASK_STATE;
    info.m_stateMask = stateMask;
    info.m_itemId = item;

    if (!GetItem(info))
        return 0;

    return info.m_state;
}

bool wxListCtrl::SetItemState(long item, long state, long stateMask)
{
    wxListItem info;

    info.m_mask = wxLIST_MASK_STATE;
    info.m_stateMask = stateMask;
    info.m_state = state;
    info.m_itemId = item;

    return SetItem(info);
}

bool wxListCtrl::SetItemImage(long item, int image, int WXUNUSED(selImage))
{
    return SetItemColumnImage(item, 0, image);
}

bool wxListCtrl::SetItemColumnImage(long item, long column, int image)
{
    wxListItem info;

    info.m_mask = wxLIST_MASK_IMAGE;
    info.m_image = image;
    info.m_itemId = item;
    info.m_col = column;

    return SetItem(info);
}

wxString wxListCtrl::GetItemText(long item, int col) const
{
    QTreeWidgetItem *qitem = QtGetItem(item);
    if ( qitem )
        return wxQtConvertString( qitem->text(col) );
    else
        return wxString();
}

void wxListCtrl::SetItemText(long item, const wxString& str)
{
    QTreeWidgetItem *qitem = QtGetItem(item);
    if ( qitem )
        qitem->setText( 0, wxQtConvertString( str ) );
}

wxUIntPtr wxListCtrl::GetItemData(long item) const
{
    QTreeWidgetItem *qitem = QtGetItem(item);
    if ( qitem != NULL )
    {
        QVariant variant = qitem->data(0, Qt::UserRole);
        return variant.value<wxUIntPtr>();
    }
    else
        return 0;
}

bool wxListCtrl::SetItemPtrData(long item, wxUIntPtr data)
{
    QTreeWidgetItem *qitem = QtGetItem(item);
    if ( qitem != NULL )
    {
        QVariant variant = qVariantFromValue(data);
        qitem->setData(0, Qt::UserRole, variant);
        return true;
    }
    else
        return false;
}

bool wxListCtrl::SetItemData(long item, long data)
{
    QTreeWidgetItem *qitem = QtGetItem(item);
    if ( qitem != NULL )
    {
        QVariant variant = qVariantFromValue(data);
        qitem->setData(0, Qt::UserRole, variant);
        return true;
    }
    else
        return false;
}

bool wxListCtrl::GetItemRect(long item, wxRect& rect, int WXUNUSED(code)) const
{
    QTreeWidgetItem *qitem = QtGetItem(item);
    if ( qitem != NULL )
    {
        rect = wxQtConvertRect( m_qtTreeWidget->visualItemRect(qitem) );
        return true;
    }
    else
        return false;
}

bool wxListCtrl::GetSubItemRect(long item, long subItem, wxRect& rect, int WXUNUSED(code)) const
{
    QTreeWidgetItem *qitem = QtGetItem(item);
    if ( qitem != NULL )
    {
        wxCHECK_MSG( item >= 0 && item < GetItemCount(), NULL,
                     wxT("invalid row index in GetSubItemRect") );
        wxCHECK_MSG( subItem >= 0 && subItem < GetColumnCount(), NULL,
                     wxT("invalid column index in GetSubItemRect") );
        QModelIndex qindex = m_qtTreeWidget->model()->index(item, subItem);
        rect = wxQtConvertRect( m_qtTreeWidget->visualRect(qindex) );
        return true;
    }
    else
        return false;
}

bool wxListCtrl::GetItemPosition(long item, wxPoint& pos) const
{
    wxRect rect;
    GetItemRect(item, rect);

    pos.x = rect.x;
    pos.y = rect.y;

    return true;
}

bool wxListCtrl::SetItemPosition(long WXUNUSED(item), const wxPoint& WXUNUSED(pos))
{
    return false;
}

int wxListCtrl::GetItemCount() const
{
    return m_qtTreeWidget->topLevelItemCount();
}

int wxListCtrl::GetColumnCount() const
{
    return m_qtTreeWidget->columnCount();
}

wxSize wxListCtrl::GetItemSpacing() const
{
    return wxSize();
}

void wxListCtrl::SetItemTextColour( long WXUNUSED(item), const wxColour& WXUNUSED(col))
{
}

wxColour wxListCtrl::GetItemTextColour( long WXUNUSED(item) ) const
{
    return wxColour();
}

void wxListCtrl::SetItemBackgroundColour( long WXUNUSED(item), const wxColour &WXUNUSED(col))
{
}

wxColour wxListCtrl::GetItemBackgroundColour( long WXUNUSED(item) ) const
{
    return wxColour();
}

void wxListCtrl::SetItemFont( long WXUNUSED(item), const wxFont &WXUNUSED(f))
{
}

wxFont wxListCtrl::GetItemFont( long WXUNUSED(item) ) const
{
    return wxFont();
}

int wxListCtrl::GetSelectedItemCount() const
{
    return m_qtTreeWidget->selectedItems().length();
}

wxColour wxListCtrl::GetTextColour() const
{
    return wxColour();
}

void wxListCtrl::SetTextColour(const wxColour& WXUNUSED(col))
{
}

long wxListCtrl::GetTopItem() const
{
    return 0;
}

void wxListCtrl::SetSingleStyle(long WXUNUSED(style), bool WXUNUSED(add))
{
}

void wxListCtrl::SetWindowStyleFlag(long WXUNUSED(style))
{
}

long wxListCtrl::GetNextItem(long item, int WXUNUSED(geometry), int state) const
{
    wxListItem info;
    long ret = item,
         max = GetItemCount();
    wxCHECK_MSG( (ret == -1) || (ret < max), -1,
                 wxT("invalid listctrl index in GetNextItem()") );

    // notice that we start with the next item (or the first one if item == -1)
    // and this is intentional to allow writing a simple loop to iterate over
    // all selected items
    ret++;
    if ( ret == max )
        // this is not an error because the index was OK initially,
        // just no such item
        return -1;

    if ( !state )
        // any will do
        return (size_t)ret;

    size_t count = GetItemCount();
    for ( size_t line = (size_t)ret; line < count; line++ )
    {
        if ( GetItemState(line, state) )
            return line;
    }

    return -1;
}

wxImageList *wxListCtrl::GetImageList(int which) const
{
    if ( which == wxIMAGE_LIST_NORMAL )
    {
        return m_imageListNormal;
    }
    else if ( which == wxIMAGE_LIST_SMALL )
    {
        return m_imageListSmall;
    }
    else if ( which == wxIMAGE_LIST_STATE )
    {
        return m_imageListState;
    }
    return NULL;
}


void wxListCtrl::SetImageList(wxImageList *imageList, int which)
{
    if ( which == wxIMAGE_LIST_NORMAL )
    {
        if (m_ownsImageListNormal) delete m_imageListNormal;
        m_imageListNormal = imageList;
        m_ownsImageListNormal = false;
    }
    else if ( which == wxIMAGE_LIST_SMALL )
    {
        if (m_ownsImageListSmall) delete m_imageListSmall;
        m_imageListSmall = imageList;
        m_ownsImageListSmall = false;
    }
    else if ( which == wxIMAGE_LIST_STATE )
    {
        if (m_ownsImageListState) delete m_imageListState;
        m_imageListState = imageList;
        m_ownsImageListState = false;
    }
}

void wxListCtrl::AssignImageList(wxImageList *imageList, int which)
{
    SetImageList(imageList, which);
    if ( which == wxIMAGE_LIST_NORMAL )
        m_ownsImageListNormal = true;
    else if ( which == wxIMAGE_LIST_SMALL )
        m_ownsImageListSmall = true;
    else if ( which == wxIMAGE_LIST_STATE )
        m_ownsImageListState = true;
}

void wxListCtrl::RefreshItem(long WXUNUSED(item))
{
}

void wxListCtrl::RefreshItems(long WXUNUSED(itemFrom), long WXUNUSED(itemTo))
{
}

bool wxListCtrl::Arrange(int WXUNUSED(flag))
{
    return false;
}

bool wxListCtrl::DeleteItem(long item)
{
    QTreeWidgetItem *qitem = QtGetItem(item);
    if ( qitem != NULL )
    {
        delete qitem;
        return true;
    }
    else
        return false;
}

bool wxListCtrl::DeleteAllItems()
{
    m_qtTreeWidget->clear();
    return true;
}

bool wxListCtrl::DeleteColumn(int col)
{
    // Qt cannot easily add or remove columns, so only the last one can be deleted
    if ( col == GetColumnCount() - 1 )
    {
        m_qtTreeWidget->setColumnCount(col);
        return true;
    }
    else
        return false;
}

bool wxListCtrl::DeleteAllColumns()
{
    m_qtTreeWidget->setColumnCount(0);
    return true;
}

void wxListCtrl::ClearAll()
{
    m_qtTreeWidget->clear();
}

wxTextCtrl* wxListCtrl::EditLabel(long item, wxClassInfo* WXUNUSED(textControlClass))
{
    QTreeWidgetItem *qitem = QtGetItem(item);
    if ( qitem != NULL )
    {
        m_qtTreeWidget->openPersistentEditor(qitem);
    }
    return NULL;
}

bool wxListCtrl::EndEditLabel(bool WXUNUSED(cancel))
{
    int item = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
    if (item > 0)
    {
        QTreeWidgetItem *qitem = QtGetItem(item);
        if ( qitem != NULL )
        {
            m_qtTreeWidget->closePersistentEditor(qitem);
            return true;
        }
    }
    return false;
}

bool wxListCtrl::EnsureVisible(long item)
{
    QTreeWidgetItem *qitem = QtGetItem(item);
    if ( qitem != NULL )
    {
         m_qtTreeWidget->scrollToItem(qitem);
         return true;
    }
    else
        return false;
}

long wxListCtrl::FindItem(long start, const wxString& str, bool partial)
{
    QList <QTreeWidgetItem *> qitems = m_qtTreeWidget->findItems(
                wxQtConvertString(str),
                !partial ? Qt::MatchExactly : Qt::MatchContains );
    for (int i=0; i<qitems.length(); i++)
    {
        int ret;
        ret = m_qtTreeWidget->indexOfTopLevelItem(qitems.at(i));
        if ( ret >= start )
            return ret;
    }
    return -1;
}

long wxListCtrl::FindItem(long start, wxUIntPtr data)
{
    QVariant variant = qVariantFromValue(data);
    // search only one hit (if any):
    QModelIndexList qindexes = m_qtTreeWidget->model()->match(
                 m_qtTreeWidget->model()->index(start, 0),
                Qt::UserRole, variant, 1 );
    if (qindexes.isEmpty())
        return -1;
    return qindexes.at(0).row();
}

long wxListCtrl::FindItem(long WXUNUSED(start), const wxPoint& WXUNUSED(pt), int WXUNUSED(direction))
{
    return -1;
}

long wxListCtrl::HitTest(const wxPoint& point, int &flags, long* ptrSubItem) const
{
    QModelIndex index = m_qtTreeWidget->indexAt(wxQtConvertPoint(point));
    if ( index.isValid() )
    {
        flags = wxLIST_HITTEST_ONITEM;
        if (ptrSubItem)
            *ptrSubItem = index.column();
    }
    else
    {
        flags = wxLIST_HITTEST_NOWHERE;
        if (ptrSubItem)
            *ptrSubItem = 0;
    }
    return index.row();
}

long wxListCtrl::InsertItem(const wxListItem& info)
{
    // default return value if not successful:
    int index = -1;
    wxASSERT_MSG( info.m_itemId != -1, wxS("Item ID must be set.") );
    QTreeWidgetItem *qitem = new QTreeWidgetItem();
    if ( qitem != NULL )
    {
        // insert at the correct index and return it:
        m_qtTreeWidget->insertTopLevelItem(info.GetId(), qitem);
        // return the correct position of the item or -1 if not found:
        index = m_qtTreeWidget->indexOfTopLevelItem(qitem);
        if ( index != -1 )
        {
            // temporarily copy the item info (we need a non-const instance)
            wxListItem tmp = info;
            // set the text, image, etc.:
            SetItem(tmp);
        }
    }
    return index;
}

long wxListCtrl::InsertItem(long index, const wxString& label)
{
    wxListItem info;
    info.m_text = label;
    info.m_mask = wxLIST_MASK_TEXT;
    info.m_itemId = index;
    return InsertItem(info);
}

long wxListCtrl::InsertItem(long index, int imageIndex)
{
    wxListItem info;
    info.m_image = imageIndex;
    info.m_mask = wxLIST_MASK_IMAGE;
    info.m_itemId = index;
    return InsertItem(info);
}

long wxListCtrl::InsertItem(long index, const wxString& label, int imageIndex)
{
    wxListItem info;
    //info.m_image = imageIndex == -1 ? I_IMAGENONE : imageIndex;
    info.m_text = label;
    info.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE;
    info.m_image = imageIndex;
    info.m_itemId = index;
    return InsertItem(info);
}

long wxListCtrl::DoInsertColumn(long col, const wxListItem& info)
{
    QTreeWidgetItem *qitem = m_qtTreeWidget->headerItem();
    if ( qitem != NULL )
    {
        qitem->setText(col, wxQtConvertString(info.GetText()));
        qitem->setTextAlignment(col, wxQtConvertTextAlign(info.GetAlign()));
        if (info.GetWidth())
            m_qtTreeWidget->setColumnWidth(col, info.GetWidth());
        return col;
    }
    else
        return -1;
}


void wxListCtrl::SetItemCount(long WXUNUSED(count))
{
}

bool wxListCtrl::ScrollList(int dx, int dy)
{
    // aproximate, as scrollContentsBy is protected
    m_qtTreeWidget->scroll(dx, dy);
    return true;
}

bool wxListCtrl::SortItems(wxListCtrlCompare WXUNUSED(fn), wxIntPtr WXUNUSED(data))
{
    return false;
}

// ----------------------------------------------------------------------------
// virtual list controls (not currently implemented in wxQT)
// ----------------------------------------------------------------------------

wxString wxListCtrl::OnGetItemText(long WXUNUSED(item), long WXUNUSED(col)) const
{
    // this is a pure virtual function, in fact - which is not really pure
    // because the controls which are not virtual don't need to implement it
    wxFAIL_MSG( wxT("wxListCtrl::OnGetItemText not supposed to be called") );

    return wxEmptyString;
}

int wxListCtrl::OnGetItemImage(long WXUNUSED(item)) const
{
    wxCHECK_MSG(!GetImageList(wxIMAGE_LIST_SMALL),
                -1,
                wxT("List control has an image list, OnGetItemImage or OnGetItemColumnImage should be overridden."));
    return -1;
}

int wxListCtrl::OnGetItemColumnImage(long item, long column) const
{
    if (!column)
        return OnGetItemImage(item);

    return -1;
}

QWidget *wxListCtrl::GetHandle() const
{
    return m_qtTreeWidget;
}

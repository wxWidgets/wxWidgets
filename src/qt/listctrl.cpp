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
#include <QtWidgets/QTreeView>
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

class wxQtTreeWidget : public wxQtEventSignalHandler< QTreeView, wxListCtrl >
{
public:
    wxQtTreeWidget( wxWindow *parent, wxListCtrl *handler );

    void EmitListEvent(wxEventType typ, const QModelIndex &index) const;

    void closeEditor(
        QWidget *editor,
        QAbstractItemDelegate::EndEditHint hint
    ) wxOVERRIDE
    {
        QTreeView::closeEditor(editor, hint);
        m_editorFactory.ClearEditor();
    }

    wxTextCtrl *GetEditControl()
    {
        return m_editorFactory.GetEditControl();
    }

    virtual void paintEvent(QPaintEvent *event) wxOVERRIDE
    {
        QTreeView::paintEvent(event);
    }

private:
    void itemClicked(const QModelIndex &index);
    void itemActivated(const QModelIndex &index);
    void itemPressed(const QModelIndex &index);

    void ChangeEditorFactory()
    {
        QItemDelegate *qItemDelegate = static_cast<QItemDelegate*>(itemDelegate());
        qItemDelegate->setItemEditorFactory(&m_editorFactory);
    }

    wxQtItemEditorFactory m_editorFactory;
};

wxQtTreeWidget::wxQtTreeWidget( wxWindow *parent, wxListCtrl *handler )
    : wxQtEventSignalHandler< QTreeView, wxListCtrl >( parent, handler ),
      m_editorFactory(handler)
{
    connect(this, &QTreeView::clicked, this, &wxQtTreeWidget::itemClicked);
    connect(this, &QTreeView::pressed, this, &wxQtTreeWidget::itemPressed);
    connect(this, &QTreeView::activated, this, &wxQtTreeWidget::itemActivated);

    ChangeEditorFactory();
}

void wxQtTreeWidget::EmitListEvent(wxEventType typ, const QModelIndex &index) const
{
    wxListCtrl *handler = GetHandler();
    if ( handler )
    {
        // prepare the event
        // -----------------
        wxListEvent event;
        event.SetEventType(typ);
        event.SetId(handler->GetId());
        event.m_itemIndex = index.row();
        event.m_col = index.column();
        event.m_item.SetId(event.m_itemIndex);
        event.m_item.SetMask(wxLIST_MASK_TEXT |
                             wxLIST_MASK_IMAGE |
                             wxLIST_MASK_DATA);
        handler->GetItem(event.m_item);
        EmitEvent(event);
    }
}

void wxQtTreeWidget::itemClicked(const QModelIndex &index)
{
    EmitListEvent(wxEVT_LIST_ITEM_SELECTED, index);
}

void wxQtTreeWidget::itemPressed(const QModelIndex &index)
{
    EmitListEvent(wxEVT_LIST_ITEM_SELECTED, index);
}

void wxQtTreeWidget::itemActivated(const QModelIndex &index)
{
    EmitListEvent(wxEVT_LIST_ITEM_ACTIVATED,index);
}

Qt::AlignmentFlag wxQtConvertTextAlign(wxListColumnFormat align)
{
    switch ( align )
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
    switch ( align )
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

class wxQtListModel : public QAbstractTableModel
{
public:
    wxQtListModel(wxListCtrl *listCtrl) :
        m_view(NULL),
        m_listCtrl(listCtrl)
    {
    }

    int rowCount(const QModelIndex& WXUNUSED(parent)) const wxOVERRIDE
    {
        return static_cast<int>(m_rows.size());
    }
    int columnCount(const QModelIndex& WXUNUSED(parent)) const wxOVERRIDE
    {
        return static_cast<int>(m_headers.size());
    }

    QVariant data(const QModelIndex &index, int role) const wxOVERRIDE
    {
        const int row = index.row();
        const int col = index.column();

        wxCHECK_MSG(row >= 0 && static_cast<size_t>(row) < m_rows.size(),
            QVariant(),
            "Invalid row index"
        );

        const RowItem &rowItem = m_rows[row];
        const ColumnItem &columnItem = rowItem[col];

        const bool isSelected = IsSelected(index);

        switch ( role )
        {
            case Qt::DisplayRole:
            {
                return QVariant::fromValue(columnItem.m_label);
            }
            case Qt::EditRole:
            {
                wxListItem listItem;
                listItem.SetId(row);
                listItem.SetColumn(col);

                wxListEvent event(wxEVT_LIST_BEGIN_LABEL_EDIT, m_listCtrl->GetId());
                event.SetEventObject(m_listCtrl);
                event.SetItem(listItem); 
                if ( m_listCtrl->HandleWindowEvent(event) && !event.IsAllowed() )
                    return QVariant();

                return QVariant::fromValue(columnItem.m_label); 
            }
            case Qt::DecorationRole:
            {
                wxImageList *imageList = GetImageList();
                if ( imageList == NULL )
                    return QVariant();

                int imageIndex = -1;

                if ( columnItem.m_selectedImage != -1 )
                {
                    if ( isSelected )
                        imageIndex = columnItem.m_selectedImage;
                }

                if ( imageIndex == -1 )
                    imageIndex = columnItem.m_image;

                if ( imageIndex == -1 )
                    return QVariant();

                wxBitmap image = imageList->GetBitmap(imageIndex);
                wxCHECK_MSG(image.IsOk(), QVariant(), "Invalid image");
                return QVariant::fromValue(*image.GetHandle());
            }
            case Qt::FontRole:
                return QVariant::fromValue(columnItem.m_font);

            case Qt::BackgroundRole:
                return columnItem.m_backgroundColour.isValid() && !isSelected
                    ? QVariant::fromValue(columnItem.m_backgroundColour)
                    : QVariant();

            case Qt::ForegroundRole:
                return columnItem.m_textColour.isValid() && !isSelected
                    ? QVariant::fromValue(columnItem.m_textColour)
                    : QVariant();

            case Qt::TextAlignmentRole:
                return columnItem.m_align;

             case Qt::CheckStateRole:
                {
                  if ( !m_listCtrl->HasCheckBoxes() || col > 0 )
                    return QVariant();

                  return rowItem.m_checked; 
                }

            default:
                return QVariant();
        }
    }

    bool setData(
        const QModelIndex &index,
        const QVariant &value,
        int role
    ) wxOVERRIDE
    {
        const int row = index.row();
        const int col = index.column();

        wxCHECK_MSG(
            row >= 0 && static_cast<size_t>(row) < m_rows.size(),
            false,
            "Invalid row index"
        );
        wxCHECK_MSG(
            col >= 0 && static_cast<size_t>(col) < m_rows[row].m_columns.size(),
            false,
            "Invalid column index"
        );

        if ( role == Qt::DisplayRole || role == Qt::EditRole )
        { 
            wxListItem listItem;
            listItem.SetId(row);
            listItem.SetColumn(col);

            wxListEvent event(wxEVT_LIST_END_LABEL_EDIT, m_listCtrl->GetId());
            event.SetEventObject(m_listCtrl);
            event.SetItem(listItem); 

            if ( m_listCtrl->HandleWindowEvent(event) && !event.IsAllowed() )
                return false;

            m_rows[row][col].m_label = value.toString();
            return true;
        }
    
        if ( role == Qt::CheckStateRole && col == 0)
        {
            m_rows[row].m_checked = static_cast<Qt::CheckState>(value.toUInt()) == Qt::Checked;

            wxListItem listItem;
            listItem.SetId(row);

            const wxEventType eventType = m_rows[row].m_checked ?
                wxEVT_LIST_ITEM_CHECKED : wxEVT_LIST_ITEM_UNCHECKED;

            wxListEvent event(eventType, m_listCtrl->GetId());
            event.SetEventObject(m_listCtrl);
            event.SetItem(listItem);
            m_listCtrl->HandleWindowEvent(event);
            return true;
        }

        return false;
    }

    QVariant headerData(
        int section,
        Qt::Orientation orientation,
        int role
    ) const wxOVERRIDE
    {
        if ( orientation == Qt::Vertical )
            return QVariant();
        
        wxCHECK_MSG(static_cast<size_t>(section) < m_headers.size(),
            QVariant(), "Invalid header index");

        const ColumnItem& header = m_headers[section];

        switch ( role )
        {
            case Qt::DisplayRole:
                return header.m_label;

            case Qt::TextAlignmentRole:
                return header.m_align;
        }
        return QVariant();
    }

    Qt::ItemFlags flags(const QModelIndex &index) const wxOVERRIDE
    {
        Qt::ItemFlags itemFlags = Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;

        if ( m_listCtrl->HasFlag(wxLC_EDIT_LABELS) )
            itemFlags |= Qt::ItemIsEditable;

         if ( index.column() == 0 && m_listCtrl->HasCheckBoxes() )
            itemFlags |= Qt::ItemIsUserCheckable;

         return itemFlags | QAbstractTableModel::flags(index);
    }

    bool removeRows(int row, int count, const QModelIndex &parent) wxOVERRIDE
    {
        beginRemoveRows(parent, row, row + count - 1);
        eraseFromContainer(m_rows, row, count);
        endRemoveRows();
        return true;
    }

    bool removeColumns(int column, int count, const QModelIndex &parent) wxOVERRIDE
    {
        beginRemoveColumns(parent, column, column + count - 1);

        eraseFromContainer(m_headers, column, count);

        const int nRows = this->m_rows.size();
        for ( int i = 0; i < nRows; ++i )
        {
            eraseFromContainer(m_rows[i].m_columns, column, count);
        }

        endRemoveColumns();
        return true;
    }

    bool GetColumn(int index, wxListItem &info) const
    {
        wxCHECK_MSG(static_cast<size_t>(index) < m_headers.size(),
            false, "Invalid column");

        const ColumnItem &column = m_headers[index];
        info.SetText(wxQtConvertString(column.m_label));
        info.SetAlign(wxQtConvertAlignFlag(column.m_align));
        info.SetWidth(m_view->columnWidth(index));
        return true;
    }

    bool GetItem(wxListItem& info)
    {
        const int row = static_cast<int>(info.GetId());
        const int col = info.m_col;

        wxCHECK_MSG(
            row >= 0 && static_cast<size_t>(row) < m_rows.size(),
            false,
            "Invalid row"
        );

        wxCHECK_MSG(
            col >= 0 && static_cast<size_t>(col) < m_rows[col].m_columns.size(),
            false,
            "Invalid col"
        );

        const RowItem &rowItem = m_rows[row];
        const ColumnItem &columnItem = rowItem[col];

        if ( !info.m_mask )
            // by default, get everything for backwards compatibility
            info.m_mask = -1;

        if ( info.m_mask & wxLIST_MASK_TEXT )
            info.SetText(wxQtConvertString(columnItem.m_label));

        if ( info.m_mask & wxLIST_MASK_DATA )
        {
            info.SetData(rowItem.m_data);
        }

        if ( info.m_mask & wxLIST_MASK_STATE )
        {
            info.m_state = wxLIST_STATE_DONTCARE;
            if ( info.m_stateMask & wxLIST_STATE_FOCUSED )
            {
                if ( m_view->currentIndex().row() == row )
                    info.m_state |= wxLIST_STATE_FOCUSED;
            }
            if ( info.m_stateMask & wxLIST_STATE_SELECTED )
            {
                if ( IsSelected(index(row,col)) )
                    info.m_state |= wxLIST_STATE_SELECTED;
            }
        }

        return true;
    }

    bool SetItem(wxListItem &info)
    {
        const int row = static_cast<int>(info.GetId());
        const int col = info.m_col;

        wxCHECK_MSG( static_cast<size_t>(row) < m_rows.size(),
            false, "Invalid row");
        wxCHECK_MSG( static_cast<size_t>(col) < m_headers.size(),
            false, "Invalid col");

        const QModelIndex modelIndex = index(row, col);
        RowItem &rowItem = m_rows[row];
        ColumnItem &columnItem = rowItem[col];

        QVector<int> roles;

        if ( (info.m_mask & wxLIST_MASK_TEXT) && !info.GetText().IsNull() )
        {
            columnItem.m_label =  wxQtConvertString(info.GetText());
            roles.push_back(Qt::DisplayRole);
        }

        columnItem.m_align = wxQtConvertTextAlign(info.GetAlign());
        roles.push_back(Qt::TextAlignmentRole);

        if ( info.m_mask & wxLIST_MASK_DATA )
        {
            rowItem.m_data = (void*)(info.GetData());
            roles.push_back(Qt::UserRole);
        }

        if ( info.m_mask & wxLIST_MASK_STATE )
        {
            if ( (info.m_stateMask & wxLIST_STATE_FOCUSED) &&
                (info.m_state & wxLIST_STATE_FOCUSED) )
                m_view->setCurrentIndex(modelIndex);
            if ( info.m_stateMask & wxLIST_STATE_SELECTED )
            {
                QItemSelectionModel *selection = m_view->selectionModel();
                const QItemSelectionModel::SelectionFlag flag =
                    info.m_state & wxLIST_STATE_SELECTED
                        ? QItemSelectionModel::Select
                        : QItemSelectionModel::Deselect;
                selection->select(modelIndex,  flag|QItemSelectionModel::Rows);
            }
        }

        if ( info.m_mask & wxLIST_MASK_IMAGE )
        {
            columnItem.m_image = info.m_image;
            roles.push_back(Qt::DecorationRole);
        }

        if ( info.GetFont().IsOk() )
        {
            columnItem.m_font = info.GetFont().GetHandle();
            roles.push_back(Qt::FontRole);
        }

        if ( info.GetTextColour().IsOk() )
        {
            columnItem.m_textColour = info.GetTextColour().GetQColor();
            roles.push_back(Qt::BackgroundRole);
        }

        if ( info.GetBackgroundColour().IsOk() )
        {
            columnItem.m_backgroundColour = info.GetBackgroundColour().GetQColor();
            roles.push_back(Qt::ForegroundRole);
        }

        dataChanged(modelIndex, modelIndex, roles);

        return true;
    }

    void SetView(QTreeView *view)
    {
        m_view = view;
    }

    wxColour GetItemTextColour(long item)
    {
        wxCHECK_MSG(item >= 0 && static_cast<size_t>(item) < m_rows.size(),
            wxNullColour, "Invalid row");

        wxCHECK_MSG(!m_rows[item].m_columns.empty(),
            wxNullColour, "No columns in model");

        return wxColour(m_rows[item][0].m_textColour);
    }

    wxColour GetItemBackgroundColour(long item)
    {
        wxCHECK_MSG(item >= 0 && static_cast<size_t>(item) < m_rows.size(),
            wxNullColour, "Invalid row");
        wxCHECK_MSG(!m_headers.empty(),
            wxNullColour, "No columns in model");

        return wxColour(m_rows[item][0].m_backgroundColour);
    }

    wxFont GetItemFont(long item)
    {
        wxCHECK_MSG(item >= 0 && static_cast<size_t>(item) < m_rows.size(),
            wxNullFont, "Invalid row");

        wxCHECK_MSG(!m_headers.empty(),
            wxNullFont, "No columns in model");

        return wxFont(m_rows[item][0].m_font);
    }

    long FindItem(long start, const QString& str, bool partial) const
    {
        if ( start < 0 )
            start = 0;

        const QString strUpper = str.toUpper();

        const long numberOfRows = m_rows.size();
        const long numberOfColumns = m_headers.size();

        if ( partial )
        {
            for ( long i = start; i < numberOfRows; ++i )
            {
                for ( long j = 0; j < numberOfColumns; ++j )
                {
                    const QString current = m_rows[i][j].m_label.toUpper();
                    if ( current.contains(strUpper) )
                        return i;
                }
            }

            return -1;
        }

        for ( long i = start; i < numberOfRows; ++i )
        {
            for ( long j = 0; j < numberOfColumns; ++j )
            {
                if ( m_rows[i][j].m_label.toUpper() == strUpper )
                    return i;
            }
        }

        return -1;
    }

    long FindItem(long start, wxUIntPtr data) const
    {
        if ( start < 0 )
            start = 0;

        const long count = m_rows.size();

        for ( long i = start; i < count; ++i )
        {
            if ( m_rows[i].m_data == reinterpret_cast<void*>(data) )
                return i;
        }

        return -1;
    }


    long InsertItem(wxListItem info)
    {
        const int column = info.GetColumn();

        if ( static_cast<size_t>(column) >= m_headers.size() )
        {
            beginInsertColumns(QModelIndex(), m_headers.size(), column);
       
            const ColumnItem emptyColumn;

            for ( int c = m_headers.size(); c <= column; ++c )
            {
                m_headers.push_back(emptyColumn);
                for ( size_t r = 0; r < m_rows.size(); ++r )
                {
                    m_rows[r].m_columns.push_back(emptyColumn);
                }
            }

            endInsertColumns();
        }

        const long row = info.GetId();
        long newRowIndex;

        if ( row == -1 || static_cast<size_t>(row) >= m_rows.size() )
        {
            m_rows.push_back(RowItem(m_headers.size()));
            newRowIndex = m_rows.size() - 1;
        }
        else
        {
            std::vector<RowItem>::iterator i = m_rows.begin();
            std::advance(i, row);
            m_rows.insert(i, RowItem(m_headers.size()));
            newRowIndex = row;
        }

        beginInsertRows(QModelIndex(), newRowIndex, newRowIndex);

        info.SetId(newRowIndex);
        SetItem(info);

        endInsertRows();

        return newRowIndex;
    }

    long InsertColumn(long col, const wxListItem& info)
    {
        long newColumnIndex;

        ColumnItem newColumn;
        newColumn.m_align = wxQtConvertTextAlign(info.GetAlign());
        newColumn.m_label = wxQtConvertString(info.GetText());

        if ( col == -1 || static_cast<size_t>(col) >= m_headers.size() )
        {
            newColumnIndex = m_headers.empty() ? 0 : m_headers.size() - 1;
        }
        else
        {
            newColumnIndex = col;
        }

        beginInsertColumns(QModelIndex(), newColumnIndex, newColumnIndex);

        std::vector<ColumnItem>::iterator i = m_headers.begin();
        std::advance(i, newColumnIndex);
        m_headers.insert(i, newColumn);

        const int numberOfRows = m_rows.size();

        for (int i = 0; i < numberOfRows; ++i )
        {
            std::vector<ColumnItem>::iterator it = m_rows[i].m_columns.begin();
            std::advance(it, newColumnIndex);
            m_rows[i].m_columns.insert(it, newColumn);
        }

        endInsertColumns();

        return true;
    }

    void SortItems(wxListCtrlCompare fn, wxIntPtr data)
    {
        CompareAdapter compare(fn, data);
        std::sort(m_rows.begin(), m_rows.end(), compare);
    }

    bool IsItemChecked(long item) const
    {
        wxCHECK_MSG(item >= 0 && static_cast<size_t>(item) <= m_rows.size(),
            false, "Invalid row");

        return m_rows[item].m_checked;
    }

    void CheckItem(long item, bool check)
    {
        wxCHECK_RET(item >= 0 && static_cast<size_t>(item) <= m_rows.size(),
            "Invalid row");

        m_rows[item].m_checked = check;

        QVector<int> roles;
        roles.push_back(Qt::CheckStateRole); 
        const QModelIndex modelIndex = index(item,0);
        dataChanged(modelIndex, modelIndex, roles);
    }

    virtual bool IsVirtual() const
    {
        return false;
    }

    virtual void SetVirtualItemCount(long WXUNUSED(count))
    {
    }

protected:
    wxImageList *GetImageList() const
    {
        const int requiredList = m_listCtrl->HasFlag(wxLC_SMALL_ICON) ?
            wxIMAGE_LIST_SMALL : wxIMAGE_LIST_NORMAL;
        return m_listCtrl->GetImageList(requiredList);
    }

    wxListCtrl *GetListCtrl() const
    {
        return m_listCtrl;
    }

    bool IsSelected(const QModelIndex& index) const
    {
        QModelIndexList selectedIndices = m_view->selectionModel()->selectedIndexes();
        return selectedIndices.contains(index);
    }

private:
    template <typename  T>
    static void eraseFromContainer(T &container, int start_index, int count)
    {

        typename T::iterator first = container.begin();
        std::advance(first, start_index);

        typename T::iterator last = first;
        std::advance(last, count);

        container.erase(first, last);
    }

    struct ColumnItem
    {
        ColumnItem() :
            m_align(Qt::AlignLeft),
            m_image(-1),
            m_selectedImage(-1)
        {
        }

        QString m_label;
        QColor m_backgroundColour;
        QColor m_textColour;
        QFont m_font;
        Qt::AlignmentFlag m_align;
        int m_image;
        int m_selectedImage;
    };

    struct RowItem
    {
        RowItem() :
            m_data(NULL),
            m_checked(false)
        {
        }

        RowItem(int columnCount ) :
            m_columns(columnCount),
            m_data(NULL),
            m_checked(false)
        {
        }

        const ColumnItem &operator[](int index) const
        {
            return m_columns[index];
        }

        ColumnItem &operator[](int index)
        {
            return m_columns[index];
        }

        std::vector<ColumnItem> m_columns;
        void *m_data;
        bool m_checked;

    };

    struct CompareAdapter
    {
        CompareAdapter(wxListCtrlCompare fn, wxIntPtr data) :
            m_fn(fn),
            m_data(data)
        {
        }

        bool operator()(const RowItem &lhs, const RowItem &rhs ) const
        {
            return m_fn((wxIntPtr)lhs.m_data, (wxIntPtr)rhs.m_data, m_data) < 0;
        }

        wxListCtrlCompare m_fn;
        wxIntPtr m_data;
    };

    std::vector<ColumnItem> m_headers;
    std::vector<RowItem> m_rows;
    QTreeView *m_view;
    wxListCtrl *m_listCtrl;
};

class wxQtVirtualListModel : public wxQtListModel
{
public:
    wxQtVirtualListModel(wxListCtrl *listCtrl) :
        wxQtListModel(listCtrl),
        m_rowCount(0)
    {
    }

    int rowCount(const QModelIndex& WXUNUSED(parent)) const wxOVERRIDE
    {
        return m_rowCount;
    }

    QVariant data(const QModelIndex &index, int role) const wxOVERRIDE
    {
        wxListCtrl *listCtrl = GetListCtrl();

        const int row = index.row();
        const int col = index.column();

        if ( role == Qt::DisplayRole || role == Qt::EditRole )
        {
            wxString text = listCtrl->OnGetItemText(row, col);
            return QVariant::fromValue(wxQtConvertString(text));
        }

        if ( role == Qt::DecorationRole )
        {
            wxImageList *imageList = GetImageList();
            if ( imageList == NULL )
                return QVariant();

            const int imageIndex = listCtrl->OnGetItemColumnImage(row, col);
            wxBitmap image = imageList->GetBitmap(imageIndex);
            wxCHECK_MSG(image.IsOk(), QVariant(), "Invalid Bitmap");
            return QVariant::fromValue(*image.GetHandle());
        }

        return QVariant();
    }

    virtual bool IsVirtual() const wxOVERRIDE
    {
        return true;
    }

    virtual void SetVirtualItemCount(long count) wxOVERRIDE
    {
        const int last = count > 0 ? count - 1 : 0;
        beginInsertRows(QModelIndex(), 0, last);
        m_rowCount = static_cast<int>(count);
        endInsertRows();
    }
private:
    int m_rowCount;
};

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
    m_qtTreeWidget->setModel(m_model);

    if (style & wxLC_NO_HEADER)
        m_qtTreeWidget->setHeaderHidden(true);

    m_qtTreeWidget->setRootIsDecorated(false);
    m_qtTreeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_model->SetView(m_qtTreeWidget);

    if ( !QtCreateControl( parent, id, pos, size, style, validator, name ) )
        return false;

    SetWindowStyleFlag(style);
    return true;
}

void wxListCtrl::Init()
{
    m_hasCheckBoxes = false;
    m_model = new wxQtListModel(this);
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
    m_qtTreeWidget->setModel(NULL);
    delete m_model;

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
    return m_model->GetColumn(col, info);
}

bool wxListCtrl::SetColumn(int col, const wxListItem& info)
{
    DoInsertColumn(col, info);
    if ( info.GetMask() & wxLIST_MASK_WIDTH)
        SetColumnWidth(col, info.GetWidth());
    return true;
}

int wxListCtrl::GetColumnWidth(int col) const
{
    return m_qtTreeWidget->columnWidth(col);
}

bool wxListCtrl::SetColumnWidth(int col, int width)
{
    if ( width < 0)
    {
        m_qtTreeWidget->resizeColumnToContents(width);
        return true;
    }

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
    // this may not be exact but should be a good approximation:
    int h = m_qtTreeWidget->visualRect(m_model->index(0, 0)).height();
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

bool wxListCtrl::GetItem(wxListItem& info) const
{
    return m_model->GetItem(info);
}

bool wxListCtrl::SetItem(wxListItem& info)
{
    return m_model->SetItem(info);
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
    wxListItem info;
    info.SetId(item);
    info.m_col = col;
    info.m_mask = wxLIST_MASK_TEXT;
    GetItem(info);
    return info.GetText();
}

void wxListCtrl::SetItemText(long item, const wxString& str)
{
    wxListItem info;
    info.SetId(item);
    info.m_mask = wxLIST_MASK_TEXT;
    info.SetText(str);
    SetItem(info);
}

wxUIntPtr wxListCtrl::GetItemData(long item) const
{
    wxListItem info;
    info.SetId(item);
    info.m_mask = wxLIST_MASK_DATA;
    GetItem(info);
    return info.GetData();
}

bool wxListCtrl::SetItemPtrData(long item, wxUIntPtr data)
{
    wxListItem info;
    info.SetId(item);
    info.m_mask = wxLIST_MASK_DATA;
    info.SetData(data);
    return SetItem(info);
}

bool wxListCtrl::SetItemData(long item, long data)
{
    return SetItemPtrData(item, static_cast<wxUIntPtr>(data));
}

bool wxListCtrl::GetItemRect(long item, wxRect& rect, int WXUNUSED(code)) const
{
    wxCHECK_MSG(item >= 0 && (item < GetItemCount()), false,
                 "invalid item in GetSubItemRect");

    const int columnCount = m_model->columnCount(QModelIndex());
    if ( columnCount == 0)
        return false;

    //Calculate the union of the bounds of the items in the first and last column
    //for the given row
    QRect first = m_qtTreeWidget->visualRect(m_model->index(item, 0));
    QRect last = m_qtTreeWidget->visualRect(m_model->index(item, columnCount - 1));
    rect = wxQtConvertRect(first.united(last));

    return true;
}

bool wxListCtrl::GetSubItemRect(long item, long subItem, wxRect& rect, int WXUNUSED(code)) const
{
    wxCHECK_MSG( item >= 0 && item < GetItemCount(),
        false, "invalid row index in GetSubItemRect");

    wxCHECK_MSG( subItem >= 0 && subItem < GetColumnCount(),
        false, "invalid column index in GetSubItemRect");

    const QModelIndex index = m_qtTreeWidget->model()->index(item, subItem);
    rect = wxQtConvertRect(m_qtTreeWidget->visualRect(index));
    return true;
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
    return m_model->rowCount(QModelIndex());
}

int wxListCtrl::GetColumnCount() const
{
    return m_model->columnCount(QModelIndex());
}

wxSize wxListCtrl::GetItemSpacing() const
{
    return wxSize();
}

void wxListCtrl::SetItemTextColour( long item, const wxColour& col)
{
    const int columnCount = m_model->columnCount(QModelIndex());

    wxListItem listItem;
    listItem.SetId(item);
    listItem.SetTextColour(col);

    for ( int i = 0; i < columnCount; ++i)
    {
        listItem.m_col = i;
        SetItem(listItem);
    }
}

wxColour wxListCtrl::GetItemTextColour( long item ) const
{
    return m_model->GetItemTextColour(item);
}

void wxListCtrl::SetItemBackgroundColour( long item, const wxColour &col)
{
    wxListItem listItem;
    listItem.SetId(item);

    listItem.SetBackgroundColour(col);

    const int columnCount = m_model->columnCount(QModelIndex());

    for ( int i = 0; i < columnCount; ++i)
    {
        listItem.m_col = i;
        SetItem(listItem);
    }
}

wxColour wxListCtrl::GetItemBackgroundColour( long item ) const
{
    return m_model->GetItemBackgroundColour(item);
}

void wxListCtrl::SetItemFont( long item, const wxFont &f)
{
    const int columnCount = m_model->columnCount(QModelIndex());

    wxListItem listItem;
    listItem.SetId(item);
    listItem.SetFont(f);

    for ( int i = 0; i < columnCount; ++i)
    {
        listItem.m_col = i;
        SetItem(listItem);
    }
}

wxFont wxListCtrl::GetItemFont( long item ) const
{
    return m_model->GetItemFont(item);
}

int wxListCtrl::GetSelectedItemCount() const
{
    QItemSelectionModel *selectionModel = m_qtTreeWidget->selectionModel();
    return selectionModel->selectedIndexes().length();
}

wxColour wxListCtrl::GetTextColour() const
{
    const QPalette palette = m_qtTreeWidget->palette();
    const QColor color = palette.color(QPalette::WindowText);
    return wxColour(color);
}

void wxListCtrl::SetTextColour(const wxColour& col)
{
    QPalette palette = m_qtTreeWidget->palette();
    palette.setColor(QPalette::WindowText, col.GetQColor());
    m_qtTreeWidget->setPalette(palette);
}

long wxListCtrl::GetTopItem() const
{
    const long itemCount = GetItemCount();
    for ( long i = 0; i < itemCount; ++i )
    {
        wxRect itemRect;
        GetItemRect(i, itemRect);
        if ( itemRect.GetY() >= 0 )
            return i;
    }
    return 0;
}

bool wxListCtrl::HasCheckBoxes() const
{
    return m_hasCheckBoxes;
}

bool wxListCtrl::EnableCheckBoxes(bool enable /*= true*/)
{  
    m_hasCheckBoxes = enable;
    QVector<int> roles;
    roles.push_back(Qt::CheckStateRole); 
    m_model->dataChanged(m_model->index(0,0), 
        m_model->index(GetItemCount() - 1, GetColumnCount() - 1), 
        roles);
    return true;
}

bool wxListCtrl::IsItemChecked(long item) const
{
    return m_model->IsItemChecked(item);
}

void wxListCtrl::CheckItem(long item, bool check)
{
    m_model->CheckItem(item, check);
}

void wxListCtrl::SetSingleStyle(long WXUNUSED(style), bool WXUNUSED(add))
{
}

void wxListCtrl::SetWindowStyleFlag(long style)
{
    m_qtTreeWidget->setHeaderHidden((style & wxLC_NO_HEADER) != 0);
    m_qtTreeWidget->setSelectionMode((style & wxLC_SINGLE_SEL) != 0 ? 
        QAbstractItemView::SingleSelection : QAbstractItemView::ExtendedSelection);
    const bool needVirtual = (style & wxLC_VIRTUAL) != 0;
    
    if ( needVirtual != m_model->IsVirtual() )
    {
        wxQtListModel *oldModel = m_model;
        m_model = needVirtual ? new wxQtVirtualListModel(this) : new wxQtListModel(this);
        m_model->SetView(m_qtTreeWidget);
        m_qtTreeWidget->setModel(m_model);
        delete oldModel;
    }
}

long wxListCtrl::GetNextItem(long item, int WXUNUSED(geometry), int state) const
{
    wxListItem info;
    long ret = item,
         max = GetItemCount();
    wxCHECK_MSG((ret == -1) || (ret < max), -1,
                 "invalid listctrl index in GetNextItem()");

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

void wxListCtrl::RefreshItem(long item)
{
    const int columnCount = GetColumnCount();
    const QModelIndex start = m_model->index(item, 0);
    const QModelIndex end = m_model->index(item, columnCount - 1);
    m_model->dataChanged(start, end);
}

void wxListCtrl::RefreshItems(long itemFrom, long itemTo)
{
    const int columnCount = GetColumnCount();
    const QModelIndex start = m_model->index(itemFrom, 0);
    const QModelIndex end = m_model->index(itemTo, columnCount - 1);
    m_model->dataChanged(start, end);
}

bool wxListCtrl::Arrange(int WXUNUSED(flag))
{
    return false;
}

bool wxListCtrl::DeleteItem(long item)
{
    if ( item < 0 || item >= GetItemCount() )
        return false;

    m_model->removeRow(item, QModelIndex());

    wxListItem listItem;
    listItem.SetId(item);
    wxListEvent event(wxEVT_LIST_DELETE_ITEM, GetId());
    event.SetEventObject(this);
    event.SetItem(listItem);

    HandleWindowEvent(event);

    return true;
}

bool wxListCtrl::DeleteAllItems()
{
    if ( GetItemCount() == 0)
        return true;

    m_model->removeRows(0, GetItemCount(), QModelIndex());

    wxListEvent event(wxEVT_LIST_DELETE_ALL_ITEMS, GetId());
    event.SetEventObject(this);
    HandleWindowEvent(event);

    return true;
}

bool wxListCtrl::DeleteColumn(int col)
{
    if ( col < 0 || col >= m_model->columnCount(QModelIndex()) )
        return false;

    m_model->removeColumn(0, QModelIndex());
    return true;
}

bool wxListCtrl::DeleteAllColumns()
{
    m_model->removeColumns(0, m_model->columnCount(QModelIndex()), QModelIndex());
    return true;
}

void wxListCtrl::ClearAll()
{
    DeleteAllColumns();
    DeleteAllItems();
}

wxTextCtrl* wxListCtrl::EditLabel(long item, wxClassInfo* WXUNUSED(textControlClass))
{
    m_qtTreeWidget->openPersistentEditor(m_model->index(item,0));
    return m_qtTreeWidget->GetEditControl();
;
}

bool wxListCtrl::EndEditLabel(bool WXUNUSED(cancel))
{
    int item = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
    if (item < 0)
        return false;

    m_qtTreeWidget->closePersistentEditor(m_model->index(item,0));
    return true;
}

bool wxListCtrl::EnsureVisible(long item)
{
    if ( item < 0 || item >= GetItemCount() )
        return false;

    m_qtTreeWidget->scrollTo(m_model->index(item,0));
    return true;
}

long wxListCtrl::FindItem(long start, const wxString& str, bool partial)
{
    return m_model->FindItem(start, wxQtConvertString(str), partial);
}

long wxListCtrl::FindItem(long start, wxUIntPtr data)
{
    return m_model->FindItem(start, data);
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
    const long index =  m_model->InsertItem(info);

    wxListItem tmp =info;
    tmp.SetId(index);

    wxListEvent event(wxEVT_LIST_INSERT_ITEM, GetId());
    event.SetItem(tmp);
    event.SetEventObject(this);
    HandleWindowEvent(event);

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
    info.m_text = label;
    info.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE;
    info.m_image = imageIndex;
    info.m_itemId = index;
    return InsertItem(info);
}

long wxListCtrl::DoInsertColumn(long col, const wxListItem& info)
{
    return m_model->InsertColumn(col, info);
}


void wxListCtrl::SetItemCount(long count)
{
    wxASSERT( HasFlag(wxLC_VIRTUAL) );
    m_model->SetVirtualItemCount(count);
}

bool wxListCtrl::ScrollList(int dx, int dy)
{
    // aproximate, as scrollContentsBy is protected
    m_qtTreeWidget->scroll(dx, dy);
    return true;
}

bool wxListCtrl::SortItems(wxListCtrlCompare fn, wxIntPtr data)
{
    m_model->SortItems(fn, data);
    return true;
}

wxString wxListCtrl::OnGetItemText(long WXUNUSED(item), long WXUNUSED(col)) const
{
    // this is a pure virtual function, in fact - which is not really pure
    // because the controls which are not virtual don't need to implement it
    wxFAIL_MSG("wxListCtrl::OnGetItemText not supposed to be called");

    return wxEmptyString;
}

int wxListCtrl::OnGetItemImage(long WXUNUSED(item)) const
{
    wxCHECK_MSG(!GetImageList(wxIMAGE_LIST_SMALL),
                -1,
                "List control has an image list, OnGetItemImage or OnGetItemColumnImage should be overridden.");
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

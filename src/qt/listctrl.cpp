/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/listctrl.cpp
// Author:      Mariano Reingart, Peter Most
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QItemDelegate>
#include <QtWidgets/QStyledItemDelegate>

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
#endif // WX_PRECOMP

#include "wx/app.h"
#include "wx/listctrl.h"
#include "wx/imaglist.h"
#include "wx/recguard.h"

#include "wx/qt/private/winevent.h"
#include "wx/qt/private/treeitemfactory.h"

namespace
{

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

void InitListEvent(wxListEvent& event,
                   wxListCtrl* listctrl,
                   wxEventType eventType,
                   const QModelIndex& index = QModelIndex())
{
    event.SetEventObject(listctrl);
    event.SetEventType(eventType);
    event.SetId(listctrl->GetId());

    if ( index.isValid() )
    {
        event.m_itemIndex = index.row();
        event.m_col = index.column();
        event.m_item.SetId(event.m_itemIndex);
        event.m_item.SetMask(wxLIST_MASK_TEXT |
                             wxLIST_MASK_IMAGE |
                             wxLIST_MASK_DATA);
        listctrl->GetItem(event.m_item);
    }
}

} // anonymous namespace

class wxQtStyledItemDelegate : public QStyledItemDelegate
{
public:
    explicit wxQtStyledItemDelegate(wxWindow* parent)
        : m_parent(parent),
        m_textCtrl(nullptr)
    {
    }

    QWidget* createEditor(QWidget *parent,
                          const QStyleOptionViewItem &WXUNUSED(option),
                          const QModelIndex &index) const override
    {
        if (m_textCtrl != nullptr)
            destroyEditor(m_textCtrl->GetHandle(), m_currentModelIndex);

        m_currentModelIndex = index;
        m_textCtrl = new wxQtListTextCtrl(m_parent, parent);
        m_textCtrl->SetFocus();
        return m_textCtrl->GetHandle();
    }

    void destroyEditor(QWidget *WXUNUSED(editor),
                       const QModelIndex &WXUNUSED(index)) const override
    {
        if (m_textCtrl != nullptr)
        {
            m_currentModelIndex = QModelIndex(); // invalidate the index
            wxTheApp->ScheduleForDestruction(m_textCtrl);
            m_textCtrl = nullptr;
        }
    }

    void setModelData(QWidget *WXUNUSED(editor),
                      QAbstractItemModel *WXUNUSED(model),
                      const QModelIndex &WXUNUSED(index)) const override
    {
        // Don't set model data until wx has had a chance to send out events
    }

    wxTextCtrl* GetEditControl() const
    {
        return m_textCtrl;
    }

    QModelIndex GetCurrentModelIndex() const
    {
        return m_currentModelIndex;
    }

    void AcceptModelData(QWidget *editor,
                         QAbstractItemModel *model,
                         const QModelIndex &index) const
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }

private:
    wxWindow* m_parent;
    mutable wxTextCtrl* m_textCtrl;
    mutable QModelIndex m_currentModelIndex;
};

class wxQtListModel : public QAbstractTableModel
{
public:
    explicit wxQtListModel(wxListCtrl *listCtrl) :
        m_view(nullptr),
        m_listCtrl(listCtrl)
    {
    }

    int rowCount(const QModelIndex& WXUNUSED(parent)) const override
    {
        return static_cast<int>(m_rows.size());
    }
    int columnCount(const QModelIndex& WXUNUSED(parent)) const override
    {
        return static_cast<int>(m_headers.size());
    }

    QVariant data(const QModelIndex &index, int role) const override
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
                return QVariant::fromValue(columnItem.m_label);

            case Qt::EditRole:
                return QVariant::fromValue(columnItem.m_label);

            case Qt::DecorationRole:
            {
                wxImageList *imageList = GetImageList();
                if ( imageList == nullptr )
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
                return col == 0 && m_listCtrl->HasCheckBoxes()
                    ? rowItem.m_checked
                    : QVariant();

            default:
                return QVariant();
        }
    }

    bool setData(
        const QModelIndex &index,
        const QVariant &value,
        int role
    ) override
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
            m_rows[row][col].m_label = value.toString();
            return true;
        }

        if ( role == Qt::CheckStateRole && col == 0 )
        {
            m_rows[row].m_checked =
                static_cast<Qt::CheckState>(value.toUInt()) == Qt::Checked;

            wxListEvent event;
            InitListEvent(event,
                          m_listCtrl,
                          m_rows[row].m_checked ? wxEVT_LIST_ITEM_CHECKED
                                                : wxEVT_LIST_ITEM_UNCHECKED,
                          index);
            m_listCtrl->HandleWindowEvent(event);
            return true;
        }

        return false;
    }

    QVariant headerData(
        int section,
        Qt::Orientation orientation,
        int role
    ) const override
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

    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        Qt::ItemFlags
            itemFlags = Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;

        if ( m_listCtrl->HasFlag(wxLC_EDIT_LABELS) )
            itemFlags |= Qt::ItemIsEditable;

         if ( index.column() == 0 && m_listCtrl->HasCheckBoxes() )
            itemFlags |= Qt::ItemIsUserCheckable;

         return itemFlags | QAbstractTableModel::flags(index);
    }

    bool removeRows(int row, int count, const QModelIndex &parent) override
    {
        if ( count == 0 )
            return true;

        beginRemoveRows(parent, row, row + count - 1);
        eraseFromContainer(m_rows, row, count);
        endRemoveRows();
        return true;
    }

    bool removeColumns(int column,
                       int count,
                       const QModelIndex &parent) override
    {
        if ( count == 0 )
            return true;

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

    bool SetColumn(int index, const wxListItem& info)
    {
        wxCHECK_MSG(static_cast<size_t>(index) < m_headers.size(),
            false, "Invalid column");

        ColumnItem &column = m_headers[index];
        column.m_label = wxQtConvertString(info.GetText());
        column.m_align = wxQtConvertTextAlign(info.GetAlign());

        headerDataChanged(Qt::Horizontal, index, index);
        return true;

    }

    virtual bool GetItem(wxListItem& info)
    {
        const int row = static_cast<int>(info.GetId());
        const int col = info.m_col;

        wxCHECK_MSG(
            row >= 0 && static_cast<size_t>(row) < m_rows.size(),
            false,
            "Invalid row"
        );

        wxCHECK_MSG(
            col >= 0 && static_cast<size_t>(col) < m_rows[row].m_columns.size(),
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

        CopySelectStatusToItem(info, row, col);

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

        if ( (info.m_mask & wxLIST_MASK_TEXT) && !info.GetText().empty() )
        {
            columnItem.m_label =  wxQtConvertString(info.GetText());
            roles.push_back(Qt::DisplayRole);
        }

        if ( info.m_mask & wxLIST_MASK_FORMAT )
        {
            columnItem.m_align = wxQtConvertTextAlign(info.GetAlign());
            roles.push_back(Qt::TextAlignmentRole);
        }

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


    long InsertItem(const wxListItem& info)
    {
        const int column = info.GetColumn();
        wxCHECK_MSG(column >= 0, -1, "Invalid column index");

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

        wxListItem newItem = info;
        newItem.SetId(newRowIndex);
        SetItem(newItem);

        endInsertRows();

        return newRowIndex;
    }

    long InsertColumn(long col, const wxListItem& info)
    {
        long newColumnIndex;

        ColumnItem newColumn;
        if ( info.m_mask & wxLIST_MASK_FORMAT )
        {
            newColumn.m_align = wxQtConvertTextAlign(info.GetAlign());
        }
        newColumn.m_label = wxQtConvertString(info.GetText());

        if ( col == -1 || static_cast<size_t>(col) >= m_headers.size() )
        {
            newColumnIndex = m_headers.empty() ? 0 : m_headers.size();
        }
        else
        {
            newColumnIndex = col;
        }

        beginInsertColumns(QModelIndex(), newColumnIndex, newColumnIndex);

        std::vector<ColumnItem>::iterator it = m_headers.begin();
        std::advance(it, newColumnIndex);
        m_headers.insert(it, newColumn);

        const int numberOfRows = m_rows.size();

        for (int i = 0; i < numberOfRows; ++i )
        {
            it = m_rows[i].m_columns.begin();
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
        const QModelIndex modelIndex = index(item, 0);
        dataChanged(modelIndex, modelIndex, roles);
    }

    virtual bool IsVirtual() const
    {
        return false;
    }

    virtual void SetVirtualItemCount(long WXUNUSED(count))
    {
        wxFAIL_MSG("Shouldn't be called if wxLC_VIRTUAL is not used");
    }

protected:
    wxImageList *GetImageList() const
    {
        const int requiredList = m_listCtrl->HasFlag(wxLC_SMALL_ICON)
            ? wxIMAGE_LIST_SMALL
            : wxIMAGE_LIST_NORMAL;
        return m_listCtrl->GetImageList(requiredList);
    }

    wxListCtrl *GetListCtrl() const
    {
        return m_listCtrl;
    }

    bool IsSelected(const QModelIndex& index) const
    {
        QModelIndexList indices = m_view->selectionModel()->selectedIndexes();
        return indices.contains(index);
    }

    void CopySelectStatusToItem(wxListItem& info, int row, int col)
    {
        if ( info.m_mask & wxLIST_MASK_STATE )
        {
            info.m_state = wxLIST_STATE_DONTCARE;
            if (info.m_stateMask & wxLIST_STATE_FOCUSED)
            {
                if (m_view->currentIndex().row() == row)
                    info.m_state |= wxLIST_STATE_FOCUSED;
            }
            if (info.m_stateMask & wxLIST_STATE_SELECTED)
            {
                if (IsSelected(index(row, col)))
                    info.m_state |= wxLIST_STATE_SELECTED;
            }
        }
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
            m_data(nullptr),
            m_checked(false)
        {
        }

        RowItem(int columnCount ) :
            m_columns(columnCount),
            m_data(nullptr),
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

        bool operator()(const RowItem &lhs, const RowItem &rhs) const
        {
            return m_fn(
                reinterpret_cast<wxIntPtr>(lhs.m_data),
                reinterpret_cast<wxIntPtr>(rhs.m_data),
                m_data) < 0;
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

    int rowCount(const QModelIndex& WXUNUSED(parent)) const override
    {
        return m_rowCount;
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        wxListCtrl *listCtrl = GetListCtrl();

        const int row = index.row();
        const int col = index.column();

        if ( role == Qt::DisplayRole || role == Qt::EditRole )
        {
            const wxString text = listCtrl->OnGetItemText(row, col);
            return QVariant::fromValue(wxQtConvertString(text));
        }

        if ( role == Qt::DecorationRole )
        {
            wxImageList *imageList = GetImageList();
            if ( imageList == nullptr )
                return QVariant();

            const int imageIndex = listCtrl->OnGetItemColumnImage(row, col);
            if ( imageIndex == -1 )
                return QVariant();
            wxBitmap image = imageList->GetBitmap(imageIndex);
            wxCHECK_MSG(image.IsOk(), QVariant(), "Invalid Bitmap");
            return QVariant::fromValue(*image.GetHandle());
        }

        return QVariant();
    }

    bool GetItem(wxListItem& info) override
    {
        const int row = static_cast<int>(info.GetId());
        const int col = info.m_col;

        if ( info.m_mask & wxLIST_MASK_TEXT )
            info.SetText(GetListCtrl()->OnGetItemText(row, col));

        CopySelectStatusToItem(info, row, col);

        return true;
    }

    bool IsVirtual() const override
    {
        return true;
    }

    void SetVirtualItemCount(long count) override
    {
        beginResetModel();
        m_rowCount = static_cast<int>(count);
        endResetModel();
    }
private:
    int m_rowCount;
};


class wxQtListTreeWidget : public wxQtEventSignalHandler< QTreeView, wxListCtrl >
{
public:
    wxQtListTreeWidget( wxWindow *parent, wxListCtrl *handler );

    void EmitListEvent(wxEventType typ, const QModelIndex &index) const;

    void closeEditor(
        QWidget *editor,
        QAbstractItemDelegate::EndEditHint hint
    ) override
    {
        // Close process can re-signal closeEditor so we need to guard against
        // reentrant calls.
        wxRecursionGuard guard(m_closingEditor);

        if (guard.IsInside())
            return;

        // There can be multiple calls to close editor when the item loses focus
        const QModelIndex current_index = m_itemDelegate.GetCurrentModelIndex();
        if (!current_index.isValid())
            return;

        const wxString editedText = m_itemDelegate.GetEditControl()->GetLineText(0);

        wxListEvent event;
        InitListEvent(event,
                      GetHandler(),
                      wxEVT_LIST_END_LABEL_EDIT,
                      current_index);
        event.m_item.SetText(editedText);

        if (hint == QAbstractItemDelegate::RevertModelCache)
        {
            event.SetEditCanceled(true);
            EmitEvent(event);
        }
        else
        {
            // Allow event handlers to decide whether to accept edited text
            if (!GetHandler()->HandleWindowEvent(event) || event.IsAllowed())
                m_itemDelegate.AcceptModelData(editor, model(), current_index);
        }

        // wx doesn't have hints to edit next/previous item
        if ( hint == QAbstractItemDelegate::EditNextItem ||
             hint == QAbstractItemDelegate::EditPreviousItem )
        {
            hint = QAbstractItemDelegate::SubmitModelCache;
        }

        QTreeView::closeEditor(editor, hint);
        closePersistentEditor(current_index);
    }

    wxTextCtrl *GetEditControl()
    {
        return m_itemDelegate.GetEditControl();
    }

    virtual void paintEvent(QPaintEvent *event) override
    {
        QTreeView::paintEvent(event);
    }

    int GetHeaderHeight() const
    {
        return header() != nullptr ? header()->height() : 0;
    }

private:
    void itemClicked(const QModelIndex &index);
    void itemActivated(const QModelIndex &index);
    void itemPressed(const QModelIndex &index);

    wxQtStyledItemDelegate m_itemDelegate;
    wxRecursionGuardFlag m_closingEditor;
};

wxQtListTreeWidget::wxQtListTreeWidget( wxWindow *parent, wxListCtrl *handler )
    : wxQtEventSignalHandler< QTreeView, wxListCtrl >( parent, handler ),
    m_itemDelegate(handler),
    m_closingEditor(0)
{
    connect(this, &QTreeView::clicked, this, &wxQtListTreeWidget::itemClicked);
    connect(this, &QTreeView::pressed, this, &wxQtListTreeWidget::itemPressed);
    connect(this, &QTreeView::activated, this, &wxQtListTreeWidget::itemActivated);

    setItemDelegate(&m_itemDelegate);
    setEditTriggers(NoEditTriggers);
}

void wxQtListTreeWidget::EmitListEvent(wxEventType typ,
                                       const QModelIndex &index) const
{
    wxListCtrl *handler = GetHandler();
    if ( handler )
    {
        // prepare the event
        // -----------------
        wxListEvent event;
        InitListEvent(event, handler, typ, index);
        EmitEvent(event);
    }
}

void wxQtListTreeWidget::itemClicked(const QModelIndex &index)
{
    EmitListEvent(wxEVT_LIST_ITEM_SELECTED, index);
}

void wxQtListTreeWidget::itemPressed(const QModelIndex &index)
{
    EmitListEvent(wxEVT_LIST_ITEM_SELECTED, index);
}

void wxQtListTreeWidget::itemActivated(const QModelIndex &index)
{
    EmitListEvent(wxEVT_LIST_ITEM_ACTIVATED, index);
}

// Specialization: to safely remove and delete the model associated with QTreeView
template<>
void wxQtEventSignalHandler< QTreeView, wxListCtrl >::HandleDestroyedSignal()
{
    // This handler is emitted immediately before the QTreeView obj is destroyed
    // at which point the parent object (wxListCtrl) pointer is guaranteed to still
    // be valid for the model to be safely removed.
    this->setModel(nullptr);
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
    m_model = style & wxLC_VIRTUAL
        ? new wxQtVirtualListModel(this)
        : new wxQtListModel(this);

    m_qtTreeWidget = new wxQtListTreeWidget(parent, this);
    m_qtTreeWidget->setModel(m_model);
    m_model->SetView(m_qtTreeWidget);

    if (style & wxLC_NO_HEADER)
        m_qtTreeWidget->setHeaderHidden(true);

    m_qtTreeWidget->setRootIsDecorated(false);
    m_qtTreeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    if ( !QtCreateControl(parent, id, pos, size, style, validator, name) )
        return false;

    SetWindowStyleFlag(style);
    return true;
}

void wxListCtrl::Init()
{
    m_hasCheckBoxes = false;
    m_model = nullptr;
    m_qtTreeWidget = nullptr;
}

wxListCtrl::~wxListCtrl()
{
    m_model->deleteLater();
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
    if ( !m_model->SetColumn(col, info) )
        return false;

    if ( info.GetMask() & wxLIST_MASK_WIDTH )
        SetColumnWidth(col, info.GetWidth());

    return true;
}

int wxListCtrl::GetColumnWidth(int col) const
{
    return m_qtTreeWidget->columnWidth(col);
}

bool wxListCtrl::SetColumnWidth(int col, int width)
{
    if ( width < 0 )
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
    const int h = m_qtTreeWidget->visualRect(m_model->index(0, 0)).height();
    if ( h )
        return m_qtTreeWidget->height() / h;
    else
        return 0;
}

wxRect wxListCtrl::GetViewRect() const
{
    // this may not be exact but should be a good approximation:
    wxRect rect = wxQtConvertRect(m_qtTreeWidget->rect());
    const int h = m_qtTreeWidget->header()->defaultSectionSize();
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

int wxListCtrl::GetItemState(long item, long stateMask) const
{
    wxListItem info;

    info.m_mask = wxLIST_MASK_STATE;
    info.m_stateMask = stateMask;
    info.m_itemId = item;

    if ( !GetItem(info) )
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
    if ( columnCount == 0 )
        return false;

    // Calculate the union of the bounds of the items in the first and last
    // column for the given row.
    QRect first = m_qtTreeWidget->visualRect(m_model->index(item, 0));
    QRect last = m_qtTreeWidget->visualRect(m_model->index(item, columnCount-1));
    rect = wxQtConvertRect(first.united(last));
    rect.Offset(0, m_qtTreeWidget->GetHeaderHeight());

    return true;
}

bool wxListCtrl::GetSubItemRect(long item,
                                long subItem,
                                wxRect& rect,
                                int WXUNUSED(code)) const
{
    wxCHECK_MSG(item >= 0 && item < GetItemCount(),
        false, "invalid row index in GetSubItemRect");

    wxCHECK_MSG(subItem >= 0 && subItem < GetColumnCount(),
        false, "invalid column index in GetSubItemRect");

    const QModelIndex index = m_qtTreeWidget->model()->index(item, subItem);
    rect = wxQtConvertRect(m_qtTreeWidget->visualRect(index));
    rect.Offset(0, m_qtTreeWidget->GetHeaderHeight());
    return true;
}

bool wxListCtrl::GetItemPosition(long item, wxPoint& pos) const
{
    wxRect rect;
    if ( !GetItemRect(item, rect) )
        return false;

    pos.x = rect.x;
    pos.y = rect.y;

    return true;
}

bool wxListCtrl::SetItemPosition(long WXUNUSED(item),
                                 const wxPoint& WXUNUSED(pos))
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

void wxListCtrl::SetItemTextColour(long item, const wxColour& col)
{
    const int columnCount = m_model->columnCount(QModelIndex());

    wxListItem listItem;
    listItem.SetId(item);
    listItem.SetTextColour(col);

    for ( int i = 0; i < columnCount; ++i )
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

    for ( int i = 0; i < columnCount; ++i )
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

    for ( int i = 0; i < columnCount; ++i )
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
    QModelIndexList selectedRows = selectionModel->selectedRows();
    return selectedRows.length();
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
    m_model->dataChanged(m_model->index(0, 0),
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
    m_windowStyle = style;
    m_qtTreeWidget->setHeaderHidden((style & wxLC_NO_HEADER) != 0);
    m_qtTreeWidget->setSelectionMode((style & wxLC_SINGLE_SEL) != 0
        ? QAbstractItemView::SingleSelection
        : QAbstractItemView::ExtendedSelection
    );
    const bool needVirtual = (style & wxLC_VIRTUAL) != 0;

    if ( needVirtual != m_model->IsVirtual() )
    {
        wxQtListModel *oldModel = m_model;
        m_model = needVirtual
            ? new wxQtVirtualListModel(this)
            : new wxQtListModel(this);
        m_model->SetView(m_qtTreeWidget);
        m_qtTreeWidget->setModel(m_model);
        delete oldModel;
    }
}

long wxListCtrl::GetNextItem(long item, int WXUNUSED(geometry), int state) const
{
    wxListItem info;
    long ret = item;
    const long max = GetItemCount();
    wxCHECK_MSG((ret >= -1) || (ret < max), -1,
                 "invalid listctrl index in GetNextItem()");

    // notice that we start with the next item (or the first one if item == -1)
    // and this is intentional to allow writing a simple loop to iterate over
    // all selected items
    ret++;
    if ( ret == max )
        // this is not an error because the index was OK initially,
        // just no such item
        return -1;

    if ( state == wxLIST_STATE_DONTCARE )
        return ret;

    for ( long line = ret; line < max; line++ )
    {
        if ( GetItemState(line, state) )
            return line;
    }

    return -1;
}

void wxListCtrl::DoUpdateImages(int WXUNUSED(which))
{
    // TODO: Ensure the icons are actually updated.
}

void wxListCtrl::RefreshItem(long item)
{
    RefreshItems(item, item);
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

    wxListEvent event;
    InitListEvent(event, this, wxEVT_LIST_DELETE_ITEM);
    event.m_item.SetId(item);

    HandleWindowEvent(event);

    return true;
}

bool wxListCtrl::DeleteAllItems()
{
    if ( GetItemCount() == 0 )
        return true;

    m_model->removeRows(0, GetItemCount(), QModelIndex());

    wxListEvent event;
    InitListEvent(event, this, wxEVT_LIST_DELETE_ALL_ITEMS);

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
    m_model->removeColumns(0,
                           m_model->columnCount(QModelIndex()),
                           QModelIndex());
    return true;
}

void wxListCtrl::ClearAll()
{
    DeleteAllColumns();
    DeleteAllItems();
}

wxTextCtrl* wxListCtrl::EditLabel(long item,
                                  wxClassInfo* WXUNUSED(textControlClass))
{
    // Open the editor first so that it's available when handling events as per
    // wx standard.
    const QModelIndex index = m_model->index(item, 0);
    m_qtTreeWidget->openPersistentEditor(index);

    wxListEvent event;
    InitListEvent(event, this, wxEVT_LIST_BEGIN_LABEL_EDIT, index);

    // close the editor again if event is vetoed
    if (HandleWindowEvent(event) && !event.IsAllowed())
        m_qtTreeWidget->closePersistentEditor(index);

    return m_qtTreeWidget->GetEditControl();
}

bool wxListCtrl::EndEditLabel(bool WXUNUSED(cancel))
{
    const int item = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
    if ( item < 0 )
        return false;

    m_qtTreeWidget->closePersistentEditor(m_model->index(item, 0));
    return true;
}

bool wxListCtrl::EnsureVisible(long item)
{
    if ( item < 0 || item >= GetItemCount() )
        return false;

    m_qtTreeWidget->scrollTo(m_model->index(item, 0));
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

long wxListCtrl::FindItem(
    long WXUNUSED(start),
    const wxPoint& WXUNUSED(pt),
    int WXUNUSED(direction)
)
{
    return -1;
}

long wxListCtrl::HitTest(
    const wxPoint& point,
    int &flags,
    long* ptrSubItem
) const
{
    // Remove the header height as qt expects point relative to the table sub widget
    QPoint qPoint = wxQtConvertPoint(point);
    qPoint.setY(qPoint.y() - m_qtTreeWidget->GetHeaderHeight());

    QModelIndex index = m_qtTreeWidget->indexAt(qPoint);
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

    wxListEvent event;
    InitListEvent(event, this, wxEVT_LIST_INSERT_ITEM);

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
    m_model->SetVirtualItemCount(count);
}

bool wxListCtrl::ScrollList(int dx, int dy)
{
    // approximate, as scrollContentsBy is protected
    m_qtTreeWidget->scroll(dx, dy);
    return true;
}

bool wxListCtrl::SortItems(wxListCtrlCompare fn, wxIntPtr data)
{
    m_model->SortItems(fn, data);
    return true;
}

QWidget *wxListCtrl::GetHandle() const
{
    return m_qtTreeWidget;
}

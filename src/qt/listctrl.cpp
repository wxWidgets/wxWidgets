/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/listctrl.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/listctrl.h"
#include "wx/qt/private/winevent.h"


class wxQtTreeWidget : public wxQtEventSignalHandler< QTreeWidget, wxListCtrl >
{
public:
    wxQtTreeWidget( wxWindow *parent, wxListCtrl *handler );

    void EmitListEvent(wxEventType typ, QTreeWidgetItem *item, int column) const;

private:
    void itemClicked(QTreeWidgetItem * item, int column);
    void itemActivated(QTreeWidgetItem * item, int column);
    void itemPressed(QTreeWidgetItem * item, int column);
};

wxQtTreeWidget::wxQtTreeWidget( wxWindow *parent, wxListCtrl *handler )
    : wxQtEventSignalHandler< QTreeWidget, wxListCtrl >( parent, handler )
{
    connect(this, &QTreeWidget::itemClicked, this, &wxQtTreeWidget::itemClicked);
    connect(this, &QTreeWidget::itemPressed, this, &wxQtTreeWidget::itemPressed);
    connect(this, &QTreeWidget::itemActivated, this, &wxQtTreeWidget::itemActivated);
}

void wxQtTreeWidget::EmitListEvent(wxEventType typ, QTreeWidgetItem *item, int column) const
{
    wxListCtrl *handler = GetHandler();
    if ( handler )
    {
        // prepare the event
        // -----------------
        wxListEvent event;
        event.SetEventType(typ);
        event.SetId(handler->GetId());
        event.m_itemIndex = this->indexFromItem(item, column).row();
        event.m_item.SetId(event.m_itemIndex);
        event.m_item.SetMask(wxLIST_MASK_TEXT |
                             wxLIST_MASK_IMAGE |
                             wxLIST_MASK_DATA);
        handler->GetItem(event.m_item);
        EmitEvent(event);
    }
}

void wxQtTreeWidget::itemClicked(QTreeWidgetItem *item, int column)
{
    EmitListEvent(wxEVT_LIST_ITEM_SELECTED, item, column);
}

void wxQtTreeWidget::itemPressed(QTreeWidgetItem *item, int column)
{
    EmitListEvent(wxEVT_LIST_ITEM_SELECTED, item, column);
}

void wxQtTreeWidget::itemActivated(QTreeWidgetItem *item, int column)
{
    EmitListEvent(wxEVT_LIST_ITEM_ACTIVATED, item, column);
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
}


wxListCtrl::wxListCtrl()
{
}

wxListCtrl::wxListCtrl(wxWindow *parent,
           wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxValidator& validator,
           const wxString& name)
{
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

    return QtCreateControl( parent, id, pos, size, style, validator, name );
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
    QTreeWidgetItem *item = m_qtTreeWidget->headerItem();
    info.SetText(wxQtConvertString(item->text(col)));
    info.SetAlign(wxQtConvertAlignFlag(item->textAlignment(col)));
    info.SetWidth(m_qtTreeWidget->columnWidth(col));
    return true;
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

bool wxListCtrl::SetColumnsOrder(const wxArrayInt& orders)
{
    return false;
}

int wxListCtrl::GetCountPerPage() const
{
    return 0;
}

wxRect wxListCtrl::GetViewRect() const
{
    return wxRect();
}

wxTextCtrl* wxListCtrl::GetEditControl() const
{
    return NULL;
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
    QTreeWidgetItem *item = QtGetItem(id);
    info.SetText(wxQtConvertString(item->text(info.GetColumn())));
    return true;
}

bool wxListCtrl::SetItem(wxListItem& info)
{
    const long id = info.GetId();
    QTreeWidgetItem *item = QtGetItem(id);
    if ( !info.GetText().IsNull() )
        item->setText(info.GetColumn(), wxQtConvertString(info.GetText()));
    item->setTextAlignment(info.GetColumn(), wxQtConvertTextAlign(info.GetAlign()));

    for (int col=0; col<GetColumnCount(); col++)
    {
        if ( info.GetFont().IsOk() )
            item->setFont(col, info.GetFont().GetHandle() );
        if ( info.GetTextColour().IsOk() )
            item->setTextColor(col, info.GetTextColour().GetHandle());
        if ( info.GetBackgroundColour().IsOk() )
            item->setBackgroundColor(col, info.GetBackgroundColour().GetHandle());
    }
    return true;
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
    return 0;
}

bool wxListCtrl::SetItemState(long item, long state, long stateMask)
{
    return false;
}

bool wxListCtrl::SetItemImage(long item, int image, int selImage)
{
    return false;
}

bool wxListCtrl::SetItemColumnImage(long item, long column, int image)
{
    return false;
}

wxString wxListCtrl::GetItemText(long item, int col) const
{
    QTreeWidgetItem *qitem = QtGetItem(item);
    if ( qitem )
        return wxQtConvertString( qitem->text(0) );
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
    return wxUIntPtr();
}

bool wxListCtrl::SetItemPtrData(long item, wxUIntPtr data)
{
    return false;
}

bool wxListCtrl::SetItemData(long item, long data)
{
    return false;
}

bool wxListCtrl::GetItemRect(long item, wxRect& rect, int code) const
{
    return false;
}

bool wxListCtrl::GetSubItemRect(long item, long subItem, wxRect& rect, int code) const
{
    return false;
}

bool wxListCtrl::GetItemPosition(long item, wxPoint& pos) const
{
    return false;
}

bool wxListCtrl::SetItemPosition(long item, const wxPoint& pos)
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

void wxListCtrl::SetItemTextColour( long item, const wxColour& col)
{
}

wxColour wxListCtrl::GetItemTextColour( long item ) const
{
    return wxColour();
}

void wxListCtrl::SetItemBackgroundColour( long item, const wxColour &col)
{
}

wxColour wxListCtrl::GetItemBackgroundColour( long item ) const
{
    return wxColour();
}

void wxListCtrl::SetItemFont( long item, const wxFont &f)
{
}

wxFont wxListCtrl::GetItemFont( long item ) const
{
    return wxFont();
}

int wxListCtrl::GetSelectedItemCount() const
{
    return 0;
}

wxColour wxListCtrl::GetTextColour() const
{
    return wxColour();
}

void wxListCtrl::SetTextColour(const wxColour& col)
{
}

long wxListCtrl::GetTopItem() const
{
    return 0;
}

void wxListCtrl::SetSingleStyle(long style, bool add)
{
}

void wxListCtrl::SetWindowStyleFlag(long style)
{
}

long wxListCtrl::GetNextItem(long item, int geometry, int state) const
{
    return 0;
}

wxImageList *wxListCtrl::GetImageList(int which) const
{
    return NULL;
}


void wxListCtrl::SetImageList(wxImageList *imageList, int which)
{
}

void wxListCtrl::AssignImageList(wxImageList *imageList, int which)
{
}

bool wxListCtrl::InReportView() const
{
    return true;
}

bool wxListCtrl::IsVirtual() const
{
    return false;
}

void wxListCtrl::RefreshItem(long item)
{
}

void wxListCtrl::RefreshItems(long itemFrom, long itemTo)
{
}

bool wxListCtrl::Arrange(int flag)
{
    return false;
}

bool wxListCtrl::DeleteItem(long item)
{
    return false;
}

bool wxListCtrl::DeleteAllItems()
{
    return false;
}

bool wxListCtrl::DeleteColumn(int col)
{
    return false;
}

bool wxListCtrl::DeleteAllColumns()
{
    return false;
}

void wxListCtrl::ClearAll()
{
    m_qtTreeWidget->clear();
}

wxTextCtrl* wxListCtrl::EditLabel(long item, wxClassInfo* textControlClass)
{
    return NULL;
}

bool wxListCtrl::EndEditLabel(bool cancel)
{
    return false;
}

bool wxListCtrl::EnsureVisible(long item)
{
    return false;
}

long wxListCtrl::FindItem(long start, const wxString& str, bool partial)
{
    return 0;
}

long wxListCtrl::FindItem(long start, wxUIntPtr data)
{
    return 0;
}

long wxListCtrl::FindItem(long start, const wxPoint& pt, int direction)
{
    return 0;
}

long wxListCtrl::HitTest(const wxPoint& point, int& flags, long* ptrSubItem) const
{
    return 0;
}

long wxListCtrl::InsertItem(const wxListItem& info)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(m_qtTreeWidget);
    item->setText(info.GetColumn(), wxQtConvertString(info.GetText()));
    item->setTextAlignment(info.GetColumn(), wxQtConvertTextAlign(info.GetAlign()));
    for (int col=0; col<GetColumnCount();col++)
    {
        if ( info.GetFont().IsOk() )
            item->setFont(col, info.GetFont().GetHandle() );
        if ( info.GetTextColour().IsOk() )
            item->setTextColor(col, info.GetTextColour().GetHandle());
        if ( info.GetBackgroundColour().IsOk() )
            item->setBackgroundColor(col, info.GetBackgroundColour().GetHandle());
    }
    return GetItemCount() - 1;
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
    info.m_itemId = index;
    return InsertItem(info);
}

long wxListCtrl::DoInsertColumn(long col, const wxListItem& info)
{
    QTreeWidgetItem *item = m_qtTreeWidget->headerItem();
    item->setText(col, wxQtConvertString(info.GetText()));
    item->setTextAlignment(col, wxQtConvertTextAlign(info.GetAlign()));
    if (info.GetWidth())
        m_qtTreeWidget->setColumnWidth(col, info.GetWidth());
    return col;
}


void wxListCtrl::SetItemCount(long count)
{
}

bool wxListCtrl::ScrollList(int dx, int dy)
{
    return false;
}

bool wxListCtrl::SortItems(wxListCtrlCompare fn, wxIntPtr data)
{
    return false;
}


QTreeWidget *wxListCtrl::GetHandle() const
{
    return m_qtTreeWidget;
}

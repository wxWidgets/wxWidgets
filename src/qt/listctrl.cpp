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

private:
    void clicked( const QModelIndex &index );
    void doubleClicked( const QModelIndex &index );
};

wxQtTreeWidget::wxQtTreeWidget( wxWindow *parent, wxListCtrl *handler )
    : wxQtEventSignalHandler< QTreeWidget, wxListCtrl >( parent, handler )
{
    connect(this, &QTreeWidget::clicked, this, &wxQtTreeWidget::clicked);
    connect(this, &QTreeWidget::doubleClicked, this, &wxQtTreeWidget::doubleClicked);
}

void wxQtTreeWidget::clicked(const QModelIndex &index )
{
    wxListCtrl *handler = GetHandler();
    //if ( handler )
    //    handler->QtSendEvent(wxEVT_LISTBOX, index, true);
}

void wxQtTreeWidget::doubleClicked( const QModelIndex &index )
{
    wxListCtrl *handler = GetHandler();
    //if ( handler )
    //    handler->QtSendEvent(wxEVT_LISTBOX_DCLICK, index, true);
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
    m_qtWindow = m_qtTreeWidget = new wxQtTreeWidget( parent, this );

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

bool wxListCtrl::SetForegroundColour(const wxColour& col)
{
    return false;
}

bool wxListCtrl::SetBackgroundColour(const wxColour& col)
{
    return false;
}

bool wxListCtrl::GetColumn(int col, wxListItem& item) const
{
    return false;
}

bool wxListCtrl::SetColumn(int col, const wxListItem& item)
{
    return false;
}

int wxListCtrl::GetColumnWidth(int col) const
{
    return 0;
}

bool wxListCtrl::SetColumnWidth(int col, int width)
{
    return false;
}

int wxListCtrl::GetColumnOrder(int col) const
{
    return 0;
}

int wxListCtrl::GetColumnIndexFromOrder(int order) const
{
    return 0;
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

bool wxListCtrl::GetItem(wxListItem& info) const
{
    return false;
}

bool wxListCtrl::SetItem(wxListItem& info)
{
    return false;
}

long wxListCtrl::SetItem(long index, int col, const wxString& label, int imageId)
{
    return 0;
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
    return wxString();
}

void wxListCtrl::SetItemText(long item, const wxString& str)
{
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
    return 0;
}

int wxListCtrl::GetColumnCount() const
{
    return 0;
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
    return false;
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

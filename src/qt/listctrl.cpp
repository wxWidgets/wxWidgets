/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/listctrl.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/listctrl.h"

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
    m_qtListWidget = new QListWidget( parent->GetHandle() );

    return wxControl::Create( parent, id, pos, size, style, validator, name );
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
    return 0;
}

long wxListCtrl::InsertItem(long index, const wxString& label)
{
    return 0;
}

long wxListCtrl::InsertItem(long index, int imageIndex)
{
    return 0;
}

long wxListCtrl::InsertItem(long index, const wxString& label, int imageIndex)
{
    return 0;
}

long wxListCtrl::InsertColumn(long col, const wxListItem& info)
{
    return 0;
}

long wxListCtrl::InsertColumn(long col,
                  const wxString& heading,
                  int format,
                  int width)
{
    return 0;
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

#if WXWIN_COMPATIBILITY_2_6
int wxListCtrl::GetItemSpacing(bool isSmall) const
{
    return 0;
}

#endif // WXWIN_COMPATIBILITY_2_6


QListWidget *wxListCtrl::GetHandle() const
{
    return m_qtListWidget;
}

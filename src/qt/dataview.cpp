/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dataview.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_DATAVIEWCTRL

#include "wx/dataview.h"

#ifndef wxHAS_GENERIC_DATAVIEWCTRL

wxDataViewColumn::wxDataViewColumn( const wxString &title, wxDataViewRenderer *renderer,
                  unsigned int model_column, int width,
                  wxAlignment align,
                  int flags )
    : wxDataViewColumnBase( renderer, model_column )
{
}

wxDataViewColumn::wxDataViewColumn( const wxBitmapBundle &bitmap, wxDataViewRenderer *renderer,
                  unsigned int model_column, int width,
                  wxAlignment align,
                  int flags )
    : wxDataViewColumnBase( bitmap, renderer, model_column )
{
}

void wxDataViewColumn::SetTitle( const wxString &title )
{
}

void wxDataViewColumn::SetBitmap( const wxBitmapBundle &bitmap )
{
}

void wxDataViewColumn::SetOwner( wxDataViewCtrl *owner )
{
}

void wxDataViewColumn::SetAlignment( wxAlignment align )
{
}

void wxDataViewColumn::SetSortable( bool sortable )
{
}

void wxDataViewColumn::SetSortOrder( bool ascending )
{
}

void wxDataViewColumn::SetAsSortKey(bool sort)
{
}

void wxDataViewColumn::SetResizeable( bool resizeable )
{
}

void wxDataViewColumn::SetHidden( bool hidden )
{
}

void wxDataViewColumn::SetMinWidth( int minWidth )
{
}

void wxDataViewColumn::SetWidth( int width )
{
}

void wxDataViewColumn::SetReorderable( bool reorderable )
{
}

void wxDataViewColumn::SetFlags(int flags)
{
}

wxString wxDataViewColumn::GetTitle() const
{
    return wxString();
}

wxAlignment wxDataViewColumn::GetAlignment() const
{
    return wxAlignment();
}

bool wxDataViewColumn::IsSortable() const
{
    return false;
}

bool wxDataViewColumn::IsSortOrderAscending() const
{
    return false;
}

bool wxDataViewColumn::IsSortKey() const
{
    return false;
}

bool wxDataViewColumn::IsResizeable() const
{
    return false;
}

bool wxDataViewColumn::IsHidden() const
{
    return false;
}

int wxDataViewColumn::GetWidth() const
{
    return 0;
}

int wxDataViewColumn::GetMinWidth() const
{
    return 0;
}

bool wxDataViewColumn::IsReorderable() const
{
    return false;
}

int wxDataViewColumn::GetFlags() const
{
    return 0;
}

//##############################################################################


wxDataViewCtrl::wxDataViewCtrl( wxWindow *parent, wxWindowID id,
       const wxPoint& pos,
       const wxSize& size, long style,
       const wxValidator& validator)
{
}

wxDataViewCtrl::~wxDataViewCtrl()
{
}



bool wxDataViewCtrl::Create(wxWindow *parent, wxWindowID id,
       const wxPoint& pos,
       const wxSize& size, long style,
       const wxValidator& validator)
{
    return false;
}

bool wxDataViewCtrl::AssociateModel( wxDataViewModel *model )
{
    return false;
}

bool wxDataViewCtrl::PrependColumn( wxDataViewColumn *col )
{
    return false;
}

bool wxDataViewCtrl::AppendColumn( wxDataViewColumn *col )
{
    return false;
}

bool wxDataViewCtrl::InsertColumn( unsigned int pos, wxDataViewColumn *col )
{
    return false;
}

unsigned wxDataViewCtrl::GetColumnCount() const
{
    return 0;
}

wxDataViewColumn* wxDataViewCtrl::GetColumn( unsigned int pos ) const
{
    return nullptr;
}

bool wxDataViewCtrl::DeleteColumn( wxDataViewColumn *column )
{
    return false;
}

bool wxDataViewCtrl::ClearColumns()
{
    return false;
}

int wxDataViewCtrl::GetColumnPosition( const wxDataViewColumn *column ) const
{
    return 0;
}

wxDataViewColumn *wxDataViewCtrl::GetSortingColumn() const
{
    return nullptr;
}

wxDataViewItem wxDataViewCtrl::GetSelection() const
{
    return wxDataViewItem();
}

int wxDataViewCtrl::GetSelections( wxDataViewItemArray & sel ) const
{
    return 0;
}

void wxDataViewCtrl::SetSelections( const wxDataViewItemArray & sel )
{
}

void wxDataViewCtrl::Select( const wxDataViewItem & item )
{
}

void wxDataViewCtrl::Unselect( const wxDataViewItem & item )
{
}

bool wxDataViewCtrl::IsSelected( const wxDataViewItem & item ) const
{
    return false;
}

void wxDataViewCtrl::SelectAll()
{
}

void wxDataViewCtrl::UnselectAll()
{
}

void wxDataViewCtrl::EnsureVisible( const wxDataViewItem& item,
                            const wxDataViewColumn *column)
{
}

void wxDataViewCtrl::HitTest( const wxPoint &point,
                      wxDataViewItem &item,
                      wxDataViewColumn *&column ) const
{
}

wxRect wxDataViewCtrl::GetItemRect( const wxDataViewItem &item,
                            const wxDataViewColumn *column) const
{
    return wxRect();
}

void wxDataViewCtrl::DoExpand( const wxDataViewItem & item, bool expandChildren )
{
}

void wxDataViewCtrl::Collapse( const wxDataViewItem & item )
{
}

bool wxDataViewCtrl::IsExpanded( const wxDataViewItem & item ) const
{
    return false;
}

bool wxDataViewCtrl::EnableDragSource( const wxDataFormat &format )
{
    return false;
}

bool wxDataViewCtrl::DoEnableDropTarget( const wxVector<wxDataFormat> &formats )
{
    return false;
}

wxVisualAttributes wxDataViewCtrl::GetClassDefaultAttributes(wxWindowVariant variant)
{
    return wxVisualAttributes();
}

void wxDataViewCtrl::OnInternalIdle()
{
}

void wxDataViewCtrl::DoSetExpanderColumn()
{
}

void wxDataViewCtrl::DoSetIndent()
{
}

wxDataViewItem wxDataViewCtrl::DoGetCurrentItem() const
{
    return wxDataViewItem();
}

void wxDataViewCtrl::DoSetCurrentItem(const wxDataViewItem& item)
{
}

#endif // !wxHAS_GENERIC_DATAVIEWCTRL

#endif // wxUSE_DATAVIEWCTRL

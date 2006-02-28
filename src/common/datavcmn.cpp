/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/datavcmn.cpp
// Purpose:     wxDataViewCtrl base classes and common parts
// Author:      Robert Roebling
// Created:     2006/02/20
// RCS-ID:      $Id$
// Copyright:   (c) 2006, Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/defs.h"

#if wxUSE_DATAVIEWCTRL

#include "wx/object.h"
#include "wx/dataview.h"
#include "wx/log.h"
#include "wx/image.h"

// --------------------------------------------------------- 
// wxDataViewModel
// --------------------------------------------------------- 

IMPLEMENT_ABSTRACT_CLASS(wxDataViewModel, wxObject)

// --------------------------------------------------------- 
// wxDataViewListModel
// --------------------------------------------------------- 

IMPLEMENT_ABSTRACT_CLASS(wxDataViewListModel, wxDataViewModel)

wxDataViewListModel::wxDataViewListModel()
{
    m_viewingColumns.DeleteContents( true );
    m_notifiers.DeleteContents( true );
}

wxDataViewListModel::~wxDataViewListModel()
{
}

bool wxDataViewListModel::RowAppended()
{
    bool ret = true;

    wxNode *node = m_notifiers.GetFirst();
    while (node)
    {
        wxDataViewListModelNotifier* notifier = (wxDataViewListModelNotifier*) node->GetData();
        if (!notifier->RowAppended())
            ret = false;
        node = node->GetNext();
    }
        
    return ret;
}

bool wxDataViewListModel::RowPrepended()
{
    bool ret = true;

    wxNode *node = m_notifiers.GetFirst();
    while (node)
    {
        wxDataViewListModelNotifier* notifier = (wxDataViewListModelNotifier*) node->GetData();
        if (!notifier->RowPrepended())
            ret = false;
        node = node->GetNext();
    }
        
    return ret;
}

bool wxDataViewListModel::RowInserted( size_t before )
{
    bool ret = true;

    wxNode *node = m_notifiers.GetFirst();
    while (node)
    {
        wxDataViewListModelNotifier* notifier = (wxDataViewListModelNotifier*) node->GetData();
        if (!notifier->RowInserted(before))
            ret = false;
        node = node->GetNext();
    }
        
    return ret;
}

bool wxDataViewListModel::RowDeleted( size_t row )
{
    bool ret = true;

    wxNode *node = m_notifiers.GetFirst();
    while (node)
    {
        wxDataViewListModelNotifier* notifier = (wxDataViewListModelNotifier*) node->GetData();
        if (!notifier->RowDeleted( row ))
            ret = false;
        node = node->GetNext();
    }
        
    return ret;
}

bool wxDataViewListModel::RowChanged( size_t row )
{
    bool ret = true;

    wxNode *node = m_notifiers.GetFirst();
    while (node)
    {
        wxDataViewListModelNotifier* notifier = (wxDataViewListModelNotifier*) node->GetData();
        if (!notifier->RowChanged( row ))
            ret = false;
        node = node->GetNext();
    }
        
    return ret;
}

bool wxDataViewListModel::ValueChanged( size_t col, size_t row )
{
    bool ret = true;

    wxNode *node = m_notifiers.GetFirst();
    while (node)
    {
        wxDataViewListModelNotifier* notifier = (wxDataViewListModelNotifier*) node->GetData();
        if (!notifier->ValueChanged( col, row ))
            ret = false;
        node = node->GetNext();
    }
    
    return ret;
}

bool wxDataViewListModel::Cleared()
{
    bool ret = true;

    wxNode *node = m_notifiers.GetFirst();
    while (node)
    {
        wxDataViewListModelNotifier* notifier = (wxDataViewListModelNotifier*) node->GetData();
        if (!notifier->Cleared())
            ret = false;
        node = node->GetNext();
    }
        
    return ret;
}

void wxDataViewListModel::AddViewingColumn( wxDataViewColumn *view_column, size_t model_column )
{
    m_viewingColumns.Append( new wxDataViewViewingColumn( view_column, model_column ) );
}

void wxDataViewListModel::RemoveViewingColumn( wxDataViewColumn *column )
{
    wxNode *node = m_viewingColumns.GetFirst();
    while (node)
    {
        wxDataViewViewingColumn* tmp = (wxDataViewViewingColumn*) node->GetData();
        
        if (tmp->m_viewColumn == column)
        {
            m_viewingColumns.DeleteObject( tmp );
            return;
        }
    
        node = node->GetNext();
    }
}

void wxDataViewListModel::AddNotifier( wxDataViewListModelNotifier *notifier )
{
    m_notifiers.Append( notifier );
    notifier->SetOwner( this );
}

void wxDataViewListModel::RemoveNotifier( wxDataViewListModelNotifier *notifier )
{
    m_notifiers.DeleteObject( notifier );
}

// --------------------------------------------------------- 
// wxDataViewSortedListModel
// --------------------------------------------------------- 

int wxCALLBACK wxDataViewListModelSortedDefaultCompare
      (size_t row1, size_t row2, size_t col, wxDataViewListModel* model )
{
    wxVariant value1 = model->GetValue( col, row1 );
    wxVariant value2 = model->GetValue( col, row2 );
    if (value1.GetType() == wxT("string"))
    {
        wxString str1 = value1.GetString();
        wxString str2 = value2.GetString();
        return str1.Cmp( str2 );
    }
    if (value1.GetType() == wxT("long"))
    {
        long l1 = value1.GetLong();
        long l2 = value2.GetLong();
        return l1-l2;
    }
    if (value1.GetType() == wxT("double"))
    {
        double d1 = value1.GetDouble();
        double d2 = value2.GetDouble();
        if (d1 == d2) return 0;
        if (d1 < d2) return 1;
        return -1;
    }
    if (value1.GetType() == wxT("datetime"))
    {
        wxDateTime dt1 = value1.GetDateTime();
        wxDateTime dt2 = value2.GetDateTime();
        if (dt1.IsEqualTo(dt2)) return 0;
        if (dt1.IsEarlierThan(dt2)) return 1;
        return -1;
    }

    return 0;
}

static wxDataViewListModelCompare   s_CmpFunc;
static wxDataViewListModel         *s_CmpModel;
static size_t                       s_CmpCol;

int LINKAGEMODE wxDataViewIntermediateCmp( size_t row1, size_t row2 )
{
    return s_CmpFunc( row1, row2, s_CmpCol, s_CmpModel );
}


IMPLEMENT_ABSTRACT_CLASS(wxDataViewSortedListModel, wxDataViewListModel)

wxDataViewSortedListModel::wxDataViewSortedListModel( wxDataViewListModel *child ) :
  m_array( wxDataViewIntermediateCmp )
{
    m_child = child;
    s_CmpCol = 0;
    s_CmpModel = child;
    s_CmpFunc = wxDataViewListModelSortedDefaultCompare;
    
    Resort();    
}

wxDataViewSortedListModel::~wxDataViewSortedListModel()
{
}

void wxDataViewSortedListModel::Resort()
{
    m_array.Clear();
    size_t n = m_child->GetNumberOfRows();
    size_t i;
    for (i = 0; i < n; i++)
        m_array.Add( i );
}

size_t wxDataViewSortedListModel::GetNumberOfRows()
{
    return m_child->GetNumberOfRows();
}

size_t wxDataViewSortedListModel::GetNumberOfCols()
{
    return m_child->GetNumberOfCols();
}

wxString wxDataViewSortedListModel::GetColType( size_t col )
{
    return m_child->GetColType( col );
}

wxVariant wxDataViewSortedListModel::GetValue( size_t col, size_t row )
{
    size_t child_row = m_array[row];
    return m_child->GetValue( col, child_row );
}

bool wxDataViewSortedListModel::SetValue( wxVariant &variant, size_t col, size_t row )
{
    size_t child_row = m_array[row];
    bool ret = m_child->SetValue( variant, col, child_row );
    // resort in ::ValueChanged()
    return ret;
}

bool wxDataViewSortedListModel::RowAppended()
{
    // you can only append
    bool ret = m_child->RowAppended();
    
    // report RowInsrted
    
    return ret;
}

bool wxDataViewSortedListModel::RowPrepended()
{
    // you can only append
    bool ret = m_child->RowAppended();
    
    // report RowInsrted
    
    return ret;
}

bool wxDataViewSortedListModel::RowInserted( size_t before )
{
    // you can only append
    bool ret = m_child->RowAppended();
    
    // report different RowInsrted
    
    return ret;
}

bool wxDataViewSortedListModel::RowDeleted( size_t row )
{
    size_t child_row = m_array[row];
    
    bool ret = m_child->RowDeleted( child_row );
    
    wxDataViewListModel::RowDeleted( row );
    
    return ret;
}

bool wxDataViewSortedListModel::RowChanged( size_t row )
{
    size_t child_row = m_array[row];
    bool ret = m_child->RowChanged( child_row );
    
    // report delete old pos, inserted new pos
    
    return ret;
}

bool wxDataViewSortedListModel::ValueChanged( size_t col, size_t row )
{
    size_t child_row = m_array[row];
    bool ret = m_child->ValueChanged( col, child_row );
    
    // Do nothing if not the sorted col..
    // report delete old pos, inserted new pos
    
    return ret;
}

bool wxDataViewSortedListModel::Cleared()
{
    bool ret = m_child->Cleared();
    
    wxDataViewListModel::Cleared();
    
    return ret;
}

// --------------------------------------------------------- 
// wxDataViewCellBase
// --------------------------------------------------------- 

IMPLEMENT_ABSTRACT_CLASS(wxDataViewCellBase, wxObject)

wxDataViewCellBase::wxDataViewCellBase( const wxString &varianttype, wxDataViewCellMode mode )
{
    m_variantType = varianttype;
    m_mode = mode;
}
    
// --------------------------------------------------------- 
// wxDataViewColumnBase
// --------------------------------------------------------- 

IMPLEMENT_ABSTRACT_CLASS(wxDataViewColumnBase, wxObject)

wxDataViewColumnBase::wxDataViewColumnBase( const wxString &title, wxDataViewCell *cell, size_t model_column, int flags)
{
    m_cell = cell;
    m_model_column = model_column;
    m_flags = flags;
    m_title = title;
    m_owner = NULL;
    m_cell->SetOwner( (wxDataViewColumn*) this );
}

wxDataViewColumnBase::~wxDataViewColumnBase()
{
    if (m_cell)
        delete m_cell;
        
    if (GetOwner())
    {
        GetOwner()->GetModel()->RemoveViewingColumn( (wxDataViewColumn*) this );
    }
}

void wxDataViewColumnBase::SetTitle( const wxString &title )
{
    m_title = title;
}

wxString wxDataViewColumnBase::GetTitle()
{
    return m_title;
}

// --------------------------------------------------------- 
// wxDataViewCtrlBase
// --------------------------------------------------------- 

IMPLEMENT_ABSTRACT_CLASS(wxDataViewCtrlBase, wxControl)

wxDataViewCtrlBase::wxDataViewCtrlBase()
{
    m_model = NULL;
    m_cols.DeleteContents( true );
}

wxDataViewCtrlBase::~wxDataViewCtrlBase()
{
    if (m_model)
        delete m_model;
}

bool wxDataViewCtrlBase::AssociateModel( wxDataViewListModel *model )
{
    if (m_model)
        delete m_model;
        
    m_model = model;
    
    return true;
}

wxDataViewListModel* wxDataViewCtrlBase::GetModel()
{
    return m_model;
}

bool wxDataViewCtrlBase::AppendTextColumn( const wxString &label, size_t model_column )
{
    return AppendColumn( new wxDataViewColumn( label, new wxDataViewTextCell(), model_column ) );
}

bool wxDataViewCtrlBase::AppendToggleColumn( const wxString &label, size_t model_column )
{
    return AppendColumn( new wxDataViewColumn( label, new wxDataViewToggleCell(), model_column ) );
}

bool wxDataViewCtrlBase::AppendProgressColumn( const wxString &label, size_t model_column )
{
    return AppendColumn( new wxDataViewColumn( label, new wxDataViewProgressCell(), model_column ) );
}

bool wxDataViewCtrlBase::AppendDateColumn( const wxString &label, size_t model_column )
{
    return AppendColumn( new wxDataViewColumn( label, new wxDataViewDateCell(), model_column ) );
}

bool wxDataViewCtrlBase::AppendColumn( wxDataViewColumn *col )
{
    m_cols.Append( (wxObject*) col );
    col->SetOwner( (wxDataViewCtrl*) this );
    m_model->AddViewingColumn( col, col->GetModelColumn() );
    return true;
}

size_t wxDataViewCtrlBase::GetNumberOfColumns()
{
    return m_cols.GetCount();
}

bool wxDataViewCtrlBase::DeleteColumn( size_t pos )
{
    return false;
}

bool wxDataViewCtrlBase::ClearColumns()
{
    return false;
}

wxDataViewColumn* wxDataViewCtrlBase::GetColumn( size_t pos )
{
    return (wxDataViewColumn*) m_cols[ pos ];
}

#endif

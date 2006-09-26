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

#if wxUSE_DATAVIEWCTRL

#include "wx/dataview.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif

const wxChar wxDataViewCtrlNameStr[] = wxT("dataviewCtrl");

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

    wxList::compatibility_iterator node = m_notifiers.GetFirst();
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

    wxList::compatibility_iterator node = m_notifiers.GetFirst();
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

    wxList::compatibility_iterator node = m_notifiers.GetFirst();
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

    wxList::compatibility_iterator node = m_notifiers.GetFirst();
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

    wxList::compatibility_iterator node = m_notifiers.GetFirst();
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

    wxList::compatibility_iterator node = m_notifiers.GetFirst();
    while (node)
    {
        wxDataViewListModelNotifier* notifier = (wxDataViewListModelNotifier*) node->GetData();
        if (!notifier->ValueChanged( col, row ))
            ret = false;
        node = node->GetNext();
    }

    return ret;
}

bool wxDataViewListModel::RowsReordered( size_t *new_order )
{
    bool ret = true;

    wxList::compatibility_iterator node = m_notifiers.GetFirst();
    while (node)
    {
        wxDataViewListModelNotifier* notifier = (wxDataViewListModelNotifier*) node->GetData();
        if (!notifier->RowsReordered( new_order ))
            ret = false;
        node = node->GetNext();
    }

    return ret;
}

bool wxDataViewListModel::Cleared()
{
    bool ret = true;

    wxList::compatibility_iterator node = m_notifiers.GetFirst();
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
    wxList::compatibility_iterator node = m_viewingColumns.GetFirst();
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
// wxDataViewSortedListModelNotifier
// ---------------------------------------------------------

class wxDataViewSortedListModelNotifier: public wxDataViewListModelNotifier
{
public:
    wxDataViewSortedListModelNotifier( wxDataViewSortedListModel *model )
        { m_model = model; }

    virtual bool RowAppended()
        { return m_model->ChildRowAppended(); }

    virtual bool RowPrepended()
        { return m_model->ChildRowPrepended(); }

    virtual bool RowInserted( size_t before )
        { return m_model->ChildRowInserted( before ); }

    virtual bool RowDeleted( size_t row )
        { return m_model->ChildRowDeleted( row ); }

    virtual bool RowChanged( size_t row )
        { return m_model->ChildRowChanged( row ); }

    virtual bool ValueChanged( size_t col, size_t row )
        { return m_model->ChildValueChanged( col, row); }

    virtual bool RowsReordered( size_t *new_order )
        { return m_model->ChildRowsReordered( new_order ); }

    virtual bool Cleared()
        { return m_model->ChildCleared(); }

    wxDataViewSortedListModel *m_model;
};

// ---------------------------------------------------------
// wxDataViewSortedListModel compare function
// ---------------------------------------------------------

int wxCALLBACK wxDataViewListModelSortedDefaultCompare
      (size_t row1, size_t row2, size_t col, wxDataViewListModel* model )
{
    wxVariant value1,value2;
    model->GetValue( value1, col, row1 );
    model->GetValue( value2, col, row2 );
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

// ---------------------------------------------------------
// wxDataViewSortedListModel
// ---------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDataViewSortedListModel, wxDataViewListModel)

wxDataViewSortedListModel::wxDataViewSortedListModel( wxDataViewListModel *child ) :
  m_array( wxDataViewIntermediateCmp )
{
    m_child = child;
    s_CmpCol = 0;
    s_CmpModel = child;
    s_CmpFunc = wxDataViewListModelSortedDefaultCompare;

    m_notifierOnChild = new wxDataViewSortedListModelNotifier( this );
    m_child->AddNotifier( m_notifierOnChild );

    Resort();
}

wxDataViewSortedListModel::~wxDataViewSortedListModel()
{
    m_child->RemoveNotifier( m_notifierOnChild );
}

void wxDataViewSortedListModel::Resort()
{
    m_array.Clear();
    size_t n = m_child->GetNumberOfRows();
    size_t i;
    for (i = 0; i < n; i++)
        m_array.Add( i );
}

#if 0
static void Dump( wxDataViewListModel *model, size_t col )
{
    size_t n = model->GetNumberOfRows();
    size_t i;
    for (i = 0; i < n; i++)
    {
        wxVariant variant;
        model->GetValue( variant, col, i );
        wxString tmp;
        tmp = variant.GetString();
        wxPrintf( wxT("%d: %s\n"), (int) i, tmp.c_str() );
    }
}
#endif

bool wxDataViewSortedListModel::ChildRowAppended()
{
    // no need to fix up array

    size_t len = m_array.GetCount();

    size_t pos = m_array.Add( len );

    if (pos == 0)
        return wxDataViewListModel::RowPrepended();

    if (pos == len)
        return wxDataViewListModel::RowAppended();

    return wxDataViewListModel::RowInserted( pos );
}

bool wxDataViewSortedListModel::ChildRowPrepended()
{
    // fix up array
    size_t i;
    size_t len = m_array.GetCount();
    for (i = 0; i < len; i++)
    {
        size_t value = m_array[i];
        m_array[i] = value+1;
    }

    size_t pos = m_array.Add( 0 );

    if (pos == 0)
        return wxDataViewListModel::RowPrepended();

    if (pos == len)
        return wxDataViewListModel::RowAppended();

    return wxDataViewListModel::RowInserted( pos );
}

bool wxDataViewSortedListModel::ChildRowInserted( size_t before )
{
    // fix up array
    size_t i;
    size_t len = m_array.GetCount();
    for (i = 0; i < len; i++)
    {
        size_t value = m_array[i];
        if (value >= before)
           m_array[i] = value+1;
    }

    size_t pos = m_array.Add( before );

    if (pos == 0)
        return wxDataViewListModel::RowPrepended();

    if (pos == len)
        return wxDataViewListModel::RowAppended();

    return wxDataViewListModel::RowInserted( pos );
}

bool wxDataViewSortedListModel::ChildRowDeleted( size_t row )
{
    size_t i;
    size_t len = m_array.GetCount();
    int pos = -1;
    for (i = 0; i < len; i++)
    {
        size_t value = m_array[i];
        if (value == row)
        {
            // delete later
            pos = (int) i;
        }
        else
        {
            // Fix up array
            if (value > row)
                m_array[i] = value-1;
        }
    }

    if (pos == -1)
        return false; // we should probably assert

    // remove
    m_array.RemoveAt( (size_t) pos );

    return wxDataViewListModel::RowDeleted( (size_t) pos);
}

bool wxDataViewSortedListModel::ChildRowChanged( size_t row )
{
    size_t i;
    size_t len = m_array.GetCount();

    // Remove and readd sorted. Find out at which
    // position it was and where it ended.
    size_t start_pos = 0,end_pos = 0;
    for (i = 0; i < len; i++)
        if (m_array[i] == row)
        {
            start_pos = i;
            break;
        }
    m_array.RemoveAt( start_pos );
    m_array.Add( row );

    for (i = 0; i < len; i++)
        if (m_array[i] == row)
        {
            end_pos = i;
            break;
        }

    if (end_pos == start_pos)
        return wxDataViewListModel::RowChanged( start_pos );

    // Create an array where order[old] -> new_pos, so that
    // if nothing changed order[0] -> 0 etc.
    size_t *order = new size_t[ len ];
    // Fill up initial values.
    for (i = 0; i < len; i++)
        order[i] = i;

    if (start_pos < end_pos)
    {
        for (i = start_pos; i < end_pos; i++)
            order[i] = order[i+1];
        order[end_pos] = start_pos;
    }
    else
    {
        for (i = end_pos; i > start_pos; i--)
            order[i] = order[i-1];
        order[start_pos] = end_pos;
    }

    wxDataViewListModel::RowsReordered( order );

    delete [] order;

    return true;
}

bool wxDataViewSortedListModel::ChildValueChanged( size_t col, size_t row )
{
    size_t i;
    size_t len = m_array.GetCount();

    // Remove and readd sorted. Find out at which
    // position it was and where it ended.
    size_t start_pos = 0,end_pos = 0;
    for (i = 0; i < len; i++)
        if (m_array[i] == row)
        {
            start_pos = i;
            break;
        }
    m_array.RemoveAt( start_pos );
    m_array.Add( row );

    for (i = 0; i < len; i++)
        if (m_array[i] == row)
        {
            end_pos = i;
            break;
        }

    if (end_pos == start_pos)
        return wxDataViewListModel::ValueChanged( col, start_pos );

    // Create an array where order[old] -> new_pos, so that
    // if nothing changed order[0] -> 0 etc.
    size_t *order = new size_t[ len ];
    // Fill up initial values.
    for (i = 0; i < len; i++)
        order[i] = i;

    if (start_pos < end_pos)
    {
        for (i = start_pos; i < end_pos; i++)
            order[i] = order[i+1];
        order[end_pos] = start_pos;
    }
    else
    {
        for (i = end_pos; i > start_pos; i--)
            order[i] = order[i-1];
        order[start_pos] = end_pos;
    }

    wxDataViewListModel::RowsReordered( order );

    delete [] order;

    return true;
}

bool wxDataViewSortedListModel::ChildRowsReordered( size_t *WXUNUSED(new_order) )
{
    // Nothing needs to be done. If the sort criteria
    // of this list don't change, the order of the
    // items of the child list isn't relevant.
    return true;
}

bool wxDataViewSortedListModel::ChildCleared()
{
    return wxDataViewListModel::Cleared();
}

size_t wxDataViewSortedListModel::GetNumberOfRows()
{
    return m_array.GetCount();
}

size_t wxDataViewSortedListModel::GetNumberOfCols()
{
    return m_child->GetNumberOfCols();
}

wxString wxDataViewSortedListModel::GetColType( size_t col )
{
    return m_child->GetColType( col );
}

void wxDataViewSortedListModel::GetValue( wxVariant &variant, size_t col, size_t row )
{
    size_t child_row = m_array[row];
    m_child->GetValue( variant, col, child_row );
}

bool wxDataViewSortedListModel::SetValue( wxVariant &variant, size_t col, size_t row )
{
    size_t child_row = m_array[row];
    bool ret = m_child->SetValue( variant, col, child_row );

    // Do nothing here as the change in the
    // child model will be reported back.

    return ret;
}

bool wxDataViewSortedListModel::RowAppended()
{
    // you can only append
    bool ret = m_child->RowAppended();

    // Do nothing here as the change in the
    // child model will be reported back.

    return ret;
}

bool wxDataViewSortedListModel::RowPrepended()
{
    // you can only append
    bool ret = m_child->RowAppended();

    // Do nothing here as the change in the
    // child model will be reported back.

    return ret;
}

bool wxDataViewSortedListModel::RowInserted( size_t WXUNUSED(before) )
{
    // you can only append
    bool ret = m_child->RowAppended();

    // Do nothing here as the change in the
    // child model will be reported back.

    return ret;
}

bool wxDataViewSortedListModel::RowDeleted( size_t row )
{
    size_t child_row = m_array[row];

    bool ret = m_child->RowDeleted( child_row );

    // Do nothing here as the change in the
    // child model will be reported back.

    return ret;
}

bool wxDataViewSortedListModel::RowChanged( size_t row )
{
    size_t child_row = m_array[row];
    bool ret = m_child->RowChanged( child_row );

    // Do nothing here as the change in the
    // child model will be reported back.

    return ret;
}

bool wxDataViewSortedListModel::ValueChanged( size_t col, size_t row )
{
    size_t child_row = m_array[row];
    bool ret = m_child->ValueChanged( col, child_row );

    // Do nothing here as the change in the
    // child model will be reported back.

    return ret;
}

bool wxDataViewSortedListModel::RowsReordered( size_t *WXUNUSED(new_order) )
{
    // We sort them ourselves.

    return false;
}

bool wxDataViewSortedListModel::Cleared()
{
    bool ret = m_child->Cleared();

    // Do nothing here as the change in the
    // child model will be reported back.

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

wxDataViewColumnBase::wxDataViewColumnBase(const wxString& title,
                                           wxDataViewCell *cell,
                                           size_t model_column,
                                           int WXUNUSED(fixed_width),
                                           wxDataViewColumnSizing WXUNUSED(sizing),
                                           int flags )
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
}

bool wxDataViewCtrlBase::AssociateModel( wxDataViewListModel *model )
{
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
    return AppendColumn( new wxDataViewColumn( label, new wxDataViewToggleCell(), model_column, 30 ) );
}

bool wxDataViewCtrlBase::AppendProgressColumn( const wxString &label, size_t model_column )
{
    return AppendColumn( new wxDataViewColumn( label, new wxDataViewProgressCell(), model_column, 70 ) );
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

bool wxDataViewCtrlBase::DeleteColumn( size_t WXUNUSED(pos) )
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

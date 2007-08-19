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


bool operator == (const wxDataViewItem &left, const wxDataViewItem &right)
{
    return (left.GetID() == right.GetID() );
}


// ---------------------------------------------------------
// wxDataViewModel
// ---------------------------------------------------------

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(wxDataViewModelNotifiers);

wxDataViewModel::wxDataViewModel()
{
    m_notifiers.DeleteContents( true );
}

bool wxDataViewModel::ItemAdded( const wxDataViewItem &parent, const wxDataViewItem &item )
{
    bool ret = true;

    wxDataViewModelNotifiers::iterator iter;
    for (iter = m_notifiers.begin(); iter != m_notifiers.end(); ++iter)
    {
        wxDataViewModelNotifier* notifier = *iter;
        if (!notifier->ItemAdded( parent, item ))
            ret = false;
    }

    return ret;
}

bool wxDataViewModel::ItemDeleted( const wxDataViewItem &parent, const wxDataViewItem &item )
{
    bool ret = true;

    wxDataViewModelNotifiers::iterator iter;
    for (iter = m_notifiers.begin(); iter != m_notifiers.end(); ++iter)
    {
        wxDataViewModelNotifier* notifier = *iter;
        if (!notifier->ItemDeleted( parent, item ))
            ret = false;
    }

    return ret;
}

bool wxDataViewModel::ItemChanged( const wxDataViewItem &item )
{
    bool ret = true;

    wxDataViewModelNotifiers::iterator iter;
    for (iter = m_notifiers.begin(); iter != m_notifiers.end(); ++iter)
    {
        wxDataViewModelNotifier* notifier = *iter;
        if (!notifier->ItemChanged( item ))
            ret = false;
    }

    return ret;
}

bool wxDataViewModel::ValueChanged( const wxDataViewItem &item, unsigned int col )
{
    bool ret = true;

    wxDataViewModelNotifiers::iterator iter;
    for (iter = m_notifiers.begin(); iter != m_notifiers.end(); ++iter)
    {
        wxDataViewModelNotifier* notifier = *iter;
        if (!notifier->ValueChanged( item, col ))
            ret = false;
    }

    return ret;
}

bool wxDataViewModel::Cleared()
{
    bool ret = true;

    wxDataViewModelNotifiers::iterator iter;
    for (iter = m_notifiers.begin(); iter != m_notifiers.end(); ++iter)
    {
        wxDataViewModelNotifier* notifier = *iter;
        if (!notifier->Cleared())
            ret = false;
    }

    return ret;
}

void wxDataViewModel::Resort()
{
    wxDataViewModelNotifiers::iterator iter;
    for (iter = m_notifiers.begin(); iter != m_notifiers.end(); ++iter)
    {
        wxDataViewModelNotifier* notifier = *iter;
        notifier->Resort();
    }
}

void wxDataViewModel::AddNotifier( wxDataViewModelNotifier *notifier )
{
    m_notifiers.push_back( notifier );
    notifier->SetOwner( this );
}

void wxDataViewModel::RemoveNotifier( wxDataViewModelNotifier *notifier )
{
    m_notifiers.DeleteObject( notifier );
}

int wxDataViewModel::Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
                              unsigned int column, bool ascending )
{
    // sort branches before leaves
    bool item1_is_container = IsContainer(item1);
    bool item2_is_container = IsContainer(item2);

    if (item1_is_container && !item2_is_container)
        return 1;
    if (item2_is_container && !item1_is_container)
        return -1;

    wxVariant value1,value2;
    GetValue( value1, item1, column );
    GetValue( value2, item2, column );

    if (!ascending)
    {
        wxVariant temp = value1;
        value1 = value2;
        value2 = temp;
    }

    if (value1.GetType() == wxT("string"))
    {
        wxString str1 = value1.GetString();
        wxString str2 = value2.GetString();
        int res = str1.Cmp( str2 );
        if (res) return res;
    } else
    if (value1.GetType() == wxT("long"))
    {
        long l1 = value1.GetLong();
        long l2 = value2.GetLong();
        long res = l1-l2;
        if (res) return res;
    } else
    if (value1.GetType() == wxT("double"))
    {
        double d1 = value1.GetDouble();
        double d2 = value2.GetDouble();
        if (d1 < d2) return 1;
        if (d1 > d2) return -1;
    } else
    if (value1.GetType() == wxT("datetime"))
    {
        wxDateTime dt1 = value1.GetDateTime();
        wxDateTime dt2 = value2.GetDateTime();
        if (dt1.IsEarlierThan(dt2)) return 1;
        if (dt2.IsEarlierThan(dt1)) return -11;
    }

    // items must be different
    unsigned long litem1 = (unsigned long) item1.GetID();
    unsigned long litem2 = (unsigned long) item2.GetID();
    
    if (!ascending)
        return litem2-litem2;

    return litem1-litem2;
}

// ---------------------------------------------------------
// wxDataViewIndexListModel
// ---------------------------------------------------------

wxDataViewIndexListModel::wxDataViewIndexListModel( unsigned int initial_size )
{
    // build initial index
    unsigned int i;
    for (i = 1; i < initial_size+1; i++)
        m_hash.Add( (void*) i );
    m_lastIndex = initial_size + 1;
}

wxDataViewIndexListModel::~wxDataViewIndexListModel()
{
}

void wxDataViewIndexListModel::RowPrepended()
{
    unsigned int id = m_lastIndex++;
    m_hash.Insert( (void*) id, 0 );
    wxDataViewItem item( (void*) id );
    ItemAdded( wxDataViewItem(0), item );
}

void wxDataViewIndexListModel::RowInserted( unsigned int before )
{
    unsigned int id = m_lastIndex++;
    m_hash.Insert( (void*) id, before );
    wxDataViewItem item( (void*) id );
    ItemAdded( wxDataViewItem(0), item );
}

void wxDataViewIndexListModel::RowAppended()
{
    unsigned int id = m_lastIndex++;
    m_hash.Add( (void*) id );
    wxDataViewItem item( (void*) id );
    ItemAdded( wxDataViewItem(0), item );
}

void wxDataViewIndexListModel::RowDeleted( unsigned int row )
{
    wxDataViewItem item( m_hash[row] );
    wxDataViewModel::ItemDeleted( wxDataViewItem(0), item );
    m_hash.RemoveAt( row );
}

void wxDataViewIndexListModel::RowChanged( unsigned int row )
{
    wxDataViewModel::ItemChanged( GetItem(row) );
}

void wxDataViewIndexListModel::RowValueChanged( unsigned int row, unsigned int col )
{
    wxDataViewModel::ValueChanged( GetItem(row), col );
}

unsigned int wxDataViewIndexListModel::GetRow( const wxDataViewItem &item ) const
{
    // assert for not found
    return (unsigned int) m_hash.Index( item.GetID() );
}

wxDataViewItem wxDataViewIndexListModel::GetItem( unsigned int row ) const
{
    return wxDataViewItem( m_hash[row] );
}

int wxDataViewIndexListModel::Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
                                       unsigned int column, bool ascending )
{
    if (ascending)
        return GetRow(item1) - GetRow(item2);
    
    return GetRow(item2) - GetRow(item1);
}

void wxDataViewIndexListModel::GetValue( wxVariant &variant,
                           const wxDataViewItem &item, unsigned int col ) const
{
    GetValue( variant, GetRow(item), col );
}

bool wxDataViewIndexListModel::SetValue( const wxVariant &variant,
                           const wxDataViewItem &item, unsigned int col )
{
    return SetValue( variant, GetRow(item), col );
}

wxDataViewItem wxDataViewIndexListModel::GetParent( const wxDataViewItem & WXUNUSED(item) ) const
{
    return wxDataViewItem(0);
}

bool wxDataViewIndexListModel::IsContainer( const wxDataViewItem &item ) const
{
    // only the invisible root item has children
    if (!item.IsOk())
        return true;

    return false;
}

wxDataViewItem wxDataViewIndexListModel::GetFirstChild( const wxDataViewItem &parent ) const
{
    if (!parent.IsOk())
    {
        if (m_hash.GetCount() == 0)
            return wxDataViewItem(0);

        return wxDataViewItem( m_hash[0]);
    }

    return wxDataViewItem(0);
}

wxDataViewItem wxDataViewIndexListModel::GetNextSibling( const wxDataViewItem &item ) const
{
    if (!item.IsOk())
        return wxDataViewItem(0);

    int pos = m_hash.Index( item.GetID() );
    if ((pos == wxNOT_FOUND) || (pos == (int) (m_hash.GetCount()-1)))
        return wxDataViewItem(0);

    return wxDataViewItem( m_hash[pos+1] );
}

// ---------------------------------------------------------
// wxDataViewRendererBase
// ---------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDataViewRendererBase, wxObject)

wxDataViewRendererBase::wxDataViewRendererBase( const wxString &varianttype,
                                                wxDataViewCellMode WXUNUSED(mode),
                                                int WXUNUSED(align) )
{
    m_variantType = varianttype;
    m_editorCtrl = NULL;
}

const wxDataViewCtrl* wxDataViewRendererBase::GetView() const
{
    return wx_const_cast(wxDataViewRendererBase*, this)->GetOwner()->GetOwner();
}

bool wxDataViewRendererBase::StartEditing( const wxDataViewItem &item, wxRect labelRect )
{
    m_item = item; // remember for later

    unsigned int col = GetOwner()->GetModelColumn();
    wxVariant value;
    GetOwner()->GetOwner()->GetModel()->GetValue( value, item, col );

    m_editorCtrl = CreateEditorCtrl( GetOwner()->GetOwner()->GetMainWindow(), labelRect, value );

    wxDataViewEditorCtrlEvtHandler *handler =
        new wxDataViewEditorCtrlEvtHandler( m_editorCtrl, (wxDataViewRenderer*) this );

    m_editorCtrl->PushEventHandler( handler );

#if defined(__WXGTK20__) && !defined(wxUSE_GENERICDATAVIEWCTRL)
    handler->SetFocusOnIdle();
#else
    m_editorCtrl->SetFocus();
#endif

    return true;
}

void wxDataViewRendererBase::CancelEditing()
{
    wxPendingDelete.Append( m_editorCtrl );

    GetOwner()->GetOwner()->GetMainWindow()->SetFocus();

    // m_editorCtrl->PopEventHandler( true );
}

bool wxDataViewRendererBase::FinishEditing()
{
    wxVariant value;
    GetValueFromEditorCtrl( m_editorCtrl, value );

    wxPendingDelete.Append( m_editorCtrl );

    GetOwner()->GetOwner()->GetMainWindow()->SetFocus();

    if (!Validate(value))
        return false;

    unsigned int col = GetOwner()->GetModelColumn();
    GetOwner()->GetOwner()->GetModel()->SetValue( value, m_item, col );
    GetOwner()->GetOwner()->GetModel()->ValueChanged( m_item, col );

    // m_editorCtrl->PopEventHandler( true );

    return true;
}

//-----------------------------------------------------------------------------
// wxDataViewEditorCtrlEvtHandler
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxDataViewEditorCtrlEvtHandler, wxEvtHandler)
    EVT_CHAR           (wxDataViewEditorCtrlEvtHandler::OnChar)
    EVT_KILL_FOCUS     (wxDataViewEditorCtrlEvtHandler::OnKillFocus)
    EVT_IDLE           (wxDataViewEditorCtrlEvtHandler::OnIdle)
END_EVENT_TABLE()

wxDataViewEditorCtrlEvtHandler::wxDataViewEditorCtrlEvtHandler(
                                wxControl *editorCtrl,
                                wxDataViewRenderer *owner )
{
    m_owner = owner;
    m_editorCtrl = editorCtrl;

    m_finished = false;
}

void wxDataViewEditorCtrlEvtHandler::OnIdle( wxIdleEvent &event )
{
    if (m_focusOnIdle)
    {
        m_focusOnIdle = false;
        if (wxWindow::FindFocus() != m_editorCtrl)
            m_editorCtrl->SetFocus();
    }

    event.Skip();
}

void wxDataViewEditorCtrlEvtHandler::OnChar( wxKeyEvent &event )
{
    switch ( event.m_keyCode )
    {
        case WXK_RETURN:
            m_finished = true;
            m_owner->FinishEditing();
            break;

        case WXK_ESCAPE:
            m_finished = true;
            m_owner->CancelEditing();
            break;

        default:
            event.Skip();
    }
}

void wxDataViewEditorCtrlEvtHandler::OnKillFocus( wxFocusEvent &event )
{
    if (!m_finished)
    {
        m_finished = true;
        m_owner->FinishEditing();
    }

    event.Skip();
}

// ---------------------------------------------------------
// wxDataViewColumnBase
// ---------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDataViewColumnBase, wxObject)

wxDataViewColumnBase::wxDataViewColumnBase(const wxString& WXUNUSED(title),
                                           wxDataViewRenderer *renderer,
                                           unsigned int model_column,
                                           int WXUNUSED(width),
                                           wxAlignment WXUNUSED(align),
                                           int WXUNUSED(flags))
{
    m_renderer = renderer;
    m_model_column = model_column;
    m_owner = NULL;
    m_renderer->SetOwner( (wxDataViewColumn*) this );

    // NOTE: the wxDataViewColumn's ctor must store the width, align, flags
    //       parameters inside the native control!
}

wxDataViewColumnBase::wxDataViewColumnBase(const wxBitmap& bitmap,
                                           wxDataViewRenderer *renderer,
                                           unsigned int model_column,
                                           int WXUNUSED(width),
                                           wxAlignment WXUNUSED(align),
                                           int WXUNUSED(flags) )
{
    m_renderer = renderer;
    m_model_column = model_column;
    m_bitmap = bitmap;
    m_owner = NULL;
    m_renderer->SetOwner( (wxDataViewColumn*) this );
}

wxDataViewColumnBase::~wxDataViewColumnBase()
{
    if (m_renderer)
        delete m_renderer;
}

int wxDataViewColumnBase::GetFlags() const
{
    int ret = 0;

    if (IsSortable())
        ret |= wxDATAVIEW_COL_SORTABLE;
    if (IsResizeable())
        ret |= wxDATAVIEW_COL_RESIZABLE;
    if (IsHidden())
        ret |= wxDATAVIEW_COL_HIDDEN;

    return ret;
}

void wxDataViewColumnBase::SetFlags(int flags)
{
    SetSortable((flags & wxDATAVIEW_COL_SORTABLE) != 0);
    SetResizeable((flags & wxDATAVIEW_COL_RESIZABLE) != 0);
    SetHidden((flags & wxDATAVIEW_COL_HIDDEN) != 0);
}

// ---------------------------------------------------------
// wxDataViewCtrlBase
// ---------------------------------------------------------

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxDataViewItemArray);

IMPLEMENT_ABSTRACT_CLASS(wxDataViewCtrlBase, wxControl)

wxDataViewCtrlBase::wxDataViewCtrlBase()
{
    m_model = NULL;
    m_cols.DeleteContents( true );
    m_expander_column = 0;
    m_indent = 8;
}

wxDataViewCtrlBase::~wxDataViewCtrlBase()
{
    // IMPORTANT: before calling DecRef() on our model (since it may
    //            result in a free() call), erase all columns (since
    //            they hold a pointer to our model)
    m_cols.Clear();

    if (m_model)
    {
        m_model->DecRef();
        m_model = NULL;
    }
}

bool wxDataViewCtrlBase::AssociateModel( wxDataViewModel *model )
{
    if (m_model)
    {
        m_model->DecRef();   // discard old model, if any
    }

    // add our own reference to the new model:
    m_model = model;
    if (m_model)
    {
        m_model->IncRef();
    }

    return true;
}

wxDataViewModel* wxDataViewCtrlBase::GetModel()
{
    return m_model;
}

bool wxDataViewCtrlBase::AppendTextColumn( const wxString &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumn( new wxDataViewColumn( label,
        new wxDataViewTextRenderer( wxT("string"), mode, (int)align ),
        model_column, width, align, flags ) );
}

bool wxDataViewCtrlBase::AppendToggleColumn( const wxString &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumn( new wxDataViewColumn( label,
        new wxDataViewToggleRenderer( wxT("bool"), mode, (int)align ),
        model_column, width, align, flags ) );
}

bool wxDataViewCtrlBase::AppendProgressColumn( const wxString &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumn( new wxDataViewColumn( label,
        new wxDataViewProgressRenderer( wxEmptyString, wxT("long"), mode, (int)align ),
        model_column, width, align, flags ) );
}

bool wxDataViewCtrlBase::AppendDateColumn( const wxString &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumn( new wxDataViewColumn( label,
        new wxDataViewDateRenderer( wxT("datetime"), mode, (int)align ),
        model_column, width, align, flags ) );
}

bool wxDataViewCtrlBase::AppendBitmapColumn( const wxString &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumn( new wxDataViewColumn( label,
        new wxDataViewBitmapRenderer( wxT("wxBitmap"), mode, (int)align ),
        model_column, width, align, flags ) );
}

bool wxDataViewCtrlBase::AppendTextColumn( const wxBitmap &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumn( new wxDataViewColumn( label,
        new wxDataViewTextRenderer( wxT("string"), mode, (int)align ),
        model_column, width, align, flags ) );
}

bool wxDataViewCtrlBase::AppendToggleColumn( const wxBitmap &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumn( new wxDataViewColumn( label,
        new wxDataViewToggleRenderer( wxT("bool"), mode, (int)align ),
        model_column, width, align, flags ) );
}

bool wxDataViewCtrlBase::AppendProgressColumn( const wxBitmap &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumn( new wxDataViewColumn( label,
        new wxDataViewProgressRenderer( wxEmptyString, wxT("long"), mode, (int)align ),
        model_column, width, align, flags ) );
}

bool wxDataViewCtrlBase::AppendDateColumn( const wxBitmap &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumn( new wxDataViewColumn( label,
        new wxDataViewDateRenderer( wxT("datetime"), mode, (int)align ),
        model_column, width, align, flags ) );
}

bool wxDataViewCtrlBase::AppendBitmapColumn( const wxBitmap &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumn( new wxDataViewColumn( label,
        new wxDataViewBitmapRenderer( wxT("wxBitmap"), mode, (int)align ),
        model_column, width, align, flags ) );
}

bool wxDataViewCtrlBase::AppendColumn( wxDataViewColumn *col )
{
    m_cols.Append( (wxObject*) col );
    col->SetOwner( (wxDataViewCtrl*) this );
    return true;
}

unsigned int wxDataViewCtrlBase::GetColumnCount() const
{
    return m_cols.GetCount();
}

bool wxDataViewCtrlBase::DeleteColumn( unsigned int WXUNUSED(pos) )
{
    return false;
}

bool wxDataViewCtrlBase::ClearColumns()
{
    return false;
}

wxDataViewColumn* wxDataViewCtrlBase::GetColumn( unsigned int pos )
{
    return (wxDataViewColumn*) m_cols[ pos ];
}

// ---------------------------------------------------------
// wxDataViewEvent
// ---------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxDataViewEvent,wxNotifyEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_ITEM_SELECTED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_ITEM_DESELECTED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_ITEM_COLLAPSED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_COLUMN_SORTED)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_MODEL_ITEM_ADDED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_MODEL_ITEM_DELETED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_MODEL_ITEM_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_MODEL_VALUE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_MODEL_CLEARED)

#endif

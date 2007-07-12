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
// wxDataViewListModel
// ---------------------------------------------------------

wxDataViewModel::wxDataViewModel()
{
    m_notifiers.DeleteContents( true );
    m_cmpFunc = NULL;
}

bool wxDataViewModel::ItemAdded( const wxDataViewItem &parent, const wxDataViewItem &item )
{
    bool ret = true;

    wxList::compatibility_iterator node = m_notifiers.GetFirst();
    while (node)
    {
        wxDataViewModelNotifier* notifier = (wxDataViewModelNotifier*) node->GetData();
        if (!notifier->ItemAdded( parent, item ))
            ret = false;
        node = node->GetNext();
    }

    return ret;
}

bool wxDataViewModel::ItemDeleted( const wxDataViewItem &item )
{
    bool ret = true;

    wxList::compatibility_iterator node = m_notifiers.GetFirst();
    while (node)
    {
        wxDataViewModelNotifier* notifier = (wxDataViewModelNotifier*) node->GetData();
        if (!notifier->ItemDeleted( item ))
            ret = false;
        node = node->GetNext();
    }

    return ret;
}

bool wxDataViewModel::ItemChanged( const wxDataViewItem &item )
{
    bool ret = true;

    wxList::compatibility_iterator node = m_notifiers.GetFirst();
    while (node)
    {
        wxDataViewModelNotifier* notifier = (wxDataViewModelNotifier*) node->GetData();
        if (!notifier->ItemChanged( item ))
            ret = false;
        node = node->GetNext();
    }

    return ret;
}

bool wxDataViewModel::ValueChanged( const wxDataViewItem &item, unsigned int col )
{
    bool ret = true;

    wxList::compatibility_iterator node = m_notifiers.GetFirst();
    while (node)
    {
        wxDataViewModelNotifier* notifier = (wxDataViewModelNotifier*) node->GetData();
        if (!notifier->ValueChanged( item, col ))
            ret = false;
        node = node->GetNext();
    }

    return ret;
}

bool wxDataViewModel::Cleared()
{
    bool ret = true;

    wxList::compatibility_iterator node = m_notifiers.GetFirst();
    while (node)
    {
        wxDataViewModelNotifier* notifier = (wxDataViewModelNotifier*) node->GetData();
        if (!notifier->Cleared())
            ret = false;
        node = node->GetNext();
    }

    return ret;
}

void wxDataViewModel::AddNotifier( wxDataViewModelNotifier *notifier )
{
    m_notifiers.Append( notifier );
    notifier->SetOwner( this );
}

void wxDataViewModel::RemoveNotifier( wxDataViewModelNotifier *notifier )
{
    m_notifiers.DeleteObject( notifier );
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
// wxDataViewEventModelNotifier
// ---------------------------------------------------------

class WXDLLIMPEXP_ADV wxDataViewEventModelNotifier: public wxDataViewModelNotifier
{
public:
    wxDataViewEventModelNotifier( wxDataViewCtrl *ctrl ) { m_ctrl = ctrl; }
    
    bool SendEvent( wxEventType event_type, const wxDataViewItem &item, unsigned int col = 0 )
    {
        wxDataViewEvent event( event_type, m_ctrl->GetId() );
        event.SetEventObject( m_ctrl );
        event.SetModel( m_ctrl->GetModel() );
        event.SetItem( item );
        event.SetColumn( col );
        m_ctrl->GetEventHandler()->ProcessEvent( event );
        return true;
    }

    virtual bool ItemAdded( const wxDataViewItem &parent, const wxDataViewItem &item )  
        { return SendEvent( wxEVT_COMMAND_DATAVIEW_MODEL_ITEM_ADDED, item ); }
    virtual bool ItemDeleted( const wxDataViewItem &item )
        { return SendEvent( wxEVT_COMMAND_DATAVIEW_MODEL_ITEM_DELETED, item ); }
    virtual bool ItemChanged( const wxDataViewItem &item )
        { return SendEvent( wxEVT_COMMAND_DATAVIEW_MODEL_ITEM_CHANGED, item ); }
    virtual bool ValueChanged( const wxDataViewItem &item, unsigned int col )
        { return SendEvent( wxEVT_COMMAND_DATAVIEW_MODEL_VALUE_CHANGED, item, col ); }
    virtual bool Cleared()      
        { return SendEvent( wxEVT_COMMAND_DATAVIEW_MODEL_CLEARED, wxDataViewItem(0) ); }

private:
    wxDataViewCtrl *m_ctrl;
};


// ---------------------------------------------------------
// wxDataViewCtrlBase
// ---------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxDataViewCtrlBase, wxControl)

wxDataViewCtrlBase::wxDataViewCtrlBase()
{
    m_model = NULL;
    m_cols.DeleteContents( true );
    m_eventNotifier = NULL;
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
        if (m_eventNotifier)
            m_model->RemoveNotifier( m_eventNotifier );
        m_eventNotifier = NULL;
    
        m_model->DecRef();
        m_model = NULL;
    }
}

bool wxDataViewCtrlBase::AssociateModel( wxDataViewModel *model )
{
    if (m_model)
    {
        if (m_eventNotifier)
            m_model->RemoveNotifier( m_eventNotifier );
        m_eventNotifier = NULL;
        
        m_model->DecRef();   // discard old model, if any
    }

    // add our own reference to the new model:
    m_model = model;
    if (m_model)
    {
        m_model->IncRef(); 
        m_eventNotifier = new wxDataViewEventModelNotifier( (wxDataViewCtrl*) this );
        m_model->AddNotifier( m_eventNotifier );
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
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_COLUMN_SORTED)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_MODEL_ITEM_ADDED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_MODEL_ITEM_DELETED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_MODEL_ITEM_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_MODEL_VALUE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DATAVIEW_MODEL_CLEARED)

#endif

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
    m_notifier = NULL;
}

wxDataViewListModel::~wxDataViewListModel()
{
    if (m_notifier)
        delete m_notifier;
}

bool wxDataViewListModel::RowAppended()
{
    if (m_notifier)
        return m_notifier->RowAppended();
        
    return false;
}

bool wxDataViewListModel::RowPrepended()
{
    if (m_notifier)
        return m_notifier->RowPrepended();
        
    return false;
}

bool wxDataViewListModel::RowInserted( size_t before )
{
    if (m_notifier)
        return m_notifier->RowInserted( before );
        
    return false;
}

bool wxDataViewListModel::RowDeleted( size_t row )
{
    if (m_notifier)
        return m_notifier->RowDeleted( row );
        
    return false;
}

bool wxDataViewListModel::RowChanged( size_t row )
{
    if (m_notifier)
        return m_notifier->RowChanged( row );
        
    return false;
}

bool wxDataViewListModel::ValueChanged( size_t row, size_t col )
{
    if (m_notifier)
        return m_notifier->RowAppended();
        
    return false;
}

bool wxDataViewListModel::Cleared()
{
    if (m_notifier)
        return m_notifier->Cleared();
        
    return false;
}

void wxDataViewListModel::SetNotifier( wxDataViewListModelNotifier *notifier )
{
    if (m_notifier)
        delete m_notifier;
        
   m_notifier = notifier;
}

wxDataViewListModelNotifier* wxDataViewListModel::GetNotifier()
{
    return m_notifier;
}

// --------------------------------------------------------- 
// wxDataViewColumnBase
// --------------------------------------------------------- 

IMPLEMENT_ABSTRACT_CLASS(wxDataViewColumnBase, wxObject)

wxDataViewColumnBase::wxDataViewColumnBase( const wxString &title, wxDataViewCtrl *ctrl, 
    wxDataViewColumnType kind, int flags)
{
    m_ctrl = ctrl;
    m_kind = kind;
    m_flags = flags;
    m_title = title;
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

bool wxDataViewCtrlBase::AppendStringColumn( const wxString &label )
{
    return AppendColumn( new wxDataViewColumn( label, (wxDataViewCtrl*) this, wxDATAVIEW_COL_TEXT ) );
}

bool wxDataViewCtrlBase::AppendColumn( wxDataViewColumn *col )
{
    m_cols.Append( (wxObject*) col );
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


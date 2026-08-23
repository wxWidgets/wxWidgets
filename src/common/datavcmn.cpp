/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/datavcmn.cpp
// Purpose:     wxDataViewCtrl base classes and common parts
// Author:      Robert Roebling
// Created:     2006/02/20
// Copyright:   (c) 2006, Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_DATAVIEWCTRL

#include "wx/dataview.h"

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/settings.h"
    #include "wx/log.h"
    #include "wx/crt.h"
#endif

#include "wx/datectrl.h"
#include "wx/except.h"
#include "wx/spinctrl.h"
#include "wx/choice.h"
#include "wx/imaglist.h"
#include "wx/renderer.h"
#include "wx/uilocale.h"

#if wxUSE_ACCESSIBILITY
    #include "wx/access.h"
#endif // wxUSE_ACCESSIBILITY

#include "wx/private/safecall.h"
#include "wx/private/dataview.h"
#include "wx/scopeguard.h"

#include <algorithm>
#include <unordered_map>
#include <vector>

// Uncomment this line to, for custom renderers, visually show the extent
// of both a cell and its item.
//#define DEBUG_RENDER_EXTENTS

const char wxDataViewCtrlNameStr[] = "dataviewCtrl";

namespace
{

struct DataViewEditState
{
    wxDataViewRendererBase* renderer = nullptr;
    wxPrivate::DataViewEditGeneration generation = 0;
};

std::unordered_map<wxDataViewCtrl*, DataViewEditState>
    gs_dataViewEditStates;
wxPrivate::DataViewEditGeneration gs_nextDataViewEditGeneration = 0;

using DataViewNotifierGeneration = unsigned long long;
std::unordered_map<wxDataViewModelNotifier*, DataViewNotifierGeneration>
    gs_dataViewNotifierGenerations;
DataViewNotifierGeneration gs_nextDataViewNotifierGeneration = 0;

struct DataViewNotifierDispatchState
{
    unsigned int depth = 0;
    bool deletePending = false;
};

std::unordered_map<wxDataViewModelNotifier*, DataViewNotifierDispatchState>
    gs_dataViewNotifierDispatchStates;

bool IsDataViewNotifierGenerationCurrent(
    wxDataViewModelNotifier* notifier,
    DataViewNotifierGeneration generation)
{
    const auto current = gs_dataViewNotifierGenerations.find(notifier);
    return current != gs_dataViewNotifierGenerations.end() &&
           current->second == generation;
}

void DeleteDataViewNotifierWhenIdle(wxDataViewModelNotifier* notifier)
{
    const auto state = gs_dataViewNotifierDispatchStates.find(notifier);
    if ( state == gs_dataViewNotifierDispatchStates.end() )
    {
        delete notifier;
        return;
    }

    if ( state->second.depth )
    {
        state->second.deletePending = true;
        return;
    }

    gs_dataViewNotifierDispatchStates.erase(state);
    delete notifier;
}

template <typename NotifierContainer, typename Callback>
bool DispatchDataViewModelNotification(
    wxDataViewModel* model,
    const NotifierContainer& notifiers,
    Callback&& callback)
{
    model->IncRef();
    const wxScopeGuard releaseModel = wxMakeGuard([model]()
    {
        model->DecRef();
    });
    wxUnusedVar(releaseModel);

    using SnapshotEntry =
        std::pair<wxDataViewModelNotifier*, DataViewNotifierGeneration>;
    std::vector<SnapshotEntry> snapshot;
    snapshot.reserve(notifiers.size());
    for ( wxDataViewModelNotifier* const notifier : notifiers )
    {
        const auto generation = gs_dataViewNotifierGenerations.find(notifier);
        if ( generation != gs_dataViewNotifierGenerations.end() )
            snapshot.emplace_back(notifier, generation->second);
    }

    bool result = true;
    for ( const SnapshotEntry& entry : snapshot )
    {
        const auto generation =
            gs_dataViewNotifierGenerations.find(entry.first);
        if ( generation == gs_dataViewNotifierGenerations.end() ||
                generation->second != entry.second ||
                std::find(notifiers.begin(), notifiers.end(), entry.first) ==
                    notifiers.end() )
        {
            continue;
        }

        DataViewNotifierDispatchState& dispatchState =
            gs_dataViewNotifierDispatchStates[entry.first];
        ++dispatchState.depth;
        bool callbackResult = false;
        {
            const wxScopeGuard leaveNotifier = wxMakeGuard([notifier = entry.first]()
            {
                const auto state =
                    gs_dataViewNotifierDispatchStates.find(notifier);
                wxASSERT(state != gs_dataViewNotifierDispatchStates.end());
                wxASSERT(state->second.depth != 0);

                if ( --state->second.depth == 0 &&
                        state->second.deletePending )
                {
                    gs_dataViewNotifierDispatchStates.erase(state);
                    delete notifier;
                }
            });
            wxUnusedVar(leaveNotifier);

            callbackResult = callback(entry.first);
        }

        if ( !callbackResult )
            result = false;
    }

    return result;
}

// Custom handler pushed on top of the edit control used by wxDataViewCtrl to
// forward some events to the main control itself.
class wxDataViewEditorCtrlEvtHandler: public wxEvtHandler
{
public:
    wxDataViewEditorCtrlEvtHandler(wxWindow *editor, wxDataViewRenderer *owner)
    {
        m_editorCtrl = editor;
        m_owner = owner;
        m_finished = false;
        m_focusOnIdle = false;
    }

#if defined(__WXGTK__) && !defined(wxHAS_GENERIC_DATAVIEWCTRL)
    void SetFocusOnIdle( bool focus = true ) { m_focusOnIdle = focus; }
#endif

protected:
    void OnChar( wxKeyEvent &event );
    void OnTextEnter( wxCommandEvent &event );
    void OnKillFocus( wxFocusEvent &event );
    void OnIdle( wxIdleEvent &event );

private:
    bool IsEditorSubControl(wxWindow* win) const;

    wxDataViewRenderer     *m_owner;
    wxWindow               *m_editorCtrl;
    bool                    m_finished;
    bool                    m_focusOnIdle;

private:
    wxDECLARE_EVENT_TABLE();
};

} // anonymous namespace

namespace wxPrivate
{

DataViewEditGeneration
BeginDataViewEdit(wxDataViewCtrl* ctrl, wxDataViewRendererBase* renderer)
{
    if ( !ctrl || !renderer ||
            gs_dataViewEditStates.find(ctrl) != gs_dataViewEditStates.end() )
    {
        return 0;
    }

    // Zero is reserved for "not entered". Skipping it on wrap keeps that
    // invariant without imposing a practical lifetime limit.
    if ( ++gs_nextDataViewEditGeneration == 0 )
        ++gs_nextDataViewEditGeneration;

    const DataViewEditGeneration generation =
        gs_nextDataViewEditGeneration;
    gs_dataViewEditStates.emplace(
        ctrl, DataViewEditState{ renderer, generation });
    return generation;
}

bool IsCurrentDataViewEdit(wxDataViewCtrl* ctrl,
                           wxDataViewRendererBase* renderer,
                           DataViewEditGeneration generation)
{
    const auto it = gs_dataViewEditStates.find(ctrl);
    return it != gs_dataViewEditStates.end() &&
           it->second.renderer == renderer &&
           it->second.generation == generation;
}

DataViewEditGeneration
GetCurrentDataViewEditGeneration(wxDataViewCtrl* ctrl,
                                 wxDataViewRendererBase* renderer)
{
    const auto it = gs_dataViewEditStates.find(ctrl);
    if ( it == gs_dataViewEditStates.end() ||
            it->second.renderer != renderer )
    {
        return 0;
    }

    return it->second.generation;
}

void EndDataViewEdit(wxDataViewCtrl* ctrl,
                     wxDataViewRendererBase* renderer,
                     DataViewEditGeneration generation)
{
    if ( IsCurrentDataViewEdit(ctrl, renderer, generation) )
        gs_dataViewEditStates.erase(ctrl);
}

void ReleaseDataViewEditForDone(wxDataViewCtrl* ctrl,
                                wxDataViewRendererBase* renderer,
                                DataViewEditGeneration generation)
{
    EndDataViewEdit(ctrl, renderer, generation);
}

} // namespace wxPrivate

// ---------------------------------------------------------
// wxDataViewItemAttr
// ---------------------------------------------------------

wxFont wxDataViewItemAttr::GetEffectiveFont(const wxFont& font) const
{
    if ( !HasFont() )
        return font;

    wxFont f(font);
    if ( GetBold() )
        f.MakeBold();
    if ( GetItalic() )
        f.MakeItalic();
    if ( GetStrikethrough() )
        f.MakeStrikethrough();
    return f;
}


// ---------------------------------------------------------
// wxDataViewModelNotifier
// ---------------------------------------------------------

bool wxDataViewModelNotifier::ItemsAdded( const wxDataViewItem &parent, const wxDataViewItemArray &items )
{
    const auto registered = gs_dataViewNotifierGenerations.find(this);
    const bool trackRegistration =
        registered != gs_dataViewNotifierGenerations.end();
    const DataViewNotifierGeneration generation =
        trackRegistration ? registered->second : 0;

    const size_t count = items.GetCount();
    for ( size_t i = 0; i < count; ++i )
    {
        if ( trackRegistration &&
                !IsDataViewNotifierGenerationCurrent(this, generation) )
        {
            break;
        }

        if ( !ItemAdded(parent, items[i]) )
            return false;
    }

    return true;
}

bool wxDataViewModelNotifier::ItemsDeleted( const wxDataViewItem &parent, const wxDataViewItemArray &items )
{
    const auto registered = gs_dataViewNotifierGenerations.find(this);
    const bool trackRegistration =
        registered != gs_dataViewNotifierGenerations.end();
    const DataViewNotifierGeneration generation =
        trackRegistration ? registered->second : 0;

    const size_t count = items.GetCount();
    for ( size_t i = 0; i < count; ++i )
    {
        if ( trackRegistration &&
                !IsDataViewNotifierGenerationCurrent(this, generation) )
        {
            break;
        }

        if ( !ItemDeleted(parent, items[i]) )
            return false;
    }

    return true;
}

bool wxDataViewModelNotifier::ItemsChanged( const wxDataViewItemArray &items )
{
    const auto registered = gs_dataViewNotifierGenerations.find(this);
    const bool trackRegistration =
        registered != gs_dataViewNotifierGenerations.end();
    const DataViewNotifierGeneration generation =
        trackRegistration ? registered->second : 0;

    const size_t count = items.GetCount();
    for ( size_t i = 0; i < count; ++i )
    {
        if ( trackRegistration &&
                !IsDataViewNotifierGenerationCurrent(this, generation) )
        {
            break;
        }

        if ( !ItemChanged(items[i]) )
            return false;
    }

    return true;
}

// ---------------------------------------------------------
// wxDataViewModel
// ---------------------------------------------------------

wxDataViewModel::wxDataViewModel()
{
}

wxDataViewModel::~wxDataViewModel()
{
    for ( wxDataViewModelNotifier* const notifier : m_notifiers )
    {
        gs_dataViewNotifierGenerations.erase(notifier);
        notifier->SetOwner(nullptr);
        DeleteDataViewNotifierWhenIdle(notifier);
    }
}

bool wxDataViewModel::ItemAdded( const wxDataViewItem &parent, const wxDataViewItem &item )
{
    return DispatchDataViewModelNotification(
        this, m_notifiers, [&](wxDataViewModelNotifier* notifier)
    {
        return notifier->ItemAdded(parent, item);
    });
}

bool wxDataViewModel::ItemDeleted( const wxDataViewItem &parent, const wxDataViewItem &item )
{
    return DispatchDataViewModelNotification(
        this, m_notifiers, [&](wxDataViewModelNotifier* notifier)
    {
        return notifier->ItemDeleted(parent, item);
    });
}

bool wxDataViewModel::ItemChanged( const wxDataViewItem &item )
{
    return DispatchDataViewModelNotification(
        this, m_notifiers, [&](wxDataViewModelNotifier* notifier)
    {
        return notifier->ItemChanged(item);
    });
}

bool wxDataViewModel::ItemsAdded( const wxDataViewItem &parent, const wxDataViewItemArray &items )
{
    return DispatchDataViewModelNotification(
        this, m_notifiers, [&](wxDataViewModelNotifier* notifier)
    {
        return notifier->ItemsAdded(parent, items);
    });
}

bool wxDataViewModel::ItemsDeleted( const wxDataViewItem &parent, const wxDataViewItemArray &items )
{
    return DispatchDataViewModelNotification(
        this, m_notifiers, [&](wxDataViewModelNotifier* notifier)
    {
        return notifier->ItemsDeleted(parent, items);
    });
}

bool wxDataViewModel::ItemsChanged( const wxDataViewItemArray &items )
{
    return DispatchDataViewModelNotification(
        this, m_notifiers, [&](wxDataViewModelNotifier* notifier)
    {
        return notifier->ItemsChanged(items);
    });
}

bool wxDataViewModel::ValueChanged( const wxDataViewItem &item, unsigned int col )
{
    return DispatchDataViewModelNotification(
        this, m_notifiers, [&](wxDataViewModelNotifier* notifier)
    {
        return notifier->ValueChanged(item, col);
    });
}

bool wxDataViewModel::Cleared()
{
    return DispatchDataViewModelNotification(
        this, m_notifiers, [](wxDataViewModelNotifier* notifier)
    {
        return notifier->Cleared();
    });
}

bool wxDataViewModel::BeforeReset()
{
    return DispatchDataViewModelNotification(
        this, m_notifiers, [](wxDataViewModelNotifier* notifier)
    {
        return notifier->BeforeReset();
    });
}

bool wxDataViewModel::AfterReset()
{
    return DispatchDataViewModelNotification(
        this, m_notifiers, [](wxDataViewModelNotifier* notifier)
    {
        return notifier->AfterReset();
    });
}

void wxDataViewModel::Resort()
{
    DispatchDataViewModelNotification(
        this, m_notifiers, [](wxDataViewModelNotifier* notifier)
    {
        notifier->Resort();
        return true;
    });
}

void wxDataViewModel::AddNotifier( wxDataViewModelNotifier *notifier )
{
    if ( ++gs_nextDataViewNotifierGeneration == 0 )
        ++gs_nextDataViewNotifierGeneration;
    gs_dataViewNotifierGenerations[notifier] =
        gs_nextDataViewNotifierGeneration;
    gs_dataViewNotifierDispatchStates.try_emplace(notifier);

    m_notifiers.push_back( notifier );
    notifier->SetOwner( this );
}

void wxDataViewModel::RemoveNotifier( wxDataViewModelNotifier *notifier )
{
    wxDataViewModelNotifiers::iterator iter;
    for (iter = m_notifiers.begin(); iter != m_notifiers.end(); ++iter)
    {
        if ( *iter == notifier )
        {
            gs_dataViewNotifierGenerations.erase(notifier);
            m_notifiers.erase(iter);
            notifier->SetOwner(nullptr);
            DeleteDataViewNotifierWhenIdle(notifier);

            // Skip the assert below.
            return;
        }
    }

    wxFAIL_MSG(wxS("Removing non-registered notifier"));
}

int wxDataViewModel::Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
                              unsigned int column, bool ascending ) const
{
    wxVariant value1,value2;

    // Avoid calling GetValue() for the cells that are not supposed to have any
    // value, this might be unexpected.
    if ( HasValue(item1, column) )
        GetValue( value1, item1, column );
    if ( HasValue(item2, column) )
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
        if (res)
            return res;
    }
    else if (value1.GetType() == wxT("long"))
    {
        long l1 = value1.GetLong();
        long l2 = value2.GetLong();
        if (l1 < l2)
            return -1;
        else if (l1 > l2)
            return 1;
    }
    else if (value1.GetType() == wxT("double"))
    {
        double d1 = value1.GetDouble();
        double d2 = value2.GetDouble();
        if (d1 < d2)
            return -1;
        else if (d1 > d2)
            return 1;
    }
#if wxUSE_DATETIME
    else if (value1.GetType() == wxT("datetime"))
    {
        wxDateTime dt1 = value1.GetDateTime();
        wxDateTime dt2 = value2.GetDateTime();
        if (dt1.IsEarlierThan(dt2))
            return -1;
        if (dt2.IsEarlierThan(dt1))
            return 1;
    }
#endif // wxUSE_DATETIME
    else if (value1.GetType() == wxT("bool"))
    {
        bool b1 = value1.GetBool();
        bool b2 = value2.GetBool();

        if (b1 != b2)
            return b1 ? 1 : -1;
    }
    else if (value1.GetType() == wxT("wxDataViewIconText"))
    {
        wxDataViewIconText iconText1, iconText2;

        iconText1 << value1;
        iconText2 << value2;

        int res = iconText1.GetText().Cmp(iconText2.GetText());
        if (res != 0)
          return res;
    }
    else
    {
        int res = DoCompareValues(value1, value2);
        if (res != 0)
            return res;
    }


    // items must be different
    wxUIntPtr id1 = wxPtrToUInt(item1.GetID()),
              id2 = wxPtrToUInt(item2.GetID());

    return ascending ? id1 - id2 : id2 - id1;
}

// ---------------------------------------------------------
// wxDataViewIndexListModel
// ---------------------------------------------------------

static int my_sort( int *v1, int *v2 )
{
   return *v2-*v1;
}


wxDataViewIndexListModel::wxDataViewIndexListModel( unsigned int initial_size )
{
    // IDs are ordered until an item gets deleted or inserted
    m_ordered = true;

    // build initial index
    unsigned int i;
    for (i = 1; i < initial_size+1; i++)
            m_hash.Add( wxDataViewItem(wxUIntToPtr(i)) );
    m_nextFreeID = initial_size + 1;
}

void wxDataViewIndexListModel::Reset( unsigned int new_size )
{
    /* wxDataViewModel:: */ BeforeReset();

    m_hash.Clear();

    // IDs are ordered until an item gets deleted or inserted
    m_ordered = true;

    // build initial index
    unsigned int i;
    for (i = 1; i < new_size+1; i++)
            m_hash.Add( wxDataViewItem(wxUIntToPtr(i)) );

    m_nextFreeID = new_size + 1;

    /* wxDataViewModel:: */ AfterReset();
}

void wxDataViewIndexListModel::RowPrepended()
{
    m_ordered = false;

    unsigned int id = m_nextFreeID;
    m_nextFreeID++;

    wxDataViewItem item( wxUIntToPtr(id) );
    m_hash.Insert( item, 0 );
    ItemAdded( wxDataViewItem(nullptr), item );

}

void wxDataViewIndexListModel::RowInserted( unsigned int before )
{
    m_ordered = false;

    unsigned int id = m_nextFreeID;
    m_nextFreeID++;

    wxDataViewItem item( wxUIntToPtr(id) );
    m_hash.Insert( item, before );
    ItemAdded( wxDataViewItem(nullptr), item );
}

void wxDataViewIndexListModel::RowAppended()
{
    unsigned int id = m_nextFreeID;
    m_nextFreeID++;

    wxDataViewItem item( wxUIntToPtr(id) );
    m_hash.Add( item );
    ItemAdded( wxDataViewItem(nullptr), item );
}

void wxDataViewIndexListModel::RowDeleted( unsigned int row )
{
    m_ordered = false;

    wxDataViewItem item( m_hash[row] );
    m_hash.RemoveAt( row );
    /* wxDataViewModel:: */ ItemDeleted( wxDataViewItem(nullptr), item );
}

void wxDataViewIndexListModel::RowsDeleted( const wxArrayInt &rows )
{
    m_ordered = false;

    wxDataViewItemArray array;
    unsigned int i;
    for (i = 0; i < rows.GetCount(); i++)
    {
            wxDataViewItem item( m_hash[rows[i]] );
            array.Add( item );
    }

    wxArrayInt sorted = rows;
    sorted.Sort( my_sort );
    for (i = 0; i < sorted.GetCount(); i++)
           m_hash.RemoveAt( sorted[i] );

    /* wxDataViewModel:: */ ItemsDeleted( wxDataViewItem(nullptr), array );
}

void wxDataViewIndexListModel::RowChanged( unsigned int row )
{
    /* wxDataViewModel:: */ ItemChanged( GetItem(row) );
}

void wxDataViewIndexListModel::RowValueChanged( unsigned int row, unsigned int col )
{
    /* wxDataViewModel:: */ ValueChanged( GetItem(row), col );
}

unsigned int wxDataViewIndexListModel::GetRow( const wxDataViewItem &item ) const
{
    if (m_ordered)
        return wxPtrToUInt(item.GetID())-1;

    // assert for not found
    return (unsigned int) m_hash.Index( item );
}

wxDataViewItem wxDataViewIndexListModel::GetItem( unsigned int row ) const
{
    wxCHECK_MSG( row < m_hash.GetCount(), wxDataViewItem(), wxS("invalid index") );
    return wxDataViewItem( m_hash[row] );
}

unsigned int wxDataViewIndexListModel::GetChildren( const wxDataViewItem &item, wxDataViewItemArray &children ) const
{
    if (item.IsOk())
        return 0;

    children = m_hash;

    return m_hash.GetCount();
}

// ---------------------------------------------------------
// wxDataViewVirtualListModel
// ---------------------------------------------------------

#ifndef __WXMAC__

wxDataViewVirtualListModel::wxDataViewVirtualListModel( unsigned int initial_size )
{
    m_size = initial_size;
}

void wxDataViewVirtualListModel::Reset( unsigned int new_size )
{
    /* wxDataViewModel:: */ BeforeReset();

    m_size = new_size;

    /* wxDataViewModel:: */ AfterReset();
}

void wxDataViewVirtualListModel::RowPrepended()
{
    m_size++;
    wxDataViewItem item( wxUIntToPtr(1) );
    ItemAdded( wxDataViewItem(nullptr), item );
}

void wxDataViewVirtualListModel::RowInserted( unsigned int before )
{
    m_size++;
    wxDataViewItem item( wxUIntToPtr(before+1) );
    ItemAdded( wxDataViewItem(nullptr), item );
}

void wxDataViewVirtualListModel::RowAppended()
{
    m_size++;
    wxDataViewItem item( wxUIntToPtr(m_size) );
    ItemAdded( wxDataViewItem(nullptr), item );
}

void wxDataViewVirtualListModel::RowDeleted( unsigned int row )
{
    m_size--;
    wxDataViewItem item( wxUIntToPtr(row+1) );
    /* wxDataViewModel:: */ ItemDeleted( wxDataViewItem(nullptr), item );
}

void wxDataViewVirtualListModel::RowsDeleted( const wxArrayInt &rows )
{
    m_size -= rows.GetCount();

    wxArrayInt sorted = rows;
    sorted.Sort( my_sort );

    wxDataViewItemArray array;
    unsigned int i;
    for (i = 0; i < sorted.GetCount(); i++)
    {
        wxDataViewItem item( wxUIntToPtr(sorted[i]+1) );
        array.Add( item );
    }
    /* wxDataViewModel:: */ ItemsDeleted( wxDataViewItem(nullptr), array );
}

void wxDataViewVirtualListModel::RowChanged( unsigned int row )
{
    /* wxDataViewModel:: */ ItemChanged( GetItem(row) );
}

void wxDataViewVirtualListModel::RowValueChanged( unsigned int row, unsigned int col )
{
    /* wxDataViewModel:: */ ValueChanged( GetItem(row), col );
}

unsigned int wxDataViewVirtualListModel::GetRow( const wxDataViewItem &item ) const
{
    return wxPtrToUInt( item.GetID() ) -1;
}

wxDataViewItem wxDataViewVirtualListModel::GetItem( unsigned int row ) const
{
    return wxDataViewItem( wxUIntToPtr(row+1) );
}

bool wxDataViewVirtualListModel::HasDefaultCompare() const
{
    return true;
}

int wxDataViewVirtualListModel::Compare(const wxDataViewItem& item1,
                                      const wxDataViewItem& item2,
                                      unsigned int WXUNUSED(column),
                                      bool ascending) const
{
    unsigned int pos1 = wxPtrToUInt(item1.GetID());  // -1 not needed here
    unsigned int pos2 = wxPtrToUInt(item2.GetID());  // -1 not needed here

    if (ascending)
       return pos1 - pos2;
    else
       return pos2 - pos1;
}

unsigned int wxDataViewVirtualListModel::GetChildren( const wxDataViewItem &WXUNUSED(item), wxDataViewItemArray &WXUNUSED(children) ) const
{
    return 0;  // should we report an error ?
}

#endif  // __WXMAC__

//-----------------------------------------------------------------------------
// wxDataViewIconText
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxDataViewIconText,wxObject);

IMPLEMENT_VARIANT_OBJECT_EXPORTED(wxDataViewIconText, WXDLLIMPEXP_ADV)

// ---------------------------------------------------------
// wxDataViewRendererBase
// ---------------------------------------------------------

namespace
{

bool IsDataViewRendererOwned(wxDataViewCtrl* ctrl,
                             wxDataViewColumn* column,
                             const wxDataViewRendererBase* renderer)
{
    if ( !ctrl || !column )
        return false;

    for ( unsigned int n = 0; n < ctrl->GetColumnCount(); ++n )
    {
        if ( ctrl->GetColumn(n) == column )
            return column->GetRenderer() == renderer;
    }

    return false;
}

} // anonymous namespace

wxIMPLEMENT_ABSTRACT_CLASS(wxDataViewRendererBase, wxObject);

wxDataViewRendererBase::wxDataViewRendererBase( const wxString &varianttype,
                                                wxDataViewCellMode WXUNUSED(mode),
                                                int WXUNUSED(align) )
    : m_variantType(varianttype)
{
    m_owner = nullptr;
    m_valueAdjuster = nullptr;
}

wxDataViewRendererBase::~wxDataViewRendererBase()
{
    if ( m_editorCtrl )
        DestroyEditControl();
    delete m_valueAdjuster;
}

wxDataViewCtrl* wxDataViewRendererBase::GetView() const
{
    return const_cast<wxDataViewRendererBase*>(this)->GetOwner()->GetOwner();
}

bool wxDataViewRendererBase::StartEditing( const wxDataViewItem &item, wxRect labelRect )
{
    wxDataViewColumn* const column = GetOwner();
    wxDataViewCtrl* const dv_ctrl = column ? column->GetOwner() : nullptr;
    if ( !dv_ctrl )
        return false;

    const wxWeakRef<wxDataViewCtrl> weakCtrl(dv_ctrl);

    // Before doing anything we send an event asking if editing of this item is really wanted.
    wxDataViewEvent event(wxEVT_DATAVIEW_ITEM_START_EDITING, dv_ctrl, column, item);
    dv_ctrl->GetEventHandler()->ProcessEvent( event );
    if ( !event.IsAllowed() ||
            weakCtrl.get() != dv_ctrl ||
            !IsDataViewRendererOwned(dv_ctrl, column, this) )
    {
        return false;
    }

    // Remember the item being edited for use in FinishEditing() later.
    m_item = item;

    const unsigned int col = column->GetModelColumn();
    wxDataViewModel* const model = dv_ctrl->GetModel();
    if ( model )
        model->IncRef();
    const wxScopeGuard releaseModel = wxMakeGuard([model]()
    {
        if ( model )
            model->DecRef();
    });
    wxUnusedVar(releaseModel);

    const wxVariant value = CheckedGetValue(model, item, col);
    if ( weakCtrl.get() != dv_ctrl ||
            dv_ctrl->GetModel() != model ||
            !IsDataViewRendererOwned(dv_ctrl, column, this) )
    {
        return false;
    }

    wxWindow* const editor =
        CreateEditorCtrl(dv_ctrl->GetMainWindow(), labelRect, value);
    if ( weakCtrl.get() != dv_ctrl ||
            !IsDataViewRendererOwned(dv_ctrl, column, this) )
    {
        return false;
    }
    if ( dv_ctrl->GetModel() != model ||
            column->GetModelColumn() != col )
    {
        if ( editor )
            editor->Destroy();
        m_item = wxDataViewItem();
        return false;
    }
    m_editorCtrl = editor;

    // there might be no editor control for the given item
    if(!m_editorCtrl)
    {
        m_item = wxDataViewItem();
        return false;
    }

    wxDataViewEditorCtrlEvtHandler *handler =
        new wxDataViewEditorCtrlEvtHandler( m_editorCtrl, (wxDataViewRenderer*) this );

    m_editorCtrl->PushEventHandler( handler );

#if defined(__WXGTK__) && !defined(wxHAS_GENERIC_DATAVIEWCTRL)
    handler->SetFocusOnIdle();
#else
    m_editorCtrl->SetFocus();
#endif

    // SetFocus() is another application callback boundary.
    if ( weakCtrl.get() == dv_ctrl &&
            IsDataViewRendererOwned(dv_ctrl, column, this) )
    {
        if ( dv_ctrl->GetModel() == model &&
                column->GetModelColumn() == col )
        {
            return true;
        }

        m_item = wxDataViewItem();
        DestroyEditControl();
    }

    return false;
}

void wxDataViewRendererBase::NotifyEditingStarted(const wxDataViewItem& item)
{
    wxDataViewColumn* const column = GetOwner();
    wxDataViewCtrl* const dv_ctrl = column->GetOwner();

    wxDataViewEvent event(wxEVT_DATAVIEW_ITEM_EDITING_STARTED, dv_ctrl, column, item);
    dv_ctrl->GetEventHandler()->ProcessEvent( event );
}

void wxDataViewRendererBase::DestroyEditControl()
{
    wxWindow* const editor = m_editorCtrl.get();

    // Stop publishing the editor before hiding it: Hide() moves focus and can
    // run arbitrary application handlers, including ones deleting this
    // renderer or the whole data view.
    m_editorCtrl.Release();
    const wxWeakRef<wxWindow> weakEditor(editor);

    // Remove our event handler first to prevent it from (recursively) calling
    // us again as it would do via a call to FinishEditing() when the editor
    // loses focus when we hide it below.
    wxEvtHandler * const handler = editor->PopEventHandler();

    // Hide the control immediately but don't delete it yet as there could be
    // some pending messages for it.
    editor->Hide();

    wxPendingDelete.Append(handler);
    if ( weakEditor.get() == editor )
        wxPendingDelete.Append(editor);
}

void wxDataViewRendererBase::CancelEditing()
{
    wxDataViewColumn* const column = GetOwner();
    wxDataViewCtrl* const dv_ctrl = column ? column->GetOwner() : nullptr;
    const wxWeakRef<wxDataViewCtrl> weakCtrl(dv_ctrl);
    wxDataViewModel* const model = dv_ctrl ? dv_ctrl->GetModel() : nullptr;
    if ( model )
        model->IncRef();
    const wxScopeGuard releaseModel = wxMakeGuard([model]()
    {
        if ( model )
            model->DecRef();
    });
    wxUnusedVar(releaseModel);

    if ( m_editorCtrl )
        DestroyEditControl();

    if ( weakCtrl.get() != dv_ctrl ||
            !dv_ctrl ||
            dv_ctrl->GetModel() != model ||
            !IsDataViewRendererOwned(dv_ctrl, column, this) )
    {
        return;
    }

    DoHandleEditingDone(nullptr);
}

bool wxDataViewRendererBase::FinishEditing()
{
    if (!m_editorCtrl)
        return true;

    wxDataViewColumn* const column = GetOwner();
    wxDataViewCtrl* const dv_ctrl = column ? column->GetOwner() : nullptr;
    if ( !dv_ctrl )
        return false;

    const wxWeakRef<wxDataViewCtrl> weakCtrl(dv_ctrl);
    wxDataViewModel* const model = dv_ctrl->GetModel();
    if ( model )
        model->IncRef();
    const wxScopeGuard releaseModel = wxMakeGuard([model]()
    {
        if ( model )
            model->DecRef();
    });
    wxUnusedVar(releaseModel);

    bool gotValue = false;

    wxWindow* const editor = m_editorCtrl.get();
    const wxWeakRef<wxWindow> weakEditor(editor);
    wxVariant value;
    if ( GetValueFromEditorCtrl(editor, value) )
    {
        // This is the normal case and we will use this value below (if it
        // passes validation).
        gotValue = true;
    }
    //else: Not really supposed to happen, but still proceed with
    //      destroying the edit control if it does.

    if ( weakCtrl.get() != dv_ctrl ||
            dv_ctrl->GetModel() != model ||
            !IsDataViewRendererOwned(dv_ctrl, column, this) ||
            weakEditor.get() != editor ||
            m_editorCtrl.get() != editor )
    {
        return false;
    }

    DestroyEditControl();

    if ( weakCtrl.get() != dv_ctrl ||
            dv_ctrl->GetModel() != model ||
            !IsDataViewRendererOwned(dv_ctrl, column, this) )
    {
        return false;
    }

    dv_ctrl->GetMainWindow()->SetFocus();
    if ( weakCtrl.get() != dv_ctrl ||
            dv_ctrl->GetModel() != model ||
            !IsDataViewRendererOwned(dv_ctrl, column, this) )
    {
        return false;
    }

    return DoHandleEditingDone(gotValue ? &value : nullptr);
}

bool
wxDataViewRendererBase::DoHandleEditingDone(wxVariant* value)
{
    wxDataViewColumn* const column = GetOwner();
    wxDataViewCtrl* const dv_ctrl = column ? column->GetOwner() : nullptr;
    if ( !dv_ctrl )
    {
        m_item = wxDataViewItem();
        return false;
    }

    const wxDataViewItem item = m_item;
    wxDataViewModel* const model = dv_ctrl->GetModel();
    const wxWeakRef<wxDataViewCtrl> weakCtrl(dv_ctrl);
    const wxPrivate::DataViewEditGeneration generation =
        wxPrivate::GetCurrentDataViewEditGeneration(dv_ctrl, this);

    // From this point on no member of the renderer may be used after invoking
    // user code. Keep the model alive independently of the control and clear
    // the editing item before either Validate() or the public event can
    // re-enter the renderer.
    m_item = wxDataViewItem();
    if ( model )
        model->IncRef();
    const wxScopeGuard releaseModel = wxMakeGuard([model]()
    {
        if ( model )
            model->DecRef();
    });
    wxUnusedVar(releaseModel);

    if ( value )
    {
        if ( !Validate(*value) )
        {
            // Invalid value can't be used, so if it's the same as if we hadn't
            // got it in the first place.
            value = nullptr;
        }
    }

    if ( weakCtrl.get() != dv_ctrl || dv_ctrl->GetModel() != model ||
            (generation &&
             !wxPrivate::IsCurrentDataViewEdit(
                 dv_ctrl, this, generation)) )
    {
        return false;
    }

    bool columnStillOwned = false;
    for ( unsigned int n = 0; n < dv_ctrl->GetColumnCount(); ++n )
    {
        if ( dv_ctrl->GetColumn(n) == column )
        {
            columnStillOwned = true;
            break;
        }
    }
    if ( !columnStillOwned )
        return false;

    // Validation and value extraction belong to the old transaction and must
    // not be interleaved with another editor. The public editing-done event,
    // however, is the documented point where starting a new editor is legal.
    wxPrivate::ReleaseDataViewEditForDone(dv_ctrl, this, generation);

    // Now we should send Editing Done event
    wxDataViewEvent event(wxEVT_DATAVIEW_ITEM_EDITING_DONE, dv_ctrl, column, item);
    if ( value )
        event.SetValue(*value);
    else
        event.SetEditCancelled();

    dv_ctrl->GetEventHandler()->ProcessEvent( event );

    bool accepted = false;
    if ( value &&
            event.IsAllowed() &&
            weakCtrl.get() == dv_ctrl &&
            dv_ctrl->GetModel() == model &&
            model )
    {
        // The editing-done handler may have inserted or removed columns.
        // Re-find the same column identity and use its current model mapping;
        // never apply the value to a stale positional index.
        for ( unsigned int n = 0; n < dv_ctrl->GetColumnCount(); ++n )
        {
            if ( dv_ctrl->GetColumn(n) == column )
            {
                model->ChangeValue(
                    *value,
                    item,
                    column->GetModelColumn());
                accepted = true;
                break;
            }
        }
    }

    return accepted;
}

wxVariant
wxDataViewRendererBase::CheckedGetValue(const wxDataViewModel* model,
                                        const wxDataViewItem& item,
                                        unsigned column) const
{
    if ( !model )
        return wxVariant();

    wxDataViewColumn* const ownerColumn =
        const_cast<wxDataViewRendererBase*>(this)->GetOwner();
    wxDataViewCtrl* const ownerCtrl =
        ownerColumn ? ownerColumn->GetOwner() : nullptr;
    const wxWeakRef<wxDataViewCtrl> weakCtrl(ownerCtrl);
    const wxString rendererVariantType = GetVariantType();

    wxDataViewModel* const modelRef =
        const_cast<wxDataViewModel*>(model);
    modelRef->IncRef();
    const wxScopeGuard releaseModel = wxMakeGuard([modelRef]()
    {
        modelRef->DecRef();
    });
    wxUnusedVar(releaseModel);

    const auto rendererStillOwned = [&]()
    {
        return !ownerCtrl ||
                (weakCtrl.get() == ownerCtrl &&
                 ownerCtrl->GetModel() == model &&
                 IsDataViewRendererOwned(ownerCtrl, ownerColumn, this) &&
                 ownerColumn->GetModelColumn() == column);
    };

    wxVariant value;
    // Avoid calling GetValue() if the model isn't supposed to have any values
    // in this cell (e.g. a non-first column of a container item), this could
    // be unexpected.
    if ( model->HasValue(item, column) )
    {
        if ( !rendererStillOwned() )
            return wxVariant();

        model->GetValue(value, item, column);
        if ( !rendererStillOwned() )
            return wxVariant();
    }

    // We always allow the cell to be null, regardless of the renderer type.
    if ( !value.IsNull() )
    {
        const bool isCompatible = IsCompatibleVariantType(value.GetType());
        if ( !rendererStillOwned() )
            return wxVariant();

        if ( !isCompatible )
        {
            // If you're seeing this message, this indicates that either your
            // renderer is using the wrong type, or your model returns values
            // of the wrong type.
            wxLogDebug("Wrong type returned from the model for column %u: "
                       "%s required but actual type is %s",
                       column,
                       rendererVariantType,
                       value.GetType());

            // Don't return data of mismatching type, this could be unexpected.
            value.MakeNull();
        }
    }

    return value;
}

bool
wxDataViewRendererBase::PrepareForItem(const wxDataViewModel *model,
                                       const wxDataViewItem& item,
                                       unsigned column)
{
    if ( !model )
        return false;

    wxDataViewColumn* const ownerColumn = GetOwner();
    wxDataViewCtrl* const ownerCtrl =
        ownerColumn ? ownerColumn->GetOwner() : nullptr;
    const wxWeakRef<wxDataViewCtrl> weakCtrl(ownerCtrl);

    wxDataViewModel* const modelRef =
        const_cast<wxDataViewModel*>(model);
    modelRef->IncRef();
    const wxScopeGuard releaseModel = wxMakeGuard([modelRef]()
    {
        modelRef->DecRef();
    });
    wxUnusedVar(releaseModel);

    // All methods called below can be implemented by application code. In
    // particular, they can delete a column, replace the model or destroy the
    // control, so never touch the renderer again unless it is still owned by
    // the same cell.
    const auto rendererStillOwned = [&]()
    {
        return !ownerCtrl ||
               (weakCtrl.get() == ownerCtrl &&
                ownerCtrl->GetModel() == model &&
                IsDataViewRendererOwned(ownerCtrl, ownerColumn, this) &&
                ownerColumn->GetModelColumn() == column);
    };

    // This method is called by the native control, so we shouldn't allow
    // exceptions to escape from it.
    return wxSafeCall<bool>([&, this]()
    {

    // Now check if we have a value and remember it if we do.
    wxVariant value = CheckedGetValue(model, item, column);
    if ( !rendererStillOwned() )
        return false;

    if ( !value.IsNull() )
    {
        if ( m_valueAdjuster )
        {
            const bool highlighted = IsHighlighted();
            if ( !rendererStillOwned() )
                return false;

            if ( highlighted )
            {
                // IsHighlighted() is virtual and may replace the adjuster
                // while keeping this renderer installed. Never retain the old
                // adjuster pointer across it.
                if ( m_valueAdjuster )
                {
                    value = m_valueAdjuster->MakeHighlighted(value);
                    if ( !rendererStillOwned() )
                        return false;
                }
            }
        }

        SetValue(value);
        if ( !rendererStillOwned() )
            return false;
    }

    // Also set up the attributes: note that we need to do this even for the
    // empty cells because background colour is still relevant for them.
    wxDataViewItemAttr attr;
    model->GetAttr(item, column, attr);
    if ( !rendererStillOwned() )
        return false;

    SetAttr(attr);
    if ( !rendererStillOwned() )
        return false;

    // Finally determine the enabled/disabled state and apply it, even to the
    // empty cells.
    const bool enabled = model->IsEnabled(item, column);
    if ( !rendererStillOwned() )
        return false;

    SetEnabled(enabled);
    if ( !rendererStillOwned() )
        return false;

    return !value.IsNull();
    }, []()
    {
        // There is not much we can do about it here, just log it and don't
        // show anything in this cell.
        wxLogDebug("Retrieving the value from the model threw an exception");
        return false;
    });
}


int wxDataViewRendererBase::GetEffectiveAlignment() const
{
    int alignment = GetEffectiveAlignmentIfKnown();
    wxASSERT( alignment != wxDVR_DEFAULT_ALIGNMENT );
    return alignment;
}


int wxDataViewRendererBase::GetEffectiveAlignmentIfKnown() const
{
    int alignment = GetAlignment();

    if ( alignment == wxDVR_DEFAULT_ALIGNMENT )
    {
        if ( GetOwner() != nullptr )
        {
            // if we don't have an explicit alignment ourselves, use that of the
            // column in horizontal direction and default vertical alignment
            alignment = GetOwner()->GetAlignment() | wxALIGN_CENTRE_VERTICAL;
        }
    }

    return alignment;
}

// ----------------------------------------------------------------------------
// wxDataViewCustomRendererBase
// ----------------------------------------------------------------------------

bool wxDataViewCustomRendererBase::ActivateCell(const wxRect& cell,
                                                wxDataViewModel *model,
                                                const wxDataViewItem & item,
                                                unsigned int col,
                                                const wxMouseEvent* mouseEvent)
{
    // Compatibility code
    if ( mouseEvent )
        return LeftClick(mouseEvent->GetPosition(), cell, model, item, col);
    else
        return Activate(cell, model, item, col);
}

void wxDataViewCustomRendererBase::RenderBackground(wxDC* dc, const wxRect& rect)
{
    if ( !m_attr.HasBackgroundColour() )
        return;

    const wxColour& colour = m_attr.GetBackgroundColour();
    wxDCPenChanger changePen(*dc, colour);
    wxDCBrushChanger changeBrush(*dc, colour);

    dc->DrawRectangle(rect);
}

void
wxDataViewCustomRendererBase::WXCallRender(wxRect rectCell, wxDC *dc, int state)
{
    wxCHECK_RET( dc, "no DC to draw on in custom renderer?" );

    // adjust the rectangle ourselves to account for the alignment
    wxRect rectItem = rectCell;
    const int align = GetEffectiveAlignment();

    const wxSize size = GetSize();

    // take alignment into account only if there is enough space, otherwise
    // show as much contents as possible
    //
    // notice that many existing renderers (e.g. wxDataViewSpinRenderer)
    // return hard-coded size which can be more than they need and if we
    // trusted their GetSize() we'd draw the text out of cell bounds
    // entirely

    if ( size.x >= 0 && size.x < rectCell.width )
    {
        if ( align & wxALIGN_CENTER_HORIZONTAL )
            rectItem.x += (rectCell.width - size.x)/2;
        else if ( align & wxALIGN_RIGHT )
            rectItem.x += rectCell.width - size.x;
        // else: wxALIGN_LEFT is the default

        rectItem.width = size.x;
    }

    if ( size.y >= 0 && size.y < rectCell.height )
    {
        if ( align & wxALIGN_CENTER_VERTICAL )
            rectItem.y += (rectCell.height - size.y)/2;
        else if ( align & wxALIGN_BOTTOM )
            rectItem.y += rectCell.height - size.y;
        // else: wxALIGN_TOP is the default

        rectItem.height = size.y;
    }


    // set up the DC attributes

    // override custom foreground with the standard one for the selected items
    // because we currently don't allow changing the selection background and
    // custom colours may be unreadable on it
    wxColour col;
    if ( state & wxDATAVIEW_CELL_SELECTED )
        col = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    else if ( m_attr.HasColour() )
        col = m_attr.GetColour();
    else // use default foreground
        col = GetOwner()->GetOwner()->GetForegroundColour();

    wxDCTextColourChanger changeFg(*dc, col);

    wxDCFontChanger changeFont(*dc);
    if ( m_attr.HasFont() )
        changeFont.Set(m_attr.GetEffectiveFont(dc->GetFont()));

#ifdef DEBUG_RENDER_EXTENTS
    {

    wxDCBrushChanger changeBrush(*dc, *wxTRANSPARENT_BRUSH);
    wxDCPenChanger changePen(*dc, *wxRED);

    dc->DrawRectangle(rectCell);

    dc->SetPen(*wxGREEN);
    dc->DrawRectangle(rectItem);

    }
#endif

    Render(rectItem, dc, state);
}

wxSize wxDataViewCustomRendererBase::GetTextExtent(const wxString& str) const
{
    const wxDataViewCtrl *view = GetView();

    if ( m_attr.HasFont() )
    {
        wxFont font(m_attr.GetEffectiveFont(view->GetFont()));
        wxSize size;
        view->GetTextExtent(str, &size.x, &size.y, nullptr, nullptr, &font);
        return size;
    }
    else
    {
        return view->GetTextExtent(str);
    }
}

void
wxDataViewCustomRendererBase::RenderText(const wxString& text,
                                         int xoffset,
                                         wxRect rect,
                                         wxDC *dc,
                                         int state)
{
    wxRect rectText = rect;
    rectText.x += xoffset;
    rectText.width -= xoffset;

    int flags = 0;
    if ( state & wxDATAVIEW_CELL_SELECTED )
        flags |= wxCONTROL_SELECTED;

    // Use IsThisEnabled() rather than IsEnabled() to grey the text out if the
    // item itself or the entire control is disabled, but not if it's
    // implicitly disabled due to its parent being disabled.
    if ( !(GetOwner()->GetOwner()->IsThisEnabled() && GetEnabled()) )
        flags |= wxCONTROL_DISABLED;

    wxRendererNative::Get().DrawItemText(
        GetOwner()->GetOwner(),
        *dc,
        text,
        rectText,
        GetEffectiveAlignment(),
        flags,
        GetEllipsizeMode());
}

#if wxUSE_DATAVIEW_A11Y
wxString wxDataViewCustomRendererBase::GetAccessibleDescription() const
{
    wxVariant value;
    GetValue(value);

    if ( value.IsType(wxS("bool")) )
    {
        /* TRANSLATORS: Name of Boolean true value */
        return value.GetBool() ? _("true")
        /* TRANSLATORS: Name of Boolean false value */
                               : _("false");
    }

    // wxVariant::MakeString() doesn't know how to stringify this one either,
    // and it's an extremely common choice of variant type for a custom
    // renderer that draws an icon next to some text (it's the same type
    // wxDataViewIconTextRenderer itself uses).
    if ( value.IsType(wxS("wxDataViewIconText")) )
    {
        wxDataViewIconText iconText;
        iconText << value;
        return iconText.GetText();
    }

    return value.MakeString();
}
#endif // wxUSE_DATAVIEW_A11Y

void wxDataViewCustomRendererBase::SetEnabled(bool enabled)
{
    // The native base renderer needs to know about the enabled state as well
    // but in the generic case the base class method is pure, so we can't just
    // call it unconditionally.
#ifndef wxHAS_GENERIC_DATAVIEWCTRL
    wxDataViewRenderer::SetEnabled(enabled);
#endif // !wxHAS_GENERIC_DATAVIEWCTRL

    m_enabled = enabled;
}

//-----------------------------------------------------------------------------
// wxDataViewEditorCtrlEvtHandler
//-----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxDataViewEditorCtrlEvtHandler, wxEvtHandler)
    EVT_CHAR           (wxDataViewEditorCtrlEvtHandler::OnChar)
    EVT_KILL_FOCUS     (wxDataViewEditorCtrlEvtHandler::OnKillFocus)
    EVT_IDLE           (wxDataViewEditorCtrlEvtHandler::OnIdle)
    EVT_TEXT_ENTER     (-1, wxDataViewEditorCtrlEvtHandler::OnTextEnter)
wxEND_EVENT_TABLE()

void wxDataViewEditorCtrlEvtHandler::OnIdle( wxIdleEvent &event )
{
    if (m_focusOnIdle)
    {
        m_focusOnIdle = false;

        // Ignore focused items within the compound editor control
        wxWindow* win = wxWindow::FindFocus();
        if ( !IsEditorSubControl(win) )
        {
            m_editorCtrl->SetFocus();
        }
    }

    event.Skip();
}

void wxDataViewEditorCtrlEvtHandler::OnTextEnter( wxCommandEvent &WXUNUSED(event) )
{
    m_finished = true;
    m_owner->FinishEditing();
}

void wxDataViewEditorCtrlEvtHandler::OnChar( wxKeyEvent &event )
{
    switch ( event.m_keyCode )
    {
        case WXK_ESCAPE:
            m_finished = true;
            m_owner->CancelEditing();
            break;

        case WXK_RETURN:
            if ( !event.HasAnyModifiers() )
            {
                m_finished = true;
                m_owner->FinishEditing();
                break;
            }
            wxFALLTHROUGH; // Ctrl/Alt/Shift-Enter is not handled specially

        default:
            event.Skip();
    }
}

void wxDataViewEditorCtrlEvtHandler::OnKillFocus( wxFocusEvent &event )
{
    // Ignore focus changes within the compound editor control
    wxWindow* win = event.GetWindow();
    if ( IsEditorSubControl(win) )
    {
        event.Skip();
        return;
    }

    if (!m_finished)
    {
        m_finished = true;
        m_owner->FinishEditing();
    }

    event.Skip();
}

bool wxDataViewEditorCtrlEvtHandler::IsEditorSubControl(wxWindow* win) const
{
    // Checks whether the given window belongs to the editor control
    // (is either the editor itself or a child of the compound editor).
    while ( win )
    {
        if ( win == m_editorCtrl )
        {
            return true;
        }

        win = win->GetParent();
    }

    return false;
}

// ---------------------------------------------------------
// wxDataViewColumnBase
// ---------------------------------------------------------

wxDataViewColumnBase::~wxDataViewColumnBase()
{
    delete m_renderer;
}

// ---------------------------------------------------------
// wxDataViewCtrlBase
// ---------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxDataViewCtrlBase, wxControl);

wxDataViewCtrlBase::wxDataViewCtrlBase()
{
    m_model = nullptr;
    m_expander_column = nullptr;
    m_indent = 8;
}

wxDataViewCtrlBase::~wxDataViewCtrlBase()
{
    if (m_model)
    {
        m_model->DecRef();
        m_model = nullptr;
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

const wxDataViewModel* wxDataViewCtrlBase::GetModel() const
{
    return m_model;
}

void wxDataViewCtrlBase::Expand(const wxDataViewItem& item)
{
    wxDataViewModel* const model = m_model;
    if ( !model )
        return;

    model->IncRef();
    const wxScopeGuard releaseModel = wxMakeGuard([model]()
    {
        model->DecRef();
    });
    wxUnusedVar(releaseModel);
    const wxWeakRef<wxWindow> weakThis(this);

    ExpandAncestors(item);
    if ( weakThis.get() != this || m_model != model )
        return;

    DoExpand(item, false);
}

void wxDataViewCtrlBase::ExpandChildren(const wxDataViewItem& item)
{
    wxDataViewModel* const model = m_model;
    if ( !model )
        return;

    model->IncRef();
    const wxScopeGuard releaseModel = wxMakeGuard([model]()
    {
        model->DecRef();
    });
    wxUnusedVar(releaseModel);
    const wxWeakRef<wxWindow> weakThis(this);

    ExpandAncestors(item);
    if ( weakThis.get() != this || m_model != model )
        return;

    DoExpand(item, true);
}

void wxDataViewCtrlBase::ExpandAncestors( const wxDataViewItem & item )
{
    wxDataViewModel* const model = m_model;
    if ( !model )
        return;

    if (!item.IsOk()) return;

    model->IncRef();
    const wxScopeGuard releaseModel = wxMakeGuard([model]()
    {
        model->DecRef();
    });
    wxUnusedVar(releaseModel);
    const wxWeakRef<wxWindow> weakThis(this);

    wxVector<wxDataViewItem> parentChain;

    // at first we get all the parents of the selected item
    wxDataViewItem parent = model->GetParent(item);
    if ( weakThis.get() != this || m_model != model )
        return;

    while (parent.IsOk())
    {
        if ( std::find(parentChain.begin(), parentChain.end(), parent) !=
                parentChain.end() )
        {
            return;
        }

        parentChain.push_back(parent);
        parent = model->GetParent(parent);
        if ( weakThis.get() != this || m_model != model )
            return;
    }

    // then we expand the parents, starting at the root
    while (!parentChain.empty())
    {
        DoExpand(parentChain.back(), false);
        if ( weakThis.get() != this || m_model != model )
            return;

        parentChain.pop_back();
    }
}

wxDataViewItem wxDataViewCtrlBase::GetCurrentItem() const
{
    return HasFlag(wxDV_MULTIPLE) ? DoGetCurrentItem()
                                  : GetSelection();
}

void wxDataViewCtrlBase::SetCurrentItem(const wxDataViewItem& item)
{
    wxCHECK_RET( item.IsOk(), "Can't make current an invalid item." );

    if ( HasFlag(wxDV_MULTIPLE) )
        DoSetCurrentItem(item);
    else
        Select(item);
}

wxDataViewItem wxDataViewCtrlBase::GetSelection() const
{
    if ( GetSelectedItemsCount() != 1 )
        return wxDataViewItem();

    wxDataViewItemArray selections;
    GetSelections(selections);
    return selections[0];
}

namespace
{

// Helper to account for inconsistent signature of wxDataViewProgressRenderer
// ctor: it takes an extra "label" argument as first parameter, unlike all the
// other renderers.
template <typename Renderer>
struct RendererFactory
{
    static Renderer*
    New(wxDataViewCellMode mode, int align)
    {
        return new Renderer(Renderer::GetDefaultType(), mode, align);
    }
};

template <>
struct RendererFactory<wxDataViewProgressRenderer>
{
    static wxDataViewProgressRenderer*
    New(wxDataViewCellMode mode, int align)
    {
        return new wxDataViewProgressRenderer(
                        wxString(),
                        wxDataViewProgressRenderer::GetDefaultType(),
                        mode,
                        align
                    );
    }
};

template <typename Renderer, typename LabelType>
wxDataViewColumn*
CreateColumnWithRenderer(const LabelType& label,
                         unsigned model_column,
                         wxDataViewCellMode mode,
                         int width,
                         wxAlignment align,
                         int flags)
{
    // For compatibility reason, handle wxALIGN_NOT as wxDVR_DEFAULT_ALIGNMENT
    // when creating the renderer here because a lot of existing code,
    // including our own dataview sample, uses wxALIGN_NOT just because it's
    // the default value of the alignment argument in AppendXXXColumn()
    // methods, but this doesn't mean that it actually wants to top-align the
    // column text.
    //
    // This does make it impossible to create top-aligned text using these
    // functions, but it can always be done by creating the renderer with the
    // desired alignment explicitly and should be so rarely needed in practice
    // (without speaking that vertical alignment is completely unsupported in
    // native OS X version), that it's preferable to do the right thing by
    // default here rather than account for it.
    return new wxDataViewColumn(
                    label,
                    RendererFactory<Renderer>::New(
                        mode,
                        align & wxALIGN_BOTTOM
                            ? align
                            : align | wxALIGN_CENTRE_VERTICAL
                    ),
                    model_column,
                    width,
                    align,
                    flags
                );
}

// Common implementation of all {Append,Prepend}XXXColumn() below.
template <typename Renderer, typename LabelType>
wxDataViewColumn*
AppendColumnWithRenderer(wxDataViewCtrlBase* dvc,
                         const LabelType& label,
                         unsigned model_column,
                         wxDataViewCellMode mode,
                         int width,
                         wxAlignment align,
                         int flags)
{
    wxDataViewColumn* const
        col = CreateColumnWithRenderer<Renderer>(
                label, model_column, mode, width, align, flags
            );

    dvc->AppendColumn(col);
    return col;
}

template <typename Renderer, typename LabelType>
wxDataViewColumn*
PrependColumnWithRenderer(wxDataViewCtrlBase* dvc,
                          const LabelType& label,
                          unsigned model_column,
                          wxDataViewCellMode mode,
                          int width,
                          wxAlignment align,
                          int flags)
{
    wxDataViewColumn* const
        col = CreateColumnWithRenderer<Renderer>(
                label, model_column, mode, width, align, flags
            );

    dvc->PrependColumn(col);
    return col;
}

} // anonymous namespace

wxDataViewColumn *
wxDataViewCtrlBase::AppendTextColumn( const wxString &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumnWithRenderer<wxDataViewTextRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::AppendIconTextColumn( const wxString &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumnWithRenderer<wxDataViewIconTextRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::AppendToggleColumn( const wxString &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumnWithRenderer<wxDataViewToggleRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::AppendProgressColumn( const wxString &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumnWithRenderer<wxDataViewProgressRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::AppendDateColumn( const wxString &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumnWithRenderer<wxDataViewDateRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::AppendBitmapColumn( const wxString &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumnWithRenderer<wxDataViewBitmapRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::AppendTextColumn( const wxBitmap &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumnWithRenderer<wxDataViewTextRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::AppendIconTextColumn( const wxBitmap &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumnWithRenderer<wxDataViewIconTextRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::AppendToggleColumn( const wxBitmap &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumnWithRenderer<wxDataViewToggleRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::AppendProgressColumn( const wxBitmap &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumnWithRenderer<wxDataViewProgressRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::AppendDateColumn( const wxBitmap &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumnWithRenderer<wxDataViewDateRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::AppendBitmapColumn( const wxBitmap &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return AppendColumnWithRenderer<wxDataViewBitmapRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::PrependTextColumn( const wxString &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return PrependColumnWithRenderer<wxDataViewTextRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::PrependIconTextColumn( const wxString &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return PrependColumnWithRenderer<wxDataViewIconTextRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::PrependToggleColumn( const wxString &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return PrependColumnWithRenderer<wxDataViewToggleRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::PrependProgressColumn( const wxString &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return PrependColumnWithRenderer<wxDataViewProgressRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::PrependDateColumn( const wxString &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return PrependColumnWithRenderer<wxDataViewDateRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::PrependBitmapColumn( const wxString &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return PrependColumnWithRenderer<wxDataViewBitmapRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::PrependTextColumn( const wxBitmap &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return PrependColumnWithRenderer<wxDataViewTextRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::PrependIconTextColumn( const wxBitmap &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return PrependColumnWithRenderer<wxDataViewIconTextRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::PrependToggleColumn( const wxBitmap &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return PrependColumnWithRenderer<wxDataViewToggleRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::PrependProgressColumn( const wxBitmap &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return PrependColumnWithRenderer<wxDataViewProgressRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::PrependDateColumn( const wxBitmap &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return PrependColumnWithRenderer<wxDataViewDateRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

wxDataViewColumn *
wxDataViewCtrlBase::PrependBitmapColumn( const wxBitmap &label, unsigned int model_column,
                            wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    return PrependColumnWithRenderer<wxDataViewBitmapRenderer>(
                this, label, model_column, mode, width, align, flags
            );
}

bool
wxDataViewCtrlBase::AppendColumn( wxDataViewColumn *col )
{
    col->SetOwner( (wxDataViewCtrl*) this );
    return true;
}

bool
wxDataViewCtrlBase::PrependColumn( wxDataViewColumn *col )
{
    col->SetOwner( (wxDataViewCtrl*) this );
    return true;
}

bool
wxDataViewCtrlBase::InsertColumn( unsigned int WXUNUSED(pos), wxDataViewColumn *col )
{
    col->SetOwner( (wxDataViewCtrl*) this );
    return true;
}

void wxDataViewCtrlBase::StartEditor(const wxDataViewItem& item, unsigned int column)
{
    EditItem(item, GetColumn(column));
}

#if wxUSE_DRAG_AND_DROP

/* static */
wxDataObjectComposite*
wxDataViewCtrlBase::CreateDataObject(const wxVector<wxDataFormat>& formats)
{
    if (formats.empty())
    {
         return nullptr;
    }

    wxDataObjectComposite *dataObject(new wxDataObjectComposite);
    for ( const auto& fmt : formats )
    {
        switch ( fmt.GetType() )
        {
            case wxDF_TEXT:
            case wxDF_OEMTEXT:
            case wxDF_UNICODETEXT:
                dataObject->Add(new wxTextDataObject);
                break;

            case wxDF_BITMAP:
            case wxDF_PNG:
                dataObject->Add(new wxBitmapDataObject);
                break;

            case wxDF_FILENAME:
                dataObject->Add(new wxFileDataObject);
                break;

            case wxDF_HTML:
                dataObject->Add(new wxHTMLDataObject);
                break;

            case wxDF_METAFILE:
            case wxDF_SYLK:
            case wxDF_DIF:
            case wxDF_TIFF:
            case wxDF_DIB:
            case wxDF_PALETTE:
            case wxDF_PENDATA:
            case wxDF_RIFF:
            case wxDF_WAVE:
            case wxDF_ENHMETAFILE:
            case wxDF_LOCALE:
            case wxDF_PRIVATE:
            default: // any other custom format
                dataObject->Add(new wxCustomDataObject(fmt));
                break;

            case wxDF_INVALID:
            case wxDF_MAX:
                break;
        }
    }

    return dataObject;
}

#endif // wxUSE_DRAG_AND_DROP

// ---------------------------------------------------------
// wxDataViewEvent
// ---------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxDataViewEvent,wxNotifyEvent);

wxDEFINE_EVENT( wxEVT_DATAVIEW_SELECTION_CHANGED, wxDataViewEvent );

wxDEFINE_EVENT( wxEVT_DATAVIEW_ITEM_ACTIVATED, wxDataViewEvent );
wxDEFINE_EVENT( wxEVT_DATAVIEW_ITEM_COLLAPSING, wxDataViewEvent );
wxDEFINE_EVENT( wxEVT_DATAVIEW_ITEM_COLLAPSED, wxDataViewEvent );
wxDEFINE_EVENT( wxEVT_DATAVIEW_ITEM_EXPANDING, wxDataViewEvent );
wxDEFINE_EVENT( wxEVT_DATAVIEW_ITEM_EXPANDED, wxDataViewEvent );
wxDEFINE_EVENT( wxEVT_DATAVIEW_ITEM_EDITING_STARTED, wxDataViewEvent );
wxDEFINE_EVENT( wxEVT_DATAVIEW_ITEM_START_EDITING, wxDataViewEvent );
wxDEFINE_EVENT( wxEVT_DATAVIEW_ITEM_EDITING_DONE, wxDataViewEvent );
wxDEFINE_EVENT( wxEVT_DATAVIEW_ITEM_VALUE_CHANGED, wxDataViewEvent );

wxDEFINE_EVENT( wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, wxDataViewEvent );

wxDEFINE_EVENT( wxEVT_DATAVIEW_COLUMN_HEADER_CLICK, wxDataViewEvent );
wxDEFINE_EVENT( wxEVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, wxDataViewEvent );
wxDEFINE_EVENT( wxEVT_DATAVIEW_COLUMN_SORTED, wxDataViewEvent );
wxDEFINE_EVENT( wxEVT_DATAVIEW_COLUMN_REORDERED, wxDataViewEvent );

wxDEFINE_EVENT( wxEVT_DATAVIEW_CACHE_HINT, wxDataViewEvent );

wxDEFINE_EVENT( wxEVT_DATAVIEW_ITEM_BEGIN_DRAG, wxDataViewEvent );
wxDEFINE_EVENT( wxEVT_DATAVIEW_ITEM_DROP_POSSIBLE, wxDataViewEvent );
wxDEFINE_EVENT( wxEVT_DATAVIEW_ITEM_DROP, wxDataViewEvent );

// Common part of non-copy ctors.
void wxDataViewEvent::Init(wxDataViewCtrlBase* dvc,
                           wxDataViewColumn* column,
                           const wxDataViewItem& item)
{
    m_item = item;
    m_col = column ? column->GetModelColumn() : -1;
    m_model = dvc ? dvc->GetModel() : nullptr;
    m_column = column;
    m_pos = wxDefaultPosition;
    m_cacheFrom = 0;
    m_cacheTo = 0;
    m_editCancelled = false;
#if wxUSE_DRAG_AND_DROP
    m_dataObject = nullptr;
    m_dataBuffer = nullptr;
    m_dataSize = 0;
    m_dragFlags = 0;
    m_dropEffect = wxDragNone;
    m_proposedDropIndex = -1;
#endif // wxUSE_DRAG_AND_DROP

    SetEventObject(dvc);
}

#if wxUSE_DRAG_AND_DROP

void wxDataViewEvent::InitData(wxDataObjectComposite* obj, wxDataFormat format)
{
    SetDataFormat(format);

    SetDataObject(obj->GetObject(format));

    const size_t size = obj->GetDataSize(format);
    SetDataSize(size);

    if ( size )
    {
        obj->GetDataHere(format, m_dataBuf.GetWriteBuf(size));
        m_dataBuf.UngetWriteBuf(size);

        SetDataBuffer(m_dataBuf.GetData());
    }
}

#endif // wxUSE_DRAG_AND_DROP

#if wxUSE_SPINCTRL

// -------------------------------------
// wxDataViewSpinRenderer
// -------------------------------------

wxDataViewSpinRenderer::wxDataViewSpinRenderer( int min, int max, wxDataViewCellMode mode, int alignment ) :
   wxDataViewCustomRenderer(wxT("long"), mode, alignment )
{
    m_min = min;
    m_max = max;
}

wxWindow* wxDataViewSpinRenderer::CreateEditorCtrl( wxWindow *parent, wxRect labelRect, const wxVariant &value )
{
    long l = value;
    wxString str;
    str.Printf( wxT("%d"), (int) l );
    wxSpinCtrl *sc = new wxSpinCtrl( parent, wxID_ANY, str,
               labelRect.GetTopLeft(), labelRect.GetSize(), wxSP_ARROW_KEYS|wxTE_PROCESS_ENTER, m_min, m_max, l );
#ifdef __WXMAC__
    const wxSize size = sc->GetSize();
    wxPoint pt = sc->GetPosition();
    sc->SetSize( pt.x - 4, pt.y - 4, size.x, size.y );
#endif

    return sc;
}

bool wxDataViewSpinRenderer::GetValueFromEditorCtrl( wxWindow* editor, wxVariant &value )
{
    wxSpinCtrl *sc = (wxSpinCtrl*) editor;
    long l = sc->GetValue();
    value = l;
    return true;
}

bool wxDataViewSpinRenderer::Render( wxRect rect, wxDC *dc, int state )
{
    wxString str;
    str.Printf(wxT("%d"), (int) m_data );
    RenderText( str, 0, rect, dc, state );
    return true;
}

wxSize wxDataViewSpinRenderer::GetSize() const
{
    wxSize sz = GetTextExtent(wxString::Format("%d", (int)m_data));

    // Allow some space for the spin buttons, which is approximately the size
    // of a scrollbar (and getting pixel-exact value would be complicated).
    // Also add some whitespace between the text and the button:
    sz.x += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X, m_editorCtrl);
    sz.x += GetTextExtent("M").x;

    return sz;
}

bool wxDataViewSpinRenderer::SetValue( const wxVariant &value )
{
    m_data = value.GetLong();
    return true;
}

bool wxDataViewSpinRenderer::GetValue( wxVariant &value ) const
{
    value = m_data;
    return true;
}

#if wxUSE_ACCESSIBILITY
wxString wxDataViewSpinRenderer::GetAccessibleDescription() const
{
    return wxString::Format(wxS("%li"), m_data);
}
#endif // wxUSE_ACCESSIBILITY

#endif // wxUSE_SPINCTRL

// -------------------------------------
// wxDataViewChoiceRenderer
// -------------------------------------

#if defined(wxHAS_GENERIC_DATAVIEWCTRL)

wxDataViewChoiceRenderer::wxDataViewChoiceRenderer( const wxArrayString& choices, wxDataViewCellMode mode, int alignment ) :
   wxDataViewCustomRenderer(wxT("string"), mode, alignment )
{
    m_choices = choices;
}

wxWindow* wxDataViewChoiceRenderer::CreateEditorCtrl( wxWindow *parent, wxRect labelRect, const wxVariant &value )
{
    wxChoice* c = new wxChoice
                      (
                          parent,
                          wxID_ANY,
                          labelRect.GetTopLeft(),
                          wxSize(labelRect.GetWidth(), -1),
                          m_choices
                      );
    c->Move(labelRect.GetRight() - c->GetRect().width, wxDefaultCoord);
    c->SetStringSelection( value.GetString() );
    return c;
}

bool wxDataViewChoiceRenderer::GetValueFromEditorCtrl( wxWindow* editor, wxVariant &value )
{
    wxChoice *c = (wxChoice*) editor;
    wxString s = c->GetStringSelection();
    value = s;
    return true;
}

bool wxDataViewChoiceRenderer::Render( wxRect rect, wxDC *dc, int state )
{
    RenderText( m_data, 0, rect, dc, state );
    return true;
}

wxSize wxDataViewChoiceRenderer::GetSize() const
{
    wxSize sz;

    for ( wxArrayString::const_iterator i = m_choices.begin(); i != m_choices.end(); ++i )
        sz.IncTo(GetTextExtent(*i));

    // Allow some space for the right-side button, which is approximately the
    // size of a scrollbar (and getting pixel-exact value would be complicated).
    // Also add some whitespace between the text and the button:
    sz.x += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X, m_editorCtrl);
    sz.x += GetTextExtent("M").x;

    return sz;
}

bool wxDataViewChoiceRenderer::SetValue( const wxVariant &value )
{
    m_data = value.GetString();
    return true;
}

bool wxDataViewChoiceRenderer::GetValue( wxVariant &value ) const
{
    value = m_data;
    return true;
}

#if wxUSE_ACCESSIBILITY
wxString wxDataViewChoiceRenderer::GetAccessibleDescription() const
{
    return m_data;
}
#endif // wxUSE_ACCESSIBILITY

// ----------------------------------------------------------------------------
// wxDataViewChoiceByIndexRenderer
// ----------------------------------------------------------------------------

wxDataViewChoiceByIndexRenderer::wxDataViewChoiceByIndexRenderer( const wxArrayString &choices,
                                  wxDataViewCellMode mode, int alignment ) :
      wxDataViewChoiceRenderer( choices, mode, alignment )
{
    m_variantType = wxS("long");
}

wxWindow* wxDataViewChoiceByIndexRenderer::CreateEditorCtrl( wxWindow *parent, wxRect labelRect, const wxVariant &value )
{
    wxVariant string_value = value.GetLong() != wxNOT_FOUND ? GetChoice( value.GetLong() )
                                                            : wxString();

    return wxDataViewChoiceRenderer::CreateEditorCtrl( parent, labelRect, string_value );
}

bool wxDataViewChoiceByIndexRenderer::GetValueFromEditorCtrl( wxWindow* editor, wxVariant &value )
{
    wxVariant string_value;
    if (!wxDataViewChoiceRenderer::GetValueFromEditorCtrl( editor, string_value ))
        return false;

    value = (long) GetChoices().Index( string_value.GetString() );
    return true;
}

bool wxDataViewChoiceByIndexRenderer::SetValue( const wxVariant &value )
{
    wxVariant string_value = value.GetLong() != wxNOT_FOUND ? GetChoice( value.GetLong() )
                                                            : wxString();
    return wxDataViewChoiceRenderer::SetValue( string_value );
}

bool wxDataViewChoiceByIndexRenderer::GetValue( wxVariant &value ) const
{
    wxVariant string_value;
    if (!wxDataViewChoiceRenderer::GetValue( string_value ))
        return false;

    value = (long) GetChoices().Index( string_value.GetString() );
    return true;
}

#if wxUSE_ACCESSIBILITY
wxString wxDataViewChoiceByIndexRenderer::GetAccessibleDescription() const
{
    wxVariant strVal;
    if ( wxDataViewChoiceRenderer::GetValue(strVal) )
        return strVal;

    return wxString::Format(wxS("%li"), (long)GetChoices().Index(strVal.GetString()));
}
#endif // wxUSE_ACCESSIBILITY

#endif // wxHAS_GENERIC_DATAVIEWCTRL

// ---------------------------------------------------------
// wxDataViewDateRenderer
// ---------------------------------------------------------

#if (defined(wxHAS_GENERIC_DATAVIEWCTRL) || defined(__WXGTK__)) && wxUSE_DATEPICKCTRL

wxDataViewDateRenderer::wxDataViewDateRenderer(const wxString& varianttype,
                                              wxDataViewCellMode mode, int align)
    : wxDataViewCustomRenderer(varianttype, mode, align)
{
}

wxWindow *
wxDataViewDateRenderer::CreateEditorCtrl(wxWindow *parent, wxRect labelRect, const wxVariant& value)
{
    return new wxDatePickerCtrl
               (
                   parent,
                   wxID_ANY,
                   value.GetDateTime(),
                   labelRect.GetTopLeft(),
                   labelRect.GetSize()
               );
}

bool wxDataViewDateRenderer::GetValueFromEditorCtrl(wxWindow *editor, wxVariant& value)
{
    wxDatePickerCtrl *ctrl = static_cast<wxDatePickerCtrl*>(editor);
    value = ctrl->GetValue();
    return true;
}

bool wxDataViewDateRenderer::SetValue(const wxVariant& value)
{
    m_date = value.GetDateTime();
    return true;
}

bool wxDataViewDateRenderer::GetValue(wxVariant& value) const
{
    value = m_date;
    return true;
}

wxString wxDataViewDateRenderer::FormatDate() const
{
    return m_date.Format(wxGetUIDateFormat());
}

#if wxUSE_ACCESSIBILITY
wxString wxDataViewDateRenderer::GetAccessibleDescription() const
{
    return FormatDate();
}
#endif // wxUSE_ACCESSIBILITY

bool wxDataViewDateRenderer::Render(wxRect cell, wxDC* dc, int state)
{
    wxString tmp = FormatDate();
    RenderText( tmp, 0, cell, dc, state );
    return true;
}

wxSize wxDataViewDateRenderer::GetSize() const
{
    return GetTextExtent(FormatDate());
}

#endif // (defined(wxHAS_GENERIC_DATAVIEWCTRL) || defined(__WXGTK__)) && wxUSE_DATEPICKCTRL

// ----------------------------------------------------------------------------
// wxDataViewCheckIconTextRenderer implementation
// ----------------------------------------------------------------------------

#if defined(wxHAS_GENERIC_DATAVIEWCTRL) || !defined(__WXOSX__)

IMPLEMENT_VARIANT_OBJECT_EXPORTED(wxDataViewCheckIconText, WXDLLIMPEXP_ADV)

wxIMPLEMENT_CLASS(wxDataViewCheckIconText, wxDataViewIconText);

wxIMPLEMENT_CLASS(wxDataViewCheckIconTextRenderer, wxDataViewRenderer);

wxDataViewCheckIconTextRenderer::wxDataViewCheckIconTextRenderer
                                 (
                                      wxDataViewCellMode mode,
                                      int align
                                 )
    : wxDataViewCustomRenderer(GetDefaultType(), mode, align)
{
    m_allow3rdStateForUser = false;
}

void wxDataViewCheckIconTextRenderer::Allow3rdStateForUser(bool allow)
{
    m_allow3rdStateForUser = allow;
}

bool wxDataViewCheckIconTextRenderer::SetValue(const wxVariant& value)
{
    m_value << value;
    return true;
}

bool wxDataViewCheckIconTextRenderer::GetValue(wxVariant& value) const
{
    value << m_value;
    return true;
}

#if wxUSE_ACCESSIBILITY
wxString wxDataViewCheckIconTextRenderer::GetAccessibleDescription() const
{
    wxString text = m_value.GetText();
    if ( !text.empty() )
    {
        text += wxS(" ");
    }

    switch ( m_value.GetCheckedState() )
    {
        case wxCHK_CHECKED:
            /* TRANSLATORS: Checkbox state name */
            text += _("checked");
            break;
        case wxCHK_UNCHECKED:
            /* TRANSLATORS: Checkbox state name */
            text += _("unchecked");
            break;
        case wxCHK_UNDETERMINED:
            /* TRANSLATORS: Checkbox state name */
            text += _("undetermined");
            break;
    }

    return text;
}
#endif // wxUSE_ACCESSIBILITY

wxSize wxDataViewCheckIconTextRenderer::GetSize() const
{
    wxSize size = GetCheckSize();
    size.x += MARGIN_CHECK_ICON;

    const wxBitmapBundle& bb = m_value.GetBitmapBundle();
    if ( bb.IsOk() )
    {
        const wxSize sizeIcon = bb.GetPreferredLogicalSizeFor(GetView());
        if ( sizeIcon.y > size.y )
            size.y = sizeIcon.y;

        size.x += sizeIcon.x + MARGIN_ICON_TEXT;
    }

    wxString text = m_value.GetText();
    if ( text.empty() )
        text = "Dummy";

    const wxSize sizeText = GetTextExtent(text);
    if ( sizeText.y > size.y )
        size.y = sizeText.y;

    size.x += sizeText.x;

    return size;
}

bool wxDataViewCheckIconTextRenderer::Render(wxRect cell, wxDC* dc, int state)
{
    /*
    Draw the text first because if the item has a background colour set
    then with wxGTK the entire cell is painted over during RenderText()
    when attributes are applied.
    */

    const wxSize sizeCheck = GetCheckSize();

    int xoffset = sizeCheck.x + MARGIN_CHECK_ICON;

    wxRect rectIcon;
    const wxBitmapBundle& bb = m_value.GetBitmapBundle();
    const bool drawIcon = bb.IsOk();
    if ( drawIcon )
    {
        const wxSize sizeIcon = bb.GetPreferredLogicalSizeFor(GetView());
        rectIcon = wxRect(cell.GetPosition(), sizeIcon);
        rectIcon.x += xoffset;
        rectIcon = rectIcon.CentreIn(cell, wxVERTICAL);

        xoffset += sizeIcon.x + MARGIN_ICON_TEXT;
    }

    RenderText(m_value.GetText(), xoffset, cell, dc, state);

    // Then draw the checkbox.
    int renderFlags = 0;
    switch ( m_value.GetCheckedState() )
    {
        case wxCHK_UNCHECKED:
            break;

        case wxCHK_CHECKED:
            renderFlags |= wxCONTROL_CHECKED;
            break;

        case wxCHK_UNDETERMINED:
            renderFlags |= wxCONTROL_UNDETERMINED;
            break;
    }

    if ( state & wxDATAVIEW_CELL_PRELIT )
        renderFlags |= wxCONTROL_CURRENT;

    wxRect rectCheck(cell.GetPosition(), sizeCheck);
    rectCheck = rectCheck.CentreIn(cell, wxVERTICAL);

    wxRendererNative::Get().DrawCheckBox
                            (
                                GetView(), *dc, rectCheck, renderFlags
                            );

    // Finally draw the icon, if any.
    if ( drawIcon )
        dc->DrawIcon(bb.GetIconFor(GetView()), rectIcon.GetPosition());

    return true;
}

bool
wxDataViewCheckIconTextRenderer::ActivateCell(const wxRect& WXUNUSED(cell),
                                              wxDataViewModel *model,
                                              const wxDataViewItem & item,
                                              unsigned int col,
                                              const wxMouseEvent *mouseEvent)
{
    if ( mouseEvent )
    {
        if ( !wxRect(GetCheckSize()).Contains(mouseEvent->GetPosition()) )
            return false;
    }

    // If the 3rd state is user-settable then the cycle is
    // unchecked->checked->undetermined.
    wxCheckBoxState checkedState = m_value.GetCheckedState();
    switch ( checkedState )
    {
        case wxCHK_CHECKED:
            checkedState = m_allow3rdStateForUser ? wxCHK_UNDETERMINED
                                                  : wxCHK_UNCHECKED;
            break;

        case wxCHK_UNDETERMINED:
            // Whether 3rd state is user-settable or not, the next state is
            // unchecked.
            checkedState = wxCHK_UNCHECKED;
            break;

        case wxCHK_UNCHECKED:
            checkedState = wxCHK_CHECKED;
            break;
    }

    m_value.SetCheckedState(checkedState);

    wxVariant value;
    value << m_value;

    model->ChangeValue(value, item, col);
    return true;
}

wxSize wxDataViewCheckIconTextRenderer::GetCheckSize() const
{
    return wxRendererNative::Get().GetCheckBoxSize(GetView());
}

#endif // ! native __WXOSX__

//-----------------------------------------------------------------------------
// wxDataViewListStore
//-----------------------------------------------------------------------------

wxDataViewListStore::wxDataViewListStore()
{
}

wxDataViewListStore::~wxDataViewListStore()
{
    wxVector<wxDataViewListStoreLine*>::iterator it;
    for (it = m_data.begin(); it != m_data.end(); ++it)
    {
        wxDataViewListStoreLine* line = *it;
        delete line;
    }
}

void wxDataViewListStore::PrependColumn( const wxString &varianttype )
{
    InsertColumn(0, varianttype);
}

void wxDataViewListStore::InsertColumn( unsigned int pos, const wxString &varianttype )
{
    wxCHECK_RET( pos <= m_cols.size(), "invalid list store column position" );

    m_cols.Insert( varianttype, pos );
    for ( wxVector<wxDataViewListStoreLine*>::iterator it = m_data.begin();
          it != m_data.end();
          ++it )
    {
        (*it)->m_values.insert((*it)->m_values.begin() + pos, wxVariant());
    }
}

void wxDataViewListStore::AppendColumn( const wxString &varianttype )
{
    InsertColumn(m_cols.size(), varianttype);
}

void wxDataViewListStore::DeleteColumn( unsigned int pos )
{
    wxCHECK_RET( pos < m_cols.size(), "invalid list store column position" );

    m_cols.RemoveAt(pos);
    for ( wxVector<wxDataViewListStoreLine*>::iterator it = m_data.begin();
          it != m_data.end();
          ++it )
    {
        (*it)->m_values.erase((*it)->m_values.begin() + pos);
    }
}

unsigned int wxDataViewListStore::GetItemCount() const
{
    return m_data.size();
}

void wxDataViewListStore::AppendItem( const wxVector<wxVariant> &values, wxUIntPtr data )
{
    wxCHECK_RET( m_data.empty() || values.size() == m_data[0]->m_values.size(),
                 "wrong number of values" );

    wxDataViewListStoreLine *line = new wxDataViewListStoreLine( data );
    line->m_values = values;
    m_data.push_back( line );

    RowAppended();
}

void wxDataViewListStore::PrependItem( const wxVector<wxVariant> &values, wxUIntPtr data )
{
    wxCHECK_RET( m_data.empty() || values.size() == m_data[0]->m_values.size(),
                 "wrong number of values" );

    wxDataViewListStoreLine *line = new wxDataViewListStoreLine( data );
    line->m_values = values;
    m_data.insert( m_data.begin(), line );

    RowPrepended();
}

void wxDataViewListStore::InsertItem(  unsigned int row, const wxVector<wxVariant> &values,
                                       wxUIntPtr data )
{
    wxCHECK_RET( m_data.empty() || values.size() == m_data[0]->m_values.size(),
                 "wrong number of values" );

    wxDataViewListStoreLine *line = new wxDataViewListStoreLine( data );
    line->m_values = values;
    m_data.insert( m_data.begin()+row, line );

    RowInserted( row );
}

void wxDataViewListStore::DeleteItem( unsigned int row )
{
    wxVector<wxDataViewListStoreLine*>::iterator it = m_data.begin() + row;
    delete *it;
    m_data.erase( it );

    RowDeleted( row );
}

void wxDataViewListStore::DeleteAllItems()
{
    wxVector<wxDataViewListStoreLine*>::iterator it;
    for (it = m_data.begin(); it != m_data.end(); ++it)
    {
        wxDataViewListStoreLine* line = *it;
        delete line;
    }

    m_data.clear();

    Reset( 0 );
}

void wxDataViewListStore::ClearColumns()
{
    m_cols.clear();
    for ( wxVector<wxDataViewListStoreLine*>::iterator it = m_data.begin();
          it != m_data.end();
          ++it )
    {
        (*it)->m_values.clear();
    }
}

void wxDataViewListStore::SetItemData( const wxDataViewItem& item, wxUIntPtr data )
{
    wxDataViewListStoreLine* line = m_data[GetRow(item)];
    if (!line) return;

    line->SetData( data );
}

wxUIntPtr wxDataViewListStore::GetItemData( const wxDataViewItem& item ) const
{
    wxDataViewListStoreLine* line = m_data[GetRow(item)];
    if (!line) return 0;

    return line->GetData();
}

void wxDataViewListStore::GetValueByRow( wxVariant &value, unsigned int row, unsigned int col ) const
{
    wxDataViewListStoreLine *line = m_data[row];
    value = line->m_values[col];
}

bool wxDataViewListStore::SetValueByRow( const wxVariant &value, unsigned int row, unsigned int col )
{
    wxDataViewListStoreLine *line = m_data[row];
    line->m_values[col] = value;

    return true;
}

//-----------------------------------------------------------------------------
// wxDataViewListCtrl
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxDataViewListCtrl,wxDataViewCtrl);

wxDataViewListCtrl::wxDataViewListCtrl()
{
}

wxDataViewListCtrl::wxDataViewListCtrl( wxWindow *parent, wxWindowID id,
           const wxPoint& pos, const wxSize& size, long style,
           const wxValidator& validator )
{
    Create( parent, id, pos, size, style, validator );
}

wxDataViewListCtrl::~wxDataViewListCtrl()
{
}


bool wxDataViewListCtrl::Create( wxWindow *parent, wxWindowID id,
           const wxPoint& pos, const wxSize& size, long style,
           const wxValidator& validator )
{
    if ( !wxDataViewCtrl::Create( parent, id, pos, size, style, validator ) )
        return false;

    wxDataViewListStore *store = new wxDataViewListStore;
    AssociateModel( store );
    store->DecRef();

    return true;
}

bool wxDataViewListCtrl::AppendColumn( wxDataViewColumn *column, const wxString &varianttype )
{
#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    // Append position is defined after finishing the editor because its
    // editing-done callback is allowed to change the column topology.
    if ( !PrepareForColumnMutation() )
        return false;
#endif

    return InsertColumn(GetColumnCount(), column, varianttype);
}

bool wxDataViewListCtrl::PrependColumn( wxDataViewColumn *column, const wxString &varianttype )
{
    return InsertColumn(0, column, varianttype);
}

bool wxDataViewListCtrl::InsertColumn( unsigned int pos, wxDataViewColumn *column, const wxString &varianttype )
{
#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    if ( !PrepareForColumnMutation() )
        return false;
#endif

    const unsigned int count = GetColumnCount();
    wxCHECK_MSG( pos <= count, false, "invalid list control column position" );

    wxDataViewListStore* const store = GetStore();
    store->IncRef();
    wxObjectDataPtr<wxDataViewListStore> pinnedStore(store);

    // wxDataViewListStore uses positional model columns. Shift the existing
    // view-to-model mapping before publishing the new topology so that any
    // synchronous refresh observes a coherent schema.
    for ( unsigned int i = 0; i < count; ++i )
    {
        wxDataViewColumn* const existing = GetColumn(i);
        if ( existing->GetModelColumn() >= pos )
        {
            existing->WXSetModelColumn(
                existing->GetModelColumn() + 1);
        }
    }
    column->WXSetModelColumn(pos);

    pinnedStore->InsertColumn(pos, varianttype);
    const wxWeakRef<wxWindow> weakThis(this);
    if ( wxDataViewCtrl::InsertColumn(pos, column) )
        return true;

    // A synchronous header callback can destroy the view while an external
    // owner still keeps the store alive. Its schema must be rolled back
    // independently of the view mapping.
    pinnedStore->DeleteColumn(pos);
    if ( weakThis.get() == this )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            wxDataViewColumn* const existing = GetColumn(i);
            if ( existing->GetModelColumn() > pos )
            {
                existing->WXSetModelColumn(
                    existing->GetModelColumn() - 1);
            }
        }
    }

    return false;
}

bool wxDataViewListCtrl::PrependColumn( wxDataViewColumn *col )
{
    return PrependColumn( col, col->GetRenderer()->GetVariantType() );
}

bool wxDataViewListCtrl::InsertColumn( unsigned int pos, wxDataViewColumn *col )
{
    return InsertColumn( pos, col, col->GetRenderer()->GetVariantType() );
}

bool wxDataViewListCtrl::AppendColumn( wxDataViewColumn *col )
{
    return AppendColumn( col, col->GetRenderer()->GetVariantType() );
}

bool wxDataViewListCtrl::DeleteColumn( wxDataViewColumn *col )
{
#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    if ( !PrepareForColumnMutation() )
        return false;
#endif

    // PrepareForColumnMutation() runs public editing-done handlers. The
    // original pointer is only an identity until it has been found again in
    // the current control; dereferencing it before then would be a UAF if the
    // handler removed it.
    wxDataViewColumn* currentColumn = nullptr;
    for ( unsigned int n = 0; n < GetColumnCount(); ++n )
    {
        if ( GetColumn(n) == col )
        {
            currentColumn = GetColumn(n);
            break;
        }
    }
    if ( !currentColumn )
        return false;

    const unsigned int pos = currentColumn->GetModelColumn();
    const unsigned int count = GetColumnCount();
    if ( pos >= count )
        return false;

    wxDataViewListStore* const store = GetStore();
    store->IncRef();
    wxObjectDataPtr<wxDataViewListStore> pinnedStore(store);
    const wxString variantType = pinnedStore->m_cols[pos];
    wxVector<wxVariant> removedValues;
    removedValues.reserve(pinnedStore->m_data.size());
    for ( wxVector<wxDataViewListStoreLine*>::const_iterator
              it = pinnedStore->m_data.begin();
          it != pinnedStore->m_data.end();
          ++it )
    {
        removedValues.push_back((*it)->m_values[pos]);
    }

    pinnedStore->DeleteColumn(pos);
    for ( unsigned int i = 0; i < count; ++i )
    {
        wxDataViewColumn* const existing = GetColumn(i);
        if ( existing != currentColumn && existing->GetModelColumn() > pos )
        {
            existing->WXSetModelColumn(
                existing->GetModelColumn() - 1);
        }
    }

    const wxWeakRef<wxWindow> weakThis(this);
    if ( wxDataViewCtrl::DeleteColumn(currentColumn) )
        return true;

    // ResetColumnsOrder() can synchronously cancel an active header gesture.
    // Its cancellation handler is allowed to destroy this control. In that
    // case the view columns have already been consumed by destruction, while
    // the pinned store intentionally keeps the reduced schema alive. Restoring
    // the positional snapshot would both contradict the completed lifetime
    // transition and index rows that the callback may already have changed.
    if ( weakThis.get() != this )
        return true;

    pinnedStore->InsertColumn(pos, variantType);
    for ( size_t row = 0; row < removedValues.size(); ++row )
        pinnedStore->m_data[row]->m_values[pos] = removedValues[row];

    if ( weakThis.get() == this )
    {
        for ( unsigned int i = 0; i < count; ++i )
        {
            wxDataViewColumn* const existing = GetColumn(i);
            if ( existing != currentColumn && existing->GetModelColumn() >= pos )
            {
                existing->WXSetModelColumn(
                    existing->GetModelColumn() + 1);
            }
        }
    }

    return false;
}

bool wxDataViewListCtrl::ClearColumns()
{
#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    // Finish the editor before choosing the store to mutate. Its DONE handler
    // may legally associate another list store; capturing the old one first
    // would clear the new view but the old schema.
    if ( !PrepareForColumnMutation() )
        return false;
#endif

    wxDataViewListStore* const store = GetStore();
    store->IncRef();
    wxObjectDataPtr<wxDataViewListStore> pinnedStore(store);

    // Clear the backend first. If it rejects the operation, the list-store
    // schema and every row remain untouched and no rollback is necessary.
    if ( !wxDataViewCtrl::ClearColumns() )
        return false;

    pinnedStore->ClearColumns();
    return true;
}

wxDataViewColumn *wxDataViewListCtrl::AppendTextColumn( const wxString &label,
          wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    wxDataViewColumn *ret = new wxDataViewColumn( label,
        new wxDataViewTextRenderer( wxT("string"), mode ),
        GetColumnCount(), width, align, flags );

    if ( AppendColumn(ret, wxT("string")) )
        return ret;

#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    // The generic implementation has not taken ownership when it reports
    // failure. Some native ports do take (and destroy) the column on failure,
    // so this cleanup must remain generic-only.
    delete ret;
#endif
    return nullptr;
}

wxDataViewColumn *wxDataViewListCtrl::AppendToggleColumn( const wxString &label,
          wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    wxDataViewColumn *ret = new wxDataViewColumn( label,
        new wxDataViewToggleRenderer( wxT("bool"), mode ),
        GetColumnCount(), width, align, flags );

    if ( AppendColumn(ret, wxT("bool")) )
        return ret;

#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    delete ret;
#endif
    return nullptr;
}

wxDataViewColumn *wxDataViewListCtrl::AppendProgressColumn( const wxString &label,
          wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    wxDataViewColumn *ret = new wxDataViewColumn( label,
        new wxDataViewProgressRenderer( wxEmptyString, wxT("long"), mode ),
        GetColumnCount(), width, align, flags );

    if ( AppendColumn(ret, wxT("long")) )
        return ret;

#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    delete ret;
#endif
    return nullptr;
}

wxDataViewColumn *wxDataViewListCtrl::AppendIconTextColumn( const wxString &label,
          wxDataViewCellMode mode, int width, wxAlignment align, int flags )
{
    wxDataViewColumn *ret = new wxDataViewColumn( label,
        new wxDataViewIconTextRenderer( wxT("wxDataViewIconText"), mode ),
        GetColumnCount(), width, align, flags );

    if ( AppendColumn(ret, wxT("wxDataViewIconText")) )
        return ret;

#ifdef wxHAS_GENERIC_DATAVIEWCTRL
    delete ret;
#endif
    return nullptr;
}

//-----------------------------------------------------------------------------
// wxDataViewTreeStore
//-----------------------------------------------------------------------------

wxDataViewTreeStoreNode::wxDataViewTreeStoreNode(
        wxDataViewTreeStoreNode *parent,
        const wxString &text, const wxBitmapBundle &icon, wxClientData *data )
    : m_text(text)
    , m_icon(icon)
{
    m_parent = parent;
    m_data = data;
}

wxDataViewTreeStoreNode::~wxDataViewTreeStoreNode()
{
    delete m_data;
}

wxDataViewTreeStoreContainerNode::wxDataViewTreeStoreContainerNode(
        wxDataViewTreeStoreNode *parent, const wxString &text,
        const wxBitmapBundle &icon, const wxBitmapBundle &expanded, wxClientData *data )
    : wxDataViewTreeStoreNode( parent, text, icon, data )
    , m_iconExpanded(expanded)
{
    m_isExpanded = false;
}

wxDataViewTreeStoreContainerNode::~wxDataViewTreeStoreContainerNode()
{
    DestroyChildren();
}

wxDataViewTreeStoreNodes::iterator
wxDataViewTreeStoreContainerNode::FindChild(wxDataViewTreeStoreNode* node)
{
    wxDataViewTreeStoreNodes::iterator iter;
    for (iter = m_children.begin(); iter != m_children.end(); ++iter)
    {
        if ( *iter == node )
            break;
    }

    return iter;
}

void wxDataViewTreeStoreContainerNode::DestroyChildren()
{
    wxDataViewTreeStoreNodes::const_iterator iter;
    for (iter = m_children.begin(); iter != m_children.end(); ++iter)
    {
        delete *iter;
    }

    m_children.clear();
}

//-----------------------------------------------------------------------------

wxDataViewTreeStore::wxDataViewTreeStore()
{
    m_root = new wxDataViewTreeStoreContainerNode( nullptr, wxEmptyString );
}

wxDataViewTreeStore::~wxDataViewTreeStore()
{
    delete m_root;
}

wxDataViewItem wxDataViewTreeStore::AppendItem( const wxDataViewItem& parent,
        const wxString &text, const wxBitmapBundle &icon, wxClientData *data )
{
    wxDataViewTreeStoreContainerNode *parent_node = FindContainerNode( parent );
    if (!parent_node) return wxDataViewItem(nullptr);

    wxDataViewTreeStoreNode *node =
        new wxDataViewTreeStoreNode( parent_node, text, icon, data );
    parent_node->GetChildren().push_back( node );

    return node->GetItem();
}

wxDataViewItem wxDataViewTreeStore::PrependItem( const wxDataViewItem& parent,
        const wxString &text, const wxBitmapBundle &icon, wxClientData *data )
{
    wxDataViewTreeStoreContainerNode *parent_node = FindContainerNode( parent );
    if (!parent_node) return wxDataViewItem(nullptr);

    wxDataViewTreeStoreNode *node =
        new wxDataViewTreeStoreNode( parent_node, text, icon, data );
    wxDataViewTreeStoreNodes& children = parent_node->GetChildren();
    children.insert(children.begin(), node);

    return node->GetItem();
}

wxDataViewItem
wxDataViewTreeStore::InsertItem(const wxDataViewItem& parent,
                                const wxDataViewItem& previous,
                                const wxString& text,
                                const wxBitmapBundle& icon,
                                wxClientData *data)
{
    wxDataViewTreeStoreContainerNode *parent_node = FindContainerNode( parent );
    if (!parent_node) return wxDataViewItem(nullptr);

    wxDataViewTreeStoreNode *previous_node = FindNode( previous );
    wxDataViewTreeStoreNodes& children = parent_node->GetChildren();
    const wxDataViewTreeStoreNodes::iterator iter = parent_node->FindChild( previous_node );
    if (iter == children.end()) return wxDataViewItem(nullptr);

    wxDataViewTreeStoreNode *node =
        new wxDataViewTreeStoreNode( parent_node, text, icon, data );
    children.insert(iter, node);

    return node->GetItem();
}

wxDataViewItem wxDataViewTreeStore::PrependContainer( const wxDataViewItem& parent,
        const wxString &text, const wxBitmapBundle &icon, const wxBitmapBundle &expanded,
        wxClientData *data )
{
    wxDataViewTreeStoreContainerNode *parent_node = FindContainerNode( parent );
    if (!parent_node) return wxDataViewItem(nullptr);

    wxDataViewTreeStoreContainerNode *node =
        new wxDataViewTreeStoreContainerNode( parent_node, text, icon, expanded, data );
    wxDataViewTreeStoreNodes& children = parent_node->GetChildren();
    children.insert(children.begin(), node);

    return node->GetItem();
}

wxDataViewItem
wxDataViewTreeStore::AppendContainer(const wxDataViewItem& parent,
                                     const wxString &text,
                                     const wxBitmapBundle& icon,
                                     const wxBitmapBundle& expanded,
                                     wxClientData * data)
{
    wxDataViewTreeStoreContainerNode *parent_node = FindContainerNode( parent );
    if (!parent_node) return wxDataViewItem(nullptr);

    wxDataViewTreeStoreContainerNode *node =
        new wxDataViewTreeStoreContainerNode( parent_node, text, icon, expanded, data );
    parent_node->GetChildren().push_back( node );

    return node->GetItem();
}

wxDataViewItem
wxDataViewTreeStore::InsertContainer(const wxDataViewItem& parent,
                                     const wxDataViewItem& previous,
                                     const wxString& text,
                                     const wxBitmapBundle& icon,
                                     const wxBitmapBundle& expanded,
                                     wxClientData * data)
{
    wxDataViewTreeStoreContainerNode *parent_node = FindContainerNode( parent );
    if (!parent_node) return wxDataViewItem(nullptr);

    wxDataViewTreeStoreNode *previous_node = FindNode( previous );
    wxDataViewTreeStoreNodes& children = parent_node->GetChildren();
    const wxDataViewTreeStoreNodes::iterator iter = parent_node->FindChild( previous_node );
    if (iter == children.end()) return wxDataViewItem(nullptr);

    wxDataViewTreeStoreContainerNode *node =
        new wxDataViewTreeStoreContainerNode( parent_node, text, icon, expanded, data );
    children.insert(iter, node);

    return node->GetItem();
}

bool wxDataViewTreeStore::IsContainer( const wxDataViewItem& item ) const
{
    wxDataViewTreeStoreNode *node = FindNode( item );
    if (!node) return false;

    return node->IsContainer();
}

wxDataViewItem wxDataViewTreeStore::GetNthChild( const wxDataViewItem& parent, unsigned int pos ) const
{
    wxDataViewTreeStoreContainerNode *parent_node = FindContainerNode( parent );
    if (!parent_node) return wxDataViewItem(nullptr);

    wxDataViewTreeStoreNode* const node = parent_node->GetChildren()[pos];
    if (node)
        return node->GetItem();

    return wxDataViewItem(nullptr);
}

int wxDataViewTreeStore::GetChildCount( const wxDataViewItem& parent ) const
{
    wxDataViewTreeStoreNode *node = FindNode( parent );
    if (!node) return -1;

    if (!node->IsContainer())
        return 0;

    wxDataViewTreeStoreContainerNode *container_node = (wxDataViewTreeStoreContainerNode*) node;
    return (int) container_node->GetChildren().size();
}

void wxDataViewTreeStore::SetItemText( const wxDataViewItem& item, const wxString &text )
{
    wxDataViewTreeStoreNode *node = FindNode( item );
    if (!node) return;

    node->SetText( text );
}

wxString wxDataViewTreeStore::GetItemText( const wxDataViewItem& item ) const
{
    wxDataViewTreeStoreNode *node = FindNode( item );
    if (!node) return wxEmptyString;

    return node->GetText();
}

void wxDataViewTreeStore::SetItemIcon( const wxDataViewItem& item, const wxBitmapBundle &icon )
{
    wxDataViewTreeStoreNode *node = FindNode( item );
    if (!node) return;

    node->SetIcon( icon );
}

wxIcon wxDataViewTreeStore::GetItemIcon( const wxDataViewItem& item ) const
{
    wxDataViewTreeStoreNode *node = FindNode( item );
    if (!node) return wxNullIcon;

    return node->GetIcon();
}

void wxDataViewTreeStore::SetItemExpandedIcon( const wxDataViewItem& item, const wxBitmapBundle &icon )
{
    wxDataViewTreeStoreContainerNode *node = FindContainerNode( item );
    if (!node) return;

    node->SetExpandedIcon( icon );
}

wxIcon wxDataViewTreeStore::GetItemExpandedIcon( const wxDataViewItem& item ) const
{
    wxDataViewTreeStoreContainerNode *node = FindContainerNode( item );
    if (!node) return wxNullIcon;

    return node->GetExpandedIcon();
}

void wxDataViewTreeStore::SetItemData( const wxDataViewItem& item, wxClientData *data )
{
    wxDataViewTreeStoreNode *node = FindNode( item );
    if (!node) return;

    node->SetData( data );
}

wxClientData *wxDataViewTreeStore::GetItemData( const wxDataViewItem& item ) const
{
    wxDataViewTreeStoreNode *node = FindNode( item );
    if (!node) return nullptr;

    return node->GetData();
}

void wxDataViewTreeStore::DeleteItem( const wxDataViewItem& item )
{
    if (!item.IsOk()) return;

    wxDataViewItem parent_item = GetParent( item );

    wxDataViewTreeStoreContainerNode *parent_node = FindContainerNode( parent_item );
    if (!parent_node) return;

    const wxDataViewTreeStoreNodes::iterator
        iter = parent_node->FindChild(FindNode(item));
    if ( iter != parent_node->GetChildren().end() )
    {
        delete *iter;
        parent_node->GetChildren().erase(iter);
    }
}

void wxDataViewTreeStore::DeleteChildren( const wxDataViewItem& item )
{
    wxDataViewTreeStoreContainerNode *node = FindContainerNode( item );
    if (!node) return;

    node->DestroyChildren();
}

void wxDataViewTreeStore::DeleteAllItems()
{
    DeleteChildren(wxDataViewItem(m_root));
}

void
wxDataViewTreeStore::GetValue(wxVariant &variant,
                              const wxDataViewItem &item,
                              unsigned int WXUNUSED(col)) const
{
    // if (col != 0) return;

    wxDataViewTreeStoreNode *node = FindNode( item );
    if (!node) return;

    wxBitmapBundle bb;
    if (node->IsContainer())
    {
        wxDataViewTreeStoreContainerNode *container = (wxDataViewTreeStoreContainerNode*) node;
        if (container->IsExpanded())
           bb = container->GetExpandedBitmapBundle();
    }

    if (!bb.IsOk())
        bb = node->GetBitmapBundle();

    wxDataViewIconText data( node->GetText(), bb );

    variant << data;
}

bool
wxDataViewTreeStore::SetValue(const wxVariant& variant,
                              const wxDataViewItem& item,
                              unsigned int WXUNUSED(col))
{
    // if (col != 0) return false;

    wxDataViewTreeStoreNode *node = FindNode( item );
    if (!node) return false;

    wxDataViewIconText data;

    data << variant;

    node->SetText( data.GetText() );
    node->SetIcon( data.GetIcon() );

    return true;
}

wxDataViewItem wxDataViewTreeStore::GetParent( const wxDataViewItem &item ) const
{
    wxDataViewTreeStoreNode *node = FindNode( item );
    if (!node) return wxDataViewItem(nullptr);

    wxDataViewTreeStoreNode *parent = node->GetParent();
    if (!parent) return wxDataViewItem(nullptr);

    if (parent == m_root)
        return wxDataViewItem(nullptr);

    return parent->GetItem();
}

unsigned int wxDataViewTreeStore::GetChildren( const wxDataViewItem &item, wxDataViewItemArray &children ) const
{
    wxDataViewTreeStoreContainerNode *node = FindContainerNode( item );
    if (!node) return 0;

    wxDataViewTreeStoreNodes::iterator iter;
    for (iter = node->GetChildren().begin(); iter != node->GetChildren().end(); ++iter)
    {
        wxDataViewTreeStoreNode* child = *iter;
        children.Add( child->GetItem() );
    }

    return node->GetChildren().size();
}

int wxDataViewTreeStore::Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
                         unsigned int WXUNUSED(column), bool WXUNUSED(ascending) ) const
{
    wxDataViewTreeStoreNode *node1 = FindNode( item1 );
    wxDataViewTreeStoreNode *node2 = FindNode( item2 );

    if (!node1 || !node2 || (node1 == node2))
        return 0;

    wxDataViewTreeStoreContainerNode* const parent =
        (wxDataViewTreeStoreContainerNode*) node1->GetParent();

    wxCHECK_MSG( node2->GetParent() == parent, 0,
                 wxS("Comparing items with different parent.") );

    if (node1->IsContainer() && !node2->IsContainer())
        return -1;

    if (node2->IsContainer() && !node1->IsContainer())
        return 1;

    wxDataViewTreeStoreNodes::const_iterator iter;
    for (iter = parent->GetChildren().begin(); iter != parent->GetChildren().end(); ++iter)
    {
        if ( *iter == node1 )
            return -1;

        if ( *iter == node2 )
            return 1;
    }

    wxFAIL_MSG(wxS("Unreachable"));
    return 0;
}

wxDataViewTreeStoreNode *wxDataViewTreeStore::FindNode( const wxDataViewItem &item ) const
{
    if (!item.IsOk())
        return m_root;

    return (wxDataViewTreeStoreNode*) item.GetID();
}

wxDataViewTreeStoreContainerNode *wxDataViewTreeStore::FindContainerNode( const wxDataViewItem &item ) const
{
    if (!item.IsOk())
        return (wxDataViewTreeStoreContainerNode*) m_root;

    wxDataViewTreeStoreNode* node = (wxDataViewTreeStoreNode*) item.GetID();

    if (!node->IsContainer())
        return nullptr;

    return (wxDataViewTreeStoreContainerNode*) node;
}

//-----------------------------------------------------------------------------
// wxDataViewTreeCtrl
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxDataViewTreeCtrl,wxDataViewCtrl);

wxBEGIN_EVENT_TABLE(wxDataViewTreeCtrl,wxDataViewCtrl)
   EVT_DATAVIEW_ITEM_EXPANDED(-1, wxDataViewTreeCtrl::OnExpanded)
   EVT_DATAVIEW_ITEM_COLLAPSED(-1, wxDataViewTreeCtrl::OnCollapsed)
   EVT_SIZE( wxDataViewTreeCtrl::OnSize )
wxEND_EVENT_TABLE()

bool wxDataViewTreeCtrl::Create( wxWindow *parent, wxWindowID id,
           const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator )
{
    if ( !wxDataViewCtrl::Create( parent, id, pos, size, style, validator ) )
        return false;

    // create the standard model and a column in the tree
    wxDataViewTreeStore *store = new wxDataViewTreeStore;
    AssociateModel( store );
    store->DecRef();

    AppendIconTextColumn
    (
        wxString(),                 // no label (header is not shown anyhow)
        0,                          // the only model column
        wxDATAVIEW_CELL_EDITABLE,
        -1,                         // default width
        wxALIGN_NOT,                //  and alignment
        0                           // not resizable
    );

    return true;
}

wxDataViewItem wxDataViewTreeCtrl::AppendItem( const wxDataViewItem& parent,
        const wxString &text, int iconIndex, wxClientData *data )
{
    wxDataViewItem res = GetStore()->
        AppendItem( parent, text, GetBitmapBundle(iconIndex), data );

    GetStore()->ItemAdded( parent, res );

    return res;
}

wxDataViewItem wxDataViewTreeCtrl::PrependItem( const wxDataViewItem& parent,
        const wxString &text, int iconIndex, wxClientData *data )
{
    wxDataViewItem res = GetStore()->
        PrependItem( parent, text, GetBitmapBundle(iconIndex), data );

    GetStore()->ItemAdded( parent, res );

    return res;
}

wxDataViewItem wxDataViewTreeCtrl::InsertItem( const wxDataViewItem& parent, const wxDataViewItem& previous,
        const wxString &text, int iconIndex, wxClientData *data )
{
    wxDataViewItem res = GetStore()->
        InsertItem( parent, previous, text, GetBitmapBundle(iconIndex), data );

    GetStore()->ItemAdded( parent, res );

    return res;
}

wxDataViewItem wxDataViewTreeCtrl::PrependContainer( const wxDataViewItem& parent,
        const wxString &text, int iconIndex, int expandedIndex, wxClientData *data )
{
    wxDataViewItem res = GetStore()->
        PrependContainer( parent, text,
                          GetBitmapBundle(iconIndex), GetBitmapBundle(expandedIndex), data );

    GetStore()->ItemAdded( parent, res );

    return res;
}

wxDataViewItem wxDataViewTreeCtrl::AppendContainer( const wxDataViewItem& parent,
        const wxString &text, int iconIndex, int expandedIndex, wxClientData *data )
{
    wxDataViewItem res = GetStore()->
        AppendContainer( parent, text,
                         GetBitmapBundle(iconIndex), GetBitmapBundle(expandedIndex), data );

    GetStore()->ItemAdded( parent, res );

    return res;
}

wxDataViewItem wxDataViewTreeCtrl::InsertContainer( const wxDataViewItem& parent, const wxDataViewItem& previous,
        const wxString &text, int iconIndex, int expandedIndex, wxClientData *data )
{
    wxDataViewItem res = GetStore()->
        InsertContainer( parent, previous, text,
                         GetBitmapBundle(iconIndex), GetBitmapBundle(expandedIndex), data );

    GetStore()->ItemAdded( parent, res );

    return res;
}

void wxDataViewTreeCtrl::SetItemText( const wxDataViewItem& item, const wxString &text )
{
    GetStore()->SetItemText(item,text);

    // notify control
    GetStore()->ValueChanged( item, 0 );
}

void wxDataViewTreeCtrl::SetItemIcon( const wxDataViewItem& item, const wxBitmapBundle &icon )
{
    GetStore()->SetItemIcon(item,icon);

    // notify control
    GetStore()->ValueChanged( item, 0 );
}

void wxDataViewTreeCtrl::SetItemExpandedIcon( const wxDataViewItem& item, const wxBitmapBundle &icon )
{
    GetStore()->SetItemExpandedIcon(item,icon);

    // notify control
    GetStore()->ValueChanged( item, 0 );
}

void wxDataViewTreeCtrl::DeleteItem( const wxDataViewItem& item )
{
    wxDataViewItem parent_item = GetStore()->GetParent( item );

    GetStore()->DeleteItem(item);

    // notify control
    GetStore()->ItemDeleted( parent_item, item );
}

void wxDataViewTreeCtrl::DeleteChildren( const wxDataViewItem& item )
{
    wxDataViewTreeStoreContainerNode *node = GetStore()->FindContainerNode( item );
    if (!node) return;

    wxDataViewItemArray array;
    wxDataViewTreeStoreNodes::iterator iter;
    for (iter = node->GetChildren().begin(); iter != node->GetChildren().end(); ++iter)
    {
        wxDataViewTreeStoreNode* child = *iter;
        array.Add( child->GetItem() );
    }

    GetStore()->DeleteChildren( item );

    // notify control
    GetStore()->ItemsDeleted( item, array );
}

void  wxDataViewTreeCtrl::DeleteAllItems()
{
    GetStore()->DeleteAllItems();

    GetStore()->Cleared();
}

void wxDataViewTreeCtrl::OnExpanded( wxDataViewEvent &event )
{
    wxDataViewTreeStoreContainerNode* container = GetStore()->FindContainerNode( event.GetItem() );
    if (!container) return;

    container->SetExpanded( true );

    GetStore()->ItemChanged( event.GetItem() );
}

void wxDataViewTreeCtrl::OnCollapsed( wxDataViewEvent &event )
{
    wxDataViewTreeStoreContainerNode* container = GetStore()->FindContainerNode( event.GetItem() );
    if (!container) return;

    container->SetExpanded( false );

    GetStore()->ItemChanged( event.GetItem() );
}

void wxDataViewTreeCtrl::OnSize( wxSizeEvent &event )
{
#if defined(wxHAS_GENERIC_DATAVIEWCTRL)
    // automatically resize our only column to take the entire control width
    if ( GetColumnCount() )
    {
        wxSize size = GetClientSize();
        GetColumn(0)->SetWidth(size.x);
    }
#endif
    event.Skip( true );
}

void wxDataViewTreeCtrl::OnImagesChanged()
{
    Refresh();
}

#endif // wxUSE_DATAVIEWCTRL

/////////////////////////////////////////////////////////////////////////////
// Name:        src/propgrid/propgridiface.cpp
// Purpose:     wxPropertyGridInterface class
// Author:      Jaakko Salli
// Modified by:
// Created:     2008-08-24
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#if wxUSE_PROPGRID

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/colour.h"
    #include "wx/font.h"
    #include "wx/log.h"
    #include "wx/textctrl.h"
    #include "wx/validate.h"
#endif

#include "wx/propgrid/propgrid.h"

// ----------------------------------------------------------------------------
// VariantDatas
// ----------------------------------------------------------------------------

WX_PG_IMPLEMENT_VARIANT_DATA_EXPORTED(wxPoint, WXDLLIMPEXP_PROPGRID)
WX_PG_IMPLEMENT_VARIANT_DATA_EXPORTED(wxSize, WXDLLIMPEXP_PROPGRID)
WX_PG_IMPLEMENT_VARIANT_DATA_EXPORTED_DUMMY_EQ(wxArrayInt, WXDLLIMPEXP_PROPGRID)
IMPLEMENT_VARIANT_OBJECT_EXPORTED(wxFont, WXDLLIMPEXP_PROPGRID)

// -----------------------------------------------------------------------
// wxPGPropArgCls
// -----------------------------------------------------------------------

wxPGProperty* wxPGPropArgCls::GetPtr( wxPropertyGridInterface* iface ) const
{
    if ( m_isProperty )
    {
        wxASSERT_MSG( m_property, "invalid property ptr" );
        return m_property;
    }

    return iface->GetPropertyByNameA(m_name);
}

// -----------------------------------------------------------------------
// wxPropertyGridInterface
// -----------------------------------------------------------------------

void wxPropertyGridInterface::RefreshGrid( wxPropertyGridPageState* state )
{
    if ( !state )
        state = m_pState;

    wxPropertyGrid* grid = state->GetGrid();
    if ( grid->GetState() == state && !grid->IsFrozen() )
    {
        grid->Refresh();
    }
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridInterface::Append( wxPGProperty* property )
{
    wxPGProperty* retp = m_pState->DoAppend(property);

    wxPropertyGrid* grid = m_pState->GetGrid();
    if ( grid )
        grid->RefreshGrid();

    return retp;
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridInterface::AppendIn( wxPGPropArg id, wxPGProperty* newproperty )
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(wxNullProperty)
    wxPGProperty* retp = m_pState->DoInsert(p, p->GetChildCount(), newproperty);
    return retp;
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridInterface::Insert( wxPGPropArg id, wxPGProperty* property )
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(wxNullProperty)
    wxPGProperty* retp = m_pState->DoInsert(p->GetParent(), p->GetIndexInParent(), property);
    RefreshGrid();
    return retp;
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridInterface::Insert( wxPGPropArg id, int index, wxPGProperty* newproperty )
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(wxNullProperty)
    wxPGProperty* retp = m_pState->DoInsert(p,index,newproperty);
    RefreshGrid();
    return retp;
}

// -----------------------------------------------------------------------

void wxPropertyGridInterface::DeleteProperty( wxPGPropArg id )
{
    wxPG_PROP_ARG_CALL_PROLOG()

    wxPropertyGridPageState* state = p->GetParentState();

    state->DoDelete( p, true );

    RefreshGrid(state);
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridInterface::RemoveProperty( wxPGPropArg id )
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(wxNullProperty)

    wxCHECK( !p->HasAnyChild() || p->HasFlag(wxPG_PROP_AGGREGATE),
             wxNullProperty);

    wxPropertyGridPageState* state = p->GetParentState();

    state->DoDelete( p, false );

    RefreshGrid(state);

    return p;
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridInterface::ReplaceProperty( wxPGPropArg id, wxPGProperty* property )
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(wxNullProperty)

    wxPGProperty* replaced = p;
    wxCHECK_MSG( replaced && property,
                 wxNullProperty,
                 wxS("null property") );
    wxCHECK_MSG( !replaced->IsCategory(),
                 wxNullProperty,
                 wxS("cannot replace this type of property") );
    wxCHECK_MSG( !m_pState->IsInNonCatMode(),
                 wxNullProperty,
                 wxS("cannot replace properties in alphabetic mode") );

    // Get address to the slot
    wxPGProperty* parent = replaced->GetParent();
    int ind = replaced->GetIndexInParent();

    wxPropertyGridPageState* state = replaced->GetParentState();
    DeleteProperty(replaced); // Must use generic Delete
    state->DoInsert(parent,ind,property);

    return property;
}

// -----------------------------------------------------------------------
// wxPropertyGridInterface property operations
// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridInterface::GetSelection() const
{
    return m_pState->GetSelection();
}

// -----------------------------------------------------------------------

bool wxPropertyGridInterface::ClearSelection( bool validation )
{
    bool res = DoClearSelection(validation, wxPGSelectPropertyFlags::DontSendEvent);
    wxPropertyGrid* pg = GetPropertyGrid();
    if ( pg )
        pg->Refresh();
    return res;
}

// -----------------------------------------------------------------------

bool wxPropertyGridInterface::DoClearSelection( bool validation,
                                                wxPGSelectPropertyFlags selFlags )
{
    if ( !validation )
        selFlags |= wxPGSelectPropertyFlags::NoValidate;

    wxPropertyGridPageState* state = m_pState;

    if ( state )
    {
        wxPropertyGrid* pg = state->GetGrid();
        if ( pg->GetState() == state )
            return pg->DoSelectProperty(nullptr, selFlags);
        else
            state->DoSetSelection(nullptr);
    }

    return true;
}

// -----------------------------------------------------------------------

void wxPropertyGridInterface::LimitPropertyEditing( wxPGPropArg id, bool limit )
{
    wxPG_PROP_ARG_CALL_PROLOG()

    m_pState->DoLimitPropertyEditing(p, limit);
    RefreshProperty(p);
}

// -----------------------------------------------------------------------

bool wxPropertyGridInterface::EnableProperty( wxPGPropArg id, bool enable )
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)

    wxPropertyGridPageState* state = p->GetParentState();
    wxPropertyGrid* grid = state->GetGrid();

    if ( enable )
    {
        if ( !p->HasFlag(wxPG_PROP_DISABLED) )
            return false;

        // If active, Set active Editor.
        if ( grid && grid->GetState() == state && p == grid->GetSelection() )
            grid->DoSelectProperty( p, wxPGSelectPropertyFlags::Force );
    }
    else
    {
        if ( p->HasFlag(wxPG_PROP_DISABLED) )
            return false;

        // If active, Disable as active Editor.
        if ( grid && grid->GetState() == state && p == grid->GetSelection() )
            grid->DoSelectProperty( p, wxPGSelectPropertyFlags::Force );
    }

    p->DoEnable(enable);

    RefreshProperty( p );

    return true;
}

void wxPropertyGridInterface::SetPropertyReadOnly( wxPGPropArg id, bool set, wxPGPropertyValuesFlags flags)
{
    wxPG_PROP_ARG_CALL_PROLOG()

    if ( !!(flags & wxPGPropertyValuesFlags::Recurse) )
    {
        p->SetFlagRecursively(wxPG_PROP_READONLY, set);
    }
    else
    {
        // Do nothing if flag is already set as required.
        if ( set && p->HasFlag(wxPG_PROP_READONLY) )
            return;
        if ( !set && !p->HasFlag(wxPG_PROP_READONLY) )
            return;

        p->ChangeFlag(wxPG_PROP_READONLY, set);
    }

    wxPropertyGridPageState* state = p->GetParentState();
    if( state )
    {
        // If property is attached to the property grid
        // then refresh the view.
        RefreshProperty( p );
    }
}

// -----------------------------------------------------------------------

bool wxPropertyGridInterface::ExpandAll( bool doExpand )
{
    wxPropertyGridPageState* state = m_pState;

    if ( !state->DoGetRoot()->HasAnyChild() )
        return true;

    wxPropertyGrid* pg = state->GetGrid();

    if ( GetSelection() && GetSelection() != state->DoGetRoot() &&
         !doExpand )
    {
        pg->DoClearSelection();
    }

    wxPGVIterator it;

    for ( it = GetVIterator( wxPG_ITERATE_ALL ); !it.AtEnd(); it.Next() )
    {
        wxPGProperty* p = it.GetProperty();
        if ( p->HasAnyChild() )
        {
            if ( doExpand )
            {
                if ( !p->IsExpanded() )
                {
                    state->DoExpand(p);
                }
            }
            else
            {
                if ( p->IsExpanded() )
                {
                    state->DoCollapse(p);
                }
            }
        }
    }

    pg->RecalculateVirtualSize();

    RefreshGrid();

    return true;
}

// -----------------------------------------------------------------------

void wxPropertyGridInterface::ClearModifiedStatus()
{
    unsigned int pageIndex = 0;

    wxPropertyGridPageState* page;
    while ( (page = GetPageState(pageIndex)) != nullptr )
    {
        page->DoGetRoot()->SetFlagRecursively(wxPG_PROP_MODIFIED, false);
        page->m_anyModified = false;

        pageIndex++;
    }

    // Update active editor control, if any
    GetPropertyGrid()->RefreshEditor();
}

bool wxPropertyGridInterface::SetColumnProportion( unsigned int column,
                                                   int proportion )
{
    wxCHECK(m_pState, false);
    wxPropertyGrid* pg = m_pState->GetGrid();
    wxCHECK(pg, false);
    wxCHECK(pg->HasFlag(wxPG_SPLITTER_AUTO_CENTER), false);
    m_pState->DoSetColumnProportion(column, proportion);
    return true;
}

// -----------------------------------------------------------------------
// wxPropertyGridInterface property value setting and getting
// -----------------------------------------------------------------------

void wxPGGetFailed( const wxPGProperty* p, const wxString& typestr )
{
    wxPGTypeOperationFailed(p, typestr, wxS("Get"));
}

// -----------------------------------------------------------------------

void wxPGTypeOperationFailed( const wxPGProperty* p,
                              const wxString& typestr,
                              const wxString& op )
{
    wxASSERT( p != nullptr );
    wxLogError( _("Type operation \"%s\" failed: Property labeled \"%s\" is of type \"%s\", NOT \"%s\"."),
        op, p->GetLabel(), p->GetValue().GetType(), typestr );
}

// -----------------------------------------------------------------------

void wxPropertyGridInterface::SetPropVal( wxPGPropArg id, wxVariant& value )
{
    wxPG_PROP_ARG_CALL_PROLOG()

    if ( p )
        p->SetValue(value);
}

// -----------------------------------------------------------------------

void wxPropertyGridInterface::SetPropertyValueString( wxPGPropArg id, const wxString& value )
{
    wxPG_PROP_ARG_CALL_PROLOG()

    if ( p )
        m_pState->DoSetPropertyValueString(p, value);
}

// -----------------------------------------------------------------------

void wxPropertyGridInterface::SetValidationFailureBehavior( wxPGVFBFlags vfbFlags )
{
    GetPropertyGrid()->m_permanentValidationFailureBehavior = vfbFlags;
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridInterface::GetPropertyByNameA( const wxString& name ) const
{
    wxPGProperty* p = GetPropertyByName(name);
    wxASSERT_MSG(p,wxString::Format(wxS("no property with name '%s'"),name));
    return p;
}

// ----------------------------------------------------------------------------

wxPGProperty* wxPropertyGridInterface::GetPropertyByLabel( const wxString& label ) const
{
    return m_pState->BaseGetPropertyByLabel(label, nullptr);
}

// ----------------------------------------------------------------------------

void wxPropertyGridInterface::DoSetPropertyAttribute( wxPGPropArg id, const wxString& name,
                                                      wxVariant& value, wxPGPropertyValuesFlags argFlags )
{
    wxPG_PROP_ARG_CALL_PROLOG()

    p->SetAttribute( name, value ); // property is also refreshed here

    if ( !!(argFlags & wxPGPropertyValuesFlags::Recurse) )
    {
        for ( unsigned int i = 0; i < p->GetChildCount(); i++ )
            DoSetPropertyAttribute(p->Item(i), name, value, argFlags);
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridInterface::SetPropertyAttributeAll( const wxString& attrName,
                                                       wxVariant value )
{
    unsigned int pageIndex = 0;

    wxPropertyGridPageState* page;
    while ( (page = GetPageState(pageIndex)) != nullptr )
    {
        DoSetPropertyAttribute(page->DoGetRoot(), attrName, value, wxPGPropertyValuesFlags::Recurse);

        pageIndex++;
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridInterface::GetPropertiesWithFlag( wxArrayPGProperty* targetArr,
                                                     wxPGProperty::FlagType flags,
                                                     bool inverse,
                                                     int iterFlags ) const
{
    wxASSERT( targetArr );
    wxPGVIterator it = GetVIterator( iterFlags );

    for ( ;
          !it.AtEnd();
          it.Next() )
    {
        wxPGProperty* property = it.GetProperty();

        if ( !inverse )
        {
            if ( property->HasFlagsExact(flags) )
                targetArr->push_back(property);
        }
        else
        {
            if ( !property->HasFlagsExact(flags) )
                targetArr->push_back(property);
        }
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridInterface::SetBoolChoices( const wxString& trueChoice,
                                                 const wxString& falseChoice )
{
    wxPGGlobalVars->m_boolChoices[0] = falseChoice;
    wxPGGlobalVars->m_boolChoices[1] = trueChoice;
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridInterface::DoGetPropertyByName( const wxString& name ) const
{
    return m_pState->BaseGetPropertyByName(name);
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridInterface::GetPropertyByName( const wxString& name,
                                                             const wxString& subname ) const
{
    wxPGProperty* p = DoGetPropertyByName(name);
    if ( !p || !p->HasAnyChild() )
        return wxNullProperty;

    return p->GetPropertyByName(subname);
}

// -----------------------------------------------------------------------

// Since GetPropertyByName is used *a lot*, this makes sense
// since non-virtual method can be called with less code.
wxPGProperty* wxPropertyGridInterface::GetPropertyByName( const wxString& name ) const
{
    wxPGProperty* p = DoGetPropertyByName(name);
    if ( p )
        return p;

    // Check if it is "Property.SubProperty" format
    int pos = name.Find(wxS('.'));
    if ( pos <= 0 )
        return nullptr;

    return GetPropertyByName(name.substr(0,pos),
                             name.substr(pos+1,name.length()-pos-1));
}

// -----------------------------------------------------------------------

bool wxPropertyGridInterface::HideProperty(wxPGPropArg id, bool hide, wxPGPropertyValuesFlags flags)
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)

    // Do nothing if single property is already hidden/visible as requested.
    if ( !(flags & wxPGPropertyValuesFlags::Recurse) )
    {
        if ( hide && p->HasFlag(wxPG_PROP_HIDDEN) )
            return false;
        if ( !hide && !p->HasFlag(wxPG_PROP_HIDDEN) )
            return false;
    }

    wxPropertyGrid* pg = m_pState->GetGrid();

    if ( pg == p->GetGrid() )
        return pg->DoHideProperty(p, hide, flags);
    else
        m_pState->DoHideProperty(p, hide, flags);

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGridInterface::Collapse( wxPGPropArg id )
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)
    wxPropertyGrid* pg = p->GetGridIfDisplayed();
    if ( pg )
        return pg->DoCollapse(p);

    return p->GetParentState()->DoCollapse(p);
}

// -----------------------------------------------------------------------

bool wxPropertyGridInterface::Expand( wxPGPropArg id )
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)
    wxPropertyGrid* pg = p->GetGridIfDisplayed();
    if ( pg )
        return pg->DoExpand(p);

    return p->GetParentState()->DoExpand(p);
}

// -----------------------------------------------------------------------

void wxPropertyGridInterface::Sort(wxPGPropertyValuesFlags flags)
{
    wxPropertyGrid* pg = GetPropertyGrid();

    unsigned int pageIndex = 0;

    wxPropertyGridPageState* page;
    while( (page = GetPageState(pageIndex)) != nullptr )
    {
        page->DoSort(flags);
        pageIndex++;
    }

    // Fix positions of any open editor controls
    if ( pg )
        pg->CorrectEditorWidgetPosY();
}

// -----------------------------------------------------------------------

void wxPropertyGridInterface::SetPropertyLabel( wxPGPropArg id, const wxString& newproplabel )
{
    wxPG_PROP_ARG_CALL_PROLOG()

    if ( p->GetLabel() == newproplabel )
        return;

    p->SetLabel( newproplabel );

    wxPropertyGridPageState* state = p->GetParentState();
    wxPropertyGrid* pg = state->GetGrid();

    if ( pg->HasFlag(wxPG_AUTO_SORT) )
        pg->SortChildren(p->GetParent());

    if ( pg->GetState() == state )
    {
        if ( pg->HasFlag(wxPG_AUTO_SORT) )
        {
            pg->Refresh();
            // If any property is selected it has to
            // be refreshed in the new location.
            if ( pg == p->GetGrid() && pg->GetSelectedProperty() )
            {
                RefreshProperty(pg->GetSelectedProperty());
            }
        }
        else
        {
            pg->DrawItem( p );
        }
    }
}

// -----------------------------------------------------------------------

bool wxPropertyGridInterface::SetPropertyMaxLength( wxPGPropArg id, int maxLen )
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)

    if ( !p->SetMaxLength(maxLen) )
        return false;

    wxPropertyGrid* pg = m_pState->GetGrid();

    // Adjust control if selected currently
    if ( pg == p->GetGrid() && p == m_pState->GetSelection() )
    {
        wxWindow* wnd = pg->GetEditorControl();
        wxTextCtrl* tc = wxDynamicCast(wnd, wxTextCtrl);
        wxCHECK_MSG(tc, false, "Text ctrl is expected here");
        tc->SetMaxLength(maxLen);
    }

    return true;
}

// -----------------------------------------------------------------------

void wxPropertyGridInterface::SetPropertyBackgroundColour(wxPGPropArg id, const wxColour& colour,
                                                          wxPGPropertyValuesFlags flags )
{
    wxPG_PROP_ARG_CALL_PROLOG()
    p->SetBackgroundColour(colour, flags);

    // Redraw the control
    wxPropertyGrid* pg = m_pState->GetGrid();
    if ( pg == p->GetGrid() )
    {
        if ( !!(flags & wxPGPropertyValuesFlags::Recurse) )
            pg->DrawItemAndChildren(p);
        else
            pg->DrawItem(p);
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridInterface::SetPropertyTextColour(wxPGPropArg id, const wxColour& colour,
                                                    wxPGPropertyValuesFlags flags)
{
    wxPG_PROP_ARG_CALL_PROLOG()
    p->SetTextColour(colour, flags);

    // Redraw the control
    wxPropertyGrid* pg = m_pState->GetGrid();
    if ( pg == p->GetGrid() )
    {
        if ( !!(flags & wxPGPropertyValuesFlags::Recurse) )
            pg->DrawItemAndChildren(p);
        else
            pg->DrawItem(p);
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridInterface::SetPropertyColoursToDefault(wxPGPropArg id, wxPGPropertyValuesFlags flags)
{
    wxPG_PROP_ARG_CALL_PROLOG()
    p->SetDefaultColours(flags);

    // Redraw the control
    wxPropertyGrid* pg = m_pState->GetGrid();
    if ( pg == p->GetGrid() )
    {
        if ( !!(flags & wxPGPropertyValuesFlags::Recurse) )
            pg->DrawItemAndChildren(p);
        else
            pg->DrawItem(p);
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridInterface::SetPropertyCell( wxPGPropArg id,
                                               int column,
                                               const wxString& text,
                                               const wxBitmapBundle& bitmap,
                                               const wxColour& fgCol,
                                               const wxColour& bgCol )
{
    wxPG_PROP_ARG_CALL_PROLOG()

    wxPGCell& cell = p->GetCell(column);
    if ( !text.empty() && text != wxPG_LABEL )
        cell.SetText(text);
    if ( bitmap.IsOk() )
        cell.SetBitmap(bitmap);
    if ( fgCol != wxNullColour )
        cell.SetFgCol(fgCol);
    if ( bgCol != wxNullColour )
        cell.SetBgCol(bgCol);
}

// -----------------------------------------------------------------------
// GetPropertyValueAsXXX methods

wxVariant wxPropertyGridInterface::GetPropertyValue(wxPGPropArg id)
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(wxVariant())
    return p->GetValue();
}

// String is different from others.
wxString wxPropertyGridInterface::GetPropertyValueAsString( wxPGPropArg id ) const
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(wxString())
    return p->GetValueAsString(wxPG_FULL_VALUE);
}

bool wxPropertyGridInterface::GetPropertyValueAsBool( wxPGPropArg id ) const
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)
    wxVariant value = p->GetValue();
    if ( value.IsType(wxPG_VARIANT_TYPE_BOOL) )
    {
        return value.GetBool();
    }
    if ( value.IsType(wxPG_VARIANT_TYPE_LONG) )
    {
        return value.GetLong()?true:false;
    }
    wxPGGetFailed(p, wxPG_VARIANT_TYPE_BOOL);
    return false;
}

#define wxPG_PROP_ID_GETPROPVAL_CALL_PROLOG_RETVAL(PGTypeName, DEFVAL) \
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(DEFVAL) \
    wxVariant value = p->GetValue(); \
    if ( !value.IsType(PGTypeName) ) \
    { \
        wxPGGetFailed(p, PGTypeName); \
        return DEFVAL; \
    }

long wxPropertyGridInterface::GetPropertyValueAsLong(wxPGPropArg id) const
{
    wxPG_PROP_ID_GETPROPVAL_CALL_PROLOG_RETVAL(wxPG_VARIANT_TYPE_LONG, 0L)
    return value.GetLong();
}

double wxPropertyGridInterface::GetPropertyValueAsDouble(wxPGPropArg id) const
{
    wxPG_PROP_ID_GETPROPVAL_CALL_PROLOG_RETVAL(wxPG_VARIANT_TYPE_DOUBLE, 0.0)
    return value.GetDouble();
}

wxArrayString wxPropertyGridInterface::GetPropertyValueAsArrayString(wxPGPropArg id) const
{
    wxPG_PROP_ID_GETPROPVAL_CALL_PROLOG_RETVAL(wxPG_VARIANT_TYPE_ARRSTRING, wxArrayString())
    return value.GetArrayString();
}

#if defined(wxLongLong_t) && wxUSE_LONGLONG
wxLongLong_t wxPropertyGridInterface::GetPropertyValueAsLongLong(wxPGPropArg id) const
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(0)
    return p->GetValue().GetLongLong().GetValue();
}

wxULongLong_t wxPropertyGridInterface::GetPropertyValueAsULongLong(wxPGPropArg id) const
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(0)
    return p->GetValue().GetULongLong().GetValue();
}
#endif

wxArrayInt wxPropertyGridInterface::GetPropertyValueAsArrayInt(wxPGPropArg id) const
{
    wxPG_PROP_ID_GETPROPVAL_CALL_PROLOG_RETVAL(wxArrayInt_VariantType, wxArrayInt())
    wxArrayInt arr;
    arr << value;
    return arr;
}

#if wxUSE_DATETIME
wxDateTime wxPropertyGridInterface::GetPropertyValueAsDateTime(wxPGPropArg id) const
{
    wxPG_PROP_ID_GETPROPVAL_CALL_PROLOG_RETVAL(wxPG_VARIANT_TYPE_DATETIME, wxDateTime())
    return value.GetDateTime();
}
#endif

bool wxPropertyGridInterface::IsPropertyExpanded( wxPGPropArg id ) const
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)
    return p->IsExpanded();
}

// -----------------------------------------------------------------------
// wxPropertyGridInterface wrappers
// -----------------------------------------------------------------------

bool wxPropertyGridInterface::ChangePropertyValue( wxPGPropArg id, wxVariant newValue )
{
    return GetPropertyGrid()->ChangePropertyValue(id, newValue);
}

// -----------------------------------------------------------------------

void wxPropertyGridInterface::BeginAddChildren( wxPGPropArg id )
{
    wxPG_PROP_ARG_CALL_PROLOG()
    wxCHECK_RET( p->HasFlag(wxPG_PROP_AGGREGATE), wxS("only call on properties with fixed children") );
    p->ClearFlag(wxPG_PROP_AGGREGATE);
    p->SetFlag(wxPG_PROP_MISC_PARENT);
}

// -----------------------------------------------------------------------

bool wxPropertyGridInterface::EditorValidate()
{
    return GetPropertyGrid()->DoEditorValidate();
}

// -----------------------------------------------------------------------

void wxPropertyGridInterface::EndAddChildren( wxPGPropArg id )
{
    wxPG_PROP_ARG_CALL_PROLOG()
    wxCHECK_RET( p->HasFlag(wxPG_PROP_MISC_PARENT), wxS("only call on properties for which BeginAddChildren was called prior") );
    p->ClearFlag(wxPG_PROP_MISC_PARENT);
    p->SetFlag(wxPG_PROP_AGGREGATE);
}

// -----------------------------------------------------------------------
// wxPGVIterator_State
// -----------------------------------------------------------------------

// Default returned by wxPropertyGridInterface::GetVIterator().
class wxPGVIteratorBase_State : public wxPGVIteratorBase
{
public:
    wxPGVIteratorBase_State( wxPropertyGridPageState* state, int flags )
    {
        m_it.Init( state, flags );
    }
    virtual ~wxPGVIteratorBase_State() = default;
    virtual void Next() override { m_it.Next(); }
    virtual bool AtEnd() const override { return m_it.AtEnd(); }
};

wxPGVIterator wxPropertyGridInterface::GetVIterator( int flags ) const
{
    return wxPGVIterator( new wxPGVIteratorBase_State( m_pState, flags ) );
}

// -----------------------------------------------------------------------
// wxPGEditableState related functions
// -----------------------------------------------------------------------

// EscapeDelimiters() changes ";" into "\;" and "|" into "\|"
// in the input string.  This is an internal functions which is
// used for saving states
// NB: Similar function exists in aui/framemanager.cpp
static wxString EscapeDelimiters(const wxString& s)
{
    wxString result;
    result.reserve(s.length());

    for ( wxStringCharType ch : s )
    {
        if ( ch == wxS(';') || ch == wxS('|') || ch == wxS(',') )
            result += wxS('\\');
        result += ch;
    }

    return result;
}

wxString wxPropertyGridInterface::SaveEditableState( int includedStates ) const
{
    wxString result;

    //
    // Save state on page basis
    std::vector<wxPropertyGridPageState*> pageStates;
    unsigned int pageIndex = 0;
    wxPropertyGridPageState* page;
    while ( (page = GetPageState(pageIndex)) != nullptr )
    {
        pageStates.push_back(page);
        pageIndex++;
    }

    for( wxPropertyGridPageState* pageState : pageStates )
    {
        if ( includedStates & SelectionState )
        {
            wxString sel;
            if ( pageState->GetSelection() )
                sel = pageState->GetSelection()->GetName();
            result += wxS("selection=");
            result += EscapeDelimiters(sel);
            result += wxS(";");
        }
        if ( includedStates & ExpandedState )
        {
            wxPropertyGridConstIterator it =
                wxPropertyGridConstIterator( pageState,
                                             wxPG_ITERATE_ALL_PARENTS_RECURSIVELY|wxPG_ITERATE_HIDDEN,
                                             wxNullProperty );

            result += wxS("expanded=");

            for ( ;
                  !it.AtEnd();
                  it.Next() )
            {
                const wxPGProperty* p = it.GetProperty();

                if ( !p->HasFlag(wxPG_PROP_COLLAPSED) )
                    result += EscapeDelimiters(p->GetName());
                result += wxS(",");

            }

            if ( result.Last() == wxS(',') )
                result.RemoveLast();

            result += wxS(";");
        }
        if ( includedStates & ScrollPosState )
        {
            int x, y;
            GetPropertyGrid()->GetViewStart(&x,&y);
            result += wxString::Format(wxS("scrollpos=%i,%i;"), x, y);
        }
        if ( includedStates & SplitterPosState )
        {
            result += wxS("splitterpos=");

            for ( size_t i=0; i<pageState->GetColumnCount(); i++ )
                result += wxString::Format(wxS("%i,"), pageState->DoGetSplitterPosition(i));

            result.RemoveLast();  // Remove last comma
            result += wxS(";");
        }
        if ( includedStates & PageState )
        {
            result += wxS("ispageselected=");

            if ( GetPageState(-1) == pageState )
                result += wxS("1;");
            else
                result += wxS("0;");
        }
        if ( includedStates & DescBoxState )
        {
            wxVariant v = GetEditableStateItem(wxS("descboxheight"));
            if ( !v.IsNull() )
                result += wxString::Format(wxS("descboxheight=%i;"), (int)v.GetLong());
        }
        result.RemoveLast();  // Remove last semicolon
        result += wxS("|");
    }

    // Remove last '|'
    if ( !result.empty() )
        result.RemoveLast();

    return result;
}

bool wxPropertyGridInterface::RestoreEditableState( const wxString& src, int restoreStates )
{
    wxPropertyGrid* pg = GetPropertyGrid();
    wxPGProperty* newSelection = nullptr;
    size_t pageIndex;
    long vx = -1;
    long vy = -1;
    long selectedPage = -1;
    bool pgSelectionSet = false;
    bool res = true;

    pg->Freeze();
    wxArrayString pageStrings = ::wxSplit(src, wxS('|'), wxS('\\'));

    for ( pageIndex=0; pageIndex<pageStrings.size(); pageIndex++ )
    {
        wxPropertyGridPageState* pageState = GetPageState(pageIndex);
        if ( !pageState )
            break;

        wxArrayString kvpairStrings = ::wxSplit(pageStrings[pageIndex], wxS(';'), wxS('\\'));

        for( const wxString& kvs : kvpairStrings )
        {
            int eq_pos = kvs.Find(wxS('='));
            if ( eq_pos != wxNOT_FOUND )
            {
                wxString key = kvs.substr(0, eq_pos);
                wxString value = kvs.substr(eq_pos+1);

                // Further split value by commas
                wxArrayString values = ::wxSplit(value, wxS(','), wxS('\\'));

                if ( key == wxS("expanded") )
                {
                    if ( restoreStates & ExpandedState )
                    {
                        wxPropertyGridIterator it =
                            wxPropertyGridIterator( pageState,
                                                    wxPG_ITERATE_ALL,
                                                    wxNullProperty );

                        // First collapse all
                        for ( ; !it.AtEnd(); it.Next() )
                        {
                            wxPGProperty* p = it.GetProperty();
                            pageState->DoCollapse(p);
                        }

                        // Then expand those which names are in values
                        for( const wxString& name : values )
                        {
                            wxPGProperty* prop = GetPropertyByName(name);
                            if ( prop )
                                pageState->DoExpand(prop);
                        }
                    }
                }
                else if ( key == wxS("scrollpos") )
                {
                    if ( restoreStates & ScrollPosState )
                    {
                        if ( values.size() == 2 )
                        {
                            values[0].ToLong(&vx);
                            values[1].ToLong(&vy);
                        }
                        else
                        {
                            res = false;
                        }
                    }
                }
                else if ( key == wxS("splitterpos") )
                {
                    if ( restoreStates & SplitterPosState )
                    {
                        for ( size_t n=0; n<values.size(); n++ )
                        {
                            long pos = 0;
                            values[n].ToLong(&pos);
                            if ( pos > 0 )
                                pageState->DoSetSplitter(pos, n);
                        }
                    }
                }
                else if ( key == wxS("selection") )
                {
                    if ( restoreStates & SelectionState )
                    {
                        if ( !values.empty() )
                        {
                            if ( pageState->IsDisplayed() )
                            {
                                if ( !values[0].empty() )
                                    newSelection = GetPropertyByName(value);
                                pgSelectionSet = true;
                            }
                            else
                            {
                                if ( !values[0].empty() )
                                    pageState->DoSetSelection(GetPropertyByName(value));
                                else
                                    pageState->DoClearSelection();
                            }
                        }
                    }
                }
                else if ( key == wxS("ispageselected") )
                {
                    if ( restoreStates & PageState )
                    {
                        long pageSelStatus;
                        if ( values.size() == 1 && values[0].ToLong(&pageSelStatus) )
                        {
                            if ( pageSelStatus )
                                selectedPage = pageIndex;
                        }
                        else
                        {
                            res = false;
                        }
                    }
                }
                else if ( key == wxS("descboxheight") )
                {
                    if ( restoreStates & DescBoxState )
                    {
                        long descBoxHeight;
                        if ( values.size() == 1 && values[0].ToLong(&descBoxHeight) )
                        {
                            SetEditableStateItem(wxS("descboxheight"), descBoxHeight);
                        }
                        else
                        {
                            res = false;
                        }
                    }
                }
                else
                {
                    res = false;
                }
            }
        }
    }

    //
    // Force recalculation of virtual heights of all pages
    // (may be needed on unclean source string).
    pageIndex = 0;
    wxPropertyGridPageState* pageState;
    while ( (pageState = GetPageState(pageIndex)) != nullptr )
    {
        pageState->VirtualHeightChanged();
        pageIndex++;
    }

    pg->Thaw();

    //
    // Selection of visible grid page must be set after Thaw() call
    if ( pgSelectionSet )
    {
        if ( newSelection )
            pg->DoSelectProperty(newSelection);
        else
            pg->DoClearSelection();
    }

    if ( selectedPage != -1 )
    {
        wxPropertyGridPageState* curPageState = GetPageState(-1);
        DoSelectPage(selectedPage);
        if ( GetPageState(-1) != curPageState )
        {
            pg->SendEvent(wxEVT_PG_SELECTED, pg->GetSelectedProperty());
        }
    }

    if ( vx >= 0 )
    {
        pg->Scroll(vx, vy);
    }

    return res;
}

#endif  // wxUSE_PROPGRID


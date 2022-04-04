/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/choice_osx.cpp
// Purpose:     wxChoice
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_CHOICE

#include "wx/choice.h"

#ifndef WX_PRECOMP
    #include "wx/menu.h"
    #include "wx/dcclient.h"
#endif

#include "wx/osx/private.h"

wxChoice::~wxChoice()
{
    if ( HasClientObjectData() )
    {
        unsigned int i, max = GetCount();

        for ( i = 0; i < max; ++i )
            delete GetClientObject( i );
    }
}

bool wxChoice::Create(wxWindow *parent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    const wxArrayString& choices,
    long style,
    const wxValidator& validator,
    const wxString& name )
{
    if ( !Create( parent, id, pos, size, 0, NULL, style, validator, name ) )
        return false;

    Append( choices );

    if ( !choices.empty() )
        SetSelection( 0 );

    SetInitialSize( size );

    return true;
}

bool wxChoice::Create(wxWindow *parent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    int n,
    const wxString choices[],
    long style,
    const wxValidator& validator,
    const wxString& name )
{    
    DontCreatePeer();
    
    if ( !wxChoiceBase::Create( parent, id, pos, size, style, validator, name ) )
        return false;

    SetPeer(wxWidgetImpl::CreateChoice( this, parent, id, NULL, pos, size, style, GetExtraStyle() ));

    MacPostControlCreate( pos, size );

#if !wxUSE_STD_CONTAINERS
    if ( style & wxCB_SORT )
        // autosort
        m_strings = wxArrayString( 1 );
#endif

    Append(n, choices);

    // Set the first item as being selected
    if (n > 0)
        SetSelection( 0 );

    // Needed because it is a wxControlWithItems
    SetInitialSize( size );

    return true;
}

// ----------------------------------------------------------------------------
// adding/deleting items to/from the list
// ----------------------------------------------------------------------------

void wxChoice::DoAfterItemCountChange()
{
    InvalidateBestSize();

    GetPeer()->SetMaximum( GetCount() );
}

int wxChoice::DoInsertItems(const wxArrayStringsAdapter & items,
                            unsigned int pos,
                            void **clientData, wxClientDataType type)
{
    const unsigned int numItems = items.GetCount();
    for( unsigned int i = 0; i < numItems; ++i, ++pos )
    {
        unsigned int idx;

#if wxUSE_STD_CONTAINERS
        if ( IsSorted() )
        {
            wxArrayString::iterator
                insertPoint = std::lower_bound( m_strings.begin(), m_strings.end(), items[i] );
            idx = insertPoint - m_strings.begin();
            m_strings.insert( insertPoint, items[i] );
        }
        else
#endif // wxUSE_STD_CONTAINERS
        {
            idx = pos;
            m_strings.Insert( items[i], idx );
        }

        wxString text = items[i];
        if (text.empty())
            text = " ";  // menu items can't have empty labels
        dynamic_cast<wxChoiceWidgetImpl*>(GetPeer())->InsertItem( idx, i+1, text );
        m_datas.Insert( NULL, idx );
        AssignNewItemClientData(idx, clientData, i, type);
    }

    DoAfterItemCountChange();

    return pos - 1;
}

void wxChoice::DoDeleteOneItem(unsigned int n)
{
    wxCHECK_RET( IsValid(n) , wxT("wxChoice::Delete: invalid index") );

    if ( HasClientObjectData() )
        delete GetClientObject( n );

    // Deselect item being removed
    int selIdx = GetSelection();
    if ( selIdx != -1 && selIdx == int(n) )
        SetSelection(-1);

    dynamic_cast<wxChoiceWidgetImpl*>(GetPeer())->RemoveItem(n);
    m_strings.RemoveAt( n ) ;
    m_datas.RemoveAt( n ) ;

    DoAfterItemCountChange();
}

void wxChoice::DoClear()
{
    dynamic_cast<wxChoiceWidgetImpl*>(GetPeer())->Clear();

    m_strings.Empty() ;
    m_datas.Empty() ;

    DoAfterItemCountChange();
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------
int wxChoice::GetSelection() const
{
    return GetPeer()->GetValue();
}

void wxChoice::SetSelection( int n )
{
    GetPeer()->SetValue( n );
}

// ----------------------------------------------------------------------------
// string list functions
// ----------------------------------------------------------------------------

unsigned int wxChoice::GetCount() const
{
    return m_strings.GetCount() ;
}

int wxChoice::FindString( const wxString& s, bool bCase ) const
{
#if !wxUSE_STD_CONTAINERS
    // Avoid assert for non-default args passed to sorted array Index
    if ( IsSorted() )
        bCase = true;
#endif

    return m_strings.Index( s , bCase ) ;
}

void wxChoice::SetString(unsigned int n, const wxString& s)
{
    wxCHECK_RET( IsValid(n), wxT("wxChoice::SetString(): invalid index") );

    m_strings[n] = s ;

    dynamic_cast<wxChoiceWidgetImpl*>(GetPeer())->SetItem(n,s);
}

wxString wxChoice::GetString(unsigned int n) const
{
    wxCHECK_MSG( IsValid(n), wxEmptyString, wxT("wxChoice::GetString(): invalid index") );

    return m_strings[n] ;
}

// ----------------------------------------------------------------------------
// client data
// ----------------------------------------------------------------------------
void wxChoice::DoSetItemClientData(unsigned int n, void* clientData)
{
    m_datas[n] = (char*)clientData ;
}

void * wxChoice::DoGetItemClientData(unsigned int n) const
{
    return (void *)m_datas[n];
}

bool wxChoice::OSXHandleClicked( double WXUNUSED(timestampsec) )
{
    SendSelectionChangedEvent(wxEVT_CHOICE);

    return true ;
}

wxSize wxChoice::DoGetBestSize() const
{
    // We use the base window size for the height (which is wrong as it doesn't
    // take the font into account -- TODO) and add some margins to the width
    // computed by the base class method to account for the arrow.
    const int lbHeight = wxWindow::DoGetBestSize().y;

    return wxSize(wxChoiceBase::DoGetBestSize().x + 4*GetCharWidth(),
                  lbHeight);
}

#endif // wxUSE_CHOICE

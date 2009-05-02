/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/choice.cpp
// Purpose:     wxChoice
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
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

#include "wx/mac/uma.h"

extern MenuHandle NewUniqueMenu() ;

IMPLEMENT_DYNAMIC_CLASS(wxChoice, wxControl)


wxChoice::~wxChoice()
{
    if ( HasClientObjectData() )
    {
        unsigned int i, max = GetCount();

        for ( i = 0; i < max; ++i )
            delete GetClientObject( i );
    }

    // DeleteMenu( m_macPopUpMenuId ) ;
    // DisposeMenu( m_macPopUpMenuHandle ) ;
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
    wxCArrayString chs( choices );

    return Create(
        parent, id, pos, size, chs.GetCount(), chs.GetStrings(),
        style, validator, name );
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
    m_macIsUserPane = false;

    if ( !wxChoiceBase::Create( parent, id, pos, size, style, validator, name ) )
        return false;

    Rect bounds = wxMacGetBoundsForControl( this , pos , size );

    m_peer = new wxMacControl( this ) ;
    OSStatus err = CreatePopupButtonControl(
        MAC_WXHWND(parent->MacGetTopLevelWindowRef()) , &bounds , CFSTR("") ,
        -12345 , false /* no variable width */ , 0 , 0 , 0 , m_peer->GetControlRefAddr() );
    verify_noerr( err );

    m_macPopUpMenuHandle = NewUniqueMenu() ;
    m_peer->SetData<MenuHandle>( kControlNoPart , kControlPopupButtonMenuHandleTag , (MenuHandle) m_macPopUpMenuHandle ) ;
    m_peer->SetValueAndRange( n > 0 ? 1 : 0 , 0 , 0 );
    MacPostControlCreate( pos, size );

#if !wxUSE_STL
    if ( style & wxCB_SORT )
        // autosort
        m_strings = wxArrayString( 1 );
#endif

    for ( int i = 0; i < n; i++ )
    {
        Append( choices[i] );
    }

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
int wxChoice::DoAppend( const wxString& item )
{
#if wxUSE_STL
    wxArrayString::iterator insertPoint;
    unsigned int index;

    if (GetWindowStyle() & wxCB_SORT)
    {
        insertPoint = std::lower_bound( m_strings.begin(), m_strings.end(), item );
        index = insertPoint - m_strings.begin();
    }
    else
    {
        insertPoint = m_strings.end();
        index = m_strings.size();
    }

    m_strings.insert( insertPoint, item );
#else
    unsigned int index = m_strings.Add( item );
#endif

    m_datas.Insert( NULL , index );
    UMAInsertMenuItem( MAC_WXHMENU( m_macPopUpMenuHandle ), item, GetFont().GetEncoding(), index );
    DoSetItemClientData( index, NULL );
    m_peer->SetMaximum( GetCount() );

    return index;
}

int wxChoice::DoInsert( const wxString& item, unsigned int pos )
{
    wxCHECK_MSG( !(GetWindowStyle() & wxCB_SORT), -1, wxT("wxChoice::DoInsert: can't insert into sorted list") );
    wxCHECK_MSG( IsValidInsert(pos), -1, wxT("wxChoice::DoInsert: invalid index") );

    if (pos == GetCount())
        return DoAppend( item );

    UMAInsertMenuItem( MAC_WXHMENU( m_macPopUpMenuHandle ), item, GetFont().GetEncoding(), pos );
    m_strings.Insert( item, pos );
    m_datas.Insert( NULL, pos );
    DoSetItemClientData( pos, NULL );
    m_peer->SetMaximum( GetCount() );

    return pos;
}

void wxChoice::Delete(unsigned int n)
{
    wxCHECK_RET( IsValid(n) , wxT("wxChoice::Delete: invalid index") );

    if ( HasClientObjectData() )
        delete GetClientObject( n );

    ::DeleteMenuItem( MAC_WXHMENU(m_macPopUpMenuHandle) , n + 1 ) ;
    m_strings.RemoveAt( n ) ;
    m_datas.RemoveAt( n ) ;
    m_peer->SetMaximum( GetCount() ) ;
}

void wxChoice::Clear()
{
    FreeData();
    for ( unsigned int i = 0 ; i < GetCount() ; i++ )
    {
        ::DeleteMenuItem( MAC_WXHMENU(m_macPopUpMenuHandle) , 1 ) ;
    }

    m_strings.Empty() ;
    m_datas.Empty() ;
    m_peer->SetMaximum( 0 ) ;
}

void wxChoice::FreeData()
{
    if ( HasClientObjectData() )
    {
        unsigned int count = GetCount();
        for ( unsigned int n = 0; n < count; n++ )
        {
            delete GetClientObject( n );
        }
    }
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------
int wxChoice::GetSelection() const
{
    return m_peer->GetValue() - 1 ;
}

void wxChoice::SetSelection( int n )
{
    m_peer->SetValue( n + 1 ) ;
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
#if !wxUSE_STL
    // Avoid assert for non-default args passed to sorted array Index
    if ( HasFlag(wxCB_SORT) )
        bCase = true;
#endif

    return m_strings.Index( s , bCase ) ;
}

void wxChoice::SetString(unsigned int n, const wxString& s)
{
    wxCHECK_RET( IsValid(n), wxT("wxChoice::SetString(): invalid index") );

    m_strings[n] = s ;

    // apple menu pos is 1-based
    UMASetMenuItemText( MAC_WXHMENU(m_macPopUpMenuHandle) , n + 1 , s , wxFont::GetDefaultEncoding() ) ;
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
    wxCHECK_RET( IsValid(n), wxT("wxChoice::DoSetItemClientData: invalid index") );

    m_datas[n] = (char*)clientData ;
}

void * wxChoice::DoGetItemClientData(unsigned int n) const
{
    wxCHECK_MSG( IsValid(n), NULL, wxT("wxChoice::DoGetClientData: invalid index") );

    return (void *)m_datas[n];
}

void wxChoice::DoSetItemClientObject(unsigned int n, wxClientData* clientData)
{
    DoSetItemClientData(n, clientData);
}

wxClientData* wxChoice::DoGetItemClientObject(unsigned int n) const
{
    return (wxClientData*)DoGetItemClientData( n ) ;
}

wxInt32 wxChoice::MacControlHit( WXEVENTHANDLERREF WXUNUSED(handler) , WXEVENTREF WXUNUSED(event) )
{
    wxCommandEvent event( wxEVT_COMMAND_CHOICE_SELECTED, m_windowId );

    // actually n should be made sure by the os to be a valid selection, but ...
    int n = GetSelection();
    if ( n > -1 )
    {
        event.SetInt( n );
        event.SetString( GetStringSelection() );
        event.SetEventObject( this );

        if ( HasClientObjectData() )
            event.SetClientObject( GetClientObject( n ) );
        else if ( HasClientUntypedData() )
            event.SetClientData( GetClientData( n ) );

        ProcessCommand( event );
    }

    return noErr ;
}

wxSize wxChoice::DoGetBestSize() const
{
    int lbWidth = GetCount() > 0 ? 20 : 100;  // some defaults
    int lbHeight = 20;
    int wLine;

#if TARGET_CARBON
    SInt32 metric ;

    GetThemeMetric( kThemeMetricPopupButtonHeight , &metric );
    lbHeight = metric ;
#endif

    {
#if wxMAC_USE_CORE_GRAPHICS
        wxClientDC dc(const_cast<wxChoice*>(this));
#else
        wxMacPortStateHelper st( UMAGetWindowPort( (WindowRef) MacGetTopLevelWindowRef() ) ) ;
        if ( m_font.Ok() )
        {
            ::TextFont( m_font.MacGetFontNum() ) ;
            ::TextSize( m_font.MacGetFontSize() ) ;
            ::TextFace( m_font.MacGetFontStyle() ) ;
        }
        else
        {
            ::TextFont( kFontIDMonaco ) ;
            ::TextSize( 9 ) ;
            ::TextFace( 0 ) ;
        }
#endif
        // Find the widest line
        for(unsigned int i = 0; i < GetCount(); i++)
        {
            wxString str(GetString(i));
#if wxMAC_USE_CORE_GRAPHICS
            wxCoord width, height ;
            dc.GetTextExtent( str , &width, &height);
            wLine = width ;
#else
#if wxUSE_UNICODE
            Point bounds = { 0, 0 } ;
            SInt16 baseline ;

            ::GetThemeTextDimensions( wxMacCFStringHolder( str , GetFont().GetEncoding() ) ,
                kThemeCurrentPortFont,
                kThemeStateActive,
                false,
                &bounds,
                &baseline );

            wLine = bounds.h ;
#else
            wLine = ::TextWidth( str.c_str() , 0 , str.length() ) ;
#endif
#endif
            lbWidth = wxMax( lbWidth, wLine ) ;
        }

        // Add room for the popup arrow
        lbWidth += 2 * lbHeight ;
#if wxMAC_USE_CORE_GRAPHICS
        wxCoord width, height ;
        dc.GetTextExtent( wxT("X"), &width, &height);
        int cx = width ;
#else
        // And just a bit more
        int cx = ::TextWidth( "X" , 0 , 1 ) ;
#endif
        lbWidth += cx ;
    }

    return wxSize( lbWidth, lbHeight );
}

#endif // wxUSE_CHOICE

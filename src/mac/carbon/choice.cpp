/////////////////////////////////////////////////////////////////////////////
// Name:        choice.cpp
// Purpose:     wxChoice
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "choice.h"
#endif

#include "wx/defs.h"

#if wxUSE_CHOICE

#include "wx/choice.h"
#include "wx/menu.h"
#include "wx/mac/uma.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxChoice, wxControl)
#endif

extern MenuHandle NewUniqueMenu() ;

wxChoice::~wxChoice()
{
    if ( HasClientObjectData() )
    {
        size_t i, max = GetCount();

        for ( i = 0; i < max; ++i )
            delete GetClientObject(i);
    }

        // DeleteMenu( m_macPopUpMenuId ) ;
    // DisposeMenu( m_macPopUpMenuHandle ) ;
}

bool wxChoice::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
           const wxArrayString& choices,
           long style,
           const wxValidator& validator,
           const wxString& name)
{
    wxCArrayString chs(choices);

    return Create(parent, id, pos, size, chs.GetCount(), chs.GetStrings(),
                  style, validator, name);
}

bool wxChoice::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
           int n, const wxString choices[],
           long style,
           const wxValidator& validator,
           const wxString& name)
{
    m_macIsUserPane = false ;

    if ( !wxChoiceBase::Create(parent, id, pos, size, style, validator, name) )
        return false;

    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;

    m_peer = new wxMacControl() ;
    verify_noerr ( CreatePopupButtonControl( MAC_WXHWND(parent->MacGetTopLevelWindowRef()) , &bounds , CFSTR("") ,
        -12345 , false /* no variable width */ , 0 , 0 , 0 , m_peer->GetControlRefAddr() ) );


    m_macPopUpMenuHandle =  NewUniqueMenu() ;
    m_peer->SetData<MenuHandle>( kControlNoPart , kControlPopupButtonMenuHandleTag , (MenuHandle) m_macPopUpMenuHandle ) ;
    m_peer->SetValueAndRange( n > 0 ? 1 : 0 , 0 , 0 ) ;
    MacPostControlCreate(pos,size) ;
    // TODO wxCB_SORT
    for ( int i = 0; i < n; i++ )
    {
        Append(choices[i]);
    }
    SetBestSize(size);   // Needed because it is a wxControlWithItems
    return true;
}

// ----------------------------------------------------------------------------
// adding/deleting items to/from the list
// ----------------------------------------------------------------------------
int wxChoice::DoAppend(const wxString& item)
{
    UMAAppendMenuItem(MAC_WXHMENU( m_macPopUpMenuHandle ) , item, m_font.GetEncoding() );
    m_strings.Add( item ) ;
    m_datas.Add( NULL ) ;
    int index = m_strings.GetCount()  - 1  ;
    DoSetItemClientData( index , NULL ) ;
    m_peer->SetMaximum( GetCount() ) ;
    return index ;
}

int wxChoice::DoInsert(const wxString& item, int pos)
{
    wxCHECK_MSG(!(GetWindowStyle() & wxCB_SORT), -1, wxT("can't insert into sorted list"));
    wxCHECK_MSG((pos>=0) && (pos<=GetCount()), -1, wxT("invalid index"));

    if (pos == GetCount())
        return DoAppend(item);

    UMAAppendMenuItem(MAC_WXHMENU( m_macPopUpMenuHandle ) , item, m_font.GetEncoding() );
    m_strings.Insert( item, pos ) ;
    m_datas.Insert( NULL, pos ) ;
    DoSetItemClientData( pos , NULL ) ;
    m_peer->SetMaximum( GetCount() ) ;
    return pos ;
}

void wxChoice::Delete(int n)
{
    wxCHECK_RET( n < GetCount(), wxT("invalid item index in wxChoice::Delete") );
    if ( HasClientObjectData() )
    {
        delete GetClientObject(n);
    }
    ::DeleteMenuItem( MAC_WXHMENU(m_macPopUpMenuHandle) , n + 1) ;
    m_strings.RemoveAt( n ) ;
    m_datas.RemoveAt( n ) ;
    m_peer->SetMaximum( GetCount() ) ;
}

void wxChoice::Clear()
{
    FreeData();
    for ( int i = 0 ; i < GetCount() ; i++ )
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
        size_t count = GetCount();
        for ( size_t n = 0; n < count; n++ )
        {
            delete GetClientObject(n);
        }
    }
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------
int wxChoice::GetSelection() const
{
    return m_peer->GetValue() -1 ;
}

void wxChoice::SetSelection(int n)
{
    m_peer->SetValue( n + 1 ) ;
}

// ----------------------------------------------------------------------------
// string list functions
// ----------------------------------------------------------------------------

int wxChoice::GetCount() const
{
    return m_strings.GetCount() ;
}

int wxChoice::FindString(const wxString& s) const
{
    for( int i = 0 ; i < GetCount() ; i++ )
    {
        if ( GetString( i ).IsSameAs(s, false) )
            return i ;
    }
    return wxNOT_FOUND ;
}

void wxChoice::SetString(int n, const wxString& s)
{
    wxFAIL_MSG(wxT("wxChoice::SetString() not yet implemented"));
#if 0 // should do this, but no Insert() so far
    Delete(n);
    Insert(n + 1, s);
#endif
}

wxString wxChoice::GetString(int n) const
{
    wxCHECK_MSG( n >= 0 && (size_t)n < m_strings.GetCount(), _T(""),
                    _T("wxChoice::GetString(): invalid index") );

    return m_strings[n] ;
}

// ----------------------------------------------------------------------------
// client data
// ----------------------------------------------------------------------------
void wxChoice::DoSetItemClientData( int n, void* clientData )
{
    wxCHECK_RET( n >= 0 && (size_t)n < m_datas.GetCount(),
                 wxT("invalid index in wxChoice::SetClientData") );

    m_datas[n] = (char*) clientData ;
}

void *wxChoice::DoGetItemClientData(int n) const
{
    wxCHECK_MSG( n >= 0 && (size_t)n < m_datas.GetCount(), NULL,
                 wxT("invalid index in wxChoice::GetClientData") );
    return (void *)m_datas[n];
}

void wxChoice::DoSetItemClientObject( int n, wxClientData* clientData )
{
    DoSetItemClientData(n, clientData);
}

wxClientData* wxChoice::DoGetItemClientObject( int n ) const
{
    return (wxClientData *)DoGetItemClientData(n);
}

wxInt32 wxChoice::MacControlHit(WXEVENTHANDLERREF WXUNUSED(handler) , WXEVENTREF WXUNUSED(event) )
{
    wxCommandEvent event(wxEVT_COMMAND_CHOICE_SELECTED, m_windowId );
    int n = GetSelection();
    // actually n should be made sure by the os to be a valid selection, but ...
    if ( n > -1 )
    {
        event.SetInt( n );
        event.SetString(GetStringSelection());
        event.SetEventObject(this);
        if ( HasClientObjectData() )
            event.SetClientObject( GetClientObject(n) );
        else if ( HasClientUntypedData() )
            event.SetClientData( GetClientData(n) );
        ProcessCommand(event);
    }
    return noErr ;
}

wxSize wxChoice::DoGetBestSize() const
{
    int lbWidth = GetCount() > 0 ? 20 : 100;  // some defaults
    int lbHeight = 20;
    int wLine;
#if TARGET_CARBON
    long metric ;
    GetThemeMetric(kThemeMetricPopupButtonHeight , &metric );
    lbHeight = metric ;
#endif
    {
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
            ::TextSize( 9  );
            ::TextFace( 0 ) ;
        }
        // Find the widest line
        for(int i = 0; i < GetCount(); i++) {
            wxString str(GetString(i));
        #if wxUSE_UNICODE
            Point bounds={0,0} ;
            SInt16 baseline ;
            ::GetThemeTextDimensions( wxMacCFStringHolder( str , m_font.GetEncoding() ) ,
                kThemeCurrentPortFont,
                kThemeStateActive,
                false,
                &bounds,
                &baseline );
            wLine = bounds.h ;
        #else
            wLine = ::TextWidth( str.c_str() , 0 , str.Length() ) ;
        #endif
            lbWidth = wxMax(lbWidth, wLine);
        }
        // Add room for the popup arrow
        lbWidth += 2 * lbHeight ;
        // And just a bit more
        int cx = ::TextWidth( "X" , 0 , 1 ) ;
        lbWidth += cx ;

    }
    return wxSize(lbWidth, lbHeight);
}

#endif // wxUSE_CHOICE

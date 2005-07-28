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

IMPLEMENT_DYNAMIC_CLASS(wxChoice, wxControl)

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
    if ( !wxChoiceBase::Create(parent, id, pos, size, style, validator, name) )
        return false;

    Rect bounds ;
    Str255 title ;

    MacPreControlCreate( parent , id ,  wxEmptyString , pos , size ,style, validator , name , &bounds , title ) ;
    m_macControl = (WXWidget) ::NewControl( MAC_WXHWND(parent->MacGetRootWindow()) , &bounds , title , false , 0 , -12345 , 0 ,
        kControlPopupButtonProc + kControlPopupFixedWidthVariant , (long) this ) ;

    m_macPopUpMenuHandle =  NewUniqueMenu() ;
    SetControlData( (ControlHandle) m_macControl , kControlNoPart , kControlPopupButtonMenuHandleTag , sizeof( MenuHandle ) , (char*) &m_macPopUpMenuHandle) ;
    SetControl32BitMinimum( (ControlHandle) m_macControl , 0 ) ;
    SetControl32BitMaximum( (ControlHandle) m_macControl , 0) ;
    if ( n > 0 )
        SetControl32BitValue( (ControlHandle) m_macControl , 1 ) ;
    MacPostControlCreate() ;
    // TODO wxCB_SORT
    for ( int i = 0; i < n; i++ )
    {
        Append(choices[i]);
    }
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
    SetControl32BitMaximum( (ControlHandle) m_macControl , GetCount()) ;
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
    SetControl32BitMaximum( (ControlHandle) m_macControl , pos) ;
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
    SetControl32BitMaximum( (ControlHandle) m_macControl , GetCount()) ;
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
    SetControl32BitMaximum( (ControlHandle) m_macControl , 0 ) ;
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
    return GetControl32BitValue( (ControlHandle) m_macControl ) -1 ;
}

void wxChoice::SetSelection(int n)
{
    SetControl32BitValue( (ControlHandle) m_macControl , n + 1 ) ;
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

void wxChoice::MacHandleControlClick( WXWidget control , wxInt16 controlpart , bool WXUNUSED(mouseStillDown))
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
        wxMacPortStateHelper st( UMAGetWindowPort( (WindowRef) MacGetRootWindow() ) ) ;
        if ( m_font.Ok() )
        {
            ::TextFont( m_font.GetMacFontNum() ) ;
            ::TextSize( m_font.GetMacFontSize() ) ;
            ::TextFace( m_font.GetMacFontStyle() ) ;
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

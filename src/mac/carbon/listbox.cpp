///////////////////////////////////////////////////////////////////////////////
// Name:        listbox.cpp
// Purpose:     wxListBox
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "listbox.h"
#endif

#include "wx/app.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/settings.h"
#include "wx/toplevel.h"
#include "wx/dynarray.h"
#include "wx/log.h"

#include "wx/utils.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxControl)

BEGIN_EVENT_TABLE(wxListBox, wxControl)
#if !__WXMAC_OSX__
    EVT_SIZE( wxListBox::OnSize )
    EVT_CHAR( wxListBox::OnChar )
#endif
END_EVENT_TABLE()
#endif

#include "wx/mac/uma.h"

const short kTextColumnId = 1024 ;

// new databrowserbased version
// because of the limited insert
// functionality of DataBrowser, 
// we just introduce id s corresponding
// to the line number

// Listbox item
wxListBox::wxListBox()
{
  m_noItems = 0;
  m_selected = 0;
  m_macList = NULL ;
}

bool wxListBox::Create(wxWindow *parent, wxWindowID id,
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

#if TARGET_API_MAC_OSX
static pascal void DataBrowserItemNotificationProc(ControlRef browser, DataBrowserItemID itemID, 
    DataBrowserItemNotification message, DataBrowserItemDataRef itemData)
#else
static pascal  void DataBrowserItemNotificationProc(ControlRef browser, DataBrowserItemID itemID, 
    DataBrowserItemNotification message)
#endif
{
    long ref = GetControlReference( browser ) ;
    if ( ref )
    {
        wxListBox* list = wxDynamicCast( (wxObject*) ref , wxListBox ) ;
        int i = itemID - 1 ;
        if (i >= 0 && i < list->GetCount() )
        {
            bool trigger = false ;
            wxCommandEvent event(
                wxEVT_COMMAND_LISTBOX_SELECTED, list->GetId() );
            switch( message )
            {
                case kDataBrowserItemDeselected :
                    if ( list->HasMultipleSelection() )
                        trigger = true ;
                    break ;
                case kDataBrowserItemSelected :
                    trigger = true ;
                    break ;
                case kDataBrowserItemDoubleClicked :
                    event.SetEventType(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED) ;
                    trigger = true ;
                    break ;
                default :
                    break ;
            }
            if ( trigger )
            {
                event.SetEventObject( list );
                if ( list->HasClientObjectData() )
                    event.SetClientObject( list->GetClientObject(i) );
                else if ( list->HasClientUntypedData() )
                    event.SetClientData( list->GetClientData(i) );
                event.SetString( list->GetString(i) );
                event.SetInt(i) ;
                event.SetExtraLong( list->HasMultipleSelection() ? message == kDataBrowserItemSelected : TRUE );
                wxPostEvent( list->GetEventHandler() , event ) ;
                // direct notification is not always having the listbox GetSelection() having in synch with event
                // list->GetEventHandler()->ProcessEvent(event) ; 
            } 
        }
    }
}


static pascal OSStatus ListBoxGetSetItemData(ControlRef browser, 
    DataBrowserItemID itemID, DataBrowserPropertyID property, 
    DataBrowserItemDataRef itemData, Boolean changeValue)
{
	OSStatus err = errDataBrowserPropertyNotSupported;
	
	if ( ! changeValue )
	{ 	
    	switch (property)
    	{
    		
    	    case kTextColumnId:
    		{	
    		    long ref = GetControlReference( browser ) ;
    		    if ( ref )
    		    {
    		        wxListBox* list = wxDynamicCast( (wxObject*) ref , wxListBox ) ;
                    int i = itemID - 1 ;
                    if (i >= 0 && i < list->GetCount() )
		            {
		                wxMacCFStringHolder cf( list->GetString(i) , list->GetFont().GetEncoding() ) ;
		                verify_noerr( ::SetDataBrowserItemDataText( itemData , cf ) ) ;
		                err = noErr ;
		            }
    			}
    		}	
    		break;
    				
    		default:
    		
    		break;
    	}
	}
	
	return err;
}
bool wxListBox::Create(wxWindow *parent, wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       int n, const wxString choices[],
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    m_macIsUserPane = FALSE ;

    wxASSERT_MSG( !(style & wxLB_MULTIPLE) || !(style & wxLB_EXTENDED),
                  _T("only one of listbox selection modes can be specified") );
    
    if ( !wxListBoxBase::Create(parent, id, pos, size, style & ~(wxHSCROLL|wxVSCROLL), validator, name) )
        return false;

    m_noItems = 0 ; // this will be increased by our append command
    m_selected = 0;

    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;

    m_peer = new wxMacControl() ;
    verify_noerr( ::CreateDataBrowserControl( MAC_WXHWND(parent->MacGetTopLevelWindowRef()), &bounds, kDataBrowserListView , m_peer->GetControlRefAddr() ) );

    DataBrowserSelectionFlags  options = kDataBrowserDragSelect ;
    if ( style & wxLB_MULTIPLE )
    {
        options += kDataBrowserAlwaysExtendSelection + kDataBrowserCmdTogglesSelection  ;
    }
    else if ( style & wxLB_EXTENDED )
    {
        // default behaviour
    }
    else
    {
        options += kDataBrowserSelectOnlyOne ;
    }
    verify_noerr(m_peer->SetSelectionFlags( options ) ); 

    DataBrowserListViewColumnDesc columnDesc ;
    columnDesc.headerBtnDesc.titleOffset = 0;
	columnDesc.headerBtnDesc.version = kDataBrowserListViewLatestHeaderDesc;
		
	columnDesc.headerBtnDesc.btnFontStyle.flags	= 
		kControlUseFontMask | kControlUseJustMask;
	
	columnDesc.headerBtnDesc.btnContentInfo.contentType = kControlNoContent;
	columnDesc.propertyDesc.propertyType = kDataBrowserTextType;
	columnDesc.headerBtnDesc.btnFontStyle.just = teFlushDefault;
	columnDesc.headerBtnDesc.minimumWidth = 0;
	columnDesc.headerBtnDesc.maximumWidth = 10000;
	
	columnDesc.headerBtnDesc.btnFontStyle.font = kControlFontViewSystemFont;
	columnDesc.headerBtnDesc.btnFontStyle.style = normal;
	columnDesc.headerBtnDesc.titleString = NULL ; // CFSTR( "" );

	columnDesc.propertyDesc.propertyID = kTextColumnId;
	columnDesc.propertyDesc.propertyType = kDataBrowserTextType;
	columnDesc.propertyDesc.propertyFlags =
#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2
	 kDataBrowserListViewTypeSelectColumn | 
#endif
	 kDataBrowserTableViewSelectionColumn ;

	
	verify_noerr(m_peer->AddListViewColumn( &columnDesc, kDataBrowserListViewAppendColumn) ) ;
    verify_noerr(m_peer->AutoSizeListViewColumns() ) ;
    verify_noerr(m_peer->SetHasScrollBars(false , true ) ) ;
    verify_noerr(m_peer->SetTableViewHiliteStyle(kDataBrowserTableViewFillHilite  ) ) ;
    verify_noerr(m_peer->SetListViewHeaderBtnHeight( 0 ) ) ;
    DataBrowserCallbacks callbacks ;
    
    callbacks.version = kDataBrowserLatestCallbacks;
    
    InitDataBrowserCallbacks(&callbacks);
    
    callbacks.u.v1.itemDataCallback = 
        NewDataBrowserItemDataUPP(ListBoxGetSetItemData);
	
	callbacks.u.v1.itemNotificationCallback =
#if TARGET_API_MAC_OSX
	    (DataBrowserItemNotificationUPP) NewDataBrowserItemNotificationWithItemUPP(DataBrowserItemNotificationProc) ;
#else
	    NewDataBrowserItemNotificationUPP(DataBrowserItemNotificationProc) ;
#endif
    m_peer->SetCallbacks( &callbacks);

#if TARGET_API_MAC_OSX
    // there is a redraw bug in 10.2.X
    if ( UMAGetSystemVersion() < 0x1030 )
        m_peer->SetData( kControlNoPart, kControlDataBrowserIncludesFrameAndFocusTag, (Boolean) false ) ;
#endif

    MacPostControlCreate(pos,size) ;

    for ( int i = 0 ; i < n ; i++ )
    {
        Append( choices[i] ) ;
    }

    SetBestSize(size);   // Needed because it is a wxControlWithItems
    
    return TRUE;
}

wxListBox::~wxListBox()
{
    m_peer->SetReference( NULL ) ;
    FreeData() ;
    // avoid access during destruction
    if ( m_macList )
    {
        m_macList = NULL ;
    }
}

void wxListBox::FreeData()
{
#if wxUSE_OWNER_DRAWN
    if ( m_windowStyle & wxLB_OWNERDRAW )
    {
        size_t uiCount = m_aItems.Count();
        while ( uiCount-- != 0 ) {
            delete m_aItems[uiCount];
            m_aItems[uiCount] = NULL;
        }

        m_aItems.Clear();
    }
    else
#endif // wxUSE_OWNER_DRAWN
    if ( HasClientObjectData() )
    {
        for ( size_t n = 0; n < (size_t)m_noItems; n++ )
        {
            delete GetClientObject(n);
        }
    }
}

void  wxListBox::DoSetSize(int x, int y,
            int width, int height,
            int sizeFlags )
{
    wxControl::DoSetSize( x , y , width , height , sizeFlags ) ;
}

void wxListBox::DoSetFirstItem(int N)
{
    MacScrollTo( N ) ;
}

void wxListBox::Delete(int N)
{
    wxCHECK_RET( N >= 0 && N < m_noItems,
                 wxT("invalid index in wxListBox::Delete") );

#if wxUSE_OWNER_DRAWN
    delete m_aItems[N];
    m_aItems.RemoveAt(N);
#else // !wxUSE_OWNER_DRAWN
    if ( HasClientObjectData() )
    {
        delete GetClientObject(N);
    }
#endif // wxUSE_OWNER_DRAWN/!wxUSE_OWNER_DRAWN
    m_stringArray.RemoveAt( N ) ;
    m_dataArray.RemoveAt( N ) ;
    m_noItems --;

    MacDelete( N ) ;
}

int wxListBox::DoAppend(const wxString& item)
{
    InvalidateBestSize();

    int index = m_noItems ;
    m_stringArray.Add( item ) ;
    m_dataArray.Add( NULL );
    m_noItems ++;
    DoSetItemClientData( index , NULL ) ;
    MacAppend( item ) ;

    return index ;
}

void wxListBox::DoSetItems(const wxArrayString& choices, void** clientData)
{
    Clear() ;
    int n = choices.GetCount();
    
    for( int i = 0 ; i < n ; ++i )
    {
        if ( clientData )
        {
#if wxUSE_OWNER_DRAWN
            wxASSERT_MSG(clientData[i] == NULL,
                wxT("Can't use client data with owner-drawn listboxes"));
#else // !wxUSE_OWNER_DRAWN
            Append( choices[i] , clientData[i] ) ;
#endif
        }
        else
            Append( choices[i] ) ;
    }
    
#if wxUSE_OWNER_DRAWN
    if ( m_windowStyle & wxLB_OWNERDRAW ) {
        // first delete old items
        size_t ui = m_aItems.Count();
        while ( ui-- != 0 ) {
            delete m_aItems[ui];
            m_aItems[ui] = NULL;
        }
        m_aItems.Empty();
        
        // then create new ones
        for ( ui = 0; ui < (size_t)m_noItems; ui++ ) {
            wxOwnerDrawn *pNewItem = CreateItem(ui);
            pNewItem->SetName(choices[ui]);
            m_aItems.Add(pNewItem);
        }
    }
#endif // wxUSE_OWNER_DRAWN
}

int wxListBox::FindString(const wxString& s) const
{
    
    if ( s.Right(1) == wxT("*") )
    {
        wxString search = s.Left( s.Length() - 1 ) ;
        int len = search.Length() ;
        Str255 s1 , s2 ;
        wxMacStringToPascal( search , s2 ) ;
        
        for ( int i = 0 ; i < m_noItems ; ++ i )
        {
        	wxMacStringToPascal( m_stringArray[i].Left( len ) , s1 ) ;

            if ( EqualString( s1 , s2 , false , false ) )
                return i ;
        }
        if ( s.Left(1) == wxT("*") && s.Length() > 1 )
        {
            wxString st = s ;
            st.MakeLower() ;
            for ( int i = 0 ; i < m_noItems ; ++i )
            {
                if ( GetString(i).Lower().Matches(st) )
                    return i ;
            }
        }
        
    }
    else
    {
        Str255 s1 , s2 ;
        
        wxMacStringToPascal( s , s2 ) ;
        
        for ( int i = 0 ; i < m_noItems ; ++ i )
        {
        	wxMacStringToPascal( m_stringArray[i] , s1 ) ;

            if ( EqualString( s1 , s2 , false , false ) )
                return i ;
        }
    }
    return -1;
}

void wxListBox::Clear()
{
    FreeData();
    m_noItems = 0;
    m_stringArray.Empty() ;
    m_dataArray.Empty() ;
    MacClear() ;
}

void wxListBox::SetSelection(int N, bool select)
{
    wxCHECK_RET( N >= 0 && N < m_noItems,
        wxT("invalid index in wxListBox::SetSelection") );
    MacSetSelection( N , select ) ;
    GetSelections( m_selectionPreImage ) ;
}

bool wxListBox::IsSelected(int N) const
{
    wxCHECK_MSG( N >= 0 && N < m_noItems, FALSE,
        wxT("invalid index in wxListBox::Selected") );
    
    return MacIsSelected( N ) ;
}

void *wxListBox::DoGetItemClientData(int N) const
{
    wxCHECK_MSG( N >= 0 && N < m_noItems, NULL,
        wxT("invalid index in wxListBox::GetClientData"));
    
    return (void *)m_dataArray[N];
}

wxClientData *wxListBox::DoGetItemClientObject(int N) const
{
    return (wxClientData *) DoGetItemClientData( N ) ;
}

void wxListBox::DoSetItemClientData(int N, void *Client_data)
{
    wxCHECK_RET( N >= 0 && N < m_noItems,
        wxT("invalid index in wxListBox::SetClientData") );
    
#if wxUSE_OWNER_DRAWN
    if ( m_windowStyle & wxLB_OWNERDRAW )
    {
        // client data must be pointer to wxOwnerDrawn, otherwise we would crash
        // in OnMeasure/OnDraw.
        wxFAIL_MSG(wxT("Can't use client data with owner-drawn listboxes"));
    }
#endif // wxUSE_OWNER_DRAWN
    wxASSERT_MSG( m_dataArray.GetCount() >= (size_t) N , wxT("invalid client_data array") ) ;
    
    if ( m_dataArray.GetCount() > (size_t) N )
    {
        m_dataArray[N] = (char*) Client_data ;
    }
    else
    {
        m_dataArray.Add( (char*) Client_data ) ;
    }
}

void wxListBox::DoSetItemClientObject(int n, wxClientData* clientData)
{
    DoSetItemClientData(n, clientData);
}

// Return number of selections and an array of selected integers
int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    return MacGetSelections( aSelections ) ;
}

// Get single selection, for single choice list items
int wxListBox::GetSelection() const
{
    return MacGetSelection() ;
}

// Find string for position
wxString wxListBox::GetString(int N) const
{
	return m_stringArray[N]  ;
}

void wxListBox::DoInsertItems(const wxArrayString& items, int pos)
{
    wxCHECK_RET( pos >= 0 && pos <= m_noItems,
        wxT("invalid index in wxListBox::InsertItems") );
    
    InvalidateBestSize();

    int nItems = items.GetCount();
    
    for ( int i = 0 ; i < nItems ; i++ )
    {
        m_stringArray.Insert( items[i] , pos + i ) ;
        m_dataArray.Insert( NULL , pos + i ) ;
        m_noItems++ ;
        MacInsert( pos + i , items[i] ) ;
    }
}

void wxListBox::SetString(int N, const wxString& s)
{
    m_stringArray[N] = s ;
    MacSet( N , s ) ;
}

wxSize wxListBox::DoGetBestSize() const
{
    int lbWidth = 100;  // some defaults
    int lbHeight = 110;
    int wLine;

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
        
        // Add room for the scrollbar
        lbWidth += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
        
        // And just a bit more
        int cy = 12 ;
        int cx = ::TextWidth( "X" , 0 , 1 ) ;
        lbWidth += cx ;
        
        // don't make the listbox too tall (limit height to around 10 items) but don't
        // make it too small neither
        lbHeight = (cy+4) * wxMin(wxMax(GetCount(), 3), 10);
    }

    return wxSize(lbWidth, lbHeight);
}

int wxListBox::GetCount() const
{
    return m_noItems;
}

void wxListBox::Refresh(bool eraseBack, const wxRect *rect)
{
    wxControl::Refresh( eraseBack , rect ) ;
    //    MacRedrawControl() ;
}

#if wxUSE_OWNER_DRAWN

class wxListBoxItem : public wxOwnerDrawn
{
public:
    wxListBoxItem(const wxString& str = "");
};

wxListBoxItem::wxListBoxItem(const wxString& str) : wxOwnerDrawn(str, FALSE)
{
    // no bitmaps/checkmarks
    SetMarginWidth(0);
}

wxOwnerDrawn *wxListBox::CreateItem(size_t n)
{
    return new wxListBoxItem();
}

#endif  //USE_OWNER_DRAWN


// Some custom controls depend on this
/* static */ wxVisualAttributes
wxListBox::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    wxVisualAttributes attr;
    attr.colFg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    attr.colBg = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX);
    attr.font  = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    return attr;
}

// ============================================================================
// list box control implementation
// ============================================================================

void wxListBox::MacDelete( int N )
{
    UInt32 id = m_noItems+1 ;
    verify_noerr( m_peer->RemoveItems( kDataBrowserNoItem , 1 , (UInt32*) &id , kDataBrowserItemNoProperty ) ) ;
    verify_noerr( m_peer->UpdateItems( kDataBrowserNoItem , 1 , (UInt32*) kDataBrowserNoItem , kDataBrowserItemNoProperty , kDataBrowserItemNoProperty ) ) ;
}

void wxListBox::MacInsert( int n , const wxString& text)
{
    UInt32 id = m_noItems ; // this has already been increased
    verify_noerr( m_peer->AddItems( kDataBrowserNoItem , 1 ,  (UInt32*) &id , kDataBrowserItemNoProperty ) ) ;
    verify_noerr( m_peer->UpdateItems( kDataBrowserNoItem , 1 , (UInt32*) kDataBrowserNoItem , kDataBrowserItemNoProperty , kDataBrowserItemNoProperty ) ) ;
}

void wxListBox::MacAppend( const wxString& text)
{
    UInt32 id = m_noItems ; // this has already been increased
    verify_noerr( m_peer->AddItems( kDataBrowserNoItem , 1 ,  (UInt32*) &id , kDataBrowserItemNoProperty ) ) ;
}

void wxListBox::MacClear()
{
    verify_noerr( m_peer->RemoveItems( kDataBrowserNoItem , 0 , NULL , kDataBrowserItemNoProperty ) ) ;
}

void wxListBox::MacSetSelection( int n , bool select )
{
    UInt32 id = n + 1 ;
    if ( !(GetWindowStyle() & (wxLB_MULTIPLE|wxLB_EXTENDED) ) )
    {
        int n = MacGetSelection() ;
        if ( n >= 0 )
        {
            UInt32 idOld = n + 1 ;
            m_peer->SetSelectedItems( 1 , & idOld , kDataBrowserItemsRemove ) ;
        }
    }
    if ( m_peer->IsItemSelected( id ) != select )
    {
        verify_noerr(m_peer->SetSelectedItems( 1 , & id , kDataBrowserItemsToggle ) ) ;
    }
    MacScrollTo( n ) ;
}

bool wxListBox::MacIsSelected( int n ) const
{
    return m_peer->IsItemSelected( n + 1 ) ;
}

int wxListBox::MacGetSelection() const
{
    for ( int i = 0 ; i < GetCount() ; ++i )
    {
        if ( m_peer->IsItemSelected( i + 1 ) )
        {
            return i ;
        }
    }
        return -1 ;
}

int wxListBox::MacGetSelections( wxArrayInt& aSelections ) const
{
    int no_sel = 0 ;
    
    aSelections.Empty();
    for ( int i = 0 ; i < GetCount() ; ++i )
    {
        if ( m_peer->IsItemSelected( i + 1 ) )
        {
            aSelections.Add( i ) ;
            no_sel++ ;
        }
    }
    return no_sel ;
}

void wxListBox::MacSet( int n , const wxString& text )
{
    // as we don't store the strings we only have to issue a redraw
    UInt32 id = n + 1 ;
    verify_noerr( m_peer->UpdateItems( kDataBrowserNoItem , 1 , &id , kDataBrowserItemNoProperty , kDataBrowserItemNoProperty ) ) ;
}

void wxListBox::MacScrollTo( int n )
{
    UInt32 id = n + 1 ;
    verify_noerr(  m_peer->RevealItem( id , kTextColumnId , kDataBrowserRevealWithoutSelecting ) ) ;
}

#if !TARGET_API_MAC_OSX
void wxListBox::OnSize( wxSizeEvent &event)
{
}
#endif

void wxListBox::MacSetRedraw( bool doDraw )
{
    // nothing to do in compositing mode
}

void wxListBox::MacDoClick()
{/*
    wxArrayInt aSelections;
    int n ;
    size_t count = GetSelections(aSelections);
    
    if ( count == m_selectionPreImage.GetCount() )
    {
        bool hasChanged = false ;
        for ( size_t i = 0 ; i < count ; ++i )
        {
            if ( aSelections[i] != m_selectionPreImage[i] )
            {
                hasChanged = true ;
                break ;
            }
        }
        if ( !hasChanged )
        {
            return ;
        }
    }
    
    m_selectionPreImage = aSelections;
    
    wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, m_windowId);
    event.SetEventObject( this );
    
    if ( count > 0 )
    {
        n = aSelections[0];
        if ( HasClientObjectData() )
            event.SetClientObject( GetClientObject(n) );
        else if ( HasClientUntypedData() )
            event.SetClientData( GetClientData(n) );
        event.SetString( GetString(n) );
    }
    else
    {
        n = -1;
    }
    
    event.m_commandInt = n;
    
    GetEventHandler()->ProcessEvent(event);
*/
}

void wxListBox::MacDoDoubleClick()
{
/*
    wxCommandEvent event(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, m_windowId);
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent(event) ;
*/
}

#if !TARGET_API_MAC_OSX

void wxListBox::OnChar(wxKeyEvent& event)
{
    // todo trigger proper events here
    event.Skip() ;
    return ;
    
    if ( event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER)
    {
        wxWindow* parent = GetParent() ;
        while( parent  && !parent->IsTopLevel() && parent->GetDefaultItem() == NULL )
            parent = parent->GetParent() ;
        
        if ( parent && parent->GetDefaultItem() )
        {
            wxButton *def = wxDynamicCast(parent->GetDefaultItem(),
                wxButton);
            if ( def && def->IsEnabled() )
            {
                wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, def->GetId() );
                event.SetEventObject(def);
                def->Command(event);
                return ;
            }
        }
        event.Skip() ;
    }
    /* generate wxID_CANCEL if command-. or <esc> has been pressed (typically in dialogs) */
    else if (event.GetKeyCode() == WXK_ESCAPE || (event.GetKeyCode() == '.' && event.MetaDown() ) )
    {
    	// FIXME: look in ancestors, not just parent.
        wxWindow* win = GetParent()->FindWindow( wxID_CANCEL ) ;
        if (win)
        {
        	wxCommandEvent new_event(wxEVT_COMMAND_BUTTON_CLICKED,wxID_CANCEL);
        	new_event.SetEventObject( win );
        	win->GetEventHandler()->ProcessEvent( new_event );
      	}
    }
    else if ( event.GetKeyCode() == WXK_TAB )
    {
        wxNavigationKeyEvent new_event;
        new_event.SetEventObject( this );
        new_event.SetDirection( !event.ShiftDown() );
        /* CTRL-TAB changes the (parent) window, i.e. switch notebook page */
        new_event.SetWindowChange( event.ControlDown() );
        new_event.SetCurrentFocus( this );
        if ( !GetEventHandler()->ProcessEvent( new_event ) )
            event.Skip() ;
    }
    else if ( event.GetKeyCode() == WXK_DOWN || event.GetKeyCode() == WXK_UP )
    {
        // perform the default key handling first
        wxControl::OnKeyDown( event ) ;
        
        wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, m_windowId);
        event.SetEventObject( this );
        
        wxArrayInt aSelections;
        int n, count = GetSelections(aSelections);
        if ( count > 0 )
        {
            n = aSelections[0];
            if ( HasClientObjectData() )
                event.SetClientObject( GetClientObject(n) );
            else if ( HasClientUntypedData() )
                event.SetClientData( GetClientData(n) );
            event.SetString( GetString(n) );
        }
        else
        {
            n = -1;
        }
        
        event.m_commandInt = n;
        
        GetEventHandler()->ProcessEvent(event);
    }
    else
    {
        if ( event.GetTimestamp() > m_lastTypeIn + 60 )
        {
            m_typeIn = wxEmptyString ;
        }
        m_lastTypeIn = event.GetTimestamp() ;
        m_typeIn += (char) event.GetKeyCode() ;
        int line = FindString(wxT("*")+m_typeIn+wxT("*")) ;
        if ( line >= 0 )
        {
            if ( GetSelection() != line )
            {
                SetSelection(line) ;
                wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, m_windowId);
                event.SetEventObject( this );
                
                if ( HasClientObjectData() )
                    event.SetClientObject( GetClientObject( line ) );
                else if ( HasClientUntypedData() )
                    event.SetClientData( GetClientData(line) );
                event.SetString( GetString(line) );
                
                event.m_commandInt = line ;
                
                GetEventHandler()->ProcessEvent(event);
            }
        }
    }
}

#endif


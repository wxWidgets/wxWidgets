///////////////////////////////////////////////////////////////////////////////
// Name:        listbox.cpp
// Purpose:     wxListBox
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "listbox.h"
#endif

#include "wx/app.h"
#include "wx/listbox.h"
#include "wx/settings.h"
#include "wx/dynarray.h"
#include "wx/log.h"

#include "wx/utils.h"
#ifdef __UNIX__
  #include "ldef/extldef.h"
#else
  #include "extldef.h"
#endif

#if !USE_SHARED_LIBRARY
  IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxControl)

BEGIN_EVENT_TABLE(wxListBox, wxControl)
	EVT_SIZE( wxListBox::OnSize ) 
END_EVENT_TABLE()
#endif

#include <wx/mac/uma.h>

extern "C" void MacDrawStringCell(Rect *cellRect, Cell lCell, ListHandle theList, long refCon) ;
const short kwxMacListWithVerticalScrollbar = 128 ;

// ============================================================================
// list box control implementation
// ============================================================================

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
                       int n, const wxString choices[],
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
  m_noItems = 0 ; // this will be increased by our append command
  m_selected = 0;

	Rect bounds ;
	Str255 title ;
	
	MacPreControlCreate( parent , id ,  "" , pos , size ,style, validator , name , &bounds , title ) ;

	m_macControl = UMANewControl( parent->GetMacRootWindow() , &bounds , title , true , kwxMacListWithVerticalScrollbar , 0 , 0, 
	  	kControlListBoxProc , (long) this ) ;
	
	long	result ;
	UMAGetControlData( m_macControl , kControlNoPart , kControlListBoxListHandleTag , sizeof( ListHandle ) , (char*) &m_macList  , &result ) ;

	HLock( (Handle) m_macList ) ;
	NewExtLDEFInfo( m_macList , MacDrawStringCell , (long) this ) ;
	(**m_macList).selFlags = 0 ;
	if ( style & wxLB_MULTIPLE )
	{
		(**m_macList).selFlags += lNoExtend ;
	}
	else if ( style & wxLB_EXTENDED )
	{
		(**m_macList).selFlags += lExtendDrag ;
	}
	else
	{
		(**m_macList).selFlags = lOnlyOne ;
	}
	Point pt = (**m_macList).cellSize ;
	pt.v = 14 ;
	LCellSize( pt , m_macList ) ;

	LAddColumn( 1 , 0 , m_macList ) ;

	MacPostControlCreate() ;

	ControlFontStyleRec		controlstyle ;
	controlstyle.flags = kControlUseFontMask + kControlUseSizeMask ;
	//controlstyle.font = kControlFontSmallSystemFont ;
	controlstyle.font = kFontIDMonaco ;
	controlstyle.size = 9 ;
	::UMASetControlFontStyle( m_macControl , &controlstyle ) ;

	for ( int i = 0 ; i < n ; i++ )
	{
		Append( choices[i] ) ;
	}

	LSetDrawingMode( true , m_macList ) ;

  return TRUE;
}

wxListBox::~wxListBox()
{
	Free() ;
	if ( m_macList )
	{
		DisposeExtLDEFInfo( m_macList ) ;
		m_macList = NULL ;
	}
}

void wxListBox::Free()
{
#if wxUSE_OWNER_DRAWN
    if ( m_windowStyle & wxLB_OWNERDRAW )
    {
        size_t uiCount = m_aItems.Count();
        while ( uiCount-- != 0 ) {
            delete m_aItems[uiCount];
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
    m_aItems.Remove(N);
#else // !wxUSE_OWNER_DRAWN
    if ( HasClientObjectData() )
    {
        delete GetClientObject(N);
    }
#endif // wxUSE_OWNER_DRAWN/!wxUSE_OWNER_DRAWN
	m_stringArray.Remove( N ) ;
	m_dataArray.Remove( N ) ;
	m_noItems --;
	
	MacDelete( N ) ;
}

int wxListBox::DoAppend(const wxString& item)
{
	int index = m_noItems ;
	if( wxApp::s_macDefaultEncodingIsPC )
	{
		m_stringArray.Add( wxMacMakeMacStringFromPC( item ) ) ;
		m_dataArray.Add( NULL );
	}
	else {
		m_stringArray.Add( item ) ;
		m_dataArray.Add( NULL );
	}
 	m_noItems ++;
	DoSetItemClientData( index , NULL ) ;
	MacAppend( item ) ;

	return index ;
}

void wxListBox::DoSetItems(const wxArrayString& choices, void** clientData)
{ 
  MacSetRedraw( false ) ;
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
  MacSetRedraw( true ) ;
}

bool wxListBox::HasMultipleSelection() const
{
    return (m_windowStyle & wxLB_MULTIPLE) || (m_windowStyle & wxLB_EXTENDED);
}

int wxListBox::FindString(const wxString& st) const
{
	wxString s ;
	if( wxApp::s_macDefaultEncodingIsPC )
	{
		s = wxMacMakeMacStringFromPC( st ) ;
	}
	else
		s = st ;
		
	if ( s.Right(1) == "*" )
	{
		wxString search = s.Left( s.Length() - 1 ) ;
		int len = search.Length() ;
		Str255 s1 , s2 ;

#if TARGET_CARBON
		c2pstrcpy( (StringPtr) s2 , search.c_str() ) ;
#else
		strcpy( (char *) s2 , search.c_str() ) ;
		c2pstr( (char *) s2 ) ;
#endif

    	for ( int i = 0 ; i < m_noItems ; ++ i )
	    {
#if TARGET_CARBON
	    	c2pstrcpy( (StringPtr) s1 , m_stringArray[i].Left( len ).c_str() ) ;
#else
	    	strcpy( (char *) s1 , m_stringArray[i].Left( len ).c_str() ) ;
			c2pstr( (char *) s1 ) ;
#endif
	    	if ( EqualString( s1 , s2 , false , false ) )
	    		return i ;
	    }
	    if ( s.Left(1) = "*" && s.Length() > 1 )
	    {
	    	s.MakeLower() ;
		    for ( int i = 0 ; i < m_noItems ; ++i )
			{
				if ( GetString(i).Lower().Matches(s) )
					return i ;
			}
		}	

	}
	else
	{
    	Str255 s1 , s2 ;

#if TARGET_CARBON
		c2pstrcpy( (StringPtr) s2 , s.c_str() ) ;
#else
		strcpy( (char *) s2 , s.c_str() ) ;
		c2pstr( (char *) s2 ) ;
#endif

	    for ( int i = 0 ; i < m_noItems ; ++ i )
	    {
#if TARGET_CARBON
	    	c2pstrcpy( (StringPtr) s1 , m_stringArray[i].c_str() ) ;
#else
	    	strcpy( (char *) s1 , m_stringArray[i].c_str() ) ;
			c2pstr( (char *) s1 ) ;
#endif
	    	if ( EqualString( s1 , s2 , false , false ) )
	    		return i ;
	    }
   }
   return -1;
}

void wxListBox::Clear()
{
  Free();
  m_noItems = 0;
  m_stringArray.Empty() ;
  m_dataArray.Empty() ;
  MacClear() ;
}

void wxListBox::SetSelection(int N, bool select)
{
    wxCHECK_RET( N >= 0 && N < m_noItems,
                 "invalid index in wxListBox::SetSelection" );
	MacSetSelection( N , select ) ;
}

bool wxListBox::IsSelected(int N) const
{
    wxCHECK_MSG( N >= 0 && N < m_noItems, FALSE,
                 "invalid index in wxListBox::Selected" );

	return MacIsSelected( N ) ;
}

void *wxListBox::DoGetItemClientData(int N) const
{
    wxCHECK_MSG( N >= 0 && N < m_noItems, NULL,
                 "invalid index in wxListBox::GetClientData" );

    return (void *)m_dataArray[N];
}

wxClientData *wxListBox::DoGetItemClientObject(int N) const
{
	return (wxClientData *) DoGetItemClientData( N ) ;
}

void wxListBox::DoSetItemClientData(int N, void *Client_data)
{
    wxCHECK_RET( N >= 0 && N < m_noItems,
                 "invalid index in wxListBox::SetClientData" );

#if wxUSE_OWNER_DRAWN
    if ( m_windowStyle & wxLB_OWNERDRAW )
    {
        // client data must be pointer to wxOwnerDrawn, otherwise we would crash
        // in OnMeasure/OnDraw.
        wxFAIL_MSG(wxT("Can't use client data with owner-drawn listboxes"));
    }
#endif // wxUSE_OWNER_DRAWN
	wxASSERT_MSG( m_dataArray.GetCount() >= N , "invalid client_data array" ) ;
	
	if ( m_dataArray.GetCount() > N )
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

/* TODO
    if ((m_windowStyle & wxLB_MULTIMacE) || (m_windowStyle & wxLB_EXTENDED))
    {
        int no_sel = ??
        for ( int n = 0; n < no_sel; n++ )
            aSelections.Add(??);

        return no_sel;
    }
    else  // single-selection listbox
    {
        aSelections.Add(??);

        return 1;
    }
*/
}

// Get single selection, for single choice list items
int wxListBox::GetSelection() const
{
	return MacGetSelection() ;
}

// Find string for position
wxString wxListBox::GetString(int N) const
{
	if( wxApp::s_macDefaultEncodingIsPC )
	{
		return 		wxMacMakePCStringFromMac( m_stringArray[N] ) ;
	}
	else
		return m_stringArray[N]  ;
}

void wxListBox::DoInsertItems(const wxArrayString& items, int pos)
{
    wxCHECK_RET( pos >= 0 && pos <= m_noItems,
                 wxT("invalid index in wxListBox::InsertItems") );

    int nItems = items.GetCount();

	for ( int i = 0 ; i < nItems ; i++ )
	{
		m_stringArray.Insert( items[i] , pos + i ) ;
		m_dataArray.Insert( NULL , pos + i ) ;
		MacInsert( pos + i , items[i] ) ;
	}

    m_noItems += nItems;
}

void wxListBox::SetString(int N, const wxString& s)
{
	wxString str ;
	if( wxApp::s_macDefaultEncodingIsPC )
	{
		str = wxMacMakeMacStringFromPC( s )  ;
	}
	else
		str = s ;
	m_stringArray[N] = str ;
	MacSet( N , s ) ;
}

wxSize wxListBox::DoGetBestSize() const
{
    return wxSize(100, 100);
}

int wxListBox::GetCount() const
{
    return m_noItems;
}

void wxListBox::SetupColours()
{
    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOW));
    SetForegroundColour(GetParent()->GetForegroundColour());
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

// ============================================================================
// list box control implementation
// ============================================================================

void MacDrawStringCell(Rect *cellRect, Cell lCell, ListHandle theList, long refCon)
{
	wxListBox*			list;
	// typecast our refCon
	list = (wxListBox*)refCon;
		
	MoveTo(cellRect->left + 4 , cellRect->top + 10 );
	const wxString text = list->m_stringArray[lCell.v] ;
	::TextFont( kFontIDMonaco ) ;
	::TextSize( 9  );
	::TextFace( 0 ) ;
	DrawText(text, 0 , text.Length());
		
}

void wxListBox::MacDelete( int N )
{
	ListHandle list ;
	long	result ;
	UMAGetControlData( m_macControl , kControlNoPart , kControlListBoxListHandleTag , sizeof( ListHandle ) , (char*) &list  , &result ) ;
	LDelRow( 1 , N , list ) ;
}

void wxListBox::MacInsert( int n , const char * text) 
{
	Cell	cell ;

	cell.h = 0 ;
	cell.v = n ;

	LAddRow( 1 , cell.v , m_macList ) ;
}

void wxListBox::MacAppend( const char * text) 
{
	Cell cell = { 0 , 0 } ;
	cell.v = (**m_macList).dataBounds.bottom ;
	LAddRow( 1 , cell.v , m_macList ) ;
}

void wxListBox::MacClear() 
{
	LDelRow( (**m_macList).dataBounds.bottom , 0 , m_macList ) ;
}

void wxListBox::MacSetSelection( int n , bool select )
{
	Cell cell = { 0 , 0 } ;
	if ( LGetSelect( TRUE , &cell , m_macList ) )
	{
		LSetSelect( false , cell , m_macList ) ;
	}
	
	cell.v = n ;
	LSetSelect( select , cell , m_macList ) ;
	LAutoScroll( m_macList ) ;
}

bool wxListBox::MacIsSelected( int n ) const
{
	Cell cell = { 0 , 0 } ;
	cell.v = n ;
	return LGetSelect( false , &cell , m_macList ) ;
}

void wxListBox::MacDestroy()
{
//	DisposeExtLDEFInfo( m_macList ) ;
}

int wxListBox::MacGetSelection() const
{
	Cell cell = { 0 , 0 } ;
	if ( LGetSelect( true , &cell , m_macList ) )
		return cell.v ;
	else
		return -1 ;
}

int wxListBox::MacGetSelections( wxArrayInt& aSelections ) const
{
	int no_sel = 0 ;
	
    aSelections.Empty();

	Cell cell = { 0 , 0 } ;
	cell.v = 0 ;
	
	while ( LGetSelect( true , &cell , m_macList ) )
	{
		aSelections.Add( cell.v ) ;
		no_sel++ ;
		cell.v++ ;
	}
	return no_sel ;
}

void wxListBox::MacSet( int n , const char * text )
{
	// our implementation does not store anything in the list
	// so we just have to redraw
	Cell cell = { 0 , 0 } ;
	cell.v = n ;
	LDraw( cell , m_macList ) ;
}

void wxListBox::MacScrollTo( int n )
{
	// TODO implement scrolling
}

void wxListBox::OnSize( const wxSizeEvent &event)
{
	Point pt = (**m_macList).cellSize ;
	pt.h =  m_width - 15  ;
	LCellSize( pt , m_macList ) ;
}

void wxListBox::MacHandleControlClick( ControlHandle control , SInt16 controlpart ) 
{
	Boolean wasDoubleClick = false ;
	long	result ;

	UMAGetControlData( m_macControl , kControlNoPart , kControlListBoxDoubleClickTag , sizeof( wasDoubleClick ) , (char*) &wasDoubleClick  , &result ) ;
	if ( !wasDoubleClick )
	{
		MacDoClick() ;
	}
	else
	{
		MacDoDoubleClick() ;
	}
}

void wxListBox::MacSetRedraw( bool doDraw ) 
{
	LSetDrawingMode( doDraw , m_macList ) ;
	
}

void wxListBox::MacDoClick()
{
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

void wxListBox::MacDoDoubleClick()
{
    wxCommandEvent event(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, m_windowId);
    event.SetEventObject( this );
	GetEventHandler()->ProcessEvent(event) ; 
}

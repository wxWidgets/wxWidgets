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

#include "wx/listbox.h"
#include "wx/settings.h"
#include "wx/dynarray.h"
#include "wx/log.h"

#include "wx/utils.h"
#include "extldef.h"

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
	m_macHorizontalBorder = 5 ; // additional pixels around the real control
	m_macVerticalBorder = 5 ;
	
	MacPreControlCreate( parent , id ,  "" , pos , size ,style, validator , name , &bounds , title ) ;

	m_macControl = UMANewControl( parent->GetMacRootWindow() , &bounds , title , true , kwxMacListWithVerticalScrollbar , 0 , 0, 
	  	kControlListBoxProc , (long) this ) ;
	
	long	result ;
	UMAGetControlData( m_macControl , kControlNoPart , kControlListBoxListHandleTag , sizeof( ListHandle ) , (char*) &m_macList  , &result ) ;

	NewExtLDEFInfo( m_macList , MacDrawStringCell , (long) this ) ;
	(**m_macList).selFlags = lOnlyOne ;
	if ( style  & wxLB_MULTIPLE )
	{
		(**m_macList).selFlags += lNoExtend ;
	}
	else if ( style & wxLB_EXTENDED )
	{
		(**m_macList).selFlags += lExtendDrag ;
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
//	DisposeExtLDEFInfo( m_macList ) ;
}

void wxListBox::SetFirstItem(int N)
{
	MacScrollTo( N ) ;
}

void wxListBox::SetFirstItem(const wxString& s)
{
	MacScrollTo( FindString( s ) ) ;
}

void wxListBox::Delete(int N)
{
	m_dataArray.Remove( N ) ;
	m_stringArray.Remove( N ) ;
  	m_noItems --;
	
	MacDelete( N ) ;
}

void wxListBox::Append(const wxString& item)
{
	Append( item , NULL ) ;
}

void wxListBox::Append(const wxString& item, char *Client_data)
{
	if( wxApp::s_macDefaultEncodingIsPC )
	{
		m_stringArray.Add( wxMacMakeMacStringFromPC( item ) ) ;
	}
	else
		m_stringArray.Add( item ) ;
	m_dataArray.Add( Client_data ) ;
  	m_noItems ++;
  	
	MacAppend( item ) ;
}

void wxListBox::Set(int n, const wxString *choices, char** clientData)
{
  Clear() ;  
  for( int i = 0 ; i < n ; ++i )
  {
  	if ( clientData )
  		Append( choices[i] , clientData[0] ) ;
  	else
  		Append( choices[i] ) ;
  }
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
    for ( int i = 0 ; i < m_noItems ; ++ i )
    {
    	if ( equalstring( m_stringArray[i].Left( len ) , search , false , false ) )
    		return i ;
    }
	}
	else
	{
    for ( int i = 0 ; i < m_noItems ; ++ i )
    {
    	if ( equalstring( m_stringArray[i] , s , false , false ) )
    		return i ;
    }
   }
   return -1;
}

void wxListBox::Clear()
{
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

bool wxListBox::Selected(int N) const
{
    wxCHECK_MSG( N >= 0 && N < m_noItems, FALSE,
                 "invalid index in wxListBox::Selected" );

	return MacIsSelected( N ) ;
}

void wxListBox::Deselect(int N)
{
    wxCHECK_RET( N >= 0 && N < m_noItems,
                 "invalid index in wxListBox::Deselect" );

    SetSelection( N , false ) ;
}

char *wxListBox::GetClientData(int N) const
{
    wxCHECK_MSG( N >= 0 && N < m_noItems, NULL,
                 "invalid index in wxListBox::GetClientData" );

    return m_dataArray[N];
}

void wxListBox::SetClientData(int N, char *Client_data)
{
    wxCHECK_RET( N >= 0 && N < m_noItems,
                 "invalid index in wxListBox::SetClientData" );

    m_dataArray[N] = Client_data ;
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

void wxListBox::InsertItems(int nItems, const wxString items[], int pos)
{
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
	m_stringArray[N] = s ;
	MacSet( N , s ) ;
}

int wxListBox::Number () const
{
  return m_noItems;
}

// For single selection items only
wxString wxListBox::GetStringSelection () const
{
  int sel = GetSelection ();
  if (sel > -1)
  {
			return GetString (sel);
  }
  else
    return wxString("");
}

bool wxListBox::SetStringSelection (const wxString& s, bool flag)
{
  int sel = FindString (s);
  if (sel > -1)
    {
      SetSelection (sel, flag);
      return TRUE;
    }
  else
    return FALSE;
}

void wxListBox::Command (wxCommandEvent & event)
{
  if (event.m_extraLong)
    SetSelection (event.m_commandInt);
  else
    {
      Deselect (event.m_commandInt);
      return;
    }
  ProcessCommand (event);
}

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
	Cell cell = { 0 , 0 } ;
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
	pt.h =  m_width - 15 /* scrollbar */ - m_macHorizontalBorder * 2 ;
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
	wxArrayInt aSelections;
	int count = GetSelections(aSelections);
	if ( count > 0 )
	{
		  event.m_commandInt = aSelections[0] ;
		  event.m_clientData = GetClientData(event.m_commandInt);
		  wxString str(GetString(event.m_commandInt));
		  if (str != "")
		    event.m_commandString = copystring((char *)(const char *)str);
	}
	else
	{
		return ;
		/*
		  event.m_commandInt = -1 ;
		  event.m_commandString = copystring("") ;
		 */
	}
	
	event.SetEventObject( this );
	ProcessCommand(event);
	if (event.m_commandString)
	  delete[] event.m_commandString ;
}

void wxListBox::MacDoDoubleClick()
{
    wxCommandEvent event(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, m_windowId);
    event.SetEventObject( this );
	GetEventHandler()->ProcessEvent(event) ; 
}

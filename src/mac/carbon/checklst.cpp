///////////////////////////////////////////////////////////////////////////////
// Name:        checklst.cpp
// Purpose:     implementation of wxCheckListBox class
// Author:      Stefan Csomor
// Modified by: 
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers & declarations
// ============================================================================

#ifdef __GNUG__
#pragma implementation "checklst.h"
#endif

#include "wx/defs.h"

#if wxUSE_CHECKLISTBOX

#include "wx/checklst.h"
#include "wx/arrstr.h"

#include "wx/mac/uma.h"
#include <Appearance.h>

// ============================================================================
// implementation of wxCheckListBox
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxCheckListBox, wxListBox)

BEGIN_EVENT_TABLE(wxCheckListBox, wxListBox)
END_EVENT_TABLE()

const short kTextColumnId = 1024 ;
const short kCheckboxColumnId = 1025 ;

// new databrowser based version

// Listbox item
void wxCheckListBox::Init()
{
}

bool wxCheckListBox::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxPoint &pos,
                            const wxSize &size,
                            const wxArrayString& choices,
                            long style,
                            const wxValidator& validator,
                            const wxString &name)
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
        wxCheckListBox* list = wxDynamicCast( (wxObject*) ref , wxCheckListBox ) ;
        for ( size_t i = 0 ; i < list->m_idArray.GetCount() ; ++i )
            if ( list->m_idArray[i] == (long) itemID )
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

                break ;
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
    		        wxCheckListBox* list = wxDynamicCast( (wxObject*) ref , wxCheckListBox ) ;
    		        for ( size_t i = 0 ; i < list->m_idArray.GetCount() ; ++i )
    		            if ( list->m_idArray[i] == (long) itemID )
    		            {
    		                wxMacCFStringHolder cf( list->GetString(i) , list->GetFont().GetEncoding() ) ;
    		                verify_noerr( ::SetDataBrowserItemDataText( itemData , cf ) ) ;
    		                err = noErr ;
    		                break ;
    		            }
    			}
    		}	
    		break;
    	    case kCheckboxColumnId :
    	    {
    		    long ref = GetControlReference( browser ) ;
    		    if ( ref )
    		    {
    		        wxCheckListBox* list = wxDynamicCast( (wxObject*) ref , wxCheckListBox ) ;
    		        for ( size_t i = 0 ; i < list->m_idArray.GetCount() ; ++i )
    		            if ( list->m_idArray[i] == (long) itemID )
    		            {
    		                verify_noerr( ::SetDataBrowserItemDataButtonValue( itemData , list->IsChecked( i ) ? kThemeButtonOn : kThemeButtonOff ) ) ;
    		                err = noErr ;
    		                break ;
    		            }
    			}
    	    }
    	    break ;
    		case kDataBrowserItemIsEditableProperty:
    		{	
    		    err = ::SetDataBrowserItemDataBooleanValue(itemData, true);
    		}	
    		break;

    		default:    		
    		break;
    	}
	}
	else
	{
	    switch( property )
	    {
	        case kCheckboxColumnId :
	        {
     		    long ref = GetControlReference( browser ) ;
    		    if ( ref )
    		    {
    		        wxCheckListBox* list = wxDynamicCast( (wxObject*) ref , wxCheckListBox ) ;
    		        for ( size_t i = 0 ; i < list->m_idArray.GetCount() ; ++i )
    		            if ( list->m_idArray[i] == (long) itemID )
    		            {
    		                // we have to change this behind the back, since Check() would be triggering another update round
                            bool newVal = !list->IsChecked( i ) ;
    		                verify_noerr( ::SetDataBrowserItemDataButtonValue( itemData , newVal ? kThemeButtonOn : kThemeButtonOff ) ) ;
    		                err = noErr ;
    		                list->m_checks[ i ] = newVal ;

                            wxCommandEvent event(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, list->GetId());
                            event.SetInt(i);
                            event.SetEventObject(list);
                            list->GetEventHandler()->ProcessEvent(event);

    		                break ;
    		            }
    			}
	        
	        }
	        break ;
	        
	        default :
	        break ;
	    }
	}
	
	return err;
}
bool wxCheckListBox::Create(wxWindow *parent, wxWindowID id,
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
    m_nextId = 1 ;
    

    Rect bounds = wxMacGetBoundsForControl( this , pos , size ) ;

    m_peer = new wxMacControl() ;
    verify_noerr( ::CreateDataBrowserControl( MAC_WXHWND(parent->MacGetTopLevelWindowRef()), &bounds, kDataBrowserListView , *m_peer ) );
    

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
    verify_noerr(SetDataBrowserSelectionFlags  (*m_peer, options ) ); 

    DataBrowserListViewColumnDesc columnDesc ;
    columnDesc.headerBtnDesc.titleOffset = 0;
	columnDesc.headerBtnDesc.version = kDataBrowserListViewLatestHeaderDesc;
		
	columnDesc.headerBtnDesc.btnFontStyle.flags	= 
		kControlUseFontMask | kControlUseJustMask;
	
	columnDesc.headerBtnDesc.btnContentInfo.contentType = kControlNoContent;
	columnDesc.headerBtnDesc.btnFontStyle.just = teFlushDefault;
	columnDesc.headerBtnDesc.btnFontStyle.font = kControlFontViewSystemFont;
	columnDesc.headerBtnDesc.btnFontStyle.style = normal;
	columnDesc.headerBtnDesc.titleString = NULL ; // CFSTR( "" );

    // check column

	columnDesc.headerBtnDesc.minimumWidth = 30 ;
	columnDesc.headerBtnDesc.maximumWidth = 30;

	columnDesc.propertyDesc.propertyID = kCheckboxColumnId;
	columnDesc.propertyDesc.propertyType = kDataBrowserCheckboxType;
	columnDesc.propertyDesc.propertyFlags = kDataBrowserPropertyIsMutable | kDataBrowserTableViewSelectionColumn |
                                            kDataBrowserDefaultPropertyFlags;
	verify_noerr(::AddDataBrowserListViewColumn(*m_peer, &columnDesc, kDataBrowserListViewAppendColumn) ) ;

    // text column

	columnDesc.headerBtnDesc.minimumWidth = 0;
	columnDesc.headerBtnDesc.maximumWidth = 10000;

	columnDesc.propertyDesc.propertyID = kTextColumnId;
	columnDesc.propertyDesc.propertyType = kDataBrowserTextType;
	columnDesc.propertyDesc.propertyFlags = kDataBrowserTableViewSelectionColumn
#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2
	 | kDataBrowserListViewTypeSelectColumn
#endif
	  ;

	
	verify_noerr(::AddDataBrowserListViewColumn(*m_peer, &columnDesc, kDataBrowserListViewAppendColumn) ) ;

    verify_noerr(::AutoSizeDataBrowserListViewColumns( *m_peer ) ) ;
    verify_noerr(::SetDataBrowserHasScrollBars( *m_peer , false , true ) ) ;
    verify_noerr(::SetDataBrowserTableViewHiliteStyle( *m_peer, kDataBrowserTableViewFillHilite  ) ) ;
    verify_noerr(::SetDataBrowserListViewHeaderBtnHeight( *m_peer , 0 ) ) ;

    DataBrowserCallbacks callbacks ;
    callbacks.version = kDataBrowserLatestCallbacks;
    InitDataBrowserCallbacks(&callbacks);
    callbacks.u.v1.itemDataCallback = NewDataBrowserItemDataUPP(ListBoxGetSetItemData);	
	callbacks.u.v1.itemNotificationCallback =
#if TARGET_API_MAC_OSX
	    (DataBrowserItemNotificationUPP) NewDataBrowserItemNotificationWithItemUPP(DataBrowserItemNotificationProc) ;
#else
	    NewDataBrowserItemNotificationUPP(DataBrowserItemNotificationProc) ;
#endif
    SetDataBrowserCallbacks(*m_peer, &callbacks);

    MacPostControlCreate(pos,size) ;

    for ( int i = 0 ; i < n ; i++ )
    {
        Append( choices[i] ) ;
    }

    SetBestSize(size);   // Needed because it is a wxControlWithItems
    
    return TRUE;
}

// ----------------------------------------------------------------------------
// wxCheckListBox functions
// ----------------------------------------------------------------------------

bool wxCheckListBox::IsChecked(size_t item) const
{
    wxCHECK_MSG( item < m_checks.GetCount(), FALSE,
                 _T("invalid index in wxCheckListBox::IsChecked") );

    return m_checks[item] != 0;
}

void wxCheckListBox::Check(size_t item, bool check)
{
    wxCHECK_RET( item < m_checks.GetCount(),
                 _T("invalid index in wxCheckListBox::Check") );

    // intermediate var is needed to avoid compiler warning with VC++
    bool isChecked = m_checks[item] != 0;
    if ( check != isChecked )
    {
        m_checks[item] = check;
        UInt32 id = m_idArray[item] ;
        verify_noerr( ::UpdateDataBrowserItems( *m_peer , kDataBrowserNoItem , 1 , &id , kDataBrowserItemNoProperty , kDataBrowserItemNoProperty ) ) ;
    }
}

// ----------------------------------------------------------------------------
// methods forwarded to wxCheckListBox
// ----------------------------------------------------------------------------

void wxCheckListBox::Delete(int n)
{
    wxCHECK_RET( n < GetCount(), _T("invalid index in wxCheckListBox::Delete") );

    wxListBox::Delete(n);

    m_checks.RemoveAt(n);
}

int wxCheckListBox::DoAppend(const wxString& item)
{
    int pos = wxListBox::DoAppend(item);

    // the item is initially unchecked
    m_checks.Insert(FALSE, pos);

    return pos;
}

void wxCheckListBox::DoInsertItems(const wxArrayString& items, int pos)
{
    wxListBox::DoInsertItems(items, pos);

    size_t count = items.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_checks.Insert(FALSE, pos + n);
    }
}

void wxCheckListBox::DoSetItems(const wxArrayString& items, void **clientData)
{
    // call it first as it does DoClear()
    wxListBox::DoSetItems(items, clientData);

    size_t count = items.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_checks.Add(FALSE);
    }
}

void wxCheckListBox::DoClear()
{
    m_checks.Empty();
}

#endif // wxUSE_CHECKLISTBOX

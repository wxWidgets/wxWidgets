///////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/checklst.cpp
// Purpose:     implementation of wxCheckListBox class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////
//
// new DataBrowser-based version


#include "wx/wxprec.h"

#if wxUSE_CHECKLISTBOX

#include "wx/checklst.h"
#include "wx/arrstr.h"

#include "wx/mac/uma.h"

#ifndef __DARWIN__
#include <Appearance.h>
#endif

IMPLEMENT_DYNAMIC_CLASS(wxCheckListBox, wxListBox)

BEGIN_EVENT_TABLE(wxCheckListBox, wxListBox)
END_EVENT_TABLE()

const short kTextColumnId = 1024;
const short kCheckboxColumnId = 1025;


void wxCheckListBox::Init()
{
}

bool wxCheckListBox::Create(
    wxWindow *parent,
    wxWindowID id,
    const wxPoint &pos,
    const wxSize &size,
    const wxArrayString& choices,
    long style,
    const wxValidator& validator,
    const wxString &name )
{
    wxCArrayString chs( choices );

    return Create( parent, id, pos, size, chs.GetCount(), chs.GetStrings(), style, validator, name );
}

#if TARGET_API_MAC_OSX
static pascal void DataBrowserItemNotificationProc(
    ControlRef browser,
    DataBrowserItemID itemID,
    DataBrowserItemNotification message,
    DataBrowserItemDataRef itemData )
#else
static pascal void DataBrowserItemNotificationProc(
    ControlRef browser,
    DataBrowserItemID itemID,
    DataBrowserItemNotification message )
#endif
{
    long ref = GetControlReference( browser );
    if (ref != 0)
    {
        wxCheckListBox* list = wxDynamicCast( (wxObject*)ref, wxCheckListBox );
        int i = itemID - 1;
        if ((i >= 0) && (i < (int)list->GetCount()))
        {
            bool trigger = false;
            wxCommandEvent event( wxEVT_COMMAND_LISTBOX_SELECTED, list->GetId() );
            switch ( message )
            {
                case kDataBrowserItemDeselected:
                    if ( list->HasMultipleSelection() )
                        trigger = true;
                    break;

                case kDataBrowserItemSelected:
                    trigger = true;
                    break;

                case kDataBrowserItemDoubleClicked:
                    event.SetEventType( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED );
                    trigger = true;
                    break;

                 default:
                    break;
            }

            if ( trigger )
            {
                event.SetEventObject( list );
                if ( list->HasClientObjectData() )
                    event.SetClientObject( list->GetClientObject( i ) );
                else if ( list->HasClientUntypedData() )
                    event.SetClientData( list->GetClientData( i ) );
                event.SetString( list->GetString( i ) );
                event.SetInt( i );
                event.SetExtraLong( list->HasMultipleSelection() ? message == kDataBrowserItemSelected : true );
                wxPostEvent( list->GetEventHandler(), event );

                // direct notification is not always having the listbox GetSelection() having in sync with event
                // list->GetEventHandler()->ProcessEvent( event );
            }
        }
    }
}

static pascal OSStatus ListBoxGetSetItemData(
    ControlRef browser,
    DataBrowserItemID itemID,
    DataBrowserPropertyID property,
    DataBrowserItemDataRef itemData,
    Boolean changeValue )
{
    OSStatus err = errDataBrowserPropertyNotSupported;

    if ( !changeValue )
    {
        switch (property)
        {
            case kTextColumnId:
            {
                long ref = GetControlReference( browser );
                if (ref != 0)
                {
                    wxCheckListBox* list = wxDynamicCast( (wxObject*) ref, wxCheckListBox );
                    int i = itemID - 1;
                    if ((i >= 0) && (i < (int)list->GetCount()))
                    {
                        wxMacCFStringHolder cf( list->GetString( i ), list->GetFont().GetEncoding() );
                        verify_noerr( ::SetDataBrowserItemDataText( itemData, cf ) );
                        err = noErr;
                    }
                }
            }
            break;

            case kCheckboxColumnId:
            {
                long ref = GetControlReference( browser );
                if (ref != 0)
                {
                    wxCheckListBox* list = wxDynamicCast( (wxObject*)ref, wxCheckListBox );
                    int i = itemID - 1;
                    if ((i >= 0) && (i < (int)list->GetCount()))
                    {
                        verify_noerr( ::SetDataBrowserItemDataButtonValue( itemData, list->IsChecked(i) ? kThemeButtonOn : kThemeButtonOff ) );
                        err = noErr;
                    }
                }
            }
            break;

            case kDataBrowserItemIsEditableProperty:
            {
                err = ::SetDataBrowserItemDataBooleanValue( itemData, true );
            }
            break;

            default:
            break;
        }
    }
    else
    {
        switch ( property )
        {
            case kCheckboxColumnId:
            {
                long ref = GetControlReference( browser );
                if (ref != 0)
                {
                    wxCheckListBox* list = wxDynamicCast( (wxObject*) ref, wxCheckListBox );
                    int i = itemID - 1;
                    if ((i >= 0) && (i < (int)list->GetCount()))
                    {
                        // we have to change this behind the back, since Check() would be triggering another update round
                        bool newVal = !list->IsChecked(i);
                        verify_noerr( ::SetDataBrowserItemDataButtonValue( itemData, newVal ? kThemeButtonOn : kThemeButtonOff ) );
                        err = noErr;
                        list->m_checks[i] = newVal;

                        wxCommandEvent event(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, list->GetId());
                        event.SetInt( i );
                        event.SetEventObject( list );
                        list->GetEventHandler()->ProcessEvent( event );
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

bool wxCheckListBox::Create(
   wxWindow *parent,
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

    wxASSERT_MSG( !(style & wxLB_MULTIPLE) || !(style & wxLB_EXTENDED),
                  wxT("only one of listbox selection modes can be specified") );

    if ( !wxListBoxBase::Create( parent, id, pos, size, style & ~(wxHSCROLL | wxVSCROLL), validator, name ) )
        return false;

    // this will be increased by our Append command
    m_noItems = 0;
    m_selected = 0;

    Rect bounds = wxMacGetBoundsForControl( this, pos, size );

    m_peer = new wxMacControl( this );
    OSStatus err = ::CreateDataBrowserControl(
        MAC_WXHWND(parent->MacGetTopLevelWindowRef()),
        &bounds, kDataBrowserListView, m_peer->GetControlRefAddr() );
    verify_noerr( err );

    DataBrowserSelectionFlags  options = kDataBrowserDragSelect;
    if ( style & wxLB_MULTIPLE )
    {
        options |= kDataBrowserAlwaysExtendSelection | kDataBrowserCmdTogglesSelection;
    }
    else if ( style & wxLB_EXTENDED )
    {
        // default behaviour
    }
    else
    {
        options |= kDataBrowserSelectOnlyOne;
    }

    err = m_peer->SetSelectionFlags( options );
    verify_noerr( err );

    DataBrowserListViewColumnDesc columnDesc;
    columnDesc.headerBtnDesc.titleOffset = 0;
    columnDesc.headerBtnDesc.version = kDataBrowserListViewLatestHeaderDesc;

    columnDesc.headerBtnDesc.btnFontStyle.flags =
        kControlUseFontMask | kControlUseJustMask;

    columnDesc.headerBtnDesc.btnContentInfo.contentType = kControlNoContent;
    columnDesc.headerBtnDesc.btnFontStyle.just = teFlushDefault;
    columnDesc.headerBtnDesc.btnFontStyle.font = kControlFontViewSystemFont;
    columnDesc.headerBtnDesc.btnFontStyle.style = normal;
    columnDesc.headerBtnDesc.titleString = NULL; // CFSTR( "" );

    // check column

    columnDesc.headerBtnDesc.minimumWidth = 30;
    columnDesc.headerBtnDesc.maximumWidth = 30;

    columnDesc.propertyDesc.propertyID = kCheckboxColumnId;
    columnDesc.propertyDesc.propertyType = kDataBrowserCheckboxType;
    columnDesc.propertyDesc.propertyFlags =
        kDataBrowserPropertyIsMutable
        | kDataBrowserTableViewSelectionColumn
        | kDataBrowserDefaultPropertyFlags;
    err = m_peer->AddListViewColumn( &columnDesc, kDataBrowserListViewAppendColumn );
    verify_noerr( err );

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

    verify_noerr( m_peer->AddListViewColumn( &columnDesc, kDataBrowserListViewAppendColumn ) );

    verify_noerr( m_peer->AutoSizeListViewColumns() );
    verify_noerr( m_peer->SetHasScrollBars( false, true ) );
    verify_noerr( m_peer->SetTableViewHiliteStyle( kDataBrowserTableViewFillHilite ) );
    verify_noerr( m_peer->SetListViewHeaderBtnHeight( 0 ) );

    DataBrowserCallbacks callbacks;
    callbacks.version = kDataBrowserLatestCallbacks;
    InitDataBrowserCallbacks( &callbacks );
    callbacks.u.v1.itemDataCallback = NewDataBrowserItemDataUPP( &ListBoxGetSetItemData );
    callbacks.u.v1.itemNotificationCallback =
#if TARGET_API_MAC_OSX
        (DataBrowserItemNotificationUPP) NewDataBrowserItemNotificationWithItemUPP( &DataBrowserItemNotificationProc );
#else
        NewDataBrowserItemNotificationUPP( &DataBrowserItemNotificationProc );
#endif
    m_peer->SetCallbacks( &callbacks );

#if 0
    // shouldn't be necessary anymore under 10.2
    m_peer->SetData( kControlNoPart, kControlDataBrowserIncludesFrameAndFocusTag, (Boolean)false );
    m_peer->SetNeedsFocusRect( true );
#endif

    MacPostControlCreate( pos, size );

    for ( int i = 0; i < n; i++ )
    {
        Append( choices[i] );
    }

    // Needed because it is a wxControlWithItems
    SetBestSize( size );

    return true;
}

// ----------------------------------------------------------------------------
// wxCheckListBox functions
// ----------------------------------------------------------------------------

bool wxCheckListBox::IsChecked(unsigned int item) const
{
    wxCHECK_MSG( IsValid(item), false,
                 wxT("invalid index in wxCheckListBox::IsChecked") );

    return m_checks[item] != 0;
}

void wxCheckListBox::Check(unsigned int item, bool check)
{
    wxCHECK_RET( IsValid(item),
                 wxT("invalid index in wxCheckListBox::Check") );

    bool isChecked = m_checks[item] != 0;
    if ( check != isChecked )
    {
        m_checks[item] = check;
        UInt32 id = item + 1;
        OSStatus err = m_peer->UpdateItems(
            kDataBrowserNoItem, 1, &id,
            kDataBrowserItemNoProperty, kDataBrowserItemNoProperty );
        verify_noerr( err );
    }
}

// ----------------------------------------------------------------------------
// methods forwarded to wxCheckListBox
// ----------------------------------------------------------------------------

void wxCheckListBox::Delete(unsigned int n)
{
    wxCHECK_RET( IsValid(n), wxT("invalid index in wxCheckListBox::Delete") );

    wxListBox::Delete( n );
    m_checks.RemoveAt( n );
}

int wxCheckListBox::DoAppend(const wxString& item)
{
    int pos = wxListBox::DoAppend( item );

    // the item is initially unchecked
    m_checks.Insert( false, pos );

    return pos;
}

void wxCheckListBox::DoInsertItems(const wxArrayString& items, unsigned int pos)
{
    wxListBox::DoInsertItems( items, pos );

    unsigned int count = items.GetCount();
    for ( unsigned int n = 0; n < count; n++ )
    {
        m_checks.Insert( false, pos + n );
    }
}

void wxCheckListBox::DoSetItems(const wxArrayString& items, void **clientData)
{
    // call it first as it does DoClear()
    wxListBox::DoSetItems( items, clientData );

    unsigned int count = items.GetCount();
    for ( unsigned int n = 0; n < count; n++ )
    {
        m_checks.Add( false );
    }
}

void wxCheckListBox::DoClear()
{
    m_checks.Empty();
}

#endif // wxUSE_CHECKLISTBOX

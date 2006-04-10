///////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/listbox.cpp
// Purpose:     wxListBox
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_LISTBOX

#include "wx/app.h"
#include "wx/listbox.h"
#include "wx/checklst.h"
#include "wx/button.h"
#include "wx/settings.h"
#include "wx/toplevel.h"
#include "wx/dynarray.h"
#include "wx/log.h"

#include "wx/utils.h"

IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxControl)

BEGIN_EVENT_TABLE(wxListBox, wxControl)
END_EVENT_TABLE()

#include "wx/mac/uma.h"

// common interface for all implementations
class wxMacListControl : public wxMacControl
{
public:
    wxMacListControl( wxListBox *peer ) :
        wxMacControl( peer )
    {
    }

    ~wxMacListControl()
    {
    }

    virtual void            UpdateLine( int n ) = 0;

    virtual void            MacDelete( int n ) = 0;
    virtual void            MacInsert( int n, const wxString& item) = 0;
    virtual void            MacInsert( int n, const wxArrayString& items) = 0;
    virtual void            MacAppend( const wxString& item) = 0;
    virtual void            MacSet( int n, const wxString& item ) = 0;
    virtual void            MacClear() = 0;
    virtual void            MacDeselectAll() = 0;
    virtual void            MacSetSelection( int n, bool select ) = 0;
    virtual int             MacGetSelection() const = 0;
    virtual int             MacGetSelections(wxArrayInt& aSelections) const = 0;
    virtual bool            MacIsSelected( int n ) const= 0;
    virtual void            MacScrollTo( int n ) = 0;

    wxListBox * GetPeer() const
    {
        return (wxListBox*)m_peer;
    }
};

#if 0
// In case we have to replace data browser ...
// custom HIView-based implementation

class wxMacCustomHIViewListControl : public wxMacListControl
{
public:
    wxMacCustomHIViewListControl( wxListBox *peer, const wxPoint& pos, const wxSize& size, long style );
    ~wxMacCustomHIViewListControl();

    void            MacDelete( int n );
    void            MacInsert( int n, const wxString& item);
    void            MacInsert( int n, const wxArrayString& items);
    void            MacAppend( const wxString& item);
    void            MacSet( int n, const wxString& item );
    void            MacClear();
    void            MacDeselectAll();
    void            MacSetSelection( int n, bool select );
    int             MacGetSelection() const;
    int             MacGetSelections(wxArrayInt& aSelections) const;
    bool            MacIsSelected( int n ) const;
    void            MacScrollTo( int n );
};
#endif

// DataBrowser-based implementation

class wxMacDataBrowserListControl : public wxMacListControl
{
public:
    wxMacDataBrowserListControl( wxListBox *peer, const wxPoint& pos, const wxSize& size, long style );
    ~wxMacDataBrowserListControl();

    void            UpdateLine( int n );

    void            MacDelete( int n );
    void            MacInsert( int n, const wxString& item);
    void            MacInsert( int n, const wxArrayString& items);
    void            MacAppend( const wxString& item);
    void            MacSet( int n, const wxString& item );
    void            MacClear();
    void            MacDeselectAll();
    void            MacSetSelection( int n, bool select );
    int             MacGetSelection() const;
    int             MacGetSelections(wxArrayInt& aSelections) const;
    bool            MacIsSelected( int n ) const;
    void            MacScrollTo( int n );

    virtual OSStatus SetSelectionFlags( DataBrowserSelectionFlags );
    virtual OSStatus AddListViewColumn( DataBrowserListViewColumnDesc *columnDesc,
        DataBrowserTableViewColumnIndex position );
    virtual OSStatus AutoSizeListViewColumns();
    virtual OSStatus SetHasScrollBars( bool horiz, bool vert );
    virtual OSStatus SetTableViewHiliteStyle( DataBrowserTableViewHiliteStyle hiliteStyle );
    virtual OSStatus SetListViewHeaderBtnHeight(UInt16 height);
    virtual OSStatus SetCallbacks(const DataBrowserCallbacks *callbacks);
    virtual OSStatus UpdateItems( DataBrowserItemID container, UInt32 numItems,
            const DataBrowserItemID *items,
            DataBrowserPropertyID preSortProperty,
            DataBrowserPropertyID propertyID );
    virtual OSStatus AddItems( DataBrowserItemID container, UInt32 numItems,
            const DataBrowserItemID *items,
            DataBrowserPropertyID preSortProperty );
    virtual OSStatus RemoveItems( DataBrowserItemID container, UInt32 numItems,
            const DataBrowserItemID *items,
            DataBrowserPropertyID preSortProperty );
    virtual OSStatus RevealItem( DataBrowserItemID item,
            DataBrowserPropertyID propertyID,
            DataBrowserRevealOptions options );
    virtual OSStatus GetSelectionAnchor( DataBrowserItemID *first, DataBrowserItemID *last ) const;
    virtual bool IsItemSelected( DataBrowserItemID item ) const;
    virtual OSStatus SetSelectedItems(UInt32 numItems,
            const DataBrowserItemID *items,
            DataBrowserSetOption operation );

private:
    // as we are getting the same events for human and API selection we have to suppress
    // events in the latter case
    bool MacSuppressSelection( bool suppress );
    bool MacIsSelectionSuppressed() const { return m_suppressSelection; }

    bool m_suppressSelection;

#if TARGET_API_MAC_OSX
    static pascal void DataBrowserItemNotificationProc(
        ControlRef browser,
        DataBrowserItemID itemID,
        DataBrowserItemNotification message,
        DataBrowserItemDataRef itemData );
#else
    static pascal  void DataBrowserItemNotificationProc(
        ControlRef browser,
        DataBrowserItemID itemID,
        DataBrowserItemNotification message );
#endif
};

// ============================================================================
// data browser based implementation
// ============================================================================

const short kTextColumnId = 1024;
const short kCheckboxColumnId = 1025;

// new databrowserbased version
// because of the limited insert
// functionality of DataBrowser,
// we just introduce id s corresponding
// to the line number

DataBrowserItemDataUPP gDataBrowserItemDataUPP = NULL;
DataBrowserItemNotificationUPP gDataBrowserItemNotificationUPP = NULL;

#if TARGET_API_MAC_OSX
pascal void wxMacDataBrowserListControl::DataBrowserItemNotificationProc(
    ControlRef browser,
    DataBrowserItemID itemID,
    DataBrowserItemNotification message,
    DataBrowserItemDataRef itemData)
#else
pascal void wxMacDataBrowserListControl::DataBrowserItemNotificationProc(
    ControlRef browser,
    DataBrowserItemID itemID,
    DataBrowserItemNotification message)
#endif
{
    long ref = GetControlReference( browser );
    if ( ref != 0 )
    {
        wxListBox *list = wxDynamicCast( (wxObject*)ref, wxListBox );
        wxMacDataBrowserListControl* peer = (wxMacDataBrowserListControl*) list->GetPeer();

        int i = itemID - 1;
        if (i >= 0 && i < (int)list->GetCount())
        {
            bool trigger = false;
            wxCommandEvent event( wxEVT_COMMAND_LISTBOX_SELECTED, list->GetId() );
            switch (message)
            {
                case kDataBrowserItemDeselected:
                    if ( list->HasMultipleSelection() )
                        trigger = !peer->MacIsSelectionSuppressed();
                    break;

                case kDataBrowserItemSelected:
                    trigger = !peer->MacIsSelectionSuppressed();
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
                // direct notification is not always having the listbox GetSelection() having in synch with event
                // list->GetEventHandler()->ProcessEvent(event);
            }
        }
    }
}

static pascal OSStatus ListBoxGetSetItemData(ControlRef browser,
    DataBrowserItemID itemID, DataBrowserPropertyID property,
    DataBrowserItemDataRef itemData, Boolean changeValue)
{
    OSStatus err = errDataBrowserPropertyNotSupported;

    long ref = GetControlReference( browser );

    if ( ! changeValue )
    {
        wxListBox* list = wxDynamicCast( (wxObject*) ref, wxListBox );
        bool isCheckList = false;
        if (list)
            isCheckList = list->IsKindOf( CLASSINFO(wxCheckListBox) );

        switch (property)
        {
            case kTextColumnId:
                if ( ref )
                {
                    int i = itemID - 1;
                    if (i >= 0 && i < (int)list->GetCount() )
                    {
                        wxMacCFStringHolder cf( list->GetString( i ), list->GetFont().GetEncoding() );
                        verify_noerr( ::SetDataBrowserItemDataText( itemData, cf ) );
                        err = noErr;
                    }
                }
                break;

            case kCheckboxColumnId:
                if ( ref )
                {
                    wxCheckListBox* list = wxDynamicCast( (wxObject*) ref, wxCheckListBox );
                    int i = itemID - 1;
                    if (i >= 0 && (unsigned int) i < list->GetCount())
                    {
                        verify_noerr( ::SetDataBrowserItemDataButtonValue( itemData, list->IsChecked( i ) ? kThemeButtonOn : kThemeButtonOff ) );
                        err = noErr;
                    }
                }
                break;

            case kDataBrowserItemIsEditableProperty:
                if ( isCheckList )
                    err = ::SetDataBrowserItemDataBooleanValue(itemData, true);
                break;

            default:
                break;
        }
    }
    else
    {
        switch (property)
        {
        case kCheckboxColumnId:
            if ( ref )
            {
                wxCheckListBox* list = wxDynamicCast( (wxObject*) ref, wxCheckListBox );
                int i = itemID - 1;
                if (i >= 0 && (unsigned int)i < list->GetCount() )
                {
                    // we have to change this behind the back, since Check() would be triggering another update round
                    bool newVal = !list->IsChecked( i );
                    verify_noerr( ::SetDataBrowserItemDataButtonValue( itemData, newVal ? kThemeButtonOn : kThemeButtonOff ) );
                    err = noErr;
                    list->m_checks[i] = newVal;

                    wxCommandEvent event(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, list->GetId());
                    event.SetInt(i);
                    event.SetEventObject(list);
                    list->GetEventHandler()->ProcessEvent(event);
                }
            }
            break;

        default:
            break;
        }
    }

    return err;
}

wxMacDataBrowserListControl::wxMacDataBrowserListControl( wxListBox *peer, const wxPoint& pos, const wxSize& size, long style)
    : wxMacListControl( peer )
{
    bool isCheckList = peer->IsKindOf( CLASSINFO(wxCheckListBox));

    m_suppressSelection = false;
    Rect bounds = wxMacGetBoundsForControl( peer, pos, size );
    OSStatus err = ::CreateDataBrowserControl(
        MAC_WXHWND(peer->MacGetTopLevelWindowRef()),
        &bounds, kDataBrowserListView, &m_controlRef );
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
    err = SetSelectionFlags( options );
    verify_noerr( err );

    if ( gDataBrowserItemDataUPP == NULL )
        gDataBrowserItemDataUPP = NewDataBrowserItemDataUPP(ListBoxGetSetItemData);
    if ( gDataBrowserItemNotificationUPP == NULL )
    {
        gDataBrowserItemNotificationUPP =
#if TARGET_API_MAC_OSX
            (DataBrowserItemNotificationUPP) NewDataBrowserItemNotificationWithItemUPP(DataBrowserItemNotificationProc);
#else
            NewDataBrowserItemNotificationUPP(DataBrowserItemNotificationProc);
#endif
    }

    DataBrowserCallbacks callbacks;
    InitializeDataBrowserCallbacks( &callbacks, kDataBrowserLatestCallbacks );

    callbacks.u.v1.itemDataCallback = gDataBrowserItemDataUPP;
    callbacks.u.v1.itemNotificationCallback = gDataBrowserItemNotificationUPP;
    SetCallbacks( &callbacks );

    DataBrowserListViewColumnDesc columnDesc;
    columnDesc.headerBtnDesc.titleOffset = 0;
    columnDesc.headerBtnDesc.version = kDataBrowserListViewLatestHeaderDesc;

    columnDesc.headerBtnDesc.btnFontStyle.flags =
        kControlUseFontMask | kControlUseJustMask;

    columnDesc.headerBtnDesc.btnContentInfo.contentType = kControlNoContent;
    columnDesc.headerBtnDesc.btnFontStyle.just = teFlushDefault;
    columnDesc.headerBtnDesc.btnFontStyle.font = kControlFontViewSystemFont;
    columnDesc.headerBtnDesc.btnFontStyle.style = normal;
    columnDesc.headerBtnDesc.titleString = NULL;

    if ( isCheckList )
    {
        columnDesc.headerBtnDesc.minimumWidth = 30;
        columnDesc.headerBtnDesc.maximumWidth = 30;

        columnDesc.propertyDesc.propertyID = kCheckboxColumnId;
        columnDesc.propertyDesc.propertyType = kDataBrowserCheckboxType;
        columnDesc.propertyDesc.propertyFlags =
            kDataBrowserPropertyIsMutable
            | kDataBrowserTableViewSelectionColumn
            | kDataBrowserDefaultPropertyFlags;
        err = AddListViewColumn( &columnDesc, kDataBrowserListViewAppendColumn );
        verify_noerr( err );
    }

    columnDesc.headerBtnDesc.minimumWidth = 0;
    columnDesc.headerBtnDesc.maximumWidth = 10000;


    columnDesc.propertyDesc.propertyID = kTextColumnId;
    columnDesc.propertyDesc.propertyType = kDataBrowserTextType;
    columnDesc.propertyDesc.propertyFlags = kDataBrowserTableViewSelectionColumn;
#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2
    columnDesc.propertyDesc.propertyFlags |= kDataBrowserListViewTypeSelectColumn;
#endif

    verify_noerr( AddListViewColumn( &columnDesc, kDataBrowserListViewAppendColumn ) );
    verify_noerr( AutoSizeListViewColumns() );
    verify_noerr( SetHasScrollBars( false, true ) );
    verify_noerr( SetTableViewHiliteStyle(kDataBrowserTableViewFillHilite ) );
    verify_noerr( SetListViewHeaderBtnHeight( 0 ) );

#if 0
    // shouldn't be necessary anymore under 10.2
    m_peer->SetData( kControlNoPart, kControlDataBrowserIncludesFrameAndFocusTag, (Boolean)false );
    m_peer->SetNeedsFocusRect( true );
#endif
}

wxMacDataBrowserListControl::~wxMacDataBrowserListControl()
{
}

void wxMacDataBrowserListControl::MacDelete( int n )
{
    wxArrayInt selectionBefore;
    MacGetSelections( selectionBefore );

    UInt32 id = GetPeer()->GetCount() + 1;

    OSStatus err = RemoveItems( kDataBrowserNoItem, 1, (UInt32*) &id, kDataBrowserItemNoProperty );
    verify_noerr( err );

    for ( size_t i = 0; i < selectionBefore.GetCount(); ++i )
    {
        int current = selectionBefore[i];
        if ( current == n )
        {
            // selection was deleted
            MacSetSelection( current, false );
        }
        else if ( current > n )
        {
            // something behind the deleted item was selected -> move up
            MacSetSelection( current - 1, true );
            MacSetSelection( current, false );
        }
    }

    // refresh all
    err = UpdateItems(
        kDataBrowserNoItem, 1, (UInt32*)kDataBrowserNoItem,
        kDataBrowserItemNoProperty, kDataBrowserItemNoProperty );
    verify_noerr( err );
}

void wxMacDataBrowserListControl::MacInsert( int n, const wxString& text)
{
    wxArrayInt selectionBefore;
    MacGetSelections( selectionBefore );

    UInt32 id = GetPeer()->GetCount(); // this has already been increased
    OSStatus err = AddItems( kDataBrowserNoItem, 1,  (UInt32*) &id, kDataBrowserItemNoProperty );
    verify_noerr( err );

    for ( int i = selectionBefore.GetCount() - 1; i >= 0; --i )
    {
        int current = selectionBefore[i];
        if ( current >= n )
        {
            MacSetSelection( current + 1, true );
            MacSetSelection( current, false );
        }
    }

    // refresh all
    err = UpdateItems(
        kDataBrowserNoItem, 1, (UInt32*)kDataBrowserNoItem,
        kDataBrowserItemNoProperty, kDataBrowserItemNoProperty );
    verify_noerr( err );
}

void wxMacDataBrowserListControl::MacInsert( int n, const wxArrayString& items)
{
    wxArrayInt selectionBefore;
    MacGetSelections( selectionBefore );
    size_t itemsCount = items.GetCount();

    UInt32 *ids = new UInt32[itemsCount];
    for ( unsigned int i = 0; i < itemsCount; ++i )
        ids[i] = GetPeer()->GetCount() - itemsCount + i + 1;

    OSStatus err = AddItems( kDataBrowserNoItem, itemsCount, ids, kDataBrowserItemNoProperty );
    verify_noerr( err );
    delete [] ids;

    for ( int i = selectionBefore.GetCount() - 1; i >= 0; --i )
    {
        int current = selectionBefore[i];
        if ( current >= n )
        {
            MacSetSelection( current + 1, true );
            MacSetSelection( current, false );
        }
    }

    // refresh all
    err = UpdateItems(
        kDataBrowserNoItem, 1, (UInt32*)kDataBrowserNoItem,
        kDataBrowserItemNoProperty, kDataBrowserItemNoProperty );
    verify_noerr( err );
}

void wxMacDataBrowserListControl::MacAppend( const wxString& text)
{
    UInt32 id = GetPeer()->GetCount(); // this has already been increased
    verify_noerr( AddItems( kDataBrowserNoItem, 1,  (UInt32*) &id, kDataBrowserItemNoProperty ) );
    // no need to deal with selections nor refreshed, as we have appended
}

void wxMacDataBrowserListControl::MacClear()
{
    verify_noerr( RemoveItems( kDataBrowserNoItem, 0, NULL, kDataBrowserItemNoProperty ) );
}

void wxMacDataBrowserListControl::MacDeselectAll()
{
    bool former = MacSuppressSelection( true );
    verify_noerr(SetSelectedItems( 0, NULL, kDataBrowserItemsRemove ) );
    MacSuppressSelection( former );
}

void wxMacDataBrowserListControl::MacSetSelection( int n, bool select )
{
    UInt32 id = n + 1;
    bool former = MacSuppressSelection( true );

    if ( IsItemSelected( id ) != select )
    {
        OSStatus err;

        if ( select )
            err = SetSelectedItems( 1, &id, GetPeer()->HasMultipleSelection() ? kDataBrowserItemsAdd : kDataBrowserItemsAssign );
        else
            err = SetSelectedItems( 1, &id, kDataBrowserItemsRemove );

        verify_noerr( err );
    }

    MacScrollTo( n );
    MacSuppressSelection( former );
}

bool  wxMacDataBrowserListControl::MacSuppressSelection( bool suppress )
{
    bool former = m_suppressSelection;
    m_suppressSelection = suppress;

    return former;
}

bool wxMacDataBrowserListControl::MacIsSelected( int n ) const
{
    return IsItemSelected( n + 1 );
}

int wxMacDataBrowserListControl::MacGetSelection() const
{
    for ( unsigned int i = 0; i < GetPeer()->GetCount(); ++i )
    {
        if ( IsItemSelected( i + 1 ) )
        {
            return i;
        }
    }

    return -1;
}

int wxMacDataBrowserListControl::MacGetSelections( wxArrayInt& aSelections ) const
{
    int no_sel = 0;

    aSelections.Empty();

    UInt32 first, last;
    GetSelectionAnchor( &first, &last );
    if ( first != kDataBrowserNoItem )
    {
        for ( size_t i = first; i <= last; ++i )
        {
            if ( IsItemSelected( i ) )
            {
                aSelections.Add( i - 1 );
                no_sel++;
            }
        }
    }

    return no_sel;
}

void wxMacDataBrowserListControl::MacSet( int n, const wxString& text )
{
    // as we don't store the strings we only have to issue a redraw
    UInt32 id = n + 1;
    verify_noerr( UpdateItems( kDataBrowserNoItem, 1, &id, kDataBrowserItemNoProperty, kDataBrowserItemNoProperty ) );
}

void wxMacDataBrowserListControl::MacScrollTo( int n )
{
    UInt32 id = n + 1;
    verify_noerr(  RevealItem( id, kTextColumnId, kDataBrowserRevealWithoutSelecting ) );
}

void wxMacDataBrowserListControl::UpdateLine( int n )
{
    UInt32 id = n + 1;
    verify_noerr( UpdateItems(kDataBrowserNoItem, 1, &id, kDataBrowserItemNoProperty, kDataBrowserItemNoProperty ) );
}

//
// Databrowser
//

OSStatus wxMacDataBrowserListControl::SetSelectionFlags( DataBrowserSelectionFlags options )
{
    return SetDataBrowserSelectionFlags( m_controlRef, options );
}

OSStatus wxMacDataBrowserListControl::AddListViewColumn( DataBrowserListViewColumnDesc *columnDesc,
        DataBrowserTableViewColumnIndex position )
{
    return AddDataBrowserListViewColumn( m_controlRef, columnDesc, position );
}

OSStatus wxMacDataBrowserListControl::AutoSizeListViewColumns()
{
    return AutoSizeDataBrowserListViewColumns(m_controlRef);
}

OSStatus wxMacDataBrowserListControl::SetHasScrollBars( bool horiz, bool vert )
{
    return SetDataBrowserHasScrollBars( m_controlRef, horiz, vert );
}

OSStatus wxMacDataBrowserListControl::SetTableViewHiliteStyle( DataBrowserTableViewHiliteStyle hiliteStyle )
{
    return SetDataBrowserTableViewHiliteStyle( m_controlRef, hiliteStyle );
}

OSStatus wxMacDataBrowserListControl::SetListViewHeaderBtnHeight(UInt16 height)
{
    return SetDataBrowserListViewHeaderBtnHeight( m_controlRef, height );
}

OSStatus wxMacDataBrowserListControl::SetCallbacks(const DataBrowserCallbacks *callbacks)
{
    return SetDataBrowserCallbacks( m_controlRef, callbacks );
}

OSStatus wxMacDataBrowserListControl::UpdateItems( 
    DataBrowserItemID container,
    UInt32 numItems,
    const DataBrowserItemID *items,
    DataBrowserPropertyID preSortProperty,
    DataBrowserPropertyID propertyID )
{
    return UpdateDataBrowserItems( m_controlRef, container, numItems, items, preSortProperty, propertyID );
}

bool wxMacDataBrowserListControl::IsItemSelected( DataBrowserItemID item ) const
{
    return IsDataBrowserItemSelected( m_controlRef, item );
}

OSStatus wxMacDataBrowserListControl::AddItems(
    DataBrowserItemID container,
    UInt32 numItems,
    const DataBrowserItemID *items,
    DataBrowserPropertyID preSortProperty )
{
    return AddDataBrowserItems( m_controlRef, container, numItems, items, preSortProperty );
}

OSStatus wxMacDataBrowserListControl::RemoveItems(
    DataBrowserItemID container,
    UInt32 numItems,
    const DataBrowserItemID *items,
    DataBrowserPropertyID preSortProperty )
{
    return RemoveDataBrowserItems( m_controlRef, container, numItems, items, preSortProperty );
}

OSStatus wxMacDataBrowserListControl::RevealItem(
    DataBrowserItemID item,
    DataBrowserPropertyID propertyID,
    DataBrowserRevealOptions options )
{
    return RevealDataBrowserItem( m_controlRef, item, propertyID, options );
}

OSStatus wxMacDataBrowserListControl::SetSelectedItems(
    UInt32 numItems,
    const DataBrowserItemID *items,
    DataBrowserSetOption operation )
{
    return SetDataBrowserSelectedItems( m_controlRef, numItems, items, operation );
}

OSStatus wxMacDataBrowserListControl::GetSelectionAnchor( DataBrowserItemID *first, DataBrowserItemID *last ) const
{
    return GetDataBrowserSelectionAnchor( m_controlRef, first, last );
}

#if 0

// in case we need that one day

// ============================================================================
// HIView owner-draw-based implementation
// ============================================================================

static pascal void ListBoxDrawProc(
    ControlRef browser, DataBrowserItemID item, DataBrowserPropertyID property,
    DataBrowserItemState itemState, const Rect *itemRect, SInt16 depth, Boolean isColorDevice )
{
    CFStringRef cfString;
    ThemeDrawingState themeState;
    long systemVersion;

    GetThemeDrawingState( &themeState );
    cfString = CFStringCreateWithFormat( NULL, NULL, CFSTR("Row %d"), item );

    //  In this sample we handle the "selected" state; all others fall through to our "active" state
    if ( itemState == kDataBrowserItemIsSelected )
    {
        ThemeBrush colorBrushID;

        // TODO: switch over to wxSystemSettingsNative::GetColour() when kThemeBrushSecondaryHighlightColor
        // is incorporated Panther DB starts using kThemeBrushSecondaryHighlightColor
        // for inactive browser highlighting
        Gestalt( gestaltSystemVersion, &systemVersion );
        if ( (systemVersion >= 0x00001030) && !IsControlActive( browser ) )
            colorBrushID = kThemeBrushSecondaryHighlightColor;
        else
            colorBrushID = kThemeBrushPrimaryHighlightColor;

        // First paint the hilite rect, then the text on top
        SetThemePen( colorBrushID, 32, true );
        PaintRect( itemRect );
        SetThemeDrawingState( themeState, false );
    }

    DrawThemeTextBox( cfString, kThemeApplicationFont, kThemeStateActive, true, itemRect, teFlushDefault, NULL );
    SetThemeDrawingState( themeState, true );

    if ( cfString != NULL )
        CFRelease( cfString );
}
#endif

// ============================================================================
// list box control implementation
// ============================================================================

wxListBox::wxListBox()
{
  m_noItems = 0;
}

bool wxListBox::Create(wxWindow *parent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    const wxArrayString& choices,
    long style,
    const wxValidator& validator,
    const wxString& name)
{
    wxCArrayString chs(choices);

    return Create(
        parent, id, pos, size, chs.GetCount(), chs.GetStrings(),
        style, validator, name);
}

bool wxListBox::Create(wxWindow *parent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    int n,
    const wxString choices[],
    long style,
    const wxValidator& validator,
    const wxString& name)
{
    m_macIsUserPane = false;

    wxASSERT_MSG( !(style & wxLB_MULTIPLE) || !(style & wxLB_EXTENDED),
                  _T("only one of listbox selection modes can be specified") );

    if ( !wxListBoxBase::Create(parent, id, pos, size, style & ~(wxHSCROLL | wxVSCROLL), validator, name) )
        return false;

   // this will be increased by our append command
    m_noItems = 0;

    m_peer = CreateMacListControl( pos, size, style );

    MacPostControlCreate( pos, size );

    InsertItems( n, choices, 0 );

   // Needed because it is a wxControlWithItems
    SetBestSize( size );

    return true;
}

wxListBox::~wxListBox()
{
    m_peer->SetReference( 0 );
    FreeData();
}

wxMacListControl * wxListBox::CreateMacListControl(const wxPoint& pos, const wxSize& size, long style)
{
    return new wxMacDataBrowserListControl( this, pos, size, style );
}

void wxListBox::FreeData()
{
#if wxUSE_OWNER_DRAWN
    if ( m_windowStyle & wxLB_OWNERDRAW )
    {
        size_t uiCount = m_aItems.Count();
        while ( uiCount != 0 )
        {
            uiCount--;
            delete m_aItems[uiCount];
            m_aItems[uiCount] = NULL;
        }

        m_aItems.Clear();
    }
    else
#endif
    if ( HasClientObjectData() )
    {
        for ( unsigned int n = 0; n < m_noItems; n++ )
        {
            delete GetClientObject( n );
        }
    }
}

void  wxListBox::DoSetSize(int x, int y,
            int width, int height,
            int sizeFlags )
{
    wxControl::DoSetSize( x, y, width, height, sizeFlags );
}

void wxListBox::DoSetFirstItem(int n)
{
    GetPeer()->MacScrollTo( n );
}

void wxListBox::Delete(unsigned int n)
{
    wxCHECK_RET( IsValid(n), wxT("invalid index in wxListBox::Delete") );

#if wxUSE_OWNER_DRAWN
    delete m_aItems[n];
    m_aItems.RemoveAt(n);
#else
    if ( HasClientObjectData() )
    {
        delete GetClientObject(n);
    }
#endif

    m_stringArray.RemoveAt( n );
    m_dataArray.RemoveAt( n );
    m_noItems--;

    GetPeer()->MacDelete( n );
}

int wxListBox::DoAppend(const wxString& item)
{
    InvalidateBestSize();

    unsigned int index = m_noItems;
    m_stringArray.Add( item );
    m_dataArray.Add( NULL );
    m_noItems++;
    DoSetItemClientData( index, NULL );
    GetPeer()->MacAppend( item );

    return index;
}

void wxListBox::DoSetItems(const wxArrayString& choices, void** clientData)
{
    Clear();
    unsigned int n = choices.GetCount();

    for ( size_t i = 0; i < n; ++i )
    {
        if ( clientData )
        {
#if wxUSE_OWNER_DRAWN
            wxASSERT_MSG(clientData[i] == NULL,
                wxT("Can't use client data with owner-drawn listboxes"));
#else
            Append( choices[i], clientData[i] );
#endif
        }
        else
            Append( choices[i] );
    }

#if wxUSE_OWNER_DRAWN
    if ( m_windowStyle & wxLB_OWNERDRAW )
    {
        // first delete old items
        size_t ui = m_aItems.Count();
        while ( ui != 0 )
        {
            ui--;
            delete m_aItems[ui];
            m_aItems[ui] = NULL;
        }

        m_aItems.Empty();

        // then create new ones
        for ( ui = 0; ui < (size_t)m_noItems; ui++ )
        {
            wxOwnerDrawn *pNewItem = CreateItem(ui);
            pNewItem->SetName(choices[ui]);
            m_aItems.Add(pNewItem);
        }
    }
#endif
}

int wxListBox::FindString(const wxString& s, bool bCase) const
{
    for ( size_t i = 0; i < m_noItems; ++ i )
    {
        if (s.IsSameAs(GetString(i), bCase))
            return (int)i;
    }

    return wxNOT_FOUND;
}

void wxListBox::Clear()
{
    FreeData();
    m_noItems = 0;
    m_stringArray.Empty();
    m_dataArray.Empty();
    GetPeer()->MacClear();
}

void wxListBox::DoSetSelection(int n, bool select)
{
    wxCHECK_RET( n == wxNOT_FOUND || IsValid(n),
        wxT("invalid index in wxListBox::SetSelection") );

    if ( n == wxNOT_FOUND )
        GetPeer()->MacDeselectAll();
    else
        GetPeer()->MacSetSelection( n, select );
}

bool wxListBox::IsSelected(int n) const
{
    wxCHECK_MSG( IsValid(n), false, wxT("invalid index in wxListBox::Selected") );

    return GetPeer()->MacIsSelected( n );
}

void *wxListBox::DoGetItemClientData(unsigned int n) const
{
    wxCHECK_MSG( IsValid(n), NULL, wxT("invalid index in wxListBox::GetClientData"));

    wxASSERT_MSG( m_dataArray.GetCount() >= (unsigned int) n, wxT("invalid client_data array") );

    return (void *)m_dataArray[n];
}

wxClientData *wxListBox::DoGetItemClientObject(unsigned int n) const
{
    return (wxClientData *) DoGetItemClientData( n );
}

void wxListBox::DoSetItemClientData(unsigned int n, void *clientData)
{
    wxCHECK_RET( IsValid(n), wxT("invalid index in wxListBox::SetClientData") );

#if wxUSE_OWNER_DRAWN
    if ( m_windowStyle & wxLB_OWNERDRAW )
    {
        // client data must be pointer to wxOwnerDrawn, otherwise we would crash
        // in OnMeasure/OnDraw.
        wxFAIL_MSG(wxT("Can't use client data with owner-drawn listboxes"));
    }
#endif

    wxASSERT_MSG( m_dataArray.GetCount() >= (unsigned int) n, wxT("invalid client_data array") );

    if ( m_dataArray.GetCount() > (unsigned int) n )
        m_dataArray[n] = (char*)clientData;
    else
        m_dataArray.Add( (char*)clientData );
}

void wxListBox::DoSetItemClientObject(unsigned int n, wxClientData* clientData)
{
    DoSetItemClientData(n, clientData);
}

// Return number of selections and an array of selected integers
int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    return GetPeer()->MacGetSelections( aSelections );
}

// Get single selection, for single choice list items
int wxListBox::GetSelection() const
{
    return GetPeer()->MacGetSelection();
}

// Find string for position
wxString wxListBox::GetString(unsigned int n) const
{
    wxCHECK_MSG( IsValid(n), wxEmptyString, wxT("invalid index in wxListBox::GetString") );

    return m_stringArray[n];
}

void wxListBox::DoInsertItems(const wxArrayString& items, unsigned int pos)
{
    wxCHECK_RET( IsValidInsert(pos), wxT("invalid index in wxListBox::InsertItems") );

    InvalidateBestSize();

    unsigned int nItems = items.GetCount();

    for ( unsigned int i = 0; i < nItems; i++ )
        m_stringArray.Insert( items[i], pos + i );
    m_dataArray.Insert( NULL, pos, nItems );
    m_noItems += nItems;
    GetPeer()->MacInsert( pos, items );
}

void wxListBox::SetString(unsigned int n, const wxString& s)
{
    m_stringArray[n] = s;
    GetPeer()->MacSet( n, s );
}

wxSize wxListBox::DoGetBestSize() const
{
    int lbWidth = 100;  // some defaults
    int lbHeight = 110;
    int wLine;

    {
        wxMacPortStateHelper st( UMAGetWindowPort( (WindowRef)MacGetTopLevelWindowRef() ) );

        // TODO: clean this up
        if ( m_font.Ok() )
        {
            ::TextFont( m_font.MacGetFontNum() );
            ::TextSize( m_font.MacGetFontSize() );
            ::TextFace( m_font.MacGetFontStyle() );
        }
        else
        {
            ::TextFont( kFontIDMonaco );
            ::TextSize( 9  );
            ::TextFace( 0 );
        }

        // Find the widest line
        for (unsigned int i = 0; i < GetCount(); i++)
        {
            wxString str(GetString(i));

#if wxUSE_UNICODE
            Point bounds = {0, 0};
            SInt16 baseline;

            // NB: what if m_font.Ok() == false ???
            ::GetThemeTextDimensions(
                wxMacCFStringHolder( str, m_font.GetEncoding() ),
                kThemeCurrentPortFont,
                kThemeStateActive,
                false,
                &bounds,
                &baseline );
            wLine = bounds.h;
#else
            wLine = ::TextWidth( str.c_str(), 0, str.length() );
#endif

            lbWidth = wxMax( lbWidth, wLine );
        }

        // Add room for the scrollbar
        lbWidth += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

        // And just a bit more
        int cy = 12;
        int cx = ::TextWidth( "X", 0, 1 );
        lbWidth += cx;

        // don't make the listbox too tall (limit height to around 10 items)
        // but don't make it too small neither
        lbHeight = wxMax( (cy + 4) * wxMin( wxMax( GetCount(), 3 ), 10 ), 70 );
    }

    return wxSize( lbWidth, lbHeight );
}

unsigned int wxListBox::GetCount() const
{
    return m_noItems;
}

void wxListBox::Refresh(bool eraseBack, const wxRect *rect)
{
    wxControl::Refresh( eraseBack, rect );
}

void wxListBox::MacUpdateLine(int n)
{
    GetPeer()->UpdateLine(n);
}

#if wxUSE_OWNER_DRAWN

class wxListBoxItem : public wxOwnerDrawn
{
public:
    wxListBoxItem(const wxString& str = "");
};

wxListBoxItem::wxListBoxItem(const wxString& str)
    : wxOwnerDrawn(str, false)
{
    // no bitmaps/checkmarks
    SetMarginWidth(0);
}

wxOwnerDrawn *wxListBox::CreateItem(size_t n)
{
    return new wxListBoxItem();
}

#endif // USE_OWNER_DRAWN


// Some custom controls depend on this
/* static */ wxVisualAttributes
wxListBox::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    wxVisualAttributes attr;

    attr.colFg = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT );
    attr.colBg = wxSystemSettings::GetColour( wxSYS_COLOUR_LISTBOX );
    attr.font = wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT );

    return attr;
}

int wxListBox::DoListHitTest(const wxPoint& inpoint) const
{
    OSStatus err;

    // There are few reasons why this is complicated:
    // 1) There is no native HitTest function for Mac
    // 2) GetDataBrowserItemPartBounds only works on visible items
    // 3) We can't do it through GetDataBrowserTableView[Item]RowHeight
    //    because what it returns is basically inaccurate in the context
    //    of the coordinates we want here, but we use this as a guess
    //    for where the first visible item lies

    wxPoint point = inpoint;

    // interestingly enough 10.2 (and below?) have GetDataBrowserItemPartBounds
    // giving root window coordinates but 10.3 and above give client coordinates
    // so we only compare using root window coordinates on 10.3 and up
    if ( UMAGetSystemVersion() < 0x1030 )
        MacClientToRootWindow(&point.x, &point.y);

    // get column property ID (req. for call to itempartbounds)
    DataBrowserTableViewColumnID colId = 0;
    err = GetDataBrowserTableViewColumnProperty(m_peer->GetControlRef(), 0, &colId);
    wxCHECK_MSG(err == noErr, wxNOT_FOUND, wxT("Unexpected error from GetDataBrowserTableViewColumnProperty"));

    // OK, first we need to find the first visible item we have -
    // this will be the "low" for our binary search. There is no real
    // easy way around this, as we will need to do a SLOW linear search
    // until we find a visible item, but we can do a cheap calculation
    // via the row height to speed things up a bit
    UInt32 scrollx, scrolly;
    err = GetDataBrowserScrollPosition(m_peer->GetControlRef(), &scrollx, &scrolly);
    wxCHECK_MSG(err == noErr, wxNOT_FOUND, wxT("Unexpected error from GetDataBrowserScrollPosition"));

    UInt16 height;
    err = GetDataBrowserTableViewRowHeight(m_peer->GetControlRef(), &height);
    wxCHECK_MSG(err == noErr, wxNOT_FOUND, wxT("Unexpected error from GetDataBrowserTableViewRowHeight"));

    // these indices are 0-based, as usual, so we need to add 1 to them when
    // passing them to data browser functions which use 1-based indices
    int low = scrolly / height,
        high = GetCount() - 1;

    // search for the first visible item (note that the scroll guess above
    // is the low bounds of where the item might lie so we only use that as a
    // starting point - we should reach it within 1 or 2 iterations of the loop)
    while ( low <= high )
    {
        Rect bounds;
        err = GetDataBrowserItemPartBounds(
            m_peer->GetControlRef(), low + 1, colId,
            kDataBrowserPropertyEnclosingPart,
            &bounds); // note +1 to translate to Mac ID
        if ( err == noErr )
            break;

        // errDataBrowserItemNotFound is expected as it simply means that the
        // item is not currently visible -- but other errors are not
        wxCHECK_MSG( err == errDataBrowserItemNotFound, wxNOT_FOUND,
                     wxT("Unexpected error from GetDataBrowserItemPartBounds") );

        low++;
    }

    // NOW do a binary search for where the item lies, searching low again if
    // we hit an item that isn't visible
    while ( low <= high )
    {
        int mid = (low + high) / 2;

        Rect bounds;
        err = GetDataBrowserItemPartBounds(
            m_peer->GetControlRef(), mid + 1, colId,
            kDataBrowserPropertyEnclosingPart,
            &bounds); //note +1 to trans to mac id
        wxCHECK_MSG( err == noErr || err == errDataBrowserItemNotFound,
                     wxNOT_FOUND,
                     wxT("Unexpected error from GetDataBrowserItemPartBounds") );

        if ( err == errDataBrowserItemNotFound )
        {
            // item not visible, attempt to find a visible one
            // search lower
            high = mid - 1;
        }
        else // visible item, do actual hitttest
        {
            // if point is within the bounds, return this item (since we assume
            // all x coords of items are equal we only test the x coord in
            // equality)
            if ((point.x >= bounds.left && point.x <= bounds.right) &&
                (point.y >= bounds.top && point.y <= bounds.bottom) )
            {
                // found!
                return mid;
            }

            if ( point.y < bounds.top )
                // index(bounds) greater then key(point)
                high = mid - 1;
            else
                // index(bounds) less then key(point)
                low = mid + 1;
        }
    }

    return wxNOT_FOUND;
}

#endif

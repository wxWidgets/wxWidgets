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

#include "wx/listbox.h"

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/button.h"
    #include "wx/settings.h"
    #include "wx/arrstr.h"
    #include "wx/toplevel.h"
#endif

IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxControl)

BEGIN_EVENT_TABLE(wxListBox, wxControl)
END_EVENT_TABLE()

#include "wx/mac/uma.h"
#include "wx/dynarray.h"

// ============================================================================
// list box control implementation
// ============================================================================

wxListBox::wxListBox()
{
}

bool wxListBox::Create(
    wxWindow *parent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    const wxArrayString& choices,
    long style,
    const wxValidator& validator,
    const wxString& name )
{
    wxCArrayString chs(choices);

    return Create(
        parent, id, pos, size, chs.GetCount(), chs.GetStrings(),
        style, validator, name );
}

wxMacListControl* wxListBox::GetPeer() const
{
    return dynamic_cast<wxMacListControl*>(m_peer);
}

bool wxListBox::Create(
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
                  wxT("only a single listbox selection mode can be specified") );

    if ( !wxListBoxBase::Create( parent, id, pos, size, style & ~(wxHSCROLL | wxVSCROLL), validator, name ) )
        return false;

    wxMacDataBrowserListControl* control = new wxMacDataBrowserListControl( this, pos, size, style );
    control->SetClientDataType( m_clientDataItemsType );
    m_peer = control;

    MacPostControlCreate( pos, size );

    InsertItems( n, choices, 0 );

   // Needed because it is a wxControlWithItems
    SetBestSize( size );

    return true;
}

wxListBox::~wxListBox()
{
    FreeData();
    m_peer->SetReference( 0 );
}

void wxListBox::FreeData()
{
    GetPeer()->MacClear();
}

void wxListBox::DoSetFirstItem(int n)
{
    GetPeer()->MacScrollTo( n );
}

void wxListBox::EnsureVisible(int n)
{
    GetPeer()->MacScrollTo( n );
}

void wxListBox::Delete(unsigned int n)
{
    wxCHECK_RET( IsValid(n), wxT("invalid index in wxListBox::Delete") );

    GetPeer()->MacDelete( n );
}

int wxListBox::DoAppend(const wxString& item)
{
    InvalidateBestSize();

    return GetPeer()->MacAppend( item );
}

void wxListBox::DoSetItems(const wxArrayString& choices, void** clientData)
{
    Clear();

    unsigned int n = choices.GetCount();

    for ( size_t i = 0; i < n; ++i )
    {
        if ( clientData )
        {
            Append( choices[i], clientData[i] );
        }
        else
            Append( choices[i] );
    }

}

int wxListBox::FindString(const wxString& s, bool bCase) const
{
    for ( size_t i = 0; i < GetCount(); ++ i )
    {
        if (s.IsSameAs( GetString( i ), bCase) )
            return (int)i;
    }

    return wxNOT_FOUND;
}

void wxListBox::Clear()
{
    FreeData();
}

void wxListBox::DoSetSelection(int n, bool select)
{
    wxCHECK_RET( n == wxNOT_FOUND || IsValid(n),
        wxT("invalid index in wxListBox::SetSelection") );

    if ( n == wxNOT_FOUND )
        GetPeer()->MacDeselectAll();
    else
        GetPeer()->MacSetSelection( n, select, HasMultipleSelection() );
}

bool wxListBox::IsSelected(int n) const
{
    wxCHECK_MSG( IsValid(n), false, wxT("invalid index in wxListBox::Selected") );

    return GetPeer()->MacIsSelected( n );
}

void *wxListBox::DoGetItemClientData(unsigned int n) const
{
    wxCHECK_MSG( IsValid(n), NULL, wxT("invalid index in wxListBox::GetClientData"));
    return GetPeer()->MacGetClientData( n );
}

wxClientData *wxListBox::DoGetItemClientObject(unsigned int n) const
{
    return (wxClientData*)DoGetItemClientData( n );
}

void wxListBox::DoSetItemClientData(unsigned int n, void *clientData)
{
    wxCHECK_RET( IsValid(n), wxT("invalid index in wxListBox::SetClientData") );
    GetPeer()->MacSetClientData( n , clientData);
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
    return GetPeer()->MacGetString(n);
}

void wxListBox::DoInsertItems(const wxArrayString& items, unsigned int pos)
{
    wxCHECK_RET( IsValidInsert(pos), wxT("invalid index in wxListBox::InsertItems") );

    InvalidateBestSize();

    GetPeer()->MacInsert( pos, items );
}

void wxListBox::SetString(unsigned int n, const wxString& s)
{
    GetPeer()->MacSetString( n, s );
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
            ::TextSize( 9 );
            ::TextFace( 0 );
        }

        // Find the widest line
        for (unsigned int i = 0; i < GetCount(); i++)
        {
            wxString str( GetString( i ) );

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
        lbWidth += wxSystemSettings::GetMetric( wxSYS_VSCROLL_X );

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
    return GetPeer()->MacGetCount();
}

void wxListBox::Refresh(bool eraseBack, const wxRect *rect)
{
    wxControl::Refresh( eraseBack, rect );
}

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

// ============================================================================
// data browser based implementation
// ============================================================================

wxMacListBoxItem::wxMacListBoxItem()
        :wxMacDataItem()
{
}

wxMacListBoxItem::~wxMacListBoxItem()
{
}

void wxMacListBoxItem::Notification(wxMacDataItemBrowserControl *owner ,
    DataBrowserItemNotification message,
    DataBrowserItemDataRef itemData ) const
{
    wxMacDataBrowserListControl *lb = dynamic_cast<wxMacDataBrowserListControl*>(owner);

    // we want to depend on as little as possible to make sure tear-down of controls is safe

    if ( message == kDataBrowserItemRemoved)
    {
        if ( lb != NULL && lb->GetClientDataType() == wxClientData_Object )
        {
            delete (wxClientData*) (m_data);
        }

        delete this;
        return;
    }

    wxListBox *list = wxDynamicCast( owner->GetPeer() , wxListBox );
    wxCHECK_RET( list != NULL , wxT("Listbox expected"));

    bool trigger = false;
    wxCommandEvent event( wxEVT_COMMAND_LISTBOX_SELECTED, list->GetId() );
    switch (message)
    {
        case kDataBrowserItemDeselected:
            if ( list->HasMultipleSelection() )
                trigger = !lb->IsSelectionSuppressed();
            break;

        case kDataBrowserItemSelected:
            trigger = !lb->IsSelectionSuppressed();
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
            event.SetClientObject( (wxClientData*) m_data );
        else if ( list->HasClientUntypedData() )
            event.SetClientData( m_data );
        event.SetString( m_label );
        event.SetInt( owner->GetLineFromItem( this ) );
        event.SetExtraLong( list->HasMultipleSelection() ? message == kDataBrowserItemSelected : true );

        // direct notification is not always having the listbox GetSelection()
        // having in synch with event, so use wxPostEvent instead
        // list->GetEventHandler()->ProcessEvent(event);

        wxPostEvent( list->GetEventHandler(), event );
    }
}

wxMacDataBrowserListControl::wxMacDataBrowserListControl( wxWindow *peer, const wxPoint& pos, const wxSize& size, long style)
    : wxMacDataItemBrowserControl( peer, pos, size, style )
{
    OSStatus err = noErr;
    m_clientDataItemsType = wxClientData_None;
    if ( style & wxLB_SORT )
        m_sortOrder = SortOrder_Text_Ascending;

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

    columnDesc.headerBtnDesc.minimumWidth = 0;
    columnDesc.headerBtnDesc.maximumWidth = 10000;

    columnDesc.propertyDesc.propertyID = kTextColumnId;
    columnDesc.propertyDesc.propertyType = kDataBrowserTextType;
    columnDesc.propertyDesc.propertyFlags = kDataBrowserTableViewSelectionColumn;
#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2
    columnDesc.propertyDesc.propertyFlags |= kDataBrowserListViewTypeSelectColumn;
#endif

    verify_noerr( AddColumn( &columnDesc, kDataBrowserListViewAppendColumn ) );

    columnDesc.headerBtnDesc.minimumWidth = 0;
    columnDesc.headerBtnDesc.maximumWidth = 0;
    columnDesc.propertyDesc.propertyID = kNumericOrderColumnId;
    columnDesc.propertyDesc.propertyType = kDataBrowserPropertyRelevanceRankPart;
    columnDesc.propertyDesc.propertyFlags = kDataBrowserTableViewSelectionColumn;
#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_2
    columnDesc.propertyDesc.propertyFlags |= kDataBrowserListViewTypeSelectColumn;
#endif

    verify_noerr( AddColumn( &columnDesc, kDataBrowserListViewAppendColumn ) );

    SetDataBrowserSortProperty( m_controlRef , kTextColumnId);
    if ( m_sortOrder == SortOrder_Text_Ascending )
    {
        SetDataBrowserSortProperty( m_controlRef , kTextColumnId);
        SetDataBrowserSortOrder( m_controlRef , kDataBrowserOrderIncreasing);
    }
    else
    {
        SetDataBrowserSortProperty( m_controlRef , kNumericOrderColumnId);
        SetDataBrowserSortOrder( m_controlRef , kDataBrowserOrderIncreasing);
    }

    verify_noerr( AutoSizeColumns() );
    verify_noerr( SetHiliteStyle(kDataBrowserTableViewFillHilite ) );
    verify_noerr( SetHeaderButtonHeight( 0 ) );
    err = SetHasScrollBars( (style & wxHSCROLL) != 0 , true );
#if 0
    // shouldn't be necessary anymore under 10.2
    m_peer->SetData( kControlNoPart, kControlDataBrowserIncludesFrameAndFocusTag, (Boolean)false );
    m_peer->SetNeedsFocusRect( true );
#endif
}

wxMacDataBrowserListControl::~wxMacDataBrowserListControl()
{
}

wxWindow * wxMacDataBrowserListControl::GetPeer() const
{
    return wxDynamicCast( wxMacControl::GetPeer() , wxWindow );
}

wxMacDataItem* wxMacDataBrowserListControl::CreateItem()
{
    return new wxMacListBoxItem();
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


#endif // wxUSE_LISTBOX

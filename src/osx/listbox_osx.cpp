///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/listbox_osx.cpp
// Purpose:     wxListBox
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_LISTBOX

#include "wx/listbox.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/utils.h"
    #include "wx/settings.h"
    #include "wx/arrstr.h"
    #include "wx/dcclient.h"
#endif

wxBEGIN_EVENT_TABLE(wxListBox, wxControl)
wxEND_EVENT_TABLE()

#include "wx/osx/private.h"

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

wxListWidgetImpl* wxListBox::GetListPeer() const
{
    wxListWidgetImpl* impl =  dynamic_cast<wxListWidgetImpl*> ( GetPeer() );
    return impl;
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
    DontCreatePeer();
    m_blockEvents = false;

    if ( ! (style & wxNO_BORDER) )
        style = (style & ~wxBORDER_MASK) | wxSUNKEN_BORDER ;

    wxASSERT_MSG( !(style & wxLB_MULTIPLE) || !(style & wxLB_EXTENDED),
                  wxT("only a single listbox selection mode can be specified") );

    if ( !wxListBoxBase::Create( parent, id, pos, size, style & ~(wxHSCROLL | wxVSCROLL), validator, name ) )
        return false;

    if ( IsSorted() )
        m_strings.sorted = new wxSortedArrayString(wxDictionaryStringSortAscending);
    else
        m_strings.unsorted = new wxArrayString;

    SetPeer(wxWidgetImpl::CreateListBox( this, parent, id, pos, size, style, GetExtraStyle() ));

    MacPostControlCreate( pos, size );

    m_textColumn = GetListPeer()->InsertTextColumn(0,wxEmptyString);

    Append(n, choices);

   // Needed because it is a wxControlWithItems
    SetInitialSize( size );

    return true;
}

wxListBox::~wxListBox()
{
    m_blockEvents = true;
    FreeData();
    m_blockEvents = false;

    // make sure no native events get sent to a object in destruction
    SetPeer(nullptr);

    if ( IsSorted() )
        delete m_strings.sorted;
    else
        delete m_strings.unsorted;

    m_strings.sorted = nullptr;
}

void wxListBox::FreeData()
{
    if ( IsSorted() )
        m_strings.sorted->Clear();
    else
        m_strings.unsorted->Clear();

    m_itemsClientData.Clear();

    GetListPeer()->ListClear();
}

void wxListBox::DoSetFirstItem(int n)
{
    // osx actually only has an implementation for ensuring the visibility of a row, it does so  
    // by scrolling the minimal amount necessary from the current scrolling position.
    // in order to get the same behaviour I'd have to make sure first that the last line is visible, 
    // followed by a scrollRowToVisible for the desired line 
    GetListPeer()->ListScrollTo( GetCount()-1 );
    GetListPeer()->ListScrollTo( n );
}

void wxListBox::EnsureVisible(int n)
{
    GetListPeer()->ListScrollTo( n );
}

int wxListBox::GetTopItem() const
{
    return GetListPeer()->ListGetTopItem();
}

int wxListBox::GetCountPerPage() const
{
    return GetListPeer()->ListGetCountPerPage();
}

void wxListBox::DoDeleteOneItem(unsigned int n)
{
    wxCHECK_RET( IsValid(n), wxT("invalid index in wxListBox::Delete") );

    m_blockEvents = true;
    if ( IsSorted() )
        m_strings.sorted->RemoveAt(n);
    else
        m_strings.unsorted->RemoveAt(n);

    m_itemsClientData.RemoveAt(n);

    GetListPeer()->ListDelete( n );
    m_blockEvents = false;

    UpdateOldSelections();
}

void wxListBox::DoClear()
{
    m_blockEvents = true;
    FreeData();
    m_blockEvents = false;

    UpdateOldSelections();
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

void wxListBox::DoSetSelection(int n, bool select)
{
    wxCHECK_RET( n == wxNOT_FOUND || IsValid(n),
        wxT("invalid index in wxListBox::SetSelection") );

    DoSetSelectionWithoutEnsureVisible(n, select);

    if (select)
        EnsureVisible(n);
}

void wxListBox::DoSetSelectionWithoutEnsureVisible(int n, bool select)
{
    m_blockEvents = true;

    if ( n == wxNOT_FOUND )
        GetListPeer()->ListDeselectAll();
    else
        GetListPeer()->ListSetSelection( n, select, HasMultipleSelection() );

    m_blockEvents = false;

    UpdateOldSelections();
}

bool wxListBox::IsSelected(int n) const
{
    wxCHECK_MSG( IsValid(n), false, wxT("invalid index in wxListBox::Selected") );

    return GetListPeer()->ListIsSelected( n );
}

// Return number of selections and an array of selected integers
int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    return GetListPeer()->ListGetSelections( aSelections );
}

// Get single selection, for single choice list items
int wxListBox::GetSelection() const
{
    return GetListPeer()->ListGetSelection();
}

int wxListBox::DoListHitTest(const wxPoint& inpoint) const
{
    return GetListPeer()->DoListHitTest( inpoint );
}

// ----------------------------------------------------------------------------
// display
// ----------------------------------------------------------------------------

void wxListBox::GetValueCallback( unsigned int n, wxListWidgetColumn* col , wxListWidgetCellValue& value )
{
    if ( col == m_textColumn )
        value.Set( GetString( n ) );
}

void wxListBox::SetValueCallback( unsigned int WXUNUSED(n), wxListWidgetColumn* WXUNUSED(col) , wxListWidgetCellValue& WXUNUSED(value) )
{
}

wxSize wxListBox::DoGetBestSize() const
{
    int lbWidth = 100;  // some defaults
    int lbHeight;

    {
        wxClientDC dc(const_cast<wxListBox*>(this));
        dc.SetFont(GetFont());

        // Find the widest line
        for (unsigned int i = 0; i < GetCount(); i++)
        {
            wxString str( GetString( i ) );

            wxCoord width, height ;
            dc.GetTextExtent( str , &width, &height);
            lbWidth = wxMax( lbWidth, width );
        }

        // Add room for the scrollbar
        lbWidth += wxSystemSettings::GetMetric( wxSYS_VSCROLL_X );

        // And just a bit more
        int cy = 12;

        wxCoord width, height ;
        dc.GetTextExtent( wxT("XX") , &width, &height);
        int cx = width ;
        lbWidth += cx;

        // don't make the listbox too tall (limit height to around 10 items)
        // but don't make it too small either
        lbHeight = wxMax( (cy + 4) * wxMin( wxMax( GetCount(), 3 ), 10 ), 70 );
    }

    return wxSize( lbWidth, lbHeight );
}

// Some custom controls depend on this
/* static */ wxVisualAttributes
wxListBox::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    wxVisualAttributes attr;

    attr.colFg = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT );
    attr.colBg = wxSystemSettings::GetColour( wxSYS_COLOUR_LISTBOX );
    static wxFont font = wxFont(wxOSX_SYSTEM_FONT_VIEWS);
    attr.font = font;

    return attr;
}

// below is all code copied from univ

// ----------------------------------------------------------------------------
// client data handling
// ----------------------------------------------------------------------------

void wxListBox::DoSetItemClientData(unsigned int n, void* clientData)
{
    m_itemsClientData[n] = clientData;
}

void *wxListBox::DoGetItemClientData(unsigned int n) const
{
    return m_itemsClientData[n];
}

// ----------------------------------------------------------------------------
// accessing strings
// ----------------------------------------------------------------------------

unsigned int wxListBox::GetCount() const
{
    return IsSorted() ? m_strings.sorted->size()
                      : m_strings.unsorted->size();
}

wxString wxListBox::GetString(unsigned int n) const
{
    return IsSorted() ? m_strings.sorted->Item(n)
                      : m_strings.unsorted->Item(n);
}

int wxListBox::FindString(const wxString& s, bool bCase) const
{
    return IsSorted() ? m_strings.sorted->Index(s, bCase)
                      : m_strings.unsorted->Index(s, bCase);
}

// ----------------------------------------------------------------------------
// adding/inserting strings
// ----------------------------------------------------------------------------

void wxListBox::OnItemInserted(unsigned int WXUNUSED(pos))
{
}

int wxListBox::DoInsertItems(const wxArrayStringsAdapter& items,
                             unsigned int pos,
                             void **clientData,
                             wxClientDataType type)
{
    int idx = wxNOT_FOUND;
    unsigned int startpos = pos;
    wxArrayInt selections;

    if ( !IsSorted() )
    {
        if ( HasMultipleSelection() )
        {
            GetSelections(selections);
        }
        else
        {
            int sel = GetSelection();
            if ( sel != wxNOT_FOUND )
            {
                selections.Add(sel);
            }
        }
    }

    const unsigned int numItems = items.GetCount();
    for ( unsigned int i = 0; i < numItems; ++i )
    {
        const wxString& item = items[i];
        idx = IsSorted() ? m_strings.sorted->Add(item)
                         : (m_strings.unsorted->Insert(item, pos), pos++);

        m_itemsClientData.Insert(nullptr, idx);
        AssignNewItemClientData(idx, clientData, i, type);

        GetListPeer()->ListInsert(startpos+i);

        OnItemInserted(idx);
    }

    GetListPeer()->UpdateLineToEnd(startpos);

    const size_t numSelections = selections.size();
    for ( size_t i = 0; i < numSelections; ++i )
    {
        if ( int(startpos) <= selections[i] )
        {
            if ( HasMultipleSelection() )
            {
                size_t j;

                // Do not deselect item if it is to be selected below
                for ( j = 0; j < numSelections; ++j )
                {
                    if ( selections[i] == selections[j] + int(numItems) )
                        break;
                }

                if ( j == numSelections )
                    Deselect(selections[i]);
            }

            DoSetSelectionWithoutEnsureVisible(selections[i] + numItems, true);
        }
    }

    InvalidateBestSize();

    UpdateOldSelections();

    return idx;
}

void wxListBox::SetString(unsigned int n, const wxString& s)
{
    wxCHECK_RET( !IsSorted(), wxT("can't set string in sorted listbox") );

    if ( IsSorted() )
        (*m_strings.sorted)[n] = s;
    else
        (*m_strings.unsorted)[n] = s;

    GetListPeer()->UpdateLine(n);
}

//
// common event handling
//

void wxListBox::HandleLineEvent( unsigned int n, bool doubleClick )
{
    wxCommandEvent event( doubleClick ? wxEVT_LISTBOX_DCLICK :
        wxEVT_LISTBOX, GetId() );
    event.SetEventObject( this );
    if ( HasClientObjectData() )
        event.SetClientObject( GetClientObject(n) );
    else if ( HasClientUntypedData() )
        event.SetClientData( GetClientData(n) );
    event.SetString( GetString(n) );
    event.SetInt( n );
    event.SetExtraLong( 1 );
    HandleWindowEvent(event);
}

void wxListBox::MacHandleSelectionChange(int row)
{
    if ( m_blockEvents )
        return;

    // Correct notification events for multiselection list.
    if ( HasMultipleSelection() )
    {
        CalcAndSendEvent();
        return;
    }

    // OS X can select an item below the last item. In that case keep the old
    // selection because in wxWidgets API there is no notification event for
    // removing the selection from a single-selection list box.
    //
    // Otherwise call DoChangeSingleSelection so that m_oldSelections is
    // updated with the correct value before it's possible used later.
    const int count = static_cast<int>(GetCount());
    if ( row < 0 || row >= count )
    {
        if ( !m_oldSelections.empty() )
        {
            const int oldsel = m_oldSelections[0];
            if ( oldsel >= 0 && oldsel < count )
                SetSelection(oldsel);
        }
    }
    else if ( DoChangeSingleSelection(row) )
    {
        HandleLineEvent( row, false );
    }
}

//
// common list cell value operations
//

void wxListWidgetCellValue::Check( bool check )
{
    Set( check ? 1 : 0 );
}

bool wxListWidgetCellValue::IsChecked() const
{
    return GetIntValue() != 0;
}



#endif // wxUSE_LISTBOX

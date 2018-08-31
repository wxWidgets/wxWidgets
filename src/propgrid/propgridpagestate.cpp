/////////////////////////////////////////////////////////////////////////////
// Name:        src/propgrid/propgridpagestate.cpp
// Purpose:     wxPropertyGridPageState class
// Author:      Jaakko Salli
// Modified by:
// Created:     2008-08-24
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_PROPGRID

#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/object.h"
    #include "wx/hash.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/event.h"
    #include "wx/window.h"
    #include "wx/panel.h"
    #include "wx/dc.h"
    #include "wx/dcmemory.h"
    #include "wx/pen.h"
    #include "wx/brush.h"
    #include "wx/intl.h"
    #include "wx/stopwatch.h"
#endif

// This define is necessary to prevent macro clearing
#define __wxPG_SOURCE_FILE__

#include "wx/propgrid/propgridpagestate.h"
#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/editors.h"

#define wxPG_DEFAULT_SPLITTERX      110


// -----------------------------------------------------------------------
// wxPropertyGridIterator
// -----------------------------------------------------------------------

void wxPropertyGridIteratorBase::Init( wxPropertyGridPageState* state, int flags, wxPGProperty* property, int dir  )
{
    wxASSERT( dir == 1 || dir == -1 );

    m_state = state;
    m_baseParent = state->DoGetRoot();
    if ( !property && m_baseParent->GetChildCount() )
        property = m_baseParent->Item(0);

    m_property = property;

    wxPG_ITERATOR_CREATE_MASKS(flags, m_itemExMask, m_parentExMask)

    // Need to skip first?
    if ( property && property->HasFlag(m_itemExMask) )
    {
        if ( dir == 1 )
            Next();
        else
            Prev();
    }
}

void wxPropertyGridIteratorBase::Init( wxPropertyGridPageState* state, int flags, int startPos, int dir  )
{
    wxPGProperty* property = NULL;

    if ( startPos == wxTOP )
    {
        if ( dir == 0 )
            dir = 1;
    }
    else if ( startPos == wxBOTTOM )
    {
        property = state->GetLastItem(flags);
        if ( dir == 0 )
            dir = -1;
    }
    else
    {
        wxFAIL_MSG("Only supported starting positions are wxTOP and wxBOTTOM");
    }

    Init( state, flags, property, dir );
}

void wxPropertyGridIteratorBase::Assign( const wxPropertyGridIteratorBase& it )
{
    m_property = it.m_property;
    m_state = it.m_state;
    m_baseParent = it.m_baseParent;
    m_itemExMask = it.m_itemExMask;
    m_parentExMask = it.m_parentExMask;
}

void wxPropertyGridIteratorBase::Prev()
{
    wxPGProperty* property = m_property;
    if ( !property )
        return;

    wxPGProperty* parent = property->GetParent();
    wxASSERT( parent );
    unsigned int index = property->GetIndexInParent();

    if ( index > 0 )
    {
        // Previous sibling
        index--;

        property = parent->Item(index);

        // Go to last children?
        if ( property->GetChildCount() &&
             wxPG_ITERATOR_PARENTEXMASK_TEST(property, m_parentExMask) )
        {
            // First child
            property = property->Last();
        }
    }
    else
    {
        // Up to a parent
        if ( parent == m_baseParent )
        {
            m_property = NULL;
            return;
        }
        else
        {
            property = parent;
        }
    }

    m_property = property;

    // If property does not match our criteria, skip it
    if ( property->HasFlag(m_itemExMask) )
        Prev();
}

void wxPropertyGridIteratorBase::Next( bool iterateChildren )
{
    wxPGProperty* property = m_property;
    if ( !property )
        return;

    if ( property->GetChildCount() &&
         wxPG_ITERATOR_PARENTEXMASK_TEST(property, m_parentExMask) &&
         iterateChildren )
    {
        // First child
        property = property->Item(0);
    }
    else
    {
        wxPGProperty* parent = property->GetParent();
        wxASSERT( parent );
        unsigned int index = property->GetIndexInParent() + 1;

        if ( index < parent->GetChildCount() )
        {
            // Next sibling
            property = parent->Item(index);
        }
        else
        {
            // Next sibling of parent
            if ( parent == m_baseParent )
            {
                m_property = NULL;
            }
            else
            {
                m_property = parent;
                Next(false);
            }
            return;
        }
    }

    m_property = property;

    // If property does not match our criteria, skip it
    if ( property->HasFlag(m_itemExMask) )
        Next();
}

// -----------------------------------------------------------------------
// wxPropertyGridPageState
// -----------------------------------------------------------------------

wxPropertyGridPageState::wxPropertyGridPageState()
{
    m_pPropGrid = NULL;
    m_regularArray.SetParentState(this);
    m_properties = &m_regularArray;
    m_abcArray = NULL;
    m_currentCategory = NULL;
    m_width = 0;
    m_virtualHeight = 0;
    m_lastCaptionBottomnest = true;
    m_itemsAdded = false;
    m_anyModified = false;
    m_vhCalcPending = false;
    m_colWidths.push_back( wxPG_DEFAULT_SPLITTERX );
    m_colWidths.push_back( wxPG_DEFAULT_SPLITTERX );
    m_fSplitterX = wxPG_DEFAULT_SPLITTERX;

    m_columnProportions.push_back(1);
    m_columnProportions.push_back(1);

    m_isSplitterPreSet = false;
    m_dontCenterSplitter = false;

    // By default, we only have the 'value' column editable
    m_editableColumns.push_back(1);
}

// -----------------------------------------------------------------------

wxPropertyGridPageState::~wxPropertyGridPageState()
{
    delete m_abcArray;
}

// -----------------------------------------------------------------------

void wxPropertyGridPageState::InitNonCatMode()
{
    if ( !m_abcArray )
    {
        m_abcArray = new wxPGRootProperty(wxS("<Root_NonCat>"));
        m_abcArray->SetParentState(this);
        m_abcArray->SetFlag(wxPG_PROP_CHILDREN_ARE_COPIES);
    }

    // Must be called when state::m_properties still points to regularArray.
    wxPGProperty* oldProperties = m_properties;

    // Must use temp value in state::m_properties for item iteration loop
    // to run as expected.
    m_properties = &m_regularArray;

    if ( m_properties->GetChildCount() )
    {
        //
        // Prepare m_abcArray
        wxPropertyGridIterator it( this, wxPG_ITERATE_PROPERTIES );

        for ( ; !it.AtEnd(); it.Next() )
        {
            wxPGProperty* p = it.GetProperty();
            wxPGProperty* parent = p->GetParent();
            if ( parent->IsCategory() || parent->IsRoot() )
            {
                m_abcArray->DoAddChild(p);
                p->m_parent = &m_regularArray;
            }
        }
    }

    m_properties = oldProperties;
}

// -----------------------------------------------------------------------

void wxPropertyGridPageState::DoClear()
{
    if ( m_pPropGrid && m_pPropGrid->GetState() == this )
    {
        m_pPropGrid->ClearSelection(false);
    }
    else
    {
        m_selection.clear();
    }

    // If handling wxPG event then every property item must be
    // deleted individually (and with deferral).
    if ( m_pPropGrid && m_pPropGrid->m_processedEvent )
    {
        for (unsigned int i = 0; i < m_regularArray.GetChildCount(); i++)
        {
            wxPGProperty* p = m_regularArray.Item(i);
            DoDelete(p, true);
        }
    }
    else
    {
        // Properties which will be deleted immediately
        // should be removed from the lists of pending deletions.
        for (unsigned int i = 0; i < m_regularArray.GetChildCount(); i++)
        {
            wxPGProperty* p = m_regularArray.Item(i);
            int index = m_pPropGrid->m_deletedProperties.Index(p);
            if (index != wxNOT_FOUND)
            {
                m_pPropGrid->m_deletedProperties.RemoveAt(index);
            }
            index = m_pPropGrid->m_removedProperties.Index(p);
            if (index != wxNOT_FOUND)
            {
                m_pPropGrid->m_removedProperties.RemoveAt(index);
            }
        }

        m_regularArray.Empty();
        if ( m_abcArray )
            m_abcArray->Empty();

        m_dictName.clear();

        m_currentCategory = NULL;
        m_lastCaptionBottomnest = true;
        m_itemsAdded = false;

        m_virtualHeight = 0;
        m_vhCalcPending = false;
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridPageState::CalculateFontAndBitmapStuff( int WXUNUSED(vspacing) )
{
    wxPropertyGrid* propGrid = GetGrid();

    VirtualHeightChanged();

    // Recalculate caption text extents.
    for ( unsigned int i = 0; i < m_regularArray.GetChildCount();i++ )
    {
        wxPGProperty* p =m_regularArray.Item(i);

        if ( p->IsCategory() )
            ((wxPropertyCategory*)p)->CalculateTextExtent(propGrid, propGrid->GetCaptionFont());
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridPageState::SetVirtualWidth( int width )
{
    // Sometimes width less than 0 is offered. Let's make things easy for
    // everybody and deal with it here.
    if ( width < 0 )
        width = 0;

    wxPropertyGrid* pg = GetGrid();
    int gw = pg->GetClientSize().x;
    if ( width < gw )
        width = gw;

    m_width = width;
}

// -----------------------------------------------------------------------

void wxPropertyGridPageState::OnClientWidthChange( int newWidth, int widthChange, bool fromOnResize )
{
    wxPropertyGrid* pg = GetGrid();

    if ( pg->HasVirtualWidth() )
    {
        if ( m_width < newWidth )
            SetVirtualWidth( newWidth );

        CheckColumnWidths(widthChange);
    }
    else
    {
        SetVirtualWidth( newWidth );

        // This should be done before splitter auto centering
        // NOTE: Splitter auto-centering is done in this function.
        if ( !fromOnResize )
            widthChange = 0;
        CheckColumnWidths(widthChange);

        if ( !m_isSplitterPreSet && m_dontCenterSplitter )
        {
             wxMilliClock_t timeSinceCreation = ::wxGetLocalTimeMillis() - GetGrid()->m_timeCreated;

            // If too long, don't set splitter
            if ( timeSinceCreation < 250 )
            {
                if ( m_properties->GetChildCount() )
                {
                    SetSplitterLeft( false );
                }
                else
                {
                    DoSetSplitterPosition( newWidth / 2 );
                    m_isSplitterPreSet = false;
                }
            }
        }
    }
}

// -----------------------------------------------------------------------
// wxPropertyGridPageState item iteration methods
// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridPageState::GetLastItem( int flags )
{
    if ( !m_properties->GetChildCount() )
        return NULL;

    wxPG_ITERATOR_CREATE_MASKS(flags, wxPGProperty::FlagType itemExMask, wxPGProperty::FlagType parentExMask)

    // First, get last child of last parent
    wxPGProperty* pwc = (wxPGProperty*)m_properties->Last();
    while ( pwc->GetChildCount() &&
            wxPG_ITERATOR_PARENTEXMASK_TEST(pwc, parentExMask) )
        pwc = (wxPGProperty*) pwc->Last();

    // Then, if it doesn't fit our criteria, back up until we find something that does
    if ( pwc->HasFlag(itemExMask) )
    {
        wxPropertyGridIterator it( this, flags, pwc );
        for ( ; !it.AtEnd(); it.Prev() )
            ;
        pwc = (wxPGProperty*) it.GetProperty();
    }

    return pwc;
}

wxPropertyCategory* wxPropertyGridPageState::GetPropertyCategory( const wxPGProperty* p ) const
{
    const wxPGProperty* parent = (const wxPGProperty*)p;
    const wxPGProperty* grandparent = (const wxPGProperty*)parent->GetParent();
    do
    {
        parent = grandparent;
        grandparent = (wxPGProperty*)parent->GetParent();
        if ( parent->IsCategory() && grandparent )
            return (wxPropertyCategory*)parent;
    } while ( grandparent );

    return NULL;
}

// -----------------------------------------------------------------------
// wxPropertyGridPageState GetPropertyXXX methods
// -----------------------------------------------------------------------

#if WXWIN_COMPATIBILITY_3_0
wxPGProperty* wxPropertyGridPageState::GetPropertyByLabel
                        ( const wxString& label, wxPGProperty* parent ) const
{
    return BaseGetPropertyByLabel(label, parent);
}
#endif // WXWIN_COMPATIBILITY_3_0

wxPGProperty* wxPropertyGridPageState::BaseGetPropertyByLabel
                        ( const wxString& label, wxPGProperty* parent ) const
{
    if ( !parent )
    {
        parent = (wxPGProperty*) &m_regularArray;
    }

    for ( size_t i=0; i<parent->GetChildCount(); i++ )
    {
        wxPGProperty* p = parent->Item(i);
        if ( p->GetLabel() == label )
            return p;
        // Check children recursively.
        if ( p->GetChildCount() )
        {
            p = BaseGetPropertyByLabel(label, p);
            if ( p )
                return p;
        }
    }

    return NULL;
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridPageState::BaseGetPropertyByName( const wxString& name ) const
{
    wxPGHashMapS2P::const_iterator it;
    it = m_dictName.find(name);
    if ( it != m_dictName.end() )
        return (wxPGProperty*) it->second;
    return NULL;
}

// -----------------------------------------------------------------------

void wxPropertyGridPageState::DoSetPropertyName( wxPGProperty* p,
                                                 const wxString& newName )
{
    wxCHECK_RET( p, wxS("invalid property id") );

    wxPGProperty* parent = p->GetParent();

    if ( parent->IsCategory() || parent->IsRoot() )
    {
        if ( !p->GetBaseName().empty() )
            m_dictName.erase( p->GetBaseName() );
        if ( !newName.empty() )
            m_dictName[newName] = (void*) p;
    }

    p->DoSetName(newName);
}

// -----------------------------------------------------------------------
// wxPropertyGridPageState global operations
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// Item iteration macros
//   NB: Nowadays only needed for alphabetic/categoric mode switching.
// -----------------------------------------------------------------------

//#define II_INVALID_I    0x00FFFFFF

#define ITEM_ITERATION_VARIABLES \
    wxPGProperty* parent; \
    unsigned int i; \
    unsigned int iMax;

#define ITEM_ITERATION_INIT_FROM_THE_TOP \
    parent = m_properties; \
    i = 0;

#if 0
#define ITEM_ITERATION_INIT(startparent, startindex, state) \
    parent = startparent; \
    i = (unsigned int)startindex; \
    if ( parent == NULL ) \
    { \
        parent = state->m_properties; \
        i = 0; \
    }
#endif

#define ITEM_ITERATION_LOOP_BEGIN \
    do \
    { \
        iMax = parent->GetChildCount(); \
        while ( i < iMax ) \
        {  \
            wxPGProperty* p = parent->Item(i);

#define ITEM_ITERATION_LOOP_END \
            if ( p->GetChildCount() ) \
            { \
                i = 0; \
                parent = (wxPGProperty*)p; \
                iMax = parent->GetChildCount(); \
            } \
            else \
                i++; \
        } \
        i = parent->m_arrIndex + 1; \
        parent = parent->m_parent; \
    } \
    while ( parent != NULL );

bool wxPropertyGridPageState::EnableCategories( bool enable )
{
    //
    // NB: We can't use wxPropertyGridIterator in this
    //     function, since it depends on m_arrIndexes,
    //     which, among other things, is being fixed here.
    //
    ITEM_ITERATION_VARIABLES

    if ( enable )
    {
        //
        // Enable categories
        //

        if ( !IsInNonCatMode() )
            return false;

        m_properties = &m_regularArray;

        // fix parents, indexes, and depths
        ITEM_ITERATION_INIT_FROM_THE_TOP

        ITEM_ITERATION_LOOP_BEGIN

            p->m_arrIndex = i;

            p->m_parent = parent;

            // If parent was category, and this is not,
            // then the depth stays the same.
            if ( parent->IsCategory() &&
                 !p->IsCategory() )
                p->m_depth = parent->GetDepth();
            else
                p->m_depth = parent->GetDepth() + 1;

        ITEM_ITERATION_LOOP_END

    }
    else
    {
        //
        // Disable categories
        //

        if ( IsInNonCatMode() )
            return false;

        // Create array, if necessary.
        if ( !m_abcArray )
            InitNonCatMode();

        m_properties = m_abcArray;

        // fix parents, indexes, and depths
        ITEM_ITERATION_INIT_FROM_THE_TOP

        ITEM_ITERATION_LOOP_BEGIN

            p->m_arrIndex = i;

            p->m_parent = parent;

            p->m_depth = parent->GetDepth() + 1;

        ITEM_ITERATION_LOOP_END
    }

    VirtualHeightChanged();

    if ( m_pPropGrid->GetState() == this )
        m_pPropGrid->RecalculateVirtualSize();

    return true;
}

// -----------------------------------------------------------------------

static int wxPG_SortFunc_ByFunction(wxPGProperty **pp1, wxPGProperty **pp2)
{
    wxPGProperty *p1 = *pp1;
    wxPGProperty *p2 = *pp2;
    wxPropertyGrid* pg = p1->GetGrid();
    wxPGSortCallback sortFunction = pg->GetSortFunction();
    return sortFunction(pg, p1, p2);
}

static int wxPG_SortFunc_ByLabel(wxPGProperty **pp1, wxPGProperty **pp2)
{
    wxPGProperty *p1 = *pp1;
    wxPGProperty *p2 = *pp2;
    return p1->GetLabel().CmpNoCase( p2->GetLabel() );
}

#if 0
//
// For wxVector w/ wxUSE_STL=1, you would use code like this instead:
//

#include <algorithm>

static bool wxPG_SortFunc_ByFunction(wxPGProperty *p1, wxPGProperty *p2)
{
    wxPropertyGrid* pg = p1->GetGrid();
    wxPGSortCallback sortFunction = pg->GetSortFunction();
    return sortFunction(pg, p1, p2) < 0;
}

static bool wxPG_SortFunc_ByLabel(wxPGProperty *p1, wxPGProperty *p2)
{
    return p1->GetLabel().CmpNoCase( p2->GetLabel() ) < 0;
}
#endif

void wxPropertyGridPageState::DoSortChildren( wxPGProperty* p,
                                              int flags )
{
    if ( !p )
        p = m_properties;

    // Can only sort items with children
    if ( !p->GetChildCount() )
        return;

    // Never sort children of aggregate properties
    if ( p->HasFlag(wxPG_PROP_AGGREGATE) )
        return;

    if ( (flags & wxPG_SORT_TOP_LEVEL_ONLY)
         && !p->IsCategory() && !p->IsRoot() )
        return;

    if ( GetGrid()->GetSortFunction() )
        p->SortChildren(wxPG_SortFunc_ByFunction);
    else
        p->SortChildren(wxPG_SortFunc_ByLabel);

    // Fix indices
    p->FixIndicesOfChildren();

    if ( flags & wxPG_RECURSE )
    {
        // Apply sort recursively
        for ( unsigned int i=0; i<p->GetChildCount(); i++ )
            DoSortChildren(p->Item(i), flags);
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridPageState::DoSort( int flags )
{
    DoSortChildren( m_properties, flags | wxPG_RECURSE );

    // We used to sort categories as well here also if in non-categorized
    // mode, but doing would naturally cause child indices to become
    // corrupted.
}

// -----------------------------------------------------------------------

bool wxPropertyGridPageState::PrepareAfterItemsAdded()
{
    if ( !m_itemsAdded ) return false;

    wxPropertyGrid* pg = GetGrid();

    m_itemsAdded = false;

    if ( pg->HasFlag(wxPG_AUTO_SORT) )
        DoSort(wxPG_SORT_TOP_LEVEL_ONLY);

    return true;
}

// -----------------------------------------------------------------------
// wxPropertyGridPageState splitter, column and hittest functions
// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridPageState::DoGetItemAtY( int y ) const
{
    // Outside?
    if ( y < 0 )
        return NULL;

    unsigned int a = 0;
    return m_properties->GetItemAtY(y, GetGrid()->GetRowHeight(), &a);
}

// -----------------------------------------------------------------------

wxPropertyGridHitTestResult
wxPropertyGridPageState::HitTest( const wxPoint&pt ) const
{
    wxPropertyGridHitTestResult result;
    result.m_column = HitTestH( pt.x, &result.m_splitter,
                                &result.m_splitterHitOffset );
    result.m_property = DoGetItemAtY( pt.y );
    return result;
}

// -----------------------------------------------------------------------

// Used by SetSplitterLeft() and DotFitColumns()
int wxPropertyGridPageState::GetColumnFitWidth(wxClientDC& dc,
                                           wxPGProperty* pwc,
                                           unsigned int col,
                                           bool subProps) const
{
    wxPropertyGrid* pg = m_pPropGrid;
    int maxW = 0;
    int w, h;

    for ( unsigned int i = 0; i <pwc->GetChildCount(); i++ )
    {
        wxPGProperty* p = pwc->Item(i);
        if ( !p->IsCategory() )
        {
            wxString text;
            p->GetDisplayInfo(col, -1, 0, &text, (wxPGCell*)NULL);
            dc.GetTextExtent(text, &w, &h);
            if ( col == 0 )
                w += ( (p->GetDepth()-1) * pg->m_subgroup_extramargin );

            // account for the bitmap
            if ( col == 1 )
                w += p->GetImageOffset(pg->GetImageRect(p, -1).GetWidth());


            w += (wxPG_XBEFORETEXT*2);

            if ( w > maxW )
                maxW = w;
        }

        if ( p->GetChildCount() &&
             ( subProps || p->IsCategory() ) )
        {
            w = GetColumnFitWidth( dc, p, col, subProps );

            if ( w > maxW )
                maxW = w;
        }
    }

    return maxW;
}

int wxPropertyGridPageState::GetColumnFullWidth( wxClientDC &dc, wxPGProperty *p, unsigned int col )
{
    if ( p->IsCategory() )
        return 0;

    wxString text;
    p->GetDisplayInfo(col, -1, 0, &text, (wxPGCell*)NULL);
    int w = dc.GetTextExtent(text).x;

    if ( col == 0 )
        w += p->GetDepth() * m_pPropGrid->m_subgroup_extramargin;

    // account for the bitmap
    if ( col == 1 )
        w += p->GetImageOffset(m_pPropGrid->GetImageRect(p, -1).GetWidth());

    w += (wxPG_XBEFORETEXT*2);
    return w;
}

int wxPropertyGridPageState::DoGetSplitterPosition( int splitterColumn ) const
{
    int n = GetGrid()->GetMarginWidth();
    for ( int i = 0; i <= splitterColumn; i++ )
        n += m_colWidths[i];
    return n;
}

int wxPropertyGridPageState::GetColumnMinWidth( int WXUNUSED(column) ) const
{
    return wxPG_DRAG_MARGIN;
}

void wxPropertyGridPageState::PropagateColSizeDec( int column,
                                                   int decrease,
                                                   int dir )
{
    wxASSERT( decrease >= 0 );
    wxASSERT( dir == 1 || dir == -1 );

    int col = column;
    while(decrease > 0 && col >= 0 && col < (int)m_colWidths.size())
    {
        const int origWidth = m_colWidths[col];
        const int min = GetColumnMinWidth(col);
        m_colWidths[col] -= decrease;
        if ( m_colWidths[col] < min )
        {
            m_colWidths[col] = min;
        }
        decrease -= (origWidth - m_colWidths[col]);
        col += dir;
    }
    // As a last resort, if change of width was not fully absorbed
    // on the requested side we try to do this on the other side.
    col = column;
    dir *= -1;
    while(decrease > 0 && col >= 0 && col < (int)m_colWidths.size())
    {
        const int origWidth = m_colWidths[col];
        const int min = GetColumnMinWidth(col);
        m_colWidths[col] -= decrease;
        if ( m_colWidths[col] < min )
        {
            m_colWidths[col] = min;
        }
        decrease -= (origWidth - m_colWidths[col]);
        col += dir;
    }

    wxASSERT( decrease == 0 );
}

void wxPropertyGridPageState::DoSetSplitterPosition( int newXPos,
                                                     int splitterColumn,
                                                     int flags )
{
    wxPropertyGrid* pg = GetGrid();

    int adjust = newXPos - DoGetSplitterPosition(splitterColumn);

    if ( !pg->HasVirtualWidth() )
    {
        // No virtual width
        int otherColumn;
        if ( adjust > 0 )
        {
            otherColumn = splitterColumn + 1;
            if ( otherColumn == (int)m_colWidths.size() )
                otherColumn = 0;
            m_colWidths[splitterColumn] += adjust;
            PropagateColSizeDec( otherColumn, adjust, 1 );
        }
        else if ( adjust < 0 )
        {
            otherColumn = splitterColumn + 1;
            if ( otherColumn == (int)m_colWidths.size() )
                otherColumn = 0;
            m_colWidths[otherColumn] -= adjust;
            PropagateColSizeDec( splitterColumn, -adjust, -1 );
        }
    }
    else
    {
        m_colWidths[splitterColumn] += adjust;
    }
    // Actual adjustment can be different from demanded.
    newXPos = DoGetSplitterPosition(splitterColumn);

    if ( splitterColumn == 0 )
        m_fSplitterX = (double) newXPos;

    if ( !(flags & wxPG_SPLITTER_FROM_AUTO_CENTER) &&
         !(flags & wxPG_SPLITTER_FROM_EVENT) )
    {
        // Don't allow initial splitter auto-positioning after this.
        m_isSplitterPreSet = true;

        CheckColumnWidths();
    }
}

// Moves splitter so that all labels are visible, but just.
void wxPropertyGridPageState::SetSplitterLeft( bool subProps )
{
    wxPropertyGrid* pg = GetGrid();
    wxClientDC dc(pg);
    dc.SetFont(pg->GetFont());

    int maxW = GetColumnFitWidth(dc, m_properties, 0, subProps);

    if ( maxW > 0 )
    {
        maxW += pg->GetMarginWidth();
        DoSetSplitterPosition( maxW );
    }

    m_dontCenterSplitter = true;
}

wxSize wxPropertyGridPageState::DoFitColumns( bool WXUNUSED(allowGridResize) )
{
    wxPropertyGrid* pg = GetGrid();
    wxClientDC dc(pg);
    dc.SetFont(pg->GetFont());

    int marginWidth = pg->GetMarginWidth();
    int accWid = marginWidth;
    int maxColWidth = 500;

    for ( unsigned int col=0; col < GetColumnCount(); col++ )
    {
        int fitWid = GetColumnFitWidth(dc, m_properties, col, true);
        int colMinWidth = GetColumnMinWidth(col);
        if ( fitWid < colMinWidth )
            fitWid = colMinWidth;
        else if ( fitWid > maxColWidth )
            fitWid = maxColWidth;

        m_colWidths[col] = fitWid;

        accWid += fitWid;
    }

    // Expand last one to fill the width
    int remaining = m_width - accWid;
    m_colWidths[GetColumnCount()-1] += remaining;

    m_dontCenterSplitter = true;

    int firstSplitterX = marginWidth + m_colWidths[0];
    m_fSplitterX = (double) firstSplitterX;

    // Don't allow initial splitter auto-positioning after this.
    if ( pg->GetState() == this )
    {
        pg->SetSplitterPosition(firstSplitterX, false);
        pg->Refresh();
    }

    int x, y;
    pg->GetVirtualSize(&x, &y);

    return wxSize(accWid, y);
}

void wxPropertyGridPageState::CheckColumnWidths( int widthChange )
{
    if ( m_width == 0 )
        return;

    wxPropertyGrid* pg = GetGrid();

    unsigned int i;
    unsigned int lastColumn = m_colWidths.size() - 1;
    int width = m_width;
    int clientWidth = pg->GetClientSize().x;

    //
    // Column to reduce, if needed. Take last one that exceeds minimum width.
    int reduceCol = -1;

    wxLogTrace("propgrid",
               wxS("ColumnWidthCheck (virtualWidth: %i, clientWidth: %i)"),
               width, clientWidth);

    //
    // Check min sizes
    for ( i=0; i<m_colWidths.size(); i++ )
    {
        int min = GetColumnMinWidth(i);
        if ( m_colWidths[i] <= min )
        {
            m_colWidths[i] = min;
        }
        else
        {
            // Always reduce the last column that is larger than minimum size
            // (looks nicer, even with auto-centering enabled).
            reduceCol = i;
        }
    }

    int colsWidth = pg->GetMarginWidth();
    for ( i=0; i<m_colWidths.size(); i++ )
        colsWidth += m_colWidths[i];

    wxLogTrace("propgrid",
               wxS("  HasVirtualWidth: %i  colsWidth: %i"),
               (int)pg->HasVirtualWidth(), colsWidth);

    // Then mode-based requirement
    if ( !pg->HasVirtualWidth() )
    {
        int widthHigher = width - colsWidth;

        // Adapt colsWidth to width
        if ( colsWidth < width )
        {
            // Increase column
            wxLogTrace("propgrid",
                       wxS("  Adjust last column to %i"),
                       m_colWidths[lastColumn] + widthHigher);
            m_colWidths[lastColumn] = m_colWidths[lastColumn] + widthHigher;
        }
        else if ( colsWidth > width )
        {
            // Reduce column
            if ( reduceCol != -1 )
            {
                wxLogTrace("propgrid",
                           wxS("  Reduce column %i (by %i)"),
                           reduceCol, -widthHigher);

                // Reduce widest column, and recheck
                m_colWidths[reduceCol] = m_colWidths[reduceCol] + widthHigher;
                CheckColumnWidths();
            }
        }
    }
    else
    {
        // Only check colsWidth against clientWidth
        if ( colsWidth < clientWidth )
        {
            m_colWidths[lastColumn] = m_colWidths[lastColumn] + (clientWidth-colsWidth);
        }

        m_width = colsWidth;

        // If width changed, recalculate virtual size
        if ( pg->GetState() == this )
            pg->RecalculateVirtualSize();
    }

    for ( i=0; i<m_colWidths.size(); i++ )
    {
        wxLogTrace("propgrid", wxS("col%i: %i"), i, m_colWidths[i]);
    }

    // Auto center splitter
    if ( !m_dontCenterSplitter )
    {
        if ( m_colWidths.size() == 2 &&
             m_columnProportions[0] == m_columnProportions[1] )
        {
            //
            // When we have two columns of equal proportion, then use this
            // code. It will look nicer when the scrollbar visibility is
            // toggled on and off.
            //
            // TODO: Adapt this to generic recenter code.
            //
            double centerX = pg->m_width / 2.0;
            double splitterX;

            if ( m_fSplitterX < 0.0 )
            {
                splitterX = centerX;
            }
            else if ( widthChange )
            {
                //float centerX = float(pg->GetSize().x) * 0.5;

                // Recenter?
                splitterX = m_fSplitterX + (widthChange * 0.5);
                double deviation = fabs(centerX - splitterX);

                // If deviating from center, adjust towards it
                if ( deviation > 20.0 )
                {
                    if ( splitterX > centerX)
                        splitterX -= 2;
                    else
                        splitterX += 2;
                }
            }
            else
            {
                // No width change, just keep sure we keep splitter position intact
                splitterX = m_fSplitterX;
                double deviation = fabs(centerX - splitterX);
                if ( deviation > 50.0 )
                {
                    splitterX = centerX;
                }
            }

            DoSetSplitterPosition((int)splitterX, 0,
                                  wxPG_SPLITTER_FROM_AUTO_CENTER);

            m_fSplitterX = splitterX; // needed to retain accuracy
        }
        else
        {
            //
            // Generic re-center code
            //
            ResetColumnSizes(wxPG_SPLITTER_FROM_AUTO_CENTER);
        }
    }
}

void wxPropertyGridPageState::ResetColumnSizes( int setSplitterFlags )
{
    unsigned int i;
    // Calculate sum of proportions
    int psum = 0;
    for ( i=0; i<m_colWidths.size(); i++ )
        psum += m_columnProportions[i];
    int puwid = (m_pPropGrid->m_width*256) / psum;
    int cpos = 0;

    // Convert proportion to splitter positions
    for ( i=0; i<(m_colWidths.size() - 1); i++ )
    {
        int cwid = (puwid*m_columnProportions[i]) / 256;
        cpos += cwid;
        DoSetSplitterPosition(cpos, i,
                              setSplitterFlags);
    }
}

void wxPropertyGridPageState::SetColumnCount( int colCount )
{
    wxASSERT( colCount >= 2 );
    m_colWidths.SetCount( colCount, wxPG_DRAG_MARGIN );
    m_columnProportions.SetCount( colCount, 1 );
    if ( m_colWidths.size() > (unsigned int)colCount )
        m_colWidths.RemoveAt( m_colWidths.size()-1,
                              m_colWidths.size() - colCount );

    if ( m_pPropGrid->GetState() == this )
        m_pPropGrid->RecalculateVirtualSize();
    else
        CheckColumnWidths();
}

void wxPropertyGridPageState::DoSetColumnProportion( unsigned int column,
                                                 int proportion )
{
    wxASSERT_MSG( proportion >= 1,
                  wxS("Column proportion must 1 or higher") );

    if ( proportion < 1 )
        proportion = 1;

    while ( m_columnProportions.size() <= column )
        m_columnProportions.push_back(1);

    m_columnProportions[column] = proportion;
}

// Returns column index, -1 for margin
int wxPropertyGridPageState::HitTestH( int x, int* pSplitterHit, int* pSplitterHitOffset ) const
{
    int cx = GetGrid()->GetMarginWidth();
    int col = -1;
    int prevSplitter = -1;

    while ( x > cx )
    {
        col++;
        if ( col >= (int)m_colWidths.size() )
        {
            *pSplitterHit = -1;
            return col;
        }
        prevSplitter = cx;
        cx += m_colWidths[col];
    }

    // Near prev. splitter
    if ( col >= 1 )
    {
        int diff = x - prevSplitter;
        if ( abs(diff) < wxPG_SPLITTERX_DETECTMARGIN1 )
        {
            *pSplitterHit = col - 1;
            *pSplitterHitOffset = diff;
            return col;
        }
    }

    // Near next splitter
    int nextSplitter = cx;
    if ( col < (int)(m_colWidths.size()-1) )
    {
        int diff = x - nextSplitter;
        if ( abs(diff) < wxPG_SPLITTERX_DETECTMARGIN1 )
        {
            *pSplitterHit = col;
            *pSplitterHitOffset = diff;
            return col;
        }
    }

    *pSplitterHit = -1;
    return col;
}

bool wxPropertyGridPageState::ArePropertiesAdjacent( wxPGProperty* prop1,
                                                     wxPGProperty* prop2,
                                                     int iterFlags ) const
{
    const wxPGProperty* ap1 =
        wxPropertyGridConstIterator::OneStep(this,
                                             iterFlags,
                                             prop1,
                                             1);
    if ( ap1 && ap1 == prop2 )
        return true;

    const wxPGProperty* ap2 =
        wxPropertyGridConstIterator::OneStep(this,
                                             iterFlags,
                                             prop1,
                                             -1);
    if ( ap2 && ap2 == prop2 )
        return true;

    return false;
}

// -----------------------------------------------------------------------
// wxPropertyGridPageState property value setting and getting
// -----------------------------------------------------------------------

bool wxPropertyGridPageState::DoSetPropertyValueString( wxPGProperty* p, const wxString& value )
{
    if ( p )
    {
        int flags = wxPG_REPORT_ERROR|wxPG_FULL_VALUE|wxPG_PROGRAMMATIC_VALUE;

        wxVariant variant = p->GetValueRef();
        bool res;

        if ( p->GetMaxLength() <= 0 )
            res = p->StringToValue( variant, value, flags );
        else
            res = p->StringToValue( variant, value.Mid(0,p->GetMaxLength()), flags );

        if ( res )
        {
            p->SetValue(variant);
            if ( p == m_pPropGrid->GetSelection() &&
                 this == m_pPropGrid->GetState() )
                m_pPropGrid->RefreshEditor();
        }

        return true;
    }
    return false;
}

// -----------------------------------------------------------------------

bool wxPropertyGridPageState::DoSetPropertyValue( wxPGProperty* p, wxVariant& value )
{
    if ( p )
    {
        p->SetValue(value);
        if ( p == m_pPropGrid->GetSelection() &&
             this == m_pPropGrid->GetState() )
            m_pPropGrid->RefreshEditor();

        return true;
    }
    return false;
}

// -----------------------------------------------------------------------

bool wxPropertyGridPageState::DoSetPropertyValueWxObjectPtr( wxPGProperty* p, wxObject* value )
{
    if ( p )
    {
        // wnd_primary has to be given so the control can be updated as well.
        wxVariant v(value);
        DoSetPropertyValue(p, v);
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------
// wxPropertyGridPageState property operations
// -----------------------------------------------------------------------

bool wxPropertyGridPageState::DoIsPropertySelected( wxPGProperty* prop ) const
{
    return m_selection.Index(prop) != wxNOT_FOUND;
}

// -----------------------------------------------------------------------

void wxPropertyGridPageState::DoRemoveFromSelection( wxPGProperty* prop )
{
    for ( unsigned int i=0; i<m_selection.size(); i++ )
    {
        if ( m_selection[i] == prop )
        {
            wxPropertyGrid* pg = m_pPropGrid;
            if ( i == 0 && pg->GetState() == this )
            {
                // If first item (i.e. one with the active editor) was
                // deselected, then we need to take some extra measures.
                wxArrayPGProperty sel(m_selection.begin() + 1, m_selection.end());

                wxPGProperty* newFirst = sel.empty()? NULL: sel[0];

                pg->DoSelectProperty(newFirst,
                                     wxPG_SEL_DONT_SEND_EVENT);

                m_selection = sel;

                pg->Refresh();
            }
            else
            {
                m_selection.erase( m_selection.begin() + i );
            }
            return;
        }
    }
}

// -----------------------------------------------------------------------

bool wxPropertyGridPageState::DoCollapse( wxPGProperty* p )
{
    wxCHECK_MSG( p, false, wxS("invalid property id") );

    if ( !p->GetChildCount() ) return false;

    if ( !p->IsExpanded() ) return false;

    p->SetExpanded(false);

    VirtualHeightChanged();

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGridPageState::DoExpand( wxPGProperty* p )
{
    wxCHECK_MSG( p, false, wxS("invalid property id") );

    if ( !p->GetChildCount() ) return false;

    if ( p->IsExpanded() ) return false;

    p->SetExpanded(true);

    VirtualHeightChanged();

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGridPageState::DoSelectProperty( wxPGProperty* p, unsigned int flags )
{
    if ( this == m_pPropGrid->GetState() )
        return m_pPropGrid->DoSelectProperty( p, flags );

    DoSetSelection(p);
    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGridPageState::DoHideProperty( wxPGProperty* p, bool hide, int flags )
{
    p->DoHide(hide, flags);
    VirtualHeightChanged();

    return true;
}

// -----------------------------------------------------------------------
// wxPropertyGridPageState wxVariant related routines
// -----------------------------------------------------------------------

// Returns list of wxVariant objects (non-categories and non-sub-properties only).
// Never includes sub-properties (unless they are parented by wxParentProperty).
wxVariant wxPropertyGridPageState::DoGetPropertyValues( const wxString& listname,
                                                    wxPGProperty* baseparent,
                                                    long flags ) const
{
    wxPGProperty* pwc = (wxPGProperty*) baseparent;

    // Root is the default base-parent.
    if ( !pwc )
        pwc = m_properties;

    wxVariantList tempList;
    wxVariant v( tempList, listname );

    if ( pwc->GetChildCount() )
    {
        if ( flags & wxPG_KEEP_STRUCTURE )
        {
            wxASSERT( !pwc->HasFlag(wxPG_PROP_AGGREGATE) );

            for ( unsigned int i = 0; i < pwc->GetChildCount(); i++ )
            {
                wxPGProperty* p = pwc->Item(i);
                if ( !p->GetChildCount() || p->HasFlag(wxPG_PROP_AGGREGATE) )
                {
                    wxVariant variant = p->GetValue();
                    variant.SetName( p->GetBaseName() );
                    v.Append( variant );
                }
                else
                {
                    v.Append( DoGetPropertyValues(p->GetBaseName(),p,flags|wxPG_KEEP_STRUCTURE) );
                }
                if ( (flags & wxPG_INC_ATTRIBUTES) && p->GetAttributes().GetCount() )
                    v.Append( p->GetAttributesAsList() );
            }
        }
        else
        {
            wxPropertyGridConstIterator it( this, wxPG_ITERATE_DEFAULT, pwc->Item(0) );
            it.SetBaseParent( pwc );

            for ( ; !it.AtEnd(); it.Next() )
            {
                const wxPGProperty* p = it.GetProperty();

                // Use a trick to ignore wxParentProperty itself, but not its sub-properties.
                if ( !p->GetChildCount() || p->HasFlag(wxPG_PROP_AGGREGATE) )
                {
                    wxVariant variant = p->GetValue();
                    variant.SetName( p->GetName() );
                    v.Append( variant );
                    if ( (flags & wxPG_INC_ATTRIBUTES) && p->GetAttributes().GetCount() )
                        v.Append( p->GetAttributesAsList() );
                }
            }
        }
    }

    return v;
}

// -----------------------------------------------------------------------

void wxPropertyGridPageState::DoSetPropertyValues( const wxVariantList& list, wxPGProperty* defaultCategory )
{
    bool origFrozen = true;

    if ( m_pPropGrid->GetState() == this )
    {
        origFrozen = m_pPropGrid->IsFrozen();
        if ( !origFrozen ) m_pPropGrid->Freeze();
    }

    wxPropertyCategory* use_category = (wxPropertyCategory*)defaultCategory;

    if ( !use_category )
        use_category = (wxPropertyCategory*)m_properties;

    // Let's iterate over the list of variants.
    wxVariantList::const_iterator node;
    int numSpecialEntries = 0;

    //
    // Second pass for special entries
    for ( node = list.begin(); node != list.end(); ++node )
    {
        wxVariant *current = (wxVariant*)*node;

        // Make sure it is wxVariant.
        wxASSERT( current );
        wxASSERT( wxStrcmp(current->GetClassInfo()->GetClassName(),wxS("wxVariant")) == 0 );

        const wxString& name = current->GetName();
        if ( !name.empty() )
        {
            //
            // '@' signified a special entry
            if ( name[0] == wxS('@') )
            {
                numSpecialEntries++;
            }
            else
            {
                wxPGProperty* foundProp = BaseGetPropertyByName(name);
                if ( foundProp )
                {
                    wxPGProperty* p = foundProp;

                    // If it was a list, we still have to go through it.
                    if ( current->IsType(wxPG_VARIANT_TYPE_LIST) )
                    {
                        DoSetPropertyValues( current->GetList(),
                                p->IsCategory()?p:(NULL)
                            );
                    }
                    else
                    {
                        wxASSERT_LEVEL_2_MSG(
                            wxStrcmp(current->GetType(), p->GetValue().GetType()) == 0,
                            wxString::Format(
                                wxS("setting value of property \"%s\" from variant"),
                                p->GetName().c_str())
                        );

                        p->SetValue(*current);
                    }
                }
                else
                {
                    // Is it list?
                    if ( !current->IsType(wxPG_VARIANT_TYPE_LIST) )
                    {
                        // Not.
                    }
                    else
                    {
                        // Yes, it is; create a sub category and append contents there.
                        wxPGProperty* newCat = DoInsert(use_category,-1,new wxPropertyCategory(current->GetName(),wxPG_LABEL));
                        DoSetPropertyValues( current->GetList(), newCat );
                    }
                }
            }
        }
    }

    if ( numSpecialEntries )
    {
        for ( node = list.begin(); node != list.end(); ++node )
        {
            wxVariant *current = (wxVariant*)*node;

            const wxString& name = current->GetName();
            if ( !name.empty() )
            {
                //
                // '@' signified a special entry
                if ( name[0] == wxS('@') )
                {
                    numSpecialEntries--;

                    size_t pos2 = name.rfind(wxS('@'));
                    if ( pos2 > 0 && pos2 < (name.size()-1) )
                    {
                        wxString propName = name.substr(1, pos2-1);
                        wxString entryType = name.substr(pos2+1, wxString::npos);

                        if ( entryType == wxS("attr") )
                        {
                            //
                            // List of attributes
                            wxPGProperty* foundProp = BaseGetPropertyByName(propName);
                            if ( foundProp )
                            {
                                wxASSERT( current->IsType(wxPG_VARIANT_TYPE_LIST) );

                                wxVariantList& list2 = current->GetList();
                                wxVariantList::const_iterator node2;

                                for ( node2 = list2.begin(); node2 != list2.end(); ++node2 )
                                {
                                    wxVariant *attr = (wxVariant*)*node2;
                                    foundProp->SetAttribute( attr->GetName(), *attr );
                                }
                            }
                            else
                            {
                                // ERROR: No such property: 'propName'
                            }
                        }
                    }
                    else
                    {
                        // ERROR: Special entry requires name of format @<propname>@<entrytype>
                    }
                }
            }

            if ( !numSpecialEntries )
                break;
        }
    }

    if ( !origFrozen )
    {
        m_pPropGrid->Thaw();

        if ( this == m_pPropGrid->GetState() )
            m_pPropGrid->RefreshEditor();
    }

}

// -----------------------------------------------------------------------
// wxPropertyGridPageState property adding and removal
// -----------------------------------------------------------------------

bool wxPropertyGridPageState::PrepareToAddItem( wxPGProperty* property,
                                                wxPGProperty* scheduledParent )
{
    wxPropertyGrid* propGrid = m_pPropGrid;

    // This will allow better behaviour.
    if ( scheduledParent == m_properties )
        scheduledParent = NULL;

    if ( scheduledParent && !scheduledParent->IsCategory() )
    {
        wxASSERT_MSG( !property->GetBaseName().empty(),
                      wxS("Property's children must have unique, non-empty names within their scope") );
    }

    property->m_parentState = this;

    if ( property->IsCategory() )
    {

        // Parent of a category must be either root or another category
        // (otherwise Bad Things might happen).
        wxASSERT_MSG( scheduledParent == NULL ||
                      scheduledParent == m_properties ||
                      scheduledParent->IsCategory(),
                 wxS("Parent of a category must be either root or another category."));

        // If we already have category with same name, delete given property
        // and use it instead as most recent caption item.
        wxPGProperty* found_id = BaseGetPropertyByName( property->GetBaseName() );
        if ( found_id )
        {
            wxPropertyCategory* pwc = (wxPropertyCategory*) found_id;
            if ( pwc->IsCategory() ) // Must be a category.
            {
                delete property;
                m_currentCategory = pwc;
                return false;
            }
        }
    }

#if wxDEBUG_LEVEL
    // Warn for identical names in debug mode.
    if ( BaseGetPropertyByName(property->GetName()) &&
         (!scheduledParent || scheduledParent->IsCategory()) )
    {
        wxFAIL_MSG(wxString::Format(
            wxS("wxPropertyGrid item with name \"%s\" already exists"),
            property->GetName()));

        wxPGGlobalVars->m_warnings++;
    }
#endif // wxDEBUG_LEVEL

    // NULL parent == root parent
    if ( !scheduledParent )
        scheduledParent = DoGetRoot();

    property->m_parent = scheduledParent;

    property->InitAfterAdded(this, propGrid);

    if ( property->IsCategory() )
    {
        wxPropertyCategory* pc = wxStaticCast(property, wxPropertyCategory);

        m_currentCategory = pc;

        // Calculate text extent for category caption
        if ( propGrid )
            pc->CalculateTextExtent(propGrid, propGrid->GetCaptionFont());
    }

    return true;
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridPageState::DoAppend( wxPGProperty* property )
{
    wxPropertyCategory* cur_cat = m_currentCategory;
    if ( property->IsCategory() )
        cur_cat = NULL;

    return DoInsert( cur_cat, -1, property );
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridPageState::DoInsert( wxPGProperty* parent, int index, wxPGProperty* property )
{
    if ( !parent )
        parent = m_properties;

    wxCHECK_MSG( !parent->HasFlag(wxPG_PROP_AGGREGATE),
                 wxNullProperty,
                 wxS("when adding properties to fixed parents, use BeginAddChildren and EndAddChildren.") );

    bool res = PrepareToAddItem( property, (wxPropertyCategory*)parent );

    // PrepareToAddItem() may just decide to use current category
    // instead of adding new one.
    if ( !res )
        return m_currentCategory;

    bool parentIsRoot = parent->IsRoot();
    bool parentIsCategory = parent->IsCategory();

    // Note that item must be added into current mode later.

    // If parent is wxParentProperty, just stick it in...
    // If parent is root (m_properties), then...
    //   In categoric mode: Add as last item in m_abcArray (if not category).
    //                      Add to given index in m_regularArray.
    //   In non-cat mode:   Add as last item in m_regularArray.
    //                      Add to given index in m_abcArray.
    // If parent is category, then...
    //   1) Add to given category in given index.
    //   2) Add as last item in m_abcArray.

    if ( m_properties == &m_regularArray )
    {
        // We are currently in Categorized mode

        // Only add non-categories to m_abcArray.
        if ( m_abcArray && !property->IsCategory() &&
             (parentIsCategory || parentIsRoot) )
        {
            m_abcArray->DoAddChild( property, -1, false );
        }

        // Add to current mode.
        parent->DoAddChild( property, index, true );
    }
    else
    {
        // We are currently in Non-categorized/Alphabetic mode

        if ( parentIsCategory )
            // Parent is category.
            parent->DoAddChild( property, index, false );
        else if ( parentIsRoot )
            // Parent is root.
            m_regularArray.DoAddChild( property, -1, false );

        // Add to current mode
        if ( !property->IsCategory() )
            m_abcArray->DoAddChild( property, index, true );
    }

    // category stuff
    if ( property->IsCategory() )
    {
        // This is a category caption item.

        // Last caption is not the bottom one (this info required by append)
        m_lastCaptionBottomnest = false;
    }

    // Only add name to hashmap if parent is root or category
    if ( !property->GetBaseName().empty() &&
        (parentIsCategory || parentIsRoot) )
        m_dictName[property->GetBaseName()] = (void*) property;

    VirtualHeightChanged();

    // Update values of all parents if they are containers of composed values.
    property->UpdateParentValues();

    // Update editor controls of all parents if they are containers of composed values.
    for( wxPGProperty *p = property->GetParent();
         p && !p->IsRoot() && !p->IsCategory() && p->HasFlag(wxPG_PROP_COMPOSED_VALUE);
         p = p->GetParent() )
    {
        p->RefreshEditor();
    }

    m_itemsAdded = true;

    return property;
}

// -----------------------------------------------------------------------

void wxPropertyGridPageState::DoRemoveChildrenFromSelection(wxPGProperty* p,
                                                            bool recursive,
                                                            int selFlags)
{
    wxPropertyGrid* pg = GetGrid();

    for( unsigned int i = 0; i < p->GetChildCount(); i++ )
    {
        wxPGProperty* child = p->Item(i);
        if ( DoIsPropertySelected(child) )
        {
            if ( pg && pg->GetState() == this )
            {
                pg->DoRemoveFromSelection(child, selFlags);
            }
            else
            {
                DoRemoveFromSelection(child);
            }
        }

        if ( recursive )
        {
            DoRemoveChildrenFromSelection(child, recursive, selFlags);
        }
    }
}

void wxPropertyGridPageState::DoMarkChildrenAsDeleted(wxPGProperty* p,
                                                      bool recursive)
{
    for( unsigned int i = 0; i < p->GetChildCount(); i++ )
    {
        wxPGProperty* child = p->Item(i);

        child->SetFlag(wxPG_PROP_BEING_DELETED);

        if ( recursive )
        {
            DoMarkChildrenAsDeleted(child, recursive);
        }
    }
}

void wxPropertyGridPageState::DoInvalidatePropertyName(wxPGProperty* p)
{
    // Let's trust that no sane property uses prefix like
    // this. It would be anyway fairly inconvenient (in
    // current code) to check whether a new name is used
    // by another property with parent (due to the child
    // name notation).
    wxString newName = wxS("_&/_%$") + p->GetBaseName();
    DoSetPropertyName(p, newName);
}

void wxPropertyGridPageState::DoInvalidateChildrenNames(wxPGProperty* p,
                                                        bool recursive)
{
    if (p->IsCategory())
    {
        for( unsigned int i = 0; i < p->GetChildCount(); i++ )
        {
            wxPGProperty* child = p->Item(i);
            DoInvalidatePropertyName(child);

            if ( recursive )
            {
                DoInvalidateChildrenNames(child, recursive);
            }
        }
    }
}

bool wxPropertyGridPageState::IsChildCategory(wxPGProperty* p,
                                              wxPropertyCategory* cat,
                                              bool recursive)
{
    if (p->IsCategory())
    {
        for( unsigned int i = 0; i < p->GetChildCount(); i++ )
        {
            wxPGProperty* child = p->Item(i);

            if (child->IsCategory() && child == cat)
            {
                return true;
            }

            if ( recursive && IsChildCategory(child, cat, recursive) )
            {
                return true;
            }
        }
    }

    return false;
}

void wxPropertyGridPageState::DoDelete( wxPGProperty* item, bool doDelete )
{
    wxCHECK_RET( item->GetParent(),
        wxS("wxPropertyGrid: This property was already deleted.") );

    wxCHECK_RET( item != &m_regularArray && item != m_abcArray,
        wxS("wxPropertyGrid: Do not attempt to remove the root item.") );

    wxPGProperty* parent = item->GetParent();

    wxCHECK_RET( !parent->HasFlag(wxPG_PROP_AGGREGATE),
        wxS("wxPropertyGrid: Do not attempt to remove sub-properties.") );

    wxASSERT( item->GetParentState() == this );

    wxPropertyGrid* pg = GetGrid();

    // Try to unselect property and its sub-properties.
    if ( DoIsPropertySelected(item) )
    {
        if ( pg && pg->GetState() == this )
        {
            pg->DoRemoveFromSelection(item,
                wxPG_SEL_DELETING|wxPG_SEL_NOVALIDATE);
        }
        else
        {
            DoRemoveFromSelection(item);
        }
    }

    if ( item->IsChildSelected(true) )
    {
        DoRemoveChildrenFromSelection(item, true,
                wxPG_SEL_DELETING|wxPG_SEL_NOVALIDATE);
    }

    // If deleted category or its sub-category is
    // a current category then reset current category marker.
    if ( item->IsCategory() )
    {
        if (item == m_currentCategory || IsChildCategory(item, m_currentCategory, true))
        {
            m_currentCategory = NULL;
        }
    }

    // Must defer deletion? Yes, if handling a wxPG event.
    if ( pg && pg->m_processedEvent )
    {
        // Prevent adding duplicates to the lists.
        if ( doDelete )
        {
            if ( pg->m_deletedProperties.Index(item) != wxNOT_FOUND )
                return;

            pg->m_deletedProperties.push_back(item);
        }
        else
        {
            if ( pg->m_removedProperties.Index(item) != wxNOT_FOUND )
                return;

            pg->m_removedProperties.push_back(item);
        }

        // Rename the property and its children so it won't remain in the way
        // of the user code.
        DoInvalidatePropertyName(item);
        DoInvalidateChildrenNames(item, true);

        return;
    }

    // Property has to be unselected prior deleting.
    // Otherwise crash can happen.
    wxASSERT_MSG( !DoIsPropertySelected(item) && !item->IsChildSelected(true),
                  wxS("Failed to unselect deleted property") );
    // Don't attempt to delete current category.
    wxASSERT_MSG( !item->IsCategory() || item != m_currentCategory,
                  wxS("Current category cannot be deleted") );

    // Prevent property and its children from being re-selected
    item->SetFlag(wxPG_PROP_BEING_DELETED);
    DoMarkChildrenAsDeleted(item, true);

    unsigned int indinparent = item->GetIndexInParent();

    // Delete children
    if ( item->GetChildCount() && !item->HasFlag(wxPG_PROP_AGGREGATE) )
    {
        // deleting a category
        item->DeleteChildren();
    }

    if ( !IsInNonCatMode() )
    {
        // categorized mode - non-categorized array

        // Remove from non-cat array
        if ( !item->IsCategory() &&
             (parent->IsCategory() || parent->IsRoot()) )
        {
            if ( m_abcArray )
                m_abcArray->RemoveChild(item);
        }

        // categorized mode - categorized array
        parent->RemoveChild(indinparent);
        item->m_parent->FixIndicesOfChildren();
    }
    else
    {
        // non-categorized mode - categorized array

        // We need to find location of item.
        wxPGProperty* cat_parent = &m_regularArray;
        int cat_index = m_regularArray.GetChildCount();
        for ( unsigned int i = 0; i < m_regularArray.GetChildCount(); i++ )
        {
            wxPGProperty* p = m_regularArray.Item(i);
            if ( p == item ) { cat_index = i; break; }
            if ( p->IsCategory() )
            {
                int subind = ((wxPGProperty*)p)->Index(item);
                if ( subind != wxNOT_FOUND )
                {
                    cat_parent = ((wxPGProperty*)p);
                    cat_index = subind;
                    break;
                }
            }
        }
        cat_parent->RemoveChild(cat_index);

        // non-categorized mode - non-categorized array
        if ( !item->IsCategory() )
        {
            wxASSERT( item->m_parent == m_abcArray );
            item->m_parent->RemoveChild(indinparent);
            item->m_parent->FixIndicesOfChildren(indinparent);
        }
    }

    if ( !item->GetBaseName().empty() &&
         (parent->IsCategory() || parent->IsRoot()) )
        m_dictName.erase(item->GetBaseName());

    // We need to clear parent grid's m_propHover, if it matches item
    if ( pg && pg->m_propHover == item )
        pg->m_propHover = NULL;

    // Mark the property as 'unattached'
    item->m_parentState = NULL;
    item->m_parent = NULL;

    // We can actually delete it now
    if ( doDelete )
    {
        // Remove the item from both lists of pending operations.
        // (Deleted item cannot be also the subject of further removal.)
        int index = pg->m_deletedProperties.Index(item);
        if ( index != wxNOT_FOUND )
        {
            pg->m_deletedProperties.RemoveAt(index);
        }
        wxASSERT_MSG( pg->m_deletedProperties.Index(item) == wxNOT_FOUND,
                    wxS("Too many occurrences of the item"));

        index = pg->m_removedProperties.Index(item);
        if ( index != wxNOT_FOUND )
        {
            pg->m_removedProperties.RemoveAt(index);
        }
        wxASSERT_MSG( pg->m_removedProperties.Index(item) == wxNOT_FOUND,
                    wxS("Too many occurrences of the item"));

        delete item;
    }
    else
    {
        // Remove the item from the list of pending removals.
        int index = pg->m_removedProperties.Index(item);
        if ( index != wxNOT_FOUND )
        {
            pg->m_removedProperties.RemoveAt(index);
        }
        wxASSERT_MSG( pg->m_removedProperties.Index(item) == wxNOT_FOUND,
                    wxS("Too many occurrences of the item"));

        item->OnDetached(this, pg);
    }

    m_itemsAdded = true; // Not a logical assignment (but required nonetheless).

    VirtualHeightChanged();
}

// -----------------------------------------------------------------------

#endif  // wxUSE_PROPGRID

/////////////////////////////////////////////////////////////////////////////
// Name:        src/propgrid/propgridpagestate.cpp
// Purpose:     wxPropertyGridPageState class
// Author:      Jaakko Salli
// Modified by:
// Created:     2008-08-24
// RCS-ID:      $Id$
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
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
    if ( property && (property->GetFlags() & m_itemExMask) )
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
    wxASSERT( property );

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
    if ( property->GetFlags() & m_itemExMask )
        Prev();
}

void wxPropertyGridIteratorBase::Next( bool iterateChildren )
{
    wxPGProperty* property = m_property;
    wxASSERT( property );

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
    if ( property->GetFlags() & m_itemExMask )
        Next();
}

// -----------------------------------------------------------------------
// wxPropertyGridPageState
// -----------------------------------------------------------------------

wxPropertyGridPageState::wxPropertyGridPageState()
{
    m_pPropGrid = (wxPropertyGrid*) NULL;
    m_regularArray.SetParentState(this);
    m_properties = &m_regularArray;
    m_abcArray = (wxPGRootProperty*) NULL;
    m_currentCategory = (wxPropertyCategory*) NULL;
    m_selected = (wxPGProperty*) NULL;
    m_width = 0;
    m_virtualHeight = 0;
    m_lastCaptionBottomnest = 1;
    m_itemsAdded = 0;
    m_anyModified = 0;
    m_vhCalcPending = 0;
    m_colWidths.push_back( wxPG_DEFAULT_SPLITTERX );
    m_colWidths.push_back( wxPG_DEFAULT_SPLITTERX );
    m_fSplitterX = wxPG_DEFAULT_SPLITTERX;
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
        m_abcArray = new wxPGRootProperty();
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
                m_abcArray->AddChild2(p);
                p->m_parent = &m_regularArray;
            }
        }
    }

    m_properties = oldProperties;
}

// -----------------------------------------------------------------------

void wxPropertyGridPageState::DoClear()
{
    m_regularArray.Empty();
    if ( m_abcArray )
        m_abcArray->Empty();

    m_dictName.clear();

    m_currentCategory = (wxPropertyCategory*) NULL;
    m_lastCaptionBottomnest = 1;
    m_itemsAdded = 0;

    m_virtualHeight = 0;
    m_vhCalcPending = 0;

    m_selected = (wxPGProperty*) NULL;
}

// -----------------------------------------------------------------------

void wxPropertyGridPageState::CalculateFontAndBitmapStuff( int WXUNUSED(vspacing) )
{
    wxPropertyGrid* propGrid = GetGrid();

    VirtualHeightChanged();

    // Recalculate caption text extents.
    unsigned int i;

    for ( i=0;i<m_regularArray.GetChildCount();i++ )
    {
        wxPGProperty* p =m_regularArray.Item(i);

        if ( p->IsCategory() )
            ((wxPropertyCategory*)p)->CalculateTextExtent(propGrid, propGrid->GetCaptionFont());
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridPageState::SetVirtualWidth( int width )
{
    wxASSERT( width >= 0 );
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

        if ( !(GetGrid()->GetInternalFlags() & wxPG_FL_SPLITTER_PRE_SET) &&
             (GetGrid()->GetInternalFlags() & wxPG_FL_DONT_CENTER_SPLITTER) )
        {
            long timeSinceCreation = (::wxGetLocalTimeMillis() - GetGrid()->m_timeCreated).ToLong();

            // If too long, don't set splitter
            if ( timeSinceCreation < 3000 )
            {
                if ( m_properties->GetChildCount() || timeSinceCreation > 750 )
                {
                    SetSplitterLeft( false );
                }
                else
                {
                    DoSetSplitterPosition( newWidth / 2 );
                    GetGrid()->ClearInternalFlag(wxPG_FL_SPLITTER_PRE_SET);
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
        return (wxPGProperty*) NULL;

    wxPG_ITERATOR_CREATE_MASKS(flags, int itemExMask, int parentExMask)

    // First, get last child of last parent
    wxPGProperty* pwc = (wxPGProperty*)m_properties->Last();
    while ( pwc->GetChildCount() &&
            wxPG_ITERATOR_PARENTEXMASK_TEST(pwc, parentExMask) )
        pwc = (wxPGProperty*) pwc->Last();

    // Then, if it doesn't fit our criteria, back up until we find something that does
    if ( pwc->GetFlags() & itemExMask )
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

    return (wxPropertyCategory*) NULL;
}

// -----------------------------------------------------------------------
// wxPropertyGridPageState GetPropertyXXX methods
// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridPageState::GetPropertyByLabel( const wxString& label,
                                                           wxPGProperty* parent ) const
{

    size_t i;

    if ( !parent ) parent = (wxPGProperty*) &m_regularArray;

    for ( i=0; i<parent->GetChildCount(); i++ )
    {
        wxPGProperty* p = parent->Item(i);
        if ( p->m_label == label )
            return p;
        // Check children recursively.
        if ( p->GetChildCount() )
        {
            p = GetPropertyByLabel(label,(wxPGProperty*)p);
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
    return (wxPGProperty*) NULL;
}

// -----------------------------------------------------------------------

void wxPropertyGridPageState::DoSetPropertyName( wxPGProperty* p,
                                                 const wxString& newName )
{
    wxCHECK_RET( p, wxT("invalid property id") );

    wxPGProperty* parent = p->GetParent();

    if ( parent->IsCategory() || parent->IsRoot() )
    {
        if ( p->GetBaseName().length() )
            m_dictName.erase( p->GetBaseName() );
        if ( newName.length() )
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
    if ( parent == (wxPGProperty*) NULL ) \
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
                p->m_depth = parent->m_depth;
            else
                p->m_depth = parent->m_depth + 1;

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

            p->m_depth = parent->m_depth + 1;

        ITEM_ITERATION_LOOP_END
    }

    VirtualHeightChanged();

    if ( m_pPropGrid->GetState() == this )
        m_pPropGrid->RecalculateVirtualSize();

    return true;
}

// -----------------------------------------------------------------------

#if wxUSE_STL
#include <algorithm>

static bool wxPG_SortFunc(wxPGProperty *p1, wxPGProperty *p2)
{
    return p1->GetLabel() < p2->GetLabel();
}

#else

static int wxPG_SortFunc(wxPGProperty **p1, wxPGProperty **p2)
{
    wxPGProperty *pp1 = *p1;
    wxPGProperty *pp2 = *p2;
    return pp1->GetLabel().compare( pp2->GetLabel() );
}

#endif

void wxPropertyGridPageState::SortChildren( wxPGProperty* p )
{
    if ( !p )
        p = (wxPGProperty*)m_properties;

    if ( !p->GetChildCount() )
        return;

    wxPGProperty* pwc = (wxPGProperty*)p;

    // Can only sort items with children
    if ( pwc->GetChildCount() < 1 )
        return;

#if wxUSE_STL
    std::sort(pwc->m_children.begin(), pwc->m_children.end(), wxPG_SortFunc);
#else
    pwc->m_children.Sort( wxPG_SortFunc );
#endif

    // Fix indexes
    pwc->FixIndicesOfChildren();

}

// -----------------------------------------------------------------------

void wxPropertyGridPageState::Sort()
{
    SortChildren( m_properties );

    // Sort categories as well
    if ( !IsInNonCatMode() )
    {
        size_t i;
        for ( i=0;i<m_properties->GetChildCount();i++)
        {
            wxPGProperty* p = m_properties->Item(i);
            if ( p->IsCategory() )
                SortChildren( p );
        }
    }
}

// -----------------------------------------------------------------------
// wxPropertyGridPageState splitter, column and hittest functions
// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridPageState::DoGetItemAtY( int y ) const
{
    // Outside?
    if ( y < 0 )
        return (wxPGProperty*) NULL;

    unsigned int a = 0;
    return m_properties->GetItemAtY(y, GetGrid()->m_lineHeight, &a);
}

// -----------------------------------------------------------------------

wxPropertyGridHitTestResult wxPropertyGridPageState::HitTest( const wxPoint&pt ) const
{
    wxPropertyGridHitTestResult result;
    result.column = HitTestH( pt.x, &result.splitter, &result.splitterHitOffset );
    result.property = DoGetItemAtY( pt.y );
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
    size_t i;
    int maxW = 0;
    int w, h;

    for ( i=0; i<pwc->GetChildCount(); i++ )
    {
        wxPGProperty* p = pwc->Item(i);
        if ( !p->IsCategory() )
        {
            const wxPGCell* cell = NULL;
            wxString text;
            p->GetDisplayInfo(col, -1, 0, &text, &cell);
            dc.GetTextExtent(text, &w, &h);
            if ( col == 0 )
                w += ( ((int)p->m_depth-1) * pg->m_subgroup_extramargin );

            //
            // TODO: Add bitmap support.

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

int wxPropertyGridPageState::DoGetSplitterPosition( int splitterColumn ) const
{
    int n = GetGrid()->m_marginWidth;
    int i;
    for ( i=0; i<=splitterColumn; i++ )
        n += m_colWidths[i];
    return n;
}

int wxPropertyGridPageState::GetColumnMinWidth( int WXUNUSED(column) ) const
{
    return wxPG_DRAG_MARGIN;
}

void wxPropertyGridPageState::PropagateColSizeDec( int column, int decrease, int dir )
{
    int origWidth = m_colWidths[column];
    m_colWidths[column] -= decrease;
    int min = GetColumnMinWidth(column);
    int more = 0;
    if ( m_colWidths[column] < min )
    {
        more = decrease - (origWidth - min);
        m_colWidths[column] = min;
    }

    //
    // FIXME: Causes erratic splitter changing, so as a workaround
    //        disabled if two or less columns.

    if ( m_colWidths.size() <= 2 )
        return;

    column += dir;
    if ( more && column < (int)m_colWidths.size() && column >= 0 )
        PropagateColSizeDec( column, more, dir );
}

void wxPropertyGridPageState::DoSetSplitterPosition( int newXPos, int splitterColumn, bool WXUNUSED(allPages), bool fromAutoCenter )
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
        else
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

    if ( splitterColumn == 0 )
        m_fSplitterX = (double) newXPos;

    if ( !fromAutoCenter )
    {
        // Don't allow initial splitter auto-positioning after this.
        if ( pg->GetState() == this )
            pg->SetInternalFlag(wxPG_FL_SPLITTER_PRE_SET);

        CheckColumnWidths();
    }
}

// Moves splitter so that all labels are visible, but just.
void wxPropertyGridPageState::SetSplitterLeft( bool subProps )
{
    wxPropertyGrid* pg = GetGrid();
    wxClientDC dc(pg);
    dc.SetFont(pg->m_font);

    int maxW = GetColumnFitWidth(dc, m_properties, 0, subProps);

    if ( maxW > 0 )
    {
        maxW += pg->m_marginWidth;
        DoSetSplitterPosition( maxW );
    }

    pg->SetInternalFlag(wxPG_FL_DONT_CENTER_SPLITTER);
}

wxSize wxPropertyGridPageState::DoFitColumns( bool WXUNUSED(allowGridResize) )
{
    wxPropertyGrid* pg = GetGrid();
    wxClientDC dc(pg);
    dc.SetFont(pg->m_font);

    int marginWidth = pg->m_marginWidth;
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

    pg->SetInternalFlag(wxPG_FL_DONT_CENTER_SPLITTER);

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

#ifdef __WXDEBUG__
    const bool debug = false;
#endif

    unsigned int i;
    unsigned int lastColumn = m_colWidths.size() - 1;
    int width = m_width;
    int clientWidth = pg->GetClientSize().x;

    //
    // Column to reduce, if needed. Take last one that exceeds minimum width.
    // Except if auto splitter centering is used, in which case use widest.
    int reduceCol = -1;
    int highestColWidth = 0;

#ifdef __WXDEBUG__
    if ( debug )
        wxLogDebug(wxT("ColumnWidthCheck (virtualWidth: %i, clientWidth: %i)"), width, clientWidth);
#endif

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
            if ( pg->HasFlag(wxPG_SPLITTER_AUTO_CENTER) )
            {
                if ( m_colWidths[i] >= highestColWidth )
                {
                    highestColWidth = m_colWidths[i];
                    reduceCol = i;
                }
            }
            else
            {
                reduceCol = i;
            }
        }
    }

    int colsWidth = pg->m_marginWidth;
    for ( i=0; i<m_colWidths.size(); i++ )
        colsWidth += m_colWidths[i];

#ifdef __WXDEBUG__
    if ( debug )
        wxLogDebug(wxT("  HasVirtualWidth: %i  colsWidth: %i"),(int)pg->HasVirtualWidth(),colsWidth);
#endif

    // Then mode-based requirement
    if ( !pg->HasVirtualWidth() )
    {
        int widthHigher = width - colsWidth;

        // Adapt colsWidth to width
        if ( colsWidth < width )
        {
            // Increase column
#ifdef __WXDEBUG__
            if ( debug )
                wxLogDebug(wxT("  Adjust last column to %i"), m_colWidths[lastColumn] + widthHigher);
#endif
            m_colWidths[lastColumn] = m_colWidths[lastColumn] + widthHigher;
        }
        else if ( colsWidth > width )
        {
            // Reduce column
            if ( reduceCol != -1 )
            {
            #ifdef __WXDEBUG__
                if ( debug )
                    wxLogDebug(wxT("  Reduce column %i (by %i)"), reduceCol, -widthHigher);
            #endif
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

#ifdef __WXDEBUG__
    if ( debug )
        for ( i=0; i<m_colWidths.size(); i++ )
            wxLogDebug(wxT("col%i: %i"),i,m_colWidths[i]);
#endif

    // Auto center splitter
    if ( !(pg->GetInternalFlags() & wxPG_FL_DONT_CENTER_SPLITTER) &&
         m_colWidths.size() == 2 )
    {
        float centerX = (float)(pg->m_width/2);
        float splitterX;

        if ( m_fSplitterX < 0.0 )
        {
            splitterX = centerX;
        }
        else if ( widthChange )
        {
            //float centerX = float(pg->GetSize().x) * 0.5;

            // Recenter?
            splitterX = m_fSplitterX + (float(widthChange) * 0.5);
            float deviation = fabs(centerX - splitterX);

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
            float deviation = fabs(centerX - splitterX);
            if ( deviation > 50.0 )
            {
                splitterX = centerX;
            }
        }

        DoSetSplitterPosition((int)splitterX, 0, false, true);

        m_fSplitterX = splitterX; // needed to retain accuracy
    }
}

void wxPropertyGridPageState::SetColumnCount( int colCount )
{
    wxASSERT( colCount >= 2 );
    m_colWidths.SetCount( colCount, wxPG_DRAG_MARGIN );
    if ( m_colWidths.size() > (unsigned int)colCount )
        m_colWidths.RemoveAt( m_colWidths.size(), m_colWidths.size() - colCount );

    if ( m_pPropGrid->GetState() == this )
        m_pPropGrid->RecalculateVirtualSize();
    else
        CheckColumnWidths();
}

// Returns column index, -1 for margin
int wxPropertyGridPageState::HitTestH( int x, int* pSplitterHit, int* pSplitterHitOffset ) const
{
    int cx = GetGrid()->m_marginWidth;
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
            if ( m_selected==p && this==m_pPropGrid->GetState() )
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
        if ( m_selected==p && this==m_pPropGrid->GetState() )
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

void wxPropertyGridPageState::DoSetPropertyValueUnspecified( wxPGProperty* p )
{
    wxCHECK_RET( p, wxT("invalid property id") );

    if ( !p->IsValueUnspecified() )
    {
        // Value should be set first - editor class methods may need it
        p->m_value.MakeNull();

        wxASSERT( m_pPropGrid );

        if ( m_pPropGrid->GetState() == this )
        {
            if ( m_pPropGrid->m_selected == p && m_pPropGrid->m_wndEditor )
            {
                p->GetEditorClass()->SetValueToUnspecified(p, m_pPropGrid->GetEditorControl());
            }
        }

        unsigned int i;
        for ( i = 0; i < p->GetChildCount(); i++ )
            DoSetPropertyValueUnspecified( p->Item(i) );
    }
}

// -----------------------------------------------------------------------
// wxPropertyGridPageState property operations
// -----------------------------------------------------------------------

bool wxPropertyGridPageState::DoCollapse( wxPGProperty* p )
{
    wxCHECK_MSG( p, false, wxT("invalid property id") );

    if ( !p->GetChildCount() ) return false;

    if ( !p->IsExpanded() ) return false;

    p->SetExpanded(false);

    VirtualHeightChanged();

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGridPageState::DoExpand( wxPGProperty* p )
{
    wxCHECK_MSG( p, false, wxT("invalid property id") );

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

    m_selected = p;
    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGridPageState::DoHideProperty( wxPGProperty* p, bool hide, int flags )
{
    if ( !hide )
        p->ClearFlag( wxPG_PROP_HIDDEN );
    else
        p->SetFlag( wxPG_PROP_HIDDEN );

    if ( flags & wxPG_RECURSE )
    {
        unsigned int i;
        for ( i = 0; i < p->GetChildCount(); i++ )
            DoHideProperty(p->Item(i), hide, flags | wxPG_RECURSE_STARTS);
    }

    VirtualHeightChanged();

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGridPageState::DoEnableProperty( wxPGProperty* p, bool enable )
{
    if ( p )
    {
        if ( enable )
        {
            if ( !(p->m_flags & wxPG_PROP_DISABLED) )
                return false;

            // Enabling

            p->m_flags &= ~(wxPG_PROP_DISABLED);
        }
        else
        {
            if ( p->m_flags & wxPG_PROP_DISABLED )
                return false;

            // Disabling

            p->m_flags |= wxPG_PROP_DISABLED;

        }

        // Apply same to sub-properties as well
        unsigned int i;
        for ( i = 0; i < p->GetChildCount(); i++ )
            DoEnableProperty( p->Item(i), enable );

        return true;
    }
    return false;
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

            size_t i;
            for ( i=0; i<pwc->GetChildCount(); i++ )
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
                    v.Append( DoGetPropertyValues(p->m_name,p,flags|wxPG_KEEP_STRUCTURE) );
                }
                if ( (flags & wxPG_INC_ATTRIBUTES) && p->m_attributes.GetCount() )
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
                    if ( (flags & wxPG_INC_ATTRIBUTES) && p->m_attributes.GetCount() )
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
    unsigned char origFrozen = 1;

    if ( m_pPropGrid->GetState() == this )
    {
        origFrozen = m_pPropGrid->m_frozen;
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
        wxASSERT( wxStrcmp(current->GetClassInfo()->GetClassName(),wxT("wxVariant")) == 0 );

        const wxString& name = current->GetName();
        if ( name.length() > 0 )
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
                    if ( wxStrcmp(current->GetType(), wxS("list")) == 0 )
                    {
                        DoSetPropertyValues( current->GetList(),
                                p->IsCategory()?p:((wxPGProperty*)NULL)
                            );
                    }
                    else
                    {
                #ifdef __WXDEBUG__
                        if ( wxStrcmp(current->GetType(), p->GetValue().GetType()) != 0)
                        {
                            wxLogDebug(wxT("wxPropertyGridPageState::DoSetPropertyValues Warning: Setting value of property \"%s\" from variant"),
                                p->GetName().c_str());
                        }
                #endif

                        p->SetValue(*current);
                    }
                }
                else
                {
                    // Is it list?
                    if ( current->GetType() != wxS("list") )
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
            if ( name.length() > 0 )
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
                                wxASSERT( current->GetType() == wxPG_VARIANT_TYPE_LIST );

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

    // This will allow better behavior.
    if ( scheduledParent == m_properties )
        scheduledParent = (wxPGProperty*) NULL;

    if ( scheduledParent && !scheduledParent->IsCategory() )
    {
        wxASSERT_MSG( property->GetBaseName().length(),
                      "Property's children must have unique, non-empty names within their scope" );
    }

    property->m_parentState = this;

    if ( property->IsCategory() )
    {

        // Parent of a category must be either root or another category
        // (otherwise Bad Things might happen).
        wxASSERT_MSG( scheduledParent == NULL ||
                      scheduledParent == m_properties ||
                      scheduledParent->IsCategory(),
                 wxT("Parent of a category must be either root or another category."));

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

#ifdef __WXDEBUG__
    // Warn for identical names in debug mode.
    if ( BaseGetPropertyByName(property->GetName()) &&
         (!scheduledParent || scheduledParent->IsCategory()) )
    {
        wxLogError(wxT("wxPropertyGrid: Warning - item with name \"%s\" already exists."),
            property->GetName().c_str());
        wxPGGlobalVars->m_warnings++;
    }
#endif

    // Make sure nothing is selected.
    if ( propGrid )
        propGrid->ClearSelection(false);

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
        cur_cat = (wxPropertyCategory*) NULL;

    return DoInsert( cur_cat, -1, property );
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridPageState::DoInsert( wxPGProperty* parent, int index, wxPGProperty* property )
{
    if ( !parent )
        parent = m_properties;

    wxCHECK_MSG( !parent->HasFlag(wxPG_PROP_AGGREGATE),
                 wxNullProperty,
                 wxT("when adding properties to fixed parents, use BeginAddChildren and EndAddChildren.") );

    bool res = PrepareToAddItem( property, (wxPropertyCategory*)parent );

    // PrepareToAddItem() may just decide to use use current category
    // instead of adding new one.
    if ( !res )
        return m_currentCategory;

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

    if ( !parent->IsCategory() && !parent->IsRoot() )
    {
        // Parent is wxParentingProperty: Just stick it in...
        parent->AddChild2( property, index );
    }
    else
    {
        // Parent is Category or Root.

        if ( m_properties == &m_regularArray )
        {
            // Categorized mode

            // Only add non-categories to m_abcArray.
            if ( m_abcArray && !property->IsCategory() )
                m_abcArray->AddChild2( property, -1, false );

            // Add to current mode.
            parent->AddChild2( property, index );

        }
        else
        {
            // Non-categorized mode.

            if ( parent != m_properties )
                // Parent is category.
                parent->AddChild2( property, index, false );
            else
                // Parent is root.
                m_regularArray.AddChild2( property, -1, false );

            // Add to current mode (no categories).
            if ( !property->IsCategory() )
                m_abcArray->AddChild2( property, index );
        }
    }

    // category stuff
    if ( property->IsCategory() )
    {
        // This is a category caption item.

        // Last caption is not the bottom one (this info required by append)
        m_lastCaptionBottomnest = 0;
    }

    // Only add name to hashmap if parent is root or category
    if ( property->m_name.length() &&
        (parent->IsCategory() || parent->IsRoot()) )
        m_dictName[property->m_name] = (void*) property;

    VirtualHeightChanged();

    property->UpdateParentValues();

    m_itemsAdded = 1;

    return property;
}

// -----------------------------------------------------------------------

void wxPropertyGridPageState::DoDelete( wxPGProperty* item, bool doDelete )
{
    wxCHECK_RET( item->GetParent(),
        wxT("this property was already deleted") );

    wxCHECK_RET( item != &m_regularArray && item != m_abcArray,
        wxT("wxPropertyGrid: Do not attempt to remove the root item.") );

    unsigned int indinparent = item->GetIndexInParent();

    wxPGProperty* pwc = (wxPGProperty*)item;
    wxPGProperty* parent = item->GetParent();

    wxCHECK_RET( !parent->HasFlag(wxPG_PROP_AGGREGATE),
        wxT("wxPropertyGrid: Do not attempt to remove sub-properties.") );

    // Delete children
    if ( item->GetChildCount() && !item->HasFlag(wxPG_PROP_AGGREGATE) )
    {
        // deleting a category
        if ( item->IsCategory() )
        {
            if ( pwc == m_currentCategory )
                m_currentCategory = (wxPropertyCategory*) NULL;
        }

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
        wxArrayPGProperty& parentsChildren = parent->m_children;
        parentsChildren.erase( parentsChildren.begin() + indinparent );
        item->m_parent->FixIndicesOfChildren();
    }
    else
    {
        // non-categorized mode - categorized array

        // We need to find location of item.
        wxPGProperty* cat_parent = &m_regularArray;
        int cat_index = m_regularArray.GetChildCount();
        size_t i;
        for ( i = 0; i < m_regularArray.GetChildCount(); i++ )
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
        cat_parent->m_children.erase(cat_parent->m_children.begin()+cat_index);

        // non-categorized mode - non-categorized array
        if ( !item->IsCategory() )
        {
            wxASSERT( item->m_parent == m_abcArray );
            wxArrayPGProperty& parentsChildren = item->m_parent->m_children;
            parentsChildren.erase(parentsChildren.begin() + indinparent);
            item->m_parent->FixIndicesOfChildren(indinparent);
        }
    }

    if ( item->GetBaseName().length() && 
         (parent->IsCategory() || parent->IsRoot()) )
        m_dictName.erase(item->GetBaseName());

    // We can actually delete it now
    if ( doDelete )
        delete item;

    m_itemsAdded = 1; // Not a logical assignment (but required nonetheless).

    VirtualHeightChanged();
}

// -----------------------------------------------------------------------

#endif  // wxUSE_PROPGRID

///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/radiocmn.cpp
// Purpose:     wxRadioBox methods common to all ports
// Author:      Vadim Zeitlin
// Modified by:
// Created:     03.06.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "radioboxbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_RADIOBOX

#ifndef WX_PRECOMP
    #include "wx/radiobox.h"
#endif //WX_PRECOMP

// ============================================================================
// implementation
// ============================================================================

int wxRadioBoxBase::GetNextItem(int item, wxDirection dir, long style) const
{
    int count = GetCount(),
        numCols = GetColumnCount(),
        numRows = GetRowCount();

    bool horz = (style & wxRA_SPECIFY_COLS) != 0;

    switch ( dir )
    {
        case wxUP:
            if ( horz )
            {
                item -= numCols;
            }
            else // vertical layout
            {
                if ( !item-- )
                    item = count - 1;
            }
            break;

        case wxLEFT:
            if ( horz )
            {
                if ( !item-- )
                    item = count - 1;
            }
            else // vertical layout
            {
                item -= numRows;
            }
            break;

        case wxDOWN:
            if ( horz )
            {
                item += numCols;
            }
            else // vertical layout
            {
                if ( ++item == count )
                    item = 0;
            }
            break;

        case wxRIGHT:
            if ( horz )
            {
                if ( ++item == count )
                    item = 0;
            }
            else // vertical layout
            {
                item += numRows;
            }
            break;

        default:
            wxFAIL_MSG( _T("unexpected wxDirection value") );
            return wxNOT_FOUND;
    }

    // ensure that the item is in range [0..count)
    if ( item < 0 )
    {
        // first map the item to the one in the same column but in the last row
        item += count;

        // now there are 2 cases: either it is the first item of the last row
        // in which case we need to wrap again and get to the last item or we
        // can just go to the previous item
        if ( item % (horz ? numCols : numRows) )
            item--;
        else
            item = count - 1;
    }
    else if ( item >= count )
    {
        // same logic as above
        item -= count;

        // ... except that we need to check if this is not the last item, not
        // the first one
        if ( (item + 1) % (horz ? numCols : numRows) )
            item++;
        else
            item = 0;
    }

    wxASSERT_MSG( item < count && item >= 0,
                  _T("logic error in wxRadioBox::GetNextItem()") );

    return item;
}

#if WXWIN_COMPATIBILITY_2_4

// these functions are deprecated and don't do anything
int wxRadioBoxBase::GetNumberOfRowsOrCols() const
{
    return 1;
}

void wxRadioBoxBase::SetNumberOfRowsOrCols(int WXUNUSED(n))
{
}

#endif // WXWIN_COMPATIBILITY_2_4

#endif // wxUSE_RADIOBOX


///////////////////////////////////////////////////////////////////////////
// Name:        src/generic/gridsel.cpp
// Purpose:     wxGridSelection
// Author:      Stefan Neis
// Created:     20/02/1999
// Copyright:   (c) Stefan Neis (Stefan.Neis@t-online.de)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if wxUSE_GRID

#include "wx/generic/gridsel.h"
#include "wx/dynarray.h"

#include "wx/generic/private/grid.h"

namespace
{

// The helper function to compare wxIntSortedArray elements.
int CompareInts(int n1, int n2)
{
    return n1 - n2;
}

}

WX_DEFINE_SORTED_ARRAY_CMP_INT(int, CompareInts, wxIntSortedArray);


wxGridSelection::wxGridSelection( wxGrid * grid,
                                  wxGrid::wxGridSelectionModes sel )
{
    m_grid = grid;
    m_selectionMode = sel;
}

bool wxGridSelection::IsSelection()
{
    return !m_selection.empty();
}

void wxGridSelection::EndSelecting()
{
    // It's possible that nothing was selected finally, e.g. the mouse could
    // have been dragged around only to return to the starting cell, just don't
    // do anything in this case.
    if ( !IsSelection() )
        return;

    // Send RANGE_SELECTED event for the last modified block.
    const wxGridBlockCoords& block = m_selection.back();
    wxGridRangeSelectEvent gridEvt(m_grid->GetId(),
                                   wxEVT_GRID_RANGE_SELECTED,
                                   m_grid,
                                   block.GetTopLeft(),
                                   block.GetBottomRight(),
                                   true);

    m_grid->GetEventHandler()->ProcessEvent(gridEvt);
}

void wxGridSelection::CancelSelecting()
{
    // It's possible that nothing was selected finally, e.g. the mouse could
    // have been dragged around only to return to the starting cell, just don't
    // do anything in this case.
    if ( !IsSelection() )
        return;

    const wxGridBlockCoords& block = m_selection.back();
    m_grid->RefreshBlock(block.GetTopLeft(), block.GetBottomRight());
    m_selection.pop_back();
}


bool wxGridSelection::IsInSelection( int row, int col ) const
{
    // Check whether the given cell is contained in one of the selected blocks.
    //
    // Note that this algorithm is O(N) in number of selected blocks, not in
    // number of cells in the grid, so it should be reasonably efficient even
    // for very large grids, as the user shouldn't be able to select too many
    // blocks. If we still run into problems with this, we should find a more
    // efficient way of storing the selection, e.g. using k-d trees.
    const size_t count = m_selection.size();
    for ( size_t n = 0; n < count; n++ )
    {
        if ( m_selection[n].Contains(wxGridCellCoords(row, col)) )
            return true;
    }

    return false;
}

// Change the selection mode
void wxGridSelection::SetSelectionMode( wxGrid::wxGridSelectionModes selmode )
{
    // if selection mode is unchanged return immediately
    if (selmode == m_selectionMode)
        return;

    if (selmode == wxGrid::wxGridSelectNone)
    {
        ClearSelection();
        m_selectionMode = selmode;
        return;
    }

    if ( m_selectionMode != wxGrid::wxGridSelectCells )
    {
        // if changing form row to column selection
        // or vice versa, clear the selection.
        if ( selmode != wxGrid::wxGridSelectCells )
            ClearSelection();

        m_selectionMode = selmode;
    }
    else
    {
        // Preserve only fully selected rows/columns when switching from cell
        // selection mode and discard the selected blocks that are invalid in
        // the new selection mode.
        const int lastCol = m_grid->GetNumberCols() - 1;
        const int lastRow = m_grid->GetNumberRows() - 1;
        for ( size_t n = m_selection.size(); n > 0; )
        {
            n--;
            const wxGridBlockCoords& block = m_selection[n];
            const int topRow = block.GetTopRow();
            const int leftCol = block.GetLeftCol();
            const int bottomRow = block.GetBottomRow();
            const int rightCol = block.GetRightCol();

            bool valid = false;
            switch ( selmode )
            {
                case wxGrid::wxGridSelectCells:
                case wxGrid::wxGridSelectNone:
                    wxFAIL_MSG("unreachable");
                    break;

                case wxGrid::wxGridSelectRows:
                    valid = leftCol == 0 && rightCol == lastCol;
                    break;

                case wxGrid::wxGridSelectColumns:
                    valid = topRow == 0 && bottomRow == lastRow;
                    break;

                case wxGrid::wxGridSelectRowsOrColumns:
                    valid = (leftCol == 0 && rightCol == lastCol) ||
                            (topRow == 0 && bottomRow == lastRow);
                    break;
            }

            if ( !valid )
            {
                m_selection.erase(m_selection.begin() + n);

                if ( m_grid->UsesOverlaySelection() )
                {
                    ComputeSelectionShape();
                }
                else if ( !m_grid->GetBatchCount() )
                {
                    m_grid->RefreshBlock(block.GetTopLeft(), block.GetBottomRight());
                }
            }
        }

        m_selectionMode = selmode;
    }
}

void wxGridSelection::SelectRow(int row, const wxKeyboardState& kbd)
{
    if ( m_selectionMode == wxGrid::wxGridSelectColumns ||
            m_selectionMode == wxGrid::wxGridSelectNone )
        return;

    Select(wxGridBlockCoords(row, 0, row, m_grid->GetNumberCols() - 1),
           kbd, wxEVT_GRID_RANGE_SELECTED);
}

void wxGridSelection::SelectCol(int col, const wxKeyboardState& kbd)
{
    if ( m_selectionMode == wxGrid::wxGridSelectRows ||
            m_selectionMode == wxGrid::wxGridSelectNone )
        return;

    Select(wxGridBlockCoords(0, col, m_grid->GetNumberRows() - 1, col),
           kbd, wxEVT_GRID_RANGE_SELECTED);
}

void wxGridSelection::SelectBlock( int topRow, int leftCol,
                                   int bottomRow, int rightCol,
                                   const wxKeyboardState& kbd,
                                   wxEventType eventType )
{
    // Fix the coordinates of the block if needed.
    int allowed = -1;
    switch ( m_selectionMode )
    {
        case wxGrid::wxGridSelectCells:
            // In this mode arbitrary blocks can be selected.
            allowed = 1;
            break;

        case wxGrid::wxGridSelectRows:
            leftCol = 0;
            rightCol = m_grid->GetNumberCols() - 1;
            allowed = 1;
            break;

        case wxGrid::wxGridSelectColumns:
            topRow = 0;
            bottomRow = m_grid->GetNumberRows() - 1;
            allowed = 1;
            break;

        case wxGrid::wxGridSelectRowsOrColumns:
            // Arbitrary block selection doesn't make sense for this mode, as
            // we could only select the entire grid, which wouldn't be useful,
            // but we do allow selecting blocks that are already composed of
            // only rows or only columns.
            if ( topRow == 0 && bottomRow == m_grid->GetNumberRows() - 1 )
                allowed = 1;
            else if ( leftCol == 0 && rightCol == m_grid->GetNumberCols() - 1 )
                allowed = 1;
            else
                allowed = 0;
            break;

        case wxGrid::wxGridSelectNone:
            allowed = 0;
            break;
    }

    wxASSERT_MSG(allowed != -1, "unknown selection mode?");
    if ( !allowed )
        return;

    Select(wxGridBlockCoords(topRow, leftCol, bottomRow, rightCol).Canonicalize(),
           kbd, eventType);
}

void
wxGridSelection::SelectAll()
{
    // There is no need to refresh anything, as Select() will do it anyhow, and
    // no need to generate any events, so do not call ClearSelection() here.
    m_selection.clear();

    const int numRows = m_grid->GetNumberRows();
    const int numCols = m_grid->GetNumberCols();

    if ( numRows && numCols )
    {
        Select(wxGridBlockCoords(0, 0, numRows - 1, numCols - 1),
               wxKeyboardState(), wxEVT_GRID_RANGE_SELECTED);
    }
}

void
wxGridSelection::DeselectBlock(const wxGridBlockCoords& block,
                               const wxKeyboardState& kbd,
                               wxEventType eventType)
{
    // In wxGridSelectNone mode, all blocks should already be deselected.
    if ( m_selectionMode == wxGrid::wxGridSelectNone )
        return;

    const wxGridBlockCoords canonicalizedBlock = block.Canonicalize();

    size_t count, n;

    // If the selected block intersects with the deselection block, split it
    // in up to 4 new parts, that don't contain the block to be selected, like
    // this (for rows):
    // |---------------------------|
    // |                           |
    // |           part 1          |
    // |                           |
    // |---------------------------|
    // | part 3 |    x    | part 4 |
    // |---------------------------|
    // |                           |
    // |           part 2          |
    // |                           |
    // |---------------------------|
    // And for columns:
    // |---------------------------|
    // |        |         |        |
    // |        | part 3  |        |
    // |        |         |        |
    // |        |---------|        |
    // | part 1 |    x    | part 2 |
    // |        |---------|        |
    // |        |         |        |
    // |        | part 4  |        |
    // |        |         |        |
    // |---------------------------|
    //   (The x marks the newly deselected block).
    // Note: in row/column selection mode, we only need part1 and part2

    // Blocks to refresh.
    wxGridBlockCoordsVector refreshBlocks;
    // Add the deselected block.
    refreshBlocks.push_back(canonicalizedBlock);

    count = m_selection.size();
    for ( n = 0; n < count; n++ )
    {
        const wxGridBlockCoords& selBlock = m_selection[n];

        // Whether blocks intersect.
        if ( !m_selection[n].Intersects(canonicalizedBlock) )
            continue;

        int splitOrientation = -1;
        switch ( m_selectionMode )
        {
        case wxGrid::wxGridSelectRows:
            splitOrientation = wxHORIZONTAL;
            break;

        case wxGrid::wxGridSelectColumns:
            splitOrientation = wxVERTICAL;
            break;

        case wxGrid::wxGridSelectCells:
        case wxGrid::wxGridSelectRowsOrColumns:
            if ( selBlock.GetLeftCol() == 0 &&
                 selBlock.GetRightCol() == m_grid->GetNumberCols() - 1 )
                splitOrientation = wxHORIZONTAL;
            else
                splitOrientation = wxVERTICAL;
            break;

        case wxGrid::wxGridSelectNone:
            wxFAIL_MSG("unreachable");
            break;
        }

        wxASSERT_MSG( splitOrientation != -1, "unknown selection mode" );

        const wxGridBlockDiffResult result =
            selBlock.Difference(canonicalizedBlock, splitOrientation);

        // remove the block (note that selBlock, being a reference, is
        // invalidated here and can't be used any more below)
        m_selection.erase(m_selection.begin() + n);
        n--;
        count--;

        for ( int i = 0; i < 2; ++i )
        {
            const wxGridBlockCoords& part = result.m_parts[i];
            if ( part != wxGridNoBlockCoords )
                SelectBlockNoEvent(part);
        }

        for ( int i = 2; i < 4; ++i )
        {
            const wxGridBlockCoords& part = result.m_parts[i];
            if ( part != wxGridNoBlockCoords )
            {
                // Add part[2] and part[3] only in the cells selection mode.
                if ( m_selectionMode == wxGrid::wxGridSelectCells )
                    SelectBlockNoEvent(part);
                else
                    MergeOrAddBlock(refreshBlocks, part);
            }
        }
    }

    wxGridPrivate::MergeAdjacentBlocks(m_selection);

    // Refresh the screen and send events.

    if ( m_grid->UsesOverlaySelection() )
    {
        ComputeSelectionShape();
    }

    count = refreshBlocks.size();
    for ( n = 0; n < count; n++ )
    {
        const wxGridBlockCoords& refBlock = refreshBlocks[n];

        if ( !m_grid->UsesOverlaySelection() && !m_grid->GetBatchCount() )
        {
            m_grid->RefreshBlock(refBlock.GetTopLeft(), refBlock.GetBottomRight());
        }

        if ( eventType != wxEVT_NULL )
        {
            wxGridRangeSelectEvent gridEvt(m_grid->GetId(),
                                           eventType,
                                           m_grid,
                                           refBlock.GetTopLeft(),
                                           refBlock.GetBottomRight(),
                                           false,
                                           kbd);
            m_grid->GetEventHandler()->ProcessEvent(gridEvt);
        }
    }
}

void wxGridSelection::ClearSelection()
{
    size_t n;
    wxRect r;
    wxGridCellCoords coords1, coords2;

    if ( m_grid->UsesOverlaySelection() )
    {
        m_selection.clear();

        ComputeSelectionShape();
    }
    else
    {
        // deselect all blocks and update the screen
        while ( ( n = m_selection.size() ) > 0)
        {
            n--;
            const wxGridBlockCoords& block = m_selection[n];
            coords1 = block.GetTopLeft();
            coords2 = block.GetBottomRight();
            m_selection.erase(m_selection.begin() + n);
            if ( !m_grid->GetBatchCount() )
            {
                m_grid->RefreshBlock(coords1, coords2);

#ifdef __WXMAC__
                m_grid->UpdateGridWindows();
#endif
            }
        }
    }

    // One deselection event, indicating deselection of _all_ cells.
    // (No finer grained events for each of the smaller regions
    //  deselected above!)
    wxGridRangeSelectEvent gridEvt( m_grid->GetId(),
                                    wxEVT_GRID_RANGE_SELECTED,
                                    m_grid,
                                    wxGridCellCoords( 0, 0 ),
                                    wxGridCellCoords(
                                        m_grid->GetNumberRows() - 1,
                                        m_grid->GetNumberCols() - 1 ),
                                    false );

    m_grid->GetEventHandler()->ProcessEvent(gridEvt);
}


void wxGridSelection::UpdateRows( size_t pos, int numRows )
{
    size_t count = m_selection.size();
    size_t n;

    for ( n = 0; n < count; n++ )
    {
        wxGridBlockCoords& block = m_selection[n];
        wxCoord row1 = block.GetTopRow();
        wxCoord row2 = block.GetBottomRow();

        if ((size_t)row2 >= pos)
        {
            if (numRows > 0)
            {
                // If rows inserted, increase row counter where necessary
                block.SetBottomRow( row2 + numRows );
                if ((size_t)row1 >= pos)
                    block.SetTopRow( row1 + numRows );
            }
            else if (numRows < 0)
            {
                // If rows deleted ...
                if ((size_t)row2 >= pos - numRows)
                {
                    // ...either decrement row counter (if row still exists)...
                    block.SetBottomRow( row2 + numRows );
                    if ((size_t)row1 >= pos)
                        block.SetTopRow( wxMax(row1 + numRows, (int)pos) );

                }
                else
                {
                    if ((size_t)row1 >= pos)
                    {
                        // ...or remove the attribute
                        m_selection.erase(m_selection.begin() + n);
                        n--;
                        count--;
                    }
                    else
                        block.SetBottomRow( pos - 1 );
                }
            }
        }
    }
}


void wxGridSelection::UpdateCols( size_t pos, int numCols )
{
    size_t count = m_selection.size();
    size_t n;

    for ( n = 0; n < count; n++ )
    {
        wxGridBlockCoords& block = m_selection[n];
        wxCoord col1 = block.GetLeftCol();
        wxCoord col2 = block.GetRightCol();

        if ((size_t)col2 >= pos)
        {
            if (numCols > 0)
            {
                // If rows inserted, increase row counter where necessary
                block.SetRightCol(col2 + numCols);
                if ((size_t)col1 >= pos)
                    block.SetLeftCol(col1 + numCols);
            }
            else if (numCols < 0)
            {
                // If cols deleted ...
                if ((size_t)col2 >= pos - numCols)
                {
                    // ...either decrement col counter (if col still exists)...
                    block.SetRightCol(col2 + numCols);
                    if ( (size_t) col1 >= pos)
                        block.SetLeftCol( wxMax(col1 + numCols, (int)pos) );

                }
                else
                {
                    if ((size_t)col1 >= pos)
                    {
                        // ...or remove the attribute
                        m_selection.erase(m_selection.begin() + n);
                        n--;
                        count--;
                    }
                    else
                        block.SetRightCol( pos - 1 );
                }
            }
        }
    }
}

bool wxGridSelection::ExtendCurrentBlock(const wxGridCellCoords& blockStart,
                                         const wxGridCellCoords& blockEnd,
                                         const wxKeyboardState& kbd,
                                         wxEventType eventType)
{
    wxASSERT( blockStart.GetRow() != -1 && blockStart.GetCol() != -1 &&
              blockEnd.GetRow() != -1 && blockEnd.GetCol() != -1 );

    if ( m_selectionMode == wxGrid::wxGridSelectNone )
        return false;

    // If selection doesn't contain the current cell (which also covers the
    // special case of nothing being selected yet), we have to create a new
    // block containing it because it doesn't make sense to extend any existing
    // block to non-selected current cell.

    if ( !IsInSelection(m_grid->GetGridCursorCoords()) )
    {
        SelectBlock(blockStart, blockEnd, kbd, eventType);
        return true;
    }

    const wxGridBlockCoords& block = *m_selection.rbegin();
    wxGridBlockCoords newBlock = block;

    // Determine if we should try to extend the current block rows and/or
    // columns at all.
    bool canChangeRow = false,
         canChangeCol = false;

    switch ( m_selectionMode )
    {
        case wxGrid::wxGridSelectCells:
            // Nothing prevents us from doing it in this case.
            canChangeRow =
            canChangeCol = true;
            break;

        case wxGrid::wxGridSelectColumns:
            // Rows are always fixed, so prevent us from ever selecting only
            // part of a column in this case by leaving canChangeRow false.
            canChangeCol = true;
            break;

        case wxGrid::wxGridSelectRows:
            // Same as above but mirrored.
            canChangeRow = true;
            break;

        case wxGrid::wxGridSelectRowsOrColumns:
            // In this case we may only change component which is not fixed.
            if ( block.GetTopRow() != 0 ||
                    block.GetBottomRow() != m_grid->GetNumberRows() - 1 )
            {
                // This is a row block, so we can extend it in row direction.
                canChangeRow = true;
            }
            else if ( block.GetLeftCol() != 0 ||
                        block.GetRightCol() != m_grid->GetNumberCols() - 1 )
            {
                canChangeCol = true;
            }
            else // The entire grid is selected.
            {
                // In this case we can shrink it in either direction.
                canChangeRow =
                canChangeCol = true;
            }
            break;

        case wxGrid::wxGridSelectNone:
            wxFAIL_MSG("unreachable");
            break;
    }

    if ( canChangeRow )
    {
        // If the new block starts at the same top row as the current one, the
        // end block coordinates must correspond to the new bottom row -- and
        // vice versa, if the new block starts at the bottom, its other end
        // must correspond to the top.
        if ( blockStart.GetRow() == block.GetTopRow() )
        {
            newBlock.SetBottomRow(blockEnd.GetRow());
        }
        else if ( blockStart.GetRow() == block.GetBottomRow() )
        {
            newBlock.SetTopRow(blockEnd.GetRow());
        }
        else // current and new block don't have common row boundary
        {
            // This can happen when mixing entire column and cell selection, e.g.
            // by Shift-clicking on the column header. In this case, the right
            // thing to do is to just expand the current block to the new one
            // boundaries, extending the selection to the entire column height when
            // a column is selected. However notice that we should not shrink the
            // current block here, in order to allow Shift-Left/Right (which don't
            // know anything about the column selection and so just use single row
            // blocks) to keep the full column selection.
            int top = blockStart.GetRow(),
                bottom = blockEnd.GetRow();
            if ( top > bottom )
                wxSwap(top, bottom);

            if ( top < newBlock.GetTopRow() )
                newBlock.SetTopRow(top);
            if ( bottom > newBlock.GetBottomRow() )
                newBlock.SetBottomRow(bottom);
        }
    }

    // Same as above but mirrored for columns.
    if ( canChangeCol )
    {
        if ( blockStart.GetCol() == block.GetLeftCol() )
        {
            newBlock.SetRightCol(blockEnd.GetCol());
        }
        else if ( blockStart.GetCol() == block.GetRightCol() )
        {
            newBlock.SetLeftCol(blockEnd.GetCol());
        }
        else
        {
            int left = blockStart.GetCol(),
                right = blockEnd.GetCol();
            if ( left > right )
                wxSwap(left, right);

            if ( left < newBlock.GetLeftCol() )
                newBlock.SetLeftCol(left);
            if ( right > newBlock.GetRightCol() )
                newBlock.SetRightCol(right);
        }
    }

    newBlock = newBlock.Canonicalize();

    if ( newBlock == block )
        return false;

    // Update View.
    if ( !m_grid->UsesOverlaySelection() && !m_grid->GetBatchCount() )
    {
        wxGridBlockDiffResult refreshBlocks = block.SymDifference(newBlock);
        for ( int i = 0; i < 4; ++i )
        {
            const wxGridBlockCoords& refreshBlock = refreshBlocks.m_parts[i];
            m_grid->RefreshBlock(refreshBlock.GetTopLeft(),
                                 refreshBlock.GetBottomRight());
        }
    }

    // Update the current block in place.
    *m_selection.rbegin() = newBlock;

    if ( m_grid->UsesOverlaySelection() )
    {
        ComputeSelectionShape();
    }

    // Send Event.
    wxGridRangeSelectEvent gridEvt(m_grid->GetId(),
                                    eventType,
                                    m_grid,
                                    newBlock.GetTopLeft(),
                                    newBlock.GetBottomRight(),
                                    true,
                                    kbd);
    m_grid->GetEventHandler()->ProcessEvent(gridEvt);

    return true;
}

wxGridCellCoords wxGridSelection::GetExtensionAnchor() const
{
    wxGridCellCoords coords = m_grid->m_currentCellCoords;

    // If the current cell isn't selected (which also covers the special case
    // of nothing being selected yet), we have to use it as anchor as we need
    // to ensure that it will get selected.
    if ( !IsInSelection(coords) )
        return coords;

    const wxGridBlockCoords& block = *m_selection.rbegin();
    if ( block.GetTopRow() == coords.GetRow() )
        coords.SetRow(block.GetBottomRow());
    else if ( block.GetBottomRow() == coords.GetRow() )
        coords.SetRow(block.GetTopRow());

    if ( block.GetLeftCol() == coords.GetCol() )
        coords.SetCol(block.GetRightCol());
    else if ( block.GetRightCol() == coords.GetCol() )
        coords.SetCol(block.GetLeftCol());

    return coords;
}

wxGridCellCoordsArray wxGridSelection::GetCellSelection() const
{
    if ( m_selectionMode != wxGrid::wxGridSelectCells )
        return wxGridCellCoordsArray();

    wxGridCellCoordsArray cells;
    const size_t count = m_selection.size();
    cells.reserve(count);
    for ( size_t n = 0; n < count; n++ )
    {
        const wxGridBlockCoords& block = m_selection[n];
        if ( block.GetTopRow() == block.GetBottomRow() &&
             block.GetLeftCol() == block.GetRightCol() )
        {
            cells.push_back(block.GetTopLeft());
        }
    }
    return cells;
}

wxGridCellCoordsArray wxGridSelection::GetBlockSelectionTopLeft() const
{
    wxGridCellCoordsArray coords;
    const size_t count = m_selection.size();
    coords.reserve(count);
    for ( size_t n = 0; n < count; n++ )
    {
        coords.push_back(m_selection[n].GetTopLeft());
    }
    return coords;
}

wxGridCellCoordsArray wxGridSelection::GetBlockSelectionBottomRight() const
{
    wxGridCellCoordsArray coords;
    const size_t count = m_selection.size();
    coords.reserve(count);
    for ( size_t n = 0; n < count; n++ )
    {
        coords.push_back(m_selection[n].GetBottomRight());
    }
    return coords;
}

// For compatibility with the existing code, try reconstructing the selected
// rows/columns from the selected blocks we store internally. Of course, this
// only works well in the corresponding selection mode in which the user can
// only select the entire lines in the first place, as otherwise it's difficult to
// efficiently determine that a line is selected because all of its cells
// were selected one by one. But this should work well enough in practice and
// is, anyhow, the best we can do.
wxArrayInt wxGridSelection::GetRowSelection() const
{
    if ( m_selectionMode == wxGrid::wxGridSelectColumns ||
            m_selectionMode == wxGrid::wxGridSelectNone )
        return wxArrayInt();

    wxIntSortedArray uniqueRows;
    const size_t count = m_selection.size();
    for ( size_t n = 0; n < count; ++n )
    {
        const wxGridBlockCoords& block = m_selection[n];
        if ( block.GetLeftCol() == 0 &&
             block.GetRightCol() == m_grid->GetNumberCols() - 1 )
        {
            for ( int r = block.GetTopRow(); r <= block.GetBottomRow(); ++r )
            {
                if ( uniqueRows.Index(r) == wxNOT_FOUND )
                    uniqueRows.Add(r);
            }
        }
    }

    wxArrayInt result;
    result.reserve(uniqueRows.size());
    for( size_t i = 0; i < uniqueRows.size(); ++i )
    {
        result.push_back(uniqueRows[i]);
    }
    return result;
}

// See comments for GetRowSelection().
wxArrayInt wxGridSelection::GetColSelection() const
{
    if ( m_selectionMode == wxGrid::wxGridSelectRows ||
            m_selectionMode == wxGrid::wxGridSelectNone )
        return wxArrayInt();

    wxIntSortedArray uniqueCols;
    const size_t count = m_selection.size();
    for ( size_t n = 0; n < count; ++n )
    {
        const wxGridBlockCoords& block = m_selection[n];
        if ( block.GetTopRow() == 0 &&
             block.GetBottomRow() == m_grid->GetNumberRows() - 1 )
        {
            for ( int c = block.GetLeftCol(); c <= block.GetRightCol(); ++c )
            {
                if ( uniqueCols.Index(c) == wxNOT_FOUND )
                    uniqueCols.Add(c);
            }
        }
    }

    wxArrayInt result;
    result.reserve(uniqueCols.size());
    for( size_t i = 0; i < uniqueCols.size(); ++i )
    {
        result.push_back(uniqueCols[i]);
    }
    return result;
}

void
wxGridSelection::Select(const wxGridBlockCoords& block,
                        const wxKeyboardState& kbd,
                        wxEventType eventType)
{
    if (m_grid->GetNumberRows() == 0 || m_grid->GetNumberCols() == 0)
        return;

    m_selection.push_back(block);

    wxGridPrivate::MergeAdjacentBlocks(m_selection);

    // Update View:
    if ( m_grid->UsesOverlaySelection() )
    {
        ComputeSelectionShape();
    }
    else if ( !m_grid->GetBatchCount() )
    {
        m_grid->RefreshBlock(block.GetTopLeft(), block.GetBottomRight());
    }

#if wxUSE_ACCESSIBILITY
    wxAccessible::NotifyEvent(wxACC_EVENT_OBJECT_SELECTIONWITHIN, m_grid, wxOBJID_CLIENT, wxACC_SELF);
#endif // wxUSE_ACCESSIBILITY

    // Send Event, if not disabled.
    if ( eventType != wxEVT_NULL )
    {
        wxGridRangeSelectEvent gridEvt( m_grid->GetId(),
            eventType,
            m_grid,
            block.GetTopLeft(),
            block.GetBottomRight(),
            true,
            kbd);
        m_grid->GetEventHandler()->ProcessEvent( gridEvt );
    }
}

void wxGridSelection::MergeOrAddBlock(wxGridBlockCoordsVector& blocks,
                                      const wxGridBlockCoords& newBlock)
{
    size_t count = blocks.size();
    for ( size_t n = 0; n < count; n++ )
    {
        const wxGridBlockCoords& block = blocks[n];

        if ( block.Contains(newBlock) )
            return;

        if ( newBlock.Contains(block) )
        {
            blocks.erase(blocks.begin() + n);
            n--;
            count--;
        }
    }

    blocks.push_back(newBlock);
}

void wxGridPrivate::MergeAdjacentBlocks(wxGridBlockCoordsVector& selection)
{
    auto CanMergeBlocks = [](const wxGridBlockCoords& b1,
                             const wxGridBlockCoords& b2) -> bool
    {
        if ( b1.GetLeftCol() == b2.GetLeftCol() &&
             b1.GetRightCol() == b2.GetRightCol() )
        {
            if ( std::abs(b1.GetTopRow() - b2.GetBottomRow()) == 1 ||
                 std::abs(b2.GetTopRow() - b1.GetBottomRow()) == 1 )
                return true;
        }
        else if ( b1.GetTopRow() == b2.GetTopRow() &&
                  b1.GetBottomRow() == b2.GetBottomRow() )
        {
            if ( std::abs(b1.GetLeftCol() - b2.GetRightCol()) == 1 ||
                 std::abs(b2.GetLeftCol() - b1.GetRightCol()) == 1 )
                return true;
        }

        return false;
    };

    auto outerItr = selection.begin();

    while ( outerItr != selection.end() )
    {
        auto& b1 = *outerItr++;

        auto innerItr = outerItr;

        while ( innerItr != selection.end() )
        {
            const auto& b2 = *innerItr;

            if ( CanMergeBlocks(b1, b2) )
            {
                if ( b2.GetTopRow() < b1.GetTopRow() )
                    b1.SetTopRow(b2.GetTopRow());
                if ( b2.GetLeftCol() < b1.GetLeftCol() )
                    b1.SetLeftCol(b2.GetLeftCol());
                if ( b2.GetBottomRow() > b1.GetBottomRow() )
                    b1.SetBottomRow(b2.GetBottomRow());
                if ( b2.GetRightCol() > b1.GetRightCol() )
                    b1.SetRightCol(b2.GetRightCol());

                innerItr = selection.erase(innerItr); // get rid of b2
                outerItr = selection.begin();
            }
            else
            {
                ++innerItr;
            }
        }
    }
}

void wxGridPrivate::MergeAdjacentRects(std::vector<wxRect>& rectangles)
{
    auto CanMergeRects = [](const wxRect& r1, const wxRect& r2) -> bool
    {
        if ( r1.x == r2.x && r1.width == r2.width )
        {
            if ( (r1.y > r2.y && (r2.y + r2.height == r1.y + 1)) ||
                 (r2.y > r1.y && (r1.y + r1.height == r2.y + 1)) )
                return true;
        }
        else if ( r1.y == r2.y && r1.height == r2.height )
        {
            if ( (r1.x > r2.x && (r2.x + r2.width == r1.x + 1)) ||
                 (r2.x > r1.x && (r1.x + r1.width == r2.x + 1)) )
                return true;
        }

        return false;
    };

    auto outerItr = rectangles.begin();

    while ( outerItr != rectangles.end() )
    {
        auto& r1 = *outerItr++;

        auto innerItr = outerItr;

        while ( innerItr != rectangles.end() )
        {
            const auto& r2 = *innerItr;

            if ( CanMergeRects(r1, r2) )
            {
                r1 += r2;

                innerItr = rectangles.erase(innerItr); // get rid of r2
                outerItr = rectangles.begin();
            }
            else
            {
                ++innerItr;
            }
        }
    }
}

void wxGridSelection::ComputeSelectionShape(const wxRect& renderExtent)
{
    if ( m_grid->GetBatchCount() )
        return;

    wxRect updateRect;

    if ( m_selectionShape )
    {
        // the old rect will be refreshed too.
        updateRect = m_selectionShape->GetBoundingBox();
    }

    m_selectionShape.reset(new wxSelectionShape);

    std::vector<wxRect> rectangles;
    m_grid->GetSelectedRectangles(rectangles, renderExtent);

    if ( !IsSelection() )
    {
        updateRect = {};

        m_updateHighlightedLabels = false;
    }
    else if ( rectangles.size() == 1 )
    {
        m_selectionShape->SetBoundingBox(rectangles[0]);

        updateRect.Union(rectangles[0]);
    }
    else if ( rectangles.size() > 1 )
    {
        m_selectionShape->FromRectangles(rectangles);

        updateRect.Union(m_selectionShape->GetBoundingBox());
    }
    else // out-of-view selection
    {
        m_updateHighlightedLabels = m_selection.size() == 1;
    }

    if ( !renderExtent.IsEmpty() )
    {
        // We are rendering on external DC, no need to refresh anything.
        return;
    }

    // The following code is only needed when an already selected block is scrolled
    // out of view and had corresponding labels highlighted. We need to toggle these
    // labels on or off depending on the currect m_selection. In other words, toggle
    // them on if m_selection represents only one block, otherwise toggle them off.
    if ( m_updateHighlightedLabels )
    {
        const wxGridBlockCoords& block = m_selection[0];

        updateRect  = m_grid->CellToRect(block.GetTopLeft()).Inflate(1);
        updateRect += m_grid->CellToRect(block.GetBottomRight()).Inflate(1);

        int left, top, right, bottom;

        wxGridWindow* gridWin = (wxGridWindow*)m_grid->GetGridWindow();
        const wxPoint offset = m_grid->GetGridWindowOffset(gridWin);

        // Convert to scrolled coords
        m_grid->CalcGridWindowScrolledPosition( updateRect.GetLeft() - offset.x,
                                                updateRect.GetTop() - offset.y,
                                                &left, &top, gridWin );
        m_grid->CalcGridWindowScrolledPosition( updateRect.GetRight() - offset.x,
                                                updateRect.GetBottom() - offset.y,
                                                &right, &bottom, gridWin );

        updateRect = wxRect(wxPoint(left, top), wxPoint(right, bottom));

        updateRect.Offset(offset);
    }

    m_grid->RefreshRect(!updateRect.IsEmpty() ? &updateRect : nullptr);
}

const wxSelectionShape&
wxGridSelection::GetSelectionShape(const wxRect& renderExtent)
{
    if ( !m_selectionShape )
    {
        ComputeSelectionShape(renderExtent);
    }

    return *m_selectionShape.get();
}

void wxGridSelection::InvalidateSelectionShape()
{
    m_selectionShape.reset();
}

void wxGridPrivate::SelectionShape::Append(const std::vector<wxPoint>& points)
{
    CalcBoundingBox(points);

    m_counts.push_back(points.size());
    std::copy(points.begin(), points.end(),
              std::back_inserter(m_points));
}

void wxGridPrivate::SelectionShape::CalcBoundingBox(const std::vector<wxPoint>& points)
{
    for ( const auto& point : points )
    {
        if ( point.x < m_minX )
            m_minX = point.x;
        if ( point.x > m_maxX )
            m_maxX = point.x;

        if ( point.y < m_minY )
            m_minY = point.y;
        if ( point.y > m_maxY )
            m_maxY = point.y;
    }
}

wxRect wxGridPrivate::SelectionShape::GetBoundingBox() const
{
    return wxRect(m_minX,  m_minY,
                  m_maxX - m_minX,
                  m_maxY - m_minY);
}

void wxGridPrivate::SelectionShape::SetBoundingBox(const wxRect& rect)
{
    m_minX = rect.x;
    m_minY = rect.y;
    m_maxX = rect.width + rect.x;
    m_maxY = rect.height + rect.y;
}

void wxGridPrivate::SelectionShape::FromRectangles(const std::vector<wxRect>& rectangles)
{
    // The implementation is literally the translation of the Python code found here:
    // https://stackoverflow.com/a/13851341/8528670

    std::vector<wxPoint> points = GetVertices(rectangles);

    InitHorzEdges(points);
    InitVertEdges(points);

    while ( !m_horzEdges.empty() )
    {
        EdgeType::iterator       iter = m_horzEdges.begin();
        std::pair<wxPoint, bool> pair = std::make_pair((*iter).first, false);

        m_horzEdges.erase(iter);

        // The boolean associated with the vertex is just a marker to help
        // with the construction of the polygons only.
        using PolygonType = std::vector<std::pair<wxPoint, bool>>;
        PolygonType polygon;
        polygon.push_back(pair);

        while ( true )
        {
            pair = polygon.back();

            if ( pair.second )
            {
                iter = m_horzEdges.find(pair.first);
                wxASSERT_MSG( iter != m_horzEdges.end(), "vertex not found in m_horzEdges" );

                const wxPoint& nextVertex = iter->second;
                polygon.push_back(std::make_pair(nextVertex, false));
                m_horzEdges.erase(iter);
            }
            else
            {
                iter = m_vertEdges.find(pair.first);
                wxASSERT_MSG ( iter != m_vertEdges.end(), "vertex not found in m_vertEdges" );

                const wxPoint& nextVertex = iter->second;
                polygon.push_back(std::make_pair(nextVertex, true));
                m_vertEdges.erase(iter);
            }

            if ( polygon.front() == polygon.back() )
            {
                // Closed polygon
                polygon.pop_back();
                break;
            }
        }

        // Remove implementation-markers from the polygon.
        std::vector<wxPoint> poly;

        for ( const auto& p : polygon )
        {
            poly.push_back(p.first);
        }

        for ( const auto& pt : poly )
        {
            iter = m_horzEdges.find(pt);
            if ( iter != m_horzEdges.end() )
            {
                m_horzEdges.erase(iter);
            }

            iter = m_vertEdges.find(pt);
            if ( iter != m_vertEdges.end() )
            {
                m_vertEdges.erase(iter);
            }
        }

        Append(poly);
    }
}

#endif

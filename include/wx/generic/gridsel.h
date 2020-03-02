/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/gridsel.h
// Purpose:     wxGridSelection
// Author:      Stefan Neis
// Modified by:
// Created:     20/02/2000
// Copyright:   (c) Stefan Neis
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_GRIDSEL_H_
#define _WX_GENERIC_GRIDSEL_H_

#include "wx/defs.h"

#if wxUSE_GRID

#include "wx/grid.h"

#include "wx/vector.h"

typedef wxVector<wxGridBlockCoords> wxVectorGridBlockCoords;

class WXDLLIMPEXP_CORE wxGridSelection
{
public:
    wxGridSelection(wxGrid *grid,
                    wxGrid::wxGridSelectionModes sel = wxGrid::wxGridSelectCells);

    bool IsSelection();
    bool IsInSelection(int row, int col);
    bool IsInSelection(const wxGridCellCoords& coords)
    {
        return IsInSelection(coords.GetRow(), coords.GetCol());
    }

    void SetSelectionMode(wxGrid::wxGridSelectionModes selmode);
    wxGrid::wxGridSelectionModes GetSelectionMode() { return m_selectionMode; }
    void SelectRow(int row, const wxKeyboardState& kbd = wxKeyboardState());
    void SelectCol(int col, const wxKeyboardState& kbd = wxKeyboardState());
    void SelectBlock(int topRow, int leftCol,
                     int bottomRow, int rightCol,
                     const wxKeyboardState& kbd = wxKeyboardState(),
                     bool sendEvent = true );
    void SelectBlock(const wxGridCellCoords& topLeft,
                     const wxGridCellCoords& bottomRight,
                     const wxKeyboardState& kbd = wxKeyboardState(),
                     bool sendEvent = true )
    {
        SelectBlock(topLeft.GetRow(), topLeft.GetCol(),
                    bottomRight.GetRow(), bottomRight.GetCol(),
                    kbd, sendEvent);
    }

    void ToggleCellSelection(int row, int col,
                             const wxKeyboardState& kbd = wxKeyboardState());
    void ToggleCellSelection(const wxGridCellCoords& coords,
                             const wxKeyboardState& kbd = wxKeyboardState())
    {
        ToggleCellSelection(coords.GetRow(), coords.GetCol(), kbd);
    }

    void DeselectBlock(const wxGridBlockCoords& block,
                       const wxKeyboardState& kbd = wxKeyboardState(),
                       bool sendEvent = true );

    void ClearSelection();

    void UpdateRows( size_t pos, int numRows );
    void UpdateCols( size_t pos, int numCols );

    wxGridCellCoordsArray GetCellSelection() const;
    wxGridCellCoordsArray GetBlockSelectionTopLeft() const;
    wxGridCellCoordsArray GetBlockSelectionBottomRight() const;
    wxArrayInt GetRowSelection() const;
    wxArrayInt GetColSelection() const;

private:
    int BlockContain( int topRow1, int leftCol1,
                       int bottomRow1, int rightCol1,
                       int topRow2, int leftCol2,
                       int bottomRow2, int rightCol2 );
      // returns 1, if Block1 contains Block2,
      //        -1, if Block2 contains Block1,
      //         0, otherwise

    void SelectBlockNoEvent(const wxGridBlockCoords& block)
    {
        SelectBlock(block.GetTopRow(), block.GetLeftCol(),
                    block.GetBottomRow(), block.GetRightCol(),
                    wxKeyboardState(), false);
    }

    // Really select the block and don't check for the current selection mode.
    void Select(const wxGridBlockCoords& block,
                const wxKeyboardState& kbd, bool sendEvent);

    // If the new block containing any of the passed blocks, remove them.
    // if a new block contained in the passed blockc, return.
    // Otherwise add the new block to the blocks array.
    void MergeOrAddBlock(wxVectorGridBlockCoords& blocks,
                         const wxGridBlockCoords& block);

    // The vector of selection blocks. We expect that the users select
    // relatively small amount of blocks. K-D tree can be used to speed up
    // searching speed.
    wxVectorGridBlockCoords             m_selection;

    wxGrid                              *m_grid;
    wxGrid::wxGridSelectionModes        m_selectionMode;

    wxDECLARE_NO_COPY_CLASS(wxGridSelection);
};

#endif  // wxUSE_GRID
#endif  // _WX_GENERIC_GRIDSEL_H_

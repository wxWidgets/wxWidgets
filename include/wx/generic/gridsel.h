/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/gridsel.h
// Purpose:     wxGridSelection
// Author:      Stefan Neis
// Modified by:
// Created:     20/02/2000
// RCS-ID:      $$
// Copyright:   (c) Stefan Neis
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/defs.h"

#if defined(wxUSE_NEW_GRID) && (wxUSE_NEW_GRID)

#ifndef __WXGRIDSEL_H__
#define __WXGRIDSEL_H__

#ifdef __GNUG__
#pragma interface "gridsel.h"
#endif

#include "wx/grid.h"

class WXDLLEXPORT wxGridSelection{
public:
    wxGridSelection( wxGrid * grid, wxGrid::wxGridSelectionModes sel =
                     wxGrid::wxGridSelectCells );
    bool IsSelection();
    bool IsInSelection ( int row, int col );
    void SetSelectionMode(wxGrid::wxGridSelectionModes selmode);
    void SelectRow( int row, bool addToSelected = FALSE );
    void SelectCol( int col, bool addToSelected = FALSE );
    void SelectBlock( int topRow, int leftCol, int bottomRow, int rightCol );
    void SelectCell( int row, int col);
    void ToggleCellSelection( int row, int col);
    void ClearSelection();

    void UpdateRows( size_t pos, int numRows );
    void UpdateCols( size_t pos, int numCols );

private:
    int BlockContain( int topRow1, int leftCol1,
                       int bottomRow1, int rightCol1,
                       int topRow2, int leftCol2,
                       int bottomRow2, int rightCol2 );
      // returns 1, if Block1 contains Block2,
      //        -1, if Block2 contains Block1,
      //         0, otherwise

    int BlockContainsCell( int topRow, int leftCol,
                           int bottomRow, int rightCol,
                           int row, int col )
      // returns 1, if Block contains Cell,
      //         0, otherwise
    {
        return ( topRow <= row && row <= bottomRow &&
                 leftCol <= col && col <= rightCol );
    }

    wxGridCellCoordsArray               m_cellSelection;
    wxGridCellCoordsArray               m_blockSelectionTopLeft;
    wxGridCellCoordsArray               m_blockSelectionBottomRight;
    wxArrayInt                          m_rowSelection;
    wxArrayInt                          m_colSelection;

    wxGrid                              *m_grid;
    wxGrid::wxGridSelectionModes        m_selectionMode;
};

#endif  // #ifdef __WXGRIDSEL_H__
#endif  // #ifndef wxUSE_NEW_GRID

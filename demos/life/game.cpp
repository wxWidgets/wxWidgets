/////////////////////////////////////////////////////////////////////////////
// Name:        game.cpp
// Purpose:     Life! game logic
// Author:      Guillermo Rodriguez Garcia, <guille@iies.es>
// Modified by:
// Created:     Jan/2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000, Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ==========================================================================
// declarations
// ==========================================================================

// --------------------------------------------------------------------------
// headers
// --------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "game.h"
#endif

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "game.h"

// ==========================================================================
// implementation
// ==========================================================================

// --------------------------------------------------------------------------
// Life
// --------------------------------------------------------------------------

/* Format of the cell: (32 bit integer)
 *
 * 0yyyyyyy yyyyyy0x xxxxxxxxx xxx000ff
 *
 * y = y coordinate (13 bits)
 * x = x coordinate (13 bits)
 * f = flags (2 bits)
 *
 * OK, there is a reason for this, I promise :-). But I won't explain
 * it now; it will be more clear when I implement the optimized life
 * algorithm for large universes.
 */

Life::Life(int width, int height)
{            
    m_wrap = TRUE;
    Create(width, height);
}

Life::~Life()
{
    Destroy();
}

void Life::Create(int width, int height)
{
    wxASSERT(width > 0 && height > 0 && m_cells.IsEmpty());

    m_width = width;
    m_height = height;

    // preallocate memory to speed up initialization
    m_cells.Alloc(m_width * m_height);
    m_changed.Alloc(1000);

    // add all cells
    for (int j = 0; j < m_height; j++)
        for (int i = 0; i < m_width; i++)
            m_cells.Add( MakeCell(i, j, FALSE) );
}

void Life::Destroy()
{
    m_cells.Clear();
    m_changed.Clear();
}

void Life::Clear()
{
    // set all cells in the array to dead
    for (int j = 0; j < m_height; j++)
        for (int i = 0; i < m_width; i++)
            m_cells[j * m_width + i] &= ~CELL_ALIVE;
}

Cell Life::MakeCell(int i, int j, bool alive) const
{
    return (Cell)((j << 18) | (i << 5) | (alive? CELL_ALIVE : CELL_DEAD));
}

bool Life::IsAlive(int i, int j) const
{
    wxASSERT(i >= 0 && j >= 0 && i < m_width && j < m_height);

    return (m_cells[j * m_width + i] & CELL_ALIVE);
}

bool Life::IsAlive(Cell c) const { return (c & CELL_ALIVE); };
int  Life::GetX(Cell c) const    { return (c >> 5) & 0x1fff; };
int  Life::GetY(Cell c) const    { return (c >> 18); };

Cell Life::SetCell(int i, int j, bool alive)
{
    wxASSERT(i >= 0 && j >= 0 && i < m_width && j < m_height);

    Cell c = MakeCell(i, j, alive);

    m_cells[j * m_width + i] = c;
    return c;
}

void Life::SetShape(LifeShape& shape)
{
    wxASSERT((m_width >= shape.m_width) && (m_height >= shape.m_height));

    int i0  = (m_width - shape.m_width) / 2;
    int j0  = (m_height - shape.m_height) / 2;
    char *p = shape.m_data;

    Clear();
    for (int j = j0; j < j0 + shape.m_height; j++)
        for (int i = i0; i < i0 + shape.m_width; i++)
            SetCell(i, j, *(p++) == '*');
}

bool Life::NextTic()
{
    int i, j;

    m_changed.Empty();

    /* 1st pass. Find and mark deaths and births for this generation.
     *
     * Rules:
     *   An organism with <= 1 neighbors will die due to isolation.
     *   An organism with >= 4 neighbors will die due to starvation.
     *   New organisms are born in cells with exactly 3 neighbors.
     */
    for (j = 0; j < m_height; j++)
        for (i = 0; i < m_width; i++)
        {
            int neighbors = GetNeighbors(i, j);
            bool alive    = IsAlive(i, j);

            /* Set CELL_MARK if this cell must change, clear it
             * otherwise. We cannot toggle the CELL_ALIVE bit yet
             * because all deaths and births are simultaneous (it
             * would affect neighbouring cells).
             */
            if ((!alive && neighbors == 3) ||
                (alive && (neighbors <= 1 || neighbors >= 4)))
            {
                m_cells[j * m_width + i] |= CELL_MARK;
                m_changed.Add( MakeCell(i, j, !alive) );
            }
            else
                m_cells[j * m_width + i] &= ~CELL_MARK;
        }

    /* 2nd pass. Stabilize.
     */
    for (j = 0; j < m_height; j++)
        for (i = 0; i < m_width; i++)
        {
            /* Toggle CELL_ALIVE for those cells marked in the
             * previous pass. Do not clear the CELL_MARK bit yet;
             * it is useful to know which cells have changed and
             * thus must be updated in the screen.
             */
            if (m_cells[j * m_width + i] & CELL_MARK)
                m_cells[j * m_width + i] ^= CELL_ALIVE;
        }

    return (!m_changed.IsEmpty());
}

int Life::GetNeighbors(int x, int y) const
{
    wxASSERT(x >= 0 && y >= 0 && x < m_width && y < m_height);

    int neighbors = 0;

    int i0 = (x)? (x - 1) : 0;
    int j0 = (y)? (y - 1) : 0;
    int i1 = (x < (m_width - 1))? (x + 1) : (m_width - 1);
    int j1 = (y < (m_height - 1))? (y + 1) : (m_height - 1);

    if (m_wrap && ( !x || !y || x == (m_width - 1) || y == (m_height - 1)))
    {
        // this is an outer cell and wraparound is on
        for (int j = y - 1; j <= y + 1; j++)
            for (int i = x - 1; i <= x + 1; i++)
                if (IsAlive( ((i < 0)? (i + m_width ) : (i % m_width)),
                             ((j < 0)? (j + m_height) : (j % m_height)) ))
                     neighbors++;
    }
    else
    {
        // this is an inner cell, or wraparound is off
        for (int j = j0; j <= j1; j++)
            for (int i = i0; i <= i1; i++)
                if (IsAlive(i, j))
                     neighbors++;
    }

    // do not count ourselves
    if (IsAlive(x, y)) neighbors--;

    return neighbors;
}


/////////////////////////////////////////////////////////////////////////////
// Name:        game.h
// Purpose:     Life! game logic
// Author:      Guillermo Rodriguez Garcia, <guille@iies.es>
// Modified by:
// Created:     Jan/2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000, Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _LIFE_GAME_H_
#define _LIFE_GAME_H_

#ifdef __GNUG__
    #pragma interface "game.h"
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

// --------------------------------------------------------------------------
// Cell
// --------------------------------------------------------------------------

// A Cell is just a struct which contains a pair of (i, j) coords.
// These structs are not used internally anywhere; they are just
// used to pass cell coordinates around.

struct Cell
{
    wxInt32 i;
    wxInt32 j;
};       

// --------------------------------------------------------------------------
// LifeShape
// --------------------------------------------------------------------------

// A class which holds a pattern
class LifeShape
{
public:
    LifeShape(wxString name,
              wxString desc,
              int width,
              int height,
              char *data)
    {
        m_name   = name;
        m_desc   = desc;
        m_width  = width;
        m_height = height;
        m_data   = data;
    }

    wxString  m_name;
    wxString  m_desc;
    int       m_width;
    int       m_height;
    char     *m_data;
};


// --------------------------------------------------------------------------
// Life
// --------------------------------------------------------------------------

class CellBox;

class Life
{
public:
    // ctor and dtor
    Life();
    ~Life();

    // accessors
    inline wxUint32 GetNumCells() const { return m_numcells; };
    bool IsAlive  (wxInt32 x, wxInt32 y);
    void SetCell  (wxInt32 x, wxInt32 y, bool alive = TRUE);
    void SetShape (const LifeShape &shape);

    // game control
    void Clear();
    bool NextTic();

    // navigation
    Cell FindNorth();
    Cell FindSouth();
    Cell FindWest();
    Cell FindEast();
    Cell FindCenter();

    // The following functions find cells within a given viewport; either
    // all alive cells, or only those cells which have changed since last
    // generation. You first call BeginFind() to specify the viewport,
    // then keep calling FindMore() until it returns TRUE.
    //
    // BeginFind:
    //  Specify the viewport and whether to look for alive cells or for
    //  cells which have changed since the last generation and thus need
    //  to be repainted. In this latter case, there is no distinction
    //  between newborn or just-dead cells.
    //
    // FindMore:
    //  Fills an array with cells that match the specification given with
    //  BeginFind(). The array itself belongs to the Life object and must
    //  not be modified or freed by the caller. If this function returns
    //  FALSE, then the operation is not complete: just process all cells
    //  and call FillMore() again.
    //
    void BeginFind(wxInt32 i0, wxInt32 j0,
                   wxInt32 i1, wxInt32 j1,
                   bool changed);
    bool FindMore(Cell *cells[], size_t *ncells);

private:
    // cellbox-related
    CellBox *CreateBox(wxInt32 x, wxInt32 y, wxUint32 hv);
    CellBox *LinkBox(wxInt32 x, wxInt32 y, bool create = TRUE);
    void KillBox(CellBox *c);

    // helper for BeginFind & FindMore
    void DoLine(wxInt32 i, wxInt32 j, wxUint32 alive, wxUint32 old = 0);


    CellBox  *m_head;           // list of alive boxes
    CellBox  *m_available;      // list of reusable dead boxes
    CellBox **m_boxes;          // hash table of alive boxes
    wxUint32  m_numcells;       // population (number of alive cells)
    Cell     *m_cells;          // cell array for FindMore()
    size_t    m_ncells;         // number of valid cells in cell array
    wxInt32   m_i, m_j,         // state vars for FindMore()
              m_i0, m_j0,
              m_i1, m_j1;
    bool      m_changed;
    bool      m_findmore;
};

#endif  // _LIFE_GAME_H_

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
// constants
// --------------------------------------------------------------------------

// minimum and maximum table size, in each dimension
#define LIFE_MIN 20
#define LIFE_MAX 200

// --------------------------------------------------------------------------
// Cell and CellArray;
// --------------------------------------------------------------------------

typedef long        Cell;
typedef wxArrayLong CellArray;

// --------------------------------------------------------------------------
// LifeShape
// --------------------------------------------------------------------------

class LifeShape
{
public:
    LifeShape::LifeShape(wxString name,
                         wxString desc,
                         int width, int height, char *data,
                         int fieldWidth = 20, int fieldHeight = 20,
                         bool wrap = TRUE)
    {
        m_name        = name;
        m_desc        = desc;
        m_width       = width;
        m_height      = height;
        m_data        = data;
        m_fieldWidth  = fieldWidth;
        m_fieldHeight = fieldHeight;
        m_wrap        = wrap;
    }

    wxString  m_name;
    wxString  m_desc;
    int       m_width;
    int       m_height;
    char     *m_data;
    int       m_fieldWidth;
    int       m_fieldHeight;
    bool      m_wrap;
};

// --------------------------------------------------------------------------
// Life
// --------------------------------------------------------------------------

class Life
{
public:
    // ctor and dtor
    Life(int width, int height);
    ~Life();
    void Create(int width, int height);
    void Destroy();

    // game field
    inline int       GetWidth() const        { return m_width; };
    inline int       GetHeight() const       { return m_height; };
    inline void      SetBorderWrap(bool on)  { m_wrap = on; };

    // cells
    inline bool      IsAlive(int i, int j) const;
    inline bool      IsAlive(Cell c) const;
    inline int       GetX(Cell c) const;
    inline int       GetY(Cell c) const;
    const CellArray* GetCells() const        { return &m_cells; };
    const CellArray* GetChangedCells() const { return &m_changed; };

    // game logic
    void Clear();
    Cell SetCell(int i, int j, bool alive = TRUE);
    void SetShape(LifeShape &shape);
    bool NextTic();

private:
    int         GetNeighbors(int i, int j) const;
    inline Cell MakeCell(int i, int j, bool alive) const;

    enum CellFlags
    {
        CELL_DEAD    = 0x0000,  // is dead
        CELL_ALIVE   = 0x0001,  // is alive
        CELL_MARK    = 0x0002,  // will change / has changed
    };

    int       m_width;
    int       m_height;
    CellArray m_cells;
    CellArray m_changed;
    bool      m_wrap;
};

#endif  // _LIFE_GAME_H_

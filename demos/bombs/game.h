///////////////////////////////////////////////////////////////////////////////
// Name:        game.h
// Purpose:     Bombs game
// Author:      P. Foggia 1996
// Modified by: Wlodzimierz Skiba (ABX) since 2003
// Created:     1996
// Copyright:   (c) 1996 P. Foggia
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DEMOS_BOMBS_GAME_H_
#define _WX_DEMOS_BOMBS_GAME_H_

#define BG_HIDDEN   0x100
#define BG_BOMB     0x200
#define BG_MARKED   0x400
#define BG_EXPLODED 0x800
#define BG_SELECTED 0x080
#define BG_MASK     0x03F

#include <stddef.h>

class BombsGame
{
public:
    BombsGame()
    {
        m_width = m_height = 0;
        m_field = NULL;
    }

    ~BombsGame();

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    int Get(int x, int y) const
    {
        return m_field[x+y*m_width];
    }

    int IsFocussed(int x, int y) const
    {
        return (m_gridFocusX == x) && (m_gridFocusY == y);
    }

    int IsHidden(int x, int y) const
    {
        return Get(x,y) & BG_HIDDEN;
    }

    int IsMarked(int x, int y) const
    {
        return Get(x,y) & BG_MARKED;
    }

    int IsBomb(int x, int y) const
    {
        return Get(x,y) & BG_BOMB;
    }

    int IsExploded(int x, int y) const
    {
        return Get(x,y) & BG_EXPLODED;
    }

    int IsSelected(int x, int y) const
    {
        return Get(x,y) & BG_SELECTED;
    }

    int GetNumBombs() const
    {
        return m_numBombCells;
    }

    int GetNumRemainingCells() const
    {
        return m_numRemainingCells;
    }

    int GetNumMarkedCells() const
    {
        return m_numMarkedCells;
    }


    bool Init(int width, int height, bool easyCorner = false);


    // Marks/unmarks a cell
    void Mark(int x, int y);

    // Unhides a cell
    void Unhide(int x, int y, bool b_selected);

    // Makes a cell exploded
    void Explode(int x, int y);

    int m_gridFocusX;
    int m_gridFocusY;

private:

    // Current difficulty level (Determines grid size).
    //int m_level;

    int m_width, m_height;
    short *m_field;
    int m_numBombCells, m_numRemainingCells, m_numMarkedCells;

};

#endif // #ifndef _WX_DEMOS_BOMBS_GAME_H_

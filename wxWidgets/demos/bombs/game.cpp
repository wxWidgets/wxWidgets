///////////////////////////////////////////////////////////////////////////////
// Name:        bombs1.cpp
// Purpose:     Implementation of the class BombsGame
// Author:      P. Foggia 1996
// Modified by: Wlodzimierz Skiba (ABX) since 2003
// Created:     1996
// RCS-ID:      $Id$
// Copyright:   (c) 1996 P. Foggia
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef  WX_PRECOMP
#   include "wx/wx.h"
#endif

#include "game.h"
#include <stdlib.h>
#include <limits.h>

#define PROB 0.2

#ifndef RAND_MAX
#   define RAND_MAX INT_MAX
#endif


BombsGame::~BombsGame()
{
    if (m_field)
    {
        delete[] m_field;
    }
}

// Initialize the play field. Returns false on failure
bool BombsGame::Init(int aWidth, int aHeight, bool easyCorner)
{
    m_gridFocusX = m_gridFocusY = -1;

    int x, y;
    int xx, yy;

    if (m_field)
    {
        delete[] m_field;
    }

    m_field = new short[aWidth*aHeight];
    if (!m_field)
    {
        m_width = m_height = 0;
        return false;
    }

    m_width = aWidth;
    m_height = aHeight;

    for(x=0; x<m_width; x++)
    {
        for(y=0; y<m_height; y++)
        {
            m_field[x+y*m_width] = ((float)rand()/RAND_MAX <PROB)
                ? BG_HIDDEN | BG_BOMB
                : BG_HIDDEN;
        }
    }

    /* Force (0,0) not to have a bomb for those that don't want to have
       to guess on the first move. Better would be to for the MS rule that
       whatever is picked first isn't a bomb.
     */
    if(easyCorner)
    {
        m_field[0] = BG_HIDDEN;
    }

    m_numBombCells = 0;
    for(x=0; x<m_width; x++)
        for(y=0; y<m_height; y++)
            if (m_field[x+y*m_width] & BG_BOMB)
            {
                m_numBombCells++;

                for(xx=x-1; xx<=x+1; xx++)
                    if (xx>=0 && xx<m_width)
                        for(yy=y-1; yy<=y+1; yy++)
                            if (yy>=0 && yy<m_height && (yy!=y || xx!=x))
                                m_field[xx+yy*m_width]++;
            }

    m_numRemainingCells = m_height*m_width-m_numBombCells;
    m_numMarkedCells = 0;

    return true;
}

void BombsGame::Mark(int x, int y)
{
    m_field[x+y*m_width] ^= BG_MARKED;
    if (IsMarked(x, y))
        m_numMarkedCells++;
    else
        m_numMarkedCells--;
}

void BombsGame::Unhide(int x, int y, bool b_selected)
{
    if (!IsHidden(x,y))
    {
        return;
    }

    if (b_selected)
        m_field[x+y*m_width] |= BG_SELECTED;

    m_field[x+y*m_width] &= ~BG_HIDDEN;

    if (!IsBomb(x,y))
    {
        m_numRemainingCells--;
    }
}


void BombsGame::Explode(int x, int y)
{
    m_field[x+y*m_width] |= BG_EXPLODED;
}

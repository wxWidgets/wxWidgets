///////////////////////////////////////////////////////////////////////////////
// Name:        bombs1.cpp
// Purpose:     Implementation of the class BombsGame
// Author:      P. Foggia 1996
// Modified by:
// Created:     1996
// RCS-ID:      $Id$
// Copyright:   (c) 1996 P. Foggia
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "game.h"
#include <stdlib.h>
#include <limits.h>

#define PROB 0.2

#ifndef RAND_MAX
#define RAND_MAX INT_MAX
#endif


/*--------------------  BombsGame::~BombsGame()  ---------------------*/
/*--------------------------------------------------------------------*/
BombsGame::~BombsGame()
  { if (field)
      free(field);
  }

/*------------------  int BombsGame::Init(width,height)  -------------------*/
/* Initialize the play field.   Returns 0 on failure                        */
/*--------------------------------------------------------------------------*/
int BombsGame::Init(int aWidth, int aHeight)
  { int x, y;
    int xx, yy;

    if (field)
      free(field);
    field=(short *)malloc(aWidth*aHeight*sizeof(short));
    if (!field)
      { width=height=0;
        return 0;
      }
    width=aWidth;
    height=aHeight;

    for(x=0; x<width; x++)
      for(y=0; y<height; y++)
        { field[x+y*width] = ((float)rand()/RAND_MAX <PROB)?
                             BG_HIDDEN | BG_BOMB :
                             BG_HIDDEN;
        }

    bombs=0;
    for(x=0; x<width; x++)
      for(y=0; y<height; y++)
        if (field[x+y*width] & BG_BOMB)
          { bombs++;
            for(xx=x-1; xx<=x+1; xx++)
              if (xx>=0 && xx<width)
                for(yy=y-1; yy<=y+1; yy++)
                  if (yy>=0 && yy<height && (yy!=y || xx!=x))
                    field[xx+yy*width]++;
          }
    normal_cells=height*width-bombs;
    return 1;
  }

/*----------------------  BombsGame::Mark(x,y)  -------------------------*/
/* Marks/unmarks a cell                                                  */
/*-----------------------------------------------------------------------*/
void BombsGame::Mark(int x, int y)
  {
      field[x+y*width] ^= BG_MARKED;
  }

/*-------------------  BombsGame::Unhide(x,y)  ------------------------*/
/* Unhides a cell                                                      */
/*---------------------------------------------------------------------*/
void BombsGame::Unhide(int x, int y)
  { if (!IsHidden(x,y))
      return;
    field[x+y*width] &= ~BG_HIDDEN;
    if (!IsBomb(x,y))
      normal_cells--;
  }

/*-------------------  BombsGame::Explode(x,y)  ------------------------*/
/* Makes a cell exploded                                                */
/*----------------------------------------------------------------------*/
void BombsGame::Explode(int x, int y)
  {
    field[x+y*width] |= BG_EXPLODED;
  }


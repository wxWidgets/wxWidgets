//---------------------------------------------------------------
// game.h
// Definition of the class BombsGame, containing the data for a
// playfield
//---------------------------------------------------------------
#ifndef GAME_H
#define GAME_H

#define BG_HIDDEN   0x100
#define BG_BOMB     0x200
#define BG_MARKED   0x400
#define BG_EXPLODED 0x800
#define BG_MASK     0x0FF


#include <stddef.h>

class BombsGame
  { protected:
      int width,height;
      short *field;
      int bombs,normal_cells;
    public:
      BombsGame() { width=height=0; field=NULL; };
      ~BombsGame();
      int Init(int width, int height);
      int GetWidth() { return width; };
      int GetHeight() { return height; };
      int Get(int x, int y) { return field[x+y*width]; };
      void Mark(int x, int y);
      void Unhide(int x, int y);
      void Explode(int x, int y);
      int IsHidden(int x, int y) { return Get(x,y) & BG_HIDDEN; };
      int IsMarked(int x, int y) { return Get(x,y) & BG_MARKED; };
      int IsBomb(int x, int y) { return Get(x,y) & BG_BOMB; };
      int IsExploded(int x, int y) { return Get(x,y) & BG_EXPLODED; };
      int GetBombs() { return bombs; };
      int GetRemainingCells() { return normal_cells; };
  };

#endif /* def GAME_H */


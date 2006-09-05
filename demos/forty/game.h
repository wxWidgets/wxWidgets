/////////////////////////////////////////////////////////////////////////////
// Name:        game.h
// Purpose:     Forty Thieves patience game
// Author:      Chris Breeze
// Modified by:
// Created:     21/07/97
// RCS-ID:      $Id$
// Copyright:   (c) 1993-1998 Chris Breeze
// Licence:     wxWindows licence
//---------------------------------------------------------------------------
// Last modified: 22nd July 1998 - ported to wxWidgets 2.0
/////////////////////////////////////////////////////////////////////////////
#ifndef _GAME_H_
#define _GAME_H_
#include "card.h"
#include "pile.h"

const int MaxMoves = 800;


//---------------------------------------//
// A class which holds the pack of cards //
//---------------------------------------//
class Pack : public Pile {
public:
    Pack(int x, int y);
    virtual ~Pack();
    void Redraw(wxDC& dc);
    void ResetPile() { m_topCard = NumCards - 1; }
    void Shuffle();
    void AddCard(Card* card); // Add card
    void AddCard(wxDC& dc, Card* card) { AddCard(card); Redraw(dc); }
};


//----------------------------------------------------------//
// A class which holds a base i.e. the initial 10 x 4 cards //
//----------------------------------------------------------//
class Base : public Pile {
public:
    Base(int x, int y);
    virtual ~Base(){}
    bool AcceptCard(Card* card);
};


//----------------------------------------------------//
// A class which holds a foundation i.e. Ace, 2, 3... //
//----------------------------------------------------//
class Foundation : public Pile {
public:
    Foundation(int x, int y);
    virtual ~Foundation(){}
    bool AcceptCard(Card* card);
};


//--------------------------------------//
// A class which holds the discard pile //
//--------------------------------------//
class Discard : public Pile {
public:
    Discard(int x, int y);
    virtual ~Discard(){}
    void Redraw(wxDC& dc);
    void GetTopCardPos(int& x, int& y);
    Card* RemoveTopCard(wxDC& dc, int m_xOffset, int m_yOffset);
};


class Game {
public:
    Game(int wins, int games, int score);
    virtual ~Game();

    void Layout();
    void NewPlayer(int wins, int games, int score);
    void Deal(); // Shuffle and deal a new game
    bool CanYouGo(int x, int y); // can card under (x,y) go somewhere?
    bool HaveYouWon(); // have you won the game?

    void Undo(wxDC& dc); // Undo the last go
    void Redo(wxDC& dc); // Redo the last go

    void Redraw(wxDC& dc);
    void DisplayScore(wxDC& dc);
    bool LButtonDown(wxDC& dc, int mx, int my);
    void LButtonUp(wxDC& dc, int mx, int my);
    void LButtonDblClk(wxDC& dc, int mx, int my);
    void MouseMove(wxDC& dc, int mx, int my);

    int GetNumWins() const { return m_numWins; }
    int GetNumGames() const { return m_numGames; }
    int GetScore() const { return m_currentScore + m_totalScore; }

    bool InPlay() const { return m_inPlay; }

private:
    bool DropCard(int x, int y, Pile* pile, Card* card);
            //  can the card at (x, y) be dropped on the pile?
    Pile* WhichPile(int x, int y); // which pile is (x, y) over?
    void DoMove(wxDC& dc, Pile* src, Pile* dest);

    bool m_inPlay; // flag indicating that the game has started

    // undo buffer
    struct {
        Pile* src;
        Pile* dest;
    } m_moves[MaxMoves];
    int m_moveIndex; // current position in undo/redo buffer
    int m_redoIndex; // max move index available for redo

    // the various piles of cards
    Pack* m_pack;
    Discard* m_discard;
    Base* m_bases[10];
    Foundation* m_foundations[8];

    // variables to do with dragging cards
    Pile* m_srcPile;
    Card* m_liftedCard;
    int m_xPos, m_yPos; // current coords of card being dragged
    int m_xOffset, m_yOffset; // card/mouse offset when dragging a card

    wxBitmap* m_bmap;
    wxBitmap* m_bmapCard;

    // variables to do with scoring
    int m_numGames;
    int m_numWins;
    int m_totalScore;
    int m_currentScore;
};

#endif // _GAME_H_

/////////////////////////////////////////////////////////////////////////////
// Name:        card.h
// Purpose:     Forty Thieves patience game
// Author:      Chris Breeze
// Modified by:
// Created:     21/07/97
// RCS-ID:      $Id$
// Copyright:   (c) 1993-1998 Chris Breeze
// Licence:   	wxWindows licence
//---------------------------------------------------------------------------
// Last modified: 22nd July 1998 - ported to wxWindows 2.0
/////////////////////////////////////////////////////////////////////////////
//+-------------------------------------------------------------+
//| Description:												|
//|	A class for drawing playing cards.							|
//|	InitCards() must be called before using the Card class,		|
//|	otherwise the card bitmaps will not be loaded.				|
//|	CloseCards() must be called before terminating the			|
//|	program so that the bitmaps are deleted and the memory		|
//|	given back to Windows.										|
//+-------------------------------------------------------------+
#ifndef _CARD_H_
#define _CARD_H_

	// Constants
const int PackSize = 52;
const int CardWidth = 50;
const int CardHeight = 70;

	// Data types
enum Suit { clubs = 0, diamonds = 1, hearts = 2, spades = 3 };
enum SuitColour { red = 0, black = 1 };
enum WayUp { faceup, facedown };


//--------------------------------//
// A class defining a single card //
//--------------------------------//
class Card {
public:
	Card(int value, WayUp way_up = facedown);
	virtual ~Card();

	void		Draw(wxDC& pDC, int x, int y);
	static void DrawNullCard(wxDC& pDC, int x, int y);	// Draw card place-holder
	void		Erase(wxDC& pDC, int x, int y);

	void		TurnCard(WayUp way_up = faceup) { m_wayUp = way_up; }
	WayUp		GetWayUp() const { return m_wayUp; }
	int			GetPipValue() const { return m_pipValue; }
	Suit		GetSuit() const { return m_suit; }
	SuitColour	GetColour() const { return m_colour; }

private:
	Suit		m_suit;
	int			m_pipValue;	// in the range 1 (Ace) to 13 (King)
	SuitColour	m_colour;	// red or black
	bool		m_status;
	WayUp		m_wayUp;

	static wxBitmap*	m_symbolBmap;
	static wxBitmap*	m_pictureBmap;
};

#endif // _CARD_H_

/////////////////////////////////////////////////////////////////////////////
// Name:        game.cpp
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

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "forty.h"
#include "game.h"

Game::Game(int wins, int games, int score) :
	m_inPlay(FALSE),
	m_moveIndex(0),
	m_redoIndex(0),
	m_bmap(0),
	m_bmapCard(0)
{
    int i;

    m_pack = new Pack(2, 2 + 4 * (CardHeight + 2));
    srand(time(0));

    for (i = 0; i < 5; i++) m_pack->Shuffle();

    m_discard = new Discard(2, 2 + 5 * (CardHeight + 2));

    for (i = 0; i < 8; i++)
    {
		m_foundations[i] = new Foundation(2 + (i / 4) * (CardWidth + 2),
					2 + (i % 4) * (CardHeight + 2));
    }

    for (i = 0; i < 10; i++)
    {
    	m_bases[i] = new Base(8 + (i + 2) * (CardWidth + 2), 2);
    }
    Deal();
    m_srcPile = 0;
    m_liftedCard = 0;

	// copy the input parameters for future reference
    m_numWins = wins;
    m_numGames = games;
    m_totalScore = score;
    m_currentScore = 0;
}


// Make sure we delete all objects created by the game object
Game::~Game()
{
    int i;

    delete m_pack;
    delete m_discard;
    for (i = 0; i < 8; i++)
    {
		delete m_foundations[i];
    }
    for (i = 0; i < 10; i++)
    {
		delete m_bases[i];
    }
	delete m_bmap;
	delete m_bmapCard;
}

/*
Set the score for a new player.
NB: call Deal() first if the new player is to start
a new game
*/
void Game::NewPlayer(int wins, int games, int score)
{
    m_numWins = wins;
    m_numGames = games;
    m_totalScore = score;
    m_currentScore = 0;
}

// Undo the last move
void Game::Undo(wxDC& dc)
{
    if (m_moveIndex > 0)
    {
		m_moveIndex--;
		Card* card = m_moves[m_moveIndex].dest->RemoveTopCard(dc);
		m_moves[m_moveIndex].src->AddCard(dc, card);
		DisplayScore(dc);
    }
}

// Redo the last move
void Game::Redo(wxDC& dc)
{
    if (m_moveIndex < m_redoIndex)
    {
		Card* card = m_moves[m_moveIndex].src->RemoveTopCard(dc);
		if (m_moves[m_moveIndex].src == m_pack)
		{
			m_pack->Redraw(dc);
			card->TurnCard(faceup);
		}
		m_moves[m_moveIndex].dest->AddCard(dc, card);
		DisplayScore(dc);
		m_moveIndex++;
    }
}

void Game::DoMove(wxDC& dc, Pile* src, Pile* dest)
{
    if (m_moveIndex < MaxMoves)
    {
		if (src == dest)
		{
			wxMessageBox("Game::DoMove() src == dest", "Debug message",
				   wxOK | wxICON_EXCLAMATION);
		}
		m_moves[m_moveIndex].src = src;
		m_moves[m_moveIndex].dest = dest;
		m_moveIndex++;

		// when we do a move any moves in redo buffer are discarded
		m_redoIndex = m_moveIndex;
    }
    else
    {
		wxMessageBox("Game::DoMove() Undo buffer full", "Debug message",
			   wxOK | wxICON_EXCLAMATION);
    }

    if (!m_inPlay)
	{
		m_inPlay = TRUE;
		m_numGames++;
	}
    DisplayScore(dc);
}


void Game::DisplayScore(wxDC& dc)
{
    wxColour* bgColour = FortyApp::BackgroundColour();
	wxPen* pen = wxThePenList->FindOrCreatePen(*bgColour, 1, wxSOLID);
    dc.SetTextBackground(*bgColour);
    dc.SetTextForeground(*FortyApp::TextColour());
	dc.SetBrush(FortyApp::BackgroundBrush());
	dc.SetPen(pen);

	// count the number of cards in foundations
    m_currentScore = 0;
    for (int i = 0; i < 8; i++)
    {
    	m_currentScore += m_foundations[i]->GetNumCards();
    }

    int x, y;
    m_pack->GetTopCardPos(x, y);
    x += 12 * CardWidth - 105;

	int w, h;
	{
		long width, height;
		dc.GetTextExtent("Average score:m_x", &width, &height);
		w = width;
		h = height;
	}
	dc.DrawRectangle(x + w, y, 20, 4 * h);

    char str[80];
    sprintf(str, "%d", m_currentScore);
    dc.DrawText("Score:", x, y);
    dc.DrawText(str, x + w, y);
    y += h;

    sprintf(str, "%d", m_numGames);
    dc.DrawText("Games played:", x, y);
    dc.DrawText(str, x + w, y);
    y += h;

    sprintf(str, "%d", m_numWins);
    dc.DrawText("Games won:", x, y);
    dc.DrawText(str, x + w, y);
    y += h;

    int average = 0;
	if (m_numGames > 0)
	{
		average = (2 * (m_currentScore + m_totalScore) + m_numGames ) / (2 * m_numGames);
	}
    sprintf(str, "%d", average);
    dc.DrawText("Average score:", x, y);
    dc.DrawText(str, x + w, y);
}


// Shuffle the m_pack and deal the cards
void Game::Deal()
{
    int i, j;
    Card* card;

	// Reset all the piles, the undo buffer and shuffle the m_pack
    m_moveIndex = 0;
    m_pack->ResetPile();
    for (i = 0; i < 5; i++)
	{
		m_pack->Shuffle();
	}
    m_discard->ResetPile();
    for (i = 0; i < 10; i++)
	{
		m_bases[i]->ResetPile();
	}
    for (i = 0; i <  8; i++)
	{
		m_foundations[i]->ResetPile();
	}

    	// Deal the initial 40 cards onto the bases
    for (i = 0; i < 10; i++)
    {
		for (j = 1; j <= 4; j++)
		{
			card = m_pack->RemoveTopCard();
			card->TurnCard(faceup);
			m_bases[i]->AddCard(card);
		}
    }

    if (m_inPlay)
	{
		// player has started the game and then redealt
		// and so we must add the score for this game to the total score
		m_totalScore += m_currentScore;
	}
    m_currentScore = 0;
    m_inPlay = FALSE;
}


// Redraw the m_pack, discard pile, the bases and the foundations
void Game::Redraw(wxDC& dc)
{
	int i;
	m_pack->Redraw(dc);
	m_discard->Redraw(dc);
	for (i = 0; i < 8; i++)
	{
		m_foundations[i]->Redraw(dc);
	}
	for (i = 0; i < 10; i++)
	{
		m_bases[i]->Redraw(dc);
	}
	DisplayScore(dc);

	if (m_bmap == 0)
	{
		m_bmap = new wxBitmap(CardWidth, CardHeight);
		m_bmapCard = new wxBitmap(CardWidth, CardHeight);

		// Initialise the card bitmap to the background colour
		wxMemoryDC memoryDC;
		memoryDC.SelectObject(*m_bmapCard);
		memoryDC.SetBrush(FortyApp::BackgroundBrush());
		memoryDC.DrawRectangle(0, 0, CardWidth, CardHeight);
		memoryDC.SelectObject(*m_bmap);
		memoryDC.DrawRectangle(0, 0, CardWidth, CardHeight);
		memoryDC.SelectObject(wxNullBitmap);
	}
}


// Test to see if the point (x, y) is over the top card of one of the piles
// Returns pointer to the pile, or 0 if (x, y) is not over a pile
// or the pile is empty
Pile* Game::WhichPile(int x, int y)
{
	if (m_pack->GetCard(x, y) &&
		m_pack->GetCard(x, y) == m_pack->GetTopCard())
	{
		return m_pack;
	}

	if (m_discard->GetCard(x, y) &&
		m_discard->GetCard(x, y) == m_discard->GetTopCard())
	{
		return m_discard;
	}

	int i;
	for (i = 0; i < 8; i++)
	{
		if (m_foundations[i]->GetCard(x, y) &&
			m_foundations[i]->GetCard(x, y) == m_foundations[i]->GetTopCard())
		{
			return m_foundations[i];
		}
	}

	for (i = 0; i < 10; i++)
	{
		if (m_bases[i]->GetCard(x, y) &&
			m_bases[i]->GetCard(x, y) == m_bases[i]->GetTopCard())
		{
			return m_bases[i];
		}
	}
	return 0;
}


// Left button is pressed - if cursor is over the m_pack then deal a card
// otherwise if it is over a card pick it up ready to be dragged - see MouseMove()
bool Game::LButtonDown(wxDC& dc, int x, int y)
{
    m_srcPile = WhichPile(x, y);
    if (m_srcPile == m_pack)
    {
		Card* card = m_pack->RemoveTopCard();
		if (card)
		{
			m_pack->Redraw(dc);
			card->TurnCard(faceup);
			m_discard->AddCard(dc, card);
			DoMove(dc, m_pack, m_discard);
		}
        m_srcPile = 0;
    }
    else if (m_srcPile)
    {
		m_srcPile->GetTopCardPos(m_xPos, m_yPos);
		m_xOffset = m_xPos - x;
		m_yOffset = m_yPos - y;

			// Copy the area under the card
			// Initialise the card bitmap to the background colour
		{
			wxMemoryDC memoryDC;
			memoryDC.SelectObject(m_bmap);
			m_liftedCard = m_srcPile->RemoveTopCard(memoryDC, m_xPos, m_yPos);
		}

			// Draw the card in card bitmap ready for blitting onto
			// the screen
		{
			wxMemoryDC memoryDC;
			memoryDC.SelectObject(m_bmapCard);
			m_liftedCard->Draw(memoryDC, 0, 0);
		}
    }
    return m_srcPile != 0;
}

// Called when the left button is double clicked
// If a card is under the pointer and it can move elsewhere then move it.
// Move onto a foundation as first choice, a populated base as second and
// an empty base as third choice.
// NB Cards in the m_pack cannot be moved in this way - they aren't in play
// yet
void Game::LButtonDblClk(wxDC& dc, int x, int y)
{
    Pile* pile = WhichPile(x, y);
    if (!pile) return;

    	// Double click on m_pack is the same as left button down
    if (pile == m_pack)
    {
		LButtonDown(dc, x, y);
    }
    else
    {
		Card* card = pile->GetTopCard();

		if (card)
		{
			int i;

			// if the card is an ace then try to place it next
			// to an ace of the same suit
			if (card->GetPipValue() == 1)
			{
				for(i = 0; i < 4; i++)
				{
					Card* m_topCard;
					if ((m_topCard = m_foundations[i]->GetTopCard()))
                    {
						if (m_topCard->GetSuit() == card->GetSuit() &&
                            m_foundations[i + 4] != pile &&
							m_foundations[i + 4]->GetTopCard() == 0)
						{
							pile->RemoveTopCard(dc);
							m_foundations[i + 4]->AddCard(dc, card);
							DoMove(dc, pile, m_foundations[i + 4]);
							return;
						}
                    }
				}
			}

			// try to place the card on a foundation
			for(i = 0; i < 8; i++)
			{
				if (m_foundations[i]->AcceptCard(card) && m_foundations[i] != pile)
				{
					pile->RemoveTopCard(dc);
					m_foundations[i]->AddCard(dc, card);
					DoMove(dc, pile, m_foundations[i]);
					return;
				}
            }
			// try to place the card on a populated base
			for(i = 0; i < 10; i++)
			{
				if (m_bases[i]->AcceptCard(card) &&
					m_bases[i] != pile &&
					m_bases[i]->GetTopCard())
				{
					pile->RemoveTopCard(dc);
					m_bases[i]->AddCard(dc, card);
					DoMove(dc, pile, m_bases[i]);
					return;
				}
            }
			// try to place the card on any base
			for(i = 0; i < 10; i++)
			{
				if (m_bases[i]->AcceptCard(card) && m_bases[i] != pile)
				{
					pile->RemoveTopCard(dc);
					m_bases[i]->AddCard(dc, card);
					DoMove(dc, pile, m_bases[i]);
					return;
				}
            }
		}
    }
}


// Test to see whether the game has been won:
// i.e. m_pack, discard and bases are empty
bool Game::HaveYouWon()
{
    if (m_pack->GetTopCard()) return FALSE;
    if (m_discard->GetTopCard()) return FALSE;
    for(int i = 0; i < 10; i++)
    {
    	if (m_bases[i]->GetTopCard()) return FALSE;
    }
    m_numWins++;
    m_totalScore += m_currentScore;
    m_currentScore = 0;
    return TRUE;
}


// See whether the card under the cursor can be moved somewhere else
// Returns TRUE if it can be moved, FALSE otherwise
bool Game::CanYouGo(int x, int y)
{
    Pile* pile = WhichPile(x, y);
    if (pile && pile != m_pack)
    {
	Card* card = pile->GetTopCard();

	if (card)
	{
	    int i;
	    for(i = 0; i < 8; i++)
	    {
	    	if (m_foundations[i]->AcceptCard(card) && m_foundations[i] != pile)
		{
                    return TRUE;
		}
            }
	    for(i = 0; i < 10; i++)
	    {
		if (m_bases[i]->GetTopCard() &&
		    m_bases[i]->AcceptCard(card) &&
		    m_bases[i] != pile)
		{
		    return TRUE;
		}
            }
	}
    }
    return FALSE;
}


// Called when the left button is released after dragging a card
// Scan the piles to see if this card overlaps a pile and can be added
// to the pile. If the card overlaps more than one pile on which it can be placed
// then put it on the nearest pile.
void Game::LButtonUp(wxDC& dc, int x, int y)
{
    if (m_srcPile)
    {
        	// work out the position of the dragged card
		x += m_xOffset;
        y += m_yOffset;

		Pile* nearestPile = 0;
		int distance = (CardHeight + CardWidth) * (CardHeight + CardWidth);

        	// find the nearest pile which will accept the card
		int i;
		for (i = 0; i < 8; i++)
		{
			if (DropCard(x, y, m_foundations[i], m_liftedCard))
			{
				if (m_foundations[i]->CalcDistance(x, y) < distance)
				{
					nearestPile = m_foundations[i];
                    distance = nearestPile->CalcDistance(x, y);
                }
            }
		}
		for (i = 0; i < 10; i++)
		{
			if (DropCard(x, y, m_bases[i], m_liftedCard))
			{
				if (m_bases[i]->CalcDistance(x, y) < distance)
                {
					nearestPile = m_bases[i];
                    distance = nearestPile->CalcDistance(x, y);
                }
            }
		}

		// Restore the area under the card
		wxMemoryDC memoryDC;
		memoryDC.SelectObject(m_bmap);
		dc.Blit(m_xPos, m_yPos, CardWidth, CardHeight,
			   &memoryDC, 0, 0, wxCOPY);

        	// Draw the card in its new position
		if (nearestPile)
		{
			// Add to new pile
			nearestPile->AddCard(dc, m_liftedCard);
			if (nearestPile != m_srcPile)
			{
				DoMove(dc, m_srcPile, nearestPile);
			}
		}
        else
        {
			// Return card to src pile
			m_srcPile->AddCard(dc, m_liftedCard);
		}
		m_srcPile = 0;
		m_liftedCard = 0;
    }
}




bool Game::DropCard(int x, int y, Pile* pile, Card* card)
{
    bool retval = FALSE;
    if (pile->Overlap(x, y))
    {
	if (pile->AcceptCard(card))
	{
	    retval = TRUE;
        }
    }
    return retval;
}


void Game::MouseMove(wxDC& dc, int mx, int my)
{
    if (m_liftedCard)
    {
		wxMemoryDC memoryDC;
		memoryDC.SelectObject(m_bmap);

		int dx = mx + m_xOffset - m_xPos;
		int dy = my + m_yOffset - m_yPos;

		if (abs(dx) >= CardWidth || abs(dy) >= CardHeight)
        {
			// Restore the area under the card
			dc.Blit(m_xPos, m_yPos, CardWidth, CardHeight,
			   &memoryDC, 0, 0, wxCOPY);

			// Copy the area under the card in the new position
			memoryDC.Blit(0, 0, CardWidth, CardHeight,
			   &dc, m_xPos + dx, m_yPos + dy, wxCOPY);
		}
		else if (dx >= 0)
		{
			// dx >= 0
			dc.Blit(m_xPos, m_yPos, dx, CardHeight, &memoryDC, 0, 0, wxCOPY);
			if (dy >= 0)
			{
					// dy >= 0
				dc.Blit(m_xPos + dx, m_yPos, CardWidth - dx, dy, &memoryDC, dx, 0, wxCOPY);
				memoryDC.Blit(0, 0, CardWidth - dx, CardHeight - dy,
					   &memoryDC, dx, dy, wxCOPY);
				memoryDC.Blit(0, CardHeight - dy, CardWidth - dx, dy,
					   &dc, m_xPos + dx, m_yPos + CardHeight, wxCOPY);
			}
			else
			{
                		// dy < 0
				dc.Blit(m_xPos + dx, m_yPos + dy + CardHeight, CardWidth - dx, -dy,
					   &memoryDC, dx, CardHeight + dy, wxCOPY);
				memoryDC.Blit(0, -dy, CardWidth - dx, CardHeight + dy,
					   &memoryDC, dx, 0, wxCOPY);
				memoryDC.Blit(0, 0, CardWidth - dx, -dy,
					   &dc, m_xPos + dx, m_yPos + dy, wxCOPY);
			}
			memoryDC.Blit(CardWidth - dx, 0, dx, CardHeight,
	    		   &dc, m_xPos + CardWidth, m_yPos + dy, wxCOPY);
		}
		else
		{
			// dx < 0
			dc.Blit(m_xPos + CardWidth + dx, m_yPos, -dx, CardHeight,
	    		   &memoryDC, CardWidth + dx, 0, wxCOPY);
			if (dy >= 0)
			{
				dc.Blit(m_xPos, m_yPos, CardWidth + dx, dy, &memoryDC, 0, 0, wxCOPY);
				memoryDC.Blit(-dx, 0, CardWidth + dx, CardHeight - dy,
					   &memoryDC, 0, dy, wxCOPY);
				memoryDC.Blit(-dx, CardHeight - dy, CardWidth + dx, dy,
					   &dc, m_xPos, m_yPos + CardHeight, wxCOPY);
			}
			else
			{
                			// dy < 0
				dc.Blit(m_xPos, m_yPos + CardHeight + dy, CardWidth + dx, -dy,
					   &memoryDC, 0, CardHeight + dy, wxCOPY);
				memoryDC.Blit(-dx, -dy, CardWidth + dx, CardHeight + dy,
					   &memoryDC, 0, 0, wxCOPY);
				memoryDC.Blit(-dx, 0, CardWidth + dx, -dy,
					   &dc, m_xPos, m_yPos + dy, wxCOPY);
			}
			memoryDC.Blit(0, 0, -dx, CardHeight,
	    		   &dc, m_xPos + dx, m_yPos + dy, wxCOPY);
		}
		m_xPos += dx;
		m_yPos += dy;

			// draw the card in its new position
		memoryDC.SelectObject(m_bmapCard);
		dc.Blit(m_xPos, m_yPos, CardWidth, CardHeight,
			   &memoryDC, 0, 0, wxCOPY);
    }
}



//----------------------------------------------//
// The Pack class: holds the two decks of cards //
//----------------------------------------------//
Pack::Pack(int x, int y) : Pile(x, y, 0, 0)
{
    for (m_topCard = 0; m_topCard < NumCards; m_topCard++)
    {
    	m_cards[m_topCard] = new Card(1 + m_topCard / 2, facedown);
    }
    m_topCard = NumCards - 1;
}


void Pack::Shuffle()
{
    Card* temp[NumCards];
    int i;

	// Don't try to shuffle an empty m_pack!
    if (m_topCard < 0) return;

	// Copy the cards into a temporary array. Start by clearing
	// the array and then copy the card into a random position.
        // If the position is occupied then find the next lower position.
    for (i = 0; i <= m_topCard; i++)
    {
		temp[i] = 0;
    }
    for (i = 0; i <= m_topCard; i++)
    {
	int pos = rand() % (m_topCard + 1);
	while (temp[pos])
	{
	    pos--;
	    if (pos < 0) pos = m_topCard;
	}
	m_cards[i]->TurnCard(facedown);
	temp[pos] = m_cards[i];
        m_cards[i] = 0;
    }

	// Copy each card back into the m_pack in a random
	// position. If position is occupied then find nearest
        // unoccupied position after the random position.
    for (i = 0; i <= m_topCard; i++)
    {
	int pos = rand() % (m_topCard + 1);
	while (m_cards[pos])
	{
	    pos++;
            if (pos > m_topCard) pos = 0;
	}
        m_cards[pos] = temp[i];
    }
}

void Pack::Redraw(wxDC& dc)
{
    Pile::Redraw(dc);

    char str[10];
    sprintf(str, "%d  ", m_topCard + 1);

	dc.SetTextBackground(*FortyApp::BackgroundColour());
	dc.SetTextForeground(*FortyApp::TextColour());
    dc.DrawText(str, m_x + CardWidth + 5, m_y + CardHeight / 2);

}

void Pack::AddCard(Card* card)
{
    if (card == m_cards[m_topCard + 1])
    {
		m_topCard++;
    }
    else
    {
		wxMessageBox("Pack::AddCard() Undo error", "Forty Thieves: Warning",
		   wxOK | wxICON_EXCLAMATION);
    }
    card->TurnCard(facedown);
}


Pack::~Pack()
{
    for (m_topCard = 0; m_topCard < NumCards; m_topCard++)
    {
	delete m_cards[m_topCard];
    }
};


//------------------------------------------------------//
// The Base class: holds the initial pile of four cards //
//------------------------------------------------------//
Base::Base(int x, int y) : Pile(x, y, 0, 12)
{
    m_topCard = -1;
}


bool Base::AcceptCard(Card* card)
{
    bool retval = FALSE;

    if (m_topCard >= 0)
    {
	if (m_cards[m_topCard]->GetSuit() == card->GetSuit() &&
	    m_cards[m_topCard]->GetPipValue() - 1 == card->GetPipValue())
	{
            retval = TRUE;
        }
    }
    else
    {
		// pile is empty - ACCEPT
        retval = TRUE;
    }
    return retval;
}

Base::~Base()
{
// nothing special at the moment
};


//----------------------------------------------------------------//
// The Foundation class: holds the cards built up from the ace... //
//----------------------------------------------------------------//
Foundation::Foundation(int x, int y) : Pile(x, y, 0, 0)
{
    m_topCard = -1;
}

bool Foundation::AcceptCard(Card* card)
{
    bool retval = FALSE;

    if (m_topCard >= 0)
    {
	if (m_cards[m_topCard]->GetSuit() == card->GetSuit() &&
	    m_cards[m_topCard]->GetPipValue() + 1 == card->GetPipValue())
	{
            retval = TRUE;
        }
    }
    else if (card->GetPipValue() == 1)
    {
	// It's an ace and the pile is empty - ACCEPT
        retval = TRUE;
    }
    return retval;
}

Foundation::~Foundation()
{
// nothing special at the moment
};


//----------------------------------------------------//
// The Discard class: holds cards dealt from the m_pack //
//----------------------------------------------------//
Discard::Discard(int x, int y) : Pile(x, y, 19, 0)
{
    m_topCard = -1;
}

void Discard::Redraw(wxDC& dc)
{
    if (m_topCard >= 0)
    {
	if (m_dx == 0 && m_dy == 0)
	{
            m_cards[m_topCard]->Draw(dc, m_x, m_y);
	}
	else
	{
	    int x = m_x;
	    int y = m_y;
	    for (int i = 0; i <= m_topCard; i++)
	    {
		m_cards[i]->Draw(dc, x, y);
		x += m_dx;
		y += m_dy;
		if (i == 31)
		{
		    x = m_x;
                    y = m_y + CardHeight / 3;
                }
            }
        }
    }
    else
    {
		Card::DrawNullCard(dc, m_x, m_y);
    }
}


void Discard::GetTopCardPos(int& x, int& y)
{
    if (m_topCard < 0)
    {
	x = m_x;
	y = m_y;
    }
    else if (m_topCard > 31)
    {
	x = m_x + m_dx * (m_topCard - 32);
	y = m_y + CardHeight / 3;
    }
    else
    {
	x = m_x + m_dx * m_topCard;
	y = m_y;
    }
}


Card* Discard::RemoveTopCard(wxDC& dc, int m_xOffset, int m_yOffset)
{
    Card* card;

    if (m_topCard <= 31)
    {
		card = Pile::RemoveTopCard(dc, m_xOffset, m_yOffset);
    }
    else
    {
    	int topX, topY, x, y;
    	GetTopCardPos(topX, topY);
		card = Pile::RemoveTopCard();
		card->Erase(dc, topX - m_xOffset, topY - m_yOffset);
		GetTopCardPos(x, y);
		dc.SetClippingRegion(topX - m_xOffset, topY - m_yOffset,
					 CardWidth, CardHeight);

		for (int i = m_topCard - 31; i <= m_topCard - 31 + CardWidth / m_dx; i++)
		{
			m_cards[i]->Draw(dc, m_x - m_xOffset + i * m_dx, m_y - m_yOffset); 
		}
		if (m_topCard > 31)
		{
			m_cards[m_topCard]->Draw(dc, topX - m_xOffset - m_dx, topY - m_yOffset);
		}
		dc.DestroyClippingRegion();
    }

    return card;
}


Discard::~Discard()
{
// nothing special at the moment
};

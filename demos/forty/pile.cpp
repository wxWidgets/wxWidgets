/////////////////////////////////////////////////////////////////////////////
// Name:        pile.cpp
// Purpose:     Forty Thieves patience game
// Author:      Chris Breeze
// Modified by:
// Created:     21/07/97
// Copyright:   (c) 1993-1998 Chris Breeze
// Licence:     wxWindows licence
//---------------------------------------------------------------------------
// Last modified: 22nd July 1998 - ported to wxWidgets 2.0
/////////////////////////////////////////////////////////////////////////////
//+-------------------------------------------------------------+
//| Description:                                                |
//| The base class for holding piles of playing cards.          |
//+-------------------------------------------------------------+

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "card.h"
#include "pile.h"
#include "forty.h"
#include "canvas.h"

#include "wx/app.h"

//+-------------------------------------------------------------+
//| Pile::Pile()                                                |
//+-------------------------------------------------------------+
//| Description:                                                |
//| Initialise the pile to be empty of cards.                   |
//+-------------------------------------------------------------+
Pile::Pile(int x, int y, int dx, int dy)
{
    m_x = x;
    m_y = y;
    m_dx = dx;
    m_dy = dy;
    for (m_topCard = 0; m_topCard < NumCards; m_topCard++)
    {
        m_cards[m_topCard] = 0;
    }
    m_topCard = -1; // i.e. empty
}


//+-------------------------------------------------------------+
//| Pile::Redraw()                                              |
//+-------------------------------------------------------------+
//| Description:                                                |
//| Redraw the pile on the screen. If the pile is empty         |
//| just draw a null card as a place holder for the pile.       |
//| Otherwise draw the pile from the bottom up, starting        |
//| at the origin of the pile, shifting each subsequent         |
//| card by the pile's x and y offsets.                         |
//+-------------------------------------------------------------+
void Pile::Redraw(wxDC& dc )
{
    FortyFrame *frame = (FortyFrame*) wxTheApp->GetTopWindow();
    wxWindow *canvas = nullptr;
    if (frame)
    {
        canvas = frame->GetCanvas();
    }

    if (m_topCard >= 0)
    {
        if (m_dx == 0 && m_dy == 0)
        {
            if ((canvas) && (canvas->IsExposed(m_x,m_y,(int)(Card::GetScale()*60),(int)(Card::GetScale()*200))))
                m_cards[m_topCard]->Draw(dc, m_x, m_y);
        }
        else
        {
            int x = m_x;
            int y = m_y;
            for (int i = 0; i <= m_topCard; i++)
            {
                if ((canvas) && (canvas->IsExposed(x,y,(int)(Card::GetScale()*60),(int)(Card::GetScale()*200))))
                    m_cards[i]->Draw(dc, x, y);
                              x += (int)Card::GetScale()*m_dx;
                              y += (int)Card::GetScale()*m_dy;
            }
        }
    }
    else
    {
        if ((canvas) && (canvas->IsExposed(m_x,m_y,(int)(Card::GetScale()*60),(int)(Card::GetScale()*200))))
            Card::DrawNullCard(dc, m_x, m_y);
    }
}


//+-------------------------------------------------------------+
//| Pile::GetTopCard()                                          |
//+-------------------------------------------------------------+
//| Description:                                                |
//| Return a pointer to the top card in the pile or nullptr     |
//| if the pile is empty.                                       |
//| NB: Gets a copy of the card without removing it from the    |
//| pile.                                                       |
//+-------------------------------------------------------------+
Card* Pile::GetTopCard()
{
    Card* card = 0;

    if (m_topCard >= 0)
    {
        card = m_cards[m_topCard];
    }
    return card;
}


//+-------------------------------------------------------------+
//| Pile::RemoveTopCard()                                       |
//+-------------------------------------------------------------+
//| Description:                                                |
//| If the pile is not empty, remove the top card from the      |
//| pile and return the pointer to the removed card.            |
//| If the pile is empty return a null pointer.                 |
//+-------------------------------------------------------------+
Card* Pile::RemoveTopCard()
{
    Card* card = 0;

    if (m_topCard >= 0)
    {
        card = m_cards[m_topCard--];
    }
    return card;
}


//+-------------------------------------------------------------+
//| Pile::RemoveTopCard()                                       |
//+-------------------------------------------------------------+
//| Description:                                                |
//| As RemoveTopCard() but also redraw the top of the pile      |
//| after the card has been removed.                            |
//| NB: the offset allows for the redrawn area to be in a       |
//| bitmap ready for 'dragging' cards acrosss the screen.       |
//+-------------------------------------------------------------+
Card* Pile::RemoveTopCard(wxDC& dc, int xOffset, int yOffset)
{
    int topX, topY;

    GetTopCardPos(topX, topY);
    Card* card = RemoveTopCard();

    if (card)
    {
        int x, y;
        card->Erase(dc, topX - xOffset, topY - yOffset);
        GetTopCardPos(x, y);
        if (m_topCard < 0)
        {
            Card::DrawNullCard(dc, x - xOffset, y - yOffset);
        }
        else
        {
            m_cards[m_topCard]->Draw(dc, x - xOffset, y - yOffset);
        }
    }

    return card;
}


void Pile::GetTopCardPos(int& x, int& y)
{
    if (m_topCard < 0)
    {
        x = m_x;
        y = m_y;
    }
    else
    {
        x = m_x + (int)Card::GetScale()*m_dx * m_topCard;
        y = m_y + (int)Card::GetScale()*m_dy * m_topCard;
    }
}

void Pile::AddCard(Card* card)
{
    if (m_topCard < -1) m_topCard = -1;

    m_cards[++m_topCard] = card;
}

void Pile::AddCard(wxDC& dc, Card* card)
{
    AddCard(card);
    int x, y;
    GetTopCardPos(x, y);
    card->Draw(dc, x, y);
}

// Can the card leave this pile.
// If it is a member of the pile then the answer is yes.
// Derived classes may override this behaviour to incorporate
// the rules of the game
bool Pile::CanCardLeave(Card* card)
{
    for (int i = 0; i <= m_topCard; i++)
    {
        if (card == m_cards[i]) return true;
    }
    return false;
}

// Calculate how far x, y is from top card in the pile
// Returns the square of the distance
int Pile::CalcDistance(int x, int y)
{
    int cx, cy;
    GetTopCardPos(cx, cy);
    return ((cx - x) * (cx - x) + (cy - y) * (cy - y));
}


// Return the card at x, y. Check the top card first, then
// work down the pile. If a card is found then return a pointer
// to the card, otherwise return nullptr
Card* Pile::GetCard(int x, int y)
{
    int cardX;
    int cardY;
    GetTopCardPos(cardX, cardY);

    for (int i = m_topCard; i >= 0; i--)
    {
        if (x >= cardX && x <= cardX + Card::GetWidth() &&
            y >= cardY && y <= cardY + Card::GetHeight())
        {
            return m_cards[i];
        }
        cardX -= (int)Card::GetScale()*m_dx;
        cardY -= (int)Card::GetScale()*m_dy;
    }
    return 0;
}


// Return the position of the given card. If it is not a member of this pile
// return the origin of the pile.
void Pile::GetCardPos(Card* card, int& x, int& y)
{
    x = m_x;
    y = m_y;

    for (int i = 0; i <= m_topCard; i++)
    {
        if (card == m_cards[i])
        {
            return;
        }
        x += (int)Card::GetScale()*m_dx;
        y += (int)Card::GetScale()*m_dy;
    }

    // card not found in pile, return origin of pile
    x = m_x;
    y = m_y;
}


bool Pile::Overlap(int x, int y)
{
    int cardX;
    int cardY;
    GetTopCardPos(cardX, cardY);

    if (x >= cardX - Card::GetWidth()  && x <= cardX + Card::GetWidth() &&
        y >= cardY - Card::GetHeight() && y <= cardY + Card::GetHeight())
    {
        return true;
    }
    return false;
}

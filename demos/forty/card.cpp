/////////////////////////////////////////////////////////////////////////////
// Name:        card.cpp
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
//+-------------------------------------------------------------+
//| Description
//| A class for drawing playing cards.
//| Currently assumes that the card symbols have been
//| loaded into hbmap_symbols and the pictures for the
//| Jack, Queen and King have been loaded into
//| hbmap_pictures.
//+-------------------------------------------------------------+

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
#include <string.h>
#include "forty.h"
#include "card.h"

#include "pictures.xpm"
#include "symbols.xpm"

wxBitmap* Card::m_pictureBmap = 0;
wxBitmap* Card::m_symbolBmap = 0;

double Card::m_scale = 1.0;
int Card::m_width = 50;
int Card::m_height = 70;

//+-------------------------------------------------------------+
//| Card::Card()                                                |
//+-------------------------------------------------------------+
//| Description:                                                |
//| Constructor for a playing card.                             |
//| Checks that the value is in the range 1..52 and then        |
//| initialises the suit, colour, pipValue and wayUp.           |
//+-------------------------------------------------------------+
Card::Card(int value, WayUp way_up) :
      m_wayUp(way_up)
{
    if (!m_symbolBmap)
    {
        m_symbolBmap = new wxBitmap(symbols_xpm);
        if (!m_symbolBmap->IsOk())
        {
            ::wxMessageBox(wxT("Failed to load bitmap CardSymbols"), wxT("Error"));
        }
    }
    if (!m_pictureBmap)
    {
        m_pictureBmap = new wxBitmap(Pictures);
        if (!m_pictureBmap->IsOk())
        {
            ::wxMessageBox(wxT("Failed to load bitmap CardPictures"), wxT("Error"));
        }
    }

    if (value >= 1 && value <= PackSize)
    {
        switch ((value - 1) / 13)
        {
            case 0:
                m_suit = clubs;
                m_colour = black;
                break;
            case 1:
                m_suit = diamonds;
                m_colour = red;
                break;
            case 2:
                m_suit = hearts;
                m_colour = red;
                break;
            case 3:
                m_suit = spades;
                m_colour = black;
                break;
        }
        m_pipValue = 1 + (value - 1) % 13;
        m_status = true;
    }
    else
    {
        m_status = false;
    }
} // Card::Card()


//+-------------------------------------------------------------+
//| Card::SetScale()                                            |
//+-------------------------------------------------------------+
//| Description:                                                |
//|     Scales the cards                                        |
//+-------------------------------------------------------------+
void Card::SetScale(double scale)
{
    m_scale = scale;
    m_width = int(50*scale);
    m_height = int(70*scale);
}

//+-------------------------------------------------------------+
//| Card::Erase()                                               |
//+-------------------------------------------------------------+
//| Description:                                                |
//| Erase the card at (x, y) by drawing a rectangle in the      |
//| background colour.                                          |
//+-------------------------------------------------------------+
void Card::Erase(wxDC& dc, int x, int y)
{
    wxPen* pen = wxThePenList->FindOrCreatePen(
                        FortyApp::BackgroundColour(),
                        1,
                        wxSOLID
                        );
    dc.SetPen(* pen);
    dc.SetBrush(FortyApp::BackgroundBrush());
        dc.DrawRectangle(x, y, m_width, m_height);
} // Card::Erase()


//+-------------------------------------------------------------+
//| Card::Draw()                                                |
//+-------------------------------------------------------------+
//| Description:                                                |
//| Draw the card at (x, y).                                    |
//| If the card is facedown draw the back of the card.          |
//| If the card is faceup draw the front of the card.           |
//| Cards are not held in bitmaps, instead they are drawn       |
//| from their constituent parts when required.                 |
//| hbmap_symbols contains large and small suit symbols and     |
//| pip values. These are copied to the appropriate part of     |
//| the card. Picture cards use the pictures defined in         |
//| hbmap_pictures. Note that only one picture is defined       |
//| for the Jack, Queen and King, unlike a real pack where      |
//| each suit is different.                                     |
//|                                                             |
//| WARNING:                                                    |
//| The locations of these symbols is 'hard-wired' into the     |
//| code. Editing the bitmaps or the numbers below will         |
//| result in the wrong symbols being displayed.                |
//+-------------------------------------------------------------+
void Card::Draw(wxDC& dc, int x, int y)
{
    wxBrush backgroundBrush( dc.GetBackground() );
    dc.SetBrush(* wxWHITE_BRUSH);
    dc.SetPen(* wxBLACK_PEN);
        dc.DrawRoundedRectangle(x, y, m_width, m_height, 4);
    if (m_wayUp == facedown)
    {
        dc.SetBackground(* wxRED_BRUSH);
        dc.SetBackgroundMode(wxSOLID);
        wxBrush* brush = wxTheBrushList->FindOrCreateBrush(
                            *wxBLACK, wxCROSSDIAG_HATCH
                            );
        dc.SetBrush(* brush);

        dc.DrawRoundedRectangle(
                x + 4, y + 4,
                m_width - 8, m_height - 8,
                2
                );
    }
    else
    {
        wxMemoryDC memoryDC;

        memoryDC.SelectObject(*m_symbolBmap);

//        dc.SetBackgroundMode(wxTRANSPARENT);

        dc.SetTextBackground(*wxWHITE);
        switch (m_suit)
        {
            case spades:
            case clubs:
                dc.SetTextForeground(*wxBLACK);
                break;
            case diamonds:
            case hearts:
                dc.SetTextForeground(*wxRED);
                break;
        }

        int symsize = 11;
        int sympos = 14;
        int sympos2 = 25;
        int symdist = 5;
        int symdist2 = 6;

        int pipsize,pippos,valueheight,valuewidth;
        int valuepos;
        if (m_scale > 1.2)
        {
            pipsize = symsize;
            pippos = sympos;
            valueheight = 10;
            valuewidth = 9;
            valuepos = 50;
        }
        else
        {
            pipsize = 7;
            pippos = 0;
            valueheight = 7;
            valuewidth = 6;
            valuepos = 36;
        }

        // Draw the value
        dc.Blit((wxCoord)(x + m_scale*3),
                (wxCoord)(y + m_scale*3),
                valuewidth,
                valueheight,
                &memoryDC,
                valuewidth * (m_pipValue - 1),
                valuepos,
                wxCOPY);
        dc.Blit((wxCoord)(x + m_width - m_scale*3 - valuewidth),
                (wxCoord)(y + m_height - valueheight - m_scale*3),
                valuewidth,
                valueheight,
                &memoryDC,
                valuewidth * (m_pipValue - 1),
                valuepos+valueheight,
                wxCOPY);

        // Draw the pips
        dc.Blit((wxCoord)(x + m_scale*3 + valuewidth+2),
                (wxCoord)(y + m_scale*3),
                pipsize,
                pipsize,
                &memoryDC,
                pipsize * m_suit,
                pippos,
                wxCOPY);
        dc.Blit((wxCoord)(x + m_width - m_scale*3-valuewidth-pipsize-2),
                (wxCoord)(y + m_height - pipsize - m_scale*3),
                pipsize,
                pipsize,
                &memoryDC,
                pipsize * m_suit,
                pipsize+pippos,
                wxCOPY);

        switch (m_pipValue)
        {
        case 1:
            dc.Blit((wxCoord)(x - symdist + m_width / 2),
                    (wxCoord)(y - m_scale*5 + m_height / 2),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos,
                    wxCOPY);
            break;

        case 3:
            dc.Blit((wxCoord)(x - symdist + m_width / 2),
                    (wxCoord)(y - symdist + m_height / 2),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos,
                    wxCOPY);
        case 2:
            dc.Blit((wxCoord)(x - symdist + m_width / 2),
                    (wxCoord)(y - symdist + m_height / 4),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos,
                    wxCOPY);
            dc.Blit((wxCoord)(x - symdist + m_width / 2),
                    (wxCoord)(y - symdist + 3 * m_height / 4),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos2,
                    wxCOPY);
            break;

        case 5:
            dc.Blit((wxCoord)(x - symdist + m_width / 2),
                    (wxCoord)(y - symdist + m_height / 2),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos,
                    wxCOPY);
        case 4:
            dc.Blit((wxCoord)(x - symdist +  m_width / 4),
                    (wxCoord)(y - symdist + m_height / 4),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos,
                    wxCOPY);
            dc.Blit((wxCoord)(x - symdist + m_width / 4),
                    (wxCoord)(y - symdist + 3 * m_height / 4),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos2,
                    wxCOPY);
            dc.Blit((wxCoord)(x - symdist + 3 * m_width / 4),
                    (wxCoord)(y - symdist + m_height / 4),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos,
                    wxCOPY);
            dc.Blit((wxCoord)(x - symdist + 3 * m_width / 4),
                    (wxCoord)(y - symdist + 3 * m_height / 4),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos2,
                    wxCOPY);
            break;

        case 8:
            dc.Blit((wxCoord)(x - symdist + 5 * m_width / 10),
                    (wxCoord)(y - symdist + 5 * m_height / 8),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos2,
                    wxCOPY);
        case 7:
            dc.Blit((wxCoord)(x - symdist + 5 * m_width / 10),
                    (wxCoord)(y - symdist + 3 * m_height / 8),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos,
                    wxCOPY);
        case 6:
            dc.Blit((wxCoord)(x - symdist + m_width / 4),
                    (wxCoord)(y - symdist + m_height / 4),
                    symsize,
                    symsize,
                    &memoryDC, symsize * m_suit, sympos, wxCOPY);
            dc.Blit((wxCoord)(x - symdist + m_width / 4),
                    (wxCoord)(y - symdist + m_height / 2),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos,
                    wxCOPY);
            dc.Blit((wxCoord)(x - symdist + m_width / 4),
                    (wxCoord)(y - symdist + 3 * m_height / 4),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos2,
                    wxCOPY);
            dc.Blit((wxCoord)(x - symdist + 3 * m_width / 4),
                    (wxCoord)(y - symdist + m_height / 4),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos,
                    wxCOPY);
            dc.Blit((wxCoord)(x - symdist + 3 * m_width / 4),
                    (wxCoord)(y - symdist + m_height / 2),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos,
                    wxCOPY);
            dc.Blit((wxCoord)(x - symdist + 3 * m_width / 4),
                    (wxCoord)(y - symdist + 3 * m_height / 4),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos2,
                    wxCOPY);
            break;

        case 10:
            dc.Blit((wxCoord)(x - symdist + m_width / 2),
                    (wxCoord)(y - symdist + 2 * m_height / 3),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos2,
                    wxCOPY);
        case 9:
            dc.Blit((wxCoord)(x - symdist + m_width / 4),
                    (wxCoord)(y - symdist2 + m_height / 4),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos,
                    wxCOPY);
            dc.Blit((wxCoord)(x - symdist + m_width / 4),
                    (wxCoord)(y - symdist2 + 5 * m_height / 12),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos,
                    wxCOPY);
            dc.Blit((wxCoord)(x - symdist + m_width / 4),
                    (wxCoord)(y - symdist + 7 * m_height / 12),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos2,
                    wxCOPY);
            dc.Blit((wxCoord)(x - symdist + m_width / 4),
                    (wxCoord)(y - symdist + 3 * m_height / 4),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos2,
                    wxCOPY);

            dc.Blit((wxCoord)(x - symdist + 3 * m_width / 4),
                    (wxCoord)(y - symdist2 + m_height / 4),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos,
                    wxCOPY);
            dc.Blit((wxCoord)(x - symdist + 3 * m_width / 4),
                    (wxCoord)(y - symdist2 + 5 * m_height / 12),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos,
                    wxCOPY);
            dc.Blit((wxCoord)(x - symdist + 3 * m_width / 4),
                    (wxCoord)(y - symdist + 7 * m_height / 12),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos2,
                    wxCOPY);
            dc.Blit((wxCoord)(x - symdist + 3 * m_width / 4),
                    (wxCoord)(y - symdist + 3 * m_height / 4),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos2,
                    wxCOPY);
            dc.Blit((wxCoord)(x - symdist + m_width / 2),
                    (wxCoord)(y - symdist + m_height / 3),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos,
                    wxCOPY);
            break;
        case 11:
        case 12:
        case 13:
            memoryDC.SelectObject(*m_pictureBmap);
            int picwidth = 40,picheight = 45;
            dc.Blit((wxCoord)(x + (m_width-picwidth)/2),
                    (wxCoord)(y - picheight/2 + m_height/2),
                    picwidth,
                    picheight,
                    &memoryDC,
                    picwidth * (m_pipValue - 11),
                    0,
                    wxCOPY);

            memoryDC.SelectObject(*m_symbolBmap);
            dc.Blit((wxCoord)(x + m_width-(m_width-picwidth)/2-symsize-3),
                    (wxCoord)(y - picheight/2+m_height/2+1),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos,
                    wxCOPY);
            dc.Blit((wxCoord)(x + (m_width-picwidth)/2+2),
                    (wxCoord)(y + picheight/2 + m_height/2-symsize),
                    symsize,
                    symsize,
                    &memoryDC,
                    symsize * m_suit,
                    sympos2,
                    wxCOPY);
            break;
        }

    }
    dc.SetBackground( backgroundBrush );
} // Card:Draw()


//+-------------------------------------------------------------+
//| Card::DrawNullCard()                                        |
//+-------------------------------------------------------------+
//| Description:                                                |
//| Draws the outline of a card at (x, y).                      |
//| Used to draw place holders for empty piles of cards.        |
//+-------------------------------------------------------------+
void Card::DrawNullCard(wxDC& dc, int x, int y)
{
    wxPen* pen = wxThePenList->FindOrCreatePen(FortyApp::TextColour(), 1, wxSOLID);
    dc.SetBrush(FortyApp::BackgroundBrush());
    dc.SetPen(*pen);
    dc.DrawRoundedRectangle(x, y, m_width, m_height, 4);
} // Card::DrawNullCard()

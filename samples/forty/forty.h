/////////////////////////////////////////////////////////////////////////////
// Name:        forty.h
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
#ifndef _FORTY_H_
#define _FORTY_H_

class FortyApp: public wxApp
{
public:
	bool OnInit();

	static wxColour* BackgroundColour();
	static wxColour* TextColour();
	static wxBrush*  BackgroundBrush();

private:
	static wxColour* m_backgroundColour;
	static wxColour* m_textColour;
	static wxBrush*  m_backgroundBrush;
};

#endif

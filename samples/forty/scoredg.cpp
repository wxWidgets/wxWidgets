/////////////////////////////////////////////////////////////////////////////
// Name:        scoredg.cpp
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

#if defined(__WXMSW__) && !defined(GNUWIN32)
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#include "scorefil.h"
#include "scoredg.h"

class ScoreCanvas : public wxScrolledWindow
{
public:
	ScoreCanvas(wxWindow* parent, ScoreFile* scoreFile);
	virtual ~ScoreCanvas();

	void OnDraw(wxDC& dc);

private:
	wxFont*		m_font;
	wxString	m_text;
};


ScoreCanvas::ScoreCanvas(wxWindow* parent, ScoreFile* scoreFile) :
	wxScrolledWindow(parent)
{
#ifdef __WXGTK__
	m_font = wxTheFontList->FindOrCreateFont(12, wxROMAN, wxNORMAL, wxNORMAL);
#else
	m_font = wxTheFontList->FindOrCreateFont(10, wxSWISS, wxNORMAL, wxNORMAL);
#endif

        wxArrayString players;
	scoreFile->GetPlayerList( players);

	ostrstream os;

	os << "Player\tWins\tGames\tScore\n";
	for (int i = 0; i < players.Count(); i++)
	{
		int wins, games, score;
		scoreFile->ReadPlayersScore(players[i], wins, games, score);
		int average = 0;
		if (games > 0)
		{
			average = (2 * score + games) / (2 * games);
		}

		os << players[i] << '\t' 
		   << wins  << '\t'
		   << games << '\t'
		   << average << '\n';
	}
	os << '\0';
	char* str = os.str();
	m_text = str;
	delete str;
}

ScoreCanvas::~ScoreCanvas()
{
}

void ScoreCanvas::OnDraw(wxDC& dc)
{
	dc.SetFont(m_font);

	const char* str = m_text;
	unsigned int tab = 0;
	unsigned int tabstops[] = { 5, 100, 150, 200 };

	// get the line spacing for the current font
	int lineSpacing;
	{
		long w, h;
		dc.GetTextExtent("Testing", &w, &h);
		lineSpacing = (int)h;
	}

	int y = 0;
	while (*str)
	{
		char text[256];
		char* dest = text;
		
		while (*str && *str >= ' ') *dest++ = *str++;
		*dest = '\0';

		dc.DrawText(text, tabstops[tab], y);

		if (*str == '\t')
		{
			if (tab < sizeof(tabstops) / sizeof(tabstops[0]) - 1)
			{
				tab++;
			}
		}
		else if (*str == '\n')
		{
			tab = 0;
			y += lineSpacing;
		}
		if (*str) str++;
	}
}


ScoreDialog::ScoreDialog(
							wxWindow* parent,
							ScoreFile* file
							) :
	wxDialog(parent, -1, "Scores",
			wxDefaultPosition, wxSize(310, 200),
			wxDIALOG_MODAL | wxDEFAULT_DIALOG_STYLE),
	m_scoreFile(file)
{
	// enable constraints
	SetAutoLayout (TRUE);

	ScoreCanvas* list = new ScoreCanvas(this, m_scoreFile);
	m_OK = new wxButton(this, wxID_OK, "OK");

	wxLayoutConstraints* layout;

	// Constrain the OK button
	layout = new wxLayoutConstraints;
	layout->left.SameAs		(this,	wxLeft,		10);
	layout->bottom.SameAs	(this,	 wxBottom,	10);
	layout->height.AsIs();
	layout->width.AsIs();
	m_OK->SetConstraints(layout);

	// Constrain the list of players
	layout = new wxLayoutConstraints;
	layout->left.SameAs		(this,	wxLeft,		10);
	layout->right.SameAs	(this,	wxRight,	10);
	layout->top.SameAs		(this,	wxTop,		10);
	layout->bottom.SameAs	(m_OK,	wxTop,		10);
	list->SetConstraints(layout);

	Layout();
}

ScoreDialog::~ScoreDialog()
{
}

void ScoreDialog::Display()
{
	Show(TRUE);
}

bool ScoreDialog::OnClose()
{
	// hide the dialog
	// NB don't return TRUE otherwise delete is called
	Show(FALSE);
	return FALSE;
}

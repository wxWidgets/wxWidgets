/////////////////////////////////////////////////////////////////////////////
// Name:        canvas.cpp
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

#include "forty.h"
#include "card.h"
#include "game.h"
#include "scorefil.h"
#include "playerdg.h"
#include "canvas.h"

BEGIN_EVENT_TABLE(FortyCanvas, wxScrolledWindow)
    EVT_MOUSE_EVENTS(FortyCanvas::OnMouseEvent)
END_EVENT_TABLE()

FortyCanvas::FortyCanvas(wxWindow* parent, int x, int y, int w, int h) :
	wxScrolledWindow(parent, -1, wxPoint(x, y), wxSize(w, h)),
	m_helpingHand(TRUE),
	m_rightBtnUndo(TRUE),
	m_playerDialog(0),
	m_leftBtnDown(FALSE)
{
#ifdef __WXGTK__
	m_font = wxTheFontList->FindOrCreateFont(12, wxROMAN, wxNORMAL, wxNORMAL);
#else
	m_font = wxTheFontList->FindOrCreateFont(10, wxSWISS, wxNORMAL, wxNORMAL);
#endif
	SetBackgroundColour(FortyApp::BackgroundColour());
	AllowDoubleClick(true);

	m_handCursor = new wxCursor(wxCURSOR_HAND);
	m_arrowCursor = new wxCursor(wxCURSOR_ARROW);

	wxString name = wxTheApp->GetAppName();
	if (name.Length() <= 0) name = "forty";
	m_scoreFile = new ScoreFile(name);
	m_game = new Game(0, 0, 0);
	m_game->Deal();
}


FortyCanvas::~FortyCanvas()
{
	UpdateScores();
	delete m_game;
	delete m_scoreFile;
}


/*
Write the current player's score back to the score file
*/
void FortyCanvas::UpdateScores()
{
	if (m_player.Length() > 0 && m_scoreFile && m_game)
	{
		m_scoreFile->WritePlayersScore(
				m_player,
				m_game->GetNumWins(),
				m_game->GetNumGames(),
				m_game->GetScore()
				);
	}
}


void FortyCanvas::OnDraw(wxDC& dc)
{
	dc.SetFont(* m_font);
	m_game->Redraw(dc);

	// if player name not set (and selection dialog is not displayed)
	// then ask the player for their name
	if (m_player.Length() == 0 && !m_playerDialog)
	{
		m_playerDialog = new PlayerSelectionDialog(this, m_scoreFile);
		m_playerDialog->ShowModal();
		m_player = m_playerDialog->GetPlayersName();
		if (m_player.Length() > 0)
		{
			// user entered a name - lookup their score
			int wins, games, score;
			m_scoreFile->ReadPlayersScore(m_player, wins, games, score);
			m_game->NewPlayer(wins, games, score);
			m_game->DisplayScore(dc);
			delete m_playerDialog;
			m_playerDialog = 0;
			Refresh();
		}
		else
		{
			// user cancelled the dialog - exit the app
			((wxFrame*)GetParent())->Close(TRUE);
		}
	}
}

/*
Called when the main frame is closed
*/
bool FortyCanvas::OnClose()
{
	if (m_game->InPlay() &&
		wxMessageBox("Are you sure you want to\nabandon the current game?",
			"Warning", wxYES_NO | wxICON_QUESTION) == wxNO)
	{
		return FALSE;
	}
	return TRUE;
}

void FortyCanvas::OnMouseEvent(wxMouseEvent& event)
{
	int mouseX = (int)event.GetX();
	int mouseY = (int)event.GetY();

	wxClientDC dc(this); 
	PrepareDC(dc);
	dc.SetFont(* m_font);

	if (event.LeftDClick())
	{
		if (m_leftBtnDown)
		{
			m_leftBtnDown = FALSE;
			ReleaseMouse();
			m_game->LButtonUp(dc, mouseX, mouseY);
		}
		m_game->LButtonDblClk(dc, mouseX, mouseY);
	}
	else if (event.LeftDown())
	{
		if (!m_leftBtnDown)
		{
			m_leftBtnDown = TRUE;
			CaptureMouse();
			m_game->LButtonDown(dc, mouseX, mouseY);
		}
	}
	else if (event.LeftUp())
	{
		if (m_leftBtnDown)
		{
			m_leftBtnDown = FALSE;
			ReleaseMouse();
			m_game->LButtonUp(dc, mouseX, mouseY);
		}
	}
	else if (event.RightDown() && !event.LeftIsDown())
	{
		// only allow right button undo if m_rightBtnUndo is TRUE
		if (m_rightBtnUndo)
		{
			if (event.ControlDown() || event.ShiftDown())
			{
				m_game->Redo(dc);
			}
			else
			{
				m_game->Undo(dc);
			}
		}
	}
	else if (event.Dragging())
	{
		m_game->MouseMove(dc, mouseX, mouseY);
	}

	if (!event.LeftIsDown())
	{
		SetCursorStyle(mouseX, mouseY);
	}
}

void FortyCanvas::SetCursorStyle(int x, int y)
{
	if (m_game->HaveYouWon())
	{
		if (wxMessageBox("Do you wish to play again?",
			"Well Done, You have won!", wxYES_NO | wxICON_QUESTION) == wxYES)
		{
			m_game->Deal();

			wxClientDC dc(this); 
			PrepareDC(dc);
			dc.SetFont(* m_font);
			m_game->Redraw(dc);
		}
		else
		{
			// user cancelled the dialog - exit the app
			((wxFrame*)GetParent())->Close(TRUE);
		}
	}

	// Only set cursor to a hand if 'helping hand' is enabled and
	// the card under the cursor can go somewhere 
	if (m_game->CanYouGo(x, y) && m_helpingHand)
	{
		SetCursor(* m_handCursor);
	}
	else
	{
		SetCursor(* m_arrowCursor);
	}

}

void FortyCanvas::NewGame()
{
	m_game->Deal();
	Refresh();
}

void FortyCanvas::Undo()
{
	wxClientDC dc(this); 
	PrepareDC(dc);
	dc.SetFont(* m_font);
	m_game->Undo(dc);
}

void FortyCanvas::Redo()
{
	wxClientDC dc(this); 
	PrepareDC(dc);
	dc.SetFont(* m_font);
	m_game->Redo(dc);
}

/////////////////////////////////////////////////////////////////////////////
// Name:        forty.cpp
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

#include "canvas.h"
#include "forty.h"
#include "scoredg.h"
#ifdef wx_x
#include "cards.xbm"
#endif

class FortyFrame: public wxFrame
{
public:
	FortyFrame(wxFrame* frame, char* title, int x, int y, int w, int h);
	virtual ~FortyFrame();

	bool OnClose();

	// Menu callbacks
	void NewGame(wxCommandEvent& event);
	void Exit(wxCommandEvent& event);
	void About(wxCommandEvent& event);
	void Undo(wxCommandEvent& event);
	void Redo(wxCommandEvent& event);
	void Scores(wxCommandEvent& event);
	void ToggleRightButtonUndo(wxCommandEvent& event);
	void ToggleHelpingHand(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()

private:
	enum MenuCommands { NEW_GAME = 10, SCORES, EXIT,
						UNDO, REDO,
						RIGHT_BUTTON_UNDO, HELPING_HAND,
						ABOUT };

	wxMenuBar*		m_menuBar;
	FortyCanvas*	m_canvas;
};

BEGIN_EVENT_TABLE(FortyFrame, wxFrame)
	EVT_MENU(NEW_GAME, FortyFrame::NewGame)
	EVT_MENU(EXIT, FortyFrame::Exit)
	EVT_MENU(ABOUT, FortyFrame::About)
	EVT_MENU(UNDO, FortyFrame::Undo)
	EVT_MENU(REDO, FortyFrame::Redo)
	EVT_MENU(SCORES, FortyFrame::Scores)
	EVT_MENU(RIGHT_BUTTON_UNDO, FortyFrame::ToggleRightButtonUndo)
	EVT_MENU(HELPING_HAND, FortyFrame::ToggleHelpingHand)
END_EVENT_TABLE()

// Create a new application object
IMPLEMENT_APP	(FortyApp)

wxColour* FortyApp::m_backgroundColour = 0;
wxColour* FortyApp::m_textColour = 0;
wxBrush*  FortyApp::m_backgroundBrush = 0;

bool FortyApp::OnInit()
{
	FortyFrame* frame = new FortyFrame(
			0,
			"Forty Thieves",
			-1, -1, 668, 510
			);

	// Show the frame
	frame->Show(TRUE);

	return true;
}

const wxColour& FortyApp::BackgroundColour()
{
	if (!m_backgroundColour)
	{
		m_backgroundColour = new wxColour(0, 128, 0);
	}

	return *m_backgroundColour;
}

const wxBrush& FortyApp::BackgroundBrush()
{
	if (!m_backgroundBrush)
	{
		m_backgroundBrush = new wxBrush(BackgroundColour(), wxSOLID);
	}

	return *m_backgroundBrush;
}

const wxColour& FortyApp::TextColour()
{
	if (!m_textColour)
	{
		m_textColour = new wxColour("BLACK");
	}

	return *m_textColour;
}

// My frame constructor
FortyFrame::FortyFrame(wxFrame* frame, char* title, int x, int y, int w, int h):
	wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
	// set the icon
#ifdef __WXMSW__
	SetIcon(wxIcon("CardsIcon"));
#else
#ifdef GTK_TBD
	SetIcon(wxIcon(Cards_bits, Cards_width, Cards_height));
#endif
#endif

	// Make a menu bar
	wxMenu* gameMenu = new wxMenu;
	gameMenu->Append(NEW_GAME, "&New", "Start a new game");
	gameMenu->Append(SCORES, "&Scores...", "Displays scores");
	gameMenu->Append(EXIT, "E&xit", "Exits Forty Thieves");

	wxMenu* editMenu = new wxMenu;
	editMenu->Append(UNDO, "&Undo", "Undo the last move");
	editMenu->Append(REDO, "&Redo", "Redo a move that has been undone");

	wxMenu*	optionsMenu = new wxMenu;
	optionsMenu->Append(RIGHT_BUTTON_UNDO,
			"&Right button undo",
			"Enables/disables right mouse button undo and redo",
			TRUE
			);
	optionsMenu->Append(HELPING_HAND,
			"&Helping hand",
			"Enables/disables hand cursor when a card can be moved",
			TRUE
			);
	optionsMenu->Check(HELPING_HAND, TRUE);
	optionsMenu->Check(RIGHT_BUTTON_UNDO, TRUE);

	wxMenu* helpMenu = new wxMenu;
	helpMenu->Append(ABOUT, "&About", "Displays program version information");

	m_menuBar = new wxMenuBar;
	m_menuBar->Append(gameMenu,    "&Game");
	m_menuBar->Append(editMenu,    "&Edit");
	m_menuBar->Append(optionsMenu, "&Options");
	m_menuBar->Append(helpMenu,    "&Help");

	SetMenuBar(m_menuBar);

	m_canvas = new FortyCanvas(this, 0, 0, 400, 400);
	wxLayoutConstraints* constr = new wxLayoutConstraints;
	constr->left.SameAs(this, wxLeft);
	constr->top.SameAs(this, wxTop);
	constr->right.SameAs(this, wxRight);
	constr->height.SameAs(this, wxHeight);
	m_canvas->SetConstraints(constr);

	CreateStatusBar();
}

FortyFrame::~FortyFrame()
{
}

bool FortyFrame::OnClose()
{
	return m_canvas->OnClose();
}

void
FortyFrame::NewGame(wxCommandEvent&)
{
	m_canvas->NewGame();
}

void
FortyFrame::Exit(wxCommandEvent&)
{
#ifdef __WXGTK__
	// wxGTK doesn't call OnClose() so we do it here
	if (OnClose())
#endif
	Close(TRUE);
}

void
FortyFrame::About(wxCommandEvent&)
{
	wxMessageBox(
		"Forty Thieves\n\n"
		"A freeware program using the wxWindows\n"
		"portable C++ GUI toolkit.\n"
		"http://web.ukonline.co.uk/julian.smart/wxwin\n"
		"http://www.freiburg.linux.de/~wxxt\n\n"
		"Author: Chris Breeze (c) 1992-1998\n"
		"email: chris.breeze@iname.com",
		"About Forty Thieves",
		wxOK, this
		);
}

void
FortyFrame::Undo(wxCommandEvent&)
{
	m_canvas->Undo();
}

void
FortyFrame::Redo(wxCommandEvent&)
{
	m_canvas->Redo();
}

void
FortyFrame::Scores(wxCommandEvent&)
{
	m_canvas->UpdateScores();
	ScoreDialog scores(this, m_canvas->GetScoreFile());
	scores.Display();
}

void
FortyFrame::ToggleRightButtonUndo(wxCommandEvent& event)
{
	bool checked = m_menuBar->IsChecked(event.GetId());
	m_canvas->EnableRightButtonUndo(checked);
}

void
FortyFrame::ToggleHelpingHand(wxCommandEvent& event)
{
	bool checked = m_menuBar->IsChecked(event.GetId());
	m_canvas->EnableHelpingHand(checked);
}

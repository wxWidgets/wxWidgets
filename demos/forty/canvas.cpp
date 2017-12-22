/////////////////////////////////////////////////////////////////////////////
// Name:        canvas.cpp
// Purpose:     Forty Thieves patience game
// Author:      Chris Breeze
// Modified by:
// Created:     21/07/97
// Copyright:   (c) 1993-1998 Chris Breeze
// Licence:     wxWindows licence
//---------------------------------------------------------------------------
// Last modified: 22nd July 1998 - ported to wxWidgets 2.0
/////////////////////////////////////////////////////////////////////////////

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

wxBEGIN_EVENT_TABLE(FortyCanvas, wxScrolledWindow)
    EVT_MOUSE_EVENTS(FortyCanvas::OnMouseEvent)
wxEND_EVENT_TABLE()

FortyCanvas::FortyCanvas(wxWindow* parent, const wxPoint& pos, const wxSize& size) :
             wxScrolledWindow(parent, wxID_ANY, pos, size, 0),
             m_helpingHand(true),
             m_rightBtnUndo(true),
             m_playerDialog(0),
             m_leftBtnDown(false)
{
    SetScrollbars(0, 0, 0, 0);

#ifdef __WXGTK__
    m_font = wxTheFontList->FindOrCreateFont(wxFontInfo(12).Family(wxFONTFAMILY_ROMAN));
#else
    m_font = wxTheFontList->FindOrCreateFont(wxFontInfo(10).Family(wxFONTFAMILY_SWISS));
#endif
    SetBackgroundColour(FortyApp::BackgroundColour());

    m_handCursor = new wxCursor(wxCURSOR_HAND);
    m_arrowCursor = new wxCursor(wxCURSOR_ARROW);

    wxString name = wxTheApp->GetAppName();
    if ( name.empty() ) name = wxT("forty");
    m_scoreFile = new ScoreFile(name);
    m_game = new Game(0, 0, 0);
    m_game->Deal();
}


FortyCanvas::~FortyCanvas()
{
    UpdateScores();
    delete m_game;
    delete m_scoreFile;
    delete m_handCursor;
    delete m_arrowCursor;
}


/*
Write the current player's score back to the score file
*/
void FortyCanvas::UpdateScores()
{
    if (!m_player.empty() && m_scoreFile && m_game)
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
#if 0
    // if player name not set (and selection dialog is not displayed)
    // then ask the player for their name
    if (m_player.empty() && !m_playerDialog)
    {
        m_playerDialog = new PlayerSelectionDialog(this, m_scoreFile);
        m_playerDialog->ShowModal();
        m_player = m_playerDialog->GetPlayersName();
        if ( !m_player.empty() )
        {
            // user entered a name - lookup their score
            int wins, games, score;
            m_scoreFile->ReadPlayersScore(m_player, wins, games, score);
            m_game->NewPlayer(wins, games, score);
            m_game->DisplayScore(dc);
            m_playerDialog->Destroy();
            m_playerDialog = 0;
            Refresh(false);
        }
        else
        {
            // user cancelled the dialog - exit the app
            ((wxFrame*)GetParent())->Close(true);
        }
    }
#endif
}

void FortyCanvas::ShowPlayerDialog()
{
    // if player name not set (and selection dialog is not displayed)
    // then ask the player for their name
    if (m_player.empty() && !m_playerDialog)
    {
        m_playerDialog = new PlayerSelectionDialog(this, m_scoreFile);
        m_playerDialog->ShowModal();
        m_player = m_playerDialog->GetPlayersName();
        if ( !m_player.empty() )
        {
            // user entered a name - lookup their score
            int wins, games, score;
            m_scoreFile->ReadPlayersScore(m_player, wins, games, score);
            m_game->NewPlayer(wins, games, score);

            wxClientDC dc(this);
            dc.SetFont(* m_font);
            m_game->DisplayScore(dc);
            m_playerDialog->Destroy();
            m_playerDialog = 0;
            Refresh(false);
        }
        else
        {
            // user cancelled the dialog - exit the app
            ((wxFrame*)GetParent())->Close(true);
        }
    }
}

/*
Called when the main frame is closed
*/
bool FortyCanvas::OnCloseCanvas()
{
    if (m_game->InPlay() &&
        wxMessageBox(wxT("Are you sure you want to\nabandon the current game?"),
            wxT("Warning"), wxYES_NO | wxICON_QUESTION) == wxNO)
    {
        return false;
    }
    return true;
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
            m_leftBtnDown = false;
            ReleaseMouse();
            m_game->LButtonUp(dc, mouseX, mouseY);
        }
        m_game->LButtonDblClk(dc, mouseX, mouseY);
    }
    else if (event.LeftDown())
    {
        if (!m_leftBtnDown)
        {
            m_leftBtnDown = true;
            CaptureMouse();
            m_game->LButtonDown(dc, mouseX, mouseY);
        }
    }
    else if (event.LeftUp())
    {
        if (m_leftBtnDown)
        {
            m_leftBtnDown = false;
            ReleaseMouse();
            m_game->LButtonUp(dc, mouseX, mouseY);
        }
    }
    else if (event.RightDown() && !event.LeftIsDown())
    {
        // only allow right button undo if m_rightBtnUndo is true
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

void FortyCanvas::LayoutGame()
{
       m_game->Layout();
}

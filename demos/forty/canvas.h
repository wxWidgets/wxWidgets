/////////////////////////////////////////////////////////////////////////////
// Name:        canvas.h
// Purpose:     Forty Thieves patience game
// Author:      Chris Breeze
// Modified by:
// Created:     21/07/97
// Copyright:   (c) 1993-1998 Chris Breeze
// Licence:     wxWindows licence
//---------------------------------------------------------------------------
// Last modified: 22nd July 1998 - ported to wxWidgets 2.0
/////////////////////////////////////////////////////////////////////////////
#ifndef _CANVAS_H_
#define _CANVAS_H_

class Card;
class Game;
class ScoreFile;
class PlayerSelectionDialog;

class FortyCanvas: public wxScrolledWindow
{
public:
    FortyCanvas(wxWindow* parent, const wxPoint& pos, const wxSize& size);
    virtual ~FortyCanvas();

    virtual void OnDraw(wxDC& dc) wxOVERRIDE;
    bool OnCloseCanvas();
    void OnMouseEvent(wxMouseEvent& event);
    void SetCursorStyle(int x, int y);

    void NewGame();
    void Undo();
    void Redo();

    ScoreFile* GetScoreFile() const { return m_scoreFile; }
    void UpdateScores();
    void EnableHelpingHand(bool enable) { m_helpingHand = enable; }
    void EnableRightButtonUndo(bool enable) { m_rightBtnUndo = enable; }
    void LayoutGame();
    void ShowPlayerDialog();

    wxDECLARE_EVENT_TABLE();

private:
    wxFont* m_font;
    Game* m_game;
    ScoreFile* m_scoreFile;
    wxCursor* m_arrowCursor;
    wxCursor* m_handCursor;
    bool m_helpingHand;
    bool m_rightBtnUndo;
    wxString m_player;
    PlayerSelectionDialog* m_playerDialog;
    bool m_leftBtnDown;
};

#endif

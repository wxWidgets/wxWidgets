///////////////////////////////////////////////////////////////////////////////
// Name:        bombs.h
// Purpose:     Bombs game
// Author:      P. Foggia 1996
// Modified by: Wlodzimierz Skiba (ABX) since 2003
// Created:     1996
// Copyright:   (c) 1996 P. Foggia
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DEMOS_BOMBS_BOMBS_H_
#define _WX_DEMOS_BOMBS_BOMBS_H_

#include "game.h"

class BombsFrame;

/*
 * Class representing the entire Application
 */
class BombsApp: public wxApp
{
public:
    virtual bool OnInit() override;

private :
    BombsFrame *m_frame;

    BombsGame m_game;

};

wxDECLARE_APP(BombsApp);

class BombsCanvas;

class BombsFrame : public wxFrame
{
public:

    BombsFrame(BombsGame *bombsGame);

    void NewGame(int level, bool query);

private:

    void OnNewGame(wxCommandEvent& event);
    void OnEasyGame(wxCommandEvent& event);
    void OnMediumGame(wxCommandEvent& event);
    void OnHardGame(wxCommandEvent& event);

    void OnEasyCorner(wxCommandEvent& event);

    void OnExit(wxCommandEvent& event);

    void OnAbout(wxCommandEvent& event);

    BombsGame *m_game;
    bool m_easyCorner;
    int m_lastLevel;

    // Subwindows for reference within the program.
    BombsCanvas *m_canvas;

    wxDECLARE_EVENT_TABLE();
};

// App specific menu identifiers
enum
{
    bombsID_LEVEL = wxID_HIGHEST,
    bombsID_EASY,
    bombsID_MEDIUM,
    bombsID_HARD,
    bombsID_EASYCORNER
};

class BombsCanvas : public wxPanel
{
public:

    // Constructor and destructor

    BombsCanvas(wxFrame *parent, BombsGame *game);

    void UpdateGridSize();

    wxSize GetGridSizeInPixels() const;

    virtual ~BombsCanvas();

private:

    void OnPaint(wxPaintEvent& event);
    void DrawField(wxDC *, int xc1, int yc1, int xc2, int yc2);
    void RefreshField(int xc1, int yc1, int xc2, int yc2);
    void Uncover(int x, int y);
    void OnMouseEvent(wxMouseEvent& event);
    void OnChar(wxKeyEvent& event);

    BombsGame *m_game;

    wxBitmap *m_bmp;

    // Cell size in pixels
    int m_cellWidth;
    int m_cellHeight;

    wxDECLARE_EVENT_TABLE();
};

/* The following sizes should probably be redefined */
/* dimensions of a scroll unit, in pixels */
#define X_UNIT 4
#define Y_UNIT 4

/* the dimensions of a cell, in scroll units are in
 * BombsCanvas::x_cell and y_cell
 */

#define BOMBS_FONT wxFont(wxFontInfo(14).Family(wxFONTFAMILY_ROMAN))

#endif // #ifndef _WX_DEMOS_BOMBS_BOMBS_H_


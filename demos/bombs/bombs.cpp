///////////////////////////////////////////////////////////////////////////////
// Name:        bombs.cpp
// Purpose:     Bombs game
// Author:      P. Foggia 1996
// Modified by: Wlodzimierz Skiba (ABX) since 2003
// Created:     1996
// RCS-ID:      $Id$
// Copyright:   (c) 1996 P. Foggia
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#ifndef  WX_PRECOMP
#   include "wx/wx.h"
#endif //precompiled headers

#include "wx/stockitem.h"

#include "bombs.h"

#include <stdlib.h>

#ifndef __WXWINCE__
#   include <time.h>
#endif

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) \
    || defined(__WXMAC__) || defined(__WXMGL__)
#   include "bombs.xpm"
#endif

IMPLEMENT_APP(BombsApp)

#ifdef __WXWINCE__
    STDAPI_(__int64) CeGetRandomSeed();
#endif

// Called to initialize the program
bool BombsApp::OnInit()
{
#ifdef __WXWINCE__
    srand((unsigned) CeGetRandomSeed());
#else
    srand((unsigned) time(NULL));
#endif

    m_frame = new BombsFrame(&m_game);

    m_frame->NewGame(bombsID_EASY, false);

    return true;
}

BEGIN_EVENT_TABLE(BombsFrame, wxFrame)
    EVT_MENU(wxID_NEW,           BombsFrame::OnNewGame)
    EVT_MENU(bombsID_EASY,       BombsFrame::OnEasyGame)
    EVT_MENU(bombsID_MEDIUM,     BombsFrame::OnMediumGame)
    EVT_MENU(bombsID_HARD,       BombsFrame::OnHardGame)
    EVT_MENU(bombsID_EASYCORNER, BombsFrame::OnEasyCorner)
    EVT_MENU(wxID_EXIT,          BombsFrame::OnExit)
    EVT_MENU(wxID_ABOUT,         BombsFrame::OnAbout)
END_EVENT_TABLE()

BombsFrame::BombsFrame(BombsGame *game)
    : wxFrame(NULL, wxID_ANY, wxT("wxBombs"), wxDefaultPosition,
        wxSize(300, 300), wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX)
{
    m_game = game;
    m_easyCorner = false;
    m_lastLevel = bombsID_EASY;

    SetIcon(wxICON(bombs));

#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif

    // Create a menu bar for the frame
    wxMenuBar *menuBar = new wxMenuBar;
    wxMenu *menuFile = new wxMenu;
    wxMenu *menuLevel = new wxMenu;
    menuLevel->AppendRadioItem(bombsID_EASY, wxT("&Easy (10x10)\tCtrl-1"));
    menuLevel->AppendRadioItem(bombsID_MEDIUM, wxT("&Medium (15x15)\tCtrl-2"));
    menuLevel->AppendRadioItem(bombsID_HARD, wxT("&Hard (25x20)\tCtrl-3"));

    menuFile->Append(wxID_NEW, wxT("&New game\tCtrl-N"));
    menuFile->Append(bombsID_LEVEL, wxT("&Level"),menuLevel, wxT("Starts a new game"));
    menuFile->AppendCheckItem(bombsID_EASYCORNER, wxT("&Easy corner"));

    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, wxGetStockLabel(wxID_EXIT), wxT("Quits the application"));

    menuBar->Append(menuFile, wxT("&File"));


    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT, wxT("&About"),
        wxT("Displays the program information") );

    menuBar->Append(menuHelp, wxT("&Help"));

    SetMenuBar(menuBar);

    // Create child subwindows.
    m_canvas = new BombsCanvas(this, m_game);

    // Ensure the subwindows get resized o.k.
    //  OnSize(width, height);

    // Centre frame on the screen.
    Centre(wxBOTH);

    // Show the frame.
    Show();
}

void BombsFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void BombsFrame::NewGame(int level, bool query)
{
    if(query)
    {
       int ok = wxMessageBox(
                  wxT("Start new game regardless previous board?"),
                  wxT("Confirm"),
                  wxYES_NO | wxICON_QUESTION,
                  this
                );
       if(ok!=wxYES)return;
    }

    int numHorzCells = 20, numVertCells = 20;
    m_lastLevel = level;

    switch(level)
    {
    case bombsID_EASY:
        numHorzCells = numVertCells = 10;
        break;

    case bombsID_MEDIUM:
        numHorzCells = numVertCells = 15;
        break;

    case bombsID_HARD:
        numHorzCells = 25; numVertCells = 20;
        break;

    default :
        wxFAIL_MSG(wxT("Invalid level"));
        break;
    }

    m_game->Init(numHorzCells, numVertCells, m_easyCorner);

    GetMenuBar()->Check(level, true);

    m_canvas->UpdateGridSize();
    SetClientSize(m_canvas->GetGridSizeInPixels());
}

void BombsFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(
        wxT("wxBombs (c) 1996 by P. Foggia\n<foggia@amalfi.dis.unina.it>"),
        wxT("About wxBombs") );
}

void BombsFrame::OnNewGame(wxCommandEvent& WXUNUSED(event))
{
    NewGame(m_lastLevel, true);
}

void BombsFrame::OnEasyGame(wxCommandEvent& WXUNUSED(event))
{
    NewGame(bombsID_EASY, true);
}

void BombsFrame::OnMediumGame(wxCommandEvent& WXUNUSED(event))
{
    NewGame(bombsID_MEDIUM, true);
}

void BombsFrame::OnHardGame(wxCommandEvent& WXUNUSED(event))
{
    NewGame(bombsID_HARD, true);
}

void BombsFrame::OnEasyCorner(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    if(m_easyCorner)
        msg = wxT("enable");
    else
        msg = wxT("disable");

    msg = wxT("Do you really want to ") + msg + wxT(" having\ntop left corner always empty for easier start?");

    int ok = wxMessageBox(
               msg,
               wxT("Confirm"),
               wxYES_NO | wxICON_QUESTION,
               this
             );

    if(ok!=wxYES)return;

    m_easyCorner = !m_easyCorner;

    NewGame(m_lastLevel, true);
}

BEGIN_EVENT_TABLE(BombsCanvas, wxPanel)
    EVT_PAINT(BombsCanvas::OnPaint)
    EVT_MOUSE_EVENTS(BombsCanvas::OnMouseEvent)
    EVT_CHAR(BombsCanvas::OnChar)
END_EVENT_TABLE()

BombsCanvas::BombsCanvas(wxFrame *parent, BombsGame *game)
    : wxPanel(parent, wxID_ANY)
{
    m_game = game;
    int sx, sy;
    wxClientDC dc(this);
    wxFont font= BOMBS_FONT;
    dc.SetFont(font);

    wxCoord chw, chh;
    wxString buf = wxT("M");

    dc.GetTextExtent(buf, &chw, &chh);
    dc.SetFont(wxNullFont);

    dc.SetMapMode(wxMM_METRIC);

    int xcm = dc.LogicalToDeviceX(10);
    int ycm = dc.LogicalToDeviceY(10);
    // To have a square cell, there must be :
    //    sx*ycm == sy*xcm
    if (chw*ycm < chh*xcm)
    {
        sy = chh;
        sx = chh*xcm/ycm;
    }
    else
    {
        sx = chw;
        sy = chw*ycm/xcm;
    }

    m_cellWidth = (sx+3+X_UNIT)/X_UNIT;
    m_cellHeight = (sy+3+Y_UNIT)/Y_UNIT;
    dc.SetMapMode(wxMM_TEXT);
    m_bmp = NULL;
}

BombsCanvas::~BombsCanvas()
{
    if (m_bmp)
    {
        delete m_bmp;
        m_bmp = NULL;
    }
}

// Called when canvas needs to be repainted.
void BombsCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    const int numHorzCells = m_game->GetWidth();
    const int numVertCells = m_game->GetHeight();
    // Insert your drawing code here.
    if (!m_bmp)
    {
        wxSize size = dc.GetSize();
        m_bmp = new wxBitmap(size.GetWidth(), size.GetHeight());
        if (m_bmp)
        {
            wxMemoryDC memDC;
            memDC.SelectObject(*m_bmp);
            DrawField(&memDC, 0, 0, numHorzCells-1, numVertCells-1);
            memDC.SelectObject(wxNullBitmap);
        }
    }

    if (m_bmp)
    {
        wxMemoryDC memDC;
        memDC.SelectObject(*m_bmp);
        wxSize size = dc.GetSize();
        dc.Blit(0, 0, size.GetWidth(), size.GetHeight(),
            &memDC, 0, 0, wxCOPY);
      memDC.SelectObject(wxNullBitmap);
    }
    else
    {
        DrawField(&dc, 0, 0, numHorzCells-1, numVertCells-1);
    }
}

void BombsCanvas::UpdateGridSize()
{

    if (m_bmp)
    {
        delete m_bmp;
        m_bmp = NULL;
    }
    SetSize(GetGridSizeInPixels());
    Refresh();
}

wxSize BombsCanvas::GetGridSizeInPixels() const
{
    return wxSize(m_cellWidth*X_UNIT*m_game->GetWidth(),
        m_cellHeight*Y_UNIT*m_game->GetHeight());
}


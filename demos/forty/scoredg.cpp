/////////////////////////////////////////////////////////////////////////////
// Name:        scoredg.cpp
// Purpose:     Forty Thieves patience game
// Author:      Chris Breeze
// Modified by:
// Created:     21/07/97
// Copyright:   (c) 1993-1998 Chris Breeze
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "scorefil.h"
#include "scoredg.h"

// adjust USE_GRID_FOR_SCORE with O or 1 to your preferences
// by default it takes wxGrid component for score display if available in target port
#define USE_GRID_FOR_SCORE     wxUSE_GRID

#if USE_GRID_FOR_SCORE
#include "wx/grid.h"
#else
class ScoreCanvas : public wxScrolledWindow
{
public:
    ScoreCanvas(wxWindow* parent, ScoreFile* scoreFile, const wxPoint& pos, wxSize& size);
    virtual ~ScoreCanvas();

    void OnDraw(wxDC& dc);

private:
    wxFont     *m_font;
    wxString    m_text;
};

ScoreCanvas::ScoreCanvas(wxWindow* parent, ScoreFile* scoreFile, const wxPoint& pos, wxSize& size) :
    wxScrolledWindow(parent, wxID_ANY, pos, size, wxSUNKEN_BORDER)
{
    SetBackgroundColour(*wxWHITE);
#ifdef __WXGTK__
    m_font = wxTheFontList->FindOrCreateFont(wxFontInfo(12).Family(wxFONTFAMILY_ROMAN));
#else
    m_font = wxTheFontList->FindOrCreateFont(wxFontInfo(10).Family(wxFONTFAMILY_SWISS));
#endif

    wxArrayString players;
    scoreFile->GetPlayerList( players);

    wxString os;

    os << wxT("Player\tWins\tGames\tScore\n");
    for (unsigned int i = 0; i < players.Count(); i++)
    {
        int wins, games, score;
        scoreFile->ReadPlayersScore(players[i], wins, games, score);
        int average = 0;
        if (games > 0)
        {
            average = (2 * score + games) / (2 * games);
        }

        os << players[i] << wxT('\t')
           << wins  << wxT('\t')
           << games << wxT('\t')
           << average << wxT('\n');
    }
    os << wxT('\0');
    m_text = os;
}

ScoreCanvas::~ScoreCanvas()
{
}

void ScoreCanvas::OnDraw(wxDC& dc)
{
    dc.SetFont(* m_font);

    const wxChar* str = m_text;
    unsigned int tab = 0;
    unsigned int tabstops[] = { 5, 100, 150, 200 };

    // get the line spacing for the current font
    int lineSpacing;
    {
        wxCoord w, h;
        dc.GetTextExtent(wxT("Testing"), &w, &h);
        lineSpacing = (int)h;
    }

    int y = 0;
    while (*str)
    {
        wxChar text[256];
        wxChar* dest = text;

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
#endif

wxBEGIN_EVENT_TABLE(ScoreDialog, wxDialog)
    EVT_CLOSE(ScoreDialog::OnCloseWindow)
wxEND_EVENT_TABLE()

ScoreDialog::ScoreDialog(wxWindow* parent, ScoreFile* file) :
    wxDialog(parent, wxID_ANY, _("Scores"),
            wxDefaultPosition, wxSize(400, 300)),
    m_scoreFile(file)
{
    // create grid with players
    wxArrayString players;
    file->GetPlayerList(players);

    wxSize sz = wxSize(400, 300);

#if USE_GRID_FOR_SCORE
    wxGrid* list = new wxGrid(this, wxID_ANY, wxDefaultPosition, sz, 0);
    list->CreateGrid(players.Count(), 4);
    for (unsigned int i = 0; i < players.Count(); i++)
    {
        int wins, games, score;
        wxString string_value;

        file->ReadPlayersScore(players[i], wins, games, score);
        int average = 0;
        if (games > 0)
        {
            average = (2 * score + games) / (2 * games);
        }
        list->SetCellValue(i,0,players[i]);
        string_value.Printf( wxT("%u"), wins );
        list->SetCellValue(i,1,string_value);
        string_value.Printf( wxT("%u"), games );
        list->SetCellValue(i,2,string_value);
        string_value.Printf( wxT("%u"), average );
        list->SetCellValue(i,3,string_value);
    }
    list->SetColLabelValue(0, wxT("Players"));
    list->SetColLabelValue(1, wxT("Wins"));
    list->SetColLabelValue(2, wxT("Games"));
    list->SetColLabelValue(3, wxT("Score"));
    list->EnableEditing(false);
    list->AutoSizeColumns();
    list->AutoSizeRows();
    list->SetRowLabelSize(0);
    list->EnableDragRowSize(false);
    list->EnableDragColSize(false);
    list->EnableDragGridSize(false);
    list->ClearSelection();
    list->EnableEditing(false);
    sz.x = wxDefaultCoord;
#else
    ScoreCanvas* list = new ScoreCanvas(this, m_scoreFile, wxDefaultPosition, sz);
#endif

    list->SetInitialSize(sz);

    // locate and resize with sizers
    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
    topsizer->Add( list, 1, wxALL|wxGROW, 10 );
    wxButton *button = new wxButton(this, wxID_OK);
    topsizer->Add( button, 0, wxALIGN_CENTER_HORIZONTAL|wxALL , 10 );
    button->SetFocus();

    SetSizer( topsizer );

    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    CentreOnParent();
}

void ScoreDialog::Display()
{
    ShowModal();
}

void ScoreDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    EndModal(wxID_OK);
}

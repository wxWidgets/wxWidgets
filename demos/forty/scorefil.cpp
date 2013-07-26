/////////////////////////////////////////////////////////////////////////////
// Name:        scorefil.cpp
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

#include "wx/textfile.h"
#include "wx/config.h"
#include "wx/fileconf.h"

#include "scorefil.h"

ScoreFile::ScoreFile(const wxString& appName)
{
    m_config = new wxConfig(appName, wxT("wxWidgets"), appName, wxEmptyString,
                                wxCONFIG_USE_LOCAL_FILE);  // only local
}

ScoreFile::~ScoreFile()
{
    delete m_config;
}


void ScoreFile::GetPlayerList( wxArrayString &list )
{
    m_config->SetPath(wxT("/Players"));
    int length = m_config->GetNumberOfGroups();

    if (length <= 0) return;

    wxString player;
    long index;
    if (m_config->GetFirstGroup(player, index))
    {
         list.Add( player );
        while (m_config->GetNextGroup(player, index))
        {
              list.Add( player );
        }
    }
}


// Calculate an encrypted check number to prevent tampering with
// score file
long ScoreFile::CalcCheck(const wxString& name, int p1, int p2, int p3)
{
    long check = 0;
    size_t i, max = name.length();

    for(i = 0; i < max; ++i )
    {
        check = (check << 1) ^ (long)name[i];
        check = ((check >> 23) ^ check) & 0xFFFFFF;
    }
    check = (check << 1) ^ (long)p1;
    check = ((check >> 23) ^ check) & 0xFFFFFF;
    check = (check << 1) ^ (long)p2;
    check = ((check >> 23) ^ check) & 0xFFFFFF;
    check = (check << 1) ^ (long)p3;
    check = ((check >> 23) ^ check) & 0xFFFFFF;
    return check;
}

wxString ScoreFile::GetPreviousPlayer() const
{
    wxString result;
    m_config->SetPath(wxT("/General"));
    m_config->Read(wxT("LastPlayer"), &result);
    return result;
}

void ScoreFile::ReadPlayersScore(
                        const wxString& player,
                        int& wins,
                        int& games,
                        int& score)
{
    long check = 0;
    long myWins = 0, myGames = 0, myScore = 0;

    games = wins = score = 0;

    m_config->SetPath(wxT("/Players"));
    m_config->SetPath(player);
    if (m_config->Read(wxT("Score"), &myScore, 0L) &&
        m_config->Read(wxT("Games"), &myGames, 0L) &&
        m_config->Read(wxT("Wins"),  &myWins, 0L) &&
        m_config->Read(wxT("Check"), &check, 0L))
    {
        if (check != CalcCheck(player, myGames, myWins, myScore))
        {
            wxMessageBox(wxT("Score file corrupted"), wxT("Warning"),
                                     wxOK | wxICON_EXCLAMATION);
        }
        else
        {
            games = myGames;
            wins = myWins;
            score = myScore;
        }
    }
    WritePlayersScore(player, wins, games, score);
}


void ScoreFile::WritePlayersScore(const wxString& player, int wins, int games, int score)
{
    if (!player.empty())
    {
        m_config->SetPath(wxT("/General"));
        m_config->Write(wxT("LastPlayer"), wxString(player)); // Without wxString tmp, thinks it's bool in VC++

        m_config->SetPath(wxT("/Players"));
        m_config->SetPath(player);
        m_config->Write(wxT("Score"), (long)score);
        m_config->Write(wxT("Games"), (long)games);
        m_config->Write(wxT("Wins"), (long)wins);
        m_config->Write(wxT("Check"), CalcCheck(player, games, wins, score));
    }
}

/////////////////////////////////////////////////////////////////////////////
// Name:        scorefil.cpp
// Purpose:     Forty Thieves patience game
// Author:      Chris Breeze
// Modified by:
// Created:     21/07/97
// RCS-ID:      $Id$
// Copyright:   (c) 1993-1998 Chris Breeze
// Licence:   	wxWindows licence
//---------------------------------------------------------------------------
// Last modified: 14th May 1998 - ported to wxWindows 2.0
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

#ifdef __WXGTK__
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif
#include "wx/textfile.h"
#include "wx/config.h"
#include "wx/fileconf.h"

#include "scorefil.h"

ScoreFile::ScoreFile(const char* appName)
{
#if 0
	wxString filename;
	m_configFilename << "/usr/local/share/" << appName << ".scores";
	if (access(m_configFilename, F_OK) == 0)
	{
		if (access(m_configFilename, R_OK | W_OK) != 0)
		{
			// file is not r/w - use local file instead
			m_configFilename = wxFileConfig::GetLocalFileName(appName);
		}
	}
	else
	{
		int fd = creat(m_configFilename, 0666);

		if (fd < 0)
		{
			// failed to create file - use local file instead
			m_configFilename = wxFileConfig::GetLocalFileName(appName);
		}
		else
		{
			// ensure created file has rw-rw-rw permissions
			close(fd);
		}
	}
#endif

	m_config = new wxConfig(appName, "wxWindows", appName, "", wxCONFIG_USE_LOCAL_FILE);  // only local
}

ScoreFile::~ScoreFile()
{
	delete m_config;
#ifdef __WXGTK__
	// ensure score file has rw-rw-rw permissions
	// (wxFileConfig sets them to rw-------)
	chmod(m_configFilename, 0666);
#endif
}


void ScoreFile::GetPlayerList( wxArrayString &list )
{
	m_config->SetPath("/Players");
	int length = m_config->GetNumberOfGroups();

	if (length <= 0) return;

	wxString player;
	long index, i = 0;
	if (m_config->GetFirstGroup(player, index))
	{
	     list.Add( player );
	     i++;
		while (m_config->GetNextGroup(player, index))
		{
	          list.Add( player );
		  i++;
		}
	}
}


// Calculate an encrypted check number to prevent tampering with
// score file
long ScoreFile::CalcCheck(const char* name, int p1, int p2, int p3)
{
    long check = 0;
    while (*name)
	{
		check = (check << 1) ^ (long)*name++;
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
	m_config->SetPath("/General");
	m_config->Read("LastPlayer", &result);
	return result;
}

void ScoreFile::ReadPlayersScore(
						const char* player,
						int& wins,
						int& games,
						int& score)
{
	long check = 0;
	long myWins = 0, myGames = 0, myScore = 0;

	games = wins = score = 0;

	m_config->SetPath("/Players");
	m_config->SetPath(player);
	if (m_config->Read("Score", &myScore, 0L) &&
		m_config->Read("Games", &myGames, 0L) &&
		m_config->Read("Wins",  &myWins, 0L) &&
		m_config->Read("Check", &check, 0L))
	{
	    if (check != CalcCheck(player, myGames, myWins, myScore))
		{
			wxMessageBox("Score file corrupted", "Warning",
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


void ScoreFile::WritePlayersScore(const char* player, int wins, int games, int score)
{
    if (player)
	{
		m_config->SetPath("/General");
		m_config->Write("LastPlayer", wxString(player)); // Without wxString tmp, thinks it's bool in VC++

		m_config->SetPath("/Players");
		m_config->SetPath(player);
		m_config->Write("Score", (long)score);
		m_config->Write("Games", (long)games);
		m_config->Write("Wins", (long)wins);
		m_config->Write("Check", CalcCheck(player, games, wins, score));
	}
}

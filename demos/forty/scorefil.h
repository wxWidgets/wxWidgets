/////////////////////////////////////////////////////////////////////////////
// Name:        scorefil.h
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
#ifndef _SCOREFILE_H_
#define _SCOREFILE_H_

#include <wx/config.h>

#ifdef __WIN16__
#include <wx/fileconf.h>

#undef wxConfig
#define wxConfig wxFileConfig
#endif

class ScoreFile {
public:
	ScoreFile(const char* appName);
	virtual ~ScoreFile();

	void GetPlayerList( wxArrayString &list );
	wxString GetPreviousPlayer() const;

	void ReadPlayersScore(const char* player, int& wins, int& games, int &score);
	void WritePlayersScore(const char* player, int wins, int games, int score);

private:
	long CalcCheck(const char* name, int p1, int p2, int p3);
	wxString	    m_configFilename;
	wxConfig*	    m_config;
};

#endif

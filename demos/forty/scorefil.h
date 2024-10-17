/////////////////////////////////////////////////////////////////////////////
// Name:        scorefil.h
// Purpose:     Forty Thieves patience game
// Author:      Chris Breeze
// Created:     21/07/97
// Copyright:   (c) 1993-1998 Chris Breeze
// Licence:     wxWindows licence
//---------------------------------------------------------------------------
// Last modified: 22nd July 1998 - ported to wxWidgets 2.0
/////////////////////////////////////////////////////////////////////////////
#ifndef _SCOREFILE_H_
#define _SCOREFILE_H_

#include <wx/config.h>

class ScoreFile {
public:
    ScoreFile(const wxString& appName);
    virtual ~ScoreFile();

    void GetPlayerList( wxArrayString &list );
    wxString GetPreviousPlayer() const;

    void ReadPlayersScore(const wxString& player, int& wins, int& games, int &score);
    void WritePlayersScore(const wxString& player, int wins, int games, int score);

private:
    long CalcCheck(const wxString& name, int p1, int p2, int p3);
    wxString m_configFilename;
    wxConfig* m_config;
};

#endif

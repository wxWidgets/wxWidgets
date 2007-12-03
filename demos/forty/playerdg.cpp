/////////////////////////////////////////////////////////////////////////////
// Name:        playerdg.cpp
// Purpose:     Forty Thieves patience game
// Author:      Chris Breeze
// Modified by:
// Created:     21/07/97
// RCS-ID:      $Id$
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
#include "playerdg.h"

const int ID_LISTBOX = 101;

BEGIN_EVENT_TABLE(PlayerSelectionDialog, wxDialog)
    EVT_SIZE(PlayerSelectionDialog::OnSize)
    EVT_BUTTON(wxID_OK, PlayerSelectionDialog::ButtonCallback)
    EVT_BUTTON(wxID_CANCEL, PlayerSelectionDialog::ButtonCallback)
    EVT_LISTBOX(ID_LISTBOX, PlayerSelectionDialog::SelectCallback)
    EVT_CLOSE(PlayerSelectionDialog::OnCloseWindow)
END_EVENT_TABLE()

PlayerSelectionDialog::PlayerSelectionDialog(
                            wxWindow* parent,
                            ScoreFile* file
                            ) :
    wxDialog(parent, wxID_ANY, _T("Player Selection"), wxDefaultPosition),
    m_scoreFile(file)
{
    wxStaticText* msg = new wxStaticText(this, wxID_ANY, _T("Please select a name or type a new one:"));

    wxListBox* list = new wxListBox(
                        this, ID_LISTBOX,
                        wxDefaultPosition, wxSize(-1, 150),
                        0, 0,
                        wxLB_SINGLE
                        );

    wxArrayString players;
    m_scoreFile->GetPlayerList(players);
    for (unsigned int i = 0; i < players.Count(); i++)
    {
        list->Append(players[i]);
    }

    m_textField = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);

    m_OK = new wxButton(this, wxID_OK);
    m_cancel = new wxButton(this, wxID_CANCEL);

    wxBoxSizer *button_sizer = new wxBoxSizer( wxHORIZONTAL );
    button_sizer->Add( m_OK, 0, wxALL, 10 );
    button_sizer->Add( m_cancel, 0, wxALL, 10 );

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
    topsizer->Add( msg, 0, wxALL , 10 );
    topsizer->Add( list, 1, wxEXPAND | wxLEFT | wxRIGHT, 10 );
    topsizer->Add( m_textField, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10 );
    topsizer->Add( button_sizer, 0, wxALIGN_LEFT );

    SetSizer( topsizer );

    topsizer->SetSizeHints( this );

    CentreOnParent();

    m_OK->SetDefault();
}

void PlayerSelectionDialog::OnSize(wxSizeEvent& WXUNUSED(event))
{
    Layout();
}

const wxString& PlayerSelectionDialog::GetPlayersName()
{
/*
    m_player = wxEmptyString;
    Show(true);
*/
    return m_player;
}

void PlayerSelectionDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    m_player = wxEmptyString;
    EndModal(wxID_CANCEL);
}

void PlayerSelectionDialog::SelectCallback(wxCommandEvent& event)
{
    if (event.GetEventType() == wxEVT_COMMAND_LISTBOX_SELECTED)
    {
//        if (event.IsSelection())
        m_textField->SetValue(event.GetString());
    }
}

void PlayerSelectionDialog::ButtonCallback(wxCommandEvent& event)
{
    if (event.GetId() == wxID_OK)
    {
        wxString name = m_textField->GetValue();
        if (!name.IsNull() && name.Length() > 0)
        {
            if (name.Contains(_T('@')))
            {
                wxMessageBox(_T("Names should not contain the '@' character"), _T("Forty Thieves"));
            }
            else
            {
                m_player = name;
                EndModal(wxID_OK);
            }
        }
        else
        {
             wxMessageBox(_T("Please enter your name"), _T("Forty Thieves"));
        }
    }
    else
    {
        m_player = wxEmptyString;
        EndModal(wxID_CANCEL);
    }
}

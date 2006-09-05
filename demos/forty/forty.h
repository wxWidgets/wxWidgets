/////////////////////////////////////////////////////////////////////////////
// Name:        forty.h
// Purpose:     Forty Thieves patience game
// Author:      Chris Breeze
// Modified by:
// Created:     21/07/97
// RCS-ID:      $Id$
// Copyright:   (c) 1993-1998 Chris Breeze
// Licence:     wxWindows licence
//---------------------------------------------------------------------------
// Last modified: 22nd July 1998 - ported to wxWidgets 2.0
/////////////////////////////////////////////////////////////////////////////
#ifndef _FORTY_H_
#define _FORTY_H_

class FortyApp: public wxApp
{
public:
    FortyApp(){}
    virtual ~FortyApp();
    bool OnInit();

    static const wxColour& BackgroundColour();
    static const wxColour& TextColour();
    static const wxBrush&  BackgroundBrush();
    const wxString& GetHelpFile() const { return m_helpFile; }

private:
    static wxColour* m_backgroundColour;
    static wxColour* m_textColour;
    static wxBrush*  m_backgroundBrush;
    wxString m_helpFile;
};

DECLARE_APP(FortyApp)

class FortyCanvas;
class FortyFrame: public wxFrame
{
public:
    FortyFrame(wxFrame* frame, const wxString& title, const wxPoint& pos, const wxSize& size, bool largecards);
    virtual ~FortyFrame(){};

    void OnCloseWindow(wxCloseEvent& event);

    // Menu callbacks
    void NewGame(wxCommandEvent& event);
    void Exit(wxCommandEvent& event);
    void About(wxCommandEvent& event);
    void Help(wxCommandEvent& event);
    void Undo(wxCommandEvent& event);
    void Redo(wxCommandEvent& event);
    void Scores(wxCommandEvent& event);
    void ToggleRightButtonUndo(wxCommandEvent& event);
    void ToggleHelpingHand(wxCommandEvent& event);
    void ToggleCardSize(wxCommandEvent& event);

    FortyCanvas* GetCanvas() { return m_canvas; }

    DECLARE_EVENT_TABLE()

private:
    enum MenuCommands {
        SCORES = 10,
        RIGHT_BUTTON_UNDO,
        HELPING_HAND,
        LARGE_CARDS
    };

    wxMenuBar* m_menuBar;
    FortyCanvas* m_canvas;
};

//----------------------------------------------------------------------------
// stAboutDialog
//----------------------------------------------------------------------------

class FortyAboutDialog: public wxDialog
{
public:
    // constructors and destructors
    FortyAboutDialog( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE );

    bool AddControls(wxWindow* parent);
};

#define ID_ABOUT_HTML_WINDOW    1000

#endif

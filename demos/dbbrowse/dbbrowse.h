//----------------------------------------------------------------------------------------
// Name:        dbbrowse.h
// Purpose:     Through ODBC - Databases Browsen
// Author:      Mark Johnson
// Modified by:
// Created:     19991127
// Copyright:   (c) Mark Johnson
// Licence:     wxWindows license
// RCS-ID:      $Id$
//----------------------------------------------------------------------------------------
// Define a new frame type
//----------------------------------------------------------------------------------------
class MainFrame: public wxFrame
{
public:
    MainFrame(wxFrame *frame, char *title,  const wxPoint& pos, const wxSize& size);
    ~MainFrame(void);
    
public:
    // menu callbacks
    void OnAbout(wxCommandEvent& event);
    void OnHelp(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    //--------------------------------------------------------------------------------------
    int DiffW, DiffH;                  // Needed the saving of Frame size
    //--------------------------------------------------------------------------------------
    DocSplitterWindow    *p_Splitter;  // for Document Views
    MainDoc              *pDoc;        // Self made Document
    wxHtmlHelpController *p_Help;      // Help System

    //--------------------------------------------------------------------------------------
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------------------
// Define a new application type
//----------------------------------------------------------------------------------------
class MainApp: public wxApp
{
public:
    MainFrame *frame;                           // The one and only MainFrame
    bool OnInit(void); // Programmstart
    wxLocale m_locale; // locale we'll be using and language support - MUST be here !
};

//----------------------------------------------------------------------------------------
// ID for the menu quit command
//----------------------------------------------------------------------------------------
#define QUIT                         777
#define ABOUT                        778
#define HELP                         779
#define TREE_CTRL_PGM                102
#define GRID_CTRL                    103
#define TREE_CTRL_DB                 104
#define GRID_CTRL_DB                 105
//----------------------------------------------------------------------------------------

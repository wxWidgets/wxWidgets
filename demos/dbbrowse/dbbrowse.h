//------------------------------------------------------------------------------
// Name:        dbbrowse.h
// Purpose:     Through ODBC - Databases Browsen
// Author:      Mark Johnson, mj10777@gmx.net
// Modified by:
// Created:     19991127
// Copyright:   (c) Mark Johnson
// Licence:     wxWindows license
//---------------------------------------------------------------------------
// Define a new application type
//------------------------------------------------------------------------------
class MyApp: public wxApp
{
 public:
  bool OnInit(void);
  wxLocale m_locale; // locale we'll be using
};
//------------------------------------------------------------------------------
// Define a new frame type
//------------------------------------------------------------------------------
class MainFrame: public wxFrame
{
 public:
  MainFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
  ~MainFrame(void);

 public:
  // menu callbacks
  void InitializeMenu();
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnHelp(wxCommandEvent& event);
  //--------------------------------------------------------
  wxHtmlHelpController help;
  //--------------------------------------------------------
  int DiffW, DiffH;
  mjDoc *pDoc;
  DocSplitterWindow *p_Splitter;
  //--------------------------------------------------------
 DECLARE_EVENT_TABLE()
};
//------------------------------------------------------------------------------
// ID for the menu quit command
//------------------------------------------------------------------------------
#define QUIT                           1
#define ABOUT                          2
#define HELP                           3
#define SPLITTER_FRAME               100
#define SPLITTER_WINDOW              101
#define TREE_CTRL_PGM                102
#define GRID_CTRL                    103
#define TREE_CTRL_DB                 104
#define GRID_CTRL_DB                 105
//------------------------------------------------------------------------------

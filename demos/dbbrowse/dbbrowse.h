//----------------------------------------------------------------------------------------
// Name:        dbbrowse.h
// Purpose:     Through ODBC - Databases Browsen
// Author:      Mark Johnson, mj10777@gmx.net
// Modified by:
// Created:     19991127
// Copyright:   (c) Mark Johnson
// Licence:     wxWindows license
// RCS-ID:      $Id$
//----------------------------------------------------------------------------------------
// Define a new application type
//----------------------------------------------------------------------------------------
class MainApp: public wxApp
{
 public:
  bool OnInit(void); // Programmstart
  wxLocale m_locale; // locale we'll be using and language support
};
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
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnHelp(wxCommandEvent& event);
  //--------------------------------------------------------------------------------------
  wxHtmlHelpController *p_Help;
  //--------------------------------------------------------------------------------------
  int DiffW, DiffH;
  MainDoc *pDoc;
  DocSplitterWindow *p_Splitter;
  //--------------------------------------------------------------------------------------
 DECLARE_EVENT_TABLE()
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

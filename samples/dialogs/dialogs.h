/////////////////////////////////////////////////////////////////////////////
// Name:        dialogs.h
// Purpose:     Common dialogs demo
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __DIALOGSH__
#define __DIALOGSH__

// Define a new application type
class MyApp: public wxApp
{ public:
    bool OnInit(void);

    wxFont       m_canvasFont;
    wxColour     m_canvasTextColour;
};

// Define a new frame type
class MyFrame: public wxFrame
{ public:
    MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos,
		const wxSize& size);

	void ChooseColour(wxCommandEvent& event);
	void ChooseFont(wxCommandEvent& event);
    void MessageBox(wxCommandEvent& event);
    void SingleChoice(wxCommandEvent& event);
    void TextEntry(wxCommandEvent& event);
    void FileOpen(wxCommandEvent& event);
    void FileSave(wxCommandEvent& event);
    void DirChoose(wxCommandEvent& event);
    void ShowTip(wxCommandEvent& event);

#if !defined(__WXMSW__) || wxTEST_GENERIC_DIALOGS_IN_MSW
	void ChooseColourGeneric(wxCommandEvent& event);
	void ChooseFontGeneric(wxCommandEvent& event);
#endif
	void OnExit(wxCommandEvent& event);
DECLARE_EVENT_TABLE()
};

class MyCanvas: public wxScrolledWindow
{
 public:
  MyCanvas(wxWindow *parent):
      wxScrolledWindow(parent)
    {
    }
  void OnPaint(wxPaintEvent& event);
DECLARE_EVENT_TABLE()
};


// Menu IDs
#define DIALOGS_CHOOSE_COLOUR               1
#define DIALOGS_CHOOSE_COLOUR_GENERIC       2
#define DIALOGS_CHOOSE_FONT                 3
#define DIALOGS_CHOOSE_FONT_GENERIC         4
#define DIALOGS_MESSAGE_BOX                 5
#define DIALOGS_SINGLE_CHOICE               6
#define DIALOGS_TEXT_ENTRY                  7
#define DIALOGS_FILE_OPEN                   8
#define DIALOGS_FILE_SAVE                   9
#define DIALOGS_DIR_CHOOSE                  10
#define DIALOGS_TIP                         11

#endif


/////////////////////////////////////////////////////////////////////////////
// Name:        tex2any.h
// Purpose:     tex2RTF conversion header
// Author:      Julian Smart
// Modified by:
// Created:     7.9.93
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef NO_GUI
// Define a new application type
class MyApp: public wxApp
{ public:
    bool OnInit();
    int OnExit();
};

// Define a new frame type
class MyFrame: public wxFrame
{ public:
    wxTextCtrl *textWindow;
    MyFrame(wxFrame *frame, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size);
    void OnMenuCommand(int id);

    void OnCloseWindow(wxCloseEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnGo(wxCommandEvent& event);
    void OnSetInput(wxCommandEvent& event);
    void OnSetOutput(wxCommandEvent& event);
    void OnSaveFile(wxCommandEvent& event);
    void OnViewOutput(wxCommandEvent& event);
    void OnViewLatex(wxCommandEvent& event);
    void OnLoadMacros(wxCommandEvent& event);
    void OnShowMacros(wxCommandEvent& event);
    void OnModeRTF(wxCommandEvent& event);
    void OnModeWinHelp(wxCommandEvent& event);
    void OnModeHTML(wxCommandEvent& event);
    void OnModeXLP(wxCommandEvent& event);
    void OnHelp(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};

#ifdef __WXMSW__
#include "wx/dde.h"

class Tex2RTFConnection: public wxDDEConnection
{
 public:
  Tex2RTFConnection(char *buf, int size);
  ~Tex2RTFConnection(void);
  bool OnExecute(const wxString& topic, char *data, int size, int format);
  char *OnRequest(const wxString& topic, const wxString& item, int *size, int format);
};

class Tex2RTFServer: public wxDDEServer
{
 public:
  wxConnectionBase *OnAcceptConnection(const wxString& topic);
};

#endif // __WXMSW__

#endif // NO_GUI

/*
 * Itemize/enumerate structure: put on a stack for
 * getting the indentation right
 *
 */

#define LATEX_ENUMERATE   1
#define LATEX_ITEMIZE     2
#define LATEX_DESCRIPTION 3
#define LATEX_TWOCOL      5
#define LATEX_INDENT      6

class ItemizeStruc: public wxObject
{
  public:
    int listType;
    int currentItem;
    int indentation;
    int labelIndentation;
    inline ItemizeStruc(int lType, int indent = 0, int labIndent = 0)
    { listType = lType; currentItem = 0;
      indentation = indent; labelIndentation = labIndent; }
};

// ID for the menu quit command
#define TEX_QUIT 1
#define TEX_GO   2

#define TEX_SET_INPUT   3
#define TEX_SET_OUTPUT  4

#define TEX_VIEW_LATEX  5
#define TEX_VIEW_OUTPUT 6

#define TEX_VIEW_CUSTOM_MACROS 7
#define TEX_LOAD_CUSTOM_MACROS 8

#define TEX_MODE_RTF    9
#define TEX_MODE_WINHELP 10
#define TEX_MODE_HTML   11
#define TEX_MODE_XLP    12

#define TEX_HELP        13
#define TEX_ABOUT       14
#define TEX_SAVE_FILE   15

extern TexChunk *currentMember;
extern bool startedSections;
extern char *contentsString;
extern bool suppressNameDecoration;
extern wxList itemizeStack;

extern FILE *Contents;
extern FILE *Chapters;
extern FILE *Sections;
extern FILE *Subsections;
extern FILE *Subsubsections;

extern char *InputFile;
extern char *OutputFile;
extern char *MacroFile;

extern char *FileRoot;
extern char *ContentsName;    // Contents page from last time around
extern char *TmpContentsName; // Current contents page
extern char *TmpFrameContentsName; // Current frame contents page
extern char *WinHelpContentsFileName; // WinHelp .cnt file
extern char *RefName;         // Reference file name
extern char *bulletFile;

#ifndef NO_GUI
void ChooseOutputFile(bool force = FALSE);
void ChooseInputFile(bool force = FALSE);
#endif

void RTFOnMacro(int macroId, int no_args, bool start);
bool RTFOnArgument(int macroId, int arg_no, bool start);

void HTMLOnMacro(int macroId, int no_args, bool start);
bool HTMLOnArgument(int macroId, int arg_no, bool start);

void XLPOnMacro(int macroId, int no_args, bool start);
bool XLPOnArgument(int macroId, int arg_no, bool start);

bool RTFGo(void);
bool HTMLGo(void);
bool XLPGo(void);

#define ltHARDY         10000

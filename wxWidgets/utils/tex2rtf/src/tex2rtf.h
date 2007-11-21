/////////////////////////////////////////////////////////////////////////////
// Name:        tex2any.h
// Purpose:     tex2RTF conversion header
// Author:      Julian Smart
// Modified by: Wlodzimiez ABX Skiba 2003/2004 Unicode support
// Created:     7.9.93
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/app.h"

#ifndef NO_GUI
    #include "wx/frame.h"
    #include "wx/textctrl.h"

#ifdef __WXMSW__
    #include "wx/dde.h"
#endif

#endif

// Define a new application type
class MyApp: public
                    #ifndef NO_GUI
                        wxApp
                    #else
                        wxAppConsole
                    #endif
{ public:
    bool OnInit();
#ifdef NO_GUI
    int OnRun() { return EXIT_SUCCESS; }
#else
    int OnExit();
#endif
};

#ifndef NO_GUI

// Define a new frame type
class MyFrame: public wxFrame
{ public:
    wxTextCtrl *textWindow;
    MyFrame(wxFrame *frame, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size);

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
    void OnOptionsCurlyBrace(wxCommandEvent& event);
    void OnOptionsSyntaxChecking(wxCommandEvent& event);
    void OnHelp(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};

#ifdef __WXMSW__

class Tex2RTFConnection: public wxDDEConnection
{
 public:
  Tex2RTFConnection(wxChar *buf, int size);
  virtual ~Tex2RTFConnection(void){}
  bool OnExecute(const wxString& topic, wxChar *data, int size, wxIPCFormat format);
  wxChar *OnRequest(const wxString& topic, const wxString& item, int *size, wxIPCFormat format);
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
#define TEX_QUIT wxID_EXIT
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

#define TEX_OPTIONS_CURLY_BRACE 13
#define TEX_OPTIONS_SYNTAX_CHECKING 14

#define TEX_HELP        15
#define TEX_ABOUT       wxID_ABOUT
#define TEX_SAVE_FILE   17

extern TexChunk *currentMember;
extern bool startedSections;
extern wxChar *contentsString;
extern bool suppressNameDecoration;
extern wxList itemizeStack;

extern FILE *Contents;
extern FILE *Chapters;
extern FILE *Sections;
extern FILE *Subsections;
extern FILE *Subsubsections;

extern wxString InputFile;
extern wxString OutputFile;
extern wxChar *MacroFile;

extern wxChar *FileRoot;
extern wxChar *ContentsName;    // Contents page from last time around
extern wxChar *TmpContentsName; // Current contents page
extern wxChar *TmpFrameContentsName; // Current frame contents page
extern wxChar *WinHelpContentsFileName; // WinHelp .cnt file
extern wxChar *RefName;         // Reference file name
extern wxChar *bulletFile;

#ifndef NO_GUI
void ChooseOutputFile(bool force = false);
void ChooseInputFile(bool force = false);
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

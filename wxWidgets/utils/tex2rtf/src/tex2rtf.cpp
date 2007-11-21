/////////////////////////////////////////////////////////////////////////////
// Name:        tex2rtf.cpp
// Purpose:     Converts Latex to linear/WinHelp RTF, HTML, wxHelp.
// Author:      Julian Smart
// Modified by: Wlodzimiez ABX Skiba 2003/2004 Unicode support
//              Ron Lee
// Created:     7.9.93
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if defined(__WXMSW__)
    #include "wx/msw/wrapwin.h"
#endif

#ifndef WX_PRECOMP
    #ifndef NO_GUI
        #include "wx/menu.h"
        #include "wx/textctrl.h"
        #include "wx/filedlg.h"
        #include "wx/msgdlg.h"
        #include "wx/icon.h"
    #endif
#endif

#include "wx/log.h"

#ifndef NO_GUI
    #include "wx/timer.h"
    #include "wx/help.h"
    #include "wx/cshelp.h"
    #include "wx/helphtml.h"
    #ifdef __WXMSW__
        #include "wx/msw/helpchm.h"
    #else
        #include "wx/html/helpctrl.h"
    #endif
#endif // !NO_GUI

#include "wx/utils.h"

#if wxUSE_IOSTREAMH
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

#include <ctype.h>
#include <stdlib.h>
#include "tex2any.h"
#include "tex2rtf.h"
#include "rtfutils.h"
#include "symbols.h"

#if (defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXX11__) || defined(__WXMGL__)) && !defined(NO_GUI)
#include "tex2rtf.xpm"
#endif

#if !WXWIN_COMPATIBILITY_2_4
static inline wxChar* copystring(const wxChar* s)
    { return wxStrcpy(new wxChar[wxStrlen(s) + 1], s); }
#endif

const float versionNo = float(TEX2RTF_VERSION_NUMBER);

TexChunk *currentMember = NULL;
bool startedSections = false;
wxChar *contentsString = NULL;
bool suppressNameDecoration = false;
bool OkToClose = true;
int passNumber = 1;
unsigned long errorCount = 0;

#ifndef NO_GUI

extern wxChar *BigBuffer;
extern wxChar *TexFileRoot;
extern wxChar *TexBibName;         // Bibliography output file name
extern wxChar *TexTmpBibName;      // Temporary bibliography output file name
extern wxList ColourTable;
extern TexChunk *TopLevel;

#if wxUSE_HELP
wxHelpControllerBase *HelpInstance = NULL;
#endif // wxUSE_HELP

#ifdef __WXMSW__
static wxChar *ipc_buffer = NULL;
static wxChar Tex2RTFLastStatus[100];
Tex2RTFServer *TheTex2RTFServer = NULL;
#endif // __WXMSW__

#endif // !NO_GUI

wxChar *bulletFile = NULL;

FILE *Contents = NULL;   // Contents page
FILE *Chapters = NULL;   // Chapters (WinHelp RTF) or rest of file (linear RTF)
FILE *Sections = NULL;
FILE *Subsections = NULL;
FILE *Subsubsections = NULL;
FILE *Popups = NULL;
FILE *WinHelpContentsFile = NULL;

wxString InputFile;
wxString OutputFile;
wxChar *MacroFile = copystring(_T("tex2rtf.ini"));

wxChar *FileRoot = NULL;
wxChar *ContentsName = NULL;    // Contents page from last time around
wxChar *TmpContentsName = NULL; // Current contents page
wxChar *TmpFrameContentsName = NULL; // Current frame contents page
wxChar *WinHelpContentsFileName = NULL; // WinHelp .cnt file
wxChar *RefFileName = NULL;         // Reference file name

wxChar *RTFCharset = copystring(_T("ansi"));

#ifdef __WXMSW__
int BufSize = 100;             // Size of buffer in K
#else
int BufSize = 500;
#endif

bool Go(void);
void ShowOptions(void);
void ShowVersion(void);

wxChar wxTex2RTFBuffer[1500];

#ifdef NO_GUI
    IMPLEMENT_APP_CONSOLE(MyApp)
#else
    wxMenuBar *menuBar = NULL;
    MyFrame *frame = NULL;
    // DECLARE_APP(MyApp)
    IMPLEMENT_APP(MyApp)
#endif

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
  // Use default list of macros defined in tex2any.cc
  DefineDefaultMacros();
  AddMacroDef(ltHARDY, _T("hardy"), 0);

  FileRoot = new wxChar[300];
  ContentsName = new wxChar[300];
  TmpContentsName = new wxChar[300];
  TmpFrameContentsName = new wxChar[300];
  WinHelpContentsFileName = new wxChar[300];
  RefFileName = new wxChar[300];

  WX_CLEAR_LIST(wxList,ColourTable);

  int n = 1;

  // Read input/output files
  if (argc > 1)
  {
      if (argv[1][0] != _T('-'))
      {
          InputFile = argv[1];
          n ++;

          if (argc > 2)
          {
              if (argv[2][0] != _T('-'))
              {
                  OutputFile = argv[2];
                  n ++;
              }
          }
      }
  }

  TexPathList.Add(::wxGetCwd());

  int i;
  for (i = n; i < argc;)
  {
    if (wxStrcmp(argv[i], _T("-winhelp")) == 0)
    {
      i ++;
      convertMode = TEX_RTF;
      winHelp = true;
    }
#ifndef NO_GUI
    else if (wxStrcmp(argv[i], _T("-interactive")) == 0)
    {
      i ++;
      isInteractive = true;
    }
#endif
    else if (wxStrcmp(argv[i], _T("-sync")) == 0)  // Don't yield
    {
      i ++;
      isSync = true;
    }
    else if (wxStrcmp(argv[i], _T("-rtf")) == 0)
    {
      i ++;
      convertMode = TEX_RTF;
    }
    else if (wxStrcmp(argv[i], _T("-html")) == 0)
    {
      i ++;
      convertMode = TEX_HTML;
    }
    else if (wxStrcmp(argv[i], _T("-xlp")) == 0)
    {
      i ++;
      convertMode = TEX_XLP;
    }
    else if (wxStrcmp(argv[i], _T("-twice")) == 0)
    {
      i ++;
      runTwice = true;
    }
    else if (wxStrcmp(argv[i], _T("-macros")) == 0)
    {
      i ++;
      if (i < argc)
      {
        MacroFile = copystring(argv[i]);
        i ++;
      }
    }
    else if (wxStrcmp(argv[i], _T("-bufsize")) == 0)
    {
      i ++;
      if (i < argc)
      {
        BufSize = wxAtoi(argv[i]);
        i ++;
      }
    }
    else if (wxStrcmp(argv[i], _T("-charset")) == 0)
    {
      i ++;
      if (i < argc)
      {
        wxChar *s = argv[i];
        i ++;
        if (wxStrcmp(s, _T("ansi")) == 0 || wxStrcmp(s, _T("pc")) == 0 || wxStrcmp(s, _T("mac")) == 0 ||
            wxStrcmp(s, _T("pca")) == 0)
          RTFCharset = copystring(s);
        else
        {
          OnError(_T("Incorrect argument for -charset"));
          return false;
        }
      }
    }
    else if (wxStrcmp(argv[i], _T("-checkcurlybraces")) == 0)
    {
      i ++;
      checkCurlyBraces = true;
    }
    else if (wxStrcmp(argv[i], _T("-checkcurleybraces")) == 0)
    {
      // Support the old, incorrectly spelled version of -checkcurlybraces
      // so that old scripts which run tex2rtf -checkcurleybraces still work.
      i ++;
      checkCurlyBraces = true;
    }
    else if (wxStrcmp(argv[i], _T("-checksyntax")) == 0)
    {
      i ++;
      checkSyntax = true;
    }
    else if (wxStrcmp(argv[i], _T("-version")) == 0)
    {
      i ++;
      ShowVersion();
#ifdef NO_GUI
      exit(1);
#else
      return false;
#endif
    }
    else
    {
      wxString buf;
      buf.Printf(_T("Invalid switch %s.\n"), argv[i]);
      OnError((wxChar *)buf.c_str());
#ifdef NO_GUI
      ShowOptions();
      exit(1);
#else
      return false;
#endif
    }
  }

#ifdef NO_GUI
  if (InputFile.empty() || OutputFile.empty())
  {
      wxSTD cout << "Tex2RTF: input or output file is missing.\n";
      ShowOptions();
      exit(1);
  }
#endif

  if (!InputFile.empty())
  {
      TexPathList.EnsureFileAccessible(InputFile);
  }
  if (InputFile.empty() || OutputFile.empty())
      isInteractive = true;

#if defined(__WXMSW__) && !defined(NO_GUI)
  wxDDEInitialize();
  Tex2RTFLastStatus[0] = 0; // DDE connection return value
  TheTex2RTFServer = new Tex2RTFServer;
  TheTex2RTFServer->Create(_T("TEX2RTF"));
#endif

  TexInitialize(BufSize);
  ResetContentsLevels(0);

#ifndef NO_GUI

  if (isInteractive)
  {
    // Create the main frame window
    frame = new MyFrame(NULL, wxID_ANY, _T("Tex2RTF"), wxDefaultPosition, wxSize(400, 300));
#if wxUSE_STATUSBAR
    frame->CreateStatusBar(2);
#endif // wxUSE_STATUSBAR

    // Give it an icon
    // TODO: uncomment this when we have tex2rtf.xpm
    frame->SetIcon(wxICON(tex2rtf));

    if (!InputFile.empty())
    {
        wxString title;
        title.Printf( _T("Tex2RTF [%s]"), wxFileNameFromPath(InputFile).c_str());
        frame->SetTitle(title);
    }

    // Make a menubar
    wxMenu *file_menu = new wxMenu;
    file_menu->Append(TEX_GO, _T("&Go"), _T("Run converter"));
    file_menu->Append(TEX_SET_INPUT, _T("Set &Input File"), _T("Set the LaTeX input file"));
    file_menu->Append(TEX_SET_OUTPUT, _T("Set &Output File"), _T("Set the output file"));
    file_menu->AppendSeparator();
    file_menu->Append(TEX_VIEW_LATEX, _T("View &LaTeX File"), _T("View the LaTeX input file"));
    file_menu->Append(TEX_VIEW_OUTPUT, _T("View Output &File"), _T("View output file"));
    file_menu->Append(TEX_SAVE_FILE, _T("&Save log file"), _T("Save displayed text into file"));
    file_menu->AppendSeparator();
    file_menu->Append(TEX_QUIT, _T("E&xit"), _T("Exit Tex2RTF"));

    wxMenu *macro_menu = new wxMenu;

    macro_menu->Append(TEX_LOAD_CUSTOM_MACROS, _T("&Load Custom Macros"), _T("Load custom LaTeX macro file"));
    macro_menu->Append(TEX_VIEW_CUSTOM_MACROS, _T("View &Custom Macros"), _T("View custom LaTeX macros"));

    wxMenu *mode_menu = new wxMenu;

    mode_menu->Append(TEX_MODE_RTF, _T("Output linear &RTF"), _T("Wordprocessor-compatible RTF"));
    mode_menu->Append(TEX_MODE_WINHELP, _T("Output &WinHelp RTF"), _T("WinHelp-compatible RTF"));
    mode_menu->Append(TEX_MODE_HTML, _T("Output &HTML"), _T("HTML World Wide Web hypertext file"));
    mode_menu->Append(TEX_MODE_XLP, _T("Output &XLP"), _T("wxHelp hypertext help file"));

    wxMenu *options_menu = new wxMenu;

    options_menu->Append(TEX_OPTIONS_CURLY_BRACE, _T("Curly brace matching"), _T("Checks for mismatched curly braces"),true);
    options_menu->Append(TEX_OPTIONS_SYNTAX_CHECKING, _T("Syntax checking"), _T("Syntax checking for common errors"),true);

    options_menu->Check(TEX_OPTIONS_CURLY_BRACE, checkCurlyBraces);
    options_menu->Check(TEX_OPTIONS_SYNTAX_CHECKING, checkSyntax);

    wxMenu *help_menu = new wxMenu;

    help_menu->Append(TEX_HELP, _T("&Help"), _T("Tex2RTF Contents Page"));
    help_menu->Append(TEX_ABOUT, _T("&About Tex2RTF"), _T("About Tex2RTF"));

    menuBar = new wxMenuBar;
    menuBar->Append(file_menu, _T("&File"));
    menuBar->Append(macro_menu, _T("&Macros"));
    menuBar->Append(mode_menu, _T("&Conversion Mode"));
    menuBar->Append(options_menu, _T("&Options"));
    menuBar->Append(help_menu, _T("&Help"));

    frame->SetMenuBar(menuBar);
    frame->textWindow = new wxTextCtrl(frame, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_MULTILINE);

    (*frame->textWindow) << _T("Welcome to Tex2RTF.\n");
//    ShowOptions();

#if wxUSE_HELP
#if wxUSE_MS_HTML_HELP && !defined(__WXUNIVERSAL__)
    HelpInstance = new wxCHMHelpController;
#else
    HelpInstance = new wxHelpController;
#endif
    HelpInstance->Initialize(_T("tex2rtf"));
#endif // wxUSE_HELP

    /*
     * Read macro/initialisation file
     *
     */

    wxString path = TexPathList.FindValidPath(MacroFile);
    if (!path.empty())
        ReadCustomMacros(path);

#if wxUSE_STATUSBAR
    wxString inStr(_T("In "));
    switch (convertMode)
    {
        case TEX_RTF:
            if(winHelp)
                inStr += _T("WinHelp RTF");
            else
                inStr += _T("linear RTF");
            break;

        case TEX_HTML:
            inStr += _T("HTML");
            break;

        case TEX_XLP:
            inStr += _T("XLP");
            break;

        default:
            inStr += _T("unknown");
            break;
    }
    inStr += _T(" mode.");
    frame->SetStatusText(inStr, 1);
#endif // wxUSE_STATUSBAR

    frame->Show(true);
    return true;
  }
  else
#endif // NO_GUI
  {
    /*
     * Read macro/initialisation file
     *
     */

    wxString path = TexPathList.FindValidPath(MacroFile);
    if (!path.empty())
        ReadCustomMacros(path);

    bool rc = Go();
    if ( rc && runTwice )
    {
        rc = Go();
    }
#ifdef NO_GUI
    return rc;
#else
    OnExit(); // Do cleanup since OnExit won't be called now
    return false;
#endif
  }
}

#ifndef NO_GUI
int MyApp::OnExit()
{
  wxNode *node = CustomMacroList.GetFirst();
  while (node)
  {
    CustomMacro *macro = (CustomMacro *)node->GetData();
    delete macro;
    delete node;
    node = CustomMacroList.GetFirst();
  }
  MacroDefs.BeginFind();
  wxHashTable::Node* mNode = MacroDefs.Next();
  while (mNode)
  {
    TexMacroDef* def = (TexMacroDef*) mNode->GetData();
    delete def;
    mNode = MacroDefs.Next();
  }
  MacroDefs.Clear();
#ifdef __WXMSW__
  delete TheTex2RTFServer;
  wxDDECleanUp();
#endif

#if wxUSE_HELP
  delete HelpInstance;
#endif // wxUSE_HELP

    if (BigBuffer)
    {
      delete BigBuffer;
      BigBuffer = NULL;
    }
    if (currentArgData)
    {
      delete currentArgData;
      currentArgData = NULL;
    }
    if (TexFileRoot)
    {
      delete TexFileRoot;
      TexFileRoot = NULL;
    }
    if (TexBibName)
    {
      delete TexBibName;
      TexBibName = NULL;
    }
    if (TexTmpBibName)
    {
      delete TexTmpBibName;
      TexTmpBibName = NULL;
    }
    if (FileRoot)
    {
      delete FileRoot;
      FileRoot = NULL;
    }
    if (ContentsName)
    {
      delete ContentsName;
      ContentsName = NULL;
    }
    if (TmpContentsName)
    {
      delete TmpContentsName;
      TmpContentsName = NULL;
    }
    if (TmpFrameContentsName)
    {
      delete TmpFrameContentsName;
      TmpFrameContentsName = NULL;
    }
    if (WinHelpContentsFileName)
    {
      delete WinHelpContentsFileName;
      WinHelpContentsFileName = NULL;
    }
    if (RefFileName)
    {
      delete RefFileName;
      RefFileName = NULL;
    }
    if (TopLevel)
    {
      delete TopLevel;
      TopLevel = NULL;
    }
    if (MacroFile)
    {
      delete MacroFile;
      MacroFile = NULL;
    }
    if (RTFCharset)
    {
      delete RTFCharset;
      RTFCharset = NULL;
    }

    delete [] PageStyle;
    delete [] BibliographyStyleString;
    delete [] DocumentStyleString;
    delete [] bitmapMethod;
    delete [] backgroundColourString;
    delete [] ContentsNameString;
    delete [] AbstractNameString;
    delete [] GlossaryNameString;
    delete [] ReferencesNameString;
    delete [] FiguresNameString;
    delete [] TablesNameString;
    delete [] FigureNameString;
    delete [] TableNameString;
    delete [] IndexNameString;
    delete [] ChapterNameString;
    delete [] SectionNameString;
    delete [] SubsectionNameString;
    delete [] SubsubsectionNameString;
    delete [] UpNameString;
    if (winHelpTitle)
      delete[] winHelpTitle;

  // TODO: this simulates zero-memory leaks!
  // Otherwise there are just too many...
#ifndef __WXGTK__
#if (defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING) || wxUSE_DEBUG_CONTEXT
  wxDebugContext::SetCheckpoint();
#endif
#endif

  return 0;
}
#endif

void ShowVersion(void)
{
    wxChar buf[100];
    wxSnprintf(buf, sizeof(buf), _T("Tex2RTF version %.2f"), versionNo);
    OnInform(buf);
}

void ShowOptions(void)
{
    ShowVersion();
    OnInform(_T("Usage: tex2rtf [input] [output] [switches]\n"));
    OnInform(_T("where valid switches are"));
#ifndef NO_GUI
    OnInform(_T("    -interactive"));
#endif
    OnInform(_T("    -bufsize <size in K>"));
    OnInform(_T("    -charset <pc | pca | ansi | mac> (default ansi)"));
    OnInform(_T("    -twice"));
    OnInform(_T("    -sync"));
    OnInform(_T("    -checkcurlybraces"));
    OnInform(_T("    -checksyntax"));
    OnInform(_T("    -version"));
    OnInform(_T("    -macros <filename>"));
    OnInform(_T("    -winhelp"));
    OnInform(_T("    -rtf"));
    OnInform(_T("    -html"));
    OnInform(_T("    -xlp\n"));
}

#ifndef NO_GUI

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_CLOSE(MyFrame::OnCloseWindow)
    EVT_MENU(TEX_QUIT, MyFrame::OnExit)
    EVT_MENU(TEX_GO, MyFrame::OnGo)
    EVT_MENU(TEX_SET_INPUT, MyFrame::OnSetInput)
    EVT_MENU(TEX_SET_OUTPUT, MyFrame::OnSetOutput)
    EVT_MENU(TEX_SAVE_FILE, MyFrame::OnSaveFile)
    EVT_MENU(TEX_VIEW_LATEX, MyFrame::OnViewLatex)
    EVT_MENU(TEX_VIEW_OUTPUT, MyFrame::OnViewOutput)
    EVT_MENU(TEX_VIEW_CUSTOM_MACROS, MyFrame::OnShowMacros)
    EVT_MENU(TEX_LOAD_CUSTOM_MACROS, MyFrame::OnLoadMacros)
    EVT_MENU(TEX_MODE_RTF, MyFrame::OnModeRTF)
    EVT_MENU(TEX_MODE_WINHELP, MyFrame::OnModeWinHelp)
    EVT_MENU(TEX_MODE_HTML, MyFrame::OnModeHTML)
    EVT_MENU(TEX_MODE_XLP, MyFrame::OnModeXLP)
    EVT_MENU(TEX_OPTIONS_CURLY_BRACE, MyFrame::OnOptionsCurlyBrace)
    EVT_MENU(TEX_OPTIONS_SYNTAX_CHECKING, MyFrame::OnOptionsSyntaxChecking)
    EVT_MENU(TEX_HELP, MyFrame::OnHelp)
    EVT_MENU(TEX_ABOUT, MyFrame::OnAbout)
END_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size):
  wxFrame(frame, id, title, pos, size)
{}

void MyFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
  if (!stopRunning && !OkToClose)
  {
    stopRunning = true;
    runTwice = false;
    return;
  }
  else if (OkToClose)
  {
    this->Destroy();
  }
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
  Close();
//    this->Destroy();
}

void MyFrame::OnGo(wxCommandEvent& WXUNUSED(event))
{
      passNumber = 1;
      errorCount = 0;
      menuBar->EnableTop(0, false);
      menuBar->EnableTop(1, false);
      menuBar->EnableTop(2, false);
      menuBar->EnableTop(3, false);
      textWindow->Clear();
      Tex2RTFYield(true);
      Go();

      if (stopRunning)
      {
#if wxUSE_STATUSBAR
        SetStatusText(_T("Build aborted!"));
#endif // wxUSE_STATUSBAR
        wxString errBuf;
        errBuf.Printf(_T("\nErrors encountered during this pass: %lu\n"), errorCount);
        OnInform((wxChar *)errBuf.c_str());
      }


      if (runTwice && !stopRunning)
      {
        Tex2RTFYield(true);
        Go();
      }
      menuBar->EnableTop(0, true);
      menuBar->EnableTop(1, true);
      menuBar->EnableTop(2, true);
      menuBar->EnableTop(3, true);
}

void MyFrame::OnSetInput(wxCommandEvent& WXUNUSED(event))
{
      ChooseInputFile(true);
}

void MyFrame::OnSetOutput(wxCommandEvent& WXUNUSED(event))
{
      ChooseOutputFile(true);
}

void MyFrame::OnSaveFile(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_FILEDLG
    wxString s = wxFileSelector(_T("Save text to file"), wxEmptyString, wxEmptyString, _T("txt"), _T("*.txt"));
    if (!s.empty())
    {
        textWindow->SaveFile(s);
#if wxUSE_STATUSBAR
        wxChar buf[350];
        wxSnprintf(buf, sizeof(buf), _T("Saved text to %s"), (const wxChar*) s.c_str());
        frame->SetStatusText(buf, 0);
#endif // wxUSE_STATUSBAR
    }
#endif // wxUSE_FILEDLG
}

void MyFrame::OnViewOutput(wxCommandEvent& WXUNUSED(event))
{
    ChooseOutputFile();
    if (!OutputFile.empty() && wxFileExists(OutputFile))
    {
        textWindow->LoadFile(OutputFile);
        wxChar buf[300];
        wxString str(wxFileNameFromPath(OutputFile));
        wxSnprintf(buf, sizeof(buf), _T("Tex2RTF [%s]"), (const wxChar*) str.c_str());
        frame->SetTitle(buf);
    }
}

void MyFrame::OnViewLatex(wxCommandEvent& WXUNUSED(event))
{
    ChooseInputFile();
    if (!InputFile.empty() && wxFileExists(InputFile))
    {
        textWindow->LoadFile(InputFile);
        wxChar buf[300];
        wxString str(wxFileNameFromPath(OutputFile));
        wxSnprintf(buf, sizeof(buf), _T("Tex2RTF [%s]"), (const wxChar*) str.c_str());
        frame->SetTitle(buf);
    }
}

void MyFrame::OnLoadMacros(wxCommandEvent& WXUNUSED(event))
{
    textWindow->Clear();
#if wxUSE_FILEDLG
    wxString s = wxFileSelector(_T("Choose custom macro file"), wxPathOnly(MacroFile), wxFileNameFromPath(MacroFile), _T("ini"), _T("*.ini"));
    if (!s.empty() && wxFileExists(s))
    {
        MacroFile = copystring(s);
        ReadCustomMacros(s);
        ShowCustomMacros();
    }
#endif // wxUSE_FILEDLG
}

void MyFrame::OnShowMacros(wxCommandEvent& WXUNUSED(event))
{
    textWindow->Clear();
    Tex2RTFYield(true);
    ShowCustomMacros();
}

void MyFrame::OnModeRTF(wxCommandEvent& WXUNUSED(event))
{
    convertMode = TEX_RTF;
    winHelp = false;
    InputFile = wxEmptyString;
    OutputFile = wxEmptyString;
#if wxUSE_STATUSBAR
    SetStatusText(_T("In linear RTF mode."), 1);
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnModeWinHelp(wxCommandEvent& WXUNUSED(event))
{
    convertMode = TEX_RTF;
    winHelp = true;
    InputFile = wxEmptyString;
    OutputFile = wxEmptyString;
#if wxUSE_STATUSBAR
    SetStatusText(_T("In WinHelp RTF mode."), 1);
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnModeHTML(wxCommandEvent& WXUNUSED(event))
{
    convertMode = TEX_HTML;
    winHelp = false;
    InputFile = wxEmptyString;
    OutputFile = wxEmptyString;
#if wxUSE_STATUSBAR
    SetStatusText(_T("In HTML mode."), 1);
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnModeXLP(wxCommandEvent& WXUNUSED(event))
{
    convertMode = TEX_XLP;
    InputFile = wxEmptyString;
    OutputFile = wxEmptyString;
#if wxUSE_STATUSBAR
    SetStatusText(_T("In XLP mode."), 1);
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnOptionsCurlyBrace(wxCommandEvent& WXUNUSED(event))
{
    checkCurlyBraces = !checkCurlyBraces;
#if wxUSE_STATUSBAR
    if (checkCurlyBraces)
    {
        SetStatusText(_T("Checking curly braces: YES"), 1);
    }
    else
    {
        SetStatusText(_T("Checking curly braces: NO"), 1);
    }
#endif // wxUSE_STATUSBAR
}


void MyFrame::OnOptionsSyntaxChecking(wxCommandEvent& WXUNUSED(event))
{
    checkSyntax = !checkSyntax;
#if wxUSE_STATUSBAR
    if (checkSyntax)
    {
        SetStatusText(_T("Checking syntax: YES"), 1);
    }
    else
    {
        SetStatusText(_T("Checking syntax: NO"), 1);
    }
#endif // wxUSE_STATUSBAR
}


void MyFrame::OnHelp(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_HELP
      HelpInstance->LoadFile();
      HelpInstance->DisplayContents();
#endif // wxUSE_HELP
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
      wxChar buf[300];
      wxString platform = wxGetOsDescription();
      wxSnprintf(buf, sizeof(buf), _T("Tex2RTF Version %.2f %s\nLaTeX to RTF, WinHelp, and HTML Conversion\n\n(c) Julian Smart, George Tasker and others, 1999-2005"), versionNo, platform.c_str());
      wxMessageBox(buf, _T("About Tex2RTF"));
}

void ChooseInputFile(bool force)
{
#if wxUSE_FILEDLG
    if (force || InputFile.empty())
    {
        wxString s = wxFileSelector(_T("Choose LaTeX input file"), wxPathOnly(InputFile), wxFileNameFromPath(InputFile), _T("tex"), _T("*.tex"));
        if (!s.empty())
        {
            // Different file, so clear index entries.
            ClearKeyWordTable();
            ResetContentsLevels(0);
            passNumber = 1;
            errorCount = 0;

            InputFile = s;
            wxString str = wxFileNameFromPath(InputFile);
            wxString buf;
            buf.Printf(_T("Tex2RTF [%s]"), str.c_str());
            frame->SetTitle((wxChar *)buf.c_str());
            OutputFile = wxEmptyString;
        }
    }
#else
    wxUnusedVar(force);
#endif // wxUSE_FILEDLG
}

void ChooseOutputFile(bool force)
{
    wxChar extensionBuf[10];
    wxChar wildBuf[10];
    wxStrcpy(wildBuf, _T("*."));
    wxString path;
    if (!OutputFile.empty())
        path = wxPathOnly(OutputFile);
    else if (!InputFile.empty())
        path = wxPathOnly(InputFile);

    switch (convertMode)
    {
        case TEX_RTF:
        {
            wxStrcpy(extensionBuf, _T("rtf"));
            wxStrcat(wildBuf, _T("rtf"));
            break;
        }
        case TEX_XLP:
        {
            wxStrcpy(extensionBuf, _T("xlp"));
            wxStrcat(wildBuf, _T("xlp"));
            break;
        }
        case TEX_HTML:
        {
            wxStrcpy(extensionBuf, _T("html"));
            wxStrcat(wildBuf, _T("html"));
            break;
        }
    }
#if wxUSE_FILEDLG
    if (force || OutputFile.empty())
    {
        wxString s = wxFileSelector(_T("Choose output file"), path, wxFileNameFromPath(OutputFile),
                                    extensionBuf, wildBuf);
        if (!s.empty())
            OutputFile = s;
    }
#else
    wxUnusedVar(force);
#endif // wxUSE_FILEDLG
}
#endif

bool Go(void)
{
#ifndef NO_GUI
  ChooseInputFile();
  ChooseOutputFile();
#endif

  if (InputFile.empty() || OutputFile.empty() || stopRunning)
    return false;

#ifndef NO_GUI
  if (isInteractive)
  {
    wxChar buf[300];
    wxString str = wxFileNameFromPath(InputFile);

    wxSnprintf(buf, sizeof(buf), _T("Tex2RTF [%s]"), (const wxChar*) str);
    frame->SetTitle(buf);
  }

  wxLongLong localTime = wxGetLocalTimeMillis();
#endif

  // Find extension-less filename
  wxStrcpy(FileRoot, OutputFile.c_str());
  StripExtension(FileRoot);

  if (truncateFilenames && convertMode == TEX_HTML)
  {
    // Truncate to five characters. This ensures that
    // we can generate DOS filenames such as thing999. But 1000 files
    // may not be enough, of course...
    wxChar* sName = wxFileNameFromPath( FileRoot);  // this Julian's method is non-destructive reference

    if(sName)
      if(wxStrlen( sName) > 5)
        sName[5] = '\0';  // that should do!
  }

  wxSnprintf(ContentsName, 300, _T("%s.con"), FileRoot);
  wxSnprintf(TmpContentsName, 300, _T("%s.cn1"), FileRoot);
  wxSnprintf(TmpFrameContentsName, 300, _T("%s.frc"), FileRoot);
  wxSnprintf(WinHelpContentsFileName, 300, _T("%s.cnt"), FileRoot);
  wxSnprintf(RefFileName, 300, _T("%s.ref"), FileRoot);

  TexPathList.EnsureFileAccessible(InputFile);
  if (!bulletFile)
  {
    wxString s = TexPathList.FindValidPath(_T("bullet.bmp"));
    if (!s.empty())
    {
      wxString str = wxFileNameFromPath(s);
      bulletFile = copystring(str);
    }
  }

  if (wxFileExists(RefFileName))
    ReadTexReferences(RefFileName);

  bool success = false;

  if (!InputFile.empty() && !OutputFile.empty())
  {
    if (!wxFileExists(InputFile))
    {
      OnError(_T("Cannot open input file!"));
      TexCleanUp();
      return false;
    }
#if !defined(NO_GUI) && wxUSE_STATUSBAR
    if (isInteractive)
    {
      wxString buf;
      buf.Printf(_T("Working, pass %d...Click CLOSE to abort"), passNumber);
      frame->SetStatusText((wxChar *)buf.c_str());
    }
#endif
    OkToClose = false;
    OnInform(_T("Reading LaTeX file..."));
    TexLoadFile(InputFile);

    if (stopRunning)
    {
        OkToClose = true;
        return false;
    }

    switch (convertMode)
    {
      case TEX_RTF:
      {
        success = RTFGo();
        break;
      }
      case TEX_XLP:
      {
        success = XLPGo();
        break;
      }
      case TEX_HTML:
      {
        success = HTMLGo();
        break;
      }
    }
  }
  if (stopRunning)
  {
    OnInform(_T("*** Aborted by user."));
    success = false;
    stopRunning = false;
    OkToClose = true;
  }

  if (success)
  {
    WriteTexReferences(RefFileName);
    TexCleanUp();
    startedSections = false;

    wxString buf;
#ifndef NO_GUI
    wxLongLong elapsed = wxGetLocalTimeMillis() - localTime;
    buf.Printf(_T("Finished PASS #%d in %ld seconds.\n"), passNumber, (long)(elapsed.GetLo()/1000.0));
    OnInform((wxChar *)buf.c_str());

    if (errorCount)
    {
        buf.Printf(_T("Errors encountered during this pass: %lu\n"), errorCount);
        OnInform((wxChar *)buf.c_str());
    }

#if wxUSE_STATUSBAR
    if (isInteractive)
    {
      buf.Printf(_T("Done, %d %s."), passNumber, (passNumber > 1) ? _T("passes") : _T("pass"));
      frame->SetStatusText((wxChar *)buf.c_str());
    }
#endif // wxUSE_STATUSBAR
#else
    buf.Printf(_T("Done, %d %s."), passNumber, (passNumber > 1) ? _T("passes") : _T("pass"));
    OnInform((wxChar *)buf.c_str());
    if (errorCount)
    {
        buf.Printf(_T("Errors encountered during this pass: %lu\n"), errorCount);
        OnInform((wxChar *)buf.c_str());
    }
#endif
    passNumber ++;
    errorCount = 0;
    OkToClose = true;
    return true;
  }

  TexCleanUp();
  startedSections = false;

#if !defined(NO_GUI) && wxUSE_STATUSBAR
  frame->SetStatusText(_T("Aborted by user."));
#endif // GUI

  OnInform(_T("Sorry, unsuccessful."));
  OkToClose = true;
  return false;
}

void OnError(const wxChar *msg)
{
    wxString msg_string = msg;
    errorCount++;

#ifdef NO_GUI
    wxSTD cerr << "Error: " << msg_string.mb_str() << "\n";
    wxSTD cerr.flush();
#else
    if (isInteractive && frame)
    {
        (*frame->textWindow) << _T("Error: ") << msg << _T("\n");
    }
    else
    {
#if defined(__UNIX__)
        wxSTD cerr << "Error: " << msg_string.mb_str() << "\n";
        wxSTD cerr.flush();
#elif defined(__WXMSW__)
        wxLogError(msg);
#endif
    }

    Tex2RTFYield(true);
#endif // NO_GUI
}

void OnInform(const wxChar *msg)
{
    wxString msg_string = msg;
#ifdef NO_GUI
    wxSTD cout << msg_string.mb_str() << "\n";
    wxSTD cout.flush();
#else
    if (isInteractive && frame)
    {
       (*frame->textWindow) << msg << _T("\n");
    }
    else
    {
#if defined(__UNIX__)
        wxSTD cout << msg_string.mb_str() << "\n";
        wxSTD cout.flush();
#elif defined(__WXMSW__)
        wxLogInfo(msg);
#endif
    }

    if (isInteractive)
    {
        Tex2RTFYield(true);
    }
#endif // NO_GUI
}

void OnMacro(int macroId, int no_args, bool start)
{
  switch (convertMode)
  {
    case TEX_RTF:
    {
      RTFOnMacro(macroId, no_args, start);
      break;
    }
    case TEX_XLP:
    {
      XLPOnMacro(macroId, no_args, start);
      break;
    }
    case TEX_HTML:
    {
      HTMLOnMacro(macroId, no_args, start);
      break;
    }
  }
}

bool OnArgument(int macroId, int arg_no, bool start)
{
  switch (convertMode)
  {
    case TEX_RTF:
    {
      return RTFOnArgument(macroId, arg_no, start);
      // break;
    }
    case TEX_XLP:
    {
      return XLPOnArgument(macroId, arg_no, start);
      // break;
    }
    case TEX_HTML:
    {
      return HTMLOnArgument(macroId, arg_no, start);
      // break;
    }
  }
  return true;
}

/*
 * DDE Stuff
 */
#if defined(__WXMSW__) && !defined(NO_GUI)

/*
 * Server
 */

wxConnectionBase *Tex2RTFServer::OnAcceptConnection(const wxString& topic)
{
  if (topic == _T("TEX2RTF"))
  {
    if (!ipc_buffer)
      ipc_buffer = new wxChar[1000];

    return new Tex2RTFConnection(ipc_buffer, 4000);
  }
  else
    return NULL;
}

 /*
  * Connection
  */

Tex2RTFConnection::Tex2RTFConnection(wxChar *buf, int size):wxDDEConnection(buf, size)
{
}

bool SplitCommand(wxChar *data, wxChar *firstArg, wxChar *secondArg)
{
  firstArg[0] = 0;
  secondArg[0] = 0;
  int i = 0;
  bool stop = false;
  // Find first argument (command name)
  while (!stop)
  {
    if (data[i] == ' ' || data[i] == 0)
      stop = true;
    else
    {
      firstArg[i] = data[i];
      i ++;
    }
  }
  firstArg[i] = 0;
  if (data[i] == ' ')
  {
    // Find second argument
    i ++;
    int j = 0;
    while (data[i] != 0)
    {
      secondArg[j] = data[i];
      i ++;
      j ++;
    }
    secondArg[j] = 0;
  }
  return true;
}

bool Tex2RTFConnection::OnExecute(const wxString& WXUNUSED(topic), wxChar *data, int WXUNUSED(size), wxIPCFormat WXUNUSED(format))
{
  wxStrcpy(Tex2RTFLastStatus, _T("OK"));

  wxChar firstArg[50];
  wxChar secondArg[300];
  if (SplitCommand(data, firstArg, secondArg))
  {
    bool hasArg = (wxStrlen(secondArg) > 0);
    if (wxStrcmp(firstArg, _T("INPUT")) == 0 && hasArg)
    {
        InputFile = secondArg;
        if (frame)
        {
            wxChar buf[100];
            wxString str = wxFileNameFromPath(InputFile);
            wxSnprintf(buf, sizeof(buf), _T("Tex2RTF [%s]"), (const wxChar*) str);
            frame->SetTitle(buf);
        }
    }
    else if (wxStrcmp(firstArg, _T("OUTPUT")) == 0 && hasArg)
    {
        OutputFile = secondArg;
    }
    else if (wxStrcmp(firstArg, _T("GO")) == 0)
    {
      wxStrcpy(Tex2RTFLastStatus, _T("WORKING"));
      if (!Go())
        wxStrcpy(Tex2RTFLastStatus, _T("CONVERSION ERROR"));
      else
        wxStrcpy(Tex2RTFLastStatus, _T("OK"));
    }
    else if (wxStrcmp(firstArg, _T("EXIT")) == 0)
    {
      if (frame) frame->Close();
    }
    else if (wxStrcmp(firstArg, _T("MINIMIZE")) == 0 || wxStrcmp(firstArg, _T("ICONIZE")) == 0)
    {
      if (frame)
        frame->Iconize(true);
    }
    else if (wxStrcmp(firstArg, _T("SHOW")) == 0 || wxStrcmp(firstArg, _T("RESTORE")) == 0)
    {
      if (frame)
      {
        frame->Iconize(false);
        frame->Show(true);
      }
    }
    else
    {
      // Try for a setting
      wxStrcpy(Tex2RTFLastStatus, RegisterSetting(firstArg, secondArg, false));
#if !defined(NO_GUI) && wxUSE_STATUSBAR
      if (frame && wxStrcmp(firstArg, _T("conversionMode")) == 0)
      {
        wxChar buf[100];
        wxStrcpy(buf, _T("In "));

        if (winHelp && (convertMode == TEX_RTF))
          wxStrcat(buf, _T("WinHelp RTF"));
        else if (!winHelp && (convertMode == TEX_RTF))
          wxStrcat(buf, _T("linear RTF"));
        else if (convertMode == TEX_HTML) wxStrcat(buf, _T("HTML"));
        else if (convertMode == TEX_XLP) wxStrcat(buf, _T("XLP"));
          wxStrcat(buf, _T(" mode."));
        frame->SetStatusText(buf, 1);
      }
#endif
    }
  }
  return true;
}

wxChar *Tex2RTFConnection::OnRequest(const wxString& WXUNUSED(topic), const wxString& WXUNUSED(item), int *WXUNUSED(size), wxIPCFormat WXUNUSED(format))
{
  return Tex2RTFLastStatus;
}

#endif

#ifndef NO_GUI
#ifndef __WXGTK__
//void wxObject::Dump(wxSTD ostream& str)
//{
//  if (GetClassInfo() && GetClassInfo()->GetClassName())
//    str << GetClassInfo()->GetClassName();
//  else
//    str << "unknown object class";
//}
#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// Name:        tex2rtf.cpp
// Purpose:     Converts Latex to linear/WinHelp RTF, HTML, wxHelp.
// Author:      Julian Smart
// Modified by:
// Created:     7.9.93
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifndef NO_GUI
#include <wx/help.h>
#include <wx/timer.h>
#endif

#if defined(NO_GUI) || defined(__UNIX__)
#if wxUSE_IOSTREAMH
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif
#endif

#include <ctype.h>
#include <stdlib.h>
#include "tex2any.h"
#include "tex2rtf.h"
#include "rtfutils.h"

#if (defined(__WXGTK__) || defined(__WXMOTIF__)) && !defined(NO_GUI)
#include "tex2rtf.xpm"
#endif

const float versionNo = 2.0;

TexChunk *currentMember = NULL;
bool startedSections = FALSE;
char *contentsString = NULL;
bool suppressNameDecoration = FALSE;
bool OkToClose = TRUE;
int passNumber = 1;

#ifndef NO_GUI

extern char *BigBuffer;
extern char *TexFileRoot;
extern char *TexBibName;         // Bibliography output file name
extern char *TexTmpBibName;      // Temporary bibliography output file name
extern wxList ColourTable;
extern TexChunk *TopLevel;
extern char *PageStyle;
extern char *BibliographyStyleString;
extern char *DocumentStyleString;
extern char *bitmapMethod;
extern char *backgroundColourString;
extern char *ContentsNameString;
extern char *AbstractNameString;
extern char *GlossaryNameString;
extern char *ReferencesNameString;
extern char *FiguresNameString;
extern char *TablesNameString;
extern char *FigureNameString;
extern char *TableNameString;
extern char *IndexNameString;
extern char *ChapterNameString;
extern char *SectionNameString;
extern char *SubsectionNameString;
extern char *SubsubsectionNameString;
extern char *UpNameString;



#if wxUSE_HELP
wxHelpController *HelpInstance = NULL;
#endif // wxUSE_HELP

#ifdef __WXMSW__
static char *ipc_buffer = NULL;
static char Tex2RTFLastStatus[100];
Tex2RTFServer *TheTex2RTFServer = NULL;
#endif
#endif

char *bulletFile = NULL;

FILE *Contents = NULL;   // Contents page
FILE *Chapters = NULL;   // Chapters (WinHelp RTF) or rest of file (linear RTF)
FILE *Sections = NULL;
FILE *Subsections = NULL;
FILE *Subsubsections = NULL;
FILE *Popups = NULL;
FILE *WinHelpContentsFile = NULL;

char *InputFile = NULL;
char *OutputFile = NULL;
char *MacroFile = copystring("tex2rtf.ini");

char *FileRoot = NULL;
char *ContentsName = NULL;    // Contents page from last time around
char *TmpContentsName = NULL; // Current contents page
char *TmpFrameContentsName = NULL; // Current frame contents page
char *WinHelpContentsFileName = NULL; // WinHelp .cnt file
char *RefName = NULL;         // Reference file name

char *RTFCharset = copystring("ansi");

#ifdef __WXMSW__
int BufSize = 100;             // Size of buffer in K
#else
int BufSize = 500;
#endif

bool Go(void);
void ShowOptions(void);

#ifdef NO_GUI

#if wxUSE_GUI || !defined(__UNIX__)
// wxBase for Unix does not have wxBuffer
extern 
#endif
char *wxBuffer; // we must init it, otherwise tex2rtf will crash

int main(int argc, char **argv)
#else
wxMenuBar *menuBar = NULL;
MyFrame *frame = NULL;

// DECLARE_APP(MyApp)
IMPLEMENT_APP(MyApp)

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
#endif
{
  // Use default list of macros defined in tex2any.cc
  DefineDefaultMacros();
  AddMacroDef(ltHARDY, "hardy", 0);

  FileRoot = new char[300];
  ContentsName = new char[300];
  TmpContentsName = new char[300];
  TmpFrameContentsName = new char[300];
  WinHelpContentsFileName = new char[300];
  RefName = new char[300];

  ColourTable.DeleteContents(TRUE);

  int n = 1;

  // Read input/output files
  if (argc > 1)
  {
    if (argv[1][0] != '-')
    {
      InputFile = argv[1];
      n ++;

      if (argc > 2)
      {
        if (argv[2][0] != '-')
        {
          OutputFile = argv[2];
          n ++;
        }
      }
    }
  }

#ifdef NO_GUI
  wxBuffer = new char[1500];
  // this is done in wxApp, but NO_GUI version doesn't call it :-(

  if (!InputFile || !OutputFile)
  {
    cout << "Tex2RTF: input or output file is missing.\n";
    ShowOptions();
    exit(1);
  }

#endif
  if (InputFile)
  {
    TexPathList.EnsureFileAccessible(InputFile);
  }
  if (!InputFile || !OutputFile)
    isInteractive = TRUE;

  int i;
  for (i = n; i < argc;)
  {
    if (strcmp(argv[i], "-winhelp") == 0)
    {
      i ++;
      convertMode = TEX_RTF;
      winHelp = TRUE;
    }
#ifndef NO_GUI
    else if (strcmp(argv[i], "-interactive") == 0)
    {
      i ++;
      isInteractive = TRUE;
    }
#endif
    else if (strcmp(argv[i], "-sync") == 0)  // Don't yield
    {
      i ++;
      isSync = TRUE;
    }
    else if (strcmp(argv[i], "-rtf") == 0)
    {
      i ++;
      convertMode = TEX_RTF;
    }
    else if (strcmp(argv[i], "-html") == 0)
    {
      i ++;
      convertMode = TEX_HTML;
    }
    else if (strcmp(argv[i], "-xlp") == 0)
    {
      i ++;
      convertMode = TEX_XLP;
    }
    else if (strcmp(argv[i], "-twice") == 0)
    {
      i ++;
      runTwice = TRUE;
    }
    else if (strcmp(argv[i], "-macros") == 0)
    {
      i ++;
      if (i < argc)
      {
        MacroFile = copystring(argv[i]);
        i ++;
      }
    }
    else if (strcmp(argv[i], "-bufsize") == 0)
    {
      i ++;
      if (i < argc)
      {
        BufSize = atoi(argv[i]);
        i ++;
      }
    }
    else if (strcmp(argv[i], "-charset") == 0)
    {
      i ++;
      if (i < argc)
      {
        char *s = argv[i];
        i ++;
        if (strcmp(s, "ansi") == 0 || strcmp(s, "pc") == 0 || strcmp(s, "mac") == 0 ||
            strcmp(s, "pca") == 0)
          RTFCharset = copystring(s);
        else
        {
          OnError("Incorrect argument for -charset");
          return FALSE;
        }
      }
    }
    else
    {
      char buf[100];
      sprintf(buf, "Invalid switch %s.\n", argv[i]);
      OnError(buf);
      i++;
#ifdef NO_GUI
      ShowOptions();
      exit(1);
#endif
      return FALSE;
    }
  }

#if defined(__WXMSW__) && !defined(NO_GUI)
  wxDDEInitialize();
  Tex2RTFLastStatus[0] = 0; // DDE connection return value
  TheTex2RTFServer = new Tex2RTFServer;
  TheTex2RTFServer->Create("TEX2RTF");
#endif

#if defined(__WXMSW__) && defined(__WIN16__)
  // Limit to max Windows array size
  if (BufSize > 64) BufSize = 64;
#endif

  TexInitialize(BufSize);
  ResetContentsLevels(0);

#ifndef NO_GUI

  if (isInteractive)
  {
    char buf[100];

    // Create the main frame window
    frame = new MyFrame(NULL, -1, "Tex2RTF", wxPoint(-1, -1), wxSize(400, 300));
    frame->CreateStatusBar(2);

    // Give it an icon
    // TODO: uncomment this when we have tex2rtf.xpm
    frame->SetIcon(wxICON(tex2rtf));

    if (InputFile)
    {
      sprintf(buf, "Tex2RTF [%s]", FileNameFromPath(InputFile));
      frame->SetTitle(buf);
    }

    // Make a menubar
    wxMenu *file_menu = new wxMenu;
    file_menu->Append(TEX_GO, "&Go",                        "Run converter");
    file_menu->Append(TEX_SET_INPUT, "Set &Input File",     "Set the LaTeX input file");
    file_menu->Append(TEX_SET_OUTPUT, "Set &Output File",   "Set the output file");
    file_menu->AppendSeparator();
    file_menu->Append(TEX_VIEW_LATEX, "View &LaTeX File",   "View the LaTeX input file");
    file_menu->Append(TEX_VIEW_OUTPUT, "View Output &File", "View output file");
    file_menu->Append(TEX_SAVE_FILE, "&Save log file", "Save displayed text into file");
    file_menu->AppendSeparator();
    file_menu->Append(TEX_QUIT, "E&xit",                    "Exit Tex2RTF");

    wxMenu *macro_menu = new wxMenu;

    macro_menu->Append(TEX_LOAD_CUSTOM_MACROS, "&Load Custom Macros", "Load custom LaTeX macro file");
    macro_menu->Append(TEX_VIEW_CUSTOM_MACROS, "View &Custom Macros", "View custom LaTeX macros");

    wxMenu *mode_menu = new wxMenu;

    mode_menu->Append(TEX_MODE_RTF, "Output linear &RTF",   "Wordprocessor-compatible RTF");
    mode_menu->Append(TEX_MODE_WINHELP, "Output &WinHelp RTF", "WinHelp-compatible RTF");
    mode_menu->Append(TEX_MODE_HTML, "Output &HTML",        "HTML World Wide Web hypertext file");
    mode_menu->Append(TEX_MODE_XLP, "Output &XLP",          "wxHelp hypertext help file");

    wxMenu *help_menu = new wxMenu;

    help_menu->Append(TEX_HELP, "&Help", "Tex2RTF Contents Page");
    help_menu->Append(TEX_ABOUT, "&About Tex2RTF", "About Tex2RTF");

    menuBar = new wxMenuBar;
    menuBar->Append(file_menu, "&File");
    menuBar->Append(macro_menu, "&Macros");
    menuBar->Append(mode_menu, "&Conversion Mode");
    menuBar->Append(help_menu, "&Help");

    frame->SetMenuBar(menuBar);
    frame->textWindow = new wxTextCtrl(frame, -1, "", wxPoint(-1, -1), wxSize(-1, -1), wxTE_READONLY|wxTE_MULTILINE);

    (*frame->textWindow) << "Welcome to Julian Smart's LaTeX to RTF converter.\n";
//    ShowOptions();

#if wxUSE_HELP
    HelpInstance = new wxHelpController();
    HelpInstance->Initialize("tex2rtf");
#endif // wxUSE_HELP

    /*
     * Read macro/initialisation file
     *
     */

    wxString path;
    if ((path = TexPathList.FindValidPath(MacroFile)) != "")
      ReadCustomMacros((char*) (const char*) path);

    strcpy(buf, "In ");

    if (winHelp && (convertMode == TEX_RTF))
      strcat(buf, "WinHelp RTF");
    else if (!winHelp && (convertMode == TEX_RTF))
      strcat(buf, "linear RTF");
    else if (convertMode == TEX_HTML) strcat(buf, "HTML");
    else if (convertMode == TEX_XLP) strcat(buf, "XLP");
      strcat(buf, " mode.");
    frame->SetStatusText(buf, 1);

    frame->Show(TRUE);
    return TRUE;
  }
  else
#endif // NO_GUI
  {
    /*
     * Read macro/initialisation file
     *
     */

    wxString path;
    if ((path = TexPathList.FindValidPath(MacroFile)) != "")
      ReadCustomMacros((char*) (const char*) path);

    Go();
    if (runTwice) 
    {
        Go();
    }
#ifdef NO_GUI
    return 0;
#else
    return NULL;
#endif
  }

#ifndef NO_GUI
  // Return the main frame window
  return TRUE;
#else
  delete[] wxBuffer;
  return FALSE;
#endif
}

#ifndef NO_GUI
int MyApp::OnExit()
{
  wxNode *node = CustomMacroList.First();
  while (node)
  {
    CustomMacro *macro = (CustomMacro *)node->Data();
    delete macro;
    delete node;
    node = CustomMacroList.First();
  }
  MacroDefs.BeginFind();
  node = MacroDefs.Next();
  while (node)
  {
    TexMacroDef* def = (TexMacroDef*) node->Data();
    delete def;
    node = MacroDefs.Next();
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
    if (RefName)
    {
      delete RefName;
      RefName = NULL;
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
  wxDebugContext::SetCheckpoint();
#endif

  return 0;
}
#endif
void ShowOptions(void)
{
    char buf[100];
    sprintf(buf, "Tex2RTF version %.2f", versionNo);
    OnInform(buf);
    OnInform("Usage: tex2rtf [input] [output] [switches]\n");
    OnInform("where valid switches are");
    OnInform("    -interactive");
    OnInform("    -bufsize <size in K>");
    OnInform("    -charset <pc | pca | ansi | mac> (default ansi)");
    OnInform("    -twice");
    OnInform("    -sync");
    OnInform("    -macros <filename>");
    OnInform("    -winhelp");
    OnInform("    -rtf");
    OnInform("    -html");
    OnInform("    -xlp\n");
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
    EVT_MENU(TEX_HELP, MyFrame::OnHelp)
    EVT_MENU(TEX_ABOUT, MyFrame::OnAbout)
END_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size):
  wxFrame(frame, id, title, pos, size)
{}

void MyFrame::OnCloseWindow(wxCloseEvent& event)
{
  if (!stopRunning && !OkToClose)
  {
    stopRunning = TRUE;
    runTwice = FALSE;
    return;
  }
  else if (OkToClose)
  {
    this->Destroy();
  }
}

void MyFrame::OnExit(wxCommandEvent& event)
{
  Close();
//    this->Destroy();
}

void MyFrame::OnGo(wxCommandEvent& event)
{
      passNumber = 1;
      menuBar->EnableTop(0, FALSE);
      menuBar->EnableTop(1, FALSE);
      menuBar->EnableTop(2, FALSE);
      menuBar->EnableTop(3, FALSE);
      textWindow->Clear();
      Tex2RTFYield(TRUE);
      Go();

      if (runTwice)
      {
        Tex2RTFYield(TRUE);
        Go();
      }
      menuBar->EnableTop(0, TRUE);
      menuBar->EnableTop(1, TRUE);
      menuBar->EnableTop(2, TRUE);
      menuBar->EnableTop(3, TRUE);
}

void MyFrame::OnSetInput(wxCommandEvent& event)
{
      ChooseInputFile(TRUE);
}

void MyFrame::OnSetOutput(wxCommandEvent& event)
{
      ChooseOutputFile(TRUE);
}

void MyFrame::OnSaveFile(wxCommandEvent& event)
{
      wxString s = wxFileSelector("Save text to file", "", "", "txt", "*.txt");
      if (s != "")
      {
        textWindow->SaveFile(s);
        char buf[350];
        sprintf(buf, "Saved text to %s", (const char*) s);
        frame->SetStatusText(buf, 0);
      }
}

void MyFrame::OnViewOutput(wxCommandEvent& event)
{
      ChooseOutputFile();
      if (OutputFile && wxFileExists(OutputFile))
      {
        textWindow->LoadFile(OutputFile);
        char buf[300];
        wxString str(wxFileNameFromPath(OutputFile));
        sprintf(buf, "Tex2RTF [%s]", (const char*) str);
        frame->SetTitle(buf);
      }
}

void MyFrame::OnViewLatex(wxCommandEvent& event)
{
      ChooseInputFile();
      if (InputFile && wxFileExists(InputFile))
      {
        textWindow->LoadFile(InputFile);
        char buf[300];
        wxString str(wxFileNameFromPath(OutputFile));
        sprintf(buf, "Tex2RTF [%s]", (const char*) str);
        frame->SetTitle(buf);
      }
}

void MyFrame::OnLoadMacros(wxCommandEvent& event)
{
      textWindow->Clear();
      wxString s = wxFileSelector("Choose custom macro file", wxPathOnly(MacroFile), wxFileNameFromPath(MacroFile), "ini", "*.ini");
      if (s != "" && wxFileExists(s))
      {
        MacroFile = copystring(s);
        ReadCustomMacros((char*) (const char*) s);
        ShowCustomMacros();
      }
}

void MyFrame::OnShowMacros(wxCommandEvent& event)
{
      textWindow->Clear();
      Tex2RTFYield(TRUE);
      ShowCustomMacros();
}

void MyFrame::OnModeRTF(wxCommandEvent& event)
{
      convertMode = TEX_RTF;
      winHelp = FALSE;
      InputFile = NULL;
      OutputFile = NULL;
      SetStatusText("In linear RTF mode.", 1);
}

void MyFrame::OnModeWinHelp(wxCommandEvent& event)
{
      convertMode = TEX_RTF;
      winHelp = TRUE;
      InputFile = NULL;
      OutputFile = NULL;
      SetStatusText("In WinHelp RTF mode.", 1);
}

void MyFrame::OnModeHTML(wxCommandEvent& event)
{
      convertMode = TEX_HTML;
      winHelp = FALSE;
      InputFile = NULL;
      OutputFile = NULL;
      SetStatusText("In HTML mode.", 1);
}

void MyFrame::OnModeXLP(wxCommandEvent& event)
{
      convertMode = TEX_XLP;
      InputFile = NULL;
      OutputFile = NULL;
      SetStatusText("In XLP mode.", 1);
}

void MyFrame::OnHelp(wxCommandEvent& event)
{
#if wxUSE_HELP
      HelpInstance->LoadFile();
      HelpInstance->DisplayContents();
#endif // wxUSE_HELP
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
      char buf[300];
#ifdef __WIN32__
      char *platform = " (32-bit)";
#else
#ifdef __WXMSW__
      char *platform = " (16-bit)";
#else
      char *platform = "";
#endif
#endif
      sprintf(buf, "Tex2RTF Version %.2f%s\nLaTeX to RTF, WinHelp, HTML and wxHelp Conversion\n\n(c) Julian Smart 1999", versionNo, platform);
      wxMessageBox(buf, "About Tex2RTF");
}

void ChooseInputFile(bool force)
{
  if (force || !InputFile)
  {
    wxString s = wxFileSelector("Choose LaTeX input file", wxPathOnly(InputFile), wxFileNameFromPath(InputFile), "tex", "*.tex");
    if (s != "")
    {
      // Different file, so clear index entries.
      ClearKeyWordTable();
      ResetContentsLevels(0);
      passNumber = 1;
      char buf[300];
      InputFile = copystring(s);
      wxString str = wxFileNameFromPath(InputFile);
      sprintf(buf, "Tex2RTF [%s]", (const char*) str);
      frame->SetTitle(buf);
      OutputFile = NULL;
    }
  }
}

void ChooseOutputFile(bool force)
{
  char extensionBuf[10];
  char wildBuf[10];
  strcpy(wildBuf, "*.");
  wxString path;
  if (OutputFile)
    path = wxPathOnly(OutputFile);
  else if (InputFile)
    path = wxPathOnly(InputFile);

  switch (convertMode)
  {
    case TEX_RTF:
    {
      strcpy(extensionBuf, "rtf");
      strcat(wildBuf, "rtf");
      break;
    }
    case TEX_XLP:
    {
      strcpy(extensionBuf, "xlp");
      strcat(wildBuf, "xlp");
      break;
    }
    case TEX_HTML:
    {
#if defined(__WXMSW__) && defined(__WIN16__)
      strcpy(extensionBuf, "htm");
      strcat(wildBuf, "htm");
#else
      strcpy(extensionBuf, "html");
      strcat(wildBuf, "html");
#endif
      break;
    }
  }
  if (force || !OutputFile)
  {
    wxString s = wxFileSelector("Choose output file", path, wxFileNameFromPath(OutputFile),
                   extensionBuf, wildBuf);
    if (s != "")
      OutputFile = copystring(s);
  }
}
#endif

bool Go(void)
{
#ifndef NO_GUI
  ChooseInputFile();
  ChooseOutputFile();
#endif

  if (!InputFile || !OutputFile)
    return FALSE;

#ifndef NO_GUI
  if (isInteractive)
  {
    char buf[300];
    wxString str = wxFileNameFromPath(InputFile);

    sprintf(buf, "Tex2RTF [%s]", (const char*) str);
    frame->SetTitle(buf);
  }

  wxStartTimer();
#endif

  // Find extension-less filename
  strcpy(FileRoot, OutputFile);
  StripExtension(FileRoot);

  if (truncateFilenames && convertMode == TEX_HTML)
  {
    // Truncate to five characters. This ensures that
    // we can generate DOS filenames such as thing999. But 1000 files
    // may not be enough, of course...
    char* sName = wxFileNameFromPath( FileRoot);  // this Julian's method is non-destructive reference

    if(sName)
      if(strlen( sName) > 5)
        sName[5] = '\0';  // that should do!
  }

  sprintf(ContentsName, "%s.con", FileRoot);
  sprintf(TmpContentsName, "%s.cn1", FileRoot);
  sprintf(TmpFrameContentsName, "%s.frc", FileRoot);
  sprintf(WinHelpContentsFileName, "%s.cnt", FileRoot);
  sprintf(RefName, "%s.ref", FileRoot);

  TexPathList.EnsureFileAccessible(InputFile);
  if (!bulletFile)
  {
    wxString s = TexPathList.FindValidPath("bullet.bmp");
    if (s != "")
    {
      wxString str = wxFileNameFromPath(s);
      bulletFile = copystring(str);
    }
  }

  if (wxFileExists(RefName))
    ReadTexReferences(RefName);

  bool success = FALSE;

  if (InputFile && OutputFile)
  {
    if (!FileExists(InputFile))
    {
      OnError("Cannot open input file!");
      TexCleanUp();
      return FALSE;
    }
#ifndef NO_GUI
    if (isInteractive)
    {
      char buf[50];
      sprintf(buf, "Working, pass %d...", passNumber);
      frame->SetStatusText(buf);
    }
#endif
    OkToClose = FALSE;
    OnInform("Reading LaTeX file...");
    TexLoadFile(InputFile);

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
    OnInform("*** Aborted by user.");
    success = FALSE;
    stopRunning = FALSE;
  }

  if (success)
  {
    WriteTexReferences(RefName);
    TexCleanUp();
    startedSections = FALSE;

    char buf[100];
#ifndef NO_GUI
    long tim = wxGetElapsedTime();
    sprintf(buf, "Finished PASS #%d in %ld seconds.\n", passNumber, (long)(tim/1000.0));
    OnInform(buf);
    if (isInteractive)
    {
      sprintf(buf, "Done, %d %s.", passNumber, (passNumber > 1) ? "passes" : "pass");
      frame->SetStatusText(buf);
    }
#else
    sprintf(buf, "Done, %d %s.", passNumber, (passNumber > 1) ? "passes" : "pass");
    OnInform(buf);
#endif
    passNumber ++;
    OkToClose = TRUE;
    return TRUE;
  }

  TexCleanUp();
  startedSections = FALSE;

  OnInform("Sorry, unsuccessful.");
  OkToClose = TRUE;
  return FALSE;
}

void OnError(char *msg)
{
#ifdef NO_GUI
  cerr << "Error: " << msg << "\n";
  cerr.flush();
#else
  if (isInteractive && frame)
    (*frame->textWindow) << "Error: " << msg << "\n";
  else
#ifdef __UNIX__
  {
    cerr << "Error: " << msg << "\n";
    cerr.flush();
  }
#endif
#ifdef __WXMSW__
    wxError(msg);
#endif
  Tex2RTFYield(TRUE);
#endif // NO_GUI
}

void OnInform(char *msg)
{
#ifdef NO_GUI
  cout << msg << "\n";
  cout.flush();
#else
  if (isInteractive && frame)
    (*frame->textWindow) << msg << "\n";
  else
#ifdef __WXMSW__
  {
    cout << msg << "\n";
    cout.flush();
  }
#endif
#ifdef __WXMSW__
    {}
#endif
  if (isInteractive)
  {
    Tex2RTFYield(TRUE);
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
      break;
    }
    case TEX_XLP:
    {
      return XLPOnArgument(macroId, arg_no, start);
      break;
    }
    case TEX_HTML:
    {
      return HTMLOnArgument(macroId, arg_no, start);
      break;
    }
  }
  return TRUE;
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
  if (topic == "TEX2RTF")
  {
    if (!ipc_buffer)
      ipc_buffer = new char[1000];

    return new Tex2RTFConnection(ipc_buffer, 4000);
  }
  else
    return NULL;
}

 /*
  * Connection
  */

Tex2RTFConnection::Tex2RTFConnection(char *buf, int size):wxDDEConnection(buf, size)
{
}

Tex2RTFConnection::~Tex2RTFConnection(void)
{
}

bool SplitCommand(char *data, char *firstArg, char *secondArg)
{
  firstArg[0] = 0;
  secondArg[0] = 0;
  int i = 0;
  int len = strlen(data);
  bool stop = FALSE;
  // Find first argument (command name)
  while (!stop)
  {
    if (data[i] == ' ' || data[i] == 0)
      stop = TRUE;
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
  return TRUE;
}

bool Tex2RTFConnection::OnExecute(const wxString& topic, char *data, int size, int format)
{
  strcpy(Tex2RTFLastStatus, "OK");

  char firstArg[50];
  char secondArg[300];
  if (SplitCommand(data, firstArg, secondArg))
  {
    bool hasArg = (strlen(secondArg) > 0);
    if (strcmp(firstArg, "INPUT") == 0 && hasArg)
    {
      if (InputFile) delete[] InputFile;
      InputFile = copystring(secondArg);
      if (frame)
      {
        char buf[100];
        wxString str = wxFileNameFromPath(InputFile);
        sprintf(buf, "Tex2RTF [%s]", (const char*) str);
        frame->SetTitle(buf);
      }
    }
    else if (strcmp(firstArg, "OUTPUT") == 0 && hasArg)
    {
      if (OutputFile) delete[] OutputFile;
      OutputFile = copystring(secondArg);
    }
    else if (strcmp(firstArg, "GO") == 0)
    {
      strcpy(Tex2RTFLastStatus, "WORKING");
      if (!Go())
        strcpy(Tex2RTFLastStatus, "CONVERSION ERROR");
      else
        strcpy(Tex2RTFLastStatus, "OK");
    }
    else if (strcmp(firstArg, "EXIT") == 0)
    {
      if (frame) frame->Close();
    }
    else if (strcmp(firstArg, "MINIMIZE") == 0 || strcmp(firstArg, "ICONIZE") == 0)
    {
      if (frame)
        frame->Iconize(TRUE);
    }
    else if (strcmp(firstArg, "SHOW") == 0 || strcmp(firstArg, "RESTORE") == 0)
    {
      if (frame)
      {
        frame->Iconize(FALSE);
        frame->Show(TRUE);
      }
    }
    else
    {
      // Try for a setting
      strcpy(Tex2RTFLastStatus, RegisterSetting(firstArg, secondArg, FALSE));
#ifndef NO_GUI
      if (frame && strcmp(firstArg, "conversionMode") == 0)
      {
        char buf[100];
        strcpy(buf, "In ");

        if (winHelp && (convertMode == TEX_RTF))
          strcat(buf, "WinHelp RTF");
        else if (!winHelp && (convertMode == TEX_RTF))
          strcat(buf, "linear RTF");
        else if (convertMode == TEX_HTML) strcat(buf, "HTML");
        else if (convertMode == TEX_XLP) strcat(buf, "XLP");
          strcat(buf, " mode.");
        frame->SetStatusText(buf, 1);
      }
#endif
    }
  }
  return TRUE;
}

char *Tex2RTFConnection::OnRequest(const wxString& topic, const wxString& item, int *size, int format)
{
  return Tex2RTFLastStatus;
}

#endif

#ifndef NO_GUI
#ifndef __WXGTK__
//void wxObject::Dump(ostream& str)
//{
//  if (GetClassInfo() && GetClassInfo()->GetClassName())
//    str << GetClassInfo()->GetClassName();
//  else
//    str << "unknown object class";
//}
#endif
#endif

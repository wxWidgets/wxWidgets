/////////////////////////////////////////////////////////////////////////////
// Name:        tex2any.cpp
// Purpose:     Utilities for Latex conversion.
// Author:      Julian Smart
// Modified by: Wlodzimierz ABX Skiba 2003/2004 Unicode support
//              Ron Lee
// Created:     01/01/99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif

#include <ctype.h>
#include "tex2any.h"
#include <stdlib.h>
#include <time.h>

static inline wxChar* copystring(const wxChar* s)
    { return wxStrcpy(new wxChar[wxStrlen(s) + 1], s); }

/*
 * Variables accessible from clients
 *
 */

TexChunk *      DocumentTitle = NULL;
TexChunk *      DocumentAuthor = NULL;
TexChunk *      DocumentDate = NULL;

// Header/footers/pagestyle
TexChunk *      LeftHeaderEven = NULL;
TexChunk *      LeftFooterEven = NULL;
TexChunk *      CentreHeaderEven = NULL;
TexChunk *      CentreFooterEven = NULL;
TexChunk *      RightHeaderEven = NULL;
TexChunk *      RightFooterEven = NULL;
TexChunk *      LeftHeaderOdd = NULL;
TexChunk *      LeftFooterOdd = NULL;
TexChunk *      CentreHeaderOdd = NULL;
TexChunk *      CentreFooterOdd = NULL;
TexChunk *      RightHeaderOdd = NULL;
TexChunk *      RightFooterOdd = NULL;
wxChar *        PageStyle = copystring(_T("plain"));

int             DocumentStyle = LATEX_REPORT;
int             MinorDocumentStyle = 0;
wxPathList      TexPathList;
wxChar *        BibliographyStyleString = copystring(_T("plain"));
wxChar *        DocumentStyleString = copystring(_T("report"));
wxChar *        MinorDocumentStyleString = NULL;
int             ParSkip = 0;
int             ParIndent = 0;

int             normalFont = 10;
int             smallFont = 8;
int             tinyFont = 6;
int             largeFont1 = 12;
int             LargeFont2 = 14;
int             LARGEFont3 = 18;
int             hugeFont1 = 20;
int             HugeFont2 = 24;
int             HUGEFont3 = 28;

// All of these tokens MUST be found on a line by themselves (no other
// text) and must start at the first character of the line, or tex2rtf
// will fail to process them correctly (a limitation of tex2rtf, not TeX)
static const wxString syntaxTokens[] =
{ _T("\\begin{verbatim}"),
  _T("\\begin{toocomplex}"),
  _T("\\end{verbatim}"),
  _T("\\end{toocomplex}"),
  _T("\\verb"),
  _T("\\begin{comment}"),
  _T("\\end{comment}"),
  _T("\\verbatiminput"),
//  _T("\\par"),
  _T("\\input"),
  _T("\\helpinput"),
  _T("\\include"),
  wxEmptyString
};


/*
 * USER-ADJUSTABLE SETTINGS
 *
 */

// Section font sizes
int             chapterFont =    12; // LARGEFont3;
int             sectionFont =    12; // LargeFont2;
int             subsectionFont = 12; // largeFont1;
int             titleFont = LARGEFont3;
int             authorFont = LargeFont2;
int             mirrorMargins = true;
bool            winHelp = false;  // Output in Windows Help format if true, linear otherwise
bool            isInteractive = false;
bool            runTwice = false;
int             convertMode = TEX_RTF;
bool            checkCurlyBraces = false;
bool            checkSyntax = false;
bool            headerRule = false;
bool            footerRule = false;
bool            compatibilityMode = false; // If true, maximum Latex compatibility
                                // (Quality of RTF generation deteriorate)
bool            generateHPJ; // Generate WinHelp Help Project file
wxChar         *winHelpTitle = NULL; // Windows Help title
int             defaultTableColumnWidth = 2000;

int             labelIndentTab = 18;  // From left indent to item label (points)
int             itemIndentTab = 40;   // From left indent to item (points)

bool            useUpButton = true;
int             htmlBrowseButtons = HTML_BUTTONS_TEXT;

bool            truncateFilenames = false; // Truncate for DOS
int             winHelpVersion = 3; // WinHelp Version (3 for Windows 3.1, 4 for Win95)
bool            winHelpContents = false; // Generate .cnt file for WinHelp 4
bool            htmlIndex = false; // Generate .htx file for HTML
bool            htmlFrameContents = false; // Use frames for HTML contents page
wxChar         *htmlStylesheet = NULL; // Use this CSS stylesheet for HTML pages
bool            useHeadingStyles = true; // Insert \s1, s2 etc.
bool            useWord = true; // Insert proper Word table of contents, etc etc
int             contentsDepth = 4; // Depth of Word table of contents
bool            indexSubsections = true; // Index subsections in linear RTF
// Linear RTF method of including bitmaps. Can be "includepicture", "hex"
wxChar         *bitmapMethod = copystring(_T("includepicture"));
bool            upperCaseNames = false;
// HTML background and text colours
wxChar         *backgroundImageString = NULL;
wxChar         *backgroundColourString = copystring(_T("255;255;255"));
wxChar         *textColourString = NULL;
wxChar         *linkColourString = NULL;
wxChar         *followedLinkColourString = NULL;
bool            combineSubSections = false;
bool            htmlWorkshopFiles = false;
bool            ignoreBadRefs = false;
wxChar         *htmlFaceName = NULL;

extern int passNumber;

extern wxHashTable TexReferences;

/*
 * International support
 */

// Names to help with internationalisation
wxChar *ContentsNameString = copystring(_T("Contents"));
wxChar *AbstractNameString = copystring(_T("Abstract"));
wxChar *GlossaryNameString = copystring(_T("Glossary"));
wxChar *ReferencesNameString = copystring(_T("References"));
wxChar *FiguresNameString = copystring(_T("List of Figures"));
wxChar *TablesNameString = copystring(_T("List of Tables"));
wxChar *FigureNameString = copystring(_T("Figure"));
wxChar *TableNameString = copystring(_T("Table"));
wxChar *IndexNameString = copystring(_T("Index"));
wxChar *ChapterNameString = copystring(_T("chapter"));
wxChar *SectionNameString = copystring(_T("section"));
wxChar *SubsectionNameString = copystring(_T("subsection"));
wxChar *SubsubsectionNameString = copystring(_T("subsubsection"));
wxChar *UpNameString = copystring(_T("Up"));

/*
 * Section numbering
 *
 */

int             chapterNo = 0;
int             sectionNo = 0;
int             subsectionNo = 0;
int             subsubsectionNo = 0;
int             figureNo = 0;
int             tableNo = 0;

/*
 * Other variables
 *
 */

FILE *CurrentOutput1 = NULL;
FILE *CurrentOutput2 = NULL;
FILE *Inputs[15];
unsigned long LineNumbers[15];
wxChar *FileNames[15];
int CurrentInputIndex = 0;

wxChar *TexFileRoot = NULL;
wxChar *TexBibName = NULL;         // Bibliography output file name
wxChar *TexTmpBibName = NULL;      // Temporary bibliography output file name
bool isSync = false;             // If true, should not yield to other processes.
bool stopRunning = false;        // If true, should abort.

static int currentColumn = 0;
wxChar *currentArgData = NULL;
bool haveArgData = false; // If true, we're simulating the data.
TexChunk *currentArgument = NULL;
TexChunk *nextChunk = NULL;
bool isArgOptional = false;
int noArgs = 0;

TexChunk *TopLevel = NULL;
// wxList MacroDefs(wxKEY_STRING);
wxHashTable MacroDefs(wxKEY_STRING);
wxStringList IgnorableInputFiles; // Ignorable \input files, e.g. psbox.tex
wxChar *BigBuffer = NULL;  // For reading in large chunks of text
TexMacroDef *SoloBlockDef = NULL;
TexMacroDef *VerbatimMacroDef = NULL;

#define IncrementLineNumber() LineNumbers[CurrentInputIndex] ++


TexRef::TexRef(const wxChar *label, const wxChar *file,
               const wxChar *section, const wxChar *sectionN)
{
    refLabel = copystring(label);
    refFile = file ? copystring(file) : (wxChar*) NULL;
    sectionNumber = section ? copystring(section) : copystring(_T("??"));
    sectionName = sectionN ? copystring(sectionN) : copystring(_T("??"));
}

TexRef::~TexRef(void)
{
    delete [] refLabel;      refLabel = NULL;
    delete [] refFile;       refFile = NULL;
    delete [] sectionNumber; sectionNumber = NULL;
    delete [] sectionName;   sectionName = NULL;
}


CustomMacro::~CustomMacro()
{
    if (macroName)
        delete [] macroName;
    if (macroBody)
        delete [] macroBody;
}

void TexOutput(const wxChar *s, bool ordinaryText)
{
  int len = wxStrlen(s);

  // Update current column, but only if we're guaranteed to
  // be ordinary text (not mark-up stuff)
  int i;
  if (ordinaryText)
    for (i = 0; i < len; i++)
    {
      if (s[i] == 13 || s[i] == 10)
        currentColumn = 0;
      else
        currentColumn ++;
    }

  if (CurrentOutput1)
    wxFprintf(CurrentOutput1, _T("%s"), s);
  if (CurrentOutput2)
    wxFprintf(CurrentOutput2, _T("%s"), s);
}

/*
 * Try to find a Latex macro, in one of the following forms:
 * (1) \begin{} ... \end{}
 * (2) \macroname{arg1}...{argn}
 * (3) {\bf arg1}
 */

void ForbidWarning(TexMacroDef *def)
{
  wxString informBuf;
  switch (def->forbidden)
  {
    case FORBID_WARN:
    {
      informBuf.Printf(_T("Warning: it is recommended that command %s is not used."), def->name);
      OnInform((const wxChar *)informBuf.c_str());
      break;
    }
    case FORBID_ABSOLUTELY:
    {
      informBuf.Printf(_T("Error: command %s cannot be used and will lead to errors."), def->name);
      OnInform((const wxChar *)informBuf.c_str());
      break;
    }
    default:
      break;
  }
}

TexMacroDef *MatchMacro(wxChar *buffer, int *pos, wxChar **env, bool *parseToBrace)
{
    *parseToBrace = true;
    int i = (*pos);
    TexMacroDef *def = NULL;
    wxChar macroBuf[40];

    // First, try to find begin{thing}
    if (wxStrncmp(buffer+i, _T("begin{"), 6) == 0)
    {
        i += 6;

        int j = i;
        while ((isalpha(buffer[j]) || buffer[j] == '*') && ((j - i) < 39))
        {
              macroBuf[j-i] = buffer[j];
              j ++;
        }
        macroBuf[j-i] = 0;
        def = (TexMacroDef *)MacroDefs.Get(macroBuf);

        if (def)
        {
            *pos = j + 1;  // BUGBUG Should this be + 1???
            *env = def->name;
            ForbidWarning(def);
            return def;
        }
        else
        {
            return NULL;
        }
    }

    // Failed, so try to find macro from definition list
    int j = i;

    // First try getting a one-character macro, but ONLY
    // if these TWO characters are not both alphabetical (could
    // be a longer macro)
    if (!(isalpha(buffer[i]) && isalpha(buffer[i+1])))
    {
        macroBuf[0] = buffer[i];
        macroBuf[1] = 0;

        def = (TexMacroDef *)MacroDefs.Get(macroBuf);
        if (def) j ++;
    }

    if (!def)
    {
        while ((isalpha(buffer[j]) || buffer[j] == '*') && ((j - i) < 39))
        {
            macroBuf[j-i] = buffer[j];
            j ++;
        }
        macroBuf[j-i] = 0;
        def = (TexMacroDef *)MacroDefs.Get(macroBuf);
    }

    if (def)
    {
        i = j;

        // We want to check whether this is a space-consuming macro
        // (e.g. {\bf word})
        // No brace, e.g. \input thing.tex instead of \input{thing};
        // or a numeric argument, such as \parindent0pt
        if ((def->no_args > 0) && ((buffer[i] == 32) || (buffer[i] == '=') || (isdigit(buffer[i]))))
        {
            if ((buffer[i] == 32) || (buffer[i] == '='))
            i ++;

            *parseToBrace = false;
        }
        *pos = i;
        ForbidWarning(def);
        return def;
    }
    return NULL;
}

void EatWhiteSpace(wxChar *buffer, int *pos)
{
  int len = wxStrlen(buffer);
  int j = *pos;
  bool keepGoing = true;
  bool moreLines = true;
  while ((j < len) && keepGoing &&
         (buffer[j] == 10 || buffer[j] == 13 || buffer[j] == ' ' || buffer[j] == 9))
  {
    j ++;
    if (j >= len)
    {
      if (moreLines)
      {
        moreLines = read_a_line(buffer);
        len = wxStrlen(buffer);
        j = 0;
      }
      else
        keepGoing = false;
    }
  }
  *pos = j;
}

bool FindEndEnvironment(wxChar *buffer, int *pos, wxChar *env)
{
  int i = (*pos);

  // Try to find end{thing}
  if ((wxStrncmp(buffer+i, _T("end{"), 4) == 0) &&
      (wxStrncmp(buffer+i+4, env, wxStrlen(env)) == 0))
  {
    *pos = i + 5 + wxStrlen(env);
    return true;
  }
  else return false;
}

bool readingVerbatim = false;
bool readInVerbatim = false;  // Within a verbatim, but not nec. verbatiminput

// Switched this off because e.g. \verb${$ causes it to fail. There is no
// detection of \verb yet.
// #define CHECK_BRACES 1

unsigned long leftCurly = 0;
unsigned long rightCurly = 0;
static wxString currentFileName = wxEmptyString;

bool read_a_line(wxChar *buf)
{
  if (CurrentInputIndex < 0)
  {
    buf[0] = 0;
    return false;
  }

  int ch = -2;
  unsigned long bufIndex = 0;
  buf[0] = 0;
  int lastChar;

  while (ch != EOF && ch != 10)
  {
    if (bufIndex >= MAX_LINE_BUFFER_SIZE)
    {
       wxString errBuf;
       errBuf.Printf(_T("Line %lu of file %s is too long.  Lines can be no longer than %lu characters.  Truncated."),
           LineNumbers[CurrentInputIndex], (const wxChar*) currentFileName.c_str(), MAX_LINE_BUFFER_SIZE);
       OnError((wxChar *)errBuf.c_str());
       return false;
    }

    if (((bufIndex == 14) && (wxStrncmp(buf, _T("\\end{verbatim}"), 14) == 0)) ||
         ((bufIndex == 16) && (wxStrncmp(buf, _T("\\end{toocomplex}"), 16) == 0)))
      readInVerbatim = false;

    lastChar = ch;
    ch = getc(Inputs[CurrentInputIndex]);

    if (checkCurlyBraces)
    {
        if (ch == '{' && !readInVerbatim && lastChar != _T('\\'))
           leftCurly++;
        if (ch == '}' && !readInVerbatim && lastChar != _T('\\'))
        {
           rightCurly++;
           if (rightCurly > leftCurly)
           {
               wxString errBuf;
               errBuf.Printf(_T("An extra right Curly brace ('}') was detected at line %lu inside file %s"), LineNumbers[CurrentInputIndex], (const wxChar*) currentFileName.c_str());
               OnError((wxChar *)errBuf.c_str());

               // Reduce the count of right Curly braces, so the mismatched count
               // isn't reported on every line that has a '}' after the first mismatch
               rightCurly--;
           }
        }
    }

    if (ch != EOF)
    {
      // Check for 2 consecutive newlines and replace with \par
      if (ch == 10 && !readInVerbatim)
      {
        int ch1 = getc(Inputs[CurrentInputIndex]);
        if ((ch1 == 10) || (ch1 == 13))
        {
          // Eliminate newline (10) following DOS linefeed
          if (ch1 == 13)
            getc(Inputs[CurrentInputIndex]);
          buf[bufIndex] = 0;
          IncrementLineNumber();
//          wxStrcat(buf, "\\par\n");
//          i += 6;
          if (bufIndex+5 >= MAX_LINE_BUFFER_SIZE)
          {
             wxString errBuf;
             errBuf.Printf(_T("Line %lu of file %s is too long.  Lines can be no longer than %lu characters.  Truncated."),
                 LineNumbers[CurrentInputIndex], (const wxChar*) currentFileName.c_str(),MAX_LINE_BUFFER_SIZE);
             OnError((wxChar *)errBuf.c_str());
             return false;
          }
          wxStrcat(buf, _T("\\par"));
          bufIndex += 5;

        }
        else
        {
          ungetc(ch1, Inputs[CurrentInputIndex]);
          if (bufIndex >= MAX_LINE_BUFFER_SIZE)
          {
             wxString errBuf;
             errBuf.Printf(_T("Line %lu of file %s is too long.  Lines can be no longer than %lu characters.  Truncated."),
                 LineNumbers[CurrentInputIndex], (const wxChar*) currentFileName.c_str(),MAX_LINE_BUFFER_SIZE);
             OnError((wxChar *)errBuf.c_str());
             return false;
          }

          buf[bufIndex] = (wxChar)ch;
          bufIndex ++;
        }
      }
      else
      {

        // Convert embedded characters to RTF equivalents
        switch(ch)
        {
        case 0xf6: // ö
        case 0xe4: // ü
        case 0xfc: // ü
        case 0xd6: // Ö
        case 0xc4: // Ä
        case 0xdc: // Ü
                if (bufIndex+5 >= MAX_LINE_BUFFER_SIZE)
                {
                   wxString errBuf;
                   errBuf.Printf(_T("Line %lu of file %s is too long.  Lines can be no longer than %lu characters.  Truncated."),
                       LineNumbers[CurrentInputIndex], (const wxChar*) currentFileName.c_str(),MAX_LINE_BUFFER_SIZE);
                   OnError((wxChar *)errBuf.c_str());
                   return false;
                }
                buf[bufIndex++]='\\';
                buf[bufIndex++]='"';
                buf[bufIndex++]='{';
                switch(ch)
                {
                    case 0xf6:buf[bufIndex++]='o';break; // ö
                    case 0xe4:buf[bufIndex++]='a';break; // ä
                    case 0xfc:buf[bufIndex++]='u';break; // ü
                    case 0xd6:buf[bufIndex++]='O';break; // Ö
                    case 0xc4:buf[bufIndex++]='A';break; // Ä
                    case 0xdc:buf[bufIndex++]='U';break; // Ü
                }
                buf[bufIndex++]='}';
                break;
        case 0xdf: // ß
            if (bufIndex+5 >= MAX_LINE_BUFFER_SIZE)
            {
              wxString errBuf;
              errBuf.Printf(_T("Line %lu of file %s is too long.  Lines can be no longer than %lu characters.  Truncated."),
                  LineNumbers[CurrentInputIndex], (const wxChar*) currentFileName.c_str(),MAX_LINE_BUFFER_SIZE);
              OnError((wxChar *)errBuf.c_str());
              return false;
            }
            buf[bufIndex++]='\\';
            buf[bufIndex++]='s';
            buf[bufIndex++]='s';
            buf[bufIndex++]='\\';
            buf[bufIndex++]='/';
            break;
        default:
            if (bufIndex >= MAX_LINE_BUFFER_SIZE)
            {
              wxString errBuf;
              errBuf.Printf(_T("Line %lu of file %s is too long.  Lines can be no longer than %lu characters.  Truncated."),
                  LineNumbers[CurrentInputIndex], (const wxChar*) currentFileName.c_str(),MAX_LINE_BUFFER_SIZE);
              OnError((wxChar *)errBuf.c_str());
              return false;
            }
            // If the current character read in is a '_', we need to check
            // whether there should be a '\' before it or not
            if (ch != '_')
            {
                buf[bufIndex++] = (wxChar)ch;
                break;
            }

            if (checkSyntax)
            {
                if (readInVerbatim)
                {
                    // There should NOT be a '\' before the '_'
                    if ((bufIndex > 0 && (buf[bufIndex-1] == '\\')) && (buf[0] != '%'))
                    {
//                        wxString errBuf;
//                        errBuf.Printf(_T("An underscore ('_') was detected at line %lu inside file %s that should NOT have a '\\' before it."),
//                            LineNumbers[CurrentInputIndex], (const wxChar*) currentFileName.c_str());
//                        OnError((wxChar *)errBuf.c_str());
                    }
                }
                else
                {
                    // There should be a '\' before the '_'
                    if (bufIndex == 0)
                    {
                        wxString errBuf;
                        errBuf.Printf(_T("An underscore ('_') was detected at line %lu inside file %s that may need a '\\' before it."),
                            LineNumbers[CurrentInputIndex], (const wxChar*) currentFileName.c_str());
                        OnError((wxChar *)errBuf.c_str());
                    }
                    else if ((buf[bufIndex-1] != '\\') && (buf[0] != '%') &&  // If it is a comment line, then no warnings
                        (wxStrncmp(buf, _T("\\input"), 6))) // do not report filenames that have underscores in them
                    {
                        wxString errBuf;
                        errBuf.Printf(_T("An underscore ('_') was detected at line %lu inside file %s that may need a '\\' before it."),
                            LineNumbers[CurrentInputIndex], (const wxChar*) currentFileName.c_str());
                        OnError((wxChar *)errBuf.c_str());
                    }
                }
            }
            buf[bufIndex++] = (wxChar)ch;
            break;
        }  // switch
      }  // else
    }
    else
    {
      buf[bufIndex] = 0;
      fclose(Inputs[CurrentInputIndex]);
      Inputs[CurrentInputIndex] = NULL;
      if (CurrentInputIndex > 0)
         ch = ' '; // No real end of file
      CurrentInputIndex --;

      if (checkCurlyBraces)
      {
          if (leftCurly != rightCurly)
          {
            wxString errBuf;
            errBuf.Printf(_T("Curly braces do not match inside file %s\n%lu opens, %lu closes"),
                          (const wxChar*) currentFileName.c_str(),leftCurly,rightCurly);
            OnError((wxChar *)errBuf.c_str());
          }
          leftCurly = 0;
          rightCurly = 0;
      }

      if (readingVerbatim)
      {
        readingVerbatim = false;
        readInVerbatim = false;
        wxStrcat(buf, _T("\\end{verbatim}\n"));
        return false;
      }
    }
    if (ch == 10)
      IncrementLineNumber();
  }
  buf[bufIndex] = 0;

  // Strip out comment environment
  if (wxStrncmp(buf, _T("\\begin{comment}"), 15) == 0)
  {
    while (wxStrncmp(buf, _T("\\end{comment}"), 13) != 0)
      read_a_line(buf);
    return read_a_line(buf);
  }
  // Read a verbatim input file as if it were a verbatim environment
  else if (wxStrncmp(buf, _T("\\verbatiminput"), 14) == 0)
  {
    int wordLen = 14;
    wxChar *fileName = buf + wordLen + 1;

    int j = bufIndex - 1;
    buf[j] = 0;

    // thing}\par -- eliminate the \par!
    if (wxStrncmp((buf + wxStrlen(buf)-5), _T("\\par"), 4) == 0)
    {
      j -= 5;
      buf[j] = 0;
    }

    if (buf[j-1] == '}')
        buf[j-1] = 0; // Ignore final brace

    wxString actualFile = TexPathList.FindValidPath(fileName);
    currentFileName = actualFile;
    if (actualFile.empty())
    {
      wxString errBuf;
      errBuf.Printf(_T("Could not find file: %s"),fileName);
      OnError((wxChar *)errBuf.c_str());
    }
    else
    {
      wxString informStr;
      informStr.Printf(_T("Processing: %s"),actualFile.c_str());
      OnInform((wxChar *)informStr.c_str());
      CurrentInputIndex ++;

      Inputs[CurrentInputIndex] = wxFopen(actualFile, _T("r"));
      LineNumbers[CurrentInputIndex] = 1;
      if (FileNames[CurrentInputIndex])
        delete[] FileNames[CurrentInputIndex];
      FileNames[CurrentInputIndex] = copystring(actualFile);

      if (!Inputs[CurrentInputIndex])
      {
        CurrentInputIndex --;
        OnError(_T("Could not open verbatiminput file."));
      }
      else
      {
        readingVerbatim = true;
        readInVerbatim = true;
        wxStrcpy(buf, _T("\\begin{verbatim}\n"));
        return false;
      }
    }
    return false;
  }
  else if (wxStrncmp(buf, _T("\\input"), 6) == 0 || wxStrncmp(buf, _T("\\helpinput"), 10) == 0 ||
      wxStrncmp(buf, _T("\\include"), 8) == 0)
  {
    int wordLen;
    if (wxStrncmp(buf, _T("\\input"), 6) == 0)
      wordLen = 6;
    else
    if (wxStrncmp(buf, _T("\\include"), 8) == 0)
      wordLen = 8;
    else
      wordLen = 10;

    wxChar *fileName = buf + wordLen + 1;

    int j = bufIndex - 1;
    buf[j] = 0;

    // \input{thing}\par -- eliminate the \par!
//    if (wxStrncmp((buf + wxStrlen(buf)-5), "\\par", 4) == 0)
    if (wxStrncmp((buf + wxStrlen(buf)-4), _T("\\par"), 4) == 0) // Bug fix 8/2/95 Ulrich Leodolter
    {
//      j -= 5;
      j -= 4; // Ditto
      buf[j] = 0;
    }

    if (buf[j-1] == _T('}'))
        buf[j-1] = 0; // Ignore final brace

    // Remove backslashes from name
    wxString fileNameStr(fileName);
    fileNameStr.Replace(_T("\\"), _T(""));

    // Ignore some types of input files (e.g. macro definition files)
    wxChar *fileOnly = wxFileNameFromPath((wxChar*) (const wxChar*) fileNameStr);
    currentFileName = fileOnly;
    if (IgnorableInputFiles.Member(fileOnly))
      return read_a_line(buf);

    wxString actualFile = TexPathList.FindValidPath(fileNameStr);
    if (actualFile.empty())
    {
      wxChar buf2[400];
      wxSnprintf(buf2, sizeof(buf2), _T("%s.tex"), fileNameStr.c_str());
      actualFile = TexPathList.FindValidPath(buf2);
    }
    currentFileName = actualFile;

    if (actualFile.empty())
    {
      wxString errBuf;
      errBuf.Printf(_T("Could not find file: %s"),fileName);
      OnError((wxChar *)errBuf.c_str());
    }
    else
    {
      // Ensure that if this file includes another,
      // then we look in the same directory as this one.
      TexPathList.EnsureFileAccessible(actualFile);

      wxString informStr;
      informStr.Printf(_T("Processing: %s"),actualFile.c_str());
      OnInform((wxChar *)informStr.c_str());
      CurrentInputIndex ++;

      Inputs[CurrentInputIndex] = wxFopen(actualFile, _T("r"));
      LineNumbers[CurrentInputIndex] = 1;
      if (FileNames[CurrentInputIndex])
        delete[] FileNames[CurrentInputIndex];
      FileNames[CurrentInputIndex] = copystring(actualFile);

      if (!Inputs[CurrentInputIndex])
      {
        wxString errBuf;
        errBuf.Printf(_T("Could not open include file %s"), (const wxChar*) actualFile);
        CurrentInputIndex --;
        OnError((wxChar *)errBuf.c_str());
      }
    }
    bool succ = read_a_line(buf);
    return succ;
  }

  if (checkSyntax)
  {
      wxString bufStr = buf;
      for (int index=0; !syntaxTokens[index].empty(); index++)
      {
          size_t pos = bufStr.find(syntaxTokens[index]);
          if (pos != wxString::npos && pos != 0)
          {
              size_t commentStart = bufStr.find(_T("%"));
              if (commentStart == wxString::npos || commentStart > pos)
              {
                  wxString errBuf;
                  if (syntaxTokens[index] == _T("\\verb"))
                  {
                      errBuf.Printf(_T("'%s$....$' was detected at line %lu inside file %s.  Please replace this form with \\tt{....}"),
                                    syntaxTokens[index].c_str(),
                                    LineNumbers[CurrentInputIndex],
                                    currentFileName.c_str());
                  }
                  else
                  {
                      errBuf.Printf(_T("'%s' was detected at line %lu inside file %s that is not the only text on the line, starting at column one."),
                                    syntaxTokens[index].c_str(),
                                    LineNumbers[CurrentInputIndex],
                                    currentFileName.c_str());
                  }
                  OnError((wxChar *)errBuf.c_str());
              }
          }
      }
  }  // checkSyntax

  if (wxStrncmp(buf, _T("\\begin{verbatim}"), 16) == 0 ||
      wxStrncmp(buf, _T("\\begin{toocomplex}"), 18) == 0)
    readInVerbatim = true;
  else if (wxStrncmp(buf, _T("\\end{verbatim}"), 14) == 0 ||
           wxStrncmp(buf, _T("\\end{toocomplex}"), 16) == 0)
    readInVerbatim = false;

  if (checkCurlyBraces)
  {
      if (ch == EOF && leftCurly != rightCurly)
      {
        wxString errBuf;
        errBuf.Printf(_T("Curly braces do not match inside file %s\n%lu opens, %lu closes"),
            (const wxChar*) currentFileName.c_str(),leftCurly,rightCurly);
        OnError((wxChar *)errBuf.c_str());
      }
  }

  return (ch == EOF);
}  // read_a_line

/*
 * Parse newcommand
 *
 */

bool ParseNewCommand(wxChar *buffer, int *pos)
{
  if ((wxStrncmp((buffer+(*pos)), _T("newcommand"), 10) == 0) ||
      (wxStrncmp((buffer+(*pos)), _T("renewcommand"), 12) == 0))
  {
    if (wxStrncmp((buffer+(*pos)), _T("newcommand"), 10) == 0)
      *pos = *pos + 12;
    else
      *pos = *pos + 14;

    wxChar commandName[100];
    wxChar commandValue[1000];
    int noArgs = 0;
    int i = 0;
    while (buffer[*pos] != _T('}') && (buffer[*pos] != 0))
    {
      commandName[i] = buffer[*pos];
      *pos += 1;
      i ++;
    }
    commandName[i] = 0;
    i = 0;
    *pos += 1;
    if (buffer[*pos] == _T('['))
    {
      *pos += 1;
      noArgs = (int)(buffer[*pos]) - 48;
      *pos += 2; // read past argument and '['
    }
    bool end = false;
    int braceCount = 0;
    while (!end)
    {
      wxChar ch = buffer[*pos];
      if (ch == _T('{'))
        braceCount ++;
      else if (ch == _T('}'))
      {
        braceCount --;
        if (braceCount == 0)
          end = true;
      }
      else if (ch == 0)
      {
        end = !read_a_line(buffer);
        wxUnusedVar(end);
        *pos = 0;
        break;
      }
      commandValue[i] = ch;
      i ++;
      *pos += 1;
    }
    commandValue[i] = 0;

    CustomMacro *macro = new CustomMacro(commandName, noArgs, NULL);
    if (wxStrlen(commandValue) > 0)
      macro->macroBody = copystring(commandValue);
    if (!CustomMacroList.Find(commandName))
    {
      CustomMacroList.Append(commandName, macro);
      AddMacroDef(ltCUSTOM_MACRO, commandName, noArgs);
    }
    return true;
  }
  else return false;
}

void MacroError(wxChar *buffer)
{
  wxString errBuf;
  wxChar macroBuf[200];
  macroBuf[0] = '\\';
  int i = 1;
  wxChar ch;
  while (((ch = buffer[i-1]) != '\n') && (ch != 0))
  {
    macroBuf[i] = ch;
    i ++;
  }
  macroBuf[i] = 0;
  if (i > 20)
    macroBuf[20] = 0;

  errBuf.Printf(_T("Could not find macro: %s at line %d, file %s"),
             macroBuf, (int)(LineNumbers[CurrentInputIndex]-1), FileNames[CurrentInputIndex]);
  OnError((wxChar *)errBuf.c_str());

  if (wxStrcmp(macroBuf,_T("\\end{document}")) == 0)
  {
      OnInform( _T("Halted build due to unrecoverable error.") );
      stopRunning = true;
  }
}

/*
 * Parse an argument.
 * 'environment' specifies the name of the macro IFF if we're looking for the end
 * of an environment, e.g. \end{itemize}. Otherwise it's NULL.
 * 'parseToBrace' is true if the argument should extend to the next right brace,
 * e.g. in {\bf an argument} as opposed to \vskip 30pt
 *
 */
int ParseArg(TexChunk *thisArg, wxList& children, wxChar *buffer, int pos, wxChar *environment, bool parseToBrace, TexChunk *customMacroArgs)
{
  Tex2RTFYield();
  if (stopRunning) return pos;

  bool eof = false;
  BigBuffer[0] = 0;
  int buf_ptr = 0;
  int len;

/*

  // Consume leading brace or square bracket, but ONLY if not following
  // a space, because this could be e.g. {\large {\bf thing}} where {\bf thing}
  // is the argument of \large AS WELL as being a block in its
  // own right.
  if (!environment)
  {
    if ((pos > 0) && (buffer[pos-1] != ' ') && buffer[pos] == '{')
      pos ++;
    else

    if ((pos > 0) && (buffer[pos-1] != ' ') && (buffer[pos] == '[' || buffer[pos] == '('))
    {
      isOptional = true;
      pos ++;
    }
    else if ((pos > 1) && (buffer[pos-1] != ' ') && (buffer[pos+1] == '[' || buffer[pos+1] == '('))
    {
      isOptional = true;
      pos += 2;
    }
  }
*/

  // If not parsing to brace, just read the next word
  // (e.g. \vskip 20pt)
  if (!parseToBrace)
  {
    int ch = buffer[pos];
    while (!eof && ch != 13 && ch != 32 && ch != 10 &&
           ch != 0 && ch != '{')
    {
      BigBuffer[buf_ptr] = (wxChar)ch;
      buf_ptr ++;
      pos ++;
      ch = buffer[pos];
    }
    if (buf_ptr > 0)
    {
      TexChunk *chunk = new TexChunk(CHUNK_TYPE_STRING);
      BigBuffer[buf_ptr] = 0;
      chunk->value = copystring(BigBuffer);
      children.Append((wxObject *)chunk);
    }
    return pos;
  }

  while (!eof)
  {
    len = wxStrlen(buffer);
    if (pos >= len)
    {
      if (customMacroArgs) return 0;

      eof = read_a_line(buffer);
      pos = 0;
      // Check for verbatim (or toocomplex, which comes to the same thing)
      wxString bufStr = buffer;
//      if (bufStr.find("\\begin{verbatim}") != wxString::npos ||
//          bufStr.find("\\begin{toocomplex}") != wxString::npos)
      if (wxStrncmp(buffer, _T("\\begin{verbatim}"), 16) == 0 ||
          wxStrncmp(buffer, _T("\\begin{toocomplex}"), 18) == 0)
      {
        if (buf_ptr > 0)
        {
          TexChunk *chunk = new TexChunk(CHUNK_TYPE_STRING);
          BigBuffer[buf_ptr] = 0;
          chunk->value = copystring(BigBuffer);
          children.Append((wxObject *)chunk);
        }
        BigBuffer[0] = 0;
        buf_ptr = 0;

        eof = read_a_line(buffer);
        while (!eof && (wxStrncmp(buffer, _T("\\end{verbatim}"), 14) != 0) &&
                       (wxStrncmp(buffer, _T("\\end{toocomplex}"), 16) != 0)
               )
        {
          wxStrcat(BigBuffer, buffer);
          buf_ptr += wxStrlen(buffer);
          eof = read_a_line(buffer);
        }
        eof = read_a_line(buffer);
        buf_ptr = 0;

        TexChunk *chunk = new TexChunk(CHUNK_TYPE_MACRO, VerbatimMacroDef);
        chunk->no_args = 1;
        chunk->macroId = ltVERBATIM;
        TexChunk *arg = new TexChunk(CHUNK_TYPE_ARG, VerbatimMacroDef);
        arg->argn = 1;
        arg->macroId = ltVERBATIM;
        TexChunk *str = new TexChunk(CHUNK_TYPE_STRING);
        str->value = copystring(BigBuffer);

        children.Append((wxObject *)chunk);
        chunk->children.Append((wxObject *)arg);
        arg->children.Append((wxObject *)str);

        // Also want to include the following newline (is always a newline
        // after a verbatim): EXCEPT in HTML
        if (convertMode != TEX_HTML)
        {
          TexMacroDef *parDef = (TexMacroDef *)MacroDefs.Get(_T("\\"));
          TexChunk *parChunk = new TexChunk(CHUNK_TYPE_MACRO, parDef);
          parChunk->no_args = 0;
          parChunk->macroId = ltBACKSLASHCHAR;
          children.Append((wxObject *)parChunk);
        }
      }
    }

    wxChar wxCh = buffer[pos];
    // End of optional argument -- pretend it's right brace for simplicity
    if (thisArg->optional && (wxCh == _T(']')))
      wxCh = _T('}');

    switch (wxCh)
    {
      case 0:
      case _T('}'):  // End of argument
      {
        if (buf_ptr > 0)
        {
          TexChunk *chunk = new TexChunk(CHUNK_TYPE_STRING);
          BigBuffer[buf_ptr] = 0;
          chunk->value = copystring(BigBuffer);
          children.Append((wxObject *)chunk);
        }
        if (wxCh == _T('}')) pos ++;
        return pos;
      }
      case _T('\\'):
      {
        if (buf_ptr > 0)  // Finish off the string we've read so far
        {
          TexChunk *chunk = new TexChunk(CHUNK_TYPE_STRING);
          BigBuffer[buf_ptr] = 0;
          buf_ptr = 0;
          chunk->value = copystring(BigBuffer);
          children.Append((wxObject *)chunk);
        }
        pos ++;

        // Try matching \end{environment}
        if (environment && FindEndEnvironment(buffer, &pos, environment))
        {
          // Eliminate newline after an \end{} if possible
          if (buffer[pos] == 13)
          {
            pos ++;
            if (buffer[pos] == 10)
              pos ++;
          }
          return pos;
        }

        if (ParseNewCommand(buffer, &pos))
          break;

        if (wxStrncmp(buffer+pos, _T("special"), 7) == 0)
        {
          pos += 7;

          // Discard {
          pos ++;
          int noBraces = 1;

          wxTex2RTFBuffer[0] = 0;
          int i = 0;
          bool end = false;
          while (!end)
          {
            wxChar ch = buffer[pos];
            if (ch == _T('}'))
            {
              noBraces --;
              if (noBraces == 0)
              {
                wxTex2RTFBuffer[i] = 0;
                end = true;
              }
              else
              {
                wxTex2RTFBuffer[i] = _T('}');
                i ++;
              }
              pos ++;
            }
            else if (ch == _T('{'))
            {
              wxTex2RTFBuffer[i] = _T('{');
              i ++;
              pos ++;
            }
            else if (ch == _T('\\') && buffer[pos+1] == _T('}'))
            {
              wxTex2RTFBuffer[i] = _T('}');
              pos += 2;
              i++;
            }
            else if (ch == _T('\\') && buffer[pos+1] == _T('{'))
            {
              wxTex2RTFBuffer[i] = _T('{');
              pos += 2;
              i++;
            }
            else
            {
              wxTex2RTFBuffer[i] = ch;
              pos ++;
              i ++;
              if (ch == 0)
                end = true;
            }
          }
          TexChunk *chunk = new TexChunk(CHUNK_TYPE_MACRO);
          chunk->no_args = 1;
          chunk->macroId = ltSPECIAL;
          TexMacroDef *specialDef = (TexMacroDef *)MacroDefs.Get(_T("special"));
          chunk->def = specialDef;
          TexChunk *arg = new TexChunk(CHUNK_TYPE_ARG, specialDef);
          chunk->children.Append((wxObject *)arg);
          arg->argn = 1;
          arg->macroId = chunk->macroId;

          // The value in the first argument.
          TexChunk *argValue = new TexChunk(CHUNK_TYPE_STRING);
          arg->children.Append((wxObject *)argValue);
          argValue->argn = 1;
          argValue->value = copystring(wxTex2RTFBuffer);

          children.Append((wxObject *)chunk);
        }
        else if (wxStrncmp(buffer+pos, _T("verb"), 4) == 0)
        {
          pos += 4;
          if (buffer[pos] == _T('*'))
            pos ++;

          // Find the delimiter character
          wxChar ch = buffer[pos];
          pos ++;
          // Now at start of verbatim text
          int j = pos;
          while ((buffer[pos] != ch) && buffer[pos] != 0)
            pos ++;
          wxChar *val = new wxChar[pos - j + 1];
          int i;
          for (i = j; i < pos; i++)
          {
            val[i-j] = buffer[i];
          }
          val[i-j] = 0;

          pos ++;

          TexChunk *chunk = new TexChunk(CHUNK_TYPE_MACRO);
          chunk->no_args = 1;
          chunk->macroId = ltVERB;
          TexMacroDef *verbDef = (TexMacroDef *)MacroDefs.Get(_T("verb"));
          chunk->def = verbDef;
          TexChunk *arg = new TexChunk(CHUNK_TYPE_ARG, verbDef);
          chunk->children.Append((wxObject *)arg);
          arg->argn = 1;
          arg->macroId = chunk->macroId;

          // The value in the first argument.
          TexChunk *argValue = new TexChunk(CHUNK_TYPE_STRING);
          arg->children.Append((wxObject *)argValue);
          argValue->argn = 1;
          argValue->value = val;

          children.Append((wxObject *)chunk);
        }
        else
        {
          wxChar *env = NULL;
          bool tmpParseToBrace = true;
          TexMacroDef *def = MatchMacro(buffer, &pos, &env, &tmpParseToBrace);
          if (def)
          {
          CustomMacro *customMacro = FindCustomMacro(def->name);

          TexChunk *chunk = new TexChunk(CHUNK_TYPE_MACRO, def);

          chunk->no_args = def->no_args;
//          chunk->name = copystring(def->name);
          chunk->macroId = def->macroId;

          if  (!customMacro)
            children.Append((wxObject *)chunk);

          // Eliminate newline after a \begin{} or a \\ if possible
          if ((env || wxStrcmp(def->name, _T("\\")) == 0) && (buffer[pos] == 13))
          {
              pos ++;
              if (buffer[pos] == 10)
                pos ++;
          }

          pos = ParseMacroBody(def->name,
                               chunk, chunk->no_args,
                               buffer,
                               pos,
                               env,
                               tmpParseToBrace,
                               customMacroArgs);

          // If custom macro, parse the body substituting the above found args.
          if (customMacro)
          {
            if (customMacro->macroBody)
            {
              wxChar macroBuf[300];
//              wxStrcpy(macroBuf, _T("{"));
              wxStrcpy(macroBuf, customMacro->macroBody);
              wxStrcat(macroBuf, _T("}"));
              ParseArg(thisArg, children, macroBuf, 0, NULL, true, chunk);
            }

//            delete chunk; // Might delete children
          }
        }
        else
        {
          MacroError(buffer+pos);
        }
        }
        break;
      }
      // Parse constructs like {\bf thing} as if they were
      // \bf{thing}
      case _T('{'):
      {
        pos ++;
        if (buffer[pos] == _T('\\'))
        {
          if (buf_ptr > 0)
          {
            TexChunk *chunk = new TexChunk(CHUNK_TYPE_STRING);
            BigBuffer[buf_ptr] = 0;
            buf_ptr = 0;
            chunk->value = copystring(BigBuffer);
            children.Append((wxObject *)chunk);
          }
          pos ++;

          wxChar *env;
          bool tmpParseToBrace;
          TexMacroDef *def = MatchMacro(buffer, &pos, &env, &tmpParseToBrace);
          if (def)
          {
            CustomMacro *customMacro = FindCustomMacro(def->name);

            TexChunk *chunk = new TexChunk(CHUNK_TYPE_MACRO, def);
            chunk->no_args = def->no_args;
//            chunk->name = copystring(def->name);
            chunk->macroId = def->macroId;
            if (!customMacro)
              children.Append((wxObject *)chunk);

            pos = ParseMacroBody(def->name, chunk, chunk->no_args,
                       buffer, pos, NULL, true, customMacroArgs);

            // If custom macro, parse the body substituting the above found args.
            if (customMacro)
            {
              if (customMacro->macroBody)
              {
                wxChar macroBuf[300];
//                wxStrcpy(macroBuf, _T("{"));
                wxStrcpy(macroBuf, customMacro->macroBody);
                wxStrcat(macroBuf, _T("}"));
                ParseArg(thisArg, children, macroBuf, 0, NULL, true, chunk);
              }

//            delete chunk; // Might delete children
        }
          }
          else
          {
            MacroError(buffer+pos);
          }
        }
        else
        {
         /*
          * If all else fails, we assume that we have
          * a pair of braces on their own, so return a `dummy' macro
          * definition with just one argument to parse.
          */
          if (!SoloBlockDef)
          {
            SoloBlockDef = new TexMacroDef(ltSOLO_BLOCK, _T("solo block"), 1, false);
          }
          // Save text so far
          if (buf_ptr > 0)
          {
            TexChunk *chunk1 = new TexChunk(CHUNK_TYPE_STRING);
            BigBuffer[buf_ptr] = 0;
            buf_ptr = 0;
            chunk1->value = copystring(BigBuffer);
            children.Append((wxObject *)chunk1);
          }
          TexChunk *chunk = new TexChunk(CHUNK_TYPE_MACRO, SoloBlockDef);
          chunk->no_args = SoloBlockDef->no_args;
//          chunk->name = copystring(SoloBlockDef->name);
          chunk->macroId = SoloBlockDef->macroId;
          children.Append((wxObject *)chunk);

          TexChunk *arg = new TexChunk(CHUNK_TYPE_ARG, SoloBlockDef);

          chunk->children.Append((wxObject *)arg);
//          arg->name = copystring(SoloBlockDef->name);
          arg->argn = 1;
          arg->macroId = chunk->macroId;

          pos = ParseArg(arg, arg->children, buffer, pos, NULL, true, customMacroArgs);
        }
        break;
      }
      case _T('$'):
      {
        if (buf_ptr > 0)
        {
          TexChunk *chunk = new TexChunk(CHUNK_TYPE_STRING);
          BigBuffer[buf_ptr] = 0;
          buf_ptr = 0;
          chunk->value = copystring(BigBuffer);
          children.Append((wxObject *)chunk);
        }

        pos ++;

        if (buffer[pos] == _T('$'))
        {
          TexChunk *chunk = new TexChunk(CHUNK_TYPE_MACRO);
          chunk->no_args = 0;
//          chunk->name = copystring(_T("$$"));
          chunk->macroId = ltSPECIALDOUBLEDOLLAR;
          children.Append((wxObject *)chunk);
          pos ++;
        }
        else
        {
          TexChunk *chunk = new TexChunk(CHUNK_TYPE_MACRO);
          chunk->no_args = 0;
//          chunk->name = copystring(_T("_$"));
          chunk->macroId = ltSPECIALDOLLAR;
          children.Append((wxObject *)chunk);
        }
        break;
      }
      case _T('~'):
      {
        if (buf_ptr > 0)
        {
          TexChunk *chunk = new TexChunk(CHUNK_TYPE_STRING);
          BigBuffer[buf_ptr] = 0;
          buf_ptr = 0;
          chunk->value = copystring(BigBuffer);
          children.Append((wxObject *)chunk);
        }

        pos ++;
        TexChunk *chunk = new TexChunk(CHUNK_TYPE_MACRO);
        chunk->no_args = 0;
//        chunk->name = copystring(_T("_~"));
        chunk->macroId = ltSPECIALTILDE;
        children.Append((wxObject *)chunk);
        break;
      }
      case _T('#'): // Either treat as a special TeX character or as a macro arg
      {
        if (buf_ptr > 0)
        {
          TexChunk *chunk = new TexChunk(CHUNK_TYPE_STRING);
          BigBuffer[buf_ptr] = 0;
          buf_ptr = 0;
          chunk->value = copystring(BigBuffer);
          children.Append((wxObject *)chunk);
        }

        pos ++;
        if (!customMacroArgs)
        {
          TexChunk *chunk = new TexChunk(CHUNK_TYPE_MACRO);
          chunk->no_args = 0;
//          chunk->name = copystring(_T("_#"));
          chunk->macroId = ltSPECIALHASH;
          children.Append((wxObject *)chunk);
        }
        else
        {
          if (isdigit(buffer[pos]))
          {
            int n = buffer[pos] - 48;
            pos ++;
            wxNode *node = customMacroArgs->children.Item(n-1);
            if (node)
            {
              TexChunk *argChunk = (TexChunk *)node->GetData();
              children.Append((wxObject *)new TexChunk(*argChunk));
            }
          }
        }
        break;
      }
      case _T('&'):
      {
        // Remove white space before and after the ampersand,
        // since this is probably a table column separator with
        // some convenient -- but useless -- white space in the text.
        while ((buf_ptr > 0) && ((BigBuffer[buf_ptr-1] == _T(' ')) || (BigBuffer[buf_ptr-1] == 9)))
          buf_ptr --;

        if (buf_ptr > 0)
        {
          TexChunk *chunk = new TexChunk(CHUNK_TYPE_STRING);
          BigBuffer[buf_ptr] = 0;
          buf_ptr = 0;
          chunk->value = copystring(BigBuffer);
          children.Append((wxObject *)chunk);
        }

        pos ++;

        while (buffer[pos] == _T(' ') || buffer[pos] == 9)
          pos ++;

        TexChunk *chunk = new TexChunk(CHUNK_TYPE_MACRO);
        chunk->no_args = 0;
//        chunk->name = copystring(_T("_&"));
        chunk->macroId = ltSPECIALAMPERSAND;
        children.Append((wxObject *)chunk);
        break;
      }
      // Eliminate end-of-line comment
      case _T('%'):
      {
        wxCh = buffer[pos];
        while (wxCh != 10 && wxCh != 13 && wxCh != 0)
        {
          pos ++;
          wxCh = buffer[pos];
        }
        if (buffer[pos] == 10 || buffer[pos] == 13)
        {
          pos ++;
          if (buffer[pos] == 10) pos ++; // Eliminate newline following DOS line feed
        }
        break;
      }
      // Eliminate tab
      case 9:
      {
        BigBuffer[buf_ptr] = _T(' ');
        BigBuffer[buf_ptr+1] = 0;
        buf_ptr ++;
        pos ++;
        break;
      }
      default:
      {
        BigBuffer[buf_ptr] = wxCh;
        BigBuffer[buf_ptr+1] = 0;
        buf_ptr ++;
        pos ++;
        break;
      }
    }
  }
  return pos;
}

/*
 * Consume as many arguments as the macro definition specifies
 *
 */

int ParseMacroBody(const wxChar *WXUNUSED(macro_name), TexChunk *parent,
                   int no_args, wxChar *buffer, int pos,
                   wxChar *environment, bool parseToBrace,
                   TexChunk *customMacroArgs)
{
  Tex2RTFYield();
  if (stopRunning) return pos;

  // Check for a first optional argument
  if (buffer[pos] == ' ' && buffer[pos+1] == '[')
  {
    // Fool following code into thinking that this is definitely
    // an optional first argument. (If a space before a non-first argument,
    // [ is interpreted as a [, not an optional argument.)
    buffer[pos] = '!';
    pos ++;
    no_args ++;
  }
  else
    if (buffer[pos] == '[')
      no_args ++;

  int maxArgs = 0;

  int i;
  for (i = 0; i < no_args; i++)
  {
    maxArgs ++;
    TexChunk *arg = new TexChunk(CHUNK_TYPE_ARG, parent->def);

    parent->children.Append((wxObject *)arg);
//    arg->name = copystring(macro_name);
    arg->argn = maxArgs;
    arg->macroId = parent->macroId;

    // To parse the first arg of a 2 arg \begin{thing}{arg} ... \end{thing}
    // have to fool parser into thinking this is a regular kind of block.
    wxChar *actualEnv;
    if ((no_args == 2) && (i == 0))
      actualEnv = NULL;
    else
      actualEnv = environment;

    bool isOptional = false;

    // Remove the first { of the argument so it doesn't get recognized as { ... }
//    EatWhiteSpace(buffer, &pos);
    if (!actualEnv)
    {
      // The reason for these tests is to not consume braces that don't
      // belong to this macro.
      // E.g. {\bf {\small thing}}
      if ((pos > 0) && (buffer[pos-1] != ' ') && buffer[pos] == '{')
        pos ++;
      else
      if ((pos > 0) && (buffer[pos-1] != ' ') && (buffer[pos] == '['))
      {
        isOptional = true;
        pos ++;
      }
      else if ((pos > 1) && (buffer[pos-1] != ' ') && (buffer[pos+1] == '['))
      {
        isOptional = true;
        pos += 2;
      }
      else if (i > 0)
      {
        wxString errBuf;
        wxString tmpBuffer(buffer);
        if (tmpBuffer.length() > 4)
        {
            if (tmpBuffer.Right(4) == _T("\\par"))
                tmpBuffer = tmpBuffer.Mid(0,tmpBuffer.length()-4);
        }
        errBuf.Printf(_T("Missing macro argument in the line:\n\t%s\n"),tmpBuffer.c_str());
        OnError((wxChar *)errBuf.c_str());
      }

    }
    arg->optional = isOptional;

    pos = ParseArg(arg, arg->children, buffer, pos, actualEnv, parseToBrace, customMacroArgs);

    // If we've encountered an OPTIONAL argument, go another time around
    // the loop, because we've got more than we thought.
    // Hopefully optional args don't occur at the end of a macro use
    // or we might miss it.
    // Don't increment no of times round loop if the first optional arg
    // -- we already did it before the loop.
    if (arg->optional && (i > 0))
      i --;
  }
  parent->no_args = maxArgs;

  // Tell each argument how many args there are (useful when processing an arg)
  wxNode *node = parent->children.GetFirst();
  while (node)
  {
    TexChunk *chunk = (TexChunk *)node->GetData();
    chunk->no_args = maxArgs;
    node = node->GetNext();
  }
  return pos;
}

bool TexLoadFile(const wxString& filename)
{
    static wxChar *line_buffer;
    stopRunning = false;
    wxStrcpy(TexFileRoot, filename);
    StripExtension(TexFileRoot);
    wxSnprintf(TexBibName, 300, _T("%s.bb"), TexFileRoot);
    wxSnprintf(TexTmpBibName, 300, _T("%s.bb1"), TexFileRoot);

    TexPathList.EnsureFileAccessible(filename);

    if (line_buffer)
        delete line_buffer;

    line_buffer = new wxChar[MAX_LINE_BUFFER_SIZE];

    Inputs[0] = wxFopen(filename, _T("r"));
    LineNumbers[0] = 1;
    FileNames[0] = copystring(filename);
    if (Inputs[0])
    {
        read_a_line(line_buffer);
        ParseMacroBody(_T("toplevel"), TopLevel, 1, line_buffer, 0, NULL, true);
        if (Inputs[0]) fclose(Inputs[0]);
        return true;
    }

    return false;
}

TexMacroDef::TexMacroDef(int the_id, const wxChar *the_name, int n, bool ig, bool forbidLevel)
{
  name = copystring(the_name);
  no_args = n;
  ignore = ig;
  macroId = the_id;
  forbidden = forbidLevel;
}

TexMacroDef::~TexMacroDef(void)
{
  if (name) delete[] name;
}

TexChunk::TexChunk(int the_type, TexMacroDef *the_def)
{
  type = the_type;
  no_args = 0;
  argn = 0;
//  name = NULL;
  def = the_def;
  macroId = 0;
  value = NULL;
  optional = false;
}

TexChunk::TexChunk(TexChunk& toCopy)
{
  type = toCopy.type;
  no_args = toCopy.no_args;
  argn = toCopy.argn;
  macroId = toCopy.macroId;

//  if (toCopy.name)
//    name = copystring(toCopy.name);
//  else
//    name = NULL;
  def = toCopy.def;

  if (toCopy.value)
    value = copystring(toCopy.value);
  else
    value = NULL;

  optional = toCopy.optional;
  wxNode *node = toCopy.children.GetFirst();
  while (node)
  {
    TexChunk *child = (TexChunk *)node->GetData();
    children.Append((wxObject *)new TexChunk(*child));
    node = node->GetNext();
  }
}

TexChunk::~TexChunk(void)
{
//  if (name) delete[] name;
  if (value) delete[] value;
  wxNode *node = children.GetFirst();
  while (node)
  {
    TexChunk *child = (TexChunk *)node->GetData();
    delete child;
    wxNode *next = node->GetNext();
    delete node;
    node = next;
  }
}

bool IsArgOptional(void)  // Is this argument an optional argument?
{
  return isArgOptional;
}

int GetNoArgs(void) // Number of args for this macro
{
  return noArgs;
}

/* Gets the text of a chunk on request (must be for small arguments
 * only!)
 *
 */

void GetArgData1(TexChunk *chunk)
{
  switch (chunk->type)
  {
    case CHUNK_TYPE_MACRO:
    {
      TexMacroDef *def = chunk->def;
      if (def && def->ignore)
        return;

      if (def && (wxStrcmp(def->name, _T("solo block")) != 0))
      {
        wxStrcat(currentArgData, _T("\\"));
        wxStrcat(currentArgData, def->name);
      }

      wxNode *node = chunk->children.GetFirst();
      while (node)
      {
        TexChunk *child_chunk = (TexChunk *)node->GetData();
        wxStrcat(currentArgData, _T("{"));
        GetArgData1(child_chunk);
        wxStrcat(currentArgData, _T("}"));
        node = node->GetNext();
      }
      break;
    }
    case CHUNK_TYPE_ARG:
    {
      wxNode *node = chunk->children.GetFirst();
      while (node)
      {
        TexChunk *child_chunk = (TexChunk *)node->GetData();
        GetArgData1(child_chunk);
        node = node->GetNext();
      }
      break;
    }
    case CHUNK_TYPE_STRING:
    {
      if (chunk->value)
        wxStrcat(currentArgData, chunk->value);
      break;
    }
  }
}

wxChar *GetArgData(TexChunk *WXUNUSED(chunk))
{
  currentArgData[0] = 0;
  GetArgData1(currentArgument);
  haveArgData = false;
  return currentArgData;
}

wxChar *GetArgData(void)
{
  if (!haveArgData)
  {
    currentArgData[0] = 0;
    GetArgData1(currentArgument);
  }
  return currentArgData;
}

TexChunk *GetArgChunk(void)
{
  return currentArgument;
}

TexChunk *GetNextChunk(void)     // Look ahead to the next chunk
{
  return nextChunk;
}

TexChunk *GetTopLevelChunk(void)
{
  return TopLevel;
}

int GetCurrentColumn(void)
{
  return currentColumn;
}

/*
 * Traverses document calling functions to allow the client to
 * write out the appropriate stuff
 */


void TraverseFromChunk(TexChunk *chunk, wxNode *thisNode, bool childrenOnly)
{
  Tex2RTFYield();
  if (stopRunning) return;

  switch (chunk->type)
  {
    case CHUNK_TYPE_MACRO:
    {
      TexMacroDef *def = chunk->def;
      if (def && def->ignore)
        return;

      if (!childrenOnly)
        OnMacro(chunk->macroId, chunk->no_args, true);

      wxNode *node = chunk->children.GetFirst();
      while (node)
      {
        TexChunk *child_chunk = (TexChunk *)node->GetData();
        TraverseFromChunk(child_chunk, node);
        node = node->GetNext();
      }

      if (thisNode && thisNode->GetNext())
          nextChunk = (TexChunk *)thisNode->GetNext()->GetData();

      if (!childrenOnly)
        OnMacro(chunk->macroId, chunk->no_args, false);
      break;
    }
    case CHUNK_TYPE_ARG:
    {
      currentArgument = chunk;

      isArgOptional = chunk->optional;
      noArgs = chunk->no_args;

      // If OnArgument returns false, don't output.

      if (childrenOnly || OnArgument(chunk->macroId, chunk->argn, true))
      {
        wxNode *node = chunk->children.GetFirst();
        while (node)
        {
          TexChunk *child_chunk = (TexChunk *)node->GetData();
          TraverseFromChunk(child_chunk, node);
          node = node->GetNext();
        }
      }

      currentArgument = chunk;

      if (thisNode && thisNode->GetNext())
          nextChunk = (TexChunk *)thisNode->GetNext()->GetData();

      isArgOptional = chunk->optional;
      noArgs = chunk->no_args;

      if (!childrenOnly)
        (void)OnArgument(chunk->macroId, chunk->argn, false);
      break;
    }
    case CHUNK_TYPE_STRING:
    {
      extern int issuedNewParagraph;
      extern int forbidResetPar;
      if (chunk->value && (forbidResetPar == 0))
      {
        // If non-whitespace text, we no longer have a new paragraph.
        if (issuedNewParagraph && !((chunk->value[0] == 10 || chunk->value[0] == 13 || chunk->value[0] == 32)
                                    && chunk->value[1] == 0))
        {
          issuedNewParagraph = false;
        }
        TexOutput(chunk->value, true);
      }
      break;
    }
  }
}

void TraverseDocument(void)
{
  TraverseFromChunk(TopLevel, NULL);
}

void SetCurrentOutput(FILE *fd)
{
  CurrentOutput1 = fd;
  CurrentOutput2 = NULL;
}

void SetCurrentOutputs(FILE *fd1, FILE *fd2)
{
  CurrentOutput1 = fd1;
  CurrentOutput2 = fd2;
}

void AddMacroDef(int the_id, const wxChar *name, int n, bool ignore, bool forbid)
{
  MacroDefs.Put(name, new TexMacroDef(the_id, name, n, ignore, forbid));
}

void TexInitialize(int bufSize)
{
  InitialiseColourTable();
#ifdef __WXMSW__
  TexPathList.AddEnvList(_T("TEXINPUT"));
#endif
#ifdef __UNIX__
  TexPathList.AddEnvList(_T("TEXINPUTS"));
#endif
  int i;
  for (i = 0; i < 15; i++)
  {
    Inputs[i] = NULL;
    LineNumbers[i] = 1;
    FileNames[i] = NULL;
  }

  IgnorableInputFiles.Add(_T("psbox.tex"));
  BigBuffer = new wxChar[(bufSize*1000)];
  currentArgData = new wxChar[2000];
  TexFileRoot = new wxChar[300];
  TexBibName = new wxChar[300];
  TexTmpBibName = new wxChar[300];
  AddMacroDef(ltTOPLEVEL, _T("toplevel"), 1);
  TopLevel = new TexChunk(CHUNK_TYPE_MACRO);
//  TopLevel->name = copystring(_T("toplevel"));
  TopLevel->macroId = ltTOPLEVEL;
  TopLevel->no_args = 1;
  VerbatimMacroDef = (TexMacroDef *)MacroDefs.Get(_T("verbatim"));
}

void TexCleanUp(void)
{
  int i;
  for (i = 0; i < 15; i++)
    Inputs[i] = NULL;

  chapterNo = 0;
  sectionNo = 0;
  subsectionNo = 0;
  subsubsectionNo = 0;
  figureNo = 0;

  CurrentOutput1 = NULL;
  CurrentOutput2 = NULL;
  CurrentInputIndex = 0;
  haveArgData = false;
  noArgs = 0;

  if (TopLevel)
    delete TopLevel;
  TopLevel = new TexChunk(CHUNK_TYPE_MACRO);
//  TopLevel->name = copystring(_T("toplevel"));
  TopLevel->macroId = ltTOPLEVEL;
  TopLevel->no_args = 1;

  DocumentTitle = NULL;
  DocumentAuthor = NULL;
  DocumentDate = NULL;
  DocumentStyle = LATEX_REPORT;
  MinorDocumentStyle = 0;
  BibliographyStyleString = copystring(_T("plain"));
  DocumentStyleString = copystring(_T("report"));
  MinorDocumentStyleString = NULL;

  // gt - Changed this so if this is the final pass
  // then we DO want to remove these macros, so that
  // memory is not MASSIVELY leaked if the user
  // does not exit the program, but instead runs
  // the program again
  if ((passNumber == 1 && !runTwice) ||
      (passNumber == 2 && runTwice))
  {
/* Don't want to remove custom macros after each pass.*/
      SetFontSizes(10);
      wxNode *node = CustomMacroList.GetFirst();
      while (node)
      {
        CustomMacro *macro = (CustomMacro *)node->GetData();
        delete macro;
        delete node;
        node = CustomMacroList.GetFirst();
      }
  }
/**/
  TexReferences.BeginFind();
  wxHashTable::Node *refNode = TexReferences.Next();
  while (refNode)
  {
    TexRef *ref = (TexRef *)refNode->GetData();
    delete ref;
    refNode = TexReferences.Next();
  }
  TexReferences.Clear();

  wxNode* bibNode = BibList.GetFirst();
  while (bibNode)
  {
    BibEntry *entry = (BibEntry *)bibNode->GetData();
    delete entry;
    delete bibNode;
    bibNode = BibList.GetFirst();
  }
  CitationList.Clear();
  ResetTopicCounter();
}

// There is likely to be one set of macros used by all utilities.
void DefineDefaultMacros(void)
{
  // Put names which subsume other names at the TOP
  // so they get recognized first

  AddMacroDef(ltACCENT_GRAVE,        _T("`"), 1);
  AddMacroDef(ltACCENT_ACUTE,        _T("'"), 1);
  AddMacroDef(ltACCENT_CARET,        _T("^"), 1);
  AddMacroDef(ltACCENT_UMLAUT,       _T("\""), 1);
  AddMacroDef(ltACCENT_TILDE,        _T("~"), 1);
  AddMacroDef(ltACCENT_DOT,          _T("."), 1);
  AddMacroDef(ltACCENT_CADILLA,      _T("c"), 1);
  AddMacroDef(ltSMALLSPACE1,         _T(","), 0);
  AddMacroDef(ltSMALLSPACE2,         _T(";"), 0);

  AddMacroDef(ltABSTRACT,            _T("abstract"), 1);
  AddMacroDef(ltADDCONTENTSLINE,     _T("addcontentsline"), 3);
  AddMacroDef(ltADDTOCOUNTER,        _T("addtocounter"), 2);
  AddMacroDef(ltALEPH,               _T("aleph"), 0);
  AddMacroDef(ltALPHA,               _T("alpha"), 0);
  AddMacroDef(ltALPH1,               _T("alph"), 1);
  AddMacroDef(ltALPH2,               _T("Alph"), 1);
  AddMacroDef(ltANGLE,               _T("angle"), 0);
  AddMacroDef(ltAPPENDIX,            _T("appendix"), 0);
  AddMacroDef(ltAPPROX,              _T("approx"), 0);
  AddMacroDef(ltARABIC,              _T("arabic"), 1);
  AddMacroDef(ltARRAY,               _T("array"), 1);
  AddMacroDef(ltAST,                 _T("ast"), 0);
  AddMacroDef(ltASYMP,               _T("asymp"), 0);
  AddMacroDef(ltAUTHOR,              _T("author"), 1);

  AddMacroDef(ltBACKGROUNDCOLOUR,    _T("backgroundcolour"), 1);
  AddMacroDef(ltBACKGROUNDIMAGE,     _T("backgroundimage"), 1);
  AddMacroDef(ltBACKGROUND,          _T("background"), 1);
  AddMacroDef(ltBACKSLASHRAW,        _T("backslashraw"), 0);
  AddMacroDef(ltBACKSLASH,           _T("backslash"), 0);
  AddMacroDef(ltBASELINESKIP,        _T("baselineskip"), 1);
  AddMacroDef(ltBCOL,                _T("bcol"), 2);
  AddMacroDef(ltBETA,                _T("beta"), 0);
  AddMacroDef(ltBFSERIES,            _T("bfseries"), 1);
  AddMacroDef(ltBF,                  _T("bf"), 1);
  AddMacroDef(ltBIBITEM,             _T("bibitem"), 2);
             // For convenience, bibitem has 2 args: label and item.
                              // The Latex syntax permits writing as 2 args.
  AddMacroDef(ltBIBLIOGRAPHYSTYLE,   _T("bibliographystyle"), 1);
  AddMacroDef(ltBIBLIOGRAPHY,        _T("bibliography"), 1);
  AddMacroDef(ltBIGTRIANGLEDOWN,     _T("bigtriangledown"), 0);
  AddMacroDef(ltBOT,                 _T("bot"), 0);
  AddMacroDef(ltBOXIT,               _T("boxit"), 1);
  AddMacroDef(ltBOX,                 _T("box"), 0);
  AddMacroDef(ltBRCLEAR,             _T("brclear"), 0);
  AddMacroDef(ltBULLET,              _T("bullet"), 0);

  AddMacroDef(ltCAPTIONSTAR,         _T("caption*"), 1);
  AddMacroDef(ltCAPTION,             _T("caption"), 1);
  AddMacroDef(ltCAP,                 _T("cap"), 0);
  AddMacroDef(ltCDOTS,               _T("cdots"), 0);
  AddMacroDef(ltCDOT,                _T("cdot"), 0);
  AddMacroDef(ltCENTERLINE,          _T("centerline"), 1);
  AddMacroDef(ltCENTERING,           _T("centering"), 0);
  AddMacroDef(ltCENTER,              _T("center"), 1);
  AddMacroDef(ltCEXTRACT,            _T("cextract"), 0);
  AddMacroDef(ltCHAPTERHEADING,      _T("chapterheading"), 1);
  AddMacroDef(ltCHAPTERSTAR,         _T("chapter*"), 1);
  AddMacroDef(ltCHAPTER,             _T("chapter"), 1);
  AddMacroDef(ltCHI,                 _T("chi"), 0);
  AddMacroDef(ltCINSERT,             _T("cinsert"), 0);
  AddMacroDef(ltCIRC,                _T("circ"), 0);
  AddMacroDef(ltCITE,                _T("cite"), 1);
  AddMacroDef(ltCLASS,               _T("class"), 1);
  AddMacroDef(ltCLEARDOUBLEPAGE,     _T("cleardoublepage"), 0);
  AddMacroDef(ltCLEARPAGE,           _T("clearpage"), 0);
  AddMacroDef(ltCLINE,               _T("cline"), 1);
  AddMacroDef(ltCLIPSFUNC,           _T("clipsfunc"), 3);
  AddMacroDef(ltCLUBSUIT,            _T("clubsuit"), 0);
  AddMacroDef(ltCOLUMNSEP,           _T("columnsep"), 1);
  AddMacroDef(ltCOMMENT,             _T("comment"), 1, true);
  AddMacroDef(ltCONG,                _T("cong"), 0);
  AddMacroDef(ltCOPYRIGHT,           _T("copyright"), 0);
  AddMacroDef(ltCPARAM,              _T("cparam"), 2);
  AddMacroDef(ltCHEAD,               _T("chead"), 1);
  AddMacroDef(ltCFOOT,               _T("cfoot"), 1);
  AddMacroDef(ltCUP,                 _T("cup"), 0);

  AddMacroDef(ltDASHV,               _T("dashv"), 0);
  AddMacroDef(ltDATE,                _T("date"), 1);
  AddMacroDef(ltDELTA,               _T("delta"), 0);
  AddMacroDef(ltCAP_DELTA,           _T("Delta"), 0);
  AddMacroDef(ltDEFINECOLOUR,        _T("definecolour"), 4);
  AddMacroDef(ltDEFINECOLOR,         _T("definecolor"), 4);
  AddMacroDef(ltDESCRIPTION,         _T("description"), 1);
  AddMacroDef(ltDESTRUCT,            _T("destruct"), 1);
  AddMacroDef(ltDIAMOND2,            _T("diamond2"), 0);
  AddMacroDef(ltDIAMOND,             _T("diamond"), 0);
  AddMacroDef(ltDIV,                 _T("div"), 0);
  AddMacroDef(ltDOCUMENTCLASS,       _T("documentclass"), 1);
  AddMacroDef(ltDOCUMENTSTYLE,       _T("documentstyle"), 1);
  AddMacroDef(ltDOCUMENT,            _T("document"), 1);
  AddMacroDef(ltDOUBLESPACE,         _T("doublespace"), 1);
  AddMacroDef(ltDOTEQ,               _T("doteq"), 0);
  AddMacroDef(ltDOWNARROW,           _T("downarrow"), 0);
  AddMacroDef(ltDOWNARROW2,          _T("Downarrow"), 0);

  AddMacroDef(ltEMPTYSET,            _T("emptyset"), 0);
  AddMacroDef(ltEMPH,                _T("emph"), 1);
  AddMacroDef(ltEM,                  _T("em"), 1);
  AddMacroDef(ltENUMERATE,           _T("enumerate"), 1);
  AddMacroDef(ltEPSILON,             _T("epsilon"), 0);
  AddMacroDef(ltEQUATION,            _T("equation"), 1);
  AddMacroDef(ltEQUIV,               _T("equiv"), 0);
  AddMacroDef(ltETA,                 _T("eta"), 0);
  AddMacroDef(ltEVENSIDEMARGIN,      _T("evensidemargin"), 1);
  AddMacroDef(ltEXISTS,              _T("exists"), 0);

  AddMacroDef(ltFBOX,                _T("fbox"), 1);
  AddMacroDef(ltFCOL,                _T("fcol"), 2);
  AddMacroDef(ltFIGURE,              _T("figure"), 1);
  AddMacroDef(ltFIGURESTAR,          _T("figure*"), 1);
  AddMacroDef(ltFLUSHLEFT,           _T("flushleft"), 1);
  AddMacroDef(ltFLUSHRIGHT,          _T("flushright"), 1);
  AddMacroDef(ltFOLLOWEDLINKCOLOUR,  _T("followedlinkcolour"), 1);
  AddMacroDef(ltFOOTHEIGHT,          _T("footheight"), 1);
  AddMacroDef(ltFOOTNOTEPOPUP,       _T("footnotepopup"), 2);
  AddMacroDef(ltFOOTNOTE,            _T("footnote"), 1);
  AddMacroDef(ltFOOTSKIP,            _T("footskip"), 1);
  AddMacroDef(ltFORALL,              _T("forall"), 0);
  AddMacroDef(ltFRAMEBOX,            _T("framebox"), 1);
  AddMacroDef(ltFROWN,               _T("frown"), 0);
  AddMacroDef(ltFUNCTIONSECTION,     _T("functionsection"), 1);
  AddMacroDef(ltFUNC,                _T("func"), 3);
  AddMacroDef(ltFOOTNOTESIZE,        _T("footnotesize"), 0);
  AddMacroDef(ltFANCYPLAIN,          _T("fancyplain"), 2);

  AddMacroDef(ltGAMMA,               _T("gamma"), 0);
  AddMacroDef(ltCAP_GAMMA,           _T("Gamma"), 0);
  AddMacroDef(ltGEQ,                 _T("geq"), 0);
  AddMacroDef(ltGE,                  _T("ge"), 0);
  AddMacroDef(ltGG,                  _T("gg"), 0);
  AddMacroDef(ltGLOSSARY,            _T("glossary"), 1);
  AddMacroDef(ltGLOSS,               _T("gloss"), 1);

  AddMacroDef(ltHEADHEIGHT,          _T("headheight"), 1);
  AddMacroDef(ltHEARTSUIT,           _T("heartsuit"), 0);
  AddMacroDef(ltHELPGLOSSARY,        _T("helpglossary"), 1);
  AddMacroDef(ltHELPIGNORE,          _T("helpignore"), 1, true);
  AddMacroDef(ltHELPONLY,            _T("helponly"), 1);
  AddMacroDef(ltHELPINPUT,           _T("helpinput"), 1);
  AddMacroDef(ltHELPFONTFAMILY,      _T("helpfontfamily"), 1);
  AddMacroDef(ltHELPFONTSIZE,        _T("helpfontsize"), 1);
  AddMacroDef(ltHELPREFN,            _T("helprefn"), 2);
  AddMacroDef(ltHELPREF,             _T("helpref"), 2);
  AddMacroDef(ltHFILL,               _T("hfill"), 0);
  AddMacroDef(ltHLINE,               _T("hline"), 0);
  AddMacroDef(ltHRULE,               _T("hrule"), 0);
  AddMacroDef(ltHSPACESTAR,          _T("hspace*"), 1);
  AddMacroDef(ltHSPACE,              _T("hspace"), 1);
  AddMacroDef(ltHSKIPSTAR,           _T("hskip*"), 1);
  AddMacroDef(ltHSKIP,               _T("hskip"), 1);
  AddMacroDef(lthuge,                _T("huge"), 1);
  AddMacroDef(ltHuge,                _T("Huge"), 1);
  AddMacroDef(ltHUGE,                _T("HUGE"), 1);
  AddMacroDef(ltHTMLIGNORE,          _T("htmlignore"), 1);
  AddMacroDef(ltHTMLONLY,            _T("htmlonly"), 1);

  AddMacroDef(ltIM,                  _T("im"), 0);
  AddMacroDef(ltINCLUDEONLY,         _T("includeonly"), 1);
  AddMacroDef(ltINCLUDE,             _T("include"), 1);
  AddMacroDef(ltINDENTED,            _T("indented"), 2);
  AddMacroDef(ltINDEX,               _T("index"), 1);
  AddMacroDef(ltINPUT,               _T("input"), 1, true);
  AddMacroDef(ltIOTA,                _T("iota"), 0);
  AddMacroDef(ltITEMIZE,             _T("itemize"), 1);
  AddMacroDef(ltITEM,                _T("item"), 0);
  AddMacroDef(ltIMAGEMAP,            _T("imagemap"), 3);
  AddMacroDef(ltIMAGEL,              _T("imagel"), 2);
  AddMacroDef(ltIMAGER,              _T("imager"), 2);
  AddMacroDef(ltIMAGE,               _T("image"), 2);
  AddMacroDef(ltIN,                  _T("in"), 0);
  AddMacroDef(ltINFTY,               _T("infty"), 0);
  AddMacroDef(ltITSHAPE,             _T("itshape"), 1);
  AddMacroDef(ltIT,                  _T("it"), 1);
  AddMacroDef(ltITEMSEP,             _T("itemsep"), 1);
  AddMacroDef(ltINSERTATLEVEL,       _T("insertatlevel"), 2);

  AddMacroDef(ltKAPPA,               _T("kappa"), 0);
  AddMacroDef(ltKILL,                _T("kill"), 0);

  AddMacroDef(ltLABEL,               _T("label"), 1);
  AddMacroDef(ltLAMBDA,              _T("lambda"), 0);
  AddMacroDef(ltCAP_LAMBDA,          _T("Lambda"), 0);
  AddMacroDef(ltlarge,               _T("large"), 1);
  AddMacroDef(ltLarge,               _T("Large"), 1);
  AddMacroDef(ltLARGE,               _T("LARGE"), 1);
  AddMacroDef(ltLATEXIGNORE,         _T("latexignore"), 1);
  AddMacroDef(ltLATEXONLY,           _T("latexonly"), 1);
  AddMacroDef(ltLATEX,               _T("LaTeX"), 0);
  AddMacroDef(ltLBOX,                _T("lbox"), 1);
  AddMacroDef(ltLBRACERAW,           _T("lbraceraw"), 0);
  AddMacroDef(ltLDOTS,               _T("ldots"), 0);
  AddMacroDef(ltLEQ,                 _T("leq"), 0);
  AddMacroDef(ltLE,                  _T("le"), 0);
  AddMacroDef(ltLEFTARROW,           _T("leftarrow"), 0);
  AddMacroDef(ltLEFTRIGHTARROW,      _T("leftrightarrow"), 0);
  AddMacroDef(ltLEFTARROW2,          _T("Leftarrow"), 0);
  AddMacroDef(ltLEFTRIGHTARROW2,     _T("Leftrightarrow"), 0);
  AddMacroDef(ltLINEBREAK,           _T("linebreak"), 0);
  AddMacroDef(ltLINKCOLOUR,          _T("linkcolour"), 1);
  AddMacroDef(ltLISTOFFIGURES,       _T("listoffigures"), 0);
  AddMacroDef(ltLISTOFTABLES,        _T("listoftables"), 0);
  AddMacroDef(ltLHEAD,               _T("lhead"), 1);
  AddMacroDef(ltLFOOT,               _T("lfoot"), 1);
  AddMacroDef(ltLOWERCASE,           _T("lowercase"), 1);
  AddMacroDef(ltLL,                  _T("ll"), 0);

  AddMacroDef(ltMAKEGLOSSARY,        _T("makeglossary"), 0);
  AddMacroDef(ltMAKEINDEX,           _T("makeindex"), 0);
  AddMacroDef(ltMAKETITLE,           _T("maketitle"), 0);
  AddMacroDef(ltMARKRIGHT,           _T("markright"), 1);
  AddMacroDef(ltMARKBOTH,            _T("markboth"), 2);
  AddMacroDef(ltMARGINPARWIDTH,      _T("marginparwidth"), 1);
  AddMacroDef(ltMARGINPARSEP,        _T("marginparsep"), 1);
  AddMacroDef(ltMARGINPARODD,        _T("marginparodd"), 1);
  AddMacroDef(ltMARGINPAREVEN,       _T("marginpareven"), 1);
  AddMacroDef(ltMARGINPAR,           _T("marginpar"), 1);
  AddMacroDef(ltMBOX,                _T("mbox"), 1);
  AddMacroDef(ltMDSERIES,            _T("mdseries"), 1);
  AddMacroDef(ltMEMBERSECTION,       _T("membersection"), 1);
  AddMacroDef(ltMEMBER,              _T("member"), 2);
  AddMacroDef(ltMID,                 _T("mid"), 0);
  AddMacroDef(ltMODELS,              _T("models"), 0);
  AddMacroDef(ltMP,                  _T("mp"), 0);
  AddMacroDef(ltMULTICOLUMN,         _T("multicolumn"), 3);
  AddMacroDef(ltMU,                  _T("mu"), 0);

  AddMacroDef(ltNABLA,               _T("nabla"), 0);
  AddMacroDef(ltNEG,                 _T("neg"), 0);
  AddMacroDef(ltNEQ,                 _T("neq"), 0);
  AddMacroDef(ltNEWCOUNTER,          _T("newcounter"), 1, false, (bool)FORBID_ABSOLUTELY);
  AddMacroDef(ltNEWLINE,             _T("newline"), 0);
  AddMacroDef(ltNEWPAGE,             _T("newpage"), 0);
  AddMacroDef(ltNI,                  _T("ni"), 0);
  AddMacroDef(ltNOCITE,              _T("nocite"), 1);
  AddMacroDef(ltNOINDENT,            _T("noindent"), 0);
  AddMacroDef(ltNOLINEBREAK,         _T("nolinebreak"), 0);
  AddMacroDef(ltNOPAGEBREAK,         _T("nopagebreak"), 0);
  AddMacroDef(ltNORMALSIZE,          _T("normalsize"), 1);
  AddMacroDef(ltNORMALBOX,           _T("normalbox"), 1);
  AddMacroDef(ltNORMALBOXD,          _T("normalboxd"), 1);
  AddMacroDef(ltNOTEQ,               _T("noteq"), 0);
  AddMacroDef(ltNOTIN,               _T("notin"), 0);
  AddMacroDef(ltNOTSUBSET,           _T("notsubset"), 0);
  AddMacroDef(ltNU,                  _T("nu"), 0);

  AddMacroDef(ltODDSIDEMARGIN,       _T("oddsidemargin"), 1);
  AddMacroDef(ltOMEGA,               _T("omega"), 0);
  AddMacroDef(ltCAP_OMEGA,           _T("Omega"), 0);
  AddMacroDef(ltONECOLUMN,           _T("onecolumn"), 0);
  AddMacroDef(ltOPLUS,               _T("oplus"), 0);
  AddMacroDef(ltOSLASH,              _T("oslash"), 0);
  AddMacroDef(ltOTIMES,              _T("otimes"), 0);

  AddMacroDef(ltPAGEBREAK,           _T("pagebreak"), 0);
  AddMacroDef(ltPAGEREF,             _T("pageref"), 1);
  AddMacroDef(ltPAGESTYLE,           _T("pagestyle"), 1);
  AddMacroDef(ltPAGENUMBERING,       _T("pagenumbering"), 1);
  AddMacroDef(ltPARAGRAPHSTAR,       _T("paragraph*"), 1);
  AddMacroDef(ltPARAGRAPH,           _T("paragraph"), 1);
  AddMacroDef(ltPARALLEL,            _T("parallel"), 0);
  AddMacroDef(ltPARAM,               _T("param"), 2);
  AddMacroDef(ltPARINDENT,           _T("parindent"), 1);
  AddMacroDef(ltPARSKIP,             _T("parskip"), 1);
  AddMacroDef(ltPARTIAL,             _T("partial"), 0);
  AddMacroDef(ltPARTSTAR,            _T("part*"), 1);
  AddMacroDef(ltPART,                _T("part"), 1);
  AddMacroDef(ltPAR,                 _T("par"), 0);
  AddMacroDef(ltPERP,                _T("perp"), 0);
  AddMacroDef(ltPHI,                 _T("phi"), 0);
  AddMacroDef(ltCAP_PHI,             _T("Phi"), 0);
  AddMacroDef(ltPFUNC,               _T("pfunc"), 3);
  AddMacroDef(ltPICTURE,             _T("picture"), 1);
  AddMacroDef(ltPI,                  _T("pi"), 0);
  AddMacroDef(ltCAP_PI,              _T("Pi"), 0);
  AddMacroDef(ltPM,                  _T("pm"), 0);
  AddMacroDef(ltPOPREFONLY,          _T("poprefonly"), 1);
  AddMacroDef(ltPOPREF,              _T("popref"), 2);
  AddMacroDef(ltPOUNDS,              _T("pounds"), 0);
  AddMacroDef(ltPREC,                _T("prec"), 0);
  AddMacroDef(ltPRECEQ,              _T("preceq"), 0);
  AddMacroDef(ltPRINTINDEX,          _T("printindex"), 0);
  AddMacroDef(ltPROPTO,              _T("propto"), 0);
  AddMacroDef(ltPSBOXTO,             _T("psboxto"), 1, false, (bool)FORBID_ABSOLUTELY);
  AddMacroDef(ltPSBOX,               _T("psbox"), 1, false, (bool)FORBID_ABSOLUTELY);
  AddMacroDef(ltPSI,                 _T("psi"), 0);
  AddMacroDef(ltCAP_PSI,             _T("Psi"), 0);

  AddMacroDef(ltQUOTE,               _T("quote"), 1);
  AddMacroDef(ltQUOTATION,           _T("quotation"), 1);

  AddMacroDef(ltRAGGEDBOTTOM,        _T("raggedbottom"), 0);
  AddMacroDef(ltRAGGEDLEFT,          _T("raggedleft"), 0);
  AddMacroDef(ltRAGGEDRIGHT,         _T("raggedright"), 0);
  AddMacroDef(ltRBRACERAW,           _T("rbraceraw"), 0);
  AddMacroDef(ltREF,                 _T("ref"), 1);
  AddMacroDef(ltREGISTERED,          _T("registered"), 0);
  AddMacroDef(ltRE,                  _T("we"), 0);
  AddMacroDef(ltRHO,                 _T("rho"), 0);
  AddMacroDef(ltRIGHTARROW,          _T("rightarrow"), 0);
  AddMacroDef(ltRIGHTARROW2,         _T("rightarrow2"), 0);
  AddMacroDef(ltRMFAMILY,            _T("rmfamily"), 1);
  AddMacroDef(ltRM,                  _T("rm"), 1);
  AddMacroDef(ltROMAN,               _T("roman"), 1);
  AddMacroDef(ltROMAN2,              _T("Roman"), 1);
//  AddMacroDef(lt"row", 1);
  AddMacroDef(ltRTFSP,               _T("rtfsp"), 0);
  AddMacroDef(ltRTFIGNORE,           _T("rtfignore"), 1);
  AddMacroDef(ltRTFONLY,             _T("rtfonly"), 1);
  AddMacroDef(ltRULEDROW,            _T("ruledrow"), 1);
  AddMacroDef(ltDRULED,              _T("druled"), 1);
  AddMacroDef(ltRULE,                _T("rule"), 2);
  AddMacroDef(ltRHEAD,               _T("rhead"), 1);
  AddMacroDef(ltRFOOT,               _T("rfoot"), 1);
  AddMacroDef(ltROW,                 _T("row"), 1);

  AddMacroDef(ltSCSHAPE,             _T("scshape"), 1);
  AddMacroDef(ltSC,                  _T("sc"), 1);
  AddMacroDef(ltSECTIONHEADING,      _T("sectionheading"), 1);
  AddMacroDef(ltSECTIONSTAR,         _T("section*"), 1);
  AddMacroDef(ltSECTION,             _T("section"), 1);
  AddMacroDef(ltSETCOUNTER,          _T("setcounter"), 2);
  AddMacroDef(ltSFFAMILY,            _T("sffamily"), 1);
  AddMacroDef(ltSF,                  _T("sf"), 1);
  AddMacroDef(ltSHARP,               _T("sharp"), 0);
  AddMacroDef(ltSHORTCITE,           _T("shortcite"), 1);
  AddMacroDef(ltSIGMA,               _T("sigma"), 0);
  AddMacroDef(ltCAP_SIGMA,           _T("Sigma"), 0);
  AddMacroDef(ltSIM,                 _T("sim"), 0);
  AddMacroDef(ltSIMEQ,               _T("simeq"), 0);
  AddMacroDef(ltSINGLESPACE,         _T("singlespace"), 1);
  AddMacroDef(ltSIZEDBOX,            _T("sizedbox"), 2);
  AddMacroDef(ltSIZEDBOXD,           _T("sizedboxd"), 2);
  AddMacroDef(ltSLOPPYPAR,           _T("sloppypar"), 1);
  AddMacroDef(ltSLOPPY,              _T("sloppy"), 0);
  AddMacroDef(ltSLSHAPE,             _T("slshape"), 1);
  AddMacroDef(ltSL,                  _T("sl"), 1);
  AddMacroDef(ltSMALL,               _T("small"), 1);
  AddMacroDef(ltSMILE,               _T("smile"), 0);
  AddMacroDef(ltSS,                  _T("ss"), 0);
  AddMacroDef(ltSTAR,                _T("star"), 0);
  AddMacroDef(ltSUBITEM,             _T("subitem"), 0);
  AddMacroDef(ltSUBPARAGRAPHSTAR,    _T("subparagraph*"), 1);
  AddMacroDef(ltSUBPARAGRAPH,        _T("subparagraph"), 1);
  AddMacroDef(ltSPECIAL,             _T("special"), 1);
  AddMacroDef(ltSUBSECTIONSTAR,      _T("subsection*"), 1);
  AddMacroDef(ltSUBSECTION,          _T("subsection"), 1);
  AddMacroDef(ltSUBSETEQ,            _T("subseteq"), 0);
  AddMacroDef(ltSUBSET,              _T("subset"), 0);
  AddMacroDef(ltSUCC,                _T("succ"), 0);
  AddMacroDef(ltSUCCEQ,              _T("succeq"), 0);
  AddMacroDef(ltSUPSETEQ,            _T("supseteq"), 0);
  AddMacroDef(ltSUPSET,              _T("supset"), 0);
  AddMacroDef(ltSUBSUBSECTIONSTAR,   _T("subsubsection*"), 1);
  AddMacroDef(ltSUBSUBSECTION,       _T("subsubsection"), 1);
  AddMacroDef(ltSUPERTABULAR,        _T("supertabular"), 2, false);
  AddMacroDef(ltSURD,                _T("surd"), 0);
  AddMacroDef(ltSCRIPTSIZE,          _T("scriptsize"), 1);
  AddMacroDef(ltSETHEADER,           _T("setheader"), 6);
  AddMacroDef(ltSETFOOTER,           _T("setfooter"), 6);
  AddMacroDef(ltSETHOTSPOTCOLOUR,    _T("sethotspotcolour"), 1);
  AddMacroDef(ltSETHOTSPOTCOLOR,     _T("sethotspotcolor"), 1);
  AddMacroDef(ltSETHOTSPOTUNDERLINE, _T("sethotspotunderline"), 1);
  AddMacroDef(ltSETTRANSPARENCY,     _T("settransparency"), 1);
  AddMacroDef(ltSPADESUIT,           _T("spadesuit"), 0);

  AddMacroDef(ltTABBING,             _T("tabbing"), 2);
  AddMacroDef(ltTABLEOFCONTENTS,     _T("tableofcontents"), 0);
  AddMacroDef(ltTABLE,               _T("table"), 1);
  AddMacroDef(ltTABULAR,             _T("tabular"), 2, false);
  AddMacroDef(ltTAB,                 _T("tab"), 0);
  AddMacroDef(ltTAU,                 _T("tau"), 0);
  AddMacroDef(ltTEXTRM,              _T("textrm"), 1);
  AddMacroDef(ltTEXTSF,              _T("textsf"), 1);
  AddMacroDef(ltTEXTTT,              _T("texttt"), 1);
  AddMacroDef(ltTEXTBF,              _T("textbf"), 1);
  AddMacroDef(ltTEXTIT,              _T("textit"), 1);
  AddMacroDef(ltTEXTSL,              _T("textsl"), 1);
  AddMacroDef(ltTEXTSC,              _T("textsc"), 1);
  AddMacroDef(ltTEXTWIDTH,           _T("textwidth"), 1);
  AddMacroDef(ltTEXTHEIGHT,          _T("textheight"), 1);
  AddMacroDef(ltTEXTCOLOUR,          _T("textcolour"), 1);
  AddMacroDef(ltTEX,                 _T("TeX"), 0);
  AddMacroDef(ltTHEBIBLIOGRAPHY,     _T("thebibliography"), 2);
  AddMacroDef(ltTHETA,               _T("theta"), 0);
  AddMacroDef(ltTIMES,               _T("times"), 0);
  AddMacroDef(ltCAP_THETA,           _T("Theta"), 0);
  AddMacroDef(ltTITLEPAGE,           _T("titlepage"), 1);
  AddMacroDef(ltTITLE,               _T("title"), 1);
  AddMacroDef(ltTINY,                _T("tiny"), 1);
  AddMacroDef(ltTODAY,               _T("today"), 0);
  AddMacroDef(ltTOPMARGIN,           _T("topmargin"), 1);
  AddMacroDef(ltTOPSKIP,             _T("topskip"), 1);
  AddMacroDef(ltTRIANGLE,            _T("triangle"), 0);
  AddMacroDef(ltTTFAMILY,            _T("ttfamily"), 1);
  AddMacroDef(ltTT,                  _T("tt"), 1);
  AddMacroDef(ltTYPEIN,              _T("typein"), 1);
  AddMacroDef(ltTYPEOUT,             _T("typeout"), 1);
  AddMacroDef(ltTWOCOLWIDTHA,        _T("twocolwidtha"), 1);
  AddMacroDef(ltTWOCOLWIDTHB,        _T("twocolwidthb"), 1);
  AddMacroDef(ltTWOCOLSPACING,       _T("twocolspacing"), 1);
  AddMacroDef(ltTWOCOLITEMRULED,     _T("twocolitemruled"), 2);
  AddMacroDef(ltTWOCOLITEM,          _T("twocolitem"), 2);
  AddMacroDef(ltTWOCOLLIST,          _T("twocollist"), 1);
  AddMacroDef(ltTWOCOLUMN,           _T("twocolumn"), 0);
  AddMacroDef(ltTHEPAGE,             _T("thepage"), 0);
  AddMacroDef(ltTHECHAPTER,          _T("thechapter"), 0);
  AddMacroDef(ltTHESECTION,          _T("thesection"), 0);
  AddMacroDef(ltTHISPAGESTYLE,       _T("thispagestyle"), 1);

  AddMacroDef(ltUNDERLINE,           _T("underline"), 1);
  AddMacroDef(ltUPSILON,             _T("upsilon"), 0);
  AddMacroDef(ltCAP_UPSILON,         _T("Upsilon"), 0);
  AddMacroDef(ltUPARROW,             _T("uparrow"), 0);
  AddMacroDef(ltUPARROW2,            _T("Uparrow"), 0);
  AddMacroDef(ltUPPERCASE,           _T("uppercase"), 1);
  AddMacroDef(ltUPSHAPE,             _T("upshape"), 1);
  AddMacroDef(ltURLREF,              _T("urlref"), 2);
  AddMacroDef(ltUSEPACKAGE,          _T("usepackage"), 1);

  AddMacroDef(ltVAREPSILON,          _T("varepsilon"), 0);
  AddMacroDef(ltVARPHI,              _T("varphi"), 0);
  AddMacroDef(ltVARPI,               _T("varpi"), 0);
  AddMacroDef(ltVARRHO,              _T("varrho"), 0);
  AddMacroDef(ltVARSIGMA,            _T("varsigma"), 0);
  AddMacroDef(ltVARTHETA,            _T("vartheta"), 0);
  AddMacroDef(ltVDOTS,               _T("vdots"), 0);
  AddMacroDef(ltVEE,                 _T("vee"), 0);
  AddMacroDef(ltVERBATIMINPUT,       _T("verbatiminput"), 1);
  AddMacroDef(ltVERBATIM,            _T("verbatim"), 1);
  AddMacroDef(ltVERBSTAR,            _T("verb*"), 1);
  AddMacroDef(ltVERB,                _T("verb"), 1);
  AddMacroDef(ltVERSE,               _T("verse"), 1);
  AddMacroDef(ltVFILL,               _T("vfill"), 0);
  AddMacroDef(ltVLINE,               _T("vline"), 0);
  AddMacroDef(ltVOID,                _T("void"), 0);
  AddMacroDef(ltVDASH,               _T("vdash"), 0);
  AddMacroDef(ltVRULE,               _T("vrule"), 0);
  AddMacroDef(ltVSPACESTAR,          _T("vspace*"), 1);
  AddMacroDef(ltVSKIPSTAR,           _T("vskip*"), 1);
  AddMacroDef(ltVSPACE,              _T("vspace"), 1);
  AddMacroDef(ltVSKIP,               _T("vskip"), 1);

  AddMacroDef(ltWEDGE,               _T("wedge"), 0);
  AddMacroDef(ltWXCLIPS,             _T("wxclips"), 0);
  AddMacroDef(ltWINHELPIGNORE,       _T("winhelpignore"), 1);
  AddMacroDef(ltWINHELPONLY,         _T("winhelponly"), 1);
  AddMacroDef(ltWP,                  _T("wp"), 0);

  AddMacroDef(ltXI,                  _T("xi"), 0);
  AddMacroDef(ltCAP_XI,              _T("Xi"), 0);
  AddMacroDef(ltXLPIGNORE,           _T("xlpignore"), 1);
  AddMacroDef(ltXLPONLY,             _T("xlponly"), 1);

  AddMacroDef(ltZETA,                _T("zeta"), 0);

  AddMacroDef(ltSPACE,               _T(" "), 0);
  AddMacroDef(ltBACKSLASHCHAR,       _T("\\"), 0);
  AddMacroDef(ltPIPE,                _T("|"), 0);
  AddMacroDef(ltFORWARDSLASH,        _T("/"), 0);
  AddMacroDef(ltUNDERSCORE,          _T("_"), 0);
  AddMacroDef(ltAMPERSAND,           _T("&"), 0);
  AddMacroDef(ltPERCENT,             _T("%"), 0);
  AddMacroDef(ltDOLLAR,              _T("$"), 0);
  AddMacroDef(ltHASH,                _T("#"), 0);
  AddMacroDef(ltLPARENTH,            _T("("), 0);
  AddMacroDef(ltRPARENTH,            _T(")"), 0);
  AddMacroDef(ltLBRACE,              _T("{"), 0);
  AddMacroDef(ltRBRACE,              _T("}"), 0);
//  AddMacroDef(ltEQUALS,              _T("="), 0);
  AddMacroDef(ltRANGLEBRA,           _T(">"), 0);
  AddMacroDef(ltLANGLEBRA,           _T("<"), 0);
  AddMacroDef(ltPLUS,                _T("+"), 0);
  AddMacroDef(ltDASH,                _T("-"), 0);
  AddMacroDef(ltAT_SYMBOL,           _T("@"), 0);
//  AddMacroDef(ltSINGLEQUOTE,         _T("'"), 0);
//  AddMacroDef(ltBACKQUOTE,           _T("`"), 0);
}

/*
 * Default behaviour, should be called by client if can't match locally.
 *
 */

// Called on start/end of macro examination
void DefaultOnMacro(int macroId, int no_args, bool start)
{
  switch (macroId)
  {
    // Default behaviour for abstract
    case ltABSTRACT:
    {
      if (start)
      {
        // Write the heading
        FakeCurrentSection(AbstractNameString);
        OnMacro(ltPAR, 0, true);
        OnMacro(ltPAR, 0, false);
      }
      else
      {
        if (DocumentStyle == LATEX_ARTICLE)
          sectionNo --;
        else
          chapterNo --;
      }
      break;
    }

    // Default behaviour for glossary
    case ltHELPGLOSSARY:
    {
      if (start)
      {
        // Write the heading
        FakeCurrentSection(GlossaryNameString);
        OnMacro(ltPAR, 0, true);
        OnMacro(ltPAR, 0, false);
        if ((convertMode == TEX_RTF) && !winHelp)
        {
          OnMacro(ltPAR, 0, true);
          OnMacro(ltPAR, 0, false);
        }
      }
      break;
    }
    case ltSPECIALAMPERSAND:
      if (start)
        TexOutput(_T("  "));
      break;

    case ltCINSERT:
      if (start)
      {
        if (convertMode == TEX_HTML)
            TexOutput(_T("&lt;&lt;"));
        else
            TexOutput(_T("<<"), true);
      }
      break;
    case ltCEXTRACT:
      if (start)
      {
        if (convertMode == TEX_HTML)
            TexOutput(_T("&gt;&gt;"));
        else
            TexOutput(_T(">>"), true);
      }
      break;
    case ltDESTRUCT:
      if (start)
        TexOutput(_T("~"), true);
      break;
    case ltTILDE:
      if (start)
        TexOutput(_T("~"), true);
      break;
    case ltSPECIALTILDE:
      if (start)
        TexOutput(_T(" "), true);
      break;
    case ltUNDERSCORE:
      if (start)
        TexOutput(_T("_"), true);
      break;
    case ltHASH:
      if (start)
        TexOutput(_T("#"), true);
      break;
    case ltAMPERSAND:
      if (start)
        TexOutput(_T("&"), true);
      break;
    case ltSPACE:
      if (start)
        TexOutput(_T(" "), true);
      break;
    case ltPIPE:
      if (start)
        TexOutput(_T("|"), true);
      break;
    case ltPERCENT:
      if (start)
        TexOutput(_T("%"), true);
      break;
    case ltDOLLAR:
      if (start)
        TexOutput(_T("$"), true);
      break;
    case ltLPARENTH:
      if (start)
        TexOutput(_T(""), true);
      break;
    case ltRPARENTH:
      if (start)
        TexOutput(_T(""), true);
      break;
    case ltLBRACE:
      if (start)
        TexOutput(_T("{"), true);
      break;
    case ltRBRACE:
      if (start)
        TexOutput(_T("}"), true);
      break;
    case ltCOPYRIGHT:
      if (start)
        TexOutput(_T("(c)"), true);
      break;
    case ltREGISTERED:
      if (start)
        TexOutput(_T("(r)"), true);
      break;
    case ltBACKSLASH:
      if (start)
        TexOutput(_T("\\"), true);
      break;
    case ltLDOTS:
    case ltCDOTS:
      if (start)
        TexOutput(_T("..."), true);
      break;
    case ltVDOTS:
      if (start)
        TexOutput(_T("|"), true);
      break;
    case ltLATEX:
      if (start)
        TexOutput(_T("LaTeX"), true);
      break;
    case ltTEX:
      if (start)
        TexOutput(_T("TeX"), true);
      break;
    case ltPOUNDS:
      if (start)
        // FIXME: this is valid only if the output is iso-8859-1
        TexOutput(wxString::FromAscii("£"), true);
      break;
    case ltSPECIALDOUBLEDOLLAR:  // Interpret as center
      OnMacro(ltCENTER, no_args, start);
      break;
    case ltEMPH:
    case ltTEXTSL:
    case ltSLSHAPE:
    case ltSL:
      OnMacro(ltIT, no_args, start);
      break;
    case ltPARAGRAPH:
    case ltPARAGRAPHSTAR:
    case ltSUBPARAGRAPH:
    case ltSUBPARAGRAPHSTAR:
      OnMacro(ltSUBSUBSECTION, no_args, start);
      break;
    case ltTODAY:
    {
      if (start)
      {
        time_t when;
        (void) time(&when);
        TexOutput(wxCtime(&when), true);
      }
      break;
    }
    case ltNOINDENT:
      if (start)
        ParIndent = 0;
      break;

    // Symbols
    case ltALPHA:
      if (start) TexOutput(_T("alpha"));
      break;
    case ltBETA:
      if (start) TexOutput(_T("beta"));
      break;
    case ltGAMMA:
      if (start) TexOutput(_T("gamma"));
      break;
    case ltDELTA:
      if (start) TexOutput(_T("delta"));
      break;
    case ltEPSILON:
    case ltVAREPSILON:
      if (start) TexOutput(_T("epsilon"));
      break;
    case ltZETA:
      if (start) TexOutput(_T("zeta"));
      break;
    case ltETA:
      if (start) TexOutput(_T("eta"));
      break;
    case ltTHETA:
    case ltVARTHETA:
      if (start) TexOutput(_T("theta"));
      break;
    case ltIOTA:
      if (start) TexOutput(_T("iota"));
      break;
    case ltKAPPA:
      if (start) TexOutput(_T("kappa"));
      break;
    case ltLAMBDA:
      if (start) TexOutput(_T("lambda"));
      break;
    case ltMU:
      if (start) TexOutput(_T("mu"));
      break;
    case ltNU:
      if (start) TexOutput(_T("nu"));
      break;
    case ltXI:
      if (start) TexOutput(_T("xi"));
      break;
    case ltPI:
    case ltVARPI:
      if (start) TexOutput(_T("pi"));
      break;
    case ltRHO:
    case ltVARRHO:
      if (start) TexOutput(_T("rho"));
      break;
    case ltSIGMA:
    case ltVARSIGMA:
      if (start) TexOutput(_T("sigma"));
      break;
    case ltTAU:
      if (start) TexOutput(_T("tau"));
      break;
    case ltUPSILON:
      if (start) TexOutput(_T("upsilon"));
      break;
    case ltPHI:
    case ltVARPHI:
      if (start) TexOutput(_T("phi"));
      break;
    case ltCHI:
      if (start) TexOutput(_T("chi"));
      break;
    case ltPSI:
      if (start) TexOutput(_T("psi"));
      break;
    case ltOMEGA:
      if (start) TexOutput(_T("omega"));
      break;
    case ltCAP_GAMMA:
      if (start) TexOutput(_T("GAMMA"));
      break;
    case ltCAP_DELTA:
      if (start) TexOutput(_T("DELTA"));
      break;
    case ltCAP_THETA:
      if (start) TexOutput(_T("THETA"));
      break;
    case ltCAP_LAMBDA:
      if (start) TexOutput(_T("LAMBDA"));
      break;
    case ltCAP_XI:
      if (start) TexOutput(_T("XI"));
      break;
    case ltCAP_PI:
      if (start) TexOutput(_T("PI"));
      break;
    case ltCAP_SIGMA:
      if (start) TexOutput(_T("SIGMA"));
      break;
    case ltCAP_UPSILON:
      if (start) TexOutput(_T("UPSILON"));
      break;
    case ltCAP_PHI:
      if (start) TexOutput(_T("PHI"));
      break;
    case ltCAP_PSI:
      if (start) TexOutput(_T("PSI"));
      break;
    case ltCAP_OMEGA:
      if (start) TexOutput(_T("OMEGA"));
      break;

    // Binary operation symbols
    case ltLE:
    case ltLEQ:
      if (start)
      {
        if (convertMode == TEX_HTML)
            TexOutput(_T("&lt;="));
        else
            TexOutput(_T("<="));
      }
      break;
    case ltLL:
      if (start)
      {
        if (convertMode == TEX_HTML)
            TexOutput(_T("&lt;&lt;"));
        else
            TexOutput(_T("<<"));
      }
      break;
    case ltSUBSET:
      if (start) TexOutput(_T("SUBSET"));
      break;
    case ltSUBSETEQ:
      if (start) TexOutput(_T("SUBSETEQ"));
      break;
    case ltIN:
      if (start) TexOutput(_T("IN"));
      break;
    case ltVDASH:
      if (start) TexOutput(_T("VDASH"));
      break;
    case ltMODELS:
      if (start) TexOutput(_T("MODELS"));
      break;
    case ltGE:
    case ltGEQ:
    {
      if (start)
      {
        if (convertMode == TEX_HTML)
            TexOutput(_T("&gt;="));
        else
            TexOutput(_T(">="));
      }
      break;
    }
    case ltGG:
      if (start)
      {
        if (convertMode == TEX_HTML)
            TexOutput(_T("&gt;&gt;"));
        else
            TexOutput(_T(">>"));
      }
      break;
    case ltSUPSET:
      if (start) TexOutput(_T("SUPSET"));
      break;
    case ltSUPSETEQ:
      if (start) TexOutput(_T("SUPSETEQ"));
      break;
    case ltNI:
      if (start) TexOutput(_T("NI"));
      break;
    case ltDASHV:
      if (start) TexOutput(_T("DASHV"));
      break;
    case ltPERP:
      if (start) TexOutput(_T("PERP"));
      break;
    case ltNEQ:
      if (start) TexOutput(_T("NEQ"));
      break;
    case ltDOTEQ:
      if (start) TexOutput(_T("DOTEQ"));
      break;
    case ltAPPROX:
      if (start) TexOutput(_T("APPROX"));
      break;
    case ltCONG:
      if (start) TexOutput(_T("CONG"));
      break;
    case ltEQUIV:
      if (start) TexOutput(_T("EQUIV"));
      break;
    case ltPROPTO:
      if (start) TexOutput(_T("PROPTO"));
      break;
    case ltPREC:
      if (start) TexOutput(_T("PREC"));
      break;
    case ltPRECEQ:
      if (start) TexOutput(_T("PRECEQ"));
      break;
    case ltPARALLEL:
      if (start) TexOutput(_T("|"));
      break;
    case ltSIM:
      if (start) TexOutput(_T("~"));
      break;
    case ltSIMEQ:
      if (start) TexOutput(_T("SIMEQ"));
      break;
    case ltASYMP:
      if (start) TexOutput(_T("ASYMP"));
      break;
    case ltSMILE:
      if (start) TexOutput(_T(":-)"));
      break;
    case ltFROWN:
      if (start) TexOutput(_T(":-("));
      break;
    case ltSUCC:
      if (start) TexOutput(_T("SUCC"));
      break;
    case ltSUCCEQ:
      if (start) TexOutput(_T("SUCCEQ"));
      break;
    case ltMID:
      if (start) TexOutput(_T("|"));
      break;

    // Negated relation symbols
    case ltNOTEQ:
      if (start) TexOutput(_T("!="));
      break;
    case ltNOTIN:
      if (start) TexOutput(_T("NOTIN"));
      break;
    case ltNOTSUBSET:
      if (start) TexOutput(_T("NOTSUBSET"));
      break;

    // Arrows
    case ltLEFTARROW:
      if (start)
      {
        if (convertMode == TEX_HTML)
            TexOutput(_T("&lt;--"));
        else
            TexOutput(_T("<--"));
      }
      break;
    case ltLEFTARROW2:
      if (start)
      {
        if (convertMode == TEX_HTML)
            TexOutput(_T("&lt;=="));
        else
            TexOutput(_T("<=="));
      }
      break;
    case ltRIGHTARROW:
      if (start)
      {
        if (convertMode == TEX_HTML)
            TexOutput(_T("--&gt;"));
        else
            TexOutput(_T("-->"));
      }
      break;
    case ltRIGHTARROW2:
      if (start)
      {
        if (convertMode == TEX_HTML)
            TexOutput(_T("==&gt;"));
        else
            TexOutput(_T("==>"));
      }
      break;
    case ltLEFTRIGHTARROW:
      if (start)
      {
        if (convertMode == TEX_HTML)
            TexOutput(_T("&lt;--&gt;"));
        else
            TexOutput(_T("<-->"));
      }
      break;
    case ltLEFTRIGHTARROW2:
      if (start)
      {
        if (convertMode == TEX_HTML)
            TexOutput(_T("&lt;==&gt;"));
        else
            TexOutput(_T("<==>"));
      }
      break;
    case ltUPARROW:
      if (start) TexOutput(_T("UPARROW"));
      break;
    case ltUPARROW2:
      if (start) TexOutput(_T("UPARROW2"));
      break;
    case ltDOWNARROW:
      if (start) TexOutput(_T("DOWNARROW"));
      break;
    case ltDOWNARROW2:
      if (start) TexOutput(_T("DOWNARROW2"));
      break;
    // Miscellaneous symbols
    case ltALEPH:
      if (start) TexOutput(_T("ALEPH"));
      break;
    case ltWP:
      if (start) TexOutput(_T("WP"));
      break;
    case ltRE:
      if (start) TexOutput(_T("RE"));
      break;
    case ltIM:
      if (start) TexOutput(_T("IM"));
      break;
    case ltEMPTYSET:
      if (start) TexOutput(_T("EMPTYSET"));
      break;
    case ltNABLA:
      if (start) TexOutput(_T("NABLA"));
      break;
    case ltSURD:
      if (start) TexOutput(_T("SURD"));
      break;
    case ltPARTIAL:
      if (start) TexOutput(_T("PARTIAL"));
      break;
    case ltBOT:
      if (start) TexOutput(_T("BOT"));
      break;
    case ltFORALL:
      if (start) TexOutput(_T("FORALL"));
      break;
    case ltEXISTS:
      if (start) TexOutput(_T("EXISTS"));
      break;
    case ltNEG:
      if (start) TexOutput(_T("NEG"));
      break;
    case ltSHARP:
      if (start) TexOutput(_T("SHARP"));
      break;
    case ltANGLE:
      if (start) TexOutput(_T("ANGLE"));
      break;
    case ltTRIANGLE:
      if (start) TexOutput(_T("TRIANGLE"));
      break;
    case ltCLUBSUIT:
      if (start) TexOutput(_T("CLUBSUIT"));
      break;
    case ltDIAMONDSUIT:
      if (start) TexOutput(_T("DIAMONDSUIT"));
      break;
    case ltHEARTSUIT:
      if (start) TexOutput(_T("HEARTSUIT"));
      break;
    case ltSPADESUIT:
      if (start) TexOutput(_T("SPADESUIT"));
      break;
    case ltINFTY:
      if (start) TexOutput(_T("INFTY"));
      break;
    case ltPM:
      if (start) TexOutput(_T("PM"));
      break;
    case ltMP:
      if (start) TexOutput(_T("MP"));
      break;
    case ltTIMES:
      if (start) TexOutput(_T("TIMES"));
      break;
    case ltDIV:
      if (start) TexOutput(_T("DIV"));
      break;
    case ltCDOT:
      if (start) TexOutput(_T("CDOT"));
      break;
    case ltAST:
      if (start) TexOutput(_T("AST"));
      break;
    case ltSTAR:
      if (start) TexOutput(_T("STAR"));
      break;
    case ltCAP:
      if (start) TexOutput(_T("CAP"));
      break;
    case ltCUP:
      if (start) TexOutput(_T("CUP"));
      break;
    case ltVEE:
      if (start) TexOutput(_T("VEE"));
      break;
    case ltWEDGE:
      if (start) TexOutput(_T("WEDGE"));
      break;
    case ltCIRC:
      if (start) TexOutput(_T("CIRC"));
      break;
    case ltBULLET:
      if (start) TexOutput(_T("BULLET"));
      break;
    case ltDIAMOND:
      if (start) TexOutput(_T("DIAMOND"));
      break;
    case ltOSLASH:
      if (start) TexOutput(_T("OSLASH"));
      break;
    case ltBOX:
      if (start) TexOutput(_T("BOX"));
      break;
    case ltDIAMOND2:
      if (start) TexOutput(_T("DIAMOND2"));
      break;
    case ltBIGTRIANGLEDOWN:
      if (start) TexOutput(_T("BIGTRIANGLEDOWN"));
      break;
    case ltOPLUS:
      if (start) TexOutput(_T("OPLUS"));
      break;
    case ltOTIMES:
      if (start) TexOutput(_T("OTIMES"));
      break;
    case ltSS:
      if (start) TexOutput(_T("s"));
      break;
    case ltBACKSLASHRAW:
      if (start) TexOutput(_T("\\"));
      break;
    case ltLBRACERAW:
      if (start) TexOutput(_T("{"));
      break;
    case ltRBRACERAW:
      if (start) TexOutput(_T("}"));
      break;
    case ltSMALLSPACE1:
    case ltSMALLSPACE2:
      if (start) TexOutput(_T(" "));
      break;
    default:
      break;
  }
}

// Called on start/end of argument examination
bool DefaultOnArgument(int macroId, int arg_no, bool start)
{
  switch (macroId)
  {
    case ltREF:
    {
    if (arg_no == 1 && start)
    {
      wxChar *refName = GetArgData();
      if (refName)
      {
        TexRef *texRef = FindReference(refName);
        if (texRef)
        {
          // Must strip the 'section' or 'chapter' or 'figure' text
          // from a normal 'ref' reference
          wxChar buf[150];
          wxStrcpy(buf, texRef->sectionNumber);
          int len = wxStrlen(buf);
          int i = 0;
          if (wxStrcmp(buf, _T("??")) != 0)
          {
            while (i < len)
            {
              if (buf[i] == ' ')
              {
                i ++;
                break;
              }
              else i ++;
            }
          }
          TexOutput(texRef->sectionNumber + i, true);
        }
        else
        {
           wxString informBuf;
           informBuf.Printf(_T("Warning: unresolved reference '%s'"), refName);
           OnInform((wxChar *)informBuf.c_str());
        }
      }
      else TexOutput(_T("??"), true);
      return false;
    }
    break;
    }
    case ltLABEL:
    {
      return false;
    }
    case ltAUTHOR:
    {
      if (start && (arg_no == 1))
        DocumentAuthor = GetArgChunk();
      return false;
    }
    case ltDATE:
    {
      if (start && (arg_no == 1))
        DocumentDate = GetArgChunk();
      return false;
    }
    case ltTITLE:
    {
      if (start && (arg_no == 1))
        DocumentTitle = GetArgChunk();
      return false;
    }
  case ltDOCUMENTCLASS:
  case ltDOCUMENTSTYLE:
  {
    if (start && !IsArgOptional())
    {
      DocumentStyleString = copystring(GetArgData());
      if (wxStrncmp(DocumentStyleString, _T("art"), 3) == 0)
        DocumentStyle = LATEX_ARTICLE;
      else if (wxStrncmp(DocumentStyleString, _T("rep"), 3) == 0)
        DocumentStyle = LATEX_REPORT;
      else if (wxStrncmp(DocumentStyleString, _T("book"), 4) == 0 ||
               wxStrncmp(DocumentStyleString, _T("thesis"), 6) == 0)
        DocumentStyle = LATEX_BOOK;
      else if (wxStrncmp(DocumentStyleString, _T("letter"), 6) == 0)
        DocumentStyle = LATEX_LETTER;
      else if (wxStrncmp(DocumentStyleString, _T("slides"), 6) == 0)
        DocumentStyle = LATEX_SLIDES;

      if (StringMatch(_T("10"), DocumentStyleString))
        SetFontSizes(10);
      else if (StringMatch(_T("11"), DocumentStyleString))
        SetFontSizes(11);
      else if (StringMatch(_T("12"), DocumentStyleString))
        SetFontSizes(12);

      OnMacro(ltHELPFONTSIZE, 1, true);
      wxSnprintf(currentArgData, 2000, _T("%d"), normalFont);
      haveArgData = true;
      OnArgument(ltHELPFONTSIZE, 1, true);
      OnArgument(ltHELPFONTSIZE, 1, false);
      haveArgData = false;
      OnMacro(ltHELPFONTSIZE, 1, false);
    }
    else if (start && IsArgOptional())
    {
      MinorDocumentStyleString = copystring(GetArgData());

      if (StringMatch(_T("10"), MinorDocumentStyleString))
        SetFontSizes(10);
      else if (StringMatch(_T("11"), MinorDocumentStyleString))
        SetFontSizes(11);
      else if (StringMatch(_T("12"), MinorDocumentStyleString))
        SetFontSizes(12);
    }
    return false;
  }
  case ltBIBLIOGRAPHYSTYLE:
  {
    if (start && !IsArgOptional())
      BibliographyStyleString = copystring(GetArgData());
    return false;
  }
  case ltPAGESTYLE:
  {
    if (start && !IsArgOptional())
    {
      if (PageStyle) delete[] PageStyle;
      PageStyle = copystring(GetArgData());
    }
    return false;
  }
/*
  case ltLHEAD:
  {
    if (start && !IsArgOptional())
      LeftHeader = GetArgChunk();
    return false;
    break;
  }
  case ltLFOOT:
  {
    if (start && !IsArgOptional())
      LeftFooter = GetArgChunk();
    return false;
    break;
  }
  case ltCHEAD:
  {
    if (start && !IsArgOptional())
      CentreHeader = GetArgChunk();
    return false;
    break;
  }
  case ltCFOOT:
  {
    if (start && !IsArgOptional())
      CentreFooter = GetArgChunk();
    return false;
    break;
  }
  case ltRHEAD:
  {
    if (start && !IsArgOptional())
      RightHeader = GetArgChunk();
    return false;
    break;
  }
  case ltRFOOT:
  {
    if (start && !IsArgOptional())
      RightFooter = GetArgChunk();
    return false;
    break;
  }
*/
  case ltCITE:
  case ltSHORTCITE:
  {
    if (start && !IsArgOptional())
    {
      wxChar *citeKeys = GetArgData();
      int pos = 0;
      wxChar *citeKey = ParseMultifieldString(citeKeys, &pos);
      while (citeKey)
      {
        AddCitation(citeKey);
        TexRef *ref = FindReference(citeKey);
        if (ref)
        {
          TexOutput(ref->sectionNumber, true);
          if (wxStrcmp(ref->sectionNumber, _T("??")) == 0)
          {
            wxString informBuf;
            informBuf.Printf(_T("Warning: unresolved citation %s."), citeKey);
            OnInform((wxChar *)informBuf.c_str());
          }
        }
        citeKey = ParseMultifieldString(citeKeys, &pos);
        if (citeKey)
        {
          TexOutput(_T(", "), true);
        }
      }
      return false;
    }
    break;
  }
  case ltNOCITE:
  {
    if (start && !IsArgOptional())
    {
      wxChar *citeKey = GetArgData();
      AddCitation(citeKey);
      return false;
    }
    break;
  }
  case ltHELPFONTSIZE:
  {
    if (start)
    {
      wxChar *data = GetArgData();
      if (wxStrcmp(data, _T("10")) == 0)
        SetFontSizes(10);
      else if (wxStrcmp(data, _T("11")) == 0)
        SetFontSizes(11);
      else if (wxStrcmp(data, _T("12")) == 0)
        SetFontSizes(12);
      return false;
    }
    break;
  }
  case ltPAGEREF:
  {
    if (start)
    {
      TexOutput(_T(" ??"), true);
      return false;
    }
    break;
  }
  case ltPARSKIP:
  {
    if (start && arg_no == 1)
    {
      wxChar *data = GetArgData();
      ParSkip = ParseUnitArgument(data);
      return false;
    }
    break;
  }
  case ltPARINDENT:
  {
    if (start && arg_no == 1)
    {
      wxChar *data = GetArgData();
      ParIndent = ParseUnitArgument(data);
      return false;
    }
    break;
  }
  case ltSL:
  {
    return OnArgument(ltIT, arg_no, start);
  }
  case ltSPECIALDOUBLEDOLLAR:
  {
    return OnArgument(ltCENTER, arg_no, start);
  }
  case ltPARAGRAPH:
  case ltPARAGRAPHSTAR:
  case ltSUBPARAGRAPH:
  case ltSUBPARAGRAPHSTAR:
  {
    return OnArgument(ltSUBSUBSECTION, arg_no, start);
  }
  case ltTYPEOUT:
  {
    if (start)
      OnInform(GetArgData());
    break;
  }
  case ltFOOTNOTE:
  {
    if (start)
      TexOutput(_T(" ("), true);
    else
      TexOutput(_T(")"), true);
    break;
  }
  case ltBIBLIOGRAPHY:
  {
    if (start)
    {
      int ch;
      wxChar smallBuf[2];
      smallBuf[1] = 0;
      FILE *fd = wxFopen(TexBibName, _T("r"));
      if (fd)
      {
        ch = getc(fd);
        smallBuf[0] = (wxChar)ch;
        while (ch != EOF)
        {
          TexOutput(smallBuf);
          ch = getc(fd);
          smallBuf[0] = (wxChar)ch;
        }
        fclose(fd);
      }
      else
      {
        OnInform(_T("Run Tex2RTF again to include bibliography."));
      }

      // Read in the .bib file, resolve all known references, write out the RTF.
      wxChar *allFiles = GetArgData();
      int pos = 0;
      wxChar *bibFile = ParseMultifieldString(allFiles, &pos);
      while (bibFile)
      {
        wxChar fileBuf[300];
        wxStrcpy(fileBuf, bibFile);
        wxString actualFile = TexPathList.FindValidPath(fileBuf);
        if (actualFile.empty())
        {
          wxStrcat(fileBuf, _T(".bib"));
          actualFile = TexPathList.FindValidPath(fileBuf);
        }
        if (!actualFile.empty())
        {
          if (!ReadBib((wxChar*) (const wxChar*) actualFile))
          {
            wxString errBuf;
            errBuf.Printf(_T(".bib file %s not found or malformed"), (const wxChar*) actualFile);
            OnError((wxChar *)errBuf.c_str());
          }
        }
        else
        {
          wxString errBuf;
          errBuf.Printf(_T(".bib file %s not found"), fileBuf);
          OnError((wxChar *)errBuf.c_str());
        }
        bibFile = ParseMultifieldString(allFiles, &pos);
      }

      ResolveBibReferences();

      // Write it a new bib section in the appropriate format.
      FILE *save1 = CurrentOutput1;
      FILE *save2 = CurrentOutput2;
      FILE *Biblio = wxFopen(TexTmpBibName, _T("w"));
      SetCurrentOutput(Biblio);
      OutputBib();
      fclose(Biblio);
      if (wxFileExists(TexTmpBibName))
      {
        if (wxFileExists(TexBibName)) wxRemoveFile(TexBibName);
        wxRenameFile(TexTmpBibName, TexBibName);
      }
      SetCurrentOutputs(save1, save2);
      return false;
    }
    break;
  }
  case ltMULTICOLUMN:
    return (start && (arg_no == 3));
  case ltSCSHAPE:
  case ltTEXTSC:
  case ltSC:
  {
    if (start && (arg_no == 1))
    {
      wxChar *s = GetArgData();
      if (s)
      {
        wxChar *s1 = copystring(s);
        int i;
        for (i = 0; i < (int)wxStrlen(s); i++)
          s1[i] = (wxChar)wxToupper(s[i]);
        TexOutput(s1);
        delete[] s1;
        return false;
      }
      else return true;

    }
    return true;
  }
  case ltLOWERCASE:
  {
    if (start && (arg_no == 1))
    {
      wxChar *s = GetArgData();
      if (s)
      {
        wxChar *s1 = copystring(s);
        int i;
        for (i = 0; i < (int)wxStrlen(s); i++)
          s1[i] = (wxChar)wxTolower(s[i]);
        TexOutput(s1);
        delete[] s1;
        return false;
      }
      else return true;

    }
    return true;
  }
  case ltUPPERCASE:
  {
    if (start && (arg_no == 1))
    {
      wxChar *s = GetArgData();
      if (s)
      {
        wxChar *s1 = copystring(s);
        int i;
        for (i = 0; i < (int)wxStrlen(s); i++)
          s1[i] = (wxChar)wxToupper(s[i]);
        TexOutput(s1);
        delete[] s1;
        return false;
      }
      else return true;

    }
    return true;
  }
  case ltPOPREF:  // Ignore second argument by default
    return (start && (arg_no == 1));
  case ltTWOCOLUMN:
    return true;
  case ltXLPIGNORE:
    return ((convertMode == TEX_XLP) ? false : true);
  case ltXLPONLY:
    return ((convertMode != TEX_XLP) ? false : true);
  case ltHTMLIGNORE:
    return ((convertMode == TEX_HTML) ? false : true);
  case ltHTMLONLY:
    return ((convertMode != TEX_HTML) ? false : true);
  case ltRTFIGNORE:
    return (((convertMode == TEX_RTF) && !winHelp) ? false : true);
  case ltRTFONLY:
    return (!((convertMode == TEX_RTF) && !winHelp) ? false : true);
  case ltWINHELPIGNORE:
    return (winHelp ? false : true);
  case ltWINHELPONLY:
    return (!winHelp ? false : true);
  case ltLATEXIGNORE:
    return true;
  case ltLATEXONLY:
    return false;
  case ltCLINE:
  case ltARABIC:
  case ltALPH1:
  case ltALPH2:
  case ltROMAN:
  case ltROMAN2:
  case ltSETCOUNTER:
  case ltADDTOCOUNTER:
  case ltADDCONTENTSLINE:
  case ltNEWCOUNTER:
  case ltTEXTWIDTH:
  case ltTEXTHEIGHT:
  case ltBASELINESKIP:
  case ltVSPACESTAR:
  case ltHSPACESTAR:
  case ltVSPACE:
  case ltHSPACE:
  case ltVSKIPSTAR:
  case ltHSKIPSTAR:
  case ltVSKIP:
  case ltHSKIP:
  case ltPAGENUMBERING:
  case ltTHEPAGE:
  case ltTHECHAPTER:
  case ltTHESECTION:
  case ltITEMSEP:
  case ltFANCYPLAIN:
  case ltCHEAD:
  case ltRHEAD:
  case ltLHEAD:
  case ltCFOOT:
  case ltRFOOT:
  case ltLFOOT:
  case ltTHISPAGESTYLE:
  case ltMARKRIGHT:
  case ltMARKBOTH:
  case ltEVENSIDEMARGIN:
  case ltODDSIDEMARGIN:
  case ltMARGINPAR:
  case ltMARGINPARWIDTH:
  case ltMARGINPARSEP:
  case ltMARGINPAREVEN:
  case ltMARGINPARODD:
  case ltTWOCOLWIDTHA:
  case ltTWOCOLWIDTHB:
  case ltTWOCOLSPACING:
  case ltSETHEADER:
  case ltSETFOOTER:
  case ltINDEX:
  case ltITEM:
  case ltBCOL:
  case ltFCOL:
  case ltSETHOTSPOTCOLOUR:
  case ltSETHOTSPOTCOLOR:
  case ltSETHOTSPOTUNDERLINE:
  case ltSETTRANSPARENCY:
  case ltUSEPACKAGE:
  case ltBACKGROUND:
  case ltBACKGROUNDCOLOUR:
  case ltBACKGROUNDIMAGE:
  case ltLINKCOLOUR:
  case ltFOLLOWEDLINKCOLOUR:
  case ltTEXTCOLOUR:
  case ltIMAGE:
  case ltIMAGEMAP:
  case ltIMAGEL:
  case ltIMAGER:
  case ltPOPREFONLY:
  case ltINSERTATLEVEL:
    return false;
  case ltTABULAR:
  case ltSUPERTABULAR:
  case ltINDENTED:
  case ltSIZEDBOX:
  case ltSIZEDBOXD:
    return (arg_no == 2);
  case ltDEFINECOLOUR:
  case ltDEFINECOLOR:
  {
    static int redVal = 0;
    static int greenVal = 0;
    static int blueVal = 0;
    static wxChar *colourName = NULL;
    if (start)
    {
      switch (arg_no)
      {
        case 1:
        {
          if (colourName) delete[] colourName;
          colourName = copystring(GetArgData());
          break;
        }
        case 2:
        {
          redVal = wxAtoi(GetArgData());
          break;
        }
        case 3:
        {
          greenVal = wxAtoi(GetArgData());
          break;
        }
        case 4:
        {
          blueVal = wxAtoi(GetArgData());
          AddColour(colourName, redVal, greenVal, blueVal);
          break;
        }
        default:
          break;
      }
    }
    return false;
  }
  case ltFIGURE:
  case ltFIGURESTAR:
  case ltNORMALBOX:
  case ltNORMALBOXD:
  default:
    return (!IsArgOptional());
  }
  return true;
}

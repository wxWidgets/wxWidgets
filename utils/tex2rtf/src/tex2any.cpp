/////////////////////////////////////////////////////////////////////////////
// Name:        tex2any.cpp
// Purpose:     Utilities for Latex conversion.
// Author:      Julian Smart
// Modified by:
// Created:     01/01/99
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

#include <ctype.h>
#include "tex2any.h"
#include <stdlib.h>
#include <time.h>

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
char *          PageStyle = copystring("plain");

int             DocumentStyle = LATEX_REPORT;
int             MinorDocumentStyle = 0;
wxPathList      TexPathList;
char *          BibliographyStyleString = copystring("plain");
char *          DocumentStyleString = copystring("report");
char *          MinorDocumentStyleString = NULL;
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
int             mirrorMargins = TRUE;
bool            winHelp = FALSE;  // Output in Windows Help format if TRUE, linear otherwise
bool            isInteractive = FALSE;
bool            runTwice = FALSE;
int             convertMode = TEX_RTF;
bool            headerRule = FALSE;
bool            footerRule = FALSE;
bool            compatibilityMode = FALSE; // If TRUE, maximum Latex compatibility
                                // (Quality of RTF generation deteriorate)
bool            generateHPJ; // Generate WinHelp Help Project file
char            *winHelpTitle = NULL; // Windows Help title
int             defaultTableColumnWidth = 2000;

int             labelIndentTab = 18;  // From left indent to item label (points)
int             itemIndentTab = 40;   // From left indent to item (points)

bool            useUpButton = TRUE;
int             htmlBrowseButtons = HTML_BUTTONS_TEXT;

bool            truncateFilenames = FALSE; // Truncate for DOS
int             winHelpVersion = 3; // WinHelp Version (3 for Windows 3.1, 4 for Win95)
bool            winHelpContents = FALSE; // Generate .cnt file for WinHelp 4
bool            htmlIndex = FALSE; // Generate .htx file for HTML
bool            htmlFrameContents = FALSE; // Use frames for HTML contents page
bool            useHeadingStyles = TRUE; // Insert \s1, s2 etc.
bool            useWord = TRUE; // Insert proper Word table of contents, etc etc
int             contentsDepth = 4; // Depth of Word table of contents
bool            indexSubsections = TRUE; // Index subsections in linear RTF
// Linear RTF method of including bitmaps. Can be "includepicture", "hex"
char            *bitmapMethod = copystring("includepicture");
bool            upperCaseNames = FALSE;
// HTML background and text colours
char            *backgroundImageString = NULL;
char            *backgroundColourString = copystring("255;255;255");
char            *textColourString = NULL;
char            *linkColourString = NULL;
char            *followedLinkColourString = NULL;
bool            combineSubSections = FALSE;
bool            htmlWorkshopFiles = FALSE;

/*
 * International support
 */

// Names to help with internationalisation
char *ContentsNameString = copystring("Contents");
char *AbstractNameString = copystring("Abstract");
char *GlossaryNameString = copystring("Glossary");
char *ReferencesNameString = copystring("References");
char *FiguresNameString = copystring("List of Figures");
char *TablesNameString = copystring("List of Tables");
char *FigureNameString = copystring("Figure");
char *TableNameString = copystring("Table");
char *IndexNameString = copystring("Index");
char *ChapterNameString = copystring("chapter");
char *SectionNameString = copystring("section");
char *SubsectionNameString = copystring("subsection");
char *SubsubsectionNameString = copystring("subsubsection");
char *UpNameString = copystring("Up");

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
int LineNumbers[15];
char *FileNames[15];
int CurrentInputIndex = 0;

char *TexFileRoot = NULL;
char *TexBibName = NULL;         // Bibliography output file name
char *TexTmpBibName = NULL;      // Temporary bibliography output file name
bool isSync = FALSE;             // If TRUE, should not yield to other processes.
bool stopRunning = FALSE;        // If TRUE, should abort.

static int currentColumn = 0;
char *currentArgData = NULL;
bool haveArgData = FALSE; // If TRUE, we're simulating the data.
TexChunk *currentArgument = NULL;
TexChunk *nextChunk = NULL;
bool isArgOptional = FALSE;
int noArgs = 0;

TexChunk *TopLevel = NULL;
// wxList MacroDefs(wxKEY_STRING);
wxHashTable MacroDefs(wxKEY_STRING);
wxStringList IgnorableInputFiles; // Ignorable \input files, e.g. psbox.tex
char *BigBuffer = NULL;  // For reading in large chunks of text
TexMacroDef *SoloBlockDef = NULL;
TexMacroDef *VerbatimMacroDef = NULL;

#define IncrementLineNumber() LineNumbers[CurrentInputIndex] ++

void TexOutput(char *s, bool ordinaryText)
{
  int len = strlen(s);

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
    fprintf(CurrentOutput1, "%s", s);
  if (CurrentOutput2)
    fprintf(CurrentOutput2, "%s", s);
}

/*
 * Try to find a Latex macro, in one of the following forms:
 * (1) \begin{} ... \end{}
 * (2) \macroname{arg1}...{argn}
 * (3) {\bf arg1}
 */

void ForbidWarning(TexMacroDef *def)
{
  char buf[100];
  switch (def->forbidden)
  {
    case FORBID_WARN:
    {
      sprintf(buf, "Warning: it is recommended that command %s is not used.", def->name);
      OnInform(buf);
      break;
    }
    case FORBID_ABSOLUTELY:
    {
      sprintf(buf, "Error: command %s cannot be used and will lead to errors.", def->name);
      OnInform(buf);
      break;
    }
    default:
      break;
  }
}
 
TexMacroDef *MatchMacro(char *buffer, int *pos, char **env, bool *parseToBrace)
{
  *parseToBrace = TRUE;
  int i = (*pos);
  TexMacroDef *def = NULL;
  char macroBuf[40];

  // First, try to find begin{thing}
  if (strncmp(buffer+i, "begin{", 6) == 0)
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
    else return NULL;
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

      *parseToBrace = FALSE;
    }
    *pos = i;
    ForbidWarning(def);
    return def;
  }
  return NULL;
}

void EatWhiteSpace(char *buffer, int *pos)
{
  int len = strlen(buffer);
  int j = *pos;
  bool keepGoing = TRUE;
  bool moreLines = TRUE;
  while ((j < len) && keepGoing &&
         (buffer[j] == 10 || buffer[j] == 13 || buffer[j] == ' ' || buffer[j] == 9))
  {
    j ++;
    if (j >= len)
    {
      if (moreLines)
      {
        moreLines = read_a_line(buffer);
        len = strlen(buffer);
        j = 0;
      }
      else
        keepGoing = FALSE;
    }
  }
  *pos = j;
}

bool FindEndEnvironment(char *buffer, int *pos, char *env)
{
  int i = (*pos);

  // Try to find end{thing}
  if ((strncmp(buffer+i, "end{", 4) == 0) &&
      (strncmp(buffer+i+4, env, strlen(env)) == 0))
  {
    *pos = i + 5 + strlen(env);
    return TRUE;
  }
  else return FALSE;
}

bool readingVerbatim = FALSE;
bool readInVerbatim = FALSE;  // Within a verbatim, but not nec. verbatiminput

// Switched this off because e.g. \verb${$ causes it to fail. There is no
// detection of \verb yet.
#define CHECK_BRACES 0

unsigned long leftCurly = 0;
unsigned long rightCurly = 0;
static wxString currentFileName = "";

bool read_a_line(char *buf)
{
  if (CurrentInputIndex < 0)
  {
    buf[0] = 0;
    return FALSE;
  }
  
  int ch = -2;
  int i = 0;
  buf[0] = 0;

  while (ch != EOF && ch != 10)
  {
    if (((i == 14) && (strncmp(buf, "\\end{verbatim}", 14) == 0)) ||
         ((i == 16) && (strncmp(buf, "\\end{toocomplex}", 16) == 0)))
      readInVerbatim = FALSE;

    ch = getc(Inputs[CurrentInputIndex]);

#if CHECK_BRACES
    if (ch == '{' && !readInVerbatim)
       leftCurly++;
    if (ch == '}' && !readInVerbatim)
    {
       rightCurly++;
       if (rightCurly > leftCurly)
       {
           wxString errBuf;
           errBuf.Printf("An extra right Curly brace ('}') was detected at line %l inside file %s",LineNumbers[CurrentInputIndex], (const char*) currentFileName.c_str());
           OnError((char *)errBuf.c_str());

           // Reduce the count of right curly braces, so the mismatched count
           // isn't reported on every line that has a '}' after the first mismatch
           rightCurly--;
       }
    }
#endif

    if (ch != EOF)
    {
      // Check for 2 consecutive newlines and replace with \par
      if (ch == 10 && !readInVerbatim)
      {
        int ch1 = getc(Inputs[CurrentInputIndex]);
        if ((ch1 == 10) || (ch1 == 13))
        {
          // Eliminate newline (10) following DOS linefeed
          if (ch1 == 13) ch1 = getc(Inputs[CurrentInputIndex]);
          buf[i] = 0;
          IncrementLineNumber();
//          strcat(buf, "\\par\n");
//          i += 6;
          strcat(buf, "\\par");
          i += 5;
        }
        else
        {
          ungetc(ch1, Inputs[CurrentInputIndex]);
          buf[i] = ch;
          i ++;
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
				buf[i++]='\\';
				buf[i++]='"';
				buf[i++]='{';
				switch(ch)
				{
					case 0xf6:buf[i++]='o';break; // ö
					case 0xe4:buf[i++]='a';break; // ä
					case 0xfc:buf[i++]='u';break; // ü
					case 0xd6:buf[i++]='O';break; // Ö
					case 0xc4:buf[i++]='A';break; // Ä
					case 0xdc:buf[i++]='U';break; // Ü					
				}				
				buf[i++]='}';
				break;
		case 0xdf: // ß 
			buf[i++]='\\';
			buf[i++]='s';
			buf[i++]='s';
            buf[i++]='\\';
            buf[i++]='/';
			break;	
		default:
			buf[i++] = ch;
			break;
		}
        
      }
    }
    else
    {
      buf[i] = 0;
      fclose(Inputs[CurrentInputIndex]);
      Inputs[CurrentInputIndex] = NULL;
      if (CurrentInputIndex > 0) 
         ch = ' '; // No real end of file
      CurrentInputIndex --;
#if CHECK_BRACES
      if (leftCurly != rightCurly)
      {
        wxString errBuf;
        errBuf.Printf("Curly braces do not match inside file %s\n%lu opens, %lu closes", (const char*) currentFileName.c_str(),leftCurly,rightCurly);
        OnError((char *)errBuf.c_str());
      }
      leftCurly = 0;
      rightCurly = 0;
#endif
      if (readingVerbatim)
      {
        readingVerbatim = FALSE;
        readInVerbatim = FALSE;
        strcat(buf, "\\end{verbatim}\n");
        return FALSE;
      }
    }
    if (ch == 10)
      IncrementLineNumber();
  }
  buf[i] = 0;

  // Strip out comment environment
  if (strncmp(buf, "\\begin{comment}", 15) == 0)
  {
    while (strncmp(buf, "\\end{comment}", 13) != 0)
      read_a_line(buf);
    return read_a_line(buf);
  }
  // Read a verbatim input file as if it were a verbatim environment
  else if (strncmp(buf, "\\verbatiminput", 14) == 0)
  {
    int wordLen = 14;
    char *fileName = buf + wordLen + 1;

    int j = i - 1;
    buf[j] = 0;

    // thing}\par -- eliminate the \par!
    if (strncmp((buf + strlen(buf)-5), "\\par", 4) == 0)
    {
      j -= 5;
      buf[j] = 0;
    }
    
    if (buf[j-1] == '}') buf[j-1] = 0; // Ignore final brace

    wxString actualFile = TexPathList.FindValidPath(fileName);
    currentFileName = actualFile;
    if (actualFile == "")
    {
      char errBuf[300];
      strcpy(errBuf, "Could not find file: ");
      strncat(errBuf, fileName, 100);
      OnError(errBuf);
    }
    else
    {
      wxString informStr;
      informStr.Printf("Processing: %s",actualFile.c_str());
      OnInform((char *)informStr.c_str());
      CurrentInputIndex ++;
      Inputs[CurrentInputIndex] = fopen(actualFile, "r");
      LineNumbers[CurrentInputIndex] = 1;
      if (FileNames[CurrentInputIndex])
        delete[] FileNames[CurrentInputIndex];
      FileNames[CurrentInputIndex] = copystring(actualFile);

      if (!Inputs[CurrentInputIndex])
      {
        CurrentInputIndex --;
        OnError("Could not open verbatiminput file.");
      }
      else
      {
        readingVerbatim = TRUE;
        readInVerbatim = TRUE;
        strcpy(buf, "\\begin{verbatim}\n");
        return FALSE;
      }
    }
    return FALSE;
  }
  else if (strncmp(buf, "\\input", 6) == 0 || strncmp(buf, "\\helpinput", 10) == 0 ||
      strncmp(buf, "\\include", 8) == 0)
  {
    int wordLen;
    if (strncmp(buf, "\\input", 6) == 0)
      wordLen = 6;
    else
    if (strncmp(buf, "\\include", 8) == 0)
      wordLen = 8;
    else
      wordLen = 10;

    char *fileName = buf + wordLen + 1;

    int j = i - 1;
    buf[j] = 0;

    // \input{thing}\par -- eliminate the \par!
//    if (strncmp((buf + strlen(buf)-5), "\\par", 4) == 0)
    if (strncmp((buf + strlen(buf)-4), "\\par", 4) == 0) // Bug fix 8/2/95 Ulrich Leodolter
    {
//      j -= 5;
      j -= 4; // Ditto
      buf[j] = 0;
    }

    if (buf[j-1] == '}') buf[j-1] = 0; // Ignore final brace

    // Ignore some types of input files (e.g. macro definition files)
    char *fileOnly = FileNameFromPath(fileName);
    currentFileName = fileOnly;
    if (IgnorableInputFiles.Member(fileOnly))
      return read_a_line(buf);

    wxString actualFile = TexPathList.FindValidPath(fileName);
    if (actualFile == "")
    {
      char buf2[400];
      sprintf(buf2, "%s.tex", fileName);
      actualFile = TexPathList.FindValidPath(buf2);
    }
    currentFileName = actualFile;

    if (actualFile == "")
    {
      char errBuf[300];
      strcpy(errBuf, "Could not find file: ");
      strncat(errBuf, fileName, 100);
      OnError(errBuf);
    }
    else
    {
      // Ensure that if this file includes another,
      // then we look in the same directory as this one.
      TexPathList.EnsureFileAccessible(actualFile);

      wxString informStr;
      informStr.Printf("Processing: %s",actualFile.c_str());
      OnInform((char *)informStr.c_str());
      CurrentInputIndex ++;
      Inputs[CurrentInputIndex] = fopen(actualFile, "r");
      LineNumbers[CurrentInputIndex] = 1;
      if (FileNames[CurrentInputIndex])
        delete[] FileNames[CurrentInputIndex];
      FileNames[CurrentInputIndex] = copystring(actualFile);

      if (!Inputs[CurrentInputIndex])
      {
        char errBuf[300];
        sprintf(errBuf, "Could not open include file %s", (const char*) actualFile);
        CurrentInputIndex --;
        OnError(errBuf);
      }
    }
    bool succ = read_a_line(buf);
    return succ;
  }
  if (strncmp(buf, "\\begin{verbatim}", 16) == 0 ||
      strncmp(buf, "\\begin{toocomplex}", 18) == 0)
    readInVerbatim = TRUE;
  else if (strncmp(buf, "\\end{verbatim}", 14) == 0 ||
           strncmp(buf, "\\end{toocomplex}", 16) == 0)
    readInVerbatim = FALSE;

#if CHECK_BRACES
  if (ch == EOF && leftCurly != rightCurly)
  {
    wxString errBuf;
    errBuf.Printf("Curly braces do not match inside file %s\n%lu opens, %lu closes", (const char*) currentFileName.c_str(),leftCurly,rightCurly);
    OnError((char *)errBuf.c_str());
  }
#endif

  return (ch == EOF);
}

/*
 * Parse newcommand
 *
 */

bool ParseNewCommand(char *buffer, int *pos)
{
  if ((strncmp((buffer+(*pos)), "newcommand", 10) == 0) ||
      (strncmp((buffer+(*pos)), "renewcommand", 12) == 0))
  {
    if (strncmp((buffer+(*pos)), "newcommand", 10) == 0)
      *pos = *pos + 12;
    else
      *pos = *pos + 14;

    char commandName[100];
    char commandValue[1000];
    int noArgs = 0;
    int i = 0;
    while (buffer[*pos] != '}' && (buffer[*pos] != 0))
    {
      commandName[i] = buffer[*pos];
      *pos += 1;
      i ++;
    }
    commandName[i] = 0;
    i = 0;
    *pos += 1;
    if (buffer[*pos] == '[')
    {
      *pos += 1;
      noArgs = (int)(buffer[*pos]) - 48;
      *pos += 2; // read past argument and '['
    }
    bool end = FALSE;
    int braceCount = 0;
    while (!end)
    {
      char ch = buffer[*pos];
      if (ch == '{')
        braceCount ++;
      else if (ch == '}')
      {
        braceCount --;
        if (braceCount == 0)
          end = TRUE;
      }
      else if (ch == 0)
      {
        if (!read_a_line(buffer))
          end = TRUE;
        *pos = 0;
        break;
      }
      commandValue[i] = ch;
      i ++;
      *pos += 1;
    }
    commandValue[i] = 0;

    CustomMacro *macro = new CustomMacro(commandName, noArgs, NULL);
    if (strlen(commandValue) > 0)
      macro->macroBody = copystring(commandValue);
    if (!CustomMacroList.Find(commandName))
    {
      CustomMacroList.Append(commandName, macro);
      AddMacroDef(ltCUSTOM_MACRO, commandName, noArgs);
    }
    return TRUE;
  }
  else return FALSE;
}

void MacroError(char *buffer)
{
  char errBuf[300];
  char macroBuf[200];
  macroBuf[0] = '\\';
  int i = 1;
  char ch;
  while (((ch = buffer[i-1]) != '\n') && (ch != 0))
  {
    macroBuf[i] = ch;
    i ++;
  }
  macroBuf[i] = 0;
  if (i > 20)
    macroBuf[20] = 0;

  sprintf(errBuf, "Could not find macro: %s at line %d, file %s",
             macroBuf, (int)(LineNumbers[CurrentInputIndex]-1), FileNames[CurrentInputIndex]);
  OnError(errBuf);
}

/*
 * Parse an argument.
 * 'environment' specifies the name of the macro IFF if we're looking for the end
 * of an environment, e.g. \end{itemize}. Otherwise it's NULL.
 * 'parseToBrace' is TRUE if the argument should extend to the next right brace,
 * e.g. in {\bf an argument} as opposed to \vskip 30pt
 *
 */
int ParseArg(TexChunk *thisArg, wxList& children, char *buffer, int pos, char *environment, bool parseToBrace, TexChunk *customMacroArgs)
{
  Tex2RTFYield();
  if (stopRunning) return pos;
  
  bool eof = FALSE;
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
      isOptional = TRUE;
      pos ++;
    }
    else if ((pos > 1) && (buffer[pos-1] != ' ') && (buffer[pos+1] == '[' || buffer[pos+1] == '('))
    {
      isOptional = TRUE;
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
      BigBuffer[buf_ptr] = ch;
      buf_ptr ++;
      pos ++;
      ch = buffer[pos];
    }
    if (buf_ptr > 0)
    {
      TexChunk *chunk = new TexChunk(CHUNK_TYPE_STRING);
      BigBuffer[buf_ptr] = 0;
      buf_ptr = 0;
      chunk->value = copystring(BigBuffer);
      children.Append((wxObject *)chunk);
    }
    return pos;
  }

  while (!eof)
  {
    len = strlen(buffer);
    if (pos >= len)
    {
      if (customMacroArgs) return 0;

      eof = read_a_line(buffer);
      pos = 0;
      len = strlen(buffer);
      // Check for verbatim (or toocomplex, which comes to the same thing)
      if (strncmp(buffer, "\\begin{verbatim}", 16) == 0 ||
          strncmp(buffer, "\\begin{toocomplex}", 18) == 0)
      {
        if (buf_ptr > 0)
        {
          TexChunk *chunk = new TexChunk(CHUNK_TYPE_STRING);
          BigBuffer[buf_ptr] = 0;
          buf_ptr = 0;
          chunk->value = copystring(BigBuffer);
          children.Append((wxObject *)chunk);
        }
        BigBuffer[0] = 0;
        buf_ptr = 0;

        eof = read_a_line(buffer);
        while (!eof && (strncmp(buffer, "\\end{verbatim}", 14) != 0) &&
                       (strncmp(buffer, "\\end{toocomplex}", 16) != 0)
               )
	{
          strcat(BigBuffer, buffer);
          buf_ptr += strlen(buffer);
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
          TexMacroDef *parDef = (TexMacroDef *)MacroDefs.Get("\\");
          TexChunk *parChunk = new TexChunk(CHUNK_TYPE_MACRO, parDef);
          parChunk->no_args = 0;
          parChunk->macroId = ltBACKSLASHCHAR;
          children.Append((wxObject *)parChunk);
        }
      }
    }

    char ch = buffer[pos];
    // End of optional argument -- pretend it's right brace for simplicity
    if (thisArg->optional && (ch == ']'))
      ch = '}';

    switch (ch)
    {
      case 0:
      case '}':  // End of argument
      {
        if (buf_ptr > 0)
        {
          TexChunk *chunk = new TexChunk(CHUNK_TYPE_STRING);
          BigBuffer[buf_ptr] = 0;
          buf_ptr = 0;
          chunk->value = copystring(BigBuffer);
          children.Append((wxObject *)chunk);
        }
        if (ch == '}') pos ++;
        return pos;
        break;
      }
      case '\\':
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

        if (strncmp(buffer+pos, "special", 7) == 0)
        {
          pos += 7;

          // Discard {
          pos ++;
          int noBraces = 1;

          wxBuffer[0] = 0;
          int i = 0;
          bool end = FALSE;
          while (!end)
          {
            int ch = buffer[pos];
            if (ch == '}')
            {
              noBraces --;
              if (noBraces == 0)
              {
                wxBuffer[i] = 0;
                end = TRUE;
              }
              else
              {
                wxBuffer[i] = '}';
                i ++;
              }
              pos ++;
            }
            else if (ch == '{')
            {
              wxBuffer[i] = '{';
              i ++;
              pos ++;
            }
            else if (ch == '\\' && buffer[pos+1] == '}')
            {
              wxBuffer[i] = '}';
              pos += 2;
              i++;
            }
            else if (ch == '\\' && buffer[pos+1] == '{')
            {
              wxBuffer[i] = '{';
              pos += 2;
              i++;
            }
            else
            {
              wxBuffer[i] = ch;
              pos ++;
              i ++;
              if (ch == 0)
                end = TRUE;
            }
          }
          TexChunk *chunk = new TexChunk(CHUNK_TYPE_MACRO);
          chunk->no_args = 1;
          chunk->macroId = ltSPECIAL;
          TexMacroDef *specialDef = (TexMacroDef *)MacroDefs.Get("special");
          chunk->def = specialDef;
          TexChunk *arg = new TexChunk(CHUNK_TYPE_ARG, specialDef);
          chunk->children.Append((wxObject *)arg);
          arg->argn = 1;
          arg->macroId = chunk->macroId;

          // The value in the first argument.
          TexChunk *argValue = new TexChunk(CHUNK_TYPE_STRING);
          arg->children.Append((wxObject *)argValue);
          argValue->argn = 1;
          argValue->value = copystring(wxBuffer);

          children.Append((wxObject *)chunk);
        }
        else if (strncmp(buffer+pos, "verb", 4) == 0)
        {
          pos += 4;
          if (buffer[pos] == '*')
            pos ++;

          // Find the delimiter character         
          int ch = buffer[pos];
          pos ++;
          // Now at start of verbatim text
          int j = pos;
          while ((buffer[pos] != ch) && buffer[pos] != 0)
            pos ++;
          char *val = new char[pos - j + 1];
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
          TexMacroDef *verbDef = (TexMacroDef *)MacroDefs.Get("verb");
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
          char *env = NULL;
          bool tmpParseToBrace = TRUE;
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
          if (env || strcmp(def->name, "\\") == 0)
            if (buffer[pos] == 13)
            {
              pos ++;
              if (buffer[pos] == 10)
                pos ++;
            }

          pos = ParseMacroBody(def->name, chunk, chunk->no_args,
                     buffer, pos, env, tmpParseToBrace, customMacroArgs);

          // If custom macro, parse the body substituting the above found args.
          if (customMacro)
          {
            if (customMacro->macroBody)
            {
              char macroBuf[300];
//              strcpy(macroBuf, "{");
              strcpy(macroBuf, customMacro->macroBody);
              strcat(macroBuf, "}");
              ParseArg(thisArg, children, macroBuf, 0, NULL, TRUE, chunk);
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
      case '{':
      {
        pos ++;
        if (buffer[pos] == '\\')
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

          char *env;
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
                       buffer, pos, NULL, TRUE, customMacroArgs);

            // If custom macro, parse the body substituting the above found args.
            if (customMacro)
            {
              if (customMacro->macroBody)
              {
                char macroBuf[300];
//                strcpy(macroBuf, "{");
                strcpy(macroBuf, customMacro->macroBody);
                strcat(macroBuf, "}");
                ParseArg(thisArg, children, macroBuf, 0, NULL, TRUE, chunk);
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
            SoloBlockDef = new TexMacroDef(ltSOLO_BLOCK, "solo block", 1, FALSE);
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

          pos = ParseArg(arg, arg->children, buffer, pos, NULL, TRUE, customMacroArgs);
	}
        break;
      }
      case '$':
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

        if (buffer[pos] == '$')
        {
          TexChunk *chunk = new TexChunk(CHUNK_TYPE_MACRO);
          chunk->no_args = 0;
//          chunk->name = copystring("$$");
          chunk->macroId = ltSPECIALDOUBLEDOLLAR;
          children.Append((wxObject *)chunk);
          pos ++;
        }
        else
        {
          TexChunk *chunk = new TexChunk(CHUNK_TYPE_MACRO);
          chunk->no_args = 0;
//          chunk->name = copystring("_$");
          chunk->macroId = ltSPECIALDOLLAR;
          children.Append((wxObject *)chunk);
        }
        break;
      }
      case '~':
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
//        chunk->name = copystring("_~");
        chunk->macroId = ltSPECIALTILDE;
        children.Append((wxObject *)chunk);
        break;
      }
      case '#': // Either treat as a special TeX character or as a macro arg
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
//          chunk->name = copystring("_#");
          chunk->macroId = ltSPECIALHASH;
          children.Append((wxObject *)chunk);
        }
        else
        {
          if (isdigit(buffer[pos]))
          {
            int n = buffer[pos] - 48;
            pos ++;
            wxNode *node = customMacroArgs->children.Nth(n-1);
            if (node)
            {
              TexChunk *argChunk = (TexChunk *)node->Data();
              children.Append((wxObject *)new TexChunk(*argChunk));
            }
          }
        }
        break;
      }
      case '&':
      {
        // Remove white space before and after the ampersand,
        // since this is probably a table column separator with
        // some convenient -- but useless -- white space in the text.
        while ((buf_ptr > 0) && ((BigBuffer[buf_ptr-1] == ' ') || (BigBuffer[buf_ptr-1] == 9)))
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

        while (buffer[pos] == ' ' || buffer[pos] == 9)
          pos ++;

        TexChunk *chunk = new TexChunk(CHUNK_TYPE_MACRO);
        chunk->no_args = 0;
//        chunk->name = copystring("_&");
        chunk->macroId = ltSPECIALAMPERSAND;
        children.Append((wxObject *)chunk);
        break;
      }
      // Eliminate end-of-line comment
      case '%':
      {
        ch = buffer[pos];
        while (ch != 10 && ch != 13 && ch != 0)
        {
          pos ++;
          ch = buffer[pos];
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
        BigBuffer[buf_ptr] = ' ';
        BigBuffer[buf_ptr+1] = 0;
        buf_ptr ++;
        pos ++;
        break;
      }
      default:
      {
        BigBuffer[buf_ptr] = ch;
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
 
int ParseMacroBody(char *macro_name, TexChunk *parent,
                   int no_args, char *buffer, int pos,
                   char *environment, bool parseToBrace,
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
    char *actualEnv;
    if ((no_args == 2) && (i == 0))
      actualEnv = NULL;
    else
      actualEnv = environment;

    bool isOptional = FALSE;

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
        isOptional = TRUE;
        pos ++;
      }
      else if ((pos > 1) && (buffer[pos-1] != ' ') && (buffer[pos+1] == '['))
      {
        isOptional = TRUE;
        pos += 2;
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
  wxNode *node = parent->children.First();
  while (node)
  {
    TexChunk *chunk = (TexChunk *)node->Data();
    chunk->no_args = maxArgs;
    node = node->Next();
  }
  return pos;
}

bool TexLoadFile(char *filename)
{
  stopRunning = FALSE;
  strcpy(TexFileRoot, filename);
  StripExtension(TexFileRoot);
  sprintf(TexBibName, "%s.bb", TexFileRoot);
  sprintf(TexTmpBibName, "%s.bb1", TexFileRoot);

  TexPathList.EnsureFileAccessible(filename);

#ifdef __WXMSW__
  static char *line_buffer = new char[600];
#else
  static char *line_buffer = new char[11000];
#endif
  
  Inputs[0] = fopen(filename, "r");
  LineNumbers[0] = 1;
  FileNames[0] = copystring(filename);
  if (Inputs[0])
  {
    read_a_line(line_buffer);
    ParseMacroBody("toplevel", TopLevel, 1, line_buffer, 0, NULL, TRUE);
    if (Inputs[0]) fclose(Inputs[0]);
    return TRUE;
  }
  else return FALSE;
}

TexMacroDef::TexMacroDef(int the_id, char *the_name, int n, bool ig, bool forbidLevel)
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
  optional = FALSE;
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
  wxNode *node = toCopy.children.First();
  while (node)
  {
    TexChunk *child = (TexChunk *)node->Data();
    children.Append((wxObject *)new TexChunk(*child));
    node = node->Next();
  }
}

TexChunk::~TexChunk(void)
{
//  if (name) delete[] name;
  if (value) delete[] value;
  wxNode *node = children.First();
  while (node)
  {
    TexChunk *child = (TexChunk *)node->Data();
    delete child;
    wxNode *next = node->Next();
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

      if (def && (strcmp(def->name, "solo block") != 0))
      {
        strcat(currentArgData, "\\");
        strcat(currentArgData, def->name);
      }

      wxNode *node = chunk->children.First();
      while (node)
      {
        TexChunk *child_chunk = (TexChunk *)node->Data();
        strcat(currentArgData, "{");
        GetArgData1(child_chunk);
        strcat(currentArgData, "}");
        node = node->Next();
      }
      break;
    }
    case CHUNK_TYPE_ARG:
    {
      wxNode *node = chunk->children.First();
      while (node)
      {
        TexChunk *child_chunk = (TexChunk *)node->Data();
        GetArgData1(child_chunk);
        node = node->Next();
      }
      break;
    }
    case CHUNK_TYPE_STRING:
    {
      if (chunk->value)
        strcat(currentArgData, chunk->value);
      break;
    }
  }
}

char *GetArgData(TexChunk *chunk)
{
  currentArgData[0] = 0;
  GetArgData1(currentArgument);
  haveArgData = FALSE;
  return currentArgData;
}

char *GetArgData(void)
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
        OnMacro(chunk->macroId, chunk->no_args, TRUE);

      wxNode *node = chunk->children.First();
      while (node)
      {
        TexChunk *child_chunk = (TexChunk *)node->Data();
        TraverseFromChunk(child_chunk, node);
        node = node->Next();
      }

      if (thisNode && thisNode->Next()) nextChunk = (TexChunk *)thisNode->Next()->Data();

      if (!childrenOnly)
        OnMacro(chunk->macroId, chunk->no_args, FALSE);
      break;
    }
    case CHUNK_TYPE_ARG:
    {
      currentArgument = chunk;

      isArgOptional = chunk->optional;
      noArgs = chunk->no_args;

      // If OnArgument returns FALSE, don't output.

      if (childrenOnly || OnArgument(chunk->macroId, chunk->argn, TRUE))
      {
        wxNode *node = chunk->children.First();
        while (node)
        {
          TexChunk *child_chunk = (TexChunk *)node->Data();
          TraverseFromChunk(child_chunk, node);
          node = node->Next();
        }
      }

      currentArgument = chunk;

      if (thisNode && thisNode->Next()) nextChunk = (TexChunk *)thisNode->Next()->Data();

      isArgOptional = chunk->optional;
      noArgs = chunk->no_args;

      if (!childrenOnly)
        (void)OnArgument(chunk->macroId, chunk->argn, FALSE);
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
          issuedNewParagraph = FALSE;
        TexOutput(chunk->value, TRUE);
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

void AddMacroDef(int the_id, char *name, int n, bool ignore, bool forbid)
{
  MacroDefs.Put(name, new TexMacroDef(the_id, name, n, ignore, forbid));
}

void TexInitialize(int bufSize)
{
  InitialiseColourTable();
#ifdef __WXMSW__
  TexPathList.AddEnvList("TEXINPUT");
#endif
#ifdef __UNIX__
  TexPathList.AddEnvList("TEXINPUTS");
#endif
  int i;
  for (i = 0; i < 15; i++)
  {
    Inputs[i] = NULL;
    LineNumbers[i] = 1;
    FileNames[i] = NULL;
  }

  IgnorableInputFiles.Add("psbox.tex");
  BigBuffer = new char[(bufSize*1000)];
  currentArgData = new char[2000];
  TexFileRoot = new char[300];
  TexBibName = new char[300];
  TexTmpBibName = new char[300];
  AddMacroDef(ltTOPLEVEL, "toplevel", 1);
  TopLevel = new TexChunk(CHUNK_TYPE_MACRO);
//  TopLevel->name = copystring("toplevel");
  TopLevel->macroId = ltTOPLEVEL;
  TopLevel->no_args = 1;
  VerbatimMacroDef = (TexMacroDef *)MacroDefs.Get("verbatim");
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
  haveArgData = FALSE;
  noArgs = 0;

  if (TopLevel)
    delete TopLevel;
  TopLevel = new TexChunk(CHUNK_TYPE_MACRO);
//  TopLevel->name = copystring("toplevel");
  TopLevel->macroId = ltTOPLEVEL;
  TopLevel->no_args = 1;

  DocumentTitle = NULL;
  DocumentAuthor = NULL;
  DocumentDate = NULL;
  DocumentStyle = LATEX_REPORT;
  MinorDocumentStyle = 0;
  BibliographyStyleString = copystring("plain");
  DocumentStyleString = copystring("report");
  MinorDocumentStyleString = NULL;
/* Don't want to remove custom macros after each pass.
  SetFontSizes(10);
  wxNode *node = CustomMacroList.First();
  while (node)
  {
    CustomMacro *macro = (CustomMacro *)node->Data();
    delete macro;
    delete node;
    node = CustomMacroList.First();
  }
*/
  TexReferences.BeginFind();
  wxNode *node = TexReferences.Next();
  while (node)
  {
    TexRef *ref = (TexRef *)node->Data();
    delete ref;
    node = TexReferences.Next();
  }
  TexReferences.Clear();
  
  node = BibList.First();
  while (node)
  {
    BibEntry *entry = (BibEntry *)node->Data();
    delete entry;
    delete node;
    node = BibList.First();
  }
  CitationList.Clear();
  ResetTopicCounter();
}

// There is likely to be one set of macros used by all utilities.
void DefineDefaultMacros(void)
{
  // Put names which subsume other names at the TOP
  // so they get recognized first

  AddMacroDef(ltACCENT_GRAVE,     "`", 1);
  AddMacroDef(ltACCENT_ACUTE,     "'", 1);
  AddMacroDef(ltACCENT_CARET,     "^", 1);
  AddMacroDef(ltACCENT_UMLAUT,    "\"", 1);
  AddMacroDef(ltACCENT_TILDE,     "~", 1);
  AddMacroDef(ltACCENT_DOT,       ".", 1);
  AddMacroDef(ltACCENT_CADILLA,   "c", 1);
  AddMacroDef(ltSMALLSPACE1,      ",", 0);
  AddMacroDef(ltSMALLSPACE2,      ";", 0);

  AddMacroDef(ltABSTRACT,         "abstract", 1);
  AddMacroDef(ltADDCONTENTSLINE,  "addcontentsline", 3);
  AddMacroDef(ltADDTOCOUNTER,     "addtocounter", 2);
  AddMacroDef(ltALEPH,            "aleph", 0);
  AddMacroDef(ltALPHA,            "alpha", 0);
  AddMacroDef(ltALPH1,            "alph", 1);
  AddMacroDef(ltALPH2,            "Alph", 1);
  AddMacroDef(ltANGLE,            "angle", 0);
  AddMacroDef(ltAPPENDIX,         "appendix", 0);
  AddMacroDef(ltAPPROX,           "approx", 0);
  AddMacroDef(ltARABIC,           "arabic", 1);
  AddMacroDef(ltARRAY,            "array", 1);
  AddMacroDef(ltAST,              "ast", 0);
  AddMacroDef(ltASYMP,            "asymp", 0);
  AddMacroDef(ltAUTHOR,           "author", 1);

  AddMacroDef(ltBACKGROUNDCOLOUR, "backgroundcolour", 1);
  AddMacroDef(ltBACKGROUNDIMAGE,  "backgroundimage", 1);
  AddMacroDef(ltBACKGROUND,       "background", 1);
  AddMacroDef(ltBACKSLASHRAW,     "backslashraw", 0);
  AddMacroDef(ltBACKSLASH,        "backslash", 0);
  AddMacroDef(ltBASELINESKIP,     "baselineskip", 1);
  AddMacroDef(ltBCOL,             "bcol", 2);
  AddMacroDef(ltBETA,             "beta", 0);
  AddMacroDef(ltBFSERIES,         "bfseries", 1);
  AddMacroDef(ltBF,               "bf", 1);
  AddMacroDef(ltBIBITEM,          "bibitem", 2);  // For convenience, bibitem has 2 args: label and item.
                              // The Latex syntax permits writing as 2 args.
  AddMacroDef(ltBIBLIOGRAPHYSTYLE,    "bibliographystyle", 1);
  AddMacroDef(ltBIBLIOGRAPHY,     "bibliography", 1);
  AddMacroDef(ltBIGTRIANGLEDOWN,  "bigtriangledown", 0);
  AddMacroDef(ltBOT,              "bot", 0);
  AddMacroDef(ltBOXIT,            "boxit", 1);
  AddMacroDef(ltBOX,              "box", 0);
  AddMacroDef(ltBRCLEAR,          "brclear", 0);
  AddMacroDef(ltBULLET,           "bullet", 0);

  AddMacroDef(ltCAPTIONSTAR,      "caption*", 1);
  AddMacroDef(ltCAPTION,          "caption", 1);
  AddMacroDef(ltCAP,              "cap", 0);
  AddMacroDef(ltCDOTS,            "cdots", 0);
  AddMacroDef(ltCDOT,             "cdot", 0);
  AddMacroDef(ltCENTERLINE,       "centerline", 1);
  AddMacroDef(ltCENTERING,        "centering", 0);
  AddMacroDef(ltCENTER,           "center", 1);
  AddMacroDef(ltCEXTRACT,         "cextract", 0);
  AddMacroDef(ltCHAPTERHEADING,   "chapterheading", 1);
  AddMacroDef(ltCHAPTERSTAR,      "chapter*", 1);
  AddMacroDef(ltCHAPTER,          "chapter", 1);
  AddMacroDef(ltCHI,              "chi", 0);
  AddMacroDef(ltCINSERT,          "cinsert", 0);
  AddMacroDef(ltCIRC,             "circ", 0);
  AddMacroDef(ltCITE,             "cite", 1);
  AddMacroDef(ltCLASS,            "class", 1);
  AddMacroDef(ltCLEARDOUBLEPAGE,  "cleardoublepage", 0);
  AddMacroDef(ltCLEARPAGE,        "clearpage", 0);
  AddMacroDef(ltCLINE,            "cline", 1);
  AddMacroDef(ltCLIPSFUNC,        "clipsfunc", 3);
  AddMacroDef(ltCLUBSUIT,         "clubsuit", 0);
  AddMacroDef(ltCOLUMNSEP,        "columnsep", 1);
  AddMacroDef(ltCOMMENT,          "comment", 1, TRUE);
  AddMacroDef(ltCONG,             "cong", 0);
  AddMacroDef(ltCOPYRIGHT,        "copyright", 0);
  AddMacroDef(ltCPARAM,           "cparam", 2);
  AddMacroDef(ltCHEAD,            "chead", 1);
  AddMacroDef(ltCFOOT,            "cfoot", 1);
  AddMacroDef(ltCUP,              "cup", 0);

  AddMacroDef(ltDASHV,            "dashv", 0);
  AddMacroDef(ltDATE,             "date", 1);
  AddMacroDef(ltDELTA,            "delta", 0);
  AddMacroDef(ltCAP_DELTA,        "Delta", 0);
  AddMacroDef(ltDEFINECOLOUR,     "definecolour", 4);
  AddMacroDef(ltDEFINECOLOR,      "definecolor", 4);
  AddMacroDef(ltDESCRIPTION,      "description", 1);
  AddMacroDef(ltDESTRUCT,         "destruct", 1);
  AddMacroDef(ltDIAMOND2,         "diamond2", 0);
  AddMacroDef(ltDIAMOND,          "diamond", 0);
  AddMacroDef(ltDIV,              "div", 0);
  AddMacroDef(ltDOCUMENTCLASS,    "documentclass", 1);
  AddMacroDef(ltDOCUMENTSTYLE,    "documentstyle", 1);
  AddMacroDef(ltDOCUMENT,         "document", 1);
  AddMacroDef(ltDOUBLESPACE,      "doublespace", 1);
  AddMacroDef(ltDOTEQ,            "doteq", 0);
  AddMacroDef(ltDOWNARROW,        "downarrow", 0);
  AddMacroDef(ltDOWNARROW2,       "Downarrow", 0);

  AddMacroDef(ltEMPTYSET,         "emptyset", 0);
  AddMacroDef(ltEMPH,             "emph", 1);
  AddMacroDef(ltEM,               "em", 1);
  AddMacroDef(ltENUMERATE,        "enumerate", 1);
  AddMacroDef(ltEPSILON,          "epsilon", 0);
  AddMacroDef(ltEQUATION,         "equation", 1);
  AddMacroDef(ltEQUIV,            "equiv", 0);
  AddMacroDef(ltETA,              "eta", 0);
  AddMacroDef(ltEVENSIDEMARGIN,   "evensidemargin", 1);
  AddMacroDef(ltEXISTS,           "exists", 0);

  AddMacroDef(ltFBOX,             "fbox", 1);
  AddMacroDef(ltFCOL,             "fcol", 2);
  AddMacroDef(ltFIGURE,           "figure", 1);
  AddMacroDef(ltFIGURESTAR,       "figure*", 1);
  AddMacroDef(ltFLUSHLEFT,        "flushleft", 1);
  AddMacroDef(ltFLUSHRIGHT,       "flushright", 1);
  AddMacroDef(ltFOLLOWEDLINKCOLOUR, "followedlinkcolour", 1);
  AddMacroDef(ltFOOTHEIGHT,       "footheight", 1);
  AddMacroDef(ltFOOTNOTEPOPUP,    "footnotepopup", 2);
  AddMacroDef(ltFOOTNOTE,         "footnote", 1);
  AddMacroDef(ltFOOTSKIP,         "footskip", 1);
  AddMacroDef(ltFORALL,           "forall", 0);
  AddMacroDef(ltFRAMEBOX,         "framebox", 1);
  AddMacroDef(ltFROWN,            "frown", 0);
  AddMacroDef(ltFUNCTIONSECTION,  "functionsection", 1);
  AddMacroDef(ltFUNC,             "func", 3);
  AddMacroDef(ltFOOTNOTESIZE,     "footnotesize", 0);
  AddMacroDef(ltFANCYPLAIN,       "fancyplain", 2);

  AddMacroDef(ltGAMMA,            "gamma", 0);
  AddMacroDef(ltCAP_GAMMA,        "Gamma", 0);
  AddMacroDef(ltGEQ,              "geq", 0);
  AddMacroDef(ltGE,               "ge", 0);
  AddMacroDef(ltGG,               "gg", 0);
  AddMacroDef(ltGLOSSARY,         "glossary", 1);
  AddMacroDef(ltGLOSS,            "gloss", 1);

  AddMacroDef(ltHEADHEIGHT,       "headheight", 1);
  AddMacroDef(ltHEARTSUIT,        "heartsuit", 0);
  AddMacroDef(ltHELPGLOSSARY,     "helpglossary", 1);
  AddMacroDef(ltHELPIGNORE,       "helpignore", 1, TRUE);
  AddMacroDef(ltHELPONLY,         "helponly", 1);
  AddMacroDef(ltHELPINPUT,        "helpinput", 1);
  AddMacroDef(ltHELPFONTFAMILY,   "helpfontfamily", 1);
  AddMacroDef(ltHELPFONTSIZE,     "helpfontsize", 1);
  AddMacroDef(ltHELPREFN,         "helprefn", 2);
  AddMacroDef(ltHELPREF,          "helpref", 2);
  AddMacroDef(ltHFILL,            "hfill", 0);
  AddMacroDef(ltHLINE,            "hline", 0);
  AddMacroDef(ltHRULE,            "hrule", 0);
  AddMacroDef(ltHSPACESTAR,       "hspace*", 1);
  AddMacroDef(ltHSPACE,           "hspace", 1);
  AddMacroDef(ltHSKIPSTAR,        "hskip*", 1);
  AddMacroDef(ltHSKIP,            "hskip", 1);
  AddMacroDef(lthuge,             "huge", 1);
  AddMacroDef(ltHuge,             "Huge", 1);
  AddMacroDef(ltHUGE,             "HUGE", 1);
  AddMacroDef(ltHTMLIGNORE,       "htmlignore", 1);
  AddMacroDef(ltHTMLONLY,         "htmlonly", 1);

  AddMacroDef(ltIM,               "im", 0);
  AddMacroDef(ltINCLUDEONLY,      "includeonly", 1);
  AddMacroDef(ltINCLUDE,          "include", 1);
  AddMacroDef(ltINDENTED,         "indented", 2);
  AddMacroDef(ltINDEX,            "index", 1);
  AddMacroDef(ltINPUT,            "input", 1, TRUE);
  AddMacroDef(ltIOTA,             "iota", 0);
  AddMacroDef(ltITEMIZE,          "itemize", 1);
  AddMacroDef(ltITEM,             "item", 0);
  AddMacroDef(ltIMAGEMAP,         "imagemap", 3);
  AddMacroDef(ltIMAGEL,           "imagel", 2);
  AddMacroDef(ltIMAGER,           "imager", 2);
  AddMacroDef(ltIMAGE,            "image", 2);
  AddMacroDef(ltIN,               "in", 0);
  AddMacroDef(ltINFTY,            "infty", 0);
  AddMacroDef(ltITSHAPE,          "itshape", 1);
  AddMacroDef(ltIT,               "it", 1);
  AddMacroDef(ltITEMSEP,          "itemsep", 1);
  AddMacroDef(ltINSERTATLEVEL,    "insertatlevel", 2);

  AddMacroDef(ltKAPPA,            "kappa", 0);
  AddMacroDef(ltKILL,             "kill", 0);

  AddMacroDef(ltLABEL,            "label", 1);
  AddMacroDef(ltLAMBDA,           "lambda", 0);
  AddMacroDef(ltCAP_LAMBDA,       "Lambda", 0);
  AddMacroDef(ltlarge,            "large", 1);
  AddMacroDef(ltLarge,            "Large", 1);
  AddMacroDef(ltLARGE,            "LARGE", 1);
  AddMacroDef(ltLATEXIGNORE,      "latexignore", 1);
  AddMacroDef(ltLATEXONLY,        "latexonly", 1);
  AddMacroDef(ltLATEX,            "LaTeX", 0);
  AddMacroDef(ltLBOX,             "lbox", 1);
  AddMacroDef(ltLBRACERAW,        "lbraceraw", 0);
  AddMacroDef(ltLDOTS,            "ldots", 0);
  AddMacroDef(ltLEQ,              "leq", 0);
  AddMacroDef(ltLE,               "le", 0);
  AddMacroDef(ltLEFTARROW,        "leftarrow", 0);
  AddMacroDef(ltLEFTRIGHTARROW,   "leftrightarrow", 0);
  AddMacroDef(ltLEFTARROW2,       "Leftarrow", 0);
  AddMacroDef(ltLEFTRIGHTARROW2,  "Leftrightarrow", 0);
  AddMacroDef(ltLINEBREAK,        "linebreak", 0);
  AddMacroDef(ltLINKCOLOUR,       "linkcolour", 1);
  AddMacroDef(ltLISTOFFIGURES,    "listoffigures", 0);
  AddMacroDef(ltLISTOFTABLES,     "listoftables", 0);
  AddMacroDef(ltLHEAD,            "lhead", 1);
  AddMacroDef(ltLFOOT,            "lfoot", 1);
  AddMacroDef(ltLOWERCASE,        "lowercase", 1);
  AddMacroDef(ltLL,               "ll", 0);

  AddMacroDef(ltMAKEGLOSSARY,     "makeglossary", 0);
  AddMacroDef(ltMAKEINDEX,        "makeindex", 0);
  AddMacroDef(ltMAKETITLE,        "maketitle", 0);
  AddMacroDef(ltMARKRIGHT,        "markright", 1);
  AddMacroDef(ltMARKBOTH,         "markboth", 2);
  AddMacroDef(ltMARGINPARWIDTH,   "marginparwidth", 1);
  AddMacroDef(ltMARGINPARSEP,     "marginparsep", 1);
  AddMacroDef(ltMARGINPARODD,     "marginparodd", 1);
  AddMacroDef(ltMARGINPAREVEN,    "marginpareven", 1);
  AddMacroDef(ltMARGINPAR,        "marginpar", 1);
  AddMacroDef(ltMBOX,             "mbox", 1);
  AddMacroDef(ltMDSERIES,         "mdseries", 1);
  AddMacroDef(ltMEMBERSECTION,    "membersection", 1);
  AddMacroDef(ltMEMBER,           "member", 2);
  AddMacroDef(ltMID,              "mid", 0);
  AddMacroDef(ltMODELS,           "models", 0);
  AddMacroDef(ltMP,               "mp", 0);
  AddMacroDef(ltMULTICOLUMN,      "multicolumn", 3);
  AddMacroDef(ltMU,               "mu", 0);

  AddMacroDef(ltNABLA,            "nabla", 0);
  AddMacroDef(ltNEG,              "neg", 0);
  AddMacroDef(ltNEQ,              "neq", 0);
  AddMacroDef(ltNEWCOUNTER,       "newcounter", 1, FALSE, (bool)FORBID_ABSOLUTELY);
  AddMacroDef(ltNEWLINE,          "newline", 0);
  AddMacroDef(ltNEWPAGE,          "newpage", 0);
  AddMacroDef(ltNI,               "ni", 0);
  AddMacroDef(ltNOCITE,           "nocite", 1);
  AddMacroDef(ltNOINDENT,         "noindent", 0);
  AddMacroDef(ltNOLINEBREAK,      "nolinebreak", 0);
  AddMacroDef(ltNOPAGEBREAK,      "nopagebreak", 0);
  AddMacroDef(ltNORMALSIZE,       "normalsize", 1);
  AddMacroDef(ltNORMALBOX,        "normalbox", 1);
  AddMacroDef(ltNORMALBOXD,       "normalboxd", 1);
  AddMacroDef(ltNOTEQ,            "noteq", 0);
  AddMacroDef(ltNOTIN,            "notin", 0);
  AddMacroDef(ltNOTSUBSET,        "notsubset", 0);
  AddMacroDef(ltNU,               "nu", 0);

  AddMacroDef(ltODDSIDEMARGIN,    "oddsidemargin", 1);
  AddMacroDef(ltOMEGA,            "omega", 0);
  AddMacroDef(ltCAP_OMEGA,        "Omega", 0);
  AddMacroDef(ltONECOLUMN,        "onecolumn", 0);
  AddMacroDef(ltOPLUS,            "oplus", 0);
  AddMacroDef(ltOSLASH,           "oslash", 0);
  AddMacroDef(ltOTIMES,           "otimes", 0);

  AddMacroDef(ltPAGEBREAK,        "pagebreak", 0);
  AddMacroDef(ltPAGEREF,          "pageref", 1);
  AddMacroDef(ltPAGESTYLE,        "pagestyle", 1);
  AddMacroDef(ltPAGENUMBERING,    "pagenumbering", 1);
  AddMacroDef(ltPARAGRAPHSTAR,    "paragraph*", 1);
  AddMacroDef(ltPARAGRAPH,        "paragraph", 1);
  AddMacroDef(ltPARALLEL,         "parallel", 0);
  AddMacroDef(ltPARAM,            "param", 2);
  AddMacroDef(ltPARINDENT,        "parindent", 1);
  AddMacroDef(ltPARSKIP,          "parskip", 1);
  AddMacroDef(ltPARTIAL,          "partial", 0);
  AddMacroDef(ltPARTSTAR,         "part*", 1);
  AddMacroDef(ltPART,             "part", 1);
  AddMacroDef(ltPAR,              "par", 0);
  AddMacroDef(ltPERP,             "perp", 0);
  AddMacroDef(ltPHI,              "phi", 0);
  AddMacroDef(ltCAP_PHI,          "Phi", 0);
  AddMacroDef(ltPFUNC,            "pfunc", 3);
  AddMacroDef(ltPICTURE,          "picture", 1);
  AddMacroDef(ltPI,               "pi", 0);
  AddMacroDef(ltCAP_PI,           "Pi", 0);
  AddMacroDef(ltPM,               "pm", 0);
  AddMacroDef(ltPOPREFONLY,       "poprefonly", 1);
  AddMacroDef(ltPOPREF,           "popref", 2);
  AddMacroDef(ltPOUNDS,           "pounds", 0);
  AddMacroDef(ltPREC,             "prec", 0);
  AddMacroDef(ltPRECEQ,           "preceq", 0);
  AddMacroDef(ltPRINTINDEX,       "printindex", 0);
  AddMacroDef(ltPROPTO,           "propto", 0);
  AddMacroDef(ltPSBOXTO,          "psboxto", 1, FALSE, (bool)FORBID_ABSOLUTELY);
  AddMacroDef(ltPSBOX,            "psbox", 1, FALSE, (bool)FORBID_ABSOLUTELY);
  AddMacroDef(ltPSI,              "psi", 0);
  AddMacroDef(ltCAP_PSI,          "Psi", 0);

  AddMacroDef(ltQUOTE,            "quote", 1);
  AddMacroDef(ltQUOTATION,        "quotation", 1);

  AddMacroDef(ltRAGGEDBOTTOM,     "raggedbottom", 0);
  AddMacroDef(ltRAGGEDLEFT,       "raggedleft", 0);
  AddMacroDef(ltRAGGEDRIGHT,      "raggedright", 0);
  AddMacroDef(ltRBRACERAW,        "rbraceraw", 0);
  AddMacroDef(ltREF,              "ref", 1);
  AddMacroDef(ltREGISTERED,       "registered", 0);
  AddMacroDef(ltRE,               "we", 0);
  AddMacroDef(ltRHO,              "rho", 0);
  AddMacroDef(ltRIGHTARROW,       "rightarrow", 0);
  AddMacroDef(ltRIGHTARROW2,      "rightarrow2", 0);
  AddMacroDef(ltRMFAMILY,         "rmfamily", 1);
  AddMacroDef(ltRM,               "rm", 1);
  AddMacroDef(ltROMAN,            "roman", 1);
  AddMacroDef(ltROMAN2,           "Roman", 1);
//  AddMacroDef(lt"row", 1);
  AddMacroDef(ltRTFSP,            "rtfsp", 0);
  AddMacroDef(ltRTFIGNORE,        "rtfignore", 1);
  AddMacroDef(ltRTFONLY,          "rtfonly", 1);
  AddMacroDef(ltRULEDROW,         "ruledrow", 1);
  AddMacroDef(ltDRULED,           "druled", 1);
  AddMacroDef(ltRULE,             "rule", 2);
  AddMacroDef(ltRHEAD,            "rhead", 1);
  AddMacroDef(ltRFOOT,            "rfoot", 1);
  AddMacroDef(ltROW,              "row", 1);

  AddMacroDef(ltSCSHAPE,          "scshape", 1);
  AddMacroDef(ltSC,               "sc", 1);
  AddMacroDef(ltSECTIONHEADING,   "sectionheading", 1);
  AddMacroDef(ltSECTIONSTAR,      "section*", 1);
  AddMacroDef(ltSECTION,          "section", 1);
  AddMacroDef(ltSETCOUNTER,       "setcounter", 2);
  AddMacroDef(ltSFFAMILY,         "sffamily", 1);
  AddMacroDef(ltSF,               "sf", 1);
  AddMacroDef(ltSHARP,            "sharp", 0);
  AddMacroDef(ltSHORTCITE,        "shortcite", 1);
  AddMacroDef(ltSIGMA,            "sigma", 0);
  AddMacroDef(ltCAP_SIGMA,        "Sigma", 0);
  AddMacroDef(ltSIM,              "sim", 0);
  AddMacroDef(ltSIMEQ,            "simeq", 0);
  AddMacroDef(ltSINGLESPACE,      "singlespace", 1);
  AddMacroDef(ltSIZEDBOX,         "sizedbox", 2);
  AddMacroDef(ltSIZEDBOXD,        "sizedboxd", 2);
  AddMacroDef(ltSLOPPYPAR,        "sloppypar", 1);
  AddMacroDef(ltSLOPPY,           "sloppy", 0);
  AddMacroDef(ltSLSHAPE,          "slshape", 1);
  AddMacroDef(ltSL,               "sl", 1);
  AddMacroDef(ltSMALL,            "small", 1);
  AddMacroDef(ltSMILE,            "smile", 0);
  AddMacroDef(ltSS,               "ss", 0);
  AddMacroDef(ltSTAR,             "star", 0);
  AddMacroDef(ltSUBITEM,          "subitem", 0);
  AddMacroDef(ltSUBPARAGRAPHSTAR, "subparagraph*", 1);
  AddMacroDef(ltSUBPARAGRAPH,     "subparagraph", 1);
  AddMacroDef(ltSPECIAL,          "special", 1);
  AddMacroDef(ltSUBSECTIONSTAR,   "subsection*", 1);
  AddMacroDef(ltSUBSECTION,       "subsection", 1);
  AddMacroDef(ltSUBSETEQ,         "subseteq", 0);
  AddMacroDef(ltSUBSET,           "subset", 0);
  AddMacroDef(ltSUCC,             "succ", 0);
  AddMacroDef(ltSUCCEQ,           "succeq", 0);
  AddMacroDef(ltSUPSETEQ,         "supseteq", 0);
  AddMacroDef(ltSUPSET,           "supset", 0);
  AddMacroDef(ltSUBSUBSECTIONSTAR,"subsubsection*", 1);
  AddMacroDef(ltSUBSUBSECTION,    "subsubsection", 1);
  AddMacroDef(ltSUPERTABULAR,     "supertabular", 2, FALSE);
  AddMacroDef(ltSURD,             "surd", 0);
  AddMacroDef(ltSCRIPTSIZE,       "scriptsize", 1);
  AddMacroDef(ltSETHEADER,        "setheader", 6);
  AddMacroDef(ltSETFOOTER,        "setfooter", 6);
  AddMacroDef(ltSETHOTSPOTCOLOUR, "sethotspotcolour", 1);
  AddMacroDef(ltSETHOTSPOTCOLOR,  "sethotspotcolor", 1);
  AddMacroDef(ltSETHOTSPOTUNDERLINE, "sethotspotunderline", 1);
  AddMacroDef(ltSETTRANSPARENCY,  "settransparency", 1);
  AddMacroDef(ltSPADESUIT,        "spadesuit", 0);

  AddMacroDef(ltTABBING,          "tabbing", 2);
  AddMacroDef(ltTABLEOFCONTENTS,  "tableofcontents", 0);
  AddMacroDef(ltTABLE,            "table", 1);
  AddMacroDef(ltTABULAR,          "tabular", 2, FALSE);
  AddMacroDef(ltTAB,              "tab", 0);
  AddMacroDef(ltTAU,              "tau", 0);
  AddMacroDef(ltTEXTRM,           "textrm", 1);
  AddMacroDef(ltTEXTSF,           "textsf", 1);
  AddMacroDef(ltTEXTTT,           "texttt", 1);
  AddMacroDef(ltTEXTBF,           "textbf", 1);
  AddMacroDef(ltTEXTIT,           "textit", 1);
  AddMacroDef(ltTEXTSL,           "textsl", 1);
  AddMacroDef(ltTEXTSC,           "textsc", 1);
  AddMacroDef(ltTEXTWIDTH,        "textwidth", 1);
  AddMacroDef(ltTEXTHEIGHT,       "textheight", 1);
  AddMacroDef(ltTEXTCOLOUR,       "textcolour", 1);
  AddMacroDef(ltTEX,              "TeX", 0);
  AddMacroDef(ltTHEBIBLIOGRAPHY,  "thebibliography", 2);
  AddMacroDef(ltTHETA,            "theta", 0);
  AddMacroDef(ltTIMES,            "times", 0);
  AddMacroDef(ltCAP_THETA,        "Theta", 0);
  AddMacroDef(ltTITLEPAGE,        "titlepage", 1);
  AddMacroDef(ltTITLE,            "title", 1);
  AddMacroDef(ltTINY,             "tiny", 1);
  AddMacroDef(ltTODAY,            "today", 0);
  AddMacroDef(ltTOPMARGIN,        "topmargin", 1);
  AddMacroDef(ltTOPSKIP,          "topskip", 1);
  AddMacroDef(ltTRIANGLE,         "triangle", 0);
  AddMacroDef(ltTTFAMILY,         "ttfamily", 1);
  AddMacroDef(ltTT,               "tt", 1);
  AddMacroDef(ltTYPEIN,           "typein", 1);
  AddMacroDef(ltTYPEOUT,          "typeout", 1);
  AddMacroDef(ltTWOCOLWIDTHA,     "twocolwidtha", 1);
  AddMacroDef(ltTWOCOLWIDTHB,     "twocolwidthb", 1);
  AddMacroDef(ltTWOCOLSPACING,    "twocolspacing", 1);
  AddMacroDef(ltTWOCOLITEMRULED,  "twocolitemruled", 2);
  AddMacroDef(ltTWOCOLITEM,       "twocolitem", 2);
  AddMacroDef(ltTWOCOLLIST,       "twocollist", 1);
  AddMacroDef(ltTWOCOLUMN,        "twocolumn", 0);
  AddMacroDef(ltTHEPAGE,          "thepage", 0);
  AddMacroDef(ltTHECHAPTER,       "thechapter", 0);
  AddMacroDef(ltTHESECTION,       "thesection", 0);
  AddMacroDef(ltTHISPAGESTYLE,    "thispagestyle", 1);

  AddMacroDef(ltUNDERLINE,        "underline", 1);
  AddMacroDef(ltUPSILON,          "upsilon", 0);
  AddMacroDef(ltCAP_UPSILON,      "Upsilon", 0);
  AddMacroDef(ltUPARROW,          "uparrow", 0);
  AddMacroDef(ltUPARROW2,         "Uparrow", 0);
  AddMacroDef(ltUPPERCASE,        "uppercase", 1);
  AddMacroDef(ltUPSHAPE,          "upshape", 1);
  AddMacroDef(ltURLREF,           "urlref", 2);
  AddMacroDef(ltUSEPACKAGE,       "usepackage", 1);
  
  AddMacroDef(ltVAREPSILON,       "varepsilon", 0);
  AddMacroDef(ltVARPHI,           "varphi", 0);
  AddMacroDef(ltVARPI,            "varpi", 0);
  AddMacroDef(ltVARRHO,           "varrho", 0);
  AddMacroDef(ltVARSIGMA,         "varsigma", 0);
  AddMacroDef(ltVARTHETA,         "vartheta", 0);
  AddMacroDef(ltVDOTS,            "vdots", 0);
  AddMacroDef(ltVEE,              "vee", 0);
  AddMacroDef(ltVERBATIMINPUT,    "verbatiminput", 1);
  AddMacroDef(ltVERBATIM,         "verbatim", 1);
  AddMacroDef(ltVERBSTAR,         "verb*", 1);
  AddMacroDef(ltVERB,             "verb", 1);
  AddMacroDef(ltVERSE,            "verse", 1);
  AddMacroDef(ltVFILL,            "vfill", 0);
  AddMacroDef(ltVLINE,            "vline", 0);
  AddMacroDef(ltVOID,             "void", 0);
  AddMacroDef(ltVDASH,            "vdash", 0);
  AddMacroDef(ltVRULE,            "vrule", 0);
  AddMacroDef(ltVSPACESTAR,       "vspace*", 1);
  AddMacroDef(ltVSKIPSTAR,        "vskip*", 1);
  AddMacroDef(ltVSPACE,           "vspace", 1);
  AddMacroDef(ltVSKIP,            "vskip", 1);

  AddMacroDef(ltWEDGE,            "wedge", 0);
  AddMacroDef(ltWXCLIPS,          "wxclips", 0);
  AddMacroDef(ltWINHELPIGNORE,    "winhelpignore", 1);
  AddMacroDef(ltWINHELPONLY,      "winhelponly", 1);
  AddMacroDef(ltWP,               "wp", 0);

  AddMacroDef(ltXI,               "xi", 0);
  AddMacroDef(ltCAP_XI,           "Xi", 0);
  AddMacroDef(ltXLPIGNORE,        "xlpignore", 1);
  AddMacroDef(ltXLPONLY,          "xlponly", 1);

  AddMacroDef(ltZETA,             "zeta", 0);

  AddMacroDef(ltSPACE,            " ", 0);
  AddMacroDef(ltBACKSLASHCHAR,    "\\", 0);
  AddMacroDef(ltPIPE,             "|", 0);
  AddMacroDef(ltFORWARDSLASH,     "/", 0);
  AddMacroDef(ltUNDERSCORE,       "_", 0);
  AddMacroDef(ltAMPERSAND,        "&", 0);
  AddMacroDef(ltPERCENT,          "%", 0);
  AddMacroDef(ltDOLLAR,           "$", 0);
  AddMacroDef(ltHASH,             "#", 0);
  AddMacroDef(ltLPARENTH,         "(", 0);
  AddMacroDef(ltRPARENTH,         ")", 0);
  AddMacroDef(ltLBRACE,           "{", 0);
  AddMacroDef(ltRBRACE,           "}", 0);
//  AddMacroDef(ltEQUALS,           "=", 0);
  AddMacroDef(ltRANGLEBRA,        ">", 0);
  AddMacroDef(ltLANGLEBRA,        "<", 0);
  AddMacroDef(ltPLUS,             "+", 0);
  AddMacroDef(ltDASH,             "-", 0);
  AddMacroDef(ltAT_SYMBOL,        "@", 0);
//  AddMacroDef(ltSINGLEQUOTE,      "'", 0);
//  AddMacroDef(ltBACKQUOTE,        "`", 0);
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
        OnMacro(ltPAR, 0, TRUE);
        OnMacro(ltPAR, 0, FALSE);
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
        OnMacro(ltPAR, 0, TRUE);
        OnMacro(ltPAR, 0, FALSE);
        if ((convertMode == TEX_RTF) && !winHelp)
        {
          OnMacro(ltPAR, 0, TRUE);
          OnMacro(ltPAR, 0, FALSE);
        }
      }
      break;
    }
    case ltSPECIALAMPERSAND:
      if (start)
        TexOutput("  ");
      break;

    case ltCINSERT:
      if (start)
        TexOutput("<<", TRUE);
      break;
    case ltCEXTRACT:
      if (start)
        TexOutput(">>", TRUE);
      break;
    case ltDESTRUCT:
      if (start)
        TexOutput("~", TRUE);
      break;
    case ltTILDE:
      if (start)
        TexOutput("~", TRUE);
      break;
    case ltSPECIALTILDE:
      if (start)
        TexOutput(" ", TRUE);
      break;
    case ltUNDERSCORE:
      if (start)
        TexOutput("_", TRUE);
      break;
    case ltHASH:
      if (start)
        TexOutput("#", TRUE);
      break;
    case ltAMPERSAND:
      if (start)
        TexOutput("&", TRUE);
      break;
    case ltSPACE:
      if (start)
        TexOutput(" ", TRUE);
      break;
    case ltPIPE:
      if (start)
        TexOutput("|", TRUE);
      break;
    case ltPERCENT:
      if (start)
        TexOutput("%", TRUE);
      break;
    case ltDOLLAR:
      if (start)
        TexOutput("$", TRUE);
      break;
    case ltLPARENTH:
      if (start)
        TexOutput("", TRUE);
      break;
    case ltRPARENTH:
      if (start)
        TexOutput("", TRUE);
      break;
    case ltLBRACE:
      if (start)
        TexOutput("{", TRUE);
      break;
    case ltRBRACE:
      if (start)
        TexOutput("}", TRUE);
      break;
    case ltCOPYRIGHT:
      if (start)
        TexOutput("(c)", TRUE);
      break;
    case ltREGISTERED:
      if (start)
        TexOutput("(r)", TRUE);
      break;
    case ltBACKSLASH:
      if (start)    
        TexOutput("\\", TRUE);
      break;
    case ltLDOTS:
    case ltCDOTS:
      if (start)
        TexOutput("...", TRUE);
      break;
    case ltVDOTS:
      if (start)
        TexOutput("|", TRUE);
      break;
    case ltLATEX:
      if (start)
        TexOutput("LaTeX", TRUE);
      break;
    case ltTEX:
      if (start)
        TexOutput("TeX", TRUE);
      break;
    case ltPOUNDS:
      if (start)
        TexOutput("£", TRUE);
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
        TexOutput(ctime(&when), TRUE);
      }
      break;
    }
    case ltNOINDENT:
      if (start)
        ParIndent = 0;
      break;

    // Symbols
    case ltALPHA:
      if (start) TexOutput("alpha");
      break;
    case ltBETA:
      if (start) TexOutput("beta");
      break;
    case ltGAMMA:
      if (start) TexOutput("gamma");
      break;
    case ltDELTA:
      if (start) TexOutput("delta");
      break;
    case ltEPSILON:
    case ltVAREPSILON:
      if (start) TexOutput("epsilon");
      break;
    case ltZETA:
      if (start) TexOutput("zeta");
      break;
    case ltETA:
      if (start) TexOutput("eta");
      break;
    case ltTHETA:
    case ltVARTHETA:
      if (start) TexOutput("theta");
      break;
    case ltIOTA:
      if (start) TexOutput("iota");
      break;
    case ltKAPPA:
      if (start) TexOutput("kappa");
      break;
    case ltLAMBDA:
      if (start) TexOutput("lambda");
      break;
    case ltMU:
      if (start) TexOutput("mu");
      break;
    case ltNU:
      if (start) TexOutput("nu");
      break;
    case ltXI:
      if (start) TexOutput("xi");
      break;
    case ltPI:
    case ltVARPI:
      if (start) TexOutput("pi");
      break;
    case ltRHO:
    case ltVARRHO:
      if (start) TexOutput("rho");
      break;
    case ltSIGMA:
    case ltVARSIGMA:
      if (start) TexOutput("sigma");
      break;
    case ltTAU:
      if (start) TexOutput("tau");
      break;
    case ltUPSILON:
      if (start) TexOutput("upsilon");
      break;
    case ltPHI:
    case ltVARPHI:
      if (start) TexOutput("phi");
      break;
    case ltCHI:
      if (start) TexOutput("chi");
      break;
    case ltPSI:
      if (start) TexOutput("psi");
      break;
    case ltOMEGA:
      if (start) TexOutput("omega");
      break;
    case ltCAP_GAMMA:
      if (start) TexOutput("GAMMA");
      break;
    case ltCAP_DELTA:
      if (start) TexOutput("DELTA");
      break;
    case ltCAP_THETA:
      if (start) TexOutput("THETA");
      break;
    case ltCAP_LAMBDA:
      if (start) TexOutput("LAMBDA");
      break;
    case ltCAP_XI:
      if (start) TexOutput("XI");
      break;
    case ltCAP_PI:
      if (start) TexOutput("PI");
      break;
    case ltCAP_SIGMA:
      if (start) TexOutput("SIGMA");
      break;
    case ltCAP_UPSILON:
      if (start) TexOutput("UPSILON");
      break;
    case ltCAP_PHI:
      if (start) TexOutput("PHI");
      break;
    case ltCAP_PSI:
      if (start) TexOutput("PSI");
      break;
    case ltCAP_OMEGA:
      if (start) TexOutput("OMEGA");
      break;

    // Binary operation symbols
    case ltLE:
    case ltLEQ:
      if (start) TexOutput("<=");
      break;
    case ltLL:
      if (start) TexOutput("<<");
      break;
    case ltSUBSET:
      if (start) TexOutput("SUBSET");
      break;
    case ltSUBSETEQ:
      if (start) TexOutput("SUBSETEQ");
      break;
    case ltIN:
      if (start) TexOutput("IN");
      break;
    case ltVDASH:
      if (start) TexOutput("VDASH");
      break;
    case ltMODELS:
      if (start) TexOutput("MODELS");
      break;
    case ltGE:
    case ltGEQ:
      if (start) TexOutput(">=");
      break;
    case ltGG:
      if (start) TexOutput(">>");
      break;
    case ltSUPSET:
      if (start) TexOutput("SUPSET");
      break;
    case ltSUPSETEQ:
      if (start) TexOutput("SUPSETEQ");
      break;
    case ltNI:
      if (start) TexOutput("NI");
      break;
    case ltDASHV:
      if (start) TexOutput("DASHV");
      break;
    case ltPERP:
      if (start) TexOutput("PERP");
      break;
    case ltNEQ:
      if (start) TexOutput("NEQ");
      break;
    case ltDOTEQ:
      if (start) TexOutput("DOTEQ");
      break;
    case ltAPPROX:
      if (start) TexOutput("APPROX");
      break;
    case ltCONG:
      if (start) TexOutput("CONG");
      break;
    case ltEQUIV:
      if (start) TexOutput("EQUIV");
      break;
    case ltPROPTO:
      if (start) TexOutput("PROPTO");
      break;
    case ltPREC:
      if (start) TexOutput("PREC");
      break;
    case ltPRECEQ:
      if (start) TexOutput("PRECEQ");
      break;
    case ltPARALLEL:
      if (start) TexOutput("|");
      break;
    case ltSIM:
      if (start) TexOutput("~");
      break;
    case ltSIMEQ:
      if (start) TexOutput("SIMEQ");
      break;
    case ltASYMP:
      if (start) TexOutput("ASYMP");
      break;
    case ltSMILE:
      if (start) TexOutput(":-)");
      break;
    case ltFROWN:
      if (start) TexOutput(":-(");
      break;
    case ltSUCC:
      if (start) TexOutput("SUCC");
      break;
    case ltSUCCEQ:
      if (start) TexOutput("SUCCEQ");
      break;
    case ltMID:
      if (start) TexOutput("|");
      break;

    // Negated relation symbols
    case ltNOTEQ:
      if (start) TexOutput("!=");
      break;
    case ltNOTIN:
      if (start) TexOutput("NOTIN");
      break;
    case ltNOTSUBSET:
      if (start) TexOutput("NOTSUBSET");
      break;

    // Arrows
    case ltLEFTARROW:
      if (start) TexOutput("<--");
      break;
    case ltLEFTARROW2:
      if (start) TexOutput("<==");
      break;
    case ltRIGHTARROW:
      if (start) TexOutput("-->");
      break;
    case ltRIGHTARROW2:
      if (start) TexOutput("==>");
      break;
    case ltLEFTRIGHTARROW:
      if (start) TexOutput("<-->");
      break;
    case ltLEFTRIGHTARROW2:
      if (start) TexOutput("<==>");
      break;
    case ltUPARROW:
      if (start) TexOutput("UPARROW");
      break;
    case ltUPARROW2:
      if (start) TexOutput("UPARROW2");
      break;
    case ltDOWNARROW:
      if (start) TexOutput("DOWNARROW");
      break;
    case ltDOWNARROW2:
      if (start) TexOutput("DOWNARROW2");
      break;
    // Miscellaneous symbols
    case ltALEPH:
      if (start) TexOutput("ALEPH");
      break;
    case ltWP:
      if (start) TexOutput("WP");
      break;
    case ltRE:
      if (start) TexOutput("RE");
      break;
    case ltIM:
      if (start) TexOutput("IM");
      break;
    case ltEMPTYSET:
      if (start) TexOutput("EMPTYSET");
      break;
    case ltNABLA:
      if (start) TexOutput("NABLA");
      break;
    case ltSURD:
      if (start) TexOutput("SURD");
      break;
    case ltPARTIAL:
      if (start) TexOutput("PARTIAL");
      break;
    case ltBOT:
      if (start) TexOutput("BOT");
      break;
    case ltFORALL:
      if (start) TexOutput("FORALL");
      break;
    case ltEXISTS:
      if (start) TexOutput("EXISTS");
      break;
    case ltNEG:
      if (start) TexOutput("NEG");
      break;
    case ltSHARP:
      if (start) TexOutput("SHARP");
      break;
    case ltANGLE:
      if (start) TexOutput("ANGLE");
      break;
    case ltTRIANGLE:
      if (start) TexOutput("TRIANGLE");
      break;
    case ltCLUBSUIT:
      if (start) TexOutput("CLUBSUIT");
      break;
    case ltDIAMONDSUIT:
      if (start) TexOutput("DIAMONDSUIT");
      break;
    case ltHEARTSUIT:
      if (start) TexOutput("HEARTSUIT");
      break;
    case ltSPADESUIT:
      if (start) TexOutput("SPADESUIT");
      break;
    case ltINFTY:
      if (start) TexOutput("INFTY");
      break;
    case ltPM:
      if (start) TexOutput("PM");
      break;
    case ltMP:
      if (start) TexOutput("MP");
      break;
    case ltTIMES:
      if (start) TexOutput("TIMES");
      break;
    case ltDIV:
      if (start) TexOutput("DIV");
      break;
    case ltCDOT:
      if (start) TexOutput("CDOT");
      break;
    case ltAST:
      if (start) TexOutput("AST");
      break;
    case ltSTAR:
      if (start) TexOutput("STAR");
      break;
    case ltCAP:
      if (start) TexOutput("CAP");
      break;
    case ltCUP:
      if (start) TexOutput("CUP");
      break;
    case ltVEE:
      if (start) TexOutput("VEE");
      break;
    case ltWEDGE:
      if (start) TexOutput("WEDGE");
      break;
    case ltCIRC:
      if (start) TexOutput("CIRC");
      break;
    case ltBULLET:
      if (start) TexOutput("BULLET");
      break;
    case ltDIAMOND:
      if (start) TexOutput("DIAMOND");
      break;
    case ltOSLASH:
      if (start) TexOutput("OSLASH");
      break;
    case ltBOX:
      if (start) TexOutput("BOX");
      break;
    case ltDIAMOND2:
      if (start) TexOutput("DIAMOND2");
      break;
    case ltBIGTRIANGLEDOWN:
      if (start) TexOutput("BIGTRIANGLEDOWN");
      break;
    case ltOPLUS:
      if (start) TexOutput("OPLUS");
      break;
    case ltOTIMES:
      if (start) TexOutput("OTIMES");
      break;
    case ltSS:
      if (start) TexOutput("s");
      break;
    case ltBACKSLASHRAW:
      if (start) TexOutput("\\");
      break;
    case ltLBRACERAW:
      if (start) TexOutput("{");
      break;
    case ltRBRACERAW:
      if (start) TexOutput("}");
      break;
    case ltSMALLSPACE1:
    case ltSMALLSPACE2:
      if (start) TexOutput(" ");
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
      char *refName = GetArgData();
      if (refName)
      {
        TexRef *texRef = FindReference(refName);
        if (texRef)
        {
          // Must strip the 'section' or 'chapter' or 'figure' text
          // from a normal 'ref' reference
          char buf[150];
          strcpy(buf, texRef->sectionNumber);
          int len = strlen(buf);
          int i = 0;
          if (strcmp(buf, "??") != 0)
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
          TexOutput(texRef->sectionNumber + i, TRUE);
        }
        else
        {
          char buf[300];
          TexOutput("??", TRUE);
          sprintf(buf, "Warning: unresolved reference %s.", refName); 
          OnInform(buf);
        }
      }
      else TexOutput("??", TRUE);
      return FALSE;
    }
    break;
    }
    case ltLABEL:
    {
      return FALSE;
      break;
    }
    case ltAUTHOR:
    {
      if (start && (arg_no == 1))
        DocumentAuthor = GetArgChunk();
      return FALSE;
      break;
    }
    case ltDATE:
    {
      if (start && (arg_no == 1))
        DocumentDate = GetArgChunk();
      return FALSE;
      break;
    }
    case ltTITLE:
    {
      if (start && (arg_no == 1))
        DocumentTitle = GetArgChunk();
      return FALSE;
      break;
    }
  case ltDOCUMENTCLASS:
  case ltDOCUMENTSTYLE:
  {
    if (start && !IsArgOptional())
    {
      DocumentStyleString = copystring(GetArgData());
      if (strncmp(DocumentStyleString, "art", 3) == 0)
        DocumentStyle = LATEX_ARTICLE;
      else if (strncmp(DocumentStyleString, "rep", 3) == 0)
        DocumentStyle = LATEX_REPORT;
      else if (strncmp(DocumentStyleString, "book", 4) == 0 ||
               strncmp(DocumentStyleString, "thesis", 6) == 0)
        DocumentStyle = LATEX_BOOK;
      else if (strncmp(DocumentStyleString, "letter", 6) == 0)
        DocumentStyle = LATEX_LETTER;
      else if (strncmp(DocumentStyleString, "slides", 6) == 0)
        DocumentStyle = LATEX_SLIDES;
        
      if (StringMatch("10", DocumentStyleString))
        SetFontSizes(10);
      else if (StringMatch("11", DocumentStyleString))
        SetFontSizes(11);
      else if (StringMatch("12", DocumentStyleString))
        SetFontSizes(12);

      OnMacro(ltHELPFONTSIZE, 1, TRUE);
      sprintf(currentArgData, "%d", normalFont);
      haveArgData = TRUE;
      OnArgument(ltHELPFONTSIZE, 1, TRUE);
      OnArgument(ltHELPFONTSIZE, 1, FALSE);
      haveArgData = FALSE;
      OnMacro(ltHELPFONTSIZE, 1, FALSE);
    }
    else if (start && IsArgOptional())
    {
      MinorDocumentStyleString = copystring(GetArgData());

      if (StringMatch("10", MinorDocumentStyleString))
        SetFontSizes(10);
      else if (StringMatch("11", MinorDocumentStyleString))
        SetFontSizes(11);
      else if (StringMatch("12", MinorDocumentStyleString))
        SetFontSizes(12);
    }
    return FALSE;
    break;
  }
  case ltBIBLIOGRAPHYSTYLE:
  {
    if (start && !IsArgOptional())
      BibliographyStyleString = copystring(GetArgData());
    return FALSE;
    break;
  }
  case ltPAGESTYLE:
  {
    if (start && !IsArgOptional())
    {
      if (PageStyle) delete[] PageStyle;
      PageStyle = copystring(GetArgData());
    }
    return FALSE;
    break;
  }
/*
  case ltLHEAD:
  {
    if (start && !IsArgOptional())
      LeftHeader = GetArgChunk();
    return FALSE;
    break;
  }
  case ltLFOOT:
  {
    if (start && !IsArgOptional())
      LeftFooter = GetArgChunk();
    return FALSE;
    break;
  }
  case ltCHEAD:
  {
    if (start && !IsArgOptional())
      CentreHeader = GetArgChunk();
    return FALSE;
    break;
  }
  case ltCFOOT:
  {
    if (start && !IsArgOptional())
      CentreFooter = GetArgChunk();
    return FALSE;
    break;
  }
  case ltRHEAD:
  {
    if (start && !IsArgOptional())
      RightHeader = GetArgChunk();
    return FALSE;
    break;
  }
  case ltRFOOT:
  {
    if (start && !IsArgOptional())
      RightFooter = GetArgChunk();
    return FALSE;
    break;
  }
*/
  case ltCITE:
  case ltSHORTCITE:
  {
    if (start && !IsArgOptional())
    {
      char *citeKeys = GetArgData();
      int pos = 0;
      char *citeKey = ParseMultifieldString(citeKeys, &pos);
      while (citeKey)
      {
        AddCitation(citeKey);
        TexRef *ref = FindReference(citeKey);
        if (ref)
        {
          TexOutput(ref->sectionNumber, TRUE);
          if (strcmp(ref->sectionNumber, "??") == 0)
          {
            char buf[300];
            sprintf(buf, "Warning: unresolved citation %s.", citeKey);
            OnInform(buf);
          }
        }
        citeKey = ParseMultifieldString(citeKeys, &pos);
        if (citeKey)
        {
          TexOutput(", ", TRUE);
        }
      }
      return FALSE;
    }
    break;
  }
  case ltNOCITE:
  {
    if (start && !IsArgOptional())
    {
      char *citeKey = GetArgData();
      AddCitation(citeKey);
      return FALSE;
    }
    break;
  }
  case ltHELPFONTSIZE:
  {
    if (start)
    {
      char *data = GetArgData();
      if (strcmp(data, "10") == 0)
        SetFontSizes(10);
      else if (strcmp(data, "11") == 0)
        SetFontSizes(11);
      else if (strcmp(data, "12") == 0)
        SetFontSizes(12);
      return FALSE;
    }
    break;
  }
  case ltPAGEREF:
  {
    if (start)
    {
      TexOutput(" ??", TRUE);
      return FALSE;
    }
    break;
  }
  case ltPARSKIP:
  {
    if (start && arg_no == 1)
    {
      char *data = GetArgData();
      ParSkip = ParseUnitArgument(data);
      return FALSE;
    }
    break;
  }
  case ltPARINDENT:
  {
    if (start && arg_no == 1)
    {
      char *data = GetArgData();
      ParIndent = ParseUnitArgument(data);
      return FALSE;
    }
    break;
  }
  case ltSL:
  {
    return OnArgument(ltIT, arg_no, start);
    break;
  }
  case ltSPECIALDOUBLEDOLLAR:
  {
    return OnArgument(ltCENTER, arg_no, start);
    break;
  }
  case ltPARAGRAPH:
  case ltPARAGRAPHSTAR:
  case ltSUBPARAGRAPH:
  case ltSUBPARAGRAPHSTAR:
  {
    return OnArgument(ltSUBSUBSECTION, arg_no, start);
    break;
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
      TexOutput(" (", TRUE);
    else
      TexOutput(")", TRUE);
    break;
  }
  case ltBIBLIOGRAPHY:
  {
    if (start)
    {
      FILE *fd;
      int ch;
      char smallBuf[2];
      smallBuf[1] = 0;
      if ((fd = fopen(TexBibName, "r")))
      {
        ch = getc(fd);
        smallBuf[0] = ch;
        while (ch != EOF)
        {
          TexOutput(smallBuf);
          ch = getc(fd);
          smallBuf[0] = ch;
        }
        fclose(fd);
      }
      else
      {
        OnInform("Run Tex2RTF again to include bibliography.");
      }

      // Read in the .bib file, resolve all known references, write out the RTF.
      char *allFiles = GetArgData();
      int pos = 0;
      char *bibFile = ParseMultifieldString(allFiles, &pos);
      while (bibFile)
      {
        char fileBuf[300];
        strcpy(fileBuf, bibFile);
        wxString actualFile = TexPathList.FindValidPath(fileBuf);
        if (actualFile == "")
        {
          strcat(fileBuf, ".bib");
          actualFile = TexPathList.FindValidPath(fileBuf);
        }
        if (actualFile != "")
        {
          if (!ReadBib((char*) (const char*) actualFile))
          {
            char buf[300];
            sprintf(buf, ".bib file %s not found or malformed", (const char*) actualFile);
            OnError(buf);
          }
        }
        else
        {
          char buf[300];
          sprintf(buf, ".bib file %s not found", fileBuf);
          OnError(buf);
        }
        bibFile = ParseMultifieldString(allFiles, &pos);
      }

      ResolveBibReferences();

      // Write it a new bib section in the appropriate format.
      FILE *save1 = CurrentOutput1;
      FILE *save2 = CurrentOutput2;
      FILE *Biblio = fopen(TexTmpBibName, "w");
      SetCurrentOutput(Biblio);
      OutputBib();
      fclose(Biblio);
      if (wxFileExists(TexTmpBibName))
      {
        if (wxFileExists(TexBibName)) wxRemoveFile(TexBibName);
        wxRenameFile(TexTmpBibName, TexBibName);
      }
      SetCurrentOutputs(save1, save2);
      return FALSE;
    }
    break;
  }
  case ltMULTICOLUMN:
  {
    if (start && (arg_no == 3))
      return TRUE;
    else
      return FALSE;
    break;
  }
  case ltSCSHAPE:
  case ltTEXTSC:
  case ltSC:
  {
    if (start && (arg_no == 1))
    {
      char *s = GetArgData();
      if (s)
      {
        char *s1 = copystring(s);
        int i;
        for (i = 0; i < (int)strlen(s); i++)
          s1[i] = toupper(s[i]);
        TexOutput(s1);
        delete[] s1;
        return FALSE;
      }
      else return TRUE;

    }
    return TRUE;
    break;
  }
  case ltLOWERCASE:
  {
    if (start && (arg_no == 1))
    {
      char *s = GetArgData();
      if (s)
      {
        char *s1 = copystring(s);
        int i;
        for (i = 0; i < (int)strlen(s); i++)
          s1[i] = tolower(s[i]);
        TexOutput(s1);
        delete[] s1;
        return FALSE;
      }
      else return TRUE;

    }
    return TRUE;
    break;
  }
  case ltUPPERCASE:
  {
    if (start && (arg_no == 1))
    {
      char *s = GetArgData();
      if (s)
      {
        char *s1 = copystring(s);
        int i;
        for (i = 0; i < (int)strlen(s); i++)
          s1[i] = toupper(s[i]);
        TexOutput(s1);
        delete[] s1;
        return FALSE;
      }
      else return TRUE;

    }
    return TRUE;
    break;
  }
  case ltPOPREF:  // Ignore second argument by default
  {
    if (start && (arg_no == 1))
      return TRUE;
    else
      return FALSE;
    break;
  }
  case ltTWOCOLUMN:
    return TRUE;
    break;
  case ltXLPIGNORE:
    return ((convertMode == TEX_XLP) ? FALSE : TRUE);
    break;
  case ltXLPONLY:
    return ((convertMode != TEX_XLP) ? FALSE : TRUE);
    break;
  case ltHTMLIGNORE:
    return ((convertMode == TEX_HTML) ? FALSE : TRUE);
    break;
  case ltHTMLONLY:
    return ((convertMode != TEX_HTML) ? FALSE : TRUE);
    break;
  case ltRTFIGNORE:
    return (((convertMode == TEX_RTF) && !winHelp) ? FALSE : TRUE);
    break;
  case ltRTFONLY:
    return (!((convertMode == TEX_RTF) && !winHelp) ? FALSE : TRUE);
    break;
  case ltWINHELPIGNORE:
    return (winHelp ? FALSE : TRUE);
    break;
  case ltWINHELPONLY:
    return (!winHelp ? FALSE : TRUE);
    break;
  case ltLATEXIGNORE:
    return TRUE;
    break;
  case ltLATEXONLY:
    return FALSE;
    break;
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
    return FALSE;
    break;
  case ltTABULAR:
  case ltSUPERTABULAR:
  {
    if (arg_no == 2)
      return TRUE;
    else return FALSE;
    break;
  }
  case ltINDENTED:
  {
    if (arg_no == 2) return TRUE;
    else return FALSE;
    break;
  }
  case ltSIZEDBOX:
  case ltSIZEDBOXD:
  {
    if (arg_no == 2) return TRUE;
    else return FALSE;
    break;
  }
  case ltDEFINECOLOUR:
  case ltDEFINECOLOR:
  {
    static int redVal = 0;
    static int greenVal = 0;
    static int blueVal = 0;
    static char *colourName = NULL;
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
          redVal = atoi(GetArgData());
          break;
        }
        case 3:
        {
          greenVal = atoi(GetArgData());
          break;
        }
        case 4:
        {
          blueVal = atoi(GetArgData());
          AddColour(colourName, redVal, greenVal, blueVal);
          break;
        }
        default:
          break;
      }
    }
    return FALSE;
    break;
  }
  case ltFIGURE:
  case ltFIGURESTAR:
  case ltNORMALBOX:
  case ltNORMALBOXD:
  default:
  {
    if (IsArgOptional())
      return FALSE;
    else
      return TRUE;
    break;
  }
  }
  return TRUE;
}


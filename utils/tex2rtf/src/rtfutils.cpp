/////////////////////////////////////////////////////////////////////////////
// Name:        rtfutils.cpp
// Purpose:     Converts Latex to Word RTF/WinHelp RTF
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

#ifndef WX_PRECOMP
#endif

#include "tex2any.h"
#include "tex2rtf.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __WIN32__
#include <windows.h>
#endif

#include "bmputils.h"
#include "table.h"

static inline wxChar* copystring(const wxChar* s)
    { return wxStrcpy(new wxChar[wxStrlen(s) + 1], s); }

wxList itemizeStack;
static int indentLevel = 0;
static int forbidParindent = 0; // if > 0, no parindent (e.g. in center environment)
int forbidResetPar = 0; // If > 0, don't reset memory of having output a new par

static wxChar *contentsLineSection = NULL;
static wxChar *contentsLineValue = NULL;
static TexChunk *descriptionItemArg = NULL;
static wxStringList environmentStack; // Stack of paragraph styles we need to remember
static int footnoteCount = 0;
static int citeCount = 1;
extern bool winHelp;
extern bool startedSections;
extern FILE *Contents;
extern FILE *Chapters;
extern FILE *Popups;
extern FILE *WinHelpContentsFile;
extern wxChar *RTFCharset;
// This is defined in the Tex2Any library and isn't in use after parsing
extern wxChar *BigBuffer;

extern wxHashTable TexReferences;

// Are we in verbatim mode? If so, format differently.
static bool inVerbatim = false;

// We're in a series of PopRef topics, so don't output section headings
bool inPopRefSection = false;

// Green colour?
static bool hotSpotColour = true;
static bool hotSpotUnderline = true;

// Transparency (WHITE = transparent)
static bool bitmapTransparency = true;

// Linear RTF requires us to set the style per section.
static wxChar *currentNumberStyle = NULL;
static int currentItemSep = 8;
static int CurrentTextWidth = 8640; // Say, six inches
static int CurrentLeftMarginOdd = 400;
static int CurrentLeftMarginEven = 1440;
static int CurrentRightMarginOdd = 1440;
static int CurrentRightMarginEven = 400;
static int CurrentMarginParWidth = 2000;
static int CurrentMarginParSep = 400;  // Gap between marginpar and text
static int CurrentMarginParX = CurrentLeftMarginOdd + CurrentTextWidth + CurrentMarginParSep;
static int GutterWidth = 2300;

// Two-column table dimensions, in twips
static int TwoColWidthA = 1500;
static int TwoColWidthB = 3000;

const int PageWidth = 12242; // 8.25 inches wide for A4

// Remember the anchor in a helpref
static TexChunk *helpRefText = NULL;

/*
 * Flag to say we've just issued a \par\pard command, so don't
 * repeat this unnecessarily.
 *
 */

int issuedNewParagraph = 0;

// Need to know whether we're in a table or figure for benefit
// of listoffigures/listoftables
static bool inFigure = false;
static bool inTable = false;

/*
 * Current topics
 *
 */
static wxChar *CurrentChapterName = NULL;
static wxChar *CurrentSectionName = NULL;
static wxChar *CurrentSubsectionName = NULL;
static wxChar *CurrentTopic = NULL;

static bool InPopups()
{
  if (CurrentChapterName && (wxStrcmp(CurrentChapterName, _T("popups")) == 0))
    return true;
  if (CurrentSectionName && (wxStrcmp(CurrentSectionName, _T("popups")) == 0))
    return true;
  return false;
}

static void SetCurrentTopic(wxChar *s)
{
  if (CurrentTopic) delete[] CurrentTopic;
  CurrentTopic = copystring(s);
}

void SetCurrentChapterName(wxChar *s)
{
  if (CurrentChapterName) delete[] CurrentChapterName;
  CurrentChapterName = copystring(s);
  SetCurrentTopic(s);
}
void SetCurrentSectionName(wxChar *s)
{
  if (CurrentSectionName) delete[] CurrentSectionName;
  CurrentSectionName = copystring(s);
  SetCurrentTopic(s);
}
void SetCurrentSubsectionName(wxChar *s)
{
  if (CurrentSubsectionName) delete[] CurrentSubsectionName;
  CurrentSubsectionName = copystring(s);
  SetCurrentTopic(s);
}

// Indicate that a parent topic at level 'level' has children.
// Level 1 is a chapter, 2 is a section, etc.
void NotifyParentHasChildren(int parentLevel)
{
  wxChar *parentTopic = NULL;
  switch (parentLevel)
  {
    case 1:
    {
      parentTopic = CurrentChapterName;
      break;
    }
    case 2:
    {
      parentTopic = CurrentSectionName;
      break;
    }
    case 3:
    {
      parentTopic = CurrentSubsectionName;
      break;
    }
    default:
    {
      break;
    }
  }
  if (parentTopic)
  {
    TexTopic *texTopic = (TexTopic *)TopicTable.Get(parentTopic);
    if (!texTopic)
    {
      texTopic = new TexTopic;
      TopicTable.Put(parentTopic, texTopic);
    }
    texTopic->hasChildren = true;
  }
}

// Have to keep a count of what levels are books, what are pages,
// in order to correct for a Win95 bug which means that if you
// have a book at level n, and then a page at level n, the page
// ends up on level n + 1.

bool ContentsLevels[5];

// Reset below this level (starts from 1)
void ResetContentsLevels(int l)
{
  int i;
  for (i = l; i < 5; i++)
    ContentsLevels[i] = false;

  // There are always books on the top level
  ContentsLevels[0] = true;
}

// Output a WinHelp section as a keyword, substituting
// : for space.
void OutputSectionKeyword(FILE *fd)
{
  OutputCurrentSectionToString(wxTex2RTFBuffer);

  unsigned int i;
  for (i = 0; i < wxStrlen(wxTex2RTFBuffer); i++)
    if (wxTex2RTFBuffer[i] == ':')
      wxTex2RTFBuffer[i] = ' ';
    // Don't write to index if there's some RTF in the string
    else if ( wxTex2RTFBuffer[i] == '{' )
        return;

  wxFprintf(fd, _T("K{\\footnote {K} "));
  wxFprintf(fd, _T("%s"), wxTex2RTFBuffer);

  wxFprintf(fd, _T("}\n"));
}

// Write a line for the .cnt file, if we're doing this.
void WriteWinHelpContentsFileLine(wxChar *topicName, wxChar *xitle, int level)
{
  // First, convert any RTF characters to ASCII
  wxChar title[255];
  int s=0;
  int d=0;
  // assuming iso-8859-1 here even in Unicode build (FIXME?)
  while ( (xitle[s]!=0)&&(d<255) )
  {
    wxChar ch=wxChar(xitle[s]&0xff);
    if (ch==0x5c) {
      wxChar ch1=wxChar(xitle[s+1]&0xff);
      wxChar ch2=wxChar(xitle[s+2]&0xff);
      wxChar ch3=wxChar(xitle[s+3]&0xff);
      s+=4; // next character
      if ((ch1==0x27)&&(ch2==0x66)&&(ch3==0x36)) { title[d++]=wxChar('ö');  }
      if ((ch1==0x27)&&(ch2==0x65)&&(ch3==0x34)) { title[d++]=wxChar('ä');  }
      if ((ch1==0x27)&&(ch2==0x66)&&(ch3==0x63)) { title[d++]=wxChar('ü');  }
      if ((ch1==0x27)&&(ch2==0x64)&&(ch3==0x36)) { title[d++]=wxChar('Ö');  }
      if ((ch1==0x27)&&(ch2==0x63)&&(ch3==0x34)) { title[d++]=wxChar('Ä');  }
      if ((ch1==0x27)&&(ch2==0x64)&&(ch3==0x63)) { title[d++]=wxChar('Ü');  }
    } else {
      title[d++]=ch;
      s++;
    }
  }
  title[d]=0;

  // Section (2) becomes level 1 if it's an article.
  if (DocumentStyle == LATEX_ARTICLE)
    level --;

  if (level == 0) // Means we had a Chapter in an article, oops.
    return;

  ResetContentsLevels(level);

  if (winHelp && winHelpContents && WinHelpContentsFile)
  {
    TexTopic *texTopic = (TexTopic *)TopicTable.Get(topicName);
    if (texTopic)
    {
      // If a previous section at this level was a book, we *have* to have a
      // book not a page, because of a bug in WHC (or WinHelp 4).
      if (texTopic->hasChildren || level == 1 || ContentsLevels[level-1])
      {
        // At this level, we have a pointer to a further hierarchy.
        // So we need a 'book' consisting of (say) Chapter 1.
        wxFprintf(WinHelpContentsFile, _T("%d %s\n"), level, title);

        // Then we have a 'page' consisting of the text for this chapter
        wxFprintf(WinHelpContentsFile, _T("%d %s=%s\n"), level+1, title, topicName);

        // Then we'll be writing out further pages or books at level + 1...

        // Remember that at this level, we had a book and *must* for the
        // remainder of sections at this level.
        ContentsLevels[level-1] = true;
      }
      else
      {
        wxFprintf(WinHelpContentsFile, _T("%d %s=%s\n"), level, title, topicName);
      }
    }
    else
    {
      if (level == 1 || ContentsLevels[level-1])
      {
        // Always have a book at level 1
        wxFprintf(WinHelpContentsFile, _T("%d %s\n"), level, title);
        wxFprintf(WinHelpContentsFile, _T("%d %s=%s\n"), level+1, title, topicName);
        ContentsLevels[level-1] = true;
      }
      else
        // Probably doesn't have children if it hasn't been added to the topic table
        wxFprintf(WinHelpContentsFile, _T("%d %s=%s\n"), level, title, topicName);
    }
  }
}

void SplitIndexEntry(wxChar *entry, wxChar *buf1, wxChar *buf2)
{
  int len = wxStrlen(entry); int i = 0;
  while ((i < len) && entry[i] != '!')
  { buf1[i] = entry[i]; i ++; }
  buf1[i] = 0; buf2[0] = 0; int j = 0;

  if (entry[i] == '!')
  {
    i ++;
    while (i < len) { buf2[j] = entry[i]; i ++; j++; }
    buf2[j] = 0;
  }
}

/*
 * Output topic index entries in WinHelp RTF
 *
 */
void GenerateKeywordsForTopic(wxChar *topic)
{
  TexTopic *texTopic = (TexTopic *)TopicTable.Get(topic);
  if (!texTopic)
    return;

  wxStringList *list = texTopic->keywords;
  if (list)
  {
    wxStringListNode *node = list->GetFirst();
    while (node)
    {
      wxChar *s = (wxChar *)node->GetData();

      // Must separate out main entry form subentry (only 1 subentry allowed)
      wxChar buf1[100]; wxChar buf2[100];
      SplitIndexEntry(s, buf1, buf2);

      // Check for ':' which messes up index
      unsigned int i;
      for (i = 0; i < wxStrlen(buf1) ; i++)
        if (buf1[i] == ':')
          buf1[i] = ' ';
      for (i = 0; i < wxStrlen(buf2) ; i++)
        if (buf2[i] == ':')
          buf2[i] = ' ';

      // {K} is a strange fix to prevent words beginning with K not
      // being indexed properly
      TexOutput(_T("K{\\footnote {K} "));
      TexOutput(buf1);
      if (wxStrlen(buf2) > 0)
      {
        // Output subentry
        TexOutput(_T(", "));
        TexOutput(buf2);
      }
      TexOutput(_T("}\n"));
      node = node->GetNext();
    }
  }
}

/*
 * Output index entry in linear RTF
 *
 */

void GenerateIndexEntry(wxChar *entry)
{
  if (useWord)
  {
    wxChar buf1[100]; wxChar buf2[100];
    SplitIndexEntry(entry, buf1, buf2);

    TexOutput(_T("{\\xe\\v {"));
    TexOutput(buf1);
    if (wxStrlen(buf2) > 0)
    {
      TexOutput(_T("\\:"));
      TexOutput(buf2);
    }
    TexOutput(_T("}}"));
  }
}

 /*
  * Write a suitable RTF header.
  *
  */

void WriteColourTable(FILE *fd)
{
  wxFprintf(fd, _T("{\\colortbl"));
  wxNode *node = ColourTable.GetFirst();
  while (node)
  {
    ColourTableEntry *entry = (ColourTableEntry *)node->GetData();
    wxFprintf(fd, _T("\\red%d\\green%d\\blue%d;\n"), entry->red, entry->green, entry->blue);
    node = node->GetNext();
  }
  wxFprintf(fd, _T("}"));
}

/*
 * Write heading style
 *
 */

void WriteHeadingStyle(FILE *fd, int heading)
{
  switch (heading)
  {
    case 1:
    {
      wxFprintf(fd, _T("\\sb300\\sa260\\f2\\b\\fs%d"), chapterFont*2);
      break;
    }
    case 2:
    {
      wxFprintf(fd, _T("\\sb200\\sa240\\f2\\b\\fs%d"), sectionFont*2);
      break;
    }
    case 3:
    {
      wxFprintf(fd, _T("\\sb120\\sa240\\f2\\b\\fs%d"), subsectionFont*2);
      break;
    }
    case 4:
    {
      wxFprintf(fd, _T("\\sb120\\sa240\\f2\\b\\fs%d"), subsectionFont*2);
      break;
    }
    default:
      break;
  }
}

void WriteRTFHeader(FILE *fd)
{
  wxFprintf(fd, _T("{\\rtf1\\%s \\deff0\n"), RTFCharset);
  wxFprintf(fd, _T("{\\fonttbl{\\f0\\froman Times New Roman;}{\\f1\\ftech Symbol;}{\\f2\\fswiss Arial;}\n"));
  wxFprintf(fd, _T("{\\f3\\fmodern Courier New;}{\\f4\\ftech Wingdings;}{\\f5\\ftech Monotype Sorts;}\n}"));
  /*
   * Style sheet
   */
  wxFprintf(fd, _T("{\\stylesheet{\\f2\\fs22\\sa200 \\snext0 Normal;}\n"));
  // Headings
  wxFprintf(fd, _T("{\\s1 ")); WriteHeadingStyle(fd, 1); wxFprintf(fd, _T("\\sbasedon0\\snext0 heading 1;}\n"));
  wxFprintf(fd, _T("{\\s2 ")); WriteHeadingStyle(fd, 2); wxFprintf(fd, _T("\\sbasedon0\\snext0 heading 2;}\n"));
  wxFprintf(fd, _T("{\\s3 ")); WriteHeadingStyle(fd, 3); wxFprintf(fd, _T("\\sbasedon0\\snext0 heading 3;}\n"));
  wxFprintf(fd, _T("{\\s4 ")); WriteHeadingStyle(fd, 4); wxFprintf(fd, _T("\\sbasedon0\\snext0 heading 4;}\n"));

  // Code style
  wxFprintf(fd, _T("{\\s10\\ql \\li720\\ri0\\nowidctlpar\\faauto\\rin0\\lin720\\itap0 \\cbpat17\
\\f2\\fs20 \\sbasedon0 \\snext24 Code;}\n"));

  // Table of contents styles
  wxFprintf(fd, _T("{\\s20\\sb300\\tqr\\tldot\\tx8640 \\b\\f2 \\sbasedon0\\snext0 toc 1;}\n"));

  wxFprintf(fd, _T("{\\s21\\sb90\\tqr\\tldot\\li400\\tqr\\tx8640 \\f2\\fs20\\sbasedon0\\snext0 toc 2;}\n"));
  wxFprintf(fd, _T("{\\s22\\sb90\\tqr\\tldot\\li800\\tx8640 \\f2\\fs20 \\sbasedon0\\snext0 toc 3;}\n"));
  wxFprintf(fd, _T("{\\s23\\sb90\\tqr\\tldot\\li1200\\tx8640 \\f2\\fs20 \\sbasedon0\\snext0 toc 4;}\n"));

  // Index styles
  wxFprintf(fd, _T("{\\s30\\fi-200\\li200\\tqr\\tx3960 \\f2\\fs18 \\sbasedon0\\snext0 index 1;}\n"));
  wxFprintf(fd, _T("{\\s31\\fi-200\\li400\\tqr\\tx3960 \\f2\\fs18 \\sbasedon0\\snext0 index 2;}\n"));
  wxFprintf(fd, _T("{\\s32\\fi-200\\li600\\tqr\\tx3960 \\f2\\fs18 \\sbasedon0\\snext0 index 3;}\n"));
  wxFprintf(fd, _T("{\\s33\\fi-200\\li800\\tqr\\tx3960 \\f2\\fs18 \\sbasedon0\\snext0 index 4;}\n"));
  wxFprintf(fd, _T("{\\s35\\qc\\sb240\\sa120 \\b\\f2\\fs26 \\sbasedon0\\snext30 index heading;}\n"));
  wxFprintf(fd, _T("}\n"));

  WriteColourTable(fd);
  wxFprintf(fd, _T("\n\\ftnbj\\ftnrestart")); // Latex default is footnotes at bottom of page, not section.
  wxFprintf(fd, _T("\n"));
}

void OutputNumberStyle(wxChar *numberStyle)
{
  if (numberStyle)
  {
    if (wxStrcmp(numberStyle, _T("arabic")) == 0)
    {
      TexOutput(_T("\\pgndec"));
    }
    else if (wxStrcmp(numberStyle, _T("roman")) == 0)
    {
      TexOutput(_T("\\pgnlcrm"));
    }
    else if (wxStrcmp(numberStyle, _T("Roman")) == 0)
    {
      TexOutput(_T("\\pgnucrm"));
    }
    else if (wxStrcmp(numberStyle, _T("alph")) == 0)
    {
      TexOutput(_T("\\pgnlcltr"));
    }
    else if (wxStrcmp(numberStyle, _T("Alph")) == 0)
    {
      TexOutput(_T("\\pgnucltr"));
    }
  }
}

/*
 * Write a Windows help project file
 */

bool WriteHPJ(const wxString& filename)
{
    wxChar hpjFilename[256];
    wxChar helpFile[50];
    wxChar rtfFile[50];
    wxStrcpy(hpjFilename, filename);
    StripExtension(hpjFilename);
    wxStrcat(hpjFilename, _T(".hpj"));

    wxStrcpy(helpFile, wxFileNameFromPath(filename));
    StripExtension(helpFile);
    wxStrcpy(rtfFile, helpFile);
    wxStrcat(helpFile, _T(".hlp"));
    wxStrcat(rtfFile, _T(".rtf"));

    FILE *fd = wxFopen(hpjFilename, _T("w"));
    if (!fd)
        return false;

    wxChar *helpTitle = winHelpTitle;
    if (!helpTitle)
        helpTitle = _T("Untitled");

    wxString thePath = wxPathOnly(InputFile);
    if (thePath.empty())
        thePath = _T(".");
    wxFprintf(fd, _T("[OPTIONS]\n"));
    wxFprintf(fd, _T("BMROOT=%s ; Assume that bitmaps are where the source is\n"), thePath.c_str());
    wxFprintf(fd, _T("TITLE=%s\n"), helpTitle);
    wxFprintf(fd, _T("CONTENTS=Contents\n"));

    if (winHelpVersion > 3)
    {
        wxFprintf(fd, _T("; COMPRESS=12 Hall Zeck ; Max compression, but needs lots of memory\n"));
        wxFprintf(fd, _T("COMPRESS=8 Zeck\n"));
        wxFprintf(fd, _T("LCID=0x809 0x0 0x0 ;English (British)\n"));
        wxFprintf(fd, _T("HLP=.\\%s.hlp\n"), wxFileNameFromPath(FileRoot));
    }
    else
    {
        wxFprintf(fd, _T("COMPRESS=HIGH\n"));
    }
    wxFprintf(fd, _T("\n"));

    if (winHelpVersion > 3)
    {
        wxFprintf(fd, _T("[WINDOWS]\n"));
        wxFprintf(fd, _T("Main=\"\",(553,102,400,600),20736,(r14876671),(r12632256),f3\n"));
        wxFprintf(fd, _T("\n"));
    }

    wxFprintf(fd, _T("[FILES]\n%s\n\n"), rtfFile);
    wxFprintf(fd, _T("[CONFIG]\n"));
    if (useUpButton)
        wxFprintf(fd, _T("CreateButton(\"Up\", \"&Up\", \"JumpId(`%s', `Contents')\")\n"), helpFile);
    wxFprintf(fd, _T("BrowseButtons()\n\n"));
    wxFprintf(fd, _T("[MAP]\n\n[BITMAPS]\n\n"));
    fclose(fd);
    return true;
}


/*
 * Given a TexChunk with a string value, scans through the string
 * converting Latex-isms into RTF-isms, such as 2 newlines -> \par,
 * and inserting spaces at the start of lines since in Latex, a newline
 * implies a space, but not in RTF.
 *
 */

void ProcessText2RTF(TexChunk *chunk)
{
  bool changed = false;
  int ptr = 0;
  int i = 0;
  wxChar ch = 1;
  int len = wxStrlen(chunk->value);
  while (ch != 0)
  {
    ch = chunk->value[i];

    if (ch == 10)
    {
      if (inVerbatim)
      {
        BigBuffer[ptr] = 0; wxStrcat(BigBuffer, _T("\\par\n")); ptr += 5;
//        BigBuffer[ptr] = 0; wxStrcat(BigBuffer, _T("\\par{\\v this was verbatim}\n")); ptr += 5;
        i ++;
        changed = true;
      }
      else
      {
        // If the first character of the next line is ASCII,
        // put a space in. Implicit in Latex, not in RTF.
        /*
          The reason this is difficult is that you don't really know
          where a space would be appropriate. If you always put in a space
          when you find a newline, unwanted spaces appear in the text.
         */
        if ((i > 0) && (len > i+1 && isascii(chunk->value[i+1]) &&
                  !isspace(chunk->value[i+1])) ||
            ((len > i+1 && chunk->value[i+1] == 13) &&
             (len > i+2 && isascii(chunk->value[i+2]) &&
              !isspace(chunk->value[i+2]))))
//        if (true)
        {
          // DOS files have a 13 after the 10
          BigBuffer[ptr] = 10;
          ptr ++;
          i ++;
          if (chunk->value[i] == 13)
          {
            BigBuffer[ptr] = 13;
            ptr ++;
            i ++;
          }

          BigBuffer[ptr] = ' ';
          ptr ++;

          // Note that the actual ASCII character seen is dealt with in the next
          // iteration
          changed = true;
        }
        else
        {
          BigBuffer[ptr] = ch;
          i ++;
        }
      }
    }
    else if (!inVerbatim && ch == '`' && (len >= i+1 && chunk->value[i+1] == '`'))
    {
      BigBuffer[ptr] = '"'; ptr ++;
      i += 2;
      changed = true;
    }
    else if (!inVerbatim && ch == '`') // Change ` to '
    {
      BigBuffer[ptr] = 39; ptr ++;
      i += 1;
      changed = true;
    }
    else if (inVerbatim && ch == '\\') // Change backslash to two backslashes
    {
      BigBuffer[ptr] = '\\'; ptr ++;
      BigBuffer[ptr] = '\\'; ptr ++;
      i += 1;
      changed = true;
    }
    else if (inVerbatim && (ch == '{' || ch == '}')) // Escape the curly bracket
    {
      BigBuffer[ptr] = '\\'; ptr ++;
      BigBuffer[ptr] = ch; ptr ++;
      i += 1;
      changed = true;
    }
    else
    {
      BigBuffer[ptr] = ch;
      i ++;
      ptr ++;
    }
  }
  BigBuffer[ptr] = 0;

  if (changed)
  {
    delete[] chunk->value;
    chunk->value = copystring(BigBuffer);
  }
}

/*
 * Scan through all chunks starting from the given one,
 * calling ProcessText2RTF to convert Latex-isms to RTF-isms.
 * This should be called after Tex2Any has parsed the file,
 * and before TraverseDocument is called.
 *
 */

void Text2RTF(TexChunk *chunk)
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

      if (def && (def->macroId == ltVERBATIM || def->macroId == ltVERB))
        inVerbatim = true;

      wxNode *node = chunk->children.GetFirst();
      while (node)
      {
        TexChunk *child_chunk = (TexChunk *)node->GetData();
        Text2RTF(child_chunk);
        node = node->GetNext();
      }

      if (def && (def->macroId == ltVERBATIM || def->macroId == ltVERB))
        inVerbatim = false;

      break;
    }
    case CHUNK_TYPE_ARG:
    {
      wxNode *node = chunk->children.GetFirst();
      while (node)
      {
        TexChunk *child_chunk = (TexChunk *)node->GetData();
        Text2RTF(child_chunk);
        node = node->GetNext();
      }

      break;
    }
    case CHUNK_TYPE_STRING:
    {
      if (chunk->value)
        ProcessText2RTF(chunk);
      break;
    }
  }
}

/*
 * Not used yet
 *
 */

wxChar browseBuf[10];
static long browseId = 0;
wxChar *GetBrowseString(void)
{
  wxChar buf[10];
  browseId ++;
  wxSnprintf(buf, sizeof(buf), _T("%ld"), browseId);
  int noZeroes = 5-wxStrlen(buf);
  wxStrcpy(browseBuf, _T("browse"));
  for (int i = 0; i < noZeroes; i++)
    wxStrcat(browseBuf, _T("0"));
  wxStrcat(browseBuf, buf);
  return browseBuf;
}

/*
 * Keeping track of environments to restore the styles after \pard.
 * Push strings like "\qc" onto stack.
 *
 */

void PushEnvironmentStyle(wxChar *style)
{
  environmentStack.Add(style);
}

void PopEnvironmentStyle(void)
{
  wxStringListNode *node = environmentStack.GetLast();
  if (node)
  {
    wxChar *val = (wxChar *)node->GetData();
    delete[] val;
    delete node;
  }
}

// Write out the styles, most recent first.
void WriteEnvironmentStyles(void)
{
  wxStringListNode *node = environmentStack.GetLast();
  while (node)
  {
    wxChar *val = (wxChar *)node->GetData();
    TexOutput(val);
    node = node->GetNext();
  }
  if (!inTabular && (ParIndent > 0) && (forbidParindent == 0))
  {
    wxChar buf[15];
    wxSnprintf(buf, sizeof(buf), _T("\\fi%d"), ParIndent*20); // Convert points to TWIPS
    TexOutput(buf);
  }
  if (environmentStack.GetCount() > 0 || (ParIndent > 0))
    TexOutput(_T("\n"));
}


/*
 * Output a header
 *
 */

void OutputRTFHeaderCommands(void)
{
  wxChar buf[300];
  if (PageStyle && wxStrcmp(PageStyle, _T("plain")) == 0)
  {
    TexOutput(_T("{\\headerl }{\\headerr }"));
  }
  else if (PageStyle && wxStrcmp(PageStyle, _T("empty")) == 0)
  {
    TexOutput(_T("{\\headerl }{\\headerr }"));
  }
  else if (PageStyle && wxStrcmp(PageStyle, _T("headings")) == 0)
  {
    // Left header
    TexOutput(_T("{\\headerl\\fi0 "));

    if (headerRule)
      TexOutput(_T("\\brdrb\\brdrs\\brdrw15\\brsp20 "));

    TexOutput(_T("{\\i \\qr "));
    if (DocumentStyle == LATEX_ARTICLE)
    {
      wxSnprintf(buf, sizeof(buf), _T("SECTION %d"), sectionNo);
      TexOutput(buf);
    }
    else
    {
      wxSnprintf(buf, sizeof(buf), _T("CHAPTER %d: "), chapterNo);
      TexOutput(buf);
    }
    TexOutput(_T("{\\field{\\*\\fldinst PAGE \\\\* MERGEFORMAT }{\\fldrslt 1}}"));
    TexOutput(_T("}\\par\\pard}"));

    // Right header
    TexOutput(_T("{\\headerr\\fi0 "));

    if (headerRule)
      TexOutput(_T("\\brdrb\\brdrs\\brdrw15\\brsp20 "));

    TexOutput(_T("{\\i \\qc "));
    if (DocumentStyle == LATEX_ARTICLE)
    {
      wxSnprintf(buf, sizeof(buf), _T("SECTION %d"), sectionNo);
      TexOutput(buf);
    }
    else
    {
      wxSnprintf(buf, sizeof(buf), _T("CHAPTER %d"), chapterNo);
      TexOutput(buf);
    }
    TexOutput(_T("{\\field{\\*\\fldinst PAGE \\\\* MERGEFORMAT }{\\fldrslt 1}}"));
    TexOutput(_T("}\\par\\pard}"));
  }
  else
  {
    int oldForbidResetPar = forbidResetPar;
    forbidResetPar = 0;

    if (LeftHeaderEven || CentreHeaderEven || RightHeaderEven)
    {
      TexOutput(_T("{\\headerl\\fi0 "));

      if (headerRule)
        TexOutput(_T("\\brdrb\\brdrs\\brdrw15\\brsp20 "));

      if (LeftHeaderEven)
      {
        if (!CentreHeaderEven && !RightHeaderEven)
          TexOutput(_T("\\ql "));
        TraverseChildrenFromChunk(LeftHeaderEven);
      }
      if (CentreHeaderEven)
      {
        if (!LeftHeaderEven && !RightHeaderEven)
          TexOutput(_T("\\qc "));
        else
          TexOutput(_T("\\tab\\tab\\tab "));
        TraverseChildrenFromChunk(CentreHeaderEven);
      }
      if (RightHeaderEven)
      {
        if (!LeftHeaderEven && !CentreHeaderEven)
          TexOutput(_T("\\qr "));
        else
          TexOutput(_T("\\tab\\tab\\tab\\tab\\tab\\tab "));
        TraverseChildrenFromChunk(RightHeaderEven);
      }
      TexOutput(_T("\\par\\pard}"));
    }

    if (LeftHeaderOdd || CentreHeaderOdd || RightHeaderOdd)
    {
      TexOutput(_T("{\\headerr\\fi0 "));

      if (headerRule)
        TexOutput(_T("\\brdrb\\brdrs\\brdrw15\\brsp20 "));

      if (LeftHeaderOdd)
      {
        if (!CentreHeaderOdd && !RightHeaderOdd)
          TexOutput(_T("\\ql "));
        TraverseChildrenFromChunk(LeftHeaderOdd);
      }
      if (CentreHeaderOdd)
      {
        if (!LeftHeaderOdd && !RightHeaderOdd)
          TexOutput(_T("\\qc "));
        else
          TexOutput(_T("\\tab\\tab\\tab "));
        TraverseChildrenFromChunk(CentreHeaderOdd);
      }
      if (RightHeaderOdd)
      {
        if (!LeftHeaderOdd && !CentreHeaderOdd)
          TexOutput(_T("\\qr "));
        else
          TexOutput(_T("\\tab\\tab\\tab\\tab\\tab\\tab "));
        TraverseChildrenFromChunk(RightHeaderOdd);
      }
      TexOutput(_T("\\par\\pard}"));
    }
    // As an approximation, don't put a header on the first page of a section.
    // This may not always be desired, but it's a reasonable guess.
    TexOutput(_T("{\\headerf }"));

    forbidResetPar = oldForbidResetPar;
  }
}

void OutputRTFFooterCommands(void)
{
  if (PageStyle && wxStrcmp(PageStyle, _T("plain")) == 0)
  {
    TexOutput(_T("{\\footerl\\fi0 "));
    if (footerRule)
      TexOutput(_T("\\brdrt\\brdrs\\brdrw15\\brsp20 "));
    TexOutput(_T("{\\qc "));
    TexOutput(_T("{\\field{\\*\\fldinst PAGE \\\\* MERGEFORMAT }{\\fldrslt 1}}"));
    TexOutput(_T("}\\par\\pard}"));

    TexOutput(_T("{\\footerr\\fi0 "));
    if (footerRule)
      TexOutput(_T("\\brdrt\\brdrs\\brdrw15\\brsp20 "));
    TexOutput(_T("{\\qc "));
    TexOutput(_T("{\\field{\\*\\fldinst PAGE \\\\* MERGEFORMAT }{\\fldrslt 1}}"));
    TexOutput(_T("}\\par\\pard}"));
  }
  else if (PageStyle && wxStrcmp(PageStyle, _T("empty")) == 0)
  {
    TexOutput(_T("{\\footerl }{\\footerr }"));
  }
  else if (PageStyle && wxStrcmp(PageStyle, _T("headings")) == 0)
  {
    TexOutput(_T("{\\footerl }{\\footerr }"));
  }
  else
  {
    if (LeftFooterEven || CentreFooterEven || RightFooterEven)
    {
      TexOutput(_T("{\\footerl\\fi0 "));
      if (footerRule)
        TexOutput(_T("\\brdrt\\brdrs\\brdrw15\\brsp20 "));
      if (LeftFooterEven)
      {
        if (!CentreFooterEven && !RightFooterEven)
          TexOutput(_T("\\ql "));
        TraverseChildrenFromChunk(LeftFooterEven);
      }
      if (CentreFooterEven)
      {
        if (!LeftFooterEven && !RightFooterEven)
          TexOutput(_T("\\qc "));
        else
          TexOutput(_T("\\tab\\tab\\tab "));
        TraverseChildrenFromChunk(CentreFooterEven);
      }
      if (RightFooterEven)
      {
        if (!LeftFooterEven && !CentreFooterEven)
          TexOutput(_T("\\qr "));
        else
          TexOutput(_T("\\tab\\tab\\tab\\tab\\tab\\tab "));
        TraverseChildrenFromChunk(RightFooterEven);
      }
      TexOutput(_T("\\par\\pard}"));
    }

    if (LeftFooterOdd || CentreFooterOdd || RightFooterOdd)
    {
      TexOutput(_T("{\\footerr\\fi0 "));
      if (footerRule)
        TexOutput(_T("\\brdrt\\brdrs\\brdrw15\\brsp20 "));
      if (LeftFooterOdd)
      {
        if (!CentreFooterOdd && !RightFooterOdd)
          TexOutput(_T("\\ql "));
        TraverseChildrenFromChunk(LeftFooterOdd);
      }
      if (CentreFooterOdd)
      {
        if (!LeftFooterOdd && !RightFooterOdd)
          TexOutput(_T("\\qc "));
        else
          TexOutput(_T("\\tab\\tab\\tab "));
        TraverseChildrenFromChunk(CentreFooterOdd);
      }
      if (RightFooterOdd)
      {
        if (!LeftFooterOdd && !CentreFooterOdd)
          TexOutput(_T("\\qr "));
        else
          TexOutput(_T("\\tab\\tab\\tab\\tab\\tab\\tab "));
        TraverseChildrenFromChunk(RightFooterOdd);
      }
      TexOutput(_T("\\par\\pard}"));
    }

    // As an approximation, put a footer on the first page of a section.
    // This may not always be desired, but it's a reasonable guess.
    if (LeftFooterOdd || CentreFooterOdd || RightFooterOdd)
    {
      TexOutput(_T("{\\footerf\\fi0 "));
      if (LeftFooterOdd)
      {
        if (!CentreFooterOdd && !RightFooterOdd)
          TexOutput(_T("\\ql "));
        TraverseChildrenFromChunk(LeftFooterOdd);
      }
      if (CentreFooterOdd)
      {
        if (!LeftFooterOdd && !RightFooterOdd)
          TexOutput(_T("\\qc "));
        else
          TexOutput(_T("\\tab\\tab\\tab "));
        TraverseChildrenFromChunk(CentreFooterOdd);
      }
      if (RightFooterOdd)
      {
        if (!LeftFooterOdd && !CentreFooterOdd)
          TexOutput(_T("\\qr "));
        else
          TexOutput(_T("\\tab\\tab\\tab\\tab\\tab\\tab "));
        TraverseChildrenFromChunk(RightFooterOdd);
      }
      TexOutput(_T("\\par\\pard}"));
    }
  }
}

// Called on start/end of macro examination
void RTFOnMacro(int macroId, int no_args, bool start)
{
/*
  wxChar tmpBuf[40];
  wxSnprintf(tmpBuf, sizeof(tmpBuf), _T("%d (%d)"), macroId, (int)start);
  OutputDebugString("RTFOnMacro Start "); OutputDebugString(tmpBuf);
  OutputDebugString("\n"); wxYield();
*/

  // ltLABEL is included here because after a section but BEFORE
  // the label is seen, a new paragraph is issued. Don't upset this by
  // immediately forgetting we've done it.
  if (start && (macroId != ltPAR && macroId != ltITEMIZE &&
                        macroId != ltENUMERATE && macroId != ltDESCRIPTION &&
                        macroId != ltVERBATIM && macroId != ltLABEL &&
                        macroId != ltSETHEADER && macroId != ltSETFOOTER &&
                        macroId != ltPAGENUMBERING &&
                        (forbidResetPar == 0)))
  {
    issuedNewParagraph = 0;
  }

  wxChar buf[300];
  switch (macroId)
  {
  case ltCHAPTER:
  case ltCHAPTERSTAR:
  case ltCHAPTERHEADING:
  case ltCHAPTERHEADINGSTAR:
  {
    if (!start)
    {
      sectionNo = 0;
      figureNo = 0;
      tableNo = 0;
      subsectionNo = 0;
      subsubsectionNo = 0;
      footnoteCount = 0;

      if (macroId != ltCHAPTERSTAR && macroId != ltCHAPTERHEADINGSTAR)
        chapterNo ++;

      wxChar *topicName = FindTopicName(GetNextChunk());
      SetCurrentChapterName(topicName);

      if (winHelpContents && winHelp && !InPopups())
      {
        OutputCurrentSectionToString(wxTex2RTFBuffer);
        WriteWinHelpContentsFileLine(topicName, wxTex2RTFBuffer, 1);
      }
      AddTexRef(topicName, NULL, ChapterNameString, chapterNo);

      if (winHelp)
      {
        if (!InPopups())
          wxFprintf(Contents, _T("\n{\\uldb "));
        wxFprintf(Chapters, _T("\\page"));
        wxFprintf(Chapters, _T("\n${\\footnote "));
        if (!InPopups())
          SetCurrentOutputs(Contents, Chapters);
        else
          SetCurrentOutput(Chapters);
      }
      else
      {
        wxFprintf(Chapters, _T("\\sect\\pgncont\\titlepg\n"));

        // If a non-custom page style, we generate the header now.
        if (PageStyle && (wxStrcmp(PageStyle, _T("plain")) == 0 ||
                          wxStrcmp(PageStyle, _T("empty")) == 0 ||
                          wxStrcmp(PageStyle, _T("headings")) == 0))
        {
          OutputRTFHeaderCommands();
          OutputRTFFooterCommands();
        }

        // Need to reset the current numbering style, or RTF forgets it.
        SetCurrentOutput(Chapters);
        OutputNumberStyle(currentNumberStyle);

        SetCurrentOutput(Contents);

        if (!InPopups())
        {
          if (macroId == ltCHAPTER)
          {
            // Section
            wxFprintf(Contents, _T("\\par\n\\pard{\\b %d\\tab "), chapterNo);
          }
          else if (macroId == ltCHAPTERHEADING)
          {
            wxFprintf(Contents, _T("\\par\n\\pard{\\b "));
          }
          else SetCurrentOutput(NULL); // No entry in table of contents
        }
      }

      startedSections = true;

      // Output heading to contents page
      if (!InPopups())
      {
        OutputCurrentSection();

        if (winHelp)
        {
          wxFprintf(Contents, _T("}{\\v %s}\\pard\\par\n"), topicName);
          //WriteEnvironmentStyles();
        }
        else if ((macroId == ltCHAPTER) || (macroId == ltCHAPTERHEADING))
          wxFprintf(Contents, _T("}\\par\\par\\pard\n"));

        // From here, just output to chapter
        SetCurrentOutput(Chapters);
      }

      if (winHelp)
      {
        wxFprintf(Chapters, _T("}\n#{\\footnote %s}\n"), topicName);
        wxFprintf(Chapters, _T("+{\\footnote %s}\n"), GetBrowseString());

        OutputSectionKeyword(Chapters);

        GenerateKeywordsForTopic(topicName);
        if (useUpButton)
        {
          // If we're generating a .cnt file, we don't want to be able
          // jump up to the old-style contents page, so disable it.
          if (winHelpContents)
            wxFprintf(Chapters, _T("!{\\footnote DisableButton(\"Up\")}\n"));
          else
            wxFprintf(Chapters, _T("!{\\footnote EnableButton(\"Up\");ChangeButtonBinding(\"Up\", \"JumpId(`%s.hlp', `%s')\")}\n"),
               wxFileNameFromPath(FileRoot), _T("Contents"));
        }
      }

      if (!InPopups())
      {
      wxChar *styleCommand = _T("");
      if (!winHelp && useHeadingStyles && (macroId == ltCHAPTER || macroId == ltCHAPTERHEADING || macroId == ltCHAPTERHEADINGSTAR))
        styleCommand = _T("\\s1");
      wxFprintf(Chapters, _T("\\pard{%s"), ((winHelp && !InPopups()) ? _T("\\keepn\\sa140\\sb140") : styleCommand));
      WriteHeadingStyle(Chapters, 1);  wxFprintf(Chapters, _T(" "));
      if (!winHelp)
      {
        if (macroId == ltCHAPTER)
        {
          if (useWord)
//            wxFprintf(Chapters, "{\\bkmkstart %s}%d{\\bkmkend %s}. ", topicName, chapterNo,
            wxFprintf(Chapters, _T("{\\bkmkstart %s}{\\bkmkend %s}"), topicName, topicName);
          else
            wxFprintf(Chapters, _T("%d. "), chapterNo);
        }
        else if ( useWord )
        {
              wxFprintf(Chapters, _T("{\\bkmkstart %s}{\\bkmkend %s}"), topicName, topicName);
        }
      }
      OutputCurrentSection();
      TexOutput(_T("\\par\\pard}\n"));
      }
      issuedNewParagraph = 1;
      WriteEnvironmentStyles();
    }
    break;
  }
  case ltSECTION:
  case ltSECTIONSTAR:
  case ltSECTIONHEADING:
  case ltSECTIONHEADINGSTAR:
  case ltGLOSS:
  {
    FILE *jumpFrom;
    if (DocumentStyle == LATEX_ARTICLE)
      jumpFrom = Contents;
    else
      jumpFrom = Chapters;

    if (!start)
    {
      subsectionNo = 0;
      subsubsectionNo = 0;
      if (DocumentStyle == LATEX_ARTICLE)
        footnoteCount = 0;

      if (macroId != ltSECTIONSTAR && macroId != ltSECTIONHEADINGSTAR)
        sectionNo ++;

      wxChar *topicName = FindTopicName(GetNextChunk());
      SetCurrentSectionName(topicName);
      NotifyParentHasChildren(1);
      if (winHelpContents && winHelp && !InPopups())
      {
        OutputCurrentSectionToString(wxTex2RTFBuffer);
        WriteWinHelpContentsFileLine(topicName, wxTex2RTFBuffer, 2);
      }
      AddTexRef(topicName, NULL, SectionNameString, chapterNo, sectionNo);

      if (winHelp)
      {
        SetCurrentOutputs(jumpFrom, Sections);
        // Newline for a new section if this is an article
        if ((DocumentStyle == LATEX_ARTICLE) &&
            ((macroId == ltSECTION) || (macroId == ltSECTIONSTAR) || (macroId == ltSECTIONHEADINGSTAR)))
          wxFprintf(Sections, _T("\\page\n"));

        if (!InPopups())
          wxFprintf(jumpFrom, _T("\n{\\uldb "));
      }
      else
      {
        if (DocumentStyle == LATEX_ARTICLE)
        {
          TexOutput(_T("\\sect\\pgncont\n"));
          // If a non-custom page style, we generate the header now.
          if (PageStyle && (wxStrcmp(PageStyle, _T("plain")) == 0 ||
                            wxStrcmp(PageStyle, _T("empty")) == 0 ||
                            wxStrcmp(PageStyle, _T("headings")) == 0))
          {
            OutputRTFHeaderCommands();
            OutputRTFFooterCommands();
          }
        }
        SetCurrentOutput(Contents);

        if (macroId == ltSECTION)
        {
          if (!InPopups())
          {
            if (DocumentStyle == LATEX_REPORT)
              wxFprintf(Contents, _T("\n\\pard{\\tab %d.%d\\tab "), chapterNo, sectionNo);
            else
              wxFprintf(Contents, _T("\\par\n\\pard{\\b %d\\tab "), sectionNo);
          }
        }
        else if (macroId == ltSECTIONHEADING)
        {
          if (!InPopups())
          {
            if (DocumentStyle == LATEX_REPORT)
              wxFprintf(Contents, _T("\n\\pard{\\tab ")); //, chapterNo, sectionNo);
            else
              wxFprintf(Contents, _T("\\par\n\\pard{\\b ")); //, sectionNo);
          }
        }
        else SetCurrentOutput(NULL);
      }

      if (startedSections)
      {
        if (winHelp)
          wxFprintf(Sections, _T("\\page\n"));
      }
      startedSections = true;

      if (winHelp)
        wxFprintf(Sections, _T("\n${\\footnote "));

      // Output heading to contents page
      if (!InPopups())
        OutputCurrentSection();

      if (winHelp)
      {
        if (!InPopups())
        {
          wxFprintf(jumpFrom, _T("}{\\v %s}\\pard\\par\n"), topicName);
          //WriteEnvironmentStyles();
        }
      }
      else if ((macroId != ltSECTIONSTAR) && (macroId != ltGLOSS))
      {
        if (DocumentStyle == LATEX_REPORT)
          wxFprintf(Contents, _T("}\\par\\pard\n"));
        else
          wxFprintf(Contents, _T("}\\par\\par\\pard\n"));
      }

      SetCurrentOutput(winHelp ? Sections : Chapters);

      if (winHelp)
      {
        wxFprintf(Sections, _T("}\n#{\\footnote %s}\n"), topicName);
        wxFprintf(Sections, _T("+{\\footnote %s}\n"), GetBrowseString());
        OutputSectionKeyword(Sections);
        GenerateKeywordsForTopic(topicName);
        if (useUpButton)
        {
          if (DocumentStyle == LATEX_ARTICLE)
          {
            wxFprintf(Sections, _T("!{\\footnote EnableButton(\"Up\");ChangeButtonBinding(\"Up\", \"JumpId(`%s.hlp', `%s')\")}\n"),
               wxFileNameFromPath(FileRoot), _T("Contents"));
          }
          else if (CurrentChapterName)
          {
            wxFprintf(Sections, _T("!{\\footnote EnableButton(\"Up\");ChangeButtonBinding(\"Up\", \"JumpId(`%s.hlp', `%s')\")}\n"),
               wxFileNameFromPath(FileRoot), CurrentChapterName);
          }
        }
      }

      if (!InPopups())
      {
      wxChar *styleCommand = _T("");
      if (!winHelp && useHeadingStyles && (macroId != ltSECTIONSTAR))
      {
        if (DocumentStyle == LATEX_ARTICLE)
          styleCommand = _T("\\s1");
        else
          styleCommand = _T("\\s2");
      }
      wxChar *keep = _T("");
      if (winHelp && (macroId != ltGLOSS) && !InPopups())
        keep = _T("\\keepn\\sa140\\sb140");

      wxFprintf(winHelp ? Sections : Chapters, _T("\\pard{%s%s"),
         keep, styleCommand);

      WriteHeadingStyle((winHelp ? Sections : Chapters),
                        (DocumentStyle == LATEX_ARTICLE ? 1 : 2));
      wxFprintf(winHelp ? Sections : Chapters, _T(" "));

      if (!winHelp)
      {
        if ((macroId != ltSECTIONSTAR) && (macroId != ltSECTIONHEADINGSTAR) && (macroId != ltGLOSS))
        {
          if (DocumentStyle == LATEX_REPORT)
          {
            if (useWord)
//              wxFprintf(Chapters, _T("{\\bkmkstart %s}%d.%d{\\bkmkend %s}. "), topicName, chapterNo, sectionNo,
              wxFprintf(Chapters, _T("{\\bkmkstart %s}{\\bkmkend %s}"), topicName,
                  topicName);
            else
              wxFprintf(Chapters, _T("%d.%d. "), chapterNo, sectionNo);
          }
          else
          {
            if (useWord)
//              wxFprintf(Chapters, "{\\bkmkstart %s}%d{\\bkmkend %s}. ", topicName, sectionNo,
              wxFprintf(Chapters, _T("{\\bkmkstart %s}{\\bkmkend %s}"), topicName,
                 topicName);
            else
              wxFprintf(Chapters, _T("%d. "), sectionNo);
          }
        }
        else if ( useWord )
        {
              wxFprintf(Chapters, _T("{\\bkmkstart %s}{\\bkmkend %s}"), topicName, topicName);
        }
      }
      OutputCurrentSection();
      TexOutput(_T("\\par\\pard}\n"));
//      TexOutput(_T("\\par\\pard}\\par\n"));
      }
      issuedNewParagraph = 1;
      WriteEnvironmentStyles();
//      issuedNewParagraph = 2;
    }
    break;
  }
  case ltSUBSECTION:
  case ltSUBSECTIONSTAR:
  case ltMEMBERSECTION:
  case ltFUNCTIONSECTION:
  {
    if (!start)
    {
      if (winHelp && !Sections)
      {
        OnError(_T("You cannot have a subsection before a section!"));
      }
      else
      {
      subsubsectionNo = 0;

      if (macroId != ltSUBSECTIONSTAR)
        subsectionNo ++;

      wxChar *topicName = FindTopicName(GetNextChunk());
      SetCurrentSubsectionName(topicName);
      NotifyParentHasChildren(2);
      if (winHelpContents && winHelp && !InPopups())
      {
        OutputCurrentSectionToString(wxTex2RTFBuffer);
        WriteWinHelpContentsFileLine(topicName, wxTex2RTFBuffer, 3);
      }
      AddTexRef(topicName, NULL, SectionNameString, chapterNo, sectionNo, subsectionNo);

      if (winHelp)
      {
        SetCurrentOutputs(Sections, Subsections);
        SetCurrentOutputs(Sections, Subsections);
        if (!InPopups())
          wxFprintf(Sections, _T("\n{\\uldb "));
      }
      else
      {
        if ((macroId != ltSUBSECTIONSTAR) && (macroId != ltMEMBERSECTION) &&
            (macroId != ltFUNCTIONSECTION))
        {
          SetCurrentOutput(Contents);
          if (DocumentStyle == LATEX_REPORT)
            wxFprintf(Contents, _T("\n\\pard\\tab\\tab %d.%d.%d\\tab "), chapterNo, sectionNo, subsectionNo);
          else
            wxFprintf(Contents, _T("\n\\pard\\tab %d.%d\\tab "), sectionNo, subsectionNo);
        } else SetCurrentOutput(NULL);
      }
      if (startedSections)
      {
        if (winHelp)
        {
          if (!InPopups())
            wxFprintf(Subsections, _T("\\page\n"));
        }
        // Experimental JACS 2004-02-21
#if 0
        else
          wxFprintf(Chapters, _T("\\par\n"));
#endif
      }
      startedSections = true;

      if (winHelp)
        wxFprintf(Subsections, _T("\n${\\footnote "));

      // Output to contents page
      if (!InPopups())
        OutputCurrentSection();

      if (winHelp)
      {
        if (!InPopups())
        {
          wxFprintf(Sections, _T("}{\\v %s}\\pard\\par\n"), topicName);
          //WriteEnvironmentStyles();
        }
      }
      else if ((macroId != ltSUBSECTIONSTAR) && (macroId != ltMEMBERSECTION) &&
         (macroId != ltFUNCTIONSECTION))
        wxFprintf(Contents, _T("\\par\\pard\n"));

      SetCurrentOutput(winHelp ? Subsections : Chapters);
      if (winHelp)
      {
        wxFprintf(Subsections, _T("}\n#{\\footnote %s}\n"), topicName);
        wxFprintf(Subsections, _T("+{\\footnote %s}\n"), GetBrowseString());
        OutputSectionKeyword(Subsections);
        GenerateKeywordsForTopic(topicName);
        if (useUpButton && CurrentSectionName)
        {
          wxFprintf(Subsections, _T("!{\\footnote EnableButton(\"Up\");ChangeButtonBinding(\"Up\", \"JumpId(`%s.hlp', `%s')\")}\n"),
             wxFileNameFromPath(FileRoot), CurrentSectionName);
        }
      }
      if (!winHelp && indexSubsections && useWord)
      {
        // Insert index entry for this subsection
        TexOutput(_T("{\\xe\\v {"));
        OutputCurrentSection();
        TexOutput(_T("}}"));
      }

      if (!InPopups())
      {
      wxChar *styleCommand = _T("");
      if (!winHelp && useHeadingStyles && (macroId != ltSUBSECTIONSTAR))
      {
        if (DocumentStyle == LATEX_ARTICLE)
          styleCommand = _T("\\s2");
        else
          styleCommand = _T("\\s3");
      }
      wxChar *keep = _T("");
      if (winHelp && !InPopups())
        keep = _T("\\keepn\\sa140\\sb140");

      wxFprintf(winHelp ? Subsections : Chapters, _T("\\pard{%s%s"),
         keep, styleCommand);

      WriteHeadingStyle((winHelp ? Subsections : Chapters),
                        (DocumentStyle == LATEX_ARTICLE ? 2 : 3));
      wxFprintf(winHelp ? Subsections : Chapters, _T(" "));

      if (!winHelp)
      {
        if ((macroId != ltSUBSECTIONSTAR) && (macroId != ltMEMBERSECTION) &&
         (macroId != ltFUNCTIONSECTION))
        {
          if (DocumentStyle == LATEX_REPORT)
          {
            if (useWord)
//              wxFprintf(Chapters, _T("{\\bkmkstart %s}%d.%d.%d{\\bkmkend %s}. "), topicName, chapterNo, sectionNo, subsectionNo,
              wxFprintf(Chapters, _T("{\\bkmkstart %s}{\\bkmkend %s}"), topicName,
                       topicName);
            else
              wxFprintf(Chapters, _T("%d.%d.%d. "), chapterNo, sectionNo, subsectionNo);
          }
          else
          {
            if (useWord)
//              wxFprintf(Chapters, _T("{\\bkmkstart %s}%d.%d{\\bkmkend %s}. "), topicName, sectionNo, subsectionNo,
              wxFprintf(Chapters, _T("{\\bkmkstart %s}{\\bkmkend %s}"), topicName,
                       topicName);
            else
              wxFprintf(Chapters, _T("%d.%d. "), sectionNo, subsectionNo);
          }
        }
        else if ( useWord )
        {
              wxFprintf(Chapters, _T("{\\bkmkstart %s}{\\bkmkend %s}"), topicName, topicName);
        }
      }
      OutputCurrentSection(); // Repeat section header

      // Experimental JACS
      TexOutput(_T("\\par\\pard}\n"));
      // TexOutput(_T("\\par\\pard}\\par\n"));
      }
      issuedNewParagraph = 1;
      WriteEnvironmentStyles();
    }
    }
    break;
  }
  case ltSUBSUBSECTION:
  case ltSUBSUBSECTIONSTAR:
  {
    if (!start)
    {
      if (winHelp && !Subsections)
      {
        OnError(_T("You cannot have a subsubsection before a subsection!"));
      }
      else
      {
      if (macroId != ltSUBSUBSECTIONSTAR)
        subsubsectionNo ++;

      wxChar *topicName = FindTopicName(GetNextChunk());
      SetCurrentTopic(topicName);
      NotifyParentHasChildren(3);
      if (winHelpContents && winHelp)
      {
        OutputCurrentSectionToString(wxTex2RTFBuffer);
        WriteWinHelpContentsFileLine(topicName, wxTex2RTFBuffer, 4);
      }
      AddTexRef(topicName, NULL, SectionNameString, chapterNo, sectionNo, subsectionNo, subsubsectionNo);

      if (winHelp)
      {
        SetCurrentOutputs(Subsections, Subsubsections);
        wxFprintf(Subsections, _T("\n{\\uldb "));
      }
      else
      {
        if (macroId != ltSUBSUBSECTIONSTAR)
        {
          if (DocumentStyle == LATEX_ARTICLE)
          {
            SetCurrentOutput(Contents);
            wxFprintf(Contents, _T("\n\\tab\\tab %d.%d.%d\\tab "),
                               sectionNo, subsectionNo, subsubsectionNo);
          }
          else
            SetCurrentOutput(NULL); // Don't write it into the contents, or anywhere else
        }
        else
          SetCurrentOutput(NULL); // Don't write it into the contents, or anywhere else
      }

      if (startedSections)
      {
        if (winHelp)
          wxFprintf(Subsubsections, _T("\\page\n"));
        // Experimental JACS 2004-02-21
#if 0
        else
          wxFprintf(Chapters, _T("\\par\n"));
#endif
      }

      startedSections = true;

      if (winHelp)
        wxFprintf(Subsubsections, _T("\n${\\footnote "));

      // Output header to contents page
      OutputCurrentSection();

      if (winHelp)
      {
        wxFprintf(Subsections, _T("}{\\v %s}\\pard\\par\n"), topicName);
        //WriteEnvironmentStyles();
      }
      else if ((DocumentStyle == LATEX_ARTICLE) && (macroId != ltSUBSUBSECTIONSTAR))
        wxFprintf(Contents, _T("\\par\\pard\n"));

      SetCurrentOutput(winHelp ? Subsubsections : Chapters);
      if (winHelp)
      {
        wxFprintf(Subsubsections, _T("}\n#{\\footnote %s}\n"), topicName);
        wxFprintf(Subsubsections, _T("+{\\footnote %s}\n"), GetBrowseString());
        OutputSectionKeyword(Subsubsections);
        GenerateKeywordsForTopic(topicName);
        if (useUpButton && CurrentSubsectionName)
        {
          wxFprintf(Subsubsections, _T("!{\\footnote EnableButton(\"Up\");ChangeButtonBinding(\"Up\", \"JumpId(`%s.hlp', `%s')\")}\n"),
             wxFileNameFromPath(FileRoot), CurrentSubsectionName);
        }
      }
      if (!winHelp && indexSubsections && useWord)
      {
        // Insert index entry for this subsubsection
        TexOutput(_T("{\\xe\\v {"));
        OutputCurrentSection();
        TexOutput(_T("}}"));
      }

      wxChar *styleCommand = _T("");
      if (!winHelp && useHeadingStyles && (macroId != ltSUBSUBSECTIONSTAR))
      {
        if (DocumentStyle == LATEX_ARTICLE)
          styleCommand = _T("\\s3");
        else
          styleCommand = _T("\\s4");
      }
      wxChar *keep = _T("");
      if (winHelp)
        keep = _T("\\keepn\\sa140\\sb140");

      wxFprintf(winHelp ? Subsubsections : Chapters, _T("\\pard{%s%s"),
         keep, styleCommand);

      WriteHeadingStyle((winHelp ? Subsubsections : Chapters),
                        (DocumentStyle == LATEX_ARTICLE ? 3 : 4));
      wxFprintf(winHelp ? Subsubsections : Chapters, _T(" "));

      if (!winHelp)
      {
        if ((macroId != ltSUBSUBSECTIONSTAR))
        {
          if (DocumentStyle == LATEX_ARTICLE)
          {
            if (useWord)
//              wxFprintf(Chapters, _T("{\\bkmkstart %s}%d.%d.%d{\\bkmkend %s}. "), topicName, sectionNo, subsectionNo, subsubsectionNo,
              wxFprintf(Chapters, _T("{\\bkmkstart %s}{\\bkmkend %s}"), topicName,
                     topicName);
            else
              wxFprintf(Chapters, _T("%d.%d.%d. "), sectionNo, subsectionNo, subsubsectionNo);
          }
          else
          {
            if (useWord)
//              wxFprintf(Chapters, _T("{\\bkmkstart %s}%d.%d.%d.%d{\\bkmkend %s}. "), topicName, chapterNo, sectionNo, subsectionNo, subsubsectionNo,
              wxFprintf(Chapters, _T("{\\bkmkstart %s}{\\bkmkend %s}"), topicName,
                      topicName);
            else
              wxFprintf(Chapters, _T("%d.%d.%d.%d. "), chapterNo, sectionNo, subsectionNo, subsubsectionNo);
          }
        }
        else if ( useWord )
        {
              wxFprintf(Chapters, _T("{\\bkmkstart %s}{\\bkmkend %s}"), topicName, topicName);
        }
      }
      OutputCurrentSection(); // Repeat section header
      TexOutput(_T("\\par\\pard}\n"));
      issuedNewParagraph = 1;
      WriteEnvironmentStyles();
//      TexOutput(_T("\\par\\pard}\\par\n"));
//      issuedNewParagraph = 2;
    }
    }
    break;
  }
  case ltCAPTION:
  case ltCAPTIONSTAR:
  {
    if (!start)
    {
      wxChar *topicName = FindTopicName(GetNextChunk());
      SetCurrentTopic(topicName);

      TexOutput(_T("\\pard\\par"));
      wxChar figBuf[200];

      if (inFigure)
      {
        figureNo ++;

        if (winHelp || !useWord)
        {
          if (DocumentStyle != LATEX_ARTICLE)
            wxSnprintf(figBuf, sizeof(figBuf), _T("%s %d.%d: "), FigureNameString, chapterNo, figureNo);
          else
            wxSnprintf(figBuf, sizeof(figBuf), _T("%s %d: "), FigureNameString, figureNo);
        }
        else
        {
          wxSnprintf(figBuf, sizeof(figBuf), _T("%s {\\field\\flddirty{\\*\\fldinst  SEQ Figure \\\\* ARABIC }{\\fldrslt {\\bkmkstart %s}??{\\bkmkend %s}}}: "),
               FigureNameString, topicName, topicName);
        }
      }
      else
      {
        tableNo ++;

        if (winHelp || !useWord)
        {
          if (DocumentStyle != LATEX_ARTICLE)
            wxSnprintf(figBuf, sizeof(figBuf), _T("%s %d.%d: "), TableNameString, chapterNo, tableNo);
          else
            wxSnprintf(figBuf, sizeof(figBuf), _T("%s %d: "), TableNameString, tableNo);
        }
        else
        {
          wxSnprintf(figBuf, sizeof(figBuf), _T("%s {\\field\\flddirty{\\*\\fldinst  SEQ Table \\\\* ARABIC }{\\fldrslt {\\bkmkstart %s}??{\\bkmkend %s}}}: "),
               TableNameString, topicName, topicName);
        }
      }

      int n = (inTable ? tableNo : figureNo);
      AddTexRef(topicName, NULL, NULL,
           ((DocumentStyle != LATEX_ARTICLE) ? chapterNo : n),
            ((DocumentStyle != LATEX_ARTICLE) ? n : 0));

      if (winHelp)
        TexOutput(_T("\\qc{\\b "));
      else
        TexOutput(_T("\\ql{\\b "));
      TexOutput(figBuf);

      OutputCurrentSection();

      TexOutput(_T("}\\par\\pard\n"));
      WriteEnvironmentStyles();
    }
    break;
  }
  case ltFUNC:
  case ltPFUNC:
  {
//    SetCurrentOutput(winHelp ? Subsections : Chapters);
    if (start)
    {
      TexOutput(_T("{"));
    }
    else
    {
      TexOutput(_T("}\n"));
      if (winHelp)
      {
        TexOutput(_T("K{\\footnote {K} "));
        suppressNameDecoration = true;
        TraverseChildrenFromChunk(currentMember);
        suppressNameDecoration = false;
        TexOutput(_T("}\n"));
      }
      if (!winHelp && useWord)
      {
        // Insert index entry for this function
        TexOutput(_T("{\\xe\\v {"));
        suppressNameDecoration = true;  // Necessary so don't print "(\\bf" etc.
        TraverseChildrenFromChunk(currentMember);
        suppressNameDecoration = false;
        TexOutput(_T("}}"));
      }
    }
    break;
  }
  case ltCLIPSFUNC:
  {
//    SetCurrentOutput(winHelp ? Subsections : Chapters);
    if (start)
    {
      TexOutput(_T("{"));
    }
    else
    {
      TexOutput(_T("}\n"));
      if (winHelp)
      {
        TexOutput(_T("K{\\footnote {K} "));
        suppressNameDecoration = true;  // Necessary so don't print "(\\bf" etc.
        TraverseChildrenFromChunk(currentMember);
        suppressNameDecoration = false;
        TexOutput(_T("}\n"));
      }
      if (!winHelp && useWord)
      {
        // Insert index entry for this function
        TexOutput(_T("{\\xe\\v {"));
        suppressNameDecoration = true;  // Necessary so don't print "(\\bf" etc.
        TraverseChildrenFromChunk(currentMember);
        suppressNameDecoration = false;
        TexOutput(_T("}}"));
      }
    }
    break;
  }
  case ltMEMBER:
  {
//    SetCurrentOutput(winHelp ? Subsections : Chapters);
    if (start)
    {
      TexOutput(_T("{\\b "));
    }
    else
    {
      TexOutput(_T("}\n"));
      if (winHelp)
      {
        TexOutput(_T("K{\\footnote {K} "));
        TraverseChildrenFromChunk(currentMember);
        TexOutput(_T("}\n"));
      }
      if (!winHelp && useWord)
      {
        // Insert index entry for this function
        TexOutput(_T("{\\xe\\v {"));
        suppressNameDecoration = true;  // Necessary so don't print "(\\bf" etc.
        TraverseChildrenFromChunk(currentMember);
        suppressNameDecoration = false;
        TexOutput(_T("}}"));
      }
    }
    break;
  }
  case ltDOCUMENT:
  {
    if (start)
      SetCurrentOutput(Chapters);
    break;
  }
  case ltTABLEOFCONTENTS:
  {
    if (start)
    {
      if (!winHelp && useWord)
      {
        // Insert Word for Windows table of contents
        TexOutput(_T("\\par\\pard\\pgnrestart\\sect\\titlepg"));

       // In linear RTF, same as chapter headings.
        wxSnprintf(buf, sizeof(buf), _T("{\\b\\fs%d %s}\\par\\par\\pard\n\n"), chapterFont*2, ContentsNameString);

        TexOutput(buf);
        wxSnprintf(buf, sizeof(buf), _T("{\\field{\\*\\fldinst TOC \\\\o \"1-%d\" }{\\fldrslt PRESS F9 TO REFORMAT CONTENTS}}\n"), contentsDepth);
        TexOutput(buf);
//        TexOutput(_T("\\sect\\sectd"));
      }
      else
      {
        FILE *fd = wxFopen(ContentsName, _T("r"));
        if (fd)
        {
          int ch = getc(fd);
          while (ch != EOF)
          {
            wxPutc(ch, Chapters);
            ch = getc(fd);
          }
          fclose(fd);
        }
        else
        {
          TexOutput(_T("{\\i RUN TEX2RTF AGAIN FOR CONTENTS PAGE}\\par\n"));
          OnInform(_T("Run Tex2RTF again to include contents page."));
        }
      }
    }
    break;
  }
  case ltVOID:
  {
//    if (start)
//      TexOutput(_T("{\\b void}"));
    break;
  }
  case ltHARDY:
  {
    if (start)
      TexOutput(_T("{\\scaps HARDY}"));
    break;
  }
  case ltWXCLIPS:
  {
    if (start)
      TexOutput(_T("wxCLIPS"));
    break;
  }
  case ltSPECIALAMPERSAND:
  {
    if (start)
    {
      if (inTabular)
        TexOutput(_T("\\cell "));
      else
        TexOutput(_T("&"));
    }
    break;
  }
  case ltSPECIALTILDE:
  {
    if (start)
    {
      #if 1 // if(inVerbatim)
        TexOutput(_T("~"));
      #else
        TexOutput(_T(" "));
      #endif
    }
    break;
  }
  case ltBACKSLASHCHAR:
  {
    if (start)
    {
      if (inTabular)
      {
//        TexOutput(_T("\\cell\\row\\trowd\\trgaph108\\trleft-108\n"));
        TexOutput(_T("\\cell\\row\\trowd\\trgaph108\n"));
        int currentWidth = 0;
        for (int i = 0; i < noColumns; i++)
        {
          currentWidth += TableData[i].width;
          if (TableData[i].rightBorder)
            TexOutput(_T("\\clbrdrr\\brdrs\\brdrw15"));

          if (TableData[i].leftBorder)
            TexOutput(_T("\\clbrdrl\\brdrs\\brdrw15"));

          wxSnprintf(buf, sizeof(buf), _T("\\cellx%d"), currentWidth);
          TexOutput(buf);
        }
        TexOutput(_T("\\pard\\intbl\n"));
      }
      else
        TexOutput(_T("\\line\n"));
    }
    break;
  }
  case ltRANGLEBRA:
  {
    if (start)
      TexOutput(_T("\tab "));
    break;
  }
  case ltRTFSP:  // Explicit space, RTF only
  {
    if (start)
      TexOutput(_T(" "));
   break;
  }
  case ltITEMIZE:
  case ltENUMERATE:
  case ltDESCRIPTION:
  {
    if (start)
    {
      if (indentLevel > 0)
      {
        // Experimental JACS 2004-02-21
        TexOutput(_T("\\par\n"));
        issuedNewParagraph = 1;
//        TexOutput(_T("\\par\\par\n"));
//        issuedNewParagraph = 2;
      }
      else
      {
        // Top-level list: issue a new paragraph if we haven't
        // just done so
        if (!issuedNewParagraph)
        {
          TexOutput(_T("\\par\\pard"));
          WriteEnvironmentStyles();
          issuedNewParagraph = 1;
        }
        else issuedNewParagraph = 0;
      }
      indentLevel ++;
      TexOutput(_T("\\fi0\n"));
      int listType;
      if (macroId == ltENUMERATE)
        listType = LATEX_ENUMERATE;
      else if (macroId == ltITEMIZE)
        listType = LATEX_ITEMIZE;
      else
        listType = LATEX_DESCRIPTION;

      int oldIndent = 0;
      wxNode *node = itemizeStack.GetFirst();
      if (node)
        oldIndent = ((ItemizeStruc *)node->GetData())->indentation;

      int indentSize1 = oldIndent + 20*labelIndentTab;
      int indentSize2 = oldIndent + 20*itemIndentTab;

      ItemizeStruc *struc = new ItemizeStruc(listType, indentSize2, indentSize1);
      itemizeStack.Insert(struc);

      wxSnprintf(buf, sizeof(buf), _T("\\tx%d\\tx%d\\li%d\\sa200"), indentSize1, indentSize2, indentSize2);
      PushEnvironmentStyle(buf);
    }
    else
    {
      currentItemSep = 8; // Reset to the default
      indentLevel --;
      PopEnvironmentStyle();

      if (itemizeStack.GetFirst())
      {
        ItemizeStruc *struc = (ItemizeStruc *)itemizeStack.GetFirst()->GetData();
        delete struc;
        delete itemizeStack.GetFirst();
      }
/* Change 18/7/97 - don't know why we wish to do this
      if (itemizeStack.Number() == 0)
      {
        OnMacro(ltPAR, 0, true);
        OnMacro(ltPAR, 0, false);
        issuedNewParagraph = 2;
      }
*/
    }
    break;
  }
  case ltTWOCOLLIST:
  {
    if (start)
    {
      indentLevel ++;
      int oldIndent = 0;
      wxNode *node = itemizeStack.GetFirst();
      if (node)
        oldIndent = ((ItemizeStruc *)node->GetData())->indentation;

      int indentSize = oldIndent + TwoColWidthA;

      ItemizeStruc *struc = new ItemizeStruc(LATEX_TWOCOL, indentSize);
      itemizeStack.Insert(struc);

//      wxSnprintf(buf, sizeof(buf), _T("\\tx%d\\li%d\\ri%d"), indentSize, indentSize, TwoColWidthA+TwoColWidthB+oldIndent);
      wxSnprintf(buf, sizeof(buf), _T("\\tx%d\\li%d\\sa200"), indentSize, indentSize);
      PushEnvironmentStyle(buf);
    }
    else
    {
      indentLevel --;
      PopEnvironmentStyle();
      if (itemizeStack.GetFirst())
      {
        ItemizeStruc *struc = (ItemizeStruc *)itemizeStack.GetFirst()->GetData();
        delete struc;
        delete itemizeStack.GetFirst();
      }
/*
      // JACS June 1997
      TexOutput(_T("\\pard\n"));
      WriteEnvironmentStyles();
*/
/* why do we need this? */
// Experimental
      TexOutput(_T("\\pard\n"));
#if 0
      if (itemizeStack.GetCount() == 0)
      {
        issuedNewParagraph = 0;
        OnMacro(ltPAR, 0, true);
        OnMacro(ltPAR, 0, false);
      }
#endif
    }
    break;
  }
  case ltITEM:
  {
    wxNode *node = itemizeStack.GetFirst();
    if (node)
    {
      ItemizeStruc *struc = (ItemizeStruc *)node->GetData();
      if (!start)
      {
        struc->currentItem += 1;
        wxChar indentBuf[60];

        int indentSize1 = struc->labelIndentation;
        int indentSize2 = struc->indentation;

        TexOutput(_T("\n"));
        if (struc->currentItem > 1 && issuedNewParagraph == 0)
        {
            // JACS
//          if (currentItemSep > 0)
//            TexOutput(_T("\\par"));

          TexOutput(_T("\\par"));
          issuedNewParagraph = 1;
//          WriteEnvironmentStyles();
        }

        wxSnprintf(buf, sizeof(buf), _T("\\tx%d\\tx%d\\li%d\\fi-%d\n"), indentSize1, indentSize2,
                  indentSize2, 20*itemIndentTab);
        TexOutput(buf);

        switch (struc->listType)
        {
          case LATEX_ENUMERATE:
          {
            if (descriptionItemArg)
            {
              TexOutput(_T("\\tab{ "));
              TraverseChildrenFromChunk(descriptionItemArg);
              TexOutput(_T("}\\tab"));
              descriptionItemArg = NULL;
            }
            else
            {
              wxSnprintf(indentBuf, sizeof(indentBuf), _T("\\tab{\\b %d.}\\tab"), struc->currentItem);
              TexOutput(indentBuf);
            }
            break;
          }
          case LATEX_ITEMIZE:
          {
            if (descriptionItemArg)
            {
              TexOutput(_T("\\tab{ "));
              TraverseChildrenFromChunk(descriptionItemArg);
              TexOutput(_T("}\\tab"));
              descriptionItemArg = NULL;
            }
          else
            {
              if (bulletFile && winHelp)
              {
                if (winHelpVersion > 3) // Transparent bitmap
                  wxSnprintf(indentBuf, sizeof(indentBuf), _T("\\tab\\{bmct %s\\}\\tab"), bulletFile);
                else
                  wxSnprintf(indentBuf, sizeof(indentBuf), _T("\\tab\\{bmc %s\\}\\tab"), bulletFile);
              }
              else if (winHelp)
                wxSnprintf(indentBuf, sizeof(indentBuf), _T("\\tab{\\b o}\\tab"));
              else
                wxSnprintf(indentBuf, sizeof(indentBuf), _T("\\tab{\\f1\\'b7}\\tab"));
              TexOutput(indentBuf);
            }
            break;
          }
          default:
          case LATEX_DESCRIPTION:
          {
            if (descriptionItemArg)
            {
              TexOutput(_T("\\tab{\\b "));
              TraverseChildrenFromChunk(descriptionItemArg);
              TexOutput(_T("}  "));
              descriptionItemArg = NULL;
            }
            break;
          }
        }
      }
    }
    break;
  }
  case ltTWOCOLITEM:
  case ltTWOCOLITEMRULED:
  {
    wxNode *node = itemizeStack.GetFirst();
    if (node)
    {
      ItemizeStruc *struc = (ItemizeStruc *)node->GetData();
      if (start)
      {
        struc->currentItem += 1;

        int oldIndent = 0;
        wxNode *node2 = NULL;
        if (itemizeStack.GetCount() > 1) // TODO: do I actually mean Nth(0) here??
            node2 = itemizeStack.Item(1);
        if (node2)
          oldIndent = ((ItemizeStruc *)node2->GetData())->indentation;

        TexOutput(_T("\n"));
        // JACS
#if 0
        if (struc->currentItem > 1)
        {
          if (currentItemSep > 0)
            TexOutput(_T("\\par"));

//          WriteEnvironmentStyles();
        }
#endif

//        wxSnprintf(buf, sizeof(buf), _T("\\tx%d\\li%d\\fi-%d\\ri%d\n"), TwoColWidthA,
//             TwoColWidthA, TwoColWidthA, TwoColWidthA+TwoColWidthB+oldIndent);
/*
        wxSnprintf(buf, sizeof(buf), _T("\\tx%d\\li%d\\fi-%d\n"), TwoColWidthA,
             TwoColWidthA, TwoColWidthA);
*/
        wxSnprintf(buf, sizeof(buf), _T("\\tx%d\\li%d\\fi-%d\n"), TwoColWidthA + oldIndent,
             TwoColWidthA + oldIndent, TwoColWidthA);
        TexOutput(buf);
      }
    }
    break;
  }
  case ltVERBATIM:
  case ltVERB:
  {
    if (start)
    {
        // JACS
#if 0
      if (macroId == ltVERBATIM)
      {
        if (!issuedNewParagraph)
        {
          TexOutput(_T("\\par\\pard"));
          WriteEnvironmentStyles();
          issuedNewParagraph = 1;
        }
        else issuedNewParagraph = 0;
      }
#endif

      if (macroId == ltVERBATIM)
        wxSnprintf(buf, sizeof(buf), _T("{\\f3\\s10\\fs20\\li720\\sa0 "));
      else
        wxSnprintf(buf, sizeof(buf), _T("{\\f3\\fs20 "));
      TexOutput(buf);
    }
    else
    {
      TexOutput(_T("}"));
      if (macroId == ltVERBATIM)
      {
        TexOutput(_T("\\pard\n"));
        WriteEnvironmentStyles();
        // JACS
#if 0
        TexOutput(_T("\\par\n"));
        issuedNewParagraph = 1;
#endif
      }
    }
    break;
  }
  case ltCENTERLINE:
  case ltCENTER:
  {
    if (start)
    {
      TexOutput(_T("\\qc "));
      forbidParindent ++;
      PushEnvironmentStyle(_T("\\qc\\sa200"));
    }
    else
    {
      TexOutput(_T("\\par\\pard\n"));
      issuedNewParagraph = 1;
      forbidParindent --;
      PopEnvironmentStyle();
      WriteEnvironmentStyles();
    }
    break;
  }
  case ltFLUSHLEFT:
  {
    if (start)
    {
      TexOutput(_T("\\ql\\sa200 "));
      forbidParindent ++;
      PushEnvironmentStyle(_T("\\ql"));
    }
    else
    {
      TexOutput(_T("\\par\\pard\n"));
      issuedNewParagraph = 1;
      forbidParindent --;
      PopEnvironmentStyle();
      WriteEnvironmentStyles();
    }
    break;
  }
  case ltFLUSHRIGHT:
  {
    if (start)
    {
      TexOutput(_T("\\qr\\sa200 "));
      forbidParindent ++;
      PushEnvironmentStyle(_T("\\qr"));
    }
    else
    {
      TexOutput(_T("\\par\\pard\n"));
      issuedNewParagraph = 1;
      forbidParindent --;
      PopEnvironmentStyle();
      WriteEnvironmentStyles();
    }
    break;
  }
  case ltSMALL:
  case ltFOOTNOTESIZE:
  {
    if (start)
    {
      wxSnprintf(buf, sizeof(buf), _T("{\\fs%d\n"), smallFont*2);
      TexOutput(buf);
    }
    else TexOutput(_T("}\n"));
    break;
  }
  case ltTINY:
  case ltSCRIPTSIZE:
  {
    if (start)
    {
      wxSnprintf(buf, sizeof(buf), _T("{\\fs%d\n"), tinyFont*2);
      TexOutput(buf);
    }
    else TexOutput(_T("}\n"));
    break;
  }
  case ltNORMALSIZE:
  {
    if (start)
    {
      wxSnprintf(buf, sizeof(buf), _T("{\\fs%d\n"), normalFont*2);
      TexOutput(buf);
    }
    else TexOutput(_T("}\n"));
    break;
  }
  case ltlarge:
  {
    if (start)
    {
      wxSnprintf(buf, sizeof(buf), _T("{\\fs%d\n"), largeFont1*2);
      TexOutput(buf);
    }
    else TexOutput(_T("}\n"));
    break;
  }
  case ltLarge:
  {
    if (start)
    {
      wxSnprintf(buf, sizeof(buf), _T("{\\fs%d\n"), LargeFont2*2);
      TexOutput(buf);
    }
    else TexOutput(_T("}\n"));
    break;
  }
  case ltLARGE:
  {
    if (start)
    {
      wxSnprintf(buf, sizeof(buf), _T("{\\fs%d\n"), LARGEFont3*2);
      TexOutput(buf);
    }
    else TexOutput(_T("}\n"));
    break;
  }
  case lthuge:
  {
    if (start)
    {
      wxSnprintf(buf, sizeof(buf), _T("{\\fs%d\n"), hugeFont1*2);
      TexOutput(buf);
    }
    else TexOutput(_T("}\n"));
    break;
  }
  case ltHuge:
  {
    if (start)
    {
      wxSnprintf(buf, sizeof(buf), _T("{\\fs%d\n"), HugeFont2*2);
      TexOutput(buf);
    }
    else TexOutput(_T("}\n"));
    break;
  }
  case ltHUGE:
  {
    if (start)
    {
      wxSnprintf(buf, sizeof(buf), _T("{\\fs%d\n"), HUGEFont3*2);
      TexOutput(buf);
    }
    else TexOutput(_T("}\n"));
    break;
  }
  case ltTEXTBF:
  case ltBFSERIES:
  case ltBF:
  {
    if (start)
    {
      TexOutput(_T("{\\b "));
    }
    else TexOutput(_T("}"));
    break;
  }
  case ltUNDERLINE:
  {
    if (start)
    {
      TexOutput(_T("{\\ul "));
    }
    else TexOutput(_T("}"));
    break;
  }
  case ltTEXTIT:
  case ltITSHAPE:
  case ltIT:
  case ltEMPH:
  case ltEM:
  {
    if (start)
    {
      TexOutput(_T("{\\i "));
    }
    else TexOutput(_T("}"));
    break;
  }
  // Roman font: do nothing. Should really switch between
  // fonts.
  case ltTEXTRM:
  case ltRMFAMILY:
  case ltRM:
  {
/*
    if (start)
    {
      TexOutput(_T("{\\plain "));
    }
    else TexOutput(_T("}"));
 */
    break;
  }
  // Medium-weight font. Unbolden...
  case ltMDSERIES:
  {
    if (start)
    {
      TexOutput(_T("{\\b0 "));
    }
    else TexOutput(_T("}"));
    break;
  }
  // Upright (un-italic or slant)
  case ltUPSHAPE:
  {
    if (start)
    {
      TexOutput(_T("{\\i0 "));
    }
    else TexOutput(_T("}"));
    break;
  }
  case ltTEXTSC:
  case ltSCSHAPE:
  case ltSC:
  {
    if (start)
    {
      TexOutput(_T("{\\scaps "));
    }
    else TexOutput(_T("}"));
    break;
  }
  case ltTEXTTT:
  case ltTTFAMILY:
  case ltTT:
  {
    if (start)
    {
      TexOutput(_T("{\\f3 "));
    }
    else TexOutput(_T("}"));
    break;
  }
  case ltLBRACE:
  {
    if (start)
      TexOutput(_T("\\{"));
    break;
  }
  case ltRBRACE:
  {
    if (start)
      TexOutput(_T("\\}"));
    break;
  }
  case ltBACKSLASH:
  {
    if (start)
      TexOutput(_T("\\\\"));
    break;
  }
  case ltPAR:
  {
    if (start)
    {
      if ( issuedNewParagraph == 0 )
      {
          TexOutput(_T("\\par\\pard"));
          issuedNewParagraph ++;

          // Extra par if parskip is more than zero (usually looks best.)
          // N.B. JACS 2004-02-21: shouldn't need this for linear RTF if
          // we have a suitable set of styles.
#if 0
          if (winHelp && !inTabular && (ParSkip > 0))
          {
            TexOutput(_T("\\par"));
            issuedNewParagraph ++;
          }
#endif
          WriteEnvironmentStyles();
      }
      // 1 is a whole paragraph if ParSkip == 0,
      // half a paragraph if ParSkip > 0
      else if ( issuedNewParagraph == 1 )
      {
        // Don't need a par at all if we've already had one,
        // and ParSkip == 0.
#if 0
          // Extra par if parskip is more than zero (usually looks best.)
          if (winHelp && !inTabular && (ParSkip > 0))
          {
            TexOutput(_T("\\par"));
            issuedNewParagraph ++;
          }
#endif
          WriteEnvironmentStyles();
      }
/*
      if (!issuedNewParagraph || (issuedNewParagraph > 1))
      {
        TexOutput(_T("\\par\\pard"));

        // Extra par if parskip is more than zero (usually looks best.)
        if (!inTabular && (ParSkip > 0))
          TexOutput(_T("\\par"));
        WriteEnvironmentStyles();
      }
*/

      TexOutput(_T("\n"));
    }
    break;
  }
  case ltNEWPAGE:
  {
    // In Windows Help, no newpages until we've started some chapters or sections
    if (!(winHelp && !startedSections))
      if (start)
        TexOutput(_T("\\page\n"));
    break;
  }
  case ltMAKETITLE:
  {
    if (start && DocumentTitle)
    {
      TexOutput(_T("\\par\\pard"));
      if (!winHelp)
        TexOutput(_T("\\par"));
      wxSnprintf(buf, sizeof(buf), _T("\\qc{\\fs%d\\b "), titleFont*2);
      TexOutput(buf);
      TraverseChildrenFromChunk(DocumentTitle);
      TexOutput(_T("}\\par\\pard\n"));

      if (DocumentAuthor)
      {
        if (!winHelp)
          TexOutput(_T("\\par"));
        wxSnprintf(buf, sizeof(buf), _T("\\par\\qc{\\fs%d "), authorFont*2);
        TexOutput(buf);
        TraverseChildrenFromChunk(DocumentAuthor);
        TexOutput(_T("}"));
        TexOutput(_T("\\par\\pard\n"));
      }
      if (DocumentDate)
      {
        TexOutput(_T("\\par"));
        wxSnprintf(buf, sizeof(buf), _T("\\qc{\\fs%d "), authorFont*2);
        TexOutput(buf);
        TraverseChildrenFromChunk(DocumentDate);
        TexOutput(_T("}\\par\\pard\n"));
      }
      // If linear RTF, we want this titlepage to be in a separate
      // section with its own (blank) header and footer
      if (!winHelp && (DocumentStyle != LATEX_ARTICLE))
      {
        TexOutput(_T("{\\header }{\\footer }\n"));
        // Not sure about this: we get too many sections.
//        TexOutput(_T("\\sect"));
      }
    }
    break;
  }
  case ltADDCONTENTSLINE:
  {
    if (!start)
    {
    if (contentsLineSection && contentsLineValue)
    {
      if (wxStrcmp(contentsLineSection, _T("chapter")) == 0)
      {
        wxFprintf(Contents, _T("\\par\n{\\b %s}\\par\n"), contentsLineValue);
      }
      else if (wxStrcmp(contentsLineSection, _T("section")) == 0)
      {
        if (DocumentStyle != LATEX_ARTICLE)
          wxFprintf(Contents, _T("\n\\tab%s\\par\n"), contentsLineValue);
        else
          wxFprintf(Contents, _T("\\par\n{\\b %s}\\par\n"), contentsLineValue);
      }
    }
    }
    break;
  }
  case ltHRULE:
  {
    if (start)
    {
      TexOutput(_T("\\brdrb\\brdrs\\par\\pard\n"));
      issuedNewParagraph = 1;
      WriteEnvironmentStyles();
    }
    break;
  }
  case ltRULE:
  {
    if (start)
    {
      TexOutput(_T("\\brdrb\\brdrs\\par\\pard\n"));
      issuedNewParagraph = 1;
      WriteEnvironmentStyles();
    }
    break;
  }
  case ltHLINE:
  {
    if (start)
      ruleTop ++;
    break;
  }
  case ltNUMBEREDBIBITEM:
  {
    if (start)
      TexOutput(_T("\\li260\\fi-260 ")); // Indent from 2nd line
    else
      TexOutput(_T("\\par\\pard\\par\n\n"));
    break;
  }
  case ltTHEPAGE:
  {
    if (start)
    {
      TexOutput(_T("{\\field{\\*\\fldinst PAGE \\\\* MERGEFORMAT }{\\fldrslt 1}}"));
    }
    break;
  }
  case ltTHECHAPTER:
  {
    if (start)
    {
//      TexOutput(_T("{\\field{\\*\\fldinst SECTION \\\\* MERGEFORMAT }{\\fldrslt 1}}"));
      wxSnprintf(buf, sizeof(buf), _T("%d"), chapterNo);
      TexOutput(buf);
    }
    break;
  }
  case ltTHESECTION:
  {
    if (start)
    {
//      TexOutput(_T("{\\field{\\*\\fldinst SECTION \\\\* MERGEFORMAT }{\\fldrslt 1}}"));
      wxSnprintf(buf, sizeof(buf), _T("%d"), sectionNo);
      TexOutput(buf);
    }
    break;
  }
  case ltTWOCOLUMN:
  {
    if (!start && !winHelp)
    {
      TexOutput(_T("\\cols2\n"));
    }
    break;
  }
  case ltONECOLUMN:
  {
    if (!start && !winHelp)
    {
      TexOutput(_T("\\cols1\n"));
    }
    break;
  }
  case ltPRINTINDEX:
  {
    if (start && useWord && !winHelp)
    {
      FakeCurrentSection(_T("Index"));
      OnMacro(ltPAR, 0, true);
      OnMacro(ltPAR, 0, false);
      TexOutput(_T("\\par{\\field{\\*\\fldinst INDEX \\\\h \"\\emdash A\\emdash \"\\\\c \"2\"}{\\fldrslt PRESS F9 TO REFORMAT INDEX}}\n"));
    }
    break;
  }
  case ltLISTOFFIGURES:
  {
    if (start && useWord && !winHelp)
    {
      FakeCurrentSection(FiguresNameString, false);
      OnMacro(ltPAR, 0, true);
      OnMacro(ltPAR, 0, false);
      OnMacro(ltPAR, 0, true);
      OnMacro(ltPAR, 0, false);
      wxChar buf[200];
      wxSnprintf(buf, sizeof(buf), _T("{\\field\\fldedit{\\*\\fldinst  TOC \\\\c \"%s\" }{\\fldrslt PRESS F9 TO REFORMAT LIST OF FIGURES}}\n"),
               FigureNameString);
      TexOutput(buf);
    }
    break;
  }
  case ltLISTOFTABLES:
  {
    if (start && useWord && !winHelp)
    {
      FakeCurrentSection(TablesNameString, false);
      OnMacro(ltPAR, 0, true);
      OnMacro(ltPAR, 0, false);
      OnMacro(ltPAR, 0, true);
      OnMacro(ltPAR, 0, false);
      wxChar buf[200];
      wxSnprintf(buf, sizeof(buf), _T("{\\field\\fldedit{\\*\\fldinst  TOC \\\\c \"%s\" }{\\fldrslt PRESS F9 TO REFORMAT LIST OF TABLES}}\n"),
                TablesNameString);
      TexOutput(buf);
    }
    break;
  }
  // Symbols
  case ltALPHA:
    if (start) TexOutput(_T("{\\f1\\'61}"));
    break;
  case ltBETA:
    if (start) TexOutput(_T("{\\f1\\'62}"));
    break;
  case ltGAMMA:
    if (start) TexOutput(_T("{\\f1\\'63}"));
    break;
  case ltDELTA:
    if (start) TexOutput(_T("{\\f1\\'64}"));
    break;
  case ltEPSILON:
  case ltVAREPSILON:
    if (start) TexOutput(_T("{\\f1\\'65}"));
    break;
  case ltZETA:
    if (start) TexOutput(_T("{\\f1\\'7A}"));
    break;
  case ltETA:
    if (start) TexOutput(_T("{\\f1\\'68}"));
    break;
  case ltTHETA:
  case ltVARTHETA:
    if (start) TexOutput(_T("{\\f1\\'71}"));
    break;
  case ltIOTA:
    if (start) TexOutput(_T("{\\f1\\'69}"));
    break;
  case ltKAPPA:
    if (start) TexOutput(_T("{\\f1\\'6B}"));
    break;
  case ltLAMBDA:
    if (start) TexOutput(_T("{\\f1\\'6C}"));
    break;
  case ltMU:
    if (start) TexOutput(_T("{\\f1\\'6D}"));
    break;
  case ltNU:
    if (start) TexOutput(_T("{\\f1\\'6E}"));
    break;
  case ltXI:
    if (start) TexOutput(_T("{\\f1\\'78}"));
    break;
  case ltPI:
    if (start) TexOutput(_T("{\\f1\\'70}"));
    break;
  case ltVARPI:
    if (start) TexOutput(_T("{\\f1\\'76}"));
    break;
  case ltRHO:
  case ltVARRHO:
    if (start) TexOutput(_T("{\\f1\\'72}"));
    break;
  case ltSIGMA:
    if (start) TexOutput(_T("{\\f1\\'73}"));
    break;
  case ltVARSIGMA:
    if (start) TexOutput(_T("{\\f1\\'56}"));
    break;
  case ltTAU:
    if (start) TexOutput(_T("{\\f1\\'74}"));
    break;
  case ltUPSILON:
    if (start) TexOutput(_T("{\\f1\\'75}"));
    break;
  case ltPHI:
  case ltVARPHI:
    if (start) TexOutput(_T("{\\f1\\'66}"));
    break;
  case ltCHI:
    if (start) TexOutput(_T("{\\f1\\'63}"));
    break;
  case ltPSI:
    if (start) TexOutput(_T("{\\f1\\'79}"));
    break;
  case ltOMEGA:
    if (start) TexOutput(_T("{\\f1\\'77}"));
    break;
  case ltCAP_GAMMA:
    if (start) TexOutput(_T("{\\f1\\'47}"));
    break;
  case ltCAP_DELTA:
    if (start) TexOutput(_T("{\\f1\\'44}"));
    break;
  case ltCAP_THETA:
    if (start) TexOutput(_T("{\\f1\\'51}"));
    break;
  case ltCAP_LAMBDA:
    if (start) TexOutput(_T("{\\f1\\'4C}"));
    break;
  case ltCAP_XI:
    if (start) TexOutput(_T("{\\f1\\'58}"));
    break;
  case ltCAP_PI:
    if (start) TexOutput(_T("{\\f1\\'50}"));
    break;
  case ltCAP_SIGMA:
    if (start) TexOutput(_T("{\\f1\\'53}"));
    break;
  case ltCAP_UPSILON:
    if (start) TexOutput(_T("{\\f1\\'54}"));
    break;
  case ltCAP_PHI:
    if (start) TexOutput(_T("{\\f1\\'46}"));
    break;
  case ltCAP_PSI:
    if (start) TexOutput(_T("{\\f1\\'59}"));
    break;
  case ltCAP_OMEGA:
    if (start) TexOutput(_T("{\\f1\\'57}"));
    break;
  // Binary operation symbols
  case ltLE:
  case ltLEQ:
    if (start) TexOutput(_T("{\\f1\\'A3}"));
    break;
  case ltLL:
    if (start) TexOutput(_T("<<"));
    break;
  case ltSUBSET:
    if (start) TexOutput(_T("{\\f1\\'CC}"));
    break;
  case ltSUBSETEQ:
    if (start) TexOutput(_T("{\\f1\\'CD}"));
    break;
  case ltIN:
    if (start) TexOutput(_T("{\\f1\\'CE}"));
    break;
  case ltGE:
  case ltGEQ:
    if (start) TexOutput(_T("{\\f1\\'B3}"));
    break;
  case ltGG:
    if (start) TexOutput(_T(">>"));
    break;
  case ltSUPSET:
    if (start) TexOutput(_T("{\\f1\\'C9}"));
    break;
  case ltSUPSETEQ:
    if (start) TexOutput(_T("{\\f1\\'CD}"));
    break;
  case ltNI:
    if (start) TexOutput(_T("{\\f1\\'27}"));
    break;
  case ltPERP:
    if (start) TexOutput(_T("{\\f1\\'5E}"));
    break;
  case ltNEQ:
    if (start) TexOutput(_T("{\\f1\\'B9}"));
    break;
  case ltAPPROX:
    if (start) TexOutput(_T("{\\f1\\'BB}"));
    break;
  case ltCONG:
    if (start) TexOutput(_T("{\\f1\\'40}"));
    break;
  case ltEQUIV:
    if (start) TexOutput(_T("{\\f1\\'BA}"));
    break;
  case ltPROPTO:
    if (start) TexOutput(_T("{\\f1\\'B5}"));
    break;
  case ltSIM:
    if (start) TexOutput(_T("{\\f1\\'7E}"));
    break;
  case ltSMILE:
    if (start) TexOutput(_T("{\\f4\\'4A}"));
    break;
  case ltFROWN:
    if (start) TexOutput(_T("{\\f4\\'4C}"));
    break;
  case ltMID:
    if (start) TexOutput(_T("|"));
    break;

  // Negated relation symbols
  case ltNOTEQ:
    if (start) TexOutput(_T("{\\f1\\'B9}"));
    break;
  case ltNOTIN:
    if (start) TexOutput(_T("{\\f1\\'CF}"));
    break;
  case ltNOTSUBSET:
    if (start) TexOutput(_T("{\\f1\\'CB}"));
    break;

  // Arrows
  case ltLEFTARROW:
     if (start) TexOutput(_T("{\\f1\\'AC}"));
    break;
  case ltLEFTARROW2:
    if (start) TexOutput(_T("{\\f1\\'DC}"));
    break;
  case ltRIGHTARROW:
    if (start) TexOutput(_T("{\\f1\\'AE}"));
    break;
  case ltRIGHTARROW2:
    if (start) TexOutput(_T("{\\f1\\'DE}"));
    break;
  case ltLEFTRIGHTARROW:
    if (start) TexOutput(_T("{\\f1\\'AB}"));
    break;
  case ltLEFTRIGHTARROW2:
    if (start) TexOutput(_T("{\\f1\\'DB}"));
    break;
  case ltUPARROW:
    if (start) TexOutput(_T("{\\f1\\'AD}"));
    break;
  case ltUPARROW2:
    if (start) TexOutput(_T("{\\f1\\'DD}"));
    break;
  case ltDOWNARROW:
    if (start) TexOutput(_T("{\\f1\\'AF}"));
    break;
  case ltDOWNARROW2:
    if (start) TexOutput(_T("{\\f1\\'DF}"));
    break;

  // Miscellaneous symbols
  case ltALEPH:
    if (start) TexOutput(_T("{\\f1\\'CO}"));
    break;
  case ltWP:
    if (start) TexOutput(_T("{\\f1\\'C3}"));
    break;
  case ltRE:
    if (start) TexOutput(_T("{\\f1\\'C2}"));
    break;
  case ltIM:
    if (start) TexOutput(_T("{\\f1\\'C1}"));
    break;
  case ltEMPTYSET:
    if (start) TexOutput(_T("{\\f1\\'C6}"));
    break;
  case ltNABLA:
    if (start) TexOutput(_T("{\\f1\\'D1}"));
    break;
  case ltSURD:
    if (start) TexOutput(_T("{\\f1\\'D6}"));
    break;
  case ltPARTIAL:
    if (start) TexOutput(_T("{\\f1\\'B6}"));
    break;
  case ltBOT:
    if (start) TexOutput(_T("{\\f1\\'5E}"));
    break;
  case ltFORALL:
    if (start) TexOutput(_T("{\\f1\\'22}"));
    break;
  case ltEXISTS:
    if (start) TexOutput(_T("{\\f1\\'24}"));
    break;
  case ltNEG:
    if (start) TexOutput(_T("{\\f1\\'D8}"));
    break;
  case ltSHARP:
    if (start) TexOutput(_T("{\\f1\\'23}"));
    break;
  case ltANGLE:
    if (start) TexOutput(_T("{\\f1\\'D0}"));
    break;
  case ltTRIANGLE:
    if (start) TexOutput(_T("{\\f5\\'73}"));
    break;
  case ltCLUBSUIT:
    if (start) TexOutput(_T("{\\f5\\'A8}"));
    break;
  case ltDIAMONDSUIT:
    if (start) TexOutput(_T("{\\f5\\'A9}"));
    break;
  case ltHEARTSUIT:
    if (start) TexOutput(_T("{\\f5\\'AA}"));
    break;
  case ltSPADESUIT:
    if (start) TexOutput(_T("{\\f5\\'AB}"));
    break;
  case ltINFTY:
    if (start) TexOutput(_T("{\\f1\\'A5}"));
    break;
  case ltCOPYRIGHT:
    if (start) TexOutput(_T("{\\f0\\'A9}"));
    break;
  case ltREGISTERED:
    if (start) TexOutput(_T("{\\f0\\'AE}"));
    break;
  case ltPM:
    if (start) TexOutput(_T("{\\f1\\'B1}"));
    break;
  case ltMP:
    if (start) TexOutput(_T("{\\f1\\'B1}"));
    break;
  case ltTIMES:
    if (start) TexOutput(_T("{\\f1\\'B4}"));
    break;
  case ltDIV:
    if (start) TexOutput(_T("{\\f1\\'B8}"));
    break;
  case ltCDOT:
    if (start) TexOutput(_T("{\\f1\\'D7}"));
    break;
  case ltAST:
    if (start) TexOutput(_T("{\\f1\\'2A}"));
    break;
  case ltSTAR:
    if (start) TexOutput(_T("{\\f5\\'AB}"));
    break;
  case ltCAP:
    if (start) TexOutput(_T("{\\f1\\'C7}"));
    break;
  case ltCUP:
    if (start) TexOutput(_T("{\\f1\\'C8}"));
    break;
  case ltVEE:
    if (start) TexOutput(_T("{\\f1\\'DA}"));
    break;
  case ltWEDGE:
    if (start) TexOutput(_T("{\\f1\\'D9}"));
    break;
  case ltCIRC:
    if (start) TexOutput(_T("{\\f1\\'B0}"));
    break;
  case ltBULLET:
    if (start) TexOutput(_T("{\\f1\\'B7}"));
    break;
  case ltDIAMOND:
    if (start) TexOutput(_T("{\\f1\\'E0}"));
    break;
  case ltBOX:
    if (start) TexOutput(_T("{\\f1\\'C6}"));
    break;
  case ltDIAMOND2:
    if (start) TexOutput(_T("{\\f1\\'E0}"));
    break;
  case ltBIGTRIANGLEDOWN:
    if (start) TexOutput(_T("{\\f1\\'D1}"));
    break;
  case ltOPLUS:
    if (start) TexOutput(_T("{\\f1\\'C5}"));
    break;
  case ltOTIMES:
    if (start) TexOutput(_T("{\\f1\\'C4}"));
    break;
  case ltSS:
    if (start) TexOutput(_T("{\\'DF}"));
    break;
  case ltFIGURE:
  {
    if (start) inFigure = true;
    else inFigure = false;
    break;
  }
  case ltTABLE:
  {
    if (start) inTable = true;
    else inTable = false;
    break;
  }
  default:
  {
    DefaultOnMacro(macroId, no_args, start);
    break;
  }
  }
}

// Called on start/end of argument examination
bool RTFOnArgument(int macroId, int arg_no, bool start)
{
  wxChar buf[300];
  switch (macroId)
  {
  case ltCHAPTER:
  case ltCHAPTERSTAR:
  case ltCHAPTERHEADING:
  case ltSECTION:
  case ltSECTIONSTAR:
  case ltSECTIONHEADING:
  case ltSUBSECTION:
  case ltSUBSECTIONSTAR:
  case ltSUBSUBSECTION:
  case ltSUBSUBSECTIONSTAR:
  case ltGLOSS:
  case ltMEMBERSECTION:
  case ltFUNCTIONSECTION:
  case ltCAPTION:
  case ltCAPTIONSTAR:
  {
    if (!start && (arg_no == 1))
      currentSection = GetArgChunk();
    return false;
  }
  case ltFUNC:
  {
    if (start && (arg_no == 1))
      TexOutput(_T("\\pard\\li600\\fi-600{\\b "));

    if (!start && (arg_no == 1))
      TexOutput(_T("} "));

    if (start && (arg_no == 2))
    {
      if (!suppressNameDecoration) TexOutput(_T("{\\b "));
      currentMember = GetArgChunk();
    }
    if (!start && (arg_no == 2))
    {
      if (!suppressNameDecoration) TexOutput(_T("}"));
    }

    if (start && (arg_no == 3))
      TexOutput(_T("("));
    if (!start && (arg_no == 3))
    {
//      TexOutput(_T(")\\li0\\fi0"));
//      TexOutput(_T(")\\par\\pard\\li0\\fi0"));
//      issuedNewParagraph = 1;
      TexOutput(_T(")"));
      WriteEnvironmentStyles();
    }
    break;
  }
  case ltCLIPSFUNC:
  {
    if (start && (arg_no == 1))
      TexOutput(_T("\\pard\\li260\\fi-260{\\b "));
    if (!start && (arg_no == 1))
      TexOutput(_T("} "));

    if (start && (arg_no == 2))
    {
      if (!suppressNameDecoration) TexOutput(_T("({\\b "));
      currentMember = GetArgChunk();
    }
    if (!start && (arg_no == 2))
    {
      if (!suppressNameDecoration) TexOutput(_T("}"));
    }

    if (!start && (arg_no == 3))
    {
      TexOutput(_T(")\\li0\\fi0"));
      WriteEnvironmentStyles();
    }
    break;
  }
  case ltPFUNC:
  {
    if (start && (arg_no == 1))
      TexOutput(_T("\\pard\\li260\\fi-260"));

    if (!start && (arg_no == 1))
      TexOutput(_T(" "));

    if (start && (arg_no == 2))
      TexOutput(_T("(*"));
    if (!start && (arg_no == 2))
      TexOutput(_T(")"));

    if (start && (arg_no == 2))
      currentMember = GetArgChunk();

    if (start && (arg_no == 3))
      TexOutput(_T("("));
    if (!start && (arg_no == 3))
    {
      TexOutput(_T(")\\li0\\fi0"));
      WriteEnvironmentStyles();
    }
    break;
  }
  case ltPARAM:
  {
    if (start && (arg_no == 1))
      TexOutput(_T("{\\b "));
    if (!start && (arg_no == 1))
      TexOutput(_T("}"));
    if (start && (arg_no == 2))
    {
      TexOutput(_T("{\\i "));
    }
    if (!start && (arg_no == 2))
    {
      TexOutput(_T("}"));
    }
    break;
  }
  case ltCPARAM:
  {
    if (start && (arg_no == 1))
      TexOutput(_T("{\\b "));
    if (!start && (arg_no == 1))
      TexOutput(_T("} "));  // This is the difference from param - one space!
    if (start && (arg_no == 2))
    {
      TexOutput(_T("{\\i "));
    }
    if (!start && (arg_no == 2))
    {
      TexOutput(_T("}"));
    }
    break;
  }
  case ltMEMBER:
  {
    if (!start && (arg_no == 1))
      TexOutput(_T(" "));

    if (start && (arg_no == 2))
      currentMember = GetArgChunk();
    break;
  }
  case ltREF:
  {
    if (start)
    {
      wxChar *sec = NULL;

      wxChar *refName = GetArgData();
      if (winHelp || !useWord)
      {
        if (refName)
        {
          TexRef *texRef = FindReference(refName);
          if (texRef)
          {
            sec = texRef->sectionNumber;
          }
        }
        if (sec)
        {
          TexOutput(sec);
        }
      }
      else
      {
        wxFprintf(Chapters, _T("{\\field{\\*\\fldinst  REF %s \\\\* MERGEFORMAT }{\\fldrslt ??}}"),
                refName);
      }
      return false;
    }
    break;
  }
  case ltHELPREF:
  case ltHELPREFN:
  {
    if (winHelp)
    {
        if ((GetNoArgs() - arg_no) == 1)
        {
          if (start)
            TexOutput(_T("{\\uldb "));
          else
            TexOutput(_T("}"));
        }
        if ((GetNoArgs() - arg_no) == 0) // Arg = 2, or 3 if first is optional
        {
          if (start)
          {
            TexOutput(_T("{\\v "));

            // Remove green colour/underlining if specified
            if (!hotSpotUnderline && !hotSpotColour)
              TexOutput(_T("%"));
            else if (!hotSpotColour)
              TexOutput(_T("*"));
          }
          else TexOutput(_T("}"));
        }
    }
    else // If a linear document, must resolve the references ourselves
    {
      if ((GetNoArgs() - arg_no) == 1)
      {
        // In a linear document we display the anchor text in italic plus
        // the page number.
        if (start)
          TexOutput(_T("{\\i "));
        else
          TexOutput(_T("}"));

        if (start)
          helpRefText = GetArgChunk();

        return true;
      }
      else if ((GetNoArgs() - arg_no) == 0) // Arg = 2, or 3 if first is optional
      {
        if (macroId != ltHELPREFN)
        {
          wxChar *refName = GetArgData();
          TexRef *texRef = NULL;
          if (refName)
            texRef = FindReference(refName);
          if (start)
          {
            if (texRef || !ignoreBadRefs)
              TexOutput(_T(" ("));
            if (refName)
            {
                if (texRef || !ignoreBadRefs)
                {
                  if (useWord)
                  {
                      TexOutput(_T("p. "));
                      TexOutput(_T("{\\field{\\*\\fldinst  PAGEREF "));
                      TexOutput(refName);
                      TexOutput(_T(" \\\\* MERGEFORMAT }{\\fldrslt ??}}"));
                  }
                  else
                  {
                    // Only print section name if we're not in Word mode,
                    // so can't do page references
                    if (texRef)
                    {
                      TexOutput(texRef->sectionName);
                      TexOutput(_T(" "));
                      TexOutput(texRef->sectionNumber);
                    }
                    else
                    {
                      if (!ignoreBadRefs)
                        TexOutput(_T("??"));
                      wxSnprintf(buf, sizeof(buf), _T("Warning: unresolved reference '%s'"), refName);
                      OnInform(buf);
                    }
                  }
                }
            }
            else TexOutput(_T("??"));
          }
          else
          {
            if (texRef || !ignoreBadRefs)
              TexOutput(_T(")"));
          }
        }
        return false;
      }
    }
    break;
  }
  case ltURLREF:
  {
    if (arg_no == 1)
    {
      return true;
    }
    else if (arg_no == 2)
    {
      if (start)
      {
        inVerbatim = true;
        TexOutput(_T(" ({\\f3 "));
      }
      else
      {
        TexOutput(_T("})"));
        inVerbatim = false;
      }
      return true;
    }
    break;
  }
  case ltPOPREF:
  {
    if (winHelp)
    {
      if ((GetNoArgs() - arg_no) == 1)
      {
        if (start)
          TexOutput(_T("{\\ul "));
        else
          TexOutput(_T("}"));
      }
      if ((GetNoArgs() - arg_no) == 0) // Arg = 2, or 3 if first is optional
      {
        if (start)
        {
          TexOutput(_T("{\\v "));

          // Remove green colour/underlining if specified
          if (!hotSpotUnderline && !hotSpotColour)
            TexOutput(_T("%"));
          else if (!hotSpotColour)
            TexOutput(_T("*"));
        }
        else TexOutput(_T("}"));
      }
    }
    else // A linear document...
    {
      if ((GetNoArgs() - arg_no) == 1)
      {
        // In a linear document we just display the anchor text in italic
        if (start)
          TexOutput(_T("{\\i "));
        else
          TexOutput(_T("}"));
        return true;
      }
      else return false;
    }
    break;
  }
  case ltADDCONTENTSLINE:
  {
    if (start && !winHelp)
    {
      if (arg_no == 2)
        contentsLineSection = copystring(GetArgData());
      else if (arg_no == 3)
        contentsLineValue = copystring(GetArgData());
      return false;
    }
    else return false;
  }
  case ltIMAGE:
  case ltIMAGEL:
  case ltIMAGER:
  case ltIMAGEMAP:
  case ltPSBOXTO:
  {
    if (arg_no == 3)
      return false;

    static int imageWidth = 0;
    static int imageHeight = 0;

    if (start && (arg_no == 1))
    {
      wxChar *imageDimensions = copystring(GetArgData());

      // imageWidth - Convert points to TWIPS (1 twip = 1/20th of point)
      wxStringTokenizer tok(imageDimensions, _T(";:"), wxTOKEN_STRTOK);
      if(tok.HasMoreTokens())
      {
        wxString token = tok.GetNextToken();
        imageWidth = (int)(20*ParseUnitArgument(token));
      }
      else
      {
        imageWidth = 0;
      }

      // imageHeight - Convert points to TWIPS (1 twip = 1/20th of point)
      if(tok.HasMoreTokens())
      {
        wxString token = tok.GetNextToken();
        imageHeight = (int)(20*ParseUnitArgument(token));
      }
      else
      {
        imageHeight = 0;
      }

      if (imageDimensions)  // glt
          delete [] imageDimensions;
      return false;
    }
    else if (start && (arg_no == 2 ))
    {
      wxChar *filename = copystring(GetArgData());
      wxString f = _T("");
      if ((winHelp || (wxStrcmp(bitmapMethod, _T("includepicture")) == 0)  || (wxStrcmp(bitmapMethod, _T("import")) == 0)) && useWord)
      {
        if (f == _T("")) // Try for a .shg (segmented hypergraphics file)
        {
          wxStrcpy(buf, filename);
          StripExtension(buf);
          wxStrcat(buf, _T(".shg"));
          f = TexPathList.FindValidPath(buf);
        }
        if (f == _T("")) // Try for a .bmp
        {
          wxStrcpy(buf, filename);
          StripExtension(buf);
          wxStrcat(buf, _T(".bmp"));
          f = TexPathList.FindValidPath(buf);
        }
        if (f == _T("")) // Try for a metafile instead
        {
          wxStrcpy(buf, filename);
          StripExtension(buf);
          wxStrcat(buf, _T(".wmf"));
          f = TexPathList.FindValidPath(buf);
        }
        if (f != _T(""))
        {
          if (winHelp)
          {
            if (bitmapTransparency && (winHelpVersion > 3))
              TexOutput(_T("\\{bmct "));
            else
              TexOutput(_T("\\{bmc "));
            wxString str = wxFileNameFromPath(f);
            TexOutput((wxChar*) (const wxChar*) str);
            TexOutput(_T("\\}"));
          }
          else
          {
            // Microsoft Word method
            if (wxStrcmp(bitmapMethod, _T("import")) == 0)
              TexOutput(_T("{\\field{\\*\\fldinst IMPORT "));
            else
              TexOutput(_T("{\\field{\\*\\fldinst INCLUDEPICTURE "));

            // Full path appears not to be valid!
            wxString str = wxFileNameFromPath(f);
            TexOutput((wxChar*)(const wxChar*) str);
/*
            int len = wxStrlen(f);
            wxChar smallBuf[2]; smallBuf[1] = 0;
            for (int i = 0; i < len; i++)
            {
              smallBuf[0] = f[i];
              TexOutput(smallBuf);
              if (smallBuf[0] == '\\')
                TexOutput(smallBuf);
            }
*/
            TexOutput(_T("}{\\fldrslt PRESS F9 TO FORMAT PICTURE}}"));
          }
        }
        else
        {
          TexOutput(_T("[No BMP or WMF for image file "));
          TexOutput(filename);
          TexOutput(_T("]"));
          wxSnprintf(buf, sizeof(buf), _T("Warning: could not find a BMP or WMF equivalent for %s."), filename);
          OnInform(buf);
        }
        if (filename)  // glt
            delete [] filename;
      }
      else // linear RTF
      {
          if (f == _T("")) // Try for a .bmp
          {
            wxStrcpy(buf, filename);
            StripExtension(buf);
            wxStrcat(buf, _T(".bmp"));
            f = TexPathList.FindValidPath(buf);
          }
          if (f != _T(""))
          {
            FILE *fd = wxFopen(f, _T("rb"));
            if (OutputBitmapHeader(fd, winHelp))
              OutputBitmapData(fd);
            else
            {
              wxSnprintf(buf, sizeof(buf), _T("Could not read bitmap %s.\nMay be in wrong format (needs RGB-encoded Windows BMP)."), f.c_str());
              OnError(buf);
            }
            fclose(fd);
          }
          else // Try for a metafile instead
          {
#ifdef __WXMSW__
            wxStrcpy(buf, filename);
            StripExtension(buf);
            wxStrcat(buf, _T(".wmf"));
            f = TexPathList.FindValidPath(buf);
            if (f != _T(""))
            {
  //            HFILE handle = _lopen(f, READ);
              FILE *fd = wxFopen(f, _T("rb"));
              if (OutputMetafileHeader(fd, winHelp, imageWidth, imageHeight))
              {
                OutputMetafileData(fd);
              }
              else
              {
                wxSnprintf(buf, sizeof(buf), _T("Could not read metafile %s. Perhaps it's not a placeable metafile?"), f.c_str());
                OnError(buf);
              }
              fclose(fd);
            }
            else
            {
#endif
              TexOutput(_T("[No BMP or WMF for image file "));
              TexOutput(filename);
              TexOutput(_T("]"));
              wxSnprintf(buf, sizeof(buf), _T("Warning: could not find a BMP or WMF equivalent for %s."), filename);
              OnInform(buf);
#ifdef __WXMSW__
            }
#endif
        }
      }
      return false;
    }
    else
      return false;
  }
  case ltTABULAR:
  case ltSUPERTABULAR:
  {
    if (arg_no == 1)
    {
      if (start)
      {
        currentRowNumber = 0;
        inTabular = true;
        startRows = true;
        tableVerticalLineLeft = false;
        tableVerticalLineRight = false;
        int currentWidth = 0;

        wxChar *alignString = copystring(GetArgData());
        ParseTableArgument(alignString);

//        TexOutput(_T("\\trowd\\trgaph108\\trleft-108"));
        TexOutput(_T("\\trowd\\trgaph108"));

        // Write the first row formatting for compatibility
        // with standard Latex
        if (compatibilityMode)
        {
          for (int i = 0; i < noColumns; i++)
          {
            currentWidth += TableData[i].width;
            wxSnprintf(buf, sizeof(buf), _T("\\cellx%d"), currentWidth);
            TexOutput(buf);
          }
          TexOutput(_T("\\pard\\intbl\n"));
        }
        delete[] alignString;

        return false;
      }
    }
    else if (arg_no == 2 && !start)
    {
      TexOutput(_T("\\pard\n"));
      WriteEnvironmentStyles();
      inTabular = false;
    }
    break;
  }

  case ltQUOTE:
  case ltVERSE:
  {
    if (start)
    {
      TexOutput(_T("\\li360\n"));
      forbidParindent ++;
      PushEnvironmentStyle(_T("\\li360\\sa200"));
    }
    else
    {
      forbidParindent --;
      PopEnvironmentStyle();
      OnMacro(ltPAR, 0, true);
      OnMacro(ltPAR, 0, false);
    }
    break;
  }
  case ltQUOTATION:
  {
    if (start)
    {
      TexOutput(_T("\\li360\n"));
      PushEnvironmentStyle(_T("\\li360\\sa200"));
    }
    else
    {
      PopEnvironmentStyle();
      OnMacro(ltPAR, 0, true);
      OnMacro(ltPAR, 0, false);
    }
    break;
  }
  case ltBOXIT:
  case ltFRAMEBOX:
  case ltFBOX:
  case ltNORMALBOX:
  case ltNORMALBOXD:
  {
    if (start)
    {
      wxSnprintf(buf, sizeof(buf), _T("\\sa200\\box\\trgaph108%s\n"), ((macroId == ltNORMALBOXD) ? _T("\\brdrdb") : _T("\\brdrs")));
      TexOutput(buf);
      PushEnvironmentStyle(buf);
    }
    else
    {
      PopEnvironmentStyle();
      OnMacro(ltPAR, 0, true);
      OnMacro(ltPAR, 0, false);
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
      wxSnprintf(buf, sizeof(buf), _T("\\fs%d\n"), normalFont*2);
      TexOutput(buf);
      TexOutput(buf);
      return false;
    }
    break;
  }
  case ltHELPFONTFAMILY:
  {
    if (start)
    {
      wxChar *data = GetArgData();
      if (wxStrcmp(data, _T("Swiss")) == 0)
        TexOutput(_T("\\f2\n"));
      else if (wxStrcmp(data, _T("Symbol")) == 0)
        TexOutput(_T("\\f1\n"));
      else if (wxStrcmp(data, _T("Times")) == 0)
        TexOutput(_T("\\f0\n"));

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
      if (ParIndent == 0 || forbidParindent == 0)
      {
        wxSnprintf(buf, sizeof(buf), _T("\\fi%d\n"), ParIndent*20);
        TexOutput(buf);
      }
      return false;
    }
    break;
  }
  case ltITEM:
  {
    if (start && IsArgOptional())
    {
      descriptionItemArg = GetArgChunk();
      return false;
    }
    break;
  }
  case ltTWOCOLITEM:
  case ltTWOCOLITEMRULED:
  {
    switch (arg_no)
    {
      case 1:
      {
        if (!start)
          TexOutput(_T("\\tab "));
        break;
      }
      case 2:
      {
        if (!start)
        {
          if (macroId == ltTWOCOLITEMRULED)
            TexOutput(_T("\\brdrb\\brdrs\\brdrw15\\brsp20 "));
          TexOutput(_T("\\par\\pard\n"));
          issuedNewParagraph = 1;
          WriteEnvironmentStyles();
        }
        break;
      }
    }
    return true;
  }
  /*
   * Accents
   *
   */
  case ltACCENT_GRAVE:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      if (val)
      {
        switch (val[0])
        {
          case 'a':
           TexOutput(_T("\\'e0"));
           break;
          case 'e':
           TexOutput(_T("\\'e8"));
           break;
          case 'i':
           TexOutput(_T("\\'ec"));
           break;
          case 'o':
           TexOutput(_T("\\'f2"));
           break;
          case 'u':
           TexOutput(_T("\\'f9"));
           break;
          case 'A':
           TexOutput(_T("\\'c0"));
           break;
          case 'E':
           TexOutput(_T("\\'c8"));
           break;
          case 'I':
           TexOutput(_T("\\'cc"));
           break;
          case 'O':
           TexOutput(_T("\\'d2"));
           break;
          case 'U':
           TexOutput(_T("\\'d9"));
           break;
          default:
           break;
        }
      }
    }
    return false;
  }
  case ltACCENT_ACUTE:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      if (val)
      {
        switch (val[0])
        {
          case 'a':
           TexOutput(_T("\\'e1"));
           break;
          case 'e':
           TexOutput(_T("\\'e9"));
           break;
          case 'i':
           TexOutput(_T("\\'ed"));
           break;
          case 'o':
           TexOutput(_T("\\'f3"));
           break;
          case 'u':
           TexOutput(_T("\\'fa"));
           break;
          case 'y':
           TexOutput(_T("\\'fd"));
           break;
          case 'A':
           TexOutput(_T("\\'c1"));
           break;
          case 'E':
           TexOutput(_T("\\'c9"));
           break;
          case 'I':
           TexOutput(_T("\\'cd"));
           break;
          case 'O':
           TexOutput(_T("\\'d3"));
           break;
          case 'U':
           TexOutput(_T("\\'da"));
           break;
          case 'Y':
           TexOutput(_T("\\'dd"));
           break;
          default:
           break;
        }
      }
    }
    return false;
  }
  case ltACCENT_CARET:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      if (val)
      {
        switch (val[0])
        {
          case 'a':
           TexOutput(_T("\\'e2"));
           break;
          case 'e':
           TexOutput(_T("\\'ea"));
           break;
          case 'i':
           TexOutput(_T("\\'ee"));
           break;
          case 'o':
           TexOutput(_T("\\'f4"));
           break;
          case 'u':
           TexOutput(_T("\\'fb"));
           break;
          case 'A':
           TexOutput(_T("\\'c2"));
           break;
          case 'E':
           TexOutput(_T("\\'ca"));
           break;
          case 'I':
           TexOutput(_T("\\'ce"));
           break;
          case 'O':
           TexOutput(_T("\\'d4"));
           break;
          case 'U':
           TexOutput(_T("\\'db"));
           break;
          default:
           break;
        }
      }
    }
    return false;
  }
  case ltACCENT_TILDE:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      if (val)
      {
        switch (val[0])
        {
          case 'a':
           TexOutput(_T("\\'e3"));
           break;
          case ' ':
           TexOutput(_T("~"));
           break;
          case 'n':
           TexOutput(_T("\\'f1"));
           break;
          case 'o':
           TexOutput(_T("\\'f5"));
           break;
          case 'A':
           TexOutput(_T("\\'c3"));
           break;
          case 'N':
           TexOutput(_T("\\'d1"));
           break;
          case 'O':
           TexOutput(_T("\\'d5"));
           break;
          default:
           break;
        }
      }
    }
    return false;
  }
  case ltACCENT_UMLAUT:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      if (val)
      {
        switch (val[0])
        {
          case 'a':
           TexOutput(_T("\\'e4"));
           break;
          case 'e':
           TexOutput(_T("\\'eb"));
           break;
          case 'i':
           TexOutput(_T("\\'ef"));
           break;
          case 'o':
           TexOutput(_T("\\'f6"));
           break;
          case 'u':
           TexOutput(_T("\\'fc"));
           break;
          case 's':
           TexOutput(_T("\\'df"));
           break;
          case 'y':
           TexOutput(_T("\\'ff"));
           break;
          case 'A':
           TexOutput(_T("\\'c4"));
           break;
          case 'E':
           TexOutput(_T("\\'cb"));
           break;
          case 'I':
           TexOutput(_T("\\'cf"));
           break;
          case 'O':
           TexOutput(_T("\\'d6"));
           break;
          case 'U':
           TexOutput(_T("\\'dc"));
           break;
          case 'Y':
           TexOutput(_T("\\'df"));
           break;
          default:
           break;
        }
      }
    }
    return false;
  }
  case ltACCENT_DOT:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      if (val)
      {
        switch (val[0])
        {
          case 'a':
           TexOutput(_T("\\'e5"));
           break;
          case 'A':
           TexOutput(_T("\\'c5"));
           break;
          default:
           break;
        }
      }
    }
    return false;
  }
  case ltACCENT_CADILLA:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      if (val)
      {
        switch (val[0])
        {
          case 'c':
           TexOutput(_T("\\'e7"));
           break;
          case 'C':
           TexOutput(_T("\\'c7"));
           break;
          default:
           break;
        }
      }
    }
    return false;
  }
  case ltFOOTNOTE:
  {
    static wxChar *helpTopic = NULL;
    static FILE *savedOutput = NULL;
    if (winHelp)
    {
      if (arg_no == 1)
      {
        if (start)
        {
          OnInform(_T("Consider using \\footnotepopup instead of \\footnote."));
          footnoteCount ++;
          wxChar footBuf[20];
          wxSnprintf(footBuf, sizeof(footBuf), _T("(%d)"), footnoteCount);

          TexOutput(_T(" {\\ul "));
          TexOutput(footBuf);
          TexOutput(_T("}"));
          helpTopic = FindTopicName(NULL);
          TexOutput(_T("{\\v "));

          // Remove green colour/underlining if specified
          if (!hotSpotUnderline && !hotSpotColour)
            TexOutput(_T("%"));
          else if (!hotSpotColour)
            TexOutput(_T("*"));

          TexOutput(helpTopic);
          TexOutput(_T("}"));

          wxFprintf(Popups, _T("\\page\n"));
//          wxFprintf(Popups, _T("\n${\\footnote }")); // No title
          wxFprintf(Popups, _T("\n#{\\footnote %s}\n"), helpTopic);
          wxFprintf(Popups, _T("+{\\footnote %s}\n"), GetBrowseString());
          savedOutput = CurrentOutput1;
          SetCurrentOutput(Popups);
      }
      else
      {
          SetCurrentOutput(savedOutput);
      }
        return true;
      }
      return true;
    }
    else
    {
      if (start)
      {
        TexOutput(_T(" {\\super \\chftn{\\footnote \\fs20 {\\super \\chftn}"), true);
      }
      else
      {
        TexOutput(_T("}}"), true);
      }
      return true;
    }
  }
  case ltFOOTNOTEPOPUP:
  {
    static wxChar *helpTopic = NULL;
    static FILE *savedOutput = NULL;
    if (winHelp)
    {
      if (arg_no == 1)
      {
        if (start)
        {
          TexOutput(_T("{\\ul "));
      }
        else
        {
          TexOutput(_T("}"));
        }
        return true;
      }
      else if (arg_no == 2)
      {
        if (start)
        {
          helpTopic = FindTopicName(NULL);
          TexOutput(_T("{\\v "));

          // Remove green colour/underlining if specified
          if (!hotSpotUnderline && !hotSpotColour)
            TexOutput(_T("%"));
          else if (!hotSpotColour)
            TexOutput(_T("*"));

          TexOutput(helpTopic);
          TexOutput(_T("}"));

          wxFprintf(Popups, _T("\\page\n"));
//          wxFprintf(Popups, _T("\n${\\footnote }")); // No title
          wxFprintf(Popups, _T("\n#{\\footnote %s}\n"), helpTopic);
          wxFprintf(Popups, _T("+{\\footnote %s}\n"), GetBrowseString());
          savedOutput = CurrentOutput1;
          SetCurrentOutput(Popups);
        }
        else
        {
          SetCurrentOutput(savedOutput);
        }
        return true;
      }
    }
    else
    {
      if (arg_no == 1)
        return true;
      if (start)
      {
        TexOutput(_T(" {\\super \\chftn{\\footnote \\fs20 {\\super \\chftn}"), true);
      }
      else
      {
        TexOutput(_T("}}"), true);
      }
      return true;
    }
    break;
  }
  case ltFANCYPLAIN:
  {
    if (start && (arg_no == 1))
      return false;
    else
      return true;
  }
  case ltSETHEADER:
  {
    if (start)
      forbidResetPar ++;
    else
      forbidResetPar --;

    if (winHelp) return false;
    if (start)
    {
      switch (arg_no)
      {
        case 1:
          LeftHeaderEven = GetArgChunk();
          if (wxStrlen(GetArgData(LeftHeaderEven)) == 0)
            LeftHeaderEven = NULL;
          break;
        case 2:
          CentreHeaderEven = GetArgChunk();
          if (wxStrlen(GetArgData(CentreHeaderEven)) == 0)
            CentreHeaderEven = NULL;
          break;
        case 3:
          RightHeaderEven = GetArgChunk();
          if (wxStrlen(GetArgData(RightHeaderEven)) == 0)
            RightHeaderEven = NULL;
          break;
        case 4:
          LeftHeaderOdd = GetArgChunk();
          if (wxStrlen(GetArgData(LeftHeaderOdd)) == 0)
            LeftHeaderOdd = NULL;
          break;
        case 5:
          CentreHeaderOdd = GetArgChunk();
          if (wxStrlen(GetArgData(CentreHeaderOdd)) == 0)
            CentreHeaderOdd = NULL;
          break;
        case 6:
          RightHeaderOdd = GetArgChunk();
          if (wxStrlen(GetArgData(RightHeaderOdd)) == 0)
            RightHeaderOdd = NULL;
          OutputRTFHeaderCommands();
          break;
        default:
          break;
      }
    }
    return false;
  }
  case ltSETFOOTER:
  {
    if (start)
      forbidResetPar ++;
    else
      forbidResetPar --;

    if (winHelp) return false;
    if (start)
    {
      switch (arg_no)
      {
        case 1:
          LeftFooterEven = GetArgChunk();
          if (wxStrlen(GetArgData(LeftFooterEven)) == 0)
            LeftFooterEven = NULL;
          break;
        case 2:
          CentreFooterEven = GetArgChunk();
          if (wxStrlen(GetArgData(CentreFooterEven)) == 0)
            CentreFooterEven = NULL;
          break;
        case 3:
          RightFooterEven = GetArgChunk();
          if (wxStrlen(GetArgData(RightFooterEven)) == 0)
            RightFooterEven = NULL;
          break;
        case 4:
          LeftFooterOdd = GetArgChunk();
          if (wxStrlen(GetArgData(LeftFooterOdd)) == 0)
            LeftFooterOdd = NULL;
          break;
        case 5:
          CentreFooterOdd = GetArgChunk();
          if (wxStrlen(GetArgData(CentreFooterOdd)) == 0)
            CentreFooterOdd = NULL;
          break;
        case 6:
          RightFooterOdd = GetArgChunk();
          if (wxStrlen(GetArgData(RightFooterOdd)) == 0)
            RightFooterOdd = NULL;
          OutputRTFFooterCommands();
          break;
        default:
          break;
      }
    }
    return false;
  }
  case ltMARKRIGHT:
  {
    if (winHelp) return false;
    // Fake a SetHeader command
    if (start)
    {
      LeftHeaderOdd = NULL;
      CentreHeaderOdd = NULL;
      RightHeaderOdd = NULL;
      LeftHeaderEven = NULL;
      CentreHeaderEven = NULL;
      RightHeaderEven = NULL;
      OnInform(_T("Consider using setheader/setfooter rather than markright."));
    }
    RTFOnArgument(ltSETHEADER, 4, start);
    if (!start)
      OutputRTFHeaderCommands();
    return false;
  }
  case ltMARKBOTH:
  {
    if (winHelp) return false;
    // Fake a SetHeader command
    switch (arg_no)
    {
      case 1:
      {
        if (start)
        {
          LeftHeaderOdd = NULL;
          CentreHeaderOdd = NULL;
          RightHeaderOdd = NULL;
          LeftHeaderEven = NULL;
          CentreHeaderEven = NULL;
          RightHeaderEven = NULL;
          OnInform(_T("Consider using setheader/setfooter rather than markboth."));
        }
        return RTFOnArgument(ltSETHEADER, 1, start);
      }
      case 2:
      {
        RTFOnArgument(ltSETHEADER, 4, start);
        if (!start)
          OutputRTFHeaderCommands();
        return false;
      }
    }
    break;
  }
  case ltPAGENUMBERING:
  {
    if (start)
      forbidResetPar ++;
    else
      forbidResetPar --;

    if (winHelp) return false;
    if (start)
    {
      TexOutput(_T("\\pgnrestart"));
      wxChar *data = GetArgData();
      if (currentNumberStyle) delete[] currentNumberStyle;
      currentNumberStyle = copystring(data);
      OutputNumberStyle(currentNumberStyle);

      TexOutput(_T("\n"));
    }
    return false;
  }
  case ltTWOCOLUMN:
  {
    if (winHelp) return false;
    if (start)
      return true;
    break;
  }
  case ltITEMSEP:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      currentItemSep = ParseUnitArgument(val);
      return false;
    }
    break;
  }
  case ltEVENSIDEMARGIN:
  {
    return false;
  }
  case ltODDSIDEMARGIN:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      int twips = (int)(20*ParseUnitArgument(val));
      // Add an inch since in LaTeX it's specified minus an inch
      twips += 1440;
      CurrentLeftMarginOdd = twips;
      wxSnprintf(buf, sizeof(buf), _T("\\margl%d\n"), twips);
      TexOutput(buf);

      CurrentMarginParX = CurrentLeftMarginOdd + CurrentTextWidth + CurrentMarginParSep;
    }
    return false;
  }
  case ltMARGINPARWIDTH:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      int twips = (int)(20*ParseUnitArgument(val));
      CurrentMarginParWidth = twips;
    }
    return false;
  }
  case ltMARGINPARSEP:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      int twips = (int)(20*ParseUnitArgument(val));
      CurrentMarginParSep = twips;
      CurrentMarginParX = CurrentLeftMarginOdd + CurrentTextWidth + CurrentMarginParSep;
    }
    return false;
  }
  case ltTEXTWIDTH:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      int twips = (int)(20*ParseUnitArgument(val));
      CurrentTextWidth = twips;

      // Need to set an implicit right margin
      CurrentRightMarginOdd = PageWidth - CurrentTextWidth - CurrentLeftMarginOdd;
      CurrentRightMarginEven = PageWidth - CurrentTextWidth - CurrentLeftMarginEven;
      CurrentMarginParX = CurrentLeftMarginOdd + CurrentTextWidth + CurrentMarginParSep;
      wxSnprintf(buf, sizeof(buf), _T("\\margr%d\n"), CurrentRightMarginOdd);
      TexOutput(buf);
    }
    return false;
  }
  case ltMARGINPAR:
  case ltMARGINPARODD:
  {
    if (start)
    {
      if (winHelp)
      {
        TexOutput(_T("\\sa200\\box\n"));
        PushEnvironmentStyle(_T("\\sa200\\box"));
      }
      else
      {
        wxSnprintf(buf, sizeof(buf), _T("\\phpg\\posx%d\\absw%d\n"), CurrentMarginParX, CurrentMarginParWidth);
        TexOutput(buf);
      }
      return true;
    }
    else
    {
      if (winHelp)
      {
        TexOutput(_T("\\par\\pard\n"));
        PopEnvironmentStyle();
        WriteEnvironmentStyles();
      }
      else
        TexOutput(_T("\\par\\pard\n"));
      issuedNewParagraph = 1;
    }
    return false;
  }
  case ltMARGINPAREVEN:
  {
    if (start)
    {
      if (winHelp)
      {
        TexOutput(_T("\\sa200\\box\n"));
        PushEnvironmentStyle(_T("\\sa200\\box"));
      }
      else
      {
        if (mirrorMargins)
        {
          // Have to calculate what the margins are changed to in WfW margin
          // mirror mode, on an even (left-hand) page.
          int x = PageWidth - CurrentRightMarginOdd - CurrentMarginParWidth - CurrentMarginParSep
                    - CurrentTextWidth + GutterWidth;
          wxSnprintf(buf, sizeof(buf), _T("\\phpg\\posx%d\\absw%d\n"), x, CurrentMarginParWidth);
          TexOutput(buf);
        }
        else
        {
          wxSnprintf(buf, sizeof(buf), _T("\\phpg\\posx%d\\absw%d\n"), CurrentMarginParX, CurrentMarginParWidth);
          TexOutput(buf);
        }
      }
      return true;
    }
    else
    {
      if (winHelp)
      {
        TexOutput(_T("\\par\\pard\n"));
        PopEnvironmentStyle();
        WriteEnvironmentStyles();
      }
      else
        issuedNewParagraph = 1;
      TexOutput(_T("\\par\\pard\n"));
    }
    return false;
  }
  case ltTWOCOLWIDTHA:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      int twips = (int)(20*ParseUnitArgument(val));
      TwoColWidthA = twips;
    }
    return false;
  }
  case ltTWOCOLWIDTHB:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      int twips = (int)(20*ParseUnitArgument(val));
      TwoColWidthB = twips;
    }
    return false;
  }
  case ltROW:
  case ltRULEDROW:
  {
    if (start)
    {
      int currentWidth = 0;

      if (!compatibilityMode || (currentRowNumber > 0))
      {
      TexOutput(_T("\\pard\\intbl"));

      if (macroId == ltRULEDROW)
        ruleBottom = 1;
      for (int i = 0; i < noColumns; i++)
      {
        currentWidth += TableData[i].width;
        if (ruleTop == 1)
        {
          TexOutput(_T("\\clbrdrt\\brdrs\\brdrw15"));
        }
        else if (ruleTop > 1)
        {
          TexOutput(_T("\\clbrdrt\\brdrdb\\brdrw15"));
        }
        if (ruleBottom == 1)
        {
          TexOutput(_T("\\clbrdrb\\brdrs\\brdrw15"));
        }
        else if (ruleBottom > 1)
        {
          TexOutput(_T("\\clbrdrb\\brdrdb\\brdrw15"));
        }

        if (TableData[i].rightBorder)
          TexOutput(_T("\\clbrdrr\\brdrs\\brdrw15"));

        if (TableData[i].leftBorder)
          TexOutput(_T("\\clbrdrl\\brdrs\\brdrw15"));

        wxSnprintf(buf, sizeof(buf), _T("\\cellx%d"), currentWidth);
        TexOutput(buf);
      }
      TexOutput(_T("\\pard\\intbl\n"));
      }
      ruleTop = 0;
      ruleBottom = 0;
      currentRowNumber ++;
      return true;
    }
    else
    {
//      TexOutput(_T("\\cell\\row\\trowd\\trgaph108\\trleft-108\n"));
      TexOutput(_T("\\cell\\row\\trowd\\trgaph108\n"));
    }
    break;
  }
  case ltMULTICOLUMN:
  {
    static int noMultiColumns = 0;
    if (start)
    {
      switch (arg_no)
      {
        case 1:
        {
          noMultiColumns = wxAtoi(GetArgData());
          return false;
        }
        case 2:
        {
          return false;
        }
        case 3:
        {
          return true;
        }
      }
    }
    else
    {
      if (arg_no == 3)
      {
        for (int i = 1; i < noMultiColumns; i ++)
          TexOutput(_T("\\cell"));
      }
    }
    break;
  }
  case ltINDENTED:
  {
    if (start && (arg_no == 1))
    {
//      indentLevel ++;
//      TexOutput(_T("\\fi0\n"));
      int oldIndent = 0;
      wxNode *node = itemizeStack.GetFirst();
      if (node)
        oldIndent = ((ItemizeStruc *)node->GetData())->indentation;

      int indentValue = 20*ParseUnitArgument(GetArgData());
      int indentSize = indentValue + oldIndent;

      ItemizeStruc *struc = new ItemizeStruc(LATEX_INDENT, indentSize);
      itemizeStack.Insert(struc);

      wxSnprintf(buf, sizeof(buf), _T("\\tx%d\\li%d\\sa200 "), indentSize, indentSize);
      PushEnvironmentStyle(buf);
      TexOutput(buf);
      return false;
    }
    if (!start && (arg_no == 2))
    {
      PopEnvironmentStyle();
      if (itemizeStack.GetFirst())
      {
        ItemizeStruc *struc = (ItemizeStruc *)itemizeStack.GetFirst()->GetData();
        delete struc;
        delete itemizeStack.GetFirst();
      }
      if (itemizeStack.GetCount() == 0)
      {
        TexOutput(_T("\\par\\pard\n"));
        issuedNewParagraph = 1;
        WriteEnvironmentStyles();
      }
    }
    return true;
  }
/*
  case ltSIZEDBOX:
  case ltSIZEDBOXD:
  {
    if (start && (arg_no == 1))
    {
      int oldIndent = 0;
      wxNode *node = itemizeStack.GetFirst();
      if (node)
        oldIndent = ((ItemizeStruc *)node->GetData())->indentation;

      int boxWidth = 20*ParseUnitArgument(GetArgData());

      int indentValue = (int)((CurrentTextWidth - oldIndent - boxWidth)/2.0);
      int indentSize = indentValue + oldIndent;
      int indentSizeRight = indentSize + boxWidth;

      ItemizeStruc *struc = new ItemizeStruc(LATEX_INDENT, indentSize);
      itemizeStack.Insert(struc);

      wxSnprintf(buf, sizeof(buf), _T("\\tx%d\\li%d\\lr%d\\sa200\\box%s "), indentSize, indentSize, indentSizeRight,
        ((macroId == ltCENTEREDBOX) ? _T("\\brdrs") : _T("\\brdrdb")));
      PushEnvironmentStyle(buf);
      TexOutput(buf);
      return false;
    }
    if (!start && (arg_no == 2))
    {
      PopEnvironmentStyle();
      if (itemizeStack.GetFirst())
      {
        ItemizeStruc *struc = (ItemizeStruc *)itemizeStack.GetFirst()->GetData();
        delete struc;
        delete itemizeStack.GetFirst();
      }
      if (itemizeStack.Number() == 0)
      {
        TexOutput(_T("\\par\\pard\n"));
        issuedNewParagraph = 1;
        WriteEnvironmentStyles();
      }
    }
    return true;
    break;
  }
*/
  case ltDOCUMENTSTYLE:
  {
    DefaultOnArgument(macroId, arg_no, start);
    if (!start && !IsArgOptional())
    {
      if (MinorDocumentStyleString)
      {
        if (StringMatch(_T("twoside"), MinorDocumentStyleString))
          // Mirror margins, switch on odd/even headers & footers, and break sections at odd pages
          TexOutput(_T("\\margmirror\\facingp\\sbkodd"));
        if (StringMatch(_T("twocolumn"), MinorDocumentStyleString))
          TexOutput(_T("\\cols2"));
      }
      TexOutput(_T("\n"));
    }
    return false;
  }
  case ltSETHOTSPOTCOLOUR:
  case ltSETHOTSPOTCOLOR:
  {
    if (!start)
    {
      wxChar *text = GetArgData();
      if (wxStrcmp(text, _T("yes")) == 0 || wxStrcmp(text, _T("on")) == 0 || wxStrcmp(text, _T("ok")) == 0)
        hotSpotColour = true;
      else
        hotSpotColour = false;
    }
    return false;
  }
  case ltSETTRANSPARENCY:
  {
    if (!start)
    {
      wxChar *text = GetArgData();
      if (wxStrcmp(text, _T("yes")) == 0 || wxStrcmp(text, _T("on")) == 0 || wxStrcmp(text, _T("ok")) == 0)
        bitmapTransparency = true;
      else
        bitmapTransparency = false;
    }
    return false;
  }
  case ltSETHOTSPOTUNDERLINE:
  {
    if (!start)
    {
      wxChar *text = GetArgData();
      if (wxStrcmp(text, _T("yes")) == 0 || wxStrcmp(text, _T("on")) == 0 || wxStrcmp(text, _T("ok")) == 0)
        hotSpotUnderline = true;
      else
        hotSpotUnderline = false;
    }
    return false;
  }
  case ltBIBITEM:
  {
    if (arg_no == 1 && start)
    {
      wxChar *citeKey = GetArgData();
      TexRef *ref = (TexRef *)TexReferences.Get(citeKey);
      if (ref)
      {
        if (ref->sectionNumber) delete[] ref->sectionNumber;
        wxSnprintf(buf, sizeof(buf), _T("[%d]"), citeCount);
        ref->sectionNumber = copystring(buf);
      }

      TexOutput(_T("\\li260\\fi-260 ")); // Indent from 2nd line
      wxSnprintf(buf, sizeof(buf), _T("{\\b [%d]} "), citeCount);
      TexOutput(buf);
      citeCount ++;
      return false;
    }
    if (arg_no == 2 && !start)
      TexOutput(_T("\\par\\pard\\par\n\n"));
    return true;
  }
  case ltTHEBIBLIOGRAPHY:
  {
    if (start && (arg_no == 1))
    {
      citeCount = 1;
      if (winHelp)
        SetCurrentOutputs(Contents, Chapters);

      if (!winHelp)
      {
        wxFprintf(Chapters, _T("\\sect\\pgncont\\titlepg\n"));

        // If a non-custom page style, we generate the header now.
        if (PageStyle && (wxStrcmp(PageStyle, _T("plain")) == 0 ||
                          wxStrcmp(PageStyle, _T("empty")) == 0 ||
                          wxStrcmp(PageStyle, _T("headings")) == 0))
        {
          OutputRTFHeaderCommands();
          OutputRTFFooterCommands();
        }

        // Need to reset the current numbering style, or RTF forgets it.
        OutputNumberStyle(currentNumberStyle);
        SetCurrentOutput(Contents);
      }
      else
        wxFprintf(Chapters, _T("\\page\n"));

      if (winHelp)
        wxFprintf(Contents, _T("\n{\\uldb %s}"), ReferencesNameString);
      else
        wxFprintf(Contents, _T("\\par\n\\pard{\\b %s}"), ReferencesNameString);

      startedSections = true;

      if (winHelp)
        wxFprintf(Chapters, _T("\n${\\footnote %s}"), ReferencesNameString);

      wxChar *topicName = _T("bibliography");

      if (winHelp)
      {
        wxFprintf(Contents, _T("{\\v %s}\\par\\pard\n"), topicName);
        WriteEnvironmentStyles();
      }
      else
        wxFprintf(Contents, _T("\\par\\par\\pard\n"));

      if (winHelp)
      {
        wxFprintf(Chapters, _T("\n#{\\footnote %s}\n"), topicName);
        wxFprintf(Chapters, _T("+{\\footnote %s}\n"), GetBrowseString());
        wxFprintf(Chapters, _T("K{\\footnote {K} %s}\n"), ReferencesNameString);
        GenerateKeywordsForTopic(topicName);
        if (useUpButton)
        {
          wxFprintf(Chapters, _T("!{\\footnote EnableButton(\"Up\");ChangeButtonBinding(\"Up\", \"JumpId(`%s.hlp', `%s')\")}\n"),
               wxFileNameFromPath(FileRoot), "Contents");
        }
      }

      SetCurrentOutput(Chapters);
      wxChar *styleCommand = _T("");
      if (!winHelp && useHeadingStyles)
        styleCommand = _T("\\s1");
      wxFprintf(Chapters, _T("\\pard{%s"), (winHelp ? _T("\\keepn\\sa140\\sb140") : styleCommand));
      WriteHeadingStyle(Chapters, 1);  wxFprintf(Chapters, _T(" References\\par\\pard}\n"));

      return false;
    }
    return true;
  }
  case ltINDEX:
  {
    /*
     * In Windows help, all keywords should be at the start of the
     * topic, but Latex \index commands can be anywhere in the text.
     * So we're going to have to build up lists of keywords for a topic,
     * and insert them on the second pass.
     *
     * In linear RTF, we can embed the index entry now.
     *
     */
    if (start)
    {
//      wxChar *entry = GetArgData();
      wxChar buf[300];
      OutputChunkToString(GetArgChunk(), buf);
      if (winHelp)
      {
        if (CurrentTopic)
        {
          AddKeyWordForTopic(CurrentTopic, buf);
        }
      }
      else GenerateIndexEntry(buf);
    }
    return false;
  }
  case ltFCOL:
  case ltBCOL:
  {
    if (start)
    {
      switch (arg_no)
      {
        case 1:
        {
          wxChar *name = GetArgData();
          int pos = FindColourPosition(name);
          if (pos > -1)
          {
            wxSnprintf(buf, sizeof(buf), _T("{%s%d "), ((macroId == ltFCOL) ? _T("\\cf") : _T("\\cb")), pos);
            TexOutput(buf);
          }
          else
          {
            wxSnprintf(buf, sizeof(buf), _T("Could not find colour name %s"), name);
            OnError(buf);
          }
          break;
        }
        case 2:
        {
          return true;
        }
        default:
          break;
      }
    }
    else
    {
      if (arg_no == 2) TexOutput(_T("}"));
    }
    return false;
  }
  case ltLABEL:
  {
    if (start && !winHelp && useWord)
    {
      wxChar *s = GetArgData();
      // Only insert a bookmark here if it's not just been inserted
      // in a section heading.
      if ( !CurrentTopic || !(wxStrcmp(CurrentTopic, s) == 0) )
/*
      if ( (!CurrentChapterName || !(CurrentChapterName && (wxStrcmp(CurrentChapterName, s) == 0))) &&
           (!CurrentSectionName || !(CurrentSectionName && (wxStrcmp(CurrentSectionName, s) == 0))) &&
           (!CurrentSubsectionName || !(CurrentSubsectionName && (wxStrcmp(CurrentSubsectionName, s) == 0)))
         )
*/
      {
          wxFprintf(Chapters, _T("{\\bkmkstart %s}{\\bkmkend %s}"), s,s);
      }
    }
    return false;
  }
  case ltPAGEREF:
  {
    if (start && useWord && !winHelp)
    {
      wxChar *s = GetArgData();
      wxFprintf(Chapters, _T("{\\field{\\*\\fldinst  PAGEREF %s \\\\* MERGEFORMAT }{\\fldrslt ??}}"),
              s);
    }
    return false;
  }
  case ltPOPREFONLY:
  {
    if (start)
      inPopRefSection = true;
    else
      inPopRefSection = false;
    break;
  }
  case ltINSERTATLEVEL:
  {
    // This macro allows you to insert text at a different level
    // from the current level, e.g. into the Sections from within a subsubsection.
    if (!winHelp & useWord)
        return false;
    static int currentLevelNo = 1;
    static FILE* oldLevelFile = Chapters;
    if (start)
    {
      switch (arg_no)
      {
        case 1:
        {
          oldLevelFile = CurrentOutput1;

          wxChar *str = GetArgData();
          currentLevelNo = wxAtoi(str);
          FILE* outputFile;
          // TODO: cope with article style (no chapters)
          switch (currentLevelNo)
          {
            case 1:
            {
                outputFile = Chapters;
                break;
            }
            case 2:
            {
                outputFile = Sections;
                break;
            }
            case 3:
            {
                outputFile = Subsections;
                break;
            }
            case 4:
            {
                outputFile = Subsubsections;
                break;
            }
            default:
            {
                outputFile = NULL;
                break;
            }
          }
          if (outputFile)
            CurrentOutput1 = outputFile;
          return false;
        }
        case 2:
        {
          return true;
        }
        default:
          break;
      }
      return true;
    }
    else
    {
        if (arg_no == 2)
        {
            CurrentOutput1 = oldLevelFile;
        }
        return true;
    }
  }
  default:
    return DefaultOnArgument(macroId, arg_no, start);
  }
  return true;
}

bool RTFGo(void)
{
    if (stopRunning)
        return false;

    // Reset variables
    indentLevel = 0;
    forbidParindent = 0;
    contentsLineSection = NULL;
    contentsLineValue = NULL;
    descriptionItemArg = NULL;
    inTabular = false;
    inTable = false;
    inFigure = false;
    startRows = false;
    tableVerticalLineLeft = false;
    tableVerticalLineRight = false;
    noColumns = 0;
    startedSections = false;
    inVerbatim = false;
    browseId = 0;

    if (!InputFile.empty() && !OutputFile.empty())
    {
        // Do some RTF-specific transformations on all the strings,
        // recursively
        Text2RTF(GetTopLevelChunk());

        Contents = wxFopen(TmpContentsName, _T("w"));
        Chapters = wxFopen(_T("chapters.rtf"), _T("w"));
        if (winHelp)
        {
            Sections = wxFopen(_T("sections.rtf"), _T("w"));
            Subsections = wxFopen(_T("subsections.rtf"), _T("w"));
            Subsubsections = wxFopen(_T("subsubsections.rtf"), _T("w"));
            Popups = wxFopen(_T("popups.rtf"), _T("w"));
            if (winHelpContents)
            {
                WinHelpContentsFile = wxFopen(WinHelpContentsFileName, _T("w"));
                if (WinHelpContentsFile)
                    wxFprintf(WinHelpContentsFile, _T(":Base %s.hlp\n"), wxFileNameFromPath(FileRoot));
            }

            if (!Sections || !Subsections || !Subsubsections || !Popups || (winHelpContents && !WinHelpContentsFile))
            {
                OnError(_T("Ouch! Could not open temporary file(s) for writing."));
                return false;
            }
        }
        if (!Contents || !Chapters)
        {
            OnError(_T("Ouch! Could not open temporary file(s) for writing."));
            return false;
        }

        if (winHelp)
        {
            wxFprintf(Chapters, _T("\n#{\\footnote Contents}\n"));
            wxFprintf(Chapters, _T("${\\footnote Contents}\n"));
            wxFprintf(Chapters, _T("+{\\footnote %s}\n"), GetBrowseString());
            wxFprintf(Chapters, _T("K{\\footnote {K} %s}\n"), ContentsNameString);
            wxFprintf(Chapters, _T("!{\\footnote DisableButton(\"Up\")}\n"));
        }
        if (!winHelp)
        {
            wxFprintf(Chapters, _T("\\titlepg\n"));
            wxFprintf(Contents, _T("\\par\\pard\\pgnrestart\\sect\\titlepg"));
        }

        // In WinHelp, Contents title takes font of title.
        // In linear RTF, same as chapter headings.
        wxFprintf(Contents, _T("{\\b\\fs%d %s}\\par\\par\\pard\n\n"),
            (winHelp ? titleFont : chapterFont)*2, ContentsNameString);

        // By default, Swiss, 11 point.
        wxFprintf(Chapters, _T("\\f2\\fs22\n"));

        PushEnvironmentStyle(_T("\\f2\\fs22\\sa200"));

        SetCurrentOutput(Chapters);

        if (stopRunning)
            return false;

        OnInform(_T("Converting..."));

        TraverseDocument();

        FILE *Header = wxFopen(_T("header.rtf"), _T("w"));
        if (!Header)
        {
            OnError(_T("Ouch! Could not open temporary file header.rtf for writing."));
            return false;
        }
        WriteRTFHeader(Header);
        fclose(Header);

        PopEnvironmentStyle();

        Tex2RTFYield(true);
        if (winHelp)
        {
//          wxFprintf(Contents, _T("\\page\n"));
            wxFprintf(Chapters, _T("\\page\n"));
            wxFprintf(Sections, _T("\\page\n"));
            wxFprintf(Subsections, _T("\\page\n"));
            wxFprintf(Subsubsections, _T("\\page\n\n"));
            wxFprintf(Popups, _T("\\page\n}\n"));
        }

//      TexOutput(_T("\n\\info{\\doccomm Document created by Julian Smart's Tex2RTF.}\n"));
        if (!winHelp)
            TexOutput(_T("}\n"));
        fclose(Contents); Contents = NULL;
        fclose(Chapters); Chapters = NULL;
        if (winHelp)
        {
            fclose(Sections); Sections = NULL;
            fclose(Subsections); Subsections = NULL;
            fclose(Subsubsections); Subsubsections = NULL;
            fclose(Popups); Popups = NULL;
            if (winHelpContents)
            {
                fclose(WinHelpContentsFile); WinHelpContentsFile = NULL;
            }
        }

        if (winHelp)
        {
            wxConcatFiles(_T("header.rtf"), _T("chapters.rtf"), _T("tmp1.rtf"));
            Tex2RTFYield(true);
            wxConcatFiles(_T("tmp1.rtf"), _T("sections.rtf"), _T("tmp2.rtf"));
            Tex2RTFYield(true);
            wxConcatFiles(_T("tmp2.rtf"), _T("subsections.rtf"), _T("tmp3.rtf"));
            Tex2RTFYield(true);
            wxConcatFiles(_T("tmp3.rtf"), _T("subsubsections.rtf"), _T("tmp4.rtf"));
            Tex2RTFYield(true);
            wxConcatFiles(_T("tmp4.rtf"), _T("popups.rtf"), OutputFile);
            Tex2RTFYield(true);

            wxRemoveFile(_T("tmp1.rtf"));
            wxRemoveFile(_T("tmp2.rtf"));
            wxRemoveFile(_T("tmp3.rtf"));
            wxRemoveFile(_T("tmp4.rtf"));
        }
        else
        {
            wxConcatFiles(_T("header.rtf"), _T("chapters.rtf"), _T("tmp1.rtf"));
            Tex2RTFYield(true);
            if (wxFileExists(OutputFile))
                wxRemoveFile(OutputFile);

            wxString cwdStr = wxGetCwd();

            wxString outputDirStr = wxPathOnly(OutputFile);

            // Determine if the temp file and the output file are in the same directory,
            // and if they are, then just rename the temp file rather than copying
            // it, as this is much faster when working with large (multi-megabyte files)
            if ((outputDirStr.empty()) ||  // no path specified on output file
                (cwdStr != outputDirStr)) // paths do not match
            {
                wxRenameFile(_T("tmp1.rtf"), OutputFile);
            }
            else
            {
                wxCopyFile(_T("tmp1.rtf"), OutputFile);
            }
            Tex2RTFYield(true);
            wxRemoveFile(_T("tmp1.rtf"));
        }

        if (wxFileExists(ContentsName)) wxRemoveFile(ContentsName);

        if (!wxRenameFile(TmpContentsName, ContentsName))
        {
            wxCopyFile(TmpContentsName, ContentsName);
            wxRemoveFile(TmpContentsName);
        }

        wxRemoveFile(_T("chapters.rtf"));
        wxRemoveFile(_T("header.rtf"));

        if (winHelp)
        {
            wxRemoveFile(_T("sections.rtf"));
            wxRemoveFile(_T("subsections.rtf"));
            wxRemoveFile(_T("subsubsections.rtf"));
            wxRemoveFile(_T("popups.rtf"));
        }
        if (winHelp && generateHPJ)
            WriteHPJ(OutputFile);
        return true;
    }
    return false;
}

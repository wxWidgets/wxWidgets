/////////////////////////////////////////////////////////////////////////////
// Name:        rtfutils.cpp
// Purpose:     Converts Latex to Word RTF/WinHelp RTF
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

wxList itemizeStack;
static int indentLevel = 0;
static int forbidParindent = 0; // if > 0, no parindent (e.g. in center environment)
int forbidResetPar = 0; // If > 0, don't reset memory of having output a new par

static char *contentsLineSection = NULL;
static char *contentsLineValue = NULL;
static TexChunk *descriptionItemArg = NULL;
static wxStringList environmentStack; // Stack of paragraph styles we need to remember
static int footnoteCount = 0;
static int citeCount = 1;
extern char *FileRoot;
extern bool winHelp;
extern bool startedSections;
extern FILE *Contents;
extern FILE *Chapters;
extern FILE *Popups;
extern FILE *WinHelpContentsFile;
extern char *RTFCharset;
// This is defined in the Tex2Any library and isn't in use after parsing
extern char *BigBuffer;


extern wxHashTable TexReferences;


// Are we in verbatim mode? If so, format differently.
static bool inVerbatim = FALSE;

// We're in a series of PopRef topics, so don't output section headings
bool inPopRefSection = FALSE;

// Green colour?
static bool hotSpotColour = TRUE;
static bool hotSpotUnderline = TRUE;

// Transparency (WHITE = transparent)
static bool bitmapTransparency = TRUE;

// Linear RTF requires us to set the style per section.
static char *currentNumberStyle = NULL;
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


/*
 * Flag to say we've just issued a \par\pard command, so don't
 * repeat this unnecessarily.
 *
 */

int issuedNewParagraph = 0;

// Need to know whether we're in a table or figure for benefit
// of listoffigures/listoftables
static bool inFigure = FALSE;
static bool inTable = FALSE;

/*
 * Current topics
 *
 */
static char *CurrentChapterName = NULL;
static char *CurrentSectionName = NULL;
static char *CurrentSubsectionName = NULL;
static char *CurrentTopic = NULL;

static bool InPopups()
{
  if (CurrentChapterName && (strcmp(CurrentChapterName, "popups") == 0))
    return TRUE;
  if (CurrentSectionName && (strcmp(CurrentSectionName, "popups") == 0))
    return TRUE;
  return FALSE;
}

static void SetCurrentTopic(char *s)
{
  if (CurrentTopic) delete[] CurrentTopic;
  CurrentTopic = copystring(s);
}

void SetCurrentChapterName(char *s)
{
  if (CurrentChapterName) delete[] CurrentChapterName;
  CurrentChapterName = copystring(s);
  SetCurrentTopic(s);
}
void SetCurrentSectionName(char *s)
{
  if (CurrentSectionName) delete[] CurrentSectionName;
  CurrentSectionName = copystring(s);
  SetCurrentTopic(s);
}
void SetCurrentSubsectionName(char *s)
{
  if (CurrentSubsectionName) delete[] CurrentSubsectionName;
  CurrentSubsectionName = copystring(s);
  SetCurrentTopic(s);
}

// Indicate that a parent topic at level 'level' has children.
// Level 1 is a chapter, 2 is a section, etc.
void NotifyParentHasChildren(int parentLevel)
{
  char *parentTopic = NULL;
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
    texTopic->hasChildren = TRUE;
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
    ContentsLevels[i] = FALSE;
    
  // There are always books on the top level
  ContentsLevels[0] = TRUE;
}

// Output a WinHelp section as a keyword, substituting
// : for space.
void OutputSectionKeyword(FILE *fd)
{
  OutputCurrentSectionToString(wxBuffer);
  
  unsigned int i;
  for (i = 0; i < strlen(wxBuffer); i++)
    if (wxBuffer[i] == ':')
      wxBuffer[i] = ' ';
    // Don't write to index if there's some RTF in the string
    else if ( wxBuffer[i] == '{' )
        return;

  fprintf(fd, "K{\\footnote {K} ");
  fprintf(fd, "%s", wxBuffer);
  
  fprintf(fd, "}\n");
}

// Write a line for the .cnt file, if we're doing this.
void WriteWinHelpContentsFileLine(char *topicName, char *xitle, int level)
{
  // First, convert any RTF characters to ASCII
  char title[255];
  int s=0;
  int d=0;
  while ( (xitle[s]!=0)&&(d<255) )
  {
	  char ch=xitle[s]&0xff;
	  if (ch==0x5c) {
		  char ch1=xitle[s+1]&0xff;
		  char ch2=xitle[s+2]&0xff;
		  char ch3=xitle[s+3]&0xff;
		  char ch4=xitle[s+4]&0xff;
		  s+=4; // next character		  
		  char a=0;
		  if ((ch1==0x27)&&(ch2==0x66)&&(ch3==0x36)) { title[d++]='ö';  a=1; }
		  if ((ch1==0x27)&&(ch2==0x65)&&(ch3==0x34)) { title[d++]='ä';  a=1; }
		  if ((ch1==0x27)&&(ch2==0x66)&&(ch3==0x63)) { title[d++]='ü';  a=1; }
		  if ((ch1==0x27)&&(ch2==0x64)&&(ch3==0x36)) { title[d++]='Ö';  a=1; }
		  if ((ch1==0x27)&&(ch2==0x63)&&(ch3==0x34)) { title[d++]='Ä';  a=1; }
		  if ((ch1==0x27)&&(ch2==0x64)&&(ch3==0x63)) { title[d++]='Ü';  a=1; }
//		  if (a==0)
//			  printf("!!!!! %04X %04X %04X %04X! \n",ch1,ch2,ch3,ch4);
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
  
  if (!title)
    return;
    
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
        fprintf(WinHelpContentsFile, "%d %s\n", level, title);

        // Then we have a 'page' consisting of the text for this chapter
        fprintf(WinHelpContentsFile, "%d %s=%s\n", level+1, title, topicName);

        // Then we'll be writing out further pages or books at level + 1...

        // Remember that at this level, we had a book and *must* for the
        // remainder of sections at this level.
        ContentsLevels[level-1] = TRUE;
      }
      else
      {
        fprintf(WinHelpContentsFile, "%d %s=%s\n", level, title, topicName);
      }
    }
    else
    {
      if (level == 1 || ContentsLevels[level-1])
      {
        // Always have a book at level 1
        fprintf(WinHelpContentsFile, "%d %s\n", level, title);
        fprintf(WinHelpContentsFile, "%d %s=%s\n", level+1, title, topicName);
        ContentsLevels[level-1] = TRUE;
      }
      else
        // Probably doesn't have children if it hasn't been added to the topic table
        fprintf(WinHelpContentsFile, "%d %s=%s\n", level, title, topicName);
    }
  }
}

void SplitIndexEntry(char *entry, char *buf1, char *buf2)
{
  int len = strlen(entry); int i = 0;
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
void GenerateKeywordsForTopic(char *topic)
{
  TexTopic *texTopic = (TexTopic *)TopicTable.Get(topic);
  if (!texTopic)
    return;

  wxStringList *list = texTopic->keywords;
  if (list)
  {
    wxNode *node = list->First();
    while (node)
    {
      char *s = (char *)node->Data();

      // Must separate out main entry form subentry (only 1 subentry allowed)
      char buf1[100]; char buf2[100];
      SplitIndexEntry(s, buf1, buf2);
      
      // Check for ':' which messes up index
      unsigned int i;
      for (i = 0; i < strlen(buf1) ; i++)
        if (buf1[i] == ':')
          buf1[i] = ' ';
      for (i = 0; i < strlen(buf2) ; i++)
        if (buf2[i] == ':')
          buf2[i] = ' ';

      // {K} is a strange fix to prevent words beginning with K not
      // being indexed properly
      TexOutput("K{\\footnote {K} ");
      TexOutput(buf1);
      if (strlen(buf2) > 0)
      {
        // Output subentry
        TexOutput(", ");
        TexOutput(buf2);
      }
      TexOutput("}\n");
      node = node->Next();
    }
  }
}

/*
 * Output index entry in linear RTF
 *
 */
 
void GenerateIndexEntry(char *entry)
{
  if (useWord)
  {
    char buf1[100]; char buf2[100];
    SplitIndexEntry(entry, buf1, buf2);

    TexOutput("{\\xe\\v {");
    TexOutput(buf1);
    if (strlen(buf2) > 0)
    {
      TexOutput("\\:");
      TexOutput(buf2);
    }
    TexOutput("}}");
  }
}

 /*
  * Write a suitable RTF header.
  *
  */
  
void WriteColourTable(FILE *fd)
{
  fprintf(fd, "{\\colortbl");
  wxNode *node = ColourTable.First();
  while (node)
  {
    ColourTableEntry *entry = (ColourTableEntry *)node->Data();
    fprintf(fd, "\\red%d\\green%d\\blue%d;\n", entry->red, entry->green, entry->blue);
    node = node->Next();
  }
  fprintf(fd, "}");
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
      fprintf(fd, "\\b\\fs%d", chapterFont*2);
      break;
    }
    case 2:
    {
      fprintf(fd, "\\b\\fs%d", sectionFont*2);
      break;
    }
    case 3:
    {
      fprintf(fd, "\\b\\fs%d", subsectionFont*2);
      break;
    }
    case 4:
    {
      fprintf(fd, "\\b\\fs%d", subsectionFont*2);
      break;
    }
    default:
      break;
  }
}

void WriteRTFHeader(FILE *fd)
{
  fprintf(fd, "{\\rtf1\\%s \\deff0\n", RTFCharset);
  fprintf(fd, "{\\fonttbl{\\f0\\froman Times New Roman;}{\\f1\\ftech Symbol;}{\\f2\\fswiss Arial;}\n");
  fprintf(fd, "{\\f3\\fmodern Courier;}{\\f4\\ftech Wingdings;}{\\f5\\ftech Monotype Sorts;}\n}");
  /*
   * Style sheet
   */
  fprintf(fd, "{\\stylesheet{\\f2\\fs20 \\snext0 Normal;}\n");
  // Headings
  fprintf(fd, "{\\s1 "); WriteHeadingStyle(fd, 1); fprintf(fd, "\\sbasedon0\\snext0 heading 1;}\n");
  fprintf(fd, "{\\s2 "); WriteHeadingStyle(fd, 2); fprintf(fd, "\\sbasedon0\\snext0 heading 2;}\n");
  fprintf(fd, "{\\s3 "); WriteHeadingStyle(fd, 3); fprintf(fd, "\\sbasedon0\\snext0 heading 3;}\n");
  fprintf(fd, "{\\s4 "); WriteHeadingStyle(fd, 4); fprintf(fd, "\\sbasedon0\\snext0 heading 4;}\n");
  // Table of contents styles
  fprintf(fd, "{\\s20\\sb300\\tqr\\tldot\\tx8640 \\b\\f2 \\sbasedon0\\snext0 toc 1;}\n");
  
  fprintf(fd, "{\\s21\\sb90\\tqr\\tldot\\li400\\tqr\\tx8640 \\f2\\fs20\\sbasedon0\\snext0 toc 2;}\n");
  fprintf(fd, "{\\s22\\sb90\\tqr\\tldot\\li800\\tx8640 \\f2\\fs20 \\sbasedon0\\snext0 toc 3;}\n");
  fprintf(fd, "{\\s23\\sb90\\tqr\\tldot\\li1200\\tx8640 \\f2\\fs20 \\sbasedon0\\snext0 toc 4;}\n");

  // Index styles
  fprintf(fd, "{\\s30\\fi-200\\li200\\tqr\\tx3960 \\f2\\fs18 \\sbasedon0\\snext0 index 1;}\n");
  fprintf(fd, "{\\s31\\fi-200\\li400\\tqr\\tx3960 \\f2\\fs18 \\sbasedon0\\snext0 index 2;}\n");
  fprintf(fd, "{\\s32\\fi-200\\li600\\tqr\\tx3960 \\f2\\fs18 \\sbasedon0\\snext0 index 3;}\n");
  fprintf(fd, "{\\s33\\fi-200\\li800\\tqr\\tx3960 \\f2\\fs18 \\sbasedon0\\snext0 index 4;}\n");
  fprintf(fd, "{\\s35\\qc\\sb240\\sa120 \\b\\f2\\fs26 \\sbasedon0\\snext30 index heading;}\n");
  fprintf(fd, "}\n");

  WriteColourTable(fd);
  fprintf(fd, "\n\\ftnbj\\ftnrestart"); // Latex default is footnotes at bottom of page, not section.
  fprintf(fd, "\n");
}

void OutputNumberStyle(char *numberStyle)
{
  if (numberStyle)
  {
    if (strcmp(numberStyle, "arabic") == 0)
    {
      TexOutput("\\pgndec");
    }
    else if (strcmp(numberStyle, "roman") == 0)
    {
      TexOutput("\\pgnlcrm");
    }
    else if (strcmp(numberStyle, "Roman") == 0)
    {
      TexOutput("\\pgnucrm");
    }
    else if (strcmp(numberStyle, "alph") == 0)
    {
      TexOutput("\\pgnlcltr");
    }
    else if (strcmp(numberStyle, "Alph") == 0)
    {
      TexOutput("\\pgnucltr");
    }
  }
}

/*
 * Write a Windows help project file
 */
 
bool WriteHPJ(char *filename)
{
  char hpjFilename[256];
  char helpFile[50];
  char rtfFile[50];
  strcpy(hpjFilename, filename);
  StripExtension(hpjFilename);
  strcat(hpjFilename, ".hpj");

  strcpy(helpFile, FileNameFromPath(filename));
  StripExtension(helpFile);
  strcpy(rtfFile, helpFile);
  strcat(helpFile, ".hlp");
  strcat(rtfFile, ".rtf");
  
  FILE *fd = fopen(hpjFilename, "w");
  if (!fd)
    return FALSE;

  char *helpTitle = winHelpTitle;
  if (!helpTitle)
    helpTitle = "Untitled";
    
  wxString thePath = wxPathOnly(InputFile);
  if (thePath.IsEmpty())
    thePath = ".";
  fprintf(fd, "[OPTIONS]\n");
  fprintf(fd, "BMROOT=%s ; Assume that bitmaps are where the source is\n", thePath.c_str());
  fprintf(fd, "TITLE=%s\n", helpTitle);
  fprintf(fd, "CONTENTS=Contents\n");

  if (winHelpVersion > 3)
  {
    fprintf(fd, "; COMPRESS=12 Hall Zeck ; Max compression, but needs lots of memory\n");
    fprintf(fd, "COMPRESS=8 Zeck\n");
    fprintf(fd, "LCID=0x809 0x0 0x0 ;English (British)\n");
    fprintf(fd, "HLP=.\\%s.hlp\n", wxFileNameFromPath(FileRoot));
  }
  else
  {
    fprintf(fd, "COMPRESS=HIGH\n");
  }
  fprintf(fd, "\n");

  if (winHelpVersion > 3)
  {
    fprintf(fd, "[WINDOWS]\n");
    fprintf(fd, "Main=\"\",(553,102,400,600),20736,(r14876671),(r12632256),f3\n");
    fprintf(fd, "\n");
  }

  fprintf(fd, "[FILES]\n%s\n\n", rtfFile);
  fprintf(fd, "[CONFIG]\n");
  if (useUpButton)
    fprintf(fd, "CreateButton(\"Up\", \"&Up\", \"JumpId(`%s', `Contents')\")\n", helpFile);
  fprintf(fd, "BrowseButtons()\n\n");
  fprintf(fd, "[MAP]\n\n[BITMAPS]\n\n");
  fclose(fd);
  return TRUE;
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
  bool changed = FALSE;
  int ptr = 0;
  int i = 0;
  char ch = 1;
  int len = strlen(chunk->value);
  while (ch != 0)
  {
    ch = chunk->value[i];

    if (ch == 10)
    {
      if (inVerbatim)
      {
        BigBuffer[ptr] = 0; strcat(BigBuffer, "\\par\n"); ptr += 5;
        i ++;
        changed = TRUE;
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
//        if (TRUE)
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
          changed = TRUE;
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
      changed = TRUE;
    }
    else if (!inVerbatim && ch == '`') // Change ` to '
    {
      BigBuffer[ptr] = 39; ptr ++;
      i += 1;
      changed = TRUE;
    }
    else if (inVerbatim && ch == '\\') // Change backslash to two backslashes
    {
      BigBuffer[ptr] = '\\'; ptr ++;
      BigBuffer[ptr] = '\\'; ptr ++;
      i += 1;
      changed = TRUE;
    }
    else if (inVerbatim && (ch == '{' || ch == '}')) // Escape the curly bracket
    {
      BigBuffer[ptr] = '\\'; ptr ++;
      BigBuffer[ptr] = ch; ptr ++;
      i += 1;
      changed = TRUE;
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
        inVerbatim = TRUE;

      wxNode *node = chunk->children.First();
      while (node)
      {
        TexChunk *child_chunk = (TexChunk *)node->Data();
        Text2RTF(child_chunk);
        node = node->Next();
      }

      if (def && (def->macroId == ltVERBATIM || def->macroId == ltVERB))
        inVerbatim = FALSE;

      break;
    }
    case CHUNK_TYPE_ARG:
    {
      wxNode *node = chunk->children.First();
      while (node)
      {
        TexChunk *child_chunk = (TexChunk *)node->Data();
        Text2RTF(child_chunk);
        node = node->Next();
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
 
char browseBuf[10];
static long browseId = 0;
char *GetBrowseString(void)
{
  char buf[10];
  browseId ++;
  sprintf(buf, "%ld", browseId);
  int noZeroes = 5-strlen(buf);
  strcpy(browseBuf, "browse");
  for (int i = 0; i < noZeroes; i++)
    strcat(browseBuf, "0");
  strcat(browseBuf, buf);
  return browseBuf;
}

/*
 * Keeping track of environments to restore the styles after \pard.
 * Push strings like "\qc" onto stack.
 *
 */

void PushEnvironmentStyle(char *style)
{
  environmentStack.Add(style);
}

void PopEnvironmentStyle(void)
{
  wxNode *node = environmentStack.Last();
  if (node)
  {
    char *val = (char *)node->Data();
    delete[] val;
    delete node;
  }
}

// Write out the styles, most recent first.
void WriteEnvironmentStyles(void)
{
  wxNode *node = environmentStack.Last();
  while (node)
  {
    char *val = (char *)node->Data();
    TexOutput(val);
    node = node->Next();
  }
  if (!inTabular && (ParIndent > 0) && (forbidParindent == 0))
  {
    char buf[15];
    sprintf(buf, "\\fi%d", ParIndent*20); // Convert points to TWIPS
    TexOutput(buf);
  }
  if (environmentStack.Number() > 0 || (ParIndent > 0))
    TexOutput("\n");
}


/*
 * Output a header
 *
 */

void OutputRTFHeaderCommands(void)
{
  char buf[300];
  if (PageStyle && strcmp(PageStyle, "plain") == 0)
  {
    TexOutput("{\\headerl }{\\headerr }");
  }
  else if (PageStyle && strcmp(PageStyle, "empty") == 0)
  {
    TexOutput("{\\headerl }{\\headerr }");
  }
  else if (PageStyle && strcmp(PageStyle, "headings") == 0)
  {
    // Left header
    TexOutput("{\\headerl\\fi0 ");

    if (headerRule)
      TexOutput("\\brdrb\\brdrs\\brdrw15\\brsp20 ");

    TexOutput("{\\i \\qr ");
    if (DocumentStyle == LATEX_ARTICLE)
    {
      sprintf(buf, "SECTION %d", sectionNo);
      TexOutput(buf);
    }
    else
    {
      sprintf(buf, "CHAPTER %d: ", chapterNo);
      TexOutput(buf);
    }
    TexOutput("{\\field{\\*\\fldinst PAGE \\\\* MERGEFORMAT }{\\fldrslt 1}}");
    TexOutput("}\\par\\pard}");

    // Right header
    TexOutput("{\\headerr\\fi0 ");

    if (headerRule)
      TexOutput("\\brdrb\\brdrs\\brdrw15\\brsp20 ");

    TexOutput("{\\i \\qc ");
    if (DocumentStyle == LATEX_ARTICLE)
    {
      sprintf(buf, "SECTION %d", sectionNo);
      TexOutput(buf);
    }
    else
    {
      sprintf(buf, "CHAPTER %d", chapterNo);
      TexOutput(buf);
    }
    TexOutput("{\\field{\\*\\fldinst PAGE \\\\* MERGEFORMAT }{\\fldrslt 1}}");
    TexOutput("}\\par\\pard}");
  }
  else
  {
    int oldForbidResetPar = forbidResetPar;
    forbidResetPar = 0;
    
    if (LeftHeaderEven || CentreHeaderEven || RightHeaderEven)
    {
      TexOutput("{\\headerl\\fi0 ");

      if (headerRule)
        TexOutput("\\brdrb\\brdrs\\brdrw15\\brsp20 ");

      if (LeftHeaderEven)
      {
        if (!CentreHeaderEven && !RightHeaderEven)
          TexOutput("\\ql ");
        TraverseChildrenFromChunk(LeftHeaderEven);
      }
      if (CentreHeaderEven)
      {
        if (!LeftHeaderEven && !RightHeaderEven)
          TexOutput("\\qc ");
        else
          TexOutput("\\tab\\tab\\tab ");
        TraverseChildrenFromChunk(CentreHeaderEven);
      }
      if (RightHeaderEven)
      {
        if (!LeftHeaderEven && !CentreHeaderEven)
          TexOutput("\\qr ");
        else
          TexOutput("\\tab\\tab\\tab\\tab\\tab\\tab ");
        TraverseChildrenFromChunk(RightHeaderEven);
      }
      TexOutput("\\par\\pard}");
    }

    if (LeftHeaderOdd || CentreHeaderOdd || RightHeaderOdd)
    {
      TexOutput("{\\headerr\\fi0 ");

      if (headerRule)
        TexOutput("\\brdrb\\brdrs\\brdrw15\\brsp20 ");

      if (LeftHeaderOdd)
      {
        if (!CentreHeaderOdd && !RightHeaderOdd)
          TexOutput("\\ql ");
        TraverseChildrenFromChunk(LeftHeaderOdd);
      }
      if (CentreHeaderOdd)
      {
        if (!LeftHeaderOdd && !RightHeaderOdd)
          TexOutput("\\qc ");
        else
          TexOutput("\\tab\\tab\\tab ");
        TraverseChildrenFromChunk(CentreHeaderOdd);
      }
      if (RightHeaderOdd)
      {
        if (!LeftHeaderOdd && !CentreHeaderOdd)
          TexOutput("\\qr ");
        else
          TexOutput("\\tab\\tab\\tab\\tab\\tab\\tab ");
        TraverseChildrenFromChunk(RightHeaderOdd);
      }
      TexOutput("\\par\\pard}");
    }
    // As an approximation, don't put a header on the first page of a section.
    // This may not always be desired, but it's a reasonable guess.
    TexOutput("{\\headerf }");

    forbidResetPar = oldForbidResetPar;
  }
}

void OutputRTFFooterCommands(void)
{
  if (PageStyle && strcmp(PageStyle, "plain") == 0)
  {
    TexOutput("{\\footerl\\fi0 ");
    if (footerRule)
      TexOutput("\\brdrt\\brdrs\\brdrw15\\brsp20 ");
    TexOutput("{\\qc ");
    TexOutput("{\\field{\\*\\fldinst PAGE \\\\* MERGEFORMAT }{\\fldrslt 1}}");
    TexOutput("}\\par\\pard}");

    TexOutput("{\\footerr\\fi0 ");
    if (footerRule)
      TexOutput("\\brdrt\\brdrs\\brdrw15\\brsp20 ");
    TexOutput("{\\qc ");
    TexOutput("{\\field{\\*\\fldinst PAGE \\\\* MERGEFORMAT }{\\fldrslt 1}}");
    TexOutput("}\\par\\pard}");
  }
  else if (PageStyle && strcmp(PageStyle, "empty") == 0)
  {
    TexOutput("{\\footerl }{\\footerr }");
  }
  else if (PageStyle && strcmp(PageStyle, "headings") == 0)
  {
    TexOutput("{\\footerl }{\\footerr }");
  }
  else
  {
    if (LeftFooterEven || CentreFooterEven || RightFooterEven)
    {
      TexOutput("{\\footerl\\fi0 ");
      if (footerRule)
        TexOutput("\\brdrt\\brdrs\\brdrw15\\brsp20 ");
      if (LeftFooterEven)
      {
        if (!CentreFooterEven && !RightFooterEven)
          TexOutput("\\ql ");
        TraverseChildrenFromChunk(LeftFooterEven);
      }
      if (CentreFooterEven)
      {
        if (!LeftFooterEven && !RightFooterEven)
          TexOutput("\\qc ");
        else
          TexOutput("\\tab\\tab\\tab ");
        TraverseChildrenFromChunk(CentreFooterEven);
      }
      if (RightFooterEven)
      {
        if (!LeftFooterEven && !CentreFooterEven)
          TexOutput("\\qr ");
        else
          TexOutput("\\tab\\tab\\tab\\tab\\tab\\tab ");
        TraverseChildrenFromChunk(RightFooterEven);
      }
      TexOutput("\\par\\pard}");
    }

    if (LeftFooterOdd || CentreFooterOdd || RightFooterOdd)
    {
      TexOutput("{\\footerr\\fi0 ");
      if (footerRule)
        TexOutput("\\brdrt\\brdrs\\brdrw15\\brsp20 ");
      if (LeftFooterOdd)
      {
        if (!CentreFooterOdd && !RightFooterOdd)
          TexOutput("\\ql ");
        TraverseChildrenFromChunk(LeftFooterOdd);
      }
      if (CentreFooterOdd)
      {
        if (!LeftFooterOdd && !RightFooterOdd)
          TexOutput("\\qc ");
        else
          TexOutput("\\tab\\tab\\tab ");
        TraverseChildrenFromChunk(CentreFooterOdd);
      }
      if (RightFooterOdd)
      {
        if (!LeftFooterOdd && !CentreFooterOdd)
          TexOutput("\\qr ");
        else
          TexOutput("\\tab\\tab\\tab\\tab\\tab\\tab ");
        TraverseChildrenFromChunk(RightFooterOdd);
      }
      TexOutput("\\par\\pard}");
    }

    // As an approximation, put a footer on the first page of a section.
    // This may not always be desired, but it's a reasonable guess.
    if (LeftFooterOdd || CentreFooterOdd || RightFooterOdd)
    {
      TexOutput("{\\footerf\\fi0 ");
      if (LeftFooterOdd)
      {
        if (!CentreFooterOdd && !RightFooterOdd)
          TexOutput("\\ql ");
        TraverseChildrenFromChunk(LeftFooterOdd);
      }
      if (CentreFooterOdd)
      {
        if (!LeftFooterOdd && !RightFooterOdd)
          TexOutput("\\qc ");
        else
          TexOutput("\\tab\\tab\\tab ");
        TraverseChildrenFromChunk(CentreFooterOdd);
      }
      if (RightFooterOdd)
      {
        if (!LeftFooterOdd && !CentreFooterOdd)
          TexOutput("\\qr ");
        else
          TexOutput("\\tab\\tab\\tab\\tab\\tab\\tab ");
        TraverseChildrenFromChunk(RightFooterOdd);
      }
      TexOutput("\\par\\pard}");
    }
  }
}

// Called on start/end of macro examination
void RTFOnMacro(int macroId, int no_args, bool start)
{
/*
  char tmpBuf[40];
  sprintf(tmpBuf, "%d (%d)", macroId, (int)start);
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

  char buf[300];
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

      char *topicName = FindTopicName(GetNextChunk());
      SetCurrentChapterName(topicName);

      if (winHelpContents && winHelp && !InPopups())
      {
        OutputCurrentSectionToString(wxBuffer);
        WriteWinHelpContentsFileLine(topicName, wxBuffer, 1);
      }
      AddTexRef(topicName, NULL, ChapterNameString, chapterNo);

      if (winHelp)
      {
        if (!InPopups())
          fprintf(Contents, "\n{\\uldb ");
        fprintf(Chapters, "\\page");
        fprintf(Chapters, "\n${\\footnote ");
        if (!InPopups())
          SetCurrentOutputs(Contents, Chapters);
        else
          SetCurrentOutput(Chapters);
      }
      else
      {
        fprintf(Chapters, "\\sect\\pgncont\\titlepg\n");

        // If a non-custom page style, we generate the header now.
        if (PageStyle && (strcmp(PageStyle, "plain") == 0 ||
                          strcmp(PageStyle, "empty") == 0 ||
                          strcmp(PageStyle, "headings") == 0))
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
            fprintf(Contents, "\\par\n\\pard{\\b %d\\tab ", chapterNo);
          }
          else if (macroId == ltCHAPTERHEADING)
          {
            fprintf(Contents, "\\par\n\\pard{\\b ");
          }
          else SetCurrentOutput(NULL); // No entry in table of contents
        }
      }
  
      startedSections = TRUE;

      // Output heading to contents page
      if (!InPopups())
      {
        OutputCurrentSection();
      
        if (winHelp)
          fprintf(Contents, "}{\\v %s}\\par\\pard\n", topicName);
        else if ((macroId == ltCHAPTER) || (macroId == ltCHAPTERHEADING))
          fprintf(Contents, "}\\par\\par\\pard\n");

        // From here, just output to chapter
        SetCurrentOutput(Chapters);
      }
     
      if (winHelp)
      {
        fprintf(Chapters, "}\n#{\\footnote %s}\n", topicName);
        fprintf(Chapters, "+{\\footnote %s}\n", GetBrowseString());
        
        OutputSectionKeyword(Chapters);

        GenerateKeywordsForTopic(topicName);
        if (useUpButton)
        {
          // If we're generating a .cnt file, we don't want to be able
          // jump up to the old-style contents page, so disable it.
          if (winHelpContents)
            fprintf(Chapters, "!{\\footnote DisableButton(\"Up\")}\n");
          else
            fprintf(Chapters, "!{\\footnote EnableButton(\"Up\");ChangeButtonBinding(\"Up\", \"JumpId(`%s.hlp', `%s')\")}\n",
               FileNameFromPath(FileRoot), "Contents");
        }
      }

      if (!InPopups())
      {
      char *styleCommand = "";
      if (!winHelp && useHeadingStyles && (macroId == ltCHAPTER || macroId == ltCHAPTERHEADING || macroId == ltCHAPTERHEADINGSTAR))
        styleCommand = "\\s1";
      fprintf(Chapters, "\\pard{%s", ((winHelp && !InPopups()) ? "\\keepn\\sa140\\sb140" : styleCommand));
      WriteHeadingStyle(Chapters, 1);  fprintf(Chapters, " ");
      if (!winHelp)
      {
        if (macroId == ltCHAPTER)
        {
          if (useWord)
//            fprintf(Chapters, "{\\bkmkstart %s}%d{\\bkmkend %s}. ", topicName, chapterNo,
            fprintf(Chapters, "{\\bkmkstart %s}{\\bkmkend %s}", topicName, topicName);
          else
            fprintf(Chapters, "%d. ", chapterNo);
        }
        else if ( useWord )
        {
              fprintf(Chapters, "{\\bkmkstart %s}{\\bkmkend %s}", topicName, topicName);
        }
      }
      OutputCurrentSection();
      TexOutput("\\par\\pard}\\par\n");
      }
      issuedNewParagraph = 2;
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

      char *topicName = FindTopicName(GetNextChunk());
      SetCurrentSectionName(topicName);
      NotifyParentHasChildren(1);
      if (winHelpContents && winHelp && !InPopups())
      {
        OutputCurrentSectionToString(wxBuffer);
        WriteWinHelpContentsFileLine(topicName, wxBuffer, 2);
      }
      AddTexRef(topicName, NULL, SectionNameString, chapterNo, sectionNo);

      if (winHelp)
      {
        SetCurrentOutputs(jumpFrom, Sections);
        // Newline for a new section if this is an article
        if ((DocumentStyle == LATEX_ARTICLE) &&
            ((macroId == ltSECTION) || (macroId == ltSECTIONSTAR) || (macroId == ltSECTIONHEADINGSTAR)))
          fprintf(Sections, "\\page\n");

        if (!InPopups())
          fprintf(jumpFrom, "\n{\\uldb ");
      }
      else
      {
        if (DocumentStyle == LATEX_ARTICLE)
        {
          TexOutput("\\sect\\pgncont\n");
          // If a non-custom page style, we generate the header now.
          if (PageStyle && (strcmp(PageStyle, "plain") == 0 ||
                            strcmp(PageStyle, "empty") == 0 ||
                            strcmp(PageStyle, "headings") == 0))
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
              fprintf(Contents, "\n\\pard{\\tab %d.%d\\tab ", chapterNo, sectionNo);
            else
              fprintf(Contents, "\\par\n\\pard{\\b %d\\tab ", sectionNo);
          }
        }
        else if (macroId == ltSECTIONHEADING)
        {
          if (!InPopups())
          {
            if (DocumentStyle == LATEX_REPORT)
              fprintf(Contents, "\n\\pard{\\tab "); //, chapterNo, sectionNo);
            else
              fprintf(Contents, "\\par\n\\pard{\\b "); //, sectionNo);
          }
        }
        else SetCurrentOutput(NULL);
      } 

      if (startedSections)
      {
        if (winHelp)
          fprintf(Sections, "\\page\n");
      }
      startedSections = TRUE;

      if (winHelp)
        fprintf(Sections, "\n${\\footnote ");

      // Output heading to contents page
      if (!InPopups())
        OutputCurrentSection();

      if (winHelp)
      {
        if (!InPopups())
          fprintf(jumpFrom, "}{\\v %s}\\par\\pard\n", topicName);
      }
      else if ((macroId != ltSECTIONSTAR) && (macroId != ltGLOSS))
      {
        if (DocumentStyle == LATEX_REPORT)
          fprintf(Contents, "}\\par\\pard\n");
        else
          fprintf(Contents, "}\\par\\par\\pard\n");
      }
        
      SetCurrentOutput(winHelp ? Sections : Chapters);

      if (winHelp)
      {
        fprintf(Sections, "}\n#{\\footnote %s}\n", topicName);
        fprintf(Sections, "+{\\footnote %s}\n", GetBrowseString());
        OutputSectionKeyword(Sections);
        GenerateKeywordsForTopic(topicName);
        if (useUpButton)
        {
          if (DocumentStyle == LATEX_ARTICLE)
          {
            fprintf(Sections, "!{\\footnote EnableButton(\"Up\");ChangeButtonBinding(\"Up\", \"JumpId(`%s.hlp', `%s')\")}\n",
               FileNameFromPath(FileRoot), "Contents");
          }
          else if (CurrentChapterName)
          {
            fprintf(Sections, "!{\\footnote EnableButton(\"Up\");ChangeButtonBinding(\"Up\", \"JumpId(`%s.hlp', `%s')\")}\n",
               FileNameFromPath(FileRoot), CurrentChapterName);
          }
        }
      }

      if (!InPopups())
      {
      char *styleCommand = "";
      if (!winHelp && useHeadingStyles && (macroId != ltSECTIONSTAR))
      {
        if (DocumentStyle == LATEX_ARTICLE)
          styleCommand = "\\s1";
        else
          styleCommand = "\\s2";
      }
      char *keep = "";
      if (winHelp && (macroId != ltGLOSS) && !InPopups())
        keep = "\\keepn\\sa140\\sb140";
        
      fprintf(winHelp ? Sections : Chapters, "\\pard{%s%s",
         keep, styleCommand);

      WriteHeadingStyle((winHelp ? Sections : Chapters),
                        (DocumentStyle == LATEX_ARTICLE ? 1 : 2));
      fprintf(winHelp ? Sections : Chapters, " ");

      if (!winHelp)
      {
        if ((macroId != ltSECTIONSTAR) && (macroId != ltSECTIONHEADINGSTAR) && (macroId != ltGLOSS))
        {
          if (DocumentStyle == LATEX_REPORT)
          {
            if (useWord)
//              fprintf(Chapters, "{\\bkmkstart %s}%d.%d{\\bkmkend %s}. ", topicName, chapterNo, sectionNo,
              fprintf(Chapters, "{\\bkmkstart %s}{\\bkmkend %s}", topicName,
                  topicName);
            else
              fprintf(Chapters, "%d.%d. ", chapterNo, sectionNo);
          }
          else
          {
            if (useWord)
//              fprintf(Chapters, "{\\bkmkstart %s}%d{\\bkmkend %s}. ", topicName, sectionNo,
              fprintf(Chapters, "{\\bkmkstart %s}{\\bkmkend %s}", topicName,
                 topicName);
            else
              fprintf(Chapters, "%d. ", sectionNo);
          }
        }
        else if ( useWord )
        {
              fprintf(Chapters, "{\\bkmkstart %s}{\\bkmkend %s}", topicName, topicName);
        }
      }
      OutputCurrentSection();
      TexOutput("\\par\\pard}\\par\n");
      }
      issuedNewParagraph = 2;
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
        OnError("You cannot have a subsection before a section!");
      }
      else
      {
      subsubsectionNo = 0;

      if (macroId != ltSUBSECTIONSTAR)
        subsectionNo ++;

      char *topicName = FindTopicName(GetNextChunk());
      SetCurrentSubsectionName(topicName);
      NotifyParentHasChildren(2);
      if (winHelpContents && winHelp && !InPopups())
      {
        OutputCurrentSectionToString(wxBuffer);
        WriteWinHelpContentsFileLine(topicName, wxBuffer, 3);
      }
      AddTexRef(topicName, NULL, SectionNameString, chapterNo, sectionNo, subsectionNo);

      if (winHelp)
      {
        SetCurrentOutputs(Sections, Subsections);
        SetCurrentOutputs(Sections, Subsections);
        if (!InPopups())
          fprintf(Sections, "\n{\\uldb ");
      }
      else
      {
        if ((macroId != ltSUBSECTIONSTAR) && (macroId != ltMEMBERSECTION) &&
            (macroId != ltFUNCTIONSECTION))
        {
          SetCurrentOutput(Contents);
          if (DocumentStyle == LATEX_REPORT)
            fprintf(Contents, "\n\\pard\\tab\\tab %d.%d.%d\\tab ", chapterNo, sectionNo, subsectionNo);
          else
            fprintf(Contents, "\n\\pard\\tab %d.%d\\tab ", sectionNo, subsectionNo);
        } else SetCurrentOutput(NULL);
      }
      if (startedSections)
      {
        if (winHelp)
        {
          if (!InPopups())
            fprintf(Subsections, "\\page\n");
        }
        else
          fprintf(Chapters, "\\par\n");
      }
      startedSections = TRUE;

      if (winHelp)
        fprintf(Subsections, "\n${\\footnote ");

      // Output to contents page
      if (!InPopups())
        OutputCurrentSection();

      if (winHelp)
      {
        if (!InPopups())
          fprintf(Sections, "}{\\v %s}\\par\\pard\n", topicName);
      }
      else if ((macroId != ltSUBSECTIONSTAR) && (macroId != ltMEMBERSECTION) &&
         (macroId != ltFUNCTIONSECTION))
        fprintf(Contents, "\\par\\pard\n");

      SetCurrentOutput(winHelp ? Subsections : Chapters);
      if (winHelp)
      {
        fprintf(Subsections, "}\n#{\\footnote %s}\n", topicName);
        fprintf(Subsections, "+{\\footnote %s}\n", GetBrowseString());
        OutputSectionKeyword(Subsections);
        GenerateKeywordsForTopic(topicName);
        if (useUpButton && CurrentSectionName)
        {
          fprintf(Subsections, "!{\\footnote EnableButton(\"Up\");ChangeButtonBinding(\"Up\", \"JumpId(`%s.hlp', `%s')\")}\n",
             FileNameFromPath(FileRoot), CurrentSectionName);
        }
      }
      if (!winHelp && indexSubsections && useWord)
      {
        // Insert index entry for this subsection
        TexOutput("{\\xe\\v {");
        OutputCurrentSection();
        TexOutput("}}");
      }

      if (!InPopups())
      {
      char *styleCommand = "";
      if (!winHelp && useHeadingStyles && (macroId != ltSUBSECTIONSTAR))
      {
        if (DocumentStyle == LATEX_ARTICLE)
          styleCommand = "\\s2";
        else
          styleCommand = "\\s3";
      }
      char *keep = "";
      if (winHelp && !InPopups())
        keep = "\\keepn\\sa140\\sb140";

      fprintf(winHelp ? Subsections : Chapters, "\\pard{%s%s",
         keep, styleCommand);

      WriteHeadingStyle((winHelp ? Subsections : Chapters),
                        (DocumentStyle == LATEX_ARTICLE ? 2 : 3));
      fprintf(winHelp ? Subsections : Chapters, " ");

      if (!winHelp)
      {
        if ((macroId != ltSUBSECTIONSTAR) && (macroId != ltMEMBERSECTION) &&
         (macroId != ltFUNCTIONSECTION))
        {
          if (DocumentStyle == LATEX_REPORT)
          {
            if (useWord)
//              fprintf(Chapters, "{\\bkmkstart %s}%d.%d.%d{\\bkmkend %s}. ", topicName, chapterNo, sectionNo, subsectionNo,
              fprintf(Chapters, "{\\bkmkstart %s}{\\bkmkend %s}", topicName,
                       topicName);
            else
              fprintf(Chapters, "%d.%d.%d. ", chapterNo, sectionNo, subsectionNo);
          }
          else
          {
            if (useWord)
//              fprintf(Chapters, "{\\bkmkstart %s}%d.%d{\\bkmkend %s}. ", topicName, sectionNo, subsectionNo,
              fprintf(Chapters, "{\\bkmkstart %s}{\\bkmkend %s}", topicName,
                       topicName);
            else
              fprintf(Chapters, "%d.%d. ", sectionNo, subsectionNo);
          }
        }
        else if ( useWord )
        {
              fprintf(Chapters, "{\\bkmkstart %s}{\\bkmkend %s}", topicName, topicName);
        }
      }
      OutputCurrentSection(); // Repeat section header
      TexOutput("\\par\\pard}\\par\n");
      }
      issuedNewParagraph = 2;
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
        OnError("You cannot have a subsubsection before a subsection!");
      }
      else
      {
      if (macroId != ltSUBSUBSECTIONSTAR)
        subsubsectionNo ++;

      char *topicName = FindTopicName(GetNextChunk());
      SetCurrentTopic(topicName);
      NotifyParentHasChildren(3);
      if (winHelpContents && winHelp)
      {
        OutputCurrentSectionToString(wxBuffer);
        WriteWinHelpContentsFileLine(topicName, wxBuffer, 4);
      }
      AddTexRef(topicName, NULL, SectionNameString, chapterNo, sectionNo, subsectionNo, subsubsectionNo);

      if (winHelp)
      {
        SetCurrentOutputs(Subsections, Subsubsections);
        fprintf(Subsections, "\n{\\uldb ");
      }
      else
      {
        if (macroId != ltSUBSUBSECTIONSTAR)
        {
          if (DocumentStyle == LATEX_ARTICLE)
          {
            SetCurrentOutput(Contents);
            fprintf(Contents, "\n\\tab\\tab %d.%d.%d\\tab ",
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
          fprintf(Subsubsections, "\\page\n");
        else
          fprintf(Chapters, "\\par\n");
      }

      startedSections = TRUE;

      if (winHelp)
        fprintf(Subsubsections, "\n${\\footnote ");

      // Output header to contents page
      OutputCurrentSection();

      if (winHelp)
        fprintf(Subsections, "}{\\v %s}\\par\\pard\n", topicName);
      else if ((DocumentStyle == LATEX_ARTICLE) && (macroId != ltSUBSUBSECTIONSTAR))
        fprintf(Contents, "\\par\\pard\n");
        
      SetCurrentOutput(winHelp ? Subsubsections : Chapters);
      if (winHelp)
      {
        fprintf(Subsubsections, "}\n#{\\footnote %s}\n", topicName);
        fprintf(Subsubsections, "+{\\footnote %s}\n", GetBrowseString());
        OutputSectionKeyword(Subsubsections);
        GenerateKeywordsForTopic(topicName);
        if (useUpButton && CurrentSubsectionName)
        {
          fprintf(Subsubsections, "!{\\footnote EnableButton(\"Up\");ChangeButtonBinding(\"Up\", \"JumpId(`%s.hlp', `%s')\")}\n",
             FileNameFromPath(FileRoot), CurrentSubsectionName);
        }
      }
      if (!winHelp && indexSubsections && useWord)
      {
        // Insert index entry for this subsubsection
        TexOutput("{\\xe\\v {");
        OutputCurrentSection();
        TexOutput("}}");
      }

      char *styleCommand = "";
      if (!winHelp && useHeadingStyles && (macroId != ltSUBSUBSECTIONSTAR))
      {
        if (DocumentStyle == LATEX_ARTICLE)
          styleCommand = "\\s3";
        else
          styleCommand = "\\s4";
      }
      char *keep = "";
      if (winHelp)
        keep = "\\keepn\\sa140\\sb140";

      fprintf(winHelp ? Subsubsections : Chapters, "\\pard{%s%s",
         keep, styleCommand);

      WriteHeadingStyle((winHelp ? Subsubsections : Chapters),
                        (DocumentStyle == LATEX_ARTICLE ? 3 : 4));
      fprintf(winHelp ? Subsubsections : Chapters, " ");
         
      if (!winHelp)
      {
        if ((macroId != ltSUBSUBSECTIONSTAR))
        {
          if (DocumentStyle == LATEX_ARTICLE)
          {
            if (useWord)
//              fprintf(Chapters, "{\\bkmkstart %s}%d.%d.%d{\\bkmkend %s}. ", topicName, sectionNo, subsectionNo, subsubsectionNo,
              fprintf(Chapters, "{\\bkmkstart %s}{\\bkmkend %s}", topicName,
                     topicName);
            else
              fprintf(Chapters, "%d.%d.%d. ", sectionNo, subsectionNo, subsubsectionNo);
          }
          else
          {
            if (useWord)
//              fprintf(Chapters, "{\\bkmkstart %s}%d.%d.%d.%d{\\bkmkend %s}. ", topicName, chapterNo, sectionNo, subsectionNo, subsubsectionNo,
              fprintf(Chapters, "{\\bkmkstart %s}{\\bkmkend %s}", topicName,
                      topicName);
            else
              fprintf(Chapters, "%d.%d.%d.%d. ", chapterNo, sectionNo, subsectionNo, subsubsectionNo);
          }
        }
        else if ( useWord )
        {
              fprintf(Chapters, "{\\bkmkstart %s}{\\bkmkend %s}", topicName, topicName);
        }
      }
      OutputCurrentSection(); // Repeat section header
      TexOutput("\\par\\pard}\\par\n");
      issuedNewParagraph = 2;
//      if (winHelp) TexOutput("\\pard");
    }
    }
    break;
  }
  case ltCAPTION:
  case ltCAPTIONSTAR:
  {
    if (!start)
    {
      char *topicName = FindTopicName(GetNextChunk());
      SetCurrentTopic(topicName);

      TexOutput("\\pard\\par");
      char figBuf[200];

      if (inFigure)
      {
        figureNo ++;

        if (winHelp || !useWord)
        {
          if (DocumentStyle != LATEX_ARTICLE)
            sprintf(figBuf, "%s %d.%d: ", FigureNameString, chapterNo, figureNo);
          else
            sprintf(figBuf, "%s %d: ", FigureNameString, figureNo);
        }
        else
        {
          sprintf(figBuf, "%s {\\field\\flddirty{\\*\\fldinst  SEQ Figure \\\\* ARABIC }{\\fldrslt {\\bkmkstart %s}??{\\bkmkend %s}}}: ",
               FigureNameString, topicName, topicName);
        }
      }
      else
      {
        tableNo ++;

        if (winHelp || !useWord)
        {
          if (DocumentStyle != LATEX_ARTICLE)
            sprintf(figBuf, "%s %d.%d: ", TableNameString, chapterNo, tableNo);
          else
            sprintf(figBuf, "%s %d: ", TableNameString, tableNo);
        }
        else
        {
          sprintf(figBuf, "%s {\\field\\flddirty{\\*\\fldinst  SEQ Table \\\\* ARABIC }{\\fldrslt {\\bkmkstart %s}??{\\bkmkend %s}}}: ",
               TableNameString, topicName, topicName);
        }
      }

      int n = (inTable ? tableNo : figureNo);
      AddTexRef(topicName, NULL, NULL,
           ((DocumentStyle != LATEX_ARTICLE) ? chapterNo : n),
            ((DocumentStyle != LATEX_ARTICLE) ? n : 0));

      if (winHelp)
        TexOutput("\\qc{\\b ");
      else
        TexOutput("\\ql{\\b ");
      TexOutput(figBuf);

      OutputCurrentSection();

      TexOutput("}\\par\\pard\n");
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
      TexOutput("{");
    }
    else
    {
      TexOutput("}\n");
      if (winHelp)
      {
        TexOutput("K{\\footnote {K} ");
        suppressNameDecoration = TRUE;
        TraverseChildrenFromChunk(currentMember);
        suppressNameDecoration = FALSE;
        TexOutput("}\n");
      }
      if (!winHelp && useWord)
      {
        // Insert index entry for this function
        TexOutput("{\\xe\\v {");
        suppressNameDecoration = TRUE;  // Necessary so don't print "(\\bf" etc.
        TraverseChildrenFromChunk(currentMember);
        suppressNameDecoration = FALSE;
        TexOutput("}}");
      }
    }
    break;
  }
  case ltCLIPSFUNC:
  {
//    SetCurrentOutput(winHelp ? Subsections : Chapters);
    if (start)
    {
      TexOutput("{");
    }
    else
    {
      TexOutput("}\n");
      if (winHelp)
      {
        TexOutput("K{\\footnote {K} ");
        suppressNameDecoration = TRUE;  // Necessary so don't print "(\\bf" etc.
        TraverseChildrenFromChunk(currentMember);
        suppressNameDecoration = FALSE;
        TexOutput("}\n");
      }
      if (!winHelp && useWord)
      {
        // Insert index entry for this function
        TexOutput("{\\xe\\v {");
        suppressNameDecoration = TRUE;  // Necessary so don't print "(\\bf" etc.
        TraverseChildrenFromChunk(currentMember);
        suppressNameDecoration = FALSE;
        TexOutput("}}");
      }
    }
    break;
  }
  case ltMEMBER:
  {
//    SetCurrentOutput(winHelp ? Subsections : Chapters);
    if (start)
    {
      TexOutput("{\\b ");
    }
    else
    {
      TexOutput("}\n");
      if (winHelp)
      {
        TexOutput("K{\\footnote {K} ");
        TraverseChildrenFromChunk(currentMember);
        TexOutput("}\n");
      }
      if (!winHelp && useWord)
      {
        // Insert index entry for this function
        TexOutput("{\\xe\\v {");
        suppressNameDecoration = TRUE;  // Necessary so don't print "(\\bf" etc.
        TraverseChildrenFromChunk(currentMember);
        suppressNameDecoration = FALSE;
        TexOutput("}}");
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
        TexOutput("\\par\\pard\\pgnrestart\\sect\\titlepg");

       // In linear RTF, same as chapter headings.
        sprintf(buf, "{\\b\\fs%d %s}\\par\\par\\pard\n\n", chapterFont*2, ContentsNameString);

        TexOutput(buf);
        sprintf(buf, "{\\field{\\*\\fldinst TOC \\\\o \"1-%d\" }{\\fldrslt PRESS F9 TO REFORMAT CONTENTS}}\n", contentsDepth);
        TexOutput(buf);
//        TexOutput("\\sect\\sectd");
      }
      else
      {
        FILE *fd = fopen(ContentsName, "r");
        if (fd)
        {
          int ch = getc(fd);
          while (ch != EOF)
          {
            putc(ch, Chapters);
            ch = getc(fd);
          }
          fclose(fd);
        }
        else
        {
          TexOutput("{\\i RUN TEX2RTF AGAIN FOR CONTENTS PAGE}\\par\n");
          OnInform("Run Tex2RTF again to include contents page.");
        }
      }
    }
    break;
  }
  case ltVOID:
  {
//    if (start)
//      TexOutput("{\\b void}");
    break;
  }
  case ltHARDY:
  {
    if (start)
      TexOutput("{\\scaps HARDY}");
    break;
  }
  case ltWXCLIPS:
  {
    if (start)
      TexOutput("wxCLIPS");
    break;
  }
  case ltSPECIALAMPERSAND:
  {
    if (start)
    {
      if (inTabular)
        TexOutput("\\cell ");
      else
        TexOutput("&");
    }
    break;
  }
  case ltSPECIALTILDE:
  {
    if (start)
    {
      if (inVerbatim)
        TexOutput("~");
      else
        TexOutput(" ");
    }
    break;
  }
  case ltBACKSLASHCHAR:
  {
    if (start)
    {
      if (inTabular)
      {
//        TexOutput("\\cell\\row\\trowd\\trgaph108\\trleft-108\n");
        TexOutput("\\cell\\row\\trowd\\trgaph108\n");
        int currentWidth = 0;
        for (int i = 0; i < noColumns; i++)
        {
          currentWidth += TableData[i].width;
          if (TableData[i].rightBorder)
            TexOutput("\\clbrdrr\\brdrs\\brdrw15");

          if (TableData[i].leftBorder)
            TexOutput("\\clbrdrl\\brdrs\\brdrw15");
          
          sprintf(buf, "\\cellx%d", currentWidth);
          TexOutput(buf);
        }
        TexOutput("\\pard\\intbl\n");
      }
      else
        TexOutput("\\line\n");
    }
    break;
  }
  case ltRANGLEBRA:
  {
    if (start)
      TexOutput("\tab ");
    break;
  }
  case ltRTFSP:  // Explicit space, RTF only
  {
    if (start)
      TexOutput(" ");
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
        TexOutput("\\par\\par\n");
        issuedNewParagraph = 2;
      }
      else
      {
        // Top-level list: issue a new paragraph if we haven't
        // just done so
        if (!issuedNewParagraph)
        {
          TexOutput("\\par\\pard");
          WriteEnvironmentStyles();
          issuedNewParagraph = 1;
        }
        else issuedNewParagraph = 0;
      }
      indentLevel ++;
      TexOutput("\\fi0\n");
      int listType;
      if (macroId == ltENUMERATE)
        listType = LATEX_ENUMERATE;
      else if (macroId == ltITEMIZE)
        listType = LATEX_ITEMIZE;
      else
        listType = LATEX_DESCRIPTION;

      int oldIndent = 0;
      wxNode *node = itemizeStack.First();
      if (node)
        oldIndent = ((ItemizeStruc *)node->Data())->indentation;

      int indentSize1 = oldIndent + 20*labelIndentTab;
      int indentSize2 = oldIndent + 20*itemIndentTab;

      ItemizeStruc *struc = new ItemizeStruc(listType, indentSize2, indentSize1);
      itemizeStack.Insert(struc);
      
      sprintf(buf, "\\tx%d\\tx%d\\li%d", indentSize1, indentSize2, indentSize2);
      PushEnvironmentStyle(buf);
    }
    else
    {
      currentItemSep = 8; // Reset to the default
      indentLevel --;
      PopEnvironmentStyle();

      if (itemizeStack.First())
      {
        ItemizeStruc *struc = (ItemizeStruc *)itemizeStack.First()->Data();
        delete struc;
        delete itemizeStack.First();
      }
/* Change 18/7/97 - don't know why we wish to do this
      if (itemizeStack.Number() == 0)
      {
        OnMacro(ltPAR, 0, TRUE);
        OnMacro(ltPAR, 0, FALSE);
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
      wxNode *node = itemizeStack.First();
      if (node)
        oldIndent = ((ItemizeStruc *)node->Data())->indentation;

      int indentSize = oldIndent + TwoColWidthA;

      ItemizeStruc *struc = new ItemizeStruc(LATEX_TWOCOL, indentSize);
      itemizeStack.Insert(struc);
      
//      sprintf(buf, "\\tx%d\\li%d\\ri%d", indentSize, indentSize, TwoColWidthA+TwoColWidthB+oldIndent);
      sprintf(buf, "\\tx%d\\li%d", indentSize, indentSize);
      PushEnvironmentStyle(buf);
    }
    else
    {
      indentLevel --;
      PopEnvironmentStyle();
      if (itemizeStack.First())
      {
        ItemizeStruc *struc = (ItemizeStruc *)itemizeStack.First()->Data();
        delete struc;
        delete itemizeStack.First();
      }
/*
      // JACS June 1997
      TexOutput("\\pard\n");
      WriteEnvironmentStyles();
*/
/* why do we need this? */
      if (itemizeStack.Number() == 0)
      {
        issuedNewParagraph = 0;
        OnMacro(ltPAR, 0, TRUE);
        OnMacro(ltPAR, 0, FALSE);
      }
    }
    break;
  }
  case ltITEM:
  {
    wxNode *node = itemizeStack.First();
    if (node)
    {
      ItemizeStruc *struc = (ItemizeStruc *)node->Data();
      if (!start)
      {
        struc->currentItem += 1;
        char indentBuf[60];

        int indentSize1 = struc->labelIndentation;
        int indentSize2 = struc->indentation;

        TexOutput("\n");
        if (struc->currentItem > 1)
        {
          if (currentItemSep > 0)
            TexOutput("\\par");

          TexOutput("\\par");
//          WriteEnvironmentStyles();
        }

        sprintf(buf, "\\tx%d\\tx%d\\li%d\\fi-%d\n", indentSize1, indentSize2,
                  indentSize2, 20*itemIndentTab);
        TexOutput(buf);

        switch (struc->listType)
        {
          case LATEX_ENUMERATE:
          {
            if (descriptionItemArg)
            {
              TexOutput("\\tab{ ");
              TraverseChildrenFromChunk(descriptionItemArg);
              TexOutput("}\\tab");
              descriptionItemArg = NULL;
	    }
	    else
	    {
              sprintf(indentBuf, "\\tab{\\b %d.}\\tab", struc->currentItem);
              TexOutput(indentBuf);
            }
            break;
          }
          case LATEX_ITEMIZE:
          {
            if (descriptionItemArg)
            {
              TexOutput("\\tab{ ");
              TraverseChildrenFromChunk(descriptionItemArg);
              TexOutput("}\\tab");
              descriptionItemArg = NULL;
	    }
	    else
	    {
              if (bulletFile && winHelp)
              {
                if (winHelpVersion > 3) // Transparent bitmap
                  sprintf(indentBuf, "\\tab\\{bmct %s\\}\\tab", bulletFile);
                else
                  sprintf(indentBuf, "\\tab\\{bmc %s\\}\\tab", bulletFile);
              }
              else if (winHelp)
                sprintf(indentBuf, "\\tab{\\b o}\\tab");
              else
                sprintf(indentBuf, "\\tab{\\f1\\'b7}\\tab");
              TexOutput(indentBuf);
            }
            break;
          }
          default:
          case LATEX_DESCRIPTION:
          {
            if (descriptionItemArg)
            {
              TexOutput("\\tab{\\b ");
              TraverseChildrenFromChunk(descriptionItemArg);
              TexOutput("}  ");
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
    wxNode *node = itemizeStack.First();
    if (node)
    {
      ItemizeStruc *struc = (ItemizeStruc *)node->Data();
      if (start)
      {
        struc->currentItem += 1;

        int indentSize = struc->indentation;
        int oldIndent = 0;
        wxNode *node2 = NULL;
        if (itemizeStack.Number() > 1) // TODO: do I actually mean Nth(0) here??
            node2 = itemizeStack.Nth(1);
        if (node2)
          oldIndent = ((ItemizeStruc *)node2->Data())->indentation;

        TexOutput("\n");
        if (struc->currentItem > 1)
        {
          if (currentItemSep > 0)
            TexOutput("\\par");

//          WriteEnvironmentStyles();
        }

//        sprintf(buf, "\\tx%d\\li%d\\fi-%d\\ri%d\n", TwoColWidthA,
//             TwoColWidthA, TwoColWidthA, TwoColWidthA+TwoColWidthB+oldIndent);
/*
        sprintf(buf, "\\tx%d\\li%d\\fi-%d\n", TwoColWidthA,
             TwoColWidthA, TwoColWidthA);
*/
        sprintf(buf, "\\tx%d\\li%d\\fi-%d\n", TwoColWidthA + oldIndent,
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
      if (macroId == ltVERBATIM)
      {
        if (!issuedNewParagraph)
        {
          TexOutput("\\par\\pard");
          WriteEnvironmentStyles();
          issuedNewParagraph = 1;
        }
        else issuedNewParagraph = 0;
      }
      sprintf(buf, "{\\f3\\fs20 ");
      TexOutput(buf);
    }
    else
    {
      TexOutput("}");
      if (macroId == ltVERBATIM)
      {
        TexOutput("\\pard\n");
//        issuedNewParagraph = 1;
        WriteEnvironmentStyles();
      }
    }
    break;
  }
  case ltCENTERLINE:
  case ltCENTER:
  {
    if (start)
    {
      TexOutput("\\fi0\\qc ");
      forbidParindent ++;
      PushEnvironmentStyle("\\qc");
    }
    else
    {
      TexOutput("\\par\\pard\n");
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
      TexOutput("\\fi0\\ql ");
      forbidParindent ++;
      PushEnvironmentStyle("\\ql");
    }
    else
    {
      TexOutput("\\par\\pard\n");
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
      TexOutput("\\fi0\\qr ");
      forbidParindent ++;
      PushEnvironmentStyle("\\qr");
    }
    else
    {
      TexOutput("\\par\\pard\n");
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
      sprintf(buf, "{\\fs%d\n", smallFont*2);
      TexOutput(buf);
    }
    else TexOutput("}\n");
    break;
  }
  case ltTINY:
  case ltSCRIPTSIZE:
  {
    if (start)
    {
      sprintf(buf, "{\\fs%d\n", tinyFont*2);
      TexOutput(buf);
    }
    else TexOutput("}\n");
    break;
  }
  case ltNORMALSIZE:
  {
    if (start)
    {
      sprintf(buf, "{\\fs%d\n", normalFont*2);
      TexOutput(buf);
    }
    else TexOutput("}\n");
    break;
  }
  case ltlarge:
  {
    if (start)
    {
      sprintf(buf, "{\\fs%d\n", largeFont1*2);
      TexOutput(buf);
    }
    else TexOutput("}\n");
    break;
  }
  case ltLarge:
  {
    if (start)
    {
      sprintf(buf, "{\\fs%d\n", LargeFont2*2);
      TexOutput(buf);
    }
    else TexOutput("}\n");
    break;
  }
  case ltLARGE:
  {
    if (start)
    {
      sprintf(buf, "{\\fs%d\n", LARGEFont3*2);
      TexOutput(buf);
    }
    else TexOutput("}\n");
    break;
  }
  case lthuge:
  {
    if (start)
    {
      sprintf(buf, "{\\fs%d\n", hugeFont1*2);
      TexOutput(buf);
    }
    else TexOutput("}\n");
    break;
  }
  case ltHuge:
  {
    if (start)
    {
      sprintf(buf, "{\\fs%d\n", HugeFont2*2);
      TexOutput(buf);
    }
    else TexOutput("}\n");
    break;
  }
  case ltHUGE:
  {
    if (start)
    {
      sprintf(buf, "{\\fs%d\n", HUGEFont3*2);
      TexOutput(buf);
    }
    else TexOutput("}\n");
    break;
  }
  case ltTEXTBF:
  case ltBFSERIES:
  case ltBF:
  {
    if (start)
    {
      TexOutput("{\\b ");
    }
    else TexOutput("}");
    break;
  }
  case ltUNDERLINE:
  {
    if (start)
    {
      TexOutput("{\\ul ");
    }
    else TexOutput("}");
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
      TexOutput("{\\i ");
    }
    else TexOutput("}");
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
      TexOutput("{\\plain ");
    }
    else TexOutput("}");
 */
    break;
  }
  // Medium-weight font. Unbolden...
  case ltMDSERIES:
  {
    if (start)
    {
      TexOutput("{\\b0 ");
    }
    else TexOutput("}");
    break;
  }
  // Upright (un-italic or slant)
  case ltUPSHAPE:
  {
    if (start)
    {
      TexOutput("{\\i0 ");
    }
    else TexOutput("}");
    break;
  }
  case ltTEXTSC:
  case ltSCSHAPE:
  case ltSC:
  {
    if (start)
    {
      TexOutput("{\\scaps ");
    }
    else TexOutput("}");
    break;
  }
  case ltTEXTTT:
  case ltTTFAMILY:
  case ltTT:
  {
    if (start)
    {
      TexOutput("{\\f3 ");
    }
    else TexOutput("}");
    break;
  }
  case ltLBRACE:
  {
    if (start)
      TexOutput("\\{");
    break;
  }
  case ltRBRACE:
  {
    if (start)
      TexOutput("\\}");
    break;
  }
  case ltBACKSLASH:
  {
    if (start)
      TexOutput("\\\\");
    break;
  }
  case ltPAR:
  {
    if (start)
    {
		if ( issuedNewParagraph == 0 )
		{
          TexOutput("\\par\\pard");
          issuedNewParagraph ++;

          // Extra par if parskip is more than zero (usually looks best.)
          if (!inTabular && (ParSkip > 0))
		  {
            TexOutput("\\par");
            issuedNewParagraph ++;
		  }
          WriteEnvironmentStyles();
		}
		// 1 is a whole paragraph if ParSkip == 0,
		// half a paragraph if ParSkip > 0
		else if ( issuedNewParagraph == 1 )
		{
		  // Don't need a par at all if we've already had one,
		  // and ParSkip == 0.

          // Extra par if parskip is more than zero (usually looks best.)
          if (!inTabular && (ParSkip > 0))
		  {
            TexOutput("\\par");
            issuedNewParagraph ++;
		  }
          WriteEnvironmentStyles();
		}
/*
      if (!issuedNewParagraph || (issuedNewParagraph > 1))
      {
        TexOutput("\\par\\pard");

        // Extra par if parskip is more than zero (usually looks best.)
        if (!inTabular && (ParSkip > 0))
          TexOutput("\\par");
        WriteEnvironmentStyles();
      }
*/

      TexOutput("\n");
    }
    break;
  }
  case ltNEWPAGE:
  {
    // In Windows Help, no newpages until we've started some chapters or sections
    if (!(winHelp && !startedSections))
      if (start)
        TexOutput("\\page\n");
    break;
  }
  case ltMAKETITLE:
  {
    if (start && DocumentTitle)
    {
      TexOutput("\\par\\pard");
      if (!winHelp)
        TexOutput("\\par");
      sprintf(buf, "\\qc{\\fs%d\\b ", titleFont*2);
      TexOutput(buf);
      TraverseChildrenFromChunk(DocumentTitle);
      TexOutput("}\\par\\pard\n");

      if (DocumentAuthor)
      {
        if (!winHelp)
          TexOutput("\\par");
        sprintf(buf, "\\par\\qc{\\fs%d ", authorFont*2);
        TexOutput(buf);
        TraverseChildrenFromChunk(DocumentAuthor);
        TexOutput("}");
        TexOutput("\\par\\pard\n");
      }
      if (DocumentDate)
      {
        TexOutput("\\par");
        sprintf(buf, "\\qc{\\fs%d ", authorFont*2);
        TexOutput(buf);
        TraverseChildrenFromChunk(DocumentDate);
        TexOutput("}\\par\\pard\n");
      }
      // If linear RTF, we want this titlepage to be in a separate
      // section with its own (blank) header and footer
      if (!winHelp && (DocumentStyle != LATEX_ARTICLE))
      {
        TexOutput("{\\header }{\\footer }\n");
        // Not sure about this: we get too many sections.
//        TexOutput("\\sect");
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
      if (strcmp(contentsLineSection, "chapter") == 0)
      {
        fprintf(Contents, "\\par\n{\\b %s}\\par\n", contentsLineValue);
      }
      else if (strcmp(contentsLineSection, "section") == 0)
      {
        if (DocumentStyle != LATEX_ARTICLE)
          fprintf(Contents, "\n\\tab%s\\par\n", contentsLineValue);
        else
          fprintf(Contents, "\\par\n{\\b %s}\\par\n", contentsLineValue);
      }
    }
    }
    break;
  }
  case ltHRULE:
  {
    if (start)
    {
      TexOutput("\\brdrb\\brdrs\\par\\pard\n");
      issuedNewParagraph = 1;
      WriteEnvironmentStyles();
    }
    break;
  }
  case ltRULE:
  {
    if (start)
    {
      TexOutput("\\brdrb\\brdrs\\par\\pard\n");
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
      TexOutput("\\li260\\fi-260 "); // Indent from 2nd line
    else
      TexOutput("\\par\\pard\\par\n\n");
    break;
  }
  case ltTHEPAGE:
  {
    if (start)
    {
      TexOutput("{\\field{\\*\\fldinst PAGE \\\\* MERGEFORMAT }{\\fldrslt 1}}");
    }
    break;
  }
  case ltTHECHAPTER:
  {
    if (start)
    {
//      TexOutput("{\\field{\\*\\fldinst SECTION \\\\* MERGEFORMAT }{\\fldrslt 1}}");
      sprintf(buf, "%d", chapterNo);
      TexOutput(buf);
    }
    break;
  }
  case ltTHESECTION:
  {
    if (start)
    {
//      TexOutput("{\\field{\\*\\fldinst SECTION \\\\* MERGEFORMAT }{\\fldrslt 1}}");
      sprintf(buf, "%d", sectionNo);
      TexOutput(buf);
    }
    break;
  }
  case ltTWOCOLUMN:
  {
    if (!start && !winHelp)
    {
      TexOutput("\\cols2\n");
    }
    break;
  }
  case ltONECOLUMN:
  {
    if (!start && !winHelp)
    {
      TexOutput("\\cols1\n");
    }
    break;
  }
  case ltPRINTINDEX:
  {
    if (start && useWord && !winHelp)
    {
      FakeCurrentSection("Index");
      OnMacro(ltPAR, 0, TRUE);
      OnMacro(ltPAR, 0, FALSE);
      TexOutput("\\par{\\field{\\*\\fldinst INDEX \\\\h \"\\emdash A\\emdash \"\\\\c \"2\"}{\\fldrslt PRESS F9 TO REFORMAT INDEX}}\n");
    }
    break;
  }
  case ltLISTOFFIGURES:
  {
    if (start && useWord && !winHelp)
    {
      FakeCurrentSection(FiguresNameString, FALSE);
      OnMacro(ltPAR, 0, TRUE);
      OnMacro(ltPAR, 0, FALSE);
      OnMacro(ltPAR, 0, TRUE);
      OnMacro(ltPAR, 0, FALSE);
      char buf[200];
      sprintf(buf, "{\\field\\fldedit{\\*\\fldinst  TOC \\\\c \"%s\" }{\\fldrslt PRESS F9 TO REFORMAT LIST OF FIGURES}}\n",
               FigureNameString);
      TexOutput(buf);
    }
    break;
  }
  case ltLISTOFTABLES:
  {
    if (start && useWord && !winHelp)
    {
      FakeCurrentSection(TablesNameString, FALSE);
      OnMacro(ltPAR, 0, TRUE);
      OnMacro(ltPAR, 0, FALSE);
      OnMacro(ltPAR, 0, TRUE);
      OnMacro(ltPAR, 0, FALSE);
      char buf[200];
      sprintf(buf, "{\\field\\fldedit{\\*\\fldinst  TOC \\\\c \"%s\" }{\\fldrslt PRESS F9 TO REFORMAT LIST OF TABLES}}\n",
                TablesNameString);
      TexOutput(buf);
    }
    break;
  }
  // Symbols
  case ltALPHA:
    if (start) TexOutput("{\\f1\\'61}");
    break;
  case ltBETA:
    if (start) TexOutput("{\\f1\\'62}");
    break;
  case ltGAMMA:
    if (start) TexOutput("{\\f1\\'63}");
    break;
  case ltDELTA:
    if (start) TexOutput("{\\f1\\'64}");
    break;
  case ltEPSILON:
  case ltVAREPSILON:
    if (start) TexOutput("{\\f1\\'65}");
    break;
  case ltZETA:
    if (start) TexOutput("{\\f1\\'7A}");
    break;
  case ltETA:
    if (start) TexOutput("{\\f1\\'68}");
    break;
  case ltTHETA:
  case ltVARTHETA:
    if (start) TexOutput("{\\f1\\'71}");
    break;
  case ltIOTA:
    if (start) TexOutput("{\\f1\\'69}");
    break;
  case ltKAPPA:
    if (start) TexOutput("{\\f1\\'6B}");
    break;
  case ltLAMBDA:
    if (start) TexOutput("{\\f1\\'6C}");
    break;
  case ltMU:
    if (start) TexOutput("{\\f1\\'6D}");
    break;
  case ltNU:
    if (start) TexOutput("{\\f1\\'6E}");
    break;
  case ltXI:
    if (start) TexOutput("{\\f1\\'78}");
    break;
  case ltPI:
    if (start) TexOutput("{\\f1\\'70}");
    break;
  case ltVARPI:
    if (start) TexOutput("{\\f1\\'76}");
    break;
  case ltRHO:
  case ltVARRHO:
    if (start) TexOutput("{\\f1\\'72}");
    break;
  case ltSIGMA:
    if (start) TexOutput("{\\f1\\'73}");
    break;
  case ltVARSIGMA:
    if (start) TexOutput("{\\f1\\'56}");
    break;
  case ltTAU:
    if (start) TexOutput("{\\f1\\'74}");
    break;
  case ltUPSILON:
    if (start) TexOutput("{\\f1\\'75}");
    break;
  case ltPHI:
  case ltVARPHI:
    if (start) TexOutput("{\\f1\\'66}");
    break;
  case ltCHI:
    if (start) TexOutput("{\\f1\\'63}");
    break;
  case ltPSI:
    if (start) TexOutput("{\\f1\\'79}");
    break;
  case ltOMEGA:
    if (start) TexOutput("{\\f1\\'77}");
    break;
  case ltCAP_GAMMA:
    if (start) TexOutput("{\\f1\\'47}");
    break;
  case ltCAP_DELTA:
    if (start) TexOutput("{\\f1\\'44}");
    break;
  case ltCAP_THETA:
    if (start) TexOutput("{\\f1\\'51}");
    break;
  case ltCAP_LAMBDA:
    if (start) TexOutput("{\\f1\\'4C}");
    break;
  case ltCAP_XI:
    if (start) TexOutput("{\\f1\\'58}");
    break;
  case ltCAP_PI:
    if (start) TexOutput("{\\f1\\'50}");
    break;
  case ltCAP_SIGMA:
    if (start) TexOutput("{\\f1\\'53}");
    break;
  case ltCAP_UPSILON:
    if (start) TexOutput("{\\f1\\'54}");
    break;
  case ltCAP_PHI:
    if (start) TexOutput("{\\f1\\'46}");
    break;
  case ltCAP_PSI:
    if (start) TexOutput("{\\f1\\'59}");
    break;
  case ltCAP_OMEGA:
    if (start) TexOutput("{\\f1\\'57}");
    break;
  // Binary operation symbols
  case ltLE:
  case ltLEQ:
    if (start) TexOutput("{\\f1\\'A3}");
    break;
  case ltLL:
    if (start) TexOutput("<<");
    break;
  case ltSUBSET:
    if (start) TexOutput("{\\f1\\'CC}");
    break;
  case ltSUBSETEQ:
    if (start) TexOutput("{\\f1\\'CD}");
    break;
  case ltIN:
    if (start) TexOutput("{\\f1\\'CE}");
    break;
  case ltGE:
  case ltGEQ:
    if (start) TexOutput("{\\f1\\'B3}");
    break;
  case ltGG:
    if (start) TexOutput(">>");
    break;
  case ltSUPSET:
    if (start) TexOutput("{\\f1\\'C9}");
    break;
  case ltSUPSETEQ:
    if (start) TexOutput("{\\f1\\'CD}");
    break;
  case ltNI:
    if (start) TexOutput("{\\f1\\'27}");
    break;
  case ltPERP:
    if (start) TexOutput("{\\f1\\'5E}");
    break;
  case ltNEQ:
    if (start) TexOutput("{\\f1\\'B9}");
    break;
  case ltAPPROX:
    if (start) TexOutput("{\\f1\\'BB}");
    break;
  case ltCONG:
    if (start) TexOutput("{\\f1\\'40}");
    break;
  case ltEQUIV:
    if (start) TexOutput("{\\f1\\'BA}");
    break;
  case ltPROPTO:
    if (start) TexOutput("{\\f1\\'B5}");
    break;
  case ltSIM:
    if (start) TexOutput("{\\f1\\'7E}");
    break;
  case ltSMILE:
    if (start) TexOutput("{\\f4\\'4A}");
    break;
  case ltFROWN:
    if (start) TexOutput("{\\f4\\'4C}");
    break;
  case ltMID:
    if (start) TexOutput("|");
    break;

  // Negated relation symbols
  case ltNOTEQ:
    if (start) TexOutput("{\\f1\\'B9}");
    break;
  case ltNOTIN:
    if (start) TexOutput("{\\f1\\'CF}");
    break;
  case ltNOTSUBSET:
    if (start) TexOutput("{\\f1\\'CB}");
    break;

  // Arrows
  case ltLEFTARROW:
     if (start) TexOutput("{\\f1\\'AC}");
    break;
  case ltLEFTARROW2:
    if (start) TexOutput("{\\f1\\'DC}");
    break;
  case ltRIGHTARROW:
    if (start) TexOutput("{\\f1\\'AE}");
    break;
  case ltRIGHTARROW2:
    if (start) TexOutput("{\\f1\\'DE}");
    break;
  case ltLEFTRIGHTARROW:
    if (start) TexOutput("{\\f1\\'AB}");
    break;
  case ltLEFTRIGHTARROW2:
    if (start) TexOutput("{\\f1\\'DB}");
    break;
  case ltUPARROW:
    if (start) TexOutput("{\\f1\\'AD}");
    break;
  case ltUPARROW2:
    if (start) TexOutput("{\\f1\\'DD}");
    break;
  case ltDOWNARROW:
    if (start) TexOutput("{\\f1\\'AF}");
    break;
  case ltDOWNARROW2:
    if (start) TexOutput("{\\f1\\'DF}");
    break;

  // Miscellaneous symbols
  case ltALEPH:
    if (start) TexOutput("{\\f1\\'CO}");
    break;
  case ltWP:
    if (start) TexOutput("{\\f1\\'C3}");
    break;
  case ltRE:
    if (start) TexOutput("{\\f1\\'C2}");
    break;
  case ltIM:
    if (start) TexOutput("{\\f1\\'C1}");
    break;
  case ltEMPTYSET:
    if (start) TexOutput("{\\f1\\'C6}");
    break;
  case ltNABLA:
    if (start) TexOutput("{\\f1\\'D1}");
    break;
  case ltSURD:
    if (start) TexOutput("{\\f1\\'D6}");
    break;
  case ltPARTIAL:
    if (start) TexOutput("{\\f1\\'B6}");
    break;
  case ltBOT:
    if (start) TexOutput("{\\f1\\'5E}");
    break;
  case ltFORALL:
    if (start) TexOutput("{\\f1\\'22}");
    break;
  case ltEXISTS:
    if (start) TexOutput("{\\f1\\'24}");
    break;
  case ltNEG:
    if (start) TexOutput("{\\f1\\'D8}");
    break;
  case ltSHARP:
    if (start) TexOutput("{\\f1\\'23}");
    break;
  case ltANGLE:
    if (start) TexOutput("{\\f1\\'D0}");
    break;
  case ltTRIANGLE:
    if (start) TexOutput("{\\f5\\'73}");
    break;
  case ltCLUBSUIT:
    if (start) TexOutput("{\\f5\\'A8}");
    break;
  case ltDIAMONDSUIT:
    if (start) TexOutput("{\\f5\\'A9}");
    break;
  case ltHEARTSUIT:
    if (start) TexOutput("{\\f5\\'AA}");
    break;
  case ltSPADESUIT:
    if (start) TexOutput("{\\f5\\'AB}");
    break;
  case ltINFTY:
    if (start) TexOutput("{\\f1\\'A5}");
    break;
  case ltCOPYRIGHT:
    if (start) TexOutput("{\\f0\\'A9}");
    break;
  case ltREGISTERED:
    if (start) TexOutput("{\\f0\\'AE}");
    break;
  case ltPM:
    if (start) TexOutput("{\\f1\\'B1}");
    break;
  case ltMP:
    if (start) TexOutput("{\\f1\\'B1}");
    break;
  case ltTIMES:
    if (start) TexOutput("{\\f1\\'B4}");
    break;
  case ltDIV:
    if (start) TexOutput("{\\f1\\'B8}");
    break;
  case ltCDOT:
    if (start) TexOutput("{\\f1\\'D7}");
    break;
  case ltAST:
    if (start) TexOutput("{\\f1\\'2A}");
    break;
  case ltSTAR:
    if (start) TexOutput("{\\f5\\'AB}");
    break;
  case ltCAP:
    if (start) TexOutput("{\\f1\\'C7}");
    break;
  case ltCUP:
    if (start) TexOutput("{\\f1\\'C8}");
    break;
  case ltVEE:
    if (start) TexOutput("{\\f1\\'DA}");
    break;
  case ltWEDGE:
    if (start) TexOutput("{\\f1\\'D9}");
    break;
  case ltCIRC:
    if (start) TexOutput("{\\f1\\'B0}");
    break;
  case ltBULLET:
    if (start) TexOutput("{\\f1\\'B7}");
    break;
  case ltDIAMOND:
    if (start) TexOutput("{\\f1\\'E0}");
    break;
  case ltBOX:
    if (start) TexOutput("{\\f1\\'C6}");
    break;
  case ltDIAMOND2:
    if (start) TexOutput("{\\f1\\'E0}");
    break;
  case ltBIGTRIANGLEDOWN:
    if (start) TexOutput("{\\f1\\'D1}");
    break;
  case ltOPLUS:
    if (start) TexOutput("{\\f1\\'C5}");
    break;
  case ltOTIMES:
    if (start) TexOutput("{\\f1\\'C4}");
    break;
  case ltSS:
    if (start) TexOutput("{\\'DF}");
    break;
  case ltFIGURE:
  {
    if (start) inFigure = TRUE;
    else inFigure = FALSE;
    break;
  }
  case ltTABLE:
  {
    if (start) inTable = TRUE;
    else inTable = FALSE;
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
  char buf[300];
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
    return FALSE;
    break;
  }
  case ltFUNC:
  {
    if (start && (arg_no == 1))
      TexOutput("\\pard\\li600\\fi-600{\\b ");

    if (!start && (arg_no == 1))
      TexOutput("} ");

    if (start && (arg_no == 2))
    {
      if (!suppressNameDecoration) TexOutput("{\\b ");
      currentMember = GetArgChunk();
    }
    if (!start && (arg_no == 2))
    {
      if (!suppressNameDecoration) TexOutput("}");
    }
    
    if (start && (arg_no == 3))
      TexOutput("(");
    if (!start && (arg_no == 3))
    {
//      TexOutput(")\\li0\\fi0");
//      TexOutput(")\\par\\pard\\li0\\fi0");
//      issuedNewParagraph = 1;
      TexOutput(")");
      WriteEnvironmentStyles();
    }
    break;
  }
  case ltCLIPSFUNC:
  {
    if (start && (arg_no == 1))
      TexOutput("\\pard\\li260\\fi-260{\\b ");
    if (!start && (arg_no == 1))
      TexOutput("} ");

    if (start && (arg_no == 2))
    {
      if (!suppressNameDecoration) TexOutput("({\\b ");
      currentMember = GetArgChunk();
    }
    if (!start && (arg_no == 2))
    {
      if (!suppressNameDecoration) TexOutput("}");
    }

    if (!start && (arg_no == 3))
    {
      TexOutput(")\\li0\\fi0");
      WriteEnvironmentStyles();
    }
    break;
  }
  case ltPFUNC:
  {
    if (start && (arg_no == 1))
      TexOutput("\\pard\\li260\\fi-260");

    if (!start && (arg_no == 1))
      TexOutput(" ");

    if (start && (arg_no == 2))
      TexOutput("(*");
    if (!start && (arg_no == 2))
      TexOutput(")");

    if (start && (arg_no == 2))
      currentMember = GetArgChunk();

    if (start && (arg_no == 3))
      TexOutput("(");
    if (!start && (arg_no == 3))
    {
      TexOutput(")\\li0\\fi0");
      WriteEnvironmentStyles();
    }
    break;
  }
  case ltPARAM:
  {
    if (start && (arg_no == 1))
      TexOutput("{\\b ");
    if (!start && (arg_no == 1))
      TexOutput("}");
    if (start && (arg_no == 2))
    {
      TexOutput("{\\i ");
    }
    if (!start && (arg_no == 2))
    {
      TexOutput("}");
    }
    break;
  }
  case ltCPARAM:
  {
    if (start && (arg_no == 1))
      TexOutput("{\\b ");
    if (!start && (arg_no == 1))
      TexOutput("} ");  // This is the difference from param - one space!
    if (start && (arg_no == 2))
    {
      TexOutput("{\\i ");
    }
    if (!start && (arg_no == 2))
    {
      TexOutput("}");
    }
    break;
  }
  case ltMEMBER:
  {
    if (!start && (arg_no == 1))
      TexOutput(" ");

    if (start && (arg_no == 2))
      currentMember = GetArgChunk();
    break;
  }
  case ltREF:
  {
    if (start)
    {
      char *sec = NULL;
      char *secName = NULL;
      
      char *refName = GetArgData();
      if (winHelp || !useWord)
      {
        if (refName)
        {
          TexRef *texRef = FindReference(refName);
          if (texRef)
          {
            sec = texRef->sectionNumber;
            secName = texRef->sectionName;
          }
        }
        if (sec)
        {
          TexOutput(sec);
        }
      }
      else
      {
        fprintf(Chapters, "{\\field{\\*\\fldinst  REF %s \\\\* MERGEFORMAT }{\\fldrslt ??}}",
                refName);
      }
      return FALSE;
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
            TexOutput("{\\uldb ");
          else
            TexOutput("}");
        }
        if ((GetNoArgs() - arg_no) == 0) // Arg = 2, or 3 if first is optional
        {
          if (start)
          {
            TexOutput("{\\v ");

            // Remove green colour/underlining if specified
            if (!hotSpotUnderline && !hotSpotColour)
              TexOutput("%");
            else if (!hotSpotColour)
              TexOutput("*");
          }
          else TexOutput("}");
        }
    }
    else // If a linear document, must resolve the references ourselves
    {
      if ((GetNoArgs() - arg_no) == 1)
      {
        // In a linear document we display the anchor text in italic plus
        // the page number.
        if (start)
          TexOutput("{\\i ");
        else
          TexOutput("}");
        return TRUE;
      }
      else if ((GetNoArgs() - arg_no) == 0) // Arg = 2, or 3 if first is optional
      {
        if (macroId != ltHELPREFN)
        {
          if (start)
          {
            TexOutput(" (");
            char *refName = GetArgData();
            if (refName)
            {
                if (useWord)
                {
                    char *s = GetArgData();
                    TexOutput("p. ");
                    TexOutput("{\\field{\\*\\fldinst  PAGEREF ");
                    TexOutput(refName);
                    TexOutput(" \\\\* MERGEFORMAT }{\\fldrslt ??}}");
                }
                else
                {
                  // Only print section name if we're not in Word mode,
                  // so can't do page references
                  TexRef *texRef = FindReference(refName);
                  if (texRef)
                  {
                    TexOutput(texRef->sectionName) ; TexOutput(" "); TexOutput(texRef->sectionNumber);
                  }
                  else
                  {
                    TexOutput("??");
                    sprintf(buf, "Warning: unresolved reference %s.", refName);
                    OnInform(buf);
                  }
                }
            }
            else TexOutput("??");
          }
          else TexOutput(")");
        }
        return FALSE;
      }
    }
    break;
  }
  case ltURLREF:
  {
    if (arg_no == 1)
    {
      return TRUE;
    }
    else if (arg_no == 2)
    {
      if (start)
      {
        inVerbatim = TRUE;
        TexOutput(" ({\\f3 ");
      }
      else
      {
        TexOutput("})");
        inVerbatim = FALSE;
      }
      return TRUE;
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
          TexOutput("{\\ul ");
        else
          TexOutput("}");
      }
      if ((GetNoArgs() - arg_no) == 0) // Arg = 2, or 3 if first is optional
      {
        if (start)
        {
          TexOutput("{\\v ");

          // Remove green colour/underlining if specified
          if (!hotSpotUnderline && !hotSpotColour)
            TexOutput("%");
          else if (!hotSpotColour)
            TexOutput("*");
        }
        else TexOutput("}");
      }
    }
    else // A linear document...
    {
      if ((GetNoArgs() - arg_no) == 1)
      {
        // In a linear document we just display the anchor text in italic
        if (start)
          TexOutput("{\\i ");
        else
          TexOutput("}");
        return TRUE;
      }
      else return FALSE;
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
      return FALSE;
    }
    else return FALSE;
    break;
  }
  case ltIMAGE:
  case ltIMAGEL:
  case ltIMAGER:
  case ltIMAGEMAP:
  case ltPSBOXTO:
  {
    if (arg_no == 3)
      return FALSE;
      
    static int imageWidth = 0;
    static int imageHeight = 0;
    
    if (start && (arg_no == 1))
    {
      char *imageDimensions = copystring(GetArgData());
      char buf1[50];
      strcpy(buf1, imageDimensions);
      char *tok1 = strtok(buf1, ";:");
      char *tok2 = strtok(NULL, ";:");
      // Convert points to TWIPS (1 twip = 1/20th of point)
      imageWidth = (int)(20*(tok1 ? ParseUnitArgument(tok1) : 0));
      imageHeight = (int)(20*(tok2 ? ParseUnitArgument(tok2) : 0));
      if (imageDimensions)  // glt
          delete [] imageDimensions;
      return FALSE;
    }  
    else if (start && (arg_no == 2 ))
    {
      char *filename = copystring(GetArgData());
      wxString f = "";
      if ((winHelp || (strcmp(bitmapMethod, "includepicture") == 0)  || (strcmp(bitmapMethod, "import") == 0)) && useWord)
      {
        if (f == "") // Try for a .shg (segmented hypergraphics file)
        {
          strcpy(buf, filename);
          StripExtension(buf);
          strcat(buf, ".shg");
          f = TexPathList.FindValidPath(buf);
        }
        if (f == "") // Try for a .bmp
        {
          strcpy(buf, filename);
          StripExtension(buf);
          strcat(buf, ".bmp");
          f = TexPathList.FindValidPath(buf);
        }
        if (f == "") // Try for a metafile instead
        {
          strcpy(buf, filename);
          StripExtension(buf);
          strcat(buf, ".wmf");
          f = TexPathList.FindValidPath(buf);
        }
        if (f != "")
        {
          if (winHelp)
          {
            if (bitmapTransparency && (winHelpVersion > 3))
              TexOutput("\\{bmct ");
            else
              TexOutput("\\{bmc ");
            wxString str = wxFileNameFromPath(f);
            TexOutput((char*) (const char*) str);
            TexOutput("\\}");
          }
          else
          {
            // Microsoft Word method
            if (strcmp(bitmapMethod, "import") == 0)
              TexOutput("{\\field{\\*\\fldinst IMPORT ");
            else
              TexOutput("{\\field{\\*\\fldinst INCLUDEPICTURE ");

            // Full path appears not to be valid!
            wxString str = wxFileNameFromPath(f);
            TexOutput((char*)(const char*) str);
/*
            int len = strlen(f);
            char smallBuf[2]; smallBuf[1] = 0;
            for (int i = 0; i < len; i++)
            {
              smallBuf[0] = f[i];
              TexOutput(smallBuf);
              if (smallBuf[0] == '\\')
                TexOutput(smallBuf);
            }
*/
            TexOutput("}{\\fldrslt PRESS F9 TO FORMAT PICTURE}}");
          }
        }
        else
        {
          TexOutput("[No BMP or WMF for image file ");
          TexOutput(filename);
          TexOutput("]");
          sprintf(buf, "Warning: could not find a BMP or WMF equivalent for %s.", filename);
          OnInform(buf);
        }
        if (filename)  // glt
            delete [] filename;
      }
      else // linear RTF
      {
          if (f == "") // Try for a .bmp
          {
            strcpy(buf, filename);
            StripExtension(buf);
            strcat(buf, ".bmp");
            f = TexPathList.FindValidPath(buf);
          }
          if (f != "")
          {
            FILE *fd = fopen(f, "rb");
            if (OutputBitmapHeader(fd, winHelp))
              OutputBitmapData(fd);
            else
            {
              sprintf(buf, "Could not read bitmap %s.\nMay be in wrong format (needs RGB-encoded Windows BMP).", (const char*) f);
              OnError(buf);
            }
            fclose(fd);
          }
          else // Try for a metafile instead
          {
#ifdef __WXMSW__
            strcpy(buf, filename);
            StripExtension(buf);
            strcat(buf, ".wmf");
            f = TexPathList.FindValidPath(buf);
            if (f != "")
            {
  //            HFILE handle = _lopen(f, READ);
              FILE *fd = fopen(f, "rb");
              if (OutputMetafileHeader(fd, winHelp, imageWidth, imageHeight))
              {
                OutputMetafileData(fd);
              }
              else
              {
                sprintf(buf, "Could not read metafile %s. Perhaps it's not a placeable metafile?", f);
                OnError(buf);
              }
              fclose(fd);
            }
            else
            {
#endif            
              TexOutput("[No BMP or WMF for image file ");
              TexOutput(filename);
              TexOutput("]");
              sprintf(buf, "Warning: could not find a BMP or WMF equivalent for %s.", filename);
              OnInform(buf);
#ifdef __WXMSW__
            }
#endif
        }
      }
      return FALSE;
    }
    else
      return FALSE;
    break;
  }
  case ltTABULAR:
  case ltSUPERTABULAR:
  {
    if (arg_no == 1)
    {
      if (start)
      {
        currentRowNumber = 0;
        inTabular = TRUE;
        startRows = TRUE;
        tableVerticalLineLeft = FALSE;
        tableVerticalLineRight = FALSE;
        int currentWidth = 0;

        char *alignString = copystring(GetArgData());
        ParseTableArgument(alignString);

//        TexOutput("\\trowd\\trgaph108\\trleft-108");
        TexOutput("\\trowd\\trgaph108");

        // Write the first row formatting for compatibility
        // with standard Latex
        if (compatibilityMode)
        {
          for (int i = 0; i < noColumns; i++)
          {
            currentWidth += TableData[i].width;
            sprintf(buf, "\\cellx%d", currentWidth);
            TexOutput(buf);
          }
          TexOutput("\\pard\\intbl\n");
        }
        delete[] alignString;

        return FALSE;
      }
    }
    else if (arg_no == 2 && !start)
    {
      TexOutput("\\pard\n");
      WriteEnvironmentStyles();
      inTabular = FALSE;
    }
    break;
  }

  case ltQUOTE:
  case ltVERSE:
  {
    if (start)
    {
      TexOutput("\\li360\n");
      forbidParindent ++;
      PushEnvironmentStyle("\\li360");
    }
    else
    {
      forbidParindent --;
      PopEnvironmentStyle();
      OnMacro(ltPAR, 0, TRUE);
      OnMacro(ltPAR, 0, FALSE);
    }
    break;
  }
  case ltQUOTATION:
  {
    if (start)
    {
      TexOutput("\\li360\n");
      PushEnvironmentStyle("\\li360");
    }
    else
    {
      PopEnvironmentStyle();
      OnMacro(ltPAR, 0, TRUE);
      OnMacro(ltPAR, 0, FALSE);
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
      sprintf(buf, "\\box\\trgaph108%s\n", ((macroId == ltNORMALBOXD) ? "\\brdrdb" : "\\brdrs"));
      TexOutput(buf);
      PushEnvironmentStyle(buf);
    }
    else
    {
      PopEnvironmentStyle();
      OnMacro(ltPAR, 0, TRUE);
      OnMacro(ltPAR, 0, FALSE);
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
      sprintf(buf, "\\fs%d\n", normalFont*2);
      TexOutput(buf);
      TexOutput(buf);
      return FALSE;
    }
    break;
  }
  case ltHELPFONTFAMILY:
  {
    if (start)
    {
      char *data = GetArgData();
      if (strcmp(data, "Swiss") == 0)
        TexOutput("\\f2\n");
      else if (strcmp(data, "Symbol") == 0)
        TexOutput("\\f1\n");
      else if (strcmp(data, "Times") == 0)
        TexOutput("\\f0\n");

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
      if (ParIndent == 0 || forbidParindent == 0)
      {
        sprintf(buf, "\\fi%d\n", ParIndent*20);
        TexOutput(buf);
      }
      return FALSE;
    }
    break;
  }
  case ltITEM:
  {
    if (start && IsArgOptional())
    {
      descriptionItemArg = GetArgChunk();
      return FALSE;
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
          TexOutput("\\tab ");
        break;
      }
      case 2:
      {
        if (!start)
        {
          if (macroId == ltTWOCOLITEMRULED)
            TexOutput("\\brdrb\\brdrs\\brdrw15\\brsp20 ");
          TexOutput("\\par\\pard\n");
          issuedNewParagraph = 1;
          WriteEnvironmentStyles();
        }
        break;
      }
    }
    return TRUE;
    break;
  }
  /*
   * Accents
   *
   */
  case ltACCENT_GRAVE:
  {
    if (start)
    {
      char *val = GetArgData();
      if (val)
      {
        switch (val[0])
        {
          case 'a':
           TexOutput("\\'e0");
           break;
          case 'e':
           TexOutput("\\'e8");
           break;
          case 'i':
           TexOutput("\\'ec");
           break;
          case 'o':
           TexOutput("\\'f2");
           break;
          case 'u':
           TexOutput("\\'f9");
           break;
          case 'A':
           TexOutput("\\'c0");
           break;
          case 'E':
           TexOutput("\\'c8");
           break;
          case 'I':
           TexOutput("\\'cc");
           break;
          case 'O':
           TexOutput("\\'d2");
           break;
          case 'U':
           TexOutput("\\'d9");
           break;
          default:
           break;
        }
      }
    }
    return FALSE;
    break;
  }
  case ltACCENT_ACUTE:
  {
    if (start)
    {
      char *val = GetArgData();
      if (val)
      {
        switch (val[0])
        {
          case 'a':
           TexOutput("\\'e1");
           break;
          case 'e':
           TexOutput("\\'e9");
           break;
          case 'i':
           TexOutput("\\'ed");
           break;
          case 'o':
           TexOutput("\\'f3");
           break;
          case 'u':
           TexOutput("\\'fa");
           break;
          case 'y':
           TexOutput("\\'fd");
           break;
          case 'A':
           TexOutput("\\'c1");
           break;
          case 'E':
           TexOutput("\\'c9");
           break;
          case 'I':
           TexOutput("\\'cd");
           break;
          case 'O':
           TexOutput("\\'d3");
           break;
          case 'U':
           TexOutput("\\'da");
           break;
          case 'Y':
           TexOutput("\\'dd");
           break;
          default:
           break;
        }
      }
    }
    return FALSE;
    break;
  }
  case ltACCENT_CARET:
  {
    if (start)
    {
      char *val = GetArgData();
      if (val)
      {
        switch (val[0])
        {
          case 'a':
           TexOutput("\\'e2");
           break;
          case 'e':
           TexOutput("\\'ea");
           break;
          case 'i':
           TexOutput("\\'ee");
           break;
          case 'o':
           TexOutput("\\'f4");
           break;
          case 'u':
           TexOutput("\\'fb");
           break;
          case 'A':
           TexOutput("\\'c2");
           break;
          case 'E':
           TexOutput("\\'ca");
           break;
          case 'I':
           TexOutput("\\'ce");
           break;
          case 'O':
           TexOutput("\\'d4");
           break;
          case 'U':
           TexOutput("\\'db");
           break;
          default:
           break;
        }
      }
    }
    return FALSE;
    break;
  }
  case ltACCENT_TILDE:
  {
    if (start)
    {
      char *val = GetArgData();
      if (val)
      {
        switch (val[0])
        {
          case 'a':
           TexOutput("\\'e3");
           break;
          case ' ':
           TexOutput("~");
           break;
          case 'n':
           TexOutput("\\'f1");
           break;
          case 'o':
           TexOutput("\\'f5");
           break;
          case 'A':
           TexOutput("\\'c3");
           break;
          case 'N':
           TexOutput("\\'d1");
           break;
          case 'O':
           TexOutput("\\'d5");
           break;
          default:
           break;
        }
      }
    }
    return FALSE;
    break;
  }
  case ltACCENT_UMLAUT:
  {
    if (start)
    {
      char *val = GetArgData();
      if (val)
      {
        switch (val[0])
        {
          case 'a':
           TexOutput("\\'e4");
           break;
          case 'e':
           TexOutput("\\'eb");
           break;
          case 'i':
           TexOutput("\\'ef");
           break;
          case 'o':
           TexOutput("\\'f6");
           break;
          case 'u':
           TexOutput("\\'fc");
           break;
          case 's':
           TexOutput("\\'df");
           break;
          case 'y':
           TexOutput("\\'ff");
           break;
          case 'A':
           TexOutput("\\'c4");
           break;
          case 'E':
           TexOutput("\\'cb");
           break;
          case 'I':
           TexOutput("\\'cf");
           break;
          case 'O':
           TexOutput("\\'d6");
           break;
          case 'U':
           TexOutput("\\'dc");
           break;
          case 'Y':
           TexOutput("\\'df");
           break;
          default:
           break;
        }
      }
    }
    return FALSE;
    break;
  }
  case ltACCENT_DOT:
  {
    if (start)
    {
      char *val = GetArgData();
      if (val)
      {
        switch (val[0])
        {
          case 'a':
           TexOutput("\\'e5");
           break;
          case 'A':
           TexOutput("\\'c5");
           break;
          default:
           break;
        }
      }
    }
    return FALSE;
    break;
  }
  case ltACCENT_CADILLA:
  {
    if (start)
    {
      char *val = GetArgData();
      if (val)
      {
        switch (val[0])
        {
          case 'c':
           TexOutput("\\'e7");
           break;
          case 'C':
           TexOutput("\\'c7");
           break;
          default:
           break;
        }
      }
    }
    return FALSE;
    break;
  }
  case ltFOOTNOTE:
  {
    static char *helpTopic = NULL;
    static FILE *savedOutput = NULL;
    if (winHelp)
    {
      if (arg_no == 1)
      {
        if (start)
        {
          OnInform("Consider using \\footnotepopup instead of \\footnote.");
          footnoteCount ++;
          char footBuf[20];
          sprintf(footBuf, "(%d)", footnoteCount);

          TexOutput(" {\\ul ");
          TexOutput(footBuf);
          TexOutput("}");
          helpTopic = FindTopicName(NULL);
          TexOutput("{\\v ");

          // Remove green colour/underlining if specified
          if (!hotSpotUnderline && !hotSpotColour)
            TexOutput("%");
          else if (!hotSpotColour)
            TexOutput("*");

          TexOutput(helpTopic);
          TexOutput("}");

          fprintf(Popups, "\\page\n");
//          fprintf(Popups, "\n${\\footnote }"); // No title
          fprintf(Popups, "\n#{\\footnote %s}\n", helpTopic);
          fprintf(Popups, "+{\\footnote %s}\n", GetBrowseString());
          savedOutput = CurrentOutput1;
          SetCurrentOutput(Popups);
	}
	else
	{
          SetCurrentOutput(savedOutput);
	}
	return TRUE;
      }
      return TRUE;
    }
    else
    {
      if (start)
      {
        TexOutput(" {\\super \\chftn{\\footnote \\fs20 {\\super \\chftn}", TRUE);
      }
      else
      {
        TexOutput("}}", TRUE);
      }
      return TRUE;
    }
    break;
  }
  case ltFOOTNOTEPOPUP:
  {
    static char *helpTopic = NULL;
    static FILE *savedOutput = NULL;
    if (winHelp)
    {
      if (arg_no == 1)
      {
        if (start)
        {
          TexOutput("{\\ul ");
	}
	else TexOutput("}");
	return TRUE;
      }
      else if (arg_no == 2)
      {
        if (start)
        {
          helpTopic = FindTopicName(NULL);
          TexOutput("{\\v ");

          // Remove green colour/underlining if specified
          if (!hotSpotUnderline && !hotSpotColour)
            TexOutput("%");
          else if (!hotSpotColour)
            TexOutput("*");

          TexOutput(helpTopic);
          TexOutput("}");

          fprintf(Popups, "\\page\n");
//          fprintf(Popups, "\n${\\footnote }"); // No title
          fprintf(Popups, "\n#{\\footnote %s}\n", helpTopic);
          fprintf(Popups, "+{\\footnote %s}\n", GetBrowseString());
          savedOutput = CurrentOutput1;
          SetCurrentOutput(Popups);
        }
        else
        {
          SetCurrentOutput(savedOutput);
        }
        return TRUE;
      }
    }
    else
    {
      if (arg_no == 1)
        return TRUE;
      if (start)
      {
        TexOutput(" {\\super \\chftn{\\footnote \\fs20 {\\super \\chftn}", TRUE);
      }
      else
      {
        TexOutput("}}", TRUE);
      }
      return TRUE;
    }
    break;
  }
  case ltFANCYPLAIN:
  {
    if (start && (arg_no == 1))
      return FALSE;
    else
      return TRUE;
    break;
  }
  case ltSETHEADER:
  {
    if (start)
      forbidResetPar ++;
    else
      forbidResetPar --;

    if (winHelp) return FALSE;
    if (start)
    {
      switch (arg_no)
      {
        case 1:
          LeftHeaderEven = GetArgChunk();
          if (strlen(GetArgData(LeftHeaderEven)) == 0)
            LeftHeaderEven = NULL;
          break;
        case 2:
          CentreHeaderEven = GetArgChunk();
          if (strlen(GetArgData(CentreHeaderEven)) == 0)
            CentreHeaderEven = NULL;
          break;
        case 3:
          RightHeaderEven = GetArgChunk();
          if (strlen(GetArgData(RightHeaderEven)) == 0)
            RightHeaderEven = NULL;
          break;
        case 4:
          LeftHeaderOdd = GetArgChunk();
          if (strlen(GetArgData(LeftHeaderOdd)) == 0)
            LeftHeaderOdd = NULL;
          break;
        case 5:
          CentreHeaderOdd = GetArgChunk();
          if (strlen(GetArgData(CentreHeaderOdd)) == 0)
            CentreHeaderOdd = NULL;
          break;
        case 6:
          RightHeaderOdd = GetArgChunk();
          if (strlen(GetArgData(RightHeaderOdd)) == 0)
            RightHeaderOdd = NULL;
          OutputRTFHeaderCommands();
          break;
        default:
          break;
      }
    }
    return FALSE;
    break;
  }
  case ltSETFOOTER:
  {
    if (start)
      forbidResetPar ++;
    else
      forbidResetPar --;

    if (winHelp) return FALSE;
    if (start)
    {
      switch (arg_no)
      {
        case 1:
          LeftFooterEven = GetArgChunk();
          if (strlen(GetArgData(LeftFooterEven)) == 0)
            LeftFooterEven = NULL;
          break;
        case 2:
          CentreFooterEven = GetArgChunk();
          if (strlen(GetArgData(CentreFooterEven)) == 0)
            CentreFooterEven = NULL;
          break;
        case 3:
          RightFooterEven = GetArgChunk();
          if (strlen(GetArgData(RightFooterEven)) == 0)
            RightFooterEven = NULL;
          break;
        case 4:
          LeftFooterOdd = GetArgChunk();
          if (strlen(GetArgData(LeftFooterOdd)) == 0)
            LeftFooterOdd = NULL;
          break;
        case 5:
          CentreFooterOdd = GetArgChunk();
          if (strlen(GetArgData(CentreFooterOdd)) == 0)
            CentreFooterOdd = NULL;
          break;
        case 6:
          RightFooterOdd = GetArgChunk();
          if (strlen(GetArgData(RightFooterOdd)) == 0)
            RightFooterOdd = NULL;
          OutputRTFFooterCommands();
          break;
        default:
          break;
      }
    }
    return FALSE;
    break;
  }
  case ltMARKRIGHT:
  {
    if (winHelp) return FALSE;
    // Fake a SetHeader command
    if (start)
    {
      LeftHeaderOdd = NULL;
      CentreHeaderOdd = NULL;
      RightHeaderOdd = NULL;
      LeftHeaderEven = NULL;
      CentreHeaderEven = NULL;
      RightHeaderEven = NULL;
      OnInform("Consider using setheader/setfooter rather than markright.");
    }
    RTFOnArgument(ltSETHEADER, 4, start);
    if (!start)
      OutputRTFHeaderCommands();
    return FALSE;
    break;
  }
  case ltMARKBOTH:
  {
    if (winHelp) return FALSE;
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
          OnInform("Consider using setheader/setfooter rather than markboth.");
        }
        return RTFOnArgument(ltSETHEADER, 1, start);
        break;
      }
      case 2:
      {
        RTFOnArgument(ltSETHEADER, 4, start);
        if (!start)
          OutputRTFHeaderCommands();
        return FALSE;
        break;
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

    if (winHelp) return FALSE;
    if (start)
    {
      TexOutput("\\pgnrestart");
      char *data = GetArgData();
      if (currentNumberStyle) delete[] currentNumberStyle;
      currentNumberStyle = copystring(data);
      OutputNumberStyle(currentNumberStyle);
      
      TexOutput("\n");
    }
    return FALSE;
    break;
  }
  case ltTWOCOLUMN:
  {
    if (winHelp) return FALSE;
    if (start)
      return TRUE;
    break;
  }
  case ltITEMSEP:
  {
    if (start)
    {
      char *val = GetArgData();
      currentItemSep = ParseUnitArgument(val);
      return FALSE;
    }
    break;
  }
  case ltEVENSIDEMARGIN:
  {
    return FALSE;
    break;
  }
  case ltODDSIDEMARGIN:
  {
    if (start)
    {
      char *val = GetArgData();
      int twips = (int)(20*ParseUnitArgument(val));
      // Add an inch since in LaTeX it's specified minus an inch
      twips += 1440;
      CurrentLeftMarginOdd = twips;
      sprintf(buf, "\\margl%d\n", twips);
      TexOutput(buf);

      CurrentMarginParX = CurrentLeftMarginOdd + CurrentTextWidth + CurrentMarginParSep;
    }
    return FALSE;
  }
  case ltMARGINPARWIDTH:
  {
    if (start)
    {
      char *val = GetArgData();
      int twips = (int)(20*ParseUnitArgument(val));
      CurrentMarginParWidth = twips;
    }
    return FALSE;
  }
  case ltMARGINPARSEP:
  {
    if (start)
    {
      char *val = GetArgData();
      int twips = (int)(20*ParseUnitArgument(val));
      CurrentMarginParSep = twips;
      CurrentMarginParX = CurrentLeftMarginOdd + CurrentTextWidth + CurrentMarginParSep;
    }
    return FALSE;
  }
  case ltTEXTWIDTH:
  {
    if (start)
    {
      char *val = GetArgData();
      int twips = (int)(20*ParseUnitArgument(val));
      CurrentTextWidth = twips;

      // Need to set an implicit right margin
      CurrentRightMarginOdd = PageWidth - CurrentTextWidth - CurrentLeftMarginOdd;
      CurrentRightMarginEven = PageWidth - CurrentTextWidth - CurrentLeftMarginEven;
      CurrentMarginParX = CurrentLeftMarginOdd + CurrentTextWidth + CurrentMarginParSep;
      sprintf(buf, "\\margr%d\n", CurrentRightMarginOdd);
      TexOutput(buf);
    }
    return FALSE;
  }
  case ltMARGINPAR:
  case ltMARGINPARODD:
  {
    if (start)
    {
      if (winHelp)
      {
        TexOutput("\\box\n");
        PushEnvironmentStyle("\\box");
      }
      else
      {
        sprintf(buf, "\\phpg\\posx%d\\absw%d\n", CurrentMarginParX, CurrentMarginParWidth);
        TexOutput(buf);
      }
      return TRUE;
    }
    else
    {
      if (winHelp)
      {
        TexOutput("\\par\\pard\n");
        PopEnvironmentStyle();
        WriteEnvironmentStyles();
      }
      else
        TexOutput("\\par\\pard\n");
      issuedNewParagraph = 1;
    }
    return FALSE;
  }
  case ltMARGINPAREVEN:
  {
    if (start)
    {
      if (winHelp)
      {
        TexOutput("\\box\n");
        PushEnvironmentStyle("\\box");
      }
      else
      {
        if (mirrorMargins)
        {
          // Have to calculate what the margins are changed to in WfW margin
          // mirror mode, on an even (left-hand) page.
          int x = PageWidth - CurrentRightMarginOdd - CurrentMarginParWidth - CurrentMarginParSep
                    - CurrentTextWidth + GutterWidth;
          sprintf(buf, "\\phpg\\posx%d\\absw%d\n", x, CurrentMarginParWidth);
          TexOutput(buf);
        }
        else
        {
          sprintf(buf, "\\phpg\\posx%d\\absw%d\n", CurrentMarginParX, CurrentMarginParWidth);
          TexOutput(buf);
        }
      }
      return TRUE;
    }
    else
    {
      if (winHelp)
      {
        TexOutput("\\par\\pard\n");
        PopEnvironmentStyle();
        WriteEnvironmentStyles();
      }
      else
        issuedNewParagraph = 1;
      TexOutput("\\par\\pard\n");
    }
    return FALSE;
  }
  case ltTWOCOLWIDTHA:
  {
    if (start)
    {
      char *val = GetArgData();
      int twips = (int)(20*ParseUnitArgument(val));
      TwoColWidthA = twips;
    }
    return FALSE;
    break;
  }
  case ltTWOCOLWIDTHB:
  {
    if (start)
    {
      char *val = GetArgData();
      int twips = (int)(20*ParseUnitArgument(val));
      TwoColWidthB = twips;
    }
    return FALSE;
    break;
  }
  case ltROW:
  case ltRULEDROW:
  {
    if (start)
    {
      int currentWidth = 0;

      if (!compatibilityMode || (currentRowNumber > 0))
      {
      TexOutput("\\pard\\intbl");

      if (macroId == ltRULEDROW)
        ruleBottom = 1;
      for (int i = 0; i < noColumns; i++)
      {
        currentWidth += TableData[i].width;
        if (ruleTop == 1)
        {
          TexOutput("\\clbrdrt\\brdrs\\brdrw15");
        }
        else if (ruleTop > 1)
        {
          TexOutput("\\clbrdrt\\brdrdb\\brdrw15");
        }
        if (ruleBottom == 1)
        {
          TexOutput("\\clbrdrb\\brdrs\\brdrw15");
        }
        else if (ruleBottom > 1)
        {
          TexOutput("\\clbrdrb\\brdrdb\\brdrw15");
        }

        if (TableData[i].rightBorder)
          TexOutput("\\clbrdrr\\brdrs\\brdrw15");

        if (TableData[i].leftBorder)
          TexOutput("\\clbrdrl\\brdrs\\brdrw15");
          
        sprintf(buf, "\\cellx%d", currentWidth);
        TexOutput(buf);
      }
      TexOutput("\\pard\\intbl\n");
      }
      ruleTop = 0;
      ruleBottom = 0;
      currentRowNumber ++;
      return TRUE;
    }
    else
    {
//      TexOutput("\\cell\\row\\trowd\\trgaph108\\trleft-108\n");
      TexOutput("\\cell\\row\\trowd\\trgaph108\n");
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
          noMultiColumns = atoi(GetArgData());
          return FALSE;
          break;
        }
        case 2:
        {
          return FALSE;
        }
        case 3:
        {
          return TRUE;
        }
      }
    }
    else
    {
      if (arg_no == 3)
      {
        for (int i = 1; i < noMultiColumns; i ++)
          TexOutput("\\cell");
      }
    }
    break;
  }
  case ltINDENTED:
  {
    if (start && (arg_no == 1))
    {
//      indentLevel ++;
//      TexOutput("\\fi0\n");
      int oldIndent = 0;
      wxNode *node = itemizeStack.First();
      if (node)
        oldIndent = ((ItemizeStruc *)node->Data())->indentation;

      int indentValue = 20*ParseUnitArgument(GetArgData());
      int indentSize = indentValue + oldIndent;

      ItemizeStruc *struc = new ItemizeStruc(LATEX_INDENT, indentSize);
      itemizeStack.Insert(struc);
      
      sprintf(buf, "\\tx%d\\li%d ", indentSize, indentSize);
      PushEnvironmentStyle(buf);
      TexOutput(buf);
      return FALSE;
    }
    if (!start && (arg_no == 2))
    {
      PopEnvironmentStyle();
      if (itemizeStack.First())
      {
        ItemizeStruc *struc = (ItemizeStruc *)itemizeStack.First()->Data();
        delete struc;
        delete itemizeStack.First();
      }
      if (itemizeStack.Number() == 0)
      {
        TexOutput("\\par\\pard\n");
        issuedNewParagraph = 1;
        WriteEnvironmentStyles();
      }
    }
    return TRUE;
    break;
  }
/*
  case ltSIZEDBOX:
  case ltSIZEDBOXD:
  {
    if (start && (arg_no == 1))
    {
      int oldIndent = 0;
      wxNode *node = itemizeStack.First();
      if (node)
        oldIndent = ((ItemizeStruc *)node->Data())->indentation;

      int boxWidth = 20*ParseUnitArgument(GetArgData());

      int indentValue = (int)((CurrentTextWidth - oldIndent - boxWidth)/2.0);
      int indentSize = indentValue + oldIndent;
      int indentSizeRight = indentSize + boxWidth;

      ItemizeStruc *struc = new ItemizeStruc(LATEX_INDENT, indentSize);
      itemizeStack.Insert(struc);
      
      sprintf(buf, "\\tx%d\\li%d\\lr%d\\box%s ", indentSize, indentSize, indentSizeRight,
        ((macroId == ltCENTEREDBOX) ? "\\brdrs" : "\\brdrdb"));
      PushEnvironmentStyle(buf);
      TexOutput(buf);
      return FALSE;
    }
    if (!start && (arg_no == 2))
    {
      PopEnvironmentStyle();
      if (itemizeStack.First())
      {
        ItemizeStruc *struc = (ItemizeStruc *)itemizeStack.First()->Data();
        delete struc;
        delete itemizeStack.First();
      }
      if (itemizeStack.Number() == 0)
      {
        TexOutput("\\par\\pard\n");
        issuedNewParagraph = 1;
        WriteEnvironmentStyles();
      }
    }
    return TRUE;
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
        if (StringMatch("twoside", MinorDocumentStyleString))
          // Mirror margins, switch on odd/even headers & footers, and break sections at odd pages
          TexOutput("\\margmirror\\facingp\\sbkodd");
        if (StringMatch("twocolumn", MinorDocumentStyleString))
          TexOutput("\\cols2");
      }
      TexOutput("\n");
    }
    return FALSE;
  }
  case ltSETHOTSPOTCOLOUR:
  case ltSETHOTSPOTCOLOR:
  {
    if (!start)
    {
      char *text = GetArgData();
      if (strcmp(text, "yes") == 0 || strcmp(text, "on") == 0 || strcmp(text, "ok") == 0)
        hotSpotColour = TRUE;
      else
        hotSpotColour = FALSE;
    }
    return FALSE;
  }
  case ltSETTRANSPARENCY:
  {
    if (!start)
    {
      char *text = GetArgData();
      if (strcmp(text, "yes") == 0 || strcmp(text, "on") == 0 || strcmp(text, "ok") == 0)
        bitmapTransparency = TRUE;
      else
        bitmapTransparency = FALSE;
    }
    return FALSE;
  }
  case ltSETHOTSPOTUNDERLINE:
  {
    if (!start)
    {
      char *text = GetArgData();
      if (strcmp(text, "yes") == 0 || strcmp(text, "on") == 0 || strcmp(text, "ok") == 0)
        hotSpotUnderline = TRUE;
      else
        hotSpotUnderline = FALSE;
    }
    return FALSE;
  }
  case ltBIBITEM:
  {
    if (arg_no == 1 && start)
    {
      char *citeKey = GetArgData();
      TexRef *ref = (TexRef *)TexReferences.Get(citeKey);
      if (ref)
      {
        if (ref->sectionNumber) delete[] ref->sectionNumber;
        sprintf(buf, "[%d]", citeCount);
        ref->sectionNumber = copystring(buf);
      }

      TexOutput("\\li260\\fi-260 "); // Indent from 2nd line
      sprintf(buf, "{\\b [%d]} ", citeCount);
      TexOutput(buf);
      citeCount ++;
      return FALSE;
    }
    if (arg_no == 2 && !start)
      TexOutput("\\par\\pard\\par\n\n");
    return TRUE;
    break;
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
        fprintf(Chapters, "\\sect\\pgncont\\titlepg\n");

        // If a non-custom page style, we generate the header now.
        if (PageStyle && (strcmp(PageStyle, "plain") == 0 ||
                          strcmp(PageStyle, "empty") == 0 ||
                          strcmp(PageStyle, "headings") == 0))
        {
          OutputRTFHeaderCommands();
          OutputRTFFooterCommands();
        }
        
        // Need to reset the current numbering style, or RTF forgets it.
        OutputNumberStyle(currentNumberStyle);
        SetCurrentOutput(Contents);
      }
      else
        fprintf(Chapters, "\\page\n");

      if (winHelp)
        fprintf(Contents, "\n{\\uldb %s}", ReferencesNameString);
      else
        fprintf(Contents, "\\par\n\\pard{\\b %s}", ReferencesNameString);

      startedSections = TRUE;

      if (winHelp)
        fprintf(Chapters, "\n${\\footnote %s}", ReferencesNameString);

      char *topicName = "bibliography";

      if (winHelp)
        fprintf(Contents, "{\\v %s}\\par\\pard\n", topicName);
      else
        fprintf(Contents, "\\par\\par\\pard\n");

      if (winHelp)
      {
        fprintf(Chapters, "\n#{\\footnote %s}\n", topicName);
        fprintf(Chapters, "+{\\footnote %s}\n", GetBrowseString());
        fprintf(Chapters, "K{\\footnote {K} %s}\n", ReferencesNameString);
        GenerateKeywordsForTopic(topicName);
        if (useUpButton)
        {
          fprintf(Chapters, "!{\\footnote EnableButton(\"Up\");ChangeButtonBinding(\"Up\", \"JumpId(`%s.hlp', `%s')\")}\n",
               FileNameFromPath(FileRoot), "Contents");
        }
      }
      
      SetCurrentOutput(Chapters);
      char *styleCommand = "";
      if (!winHelp && useHeadingStyles)
        styleCommand = "\\s1";
      fprintf(Chapters, "\\pard{%s", (winHelp ? "\\keepn\\sa140\\sb140" : styleCommand));
      WriteHeadingStyle(Chapters, 1);  fprintf(Chapters, " References\\par\\pard}\n");

      return FALSE;
    }
    return TRUE;
    break;
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
//      char *entry = GetArgData();
      char buf[300];
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
    return FALSE;
    break;
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
          char *name = GetArgData();
          int pos = FindColourPosition(name);
          if (pos > -1)
          {
            sprintf(buf, "{%s%d ", ((macroId == ltFCOL) ? "\\cf" : "\\cb"), pos);
            TexOutput(buf);
          }
		  else
		  {
			sprintf(buf, "Could not find colour name %s", name);
            OnError(buf);
		  }
          break;
        }
        case 2:
        {
          return TRUE;
          break;
        }
        default:
          break;
      }
    }
    else
    {
      if (arg_no == 2) TexOutput("}");
    }
    return FALSE;
    break;
  }
  case ltLABEL:
  {
    if (start && !winHelp && useWord)
    {
      char *s = GetArgData();
      // Only insert a bookmark here if it's not just been inserted
      // in a section heading.
      if ( !CurrentTopic || !(strcmp(CurrentTopic, s) == 0) )
/*
      if ( (!CurrentChapterName || !(CurrentChapterName && (strcmp(CurrentChapterName, s) == 0))) &&
           (!CurrentSectionName || !(CurrentSectionName && (strcmp(CurrentSectionName, s) == 0))) &&
           (!CurrentSubsectionName || !(CurrentSubsectionName && (strcmp(CurrentSubsectionName, s) == 0)))
         )
*/
      {
          fprintf(Chapters, "{\\bkmkstart %s}{\\bkmkend %s}", s,s);
      }
    }
    return FALSE;
    break;
  }
  case ltPAGEREF:
  {
    if (start && useWord && !winHelp)
    {
      char *s = GetArgData();
      fprintf(Chapters, "{\\field{\\*\\fldinst  PAGEREF %s \\\\* MERGEFORMAT }{\\fldrslt ??}}",
              s);
    }
    return FALSE;
    break;
  }
  case ltPOPREFONLY:
  {
    if (start)
      inPopRefSection = TRUE;
    else
      inPopRefSection = FALSE;
    break;
  }
  case ltINSERTATLEVEL:
  {
    // This macro allows you to insert text at a different level
    // from the current level, e.g. into the Sections from within a subsubsection.
    if (!winHelp & useWord)
        return FALSE;
    static int currentLevelNo = 1;
    static FILE* oldLevelFile = Chapters;
    if (start)
    {
      switch (arg_no)
      {
        case 1:
        {
          oldLevelFile = CurrentOutput1;

          char *str = GetArgData();
          currentLevelNo = atoi(str);
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
          return FALSE;
          break;
        }
        case 2:
        {
          return TRUE;
          break;
        }
        default:
          break;
      }
      return TRUE;
    }
    else
    {
        if (arg_no == 2)
        {
            CurrentOutput1 = oldLevelFile;
        }
        return TRUE;
    }
    break;
  }
  default:
  {
    return DefaultOnArgument(macroId, arg_no, start);
    break;
  }
  }
  return TRUE;
}

bool RTFGo(void)
{
  // Reset variables
  indentLevel = 0;
  forbidParindent = 0;
  contentsLineSection = NULL;
  contentsLineValue = NULL;
  descriptionItemArg = NULL;
  inTabular = FALSE;
  inTable = FALSE;
  inFigure = FALSE;
  startRows = FALSE;
  tableVerticalLineLeft = FALSE;
  tableVerticalLineRight = FALSE;
  noColumns = 0;
  startedSections = FALSE;
  inVerbatim = FALSE;
  browseId = 0;
  
  if (InputFile && OutputFile)
  {
    // Do some RTF-specific transformations on all the strings,
    // recursively
    Text2RTF(GetTopLevelChunk());

    Contents = fopen(TmpContentsName, "w");
    Chapters = fopen("chapters.rtf", "w");
    if (winHelp)
    {
      Sections = fopen("sections.rtf", "w");
      Subsections = fopen("subsections.rtf", "w");
      Subsubsections = fopen("subsubsections.rtf", "w");
      Popups = fopen("popups.rtf", "w");
      if (winHelpContents)
      {
        WinHelpContentsFile = fopen(WinHelpContentsFileName, "w");
        if (WinHelpContentsFile)
          fprintf(WinHelpContentsFile, ":Base %s.hlp\n", wxFileNameFromPath(FileRoot));
      }

      if (!Sections || !Subsections || !Subsubsections || !Popups || (winHelpContents && !WinHelpContentsFile))
      {
        OnError("Ouch! Could not open temporary file(s) for writing.");
        return FALSE;
      }
    }
    if (!Contents || !Chapters)
    {
      OnError("Ouch! Could not open temporary file(s) for writing.");
      return FALSE;
    }

    if (winHelp)
    {
      fprintf(Chapters, "\n#{\\footnote Contents}\n");
      fprintf(Chapters, "${\\footnote Contents}\n");
      fprintf(Chapters, "+{\\footnote %s}\n", GetBrowseString());
      fprintf(Chapters, "K{\\footnote {K} %s}\n", ContentsNameString);
      fprintf(Chapters, "!{\\footnote DisableButton(\"Up\")}\n");
    }
    if (!winHelp)
    {
      fprintf(Chapters, "\\titlepg\n");
      fprintf(Contents, "\\par\\pard\\pgnrestart\\sect\\titlepg");
    }
    
    // In WinHelp, Contents title takes font of title.
    // In linear RTF, same as chapter headings.
    fprintf(Contents, "{\\b\\fs%d %s}\\par\\par\\pard\n\n",
      (winHelp ? titleFont : chapterFont)*2, ContentsNameString);

    // By default, Swiss, 10 point.
    fprintf(Chapters, "\\f2\\fs20\n");

    SetCurrentOutput(Chapters);

    OnInform("Converting...");

    TraverseDocument();

    FILE *Header = fopen("header.rtf", "w");
    if (!Header)
    {
      OnError("Ouch! Could not open temporary file header.rtf for writing.");
      return FALSE;
    }
    WriteRTFHeader(Header);
    fclose(Header); Header = NULL;
    
    Tex2RTFYield(TRUE);
    if (winHelp)
    {
//      fprintf(Contents, "\\page\n");
      fprintf(Chapters, "\\page\n");
      fprintf(Sections, "\\page\n");
      fprintf(Subsections, "\\page\n");
      fprintf(Subsubsections, "\\page\n\n");
      fprintf(Popups, "\\page\n}\n");
    }

//    TexOutput("\n\\info{\\doccomm Document created by Julian Smart's Tex2RTF.}\n");
    if (!winHelp)
      TexOutput("}\n");
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
      wxConcatFiles("header.rtf", "chapters.rtf", "tmp1.rtf");
      Tex2RTFYield(TRUE);
      wxConcatFiles("tmp1.rtf", "sections.rtf", "tmp2.rtf");
      Tex2RTFYield(TRUE);
      wxConcatFiles("tmp2.rtf", "subsections.rtf", "tmp3.rtf");
      Tex2RTFYield(TRUE);
      wxConcatFiles("tmp3.rtf", "subsubsections.rtf", "tmp4.rtf");
      Tex2RTFYield(TRUE);
      wxConcatFiles("tmp4.rtf", "popups.rtf", OutputFile);
      Tex2RTFYield(TRUE);

      wxRemoveFile("tmp1.rtf");
      wxRemoveFile("tmp2.rtf");
      wxRemoveFile("tmp3.rtf");
      wxRemoveFile("tmp4.rtf");
    }
    else
    {
      wxConcatFiles("header.rtf", "chapters.rtf", "tmp1.rtf");
      Tex2RTFYield(TRUE);
      if (FileExists(OutputFile))
          wxRemoveFile(OutputFile);

      char *cwdStr;
      cwdStr = wxGetWorkingDirectory();

      wxString outputDirStr;
      outputDirStr = wxPathOnly(OutputFile);

      // Determine if the temp file and the output file are in the same directory,
      // and if they are, then just rename the temp file rather than copying
      // it, as this is much faster when working with large (multi-megabyte files)
      if ((wxStrcmp(outputDirStr.c_str(),"") == 0) ||  // no path specified on output file
          (wxStrcmp(cwdStr,outputDirStr.c_str()) == 0)) // paths do not match
      {
        wxRenameFile("tmp1.rtf", OutputFile);
      }
      else
      {
        wxCopyFile("tmp1.rtf", OutputFile);
      }
      delete [] cwdStr;
      Tex2RTFYield(TRUE);
      wxRemoveFile("tmp1.rtf");
    }
    
    if (FileExists(ContentsName)) wxRemoveFile(ContentsName);

    if (!wxRenameFile(TmpContentsName, ContentsName))
    {
      wxCopyFile(TmpContentsName, ContentsName);
      wxRemoveFile(TmpContentsName);
    }

    wxRemoveFile("chapters.rtf");
    wxRemoveFile("header.rtf");
      
    if (winHelp)
    {
      wxRemoveFile("sections.rtf");
      wxRemoveFile("subsections.rtf");
      wxRemoveFile("subsubsections.rtf");
      wxRemoveFile("popups.rtf");
    }
    if (winHelp && generateHPJ)
      WriteHPJ(OutputFile);
    return TRUE;
  }
  return FALSE;
}

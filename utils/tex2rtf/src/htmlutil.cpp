/////////////////////////////////////////////////////////////////////////////
// Name:        htmlutil.cpp
// Purpose:     Converts Latex to HTML
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
#include "table.h"

extern void DecToHex(int, char *);
void GenerateHTMLIndexFile(char *fname);

void GenerateHTMLWorkshopFiles(char *fname);
void HTMLWorkshopAddToContents(int level, char *s, char *file);
void HTMLWorkshopStartContents();
void HTMLWorkshopEndContents();

void OutputContentsFrame(void);

#include "readshg.h" // Segmented hypergraphics parsing

char *ChaptersName = NULL;
char *SectionsName = NULL;
char *SubsectionsName = NULL;
char *SubsubsectionsName = NULL;
char *TitlepageName = NULL;
char *lastFileName = NULL;
char *lastTopic = NULL;
char *currentFileName = NULL;
char *contentsFrameName = NULL;

static TexChunk *descriptionItemArg = NULL;
static TexChunk *helpRefFilename = NULL;
static TexChunk *helpRefText = NULL;
static int indentLevel = 0;
static int citeCount = 1;
extern FILE *Contents;
FILE *FrameContents = NULL;
FILE *Titlepage = NULL;
// FILE *FrameTitlepage = NULL;
int fileId = 0;
bool subsectionStarted = FALSE;

// Which column of a row are we in? (Assumes no nested tables, of course)
int currentColumn = 0;

// Are we in verbatim mode? If so, format differently.
static bool inVerbatim = FALSE;

// Need to know whether we're in a table or figure for benefit
// of listoffigures/listoftables
static bool inFigure = FALSE;
static bool inTable = FALSE;

// This is defined in the Tex2Any library.
extern char *BigBuffer;

class HyperReference: public wxObject
{
 public:
  char *refName;
  char *refFile;
  HyperReference(char *name, char *file)
  {
    if (name) refName = copystring(name);
    if (file) refFile = copystring(file);
  }
};

class TexNextPage: public wxObject
{
 public:
  char *label;
  char *filename;
  TexNextPage(char *theLabel, char *theFile)
  {
    label = copystring(theLabel);
    filename = copystring(theFile);
  }
  ~TexNextPage(void)
  {
    delete[] label;
    delete[] filename;
  }
};

wxHashTable TexNextPages(wxKEY_STRING);

static char *CurrentChapterName = NULL;
static char *CurrentChapterFile = NULL;
static char *CurrentSectionName = NULL;
static char *CurrentSectionFile = NULL;
static char *CurrentSubsectionName = NULL;
static char *CurrentSubsectionFile = NULL;
static char *CurrentSubsubsectionName = NULL;
static char *CurrentSubsubsectionFile = NULL;
static char *CurrentTopic = NULL;

static void SetCurrentTopic(char *s)
{
  if (CurrentTopic) delete[] CurrentTopic;
  CurrentTopic = copystring(s);
}

void SetCurrentChapterName(char *s, char *file)
{
  if (CurrentChapterName) delete[] CurrentChapterName;
  CurrentChapterName = copystring(s);
  if (CurrentChapterFile) delete[] CurrentChapterFile;
  CurrentChapterFile = copystring(file);

  currentFileName = CurrentChapterFile;

  SetCurrentTopic(s);
}
void SetCurrentSectionName(char *s, char *file)
{
  if (CurrentSectionName) delete[] CurrentSectionName;
  CurrentSectionName = copystring(s);
  if (CurrentSectionFile) delete[] CurrentSectionFile;
  CurrentSectionFile = copystring(file);

  currentFileName = CurrentSectionFile;
  SetCurrentTopic(s);
}
void SetCurrentSubsectionName(char *s, char *file)
{
  if (CurrentSubsectionName) delete[] CurrentSubsectionName;
  CurrentSubsectionName = copystring(s);
  if (CurrentSubsectionFile) delete[] CurrentSubsectionFile;
  CurrentSubsectionFile = copystring(file);
  currentFileName = CurrentSubsectionFile;
  SetCurrentTopic(s);
}
void SetCurrentSubsubsectionName(char *s, char *file)
{
  if (CurrentSubsubsectionName) delete[] CurrentSubsubsectionName;
  CurrentSubsubsectionName = copystring(s);
  if (CurrentSubsubsectionFile) delete[] CurrentSubsubsectionFile;
  CurrentSubsubsectionFile = copystring(file);
  currentFileName = CurrentSubsubsectionFile;
  SetCurrentTopic(s);
}

/*
 * Close former filedescriptor and reopen using another filename.
 *
 */

void ReopenFile(FILE **fd, char **fileName)
{
  if (*fd)
  {
    fprintf(*fd, "\n</BODY></HTML>\n");
    fclose(*fd);
  }
  fileId ++;
  char buf[400];
  if (truncateFilenames)
    sprintf(buf, "%s%d.htm", FileRoot, fileId);
  else
    sprintf(buf, "%s%d.html", FileRoot, fileId);
  if (*fileName) delete[] *fileName;
  *fileName = copystring(FileNameFromPath(buf));
  *fd = fopen(buf, "w");
  fprintf(*fd, "<HTML>\n");
}

/*
 * Reopen section contents file, i.e. the index appended to each section
 * in subsectionCombine mode
 */

static char *SectionContentsFilename = NULL;
static FILE *SectionContentsFD = NULL;

void ReopenSectionContentsFile(void)
{
    if ( SectionContentsFD )
    {
        fclose(SectionContentsFD);
    }
    if ( SectionContentsFilename )
        delete[] SectionContentsFilename;
    SectionContentsFD = NULL;
    SectionContentsFilename = NULL;

    // Create the name from the current section filename
    if ( CurrentSectionFile )
    {
        char buf[256];
        strcpy(buf, CurrentSectionFile);
        wxStripExtension(buf);
        strcat(buf, ".con");
        SectionContentsFilename = copystring(buf);

        SectionContentsFD = fopen(SectionContentsFilename, "w");
    }
}


/*
 * Given a TexChunk with a string value, scans through the string
 * converting Latex-isms into HTML-isms, such as 2 newlines -> <P>.
 *
 */

void ProcessText2HTML(TexChunk *chunk)
{
  bool changed = FALSE;
  int ptr = 0;
  int i = 0;
  char ch = 1;
  int len = strlen(chunk->value);
  while (ch != 0)
  {
    ch = chunk->value[i];

    // 2 newlines means \par
    if (!inVerbatim && chunk->value[i] == 10 && ((len > i+1 && chunk->value[i+1] == 10) ||
                        ((len > i+1 && chunk->value[i+1] == 13) &&
                        (len > i+2 && chunk->value[i+2] == 10))))
    {
      BigBuffer[ptr] = 0; strcat(BigBuffer, "<P>\n\n"); ptr += 5;
      i += 2;
      changed = TRUE;
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
    else if (ch == '<') // Change < to &lt
    {
      BigBuffer[ptr] = 0;
      strcat(BigBuffer, "&lt;");
      ptr += 4;
      i += 1;
      changed = TRUE;
    }
    else if (ch == '>') // Change > to &gt
    {
      BigBuffer[ptr] = 0;
      strcat(BigBuffer, "&gt;");
      ptr += 4;
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
    delete chunk->value;
    chunk->value = copystring(BigBuffer);
  }
}

/*
 * Scan through all chunks starting from the given one,
 * calling ProcessText2HTML to convert Latex-isms to RTF-isms.
 * This should be called after Tex2Any has parsed the file,
 * and before TraverseDocument is called.
 *
 */

void Text2HTML(TexChunk *chunk)
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

      if (def && (def->macroId == ltVERBATIM || def->macroId == ltVERB || def->macroId == ltSPECIAL))
        inVerbatim = TRUE;

      wxNode *node = chunk->children.First();
      while (node)
      {
        TexChunk *child_chunk = (TexChunk *)node->Data();
        Text2HTML(child_chunk);
        node = node->Next();
      }

      if (def && (def->macroId == ltVERBATIM || def->macroId == ltVERB || def->macroId == ltSPECIAL))
        inVerbatim = FALSE;

      break;
    }
    case CHUNK_TYPE_ARG:
    {
      wxNode *node = chunk->children.First();
      while (node)
      {
        TexChunk *child_chunk = (TexChunk *)node->Data();
        Text2HTML(child_chunk);
        node = node->Next();
      }

      break;
    }
    case CHUNK_TYPE_STRING:
    {
      if (chunk->value)
        ProcessText2HTML(chunk);
      break;
    }
  }
}

/*
 * Add appropriate browse buttons to this page.
 *
 */

void AddBrowseButtons(char *upLabel, char *upFilename,
  char *previousLabel, char *previousFilename,
  char *thisLabel, char *thisFilename)
{
  char contentsReferenceBuf[80];
  char upReferenceBuf[80];
  char backReferenceBuf[80];
  char forwardReferenceBuf[80];
  if (htmlBrowseButtons == HTML_BUTTONS_NONE)
    return;

  char *contentsReference = NULL;
  if (htmlBrowseButtons == HTML_BUTTONS_TEXT)
    contentsReference = ContentsNameString;
  else
  {
//    contentsReference = "<img align=center src=\"contents.gif\" BORDER=0 ALT=\"Contents\">";
    contentsReference = contentsReferenceBuf;
    sprintf(contentsReference, "<img align=center src=\"%s\" BORDER=0 ALT=\"Contents\">", ConvertCase("contents.gif"));
  }

  char *upReference = NULL;
  if (htmlBrowseButtons == HTML_BUTTONS_TEXT)
    upReference = UpNameString;
  else
  {
//    upReference = "<img align=center src=\"up.gif\" ALT=\"Up\">";
    upReference = upReferenceBuf;
    sprintf(upReference, "<img align=center src=\"%s\" BORDER=0 ALT=\"Up\">", ConvertCase("up.gif"));
  }

  char *backReference = NULL;
  if (htmlBrowseButtons == HTML_BUTTONS_TEXT)
    backReference = "&lt;&lt;";
  else
  {
//    backReference = "<img align=center src=\"back.gif\" ALT=\"Previous\">";
    backReference = backReferenceBuf;
    sprintf(backReference, "<img align=center src=\"%s\" BORDER=0 ALT=\"Previous\">", ConvertCase("back.gif"));
  }

  char *forwardReference = NULL;
  if (htmlBrowseButtons == HTML_BUTTONS_TEXT)
    forwardReference = "&gt;&gt;";
  else
  {
//    forwardReference = "<img align=center src=\"forward.gif\" ALT=\"Next\">";
    forwardReference = forwardReferenceBuf;
    sprintf(forwardReference, "<img align=center src=\"%s\" BORDER=0 ALT=\"Next\">", ConvertCase("forward.gif"));
  }

  TexOutput("<CENTER>");

  char buf[200];

  /*
   * Contents button
   *
   */

  if (truncateFilenames)
  {
    char buf1[80];
    strcpy(buf1, ConvertCase(FileNameFromPath(FileRoot)));
    sprintf(buf, "\n<A HREF=\"%s.%s\">%s</A> ", buf1, ConvertCase("htm"), contentsReference);
  }
  else
  {
    char buf1[80];
    strcpy(buf1, ConvertCase(FileNameFromPath(FileRoot)));
    sprintf(buf, "\n<A HREF=\"%s%s\">%s</A> ", buf1, ConvertCase("_contents.html"), contentsReference);
  }
//  TexOutput("<NOFRAMES>");
  TexOutput(buf);
//  TexOutput("</NOFRAMES>");

  /*
   * Up button
   *
   */

  if (upLabel && upFilename)
  {
    if (strlen(upLabel) > 0)
      sprintf(buf, "<A HREF=\"%s#%s\">%s</A> ", ConvertCase(upFilename), upLabel, upReference);
    else
      sprintf(buf, "<A HREF=\"%s\">%s</A> ", ConvertCase(upFilename), upReference);
    if (strcmp(upLabel, "contents") == 0)
    {
//      TexOutput("<NOFRAMES>");
      TexOutput(buf);
//      TexOutput("</NOFRAMES>");
    }
    else
     TexOutput(buf);
  }

  /*
   * << button
   *
   */

  if (previousLabel && previousFilename)
  {
    sprintf(buf, "<A HREF=\"%s#%s\">%s</A> ", ConvertCase(previousFilename), previousLabel, backReference);
    if (strcmp(previousLabel, "contents") == 0)
    {
//      TexOutput("<NOFRAMES>");
      TexOutput(buf);
//      TexOutput("</NOFRAMES>");
    }
    else
      TexOutput(buf);
  }
  else
  {
    // A placeholder so the buttons don't keep moving position
    sprintf(buf, "%s ", backReference);
    TexOutput(buf);
  }

  char *nextLabel = NULL;
  char *nextFilename = NULL;

  // Get the next page, and record the previous page's 'next' page
  // (i.e. this page)
  TexNextPage *nextPage = (TexNextPage *)TexNextPages.Get(thisLabel);
  if (nextPage)
  {
    nextLabel = nextPage->label;
    nextFilename = nextPage->filename;
  }
  if (previousLabel && previousFilename)
  {
    TexNextPage *oldNextPage = (TexNextPage *)TexNextPages.Get(previousLabel);
    if (oldNextPage)
    {
      delete oldNextPage;
      TexNextPages.Delete(previousLabel);
    }
    TexNextPage *newNextPage = new TexNextPage(thisLabel, thisFilename);
    TexNextPages.Put(previousLabel, newNextPage);
  }

  /*
   * >> button
   *
   */

  if (nextLabel && nextFilename)
  {
    sprintf(buf, "<A HREF=\"%s#%s\">%s</A> ", ConvertCase(nextFilename), nextLabel, forwardReference);
    TexOutput(buf);
  }
  else
  {
    // A placeholder so the buttons don't keep moving position
    sprintf(buf, "%s ", forwardReference);
    TexOutput(buf);
  }

  /*
   * Horizontal rule to finish it off nicely.
   *
   */
  TexOutput("</CENTER>");
  TexOutput("<HR>\n");

  // Update last topic/filename
  if (lastFileName)
    delete[] lastFileName;
  lastFileName = copystring(thisFilename);
  if (lastTopic)
    delete[] lastTopic;
  lastTopic = copystring(thisLabel);
}

// A colour string is either 3 numbers separated by semicolons (RGB),
// or a reference to a GIF. Return the filename or a hex string like #934CE8
char *ParseColourString(char *bkStr, bool *isPicture)
{
  static char resStr[300];
  strcpy(resStr, bkStr);
  char *tok1 = strtok(resStr, ";");
  char *tok2 = strtok(NULL, ";");
  if (tok1)
  {
    if (!tok2)
    {
      *isPicture = TRUE;
      return resStr;
    }
    else
    {
      *isPicture = FALSE;
      char *tok3 = strtok(NULL, ";");
      if (tok3)
      {
        // Now convert 3 strings into decimal numbers, and then hex numbers.
        int red = atoi(tok1);
        int green = atoi(tok2);
        int blue = atoi(tok3);

        strcpy(resStr, "#");

        char buf[3];
        DecToHex(red, buf);
        strcat(resStr, buf);
        DecToHex(green, buf);
        strcat(resStr, buf);
        DecToHex(blue, buf);
        strcat(resStr, buf);
        return resStr;
      }
      else return NULL;
    }
  }
  else return NULL;
}

// Output start of <BODY> block
void OutputBodyStart(void)
{
  TexOutput("\n<BODY");
  if (backgroundImageString)
  {
    bool isPicture = FALSE;
    char *s = ParseColourString(backgroundImageString, &isPicture);
    if (s)
    {
      TexOutput(" BACKGROUND=\""); TexOutput(s); TexOutput("\"");
    }
  }
  if (backgroundColourString)
  {
    bool isPicture = FALSE;
    char *s = ParseColourString(backgroundColourString, &isPicture);
    if (s)
    {
      TexOutput(" BGCOLOR="); TexOutput(s);
    }
  }

  // Set foreground text colour, if one is specified
  if (textColourString)
  {
    bool isPicture = FALSE;
    char *s = ParseColourString(textColourString, &isPicture);
    if (s)
    {
      TexOutput(" TEXT="); TexOutput(s);
    }
  }
  // Set link text colour, if one is specified
  if (linkColourString)
  {
    bool isPicture = FALSE;
    char *s = ParseColourString(linkColourString, &isPicture);
    if (s)
    {
      TexOutput(" LINK="); TexOutput(s);
    }
  }
  // Set followed link text colour, if one is specified
  if (followedLinkColourString)
  {
    bool isPicture = FALSE;
    char *s = ParseColourString(followedLinkColourString, &isPicture);
    if (s)
    {
      TexOutput(" VLINK="); TexOutput(s);
    }
  }
  TexOutput(">\n");
}

// Called on start/end of macro examination
void HTMLOnMacro(int macroId, int no_args, bool start)
{
  switch (macroId)
  {
  case ltCHAPTER:
  case ltCHAPTERSTAR:
  case ltCHAPTERHEADING:
  {
    if (!start)
    {
      sectionNo = 0;
      figureNo = 0;
      subsectionNo = 0;
      subsubsectionNo = 0;
      if (macroId != ltCHAPTERSTAR)
        chapterNo ++;

      SetCurrentOutput(NULL);
      startedSections = TRUE;

      char *topicName = FindTopicName(GetNextChunk());
      ReopenFile(&Chapters, &ChaptersName);
      AddTexRef(topicName, ChaptersName, ChapterNameString);

      SetCurrentChapterName(topicName, ChaptersName);
      if (htmlWorkshopFiles) HTMLWorkshopAddToContents(0, topicName, ChaptersName);

      SetCurrentOutput(Chapters);

      TexOutput("<head><title>");
      OutputCurrentSection(); // Repeat section header
      TexOutput("</title></head>\n");
      OutputBodyStart();

      char titleBuf[200];
      if (truncateFilenames)
        sprintf(titleBuf, "%s.htm", FileNameFromPath(FileRoot));
      else
        sprintf(titleBuf, "%s_contents.html", FileNameFromPath(FileRoot));

      fprintf(Chapters, "<A NAME=\"%s\"></A>", topicName);

      AddBrowseButtons("", titleBuf, // Up
                       lastTopic, lastFileName,  // Last topic
                       topicName, ChaptersName); // This topic

      fprintf(Contents, "\n<LI><A HREF=\"%s#%s\">", ConvertCase(ChaptersName), topicName);

      if (htmlFrameContents && FrameContents)
      {
        SetCurrentOutput(FrameContents);
        fprintf(FrameContents, "\n<LI><A HREF=\"%s#%s\" TARGET=\"mainwindow\">", ConvertCase(ChaptersName), topicName);
        OutputCurrentSection();
        fprintf(FrameContents, "</A>\n");
      }

      SetCurrentOutputs(Contents, Chapters);
      fprintf(Chapters, "\n<H2>");
      OutputCurrentSection();
      fprintf(Contents, "</A>\n");
      fprintf(Chapters, "</H2>\n");

      SetCurrentOutput(Chapters);

      // Add this section title to the list of keywords
      if (htmlIndex)
      {
        OutputCurrentSectionToString(wxBuffer);
        AddKeyWordForTopic(topicName, wxBuffer, ConvertCase(currentFileName));
      }
    }
    break;
  }
  case ltSECTION:
  case ltSECTIONSTAR:
  case ltSECTIONHEADING:
  case ltGLOSS:
  {
    if (!start)
    {
      subsectionNo = 0;
      subsubsectionNo = 0;
      subsectionStarted = FALSE;

      if (macroId != ltSECTIONSTAR)
        sectionNo ++;

      SetCurrentOutput(NULL);
      startedSections = TRUE;

      char *topicName = FindTopicName(GetNextChunk());
      ReopenFile(&Sections, &SectionsName);
      AddTexRef(topicName, SectionsName, SectionNameString);

      SetCurrentSectionName(topicName, SectionsName);
      if (htmlWorkshopFiles) HTMLWorkshopAddToContents(1, topicName, SectionsName);

      SetCurrentOutput(Sections);
      TexOutput("<head><title>");
      OutputCurrentSection();
      TexOutput("</title></head>\n");
      OutputBodyStart();

      fprintf(Sections, "<A NAME=\"%s\"></A>", topicName);
      AddBrowseButtons(CurrentChapterName, CurrentChapterFile, // Up
                       lastTopic, lastFileName,  // Last topic
                       topicName, SectionsName); // This topic

      FILE *jumpFrom = ((DocumentStyle == LATEX_ARTICLE) ? Contents : Chapters);

      SetCurrentOutputs(jumpFrom, Sections);
      if (DocumentStyle == LATEX_ARTICLE)
        fprintf(jumpFrom, "\n<LI><A HREF=\"%s#%s\">", ConvertCase(SectionsName), topicName);
      else
        fprintf(jumpFrom, "\n<A HREF=\"%s#%s\"><B>", ConvertCase(SectionsName), topicName);

      fprintf(Sections, "\n<H2>");
      OutputCurrentSection();

      if (DocumentStyle == LATEX_ARTICLE)
        fprintf(jumpFrom, "</A>\n");
      else
        fprintf(jumpFrom, "</B></A><BR>\n");
      fprintf(Sections, "</H2>\n");

      SetCurrentOutput(Sections);
      // Add this section title to the list of keywords
      if (htmlIndex)
      {
        OutputCurrentSectionToString(wxBuffer);
        AddKeyWordForTopic(topicName, wxBuffer, currentFileName);
      }
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
      if (!Sections)
      {
        OnError("You cannot have a subsection before a section!");
      }
      else
      {
          subsubsectionNo = 0;

          if (macroId != ltSUBSECTIONSTAR)
            subsectionNo ++;

          if ( combineSubSections && !subsectionStarted )
          {
            // Read old .con file in at this point
            char buf[256];
            strcpy(buf, CurrentSectionFile);
            wxStripExtension(buf);
            strcat(buf, ".con");
            FILE *fd = fopen(buf, "r");
            if ( fd )
            {
                int ch = getc(fd);
                while (ch != EOF)
                {
                    putc(ch, Sections);
                    ch = getc(fd);
                }
                fclose(fd);
            }
            fprintf(Sections, "<P>\n");

            // Close old file, create a new file for the sub(sub)section contents entries
            ReopenSectionContentsFile();
          }

          startedSections = TRUE;
          subsectionStarted = TRUE;

          char *topicName = FindTopicName(GetNextChunk());

          if ( !combineSubSections )
          {
            SetCurrentOutput(NULL);
            ReopenFile(&Subsections, &SubsectionsName);
            AddTexRef(topicName, SubsectionsName, SubsectionNameString);
            SetCurrentSubsectionName(topicName, SubsectionsName);
            if (htmlWorkshopFiles) HTMLWorkshopAddToContents(2, topicName, SubsectionsName);
            SetCurrentOutput(Subsections);

            TexOutput("<head><title>");
            OutputCurrentSection();
            TexOutput("</title></head>\n");
            OutputBodyStart();

            fprintf(Subsections, "<A NAME=\"%s\"></A>", topicName);
            AddBrowseButtons(CurrentSectionName, CurrentSectionFile, // Up
                           lastTopic, lastFileName,  // Last topic
                           topicName, SubsectionsName); // This topic

            SetCurrentOutputs(Sections, Subsections);
            fprintf(Sections, "\n<A HREF=\"%s#%s\"><B>", ConvertCase(SubsectionsName), topicName);

            fprintf(Subsections, "\n<H3>");
            OutputCurrentSection();
            fprintf(Sections, "</B></A><BR>\n");
            fprintf(Subsections, "</H3>\n");

            SetCurrentOutput(Subsections);
          }
          else
          {
            AddTexRef(topicName, SectionsName, SubsectionNameString);
            SetCurrentSubsectionName(topicName, SectionsName);

//            if ( subsectionNo != 0 )
            fprintf(Sections, "\n<HR>\n");

            // We're putting everything into the section file
            fprintf(Sections, "<A NAME=\"%s\"></A>", topicName);
            fprintf(Sections, "\n<H3>");
            OutputCurrentSection();
            fprintf(Sections, "</H3>\n");

            SetCurrentOutput(SectionContentsFD);
            fprintf(SectionContentsFD, "<A HREF=\"#%s\">", topicName);
            OutputCurrentSection();
            TexOutput("</A><BR>\n");

            if (htmlWorkshopFiles) HTMLWorkshopAddToContents(2, topicName, SectionsName);
            SetCurrentOutput(Sections);
          }
          // Add this section title to the list of keywords
          if (htmlIndex)
          {
            OutputCurrentSectionToString(wxBuffer);
            AddKeyWordForTopic(topicName, wxBuffer, currentFileName);
          }

      }
    }
    break;
  }
  case ltSUBSUBSECTION:
  case ltSUBSUBSECTIONSTAR:
  {
    if (!start)
    {
      if (!Subsections && !combineSubSections)
      {
        OnError("You cannot have a subsubsection before a subsection!");
      }
      else
      {
        if (macroId != ltSUBSUBSECTIONSTAR)
          subsubsectionNo ++;

        startedSections = TRUE;

        char *topicName = FindTopicName(GetNextChunk());

        if ( !combineSubSections )
        {
            SetCurrentOutput(NULL);
            ReopenFile(&Subsubsections, &SubsubsectionsName);
            AddTexRef(topicName, SubsubsectionsName, SubsubsectionNameString);
            SetCurrentSubsubsectionName(topicName, SubsubsectionsName);
            if (htmlWorkshopFiles) HTMLWorkshopAddToContents(3, topicName, SubsubsectionsName);

            SetCurrentOutput(Subsubsections);
            TexOutput("<head><title>");
            OutputCurrentSection();
            TexOutput("</title></head>\n");
            OutputBodyStart();

            fprintf(Subsubsections, "<A NAME=\"%s\"></A>", topicName);

            AddBrowseButtons(CurrentSubsectionName, CurrentSubsectionFile, // Up
                         lastTopic, lastFileName,  // Last topic
                         topicName, SubsubsectionsName); // This topic

            SetCurrentOutputs(Subsections, Subsubsections);
            fprintf(Subsections, "\n<A HREF=\"%s#%s\"><B>", ConvertCase(SubsubsectionsName), topicName);

            fprintf(Subsubsections, "\n<H3>");
            OutputCurrentSection();
            fprintf(Subsections, "</B></A><BR>\n");
            fprintf(Subsubsections, "</H3>\n");
        }
        else
        {
            AddTexRef(topicName, SectionsName, SubsubsectionNameString);
            SetCurrentSubsectionName(topicName, SectionsName);
            fprintf(Sections, "\n<HR>\n");

            // We're putting everything into the section file
            fprintf(Sections, "<A NAME=\"%s\"></A>", topicName);
            fprintf(Sections, "\n<H3>");
            OutputCurrentSection();
            fprintf(Sections, "</H3>\n");
/* TODO: where do we put subsubsection contents entry - indented, with subsection entries?
            SetCurrentOutput(SectionContentsFD);
            fprintf(SectionContentsFD, "<A HREF=\"#%s\">", topicName);
            OutputCurrentSection();
            TexOutput("</A><BR>");
*/
            if (htmlWorkshopFiles) HTMLWorkshopAddToContents(2, topicName, SectionsName);
            SetCurrentOutput(Sections);
        }

        // Add this section title to the list of keywords
        if (htmlIndex)
        {
          OutputCurrentSectionToString(wxBuffer);
          AddKeyWordForTopic(topicName, wxBuffer, currentFileName);
        }
      }
    }
    break;
  }
  case ltFUNC:
  case ltPFUNC:
  {
    if ( !combineSubSections )
        SetCurrentOutput(Subsections);
    else
        SetCurrentOutput(Sections);
    if (start)
    {
    }
    else
    {
    }
    break;
  }
  case ltCLIPSFUNC:
  {
    if ( !combineSubSections )
        SetCurrentOutput(Subsections);
    else
        SetCurrentOutput(Sections);
    if (start)
    {
    }
    else
    {
    }
    break;
  }
  case ltMEMBER:
  {
    if ( !combineSubSections )
        SetCurrentOutput(Subsections);
    else
        SetCurrentOutput(Sections);
    if (start)
    {
    }
    else
    {
    }
    break;
  }
  case ltVOID:
//    if (start)
//      TexOutput("<B>void</B>");
    break;
  case ltHARDY:
    if (start)
      TexOutput("HARDY");
    break;
  case ltWXCLIPS:
    if (start)
      TexOutput("wxCLIPS");
    break;
  case ltAMPERSAND:
    if (start)
      TexOutput("&amp;");
    break;
  case ltSPECIALAMPERSAND:
  {
    if (start)
    {
      if (inTabular)
      {
        // End cell, start cell
        TexOutput("</TD>");

        // Start new row and cell, setting alignment for the first cell.
        if (currentColumn < noColumns)
          currentColumn ++;

        char buf[100];
        if (TableData[currentColumn].justification == 'c')
          sprintf(buf, "\n<TD ALIGN=CENTER>");
        else if (TableData[currentColumn].justification == 'r')
          sprintf(buf, "\n<TD ALIGN=RIGHT>");
        else if (TableData[currentColumn].absWidth)
        {
          // Convert from points * 20 into pixels.
          int points = TableData[currentColumn].width / 20;

          // Say the display is 100 DPI (dots/pixels per inch).
          // There are 72 pts to the inch. So 1pt = 1/72 inch, or 100 * 1/72 dots.
          int pixels = (int)(points * 100.0 / 72.0);
          sprintf(buf, "<TD ALIGN=CENTER WIDTH=%d>", pixels);
        }
        else
          sprintf(buf, "\n<TD ALIGN=LEFT>");
        TexOutput(buf);
      }
      else
        TexOutput("&amp;");
    }
    break;
  }
  case ltBACKSLASHCHAR:
  {
    if (start)
    {
      if (inTabular)
      {
        // End row. In fact, tables without use of \row or \ruledrow isn't supported for
        // HTML: the syntax is too different (e.g. how do we know where to put the first </TH>
        // if we've ended the last row?). So normally you wouldn't use \\ to end a row.
        TexOutput("</TR>\n");
      }
      else
        TexOutput("<BR>\n");
    }
    break;
  }
  case ltROW:
  case ltRULEDROW:
  {
    if (start)
    {
      currentColumn = 0;

      // Start new row and cell, setting alignment for the first cell.
      char buf[100];
      if (TableData[currentColumn].justification == 'c')
        sprintf(buf, "<TR>\n<TD ALIGN=CENTER>");
      else if (TableData[currentColumn].justification == 'r')
        sprintf(buf, "<TR>\n<TD ALIGN=RIGHT>");
      else if (TableData[currentColumn].absWidth)
      {
        // Convert from points * 20 into pixels.
        int points = TableData[currentColumn].width / 20;

        // Say the display is 100 DPI (dots/pixels per inch).
        // There are 72 pts to the inch. So 1pt = 1/72 inch, or 100 * 1/72 dots.
        int pixels = (int)(points * 100.0 / 72.0);
        sprintf(buf, "<TR>\n<TD ALIGN=CENTER WIDTH=%d>", pixels);
      }
      else
        sprintf(buf, "<TR>\n<TD ALIGN=LEFT>");
      TexOutput(buf);
    }
    else
    {
      // End cell and row
      // Start new row and cell
      TexOutput("</TD>\n</TR>\n");
    }
    break;
  }
  // HTML-only: break until the end of the picture (both margins are clear).
  case ltBRCLEAR:
  {
    if (start)
      TexOutput("<BR CLEAR=ALL>");
    break;
  }
  case ltRTFSP:  // Explicit space, RTF only
    break;
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
  case ltINDENTED :
  {
    if ( start )
        TexOutput("<UL><UL>\n");
    else
        TexOutput("</UL></UL>\n");
    break;
  }
  case ltITEMIZE:
  case ltENUMERATE:
  case ltDESCRIPTION:
//  case ltTWOCOLLIST:
  {
    if (start)
    {
      indentLevel ++;

      int listType;
      if (macroId == ltENUMERATE)
        listType = LATEX_ENUMERATE;
      else if (macroId == ltITEMIZE)
        listType = LATEX_ITEMIZE;
      else
        listType = LATEX_DESCRIPTION;

      itemizeStack.Insert(new ItemizeStruc(listType));
      switch (listType)
      {
        case LATEX_ITEMIZE:
          TexOutput("<UL>\n");
          break;
        case LATEX_ENUMERATE:
          TexOutput("<OL>\n");
          break;
        case LATEX_DESCRIPTION:
        default:
          TexOutput("<DL>\n");
          break;
      }
    }
    else
    {
      indentLevel --;
      if (itemizeStack.First())
      {
        ItemizeStruc *struc = (ItemizeStruc *)itemizeStack.First()->Data();
        switch (struc->listType)
        {
          case LATEX_ITEMIZE:
            TexOutput("</UL>\n");
            break;
          case LATEX_ENUMERATE:
            TexOutput("</OL>\n");
            break;
          case LATEX_DESCRIPTION:
          default:
            TexOutput("</DL>\n");
            break;
        }

        delete struc;
        delete itemizeStack.First();
      }
    }
    break;
  }
  case ltTWOCOLLIST :
  {
    if ( start )
        TexOutput("\n<TABLE>\n");
    else
        TexOutput("\n</TABLE>\n");
    break;
  }
  case ltPAR:
  {
    if (start)
      TexOutput("<P>\n");
    break;
  }
/* For footnotes we need to output the text at the bottom of the page and
 * insert a reference to it. Is it worth the trouble...
  case ltFOOTNOTE:
  case ltFOOTNOTEPOPUP:
  {
    if (start)
    {
      TexOutput("<FN>);
    }
    else TexOutput("</FN>");
    break;
  }
*/
  case ltVERB:
  {
    if (start)
      TexOutput("<TT>");
    else TexOutput("</TT>");
    break;
  }
  case ltVERBATIM:
  {
    if (start)
    {
      char buf[100];
      sprintf(buf, "<PRE>\n");
      TexOutput(buf);
    }
    else TexOutput("</PRE>\n");
    break;
  }
  case ltCENTERLINE:
  case ltCENTER:
  {
    if (start)
    {
      TexOutput("<CENTER>");
    }
    else TexOutput("</CENTER>");
    break;
  }
  case ltFLUSHLEFT:
  {
/*
    if (start)
    {
      TexOutput("{\\ql ");
    }
    else TexOutput("}\\par\\pard\n");
*/
    break;
  }
  case ltFLUSHRIGHT:
  {
/*
    if (start)
    {
      TexOutput("{\\qr ");
    }
    else TexOutput("}\\par\\pard\n");
*/
    break;
  }
  case ltSMALL:
  {
    if (start)
    {
      // Netscape extension
      TexOutput("<FONT SIZE=2>");
    }
    else TexOutput("</FONT>");
    break;
  }
  case ltTINY:
  {
    if (start)
    {
      // Netscape extension
      TexOutput("<FONT SIZE=1>");
    }
    else TexOutput("</FONT>");
    break;
  }
  case ltNORMALSIZE:
  {
    if (start)
    {
      // Netscape extension
      TexOutput("<FONT SIZE=3>");
    }
    else TexOutput("</FONT>");
    break;
  }
  case ltlarge:
  {
    if (start)
    {
      // Netscape extension
      TexOutput("<FONT SIZE=4>");
    }
    else TexOutput("</FONT>");
    break;
  }
  case ltLarge:
  {
    if (start)
    {
      // Netscape extension
      TexOutput("<FONT SIZE=5>");
    }
    else TexOutput("</FONT>");
    break;
  }
  case ltLARGE:
  {
    if (start)
    {
      // Netscape extension
      TexOutput("<FONT SIZE=6>");
    }
    else TexOutput("</FONT>");
    break;
  }
  case ltBFSERIES:
  case ltTEXTBF:
  case ltBF:
  {
    if (start)
    {
      TexOutput("<B>");
    }
    else TexOutput("</B>");
    break;
  }
  case ltITSHAPE:
  case ltTEXTIT:
  case ltIT:
  {
    if (start)
    {
      TexOutput("<I>");
    }
    else TexOutput("</I>");
    break;
  }
  case ltEMPH:
  case ltEM:
  {
    if (start)
    {
      TexOutput("<EM>");
    }
    else TexOutput("</EM>");
    break;
  }
  case ltUNDERLINE:
  {
    if (start)
    {
      TexOutput("<UL>");
    }
    else TexOutput("</UL>");
    break;
  }
  case ltTTFAMILY:
  case ltTEXTTT:
  case ltTT:
  {
    if (start)
    {
      TexOutput("<TT>");
    }
    else TexOutput("</TT>");
    break;
  }
  case ltCOPYRIGHT:
  {
    if (start)
      TexOutput("&copy;", TRUE);
    break;
  }
  case ltREGISTERED:
  {
    if (start)
      TexOutput("&reg;", TRUE);
    break;
  }
  // Arrows
  case ltLEFTARROW:
  {
    if (start) TexOutput("&lt;--");
    break;
  }
  case ltLEFTARROW2:
  {
    if (start) TexOutput("&lt;==");
    break;
  }
  case ltRIGHTARROW:
  {
      if (start) TexOutput("--&gt;");
      break;
  }
  case ltRIGHTARROW2:
  {
    if (start) TexOutput("==&gt;");
    break;
  }
  case ltLEFTRIGHTARROW:
  {
    if (start) TexOutput("&lt;--&gt;");
    break;
  }
  case ltLEFTRIGHTARROW2:
  {
    if (start) TexOutput("&lt;==&gt;");
    break;
  }
/*
  case ltSC:
  {
    break;
  }
*/
  case ltITEM:
  {
    if (!start)
    {
      wxNode *node = itemizeStack.First();
      if (node)
      {
        ItemizeStruc *struc = (ItemizeStruc *)node->Data();
        struc->currentItem += 1;
        if (struc->listType == LATEX_DESCRIPTION)
        {
          if (descriptionItemArg)
          {
            TexOutput("<DT> ");
            TraverseChildrenFromChunk(descriptionItemArg);
            TexOutput("\n");
            descriptionItemArg = NULL;
          }
          TexOutput("<DD>");
        }
        else
          TexOutput("<LI>");
      }
    }
    break;
  }
  case ltMAKETITLE:
  {
    if (start && DocumentTitle && DocumentAuthor)
    {
      // Add a special label for the contents page.
//      TexOutput("<CENTER>\n");
      TexOutput("<A NAME=\"contents\">");
      TexOutput("<H2 ALIGN=CENTER>\n");
      TraverseChildrenFromChunk(DocumentTitle);
      TexOutput("</H2>");
      TexOutput("<P>");
      TexOutput("</A>\n");
      TexOutput("<P>\n\n");
      TexOutput("<H3 ALIGN=CENTER>");
      TraverseChildrenFromChunk(DocumentAuthor);
      TexOutput("</H3><P>\n\n");
      if (DocumentDate)
      {
        TexOutput("<H3 ALIGN=CENTER>");
        TraverseChildrenFromChunk(DocumentDate);
        TexOutput("</H3><P>\n\n");
      }
//      TexOutput("\n</CENTER>\n");
      TexOutput("\n<P><HR><P>\n");

/*
      // Now do optional frame contents page
      if (htmlFrameContents && FrameContents)
      {
        SetCurrentOutput(FrameContents);

        // Add a special label for the contents page.
        TexOutput("<CENTER>\n");
        TexOutput("<H3>\n");
        TraverseChildrenFromChunk(DocumentTitle);
        TexOutput("</H3>");
        TexOutput("<P>");
        TexOutput("</A>\n");
        TexOutput("<P>\n\n");
        TexOutput("<H3>");
        TraverseChildrenFromChunk(DocumentAuthor);
        TexOutput("</H3><P>\n\n");
        if (DocumentDate)
        {
          TexOutput("<H4>");
          TraverseChildrenFromChunk(DocumentDate);
          TexOutput("</H4><P>\n\n");
        }
        TexOutput("\n</CENTER>\n");
        TexOutput("<P><HR><P>\n");

        SetCurrentOutput(Titlepage);
      }
*/
    }
    break;
  }
  case ltHELPREF:
  case ltHELPREFN:
  case ltPOPREF:
  case ltURLREF:
  {
    if (start)
    {
      helpRefFilename = NULL;
      helpRefText = NULL;
    }
    break;
  }
  case ltBIBLIOGRAPHY:
  {
    if (start)
    {
      DefaultOnMacro(macroId, no_args, start);
    }
    else
    {
      DefaultOnMacro(macroId, no_args, start);
      TexOutput("</DL>\n");
    }
    break;
  }
  case ltHRULE:
  {
    if (start)
    {
      TexOutput("<HR>\n");
    }
    break;
  }
  case ltRULE:
  {
    if (start)
    {
      TexOutput("<HR>\n");
    }
    break;
  }
  case ltTABLEOFCONTENTS:
  {
    if (start)
    {
      FILE *fd = fopen(ContentsName, "r");
      if (fd)
      {
        int ch = getc(fd);
        while (ch != EOF)
        {
          putc(ch, Titlepage);
          ch = getc(fd);
        }
        fclose(fd);
      }
      else
      {
        TexOutput("RUN TEX2RTF AGAIN FOR CONTENTS PAGE\n");
        OnInform("Run Tex2RTF again to include contents page.");
      }
    }
    break;
  }
  case ltLANGLEBRA:
  {
    if (start)
      TexOutput("&lt;");
    break;
  }
  case ltRANGLEBRA:
  {
    if (start)
      TexOutput("&gt;");
    break;
  }
  case ltQUOTE:
  case ltQUOTATION:
  {
    if (start)
      TexOutput("<BLOCKQUOTE>");
    else
      TexOutput("</BLOCKQUOTE>");
    break;
  }
  case ltCAPTION:
  case ltCAPTIONSTAR:
  {
    if (start)
    {
      if (inTabular)
        TexOutput("\n<CAPTION>");

      char figBuf[40];

      if ( inFigure )
      {
          figureNo ++;

          if (DocumentStyle != LATEX_ARTICLE)
            sprintf(figBuf, "%s %d.%d: ", FigureNameString, chapterNo, figureNo);
          else
            sprintf(figBuf, "%s %d: ", FigureNameString, figureNo);
      }
      else
      {
          tableNo ++;

          if (DocumentStyle != LATEX_ARTICLE)
            sprintf(figBuf, "%s %d.%d: ", TableNameString, chapterNo, tableNo);
          else
            sprintf(figBuf, "%s %d: ", TableNameString, tableNo);
      }

      TexOutput(figBuf);
    }
    else
    {
      if (inTabular)
        TexOutput("\n</CAPTION>\n");

      char *topicName = FindTopicName(GetNextChunk());

      int n = inFigure ? figureNo : tableNo;

      AddTexRef(topicName, NULL, NULL,
           ((DocumentStyle != LATEX_ARTICLE) ? chapterNo : n),
            ((DocumentStyle != LATEX_ARTICLE) ? n : 0));
    }
    break;
  }
  case ltSS:
  {
    if (start) TexOutput("&szlig;");
    break;
  }
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
    DefaultOnMacro(macroId, no_args, start);
    break;
  }
}

// Called on start/end of argument examination
bool HTMLOnArgument(int macroId, int arg_no, bool start)
{
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
  {
    if (!start && (arg_no == 1))
      currentSection = GetArgChunk();
    return FALSE;
    break;
  }
  case ltFUNC:
  {
    if (start && (arg_no == 1))
      TexOutput("<B>");

    if (!start && (arg_no == 1))
      TexOutput("</B> ");

    if (start && (arg_no == 2))
    {
      if (!suppressNameDecoration) TexOutput("<B>");
      currentMember = GetArgChunk();
    }
    if (!start && (arg_no == 2))
    {
      if (!suppressNameDecoration) TexOutput("</B>");
    }

    if (start && (arg_no == 3))
      TexOutput("(");
    if (!start && (arg_no == 3))
     TexOutput(")");
   break;
  }
  case ltCLIPSFUNC:
  {
    if (start && (arg_no == 1))
      TexOutput("<B>");
    if (!start && (arg_no == 1))
      TexOutput("</B> ");

    if (start && (arg_no == 2))
    {
      if (!suppressNameDecoration) TexOutput("( ");
      currentMember = GetArgChunk();
    }
    if (!start && (arg_no == 2))
    {
    }

    if (!start && (arg_no == 3))
     TexOutput(")");
    break;
  }
  case ltPFUNC:
  {
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
      TexOutput(")");
    break;
  }
  case ltPARAM:
  {
    if (start && (arg_no == 1))
      TexOutput("<B>");
    if (!start && (arg_no == 1))
      TexOutput("</B>");
    if (start && (arg_no == 2))
    {
      TexOutput("<I>");
    }
    if (!start && (arg_no == 2))
    {
      TexOutput("</I>");
    }
    break;
  }
  case ltCPARAM:
  {
    if (start && (arg_no == 1))
      TexOutput("<B>");
    if (!start && (arg_no == 1))
      TexOutput("</B> ");  // This is the difference from param - one space!
    if (start && (arg_no == 2))
    {
      TexOutput("<I>");
    }
    if (!start && (arg_no == 2))
    {
      TexOutput("</I>");
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

      char *refName = GetArgData();
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
      return FALSE;
    }
    break;
  }
  case ltURLREF:
  {
    if (IsArgOptional())
      return FALSE;
    else if ((GetNoArgs() - arg_no) == 1)
    {
      if (start)
        helpRefText = GetArgChunk();
      return FALSE;
    }
    else if ((GetNoArgs() - arg_no) == 0) // Arg = 2, or 3 if first is optional
    {
      if (start)
      {
        TexChunk *ref = GetArgChunk();
        TexOutput("<A HREF=\"");
        inVerbatim = TRUE;
        TraverseChildrenFromChunk(ref);
        inVerbatim = FALSE;
        TexOutput("\">");
        if (helpRefText)
          TraverseChildrenFromChunk(helpRefText);
        TexOutput("</A>");
      }
      return FALSE;
    }
    break;
  }
  case ltHELPREF:
  case ltHELPREFN:
  case ltPOPREF:
  {
    if (IsArgOptional())
    {
      if (start)
        helpRefFilename = GetArgChunk();
      return FALSE;
    }
    if ((GetNoArgs() - arg_no) == 1)
    {
      if (start)
        helpRefText = GetArgChunk();
      return FALSE;
    }
    else if ((GetNoArgs() - arg_no) == 0) // Arg = 2, or 3 if first is optional
    {
      if (start)
      {
        char *refName = GetArgData();
        char *refFilename = NULL;

        if (refName)
        {
          TexRef *texRef = FindReference(refName);
          if (texRef)
          {
            if (texRef->refFile && strcmp(texRef->refFile, "??") != 0)
              refFilename = texRef->refFile;

            TexOutput("<A HREF=\"");
            // If a filename is supplied, use it, otherwise try to
            // use the filename associated with the reference (from this document).
            if (helpRefFilename)
	        {
              TraverseChildrenFromChunk(helpRefFilename);
              TexOutput("#");
	        }
            else if (refFilename)
	        {
              TexOutput(ConvertCase(refFilename));
              TexOutput("#");
	        }
            TexOutput(refName);
            TexOutput("\">");
            if (helpRefText)
              TraverseChildrenFromChunk(helpRefText);
            TexOutput("</A>");
          }
          else
          {
            if (helpRefText)
              TraverseChildrenFromChunk(helpRefText);
            TexOutput(" (REF NOT FOUND)");
          }
        }
        else TexOutput("??");
      }
      return FALSE;
    }
    break;
  }
  case ltIMAGE:
  case ltIMAGEL:
  case ltIMAGER:
  case ltPSBOXTO:
  {
    if (arg_no == 2)
    {
      if (start)
      {
        char *alignment = "";
        if (macroId == ltIMAGEL)
          alignment = " align=left";
        else if  (macroId == ltIMAGER)
          alignment = " align=right";

        // Try to find an XBM or GIF image first.
        char *filename = copystring(GetArgData());
        char buf[500];

        strcpy(buf, filename);
        StripExtension(buf);
        strcat(buf, ".xbm");
        wxString f = TexPathList.FindValidPath(buf);

        if (f == "") // Try for a GIF instead
        {
          strcpy(buf, filename);
          StripExtension(buf);
          strcat(buf, ".gif");
          f = TexPathList.FindValidPath(buf);
        }

        if (f == "") // Try for a JPEG instead
        {
          strcpy(buf, filename);
          StripExtension(buf);
          strcat(buf, ".jpg");
          f = TexPathList.FindValidPath(buf);
        }

        if (f == "") // Try for a PNG instead
        {
          strcpy(buf, filename);
          StripExtension(buf);
          strcat(buf, ".png");
          f = TexPathList.FindValidPath(buf);
        }

        if (f != "")
        {
          char *inlineFilename = copystring(f);
#if 0
          char *originalFilename = TexPathList.FindValidPath(filename);
          // If we have found the existing filename, make the inline
          // image point to the original file (could be PS, for example)
          if (originalFilename && (strcmp(inlineFilename, originalFilename) != 0))
	  {
            TexOutput("<A HREF=\"");
            TexOutput(ConvertCase(originalFilename));
            TexOutput("\">");
            TexOutput("<img src=\"");
            TexOutput(ConvertCase(wxFileNameFromPath(inlineFilename)));
            TexOutput("\""); TexOutput(alignment); TexOutput("></A>");
	  }
          else
#endif
	  {
            TexOutput("<img src=\"");
            TexOutput(ConvertCase(wxFileNameFromPath(inlineFilename)));
            TexOutput("\""); TexOutput(alignment); TexOutput("></A>");
            delete[] inlineFilename;
	  }
        }
        else
        {
          // Last resort - a link to a PS file.
          TexOutput("<A HREF=\"");
          TexOutput(ConvertCase(wxFileNameFromPath(filename)));
          TexOutput("\">Picture</A>\n");
          sprintf(buf, "Warning: could not find an inline XBM/GIF for %s.", filename);
          OnInform(buf);
        }
      }
    }
    return FALSE;
    break;
  }
  // First arg is PSBOX spec (ignored), second is image file, third is map name.
  case ltIMAGEMAP:
  {
    static char *imageFile = NULL;
    if (start && (arg_no == 2))
    {
      // Try to find an XBM or GIF image first.
      char *filename = copystring(GetArgData());
      char buf[500];

      strcpy(buf, filename);
      StripExtension(buf);
      strcat(buf, ".xbm");
      wxString f = TexPathList.FindValidPath(buf);

      if (f == "") // Try for a GIF instead
      {
        strcpy(buf, filename);
        StripExtension(buf);
        strcat(buf, ".gif");
        f = TexPathList.FindValidPath(buf);
      }
      if (f == "")
      {
        char buf[300];
        sprintf(buf, "Warning: could not find an inline XBM/GIF for %s.", filename);
        OnInform(buf);
      }
      delete[] filename;
      if (imageFile)
        delete[] imageFile;
      imageFile = NULL;
      if (!f.IsEmpty())
      {
        imageFile = copystring(f);
      }
    }
    else if (start && (arg_no == 3))
    {
      if (imageFile)
      {
        // First, try to find a .shg (segmented hypergraphics file)
        // that we can convert to a map file
        char buf[256];
        strcpy(buf, imageFile);
        StripExtension(buf);
        strcat(buf, ".shg");
        wxString f = TexPathList.FindValidPath(buf);

        if (f != "")
        {
          // The default HTML file to go to is THIS file (so a no-op)
          SHGToMap((char*) (const char*) f, currentFileName);
        }

        char *mapName = GetArgData();
        TexOutput("<A HREF=\"/cgi-bin/imagemap/");
        if (mapName)
          TexOutput(mapName);
        else
          TexOutput("unknown");
        TexOutput("\">");
        TexOutput("<img src=\"");
        TexOutput(ConvertCase(wxFileNameFromPath(imageFile)));
        TexOutput("\" ISMAP></A><P>");
        delete[] imageFile;
        imageFile = NULL;
      }
    }
    return FALSE;
    break;
  }
  case ltINDENTED :
  {
    if ( arg_no == 1 )
        return FALSE;
    else
    {
        return TRUE;
    }
  }
  case ltITEM:
  {
    if (start)
    {
      descriptionItemArg = GetArgChunk();
      return FALSE;
    }
  }
  case ltTWOCOLITEM:
  case ltTWOCOLITEMRULED:
  {
/*
    if (start && (arg_no == 1))
      TexOutput("\n<DT> ");
    if (start && (arg_no == 2))
      TexOutput("<DD> ");
*/
    if (arg_no == 1)
    {
        if ( start )
            TexOutput("\n<TR><TD VALIGN=TOP>\n");
        else
            TexOutput("\n</TD>\n");
    }
    if (arg_no == 2)
    {
        if ( start )
            TexOutput("\n<TD VALIGN=TOP>\n");
        else
            TexOutput("\n</TD></TR>\n");
    }
    return TRUE;
    break;
  }
  case ltNUMBEREDBIBITEM:
  {
    if (arg_no == 1 && start)
    {
      TexOutput("\n<DT> ");
    }
    if (arg_no == 2 && !start)
      TexOutput("<P>\n");
    break;
  }
  case ltBIBITEM:
  {
    char buf[100];
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

      sprintf(buf, "\n<DT> [%d] ", citeCount);
      TexOutput(buf);
      citeCount ++;
      return FALSE;
    }
    if (arg_no == 2 && !start)
      TexOutput("<P>\n");
    return TRUE;
    break;
  }
  case ltMARGINPAR:
  case ltMARGINPARODD:
  case ltMARGINPAREVEN:
  case ltNORMALBOX:
  case ltNORMALBOXD:
  {
    if (start)
    {
      TexOutput("<HR>\n");
      return TRUE;
    }
    else
      TexOutput("<HR><P>\n");
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
           TexOutput("&agrave;");
           break;
          case 'e':
           TexOutput("&egrave;");
           break;
          case 'i':
           TexOutput("&igrave;");
           break;
          case 'o':
           TexOutput("&ograve;");
           break;
          case 'u':
           TexOutput("&ugrave;");
           break;
          case 'A':
           TexOutput("&Agrave;");
           break;
          case 'E':
           TexOutput("&Egrave;");
           break;
          case 'I':
           TexOutput("&Igrave;");
           break;
          case 'O':
           TexOutput("&Ograve;");
           break;
          case 'U':
           TexOutput("&Igrave;");
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
           TexOutput("&aacute;");
           break;
          case 'e':
           TexOutput("&eacute;");
           break;
          case 'i':
           TexOutput("&iacute;");
           break;
          case 'o':
           TexOutput("&oacute;");
           break;
          case 'u':
           TexOutput("&uacute;");
           break;
          case 'y':
           TexOutput("&yacute;");
           break;
          case 'A':
           TexOutput("&Aacute;");
           break;
          case 'E':
           TexOutput("&Eacute;");
           break;
          case 'I':
           TexOutput("&Iacute;");
           break;
          case 'O':
           TexOutput("&Oacute;");
           break;
          case 'U':
           TexOutput("&Uacute;");
           break;
          case 'Y':
           TexOutput("&Yacute;");
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
           TexOutput("&acirc;");
           break;
          case 'e':
           TexOutput("&ecirc;");
           break;
          case 'i':
           TexOutput("&icirc;");
           break;
          case 'o':
           TexOutput("&ocirc;");
           break;
          case 'u':
           TexOutput("&ucirc;");
           break;
          case 'A':
           TexOutput("&Acirc;");
           break;
          case 'E':
           TexOutput("&Ecirc;");
           break;
          case 'I':
           TexOutput("&Icirc;");
           break;
          case 'O':
           TexOutput("&Ocirc;");
           break;
          case 'U':
           TexOutput("&Icirc;");
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
          case ' ':
           TexOutput("~");
           break;
          case 'a':
           TexOutput("&atilde;");
           break;
          case 'n':
           TexOutput("&ntilde;");
           break;
          case 'o':
           TexOutput("&otilde;");
           break;
          case 'A':
           TexOutput("&Atilde;");
           break;
          case 'N':
           TexOutput("&Ntilde;");
           break;
          case 'O':
           TexOutput("&Otilde;");
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
           TexOutput("&auml;");
           break;
          case 'e':
           TexOutput("&euml;");
           break;
          case 'i':
           TexOutput("&iuml;");
           break;
          case 'o':
           TexOutput("&ouml;");
           break;
          case 'u':
           TexOutput("&uuml;");
           break;
          case 'y':
           TexOutput("&yuml;");
           break;
          case 'A':
           TexOutput("&Auml;");
           break;
          case 'E':
           TexOutput("&Euml;");
           break;
          case 'I':
           TexOutput("&Iuml;");
           break;
          case 'O':
           TexOutput("&Ouml;");
           break;
          case 'U':
           TexOutput("&Uuml;");
           break;
          case 'Y':
           TexOutput("&Yuml;");
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
           TexOutput("&aring;");
           break;
          case 'A':
           TexOutput("&Aring;");
           break;
          default:
           break;
        }
      }
    }
    return FALSE;
    break;
  }
  case ltBACKGROUND:
  {
    if (start)
    {
      char *val = GetArgData();
      if (val)
      {
        bool isPicture = FALSE;
        char *s = ParseColourString(val, &isPicture);
        if (isPicture)
        {
          if (backgroundImageString)
            delete[] backgroundImageString;
          backgroundImageString = copystring(val);
        }
        else
        {
          if (backgroundColourString)
            delete[] backgroundColourString;
          backgroundColourString = copystring(val);
        }
      }
    }
    return FALSE;
    break;
  }
  case ltBACKGROUNDIMAGE:
  {
    if (start)
    {
      char *val = GetArgData();
      if (val)
      {
        if (backgroundImageString)
          delete[] backgroundImageString;
        backgroundImageString = copystring(val);
      }
    }
    return FALSE;
    break;
  }
  case ltBACKGROUNDCOLOUR:
  {
    if (start)
    {
      char *val = GetArgData();
      if (val)
      {
        if (backgroundColourString)
          delete[] backgroundColourString;
        backgroundColourString = copystring(val);
      }
    }
    return FALSE;
    break;
  }
  case ltTEXTCOLOUR:
  {
    if (start)
    {
      char *val = GetArgData();
      if (val)
      {
        if (textColourString)
          delete[] textColourString;
        textColourString = copystring(val);
      }
    }
    return FALSE;
    break;
  }
  case ltLINKCOLOUR:
  {
    if (start)
    {
      char *val = GetArgData();
      if (val)
      {
        if (linkColourString)
          delete[] linkColourString;
        linkColourString = copystring(val);
      }
    }
    return FALSE;
    break;
  }
  case ltFOLLOWEDLINKCOLOUR:
  {
    if (start)
    {
      char *val = GetArgData();
      if (val)
      {
        if (followedLinkColourString)
          delete[] followedLinkColourString;
        followedLinkColourString = copystring(val);
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
           TexOutput("&ccedil;");
           break;
          case 'C':
           TexOutput("&Ccedil;");
           break;
          default:
           break;
        }
      }
    }
    return FALSE;
    break;
  }
/*
  case ltFOOTNOTE:
  case ltFOOTNOTEPOPUP:
  {
    if (arg_no == 1)
      return TRUE;
    else
      return FALSE;
    break;
  }
*/
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

        TexOutput("<TABLE BORDER>\n");

        // Write the first row formatting for compatibility
        // with standard Latex
        if (compatibilityMode)
        {
          TexOutput("<TR>\n<TD>");
/*
          for (int i = 0; i < noColumns; i++)
          {
            currentWidth += TableData[i].width;
            sprintf(buf, "\\cellx%d", currentWidth);
            TexOutput(buf);
          }
          TexOutput("\\pard\\intbl\n");
*/
        }
        delete[] alignString;

        return FALSE;
      }
    }
    else if (arg_no == 2 && !start)
    {
      TexOutput("</TABLE>\n");
      inTabular = FALSE;
    }
    break;
  }
  case ltTHEBIBLIOGRAPHY:
  {
    if (start && (arg_no == 1))
    {
      ReopenFile(&Chapters, &ChaptersName);
      AddTexRef("bibliography", ChaptersName, "bibliography");
      SetCurrentSubsectionName("bibliography", ChaptersName);

      citeCount = 1;

      SetCurrentOutput(Chapters);

      char titleBuf[150];
      if (truncateFilenames)
        sprintf(titleBuf, "%s.htm", FileNameFromPath(FileRoot));
      else
        sprintf(titleBuf, "%s_contents.html", FileNameFromPath(FileRoot));

      TexOutput("<head><title>");
      TexOutput(ReferencesNameString);
      TexOutput("</title></head>\n");
      OutputBodyStart();

      fprintf(Chapters, "<A NAME=\"%s\">\n<H2>%s", "bibliography", ReferencesNameString);
      AddBrowseButtons("contents", titleBuf, // Up
                       lastTopic, lastFileName,  // Last topic
                       "bibliography", ChaptersName); // This topic

      SetCurrentOutputs(Contents, Chapters);
      fprintf(Contents, "\n<LI><A HREF=\"%s#%s\">", ConvertCase(ChaptersName), "bibliography");

      fprintf(Contents, "%s</A>\n", ReferencesNameString);
      fprintf(Chapters, "</H2>\n</A>\n");

      SetCurrentOutput(Chapters);
      return FALSE;
    }
    if (!start && (arg_no == 2))
    {
    }
    return TRUE;
    break;
  }
  case ltINDEX:
  {
    /* Build up list of keywords associated with topics */
    if (start)
    {
//      char *entry = GetArgData();
      char buf[300];
      OutputChunkToString(GetArgChunk(), buf);
      if (CurrentTopic)
      {
        AddKeyWordForTopic(CurrentTopic, buf, currentFileName);
      }
    }
    return FALSE;
    break;
  }
  case ltFCOL:
//  case ltBCOL:
  {
    if (start)
    {
      switch (arg_no)
      {
        case 1:
        {
          char *name = GetArgData();
          char buf2[10];
          if (!FindColourHTMLString(name, buf2))
          {
            strcpy(buf2, "#000000");
            char buf[100];
			sprintf(buf, "Could not find colour name %s", name);
            OnError(buf);
          }
          TexOutput("<FONT COLOR=\"");
          TexOutput(buf2);
          TexOutput("\">");
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
      if (arg_no == 2) TexOutput("</FONT>");
    }
    return FALSE;
    break;
  }
  case ltINSERTATLEVEL:
  {
    // This macro allows you to insert text at a different level
    // from the current level, e.g. into the Sections from within a subsubsection.
    if (useWord)
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
  }
  default:
    return DefaultOnArgument(macroId, arg_no, start);
    break;
  }
  return TRUE;
}

bool HTMLGo(void)
{
  fileId = 0;
  inVerbatim = FALSE;
  indentLevel = 0;
  inTabular = FALSE;
  startRows = FALSE;
  tableVerticalLineLeft = FALSE;
  tableVerticalLineRight = FALSE;
  noColumns = 0;

  if (InputFile && OutputFile)
  {
    // Do some HTML-specific transformations on all the strings,
    // recursively
    Text2HTML(GetTopLevelChunk());

    char buf[300];
    if (truncateFilenames)
      sprintf(buf, "%s.htm", FileRoot);
    else
      sprintf(buf, "%s_contents.html", FileRoot);
    if (TitlepageName) delete[] TitlepageName;
    TitlepageName = copystring(buf);
    Titlepage = fopen(buf, "w");

    if (truncateFilenames)
      sprintf(buf, "%s_fc.htm", FileRoot);
    else
      sprintf(buf, "%s_fcontents.html", FileRoot);

    contentsFrameName = copystring(buf);

    Contents = fopen(TmpContentsName, "w");

    if (htmlFrameContents)
    {
//      FrameContents = fopen(TmpFrameContentsName, "w");
      FrameContents = fopen(contentsFrameName, "w");
      fprintf(FrameContents, "<HTML>\n<UL>\n");
    }

    if (!Titlepage || !Contents)
    {
      OnError("Cannot open output file!");
      return FALSE;
    }
    AddTexRef("contents", FileNameFromPath(TitlepageName), ContentsNameString);

    fprintf(Contents, "<P><P><H2>%s</H2><P><P>\n", ContentsNameString);

    fprintf(Contents, "<UL>\n");

    SetCurrentOutput(Titlepage);
    if (htmlWorkshopFiles) HTMLWorkshopStartContents();
    OnInform("Converting...");

    TraverseDocument();
    fprintf(Contents, "</UL>\n\n");

//    SetCurrentOutput(Titlepage);
    fclose(Titlepage);

    if (Contents)
    {
//      fprintf(Titlepage, "\n</BODY></HTML>\n");
      fclose(Contents);
      Contents = NULL;
    }

    if (FrameContents)
    {
      fprintf(FrameContents, "\n</UL>\n");
      fprintf(FrameContents, "</HTML>\n");
      fclose(FrameContents);
      FrameContents = NULL;
    }

    if (Chapters)
    {
      fprintf(Chapters, "\n</BODY></HTML>\n");
      fclose(Chapters);
      Chapters = NULL;
    }
    if (Sections)
    {
      fprintf(Sections, "\n</BODY></HTML>\n");
      fclose(Sections);
      Sections = NULL;
    }
    if (Subsections && !combineSubSections)
    {
      fprintf(Subsections, "\n</BODY></HTML>\n");
      fclose(Subsections);
      Subsections = NULL;
    }
    if (Subsubsections && !combineSubSections)
    {
      fprintf(Subsubsections, "\n</BODY></HTML>\n");
      fclose(Subsubsections);
      Subsubsections = NULL;
    }
    if ( SectionContentsFD )
    {
        fclose(SectionContentsFD);
        SectionContentsFD = NULL;
    }

    // Create a temporary file for the title page header, add some info,
    // and concat the titlepage just generated.
    // This is necessary in order to put the title of the document
    // at the TOP of the file within <HEAD>, even though we only find out
    // what it is later on.
    FILE *tmpTitle = fopen("title.tmp", "w");
    if (tmpTitle)
    {
      if (DocumentTitle)
      {
        SetCurrentOutput(tmpTitle);
        TexOutput("\n<HTML>\n<HEAD><TITLE>");
        TraverseChildrenFromChunk(DocumentTitle);
        TexOutput("</TITLE></HEAD>\n");
      }
      else
      {
        SetCurrentOutput(tmpTitle);
        if (contentsString)
          fprintf(tmpTitle, "<HEAD><TITLE>%s</TITLE></HEAD>\n\n", contentsString);
        else
          fprintf(tmpTitle, "<HEAD><TITLE>%s</TITLE></HEAD>\n\n", FileNameFromPath(FileRoot));
      }

      // Output frame information
      if (htmlFrameContents)
      {
        char firstFileName[300];
        if (truncateFilenames)
          sprintf(firstFileName, "%s1.htm", FileRoot);
        else
          sprintf(firstFileName, "%s1.html", FileRoot);

        fprintf(tmpTitle, "<FRAMESET COLS=\"30%%,70%%\">\n");

        fprintf(tmpTitle, "<FRAME SRC=\"%s\">\n", ConvertCase(FileNameFromPath(contentsFrameName)));
        fprintf(tmpTitle, "<FRAME SRC=\"%s\" NAME=\"mainwindow\">\n", ConvertCase(FileNameFromPath(firstFileName)));
        fprintf(tmpTitle, "</FRAMESET>\n");

        fprintf(tmpTitle, "<NOFRAMES>\n");
      }

      // Output <BODY...> to temporary title page
      OutputBodyStart();

      // Concat titlepage
      FILE *fd = fopen(TitlepageName, "r");
      if (fd)
      {
        int ch = getc(fd);
        while (ch != EOF)
        {
          putc(ch, tmpTitle);
          ch = getc(fd);
        }
        fclose(fd);
      }

      fprintf(tmpTitle, "\n</BODY>\n");

      if (htmlFrameContents)
      {
        fprintf(tmpTitle, "\n</NOFRAMES>\n");
      }
      fprintf(tmpTitle, "\n</HTML>\n");

      fclose(tmpTitle);
      if (FileExists(TitlepageName)) wxRemoveFile(TitlepageName);
      if (!wxRenameFile("title.tmp", TitlepageName))
      {
        wxCopyFile("title.tmp", TitlepageName);
        wxRemoveFile("title.tmp");
      }
    }

    if (lastFileName) delete[] lastFileName;
    lastFileName = NULL;
    if (lastTopic) delete[] lastTopic;
    lastTopic = NULL;

    if (FileExists(ContentsName)) wxRemoveFile(ContentsName);

    if (!wxRenameFile(TmpContentsName, ContentsName))
    {
      wxCopyFile(TmpContentsName, ContentsName);
      wxRemoveFile(TmpContentsName);
    }

    // Generate .htx file if requested
    if (htmlIndex)
    {
      char htmlIndexName[300];
      sprintf(htmlIndexName, "%s.htx", FileRoot);
      GenerateHTMLIndexFile(htmlIndexName);
    }

    // Generate HTML Help Workshop files if requested
    if (htmlWorkshopFiles)
    {
      HTMLWorkshopEndContents();
      GenerateHTMLWorkshopFiles(FileRoot);
    }


    return TRUE;
  }

  return FALSE;
}

// Output .htx index file
void GenerateHTMLIndexFile(char *fname)
{
  FILE *fd = fopen(fname, "w");
  if (!fd)
    return;

  TopicTable.BeginFind();
  wxNode *node = NULL;
  while ((node = TopicTable.Next()))
  {
    TexTopic *texTopic = (TexTopic *)node->Data();
    const char *topicName = node->GetKeyString();
    if (texTopic->filename && texTopic->keywords)
    {
      wxNode *node1 = texTopic->keywords->First();
      while (node1)
      {
        char *s = (char *)node1->Data();
        fprintf(fd, "%s|%s|%s\n", topicName, texTopic->filename, s);
        node1 = node1->Next();
      }
    }
  }
  fclose(fd);
}







// output .hpp, .hhc and .hhk files:


void GenerateHTMLWorkshopFiles(char *fname)
{
  FILE *f;
  char buf[300];

  /* Generate project file : */

  sprintf(buf, "%s.hhp", fname);
  f = fopen(buf, "wt");
  fprintf(f,
      "[OPTIONS]\n"
      "Compatibility=1.1\n"
      "Full-text search=Yes\n"
      "Contents file=%s.hhc\n"
      "Compiled file=%s.chm\n"
      "Default Window=%sHelp\n"
      "Default topic=%s\n"
      "Index file=%s.hhk\n"
      "Title=",
      FileNameFromPath(fname),
      FileNameFromPath(fname),
      FileNameFromPath(fname),
      FileNameFromPath(TitlepageName),
      FileNameFromPath(fname)
      );

  if (DocumentTitle) {
    SetCurrentOutput(f);
    TraverseChildrenFromChunk(DocumentTitle);
  }
  else fprintf(f, "(unknown)");

  fprintf(f, "\n\n[WINDOWS]\n"
          "%sHelp=,\"%s.hhc\",\"%s.hhk\",\"%s\",,,,,,0x2420,,0x380e,,,,,0,,,",
          FileNameFromPath(fname),
          FileNameFromPath(fname),
          FileNameFromPath(fname),
          FileNameFromPath(TitlepageName));


  fprintf(f, "\n\n[FILES]\n");
  fprintf(f, "%s\n", FileNameFromPath(TitlepageName));
  for (int i = 1; i <= fileId; i++) {
    if (truncateFilenames)
      sprintf(buf, "%s%d.htm", FileNameFromPath(FileRoot), i);
    else
      sprintf(buf, "%s%d.html", FileNameFromPath(FileRoot), i);
    fprintf(f, "%s\n", buf);
  }
  fclose(f);

  /* Generate index file : */

  sprintf(buf, "%s.hhk", fname);
  f = fopen(buf, "wt");

  fprintf(f,
      "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n"
      "<HTML>\n"
      "<HEAD>\n"
      "<meta name=\"GENERATOR\" content=\"tex2rtf\">\n"
      "<!-- Sitemap 1.0 -->\n"
      "</HEAD><BODY>\n"
      "<OBJECT type=\"text/site properties\">\n"
      " <param name=\"ImageType\" value=\"Folder\">\n"
      "</OBJECT>\n"
      "<UL>\n");

  TopicTable.BeginFind();
  wxNode *node = NULL;
  while ((node = TopicTable.Next()))
  {
    TexTopic *texTopic = (TexTopic *)node->Data();
    const char *topicName = node->GetKeyString();
    if (texTopic->filename && texTopic->keywords)
    {
      wxNode *node1 = texTopic->keywords->First();
      while (node1)
      {
        char *s = (char *)node1->Data();
        fprintf(f,
            " <LI> <OBJECT type=\"text/sitemap\">\n"
            "  <param name=\"Local\" value=\"%s#%s\">\n"
            "  <param name=\"Name\" value=\"%s\">\n"
            "  </OBJECT>\n",
	    texTopic->filename, topicName, s);
        node1 = node1->Next();
      }
    }
  }

  fprintf(f, "</UL>\n");
  fclose(f);
}



static FILE *HTMLWorkshopContents = NULL;
static int HTMLWorkshopLastLevel = 0;

void HTMLWorkshopAddToContents(int level, char *s, char *file)
{
  int i;

  if (level > HTMLWorkshopLastLevel)
    for (i = HTMLWorkshopLastLevel; i < level; i++)
      fprintf(HTMLWorkshopContents, "<UL>");
  if (level < HTMLWorkshopLastLevel)
    for (i = level; i < HTMLWorkshopLastLevel; i++)
      fprintf(HTMLWorkshopContents, "</UL>");

  SetCurrentOutput(HTMLWorkshopContents);
  fprintf(HTMLWorkshopContents,
            " <LI> <OBJECT type=\"text/sitemap\">\n"
            "  <param name=\"Local\" value=\"%s#%s\">\n"
            "  <param name=\"Name\" value=\"",
	    file, s);
  OutputCurrentSection();
  fprintf(HTMLWorkshopContents,
	    "\">\n"
            "  </OBJECT>\n");
  HTMLWorkshopLastLevel = level;
}


void HTMLWorkshopStartContents()
{
  char buf[300];
  sprintf(buf, "%s.hhc", FileRoot);
  HTMLWorkshopContents = fopen(buf, "wt");
  HTMLWorkshopLastLevel = 0;

  fprintf(HTMLWorkshopContents,
      "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n"
      "<HTML>\n"
      "<HEAD>\n"
      "<meta name=\"GENERATOR\" content=\"tex2rtf\">\n"
      "<!-- Sitemap 1.0 -->\n"
      "</HEAD><BODY>\n"
      "<OBJECT type=\"text/site properties\">\n"
      " <param name=\"ImageType\" value=\"Folder\">\n"
      "</OBJECT>\n"
      "<UL>\n"
      "<LI> <OBJECT type=\"text/sitemap\">\n"
      "<param name=\"Local\" value=\"%s\">\n"
      "<param name=\"Name\" value=\"Contents\">\n</OBJECT>\n",
      FileNameFromPath(TitlepageName)
      );

}


void HTMLWorkshopEndContents()
{
  for (int i = HTMLWorkshopLastLevel; i >= 0; i--)
    fprintf(HTMLWorkshopContents, "</UL>\n");
  fclose(HTMLWorkshopContents);
}

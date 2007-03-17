/////////////////////////////////////////////////////////////////////////////
// Name:        htmlutil.cpp
// Purpose:     Converts Latex to HTML
// Author:      Julian Smart
// Modified by: Wlodzimierz ABX Skiba 2003/2004 Unicode support
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

#include "wx/arrstr.h"

#include "tex2any.h"
#include "tex2rtf.h"
#include "table.h"
#include <stdio.h>
#define HTML_FILENAME_PATTERN _T("%s_%s.html")

static inline wxChar* copystring(const wxChar* s)
    { return wxStrcpy(new wxChar[wxStrlen(s) + 1], s); }

extern wxHashTable TexReferences;

extern int passNumber;

extern void DecToHex(int, wxChar *);
void GenerateHTMLIndexFile(wxChar *fname);

bool PrimaryAnchorOfTheFile( wxChar *file, wxChar *label );

void GenerateHTMLWorkshopFiles(wxChar *fname);
void HTMLWorkshopAddToContents(int level, wxChar *s, wxChar *file);
void HTMLWorkshopStartContents();
void HTMLWorkshopEndContents();

void OutputContentsFrame(void);

#include "readshg.h" // Segmented hypergraphics parsing

wxChar *ChaptersName = NULL;
wxChar *SectionsName = NULL;
wxChar *SubsectionsName = NULL;
wxChar *SubsubsectionsName = NULL;
wxChar *TitlepageName = NULL;
wxChar *lastFileName = NULL;
wxChar *lastTopic = NULL;
wxChar *currentFileName = NULL;
wxChar *contentsFrameName = NULL;

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
bool subsectionStarted = false;

// Which column of a row are we in? (Assumes no nested tables, of course)
int currentColumn = 0;

// Are we in verbatim mode? If so, format differently.
static bool inVerbatim = false;

// Need to know whether we're in a table or figure for benefit
// of listoffigures/listoftables
static bool inFigure = false;
static bool inTable = false;

// This is defined in the Tex2Any library.
extern wxChar *BigBuffer;

// DHS Two-column table dimensions.
static int TwoColWidthA = -1;
static int TwoColWidthB = -1;


class HyperReference: public wxObject
{
 public:
  wxChar *refName;
  wxChar *refFile;
  HyperReference(wxChar *name, wxChar *file)
  {
    if (name) refName = copystring(name);
    if (file) refFile = copystring(file);
  }
};

class TexNextPage: public wxObject
{
 public:
  wxChar *label;
  wxChar *filename;
  TexNextPage(wxChar *theLabel, wxChar *theFile)
  {
    label = copystring(theLabel);
    filename = copystring(theFile);
  }
  virtual ~TexNextPage(void)
  {
    delete[] label;
    delete[] filename;
  }
};

wxHashTable TexNextPages(wxKEY_STRING);

static wxChar *CurrentChapterName = NULL;
static wxChar *CurrentChapterFile = NULL;
static wxChar *CurrentSectionName = NULL;
static wxChar *CurrentSectionFile = NULL;
static wxChar *CurrentSubsectionName = NULL;
static wxChar *CurrentSubsectionFile = NULL;
static wxChar *CurrentSubsubsectionName = NULL;
static wxChar *CurrentSubsubsectionFile = NULL;
static wxChar *CurrentTopic = NULL;

static void SetCurrentTopic(wxChar *s)
{
  if (CurrentTopic) delete[] CurrentTopic;
  CurrentTopic = copystring(s);
}

void SetCurrentChapterName(wxChar *s, wxChar *file)
{
  if (CurrentChapterName) delete[] CurrentChapterName;
  CurrentChapterName = copystring(s);
  if (CurrentChapterFile) delete[] CurrentChapterFile;
  CurrentChapterFile = copystring(file);

  currentFileName = CurrentChapterFile;

  SetCurrentTopic(s);
}
void SetCurrentSectionName(wxChar *s, wxChar *file)
{
  if (CurrentSectionName) delete[] CurrentSectionName;
  CurrentSectionName = copystring(s);
  if (CurrentSectionFile) delete[] CurrentSectionFile;
  CurrentSectionFile = copystring(file);

  currentFileName = CurrentSectionFile;
  SetCurrentTopic(s);
}
void SetCurrentSubsectionName(wxChar *s, wxChar *file)
{
  if (CurrentSubsectionName) delete[] CurrentSubsectionName;
  CurrentSubsectionName = copystring(s);
  if (CurrentSubsectionFile) delete[] CurrentSubsectionFile;
  CurrentSubsectionFile = copystring(file);
  currentFileName = CurrentSubsectionFile;
  SetCurrentTopic(s);
}
void SetCurrentSubsubsectionName(wxChar *s, wxChar *file)
{
  if (CurrentSubsubsectionName) delete[] CurrentSubsubsectionName;
  CurrentSubsubsectionName = copystring(s);
  if (CurrentSubsubsectionFile) delete[] CurrentSubsubsectionFile;
  CurrentSubsubsectionFile = copystring(file);
  currentFileName = CurrentSubsubsectionFile;
  SetCurrentTopic(s);
}


// mapping between fileId and filenames if truncateFilenames=false:
static wxArrayString gs_filenames;


/*
 * Close former filedescriptor and reopen using another filename.
 *
 */

void ReopenFile(FILE **fd, wxChar **fileName, const wxChar *label)
{
  if (*fd)
  {
    wxFprintf(*fd, _T("\n</FONT></BODY></HTML>\n"));
    fclose(*fd);
  }
  fileId ++;
  wxChar buf[400];
  if (truncateFilenames)
  {
    wxSnprintf(buf, sizeof(buf), _T("%s%d.htm"), FileRoot, fileId);
  }
  else
  {
    if (fileId == 1)
      gs_filenames.Add(wxEmptyString);
    wxSnprintf(buf, sizeof(buf), HTML_FILENAME_PATTERN, FileRoot, label);
    gs_filenames.Add(buf);
  }
  if (*fileName) delete[] *fileName;
  *fileName = copystring(wxFileNameFromPath(buf));
  *fd = wxFopen(buf, _T("w"));
  wxFprintf(*fd, _T("<HTML>\n"));
}

/*
 * Reopen section contents file, i.e. the index appended to each section
 * in subsectionCombine mode
 */

static wxChar *SectionContentsFilename = NULL;
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
        wxChar buf[256];
        wxStrcpy(buf, CurrentSectionFile);
        wxStripExtension(buf);
        wxStrcat(buf, _T(".con"));
        SectionContentsFilename = copystring(buf);

        SectionContentsFD = wxFopen(SectionContentsFilename, _T("w"));
    }
}




struct textreplace
{
    wxChar *text;
    size_t text_length;
    wxChar *replace;
    size_t replace_length;
};

#define tr(x,y) {x, sizeof(x)-1, y, sizeof(y)-1}

static textreplace notverb_array[] =
{
    tr(_T("\r\n\r\n"),  _T("<P>\n\n")), // unix
    tr(_T("\n\n"),      _T("<P>\n\n")), // dos
    tr(_T("\r\r"),      _T("<P>\n\n")), // mac
    tr(_T("``"),        _T("&#8220;")),
    tr(_T("''"),        _T("&#8221;")),
    tr(_T("`"),         _T("&#8216;")),
    tr(_T("'"),         _T("&#8217;")),
    tr(_T("---"),       _T("&#8212;")),
    tr(_T("--"),        _T("&#8211;")),
};

#undef tr

/*
 * Given a TexChunk with a string value, scans through the string
 * converting Latex-isms into HTML-isms, such as 2 newlines -> <P>.
 *
 */

void ProcessText2HTML(TexChunk *chunk)
{
  bool changed = false;
  int ptr = 0;
  int i = 0;
  wxChar ch = 1;
  size_t len = wxStrlen(chunk->value);
  while (ch != 0)
  {
    ch = chunk->value[i];

    if (ch == _T('<')) // Change < to &lt
    {
      BigBuffer[ptr] = 0;
      wxStrcat(BigBuffer, _T("&lt;"));
      ptr += 4;
      i += 1;
      len--;
      changed = true;
    }
    else if (ch == _T('>')) // Change > to &gt
    {
      BigBuffer[ptr] = 0;
      wxStrcat(BigBuffer, _T("&gt;"));
      ptr += 4;
      i += 1;
      len--;
      changed = true;
    }
    else
    {
      bool replaced = false;
      if (!inVerbatim)
      {
          for (size_t x = 0; x < WXSIZEOF(notverb_array); x++)
          {
              textreplace& tr = notverb_array[x];
              if (ch != tr.text[0])
                  continue;
              if (len < tr.text_length)
                  continue;

              size_t y;
              for (y = 1; y < tr.text_length; y++)
              {
                  if (chunk->value[y] != tr.text[y])
                      break;
              }

              if (y != tr.text_length)
                  continue;

              // can now copy it over.
              for (y = 0; y < tr.replace_length; y++)
              {
                  BigBuffer[ptr++] = tr.replace[y];
              }

              len -= tr.text_length;
              i += tr.text_length;
              replaced = true;
              changed = true;
              break;
          }
      }

      if (!replaced)
      {
        BigBuffer[ptr] = ch;
        i ++;
        ptr ++;
        len--;
      }
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
        inVerbatim = true;

      wxNode *node = chunk->children.GetFirst();
      while (node)
      {
        TexChunk *child_chunk = (TexChunk *)node->GetData();
        Text2HTML(child_chunk);
        node = node->GetNext();
      }

      if (def && (def->macroId == ltVERBATIM || def->macroId == ltVERB || def->macroId == ltSPECIAL))
        inVerbatim = false;

      break;
    }
    case CHUNK_TYPE_ARG:
    {
      wxNode *node = chunk->children.GetFirst();
      while (node)
      {
        TexChunk *child_chunk = (TexChunk *)node->GetData();
        Text2HTML(child_chunk);
        node = node->GetNext();
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

void AddBrowseButtons(wxChar *upLabel, wxChar *upFilename,
  wxChar *previousLabel, wxChar *previousFilename,
  wxChar *thisLabel, wxChar *thisFilename)
{
  wxChar contentsReferenceBuf[80];
  wxChar upReferenceBuf[80];
  wxChar backReferenceBuf[80];
  wxChar forwardReferenceBuf[80];
  if (htmlBrowseButtons == HTML_BUTTONS_NONE)
    return;

  wxChar *contentsReference; // no need to initialize because always assigned below
  if (htmlBrowseButtons == HTML_BUTTONS_TEXT)
    contentsReference = ContentsNameString;
  else
  {
//    contentsReference = "<img align=center src=\"contents.gif\" BORDER=0 ALT=\"Contents\">";
    contentsReference = contentsReferenceBuf;
    wxSnprintf(contentsReference, sizeof(contentsReferenceBuf),
               _T("<img align=center src=\"%s\" BORDER=0 ALT=\"Contents\">"),
               ConvertCase(_T("contents.gif")));
  }

  wxChar *upReference; // no need to initialize because always assigned below
  if (htmlBrowseButtons == HTML_BUTTONS_TEXT)
    upReference = UpNameString;
  else
  {
//    upReference = "<img align=center src=\"up.gif\" ALT=\"Up\">";
    upReference = upReferenceBuf;
    wxSnprintf(upReference, sizeof(upReferenceBuf),
               _T("<img align=center src=\"%s\" BORDER=0 ALT=\"Up\">"),
               ConvertCase(_T("up.gif")));
  }

  wxChar *backReference; // no need to initialize because always assigned below
  if (htmlBrowseButtons == HTML_BUTTONS_TEXT)
    backReference = _T("&lt;&lt;");
  else
  {
//    backReference = "<img align=center src=\"back.gif\" ALT=\"Previous\">";
    backReference = backReferenceBuf;
    wxSnprintf(backReference, sizeof(backReferenceBuf),
               _T("<img align=center src=\"%s\" BORDER=0 ALT=\"Previous\">"),
               ConvertCase(_T("back.gif")));
  }

  wxChar *forwardReference; // no need to initialize because always assigned below
  if (htmlBrowseButtons == HTML_BUTTONS_TEXT)
    forwardReference = _T("&gt;&gt;");
  else
  {
//    forwardReference = "<img align=center src=\"forward.gif\" ALT=\"Next\">";
    forwardReference = forwardReferenceBuf;
    wxSnprintf(forwardReference, sizeof(forwardReferenceBuf),
               _T("<img align=center src=\"%s\" BORDER=0 ALT=\"Next\">"),
               ConvertCase(_T("forward.gif")));
  }

  TexOutput(_T("<CENTER>"));

  wxChar buf[200];

  /*
   * Contents button
   *
   */

  if (truncateFilenames)
  {
    wxChar buf1[80];
    wxStrcpy(buf1, ConvertCase(wxFileNameFromPath(FileRoot)));
    wxSnprintf(buf, sizeof(buf),
               _T("\n<A HREF=\"%s.%s\">%s</A> "),
               buf1, ConvertCase(_T("htm")), contentsReference);
  }
  else
  {
    wxChar buf1[80];
    wxStrcpy(buf1, ConvertCase(wxFileNameFromPath(FileRoot)));
    wxSnprintf(buf, sizeof(buf),
               _T("\n<A HREF=\"%s%s\">%s</A> "),
               buf1, ConvertCase(_T("_contents.html")), contentsReference);
  }
//  TexOutput(_T("<NOFRAMES>"));
  TexOutput(buf);
//  TexOutput(_T("</NOFRAMES>"));

  /*
   * Up button
   *
   */

  if (upLabel && upFilename)
  {
    if ( (wxStrlen(upLabel) > 0) && !PrimaryAnchorOfTheFile(upFilename, upLabel) )
      wxSnprintf(buf, sizeof(buf),
                 _T("<A HREF=\"%s#%s\">%s</A> "),
                 ConvertCase(upFilename), upLabel, upReference);
    else
      wxSnprintf(buf, sizeof(buf),
                 _T("<A HREF=\"%s\">%s</A> "),
                 ConvertCase(upFilename), upReference);
    if (wxStrcmp(upLabel, _T("contents")) == 0)
    {
//      TexOutput(_T("<NOFRAMES>"));
      TexOutput(buf);
//      TexOutput(_T("</NOFRAMES>"));
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
    if (PrimaryAnchorOfTheFile(previousFilename, previousLabel))
      wxSnprintf(buf, sizeof(buf),
                 _T("<A HREF=\"%s\">%s</A> "),
                 ConvertCase(previousFilename), backReference);
    else
      wxSnprintf(buf, sizeof(buf),
                 _T("<A HREF=\"%s#%s\">%s</A> "),
                 ConvertCase(previousFilename), previousLabel, backReference);
    if (wxStrcmp(previousLabel, _T("contents")) == 0)
    {
//      TexOutput(_T("<NOFRAMES>"));
      TexOutput(buf);
//      TexOutput(_T("</NOFRAMES>"));
    }
    else
      TexOutput(buf);
  }
  else
  {
    // A placeholder so the buttons don't keep moving position
    wxSnprintf(buf, sizeof(buf), _T("%s "), backReference);
    TexOutput(buf);
  }

  wxChar *nextLabel = NULL;
  wxChar *nextFilename = NULL;

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
    if (PrimaryAnchorOfTheFile(nextFilename, nextLabel))
      wxSnprintf(buf, sizeof(buf),
                 _T("<A HREF=\"%s\">%s</A> "),
                 ConvertCase(nextFilename), forwardReference);
    else
      wxSnprintf(buf, sizeof(buf),
                 _T("<A HREF=\"%s#%s\">%s</A> "),
                 ConvertCase(nextFilename), nextLabel, forwardReference);
    TexOutput(buf);
  }
  else
  {
    // A placeholder so the buttons don't keep moving position
    wxSnprintf(buf, sizeof(buf), _T("%s "), forwardReference);
    TexOutput(buf);
  }

  /*
   * Horizontal rule to finish it off nicely.
   *
   */
  TexOutput(_T("</CENTER>"));
  TexOutput(_T("<HR>\n"));

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
wxChar *ParseColourString(wxChar *bkStr, bool *isPicture)
{
  static wxChar resStr[300];
  wxStrcpy(resStr, bkStr);
  wxStringTokenizer tok(resStr, _T(";"), wxTOKEN_STRTOK);
  if (tok.HasMoreTokens())
  {
    wxString token1 = tok.GetNextToken();
    if (!tok.HasMoreTokens())
    {
      *isPicture = true;
      return resStr;
    }
    else
    {
      wxString token2 = tok.GetNextToken();
      *isPicture = false;
      if (tok.HasMoreTokens())
      {
        wxString token3 = tok.GetNextToken();

        // Now convert 3 strings into decimal numbers, and then hex numbers.
        int red = wxAtoi(token1.c_str());
        int green = wxAtoi(token2.c_str());
        int blue = wxAtoi(token3.c_str());

        wxStrcpy(resStr, _T("#"));

        wxChar buf[3];
        DecToHex(red, buf);
        wxStrcat(resStr, buf);
        DecToHex(green, buf);
        wxStrcat(resStr, buf);
        DecToHex(blue, buf);
        wxStrcat(resStr, buf);
        return resStr;
      }
      else return NULL;
    }
  }
  else return NULL;
}

void OutputFont(void)
{
  // Only output <font face> if explicitly requested by htmlFaceName= directive in
  // tex2rtf.ini. Otherwise do NOT set the font because we want to use browser's
  // default font:
  if (htmlFaceName)
  {
    // Output <FONT FACE=...>
    TexOutput(_T("<FONT FACE=\""));
    TexOutput(htmlFaceName);
    TexOutput(_T("\">\n"));
  }
}

// Output start of <BODY> block
void OutputBodyStart(void)
{
  TexOutput(_T("\n<BODY"));
  if (backgroundImageString)
  {
    bool isPicture = false;
    wxChar *s = ParseColourString(backgroundImageString, &isPicture);
    if (s)
    {
      TexOutput(_T(" BACKGROUND=\""));
      TexOutput(s);
      TexOutput(_T("\""));
    }
  }
  if (backgroundColourString)
  {
    bool isPicture = false;
    wxChar *s = ParseColourString(backgroundColourString, &isPicture);
    if (s)
    {
      TexOutput(_T(" BGCOLOR="));
      TexOutput(s);
    }
  }

  // Set foreground text colour, if one is specified
  if (textColourString)
  {
    bool isPicture = false;
    wxChar *s = ParseColourString(textColourString, &isPicture);
    if (s)
    {
      TexOutput(_T(" TEXT=")); TexOutput(s);
    }
  }
  // Set link text colour, if one is specified
  if (linkColourString)
  {
    bool isPicture = false;
    wxChar *s = ParseColourString(linkColourString, &isPicture);
    if (s)
    {
      TexOutput(_T(" LINK=")); TexOutput(s);
    }
  }
  // Set followed link text colour, if one is specified
  if (followedLinkColourString)
  {
    bool isPicture = false;
    wxChar *s = ParseColourString(followedLinkColourString, &isPicture);
    if (s)
    {
      TexOutput(_T(" VLINK=")); TexOutput(s);
    }
  }
  TexOutput(_T(">\n"));

  OutputFont();
}

void HTMLHead()
{
  TexOutput(_T("<head>"));
  if (htmlStylesheet) {
    TexOutput(_T("<link rel=stylesheet type=\"text/css\" href=\""));
    TexOutput(htmlStylesheet);
    TexOutput(_T("\">"));
  }
};

void HTMLHeadTo(FILE* f)
{
  if (htmlStylesheet)
    wxFprintf(f,_T("<head><link rel=stylesheet type=\"text/css\" href=\"%s\">"),htmlStylesheet);
  else
    wxFprintf(f,_T("<head>"));
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
      startedSections = true;

      wxChar *topicName = FindTopicName(GetNextChunk());
      ReopenFile(&Chapters, &ChaptersName, topicName);
      AddTexRef(topicName, ChaptersName, ChapterNameString);

      SetCurrentChapterName(topicName, ChaptersName);
      if (htmlWorkshopFiles) HTMLWorkshopAddToContents(0, topicName, ChaptersName);

      SetCurrentOutput(Chapters);

      HTMLHead();
      TexOutput(_T("<title>"));
      OutputCurrentSection(); // Repeat section header
      TexOutput(_T("</title></head>\n"));
      OutputBodyStart();

      wxChar titleBuf[200];
      if (truncateFilenames)
        wxSnprintf(titleBuf, sizeof(titleBuf), _T("%s.htm"), wxFileNameFromPath(FileRoot));
      else
        wxSnprintf(titleBuf, sizeof(titleBuf), _T("%s_contents.html"), wxFileNameFromPath(FileRoot));

      wxFprintf(Chapters, _T("<A NAME=\"%s\"></A>"), topicName);

      AddBrowseButtons(_T(""), titleBuf, // Up
                       lastTopic, lastFileName,  // Last topic
                       topicName, ChaptersName); // This topic

      if(PrimaryAnchorOfTheFile(ChaptersName, topicName))
        wxFprintf(Contents, _T("\n<LI><A HREF=\"%s\">"), ConvertCase(ChaptersName));
      else
        wxFprintf(Contents, _T("\n<LI><A HREF=\"%s#%s\">"), ConvertCase(ChaptersName), topicName);

      if (htmlFrameContents && FrameContents)
      {
        SetCurrentOutput(FrameContents);
        if(PrimaryAnchorOfTheFile(ChaptersName, topicName))
          wxFprintf(FrameContents, _T("\n<LI><A HREF=\"%s\" TARGET=\"mainwindow\">"), ConvertCase(ChaptersName));
        else
          wxFprintf(FrameContents, _T("\n<LI><A HREF=\"%s#%s\" TARGET=\"mainwindow\">"), ConvertCase(ChaptersName), topicName);
        OutputCurrentSection();
        wxFprintf(FrameContents, _T("</A>\n"));
      }

      SetCurrentOutputs(Contents, Chapters);
      wxFprintf(Chapters, _T("\n<H2>"));
      OutputCurrentSection();
      wxFprintf(Contents, _T("</A>\n"));
      wxFprintf(Chapters, _T("</H2>\n"));

      SetCurrentOutput(Chapters);

      // Add this section title to the list of keywords
      if (htmlIndex)
      {
        OutputCurrentSectionToString(wxTex2RTFBuffer);
        AddKeyWordForTopic(topicName, wxTex2RTFBuffer, ConvertCase(currentFileName));
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
      subsectionStarted = false;

      if (macroId != ltSECTIONSTAR)
        sectionNo ++;

      SetCurrentOutput(NULL);
      startedSections = true;

      wxChar *topicName = FindTopicName(GetNextChunk());
      ReopenFile(&Sections, &SectionsName, topicName);
      AddTexRef(topicName, SectionsName, SectionNameString);

      SetCurrentSectionName(topicName, SectionsName);
      if (htmlWorkshopFiles) HTMLWorkshopAddToContents(1, topicName, SectionsName);

      SetCurrentOutput(Sections);
      HTMLHead();
      TexOutput(_T("<title>"));
      OutputCurrentSection();
      TexOutput(_T("</title></head>\n"));
      OutputBodyStart();

      wxFprintf(Sections, _T("<A NAME=\"%s\"></A>"), topicName);
      AddBrowseButtons(CurrentChapterName, CurrentChapterFile, // Up
                       lastTopic, lastFileName,  // Last topic
                       topicName, SectionsName); // This topic

      FILE *jumpFrom = ((DocumentStyle == LATEX_ARTICLE) ? Contents : Chapters);

      SetCurrentOutputs(jumpFrom, Sections);
      if (DocumentStyle == LATEX_ARTICLE)
      {
        if(PrimaryAnchorOfTheFile(SectionsName, topicName))
          wxFprintf(jumpFrom, _T("\n<LI><A HREF=\"%s\">"), ConvertCase(SectionsName));
        else
          wxFprintf(jumpFrom, _T("\n<LI><A HREF=\"%s#%s\">"), ConvertCase(SectionsName), topicName);
      }
      else
      {
        if(PrimaryAnchorOfTheFile(SectionsName, topicName))
          wxFprintf(jumpFrom, _T("\n<A HREF=\"%s\"><B>"), ConvertCase(SectionsName));
        else
          wxFprintf(jumpFrom, _T("\n<A HREF=\"%s#%s\"><B>"), ConvertCase(SectionsName), topicName);
      }

      wxFprintf(Sections, _T("\n<H2>"));
      OutputCurrentSection();

      if (DocumentStyle == LATEX_ARTICLE)
        wxFprintf(jumpFrom, _T("</A>\n"));
      else
        wxFprintf(jumpFrom, _T("</B></A><BR>\n"));
      wxFprintf(Sections, _T("</H2>\n"));

      SetCurrentOutput(Sections);
      // Add this section title to the list of keywords
      if (htmlIndex)
      {
        OutputCurrentSectionToString(wxTex2RTFBuffer);
        AddKeyWordForTopic(topicName, wxTex2RTFBuffer, currentFileName);
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
        OnError(_T("You cannot have a subsection before a section!"));
      }
      else
      {
          subsubsectionNo = 0;

          if (macroId != ltSUBSECTIONSTAR)
            subsectionNo ++;

          if ( combineSubSections && !subsectionStarted )
          {
            fflush(Sections);

            // Read old .con file in at this point
            wxChar buf[256];
            wxStrcpy(buf, CurrentSectionFile);
            wxStripExtension(buf);
            wxStrcat(buf, _T(".con"));
            FILE *fd = wxFopen(buf, _T("r"));
            if ( fd )
            {
                int ch = getc(fd);
                while (ch != EOF)
                {
                    wxPutc(ch, Sections);
                    ch = getc(fd);
                }
                fclose(fd);
            }
            wxFprintf(Sections, _T("<P>\n"));

            // Close old file, create a new file for the sub(sub)section contents entries
            ReopenSectionContentsFile();
          }

          startedSections = true;
          subsectionStarted = true;

          wxChar *topicName = FindTopicName(GetNextChunk());

          if ( !combineSubSections )
          {
            SetCurrentOutput(NULL);
            ReopenFile(&Subsections, &SubsectionsName, topicName);
            AddTexRef(topicName, SubsectionsName, SubsectionNameString);
            SetCurrentSubsectionName(topicName, SubsectionsName);
            if (htmlWorkshopFiles) HTMLWorkshopAddToContents(2, topicName, SubsectionsName);
            SetCurrentOutput(Subsections);

            HTMLHead();
            TexOutput(_T("<title>"));
            OutputCurrentSection();
            TexOutput(_T("</title></head>\n"));
            OutputBodyStart();

            wxFprintf(Subsections, _T("<A NAME=\"%s\"></A>"), topicName);
            AddBrowseButtons(CurrentSectionName, CurrentSectionFile, // Up
                           lastTopic, lastFileName,  // Last topic
                           topicName, SubsectionsName); // This topic

            SetCurrentOutputs(Sections, Subsections);
            if(PrimaryAnchorOfTheFile(SubsectionsName, topicName))
              wxFprintf(Sections, _T("\n<A HREF=\"%s\"><B>"), ConvertCase(SubsectionsName));
            else
              wxFprintf(Sections, _T("\n<A HREF=\"%s#%s\"><B>"), ConvertCase(SubsectionsName), topicName);

            wxFprintf(Subsections, _T("\n<H3>"));
            OutputCurrentSection();
            wxFprintf(Sections, _T("</B></A><BR>\n"));
            wxFprintf(Subsections, _T("</H3>\n"));

            SetCurrentOutput(Subsections);
          }
          else
          {
            AddTexRef(topicName, SectionsName, SubsectionNameString);
            SetCurrentSubsectionName(topicName, SectionsName);

//            if ( subsectionNo != 0 )
            wxFprintf(Sections, _T("\n<HR>\n"));

            // We're putting everything into the section file
            wxFprintf(Sections, _T("<A NAME=\"%s\"></A>"), topicName);
            wxFprintf(Sections, _T("\n<H3>"));
            OutputCurrentSection();
            wxFprintf(Sections, _T("</H3>\n"));

            SetCurrentOutput(SectionContentsFD);
            wxFprintf(SectionContentsFD, _T("<A HREF=\"#%s\">"), topicName);
            OutputCurrentSection();
            TexOutput(_T("</A><BR>\n"));

            if (htmlWorkshopFiles) HTMLWorkshopAddToContents(2, topicName, SectionsName);
            SetCurrentOutput(Sections);
          }
          // Add this section title to the list of keywords
          if (htmlIndex)
          {
            OutputCurrentSectionToString(wxTex2RTFBuffer);
            AddKeyWordForTopic(topicName, wxTex2RTFBuffer, currentFileName);
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
        OnError(_T("You cannot have a subsubsection before a subsection!"));
      }
      else
      {
        if (macroId != ltSUBSUBSECTIONSTAR)
          subsubsectionNo ++;

        startedSections = true;

        wxChar *topicName = FindTopicName(GetNextChunk());

        if ( !combineSubSections )
        {
            SetCurrentOutput(NULL);
            ReopenFile(&Subsubsections, &SubsubsectionsName, topicName);
            AddTexRef(topicName, SubsubsectionsName, SubsubsectionNameString);
            SetCurrentSubsubsectionName(topicName, SubsubsectionsName);
            if (htmlWorkshopFiles) HTMLWorkshopAddToContents(3, topicName, SubsubsectionsName);

            SetCurrentOutput(Subsubsections);
            HTMLHead();
            TexOutput(_T("<title>"));
            OutputCurrentSection();
            TexOutput(_T("</title></head>\n"));
            OutputBodyStart();

            wxFprintf(Subsubsections, _T("<A NAME=\"%s\"></A>"), topicName);

            AddBrowseButtons(CurrentSubsectionName, CurrentSubsectionFile, // Up
                         lastTopic, lastFileName,  // Last topic
                         topicName, SubsubsectionsName); // This topic

            SetCurrentOutputs(Subsections, Subsubsections);
            if(PrimaryAnchorOfTheFile(SubsubsectionsName, topicName))
              wxFprintf(Subsections, _T("\n<A HREF=\"%s\"><B>"), ConvertCase(SubsubsectionsName));
            else
              wxFprintf(Subsections, _T("\n<A HREF=\"%s#%s\"><B>"), ConvertCase(SubsubsectionsName), topicName);

            wxFprintf(Subsubsections, _T("\n<H3>"));
            OutputCurrentSection();
            wxFprintf(Subsections, _T("</B></A><BR>\n"));
            wxFprintf(Subsubsections, _T("</H3>\n"));
        }
        else
        {
            AddTexRef(topicName, SectionsName, SubsubsectionNameString);
            SetCurrentSubsectionName(topicName, SectionsName);
            wxFprintf(Sections, _T("\n<HR>\n"));

            // We're putting everything into the section file
            wxFprintf(Sections, _T("<A NAME=\"%s\"></A>"), topicName);
            wxFprintf(Sections, _T("\n<H3>"));
            OutputCurrentSection();
            wxFprintf(Sections, _T("</H3>\n"));
/* TODO: where do we put subsubsection contents entry - indented, with subsection entries?
            SetCurrentOutput(SectionContentsFD);
            wxFprintf(SectionContentsFD, "<A HREF=\"#%s\">", topicName);
            OutputCurrentSection();
            TexOutput(_T("</A><BR>"));
*/
            if (htmlWorkshopFiles) HTMLWorkshopAddToContents(2, topicName, SectionsName);
            SetCurrentOutput(Sections);
        }

        // Add this section title to the list of keywords
        if (htmlIndex)
        {
          OutputCurrentSectionToString(wxTex2RTFBuffer);
          AddKeyWordForTopic(topicName, wxTex2RTFBuffer, currentFileName);
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
//      TexOutput(_T("<B>void</B>"));
    break;
  case ltHARDY:
    if (start)
      TexOutput(_T("HARDY"));
    break;
  case ltWXCLIPS:
    if (start)
      TexOutput(_T("wxCLIPS"));
    break;
  case ltAMPERSAND:
    if (start)
      TexOutput(_T("&amp;"));
    break;
  case ltSPECIALAMPERSAND:
  {
    if (start)
    {
      if (inTabular)
      {
        // End cell, start cell

        TexOutput(_T("</FONT></TD>"));

        // Start new row and cell, setting alignment for the first cell.
        if (currentColumn < noColumns)
          currentColumn ++;

        wxChar buf[100];
        if (TableData[currentColumn].justification == 'c')
          wxSnprintf(buf, sizeof(buf), _T("\n<TD ALIGN=CENTER>"));
        else if (TableData[currentColumn].justification == 'r')
          wxSnprintf(buf, sizeof(buf), _T("\n<TD ALIGN=RIGHT>"));
        else if (TableData[currentColumn].absWidth)
        {
          // Convert from points * 20 into pixels.
          int points = TableData[currentColumn].width / 20;

          // Say the display is 100 DPI (dots/pixels per inch).
          // There are 72 pts to the inch. So 1pt = 1/72 inch, or 100 * 1/72 dots.
          int pixels = (int)(points * 100.0 / 72.0);
          wxSnprintf(buf, sizeof(buf), _T("<TD ALIGN=CENTER WIDTH=%d>"), pixels);
        }
        else
          wxSnprintf(buf, sizeof(buf), _T("\n<TD ALIGN=LEFT>"));
        TexOutput(buf);
        OutputFont();
      }
      else
        TexOutput(_T("&amp;"));
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
        TexOutput(_T("</TR>\n"));
      }
      else
        TexOutput(_T("<BR>\n"));
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
      wxChar buf[100];
      if (TableData[currentColumn].justification == 'c')
        wxSnprintf(buf, sizeof(buf), _T("<TR>\n<TD ALIGN=CENTER>"));
      else if (TableData[currentColumn].justification == 'r')
        wxSnprintf(buf, sizeof(buf), _T("<TR>\n<TD ALIGN=RIGHT>"));
      else if (TableData[currentColumn].absWidth)
      {
        // Convert from points * 20 into pixels.
        int points = TableData[currentColumn].width / 20;

        // Say the display is 100 DPI (dots/pixels per inch).
        // There are 72 pts to the inch. So 1pt = 1/72 inch, or 100 * 1/72 dots.
        int pixels = (int)(points * 100.0 / 72.0);
        wxSnprintf(buf, sizeof(buf), _T("<TR>\n<TD ALIGN=CENTER WIDTH=%d>"), pixels);
      }
      else
        wxSnprintf(buf, sizeof(buf), _T("<TR>\n<TD ALIGN=LEFT>"));
      TexOutput(buf);
      OutputFont();
    }
    else
    {
      // End cell and row
      // Start new row and cell
      TexOutput(_T("</FONT></TD>\n</TR>\n"));
    }
    break;
  }
  // HTML-only: break until the end of the picture (both margins are clear).
  case ltBRCLEAR:
  {
    if (start)
      TexOutput(_T("<BR CLEAR=ALL>"));
    break;
  }
  case ltRTFSP:  // Explicit space, RTF only
    break;
  case ltSPECIALTILDE:
  {
    if (start)
    {
      #if (1) // if(inVerbatim)
        TexOutput(_T("~"));
      #else
        TexOutput(_T(" "));
      #endif
    }
    break;
  }
  case ltINDENTED :
  {
    if ( start )
        TexOutput(_T("<UL><UL>\n"));
    else
        TexOutput(_T("</UL></UL>\n"));
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
          TexOutput(_T("<UL>\n"));
          break;
        case LATEX_ENUMERATE:
          TexOutput(_T("<OL>\n"));
          break;
        case LATEX_DESCRIPTION:
        default:
          TexOutput(_T("<DL>\n"));
          break;
      }
    }
    else
    {
      indentLevel --;
      if (itemizeStack.GetFirst())
      {
        ItemizeStruc *struc = (ItemizeStruc *)itemizeStack.GetFirst()->GetData();
        switch (struc->listType)
        {
          case LATEX_ITEMIZE:
            TexOutput(_T("</UL>\n"));
            break;
          case LATEX_ENUMERATE:
            TexOutput(_T("</OL>\n"));
            break;
          case LATEX_DESCRIPTION:
          default:
            TexOutput(_T("</DL>\n"));
            break;
        }

        delete struc;
        delete itemizeStack.GetFirst();
      }
    }
    break;
  }
  case ltTWOCOLLIST :
  {
    if ( start )
        TexOutput(_T("\n<TABLE>\n"));
    else {
        TexOutput(_T("\n</TABLE>\n"));
    // DHS
        TwoColWidthA = -1;
        TwoColWidthB = -1;
    }
    break;
  }
  case ltPAR:
  {
    if (start)
      TexOutput(_T("<P>\n"));
    break;
  }
/* For footnotes we need to output the text at the bottom of the page and
 * insert a reference to it. Is it worth the trouble...
  case ltFOOTNOTE:
  case ltFOOTNOTEPOPUP:
  {
    if (start)
    {
      TexOutput(_T("<FN>"));
    }
    else TexOutput(_T("</FN>"));
    break;
  }
*/
  case ltVERB:
  {
    if (start)
      TexOutput(_T("<TT>"));
    else TexOutput(_T("</TT>"));
    break;
  }
  case ltVERBATIM:
  {
    if (start)
    {
      wxChar buf[100];
      wxSnprintf(buf, sizeof(buf), _T("<PRE>\n"));
      TexOutput(buf);
    }
    else TexOutput(_T("</PRE>\n"));
    break;
  }
  case ltCENTERLINE:
  case ltCENTER:
  {
    if (start)
    {
      TexOutput(_T("<CENTER>"));
    }
    else TexOutput(_T("</CENTER>"));
    break;
  }
  case ltFLUSHLEFT:
  {
/*
    if (start)
    {
      TexOutput(_T("{\\ql "));
    }
    else TexOutput(_T("}\\par\\pard\n"));
*/
    break;
  }
  case ltFLUSHRIGHT:
  {
/*
    if (start)
    {
      TexOutput(_T("{\\qr "));
    }
    else TexOutput(_T("}\\par\\pard\n"));
*/
    break;
  }
  case ltSMALL:
  {
    if (start)
    {
      // Netscape extension
      TexOutput(_T("<FONT SIZE=2>"));
    }
    else TexOutput(_T("</FONT>"));
    break;
  }
  case ltTINY:
  {
    if (start)
    {
      // Netscape extension
      TexOutput(_T("<FONT SIZE=1>"));
    }
    else TexOutput(_T("</FONT>"));
    break;
  }
  case ltNORMALSIZE:
  {
    if (start)
    {
      // Netscape extension
      TexOutput(_T("<FONT SIZE=3>"));
    }
    else TexOutput(_T("</FONT>"));
    break;
  }
  case ltlarge:
  {
    if (start)
    {
      // Netscape extension
      TexOutput(_T("<FONT SIZE=4>"));
    }
    else TexOutput(_T("</FONT>"));
    break;
  }
  case ltLarge:
  {
    if (start)
    {
      // Netscape extension
      TexOutput(_T("<FONT SIZE=5>"));
    }
    else TexOutput(_T("</FONT>"));
    break;
  }
  case ltLARGE:
  {
    if (start)
    {
      // Netscape extension
      TexOutput(_T("<FONT SIZE=6>"));
    }
    else TexOutput(_T("</FONT>"));
    break;
  }
  case ltBFSERIES:
  case ltTEXTBF:
  case ltBF:
  {
    if (start)
    {
      TexOutput(_T("<B>"));
    }
    else TexOutput(_T("</B>"));
    break;
  }
  case ltITSHAPE:
  case ltTEXTIT:
  case ltIT:
  {
    if (start)
    {
      TexOutput(_T("<I>"));
    }
    else TexOutput(_T("</I>"));
    break;
  }
  case ltEMPH:
  case ltEM:
  {
    if (start)
    {
      TexOutput(_T("<EM>"));
    }
    else TexOutput(_T("</EM>"));
    break;
  }
  case ltUNDERLINE:
  {
    if (start)
    {
      TexOutput(_T("<UL>"));
    }
    else TexOutput(_T("</UL>"));
    break;
  }
  case ltTTFAMILY:
  case ltTEXTTT:
  case ltTT:
  {
    if (start)
    {
      TexOutput(_T("<TT>"));
    }
    else TexOutput(_T("</TT>"));
    break;
  }
  case ltCOPYRIGHT:
  {
    if (start)
      TexOutput(_T("&copy;"), true);
    break;
  }
  case ltREGISTERED:
  {
    if (start)
      TexOutput(_T("&reg;"), true);
    break;
  }
  // Arrows
  case ltLEFTARROW:
  {
    if (start) TexOutput(_T("&lt;--"));
    break;
  }
  case ltLEFTARROW2:
  {
    if (start) TexOutput(_T("&lt;=="));
    break;
  }
  case ltRIGHTARROW:
  {
      if (start) TexOutput(_T("--&gt;"));
      break;
  }
  case ltRIGHTARROW2:
  {
    if (start) TexOutput(_T("==&gt;"));
    break;
  }
  case ltLEFTRIGHTARROW:
  {
    if (start) TexOutput(_T("&lt;--&gt;"));
    break;
  }
  case ltLEFTRIGHTARROW2:
  {
    if (start) TexOutput(_T("&lt;==&gt;"));
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
      wxNode *node = itemizeStack.GetFirst();
      if (node)
      {
        ItemizeStruc *struc = (ItemizeStruc *)node->GetData();
        struc->currentItem += 1;
        if (struc->listType == LATEX_DESCRIPTION)
        {
          if (descriptionItemArg)
          {
            TexOutput(_T("<DT> "));
            TraverseChildrenFromChunk(descriptionItemArg);
            TexOutput(_T("\n"));
            descriptionItemArg = NULL;
          }
          TexOutput(_T("<DD>"));
        }
        else
          TexOutput(_T("<LI>"));
      }
    }
    break;
  }
  case ltMAKETITLE:
  {
    if (start && DocumentTitle && DocumentAuthor)
    {
      // Add a special label for the contents page.
//      TexOutput(_T("<CENTER>\n"));
      TexOutput(_T("<A NAME=\"contents\">"));
      TexOutput(_T("<H2 ALIGN=CENTER>\n"));
      TraverseChildrenFromChunk(DocumentTitle);
      TexOutput(_T("</H2>"));
      TexOutput(_T("<P>"));
      TexOutput(_T("</A>\n"));
      TexOutput(_T("<P>\n\n"));
      TexOutput(_T("<H3 ALIGN=CENTER>"));
      TraverseChildrenFromChunk(DocumentAuthor);
      TexOutput(_T("</H3><P>\n\n"));
      if (DocumentDate)
      {
        TexOutput(_T("<H3 ALIGN=CENTER>"));
        TraverseChildrenFromChunk(DocumentDate);
        TexOutput(_T("</H3><P>\n\n"));
      }
//      TexOutput(_T("\n</CENTER>\n"));
      TexOutput(_T("\n<P><HR><P>\n"));

/*
      // Now do optional frame contents page
      if (htmlFrameContents && FrameContents)
      {
        SetCurrentOutput(FrameContents);

        // Add a special label for the contents page.
        TexOutput(_T("<CENTER>\n"));
        TexOutput(_T("<H3>\n"));
        TraverseChildrenFromChunk(DocumentTitle);
        TexOutput(_T("</H3>"));
        TexOutput(_T("<P>"));
        TexOutput(_T("</A>\n"));
        TexOutput(_T("<P>\n\n"));
        TexOutput(_T("<H3>"));
        TraverseChildrenFromChunk(DocumentAuthor);
        TexOutput(_T("</H3><P>\n\n"));
        if (DocumentDate)
        {
          TexOutput(_T("<H4>"));
          TraverseChildrenFromChunk(DocumentDate);
          TexOutput(_T("</H4><P>\n\n"));
        }
        TexOutput(_T("\n</CENTER>\n"));
        TexOutput(_T("<P><HR><P>\n"));

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
      TexOutput(_T("</DL>\n"));
    }
    break;
  }
  case ltHRULE:
  {
    if (start)
    {
      TexOutput(_T("<HR>\n"));
    }
    break;
  }
  case ltRULE:
  {
    if (start)
    {
      TexOutput(_T("<HR>\n"));
    }
    break;
  }
  case ltTABLEOFCONTENTS:
  {
    if (start)
    {
      // NB: if this is uncommented, the table of contents
      // completely disappears. If left commented, it's in the wrong
      // place.
      //fflush(Titlepage);

      FILE *fd = wxFopen(ContentsName, _T("r"));
      if (fd)
      {
        int ch = getc(fd);
        while (ch != EOF)
        {
          wxPutc(ch, Titlepage);
          ch = getc(fd);
        }
        fclose(fd);
        fflush(Titlepage);
      }
      else
      {
        TexOutput(_T("RUN TEX2RTF AGAIN FOR CONTENTS PAGE\n"));
        OnInform(_T("Run Tex2RTF again to include contents page."));
      }
    }
    break;
  }
  case ltLANGLEBRA:
  {
    if (start)
      TexOutput(_T("&lt;"));
    break;
  }
  case ltRANGLEBRA:
  {
    if (start)
      TexOutput(_T("&gt;"));
    break;
  }
  case ltQUOTE:
  case ltQUOTATION:
  {
    if (start)
      TexOutput(_T("<BLOCKQUOTE>"));
    else
      TexOutput(_T("</BLOCKQUOTE>"));
    break;
  }
  case ltCAPTION:
  case ltCAPTIONSTAR:
  {
    if (start)
    {
      if (inTabular)
        TexOutput(_T("\n<CAPTION>"));

      wxChar figBuf[40];

      if ( inFigure )
      {
          figureNo ++;

          if (DocumentStyle != LATEX_ARTICLE)
            wxSnprintf(figBuf, sizeof(figBuf), _T("%s %d.%d: "), FigureNameString, chapterNo, figureNo);
          else
            wxSnprintf(figBuf, sizeof(figBuf), _T("%s %d: "), FigureNameString, figureNo);
      }
      else
      {
          tableNo ++;

          if (DocumentStyle != LATEX_ARTICLE)
            wxSnprintf(figBuf, sizeof(figBuf), _T("%s %d.%d: "), TableNameString, chapterNo, tableNo);
          else
            wxSnprintf(figBuf, sizeof(figBuf), _T("%s %d: "), TableNameString, tableNo);
      }

      TexOutput(figBuf);
    }
    else
    {
      if (inTabular)
        TexOutput(_T("\n</CAPTION>\n"));

      wxChar *topicName = FindTopicName(GetNextChunk());

      int n = inFigure ? figureNo : tableNo;

      AddTexRef(topicName, NULL, NULL,
           ((DocumentStyle != LATEX_ARTICLE) ? chapterNo : n),
            ((DocumentStyle != LATEX_ARTICLE) ? n : 0));
    }
    break;
  }
  case ltSS:
  {
    if (start) TexOutput(_T("&szlig;"));
    break;
  }
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
    DefaultOnMacro(macroId, no_args, start);
    break;
  }
}
/*     CheckTypeRef()

       should be called at of argument which usually is
       type declaration which propably contains name of
       documented class

       examples:
               HTMLOnArgument
                       - ltFUNC,
                       - ltPARAM
                       - ltCPARAM

       checks: GetArgData() if contains Type Declaration
                               and can be referenced to some file
       prints:
               before<a href="xxx&yyy">type</a>after

       returns:
               false   - if no reference was found
               true    - if reference was found and HREF printed
*/
static bool CheckTypeRef()
{
  wxString typeDecl = GetArgData();
  if( !typeDecl.empty() ) {
    typeDecl.Replace(wxT("\\"),wxT(""));
    wxString label = typeDecl;
    label.Replace(wxT("const"),wxT(""));
    label.Replace(wxT("virtual"),wxT(""));
    label.Replace(wxT("static"),wxT(""));
    label.Replace(wxT("extern"),wxT(""));
    label = label.BeforeFirst('&');
    label = label.BeforeFirst(wxT('*'));
    label = label.BeforeFirst(wxT('\\'));
    label.Trim(true); label.Trim(false);
    wxString typeName = label;
    label.MakeLower();
    TexRef *texRef = FindReference(label);

    if (texRef && texRef->refFile && wxStrcmp(texRef->refFile, _T("??")) != 0) {
      int a = typeDecl.Find(typeName);
      wxString before = typeDecl.Mid( 0, a );
      wxString after = typeDecl.Mid( a+typeName.Length() );
      //wxFprintf(stderr,wxT("%s <%s> %s to ... %s#%s !!!!\n"),
       //      before.c_str(),
       //      typeName.c_str(),
       //      after.c_str(),
       //      texRef->refFile,label.c_str());
      TexOutput(before);
      TexOutput(_T("<A HREF=\""));
      TexOutput(texRef->refFile);
      TexOutput(_T("#"));
      TexOutput(label);
      TexOutput(wxT("\">"));
      TexOutput(typeName);
      TexOutput(wxT("</A>"));
      TexOutput(after);
      return true;
    } else {
      //wxFprintf(stderr,wxT("'%s' from (%s) -> label %s NOT FOUND\n"),
       //      typeName.c_str(),
       //      typeDecl.c_str(),
       //      label.c_str());
      return false;
    }
  }
  return false;
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
    return false;
  }
  case ltFUNC:
  {
    if (start && (arg_no == 1)) {
      TexOutput(_T("<B>"));
      if( CheckTypeRef() ) {
       TexOutput(_T("</B> "));
       return false;
      }
    }

    if (!start && (arg_no == 1))
      TexOutput(_T("</B> "));

    if (start && (arg_no == 2))
    {
      if (!suppressNameDecoration) TexOutput(_T("<B>"));
      currentMember = GetArgChunk();
    }
    if (!start && (arg_no == 2))
    {
      if (!suppressNameDecoration) TexOutput(_T("</B>"));
    }

    if (start && (arg_no == 3))
      TexOutput(_T("("));
    if (!start && (arg_no == 3))
     TexOutput(_T(")"));
   break;
  }
  case ltCLIPSFUNC:
  {
    if (start && (arg_no == 1))
      TexOutput(_T("<B>"));
    if (!start && (arg_no == 1))
      TexOutput(_T("</B> "));

    if (start && (arg_no == 2))
    {
      if (!suppressNameDecoration) TexOutput(_T("( "));
      currentMember = GetArgChunk();
    }
    if (!start && (arg_no == 2))
    {
    }

    if (!start && (arg_no == 3))
     TexOutput(_T(")"));
    break;
  }
  case ltPFUNC:
  {
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
      TexOutput(_T(")"));
    break;
  }
  case ltPARAM:
  case ltCPARAM:
  {
    const wxChar* pend = macroId == ltCPARAM ?
       _T("</B> ") : _T("</B>");
    if( arg_no == 1) {
      if( start ) {
       TexOutput(_T("<B>"));
       if( CheckTypeRef() ) {
         TexOutput(pend);
         return false;
       }
      } else {
       TexOutput(pend);
      }
    }
    if (start && (arg_no == 2))
    {
      TexOutput(_T("<I>"));
    }
    if (!start && (arg_no == 2))
    {
      TexOutput(_T("</I>"));
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
      return false;
    }
    break;
  }
  case ltURLREF:
  {
    if (IsArgOptional())
      return false;
    else if ((GetNoArgs() - arg_no) == 1)
    {
      if (start)
        helpRefText = GetArgChunk();
      return false;
    }
    else if ((GetNoArgs() - arg_no) == 0) // Arg = 2, or 3 if first is optional
    {
      if (start)
      {
        TexChunk *ref = GetArgChunk();
        TexOutput(_T("<A HREF=\""));
        inVerbatim = true;
        TraverseChildrenFromChunk(ref);
        inVerbatim = false;
        TexOutput(_T("\">"));
        if (helpRefText)
          TraverseChildrenFromChunk(helpRefText);
        TexOutput(_T("</A>"));
      }
      return false;
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
      return false;
    }
    if ((GetNoArgs() - arg_no) == 1)
    {
      if (start)
        helpRefText = GetArgChunk();
      return false;
    }
    else if ((GetNoArgs() - arg_no) == 0) // Arg = 2, or 3 if first is optional
    {
      if (start)
      {
        wxChar *refName = GetArgData();
        wxChar *refFilename = NULL;

        if (refName)
        {
          TexRef *texRef = FindReference(refName);
          if (texRef)
          {
            if (texRef->refFile && wxStrcmp(texRef->refFile, _T("??")) != 0)
              refFilename = texRef->refFile;

            TexOutput(_T("<A HREF=\""));
            // If a filename is supplied, use it, otherwise try to
            // use the filename associated with the reference (from this document).
            if (helpRefFilename)
            {
              TraverseChildrenFromChunk(helpRefFilename);
              TexOutput(_T("#"));
              TexOutput(refName);
            }
            else if (refFilename)
            {
              TexOutput(ConvertCase(refFilename));
              if(!PrimaryAnchorOfTheFile(texRef->refFile, refName))
              {
                TexOutput(_T("#"));
                TexOutput(refName);
              }
            }
            TexOutput(_T("\">"));
            if (helpRefText)
              TraverseChildrenFromChunk(helpRefText);
            TexOutput(_T("</A>"));
          }
          else
          {
            if (helpRefText)
              TraverseChildrenFromChunk(helpRefText);
            if (!ignoreBadRefs)
              TexOutput(_T(" (REF NOT FOUND)"));

            // for launching twice do not warn in preparation pass
            if ((passNumber == 1 && !runTwice) ||
                (passNumber == 2 && runTwice))
            {
              wxString errBuf;
              errBuf.Printf(_T("Warning: unresolved reference '%s'"), refName);
              OnInform(errBuf);
            }
          }
        }
        else TexOutput(_T("??"));
      }
      return false;
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
        wxChar *alignment = _T("");
        if (macroId == ltIMAGEL)
          alignment = _T(" align=left");
        else if  (macroId == ltIMAGER)
          alignment = _T(" align=right");

        // Try to find an XBM or GIF image first.
        wxChar *filename = copystring(GetArgData());
        wxChar buf[500];

        wxStrcpy(buf, filename);
        StripExtension(buf);
        wxStrcat(buf, _T(".xbm"));
        wxString f = TexPathList.FindValidPath(buf);

        if (f == _T("")) // Try for a GIF instead
        {
          wxStrcpy(buf, filename);
          StripExtension(buf);
          wxStrcat(buf, _T(".gif"));
          f = TexPathList.FindValidPath(buf);
        }

        if (f == _T("")) // Try for a JPEG instead
        {
          wxStrcpy(buf, filename);
          StripExtension(buf);
          wxStrcat(buf, _T(".jpg"));
          f = TexPathList.FindValidPath(buf);
        }

        if (f == _T("")) // Try for a PNG instead
        {
          wxStrcpy(buf, filename);
          StripExtension(buf);
          wxStrcat(buf, _T(".png"));
          f = TexPathList.FindValidPath(buf);
        }

        if (f != _T(""))
        {
          wxChar *inlineFilename = copystring(f);
#if 0
          wxChar *originalFilename = TexPathList.FindValidPath(filename);
          // If we have found the existing filename, make the inline
          // image point to the original file (could be PS, for example)
          if (originalFilename && (wxStrcmp(inlineFilename, originalFilename) != 0))
          {
            TexOutput(_T("<A HREF=\""));
            TexOutput(ConvertCase(originalFilename));
            TexOutput(_T("\">"));
            TexOutput(_T("<img src=\""));
            TexOutput(ConvertCase(wxFileNameFromPath(inlineFilename)));
            TexOutput(_T("\""));
            TexOutput(alignment);
            TexOutput(_T("></A>"));
          }
          else
#endif
          {
            TexOutput(_T("<img src=\""));
            TexOutput(ConvertCase(wxFileNameFromPath(inlineFilename)));
            TexOutput(_T("\""));
            TexOutput(alignment);
            TexOutput(_T(">"));
            delete[] inlineFilename;
          }
        }
        else
        {
          // Last resort - a link to a PS file.
          TexOutput(_T("<A HREF=\""));
          TexOutput(ConvertCase(wxFileNameFromPath(filename)));
          TexOutput(_T("\">Picture</A>\n"));
          wxSnprintf(buf, sizeof(buf), _T("Warning: could not find an inline XBM/GIF for %s."), filename);
          OnInform(buf);
        }
      }
    }
    return false;
  }
  // First arg is PSBOX spec (ignored), second is image file, third is map name.
  case ltIMAGEMAP:
  {
    static wxChar *imageFile = NULL;
    if (start && (arg_no == 2))
    {
      // Try to find an XBM or GIF image first.
      wxChar *filename = copystring(GetArgData());
      wxChar buf[500];

      wxStrcpy(buf, filename);
      StripExtension(buf);
      wxStrcat(buf, _T(".xbm"));
      wxString f = TexPathList.FindValidPath(buf);

      if (f == _T("")) // Try for a GIF instead
      {
        wxStrcpy(buf, filename);
        StripExtension(buf);
        wxStrcat(buf, _T(".gif"));
        f = TexPathList.FindValidPath(buf);
      }
      if (f == _T(""))
      {
        wxChar buf[300];
        wxSnprintf(buf, sizeof(buf), _T("Warning: could not find an inline XBM/GIF for %s."), filename);
        OnInform(buf);
      }
      delete[] filename;
      if (imageFile)
        delete[] imageFile;
      imageFile = NULL;
      if (!f.empty())
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
        wxChar buf[256];
        wxStrcpy(buf, imageFile);
        StripExtension(buf);
        wxStrcat(buf, _T(".shg"));
        wxString f = TexPathList.FindValidPath(buf);

        if (f != _T(""))
        {
          // The default HTML file to go to is THIS file (so a no-op)
          SHGToMap(f, currentFileName);
        }

        wxChar *mapName = GetArgData();
        TexOutput(_T("<A HREF=\"/cgi-bin/imagemap/"));
        if (mapName)
          TexOutput(mapName);
        else
          TexOutput(_T("unknown"));
        TexOutput(_T("\">"));
        TexOutput(_T("<img src=\""));
        TexOutput(ConvertCase(wxFileNameFromPath(imageFile)));
        TexOutput(_T("\" ISMAP></A><P>"));
        delete[] imageFile;
        imageFile = NULL;
      }
    }
    return false;
  }
  case ltINDENTED :
  {
    if ( arg_no == 1 )
        return false;
    else
    {
        return true;
    }
  }
  case ltITEM:
  {
    if (start)
    {
      descriptionItemArg = GetArgChunk();
      return false;
    }
    return true;
  }
  case ltTWOCOLITEM:
  case ltTWOCOLITEMRULED:
  {
/*
    if (start && (arg_no == 1))
      TexOutput(_T("\n<DT> "));
    if (start && (arg_no == 2))
      TexOutput(_T("<DD> "));
*/
    if (arg_no == 1)
    {
      if ( start ) {
        // DHS
        if (TwoColWidthA > -1)
        {
          wxChar buf[100];
          wxSnprintf(buf, sizeof(buf), _T("\n<TR><TD VALIGN=TOP WIDTH=%d>\n"),TwoColWidthA);
          TexOutput(buf);
        }
        else
        {
          TexOutput(_T("\n<TR><TD VALIGN=TOP>\n"));
        }
        OutputFont();
      }  else
            TexOutput(_T("\n</FONT></TD>\n"));
    }
    if (arg_no == 2)
    {
      // DHS
      if ( start )
      {
        if (TwoColWidthB > -1)
        {
          wxChar buf[100];
          wxSnprintf(buf, sizeof(buf), _T("\n<TD VALIGN=TOP WIDTH=%d>\n"),TwoColWidthB);
          TexOutput(buf);
        }
        else
        {
          TexOutput(_T("\n<TD VALIGN=TOP>\n"));
        }
        OutputFont();
      }  else
           TexOutput(_T("\n</FONT></TD></TR>\n"));
    }
    return true;
  }
  case ltNUMBEREDBIBITEM:
  {
    if (arg_no == 1 && start)
    {
      TexOutput(_T("\n<DT> "));
    }
    if (arg_no == 2 && !start)
      TexOutput(_T("<P>\n"));
    break;
  }
  case ltBIBITEM:
  {
    wxChar buf[100];
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

      wxSnprintf(buf, sizeof(buf), _T("\n<DT> [%d] "), citeCount);
      TexOutput(buf);
      citeCount ++;
      return false;
    }
    if (arg_no == 2 && !start)
      TexOutput(_T("<P>\n"));
    return true;
  }
  case ltMARGINPAR:
  case ltMARGINPARODD:
  case ltMARGINPAREVEN:
  case ltNORMALBOX:
  case ltNORMALBOXD:
  {
    if (start)
    {
      TexOutput(_T("<HR>\n"));
      return true;
    }
    else
      TexOutput(_T("<HR><P>\n"));
    break;
  }
  // DHS
  case ltTWOCOLWIDTHA:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      float points = ParseUnitArgument(val);
      TwoColWidthA = (int)((points * 100.0) / 72.0);
    }
    return false;
  }
  // DHS
  case ltTWOCOLWIDTHB:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      float points = ParseUnitArgument(val);
      TwoColWidthB = (int)((points * 100.0) / 72.0);
    }
    return false;
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
           TexOutput(_T("&agrave;"));
           break;
          case 'e':
           TexOutput(_T("&egrave;"));
           break;
          case 'i':
           TexOutput(_T("&igrave;"));
           break;
          case 'o':
           TexOutput(_T("&ograve;"));
           break;
          case 'u':
           TexOutput(_T("&ugrave;"));
           break;
          case 'A':
           TexOutput(_T("&Agrave;"));
           break;
          case 'E':
           TexOutput(_T("&Egrave;"));
           break;
          case 'I':
           TexOutput(_T("&Igrave;"));
           break;
          case 'O':
           TexOutput(_T("&Ograve;"));
           break;
          case 'U':
           TexOutput(_T("&Igrave;"));
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
           TexOutput(_T("&aacute;"));
           break;
          case 'e':
           TexOutput(_T("&eacute;"));
           break;
          case 'i':
           TexOutput(_T("&iacute;"));
           break;
          case 'o':
           TexOutput(_T("&oacute;"));
           break;
          case 'u':
           TexOutput(_T("&uacute;"));
           break;
          case 'y':
           TexOutput(_T("&yacute;"));
           break;
          case 'A':
           TexOutput(_T("&Aacute;"));
           break;
          case 'E':
           TexOutput(_T("&Eacute;"));
           break;
          case 'I':
           TexOutput(_T("&Iacute;"));
           break;
          case 'O':
           TexOutput(_T("&Oacute;"));
           break;
          case 'U':
           TexOutput(_T("&Uacute;"));
           break;
          case 'Y':
           TexOutput(_T("&Yacute;"));
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
           TexOutput(_T("&acirc;"));
           break;
          case 'e':
           TexOutput(_T("&ecirc;"));
           break;
          case 'i':
           TexOutput(_T("&icirc;"));
           break;
          case 'o':
           TexOutput(_T("&ocirc;"));
           break;
          case 'u':
           TexOutput(_T("&ucirc;"));
           break;
          case 'A':
           TexOutput(_T("&Acirc;"));
           break;
          case 'E':
           TexOutput(_T("&Ecirc;"));
           break;
          case 'I':
           TexOutput(_T("&Icirc;"));
           break;
          case 'O':
           TexOutput(_T("&Ocirc;"));
           break;
          case 'U':
           TexOutput(_T("&Icirc;"));
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
          case ' ':
           TexOutput(_T("~"));
           break;
          case 'a':
           TexOutput(_T("&atilde;"));
           break;
          case 'n':
           TexOutput(_T("&ntilde;"));
           break;
          case 'o':
           TexOutput(_T("&otilde;"));
           break;
          case 'A':
           TexOutput(_T("&Atilde;"));
           break;
          case 'N':
           TexOutput(_T("&Ntilde;"));
           break;
          case 'O':
           TexOutput(_T("&Otilde;"));
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
           TexOutput(_T("&auml;"));
           break;
          case 'e':
           TexOutput(_T("&euml;"));
           break;
          case 'i':
           TexOutput(_T("&iuml;"));
           break;
          case 'o':
           TexOutput(_T("&ouml;"));
           break;
          case 'u':
           TexOutput(_T("&uuml;"));
           break;
          case 'y':
           TexOutput(_T("&yuml;"));
           break;
          case 'A':
           TexOutput(_T("&Auml;"));
           break;
          case 'E':
           TexOutput(_T("&Euml;"));
           break;
          case 'I':
           TexOutput(_T("&Iuml;"));
           break;
          case 'O':
           TexOutput(_T("&Ouml;"));
           break;
          case 'U':
           TexOutput(_T("&Uuml;"));
           break;
          case 'Y':
           TexOutput(_T("&Yuml;"));
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
           TexOutput(_T("&aring;"));
           break;
          case 'A':
           TexOutput(_T("&Aring;"));
           break;
          default:
           break;
        }
      }
    }
    return false;
  }
  case ltBACKGROUND:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      if (val)
      {
        bool isPicture = false;
        ParseColourString(val, &isPicture);
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
    return false;
  }
  case ltBACKGROUNDIMAGE:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      if (val)
      {
        if (backgroundImageString)
          delete[] backgroundImageString;
        backgroundImageString = copystring(val);
      }
    }
    return false;
  }
  case ltBACKGROUNDCOLOUR:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      if (val)
      {
        if (backgroundColourString)
          delete[] backgroundColourString;
        backgroundColourString = copystring(val);
      }
    }
    return false;
  }
  case ltTEXTCOLOUR:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      if (val)
      {
        if (textColourString)
          delete[] textColourString;
        textColourString = copystring(val);
      }
    }
    return false;
  }
  case ltLINKCOLOUR:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      if (val)
      {
        if (linkColourString)
          delete[] linkColourString;
        linkColourString = copystring(val);
      }
    }
    return false;
  }
  case ltFOLLOWEDLINKCOLOUR:
  {
    if (start)
    {
      wxChar *val = GetArgData();
      if (val)
      {
        if (followedLinkColourString)
          delete[] followedLinkColourString;
        followedLinkColourString = copystring(val);
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
           TexOutput(_T("&ccedil;"));
           break;
          case 'C':
           TexOutput(_T("&Ccedil;"));
           break;
          default:
           break;
        }
      }
    }
    return false;
  }
/*
  case ltFOOTNOTE:
  case ltFOOTNOTEPOPUP:
  {
    if (arg_no == 1)
      return true;
    else
      return false;
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
        inTabular = true;
        startRows = true;
        tableVerticalLineLeft = false;
        tableVerticalLineRight = false;

        wxChar *alignString = copystring(GetArgData());
        ParseTableArgument(alignString);

        TexOutput(_T("<TABLE BORDER>\n"));

        // Write the first row formatting for compatibility
        // with standard Latex
        if (compatibilityMode)
        {
          TexOutput(_T("<TR>\n<TD>"));
          OutputFont();
/*
          for (int i = 0; i < noColumns; i++)
          {
            currentWidth += TableData[i].width;
            wxSnprintf(buf, sizeof(buf), _T("\\cellx%d"), currentWidth);
            TexOutput(buf);
          }
          TexOutput(_T("\\pard\\intbl\n"));
*/
        }
        delete[] alignString;

        return false;
      }
    }
    else if (arg_no == 2 && !start)
    {
      TexOutput(_T("</TABLE>\n"));
      inTabular = false;
    }
    break;
  }
  case ltTHEBIBLIOGRAPHY:
  {
    if (start && (arg_no == 1))
    {
      ReopenFile(&Chapters, &ChaptersName, _T("bibliography"));
      AddTexRef(_T("bibliography"), ChaptersName, _T("bibliography"));
      SetCurrentSubsectionName(_T("bibliography"), ChaptersName);

      citeCount = 1;

      SetCurrentOutput(Chapters);

      wxChar titleBuf[150];
      if (truncateFilenames)
        wxSnprintf(titleBuf, sizeof(titleBuf), _T("%s.htm"), wxFileNameFromPath(FileRoot));
      else
        wxSnprintf(titleBuf, sizeof(titleBuf), _T("%s_contents.html"), wxFileNameFromPath(FileRoot));

      HTMLHead();
      TexOutput(_T("<title>"));
      TexOutput(ReferencesNameString);
      TexOutput(_T("</title></head>\n"));
      OutputBodyStart();

      wxFprintf(Chapters, _T("<A NAME=\"%s\">\n<H2>%s"), _T("bibliography"), ReferencesNameString);
      AddBrowseButtons(_T("contents"), titleBuf, // Up
                       lastTopic, lastFileName,  // Last topic
                       _T("bibliography"), ChaptersName); // This topic

      SetCurrentOutputs(Contents, Chapters);
      if(PrimaryAnchorOfTheFile(ChaptersName, _T("bibliography")))
        wxFprintf(Contents, _T("\n<LI><A HREF=\"%s\">"), ConvertCase(ChaptersName));
      else
        wxFprintf(Contents, _T("\n<LI><A HREF=\"%s#%s\">"), ConvertCase(ChaptersName), _T("bibliography"));

      wxFprintf(Contents, _T("%s</A>\n"), ReferencesNameString);
      wxFprintf(Chapters, _T("</H2>\n</A>\n"));

      SetCurrentOutput(Chapters);
      return false;
    }
    if (!start && (arg_no == 2))
    {
    }
    return true;
  }
  case ltINDEX:
  {
    /* Build up list of keywords associated with topics */
    if (start)
    {
//      wxChar *entry = GetArgData();
      wxChar buf[300];
      OutputChunkToString(GetArgChunk(), buf);
      if (CurrentTopic)
      {
        AddKeyWordForTopic(CurrentTopic, buf, currentFileName);
      }
    }
    return false;
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
          wxChar *name = GetArgData();
          wxChar buf2[10];
          if (!FindColourHTMLString(name, buf2))
          {
            wxStrcpy(buf2, _T("#000000"));
            wxChar buf[100];
            wxSnprintf(buf, sizeof(buf), _T("Could not find colour name %s"), name);
            OnError(buf);
          }
          TexOutput(_T("<FONT COLOR=\""));
          TexOutput(buf2);
          TexOutput(_T("\">"));
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
      if (arg_no == 2) TexOutput(_T("</FONT>"));
    }
    return false;
  }
  case ltINSERTATLEVEL:
  {
    // This macro allows you to insert text at a different level
    // from the current level, e.g. into the Sections from within a subsubsection.
    if (useWord)
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

bool HTMLGo(void)
{
    fileId = 0;
    inVerbatim = false;
    indentLevel = 0;
    inTabular = false;
    startRows = false;
    tableVerticalLineLeft = false;
    tableVerticalLineRight = false;
    noColumns = 0;

    if (!InputFile.empty() && !OutputFile.empty())
    {
        // Do some HTML-specific transformations on all the strings,
        // recursively
        Text2HTML(GetTopLevelChunk());

        wxChar buf[300];
        if (truncateFilenames)
            wxSnprintf(buf, sizeof(buf), _T("%s.htm"), FileRoot);
        else
            wxSnprintf(buf, sizeof(buf), _T("%s_contents.html"), FileRoot);
        if (TitlepageName) delete[] TitlepageName;
        TitlepageName = copystring(buf);
        Titlepage = wxFopen(buf, _T("w"));

        if (truncateFilenames)
            wxSnprintf(buf, sizeof(buf), _T("%s_fc.htm"), FileRoot);
        else
            wxSnprintf(buf, sizeof(buf), _T("%s_fcontents.html"), FileRoot);

        contentsFrameName = copystring(buf);

        Contents = wxFopen(TmpContentsName, _T("w"));

        if (htmlFrameContents)
        {
//          FrameContents = wxFopen(TmpFrameContentsName, _T("w"));
            FrameContents = wxFopen(contentsFrameName, _T("w"));
            wxFprintf(FrameContents, _T("<HTML>\n<UL>\n"));
        }

        if (!Titlepage || !Contents)
        {
            OnError(_T("Cannot open output file!"));
            return false;
        }
        AddTexRef(_T("contents"), wxFileNameFromPath(TitlepageName), ContentsNameString);

        wxFprintf(Contents, _T("<P><P><H2>%s</H2><P><P>\n"), ContentsNameString);

        wxFprintf(Contents, _T("<UL>\n"));

        SetCurrentOutput(Titlepage);
        if (htmlWorkshopFiles) HTMLWorkshopStartContents();
        OnInform(_T("Converting..."));

        TraverseDocument();
        wxFprintf(Contents, _T("</UL>\n\n"));

//        SetCurrentOutput(Titlepage);
        fclose(Titlepage);

        if (Contents)
        {
//            wxFprintf(Titlepage, _T("\n</BODY></HTML>\n"));
            fclose(Contents);
            Contents = NULL;
        }

        if (FrameContents)
        {
            wxFprintf(FrameContents, _T("\n</UL>\n"));
            wxFprintf(FrameContents, _T("</HTML>\n"));
            fclose(FrameContents);
            FrameContents = NULL;
        }

        if (Chapters)
        {
            wxFprintf(Chapters, _T("\n</FONT></BODY></HTML>\n"));
            fclose(Chapters);
            Chapters = NULL;
        }
        if (Sections)
        {
            wxFprintf(Sections, _T("\n</FONT></BODY></HTML>\n"));
            fclose(Sections);
            Sections = NULL;
        }
        if (Subsections && !combineSubSections)
        {
            wxFprintf(Subsections, _T("\n</FONT></BODY></HTML>\n"));
            fclose(Subsections);
            Subsections = NULL;
        }
        if (Subsubsections && !combineSubSections)
        {
            wxFprintf(Subsubsections, _T("\n</FONT></BODY></HTML>\n"));
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
        FILE *tmpTitle = wxFopen(_T("title.tmp"), _T("w"));
        if (tmpTitle)
        {
            if (DocumentTitle)
            {
                SetCurrentOutput(tmpTitle);
                HTMLHead();
                TexOutput(_T("\n<TITLE>"));
                TraverseChildrenFromChunk(DocumentTitle);
                TexOutput(_T("</TITLE></HEAD>\n"));
            }
            else
            {
                SetCurrentOutput(tmpTitle);
                HTMLHeadTo(tmpTitle);
                if (contentsString)
                    wxFprintf(tmpTitle, _T("<TITLE>%s</TITLE></HEAD>\n\n"), contentsString);
                else
                    wxFprintf(tmpTitle, _T("<TITLE>%s</TITLE></HEAD>\n\n"), wxFileNameFromPath(FileRoot));
            }

            // Output frame information
            if (htmlFrameContents)
            {
                wxChar firstFileName[300];
                if (truncateFilenames)
                    wxSnprintf(firstFileName, sizeof(firstFileName), _T("%s1.htm"), FileRoot);
                else
                    wxStrcpy(firstFileName, gs_filenames[1].c_str());

                wxFprintf(tmpTitle, _T("<FRAMESET COLS=\"30%%,70%%\">\n"));

                wxFprintf(tmpTitle, _T("<FRAME SRC=\"%s\">\n"), ConvertCase(wxFileNameFromPath(contentsFrameName)));
                wxFprintf(tmpTitle, _T("<FRAME SRC=\"%s\" NAME=\"mainwindow\">\n"), ConvertCase(wxFileNameFromPath(firstFileName)));
                wxFprintf(tmpTitle, _T("</FRAMESET>\n"));

                wxFprintf(tmpTitle, _T("<NOFRAMES>\n"));
            }

            // Output <BODY...> to temporary title page
            OutputBodyStart();
            fflush(tmpTitle);

            // Concat titlepage
            FILE *fd = wxFopen(TitlepageName, _T("r"));
            if (fd)
            {
                int ch = getc(fd);
                while (ch != EOF)
                {
                    wxPutc(ch, tmpTitle);
                    ch = getc(fd);
                }
                fclose(fd);
            }

            wxFprintf(tmpTitle, _T("\n</FONT></BODY>\n"));

            if (htmlFrameContents)
            {
                wxFprintf(tmpTitle, _T("\n</NOFRAMES>\n"));
            }
            wxFprintf(tmpTitle, _T("\n</HTML>\n"));

            fclose(tmpTitle);
            if (wxFileExists(TitlepageName)) wxRemoveFile(TitlepageName);
            if (!wxRenameFile(_T("title.tmp"), TitlepageName))
            {
                wxCopyFile(_T("title.tmp"), TitlepageName);
                wxRemoveFile(_T("title.tmp"));
            }
        }

        if (lastFileName) delete[] lastFileName;
        lastFileName = NULL;
        if (lastTopic) delete[] lastTopic;
        lastTopic = NULL;

        if (wxFileExists(ContentsName)) wxRemoveFile(ContentsName);

        if (!wxRenameFile(TmpContentsName, ContentsName))
        {
            wxCopyFile(TmpContentsName, ContentsName);
            wxRemoveFile(TmpContentsName);
        }

        // Generate .htx file if requested
        if (htmlIndex)
        {
            wxChar htmlIndexName[300];
            wxSnprintf(htmlIndexName, sizeof(htmlIndexName), _T("%s.htx"), FileRoot);
            GenerateHTMLIndexFile(htmlIndexName);
        }

        // Generate HTML Help Workshop files if requested
        if (htmlWorkshopFiles)
        {
            HTMLWorkshopEndContents();
            GenerateHTMLWorkshopFiles(FileRoot);
        }

        return true;
    }

    return false;
}

// Output .htx index file
void GenerateHTMLIndexFile(wxChar *fname)
{
  FILE *fd = wxFopen(fname, _T("w"));
  if (!fd)
    return;

  TopicTable.BeginFind();
  wxHashTable::Node *node = TopicTable.Next();
  while (node)
  {
    TexTopic *texTopic = (TexTopic *)node->GetData();
    const wxChar *topicName = node->GetKeyString();
    if (texTopic->filename && texTopic->keywords)
    {
      wxStringListNode *node1 = texTopic->keywords->GetFirst();
      while (node1)
      {
        wxChar *s = (wxChar *)node1->GetData();
        wxFprintf(fd, _T("%s|%s|%s\n"), topicName, texTopic->filename, s);
        node1 = node1->GetNext();
      }
    }
    node = TopicTable.Next();
  }
  fclose(fd);
}







// output .hpp, .hhc and .hhk files:


void GenerateHTMLWorkshopFiles(wxChar *fname)
{
  FILE *f;
  wxChar buf[300];

  /* Generate project file : */

  wxSnprintf(buf, sizeof(buf), _T("%s.hhp"), fname);
  f = wxFopen(buf, _T("wt"));
  wxFprintf(f,
      _T("[OPTIONS]\n")
      _T("Compatibility=1.1\n")
      _T("Full-text search=Yes\n")
      _T("Contents file=%s.hhc\n")
      _T("Compiled file=%s.chm\n")
      _T("Default Window=%sHelp\n")
      _T("Default topic=%s\n")
      _T("Index file=%s.hhk\n")
      _T("Title="),
      wxFileNameFromPath(fname),
      wxFileNameFromPath(fname),
      wxFileNameFromPath(fname),
      wxFileNameFromPath(TitlepageName),
      wxFileNameFromPath(fname)
      );

  if (DocumentTitle) {
    SetCurrentOutput(f);
    TraverseChildrenFromChunk(DocumentTitle);
  }
  else wxFprintf(f, _T("(unknown)"));

  wxFprintf(f, _T("\n\n[WINDOWS]\n")
          _T("%sHelp=,\"%s.hhc\",\"%s.hhk\",\"%s\",,,,,,0x2420,,0x380e,,,,,0,,,"),
          wxFileNameFromPath(fname),
          wxFileNameFromPath(fname),
          wxFileNameFromPath(fname),
          wxFileNameFromPath(TitlepageName));


  wxFprintf(f, _T("\n\n[FILES]\n"));
  wxFprintf(f, _T("%s\n"), wxFileNameFromPath(TitlepageName));
  for (int i = 1; i <= fileId; i++) {
    if (truncateFilenames)
      wxSnprintf(buf, sizeof(buf), _T("%s%d.htm"), wxFileNameFromPath(FileRoot), i);
    else
      wxStrcpy(buf, wxFileNameFromPath(gs_filenames[i].c_str()));
    wxFprintf(f, _T("%s\n"), buf);
  }
  fclose(f);

  /* Generate index file : */

  wxSnprintf(buf, sizeof(buf), _T("%s.hhk"), fname);
  f = wxFopen(buf, _T("wt"));

  wxFprintf(f,
      _T("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n")
      _T("<HTML>\n"));
  HTMLHeadTo(f);
  wxFprintf(f,
      _T("\n")
      _T("<meta name=\"GENERATOR\" content=\"tex2rtf\">\n")
      _T("<!-- Sitemap 1.0 -->\n")
      _T("</HEAD><BODY>\n")
      _T("<OBJECT type=\"text/site properties\">\n")
      _T(" <param name=\"ImageType\" value=\"Folder\">\n")
      _T("</OBJECT>\n")
      _T("<UL>\n"));

  TopicTable.BeginFind();
  wxHashTable::Node *node = TopicTable.Next();
  while (node)
  {
    TexTopic *texTopic = (TexTopic *)node->GetData();
    const wxChar *topicName = node->GetKeyString();
    if (texTopic->filename && texTopic->keywords)
    {
      wxStringListNode *node1 = texTopic->keywords->GetFirst();
      while (node1)
      {
        wxChar *s = (wxChar *)node1->GetData();
        wxFprintf(f,
            _T(" <LI> <OBJECT type=\"text/sitemap\">\n")
            _T("  <param name=\"Local\" value=\"%s#%s\">\n")
            _T("  <param name=\"Name\" value=\"%s\">\n")
            _T("  </OBJECT>\n"),
        texTopic->filename, topicName, s);
        node1 = node1->GetNext();
      }
    }
    node = TopicTable.Next();
  }

  wxFprintf(f, _T("</UL>\n"));
  fclose(f);
}



static FILE *HTMLWorkshopContents = NULL;
static int HTMLWorkshopLastLevel = 0;

void HTMLWorkshopAddToContents(int level, wxChar *s, wxChar *file)
{
  int i;

  if (level > HTMLWorkshopLastLevel)
    for (i = HTMLWorkshopLastLevel; i < level; i++)
      wxFprintf(HTMLWorkshopContents, _T("<UL>"));
  if (level < HTMLWorkshopLastLevel)
    for (i = level; i < HTMLWorkshopLastLevel; i++)
      wxFprintf(HTMLWorkshopContents, _T("</UL>"));

  SetCurrentOutput(HTMLWorkshopContents);
  wxFprintf(HTMLWorkshopContents,
            _T(" <LI> <OBJECT type=\"text/sitemap\">\n")
            _T("  <param name=\"Local\" value=\"%s#%s\">\n")
            _T("  <param name=\"Name\" value=\""),
            file, s);
  OutputCurrentSection();
  wxFprintf(HTMLWorkshopContents,
            _T("\">\n")
            _T("  </OBJECT>\n"));
  HTMLWorkshopLastLevel = level;
}


void HTMLWorkshopStartContents()
{
  wxChar buf[300];
  wxSnprintf(buf, sizeof(buf), _T("%s.hhc"), FileRoot);
  HTMLWorkshopContents = wxFopen(buf, _T("wt"));
  HTMLWorkshopLastLevel = 0;

  wxFprintf(HTMLWorkshopContents,
      _T("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n")
      _T("<HTML>\n"));
  HTMLHeadTo(HTMLWorkshopContents);
  wxFprintf(HTMLWorkshopContents,
      _T("\n")
      _T("<meta name=\"GENERATOR\" content=\"tex2rtf\">\n")
      _T("<!-- Sitemap 1.0 -->\n")
      _T("</HEAD><BODY>\n")
      _T("<OBJECT type=\"text/site properties\">\n")
      _T(" <param name=\"ImageType\" value=\"Folder\">\n")
      _T("</OBJECT>\n")
      _T("<UL>\n")
      _T("<LI> <OBJECT type=\"text/sitemap\">\n")
      _T("<param name=\"Local\" value=\"%s\">\n")
      _T("<param name=\"Name\" value=\"Contents\">\n</OBJECT>\n"),
      wxFileNameFromPath(TitlepageName)
      );

}


void HTMLWorkshopEndContents()
{
    for (int i = HTMLWorkshopLastLevel; i >= 0; i--)
        wxFprintf(HTMLWorkshopContents, _T("</UL>\n"));
    fclose(HTMLWorkshopContents);
}


bool PrimaryAnchorOfTheFile( wxChar *file, wxChar *label )
{
    wxString file_label;
    file_label.Printf( HTML_FILENAME_PATTERN, FileRoot, label );
    return file_label.IsSameAs( file , false );
}

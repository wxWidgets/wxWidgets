/////////////////////////////////////////////////////////////////////////////
// Name:        texutils.cpp
// Purpose:     Miscellaneous utilities
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
    #include "wx/log.h"
#endif

#include "wx/app.h"
#include "wx/hash.h"

#ifdef new
#undef new
#endif

#if wxUSE_IOSTREAMH
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
using namespace std;
#endif

#include <ctype.h>
#include "tex2any.h"

#if !WXWIN_COMPATIBILITY_2_4
static inline wxChar* copystring(const wxChar* s)
    { return wxStrcpy(new wxChar[wxStrlen(s) + 1], s); }
static inline void StringToInt (const wxChar *s, int *number)
{
  if (s && *s && number)
    *number = (int) wxStrtol (s, (wxChar **) NULL, 10);
}
#endif

wxHashTable TexReferences(wxKEY_STRING);
wxList BibList(wxKEY_STRING);
wxStringList CitationList;
wxList ColourTable(wxKEY_STRING);
wxHashTable BibStringTable(wxKEY_STRING);
wxList CustomMacroList(wxKEY_STRING);
TexChunk *currentSection = NULL;
wxChar *fakeCurrentSection = NULL;

static long BibLine = 1;

void OutputCurrentSection(void)
{
  if (fakeCurrentSection)
    TexOutput(fakeCurrentSection);
  else if (currentSection)
    TraverseChildrenFromChunk(currentSection);
}

// Nasty but the way things are done now, necessary,
// in order to output a chunk properly to a string (macros and all).
void OutputCurrentSectionToString(wxChar *buf)
{
    if (fakeCurrentSection)
        wxStrcpy(buf, fakeCurrentSection);
    else
        OutputChunkToString(currentSection, buf);
}

void OutputChunkToString(TexChunk *chunk, wxChar *buf)
{
  FILE *tempfd = wxFopen(_T("tmp.tmp"), _T("w"));
  if (!tempfd)
    return;
    
  FILE *old1 = CurrentOutput1;
  FILE *old2 = CurrentOutput2;
  
  CurrentOutput1 = tempfd;
  CurrentOutput2 = NULL;
  
  TraverseChildrenFromChunk(chunk);
    
  CurrentOutput1 = old1;
  CurrentOutput2 = old2;
  
  fclose(tempfd);
  
  // Read from file into string
  tempfd = wxFopen(_T("tmp.tmp"), _T("r"));
  if (!tempfd)
    return;

  buf[0] = 0;
  int ch = -2;
  int i = 0;
  while (ch != EOF)
  {
    ch = getc(tempfd);
    if (ch == EOF)
      buf[i] = 0;
    else
    {
      buf[i] = ch;
      i ++;
    }
  }
  fclose(tempfd);
  wxRemoveFile(_T("tmp.tmp"));
}

// Called by Tex2Any to simulate a section
void FakeCurrentSection(wxChar *fakeSection, bool addToContents)
{
  currentSection = NULL;
  if (fakeCurrentSection) delete[] fakeCurrentSection;
  fakeCurrentSection = copystring(fakeSection);

  if (DocumentStyle == LATEX_ARTICLE)
  {
    int mac = ltSECTIONHEADING;
    if (!addToContents)
      mac = ltSECTIONHEADINGSTAR;
    OnMacro(mac, 0, TRUE);
    OnMacro(mac, 0, FALSE);
  }
  else
  {
    int mac = ltCHAPTERHEADING;
    if (!addToContents)
      mac = ltCHAPTERHEADINGSTAR;
    OnMacro(mac, 0, TRUE);
    OnMacro(mac, 0, FALSE);
  }
  if (fakeCurrentSection) delete[] fakeCurrentSection;
  fakeCurrentSection = NULL;
}

// Look for \label macro, use this ref name if found or
// make up a topic name otherwise.
static long topicCounter = 0;

void ResetTopicCounter(void)
{
  topicCounter = 0;
}

static wxChar *forceTopicName = NULL;

void ForceTopicName(const wxChar *name)
{
  if (forceTopicName)
    delete[] forceTopicName;
  if (name)
    forceTopicName = copystring(name);
  else
    forceTopicName = NULL;
}

wxChar *FindTopicName(TexChunk *chunk)
{
  if (forceTopicName)
    return forceTopicName;
    
  wxChar *topicName = NULL;
  static wxChar topicBuf[100];

  if (chunk && (chunk->type == CHUNK_TYPE_MACRO) &&
      (chunk->macroId == ltLABEL))
  {
    wxNode *node = chunk->children.GetFirst();
    if (node)
    {
      TexChunk *child = (TexChunk *)node->GetData();
      if (child->type == CHUNK_TYPE_ARG)
      {
        wxNode *snode = child->children.GetFirst();
        if (snode)
        {
          TexChunk *schunk = (TexChunk *)snode->GetData();
          if (schunk->type == CHUNK_TYPE_STRING)
            topicName = schunk->value;
        }
      }
    }
  }
  if (topicName)
    return topicName;
  else
  {
    wxSprintf(topicBuf, _T("topic%ld"), topicCounter);
    topicCounter ++;
    return topicBuf;
  }
}

/*
 * Simulate argument data, so we can 'drive' clients which implement
 * certain basic formatting behaviour.
 * Snag is that some save a TexChunk, so don't use yet...
 *
 */
 
void StartSimulateArgument(wxChar *data)
{
  wxStrcpy(currentArgData, data);
  haveArgData = TRUE;
}

void EndSimulateArgument(void)
{
  haveArgData = FALSE;
}

/*
 * Parse and convert unit arguments to points
 *
 */

int ParseUnitArgument(wxChar *unitArg)
{
  float conversionFactor = 1.0;
  float unitValue = 0.0;
  int len = wxStrlen(unitArg);
  // Get rid of any accidentally embedded commands
  for (int i = 0; i < len; i++)
    if (unitArg[i] == '\\')
      unitArg[i] = 0;
  len = wxStrlen(unitArg);
      
  if (unitArg && (len > 0) && (isdigit(unitArg[0]) || unitArg[0] == '-'))
  {
    wxSscanf(unitArg, _T("%f"), &unitValue);
    if (len > 1)
    {
      wxChar units[3]; 
      units[0] = unitArg[len-2];
      units[1] = unitArg[len-1];
      units[2] = 0;
      if (wxStrcmp(units, _T("in")) == 0)
        conversionFactor = 72.0;
      else if (wxStrcmp(units, _T("cm")) == 0)
        conversionFactor = (float)72.0/(float)2.51;
      else if (wxStrcmp(units, _T("mm")) == 0)
        conversionFactor = (float)72.0/(float)25.1;
      else if (wxStrcmp(units, _T("pt")) == 0)
        conversionFactor = 1;
    }
    return (int)(unitValue*conversionFactor);
  }
  else return 0;
}

/*
 * Strip off any extension (dot something) from end of file,
 * IF one exists. Inserts zero into buffer.
 *
 */
 
void StripExtension(wxChar *buffer)
{
  int len = wxStrlen(buffer);
  int i = len-1;
  while (i > 0)
  {
    if (buffer[i] == '.')
    {
      buffer[i] = 0;
      break;
    }
    i --;
  }
}

/*
 * Latex font setting
 *
 */

void SetFontSizes(int pointSize)
{
  switch (pointSize)
  {
    case 12:
    {
      normalFont = 12;
      smallFont = 10;
      tinyFont = 8;
      largeFont1 = 14;
      LargeFont2 = 16;
      LARGEFont3 = 20;
      hugeFont1 = 24;
      HugeFont2 = 28;
      HUGEFont3 = 32;
      break;
    }
    case 11:
    {
      normalFont = 11;
      smallFont = 9;
      tinyFont = 7;
      largeFont1 = 13;
      LargeFont2 = 16;
      LARGEFont3 = 19;
      hugeFont1 = 22;
      HugeFont2 = 26;
      HUGEFont3 = 30;
      break;
    }
    case 10:
    {
      normalFont = 10;
      smallFont = 8;
      tinyFont = 6;
      largeFont1 = 12;
      LargeFont2 = 14;
      LARGEFont3 = 18;
      hugeFont1 = 20;
      HugeFont2 = 24;
      HUGEFont3 = 28;
      break;
    }
  }
}

 
/*
 * Latex references
 *
 */
 
void AddTexRef(wxChar *name, wxChar *file, wxChar *sectionName,
               int chapter, int section, int subsection, int subsubsection)
{
  TexRef *texRef = (TexRef *)TexReferences.Get(name);
  if (texRef) TexReferences.Delete(name);
  
  wxChar buf[100];
  buf[0] = 0;
/*
  if (sectionName)
  {
    wxStrcat(buf, sectionName);
    wxStrcat(buf, " ");
  }
*/
  if (chapter)
  {
    wxChar buf2[10];
    wxSprintf(buf2, _T("%d"), chapter);
    wxStrcat(buf, buf2);
  }
  if (section)
  {
    wxChar buf2[10];
    if (chapter)
      wxStrcat(buf, _T("."));

    wxSprintf(buf2, _T("%d"), section);
    wxStrcat(buf, buf2);
  }
  if (subsection)
  {
    wxChar buf2[10];
    wxStrcat(buf, _T("."));
    wxSprintf(buf2, _T("%d"), subsection);
    wxStrcat(buf, buf2);
  }
  if (subsubsection)
  {
    wxChar buf2[10];
    wxStrcat(buf, _T("."));
    wxSprintf(buf2, _T("%d"), subsubsection);
    wxStrcat(buf, buf2);
  }
  wxChar *tmp = ((wxStrlen(buf) > 0) ? buf : (wxChar *)NULL);
  TexReferences.Put(name, new TexRef(name, file, tmp, sectionName));
}

void WriteTexReferences(wxChar *filename)
{
  wxSTD ofstream ostr(filename);
  if (ostr.bad()) return;
  wxChar buf[200];
  
  TexReferences.BeginFind();
  wxNode *node = TexReferences.Next();
  while (node)
  {
    Tex2RTFYield();
    TexRef *ref = (TexRef *)node->GetData();
    ostr << ref->refLabel << _T(" ") << (ref->refFile ? ref->refFile : _T("??")) << _T(" ");
    ostr << (ref->sectionName ? ref->sectionName : _T("??")) << _T(" ");
    ostr << (ref->sectionNumber ? ref->sectionNumber : _T("??")) << _T("\n");
    if (!ref->sectionNumber || (wxStrcmp(ref->sectionNumber, _T("??")) == 0 && wxStrcmp(ref->sectionName, _T("??")) == 0))
    {
      wxSprintf(buf, _T("Warning: reference %s not resolved."), ref->refLabel);
      OnInform(buf);
    }
    node = TexReferences.Next();
  }
}

void ReadTexReferences(wxChar *filename)
{
  if (!wxFileExists(filename))
      return;

  wxSTD ifstream istr(filename, wxSTD ios::in);

  if (istr.bad()) return;

  wxChar label[100];
  wxChar file[400];
  wxChar section[100];
  wxChar sectionName[100];

  while (!istr.eof())
  {
    istr >> label;
    if (!istr.eof())
    {
      istr >> file;
	  istr >> sectionName;
      char ch;
      istr.get(ch); // Read past space
      istr.get(ch);
      int i = 0;
      while (ch != '\n' && !istr.eof())
      {
        section[i] = ch;
        i ++;
        istr.get(ch);
      }
      section[i] = 0;

      // gt - needed to trick the hash table "TexReferences" into deleting the key 
      // strings it creates in the Put() function, but not the item that is
      // created here, as that is destroyed elsewhere.  Without doing this, there
      // were massive memory leaks
      TexReferences.DeleteContents(TRUE);
      TexReferences.Put(label, new TexRef(label, file, section, sectionName));
      TexReferences.DeleteContents(FALSE);
    }
  }
}


/*
 * Bibliography-handling code
 *
 */

void BibEatWhiteSpace(wxSTD istream& str)
{
  char ch = str.peek();
  
  while (!str.eof() && (ch == ' ' || ch == '\t' || ch == 13 || ch == 10 || ch == EOF))
  {
    if (ch == 10)
      BibLine ++;
    str.get(ch);
    if ((ch == EOF) || str.eof()) return;
    ch = str.peek();
  }

  // Ignore end-of-line comments
  if (ch == '%' || ch == ';' || ch == '#')
  {
    str.get(ch);
    ch = str.peek();
    while (ch != 10 && ch != 13 && !str.eof())
    {
      str.get(ch);
      ch = str.peek();
    }
    BibEatWhiteSpace(str);
  }
}

// Read word up to { or , or space
void BibReadWord(wxSTD istream& istr, wxChar *buffer)
{
  int i = 0;
  buffer[i] = 0;
  char ch = istr.peek();
  while (!istr.eof() && ch != ' ' && ch != '{' && ch != '(' && ch != 13 && ch != 10 && ch != '\t' &&
         ch != ',' && ch != '=')
  {
    istr.get(ch);
    buffer[i] = ch;
    i ++;
    ch = istr.peek();
  }
  buffer[i] = 0;
}

// Read string (double-quoted or not) to end quote or EOL
void BibReadToEOL(wxSTD istream& istr, wxChar *buffer)
{
  int i = 0;
  buffer[i] = 0;
  wxChar ch = istr.peek();
  bool inQuotes = FALSE;
  if (ch == _T('"'))
  {
    istr.get(ch);
    ch = istr.peek();
    inQuotes = TRUE;
  }
  // If in quotes, read white space too. If not,
  // stop at white space or comment.
  while (!istr.eof() && ch != 13 && ch != 10 && ch != _T('"') &&
         (inQuotes || ((ch != _T(' ')) && (ch != 9) &&
                        (ch != _T(';')) && (ch != _T('%')) && (ch != _T('#')))))
  {
    istr.get(ch);
    buffer[i] = ch;
    i ++;
    ch = istr.peek();
  }
  if (ch == '"')
    istr.get(ch);
  buffer[i] = 0;
}

// Read }-terminated value, taking nested braces into account.
void BibReadValue(wxSTD istream& istr, wxChar *buffer, bool ignoreBraces = TRUE,
                  bool quotesMayTerminate = TRUE)
{
  int braceCount = 1;
  int i = 0;
  buffer[i] = 0;
  char ch = istr.peek();
  bool stopping = FALSE;
  while (!istr.eof() && !stopping)
  {
//    i ++;
    if (i >= 4000)
    {
      wxChar buf[100];
      wxSprintf(buf, _T("Sorry, value > 4000 chars in bib file at line %ld."), BibLine);
      wxLogError(buf, "Tex2RTF Fatal Error");
      return;
    }
    istr.get(ch);
    
    if (ch == '{')
      braceCount ++;

    if (ch == '}')
    {
      braceCount --;
      if (braceCount == 0)
      {
        stopping = TRUE;
        break;
      }
    }
    else if (quotesMayTerminate && ch == '"')
    {
      stopping = TRUE;
      break;
    }
    if (!stopping)
    {
      if (!ignoreBraces || (ch != '{' && ch != '}'))
      {
        buffer[i] = ch;
        i ++;
      }
    }
    if (ch == 10)
      BibLine ++;
  }
  buffer[i] = 0;
  wxUnusedVar(stopping);
}
 
bool ReadBib(wxChar *filename)
{
  if (!wxFileExists(filename))
      return FALSE;

  wxChar buf[300];
  wxSTD ifstream istr(filename, wxSTD ios::in);
  if (istr.bad()) return FALSE;

  BibLine = 1;

  OnInform(_T("Reading .bib file..."));

  char ch;
  wxChar fieldValue[4000];
  wxChar recordType[100];
  wxChar recordKey[100];
  wxChar recordField[100];
  while (!istr.eof())
  {
    Tex2RTFYield();

    BibEatWhiteSpace(istr);
    istr.get(ch);
    if (ch != '@')
    {
      wxSprintf(buf, _T("Expected @: malformed bib file at line %ld (%s)"), BibLine, filename);
      OnError(buf);
      return FALSE;
    }
    BibReadWord(istr, recordType);
    BibEatWhiteSpace(istr);
    istr.get(ch);
    if (ch != '{' && ch != '(')
    {
      wxSprintf(buf, _T("Expected { or ( after record type: malformed .bib file at line %ld (%s)"), BibLine, filename);
      OnError(buf);
      return FALSE;
    }
    BibEatWhiteSpace(istr);
    if (StringMatch(recordType, _T("string"), FALSE, TRUE))
    {
      BibReadWord(istr, recordType);
      BibEatWhiteSpace(istr);
      istr.get(ch);
      if (ch != '=')
      {
        wxSprintf(buf, _T("Expected = after string key: malformed .bib file at line %ld (%s)"), BibLine, filename);
        OnError(buf);
        return FALSE;
      }
      BibEatWhiteSpace(istr);
      istr.get(ch);
      if (ch != '"' && ch != '{')
      {
        wxSprintf(buf, _T("Expected = after string key: malformed .bib file at line %ld (%s)"), BibLine, filename);
        OnError(buf);
        return FALSE;
      }
      BibReadValue(istr, fieldValue);

      // Now put in hash table if necesary
      if (!BibStringTable.Get(recordType))
        BibStringTable.Put(recordType, (wxObject *)copystring(fieldValue));

      // Read closing ) or }
      BibEatWhiteSpace(istr);
      istr.get(ch);
      BibEatWhiteSpace(istr);
    }
    else
    {
      BibReadWord(istr, recordKey);

      BibEntry *bibEntry = new BibEntry;
      bibEntry->key = copystring(recordKey);
      bibEntry->type = copystring(recordType);

      bool moreRecords = TRUE;
      while (moreRecords && !istr.eof())
      {
        BibEatWhiteSpace(istr);
        istr.get(ch);
        if (ch == '}' || ch == ')')
        {
          moreRecords = FALSE;
        }
        else if (ch == ',')
        {
          BibEatWhiteSpace(istr);
          BibReadWord(istr, recordField);
          BibEatWhiteSpace(istr);
          istr.get(ch);
          if (ch != '=')
          {
            wxSprintf(buf, _T("Expected = after field type: malformed .bib file at line %ld (%s)"), BibLine, filename);
            OnError(buf);
            return FALSE;
          }
          BibEatWhiteSpace(istr);
          istr.get(ch);
          if (ch != '{' && ch != '"')
          {
            fieldValue[0] = ch;
            BibReadWord(istr, fieldValue+1);

            // If in the table of strings, replace with string from table.
            wxChar *s = (wxChar *)BibStringTable.Get(fieldValue);
            if (s)
            {
              wxStrcpy(fieldValue, s);
            }
          }
          else
            BibReadValue(istr, fieldValue, TRUE, (ch == _T('"') ? TRUE : FALSE));

          // Now we can add a field
          if (StringMatch(recordField, _T("author"), FALSE, TRUE))
            bibEntry->author = copystring(fieldValue);
          else if (StringMatch(recordField, _T("key"), FALSE, TRUE))
            {}
          else if (StringMatch(recordField, _T("annotate"), FALSE, TRUE))
            {}
          else if (StringMatch(recordField, _T("abstract"), FALSE, TRUE))
            {}
          else if (StringMatch(recordField, _T("edition"), FALSE, TRUE))
            {}
          else if (StringMatch(recordField, _T("howpublished"), FALSE, TRUE))
            {}
          else if (StringMatch(recordField, _T("note"), FALSE, TRUE) || StringMatch(recordField, _T("notes"), FALSE, TRUE))
            {}
          else if (StringMatch(recordField, _T("series"), FALSE, TRUE))
            {}
          else if (StringMatch(recordField, _T("type"), FALSE, TRUE))
            {}
          else if (StringMatch(recordField, _T("keywords"), FALSE, TRUE))
            {}
          else if (StringMatch(recordField, _T("editor"), FALSE, TRUE) || StringMatch(recordField, _T("editors"), FALSE, TRUE))
            bibEntry->editor= copystring(fieldValue);
          else if (StringMatch(recordField, _T("title"), FALSE, TRUE))
            bibEntry->title= copystring(fieldValue);
          else if (StringMatch(recordField, _T("booktitle"), FALSE, TRUE))
            bibEntry->booktitle= copystring(fieldValue);
          else if (StringMatch(recordField, _T("journal"), FALSE, TRUE))
            bibEntry->journal= copystring(fieldValue);
          else if (StringMatch(recordField, _T("volume"), FALSE, TRUE))
            bibEntry->volume= copystring(fieldValue);
          else if (StringMatch(recordField, _T("number"), FALSE, TRUE))
            bibEntry->number= copystring(fieldValue);
          else if (StringMatch(recordField, _T("year"), FALSE, TRUE))
            bibEntry->year= copystring(fieldValue);
          else if (StringMatch(recordField, _T("month"), FALSE, TRUE))
            bibEntry->month= copystring(fieldValue);
          else if (StringMatch(recordField, _T("pages"), FALSE, TRUE))
            bibEntry->pages= copystring(fieldValue);
          else if (StringMatch(recordField, _T("publisher"), FALSE, TRUE))
            bibEntry->publisher= copystring(fieldValue);
          else if (StringMatch(recordField, _T("address"), FALSE, TRUE))
            bibEntry->address= copystring(fieldValue);
          else if (StringMatch(recordField, _T("institution"), FALSE, TRUE) || StringMatch(recordField, _T("school"), FALSE, TRUE))
            bibEntry->institution= copystring(fieldValue);
          else if (StringMatch(recordField, _T("organization"), FALSE, TRUE) || StringMatch(recordField, _T("organisation"), FALSE, TRUE))
            bibEntry->organization= copystring(fieldValue);
          else if (StringMatch(recordField, _T("comment"), FALSE, TRUE) || StringMatch(recordField, _T("comments"), FALSE, TRUE))
            bibEntry->comment= copystring(fieldValue);
          else if (StringMatch(recordField, _T("annote"), FALSE, TRUE))
            bibEntry->comment= copystring(fieldValue);
          else if (StringMatch(recordField, _T("chapter"), FALSE, TRUE))
            bibEntry->chapter= copystring(fieldValue);
          else
          {
            wxSprintf(buf, _T("Unrecognised bib field type %s at line %ld (%s)"), recordField, BibLine, filename);
            OnError(buf);
          }
        }
      }
      BibList.Append(recordKey, bibEntry);
      BibEatWhiteSpace(istr);
    }
  }
  return TRUE;
}

void OutputBibItem(TexRef *ref, BibEntry *bib)
{
  Tex2RTFYield();

  OnMacro(ltNUMBEREDBIBITEM, 2, TRUE);
  OnArgument(ltNUMBEREDBIBITEM, 1, TRUE);
  TexOutput(ref->sectionNumber);
  OnArgument(ltNUMBEREDBIBITEM, 1, FALSE);
  OnArgument(ltNUMBEREDBIBITEM, 2, TRUE);

  TexOutput(_T(" "));
  OnMacro(ltBF, 1, TRUE);
  OnArgument(ltBF, 1, TRUE);
  if (bib->author)
    TexOutput(bib->author);
  OnArgument(ltBF, 1, FALSE);
  OnMacro(ltBF, 1, FALSE);
  if (bib->author && (wxStrlen(bib->author) > 0) && (bib->author[wxStrlen(bib->author) - 1] != '.'))
    TexOutput(_T(". "));
  else
    TexOutput(_T(" "));

  if (bib->year)
  {
    TexOutput(bib->year);
  }
  if (bib->month)
  {
    TexOutput(_T(" ("));
    TexOutput(bib->month);
    TexOutput(_T(")"));
  }
  if (bib->year || bib->month)
    TexOutput(_T(". "));

  if (StringMatch(bib->type, _T("article"), FALSE, TRUE))
  {
    if (bib->title)
    {
      TexOutput(bib->title);
      TexOutput(_T(". "));
    }
    if (bib->journal)
    {
      OnMacro(ltIT, 1, TRUE);
      OnArgument(ltIT, 1, TRUE);
      TexOutput(bib->journal);
      OnArgument(ltIT, 1, FALSE);
      OnMacro(ltIT, 1, FALSE);
    }
    if (bib->volume)
    {
      TexOutput(_T(", "));
      OnMacro(ltBF, 1, TRUE);
      OnArgument(ltBF, 1, TRUE);
      TexOutput(bib->volume);
      OnArgument(ltBF, 1, FALSE);
      OnMacro(ltBF, 1, FALSE);
    }
    if (bib->number)
    {
      TexOutput(_T("("));
      TexOutput(bib->number);
      TexOutput(_T(")"));
    }
    if (bib->pages)
    {
      TexOutput(_T(", pages "));
      TexOutput(bib->pages);
    }
    TexOutput(_T("."));
  }
  else if (StringMatch(bib->type, _T("book"), FALSE, TRUE) ||
           StringMatch(bib->type, _T("unpublished"), FALSE, TRUE) ||
           StringMatch(bib->type, _T("manual"), FALSE, TRUE) ||
           StringMatch(bib->type, _T("phdthesis"), FALSE, TRUE) ||
           StringMatch(bib->type, _T("mastersthesis"), FALSE, TRUE) ||
           StringMatch(bib->type, _T("misc"), FALSE, TRUE) ||
           StringMatch(bib->type, _T("techreport"), FALSE, TRUE) ||
           StringMatch(bib->type, _T("booklet"), FALSE, TRUE))
  {
    if (bib->title || bib->booktitle)
    {
      OnMacro(ltIT, 1, TRUE);
      OnArgument(ltIT, 1, TRUE);
      TexOutput(bib->title ? bib->title : bib->booktitle);
      TexOutput(_T(". "));
      OnArgument(ltIT, 1, FALSE);
      OnMacro(ltIT, 1, FALSE);
    }
    if (StringMatch(bib->type, _T("phdthesis"), FALSE, TRUE))
      TexOutput(_T("PhD thesis. "));
    if (StringMatch(bib->type, _T("techreport"), FALSE, TRUE))
      TexOutput(_T("Technical report. "));
    if (bib->editor)
    {
      TexOutput(_T("Ed. "));
      TexOutput(bib->editor);
      TexOutput(_T(". "));
    }
    if (bib->institution)
    {
      TexOutput(bib->institution);
      TexOutput(_T(". "));
    }
    if (bib->organization)
    {
      TexOutput(bib->organization);
      TexOutput(_T(". "));
    }
    if (bib->publisher)
    {
      TexOutput(bib->publisher);
      TexOutput(_T(". "));
    }
    if (bib->address)
    {
      TexOutput(bib->address);
      TexOutput(_T(". "));
    }
  }
  else if (StringMatch(bib->type, _T("inbook"), FALSE, TRUE) ||
           StringMatch(bib->type, _T("inproceedings"), FALSE, TRUE) ||
           StringMatch(bib->type, _T("incollection"), FALSE, TRUE) ||
           StringMatch(bib->type, _T("conference"), FALSE, TRUE))
  {
    if (bib->title)
    {
      TexOutput(bib->title);
    }
    if (bib->booktitle)
    {
      TexOutput(_T(", from "));
      OnMacro(ltIT, 1, TRUE);
      OnArgument(ltIT, 1, TRUE);
      TexOutput(bib->booktitle);
      TexOutput(_T("."));
      OnArgument(ltIT, 1, FALSE);
      OnMacro(ltIT, 1, FALSE);
    }
    if (bib->editor)
    {
      TexOutput(_T(", ed. "));
      TexOutput(bib->editor);
    }
    if (bib->publisher)
    {
      TexOutput(_T(" "));
      TexOutput(bib->publisher);
    }
    if (bib->address)
    {
      if (bib->publisher) TexOutput(_T(", "));
      else TexOutput(_T(" "));
      TexOutput(bib->address);
    }
    if (bib->publisher || bib->address)
      TexOutput(_T("."));

    if (bib->volume)
    {
      TexOutput(_T(" "));
      OnMacro(ltBF, 1, TRUE);
      OnArgument(ltBF, 1, TRUE);
      TexOutput(bib->volume);
      OnArgument(ltBF, 1, FALSE);
      OnMacro(ltBF, 1, FALSE);
    }
    if (bib->number)
    {
      if (bib->volume)
      {
        TexOutput(_T("("));
        TexOutput(bib->number);
        TexOutput(_T(")."));
      }
      else
      {
        TexOutput(_T(" Number "));
        TexOutput(bib->number);
        TexOutput(_T("."));
      }
    }
    if (bib->chapter)
    {
      TexOutput(_T(" Chap. "));
      TexOutput(bib->chapter);
    }
    if (bib->pages)
    {
      if (bib->chapter) TexOutput(_T(", pages "));
      else TexOutput(_T(" Pages "));
      TexOutput(bib->pages);
      TexOutput(_T("."));
    }
  }
  OnArgument(ltNUMBEREDBIBITEM, 2, FALSE);
  OnMacro(ltNUMBEREDBIBITEM, 2, FALSE);
}

void OutputBib(void)
{
  // Write the heading
  ForceTopicName(_T("bibliography"));
  FakeCurrentSection(ReferencesNameString);
  ForceTopicName(NULL);

  OnMacro(ltPAR, 0, TRUE);
  OnMacro(ltPAR, 0, FALSE);

  if ((convertMode == TEX_RTF) && !winHelp)
  {
    OnMacro(ltPAR, 0, TRUE);
    OnMacro(ltPAR, 0, FALSE);
  }

  wxStringListNode *node = CitationList.GetFirst();
  while (node)
  {
    wxChar *citeKey = (wxChar *)node->GetData();
//    wxNode *texNode = TexReferences.Find(citeKey);
    TexRef *ref = (TexRef *)TexReferences.Get(citeKey);
    wxNode *bibNode = BibList.Find(citeKey);
    if (bibNode && ref)
    {
      BibEntry *entry = (BibEntry *)bibNode->GetData();
      OutputBibItem(ref, entry);
    }
    node = node->GetNext();
  }
}

static int citeCount = 1;

void ResolveBibReferences(void)
{
  if (CitationList.GetCount() > 0)
    OnInform(_T("Resolving bibliographic references..."));

  citeCount = 1;
  wxChar buf[200];
  wxStringListNode *node = CitationList.GetFirst();
  while (node)
  {
    Tex2RTFYield();
    wxChar *citeKey = (wxChar *)node->GetData();
//    wxNode *texNode = TexReferences.Find(citeKey);
    TexRef *ref = (TexRef *)TexReferences.Get(citeKey);
    wxNode *bibNode = BibList.Find(citeKey);
    if (bibNode && ref)
    {
      // Unused Variable
      //BibEntry *entry = (BibEntry *)bibNode->GetData();
      if (ref->sectionNumber) delete[] ref->sectionNumber;
      wxSprintf(buf, _T("[%d]"), citeCount);
      ref->sectionNumber = copystring(buf);
      citeCount ++;
    }
    else
    {
      wxSprintf(buf, _T("Warning: bib ref %s not resolved."), citeKey);
      OnInform(buf);
    }
    node = node->GetNext();
  }
}

// Remember we need to resolve this citation
void AddCitation(wxChar *citeKey)
{
  if (!CitationList.Member(citeKey))
    CitationList.Add(citeKey);

  if (!TexReferences.Get(citeKey))
  {
    TexReferences.Put(citeKey, new TexRef(citeKey, _T("??"), NULL));
  }
}

TexRef *FindReference(wxChar *key)
{
  return (TexRef *)TexReferences.Get(key);
}

/*
 * Custom macro stuff
 *
 */

bool StringTobool(wxChar *val)
{
  if (wxStrncmp(val, _T("yes"), 3) == 0 || wxStrncmp(val, _T("YES"), 3) == 0 ||
      wxStrncmp(val, _T("on"), 2) == 0 || wxStrncmp(val, _T("ON"), 2) == 0 ||
      wxStrncmp(val, _T("true"), 4) == 0 || wxStrncmp(val, _T("TRUE"), 4) == 0 ||
      wxStrncmp(val, _T("ok"), 2) == 0 || wxStrncmp(val, _T("OK"), 2) == 0 ||
      wxStrncmp(val, _T("1"), 1) == 0)
    return TRUE;
  else
    return FALSE;
}

// Define a variable value from the .ini file
wxChar *RegisterSetting(wxChar *settingName, wxChar *settingValue, bool interactive)
{
  static wxChar errorCode[100];
  wxStrcpy(errorCode, _T("OK"));
  if (StringMatch(settingName, _T("chapterName"), FALSE, TRUE))
  {
    delete[] ChapterNameString;
    ChapterNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("sectionName"), FALSE, TRUE))
  {
    delete[] SectionNameString;
    SectionNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("subsectionName"), FALSE, TRUE))
  {
    delete[] SubsectionNameString;
    SubsectionNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("subsubsectionName"), FALSE, TRUE))
  {
    delete[] SubsubsectionNameString;
    SubsubsectionNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("indexName"), FALSE, TRUE))
  {
    delete[] IndexNameString;
    IndexNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("contentsName"), FALSE, TRUE))
  {
    delete[] ContentsNameString;
    ContentsNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("glossaryName"), FALSE, TRUE))
  {
    delete[] GlossaryNameString;
    GlossaryNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("referencesName"), FALSE, TRUE))
  {
    delete[] ReferencesNameString;
    ReferencesNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("tablesName"), FALSE, TRUE))
  {
    delete[] TablesNameString;
    TablesNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("figuresName"), FALSE, TRUE))
  {
    delete[] FiguresNameString;
    FiguresNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("tableName"), FALSE, TRUE))
  {
    delete[] TableNameString;
    TableNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("figureName"), FALSE, TRUE))
  {
    delete[] FigureNameString;
    FigureNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("abstractName"), FALSE, TRUE))
  {
    delete[] AbstractNameString;
    AbstractNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("chapterFontSize"), FALSE, TRUE))
    StringToInt(settingValue, &chapterFont);
  else if (StringMatch(settingName, _T("sectionFontSize"), FALSE, TRUE))
    StringToInt(settingValue, &sectionFont);
  else if (StringMatch(settingName, _T("subsectionFontSize"), FALSE, TRUE))
    StringToInt(settingValue, &subsectionFont);
  else if (StringMatch(settingName, _T("titleFontSize"), FALSE, TRUE))
    StringToInt(settingValue, &titleFont);
  else if (StringMatch(settingName, _T("authorFontSize"), FALSE, TRUE))
    StringToInt(settingValue, &authorFont);
  else if (StringMatch(settingName, _T("ignoreInput"), FALSE, TRUE))
    IgnorableInputFiles.Add(wxFileNameFromPath(settingValue));
  else if (StringMatch(settingName, _T("mirrorMargins"), FALSE, TRUE))
    mirrorMargins = StringTobool(settingValue);
  else if (StringMatch(settingName, _T("runTwice"), FALSE, TRUE))
    runTwice = StringTobool(settingValue);
  else if (StringMatch(settingName, _T("isInteractive"), FALSE, TRUE))
    isInteractive = StringTobool(settingValue);
  else if (StringMatch(settingName, _T("headerRule"), FALSE, TRUE))
    headerRule = StringTobool(settingValue);
  else if (StringMatch(settingName, _T("footerRule"), FALSE, TRUE))
    footerRule = StringTobool(settingValue);
  else if (StringMatch(settingName, _T("combineSubSections"), FALSE, TRUE))
    combineSubSections = StringTobool(settingValue);
  else if (StringMatch(settingName, _T("listLabelIndent"), FALSE, TRUE))
    StringToInt(settingValue, &labelIndentTab);
  else if (StringMatch(settingName, _T("listItemIndent"), FALSE, TRUE))
    StringToInt(settingValue, &itemIndentTab);
  else if (StringMatch(settingName, _T("useUpButton"), FALSE, TRUE))
    useUpButton = StringTobool(settingValue);
  else if (StringMatch(settingName, _T("useHeadingStyles"), FALSE, TRUE))
    useHeadingStyles = StringTobool(settingValue);
  else if (StringMatch(settingName, _T("useWord"), FALSE, TRUE))
    useWord = StringTobool(settingValue);
  else if (StringMatch(settingName, _T("contentsDepth"), FALSE, TRUE))
    StringToInt(settingValue, &contentsDepth);
  else if (StringMatch(settingName, _T("generateHPJ"), FALSE, TRUE))
    generateHPJ = StringTobool(settingValue);
  else if (StringMatch(settingName, _T("truncateFilenames"), FALSE, TRUE))
    truncateFilenames = StringTobool(settingValue);
  else if (StringMatch(settingName, _T("winHelpVersion"), FALSE, TRUE))
    StringToInt(settingValue, &winHelpVersion);
  else if (StringMatch(settingName, _T("winHelpContents"), FALSE, TRUE))
    winHelpContents = StringTobool(settingValue);
  else if (StringMatch(settingName, _T("htmlIndex"), FALSE, TRUE))
    htmlIndex = StringTobool(settingValue);
  else if (StringMatch(settingName, _T("htmlWorkshopFiles"), FALSE, TRUE))
    htmlWorkshopFiles = StringTobool(settingValue);
  else if (StringMatch(settingName, _T("htmlFrameContents"), FALSE, TRUE))
    htmlFrameContents = StringTobool(settingValue);
  else if (StringMatch(settingName, _T("htmlStylesheet"), FALSE, TRUE))
    {
      if (htmlStylesheet) delete[] htmlStylesheet;
      htmlStylesheet = copystring(settingValue);
    }
  else if (StringMatch(settingName, _T("upperCaseNames"), FALSE, TRUE))
    upperCaseNames = StringTobool(settingValue);
  else if (StringMatch(settingName, _T("ignoreBadRefs"), FALSE, TRUE))
    ignoreBadRefs = StringTobool(settingValue);
  else if (StringMatch(settingName, _T("htmlFaceName"), FALSE, TRUE))
  {
    delete[] htmlFaceName;
    htmlFaceName = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("winHelpTitle"), FALSE, TRUE))
  {
    if (winHelpTitle)
      delete[] winHelpTitle;
    winHelpTitle = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("indexSubsections"), FALSE, TRUE))
    indexSubsections = StringTobool(settingValue);
  else if (StringMatch(settingName, _T("compatibility"), FALSE, TRUE))
    compatibilityMode = StringTobool(settingValue);
  else if (StringMatch(settingName, _T("defaultColumnWidth"), FALSE, TRUE))
  {
    StringToInt(settingValue, &defaultTableColumnWidth);
    defaultTableColumnWidth = 20*defaultTableColumnWidth;
  }
  else if (StringMatch(settingName, _T("bitmapMethod"), FALSE, TRUE))
  {
    if ((wxStrcmp(settingValue, _T("includepicture")) != 0) && (wxStrcmp(settingValue, _T("hex")) != 0) &&
        (wxStrcmp(settingValue, _T("import")) != 0))
    {
      if (interactive)
        OnError(_T("Unknown bitmapMethod"));
      wxStrcpy(errorCode, _T("Unknown bitmapMethod"));
    }
    else
    {
      delete[] bitmapMethod;
      bitmapMethod = copystring(settingValue);
    }
  }
  else if (StringMatch(settingName, _T("htmlBrowseButtons"), FALSE, TRUE))
  {
    if (wxStrcmp(settingValue, _T("none")) == 0)
      htmlBrowseButtons = HTML_BUTTONS_NONE;
    else if (wxStrcmp(settingValue, _T("bitmap")) == 0)
      htmlBrowseButtons = HTML_BUTTONS_BITMAP;
    else if (wxStrcmp(settingValue, _T("text")) == 0)
      htmlBrowseButtons = HTML_BUTTONS_TEXT;
    else
    {
      if (interactive)
        OnInform(_T("Initialisation file error: htmlBrowseButtons must be one of none, bitmap, or text."));
      wxStrcpy(errorCode, _T("Initialisation file error: htmlBrowseButtons must be one of none, bitmap, or text."));
    }
  }
  else if (StringMatch(settingName, _T("backgroundImage"), FALSE, TRUE))
  {
    backgroundImageString = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("backgroundColour"), FALSE, TRUE))
  {
    delete[] backgroundColourString;
    backgroundColourString = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("textColour"), FALSE, TRUE))
  {
    textColourString = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("linkColour"), FALSE, TRUE))
  {
    linkColourString = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("followedLinkColour"), FALSE, TRUE))
  {
    followedLinkColourString = copystring(settingValue);
  }
  else if (StringMatch(settingName, _T("conversionMode"), FALSE, TRUE))
  {
    if (StringMatch(settingValue, _T("RTF"), FALSE, TRUE))
    {
      winHelp = FALSE; convertMode = TEX_RTF;
    }
    else if (StringMatch(settingValue, _T("WinHelp"), FALSE, TRUE))
    {
      winHelp = TRUE; convertMode = TEX_RTF;
    }
    else if (StringMatch(settingValue, _T("XLP"), FALSE, TRUE) ||
             StringMatch(settingValue, _T("wxHelp"), FALSE, TRUE))
    {
      convertMode = TEX_XLP;
    }
    else if (StringMatch(settingValue, _T("HTML"), FALSE, TRUE))
    {
      convertMode = TEX_HTML;
    }
    else
    {
      if (interactive)
        OnInform(_T("Initialisation file error: conversionMode must be one of\nRTF, WinHelp, XLP (or wxHelp), HTML."));
      wxStrcpy(errorCode, _T("Initialisation file error: conversionMode must be one of\nRTF, WinHelp, XLP (or wxHelp), HTML."));
    }
  }
  else if (StringMatch(settingName, _T("documentFontSize"), FALSE, TRUE))
  {
    int n;
    StringToInt(settingValue, &n);
    if (n == 10 || n == 11 || n == 12)
      SetFontSizes(n);
    else
    {
      wxChar buf[200];
      wxSprintf(buf, _T("Initialisation file error: nonstandard document font size %d."), n);
      if (interactive)
        OnInform(buf);
      wxStrcpy(errorCode, buf);
    }
  }
  else
  {
    wxChar buf[200];
    wxSprintf(buf, _T("Initialisation file error: unrecognised setting %s."), settingName);
    if (interactive)
      OnInform(buf);
    wxStrcpy(errorCode, buf);
  }
  return errorCode;
}

bool ReadCustomMacros(wxChar *filename)
{
  if (!wxFileExists(filename))
      return FALSE;

  wxSTD ifstream istr(filename, wxSTD ios::in);

  if (istr.bad()) return FALSE;

  CustomMacroList.Clear();
  char ch;
  wxChar macroName[100];
  wxChar macroBody[1000];
  int noArgs;

  while (!istr.eof())
  {
    BibEatWhiteSpace(istr);
    istr.get(ch);
    if (istr.eof())
      break;
      
    if (ch != '\\') // Not a macro definition, so must be NAME=VALUE
    {
      wxChar settingName[100];
      settingName[0] = ch;
      BibReadWord(istr, (settingName+1));
      BibEatWhiteSpace(istr);
      istr.get(ch);
      if (ch != '=')
      {
        OnError(_T("Expected = following name: malformed tex2rtf.ini file."));
        return FALSE;
      }
      else
      {
        wxChar settingValue[200];
        BibEatWhiteSpace(istr);
        BibReadToEOL(istr, settingValue);
        RegisterSetting(settingName, settingValue);
      }
    }
    else
    {
      BibReadWord(istr, macroName);
      BibEatWhiteSpace(istr);
      istr.get(ch);
      if (ch != '[')
      {
        OnError(_T("Expected [ followed by number of arguments: malformed tex2rtf.ini file."));
        return FALSE;
      }
      istr >> noArgs;
      istr.get(ch);
      if (ch != ']')
      {
        OnError(_T("Expected ] following number of arguments: malformed tex2rtf.ini file."));
        return FALSE;
      }
      BibEatWhiteSpace(istr);
      istr.get(ch);
      if (ch != '{')
      {
        OnError(_T("Expected { followed by macro body: malformed tex2rtf.ini file."));
        return FALSE;
      }
      CustomMacro *macro = new CustomMacro(macroName, noArgs, NULL);
      BibReadValue(istr, macroBody, FALSE, FALSE); // Don't ignore extra braces
      if (wxStrlen(macroBody) > 0)
        macro->macroBody = copystring(macroBody);
    
      BibEatWhiteSpace(istr);
      CustomMacroList.Append(macroName, macro);
      AddMacroDef(ltCUSTOM_MACRO, macroName, noArgs);
    }
  }
  wxChar mbuf[200];
  wxSprintf(mbuf, _T("Read initialization file %s."), filename);
  OnInform(mbuf);
  return TRUE;
}
 
CustomMacro *FindCustomMacro(wxChar *name)
{
  wxNode *node = CustomMacroList.Find(name);
  if (node)
  {
    CustomMacro *macro = (CustomMacro *)node->GetData();
    return macro;
  }
  return NULL;
}

// Display custom macros
void ShowCustomMacros(void)
{
  wxNode *node = CustomMacroList.GetFirst();
  if (!node)
  {
    OnInform(_T("No custom macros loaded.\n"));
    return;
  }
  
  wxChar buf[400];
  while (node)
  {
    CustomMacro *macro = (CustomMacro *)node->GetData();
    wxSprintf(buf, _T("\\%s[%d]\n    {%s}"), macro->macroName, macro->noArgs,
     macro->macroBody ? macro->macroBody : _T(""));
    OnInform(buf);
    node = node->GetNext();
  }
}

// Parse a string into several comma-separated fields
wxChar *ParseMultifieldString(wxChar *allFields, int *pos)
{
  static wxChar buffer[300];
  int i = 0;
  int fieldIndex = *pos;
  int len = wxStrlen(allFields);
  int oldPos = *pos;
  bool keepGoing = TRUE;
  while ((fieldIndex <= len) && keepGoing)
  {
    if (allFields[fieldIndex] == _T(' '))
    {
      // Skip
      fieldIndex ++;
    }
    else if (allFields[fieldIndex] == _T(','))
    {
      *pos = fieldIndex + 1;
      keepGoing = FALSE;
    }
    else if (allFields[fieldIndex] == 0)
    {
      *pos = fieldIndex + 1;
      keepGoing = FALSE;
    }
    else
    {
      buffer[i] = allFields[fieldIndex];
      fieldIndex ++;
      i++;
    }
  }
  buffer[i] = 0;
  if (oldPos == (*pos))
    *pos = len + 1;
    
  if (i == 0)
    return NULL;
  else
    return buffer;
}

/*
 * Colour tables
 *
 */
 
ColourTableEntry::ColourTableEntry(const wxChar *theName, unsigned int r,  unsigned int g,  unsigned int b)
{
  name = copystring(theName);
  red = r;
  green = g;
  blue = b;
}

ColourTableEntry::~ColourTableEntry(void)
{
  delete[] name;
}

void AddColour(const wxChar *theName, unsigned int r,  unsigned int g,  unsigned int b)
{
  wxNode *node = ColourTable.Find(theName);
  if (node)
  {
    ColourTableEntry *entry = (ColourTableEntry *)node->GetData();
    if (entry->red == r || entry->green == g || entry->blue == b)
      return;
    else
    {
      delete entry;
      delete node;
    }
  }
  ColourTableEntry *entry = new ColourTableEntry(theName, r, g, b);
  ColourTable.Append(theName, entry);
}

int FindColourPosition(wxChar *theName)
{
  int i = 0;
  wxNode *node = ColourTable.GetFirst();
  while (node)
  {
    ColourTableEntry *entry = (ColourTableEntry *)node->GetData();
    if (wxStrcmp(theName, entry->name) == 0)
      return i;
    i ++;
    node = node->GetNext();
  }
  return -1;
}

// Converts e.g. "red" -> "#FF0000"
extern void DecToHex(int, wxChar *);
bool FindColourHTMLString(wxChar *theName, wxChar *buf)
{
  wxNode *node = ColourTable.GetFirst();
  while (node)
  {
    ColourTableEntry *entry = (ColourTableEntry *)node->GetData();
    if (wxStrcmp(theName, entry->name) == 0)
    {
        wxStrcpy(buf, _T("#"));
        
        wxChar buf2[3];
        DecToHex(entry->red, buf2);
        wxStrcat(buf, buf2);
        DecToHex(entry->green, buf2);
        wxStrcat(buf, buf2);
        DecToHex(entry->blue, buf2);
        wxStrcat(buf, buf2);

        return TRUE;
    }
    node = node->GetNext();
  }
  return FALSE;
}

  
void InitialiseColourTable(void)
{
  // \\red0\\green0\\blue0;
  AddColour(_T("black"), 0,0,0);

  // \\red0\\green0\\blue255;\\red0\\green255\\blue255;\n");
  AddColour(_T("cyan"), 0,255,255);

  // \\red0\\green255\\blue0;
  AddColour(_T("green"), 0,255,0);
  
  // \\red255\\green0\\blue255;
  AddColour(_T("magenta"), 255,0,255);

  // \\red255\\green0\\blue0;
  AddColour(_T("red"), 255,0,0);
  
  // \\red255\\green255\\blue0;
  AddColour(_T("yellow"), 255,255,0);
  
  // \\red255\\green255\\blue255;}");
  AddColour(_T("white"), 255,255,255);
}

/*
 * The purpose of this is to reduce the number of times wxYield is
 * called, since under Windows this can slow things down.
 */

void Tex2RTFYield(bool force)
{
#ifdef __WXMSW__
    static int yieldCount = 0;
    
    if (isSync)
	return;
    
    if (force)
	yieldCount = 0;
    if (yieldCount == 0)
    {
	if (wxTheApp)
	    wxYield();
	yieldCount = 10;
    }
    yieldCount --;
#endif
}

// In both RTF generation and HTML generation for wxHelp version 2,
// we need to associate \indexed keywords with the current filename/topics.

// Hash table for lists of keywords for topics (WinHelp).
wxHashTable TopicTable(wxKEY_STRING);
void AddKeyWordForTopic(wxChar *topic, wxChar *entry, wxChar *filename)
{
  TexTopic *texTopic = (TexTopic *)TopicTable.Get(topic);
  if (!texTopic)
  {
    texTopic = new TexTopic(filename);
    texTopic->keywords = new wxStringList;
    TopicTable.Put(topic, texTopic);
  }
  
  if (!texTopic->keywords->Member(entry))
    texTopic->keywords->Add(entry);
}

void ClearKeyWordTable(void)
{
  TopicTable.BeginFind();
  wxNode *node = TopicTable.Next();
  while (node)
  {
    TexTopic *texTopic = (TexTopic *)node->GetData();
    delete texTopic;
    node = TopicTable.Next();
  }
  TopicTable.Clear();
}


/*
 * TexTopic structure
 */
 
TexTopic::TexTopic(wxChar *f)
{
  if (f)
    filename = copystring(f);
  else
    filename = NULL;
  hasChildren = FALSE;
  keywords = NULL;
}

TexTopic::~TexTopic(void)
{
  if (keywords)
    delete keywords;
  if (filename)
    delete[] filename;
}

// Convert case, according to upperCaseNames setting.
wxChar *ConvertCase(wxChar *s)
{
  static wxChar buf[256];
  int len = wxStrlen(s);
  int i;
  if (upperCaseNames)
    for (i = 0; i < len; i ++)
      buf[i] = wxToupper(s[i]);
  else
    for (i = 0; i < len; i ++)
      buf[i] = wxTolower(s[i]);
  buf[i] = 0;
  return buf;  
}

#if !WXWIN_COMPATIBILITY_2
// if substring is TRUE, search for str1 in str2
bool StringMatch(const wxChar *str1, const wxChar *str2, bool subString, 
                 bool exact)
{
   if (subString)
   {
      wxString Sstr1(str1);
      wxString Sstr2(str2);
      if (!exact)
      {
          Sstr1.MakeUpper();
          Sstr2.MakeUpper();
      }
      return Sstr2.Index(Sstr1) != (size_t)wxNOT_FOUND;
   }
   else
      return exact ? wxString(str2).Cmp(str1) == 0 : 
                     wxString(str2).CmpNoCase(str1) == 0;
}
#endif

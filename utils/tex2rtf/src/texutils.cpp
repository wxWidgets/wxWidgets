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
#include "wx/wx.h"
#endif

#include <wx/hash.h>

#if wxUSE_IOSTREAMH
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

#include <ctype.h>
#include "tex2any.h"

wxHashTable TexReferences(wxKEY_STRING);
wxList BibList(wxKEY_STRING);
wxStringList CitationList;
wxList ColourTable(wxKEY_STRING);
wxHashTable BibStringTable(wxKEY_STRING);
wxList CustomMacroList(wxKEY_STRING);
TexChunk *currentSection = NULL;
char *fakeCurrentSection = NULL;

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
void OutputCurrentSectionToString(char *buf)
{
    if (fakeCurrentSection)
        strcpy(buf, fakeCurrentSection);
    else
        OutputChunkToString(currentSection, buf);
}

void OutputChunkToString(TexChunk *chunk, char *buf)
{
  FILE *tempfd = fopen("tmp.tmp", "w");
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
  tempfd = fopen("tmp.tmp", "r");
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
  wxRemoveFile("tmp.tmp");
}

// Called by Tex2Any to simulate a section
void FakeCurrentSection(char *fakeSection, bool addToContents)
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

static char *forceTopicName = NULL;

void ForceTopicName(char *name)
{
  if (forceTopicName)
    delete[] forceTopicName;
  if (name)
    forceTopicName = copystring(name);
  else
    forceTopicName = NULL;
}

char *FindTopicName(TexChunk *chunk)
{
  if (forceTopicName)
    return forceTopicName;
    
  char *topicName = NULL;
  static char topicBuf[100];

  if (chunk && (chunk->type == CHUNK_TYPE_MACRO) &&
      (chunk->macroId == ltLABEL))
  {
    wxNode *node = chunk->children.First();
    if (node)
    {
      TexChunk *child = (TexChunk *)node->Data();
      if (child->type == CHUNK_TYPE_ARG)
      {
        wxNode *snode = child->children.First();
        if (snode)
        {
          TexChunk *schunk = (TexChunk *)snode->Data();
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
    sprintf(topicBuf, "topic%ld", topicCounter);
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
 
void StartSimulateArgument(char *data)
{
  strcpy(currentArgData, data);
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

int ParseUnitArgument(char *unitArg)
{
  float conversionFactor = 1.0;
  float unitValue = 0.0;
  int len = strlen(unitArg);
  // Get rid of any accidentally embedded commands
  for (int i = 0; i < len; i++)
    if (unitArg[i] == '\\')
      unitArg[i] = 0;
  len = strlen(unitArg);
      
  if (unitArg && (len > 0) && (isdigit(unitArg[0]) || unitArg[0] == '-'))
  {
    sscanf(unitArg, "%f", &unitValue);
    if (len > 1)
    {
      char units[3]; 
      units[0] = unitArg[len-2];
      units[1] = unitArg[len-1];
      units[2] = 0;
      if (strcmp(units, "in") == 0)
        conversionFactor = 72.0;
      else if (strcmp(units, "cm") == 0)
        conversionFactor = 72.0/2.51;
      else if (strcmp(units, "mm") == 0)
        conversionFactor = 72.0/25.1;
      else if (strcmp(units, "pt") == 0)
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
 
void StripExtension(char *buffer)
{
  int len = strlen(buffer);
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
 
void AddTexRef(char *name, char *file, char *sectionName,
               int chapter, int section, int subsection, int subsubsection)
{
  TexRef *texRef = (TexRef *)TexReferences.Get(name);
  if (texRef) TexReferences.Delete(name);
  
  char buf[100];
  buf[0] = 0;
/*
  if (sectionName)
  {
    strcat(buf, sectionName);
    strcat(buf, " ");
  }
*/
  if (chapter)
  {
    char buf2[10];
    sprintf(buf2, "%d", chapter);
    strcat(buf, buf2);
  }
  if (section)
  {
    char buf2[10];
    if (chapter)
      strcat(buf, ".");

    sprintf(buf2, "%d", section);
    strcat(buf, buf2);
  }
  if (subsection)
  {
    char buf2[10];
    strcat(buf, ".");
    sprintf(buf2, "%d", subsection);
    strcat(buf, buf2);
  }
  if (subsubsection)
  {
    char buf2[10];
    strcat(buf, ".");
    sprintf(buf2, "%d", subsubsection);
    strcat(buf, buf2);
  }
  char *tmp = ((strlen(buf) > 0) ? buf : (char *)NULL);
  TexReferences.Put(name, new TexRef(name, file, tmp, sectionName));
}

void WriteTexReferences(char *filename)
{
  ofstream ostr(filename);
  if (ostr.bad()) return;
  char buf[200];
  
  TexReferences.BeginFind();
  wxNode *node = TexReferences.Next();
  while (node)
  {
    Tex2RTFYield();
    TexRef *ref = (TexRef *)node->Data();
    ostr << ref->refLabel << " " << (ref->refFile ? ref->refFile : "??") << " ";
    ostr << (ref->sectionName ? ref->sectionName : "??") << " ";
    ostr << (ref->sectionNumber ? ref->sectionNumber : "??") << "\n";
    if (!ref->sectionNumber || (strcmp(ref->sectionNumber, "??") == 0 && strcmp(ref->sectionName, "??") == 0))
    {
      sprintf(buf, "Warning: reference %s not resolved.", ref->refLabel);
      OnInform(buf);
    }
    node = TexReferences.Next();
  }
}

void ReadTexReferences(char *filename)
{
  if (!wxFileExists(filename))
      return;

  ifstream istr(filename, ios::in);

  if (istr.bad()) return;

  char label[100];
  char file[400];
  char section[100];
  char sectionName[100];

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
      TexReferences.Put(label, new TexRef(label, file, section, sectionName));
    }
  }
}


/*
 * Bibliography-handling code
 *
 */

void BibEatWhiteSpace(istream& str)
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
void BibReadWord(istream& istr, char *buffer)
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
void BibReadToEOL(istream& istr, char *buffer)
{
  int i = 0;
  buffer[i] = 0;
  char ch = istr.peek();
  bool inQuotes = FALSE;
  if (ch == '"')
  {
    istr.get(ch);
    ch = istr.peek();
    inQuotes = TRUE;
  }
  // If in quotes, read white space too. If not,
  // stop at white space or comment.
  while (!istr.eof() && ch != 13 && ch != 10 && ch != '"' &&
         (inQuotes || ((ch != ' ') && (ch != 9) &&
                        (ch != ';') && (ch != '%') && (ch != '#'))))
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
void BibReadValue(istream& istr, char *buffer, bool ignoreBraces = TRUE,
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
    if (i >= 2000)
    {
      char buf[100];
      sprintf(buf, "Sorry, value > 2000 chars in bib file at line %ld, terminating.", BibLine);
      wxFatalError(buf, "Tex2RTF Fatal Error");
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
}
 
bool ReadBib(char *filename)
{
  if (!wxFileExists(filename))
      return FALSE;

  char buf[300];
  ifstream istr(filename, ios::in);
  if (istr.bad()) return FALSE;

  BibLine = 1;

  OnInform("Reading .bib file...");

  char ch;
  char fieldValue[2000];
  char recordType[100];
  char recordKey[100];
  char recordField[100];
  while (!istr.eof())
  {
    Tex2RTFYield();

    BibEatWhiteSpace(istr);
    istr.get(ch);
    if (ch != '@')
    {
      sprintf(buf, "Expected @: malformed bib file at line %ld (%s)", BibLine, filename);
      OnError(buf);
      return FALSE;
    }
    BibReadWord(istr, recordType);
    BibEatWhiteSpace(istr);
    istr.get(ch);
    if (ch != '{' && ch != '(')
    {
      sprintf(buf, "Expected { or ( after record type: malformed .bib file at line %ld (%s)", BibLine, filename);
      OnError(buf);
      return FALSE;
    }
    BibEatWhiteSpace(istr);
    if (StringMatch(recordType, "string", FALSE, TRUE))
    {
      BibReadWord(istr, recordType);
      BibEatWhiteSpace(istr);
      istr.get(ch);
      if (ch != '=')
      {
        sprintf(buf, "Expected = after string key: malformed .bib file at line %ld (%s)", BibLine, filename);
        OnError(buf);
        return FALSE;
      }
      BibEatWhiteSpace(istr);
      istr.get(ch);
      if (ch != '"' && ch != '{')
      {
        sprintf(buf, "Expected = after string key: malformed .bib file at line %ld (%s)", BibLine, filename);
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
            sprintf(buf, "Expected = after field type: malformed .bib file at line %ld (%s)", BibLine, filename);
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
            char *s = (char *)BibStringTable.Get(fieldValue);
            if (s)
            {
              strcpy(fieldValue, s);
            }
          }
          else
            BibReadValue(istr, fieldValue, TRUE, (ch == '"' ? TRUE : FALSE));

          // Now we can add a field
          if (StringMatch(recordField, "author", FALSE, TRUE))
            bibEntry->author = copystring(fieldValue);
          else if (StringMatch(recordField, "key", FALSE, TRUE))
            {}
          else if (StringMatch(recordField, "annotate", FALSE, TRUE))
            {}
          else if (StringMatch(recordField, "abstract", FALSE, TRUE))
            {}
          else if (StringMatch(recordField, "edition", FALSE, TRUE))
            {}
          else if (StringMatch(recordField, "howpublished", FALSE, TRUE))
            {}
          else if (StringMatch(recordField, "note", FALSE, TRUE) || StringMatch(recordField, "notes", FALSE, TRUE))
            {}
          else if (StringMatch(recordField, "series", FALSE, TRUE))
            {}
          else if (StringMatch(recordField, "type", FALSE, TRUE))
            {}
          else if (StringMatch(recordField, "keywords", FALSE, TRUE))
            {}
          else if (StringMatch(recordField, "editor", FALSE, TRUE) || StringMatch(recordField, "editors", FALSE, TRUE))
            bibEntry->editor= copystring(fieldValue);
          else if (StringMatch(recordField, "title", FALSE, TRUE))
            bibEntry->title= copystring(fieldValue);
          else if (StringMatch(recordField, "booktitle", FALSE, TRUE))
            bibEntry->booktitle= copystring(fieldValue);
          else if (StringMatch(recordField, "journal", FALSE, TRUE))
            bibEntry->journal= copystring(fieldValue);
          else if (StringMatch(recordField, "volume", FALSE, TRUE))
            bibEntry->volume= copystring(fieldValue);
          else if (StringMatch(recordField, "number", FALSE, TRUE))
            bibEntry->number= copystring(fieldValue);
          else if (StringMatch(recordField, "year", FALSE, TRUE))
            bibEntry->year= copystring(fieldValue);
          else if (StringMatch(recordField, "month", FALSE, TRUE))
            bibEntry->month= copystring(fieldValue);
          else if (StringMatch(recordField, "pages", FALSE, TRUE))
            bibEntry->pages= copystring(fieldValue);
          else if (StringMatch(recordField, "publisher", FALSE, TRUE))
            bibEntry->publisher= copystring(fieldValue);
          else if (StringMatch(recordField, "address", FALSE, TRUE))
            bibEntry->address= copystring(fieldValue);
          else if (StringMatch(recordField, "institution", FALSE, TRUE) || StringMatch(recordField, "school", FALSE, TRUE))
            bibEntry->institution= copystring(fieldValue);
          else if (StringMatch(recordField, "organization", FALSE, TRUE) || StringMatch(recordField, "organisation", FALSE, TRUE))
            bibEntry->organization= copystring(fieldValue);
          else if (StringMatch(recordField, "comment", FALSE, TRUE) || StringMatch(recordField, "comments", FALSE, TRUE))
            bibEntry->comment= copystring(fieldValue);
          else if (StringMatch(recordField, "annote", FALSE, TRUE))
            bibEntry->comment= copystring(fieldValue);
          else if (StringMatch(recordField, "chapter", FALSE, TRUE))
            bibEntry->chapter= copystring(fieldValue);
          else
          {
            sprintf(buf, "Unrecognised bib field type %s at line %ld (%s)", recordField, BibLine, filename);
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

  TexOutput(" ");
  OnMacro(ltBF, 1, TRUE);
  OnArgument(ltBF, 1, TRUE);
  if (bib->author)
    TexOutput(bib->author);
  OnArgument(ltBF, 1, FALSE);
  OnMacro(ltBF, 1, FALSE);
  if (bib->author && (strlen(bib->author) > 0) && (bib->author[strlen(bib->author) - 1] != '.'))
    TexOutput(". ");
  else
    TexOutput(" ");

  if (bib->year)
  {
    TexOutput(bib->year);
  }
  if (bib->month)
  {
    TexOutput(" (");
    TexOutput(bib->month);
    TexOutput(")");
  }
  if (bib->year || bib->month)
    TexOutput(". ");

  if (StringMatch(bib->type, "article", FALSE, TRUE))
  {
    if (bib->title)
    {
      TexOutput(bib->title);
      TexOutput(". ");
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
      TexOutput(", ");
      OnMacro(ltBF, 1, TRUE);
      OnArgument(ltBF, 1, TRUE);
      TexOutput(bib->volume);
      OnArgument(ltBF, 1, FALSE);
      OnMacro(ltBF, 1, FALSE);
    }
    if (bib->number)
    {
      TexOutput("(");
      TexOutput(bib->number);
      TexOutput(")");
    }
    if (bib->pages)
    {
      TexOutput(", pages ");
      TexOutput(bib->pages);
    }
    TexOutput(".");
  }
  else if (StringMatch(bib->type, "book", FALSE, TRUE) ||
           StringMatch(bib->type, "unpublished", FALSE, TRUE) ||
           StringMatch(bib->type, "manual", FALSE, TRUE) ||
           StringMatch(bib->type, "phdthesis", FALSE, TRUE) ||
           StringMatch(bib->type, "mastersthesis", FALSE, TRUE) ||
           StringMatch(bib->type, "misc", FALSE, TRUE) ||
           StringMatch(bib->type, "techreport", FALSE, TRUE) ||
           StringMatch(bib->type, "booklet", FALSE, TRUE))
  {
    if (bib->title || bib->booktitle)
    {
      OnMacro(ltIT, 1, TRUE);
      OnArgument(ltIT, 1, TRUE);
      TexOutput(bib->title ? bib->title : bib->booktitle);
      TexOutput(". ");
      OnArgument(ltIT, 1, FALSE);
      OnMacro(ltIT, 1, FALSE);
    }
    if (StringMatch(bib->type, "phdthesis", FALSE, TRUE))
      TexOutput("PhD thesis. ");
    if (StringMatch(bib->type, "techreport", FALSE, TRUE))
      TexOutput("Technical report. ");
    if (bib->editor)
    {
      TexOutput("Ed. ");
      TexOutput(bib->editor);
      TexOutput(". ");
    }
    if (bib->institution)
    {
      TexOutput(bib->institution);
      TexOutput(". ");
    }
    if (bib->organization)
    {
      TexOutput(bib->organization);
      TexOutput(". ");
    }
    if (bib->publisher)
    {
      TexOutput(bib->publisher);
      TexOutput(". ");
    }
    if (bib->address)
    {
      TexOutput(bib->address);
      TexOutput(". ");
    }
  }
  else if (StringMatch(bib->type, "inbook", FALSE, TRUE) ||
           StringMatch(bib->type, "inproceedings", FALSE, TRUE) ||
           StringMatch(bib->type, "incollection", FALSE, TRUE) ||
           StringMatch(bib->type, "conference", FALSE, TRUE))
  {
    if (bib->title)
    {
      TexOutput(bib->title);
    }
    if (bib->booktitle)
    {
      TexOutput(", from ");
      OnMacro(ltIT, 1, TRUE);
      OnArgument(ltIT, 1, TRUE);
      TexOutput(bib->booktitle);
      TexOutput(".");
      OnArgument(ltIT, 1, FALSE);
      OnMacro(ltIT, 1, FALSE);
    }
    if (bib->editor)
    {
      TexOutput(", ed. ");
      TexOutput(bib->editor);
    }
    if (bib->publisher)
    {
      TexOutput(" ");
      TexOutput(bib->publisher);
    }
    if (bib->address)
    {
      if (bib->publisher) TexOutput(", ");
      else TexOutput(" ");
      TexOutput(bib->address);
    }
    if (bib->publisher || bib->address)
      TexOutput(".");

    if (bib->volume)
    {
      TexOutput(" ");
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
        TexOutput("(");
        TexOutput(bib->number);
        TexOutput(").");
      }
      else
      {
        TexOutput(" Number ");
        TexOutput(bib->number);
        TexOutput(".");
      }
    }
    if (bib->chapter)
    {
      TexOutput(" Chap. "); TexOutput(bib->chapter);
    }
    if (bib->pages)
    {
      if (bib->chapter) TexOutput(", pages ");
      else TexOutput(" Pages ");
      TexOutput(bib->pages);
      TexOutput(".");
    }
  }
  OnArgument(ltNUMBEREDBIBITEM, 2, FALSE);
  OnMacro(ltNUMBEREDBIBITEM, 2, FALSE);
}

void OutputBib(void)
{
  // Write the heading
  ForceTopicName("bibliography");
  FakeCurrentSection(ReferencesNameString);
  ForceTopicName(NULL);

  OnMacro(ltPAR, 0, TRUE);
  OnMacro(ltPAR, 0, FALSE);

  if ((convertMode == TEX_RTF) && !winHelp)
  {
    OnMacro(ltPAR, 0, TRUE);
    OnMacro(ltPAR, 0, FALSE);
  }

  wxNode *node = CitationList.First();
  while (node)
  {
    char *citeKey = (char *)node->Data();
//    wxNode *texNode = TexReferences.Find(citeKey);
    TexRef *ref = (TexRef *)TexReferences.Get(citeKey);
    wxNode *bibNode = BibList.Find(citeKey);
    if (bibNode && ref)
    {
      BibEntry *entry = (BibEntry *)bibNode->Data();
      OutputBibItem(ref, entry);
    }
    node = node->Next();
  }
}

static int citeCount = 1;

void ResolveBibReferences(void)
{
  if (CitationList.Number() > 0)
    OnInform("Resolving bibliographic references...");

  citeCount = 1;
  char buf[200];
  wxNode *node = CitationList.First();
  while (node)
  {
    Tex2RTFYield();
    char *citeKey = (char *)node->Data();
//    wxNode *texNode = TexReferences.Find(citeKey);
    TexRef *ref = (TexRef *)TexReferences.Get(citeKey);
    wxNode *bibNode = BibList.Find(citeKey);
    if (bibNode && ref)
    {
      // Unused Variable
      //BibEntry *entry = (BibEntry *)bibNode->Data();
      if (ref->sectionNumber) delete[] ref->sectionNumber;
      sprintf(buf, "[%d]", citeCount);
      ref->sectionNumber = copystring(buf);
      citeCount ++;
    }
    else
    {
      sprintf(buf, "Warning: bib ref %s not resolved.", citeKey);
      OnInform(buf);
    }
    node = node->Next();
  }
}

// Remember we need to resolve this citation
void AddCitation(char *citeKey)
{
  if (!CitationList.Member(citeKey))
    CitationList.Add(citeKey);

  if (!TexReferences.Get(citeKey))
  {
    TexReferences.Put(citeKey, new TexRef(citeKey, "??", NULL));
  }
}

TexRef *FindReference(char *key)
{
  return (TexRef *)TexReferences.Get(key);
}

/*
 * Custom macro stuff
 *
 */

bool StringTobool(char *val)
{
  if (strncmp(val, "yes", 3) == 0 || strncmp(val, "YES", 3) == 0 ||
      strncmp(val, "on", 2) == 0 || strncmp(val, "ON", 2) == 0 ||
      strncmp(val, "true", 4) == 0 || strncmp(val, "TRUE", 4) == 0 ||
      strncmp(val, "ok", 2) == 0 || strncmp(val, "OK", 2) == 0 ||
      strncmp(val, "1", 1) == 0)
    return TRUE;
  else
    return FALSE;
}

// Define a variable value from the .ini file
char *RegisterSetting(char *settingName, char *settingValue, bool interactive)
{
  static char errorCode[100];
  strcpy(errorCode, "OK");
  if (StringMatch(settingName, "chapterName", FALSE, TRUE))
  {
    delete[] ChapterNameString;
    ChapterNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, "sectionName", FALSE, TRUE))
  {
    delete[] SectionNameString;
    SectionNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, "subsectionName", FALSE, TRUE))
  {
    delete[] SubsectionNameString;
    SubsectionNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, "subsubsectionName", FALSE, TRUE))
  {
    delete[] SubsubsectionNameString;
    SubsubsectionNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, "indexName", FALSE, TRUE))
  {
    delete[] IndexNameString;
    IndexNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, "contentsName", FALSE, TRUE))
  {
    delete[] ContentsNameString;
    ContentsNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, "glossaryName", FALSE, TRUE))
  {
    delete[] GlossaryNameString;
    GlossaryNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, "referencesName", FALSE, TRUE))
  {
    delete[] ReferencesNameString;
    ReferencesNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, "tablesName", FALSE, TRUE))
  {
    delete[] TablesNameString;
    TablesNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, "figuresName", FALSE, TRUE))
  {
    delete[] FiguresNameString;
    FiguresNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, "tableName", FALSE, TRUE))
  {
    delete[] TableNameString;
    TableNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, "figureName", FALSE, TRUE))
  {
    delete[] FigureNameString;
    FigureNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, "abstractName", FALSE, TRUE))
  {
    delete[] AbstractNameString;
    AbstractNameString = copystring(settingValue);
  }
  else if (StringMatch(settingName, "chapterFontSize", FALSE, TRUE))
    StringToInt(settingValue, &chapterFont);
  else if (StringMatch(settingName, "sectionFontSize", FALSE, TRUE))
    StringToInt(settingValue, &sectionFont);
  else if (StringMatch(settingName, "subsectionFontSize", FALSE, TRUE))
    StringToInt(settingValue, &subsectionFont);
  else if (StringMatch(settingName, "titleFontSize", FALSE, TRUE))
    StringToInt(settingValue, &titleFont);
  else if (StringMatch(settingName, "authorFontSize", FALSE, TRUE))
    StringToInt(settingValue, &authorFont);
  else if (StringMatch(settingName, "ignoreInput", FALSE, TRUE))
    IgnorableInputFiles.Add(FileNameFromPath(settingValue));
  else if (StringMatch(settingName, "mirrorMargins", FALSE, TRUE))
    mirrorMargins = StringTobool(settingValue);
  else if (StringMatch(settingName, "runTwice", FALSE, TRUE))
    runTwice = StringTobool(settingValue);
  else if (StringMatch(settingName, "isInteractive", FALSE, TRUE))
    isInteractive = StringTobool(settingValue);
  else if (StringMatch(settingName, "headerRule", FALSE, TRUE))
    headerRule = StringTobool(settingValue);
  else if (StringMatch(settingName, "footerRule", FALSE, TRUE))
    footerRule = StringTobool(settingValue);
  else if (StringMatch(settingName, "combineSubSections", FALSE, TRUE))
    combineSubSections = StringTobool(settingValue);
  else if (StringMatch(settingName, "listLabelIndent", FALSE, TRUE))
    StringToInt(settingValue, &labelIndentTab);
  else if (StringMatch(settingName, "listItemIndent", FALSE, TRUE))
    StringToInt(settingValue, &itemIndentTab);
  else if (StringMatch(settingName, "useUpButton", FALSE, TRUE))
    useUpButton = StringTobool(settingValue);
  else if (StringMatch(settingName, "useHeadingStyles", FALSE, TRUE))
    useHeadingStyles = StringTobool(settingValue);
  else if (StringMatch(settingName, "useWord", FALSE, TRUE))
    useWord = StringTobool(settingValue);
  else if (StringMatch(settingName, "contentsDepth", FALSE, TRUE))
    StringToInt(settingValue, &contentsDepth);
  else if (StringMatch(settingName, "generateHPJ", FALSE, TRUE))
    generateHPJ = StringTobool(settingValue);
  else if (StringMatch(settingName, "truncateFilenames", FALSE, TRUE))
    truncateFilenames = StringTobool(settingValue);
  else if (StringMatch(settingName, "winHelpVersion", FALSE, TRUE))
    StringToInt(settingValue, &winHelpVersion);
  else if (StringMatch(settingName, "winHelpContents", FALSE, TRUE))
    winHelpContents = StringTobool(settingValue);
  else if (StringMatch(settingName, "htmlIndex", FALSE, TRUE))
    htmlIndex = StringTobool(settingValue);
  else if (StringMatch(settingName, "htmlWorkshopFiles", FALSE, TRUE))
    htmlWorkshopFiles = StringTobool(settingValue);
  else if (StringMatch(settingName, "htmlFrameContents", FALSE, TRUE))
    htmlFrameContents = StringTobool(settingValue);
  else if (StringMatch(settingName, "upperCaseNames", FALSE, TRUE))
    upperCaseNames = StringTobool(settingValue);
  else if (StringMatch(settingName, "winHelpTitle", FALSE, TRUE))
  {
    if (winHelpTitle)
      delete[] winHelpTitle;
    winHelpTitle = copystring(settingValue);
  }
  else if (StringMatch(settingName, "indexSubsections", FALSE, TRUE))
    indexSubsections = StringTobool(settingValue);
  else if (StringMatch(settingName, "compatibility", FALSE, TRUE))
    compatibilityMode = StringTobool(settingValue);
  else if (StringMatch(settingName, "defaultColumnWidth", FALSE, TRUE))
  {
    StringToInt(settingValue, &defaultTableColumnWidth);
    defaultTableColumnWidth = 20*defaultTableColumnWidth;
  }
  else if (StringMatch(settingName, "bitmapMethod", FALSE, TRUE))
  {
    if ((strcmp(settingValue, "includepicture") != 0) && (strcmp(settingValue, "hex") != 0) &&
        (strcmp(settingValue, "import") != 0))
    {
      if (interactive)
        OnError("Unknown bitmapMethod");
      strcpy(errorCode, "Unknown bitmapMethod");
    }
    else
    {
      delete[] bitmapMethod;
      bitmapMethod = copystring(settingValue);
    }
  }
  else if (StringMatch(settingName, "htmlBrowseButtons", FALSE, TRUE))
  {
    if (strcmp(settingValue, "none") == 0)
      htmlBrowseButtons = HTML_BUTTONS_NONE;
    else if (strcmp(settingValue, "bitmap") == 0)
      htmlBrowseButtons = HTML_BUTTONS_BITMAP;
    else if (strcmp(settingValue, "text") == 0)
      htmlBrowseButtons = HTML_BUTTONS_TEXT;
    else
    {
      if (interactive)
        OnInform("Initialisation file error: htmlBrowseButtons must be one of none, bitmap, or text.");
      strcpy(errorCode, "Initialisation file error: htmlBrowseButtons must be one of none, bitmap, or text.");
    }
  }
  else if (StringMatch(settingName, "backgroundImage", FALSE, TRUE))
  {
    backgroundImageString = copystring(settingValue);
  }
  else if (StringMatch(settingName, "backgroundColour", FALSE, TRUE))
  {
    delete[] backgroundColourString;
    backgroundColourString = copystring(settingValue);
  }
  else if (StringMatch(settingName, "textColour", FALSE, TRUE))
  {
    textColourString = copystring(settingValue);
  }
  else if (StringMatch(settingName, "linkColour", FALSE, TRUE))
  {
    linkColourString = copystring(settingValue);
  }
  else if (StringMatch(settingName, "followedLinkColour", FALSE, TRUE))
  {
    followedLinkColourString = copystring(settingValue);
  }
  else if (StringMatch(settingName, "conversionMode", FALSE, TRUE))
  {
    if (StringMatch(settingValue, "RTF", FALSE, TRUE))
    {
      winHelp = FALSE; convertMode = TEX_RTF;
    }
    else if (StringMatch(settingValue, "WinHelp", FALSE, TRUE))
    {
      winHelp = TRUE; convertMode = TEX_RTF;
    }
    else if (StringMatch(settingValue, "XLP", FALSE, TRUE) ||
             StringMatch(settingValue, "wxHelp", FALSE, TRUE))
    {
      convertMode = TEX_XLP;
    }
    else if (StringMatch(settingValue, "HTML", FALSE, TRUE))
    {
      convertMode = TEX_HTML;
    }
    else
    {
      if (interactive)
        OnInform("Initialisation file error: conversionMode must be one of\nRTF, WinHelp, XLP (or wxHelp), HTML.");
      strcpy(errorCode, "Initialisation file error: conversionMode must be one of\nRTF, WinHelp, XLP (or wxHelp), HTML.");
    }
  }
  else if (StringMatch(settingName, "documentFontSize", FALSE, TRUE))
  {
    int n;
    StringToInt(settingValue, &n);
    if (n == 10 || n == 11 || n == 12)
      SetFontSizes(n);
    else
    {
      char buf[200];
      sprintf(buf, "Initialisation file error: nonstandard document font size %d.", n);
      if (interactive)
        OnInform(buf);
      strcpy(errorCode, buf);
    }
  }
  else
  {
    char buf[200];
    sprintf(buf, "Initialisation file error: unrecognised setting %s.", settingName);
    if (interactive)
      OnInform(buf);
    strcpy(errorCode, buf);
  }
  return errorCode;
}

bool ReadCustomMacros(char *filename)
{
  if (!wxFileExists(filename))
      return FALSE;

  ifstream istr(filename, ios::in);

  if (istr.bad()) return FALSE;

  CustomMacroList.Clear();
  char ch;
  char macroName[100];
  char macroBody[1000];
  int noArgs;

  while (!istr.eof())
  {
    BibEatWhiteSpace(istr);
    istr.get(ch);
    if (istr.eof())
      break;
      
    if (ch != '\\') // Not a macro definition, so must be NAME=VALUE
    {
      char settingName[100];
      settingName[0] = ch;
      BibReadWord(istr, (settingName+1));
      BibEatWhiteSpace(istr);
      istr.get(ch);
      if (ch != '=')
      {
        OnError("Expected = following name: malformed tex2rtf.ini file.");
        return FALSE;
      }
      else
      {
        char settingValue[200];
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
        OnError("Expected [ followed by number of arguments: malformed tex2rtf.ini file.");
        return FALSE;
      }
      istr >> noArgs;
      istr.get(ch);
      if (ch != ']')
      {
        OnError("Expected ] following number of arguments: malformed tex2rtf.ini file.");
        return FALSE;
      }
      BibEatWhiteSpace(istr);
      istr.get(ch);
      if (ch != '{')
      {
        OnError("Expected { followed by macro body: malformed tex2rtf.ini file.");
        return FALSE;
      }
      CustomMacro *macro = new CustomMacro(macroName, noArgs, NULL);
      BibReadValue(istr, macroBody, FALSE, FALSE); // Don't ignore extra braces
      if (strlen(macroBody) > 0)
        macro->macroBody = copystring(macroBody);
    
      BibEatWhiteSpace(istr);
      CustomMacroList.Append(macroName, macro);
      AddMacroDef(ltCUSTOM_MACRO, macroName, noArgs);
    }
  }
  char mbuf[200];
  sprintf(mbuf, "Read initialization file %s.", filename);
  OnInform(mbuf);
  return TRUE;
}
 
CustomMacro *FindCustomMacro(char *name)
{
  wxNode *node = CustomMacroList.Find(name);
  if (node)
  {
    CustomMacro *macro = (CustomMacro *)node->Data();
    return macro;
  }
  return NULL;
}

// Display custom macros
void ShowCustomMacros(void)
{
  wxNode *node = CustomMacroList.First();
  if (!node)
  {
    OnInform("No custom macros loaded.\n");
    return;
  }
  
  char buf[400];
  while (node)
  {
    CustomMacro *macro = (CustomMacro *)node->Data();
    sprintf(buf, "\\%s[%d]\n    {%s}", macro->macroName, macro->noArgs,
     macro->macroBody ? macro->macroBody : "");
    OnInform(buf);
    node = node->Next();
  }
}

// Parse a string into several comma-separated fields
char *ParseMultifieldString(char *allFields, int *pos)
{
  static char buffer[300];
  int i = 0;
  int fieldIndex = *pos;
  int len = strlen(allFields);
  int oldPos = *pos;
  bool keepGoing = TRUE;
  while ((fieldIndex <= len) && keepGoing)
  {
    if (allFields[fieldIndex] == ' ')
    {
      // Skip
      fieldIndex ++;
    }
    else if (allFields[fieldIndex] == ',')
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
 
ColourTableEntry::ColourTableEntry(char *theName, unsigned int r,  unsigned int g,  unsigned int b)
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

void AddColour(char *theName, unsigned int r,  unsigned int g,  unsigned int b)
{
  wxNode *node = ColourTable.Find(theName);
  if (node)
  {
    ColourTableEntry *entry = (ColourTableEntry *)node->Data();
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

int FindColourPosition(char *theName)
{
  int i = 0;
  wxNode *node = ColourTable.First();
  while (node)
  {
    ColourTableEntry *entry = (ColourTableEntry *)node->Data();
    if (strcmp(theName, entry->name) == 0)
      return i;
    i ++;
    node = node->Next();
  }
  return -1;
}

// Converts e.g. "red" -> "#FF0000"
extern void DecToHex(int, char *);
bool FindColourHTMLString(char *theName, char *buf)
{
  int i = 0;
  wxNode *node = ColourTable.First();
  while (node)
  {
    ColourTableEntry *entry = (ColourTableEntry *)node->Data();
    if (strcmp(theName, entry->name) == 0)
    {
        strcpy(buf, "#");
        
        char buf2[3];
        DecToHex(entry->red, buf2);
        strcat(buf, buf2);
        DecToHex(entry->green, buf2);
        strcat(buf, buf2);
        DecToHex(entry->blue, buf2);
        strcat(buf, buf2);

        return TRUE;
    }
    i ++;
    node = node->Next();
  }
  return FALSE;
}

  
void InitialiseColourTable(void)
{
  // \\red0\\green0\\blue0;
  AddColour("black", 0,0,0);

  // \\red0\\green0\\blue255;\\red0\\green255\\blue255;\n");
  AddColour("cyan", 0,255,255);

  // \\red0\\green255\\blue0;
  AddColour("green", 0,255,0);
  
  // \\red255\\green0\\blue255;
  AddColour("magenta", 255,0,255);

  // \\red255\\green0\\blue0;
  AddColour("red", 255,0,0);
  
  // \\red255\\green255\\blue0;
  AddColour("yellow", 255,255,0);
  
  // \\red255\\green255\\blue255;}");
  AddColour("white", 255,255,255);
}

/*
 * The purpose of this is to reduce the number of times wxYield is
 * called, since under Windows this can slow things down.
 */
 
static int yieldCount = 0;

void Tex2RTFYield(bool force)
{
#ifdef __WXMSW__
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
void AddKeyWordForTopic(char *topic, char *entry, char *filename)
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
    TexTopic *texTopic = (TexTopic *)node->Data();
    delete texTopic;
    node = TopicTable.Next();
  }
  TopicTable.Clear();
}


/*
 * TexTopic structure
 */
 
TexTopic::TexTopic(char *f)
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
char *ConvertCase(char *s)
{
  static char buf[256];
  int len = strlen(s);
  int i;
  if (upperCaseNames)
    for (i = 0; i < len; i ++)
      buf[i] = toupper(s[i]);
  else
    for (i = 0; i < len; i ++)
      buf[i] = tolower(s[i]);
  buf[i] = 0;
  return buf;  
}

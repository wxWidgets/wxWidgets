/////////////////////////////////////////////////////////////////////////////
// Name:        xlputils.cpp
// Purpose:     Converts Latex to obsolete XLP format
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

static inline wxChar* copystring(const wxChar* s)
    { return wxStrcpy(new wxChar[wxStrlen(s) + 1], s); }

long currentBlockId = -1;
static TexChunk *descriptionItemArg = NULL;
static int indentLevel = 0;
static int noColumns = 0;
static int currentTab = 0;
static bool tableVerticalLineLeft = false;
static bool tableVerticalLineRight = false;
static bool inTable = false;
static int citeCount = 1;
wxList hyperLinks(wxKEY_INTEGER);
wxList hyperLabels(wxKEY_STRING);
FILE *Index = NULL;


extern wxHashTable TexReferences;


void PadToTab(int tabPos)
{
  int currentCol = GetCurrentColumn();
  for (int i = currentCol; i < tabPos; i++)
    TexOutput(_T(" "), true);
}

static long xlpBlockId = 0;
long NewBlockId(void)
{
  return xlpBlockId ++;
}

// Called on start/end of macro examination
void XLPOnMacro(int macroId, int no_args, bool start)
{
  wxChar buf[100];
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

      SetCurrentOutputs(Contents, Chapters);
      long id1 = NewBlockId();
      currentBlockId = NewBlockId();

      startedSections = true;
      wxFprintf(Contents, _T("\\hy-%d{%ld}{"), hyBLOCK_SMALL_HEADING, id1);
      wxFprintf(Chapters, _T("\n\\hy-%d{%ld}{"), hyBLOCK_LARGE_VISIBLE_SECTION, currentBlockId);
      wxFprintf(Index, _T("%ld %ld\n"), id1, currentBlockId);

      OutputCurrentSection(); // Repeat section header

      wxFprintf(Contents, _T("}\n\n"));
      wxFprintf(Chapters, _T("}\n\n"));
      SetCurrentOutput(Chapters);
      wxChar *topicName = FindTopicName(GetNextChunk());
      hyperLabels.Append(topicName, (wxObject *)currentBlockId);
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

      if (macroId != ltSECTIONSTAR)
        sectionNo ++;

      SetCurrentOutputs(Chapters, Sections);
      long id1 = NewBlockId();
      currentBlockId = NewBlockId();

      startedSections = true;

      if (DocumentStyle == LATEX_ARTICLE)
        wxFprintf(Contents, _T("\\hy-%d{%ld}{"), hyBLOCK_LARGE_HEADING, id1);
      else
        wxFprintf(Chapters, _T("\\hy-%d{%ld}{"), hyBLOCK_BOLD, id1);
      wxFprintf(Sections, _T("\n\\hy-%d{%ld}{"), hyBLOCK_LARGE_VISIBLE_SECTION, currentBlockId);
      wxFprintf(Index, _T("%ld %ld\n"), id1, currentBlockId);

      OutputCurrentSection(); // Repeat section header

      if (DocumentStyle == LATEX_ARTICLE)
        wxFprintf(Contents, _T("}\n\n"));
      else
        wxFprintf(Chapters, _T("}\n\n"));
      wxFprintf(Sections, _T("}\n\n"));
      SetCurrentOutput(Sections);
      wxChar *topicName = FindTopicName(GetNextChunk());
      hyperLabels.Append(topicName, (wxObject *)currentBlockId);
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
      subsubsectionNo = 0;

      if (macroId != ltSUBSECTIONSTAR)
        subsectionNo ++;

      SetCurrentOutputs(Sections, Subsections);
      long id1 = NewBlockId();
      currentBlockId = NewBlockId();
      wxFprintf(Sections, _T("\\hy-%d{%ld}{"), hyBLOCK_BOLD, id1);
      wxFprintf(Subsections, _T("\n\\hy-%d{%ld}{"), hyBLOCK_LARGE_VISIBLE_SECTION, currentBlockId);
      wxFprintf(Index, _T("%ld %ld\n"), id1, currentBlockId);

      OutputCurrentSection(); // Repeat section header

      wxFprintf(Sections, _T("}\n\n"));
      wxFprintf(Subsections, _T("}\n\n"));
      SetCurrentOutput(Subsections);
      wxChar *topicName = FindTopicName(GetNextChunk());
      hyperLabels.Append(topicName, (wxObject *)currentBlockId);
    }
    break;
  }
  case ltSUBSUBSECTION:
  case ltSUBSUBSECTIONSTAR:
  {
    if (!start)
    {
      if (macroId != ltSUBSUBSECTIONSTAR)
        subsubsectionNo ++;

      SetCurrentOutputs(Subsections, Subsubsections);
      long id1 = NewBlockId();
      currentBlockId = NewBlockId();
      wxFprintf(Subsections, _T("\\hy-%d{%ld}{"), hyBLOCK_BOLD, id1);
      wxFprintf(Subsubsections, _T("\n\\hy-%d{%ld}{"), hyBLOCK_LARGE_VISIBLE_SECTION, currentBlockId);
      wxFprintf(Index, _T("%ld %ld\n"), id1, currentBlockId);

      OutputCurrentSection(); // Repeat section header

      wxFprintf(Subsections, _T("}\n\n"));
      wxFprintf(Subsubsections, _T("}\n\n"));
      SetCurrentOutput(Subsubsections);
      wxChar *topicName = FindTopicName(GetNextChunk());
      hyperLabels.Append(topicName, (wxObject *)currentBlockId);
    }
    break;
  }
  case ltFUNC:
  case ltPFUNC:
  case ltMEMBER:
  {
    SetCurrentOutput(Subsections);
    if (start)
    {
      long id = NewBlockId();
      wxFprintf(Subsections, _T("\\hy-%d{%ld}{"), hyBLOCK_BOLD, id);
    }
    else
      wxFprintf(Subsections, _T("}"));
    break;
  }
  case ltVOID:
//    if (start)
//      TexOutput(_T("void"), true);
    break;
  case ltBACKSLASHCHAR:
    if (start)
      TexOutput(_T("\n"), true);
    break;
  case ltPAR:
  {
    if (start)
    {
      if (ParSkip > 0)
        TexOutput(_T("\n"), true);
      TexOutput(_T("\n"), true);
    }
    break;
  }
  case ltRMFAMILY:
  case ltTEXTRM:
  case ltRM:
  {
    break;
  }
  case ltTEXTBF:
  case ltBFSERIES:
  case ltBF:
  {
    if (start)
    {
      wxChar buf[100];
      long id = NewBlockId();
      wxSnprintf(buf, sizeof(buf), _T("\\hy-%d{%ld}{"), hyBLOCK_BOLD, id);
      TexOutput(buf);
    }
    else TexOutput(_T("}"));
    break;
  }
  case ltTEXTIT:
  case ltITSHAPE:
  case ltIT:
  {
    if (start)
    {
      wxChar buf[100];
      long id = NewBlockId();
      wxSnprintf(buf, sizeof(buf), _T("\\hy-%d{%ld}{"), hyBLOCK_ITALIC, id);
      TexOutput(buf);
    }
    else TexOutput(_T("}"));
    break;
  }
  case ltTTFAMILY:
  case ltTEXTTT:
  case ltTT:
  {
    if (start)
    {
      long id = NewBlockId();
      wxSnprintf(buf, sizeof(buf), _T("\\hy-%d{%ld}{"), hyBLOCK_TELETYPE, id);
      TexOutput(buf);
    }
    else TexOutput(_T("}"));
    break;
  }
  case ltSMALL:
  {
    if (start)
    {
      wxSnprintf(buf, sizeof(buf), _T("\\hy-%d{%ld}{"), hyBLOCK_SMALL_TEXT, NewBlockId());
      TexOutput(buf);
    }
    else TexOutput(_T("}"));
    break;
  }
  case ltTINY:
  {
    if (start)
    {
      wxSnprintf(buf, sizeof(buf), _T("\\hy-%d{%ld}{"), hyBLOCK_SMALL_TEXT, NewBlockId());
      TexOutput(buf);
    }
    else TexOutput(_T("}"));
    break;
  }
  case ltNORMALSIZE:
  {
    if (start)
    {
      wxSnprintf(buf, sizeof(buf), _T("\\hy-%d{%ld}{"), hyBLOCK_NORMAL, NewBlockId());
      TexOutput(buf);
    }
    else TexOutput(_T("}"));
    break;
  }
  case ltlarge:
  {
    if (start)
    {
      wxSnprintf(buf, sizeof(buf), _T("\\hy-%d{%ld}{"), hyBLOCK_SMALL_HEADING, NewBlockId());
      TexOutput(buf);
    }
    else TexOutput(_T("}\n"));
    break;
  }
  case ltLARGE:
  {
    if (start)
    {
      wxSnprintf(buf, sizeof(buf), _T("\\hy-%d{%ld}{"), hyBLOCK_LARGE_HEADING, NewBlockId());
      TexOutput(buf);
    }
    else TexOutput(_T("}\n"));
    break;
  }
  case ltITEMIZE:
  case ltENUMERATE:
  case ltDESCRIPTION:
  case ltTWOCOLLIST:
  {
    if (start)
    {
//      tabCount ++;

//      if (indentLevel > 0)
//        TexOutput(_T("\\par\\par\n"));
      indentLevel ++;
      int listType;
      if (macroId == ltENUMERATE)
        listType = LATEX_ENUMERATE;
      else if (macroId == ltITEMIZE)
        listType = LATEX_ITEMIZE;
      else
        listType = LATEX_DESCRIPTION;
      itemizeStack.Insert(new ItemizeStruc(listType));

    }
    else
    {
      indentLevel --;

      if (itemizeStack.GetFirst())
      {
        ItemizeStruc *struc = (ItemizeStruc *)itemizeStack.GetFirst()->GetData();
        delete struc;
        delete itemizeStack.GetFirst();
      }
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
        wxChar indentBuf[30];

        switch (struc->listType)
        {
          case LATEX_ENUMERATE:
          {
            wxSnprintf(indentBuf, sizeof(indentBuf), _T("\\hy-%d{%ld}{%d.} "),
              hyBLOCK_BOLD, NewBlockId(), struc->currentItem);
            TexOutput(indentBuf);
            break;
          }
          case LATEX_ITEMIZE:
          {
            wxSnprintf(indentBuf, sizeof(indentBuf), _T("\\hy-%d{%ld}{o} "),
              hyBLOCK_BOLD, NewBlockId());
            TexOutput(indentBuf);
            break;
          }
          default:
          case LATEX_DESCRIPTION:
          {
            if (descriptionItemArg)
            {
              wxSnprintf(indentBuf, sizeof(indentBuf), _T("\\hy-%d{%ld}{"),
                 hyBLOCK_BOLD, NewBlockId());
              TexOutput(indentBuf);
              TraverseChildrenFromChunk(descriptionItemArg);
              TexOutput(_T("} "));
              descriptionItemArg = NULL;
            }
            break;
          }
        }
      }
    }
    break;
  }
  case ltMAKETITLE:
  {
    if (start && DocumentTitle && DocumentAuthor)
    {
      wxSnprintf(buf, sizeof(buf), _T("\\hy-%d{%ld}{"), hyBLOCK_LARGE_HEADING, NewBlockId());
      TexOutput(buf);
      TraverseChildrenFromChunk(DocumentTitle);
      TexOutput(_T("}\n\n"));
      wxSnprintf(buf, sizeof(buf), _T("\\hy-%d{%ld}{"), hyBLOCK_SMALL_HEADING, NewBlockId());
      TexOutput(buf);
      TraverseChildrenFromChunk(DocumentAuthor);
      TexOutput(_T("}\n\n"));
      if (DocumentDate)
      {
        TraverseChildrenFromChunk(DocumentDate);
        TexOutput(_T("\n"));
      }
    }
    break;
  }
  case ltTABLEOFCONTENTS:
  {
    if (start)
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
        TexOutput(_T("RUN TEX2RTF AGAIN FOR CONTENTS PAGE\n"));
        OnInform(_T("Run Tex2RTF again to include contents page."));
      }
    }
    break;
  }
  case ltHARDY:
  {
    if (start)
      TexOutput(_T("HARDY"), true);
    break;
  }
  case ltWXCLIPS:
  {
    if (start)
      TexOutput(_T("wxCLIPS"), true);
    break;
  }
  case ltVERBATIM:
  {
    if (start)
    {
      wxChar buf[100];
      long id = NewBlockId();
      wxSnprintf(buf, sizeof(buf), _T("\\hy-%d{%ld}{"), hyBLOCK_TELETYPE, id);
      TexOutput(buf);
    }
    else TexOutput(_T("}"));
    break;
  }
  case ltHRULE:
  {
    if (start)
    {
      TexOutput(_T("\n------------------------------------------------------------------"), true);
    }
    break;
  }
  case ltHLINE:
  {
    if (start)
    {
      TexOutput(_T("--------------------------------------------------------------------------------"), true);
    }
    break;
  }
  case ltSPECIALAMPERSAND:
  {
    if (start)
    {
      currentTab ++;
      int tabPos = (80/noColumns)*currentTab;
      PadToTab(tabPos);
    }
    break;
  }
  case ltTABULAR:
  case ltSUPERTABULAR:
  {
    if (start)
    {
      wxSnprintf(buf, sizeof(buf), _T("\\hy-%d{%ld}{"), hyBLOCK_TELETYPE, NewBlockId());
      TexOutput(buf);
    }
    else
      TexOutput(_T("}"));
    break;
  }
  case ltNUMBEREDBIBITEM:
  {
    if (!start)
      TexOutput(_T("\n\n"), true);
    break;
  }
  case ltCAPTION:
  case ltCAPTIONSTAR:
  {
    if (start)
    {
      figureNo ++;

      wxChar figBuf[40];
      if (DocumentStyle != LATEX_ARTICLE)
        wxSnprintf(figBuf, sizeof(figBuf), _T("Figure %d.%d: "), chapterNo, figureNo);
      else
        wxSnprintf(figBuf, sizeof(figBuf), _T("Figure %d: "), figureNo);

      TexOutput(figBuf);
    }
    else
    {
      wxChar *topicName = FindTopicName(GetNextChunk());

      AddTexRef(topicName, NULL, NULL,
           ((DocumentStyle != LATEX_ARTICLE) ? chapterNo : figureNo),
            ((DocumentStyle != LATEX_ARTICLE) ? figureNo : 0));
    }
    break;
  }
  default:
  {
    DefaultOnMacro(macroId, no_args, start);
    break;
  }
  }
}

bool XLPOnArgument(int macroId, int arg_no, bool start)
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
  {
    if (!start && (arg_no == 1))
      currentSection = GetArgChunk();
    return false;
  }
  case ltFUNC:
  {
    if (!start && (arg_no == 1))
      TexOutput(_T(" "), true);
    if (start && (arg_no == 3))
      TexOutput(_T("("), true);
    if (!start && (arg_no == 3))
     TexOutput(_T(")"), true);
    break;
  }
  case ltPFUNC:
  {
    if (!start && (arg_no == 1))
      TexOutput(_T(" "), true);

    if (start && (arg_no == 2))
      TexOutput(_T("(*"), true);
    if (!start && (arg_no == 2))
      TexOutput(_T(")"), true);

    if (start && (arg_no == 3))
      TexOutput(_T("("), true);
    if (!start && (arg_no == 3))
      TexOutput(_T(")"), true);
    break;
  }
  case ltCLIPSFUNC:
  {
    if (!start && (arg_no == 1))
      TexOutput(_T(" "), true);
    if (start && (arg_no == 2))
    {
      TexOutput(_T("("), true);
      long id = NewBlockId();
      wxSnprintf(buf, sizeof(buf), _T("\\hy-%d{%ld}{"), hyBLOCK_BOLD, id);
      TexOutput(buf);
    }
    if (!start && (arg_no == 2))
    {
      TexOutput(_T("}"));
    }
    if (!start && (arg_no == 3))
     TexOutput(_T(")"), true);
    break;
  }
  case ltPARAM:
  {
    if (start && (arg_no == 2))
    {
      long id = NewBlockId();
      wxSnprintf(buf, sizeof(buf), _T(" \\hy-%d{%ld}{"), hyBLOCK_BOLD, id);
      TexOutput(buf);
    }
    if (!start && (arg_no == 2))
    {
      TexOutput(_T("}"));
    }
    break;
  }
  case ltCPARAM:
  {
    if (start && (arg_no == 2))
    {
      long id = NewBlockId();
      wxSnprintf(buf, sizeof(buf), _T(" \\hy-%d{%ld}{"), hyBLOCK_BOLD, id);
      TexOutput(buf);
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
      TexOutput(_T(" "), true);
    break;
  }
  case ltLABEL:
  {
    return false;
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
  case ltHELPREF:
  case ltHELPREFN:
  case ltPOPREF:
  {
    if (arg_no == 1)
    {
      if (start)
      {
        currentBlockId = NewBlockId();
        wxSnprintf(buf, sizeof(buf), _T("\\hy-%d{%ld}{"), hyBLOCK_RED_ITALIC, currentBlockId);
        TexOutput(buf);
      }
      else TexOutput(_T("}"));
    }
    if (arg_no == 2)
    {
      if (start)
      {
        wxChar *label = GetArgData();
        hyperLinks.Append(currentBlockId, (wxObject *)copystring(label));
      }

      return false;
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
        TexOutput(_T(" ("));
      else
        TexOutput(_T(")"));
      return true;
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
  case ltTABULAR:
  case ltSUPERTABULAR:
  {
    if (arg_no == 1)
    {
      if (start)
      {
        inTable = true;
        tableVerticalLineLeft = false;
        tableVerticalLineRight = false;

        wxChar *alignString = copystring(GetArgData());

        // Count the number of columns
        noColumns = 0;
        int len = wxStrlen(alignString);
        if (len > 0)
        {
          if (alignString[0] == '|')
            tableVerticalLineLeft = true;
          if (alignString[len-1] == '|')
            tableVerticalLineRight = true;
        }

        for (int i = 0; i < len; i++)
          if (isalpha(alignString[i]))
            noColumns ++;

/*
      // Experimental
      TexOutput(_T("\\brdrt\\brdrs"));
      if (tableVerticalLineLeft)
        TexOutput(_T("\\brdrl\\brdrs"));
      if (tableVerticalLineRight)
        TexOutput(_T("\\brdrr\\brdrs"));
*/

        // Calculate a rough size for each column
//        int tabPos = 80/noColumns;
        currentTab = 0;

        return false;
      }
    }
    else if (arg_no == 2 && !start)
    {
      inTable = false;
    }
    else if (arg_no == 2 && start)
      return true;
    break;
  }
  case ltMARGINPAR:
  case ltMARGINPAREVEN:
  case ltMARGINPARODD:
  case ltNORMALBOX:
  case ltNORMALBOXD:
  {
    if (start)
    {
      TexOutput(_T("----------------------------------------------------------------------\n"), true);
      return true;
    }
    else
      TexOutput(_T("\n----------------------------------------------------------------------\n"), true);
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

      wxSnprintf(buf, sizeof(buf), _T("\\hy-%d{%ld}{[%d]} "), hyBLOCK_BOLD, NewBlockId(), citeCount);
      TexOutput(buf);
      citeCount ++;
      return false;
    }
    return true;
  }
  case ltTHEBIBLIOGRAPHY:
  {
    if (start && (arg_no == 1))
    {
      citeCount = 1;

      SetCurrentOutput(Chapters);

      SetCurrentOutputs(Contents, Chapters);
      long id1 = NewBlockId();
      long id2 = NewBlockId();
      wxFprintf(Contents, _T("\\hy-%d{%ld}{%s}\n"), hyBLOCK_SMALL_HEADING, id1, ReferencesNameString);
      wxFprintf(Chapters, _T("\\hy-%d{%ld}{%s}\n\n\n"), hyBLOCK_LARGE_VISIBLE_SECTION, id2, ReferencesNameString);
      wxFprintf(Index, _T("%ld %ld\n"), id1, id2);

      SetCurrentOutput(Chapters);
      return false;
    }
    if (!start && (arg_no == 2))
    {
    }
    return true;
  }
  case ltTWOCOLITEM:
  case ltTWOCOLITEMRULED:
  {
    if (start && (arg_no == 2))
      TexOutput(_T("\n    "));

    if (!start && (arg_no == 2))
      TexOutput(_T("\n"));
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
          case _T('a'):
           TexOutput(_T("a"));
           break;
          case _T('e'):
           TexOutput(_T("e"));
           break;
          case _T('i'):
           TexOutput(_T("i"));
           break;
          case _T('o'):
           TexOutput(_T("o"));
           break;
          case _T('u'):
           TexOutput(_T("u"));
           break;
          case _T('A'):
           TexOutput(_T("a"));
           break;
          case _T('E'):
           TexOutput(_T("E"));
           break;
          case _T('I'):
           TexOutput(_T("I"));
           break;
          case _T('O'):
           TexOutput(_T("O"));
           break;
          case _T('U'):
           TexOutput(_T("U"));
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
          case _T('a'):
           TexOutput(_T("a"));
           break;
          case _T('e'):
           TexOutput(_T("e"));
           break;
          case _T('i'):
           TexOutput(_T("i"));
           break;
          case _T('o'):
           TexOutput(_T("o"));
           break;
          case _T('u'):
           TexOutput(_T("u"));
           break;
          case _T('y'):
           TexOutput(_T("y"));
           break;
          case _T('A'):
           TexOutput(_T("A"));
           break;
          case _T('E'):
           TexOutput(_T("E"));
           break;
          case _T('I'):
           TexOutput(_T("I"));
           break;
          case _T('O'):
           TexOutput(_T("O"));
           break;
          case _T('U'):
           TexOutput(_T("U"));
           break;
          case _T('Y'):
           TexOutput(_T("Y"));
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
          case _T('a'):
           TexOutput(_T("a"));
           break;
          case _T('e'):
           TexOutput(_T("e"));
           break;
          case _T('i'):
           TexOutput(_T("i"));
           break;
          case _T('o'):
           TexOutput(_T("o"));
           break;
          case _T('u'):
           TexOutput(_T("u"));
           break;
          case _T('A'):
           TexOutput(_T("A"));
           break;
          case _T('E'):
           TexOutput(_T("E"));
           break;
          case _T('I'):
           TexOutput(_T("I"));
           break;
          case _T('O'):
           TexOutput(_T("O"));
           break;
          case _T('U'):
           TexOutput(_T("U"));
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
          case _T('a'):
           TexOutput(_T("a"));
           break;
          case _T(' '):
           TexOutput(_T("~"));
           break;
          case _T('n'):
           TexOutput(_T("n"));
           break;
          case _T('o'):
           TexOutput(_T("o"));
           break;
          case _T('A'):
           TexOutput(_T("A"));
           break;
          case _T('N'):
           TexOutput(_T("N"));
           break;
          case _T('O'):
           TexOutput(_T("O"));
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
          case _T('a'):
           TexOutput(_T("a"));
           break;
          case _T('e'):
           TexOutput(_T("e"));
           break;
          case _T('i'):
           TexOutput(_T("i"));
           break;
          case _T('o'):
           TexOutput(_T("o"));
           break;
          case _T('u'):
           TexOutput(_T("u"));
           break;
          case _T('y'):
           TexOutput(_T("y"));
           break;
          case _T('A'):
           TexOutput(_T("A"));
           break;
          case _T('E'):
           TexOutput(_T("E"));
           break;
          case _T('I'):
           TexOutput(_T("I"));
           break;
          case _T('O'):
           TexOutput(_T("O"));
           break;
          case _T('U'):
           TexOutput(_T("U"));
           break;
          case _T('Y'):
           TexOutput(_T("Y"));
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
          case _T('a'):
           TexOutput(_T("a"));
           break;
          case _T('A'):
           TexOutput(_T("A"));
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
          case _T('c'):
           TexOutput(_T("c"));
           break;
          case _T('C'):
           TexOutput(_T("C"));
           break;
          default:
           break;
        }
      }
    }
    return false;
  }
  default:
  {
    return DefaultOnArgument(macroId, arg_no, start);
  }
  }
  return true;
}

bool XLPGo(void)
{
  xlpBlockId = 0;

  if (!InputFile.empty() && !OutputFile.empty())
  {
    Contents = wxFopen(TmpContentsName, _T("w"));
    Chapters = wxFopen(_T("chapters.xlp"), _T("w"));
    Sections = wxFopen(_T("sections.xlp"), _T("w"));
    Subsections = wxFopen(_T("subsections.xlp"), _T("w"));
    Subsubsections = wxFopen(_T("subsubsections.xlp"), _T("w"));
    Index = wxFopen(_T("index.xlp"), _T("w"));

    // Insert invisible section marker at beginning
    wxFprintf(Chapters, _T("\\hy-%d{%ld}{%s}\n"),
                hyBLOCK_INVISIBLE_SECTION, NewBlockId(), _T("\n"));

    wxFprintf(Contents, _T("\\hy-%d{%ld}{%s}\n\n"),
//                hyBLOCK_LARGE_HEADING, NewBlockId(), "\n\n%s\n\n", ContentsNameString);
                hyBLOCK_LARGE_HEADING, NewBlockId(), ContentsNameString);

    SetCurrentOutput(Chapters);

    wxFprintf(Index, _T("\n\\hyindex{\n\"%s\"\n"),
             contentsString ? contentsString : _T("WXHELPCONTENTS"));
    TraverseDocument();

    wxNode *node = hyperLinks.GetFirst();
    while (node)
    {
      long from = node->GetKeyInteger();
      wxChar *label = (wxChar *)node->GetData();
      wxNode *otherNode = hyperLabels.Find(label);
      if (otherNode)
      {
        long to = (long)otherNode->GetData();
        wxFprintf(Index, _T("%ld %ld\n"), from, to);
      }
      node = node->GetNext();
    }

    wxFprintf(Index, _T("}\n"));

    fclose(Contents); Contents = NULL;
    fclose(Chapters); Chapters = NULL;
    fclose(Sections); Sections = NULL;
    fclose(Subsections); Subsections = NULL;
    fclose(Subsubsections); Subsubsections = NULL;
    fclose(Index); Index = NULL;

    if (wxFileExists(ContentsName)) wxRemoveFile(ContentsName);

    if (!wxRenameFile(TmpContentsName, ContentsName))
    {
      wxCopyFile(TmpContentsName, ContentsName);
      wxRemoveFile(TmpContentsName);
    }

    wxConcatFiles(_T("chapters.xlp"), _T("sections.xlp"), _T("tmp2.xlp"));
    wxConcatFiles(_T("tmp2.xlp"), _T("subsections.xlp"), _T("tmp1.xlp"));
    wxConcatFiles(_T("tmp1.xlp"), _T("subsubsections.xlp"), _T("tmp2.xlp"));
    wxConcatFiles(_T("tmp2.xlp"), _T("index.xlp"), OutputFile);

    wxRemoveFile(_T("tmp1.xlp"));
    wxRemoveFile(_T("tmp2.xlp"));

    wxRemoveFile(_T("chapters.xlp"));
    wxRemoveFile(_T("sections.xlp"));
    wxRemoveFile(_T("subsections.xlp"));
    wxRemoveFile(_T("subsubsections.xlp"));
    wxRemoveFile(_T("index.xlp"));
    return true;
  }
  return false;
}

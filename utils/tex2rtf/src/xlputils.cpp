/////////////////////////////////////////////////////////////////////////////
// Name:        xlputils.cpp
// Purpose:     Converts Latex to obsolete XLP format
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

long currentBlockId = -1;
static TexChunk *descriptionItemArg = NULL;
static int indentLevel = 0;
static int noColumns = 0;
static int currentTab = 0;
static bool tableVerticalLineLeft = FALSE;
static bool tableVerticalLineRight = FALSE;
static bool inTable = FALSE;
static int citeCount = 1;
wxList hyperLinks(wxKEY_INTEGER);
wxList hyperLabels(wxKEY_STRING);
FILE *Index = NULL;


extern wxHashTable TexReferences;


void PadToTab(int tabPos)
{
  int currentCol = GetCurrentColumn();
  for (int i = currentCol; i < tabPos; i++)
    TexOutput(" ", TRUE);
}

static long xlpBlockId = 0;
long NewBlockId(void)
{
  return xlpBlockId ++;
}

// Called on start/end of macro examination
void XLPOnMacro(int macroId, int no_args, bool start)
{
  char buf[100];
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

      startedSections = TRUE;
      fprintf(Contents, "\\hy-%d{%ld}{", hyBLOCK_SMALL_HEADING, id1);
      fprintf(Chapters, "\n\\hy-%d{%ld}{", hyBLOCK_LARGE_VISIBLE_SECTION, currentBlockId);
      fprintf(Index, "%ld %ld\n", id1, currentBlockId);

      OutputCurrentSection(); // Repeat section header

      fprintf(Contents, "}\n\n");
      fprintf(Chapters, "}\n\n");
      SetCurrentOutput(Chapters);
      char *topicName = FindTopicName(GetNextChunk());
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

      startedSections = TRUE;

      if (DocumentStyle == LATEX_ARTICLE)
        fprintf(Contents, "\\hy-%d{%ld}{", hyBLOCK_LARGE_HEADING, id1);
      else
        fprintf(Chapters, "\\hy-%d{%ld}{", hyBLOCK_BOLD, id1);
      fprintf(Sections, "\n\\hy-%d{%ld}{", hyBLOCK_LARGE_VISIBLE_SECTION, currentBlockId);
      fprintf(Index, "%ld %ld\n", id1, currentBlockId);

      OutputCurrentSection(); // Repeat section header

      if (DocumentStyle == LATEX_ARTICLE)
        fprintf(Contents, "}\n\n");
      else
        fprintf(Chapters, "}\n\n");
      fprintf(Sections, "}\n\n");
      SetCurrentOutput(Sections);
      char *topicName = FindTopicName(GetNextChunk());
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
      fprintf(Sections, "\\hy-%d{%ld}{", hyBLOCK_BOLD, id1);
      fprintf(Subsections, "\n\\hy-%d{%ld}{", hyBLOCK_LARGE_VISIBLE_SECTION, currentBlockId);
      fprintf(Index, "%ld %ld\n", id1, currentBlockId);

      OutputCurrentSection(); // Repeat section header

      fprintf(Sections, "}\n\n");
      fprintf(Subsections, "}\n\n");
      SetCurrentOutput(Subsections);
      char *topicName = FindTopicName(GetNextChunk());
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
      fprintf(Subsections, "\\hy-%d{%ld}{", hyBLOCK_BOLD, id1);
      fprintf(Subsubsections, "\n\\hy-%d{%ld}{", hyBLOCK_LARGE_VISIBLE_SECTION, currentBlockId);
      fprintf(Index, "%ld %ld\n", id1, currentBlockId);

      OutputCurrentSection(); // Repeat section header

      fprintf(Subsections, "}\n\n");
      fprintf(Subsubsections, "}\n\n");
      SetCurrentOutput(Subsubsections);
      char *topicName = FindTopicName(GetNextChunk());
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
      fprintf(Subsections, "\\hy-%d{%ld}{", hyBLOCK_BOLD, id);
    }
    else
      fprintf(Subsections, "}");
    break;
  }
  case ltVOID:
//    if (start)
//      TexOutput("void", TRUE);
    break;
  case ltBACKSLASHCHAR:
    if (start)
      TexOutput("\n", TRUE);
    break;
  case ltPAR:
  {
    if (start)
    {
      if (ParSkip > 0)
        TexOutput("\n", TRUE);
      TexOutput("\n", TRUE);
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
      char buf[100];
      long id = NewBlockId();
      sprintf(buf, "\\hy-%d{%ld}{", hyBLOCK_BOLD, id);
      TexOutput(buf);
    }
    else TexOutput("}");
    break;
  }
  case ltTEXTIT:
  case ltITSHAPE:
  case ltIT:
  {
    if (start)
    {
      char buf[100];
      long id = NewBlockId();
      sprintf(buf, "\\hy-%d{%ld}{", hyBLOCK_ITALIC, id);
      TexOutput(buf);
    }
    else TexOutput("}");
    break;
  }
  case ltTTFAMILY:
  case ltTEXTTT:
  case ltTT:
  {
    if (start)
    {
      long id = NewBlockId();
      sprintf(buf, "\\hy-%d{%ld}{", hyBLOCK_TELETYPE, id);
      TexOutput(buf);
    }
    else TexOutput("}");
    break;
  }
  case ltSMALL:
  {
    if (start)
    {
      sprintf(buf, "\\hy-%d{%ld}{", hyBLOCK_SMALL_TEXT, NewBlockId());
      TexOutput(buf);
    }
    else TexOutput("}");
    break;
  }
  case ltTINY:
  {
    if (start)
    {
      sprintf(buf, "\\hy-%d{%ld}{", hyBLOCK_SMALL_TEXT, NewBlockId());
      TexOutput(buf);
    }
    else TexOutput("}");
    break;
  }
  case ltNORMALSIZE:
  {
    if (start)
    {
      sprintf(buf, "\\hy-%d{%ld}{", hyBLOCK_NORMAL, NewBlockId());
      TexOutput(buf);
    }
    else TexOutput("}");
    break;
  }
  case ltlarge:
  {
    if (start)
    {
      sprintf(buf, "\\hy-%d{%ld}{", hyBLOCK_SMALL_HEADING, NewBlockId());
      TexOutput(buf);
    }
    else TexOutput("}\n");
    break;
  }
  case ltLARGE:
  {
    if (start)
    {
      sprintf(buf, "\\hy-%d{%ld}{", hyBLOCK_LARGE_HEADING, NewBlockId());
      TexOutput(buf);
    }
    else TexOutput("}\n");
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
//        TexOutput("\\par\\par\n");
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

      if (itemizeStack.First())
      {
        ItemizeStruc *struc = (ItemizeStruc *)itemizeStack.First()->Data();
        delete struc;
        delete itemizeStack.First();
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
        char indentBuf[30];

        switch (struc->listType)
        {
          case LATEX_ENUMERATE:
          {
            sprintf(indentBuf, "\\hy-%d{%ld}{%d.} ",
              hyBLOCK_BOLD, NewBlockId(), struc->currentItem);
            TexOutput(indentBuf);
            break;
          }
          case LATEX_ITEMIZE:
          {
            sprintf(indentBuf, "\\hy-%d{%ld}{o} ",
              hyBLOCK_BOLD, NewBlockId());
            TexOutput(indentBuf);
            break;
          }
          default:
          case LATEX_DESCRIPTION:
          {
            if (descriptionItemArg)
            {
              sprintf(indentBuf, "\\hy-%d{%ld}{",
                 hyBLOCK_BOLD, NewBlockId());
              TexOutput(indentBuf);
              TraverseChildrenFromChunk(descriptionItemArg);
              TexOutput("} ");
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
      sprintf(buf, "\\hy-%d{%ld}{", hyBLOCK_LARGE_HEADING, NewBlockId());
      TexOutput(buf);
      TraverseChildrenFromChunk(DocumentTitle);
      TexOutput("}\n\n");
      sprintf(buf, "\\hy-%d{%ld}{", hyBLOCK_SMALL_HEADING, NewBlockId());
      TexOutput(buf);
      TraverseChildrenFromChunk(DocumentAuthor);
      TexOutput("}\n\n");
      if (DocumentDate)
      {
        TraverseChildrenFromChunk(DocumentDate);
        TexOutput("\n");
      }
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
          putc(ch, Chapters);
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
  case ltHARDY:
  {
    if (start)
      TexOutput("HARDY", TRUE);
    break;
  }
  case ltWXCLIPS:
  {
    if (start)
      TexOutput("wxCLIPS", TRUE);
    break;
  }
  case ltVERBATIM:
  {
    if (start)
    {
      char buf[100];
      long id = NewBlockId();
      sprintf(buf, "\\hy-%d{%ld}{", hyBLOCK_TELETYPE, id);
      TexOutput(buf);
    }
    else TexOutput("}");
    break;
  }
  case ltHRULE:
  {
    if (start)
    {
      TexOutput("\n------------------------------------------------------------------", TRUE);
    }
    break;
  }
  case ltHLINE:
  {
    if (start)
    {
      TexOutput("--------------------------------------------------------------------------------", TRUE);
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
      sprintf(buf, "\\hy-%d{%ld}{", hyBLOCK_TELETYPE, NewBlockId());
      TexOutput(buf);
    }
    else
      TexOutput("}");
    break;
  }
  case ltNUMBEREDBIBITEM:
  {
    if (!start)
      TexOutput("\n\n", TRUE);
    break;
  }
  case ltCAPTION:
  case ltCAPTIONSTAR:
  {
    if (start)
    {
      figureNo ++;

      char figBuf[40];
      if (DocumentStyle != LATEX_ARTICLE)
        sprintf(figBuf, "Figure %d.%d: ", chapterNo, figureNo);
      else
        sprintf(figBuf, "Figure %d: ", figureNo);

      TexOutput(figBuf);
    }
    else
    {
      char *topicName = FindTopicName(GetNextChunk());

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
  {
    if (!start && (arg_no == 1))
      currentSection = GetArgChunk();
    return FALSE;
    break;
  }
  case ltFUNC:
  {
    if (!start && (arg_no == 1))
      TexOutput(" ", TRUE);
    if (start && (arg_no == 3))
      TexOutput("(", TRUE);
    if (!start && (arg_no == 3))
     TexOutput(")", TRUE);
    break;
  }
  case ltPFUNC:
  {
    if (!start && (arg_no == 1))
      TexOutput(" ", TRUE);

    if (start && (arg_no == 2))
      TexOutput("(*", TRUE);
    if (!start && (arg_no == 2))
      TexOutput(")", TRUE);

    if (start && (arg_no == 3))
      TexOutput("(", TRUE);
    if (!start && (arg_no == 3))
      TexOutput(")", TRUE);
    break;
  }
  case ltCLIPSFUNC:
  {
    if (!start && (arg_no == 1))
      TexOutput(" ", TRUE);
    if (start && (arg_no == 2))
    {
      TexOutput("(", TRUE);
      long id = NewBlockId();
      sprintf(buf, "\\hy-%d{%ld}{", hyBLOCK_BOLD, id);
      TexOutput(buf);
    }
    if (!start && (arg_no == 2))
    {
      TexOutput("}");
    }
    if (!start && (arg_no == 3))
     TexOutput(")", TRUE);
    break;
  }
  case ltPARAM:
  {
    if (start && (arg_no == 2))
    {
      long id = NewBlockId();
      sprintf(buf, " \\hy-%d{%ld}{", hyBLOCK_BOLD, id);
      TexOutput(buf);
    }
    if (!start && (arg_no == 2))
    {
      TexOutput("}");
    }
    break;
  }
  case ltCPARAM:
  {
    if (start && (arg_no == 2))
    {
      long id = NewBlockId();
      sprintf(buf, " \\hy-%d{%ld}{", hyBLOCK_BOLD, id);
      TexOutput(buf);
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
      TexOutput(" ", TRUE);
    break;
  }
  case ltLABEL:
  {
    return FALSE;
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
  case ltHELPREF:
  case ltHELPREFN:
  case ltPOPREF:
  {
    if (arg_no == 1)
    {
      if (start)
      {
        currentBlockId = NewBlockId();
        sprintf(buf, "\\hy-%d{%ld}{", hyBLOCK_RED_ITALIC, currentBlockId);
        TexOutput(buf);
      }
      else TexOutput("}");
    }
    if (arg_no == 2)
    {
      if (start)
      {
        char *label = GetArgData();
        hyperLinks.Append(currentBlockId, (wxObject *)copystring(label));
      }
      
      return FALSE;
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
        TexOutput(" (");
      else
        TexOutput(")");
      return TRUE;
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
  case ltTABULAR:
  case ltSUPERTABULAR:
  {
    if (arg_no == 1)
    {
      if (start)
      {
        inTable = TRUE;
        tableVerticalLineLeft = FALSE;
        tableVerticalLineRight = FALSE;

        char *alignString = copystring(GetArgData());

        // Count the number of columns
        noColumns = 0;
        int len = strlen(alignString);
        if (len > 0)
        {
          if (alignString[0] == '|')
            tableVerticalLineLeft = TRUE;
          if (alignString[len-1] == '|')
            tableVerticalLineRight = TRUE;
        }

        for (int i = 0; i < len; i++)
          if (isalpha(alignString[i]))
            noColumns ++;

/*
      // Experimental
      TexOutput("\\brdrt\\brdrs");
      if (tableVerticalLineLeft)
        TexOutput("\\brdrl\\brdrs");
      if (tableVerticalLineRight)
        TexOutput("\\brdrr\\brdrs");
*/

        // Calculate a rough size for each column
//        int tabPos = 80/noColumns;
        currentTab = 0;

        return FALSE;
      }
    }
    else if (arg_no == 2 && !start)
    {
      inTable = FALSE;
    }
    else if (arg_no == 2 && start)
      return TRUE;
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
      TexOutput("----------------------------------------------------------------------\n", TRUE);
      return TRUE;
    }
    else
      TexOutput("\n----------------------------------------------------------------------\n", TRUE);
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

      sprintf(buf, "\\hy-%d{%ld}{[%d]} ", hyBLOCK_BOLD, NewBlockId(), citeCount);
      TexOutput(buf);
      citeCount ++;
      return FALSE;
    }
    return TRUE;
    break;
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
      fprintf(Contents, "\\hy-%d{%ld}{%s}\n", hyBLOCK_SMALL_HEADING, id1, ReferencesNameString);
      fprintf(Chapters, "\\hy-%d{%ld}{%s}\n\n\n", hyBLOCK_LARGE_VISIBLE_SECTION, id2, ReferencesNameString);
      fprintf(Index, "%ld %ld\n", id1, id2);

      SetCurrentOutput(Chapters);
      return FALSE;
    }
    if (!start && (arg_no == 2))
    {
    }
    return TRUE;
    break;
  }
  case ltTWOCOLITEM:
  case ltTWOCOLITEMRULED:
  {
    if (start && (arg_no == 2))
      TexOutput("\n    ");

    if (!start && (arg_no == 2))
      TexOutput("\n");
    return TRUE;
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
           TexOutput("a");
           break;
          case 'e':
           TexOutput("e");
           break;
          case 'i':
           TexOutput("i");
           break;
          case 'o':
           TexOutput("o");
           break;
          case 'u':
           TexOutput("u");
           break;
          case 'A':
           TexOutput("a");
           break;
          case 'E':
           TexOutput("E");
           break;
          case 'I':
           TexOutput("I");
           break;
          case 'O':
           TexOutput("O");
           break;
          case 'U':
           TexOutput("U");
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
           TexOutput("a");
           break;
          case 'e':
           TexOutput("e");
           break;
          case 'i':
           TexOutput("i");
           break;
          case 'o':
           TexOutput("o");
           break;
          case 'u':
           TexOutput("u");
           break;
          case 'y':
           TexOutput("y");
           break;
          case 'A':
           TexOutput("A");
           break;
          case 'E':
           TexOutput("E");
           break;
          case 'I':
           TexOutput("I");
           break;
          case 'O':
           TexOutput("O");
           break;
          case 'U':
           TexOutput("U");
           break;
          case 'Y':
           TexOutput("Y");
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
           TexOutput("a");
           break;
          case 'e':
           TexOutput("e");
           break;
          case 'i':
           TexOutput("i");
           break;
          case 'o':
           TexOutput("o");
           break;
          case 'u':
           TexOutput("u");
           break;
          case 'A':
           TexOutput("A");
           break;
          case 'E':
           TexOutput("E");
           break;
          case 'I':
           TexOutput("I");
           break;
          case 'O':
           TexOutput("O");
           break;
          case 'U':
           TexOutput("U");
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
           TexOutput("a");
           break;
          case ' ':
           TexOutput("~");
           break;
          case 'n':
           TexOutput("n");
           break;
          case 'o':
           TexOutput("o");
           break;
          case 'A':
           TexOutput("A");
           break;
          case 'N':
           TexOutput("N");
           break;
          case 'O':
           TexOutput("O");
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
           TexOutput("a");
           break;
          case 'e':
           TexOutput("e");
           break;
          case 'i':
           TexOutput("i");
           break;
          case 'o':
           TexOutput("o");
           break;
          case 'u':
           TexOutput("u");
           break;
          case 'y':
           TexOutput("y");
           break;
          case 'A':
           TexOutput("A");
           break;
          case 'E':
           TexOutput("E");
           break;
          case 'I':
           TexOutput("I");
           break;
          case 'O':
           TexOutput("O");
           break;
          case 'U':
           TexOutput("U");
           break;
          case 'Y':
           TexOutput("Y");
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
           TexOutput("a");
           break;
          case 'A':
           TexOutput("A");
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
           TexOutput("c");
           break;
          case 'C':
           TexOutput("C");
           break;
          default:
           break;
        }
      }
    }
    return FALSE;
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

bool XLPGo(void)
{
  xlpBlockId = 0;

  if (InputFile && OutputFile)
  {
    Contents = fopen(TmpContentsName, "w");
    Chapters = fopen("chapters.xlp", "w");
    Sections = fopen("sections.xlp", "w");
    Subsections = fopen("subsections.xlp", "w");
    Subsubsections = fopen("subsubsections.xlp", "w");
    Index = fopen("index.xlp", "w");

    // Insert invisible section marker at beginning
    fprintf(Chapters, "\\hy-%d{%ld}{%s}\n",
                hyBLOCK_INVISIBLE_SECTION, NewBlockId(), "\n");

    fprintf(Contents, "\\hy-%d{%ld}{%s}\n\n",
//                hyBLOCK_LARGE_HEADING, NewBlockId(), "\n\n%s\n\n", ContentsNameString);
                hyBLOCK_LARGE_HEADING, NewBlockId(), ContentsNameString);

    SetCurrentOutput(Chapters);

    fprintf(Index, "\n\\hyindex{\n\"%s\"\n",
             contentsString ? contentsString : "WXHELPCONTENTS");
    TraverseDocument();

    wxNode *node = hyperLinks.First();
    while (node)
    {
      long from = node->GetKeyInteger();
      char *label = (char *)node->Data();
      wxNode *otherNode = hyperLabels.Find(label);
      if (otherNode)
      {
        long to = (long)otherNode->Data();
        fprintf(Index, "%ld %ld\n", from, to);
      }
      node = node->Next();
    }

    fprintf(Index, "}\n");

    fclose(Contents); Contents = NULL;
    fclose(Chapters); Chapters = NULL;
    fclose(Sections); Sections = NULL;
    fclose(Subsections); Subsections = NULL;
    fclose(Subsubsections); Subsubsections = NULL;
    fclose(Index); Index = NULL;

    if (FileExists(ContentsName)) wxRemoveFile(ContentsName);

    if (!wxRenameFile(TmpContentsName, ContentsName))
    {
      wxCopyFile(TmpContentsName, ContentsName);
      wxRemoveFile(TmpContentsName);
    }

    wxConcatFiles("chapters.xlp", "sections.xlp", "tmp2.xlp");
    wxConcatFiles("tmp2.xlp", "subsections.xlp", "tmp1.xlp");
    wxConcatFiles("tmp1.xlp", "subsubsections.xlp", "tmp2.xlp");
    wxConcatFiles("tmp2.xlp", "index.xlp", OutputFile);

    wxRemoveFile("tmp1.xlp");
    wxRemoveFile("tmp2.xlp");

    wxRemoveFile("chapters.xlp");
    wxRemoveFile("sections.xlp");
    wxRemoveFile("subsections.xlp");
    wxRemoveFile("subsubsections.xlp");
    wxRemoveFile("index.xlp");
    return TRUE;
  }
  return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// Name:        resourc2.cpp
// Purpose:     Resource system (2nd file). Only required for 16-bit BC++.
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "resource.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_WX_RESOURCES

#ifdef __VISUALC__
    #pragma warning(disable:4706)   // assignment within conditional expression
#endif // VC++

#ifndef WX_PRECOMP
#include "wx/defs.h"
#include "wx/setup.h"
#include "wx/list.h"
#include "wx/hash.h"
#include "wx/gdicmn.h"
#include "wx/utils.h"
#include "wx/types.h"
#include "wx/menu.h"
#include "wx/stattext.h"
#include "wx/button.h"
#include "wx/bmpbuttn.h"
#include "wx/radiobox.h"
#include "wx/listbox.h"
#include "wx/choice.h"
#include "wx/checkbox.h"
#include "wx/settings.h"
#include "wx/slider.h"
#include "wx/statbox.h"
#if wxUSE_GAUGE
#include "wx/gauge.h"
#endif
#include "wx/textctrl.h"
#include "wx/msgdlg.h"
#include "wx/intl.h"
#endif

#if wxUSE_SCROLLBAR
#include "wx/scrolbar.h"
#endif

#if wxUSE_COMBOBOX
#include "wx/combobox.h"
#endif

#include "wx/validate.h"

#include "wx/log.h"

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "wx/resource.h"
#include "wx/string.h"
#include "wx/wxexpr.h"

#include "wx/settings.h"

#if ((defined(__BORLANDC__) || defined(__SC__)) && defined(__WIN16__))

// Forward (private) declarations
bool wxResourceInterpretResources(wxResourceTable& table, wxExprDatabase& db);
wxItemResource *wxResourceInterpretDialog(wxResourceTable& table, wxExpr *expr, bool isPanel = FALSE);
wxItemResource *wxResourceInterpretControl(wxResourceTable& table, wxExpr *expr);
wxItemResource *wxResourceInterpretMenu(wxResourceTable& table, wxExpr *expr);
wxItemResource *wxResourceInterpretMenuBar(wxResourceTable& table, wxExpr *expr);
wxItemResource *wxResourceInterpretString(wxResourceTable& table, wxExpr *expr);
wxItemResource *wxResourceInterpretBitmap(wxResourceTable& table, wxExpr *expr);
wxItemResource *wxResourceInterpretIcon(wxResourceTable& table, wxExpr *expr);
// Interpret list expression
wxFont wxResourceInterpretFontSpec(wxExpr *expr);

bool wxResourceReadOneResource(FILE *fd, wxExprDatabase& db, bool *eof, wxResourceTable *table = (wxResourceTable *) NULL);
bool wxResourceParseIncludeFile(const wxString& f, wxResourceTable *table = (wxResourceTable *) NULL);

extern wxResourceTable *wxDefaultResourceTable;

extern char *wxResourceBuffer;
extern long wxResourceBufferSize;
extern long wxResourceBufferCount;
extern int wxResourceStringPtr;

/*
 * (Re)allocate buffer for reading in from resource file
 */

bool wxReallocateResourceBuffer()
{
  if (!wxResourceBuffer)
  {
    wxResourceBufferSize = 1000;
    wxResourceBuffer = new char[wxResourceBufferSize];
    return TRUE;
  }
  if (wxResourceBuffer)
  {
    long newSize = wxResourceBufferSize + 1000;
    char *tmp = new char[(int)newSize];
    strncpy(tmp, wxResourceBuffer, (int)wxResourceBufferCount);
    delete[] wxResourceBuffer;
    wxResourceBuffer = tmp;
    wxResourceBufferSize = newSize;
  }
  return TRUE;
}

static bool wxEatWhiteSpace(FILE *fd)
{
  int ch = getc(fd);
  if ((ch != ' ') && (ch != '/') && (ch != ' ') && (ch != 10) && (ch != 13) && (ch != 9))
  {
    ungetc(ch, fd);
    return TRUE;
  }

  // Eat whitespace
  while (ch == ' ' || ch == 10 || ch == 13 || ch == 9)
    ch = getc(fd);
  // Check for comment
  if (ch == '/')
  {
    ch = getc(fd);
    if (ch == '*')
    {
      bool finished = FALSE;
      while (!finished)
      {
        ch = getc(fd);
        if (ch == EOF)
          return FALSE;
        if (ch == '*')
        {
          int newCh = getc(fd);
          if (newCh == '/')
            finished = TRUE;
          else
          {
            ungetc(newCh, fd);
          }
        }
      }
    }
    else // False alarm
      return FALSE;
  }
  else
    ungetc(ch, fd);
  return wxEatWhiteSpace(fd);
}

bool wxGetResourceToken(FILE *fd)
{
  if (!wxResourceBuffer)
    wxReallocateResourceBuffer();
  wxResourceBuffer[0] = 0;
  wxEatWhiteSpace(fd);

  int ch = getc(fd);
  if (ch == '"')
  {
    // Get string
    wxResourceBufferCount = 0;
    ch = getc(fd);
    while (ch != '"')
    {
      int actualCh = ch;
      if (ch == EOF)
      {
        wxResourceBuffer[wxResourceBufferCount] = 0;
        return FALSE;
      }
      // Escaped characters
      else if (ch == '\\')
      {
        int newCh = getc(fd);
        if (newCh == '"')
          actualCh = '"';
        else if (newCh == 10)
          actualCh = 10;
        else
        {
          ungetc(newCh, fd);
        }
      }

      if (wxResourceBufferCount >= wxResourceBufferSize-1)
        wxReallocateResourceBuffer();
      wxResourceBuffer[wxResourceBufferCount] = (char)actualCh;
      wxResourceBufferCount ++;
      ch = getc(fd);
    }
    wxResourceBuffer[wxResourceBufferCount] = 0;
  }
  else
  {
    wxResourceBufferCount = 0;
    // Any other token
    while (ch != ' ' && ch != EOF && ch != ' ' && ch != 13 && ch != 9 && ch != 10)
    {
      if (wxResourceBufferCount >= wxResourceBufferSize-1)
        wxReallocateResourceBuffer();
      wxResourceBuffer[wxResourceBufferCount] = (char)ch;
      wxResourceBufferCount ++;

      ch = getc(fd);
    }
    wxResourceBuffer[wxResourceBufferCount] = 0;
    if (ch == EOF)
      return FALSE;
  }
  return TRUE;
}

/*
 * Files are in form:
  static char *name = "....";
  with possible comments.
 */

bool wxResourceReadOneResource(FILE *fd, wxExprDatabase& db, bool *eof, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  // static or #define
  if (!wxGetResourceToken(fd))
  {
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "#define") == 0)
  {
    wxGetResourceToken(fd);
    char *name = copystring(wxResourceBuffer);
    wxGetResourceToken(fd);
    char *value = copystring(wxResourceBuffer);
    if (isalpha(value[0]))
    {
      int val = (int)atol(value);
      wxResourceAddIdentifier(name, val, table);
    }
    else
    {
      wxLogWarning(_("#define %s must be an integer."), name);
      delete[] name;
      delete[] value;
      return FALSE;
    }
    delete[] name;
    delete[] value;

    return TRUE;
  }
  else if (strcmp(wxResourceBuffer, "#include") == 0)
  {
    wxGetResourceToken(fd);
    char *name = copystring(wxResourceBuffer);
    char *actualName = name;
    if (name[0] == '"')
      actualName = name + 1;
    int len = strlen(name);
    if ((len > 0) && (name[len-1] == '"'))
      name[len-1] = 0;
    if (!wxResourceParseIncludeFile(actualName, table))
    {
      wxLogWarning(_("Could not find resource include file %s."), actualName);
    }
    delete[] name;
    return TRUE;
  }
  else if (strcmp(wxResourceBuffer, "static") != 0)
  {
    char buf[300];
    strcpy(buf, _("Found "));
    strncat(buf, wxResourceBuffer, 30);
    strcat(buf, _(", expected static, #include or #define\nwhilst parsing resource."));
    wxLogWarning(buf);
    return FALSE;
  }

  // char
  if (!wxGetResourceToken(fd))
  {
    wxLogWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "char") != 0)
  {
    wxLogWarning(_("Expected 'char' whilst parsing resource."));
    return FALSE;
  }

  // *name
  if (!wxGetResourceToken(fd))
  {
    wxLogWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }

  if (wxResourceBuffer[0] != '*')
  {
    wxLogWarning(_("Expected '*' whilst parsing resource."));
    return FALSE;
  }
  char nameBuf[100];
  strncpy(nameBuf, wxResourceBuffer+1, 99);

  // =
  if (!wxGetResourceToken(fd))
  {
    wxLogWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "=") != 0)
  {
    wxLogWarning(_("Expected '=' whilst parsing resource."));
    return FALSE;
  }

  // String
  if (!wxGetResourceToken(fd))
  {
    wxLogWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }
  else
  {
    if (!db.ReadPrologFromString(wxResourceBuffer))
    {
      wxLogWarning(_("%s: ill-formed resource file syntax."), nameBuf);
      return FALSE;
    }
  }
  // Semicolon
  if (!wxGetResourceToken(fd))
  {
    *eof = TRUE;
  }
  return TRUE;
}

/*
 * Parses string window style into integer window style
 */

/*
 * Style flag parsing, e.g.
 * "wxSYSTEM_MENU | wxBORDER" -> integer
 */

char* wxResourceParseWord(char*s, int *i)
{
  if (!s)
    return (char*) NULL;

  static char buf[150];
  int len = strlen(s);
  int j = 0;
  int ii = *i;
  while ((ii < len) && (isalpha(s[ii]) || (s[ii] == '_')))
  {
    buf[j] = s[ii];
    j ++;
    ii ++;
  }
  buf[j] = 0;

  // Eat whitespace and conjunction characters
  while ((ii < len) &&
         ((s[ii] == ' ') || (s[ii] == '|') || (s[ii] == ',')))
  {
    ii ++;
  }
  *i = ii;
  if (j == 0)
    return (char*) NULL;
  else
    return buf;
}

struct wxResourceBitListStruct
{
  char *word;
  long bits;
};

static wxResourceBitListStruct wxResourceBitListTable[] =
{
  /* wxListBox */
  { "wxSINGLE", wxLB_SINGLE },
  { "wxMULTIPLE", wxLB_MULTIPLE },
  { "wxEXTENDED", wxLB_EXTENDED },
  { "wxLB_SINGLE", wxLB_SINGLE },
  { "wxLB_MULTIPLE", wxLB_MULTIPLE },
  { "wxLB_EXTENDED", wxLB_EXTENDED },
  { "wxLB_NEEDED_SB", wxLB_NEEDED_SB },
  { "wxLB_ALWAYS_SB", wxLB_ALWAYS_SB },
  { "wxLB_SORT", wxLB_SORT },
  { "wxLB_OWNERDRAW", wxLB_OWNERDRAW },
  { "wxLB_HSCROLL", wxLB_HSCROLL },

  /* wxComboxBox */
  { "wxCB_SIMPLE", wxCB_SIMPLE },
  { "wxCB_DROPDOWN", wxCB_DROPDOWN },
  { "wxCB_READONLY", wxCB_READONLY },
  { "wxCB_SORT", wxCB_SORT },

  /* wxGauge */
  { "wxGA_PROGRESSBAR", wxGA_PROGRESSBAR },
  { "wxGA_HORIZONTAL", wxGA_HORIZONTAL },
  { "wxGA_VERTICAL", wxGA_VERTICAL },

  /* wxTextCtrl */
  { "wxPASSWORD", wxPASSWORD},
  { "wxPROCESS_ENTER", wxPROCESS_ENTER},
  { "wxTE_PASSWORD", wxTE_PASSWORD},
  { "wxTE_READONLY", wxTE_READONLY},
  { "wxTE_PROCESS_ENTER", wxTE_PROCESS_ENTER},
  { "wxTE_MULTILINE", wxTE_MULTILINE},

  /* wxRadioBox/wxRadioButton */
  { "wxRB_GROUP", wxRB_GROUP },
  { "wxRA_SPECIFY_COLS", wxRA_SPECIFY_COLS },
  { "wxRA_SPECIFY_ROWS", wxRA_SPECIFY_ROWS },
  { "wxRA_HORIZONTAL", wxRA_HORIZONTAL },
  { "wxRA_VERTICAL", wxRA_VERTICAL },

  /* wxSlider */
  { "wxSL_HORIZONTAL", wxSL_HORIZONTAL },
  { "wxSL_VERTICAL", wxSL_VERTICAL },
  { "wxSL_AUTOTICKS", wxSL_AUTOTICKS },
  { "wxSL_LABELS", wxSL_LABELS },
  { "wxSL_LEFT", wxSL_LEFT },
  { "wxSL_TOP", wxSL_TOP },
  { "wxSL_RIGHT", wxSL_RIGHT },
  { "wxSL_BOTTOM", wxSL_BOTTOM },
  { "wxSL_BOTH", wxSL_BOTH },
  { "wxSL_SELRANGE", wxSL_SELRANGE },

  /* wxScrollBar */
  { "wxSB_HORIZONTAL", wxSB_HORIZONTAL },
  { "wxSB_VERTICAL", wxSB_VERTICAL },

  /* wxButton */
  { "wxBU_AUTODRAW", wxBU_AUTODRAW },
  { "wxBU_NOAUTODRAW", wxBU_NOAUTODRAW },

  /* wxTreeCtrl */
  { "wxTR_HAS_BUTTONS", wxTR_HAS_BUTTONS },
  { "wxTR_EDIT_LABELS", wxTR_EDIT_LABELS },
  { "wxTR_LINES_AT_ROOT", wxTR_LINES_AT_ROOT },

  /* wxListCtrl */
  { "wxLC_ICON", wxLC_ICON },
  { "wxLC_SMALL_ICON", wxLC_SMALL_ICON },
  { "wxLC_LIST", wxLC_LIST },
  { "wxLC_REPORT", wxLC_REPORT },
  { "wxLC_ALIGN_TOP", wxLC_ALIGN_TOP },
  { "wxLC_ALIGN_LEFT", wxLC_ALIGN_LEFT },
  { "wxLC_AUTOARRANGE", wxLC_AUTOARRANGE },
  { "wxLC_USER_TEXT", wxLC_USER_TEXT },
  { "wxLC_EDIT_LABELS", wxLC_EDIT_LABELS },
  { "wxLC_NO_HEADER", wxLC_NO_HEADER },
  { "wxLC_NO_SORT_HEADER", wxLC_NO_SORT_HEADER },
  { "wxLC_SINGLE_SEL", wxLC_SINGLE_SEL },
  { "wxLC_SORT_ASCENDING", wxLC_SORT_ASCENDING },
  { "wxLC_SORT_DESCENDING", wxLC_SORT_DESCENDING },

  /* wxSpinButton */
  { "wxSP_VERTICAL", wxSP_VERTICAL},
  { "wxSP_HORIZONTAL", wxSP_HORIZONTAL},
  { "wxSP_ARROW_KEYS", wxSP_ARROW_KEYS},
  { "wxSP_WRAP", wxSP_WRAP},

  /* wxSplitterWnd */
  { "wxSP_NOBORDER", wxSP_NOBORDER},
  { "wxSP_3D", wxSP_3D},
  { "wxSP_BORDER", wxSP_BORDER},

  /* wxTabCtrl */
  { "wxTC_MULTILINE", wxTC_MULTILINE},
  { "wxTC_RIGHTJUSTIFY", wxTC_RIGHTJUSTIFY},
  { "wxTC_FIXEDWIDTH", wxTC_FIXEDWIDTH},
  { "wxTC_OWNERDRAW", wxTC_OWNERDRAW},

  /* wxStatusBar95 */
  { "wxST_SIZEGRIP", wxST_SIZEGRIP},

  /* wxControl */
  { "wxFIXED_LENGTH", wxFIXED_LENGTH},
  { "wxALIGN_LEFT", wxALIGN_LEFT},
  { "wxALIGN_CENTER", wxALIGN_CENTER},
  { "wxALIGN_CENTRE", wxALIGN_CENTRE},
  { "wxALIGN_RIGHT", wxALIGN_RIGHT},
  { "wxCOLOURED", wxCOLOURED},

  /* wxToolBar */
  { "wxTB_3DBUTTONS", wxTB_3DBUTTONS},
  { "wxTB_HORIZONTAL", wxTB_HORIZONTAL},
  { "wxTB_VERTICAL", wxTB_VERTICAL},
  { "wxTB_FLAT", wxTB_FLAT},

  /* Generic */
  { "wxVSCROLL", wxVSCROLL },
  { "wxHSCROLL", wxHSCROLL },
  { "wxCAPTION", wxCAPTION },
  { "wxSTAY_ON_TOP", wxSTAY_ON_TOP},
  { "wxICONIZE", wxICONIZE},
  { "wxMINIMIZE", wxICONIZE},
  { "wxMAXIMIZE", wxMAXIMIZE},
  { "wxSDI", 0},
  { "wxMDI_PARENT", 0},
  { "wxMDI_CHILD", 0},
  { "wxTHICK_FRAME", wxTHICK_FRAME},
  { "wxRESIZE_BORDER", wxRESIZE_BORDER},
  { "wxSYSTEM_MENU", wxSYSTEM_MENU},
  { "wxMINIMIZE_BOX", wxMINIMIZE_BOX},
  { "wxMAXIMIZE_BOX", wxMAXIMIZE_BOX},
  { "wxRESIZE_BOX", wxRESIZE_BOX},
  { "wxDEFAULT_FRAME_STYLE", wxDEFAULT_FRAME_STYLE},
  { "wxDEFAULT_FRAME", wxDEFAULT_FRAME_STYLE},
  { "wxDEFAULT_DIALOG_STYLE", wxDEFAULT_DIALOG_STYLE},
  { "wxBORDER", wxBORDER},
  { "wxRETAINED", wxRETAINED},
  { "wxNATIVE_IMPL", 0},
  { "wxEXTENDED_IMPL", 0},
  { "wxBACKINGSTORE", wxBACKINGSTORE},
//  { "wxFLAT", wxFLAT},
//  { "wxMOTIF_RESIZE", wxMOTIF_RESIZE},
  { "wxFIXED_LENGTH", 0},
  { "wxDOUBLE_BORDER", wxDOUBLE_BORDER},
  { "wxSUNKEN_BORDER", wxSUNKEN_BORDER},
  { "wxRAISED_BORDER", wxRAISED_BORDER},
  { "wxSIMPLE_BORDER", wxSIMPLE_BORDER},
  { "wxSTATIC_BORDER", wxSTATIC_BORDER},
  { "wxTRANSPARENT_WINDOW", wxTRANSPARENT_WINDOW},
  { "wxNO_BORDER", wxNO_BORDER},
  { "wxCLIP_CHILDREN", wxCLIP_CHILDREN},

  { "wxTINY_CAPTION_HORIZ", wxTINY_CAPTION_HORIZ},
  { "wxTINY_CAPTION_VERT", wxTINY_CAPTION_VERT},

  // Text font families
  { "wxDEFAULT", wxDEFAULT},
  { "wxDECORATIVE", wxDECORATIVE},
  { "wxROMAN", wxROMAN},
  { "wxSCRIPT", wxSCRIPT},
  { "wxSWISS", wxSWISS},
  { "wxMODERN", wxMODERN},
  { "wxTELETYPE", wxTELETYPE},
  { "wxVARIABLE", wxVARIABLE},
  { "wxFIXED", wxFIXED},
  { "wxNORMAL", wxNORMAL},
  { "wxLIGHT", wxLIGHT},
  { "wxBOLD", wxBOLD},
  { "wxITALIC", wxITALIC},
  { "wxSLANT", wxSLANT},
  { "wxSOLID", wxSOLID},
  { "wxDOT", wxDOT},
  { "wxLONG_DASH", wxLONG_DASH},
  { "wxSHORT_DASH", wxSHORT_DASH},
  { "wxDOT_DASH", wxDOT_DASH},
  { "wxUSER_DASH", wxUSER_DASH},
  { "wxTRANSPARENT", wxTRANSPARENT},
  { "wxSTIPPLE", wxSTIPPLE},
  { "wxBDIAGONAL_HATCH", wxBDIAGONAL_HATCH},
  { "wxCROSSDIAG_HATCH", wxCROSSDIAG_HATCH},
  { "wxFDIAGONAL_HATCH", wxFDIAGONAL_HATCH},
  { "wxCROSS_HATCH", wxCROSS_HATCH},
  { "wxHORIZONTAL_HATCH", wxHORIZONTAL_HATCH},
  { "wxVERTICAL_HATCH", wxVERTICAL_HATCH},
  { "wxJOIN_BEVEL", wxJOIN_BEVEL},
  { "wxJOIN_MITER", wxJOIN_MITER},
  { "wxJOIN_ROUND", wxJOIN_ROUND},
  { "wxCAP_ROUND", wxCAP_ROUND},
  { "wxCAP_PROJECTING", wxCAP_PROJECTING},
  { "wxCAP_BUTT", wxCAP_BUTT},

  // Logical ops
  { "wxCLEAR", wxCLEAR},
  { "wxXOR", wxXOR},
  { "wxINVERT", wxINVERT},
  { "wxOR_REVERSE", wxOR_REVERSE},
  { "wxAND_REVERSE", wxAND_REVERSE},
  { "wxCOPY", wxCOPY},
  { "wxAND", wxAND},
  { "wxAND_INVERT", wxAND_INVERT},
  { "wxNO_OP", wxNO_OP},
  { "wxNOR", wxNOR},
  { "wxEQUIV", wxEQUIV},
  { "wxSRC_INVERT", wxSRC_INVERT},
  { "wxOR_INVERT", wxOR_INVERT},
  { "wxNAND", wxNAND},
  { "wxOR", wxOR},
  { "wxSET", wxSET},

  { "wxFLOOD_SURFACE", wxFLOOD_SURFACE},
  { "wxFLOOD_BORDER", wxFLOOD_BORDER},
  { "wxODDEVEN_RULE", wxODDEVEN_RULE},
  { "wxWINDING_RULE", wxWINDING_RULE},
  { "wxHORIZONTAL", wxHORIZONTAL},
  { "wxVERTICAL", wxVERTICAL},
  { "wxBOTH", wxBOTH},
  { "wxCENTER_FRAME", wxCENTER_FRAME},
  { "wxOK", wxOK},
  { "wxYES_NO", wxYES_NO},
  { "wxCANCEL", wxCANCEL},
  { "wxYES", wxYES},
  { "wxNO", wxNO},
  { "wxICON_EXCLAMATION", wxICON_EXCLAMATION},
  { "wxICON_HAND", wxICON_HAND},
  { "wxICON_QUESTION", wxICON_QUESTION},
  { "wxICON_INFORMATION", wxICON_INFORMATION},
  { "wxICON_STOP", wxICON_STOP},
  { "wxICON_ASTERISK", wxICON_ASTERISK},
  { "wxICON_MASK", wxICON_MASK},
  { "wxCENTRE", wxCENTRE},
  { "wxCENTER", wxCENTRE},
  { "wxUSER_COLOURS", wxUSER_COLOURS},
  { "wxVERTICAL_LABEL", 0},
  { "wxHORIZONTAL_LABEL", 0},

  // Bitmap types (not strictly styles)
  { "wxBITMAP_TYPE_XPM", wxBITMAP_TYPE_XPM},
  { "wxBITMAP_TYPE_XBM", wxBITMAP_TYPE_XBM},
  { "wxBITMAP_TYPE_BMP", wxBITMAP_TYPE_BMP},
  { "wxBITMAP_TYPE_RESOURCE", wxBITMAP_TYPE_BMP_RESOURCE},
  { "wxBITMAP_TYPE_BMP_RESOURCE", wxBITMAP_TYPE_BMP_RESOURCE},
  { "wxBITMAP_TYPE_GIF", wxBITMAP_TYPE_GIF},
  { "wxBITMAP_TYPE_TIF", wxBITMAP_TYPE_TIF},
  { "wxBITMAP_TYPE_ICO", wxBITMAP_TYPE_ICO},
  { "wxBITMAP_TYPE_ICO_RESOURCE", wxBITMAP_TYPE_ICO_RESOURCE},
  { "wxBITMAP_TYPE_CUR", wxBITMAP_TYPE_CUR},
  { "wxBITMAP_TYPE_CUR_RESOURCE", wxBITMAP_TYPE_CUR_RESOURCE},
  { "wxBITMAP_TYPE_XBM_DATA", wxBITMAP_TYPE_XBM_DATA},
  { "wxBITMAP_TYPE_XPM_DATA", wxBITMAP_TYPE_XPM_DATA},
  { "wxBITMAP_TYPE_ANY", wxBITMAP_TYPE_ANY}
};

static int wxResourceBitListCount = (sizeof(wxResourceBitListTable)/sizeof(wxResourceBitListStruct));

long wxParseWindowStyle(const wxString& bitListString)
{
  int i = 0;
  char *word;
  long bitList = 0;
  while ((word = wxResourceParseWord((char*) (const char*) bitListString, &i)))
  {
    bool found = FALSE;
    int j;
    for (j = 0; j < wxResourceBitListCount; j++)
      if (strcmp(wxResourceBitListTable[j].word, word) == 0)
      {
        bitList |= wxResourceBitListTable[j].bits;
        found = TRUE;
        break;
      }
    if (!found)
    {
      wxLogWarning(_("Unrecognized style %s whilst parsing resource."), word);
      return 0;
    }
  }
  return bitList;
}

/*
 * Load a bitmap from a wxWindows resource, choosing an optimum
 * depth and appropriate type.
 */

wxBitmap wxResourceCreateBitmap(const wxString& resource, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  wxItemResource *item = table->FindResource(resource);
  if (item)
  {
    if ((item->GetType() == "") || (item->GetType() != "wxBitmap"))
    {
      wxLogWarning(_("%s not a bitmap resource specification."), (const char*) resource);
      return wxNullBitmap;
    }
    int thisDepth = wxDisplayDepth();
    long thisNoColours = (long)pow(2.0, (double)thisDepth);

    wxItemResource *optResource = (wxItemResource *) NULL;

    // Try to find optimum bitmap for this platform/colour depth
    wxNode *node = item->GetChildren().First();
    while (node)
    {
      wxItemResource *child = (wxItemResource *)node->Data();
      int platform = (int)child->GetValue2();
      int noColours = (int)child->GetValue3();
/*
      char *name = child->GetName();
      int bitmapType = (int)child->GetValue1();
      int xRes = child->GetWidth();
      int yRes = child->GetHeight();
*/

      switch (platform)
      {
        case RESOURCE_PLATFORM_ANY:
        {
          if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
            optResource = child;
          else
          {
            // Maximise the number of colours.
            // If noColours is zero (unspecified), then assume this
            // is the right one.
            if ((noColours == 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
              optResource = child;
          }
          break;
        }
#ifdef __WXMSW__
        case RESOURCE_PLATFORM_WINDOWS:
        {
          if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
            optResource = child;
          else
          {
            // Maximise the number of colours
            if ((noColours > 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
              optResource = child;
          }
          break;
        }
#endif
#ifdef __WXGTK__
        case RESOURCE_PLATFORM_X:
        {
          if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
            optResource = child;
          else
          {
            // Maximise the number of colours
            if ((noColours == 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
              optResource = child;
          }
          break;
        }
#endif
#ifdef wx_max
        case RESOURCE_PLATFORM_MAC:
        {
          if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
            optResource = child;
          else
          {
            // Maximise the number of colours
            if ((noColours == 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
              optResource = child;
          }
          break;
        }
#endif
        default:
          break;
      }
      node = node->Next();
    }
    // If no matching resource, fail.
    if (!optResource)
      return wxNullBitmap;

    wxString name = optResource->GetName();
    int bitmapType = (int)optResource->GetValue1();
    switch (bitmapType)
    {
      case wxBITMAP_TYPE_XBM_DATA:
      {
#ifdef __WXGTK__
        wxItemResource *item = table->FindResource(name);
        if (!item)
        {
          wxLogWarning(_("Failed to find XBM resource %s.\n"
                         "Forgot to use wxResourceLoadBitmapData?"), (const char*) name);
          return wxNullBitmap;
        }
        return wxBitmap(item->GetValue1(), (int)item->GetValue2(), (int)item->GetValue3()) ;
#else
        wxLogWarning(_("No XBM facility available!"));
#endif
        break;
      }
      case wxBITMAP_TYPE_XPM_DATA:
      {
#if (defined(__WXGTK__)) || (defined(__WXMSW__) && wxUSE_XPM_IN_MSW)
        wxItemResource *item = table->FindResource(name);
        if (!item)
        {
          wxLogWarning(_("Failed to find XPM resource %s.\n"
                         "Forgot to use wxResourceLoadBitmapData?"), (const char*) name);
          return wxNullBitmap;
        }
        return wxBitmap(item->GetValue1());
#else
        wxLogWarning(_("No XPM facility available!"));
#endif
        break;
      }
      default:
      {
        return wxBitmap(name, bitmapType);
        break;
      }
    }
    return wxNullBitmap;
  }
  else
  {
    wxLogWarning(_("Bitmap resource specification %s not found."), (const char*) resource);
    return wxNullBitmap;
  }
}

/*
 * Load an icon from a wxWindows resource, choosing an optimum
 * depth and appropriate type.
 */

wxIcon wxResourceCreateIcon(const wxString& resource, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  wxItemResource *item = table->FindResource(resource);
  if (item)
  {
    if ((item->GetType() == "") || (item->GetType() != "wxIcon"))
    {
      wxLogWarning(_("%s not an icon resource specification."), (const char*) resource);
      return wxNullIcon;
    }
    int thisDepth = wxDisplayDepth();
    long thisNoColours = (long)pow(2.0, (double)thisDepth);

    wxItemResource *optResource = (wxItemResource *) NULL;

    // Try to find optimum icon for this platform/colour depth
    wxNode *node = item->GetChildren().First();
    while (node)
    {
      wxItemResource *child = (wxItemResource *)node->Data();
      int platform = (int)child->GetValue2();
      int noColours = (int)child->GetValue3();
/*
      char *name = child->GetName();
      int bitmapType = (int)child->GetValue1();
      int xRes = child->GetWidth();
      int yRes = child->GetHeight();
*/

      switch (platform)
      {
        case RESOURCE_PLATFORM_ANY:
        {
          if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
            optResource = child;
          else
          {
            // Maximise the number of colours.
            // If noColours is zero (unspecified), then assume this
            // is the right one.
            if ((noColours == 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
              optResource = child;
          }
          break;
        }
#ifdef __WXMSW__
        case RESOURCE_PLATFORM_WINDOWS:
        {
          if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
            optResource = child;
          else
          {
            // Maximise the number of colours
            if ((noColours > 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
              optResource = child;
          }
          break;
        }
#endif
#ifdef __WXGTK__
        case RESOURCE_PLATFORM_X:
        {
          if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
            optResource = child;
          else
          {
            // Maximise the number of colours
            if ((noColours == 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
              optResource = child;
          }
          break;
        }
#endif
#ifdef wx_max
        case RESOURCE_PLATFORM_MAC:
        {
          if (!optResource && ((noColours == 0) || (noColours <= thisNoColours)))
            optResource = child;
          else
          {
            // Maximise the number of colours
            if ((noColours == 0) || ((noColours <= thisNoColours) && (noColours > optResource->GetValue3())))
              optResource = child;
          }
          break;
        }
#endif
        default:
          break;
      }
      node = node->Next();
    }
    // If no matching resource, fail.
    if (!optResource)
      return wxNullIcon;

    wxString name = optResource->GetName();
    int bitmapType = (int)optResource->GetValue1();
    switch (bitmapType)
    {
      case wxBITMAP_TYPE_XBM_DATA:
      {
#ifdef __WXGTK__
        wxItemResource *item = table->FindResource(name);
        if (!item)
        {
          wxLogWarning(_("Failed to find XBM resource %s.\n"
                         "Forgot to use wxResourceLoadIconData?"), (const char*) name);
          return wxNullIcon;
        }
        return wxIcon((const char **)item->GetValue1(), (int)item->GetValue2(), (int)item->GetValue3());
#else
        wxLogWarning(_("No XBM facility available!"));
#endif
        break;
      }
      case wxBITMAP_TYPE_XPM_DATA:
      {
      // *** XPM ICON NOT YET IMPLEMENTED IN WXWINDOWS ***
/*
#if (defined(__WXGTK__)) || (defined(__WXMSW__) && wxUSE_XPM_IN_MSW)
        wxItemResource *item = table->FindResource(name);
        if (!item)
        {
          char buf[400];
          sprintf(buf, _("Failed to find XPM resource %s.\nForgot to use wxResourceLoadIconData?"), name);
          wxLogWarning(buf);
          return NULL;
        }
        return wxIcon((char **)item->GetValue1());
#else
        wxLogWarning(_("No XPM facility available!"));
#endif
*/
        wxLogWarning(_("No XPM icon facility available!"));
        break;
      }
      default:
      {
#ifdef __WXGTK__
        wxLogWarning(_("Icon resource specification %s not found."), (const char*) resource);
#else
        return wxIcon(name, bitmapType);
#endif
        break;
      }
    }
    return wxNullIcon;
  }
  else
  {
    wxLogWarning(_("Icon resource specification %s not found."), (const char*) resource);
    return wxNullIcon;
  }
}

wxMenu *wxResourceCreateMenu(wxItemResource *item)
{
  wxMenu *menu = new wxMenu;
  wxNode *node = item->GetChildren().First();
  while (node)
  {
    wxItemResource *child = (wxItemResource *)node->Data();
    if ((child->GetType() != "") && (child->GetType() == "wxMenuSeparator"))
      menu->AppendSeparator();
    else if (child->GetChildren().Number() > 0)
    {
      wxMenu *subMenu = wxResourceCreateMenu(child);
      if (subMenu)
        menu->Append((int)child->GetValue1(), child->GetTitle(), subMenu, child->GetValue4());
    }
    else
    {
      menu->Append((int)child->GetValue1(), child->GetTitle(), child->GetValue4(), (child->GetValue2() != 0));
    }
    node = node->Next();
  }
  return menu;
}

wxMenuBar *wxResourceCreateMenuBar(const wxString& resource, wxResourceTable *table, wxMenuBar *menuBar)
{
  if (!table)
    table = wxDefaultResourceTable;

  wxItemResource *menuResource = table->FindResource(resource);
  if (menuResource && (menuResource->GetType() != "") && (menuResource->GetType() == "wxMenu"))
  {
    if (!menuBar)
      menuBar = new wxMenuBar;
    wxNode *node = menuResource->GetChildren().First();
    while (node)
    {
      wxItemResource *child = (wxItemResource *)node->Data();
      wxMenu *menu = wxResourceCreateMenu(child);
      if (menu)
        menuBar->Append(menu, child->GetTitle());
      node = node->Next();
    }
    return menuBar;
  }
  return (wxMenuBar *) NULL;
}

wxMenu *wxResourceCreateMenu(const wxString& resource, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  wxItemResource *menuResource = table->FindResource(resource);
  if (menuResource && (menuResource->GetType() != "") && (menuResource->GetType() == "wxMenu"))
//  if (menuResource && (menuResource->GetType() == wxTYPE_MENU))
    return wxResourceCreateMenu(menuResource);
  return (wxMenu *) NULL;
}

// Global equivalents (so don't have to refer to default table explicitly)
bool wxResourceParseData(const wxString& resource, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  return table->ParseResourceData(resource);
}

bool wxResourceParseFile(const wxString& filename, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  return table->ParseResourceFile(filename);
}

// Register XBM/XPM data
bool wxResourceRegisterBitmapData(const wxString& name, char bits[], int width, int height, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  return table->RegisterResourceBitmapData(name, bits, width, height);
}

bool wxResourceRegisterBitmapData(const wxString& name, char **data, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  return table->RegisterResourceBitmapData(name, data);
}

void wxResourceClear(wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  table->ClearTable();
}

/*
 * Identifiers
 */

bool wxResourceAddIdentifier(const wxString& name, int value, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  table->identifiers.Put(name, (wxObject *)value);
  return TRUE;
}

int wxResourceGetIdentifier(const wxString& name, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  return (int)table->identifiers.Get(name);
}

/*
 * Parse #include file for #defines (only)
 */

bool wxResourceParseIncludeFile(const wxString& f, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  FILE *fd = fopen(f, "r");
  if (!fd)
  {
    return FALSE;
  }
  while (wxGetResourceToken(fd))
  {
    if (strcmp(wxResourceBuffer, "#define") == 0)
    {
      wxGetResourceToken(fd);
      char *name = copystring(wxResourceBuffer);
      wxGetResourceToken(fd);
      char *value = copystring(wxResourceBuffer);
      if (isdigit(value[0]))
      {
        int val = (int)atol(value);
        wxResourceAddIdentifier(name, val, table);
      }
      delete[] name;
      delete[] value;
    }
  }
  fclose(fd);
  return TRUE;
}

/*
 * Reading strings as if they were .wxr files
 */

static int getc_string(char *s)
{
  int ch = s[wxResourceStringPtr];
  if (ch == 0)
    return EOF;
  else
  {
    wxResourceStringPtr ++;
    return ch;
  }
}

static int ungetc_string()
{
  wxResourceStringPtr --;
  return 0;
}

bool wxEatWhiteSpaceString(char *s)
{
  int ch = getc_string(s);
  if (ch == EOF)
    return TRUE;

  if ((ch != ' ') && (ch != '/') && (ch != ' ') && (ch != 10) && (ch != 13) && (ch != 9))
  {
    ungetc_string();
    return TRUE;
  }

  // Eat whitespace
  while (ch == ' ' || ch == 10 || ch == 13 || ch == 9)
    ch = getc_string(s);
  // Check for comment
  if (ch == '/')
  {
    ch = getc_string(s);
    if (ch == '*')
    {
      bool finished = FALSE;
      while (!finished)
      {
        ch = getc_string(s);
        if (ch == EOF)
          return FALSE;
        if (ch == '*')
        {
          int newCh = getc_string(s);
          if (newCh == '/')
            finished = TRUE;
          else
          {
            ungetc_string();
          }
        }
      }
    }
    else // False alarm
      return FALSE;
  }
  else if (ch != EOF)
    ungetc_string();
  return wxEatWhiteSpaceString(s);
}

bool wxGetResourceTokenString(char *s)
{
  if (!wxResourceBuffer)
    wxReallocateResourceBuffer();
  wxResourceBuffer[0] = 0;
  wxEatWhiteSpaceString(s);

  int ch = getc_string(s);
  if (ch == '"')
  {
    // Get string
    wxResourceBufferCount = 0;
    ch = getc_string(s);
    while (ch != '"')
    {
      int actualCh = ch;
      if (ch == EOF)
      {
        wxResourceBuffer[wxResourceBufferCount] = 0;
        return FALSE;
      }
      // Escaped characters
      else if (ch == '\\')
      {
        int newCh = getc_string(s);
        if (newCh == '"')
          actualCh = '"';
        else if (newCh == 10)
          actualCh = 10;
        else
        {
          ungetc_string();
        }
      }

      if (wxResourceBufferCount >= wxResourceBufferSize-1)
        wxReallocateResourceBuffer();
      wxResourceBuffer[wxResourceBufferCount] = (char)actualCh;
      wxResourceBufferCount ++;
      ch = getc_string(s);
    }
    wxResourceBuffer[wxResourceBufferCount] = 0;
  }
  else
  {
    wxResourceBufferCount = 0;
    // Any other token
    while (ch != ' ' && ch != EOF && ch != ' ' && ch != 13 && ch != 9 && ch != 10)
    {
      if (wxResourceBufferCount >= wxResourceBufferSize-1)
        wxReallocateResourceBuffer();
      wxResourceBuffer[wxResourceBufferCount] = (char)ch;
      wxResourceBufferCount ++;

      ch = getc_string(s);
    }
    wxResourceBuffer[wxResourceBufferCount] = 0;
    if (ch == EOF)
      return FALSE;
  }
  return TRUE;
}

/*
 * Files are in form:
  static char *name = "....";
  with possible comments.
 */

bool wxResourceReadOneResourceString(char *s, wxExprDatabase& db, bool *eof, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  // static or #define
  if (!wxGetResourceTokenString(s))
  {
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "#define") == 0)
  {
    wxGetResourceTokenString(s);
    char *name = copystring(wxResourceBuffer);
    wxGetResourceTokenString(s);
    char *value = copystring(wxResourceBuffer);
    if (isalpha(value[0]))
    {
      int val = (int)atol(value);
      wxResourceAddIdentifier(name, val, table);
    }
    else
    {
      wxLogWarning(_("#define %s must be an integer."), name);
      delete[] name;
      delete[] value;
      return FALSE;
    }
    delete[] name;
    delete[] value;

    return TRUE;
  }
/*
  else if (strcmp(wxResourceBuffer, "#include") == 0)
  {
    wxGetResourceTokenString(s);
    char *name = copystring(wxResourceBuffer);
    char *actualName = name;
    if (name[0] == '"')
      actualName = name + 1;
    int len = strlen(name);
    if ((len > 0) && (name[len-1] == '"'))
      name[len-1] = 0;
    if (!wxResourceParseIncludeFile(actualName, table))
    {
      char buf[400];
      sprintf(buf, _("Could not find resource include file %s."), actualName);
      wxLogWarning(buf);
    }
    delete[] name;
    return TRUE;
  }
*/
  else if (strcmp(wxResourceBuffer, "static") != 0)
  {
    char buf[300];
    strcpy(buf, _("Found "));
    strncat(buf, wxResourceBuffer, 30);
    strcat(buf, _(", expected static, #include or #define\nwhilst parsing resource."));
    wxLogWarning(buf);
    return FALSE;
  }

  // char
  if (!wxGetResourceTokenString(s))
  {
    wxLogWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "char") != 0)
  {
    wxLogWarning(_("Expected 'char' whilst parsing resource."));
    return FALSE;
  }

  // *name
  if (!wxGetResourceTokenString(s))
  {
    wxLogWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }

  if (wxResourceBuffer[0] != '*')
  {
    wxLogWarning(_("Expected '*' whilst parsing resource."));
    return FALSE;
  }
  char nameBuf[100];
  strncpy(nameBuf, wxResourceBuffer+1, 99);

  // =
  if (!wxGetResourceTokenString(s))
  {
    wxLogWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }

  if (strcmp(wxResourceBuffer, "=") != 0)
  {
    wxLogWarning(_("Expected '=' whilst parsing resource."));
    return FALSE;
  }

  // String
  if (!wxGetResourceTokenString(s))
  {
    wxLogWarning(_("Unexpected end of file whilst parsing resource."));
    *eof = TRUE;
    return FALSE;
  }
  else
  {
    if (!db.ReadPrologFromString(wxResourceBuffer))
    {
      wxLogWarning(_("%s: ill-formed resource file syntax."), nameBuf);
      return FALSE;
    }
  }
  // Semicolon
  if (!wxGetResourceTokenString(s))
  {
    *eof = TRUE;
  }
  return TRUE;
}

bool wxResourceParseString(char *s, wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  if (!s)
    return FALSE;

  // Turn backslashes into spaces
  if (s)
  {
    int len = strlen(s);
    int i;
    for (i = 0; i < len; i++)
      if (s[i] == 92 && s[i+1] == 13)
      {
        s[i] = ' ';
        s[i+1] = ' ';
      }
  }

  wxExprDatabase db;
  wxResourceStringPtr = 0;

  bool eof = FALSE;
  while (wxResourceReadOneResourceString(s, db, &eof, table) && !eof)
  {
    // Loop
  }
  return wxResourceInterpretResources(*table, db);
}

/*
 * resource loading facility
 */

bool wxWindow::LoadFromResource(wxWindow *parent, const wxString& resourceName, const wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;

  wxItemResource *resource = table->FindResource((const char *)resourceName);
//  if (!resource || (resource->GetType() != wxTYPE_DIALOG_BOX))
  if (!resource || (resource->GetType() == "") ||
    ! ((strcmp(resource->GetType(), "wxDialog") == 0) || (strcmp(resource->GetType(), "wxPanel") == 0)))
    return FALSE;

  wxString title(resource->GetTitle());
  long theWindowStyle = resource->GetStyle();
  bool isModal = (resource->GetValue1() != 0);
  int x = resource->GetX();
  int y = resource->GetY();
  int width = resource->GetWidth();
  int height = resource->GetHeight();
  wxString name = resource->GetName();

  if (IsKindOf(CLASSINFO(wxDialog)))
  {
    wxDialog *dialogBox = (wxDialog *)this;
    long modalStyle = isModal ? wxDIALOG_MODAL : 0;
    if (!dialogBox->Create(parent, -1, title, wxPoint(x, y), wxSize(width, height), theWindowStyle|modalStyle, name))
      return FALSE;

    // Only reset the client size if we know we're not going to do it again below.
    if ((resource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS) == 0)
      dialogBox->SetClientSize(width, height);
  }
  else if (IsKindOf(CLASSINFO(wxPanel)))
  {
    wxPanel* panel = (wxPanel *)this;
    if (!panel->Create(parent, -1, wxPoint(x, y), wxSize(width, height), theWindowStyle, name))
      return FALSE;
  }
  else
  {
    if (!this->Create(parent, -1, wxPoint(x, y), wxSize(width, height), theWindowStyle, name))
      return FALSE;
  }

  if ((resource->GetResourceStyle() & wxRESOURCE_USE_DEFAULTS) != 0)
  {
    // No need to do this since it's done in wxPanel or wxDialog constructor.
    // SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));
  }
  else
  {
    if (resource->GetFont().Ok())
      SetFont(resource->GetFont());
    if (resource->GetBackgroundColour().Ok())
      SetBackgroundColour(resource->GetBackgroundColour());
  }

  // Should have some kind of font at this point
  if (!GetFont().Ok())
      SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));
  if (!GetBackgroundColour().Ok())
      SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));

  // Only when we've created the window and set the font can we set the correct size,
  // if based on dialog units.
  if ((resource->GetResourceStyle() & wxRESOURCE_DIALOG_UNITS) != 0)
  {
    wxSize sz = ConvertDialogToPixels(wxSize(width, height));
    SetClientSize(sz.x, sz.y);

    wxPoint pt = ConvertDialogToPixels(wxPoint(x, y));
    Move(pt.x, pt.y);
  }

  // Now create children
  wxNode *node = resource->GetChildren().First();
  while (node)
  {
    wxItemResource *childResource = (wxItemResource *)node->Data();

    (void) CreateItem(childResource, resource, table);

    node = node->Next();
  }
  return TRUE;
}

wxControl *wxWindow::CreateItem(const wxItemResource *resource, const wxItemResource* parentResource, const wxResourceTable *table)
{
  if (!table)
    table = wxDefaultResourceTable;
  return table->CreateItem((wxWindow *)this, resource, parentResource);
}

#ifdef __VISUALC__
    #pragma warning(default:4706)   // assignment within conditional expression
#endif // VC++

#endif
  // BC++/Win16 only
#endif // wxUSE_WX_RESOURCES

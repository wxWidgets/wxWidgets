/////////////////////////////////////////////////////////////////////////////
// Name:        mfutils.h
// Purpose:     Metafile utilities: reading a placeable metafile independently
//              of Windows.
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _MFUTILS_H_
#define _MFUTILS_H_

#ifdef __GNUG__
#pragma interface "mfutils.h"
#endif

#include <wx/metafile.h>

#ifndef __WXMSW__
#define GetRValue(rgb)	    ((unsigned char)(rgb))
#define GetGValue(rgb)      ((unsigned char)(((int)(rgb)) >> 8))
#define GetBValue(rgb)	    ((unsigned char)((rgb)>>16))
#endif

/* Metafile Functions */
/* Win32s/Borland need these macros, although META_SETBKCOLOR is defined */
#if !defined(META_SETBKCOLOR) || defined(WIN32)

#define META_SETBKCOLOR		     0x0201
#define META_SETBKMODE		     0x0102
#define META_SETMAPMODE		     0x0103
#define META_SETROP2		     0x0104
#define META_SETRELABS		     0x0105
#define META_SETPOLYFILLMODE	     0x0106
#define META_SETSTRETCHBLTMODE	     0x0107
#define META_SETTEXTCHAREXTRA	     0x0108
#define META_SETTEXTCOLOR	     0x0209
#define META_SETTEXTJUSTIFICATION    0x020A
#define META_SETWINDOWORG	     0x020B
#define META_SETWINDOWEXT	     0x020C
#define META_SETVIEWPORTORG	     0x020D
#define META_SETVIEWPORTEXT	     0x020E
#define META_OFFSETWINDOWORG	     0x020F
#define META_SCALEWINDOWEXT	     0x0410
#define META_OFFSETVIEWPORTORG	     0x0211
#define META_SCALEVIEWPORTEXT	     0x0412
#define META_LINETO		     0x0213
#define META_MOVETO		     0x0214
#define META_EXCLUDECLIPRECT	     0x0415
#define META_INTERSECTCLIPRECT	     0x0416
#define META_ARC		     0x0817
#define META_ELLIPSE		     0x0418
#define META_FLOODFILL		     0x0419
#define META_PIE		     0x081A
#define META_RECTANGLE		     0x041B
#define META_ROUNDRECT		     0x061C
#define META_PATBLT		     0x061D
#define META_SAVEDC		     0x001E
#define META_SETPIXEL		     0x041F
#define META_OFFSETCLIPRGN	     0x0220
#define META_TEXTOUT		     0x0521
#define META_BITBLT		     0x0922
#define META_STRETCHBLT		     0x0B23
#define META_POLYGON		     0x0324
#define META_POLYLINE		     0x0325
#define META_ESCAPE		     0x0626
#define META_RESTOREDC		     0x0127
#define META_FILLREGION		     0x0228
#define META_FRAMEREGION	     0x0429
#define META_INVERTREGION	     0x012A
#define META_PAINTREGION	     0x012B
#define META_SELECTCLIPREGION	     0x012C
#define META_SELECTOBJECT	     0x012D
#define META_SETTEXTALIGN	     0x012E
#define META_DRAWTEXT		     0x062F

#define	META_CHORD		     0x0830
#define	META_SETMAPPERFLAGS	     0x0231
#define	META_EXTTEXTOUT		     0x0a32
#define	META_SETDIBTODEV	     0x0d33
#define	META_SELECTPALETTE	     0x0234
#define	META_REALIZEPALETTE	     0x0035
#define	META_ANIMATEPALETTE	     0x0436
#define	META_SETPALENTRIES	     0x0037
#define	META_POLYPOLYGON	     0x0538
#define	META_RESIZEPALETTE	     0x0139

#define	META_DIBBITBLT		     0x0940
#define	META_DIBSTRETCHBLT	     0x0b41
#define	META_DIBCREATEPATTERNBRUSH   0x0142
#define	META_STRETCHDIB		     0x0f43

#define META_EXTFLOODFILL	     0x0548

#define META_RESETDC		     0x014C
#define META_STARTDOC		     0x014D
#define META_STARTPAGE		     0x004F
#define META_ENDPAGE		     0x0050
#define META_ABORTDOC		     0x0052
#define META_ENDDOC		     0x005E

#define	META_DELETEOBJECT	     0x01f0

#define	META_CREATEPALETTE	     0x00f7
#define META_CREATEBRUSH	     0x00F8
#define META_CREATEPATTERNBRUSH	     0x01F9
#define META_CREATEPENINDIRECT	     0x02FA
#define META_CREATEFONTINDIRECT	     0x02FB
#define META_CREATEBRUSHINDIRECT     0x02FC
#define META_CREATEBITMAPINDIRECT    0x02FD
#define META_CREATEBITMAP	     0x06FE
#define META_CREATEREGION	     0x06FF

/* Background Modes */
#define TRANSPARENT     1
#define OPAQUE          2

/* Pen Styles */
#define PS_SOLID	    0
#define PS_DASH             1
#define PS_DOT              2
#define PS_DASHDOT          3
#define PS_DASHDOTDOT       4
#define PS_NULL 	    5
#define PS_INSIDEFRAME 	    6

/* PitchAndFamily family values (high 4 bits) */
/* Win32s/Borland don't need this */
#if !defined(__BORLANDC__) && !defined(WIN32)
#define FF_DONTCARE         0x00
#define FF_ROMAN            0x10
#define FF_SWISS            0x20
#define FF_MODERN           0x30
#define FF_SCRIPT           0x40
#define FF_DECORATIVE       0x50
#endif

/* Brush Styles */
#define BS_SOLID	    0
#define BS_NULL		    1
#define BS_HOLLOW	    BS_NULL
#define BS_HATCHED	    2
#define BS_PATTERN	    3
#define BS_INDEXED	    4
#define	BS_DIBPATTERN	    5

/* Hatch Styles */
#define HS_HORIZONTAL       0
#define HS_VERTICAL         1
#define HS_FDIAGONAL        2
#define HS_BDIAGONAL        3
#define HS_CROSS            4
#define HS_DIAGCROSS        5

#endif // metafile functions

class wxMetaRecord: public wxObject
{
  public:
  int metaFunction;
  long param1;
  long param2;
  long param3;
  long param4;
  long param5;
  long param6;
  long param7;
  long param8;
  char *stringParam;
  wxRealPoint *points;
  
  wxMetaRecord(int fun)
  {
    metaFunction = fun; points = NULL; stringParam = NULL;
    param1 = 0;
  }
  ~wxMetaRecord(void);
};

class wxXMetaFile: public wxObject
{
 public:
  float lastX;
  float lastY;
  bool ok;

  float left;
  float top;
  float right;
  float bottom;
  
  wxList metaRecords;
  wxList gdiObjects; // List of wxMetaRecord objects created with Create...,
                     // referenced by position in list by SelectObject
  wxXMetaFile(char *file = NULL);
  ~wxXMetaFile(void);
  
  // After this is called, the metafile cannot be used for anything
  // since it is now owned by the clipboard.
  bool SetClipboard(int width = 0, int height = 0);

  bool Play(wxDC *dc);
  inline bool Ok(void) const { return ok; }
  bool ReadFile(char *file);
};

#endif
 // _MFUTILS_H_

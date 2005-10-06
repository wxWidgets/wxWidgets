/////////////////////////////////////////////////////////////////////////////
// Name:        mfutils.h
// Purpose:     Metafile utilities: reading a placeable metafile independently
//              of Windows.
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _MFUTILS_H_
#define _MFUTILS_H_


#include "wx/metafile.h"

#ifndef GetRValue
#define GetRValue(rgb) ((unsigned char)(rgb))
#define GetGValue(rgb) ((unsigned char)(((int)(rgb)) >> 8))
#define GetBValue(rgb) ((unsigned char)((rgb)>>16))
#endif

/* Metafile Functions */
// some compilers have them in their include files

#ifndef META_SETBKCOLOR
  #define META_SETBKCOLOR            0x0201
#endif
#ifndef META_SETBKMODE
  #define META_SETBKMODE             0x0102
#endif
#ifndef META_SETMAPMODE
  #define META_SETMAPMODE            0x0103
#endif
#ifndef META_SETROP2
  #define META_SETROP2               0x0104
#endif
#ifndef META_SETRELABS
  #define META_SETRELABS             0x0105
#endif
#ifndef META_SETPOLYFILLMODE
  #define META_SETPOLYFILLMODE       0x0106
#endif
#ifndef META_SETSTRETCHBLTMODE
  #define META_SETSTRETCHBLTMODE     0x0107
#endif
#ifndef META_SETTEXTCHAREXTRA
  #define META_SETTEXTCHAREXTRA      0x0108
#endif
#ifndef META_SETTEXTCOLOR
  #define META_SETTEXTCOLOR          0x0209
#endif
#ifndef META_SETTEXTJUSTIFICATION
  #define META_SETTEXTJUSTIFICATION  0x020A
#endif
#ifndef META_SETWINDOWORG
  #define META_SETWINDOWORG          0x020B
#endif
#ifndef META_SETWINDOWEXT
  #define META_SETWINDOWEXT          0x020C
#endif
#ifndef META_SETVIEWPORTORG
  #define META_SETVIEWPORTORG        0x020D
#endif
#ifndef META_SETVIEWPORTEXT
  #define META_SETVIEWPORTEXT        0x020E
#endif
#ifndef META_OFFSETWINDOWORG
  #define META_OFFSETWINDOWORG       0x020F
#endif
#ifndef META_SCALEWINDOWEXT
  #define META_SCALEWINDOWEXT        0x0410
#endif
#ifndef META_OFFSETVIEWPORTORG
  #define META_OFFSETVIEWPORTORG     0x0211
#endif
#ifndef META_SCALEVIEWPORTEXT
  #define META_SCALEVIEWPORTEXT      0x0412
#endif
#ifndef META_LINETO
  #define META_LINETO                0x0213
#endif
#ifndef META_MOVETO
  #define META_MOVETO                0x0214
#endif
#ifndef META_EXCLUDECLIPRECT
  #define META_EXCLUDECLIPRECT       0x0415
#endif
#ifndef META_INTERSECTCLIPRECT
  #define META_INTERSECTCLIPRECT     0x0416
#endif
#ifndef META_ARC
  #define META_ARC                   0x0817
#endif
#ifndef META_ELLIPSE
  #define META_ELLIPSE               0x0418
#endif
#ifndef META_FLOODFILL
  #define META_FLOODFILL             0x0419
#endif
#ifndef META_PIE
  #define META_PIE                   0x081A
#endif
#ifndef META_RECTANGLE
  #define META_RECTANGLE             0x041B
#endif
#ifndef META_ROUNDRECT
  #define META_ROUNDRECT             0x061C
#endif
#ifndef META_PATBLT
  #define META_PATBLT                0x061D
#endif
#ifndef META_SAVEDC
  #define META_SAVEDC                0x001E
#endif
#ifndef META_SETPIXEL
  #define META_SETPIXEL              0x041F
#endif
#ifndef META_OFFSETCLIPRGN
  #define META_OFFSETCLIPRGN         0x0220
#endif
#ifndef META_TEXTOUT
  #define META_TEXTOUT               0x0521
#endif
#ifndef META_BITBLT
  #define META_BITBLT                0x0922
#endif
#ifndef META_STRETCHBLT
  #define META_STRETCHBLT            0x0B23
#endif
#ifndef META_POLYGON
  #define META_POLYGON               0x0324
#endif
#ifndef META_POLYLINE
  #define META_POLYLINE              0x0325
#endif
#ifndef META_ESCAPE
  #define META_ESCAPE                0x0626
#endif
#ifndef META_RESTOREDC
  #define META_RESTOREDC             0x0127
#endif
#ifndef META_FILLREGION
  #define META_FILLREGION            0x0228
#endif
#ifndef META_FRAMEREGION
  #define META_FRAMEREGION           0x0429
#endif
#ifndef META_INVERTREGION
  #define META_INVERTREGION          0x012A
#endif
#ifndef META_PAINTREGION
  #define META_PAINTREGION           0x012B
#endif
#ifndef META_SELECTCLIPREGION
  #define META_SELECTCLIPREGION      0x012C
#endif
#ifndef META_SELECTOBJECT
  #define META_SELECTOBJECT          0x012D
#endif
#ifndef META_SETTEXTALIGN
  #define META_SETTEXTALIGN          0x012E
#endif
#ifndef META_DRAWTEXT
  #define META_DRAWTEXT              0x062F
#endif

#ifndef META_CHORD
  #define META_CHORD                 0x0830
#endif
#ifndef META_SETMAPPERFLAGS
  #define META_SETMAPPERFLAGS        0x0231
#endif
#ifndef META_EXTTEXTOUT
  #define META_EXTTEXTOUT            0x0a32
#endif
#ifndef META_SETDIBTODEV
  #define META_SETDIBTODEV           0x0d33
#endif
#ifndef META_SELECTPALETTE
  #define META_SELECTPALETTE         0x0234
#endif
#ifndef META_REALIZEPALETTE
  #define META_REALIZEPALETTE        0x0035
#endif
#ifndef META_ANIMATEPALETTE
  #define META_ANIMATEPALETTE        0x0436
#endif
#ifndef META_SETPALENTRIES
  #define META_SETPALENTRIES         0x0037
#endif
#ifndef META_POLYPOLYGON
  #define META_POLYPOLYGON           0x0538
#endif
#ifndef META_RESIZEPALETTE
  #define META_RESIZEPALETTE         0x0139
#endif

#ifndef META_DIBBITBLT
  #define META_DIBBITBLT             0x0940
#endif
#ifndef META_DIBSTRETCHBLT
  #define META_DIBSTRETCHBLT         0x0b41
#endif
#ifndef META_DIBCREATEPATTERNBRUSH
  #define META_DIBCREATEPATTERNBRUSH 0x0142
#endif
#ifndef META_STRETCHDIB
  #define META_STRETCHDIB            0x0f43
#endif

#ifndef META_EXTFLOODFILL
  #define META_EXTFLOODFILL          0x0548
#endif

#ifndef META_RESETDC
  #define META_RESETDC               0x014C
#endif
#ifndef META_STARTDOC
  #define META_STARTDOC              0x014D
#endif
#ifndef META_STARTPAGE
  #define META_STARTPAGE             0x004F
#endif
#ifndef META_ENDPAGE
  #define META_ENDPAGE               0x0050
#endif
#ifndef META_ABORTDOC
  #define META_ABORTDOC              0x0052
#endif
#ifndef META_ENDDOC
  #define META_ENDDOC                0x005E
#endif

#ifndef META_DELETEOBJECT
  #define META_DELETEOBJECT          0x01f0
#endif

#ifndef META_CREATEPALETTE
  #define META_CREATEPALETTE         0x00f7
#endif
#ifndef META_CREATEBRUSH
  #define META_CREATEBRUSH           0x00F8
#endif
#ifndef META_CREATEPATTERNBRUSH
  #define META_CREATEPATTERNBRUSH    0x01F9
#endif
#ifndef META_CREATEPENINDIRECT
  #define META_CREATEPENINDIRECT     0x02FA
#endif
#ifndef META_CREATEFONTINDIRECT
  #define META_CREATEFONTINDIRECT    0x02FB
#endif
#ifndef META_CREATEBRUSHINDIRECT
  #define META_CREATEBRUSHINDIRECT   0x02FC
#endif
#ifndef META_CREATEBITMAPINDIRECT
  #define META_CREATEBITMAPINDIRECT  0x02FD
#endif
#ifndef META_CREATEBITMAP
  #define META_CREATEBITMAP          0x06FE
#endif
#ifndef META_CREATEREGION
  #define META_CREATEREGION          0x06FF
#endif

/* Background Modes */
#ifndef TRANSPARENT
  #define TRANSPARENT     1
#endif
#ifndef OPAQUE
  #define OPAQUE          2
#endif

/* Pen Styles */
#ifndef PS_SOLID
  #define PS_SOLID        0
#endif
#ifndef PS_DASH
  #define PS_DASH         1
#endif
#ifndef PS_DOT
  #define PS_DOT          2
#endif
#ifndef PS_DASHDOT
  #define PS_DASHDOT      3
#endif
#ifndef PS_DASHDOTDOT
  #define PS_DASHDOTDOT   4
#endif
#ifndef PS_NULL
  #define PS_NULL         5
#endif
#ifndef PS_INSIDEFRAME
  #define PS_INSIDEFRAME  6
#endif

/* PitchAndFamily family values (high 4 bits) */
#ifndef FF_DONTCARE
  #define FF_DONTCARE         0x00
#endif
#ifndef FF_ROMAN
  #define FF_ROMAN            0x10
#endif
#ifndef FF_SWISS
  #define FF_SWISS            0x20
#endif
#ifndef FF_MODERN
  #define FF_MODERN           0x30
#endif
#ifndef FF_SCRIPT
  #define FF_SCRIPT           0x40
#endif
#ifndef FF_DECORATIVE
  #define FF_DECORATIVE       0x50
#endif

/* Brush Styles */
#ifndef BS_SOLID
  #define BS_SOLID        0
#endif
#ifndef BS_NULL
  #define BS_NULL         1
#endif
#ifndef BS_HOLLOW
  #define BS_HOLLOW       BS_NULL
#endif
#ifndef BS_HATCHED
  #define BS_HATCHED      2
#endif
#ifndef BS_PATTERN
  #define BS_PATTERN      3
#endif
#ifndef BS_INDEXED
  #define BS_INDEXED      4
#endif
#ifndef BS_DIBPATTERN
  #define BS_DIBPATTERN   5
#endif

/* Hatch Styles */
#ifndef HS_HORIZONTAL
  #define HS_HORIZONTAL       0
#endif
#ifndef HS_VERTICAL
  #define HS_VERTICAL         1
#endif
#ifndef HS_FDIAGONAL
  #define HS_FDIAGONAL        2
#endif
#ifndef HS_BDIAGONAL
  #define HS_BDIAGONAL        3
#endif
#ifndef HS_CROSS
  #define HS_CROSS            4
#endif
#ifndef HS_DIAGCROSS
  #define HS_DIAGCROSS        5
#endif

class WXDLLIMPEXP_OGL wxMetaRecord: public wxObject
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
  wxChar *stringParam;
  wxRealPoint *points;

  wxMetaRecord(int fun)
  {
    metaFunction = fun; points = NULL; stringParam = NULL;
    param1 = 0;
  }
  ~wxMetaRecord(void);
};

class WXDLLIMPEXP_OGL wxXMetaFile: public wxObject
{
 public:
  double lastX;
  double lastY;
  bool ok;

  double left;
  double top;
  double right;
  double bottom;

  wxList metaRecords;
  wxList gdiObjects; // List of wxMetaRecord objects created with Create...,
                     // referenced by position in list by SelectObject
  wxXMetaFile(const wxChar* file = NULL);
  ~wxXMetaFile(void);

  // After this is called, the metafile cannot be used for anything
  // since it is now owned by the clipboard.
  bool SetClipboard(int width = 0, int height = 0);

  bool Play(wxDC *dc);
  inline bool Ok(void) const { return ok; }
  bool ReadFile(const wxChar *file);
};

#endif
 // _MFUTILS_H_

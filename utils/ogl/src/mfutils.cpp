/////////////////////////////////////////////////////////////////////////////
// Name:        mfutils.cpp
// Purpose:     Metafile utillities
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "mfutils.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/metafile.h>
#include <wx/utils.h>

#include "mfutils.h"
#include <stdio.h>

static char hexArray[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B',
  'C', 'D', 'E', 'F' };

static void DecToHex(int dec, char *buf)
{
  int firstDigit = (int)(dec/16.0);
  int secondDigit = (int)(dec - (firstDigit*16.0));
  buf[0] = hexArray[firstDigit];
  buf[1] = hexArray[secondDigit];
  buf[2] = 0;
}
 
// 16-bit unsigned integer
static unsigned int getshort(FILE *fp)
{
  int c, c1;
  c = getc(fp);  c1 = getc(fp);
  unsigned int res = ((unsigned int) c) + (((unsigned int) c1) << 8);
  return res;
}

// 16-bit signed integer
static int getsignedshort(FILE *fp)
{
  int c, c1;
  c = getc(fp);  c1 = getc(fp);
  int testRes = ((unsigned int) c) + (((unsigned int) c1) << 8);
  unsigned long res1 = ((unsigned int) c) + (((unsigned int) c1) << 8);
  int res = 0;
  if (res1 > 32767)
    res = (int)(res1 - 65536);
  else
    res = (int)(res1);
  return res;
}

// 32-bit integer
static long getint(FILE *fp)
{
  int c, c1, c2, c3;
  c = getc(fp);  c1 = getc(fp);  c2 = getc(fp);  c3 = getc(fp);
  long res = (long)((long) c) +
         (((long) c1) << 8) + 
	 (((long) c2) << 16) +
	 (((long) c3) << 24);
  return res;
}


/* Placeable metafile header
struct mfPLACEABLEHEADER {
	DWORD	key;         // 32-bit
	HANDLE	hmf;         // 16-bit
	RECT	bbox;        // 4x16 bit
	WORD	inch;        // 16-bit
	DWORD	reserved;    // 32-bit
	WORD	checksum;    // 16-bit
};
*/

wxMetaRecord::~wxMetaRecord(void)
{
  if (points) delete[] points;
  if (stringParam) delete[] stringParam;
}

wxXMetaFile::wxXMetaFile(char *file)
{
  ok = FALSE;
  top = 0.0;
  bottom = 0.0;
  left = 0.0;
  right = 0.0;
  
  if (file)
    ok = ReadFile(file);
}

/*
  Handle table       gdiObjects
  ------------       ----------
  [0]                  wxPen
  [1]----param2---     wxBrush
  [2]             |    wxFont
  [3]             | -> wxPen
  
 The handle table works as follows.
 When a GDI object is created whilst reading in the
 metafile, the (e.g.) createpen record is added to the
 first free entry in the handle table. The createpen
 record's param1 is a pointer to the actual wxPen, and
 its param2 is the index into the gdiObjects list, which only
 grows and never shrinks (unlike the handle table.)
 
 When SelectObject(index) is found, the index in the file
 refers to the position in the handle table. BUT we then
 set param2 to be the position of the wxPen in gdiObjects,
 i.e. to param2 of the CreatePen record, itself found in
 the handle table.

 When an object is deleted, the entry in the handletable is
 NULLed but the gdiObjects entry is not removed (no point, and
 allows us to create all GDI objects in advance of playing the
 metafile).
*/
 

static wxMetaRecord *HandleTable[100];
static int HandleTableSize = 0;

void DeleteMetaRecordHandle(int index)
{
  HandleTable[index] = NULL;
}

int AddMetaRecordHandle(wxMetaRecord *record)
{
  for (int i = 0; i < HandleTableSize; i++)
    if (!HandleTable[i])
    {
      HandleTable[i] = record;
      return i;
    }
  // No free spaces in table, so append.
  
  HandleTable[HandleTableSize] = record;
  HandleTableSize ++;
  return (HandleTableSize - 1);
}

bool wxXMetaFile::ReadFile(char *file)
{
  HandleTableSize = 0;
  
  FILE *handle = fopen(file, "rb");
  if (!handle) return FALSE;

  // Read placeable metafile header, if any
  long key = getint(handle);
    
  if (key == 0x9AC6CDD7)
  {
    long hmf = getshort(handle);
    int iLeft, iTop, iRight, iBottom;
    iLeft = getsignedshort(handle);
    iTop = getsignedshort(handle);
    iRight = getsignedshort(handle);
    iBottom = getsignedshort(handle);

    left = (float)iLeft;
    top = (float)iTop;
    right = (float)iRight;
    bottom = (float)iBottom;

    int inch = getshort(handle);
    long reserved = getint(handle);
    int checksum = getshort(handle);
/*
      float widthInUnits = (float)right - left;
      float heightInUnits = (float)bottom - top;
      *width = (int)((widthInUnits*1440.0)/inch);
      *height = (int)((heightInUnits*1440.0)/inch);
*/
  }
  else rewind(handle);

  // Read METAHEADER
  int mtType = getshort(handle);

  if (mtType != 1 && mtType != 2)
  {
    fclose(handle);
    return FALSE;
  }

  int mtHeaderSize = getshort(handle);
  int mtVersion = getshort(handle);

  if (mtVersion != 0x0300 && mtVersion != 0x0100)
  {
    fclose(handle);
    return FALSE;
  }
  
  long mtSize = getint(handle);
  int mtNoObjects = getshort(handle);
  long mtMaxRecord = getint(handle);
  int mtNoParameters = getshort(handle);

  while (!feof(handle))
  {
    long rdSize = getint(handle);      // 4 bytes
    int rdFunction = getshort(handle); // 2 bytes
    if (feof(handle))
      break;

    switch (rdFunction)
    {
      case META_SETBKCOLOR:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_SETBKCOLOR);
        long colorref = getint(handle); // COLORREF
        rec->param1 = GetRValue(colorref);
        rec->param2 = GetGValue(colorref);
        rec->param3 = GetBValue(colorref);
        metaRecords.Append(rec);
        break;
      }
      case META_SETBKMODE:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_SETBKMODE);
        rec->param1 = getshort(handle); // Background mode
        if (rec->param1 == OPAQUE) rec->param1 = wxSOLID;
        else rec->param1 = wxTRANSPARENT;
        metaRecords.Append(rec);
        break;
      }
      case META_SETMAPMODE:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_SETMAPMODE);
        rec->param1 = getshort(handle);
        metaRecords.Append(rec);
        break;
      }
//      case META_SETROP2:
//      case META_SETRELABS:
//      case META_SETPOLYFILLMODE:
//      case META_SETSTRETCHBLTMODE:
//      case META_SETTEXTCHAREXTRA:
      case META_SETTEXTCOLOR:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_SETTEXTCOLOR);
        long colorref = getint(handle); // COLORREF
        rec->param1 = GetRValue(colorref);
        rec->param2 = GetGValue(colorref);
        rec->param3 = GetBValue(colorref);
        metaRecords.Append(rec);
        break;
      }
//      case META_SETTEXTJUSTIFICATION:
      case META_SETWINDOWORG:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_SETWINDOWORG);
        rec->param2 = getshort(handle);
        rec->param1 = getshort(handle);
        metaRecords.Append(rec);
        break;
      }
      case META_SETWINDOWEXT:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_SETWINDOWEXT);
        rec->param2 = getshort(handle);
        rec->param1 = getshort(handle);
        metaRecords.Append(rec);
        break;
      }
//      case META_SETVIEWPORTORG:
//      case META_SETVIEWPORTEXT:
//      case META_OFFSETWINDOWORG:
//      case META_SCALEWINDOWEXT:
//      case META_OFFSETVIEWPORTORG:
//      case META_SCALEVIEWPORTEXT:
      case META_LINETO:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_LINETO);
        rec->param1 = getshort(handle); // x1
        rec->param2 = getshort(handle); // y1
        metaRecords.Append(rec);
        break;
      }
      case META_MOVETO:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_MOVETO);
        rec->param1 = getshort(handle); // x1
        rec->param2 = getshort(handle); // y1
        metaRecords.Append(rec);
        break;
      }
      case META_EXCLUDECLIPRECT:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_EXCLUDECLIPRECT);
        rec->param4 = getshort(handle); // y2
        rec->param3 = getshort(handle); // x2
        rec->param2 = getshort(handle); // y1
        rec->param1 = getshort(handle); // x1
        metaRecords.Append(rec);
        break;
      }
      case META_INTERSECTCLIPRECT:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_INTERSECTCLIPRECT);
        rec->param4 = getshort(handle); // y2
        rec->param3 = getshort(handle); // x2
        rec->param2 = getshort(handle); // y1
        rec->param1 = getshort(handle); // x1
        metaRecords.Append(rec);
        break;
      }
//      case META_ARC: // DO!!!
      case META_ELLIPSE:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_ELLIPSE);
        rec->param4 = getshort(handle); // y2
        rec->param3 = getshort(handle); // x2
        rec->param2 = getshort(handle); // y1
        rec->param1 = getshort(handle); // x1
        metaRecords.Append(rec);
        break;
      }
//      case META_FLOODFILL:
//      case META_PIE: // DO!!!
      case META_RECTANGLE:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_RECTANGLE);
        rec->param4 = getshort(handle); // y2
        rec->param3 = getshort(handle); // x2
        rec->param2 = getshort(handle); // y1
        rec->param1 = getshort(handle); // x1
        metaRecords.Append(rec);
        break;
      }
      case META_ROUNDRECT:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_ROUNDRECT);
        rec->param6 = getshort(handle); // width
        rec->param5 = getshort(handle); // height
        rec->param4 = getshort(handle); // y2
        rec->param3 = getshort(handle); // x2
        rec->param2 = getshort(handle); // y1
        rec->param1 = getshort(handle); // x1
        metaRecords.Append(rec);
        break;
      }
//      case META_PATBLT:
//      case META_SAVEDC:
      case META_SETPIXEL:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_SETPIXEL);
        rec->param1 = getshort(handle); // x1
        rec->param2 = getshort(handle); // y1
        rec->param3 = getint(handle);   // COLORREF
        metaRecords.Append(rec);
        break;
      }
//      case META_OFFSETCLIPRGN:
      case META_TEXTOUT:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_TEXTOUT);
        int count = getshort(handle);
        rec->stringParam = new char[count+1];
        fread((void *)rec->stringParam, sizeof(char), count, handle);
        rec->stringParam[count] = 0;
        rec->param2 = getshort(handle); // Y
        rec->param1 = getshort(handle); // X
        metaRecords.Append(rec);
        break;
      }
/*
      case META_EXTTEXTOUT:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_EXTTEXTOUT);
        int cellSpacing = getshort(handle);
        int count = getshort(handle);
        rec->stringParam = new char[count+1];
        fread((void *)rec->stringParam, sizeof(char), count, handle);
        rec->stringParam[count] = 0;
        // Rectangle
        int rectY2 = getshort(handle);
        int rectX2 = getshort(handle);
        int rectY1 = getshort(handle);
        int rectX1 = getshort(handle);
        int rectType = getshort(handle);
        rec->param2 = getshort(handle); // Y
        rec->param1 = getshort(handle); // X
        metaRecords.Append(rec);
        break;
      }
*/
//      case META_BITBLT:
//      case META_STRETCHBLT:
      case META_POLYGON:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_POLYGON);
        rec->param1 = getshort(handle);
        rec->points = new wxRealPoint[(int)rec->param1];
        for (int i = 0; i < rec->param1; i++)
        {
          rec->points[i].x = getshort(handle);
          rec->points[i].y = getshort(handle);
        }
        
        metaRecords.Append(rec);
        break;
      }
      case META_POLYLINE:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_POLYLINE);
        rec->param1 = (long)getshort(handle);
        rec->points = new wxRealPoint[(int)rec->param1];
        for (int i = 0; i < rec->param1; i++)
        {
          rec->points[i].x = getshort(handle);
          rec->points[i].y = getshort(handle);
        }
        
        metaRecords.Append(rec);
        break;
      }
//      case META_ESCAPE:
//      case META_RESTOREDC:
//      case META_FILLREGION:
//      case META_FRAMEREGION:
//      case META_INVERTREGION:
//      case META_PAINTREGION:
//      case META_SELECTCLIPREGION: // DO THIS!
      case META_SELECTOBJECT:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_SELECTOBJECT);
        rec->param1 = (long)getshort(handle); // Position of object in gdiObjects list
        metaRecords.Append(rec);
        // param2 gives the index into gdiObjects, which is different from
        // the index into the handle table.
        rec->param2 = HandleTable[(int)rec->param1]->param2;
        break;
      }
//      case META_SETTEXTALIGN:
//      case META_DRAWTEXT:
//      case META_CHORD:
//      case META_SETMAPPERFLAGS:
//      case META_EXTTEXTOUT:
//      case META_SETDIBTODEV:
//      case META_SELECTPALETTE:
//      case META_REALIZEPALETTE:
//      case META_ANIMATEPALETTE:
//      case META_SETPALENTRIES:
//      case META_POLYPOLYGON:
//      case META_RESIZEPALETTE:
//      case META_DIBBITBLT:
//      case META_DIBSTRETCHBLT:
      case META_DIBCREATEPATTERNBRUSH:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_DIBCREATEPATTERNBRUSH);
        fread((void *)wxBuffer, sizeof(char), (int)((2*rdSize) - 6), handle);
        
        metaRecords.Append(rec);
        gdiObjects.Append(rec);
        AddMetaRecordHandle(rec);
        rec->param2 = (long)(gdiObjects.Number() - 1);
        break;
      }
//      case META_STRETCHDIB:
//      case META_EXTFLOODFILL:
//      case META_RESETDC:
//      case META_STARTDOC:
//      case META_STARTPAGE:
//      case META_ENDPAGE:
//      case META_ABORTDOC:
//      case META_ENDDOC:
      case META_DELETEOBJECT:
      {
        int index = getshort(handle);
        DeleteMetaRecordHandle(index);
        break;
      }
      case META_CREATEPALETTE:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_CREATEPALETTE);
        fread((void *)wxBuffer, sizeof(char), (int)((2*rdSize) - 6), handle);
        
        metaRecords.Append(rec);
        gdiObjects.Append(rec);
        AddMetaRecordHandle(rec);
        rec->param2 = (long)(gdiObjects.Number() - 1);
        break;
      }
      case META_CREATEBRUSH:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_CREATEBRUSH);
        fread((void *)wxBuffer, sizeof(char), (int)((2*rdSize) - 6), handle);
        metaRecords.Append(rec);
        gdiObjects.Append(rec);
        AddMetaRecordHandle(rec);
        rec->param2 = (long)(gdiObjects.Number() - 1);
        break;
      }
      case META_CREATEPATTERNBRUSH:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_CREATEPATTERNBRUSH);
        fread((void *)wxBuffer, sizeof(char), (int)((2*rdSize) - 6), handle);
        metaRecords.Append(rec);
        gdiObjects.Append(rec);
        AddMetaRecordHandle(rec);
        rec->param2 = (long)(gdiObjects.Number() - 1);
        break;
      }
      case META_CREATEPENINDIRECT:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_CREATEPENINDIRECT);
        int msStyle = getshort(handle); // Style: 2 bytes
        int x = getshort(handle); // X:     2 bytes
        int y = getshort(handle); // Y:     2 bytes
        long colorref = getint(handle); // COLORREF 4 bytes

        int style;
        if (msStyle == PS_DOT)
          style = wxDOT;
        else if (msStyle == PS_DASH)
          style = wxSHORT_DASH;
        else if (msStyle == PS_NULL)
          style = wxTRANSPARENT;
        else style = wxSOLID;

        wxColour colour(GetRValue(colorref), GetGValue(colorref), GetBValue(colorref));
        rec->param1 = (long)wxThePenList->FindOrCreatePen(colour, x, style);
        metaRecords.Append(rec);
        gdiObjects.Append(rec);

        AddMetaRecordHandle(rec);
        rec->param2 = (long)(gdiObjects.Number() - 1);
        
        // For some reason, the size of this record is sometimes 9 words!!!
        // instead of the usual 8. So read 2 characters extra.
        if (rdSize == 9)
        {
          (void) getshort(handle);
        }
        break;
      }
      case META_CREATEFONTINDIRECT:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_CREATEFONTINDIRECT);
        int lfHeight = getshort(handle);    // 2 bytes
        int lfWidth = getshort(handle);     // 2 bytes
        int lfEsc = getshort(handle);       // 2 bytes
        int lfOrient = getshort(handle);    // 2 bytes
        int lfWeight = getshort(handle);    // 2 bytes
        char lfItalic = getc(handle);       // 1 byte
        char lfUnderline = getc(handle);    // 1 byte
        char lfStrikeout = getc(handle);    // 1 byte
        char lfCharSet = getc(handle);      // 1 byte
        char lfOutPrecision = getc(handle); // 1 byte
        char lfClipPrecision = getc(handle); // 1 byte
        char lfQuality = getc(handle);      // 1 byte
        char lfPitchAndFamily = getc(handle);   // 1 byte (18th)
        char lfFacename[32];
        // Read the rest of the record, which is total record size
        // minus the number of bytes already read (18 record, 6 metarecord
        // header)
        fread((void *)lfFacename, sizeof(char), (int)((2*rdSize) - 18 - 6), handle);

        int family;
        if (lfPitchAndFamily & FF_MODERN)
          family = wxMODERN;
        else if (lfPitchAndFamily & FF_MODERN)
          family = wxMODERN;
        else if (lfPitchAndFamily & FF_ROMAN)
          family = wxROMAN;
        else if (lfPitchAndFamily & FF_SWISS)
          family = wxSWISS;
        else if (lfPitchAndFamily & FF_DECORATIVE)
          family = wxDECORATIVE;
        else
          family = wxDEFAULT;

        int weight;
        if (lfWeight == 300)
          weight = wxLIGHT;
        else if (lfWeight == 400)
          weight = wxNORMAL;
        else if (lfWeight == 900)
          weight = wxBOLD;
        else weight = wxNORMAL;

        int style;
        if (lfItalic != 0)
          style = wxITALIC;
        else
          style = wxNORMAL;

        // About how many pixels per inch???
        int logPixelsY = 100;
        int pointSize = (int)(lfHeight*72.0/logPixelsY);

        wxFont *theFont =
          wxTheFontList->FindOrCreateFont(pointSize, family, style, weight, (lfUnderline != 0));

        rec->param1 = (long) theFont;
        metaRecords.Append(rec);
        gdiObjects.Append(rec);
        AddMetaRecordHandle(rec);
        rec->param2 = (long)(gdiObjects.Number() - 1);
        break;
      }
      case META_CREATEBRUSHINDIRECT:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_CREATEBRUSHINDIRECT);
        int msStyle = getshort(handle); // Style: 2 bytes
        long colorref = getint(handle);   // COLORREF: 4 bytes
        int hatchStyle = getshort(handle); // Hatch style 2 bytes

        int style;
        switch (msStyle)
        {
          case BS_HATCHED:
          {
            switch (hatchStyle)
            {
              case HS_BDIAGONAL:
                style = wxBDIAGONAL_HATCH;
                break;
              case HS_DIAGCROSS:
                style = wxCROSSDIAG_HATCH;
                break;
              case HS_FDIAGONAL:
                style = wxFDIAGONAL_HATCH;
                break;
              case HS_HORIZONTAL:
                style = wxHORIZONTAL_HATCH;
                break;
              case HS_VERTICAL:
                style = wxVERTICAL_HATCH;
                break;
              default:
              case HS_CROSS:
                style = wxCROSS_HATCH;
                break;
            }
            break;
          }
          case BS_SOLID:
          default:
            style = wxSOLID;
            break;
        }
        if (msStyle == PS_DOT)
          style = wxDOT;
        else if (msStyle == PS_DASH)
          style = wxSHORT_DASH;
        else if (msStyle == PS_NULL)
          style = wxTRANSPARENT;
        else style = wxSOLID;

        wxColour colour(GetRValue(colorref), GetGValue(colorref), GetBValue(colorref));
        rec->param1 = (long)wxTheBrushList->FindOrCreateBrush(colour, style);
        metaRecords.Append(rec);
        gdiObjects.Append(rec);
        AddMetaRecordHandle(rec);
        rec->param2 = (long)(gdiObjects.Number() - 1);
        break;
      }
      case META_CREATEBITMAPINDIRECT:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_CREATEBITMAPINDIRECT);
        fread((void *)wxBuffer, sizeof(char), (int)((2*rdSize) - 6), handle);
        
        metaRecords.Append(rec);
        gdiObjects.Append(rec);
        AddMetaRecordHandle(rec);
        rec->param2 = (long)(gdiObjects.Number() - 1);
        break;
      }
      case META_CREATEBITMAP:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_CREATEBITMAP);
        fread((void *)wxBuffer, sizeof(char), (int)((2*rdSize) - 6), handle);
        
        metaRecords.Append(rec);
        gdiObjects.Append(rec);
        AddMetaRecordHandle(rec);
        rec->param2 = (long)(gdiObjects.Number() - 1);
        break;
      }
      case META_CREATEREGION:
      {
        wxMetaRecord *rec = new wxMetaRecord(META_CREATEREGION);
        fread((void *)wxBuffer, sizeof(char), (int)((2*rdSize) - 6), handle);

        metaRecords.Append(rec);
        gdiObjects.Append(rec);
        AddMetaRecordHandle(rec);
        rec->param2 = (long)(gdiObjects.Number() - 1);
        break;
      }
      default:
      {
        fread((void *)wxBuffer, sizeof(char), (int)((2*rdSize) - 6), handle);
        break;
      }
    }
  }
  fclose(handle);
  return TRUE;
}

wxXMetaFile::~wxXMetaFile(void)
{
  wxNode *node = metaRecords.First();
  while (node)
  {
    wxMetaRecord *rec = (wxMetaRecord *)node->Data();
    delete rec;
    wxNode *next = node->Next();
    delete node;
    node = next;
  }
}

bool wxXMetaFile::SetClipboard(int width, int height)
{
  return FALSE;
}

bool wxXMetaFile::Play(wxDC *dc)
{
  wxNode *node = metaRecords.First();
  while (node)
  {
    wxMetaRecord *rec = (wxMetaRecord *)node->Data();
    int rdFunction = rec->metaFunction;

    switch (rdFunction)
    {
      case META_SETBKCOLOR:
      {
        break;
      }
      case META_SETBKMODE:
      {
        break;
      }
      case META_SETMAPMODE:
      {
        break;
      }
//      case META_SETROP2:
//      case META_SETRELABS:
//      case META_SETPOLYFILLMODE:
//      case META_SETSTRETCHBLTMODE:
//      case META_SETTEXTCHAREXTRA:
      case META_SETTEXTCOLOR:
      {
        break;
      }
//      case META_SETTEXTJUSTIFICATION:
      case META_SETWINDOWORG:
      {
        break;
      }
      case META_SETWINDOWEXT:
      {
        break;
      }
//      case META_SETVIEWPORTORG:
//      case META_SETVIEWPORTEXT:
//      case META_OFFSETWINDOWORG:
//      case META_SCALEWINDOWEXT:
//      case META_OFFSETVIEWPORTORG:
//      case META_SCALEVIEWPORTEXT:
      case META_LINETO:
      {
        long x1 = rec->param1;
        long y1 = rec->param2;
        dc->DrawLine(lastX, lastY, (float)x1, (float)y1);
        break;
      }
      case META_MOVETO:
      {
        lastX = (float)rec->param1;
        lastY = (float)rec->param2;
        break;
      }
      case META_EXCLUDECLIPRECT:
      {
        break;
      }
      case META_INTERSECTCLIPRECT:
      {
        break;
      }
//      case META_ARC: // DO!!!
      case META_ELLIPSE:
      {
        break;
      }
//      case META_FLOODFILL:
//      case META_PIE: // DO!!!
      case META_RECTANGLE:
      {
        dc->DrawRectangle((float)rec->param1, (float)rec->param2,
                          (float)rec->param3 - rec->param1,
                          (float)rec->param4 - rec->param2);
        break;
      }
      case META_ROUNDRECT:
      {
        dc->DrawRoundedRectangle((float)rec->param1, (float)rec->param2,
                          (float)rec->param3 - rec->param1,
                          (float)rec->param4 - rec->param2,
                          (float)rec->param5);
        break;
      }
//      case META_PATBLT:
//      case META_SAVEDC:
      case META_SETPIXEL:
      {
//        rec->param1 = getshort(handle); // x1
//        rec->param2 = getshort(handle); // y1
//        rec->param3 = getint(handle);   // COLORREF
        break;
      }
//      case META_OFFSETCLIPRGN:
      case META_TEXTOUT:
      {
/*
        int count = getshort(handle);
        rec->stringParam = new char[count+1];
        fread((void *)rec->stringParam, sizeof(char), count, handle);
        rec->stringParam[count] = 0;
        rec->param2 = getshort(handle); // Y
        rec->param1 = getshort(handle); // X
*/
        break;
      }
//      case META_BITBLT:
//      case META_STRETCHBLT:
      case META_POLYGON:
      {
/*
        rec->param1 = getshort(handle);
        rec->points = new wxRealPoint[(int)rec->param1];
        for (int i = 0; i < rec->param1; i++)
        {
          rec->points[i].x = getshort(handle);
          rec->points[i].y = getshort(handle);
        }
*/        
        break;
      }
      case META_POLYLINE:
      {
/*
        wxMetaRecord *rec = new wxMetaRecord(META_POLYLINE);
        rec->param1 = (long)getshort(handle);
        rec->points = new wxRealPoint[(int)rec->param1];
        for (int i = 0; i < rec->param1; i++)
        {
          rec->points[i].x = getshort(handle);
          rec->points[i].y = getshort(handle);
        }
*/        
        break;
      }
//      case META_ESCAPE:
//      case META_RESTOREDC:
//      case META_FILLREGION:
//      case META_FRAMEREGION:
//      case META_INVERTREGION:
//      case META_PAINTREGION:
//      case META_SELECTCLIPREGION: // DO THIS!
      case META_SELECTOBJECT:
      {
/*
        wxMetaRecord *rec = new wxMetaRecord(META_SELECTOBJECT);
        rec->param1 = (long)getshort(handle); // Position of object in gdiObjects list
*/
        break;
      }
//      case META_SETTEXTALIGN:
//      case META_DRAWTEXT:
//      case META_CHORD:
//      case META_SETMAPPERFLAGS:
//      case META_EXTTEXTOUT:
//      case META_SETDIBTODEV:
//      case META_SELECTPALETTE:
//      case META_REALIZEPALETTE:
//      case META_ANIMATEPALETTE:
//      case META_SETPALENTRIES:
//      case META_POLYPOLYGON:
//      case META_RESIZEPALETTE:
//      case META_DIBBITBLT:
//      case META_DIBSTRETCHBLT:
      case META_DIBCREATEPATTERNBRUSH:
      {
/*
        fread((void *)wxBuffer, sizeof(char), (int)(rdSize - 3), handle);
*/        
        break;
      }
//      case META_STRETCHDIB:
//      case META_EXTFLOODFILL:
//      case META_RESETDC:
//      case META_STARTDOC:
//      case META_STARTPAGE:
//      case META_ENDPAGE:
//      case META_ABORTDOC:
//      case META_ENDDOC:
//      case META_DELETEOBJECT: // DO!!
      case META_CREATEPALETTE:
      {
/*
        wxMetaRecord *rec = new wxMetaRecord(META_CREATEPALETTE);
        fread((void *)wxBuffer, sizeof(char), (int)(rdSize - 3), handle);
*/
        break;
      }
      case META_CREATEBRUSH:
      {
/*
        fread((void *)wxBuffer, sizeof(char), (int)(rdSize - 3), handle);
*/
        break;
      }
      case META_CREATEPATTERNBRUSH:
      {
/*
        fread((void *)wxBuffer, sizeof(char), (int)(rdSize - 3), handle);
*/
        break;
      }
      case META_CREATEPENINDIRECT:
      {
/*
        int msStyle = getshort(handle); // Style: 2 bytes
        int x = getshort(handle); // X:     2 bytes
        int y = getshort(handle); // Y:     2 bytes
        int colorref = getint(handle); // COLORREF 4 bytes

        int style;
        if (msStyle == PS_DOT)
          style = wxDOT;
        else if (msStyle == PS_DASH)
          style = wxSHORT_DASH;
        else if (msStyle == PS_NULL)
          style = wxTRANSPARENT;
        else style = wxSOLID;

        wxColour colour(GetRValue(colorref), GetGValue(colorref), GetBValue(colorref));
        rec->param1 = (long)wxThePenList->FindOrCreatePen(&colour, x, style);
*/
        break;
      }
      case META_CREATEFONTINDIRECT:
      {
/*
        int lfHeight = getshort(handle);
        int lfWidth = getshort(handle);
        int lfEsc = getshort(handle);
        int lfOrient = getshort(handle);
        int lfWeight = getshort(handle);
        char lfItalic = getc(handle);
        char lfUnderline = getc(handle);
        char lfStrikeout = getc(handle);
        char lfCharSet = getc(handle);
        char lfOutPrecision = getc(handle);
        char lfClipPrecision = getc(handle);
        char lfQuality = getc(handle);
        char lfPitchAndFamily = getc(handle);
        char lfFacename[32];
        fread((void *)lfFacename, sizeof(char), 32, handle);

        int family;
        if (lfPitchAndFamily & FF_MODERN)
          family = wxMODERN;
        else if (lfPitchAndFamily & FF_MODERN)
          family = wxMODERN;
        else if (lfPitchAndFamily & FF_ROMAN)
          family = wxROMAN;
        else if (lfPitchAndFamily & FF_SWISS)
          family = wxSWISS;
        else if (lfPitchAndFamily & FF_DECORATIVE)
          family = wxDECORATIVE;
        else
          family = wxDEFAULT;

        int weight;
        if (lfWeight == 300)
          weight = wxLIGHT;
        else if (lfWeight == 400)
          weight = wxNORMAL;
        else if (lfWeight == 900)
          weight = wxBOLD;
        else weight = wxNORMAL;

        int style;
        if ((bool)lfItalic)
          style = wxITALIC;
        else
          style = wxNORMAL;

        // About how many pixels per inch???
        int logPixelsY = 100;
        int pointSize = (int)(lfHeight*72.0/logPixelsY);

        wxFont *theFont =
          wxTheFontList->FindOrCreateFont(pointSize, family, style, weight, (bool)lfUnderline);

        rec->param1 = (long)theFont;
*/
        break;
      }
      case META_CREATEBRUSHINDIRECT:
      {
/*
        int msStyle = getshort(handle); // Style: 2 bytes
        int colorref = getint(handle);   // COLORREF: 4 bytes
        int hatchStyle = getshort(handle); // Hatch style 2 bytes

        int style;
        if (msStyle == PS_DOT)
          style = wxDOT;
        else if (msStyle == PS_DASH)
          style = wxSHORT_DASH;
        else if (msStyle == PS_NULL)
          style = wxTRANSPARENT;
        else style = wxSOLID;

        wxColour colour(GetRValue(colorref), GetGValue(colorref), GetBValue(colorref));
        rec->param1 = (long)wxTheBrushList->FindOrCreateBrush(&colour, wxSOLID);
*/
        break;
      }
      case META_CREATEBITMAPINDIRECT:
      {
/*
        fread((void *)wxBuffer, sizeof(char), (int)(rdSize - 3), handle);
*/        
        break;
      }
      case META_CREATEBITMAP:
      {
/*
        fread((void *)wxBuffer, sizeof(char), (int)(rdSize - 3), handle);
*/        
        break;
      }
      case META_CREATEREGION:
      {
        dc->DestroyClippingRegion();
/*
        rec->param1 = getshort(handle); // Style: 2 bytes
*/
        break;
      }
      default:
      {
        break;
      }
    }
    node = node->Next();
  }
  return TRUE;
}


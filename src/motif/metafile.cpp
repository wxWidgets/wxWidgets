/////////////////////////////////////////////////////////////////////////////
// Name:        metafile.cpp
// Purpose:     wxMetaFile, wxMetaFileDC etc. These classes are optional.
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "metafile.h"
#endif

#include "wx/object.h"
#include "wx/string.h"
#include "wx/dc.h"
#include "wx/motif/metafile.h"
#include "wx/clipbrd.h"

extern bool wxClipboardIsOpen;

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxMetaFile, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxMetaFileDC, wxDC)
#endif

wxMetaFile::wxMetaFile(const wxString& file)
{
    // TODO
}

wxMetaFile::~wxMetaFile()
{
    // TODO
}

bool wxMetaFile::SetClipboard(int width, int height)
{
    bool alreadyOpen=wxClipboardOpen();
    if (!alreadyOpen)
    {
        wxOpenClipboard();
        if (!wxEmptyClipboard()) return FALSE;
    }
    bool success = wxSetClipboardData(wxDF_METAFILE,this, width,height);
    if (!alreadyOpen) wxCloseClipboard();
    return (bool) success;
}

bool wxMetaFile::Play(wxDC *dc)
{
    // TODO
    return FALSE;
}

/*
 * Metafile device context
 *
 */

// Original constructor that does not takes origin and extent. If you use this,
// *DO* give origin/extent arguments to wxMakeMetaFilePlaceable.
wxMetaFileDC::wxMetaFileDC(const wxString& file)
{
    // TODO
}

// New constructor that takes origin and extent. If you use this, don't
// give origin/extent arguments to wxMakeMetaFilePlaceable.
wxMetaFileDC::wxMetaFileDC(const wxString& file, int xext, int yext, int xorg, int yorg)
{
    // TODO
}

wxMetaFileDC::~wxMetaFileDC()
{
}

void wxMetaFileDC::GetTextExtent(const wxString& string, float *x, float *y,
                                 float *descent, float *externalLeading, wxFont *theFont, bool use16bit)
{
    // TODO
}

wxMetaFile *wxMetaFileDC::Close()
{
    // TODO
    return NULL;
}

void wxMetaFileDC::SetMapMode(int mode)
{
    // TODO
}

#if 0

#ifdef __WIN32__
struct RECT32
{
  short left;
  short top;
  short right;
  short bottom;
};

struct mfPLACEABLEHEADER {
	DWORD	key;
	short	hmf;
	RECT32	bbox;
	WORD	inch;
	DWORD	reserved;
	WORD	checksum;
};
#else
struct mfPLACEABLEHEADER {
	DWORD	key;
	HANDLE	hmf;
	RECT	bbox;
	WORD	inch;
	DWORD	reserved;
	WORD	checksum;
};
#endif

/*
 * Pass filename of existing non-placeable metafile, and bounding box.
 * Adds a placeable metafile header, sets the mapping mode to anisotropic,
 * and sets the window origin and extent to mimic the wxMM_TEXT mapping mode.
 *
 */
 
bool wxMakeMetaFilePlaceable(const wxString& filename, float scale)
{
  return wxMakeMetaFilePlaceable(filename, 0, 0, 0, 0, scale, FALSE);
}

bool wxMakeMetaFilePlaceable(const wxString& filename, int x1, int y1, int x2, int y2, float scale, bool useOriginAndExtent)
{
  // I'm not sure if this is the correct way of suggesting a scale
  // to the client application, but it's the only way I can find.
  int unitsPerInch = (int)(576/scale);
  
  mfPLACEABLEHEADER header;
  header.key = 0x9AC6CDD7L;
  header.hmf = 0;
  header.bbox.left = (int)(x1);
  header.bbox.top = (int)(y1);
  header.bbox.right = (int)(x2);
  header.bbox.bottom = (int)(y2);
  header.inch = unitsPerInch;
  header.reserved = 0;

  // Calculate checksum  
  WORD *p;
  mfPLACEABLEHEADER *pMFHead = &header;
  for (p =(WORD *)pMFHead,pMFHead -> checksum = 0;
	p < (WORD *)&pMFHead ->checksum; ++p)
       pMFHead ->checksum ^= *p;

  FILE *fd = fopen((char *)(const char *)filename, "rb");
  if (!fd) return FALSE;
  
  char tempFileBuf[256];
  wxGetTempFileName("mf", tempFileBuf);
  FILE *fHandle = fopen(tempFileBuf, "wb");
  if (!fHandle)
    return FALSE;
  fwrite((void *)&header, sizeof(unsigned char), sizeof(mfPLACEABLEHEADER), fHandle);

  // Calculate origin and extent
  int originX = x1;
  int originY = y1;
  int extentX = x2 - x1;
  int extentY = (y2 - y1);

  // Read metafile header and write
  METAHEADER metaHeader;
  fread((void *)&metaHeader, sizeof(unsigned char), sizeof(metaHeader), fd);
  
  if (useOriginAndExtent)
    metaHeader.mtSize += 15;
  else
    metaHeader.mtSize += 5;
    
  fwrite((void *)&metaHeader, sizeof(unsigned char), sizeof(metaHeader), fHandle);

  // Write SetMapMode, SetWindowOrigin and SetWindowExt records
  char modeBuffer[8];
  char originBuffer[10];
  char extentBuffer[10];
  METARECORD *modeRecord = (METARECORD *)&modeBuffer;

  METARECORD *originRecord = (METARECORD *)&originBuffer;
  METARECORD *extentRecord = (METARECORD *)&extentBuffer;

  modeRecord->rdSize = 4;
  modeRecord->rdFunction = META_SETMAPMODE;
  modeRecord->rdParm[0] = wxMM_ANISOTROPIC;

  originRecord->rdSize = 5;
  originRecord->rdFunction = META_SETWINDOWORG;
  originRecord->rdParm[0] = originY;
  originRecord->rdParm[1] = originX;

  extentRecord->rdSize = 5;
  extentRecord->rdFunction = META_SETWINDOWEXT;
  extentRecord->rdParm[0] = extentY;
  extentRecord->rdParm[1] = extentX;

  fwrite((void *)modeBuffer, sizeof(char), 8, fHandle);
  
  if (useOriginAndExtent)
  {
    fwrite((void *)originBuffer, sizeof(char), 10, fHandle);
    fwrite((void *)extentBuffer, sizeof(char), 10, fHandle);
  }

  int ch = -2;
  while (ch != EOF)
  {
    ch = getc(fd);
    if (ch != EOF)
    {
      putc(ch, fHandle);
    }
  }
  fclose(fHandle);
  fclose(fd);
  wxRemoveFile(filename);
  wxCopyFile(tempFileBuf, filename);
  wxRemoveFile(tempFileBuf);
  return TRUE;
}

#endif


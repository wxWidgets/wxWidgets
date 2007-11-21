/////////////////////////////////////////////////////////////////////////////
// Name:        readshg.cpp
// Purpose:     Petr Smilauer's .SHG (Segmented Hypergraphics file) reading
//              code.
//              Note: .SHG is undocumented (anywhere!) so this is
//              reverse-engineering
//              and guesswork at its best.
// Author:      Petr Smilauer
// Modified by: Wlodzimiez ABX Skiba 2003/2004 Unicode support
//              Ron Lee
// Created:     01/01/99
// RCS-ID:      $Id$
// Copyright:   (c) Petr Smilauer
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif

#include <stdio.h>
#include <stdlib.h>

#include "readshg.h"
#include "tex2any.h"

// Returns the number of hotspots, and the array of hotspots.
// E.g.
// HotSpots *array;
// int n = ParseSHG("thing.shg", &array);

int   ParseSHG( const wxChar* fileName, HotSpot **hotspots)
{ FILE*   fSHG = wxFopen( fileName, _T("rb"));
  long    offset;
  int nHotspots = 0;

  if(fSHG == 0)
    return 0;
  nHotspots = 0;
  //first, look at offset OFF_OFFSET to get another offset :-)
  fseek( fSHG, OFF_OFFSET, SEEK_SET);
  offset = 0L;                  // init whole 4-byte variable
  fread( &offset, 2, 1, fSHG);  // get the offset in first two bytes..
  if(offset == 0)               // if zero, used next DWORD field
    fread( &offset, 4, 1, fSHG);// this is our offset for very long DIB
  offset += 9;                  // don't know hot this delta comes-about
  if(fseek( fSHG, offset, SEEK_SET) != 0)
  {
    fclose( fSHG);
    return -1;    // this is probably because incorrect offset calculation.
  }
  fread( &nHotspots, 2, 1, fSHG);

  *hotspots = new HotSpot[nHotspots];

  int nMacroStrings = 0;

  fread( &nMacroStrings, 2, 1, fSHG); // we can ignore the macros, as this is
                         // repeated later, but we need to know how much to skip
  fseek( fSHG, 2, SEEK_CUR);  // skip another 2 bytes I do not understand ;-)

  ShgInfoBlock  sib;
  int           i;

  int sizeOf = sizeof( ShgInfoBlock);

  for( i = 0 ; i < nHotspots ; ++i)
  {
    fread( &sib, sizeOf, 1, fSHG); // read one hotspot' info
    // analyse it:
    (*hotspots)[i].type    = (HotspotType)(sib.hotspotType & 0xFB);
    (*hotspots)[i].left    = sib.left;
    (*hotspots)[i].top     = sib.top;
    (*hotspots)[i].right   = sib.left + sib.width;
    (*hotspots)[i].bottom  = sib.top  + sib.height;
    (*hotspots)[i].IsVisible = ((sib.hotspotType & 4) == 0);
    (*hotspots)[i].szHlpTopic_Macro[0] = '\0';
  }
  // we have it...now read-off the macro-string block
  if(nMacroStrings > 0)
    fseek( fSHG, nMacroStrings, SEEK_CUR);  //nMacroStrings is byte offset...
  // and, at the last, read through the strings: hotspot-id[ignored], then topic/macro
  int c;
  for( i = 0 ; i < nHotspots ; ++i)
  {
    while( (c = fgetc( fSHG)) != 0)
      ;
    // now read it:
    int j = 0;
    while( (c = fgetc( fSHG)) != 0)
    {
      (*hotspots)[i].szHlpTopic_Macro[j] = (wxChar)c;
      ++j;
    }
    (*hotspots)[i].szHlpTopic_Macro[j] = 0;
  }
  fclose( fSHG);
  return nHotspots;
}


// Convert Windows .SHG file to HTML map file

bool SHGToMap(wxChar *filename, wxChar *defaultFile)
{
  // Test the SHG parser
  HotSpot *hotspots = NULL;
  int n = ParseSHG(filename, &hotspots);
  if (n == 0)
    return false;

  wxChar buf[100];
  wxSnprintf(buf, sizeof(buf), _T("Converting .SHG file to HTML map file: there are %d hotspots in %s."), n, filename);
  OnInform(buf);

  wxChar outBuf[256];
  wxStrcpy(outBuf, filename);
  StripExtension(outBuf);
  wxStrcat(outBuf, _T(".map"));

  FILE *fd = wxFopen(outBuf, _T("w"));
  if (!fd)
  {
    OnError(_T("Could not open .map file for writing."));
    delete[] hotspots;
    return false;
  }

  wxFprintf(fd, _T("default %s\n"), defaultFile);
  for (int i = 0; i < n; i++)
  {
    wxChar *refFilename = _T("??");
    
    TexRef *texRef = FindReference(hotspots[i].szHlpTopic_Macro);
    if (texRef)
      refFilename = texRef->refFile;
    else
    {
      wxChar buf[300];
      wxSnprintf(buf, sizeof(buf), _T("Warning: could not find hotspot reference %s"), hotspots[i].szHlpTopic_Macro);
      OnInform(buf);
    }
    wxFprintf(fd, _T("rect %s %d %d %d %d\n"), refFilename, (int)hotspots[i].left, (int)hotspots[i].top,
      (int)hotspots[i].right, (int)hotspots[i].bottom);
  }
  wxFprintf(fd, _T("\n"));

  fclose(fd);

  delete[] hotspots;
  return true;
}


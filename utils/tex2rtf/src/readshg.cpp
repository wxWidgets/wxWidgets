/////////////////////////////////////////////////////////////////////////////
// Name:        readshg.cpp
// Purpose:     Petr Smilauer's .SHG (Segmented Hypergraphics file) reading
//              code.
//              Note: .SHG is undocumented (anywhere!) so this is
//              reverse-engineering
//              and guesswork at its best.
// Author:      Petr Smilauer
// Modified by:
// Created:     01/01/99
// RCS-ID:      $Id$
// Copyright:   (c) Petr Smilauer
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

#include <stdio.h>
#include <stdlib.h>

#include "readshg.h"
#include "tex2any.h"

// Returns the number of hotspots, and the array of hotspots.
// E.g.
// HotSpots *array;
// int n = ParseSHG("thing.shg", &array);

int   ParseSHG( const char* fileName, HotSpot **hotspots)
{ FILE*   fSHG = fopen( fileName, "rb");
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
  int  c;
  for( i = 0 ; i < nHotspots ; ++i)
  {
    while( (c = fgetc( fSHG)) != 0)
      ;
    // now read it:
    int j = 0;
    while( (c = fgetc( fSHG)) != 0)
    {
      (*hotspots)[i].szHlpTopic_Macro[j] = c;
      ++j;
    }
    (*hotspots)[i].szHlpTopic_Macro[j] = 0;
  }
  fclose( fSHG);
  return nHotspots;
}


// Convert Windows .SHG file to HTML map file

bool SHGToMap(char *filename, char *defaultFile)
{
  // Test the SHG parser
  HotSpot *hotspots = NULL;
  int n = ParseSHG(filename, &hotspots);
  if (n == 0)
    return FALSE;

  char buf[100];
  sprintf(buf, "Converting .SHG file to HTML map file: there are %d hotspots in %s.", n, filename);
  OnInform(buf);

  char outBuf[256];
  strcpy(outBuf, filename);
  StripExtension(outBuf);
  strcat(outBuf, ".map");

  FILE *fd = fopen(outBuf, "w");
  if (!fd)
  {
    OnError("Could not open .map file for writing.");
    delete[] hotspots;
    return FALSE;
  }

  fprintf(fd, "default %s\n", defaultFile);
  for (int i = 0; i < n; i++)
  {
    char *refFilename = "??";
    
    TexRef *texRef = FindReference(hotspots[i].szHlpTopic_Macro);
    if (texRef)
      refFilename = texRef->refFile;
    else
    {
      char buf[300];
      sprintf(buf, "Warning: could not find hotspot reference %s", hotspots[i].szHlpTopic_Macro);
      OnInform(buf);
    }
    fprintf(fd, "rect %s %d %d %d %d\n", refFilename, (int)hotspots[i].left, (int)hotspots[i].top,
      (int)hotspots[i].right, (int)hotspots[i].bottom);
  }
  fprintf(fd, "\n");

  fclose(fd);

  delete[] hotspots;
  return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// Name:        dib.h
// Purpose:     Routines for loading and saving DIBs
// Author:      Various
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// Save (device dependent) wxBitmap as a DIB
bool             wxSaveBitmap(char *filename, wxBitmap *bitmap, wxColourMap *colourmap = NULL);

// Load device independent bitmap into device dependent bitmap
wxBitmap         *wxLoadBitmap(char *filename, wxColourMap **colourmap = NULL);

// Load into existing bitmap;
bool wxLoadIntoBitmap(char *filename, wxBitmap *bitmap, wxColourMap **pal = NULL);

HANDLE BitmapToDIB (HBITMAP hBitmap, HPALETTE hPal);
BOOL   ReadDIB(LPSTR lpFileName, HBITMAP *bitmap, HPALETTE *palette);
HANDLE ReadDIB2(LPSTR lpFileName);
LPSTR FindDIBBits (LPSTR lpbi);
HPALETTE MakeDIBPalette(LPBITMAPINFOHEADER lpInfo);


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
bool             wxSaveBitmap(wxChar *filename, wxBitmap *bitmap, wxColourMap *colourmap = NULL);

// Load device independent bitmap into device dependent bitmap
wxBitmap         *wxLoadBitmap(wxChar *filename, wxColourMap **colourmap = NULL);

// Load into existing bitmap;
bool wxLoadIntoBitmap(wxChar *filename, wxBitmap *bitmap, wxColourMap **pal = NULL);

HANDLE BitmapToDIB (HBITMAP hBitmap, HPALETTE hPal);
BOOL   ReadDIB(LPTSTR lpFileName, HBITMAP *bitmap, HPALETTE *palette);
HANDLE ReadDIB2(LPTSTR lpFileName);
LPSTR FindDIBBits (LPTSTR lpbi);
HPALETTE MakeDIBPalette(LPBITMAPINFOHEADER lpInfo);


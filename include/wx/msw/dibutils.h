/////////////////////////////////////////////////////////////////////////////
// Name:        dibutils.h
// Purpose:     Utilities for DIBs
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Microsoft, Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/***************************************************************************

    (C) Copyright 1994 Microsoft Corp.  All rights reserved.

    You have a royalty-free right to use, modify, reproduce and 
    distribute the Sample Files (and/or any modified version) in 
    any way you find useful, provided that you agree that 
	 Microsoft has no warranty obligations or liability for any
    Sample Application Files which are modified. 

 **************************************************************************/

/***************************************************************************
    Functions for handling Device Independent Bitmaps and clearing the 
	 System Palette.
 **************************************************************************/

#ifndef SAMPLES_UTILS_H  
#define SAMPLES_UTILS_H

#ifdef __GNUG__
#pragma interface "dibutils.h"
#endif

typedef     LPBITMAPINFOHEADER PDIB;
typedef     HANDLE             HDIB;

/***************************************************************************
   External function declarations
 **************************************************************************/

void        wxClearSystemPalette(void);
PDIB        wxDibOpenFile(LPTSTR szFile);  
int 			wxDibWriteFile(LPTSTR szFile, LPBITMAPINFOHEADER lpbi);
BOOL        wxDibSetUsage(PDIB pdib, HPALETTE hpal,UINT wUsage);
PDIB        wxDibCreate(int bits, int dx, int dy);
BOOL        wxDibMapToPalette(PDIB pdib, HPALETTE hpal);
HPALETTE wxMakePalette(const BITMAPINFO FAR* Info, UINT flags);

/****************************************************************************
   Internal function declarations
 ***************************************************************************/

PDIB        wxDibReadBitmapInfo(HFILE fh);

/****************************************************************************
   DIB macros.
 ***************************************************************************/

#ifdef  WIN32
	 #define wxHandleFromDib(lpbi) GlobalHandle(lpbi)
#else
    #define wxHandleFromDib(lpbi) (HANDLE)GlobalHandle(SELECTOROF(lpbi))
#endif

#define wxDibFromHandle(h)        (PDIB)GlobalLock(h)

#define wxDibFree(pdib)           GlobalFreePtr(pdib)

#define wxWIDTHBYTES(i)           ((unsigned)((i+31)&(~31))/8)  /* ULONG aligned ! */

#define wxDibWidth(lpbi)          (UINT)(((LPBITMAPINFOHEADER)(lpbi))->biWidth)
#define wxDibHeight(lpbi)         (UINT)(((LPBITMAPINFOHEADER)(lpbi))->biHeight)
#define wxDibBitCount(lpbi)       (UINT)(((LPBITMAPINFOHEADER)(lpbi))->biBitCount)
#define wxDibCompression(lpbi)    (DWORD)(((LPBITMAPINFOHEADER)(lpbi))->biCompression)

#define wxDibWidthBytesN(lpbi, n) (UINT)wxWIDTHBYTES((UINT)(lpbi)->biWidth * (UINT)(n))
#define wxDibWidthBytes(lpbi)     wxDibWidthBytesN(lpbi, (lpbi)->biBitCount)

#define wxDibSizeImage(lpbi)      ((lpbi)->biSizeImage == 0 \
                                    ? ((DWORD)(UINT)wxDibWidthBytes(lpbi) * (DWORD)(UINT)(lpbi)->biHeight) \
                                    : (lpbi)->biSizeImage)

#define wxDibSize(lpbi)           ((lpbi)->biSize + (lpbi)->biSizeImage + (int)(lpbi)->biClrUsed * sizeof(RGBQUAD))
#define wxDibPaletteSize(lpbi)    (wxDibNumColors(lpbi) * sizeof(RGBQUAD))

#define wxDibFlipY(lpbi, y)       ((int)(lpbi)->biHeight-1-(y))

//HACK for NT BI_BITFIELDS DIBs
#ifdef WIN32
    #define wxDibPtr(lpbi)            ((lpbi)->biCompression == BI_BITFIELDS \
                                       ? (LPVOID)(wxDibColors(lpbi) + 3) \
                                       : (LPVOID)(wxDibColors(lpbi) + (UINT)(lpbi)->biClrUsed))
#else
	 #define wxDibPtr(lpbi)            (LPVOID)(wxDibColors(lpbi) + (UINT)(lpbi)->biClrUsed)
#endif

#define wxDibColors(lpbi)         ((RGBQUAD FAR *)((LPBYTE)(lpbi) + (int)(lpbi)->biSize))

#define wxDibNumColors(lpbi)      ((lpbi)->biClrUsed == 0 && (lpbi)->biBitCount <= 8 \
                                    ? (int)(1 << (int)(lpbi)->biBitCount)          \
                                    : (int)(lpbi)->biClrUsed)

#define wxDibXYN(lpbi,pb,x,y,n)   (LPVOID)(                                     \
                                (BYTE _huge *)(pb) +                          \
                                (UINT)((UINT)(x) * (UINT)(n) / 8u) +          \
                                ((DWORD)wxDibWidthBytesN(lpbi,n) * (DWORD)(UINT)(y)))

#define wxDibXY(lpbi,x,y)         wxDibXYN(lpbi,wxDibPtr(lpbi),x,y,(lpbi)->biBitCount)

#define wxFixBitmapInfo(lpbi)     if ((lpbi)->biSizeImage == 0)                 \
												(lpbi)->biSizeImage = wxDibSizeImage(lpbi); \
                                if ((lpbi)->biClrUsed == 0)                   \
                                    (lpbi)->biClrUsed = wxDibNumColors(lpbi);

//                                if ((lpbi)->biCompression == BI_BITFIELDS && (lpbi)->biClrUsed == 0)
//                                    ; // (lpbi)->biClrUsed = 3;                    

#define wxDibInfo(pDIB)     ((BITMAPINFO FAR *)(pDIB))

/***************************************************************************/

#ifndef BI_BITFIELDS
  #define BI_BITFIELDS 3
#endif  

#ifndef HALFTONE
  #define HALFTONE COLORONCOLOR
#endif

#endif

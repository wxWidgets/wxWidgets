/////////////////////////////////////////////////////////////////////////////
// Name:        dib.cpp
// Purpose:     Routines for dealing with Device Independent Bitmaps.
// Author:
// Modified by: Jason Quijano 06/06/2001
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
/////////////////////////////////////////////////////////////////////////////
// FUNCTIONS :
//      wxReadDIB()         - Reads a DIB
//      WriteDIB()          - Writes a global handle in CF_DIB format to a file.
//      wxPaletteSize()     - Calculates the palette size in bytes of given DIB                                                                               *
//      DibNumColors()      - Determines the number of colors in DIB
//      DibFromBitmap()     - Creates a DIB repr. the DDB passed in.
//      lread()             - Private routine to read more than 64k
//      lwrite()            - Private routine to write more than 64k
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/bitmap.h"
#include "wx/log.h"
#include "wx/intl.h"
#endif

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#if !defined(__MWERKS__) && !defined(__SALFORDC__)
#include <memory.h>
#endif

#include "wx/msw/dib.h"
#include "wx/filesys.h"

#ifdef __GNUWIN32_OLD__
    #include "wx/msw/gnuwin32/extra.h"
#endif

#ifndef SEEK_CUR
// flags for _lseek
#define        SEEK_CUR 1
#define        SEEK_END 2
#define        SEEK_SET 0
#endif

// Number of bytes to be read during each read operation.
#define MAXREAD  32768

// Header signatutes for various resources
#define BFT_ICON 0x4349                 // 'IC'
#define BFT_BITMAP 0x4d42               // 'BM'
#define BFT_CURSOR 0x5450               // 'PT('

// macro to determine if resource is a DIB
#define ISDIB(bft)      ((bft) == BFT_BITMAP)

// Macro to align given value to the closest DWORD (unsigned long )
#define ALIGNULONG(i)   ((i+3)/4*4)

//  Macro to determine to round off the given value to the closest byte
#define WIDTHBYTES(i)   ((i+31)/32*4)

#define PALVERSION 0x300
#define MAXPALETTE 256                  // max. # supported palette entries

static DWORD PASCAL lwrite(int fh, VOID FAR *pv, DWORD ul);

static BOOL WriteDIB (LPTSTR szFile,HANDLE hdib);
WORD wxPaletteSize (VOID FAR * pv);     // This is non-static as some apps use it externally
static WORD DibNumColors (VOID FAR * pv);
// HANDLE DibFromBitmap (HBITMAP hbm, DWORD biStyle, WORD biBits, HPALETTE hpal);
static BOOL PASCAL MakeBitmapAndPalette(HDC,HANDLE,HPALETTE *,HBITMAP *);

// ----------------------------------------------------------------------------
// FUNCTION : WriteDIB(LPSTR szFile,HANDLE hdib)
// PURPOSE  : Write a global handle in CF_DIB format to a file.
// RETURNS  : TRUE  - if successful. FALSE - otherwise
// ----------------------------------------------------------------------------
static BOOL WriteDIB(
    LPTSTR szFile,
    HANDLE hdib )
{
    BITMAPFILEHEADER hdr;
    LPBITMAPINFOHEADER lpbi;
    int fh;
    OFSTRUCT of;

    if (!hdib)
        return FALSE;
    fh = OpenFile(wxConvertWX2MB(szFile), &of, OF_CREATE | OF_READWRITE);
    if (fh == -1)
        return FALSE;

#ifdef __WINDOWS_386__
    lpbi = (LPBITMAPINFOHEADER) MK_FP32(GlobalLock(hdib));
#else
    lpbi = (LPBITMAPINFOHEADER) GlobalLock(hdib);
#endif
    // Fill in the fields of the file header
    hdr.bfType = BFT_BITMAP;
    hdr.bfSize = GlobalSize(hdib) + sizeof(BITMAPFILEHEADER);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + lpbi->biSize + wxPaletteSize(lpbi);

    // Write the file header
    _lwrite(fh, (LPSTR) &hdr, sizeof(BITMAPFILEHEADER));

    //Write the DIB header and the bits
    lwrite(fh, (LPSTR) lpbi, GlobalSize(hdib));

    GlobalUnlock(hdib);
    _lclose(fh);
    return TRUE;
}

// ----------------------------------------------------------------------------
// FUNCTION : wxPaletteSize(VOID FAR * pv)
// PURPOSE  : Calculates the palette size in bytes. If the info. block
//            is of the BITMAPCOREHEADER type, the number of colors is
//            multiplied by 3 to give the palette size, otherwise the
//            number of colors is multiplied by 4.
// RETURNS  : Palette size in number of bytes.
// ----------------------------------------------------------------------------
WORD wxPaletteSize(
    VOID FAR * pv )
{
    LPBITMAPINFOHEADER lpbi;
    WORD NumColors;

    lpbi = (LPBITMAPINFOHEADER) pv;
    NumColors = DibNumColors(lpbi);

    if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
        return (WORD)(NumColors * sizeof(RGBTRIPLE));
    else
        return (WORD)(NumColors * sizeof(RGBQUAD));
}

// ----------------------------------------------------------------------------
// FUNCTION : DibNumColors(VOID FAR * pv)
// PURPOSE  : Determines the number of colors in the DIB by looking at
//            the BitCount filed in the info block.
// RETURNS  : The number of colors in the DIB.
// ----------------------------------------------------------------------------
static WORD DibNumColors(
    VOID FAR *pv )
{
    int bits;
    BITMAPINFOHEADER *lpbi;
    BITMAPCOREHEADER *lpbc;
    lpbi = ((BITMAPINFOHEADER*) pv);
    lpbc = ((BITMAPCOREHEADER*) pv);

    // With the BITMAPINFO format headers, the size of the palette
    // is in biClrUsed, whereas in the BITMAPCORE - style headers, it
    // is dependent on the bits per pixel ( = 2 raised to the power of
    // bits/pixel).

    if (lpbi->biSize != sizeof(BITMAPCOREHEADER)) {
        if (lpbi->biClrUsed != 0)
            return (WORD) lpbi->biClrUsed;
        bits = lpbi->biBitCount;
        }
    else
        bits = lpbc->bcBitCount;

    switch (bits) {
        case 1:
            return 2;
        case 4:
            return 16;
        case 8:
            return 256;
        default:
            // A 24 bitcount DIB has no color table
            return 0;
        }
}

// ----------------------------------------------------------------------------
// FUNCTION : DibFromBitmap()
// PURPOSE  : Will create a global memory block in DIB format that
//            represents the Device-dependent bitmap (DDB) passed in.
// RETURNS  : A handle to the DIB
// ----------------------------------------------------------------------------
#if NOTHING
static HANDLE DibFromBitmap(
    HBITMAP hbm,
    DWORD biStyle,
    WORD biBits,
    HPALETTE hpal)
{
    BITMAP bm;
    BITMAPINFOHEADER bi;
    BITMAPINFOHEADER FAR *lpbi;
    DWORD dwLen;
    HANDLE hdib;
    HANDLE h;
    HDC hdc;

    if (!hbm)
        return NULL;
    if (hpal == NULL)
        hpal = GetStockObject(DEFAULT_PALETTE);

    GetObject(hbm, sizeof (bm), (LPSTR) &bm);

    if (biBits == 0)
        biBits = bm.bmPlanes * bm.bmBitsPixel;

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bm.bmWidth;
    bi.biHeight = bm.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = biBits;
    bi.biCompression = biStyle;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    dwLen = bi.biSize + wxPaletteSize(&bi);

    hdc = GetDC((HWND) NULL);
    hpal = SelectPalette(hdc, hpal, FALSE);
    RealizePalette(hdc);

    hdib = GlobalAlloc(GHND, dwLen);

    if (!hdib) {
        SelectPalette(hdc, hpal, FALSE);
        ReleaseDC(NULL, hdc);
        return NULL;
        }

#ifdef __WINDOWS_386__
    lpbi = (BITMAPINFOHEADER FAR *) MK_FP32(GlobalLock(hdib));
#else
    lpbi = (BITMAPINFOHEADER FAR *) GlobalLock(hdib);
#endif

    *lpbi = bi;

    // call GetDIBits with a NULL lpBits param, so it will calculate the
    // biSizeImage field for us
    GetDIBits(hdc, hbm, 0, (WORD) bi.biHeight, NULL, (LPBITMAPINFO) lpbi, DIB_RGB_COLORS);
    bi = *lpbi;
    GlobalUnlock(hdib);

    // If the driver did not fill in the biSizeImage field, make one up
    if (bi.biSizeImage == 0) {
        bi.biSizeImage = WIDTHBYTES((DWORD)bm.bmWidth * biBits) * bm.bmHeight;
        if (biStyle != BI_RGB)
            bi.biSizeImage = (bi.biSizeImage * 3) / 2;
        }

    // realloc the buffer big enough to hold all the bits
    dwLen = bi.biSize + wxPaletteSize(&bi) + bi.biSizeImage;
    if (h = GlobalReAlloc(hdib, dwLen, 0))
        hdib = h;
    else {
        GlobalFree(hdib);
        hdib = NULL;
        SelectPalette(hdc, hpal, FALSE);
        ReleaseDC(NULL, hdc);
        return hdib;
        }

    // call GetDIBits with a NON-NULL lpBits param, and actualy get the
    // bits this time

#ifdef __WINDOWS_386__
        lpbi = (BITMAPINFOHEADER FAR *) MK_FP32(GlobalLock(hdib));
#else
        lpbi = (BITMAPINFOHEADER FAR *) GlobalLock(hdib);
#endif

    if (GetDIBits(hdc,
            hbm,
            0,
            (WORD) bi.biHeight,
            (LPSTR) lpbi + (WORD) lpbi->biSize + wxPaletteSize(lpbi),
            (LPBITMAPINFO) lpbi, DIB_RGB_COLORS) == 0) {
        GlobalUnlock(hdib);
        hdib = NULL;
        SelectPalette(hdc, hpal, FALSE);
        ReleaseDC((HWND) NULL, hdc);
        return NULL;
        }

        bi = *lpbi;
        GlobalUnlock(hdib);
        SelectPalette(hdc, hpal, FALSE);
        ReleaseDC(NULL, hdc);
        return hdib;
}
#endif


//************* PRIVATE ROUTINES TO WRITE MORE THAN 64K ******************
// ----------------------------------------------------------------------------
// FUNCTION   : lwrite(int fh, VOID FAR *pv, DWORD ul)
// PURPOSE    : Writes data in steps of 32k till all the data is written.
// RETURNS    : 0 - If write did not proceed correctly. number of bytes written otherwise.
// ----------------------------------------------------------------------------
static DWORD PASCAL lwrite(
    int fh,
    VOID FAR *pv,
    DWORD ul)
{
    DWORD ulT = ul;
#if defined(WINNT) || defined(__WIN32__) || defined(__WIN32__) || defined(__WXWINE__)
    BYTE *hp = (BYTE *) pv;
#else
    BYTE huge *hp = (BYTE huge *) pv;
#endif
    while (ul > MAXREAD) {
        if (_lwrite(fh, (LPSTR) hp, (WORD) MAXREAD) != MAXREAD)
            return 0;
        ul -= MAXREAD;
        hp += MAXREAD;
        }
    if (_lwrite(fh, (LPSTR) hp, (WXUINT) ul) != (WXUINT) ul)
        return 0;
    return ulT;
}

// ----------------------------------------------------------------------------
// FUNCTION : ReadDIB(hWnd)
// PURPOSE  : Reads a DIB from a file, obtains a handle to its
//            BITMAPINFO struct. and loads the DIB.  Once the DIB
//            is loaded, the function also creates a bitmap and
//            palette out of the DIB for a device-dependent form.
// RETURNS  : TRUE  - DIB loaded and bitmap/palette created
//            The DIBINIT structure pointed to by pInfo is
//            filled with the appropriate handles.
//            FALSE - otherwise
// ----------------------------------------------------------------------------
BOOL wxReadDIB(
    LPTSTR lpFileName,
    HBITMAP *bitmap,
    HPALETTE *palette)
{
    LPBITMAPINFOHEADER lpbi;
    BITMAPFILEHEADER bf;
    WORD nNumColors;
    BOOL result = FALSE;
    WORD offBits;
    HDC hDC;
    BOOL bCoreHead = FALSE;
    HANDLE hDIB = 0;

    // JQ: We want to use wxFileSystem here in stead of Openfile so
    // that we can use other FS types like a zip files.
    wxFileSystem fsys;
    wxFSFile *file = fsys.OpenFile(lpFileName);
    wxInputStream *dibs = file->GetStream();
    if (!dibs) {
        wxLogError(_("Can't open file '%s'"), lpFileName);
        return (0);
        }

    hDIB = GlobalAlloc(GHND, (DWORD)(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)));
    if (!hDIB)
        return(0);

#ifdef __WINDOWS_386__
    lpbi = (LPBITMAPINFOHEADER)MK_FP32(GlobalLock(hDIB));
#else
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIB);
#endif

    // JQ: read from the wxInputStream dibs instead of fh so
    // that we can use other FS types like zip files.
    dibs->Read((LPSTR)&bf,sizeof(bf));
    if (sizeof(bf) != dibs->LastRead())
        goto ErrExit;

    if (bf.bfType != 0x4d42)                        // 'BM'
        goto ErrExit;

    // JQ: read from the wxInputStream dibs instead of fh so
    // that we can use other FS types like zip files.
    dibs->Read((LPSTR)lpbi,sizeof(BITMAPCOREHEADER));
    if (sizeof(BITMAPCOREHEADER) != dibs->LastRead())
        goto ErrExit;

    if (lpbi->biSize == sizeof(BITMAPCOREHEADER)){
        lpbi->biSize = sizeof(BITMAPINFOHEADER);
        lpbi->biBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;
        lpbi->biPlanes = ((LPBITMAPCOREHEADER)lpbi)->bcPlanes;
        lpbi->biHeight = ((LPBITMAPCOREHEADER)lpbi)->bcHeight;
        lpbi->biWidth = ((LPBITMAPCOREHEADER)lpbi)->bcWidth;
        bCoreHead = TRUE;
        }
    else {

        // JQ: Get to the start of the header and read INFOHEADER
        // using dibs wxInputStream
        dibs->SeekI(sizeof(BITMAPFILEHEADER),wxFromStart);
        if (dibs->LastError() != wxSTREAM_NO_ERROR)
            goto ErrExit;

        // JQ: read from the wxInputStream dibs instead of fh so
        // that we can use other FS types like zip files.
        // Can I do error checking with this?
        dibs->Read((LPSTR)lpbi,sizeof(BITMAPINFOHEADER));
        if (sizeof(BITMAPINFOHEADER) != dibs->LastRead())
            goto ErrExit;

        }

    nNumColors = (WORD)lpbi->biClrUsed;
    if ( nNumColors == 0 ) {
        // no color table for 24-bit, default size otherwise
        if (lpbi->biBitCount != 24)
            nNumColors = 1 << lpbi->biBitCount;     // standard size table
        }

    // fill in some default values if they are zero
    if (lpbi->biClrUsed == 0)
        lpbi->biClrUsed = nNumColors;

    if (lpbi->biSizeImage == 0){
        lpbi->biSizeImage = ((((lpbi->biWidth * (DWORD)lpbi->biBitCount) + 31) & ~31) >> 3)
                        * lpbi->biHeight;
        }

    // get a proper-sized buffer for header, color table and bits
    GlobalUnlock(hDIB);
    hDIB = GlobalReAlloc(hDIB, lpbi->biSize + nNumColors * sizeof(RGBQUAD) +
                        lpbi->biSizeImage, 0);

    // can't resize buffer for loading
    if (!hDIB)
        goto ErrExit2;

#ifdef __WINDOWS_386__
    lpbi = (LPBITMAPINFOHEADER)MK_FP32(GlobalLock(hDIB));
#else
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIB);
#endif

    // read the color table
    if (!bCoreHead)
        dibs->Read((LPSTR)lpbi + lpbi->biSize, nNumColors * sizeof(RGBQUAD));
    else {
        signed int i;
        RGBQUAD FAR *pQuad;
        RGBTRIPLE FAR *pTriple;
        dibs->Read((LPSTR)lpbi + lpbi->biSize, nNumColors * sizeof(RGBTRIPLE));
        pQuad = (RGBQUAD FAR *)((LPSTR)lpbi + lpbi->biSize);
        pTriple = (RGBTRIPLE FAR *) pQuad;
        for (i = nNumColors - 1; i >= 0; i--){
            pQuad[i].rgbRed = pTriple[i].rgbtRed;
            pQuad[i].rgbBlue = pTriple[i].rgbtBlue;
            pQuad[i].rgbGreen = pTriple[i].rgbtGreen;
            pQuad[i].rgbReserved = 0;
            }
        }

    // offset to the bits from start of DIB header
    offBits = (WORD)(lpbi->biSize + nNumColors * sizeof(RGBQUAD));

    if (bf.bfOffBits != 0L)
        dibs->SeekI(bf.bfOffBits, wxFromStart);

    dibs->Read((LPSTR)lpbi + offBits,lpbi->biSizeImage);
    if (lpbi->biSizeImage == dibs->LastRead())
    {
        GlobalUnlock(hDIB);
        hDC = GetDC(NULL);
        if (!MakeBitmapAndPalette(hDC, hDIB, palette, bitmap))
        {
            ReleaseDC(NULL,hDC);
            goto ErrExit2;
        }
        else
        {
            ReleaseDC(NULL,hDC);
            GlobalFree(hDIB);
            result = TRUE;
        }
    }
    else
    {
ErrExit:
        GlobalUnlock(hDIB);
ErrExit2:
        GlobalFree(hDIB);
    }
    return(result);
}

// ----------------------------------------------------------------------------
// FUNCTION : MakeBitmapAndPalette
// PURPOSE  : Given a DIB, creates a bitmap and corresponding palette
//            to be used for a device-dependent representation of
//            of the image.
// RETURNS  : TRUE  --> success. phPal and phBitmap are filled with
//            appropriate handles.  Caller is responsible for freeing objects.
//            FALSE --> unable to create objects.  both pointer are not valid
// ----------------------------------------------------------------------------
static BOOL PASCAL MakeBitmapAndPalette(
    HDC hDC,
    HANDLE hDIB,
    HPALETTE * phPal,
    HBITMAP * phBitmap)
{
    LPBITMAPINFOHEADER lpInfo;
    BOOL result = FALSE;
    HBITMAP hBitmap;
    HPALETTE hPalette, hOldPal;
    LPSTR lpBits;

#ifdef __WINDOWS_386__
    lpInfo = (LPBITMAPINFOHEADER) MK_FP32(GlobalLock(hDIB));
#else
    lpInfo = (LPBITMAPINFOHEADER) GlobalLock(hDIB);
#endif

    hPalette = wxMakeDIBPalette(lpInfo);
    if ( hPalette )
    {
        // Need to realize palette for converting DIB to bitmap.
        hOldPal = SelectPalette(hDC, hPalette, TRUE);
        RealizePalette(hDC);

        lpBits = (LPSTR)lpInfo + (WORD)lpInfo->biSize +
                (WORD)lpInfo->biClrUsed * sizeof(RGBQUAD);
        hBitmap = CreateDIBitmap(hDC, lpInfo, CBM_INIT, lpBits,
                                (LPBITMAPINFO)lpInfo, DIB_RGB_COLORS);

        SelectPalette(hDC, hOldPal, TRUE);
        RealizePalette(hDC);

        if (!hBitmap)
            DeleteObject(hPalette);
        else
        {
            *phBitmap = hBitmap;
            *phPal = hPalette;
            result = TRUE;
        }
    }

	GlobalUnlock (hDIB);  // glt
    return(result);
}

// ----------------------------------------------------------------------------
// FUNCTION : wxMakeDIBPalette(lpInfo)
// PURPOSE  : Given a BITMAPINFOHEADER, create a palette based on
//            the color table.
// RETURNS  : non-zero - handle of a corresponding palette
//            zero - unable to create palette
// ----------------------------------------------------------------------------
HPALETTE wxMakeDIBPalette(
    LPBITMAPINFOHEADER lpInfo )
{
#ifdef __WXWINE__
    return (FALSE);
#else
    LPLOGPALETTE npPal;
    RGBQUAD far *lpRGB;
    HPALETTE hLogPal;
    WORD i;

    // since biClrUsed field was filled during the loading of the DIB,
    // we know it contains the number of colors in the color table.
    if (lpInfo->biClrUsed)
    {
/*
        npPal = (NPLOGPALETTE)LocalAlloc(LMEM_FIXED, sizeof(LOGPALETTE) +
                                (WORD)lpInfo->biClrUsed * sizeof(PALETTEENTRY));
*/
        npPal = (LPLOGPALETTE)malloc(sizeof(LOGPALETTE) +
                                (WORD)lpInfo->biClrUsed * sizeof(PALETTEENTRY));
        if (!npPal)
            return(FALSE);

        npPal->palVersion = 0x300;
        npPal->palNumEntries = (WORD)lpInfo->biClrUsed;

        // get pointer to the color table
        lpRGB = (RGBQUAD FAR *)((LPSTR)lpInfo + lpInfo->biSize);

        // copy colors from the color table to the LogPalette structure
        for (i = 0; (DWORD)i < lpInfo->biClrUsed; i++, lpRGB++)
        {
            npPal->palPalEntry[i].peRed = lpRGB->rgbRed;
            npPal->palPalEntry[i].peGreen = lpRGB->rgbGreen;
            npPal->palPalEntry[i].peBlue = lpRGB->rgbBlue;
            npPal->palPalEntry[i].peFlags = 0;
        }

        hLogPal = CreatePalette((LPLOGPALETTE)npPal);
//        LocalFree((HANDLE)npPal);
        free(npPal);
        return(hLogPal);
    }

    // 24-bit DIB with no color table.  return default palette.  Another
    // option would be to create a 256 color "rainbow" palette to provide
    // some good color choices.
    else
        return((HPALETTE) GetStockObject(DEFAULT_PALETTE));
#endif
}

// ----------------------------------------------------------------------------
// FUNCTION :
// PURPOSE  :
// RETURNS  :
// ----------------------------------------------------------------------------
bool wxLoadIntoBitmap(
    wxChar *filename,
    wxBitmap *bitmap,
    wxPalette **pal)
{
    HBITMAP hBitmap;
    HPALETTE hPalette;

    bool success = (wxReadDIB(filename, &hBitmap, &hPalette) != 0);
    if (!success)
    {
        DeleteObject(hPalette);
        return FALSE;
    }

    if (hPalette)
    {
        if (pal)
        {
            *pal = new wxPalette;
            (*pal)->SetHPALETTE((WXHPALETTE) hPalette);
        }
        else
            DeleteObject(hPalette);
    }
    else if (pal)
        *pal = NULL;

    if (hBitmap)
    {
        BITMAP bm;
        GetObject(hBitmap, sizeof(bm), (LPSTR)&bm);

        bitmap->SetHBITMAP((WXHBITMAP) hBitmap);
        bitmap->SetWidth(bm.bmWidth);
        bitmap->SetHeight(bm.bmHeight);
        bitmap->SetDepth(bm.bmPlanes * bm.bmBitsPixel);
#if WXWIN_COMPATIBILITY_2
        bitmap->SetOk(TRUE);
#endif // WXWIN_COMPATIBILITY_2
        return TRUE;
    }
    else return FALSE;
}

// ----------------------------------------------------------------------------
// FUNCTION :
// PURPOSE  :
// RETURNS  :
// ----------------------------------------------------------------------------
wxBitmap *wxLoadBitmap(
    wxChar *filename,
    wxPalette **pal)
{
    wxBitmap *bitmap = new wxBitmap;
    if (wxLoadIntoBitmap(filename, bitmap, pal))
        return bitmap;
    else
    {
        delete bitmap;
        return NULL;
    }
}

//---------------------------------------------------------------------
// FUNCTION : InitBitmapInfoHeader
// PURPOSE  : Does a "standard" initialization of a BITMAPINFOHEADER,
//            given the Width, Height, and Bits per Pixel for the DIB.
//
//            By standard, I mean that all the relevant fields are set
//            to the specified values.  biSizeImage is computed, the
//            biCompression field is set to "no compression," and all
//            other fields are 0.
//
//            Note that DIBs only allow BitsPixel values of 1, 4, 8, or
//            24.  This routine makes sure that one of these values is
//            used (whichever is most appropriate for the specified
//            nBPP).
//
// PARMS    : lpBmInfoHdr == Far pointer to a BITMAPINFOHEADER structure
//                            to be filled in.
//            dwWidth     == Width of DIB (not in Win 3.0 & 3.1, high
//                            word MUST be 0).
//            dwHeight    == Height of DIB (not in Win 3.0 & 3.1, high
//                            word MUST be 0).
//            nBPP        == Bits per Pixel for the DIB.
//
// History:   Date      Reason
//            11/07/91  Created
//
//---------------------------------------------------------------------
static void InitBitmapInfoHeader (
    LPBITMAPINFOHEADER lpBmInfoHdr,
    DWORD dwWidth,
    DWORD dwHeight,
    int nBPP )
{
//   _fmemset (lpBmInfoHdr, 0, sizeof (BITMAPINFOHEADER));
   memset (lpBmInfoHdr, 0, sizeof (BITMAPINFOHEADER));

   lpBmInfoHdr->biSize      = sizeof (BITMAPINFOHEADER);
   lpBmInfoHdr->biWidth     = dwWidth;
   lpBmInfoHdr->biHeight    = dwHeight;
   lpBmInfoHdr->biPlanes    = 1;

   if (nBPP <= 1)
      nBPP = 1;
   else if (nBPP <= 4)
      nBPP = 4;
   else if (nBPP <= 8)
      nBPP = 8;
/* Doesn't work
   else if (nBPP <= 16)
      nBPP = 16;
*/
   else
      nBPP = 24;

   lpBmInfoHdr->biBitCount  = nBPP;
   lpBmInfoHdr->biSizeImage = WIDTHBYTES (dwWidth * nBPP) * dwHeight;
}

// ----------------------------------------------------------------------------
// FUNCTION :
// PURPOSE  :
// RETURNS  :
// ----------------------------------------------------------------------------
LPSTR wxFindDIBBits (LPSTR lpbi)
{
   return (lpbi + *(LPDWORD)lpbi + wxPaletteSize (lpbi));
}

//---------------------------------------------------------------------
// Function:   BitmapToDIB
//
// Purpose:    Given a device dependent bitmap and a palette, returns
//             a handle to global memory with a DIB spec in it.  The
//             DIB is rendered using the colors of the palette passed in.
//
//             Stolen almost verbatim from ShowDIB.
//
// Parms:      hBitmap == Handle to device dependent bitmap compatible
//                        with default screen display device.
//             hPal    == Palette to render the DDB with.  If it's NULL,
//                        use the default palette.
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------
HANDLE wxBitmapToDIB (
    HBITMAP hBitmap,
    HPALETTE hPal)
{
    BITMAP             Bitmap;
    BITMAPINFOHEADER   bmInfoHdr;
    LPBITMAPINFOHEADER lpbmInfoHdr;
    LPSTR              lpBits;
    HDC                hMemDC;
    HANDLE             hDIB;
    HPALETTE           hOldPal = NULL;

    // Do some setup -- make sure the Bitmap passed in is valid,
    // get info on the bitmap (like its height, width, etc.),
    // then setup a BITMAPINFOHEADER.

   if (!hBitmap)
      return NULL;

   if (!GetObject (hBitmap, sizeof (Bitmap), (LPSTR) &Bitmap))
      return NULL;

   InitBitmapInfoHeader (&bmInfoHdr,
                         Bitmap.bmWidth,
                         Bitmap.bmHeight,
                         Bitmap.bmPlanes * Bitmap.bmBitsPixel);


    // Now allocate memory for the DIB.  Then, set the BITMAPINFOHEADER
    //  into this memory, and find out where the bitmap bits go.

    hDIB = GlobalAlloc (GHND, sizeof (BITMAPINFOHEADER) +
                        wxPaletteSize ((LPSTR) &bmInfoHdr) + bmInfoHdr.biSizeImage);

    if (!hDIB)
        return NULL;

#ifdef __WINDOWS_386__
    lpbmInfoHdr  = (LPBITMAPINFOHEADER) MK_FP32(GlobalLock (hDIB));
#else
    lpbmInfoHdr  = (LPBITMAPINFOHEADER) GlobalLock (hDIB);
#endif

    *lpbmInfoHdr = bmInfoHdr;
    lpBits       = wxFindDIBBits ((LPSTR) lpbmInfoHdr);

    // Now, we need a DC to hold our bitmap.  If the app passed us
    //  a palette, it should be selected into the DC.
    hMemDC       = GetDC (NULL);

    if (hPal)
    {
        hOldPal = SelectPalette (hMemDC, hPal, FALSE);
        RealizePalette (hMemDC);
    }

    // We're finally ready to get the DIB.  Call the driver and let
    //  it party on our bitmap.  It will fill in the color table,
    //  and bitmap bits of our global memory block.
    if (!GetDIBits (hMemDC,
                   hBitmap,
                   0,
                   Bitmap.bmHeight,
                   lpBits,
                   (LPBITMAPINFO) lpbmInfoHdr,
                   DIB_RGB_COLORS))
    {
        GlobalUnlock (hDIB);
        GlobalFree (hDIB);
        hDIB = NULL;
    }
    else
        GlobalUnlock (hDIB);

   // Finally, clean up and return.
   if (hOldPal)
        SelectPalette (hMemDC, hOldPal, FALSE);

   ReleaseDC (NULL, hMemDC);
   return hDIB;
}

// ----------------------------------------------------------------------------
// FUNCTION :
// PURPOSE  :
// RETURNS  :
// ----------------------------------------------------------------------------
bool wxSaveBitmap(
    wxChar *filename,
    wxBitmap *bitmap,
    wxPalette *colourmap)
{
    HPALETTE hPalette = 0;
    if (colourmap)
        hPalette = (HPALETTE) colourmap->GetHPALETTE();

    HANDLE dibHandle = wxBitmapToDIB((HBITMAP) bitmap->GetHBITMAP(), hPalette);
    if (dibHandle)
    {
        bool success = (WriteDIB(filename, dibHandle) != 0);
        GlobalFree(dibHandle);
        return success;
    }
        else return FALSE;
}



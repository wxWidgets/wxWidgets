//* Written by Microsoft Product Support Services, Windows Developer Support. *
//*****************************************************************************
//   (C) Copyright Microsoft Corp. 1993.  All rights reserved.
//   You have a royalty-free right to use, modify, reproduce and
//   distribute the Sample Files (and/or any modified version) in
//   any way you find useful, provided that you agree that
//   Microsoft has no warranty obligations or liability for any
//   Sample Application Files which are modified.

// Modified by Petr Smilauer, March 1994 for wxWin library purposes!

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <io.h>
#include <windows.h>

#if defined(__MWERKS__)
#include <wingdi.h>
#include <winuser.h>
#endif

#ifndef __TWIN32__
#if defined (__GNUWIN32__) && !defined(wxUSE_NORLANDER_HEADERS)
#include "wx/msw/gnuwin32/extra.h"
#endif
#endif

#include "wx/wxchar.h"
#include "wx/msw/curicop.h"
#include "wx/msw/curico.h"
#include "wx/string.h"

//*****************************************************************************
//* Function : ReadIconFile()                                                 *
//* Purpose  : Reads an icon resource file and creates an icon based on that  *
//*            information.                                                   *
//* Parameters : char *szFileName - The icon resource file.                   *
//* Returns :  A handle to an icon. The handle will be NULL if an icon cannot *
//*            be created for any reason.                                     *
//*****************************************************************************

HICON ReadIconFile( wxChar *szFileName, HINSTANCE hInst, int *W, int *H)
{ HICON   hIcon;
  HANDLE  hDIB;

  if( (hDIB = ReadIcon(szFileName, W, H)) == (HANDLE) NULL)
                                          // read the icon DIB from file
    return (HICON) NULL;
  hIcon = MakeIcon( hDIB, hInst);         // create an icon from DIB
  GlobalFree( hDIB);
  return hIcon;
}

//*****************************************************************************
//* Function : CursorToIcon()                                                 *
//* Purpose  : Reads a cursor resource file and creates an icon based on that *
//*            information.                                                   *
//* Parameters : char *szFileName - The cursor resource file.                 *
//* Returns :  A handle to an icon. The handle will be NULL if an icon cannot *
//*            be created for any reason.                                     *
//* Comments : A cursor is monochrome. So, the resulting icon will also be    *
//*            monochrome.                                                    *
//*****************************************************************************

HICON CursorToIcon( wxChar *szFileName, HINSTANCE hInst, int *W, int *H)
{ HANDLE  hDIB;     // Handle to DIB memory
  HICON   hIcon;    // Handle to Icon

  if( (hDIB = ReadCur( szFileName, NULL, W, H)) == (HANDLE) NULL)
                                                    // Read cursor DIB
    return (HICON) NULL;
  hIcon = MakeIcon( hDIB, hInst);      // make icon from cursor DIB
  GlobalFree( hDIB);
  return hIcon;
}

//*****************************************************************************
//* Function : ReadIcon()                                                     *
//* Purpose  : Reads an icon resource file and extracts the DIB information.  *
//* Parameters : char *szFileName - The icon resource file.                   *
//* Returns :  A handle to a DIB. The handle will be NULL if the resource file*
//*            is corrupt or if memory cannot be allocated for the DIB info.  *
//*****************************************************************************

HANDLE ReadIcon( wxChar *szFileName, int *W, int *H)
{ ICONFILEHEADER iconFileHead;   // ICON file header structure
  ICONFILERES    iconFileRes;    // ICON file resource
  WORD           cbHead,
                 cbRes,
                 cbBits;         // Used for reading in file
  int            hFile;          // File handle
  LPBITMAPINFO   lpDIB;          // Pointer to DIB memory
  HANDLE         hDIB;
  int            nWidth      = GetSystemMetrics( SM_CXICON),
                 nHeight     = GetSystemMetrics( SM_CYICON),
                 nDirEntries = 0;

   // Open and read the .ICO file header and the first ICONFILERES
  hFile  = _lopen( wxFNCONV(szFileName), OF_READ);
  cbHead = _lread( hFile, (LPSTR)&iconFileHead, sizeof(ICONFILEHEADER));
  cbRes  = _lread( hFile, (LPSTR)&iconFileRes, sizeof(ICONFILERES));
  ++nDirEntries;

  if((cbHead != sizeof( ICONFILEHEADER)) || (cbRes != sizeof( ICONFILERES)))
    return (HANDLE) NULL;
  // Verify that it's an .ICON file
  if( iconFileHead.wResourceType != 1)
    return (HANDLE) NULL;

  // inserted by P.S.
  while( ((unsigned)nDirEntries < iconFileHead.wResourceCount) &&
         ((iconFileRes.bWidth != nWidth) || (iconFileRes.bHeight != nHeight)))
  {
    cbRes = _lread( hFile, (LPSTR )&iconFileRes, sizeof( ICONFILERES));
    if(cbRes != sizeof( ICONFILERES))
      return (HANDLE) NULL;
    else
      ++nDirEntries;
  }

  if(W != 0)
    *W = iconFileRes.bWidth;
  if(H != 0)
    *H = iconFileRes.bHeight;

  // Allocate and lock memory to read in the DIB
  hDIB = GlobalAlloc(GHND, iconFileRes.dwDIBSize);
  if(hDIB == (HANDLE) NULL)
    return (HANDLE) NULL;

#ifdef __WINDOWS_386__
  lpDIB = (LPBITMAPINFO)MK_FP32(GlobalLock(hDIB));
#else
  lpDIB = (LPBITMAPINFO)GlobalLock(hDIB);
#endif

  // Now read the DIB portion of the file, which follows the
  // end of icon resource table
  _llseek( hFile, iconFileRes.dwDIBOffset, 0);
  cbBits = _lread( hFile, (LPSTR )lpDIB, (WORD )iconFileRes.dwDIBSize);

  // Done reading file
  _lclose(hFile);

  GlobalUnlock( hDIB);

  if( (DWORD )cbBits != iconFileRes.dwDIBSize)
  {
    GlobalFree( hDIB);
    return (HANDLE) NULL;
  }
  return hDIB;
}

//*****************************************************************************
//* Function : MakeIcon()                                                     *
//* Purpose  : Creates an icon based on the DIB info. returned by ReadIcon.   *
//* Parameters : HANDLE hDIB - A handle to the icon's DIB information.        *
//* Returns :  A handle to an Icon. NULL is returned if an icon cannot be     *
//*            successfully created.                                          *
//* Comments : The steps involved in making an icon from a DIB are very       *
//*            similar to those involved in making a cursor from a DIB.       *
//* Steps : 1) Obtain a pointer to the Icon's DIB bits.                       *
//*         2) Divide the DIB'd height with 2 to account for the fact that the*
//*            DIB stores both the XOR and the AND masks, one after the other.*
//*         3) Determine the offset to the XOR bits.                          *
//*         4) Determine the offset to the AND bits.                          *
//*         5) Create a device dependent bitmap with the XOR bits.            *
//*         6) Obtain the device dependent XOR bitmask and save in memory.    *
//*            The AND bitmask is monochrome. Monochrome bits are identical   *
//*            in both the device dependent bitmaps and device independent    *
//*            bitmaps. So, no need to convert the AND bitmask.               *
//*         7) Since a DIB is stored upside down, flip the monochrome AND bits*
//*            by scanlines.                                                  *
//*         8) Use the XOR and AND bits and create an icon with CreateIcon.   * 
//*****************************************************************************

HICON MakeIcon( HANDLE hDIB, HINSTANCE hInst)
{ LPSTR         lpXORbits,
                lpANDbits;    // Pointer to XOR and AND bits
  HBITMAP       hbmXor;       // handle to XOR bitmap
  BITMAP        bmpXor;       // Used to manipulate XOR bitmap
  DWORD         dwBmpSize;    // Size of XOR bitmap
  HANDLE        hXorDDB;
  LPSTR         lpXorDDB;
  LONG          szFlip[32];
  int           j,
                k;
  HDC           hDC;
  HICON         hIcon;
  LPBITMAPINFO  lpDIB;

  // 1) Obtain a pointer to the Icon's DIB bits.
#ifdef __WINDOWS_386__
  lpDIB = (LPBITMAPINFO )MK_FP32(GlobalLock( hDIB));
#else
  lpDIB = (LPBITMAPINFO )GlobalLock( hDIB);
#endif

  // 2) Divide the DIB'd height with 2 to account for the fact that the
  //    DIB stores both the XOR and the AND masks, one after the other.
  lpDIB->bmiHeader.biHeight /= 2;

  // 3) Determine the offset to the XOR bits.
  // To obtain this value, we have to skip the header, and color table
  lpXORbits = (LPSTR )lpDIB + (int )lpDIB->bmiHeader.biSize +
              (DIBNumColors( (LPSTR )lpDIB) * sizeof( RGBQUAD));

  // 4) Determine the offset to the AND bits.
  //    To obtain this value, skip the XOR bits
  lpANDbits = lpXORbits + (int )(lpDIB->bmiHeader.biHeight *
              (WIDTHBYTES ( lpDIB->bmiHeader.biWidth *
                            lpDIB->bmiHeader.biBitCount)));

  // Get a hDC so we can create a bitmap compatible with it
  hDC = CreateDC( T("DISPLAY"), NULL, NULL, NULL);

  // 5) Create a device dependent bitmap with the XOR bits.
  hbmXor = CreateDIBitmap( hDC, (LPBITMAPINFOHEADER)&(lpDIB->bmiHeader),
                           CBM_INIT, lpXORbits, lpDIB, DIB_RGB_COLORS);

  GetObject( hbmXor, sizeof(BITMAP), (LPSTR)&bmpXor);

  dwBmpSize = (DWORD )(bmpXor.bmWidthBytes * bmpXor.bmHeight * bmpXor.bmPlanes);
  hXorDDB = GlobalAlloc( GHND, dwBmpSize);
  if(hXorDDB == (HANDLE) NULL)
  {
    // clean up before quitting
    DeleteObject( hbmXor);
    DeleteDC( hDC);
    GlobalUnlock( hDIB);
    return (HICON) NULL;
  }

#ifdef __WINDOWS_386__
  lpXorDDB = (LPSTR)MK_FP32(GlobalLock( hXorDDB));
#else
  lpXorDDB = (LPSTR)GlobalLock( hXorDDB);
#endif

  // 6) Obtain the device dependent XOR bitmask and save in memory.
  // The AND bitmask is monochrome. Monochrome bits are identical
  // in both the device dependent bitmaps and device independent
  // bitmaps. So, no need to convert the AND bitmask.
  GetBitmapBits( hbmXor, dwBmpSize, lpXorDDB);

  // 7) Since a DIB is stored upside down, flip the monochrome AND bits by scanlines.
  k = (int )lpDIB->bmiHeader.biHeight;
  for( j = 0 ; j < k ; j++, lpANDbits += sizeof(DWORD))
    szFlip[(k - 1) - j] = *(DWORD FAR *)lpANDbits;

  // 8) Use the XOR and AND bits and create an icon with CreateIcon.
  hIcon = CreateIcon( hInst, bmpXor.bmWidth, bmpXor.bmHeight, bmpXor.bmPlanes,
                      bmpXor.bmBitsPixel, (const BYTE *)szFlip, (const BYTE *)lpXorDDB);

  // Clean up before exiting.
  DeleteObject( hbmXor);
  GlobalUnlock( hXorDDB);
  GlobalFree( hXorDDB);
  DeleteDC( hDC);
  GlobalUnlock( hDIB);

  return hIcon;
}

// **************************************************************************

//*****************************************************************************
//* Function : ReadCursorFile()                                               *
//* Purpose  : Reads a cursor resource file and creates a cursor based on that*
//*            information.                                                   *
//* Parameters : char *szFileName - The cursor resource file.                 *
//* Returns :  A handle to a cursor. The handle will be NULL if a cursor can't*
//*            be created for any reason.                                     *
//*****************************************************************************

HCURSOR ReadCursorFile( wxChar *szFileName, HINSTANCE hInst, int *W, int *H,
                        int *XHot, int *YHot)
{ HANDLE    hDIB;    // Handle to DIB memory
  HCURSOR   hCursor;
  POINT     ptHotSpot;

  // read cur DIB from file
  if( (hDIB = ReadCur( szFileName, (LPPOINT )&ptHotSpot, W, H)) == (HANDLE) NULL)
    return (HCURSOR) NULL;
  hCursor = MakeCursor( hDIB, (LPPOINT )&ptHotSpot, hInst);//create cur from DIB
  if(XHot != 0)
    *XHot = ptHotSpot.x;
  if(YHot != 0)
    *YHot = ptHotSpot.y;
  GlobalFree( hDIB);
  return ( hCursor);
}

//*****************************************************************************
//* Function : IconToCursor()                                                 *
//* Purpose  : Reads an icon resource file and creates a cursor based on that *
//*            information.                                                   *
//* Parameters : char *szFileName - The icon resource file.                   *
//* Returns :  A handle to a cursor. The handle will be NULL if a cursor can't*
//*            be created for any reason.                                     *
//* Comments : An icon may be in color. So, the DIB has to be forced to be    *
//*            monochrome.                                                    *
//*****************************************************************************

HCURSOR IconToCursor( wxChar *szFileName, HINSTANCE hInst, int XHot, int YHot,
                      int  *W, int *H)
{ HCURSOR   hCursor;
  HANDLE    hDIB;
  POINT     ptHotSpot;

  if( (hDIB = ReadIcon( szFileName, W, H)) == (HANDLE) NULL)
                                             //read icon file to get icon DIB
    return (HCURSOR) NULL;
  // Set the hot spot of the cursor
  ptHotSpot.x = XHot;
  ptHotSpot.y = YHot;
  hCursor     = MakeCursor( hDIB, (LPPOINT )&ptHotSpot, hInst);
                              //create cursor from DIB
  GlobalFree( hDIB);
  return hCursor;
}

//*****************************************************************************
//* Function : ReadCur()                                                      *
//* Purpose  : Reads a cursor resource file and extracts the DIB information. *
//* Parameters : LPSTR szFileName - The cursor resource file.                 *
//* Returns :  A handle to a DIB. The handle will be NULL if the resource file*
//*            is corrupt or if memory cannot be allocated for the DIB info.  *
//*****************************************************************************

HANDLE ReadCur( wxChar *szFileName, LPPOINT lpptHotSpot, int *W, int *H)
{ CURFILEHEADER   curFileHead;  // CURSOR file header structure
  CURFILERES      curFileRes;   // CURSOR file resource
  WORD            cbHead,
                  cbRes,
                  cbBits;       // Used for reading in file
  LPBITMAPINFO    lpDIB;        // Pointer to DIB memory
  int             hFile;        // Handle to File
  HANDLE          hDIB;
  int             nWidth      = GetSystemMetrics( SM_CXCURSOR),
                  nHeight     = GetSystemMetrics( SM_CYCURSOR),
                  nDirEntries = 0;

  // Open and read the .ICO file header and the first ICONFILERES
  hFile  = _lopen( wxFNCONV(szFileName), OF_READ);
  cbHead = _lread( hFile,  (LPSTR )&curFileHead, sizeof( CURFILEHEADER));
  cbRes  = _lread( hFile,  (LPSTR )&curFileRes,  sizeof( CURFILERES));
  ++nDirEntries;

  if((cbHead != sizeof( CURFILEHEADER)) || (cbRes != sizeof( CURFILERES)))
    return (HANDLE) NULL;

   // Verify that it's an .CUR file
  if ((curFileRes.bReserved1 != 0) || (curFileHead.wResourceType != 2))
    return (HANDLE) NULL;

  // following added by P.S.
  while( ((unsigned)nDirEntries < curFileHead.wResourceCount) &&
         ((curFileRes.bWidth != nWidth) || (curFileRes.bHeight != nHeight)))
  {
    cbRes = _lread( hFile, (LPSTR )&curFileRes, sizeof( CURFILERES));
    if(cbRes != sizeof( CURFILERES))
      return (HANDLE) NULL;
    else
      ++nDirEntries;
  }
  if(W != 0)
    *W = curFileRes.bWidth;
  if(H != 0)
    *H = curFileRes.bHeight;


  // Allocate & lock memory to read in the DIB
  hDIB = GlobalAlloc(GHND, curFileRes.dwDIBSize);
  if(hDIB == (HANDLE) NULL)
    return (HANDLE) NULL;

#ifdef __WINDOWS_386__
  lpDIB = (LPBITMAPINFO )MK_FP32(GlobalLock(hDIB));
#else
  lpDIB = (LPBITMAPINFO )GlobalLock(hDIB);
#endif

  // Now read the DIB portion of the file, which follows the
  // end of icon resource table
  _llseek( hFile, curFileRes.dwDIBOffset, 0);
  cbBits = _lread( hFile, (LPSTR )lpDIB, (WORD )curFileRes.dwDIBSize);

  // Done reading file
  _lclose(hFile);

  if((DWORD)cbBits != curFileRes.dwDIBSize)
  {
    GlobalUnlock( hDIB);
    GlobalFree( hDIB);
    return (HANDLE) NULL;
  }
  if(lpptHotSpot != (LPPOINT) NULL)  // If it is necessary to know the hot spot
  {
    lpptHotSpot->x = (int )curFileRes.wXHotspot;
    lpptHotSpot->y = (int )curFileRes.wYHotspot;
  }
  GlobalUnlock( hDIB);
  return( hDIB);
}

//*****************************************************************************
//* Function : ColorDDBToMonoDDB()                                            *
//* Purpose  : Converts a color bitmap to a monochrome bitmap.                *
//* Parameters : HBITMAP hbm - The color bitmap.                              *
//* Returns :  A handle to a monochrome bitmap.                               *
//*****************************************************************************
HBITMAP ColorDDBToMonoDDB ( HBITMAP hbm)
{ BITMAP              bm;
  BITMAPINFOHEADER    bi;
  LPBITMAPINFOHEADER  lpbi;
  DWORD               dwLen;
  HANDLE              hdib;
  HANDLE              h;
  HDC                 hdc;
  HBITMAP             hbmMono;

  GetObject( hbm, sizeof( bm), (LPSTR )&bm);

  bi.biSize           = sizeof( BITMAPINFOHEADER);    // size of this structure
  bi.biWidth          = bm.bmWidth;                   // bitmap width in pixels
  bi.biHeight         = bm.bmHeight;                  // bitmap height in pixels
  bi.biPlanes         = 1;                            // # of planes always 1 for DIBs
  bi.biBitCount       = bm.bmPlanes * bm.bmBitsPixel; // color bits per pixel
  bi.biCompression    = BI_RGB;                       // no compression
  bi.biSizeImage      = 0;                            // 0 means default size
  bi.biXPelsPerMeter  = 0;                            // not used
  bi.biYPelsPerMeter  = 0;                            // not used
  bi.biClrUsed        = 0;                            // 0 means default colors
  bi.biClrImportant   = 0;                            // 0 means defaults

  dwLen = bi.biSize + PaletteSize((LPSTR)&bi);

  hdc = GetDC( (HWND) NULL);

  hdib = GlobalAlloc( GHND, dwLen);
  if (hdib == (HANDLE) NULL)
  {
    ReleaseDC( (HWND) NULL, hdc);
    return (HBITMAP) NULL;
  }

#ifdef __WINDOWS_386__
  lpbi = (LPBITMAPINFOHEADER )MK_FP32(GlobalLock( hdib));
#else
  lpbi = (LPBITMAPINFOHEADER )GlobalLock( hdib);
#endif

  *lpbi = bi;

  // Call GetDIBits with a NULL lpBits parameter; it will calculate
  // the biSizeImage field.
  GetDIBits( hdc, hbm, 0, (WORD)bi.biHeight,
             NULL, (LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

  bi = *lpbi;
  GlobalUnlock( hdib);

  // If the driver did not fill in the biSizeImage field, make one up.
  if(bi.biSizeImage == 0)
    bi.biSizeImage = WIDTHBYTES( (DWORD )bm.bmWidth * bi.biBitCount) * bm.bmHeight;

  // Reallocate the buffer big enough to hold all the bits.
  dwLen = bi.biSize + PaletteSize((LPSTR)&bi) + bi.biSizeImage;
  if( (h = GlobalReAlloc( hdib, dwLen, 0)) != 0)
    hdib = h;
  else
  {
    GlobalFree( hdib);
    ReleaseDC( (HWND) NULL, hdc);
    return (HBITMAP) NULL;
  }

  // Call GetDIBits with a NON-NULL lpBits parameter, to actually
  // get the bits this time.

#ifdef __WINDOWS_386__
  lpbi = (LPBITMAPINFOHEADER )MK_FP32(GlobalLock( hdib));
#else
  lpbi = (LPBITMAPINFOHEADER )GlobalLock( hdib);
#endif

  if( GetDIBits( hdc, hbm, 0, (WORD)bi.biHeight,
                 (LPSTR)lpbi + (WORD)lpbi->biSize + PaletteSize((LPSTR)lpbi),
                 (LPBITMAPINFO)lpbi, DIB_RGB_COLORS) == 0)
  {
    GlobalUnlock( hdib);
    hdib = (HANDLE) NULL;
    ReleaseDC( (HWND) NULL, hdc);
    return (HBITMAP) NULL;
  }

  // Finally, create a monochrome DDB, and put the DIB into
  // it. SetDIBits does smart color conversion.
  hbmMono = CreateBitmap((WORD)lpbi->biWidth, (WORD)lpbi->biHeight, 1, 1, NULL);
  SetDIBits( hdc, hbmMono, (WORD)0, (WORD)lpbi->biHeight,
             (LPSTR)lpbi + (int )lpbi->biSize + PaletteSize((LPSTR)lpbi),
             (LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

  bi = *lpbi;
  GlobalUnlock( hdib);
  GlobalFree( hdib);

  ReleaseDC((HWND) NULL, hdc);
  return hbmMono;
}

//*****************************************************************************
//* Function : MakeCursor()                                                   *
//* Purpose  : Creates a cursor based on the DIB info. returned by ReadCursor.*
//* Parameters : HANDLE hDIB - A handle to the cursor's DIB information.      *
//*              LPPOINT lppt - A pointer to a point struct. indicating the   *
//*                             location of the Cursor's hot spot.            *
//* Returns :  A handle to a cursor. NULL is returned if a cursor cannot be   *
//*            successfully created.                                          *
//* Comments : The steps involved in making a cursor from a DIB are very      *
//*            similar to those involved in making an icon from a DIB.        *
//* Steps : 1) Obtain a pointer to the Cursor's DIB bits.                     *
//*         2) Divide the DIB's height with 2 to account for the fact that the*
//*            DIB stores both the XOR and the AND masks, one after the other.*
//*         3) Determine the offset to the XOR bits.                          *
//*         4) Determine the offset to the AND bits.                          *
//*         5) Create a device dependent bitmap with the XOR bits.            *
//*         6) Obtain the device dependent XOR bitmask and save in memory.    *
//*            The AND bitmask is monochrome. Monochrome bits are identical   *
//*            in both the device dependent bitmaps and device independent    *
//*            bitmaps. So, no need to convert the AND bitmask.               *
//*         7) Since a DIB is stored upside down, flip the monochrome AND bits*
//*            by scanlines.                                                  *
//*         8) Use the XOR and AND bits and create a cursor with CreateCursor.*
//*****************************************************************************

HCURSOR MakeCursor( HANDLE hDIB, LPPOINT lpptHotSpot, HINSTANCE hInst)
{ LPSTR         lpXORbits,
                lpANDbits;    // Pointer to XOR and AND bits
  HBITMAP       hbmXor;       // handle to XOR bitmap
  BITMAP        bmpXor;       // Used to manipulate XOR bitmap
  DWORD         dwBmpSize;    // Size of XOR bitmap
  HCURSOR       hCursor;
  HANDLE        hXorDDB;
  LPSTR         lpXorDDB;
  LONG          szFlip[32];
  int           j,
                k;
  HDC           hDC;
  LPBITMAPINFO  lpDIB;

  // 1) Obtain a pointer to the Cursor's DIB bits.
#ifdef __WINDOWS_386__
  lpDIB = (LPBITMAPINFO )MK_FP32(GlobalLock( hDIB));
#else
  lpDIB = (LPBITMAPINFO )GlobalLock( hDIB);
#endif

  // 2) Divide the DIB's height with 2 to account for the fact that the
  //    DIB stores both the XOR and the AND masks, one after the other.
  lpDIB->bmiHeader.biHeight /= 2;

  // 3) Determine the offset to the XOR bits.
  //    To obtain this value, we have to skip the header, and color table
  lpXORbits = (LPSTR )lpDIB + (int )lpDIB->bmiHeader.biSize +
              (DIBNumColors((LPSTR)lpDIB) * sizeof(RGBQUAD));

  // 4) Determine the offset to the AND bits
  //    To obtain this value, skip the XOR bits
  lpANDbits = lpXORbits + (int )( lpDIB->bmiHeader.biHeight *
              (WIDTHBYTES( lpDIB->bmiHeader.biWidth *
                           lpDIB->bmiHeader.biBitCount)));

  // Get a hDC so we can create a bitmap compatible with it
  hDC = CreateDC( T("DISPLAY"), NULL, NULL, NULL);

  // 5) Create a device dependent bitmap with the XOR bits.
  hbmXor = CreateBitmap( (int )lpDIB->bmiHeader.biWidth,
                         (int )lpDIB->bmiHeader.biHeight, 1, 1, NULL);
  SetDIBits( hDC, hbmXor, 0, (WORD)lpDIB->bmiHeader.biHeight, lpXORbits,
             lpDIB, DIB_RGB_COLORS);
  GetObject( hbmXor, sizeof( BITMAP), (LPSTR )&bmpXor);

  dwBmpSize = (DWORD )(bmpXor.bmWidthBytes * bmpXor.bmHeight * bmpXor.bmPlanes);
  hXorDDB = GlobalAlloc( GHND, dwBmpSize);
  if(hXorDDB == (HANDLE) NULL)
  {  // clean up before quitting
    DeleteObject( hbmXor);
    DeleteDC( hDC);
    GlobalUnlock( hDIB);
    return (HCURSOR) NULL;
  }
#ifdef __WINDOWS_386__
  lpXorDDB = (LPSTR)MK_FP32(GlobalLock( hXorDDB));
#else
  lpXorDDB = (LPSTR)GlobalLock( hXorDDB);
#endif

  // 6) Obtain the device dependent XOR bitmask and save in memory.
  //    The AND bitmask is monochrome. Monochrome bits are identical
  //    in both the device dependent bitmaps and device independent
  //    bitmaps. So, no need to convert the AND bitmask.
  GetBitmapBits( hbmXor, dwBmpSize, lpXorDDB);

  // 7) Since a DIB is stored upside down, flip the monochrome AND bits by scanlines.
  k = (int)lpDIB->bmiHeader.biHeight;
  for( j = 0 ; j < k; j++, lpANDbits += sizeof( DWORD))
    szFlip[(k - 1) - j] = *(DWORD FAR *)lpANDbits;

  // 8) Use the XOR and AND bits and create a cursor with CreateCursor.
  hCursor = CreateCursor( hInst, lpptHotSpot->x, lpptHotSpot->y,
                          bmpXor.bmWidth, bmpXor.bmHeight, (LPSTR)szFlip, lpXorDDB);

  // Clean up before exiting.
  DeleteObject( hbmXor);
  GlobalUnlock( hXorDDB);
  GlobalFree( hXorDDB);
  DeleteDC( hDC);
  GlobalUnlock( hDIB);

  return hCursor;
}

//*****************************************************************************
//* Function : PaletteSize()                                                  *
//* Purpose  : Calculates the palette size in bytes. If the info. block is of *
//*            the BITMAPCOREHEADER type, the number of colors is multiplied  *
//*            by sizeof(RGBTRIPLE) to give the palette size, otherwise the   *
//*            number of colors is multiplied by sizeof(RGBQUAD).             *
//* Parameters : LPSTR pv - pointer to the BITMAPINFOHEADER                   *
//* Returns :  The size of the palette.                                       *
//*****************************************************************************

WORD PaletteSize( LPSTR pv)
{ LPBITMAPINFOHEADER  lpbi;
  WORD                NumColors;

  lpbi      = (LPBITMAPINFOHEADER )pv;
  NumColors = DIBNumColors((LPSTR )lpbi);

  if(lpbi->biSize == sizeof( BITMAPCOREHEADER))  // OS/2 style DIBs
    return NumColors * sizeof( RGBTRIPLE);
  else
    return NumColors * sizeof( RGBQUAD);
}

//*****************************************************************************
//* Function : DIBNumColors()                                                 *
//* Purpose  : This function calculates the number of colors in the DIB's     *
//*            color table by finding the bits per pixel for the DIB (whether *
//*            Win3.0 or OS/2-style DIB). If bits per pixel is 1: colors=2,   *
//*            if 4: colors=16, if 8: colors=256, if 24, no colors in color   *
//*            table.                                                         *
//* Parameters : LPSTR lpbi - pointer to packed-DIB memory block.             *
//* Returns :  The number of colors in the color table.                       *
//*****************************************************************************

WORD DIBNumColors ( LPSTR pv)
{ int                 bits;
  BITMAPINFOHEADER  *lpbi;
  BITMAPCOREHEADER  *lpbc;

  lpbi = ((BITMAPINFOHEADER* )pv);     // assume win 3.0 style DIBs
  lpbc = ((BITMAPCOREHEADER* )pv);     // assume OS/2 style DIBs

  // With the BITMAPINFO format headers, the size of the palette
  // is in biClrUsed, whereas in the BITMAPCORE - style headers, it
  // is dependent on the bits per pixel ( = 2 raised to the power of
  // bits/pixel).
    
  if(lpbi->biSize != sizeof( BITMAPCOREHEADER))
  {
    if(lpbi->biClrUsed != 0)
      return (WORD)lpbi->biClrUsed;
    bits = lpbi->biBitCount;
  }
  else
    bits = lpbc->bcBitCount;

  switch( bits)
  {
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

#if 0
// ******************************************************************
BOOL fGetXPixmap( BOOL fIsIcon, wxChar *szFileName, HINSTANCE hInst,
                  char cData[], int &width, int &height)
{ HDC       hdc,
            hdcMemory;
  HBITMAP   hbmp,
            holdbmp;
  int       i,
            j,
            w,
            h;
  BYTE     *s,
            cByte,
            cMask;
  COLORREF  rgb;
  HCURSOR   hIconOrCursor = fIsIcon ?
                      IconToCursor( szFileName, hInst, 0, 0, &w, &h)
                    : ReadCursorFile( szFileName, hInst, &w, &h, 0, 0);
  int       sum;

  if(hIconOrCursor == 0)
    return FALSE;

  hdc       = GetDC( GetDesktopWindow());
  hdcMemory = CreateCompatibleDC( hdc);
  hbmp      = CreateCompatibleBitmap( hdc, w, h);
  holdbmp   = SelectObject( hdcMemory, hbmp);
  PatBlt( hdcMemory, 0, 0, w, h, BLACKNESS); // or use WHITENESS??
  DrawIcon( hdcMemory, 0, 0, hIconOrCursor); //using HCURSOR with DrawIcon is OK

  // the data retrieval follows:
  width  = w;
  height = h;
  for( j = 0, s = (BYTE *)cData ; j < h ; ++j)
    for( i = 0 ; i < w ; ++i, cMask >>= 1)
    {
      if( (i % 8) == 0)
      {
        cByte = 0;
        cMask = 0x80;
      }
      rgb  = GetPixel( hdcMemory, i, j);
      sum  = (int )(rgb & 0xFFL);
      sum += (int )((rgb & 0xFF00L) >> 8);
      sum += (int )((rgb & 0xFF0000L) >> 16);
      if(sum > 381)
        cByte = cByte | cMask;
      if( (i % 8) == 7)
      {
        *s = cByte;
        ++s;
      }
    }
  SelectObject( hdcMemory, holdbmp);
  DeleteDC( hdcMemory);
  ReleaseDC( GetDesktopWindow(), hdc);
  DestroyCursor( hIconOrCursor);
  DeleteObject( hbmp);
  return TRUE;
}
#endif

// Added from scavenged internet code, JACS 23/6/95
HCURSOR MakeCursorFromBitmap(HINSTANCE hInst, HBITMAP hBitmap, POINT *pPoint)
{
  HDC hDCColor, hDCMono;
  HDC hDC;
  HBITMAP   hBmpOld;
  HBITMAP   hAndBmp;
  HBITMAP   hXorBmp;
  HCURSOR   hNewCursor;
  BITMAP    bm;
  DWORD     dwBytes;
  NPSTR     andBits;
  NPSTR     xorBits;

  hDC = GetDC((HWND) NULL);
  hDCColor = CreateCompatibleDC(hDC);
  hDCMono = CreateCompatibleDC(hDC);
  hAndBmp = CreateCompatibleBitmap(hDCMono, 32, 32);
  hXorBmp = CreateCompatibleBitmap(hDCMono, 32, 32);

  hBmpOld = (HBITMAP) SelectObject(hDCColor, hBitmap);
  SelectObject(hDCMono, hAndBmp);
  SetBkColor(hDCColor, RGB(191, 191, 191));

  BitBlt(hDCMono, 0, 0, 32, 32, hDCColor, 0, 0, SRCCOPY);

  // Now we have the AND Mask

  GetObject(hAndBmp, sizeof(BITMAP), (LPSTR) &bm);
  dwBytes = (bm.bmWidthBytes * bm.bmHeight);
  andBits = (NPSTR) LocalAlloc(LPTR, dwBytes);
  GetBitmapBits(hAndBmp, dwBytes, andBits);

  SelectObject(hDCMono, hXorBmp);
  SetBkColor(hDCColor, RGB(0, 0, 0));

  BitBlt(hDCMono, 0, 0, 32, 32, hDCColor, 0, 0, SRCCOPY);

  // Now we have the XOR Mask

  GetObject(hXorBmp, sizeof(BITMAP), (LPSTR) &bm);
  dwBytes = (bm.bmWidthBytes * bm.bmHeight);
  xorBits = (NPSTR) LocalAlloc(LPTR, dwBytes);
  GetBitmapBits(hXorBmp, dwBytes, xorBits);

  if (pPoint->x > 32)
    pPoint->x = 32;
  if (pPoint->y > 32)
    pPoint->y = 32;

  hNewCursor = CreateCursor(hInst,
                            pPoint->x, pPoint->y, 32, 32, andBits, xorBits);

  SelectObject(hDCColor, hBmpOld);
  SelectObject(hDCMono, hBmpOld);
  DeleteDC(hDCColor);
  DeleteDC(hDCMono);
  DeleteObject(hAndBmp);
  DeleteObject(hXorBmp);
  ReleaseDC(NULL, hDC);
#ifndef __WIN32__
#ifdef STRICT
  LocalUnlock(LocalHandle((void NEAR*) andBits));
  LocalUnlock(LocalHandle((void NEAR*) xorBits));
  LocalFree(LocalHandle((void NEAR*) andBits));
  LocalFree(LocalHandle((void NEAR*) xorBits));
#else
  LocalUnlock(LocalHandle((WORD) andBits));
  LocalUnlock(LocalHandle((WORD) xorBits));
  LocalFree(LocalHandle((WORD) andBits));
  LocalFree(LocalHandle((WORD) xorBits));
#endif
#else
  LocalUnlock(LocalHandle((LPCVOID) andBits));
  LocalUnlock(LocalHandle((LPCVOID) xorBits));
  LocalFree(LocalHandle((LPCVOID) andBits));
  LocalFree(LocalHandle((LPCVOID) xorBits));
#endif
  return hNewCursor;
}

/*
 * This doesn't work: just gives us a grey square. Ideas, anyone?
 */
 
HICON MakeIconFromBitmap(HINSTANCE hInst, HBITMAP hBitmap)
{
  HDC hDCColor, hDCMono;
  HDC hDC;
  HBITMAP   hBmpOld;
  HBITMAP   hAndBmp;
  HBITMAP   hXorBmp;
  HICON     hNewIcon;
  BITMAP    bm;
  DWORD     dwBytes;
  NPSTR     andBits;
  NPSTR     xorBits;

  hDC = GetDC((HWND) NULL);
  hDCColor = CreateCompatibleDC(hDC);
  hDCMono = CreateCompatibleDC(hDC);
  hAndBmp = CreateCompatibleBitmap(hDCMono, 32, 32);
  hXorBmp = CreateCompatibleBitmap(hDCMono, 32, 32);

  hBmpOld = (HBITMAP) SelectObject(hDCColor, hBitmap);
  SelectObject(hDCMono, hAndBmp);
  SetBkColor(hDCColor, RGB(191, 191, 191));

  BitBlt(hDCMono, 0, 0, 32, 32, hDCColor, 0, 0, SRCCOPY);

  // Now we have the AND Mask

  GetObject(hAndBmp, sizeof(BITMAP), (LPSTR) &bm);
  dwBytes = (bm.bmWidthBytes * bm.bmHeight);
  andBits = (NPSTR) LocalAlloc(LPTR, dwBytes);
  GetBitmapBits(hAndBmp, dwBytes, andBits);

  SelectObject(hDCMono, hXorBmp);
  SetBkColor(hDCColor, RGB(0, 0, 0));

  BitBlt(hDCMono, 0, 0, 32, 32, hDCColor, 0, 0, SRCCOPY);

  // Now we have the XOR Mask

  GetObject(hXorBmp, sizeof(BITMAP), (LPSTR) &bm);
  dwBytes = (bm.bmWidthBytes * bm.bmHeight);
  xorBits = (NPSTR) LocalAlloc(LPTR, dwBytes);
  GetBitmapBits(hXorBmp, dwBytes, xorBits);

  hNewIcon = CreateIcon(hInst, 1, 4, 32, 32, (unsigned char *)andBits, (unsigned char *)xorBits);

  SelectObject(hDCColor, hBmpOld);
  SelectObject(hDCMono, hBmpOld);
  DeleteDC(hDCColor);
  DeleteDC(hDCMono);
  DeleteObject(hAndBmp);
  DeleteObject(hXorBmp);
  ReleaseDC((HWND) NULL, hDC);
#ifndef __WIN32__
#ifdef STRICT
  LocalUnlock(LocalHandle((void NEAR*) andBits));
  LocalUnlock(LocalHandle((void NEAR*) xorBits));
  LocalFree(LocalHandle((void NEAR*) andBits));
  LocalFree(LocalHandle((void NEAR*) xorBits));
#else
  LocalUnlock(LocalHandle((WORD) andBits));
  LocalUnlock(LocalHandle((WORD) xorBits));
  LocalFree(LocalHandle((WORD) andBits));
  LocalFree(LocalHandle((WORD) xorBits));
#endif
#else
  LocalUnlock(LocalHandle((LPCVOID) andBits));
  LocalUnlock(LocalHandle((LPCVOID) xorBits));
  LocalFree(LocalHandle((LPCVOID) andBits));
  LocalFree(LocalHandle((LPCVOID) xorBits));
#endif
  return hNewIcon;
}


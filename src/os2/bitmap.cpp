/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.cpp
// Purpose:     wxBitmap
// Author:      David Webster
// Modified by:
// Created:     08/08/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "bitmap.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include <stdio.h>

    #include "wx/list.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/palette.h"
    #include "wx/dcmemory.h"
    #include "wx/bitmap.h"
    #include "wx/icon.h"
#endif

#include "wx/os2/private.h"
#include "wx/log.h"

//#include "wx/msw/dib.h"
#include "wx/image.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject)

IMPLEMENT_DYNAMIC_CLASS(wxBitmapHandler, wxObject)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxBitmapRefData
// ----------------------------------------------------------------------------

wxBitmapRefData::wxBitmapRefData()
{
    m_nQuality      = 0;
    m_pSelectedInto = NULL;
    m_nNumColors    = 0;
    m_pBitmapMask   = NULL;
    m_hBitmap = (WXHBITMAP) NULL;
} // end of wxBitmapRefData::wxBitmapRefData

void wxBitmapRefData::Free()
{
    wxASSERT_MSG( !m_pSelectedInto,
                  wxT("deleting bitmap still selected into wxMemoryDC") );

    if (m_hBitmap)
    {
        if ( !::GpiDeleteBitmap((HBITMAP)m_hBitmap) )
        {
            wxLogLastError("GpiDeleteBitmap(hbitmap)");
        }
    }

    delete m_pBitmapMask;
    m_pBitmapMask = NULL;
} // end of wxBitmapRefData::Free

// ----------------------------------------------------------------------------
// wxBitmap creation
// ----------------------------------------------------------------------------

// this function should be called from all wxBitmap ctors
void wxBitmap::Init()
{
    // m_refData = NULL; done in the base class ctor

    if (wxTheBitmapList)
        wxTheBitmapList->AddBitmap(this);
} // end of wxBitmap::Init

bool wxBitmap::CopyFromIconOrCursor(
  const wxGDIImage&                 rIcon
)
{
    HPOINTER                        hIcon = (HPOINTER)rIcon.GetHandle();
    POINTERINFO                     SIconInfo;

    if (!::WinQueryPointerInfo(hIcon, &SIconInfo))
    {
        wxLogLastError(wxT("WinQueryPointerInfo"));
        return FALSE;
    }
    wxBitmapRefData*                pRefData = new wxBitmapRefData;

    m_refData = pRefData;

    int                             nWidth = rIcon.GetWidth();
    int                             nHeight = rIcon.GetHeight();

    pRefData->m_nWidth  = nWidth;
    pRefData->m_nHeight = nHeight;
    pRefData->m_nDepth  = wxDisplayDepth();

    pRefData->m_hBitmap = (WXHBITMAP)SIconInfo.hbmColor;

    //
    // No mask in the Info struct in OS/2
    //
    return(TRUE);
} // end of wxBitmap::CopyFromIconOrCursor

bool wxBitmap::CopyFromCursor(
  const wxCursor&                   rCursor
)
{
    UnRef();

    if (!rCursor.Ok())
        return(FALSE);
    return(CopyFromIconOrCursor(rCursor));
} // end of wxBitmap::CopyFromCursor

bool wxBitmap::CopyFromIcon(
  const wxIcon&                     rIcon
)
{
    UnRef();

    if (!rIcon.Ok())
        return(FALSE);

    return CopyFromIconOrCursor(rIcon);
} // end of wxBitmap::CopyFromIcon

wxBitmap::~wxBitmap()
{
    if (wxTheBitmapList)
        wxTheBitmapList->DeleteObject(this);
} // end of wxBitmap::~wxBitmap

wxBitmap::wxBitmap(
  const char                        zBits[]
, int                               nTheWidth
, int                               nTheHeight
, int                               nNoBits
)
{
    Init();

    wxBitmapRefData*                pRefData = new wxBitmapRefData;
    BITMAPINFOHEADER2               vHeader;
    BITMAPINFO2                     vInfo;
    HDC                             hDc;
    HPS                             hPs;
    DEVOPENSTRUC                    vDop = { NULL, "DISPLAY", NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    SIZEL                           vSize = {0, 0};

    wxASSERT(vHabmain != NULL);

    hDc = ::DevOpenDC(vHabmain, OD_MEMORY, (PSZ)"*", 1L, (PDEVOPENDATA)&vDop, 0L);

    vHeader.cbFix           = sizeof(vHeader);
    vHeader.cx              = (USHORT)nTheWidth;
    vHeader.cy              = (USHORT)nTheHeight;
    vHeader.cPlanes         = 1L;
    vHeader.cBitCount       = nNoBits;
    vHeader.ulCompression   = BCA_UNCOMP;
    vHeader.cxResolution    = 0;
    vHeader.cyResolution    = 0;
    vHeader.cclrUsed        = 0;
    vHeader.cclrImportant   = 0;
    vHeader.usUnits         = BRU_METRIC;
    vHeader.usRecording     = BRA_BOTTOMUP;
    vHeader.usRendering     = BRH_NOTHALFTONED;
    vHeader.cSize1          = 0;
    vHeader.cSize2          = 0;
    vHeader.ulColorEncoding = 0;
    vHeader.ulIdentifier    = 0;

    hPs = ::GpiCreatePS(vHabmain, hDc, &vSize, GPIA_ASSOC | PU_PELS);
    if (hPs == 0)
    {
        wxLogLastError("GpiCreatePS Failure");
    }

    m_refData = pRefData;

    pRefData->m_nWidth = nTheWidth;
    pRefData->m_nHeight = nTheHeight;
    pRefData->m_nDepth = nNoBits;
    pRefData->m_nNumColors = 0;
    pRefData->m_pSelectedInto = NULL;

    HBITMAP hBmp = ::GpiCreateBitmap(hPs, &vHeader, 0L, NULL, &vInfo);
    if (!hBmp)
    {
        wxLogLastError("CreateBitmap");
    }
    SetHBITMAP((WXHBITMAP)hBmp);
} // end of wxBitmap::wxBitmap

//
// Create from XPM data
//
wxBitmap::wxBitmap(
  char**                            ppData
)
{
    Init();

    (void)Create( (void *)ppData
                 ,wxBITMAP_TYPE_XPM_DATA
                 ,0
                 ,0
                 ,0
                );
} // end of wxBitmap::wxBitmap

wxBitmap::wxBitmap(
  const char**                      ppData
)
{
    Init();

    (void)Create( (void *)ppData
                 ,wxBITMAP_TYPE_XPM_DATA
                 ,0
                 ,0
                 ,0
                );
} // end of wxBitmap::wxBitmap

wxBitmap::wxBitmap(
  int                               nW
, int                               nH
, int                               nD
)
{
    Init();

    (void)Create( nW
                 ,nH
                 ,nD
                );
} // end of wxBitmap::wxBitmap

wxBitmap::wxBitmap(
  void*                             pData
, long                              lType
, int                               nWidth
, int                               nHeight
, int                               nDepth
)
{
    Init();

    (void)Create( pData
                 ,lType
                 ,nWidth
                 ,nHeight
                 ,nDepth
                );
} // end of wxBitmap::wxBitmap

wxBitmap::wxBitmap(
  const wxString&                   rFilename
, long                              lType
)
{
    Init();

    LoadFile( rFilename
             ,(int)lType
            );
} // end of wxBitmap::wxBitmap

bool wxBitmap::Create(
  int                               nW
, int                               nH
, int                               nD
)
{
    HBITMAP                         hBmp;
    BITMAPINFOHEADER2               vHeader;

    wxASSERT(vHabmain != NULL);
    UnRef();
    m_refData = new wxBitmapRefData;
    GetBitmapData()->m_nWidth = nW;
    GetBitmapData()->m_nHeight = nH;
    GetBitmapData()->m_nDepth = nD;

    if (nD > 0)
    {
        DEVOPENSTRUC                vDop  = {0L, "DISPLAY", NULL, 0L, 0L, 0L, 0L, 0L, 0L};
        SIZEL                       vSize = {0, 0};
        HDC                         hDC   = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&vDop, NULLHANDLE);
        HPS                         hPS   = ::GpiCreatePS(vHabmain, hDC, &vSize, PU_PELS | GPIA_ASSOC);

        memset(&vHeader, '\0', sizeof(BITMAPINFOHEADER2));
        vHeader.cbFix     =  sizeof(BITMAPINFOHEADER2);
        vHeader.cx        = nW;
        vHeader.cy        = nH;
        vHeader.cPlanes   = 1;
        vHeader.cBitCount = nD;

        hBmp = ::GpiCreateBitmap( hPS
                                 ,&vHeader
                                 ,0L
                                 ,NULL
                                 ,NULL
                                );
        ::GpiDestroyPS(hPS);
        ::DevCloseDC(hDC);
    }
    else
    {
        HPS                             hPSScreen;
        HDC                             hDCScreen;
        LONG                            lBitCount;

        hPSScreen = ::WinGetScreenPS(HWND_DESKTOP);
        hDCScreen = ::GpiQueryDevice(hPSScreen);
        ::DevQueryCaps(hDCScreen, CAPS_COLOR_BITCOUNT, 1L, &lBitCount);

        memset(&vHeader, '\0', sizeof(BITMAPINFOHEADER2));
        vHeader.cbFix     =  sizeof(BITMAPINFOHEADER2);
        vHeader.cx        = nW;
        vHeader.cy        = nH;
        vHeader.cPlanes   = 1;
        vHeader.cBitCount = lBitCount;

        hBmp = ::GpiCreateBitmap( hPSScreen
                                 ,&vHeader
                                 ,0L
                                 ,NULL
                                 ,NULL
                                );

        GetBitmapData()->m_nDepth = wxDisplayDepth();
        ::WinReleasePS(hPSScreen);
    }
    SetHBITMAP((WXHBITMAP)hBmp);

#if WXWIN_COMPATIBILITY_2
    GetBitmapData()->m_bOk = hBmp != 0;
#endif // WXWIN_COMPATIBILITY_2

    return Ok();
} // end of wxBitmap::Create

bool wxBitmap::LoadFile(
  const wxString&                   rFilename
, long                              lType
)
{
    HPS                             hPs = NULLHANDLE;

    UnRef();

    wxBitmapHandler*                pHandler = wxDynamicCast( FindHandler(lType)
                                                             ,wxBitmapHandler
                                                            );

    if (pHandler)
    {
        m_refData = new wxBitmapRefData;

        return(pHandler->LoadFile( this
                                  ,rFilename
                                  ,hPs
                                  ,lType
                                  , -1
                                  , -1
                                 ));
    }
    else
    {
        wxImage                     vImage;

        if (!vImage.LoadFile(rFilename, lType) || !vImage.Ok() )
            return(FALSE);

        *this = vImage.ConvertToBitmap();

        return(TRUE);
    }
} // end of wxBitmap::LoadFile

bool wxBitmap::Create(
  void*                             pData
, long                              lType
, int                               nWidth
, int                               nHeight
, int                               nDepth
)
{
    UnRef();

    wxBitmapHandler*                pHandler = wxDynamicCast( FindHandler(lType)
                                                             ,wxBitmapHandler
                                                            );

    if (!pHandler)
    {
        wxLogDebug(wxT("Failed to create bitmap: no bitmap handler for "
                       "type %d defined."), lType);

        return(FALSE);
    }

    m_refData = new wxBitmapRefData;

    return(pHandler->Create( this
                            ,pData
                            ,lType
                            ,nWidth
                            ,nHeight
                            ,nDepth
                           ));
} // end of wxBitmap::Create

bool wxBitmap::SaveFile(
  const wxString&                   rFilename
, int                               lType
, const wxPalette*                  pPalette
)
{
    wxBitmapHandler*                pHandler = wxDynamicCast( FindHandler(lType)
                                                             ,wxBitmapHandler
                                                            );

    if (pHandler)
    {
        return pHandler->SaveFile( this
                                  ,rFilename
                                  ,lType
                                  ,pPalette
                                 );
    }
    else
    {
        // FIXME what about palette? shouldn't we use it?
        wxImage                     vImage(*this);

        if (!vImage.Ok())
            return(FALSE);

        return(vImage.SaveFile( rFilename
                               ,lType
                              ));
    }
} // end of wxBitmap::SaveFile


// ----------------------------------------------------------------------------
// wxImage-wxBitmap convertion
// ----------------------------------------------------------------------------

bool wxBitmap::CreateFromImage( const wxImage& image, int depth )
{
    wxCHECK_MSG( image.Ok(), FALSE, wxT("invalid image") )

// TODO:
/*
    int sizeLimit = 1024*768*3;

    // width and height of the device-dependent bitmap
    int width = GetWidth();
    int bmpHeight = GetHeight();

    // calc the number of bytes per scanline and padding
    int bytePerLine = width*3;
    int sizeDWORD = sizeof( DWORD );
    int lineBoundary = bytePerLine % sizeDWORD;
    int padding = 0;
    if( lineBoundary > 0 )
    {
        padding = sizeDWORD - lineBoundary;
        bytePerLine += padding;
    }
    // calc the number of DIBs and heights of DIBs
    int numDIB = 1;
    int hRemain = 0;
    int height = sizeLimit/bytePerLine;
    if( height >= bmpHeight )
        height = bmpHeight;
    else
    {
        numDIB =  bmpHeight / height;
        hRemain = bmpHeight % height;
        if( hRemain >0 )  numDIB++;
    }

    // set bitmap parameters
    wxBitmap bitmap;
    wxCHECK_MSG( Ok(), bitmap, wxT("invalid image") );
    bitmap.SetWidth( width );
    bitmap.SetHeight( bmpHeight );
    bitmap.SetDepth( wxDisplayDepth() );

    // create a DIB header
    int headersize = sizeof(BITMAPINFOHEADER);
    LPBITMAPINFO lpDIBh = (BITMAPINFO *) malloc( headersize );
    wxCHECK_MSG( lpDIBh, bitmap, wxT("could not allocate memory for DIB header") );
    // Fill in the DIB header
    lpDIBh->bmiHeader.biSize = headersize;
    lpDIBh->bmiHeader.biWidth = (DWORD)width;
    lpDIBh->bmiHeader.biHeight = (DWORD)(-height);
    lpDIBh->bmiHeader.biSizeImage = bytePerLine*height;
    //   the general formula for biSizeImage:
    //      ( ( ( ((DWORD)width*24) +31 ) & ~31 ) >> 3 ) * height;
    lpDIBh->bmiHeader.biPlanes = 1;
    lpDIBh->bmiHeader.biBitCount = 24;
    lpDIBh->bmiHeader.biCompression = BI_RGB;
    lpDIBh->bmiHeader.biClrUsed = 0;
    // These seem not really needed for our purpose here.
    lpDIBh->bmiHeader.biClrImportant = 0;
    lpDIBh->bmiHeader.biXPelsPerMeter = 0;
    lpDIBh->bmiHeader.biYPelsPerMeter = 0;
    // memory for DIB data
    unsigned char *lpBits;
    lpBits = (unsigned char *)malloc( lpDIBh->bmiHeader.biSizeImage );
    if( !lpBits )
    {
        wxFAIL_MSG( wxT("could not allocate memory for DIB") );
        free( lpDIBh );
        return bitmap;
    }

    // create and set the device-dependent bitmap
    HDC hdc = ::GetDC(NULL);
    HDC memdc = ::CreateCompatibleDC( hdc );
    HBITMAP hbitmap;
    hbitmap = ::CreateCompatibleBitmap( hdc, width, bmpHeight );
    ::SelectObject( memdc, hbitmap);

    // copy image data into DIB data and then into DDB (in a loop)
    unsigned char *data = GetData();
    int i, j, n;
    int origin = 0;
    unsigned char *ptdata = data;
    unsigned char *ptbits;

    for( n=0; n<numDIB; n++ )
    {
        if( numDIB > 1 && n == numDIB-1 && hRemain > 0 )
        {
            // redefine height and size of the (possibly) last smaller DIB
            // memory is not reallocated
            height = hRemain;
            lpDIBh->bmiHeader.biHeight = (DWORD)(-height);
            lpDIBh->bmiHeader.biSizeImage = bytePerLine*height;
        }
        ptbits = lpBits;

        for( j=0; j<height; j++ )
        {
            for( i=0; i<width; i++ )
            {
                *(ptbits++) = *(ptdata+2);
                *(ptbits++) = *(ptdata+1);
                *(ptbits++) = *(ptdata  );
                ptdata += 3;
            }
            for( i=0; i< padding; i++ )   *(ptbits++) = 0;
        }
        ::StretchDIBits( memdc, 0, origin, width, height,\
            0, 0, width, height, lpBits, lpDIBh, DIB_RGB_COLORS, SRCCOPY);
        origin += height;
        // if numDIB = 1,  lines below can also be used
        //    hbitmap = CreateDIBitmap( hdc, &(lpDIBh->bmiHeader), CBM_INIT, lpBits, lpDIBh, DIB_RGB_COLORS );
        // The above line is equivalent to the following two lines.
        //    hbitmap = ::CreateCompatibleBitmap( hdc, width, height );
        //    ::SetDIBits( hdc, hbitmap, 0, height, lpBits, lpDIBh, DIB_RGB_COLORS);
        // or the following lines
        //    hbitmap = ::CreateCompatibleBitmap( hdc, width, height );
        //    HDC memdc = ::CreateCompatibleDC( hdc );
        //    ::SelectObject( memdc, hbitmap);
        //    ::SetDIBitsToDevice( memdc, 0, 0, width, height,
        //            0, 0, 0, height, (void *)lpBits, lpDIBh, DIB_RGB_COLORS);
        //    ::SelectObject( memdc, 0 );
        //    ::DeleteDC( memdc );
    }
    bitmap.SetHBITMAP( (WXHBITMAP) hbitmap );

    // similarly, created an mono-bitmap for the possible mask
    if( HasMask() )
    {
        hbitmap = ::CreateBitmap( (WORD)width, (WORD)bmpHeight, 1, 1, NULL );
        ::SelectObject( memdc, hbitmap);
        if( numDIB == 1 )   height = bmpHeight;
        else                height = sizeLimit/bytePerLine;
        lpDIBh->bmiHeader.biHeight = (DWORD)(-height);
        lpDIBh->bmiHeader.biSizeImage = bytePerLine*height;
        origin = 0;
        unsigned char r = GetMaskRed();
        unsigned char g = GetMaskGreen();
        unsigned char b = GetMaskBlue();
        unsigned char zero = 0, one = 255;
        ptdata = data;
        for( n=0; n<numDIB; n++ )
        {
            if( numDIB > 1 && n == numDIB - 1 && hRemain > 0 )
            {
                // redefine height and size of the (possibly) last smaller DIB
                // memory is not reallocated
                height = hRemain;
                lpDIBh->bmiHeader.biHeight = (DWORD)(-height);
                lpDIBh->bmiHeader.biSizeImage = bytePerLine*height;
            }
            ptbits = lpBits;
            for( int j=0; j<height; j++ )
            {
                for(i=0; i<width; i++ )
                {
                    if( (*(ptdata++)!=r) | (*(ptdata++)!=g) | (*(ptdata++)!=b) )
                    {
                        *(ptbits++) = one;
                        *(ptbits++) = one;
                        *(ptbits++) = one;
                    }
                    else
                    {
                        *(ptbits++) = zero;
                        *(ptbits++) = zero;
                        *(ptbits++) = zero;
                    }
                }
                for( i=0; i< padding; i++ )   *(ptbits++) = zero;
            }
            ::StretchDIBits( memdc, 0, origin, width, height,\
                0, 0, width, height, lpBits, lpDIBh, DIB_RGB_COLORS, SRCCOPY);
            origin += height;
        }
        // create a wxMask object
        wxMask *mask = new wxMask();
        mask->SetMaskBitmap( (WXHBITMAP) hbitmap );
        bitmap.SetMask( mask );
    }

    // free allocated resources
    ::SelectObject( memdc, 0 );
    ::DeleteDC( memdc );
    ::ReleaseDC(NULL, hdc);
    free(lpDIBh);
    free(lpBits);

    // check the wxBitmap object
    if( bitmap.GetHBITMAP() )
        bitmap.SetOk( TRUE );
    else
        bitmap.SetOk( FALSE );
*/

    return TRUE;
}

wxImage wxBitmap::ConvertToImage() const
{
    wxImage image;
    
    wxCHECK_MSG( Ok(), wxNullImage, wxT("invalid bitmap") );

    // create an wxImage object
    int width = GetWidth();
    int height = GetHeight();
    image.Create( width, height );
    unsigned char *data = image.GetData();
    if( !data )
    {
        wxFAIL_MSG( wxT("could not allocate data for image") );
        return wxNullImage;
    }

    // calc the number of bytes per scanline and padding in the DIB
    int bytePerLine = width*3;
    int sizeDWORD = sizeof( DWORD );
    int lineBoundary =  bytePerLine % sizeDWORD;
    int padding = 0;
    if( lineBoundary > 0 )
    {
        padding = sizeDWORD - lineBoundary;
        bytePerLine += padding;
    }
// TODO:
/*
    // create a DIB header
    int headersize = sizeof(BITMAPINFOHEADER);
    LPBITMAPINFO lpDIBh = (BITMAPINFO *) malloc( headersize );
    if( !lpDIBh )
    {
        wxFAIL_MSG( wxT("could not allocate data for DIB header") );
        free( data );
        return;
    }
    // Fill in the DIB header
    lpDIBh->bmiHeader.biSize = headersize;
    lpDIBh->bmiHeader.biWidth = width;
    lpDIBh->bmiHeader.biHeight = -height;
    lpDIBh->bmiHeader.biSizeImage = bytePerLine * height;
    lpDIBh->bmiHeader.biPlanes = 1;
    lpDIBh->bmiHeader.biBitCount = 24;
    lpDIBh->bmiHeader.biCompression = BI_RGB;
    lpDIBh->bmiHeader.biClrUsed = 0;
    // These seem not really needed for our purpose here.
    lpDIBh->bmiHeader.biClrImportant = 0;
    lpDIBh->bmiHeader.biXPelsPerMeter = 0;
    lpDIBh->bmiHeader.biYPelsPerMeter = 0;
    // memory for DIB data
    unsigned char *lpBits;
    lpBits = (unsigned char *) malloc( lpDIBh->bmiHeader.biSizeImage );
    if( !lpBits )
    {
        wxFAIL_MSG( wxT("could not allocate data for DIB") );
        free( data );
        free( lpDIBh );
        return;
    }

    // copy data from the device-dependent bitmap to the DIB
    HDC hdc = ::GetDC(NULL);
    HBITMAP hbitmap;
    hbitmap = (HBITMAP) bitmap.GetHBITMAP();
    ::GetDIBits( hdc, hbitmap, 0, height, lpBits, lpDIBh, DIB_RGB_COLORS );

    // copy DIB data into the wxImage object
    int i, j;
    unsigned char *ptdata = data;
    unsigned char *ptbits = lpBits;
    for( i=0; i<height; i++ )
    {
        for( j=0; j<width; j++ )
        {
            *(ptdata++) = *(ptbits+2);
            *(ptdata++) = *(ptbits+1);
            *(ptdata++) = *(ptbits  );
            ptbits += 3;
        }
        ptbits += padding;
    }

    // similarly, set data according to the possible mask bitmap
    if( bitmap.GetMask() && bitmap.GetMask()->GetMaskBitmap() )
    {
        hbitmap = (HBITMAP) bitmap.GetMask()->GetMaskBitmap();
        // memory DC created, color set, data copied, and memory DC deleted
        HDC memdc = ::CreateCompatibleDC( hdc );
        ::SetTextColor( memdc, RGB( 0, 0, 0 ) );
        ::SetBkColor( memdc, RGB( 255, 255, 255 ) );
        ::GetDIBits( memdc, hbitmap, 0, height, lpBits, lpDIBh, DIB_RGB_COLORS );
        ::DeleteDC( memdc );
        // background color set to RGB(16,16,16) in consistent with wxGTK
        unsigned char r=16, g=16, b=16;
        ptdata = data;
        ptbits = lpBits;
        for( i=0; i<height; i++ )
        {
            for( j=0; j<width; j++ )
            {
                if( *ptbits != 0 )
                    ptdata += 3;
                else
                {
                    *(ptdata++)  = r;
                    *(ptdata++)  = g;
                    *(ptdata++)  = b;
                }
                ptbits += 3;
            }
            ptbits += padding;
        }
        SetMaskColour( r, g, b );
        SetMask( TRUE );
    }
    else
    {
        SetMask( FALSE );
    }
    // free allocated resources
    ::ReleaseDC(NULL, hdc);
    free(lpDIBh);
    free(lpBits);
*/

    return image;
}


// ----------------------------------------------------------------------------
// sub bitmap extraction
// ----------------------------------------------------------------------------

wxBitmap wxBitmap::GetSubBitmap(
  const wxRect&                     rRect
) const
{
    wxCHECK_MSG( Ok() &&
                 (rRect.x >= 0) && (rRect.y >= 0) &&
                 (rRect.x + rRect.width <= GetWidth()) &&
                 (rRect.y + rRect.height <= GetHeight()),
                 wxNullBitmap, wxT("Invalid bitmap or bitmap region") );

    wxBitmap                        vRet( rRect.width
                                         ,rRect.height
                                         ,GetDepth()
                                        );
    wxASSERT_MSG( vRet.Ok(), wxT("GetSubBitmap error") );


    //
    // Copy bitmap data
    //
    SIZEL                           vSize = {0, 0};
    DEVOPENSTRUC                    vDop = {0L, "DISPLAY", NULL, 0L, 0L, 0L, 0L, 0L, 0L};
    HDC                             hDCSrc = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&vDop, NULLHANDLE);
    HDC                             hDCDst = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&vDop, NULLHANDLE);
    HPS                             hPSSrc = ::GpiCreatePS(vHabmain, hDCSrc, &vSize, PU_PELS | GPIA_ASSOC);
    HPS                             hPSDst = ::GpiCreatePS(vHabmain, hDCDst, &vSize, PU_PELS | GPIA_ASSOC);
    POINTL                          vPoint[4] = { rRect.x, rRect.y,
                                                  rRect.x + rRect.width, rRect.y + rRect.height,
                                                  0, 0, GetWidth(), GetHeight()
                                                };

    ::GpiSetBitmap(hPSSrc, (HBITMAP) GetHBITMAP());
    ::GpiSetBitmap(hPSDst, (HBITMAP) vRet.GetHBITMAP());
    ::GpiBitBlt( hPSDst
                ,hPSSrc
                ,4L
                ,vPoint
                ,ROP_SRCCOPY
                ,BBO_IGNORE
               );

    //
    // Copy mask if there is one
    //
    if (GetMask())
    {
        BITMAPINFOHEADER2           vBmih;

        memset(&vBmih, '\0', sizeof(BITMAPINFOHEADER2));
        vBmih.cbFix     = sizeof(BITMAPINFOHEADER2);
        vBmih.cx        = rRect.width;
        vBmih.cy        = rRect.height;
        vBmih.cPlanes   = 1;
        vBmih.cBitCount = 1;

        HBITMAP                     hBmpMask = ::GpiCreateBitmap( hPSDst
                                                                 ,&vBmih
                                                                 ,0L
                                                                 ,NULL
                                                                 ,NULL
                                                                );

        ::GpiSetBitmap(hPSSrc, (HBITMAP) GetHBITMAP());
        ::GpiSetBitmap(hPSDst, (HBITMAP) vRet.GetHBITMAP());

        ::GpiSetBitmap(hPSSrc, (HBITMAP) GetMask()->GetMaskBitmap());
        ::GpiSetBitmap(hPSDst, (HBITMAP) hBmpMask);
        ::GpiBitBlt( hPSDst
                    ,hPSSrc
                    ,4L
                    ,vPoint
                    ,ROP_SRCCOPY
                    ,BBO_IGNORE
                   );

        wxMask*                     pMask = new wxMask((WXHBITMAP)hBmpMask);
        vRet.SetMask(pMask);
    }

    ::GpiSetBitmap(hPSSrc, NULL);
    ::GpiSetBitmap(hPSDst, NULL);
    ::GpiDestroyPS(hPSSrc);
    ::GpiDestroyPS(hPSDst);
    ::DevCloseDC(hDCSrc);
    ::DevCloseDC(hDCDst);
    return vRet;
} // end of wxBitmap::GetSubBitmap

// ----------------------------------------------------------------------------
// wxBitmap accessors
// ----------------------------------------------------------------------------

void wxBitmap::SetQuality(
  int                               nQ
)
{
    EnsureHasData();

    GetBitmapData()->m_nQuality = nQ;
} // end of wxBitmap::SetQuality

#if WXWIN_COMPATIBILITY_2
void wxBitmap::SetOk(
  bool                              bOk
)
{
    EnsureHasData();

    GetBitmapData()->m_bOk = bOk;
} // end of wxBitmap::SetOk
#endif // WXWIN_COMPATIBILITY_2

void wxBitmap::SetPalette(
  const wxPalette&                  rPalette
)
{
    EnsureHasData();

    GetBitmapData()->m_vBitmapPalette = rPalette;
} // end of wxBitmap::SetPalette

void wxBitmap::SetMask(
  wxMask*                           pMask
)
{
    EnsureHasData();

    GetBitmapData()->m_pBitmapMask = pMask;
} // end of wxBitmap::SetMask

//
// Will try something for OS/2 but not really sure how close
// to the msw intent this is.
//
wxBitmap wxBitmap::GetBitmapForDC(
  wxDC&                             rDc
) const
{
    wxMemoryDC                      vMemDC;
    wxBitmap                        vTmpBitmap( this->GetWidth()
                                               ,this->GetHeight()
                                               ,rDc.GetDepth()
                                              );
    WXHBITMAP                       vOldBitmap;
    HPS                             hMemoryPS;
    HPS                             hPs;
    POINTL                          vPoint[4];
    SIZEL                           vSize = {0,0};

    hMemoryPS = ::GpiCreatePS(vHabmain, (HDC)vMemDC.GetHDC(), &vSize, PU_PELS | GPIT_MICRO | GPIA_ASSOC);
    hPs       = ::GpiCreatePS(vHabmain, (HDC)rDc.GetHDC(), &vSize, PU_PELS | GPIT_MICRO | GPIA_ASSOC);

    // TODO: Set the points

    vOldBitmap = (WXHBITMAP)::GpiSetBitmap(hPs, (HBITMAP)vTmpBitmap.GetHBITMAP());
    ::GpiBitBlt(hPs, hMemoryPS, 4L, vPoint, ROP_SRCCOPY, BBO_IGNORE);

    return(vTmpBitmap);
} // end of wxBitmap::GetBitmapForDC

// ----------------------------------------------------------------------------
// wxMask
// ----------------------------------------------------------------------------

wxMask::wxMask()
{
    m_hMaskBitmap = 0;
} // end of wxMask::wxMask

// Construct a mask from a bitmap and a colour indicating
// the transparent area
wxMask::wxMask(
  const wxBitmap&                   rBitmap
, const wxColour&                   rColour
)
{
    m_hMaskBitmap = 0;
    Create( rBitmap
           ,rColour
          );
} // end of wxMask::wxMask

// Construct a mask from a bitmap and a palette index indicating
// the transparent area
wxMask::wxMask(
  const wxBitmap&                   rBitmap
, int                               nPaletteIndex
)
{
    m_hMaskBitmap = 0;
    Create( rBitmap
           ,nPaletteIndex
          );
} // end of wxMask::wxMask

// Construct a mask from a mono bitmap (copies the bitmap).
wxMask::wxMask(
  const wxBitmap&                   rBitmap
)
{
    m_hMaskBitmap = 0;
    Create(rBitmap);
} // end of wxMask::wxMask

wxMask::~wxMask()
{
    if (m_hMaskBitmap)
        ::GpiDeleteBitmap((HBITMAP)m_hMaskBitmap);
} // end of wxMask::~wxMask

// Create a mask from a mono bitmap (copies the bitmap).
bool wxMask::Create(
  const wxBitmap&                   rBitmap
)
{
    BITMAPINFOHEADER2               vBmih;
    SIZEL                           vSize = {0, 0};
    DEVOPENSTRUC                    vDop = {0L, "DISPLAY", NULL, 0L, 0L, 0L, 0L, 0L, 0L};
    HDC                             hDCSrc = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&vDop, NULLHANDLE);
    HDC                             hDCDst = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&vDop, NULLHANDLE);
    HPS                             hPSSrc = ::GpiCreatePS(vHabmain, hDCSrc, &vSize, PU_PELS | GPIA_ASSOC);
    HPS                             hPSDst = ::GpiCreatePS(vHabmain, hDCDst, &vSize, PU_PELS | GPIA_ASSOC);
    POINTL                          vPoint[4] = { 0 ,0, rBitmap.GetWidth(), rBitmap.GetHeight(),
                                                  0, 0, rBitmap.GetWidth(), rBitmap.GetHeight()
                                                };

    if (m_hMaskBitmap)
    {
        ::GpiDeleteBitmap((HBITMAP) m_hMaskBitmap);
        m_hMaskBitmap = 0;
    }
    if (!rBitmap.Ok() || rBitmap.GetDepth() != 1)
    {
        return(FALSE);
    }

    memset(&vBmih, '\0', sizeof(BITMAPINFOHEADER2));
    vBmih.cbFix     =  sizeof(BITMAPINFOHEADER2);
    vBmih.cx        = rBitmap.GetWidth();
    vBmih.cy        = rBitmap.GetHeight();
    vBmih.cPlanes   = 1;
    vBmih.cBitCount = 1;

    m_hMaskBitmap = ::GpiCreateBitmap( hPSDst
                                      ,&vBmih
                                      ,0L
                                      ,NULL
                                      ,NULL
                                     );

    ::GpiSetBitmap(hPSSrc, (HBITMAP) rBitmap.GetHBITMAP());
    ::GpiSetBitmap(hPSDst, (HBITMAP) m_hMaskBitmap);
    ::GpiBitBlt( hPSDst
                ,hPSSrc
                ,4L
                ,vPoint
                ,ROP_SRCCOPY
                ,BBO_IGNORE
               );

    ::GpiDestroyPS(hPSSrc);
    ::GpiDestroyPS(hPSDst);
    ::DevCloseDC(hDCSrc);
    ::DevCloseDC(hDCDst);
    return(TRUE);
} // end of wxMask::Create

// Create a mask from a bitmap and a palette index indicating
// the transparent area
bool wxMask::Create(
  const wxBitmap&                   rBitmap
, int                               nPaletteIndex
)
{
    if (m_hMaskBitmap)
    {
        ::GpiDeleteBitmap((HBITMAP) m_hMaskBitmap);
        m_hMaskBitmap = 0;
    }
    if (rBitmap.Ok() && rBitmap.GetPalette()->Ok())
    {
        unsigned char               cRed;
        unsigned char               cGreen;
        unsigned char               cBlue;

        if (rBitmap.GetPalette()->GetRGB( nPaletteIndex
                                         ,&cRed
                                         ,&cGreen
                                         ,&cBlue
                                        ))
        {
            wxColour                vTransparentColour( cRed
                                                       ,cGreen
                                                       ,cBlue
                                                      );

            return (Create( rBitmap
                           ,vTransparentColour
                          ));
        }
    }
    return(FALSE);
} // end of wxMask::Create

// Create a mask from a bitmap and a colour indicating
// the transparent area
bool wxMask::Create(
  const wxBitmap&                   rBitmap
, const wxColour&                   rColour
)
{
    bool                            bOk = TRUE;
    COLORREF                        vMaskColour = OS2RGB( rColour.Red()
                                                         ,rColour.Green()
                                                         ,rColour.Blue()
                                                        );
    BITMAPINFOHEADER2               vBmih;
    SIZEL                           vSize = {0, 0};
    DEVOPENSTRUC                    vDop = { NULL, "DISPLAY", NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    HDC                             hDCSrc = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&vDop, NULLHANDLE);
    HDC                             hDCDst = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&vDop, NULLHANDLE);
    HPS                             hPSSrc = ::GpiCreatePS(vHabmain, hDCSrc, &vSize, PU_PELS | GPIA_ASSOC);
    HPS                             hPSDst = ::GpiCreatePS(vHabmain, hDCDst, &vSize, PU_PELS | GPIA_ASSOC);
    POINTL                          vPoint[4] = { 0 ,0, rBitmap.GetWidth(), rBitmap.GetHeight(),
                                                  0, 0, rBitmap.GetWidth(), rBitmap.GetHeight()
                                                };

    if (m_hMaskBitmap)
    {
        ::GpiDeleteBitmap((HBITMAP) m_hMaskBitmap);
        m_hMaskBitmap = 0;
    }
    if (!rBitmap.Ok())
    {
        return(FALSE);
    }

    //
    // Scan the bitmap for the transparent colour and set
    // the corresponding pixels in the mask to BLACK and
    // the rest to WHITE
    //

    memset(&vBmih, '\0', sizeof(BITMAPINFOHEADER2));
    vBmih.cbFix     =  sizeof(BITMAPINFOHEADER2);
    vBmih.cx        = rBitmap.GetWidth();
    vBmih.cy        = rBitmap.GetHeight();
    vBmih.cPlanes   = 1;
    vBmih.cBitCount = 1;

    m_hMaskBitmap = ::GpiCreateBitmap( hPSDst
                                      ,&vBmih
                                      ,0L
                                      ,NULL
                                      ,NULL
                                     );

    ::GpiSetBitmap(hPSSrc, (HBITMAP) rBitmap.GetHBITMAP());
    ::GpiSetBitmap(hPSDst, (HBITMAP) m_hMaskBitmap);

    //
    // This is not very efficient, but I can't think
    // of a better way of doing it
    //
    for (int w = 0; w < rBitmap.GetWidth(); w++)
    {
        for (int h = 0; h < rBitmap.GetHeight(); h++)
        {
            POINTL                  vPt = {w, h};
            COLORREF                vCol = (COLORREF)::GpiQueryPel(hPSSrc, &vPt);
            if (vCol == (COLORREF)CLR_NOINDEX)
            {
                //
                // Doesn't make sense to continue
                //
                bOk = FALSE;
                break;
            }

            if (vCol == vMaskColour)
            {
                ::GpiSetColor(hPSDst, OS2RGB(0, 0, 0));
                ::GpiSetPel(hPSDst, &vPt);
            }
            else
            {
                ::GpiSetColor(hPSDst, OS2RGB(255, 255, 255));
                ::GpiSetPel(hPSDst, &vPt);
            }
        }
    }
    ::GpiSetBitmap(hPSSrc, NULL);
    ::GpiSetBitmap(hPSDst, NULL);
    ::GpiDestroyPS(hPSSrc);
    ::GpiDestroyPS(hPSDst);
    ::DevCloseDC(hDCSrc);
    ::DevCloseDC(hDCDst);
    return(TRUE);
} // end of wxMask::Create

// ----------------------------------------------------------------------------
// wxBitmapHandler
// ----------------------------------------------------------------------------

bool wxBitmapHandler::Create(
  wxGDIImage*                       pImage
, void*                             pData
, long                              lFlags
, int                               nWidth
, int                               nHeight
, int                               nDepth
)
{
    wxBitmap*                       pBitmap = wxDynamicCast( pImage
                                                            ,wxBitmap
                                                           );

    return(pBitmap ? Create( pBitmap
                            ,pData
                            ,nWidth
                            ,nHeight
                            ,nDepth
                           ) : FALSE);
}

bool wxBitmapHandler::Load(
  wxGDIImage*                       pImage
, const wxString&                   rName
, HPS                               hPs
, long                              lFlags
, int                               nWidth
, int                               nHeight
)
{
    wxBitmap*                       pBitmap = wxDynamicCast( pImage
                                                            ,wxBitmap
                                                           );

    return(pBitmap ? LoadFile( pBitmap
                              ,rName
                              ,hPs
                              ,lFlags
                              ,nWidth
                              ,nHeight
                             ) : FALSE);
}

bool wxBitmapHandler::Save(
  wxGDIImage*                       pImage
, const wxString&                   rName
, int                               lType
)
{
    wxBitmap*                       pBitmap = wxDynamicCast( pImage
                                                            ,wxBitmap
                                                           );

    return(pBitmap ? SaveFile( pBitmap
                              ,rName
                              ,lType
                             ) : FALSE);
}

bool wxBitmapHandler::Create(
  wxBitmap*                         WXUNUSED(pBitmap)
, void*                             WXUNUSED(pData)
, long                              WXUNUSED(lType)
, int                               WXUNUSED(nWidth)
, int                               WXUNUSED(nHeight)
, int                               WXUNUSED(nDepth)
)
{
    return(FALSE);
}

bool wxBitmapHandler::LoadFile(
  wxBitmap*                         WXUNUSED(pBitmap)
, const wxString&                   WXUNUSED(rName)
, HPS                               WXUNUSED(hPs)
, long                              WXUNUSED(lType)
, int                               WXUNUSED(nDesiredWidth)
, int                               WXUNUSED(nDesiredHeight)
)
{
    return(FALSE);
}

bool wxBitmapHandler::SaveFile(
  wxBitmap*                         WXUNUSED(pBitmap)
, const wxString&                   WXUNUSED(rName)
, int                               WXUNUSED(nType)
, const wxPalette*                  WXUNUSED(pPalette)
)
{
    return(FALSE);
}

// ----------------------------------------------------------------------------
// Utility functions
// ----------------------------------------------------------------------------
HBITMAP wxInvertMask(
  HBITMAP                           hBmpMask
, int                               nWidth
, int                               nHeight
)
{
    HBITMAP                         hBmpInvMask = 0;

    wxCHECK_MSG( hBmpMask, 0, _T("invalid bitmap in wxInvertMask") );

    //
    // Get width/height from the bitmap if not given
    //
    if (!nWidth || !nHeight)
    {
        BITMAPINFOHEADER2           vBmhdr;

        ::GpiQueryBitmapInfoHeader( hBmpMask
                                   ,&vBmhdr
                                  );
        nWidth  = (int)vBmhdr.cx;
        nHeight = (int)vBmhdr.cy;
    }

    BITMAPINFOHEADER2               vBmih;
    SIZEL                           vSize = {0, 0};
    DEVOPENSTRUC                    vDop = {0L, "DISPLAY", NULL, 0L, 0L, 0L, 0L, 0L, 0L};
    HDC                             hDCSrc = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&vDop, NULLHANDLE);
    HDC                             hDCDst = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&vDop, NULLHANDLE);
    HPS                             hPSSrc = ::GpiCreatePS(vHabmain, hDCSrc, &vSize, PU_PELS | GPIA_ASSOC);
    HPS                             hPSDst = ::GpiCreatePS(vHabmain, hDCDst, &vSize, PU_PELS | GPIA_ASSOC);
    POINTL                          vPoint[4] = { 0 ,0, nWidth, nHeight,
                                                  0, 0, nWidth, nHeight
                                                };

    memset(&vBmih, '\0', sizeof(BITMAPINFOHEADER2));
    vBmih.cbFix     =  sizeof(BITMAPINFOHEADER2);
    vBmih.cx        = nWidth;
    vBmih.cy        = nHeight;
    vBmih.cPlanes   = 1;
    vBmih.cBitCount = 1;

    hBmpInvMask = ::GpiCreateBitmap( hPSDst
                                    ,&vBmih
                                    ,0L
                                    ,NULL
                                    ,NULL
                                   );

    ::GpiSetBitmap(hPSSrc, (HBITMAP) hBmpMask);
    ::GpiSetBitmap(hPSDst, (HBITMAP) hBmpInvMask);

    ::GpiBitBlt( hPSDst
                ,hPSSrc
                ,4L
                ,vPoint
                ,ROP_SRCINVERT
                ,BBO_IGNORE
               );

    ::GpiDestroyPS(hPSSrc);
    ::GpiDestroyPS(hPSDst);
    ::DevCloseDC(hDCSrc);
    ::DevCloseDC(hDCDst);

    return hBmpInvMask;
} // end of WxWinGdi_InvertMask

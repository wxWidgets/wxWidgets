/////////////////////////////////////////////////////////////////////////////
// Name:        os2/xpmhand.cpp
// Purpose:     wxBitmap: XPM handler and constructors
// Author:      Julian Smart
// Modified for OS/2 by: Evgeny Kotsuba
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "xpmhand.h"
#endif

// For compilers that support precompilation, includes "wx.h".
//#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
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


#if wxUSE_XPM_IN_OS2
    #define FOR_MSW 1
    #include "../xpm/xpm.h"
#endif

#include  "wx/xpmhand.h"

#if wxUSE_XPM_IN_OS2

static void XpmToBitmap(
  wxBitmap*                         pBitmap
, const XImage*                     pXimage
, const XImage*                     pXmask
, const XpmAttributes&              rXpmAttr
)
{
    wxBitmapRefData*                pRefData = pBitmap->GetBitmapData();
    pRefData->m_hBitmap = (WXHBITMAP)pXimage->bitmap;

    //
    // First set the bitmap width, height, depth...
    //
    BITMAPINFOHEADER                vBm;

    if (!::GpiQueryBitmapParameters( GetHbitmapOf(*pBitmap)
                                    ,&vBm
                                   ))
    {
        wxLogLastError(wxT("GetObject(pBitmap)"));
    }

    pRefData->m_nWidth     = vBm.cx;
    pRefData->m_nHeight    = vBm.cy;
    pRefData->m_nDepth     = vBm.cPlanes * vBm.cBitCount;
    pRefData->m_nNumColors = rXpmAttr.npixels;

    if (pXmask)
    {
        wxMask*                     pMask = new wxMask();

        pMask->SetMaskBitmap((WXHBITMAP) wxInvertMask( pXmask->bitmap
                                                      ,vBm.cx
                                                      ,vBm.cy
                                                     ));
        pBitmap->SetMask(pMask);
    }
} // end of XpmToBitmap

IMPLEMENT_DYNAMIC_CLASS(wxXPMFileHandler, wxBitmapHandler)

bool wxXPMFileHandler::LoadFile(
  wxBitmap*                         pBitmap
, const wxString&                   rName
, long                              lFlags
, int                               lDesiredWidth
, int                               lDesiredHeight
)
{
    XImage*                         pXimage = NULL;
    XImage*                         pXmask = NULL;
    XpmAttributes                   vXpmAttr;
    HDC                             hDC;
    HPS                             hPS;
    DEVOPENSTRUC                    dOP = {0L, "DISPLAY", NULL, 0L, 0L, 0L, 0L, 0L, 0L};
    SIZEL                           vSizl;

    hDC = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&dOP, NULLHANDLE);
    hPS = ::GpiCreatePS(vHabmain, hDC, &vSizl, PU_PELS | GPIT_MICRO | GPIA_ASSOC);
    if (hPS)
    {
        vXpmAttr.valuemask = XpmReturnPixels | XpmColorTable;

        int                         nErrorStatus = XpmReadFileToImage( &hPS
                                                                      ,wxMBSTRINGCAST rName.fn_str()
                                                                      ,&pXimage
                                                                      ,&pXmask
                                                                      ,&vXpmAttr
                                                                     );
        ::GpiDestroyPS(hPS);
        ::DevCloseDC(hDC);
        if (nErrorStatus == XpmSuccess)
        {
            XpmToBitmap( pBitmap
                        ,pXimage
                        ,pXmask
                        ,vXpmAttr
                       );
            XpmFree(vXpmAttr.pixels);
            XpmFreeAttributes(&vXpmAttr);
            XImageFree(pXimage);
            if (pXmask)
                XDestroyImage(pXmask);
        }
        return pBitmap->Ok();
    }
    return FALSE;
} // end of wxXPMFileHandler::LoadFile

bool wxXPMFileHandler::SaveFile(
  wxBitmap*                         pBitmap
, const wxString&                   rName
, int                               lType
, const wxPalette*                  pPalette
)
{
    XImage                          vXimage;
    XImage                          vXmask;
    bool                            bHasmask = FALSE;
    HDC                             hDC;
    HPS                             hPS;
    DEVOPENSTRUC                    dOP = {0L, "DISPLAY", NULL, 0L, 0L, 0L, 0L, 0L, 0L};
    SIZEL                           vSizl;

    hDC = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&dOP, NULLHANDLE);
    hPS = ::GpiCreatePS(vHabmain, hDC, &vSizl, PU_PELS | GPIT_MICRO | GPIA_ASSOC);
    if (hPS)
    {
        // fill the XImage struct 'by hand'
        wxBitmapRefData*            pRefData = pBitmap->GetBitmapData();

        vXimage.width  = pRefData->m_nWidth;
        vXimage.height = pRefData->m_nHeight;
        vXimage.depth  = pRefData->m_nDepth;
        vXimage.bitmap = (HBITMAP)pRefData->m_hBitmap;

        bHasmask = (pRefData->m_pBitmapMask != NULL);
        if (bHasmask)
        {
            //
            // Strangely enough, the MSW version of xpmlib is not
            // coherent with itself regarding masks; we have to invert
            // the mask we get when loading, but we still must pass it
            // 'as is' when saving...
            //
            vXmask.bitmap = (HBITMAP)pRefData->m_pBitmapMask->GetMaskBitmap();
            vXmask.width  = pRefData->m_nWidth;
            vXmask.height = pRefData->m_nHeight;
            vXmask.depth  = 1;
        }

        int                         nErrorStatus = XpmWriteFileFromImage( &hPS
                                                                         ,wxMBSTRINGCAST rName.fn_str()
                                                                         ,&vXimage
                                                                         ,(bHasmask? &vXmask : (XImage *)NULL)
                                                                         ,(XpmAttributes *) NULL
                                                                        );
        ::GpiDestroyPS(hPS);
        ::DevCloseDC(hDC);
        return (nErrorStatus == XpmSuccess);
    }
    return FALSE;
} // end of wxXPMFileHandler::SaveFile

IMPLEMENT_DYNAMIC_CLASS(wxXPMDataHandler, wxBitmapHandler)

bool wxXPMDataHandler::Create(
  wxBitmap*                         pBitmap
, void*                             pData
, long                              lFlags
, int                               nWidth
, int                               nHeight
, int                               nDepth
)
{
    XImage*                         pXimage = NULL;
    XImage*                         pXmask = NULL;
    XpmAttributes                   vXpmAttr;
    HDC                             hDC;
    HPS                             hPS;
    DEVOPENSTRUC                    dOP = {0L, "DISPLAY", NULL, 0L, 0L, 0L, 0L, 0L, 0L};
    SIZEL                           vSizl;

    hDC = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&dOP, NULLHANDLE);
    hPS = ::GpiCreatePS(vHabmain, hDC, &vSizl, PU_PELS | GPIT_MICRO | GPIA_ASSOC);
    if (hPS)
    {
        vXpmAttr.valuemask = XpmReturnInfos | XpmColorTable;

        int                         nErrorStatus = XpmCreateImageFromData( &hPS
                                                                          ,(char **)pData
                                                                          ,&pXimage
                                                                          ,&pXmask
                                                                          ,&vXpmAttr
                                                                         );
        ::GpiDestroyPS(hPS);
        ::DevCloseDC(hDC);
        if (nErrorStatus == XpmSuccess)
        {
            XpmToBitmap( pBitmap
                        ,pXimage
                        ,pXmask
                        ,vXpmAttr
                       );
            XpmFree(vXpmAttr.pixels);
            XpmFreeAttributes(&vXpmAttr);
            XImageFree(pXimage); // releases the malloc, but does not destroy bitmap
            if (pXmask)
                XDestroyImage(pXmask);
        }
        return pBitmap->Ok();
    }
    return FALSE;
} // end of wxXPMDataHandler::Create

#endif // wxUSE_XPM_IN_OS2

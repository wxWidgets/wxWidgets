/////////////////////////////////////////////////////////////////////////////
// Name:        icon.cpp
// Purpose:     wxIcon class
// Author:      David Webster
// Modified by:
// Created:     10/09/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/list.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/icon.h"
#endif

#include "wx/os2/private.h"
#include "assert.h"

#include "wx/icon.h"

    IMPLEMENT_DYNAMIC_CLASS(wxIcon, wxIconBase)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxIconRefData
// ----------------------------------------------------------------------------

void wxIconRefData::Free()
{
    if (m_hIcon)
        ::WinFreeFileIcon((HPOINTER)m_hIcon);
}

// ----------------------------------------------------------------------------
// wxIcon
// ----------------------------------------------------------------------------

wxIcon::wxIcon()
{
}

wxIcon::wxIcon(
  const char                        WXUNUSED(bits)[]
, int                               WXUNUSED(nWidth)
, int                               WXUNUSED(nHeight)
)
{
}

wxIcon::wxIcon(
  const wxString&                   rIconFile
, long                              lFlags
, int                               nDesiredWidth
, int                               nDesiredHeight
)
{
    //
    // A very poor hack, but we have to have separate icon files from windows
    // So we have a modified name where replace the last three characters
    // with os2.  Also need the extension.
    //
    wxString                         sOs2Name = rIconFile.Mid(0, rIconFile.Length() - 3);

    sOs2Name += "Os2.ico";
    LoadFile( sOs2Name
             ,lFlags
             ,nDesiredWidth
             ,nDesiredHeight
            );
}

wxIcon::~wxIcon()
{
}

void wxIcon::CreateIconFromXpm(
  const char**                      ppData
)
{
    wxBitmap                        vBmp(ppData);

    CopyFromBitmap(vBmp);
} // end of wxIcon::CreateIconFromXpm

void wxIcon::CopyFromBitmap(
  const wxBitmap&                   rBmp
)
{
    wxMask*                         pMask = rBmp.GetMask();

    if (!pMask)
    {
        //
        // We must have a mask for an icon, so even if it's probably incorrect,
        // do create it (grey is the "standard" transparent colour)
        //
        pMask = new wxMask( rBmp
                           ,*wxLIGHT_GREY
                          );
    }

    POINTERINFO                        vIconInfo;

    memset(&vIconInfo, '\0', sizeof(POINTERINFO));
    vIconInfo.fPointer = FALSE;  // we want an icon, not a pointer
    vIconInfo.hbmColor = GetHbitmapOf(rBmp);

    SIZEL                           vSize = {0, 0};
    DEVOPENSTRUC                    vDop = {0L, "DISPLAY", NULL, 0L, 0L, 0L, 0L, 0L, 0L};
    HDC                             hDCSrc = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&vDop, NULLHANDLE);
    HDC                             hDCDst = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&vDop, NULLHANDLE);
    HPS                             hPSSrc = ::GpiCreatePS(vHabmain, hDCSrc, &vSize, PU_PELS | GPIA_ASSOC);
    HPS                             hPSDst = ::GpiCreatePS(vHabmain, hDCDst, &vSize, PU_PELS | GPIA_ASSOC);
    POINTL                          vPoint[4] = { 0, 0, rBmp.GetWidth(), rBmp.GetHeight(),
                                                  0, 0, rBmp.GetWidth(), rBmp.GetHeight()
                                                };
    ::GpiSetBitmap(hPSSrc, (HBITMAP) pMask->GetMaskBitmap());
    ::GpiSetBitmap(hPSDst, (HBITMAP) vIconInfo.hbmColor);
    ::GpiBitBlt( hPSDst
                ,hPSSrc
                ,4L
                ,vPoint
                ,ROP_SRCAND
                ,BBO_IGNORE
               );

    ::GpiSetBitmap(hPSSrc, NULL);
    ::GpiSetBitmap(hPSDst, NULL);
    ::GpiDestroyPS(hPSSrc);
    ::GpiDestroyPS(hPSDst);
    ::DevCloseDC(hDCSrc);
    ::DevCloseDC(hDCDst);

    HICON                           hIcon = ::WinCreatePointerIndirect( HWND_DESKTOP
                                                                       ,&vIconInfo
                                                                      );

    if (!hIcon)
    {
        wxLogLastError(wxT("WinCreatePointerIndirect"));
    }
    else
    {
        SetHICON((WXHICON)hIcon);
        SetSize( rBmp.GetWidth()
                ,rBmp.GetHeight()
               );
    }

    if (!rBmp.GetMask())
    {
        //
        // We created the mask, now delete it
        //
        delete pMask;
    }
} // end of wxIcon::CopyFromBitmap

bool wxIcon::LoadFile(
  const wxString&                   rFilename
, long                              lType
, int                               nDesiredWidth
, int                               nDesiredHeight
)
{
    HPS                             hPs = NULLHANDLE;

    UnRef();

    wxGDIImageHandler*              pHandler = FindHandler(lType);

    if (pHandler)
        return(pHandler->Load( this
                              ,rFilename
                              ,hPs
                              ,lType
                              ,nDesiredWidth
                              ,nDesiredHeight
                             ));
    else
        return(FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// Name:        xpmhand.cpp
// Purpose:     wxBitmap: XPM handler and constructors
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "xpmhand.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/setup.h"
#include "wx/list.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/palette.h"
#include "wx/dcmemory.h"
#include "wx/bitmap.h"
#include "wx/icon.h"
#endif

#include "wx/msw/private.h"
#include "wx/log.h"

#include "assert.h"

#if wxUSE_XPM_IN_MSW
#define FOR_MSW 1
#include "../xpm/xpm34.h"
#endif

#include "wx/xpmhand.h"
#include "wx/msw/dib.h"

IMPLEMENT_DYNAMIC_CLASS(wxXPMFileHandler, wxBitmapHandler)

bool wxXPMFileHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
    int desiredWidth, int desiredHeight)
{
#if wxUSE_XPM_IN_MSW
    XImage *ximage;
    XpmAttributes xpmAttr;
    HDC     dc;

    M_BITMAPHANDLERDATA->m_ok = FALSE;
    dc = CreateCompatibleDC(NULL);
    if (dc)
    {
      xpmAttr.valuemask = XpmReturnPixels;
      int errorStatus = XpmReadFileToImage(&dc, wxMBSTRINGCAST name.fn_str(), &ximage, (XImage **) NULL, &xpmAttr);
      DeleteDC(dc);
      if (errorStatus == XpmSuccess)
      {
        M_BITMAPHANDLERDATA->m_hBitmap = (WXHBITMAP) ximage->bitmap;

        BITMAP  bm;
        GetObject((HBITMAP)M_BITMAPHANDLERDATA->m_hBitmap, sizeof(bm), (LPSTR) & bm);

        M_BITMAPHANDLERDATA->m_width = (bm.bmWidth);
        M_BITMAPHANDLERDATA->m_height = (bm.bmHeight);
        M_BITMAPHANDLERDATA->m_depth = (bm.bmPlanes * bm.bmBitsPixel);
        M_BITMAPHANDLERDATA->m_numColors = xpmAttr.npixels;
        XpmFreeAttributes(&xpmAttr);
        XImageFree(ximage);

        M_BITMAPHANDLERDATA->m_ok = TRUE;
        return TRUE;
      }
      else
      {
        M_BITMAPHANDLERDATA->m_ok = FALSE;
        return FALSE;
      }
    }
#endif

    return FALSE;
}

bool wxXPMFileHandler::SaveFile(wxBitmap *bitmap, const wxString& name, int type, const wxPalette *palette)
{
#if wxUSE_XPM_IN_MSW
    HDC     dc = NULL;
    
    XImage  ximage;
    
    dc = CreateCompatibleDC(NULL);
    if (dc)
    {
        if (SelectObject(dc, (HBITMAP) M_BITMAPHANDLERDATA->m_hBitmap))
        {
            /* for following SetPixel */
            /* fill the XImage struct 'by hand' */
            ximage.width = M_BITMAPHANDLERDATA->m_width; 
            ximage.height = M_BITMAPHANDLERDATA->m_height;
            ximage.depth = M_BITMAPHANDLERDATA->m_depth; 
            ximage.bitmap = (HBITMAP)M_BITMAPHANDLERDATA->m_hBitmap;
            int errorStatus = XpmWriteFileFromImage(&dc, wxMBSTRINGCAST name.fn_str(),
                &ximage, (XImage *) NULL, (XpmAttributes *) NULL);
            
            if (dc)
                DeleteDC(dc);
            
            if (errorStatus == XpmSuccess)
                return TRUE;    /* no error */
            else
                return FALSE;
        } else return FALSE;
    } else return FALSE;
#else
    return FALSE;
#endif
}

IMPLEMENT_DYNAMIC_CLASS(wxXPMDataHandler, wxBitmapHandler)

bool wxXPMDataHandler::Create(wxBitmap *bitmap, void *data, long flags, int width, int height, int depth)
{
#if wxUSE_XPM_IN_MSW
  XImage *ximage;
  int     ErrorStatus;
  XpmAttributes xpmAttr;
  HDC     dc;

  M_BITMAPHANDLERDATA->m_ok = FALSE;
  M_BITMAPHANDLERDATA->m_numColors = 0;

  dc = CreateCompatibleDC(NULL);  /* memory DC */

  if (dc)
  {
    xpmAttr.valuemask = XpmReturnInfos; /* get infos back */
    ErrorStatus = XpmCreateImageFromData(&dc, (char **)data,
         &ximage, (XImage **) NULL, &xpmAttr);

    if (ErrorStatus == XpmSuccess)
    {
      /* ximage is malloced and contains bitmap and attributes */
      M_BITMAPHANDLERDATA->m_hBitmap = (WXHBITMAP) ximage->bitmap;

      BITMAP  bm;
      GetObject((HBITMAP) M_BITMAPHANDLERDATA->m_hBitmap, sizeof(bm), (LPSTR) & bm);

      M_BITMAPHANDLERDATA->m_width = (bm.bmWidth);
      M_BITMAPHANDLERDATA->m_height = (bm.bmHeight);
      M_BITMAPHANDLERDATA->m_depth = (bm.bmPlanes * bm.bmBitsPixel);
      M_BITMAPHANDLERDATA->m_numColors = xpmAttr.npixels;
      XpmFreeAttributes(&xpmAttr);

      XImageFree(ximage); // releases the malloc, but does not detroy
                          // the bitmap
      M_BITMAPHANDLERDATA->m_ok = TRUE;
      DeleteDC(dc);

      return TRUE;
    } 
    else
    {
      M_BITMAPHANDLERDATA->m_ok = FALSE;
//    XpmDebugError(ErrorStatus, NULL);
      DeleteDC(dc);
      return FALSE;
    }
  }
#endif

  return FALSE;
}


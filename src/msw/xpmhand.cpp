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

#if wxUSE_XPM_IN_MSW
    #define FOR_MSW 1
    #include "../xpm/xpm34.h"
#endif

#include "wx/xpmhand.h"
#include "wx/msw/dib.h"

#if wxUSE_XPM_IN_MSW

static void XpmToBitmap(wxBitmap *bitmap,
                        const XImage *ximage,
                        const XpmAttributes& xpmAttr)
{
    wxBitmapRefData *refData = bitmap->GetBitmapData();
    refData->m_hBitmap = (WXHBITMAP)ximage->bitmap;

    // first set the bitmap width, height, depth...
    BITMAP bm;
    if ( !::GetObject(GetHbitmapOf(*bitmap), sizeof(bm), (LPSTR) & bm) )
    {
        wxLogLastError("GetObject(bitmap)");
    }

    refData->m_width = bm.bmWidth;
    refData->m_height = bm.bmHeight;
    refData->m_depth = bm.bmPlanes * bm.bmBitsPixel;
    refData->m_numColors = xpmAttr.npixels;

    // next get the mask, if any
    if ( xpmAttr.mask_pixel != XpmUndefPixel )
    {
        int red, green, blue;
        const char *clrString = xpmAttr.colorTable[xpmAttr.mask_pixel].c_color;
        if ( strcmp(clrString, "None") == 0 )
        {
            // TODO what to do here??
            red = green = 0;
            blue = 1;
        }
        else
        {
            sscanf(clrString, "#%02x%02x%02x", &red, &green, &blue);
        }

        wxMask *mask = new wxMask(*bitmap, wxColour(red, green, blue));
        bitmap->SetMask(mask);
    }
}

#endif // wxUSE_XPM_IN_MSW

IMPLEMENT_DYNAMIC_CLASS(wxXPMFileHandler, wxBitmapHandler)

bool wxXPMFileHandler::LoadFile(wxBitmap *bitmap,
                                const wxString& name,
                                long flags,
                                int desiredWidth, int desiredHeight)
{
#if wxUSE_XPM_IN_MSW
    XImage *ximage;
    XpmAttributes xpmAttr;
    HDC     dc;

    dc = CreateCompatibleDC(NULL);
    if (dc)
    {
        xpmAttr.valuemask = XpmReturnPixels | XpmColorTable;
        int errorStatus = XpmReadFileToImage(&dc,
                                             wxMBSTRINGCAST name.fn_str(),
                                             &ximage,
                                             (XImage **)NULL,
                                             &xpmAttr);
        DeleteDC(dc);
        if (errorStatus == XpmSuccess)
        {
            XpmToBitmap(bitmap, ximage, xpmAttr);

            XpmFree(xpmAttr.pixels);
            XpmFreeAttributes(&xpmAttr);
            XImageFree(ximage);
        }

#if WXWIN_COMPATIBILITY_2
        bitmap->SetOk(errorStatus == XpmSuccess);
#endif // WXWIN_COMPATIBILITY_2

        return bitmap->Ok();
    }
#endif // wxUSE_XPM_IN_MSW

    return FALSE;
}

bool wxXPMFileHandler::SaveFile(wxBitmap *bitmap,
                                const wxString& name,
                                int type,
                                const wxPalette *palette)
{
#if wxUSE_XPM_IN_MSW
    XImage ximage;

    HDC dc = CreateCompatibleDC(NULL);
    if (dc)
    {
        if ( SelectObject(dc, GetHbitmapOf(*bitmap)) )
        {
            /* for following SetPixel */
            /* fill the XImage struct 'by hand' */
            wxBitmapRefData *refData = bitmap->GetBitmapData();
            ximage.width = refData->m_width;
            ximage.height = refData->m_height;
            ximage.depth = refData->m_depth;
            ximage.bitmap = (HBITMAP)refData->m_hBitmap;
            int errorStatus = XpmWriteFileFromImage(&dc, wxMBSTRINGCAST name.fn_str(),
                                                    &ximage, (XImage *) NULL,
                                                    (XpmAttributes *) NULL);

            if (dc)
                DeleteDC(dc);

            if (errorStatus == XpmSuccess)
                return TRUE;    /* no error */
        }
    }
#endif // !wxUSE_XPM_IN_MSW

    return FALSE;
}

IMPLEMENT_DYNAMIC_CLASS(wxXPMDataHandler, wxBitmapHandler)

bool wxXPMDataHandler::Create(wxBitmap *bitmap,
                              void *data,
                              long flags,
                              int width,
                              int height,
                              int depth)
{
#if wxUSE_XPM_IN_MSW
  XImage *ximage;
  XpmAttributes xpmAttr;

  HDC dc = CreateCompatibleDC(NULL);  /* memory DC */

  if (dc)
  {
      xpmAttr.valuemask = XpmReturnInfos | XpmColorTable;
      int errorStatus = XpmCreateImageFromData(&dc, (char **)data,
                                               &ximage,
                                               (XImage **) NULL,
                                               &xpmAttr);
      DeleteDC(dc);

      if ( errorStatus == XpmSuccess )
      {
          XpmToBitmap(bitmap, ximage, xpmAttr);

          XpmFree(xpmAttr.pixels);
          XpmFreeAttributes(&xpmAttr);
          XImageFree(ximage); // releases the malloc, but does not destroy bitmap
      }

#if WXWIN_COMPATIBILITY_2
      bitmap->SetOk(errorStatus == XpmSuccess);
#endif // WXWIN_COMPATIBILITY_2

      return bitmap->Ok();
  }
#endif // wxUSE_XPM_IN_MSW

  return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// Name:        msw/icon.cpp
// Purpose:     wxIcon class
// Author:      Julian Smart
// Modified by: 20.11.99 (VZ): don't derive from wxBitmap any more
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "icon.h"
#endif

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
    #include "wx/bitmap.h"
#endif

#include "wx/msw/private.h"

#if wxUSE_RESOURCE_LOADING_IN_MSW
    #include "wx/msw/curico.h"
    #include "wx/msw/curicop.h"
#endif

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxIcon, wxIconBase)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxIconRefData
// ----------------------------------------------------------------------------

void wxIconRefData::Free()
{
    if ( m_hIcon )
    {
        ::DestroyIcon((HICON) m_hIcon);

        m_hIcon = 0;
    }
}

// ----------------------------------------------------------------------------
// wxIcon
// ----------------------------------------------------------------------------

wxIcon::wxIcon(const char bits[], int width, int height)
{
    wxBitmap bmp(bits, width, height);
    CopyFromBitmap(bmp);
}

wxIcon::wxIcon(const wxString& iconfile,
               long flags,
               int desiredWidth,
               int desiredHeight)

{
    LoadFile(iconfile, flags, desiredWidth, desiredHeight);
}

wxIcon::~wxIcon()
{
}

void wxIcon::CopyFromBitmap(const wxBitmap& bmp)
{
#ifdef __WIN32__
    wxMask *mask = bmp.GetMask();
    if ( !mask )
    {
        // we must have a mask for an icon, so even if it's probably incorrect,
        // do create it (grey is the "standard" transparent colour)
        mask = new wxMask(bmp, *wxLIGHT_GREY);
    }

    ICONINFO iconInfo;
    iconInfo.fIcon = TRUE;  // we want an icon, not a cursor
    iconInfo.hbmMask = wxInvertMask((HBITMAP)mask->GetMaskBitmap());
    iconInfo.hbmColor = GetHbitmapOf(bmp);

    /* GRG: black out the transparent area to preserve background
     * colour, because Windows blits the original bitmap using
     * SRCINVERT (XOR) after applying the mask to the dest rect.
     */
    HDC dcSrc = ::CreateCompatibleDC(NULL);
    HDC dcDst = ::CreateCompatibleDC(NULL);
    HGDIOBJ hSrcOld = SelectObject(dcSrc, (HBITMAP)mask->GetMaskBitmap()),
            hDstOld = SelectObject(dcDst, iconInfo.hbmColor);

    BitBlt(dcDst, 0, 0, bmp.GetWidth(), bmp.GetHeight(), dcSrc, 0, 0, SRCAND);

    SelectObject(dcDst, hSrcOld);
    SelectObject(dcSrc, hDstOld);
    DeleteDC(dcDst);
    DeleteDC(dcSrc);

    HICON hicon = ::CreateIconIndirect(&iconInfo);

    if ( !::DeleteObject(iconInfo.hbmMask) )
    {
        wxLogLastError(_T("DeleteObject"));
    }

    if ( !hicon )
    {
        wxLogLastError(wxT("CreateIconIndirect"));
    }
    else
    {
        SetHICON((WXHICON)hicon);
        SetSize(bmp.GetWidth(), bmp.GetHeight());
    }

    if ( !bmp.GetMask() )
    {
        // we created the mask, now delete it
        delete mask;
    }
#else // Win16
    // there are some functions in curico.cpp which probably could be used
    // here...
    // This probably doesn't work.
    HBITMAP hBitmap = (HBITMAP) bmp.GetHBITMAP();
    HICON hIcon = MakeIconFromBitmap((HINSTANCE) wxGetInstance(), hBitmap);
    if (hIcon)
    {
        SetHICON((WXHICON)hIcon);
        SetSize(bmp.GetWidth(), bmp.GetHeight());
    }

//    wxFAIL_MSG("Bitmap to icon conversion (including use of XPMs for icons) not implemented");
#endif // Win32/16
}

void wxIcon::CreateIconFromXpm(const char **data)
{
    wxBitmap bmp(data);
    CopyFromBitmap(bmp);
}

bool wxIcon::LoadFile(const wxString& filename,
                      long type,
                      int desiredWidth, int desiredHeight)
{
    UnRef();

    wxGDIImageHandler *handler = FindHandler(type);

    if ( !handler )
    {
        // say something?
        return FALSE;
    }

    return handler->Load(this, filename, type, desiredWidth, desiredHeight);
}


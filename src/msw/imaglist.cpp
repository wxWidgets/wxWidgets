/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/imaglist.cpp
// Purpose:     wxImageList implementation for Win32
// Author:      Julian Smart
// Modified by:
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
    #pragma implementation "imaglist.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if defined(__WIN95__)

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/icon.h"
    #include "wx/dc.h"
    #include "wx/string.h"
    #include "wx/dcmemory.h"

    #include <stdio.h>
#endif

#include "wx/log.h"
#include "wx/intl.h"

#include "wx/msw/imaglist.h"
#include "wx/msw/private.h"

#if !defined(__GNUWIN32_OLD__) && !defined(__TWIN32__)
    #include <commctrl.h>
#endif

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxImageList, wxObject)

#define GetHImageList()     ((HIMAGELIST)m_hImageList)

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// returns the mask if it's valid, otherwise the bitmap mask and, if it's not
// valid neither, a "solid" mask (no transparent zones at all)
static wxBitmap GetMaskForImage(const wxBitmap& bitmap, const wxBitmap& mask);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxImageList creation/destruction
// ----------------------------------------------------------------------------

wxImageList::wxImageList()
{
    m_hImageList = 0;
}

// Creates an image list
bool wxImageList::Create(int width, int height, bool mask, int initial)
{
    UINT flags = 0;

    // set appropriate color depth
    int dd = wxDisplayDepth();
    if (dd <= 4)       flags |= ILC_COLOR;	// 16 color
    else if (dd <= 8)  flags |= ILC_COLOR8;	// 256 color
    else if (dd <= 16) flags |= ILC_COLOR16;	// 64k hi-color
    else if (dd <= 24) flags |= ILC_COLOR24;	// 16m truecolor
    else if (dd <= 32) flags |= ILC_COLOR32;	// 16m truecolor

    if ( mask )
        flags |= ILC_MASK;

    // Grow by 1, I guess this is reasonable behaviour most of the time
    m_hImageList = (WXHIMAGELIST) ImageList_Create(width, height, flags,
                                                   initial, 1);
    if ( !m_hImageList )
    {
        wxLogLastError("ImageList_Create()");
    }

    return m_hImageList != 0;
}

wxImageList::~wxImageList()
{
    if ( m_hImageList )
    {
        ImageList_Destroy(GetHImageList());
        m_hImageList = 0;
    }
}

// ----------------------------------------------------------------------------
// wxImageList attributes
// ----------------------------------------------------------------------------

// Returns the number of images in the image list.
int wxImageList::GetImageCount() const
{
    wxASSERT_MSG( m_hImageList, _T("invalid image list") );

    return ImageList_GetImageCount(GetHImageList());
}

// ----------------------------------------------------------------------------
// wxImageList operations
// ----------------------------------------------------------------------------

// Adds a bitmap, and optionally a mask bitmap.
// Note that wxImageList creates new bitmaps, so you may delete
// 'bitmap' and 'mask'.
int wxImageList::Add(const wxBitmap& bitmap, const wxBitmap& mask)
{
    wxBitmap bmpMask = GetMaskForImage(bitmap, mask);
    HBITMAP hbmpMask = wxInvertMask(GetHbitmapOf(bmpMask));

    int index = ImageList_Add(GetHImageList(), GetHbitmapOf(bitmap), hbmpMask);
    if ( index == -1 )
    {
        wxLogError(_("Couldn't add an image to the image list."));
    }

    ::DeleteObject(hbmpMask);

    return index;
}

// Adds a bitmap, using the specified colour to create the mask bitmap
// Note that wxImageList creates new bitmaps, so you may delete
// 'bitmap'.
int wxImageList::Add(const wxBitmap& bitmap, const wxColour& maskColour)
{
#ifdef __TWIN32__
    wxFAIL_MSG(_T("ImageList_AddMasked not implemented in TWIN32"));
    return -1;
#else
    int index = ImageList_AddMasked(GetHImageList(),
                                    GetHbitmapOf(bitmap),
                                    wxColourToRGB(maskColour));
    if ( index == -1 )
    {
        wxLogError(_("Couldn't add an image to the image list."));
    }

    return index;
#endif
}

// Adds a bitmap and mask from an icon.
int wxImageList::Add(const wxIcon& icon)
{
    int index = ImageList_AddIcon(GetHImageList(), GetHiconOf(icon));
    if ( index == -1 )
    {
        wxLogError(_("Couldn't add an image to the image list."));
    }

    return index;
}

// Replaces a bitmap, optionally passing a mask bitmap.
// Note that wxImageList creates new bitmaps, so you may delete
// 'bitmap' and 'mask'.
bool wxImageList::Replace(int index,
                          const wxBitmap& bitmap, const wxBitmap& mask)
{
#ifdef __TWIN32__
    wxFAIL_MSG(_T("ImageList_Replace not implemented in TWIN32"));
    return FALSE;
#else
    wxBitmap bmpMask = GetMaskForImage(bitmap, mask);
    HBITMAP hbmpMask = wxInvertMask(GetHbitmapOf(bmpMask));

    bool ok = ImageList_Replace(GetHImageList(), index,
                                GetHbitmapOf(bitmap), hbmpMask) != 0;
    if ( !ok )
    {
        wxLogLastError("ImageList_Add()");
    }

    ::DeleteObject(hbmpMask);

    return ok;
#endif
}

// Replaces a bitmap and mask from an icon.
bool wxImageList::Replace(int i, const wxIcon& icon)
{
    bool ok = ImageList_ReplaceIcon(GetHImageList(), i, GetHiconOf(icon)) != 0;
    if ( !ok )
    {
        wxLogLastError("ImageList_ReplaceIcon()");
    }

    return ok;
}

// Removes the image at the given index.
bool wxImageList::Remove(int index)
{
#ifdef __TWIN32__
    wxFAIL_MSG(_T("ImageList_Replace not implemented in TWIN32"));
    return FALSE;
#else
    bool ok = ImageList_Remove(GetHImageList(), index) != 0;
    if ( !ok )
    {
        wxLogLastError("ImageList_Remove()");
    }

    return ok;
#endif
}

// Remove all images
bool wxImageList::RemoveAll()
{
    // don't use ImageList_RemoveAll() because mingw32 headers don't have it
    int count = ImageList_GetImageCount(GetHImageList());
    for ( int i = 0; i < count; i++ )
    {
        // the image indexes are shifted, so we should always remove the first
        // one
        (void)Remove(0);
    }

    return TRUE;
}

// Draws the given image on a dc at the specified position.
// If 'solidBackground' is TRUE, Draw sets the image list background
// colour to the background colour of the wxDC, to speed up
// drawing by eliminating masked drawing where possible.
bool wxImageList::Draw(int index,
                       wxDC& dc,
                       int x, int y,
                       int flags,
                       bool solidBackground)
{
#ifdef __TWIN32__
    wxFAIL_MSG(_T("ImageList_Replace not implemented in TWIN32"));
    return FALSE;
#else
    HDC hDC = GetHdcOf(dc);
    wxCHECK_MSG( hDC, FALSE, _T("invalid wxDC in wxImageList::Draw") );

    COLORREF clr = CLR_NONE;    // transparent by default
    if ( solidBackground )
    {
        wxBrush *brush = & dc.GetBackground();
        if ( brush && brush->Ok() )
        {
            clr = wxColourToRGB(brush->GetColour());
        }
    }

    ImageList_SetBkColor(GetHImageList(), clr);

    UINT style = 0;
    if ( flags & wxIMAGELIST_DRAW_NORMAL )
        style |= ILD_NORMAL;
    if ( flags & wxIMAGELIST_DRAW_TRANSPARENT )
        style |= ILD_TRANSPARENT;
    if ( flags & wxIMAGELIST_DRAW_SELECTED )
        style |= ILD_SELECTED;
    if ( flags & wxIMAGELIST_DRAW_FOCUSED )
        style |= ILD_FOCUS;

    bool ok = ImageList_Draw(GetHImageList(), index, hDC, x, y, style) != 0;
    if ( !ok )
    {
        wxLogLastError("ImageList_Draw()");
    }

    return ok;
#endif
}

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

static wxBitmap GetMaskForImage(const wxBitmap& bitmap, const wxBitmap& mask)
{
    wxBitmap bmpMask;

    if ( mask.Ok() )
    {
        bmpMask = mask;
    }
    else
    {
        wxMask *pMask = bitmap.GetMask();
        if ( pMask )
        {
            bmpMask.SetHBITMAP(pMask->GetMaskBitmap());
        }
    }

    if ( !bmpMask.Ok() )
    {
        // create a non transparent mask - apparently, this is needed under
        // Win9x (it doesn't behave correctly if it's passed 0 mask)
        bmpMask.Create(bitmap.GetWidth(), bitmap.GetHeight(), 1);

        wxMemoryDC dcMem;
        dcMem.SelectObject(bmpMask);
        dcMem.Clear();
        dcMem.SelectObject(wxNullBitmap);
    }

    return bmpMask;
}

#endif // Win95


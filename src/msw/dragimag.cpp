/////////////////////////////////////////////////////////////////////////////
// Name:        dragimag.cpp
// Purpose:     wxDragImage
// Author:      Julian Smart
// Modified by:
// Created:     08/04/99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation "dragimag.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_DRAGIMAG

#if defined(__WIN95__)

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/setup.h"
#include "wx/window.h"
#include "wx/dcclient.h"
#include "wx/dcscreen.h"
#include "wx/dcmemory.h"
#include "wx/settings.h"
#endif

#include "wx/msw/private.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/frame.h"
#include "wx/image.h"

#include "wx/msw/dragimag.h"
#include "wx/msw/private.h"

#if defined(__WIN95__) && !(defined(__GNUWIN32_OLD__) || defined(__TWIN32__))
#include <commctrl.h>
#endif

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxDragImage, wxObject)

#define GetHimageList() ((HIMAGELIST) m_hImageList)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDragImage ctors/dtor
// ----------------------------------------------------------------------------

wxDragImage::wxDragImage()
{
    Init();
}

wxDragImage::~wxDragImage()
{
    if ( m_hImageList )
        ImageList_Destroy(GetHimageList());
    if ( m_hCursorImageList )
        ImageList_Destroy((HIMAGELIST) m_hCursorImageList);
}

void wxDragImage::Init()
{
    m_hImageList = 0;
    m_hCursorImageList = 0;
    m_window = (wxWindow*) NULL;
    m_fullScreen = FALSE;
}

// Attributes
////////////////////////////////////////////////////////////////////////////


// Operations
////////////////////////////////////////////////////////////////////////////

// Create a drag image from a bitmap and optional cursor
bool wxDragImage::Create(const wxBitmap& image, const wxCursor& cursor, const wxPoint& hotspot)
{
    if ( m_hImageList )
        ImageList_Destroy(GetHimageList());
    m_hImageList = 0;

    UINT flags = 0 ;
    if (image.GetDepth() <= 4)
        flags = ILC_COLOR4;
    else if (image.GetDepth() <= 8)
        flags = ILC_COLOR8;
    else if (image.GetDepth() <= 16)
        flags = ILC_COLOR16;
    else if (image.GetDepth() <= 24)
        flags = ILC_COLOR24;
    else
        flags = ILC_COLOR32;

    bool mask = (image.GetMask() != 0);
    if ( mask )
        flags |= ILC_MASK;

    m_hImageList = (WXHIMAGELIST) ImageList_Create(image.GetWidth(), image.GetHeight(), flags, 1, 1);

    int index;
    if (!mask)
    {
        HBITMAP hBitmap1 = (HBITMAP) image.GetHBITMAP();
        index = ImageList_Add(GetHimageList(), hBitmap1, 0);
    }
    else
    {
        HBITMAP hBitmap1 = (HBITMAP) image.GetHBITMAP();
        HBITMAP hBitmap2 = (HBITMAP) image.GetMask()->GetMaskBitmap();
        HBITMAP hbmpMask = wxInvertMask(hBitmap2);

        index = ImageList_Add(GetHimageList(), hBitmap1, hbmpMask);
        ::DeleteObject(hbmpMask);
    }
    if ( index == -1 )
    {
        wxLogError(_("Couldn't add an image to the image list."));
    }
    m_cursor = cursor; // Can only combine with drag image after calling BeginDrag.
    m_hotspot = hotspot;

    return (index != -1) ;
}

// Create a drag image from an icon and optional cursor
bool wxDragImage::Create(const wxIcon& image, const wxCursor& cursor, const wxPoint& hotspot)
{
    if ( m_hImageList )
        ImageList_Destroy(GetHimageList());
    m_hImageList = 0;

    UINT flags = 0 ;
    if (image.GetDepth() <= 4)
        flags = ILC_COLOR4;
    else if (image.GetDepth() <= 8)
        flags = ILC_COLOR8;
    else if (image.GetDepth() <= 16)
        flags = ILC_COLOR16;
    else if (image.GetDepth() <= 24)
        flags = ILC_COLOR24;
    else
        flags = ILC_COLOR32;
    bool mask = TRUE;
    if ( mask )
        flags |= ILC_MASK;

    m_hImageList = (WXHIMAGELIST) ImageList_Create(image.GetWidth(), image.GetHeight(), flags, 1, 1);

    HICON hIcon = (HICON) image.GetHICON();

    int index = ImageList_AddIcon(GetHimageList(), hIcon);
    if ( index == -1 )
    {
        wxLogError(_("Couldn't add an image to the image list."));
    }

    m_cursor = cursor; // Can only combine with drag image after calling BeginDrag.
    m_hotspot = hotspot;

    return (index != -1) ;
}

// Create a drag image from a string and optional cursor
bool wxDragImage::Create(const wxString& str, const wxCursor& cursor, const wxPoint& hotspot)
{
    wxFont font(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));

    long w, h;
    wxScreenDC dc;
    dc.SetFont(font);
    dc.GetTextExtent(str, & w, & h);
    dc.SetFont(wxNullFont);

    wxMemoryDC dc2;
    dc2.SetFont(font);
    wxBitmap bitmap((int) w+2, (int) h+2);
    dc2.SelectObject(bitmap);

    dc2.SetBackground(* wxWHITE_BRUSH);
    dc2.Clear();
    dc2.SetBackgroundMode(wxTRANSPARENT);
    dc2.SetTextForeground(* wxLIGHT_GREY);
    dc2.DrawText(str, 0, 0);
    dc2.DrawText(str, 1, 0);
    dc2.DrawText(str, 2, 0);
    dc2.DrawText(str, 1, 1);
    dc2.DrawText(str, 2, 1);
    dc2.DrawText(str, 1, 2);
    dc2.DrawText(str, 2, 2);
    dc2.SetTextForeground(* wxBLACK);
    dc2.DrawText(str, 1, 1);

    dc2.SelectObject(wxNullBitmap);

    // Make the bitmap masked
    wxImage image(bitmap);
    image.SetMaskColour(255, 255, 255);
    bitmap = image.ConvertToBitmap();

    return Create(bitmap, cursor, hotspot);
}

// Create a drag image for the given tree control item
bool wxDragImage::Create(const wxTreeCtrl& treeCtrl, wxTreeItemId& id)
{
    if ( m_hImageList )
        ImageList_Destroy(GetHimageList());
    m_hImageList = (WXHIMAGELIST) TreeView_CreateDragImage((HWND) treeCtrl.GetHWND(), (HTREEITEM) (WXHTREEITEM) id);
    return TRUE;
}

// Create a drag image for the given list control item
bool wxDragImage::Create(const wxListCtrl& listCtrl, long id)
{
    if ( m_hImageList )
        ImageList_Destroy(GetHimageList());
    POINT pt;
    pt.x = 0; pt.y = 0;
    m_hImageList = (WXHIMAGELIST) ListView_CreateDragImage((HWND) listCtrl.GetHWND(), id, & pt);
    return TRUE;
}

// Begin drag
bool wxDragImage::BeginDrag(const wxPoint& hotspot, wxWindow* window, bool fullScreen, wxRect* rect)
{
    wxASSERT_MSG( (m_hImageList != 0), wxT("Image list must not be null in BeginDrag."));
    wxASSERT_MSG( (window != 0), wxT("Window must not be null in BeginDrag."));

    m_fullScreen = fullScreen;
    if (rect)
        m_boundingRect = * rect;

    bool ret = (ImageList_BeginDrag(GetHimageList(), 0, hotspot.x, hotspot.y) != 0);

    if (!ret)
    {
        wxFAIL_MSG( _T("BeginDrag failed.") );

        return FALSE;
    }

    if (m_cursor.Ok())
    {
        if (!m_hCursorImageList)
        {           
            int cxCursor = GetSystemMetrics(SM_CXCURSOR); 
            int cyCursor = GetSystemMetrics(SM_CYCURSOR); 
 
            m_hCursorImageList = (WXHIMAGELIST) ImageList_Create(cxCursor, cyCursor, ILC_MASK, 1, 1);
        }

        // First add the cursor to the image list
        HCURSOR hCursor = (HCURSOR) m_cursor.GetHCURSOR();
        int cursorIndex = ImageList_AddIcon((HIMAGELIST) m_hCursorImageList, (HICON) hCursor);

        wxASSERT_MSG( (cursorIndex != -1), wxT("ImageList_AddIcon failed in BeginDrag."));

        if (cursorIndex != -1)
        {
            ImageList_SetDragCursorImage((HIMAGELIST) m_hCursorImageList, cursorIndex, m_hotspot.x, m_hotspot.y);
        }
    }

    m_window = window;
    ::ShowCursor(FALSE);

    ::SetCapture(GetHwndOf(window));

    return TRUE;
}

// Begin drag. hotspot is the location of the drag position relative to the upper-left
// corner of the image. This is full screen only. fullScreenRect gives the
// position of the window on the screen, to restrict the drag to.
bool wxDragImage::BeginDrag(const wxPoint& hotspot, wxWindow* window, wxWindow* fullScreenRect)
{
    wxRect rect;

    int x = fullScreenRect->GetPosition().x;
    int y = fullScreenRect->GetPosition().y;
    
    wxSize sz = fullScreenRect->GetSize();

    if (fullScreenRect->GetParent() && !fullScreenRect->IsKindOf(CLASSINFO(wxFrame)))
        fullScreenRect->GetParent()->ClientToScreen(& x, & y);

    rect.x = x; rect.y = y;
    rect.width = sz.x; rect.height = sz.y;

    return BeginDrag(hotspot, window, TRUE, & rect);
}

// End drag
bool wxDragImage::EndDrag()
{
    wxASSERT_MSG( (m_hImageList != 0), wxT("Image list must not be null in EndDrag."));

    ImageList_EndDrag();

    if ( !::ReleaseCapture() )
    {
        wxLogLastError(wxT("ReleaseCapture"));
    }

    ::ShowCursor(TRUE);
    m_window = (wxWindow*) NULL;

    return TRUE;
}

// Move the image: call from OnMouseMove. Pt is in window client coordinates if window
// is non-NULL, or in screen coordinates if NULL.
bool wxDragImage::Move(const wxPoint& pt)
{
    wxASSERT_MSG( (m_hImageList != 0), wxT("Image list must not be null in Move."));

    // TODO: what coordinates are these in: window, client, or screen?
    bool ret = (ImageList_DragMove( pt.x, pt.y ) != 0);

    m_position = pt;

    return ret;
}

bool wxDragImage::Show()
{
    wxASSERT_MSG( (m_hImageList != 0), wxT("Image list must not be null in Show."));

    HWND hWnd = 0;
    if (m_window && !m_fullScreen)
        hWnd = (HWND) m_window->GetHWND();

    bool ret = (ImageList_DragEnter( hWnd, m_position.x, m_position.y ) != 0);

    return ret;
}

bool wxDragImage::Hide()
{
    wxASSERT_MSG( (m_hImageList != 0), wxT("Image list must not be null in Hide."));

    HWND hWnd = 0;
    if (m_window && !m_fullScreen)
        hWnd = (HWND) m_window->GetHWND();

    bool ret = (ImageList_DragLeave( hWnd ) != 0);

    return ret;
}

#endif
    // __WIN95__

#endif // wxUSE_DRAGIMAG

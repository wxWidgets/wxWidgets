/////////////////////////////////////////////////////////////////////////////
// Name:        dragimag.cpp
// Purpose:     wxDragImage
// Author:      Julian Smart
// Modified by:
// Created:     08/04/99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dragimag.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#include "wx/msw/private.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

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

#include "wx/log.h"
#include "wx/intl.h"

#include "wx/msw/dragimag.h"
#include "wx/msw/private.h"

#if (defined(__WIN95__) && !defined(__GNUWIN32__)) || defined(__TWIN32__) || defined(wxUSE_NORLANDER_HEADERS)
#include <commctrl.h>
#endif

IMPLEMENT_DYNAMIC_CLASS(wxDragImage, wxObject)

wxDragImage::wxDragImage()
{
    m_hImageList = 0;
}

wxDragImage::~wxDragImage()
{
	if ( m_hImageList )
		ImageList_Destroy((HIMAGELIST) m_hImageList);
	m_hImageList = 0;
}


// Attributes
////////////////////////////////////////////////////////////////////////////


// Operations
////////////////////////////////////////////////////////////////////////////

// Create a drag image from a bitmap and optional cursor
bool wxDragImage::Create(const wxBitmap& image, const wxCursor& cursor, const wxPoint& hotspot)
{
	if ( m_hImageList )
		ImageList_Destroy((HIMAGELIST) m_hImageList);
	m_hImageList = 0;

	UINT flags = 0;
    bool mask = TRUE; // ?
	if ( mask )
		flags |= ILC_MASK;

	m_hImageList = (WXHIMAGELIST) ImageList_Create(image.GetWidth(), image.GetHeight(), flags, 1, 1);

	HBITMAP hBitmap1 = (HBITMAP) image.GetHBITMAP();
	HBITMAP hBitmap2 = 0;
	if ( image.GetMask() )
	    hBitmap2 = (HBITMAP) image.GetMask()->GetMaskBitmap();

    int index = ImageList_Add((HIMAGELIST) m_hImageList, hBitmap1, hBitmap2);
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
		ImageList_Destroy((HIMAGELIST) m_hImageList);
	m_hImageList = 0;

	UINT flags = 0;
    bool mask = TRUE; // ?
	if ( mask )
		flags |= ILC_MASK;

	m_hImageList = (WXHIMAGELIST) ImageList_Create(image.GetWidth(), image.GetHeight(), flags, 1, 1);

	HICON hIcon = (HICON) image.GetHICON();

    int index = ImageList_AddIcon((HIMAGELIST) m_hImageList, hIcon);
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

    wxMemoryDC dc2;
    dc2.SetFont(font);
    wxBitmap bitmap((int) w, (int) h);
    dc2.SelectObject(bitmap);

    dc2.SetBackground(* wxWHITE_BRUSH);
    dc2.Clear();
    dc2.DrawText(str, 0, 0);

    dc2.SelectObject(wxNullBitmap);

    return Create(bitmap, cursor, hotspot);
}

// Create a drag image for the given tree control item
bool wxDragImage::Create(const wxTreeCtrl& treeCtrl, wxTreeItemId& id)
{
	if ( m_hImageList )
		ImageList_Destroy((HIMAGELIST) m_hImageList);
    m_hImageList = (WXHIMAGELIST) TreeView_CreateDragImage((HWND) treeCtrl.GetHWND(), (HTREEITEM) (WXHTREEITEM) id);
    return TRUE;
}

// Create a drag image for the given list control item
bool wxDragImage::Create(const wxListCtrl& listCtrl, long id)
{
	if ( m_hImageList )
		ImageList_Destroy((HIMAGELIST) m_hImageList);
    POINT pt;
    pt.x = 0; pt.y = 0;
    m_hImageList = (WXHIMAGELIST) ListView_CreateDragImage((HWND) listCtrl.GetHWND(), id, & pt);
    return TRUE;
}

// Begin drag
bool wxDragImage::BeginDrag(const wxPoint& hotspot, wxWindow* WXUNUSED(window))
{
    wxASSERT_MSG( (m_hImageList != 0), wxT("Image list must not be null in BeginDrag."));

    bool ret = (ImageList_BeginDrag((HIMAGELIST) m_hImageList, 0, hotspot.x, hotspot.y) != 0);

    wxASSERT_MSG( (ret), wxT("BeginDrag failed."));

    if (!ret)
        return FALSE;

    if (m_cursor.Ok())
    {
        // First add the cursor to the image list
        int cursorIndex = ImageList_AddIcon((HIMAGELIST) m_hImageList, (HICON) m_cursor.GetHCURSOR());

        wxASSERT_MSG( (cursorIndex != -1), wxT("ImageList_AddIcon failed in BeginDrag."));

        if (cursorIndex != -1)
        {
            ImageList_SetDragCursorImage((HIMAGELIST) m_hImageList, cursorIndex, m_hotspot.x, m_hotspot.y);
        }
    }

    ::ShowCursor(FALSE);

    return TRUE;
}
    
// End drag
bool wxDragImage::EndDrag(wxWindow* WXUNUSED(window))
{
    wxASSERT_MSG( (m_hImageList != 0), wxT("Image list must not be null in EndDrag."));

    ImageList_EndDrag();

    ::ShowCursor(TRUE);

    return TRUE;
}
    
// Move the image: call from OnMouseMove. Pt is in window client coordinates if window
// is non-NULL, or in screen coordinates if NULL.
bool wxDragImage::Move(const wxPoint& pt, wxWindow* window)
{
    wxASSERT_MSG( (m_hImageList != 0), wxT("Image list must not be null in Move."));

    // TODO: what coordinates are these in: window, client, or screen?
    bool ret = (ImageList_DragMove( pt.x, pt.y ) != 0);

    m_position = pt;

    return ret;
}

bool wxDragImage::Show(wxWindow* window)
{
    wxASSERT_MSG( (m_hImageList != 0), wxT("Image list must not be null in Show."));

    HWND hWnd = 0;
    if (window)
        hWnd = (HWND) window->GetHWND();

    bool ret = (ImageList_DragEnter( hWnd, m_position.x, m_position.y ) != 0);

    return ret;
}

bool wxDragImage::Hide(wxWindow* window)
{
    wxASSERT_MSG( (m_hImageList != 0), wxT("Image list must not be null in Hide."));

    HWND hWnd = 0;
    if (window)
        hWnd = (HWND) window->GetHWND();

    bool ret = (ImageList_DragLeave( hWnd ) != 0);

    return ret;
}

#endif
    // __WIN95__


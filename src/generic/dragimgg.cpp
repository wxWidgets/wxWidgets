/////////////////////////////////////////////////////////////////////////////
// Name:        dragimgg.cpp
// Purpose:     Generic wxDragImage implementation
// Author:      Julian Smart
// Modified by:
// Created:     29/2/2000
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
#pragma implementation "dragimgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/setup.h"
#include "wx/window.h"
#include "wx/frame.h"
#include "wx/dcclient.h"
#include "wx/dcscreen.h"
#include "wx/dcmemory.h"
#include "wx/settings.h"
#endif

#include "wx/log.h"
#include "wx/intl.h"

#ifdef __WIN16__
#define wxUSE_IMAGE_IN_DRAGIMAGE 0
#else
#define wxUSE_IMAGE_IN_DRAGIMAGE 1
#endif

#if wxUSE_IMAGE_IN_DRAGIMAGE
#include "wx/image.h"
#endif

#include "wx/generic/dragimgg.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGenericDragImage, wxObject)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxGenericDragImage ctors/dtor
// ----------------------------------------------------------------------------

wxGenericDragImage::~wxGenericDragImage()
{
    if (m_windowDC)
    {
        delete m_windowDC;
    }
}

void wxGenericDragImage::Init()
{
    m_isDirty = FALSE;
    m_isShown = FALSE;
    m_windowDC = (wxDC*) NULL;
    m_window = (wxWindow*) NULL;
    m_fullScreen = FALSE;
}

// Attributes
////////////////////////////////////////////////////////////////////////////


// Operations
////////////////////////////////////////////////////////////////////////////

// Create a drag image with a virtual image (need to override DoDrawImage, GetImageRect)
bool wxGenericDragImage::Create(const wxCursor& cursor, const wxPoint& hotspot)
{
    m_cursor = cursor;
    m_hotspot = hotspot;

    return TRUE;
}

// Create a drag image from a bitmap and optional cursor
bool wxGenericDragImage::Create(const wxBitmap& image, const wxCursor& cursor, const wxPoint& hotspot)
{
    // We don't have to combine the cursor explicitly since we simply show the cursor
    // as we drag. This currently will only work within one window.

    m_cursor = cursor;
    m_hotspot = hotspot;
    m_bitmap = image;

    return TRUE ;
}

// Create a drag image from an icon and optional cursor
bool wxGenericDragImage::Create(const wxIcon& image, const wxCursor& cursor, const wxPoint& hotspot)
{
    // We don't have to combine the cursor explicitly since we simply show the cursor
    // as we drag. This currently will only work within one window.

    m_cursor = cursor;
    m_hotspot = hotspot;
    m_icon = image;

    return TRUE ;
}

// Create a drag image from a string and optional cursor
bool wxGenericDragImage::Create(const wxString& str, const wxCursor& cursor, const wxPoint& hotspot)
{
    wxFont font(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));

    long w, h;
    wxScreenDC dc;
    dc.SetFont(font);
    dc.GetTextExtent(str, & w, & h);
    dc.SetFont(wxNullFont);

    wxMemoryDC dc2;
    dc2.SetFont(font);

    // Sometimes GetTextExtent isn't accurate enough, so make it longer
    wxBitmap bitmap((int) ((w+2) * 1.5), (int) h+2);
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

#if wxUSE_IMAGE_IN_DRAGIMAGE
    // Make the bitmap masked
    wxImage image(bitmap);
    image.SetMaskColour(255, 255, 255);
    bitmap = image.ConvertToBitmap();
#endif

    return Create(bitmap, cursor, hotspot);
}

// Create a drag image for the given tree control item
bool wxGenericDragImage::Create(const wxTreeCtrl& treeCtrl, wxTreeItemId& id)
{
    wxString str = treeCtrl.GetItemText(id);
    return Create(str);
}

// Create a drag image for the given list control item
bool wxGenericDragImage::Create(const wxListCtrl& listCtrl, long id)
{
    wxString str = listCtrl.GetItemText(id);
    return Create(str);
}

// Begin drag
bool wxGenericDragImage::BeginDrag(const wxPoint& hotspot,
                                   wxWindow* window,
                                   bool fullScreen,
                                   wxRect* rect)
{
    wxASSERT_MSG( (window != 0), wxT("Window must not be null in BeginDrag."));

    // The image should be offset by this amount
    m_offset = hotspot;
    m_window = window;
    m_fullScreen = fullScreen;

    if (rect)
        m_boundingRect = * rect;

    m_isDirty = FALSE;
    m_isDirty = FALSE;

    if (window)
    {
        window->CaptureMouse();

        if (m_cursor.Ok())
        {
            m_oldCursor = window->GetCursor();
            window->SetCursor(m_cursor);
        }
    }

    // Make a copy of the window so we can repair damage done as the image is
    // dragged.

    wxSize clientSize;
    wxPoint pt(0, 0);
    if (!m_fullScreen)
    {
        clientSize = window->GetClientSize();
        m_boundingRect.x = 0; m_boundingRect.y = 0;
        m_boundingRect.width = clientSize.x; m_boundingRect.height = clientSize.y;
    }
    else
    {
        int w, h;
        wxDisplaySize(& w, & h);
        clientSize.x = w; clientSize.y = h;
        if (rect)
        {
            pt.x = m_boundingRect.x; pt.y = m_boundingRect.y;
            clientSize.x = m_boundingRect.width; clientSize.y = m_boundingRect.height;
        }
        else
        {
            m_boundingRect.x = 0; m_boundingRect.y = 0;
            m_boundingRect.width = w; m_boundingRect.height = h;
        }
    }

    if (!m_backingBitmap.Ok() || (m_backingBitmap.GetWidth() < clientSize.x || m_backingBitmap.GetHeight() < clientSize.y))
        m_backingBitmap = wxBitmap(clientSize.x, clientSize.y);

    if (!m_fullScreen)
        m_windowDC = new wxClientDC(window);
    else
    {
        m_windowDC = new wxScreenDC;

#if 0
        // Use m_boundingRect to limit the area considered.
        ((wxScreenDC*) m_windowDC)->StartDrawingOnTop(rect);
#endif

        m_windowDC->SetClippingRegion(m_boundingRect.x, m_boundingRect.y,
            m_boundingRect.width, m_boundingRect.height);
    }

    return TRUE;
}

// Begin drag. hotspot is the location of the drag position relative to the upper-left
// corner of the image. This is full screen only. fullScreenRect gives the
// position of the window on the screen, to restrict the drag to.
bool wxGenericDragImage::BeginDrag(const wxPoint& hotspot, wxWindow* window, wxWindow* fullScreenRect)
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
bool wxGenericDragImage::EndDrag()
{
    if (m_window)
    {
        m_window->ReleaseMouse();
        if (m_cursor.Ok() && m_oldCursor.Ok())
        {
            m_window->SetCursor(m_oldCursor);
        }
    }

    if (m_windowDC)
    {
        m_windowDC->DestroyClippingRegion();
        delete m_windowDC;
        m_windowDC = (wxDC*) NULL;
    }

    m_repairBitmap = wxNullBitmap;

    return TRUE;
}

// Move the image: call from OnMouseMove. Pt is in window client coordinates if window
// is non-NULL, or in screen coordinates if NULL.
bool wxGenericDragImage::Move(const wxPoint& pt)
{
    wxASSERT_MSG( (m_windowDC != (wxDC*) NULL), wxT("No window DC in wxGenericDragImage::Move()") );

    // Erase at old position, then show at the current position
    wxPoint oldPos = m_position;

    bool eraseOldImage = (m_isDirty && m_isShown);
    
    if (m_isShown)
        RedrawImage(oldPos - m_offset, pt - m_offset, eraseOldImage, TRUE);

    m_position = pt;

    if (m_isShown)
        m_isDirty = TRUE;

    return TRUE;
}

bool wxGenericDragImage::Show()
{
    wxASSERT_MSG( (m_windowDC != (wxDC*) NULL), wxT("No window DC in wxGenericDragImage::Show()") );
    
    // Show at the current position

    if (!m_isShown)
    {
        // This is where we restore the backing bitmap, in case
        // something has changed on the window.

        wxMemoryDC memDC;
        memDC.SelectObject(m_backingBitmap);
        memDC.Blit(0, 0, m_boundingRect.width, m_boundingRect.height, m_windowDC, m_boundingRect.x, m_boundingRect.y);
        memDC.SelectObject(wxNullBitmap);

        RedrawImage(m_position - m_offset, m_position - m_offset, FALSE, TRUE);
    }

    m_isShown = TRUE;
    m_isDirty = TRUE;

    return TRUE;
}

bool wxGenericDragImage::Hide()
{
    wxASSERT_MSG( (m_windowDC != (wxDC*) NULL), wxT("No window DC in wxGenericDragImage::Hide()") );

    // Repair the old position

    if (m_isShown && m_isDirty)
    {
        RedrawImage(m_position - m_offset, m_position - m_offset, TRUE, FALSE);
    }

    m_isShown = FALSE;
    m_isDirty = FALSE;

    return TRUE;
}

// More efficient: erase and redraw simultaneously if possible
bool wxGenericDragImage::RedrawImage(const wxPoint& oldPos, const wxPoint& newPos,
                                     bool eraseOld, bool drawNew)
{
    if (!m_windowDC)
        return FALSE;

    if (!m_backingBitmap.Ok())
        return FALSE;

    wxRect oldRect(GetImageRect(oldPos));
    wxRect newRect(GetImageRect(newPos));

    wxRect fullRect;

    // Full rect: the combination of both rects
    if (eraseOld && drawNew)
    {
        int oldRight = oldRect.GetRight();
        int oldBottom = oldRect.GetBottom();
        int newRight = newRect.GetRight();
        int newBottom = newRect.GetBottom();

        wxPoint topLeft = wxPoint(wxMin(oldPos.x, newPos.x), wxMin(oldPos.y, newPos.y));
        wxPoint bottomRight = wxPoint(wxMax(oldRight, newRight), wxMax(oldBottom, newBottom));

        fullRect.x = topLeft.x; fullRect.y = topLeft.y;
        fullRect.SetRight(bottomRight.x);
        fullRect.SetBottom(bottomRight.y);
    }
    else if (eraseOld)
        fullRect = oldRect;
    else if (drawNew)
        fullRect = newRect;

    // Make the bitmap bigger than it need be, so we don't
    // keep reallocating all the time.
    int excess = 50;

    if (!m_repairBitmap.Ok() || (m_repairBitmap.GetWidth() < fullRect.GetWidth() || m_repairBitmap.GetHeight() < fullRect.GetHeight()))
    {
        m_repairBitmap = wxBitmap(fullRect.GetWidth() + excess, fullRect.GetHeight() + excess);
    }

    wxMemoryDC memDC;
    memDC.SelectObject(m_backingBitmap);

    wxMemoryDC memDCTemp;
    memDCTemp.SelectObject(m_repairBitmap);

    // Draw the backing bitmap onto the repair bitmap.
    // If full-screen, we may have specified the rect on the
    // screen that we're using for our backing bitmap.
    // So subtract this when we're blitting from the backing bitmap
    // (translate from screen to backing-bitmap coords).

    memDCTemp.Blit(0, 0, fullRect.GetWidth(), fullRect.GetHeight(), & memDC, fullRect.x - m_boundingRect.x, fullRect.y - m_boundingRect.y);

    // If drawing, draw the image onto the mem DC
    if (drawNew)
    {
        wxPoint pos(newPos.x - fullRect.x, newPos.y - fullRect.y) ;
        DoDrawImage(memDCTemp, pos);
    }

    // Now blit to the window
    // Finally, blit the temp mem DC to the window.
    m_windowDC->Blit(fullRect.x, fullRect.y, fullRect.width, fullRect.height, & memDCTemp, 0, 0);

    memDCTemp.SelectObject(wxNullBitmap);
    memDC.SelectObject(wxNullBitmap);

    return TRUE;
}

// Override this if you are using a virtual image (drawing your own image)
bool wxGenericDragImage::DoDrawImage(wxDC& dc, const wxPoint& pos) const
{
    if (m_bitmap.Ok())
    {
        dc.DrawBitmap(m_bitmap, pos.x, pos.y, (m_bitmap.GetMask() != 0));
        return TRUE;
    }
    else if (m_icon.Ok())
    {
        dc.DrawIcon(m_icon, pos.x, pos.y);
        return TRUE;
    }
    else
        return FALSE;
}

// Override this if you are using a virtual image (drawing your own image)
wxRect wxGenericDragImage::GetImageRect(const wxPoint& pos) const
{
    if (m_bitmap.Ok())
    {
        return wxRect(pos.x, pos.y, m_bitmap.GetWidth(), m_bitmap.GetHeight());
    }
    else if (m_icon.Ok())
    {
        return wxRect(pos.x, pos.y, m_icon.GetWidth(), m_icon.GetHeight());
    }
    else
    {
        return wxRect(pos.x, pos.y, 0, 0);
    }
}


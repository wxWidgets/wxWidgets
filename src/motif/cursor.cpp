/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.cpp
// Purpose:     wxCursor class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "cursor.h"
#endif

#include "wx/cursor.h"
#include "wx/gdicmn.h"
#include "wx/icon.h"
#include "wx/app.h"
#include "wx/utils.h"

#include <Xm/Xm.h>
#include <X11/cursorfont.h>

#include "wx/motif/private.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxCursor, wxBitmap)
IMPLEMENT_DYNAMIC_CLASS(wxXCursor, wxObject)
#endif

wxCursorRefData::wxCursorRefData()
{
    m_width = 32; m_height = 32;
    m_cursorId = wxCURSOR_NONE;
}

wxCursorRefData::~wxCursorRefData()
{
    wxNode* node = m_cursors.First();
    while (node)
    {
        wxXCursor* c = (wxXCursor*) node->Data();
        // TODO: how to delete cursor?
        // XDestroyCursor((Display*) c->m_display, (Cursor) c->m_cursor); // ??
        delete c;
        node = node->Next();
    }
}

wxCursor::wxCursor()
{
}

wxCursor::wxCursor(const char bits[], int width, int height,
    int hotSpotX, int hotSpotY, const char maskBits[])
{
    m_refData = new wxCursorRefData;

    Display *dpy = (Display*) wxGetDisplay();
    int screen_num =  DefaultScreen (dpy);

    Pixmap pixmap = XCreatePixmapFromBitmapData (dpy,
                                          RootWindow (dpy, DefaultScreen(dpy)),
                                          (char*) bits, width, height,
                                          1 , 0 , 1);

    Pixmap mask_pixmap = None;
    if (maskBits != NULL)
    {
        mask_pixmap = XCreatePixmapFromBitmapData (dpy,
                                          RootWindow (dpy, DefaultScreen(dpy)),
                                          (char*) maskBits, width, height,
                                          1 , 0 , 1);
    }

    XColor foreground_color;
    XColor background_color;
    foreground_color.pixel = BlackPixel(dpy, screen_num);
    background_color.pixel = WhitePixel(dpy, screen_num);
    Colormap cmap = (Colormap) wxTheApp->GetMainColormap((WXDisplay*) dpy);
    XQueryColor(dpy, cmap, &foreground_color);
    XQueryColor(dpy, cmap, &background_color);

    Cursor cursor = XCreatePixmapCursor (dpy,
                                  pixmap,
                                  mask_pixmap,
                                  &foreground_color,
                                  &background_color,
                                  hotSpotX , 
                                  hotSpotY);

    XFreePixmap( dpy, pixmap );
    if (mask_pixmap != None)
    {
        XFreePixmap( dpy, mask_pixmap );
    }

    if (cursor)
    {
        wxXCursor *c = new wxXCursor;

        c->m_cursor = (WXCursor) cursor;
        c->m_display = (WXDisplay*) dpy;
        M_CURSORDATA->m_cursors.Append(c);
        M_CURSORDATA->m_ok = TRUE;
    }
    else
    {
        M_CURSORDATA->m_ok = TRUE;
    }
}

wxCursor::wxCursor(const wxString& name, long flags, int hotSpotX, int hotSpotY)
{
    // Must be an XBM file
    if (flags != wxBITMAP_TYPE_XBM)
        return;

    m_refData = new wxCursorRefData;

    int hotX = -1, hotY = -1;
    unsigned int w, h;
    Pixmap pixmap;

    Display *dpy = (Display*) wxGetDisplay();
    int screen_num =  DefaultScreen (dpy);

    int value = XReadBitmapFile (dpy, RootWindow (dpy, DefaultScreen (dpy)),
                        (char*) (const char*) name, &w, &h, &pixmap, &hotX, &hotY);

    M_BITMAPDATA->m_width = w;
    M_BITMAPDATA->m_height = h;
    M_BITMAPDATA->m_depth = 1;

    if ((value == BitmapFileInvalid) ||
        (value == BitmapOpenFailed) ||
        (value == BitmapNoMemory))
    {
    }
    else
    {
        XColor foreground_color;
        XColor background_color;
        foreground_color.pixel = BlackPixel(dpy, screen_num);
        background_color.pixel = WhitePixel(dpy, screen_num);
        Colormap cmap = (Colormap) wxTheApp->GetMainColormap((WXDisplay*) dpy);
        XQueryColor(dpy, cmap, &foreground_color);
        XQueryColor(dpy, cmap, &background_color);

        // TODO: how do we determine whether hotX, hotY were read correctly?
        if (hotX < 0 || hotY < 0)
        {
            hotX = hotSpotX;
            hotY = hotSpotY;
        }
        if (hotX < 0 || hotY < 0)
        {
            hotX = 0;
            hotY = 0;
        }

        Pixmap mask_pixmap = None;
        Cursor cursor = XCreatePixmapCursor (dpy,
                                      pixmap,
                                      mask_pixmap,
                                      &foreground_color,
                                      &background_color,
                                      hotX,
                                      hotY);

        XFreePixmap( dpy, pixmap );
        if (cursor)
        {
            wxXCursor *c = new wxXCursor;

            c->m_cursor = (WXCursor) cursor;
            c->m_display = (WXDisplay*) dpy;
            M_CURSORDATA->m_cursors.Append(c);
            M_CURSORDATA->m_ok = TRUE;
        }
    }

}

// Cursors by stock number
wxCursor::wxCursor(wxStockCursor id)
{
    m_refData = new wxCursorRefData;
    M_CURSORDATA->m_cursorId = id;
    M_CURSORDATA->m_ok = TRUE;

    WXDisplay* display = wxGetDisplay();
    if (!display)
      return;

    WXCursor cursor = GetXCursor(display);
    if (cursor)
    {
        wxXCursor* c = new wxXCursor;
        c->m_cursor = cursor;
        c->m_display = wxGetDisplay();
        M_CURSORDATA->m_cursors.Append(c);
        M_CURSORDATA->m_ok = TRUE;
    }
}

wxCursor::~wxCursor()
{
}

// Motif-specific: create/get a cursor for the current display
WXCursor wxCursor::GetXCursor(WXDisplay* display)
{
    if (!M_CURSORDATA)
        return (WXCursor) 0;
    wxNode* node = M_CURSORDATA->m_cursors.First();
    while (node)
    {
        wxXCursor* c = (wxXCursor*) node->Data();
        if (c->m_display == display)
            return c->m_cursor;
        node = node->Next();
    }

    // No cursor for this display, so let's see if we're an id-type cursor.

    if (M_CURSORDATA->m_cursorId != wxCURSOR_NONE)
    {
        WXCursor cursor = MakeCursor(display, M_CURSORDATA->m_cursorId);
        if (cursor)
        {
            wxXCursor* c = new wxXCursor;
            c->m_cursor = cursor;
            c->m_display = display;
            M_CURSORDATA->m_cursors.Append(c);
            return cursor;
        }
        else
            return (WXCursor) 0;
    }

    // Not an id-type cursor, so we don't know how to create it.
    return (WXCursor) 0;
}

// Make a cursor from standard id
WXCursor wxCursor::MakeCursor(WXDisplay* display, wxStockCursor id)
{
    Display* dpy = (Display*) display;
    Cursor cursor = (Cursor) 0;

    switch (id)
    {
        case wxCURSOR_WAIT:
        {
            cursor = XCreateFontCursor (dpy, XC_watch);
            break;
        }
        case wxCURSOR_CROSS:
        {
            cursor = XCreateFontCursor (dpy, XC_crosshair);
            break;
        }
        case wxCURSOR_CHAR:
        {
            // Nothing
            break;
        }
        case wxCURSOR_HAND:
        {
            cursor = XCreateFontCursor (dpy, XC_hand1);
            break;
        }
        case wxCURSOR_BULLSEYE:
        {
            cursor = XCreateFontCursor (dpy, XC_target);
            break;
        }
        case wxCURSOR_PENCIL:
       {
            cursor = XCreateFontCursor (dpy, XC_pencil);
            break;
        }
        case wxCURSOR_MAGNIFIER:
        {
            cursor = XCreateFontCursor (dpy, XC_sizing);
            break;
        }
        case wxCURSOR_IBEAM:
        {
            cursor = XCreateFontCursor (dpy, XC_xterm);
            break;
        }
        case wxCURSOR_NO_ENTRY:
        {
            cursor = XCreateFontCursor (dpy, XC_pirate);
            break;
        }
        case wxCURSOR_LEFT_BUTTON:
        {
            cursor = XCreateFontCursor (dpy, XC_leftbutton);
            break;
        }
        case wxCURSOR_RIGHT_BUTTON:
        {
            cursor = XCreateFontCursor (dpy, XC_rightbutton);
            break;
        }
        case wxCURSOR_MIDDLE_BUTTON:
        {
            cursor = XCreateFontCursor (dpy, XC_middlebutton);
            break;
        }
        case wxCURSOR_QUESTION_ARROW:
        {
            cursor = XCreateFontCursor (dpy, XC_question_arrow);
            break;
        }
        case wxCURSOR_SIZING:
        {
            cursor = XCreateFontCursor (dpy, XC_sizing);
            break;
        }
        case wxCURSOR_WATCH:
        {
            cursor = XCreateFontCursor (dpy, XC_watch);
            break;
        }
        case wxCURSOR_SPRAYCAN:
        {
            cursor = XCreateFontCursor (dpy, XC_spraycan);
            break;
        }
        case wxCURSOR_PAINT_BRUSH:
        {
            cursor = XCreateFontCursor (dpy, XC_spraycan);
            break;
        }
        case wxCURSOR_SIZENWSE:
        case wxCURSOR_SIZENESW:
        {
            // Not available in X
            cursor = XCreateFontCursor (dpy, XC_crosshair);
            break;
        }
        case wxCURSOR_SIZEWE:
        {
            cursor = XCreateFontCursor (dpy, XC_sb_h_double_arrow);
            break;
        }
        case wxCURSOR_SIZENS:
        {
            cursor = XCreateFontCursor (dpy, XC_sb_v_double_arrow);
            break;
        }
        case wxCURSOR_POINT_LEFT:
        {
            cursor = XCreateFontCursor (dpy, XC_sb_left_arrow);
            break;
        }
        case wxCURSOR_POINT_RIGHT:
        {
            cursor = XCreateFontCursor (dpy, XC_sb_right_arrow);
            break;
        }
        // (JD Huggins) added more stock cursors for X
        // X-only cursors BEGIN
        case wxCURSOR_CROSS_REVERSE:
        {
            cursor = XCreateFontCursor(dpy, XC_cross_reverse);
            break;
        }
        case wxCURSOR_DOUBLE_ARROW:
        {
            cursor = XCreateFontCursor(dpy, XC_double_arrow);
            break;
        }
        case wxCURSOR_BASED_ARROW_UP:
        {
            cursor = XCreateFontCursor(dpy, XC_based_arrow_up);
            break;
        }
        case wxCURSOR_BASED_ARROW_DOWN:
        {
            cursor = XCreateFontCursor(dpy, XC_based_arrow_down);
            break;
        }
        default:
        case wxCURSOR_ARROW:
        {
            cursor = XCreateFontCursor (dpy, XC_top_left_arrow);
            break;
        }
        case wxCURSOR_BLANK:
        {
            GC gc;
            XGCValues gcv;
            Pixmap empty_pixmap;
            XColor blank_color;

            empty_pixmap = XCreatePixmap (dpy, RootWindow (dpy, DefaultScreen (dpy)),
                              16, 16, 1);
            gcv.function = GXxor;
            gc = XCreateGC (dpy,
                    empty_pixmap,
                    GCFunction,
                    &gcv);
            XCopyArea (dpy,
                   empty_pixmap,
                   empty_pixmap,
                   gc,
                   0, 0,
                   16, 16,
                   0, 0);
            XFreeGC (dpy, gc);
            cursor = XCreatePixmapCursor (dpy,
                            empty_pixmap,
                            empty_pixmap,
                            &blank_color,
                            &blank_color,
                            8, 8);

            break;
        }
    }
    return (WXCursor) cursor;
}

// Global cursor setting
void wxSetCursor(const wxCursor& WXUNUSED(cursor))
{
  // Nothing to do for Motif (no global cursor)
}



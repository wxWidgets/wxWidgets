/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "cursor.h"
#endif

#include "wx/cursor.h"
#include "wx/utils.h"

#include <gdk/gdk.h>

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// wxCursor
//-----------------------------------------------------------------------------

class wxCursorRefData: public wxObjectRefData
{
  public:

    wxCursorRefData();
    ~wxCursorRefData();

    GdkCursor *m_cursor;
};

wxCursorRefData::wxCursorRefData()
{
    m_cursor = (GdkCursor *) NULL;
}

wxCursorRefData::~wxCursorRefData()
{
    if (m_cursor) gdk_cursor_destroy( m_cursor );
}

//-----------------------------------------------------------------------------

#define M_CURSORDATA ((wxCursorRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxCursor,wxObject)

wxCursor::wxCursor()
{
}

wxCursor::wxCursor( int cursorId )
{
    m_refData = new wxCursorRefData();

    GdkCursorType gdk_cur = GDK_LEFT_PTR;
    switch (cursorId)
    {
        case wxCURSOR_DEFAULT:          gdk_cur = GDK_LEFT_PTR; break;
        case wxCURSOR_HAND:             gdk_cur = GDK_HAND1; break;
        case wxCURSOR_CROSS:            gdk_cur = GDK_CROSSHAIR; break;
        case wxCURSOR_SIZEWE:           gdk_cur = GDK_SB_H_DOUBLE_ARROW; break;
        case wxCURSOR_SIZENS:           gdk_cur = GDK_SB_V_DOUBLE_ARROW; break;
        case wxCURSOR_WAIT:
        case wxCURSOR_WATCH:            gdk_cur = GDK_WATCH; break;
        case wxCURSOR_SIZING:           gdk_cur = GDK_SIZING; break;
        case wxCURSOR_SPRAYCAN:         gdk_cur = GDK_SPRAYCAN; break;
        case wxCURSOR_IBEAM:            gdk_cur = GDK_XTERM; break;
        case wxCURSOR_PENCIL:           gdk_cur = GDK_PENCIL; break;
        case wxCURSOR_NO_ENTRY:         gdk_cur = GDK_PIRATE; break;
        case wxCURSOR_SIZENWSE:
        case wxCURSOR_SIZENESW:         gdk_cur = GDK_FLEUR; break;
        case wxCURSOR_QUESTION_ARROW:   gdk_cur = GDK_QUESTION_ARROW; break;
        case wxCURSOR_PAINT_BRUSH:      gdk_cur = GDK_SPRAYCAN; break;
        case wxCURSOR_MAGNIFIER:        gdk_cur = GDK_PLUS; break;
        case wxCURSOR_CHAR:             gdk_cur = GDK_XTERM; break;
        case wxCURSOR_LEFT_BUTTON:      gdk_cur = GDK_LEFTBUTTON; break;
        case wxCURSOR_MIDDLE_BUTTON:    gdk_cur = GDK_MIDDLEBUTTON; break;
        case wxCURSOR_RIGHT_BUTTON:     gdk_cur = GDK_RIGHTBUTTON; break;
        case wxCURSOR_BULLSEYE:         gdk_cur = GDK_TARGET; break;

        case wxCURSOR_POINT_LEFT:       gdk_cur = GDK_SB_LEFT_ARROW; break;
        case wxCURSOR_POINT_RIGHT:      gdk_cur = GDK_SB_RIGHT_ARROW; break;
/*
        case wxCURSOR_DOUBLE_ARROW:     gdk_cur = GDK_DOUBLE_ARROW; break;
        case wxCURSOR_CROSS_REVERSE:    gdk_cur = GDK_CROSS_REVERSE; break;
        case wxCURSOR_BASED_ARROW_UP:   gdk_cur = GDK_BASED_ARROW_UP; break;
        case wxCURSOR_BASED_ARROW_DOWN: gdk_cur = GDK_BASED_ARROW_DOWN; break;
*/
        default:
            wxFAIL_MSG(wxT("unsupported cursor type"));
            // will use the standard one

        case wxCURSOR_ARROW:
            break;
    }

    M_CURSORDATA->m_cursor = gdk_cursor_new( gdk_cur );
}

wxCursor::wxCursor( const wxCursor &cursor )
{
    Ref( cursor );
}

wxCursor::~wxCursor()
{
}

wxCursor& wxCursor::operator = ( const wxCursor& cursor )
{
    if (*this == cursor)
        return (*this);

    Ref( cursor );

    return *this;
}

bool wxCursor::operator == ( const wxCursor& cursor ) const
{
    return m_refData == cursor.m_refData;
}

bool wxCursor::operator != ( const wxCursor& cursor ) const
{
    return m_refData != cursor.m_refData;
}

bool wxCursor::Ok() const
{
    return (m_refData != NULL);
}

GdkCursor *wxCursor::GetCursor() const
{
    return M_CURSORDATA->m_cursor;
}

//-----------------------------------------------------------------------------
// busy cursor routines
//-----------------------------------------------------------------------------

extern wxCursor g_globalCursor;

static wxCursor  gs_savedCursor;
static int       gs_busyCount = 0;

void wxEndBusyCursor()
{
    if (--gs_busyCount > 0)
        return;

    wxSetCursor( gs_savedCursor );
    gs_savedCursor = wxNullCursor;

    wxYield();
}

void wxBeginBusyCursor( wxCursor *WXUNUSED(cursor) )
{
    if (gs_busyCount++ > 0)
        return;

    wxASSERT_MSG( !gs_savedCursor.Ok(),
                  wxT("forgot to call wxEndBusyCursor, will leak memory") );

    gs_savedCursor = g_globalCursor;

    wxSetCursor( wxCursor(wxCURSOR_WATCH) );

    wxYield();
}

bool wxIsBusy()
{
    return gs_busyCount > 0;
}

void wxSetCursor( const wxCursor& cursor )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    g_globalCursor = cursor;
}

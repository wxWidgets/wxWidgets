/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "cursor.h"
#endif

#include "wx/cursor.h"

//-----------------------------------------------------------------------------
// wxCursor
//-----------------------------------------------------------------------------

class wxCursorRefData: public wxObjectRefData
{
  public:
  
    wxCursorRefData(void);
    ~wxCursorRefData(void);
  
    GdkCursor *m_cursor;
};

wxCursorRefData::wxCursorRefData(void)
{
  m_cursor = (GdkCursor *) NULL;
}

wxCursorRefData::~wxCursorRefData(void)
{
  if (m_cursor) gdk_cursor_destroy( m_cursor );
}

//-----------------------------------------------------------------------------

#define M_CURSORDATA ((wxCursorRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxCursor,wxObject)

wxCursor::wxCursor(void)
{
}

wxCursor::wxCursor( int cursorId )
{
  m_refData = new wxCursorRefData();
  
  GdkCursorType gdk_cur = GDK_LEFT_PTR;
  switch (cursorId)
  {
    case wxCURSOR_HAND:       gdk_cur = GDK_HAND1; break;
    case wxCURSOR_CROSS:      gdk_cur = GDK_CROSSHAIR; break;
    case wxCURSOR_SIZEWE:     gdk_cur = GDK_SB_H_DOUBLE_ARROW; break;
    case wxCURSOR_SIZENS:     gdk_cur = GDK_SB_V_DOUBLE_ARROW; break;
    case wxCURSOR_WAIT:       gdk_cur = GDK_WATCH; break;
    case wxCURSOR_WATCH:      gdk_cur = GDK_WATCH; break;
    case wxCURSOR_SIZING:     gdk_cur = GDK_SIZING; break;
    case wxCURSOR_SPRAYCAN:   gdk_cur = GDK_SPRAYCAN; break;
    case wxCURSOR_IBEAM:      gdk_cur = GDK_XTERM; break;
    case wxCURSOR_PENCIL:     gdk_cur = GDK_PENCIL; break;
    case wxCURSOR_NO_ENTRY:   gdk_cur = GDK_PIRATE; break;
  }
  
  M_CURSORDATA->m_cursor = gdk_cursor_new( gdk_cur );
  
/*
  do that yourself
   
  wxCURSOR_BULLSEYE,
  wxCURSOR_CHAR,
  wxCURSOR_LEFT_BUTTON,
  wxCURSOR_MAGNIFIER,
  wxCURSOR_MIDDLE_BUTTON,
  wxCURSOR_NO_ENTRY,
  wxCURSOR_PAINT_BRUSH,
  wxCURSOR_POINT_LEFT,
  wxCURSOR_POINT_RIGHT,
  wxCURSOR_QUESTION_ARROW,
  wxCURSOR_RIGHT_BUTTON,
  wxCURSOR_SIZENESW,
  wxCURSOR_SIZENS,
  wxCURSOR_SIZENWSE,
  wxCURSOR_SIZEWE,
  wxCURSOR_BLANK
,
  wxCURSOR_CROSS_REVERSE,
  wxCURSOR_DOUBLE_ARROW,
  wxCURSOR_BASED_ARROW_UP,
  wxCURSOR_BASED_ARROW_DOWN
*/
   
}

wxCursor::wxCursor( const wxCursor &cursor )
{
  Ref( cursor );
}

wxCursor::wxCursor( const wxCursor *cursor )
{
  UnRef();
  if (cursor) Ref( *cursor );
}

wxCursor::~wxCursor(void)
{
}

wxCursor& wxCursor::operator = ( const wxCursor& cursor )
{
  if (*this == cursor) return (*this); 
  Ref( cursor ); 
  return *this; 
}

bool wxCursor::operator == ( const wxCursor& cursor )
{
  return m_refData == cursor.m_refData; 
}

bool wxCursor::operator != ( const wxCursor& cursor )
{
  return m_refData != cursor.m_refData; 
}

bool wxCursor::Ok(void) const
{
  return TRUE;
}

GdkCursor *wxCursor::GetCursor(void) const
{
  return M_CURSORDATA->m_cursor;
}

//-----------------------------------------------------------------------------
// busy cursor routines
//-----------------------------------------------------------------------------

bool g_isBusy = FALSE;

void wxEndBusyCursor(void)
{
  g_isBusy = FALSE;
}

void wxBeginBusyCursor( wxCursor *WXUNUSED(cursor) )
{
  g_isBusy = TRUE;
}

bool wxIsBusy(void)
{
  return g_isBusy;
}

void wxSetCursor( const wxCursor& cursor )
{
  extern wxCursor *g_globalCursor;
  if (g_globalCursor) (*g_globalCursor) = cursor;

  if (cursor.Ok()) {}
}



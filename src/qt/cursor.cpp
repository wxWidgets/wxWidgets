/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.cpp
// Purpose:     wxCursor class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "cursor.h"
#endif

#include "wx/cursor.h"

IMPLEMENT_DYNAMIC_CLASS(wxCursor, wxBitmap)

wxCursorRefData::wxCursorRefData()
{
    m_width = 32; m_height = 32;

/* TODO
    m_hCursor = 0 ;
*/
}

wxCursorRefData::~wxCursorRefData()
{
    // TODO: destroy cursor
}

// Cursors
wxCursor::wxCursor()
{
}

wxCursor::wxCursor(const char WXUNUSED(bits)[], int WXUNUSED(width), int WXUNUSED(height),
    int WXUNUSED(hotSpotX), int WXUNUSED(hotSpotY), const char WXUNUSED(maskBits)[])
{
}

wxCursor::wxCursor(const wxString& cursor_file, long flags, int hotSpotX, int hotSpotY)
{
    m_refData = new wxIconRefData;

    // TODO: create cursor from a file
}

// Cursors by stock number
wxCursor::wxCursor(int cursor_type)
{
  m_refData = new wxIconRefData;

/* TODO
  switch (cursor_type)
  {
    case wxCURSOR_WAIT:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_WAIT);
      break;
    case wxCURSOR_IBEAM:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_IBEAM);
      break;
    case wxCURSOR_CROSS:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_CROSS);
      break;
    case wxCURSOR_SIZENWSE:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_SIZENWSE);
      break;
    case wxCURSOR_SIZENESW:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_SIZENESW);
      break;
    case wxCURSOR_SIZEWE:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_SIZEWE);
      break;
    case wxCURSOR_SIZENS:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_SIZENS);
      break;
    case wxCURSOR_CHAR:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_HAND:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_HAND");
      break;
    }
    case wxCURSOR_BULLSEYE:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_BULLSEYE");
      break;
    }
    case wxCURSOR_PENCIL:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_PENCIL");
      break;
    }
    case wxCURSOR_MAGNIFIER:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_MAGNIFIER");
      break;
    }
    case wxCURSOR_NO_ENTRY:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_NO_ENTRY");
      break;
    }
    case wxCURSOR_LEFT_BUTTON:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_RIGHT_BUTTON:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_MIDDLE_BUTTON:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_SIZING:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_SIZING");
      break;
    }
    case wxCURSOR_WATCH:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_WATCH");
      break;
    }
    case wxCURSOR_SPRAYCAN:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_ROLLER");
      break;
    }
    case wxCURSOR_PAINT_BRUSH:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_PBRUSH");
      break;
    }
    case wxCURSOR_POINT_LEFT:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_PLEFT");
      break;
    }
    case wxCURSOR_POINT_RIGHT:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_PRIGHT");
      break;
    }
    case wxCURSOR_QUESTION_ARROW:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_QARROW");
      break;
    }
    case wxCURSOR_BLANK:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), "wxCURSOR_BLANK");
      break;
    }
    default:
    case wxCURSOR_ARROW:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(NULL, IDC_ARROW);
      break;
  }
*/

}

wxCursor::~wxCursor()
{
}

// Global cursor setting
void wxSetCursor(const wxCursor& cursor)
{
  // TODO (optional on platforms with no global cursor)
}



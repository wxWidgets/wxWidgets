/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.cpp
// Purpose:     wxCursor class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "cursor.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/setup.h"
#include "wx/list.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/cursor.h"
#endif

#include "wx/msw/private.h"
#include "wx/msw/dib.h"

#include "assert.h"

#if wxUSE_RESOURCE_LOADING_IN_MSW
#include "wx/msw/curico.h"
#include "wx/msw/curicop.h"
#endif

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxCursor, wxBitmap)
#endif

wxCursorRefData::wxCursorRefData(void)
{
  m_width = 32; m_height = 32;
  m_hCursor = 0 ;
  m_destroyCursor = FALSE;
}

wxCursorRefData::~wxCursorRefData(void)
{
    if ( m_hCursor && m_destroyCursor)
        ::DestroyCursor((HICON) m_hCursor);
}

// Cursors
wxCursor::wxCursor(void)
{
}

wxCursor::wxCursor(const char WXUNUSED(bits)[], int WXUNUSED(width), int WXUNUSED(height),
    int WXUNUSED(hotSpotX), int WXUNUSED(hotSpotY), const char WXUNUSED(maskBits)[])
{
}

wxCursor::wxCursor(const wxString& cursor_file, long flags, int hotSpotX, int hotSpotY)
{
  m_refData = new wxIconRefData;

  M_CURSORDATA->m_destroyCursor = FALSE;
  M_CURSORDATA->m_hCursor = 0;
  M_CURSORDATA->m_ok = FALSE;
  if (flags & wxBITMAP_TYPE_CUR_RESOURCE)
  {
    M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), cursor_file);
    if (M_CURSORDATA->m_hCursor)
      M_CURSORDATA->m_ok = TRUE;
    else
      M_CURSORDATA->m_ok = FALSE;
  }
  else if (flags & wxBITMAP_TYPE_CUR)
  {
#if wxUSE_RESOURCE_LOADING_IN_MSW
    M_CURSORDATA->m_hCursor = (WXHCURSOR) ReadCursorFile((char *)(const char *)cursor_file, wxGetInstance(), &M_CURSORDATA->m_width, &M_CURSORDATA->m_height);
    M_CURSORDATA->m_destroyCursor = TRUE;
#endif
  }
  else if (flags & wxBITMAP_TYPE_ICO)
  {
#if wxUSE_RESOURCE_LOADING_IN_MSW
    M_CURSORDATA->m_hCursor = (WXHCURSOR) IconToCursor((char *)(const char *)cursor_file, wxGetInstance(), hotSpotX, hotSpotY, &M_CURSORDATA->m_width, &M_CURSORDATA->m_height);
    M_CURSORDATA->m_destroyCursor = TRUE;
#endif
  }
  else if (flags & wxBITMAP_TYPE_BMP)
  {
#if wxUSE_RESOURCE_LOADING_IN_MSW
    HBITMAP hBitmap = 0;
    HPALETTE hPalette = 0;
    bool success = ReadDIB((char *)(const char *)cursor_file, &hBitmap, &hPalette) != 0;
    if (!success)
      return;
    if (hPalette)
      DeleteObject(hPalette);
    POINT pnt;
    pnt.x = hotSpotX;
    pnt.y = hotSpotY;
    M_CURSORDATA->m_hCursor = (WXHCURSOR) MakeCursorFromBitmap(wxGetInstance(), hBitmap, &pnt);
    M_CURSORDATA->m_destroyCursor = TRUE;
    DeleteObject(hBitmap);
    if (M_CURSORDATA->m_hCursor)
      M_CURSORDATA->m_ok = TRUE;
#endif
  }
}

// Cursors by stock number
wxCursor::wxCursor(int cursor_type)
{
  m_refData = new wxIconRefData;

  switch (cursor_type)
  {
    case wxCURSOR_WAIT:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_WAIT);
      break;
    case wxCURSOR_IBEAM:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_IBEAM);
      break;
    case wxCURSOR_CROSS:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_CROSS);
      break;
    case wxCURSOR_SIZENWSE:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_SIZENWSE);
      break;
    case wxCURSOR_SIZENESW:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_SIZENESW);
      break;
    case wxCURSOR_SIZEWE:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_SIZEWE);
      break;
    case wxCURSOR_SIZENS:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_SIZENS);
      break;
    case wxCURSOR_CHAR:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_ARROW);
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
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_RIGHT_BUTTON:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_MIDDLE_BUTTON:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_ARROW);
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
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_ARROW);
      break;
  }
}

wxCursor::~wxCursor(void)
{
//    FreeResource(TRUE);
}

bool wxCursor::FreeResource(bool WXUNUSED(force))
{
  if (M_CURSORDATA && M_CURSORDATA->m_hCursor && M_CURSORDATA->m_destroyCursor)
  {
    DestroyCursor((HCURSOR) M_CURSORDATA->m_hCursor);
    M_CURSORDATA->m_hCursor = 0;
  }
  return TRUE;
}

void wxCursor::SetHCURSOR(WXHCURSOR cursor)
{
  if ( !M_CURSORDATA )
    m_refData = new wxCursorRefData;

  M_CURSORDATA->m_hCursor = cursor;
}

// Global cursor setting
void wxSetCursor(const wxCursor& cursor)
{
    extern wxCursor *g_globalCursor;

    if ( cursor.Ok() && cursor.GetHCURSOR() )
    {
        ::SetCursor((HCURSOR) cursor.GetHCURSOR());

        if ( g_globalCursor )
            (*g_globalCursor) = cursor;
    }
}



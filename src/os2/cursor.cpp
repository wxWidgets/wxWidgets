/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.cpp
// Purpose:     wxCursor class
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/setup.h"
#include "wx/list.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/cursor.h"
#include "wx/icon.h"
#endif

#include "wx/os2/private.h"

#include "assert.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxCursor, wxBitmap)
#endif

wxCursorRefData::wxCursorRefData(void)
{
  m_nWidth = 32;
  m_nHeight = 32;
  m_hCursor = 0 ;
  m_destroyCursor = FALSE;
}

wxCursorRefData::~wxCursorRefData(void)
{
//    if ( m_hCursor && m_destroyCursor)
//        ::DestroyCursor((HICON) m_hCursor);
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
  m_refData = new wxCursorRefData;

  M_CURSORDATA->m_destroyCursor = FALSE;
  M_CURSORDATA->m_hCursor = 0;
// TODO:
/*
  M_CURSORDATA->m_bOK = FALSE;
  if (flags & wxBITMAP_TYPE_CUR_RESOURCE)
  {
    M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadImage(wxGetInstance(), cursor_file, IMAGE_CURSOR, 0, 0, 0);
    if (M_CURSORDATA->m_hCursor)
      M_CURSORDATA->m_ok = TRUE;
    else
      M_CURSORDATA->m_ok = FALSE;
  }
  else if (flags & wxBITMAP_TYPE_CUR)
  {
    M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadImage(wxGetInstance(), cursor_file, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
  }
  else if (flags & wxBITMAP_TYPE_ICO)
  {
  }
  else if (flags & wxBITMAP_TYPE_BMP)
  {
  }
*/
}

// Cursors by stock number
wxCursor::wxCursor(int cursor_type)
{
  m_refData = new wxCursorRefData;
// TODO:
/*
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
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_HAND"));
      break;
    }
    case wxCURSOR_BULLSEYE:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_BULLSEYE"));
      break;
    }
    case wxCURSOR_PENCIL:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_PENCIL"));
      break;
    }
    case wxCURSOR_MAGNIFIER:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_MAGNIFIER"));
      break;
    }
    case wxCURSOR_NO_ENTRY:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_NO_ENTRY"));
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
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_SIZING"));
      break;
    }
    case wxCURSOR_WATCH:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_WATCH"));
      break;
    }
    case wxCURSOR_SPRAYCAN:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_ROLLER"));
      break;
    }
    case wxCURSOR_PAINT_BRUSH:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_PBRUSH"));
      break;
    }
    case wxCURSOR_POINT_LEFT:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_PLEFT"));
      break;
    }
    case wxCURSOR_POINT_RIGHT:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_PRIGHT"));
      break;
    }
    case wxCURSOR_QUESTION_ARROW:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_QARROW"));
      break;
    }
    case wxCURSOR_BLANK:
    {
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_BLANK"));
      break;
    }
    default:
    case wxCURSOR_ARROW:
      M_CURSORDATA->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_ARROW);
      break;
  }
*/
}

wxCursor::~wxCursor(void)
{
//    FreeResource(TRUE);
}

bool wxCursor::FreeResource(bool WXUNUSED(force))
{
//  if (M_CURSORDATA && M_CURSORDATA->m_hCursor && M_CURSORDATA->m_destroyCursor)
//  {
//    DestroyCursor((HCURSOR) M_CURSORDATA->m_hCursor);
//    M_CURSORDATA->m_hCursor = 0;
//  }
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
//        ::SetCursor((HCURSOR) cursor.GetHCURSOR());

        if ( g_globalCursor )
            (*g_globalCursor) = cursor;
    }
}


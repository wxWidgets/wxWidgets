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

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "cursor.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/list.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/cursor.h"
    #include "wx/icon.h"
#endif

#include "wx/msw/private.h"
#include "wx/msw/dib.h"

#if wxUSE_RESOURCE_LOADING_IN_MSW
    #include "wx/msw/curico.h"
    #include "wx/msw/curicop.h"
#endif

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARIES
    IMPLEMENT_DYNAMIC_CLASS(wxCursor, wxCursorBase)
#endif

// ----------------------------------------------------------------------------
// wxCursorRefData
// ----------------------------------------------------------------------------

wxCursorRefData::wxCursorRefData()
{
  m_width = 32;
  m_height = 32;

  m_destroyCursor = FALSE;
}

void wxCursorRefData::Free()
{
    if ( m_hCursor && m_destroyCursor )
        ::DestroyCursor((HCURSOR)m_hCursor);
}

// ----------------------------------------------------------------------------
// Cursors
// ----------------------------------------------------------------------------

wxCursor::wxCursor()
{
}

wxCursor::wxCursor(const char WXUNUSED(bits)[],
                   int WXUNUSED(width),
                   int WXUNUSED(height),
                   int WXUNUSED(hotSpotX), int WXUNUSED(hotSpotY),
                   const char WXUNUSED(maskBits)[])
{
}

wxCursor::wxCursor(const wxString& cursor_file,
                   long flags,
                   int hotSpotX, int hotSpotY)
{
    wxCursorRefData *refData = new wxCursorRefData;
    m_refData = refData;

    refData->m_destroyCursor = FALSE;

    if (flags == wxBITMAP_TYPE_CUR_RESOURCE)
    {
#ifdef __WIN95__
        refData->m_hCursor = (WXHCURSOR) LoadImage(wxGetInstance(), cursor_file, IMAGE_CURSOR, 0, 0, 0);
#else
        refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), cursor_file);
#endif
    }
    else if (flags == wxBITMAP_TYPE_CUR)
    {
#ifdef __WIN95__
        refData->m_hCursor = (WXHCURSOR) LoadImage(wxGetInstance(), cursor_file, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
#else
#if wxUSE_RESOURCE_LOADING_IN_MSW
        refData->m_hCursor = (WXHCURSOR) ReadCursorFile(WXSTRINGCAST cursor_file, wxGetInstance(), &refData->m_width, &refData->m_height);
        refData->m_destroyCursor = TRUE;
#endif
#endif
    }
    else if (flags == wxBITMAP_TYPE_ICO)
    {
#if wxUSE_RESOURCE_LOADING_IN_MSW
        refData->m_hCursor = (WXHCURSOR) IconToCursor(WXSTRINGCAST cursor_file, wxGetInstance(), hotSpotX, hotSpotY, &refData->m_width, &refData->m_height);
        refData->m_destroyCursor = TRUE;
#endif
    }
    else if (flags == wxBITMAP_TYPE_BMP)
    {
#if wxUSE_RESOURCE_LOADING_IN_MSW
        HBITMAP hBitmap = 0;
        HPALETTE hPalette = 0;
        bool success = wxReadDIB(WXSTRINGCAST cursor_file, &hBitmap, &hPalette) != 0;
        if (!success)
            return;
        if (hPalette)
            DeleteObject(hPalette);
        POINT pnt;
        pnt.x = hotSpotX;
        pnt.y = hotSpotY;
        refData->m_hCursor = (WXHCURSOR) MakeCursorFromBitmap(wxGetInstance(), hBitmap, &pnt);
        refData->m_destroyCursor = TRUE;
        DeleteObject(hBitmap);
#endif
    }

#if WXWIN_COMPATIBILITY_2
    refData->SetOk();
#endif // WXWIN_COMPATIBILITY_2
}

// Cursors by stock number
wxCursor::wxCursor(int cursor_type)
{
  wxCursorRefData *refData = new wxCursorRefData;
  m_refData = refData;

  switch (cursor_type)
  {
    case wxCURSOR_WAIT:
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_WAIT);
      break;
    case wxCURSOR_IBEAM:
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_IBEAM);
      break;
    case wxCURSOR_CROSS:
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_CROSS);
      break;
    case wxCURSOR_SIZENWSE:
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_SIZENWSE);
      break;
    case wxCURSOR_SIZENESW:
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_SIZENESW);
      break;
    case wxCURSOR_SIZEWE:
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_SIZEWE);
      break;
    case wxCURSOR_SIZENS:
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_SIZENS);
      break;
    case wxCURSOR_CHAR:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_HAND:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_HAND"));
      break;
    }
    case wxCURSOR_BULLSEYE:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_BULLSEYE"));
      break;
    }
    case wxCURSOR_PENCIL:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_PENCIL"));
      break;
    }
    case wxCURSOR_MAGNIFIER:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_MAGNIFIER"));
      break;
    }
    case wxCURSOR_NO_ENTRY:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_NO_ENTRY"));
      break;
    }
    case wxCURSOR_LEFT_BUTTON:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_RIGHT_BUTTON:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_MIDDLE_BUTTON:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_ARROW);
      break;
    }
    case wxCURSOR_SIZING:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_SIZING"));
      break;
    }
    case wxCURSOR_WATCH:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_WATCH"));
      break;
    }
    case wxCURSOR_SPRAYCAN:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_ROLLER"));
      break;
    }
    case wxCURSOR_PAINT_BRUSH:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_PBRUSH"));
      break;
    }
    case wxCURSOR_POINT_LEFT:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_PLEFT"));
      break;
    }
    case wxCURSOR_POINT_RIGHT:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_PRIGHT"));
      break;
    }
    case wxCURSOR_QUESTION_ARROW:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_QARROW"));
      break;
    }
    case wxCURSOR_BLANK:
    {
      refData->m_hCursor = (WXHCURSOR) LoadCursor(wxGetInstance(), wxT("wxCURSOR_BLANK"));
      break;
    }
    default:
    case wxCURSOR_ARROW:
      refData->m_hCursor = (WXHCURSOR) LoadCursor((HINSTANCE) NULL, IDC_ARROW);
      break;
  }
}

wxCursor::~wxCursor()
{
}

// ----------------------------------------------------------------------------
// Global cursor setting
// ----------------------------------------------------------------------------

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



/////////////////////////////////////////////////////////////////////////////
// Name:        icon.cpp
// Purpose:     wxIcon class
// Author:      David Webster
// Modified by:
// Created:     10/09/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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
#include "wx/icon.h"
#endif

#include "wx/os2/private.h"
#include "assert.h"

#include "wx/icon.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxIcon, wxBitmap)
IMPLEMENT_DYNAMIC_CLASS(wxICOFileHandler, wxBitmapHandler)
IMPLEMENT_DYNAMIC_CLASS(wxICOResourceHandler, wxBitmapHandler)
#endif

/*
 * Icons
 */

wxIconRefData::wxIconRefData()
{
  m_hIcon = (WXHICON) NULL ;
}

wxIconRefData::~wxIconRefData()
{
  if ( m_hIcon )
     return;
  // TODO  ::DestroyIcon((HICON) m_hIcon);
}

wxIcon::wxIcon()
{
}

wxIcon::wxIcon(const char WXUNUSED(bits)[], int WXUNUSED(width), int WXUNUSED(height))
{
}

wxIcon::wxIcon(const wxString& icon_file, long flags,
    int desiredWidth, int desiredHeight)

{
    LoadFile(icon_file, flags, desiredWidth, desiredHeight);
}

wxIcon::~wxIcon()
{
}

bool wxIcon::LoadFile(const wxString& filename, long type,
    int desiredWidth, int desiredHeight)
{
  UnRef();

  m_refData = new wxIconRefData;

  wxBitmapHandler *handler = FindHandler(type);

  if ( handler )
    return handler->LoadFile(this, filename, type, desiredWidth, desiredHeight);
  else
    return FALSE;
}

void wxIcon::SetHICON(WXHICON ico)
{
  if ( !M_ICONDATA )
  m_refData = new wxIconRefData;

  M_ICONDATA->m_hIcon = ico;
}

bool wxICOFileHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
    int desiredWidth, int desiredHeight)
{
  return FALSE;
}

bool wxICOResourceHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
    int desiredWidth, int desiredHeight)
{
// TODO
/*
  if ( bitmap->IsKindOf(CLASSINFO(wxIcon)) )
  {
    if (desiredWidth > -1 && desiredHeight > -1)
    {
        M_ICONHANDLERDATA->m_hIcon = (WXHICON) ::LoadImage(wxGetInstance(), name, IMAGE_ICON, desiredWidth, desiredHeight, LR_DEFAULTCOLOR);
    }
    else
    {
      M_ICONHANDLERDATA->m_hIcon = (WXHICON) ::LoadIcon(wxGetInstance(), name);
    }

    ICONINFO info ;
    if (::GetIconInfo((HICON) M_ICONHANDLERDATA->m_hIcon, &info))
    {
        HBITMAP ms_bitmap = info.hbmMask ;
        if (ms_bitmap)
        {
            BITMAP bm;
            ::GetObject(ms_bitmap, sizeof(BITMAP), (LPSTR) &bm);
            M_ICONHANDLERDATA->m_width = bm.bmWidth;
            M_ICONHANDLERDATA->m_height = bm.bmHeight;
        }
        if (info.hbmMask)
            ::DeleteObject(info.hbmMask) ;
        if (info.hbmColor)
            ::DeleteObject(info.hbmColor) ;
      }
    }
    // Override the found values with desired values
    if (desiredWidth > -1 && desiredHeight > -1)
    {
       M_ICONHANDLERDATA->m_width = desiredWidth;
       M_ICONHANDLERDATA->m_height = desiredHeight;
    }

    M_ICONHANDLERDATA->m_ok = (M_ICONHANDLERDATA->m_hIcon != 0);
    return M_ICONHANDLERDATA->m_ok;
  }
  else
*/
    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// Name:        icon.cpp
// Purpose:     wxIcon class
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "icon.h"
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
#include "wx/icon.h"
#endif

#include "wx/msw/private.h"
#include "assert.h"

#if USE_XPM_IN_MSW
#define FOR_MSW 1
#include "..\..\contrib\wxxpm\libxpm.34b\lib\xpm34.h"
#endif

#if USE_RESOURCE_LOADING_IN_MSW
#include "wx/msw/curico.h"
#include "wx/msw/curicop.h"
#endif

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxIcon, wxBitmap)
IMPLEMENT_DYNAMIC_CLASS(wxICOFileHandler, wxBitmapHandler)
IMPLEMENT_DYNAMIC_CLASS(wxICOResourceHandler, wxBitmapHandler)
#endif

/*
 * Icons
 */


wxIconRefData::wxIconRefData(void)
{
  m_hIcon = (WXHICON) NULL ;
}

wxIconRefData::~wxIconRefData(void)
{
  if ( m_hIcon )
    ::DestroyIcon((HICON) m_hIcon);
}

wxIcon::wxIcon(void)
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

wxIcon::~wxIcon(void)
{
}

bool wxIcon::FreeResource(bool force)
{
  if (M_ICONDATA && M_ICONDATA->m_hIcon)
  {
    DestroyIcon((HICON) M_ICONDATA->m_hIcon);
  M_ICONDATA->m_hIcon = (WXHICON) NULL;
  }
  return TRUE;
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
#if USE_RESOURCE_LOADING_IN_MSW
  if ( bitmap->IsKindOf(CLASSINFO(wxIcon)) )
  {
    wxIcon *icon = (wxIcon *)bitmap;
    wxIconRefData *data = (wxIconRefData *)icon->GetRefData();
    data->m_hIcon = (WXHICON)ReadIconFile((char *)name.c_str(), wxGetInstance(),
                                          &data->m_width, &data->m_height);

    data->m_ok = data->m_hIcon != 0;
    return data->m_ok;
  }
  else
    return FALSE;
#else
  return FALSE;
#endif
}

bool wxICOResourceHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
    int desiredWidth, int desiredHeight)
{
  if ( bitmap->IsKindOf(CLASSINFO(wxIcon)) )
  {
#if defined(__WIN32__)
    if (desiredWidth > -1 && desiredHeight > -1)
    {
        M_ICONHANDLERDATA->m_hIcon = (WXHICON) ::LoadImage(wxGetInstance(), name, IMAGE_ICON, desiredWidth, desiredHeight, LR_DEFAULTCOLOR);
    }
    else
#endif
    {
      M_ICONHANDLERDATA->m_hIcon = (WXHICON) ::LoadIcon(wxGetInstance(), name);
    }

#ifdef __WIN32__
      // Win32s doesn't have GetIconInfo function...
      if (M_ICONHANDLERDATA->m_hIcon && wxGetOsVersion()!=wxWIN32S)
      {
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
#else
      M_ICONHANDLERDATA->m_width = 32;
      M_ICONHANDLERDATA->m_height = 32;
#endif
      M_ICONHANDLERDATA->m_ok = (M_ICONHANDLERDATA->m_hIcon != 0);
      return M_ICONHANDLERDATA->m_ok;
  }
  else
      return FALSE;
}


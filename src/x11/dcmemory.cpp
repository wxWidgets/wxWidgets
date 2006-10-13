/////////////////////////////////////////////////////////////////////////////
// Name:        src/x11/dcmemory.cpp
// Purpose:     wxMemoryDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dcmemory.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/settings.h"
#endif

#include "wx/x11/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC,wxWindowDC)

wxMemoryDC::wxMemoryDC( const wxBitmap& bitmap )
    : wxWindowDC()
{
    m_ok = false;

    m_display = (WXDisplay *) wxGlobalDisplay();

    int screen = DefaultScreen( wxGlobalDisplay() );
    m_cmap = (WXColormap) DefaultColormap( wxGlobalDisplay(), screen );

    if ( bitmap.IsOk() )
        SelectObject(bitmap);
}

wxMemoryDC::wxMemoryDC( wxDC *WXUNUSED(dc) )
  : wxWindowDC()
{
    m_ok = false;

    m_display = (WXDisplay *) wxGlobalDisplay();

    int screen = DefaultScreen( wxGlobalDisplay() );
    m_cmap = (WXColormap) DefaultColormap( wxGlobalDisplay(), screen );
}

wxMemoryDC::~wxMemoryDC()
{
}

void wxMemoryDC::SelectObject( const wxBitmap& bitmap )
{
    Destroy();

    m_selected = bitmap;
    if (m_selected.Ok())
    {
        if (m_selected.GetPixmap())
        {
            m_window = (WXWindow) m_selected.GetPixmap();
        }
        else
        {
            m_window = m_selected.GetBitmap();
        }

        m_isMemDC = true;

        SetUpDC();
    }
    else
    {
        m_ok = false;
        m_window = NULL;
    }
}

void wxMemoryDC::DoGetSize( int *width, int *height ) const
{
    if (m_selected.Ok())
    {
        if (width) (*width) = m_selected.GetWidth();
        if (height) (*height) = m_selected.GetHeight();
    }
    else
    {
        if (width) (*width) = 0;
        if (height) (*height) = 0;
    }
}

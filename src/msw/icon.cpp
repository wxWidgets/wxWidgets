/////////////////////////////////////////////////////////////////////////////
// Name:        msw/icon.cpp
// Purpose:     wxIcon class
// Author:      Julian Smart
// Modified by: 20.11.99 (VZ): don't derive from wxBitmap any more
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "icon.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/list.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/icon.h"
#endif

#include "wx/msw/private.h"

#if wxUSE_RESOURCE_LOADING_IN_MSW
    #include "wx/msw/curico.h"
    #include "wx/msw/curicop.h"
#endif

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

    IMPLEMENT_DYNAMIC_CLASS(wxIcon, wxIconBase)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxIconRefData
// ----------------------------------------------------------------------------

void wxIconRefData::Free()
{
    if ( m_hIcon )
    {
        ::DestroyIcon((HICON) m_hIcon);

        m_hIcon = 0;
    }
}

// ----------------------------------------------------------------------------
// wxIcon
// ----------------------------------------------------------------------------

wxIcon::wxIcon()
{
}

wxIcon::wxIcon(const char WXUNUSED(bits)[],
               int WXUNUSED(width),
               int WXUNUSED(height))
{
}

wxIcon::wxIcon(const wxString& iconfile,
               long flags,
               int desiredWidth,
               int desiredHeight)

{
    LoadFile(iconfile, flags, desiredWidth, desiredHeight);
}

wxIcon::~wxIcon()
{
}

bool wxIcon::LoadFile(const wxString& filename,
                      long type,
                      int desiredWidth, int desiredHeight)
{
    UnRef();

    wxGDIImageHandler *handler = FindHandler(type);

    if ( !handler )
    {
        // say something?
        return FALSE;
    }

    return handler->Load(this, filename, type, desiredWidth, desiredHeight);
}


/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/icon.cpp
// Purpose:     wxIcon class
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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
    #include "wx/bitmap.h"
    #include "wx/log.h"
#endif

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxIcon, wxGDIObject)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxIconRefData
// ----------------------------------------------------------------------------

void wxIconRefData::Free()
{
}

// ----------------------------------------------------------------------------
// wxIcon
// ----------------------------------------------------------------------------

wxIcon::wxIcon(const char bits[], int width, int height)
{
}

wxIcon::wxIcon(const wxString& iconfile,
               long flags,
               int desiredWidth,
               int desiredHeight)

{
}

wxIcon::wxIcon(const wxIconLocation& loc)
{
}

wxIcon::~wxIcon()
{
}

void wxIcon::CopyFromBitmap(const wxBitmap& bmp)
{
}

void wxIcon::CreateIconFromXpm(const char **data)
{
}

bool wxIcon::LoadFile(const wxString& filename,
                      long type,
                      int desiredWidth, int desiredHeight)
{
    return false;
}


///////////////////////////////////////////////////////////////////////////////
// Name:        msw/spawnbrowser.cpp
// Purpose:     implements wxSpawnBrowser misc function
// Author:      Jason Quijano
// Modified by:
// Created:     13.06.01
// RCS-ID:
// Copyright:   (c) 2001 Jason Quijano <jasonq@scitechsoft.com>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "spawnbrowser.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SPAWNBROWSER

#ifndef WX_PRECOMP
    #include "wx/string.h"
#endif //WX_PRECOMP

#include "wx/spawnbrowser.h"

// ----------------------------------------------------------------------------
// wxSpawnBrowser:
// ----------------------------------------------------------------------------

bool wxSpawnBrowser (wxWindow *parent, wxString href)
{
    return ShellExecute(parent ? (HWND)parent->GetHWND() : NULL,NULL,href.c_str(),NULL,"",SW_SHOWNORMAL);
}

#endif // wxUSE_SPAWNBROWSER

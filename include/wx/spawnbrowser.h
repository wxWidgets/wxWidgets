///////////////////////////////////////////////////////////////////////////////
// Name:        wx/spawnbrowser.h
// Purpose:     wxSpawnBrowser can be used to spawn a browser
// Author:      Jason Quijano
// Modified by:
// Created:     13.06.01
// RCS-ID:
// Copyright:   (c) 2001 Jason Quijano <jasonq@scitechsoft.com>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SPAWNBROWSER_H_
#define _WX_SPAWNBROWSER_H_

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "spawnbrowser.h"
#endif

#if wxUSE_SPAWNBROWSER

// ----------------------------------------------------------------------------
// wxSpawnBrowser
// ----------------------------------------------------------------------------

WXDLLEXPORT bool wxSpawnBrowser(wxWindow *parent, wxString href);
#endif // wxUSE_SPAWNBROWSER

#endif // _WX_SPAWNBROWSER_H_

/////////////////////////////////////////////////////////////////////////////
// Name:        statbmp.cpp
// Purpose:     wxStaticBitmap
// Author:      David Webster
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/window.h"
#include "wx/os2/private.h"

#ifndef WX_PRECOMP
    #include "wx/icon.h"
    #include "wx/statbmp.h"
#endif

#include <stdio.h>

// ---------------------------------------------------------------------------
// macors
// ---------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxStaticBitmap, wxControl)
#endif

// ---------------------------------------------------------------------------
//  wxStaticBitmap
// ---------------------------------------------------------------------------

bool wxStaticBitmap::Create(wxWindow *parent, wxWindowID id,
           const wxBitmap& bitmap,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    Init();

    SetName(name);
    if (parent) parent->AddChild(this);

    m_backgroundColour = parent->GetBackgroundColour() ;
    m_foregroundColour = parent->GetForegroundColour() ;

    if ( id == -1 )
        m_windowId = (int)NewControlId();
    else
        m_windowId = id;

    m_windowStyle = style;

    int x = pos.x;
    int y = pos.y;
    int width = size.x;
    int height = size.y;

    m_windowStyle = style;

    m_isIcon = bitmap.IsKindOf(CLASSINFO(wxIcon));

    // TODO: create static bitmap control
    wxCHECK_MSG( m_hWnd, FALSE, wxT("Failed to create static bitmap") );

    SetBitmap(bitmap);

    // Subclass again for purposes of dialog editing mode
    SubclassWin(m_hWnd);

    SetFont(GetParent()->GetFont());

    SetSize(x, y, width, height);

    return FALSE;
}

bool wxStaticBitmap::ImageIsOk() const
{
    if ( m_isIcon && m_image.icon )
        return m_image.icon->Ok();
    else if ( m_image.bitmap )
        return m_image.bitmap->Ok();
    else
        return FALSE;
}

void wxStaticBitmap::Free()
{
    if ( m_isIcon )
        delete m_image.icon;
    else
        delete m_image.bitmap;

    m_image.icon = NULL;
}

wxSize wxStaticBitmap::DoGetBestSize()
{
    // reuse the current size (as wxWindow does) instead of using some
    // arbitrary default size (as wxControl, our immediate base class, does)
    return wxWindow::DoGetBestSize();
}

void wxStaticBitmap::SetBitmap(const wxBitmap& bitmap)
{
    Free();

    m_isIcon = bitmap.IsKindOf(CLASSINFO(wxIcon));
    if ( m_isIcon )
        m_image.icon = new wxIcon((const wxIcon&)bitmap);
    else
        m_image.bitmap = new wxBitmap(bitmap);

    int x, y;
    int w, h;
    GetPosition(&x, &y);
    GetSize(&w, &h);

    // TODO: redraw bitmap
}



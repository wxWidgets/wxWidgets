///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/panel.cpp
// Purpose:     Implementation of wxMSW-specific wxPanel class.
// Author:      Vadim Zeitlin
// Created:     2011-03-18
// RCS-ID:      $Id: wxhead.cpp,v 1.11 2010-04-22 12:44:51 zeitlin Exp $
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/brush.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

// ============================================================================
// implementation
// ============================================================================

wxPanel::~wxPanel()
{
    delete m_backgroundBrush;
}

bool wxPanel::HasTransparentBackground()
{
    for ( wxWindow *win = GetParent(); win; win = win->GetParent() )
    {
        if ( win->MSWHasInheritableBackground() )
            return true;

        if ( win->IsTopLevel() )
            break;
    }

    return false;
}

void wxPanel::DoSetBackgroundBitmap(const wxBitmap& bmp)
{
    delete m_backgroundBrush;
    m_backgroundBrush = bmp.IsOk() ? new wxBrush(bmp) : NULL;

    // Our transparent children should use our background if we have it,
    // otherwise try to restore m_inheritBgCol to some reasonable value: true
    // if we also have non-default background colour or false otherwise.
    m_inheritBgCol = bmp.IsOk() || UseBgCol();
}

WXHBRUSH wxPanel::MSWGetCustomBgBrush()
{
    if ( m_backgroundBrush )
        return (WXHBRUSH)m_backgroundBrush->GetResourceHandle();

    return wxPanelBase::MSWGetCustomBgBrush();
}

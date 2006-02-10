///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/dpycmn.cpp
// Purpose:     wxDisplayBase implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     01.03.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DISPLAY

#ifndef WX_PRECOMP
    #include "wx/gdicmn.h"
    #include "wx/window.h"
#endif //WX_PRECOMP

#include "wx/display.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxArrayVideoModes)

const wxVideoMode wxDefaultVideoMode;

// ============================================================================
// implementation
// ============================================================================

wxDisplayBase::wxDisplayBase(size_t index)
             : m_index (index)
{
    wxASSERT_MSG( m_index < GetCount(),
                    wxT("An invalid index was passed to wxDisplay") );
}

// MSW has its own specific implementation of this
#ifndef __WXMSW__

int wxDisplayBase::GetFromWindow(wxWindow *window)
{
    wxCHECK_MSG( window, wxNOT_FOUND, _T("NULL window") );

    // consider that the window belong to the display containing its centre
    const wxRect r(window->GetRect());
    return GetFromPoint(wxPoint(r.x + r.width/2, r.y + r.height/2));
}

#endif // !__WXMSW__

#endif // wxUSE_DISPLAY

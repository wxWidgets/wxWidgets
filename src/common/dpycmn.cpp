///////////////////////////////////////////////////////////////////////////////
// Name:        common/dpycmn.cpp
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "displaybase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/display.h"

#if wxUSE_DISPLAY

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxArrayVideoModes);

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

#endif // wxUSE_DISPLAY


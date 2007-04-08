///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/btncmn.cpp
// Purpose:     implementation of wxButtonBase
// Author:      Vadim Zeitlin
// Created:     2007-04-08
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwindows.org>
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
    #include "wx/button.h"
    #include "wx/toplevel.h"
#endif //WX_PRECOMP

// ============================================================================
// implementation
// ============================================================================

wxWindow *wxButtonBase::SetDefault()
{
    wxTopLevelWindow * const
        tlw = wxDynamicCast(wxGetTopLevelParent(this), wxTopLevelWindow);

    wxCHECK_MSG( tlw, NULL, _T("button without top level window?") );

    return tlw->SetDefaultItem(this);
}


///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/desktopenv.cpp
// Purpose:     implementation of wxDesktopEnvBase
// Author:      Igor Korot
// Created:     2021-09-05
// Copyright:   (c) 2021 wxWidgets development team
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

#include "wx/desktopenv.h"

bool wxDesktopEnv::MoveToRecycleBin(const wxString &WXUNUSED(path))
{
    wxFAIL_MSG( _( "Not implemented!" ) );
    return false;
}

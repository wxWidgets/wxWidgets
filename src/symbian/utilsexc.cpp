/////////////////////////////////////////////////////////////////////////////
// Name:        src/symbian/utilsexec.cpp
// Purpose:     wxExecute implementation for Symbian
// Author:      Jordan Langholz
// RCS-ID:      $Id$
// Copyright:   (c) Jordan Langholz
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include "wx/stream.h"
#include "wx/process.h"

#include "wx/apptrait.h"

#include "wx/module.h"
#include <e32std.h>

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// private types
// ----------------------------------------------------------------------------

// ============================================================================
// implementation
// ============================================================================

// ============================================================================
// wxExecute functions family
// ============================================================================

// see http://wiki.forum.nokia.com/index.php/How_to_start_and_stop_exe for implement
long wxExecute(const wxString& cmd, int flags, wxProcess *handler)
{
    return 0;
}

long wxExecute(wxChar **argv, int flags, wxProcess *handler)
{
    return 0;
}


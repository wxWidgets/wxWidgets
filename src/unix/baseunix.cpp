///////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/baseunix.cpp
// Purpose:     misc stuff only used in console applications under Unix
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.06.2003
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

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/utils.h"
#endif //WX_PRECOMP

#include "wx/unix/execute.h"
#include "wx/evtloop.h"
#include "wx/gsocket.h"

#include "wx/unix/private/timer.h"

// for waitpid()
#include <sys/types.h>
#include <sys/wait.h>

// ============================================================================
// wxConsoleAppTraits implementation
// ============================================================================

#if wxUSE_TIMER

#endif // wxUSE_TIMER

//  Note: wxConsoleAppTraits::CreateEventLoop() is defined in evtloopunix.cpp!

///////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/basemsw.cpp
// Purpose:     misc stuff only used in applications under PalmOS
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10.13.2004
// RCS-ID:      $Id$
// Copyright:   (c) 2004 William Osborne
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
#endif //WX_PRECOMP

#include "wx/apptrait.h"

#include "wx/recguard.h"

// ============================================================================
// wxConsoleAppTraits implementation
// ============================================================================

void wxConsoleAppTraits::AlwaysYield()
{
}

void *wxConsoleAppTraits::BeforeChildWaitLoop()
{
    return NULL;
}

void wxConsoleAppTraits::AfterChildWaitLoop(void * WXUNUSED(data))
{
}

bool wxConsoleAppTraits::DoMessageFromThreadWait()
{
    return true;
}


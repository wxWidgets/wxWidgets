///////////////////////////////////////////////////////////////////////////////
// Name:        wx/build.h
// Purpose:     wxBuildOptions class declaration
// Author:      Vadim Zeitlin
// Modified by:
// Created:     07.05.02
// RCS-ID:      $Id$
// Copyright:   (c) 2002 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BUILD_H_
#define _WX_BUILD_H_

#include "wx/version.h"

// ----------------------------------------------------------------------------
// wxBuildOptions
// ----------------------------------------------------------------------------

class wxBuildOptions
{
public:
    // the ctor must be inline to get the compilation settings of the code
    // which included this header
    wxBuildOptions()
    {
        // debug/release
#ifdef __WXDEBUG__
        m_isDebug = TRUE;
#else
        m_isDebug = FALSE;
#endif

        // version: we don't test the micro version as hopefully changes
        // between 2 micro versions don't result in fatal compatibility
        // problems
        m_verMaj = wxMAJOR_VERSION;
        m_verMin = wxMINOR_VERSION;
    }

private:
    // the version
    int m_verMaj,
        m_verMin;

    // compiled with __WXDEBUG__?
    bool m_isDebug;

    // actually only CheckBuildOptions() should be our friend but well...
    friend class wxAppBase;
};

#endif // _WX_BUILD_H_


///////////////////////////////////////////////////////////////////////////////
// Name:        unix/stdpaths.cpp
// Purpose:     wxStandardPaths implementation for Unix systems
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2004-10-19
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows license
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
    #include "wx/app.h"
#endif //WX_PRECOMP

#include "wx/filename.h"

#include "wx/stdpaths.h"

#ifdef __LINUX__
    #include <unistd.h>
#endif

// ============================================================================
// wxStandardPaths implementation
// ============================================================================

// ----------------------------------------------------------------------------
// prefix management
// ----------------------------------------------------------------------------

void wxStandardPaths::SetInstallPrefix(const wxString& prefix)
{
    m_prefix = prefix;
}

wxString wxStandardPaths::GetInstallPrefix() const
{
    if ( m_prefix.empty() )
    {
        wxStandardPaths *self = wx_const_cast(wxStandardPaths *, this);

#ifdef __LINUX__
        // under Linux, we can get location of the executable
        wxString exe;
        if ( readlink("/proc/self/exe", wxStringBuffer(exe), 1024) != -1 )
        {
            // consider that we're in the last "bin" subdirectory of our prefix
            wxString basename(wxString(wxTheApp->argv[0]).AfterLast(_T('/')));
            size_t pos = exe.find(_T("/bin/") + basename);
            if ( pos != wxString::npos )
            {
                self->m_prefix.assign(exe, 0, pos);
            }
        }

        if ( m_prefix.empty() )
#endif // __LINUX__
        {
            self->m_prefix = _T("/usr/local");
        }
    }

    return m_prefix;
}

// ----------------------------------------------------------------------------
// public functions
// ----------------------------------------------------------------------------

wxString wxStandardPaths::GetConfigDir() const
{
    return _T("/etc");
}

wxString wxStandardPaths::GetUserConfigDir() const
{
    return wxFileName::GetHomeDir();
}

wxString wxStandardPaths::GetDataDir() const
{
    return AppendAppName(GetInstallPrefix() + _T("/share"));
}

wxString wxStandardPaths::GetLocalDataDir() const
{
    return AppendAppName(_T("/etc"));
}

wxString wxStandardPaths::GetUserDataDir() const
{
    return AppendAppName(wxFileName::GetHomeDir() + _T("/."));
}

wxString wxStandardPaths::GetPluginsDir() const
{
    return wxString();
}


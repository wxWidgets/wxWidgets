///////////////////////////////////////////////////////////////////////////////
// Name:        unix/stdpaths.cpp
// Purpose:     wxStandardPaths implementation for Unix & OpenVMS systems
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

#if wxUSE_STDPATHS

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif //WX_PRECOMP

#include "wx/filename.h"

#include "wx/stdpaths.h"

#if defined( __LINUX__ ) || defined( __VMS )
    #include <unistd.h>
#endif

// ============================================================================
// common VMS/Unix part of wxStandardPaths implementation
// ============================================================================

void wxStandardPaths::SetInstallPrefix(const wxString& prefix)
{
    m_prefix = prefix;
}

wxString wxStandardPaths::GetUserConfigDir() const
{
    return wxFileName::GetHomeDir();
}

wxString wxStandardPaths::GetPluginsDir() const
{
    return wxString();
}

// ============================================================================
// wxStandardPaths implementation for VMS
// ============================================================================

#ifdef __VMS

wxString wxStandardPaths::GetInstallPrefix() const
{
    if ( m_prefix.empty() )
    {
        wx_const_cast(wxStandardPaths *, this)->m_prefix = wxT("/sys$system");
    }
}

wxString wxStandardPaths::GetConfigDir() const
{
   return _T("/sys$manager");
}

wxString wxStandardPaths::GetDataDir() const
{
   return AppendAppName(GetInstallPrefix() + _T("/sys$share"));
}

wxString wxStandardPaths::GetLocalDataDir() const
{
   return AppendAppName(_T("/sys$manager"));
}

wxString wxStandardPaths::GetUserDataDir() const
{
   return wxFileName::GetHomeDir();
}

#else // !__VMS

// ============================================================================
// wxStandardPaths implementation for Unix
// ============================================================================

wxString wxStandardPaths::GetInstallPrefix() const
{
    if ( m_prefix.empty() )
    {
        wxStandardPaths *pathPtr = wx_const_cast(wxStandardPaths *, this);

#ifdef __LINUX__
        // under Linux, we can try to infer the prefix from the location of the
        // executable
        char buf[4096];
        int result = readlink("/proc/self/exe", buf, WXSIZEOF(buf) - sizeof(char));
        if ( result != -1 )
        {
            buf[result] = '\0'; // readlink() doesn't NUL-terminate the buffer

            const wxString exeStr(buf, wxConvLibc);

            // consider that we're in the last "bin" subdirectory of our prefix
            size_t pos = exeStr.rfind(wxT("/bin/"));
            if ( pos != wxString::npos )
                pathPtr->m_prefix.assign(exeStr, 0, pos);
        }
#endif // __LINUX__

        if ( m_prefix.empty() )
        {
            pathPtr->m_prefix = wxT("/usr/local");
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

#endif // __VMS/!__VMS

#endif // wxUSE_STDPATHS

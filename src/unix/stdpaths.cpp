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
        char buf[4096];
        if ( readlink("/proc/self/exe", buf, WXSIZEOF(buf)) != -1 )
        {
            wxString exe(buf, wxConvLibc);

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
#ifdef __VMS
	   self->m_prefix = _T("/sys$system");
#else
	   self->m_prefix = _T("/usr/local");
#endif
        }
    }

    return m_prefix;
}

// ----------------------------------------------------------------------------
// public functions
// ----------------------------------------------------------------------------

wxString wxStandardPaths::GetConfigDir() const
{
#ifdef __VMS
   return _T("/sys$manager");
#else
   return _T("/etc");
#endif
}

wxString wxStandardPaths::GetUserConfigDir() const
{
    return wxFileName::GetHomeDir();
}

wxString wxStandardPaths::GetDataDir() const
{
#ifdef __VMS
   return AppendAppName(GetInstallPrefix() + _T("/sys$share"));
#else
   return AppendAppName(GetInstallPrefix() + _T("/share"));
#endif
}

wxString wxStandardPaths::GetLocalDataDir() const
{
#ifdef __VMS
   return AppendAppName(_T("/sys$manager"));
#else
   return AppendAppName(_T("/etc"));
#endif
}

wxString wxStandardPaths::GetUserDataDir() const
{
#ifdef __VMS
   return wxFileName::GetHomeDir();
#else
   return AppendAppName(wxFileName::GetHomeDir() + _T("/."));
#endif
}

wxString wxStandardPaths::GetPluginsDir() const
{
    return wxString();
}

#endif // wxUSE_STDPATHS

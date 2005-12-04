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
    if (m_prefix.empty())
    {
        wxStandardPaths *pathPtr = wx_const_cast(wxStandardPaths *, this);

#ifdef __LINUX__
        // under Linux, we can get location of the executable
        wxChar buf[4096];
        int result;

        // FIXME: is readlink() Unicode-aware or not???
        result = readlink( (const char*)wxT("/proc/self/exe"), (char*)buf, WXSIZEOF(buf) - sizeof(wxChar) );
        if (result != -1)
        {
            buf[result] = wxChar(0);
            wxString exeStr( buf, wxConvLibc );

            // consider that we're in the last "bin" subdirectory of our prefix
            wxString basename( wxString(wxTheApp->argv[0]).AfterLast( wxChar('/')) );
            size_t pos = exeStr.find( wxT("/bin/") + basename );
            if (pos != wxString::npos)
                pathPtr->m_prefix.assign( exeStr, 0, pos );
        }
#endif // __LINUX__

        if (m_prefix.empty())
        {
#ifdef __VMS
	   pathPtr->m_prefix = wxT("/sys$system");
#else
	   pathPtr->m_prefix = wxT("/usr/local");
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

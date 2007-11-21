///////////////////////////////////////////////////////////////////////////////
// Name:        unix/stdpaths.cpp
// Purpose:     wxStandardPaths implementation for OS/2 systems
// Author:      Stefan Neis
// Modified by:
// Created:     2004-11-06
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Stefan Neis <Stefan.Neis@t-online.de>
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

#if wxUSE_STDPATHS

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif //WX_PRECOMP

#include "wx/filename.h"

#include "wx/stdpaths.h"


// ============================================================================
// wxStandardPaths implementation
// ============================================================================

// ----------------------------------------------------------------------------
// prefix management
// ----------------------------------------------------------------------------

wxString wxStandardPaths::m_prefix;

void wxStandardPaths::SetInstallPrefix(const wxString& prefix)
{
    m_prefix = prefix;
}

wxString wxStandardPaths::GetInstallPrefix() const
{
    if ( m_prefix.empty() )
    {
        wxStandardPaths *self = wx_const_cast(wxStandardPaths *, this);

        self->m_prefix = _T("/usr/local");
    }
    return m_prefix;
}

// ----------------------------------------------------------------------------
// public functions
// ----------------------------------------------------------------------------

wxString wxStandardPaths::GetConfigDir() const
{
    return m_prefix;
}

wxString wxStandardPaths::GetUserConfigDir() const
{
    return wxFileName::GetHomeDir();
}

wxString wxStandardPaths::GetDataDir() const
{
    return GetInstallPrefix() + _T("\\data");
}

wxString wxStandardPaths::GetUserDataDir() const
{
    return AppendAppName(wxFileName::GetHomeDir() + _T("\\."));
}

wxString wxStandardPaths::GetPluginsDir() const
{
    return wxString();
}

#endif // wxUSE_STDPATHS

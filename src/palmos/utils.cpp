/////////////////////////////////////////////////////////////////////////////
// Name:        palmos/utils.cpp
// Purpose:     Various utilities
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

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
#endif  //WX_PRECOMP

#include "wx/apptrait.h"
#include "wx/dynload.h"

#include "wx/confbase.h"        // for wxExpandEnvVars()

#include "wx/timer.h"

// VZ: there is some code using NetXXX() functions to get the full user name:
//     I don't think it's a good idea because they don't work under Win95 and
//     seem to return the same as wxGetUserId() under NT. If you really want
//     to use them, just #define USE_NET_API
#undef USE_NET_API

#ifdef USE_NET_API
    #include <lm.h>
#endif // USE_NET_API

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// In the WIN.INI file
static const wxChar WX_SECTION[] = wxT("wxWindows");
static const wxChar eUSERNAME[]  = wxT("UserName");

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// get host name and related
// ----------------------------------------------------------------------------

// Get hostname only (without domain name)
bool wxGetHostName(wxChar *buf, int maxSize)
{
    return false;
}

// get full hostname (with domain name if possible)
bool wxGetFullHostName(wxChar *buf, int maxSize)
{
    return false;
}

// Get user ID e.g. jacs
bool wxGetUserId(wxChar *buf, int maxSize)
{
    return false;
}

// Get user name e.g. Julian Smart
bool wxGetUserName(wxChar *buf, int maxSize)
{
    return false;
}

const wxChar* wxGetHomeDir(wxString *pstr)
{
    return NULL;
}

wxChar *wxGetUserHome(const wxString& WXUNUSED(user))
{
    return NULL;
}

bool wxDirExists(const wxString& dir)
{
    return false;
}

bool wxGetDiskSpace(const wxString& path, wxLongLong *pTotal, wxLongLong *pFree)
{
    return false;
}

// ----------------------------------------------------------------------------
// env vars
// ----------------------------------------------------------------------------

bool wxGetEnv(const wxString& var, wxString *value)
{
    return false;
}

bool wxSetEnv(const wxString& var, const wxChar *value)
{
    return false;
}

// ----------------------------------------------------------------------------
// process management
// ----------------------------------------------------------------------------

// structure used to pass parameters from wxKill() to wxEnumFindByPidProc()
int wxKill(long pid, wxSignal sig, wxKillError *krc)
{
    return 0;
}

// Execute a program in an Interactive Shell
bool wxShell(const wxString& command)
{
    return false;
}

// Shutdown or reboot the PC
bool wxShutdown(wxShutdownFlags wFlags)
{
    return false;
}

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
long wxGetFreeMemory()
{
    return 0;
}

unsigned long wxGetProcessId()
{
    return 0;
}

// Emit a beeeeeep
void wxBell()
{
}

wxString wxGetOsDescription()
{
    wxString str;

    return str;
}

wxToolkitInfo& wxAppTraits::GetToolkitInfo()
{
    static wxToolkitInfo info;
    info.name = _T("wxBase");
    return info;
}

// ----------------------------------------------------------------------------
// sleep functions
// ----------------------------------------------------------------------------

void wxMilliSleep(unsigned long milliseconds)
{
}

void wxMicroSleep(unsigned long microseconds)
{
}

void wxSleep(int nSecs)
{
}

// ----------------------------------------------------------------------------
// font encoding <-> Win32 codepage conversion functions
// ----------------------------------------------------------------------------

extern WXDLLIMPEXP_BASE long wxEncodingToCharset(wxFontEncoding encoding)
{
    return 0;
}

// we have 2 versions of wxCharsetToCodepage(): the old one which directly
// looks up the vlaues in the registry and the new one which is more
// politically correct and has more chances to work on other Windows versions
// as well but the old version is still needed for !wxUSE_FONTMAP case
#if wxUSE_FONTMAP

#include "wx/fontmap.h"

extern WXDLLIMPEXP_BASE long wxEncodingToCodepage(wxFontEncoding encoding)
{
    return 0;
}

extern long wxCharsetToCodepage(const wxChar *name)
{
    return 0;
}

#else // !wxUSE_FONTMAP

#include "wx/palmos/registry.h"

// this should work if Internet Exploiter is installed
extern long wxCharsetToCodepage(const wxChar *name)
{
    return 0;
}

#endif // wxUSE_FONTMAP/!wxUSE_FONTMAP


/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:     DOS implementations of utility functions
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001-2002 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/utils.h"
#include "wx/string.h"

#include "wx/intl.h"
#include "wx/apptrait.h"
#include "wx/log.h"
#include "wx/process.h"

#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

//----------------------------------------------------------------------------
// misc.
//----------------------------------------------------------------------------

void wxSleep(int nSecs)
{
    wxMilliSleep(1000 * nSecs);
}

void wxMilliSleep(unsigned long milliseconds)
{
#if HAVE_USLEEP || defined __DJGPP__
    usleep(milliseconds * 1000);
#elif defined __WATCOMC__
    delay(milliseconds);
#else
    clock_t start = clock();
    while ((clock() - start) * 1000 / CLOCKS_PER_SEC < (clock_t)milliseconds)
        ; // FIXME: need to yield here
#endif
}

void wxMicroSleep(unsigned long microseconds)
{
#if HAVE_USLEEP || defined __DJGPP__
    usleep(microseconds);
#else
    wxMilliSleep(microseconds/1000);
#endif
}

// Get Process ID
unsigned long wxGetProcessId()
{
    return (unsigned long)getpid();
}

bool wxGetEnv(const wxString& var, wxString *value)
{
    // wxGetenv is defined as getenv()
    wxChar *p = wxGetenv(var);
    if ( !p )
        return FALSE;

    if ( value )
        *value = p;

    return TRUE;
}

bool wxSetEnv(const wxString& variable, const wxChar *value)
{
    wxString s = variable;
    if ( value )
        s << _T('=') << value;

    // transform to ANSI
    const char *p = s.mb_str();

    // the string will be free()d by libc
    char *buf = (char *)malloc(strlen(p) + 1);
    strcpy(buf, p);

    return putenv(buf) == 0;
}

const wxChar* wxGetHomeDir(wxString *home)
{
    *home = wxT(".");
    return home->c_str();
}

const wxChar* wxGetUserHomeDir(wxString *home)
{
    *home = wxT(".");
    return home->c_str();
}

wxChar *wxGetUserHome(const wxString &user)
{
    return wxT(".");
}

#if WXWIN_COMPATIBILITY_2_2
void wxFatalError(const wxString &msg, const wxString &title)
{
    wxFprintf( stderr, _("Error ") );
    if (!title.IsNull()) wxFprintf( stderr, wxT("%s "), WXSTRINGCAST(title) );
    if (!msg.IsNull()) wxFprintf( stderr, wxT(": %s"), WXSTRINGCAST(msg) );
    wxFprintf( stderr, wxT(".\n") );
    exit(3); // the same exit code as for abort()
}
#endif // WXWIN_COMPATIBILITY_2_2

bool wxGetUserId(wxChar *WXUNUSED(buf), int WXUNUSED(sz))
{
    wxFAIL_MSG( wxT("wxGetUserId not implemented under MS-DOS!") );
    return FALSE;
}

bool wxGetUserName(wxChar *WXUNUSED(buf), int WXUNUSED(sz))
{
    wxFAIL_MSG( wxT("wxGetUserName not implemented under MS-DOS!") );
    return FALSE;
}

bool wxGetHostName(wxChar *WXUNUSED(buf), int WXUNUSED(sz))
{
    wxFAIL_MSG( wxT("wxGetHostName not implemented under MS-DOS!") );
    return FALSE;
}

bool wxGetFullHostName(wxChar *WXUNUSED(buf), int WXUNUSED(sz))
{
    wxFAIL_MSG( wxT("wxGetFullHostName not implemented under MS-DOS!") );
    return FALSE;
}

int wxKill(long WXUNUSED(pid), wxSignal WXUNUSED(sig), wxKillError *WXUNUSED(rc), int WXUNUSED(flags))
{
    wxFAIL_MSG( wxT("wxKill not implemented under MS-DOS!") );
    return 0;
}

long wxExecute(const wxString& WXUNUSED(command), int WXUNUSED(flags), wxProcess *WXUNUSED(process))
{
    wxFAIL_MSG( wxT("wxExecute not implemented under MS-DOS!") );
    return 0;
}

long wxExecute(char **WXUNUSED(argv), int WXUNUSED(flags), wxProcess *WXUNUSED(process))
{
    wxFAIL_MSG( wxT("wxExecute not implemented under MS-DOS!") );
    return 0;
}

wxToolkitInfo& wxConsoleAppTraits::GetToolkitInfo()
{
    static wxToolkitInfo info;
    info.versionMajor = -1; // FIXME
    info.versionMinor = -1;
    info.name = _T("wxBase");
    info.os = wxDOS;
    return info;
}

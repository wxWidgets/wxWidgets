/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:
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
#include <mgraph.hpp>

#ifdef __UNIX__
#include "wx/unix/execute.h"
#endif

#include "wx/mgl/private.h"

//----------------------------------------------------------------------------
// misc.
//----------------------------------------------------------------------------

void wxBell()
{
    // FIXME_MGL
}


#ifdef __DOS__
// VS: this should be in utilsdos.cpp, but since there will hardly ever
//     be a non-MGL MS-DOS port...

void wxSleep(int nSecs)
{
    wxUsleep(1000 * nSecs);
}

void wxUsleep(unsigned long milliseconds)
{
    PM_sleep(milliseconds);
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

#if wxUSE_UNICODE
const wxMB2WXbuf wxGetUserHome(const wxString &user)
#else // just for binary compatibility -- there is no 'const' here
wxChar *wxGetUserHome(const wxString &user)
#endif
{
    return wxT(".");
}

void wxFatalError(const wxString &msg, const wxString &title)
{
    if (!title.IsNull()) 
        wxFprintf( stderr, wxT("%s "), WXSTRINGCAST(title));
    PM_fatalError(msg.c_str());
}

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

int wxKill(long WXUNUSED(pid), wxSignal WXUNUSED(sig), wxKillError *WXUNUSED(rc))
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


#endif

// ----------------------------------------------------------------------------
// display characterstics
// ----------------------------------------------------------------------------

void wxDisplaySize(int *width, int *height)
{
   wxASSERT_MSG( g_displayDC, wxT("You must call wxApp::SetDisplayMode before using this function") );
    if (width) *width = g_displayDC->sizex()+1;
    if (height) *height = g_displayDC->sizey()+1;
}

void wxDisplaySizeMM(int *width, int *height)
{
    wxASSERT_MSG( g_displayDC, wxT("You must call wxApp::SetDisplayMode before using this function") );

    int xDPI, yDPI;
    MGL_getDotsPerInch(&xDPI, &yDPI);    
    
    if ( width ) 
        *width = (int)((g_displayDC->sizex()+1) * 25.4 / xDPI);
    if ( height ) 
        *height = (int)((g_displayDC->sizey()+1) * 25.4 / yDPI);
}

void wxClientDisplayRect(int *x, int *y, int *width, int *height)
{
    if ( x ) *x = 0;
    if ( y ) *y = 0;
    wxDisplaySize(width, height);
    // FIXME_MGL - windowed version needs different handling
}

bool wxColourDisplay()
{
    wxASSERT_MSG( g_displayDC, wxT("You must call wxApp::SetDisplayMode before using this function") );
    
    return (wxDisplayDepth() > 1);
}

int wxDisplayDepth()
{
    wxASSERT_MSG( g_displayDC, wxT("You must call wxApp::SetDisplayMode before using this function") );

    return g_displayDC->getBitsPerPixel();
}

#if wxUSE_GUI

wxToolkitInfo& wxGUIAppTraits::GetToolkitInfo()
{
    static wxToolkitInfo info;
    info.shortName = _T("mgluniv");
    info.name = _T("wxMGL");
    info.versionMajor = MGL_RELEASE_MAJOR;
    info.versionMinor = MGL_RELEASE_MINOR;
    info.os = wxGTK;
#if defined(__UNIX__)
    info.os = wxMGL_UNIX;
#elif defined(__OS2__)
    info.os = wxMGL_OS2;
#elif defined(__WIN32__)
    info.os = wxMGL_WIN32;
#elif defined(__DOS__)
    info.os = wxMGL_DOS;
#else
    #error Platform not supported by wxMGL!
#endif
    return info;
}

#endif

wxToolkitInfo& wxConsoleAppTraits::GetToolkitInfo()
{
    static wxToolkitInfo info;
    info.shortName = _T("mglbase");
    info.versionMajor = MGL_RELEASE_MAJOR;
    info.versionMinor = MGL_RELEASE_MINOR;
    info.name = _T("wxBase");
    info.os = wxGTK;
#if defined(__UNIX__)
    info.os = wxMGL_UNIX;
#elif defined(__OS2__)
    info.os = wxMGL_OS2;
#elif defined(__WIN32__)
    info.os = wxMGL_WIN32;
#elif defined(__DOS__)
    info.os = wxMGL_DOS;
#else
    #error Platform not supported by wxMGL!
#endif
    return info;
}

void wxGetMousePosition(int* x, int* y)
{
    MS_getPos(x, y);
}

wxPoint wxGetMousePosition()
{
    wxPoint pt;
    wxGetMousePosition(&pt.x, &pt.y);
    return pt;
}



#ifdef __UNIX__

int wxAddProcessCallback(wxEndProcessData *proc_data, int fd)
{
    wxFAIL_MSG(wxT("wxAddProcessCallback not implemented in wxMGL!"));
    return 0;
#if 0 // FIXME_MGL -do we need it at all?
    int tag = gdk_input_add(fd,
                            GDK_INPUT_READ,
                            GTK_EndProcessDetector,
                            (gpointer)proc_data);

    return tag;
#endif
}

#endif

/////////////////////////////////////////////////////////////////////////////
// Name:        utilsunx.cpp
// Purpose:     generic Unix implementation of many wx functions
// Author:      Vadim Zeitlin
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"
#include "wx/string.h"

#include "wx/intl.h"
#include "wx/log.h"

#include "wx/utils.h"
#include "wx/process.h"

#include "wx/unix/execute.h"

#include <stdarg.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pwd.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>          // for O_WRONLY and friends
#include <time.h>           // nanosleep() and/or usleep()
#include <ctype.h>          // isspace()

// JACS: needed for FD_SETSIZE
#include <sys/time.h>

#if HAVE_UNAME
    #include <sys/utsname.h> // for uname()
#endif // HAVE_UNAME

// ----------------------------------------------------------------------------
// conditional compilation
// ----------------------------------------------------------------------------

// many versions of Unices have this function, but it is not defined in system
// headers - please add your system here if it is the case for your OS.
// SunOS < 5.6 (i.e. Solaris < 2.6) and DG-UX are like this.
#if !defined(HAVE_USLEEP) && \
    (defined(__SUN__) && !defined(__SunOs_5_6) && \
                         !defined(__SunOs_5_7) && !defined(__SUNPRO_CC)) || \
     defined(__osf__)
    extern "C"
    {
        #ifdef __SUN__
            int usleep(unsigned int usec);
        #else // !Sun
            void usleep(unsigned long usec);
        #endif // Sun/!Sun
    };
#endif // Unices without usleep()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// sleeping
// ----------------------------------------------------------------------------

void wxSleep(int nSecs)
{
    sleep(nSecs);
}

void wxUsleep(unsigned long milliseconds)
{
#if HAVE_NANOSLEEP
    timespec tmReq;
    tmReq.tv_sec = milliseconds / 1000;
    tmReq.tv_nsec = (milliseconds % 1000) * 1000 * 1000;

    // we're not interested in remaining time nor in return value
    (void)nanosleep(&tmReq, (timespec *)NULL);
#elif HAVE_USLEEP
    // uncomment this if you feel brave or if you are sure that your version
    // of Solaris has a safe usleep() function but please notice that usleep()
    // is known to lead to crashes in MT programs in Solaris 2.[67] and is not
    // documented as MT-Safe
    #if defined(__SUN__) && wxUSE_THREADS
        #error "usleep() cannot be used in MT programs under Solaris."
    #endif // Sun

    usleep(milliseconds * 1000); // usleep(3) wants microseconds
#else // !sleep function
    #error "usleep() or nanosleep() function required for wxUsleep"
#endif // sleep function
}

// ----------------------------------------------------------------------------
// process management
// ----------------------------------------------------------------------------

int wxKill(long pid, wxSignal sig)
{
    return kill(pid, (int)sig);
}

#define WXEXECUTE_NARGS   127

long wxExecute( const wxString& command, bool sync, wxProcess *process )
{
    wxCHECK_MSG( !command.IsEmpty(), 0, _T("can't exec empty command") );

    int argc = 0;
    wxChar *argv[WXEXECUTE_NARGS];
    wxString argument;
    const wxChar *cptr = command.c_str();
    wxChar quotechar = _T('\0'); // is arg quoted?
    bool escaped = FALSE;

    // split the command line in arguments
    do
    {
        argument=_T("");
        quotechar = _T('\0');

        // eat leading whitespace:
        while ( wxIsspace(*cptr) )
            cptr++;

        if ( *cptr == _T('\'') || *cptr == _T('"') )
            quotechar = *cptr++;

        do
        {
            if ( *cptr == _T('\\') && ! escaped )
            {
                escaped = TRUE;
                cptr++;
                continue;
            }

            // all other characters:
            argument += *cptr++;
            escaped = FALSE;

            // have we reached the end of the argument?
            if ( (*cptr == quotechar && ! escaped)
                 || (quotechar == _T('\0') && wxIsspace(*cptr))
                 || *cptr == _T('\0') )
            {
                wxASSERT_MSG( argc < WXEXECUTE_NARGS,
                              _T("too many arguments in wxExecute") );

                argv[argc] = new wxChar[argument.length() + 1];
                wxStrcpy(argv[argc], argument.c_str());
                argc++;

                // if not at end of buffer, swallow last character:
                if(*cptr)
                    cptr++;

                break; // done with this one, start over
            }
        } while(*cptr);
    } while(*cptr);
    argv[argc] = NULL;

    // do execute the command
    long lRc = wxExecute(argv, sync, process);

    // clean up
    argc = 0;
    while( argv[argc] )
        delete [] argv[argc++];

    return lRc;
}

bool wxShell(const wxString& command)
{
    wxString cmd;
    if ( !!command )
        cmd.Printf(_T("xterm -e %s"), command.c_str());
    else
        cmd = command;

    return wxExecute(cmd) != 0;
}

void wxHandleProcessTermination(wxEndProcessData *proc_data)
{
    int pid = (proc_data->pid > 0) ? proc_data->pid : -(proc_data->pid);

    // waitpid is POSIX so should be available everywhere, however on older
    // systems wait() might be used instead in a loop (until the right pid
    // terminates)
    int status = 0;
    if ( waitpid(pid, &status, 0) == -1 || !WIFEXITED(status) )
    {
        wxLogSysError(_("Waiting for subprocess termination failed"));
    }
    else
    {
        // notify user about termination if required
        if (proc_data->process)
        {
            proc_data->process->OnTerminate(proc_data->pid,
                                            WEXITSTATUS(status));
        }
    }

    // clean up
    if ( proc_data->pid > 0 )
    {
        delete proc_data;
    }
    else
    {
        // wxExecute() will know about it
        proc_data->exitcode = status;

        proc_data->pid = 0;
    }
}

long wxExecute( wxChar **argv, bool sync, wxProcess *process )
{
    wxCHECK_MSG( *argv, 0, _T("can't exec empty command") );

    int end_proc_detect[2];
#if wxUSE_UNICODE
    int mb_argc = 0;
    char *mb_argv[WXEXECUTE_NARGS];

    while (argv[mb_argc]) {
      wxWX2MBbuf mb_arg = wxConv_libc.cWX2MB(argv[mb_argc]);
      mb_argv[mb_argc] = strdup(mb_arg);
      mb_argc++;
    }
    mb_argv[mb_argc] = (char *) NULL;
#else
    wxChar **mb_argv = argv;
#endif

    // create pipes
    if (pipe(end_proc_detect) == -1)
    {
        wxLogSysError( _("Pipe creation failed") );
        return 0;
    }

    // fork the process
#ifdef HAVE_VFORK
    pid_t pid = vfork();
#else
    pid_t pid = fork();
#endif
    if (pid == -1)
    {
        wxLogSysError( _("Fork failed") );
        return 0;
    }
    else if (pid == 0)
    {
        // we're in child
        close(end_proc_detect[0]); // close reading side

        // These three lines close the open file descriptors to to avoid any
        // input/output which might block the process or irritate the user. If
        // one wants proper IO for the subprocess, the right thing to do is
        // to start an xterm executing it.
        if (sync == 0)
        {
            // leave stderr opened, it won't do any hurm
            for ( int fd = 0; fd < FD_SETSIZE; fd++ )
            {
                if ( fd != end_proc_detect[1] && fd != STDERR_FILENO )
                    close(fd);
            }
        }

#if 0
        close(STDERR_FILENO);

        // some programs complain about stderr not being open, so redirect
        // them:
        open("/dev/null", O_RDONLY);  // stdin
        open("/dev/null", O_WRONLY);  // stdout
        open("/dev/null", O_WRONLY);  // stderr
#endif

        execvp (*mb_argv, mb_argv);

        // there is no return after successful exec()
        wxFprintf(stderr, _("Can't execute '%s'\n"), *argv);

        _exit(-1);
    }
    else
    {
        // we're in parent
        close(end_proc_detect[1]); // close writing side

        wxEndProcessData *data = new wxEndProcessData;
        data->tag = wxAddProcessCallback(data, end_proc_detect[0]);

#if wxUSE_UNICODE
	mb_argc = 0;
	while (mb_argv[mb_argc])
	  free(mb_argv[mb_argc++]);
#endif

        if ( sync )
        {
            wxASSERT_MSG( !process, _T("wxProcess param ignored for sync exec") );
            data->process = NULL;

            // sync execution: indicate it by negating the pid
            data->pid = -pid;

            // it will be set to 0 from GTK_EndProcessDetector
            while (data->pid != 0)
                wxYield();

            int exitcode = data->exitcode;

            delete data;

            return exitcode;
        }
        else
        {
            // async execution, nothing special to do - caller will be
            // notified about the process terminationif process != NULL, data
            // will be deleted in GTK_EndProcessDetector
            data->process = process;
            data->pid = pid;

            return pid;
        }
    }
}

// ----------------------------------------------------------------------------
// file and directory functions
// ----------------------------------------------------------------------------

const wxChar* wxGetHomeDir( wxString *home  )
{
    *home = wxGetUserHome( wxString() );
    if ( home->IsEmpty() )
        *home = _T("/");

    return home->c_str();
}

#if wxUSE_UNICODE
const wxMB2WXbuf wxGetUserHome( const wxString &user )
#else // just for binary compatibility
char *wxGetUserHome( const wxString &user )
#endif
{
    struct passwd *who = (struct passwd *) NULL;

    if ( !user )
    {
        register wxChar *ptr;

        if ((ptr = wxGetenv(_T("HOME"))) != NULL)
        {
            return ptr;
        }
        if ((ptr = wxGetenv(_T("USER"))) != NULL || (ptr = wxGetenv(_T("LOGNAME"))) != NULL)
        {
            who = getpwnam(wxConv_libc.cWX2MB(ptr));
        }

        // We now make sure the the user exists!
        if (who == NULL)
        {
            who = getpwuid(getuid());
        }
    }
    else
    {
      who = getpwnam (user.mb_str());
    }

#if wxUSE_UNICODE
    return who ? wxConv_libc.cMB2WX(who->pw_dir) : (wxMB2WXbuf)((wxChar*)NULL);
#else
    return who ? who->pw_dir : ((char*)NULL);
#endif
}

// ----------------------------------------------------------------------------
// network and user id routines
// ----------------------------------------------------------------------------

// retrieve either the hostname or FQDN depending on platform (caller must
// check whether it's one or the other, this is why this function is for
// private use only)
static bool wxGetHostNameInternal(wxChar *buf, int sz)
{
    wxCHECK_MSG( buf, FALSE, _T("NULL pointer in wxGetHostNameInternal") );

    *buf = _T('\0');

    // we're using uname() which is POSIX instead of less standard sysinfo()
#if defined(HAVE_UNAME)
    struct utsname uts;
    bool ok = uname(&uts) != -1;
    if ( ok )
    {
        wxStrncpy(buf, wxConv_libc.cMB2WX(uts.nodename), sz - 1);
        buf[sz] = _T('\0');
    }
#elif defined(HAVE_GETHOSTNAME)
    bool ok = gethostname(buf, sz) != -1;
#else // no uname, no gethostname
    wxFAIL_MSG(_T("don't know host name for this machine"));

    bool ok = FALSE;
#endif // uname/gethostname

    if ( !ok )
    {
        wxLogSysError(_("Cannot get the hostname"));
    }

    return ok;
}

bool wxGetHostName(wxChar *buf, int sz)
{
    bool ok = wxGetHostNameInternal(buf, sz);

    if ( ok )
    {
        // BSD systems return the FQDN, we only want the hostname, so extract
        // it (we consider that dots are domain separators)
        wxChar *dot = wxStrchr(buf, _T('.'));
        if ( dot )
        {
            // nuke it
            *dot = _T('\0');
        }
    }

    return ok;
}

bool wxGetFullHostName(wxChar *buf, int sz)
{
    bool ok = wxGetHostNameInternal(buf, sz);

    if ( ok )
    {
        if ( !wxStrchr(buf, _T('.')) )
        {
            struct hostent *host = gethostbyname(wxConv_libc.cWX2MB(buf));
            if ( !host )
            {
                wxLogSysError(_("Cannot get the official hostname"));

                ok = FALSE;
            }
            else
            {
                // the canonical name
                wxStrncpy(buf, wxConv_libc.cMB2WX(host->h_name), sz);
            }
        }
        //else: it's already a FQDN (BSD behaves this way)
    }

    return ok;
}

bool wxGetUserId(wxChar *buf, int sz)
{
    struct passwd *who;

    *buf = _T('\0');
    if ((who = getpwuid(getuid ())) != NULL)
    {
        wxStrncpy (buf, wxConv_libc.cMB2WX(who->pw_name), sz - 1);
        return TRUE;
    }

    return FALSE;
}

bool wxGetUserName(wxChar *buf, int sz)
{
    struct passwd *who;
    char *comma;

    *buf = _T('\0');
    if ((who = getpwuid (getuid ())) != NULL) {
       comma = strchr(who->pw_gecos, ',');
       if (comma)
           *comma = '\0'; // cut off non-name comment fields
       wxStrncpy (buf, wxConv_libc.cMB2WX(who->pw_gecos), sz - 1);
       return TRUE;
    }

    return FALSE;
}

// ----------------------------------------------------------------------------
// error and debug output routines (deprecated, use wxLog)
// ----------------------------------------------------------------------------

void wxDebugMsg( const char *format, ... )
{
  va_list ap;
  va_start( ap, format );
  vfprintf( stderr, format, ap );
  fflush( stderr );
  va_end(ap);
}

void wxError( const wxString &msg, const wxString &title )
{
  wxFprintf( stderr, _("Error ") );
  if (!title.IsNull()) wxFprintf( stderr, _T("%s "), WXSTRINGCAST(title) );
  if (!msg.IsNull()) wxFprintf( stderr, _T(": %s"), WXSTRINGCAST(msg) );
  wxFprintf( stderr, _T(".\n") );
}

void wxFatalError( const wxString &msg, const wxString &title )
{
  wxFprintf( stderr, _("Error ") );
  if (!title.IsNull()) wxFprintf( stderr, _T("%s "), WXSTRINGCAST(title) );
  if (!msg.IsNull()) wxFprintf( stderr, _T(": %s"), WXSTRINGCAST(msg) );
  wxFprintf( stderr, _T(".\n") );
  exit(3); // the same exit code as for abort()
}

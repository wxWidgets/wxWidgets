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
#ifdef HAVE_UNAME
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

// many versions of Unices have this function, but it is not defined in system
// headers - please add your system here if it is the case for your OS.
// SunOS (and Solaris) and DG-UX are like this.
#ifdef HAVE_WAIT4
  #if defined(__SOLARIS__) || defined(__osf__)
    extern "C"
    {
        pid_t wait4(pid_t pid, int *statusp, int options,
                    struct rusage *rusage);
    }
  #endif

  #define wxWait4(pid, stat, flags, rusage) wait4(pid, stat, flags, rusage)
#else
    // no wait4() at all on these systems
    // TODO verify whether wait3() really works in this situation
    #define wxWait4(pid, stat, flags, rusage) wait3(stat, flags, rusage)
#endif // HAVE_WAIT4

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
#if defined(HAVE_NANOSLEEP)
    timespec tmReq;
    tmReq.tv_sec = milliseconds / 1000;
    tmReq.tv_nsec = (milliseconds % 1000) * 1000 * 1000;

    // we're not interested in remaining time nor in return value
    (void)nanosleep(&tmReq, (timespec *)NULL);
#elif defined(HAVE_USLEEP)
    // uncomment this if you feel brave or if you are sure that your version
    // of Solaris has a safe usleep() function but please notice that usleep()
    // is known to lead to crashes in MT programs in Solaris 2.[67] and is not
    // documented as MT-Safe
    #if defined(__SUN__) && defined(wxUSE_THREADS)
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

int wxKill(long pid, int sig)
{
    return kill(pid, sig);
}

#define WXEXECUTE_NARGS   127

long wxExecute( const wxString& command, bool sync, wxProcess *process )
{
    wxCHECK_MSG( !command.IsEmpty(), 0, "can't exec empty command" );

    int argc = 0;
    char *argv[WXEXECUTE_NARGS];
    wxString argument;
    const char *cptr = command.c_str();
    char quotechar = '\0'; // is arg quoted?
    bool escaped = FALSE;

    do
    {
        argument="";
        quotechar = '\0';
        // eat leading whitespace:
        while(*cptr && isspace(*cptr))
            cptr++;
        if(*cptr == '\'' || *cptr == '"')
            quotechar = *cptr++;
        do
        {
            if(*cptr == '\\' && ! escaped)
            {
                escaped = TRUE;
                cptr++;
                continue;
            }
            // all other characters:
            argument += *cptr ++;
            escaped = FALSE;
            // Have we reached the end of the argument?
            if((*cptr == quotechar && ! escaped)
               || (quotechar == '\0' && isspace(*cptr))
               || *cptr == '\0')
            {
                wxASSERT(argc < WXEXECUTE_NARGS);
                argv[argc] = new char[argument.Len()+1];
                strcpy(argv[argc], argument.c_str());
                argc++;
                // if not at end of buffer, swallow last character:
                if(*cptr) cptr++;
                break; // done with this one, start over
            }
        }while(*cptr);
    }while(*cptr);
    argv[argc] = NULL;
    
    long lRc = wxExecute(argv, sync, process);

    argc = 0;
    while(argv[argc])
        delete [] argv[argc++];

    return lRc;
}

bool wxShell(const wxString& command)
{
    wxString cmd;
    if ( !!command )
        cmd.Printf("xterm -e %s", command.c_str());
    else
        cmd = command;

    return wxExecute(cmd) != 0;
}

void wxHandleProcessTermination(wxEndProcessData *proc_data)
{
    int pid = (proc_data->pid > 0) ? proc_data->pid : -(proc_data->pid);

    int status = 0;
    wxWait4(pid, &status, 0, (rusage *) NULL);

    if (proc_data->process)
        proc_data->process->OnTerminate(proc_data->pid, status);

    if (proc_data->pid > 0)
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

long wxExecute( char **argv, bool sync, wxProcess *process )
{
    wxCHECK_MSG( *argv, 0, "can't exec empty command" );

    int end_proc_detect[2];

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
        // one wants proper IO for the subprocess, the "right thing to do is
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

        execvp (*argv, argv);

        // there is no return after successful exec()
        fprintf(stderr, _("Can't execute '%s'\n"), *argv);

        _exit(-1);
    }
    else
    {
        // we're in parent
        close(end_proc_detect[1]); // close writing side

        wxEndProcessData *data = new wxEndProcessData;
        data->tag = wxAddProcessCallback(data, end_proc_detect[0]);

        if ( sync )
        {
            wxASSERT_MSG( !process, "wxProcess param ignored for sync exec" );
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

const char* wxGetHomeDir( wxString *home  )
{
    *home = wxGetUserHome( wxString() );
    if ( home->IsEmpty() )
        *home = "/";

    return home->c_str();
}

char *wxGetUserHome( const wxString &user )
{
    struct passwd *who = (struct passwd *) NULL;

    if (user.IsNull() || (user== ""))
    {
        register char *ptr;

        if ((ptr = getenv("HOME")) != NULL)
        {
            return ptr;
        }
        if ((ptr = getenv("USER")) != NULL || (ptr = getenv("LOGNAME")) != NULL)
        {
            who = getpwnam(ptr);
        }

        // We now make sure the the user exists!
        if (who == NULL)
        {
            who = getpwuid(getuid());
        }
    }
    else
    {
      who = getpwnam (user);
    }

    return who ? who->pw_dir : (char*)NULL;
}

// ----------------------------------------------------------------------------
// id routines
// ----------------------------------------------------------------------------

bool wxGetHostName(char *buf, int sz)
{
    wxCHECK_MSG( buf, FALSE, "NULL pointer in wxGetHostName" );

    *buf = '\0';

    // we're using uname() which is POSIX instead of less standard sysinfo()
#if defined(HAVE_UNAME)
    struct utsname uts;
    bool ok = uname(&uts) != -1;
    if ( ok )
    {
        strncpy(buf, uts.nodename, sz - 1);
        buf[sz] = '\0';
    }
#elif defined(HAVE_GETHOSTNAME)
    bool ok = gethostname(buf, sz) != -1;
#else
    wxFAIL_MSG("don't know host name for this machibe");

    bool ok = FALSE;
#endif

    if ( !ok )
    {
        wxLogSysError(_("Cannot get the hostname"));
    }

    return ok;
}

bool wxGetUserId(char *buf, int sz)
{
    struct passwd *who;

    *buf = '\0';
    if ((who = getpwuid(getuid ())) != NULL)
    {
        strncpy (buf, who->pw_name, sz - 1);
        return TRUE;
    }

    return FALSE;
}

bool wxGetUserName(char *buf, int sz)
{
    struct passwd *who;
    char *comma;

    *buf = '\0';
    if ((who = getpwuid (getuid ())) != NULL) {
       comma = strchr(who->pw_gecos, ',');
       if (comma)
           *comma = '\0'; // cut off non-name comment fields
       strncpy (buf, who->pw_gecos, sz - 1);
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
  fprintf( stderr, _("Error ") );
  if (!title.IsNull()) fprintf( stderr, "%s ", WXSTRINGCAST(title) );
  if (!msg.IsNull()) fprintf( stderr, ": %s", WXSTRINGCAST(msg) );
  fprintf( stderr, ".\n" );
}

void wxFatalError( const wxString &msg, const wxString &title )
{
  fprintf( stderr, _("Error ") );
  if (!title.IsNull()) fprintf( stderr, "%s ", WXSTRINGCAST(title) );
  if (!msg.IsNull()) fprintf( stderr, ": %s", WXSTRINGCAST(msg) );
  fprintf( stderr, ".\n" );
  exit(3); // the same exit code as for abort()
}

//------------------------------------------------------------------------
// directory routines
//------------------------------------------------------------------------

bool wxDirExists( const wxString& dir )
{
    char buf[500];
    strcpy( buf, WXSTRINGCAST(dir) );
    struct stat sbuf;
    return ((stat(buf, &sbuf) != -1) && S_ISDIR(sbuf.st_mode) ? TRUE : FALSE);
}

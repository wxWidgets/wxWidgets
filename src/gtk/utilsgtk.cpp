/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:           wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/utils.h"
#include "wx/string.h"

#include "wx/intl.h"
#include "wx/log.h"

#include "wx/process.h"

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

#include <glib.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gtk/gtkfeatures.h>
#include <gdk/gdkx.h>

#ifdef __SVR4__
  #include <sys/systeminfo.h>
#endif

// many versions of Unices have this function, but it is not defined in system
// headers - please add your system here if it is the case for your OS.
// SunOS < 5.6 (i.e. Solaris < 2.6) and DG-UX are like this.
#if (defined(__SUN__) && !defined(__SunOs_5_6) && \
                         !defined(__SunOs_5_7) && !defined(__SUNPRO_CC)) || \
     defined(__osf__)
    extern "C"
    {
        void usleep(unsigned long usec);
    };
#endif // Unices without usleep()

// many versions of Unices have this function, but it is not defined in system
// headers - please add your system here if it is the case for your OS.
// SunOS (and Solaris) and DG-UX are like this.
#if defined(__SOLARIS__) || defined(__osf__)
    extern "C"
    {
        pid_t wait4(pid_t pid, int *statusp, int options,
                    struct rusage *rusage);
    }

    #define wxWait4(pid, stat, flags, rusage) wait4(pid, stat, flags, rusage)
#elif defined(__sgi) || defined(__HPUX__)
    // no wait4() at all on these systems
    // TODO verify whether wait3() really works in this situation
    #define wxWait4(pid, stat, flags, rusage) wait3(stat, flags, rusage)
#else
    // other Unices: assume have wait4(), although it's not standard (but
    // Linux and FreeBSD do have it)
    #define wxWait4(pid, stat, flags, rusage) wait4(pid, stat, flags, rusage)
#endif // wait4()

//------------------------------------------------------------------------
// misc.
//------------------------------------------------------------------------

void wxBell(void)
{
    gdk_beep();
}

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

int wxKill(long pid, int sig)
{
    return kill(pid, sig);
}

void wxDisplaySize( int *width, int *height )
{
    if (width) *width = gdk_screen_width();
    if (height) *height = gdk_screen_height();
}

void wxGetMousePosition( int* x, int* y )
{
    gdk_window_get_pointer( (GdkWindow*) NULL, x, y, (GdkModifierType*) NULL );
}

bool wxColourDisplay(void)
{
    return TRUE;
}

int wxDisplayDepth(void)
{
    return gdk_window_get_visual( (GdkWindow*) &gdk_root_parent )->depth;
}

int wxGetOsVersion(int *majorVsn, int *minorVsn)
{
  if (majorVsn) *majorVsn = GTK_MAJOR_VERSION;
  if (minorVsn) *minorVsn = GTK_MINOR_VERSION;

  return wxGTK;
}

//------------------------------------------------------------------------
// user and home routines
//------------------------------------------------------------------------

const char* wxGetHomeDir( wxString *home  )
{
    *home = wxGetUserHome( wxString() );
    if (home->IsNull()) *home = "/";
    return *home;
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

        /* We now make sure the the user exists! */
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

//------------------------------------------------------------------------
// id routines
//------------------------------------------------------------------------

bool wxGetHostName(char *buf, int sz)
{
    *buf = '\0';
#if defined(__SVR4__) && !defined(__sgi)
    //KB: does this return the fully qualified host.domain name?
    return (sysinfo(SI_HOSTNAME, buf, sz) != -1);
#else /* BSD Sockets */
    char name[255], domain[255];
    //struct hostent *h;
    // Get hostname
    if (gethostname(name, sizeof(name)/sizeof(char)-1) == -1)
        return FALSE;
    if (getdomainname(domain, sizeof(domain)/sizeof(char)-1) == -1)
        return FALSE;
    // Get official full name of host
    // doesn't return the full qualified name, replaced by following
    // code (KB)
    // strncpy(buf, (h=gethostbyname(name))!=NULL ? h->h_name : name, sz-1);
    if((unsigned)sz > strlen(name)+strlen(domain)+1)
    {
        strcpy(buf, name);
        if(strcmp(domain,"(none)") == 0) // standalone machine
        {
            strcat(buf,".");
            strcat(buf,domain);
        }
    }
    else
        return FALSE;
    return TRUE;
#endif
}

bool wxGetUserId(char *buf, int sz)
{
    struct passwd *who;

    *buf = '\0';
    if ((who = getpwuid(getuid ())) != NULL) {
        strncpy (buf, who->pw_name, sz-1);
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
       comma = strchr(who->pw_gecos,'c');
       if(comma) *comma = '\0'; // cut off non-name comment fields
       strncpy (buf, who->pw_gecos, sz - 1);
        return TRUE;
    }
    return FALSE;
}

//------------------------------------------------------------------------
// error and debug output routines
//------------------------------------------------------------------------

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
  fprintf( stderr, "Error " );
  if (!title.IsNull()) fprintf( stderr, "%s ", WXSTRINGCAST(title) );
  if (!msg.IsNull()) fprintf( stderr, ": %s", WXSTRINGCAST(msg) );
  fprintf( stderr, ".\n" );
}

void wxFatalError( const wxString &msg, const wxString &title )
{
  fprintf( stderr, "Error " );
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

//------------------------------------------------------------------------
// subprocess routines
//------------------------------------------------------------------------

// if pid > 0, the execution is async and the data is freed in
// GTK_EndProcessDetector, if pid < 0, the execution is synchronous and the
// caller (wxExecute) frees the data
struct wxEndProcessData
{
    gint pid, tag;
    wxProcess *process;
    int  exitcode;
};

static void GTK_EndProcessDetector(gpointer data, gint source,
                                   GdkInputCondition WXUNUSED(condition) )
{
    wxEndProcessData *proc_data = (wxEndProcessData *)data;
    int pid;

    pid = (proc_data->pid > 0) ? proc_data->pid : -(proc_data->pid);

    int status = 0;
    wxWait4(pid, &status, 0, (rusage *) NULL);

    close(source);
    gdk_input_remove(proc_data->tag);

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
    wxEndProcessData *data = new wxEndProcessData;
    int end_proc_detect[2];

    wxCHECK_MSG( *argv, 0, "can't exec empty command" );

    /* Create pipes */
    if (pipe(end_proc_detect) == -1)
    {
        wxLogSysError( _("Pipe creation failed") );
        return 0;
    }

    /* fork the process */
#if defined(sun) || defined(__ultrix) || defined(__bsdi__)
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
        close(STDIN_FILENO);
        close(STDOUT_FILENO);

        // leave stderr opened, it won't do any hurm
#if 0
        close(STDERR_FILENO);

        // some programs complain about stderr not being open, so redirect
        // them:
        open("/dev/null", O_RDONLY);  // stdin
        open("/dev/null", O_WRONLY);  // stdout
        open("/dev/null", O_WRONLY);  // stderr
#endif

#ifdef _AIX
        execvp ((const char *)*argv, (const char **)argv);
#else
        execvp (*argv, argv);
#endif

        // there is no return after successful exec()
        fprintf(stderr, _("Can't execute '%s'\n"), *argv);

        _exit(-1);
    }
    else
    {
        // we're in parent
        close(end_proc_detect[1]); // close writing side
        data->tag = gdk_input_add(end_proc_detect[0], GDK_INPUT_READ,
                                  GTK_EndProcessDetector, (gpointer)data);
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

long wxExecute( const wxString& command, bool sync, wxProcess *process )
{
    static const char *IFS = " \t\n";

    wxCHECK_MSG( !command.IsEmpty(), 0, "can't exec empty command" );

    int argc = 0;
    char *argv[127];
    char *tmp = new char[command.Len() + 1];
    strcpy(tmp, command);

    argv[argc++] = strtok(tmp, IFS);
    while ((argv[argc++] = strtok((char *) NULL, IFS)) != NULL)
        /* loop */ ;

    long lRc = wxExecute(argv, sync, process);

    delete [] tmp;

    return lRc;
}

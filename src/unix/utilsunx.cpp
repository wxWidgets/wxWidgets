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
     defined(__osf__) || defined(__EMX__)
    extern "C"
    {
        #ifdef __SUN__
            int usleep(unsigned int usec);
        #else // !Sun
        #ifdef __EMX
            /* I copied this from the XFree86 diffs. AV. */
            #define INCL_DOSPROCESS
            #include <os2.h>
            void usleep(unsigned long delay)
            {
                DosSleep(delay ? (delay/1000l) : 1l);
            }
        #else
            void usleep(unsigned long usec);
        #endif
        #endif // Sun/EMX/Something else
    };
#define HAVE_USLEEP 1
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
    wxCHECK_MSG( !command.IsEmpty(), 0, wxT("can't exec empty command") );

    int argc = 0;
    wxChar *argv[WXEXECUTE_NARGS];
    wxString argument;
    const wxChar *cptr = command.c_str();
    wxChar quotechar = wxT('\0'); // is arg quoted?
    bool escaped = FALSE;

    // split the command line in arguments
    do
    {
        argument=wxT("");
        quotechar = wxT('\0');

        // eat leading whitespace:
        while ( wxIsspace(*cptr) )
            cptr++;

        if ( *cptr == wxT('\'') || *cptr == wxT('"') )
            quotechar = *cptr++;

        do
        {
            if ( *cptr == wxT('\\') && ! escaped )
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
                 || (quotechar == wxT('\0') && wxIsspace(*cptr))
                 || *cptr == wxT('\0') )
            {
                wxASSERT_MSG( argc < WXEXECUTE_NARGS,
                              wxT("too many arguments in wxExecute") );

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
        cmd.Printf(wxT("xterm -e %s"), command.c_str());
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
    wxCHECK_MSG( *argv, 0, wxT("can't exec empty command") );

#if wxUSE_UNICODE
    int mb_argc = 0;
    char *mb_argv[WXEXECUTE_NARGS];

    while (argv[mb_argc])
    {
      wxWX2MBbuf mb_arg = wxConvertWX2MB(argv[mb_argc]);
      mb_argv[mb_argc] = strdup(mb_arg);
      mb_argc++;
    }
    mb_argv[mb_argc] = (char *) NULL;

    // this macro will free memory we used above
    #define ARGS_CLEANUP                                 \
        for ( mb_argc = 0; mb_argv[mb_argc]; mb_argc++ ) \
            free(mb_argv[mb_argc])
#else // ANSI
    // no need for cleanup
    #define ARGS_CLEANUP

    wxChar **mb_argv = argv;
#endif // Unicode/ANSI

#if wxUSE_GUI
    // create pipes
    int end_proc_detect[2];
    if (pipe(end_proc_detect) == -1)
    {
        wxLogSysError( _("Pipe creation failed") );

        ARGS_CLEANUP;

        return 0;
    }
#endif // wxUSE_GUI

    // fork the process
#if HAVE_VFORK
    pid_t pid = vfork();
#else
    pid_t pid = fork();
#endif
    if (pid == -1)
    {
        wxLogSysError( _("Fork failed") );

        ARGS_CLEANUP;

        return 0;
    }
    else if (pid == 0)
    {
#if wxUSE_GUI
        // we're in child
        close(end_proc_detect[0]); // close reading side
#endif // wxUSE_GUI

        // These three lines close the open file descriptors to to avoid any
        // input/output which might block the process or irritate the user. If
        // one wants proper IO for the subprocess, the right thing to do is
        // to start an xterm executing it.
        if (sync == 0)
        {
            // leave stderr opened, it won't do any hurm
            for ( int fd = 0; fd < FD_SETSIZE; fd++ )
            {
#if wxUSE_GUI
                if ( fd == end_proc_detect[1] )
                    continue;
#endif // wxUSE_GUI

                if ( fd != STDERR_FILENO )
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
#if wxUSE_GUI
        // we're in parent
        close(end_proc_detect[1]); // close writing side

        wxEndProcessData *data = new wxEndProcessData;
        data->tag = wxAddProcessCallback(data, end_proc_detect[0]);

        ARGS_CLEANUP;

        if ( sync )
        {
            wxASSERT_MSG( !process, wxT("wxProcess param ignored for sync exec") );
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
#else // !wxUSE_GUI
        wxASSERT_MSG( sync, wxT("async execution not supported yet") );

        int exitcode = 0;
        if ( waitpid(pid, &exitcode, 0) == -1 || !WIFEXITED(exitcode) )
        {
            wxLogSysError(_("Waiting for subprocess termination failed"));
        }

        return exitcode;
#endif // wxUSE_GUI
    }

    #undef ARGS_CLEANUP
}

// ----------------------------------------------------------------------------
// file and directory functions
// ----------------------------------------------------------------------------

const wxChar* wxGetHomeDir( wxString *home  )
{
    *home = wxGetUserHome( wxString() );
    if ( home->IsEmpty() )
        *home = wxT("/");

    return home->c_str();
}

#if wxUSE_UNICODE
const wxMB2WXbuf wxGetUserHome( const wxString &user )
#else // just for binary compatibility -- there is no 'const' here
char *wxGetUserHome( const wxString &user )
#endif
{
    struct passwd *who = (struct passwd *) NULL;

    if ( !user )
    {
        wxChar *ptr;

        if ((ptr = wxGetenv(wxT("HOME"))) != NULL)
        {
            return ptr;
        }
        if ((ptr = wxGetenv(wxT("USER"))) != NULL || (ptr = wxGetenv(wxT("LOGNAME"))) != NULL)
        {
            who = getpwnam(wxConvertWX2MB(ptr));
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

    return wxConvertMB2WX(who ? who->pw_dir : 0);
}

// ----------------------------------------------------------------------------
// network and user id routines
// ----------------------------------------------------------------------------

// retrieve either the hostname or FQDN depending on platform (caller must
// check whether it's one or the other, this is why this function is for
// private use only)
static bool wxGetHostNameInternal(wxChar *buf, int sz)
{
    wxCHECK_MSG( buf, FALSE, wxT("NULL pointer in wxGetHostNameInternal") );

    *buf = wxT('\0');

    // we're using uname() which is POSIX instead of less standard sysinfo()
#if defined(HAVE_UNAME)
    struct utsname uts;
    bool ok = uname(&uts) != -1;
    if ( ok )
    {
        wxStrncpy(buf, wxConvertMB2WX(uts.nodename), sz - 1);
        buf[sz] = wxT('\0');
    }
#elif defined(HAVE_GETHOSTNAME)
    bool ok = gethostname(buf, sz) != -1;
#else // no uname, no gethostname
    wxFAIL_MSG(wxT("don't know host name for this machine"));

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
        wxChar *dot = wxStrchr(buf, wxT('.'));
        if ( dot )
        {
            // nuke it
            *dot = wxT('\0');
        }
    }

    return ok;
}

bool wxGetFullHostName(wxChar *buf, int sz)
{
    bool ok = wxGetHostNameInternal(buf, sz);

    if ( ok )
    {
        if ( !wxStrchr(buf, wxT('.')) )
        {
            struct hostent *host = gethostbyname(wxConvertWX2MB(buf));
            if ( !host )
            {
                wxLogSysError(_("Cannot get the official hostname"));

                ok = FALSE;
            }
            else
            {
                // the canonical name
                wxStrncpy(buf, wxConvertMB2WX(host->h_name), sz);
            }
        }
        //else: it's already a FQDN (BSD behaves this way)
    }

    return ok;
}

bool wxGetUserId(wxChar *buf, int sz)
{
    struct passwd *who;

    *buf = wxT('\0');
    if ((who = getpwuid(getuid ())) != NULL)
    {
        wxStrncpy (buf, wxConvertMB2WX(who->pw_name), sz - 1);
        return TRUE;
    }

    return FALSE;
}

bool wxGetUserName(wxChar *buf, int sz)
{
    struct passwd *who;
    char *comma;

    *buf = wxT('\0');
    if ((who = getpwuid (getuid ())) != NULL) {
       comma = strchr(who->pw_gecos, ',');
       if (comma)
           *comma = '\0'; // cut off non-name comment fields
       wxStrncpy (buf, wxConvertMB2WX(who->pw_gecos), sz - 1);
       return TRUE;
    }

    return FALSE;
}

#if wxUSE_GUI

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
  if (!title.IsNull()) wxFprintf( stderr, wxT("%s "), WXSTRINGCAST(title) );
  if (!msg.IsNull()) wxFprintf( stderr, wxT(": %s"), WXSTRINGCAST(msg) );
  wxFprintf( stderr, wxT(".\n") );
}

void wxFatalError( const wxString &msg, const wxString &title )
{
  wxFprintf( stderr, _("Error ") );
  if (!title.IsNull()) wxFprintf( stderr, wxT("%s "), WXSTRINGCAST(title) );
  if (!msg.IsNull()) wxFprintf( stderr, wxT(": %s"), WXSTRINGCAST(msg) );
  wxFprintf( stderr, wxT(".\n") );
  exit(3); // the same exit code as for abort()
}

// ----------------------------------------------------------------------------
// font-related functions
// ----------------------------------------------------------------------------

// define the functions to create and destroy native fonts for this toolkit
#ifdef __X__
    static inline wxNativeFont wxLoadFont(const wxString& fontSpec)
    {
        return XLoadQueryFont((Display *)wxGetDisplay(), fontSpec);
    }

    static inline void wxFreeFont(wxNativeFont font)
    {
        XFreeFont((Display *)wxGetDisplay(), font);
    }
#elif defined(__WXGTK__)

    #include "gdk/gdk.h"

    static inline wxNativeFont wxLoadFont(const wxString& fontSpec)
    {
        return gdk_font_load( wxConvertWX2MB(fontSpec) );
    }

    static inline void wxFreeFont(wxNativeFont font)
    {
        gdk_font_unref(font);
    }
#else
    #error "Unknown GUI toolkit"
#endif

// returns TRUE if there are any fonts matching this font spec
static bool wxTestFontSpec(const wxString& fontspec)
{
    wxNativeFont test = wxLoadFont(fontspec);
    if ( test )
    {
        wxFreeFont(test);

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// TODO encoding test logic should be moved to wxLoadQueryNearestFont()
static wxNativeFont wxLoadQueryFont(int pointSize,
                                    int family,
                                    int style,
                                    int weight,
                                    bool WXUNUSED(underlined),
                                    const wxString &facename,
                                    wxFontEncoding encoding )
{
    wxString xfamily;
    switch (family)
    {
        case wxDECORATIVE: xfamily = wxT("lucida"); break;
        case wxROMAN:      xfamily = wxT("times");  break;
        case wxMODERN:     xfamily = wxT("courier"); break;
        case wxSWISS:      xfamily = wxT("helvetica"); break;
        case wxTELETYPE:   xfamily = wxT("lucidatypewriter"); break;
        case wxSCRIPT:     xfamily = wxT("utopia"); break;
        default:           xfamily = wxT("*");
    }

    wxString fontSpec;
    if (!facename.IsEmpty())
    {
        fontSpec.Printf(wxT("-*-%s-*-*-normal-*-*-*-*-*-*-*-*-*"),
                        facename.c_str());

        if ( wxTestFontSpec(fontSpec) )
        {
            xfamily = facename;
        }
        //else: no such family, use default one instead
    }

    wxString xstyle;
    switch (style)
    {
        case wxITALIC:     xstyle = wxT("i"); break;
        case wxSLANT:      xstyle = wxT("o"); break;
        case wxNORMAL:     xstyle = wxT("r"); break;
        default:           xstyle = wxT("*"); break;
    }

    wxString xweight;
    switch (weight)
    {
         case wxBOLD:       
             {
                  fontSpec.Printf(wxT("-*-%s-bold-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                       xweight = wxT("bold"); 
                       break;
                  }
                  fontSpec.Printf(wxT("-*-%s-heavy-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                       xweight = wxT("heavy"); 
                       break;
                  }
                  fontSpec.Printf(wxT("-*-%s-extrabold-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                      xweight = wxT("extrabold");
                      break;
                  }
                  fontSpec.Printf(wxT("-*-%s-demibold-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                      xweight = wxT("demibold");
                      break;
                  }
                  fontSpec.Printf(wxT("-*-%s-black-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                      xweight = wxT("black");
                      break;
                  }
                  fontSpec.Printf(wxT("-*-%s-ultrablack-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                      xweight = wxT("ultrablack");
                      break;
                  }
              }   
              break;
        case wxLIGHT:
             {
                  fontSpec.Printf(wxT("-*-%s-light-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                       xweight = wxT("light");
                       break;
                  }
                  fontSpec.Printf(wxT("-*-%s-thin-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                       xweight = wxT("thin"); 
                       break;
                  }
             }
             break;
         case wxNORMAL:     
             {
                  fontSpec.Printf(wxT("-*-%s-medium-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                       xweight = wxT("medium"); 
                       break;
                  }
                  fontSpec.Printf(wxT("-*-%s-normal-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                       xweight = wxT("normal"); 
                       break;
                  }
                  fontSpec.Printf(wxT("-*-%s-regular-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                      xweight = wxT("regular");
                      break;
                  }
                  xweight = wxT("*");
              }   
              break;
        default:           xweight = wxT("*"); break;
    }

    wxString xregistry, xencoding;
    if ( encoding == wxFONTENCODING_DEFAULT )
    {
        // use the apps default
        encoding = wxFont::GetDefaultEncoding();
    }

    bool test = TRUE;   // should we test for availability of encoding?
    switch ( encoding )
    {
        case wxFONTENCODING_ISO8859_1:
        case wxFONTENCODING_ISO8859_2:
        case wxFONTENCODING_ISO8859_3:
        case wxFONTENCODING_ISO8859_4:
        case wxFONTENCODING_ISO8859_5:
        case wxFONTENCODING_ISO8859_6:
        case wxFONTENCODING_ISO8859_7:
        case wxFONTENCODING_ISO8859_8:
        case wxFONTENCODING_ISO8859_9:
        case wxFONTENCODING_ISO8859_10:
        case wxFONTENCODING_ISO8859_11:
        case wxFONTENCODING_ISO8859_13:
        case wxFONTENCODING_ISO8859_14:
        case wxFONTENCODING_ISO8859_15:
            {
                int cp = encoding - wxFONTENCODING_ISO8859_1 + 1;
                xregistry = wxT("iso8859");
                xencoding.Printf(wxT("%d"), cp);
            }
            break;

        case wxFONTENCODING_KOI8:
            xregistry = wxT("koi8");
            if ( wxTestFontSpec(wxT("-*-*-*-*-*-*-*-*-*-*-*-*-koi8-1")) )
            {
                xencoding = wxT("1");

                // test passed, no need to do it once more
                test = FALSE;
            }
            else
            {
                xencoding = wxT("*");
            }
            break;

        case wxFONTENCODING_CP1250:
        case wxFONTENCODING_CP1251:
        case wxFONTENCODING_CP1252:
            {
                int cp = encoding - wxFONTENCODING_CP1250 + 1250;
                fontSpec.Printf(wxT("-*-*-*-*-*-*-*-*-*-*-*-*-microsoft-cp%d"),
                                cp);
                if ( wxTestFontSpec(fontSpec) )
                {
                    xregistry = wxT("microsoft");
                    xencoding.Printf(wxT("cp%d"), cp);

                    // test passed, no need to do it once more
                    test = FALSE;
                }
                else
                {
                    // fall back to LatinX
                    xregistry = wxT("iso8859");
                    xencoding.Printf(wxT("%d"), cp - 1249);
                }
            }
            break;

        case wxFONTENCODING_SYSTEM:
        default:
            test = FALSE;
            xregistry =
            xencoding = wxT("*");
    }

    if ( test )
    {
        fontSpec.Printf(wxT("-*-*-*-*-*-*-*-*-*-*-*-*-%s-%s"),
                        xregistry.c_str(), xencoding.c_str());
        if ( !wxTestFontSpec(fontSpec) )
        {
            // this encoding isn't available - what to do?
            xregistry =
            xencoding = wxT("*");
        }
    }

    // construct the X font spec from our data
    fontSpec.Printf(wxT("-*-%s-%s-%s-normal-*-*-%d-*-*-*-*-%s-%s"),
                    xfamily.c_str(), xweight.c_str(), xstyle.c_str(),
                    pointSize, xregistry.c_str(), xencoding.c_str());

    return wxLoadFont(fontSpec);
}

wxNativeFont wxLoadQueryNearestFont(int pointSize,
                                    int family,
                                    int style,
                                    int weight,
                                    bool underlined,
                                    const wxString &facename,
                                    wxFontEncoding encoding)
{
    wxNativeFont font = wxLoadQueryFont( pointSize, family, style, weight,
                                         underlined, facename, encoding );

    if (!font)
    {
        // search up and down by stepsize 10
        int max_size = pointSize + 20 * (1 + (pointSize/180));
        int min_size = pointSize - 20 * (1 + (pointSize/180));

        int i;

        // Search for smaller size (approx.)
        for ( i = pointSize - 10; !font && i >= 10 && i >= min_size; i -= 10 )
        {
            font = wxLoadQueryFont(i, family, style, weight, underlined,
                                   facename, encoding );
        }

        // Search for larger size (approx.)
        for ( i = pointSize + 10; !font && i <= max_size; i += 10 )
        {
            font = wxLoadQueryFont( i, family, style, weight, underlined,
                                    facename, encoding );
        }

        // Try default family
        if ( !font && family != wxDEFAULT )
        {
            font = wxLoadQueryFont( pointSize, wxDEFAULT, style, weight,
                                    underlined, facename, encoding );
        }

        // Bogus font I
        if ( !font )
        {
            font = wxLoadQueryFont(120, wxDEFAULT, wxNORMAL, wxNORMAL,
                                   underlined, facename, encoding );
        }
 
        // Bogus font II
 	if ( !font )
 	{
 	    font = wxLoadQueryFont(120, wxDEFAULT, wxNORMAL, wxNORMAL,
 				   underlined, wxEmptyString, encoding );
 	}
    }

    return font;
}

#endif // wxUSE_GUI

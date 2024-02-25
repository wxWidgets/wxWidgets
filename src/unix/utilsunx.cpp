/////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/utilsunx.cpp
// Purpose:     generic Unix implementation of many wx functions (for wxBase)
// Author:      Vadim Zeitlin
// Copyright:   (c) 1998 Robert Roebling, Vadim Zeitlin
//              (c) 2013 Rob Bresalier, Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

// Define this as soon as possible and before string.h is included to get
// memset_s() declaration from it if available.
#define __STDC_WANT_LIB_EXT1__ 1

#include "wx/utils.h"

#if !defined(HAVE_SETENV) && defined(HAVE_PUTENV)
    #define USE_PUTENV 1
#else
    #define USE_PUTENV 0
#endif

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/wxcrtvararg.h"
    #if USE_PUTENV
        #include "wx/module.h"
    #endif
#endif

#include "wx/apptrait.h"

#include "wx/process.h"
#include "wx/thread.h"

#include "wx/cmdline.h"

#include "wx/wfstream.h"

#include "wx/private/selectdispatcher.h"
#include "wx/private/fdiodispatcher.h"
#include "wx/private/glibc.h"
#include "wx/unix/private/execute.h"
#include "wx/unix/pipe.h"
#include "wx/unix/private.h"

#include "wx/evtloop.h"
#include "wx/mstream.h"
#include "wx/private/fdioeventloopsourcehandler.h"
#include "wx/config.h"
#include "wx/filename.h"

#include <memory>

#include <pwd.h>
#include <sys/wait.h>       // waitpid()

#ifdef HAVE_SYS_SELECT_H
#   include <sys/select.h>
#endif

#define HAS_PIPE_STREAMS (wxUSE_STREAMS && wxUSE_FILE)

#if HAS_PIPE_STREAMS

#include "wx/private/pipestream.h"
#include "wx/private/streamtempinput.h"
#include "wx/unix/private/executeiohandler.h"

#endif // HAS_PIPE_STREAMS

// not only the statfs syscall is called differently depending on platform, but
// one of its incarnations, statvfs(), takes different arguments under
// different platforms and even different versions of the same system (Solaris
// 7 and 8): if you want to test for this, don't forget that the problems only
// appear if the large files support is enabled
#if defined(HAVE_STATVFS)
    #include <sys/statvfs.h>

    #define wxStatfs statvfs
#elif defined(HAVE_STATFS)
    #ifdef __BSD__
        #include <sys/param.h>
        #include <sys/mount.h>
    #else // !__BSD__
        #include <sys/vfs.h>
    #endif // __BSD__/!__BSD__

    #define wxStatfs statfs

    #ifndef HAVE_STATFS_DECL
        // some systems lack statfs() prototype in the system headers (AIX 4)
        extern "C" int statfs(const char *path, struct statfs *buf);
    #endif
#endif // HAVE_STATVFS/HAVE_STATFS

#if defined(HAVE_STATVFS) || defined(HAVE_STATFS)
    // WX_STATFS_T is detected by configure
    #define wxStatfs_t WX_STATFS_T
#endif

// SGI signal.h defines signal handler arguments differently depending on
// whether _LANGUAGE_C_PLUS_PLUS is set or not - do set it
#if defined(__SGI__) && !defined(_LANGUAGE_C_PLUS_PLUS)
    #define _LANGUAGE_C_PLUS_PLUS 1
#endif // SGI hack

#include <stdarg.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <time.h>           // nanosleep() and/or usleep()
#include <ctype.h>          // isspace()
#include <sys/time.h>       // needed for FD_SETSIZE

#ifdef HAVE_UNAME
    #include <sys/utsname.h> // for uname()
#endif // HAVE_UNAME

// Used by wxGetFreeMemory().
#ifdef __SGI__
    #include <sys/sysmp.h>
    #include <sys/sysinfo.h>   // for SAGET and MINFO structures
#endif

#ifdef HAVE_SETPRIORITY
    #include <sys/resource.h>   // for setpriority()
#endif

#if defined(__DARWIN__)
    #include <sys/sysctl.h>
#endif

// ----------------------------------------------------------------------------
// conditional compilation
// ----------------------------------------------------------------------------

// many versions of Unices have this function, but it is not defined in system
// headers - please add your system here if it is the case for your OS.
// SunOS < 5.6 (i.e. Solaris < 2.6) and DG-UX are like this.
#if !defined(HAVE_USLEEP) && \
    ((defined(__SUN__) && !defined(__SunOs_5_6) && \
                         !defined(__SunOs_5_7) && !defined(__SUNPRO_CC)) || \
     defined(__osf__))
    extern "C"
    {
        int usleep(unsigned int usec);
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

void wxMicroSleep(unsigned long microseconds)
{
#if defined(HAVE_NANOSLEEP)
    timespec tmReq;
    tmReq.tv_sec = (time_t)(microseconds / 1000000);
    tmReq.tv_nsec = (microseconds % 1000000) * 1000;

    // we're not interested in remaining time nor in return value
    (void)nanosleep(&tmReq, nullptr);
#elif defined(HAVE_USLEEP)
    // uncomment this if you feel brave or if you are sure that your version
    // of Solaris has a safe usleep() function but please notice that usleep()
    // is known to lead to crashes in MT programs in Solaris 2.[67] and is not
    // documented as MT-Safe
    #if defined(__SUN__) && wxUSE_THREADS
        #error "usleep() cannot be used in MT programs under Solaris."
    #endif // Sun

    usleep(microseconds);
#else // !sleep function
    #error "usleep() or nanosleep() function required for wxMicroSleep"
#endif // sleep function
}

void wxMilliSleep(unsigned long milliseconds)
{
    wxMicroSleep(milliseconds*1000);
}

// ----------------------------------------------------------------------------
// security
// ----------------------------------------------------------------------------

void wxSecureZeroMemory(void* v, size_t n)
{
#if wxCHECK_GLIBC_VERSION(2, 25) || \
    (defined(__FreeBSD__) && __FreeBSD__ >= 11)
    // This non-standard function is somewhat widely available elsewhere too,
    // but may be found in a non-standard header file, or in a library that is
    // not linked by default.
    explicit_bzero(v, n);
#elif defined(__DARWIN__) || defined(__STDC_LIB_EXT1__)
    // memset_s() is available since OS X 10.9, and may be available on
    // other platforms.
    memset_s(v, n, 0, n);
#else
    // A generic implementation based on the example at:
    // http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1381.pdf
    int c = 0;
    volatile unsigned char *p = reinterpret_cast<unsigned char *>(v);
    while ( n-- )
        *p++ = c;
#endif
}

// ----------------------------------------------------------------------------
// process management
// ----------------------------------------------------------------------------

int wxKill(long pid, wxSignal sig, wxKillError *rc, int flags)
{
    int err = kill((pid_t) (flags & wxKILL_CHILDREN) ? -pid : pid, (int)sig);
    if ( rc )
    {
        switch ( err ? errno : 0 )
        {
            case 0:
                *rc = wxKILL_OK;
                break;

            case EINVAL:
                *rc = wxKILL_BAD_SIGNAL;
                break;

            case EPERM:
                *rc = wxKILL_ACCESS_DENIED;
                break;

            case ESRCH:
                *rc = wxKILL_NO_PROCESS;
                break;

            default:
                // this goes against Unix98 docs so log it
                wxLogDebug(wxT("unexpected kill(2) return value %d"), err);

                // something else...
                *rc = wxKILL_ERROR;
        }
    }

    return err;
}

// Shutdown or reboot the PC
bool wxShutdown(int flags)
{
#if defined(__WXOSX__) && wxOSX_USE_IPHONE
    wxUnusedVar(flags);
    return false;
#else
    flags &= ~wxSHUTDOWN_FORCE;

    wxChar level;
    switch ( flags )
    {
        case wxSHUTDOWN_POWEROFF:
            level = wxT('0');
            break;

        case wxSHUTDOWN_REBOOT:
            level = wxT('6');
            break;

        case wxSHUTDOWN_LOGOFF:
            // TODO: use dcop to log off?
            return false;

        default:
            wxFAIL_MSG( wxT("unknown wxShutdown() flag") );
            return false;
    }

    return system(wxString::Format("init %c", level).mb_str()) == 0;
#endif
}

// ----------------------------------------------------------------------------
// wxStream classes to support IO redirection in wxExecute
// ----------------------------------------------------------------------------

#if HAS_PIPE_STREAMS

bool wxPipeInputStream::CanRead() const
{
    if ( m_lasterror == wxSTREAM_EOF )
        return false;

    // check if there is any input available
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    const int fd = m_file->fd();

    fd_set readfds;

    wxFD_ZERO(&readfds);
    wxFD_SET(fd, &readfds);

    switch ( select(fd + 1, &readfds, nullptr, nullptr, &tv) )
    {
        case -1:
            wxLogSysError(_("Impossible to get child process input"));
            wxFALLTHROUGH;

        case 0:
            return false;

        default:
            wxFAIL_MSG(wxT("unexpected select() return value"));
            wxFALLTHROUGH;

        case 1:
            // input available -- or maybe not, as select() returns 1 when a
            // read() will complete without delay, but it could still not read
            // anything
            return !Eof();
    }
}

size_t wxPipeOutputStream::OnSysWrite(const void *buffer, size_t size)
{
    // We need to suppress error logging here, because on writing to a pipe
    // which is full, wxFile::Write reports a system error. However, this is
    // not an extraordinary situation, and it should not be reported to the
    // user (but if really needed, the program can recognize it by checking
    // whether LastRead() == 0.) Other errors will be reported below.
    size_t ret;
    {
        wxLogNull logNo;
        ret = m_file->Write(buffer, size);
    }

    switch ( m_file->GetLastError() )
    {
       // pipe is full
#ifdef EAGAIN
       case EAGAIN:
#endif
#if defined(EWOULDBLOCK) && (EWOULDBLOCK != EAGAIN)
       case EWOULDBLOCK:
#endif
           // do not treat it as an error
           m_file->ClearLastError();
           wxFALLTHROUGH;

       // no error
       case 0:
           break;

       // some real error
       default:
           wxLogSysError(_("Can't write to child process's stdin"));
           m_lasterror = wxSTREAM_WRITE_ERROR;
    }

    return ret;
}

#endif // HAS_PIPE_STREAMS

// ----------------------------------------------------------------------------
// wxShell
// ----------------------------------------------------------------------------

static wxString wxMakeShellCommand(const wxString& command)
{
    wxString cmd;
    if ( !command )
    {
        // just an interactive shell
        cmd = wxT("xterm");
    }
    else
    {
        // execute command in a shell
        cmd << wxT("/bin/sh -c '") << command << wxT('\'');
    }

    return cmd;
}

bool wxShell(const wxString& command)
{
    return wxExecute(wxMakeShellCommand(command), wxEXEC_SYNC) == 0;
}

bool wxShell(const wxString& command, wxArrayString& output)
{
    wxCHECK_MSG( !command.empty(), false, wxT("can't exec shell non interactively") );

    return wxExecute(wxMakeShellCommand(command), output);
}

namespace
{

// helper class for storing arguments as char** array suitable for passing to
// execvp(), whatever form they were passed to us
class ArgsArray
{
public:
    ArgsArray(const wxArrayString& args)
    {
        Init(args.size());

        for ( int i = 0; i < m_argc; i++ )
        {
            m_argv[i] = wxStrdup(args[i].mb_str(wxConvWhateverWorks));
        }
    }

    ArgsArray(const wchar_t* const* wargv)
    {
        int argc = 0;
        while ( wargv[argc] )
            argc++;

        Init(argc);

        for ( int i = 0; i < m_argc; i++ )
        {
            m_argv[i] = wxSafeConvertWX2MB(wargv[i]).release();
        }
    }

    ~ArgsArray()
    {
        for ( int i = 0; i < m_argc; i++ )
        {
            free(m_argv[i]);
        }

        delete [] m_argv;
    }

    operator const char* const*() const { return m_argv; }

private:
    void Init(int argc)
    {
        m_argc = argc;
        m_argv = new char *[m_argc + 1];
        m_argv[m_argc] = nullptr;
    }

    int m_argc;
    char **m_argv;

    wxDECLARE_NO_COPY_CLASS(ArgsArray);
};

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxExecute implementations
// ----------------------------------------------------------------------------

#if defined(__DARWIN__) && !defined(__WXOSX_IPHONE__)
bool wxCocoaLaunch(const char* const* argv, pid_t &pid);
#endif

long wxExecute(const wxString& command, int flags, wxProcess *process,
        const wxExecuteEnv *env)
{
    ArgsArray argv(wxCmdLineParser::ConvertStringToArgs(command,
                                                        wxCMD_LINE_SPLIT_UNIX));

    return wxExecute(argv, flags, process, env);
}

long wxExecute(const wchar_t* const* wargv, int flags, wxProcess* process,
        const wxExecuteEnv *env)
{
    ArgsArray argv(wargv);

    return wxExecute(argv, flags, process, env);
}

namespace
{

// Helper function of wxExecute(): wait for the process termination without
// dispatching any events.
//
// This is used in wxEXEC_NOEVENTS case.
int BlockUntilChildExit(wxExecuteData& execData)
{
    wxCHECK_MSG( wxTheApp, -1,
                    wxS("Can't block until child exit without wxTheApp") );

#if wxUSE_SELECT_DISPATCHER

    // Even if we don't want to dispatch events, we still need to handle
    // child IO notifications and process termination concurrently, i.e.
    // we can't simply block waiting for the child to terminate as we would
    // dead lock if it writes more than the pipe buffer size (typically
    // 4KB) bytes of output -- it would then block waiting for us to read
    // the data while we'd block waiting for it to terminate.
    //
    // So while we don't use the full blown event loop, we still do use a
    // dispatcher with which we register just the 3 FDs we're interested
    // in: the child stdout and stderr and the pipe written to by the
    // signal handler so that we could react to the child process
    // termination too.

    // Notice that we must create a new dispatcher object here instead of
    // reusing the global wxFDIODispatcher::Get() because we want to
    // monitor only the events on the FDs explicitly registered with this
    // one and not all the other ones that could be registered with the
    // global dispatcher (think about the case of nested wxExecute() calls).
    wxSelectDispatcher dispatcher;

    // Do register all the FDs we want to monitor here: first, the one used to
    // handle the signals asynchronously.
    std::unique_ptr<wxFDIOHandler>
        signalHandler(wxTheApp->RegisterSignalWakeUpPipe(dispatcher));

#if wxUSE_STREAMS
    // And then the two for the child output and error streams if necessary.
    std::unique_ptr<wxFDIOHandler>
        stdoutHandler,
        stderrHandler;
    if ( execData.IsRedirected() )
    {
        stdoutHandler.reset(new wxExecuteFDIOHandler
                                (
                                    dispatcher,
                                    execData.m_fdOut,
                                    execData.m_bufOut
                                ));
        stderrHandler.reset(new wxExecuteFDIOHandler
                                (
                                    dispatcher,
                                    execData.m_fdErr,
                                    execData.m_bufErr
                                ));
    }
#endif // wxUSE_STREAMS

    // And dispatch until the PID is reset from wxExecuteData::OnExit().
    while ( execData.m_pid )
    {
        dispatcher.Dispatch();
    }

    return execData.m_exitcode;
#else // !wxUSE_SELECT_DISPATCHER
    wxFAIL_MSG( wxS("Can't block until child exit without wxSelectDispatcher") );

    return -1;
#endif // wxUSE_SELECT_DISPATCHER/!wxUSE_SELECT_DISPATCHER
}

} // anonymous namespace

// wxExecute: the real worker function
long wxExecute(const char* const* argv, int flags, wxProcess* process,
        const wxExecuteEnv *env)
{
    // for the sync execution, we return -1 to indicate failure, but for async
    // case we return 0 which is never a valid PID
    //
    // we define this as a macro, not a variable, to avoid compiler warnings
    // about "ERROR_RETURN_CODE value may be clobbered by fork()"
    #define ERROR_RETURN_CODE ((flags & wxEXEC_SYNC) ? -1 : 0)

    wxCHECK_MSG( *argv, ERROR_RETURN_CODE, wxT("can't exec empty command") );

#if wxUSE_THREADS
    // fork() doesn't mix well with POSIX threads: on many systems the program
    // deadlocks or crashes for some reason. Probably our code is buggy and
    // doesn't do something which must be done to allow this to work, but I
    // don't know what yet, so for now just warn the user (this is the least we
    // can do) about it
    wxASSERT_MSG( wxThread::IsMain(),
                    wxT("wxExecute() can be called only from the main thread") );
#endif // wxUSE_THREADS
    pid_t pid;
#if defined(__DARWIN__) && !defined(__WXOSX_IPHONE__)
    pid = -1;
    // wxCocoaLaunch() only executes app bundles and only does it asynchronously.
    // It returns false if the target is not an app bundle, thus falling
    // through to the regular code for non app bundles.
    if ( !(flags & wxEXEC_SYNC) && wxCocoaLaunch(argv, pid) )
    {
        return pid;
    }
#endif // __DARWIN__

    // this struct contains all information which we use for housekeeping
    std::unique_ptr<wxExecuteData> execDataPtr(new wxExecuteData);
    wxExecuteData& execData = *execDataPtr;

    execData.m_flags = flags;
    execData.m_process = process;

    // create pipes for inter process communication
    wxPipe pipeIn,      // stdin
           pipeOut,     // stdout
           pipeErr;     // stderr

    if ( process && process->IsRedirected() )
    {
        if ( !pipeIn.Create() || !pipeOut.Create() || !pipeErr.Create() )
        {
            wxLogError( _("Failed to execute '%s'\n"), *argv );

            return ERROR_RETURN_CODE;
        }
    }

    // priority: we need to map wxWidgets priority which is in the range 0..100
    // to Unix nice value which is in the range -20..19. As there is an odd
    // number of elements in our range and an even number in the Unix one, we
    // have to do it in this rather ugly way to guarantee that:
    //  1. wxPRIORITY_{MIN,DEFAULT,MAX} map to -20, 0 and 19 respectively.
    //  2. The mapping is monotonously increasing.
    //  3. The mapping is onto the target range.
    int prio = process ? int(process->GetPriority()) : int(wxPRIORITY_DEFAULT);
    if ( prio <= 50 )
        prio = (2*prio)/5 - 20;
    else if ( prio < 55 )
        prio = 1;
    else
        prio = (2*prio)/5 - 21;

    // fork the process
    //
    // NB: do *not* use vfork() here, it completely breaks this code for some
    //     reason under Solaris (and maybe others, although not under Linux)
    //     But on OpenVMS we do not have fork so we have to use vfork and
    //     cross our fingers that it works.
#ifdef __VMS
   pid = vfork();
#else
   pid = fork();
#endif
   if ( pid == -1 )     // error?
    {
        wxLogSysError( _("Fork failed") );

        return ERROR_RETURN_CODE;
    }
    else if ( pid == 0 )  // we're in child
    {
        // NB: we used to close all the unused descriptors of the child here
        //     but this broke some programs which relied on e.g. FD 1 being
        //     always opened so don't do it any more, after all there doesn't
        //     seem to be any real problem with keeping them opened

#if !defined(__VMS)
        if ( flags & wxEXEC_MAKE_GROUP_LEADER )
        {
            // Set process group to child process' pid.  Then killing -pid
            // of the parent will kill the process and all of its children.
            setsid();
        }
#endif // !__VMS

#if defined(HAVE_SETPRIORITY)
        if ( prio && setpriority(PRIO_PROCESS, 0, prio) != 0 )
        {
            wxLogSysError(_("Failed to set process priority"));
        }
#endif // HAVE_SETPRIORITY

        // redirect stdin, stdout and stderr
        if ( pipeIn.IsOk() )
        {
            if ( dup2(pipeIn[wxPipe::Read], STDIN_FILENO) == -1 ||
                 dup2(pipeOut[wxPipe::Write], STDOUT_FILENO) == -1 ||
                 dup2(pipeErr[wxPipe::Write], STDERR_FILENO) == -1 )
            {
                wxLogSysError(_("Failed to redirect child process input/output"));
            }

            pipeIn.Close();
            pipeOut.Close();
            pipeErr.Close();
        }

        // Close all (presumably accidentally) inherited file descriptors to
        // avoid descriptor leaks. This means that we don't allow inheriting
        // them purposefully but this seems like a lesser evil in wx code.
        // Ideally we'd provide some flag to indicate that none (or some?) of
        // the descriptors do not need to be closed but for now this is better
        // than never closing them at all as wx code never used FD_CLOEXEC.

        // TODO: Iterating up to FD_SETSIZE is both inefficient (because it may
        //       be quite big) and incorrect (because in principle we could
        //       have more opened descriptions than this number). Unfortunately
        //       there is no good portable solution for closing all descriptors
        //       above a certain threshold but non-portable solutions exist for
        //       most platforms, see [https://stackoverflow.com/questions/899038/
        //          getting-the-highest-allocated-file-descriptor]
        for ( int fd = 0; fd < (int)FD_SETSIZE; ++fd )
        {
            if ( fd != STDIN_FILENO  &&
                 fd != STDOUT_FILENO &&
                 fd != STDERR_FILENO )
            {
                close(fd);
            }
        }


        // Process additional options if we have any
        if ( env )
        {
            // Change working directory if it is specified
            if ( !env->cwd.empty() )
                wxSetWorkingDirectory(env->cwd);

            // Change environment if needed.
            //
            // NB: We can't use execve() currently because we allow using
            //     non full paths to wxExecute(), i.e. we want to search for
            //     the program in PATH. However it just might be simpler/better
            //     to do the search manually and use execve() envp parameter to
            //     set up the environment of the child process explicitly
            //     instead of doing what we do below.
            if ( !env->env.empty() )
            {
                wxEnvVariableHashMap oldenv;
                wxGetEnvMap(&oldenv);

                // Remove unwanted variables
                wxEnvVariableHashMap::const_iterator it;
                for ( it = oldenv.begin(); it != oldenv.end(); ++it )
                {
                    if ( env->env.find(it->first) == env->env.end() )
                        wxUnsetEnv(it->first);
                }

                // And add the new ones (possibly replacing the old values)
                for ( it = env->env.begin(); it != env->env.end(); ++it )
                    wxSetEnv(it->first, it->second);
            }
        }

        execvp(*argv, const_cast<char**>(argv));

        fprintf(stderr, "execvp(");
        for (const char* const* a = argv; *a; a++)
            fprintf(stderr, "%s%s", a == argv ? "" : ", ", *a);
        fprintf(stderr, ") failed with error %d!\n", errno);

        // there is no return after successful exec()
        _exit(-1);

        // some compilers complain about missing return - of course, they
        // should know that exit() doesn't return but what else can we do if
        // they don't?
        //
        // and, sure enough, other compilers complain about unreachable code
        // after exit() call, so we can just always have return here...
#if defined(__VMS) || defined(__INTEL_COMPILER)
        return 0;
#endif
    }
    else // we're in parent
    {
        // prepare for IO redirection

#if HAS_PIPE_STREAMS

        if ( process && process->IsRedirected() )
        {
            // Avoid deadlocks which could result from trying to write to the
            // child input pipe end while the child itself is writing to its
            // output end and waiting for us to read from it.
            if ( !pipeIn.MakeNonBlocking(wxPipe::Write) )
            {
                // This message is not terrible useful for the user but what
                // else can we do? Also, should we fail here or take the risk
                // to continue and deadlock? Currently we choose the latter but
                // it might not be the best idea.
                wxLogSysError(_("Failed to set up non-blocking pipe, "
                                "the program might hang."));
#if wxUSE_LOG
                wxLog::FlushActive();
#endif
            }

            wxOutputStream *inStream =
                new wxPipeOutputStream(pipeIn.Detach(wxPipe::Write));

            const int fdOut = pipeOut.Detach(wxPipe::Read);
            wxPipeInputStream *outStream = new wxPipeInputStream(fdOut);

            const int fdErr = pipeErr.Detach(wxPipe::Read);
            wxPipeInputStream *errStream = new wxPipeInputStream(fdErr);

            process->SetPipeStreams(outStream, inStream, errStream);

            if ( flags & wxEXEC_SYNC )
            {
                execData.m_bufOut.Init(outStream);
                execData.m_bufErr.Init(errStream);

                execData.m_fdOut = fdOut;
                execData.m_fdErr = fdErr;
            }
        }
#endif // HAS_PIPE_STREAMS

        if ( pipeIn.IsOk() )
        {
            pipeIn.Close();
            pipeOut.Close();
            pipeErr.Close();
        }

        if ( !(flags & wxEXEC_SYNC) )
        {
            // Ensure that the housekeeping data is kept alive, it will be
            // destroyed only when the child terminates.
            execDataPtr.release();
        }

        // Put the housekeeping data into the child process lookup table.
        // Note that when running asynchronously, if the child has already
        // finished this call will delete the execData and call any
        // wxProcess's OnTerminate() handler immediately.
        execData.OnStart(pid);

        // For the asynchronous case we don't have to do anything else, just
        // let the process run (if not already finished).
        if ( !(flags & wxEXEC_SYNC) )
            return pid;


        // If we don't need to dispatch any events, things are relatively
        // simple and we don't need to delegate to wxAppTraits.
        if ( flags & wxEXEC_NOEVENTS )
        {
            return BlockUntilChildExit(execData);
        }


        // If we do need to dispatch events, enter a local event loop waiting
        // until the child exits. As the exact kind of event loop depends on
        // the sort of application we're in (console or GUI), we delegate this
        // to wxAppTraits which virtualizes all the differences between the
        // console and the GUI programs.
        return wxApp::GetValidTraits().WaitForChild(execData);
    }

#if !defined(__VMS) && !defined(__INTEL_COMPILER)
    return ERROR_RETURN_CODE;
#endif
}

#undef ERROR_RETURN_CODE

// ----------------------------------------------------------------------------
// file and directory functions
// ----------------------------------------------------------------------------

const wxChar* wxGetHomeDir( wxString *home  )
{
    *home = wxGetUserHome();
    wxString tmp;
    if ( home->empty() )
        *home = wxT("/");
#ifdef __VMS
    tmp = *home;
    if ( tmp.Last() != wxT(']'))
        if ( tmp.Last() != wxT('/')) *home << wxT('/');
#endif
    return home->c_str();
}

wxString wxGetUserHome( const wxString &user )
{
    struct passwd *who = (struct passwd *) nullptr;

    if ( !user )
    {
        wxChar *ptr;

        if ((ptr = wxGetenv(wxT("HOME"))) != nullptr)
        {
            return ptr;
        }

        if ((ptr = wxGetenv(wxT("USER"))) != nullptr ||
             (ptr = wxGetenv(wxT("LOGNAME"))) != nullptr)
        {
            who = getpwnam(wxSafeConvertWX2MB(ptr));
        }

        // make sure the user exists!
        if ( !who )
        {
            who = getpwuid(getuid());
        }
    }
    else
    {
      who = getpwnam (user.mb_str());
    }

    return wxSafeConvertMB2WX(who ? who->pw_dir : nullptr);
}

// ----------------------------------------------------------------------------
// network and user id routines
// ----------------------------------------------------------------------------

// Private utility function which returns output of the given command, removing
// the trailing newline.
//
// Note that by default use Latin-1 just to ensure that we never fail, but if
// the encoding is known (e.g. UTF-8 for lsb_release), it should be explicitly
// used instead.
static wxString
wxGetCommandOutput(const wxString &cmd, wxMBConv& conv = wxConvISO8859_1)
{
    // Suppress stderr from the shell to avoid outputting errors if the command
    // doesn't exist.
#ifdef __VMS
    FILE *f = popen(( "pipe " + cmd + " 2>/nl:").ToAscii(), "r");
#else
    FILE *f = popen((cmd + " 2>/dev/null").ToAscii(), "r");
#endif
    if ( !f )
    {
        // Notice that this doesn't happen simply if the command doesn't exist,
        // but only in case of some really catastrophic failure inside popen()
        // so we should really notify the user about this as this is not normal.
        wxLogSysError(wxT("Executing \"%s\" failed"), cmd);
        return wxString();
    }

    wxString s;
    char buf[256];
    while ( !feof(f) )
    {
        if ( !fgets(buf, sizeof(buf), f) )
            break;

        s += wxString(buf, conv);
    }

    pclose(f);

    if ( !s.empty() && s.Last() == wxT('\n') )
        s.RemoveLast();

    return s;
}

// retrieve either the hostname or FQDN depending on platform (caller must
// check whether it's one or the other, this is why this function is for
// private use only)
static bool wxGetHostNameInternal(wxChar *buf, int sz)
{
    wxCHECK_MSG( buf, false, wxT("null pointer in wxGetHostNameInternal") );

    *buf = wxT('\0');

    // we're using uname() which is POSIX instead of less standard sysinfo()
#if defined(HAVE_UNAME)
    struct utsname uts;
    bool ok = uname(&uts) != -1;
    if ( ok )
    {
        wxStrlcpy(buf, wxSafeConvertMB2WX(uts.nodename), sz);
    }
#elif defined(HAVE_GETHOSTNAME)
    char cbuf[sz];
    bool ok = gethostname(cbuf, sz) != -1;
    if ( ok )
    {
        wxStrlcpy(buf, wxSafeConvertMB2WX(cbuf), sz);
    }
#else // no uname, no gethostname
    wxFAIL_MSG(wxT("don't know host name for this machine"));

    bool ok = false;
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
            struct hostent *host = gethostbyname(wxSafeConvertWX2MB(buf));
            if ( !host )
            {
                wxLogSysError(_("Cannot get the official hostname"));

                ok = false;
            }
            else
            {
                // the canonical name
                wxStrlcpy(buf, wxSafeConvertMB2WX(host->h_name), sz);
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
    if ((who = getpwuid(getuid ())) != nullptr)
    {
        wxStrlcpy (buf, wxSafeConvertMB2WX(who->pw_name), sz);
        return true;
    }

    return false;
}

bool wxGetUserName(wxChar *buf, int sz)
{
#ifdef HAVE_PW_GECOS
    struct passwd *who;

    *buf = wxT('\0');
    if ((who = getpwuid (getuid ())) != nullptr)
    {
       char *comma = strchr(who->pw_gecos, ',');
       if (comma)
           *comma = '\0'; // cut off non-name comment fields
       wxStrlcpy(buf, wxSafeConvertMB2WX(who->pw_gecos), sz);
       return true;
    }

    return false;
#else // !HAVE_PW_GECOS
    return wxGetUserId(buf, sz);
#endif // HAVE_PW_GECOS/!HAVE_PW_GECOS
}

bool wxIsPlatform64Bit()
{
#if SIZEOF_VOID_P == 8
    (void)wxGetCommandOutput;
    return true;  // 64-bit programs run only on 64-bit platforms
#else
    const wxString machine = wxGetCpuArchitectureName();

    // the test for "64" is obviously not 100% reliable but seems to work fine
    // in practice
    return machine.Contains(wxT("64")) ||
                machine.Contains(wxT("alpha"));
#endif
}

wxString wxGetCpuArchitectureName()
{
    return wxGetCommandOutput(wxT("uname -m"));
}

wxString wxGetNativeCpuArchitectureName()
{
#if defined(__DARWIN__)
    // macOS on ARM will report an x86_64 process as translated, assume the native CPU is arm64
    int translated;
    size_t translated_size = sizeof(translated);
    if (sysctlbyname("sysctl.proc_translated", &translated, &translated_size, nullptr, 0) == 0)
        return "arm64";
    else
#endif
        return wxGetCpuArchitectureName();
}

#ifdef __LINUX__

static bool
wxGetValuesFromOSRelease(const wxString& filename, wxLinuxDistributionInfo& ret)
{
#if wxUSE_CONFIG
    if ( !wxFileName::Exists(filename) )
    {
        return false;
    }

    wxFileConfig fc(wxEmptyString, wxEmptyString, wxEmptyString, filename);
    ret.Id = fc.Read(wxS("ID"), wxEmptyString).Capitalize();
    ret.Description = fc.Read(wxS("PRETTY_NAME"), wxEmptyString);
    ret.Release = fc.Read(wxS("VERSION_ID"), wxEmptyString);
    ret.CodeName = fc.Read(wxS("VERSION_CODENAME"), wxEmptyString);

    return true;
#else
    return false;
#endif
}

static bool
wxGetValueFromLSBRelease(const wxString& arg, const wxString& lhs, wxString* rhs)
{
    // lsb_release seems to just read a global file which is always in UTF-8
    // and hence its output is always in UTF-8 as well, regardless of the
    // locale currently configured by our environment.
    return wxGetCommandOutput(wxS("lsb_release ") + arg, wxConvUTF8)
                .StartsWith(lhs, rhs);
}

wxLinuxDistributionInfo wxGetLinuxDistributionInfo()
{
    wxLinuxDistributionInfo ret;

    // Read /etc/os-release and fall back to /usr/lib/os-release per below
    // https://www.freedesktop.org/software/systemd/man/os-release.html
    if ( wxGetValuesFromOSRelease(wxS("/etc/os-release"), ret) )
    {
        return ret;
    }
    if ( wxGetValuesFromOSRelease(wxS("/usr/lib/os-release"), ret) )
    {
        return ret;
    }

    if ( !wxGetValueFromLSBRelease(wxS("--id"), wxS("Distributor ID:\t"),
                                   &ret.Id) )
    {
        // Don't bother to continue, lsb_release is probably not available.
        return ret;
    }

    wxGetValueFromLSBRelease(wxS("--description"), wxS("Description:\t"),
                             &ret.Description);
    wxGetValueFromLSBRelease(wxS("--release"), wxS("Release:\t"),
                             &ret.Release);
    wxGetValueFromLSBRelease(wxS("--codename"), wxS("Codename:\t"),
                             &ret.CodeName);

    return ret;
}
#endif // __LINUX__

// these functions are in src/osx/utils_base.mm for wxOSX.
#ifndef __DARWIN__

wxOperatingSystemId wxGetOsVersion(int *verMaj, int *verMin, int *verMicro)
{
    // get OS version
    int major = -1, minor = -1, micro = -1;
#ifdef __VMS
    wxString release = wxGetCommandOutput(wxT("uname -v"));
#else
    wxString release = wxGetCommandOutput(wxT("uname -r"));
#endif
    if ( !release.empty() )
    {
#ifdef __VMS
        if ( wxSscanf(release.c_str(), wxT("V%d.%d-%d"), &major, &minor, &micro ) != 3 )
#else
        if ( wxSscanf(release.c_str(), wxT("%d.%d.%d"), &major, &minor, &micro ) != 3 )
#endif
        {
            micro = 0;
#ifdef __VMS
            if ( wxSscanf(release.c_str(), wxT("V%d.%d"), &major, &minor ) != 2 )
#else
            if ( wxSscanf(release.c_str(), wxT("%d.%d"), &major, &minor ) != 2 )
#endif
            {
                // failed to get version string or unrecognized format
                major = minor = micro = -1;
            }
        }
    }

    if ( verMaj )
        *verMaj = major;
    if ( verMin )
        *verMin = minor;
    if ( verMicro )
        *verMicro = micro;

    // try to understand which OS are we running
    wxString kernel = wxGetCommandOutput(wxT("uname -s"));
    if ( kernel.empty() )
        kernel = wxGetCommandOutput(wxT("uname -o"));

    if ( kernel.empty() )
        return wxOS_UNKNOWN;

    return wxPlatformInfo::GetOperatingSystemId(kernel);
}

wxString wxGetOsDescription()
{
#ifdef __VMS
    return wxGetCommandOutput(wxT("uname -s -v -m"));
#else
    return wxGetCommandOutput(wxT("uname -s -r -m"));
#endif
}

bool wxCheckOsVersion(int majorVsn, int minorVsn, int microVsn)
{
    int majorCur, minorCur, microCur;
    wxGetOsVersion(&majorCur, &minorCur, &microCur);

    return majorCur > majorVsn
        || (majorCur == majorVsn && minorCur >= minorVsn)
        || (majorCur == majorVsn && minorCur == minorVsn && microCur >= microVsn);
}

#endif // !__DARWIN__

unsigned long wxGetProcessId()
{
    return (unsigned long)getpid();
}

wxMemorySize wxGetFreeMemory()
{
#if defined(__LINUX__)
    // get it from /proc/meminfo
    FILE *fp = fopen("/proc/meminfo", "r");
    if ( fp )
    {
        wxMemorySize memFreeBytes = (wxMemorySize)-1;

        char buf[1024];
        if ( fgets(buf, WXSIZEOF(buf), fp) && fgets(buf, WXSIZEOF(buf), fp) )
        {
            // /proc/meminfo changed its format in kernel 2.6
            if ( wxPlatformInfo().CheckOSVersion(2, 6) )
            {
                unsigned long memFree;
                if ( sscanf(buf, "MemFree: %lu", &memFree) == 1 )
                {
                    // We consider memory used by the IO buffers and cache as
                    // being "free" too as Linux aggressively uses free memory
                    // for caching and the amount of memory reported as really
                    // free is far too low for lightly loaded system.
                    if ( fgets(buf, WXSIZEOF(buf), fp) )
                    {
                        unsigned long buffers;
                        if ( sscanf(buf, "Buffers: %lu", &buffers) == 1 )
                            memFree += buffers;
                    }

                    if ( fgets(buf, WXSIZEOF(buf), fp) )
                    {
                        unsigned long cached;
                        if ( sscanf(buf, "Cached: %lu", &cached) == 1 )
                        {
                            if ( cached > ULONG_MAX-memFree )
                                memFree = ULONG_MAX;
                            else
                                memFree += cached;
                        }
                    }

                    // values here are always expressed in kB and we want bytes
                    memFreeBytes = memFree;
                    memFreeBytes *= 1024;
                }
            }
            else // Linux 2.4 (or < 2.6, anyhow)
            {
                long memTotal, memUsed, memFree;
                if ( sscanf(buf, "Mem: %ld %ld %ld",
                            &memTotal, &memUsed, &memFree) == 3 )
                {
                    memFreeBytes = memFree;
                }
            }
        }

        fclose(fp);

        return memFreeBytes;
    }
#elif defined(__SGI__)
    struct rminfo realmem;
    if ( sysmp(MP_SAGET, MPSA_RMINFO, &realmem, sizeof realmem) == 0 )
        return ((wxMemorySize)realmem.physmem * sysconf(_SC_PAGESIZE));
#elif defined(_SC_AVPHYS_PAGES)
    return ((wxMemorySize)sysconf(_SC_AVPHYS_PAGES))*sysconf(_SC_PAGESIZE);
//#elif defined(__FREEBSD__) -- might use sysctl() to find it out, probably
#endif

    // can't find it out
    return -1;
}

bool wxGetDiskSpace(const wxString& path, wxDiskspaceSize_t *pTotal, wxDiskspaceSize_t *pFree)
{
#if defined(HAVE_STATFS) || defined(HAVE_STATVFS)
    // the case to "char *" is needed for AIX 4.3
    wxStatfs_t fs;
    if ( wxStatfs(const_cast<char*>(static_cast<const char*>(path.fn_str())), &fs) != 0 )
    {
        wxLogSysError( wxT("Failed to get file system statistics") );

        return false;
    }

    // under Solaris we also have to use f_frsize field instead of f_bsize
    // which is in general a multiple of f_frsize
#ifdef HAVE_STATVFS
    wxDiskspaceSize_t blockSize = fs.f_frsize;
#else // HAVE_STATFS
    wxDiskspaceSize_t blockSize = fs.f_bsize;
#endif // HAVE_STATVFS/HAVE_STATFS

    if ( pTotal )
    {
        *pTotal = wxDiskspaceSize_t(fs.f_blocks) * blockSize;
    }

    if ( pFree )
    {
        *pFree = wxDiskspaceSize_t(fs.f_bavail) * blockSize;
    }

    return true;
#else // !HAVE_STATFS && !HAVE_STATVFS
    return false;
#endif // HAVE_STATFS
}

// ----------------------------------------------------------------------------
// env vars
// ----------------------------------------------------------------------------

#if USE_PUTENV

#include <unordered_map>

static std::unordered_map<wxString, char*> gs_envVars;

class wxSetEnvModule : public wxModule
{
public:
    virtual bool OnInit() { return true; }
    virtual void OnExit()
    {
        for ( auto i = gs_envVars.begin();
              i != gs_envVars.end();
              ++i )
        {
            free(i->second);
        }

        gs_envVars.clear();
    }

    wxDECLARE_DYNAMIC_CLASS(wxSetEnvModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxSetEnvModule, wxModule);

#endif // USE_PUTENV

bool wxGetEnv(const wxString& var, wxString *value)
{
    // wxGetenv is defined as getenv()
    char *p = wxGetenv(var);
    if ( !p )
        return false;

    if ( value )
    {
        *value = p;
    }

    return true;
}

static bool wxDoSetEnv(const wxString& variable, const char *value)
{
#if defined(HAVE_SETENV)
    if ( !value )
    {
#ifdef HAVE_UNSETENV
        // don't test unsetenv() return value: it's void on some systems (at
        // least Darwin)
        unsetenv(variable.mb_str());
        return true;
#else
        value = ""; // we can't pass nullptr to setenv()
#endif
    }

    return setenv(variable.mb_str(), value, 1 /* overwrite */) == 0;
#elif defined(HAVE_PUTENV)
    wxString s = variable;
    if ( value )
        s << wxT('=') << value;

    // transform to ANSI
    const wxWX2MBbuf p = s.mb_str();

    char *buf = (char *)malloc(strlen(p) + 1);
    strcpy(buf, p);

    // store the string to free() it later
    wxEnvVars::iterator i = gs_envVars.find(variable);
    if ( i != gs_envVars.end() )
    {
        free(i->second);
        i->second = buf;
    }
    else // this variable hadn't been set before
    {
        gs_envVars[variable] = buf;
    }

    return putenv(buf) == 0;
#else // no way to set an env var
    return false;
#endif
}

bool wxSetEnv(const wxString& variable, const wxString& value)
{
    return wxDoSetEnv(variable, value.mb_str());
}

bool wxUnsetEnv(const wxString& variable)
{
    return wxDoSetEnv(variable, nullptr);
}

// ----------------------------------------------------------------------------
// signal handling
// ----------------------------------------------------------------------------

#if wxUSE_ON_FATAL_EXCEPTION

#include <signal.h>

extern "C" {
static void wxFatalSignalHandler(int WXUNUSED(signal))
{
    if ( wxTheApp )
    {
        // give the user a chance to do something special about this
        wxTheApp->OnFatalException();
    }

    abort();
}
}

bool wxHandleFatalExceptions(bool doit)
{
    // old sig handlers
    static bool s_savedHandlers = false;
    static struct sigaction s_handlerFPE,
                            s_handlerILL,
                            s_handlerBUS,
                            s_handlerSEGV;

    bool ok = true;
    if ( doit && !s_savedHandlers )
    {
        // install the signal handler
        struct sigaction act;

        // some systems extend it with non std fields, so zero everything
        memset(&act, 0, sizeof(act));

        act.sa_handler = wxFatalSignalHandler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;

        ok &= sigaction(SIGFPE, &act, &s_handlerFPE) == 0;
        ok &= sigaction(SIGILL, &act, &s_handlerILL) == 0;
        ok &= sigaction(SIGBUS, &act, &s_handlerBUS) == 0;
        ok &= sigaction(SIGSEGV, &act, &s_handlerSEGV) == 0;
        if ( !ok )
        {
            wxLogDebug(wxT("Failed to install our signal handler."));
        }

        s_savedHandlers = true;
    }
    else if ( s_savedHandlers )
    {
        // uninstall the signal handler
        ok &= sigaction(SIGFPE, &s_handlerFPE, nullptr) == 0;
        ok &= sigaction(SIGILL, &s_handlerILL, nullptr) == 0;
        ok &= sigaction(SIGBUS, &s_handlerBUS, nullptr) == 0;
        ok &= sigaction(SIGSEGV, &s_handlerSEGV, nullptr) == 0;
        if ( !ok )
        {
            wxLogDebug(wxT("Failed to uninstall our signal handler."));
        }

        s_savedHandlers = false;
    }
    //else: nothing to do

    return ok;
}

#endif // wxUSE_ON_FATAL_EXCEPTION

// ----------------------------------------------------------------------------
// wxExecute support
// ----------------------------------------------------------------------------

int wxAppTraits::WaitForChild(wxExecuteData& execData)
{
#if wxUSE_CONSOLE_EVENTLOOP
    wxConsoleEventLoop loop;
    return RunLoopUntilChildExit(execData, loop);
#else // !wxUSE_CONSOLE_EVENTLOOP
    wxFAIL_MSG( wxS("Can't wait for child process without wxConsoleEventLoop") );

    return -1;
#endif // wxUSE_CONSOLE_EVENTLOOP/!wxUSE_CONSOLE_EVENTLOOP
}

// This function is common code for both console and GUI applications and used
// by wxExecute() to wait for the child exit while dispatching events.
//
// Notice that it should not be used for all the other cases, e.g. when we
// don't need to wait for the child (wxEXEC_ASYNC) or when the events must not
// dispatched (wxEXEC_NOEVENTS).
int
wxAppTraits::RunLoopUntilChildExit(wxExecuteData& execData,
                                   wxEventLoopBase& loop)
{
    // It is possible that wxExecuteData::OnExit() had already been called
    // and reset the PID to 0, in which case we don't need to do anything
    // at all.
    if ( !execData.m_pid )
        return execData.m_exitcode;

#if wxUSE_STREAMS
    // Monitor the child streams if necessary.
    std::unique_ptr<wxEventLoopSourceHandler>
        stdoutHandler,
        stderrHandler;
    if ( execData.IsRedirected() )
    {
        stdoutHandler.reset(new wxExecuteEventLoopSourceHandler
                                (
                                    execData.m_fdOut, execData.m_bufOut
                                ));
        stderrHandler.reset(new wxExecuteEventLoopSourceHandler
                                (
                                    execData.m_fdErr, execData.m_bufErr
                                ));
    }
#endif // wxUSE_STREAMS

    // Store the event loop in the data associated with the child
    // process so that it could exit the loop when the child exits.
    execData.m_syncEventLoop = &loop;

    // And run it.
    loop.Run();

    // The exit code will have been set when the child termination was detected.
    return execData.m_exitcode;
}

// ----------------------------------------------------------------------------
// wxExecuteData
// ----------------------------------------------------------------------------

namespace
{

// Helper function that checks whether the child with the given PID has exited
// and fills the provided parameter with its return code if it did.
bool CheckForChildExit(int pid, int* exitcodeOut)
{
    wxASSERT_MSG( pid > 0, "invalid PID" );

    int status, rc;

    // loop while we're getting EINTR
    for ( ;; )
    {
        rc = waitpid(pid, &status, WNOHANG);

        if ( rc != -1 || errno != EINTR )
            break;
    }

    switch ( rc )
    {
        case 0:
            // No error but the child is still running.
            return false;

        case -1:
            // Checking child status failed. Invalid PID?
            wxLogLastError(wxString::Format("waitpid(%d)", pid));
            return false;

        default:
            // Child did terminate.
            wxASSERT_MSG( rc == pid, "unexpected waitpid() return value" );

            // notice that the caller expects the exit code to be signed, e.g. -1
            // instead of 255 so don't assign WEXITSTATUS() to an int
            signed char exitcode;
            if ( WIFEXITED(status) )
                exitcode = WEXITSTATUS(status);
            else if ( WIFSIGNALED(status) )
                exitcode = -WTERMSIG(status);
            else
            {
                wxLogError("Child process (PID %d) exited for unknown reason, "
                           "status = %d", pid, status);
                exitcode = -1;
            }

            if ( exitcodeOut )
                *exitcodeOut = exitcode;

            return true;
    }
}

} // anonymous namespace

wxExecuteData::ChildProcessesData wxExecuteData::ms_childProcesses;

/* static */
void wxExecuteData::OnSomeChildExited(int WXUNUSED(sig))
{
    // We know that some child process has terminated, but we don't know which
    // one, so check all of them (notice that more than one could have exited).
    //
    // An alternative approach would be to call waitpid(-1, &status, WNOHANG)
    // (in a loop to take care of the multiple children exiting case) and
    // perhaps this would be more efficient. But for now this seems to work.


    // Make a copy of the list before iterating over it to avoid problems due
    // to deleting entries from it in the process.
    const ChildProcessesData allChildProcesses = ms_childProcesses;
    for ( const auto& kv : allChildProcesses )
    {
        const int pid = kv.first;

        // Check whether this child exited.
        int exitcode;
        if ( !CheckForChildExit(pid, &exitcode) )
            continue;

        // And handle its termination if it did.
        //
        // Notice that this will implicitly remove it from ms_childProcesses.
        kv.second->OnExit(exitcode);
    }
}

void wxExecuteData::OnStart(int pid)
{
    wxCHECK_RET( wxTheApp,
                 wxS("Ensure wxTheApp is set before calling wxExecute()") );

    // Setup the signal handler for SIGCHLD to be able to detect the child
    // termination.
    //
    // Notice that SetSignalHandler() is idempotent, so it's fine to call
    // it more than once with the same handler.
    wxTheApp->SetSignalHandler(SIGCHLD, OnSomeChildExited);


    // Remember the child PID to be able to wait for it later.
    m_pid = pid;

    // Also save it in wxProcess where it will be accessible to the user code.
    if ( m_process )
        m_process->SetPid(m_pid);

    // Add this object itself to the list of child processes so that
    // we can check for its termination the next time we get SIGCHLD.
    ms_childProcesses[m_pid] = this;

    // However, if the child exited before we finished setting up above,
    // we may have already missed its SIGCHLD.  So we also do an explicit
    // check here before returning.
    int exitcode;
    if ( CheckForChildExit(m_pid, &exitcode) )
    {
        // Handle its termination if it did.
        // This call will implicitly remove it from ms_childProcesses
        // and, if running asynchronously, it will delete itself.
        OnExit(exitcode);
    }
}

void wxExecuteData::OnExit(int exitcode)
{
    // Remove this process from the hash list of child processes that are
    // still open as soon as possible to ensure we don't process it again even
    // if another SIGCHLD happens.
    if ( !ms_childProcesses.erase(m_pid) )
    {
        wxFAIL_MSG(wxString::Format(wxS("Data for PID %d not in the list?"), m_pid));
    }


    m_exitcode = exitcode;

#if wxUSE_STREAMS
    if ( IsRedirected() )
    {
        // Read the remaining data in a blocking way: this is fine because the
        // child has already exited and hence all the data must be already
        // available in the streams buffers.
        m_bufOut.ReadAll();
        m_bufErr.ReadAll();
    }
#endif // wxUSE_STREAMS

    // Notify user about termination if required
    if ( !(m_flags & wxEXEC_SYNC) )
    {
        if ( m_process )
            m_process->OnTerminate(m_pid, m_exitcode);

        // in case of asynchronous execution we don't need this object any more
        // after the child terminates
        delete this;
    }
    else // sync execution
    {
        // let wxExecute() know that the process has terminated
        m_pid = 0;

        // Stop the event loop for synchronous wxExecute() if we're running one.
        if ( m_syncEventLoop )
            m_syncEventLoop->ScheduleExit();
    }
}

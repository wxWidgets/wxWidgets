/////////////////////////////////////////////////////////////////////////////
// Name:        utilsexec.cpp
// Purpose:     Execution-related utilities
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma implementation
#endif

#include "wx/utils.h"
#include "wx/app.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef VMS
/*steve*/
#ifdef __HIDE_FORBIDDEN_NAMES
#undefine __HIDE_FORBIDDEN_NAMES
#endif
#include <socket.h>
#ifdef VAX
/*because 'noshare' is not valid in vax C++*/
#define CC$VAXCSHR 1
#endif
#include <unixlib.h>
#define unlink DELETE

#else

#if defined(_AIX) || defined(__xlC__)
#include <sys/socket.h>
#include <sys/select.h>
#else
#ifndef DG
#include <sys/syscall.h>
#endif
#endif

#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>

#endif

#include <sys/time.h>

#include <Xm/Xm.h>

struct wxLocalProcessData
{
  int pid, end_process;
  wxProcess *process;
};

#ifdef __SOLARIS__
// somehow missing from sys/wait.h but in the system's docs
extern "C"
{
   pid_t wait4(pid_t pid, int *statusp, int options, struct rusage
               *rusage);
}
#endif

void xt_notify_end_process(XtPointer client, int *fid,
			   XtInputId *id)
{
  wxLocalProcessData *process_data = (wxLocalProcessData *)client;

  int pid;

  pid = (process_data->pid > 0) ? process_data->pid : -(process_data->pid);

  /* wait4 is not part of any standard, use at own risk
   * not sure what wait4 does, but wait3 seems to be closest, whats a digit ;-)
   * --- offer@sgi.com */
#if !defined(__sgi)
  wait4(process_data->pid, NULL, 0, NULL);
#else
  wait3((int *) NULL, 0, (rusage *) NULL);
#endif

  XtRemoveInput(*id);
  if (process_data->process)
    process_data->process->OnTerminate(process_data->pid);

  process_data->end_process = TRUE;

  if (process_data->pid > 0)
    delete process_data;
  else
    process_data->pid = 0;
}

long wxExecute(char **argv, bool sync, wxProcess *handler)
{
#ifdef VMS
  return(0);
#else
  if (*argv == NULL)
    return 0;	// Nothing???

  int proc_link[2];
  if (pipe(proc_link))
    return 0;

  /* fork the process */
#if defined(sun) || defined(__ultrix) || defined(__bsdi__)
  pid_t pid = vfork ();
#else
  pid_t pid = fork ();
#endif

  if (pid == -1)
    {
      return 0;
    }
  else if (pid == 0)
    {
/* GUILHEM: Close all fds when sync == 0 */
      if (sync == 0)
        for (int fd=0;fd<FD_SETSIZE;fd++) {
          if (proc_link[1] != fd)
            close(fd);
        }
      /* child */
#ifdef _AIX
      execvp ((const char *)*argv, (const char **)argv);
#else
      execvp (*argv, argv);
#endif
/* GUILHEM: Reopen output stream */
//      open("/dev/console", O_WRONLY);
/* GUILHEM: End */
      if (errno == ENOENT)
	printf ("%s: command not found\n", *argv);
      else
	perror (*argv);
      printf ("wxWindows: could not execute '%s'\n", *argv);
      _exit (-1);
    }

  wxLocalProcessData *process_data = new wxLocalProcessData;

  process_data->end_process = 0;
  process_data->process = handler;
  process_data->pid = (sync) ? pid : -pid;

  close(proc_link[1]);
  XtAppAddInput((XtAppContext) wxTheApp->GetAppContext(), proc_link[0],
                (XtPointer *) XtInputReadMask,
                (XtInputCallbackProc) xt_notify_end_process,
                (XtPointer) process_data);

  if (sync) {
    while (!process_data->end_process)
       XtAppProcessEvent((XtAppContext) wxTheApp->GetAppContext(), XtIMAll);

    if (WIFEXITED(process_data->end_process) != 0)
      return WEXITSTATUS(process_data->end_process);
  }

  delete process_data;

  return pid;
#endif
  // end VMS
}

long wxExecute (const wxString& command, bool sync, wxProcess* handler)
{
#ifdef VMS
  return(0);
#else
  if (command.IsNull() || command == "")
    return 0; // Nothing to do

  // Run a program the recomended way under X (XView) 
  int argc = 0;
  char *argv[127];
  char tmp[1024];
  const char *IFS = " \t\n";

  // Build argument vector 
  strncpy (tmp, (const char*) command, sizeof (tmp) / sizeof (char) - 1);
  tmp[sizeof (tmp) / sizeof (char) - 1] = '\0';
  argv[argc++] = strtok (tmp, IFS);
  while ((argv[argc++] = strtok (NULL, IFS)) != NULL)
    /* loop */ ;

  return wxExecute(argv, sync, handler);
#endif
  // VMS
}

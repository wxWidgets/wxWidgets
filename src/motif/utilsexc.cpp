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

#define wxEXECUTE_WIN_MESSAGE 10000

void xt_notify_end_process(XtPointer client, int *fid,
			   XtInputId *id)
{
  Bool *flag = (Bool *) client;
  *flag = TRUE;

  XtRemoveInput(*id);
}

long wxExecute(char **argv, bool sync, wxProcess *WXUNUSED(handler))
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

  int end_process = 0;

  close(proc_link[1]);
  XtAppAddInput((XtAppContext) wxTheApp->GetAppContext(), proc_link[0],
                (XtPointer *) XtInputReadMask,
                (XtInputCallbackProc) xt_notify_end_process,
                (XtPointer) &end_process);

  if (sync) {
    while (!end_process)
       XtAppProcessEvent((XtAppContext) wxTheApp->GetAppContext(), XtIMAll);

    if (WIFEXITED(end_process) != 0)
      return WEXITSTATUS(end_process);
  }

  return pid;
#endif
  // end VMS
}

long wxExecute (const wxString& command, bool sync, wxProcess* WXUNUSED(process))
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

  return wxExecute(argv, sync);
#endif
  // VMS
}

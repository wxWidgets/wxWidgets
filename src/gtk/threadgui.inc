/////////////////////////////////////////////////////////////////////////////
// Name:        threadgui.inc
// Purpose:     GUI thread manager for GTK
// Author:      Original from Wolfram Gloger/Guilhem Lavaux
// Modified by:
// Created:     04/22/98
// RCS-ID:      $Id$
// Copyright:   (c) Wolfram Gloger (1996, 1997); Guilhem Lavaux (1998)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <unistd.h>

// for select()
#include <sys/time.h>
#include <sys/types.h>
#ifdef __sgi
#include <bstring.h>
#endif

#include <gdk/gdk.h>

/////////////////////////////////////////////////////////////////////////////
// Static variables
/////////////////////////////////////////////////////////////////////////////

static int p_thrd_pipe[2] = { -1, -1 };
// WorkProc in GTK
static gint p_thrd_inid;

#define THREAD_SEND_EXIT_MSG(ptr) write(p_thrd_pipe[1], &ptr, sizeof(ptr));

static void
ThreadExitProc(gpointer WXUNUSED(client), gint fid,
               GdkInputCondition WXUNUSED(cond))
{
  wxThread* ptr;

  if (fid != p_thrd_pipe[0])
    return;
  if (read(fid, &ptr, sizeof(ptr)) == sizeof(ptr)) {
    //fprintf(stderr, "calling OnExit %p\n", ptr);
    ptr->OnExit();
  } else {
    //fprintf(stderr, "this should never happen\n");
  }
}

// Global initialization
static void wxThreadGuiInit()
{
  pipe(p_thrd_pipe);
  p_thrd_inid = gdk_input_add(p_thrd_pipe[0], GDK_INPUT_READ,
                              ThreadExitProc, 0);
}

// Global cleanup
static void wxThreadGuiExit()
{
  gdk_input_remove(p_thrd_inid);
  close(p_thrd_pipe[0]);
  close(p_thrd_pipe[1]);
}

#ifdef NO_DEFINE_GDK_1_1

void wxMutexGuiEnter()
{
  gdk_mutex_enter();
}

void wxMutexGuiLeave()
{
  gdk_mutex_leave();
}

#else

void wxMutexGuiEnter()
{
  wxMainMutex.Lock();
}

void wxMutexGuiLeave()
{
  wxMainMutex.Unlock();
}

#endif

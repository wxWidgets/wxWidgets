//
// $Header$
//
// timers.i
// A SWIG file for adding various timing functions. 
// Really, this is modeled after the timers in the CMMD
// message passing library for the CM-5.
// 
// Dave Beazley
// April 2, 1996
//
/* Revision history 
 * $Log$
 * Revision 1.1  2002/04/29 19:56:49  RD
 * Since I have made several changes to SWIG over the years to accomodate
 * special cases and other things in wxPython, and since I plan on making
 * several more, I've decided to put the SWIG sources in wxPython's CVS
 * instead of relying on maintaining patches.  This effectivly becomes a
 * fork of an obsolete version of SWIG, :-( but since SWIG 1.3 still
 * doesn't have some things I rely on in 1.1, not to mention that my
 * custom patches would all have to be redone, I felt that this is the
 * easier road to take.
 *
 * Revision 1.1.1.1  1999/02/28 02:00:53  beazley
 * Swig1.1
 *
 * Revision 1.1  1996/05/22 17:27:01  beazley
 * Initial revision
 *
 */

%module timers
%{

#include <time.h>
#define  SWIG_NTIMERS     64

static clock_t  telapsed[SWIG_NTIMERS];
static clock_t  tstart[SWIG_NTIMERS];
static clock_t  tend[SWIG_NTIMERS];

/*-----------------------------------------------------------------
 * SWIG_timer_clear(int i)
 *
 * Clears timer i.
 *----------------------------------------------------------------- */

void
SWIG_timer_clear(int i)
{
  if ((i >= 0) && (i < SWIG_NTIMERS))
   telapsed[i] = 0;
}


/*-----------------------------------------------------------------
 * SWIG_timer_start(int i)
 *
 * Starts timer i
 *----------------------------------------------------------------- */

void
SWIG_timer_start(int i)
{
  if ((i >= 0) && (i < SWIG_NTIMERS))
    tstart[i] = clock();
}


/*-----------------------------------------------------------------
 * SWIG_timer_stop(int i)
 *
 * Stops timer i and accumulates elapsed time
 *----------------------------------------------------------------- */

void
SWIG_timer_stop(int i)
{
  if ((i >= 0) && (i < SWIG_NTIMERS)) {
    tend[i] = clock();
    telapsed[i] += (tend[i] - tstart[i]);
  }
}

/*-----------------------------------------------------------------
 * SWIG_timer_elapsed(int i)
 *
 * Returns the time elapsed on timer i in seconds.
 *----------------------------------------------------------------- */

double
SWIG_timer_elapsed(int i)
{
  double   t;
  if ((i >= 0) && (i < SWIG_NTIMERS)) {
    t = (double) telapsed[i]/(double) CLOCKS_PER_SEC;
    return(t);
  } else {
    return 0;
  }
}

%}

%section "Timer Functions",pre,after,chop_left=3,nosort,info,chop_right = 0, chop_top=0,chop_bottom=0

%text %{
%include timers.i

This module provides a collection of timing functions designed for
performance analysis and benchmarking of different code fragments. 

A total of 64 different timers are available.   Each timer can be
managed independently using four functions :

    timer_clear(int n)          Clears timer n
    timer_start(int n)          Start timer n
    timer_stop(int n)           Stop timer n
    timer_elapsed(int n)        Return elapsed time (in seconds)

All timers measure CPU time.

Since each timer can be accessed independently, it is possible
to use groups of timers for measuring different aspects of code
performance.   To use a timer, simply use code like this :
%}

#if defined(SWIGTCL)
%text %{
      timer_clear 0
      timer_start 0
      .. a bunch of Tcl code ...
      timer_stop 0
      puts "[timer_elapsed 0] seconds of CPU time"
%}
#elif defined(SWIGPERL)
%text %{
      timer_clear(0);
      timer_start(0);
      .. a bunch of Perl code ...
      timer_stop(0);
      print timer_elapsed(0)," seconds of CPU time\n";
%}
#elif defined(SWIGPYTHON)
%text %{
      timer_clear(0)
      timer_start(0)
      ... a bunch of Python code ...
      timer_stop(0)
      print timer_elapsed(0)," seconds of CPU time"
%}      
#endif

%text %{
A single timer can be stopped and started repeatedly to provide
a cummulative timing effect.

As a general performance note, making frequent calls to the timing
functions can severely degrade performance (due to operating system
overhead).   The resolution of the timers may be poor for extremely
short code fragments.   Therefore, the timers work best for
computationally intensive operations.
%}


%name(timer_clear)   void SWIG_timer_clear(int n);   
/* Clears timer n. */

%name(timer_start)   void SWIG_timer_start(int n);   
/* Starts timer n. */

%name(timer_stop)    void SWIG_timer_stop(int n);    
/* Stops timer n. */

%name(timer_elapsed) double SWIG_timer_elapsed(int n); 
/* Return the elapsed time (in seconds) of timer n */





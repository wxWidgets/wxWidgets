//
// $Header$
//
// SWIG File for making wish
// Dave Beazley
// April 25, 1996
//
/* Revision History
 * $Log$
 * Revision 1.1  2002/04/29 19:56:57  RD
 * Since I have made several changes to SWIG over the years to accomodate
 * special cases and other things in wxPython, and since I plan on making
 * several more, I've decided to put the SWIG sources in wxPython's CVS
 * instead of relying on maintaining patches.  This effectivly becomes a
 * fork of an obsolete version of SWIG, :-( but since SWIG 1.3 still
 * doesn't have some things I rely on in 1.1, not to mention that my
 * custom patches would all have to be redone, I felt that this is the
 * easier road to take.
 *
 * Revision 1.2  1999/11/05 21:45:14  beazley
 * Minor Changes
 *
 * Revision 1.1.1.1  1999/02/28 02:00:56  beazley
 * Swig1.1
 *
 * Revision 1.1  1996/05/22 19:47:45  beazley
 * Initial revision
 *
 */

#ifdef AUTODOC
%subsection "wish.i"
%text %{
This module provides the Tk_AppInit() function needed to build a 
new version of the wish executable.   Like tclsh.i, this file should
not be used with dynamic loading.  To make an interface file work with
both static and dynamic loading, put something like this in your
interface file :

     #ifdef STATIC
     %include wish.i
     #endif

A startup file may be specified by defining the symbol SWIG_RcFileName
as follows (this should be included in a code-block) :

     #define SWIG_RcFileName    "~/.mywishrc"
%}
#endif

%{


/* Initialization code for wish */

#include <tk.h>

#ifndef SWIG_RcFileName
char *SWIG_RcFileName = "~/.wishrc";
#endif

#ifdef MAC_TCL
extern int	MacintoshInit _ANSI_ARGS_((void));
extern int	SetupMainInterp _ANSI_ARGS_((Tcl_Interp *interp));
#endif

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AppInit --
 *
 *	This procedure performs application-specific initialization.
 *	Most applications, especially those that incorporate additional
 *	packages, will have their own version of this procedure.
 *
 * Results:
 *	Returns a standard Tcl completion code, and leaves an error
 *	message in interp->result if an error occurs.
 *
 * Side effects:
 *	Depends on the startup script.
 *
 *----------------------------------------------------------------------
 */

int Tcl_AppInit(Tcl_Interp *interp)
{
#ifndef MAC_TCL
    Tk_Window main;
    main = Tk_MainWindow(interp);
#endif
    /*
     * Call the init procedures for included packages.  Each call should
     * look like this:
     *
     * if (Mod_Init(interp) == TCL_ERROR) {
     *     return TCL_ERROR;
     * }
     *
     * where "Mod" is the name of the module.
     */

    if (Tcl_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }

    if (Tk_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }

    /*
     * Call Tcl_CreateCommand for application-specific commands, if
     * they weren't already created by the init procedures called above.
     */

    if (SWIG_init(interp) == TCL_ERROR) {
      return TCL_ERROR;
    }
    
#ifdef MAC_TCL
    SetupMainInterp(interp);
#endif
        
    /*
     * Specify a user-specific startup file to invoke if the application
     * is run interactively.  Typically the startup file is "~/.apprc"
     * where "app" is the name of the application.  If this line is deleted
     * then no user-specific startup file will be run under any conditions.
     */

#if TCL_MAJOR_VERSION >= 8 || TCL_MAJOR_VERSION == 7 && TCL_MINOR_VERSION >= 5
   Tcl_SetVar(interp,"tcl_rcFileName",SWIG_RcFileName,TCL_GLOBAL_ONLY);
#else
   tcl_RcFileName = SWIG_RcFileName;
#endif

/* For Macintosh might also want this */

#ifdef MAC_TCL
#ifdef SWIG_RcRsrcName
    Tcl_SetVar(interp,"tcl_rcRsrcName",SWIG_RcRsrcName,TCL_GLOBAL_ONLY);
#endif
#endif
    return TCL_OK;
}

#if TK_MAJOR_VERSION >= 4
int main(int argc, char **argv) {

#ifdef MAC_TCL
  char *newArgv[2];
  if (MacintoshInit() != TCL_OK) {
      Tcl_Exit(1);
  }
  argc = 1;
  newArgv[0] = "Wish";
  newArgv[1] = NULL;
  argv = newArgv;
#endif
  Tk_Main(argc, argv, Tcl_AppInit);
  return(0);
}
#else
extern int main();
#endif

%}




//
// SWIG Interface file for building a new version of itclsh
// Dave Beazley
// August 14, 1996
//


#ifdef AUTODOC
%subsection "itclsh.i"
%text %{
This module provides a main() program needed to build a new version
of the [incr Tcl] 'itclsh' executable.  It has been tested with itcl 2.1,
but may need tweaking for later versions and for use with C++.
%}
#endif

%{

/* 
 * tclAppInit.c --
 *
 *	Provides a default version of the main program and Tcl_AppInit
 *	procedure for Tcl applications (without Tk).
 *
 * Copyright (c) 1993 The Regents of the University of California.
 * Copyright (c) 1994-1995 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

static char sccsid[] = "@(#) tclAppInit.c 1.13 95/06/08 10:55:54";

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef SWIG_RcFileName
char *SWIG_RcFileName = "~/.tclshrc";
#endif

extern int		Itcl_Init _ANSI_ARGS_((Tcl_Interp *interp));

/*
 * The following variable is a special hack that is needed in order for
 * Sun shared libraries to be used for Tcl.
 */

extern int matherr _ANSI_ARGS_((void));
static int (*dummyMathPtr) _ANSI_ARGS_((void)) = matherr;

#ifdef	__cplusplus
}
#endif


/*
 *----------------------------------------------------------------------
 *
 * main --
 *
 *	This is the main program for the application.
 *
 * Results:
 *	None: Tcl_Main never returns here, so this procedure never
 *	returns either.
 *
 * Side effects:
 *	Whatever the application does.
 *
 *----------------------------------------------------------------------
 */

int
main(argc, argv)
    int argc;			/* Number of command-line arguments. */
    char **argv;		/* Values of command-line arguments. */
{
    Tcl_Main(argc, argv, Tcl_AppInit);
    return 0;			/* Needed only to prevent compiler warning. */
}

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

int
Tcl_AppInit(interp)
    Tcl_Interp *interp;		/* Interpreter for application. */
{
    if (Tcl_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }

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

    if (Itcl_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }
    if (SWIG_init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }	

#if (TCL_MAJOR_VERSION > 7) || (TCL_MINOR_VERSION > 4)
    Tcl_StaticPackage(interp, "Itcl", Itcl_Init, (Tcl_PackageInitProc *) NULL);
#endif

    /*
     * Call Tcl_CreateCommand for application-specific commands, if
     * they weren't already created by the init procedures called above.
     */

    /*
     * Specify a user-specific startup file to invoke if the application
     * is run interactively.  Typically the startup file is "~/.apprc"
     * where "app" is the name of the application.  If this line is deleted
     * then no user-specific startup file will be run under any conditions.
     */

#if (TCL_MAJOR_VERSION > 7) || (TCL_MINOR_VERSION > 4)
    Tcl_SetVar(interp, "tcl_rcFileName", SWIG_RcFileName, TCL_GLOBAL_ONLY);
#else
    tcl_RcFileName = SWIG_RcFileName;
#endif
    return TCL_OK;
}

%}

//
// $Header$
//
// SWIG File for building new tclsh program
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
 * Revision 1.1.1.1  1999/02/28 02:00:56  beazley
 * Swig1.1
 *
 * Revision 1.1  1996/05/22 19:47:45  beazley
 * Initial revision
 *
 */

#ifdef AUTODOC
%subsection "tclsh.i"
%text %{
This module provides the Tcl_AppInit() function needed to build a 
new version of the tclsh executable.   This file should not be used
when using dynamic loading.   To make an interface file work with
both static and dynamic loading, put something like this in your
interface file :

     #ifdef STATIC
     %include tclsh.i
     #endif
%}
#endif

%{

/* A TCL_AppInit() function that lets you build a new copy
 * of tclsh.
 *
 * The macro SWIG_init contains the name of the initialization
 * function in the wrapper file.
 */

#ifndef SWIG_RcFileName
char *SWIG_RcFileName = "~/.myapprc";
#endif


#ifdef MAC_TCL
extern int		MacintoshInit _ANSI_ARGS_((void));
#endif

int Tcl_AppInit(Tcl_Interp *interp){

  if (Tcl_Init(interp) == TCL_ERROR) 
    return TCL_ERROR;

  /* Now initialize our functions */

  if (SWIG_init(interp) == TCL_ERROR)
    return TCL_ERROR;
#if TCL_MAJOR_VERSION > 7 || TCL_MAJOR_VERSION == 7 && TCL_MINOR_VERSION >= 5
   Tcl_SetVar(interp,"tcl_rcFileName",SWIG_RcFileName,TCL_GLOBAL_ONLY);
#else
   tcl_RcFileName = SWIG_RcFileName;
#endif
#ifdef SWIG_RcRsrcName
  Tcl_SetVar(interp,"tcl_rcRsrcName",SWIG_RcRsrcName,TCL_GLOBAL);
#endif
  
  return TCL_OK;
}

#if TCL_MAJOR_VERSION > 7 || TCL_MAJOR_VERSION == 7 && TCL_MINOR_VERSION >= 4
int main(int argc, char **argv) {
#ifdef MAC_TCL
    char *newArgv[2];
    
    if (MacintoshInit()  != TCL_OK) {
	Tcl_Exit(1);
    }

    argc = 1;
    newArgv[0] = "tclsh";
    newArgv[1] = NULL;
    argv = newArgv;
#endif

  Tcl_Main(argc, argv, Tcl_AppInit);
  return(0);

}
#else
extern int main();
#endif

%}


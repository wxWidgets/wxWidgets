/* 
 * tclMacAppInit.c --
 *
 *	Provides a version of the Tcl_AppInit procedure for the example shell.
 *
 * Copyright (c) 1993-1994 Lockheed Missle & Space Company, AI Center
 * Copyright (c) 1995-1997 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * SCCS: @(#) tclMacAppInit.c 1.17 97/01/21 18:13:34
 */

#include "tcl.h"
#include "tclInt.h"
#include "tclMacInt.h"

#if defined(THINK_C)
#   include <console.h>
#elif defined(__MWERKS__)
#   include <SIOUX.h>
short InstallConsole _ANSI_ARGS_((short fd));
#endif



/*
 *----------------------------------------------------------------------
 *
 * MacintoshInit --
 *
 *	This procedure calls initalization routines to set up a simple
 *	console on a Macintosh.  This is necessary as the Mac doesn't
 *	have a stdout & stderr by default.
 *
 * Results:
 *	Returns TCL_OK if everything went fine.  If it didn't the 
 *	application should probably fail.
 *
 * Side effects:
 *	Inits the appropiate console package.
 *
 *----------------------------------------------------------------------
 */

#ifdef __cpluscplus
extern "C"
#endif
extern int
MacintoshInit()
{
#if defined(THINK_C)

    /* Set options for Think C console package */
    /* The console package calls the Mac init calls */
    console_options.pause_atexit = 0;
    console_options.title = "\pTcl Interpreter";
		
#elif defined(__MWERKS__)

    /* Set options for CodeWarrior SIOUX package */
    SIOUXSettings.autocloseonquit = true;
    SIOUXSettings.showstatusline = true;
    SIOUXSettings.asktosaveonclose = false;
    InstallConsole(0);
    SIOUXSetTitle("\pTcl Interpreter");
		
#elif defined(applec)

    /* Init packages used by MPW SIOW package */
    InitGraf((Ptr)&qd.thePort);
    InitFonts();
    InitWindows();
    InitMenus();
    TEInit();
    InitDialogs(nil);
    InitCursor();
		
#endif

    TclMacSetEventProc((TclMacConvertEventPtr) SIOUXHandleOneEvent);
    
    /* No problems with initialization */
    return TCL_OK;
}

/* This is a support file needed to build a new version of Wish
   Normally, this capability is found in TkAppInit.c, but this creates
   tons of namespace problems for many applications. */
   
#include <Gestalt.h>
#include <ToolUtils.h>
#include <Fonts.h>
#include <Dialogs.h>
#include <SegLoad.h>
#include <Traps.h>

#include "tk.h"
#include "tkInt.h"
#include "tkMacInt.h"

typedef int (*TclMacConvertEventPtr) _ANSI_ARGS_((EventRecord *eventPtr));
Tcl_Interp *gStdoutInterp = NULL;

void 	TclMacSetEventProc _ANSI_ARGS_((TclMacConvertEventPtr procPtr));
int 	TkMacConvertEvent _ANSI_ARGS_((EventRecord *eventPtr));

/*
 * Prototypes for functions the ANSI library needs to link against.
 */
short			InstallConsole _ANSI_ARGS_((short fd));
void			RemoveConsole _ANSI_ARGS_((void));
long			WriteCharsToConsole _ANSI_ARGS_((char *buff, long n));
long			ReadCharsFromConsole _ANSI_ARGS_((char *buff, long n));
extern char *		__ttyname _ANSI_ARGS_((long fildes));
short			SIOUXHandleOneEvent _ANSI_ARGS_((EventRecord *event));

/*
 * Forward declarations for procedures defined later in this file:
 */

/*
 *----------------------------------------------------------------------
 *
 * MacintoshInit --
 *
 *	This procedure calls Mac specific initilization calls.  Most of
 *	these calls must be made as soon as possible in the startup
 *	process.
 *
 * Results:
 *	Returns TCL_OK if everything went fine.  If it didn't the 
 *	application should probably fail.
 *
 * Side effects:
 *	Inits the application.
 *
 *----------------------------------------------------------------------
 */

int
MacintoshInit()
{
    int i;
    long result, mask = 0x0700; 		/* mask = system 7.x */

    /*
     * Tk needs us to set the qd pointer it uses.  This is needed
     * so Tk doesn't have to assume the availablity of the qd global
     * variable.  Which in turn allows Tk to be used in code resources.
     */
    tcl_macQdPtr = &qd;

    InitGraf(&tcl_macQdPtr->thePort);
    InitFonts();
    InitWindows();
    InitMenus();
    InitDialogs((long) NULL);		
    InitCursor();

    /*
     * Make sure we are running on system 7 or higher
     */
     
    if ((NGetTrapAddress(_Gestalt, ToolTrap) == 
    	    NGetTrapAddress(_Unimplemented, ToolTrap))
    	    || (((Gestalt(gestaltSystemVersion, &result) != noErr)
	    || (mask != (result & mask))))) {
	panic("Tcl/Tk requires System 7 or higher.");
    }

    /*
     * Make sure we have color quick draw 
     * (this means we can't run on 68000 macs)
     */
     
    if (((Gestalt(gestaltQuickdrawVersion, &result) != noErr)
	    || (result < gestalt32BitQD13))) {
	panic("Tk requires Color QuickDraw.");
    }

    
    FlushEvents(everyEvent, 0);
    SetEventMask(everyEvent);

    /*
     * Set up stack & heap sizes
     */
    /* TODO: stack size
       size = StackSpace();
       SetAppLimit(GetAppLimit() - 8192);
     */
    MaxApplZone();
    for (i = 0; i < 4; i++) {
	(void) MoreMasters();
    }

    TclMacSetEventProc(TkMacConvertEvent);
    TkConsoleCreate();

    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * SetupMainInterp --
 *
 *	This procedure calls initalization routines require a Tcl 
 *	interp as an argument.  This call effectively makes the passed
 *	iterpreter the "main" interpreter for the application.
 *
 * Results:
 *	Returns TCL_OK if everything went fine.  If it didn't the 
 *	application should probably fail.
 *
 * Side effects:
 *	More initilization.
 *
 *----------------------------------------------------------------------
 */

int
SetupMainInterp(
    Tcl_Interp *interp)
{
    /*
     * Initialize the console only if we are running as an interactive
     * application.
     */

    TkMacInitAppleEvents(interp);
    TkMacInitMenus(interp);

    if (strcmp(Tcl_GetVar(interp, "tcl_interactive", TCL_GLOBAL_ONLY), "1")
	    == 0) {
	if (TkConsoleInit(interp) == TCL_ERROR) {
	    goto error;
	}
    }

    /*
     * Attach the global interpreter to tk's expected global console
     */

    gStdoutInterp = interp;

    return TCL_OK;

error:
    panic(interp->result);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * InstallConsole, RemoveConsole, etc. --
 *
 *	The following functions provide the UI for the console package.
 *	Users wishing to replace SIOUX with their own console package 
 *	need only provide the four functions below in a library.
 *
 * Results:
 *	See SIOUX documentation for details.
 *
 * Side effects:
 *	See SIOUX documentation for details.
 *
 *----------------------------------------------------------------------
 */

short 
InstallConsole(short fd)
{
#pragma unused (fd)

	return 0;
}

void 
RemoveConsole(void)
{
}

long 
WriteCharsToConsole(char *buffer, long n)
{
    TkConsolePrint(gStdoutInterp, TCL_STDOUT, buffer, n);
    return n;
}

long 
ReadCharsFromConsole(char *buffer, long n)
{
    return 0;
}

extern char *
__ttyname(long fildes)
{
    static char *__devicename = "null device";

    if (fildes >= 0 && fildes <= 2) {
	return (__devicename);
    }
    
    return (0L);
}

short
SIOUXHandleOneEvent(EventRecord *event)
{
    return 0;
}

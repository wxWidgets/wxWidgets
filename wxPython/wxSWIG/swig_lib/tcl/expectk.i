//
// $Header$
//
// SWIG file for building expectk
// Dave Beazley
// March 18, 1996
//
/* Revision History
 * $Log$
 * Revision 1.1  2002/04/29 19:56:56  RD
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
 * Revision 1.1.1.1  1999/02/28 02:00:55  beazley
 * Swig1.1
 *
 * Revision 1.1  1996/05/22 19:47:45  beazley
 * Initial revision
 *
 */


#ifdef AUTODOC
%subsection "expectk.i"
%text %{
This module provides a main() function for building an extended version of
expectk.  It has been tested with Expect 5.19, but may need modification
for newer versions.
%}
#endif

%{

/* exp_main_tk.c - main for expectk

This is "main.c" from the Tk distribution with some minor modifications to
support Expect.

Don Libes, NIST, 12/19/92

*/


/* 
 * main.c --
 *
 *	This file contains the main program for "wish", a windowing
 *	shell based on Tk and Tcl.  It also provides a template that
 *	can be used as the basis for main programs for other Tk
 *	applications.
 *
 * Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*#include "tkConfig.h"*/
/*#include "tkInt.h"*/
#include <tk.h>
#include "expect_tcl.h"
#include "Dbg.h"
#include "string.h"

#ifdef TK_EXTENDED
#    include "tclExtend.h"
#endif

/*
 * Global variables used by the main program:
 */

static Tk_Window mainWindow;	/* The main window for the application.  If
				 * NULL then the application no longer
				 * exists. */
static Tcl_Interp *interp;	/* Interpreter for this application. */
#if 0
char *tcl_RcFileName = NULL;	/* Name of a user-specific startup script
				 * to source if the application is being run
				 * interactively (e.g. "~/.wishrc").  Set
				 * by Tcl_AppInit.  NULL means don't source
				 * anything ever. */
#endif
static Tcl_DString command;	/* Used to assemble lines of terminal input
				 * into Tcl commands. */
static int tty;			/* Non-zero means standard input is a
				 * terminal-like device.  Zero means it's
				 * a file. */
static char normalExitCmd[] = "exit";
static char errorExitCmd[] = "exit 1";

/*
 * Command-line options:
 */

int synchronize = 0;
char *fileName = NULL;
char *name = NULL;
char *display = NULL;
char *geometry = NULL;

/* for Expect */
int my_rc = 1;
int sys_rc = 1;
int optcmd_eval();
int dashdash;	/* not used, but Tk's arg parser requires a placeholder */
#ifdef TCL_DEBUGGER
int optcmd_debug();
#endif

Tk_ArgvInfo argTable[] = {
    {"-file", TK_ARGV_STRING, (char *) NULL, (char *) &fileName,
	"File from which to read commands"},
    {"-geometry", TK_ARGV_STRING, (char *) NULL, (char *) &geometry,
	"Initial geometry for window"},
    {"-display", TK_ARGV_STRING, (char *) NULL, (char *) &display,
	"Display to use"},
    {"-name", TK_ARGV_STRING, (char *) NULL, (char *) &name,
	"Name to use for application"},
    {"-sync", TK_ARGV_CONSTANT, (char *) 1, (char *) &synchronize,
	"Use synchronous mode for display server"},

/* for Expect */
    {"-buffer", TK_ARGV_STRING, (char *) 1, (char *) &exp_buffer_command_input,
	"Buffer command input"},
    {"-command", TK_ARGV_GENFUNC, (char *) optcmd_eval, (char *)0,
	"Command(s) to execute immediately"},
    {"-diag", TK_ARGV_CONSTANT, (char *) 1, (char *) &exp_is_debugging,
	"Enable diagnostics"},
    {"--", TK_ARGV_REST, (char *)NULL, (char *)&dashdash,
	"End of options"},
#if TCL_DEBUGGER
    {"-Debug", TK_ARGV_GENFUNC, (char *) optcmd_debug, (char *)0, 
	"Enable debugger"},
#endif
    {"-interactive", TK_ARGV_CONSTANT, (char *) 1, (char *) &exp_interactive,
	"Interactive mode"},
    {"-norc", TK_ARGV_CONSTANT, (char *) 0, (char *) &my_rc,
	"Don't read ~/.expect.rc"},
    {"-NORC", TK_ARGV_CONSTANT, (char *) 0, (char *) &sys_rc,
	"Don't read system-wide expect.rc"},
    {(char *) NULL, TK_ARGV_END, (char *) NULL, (char *) NULL,
	(char *) NULL}
};

#ifdef TCL_DEBUGGER
/*ARGSUSED*/
int
optcmd_debug(dst,interp,key,argc,argv)
char *dst;
Tcl_Interp *interp;
char *key;
int argc;
char **argv;
{
	int i;

	if (argc == 0) {
		strcpy(interp->result,"-Debug flag needs 1 or 0 argument");
		return -1;
	}

	if (Tcl_GetInt(interp,argv[0],&i) != TCL_OK) {
		return -1;
	}

	if (i) {
		Dbg_On(interp,0);
	}

	argc--;
	for (i=0;i<argc;i++) {
		argv[i] = argv[i+1];
	}

	return argc;
}
#endif /*TCL_DEBUGGER*/

/*ARGSUSED*/
int
optcmd_eval(dst,interp,key,argc,argv)
char *dst;
Tcl_Interp *interp;
char *key;
int argc;
char **argv;
{
	int i;
	int rc;

	exp_cmdlinecmds = 1;

	rc = Tcl_Eval(interp,argv[0]);
	if (rc == TCL_ERROR) return -1;

	argc--;
	for (i=0;i<argc;i++) {
		argv[i] = argv[i+1];
	}

	return argc;
}

/*
 * Declaration for Tcl command procedure to create demo widget.  This
 * procedure is only invoked if SQUARE_DEMO is defined.
 */

extern int SquareCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));

/*
 * Forward declarations for procedures defined later in this file:
 */

static void		Prompt _ANSI_ARGS_((Tcl_Interp *interp, int partial));
static void		StdinProc _ANSI_ARGS_((ClientData clientData,
			    int mask));

/*
 *----------------------------------------------------------------------
 *
 * main --
 *
 *	Main program for Wish.
 *
 * Results:
 *	None. This procedure never returns (it exits the process when
 *	it's done
 *
 * Side effects:
 *	This procedure initializes the wish world and then starts
 *	interpreting commands;  almost anything could happen, depending
 *	on the script being interpreted.
 *
 *----------------------------------------------------------------------
 */

int
main(argc, argv)
    int argc;				/* Number of arguments. */
    char **argv;			/* Array of argument strings. */
{
    char *args, *p, *msg, *class;
    char buf[20];
    int code;
    int SWIG_init(Tcl_Interp *);
	extern char *exp_argv0;
	int used_argv1_for_filename = 0;	/* added for Expect - DEL */

#ifdef TK_EXTENDED
    tk_mainInterp = interp = Tcl_CreateExtendedInterp();
#else
    interp = Tcl_CreateInterp();
#endif
#ifdef TCL_MEM_DEBUG
    Tcl_InitMemory(interp);
#endif

	if (Exp_Init(interp) == TCL_ERROR) {
		fprintf(stderr,"Exp_Init failed: %s\n",interp->result);
		return 1;
	}

	/* Add SWIG Extension */

	if (SWIG_init(interp) == TCL_ERROR) {
	        fprintf(stderr,"Unable to initialize user-extensions : %s\n", interp->result);
		return 1;
	}
	exp_argv0 = argv[0];

#ifdef TCL_DEBUGGER
	Dbg_ArgcArgv(argc,argv,1);
#endif

    /*
     * Parse command-line arguments.
     */

    if (Tk_ParseArgv(interp, (Tk_Window) NULL, &argc, argv, argTable, 0)
	    != TCL_OK) {
	fprintf(stderr, "%s\n", interp->result);
	exit(1);
    }

    if (!fileName) {			/* added for Expect - DEL */
	fileName = argv[1];
        used_argv1_for_filename = 1;
    }

    if (name == NULL) {
	if (fileName != NULL) {
	    p = fileName;
	} else {
	    p = argv[0];
	}
	name = strrchr(p, '/');
	if (name != NULL) {
	    name++;
	} else {
	    name = p;
	}
    }

  /* if user hasn't explicitly requested we be interactive */
  /* look for a file or some other source of commands */
    if (fileName && !exp_interactive) {
	if (0 == strcmp(fileName,"-")) {
		exp_cmdfile = stdin;
	} else if (exp_buffer_command_input) {
		if (NULL == (exp_cmdfile = fopen(fileName,"r"))) {
			perror(fileName);
			exp_exit(interp,1);
		} else {
			exp_close_on_exec(fileno(exp_cmdfile));
		}
	} else {
		exp_cmdfilename = fileName;
	}
    } else if (!exp_cmdlinecmds) {
	/* no other source of commands, force interactive */
	exp_interactive = 1;
    }

    /*
     * If a display was specified, put it into the DISPLAY
     * environment variable so that it will be available for
     * any sub-processes created by us.
     */

    if (display != NULL) {
	Tcl_SetVar2(interp, "env", "DISPLAY", display, TCL_GLOBAL_ONLY);
    }

    /*
     * Initialize the Tk application.  If a -name option was provided,
     * use it;  otherwise, if a file name was provided, use the last
     * element of its path as the name of the application; otherwise
     * use the last element of the program name.  For the application's
     * class, capitalize the first letter of the name.
     */

#if TK_MAJOR_VERSION >= 4
    class = (char *) ckalloc((unsigned) (strlen(name) + 1));
    strcpy(class, name);
    class[0] = toupper((unsigned char) class[0]);
    mainWindow = Tk_CreateMainWindow(interp, display, name, class);
#else
# if TK_MAJOR_VERSION == 3 && TK_MINOR_VERSION < 4
    mainWindow = Tk_CreateMainWindow(interp, display, name);
# else
    mainWindow = Tk_CreateMainWindow(interp, display, name, "Tk");
# endif
#endif

    if (mainWindow == NULL) {
	fprintf(stderr, "%s\n", interp->result);
	exit(1);
    }
#if TK_MAJOR_VERSION == 3 && TK_MINOR_VERSION < 4
    Tk_SetClass(mainWindow, "Tk");
#endif
    if (synchronize) {
	XSynchronize(Tk_Display(mainWindow), True);
    }

#if TK_MAJOR_VERSION < 4
    Tk_GeometryRequest(mainWindow, 200, 200);
#endif

    /*
     * Make command-line arguments available in the Tcl variables "argc"
     * and "argv".  Also set the "geometry" variable from the geometry
     * specified on the command line.
     */

    if (used_argv1_for_filename) {	/* added for Expect - DEL */
	argv++;
	argc--;
	/* if no script name, use interpreter name */
	if (!argv[0] && !fileName) argv[0] = name;
    }

    args = Tcl_Merge(argc-1, argv+1);
    Tcl_SetVar(interp, "argv", args, TCL_GLOBAL_ONLY);
    ckfree(args);
    sprintf(buf, "%d", argc-1);
    Tcl_SetVar(interp, "argc", buf, TCL_GLOBAL_ONLY);
    Tcl_SetVar(interp, "argv0", (fileName != NULL) ? fileName : argv[0],
	    TCL_GLOBAL_ONLY);
    if (geometry != NULL) {
#if TK_MAJOR_VERSION < 4
	Tcl_SetVar(interp, "geometry", geometry, TCL_GLOBAL_ONLY);
#else
	Tcl_SetVar(interp, "geometry", geometry, TCL_GLOBAL_ONLY);
	code = Tcl_VarEval(interp, "wm geometry . ", geometry, (char *) NULL);
	if (code != TCL_OK) {
	    fprintf(stderr, "%s\n", interp->result);
	}
#endif
    }

    /*
     * Set the "tcl_interactive" variable.
     */

    tty = isatty(0);
    Tcl_SetVar(interp, "tcl_interactive",
	    ((fileName == NULL) && tty) ? "1" : "0", TCL_GLOBAL_ONLY);

    /*
     * Add a few application-specific commands to the application's
     * interpreter.
     */

#ifdef SQUARE_DEMO
    Tcl_CreateCommand(interp, "square", SquareCmd, (ClientData) mainWindow,
	    (void (*)()) NULL);
#endif

    if (Tcl_Init(interp) == TCL_ERROR) {
	fprintf(stderr,"Tcl_Init failed: %s\n",interp->result);
	return 1;
    }
    if (Tk_Init(interp) == TCL_ERROR) {
	fprintf(stderr,"Tk_Init failed: %s\n",interp->result);
	return 1;
    }

    /* Call Exp_Init again because Tcl_Init resets auto_path, sigh. */
    /* A better solution would be to execute Tcl/Tk_Init much earlier */
    /* (before argc/argv is processed). */

    if (Exp_Init(interp) == TCL_ERROR) {
	fprintf(stderr,"Exp_Init failed: %s\n",interp->result);
	return 1;
    }

#if 0
    tcl_RcFileName = "~/.wishrc";

    /*
     * Invoke application-specific initialization.
     */

    if (Tcl_AppInit(interp) != TCL_OK) {
	fprintf(stderr, "Tcl_AppInit failed: %s\n", interp->result);
    }
#endif

    exp_interpret_rcfiles(interp,my_rc,sys_rc);

#ifdef TK_EXTENDED
     tclAppName	= "Wish";
     tclAppLongname = "Wish - Tk Shell";
     tclAppVersion  = TK_VERSION;
     Tcl_ShellEnvInit (interp, TCLSH_ABORT_STARTUP_ERR,
		name,
		0, NULL,	   /* argv var already set  */
		fileName == NULL,  /* interactive?	    */
		NULL);		   /* Standard default file */
#endif

    /*
     * Set the geometry of the main window, if requested.
     */

    if (geometry != NULL) {
	code = Tcl_VarEval(interp, "wm geometry . ", geometry, (char *) NULL);
	if (code != TCL_OK) {
	    fprintf(stderr, "%s\n", interp->result);
	}
    }

    /*
     * Invoke the script specified on the command line, if any.
     */

	/* become interactive if requested or "nothing to do" */
	if (exp_interactive) {
		(void) exp_interpreter(interp);
	} else if (exp_cmdfile) {
		int rc = exp_interpret_cmdfile(interp,exp_cmdfile);
		if (rc != TCL_OK) exp_exit(interp,rc);
		Tk_MainLoop();
	} else if (exp_cmdfilename) {
		int rc = exp_interpret_cmdfilename(interp,exp_cmdfilename);
		if (rc != TCL_OK) exp_exit(interp,rc);
		Tk_MainLoop();
	}

    /*
     * Don't exit directly, but rather invoke the Tcl "exit" command.
     * This gives the application the opportunity to redefine "exit"
     * to do additional cleanup.
     */

    Tcl_Eval(interp,normalExitCmd);
    exit(1);

#if 0
    if (fileName != NULL) {
      Dbg_On(interp,0);
	code = Tcl_VarEval(interp, "source ", fileName, (char *) NULL);
	if (code != TCL_OK) {
	    goto error;
	}
	tty = 0;
    } else {
	/*
	 * Commands will come from standard input, so set up an event
	 * handler for standard input.  If the input device is aEvaluate the
	 * .rc file, if one has been specified, set up an event handler
	 * for standard input, and print a prompt if the input
	 * device is a terminal.
	 */

	if (tcl_RcFileName != NULL) {
	    Tcl_DString buffer;
	    char *fullName;
    
	    fullName = Tcl_TildeSubst(interp, tcl_RcFileName, &buffer);
	    if (fullName == NULL) {
		fprintf(stderr, "%s\n", interp->result);
	    } else {
		if (access(fullName, R_OK) == 0) {
		    code = Tcl_EvalFile(interp, fullName);
		    if (code != TCL_OK) {
			fprintf(stderr, "%s\n", interp->result);
		    }
		}
	    }
	    Tcl_DStringFree(&buffer);
	}
	Tk_CreateFileHandler(0, TK_READABLE, StdinProc, (ClientData) 0);
	if (tty) {
	    Prompt(interp, 0);
	}
    }
    fflush(stdout);
    Tcl_DStringInit(&command);

    /*
     * Loop infinitely, waiting for commands to execute.  When there
     * are no windows left, Tk_MainLoop returns and we exit.
     */

    Tk_MainLoop();

    /*
     * Don't exit directly, but rather invoke the Tcl "exit" command.
     * This gives the application the opportunity to redefine "exit"
     * to do additional cleanup.
     */

    Tcl_Eval(interp, "exit");
    exit(1);

error:
    msg = Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY);
    if (msg == NULL) {
	msg = interp->result;
    }
    fprintf(stderr, "%s\n", msg);
    Tcl_Eval(interp, errorExitCmd);
    return 1;			/* Needed only to prevent compiler warnings. */

#endif /*0*/
}

#if 0
/*
 *----------------------------------------------------------------------
 *
 * StdinProc --
 *
 *	This procedure is invoked by the event dispatcher whenever
 *	standard input becomes readable.  It grabs the next line of
 *	input characters, adds them to a command being assembled, and
 *	executes the command if it's complete.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Could be almost arbitrary, depending on the command that's
 *	typed.
 *
 *----------------------------------------------------------------------
 */

    /* ARGSUSED */
static void
StdinProc(clientData, mask)
    ClientData clientData;		/* Not used. */
    int mask;				/* Not used. */
{
#define BUFFER_SIZE 4000
    char input[BUFFER_SIZE+1];
    static int gotPartial = 0;
    char *cmd;
    int code, count;

    count = read(fileno(stdin), input, BUFFER_SIZE);
    if (count <= 0) {
	if (!gotPartial) {
	    if (tty) {
		Tcl_Eval(interp, "exit");
		exit(1);
	    } else {
		Tk_DeleteFileHandler(0);
	    }
	    return;
	} else {
	    count = 0;
	}
    }
    cmd = Tcl_DStringAppend(&command, input, count);
    if (count != 0) {
	if ((input[count-1] != '\n') && (input[count-1] != ';')) {
	    gotPartial = 1;
	    goto prompt;
	}
	if (!Tcl_CommandComplete(cmd)) {
	    gotPartial = 1;
	    goto prompt;
	}
    }
    gotPartial = 0;

    /*
     * Disable the stdin file handler while evaluating the command;
     * otherwise if the command re-enters the event loop we might
     * process commands from stdin before the current command is
     * finished.  Among other things, this will trash the text of the
     * command being evaluated.
     */

    Tk_CreateFileHandler(0, 0, StdinProc, (ClientData) 0);
    code = Tcl_RecordAndEval(interp, cmd, 0);
    Tk_CreateFileHandler(0, TK_READABLE, StdinProc, (ClientData) 0);
    Tcl_DStringFree(&command);
    if (*interp->result != 0) {
	if ((code != TCL_OK) || (tty)) {
	    printf("%s\n", interp->result);
	}
    }

    /*
     * Output a prompt.
     */

    prompt:
    if (tty) {
	Prompt(interp, gotPartial);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Prompt --
 *
 *	Issue a prompt on standard output, or invoke a script
 *	to issue the prompt.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A prompt gets output, and a Tcl script may be evaluated
 *	in interp.
 *
 *----------------------------------------------------------------------
 */

static void
Prompt(interp, partial)
    Tcl_Interp *interp;			/* Interpreter to use for prompting. */
    int partial;			/* Non-zero means there already
					 * exists a partial command, so use
					 * the secondary prompt. */
{
    char *promptCmd;
    int code;

    promptCmd = Tcl_GetVar(interp,
	partial ? "tcl_prompt2" : "tcl_prompt1", TCL_GLOBAL_ONLY);
    if (promptCmd == NULL) {
	defaultPrompt:
	if (!partial) {
	    fputs("% ", stdout);
	}
    } else {
	code = Tcl_Eval(interp, promptCmd);
	if (code != TCL_OK) {
	    Tcl_AddErrorInfo(interp,
		    "\n    (script that generates prompt)");
	    fprintf(stderr, "%s\n", interp->result);
	    goto defaultPrompt;
	}
    }
    fflush(stdout);
}
#endif /*0*/

%}

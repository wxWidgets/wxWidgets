//
// embed.i
// SWIG file embedding the Python interpreter in something else.
// This file is based on Python-1.3, but it might work with
// later versions.
//
// This file makes it possible to extend Python and all of its
// built-in functions without having to hack it's setup script.
//

#ifdef AUTODOC
%subsection "embed13.i"
%text %{
This module provides support for building a new version of the
Python 1.3 executable.  This will be necessary on systems that do
not support shared libraries and may be necessary with C++
extensions.  This file contains everything you need to build
a new version of Python from include files and libraries normally
installed with the Python language.

This module is functionally equivalent to the embed.i library,
but has a number of changes needed to work with older versions
of Python.
%}
#else
%echo "embed.i : Using Python 1.3"
#endif




%wrapper %{

#ifndef NEED_GETOPT
#include <unistd.h>
#endif

#include <pythonrun.h>
typedef struct SWIGPyTab {
	char *name;
	void (*initfunc)();
} SWIGPyTab;

#ifdef __cplusplus
extern "C"
#endif
void SWIG_init(void);  /* Forward reference */

#define inittab    python_inittab

/* Grab Python's inittab[] structure */

#ifdef __cplusplus
extern "C" {
#endif
#include <config.c>

#undef inittab


/* Now define our own version of it.
   God forbid someone have more than 1000 built-in modules! */

SWIGPyTab inittab[1000];       

static int  swig_num_modules = 0;

/* Function for adding modules to Python */

static void swig_add_module(char *name, void (*initfunc)()) {
	inittab[swig_num_modules].name = name;
	inittab[swig_num_modules].initfunc = initfunc;
	swig_num_modules++;
	inittab[swig_num_modules].name = (char *) 0;
	inittab[swig_num_modules].initfunc = (void (*)()) 0;
}				

/* Function to add all of Python's build in modules to our interpreter */

static void swig_add_builtin() {
	int i = 0;
	while (python_inittab[i].name) {
		swig_add_module(python_inittab[i].name, python_inittab[i].initfunc);
		i++;
 	}
	
	/* Add SWIG builtin function */
	swig_add_module(SWIG_name, SWIG_init);
#ifdef SWIGMODINIT
        SWIGMODINIT
#endif
}
#ifdef __cplusplus
}
#endif

/* Interface to getopt(): */
extern int optind;
extern char *optarg;

#ifdef NEED_GETOPT
#ifdef __cplusplus
extern "C" int getopt(int, char **, char *);
#else
extern int getopt();    /* PROTO((int, char **, char *)); -- not standardized */
#endif
#endif

extern int Py_DebugFlag;             /* For parser.c, declared in pythonrun.c */
extern int Py_VerboseFlag;           /* For import.c, declared in pythonrun.c */
extern int Py_SuppressPrintingFlag;  /* For ceval.c, declared in pythonrun.c */

/* Subroutines that live in their own file */
#ifdef __cplusplus
extern "C" {
extern int  isatty(int fd);
extern int  PySys_SetArgv(int, char **);
#endif
extern char *getversion();
extern char *getcopyright();
#ifdef __cplusplus
}
#endif

/* For getprogramname(); set by main() */
static char *argv0;

/* For getargcargv(); set by main() */
static char **orig_argv;
static int  orig_argc;

/* Short usage message (with %s for argv0) */
static char *usage_line =
"usage: %s [-d] [-i] [-s] [-u ] [-v] [-c cmd | file | -] [arg] ...\n";

/* Long usage message, split into parts < 512 bytes */
static char *usage_top = "\n\
Options and arguments (and corresponding environment variables):\n\
-d     : debug output from parser (also PYTHONDEBUG=x)\n\
-i     : inspect interactively after running script (also PYTHONINSPECT=x)\n\
-s     : suppress printing of top level expressions (also PYTHONSUPPRESS=x)\n\
-u     : unbuffered stdout and stderr (also PYTHONUNBUFFERED=x)\n\
-v     : verbose (trace import statements) (also PYTHONVERBOSE=x)\n\
-c cmd : program passed in as string (terminates option list)\n\
";
static char *usage_bot = "\
file   : program read from script file\n\
-      : program read from stdin (default; interactive mode if a tty)\n\
arg ...: arguments passed to program in sys.argv[1:]\n\
\n\
Other environment variables:\n\
PYTHONSTARTUP: file executed on interactive startup (no default)\n\
PYTHONPATH   : colon-separated list of directories prefixed to the\n\
               default module search path.  The result is sys.path.\n\
";

/* Main program */

int
main(int argc, char **argv) {
	int c;
	int sts;
	char *command = NULL;
	char *filename = NULL;
	FILE *fp = stdin;
	char *p;
	int inspect = 0;
	int unbuffered = 0;

	swig_add_builtin();     /* Add SWIG built-in modules */
	orig_argc = argc;	/* For getargcargv() */
	orig_argv = argv;
	argv0 = argv[0];	/* For getprogramname() */

	if ((p = getenv("PYTHONDEBUG")) && *p != '\0')
		Py_DebugFlag = 1;
	if ((p = getenv("PYTHONSUPPRESS")) && *p != '\0')
		Py_SuppressPrintingFlag = 1;
	if ((p = getenv("PYTHONVERBOSE")) && *p != '\0')
		Py_VerboseFlag = 1;
	if ((p = getenv("PYTHONINSPECT")) && *p != '\0')
		inspect = 1;
	if ((p = getenv("PYTHONUNBUFFERED")) && *p != '\0')
		unbuffered = 1;

	while ((c = getopt(argc, argv, "c:disuv")) != EOF) {
		if (c == 'c') {
			/* -c is the last option; following arguments
			   that look like options are left for the
			   the command to interpret. */
			command = (char *) malloc(strlen(optarg) + 2);
			if (command == NULL)
				Py_FatalError(
				   "not enough memory to copy -c argument");
			strcpy(command, optarg);
			strcat(command, "\n");
			break;
		}
		
		switch (c) {

		case 'd':
			Py_DebugFlag++;
			break;

		case 'i':
			inspect++;
			break;

		case 's':
			Py_SuppressPrintingFlag++;
			break;

		case 'u':
			unbuffered++;
			break;

		case 'v':
			Py_VerboseFlag++;
			break;

		/* This space reserved for other options */

		default:
			fprintf(stderr, usage_line, argv[0]);
			fprintf(stderr, usage_top);
			fprintf(stderr, usage_bot);
			exit(2);
			/*NOTREACHED*/

		}
	}

	if (unbuffered) {
#ifndef MPW
		setbuf(stdout, (char *)NULL);
		setbuf(stderr, (char *)NULL);
#else
		/* On MPW (3.2) unbuffered seems to hang */
		setvbuf(stdout, (char *)NULL, _IOLBF, BUFSIZ);
		setvbuf(stderr, (char *)NULL, _IOLBF, BUFSIZ);
#endif
	}

	if (command == NULL && optind < argc &&
	    strcmp(argv[optind], "-") != 0)
		filename = argv[optind];

	if (Py_VerboseFlag ||
	    command == NULL && filename == NULL && isatty((int)fileno(fp)))
		fprintf(stderr, "Python %s\n%s\n",
			getversion(), getcopyright());
	
	if (filename != NULL) {
		if ((fp = fopen(filename, "r")) == NULL) {
			fprintf(stderr, "%s: can't open file '%s'\n",
				argv[0], filename);
			exit(2);
		}
	}
	
	Py_Initialize();
	if (command != NULL) {
		/* Backup optind and force sys.argv[0] = '-c' */
		optind--;
		argv[optind] = "-c";
	}

	PySys_SetArgv(argc-optind, argv+optind);

	if (command) {
		sts = PyRun_SimpleString(command) != 0;
	}
	else {
		if (filename == NULL && isatty((int)fileno(fp))) {
			char *startup = getenv("PYTHONSTARTUP");
			if (startup != NULL && startup[0] != '\0') {
				FILE *fp = fopen(startup, "r");
				if (fp != NULL) {
					(void) PyRun_SimpleFile(fp, startup);
					PyErr_Clear();
					fclose(fp);
				}
			}
		}
		sts = PyRun_AnyFile(
			fp, filename == NULL ? "<stdin>" : filename) != 0;
		if (filename != NULL)
			fclose(fp);
	}

	if (inspect && isatty((int)fileno(stdin)) &&
	    (filename != NULL || command != NULL))
		sts = PyRun_AnyFile(stdin, "<stdin>") != 0;

	Py_Exit(sts);
	/*NOTREACHED*/
}


/* Return the program name -- some code out there needs this. */

#ifdef __cplusplus
extern "C"
#endif

char *
getprogramname()
{
	return argv0;
}


/* Make the *original* argc/argv available to other modules.
   This is rare, but it is needed by the secureware extension. */

#ifdef __cplusplus
extern "C"
#endif
void
getargcargv(int *argc,char ***argv)
{
	*argc = orig_argc;
	*argv = orig_argv;
}

/* Total Hack to get getpath.c to compile under C++ */

#ifdef __cplusplus
#define malloc   (char *) malloc
extern "C" {
#endif
#include <getpath.c>
#ifdef __cplusplus
}
#undef malloc
#endif

%}


  

/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


//#ifdef __GNUG__
//#pragma implementation "utils.h"
//#endif

#include "wx/utils.h"
#include "wx/string.h"

#include <stdarg.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pwd.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>

#ifdef __SVR4__
#include <sys/systeminfo.h>
#endif

//------------------------------------------------------------------------
// misc.
//------------------------------------------------------------------------

void wxBell(void)
{
  gdk_beep();
};

void wxSleep(int nSecs)
{
  sleep(nSecs);
};

int wxKill(long pid, int sig)
{
  return kill(pid, sig);
};

void wxDisplaySize( int *width, int *height )
{
  if (width) *width = gdk_screen_width();
  if (height) *height = gdk_screen_height();
}

void wxGetMousePosition( int* x, int* y )
{
  wxFAIL_MSG( "GetMousePosition not yet implemented" );
  if (x) *x = 0;
  if (y) *y = 0;
};

bool wxColourDisplay(void)
{
  wxFAIL_MSG( "wxColourDisplay always returns TRUE" );
  return TRUE;
}

int wxDisplayDepth(void)
{
  wxFAIL_MSG( "wxDisplayDepth always returns 8" );
  return 8;
}

//------------------------------------------------------------------------
// user and home routines
//------------------------------------------------------------------------

char* wxGetHomeDir( char *dest )
{
  wxString tmp = wxGetUserHome( wxString() );
  if (tmp.IsNull())
    strcpy( wxBuffer, "/" );
  else
    strcpy( wxBuffer, tmp );
  if (dest) strcpy( dest, WXSTRINGCAST tmp );
  return wxBuffer;
};

char *wxGetUserHome( const wxString &user )
{
  struct passwd *who = (struct passwd *) NULL;

  if (user.IsNull() || (user== "")) 
  {
	register char *ptr;

	if ((ptr = getenv("HOME")) != NULL) 
	    return ptr;
	if ((ptr = getenv("USER")) != NULL
	|| (ptr = getenv("LOGNAME")) != NULL) {
	    who = getpwnam(ptr);
	}
	// We now make sure the the user exists!
	if (who == NULL)
	    who = getpwuid(getuid());
  } 
  else
    who = getpwnam (user);
    
  return who ? who->pw_dir : (char*)NULL;
};

//------------------------------------------------------------------------
// id routines
//------------------------------------------------------------------------

bool wxGetHostName(char *buf, int sz)
{
    *buf = '\0';
#if defined(__SVR4__) && !defined(__sgi)
    return (sysinfo(SI_HOSTNAME, buf, sz) != -1);
#else /* BSD Sockets */
    char name[255];
    struct hostent *h;
    // Get hostname
    if (gethostname(name, sizeof(name)/sizeof(char)-1) == -1)
	return FALSE;
    // Get official full name of host
    strncpy(buf, (h=gethostbyname(name))!=NULL ? h->h_name : name, sz-1);
    return TRUE;
#endif
}

bool wxGetUserId(char *buf, int sz)
{
    struct passwd *who;

    *buf = '\0';
    if ((who = getpwuid(getuid ())) != NULL) {
	strncpy (buf, who->pw_name, sz-1);
	return TRUE;
    }
    return FALSE;
}

bool wxGetUserName(char *buf, int sz)
{
    struct passwd *who;

    *buf = '\0';
    if ((who = getpwuid (getuid ())) != NULL) {
	strncpy (buf, who->pw_gecos, sz - 1);
	return TRUE;
    }
    return FALSE;
}

//------------------------------------------------------------------------
// error and debug output routines
//------------------------------------------------------------------------

void wxDebugMsg( const char *format, ... )
{
  va_list ap;
  va_start( ap, format );
  vfprintf( stderr, format, ap ); 
  fflush( stderr );
  va_end(ap);
};

void wxError( const wxString &msg, const wxString &title )
{
  fprintf( stderr, "Error " );
  if (!title.IsNull()) fprintf( stderr, "%s ", WXSTRINGCAST(title) );
  if (!msg.IsNull()) fprintf( stderr, ": %s", WXSTRINGCAST(msg) );
  fprintf( stderr, ".\n" );
};

void wxFatalError( const wxString &msg, const wxString &title )
{
  fprintf( stderr, "Error " );
  if (!title.IsNull()) fprintf( stderr, "%s ", WXSTRINGCAST(title) );
  if (!msg.IsNull()) fprintf( stderr, ": %s", WXSTRINGCAST(msg) );
  fprintf( stderr, ".\n" );
  exit(1);
};

//------------------------------------------------------------------------
// directory routines
//------------------------------------------------------------------------

bool wxDirExists( const wxString& dir )
{
  char buf[500];
  strcpy( buf, WXSTRINGCAST(dir) );
  struct stat sbuf;
  return ((stat(buf, &sbuf) != -1) && S_ISDIR(sbuf.st_mode) ? TRUE : FALSE);
};

//------------------------------------------------------------------------
// wild character routines
//------------------------------------------------------------------------

bool wxIsWild( const wxString& pattern )
{
  wxString tmp = pattern;
  char *pat = WXSTRINGCAST(tmp);
    while (*pat) {
	switch (*pat++) {
	case '?': case '*': case '[': case '{':
	    return TRUE;
	case '\\':
	    if (!*pat++)
		return FALSE;
	}
    }
    return FALSE;
};


bool wxMatchWild( const wxString& pat, const wxString& text, bool dot_special )
{
  wxString tmp1 = pat;
  char *pattern = WXSTRINGCAST(tmp1);
  wxString tmp2 = text;
  char *str = WXSTRINGCAST(tmp2);
    char c;
    char *cp;
    bool done = FALSE, ret_code, ok;
    // Below is for vi fans
    const char OB = '{', CB = '}';

    // dot_special means '.' only matches '.'
    if (dot_special && *str == '.' && *pattern != *str)
	return FALSE;

    while ((*pattern != '\0') && (!done)
    && (((*str=='\0')&&((*pattern==OB)||(*pattern=='*')))||(*str!='\0'))) {
	switch (*pattern) {
	case '\\':
	    pattern++;
	    if (*pattern != '\0')
		pattern++;
	    break;
	case '*':
	    pattern++;
	    ret_code = FALSE;
	    while ((*str!='\0')
	    && (!(ret_code=wxMatchWild(pattern, str++, FALSE))))
		/*loop*/;
	    if (ret_code) {
		while (*str != '\0')
		    str++;
		while (*pattern != '\0')
		    pattern++;
	    }
	    break;
	case '[':
	    pattern++;
	  repeat:
	    if ((*pattern == '\0') || (*pattern == ']')) {
		done = TRUE;
		break;
	    }
	    if (*pattern == '\\') {
		pattern++;
		if (*pattern == '\0') {
		    done = TRUE;
		    break;
		}
	    }
	    if (*(pattern + 1) == '-') {
		c = *pattern;
		pattern += 2;
		if (*pattern == ']') {
		    done = TRUE;
		    break;
		}
		if (*pattern == '\\') {
		    pattern++;
		    if (*pattern == '\0') {
			done = TRUE;
			break;
		    }
		}
		if ((*str < c) || (*str > *pattern)) {
		    pattern++;
		    goto repeat;
		}
	    } else if (*pattern != *str) {
		pattern++;
		goto repeat;
	    }
	    pattern++;
	    while ((*pattern != ']') && (*pattern != '\0')) {
		if ((*pattern == '\\') && (*(pattern + 1) != '\0'))
		    pattern++;
		pattern++;
	    }
	    if (*pattern != '\0') {
		pattern++, str++;
	    }
	    break;
	case '?':
	    pattern++;
	    str++;
	    break;
	case OB:
	    pattern++;
	    while ((*pattern != CB) && (*pattern != '\0')) {
		cp = str;
		ok = TRUE;
		while (ok && (*cp != '\0') && (*pattern != '\0')
		&&  (*pattern != ',') && (*pattern != CB)) {
		    if (*pattern == '\\')
			pattern++;
		    ok = (*pattern++ == *cp++);
		}
		if (*pattern == '\0') {
		    ok = FALSE;
		    done = TRUE;
		    break;
		} else if (ok) {
		    str = cp;
		    while ((*pattern != CB) && (*pattern != '\0')) {
			if (*++pattern == '\\') {
			    if (*++pattern == CB)
				pattern++;
			}
		    }
		} else {
		    while (*pattern!=CB && *pattern!=',' && *pattern!='\0') {
			if (*++pattern == '\\') {
                            if (*++pattern == CB || *pattern == ',')
				pattern++;
			}
		    }
		}
		if (*pattern != '\0')
		    pattern++;
	    }
	    break;
	default:
	    if (*str == *pattern) {
		str++, pattern++;
	    } else {
		done = TRUE;
	    }
	}
    }
    while (*pattern == '*')
	pattern++;
    return ((*str == '\0') && (*pattern == '\0'));
};

//------------------------------------------------------------------------
// subprocess routines
//------------------------------------------------------------------------

typedef struct {
  gint pid, tag;
  wxProcess *process;
} wxEndProcessData;

static void GTK_EndProcessDetector(gpointer data, gint source,
                                   GdkInputCondition WXUNUSED(condition) )
{
  wxEndProcessData *proc_data = (wxEndProcessData *)data;
  int pid;

  pid = (proc_data->pid > 0) ? proc_data->pid : -(proc_data->pid);

  /* wait4 is not part of any standard, use at own risk 
   * not sure what wait4 does, but wait3 seems to be closest, whats a digit ;-)
   * --- offer@sgi.com */
#if !defined(__sgi)
  wait4(proc_data->pid, NULL, 0, NULL);
#else
  wait3((int *) NULL, 0, (rusage *) NULL);
#endif

  close(source);
  gdk_input_remove(proc_data->tag);

  if (proc_data->process)
    proc_data->process->OnTerminate(proc_data->pid);

  if (proc_data->pid > 0)
    delete proc_data;
  else
    proc_data->pid = 0;
};

long wxExecute( char **argv, bool sync, wxProcess *process )
{
    wxEndProcessData *data = new wxEndProcessData;
    int end_proc_detect[2];

    if (*argv == NULL)
	return 0;

    /* Create pipes */
    if (pipe(end_proc_detect) == -1) {
      perror("pipe failed");
      return 0;
    }

    /* fork the process */
#if defined(sun) || defined(__ultrix) || defined(__bsdi__)
    pid_t pid = vfork();
#else
    pid_t pid = fork();
#endif
    if (pid == -1) {
	perror ("fork failed");
	return 0;
    } else if (pid == 0) {
        /* Close fd not useful */
        close(end_proc_detect[0]); // close reading side

	/* child */
#ifdef _AIX
	execvp ((const char *)*argv, (const char **)argv);
#else
	execvp (*argv, argv);
#endif
	if (errno == ENOENT)
	    wxError("command not found", *argv);
	else
	    perror (*argv);
	wxError("could not execute", *argv);
	_exit (-1);
    }

    close(end_proc_detect[1]); // close writing side
    data->tag = gdk_input_add(end_proc_detect[0], GDK_INPUT_READ,
                              GTK_EndProcessDetector, (gpointer)data);
    data->pid = pid;
    if (!sync) {
      data->process = process;
    } else {
      data->process = (wxProcess *) NULL;
      data->pid = -(data->pid);

      while (data->pid != 0)
        wxYield();

      delete data;
    }

    return pid;
};

long wxExecute( const wxString& command, bool sync, wxProcess *process )
{
    if (command.IsNull() || command == "") return FALSE;

    int argc = 0;
    char *argv[127];
    char tmp[1024];
    const char *IFS = " \t\n";

    strncpy (tmp, command, sizeof(tmp) / sizeof(char) - 1);
    tmp[sizeof (tmp) / sizeof (char) - 1] = '\0';
    argv[argc++] = strtok (tmp, IFS);
    while ((argv[argc++] = strtok((char *) NULL, IFS)) != NULL)
	/* loop */ ;
    return wxExecute(argv, sync, process);
};



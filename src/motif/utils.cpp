/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:     Various utilities
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// Note: this is done in utilscmn.cpp now.
// #pragma implementation
// #pragma implementation "utils.h"
#endif

#include "wx/setup.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/msgdlg.h"
#include "wx/cursor.h"

#include <ctype.h>
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

#if defined(__SVR4__) && !defined(__HPUX__)
#include <sys/systeminfo.h>
#endif

#if (defined(__SUNCC__) || defined(__CLCC__))
#include <sysent.h>
#endif

#include <Xm/Xm.h>

#include "wx/motif/private.h"

// Yuck this is really BOTH site and platform dependent
// so we should use some other strategy!
#ifdef sun
# define DEFAULT_XRESOURCE_DIR "/usr/openwin/lib/app-defaults"
#else
# define DEFAULT_XRESOURCE_DIR "/usr/lib/X11/app-defaults"
#endif

static char *GetIniFile (char *dest, const char *filename);

extern wxList wxTopLevelWindows;

// Get full hostname (eg. DoDo.BSn-Germany.crg.de)
bool wxGetHostName(char *buf, int maxSize)
{
#if defined(__SVR4__) && !defined(__HPUX__)
  return (sysinfo (SI_HOSTNAME, buf, maxSize) != -1);
#else /* BSD Sockets */
  char name[255];
  struct hostent *h;

  // Get hostname
  if (gethostname (name, sizeof (name) / sizeof (char) - 1) == -1)
      return FALSE;
  // Get official full name of host
  strncpy (buf
	   ,(h = gethostbyname (name)) != NULL ? h->h_name : name
	   ,maxSize - 1);
  return TRUE;
#endif
}

// Get user ID e.g. jacs
bool wxGetUserId(char *buf, int maxSize)
{
#ifdef VMS
  *buf = '\0'; // return empty string
  return FALSE;
#else
  struct passwd *who;

  if ((who = getpwuid (getuid ())) != NULL)
    {
      strncpy (buf, who->pw_name, maxSize - 1);
      return TRUE;
    }
  return FALSE;
#endif
}

// Get user name e.g. Julian Smart
bool wxGetUserName(char *buf, int maxSize)
{
#ifdef VMS
  *buf = '\0'; // return empty string
  return FALSE;
#else
  struct passwd *who;

  if ((who = getpwuid (getuid ())) != NULL)
    {
      strncpy (buf, who->pw_gecos, maxSize - 1);
      return TRUE;
    }
  return FALSE;
#endif
}

int wxKill(long pid, int sig)
{
  int unixSignal = 0;
  switch (sig)
  {
    case wxSIGTERM:
    default:
      unixSignal = SIGTERM;
  }
  return kill( (int)pid, unixSignal);
}

//
// Execute a program in an Interactive Shell
//
bool wxShell(const wxString& command)
{
#ifdef VMS
  return(FALSE);
#else
#if defined(sun) || defined(__ultrix) || defined(__bsdi__)
  pid_t pid = vfork ();
#else
  pid_t pid = fork ();
#endif
  switch( pid ) {
    case -1:			/* error */
	return(FALSE);
    case 0:			/* child */
	// Generic X windows terminal window
	if (command != "")
	  execlp("xterm", "-e", (char *) (const char*) command, NULL);
	else
	  execlp("xterm", NULL);
	_exit(127);
  }
  return TRUE;
#endif
 // End VMS
}

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
long wxGetFreeMemory()
{
    return -1;
}

void wxSleep(int nSecs)
{
  sleep(nSecs);
}

// Consume all events until no more left
void wxFlushEvents()
{
  Display *display = (Display*) wxGetDisplay();

  XSync (display, FALSE);
  XEvent event;
  // XtAppPending returns availability of events AND timers/inputs, which
  // are processed via callbacks, so XtAppNextEvent will not return if 
  // there are no events. So added '& XtIMXEvent' - Sergey.
  while (XtAppPending ((XtAppContext) wxTheApp->GetAppContext()) & XtIMXEvent)
    {
      XFlush (XtDisplay ((Widget) wxTheApp->GetTopLevelWidget()));
      // Jan Lessner: works better when events are non-X events
      XtAppProcessEvent((XtAppContext) wxTheApp->GetAppContext(), XtIMXEvent);
    }
}

// Output a debug message, in a system dependent fashion.
void wxDebugMsg(const char *fmt ...)
{
  va_list ap;
  char buffer[BUFSIZ];

  if (!wxTheApp->GetWantDebugOutput())
    return ;

  va_start (ap, fmt);

  vsprintf (buffer, fmt, ap);
  cerr << buffer;

  va_end (ap);
}

// Non-fatal error: pop up message box and (possibly) continue
void wxError(const wxString& msg, const wxString& title)
{
  cerr << (const char*) title << ": " << (const char*) msg << "\n";
}

// Fatal error: pop up message box and abort
void wxFatalError(const wxString& msg, const wxString& title)
{
  cerr << (const char*) title << ": " << (const char*) msg << "\n";
  exit (1);
}

// Emit a beeeeeep
void wxBell()
{
  // Use current setting for the bell
  XBell ((Display*) wxGetDisplay(), 0);
}

int wxGetOsVersion(int *majorVsn, int *minorVsn)
{
    // TODO
  // This code is WRONG!! Does NOT return the
  // Motif version of the libs but the X protocol
  // version! @@@@@ Fix ME!!!!!!!!!
  Display *display = XtDisplay ((Widget) wxTheApp->GetTopLevelWidget());
  if (majorVsn)
    *majorVsn = ProtocolVersion (display);
  if (minorVsn)
    *minorVsn = ProtocolRevision (display);
  return wxMOTIF_X;
}

// Reading and writing resources (eg WIN.INI, .Xdefaults)
#if wxUSE_RESOURCES

static char *GetResourcePath(char *buf, const char *name, bool create = FALSE)
{
  if (create && wxFileExists (name) ) {
    strcpy(buf, name);
    return buf; // Exists so ...
  }

  if (*name == '/')
    strcpy(buf, name);
  else {
    // Put in standard place for resource files if not absolute
    strcpy (buf, DEFAULT_XRESOURCE_DIR);
    strcat (buf, "/");
    strcat (buf, (const char*) wxFileNameFromPath (name));
  }

  if (create) {
    // Touch the file to create it
    FILE *fd = fopen (buf, "w");
    if (fd) fclose (fd);
  }
  return buf;
}

/*
 * We have a cache for writing different resource files,
 * which will only get flushed when we call wxFlushResources().
 * Build up a list of resource databases waiting to be written.
 *
 */

wxList wxResourceCache (wxKEY_STRING);

void 
wxFlushResources (void)
{
  char nameBuffer[512];

  wxNode *node = wxResourceCache.First ();
  while (node)
    {
      const char *file = node->GetKeyString();
      // If file doesn't exist, create it first.
      (void)GetResourcePath(nameBuffer, file, TRUE);

      XrmDatabase database = (XrmDatabase) node->Data ();
      XrmPutFileDatabase (database, nameBuffer);
      XrmDestroyDatabase (database);
      wxNode *next = node->Next ();
      delete node;
      node = next;
    }
}

static XrmDatabase wxResourceDatabase = 0;

void wxXMergeDatabases (wxApp * theApp, Display * display);

bool wxWriteResource(const wxString& section, const wxString& entry, const wxString& value, const wxString& file)
{
  char buffer[500];

  (void) GetIniFile (buffer, file);

  XrmDatabase database;
  wxNode *node = wxResourceCache.Find (buffer);
  if (node)
    database = (XrmDatabase) node->Data ();
  else
    {
      database = XrmGetFileDatabase (buffer);
      wxResourceCache.Append (buffer, (wxObject *) database);
    }

  char resName[300];
  strcpy (resName, (const char*) section);
  strcat (resName, ".");
  strcat (resName, (const char*) entry);

  XrmPutStringResource (&database, resName, value);
  return TRUE;
}

bool wxWriteResource(const wxString& section, const wxString& entry, float value, const wxString& file)
{
  char buf[50];
  sprintf(buf, "%.4f", value);
  return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, long value, const wxString& file)
{
  char buf[50];
  sprintf(buf, "%ld", value);
  return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, int value, const wxString& file)
{
  char buf[50];
  sprintf(buf, "%d", value);
  return wxWriteResource(section, entry, buf, file);
}

bool wxGetResource(const wxString& section, const wxString& entry, char **value, const wxString& file)
{
  if (!wxResourceDatabase)
    {
      Display *display = (Display*) wxGetDisplay();
      wxXMergeDatabases (wxTheApp, display);
    }

  XrmDatabase database;

  if (file != "")
  {
      char buffer[500];
      
      // Is this right? Trying to get it to look in the user's
      // home directory instead of current directory -- JACS
      (void) GetIniFile (buffer, file);

      wxNode *node = wxResourceCache.Find (buffer);
      if (node)
	database = (XrmDatabase) node->Data ();
      else
	{
	  database = XrmGetFileDatabase (buffer);
	  wxResourceCache.Append (buffer, (wxObject *) database);
	}
  }
  else
    database = wxResourceDatabase;

  XrmValue xvalue;
  char *str_type[20];
  char buf[150];
  strcpy (buf, section);
  strcat (buf, ".");
  strcat (buf, entry);

  Bool success = XrmGetResource (database, buf, "*", str_type,
				 &xvalue);
  // Try different combinations of upper/lower case, just in case...
  if (!success)
    {
      buf[0] = (isupper (buf[0]) ? tolower (buf[0]) : toupper (buf[0]));
      success = XrmGetResource (database, buf, "*", str_type,
				&xvalue);
    }
  if (success)
    {
      if (*value)
        delete[] *value;

      *value = new char[xvalue.size + 1];
      strncpy (*value, xvalue.addr, (int) xvalue.size);
      return TRUE;
    }
  return FALSE;
}

bool wxGetResource(const wxString& section, const wxString& entry, float *value, const wxString& file)
{
  char *s = NULL;
  bool succ = wxGetResource(section, entry, (char **)&s, file);
  if (succ)
  {
    *value = (float)strtod(s, NULL);
    delete[] s;
    return TRUE;
  }
  else return FALSE;
}

bool wxGetResource(const wxString& section, const wxString& entry, long *value, const wxString& file)
{
  char *s = NULL;
  bool succ = wxGetResource(section, entry, (char **)&s, file);
  if (succ)
  {
    *value = strtol(s, NULL, 10);
    delete[] s;
    return TRUE;
  }
  else return FALSE;
}

bool wxGetResource(const wxString& section, const wxString& entry, int *value, const wxString& file)
{
  char *s = NULL;
  bool succ = wxGetResource(section, entry, (char **)&s, file);
  if (succ)
  {
      // Handle True, False here
      // True, Yes, Enables, Set or  Activated 
      if (*s == 'T' || *s == 'Y' || *s == 'E' || *s == 'S' || *s == 'A')
        *value = TRUE;
      // False, No, Disabled, Reset, Cleared, Deactivated
      else if (*s == 'F' || *s == 'N' || *s == 'D' || *s == 'R' || *s == 'C')
        *value = FALSE;
      // Handle as Integer
      else
        *value = (int) strtol (s, NULL, 10);
        delete[] s;
        return TRUE;
  }
  else
    return FALSE;
}

void wxXMergeDatabases (wxApp * theApp, Display * display)
{
  XrmDatabase homeDB, serverDB, applicationDB;
  char filenamebuf[1024];

  char *filename = &filenamebuf[0];
  char *environment;
  wxString classname = theApp->GetClassName();
  char name[256];
  (void) strcpy (name, "/usr/lib/X11/app-defaults/");
  (void) strcat (name, (const char*) classname);

  /* Get application defaults file, if any */
  applicationDB = XrmGetFileDatabase (name);
  (void) XrmMergeDatabases (applicationDB, &wxResourceDatabase);

  /* Merge server defaults, created by xrdb, loaded as a property of the root
   * window when the server initializes and loaded into the display
   * structure on XOpenDisplay;
   * if not defined, use .Xdefaults
   */

  if (XResourceManagerString (display) != NULL)
    {
      serverDB = XrmGetStringDatabase (XResourceManagerString (display));
    }
  else
    {
      (void) GetIniFile (filename, NULL);
      serverDB = XrmGetFileDatabase (filename);
    }
  XrmMergeDatabases (serverDB, &wxResourceDatabase);

  /* Open XENVIRONMENT file, or if not defined, the .Xdefaults,
   * and merge into existing database
   */

  if ((environment = getenv ("XENVIRONMENT")) == NULL)
    {
      size_t len;
      environment = GetIniFile (filename, NULL);
      len = strlen (environment);
#if defined(__SVR4__) && !defined(__HPUX__)
      (void) sysinfo (SI_HOSTNAME, environment + len, 1024 - len);
#else
      (void) gethostname (environment + len, 1024 - len);
#endif
    }
  homeDB = XrmGetFileDatabase (environment);
  XrmMergeDatabases (homeDB, &wxResourceDatabase);
}

#if 0

/*
 * Not yet used but may be useful.
 *
 */
void 
wxSetDefaultResources (const Widget w, const char **resourceSpec, const char *name)
{
  int i;
  Display *dpy = XtDisplay (w);	// Retrieve the display pointer

  XrmDatabase rdb = NULL;	// A resource data base

  // Create an empty resource database
  rdb = XrmGetStringDatabase ("");

  // Add the Component resources, prepending the name of the component

  i = 0;
  while (resourceSpec[i] != NULL)
    {
      char buf[1000];

      sprintf (buf, "*%s%s", name, resourceSpec[i++]);
      XrmPutLineResource (&rdb, buf);
    }

  // Merge them into the Xt database, with lowest precendence

  if (rdb)
    {
#if (XlibSpecificationRelease>=5)
      XrmDatabase db = XtDatabase (dpy);
      XrmCombineDatabase (rdb, &db, FALSE);
#else
      XrmMergeDatabases (dpy->db, &rdb);
      dpy->db = rdb;
#endif
    }
}
#endif
   // 0

#endif // wxUSE_RESOURCES

static int wxBusyCursorCount = 0;

// Helper function
static void 
wxXSetBusyCursor (wxWindow * win, wxCursor * cursor)
{
  Display *display = (Display*) win->GetXDisplay();

  Window xwin = (Window) win->GetXWindow();
  XSetWindowAttributes attrs;

  if (cursor)
    {
      attrs.cursor = (Cursor) cursor->GetXCursor(display);
    }
  else
    {
      // Restore old cursor
      if (win->GetCursor()->Ok())
        attrs.cursor = (Cursor) win->GetCursor()->GetXCursor(display);
      else
        attrs.cursor = None;
    }
  if (xwin)
    XChangeWindowAttributes (display, xwin, CWCursor, &attrs);

  XFlush (display);

  for(wxNode *node = win->GetChildren().First (); node; node = node->Next())
  {
        wxWindow *child = (wxWindow *) node->Data ();
	    wxXSetBusyCursor (child, cursor);
  }
}

// Set the cursor to the busy cursor for all windows
void wxBeginBusyCursor(wxCursor *cursor)
{
  wxBusyCursorCount++;
  if (wxBusyCursorCount == 1)
  {
    for(wxNode *node = wxTopLevelWindows.First (); node; node = node->Next())
    {
        wxWindow *win = (wxWindow *) node->Data ();
        wxXSetBusyCursor (win, cursor);
    }
  }
}

// Restore cursor to normal
void wxEndBusyCursor()
{
  if (wxBusyCursorCount == 0)
    return;

  wxBusyCursorCount--;
  if (wxBusyCursorCount == 0)
  {
      for(wxNode *node = wxTopLevelWindows.First (); node; node = node->Next())
      {
	    wxWindow *win = (wxWindow *) node->Data ();
	    wxXSetBusyCursor (win, NULL);
      }
  }
}

// TRUE if we're between the above two calls
bool wxIsBusy()
{
  return (wxBusyCursorCount > 0);
}    

const char* wxGetHomeDir( wxString *home  )
{
  *home = wxGetUserHome( wxString() );
  if (home->IsNull()) *home = "/";
  return *home;
};

char *wxGetUserHome (const wxString& user)
{
#ifdef VMS
  return(NULL);
#else
  struct passwd *who = NULL;

  if (user == "") {
    register char *ptr;

    if ((ptr = getenv("HOME")) != NULL) 
      return ptr;
    if ((ptr = getenv("USER")) != NULL ||
	(ptr = getenv("LOGNAME")) != NULL)
      {
	who = getpwnam( ptr );
      }
    // We now make sure the the user exists!
    if (who == NULL)
      who = getpwuid( getuid() );
  } else
    who = getpwnam ((const char*) user);

  return who ? who->pw_dir : (char*) NULL;
#endif
 // ifdef VMS
}

// Check whether this window wants to process messages, e.g. Stop button
// in long calculations.
bool wxCheckForInterrupt(wxWindow *wnd)
{
	if(wnd){
		Display *dpy=(Display*) wnd->GetXDisplay();
		Window win=(Window) wnd->GetXWindow();
		XEvent event;
		XFlush(dpy);
		if(wnd->GetMainWidget()){
			XmUpdateDisplay((Widget)(wnd->GetMainWidget()));
		}
		while(XCheckMaskEvent(dpy,
							ButtonPressMask|ButtonReleaseMask|ButtonMotionMask|
							PointerMotionMask|KeyPressMask|KeyReleaseMask,
							&event)){
			if(event.xany.window==win)
				XtDispatchEvent(&event);
	//		else
	//			XBell(dpy,50);
		}
		return TRUE;//*** temporary?
	}
	else{
		wxMessageBox("wnd==NULL !!!");
		return FALSE;//*** temporary?
	}
}

void wxGetMousePosition( int* x, int* y )
{
    XMotionEvent xev;
    Window root, child;
    XQueryPointer((Display*) wxGetDisplay(),
                  DefaultRootWindow((Display*) wxGetDisplay()), &root, &child,
                  &(xev.x_root), &(xev.y_root),
                  &(xev.x),      &(xev.y),
                  &(xev.state));
    *x = xev.x_root;
    *y = xev.y_root;
};

// Return TRUE if we have a colour display
bool wxColourDisplay()
{
    Display *dpy = (Display*) wxGetDisplay();

    if (DefaultDepth (dpy, DefaultScreen (dpy)) < 2)
      return FALSE;
    else
      return TRUE;
}

// Returns depth of screen
int wxDisplayDepth()
{
    Display *dpy = (Display*) wxGetDisplay();
    return DefaultDepth (dpy, DefaultScreen (dpy));
}

// Get size of display
void wxDisplaySize(int *width, int *height)
{
  Display *dpy = (Display*) wxGetDisplay();
  
  *width = DisplayWidth (dpy, DefaultScreen (dpy));
  *height = DisplayHeight (dpy, DefaultScreen (dpy));
}

/* Configurable display in Motif */
static WXDisplay *gs_currentDisplay = NULL;
static wxString gs_displayName;

WXDisplay *wxGetDisplay()
{
  if (gs_currentDisplay)
    return gs_currentDisplay;

  if (wxTheApp && wxTheApp->GetTopLevelWidget())
    return XtDisplay ((Widget) wxTheApp->GetTopLevelWidget());
  else if (wxTheApp)
    return wxTheApp->GetInitialDisplay();
  else
    return (WXDisplay*) NULL;
}

bool wxSetDisplay(const wxString& display_name)
{
  gs_displayName = display_name;
  
  if (display_name.IsNull() || display_name.IsEmpty())
  {
      gs_currentDisplay = NULL;
      return TRUE;
  }
  else
  {
    Cardinal argc = 0;

    Display *display = XtOpenDisplay((XtAppContext) wxTheApp->GetAppContext(),
				     (const char*) display_name,
				     (const char*) wxTheApp->GetAppName(),
				     (const char*) wxTheApp->GetClassName(),
				     NULL,
# if XtSpecificationRelease < 5
				     0, &argc, NULL);
# else
				     0, (int *)&argc, NULL);
# endif

    if (display)
    {
      gs_currentDisplay = (WXDisplay*) display;
      return TRUE;
    } else
      return FALSE;
  }
  return FALSE;
}

wxString wxGetDisplayName()
{
  return gs_displayName;
}

// Find the letter corresponding to the mnemonic, for Motif
char wxFindMnemonic (const char *s)
{
  char mnem = 0;
  int len = strlen (s);
  int i;
  for (i = 0; i < len; i++)
    {
      if (s[i] == '&')
	{
	  // Carefully handle &&
	  if ((i + 1) <= len && s[i + 1] == '&')
	    i++;
	  else
	    {
	      mnem = s[i + 1];
	      break;
	    }
	}
    }
  return mnem;
}

char * wxFindAccelerator (char *s)
{
// The accelerator text is after the \t char.
  while (*s && *s != '\t')
    s++;
  if (*s == '\0')
    return (NULL);
  s++;
/*
   Now we need to format it as X standard:

   input            output

   F7           --> <Key>F7
   Ctrl+N       --> Ctrl<Key>N
   Alt+k        --> Meta<Key>k
   Ctrl+Shift+A --> Ctrl Shift<Key>A

 */

  wxBuffer[0] = '\0';
  char *tmp = copystring (s);
  s = tmp;
  char *p = s;

  while (1)
    {
      while (*p && *p != '+')
	p++;
      if (*p)
	{
	  *p = '\0';
	  if (wxBuffer[0])
	    strcat (wxBuffer, " ");
	  if (strcmp (s, "Alt"))
	    strcat (wxBuffer, s);
	  else
	    strcat (wxBuffer, "Meta");
	  s = p + 1;
	  p = s;
	}
      else
	{
	  strcat (wxBuffer, "<Key>");
	  strcat (wxBuffer, s);
	  break;
	}
    }
  delete[]tmp;
  return wxBuffer;
}

XmString wxFindAcceleratorText (char *s)
{
// The accelerator text is after the \t char.
  while (*s && *s != '\t')
    s++;
  if (*s == '\0')
    return (NULL);
  s++;
  XmString text = XmStringCreateSimple (s);
  return text;
}

#include <X11/keysym.h>

int wxCharCodeXToWX(KeySym keySym)
{
  int id;
  switch (keySym) {
    case XK_Shift_L:
    case XK_Shift_R:
      id = WXK_SHIFT; break;
    case XK_Control_L:
    case XK_Control_R:
      id = WXK_CONTROL; break;
    case XK_BackSpace:
      id = WXK_BACK; break;
    case XK_Delete:
      id = WXK_DELETE; break;
    case XK_Clear:
      id = WXK_CLEAR; break;
    case XK_Tab:
      id = WXK_TAB; break;
    case XK_numbersign:
      id = '#'; break;
    case XK_Return:
      id = WXK_RETURN; break;
    case XK_Escape:
      id = WXK_ESCAPE; break;
    case XK_Pause:
    case XK_Break:
      id = WXK_PAUSE; break;
    case XK_Num_Lock:
      id = WXK_NUMLOCK; break;
    case XK_Scroll_Lock:
      id = WXK_SCROLL; break;

    case XK_Home:
      id = WXK_HOME; break;
    case XK_End:
      id = WXK_END; break;
    case XK_Left:
      id = WXK_LEFT; break;
    case XK_Right:
      id = WXK_RIGHT; break;
    case XK_Up:
      id = WXK_UP; break;
    case XK_Down:
      id = WXK_DOWN; break;
    case XK_Next:
      id = WXK_NEXT; break;
    case XK_Prior:
      id = WXK_PRIOR; break;
    case XK_Menu:
      id = WXK_MENU; break;
    case XK_Select:
      id = WXK_SELECT; break;
    case XK_Cancel:
      id = WXK_CANCEL; break;
    case XK_Print:
      id = WXK_PRINT; break;
    case XK_Execute:
      id = WXK_EXECUTE; break;
    case XK_Insert:
      id = WXK_INSERT; break;
    case XK_Help:
      id = WXK_HELP; break;

    case XK_KP_Multiply:
      id = WXK_MULTIPLY; break;
    case XK_KP_Add:
      id = WXK_ADD; break;
    case XK_KP_Subtract:
      id = WXK_SUBTRACT; break;
    case XK_KP_Divide:
      id = WXK_DIVIDE; break;
    case XK_KP_Decimal:
      id = WXK_DECIMAL; break;
    case XK_KP_Equal:
      id = '='; break;
    case XK_KP_Space:
      id = ' '; break;
    case XK_KP_Tab:
      id = WXK_TAB; break;
    case XK_KP_Enter:
      id = WXK_RETURN; break;
    case XK_KP_0:
      id = WXK_NUMPAD0; break;
    case XK_KP_1:
      id = WXK_NUMPAD1; break;
    case XK_KP_2:
      id = WXK_NUMPAD2; break;
    case XK_KP_3:
      id = WXK_NUMPAD3; break;
    case XK_KP_4:
      id = WXK_NUMPAD4; break;
    case XK_KP_5:
      id = WXK_NUMPAD5; break;
    case XK_KP_6:
      id = WXK_NUMPAD6; break;
    case XK_KP_7:
      id = WXK_NUMPAD7; break;
    case XK_KP_8:
      id = WXK_NUMPAD8; break;
    case XK_KP_9:
      id = WXK_NUMPAD9; break;
    case XK_F1:
      id = WXK_F1; break;
    case XK_F2:
      id = WXK_F2; break;
    case XK_F3:
      id = WXK_F3; break;
    case XK_F4:
      id = WXK_F4; break;
    case XK_F5:
      id = WXK_F5; break;
    case XK_F6:
      id = WXK_F6; break;
    case XK_F7:
      id = WXK_F7; break;
    case XK_F8:
      id = WXK_F8; break;
    case XK_F9:
      id = WXK_F9; break;
    case XK_F10:
      id = WXK_F10; break;
    case XK_F11:
      id = WXK_F11; break;
    case XK_F12:
      id = WXK_F12; break;
    case XK_F13:
      id = WXK_F13; break;
    case XK_F14:
      id = WXK_F14; break;
    case XK_F15:
      id = WXK_F15; break;
    case XK_F16:
      id = WXK_F16; break;
    case XK_F17:
      id = WXK_F17; break;
    case XK_F18:
      id = WXK_F18; break;
    case XK_F19:
      id = WXK_F19; break;
    case XK_F20:
      id = WXK_F20; break;
    case XK_F21:
      id = WXK_F21; break;
    case XK_F22:
      id = WXK_F22; break;
    case XK_F23:
      id = WXK_F23; break;
    case XK_F24:
      id = WXK_F24; break;
    default:
      id = (keySym <= 255) ? (int)keySym : -1;
  } // switch
  return id;
}

KeySym wxCharCodeWXToX(int id)
{
  KeySym keySym;

  switch (id) {
    case WXK_CANCEL:            keySym = XK_Cancel; break;
    case WXK_BACK:              keySym = XK_BackSpace; break;
    case WXK_TAB:	        keySym = XK_Tab; break;
    case WXK_CLEAR:		keySym = XK_Clear; break;
    case WXK_RETURN:		keySym = XK_Return; break;
    case WXK_SHIFT:		keySym = XK_Shift_L; break;
    case WXK_CONTROL:		keySym = XK_Control_L; break;
    case WXK_MENU :		keySym = XK_Menu; break;
    case WXK_PAUSE:		keySym = XK_Pause; break;
    case WXK_ESCAPE:		keySym = XK_Escape; break;
    case WXK_SPACE:		keySym = ' '; break;
    case WXK_PRIOR:		keySym = XK_Prior; break;
    case WXK_NEXT :		keySym = XK_Next; break;
    case WXK_END:		keySym = XK_End; break;
    case WXK_HOME :		keySym = XK_Home; break;
    case WXK_LEFT :		keySym = XK_Left; break;
    case WXK_UP:		keySym = XK_Up; break;
    case WXK_RIGHT:		keySym = XK_Right; break;
    case WXK_DOWN :		keySym = XK_Down; break;
    case WXK_SELECT:		keySym = XK_Select; break;
    case WXK_PRINT:		keySym = XK_Print; break;
    case WXK_EXECUTE:		keySym = XK_Execute; break;
    case WXK_INSERT:		keySym = XK_Insert; break;
    case WXK_DELETE:		keySym = XK_Delete; break;
    case WXK_HELP :		keySym = XK_Help; break;
    case WXK_NUMPAD0:		keySym = XK_KP_0; break;
    case WXK_NUMPAD1:		keySym = XK_KP_1; break;
    case WXK_NUMPAD2:		keySym = XK_KP_2; break;
    case WXK_NUMPAD3:		keySym = XK_KP_3; break;
    case WXK_NUMPAD4:		keySym = XK_KP_4; break;
    case WXK_NUMPAD5:		keySym = XK_KP_5; break;
    case WXK_NUMPAD6:		keySym = XK_KP_6; break;
    case WXK_NUMPAD7:		keySym = XK_KP_7; break;
    case WXK_NUMPAD8:		keySym = XK_KP_8; break;
    case WXK_NUMPAD9:		keySym = XK_KP_9; break;
    case WXK_MULTIPLY:		keySym = XK_KP_Multiply; break;
    case WXK_ADD:		keySym = XK_KP_Add; break;
    case WXK_SUBTRACT:		keySym = XK_KP_Subtract; break;
    case WXK_DECIMAL:		keySym = XK_KP_Decimal; break;
    case WXK_DIVIDE:		keySym = XK_KP_Divide; break;
    case WXK_F1:		keySym = XK_F1; break;
    case WXK_F2:		keySym = XK_F2; break;
    case WXK_F3:		keySym = XK_F3; break;
    case WXK_F4:		keySym = XK_F4; break;
    case WXK_F5:		keySym = XK_F5; break;
    case WXK_F6:		keySym = XK_F6; break;
    case WXK_F7:		keySym = XK_F7; break;
    case WXK_F8:		keySym = XK_F8; break;
    case WXK_F9:		keySym = XK_F9; break;
    case WXK_F10:		keySym = XK_F10; break;
    case WXK_F11:		keySym = XK_F11; break;
    case WXK_F12:		keySym = XK_F12; break;
    case WXK_F13:		keySym = XK_F13; break;
    case WXK_F14:		keySym = XK_F14; break;
    case WXK_F15:		keySym = XK_F15; break;
    case WXK_F16:		keySym = XK_F16; break;
    case WXK_F17:		keySym = XK_F17; break;
    case WXK_F18:		keySym = XK_F18; break;
    case WXK_F19:		keySym = XK_F19; break;
    case WXK_F20:		keySym = XK_F20; break;
    case WXK_F21:		keySym = XK_F21; break;
    case WXK_F22:		keySym = XK_F22; break;
    case WXK_F23:		keySym = XK_F23; break;
    case WXK_F24:		keySym = XK_F24; break;
    case WXK_NUMLOCK:		keySym = XK_Num_Lock; break;
    case WXK_SCROLL:		keySym = XK_Scroll_Lock; break;
    default:                    keySym = id <= 255 ? (KeySym)id : 0;
  } // switch
  return keySym;
}

// Read $HOME for what it says is home, if not
// read $USER or $LOGNAME for user name else determine
// the Real User, then determine the Real home dir.
static char * GetIniFile (char *dest, const char *filename)
{
  char *home = NULL;
  if (filename && wxIsAbsolutePath(filename))
  {
    strcpy(dest, filename);
  }
  else if ((home = wxGetUserHome("")) != NULL)
  {
    strcpy(dest, home);
    if (dest[strlen(dest) - 1] != '/')
      strcat (dest, "/");
    if (filename == NULL)
      {
        if ((filename = getenv ("XENVIRONMENT")) == NULL)
          filename = ".Xdefaults";
      }
    else if (*filename != '.')
      strcat (dest, ".");
    strcat (dest, filename);
  } else
  {
    dest[0] = '\0';    
  }
  return dest;
}

/*
 * Some colour manipulation routines
 */
 
void wxHSVToXColor(wxHSV *hsv,XColor *rgb)
   {
     int h = hsv->h;
     int s = hsv->s;
     int v = hsv->v;
     int r, g, b;
     int i, f;
     int p, q, t;
     s = (s * wxMAX_RGB) / wxMAX_SV;
     v = (v * wxMAX_RGB) / wxMAX_SV;
     if (h == 360) h = 0;
     if (s == 0) { h = 0; r = g = b = v; }
     i = h / 60;
     f = h % 60;
     p = v * (wxMAX_RGB - s) / wxMAX_RGB;
     q = v * (wxMAX_RGB - s * f / 60) / wxMAX_RGB;
     t = v * (wxMAX_RGB - s * (60 - f) / 60) / wxMAX_RGB;
     switch (i) 
        {
          case 0: r = v, g = t, b = p; break;
          case 1: r = q, g = v, b = p; break;
          case 2: r = p, g = v, b = t; break;
          case 3: r = p, g = q, b = v; break;
          case 4: r = t, g = p, b = v; break;
          case 5: r = v, g = p, b = q; break;
        }
     rgb->red = r << 8;
     rgb->green = g << 8;
     rgb->blue = b << 8;
   }

void wxXColorToHSV(wxHSV *hsv,XColor *rgb)
   {
     int r = rgb->red >> 8;
     int g = rgb->green >> 8;
     int b = rgb->blue >> 8;
     int maxv = wxMax3(r, g, b);
     int minv = wxMin3(r, g, b);
     int h, s, v;
     v = maxv;
     if (maxv) s = (maxv - minv) * wxMAX_RGB / maxv;
     else s = 0;
     if (s == 0) h = 0;
     else 
        {
	      int rc, gc, bc, hex;
	      rc = (maxv - r) * wxMAX_RGB / (maxv - minv);
	      gc = (maxv - g) * wxMAX_RGB / (maxv - minv);
	      bc = (maxv - b) * wxMAX_RGB / (maxv - minv);
	      if (r == maxv) { h = bc - gc, hex = 0; } 
	      else if (g == maxv) { h = rc - bc, hex = 2; } 
	           else if (b == maxv) { h = gc - rc, hex = 4; }
	      h = hex * 60 + (h * 60 / wxMAX_RGB);
	      if (h < 0) h += 360;
        }
     hsv->h = h;
     hsv->s = (s * wxMAX_SV) / wxMAX_RGB;
     hsv->v = (v * wxMAX_SV) / wxMAX_RGB;
   }

void wxAllocNearestColor(Display *d,Colormap cmp,XColor *xc)
   {
     int llp;

     int screen = DefaultScreen(d);
     int num_colors = DisplayCells(d,screen);

     XColor *color_defs = new XColor[num_colors];
     for(llp = 0;llp < num_colors;llp++) color_defs[llp].pixel = llp;
     XQueryColors(d,cmp,color_defs,num_colors);

     wxHSV hsv_defs, hsv;
     wxXColorToHSV(&hsv,xc);

     int diff, min_diff, pixel = 0;

     for(llp = 0;llp < num_colors;llp++)
        {
          wxXColorToHSV(&hsv_defs,&color_defs[llp]);
          diff = wxSIGN(wxH_WEIGHT * (hsv.h - hsv_defs.h)) +
                 wxSIGN(wxS_WEIGHT * (hsv.s - hsv_defs.s)) +
                 wxSIGN(wxV_WEIGHT * (hsv.v - hsv_defs.v));
          if (llp == 0) min_diff = diff;
          if (min_diff > diff) { min_diff = diff; pixel = llp; }
          if (min_diff == 0) break;
        }

     xc -> red = color_defs[pixel].red;
     xc -> green = color_defs[pixel].green;
     xc -> blue = color_defs[pixel].blue;
     xc -> flags = DoRed | DoGreen | DoBlue;
     if (!XAllocColor(d,cmp,xc))
        cout << "wxAllocNearestColor : Warning : Cannot find nearest color !\n";

     delete[] color_defs;
   }

void wxAllocColor(Display *d,Colormap cmp,XColor *xc)
   {
     if (!XAllocColor(d,cmp,xc))
        {
//          cout << "wxAllocColor : Warning : Can not allocate color, attempt find nearest !\n";
          wxAllocNearestColor(d,cmp,xc);
        }
   }



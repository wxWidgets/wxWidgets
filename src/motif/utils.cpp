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

#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <Xm/Xm.h>

#include "wx/motif/private.h"

// Get full hostname (eg. DoDo.BSn-Germany.crg.de)
bool wxGetHostName(char *buf, int maxSize)
{
    // TODO
    return FALSE;
}

// Get user ID e.g. jacs
bool wxGetUserId(char *buf, int maxSize)
{
    // TODO
    return FALSE;
}

// Get user name e.g. Julian Smart
bool wxGetUserName(char *buf, int maxSize)
{
    // TODO
    return FALSE;
}

int wxKill(long pid, int sig)
{
    // TODO
    return 0;
}

//
// Execute a program in an Interactive Shell
//
bool wxShell(const wxString& command)
{
    // TODO
    return FALSE;
}

// Get free memory in bytes, or -1 if cannot determine amount (e.g. on UNIX)
long wxGetFreeMemory()
{
    // TODO
    return 0;
}

void wxSleep(int nSecs)
{
    // TODO
}

// Consume all events until no more left
void wxFlushEvents()
{
}

// Output a debug message, in a system dependent fashion.
void wxDebugMsg(const char *fmt ...)
{
  va_list ap;
  static char buffer[512];

  if (!wxTheApp->GetWantDebugOutput())
    return ;

  va_start(ap, fmt);

  // wvsprintf(buffer,fmt,ap) ;
  // TODO: output buffer

  va_end(ap);
}

// Non-fatal error: pop up message box and (possibly) continue
void wxError(const wxString& msg, const wxString& title)
{
    // TODO
    wxExit();
}

// Fatal error: pop up message box and abort
void wxFatalError(const wxString& msg, const wxString& title)
{
    // TODO
}

// Emit a beeeeeep
void wxBell()
{
    // TODO
}

int wxGetOsVersion(int *majorVsn, int *minorVsn)
{
    // TODO
    return 0;
}

// Reading and writing resources (eg WIN.INI, .Xdefaults)
#if USE_RESOURCES
bool wxWriteResource(const wxString& section, const wxString& entry, const wxString& value, const wxString& file)
{
    // TODO
    return FALSE;
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
    // TODO
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
    *value = (int)strtol(s, NULL, 10);
    delete[] s; 
    return TRUE;
  }
  else return FALSE;
}
#endif // USE_RESOURCES

static int wxBusyCursorCount = 0;

// Set the cursor to the busy cursor for all windows
void wxBeginBusyCursor(wxCursor *cursor)
{
  wxBusyCursorCount ++;
  if (wxBusyCursorCount == 1)
  {
        // TODO
  }
  else
  {
        // TODO
  }
}

// Restore cursor to normal
void wxEndBusyCursor()
{
  if (wxBusyCursorCount == 0)
    return;
    
  wxBusyCursorCount --;
  if (wxBusyCursorCount == 0)
  {
    // TODO
  }
}

// TRUE if we're between the above two calls
bool wxIsBusy()
{
  return (wxBusyCursorCount > 0);
}    

char *wxGetUserHome (const wxString& user)
{
    // TODO
    return NULL;
}

// Check whether this window wants to process messages, e.g. Stop button
// in long calculations.
bool wxCheckForInterrupt(wxWindow *wnd)
{
    // TODO
    return FALSE;
}

void wxGetMousePosition( int* x, int* y )
{
    // TODO
};

// Return TRUE if we have a colour display
bool wxColourDisplay()
{
    // TODO
    return TRUE;
}

// Returns depth of screen
int wxDisplayDepth()
{
    // TODO
    return 0;
}

// Get size of display
void wxDisplaySize(int *width, int *height)
{
    // TODO
}

/* Configurable display in Motif */
static WXDisplay *gs_currentDisplay = NULL;
static wxString gs_displayName;

WXDisplay *wxGetDisplay()
{
  if (gs_currentDisplay)
    return gs_currentDisplay;

  return XtDisplay ((Widget) wxTheApp->GetTopLevelWidget());
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

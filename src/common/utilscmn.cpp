/////////////////////////////////////////////////////////////////////////////
// Name:        utilscmn.cpp
// Purpose:     Miscellaneous utility functions and classes
// Author:      Julian Smart
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "utils.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/string.h"
    #include "wx/utils.h"
    #include "wx/intl.h"
    #include "wx/log.h"

    #if wxUSE_GUI
        #include "wx/window.h"
        #include "wx/menu.h"
        #include "wx/frame.h"
        #include "wx/msgdlg.h"
        #include "wx/textdlg.h"
        #if wxUSE_ACCEL
            #include "wx/menuitem.h"
            #include "wx/accel.h"
        #endif // wxUSE_ACCEL
    #endif // wxUSE_GUI
#endif // WX_PRECOMP

#include "wx/process.h"
#include "wx/txtstrm.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(__WATCOMC__)
    #if !(defined(_MSC_VER) && (_MSC_VER > 800))
        #include <errno.h>
    #endif
#endif

#if wxUSE_GUI
    #include "wx/colordlg.h"
#endif // wxUSE_GUI

#include <time.h>

#ifndef __MWERKS__
    #include <sys/types.h>
    #include <sys/stat.h>
#endif

#ifdef __SALFORDC__
    #include <clib.h>
#endif

#ifdef __WXMSW__
    #include "windows.h"
#endif

// ----------------------------------------------------------------------------
// function protoypes
// ----------------------------------------------------------------------------

#if wxUSE_GUI
    static wxWindow *wxFindWindowByLabel1(const wxString& title, wxWindow *parent);
    static wxWindow *wxFindWindowByName1 (const wxString& title, wxWindow *parent);
#endif // wxUSE_GUI

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// string functions
// ----------------------------------------------------------------------------

#ifdef __WXMAC__
int strcasecmp(const char *str_1, const char *str_2)
{
  register char c1, c2;
  do {
    c1 = tolower(*str_1++);
    c2 = tolower(*str_2++);
  } while ( c1 && (c1 == c2) );

  return c1 - c2;
}

int strncasecmp(const char *str_1, const char *str_2, size_t maxchar)
{

  register char c1, c2;
  while( maxchar--)
  {
    c1 = tolower(*str_1++);
    c2 = tolower(*str_2++);

    if ( !c1 || c1!=c2 )
                  return c1 - c2;

  } ;

  return 0 ;

}
#endif // wxMAC

#if defined( __VMS__ ) && ( __VMS_VER < 70000000 )
// we have no strI functions under VMS, therefore I have implemented
// an inefficient but portable version: convert copies of strings to lowercase
// and then use the normal comparison
static void myLowerString(char *s)
{
  while(*s){
    if(isalpha(*s)) *s = (char)tolower(*s);
    s++;
  }
}

int strcasecmp(const char *str_1, const char *str_2)
{
  char *temp1 = new char[strlen(str_1)+1];
  char *temp2 = new char[strlen(str_2)+1];
  strcpy(temp1,str_1);
  strcpy(temp2,str_2);
  myLowerString(temp1);
  myLowerString(temp2);

  int result = wxStrcmp(temp1,temp2);
  delete[] temp1;
  delete[] temp2;

  return(result);
}

int strncasecmp(const char *str_1, const char *str_2, size_t maxchar)
{
  char *temp1 = new char[strlen(str_1)+1];
  char *temp2 = new char[strlen(str_2)+1];
  strcpy(temp1,str_1);
  strcpy(temp2,str_2);
  myLowerString(temp1);
  myLowerString(temp2);

  int result = strncmp(temp1,temp2,maxchar);
  delete[] temp1;
  delete[] temp2;

  return(result);
}
#endif // __VMS__

#ifdef __WINDOWS__

#ifndef __GNUWIN32__
#ifndef __MWERKS__
#define strcasecmp stricmp
#define strncasecmp strnicmp
#else
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif
#endif

#else

#ifdef __EMX__
#define strcasecmp stricmp
#define strncasecmp strnicmp
#endif

// This declaration is missing in SunOS!
// (Yes, I know it is NOT ANSI-C but its in BSD libc)
#if defined(__xlC) || defined(__AIX__) || defined(__GNUG__)
extern "C"
{
  int strcasecmp (const char *, const char *);
  int strncasecmp (const char *, const char *, size_t);
}
#endif
#endif  /* __WXMSW__ */

#ifdef __WXPM__
#define strcasecmp stricmp
#define strncasecmp strnicmp
#endif

wxChar *
copystring (const wxChar *s)
{
  if (s == NULL) s = wxT("");
  size_t len = wxStrlen (s) + 1;

  wxChar *news = new wxChar[len];
  memcpy (news, s, len * sizeof(wxChar));    // Should be the fastest

  return news;
}

// Id generation
static long wxCurrentId = 100;

long
wxNewId (void)
{
  return wxCurrentId++;
}

long
wxGetCurrentId(void) { return wxCurrentId; }

void
wxRegisterId (long id)
{
  if (id >= wxCurrentId)
    wxCurrentId = id + 1;
}

void
StringToFloat (wxChar *s, float *number)
{
  if (s && *s && number)
    *number = (float) wxStrtod (s, (wxChar **) NULL);
}

void
StringToDouble (wxChar *s, double *number)
{
  if (s && *s && number)
    *number = wxStrtod (s, (wxChar **) NULL);
}

wxChar *
FloatToString (float number, const wxChar *fmt)
{
  static wxChar buf[256];

//  sprintf (buf, "%.2f", number);
  wxSprintf (buf, fmt, number);
  return buf;
}

wxChar *
DoubleToString (double number, const wxChar *fmt)
{
  static wxChar buf[256];

  wxSprintf (buf, fmt, number);
  return buf;
}

void
StringToInt (wxChar *s, int *number)
{
  if (s && *s && number)
    *number = (int) wxStrtol (s, (wxChar **) NULL, 10);
}

void
StringToLong (wxChar *s, long *number)
{
  if (s && *s && number)
    *number = wxStrtol (s, (wxChar **) NULL, 10);
}

wxChar *
IntToString (int number)
{
  static wxChar buf[20];

  wxSprintf (buf, wxT("%d"), number);
  return buf;
}

wxChar *
LongToString (long number)
{
  static wxChar buf[20];

  wxSprintf (buf, wxT("%ld"), number);
  return buf;
}

// Array used in DecToHex conversion routine.
static wxChar hexArray[] = wxT("0123456789ABCDEF");

// Convert 2-digit hex number to decimal
int wxHexToDec(const wxString& buf)
{
  int firstDigit, secondDigit;

  if (buf.GetChar(0) >= wxT('A'))
    firstDigit = buf.GetChar(0) - wxT('A') + 10;
  else
    firstDigit = buf.GetChar(0) - wxT('0');

  if (buf.GetChar(1) >= wxT('A'))
    secondDigit = buf.GetChar(1) - wxT('A') + 10;
  else
    secondDigit = buf.GetChar(1) - wxT('0');

  return firstDigit * 16 + secondDigit;
}

// Convert decimal integer to 2-character hex string
void wxDecToHex(int dec, wxChar *buf)
{
  int firstDigit = (int)(dec/16.0);
  int secondDigit = (int)(dec - (firstDigit*16.0));
  buf[0] = hexArray[firstDigit];
  buf[1] = hexArray[secondDigit];
  buf[2] = 0;
}

// Convert decimal integer to 2-character hex string
wxString wxDecToHex(int dec)
{
    wxChar buf[3];
    wxDecToHex(dec, buf);
    return wxString(buf);
}

// Match a string INDEPENDENT OF CASE
bool
StringMatch (char *str1, char *str2, bool subString, bool exact)
{
  if (str1 == NULL || str2 == NULL)
    return FALSE;
  if (str1 == str2)
    return TRUE;

  if (subString)
    {
      int len1 = strlen (str1);
      int len2 = strlen (str2);
      int i;

      // Search for str1 in str2
      // Slow .... but acceptable for short strings
      for (i = 0; i <= len2 - len1; i++)
        {
          if (strncasecmp (str1, str2 + i, len1) == 0)
            return TRUE;
        }
    }
  else if (exact)
    {
      if (strcasecmp (str1, str2) == 0)
        return TRUE;
    }
  else
    {
      int len1 = strlen (str1);
      int len2 = strlen (str2);

      if (strncasecmp (str1, str2, wxMin (len1, len2)) == 0)
        return TRUE;
    }

  return FALSE;
}

// Return the current date/time
// [volatile]
wxString wxNow()
{
  time_t now = time((time_t *) NULL);
  char *date = ctime(&now);
  date[24] = '\0';
  return wxString(date);
}

#if wxUSE_GUI

// ----------------------------------------------------------------------------
// Menu accelerators related functions
// ----------------------------------------------------------------------------

wxChar *wxStripMenuCodes (wxChar *in, wxChar *out)
{
  if (!in)
    return (wxChar *) NULL;

  if (!out)
    out = copystring(in);

  wxChar *tmpOut = out;

  while (*in)
    {
      if (*in == wxT('&'))
        {
          // Check && -> &, &x -> x
          if (*++in == wxT('&'))
            *out++ = *in++;
        }
      else if (*in == wxT('\t'))
        {
          // Remove all stuff after \t in X mode, and let the stuff as is
          // in Windows mode.
          // Accelerators are handled in wx_item.cc for Motif, and are not
          // YET supported in XView
          break;
        }
      else
        *out++ = *in++;
    }                                // while

  *out = wxT('\0');

  return tmpOut;
}

wxString wxStripMenuCodes(const wxString& str)
{
    wxChar *buf = new wxChar[str.Length() + 1];
    wxStripMenuCodes(WXSTRINGCAST str, buf);
    wxString str1(buf);
    delete[] buf;
    return str1;
}

#if wxUSE_ACCEL

// return wxAcceleratorEntry for the given menu string or NULL if none
// specified
wxAcceleratorEntry *wxGetAccelFromString(const wxString& label)
{
    // check for accelerators: they are given after '\t'
    int posTab = label.Find(wxT('\t'));
    if ( posTab != wxNOT_FOUND ) {
        // parse the accelerator string
        int keyCode = 0;
        int accelFlags = wxACCEL_NORMAL;
        wxString current;
        for ( size_t n = (size_t)posTab + 1; n < label.Len(); n++ ) {
            if ( (label[n] == '+') || (label[n] == '-') ) {
                if ( current == _("ctrl") )
                    accelFlags |= wxACCEL_CTRL;
                else if ( current == _("alt") )
                    accelFlags |= wxACCEL_ALT;
                else if ( current == _("shift") )
                    accelFlags |= wxACCEL_SHIFT;
                else {
                    wxLogDebug(wxT("Unknown accel modifier: '%s'"),
                               current.c_str());
                }

                current.Empty();
            }
            else {
                current += wxTolower(label[n]);
            }
        }

        if ( current.IsEmpty() ) {
            wxLogDebug(wxT("No accel key found, accel string ignored."));
        }
        else {
            if ( current.Len() == 1 ) {
                // it's a letter
                keyCode = wxToupper(current[0U]);
            }
            else {
                // is it a function key?
                if ( current[0U] == 'f' && isdigit(current[1U]) &&
                     (current.Len() == 2 ||
                     (current.Len() == 3 && isdigit(current[2U]))) ) {
                    int n;
                    wxSscanf(current.c_str() + 1, wxT("%d"), &n);

                    keyCode = WXK_F1 + n - 1;
                }
                else {
#if 0 // this is not supported by GTK+, apparently
                    // several special cases
                    current.MakeUpper();
                    if ( current == wxT("DEL") ) {
                        keyCode = VK_DELETE;
                    }
                    else if ( current == wxT("PGUP") ) {
                        keyCode = VK_PRIOR;
                    }
                    else if ( current == wxT("PGDN") ) {
                        keyCode = VK_NEXT;
                    }
                    else
#endif // 0
                    {
                        wxLogDebug(wxT("Unrecognized accel key '%s', accel "
                                       "string ignored."), current.c_str());
                    }
                }
            }
        }

        if ( keyCode ) {
            // we do have something
            return new wxAcceleratorEntry(accelFlags, keyCode);
        }
    }

    return (wxAcceleratorEntry *)NULL;
}

#endif // wxUSE_ACCEL

// ----------------------------------------------------------------------------
// Window search functions
// ----------------------------------------------------------------------------

/*
 * If parent is non-NULL, look through children for a label or title
 * matching the specified string. If NULL, look through all top-level windows.
 *
 */

wxWindow *
wxFindWindowByLabel (const wxString& title, wxWindow * parent)
{
    if (parent)
    {
        return wxFindWindowByLabel1(title, parent);
    }
    else
    {
        for ( wxWindowList::Node * node = wxTopLevelWindows.GetFirst();
              node;
              node = node->GetNext() )
        {
            wxWindow *win = node->GetData();
            wxWindow *retwin = wxFindWindowByLabel1 (title, win);
            if (retwin)
                return retwin;
        }                        // for()

    }
    return (wxWindow *) NULL;
}

// Recursive
static wxWindow *
wxFindWindowByLabel1 (const wxString& title, wxWindow * parent)
{
    if (parent)
    {
        if (parent->GetLabel() == title)
            return parent;
    }

    if (parent)
    {
        for ( wxWindowList::Node * node = parent->GetChildren().GetFirst();
              node;
              node = node->GetNext() )
        {
            wxWindow *win = (wxWindow *)node->GetData();
            wxWindow *retwin = wxFindWindowByLabel1 (title, win);
            if (retwin)
                return retwin;
        }

    }

    return (wxWindow *) NULL;                        // Not found
}

/*
 * If parent is non-NULL, look through children for a name
 * matching the specified string. If NULL, look through all top-level windows.
 *
 */

wxWindow *
wxFindWindowByName (const wxString& title, wxWindow * parent)
{
    if (parent)
    {
        return wxFindWindowByName1 (title, parent);
    }
    else
    {
        for ( wxWindowList::Node * node = wxTopLevelWindows.GetFirst();
              node;
              node = node->GetNext() )
        {
            wxWindow *win = node->GetData();
            wxWindow *retwin = wxFindWindowByName1 (title, win);
            if (retwin)
                return retwin;
        }

    }

    // Failed? Try by label instead.
    return wxFindWindowByLabel(title, parent);
}

// Recursive
static wxWindow *
wxFindWindowByName1 (const wxString& title, wxWindow * parent)
{
  if (parent)
    {
            if ( parent->GetName() == title )
                        return parent;
    }

  if (parent)
    {
      for (wxNode * node = parent->GetChildren().First (); node; node = node->Next ())
        {
          wxWindow *win = (wxWindow *) node->Data ();
          wxWindow *retwin = wxFindWindowByName1 (title, win);
          if (retwin)
            return retwin;
        }                        // for()

    }

  return (wxWindow *) NULL;                        // Not found

}

// Returns menu item id or -1 if none.
int
wxFindMenuItemId (wxFrame * frame, const wxString& menuString, const wxString& itemString)
{
  wxMenuBar *menuBar = frame->GetMenuBar ();
  if (!menuBar)
    return -1;
  return menuBar->FindMenuItem (menuString, itemString);
}

#endif // wxUSE_GUI

/*
On Fri, 21 Jul 1995, Paul Craven wrote:

> Is there a way to find the path of running program's executable? I can get
> my home directory, and the current directory, but I don't know how to get the
> executable directory.
>

The code below (warty as it is), does what you want on most Unix,
DOS, and Mac platforms (it's from the ALS Prolog main).

|| Ken Bowen      Applied Logic Systems, Inc.         PO Box 180,
||====            Voice:  +1 (617)965-9191            Newton Centre,
||                FAX:    +1 (617)965-1636            MA  02159  USA
                  Email:  ken@als.com        WWW: http://www.als.com
------------------------------------------------------------------------
*/

// This code is commented out but it may be integrated with wxWin at
// a later date, after testing. Thanks Ken!
#if 0

/*--------------------------------------------------------------------*
 | whereami is given a filename f in the form:  whereami(argv[0])
 | It returns the directory in which the executable file (containing
 | this code [main.c] ) may be found.  A dot will be returned to indicate
 | the current directory.
 *--------------------------------------------------------------------*/

static void
whereami(name)
    char *name;
{
    register char *cutoff = NULL;        /* stifle -Wall */
    register char *s;
    register char *t;
    int   cc;
    char  ebuf[4096];

    /*
     * See if the file is accessible either through the current directory
     * or through an absolute path.
     */

    if (access(name, R_OK) == 0) {

        /*-------------------------------------------------------------*
         * The file was accessible without any other work.  But the current
         * working directory might change on us, so if it was accessible
         * through the cwd, then we should get it for later accesses.
         *-------------------------------------------------------------*/

        t = imagedir;
        if (!absolute_pathname(name)) {
#if defined(DOS) || defined(__WIN32__)
            int   drive;
            char *newrbuf;

            newrbuf = imagedir;
#ifndef __DJGPP__
            if (*(name + 1) == ':') {
                if (*name >= 'a' && *name <= 'z')
                    drive = (int) (*name - 'a' + 1);
                else
                    drive = (int) (*name - 'A' + 1);
                *newrbuf++ = *name;
                *newrbuf++ = *(name + 1);
                *newrbuf++ = DIR_SEPARATOR;
            }
            else {
                drive = 0;
                *newrbuf++ = DIR_SEPARATOR;
            }
            if (getcwd(newrbuf, drive) == 0) {        /* } */
#else
            if (getcwd(newrbuf, 1024) == 0) {        /* } */
#endif
#else  /* DOS */
#ifdef HAVE_GETWD
            if (getwd(imagedir) == 0) {                /* } */
#else  /* !HAVE_GETWD */
            if (getcwd(imagedir, 1024) == 0) {
#endif /* !HAVE_GETWD */
#endif /* DOS */
                fatal_error(FE_GETCWD, 0);
            }
            for (; *t; t++)        /* Set t to end of buffer */
                ;
            if (*(t - 1) == DIR_SEPARATOR)        /* leave slash if already
                                                 * last char
                                                 */
                cutoff = t - 1;
            else {
                cutoff = t;        /* otherwise put one in */
                *t++ = DIR_SEPARATOR;
            }
        }
#if (!defined(__MAC__) && !defined(__DJGPP__) && !defined(__GO32__) && !defined(__WIN32__))
        else
                (*t++ = DIR_SEPARATOR);
#endif

        /*-------------------------------------------------------------*
         * Copy the rest of the string and set the cutoff if it was not
         * already set.  If the first character of name is a slash, cutoff
         * is not presently set but will be on the first iteration of the
         * loop below.
         *-------------------------------------------------------------*/

        for ((*name == DIR_SEPARATOR ? (s = name+1) : (s = name));;) {
            if (*s == DIR_SEPARATOR)
                        cutoff = t;
            if (!(*t++ = *s++))
                        break;
        }

    }
    else {

        /*-------------------------------------------------------------*
         * Get the path list from the environment.  If the path list is
         * inaccessible for any reason, leave with fatal error.
         *-------------------------------------------------------------*/

#ifdef __MAC__
        if ((s = getenv("Commands")) == (char *) 0)
#else
        if ((s = getenv("PATH")) == (char *) 0)
#endif
            fatal_error(FE_PATH, 0);

        /*
         * Copy path list into ebuf and set the source pointer to the
         * beginning of this buffer.
         */

        strcpy(ebuf, s);
        s = ebuf;

        for (;;) {
            t = imagedir;
            while (*s && *s != PATH_SEPARATOR)
                *t++ = *s++;
            if (t > imagedir && *(t - 1) == DIR_SEPARATOR)
                ;                /* do nothing -- slash already is in place */
            else
                *t++ = DIR_SEPARATOR;        /* put in the slash */
            cutoff = t - 1;        /* set cutoff */
            strcpy(t, name);
            if (access(imagedir, R_OK) == 0)
                break;

            if (*s)
                s++;                /* advance source pointer */
            else
                fatal_error(FE_INFND, 0);
        }

    }

    /*-------------------------------------------------------------*
     | At this point the full pathname should exist in imagedir and
     | cutoff should be set to the final slash.  We must now determine
     | whether the file name is a symbolic link or not and chase it down
     | if it is.  Note that we reuse ebuf for getting the link.
     *-------------------------------------------------------------*/

#ifdef HAVE_SYMLINK
    while ((cc = readlink(imagedir, ebuf, 512)) != -1) {
        ebuf[cc] = 0;
        s = ebuf;
        if (*s == DIR_SEPARATOR) {
            t = imagedir;
        }
        else {
            t = cutoff + 1;
        }
        for (;;) {
            if (*s == DIR_SEPARATOR)
                cutoff = t;        /* mark the last slash seen */
            if (!(*t++ = *s++))        /* copy the character */
                break;
        }
    }

#endif /* HAVE_SYMLINK */

    strcpy(imagename, cutoff + 1);        /* keep the image name */
    *(cutoff + 1) = 0;                /* chop off the filename part */
}

#endif

#if wxUSE_GUI

// ----------------------------------------------------------------------------
// GUI helpers
// ----------------------------------------------------------------------------

/*
 * N.B. these convenience functions must be separate from msgdlgg.cpp, textdlgg.cpp
 * since otherwise the generic code may be pulled in unnecessarily.
 */

int wxMessageBox(const wxString& message, const wxString& caption, long style,
                 wxWindow *parent, int WXUNUSED(x), int WXUNUSED(y) )
{
    wxMessageDialog dialog(parent, message, caption, style);

    int ans = dialog.ShowModal();
    switch ( ans )
    {
        case wxID_OK:
            return wxOK;
        case wxID_YES:
            return wxYES;
        case wxID_NO:
            return wxNO;
        default:
        case wxID_CANCEL:
            return wxCANCEL;
    }
}

#if wxUSE_TEXTDLG
wxString wxGetTextFromUser(const wxString& message, const wxString& caption,
                        const wxString& defaultValue, wxWindow *parent,
                        int x, int y, bool WXUNUSED(centre) )
{
    wxString str;
    wxTextEntryDialog dialog(parent, message, caption, defaultValue, wxOK|wxCANCEL, wxPoint(x, y));
    if (dialog.ShowModal() == wxID_OK)
    {
        str = dialog.GetValue();
    }

    return str;
}

wxString wxGetPasswordFromUser(const wxString& message,
                               const wxString& caption,
                               const wxString& defaultValue,
                               wxWindow *parent)
{
    wxString str;
    wxTextEntryDialog dialog(parent, message, caption, defaultValue,
                             wxOK | wxCANCEL | wxTE_PASSWORD);
    if ( dialog.ShowModal() == wxID_OK )
    {
        str = dialog.GetValue();
    }

    return str;
}

#endif // wxUSE_TEXTDLG

wxColour wxGetColourFromUser(wxWindow *parent, const wxColour& colInit)
{
      wxColourData data;
      data.SetChooseFull(TRUE);
      if ( colInit.Ok() )
      {
          data.SetColour((wxColour &)colInit); // const_cast
      }

      wxColour colRet;
      wxColourDialog dialog(parent, &data);
      if ( dialog.ShowModal() == wxID_OK )
      {
          colRet = dialog.GetColourData().GetColour();
      }
      //else: leave it invalid

      return colRet;
}

// ----------------------------------------------------------------------------
// missing C RTL functions (FIXME shouldn't be here at all)
// ----------------------------------------------------------------------------

#ifdef __MWERKS__
char *strdup(const char *s)
{
        return strcpy( (char*) malloc( strlen( s ) + 1 ) , s ) ;
}

int isascii( int c )
{
        return ( c >= 0 && c < 128 ) ;
}
#endif // __MWERKS__

// ----------------------------------------------------------------------------
// misc functions
// ----------------------------------------------------------------------------

void wxEnableTopLevelWindows(bool enable)
{
    wxWindowList::Node *node;
    for ( node = wxTopLevelWindows.GetFirst(); node; node = node->GetNext() )
        node->GetData()->Enable(enable);
}

static void wxFindDisabledWindows(wxWindowList& winDisabled, wxWindow *win)
{
    wxWindowList::Node *node;
    for ( node = win->GetChildren().GetFirst(); node; node = node->GetNext() )
    {
        wxWindow *child = node->GetData();
        wxFindDisabledWindows(winDisabled, child);

        if ( child->IsEnabled() )
        {
            winDisabled.Append(child);
            child->Disable();
        }
    }
}

wxWindowDisabler::wxWindowDisabler(wxWindow *winToSkip)
{
    // remember all windows we're going to (temporarily) disable
    m_winDisabled = new wxWindowList;

    wxWindowList::Node *node;
    for ( node = wxTopLevelWindows.GetFirst(); node; node = node->GetNext() )
    {
        wxWindow *winTop = node->GetData();
        if ( winTop->IsEnabled() )
        {
            wxFindDisabledWindows(*m_winDisabled, winTop);

            m_winDisabled->Append(winTop);
            winTop->Disable();
        }
    }

    if ( winToSkip && m_winDisabled->Find(winToSkip) )
    {
        // always enable ourselves
        m_winDisabled->DeleteObject(winToSkip);
        winToSkip->Enable();
    }
}

wxWindowDisabler::~wxWindowDisabler()
{
    wxWindowList::Node *node;
    for ( node = m_winDisabled->GetFirst(); node; node = node->GetNext() )
    {
        node->GetData()->Enable();
    }

    delete m_winDisabled;
}

// Yield to other apps/messages and disable user input to all windows except
// the given one
bool wxSafeYield(wxWindow *win)
{
    wxWindowDisabler wd;

    bool rc = wxYield();

    return rc;
}

// Don't synthesize KeyUp events holding down a key and producing KeyDown
// events with autorepeat. On by default and always on in wxMSW. wxGTK version
// in utilsgtk.cpp.
#ifndef __WXGTK__
bool wxSetDetectableAutoRepeat( bool WXUNUSED(flag) )
{
    return TRUE;    // detectable auto-repeat is the only mode MSW supports
}
#endif // !wxGTK

#endif // wxUSE_GUI

// ----------------------------------------------------------------------------
// network and user id functions
// ----------------------------------------------------------------------------

// Get Full RFC822 style email address
bool wxGetEmailAddress(wxChar *address, int maxSize)
{
    wxString email = wxGetEmailAddress();
    if ( !email )
        return FALSE;

    wxStrncpy(address, email, maxSize - 1);
    address[maxSize - 1] = wxT('\0');

    return TRUE;
}

wxString wxGetEmailAddress()
{
    wxString email;

    wxString host = wxGetHostName();
    if ( !!host )
    {
        wxString user = wxGetUserId();
        if ( !!user )
        {
            wxString email(user);
            email << wxT('@') << host;
        }
    }

    return email;
}

wxString wxGetUserId()
{
    static const int maxLoginLen = 256; // FIXME arbitrary number

    wxString buf;
    bool ok = wxGetUserId(buf.GetWriteBuf(maxLoginLen), maxLoginLen);
    buf.UngetWriteBuf();

    if ( !ok )
        buf.Empty();

    return buf;
}

wxString wxGetUserName()
{
    static const int maxUserNameLen = 1024; // FIXME arbitrary number

    wxString buf;
    bool ok = wxGetUserName(buf.GetWriteBuf(maxUserNameLen), maxUserNameLen);
    buf.UngetWriteBuf();

    if ( !ok )
        buf.Empty();

    return buf;
}

wxString wxGetHostName()
{
    static const size_t hostnameSize = 257;

    wxString buf;
    bool ok = wxGetHostName(buf.GetWriteBuf(hostnameSize), hostnameSize);

    buf.UngetWriteBuf();

    if ( !ok )
        buf.Empty();

    return buf;
}

wxString wxGetFullHostName()
{
    static const size_t hostnameSize = 257;

    wxString buf;
    bool ok = wxGetFullHostName(buf.GetWriteBuf(hostnameSize), hostnameSize);

    buf.UngetWriteBuf();

    if ( !ok )
        buf.Empty();

    return buf;
}

wxString wxGetHomeDir()
{
    wxString home;
    wxGetHomeDir(&home);

    return home;
}

#if 0

wxString wxGetCurrentDir()
{
    wxString dir;
    size_t len = 1024;
    bool ok;
    do
    {
        ok = getcwd(dir.GetWriteBuf(len + 1), len) != NULL;
        dir.UngetWriteBuf();

        if ( !ok )
        {
            if ( errno != ERANGE )
            {
                wxLogSysError(_T("Failed to get current directory"));

                return wxEmptyString;
            }
            else
            {
                // buffer was too small, retry with a larger one
                len *= 2;
            }
        }
        //else: ok
    } while ( !ok );

    return dir;
}

#endif // 0

// ----------------------------------------------------------------------------
// wxExecute
// ----------------------------------------------------------------------------

long wxExecute(const wxString& command, wxArrayString& output)
{
    // create a wxProcess which will capture the output
    wxProcess *process = new wxProcess;
    process->Redirect();

    long rc = wxExecute(command, TRUE /* sync */, process);
    if ( rc != -1 )
    {
        wxInputStream& is = *process->GetInputStream();
        wxTextInputStream tis(is);
        while ( !is.Eof() )
        {
            wxString line = tis.ReadLine();
            if ( is.LastError() )
                break;

            output.Add(line);
        }
    }

    return rc;
}

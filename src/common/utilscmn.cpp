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
#include "wx/utils.h"
#include "wx/window.h"
#include "wx/menu.h"
#include "wx/frame.h"
#include "wx/msgdlg.h"
#include "wx/textdlg.h"
#endif

#include "wx/ioswrap.h"

#if wxUSE_IOSTREAMH
    #include <fstream.h>
#else
    #include <fstream>
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(__WATCOMC__)
    #if !(defined(_MSC_VER) && (_MSC_VER > 800))
        #include <errno.h>
    #endif
#endif
#include <time.h>
#ifndef __MWERKS__
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifdef __SALFORDC__
#include <clib.h>
#endif

// Pattern matching code.
// Yes, this path is deliberate (for Borland compilation)
#ifdef wx_mac /* MATTHEW: [5] Mac doesn't like paths with "/" */
#include "glob.inc"
#else
#include "../common/glob.inc"
#endif

#ifdef __WXMSW__
#include "windows.h"
#endif

#define _MAXPATHLEN 500

extern wxChar *wxBuffer;

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static wxWindow *wxFindWindowByLabel1(const wxString& title, wxWindow * parent);
static wxWindow *wxFindWindowByName1 (const wxString& title, wxWindow * parent);

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
#endif
#ifdef __VMS__
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

  int result = strcmp(temp1,temp2);
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
#endif

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


wxChar *
copystring (const wxChar *s)
{
  if (s == NULL) s = _T("");
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

  wxSprintf (buf, _T("%d"), number);
  return buf;
}

wxChar *
LongToString (long number)
{
  static wxChar buf[20];

  wxSprintf (buf, _T("%ld"), number);
  return buf;
}

// Array used in DecToHex conversion routine.
static wxChar hexArray[] = _T("0123456789ABCDEF");

// Convert 2-digit hex number to decimal
int wxHexToDec(const wxString& buf)
{
  int firstDigit, secondDigit;

  if (buf.GetChar(0) >= _T('A'))
    firstDigit = buf.GetChar(0) - _T('A') + 10;
  else
    firstDigit = buf.GetChar(0) - _T('0');

  if (buf.GetChar(1) >= _T('A'))
    secondDigit = buf.GetChar(1) - _T('A') + 10;
  else
    secondDigit = buf.GetChar(1) - _T('0');

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

// Don't synthesize KeyUp events holding down a key and producing
// KeyDown events with autorepeat. On by default and always on
// on in wxMSW. wxGTK version in utilsgtk.cpp.
#ifndef __WXGTK__
bool wxSetDetectableAutoRepeat( bool WXUNUSED(flag) )
{
   return TRUE;          // detectable auto-repeat is the only mode MSW supports
}
#endif

// Return the current date/time
// [volatile]
wxString wxNow( void )
{
  time_t now = time((time_t *) NULL);
  char *date = ctime(&now);
  date[24] = '\0';
  return wxString(date);
}

/*
 * Strip out any menu codes
 */

wxChar *wxStripMenuCodes (wxChar *in, wxChar *out)
{
  if (!in)
    return (wxChar *) NULL;

  if (!out)
    out = copystring(in);

  wxChar *tmpOut = out;

  while (*in)
    {
      if (*in == _T('&'))
        {
          // Check && -> &, &x -> x
          if (*++in == _T('&'))
            *out++ = *in++;
        }
      else if (*in == _T('\t'))
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

  *out = _T('\0');

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

/*
 * Window search functions
 *
 */

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
void wxEnableTopLevelWindows(bool enable)
{
   wxWindowList::Node *node;
   for ( node = wxTopLevelWindows.GetFirst(); node; node = node->GetNext() )
      node->GetData()->Enable(enable);
}

// Yield to other apps/messages and disable user input
bool wxSafeYield(wxWindow *win)
{
   wxEnableTopLevelWindows(FALSE);
   // always enable ourselves
   if ( win )
      win->Enable(TRUE);
   bool rc = wxYield();
   wxEnableTopLevelWindows(TRUE);
   return rc;
}

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
            break;
        case wxID_YES:
            return wxYES;
            break;
        case wxID_NO:
            return wxNO;
            break;
        default:
        case wxID_CANCEL:
            return wxCANCEL;
            break;
    }
    return ans;
}

#if wxUSE_TEXTDLG
wxString wxGetTextFromUser(const wxString& message, const wxString& caption,
                        const wxString& defaultValue, wxWindow *parent,
                        int x, int y, bool WXUNUSED(centre) )
{
    wxTextEntryDialog dialog(parent, message, caption, defaultValue, wxOK|wxCANCEL, wxPoint(x, y));
    if (dialog.ShowModal() == wxID_OK)
        return dialog.GetValue();
    else
        return wxString("");
}
#endif // wxUSE_TEXTDLG

#ifdef __MWERKS__
char *strdup(const char *s)
{
        return strcpy( (char*) malloc( strlen( s ) + 1 ) , s ) ;
}

int isascii( int c )
{
        return ( c >= 0 && c < 128 ) ;
}
#endif

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
    address[maxSize - 1] = _T('\0');

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
            email << _T('@') << host;
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


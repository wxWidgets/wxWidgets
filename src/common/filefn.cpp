/////////////////////////////////////////////////////////////////////////////
// Name:        filefn.cpp
// Purpose:     File- and directory-related functions
// Author:      Julian Smart
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "filefn.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#include "wx/defs.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/defs.h"
#endif

#include "wx/utils.h"
#include <wx/intl.h>

// there are just too many of those...
#ifdef __VISUALC__
    #pragma warning(disable:4706)   // assignment within conditional expression
#endif // VC++

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
#else
    #include <stat.h>
    #include <unistd.h>
#endif

#ifdef __UNIX__
    #include <unistd.h>
    #include <dirent.h>
#endif

#ifdef __OS2__
    #include <direct.h>
    #include <process.h>
#endif
#ifdef __WINDOWS__
#if !defined( __GNUWIN32__ ) && !defined( __MWERKS__ ) && !defined(__SALFORDC__)
    #include <direct.h>
    #include <dos.h>
#endif // __WINDOWS__
#endif // native Win compiler

#ifdef __GNUWIN32__
    #ifndef __TWIN32__
        #include <sys/unistd.h>
    #endif

    #define stricmp strcasecmp
#endif

#ifdef __BORLANDC__ // Please someone tell me which version of Borland needs
                    // this (3.1 I believe) and how to test for it.
                    // If this works for Borland 4.0 as well, then no worries.
    #include <dir.h>
#endif

#ifdef __SALFORDC__
    #include <dir.h>
    #include <unix.h>
#endif

#include "wx/setup.h"
#include "wx/log.h"

// No, Cygwin doesn't appear to have fnmatch.h after all.
#if defined(HAVE_FNMATCH_H)
    #include "fnmatch.h"
#endif

#ifdef __WINDOWS__
    #include "windows.h"
#endif

#define _MAXPATHLEN 500

extern wxChar *wxBuffer;
#ifdef __WXMAC__
    extern wxChar gwxMacFileName[] ;
    extern wxChar gwxMacFileName2[] ;
    extern wxChar gwxMacFileName3[] ;
#endif

#if !USE_SHARED_LIBRARIES
    IMPLEMENT_DYNAMIC_CLASS(wxPathList, wxStringList)
#endif

void wxPathList::Add (const wxString& path)
{
    wxStringList::Add (WXSTRINGCAST path);
}

// Add paths e.g. from the PATH environment variable
void wxPathList::AddEnvList (const wxString& envVariable)
{
  static const wxChar PATH_TOKS[] =
#ifdef __WINDOWS__
        _T(" ;"); // Don't seperate with colon in DOS (used for drive)
#else
        _T(" :;");
#endif

  wxChar *val = wxGetenv (WXSTRINGCAST envVariable);
  if (val && *val)
    {
      wxChar *s = copystring (val);
      wxChar *save_ptr, *token = wxStrtok (s, PATH_TOKS, &save_ptr);

      if (token)
        {
          Add (copystring (token));
          while (token)
            {
              if ((token = wxStrtok ((wxChar *) NULL, PATH_TOKS, &save_ptr)) != NULL)
                Add (wxString(token));
            }
        }
      delete[]s;
    }
}

// Given a full filename (with path), ensure that that file can
// be accessed again USING FILENAME ONLY by adding the path
// to the list if not already there.
void wxPathList::EnsureFileAccessible (const wxString& path)
{
    wxString path_only(wxPathOnly(path));
    if ( !path_only.IsEmpty() )
    {
        if ( !Member(path_only) )
            Add(path_only);
    }
}

bool wxPathList::Member (const wxString& path)
{
  for (wxNode * node = First (); node != NULL; node = node->Next ())
  {
      wxString path2((wxChar *) node->Data ());
      if (
#if defined(__WINDOWS__) || defined(__VMS__) || defined (__WXMAC__)
      // Case INDEPENDENT
          path.CompareTo (path2, wxString::ignoreCase) == 0
#else
      // Case sensitive File System
          path.CompareTo (path2) == 0
#endif
        )
        return TRUE;
  }
  return FALSE;
}

wxString wxPathList::FindValidPath (const wxString& file)
{
  if (wxFileExists (wxExpandPath(wxBuffer, file)))
    return wxString(wxBuffer);

  wxChar buf[_MAXPATHLEN];
  wxStrcpy(buf, wxBuffer);

  wxChar *filename = (wxChar*) NULL; /* shut up buggy egcs warning */
  filename = IsAbsolutePath (buf) ? wxFileNameFromPath (buf) : (wxChar *)buf;

  for (wxNode * node = First (); node; node = node->Next ())
    {
      wxChar *path = (wxChar *) node->Data ();
      wxStrcpy (wxBuffer, path);
      wxChar ch = wxBuffer[wxStrlen(wxBuffer)-1];
      if (ch != _T('\\') && ch != _T('/'))
        wxStrcat (wxBuffer, _T("/"));
      wxStrcat (wxBuffer, filename);
#ifdef __WINDOWS__
      Unix2DosFilename (wxBuffer);
#endif
      if (wxFileExists (wxBuffer))
      {
        return wxString(wxBuffer);        // Found!
      }
    }                                // for()

  return wxString(_T(""));                    // Not found
}

wxString wxPathList::FindAbsoluteValidPath (const wxString& file)
{
  wxString f = FindValidPath(file);
  if (wxIsAbsolutePath(f))
    return f;
  else
  {
    wxChar buf[500];
    wxGetWorkingDirectory(buf, 499);
    int len = (int)wxStrlen(buf);
    wxChar lastCh = 0;
    if (len > 0)
      lastCh = buf[len-1];
    if (lastCh != _T('/') && lastCh != _T('\\'))
    {
#ifdef __WINDOWS__
      wxStrcat(buf, _T("\\"));
#else
      wxStrcat(buf, _T("/"));
#endif
    }
    wxStrcat(buf, (const wxChar *)f);
    wxStrcpy(wxBuffer, buf);
    return wxString(wxBuffer);
  }
}

bool
wxFileExists (const wxString& filename)
{
#ifdef __GNUWIN32__ // (fix a B20 bug)
  if (GetFileAttributes(filename) == 0xFFFFFFFF)
    return FALSE;
  else
    return TRUE;
#elif defined(__WXMAC__)
          struct stat stbuf;
        wxStrcpy( gwxMacFileName , filename ) ;
        wxUnix2MacFilename( gwxMacFileName ) ;
          if (gwxMacFileName && stat (WXSTRINGCAST gwxMacFileName, &stbuf) == 0)
            return TRUE;
    return FALSE ;
#else

#ifdef __SALFORDC__
  struct _stat stbuf;
#else
  struct stat stbuf;
#endif

  if ((filename != _T("")) && stat (FNSTRINGCAST filename.fn_str(), &stbuf) == 0)
    return TRUE;
  return FALSE;
#endif
}

/* Vadim's alternative implementation

// does the file exist?
bool wxFileExists(const char *pszFileName)
{
  struct stat st;
  return !access(pszFileName, 0) &&
         !stat(pszFileName, &st) &&
         (st.st_mode & S_IFREG);
}
*/

bool
wxIsAbsolutePath (const wxString& filename)
{
  if (filename != _T(""))
    {
      if (filename[0] == _T('/')
#ifdef __VMS__
      || (filename[0] == _T('[') && filename[1] != _T('.'))
#endif
#ifdef __WINDOWS__
      /* MSDOS */
      || filename[0] == _T('\\') || (wxIsalpha (filename[0]) && filename[1] == _T(':'))
#endif
        )
        return TRUE;
    }
  return FALSE;
}

/*
 * Strip off any extension (dot something) from end of file,
 * IF one exists. Inserts zero into buffer.
 *
 */

void wxStripExtension(wxChar *buffer)
{
  int len = wxStrlen(buffer);
  int i = len-1;
  while (i > 0)
  {
    if (buffer[i] == _T('.'))
    {
      buffer[i] = 0;
      break;
    }
    i --;
  }
}

void wxStripExtension(wxString& buffer)
{
  size_t len = buffer.Length();
  size_t i = len-1;
  while (i > 0)
  {
    if (buffer.GetChar(i) == _T('.'))
    {
      buffer = buffer.Left(i);
      break;
    }
    i --;
  }
}

// Destructive removal of /./ and /../ stuff
wxChar *wxRealPath (wxChar *path)
{
#ifdef __WXMSW__
  static const wxChar SEP = _T('\\');
  Unix2DosFilename(path);
#else
  static const wxChar SEP = _T('/');
#endif
  if (path[0] && path[1]) {
    /* MATTHEW: special case "/./x" */
    wxChar *p;
    if (path[2] == SEP && path[1] == _T('.'))
      p = &path[0];
    else
      p = &path[2];
    for (; *p; p++)
      {
        if (*p == SEP)
          {
            if (p[1] == _T('.') && p[2] == _T('.') && (p[3] == SEP || p[3] == _T('\0')))
              {
                wxChar *q;
                for (q = p - 1; q >= path && *q != SEP; q--);
                if (q[0] == SEP && (q[1] != _T('.') || q[2] != _T('.') || q[3] != SEP)
                    && (q - 1 <= path || q[-1] != SEP))
                  {
                    wxStrcpy (q, p + 3);
                    if (path[0] == _T('\0'))
                      {
                        path[0] = SEP;
                        path[1] = _T('\0');
                      }
#ifdef __WXMSW__
                    /* Check that path[2] is NULL! */
                    else if (path[1] == _T(':') && !path[2])
                      {
                        path[2] = SEP;
                        path[3] = _T('\0');
                      }
#endif
                    p = q - 1;
                  }
              }
            else if (p[1] == _T('.') && (p[2] == SEP || p[2] == _T('\0')))
              wxStrcpy (p, p + 2);
          }
      }
  }
  return path;
}

// Must be destroyed
wxChar *wxCopyAbsolutePath(const wxString& filename)
{
  if (filename == _T(""))
    return (wxChar *) NULL;

  if (! IsAbsolutePath(wxExpandPath(wxBuffer, filename))) {
    wxChar  buf[_MAXPATHLEN];
    buf[0] = _T('\0');
    wxGetWorkingDirectory(buf, WXSIZEOF(buf));
    wxChar ch = buf[wxStrlen(buf) - 1];
#ifdef __WXMSW__
    if (ch != _T('\\') && ch != _T('/'))
        wxStrcat(buf, _T("\\"));
#else
    if (ch != _T('/'))
        wxStrcat(buf, _T("/"));
#endif
    wxStrcat(buf, wxBuffer);
    return copystring( wxRealPath(buf) );
  }
  return copystring( wxBuffer );
}

/*-
 Handles:
   ~/ => home dir
   ~user/ => user's home dir
   If the environment variable a = "foo" and b = "bar" then:
   Unix:
        $a        =>        foo
        $a$b        =>        foobar
        $a.c        =>        foo.c
        xxx$a        =>        xxxfoo
        ${a}!        =>        foo!
        $(b)!        =>        bar!
        \$a        =>        \$a
   MSDOS:
        $a        ==>        $a
        $(a)        ==>        foo
        $(a)$b        ==>        foo$b
        $(a)$(b)==>        foobar
        test.$$        ==>        test.$$
 */

/* input name in name, pathname output to buf. */

wxChar *wxExpandPath(wxChar *buf, const wxChar *name)
{
    register wxChar *d, *s, *nm;
    wxChar          lnm[_MAXPATHLEN];
    int                                q;

    // Some compilers don't like this line.
//    const wxChar    trimchars[] = _T("\n \t");

    wxChar      trimchars[4];
    trimchars[0] = _T('\n');
    trimchars[1] = _T(' ');
    trimchars[2] = _T('\t');
    trimchars[3] = 0;

#ifdef __WXMSW__
     const wxChar     SEP = _T('\\');
#else
     const wxChar     SEP = _T('/');
#endif
    buf[0] = _T('\0');
    if (name == NULL || *name == _T('\0'))
        return buf;
    nm = copystring(name); // Make a scratch copy
    wxChar *nm_tmp = nm;

    /* Skip leading whitespace and cr */
    while (wxStrchr((wxChar *)trimchars, *nm) != NULL)
        nm++;
    /* And strip off trailing whitespace and cr */
    s = nm + (q = wxStrlen(nm)) - 1;
    while (q-- && wxStrchr((wxChar *)trimchars, *s) != NULL)
        *s = _T('\0');

    s = nm;
    d = lnm;
#ifdef __WXMSW__
    q = FALSE;
#else
    q = nm[0] == _T('\\') && nm[1] == _T('~');
#endif

    /* Expand inline environment variables */
#ifdef __VISAGECPP__
    while (*d)
    {
      *d++ = *s;
      if(*s == _T('\\'))
      {
        *(d - 1) = *++s;
        if (*d)
        {
          s++;
          continue;
        }
        else
           break;
      }
      else
#else
    while ((*d++ = *s)) {
#  ifndef __WXMSW__
        if (*s == _T('\\')) {
            if ((*(d - 1) = *++s)) {
                s++;
                continue;
            } else
                break;
        } else
#  endif
#endif
#ifdef __WXMSW__
        if (*s++ == _T('$') && (*s == _T('{') || *s == _T(')')))
#else
        if (*s++ == _T('$'))
#endif
        {
            register wxChar  *start = d;
            register int     braces = (*s == _T('{') || *s == _T('('));
            register wxChar  *value;
#ifdef __VISAGECPP__
    // VA gives assignment in logical expr warning
            while (*d)
               *d++ = *s;
#else
            while ((*d++ = *s))
#endif
                if (braces ? (*s == _T('}') || *s == _T(')')) : !(wxIsalnum(*s) || *s == _T('_')) )
                    break;
                else
                    s++;
            *--d = 0;
            value = wxGetenv(braces ? start + 1 : start);
            if (value) {
#ifdef __VISAGECPP__
    // VA gives assignment in logical expr warning
                for ((d = start - 1); (*d); *d++ = *value++);
#else
                for ((d = start - 1); (*d++ = *value++););
#endif
                d--;
                if (braces && *s)
                    s++;
            }
        }
    }

    /* Expand ~ and ~user */
    nm = lnm;
    s = _T("");
    if (nm[0] == _T('~') && !q)
    {
        /* prefix ~ */
        if (nm[1] == SEP || nm[1] == 0)
        {        /* ~/filename */
	    // FIXME: wxGetUserHome could return temporary storage in Unicode mode
            if ((s = WXSTRINGCAST wxGetUserHome(_T(""))) != NULL) {
                if (*++nm)
                    nm++;
            }
        } else
        {                /* ~user/filename */
            register wxChar  *nnm;
            register wxChar  *home;
            for (s = nm; *s && *s != SEP; s++);
            int was_sep; /* MATTHEW: Was there a separator, or NULL? */
            was_sep = (*s == SEP);
            nnm = *s ? s + 1 : s;
            *s = 0;
	    // FIXME: wxGetUserHome could return temporary storage in Unicode mode
            if ((home = WXSTRINGCAST wxGetUserHome(wxString(nm + 1))) == NULL) {
               if (was_sep) /* replace only if it was there: */
                   *s = SEP;
                s = _T("");
            } else {
                nm = nnm;
                s = home;
            }
        }
    }

    d = buf;
    if (s && *s) { /* MATTHEW: s could be NULL if user '~' didn't exist */
        /* Copy home dir */
        while (_T('\0') != (*d++ = *s++))
          /* loop */;
        // Handle root home
        if (d - 1 > buf && *(d - 2) != SEP)
          *(d - 1) = SEP;
    }
    s = nm;
#ifdef __VISAGECPP__
    // VA gives assignment in logical expr warning
    while (*d)
       *d++ = *s++;
#else
    while ((*d++ = *s++));
#endif
    delete[] nm_tmp; // clean up alloc
    /* Now clean up the buffer */
    return wxRealPath(buf);
}


/* Contract Paths to be build upon an environment variable
   component:

   example: "/usr/openwin/lib", OPENWINHOME --> ${OPENWINHOME}/lib

   The call wxExpandPath can convert these back!
 */
wxChar *
wxContractPath (const wxString& filename, const wxString& envname, const wxString& user)
{
  static wxChar dest[_MAXPATHLEN];

  if (filename == _T(""))
    return (wxChar *) NULL;

  wxStrcpy (dest, WXSTRINGCAST filename);
#ifdef __WXMSW__
  Unix2DosFilename(dest);
#endif

  // Handle environment
  const wxChar *val = (const wxChar *) NULL;
  wxChar *tcp = (wxChar *) NULL;
  if (envname != WXSTRINGCAST NULL && (val = wxGetenv (WXSTRINGCAST envname)) != NULL &&
     (tcp = wxStrstr (dest, val)) != NULL)
    {
        wxStrcpy (wxBuffer, tcp + wxStrlen (val));
        *tcp++ = _T('$');
        *tcp++ = _T('{');
        wxStrcpy (tcp, WXSTRINGCAST envname);
        wxStrcat (tcp, _T("}"));
        wxStrcat (tcp, wxBuffer);
    }

  // Handle User's home (ignore root homes!)
  size_t len = 0;
  if ((val = wxGetUserHome (user)) != NULL &&
      (len = wxStrlen(val)) > 2 &&
      wxStrncmp(dest, val, len) == 0)
    {
      wxStrcpy(wxBuffer, _T("~"));
      if (user != _T(""))
             wxStrcat(wxBuffer, (const wxChar*) user);
#ifdef __WXMSW__
//      strcat(wxBuffer, "\\");
#else
//      strcat(wxBuffer, "/");
#endif
      wxStrcat(wxBuffer, dest + len);
      wxStrcpy (dest, wxBuffer);
    }

  return dest;
}

// Return just the filename, not the path
// (basename)
wxChar *wxFileNameFromPath (wxChar *path)
{
  if (path)
    {
      register wxChar *tcp;

      tcp = path + wxStrlen (path);
      while (--tcp >= path)
        {
          if (*tcp == _T('/') || *tcp == _T('\\')
#ifdef __VMS__
     || *tcp == _T(':') || *tcp == _T(']'))
#else
     )
#endif
            return tcp + 1;
        }                        /* while */
#if defined(__WXMSW__) || defined(__WXPM__)
      if (wxIsalpha (*path) && *(path + 1) == _T(':'))
        return path + 2;
#endif
    }
  return path;
}

wxString wxFileNameFromPath (const wxString& path1)
{
  if (path1 != _T(""))
  {

      wxChar *path = WXSTRINGCAST path1 ;
      register wxChar *tcp;

      tcp = path + wxStrlen (path);
      while (--tcp >= path)
          {
            if (*tcp == _T('/') || *tcp == _T('\\')
#ifdef __VMS__
        || *tcp == _T(':') || *tcp == _T(']'))
#else
        )
#endif
                return wxString(tcp + 1);
            }                        /* while */
#if defined(__WXMSW__) || defined(__WXPM__)
      if (wxIsalpha (*path) && *(path + 1) == _T(':'))
            return wxString(path + 2);
#endif
    }
  // Yes, this should return the path, not an empty string, otherwise
  // we get "thing.txt" -> "".
  return path1;
}

// Return just the directory, or NULL if no directory
wxChar *
wxPathOnly (wxChar *path)
{
  if (path && *path)
    {
      static wxChar buf[_MAXPATHLEN];

      // Local copy
      wxStrcpy (buf, path);

      int l = wxStrlen(path);
      bool done = FALSE;

      int i = l - 1;

      // Search backward for a backward or forward slash
      while (!done && i > -1)
      {
        // ] is for VMS
        if (path[i] == _T('/') || path[i] == _T('\\') || path[i] == _T(']'))
        {
          done = TRUE;
#ifdef __VMS__
          buf[i+1] = 0;
#else
          buf[i] = 0;
#endif

          return buf;
        }
        else i --;
      }

#if defined(__WXMSW__) || defined(__WXPM__)
      // Try Drive specifier
      if (wxIsalpha (buf[0]) && buf[1] == _T(':'))
        {
          // A:junk --> A:. (since A:.\junk Not A:\junk)
          buf[2] = _T('.');
          buf[3] = _T('\0');
          return buf;
        }
#endif
    }

  return (wxChar *) NULL;
}

// Return just the directory, or NULL if no directory
wxString wxPathOnly (const wxString& path)
{
  if (path != _T(""))
    {
      wxChar buf[_MAXPATHLEN];

      // Local copy
      wxStrcpy (buf, WXSTRINGCAST path);

      int l = path.Length();
      bool done = FALSE;

      int i = l - 1;

      // Search backward for a backward or forward slash
      while (!done && i > -1)
      {
        // ] is for VMS
        if (path[i] == _T('/') || path[i] == _T('\\') || path[i] == _T(']'))
        {
          done = TRUE;
#ifdef __VMS__
          buf[i+1] = 0;
#else
          buf[i] = 0;
#endif

          return wxString(buf);
        }
        else i --;
      }

#if defined(__WXMSW__) || defined(__WXPM__)
      // Try Drive specifier
      if (wxIsalpha (buf[0]) && buf[1] == _T(':'))
        {
          // A:junk --> A:. (since A:.\junk Not A:\junk)
          buf[2] = _T('.');
          buf[3] = _T('\0');
          return wxString(buf);
        }
#endif
    }

  return wxString(_T(""));
}

// Utility for converting delimiters in DOS filenames to UNIX style
// and back again - or we get nasty problems with delimiters.
// Also, convert to lower case, since case is significant in UNIX.

#ifdef __WXMAC__
void
wxMac2UnixFilename (wxChar *s)
{
        if (s)
        {
                memmove( s+1 , s ,(strlen( s ) + 1)*sizeof(wxChar)) ;
                if ( *s == _T(':') )
                        *s = _T('.') ;
                else
                        *s = _T('/') ;

                while (*s)
                {
                        if (*s == _T(':'))
                          *s = _T('/');
                        else
                          *s = wxTolower(*s);        // Case INDEPENDENT
                        s++;
                }
        }
}

void
wxUnix2MacFilename (wxChar *s)
{
        if (s)
        {
                if ( *s == _T('.') )
                {
                        // relative path , since it goes on with slash which is translated to a :
                        memmove( s , s+1 ,strlen( s )*sizeof(wxChar) ) ;
                }
                else if ( *s == _T('/') )
                {
                        // absolute path -> on mac just start with the drive name
                        memmove( s , s+1 ,strlen( s )*sizeof(wxChar) ) ;
                }
                else
                {
                        wxASSERT_MSG( 1 , _T("unknown path beginning") ) ;
                }
                while (*s)
                {
                        if (*s == _T('/') || *s == _T('\\'))
                                  *s = _T(':');

                        s++ ;
                }
        }
}
#endif
void
wxDos2UnixFilename (wxChar *s)
{
  if (s)
    while (*s)
      {
        if (*s == _T('\\'))
          *s = _T('/');
#if defined(__WXMSW__) || defined(__WXPM__)
        else
          *s = wxTolower(*s);        // Case INDEPENDENT
#endif
        s++;
      }
}

void
#if defined(__WXMSW__) || defined(__WXPM__)
wxUnix2DosFilename (wxChar *s)
#else
wxUnix2DosFilename (wxChar *WXUNUSED(s))
#endif
{
// Yes, I really mean this to happen under DOS only! JACS
#if defined(__WXMSW__) || defined(__WXPM__)
  if (s)
    while (*s)
      {
        if (*s == _T('/'))
          *s = _T('\\');
        s++;
      }
#endif
}

// Concatenate two files to form third
bool
wxConcatFiles (const wxString& file1, const wxString& file2, const wxString& file3)
{
  wxChar *outfile = wxGetTempFileName("cat");

  FILE *fp1 = (FILE *) NULL;
  FILE *fp2 = (FILE *) NULL;
  FILE *fp3 = (FILE *) NULL;
  // Open the inputs and outputs
#ifdef __WXMAC__
        wxStrcpy( gwxMacFileName , file1 ) ;
        wxUnix2MacFilename( gwxMacFileName ) ;
        wxStrcpy( gwxMacFileName2 , file2) ;
        wxUnix2MacFilename( gwxMacFileName2 ) ;
        wxStrcpy( gwxMacFileName3 , outfile) ;
        wxUnix2MacFilename( gwxMacFileName3 ) ;

  if ((fp1 = fopen (gwxMacFileName, "rb")) == NULL ||
      (fp2 = fopen (gwxMacFileName2, "rb")) == NULL ||
      (fp3 = fopen (gwxMacFileName3, "wb")) == NULL)
#else
  if ((fp1 = fopen (FNSTRINGCAST file1.fn_str(), "rb")) == NULL ||
      (fp2 = fopen (FNSTRINGCAST file2.fn_str(), "rb")) == NULL ||
      (fp3 = fopen (wxFNCONV(outfile), "wb")) == NULL)
#endif
    {
      if (fp1)
        fclose (fp1);
      if (fp2)
        fclose (fp2);
      if (fp3)
        fclose (fp3);
      return FALSE;
    }

  int ch;
  while ((ch = getc (fp1)) != EOF)
    (void) putc (ch, fp3);
  fclose (fp1);

  while ((ch = getc (fp2)) != EOF)
    (void) putc (ch, fp3);
  fclose (fp2);

  fclose (fp3);
  bool result = wxRenameFile(outfile, file3);
  delete[] outfile;
  return result;
}

// Copy files
bool
wxCopyFile (const wxString& file1, const wxString& file2)
{
  FILE *fd1;
  FILE *fd2;
  int ch;

#ifdef __WXMAC__
        wxStrcpy( gwxMacFileName , file1 ) ;
        wxUnix2MacFilename( gwxMacFileName ) ;
        wxStrcpy( gwxMacFileName2 , file2) ;
        wxUnix2MacFilename( gwxMacFileName2 ) ;

  if ((fd1 = fopen (gwxMacFileName, "rb")) == NULL)
    return FALSE;
  if ((fd2 = fopen (gwxMacFileName2, "wb")) == NULL)
#else
  if ((fd1 = fopen (FNSTRINGCAST file1.fn_str(), "rb")) == NULL)
    return FALSE;
  if ((fd2 = fopen (FNSTRINGCAST file2.fn_str(), "wb")) == NULL)
#endif
    {
      fclose (fd1);
      return FALSE;
    }

  while ((ch = getc (fd1)) != EOF)
    (void) putc (ch, fd2);

  fclose (fd1);
  fclose (fd2);
  return TRUE;
}

bool
wxRenameFile (const wxString& file1, const wxString& file2)
{
#ifdef __WXMAC__
        wxStrcpy( gwxMacFileName , file1 ) ;
        wxUnix2MacFilename( gwxMacFileName ) ;
        wxStrcpy( gwxMacFileName2 , file2) ;
        wxUnix2MacFilename( gwxMacFileName2 ) ;

  if (0 == rename (gwxMacFileName, gwxMacFileName2))
    return TRUE;
#else
  // Normal system call
  if (0 == rename (FNSTRINGCAST file1.fn_str(), FNSTRINGCAST file2.fn_str()))
    return TRUE;
#endif
  // Try to copy
  if (wxCopyFile(file1, file2)) {
    wxRemoveFile(file1);
    return TRUE;
  }
  // Give up
  return FALSE;
}

bool wxRemoveFile(const wxString& file)
{
#if defined(__VISUALC__) || defined(__BORLANDC__) || defined(__WATCOMC__)
  int flag = remove(FNSTRINGCAST file.fn_str());
#elif defined( __WXMAC__ )
        wxStrcpy( gwxMacFileName , file ) ;
        wxUnix2MacFilename( gwxMacFileName ) ;
  int flag = unlink(gwxMacFileName);
#else
  int flag = unlink(FNSTRINGCAST file.fn_str());
#endif
  return (flag == 0) ;
}

bool wxMkdir(const wxString& dir, int perm)
{
#if defined( __WXMAC__ )
    wxStrcpy( gwxMacFileName , dir ) ;
    wxUnix2MacFilename( gwxMacFileName ) ;
    const wxChar *dirname = gwxMacFileName;
#else // !Mac
    const wxChar *dirname = dir.c_str();
#endif // Mac/!Mac

    // assume mkdir() has 2 args on non Windows-OS/2 platforms and on Windows too
    // for the GNU compiler
#if (!(defined(__WXMSW__) || defined(__WXPM__))) || (defined(__GNUWIN32__) && !defined(__MINGW32__)) || defined(__WXWINE__)
    if ( mkdir(wxFNCONV(dirname), perm) != 0 )
#else  // MSW and OS/2
    if ( mkdir(FNSTRINGCAST wxFNCONV(dirname)) != 0 )
#endif // !MSW/MSW
    {
        wxLogSysError(_("Directory '%s' couldn't be created"), dirname);

        return FALSE;
    }

    return TRUE;
}

bool wxRmdir(const wxString& dir, int WXUNUSED(flags))
{
#ifdef __VMS__
  return FALSE;
#elif defined( __WXMAC__ )
        wxStrcpy( gwxMacFileName , dir ) ;
        wxUnix2MacFilename( gwxMacFileName ) ;
  return (rmdir(WXSTRINGCAST gwxMacFileName) == 0);
#else

#ifdef __SALFORDC__
  return FALSE; // What to do?
#else
  return (rmdir(FNSTRINGCAST dir.fn_str()) == 0);
#endif

#endif
}

#if 0
bool wxDirExists(const wxString& dir)
{
#ifdef __VMS__
  return FALSE;
#elif !defined(__WXMSW__)
  struct stat sbuf;
  return (stat(dir.fn_str(), &sbuf) != -1) && S_ISDIR(sbuf.st_mode) ? TRUE : FALSE;
#else

  /* MATTHEW: [6] Always use same code for Win32, call FindClose */
#if defined(__WIN32__)
  WIN32_FIND_DATA fileInfo;
#else
#ifdef __BORLANDC__
  struct ffblk fileInfo;
#else
  struct find_t fileInfo;
#endif
#endif

#if defined(__WIN32__)
        HANDLE h = FindFirstFile((LPTSTR) WXSTRINGCAST dir,(LPWIN32_FIND_DATA)&fileInfo);

        if (h==INVALID_HANDLE_VALUE)
         return FALSE;
        else {
         FindClose(h);
         return ((fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
        }
#else
  // In Borland findfirst has a different argument
  // ordering from _dos_findfirst. But _dos_findfirst
  // _should_ be ok in both MS and Borland... why not?
#ifdef __BORLANDC__
  return ((findfirst(WXSTRINGCAST dir, &fileInfo, _A_SUBDIR) == 0  && (fileInfo.ff_attrib & _A_SUBDIR) != 0));
#else
  return (((_dos_findfirst(WXSTRINGCAST dir, _A_SUBDIR, &fileInfo) == 0) && (fileInfo.attrib & _A_SUBDIR)) != 0);
#endif
#endif

#endif
}

#endif

// does the path exists? (may have or not '/' or '\\' at the end)
bool wxPathExists(const wxChar *pszPathName)
{
  /* Windows API returns -1 from stat for "c:\dir\" if "c:\dir" exists
   * OTOH, we should change "d:" to "d:\" and leave "\" as is. */
  wxString strPath(pszPathName);
  if ( wxEndsWithPathSeparator(pszPathName) && pszPathName[1] != _T('\0') )
    strPath.Last() = _T('\0');

#ifdef __SALFORDC__
  struct _stat st;
#else
  struct stat st;
#endif

  return stat(FNSTRINGCAST strPath.fn_str(), &st) == 0 && (st.st_mode & S_IFDIR);
}

// Get a temporary filename, opening and closing the file.
wxChar *wxGetTempFileName(const wxString& prefix, wxChar *buf)
{
#ifdef __WINDOWS__

#ifndef        __WIN32__
  wxChar tmp[144];
  ::GetTempFileName(0, WXSTRINGCAST prefix, 0, tmp);
#else
  wxChar tmp[MAX_PATH];
  wxChar tmpPath[MAX_PATH];
  ::GetTempPath(MAX_PATH, tmpPath);
  ::GetTempFileName(tmpPath, WXSTRINGCAST prefix, 0, tmp);
#endif
  if (buf) wxStrcpy(buf, tmp);
  else buf = copystring(tmp);
  return buf;

#else
  static short last_temp = 0;        // cache last to speed things a bit
  // At most 1000 temp files to a process! We use a ring count.
  wxChar tmp[100]; // FIXME static buffer

  for (short suffix = last_temp + 1; suffix != last_temp; ++suffix %= 1000)
    {
      wxSprintf (tmp, _T("/tmp/%s%d.%03x"), WXSTRINGCAST prefix, (int) getpid (), (int) suffix);
      if (!wxFileExists( tmp ))
        {
          // Touch the file to create it (reserve name)
          FILE *fd = fopen (wxFNCONV(tmp), "w");
          if (fd)
            fclose (fd);
          last_temp = suffix;
          if (buf)
            wxStrcpy( buf, tmp);
          else
            buf = copystring( tmp );
          return buf;
        }
    }
  wxLogError( _("wxWindows: error finding temporary file name.\n") );
  if (buf) buf[0] = 0;
  return (wxChar *) NULL;
#endif
}

// Get first file name matching given wild card.

#ifdef __UNIX__

// Get first file name matching given wild card.
// Flags are reserved for future use.

#ifndef __VMS__
    static DIR *gs_dirStream = (DIR *) NULL;
    static wxString gs_strFileSpec;
    static int gs_findFlags = 0;
#endif

wxString wxFindFirstFile(const wxChar *spec, int flags)
{
    wxString result;

#ifndef __VMS__
    if (gs_dirStream)
        closedir(gs_dirStream); // edz 941103: better housekeping

    gs_findFlags = flags;

    gs_strFileSpec = spec;

    // Find path only so we can concatenate
    // found file onto path
    wxString path(wxPathOnly(gs_strFileSpec));

    // special case: path is really "/"
    if ( !path && gs_strFileSpec[0u] == _T('/') )
        path = _T('/');
    // path is empty => Local directory
    if ( !path )
        path = _T('.');

    gs_dirStream = opendir(path.fn_str());
    if ( !gs_dirStream )
    {
        wxLogSysError(_("Can not enumerate files in directory '%s'"),
                      path.c_str());
    }
    else
    {
        result = wxFindNextFile();
    }
#endif // !VMS

    return result;
}

wxString wxFindNextFile()
{
    wxString result;

#ifndef __VMS__
    wxCHECK_MSG( gs_dirStream, result, _T("must call wxFindFirstFile first") );

    // Find path only so we can concatenate
    // found file onto path
    wxString path(wxPathOnly(gs_strFileSpec));
    wxString name(wxFileNameFromPath(gs_strFileSpec));

    /* MATTHEW: special case: path is really "/" */
    if ( !path && gs_strFileSpec[0u] == _T('/'))
        path = _T('/');

    // Do the reading
    struct dirent *nextDir;
    for ( nextDir = readdir(gs_dirStream);
          nextDir != NULL;
          nextDir = readdir(gs_dirStream) )
    {
        if (wxMatchWild(name, nextDir->d_name))
        {
            result.Empty();
            if ( !path.IsEmpty() )
            {
                result = path;
                if ( path != _T('/') )
                    result += _T('/');
            }

            result += nextDir->d_name;

            // Only return "." and ".." when they match
            bool isdir;
            if ( (strcmp(nextDir->d_name, ".") == 0) ||
                 (strcmp(nextDir->d_name, "..") == 0))
            {
                if ( (gs_findFlags & wxDIR) != 0 )
                    isdir = TRUE;
                else
                    continue;
            }
            else
                isdir = wxDirExists(result);

            // and only return directories when flags & wxDIR
            if ( !gs_findFlags ||
                 ((gs_findFlags & wxDIR) && isdir) ||
                 ((gs_findFlags & wxFILE) && !isdir) )
            {
                return result;
            }
        }
    }

    result.Empty(); // not found

    closedir(gs_dirStream);
    gs_dirStream = (DIR *) NULL;
#endif // !VMS

    return result;
}

#elif defined(__WXMSW__)

#ifdef __WIN32__
    static HANDLE gs_hFileStruct = INVALID_HANDLE_VALUE;
    static WIN32_FIND_DATA gs_findDataStruct;
#else // Win16
    #ifdef __BORLANDC__
        static struct ffblk gs_findDataStruct;
    #else
        static struct _find_t gs_findDataStruct;
    #endif // Borland
#endif // Win32/16

static wxString gs_strFileSpec;
static int gs_findFlags = 0;

wxString wxFindFirstFile(const wxChar *spec, int flags)
{
    wxString result;

    gs_strFileSpec = spec;
    gs_findFlags = flags; /* MATTHEW: [5] Remember flags */

    // Find path only so we can concatenate found file onto path
    wxString path(wxPathOnly(gs_strFileSpec));
    if ( !path.IsEmpty() )
        result << path << _T('\\');

#ifdef __WIN32__
    if ( gs_hFileStruct != INVALID_HANDLE_VALUE )
        FindClose(gs_hFileStruct);

    gs_hFileStruct = ::FindFirstFile(WXSTRINGCAST spec, &gs_findDataStruct);

    if ( gs_hFileStruct == INVALID_HANDLE_VALUE )
    {
        result.Empty();

        return result;
    }

    bool isdir = !!(gs_findDataStruct.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

    if (isdir && !(flags & wxDIR))
        return wxFindNextFile();
    else if (!isdir && flags && !(flags & wxFILE))
        return wxFindNextFile();

    result += gs_findDataStruct.cFileName;

    return result;
#else
    int flag = _A_NORMAL;
    if (flags & wxDIR) /* MATTHEW: [5] Use & */
        flag = _A_SUBDIR;

#ifdef __BORLANDC__
    if (findfirst(WXSTRINGCAST spec, &gs_findDataStruct, flag) == 0)
#else
        if (_dos_findfirst(WXSTRINGCAST spec, flag, &gs_findDataStruct) == 0)
#endif
        {
            /* MATTHEW: [5] Check directory flag */
            char attrib;

#ifdef __BORLANDC__
            attrib = gs_findDataStruct.ff_attrib;
#else
            attrib = gs_findDataStruct.attrib;
#endif

            if (attrib & _A_SUBDIR) {
                if (!(gs_findFlags & wxDIR))
                    return wxFindNextFile();
            } else if (gs_findFlags && !(gs_findFlags & wxFILE))
                return wxFindNextFile();

            result +=
#ifdef __BORLANDC__
                    gs_findDataStruct.ff_name
#else
                    gs_findDataStruct.name
#endif
                    ;
        }
#endif // __WIN32__

    return result;
}

wxString wxFindNextFile()
{
    wxString result;

    // Find path only so we can concatenate found file onto path
    wxString path(wxPathOnly(gs_strFileSpec));

try_again:

#ifdef __WIN32__
    if (gs_hFileStruct == INVALID_HANDLE_VALUE)
        return result;

    bool success = (FindNextFile(gs_hFileStruct, &gs_findDataStruct) != 0);
    if (!success)
    {
        FindClose(gs_hFileStruct);
        gs_hFileStruct = INVALID_HANDLE_VALUE;
    }
    else
    {
        bool isdir = !!(gs_findDataStruct.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

        if (isdir && !(gs_findFlags & wxDIR))
            goto try_again;
        else if (!isdir && gs_findFlags && !(gs_findFlags & wxFILE))
            goto try_again;

        if ( !path.IsEmpty() )
            result << path << _T('\\');
        result << gs_findDataStruct.cFileName;
    }

    return result;
#else // Win16

#ifdef __BORLANDC__
    if (findnext(&gs_findDataStruct) == 0)
#else
        if (_dos_findnext(&gs_findDataStruct) == 0)
#endif
        {
            /* MATTHEW: [5] Check directory flag */
            char attrib;

#ifdef __BORLANDC__
            attrib = gs_findDataStruct.ff_attrib;
#else
            attrib = gs_findDataStruct.attrib;
#endif

            if (attrib & _A_SUBDIR) {
                if (!(gs_findFlags & wxDIR))
                    goto try_again;
            } else if (gs_findFlags && !(gs_findFlags & wxFILE))
                goto try_again;


            result +=
#ifdef __BORLANDC__
                      gs_findDataStruct.ff_name
#else
                      gs_findDataStruct.name
#endif
                      ;
        }
#endif // Win32/16

    return result;
}

#endif // Unix/Windows

// Get current working directory.
// If buf is NULL, allocates space using new, else
// copies into buf.
wxChar *wxGetWorkingDirectory(wxChar *buf, int sz)
{
  if (!buf)
    buf = new wxChar[sz+1];
#if wxUSE_UNICODE
  char *cbuf = new char[sz+1];
#ifdef _MSC_VER
  if (_getcwd(cbuf, sz) == NULL) {
#else
  if (getcwd(cbuf, sz) == NULL) {
#endif
    delete [] cbuf;
#else
#ifdef _MSC_VER
  if (_getcwd(buf, sz) == NULL) {
#else
  if (getcwd(buf, sz) == NULL) {
#endif
#endif
    buf[0] = _T('.');
    buf[1] = _T('\0');
  }
#if wxUSE_UNICODE
  else {
    wxConvFile.MB2WC(buf, cbuf, sz);
    delete [] cbuf;
  }
#endif
  return buf;
}

wxString wxGetCwd()
{
    static const size_t maxPathLen = 1024;

    wxString str;
    wxGetWorkingDirectory(str.GetWriteBuf(maxPathLen), maxPathLen);
    str.UngetWriteBuf();

    return str;
}

bool wxSetWorkingDirectory(const wxString& d)
{
#if defined( __UNIX__ ) || defined( __WXMAC__ ) || defined(__WXPM__)
  return (chdir(FNSTRINGCAST d.fn_str()) == 0);
#elif defined(__WINDOWS__)

#ifdef __WIN32__
  return (bool)(SetCurrentDirectory(d) != 0);
#else
  // Must change drive, too.
  bool isDriveSpec = ((strlen(d) > 1) && (d[1] == ':'));
  if (isDriveSpec)
  {
    wxChar firstChar = d[0];

    // To upper case
    if (firstChar > 90)
      firstChar = firstChar - 32;

    // To a drive number
    unsigned int driveNo = firstChar - 64;
    if (driveNo > 0)
    {
       unsigned int noDrives;
       _dos_setdrive(driveNo, &noDrives);
    }
  }
  bool success = (chdir(WXSTRINGCAST d) == 0);

  return success;
#endif

#endif
}

// Get the OS directory if appropriate (such as the Windows directory).
// On non-Windows platform, probably just return the empty string.
wxString wxGetOSDirectory()
{
#ifdef __WINDOWS__
    wxChar buf[256];
    GetWindowsDirectory(buf, 256);
    return wxString(buf);
#else
    return wxEmptyString;
#endif
}

bool wxEndsWithPathSeparator(const wxChar *pszFileName)
{
  size_t len = wxStrlen(pszFileName);
  if ( len == 0 )
    return FALSE;
  else
    return wxIsPathSeparator(pszFileName[len - 1]);
}

// find a file in a list of directories, returns false if not found
bool wxFindFileInPath(wxString *pStr, const wxChar *pszPath, const wxChar *pszFile)
{
  // we assume that it's not empty
  wxCHECK_MSG( !wxIsEmpty(pszFile), FALSE,
               _("empty file name in wxFindFileInPath"));

  // skip path separator in the beginning of the file name if present
  if ( wxIsPathSeparator(*pszFile) )
    pszFile++;

  // copy the path (strtok will modify it)
  wxChar *szPath = new wxChar[wxStrlen(pszPath) + 1];
  wxStrcpy(szPath, pszPath);

  wxString strFile;
  wxChar *pc, *save_ptr;
  for ( pc = wxStrtok(szPath, wxPATH_SEP, &save_ptr);
        pc != NULL;
        pc = wxStrtok((wxChar *) NULL, wxPATH_SEP, &save_ptr) )
  {
    // search for the file in this directory
    strFile = pc;
    if ( !wxEndsWithPathSeparator(pc) )
      strFile += wxFILE_SEP_PATH;
    strFile += pszFile;

    if ( FileExists(strFile) ) {
      *pStr = strFile;
      break;
    }
  }

  delete [] szPath;

  return pc != NULL;  // if true => we breaked from the loop
}

void WXDLLEXPORT wxSplitPath(const wxChar *pszFileName,
                             wxString *pstrPath,
                             wxString *pstrName,
                             wxString *pstrExt)
{
    // it can be empty, but it shouldn't be NULL
    wxCHECK_RET( pszFileName, _T("NULL file name in wxSplitPath") );

    const wxChar *pDot = wxStrrchr(pszFileName, wxFILE_SEP_EXT);

#ifdef __WXMSW__
    // under Windows we understand both separators
    const wxChar *pSepUnix = wxStrrchr(pszFileName, wxFILE_SEP_PATH_UNIX);
    const wxChar *pSepDos = wxStrrchr(pszFileName, wxFILE_SEP_PATH_DOS);
    const wxChar *pLastSeparator = pSepUnix > pSepDos ? pSepUnix : pSepDos;
#else // assume Unix
    const wxChar *pLastSeparator = wxStrrchr(pszFileName, wxFILE_SEP_PATH_UNIX);

    if ( pDot == pszFileName )
    {
        // under Unix files like .profile are treated in a special way
        pDot = NULL;
    }
#endif // MSW/Unix

    if ( pDot < pLastSeparator )
    {
        // the dot is part of the path, not the start of the extension
        pDot = NULL;
    }

    if ( pstrPath )
    {
        if ( pLastSeparator )
            *pstrPath = wxString(pszFileName, pLastSeparator - pszFileName);
        else
            pstrPath->Empty();
    }

    if ( pstrName )
    {
        const wxChar *start = pLastSeparator ? pLastSeparator + 1 : pszFileName;
        const wxChar *end = pDot ? pDot : pszFileName + wxStrlen(pszFileName);

        *pstrName = wxString(start, end - start);
    }

    if ( pstrExt )
    {
        if ( pDot )
            *pstrExt = wxString(pDot + 1);
        else
            pstrExt->Empty();
    }
}

//------------------------------------------------------------------------
// wild character routines
//------------------------------------------------------------------------

bool wxIsWild( const wxString& pattern )
{
  wxString tmp = pattern;
  wxChar *pat = WXSTRINGCAST(tmp);
    while (*pat) {
        switch (*pat++) {
        case _T('?'): case _T('*'): case _T('['): case _T('{'):
            return TRUE;
        case _T('\\'):
            if (!*pat++)
                return FALSE;
        }
    }
    return FALSE;
};

bool wxMatchWild( const wxString& pat, const wxString& text, bool dot_special )

#if defined(HAVE_FNMATCH_H)
{
// this probably won't work well for multibyte chars in Unicode mode?
   if(dot_special)
      return fnmatch(pat.fn_str(), text.fn_str(), FNM_PERIOD) == 0;
   else
      return fnmatch(pat.fn_str(), text.fn_str(), 0) == 0;
}
#else

// #pragma error Broken implementation of wxMatchWild() -- needs fixing!

   /*
    * WARNING: this code is broken!
    */
{
  wxString tmp1 = pat;
  wxChar *pattern = WXSTRINGCAST(tmp1);
  wxString tmp2 = text;
  wxChar *str = WXSTRINGCAST(tmp2);
    wxChar c;
    wxChar *cp;
    bool done = FALSE, ret_code, ok;
    // Below is for vi fans
    const wxChar OB = _T('{'), CB = _T('}');

    // dot_special means '.' only matches '.'
    if (dot_special && *str == _T('.') && *pattern != *str)
        return FALSE;

    while ((*pattern != _T('\0')) && (!done)
    && (((*str==_T('\0'))&&((*pattern==OB)||(*pattern==_T('*'))))||(*str!=_T('\0')))) {
        switch (*pattern) {
        case _T('\\'):
            pattern++;
            if (*pattern != _T('\0'))
                pattern++;
            break;
        case _T('*'):
            pattern++;
            ret_code = FALSE;
            while ((*str!=_T('\0'))
            && (!(ret_code=wxMatchWild(pattern, str++, FALSE))))
                /*loop*/;
            if (ret_code) {
                while (*str != _T('\0'))
                    str++;
                while (*pattern != _T('\0'))
                    pattern++;
            }
            break;
        case _T('['):
            pattern++;
          repeat:
            if ((*pattern == _T('\0')) || (*pattern == _T(']'))) {
                done = TRUE;
                break;
            }
            if (*pattern == _T('\\')) {
                pattern++;
                if (*pattern == _T('\0')) {
                    done = TRUE;
                    break;
                }
            }
            if (*(pattern + 1) == _T('-')) {
                c = *pattern;
                pattern += 2;
                if (*pattern == _T(']')) {
                    done = TRUE;
                    break;
                }
                if (*pattern == _T('\\')) {
                    pattern++;
                    if (*pattern == _T('\0')) {
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
            while ((*pattern != _T(']')) && (*pattern != _T('\0'))) {
                if ((*pattern == _T('\\')) && (*(pattern + 1) != _T('\0')))
                    pattern++;
                pattern++;
            }
            if (*pattern != _T('\0')) {
                pattern++, str++;
            }
            break;
        case _T('?'):
            pattern++;
            str++;
            break;
        case OB:
            pattern++;
            while ((*pattern != CB) && (*pattern != _T('\0'))) {
                cp = str;
                ok = TRUE;
                while (ok && (*cp != _T('\0')) && (*pattern != _T('\0'))
                &&  (*pattern != _T(',')) && (*pattern != CB)) {
                    if (*pattern == _T('\\'))
                        pattern++;
                    ok = (*pattern++ == *cp++);
                }
                if (*pattern == _T('\0')) {
                    ok = FALSE;
                    done = TRUE;
                    break;
                } else if (ok) {
                    str = cp;
                    while ((*pattern != CB) && (*pattern != _T('\0'))) {
                        if (*++pattern == _T('\\')) {
                            if (*++pattern == CB)
                                pattern++;
                        }
                    }
                } else {
                    while (*pattern!=CB && *pattern!=_T(',') && *pattern!=_T('\0')) {
                        if (*++pattern == _T('\\')) {
                            if (*++pattern == CB || *pattern == _T(','))
                                pattern++;
                        }
                    }
                }
                if (*pattern != _T('\0'))
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
    while (*pattern == _T('*'))
        pattern++;
    return ((*str == _T('\0')) && (*pattern == _T('\0')));
};

#endif

#ifdef __VISUALC__
    #pragma warning(default:4706)   // assignment within conditional expression
#endif // VC++

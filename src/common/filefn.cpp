/////////////////////////////////////////////////////////////////////////////
// Name:        filefn.cpp
// Purpose:     File- and directory-related functions
// Author:      Julian Smart
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:   	wxWindows license
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
#ifdef _MSC_VER
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

#ifdef __WINDOWS__
#if !defined( __GNUWIN32__ ) && !defined( __MWERKS__ )
#include <direct.h>
#include <dos.h>
#endif
#endif

#ifdef __GNUWIN32__
#include <sys/unistd.h>
#define stricmp strcasecmp
#endif

#ifdef __BORLANDC__ // Please someone tell me which version of Borland needs
                    // this (3.1 I believe) and how to test for it.
                    // If this works for Borland 4.0 as well, then no worries.
#include <dir.h>
#endif

#include "wx/setup.h"

// No, Cygwin doesn't appear to have fnmatch.h after all.
#if defined(HAVE_FNMATCH_H)
#include   "fnmatch.h"
#endif

#ifdef __WINDOWS__
#include "windows.h"
#endif

#define _MAXPATHLEN 500

extern char *wxBuffer;
#ifdef __WXMAC__
extern char gwxMacFileName[] ;
extern char gwxMacFileName2[] ;
extern char gwxMacFileName3[] ;
#endif

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxPathList, wxStringList)
#endif

void wxPathList::Add (const wxString& path)
{
  wxStringList::Add ((char *)(const char *)path);
}

// Add paths e.g. from the PATH environment variable
void wxPathList::AddEnvList (const wxString& envVariable)
{
  static const char PATH_TOKS[] =
#ifdef __WINDOWS__
	" ;"; // Don't seperate with colon in DOS (used for drive)
#else
	" :;";
#endif

  char *val = getenv (WXSTRINGCAST envVariable);
  if (val && *val)
    {
      char *s = copystring (val);
      char *token = strtok (s, PATH_TOKS);

      if (token)
	{
	  Add (copystring (token));
	  while (token)
	    {
	      if ((token = strtok ((char *) NULL, PATH_TOKS)) != NULL)
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
  wxString path1(path);
  char *path_only = wxPathOnly (WXSTRINGCAST path1);
  if (path_only)
  {
      if (!Member (wxString(path_only)))
	    Add (wxString(path_only));
  }
}

bool wxPathList::Member (const wxString& path)
{
  for (wxNode * node = First (); node != NULL; node = node->Next ())
  {
      wxString path2((char *) node->Data ());
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

  char buf[_MAXPATHLEN];
  strcpy(buf, wxBuffer);

  char *filename = IsAbsolutePath (buf) ? wxFileNameFromPath (buf) : (char *)buf;

  for (wxNode * node = First (); node; node = node->Next ())
    {
      char *path = (char *) node->Data ();
      strcpy (wxBuffer, path);
      char ch = wxBuffer[strlen(wxBuffer)-1];
      if (ch != '\\' && ch != '/')
        strcat (wxBuffer, "/");
      strcat (wxBuffer, filename);
#ifdef __WINDOWS__
      Unix2DosFilename (wxBuffer);
#endif
      if (wxFileExists (wxBuffer))
      {
	return wxString(wxBuffer);	// Found!
      }
    }				// for()

  return wxString("");			// Not found
}

wxString wxPathList::FindAbsoluteValidPath (const wxString& file)
{
  wxString f = FindValidPath(file);
  if (wxIsAbsolutePath(f))
    return f;
  else
  {
    char buf[500];
    wxGetWorkingDirectory(buf, 499);
    int len = (int)strlen(buf);
    char lastCh = 0;
    if (len > 0)
      lastCh = buf[len-1];
    if (lastCh != '/' && lastCh != '\\')
    {
#ifdef __WINDOWS__
      strcat(buf, "\\");
#else
      strcat(buf, "/");
#endif
    }
    strcat(buf, (const char *)f);
    strcpy(wxBuffer, buf);
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
	strcpy( gwxMacFileName , filename ) ;
	wxUnix2MacFilename( gwxMacFileName ) ;
  	if (gwxMacFileName && stat ((char *)(const char *)gwxMacFileName, &stbuf) == 0)
    	return TRUE;
    return FALSE ;
#else
  struct stat stbuf;

  if (filename && stat ((char *)(const char *)filename, &stbuf) == 0)
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
  if (filename != "")
    {
      if (filename[0] == '/'
#ifdef __VMS__
      || (filename[0] == '[' && filename[1] != '.')
#endif
#ifdef __WINDOWS__
      /* MSDOS */
      || filename[0] == '\\' || (isalpha (filename[0]) && filename[1] == ':')
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
 
void wxStripExtension(char *buffer)
{
  int len = strlen(buffer);
  int i = len-1;
  while (i > 0)
  {
    if (buffer[i] == '.')
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
    if (buffer.GetChar(i) == '.')
    {
      buffer = buffer.Left(i);
      break;
    }
    i --;
  }
}

// Destructive removal of /./ and /../ stuff
char *wxRealPath (char *path)
{
#ifdef __WXMSW__
  static const char SEP = '\\';
  Unix2DosFilename(path);
#else
  static const char SEP = '/';
#endif
  if (path[0] && path[1]) {
    /* MATTHEW: special case "/./x" */
    char *p;
    if (path[2] == SEP && path[1] == '.')
      p = &path[0];
    else
      p = &path[2];
    for (; *p; p++)
      {
	if (*p == SEP)
	  {
	    if (p[1] == '.' && p[2] == '.' && (p[3] == SEP || p[3] == '\0'))
	      {
                char *q;
		for (q = p - 1; q >= path && *q != SEP; q--);
		if (q[0] == SEP && (q[1] != '.' || q[2] != '.' || q[3] != SEP)
		    && (q - 1 <= path || q[-1] != SEP))
		  {
		    strcpy (q, p + 3);
		    if (path[0] == '\0')
		      {
			path[0] = SEP;
			path[1] = '\0';
		      }
#ifdef __WXMSW__
		    /* Check that path[2] is NULL! */
		    else if (path[1] == ':' && !path[2])
		      {
			path[2] = SEP;
			path[3] = '\0';
		      }
#endif
		    p = q - 1;
		  }
	      }
	    else if (p[1] == '.' && (p[2] == SEP || p[2] == '\0'))
	      strcpy (p, p + 2);
	  }
      }
  }
  return path;
}

// Must be destroyed
char *wxCopyAbsolutePath(const wxString& filename)
{
  if (filename == "")
    return (char *) NULL;

  if (! IsAbsolutePath(wxExpandPath(wxBuffer, filename))) {
    char    buf[_MAXPATHLEN];
    buf[0] = '\0';
    wxGetWorkingDirectory(buf, sizeof(buf)/sizeof(char));
    char ch = buf[strlen(buf) - 1];
#ifdef __WXMSW__
    if (ch != '\\' && ch != '/')
	strcat(buf, "\\");
#else
    if (ch != '/')
	strcat(buf, "/");
#endif
    strcat(buf, wxBuffer);
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
	$a	=>	foo
	$a$b	=>	foobar
	$a.c	=>	foo.c
	xxx$a	=>	xxxfoo
	${a}!	=>	foo!
	$(b)!	=>	bar!
	\$a	=>	\$a
   MSDOS:
	$a	==>	$a
	$(a)	==>	foo
	$(a)$b	==>	foo$b
	$(a)$(b)==>	foobar
	test.$$	==>	test.$$
 */

/* input name in name, pathname output to buf. */

char *wxExpandPath(char *buf, const char *name)
{
    register char  *d, *s, *nm;
    char            lnm[_MAXPATHLEN];
    int				q;

    // Some compilers don't like this line.
//    const char      trimchars[] = "\n \t";

    char      trimchars[4];
    trimchars[0] = '\n';
    trimchars[1] = ' ';
    trimchars[2] = '\t';
    trimchars[3] = 0;

#ifdef __WXMSW__
     const char     SEP = '\\';
#else
     const char     SEP = '/';
#endif
    buf[0] = '\0';
    if (name == NULL || *name == '\0')
	return buf;
    nm = copystring(name); // Make a scratch copy
    char *nm_tmp = nm;

    /* Skip leading whitespace and cr */
    while (strchr((char *)trimchars, *nm) != NULL)
	nm++;
    /* And strip off trailing whitespace and cr */
    s = nm + (q = strlen(nm)) - 1;
    while (q-- && strchr((char *)trimchars, *s) != NULL)
	*s = '\0';

    s = nm;
    d = lnm;
#ifdef __WXMSW__
    q = FALSE;
#else
    q = nm[0] == '\\' && nm[1] == '~';
#endif

    /* Expand inline environment variables */
    while ((*d++ = *s)) {
#ifndef __WXMSW__
	if (*s == '\\') {
	    if ((*(d - 1) = *++s)) {
		s++;
		continue;
	    } else
		break;
	} else
#endif
#ifdef __WXMSW__
	if (*s++ == '$' && (*s == '{' || *s == ')'))
#else
	if (*s++ == '$')
#endif
	{
	    register char  *start = d;
	    register int   braces = (*s == '{' || *s == '(');
	    register char  *value;
	    while ((*d++ = *s))
		if (braces ? (*s == '}' || *s == ')') : !(isalnum(*s) || *s == '_') )
		    break;
		else
		    s++;
	    *--d = 0;
	    value = getenv(braces ? start + 1 : start);
	    if (value) {
		for ((d = start - 1); (*d++ = *value++););
		d--;
		if (braces && *s)
		    s++;
	    }
	}
    }

    /* Expand ~ and ~user */
    nm = lnm;
    s = "";
    if (nm[0] == '~' && !q)
    {
	/* prefix ~ */
	if (nm[1] == SEP || nm[1] == 0)
        {	/* ~/filename */
	    if ((s = wxGetUserHome("")) != NULL) {
		if (*++nm)
		    nm++;
	    }
        } else
        {		/* ~user/filename */
	    register char  *nnm;
	    register char  *home;
	    for (s = nm; *s && *s != SEP; s++);
	    int was_sep; /* MATTHEW: Was there a separator, or NULL? */
            was_sep = (*s == SEP);
	    nnm = *s ? s + 1 : s;
	    *s = 0;
	    if ((home = wxGetUserHome(wxString(nm + 1))) == NULL) {
               if (was_sep) /* replace only if it was there: */
 		  *s = SEP;
		s = "";
	    } else {
		nm = nnm;
		s = home;
	    }
	}
    }

    d = buf;
    if (s && *s) { /* MATTHEW: s could be NULL if user '~' didn't exist */
	/* Copy home dir */
	while ('\0' != (*d++ = *s++))
	  /* loop */;
	// Handle root home
	if (d - 1 > buf && *(d - 2) != SEP)
	  *(d - 1) = SEP;
    }
    s = nm;
    while ((*d++ = *s++));

    delete[] nm_tmp; // clean up alloc
    /* Now clean up the buffer */
    return wxRealPath(buf);
}


/* Contract Paths to be build upon an environment variable
   component:

   example: "/usr/openwin/lib", OPENWINHOME --> ${OPENWINHOME}/lib

   The call wxExpandPath can convert these back!
 */
char *
wxContractPath (const wxString& filename, const wxString& envname, const wxString& user)
{
  static char dest[_MAXPATHLEN];

  if (filename == "")
    return (char *) NULL;

  strcpy (dest, WXSTRINGCAST filename);
#ifdef __WXMSW__
  Unix2DosFilename(dest);
#endif

  // Handle environment
  char *val = (char *) NULL;
  char *tcp = (char *) NULL;
  if (envname != WXSTRINGCAST NULL && (val = getenv (WXSTRINGCAST envname)) != NULL &&
     (tcp = strstr (dest, val)) != NULL)
    {
        strcpy (wxBuffer, tcp + strlen (val));
        *tcp++ = '$';
        *tcp++ = '{';
        strcpy (tcp, WXSTRINGCAST envname);
        strcat (tcp, "}");
        strcat (tcp, wxBuffer);
    }

  // Handle User's home (ignore root homes!)
  size_t len = 0;
  if ((val = wxGetUserHome (user)) != NULL &&
      (len = strlen(val)) > 2 &&
      strncmp(dest, val, len) == 0)
    {
      strcpy(wxBuffer, "~");
      if (user && *user)
	strcat(wxBuffer, user);
#ifdef __WXMSW__
//      strcat(wxBuffer, "\\");
#else
//      strcat(wxBuffer, "/");
#endif
      strcat(wxBuffer, dest + len);
      strcpy (dest, wxBuffer);
    }

  return dest;
}

// Return just the filename, not the path
// (basename)
char *wxFileNameFromPath (char *path)
{
  if (path)
    {
      register char *tcp;

      tcp = path + strlen (path);
      while (--tcp >= path)
	{
	  if (*tcp == '/' || *tcp == '\\'
#ifdef __VMS__
     || *tcp == ':' || *tcp == ']')
#else
     )
#endif
	    return tcp + 1;
	}			/* while */
#ifdef __WXMSW__
      if (isalpha (*path) && *(path + 1) == ':')
	return path + 2;
#endif
    }
  return path;
}

wxString wxFileNameFromPath (const wxString& path1)
{
  if (path1 != "")
  {

      char *path = WXSTRINGCAST path1 ;
      register char *tcp;

      tcp = path + strlen (path);
      while (--tcp >= path)
	  {
	    if (*tcp == '/' || *tcp == '\\'
#ifdef __VMS__
        || *tcp == ':' || *tcp == ']')
#else
        )
#endif
	        return wxString(tcp + 1);
	    }			/* while */
#ifdef __WXMSW__
      if (isalpha (*path) && *(path + 1) == ':')
	    return wxString(path + 2);
#endif
    }
  return wxString("");
}

// Return just the directory, or NULL if no directory
char *
wxPathOnly (char *path)
{
  if (path && *path)
    {
      static char buf[_MAXPATHLEN];

      // Local copy
      strcpy (buf, path);

      int l = strlen(path);
      bool done = FALSE;

      int i = l - 1;

      // Search backward for a backward or forward slash
      while (!done && i > -1)
      {
        // ] is for VMS
        if (path[i] == '/' || path[i] == '\\' || path[i] == ']')
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

#ifdef __WXMSW__
      // Try Drive specifier
      if (isalpha (buf[0]) && buf[1] == ':')
	{
	  // A:junk --> A:. (since A:.\junk Not A:\junk)
	  buf[2] = '.';
	  buf[3] = '\0';
	  return buf;
	}
#endif
    }

  return (char *) NULL;
}

// Return just the directory, or NULL if no directory
wxString wxPathOnly (const wxString& path)
{
  if (path != "")
    {
      char buf[_MAXPATHLEN];

      // Local copy
      strcpy (buf, WXSTRINGCAST path);

      int l = path.Length();
      bool done = FALSE;

      int i = l - 1;

      // Search backward for a backward or forward slash
      while (!done && i > -1)
      {
        // ] is for VMS
        if (path[i] == '/' || path[i] == '\\' || path[i] == ']')
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

#ifdef __WXMSW__
      // Try Drive specifier
      if (isalpha (buf[0]) && buf[1] == ':')
	{
	  // A:junk --> A:. (since A:.\junk Not A:\junk)
	  buf[2] = '.';
	  buf[3] = '\0';
	  return wxString(buf);
	}
#endif
    }

  return wxString("");
}

// Utility for converting delimiters in DOS filenames to UNIX style
// and back again - or we get nasty problems with delimiters.
// Also, convert to lower case, since case is significant in UNIX.

#ifdef __WXMAC__
void 
wxMac2UnixFilename (char *s)
{
	if (s)
	{
		memmove( s+1 , s ,strlen( s ) + 1) ;
		if ( *s == ':' )
			*s = '.' ;
		else
			*s = '/' ;
			
		while (*s)
		{
			if (*s == ':')
			  *s = '/';
			else
			  *s = wxToLower (*s);	// Case INDEPENDENT
			s++;
		}
	}
}

void 
wxUnix2MacFilename (char *s)
{
	if (s)
	{
		if ( *s == '.' )
		{
			// relative path , since it goes on with slash which is translated to a : 
			memmove( s , s+1 ,strlen( s ) ) ;
		}
		else if ( *s == '/' )
		{
			// absolute path -> on mac just start with the drive name
			memmove( s , s+1 ,strlen( s ) ) ;
		}
		else
		{
			wxASSERT_MSG( 1 , "unkown path beginning" ) ;
		}
		while (*s)
		{
			if (*s == '/' || *s == '\\')
		  		*s = ':';

			s++ ;
		}
	}
}
#endif
void 
wxDos2UnixFilename (char *s)
{
  if (s)
    while (*s)
      {
	if (*s == '\\')
	  *s = '/';
#ifdef __WXMSW__
	else
	  *s = wxToLower (*s);	// Case INDEPENDENT
#endif
	s++;
      }
}

void 
#ifdef __WXMSW__
wxUnix2DosFilename (char *s)
#else
wxUnix2DosFilename (char *WXUNUSED(s))
#endif
{
// Yes, I really mean this to happen under DOS only! JACS
#ifdef __WXMSW__
  if (s)
    while (*s)
      {
	if (*s == '/')
	  *s = '\\';
	s++;
      }
#endif
}

// Concatenate two files to form third
bool 
wxConcatFiles (const wxString& file1, const wxString& file2, const wxString& file3)
{
  char *outfile = wxGetTempFileName("cat");

  FILE *fp1 = (FILE *) NULL;
  FILE *fp2 = (FILE *) NULL;
  FILE *fp3 = (FILE *) NULL;
  // Open the inputs and outputs
#ifdef __WXMAC__
	strcpy( gwxMacFileName , file1 ) ;
	wxUnix2MacFilename( gwxMacFileName ) ;
	strcpy( gwxMacFileName2 , file2) ;
	wxUnix2MacFilename( gwxMacFileName2 ) ;
	strcpy( gwxMacFileName3 , outfile) ;
	wxUnix2MacFilename( gwxMacFileName3 ) ;

  if ((fp1 = fopen (gwxMacFileName, "rb")) == NULL ||
      (fp2 = fopen (gwxMacFileName2, "rb")) == NULL ||
      (fp3 = fopen (gwxMacFileName3, "wb")) == NULL)
#else
  if ((fp1 = fopen (WXSTRINGCAST file1, "rb")) == NULL ||
      (fp2 = fopen (WXSTRINGCAST file2, "rb")) == NULL ||
      (fp3 = fopen (outfile, "wb")) == NULL)
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
	strcpy( gwxMacFileName , file1 ) ;
	wxUnix2MacFilename( gwxMacFileName ) ;
	strcpy( gwxMacFileName2 , file2) ;
	wxUnix2MacFilename( gwxMacFileName2 ) ;

  if ((fd1 = fopen (gwxMacFileName, "rb")) == NULL)
    return FALSE;
  if ((fd2 = fopen (gwxMacFileName2, "wb")) == NULL)
#else
  if ((fd1 = fopen (WXSTRINGCAST file1, "rb")) == NULL)
    return FALSE;
  if ((fd2 = fopen (WXSTRINGCAST file2, "wb")) == NULL)
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
	strcpy( gwxMacFileName , file1 ) ;
	wxUnix2MacFilename( gwxMacFileName ) ;
	strcpy( gwxMacFileName2 , file2) ;
	wxUnix2MacFilename( gwxMacFileName2 ) ;

  if (0 == rename (gwxMacFileName, gwxMacFileName2))
    return TRUE;
#else
  // Normal system call
  if (0 == rename (WXSTRINGCAST file1, WXSTRINGCAST file2))
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
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__)
  int flag = remove(WXSTRINGCAST file);
#elif defined( __WXMAC__ )
	strcpy( gwxMacFileName , file ) ;
	wxUnix2MacFilename( gwxMacFileName ) ;
  int flag = unlink(gwxMacFileName);
#else
  int flag = unlink(WXSTRINGCAST file);
#endif
  return (flag == 0) ;
}

bool wxMkdir(const wxString& dir)
{
#if defined(__WXSTUBS__)
  return FALSE;
#elif defined(__VMS__)
	return FALSE;
#elif defined( __WXMAC__ )
  strcpy( gwxMacFileName , dir ) ;
  wxUnix2MacFilename( gwxMacFileName ) ;
  return (mkdir(gwxMacFileName , 0 ) == 0);
#elif (defined(__GNUWIN32__) && !defined(__MINGW32__)) || !defined(__WXMSW__)
  return (mkdir (WXSTRINGCAST dir, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == 0);
#else
  return (mkdir(WXSTRINGCAST dir) == 0);
#endif
}

bool wxRmdir(const wxString& dir, int WXUNUSED(flags))
{
#ifdef __VMS__
  return FALSE;
#elif defined( __WXMAC__ )
	strcpy( gwxMacFileName , dir ) ;
	wxUnix2MacFilename( gwxMacFileName ) ;
  return (rmdir(WXSTRINGCAST gwxMacFileName) == 0);
#else
  return (rmdir(WXSTRINGCAST dir) == 0);
#endif
}

#if 0
bool wxDirExists(const wxString& dir)
{
#ifdef __VMS__
  return FALSE;
#elif !defined(__WXMSW__)
  struct stat sbuf;
  return (stat(dir, &sbuf) != -1) && S_ISDIR(sbuf.st_mode) ? TRUE : FALSE;
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
bool wxPathExists(const char *pszPathName)
{
  // Windows API returns -1 from stat for "c:\dir\" if "c:\dir" exists
  // OTOH, we should change "d:" to "d:\" and leave "\" as is.
  wxString strPath(pszPathName);
  if ( wxEndsWithPathSeparator(pszPathName) && pszPathName[1] != '\0' )
    strPath.Last() = '\0';

  struct stat st;
  return stat(strPath, &st) == 0 && (st.st_mode & S_IFDIR);
}

// Get a temporary filename, opening and closing the file.
char *wxGetTempFileName(const wxString& prefix, char *buf)
{
#ifdef __WINDOWS__

#ifndef	__WIN32__
  char tmp[144];
  ::GetTempFileName(0, WXSTRINGCAST prefix, 0, tmp);
#else
  char tmp[MAX_PATH];
  char tmpPath[MAX_PATH];
  ::GetTempPath(MAX_PATH, tmpPath);
  ::GetTempFileName(tmpPath, WXSTRINGCAST prefix, 0, tmp);
#endif
  if (buf) strcpy(buf, tmp);
  else buf = copystring(tmp);
  return buf;

#else
  static short last_temp = 0;	// cache last to speed things a bit
  // At most 1000 temp files to a process! We use a ring count.
  char tmp[100]; // FIXME static buffer

  for (short suffix = last_temp + 1; suffix != last_temp; ++suffix %= 1000)
    {
      sprintf (tmp, "/tmp/%s%d.%03x", WXSTRINGCAST prefix, (int) getpid (), (int) suffix);
      if (!wxFileExists( tmp ))
	{
	  // Touch the file to create it (reserve name)
	  FILE *fd = fopen (tmp, "w");
	  if (fd)
	    fclose (fd);
	  last_temp = suffix;
          if (buf)
	    strcpy( buf, tmp);
	  else
	    buf = copystring( tmp );
	  return buf;
	}
    }
  cerr << _("wxWindows: error finding temporary file name.\n");
  if (buf) buf[0] = 0;
  return (char *) NULL;
#endif
}

// Get first file name matching given wild card.

#ifdef __UNIX__

// Get first file name matching given wild card.
// Flags are reserved for future use.

#ifndef __VMS__
static DIR *wxDirStream = (DIR *) NULL;
static char *wxFileSpec = (char *) NULL;
static int wxFindFileFlags = 0;
#endif

char *wxFindFirstFile(const char *spec, int flags)
{
#ifndef __VMS__
  if (wxDirStream)
    closedir(wxDirStream); // edz 941103: better housekeping

  wxFindFileFlags = flags;

  if (wxFileSpec)
    delete[] wxFileSpec;
  wxFileSpec = copystring(spec);

  // Find path only so we can concatenate
  // found file onto path
  char *p = wxPathOnly(wxFileSpec);

  /* MATTHEW: special case: path is really "/" */
  if (p && !*p && *wxFileSpec == '/')
    p = "/";
  /* MATTHEW: p is NULL => Local directory */
  if (!p)
    p = ".";
    
  if ((wxDirStream=opendir(p))==NULL)
    return (char *) NULL;

 /* MATTHEW: [5] wxFindNextFile can do the rest of the work */
  return wxFindNextFile();
#endif
 // ifndef __VMS__
  return (char *) NULL;
}

char *wxFindNextFile(void)
{
#ifndef __VMS__
  static char buf[400]; // FIXME static buffer

  /* MATTHEW: [2] Don't crash if we read too many times */
  if (!wxDirStream)
    return (char *) NULL;

  // Find path only so we can concatenate
  // found file onto path
  char *p = wxPathOnly(wxFileSpec);
  char *n = wxFileNameFromPath(wxFileSpec);

  /* MATTHEW: special case: path is really "/" */
  if (p && !*p && *wxFileSpec == '/')
    p = "/";

  // Do the reading
  struct dirent *nextDir;
  for (nextDir = readdir(wxDirStream); nextDir != NULL; nextDir = readdir(wxDirStream))
  {

    /* MATTHEW: [5] Only return "." and ".." when they match, and only return
       directories when flags & wxDIR */
    if (wxMatchWild(n, nextDir->d_name)) {
      bool isdir;

      buf[0] = 0;
      if (p && *p) {
        strcpy(buf, p);
        if (strcmp(p, "/") != 0)
          strcat(buf, "/");
      }
      strcat(buf, nextDir->d_name);

      if ((strcmp(nextDir->d_name, ".") == 0) ||
	  (strcmp(nextDir->d_name, "..") == 0)) {
	if (wxFindFileFlags && !(wxFindFileFlags & wxDIR))
	  continue;
	isdir = TRUE;
      } else
	isdir = wxDirExists(buf);
      
      if (!wxFindFileFlags
	  || ((wxFindFileFlags & wxDIR) && isdir)
	  || ((wxFindFileFlags & wxFILE) && !isdir))
	return buf;
    }
  }
  closedir(wxDirStream);
  wxDirStream = (DIR *) NULL;
#endif
 // ifndef __VMS__

  return (char *) NULL;
}

#elif defined(__WXMSW__)

#ifdef __WIN32__
HANDLE wxFileStrucHandle = INVALID_HANDLE_VALUE;
WIN32_FIND_DATA wxFileStruc;
#else
#ifdef __BORLANDC__
static struct ffblk wxFileStruc;
#else
static struct _find_t wxFileStruc;
#endif
#endif
static wxString wxFileSpec = "";
static int wxFindFileFlags;

char *wxFindFirstFile(const char *spec, int flags)
{
  wxFileSpec = spec;
  wxFindFileFlags = flags; /* MATTHEW: [5] Remember flags */

  // Find path only so we can concatenate
  // found file onto path
  wxString path1(wxFileSpec);
  char *p = wxPathOnly(WXSTRINGCAST path1);
  if (p && (strlen(p) > 0))
	 strcpy(wxBuffer, p);
  else
	 wxBuffer[0] = 0;

#ifdef __WIN32__
  if (wxFileStrucHandle != INVALID_HANDLE_VALUE)
	 FindClose(wxFileStrucHandle);

  wxFileStrucHandle = ::FindFirstFile(WXSTRINGCAST spec, &wxFileStruc);

  if (wxFileStrucHandle == INVALID_HANDLE_VALUE)
	 return NULL;

  bool isdir = !!(wxFileStruc.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

  if (isdir && !(flags & wxDIR))
	 return wxFindNextFile();
  else if (!isdir && flags && !(flags & wxFILE))
	 return wxFindNextFile();

  if (wxBuffer[0] != 0)
	 strcat(wxBuffer, "\\");
  strcat(wxBuffer, wxFileStruc.cFileName);
  return wxBuffer;
#else

  int flag = _A_NORMAL;
  if (flags & wxDIR) /* MATTHEW: [5] Use & */
    flag = _A_SUBDIR;

#ifdef __BORLANDC__
  if (findfirst(WXSTRINGCAST spec, &wxFileStruc, flag) == 0)
#else
  if (_dos_findfirst(WXSTRINGCAST spec, flag, &wxFileStruc) == 0)
#endif
  {
    /* MATTHEW: [5] Check directory flag */
    char attrib;

#ifdef __BORLANDC__
    attrib = wxFileStruc.ff_attrib;
#else
    attrib = wxFileStruc.attrib;
#endif

    if (attrib & _A_SUBDIR) {
      if (!(wxFindFileFlags & wxDIR))
	return wxFindNextFile();
    } else if (wxFindFileFlags && !(wxFindFileFlags & wxFILE))
		return wxFindNextFile();

	 if (wxBuffer[0] != 0)
		strcat(wxBuffer, "\\");

#ifdef __BORLANDC__
	 strcat(wxBuffer, wxFileStruc.ff_name);
#else
	 strcat(wxBuffer, wxFileStruc.name);
#endif
	 return wxBuffer;
  }
  else
    return NULL;
#endif // __WIN32__
}

char *wxFindNextFile(void)
{
  // Find path only so we can concatenate
  // found file onto path
  wxString p2(wxFileSpec);
  char *p = wxPathOnly(WXSTRINGCAST p2);
  if (p && (strlen(p) > 0))
	 strcpy(wxBuffer, p);
  else
	 wxBuffer[0] = 0;
  
 try_again:

#ifdef __WIN32__
  if (wxFileStrucHandle == INVALID_HANDLE_VALUE)
	 return NULL;

  bool success = (FindNextFile(wxFileStrucHandle, &wxFileStruc) != 0);
  if (!success) {
		FindClose(wxFileStrucHandle);
      wxFileStrucHandle = INVALID_HANDLE_VALUE;
		return NULL;
  }

  bool isdir = !!(wxFileStruc.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

  if (isdir && !(wxFindFileFlags & wxDIR))
    goto try_again;
  else if (!isdir && wxFindFileFlags && !(wxFindFileFlags & wxFILE))
	 goto try_again;

  if (wxBuffer[0] != 0)
    strcat(wxBuffer, "\\");
  strcat(wxBuffer, wxFileStruc.cFileName);
  return wxBuffer;  
#else

#ifdef __BORLANDC__
  if (findnext(&wxFileStruc) == 0)
#else
  if (_dos_findnext(&wxFileStruc) == 0)
#endif
  {
    /* MATTHEW: [5] Check directory flag */
    char attrib;

#ifdef __BORLANDC__
    attrib = wxFileStruc.ff_attrib;
#else
    attrib = wxFileStruc.attrib;
#endif

    if (attrib & _A_SUBDIR) {
      if (!(wxFindFileFlags & wxDIR))
	goto try_again;
    } else if (wxFindFileFlags && !(wxFindFileFlags & wxFILE))
      goto try_again;


	 if (wxBuffer[0] != 0)
      strcat(wxBuffer, "\\");
#ifdef __BORLANDC__
	 strcat(wxBuffer, wxFileStruc.ff_name);
#else
	 strcat(wxBuffer, wxFileStruc.name);
#endif
	 return wxBuffer;
  }
  else
    return NULL;
#endif
}

#endif
 // __WXMSW__

// Get current working directory.
// If buf is NULL, allocates space using new, else
// copies into buf.
char *wxGetWorkingDirectory(char *buf, int sz)
{
  if (!buf)
    buf = new char[sz+1];
#ifdef _MSC_VER 
  if (_getcwd(buf, sz) == NULL) {
#else
  if (getcwd(buf, sz) == NULL) {
#endif
    buf[0] = '.';
    buf[1] = '\0';
  }
  return buf;
}

bool wxSetWorkingDirectory(const wxString& d)
{
#if defined( __UNIX__ ) || defined( __WXMAC__ )
  return (chdir(d) == 0);
#elif defined(__WINDOWS__)

#ifdef __WIN32__
  return (bool)(SetCurrentDirectory(d) != 0);
#else
  // Must change drive, too.
  bool isDriveSpec = ((strlen(d) > 1) && (d[1] == ':'));
  if (isDriveSpec)
  {
    char firstChar = d[0];

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

bool wxEndsWithPathSeparator(const char *pszFileName)
{
  size_t len = Strlen(pszFileName);
  if ( len == 0 )
    return FALSE;
  else
    return wxIsPathSeparator(pszFileName[len - 1]);
}

// find a file in a list of directories, returns false if not found
bool wxFindFileInPath(wxString *pStr, const char *pszPath, const char *pszFile)
{
  // we assume that it's not empty
  wxCHECK_MSG( !IsEmpty(pszFile), FALSE, 
               _("empty file name in wxFindFileInPath"));

  // skip path separator in the beginning of the file name if present
  if ( wxIsPathSeparator(*pszFile) )
    pszFile++;

  // copy the path (strtok will modify it)
  char *szPath = new char[strlen(pszPath) + 1];
  strcpy(szPath, pszPath);

  wxString strFile;
  char *pc;
  for ( pc = strtok(szPath, PATH_SEP); pc; pc = strtok((char *) NULL, PATH_SEP) ) {
    // search for the file in this directory
    strFile = pc;
    if ( !wxEndsWithPathSeparator(pc) )
      strFile += FILE_SEP_PATH;
    strFile += pszFile;

    if ( FileExists(strFile) ) {
      *pStr = strFile;
      break;
    }
  }

  delete [] szPath;

  return pc != NULL;  // if true => we breaked from the loop
}

void WXDLLEXPORT wxSplitPath(const char *pszFileName,
                             wxString *pstrPath,
                             wxString *pstrName,
                             wxString *pstrExt)
{
  wxCHECK_RET( pszFileName, _("NULL file name in wxSplitPath") );

  const char *pDot = strrchr(pszFileName, FILE_SEP_EXT);
  const char *pSepUnix = strrchr(pszFileName, FILE_SEP_PATH_UNIX);
  const char *pSepDos = strrchr(pszFileName, FILE_SEP_PATH_DOS);

  // take the last of the two
  size_t nPosUnix = pSepUnix ? pSepUnix - pszFileName : 0;
  size_t nPosDos = pSepDos ? pSepDos - pszFileName : 0;
  if ( nPosDos > nPosUnix )
    nPosUnix = nPosDos;
//  size_t nLen = Strlen(pszFileName);

  if ( pstrPath )
    *pstrPath = wxString(pszFileName, nPosUnix);
  if ( pDot ) {
    size_t nPosDot = pDot - pszFileName;
    if ( pstrName )
      *pstrName = wxString(pszFileName + nPosUnix + 1, nPosDot - nPosUnix);
    if ( pstrExt )
      *pstrExt = wxString(pszFileName + nPosDot + 1);
  }
  else {
    if ( pstrName )
      *pstrName = wxString(pszFileName + nPosUnix + 1);
    if ( pstrExt )
      pstrExt->Empty();
  }
}

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

#if defined(HAVE_FNMATCH_H)
{
   if(dot_special)
      return fnmatch(pat.c_str(), text.c_str(), FNM_PERIOD) == 0;
   else
      return fnmatch(pat.c_str(), text.c_str(), 0) == 0;
}
#else

// #pragma error Broken implementation of wxMatchWild() -- needs fixing!

   /*
    * WARNING: this code is broken!
    */
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

#endif

#ifdef _MSC_VER
    #pragma warning(default:4706)   // assignment within conditional expression
#endif // VC++

/////////////////////////////////////////////////////////////////////////////
// Name:        filefn.h
// Purpose:     File- and directory-related functions
// Author:      Julian Smart
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef   _FILEFN_H_
#define   _FILEFN_H_

#ifdef __GNUG__
#pragma interface "filefn.h"
#endif

#include <wx/list.h>

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// define off_t
#include  <sys/types.h>

#ifdef    _MSC_VER
  #define   off_t       _off_t
#endif

const off_t wxInvalidOffset = (off_t)-1;

typedef enum {
  wxFromStart,
  wxFromCurrent,
  wxFromEnd
} wxSeekMode;

// ----------------------------------------------------------------------------
// functions
// ----------------------------------------------------------------------------
WXDLLEXPORT bool wxFileExists(const wxString& filename);
#define FileExists wxFileExists

// does the path exist? (may have or not '/' or '\\' at the end)
WXDLLEXPORT bool wxPathExists(const char *pszPathName);

#define wxDirExists wxPathExists
#define DirExists wxDirExists

WXDLLEXPORT bool wxIsAbsolutePath(const wxString& filename);
#define IsAbsolutePath wxIsAbsolutePath

// Get filename
WXDLLEXPORT char* wxFileNameFromPath(char *path);
WXDLLEXPORT wxString wxFileNameFromPath(const wxString& path);
#define FileNameFromPath wxFileNameFromPath

// Get directory
WXDLLEXPORT char* wxPathOnly(char *path);
WXDLLEXPORT wxString wxPathOnly(const wxString& path);
#define PathOnly wxPathOnly

// wxString version
WXDLLEXPORT wxString wxRealPath(const wxString& path);

WXDLLEXPORT void wxDos2UnixFilename(char *s);
#define Dos2UnixFilename wxDos2UnixFilename

WXDLLEXPORT void wxUnix2DosFilename(char *s);
#define Unix2DosFilename wxUnix2DosFilename

// Strip the extension, in situ
WXDLLEXPORT void wxStripExtension(char *buffer);
WXDLLEXPORT void wxStripExtension(wxString& buffer);

// Get a temporary filename, opening and closing the file.
WXDLLEXPORT char* wxGetTempFileName(const wxString& prefix, char *buf = (char *) NULL);

// Expand file name (~/ and ${OPENWINHOME}/ stuff)
WXDLLEXPORT char* wxExpandPath(char *dest, const char *path);

// Contract w.r.t environment (</usr/openwin/lib, OPENWHOME> -> ${OPENWINHOME}/lib)
// and make (if under the home tree) relative to home
// [caller must copy-- volatile]
WXDLLEXPORT char* wxContractPath (const wxString& filename,
   const wxString& envname = "", const wxString& user = "");

// Destructive removal of /./ and /../ stuff
WXDLLEXPORT char* wxRealPath(char *path);

// Allocate a copy of the full absolute path
WXDLLEXPORT char* wxCopyAbsolutePath(const wxString& path);

// Get first file name matching given wild card.
// Flags are reserved for future use.
#define wxFILE  1
#define wxDIR   2
WXDLLEXPORT char* wxFindFirstFile(const char *spec, int flags = wxFILE);
WXDLLEXPORT char* wxFindNextFile(void);

// Does the pattern contain wildcards?
WXDLLEXPORT bool wxIsWild(const wxString& pattern);

// Does the pattern match the text (usually a filename)?
// If dot_special is TRUE, doesn't match * against . (eliminating
// `hidden' dot files)
WXDLLEXPORT bool wxMatchWild(const wxString& pattern,  const wxString& text, bool dot_special = TRUE);

// Concatenate two files to form third
WXDLLEXPORT bool wxConcatFiles(const wxString& file1, const wxString& file2, const wxString& file3);

// Copy file1 to file2
WXDLLEXPORT bool wxCopyFile(const wxString& file1, const wxString& file2);

// Remove file
WXDLLEXPORT bool wxRemoveFile(const wxString& file);

// Rename file
WXDLLEXPORT bool wxRenameFile(const wxString& file1, const wxString& file2);

// Get current working directory.
// If buf is NULL, allocates space using new, else
// copies into buf.
// IMPORTANT NOTE getcwd is know not to work under some releases
// of Win32s 1.3, according to MS release notes!
WXDLLEXPORT char* wxGetWorkingDirectory(char *buf = (char *) NULL, int sz = 1000);

// Set working directory
WXDLLEXPORT bool wxSetWorkingDirectory(const wxString& d);

// Make directory
WXDLLEXPORT bool wxMkdir(const wxString& dir);

// Remove directory. Flags reserved for future use.
WXDLLEXPORT bool wxRmdir(const wxString& dir, int flags = 0);

// separators in file names
#define FILE_SEP_EXT        '.'
#define FILE_SEP_DSK        ':'
#define FILE_SEP_PATH_DOS   '\\'
#define FILE_SEP_PATH_UNIX  '/'

// separator in the path list (as in PATH environment variable)
// NB: these are strings and not characters on purpose!
#define PATH_SEP_DOS        ";"
#define PATH_SEP_UNIX       ":"

// platform independent versions
#ifdef  __UNIX__
  #define FILE_SEP_PATH     FILE_SEP_PATH_UNIX
  #define PATH_SEP          PATH_SEP_UNIX
#else   // Windows
  #define FILE_SEP_PATH     FILE_SEP_PATH_DOS
  #define PATH_SEP          PATH_SEP_DOS
#endif  // Unix/Windows

// this is useful for wxString::IsSameAs(): to compare two file names use
// filename1.IsSameAs(filename2, wxARE_FILENAMES_CASE_SENSITIVE)
#ifdef  __UNIX__
  #define wxARE_FILENAMES_CASE_SENSITIVE  TRUE
#else   // Windows
  #define wxARE_FILENAMES_CASE_SENSITIVE  FALSE
#endif  // Unix/Windows

// is the char a path separator?
inline bool wxIsPathSeparator(char c)
  { return c == FILE_SEP_PATH_DOS || c == FILE_SEP_PATH_UNIX; }

// does the string ends with path separator?
WXDLLEXPORT bool wxEndsWithPathSeparator(const char *pszFileName);

// split the full path into path (including drive for DOS), name and extension
// (understands both '/' and '\\')
WXDLLEXPORT void wxSplitPath(const char *pszFileName,
                             wxString *pstrPath,
                             wxString *pstrName,
                             wxString *pstrExt);

// find a file in a list of directories, returns false if not found
WXDLLEXPORT bool wxFindFileInPath(wxString *pStr, const char *pszPath, const char *pszFile);

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------

// Path searching
class WXDLLEXPORT wxPathList: public wxStringList
{
  DECLARE_DYNAMIC_CLASS(wxPathList)

  public:
  void AddEnvList(const wxString& envVariable);    // Adds all paths in environment variable
  void Add(const wxString& path);
  wxString FindValidPath(const wxString& filename);   // Find the first full path
                                         // for which the file exists
  wxString FindAbsoluteValidPath(const wxString& filename);   // Find the first full path
                                         // for which the file exists; ensure it's an absolute
                                         // path that gets returned.
  void EnsureFileAccessible(const wxString& path); // Given full path and filename,
                                         // add path to list
  bool Member(const wxString& path);
};

#endif
  // _WX_FILEFN_H_


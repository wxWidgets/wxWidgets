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

#include "wx/list.h"

#include <time.h>

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// define off_t

#if !defined(__WXMAC__) || defined(__DARWIN__)
    #include  <sys/types.h>
#else
    typedef long off_t;
#endif

#if defined(__VISUALC__) || ( defined(__MWERKS__) && defined( __INTEL__) )
    typedef _off_t off_t;
#elif defined(__BORLANDC__) && defined(__WIN16__)
    typedef long off_t;
#elif defined(__SC__)
    typedef long off_t;
#elif defined(__MWERKS__) && !defined(__INTEL__)
    typedef long off_t;
#endif

#if defined(__VISAGECPP__) && __IBMCPP__ >= 400
//
// VisualAge C++ V4.0 cannot have any external linkage const decs
// in headers included by more than one primary source
//
extern const off_t wxInvalidOffset;
#else
const off_t wxInvalidOffset = (off_t)-1;
#endif

enum wxSeekMode
{
  wxFromStart,
  wxFromCurrent,
  wxFromEnd
};

WXDLLEXPORT_DATA(extern const wxChar*) wxEmptyString;

// ----------------------------------------------------------------------------
// declare our versions of low level file functions: some compilers prepend
// underscores to the usual names, some also have Unicode versions of them
// ----------------------------------------------------------------------------

// Microsoft compiler loves underscores, feed them to it
#ifdef  __VISUALC__
    // functions
    #define   wxClose      _close
    #define   wxRead       _read
    #define   wxWrite      _write
    #define   wxLseek      _lseek
    #define   wxFsync      _commit
    #define   wxEof        _eof

    #define   wxTell       _tell

    #if wxUSE_UNICODE
        #define   wxOpen       _wopen
        #define   wxAccess     _waccess

        #define   wxMkDir      _wmkdir
        #define   wxRmDir      _wrmdir

        #define   wxStat       _wstat
    #else // !wxUSE_UNICODE
        #define   wxOpen       _open
        #define   wxAccess     _access

        #define   wxMkDir      _mkdir
        #define   wxRmDir      _rmdir

        #define   wxStat       _stat
    #endif

    // types
    #define   wxStructStat struct _stat

    // constants (unless already defined by the user code)
    #ifndef O_RDONLY
        #define   O_RDONLY    _O_RDONLY
        #define   O_WRONLY    _O_WRONLY
        #define   O_RDWR      _O_RDWR
        #define   O_EXCL      _O_EXCL
        #define   O_CREAT     _O_CREAT
        #define   O_BINARY    _O_BINARY

        #define   S_IFMT      _S_IFMT
        #define   S_IFDIR     _S_IFDIR
        #define   S_IFREG     _S_IFREG
    #endif // O_RDONLY
#else
    // functions
    #define   wxOpen       open
    #define   wxClose      close
    #define   wxRead       read
    #define   wxWrite      write
    #define   wxLseek      lseek
    #define   wxFsync      commit
    #define   wxAccess     access
    #define   wxEof        eof

    #define   wxMkDir      mkdir
    #define   wxRmDir      rmdir

    #define   wxTell(fd)   lseek(fd, 0, SEEK_CUR)

    #define   wxStat       stat

    // types
    #define   wxStructStat struct stat

#endif  // VC++

// ----------------------------------------------------------------------------
// functions
// ----------------------------------------------------------------------------
WXDLLEXPORT bool wxFileExists(const wxString& filename);
#define FileExists wxFileExists

// does the path exist? (may have or not '/' or '\\' at the end)
WXDLLEXPORT bool wxPathExists(const wxChar *pszPathName);

#define wxDirExists wxPathExists
#define DirExists wxDirExists

WXDLLEXPORT bool wxIsAbsolutePath(const wxString& filename);
#define IsAbsolutePath wxIsAbsolutePath

// Get filename
WXDLLEXPORT wxChar* wxFileNameFromPath(wxChar *path);
WXDLLEXPORT wxString wxFileNameFromPath(const wxString& path);
#define FileNameFromPath wxFileNameFromPath

// Get directory
WXDLLEXPORT wxString wxPathOnly(const wxString& path);
#define PathOnly wxPathOnly

// wxString version
WXDLLEXPORT wxString wxRealPath(const wxString& path);

WXDLLEXPORT void wxDos2UnixFilename(wxChar *s);
#define Dos2UnixFilename wxDos2UnixFilename

WXDLLEXPORT void wxUnix2DosFilename(wxChar *s);
#define Unix2DosFilename wxUnix2DosFilename

#ifdef __WXMAC__
  WXDLLEXPORT wxString wxMacFSSpec2MacFilename( const FSSpec *spec ) ;
  WXDLLEXPORT wxString wxMacFSSpec2UnixFilename( const FSSpec *spec ) ;
  WXDLLEXPORT void wxUnixFilename2FSSpec( const char *path , FSSpec *spec ) ;
  WXDLLEXPORT void wxMacFilename2FSSpec( const char *path , FSSpec *spec ) ;
  WXDLLEXPORT wxString wxMac2UnixFilename( const char *s) ;
  WXDLLEXPORT wxString wxUnix2MacFilename( const char *s);
#endif

// Strip the extension, in situ
WXDLLEXPORT void wxStripExtension(wxChar *buffer);
WXDLLEXPORT void wxStripExtension(wxString& buffer);

// Get a temporary filename, opening and closing the file.
WXDLLEXPORT wxChar* wxGetTempFileName(const wxString& prefix, wxChar *buf = (wxChar *) NULL);
WXDLLEXPORT bool wxGetTempFileName(const wxString& prefix, wxString& buf);

// Expand file name (~/ and ${OPENWINHOME}/ stuff)
WXDLLEXPORT wxChar* wxExpandPath(wxChar *dest, const wxChar *path);
WXDLLEXPORT bool wxExpandPath(wxString& dest, const wxChar *path);

// Contract w.r.t environment (</usr/openwin/lib, OPENWHOME> -> ${OPENWINHOME}/lib)
// and make (if under the home tree) relative to home
// [caller must copy-- volatile]
WXDLLEXPORT wxChar* wxContractPath(const wxString& filename,
				   const wxString& envname = wxEmptyString,
				   const wxString& user = wxEmptyString);

// Destructive removal of /./ and /../ stuff
WXDLLEXPORT wxChar* wxRealPath(wxChar *path);

// Allocate a copy of the full absolute path
WXDLLEXPORT wxChar* wxCopyAbsolutePath(const wxString& path);

// Get first file name matching given wild card.
// Flags are reserved for future use.
#define wxFILE  1
#define wxDIR   2
WXDLLEXPORT wxString wxFindFirstFile(const wxChar *spec, int flags = wxFILE);
WXDLLEXPORT wxString wxFindNextFile();

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
WXDLLEXPORT wxChar* wxGetWorkingDirectory(wxChar *buf = (wxChar *) NULL, int sz = 1000);
// new and preferred version of wxGetWorkingDirectory
// NB: can't have the same name because of overloading ambiguity
WXDLLEXPORT wxString wxGetCwd();

// Set working directory
WXDLLEXPORT bool wxSetWorkingDirectory(const wxString& d);

// Make directory
WXDLLEXPORT bool wxMkdir(const wxString& dir, int perm = 0777);

// Remove directory. Flags reserved for future use.
WXDLLEXPORT bool wxRmdir(const wxString& dir, int flags = 0);

// separators in file names
#define wxFILE_SEP_EXT        wxT('.')
#define wxFILE_SEP_DSK        wxT(':')
#define wxFILE_SEP_PATH_DOS   wxT('\\')
#define wxFILE_SEP_PATH_UNIX  wxT('/')
#define wxFILE_SEP_PATH_MAC   wxT(':')

// separator in the path list (as in PATH environment variable)
// NB: these are strings and not characters on purpose!
#define wxPATH_SEP_DOS        wxT(";")
#define wxPATH_SEP_UNIX       wxT(":")

// platform independent versions
#ifdef  __UNIX__
  #define wxFILE_SEP_PATH     wxFILE_SEP_PATH_UNIX
  #define wxPATH_SEP          wxPATH_SEP_UNIX
#elif defined(__MAC__)

// TODO find out whether we can really switch back to native file names
// previously this mac was emulating unix/win filename structures
//  #define wxFILE_SEP_PATH     wxFILE_SEP_PATH_MAC
  #define wxFILE_SEP_PATH     wxFILE_SEP_PATH_DOS
  #define wxPATH_SEP          wxPATH_SEP_DOS
#else   // Windows and OS/2
  #define wxFILE_SEP_PATH     wxFILE_SEP_PATH_DOS
  #define wxPATH_SEP          wxPATH_SEP_DOS
#endif  // Unix/Windows

// this is useful for wxString::IsSameAs(): to compare two file names use
// filename1.IsSameAs(filename2, wxARE_FILENAMES_CASE_SENSITIVE)
#ifdef  __UNIX__
  #define wxARE_FILENAMES_CASE_SENSITIVE  TRUE
#else   // Windows and OS/2
  #define wxARE_FILENAMES_CASE_SENSITIVE  FALSE
#endif  // Unix/Windows

// is the char a path separator?
inline bool wxIsPathSeparator(wxChar c)
  { return c == wxFILE_SEP_PATH_DOS || c == wxFILE_SEP_PATH_UNIX; }

// does the string ends with path separator?
WXDLLEXPORT bool wxEndsWithPathSeparator(const wxChar *pszFileName);

// split the full path into path (including drive for DOS), name and extension
// (understands both '/' and '\\')
WXDLLEXPORT void wxSplitPath(const wxChar *pszFileName,
                             wxString *pstrPath,
                             wxString *pstrName,
                             wxString *pstrExt);

// find a file in a list of directories, returns false if not found
WXDLLEXPORT bool wxFindFileInPath(wxString *pStr, const wxChar *pszPath, const wxChar *pszFile);

// Get the OS directory if appropriate (such as the Windows directory).
// On non-Windows platform, probably just return the empty string.
WXDLLEXPORT wxString wxGetOSDirectory();

// Get file modification time
WXDLLEXPORT time_t wxFileModificationTime(const wxString& filename);

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------

// Path searching
class WXDLLEXPORT wxPathList : public wxStringList
{
public:
    // Adds all paths in environment variable
    void AddEnvList(const wxString& envVariable);

    void Add(const wxString& path);
    // Avoid compiler warning
    wxNode *Add(const wxChar *s) { return wxStringList::Add(s); }
    // Find the first full path for which the file exists
    wxString FindValidPath(const wxString& filename);
    // Find the first full path for which the file exists; ensure it's an
    // absolute path that gets returned.
    wxString FindAbsoluteValidPath(const wxString& filename);
    // Given full path and filename, add path to list
    void EnsureFileAccessible(const wxString& path);
    // Returns TRUE if the path is in the list
    bool Member(const wxString& path);

private:
    DECLARE_DYNAMIC_CLASS(wxPathList)
};

#endif
  // _WX_FILEFN_H_

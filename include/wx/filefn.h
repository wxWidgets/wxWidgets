/////////////////////////////////////////////////////////////////////////////
// Name:        filefn.h
// Purpose:     File- and directory-related functions
// Author:      Julian Smart
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef   _FILEFN_H_
#define   _FILEFN_H_

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "filefn.h"
#endif

#include "wx/list.h"

#ifndef __WXWINCE__
#include <time.h>
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#ifdef __WXWINCE__
    typedef long off_t;
#else

// define off_t
#if !defined(__WXMAC__) || defined(__UNIX__) || defined(__MACH__)
    #include  <sys/types.h>
#else
    typedef long off_t;
#endif

#if defined(__VISUALC__) || ( defined(__MWERKS__) && defined( __INTEL__) )
    typedef _off_t off_t;
#elif defined(__BORLANDC__) && defined(__WIN16__)
    typedef long off_t;
#elif defined(__SYMANTEC__)
    typedef long off_t;
#elif defined(__MWERKS__) && !defined(__INTEL__) && !defined(__MACH__)
    typedef long off_t;
#endif

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

// ----------------------------------------------------------------------------
// declare our versions of low level file functions: some compilers prepend
// underscores to the usual names, some also have Unicode versions of them
// ----------------------------------------------------------------------------

// Microsoft compiler loves underscores, feed them to it
#if defined( __VISUALC__ ) \
    || ( defined(__MINGW32__) && !defined(__WINE__) && wxCHECK_W32API_VERSION( 0, 5 ) ) \
    || ( defined(__MWERKS__) && defined(__WXMSW__) )
    // functions
#ifdef __BORLANDC__
    #define   _tell        tell
#endif
    #define   wxClose      _close
    #define   wxRead       _read
    #define   wxWrite      _write
    #define   wxLseek      _lseek
    #define   wxFsync      _commit
    #define   wxEof        _eof

    #define   wxTell       _tell

    #if wxUSE_UNICODE
        #if wxUSE_UNICODE_MSLU
            #define   wxOpen       wxMSLU__wopen
 
            #define   wxAccess     wxMSLU__waccess
            #define   wxMkDir      wxMSLU__wmkdir
            #define   wxRmDir      wxMSLU__wrmdir
            #define   wxStat       wxMSLU__wstat
        #else
            #define   wxOpen       _wopen
            #define   wxAccess     _waccess
            #define   wxMkDir      _wmkdir
            #define   wxRmDir      _wrmdir
            #define   wxStat       _wstat
        #endif
    #else // !wxUSE_UNICODE
#ifdef __BORLANDC__
        #define   wxOpen       open
#else
        #define   wxOpen       _open
#endif
        #define   wxAccess     _access
        #define   wxMkDir      _mkdir
        #define   wxRmDir      _rmdir
        #define   wxStat       _stat
    #endif

    // types
    #define   wxStructStat struct _stat

    // constants (unless already defined by the user code)
    #if !defined(O_RDONLY) && !defined(__BORLANDC__)
        #define   O_RDONLY    _O_RDONLY
        #define   O_WRONLY    _O_WRONLY
        #define   O_RDWR      _O_RDWR
        #define   O_EXCL      _O_EXCL
        #define   O_CREAT     _O_CREAT
        #define   O_BINARY    _O_BINARY
    #endif

    #ifndef __BORLANDC__
        #define   S_IFMT      _S_IFMT
        #define   S_IFDIR     _S_IFDIR
        #define   S_IFREG     _S_IFREG
    #endif // O_RDONLY
#else
    // functions
    #define   wxClose      close
    #define   wxRead       read
    #define   wxWrite      write
    #define   wxLseek      lseek
    #define   wxFsync      commit
    #define   wxEof        eof

    #define   wxMkDir      mkdir
    #define   wxRmDir      rmdir

    #define   wxTell(fd)   lseek(fd, 0, SEEK_CUR)

    #define   wxStructStat struct stat
    
#if wxUSE_UNICODE
#   define wxNEED_WX_UNISTD_H
#if defined(__MWERKS__) && defined(macintosh)
	#include <sys/stat.h>
#endif
WXDLLEXPORT_BASE int wxStat( const wxChar *file_name, wxStructStat *buf );
WXDLLEXPORT_BASE int wxAccess( const wxChar *pathname, int mode );
WXDLLEXPORT_BASE int wxOpen( const wxChar *pathname, int flags, mode_t mode );
#else
    #define   wxOpen       open
    #define   wxStat       stat
    #define   wxAccess     access
#endif

#endif  // VC++

// ----------------------------------------------------------------------------
// functions
// ----------------------------------------------------------------------------
WXDLLEXPORT_BASE bool wxFileExists(const wxString& filename);

// does the path exist? (may have or not '/' or '\\' at the end)
WXDLLEXPORT_BASE bool wxPathExists(const wxChar *pszPathName);

WXDLLEXPORT_BASE bool wxIsAbsolutePath(const wxString& filename);

// Get filename
WXDLLEXPORT_BASE wxChar* wxFileNameFromPath(wxChar *path);
WXDLLEXPORT_BASE wxString wxFileNameFromPath(const wxString& path);

// Get directory
WXDLLEXPORT_BASE wxString wxPathOnly(const wxString& path);

// wxString version
WXDLLEXPORT_BASE wxString wxRealPath(const wxString& path);

WXDLLEXPORT_BASE void wxDos2UnixFilename(wxChar *s);

WXDLLEXPORT_BASE void wxUnix2DosFilename(wxChar *s);

// Strip the extension, in situ
WXDLLEXPORT_BASE void wxStripExtension(wxChar *buffer);
WXDLLEXPORT_BASE void wxStripExtension(wxString& buffer);

// Get a temporary filename
WXDLLEXPORT_BASE wxChar* wxGetTempFileName(const wxString& prefix, wxChar *buf = (wxChar *) NULL);
WXDLLEXPORT_BASE bool wxGetTempFileName(const wxString& prefix, wxString& buf);

// Expand file name (~/ and ${OPENWINHOME}/ stuff)
WXDLLEXPORT_BASE wxChar* wxExpandPath(wxChar *dest, const wxChar *path);
WXDLLEXPORT_BASE bool wxExpandPath(wxString& dest, const wxChar *path);

// Contract w.r.t environment (</usr/openwin/lib, OPENWHOME> -> ${OPENWINHOME}/lib)
// and make (if under the home tree) relative to home
// [caller must copy-- volatile]
WXDLLEXPORT_BASE wxChar* wxContractPath(const wxString& filename,
                                   const wxString& envname = wxEmptyString,
                                   const wxString& user = wxEmptyString);

// Destructive removal of /./ and /../ stuff
WXDLLEXPORT_BASE wxChar* wxRealPath(wxChar *path);

// Allocate a copy of the full absolute path
WXDLLEXPORT_BASE wxChar* wxCopyAbsolutePath(const wxString& path);

// Get first file name matching given wild card.
// Flags are reserved for future use.
#define wxFILE  1
#define wxDIR   2
WXDLLEXPORT_BASE wxString wxFindFirstFile(const wxChar *spec, int flags = wxFILE);
WXDLLEXPORT_BASE wxString wxFindNextFile();

// Does the pattern contain wildcards?
WXDLLEXPORT_BASE bool wxIsWild(const wxString& pattern);

// Does the pattern match the text (usually a filename)?
// If dot_special is TRUE, doesn't match * against . (eliminating
// `hidden' dot files)
WXDLLEXPORT_BASE bool wxMatchWild(const wxString& pattern,  const wxString& text, bool dot_special = TRUE);

// Concatenate two files to form third
WXDLLEXPORT_BASE bool wxConcatFiles(const wxString& file1, const wxString& file2, const wxString& file3);

// Copy file1 to file2
WXDLLEXPORT_BASE bool wxCopyFile(const wxString& file1, const wxString& file2,
                            bool overwrite = TRUE);

// Remove file
WXDLLEXPORT_BASE bool wxRemoveFile(const wxString& file);

// Rename file
WXDLLEXPORT_BASE bool wxRenameFile(const wxString& file1, const wxString& file2);

// Get current working directory.
// If buf is NULL, allocates space using new, else
// copies into buf.
// IMPORTANT NOTE getcwd is know not to work under some releases
// of Win32s 1.3, according to MS release notes!
WXDLLEXPORT_BASE wxChar* wxGetWorkingDirectory(wxChar *buf = (wxChar *) NULL, int sz = 1000);
// new and preferred version of wxGetWorkingDirectory
// NB: can't have the same name because of overloading ambiguity
WXDLLEXPORT_BASE wxString wxGetCwd();

// Set working directory
WXDLLEXPORT_BASE bool wxSetWorkingDirectory(const wxString& d);

// Make directory
WXDLLEXPORT_BASE bool wxMkdir(const wxString& dir, int perm = 0777);

// Remove directory. Flags reserved for future use.
WXDLLEXPORT_BASE bool wxRmdir(const wxString& dir, int flags = 0);

// compatibility defines, don't use in new code
#define wxDirExists wxPathExists

#if WXWIN_COMPATIBILITY_2
    #define FileExists wxFileExists
    #define DirExists wxDirExists
    #define IsAbsolutePath wxIsAbsolutePath
    #define FileNameFromPath wxFileNameFromPath
    #define PathOnly wxPathOnly
    #define Dos2UnixFilename wxDos2UnixFilename
    #define Unix2DosFilename wxUnix2DosFilename
#endif

// ----------------------------------------------------------------------------
// separators in file names
// ----------------------------------------------------------------------------

// between file name and extension
#define wxFILE_SEP_EXT        wxT('.')

// between drive/volume name and the path
#define wxFILE_SEP_DSK        wxT(':')

// between the path components
#define wxFILE_SEP_PATH_DOS   wxT('\\')
#define wxFILE_SEP_PATH_UNIX  wxT('/')
#define wxFILE_SEP_PATH_MAC   wxT(':')
#define wxFILE_SEP_PATH_VMS   wxT('.') // VMS also uses '[' and ']'

// separator in the path list (as in PATH environment variable)
// there is no PATH variable in Classic Mac OS so just use the
// semicolon (it must be different from the file name separator)
// NB: these are strings and not characters on purpose!
#define wxPATH_SEP_DOS        wxT(";")
#define wxPATH_SEP_UNIX       wxT(":")
#define wxPATH_SEP_MAC        wxT(";")

// platform independent versions
#if defined(__UNIX__) && !defined(__CYGWIN__)
  #define wxFILE_SEP_PATH     wxFILE_SEP_PATH_UNIX
  #define wxPATH_SEP          wxPATH_SEP_UNIX
#elif defined(__MAC__)
  #define wxFILE_SEP_PATH     wxFILE_SEP_PATH_MAC
  #define wxPATH_SEP          wxPATH_SEP_MAC
#elif defined(__CYGWIN__) // Cygwin
  #define wxFILE_SEP_PATH     wxFILE_SEP_PATH_DOS
  #define wxPATH_SEP          wxPATH_SEP_UNIX
#else   // Windows and OS/2
  #define wxFILE_SEP_PATH     wxFILE_SEP_PATH_DOS
  #define wxPATH_SEP          wxPATH_SEP_DOS
#endif  // Unix/Windows

// this is useful for wxString::IsSameAs(): to compare two file names use
// filename1.IsSameAs(filename2, wxARE_FILENAMES_CASE_SENSITIVE)
#if defined(__UNIX__) && !defined(__DARWIN__)
  #define wxARE_FILENAMES_CASE_SENSITIVE  TRUE
#else   // Windows, Mac OS and OS/2
  #define wxARE_FILENAMES_CASE_SENSITIVE  FALSE
#endif  // Unix/Windows

// is the char a path separator?
inline bool wxIsPathSeparator(wxChar c)
{
    // under DOS/Windows we should understand both Unix and DOS file separators
#if defined(__UNIX__) || defined(__MAC__)
    return c == wxFILE_SEP_PATH;
#else
    return c == wxFILE_SEP_PATH_DOS || c == wxFILE_SEP_PATH_UNIX;
#endif
}

// does the string ends with path separator?
WXDLLEXPORT_BASE bool wxEndsWithPathSeparator(const wxChar *pszFileName);

// split the full path into path (including drive for DOS), name and extension
// (understands both '/' and '\\')
WXDLLEXPORT_BASE void wxSplitPath(const wxChar *pszFileName,
                             wxString *pstrPath,
                             wxString *pstrName,
                             wxString *pstrExt);

// find a file in a list of directories, returns false if not found
WXDLLEXPORT_BASE bool wxFindFileInPath(wxString *pStr, const wxChar *pszPath, const wxChar *pszFile);

// Get the OS directory if appropriate (such as the Windows directory).
// On non-Windows platform, probably just return the empty string.
WXDLLEXPORT_BASE wxString wxGetOSDirectory();

// Get file modification time
WXDLLEXPORT_BASE time_t wxFileModificationTime(const wxString& filename);

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------

// Path searching
class WXDLLEXPORT_BASE wxPathList : public wxStringList
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

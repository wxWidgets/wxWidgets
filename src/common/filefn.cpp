/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/filefn.cpp
// Purpose:     File- and directory-related functions
// Author:      Julian Smart
// Modified by:
// Created:     29/01/98
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/filefn.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/crt.h"
#endif

#include "wx/dynarray.h"
#include "wx/file.h"
#include "wx/filename.h"
#include "wx/dir.h"

#include "wx/scopedptr.h"
#include "wx/tokenzr.h"

// there are just too many of those...
#ifdef __VISUALC__
    #pragma warning(disable:4706)   // assignment within conditional expression
#endif // VC++

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined(__WXMAC__)
    #include  "wx/osx/private.h"  // includes mac headers
#endif

#ifdef __WINDOWS__
    #include "wx/msw/private.h"
    #include "wx/msw/missing.h"

    // sys/cygwin.h is needed for cygwin_conv_to_full_win32_path()
    // and for cygwin_conv_path()
    //
    // note that it must be included after <windows.h>
    #ifdef __GNUWIN32__
        #ifdef __CYGWIN__
            #include <sys/cygwin.h>
            #include <cygwin/version.h>
        #endif
    #endif // __GNUWIN32__

    // io.h is needed for _get_osfhandle()
    // Already included by filefn.h for many Windows compilers
    #if defined __CYGWIN__
        #include <io.h>
    #endif
#endif // __WINDOWS__

#if defined(__VMS__)
    #include <fab.h>
#endif

#if defined(_MSC_VER) || defined(__MINGW32__)
    wxDECL_FOR_STRICT_MINGW32(wchar_t*, _wgetcwd, (wchar_t*, int))

    #define HAVE_WGETCWD
#endif

wxDECL_FOR_STRICT_MINGW32(int, _fileno, (FILE*))

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#ifndef _MAXPATHLEN
    #define _MAXPATHLEN 1024
#endif

// ----------------------------------------------------------------------------
// private globals
// ----------------------------------------------------------------------------

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxPathList
// ----------------------------------------------------------------------------

bool wxPathList::Add(const wxString& path)
{
    // add a path separator to force wxFileName to interpret it always as a directory
    // (i.e. if we are called with '/home/user' we want to consider it a folder and
    // not, as wxFileName would consider, a filename).
    wxFileName fn(path + wxFileName::GetPathSeparator());

    // add only normalized relative/absolute paths
    // NB: we won't do wxPATH_NORM_DOTS in order to avoid problems when trying to
    //     normalize paths which starts with ".." (which can be normalized only if
    //     we use also wxPATH_NORM_ABSOLUTE - which we don't want to use).
    if (!fn.Normalize(wxPATH_NORM_TILDE|wxPATH_NORM_LONG|wxPATH_NORM_ENV_VARS))
        return false;

    wxString toadd = fn.GetPath();
    if (Index(toadd) == wxNOT_FOUND)
        wxArrayString::Add(toadd);      // do not add duplicates

    return true;
}

void wxPathList::Add(const wxArrayString &arr)
{
    for (size_t j=0; j < arr.GetCount(); j++)
        Add(arr[j]);
}

// Add paths e.g. from the PATH environment variable
void wxPathList::AddEnvList (const wxString& envVariable)
{
    // The space has been removed from the tokenizers, otherwise a
    // path such as "C:\Program Files" would be split into 2 paths:
    // "C:\Program" and "Files"; this is true for both Windows and Unix.

    static const wxChar PATH_TOKS[] =
#if defined(__WINDOWS__)
        wxT(";"); // Don't separate with colon in DOS (used for drive)
#else
        wxT(":;");
#endif

    wxString val;
    if ( wxGetEnv(envVariable, &val) )
    {
        // split into an array of string the value of the env var
        wxArrayString arr = wxStringTokenize(val, PATH_TOKS);
        WX_APPEND_ARRAY(*this, arr);
    }
}

// Given a full filename (with path), ensure that that file can
// be accessed again USING FILENAME ONLY by adding the path
// to the list if not already there.
bool wxPathList::EnsureFileAccessible (const wxString& path)
{
    return Add(wxPathOnly(path));
}

wxString wxPathList::FindValidPath (const wxString& file) const
{
    // normalize the given string as it could be a path + a filename
    // and not only a filename
    wxFileName fn(file);
    wxString strend;

    // NB: normalize without making absolute otherwise calling this function with
    //     e.g. "b/c.txt" would result in removing the directory 'b' and the for loop
    //     below would only add to the paths of this list the 'c.txt' part when doing
    //     the existence checks...
    // NB: we don't use wxPATH_NORM_DOTS here, too (see wxPathList::Add for more info)
    if (!fn.Normalize(wxPATH_NORM_TILDE|wxPATH_NORM_LONG|wxPATH_NORM_ENV_VARS))
        return wxEmptyString;

    wxASSERT_MSG(!fn.IsDir(), wxT("Cannot search for directories; only for files"));
    if (fn.IsAbsolute())
        strend = fn.GetFullName();      // search for the file name and ignore the path part
    else
        strend = fn.GetFullPath();

    for (size_t i=0; i<GetCount(); i++)
    {
        wxString strstart = Item(i);
        if (!strstart.IsEmpty() && strstart.Last() != wxFileName::GetPathSeparator())
            strstart += wxFileName::GetPathSeparator();

        if (wxFileExists(strstart + strend))
            return strstart + strend;        // Found!
    }

    return wxEmptyString;                    // Not found
}

wxString wxPathList::FindAbsoluteValidPath (const wxString& file) const
{
    wxString f = FindValidPath(file);
    if ( f.empty() || wxIsAbsolutePath(f) )
        return f;

    wxString buf = ::wxGetCwd();

    if ( !wxEndsWithPathSeparator(buf) )
    {
        buf += wxFILE_SEP_PATH;
    }
    buf += f;

    return buf;
}

// ----------------------------------------------------------------------------
// miscellaneous global functions
// ----------------------------------------------------------------------------

bool
wxFileExists (const wxString& filename)
{
    return wxFileName::FileExists(filename);
}

bool
wxIsAbsolutePath (const wxString& filename)
{
    if (!filename.empty())
    {
        // Unix like or Windows
        if (filename[0] == wxT('/'))
            return true;
#ifdef __VMS__
        if (filename.size() > 1 && (filename[0] == wxT('[') && filename[1] != wxT('.')))
            return true;
#endif
#if defined(__WINDOWS__)
        // MSDOS like
        if (filename[0] == wxT('\\') ||
            (filename.size() > 1 && (wxIsalpha (filename[0]) && filename[1] == wxT(':'))))
            return true;
#endif
    }
    return false ;
}

// Return just the filename, not the path (basename)
wxChar *wxFileNameFromPath (wxChar *path)
{
    wxString p = path;
    wxString n = wxFileNameFromPath(p);

    return path + p.length() - n.length();
}

wxString wxFileNameFromPath (const wxString& path)
{
    return wxFileName(path).GetFullName();
}

// Return just the directory, or nullptr if no directory
wxChar *
wxPathOnly (wxChar *path)
{
    if (path && *path)
    {
        static wxChar buf[_MAXPATHLEN];

        int l = wxStrlen(path);
        int i = l - 1;
        if ( i >= _MAXPATHLEN )
            return nullptr;

        // Local copy
        wxStrcpy (buf, path);

        // Search backward for a backward or forward slash
        while (i > -1)
        {
            // Unix like or Windows
            if (path[i] == wxT('/') || path[i] == wxT('\\'))
            {
                buf[i] = 0;
                return buf;
            }
#ifdef __VMS__
            if (path[i] == wxT(']'))
            {
                buf[i+1] = 0;
                return buf;
            }
#endif
            i --;
        }

#if defined(__WINDOWS__)
        // Try Drive specifier
        if (wxIsalpha (buf[0]) && buf[1] == wxT(':'))
        {
            // A:junk --> A:. (since A:.\junk Not A:\junk)
            buf[2] = wxT('.');
            buf[3] = wxT('\0');
            return buf;
        }
#endif
    }
    return nullptr;
}

// Return just the directory, or nullptr if no directory
wxString wxPathOnly (const wxString& path)
{
    if (!path.empty())
    {
        wxChar buf[_MAXPATHLEN];

        int l = path.length();
        int i = l - 1;

        if ( i >= _MAXPATHLEN )
            return wxString();

        // Local copy
        wxStrcpy(buf, path);

        // Search backward for a backward or forward slash
        while (i > -1)
        {
            // Unix like or Windows
            if (path[i] == wxT('/') || path[i] == wxT('\\'))
            {
                // Don't return an empty string
                if (i == 0)
                    i ++;
                buf[i] = 0;
                return wxString(buf);
            }
#ifdef __VMS__
            if (path[i] == wxT(']'))
            {
                buf[i+1] = 0;
                return wxString(buf);
            }
#endif
            i --;
        }

#if defined(__WINDOWS__)
        // Try Drive specifier
        if (wxIsalpha (buf[0]) && buf[1] == wxT(':'))
        {
            // A:junk --> A:. (since A:.\junk Not A:\junk)
            buf[2] = wxT('.');
            buf[3] = wxT('\0');
            return wxString(buf);
        }
#endif
    }
    return wxEmptyString;
}

// Utility for converting delimiters in DOS filenames to UNIX style
// and back again - or we get nasty problems with delimiters.
// Also, convert to lower case, since case is significant in UNIX.

#ifdef __WXOSX__

CFURLRef wxOSXCreateURLFromFileSystemPath( const wxString& path)
{
    wxCFRef<CFMutableStringRef> cfMutableString(CFStringCreateMutableCopy(nullptr, 0, wxCFStringRef(path)));
    CFStringNormalize(cfMutableString,kCFStringNormalizationFormD);
    return CFURLCreateWithFileSystemPath(kCFAllocatorDefault, cfMutableString , kCFURLPOSIXPathStyle, false);
}

#endif // __WXMAC__


// Concatenate two files to form third
bool
wxConcatFiles (const wxString& file1, const wxString& file2, const wxString& file3)
{
#if wxUSE_FILE

    wxFile in1(file1), in2(file2);
    wxTempFile out(file3);

    if ( !in1.IsOpened() || !in2.IsOpened() || !out.IsOpened() )
        return false;

    ssize_t ofs;
    unsigned char buf[1024];

    for( int i=0; i<2; i++)
    {
        wxFile *in = i==0 ? &in1 : &in2;
        do{
            if ( (ofs = in->Read(buf,WXSIZEOF(buf))) == wxInvalidOffset ) return false;
            if ( ofs > 0 )
                if ( !out.Write(buf,ofs) )
                    return false;
        } while ( ofs == (ssize_t)WXSIZEOF(buf) );
    }

    return out.Commit();

#else

    wxUnusedVar(file1);
    wxUnusedVar(file2);
    wxUnusedVar(file3);
    return false;

#endif
}

// helper of generic implementation of wxCopyFile()
#if !defined(__WIN32__) && wxUSE_FILE

static bool
wxDoCopyFile(wxFile& fileIn,
             const wxStructStat& fbuf,
             const wxString& filenameDst,
             bool overwrite)
{
    // reset the umask as we want to create the file with exactly the same
    // permissions as the original one
    wxCHANGE_UMASK(0);

    // create file2 with the same permissions than file1 and open it for
    // writing

    wxFile fileOut;
    if ( !fileOut.Create(filenameDst, overwrite, fbuf.st_mode & 0777) )
        return false;

    // copy contents of file1 to file2
    char buf[4096];
    for ( ;; )
    {
        ssize_t count = fileIn.Read(buf, WXSIZEOF(buf));
        if ( count == wxInvalidOffset )
            return false;

        // end of file?
        if ( !count )
            break;

        if ( fileOut.Write(buf, count) < (size_t)count )
            return false;
    }

    // we can expect fileIn to be closed successfully, but we should ensure
    // that fileOut was closed as some write errors (disk full) might not be
    // detected before doing this
    return fileIn.Close() && fileOut.Close();
}

#endif // generic implementation of wxCopyFile

// Copy files
bool
wxCopyFile (const wxString& file1, const wxString& file2, bool overwrite)
{
#if defined(__WIN32__)
    // CopyFile() copies file attributes and modification time too, so use it
    // instead of our code if available
    //
    // NB: 3rd parameter is bFailIfExists i.e. the inverse of overwrite
    if ( !::CopyFile(file1.t_str(), file2.t_str(), !overwrite) )
    {
        wxLogSysError(_("Failed to copy the file '%s' to '%s'"),
                      file1.c_str(), file2.c_str());

        return false;
    }
#elif wxUSE_FILE // !Win32

    wxStructStat fbuf;
    // get permissions of file1
    if ( wxStat( file1, &fbuf) != 0 )
    {
        // the file probably doesn't exist or we haven't the rights to read
        // from it anyhow
        wxLogSysError(_("Impossible to get permissions for file '%s'"),
                      file1.c_str());
        return false;
    }

    // open file1 for reading
    wxFile fileIn(file1, wxFile::read);
    if ( !fileIn.IsOpened() )
        return false;

    // remove file2, if it exists. This is needed for creating
    // file2 with the correct permissions in the next step
    if ( wxFileExists(file2)  && (!overwrite || !wxRemoveFile(file2)))
    {
        wxLogSysError(_("Impossible to overwrite the file '%s'"),
                      file2.c_str());
        return false;
    }

    if ( !wxDoCopyFile(fileIn, fbuf, file2, overwrite) )
    {
        wxLogError(_("Error copying the file '%s' to '%s'."), file1, file2);
        return false;
    }

#if defined(__WXMAC__)
    // copy the resource fork of the file too if it's present
    wxString pathRsrcOut;
    wxFile fileRsrcIn;

    {
        // suppress error messages from this block as resource forks don't have
        // to exist
        wxLogNull noLog;

        // it's not enough to check for file existence: it always does on HFS
        // but is empty for files without resources
        if ( fileRsrcIn.Open(file1 + wxT("/..namedfork/rsrc")) &&
                fileRsrcIn.Length() > 0 )
        {
            // we must be using HFS or another filesystem with resource fork
            // support, suppose that destination file system also is HFS[-like]
            pathRsrcOut = file2 + wxT("/..namedfork/rsrc");
        }
        else // check if we have resource fork in separate file (non-HFS case)
        {
            wxFileName fnRsrc(file1);
            fnRsrc.SetName(wxT("._") + fnRsrc.GetName());

            fileRsrcIn.Close();
            if ( fileRsrcIn.Open( fnRsrc.GetFullPath() ) )
            {
                fnRsrc = file2;
                fnRsrc.SetName(wxT("._") + fnRsrc.GetName());

                pathRsrcOut = fnRsrc.GetFullPath();
            }
        }
    }

    if ( !pathRsrcOut.empty() )
    {
        if ( !wxDoCopyFile(fileRsrcIn, fbuf, pathRsrcOut, overwrite) )
            return false;
    }
#endif // wxMac

    if ( chmod(file2.fn_str(), fbuf.st_mode) != 0 )
    {
        wxLogSysError(_("Impossible to set permissions for the file '%s'"),
                      file2.c_str());
        return false;
    }

#else // !Win32 && ! wxUSE_FILE

    // impossible to simulate with wxWidgets API
    wxUnusedVar(file1);
    wxUnusedVar(file2);
    wxUnusedVar(overwrite);
    return false;

#endif // __WINDOWS__ && __WIN32__

    return true;
}

bool
wxRenameFile(const wxString& file1, const wxString& file2, bool overwrite)
{
    if ( !overwrite && wxFileExists(file2) )
    {
        wxLogSysError
        (
            _("Failed to rename the file '%s' to '%s' because the destination file already exists."),
            file1.c_str(), file2.c_str()
        );

        return false;
    }

    // Normal system call
  if ( wxRename (file1, file2) == 0 )
    return true;

  // Try to copy
  if (wxCopyFile(file1, file2, overwrite)) {
    wxRemoveFile(file1);
    return true;
  }
  // Give up
  wxLogSysError(_("File '%s' couldn't be renamed '%s'"), file1, file2);
  return false;
}

bool wxRemoveFile(const wxString& file)
{
#if defined(__VISUALC__) \
 || defined(__GNUWIN32__)
    int res = wxRemove(file);
#elif defined(__WXMAC__)
    int res = unlink(file.fn_str());
#else
    int res = unlink(file.fn_str());
#endif
    if ( res )
    {
        wxLogSysError(_("File '%s' couldn't be removed"), file);
    }
    return res == 0;
}

bool wxMkdir(const wxString& dir, int perm)
{
#if defined(__WXMAC__) && !defined(__UNIX__)
    if ( mkdir(dir.fn_str(), 0) != 0 )

    // assume mkdir() has 2 args on all platforms
    // for the GNU compiler
#elif (!defined(__WINDOWS__)) || \
      (defined(__GNUWIN32__) && !defined(__MINGW32__)) ||                \
      defined(__WINE__)
    const wxChar *dirname = dir.c_str();
  #if defined(MSVCRT)
    wxUnusedVar(perm);
    if ( mkdir(wxFNCONV(dirname)) != 0 )
  #else
    if ( mkdir(wxFNCONV(dirname), perm) != 0 )
  #endif
#else  // MSW and VC++
    wxUnusedVar(perm);
    if ( wxMkDir(dir.fn_str()) != 0 )
#endif // !MSW/MSW
    {
        wxLogSysError(_("Directory '%s' couldn't be created"), dir);
        return false;
    }

    return true;
}

bool wxRmdir(const wxString& dir, int WXUNUSED(flags))
{
#if defined(__VMS__)
    return false; //to be changed since rmdir exists in VMS7.x
#else
    if ( wxRmDir(dir) != 0 )
    {
        wxLogSysError(_("Directory '%s' couldn't be deleted"), dir);
        return false;
    }

    return true;
#endif
}

// does the path exists? (may have or not '/' or '\\' at the end)
bool wxDirExists(const wxString& pathName)
{
    return wxFileName::DirExists(pathName);
}


// Get first file name matching given wild card.

static wxScopedPtr<wxDir> gs_dir;
static wxString gs_dirPath;

wxString wxFindFirstFile(const wxString& spec, int flags)
{
    wxFileName::SplitPath(spec, &gs_dirPath, nullptr, nullptr);
    if ( gs_dirPath.empty() )
        gs_dirPath = wxT(".");
    if ( !wxEndsWithPathSeparator(gs_dirPath ) )
        gs_dirPath << wxFILE_SEP_PATH;

    gs_dir.reset(new wxDir(gs_dirPath));

    if ( !gs_dir->IsOpened() )
    {
        wxLogSysError(_("Cannot enumerate files '%s'"), spec);
        return wxEmptyString;
    }

    int dirFlags;
    switch (flags)
    {
        case wxDIR:  dirFlags = wxDIR_DIRS; break;
        case wxFILE: dirFlags = wxDIR_FILES; break;
        default:     dirFlags = wxDIR_DIRS | wxDIR_FILES; break;
    }

    wxString result;
    gs_dir->GetFirst(&result, wxFileNameFromPath(spec), dirFlags);
    if ( result.empty() )
        return result;

    return gs_dirPath + result;
}

wxString wxFindNextFile()
{
    wxCHECK_MSG( gs_dir, "", "You must call wxFindFirstFile before!" );

    wxString result;
    if ( !gs_dir->GetNext(&result) || result.empty() )
        return result;

    return gs_dirPath + result;
}


// Get current working directory.
// If buf is null, allocates space using new, else copies into buf.
// wxGetWorkingDirectory() is obsolete, use wxGetCwd()
// wxDoGetCwd() is their common core to be moved
// to wxGetCwd() once wxGetWorkingDirectory() will be removed.
// Do not expose wxDoGetCwd in headers!

wxChar *wxDoGetCwd(wxChar *buf, int sz)
{
    if ( !buf )
    {
        buf = new wxChar[sz + 1];
    }

    bool ok = false;

    // for the compilers which have Unicode version of _getcwd(), call it
    // directly, for the others call the ANSI version and do the translation
#if !wxUSE_UNICODE
    #define cbuf buf
#else // wxUSE_UNICODE
    bool needsANSI = true;

    #if !defined(HAVE_WGETCWD)
        char cbuf[_MAXPATHLEN];
    #endif

    #ifdef HAVE_WGETCWD
            char *cbuf = nullptr; // never really used because needsANSI will always be false
            {
                ok = _wgetcwd(buf, sz) != nullptr;
                needsANSI = false;
            }
    #endif

    if ( needsANSI )
#endif // wxUSE_UNICODE
    {
    #if defined(_MSC_VER) || defined(__MINGW32__)
        ok = _getcwd(cbuf, sz) != nullptr;
    #else // !Win32/VC++ !Mac
        ok = getcwd(cbuf, sz) != nullptr;
    #endif // platform

    #if wxUSE_UNICODE
        // finally convert the result to Unicode if needed
        wxConvFile.MB2WC(buf, cbuf, sz);
    #endif // wxUSE_UNICODE
    }

    if ( !ok )
    {
        wxLogSysError(_("Failed to get the working directory"));

        // VZ: the old code used to return "." on error which didn't make any
        //     sense at all to me - empty string is a better error indicator
        //     (nullptr might be even better but I'm afraid this could lead to
        //     problems with the old code assuming the return is never null)
        buf[0] = wxT('\0');
    }
    else // ok, but we might need to massage the path into the right format
    {
// MBN: we hope that in the case the user is compiling a GTK app,
//      he needs Unix as opposed to Win32 pathnames
#if defined( __CYGWIN__ ) && defined( __WINDOWS__ )
        // another example of DOS/Unix mix (Cygwin)
        wxString pathUnix = buf;
#if wxUSE_UNICODE
    #if CYGWIN_VERSION_DLL_MAJOR >= 1007
        cygwin_conv_path(CCP_POSIX_TO_WIN_W, pathUnix.mb_str(wxConvFile), buf, sz);
    #else
        char bufA[_MAXPATHLEN];
        cygwin_conv_to_full_win32_path(pathUnix.mb_str(wxConvFile), bufA);
        wxConvFile.MB2WC(buf, bufA, sz);
    #endif
#else
    #if CYGWIN_VERSION_DLL_MAJOR >= 1007
        cygwin_conv_path(CCP_POSIX_TO_WIN_A, pathUnix, buf, sz);
    #else
        cygwin_conv_to_full_win32_path(pathUnix, buf);
    #endif
#endif // wxUSE_UNICODE
#endif // __CYGWIN__
    }

    return buf;

#if !wxUSE_UNICODE
    #undef cbuf
#endif

}

wxString wxGetCwd()
{
    wxString str;
    wxDoGetCwd(wxStringBuffer(str, _MAXPATHLEN), _MAXPATHLEN);
    return str;
}

bool wxSetWorkingDirectory(const wxString& d)
{
    bool success = false;
#if defined(__UNIX__) || defined(__WXMAC__)
    success = (chdir(d.fn_str()) == 0);
#elif defined(__WINDOWS__)
    success = (SetCurrentDirectory(d.t_str()) != 0);
#endif
    if ( !success )
    {
       wxLogSysError(_("Could not set current working directory"));
    }
    return success;
}

// Get the OS directory if appropriate (such as the Windows directory).
// On non-Windows platform, probably just return the empty string.
wxString wxGetOSDirectory()
{
#if defined(__WINDOWS__)
    wxChar buf[MAX_PATH];
    if ( !GetWindowsDirectory(buf, MAX_PATH) )
    {
        wxLogLastError(wxS("GetWindowsDirectory"));
    }

    return wxString(buf);
#else
    return wxEmptyString;
#endif
}

bool wxEndsWithPathSeparator(const wxString& filename)
{
    return !filename.empty() && wxIsPathSeparator(filename.Last());
}

// find a file in a list of directories, returns false if not found
bool wxFindFileInPath(wxString *pStr, const wxString& szPath, const wxString& szFile)
{
    // we assume that it's not empty
    wxCHECK_MSG( !szFile.empty(), false,
                 wxT("empty file name in wxFindFileInPath"));

    // skip path separator in the beginning of the file name if present
    wxString szFile2;
    if ( wxIsPathSeparator(szFile[0u]) )
        szFile2 = szFile.Mid(1);
    else
        szFile2 = szFile;

    wxStringTokenizer tkn(szPath, wxPATH_SEP);

    while ( tkn.HasMoreTokens() )
    {
        wxString strFile = tkn.GetNextToken();
        if ( !wxEndsWithPathSeparator(strFile) )
            strFile += wxFILE_SEP_PATH;
        strFile += szFile2;

        if ( wxFileExists(strFile) )
        {
            *pStr = strFile;
            return true;
        }
    }

    return false;
}

#if wxUSE_DATETIME

time_t WXDLLIMPEXP_BASE wxFileModificationTime(const wxString& filename)
{
    wxDateTime mtime;
    if ( !wxFileName(filename).GetTimes(nullptr, &mtime, nullptr) )
        return (time_t)-1;

    return mtime.GetTicks();
}

#endif // wxUSE_DATETIME


// Parses the filterStr, returning the number of filters.
// Returns 0 if none or if there's a problem.
// filterStr is in the form: "All files (*.*)|*.*|JPEG Files (*.jpeg)|*.jpeg"

int WXDLLIMPEXP_BASE wxParseCommonDialogsFilter(const wxString& filterStr,
                                           wxArrayString& descriptions,
                                           wxArrayString& filters)
{
    descriptions.Clear();
    filters.Clear();

    wxString str(filterStr);

    wxString description, filter;
    int pos = 0;
    while( pos != wxNOT_FOUND )
    {
        pos = str.Find(wxT('|'));
        if ( pos == wxNOT_FOUND )
        {
            // if there are no '|'s at all in the string just take the entire
            // string as filter and make description empty for later autocompletion
            if ( filters.IsEmpty() )
            {
                descriptions.Add(wxEmptyString);
                filters.Add(filterStr);
            }
            else
            {
                wxFAIL_MSG( wxT("missing '|' in the wildcard string!") );
            }

            break;
        }

        description = str.Left(pos);
        str = str.Mid(pos + 1);
        pos = str.Find(wxT('|'));
        if ( pos == wxNOT_FOUND )
        {
            filter = str;
        }
        else
        {
            filter = str.Left(pos);
            str = str.Mid(pos + 1);
        }

        descriptions.Add(description);
        filters.Add(filter);
    }

    // autocompletion
    for( size_t j = 0 ; j < descriptions.GetCount() ; j++ )
    {
        if ( descriptions[j].empty() && !filters[j].empty() )
        {
            descriptions[j].Printf(_("Files (%s)"), filters[j].c_str());
        }
    }

    return filters.GetCount();
}

#if defined(__WINDOWS__) && !defined(__UNIX__)
static bool wxCheckWin32Permission(const wxString& path, DWORD access)
{
    // quoting the MSDN: "To obtain a handle to a directory, call the
    // CreateFile function with the FILE_FLAG_BACKUP_SEMANTICS flag"
    const DWORD dwAttr = ::GetFileAttributes(path.t_str());
    if ( dwAttr == INVALID_FILE_ATTRIBUTES )
    {
        // file probably doesn't exist at all
        return false;
    }

    const HANDLE h = ::CreateFile
                 (
                    path.t_str(),
                    access,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    nullptr,
                    OPEN_EXISTING,
                    dwAttr & FILE_ATTRIBUTE_DIRECTORY
                        ? FILE_FLAG_BACKUP_SEMANTICS
                        : 0,
                    nullptr
                 );
    if ( h != INVALID_HANDLE_VALUE )
        CloseHandle(h);

    return h != INVALID_HANDLE_VALUE;
}
#endif // __WINDOWS__

bool wxIsWritable(const wxString &path)
{
#if defined( __UNIX__ )
    // access() will take in count also symbolic links
    return wxAccess(path.c_str(), W_OK) == 0;
#elif defined( __WINDOWS__ )
    return wxCheckWin32Permission(path, GENERIC_WRITE);
#else
    wxUnusedVar(path);
    // TODO
    return false;
#endif
}

bool wxIsReadable(const wxString &path)
{
#if defined( __UNIX__ )
    // access() will take in count also symbolic links
    return wxAccess(path.c_str(), R_OK) == 0;
#elif defined( __WINDOWS__ )
    return wxCheckWin32Permission(path, GENERIC_READ);
#else
    wxUnusedVar(path);
    // TODO
    return false;
#endif
}

bool wxIsExecutable(const wxString &path)
{
#if defined( __UNIX__ )
    // access() will take in count also symbolic links
    return wxAccess(path.c_str(), X_OK) == 0;
#elif defined( __WINDOWS__ )
   return wxCheckWin32Permission(path, GENERIC_EXECUTE);
#else
    wxUnusedVar(path);
    // TODO
    return false;
#endif
}

// Return the type of an open file
//
// Some file types on some platforms seem seekable but in fact are not.
// The main use of this function is to allow such cases to be detected
// (IsSeekable() is implemented as wxGetFileKind() == wxFILE_KIND_DISK).
//
// This is important for the archive streams, which benefit greatly from
// being able to seek on a stream, but which will produce corrupt archives
// if they unknowingly seek on a non-seekable stream.
//
// wxFILE_KIND_DISK is a good catch all return value, since other values
// disable features of the archive streams. Some other value must be returned
// for a file type that appears seekable but isn't.
//
// Known examples:
//   *  Pipes on Windows
//   *  Files on VMS with a record format other than StreamLF
//
wxFileKind wxGetFileKind(int fd)
{
#if defined __WINDOWS__ && defined wxGetOSFHandle
    switch (::GetFileType(wxGetOSFHandle(fd)) & ~FILE_TYPE_REMOTE)
    {
        case FILE_TYPE_CHAR:
            return wxFILE_KIND_TERMINAL;
        case FILE_TYPE_DISK:
            return wxFILE_KIND_DISK;
        case FILE_TYPE_PIPE:
            return wxFILE_KIND_PIPE;
    }

    return wxFILE_KIND_UNKNOWN;

#elif defined(__UNIX__)
    if (isatty(fd))
        return wxFILE_KIND_TERMINAL;

    struct stat st;
    fstat(fd, &st);

    if (S_ISFIFO(st.st_mode))
        return wxFILE_KIND_PIPE;
    if (!S_ISREG(st.st_mode))
        return wxFILE_KIND_UNKNOWN;

    #if defined(__VMS__)
        if (st.st_fab_rfm != FAB$C_STMLF)
            return wxFILE_KIND_UNKNOWN;
    #endif

    return wxFILE_KIND_DISK;

#else
    #define wxFILEKIND_STUB
    (void)fd;
    return wxFILE_KIND_DISK;
#endif
}

wxFileKind wxGetFileKind(FILE *fp)
{
#if defined(wxFILEKIND_STUB)
    (void)fp;
    return wxFILE_KIND_DISK;
#elif defined(__WINDOWS__) && !defined(__CYGWIN__) && !defined(__WINE__)
    return fp ? wxGetFileKind(_fileno(fp)) : wxFILE_KIND_UNKNOWN;
#else
    return fp ? wxGetFileKind(fileno(fp)) : wxFILE_KIND_UNKNOWN;
#endif
}


//------------------------------------------------------------------------
// wild character routines
//------------------------------------------------------------------------

bool wxIsWild( const wxString& pattern )
{
    for ( wxString::const_iterator p = pattern.begin(); p != pattern.end(); ++p )
    {
        switch ( (*p).GetValue() )
        {
            case wxT('?'):
            case wxT('*'):
            case wxT('['):
            case wxT('{'):
                return true;

            case wxT('\\'):
                if ( ++p == pattern.end() )
                    return false;
        }
    }
    return false;
}

/*
* Written By Douglas A. Lewis <dalewis@cs.Buffalo.EDU>
*
* The match procedure is public domain code (from ircII's reg.c)
* but modified to suit our tastes (RN: No "%" syntax I guess)
*/

bool wxMatchWild( const wxString& pat, const wxString& text, bool dot_special )
{
    if (text.empty())
    {
        /* Match if both are empty. */
        return pat.empty();
    }

    const wxChar *m = pat.c_str(),
    *n = text.c_str(),
    *ma = nullptr,
    *na = nullptr;
    int just = 0,
    acount = 0,
    count = 0;

    if (dot_special && (*n == wxT('.')))
    {
        /* Never match so that hidden Unix files
         * are never found. */
        return false;
    }

    for (;;)
    {
        if (*m == wxT('*'))
        {
            ma = ++m;
            na = n;
            just = 1;
            acount = count;
        }
        else if (*m == wxT('?'))
        {
            m++;
            if (!*n++)
                return false;
        }
        else
        {
            if (*m == wxT('\\'))
            {
                m++;
                /* Quoting "nothing" is a bad thing */
                if (!*m)
                    return false;
            }
            if (!*m)
            {
                /*
                * If we are out of both strings or we just
                * saw a wildcard, then we can say we have a
                * match
                */
                if (!*n)
                    return true;
                if (just)
                    return true;
                just = 0;
                goto not_matched;
            }
            /*
            * We could check for *n == nullptr at this point, but
            * since it's more common to have a character there,
            * check to see if they match first (m and n) and
            * then if they don't match, THEN we can check for
            * the NUL of n
            */
            just = 0;
            if (*m == *n)
            {
                m++;
                count++;
                n++;
            }
            else
            {

                not_matched:

                /*
                * If there are no more characters in the
                * string, but we still need to find another
                * character (*m != nullptr), then it will be
                * impossible to match it
                */
                if (!*n)
                    return false;

                if (ma)
                {
                    m = ma;
                    n = ++na;
                    count = acount;
                }
                else
                    return false;
            }
        }
    }
}

#ifdef __VISUALC__
    #pragma warning(default:4706)   // assignment within conditional expression
#endif // VC++
